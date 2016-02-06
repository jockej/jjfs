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
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "jjfs_conf.h"

#define JJFS_DEFAULT_MODE 0544

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

/* int jjfs_read(const char *path, char *buf, size_t size, off_t offs, */
              /* struct fuse_file_info *fi); */

/* int jjfs_read_buf(const char *path, struct fuse_bufvec **bufp, */
                  /* size_t size, off_t offs, struct fuse_file_info *fi); */

int jjfs_open(const char *path, struct fuse_file_info *fi) {

  fprintf(stderr, "Got request to open path %s\n", path);
  /* jjfs_conn(); */
  
  return 0;
}

/* int jjfs_release(const char *path, struct fuse_file_info *fi); */

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
  if (amode | W_OK) return -1;
  else return 0;
}
