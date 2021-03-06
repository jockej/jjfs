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

/**
 * This file implements the caching functionality.
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include "jjfs_cache.h"
#include "jjfs_conf.h"
#include "jjfs_sftp.h"
#include "jjfs_misc.h"
#include "asn1/JjfsDir.h"
#include "asn1/JjfsFile.h"

/**
 * Memory for the result of `jjfs_cache_lookup_path'.
 */
jjfs_cache_entry ret;

/**
 * Top of cache.
 */
jjfs_cache_dir top;

#ifdef DEBUG
#define LEVEL_PADDING(lvl)                              \
  do {                                                  \
    unsigned i;                                         \
    for (i = 0; i < (lvl); i++) fprintf(stderr, "  ");  \
  } while(0)

/**
 * Helper function to be called by `jjfs_cache_debug_print'.
 */
static void jjfs_cache_debug_print_dir(unsigned level, jjfs_cache_dir *dir) {
  LEVEL_PADDING(level);
  fprintf(stderr, "%s, %lu\n", dir->name, dir->size);
  jjfs_cache_dir *d = dir->subdirs;
  while (d) {
    jjfs_cache_debug_print_dir(level + 1, d);
    d = d->next;
  }
  jjfs_cache_file *f = dir->files;
  while (f) {
    LEVEL_PADDING(level + 1);
    fprintf(stderr, "%s, %lu\n", f->name, f->size);
    f = f->next;
  }
}

/**
 * Print out the contents of the cache to stderr.
 */
static inline void jjfs_cache_debug_print() {
  jjfs_cache_debug_print_dir(0, &top);
}
#endif

/**
 * Builds the asn1 structure from `dir' down.
 *
 * @param dir A directory to descend recursively into.
 * @param asn_dir A asn node to fill with the contents of `dir'.
 */
static int jjfs_build_asn_from_cache(jjfs_cache_dir *dir, JjfsDir_t *asn_dir) {
  JJFS_DEBUG_PRINT(2, "Building ASN.1 structure from dir %s\n",
                   dir->name);
  UTF8String_t *dname = OCTET_STRING_new_fromBuf(&asn_DEF_UTF8String,
                                                 dir->name,
                                                 strlen(dir->name) + 1);
  asn_dir->name = *dname;
  asn_dir->size = dir->size;
  
  jjfs_cache_file *f = dir->files;
  while (f) {
    JJFS_DEBUG_PRINT(2, "Adding file %s\n", f->name);
    JjfsFile_t *asn_f = (JjfsFile_t*)calloc(1, sizeof(JjfsFile_t));
    UTF8String_t *fname = OCTET_STRING_new_fromBuf(&asn_DEF_UTF8String,
                                                   f->name,
                                                   strlen(f->name) + 1);
    asn_f->name = *fname;
    asn_f->size = f->size;
    ASN_SEQUENCE_ADD(&asn_dir->files, asn_f);
    f = f->next;
  }

  jjfs_cache_dir *d = dir->subdirs;
  while (d) {
    JjfsDir_t *asn_d = (JjfsDir_t*)calloc(1, sizeof(JjfsDir_t));
    jjfs_build_asn_from_cache(d, asn_d);
    ASN_SEQUENCE_ADD(&asn_dir->subdirs, asn_d);
    d = d->next;
  }

  return 0;
}


static int jjfs_build_cache_from_asn(jjfs_cache_dir *dir, JjfsDir_t *asn_dir) {
  JJFS_DEBUG_PRINT(2, "Building cache structure from dir %s\n", (char*)asn_dir->name.buf);

  dir->name = strdup((char*)asn_dir->name.buf);
  dir->size = asn_dir->size;
  if ((!strcmp(dir->name, ".")) && (!strcmp(dir->name, ".."))) return 0;
  size_t nfiles = asn_dir->files.list.count;
  size_t nsubdirs = asn_dir->subdirs.list.count;

  JJFS_DEBUG_PRINT(2, "Adding files to cache\n");
  size_t i;
  jjfs_cache_file *fprev = NULL;
  for (i = 0; i < nfiles; i++) {
    JjfsFile_t *f = asn_dir->files.list.array[i];
    jjfs_cache_file *cf = calloc(1, sizeof(jjfs_cache_file));
    cf->name = strdup((char*)f->name.buf);
    cf->size = f->size;
    JJFS_DEBUG_PRINT(3, "Added file %s to cache with size %lu\n",
                     cf->name, cf->size);
    cf->next = fprev;
    fprev = cf;
  }
  dir->files = fprev;

  jjfs_cache_dir *dprev = NULL;
  for (i = 0; i < nsubdirs; i++) {
    jjfs_cache_dir *d = calloc(1, sizeof(jjfs_cache_dir));
    jjfs_build_cache_from_asn(d, asn_dir->subdirs.list.array[i]);
    d->next = dprev;
    dprev = d;
  }
  dir->subdirs = dprev;

#if DEBUG > 2
  if (dir == &top) {
    JJFS_DEBUG_PRINT(3, "Cache built from file:\n\n");
    jjfs_cache_debug_print();
  }
#endif

  return 0;
}

static int jjfs_cache_writer(const void *buf, size_t size, void *app_key) {
  FILE *f = (FILE*) app_key;
  size_t written = fwrite(buf, 1, size, f);
  return (written == size) ? 0 : -1;
}

/**
 * Write the cache to file.
 *
 * @return 0 on success, -1 otherwise.
 */
static int jjfs_cache_write() {
  JJFS_DEBUG_PRINT(1, "Writing cache file\n");
  JjfsDir_t *asn_dir = (JjfsDir_t*)calloc(1, sizeof(JjfsDir_t));
  jjfs_build_asn_from_cache(&top, asn_dir);

#if DEBUG > 2
  asn_fprint(stdout, &asn_DEF_JjfsDir, asn_dir);    
#endif
  
  int fd = open(jjfs_get_cache_file(), O_CREAT | O_RDWR, S_IRWXU | S_IRGRP);
  if (fd < 0) JJFS_DIE("Couldn't open cache file\n");
  /* Lock the file */
  flock(fd, LOCK_EX);
  FILE *f = fdopen(fd, "wb");

  
  asn_enc_rval_t er = der_encode(&asn_DEF_JjfsDir, asn_dir, jjfs_cache_writer, f);
  if (er.encoded == -1) {
    JJFS_DIE("Could not encode cache structure, failed type was %s\n",
             er.failed_type ? er.failed_type->name : "'unknown'");
  }
  /* unlock the file */
  flock(fd, LOCK_UN);
  /* fflush(f); */
  fclose(f);
  /* close(fd); */

  /* Free the ASN.1 structure */
  asn_DEF_JjfsDir.free_struct(&asn_DEF_JjfsDir, asn_dir, 0);
  return 0;
}

/**
 * Recurse the directory represented by path and parent, building the cache
 * structure.
 *
 * @param sftp An sftp session, must be connected.
 * @param path The path to start traversal in.
 * @param parent Parent of this directory.
 *
 * @return 0 on success, -1 otherwise.
 */
static int jjfs_build_cache_sftp(sftp_session sftp, const char *path,
                            jjfs_cache_dir *parent) {

  JJFS_DEBUG_PRINT(2, "Building cache for path %s\n", path);
  sftp_dir dir = sftp_opendir(sftp, path);  
  sftp_attributes attr;
  while ((attr = sftp_readdir(sftp, dir))) {
    if (attr->type == SSH_FILEXFER_TYPE_DIRECTORY) {
      jjfs_cache_dir *d = (jjfs_cache_dir*)calloc(1, sizeof(jjfs_cache_dir));
      d->name = strdup(attr->name);
      JJFS_DEBUG_PRINT(2, "Added dir %s\n", d->name);
      d->size = attr->size;
      d->next = parent->subdirs;
      parent->subdirs = d;
      if (!strncmp(d->name, ".", 2) || !strncmp(d->name, "..", 3)) continue;
      char *subpath = (char*)malloc(JJFS_SCRATCH_SIZE);
#ifdef __OpenBSD__
      strlcpy(subpath, path, JJFS_SCRATCH_SIZE);
      strlcat(subpath, "/", JJFS_SCRATCH_SIZE);
      strlcat(subpath, attr->name, JJFS_SCRATCH_SIZE);
#else
      strncpy(subpath, path, JJFS_SCRATCH_SIZE);
      strcat(subpath, "/");
      strncat(subpath, attr->name, JJFS_SCRATCH_SIZE);
#endif
      if (jjfs_build_cache_sftp(sftp, subpath, d) == -1) return -1;
      free(subpath);
    } else if (attr->type == SSH_FILEXFER_TYPE_REGULAR) {
      jjfs_cache_file *f = (jjfs_cache_file*)calloc(1, sizeof(jjfs_cache_file));
      f->name = strdup(attr->name);
      JJFS_DEBUG_PRINT(2, "Added file %s\n", f->name);      
      f->size = attr->size;
      f->next = parent->files;
      parent->files = f;
    }
  }
  sftp_attributes_free(attr);
  if (!sftp_dir_eof(dir)) return -1;
  sftp_closedir(dir);

  return 0;
}



int jjfs_cache_rebuild() {
  JJFS_DEBUG_PRINT(1, "Rebuilding cache file %s\n", jjfs_get_cache_file());
  top.name = "/";
  top.size = 11;
  if (jjfs_conn() == -1) return -1;
  jjfs_build_cache_sftp(jjfs_sftp(), jjfs_get_remote_dir(), &top);
  jjfs_disconn();

#if DEBUG > 2
  fprintf(stderr, "CACHE:\n\n");
  jjfs_cache_debug_print();
#endif

  jjfs_cache_write();
  return 0;
}

static int jjfs_cache_read() {
  struct stat s;
  JJFS_DEBUG_PRINT(1, "Trying to read cache file\n");
  const char *cache_file = jjfs_get_cache_file();
  int r = stat(cache_file, &s);

  if (r == -1) {
    switch(errno) {
    case ENOTDIR:
    case ENAMETOOLONG:
      JJFS_DIE("Cache file %s does not exist!\n", cache_file);
    case EACCES:
      JJFS_DIE("You do not seem to have permission to read cache file %s\n",
               cache_file);
    case ENOENT:
      JJFS_DEBUG_PRINT(1, "Cache file does not exist\n");
      return -1;
    default:
      JJFS_DIE("Error reading cache file\n");
    }
  }

  if (!S_ISREG(s.st_mode) && !S_ISLNK(s.st_mode))
    JJFS_DIE("Cache file %s is some weird file\n", cache_file);
  else if (s.st_size < 16) JJFS_DIE("Cache file is too small to be valid\n");

  /* The file seems to be OK */
  char *buf = calloc(s.st_size, 1);
  FILE *f = fopen(cache_file, "rb");
  if (!f) JJFS_DIE("Failed to open cache file for writing\n");
  size_t size = fread(buf, 1, s.st_size, f);
  if (!size) JJFS_DIE("Cache file corrupted");
  
  JjfsDir_t* asn_top = NULL;
  asn_dec_rval_t rval = ber_decode(0, &asn_DEF_JjfsDir, (void**)&asn_top, buf, size);

  if (rval.code == RC_FAIL) {
    return -1;
  } else if (rval.code == RC_WMORE) {
    return -1;
  } else if (rval.code != RC_OK) {
    return -1;
  }
  jjfs_build_cache_from_asn(&top, asn_top);

  return 0;
}

int jjfs_cache_init() {
  JJFS_DEBUG_PRINT(1, "Initializing the cache structure\n");
  if (jjfs_cache_read() == -1) {
    JJFS_DEBUG_PRINT(1, "Cache file not found, trying to rebuild\n");
    if (jjfs_cache_rebuild() == -1) {
      JJFS_DEBUG_PRINT(1, "Couldn't rebuild cache\n");
      return -1;
    }
    JJFS_DEBUG_PRINT(1, "Trying to read cache file again\n");
    if (jjfs_cache_read() == -1) {
      return -1;
    }
  }
  
  return 0;
}

void jjfs_cache_free_dir(jjfs_cache_dir *dir) {
  // First free the files
  if (dir == NULL) return;
  jjfs_cache_file *f = dir->files;
  while(f) {
    jjfs_cache_file *tmp = f;
    f = f->next;
    free(tmp);
  }

  jjfs_cache_dir *d = dir->subdirs;
  while(d) {
    jjfs_cache_free_dir(d);
    jjfs_cache_dir *tmp = d;
    d = d->next;
    free(tmp);
  }
}

void jjfs_cache_free() {
  JJFS_DEBUG_PRINT(1, "Freeing cache structure\n");
  jjfs_cache_free_dir(&top);
}

/* Who the hell has more than 64 levels? */
#define JJFS_MAX_PATH_COMPS 64

static int jjfs_cache_lookup_component(char **comps,
                                       jjfs_cache_dir *dir) {

  if (comps == NULL || comps[0] == NULL) return -1;
  char *comp = comps[0];
  // if this is the last entry.
  unsigned last = (comps[1] == NULL);

  if (last) {
    jjfs_cache_file *f = dir->files;
    while(f) {
      if (!strcmp(f->name, comp)) {
        ret.tag = JJFS_CACHE_FILE;
        ret.file = f;
        return 0;
      }
      f = f->next;
    }
    
    jjfs_cache_dir *d = dir->subdirs;    
    while(d) {
      if (!strcmp(d->name, comp)) {
        ret.tag = JJFS_CACHE_DIR;
        ret.dir = d;
        return 0;
      }
      d = d->next;
    }
    return -1;
  }

  jjfs_cache_dir *d = dir->subdirs;
  while(d) {
    if (!strcmp(d->name, comp)) {
      return jjfs_cache_lookup_component(comps + 1, d);
    }
    d = d->next;
  }
  return -1;
}

jjfs_cache_entry *jjfs_cache_lookup_path(const char *inpath) {

  char *c;
  unsigned i;
  int succ = -1;
  char *comps[JJFS_MAX_PATH_COMPS] = {0};

  char *path = strndup(inpath, JJFS_SCRATCH_SIZE);
  char *n;
  // remove any newlines
  for (n = path; *n != '\0'; n++) if (*n == '\n') *n = '\0';
  
  JJFS_DEBUG_PRINT(1, "Looking up cache entry for path %s\n", path);
  
  for (c = strtok(path, "/"), i = 0; c; c = strtok(NULL, "/"), i++) {
    if (i == JJFS_MAX_PATH_COMPS - 2) JJFS_DIE("Too many path components\n");
    comps[i] = c;
  }
  
  if (strcmp(path, "/") == 0) {
    ret.tag = JJFS_CACHE_DIR;
    ret.dir = &top;
    succ = 0;
  } else {
    succ = jjfs_cache_lookup_component((char**)&comps[0], &top);
  }
  
  free(path);

  if (succ == 0) return &ret;

  JJFS_DEBUG_PRINT(1, "Lookup failed!\n");
  return NULL;
}
