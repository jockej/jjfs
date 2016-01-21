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

#include "jjfs_cache.h"
#include "jjfs_conf.h"
#include "jjfs_sftp.h"
#include "asn1/JjfsDir.h"
#include "asn1/JjfsFile.h"

jjfs_cache_entry ret;

jjfs_cache_dir top;

#ifdef JJFS_CACHE_XML
static void jjfs_cache_encode() {

}
#else
static void jjfs_cache_encode() {

}
#endif

static void jjfs_cache_write() {

}

#ifdef JJFS_CACHE_XML
static void jjfs_cache_decode() {

}
#else
static void jjfs_cache_decode() {

}
#endif

#define LEVEL_PADDING(lvl) \
  do {                     \
    unsigned i;                                         \
    for (i = 0; i < (lvl); i++) fprintf(stderr, "  ");  \
  } while(0)

static void jjfs_cache_debug_print_dir(unsigned level, jjfs_cache_dir *dir) {
  LEVEL_PADDING(level);
  fprintf(stderr, "%s, %lu\n", dir->name, dir->size);
  jjfs_cache_file *f = dir->files;
  while (f) {
    LEVEL_PADDING(level + 1);
    fprintf(stderr, "%s, %lu\n", f->name, f->size);
    f = f->next;
  }
  jjfs_cache_dir *d = dir->subdirs;
  while (d) {
    jjfs_cache_debug_print_dir(level + 1, d);
    d = d->next;
  }
}

static void jjfs_cache_debug_print() {

}

static int jjfs_recurse_dir(sftp_session sftp, sftp_dir dir,
                            const char *path, jjfs_cache_dir *parent) {
  
  sftp_attributes attr;
  while (attr = sftp_readdir(sftp, dir)) {
    if (attr->type == SSH_FILEXFER_TYPE_DIRECTORY) {
      jjfs_cache_dir *d = (jjfs_cache_dir*)malloc(sizeof(jjfs_cache_dir));
      d->name = strdup(attr->name);
      d->size = attr->size;
      d->next = parent->subdirs;
      parent->subdirs = d;

      const char *subpath = malloc(
#ifdef __OpenBSD__

#else

#endif
      
      sftp_dir subdir = sftp_opendir(sftp, 
      jjfs_recurse_dir(sftp, 
      
    } else if (attr->type SSH_FILEXFER_TYPE_FILE) {
      jjfs_cache_file *f = (jjfs_cache_file*)malloc(sizeof(jjfs_cache_file));
      f->name = strdup(attr->name);
      f->size = attr->size;
      f->next = parent->files;
      parent->files = f;
    }
  }

  if (!sftp_dir_eof()) return -1;

  sftp_attributes_free(attr);
  return 0;
}

int jjfs_cache_rebuild() {
  const char *cache_file = jjfs_get_cache_file();
  
  jjfs_conn();
  sftp_session sftp = jjfs_sftp();
  sftp_dir d = sftp_opendir(sftp, jjfs_get_top_dir());

  jjfs_read_dir(sftp, d);
  
  

  return 0;
}

static void jjfs_cache_read() {
  const char *cache_file = jjfs_get_cache_file();  
  
}

int jjfs_cache_init() {
  jjfs_cache_rebuild();

  
  return 0;
}

void jjfs_cache_free() {

}

jjfs_cache_entry *jjfs_cache_lookup_path(const char *path) {


  return &ret;
}

