/*
 * Copyright (C) 2016  Joakim Jalap
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "jjfs_fuse.h"
#include "jjfs_cache.h"
#include "jjfs_sftp.h"
#include "jjfs_misc.h"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "jjfs_conf.h"
#include <dirent.h>

#define JJFS_DEFAULT_MODE 0544
#define JJFS_MAX_SIM_TRANSFERS 64

typedef struct {
  int cache_file;
  pthread_t fetcher;
  FILE *stream;
} jjfs_transfer;
   
jjfs_transfer transfers[JJFS_MAX_SIM_TRANSFERS];

#define JJFS_NEW_TRANS(p) for (p = transfers; p->cache_file; p++)

#define JJFS_FREE_TRANS(p)                      \
  do {                                          \
    p->cache_file = 0;                          \
    p->fetcher = 0;                             \
    p->stream = NULL;                           \
  } while(0)

#define JJFS_GET_TRANS(p, fd) \
  for (p = transfers; p != (transfers + JJFS_MAX_SIM_TRANSFERS) &&      \
         p->cache_file != fd; p++)

 
int jjfs_getattr(const char *path, struct stat *st) {
  fprintf(stderr, "Got request to stat path %s\n", path);

  jjfs_cache_entry *ent = jjfs_cache_lookup_path(path);

  if (!ent) {
    fprintf(stderr, "No such path in cache\n");
    return -ENOENT;
  }

  if (JJFS_IS_DIR(ent)) {
    st->st_nlink = 2;
    st->st_size = ent->dir->size;
  } else {
    st->st_size = ent->file->size;
    st->st_nlink = 1;
  }
  st->st_uid = getuid();
  st->st_gid = getgid();
  mode_t mode = JJFS_DEFAULT_MODE;
  if (JJFS_IS_DIR(ent)) mode |= S_IFDIR;
  else mode |= S_IFREG;
  st->st_mode = mode;
  return 0;
}

/* int jjfs_fgetattr(const char *path, struct stat *st, struct fuse_file_info *fi); */

int jjfs_read(const char *path, char *buf, size_t size, off_t offs,
              struct fuse_file_info *fi) {
  JJFS_DEBUG_PRINT(3, "Request to read %lu bytes from %s at offset %lu\n",
                   size, path, offs);
  int fd = fi->fh;
  jjfs_transfer *t;
  JJFS_GET_TRANS(t, fd);

  FILE *stream;
  if (t->stream) {
    stream = t->stream;
  } else {
    stream = fdopen(fd, "r");
    t->stream = stream;
  }
  unsigned tries = 0;

  struct stat st;
  if (fstat(fd, &st) == -1) {
    JJFS_DIE("Failed to stat staging file\n");
  }

  /* lseek(fd, offs, SEEK_SET); */
  fseek(stream, offs, SEEK_SET);

  // FIXME: we should wait a while here
  if (st.st_size < offs) {
    tries++;
    return 0;
  } else if ((offs + size) > st.st_size) {
    return fread(buf, 1, st.st_size - offs, stream);
  } else {
    return fread(buf, 1, size, stream);
  }
}

/* int jjfs_read_buf(const char *path, struct fuse_bufvec *bufp, */
/*                   size_t size, off_t offs, struct fuse_file_info *fi) { */
/*   bufp = (struct fuse_bufvec*)malloc(size); */
/*   return jjfs_read(path, bufp, size, offs, fi); */
/* } */

int jjfs_open(const char *path, struct fuse_file_info *fi) {

  JJFS_DEBUG_PRINT(1, "Got request to open path %s\n", path);
  char cache_file[JJFS_SCRATCH_SIZE];

  jjfs_cache_entry *e = jjfs_cache_lookup_path(path);

  jjfs_transfer *new_trans;
  JJFS_NEW_TRANS(new_trans);

  /* FIXME: fix the strncat and stuff */
#ifdef __OpenBSD__
  strlcpy(cache_file, jjfs_get_staging_dir(), JJFS_SCRATCH_SIZE);
  strlcat(cache_file, e->file->name, JJFS_SCRATCH_SIZE);
#else
  strncpy(cache_file, jjfs_get_staging_dir(), 512); // whatever...
  strncat(cache_file, e->file->name, 1024);
#endif
  
  int cache_fd = open(cache_file, O_RDWR | O_TRUNC | O_CREAT, 0644);
  if (cache_fd == -1) {
    fprintf(stderr, "Failed to open cache file %s\n", cache_file);
    return -1;
  }

  new_trans->cache_file = cache_fd;

  pthread_t t;
  if ((t = jjfs_start_async_read(path, cache_fd)) == 0) {
    fprintf(stderr, "Failed to open %s for async read.\n", path);
    return -1;
  }

  new_trans->fetcher = t;

  int prefetch = jjfs_get_prefetch_bytes();
  if (prefetch > e->file->size) prefetch = e->file->size;
  struct stat st;
  do {
    sleep(1);
    fstat(cache_fd, &st);
  } while (st.st_size < prefetch);


  fi->fh = cache_fd;
  return 0;
}

int jjfs_release(const char *path, struct fuse_file_info *fi) {
  jjfs_cache_entry *file = jjfs_cache_lookup_path(path);
  int fd = fi->fh;
  jjfs_transfer *t;
  JJFS_GET_TRANS(t, fd);

  close(fd);

  DIR *d = opendir(jjfs_get_staging_dir());
  unlinkat(dirfd(d), file->file->name, 0);
  JJFS_FREE_TRANS(t);
  return 0;
}

int jjfs_opendir(const char *path, struct fuse_file_info *fi) {
  return 0;
}

int jjfs_readdir(const char *path, void * buf, fuse_fill_dir_t filler, off_t offs,
                 struct fuse_file_info *fi) {
  jjfs_cache_entry *ent = jjfs_cache_lookup_path(path);
  
  if (!JJFS_IS_DIR(ent)) return -1;
  jjfs_cache_file *f = ent->dir->files;
  for (; f; f = f->next) filler(buf, f->name, NULL, 0);

  jjfs_cache_dir *d = ent->dir->subdirs;
  for (; d; d = d->next) filler(buf, d->name, NULL, 0);

  return 0;
}

int jjfs_releasedir(const char *path, struct fuse_file_info *fi) {
  return 0;
}

void *jjfs_init (struct fuse_conn_info *conn) {
  jjfs_cache_init();
  return NULL;
}

void jjfs_destroy(void *v) {
  jjfs_cache_free();
  jjfs_conf_free();
}

int jjfs_access(const char *path, int amode) {
  return 0;
}
