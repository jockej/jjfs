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

#ifdef JJFS_CACHE_XML
static void jjfs_cache_encode_xml() {

}
#else
static void jjfs_cache_encode_ber() {
  
}
#endif

/**
 * Builds the asn1 structure from `dir' down.
 */
static int jjfs_cache_write_dir(jjfs_cache_dir *dir, JjfsDir_t *asn_dir) {
  UTF8String_t dname;
  OCTET_STRING_fromString(&dname, dir->name);
  asn_dir->name = dname;
  asn_dir->size = dir->size;
  
  jjfs_cache_file *f = dir->files;
  while (f) {
    JjfsFile_t *asn_f = (JjfsFile_t*)calloc(1, sizeof(JjfsFile_t));
    UTF8String_t fname;
    OCTET_STRING_fromString(&fname, f->name);
    asn_f->name = fname;
    asn_f->size = f->size;
    ASN_SEQUENCE_ADD(&asn_dir->files, asn_f);
    f = f->next;
  }

  jjfs_cache_dir *d = dir->subdirs;
  while (d) {
    JjfsDir_t *asn_d = (JjfsDir_t*)calloc(1, sizeof(JjfsDir_t));
    jjfs_cache_write_dir(d, asn_d);
    ASN_SEQUENCE_ADD(&asn_d->subdirs, asn_d);
    d = d->next;
  }

  return 0;
}


/**
 * Write the cache to file.
 *
 * @return 0 on success, -1 otherwise.
 */
static int jjfs_cache_write() {
  JjfsDir_t *asn_dir;
  asn_dir = (JjfsDir_t*)calloc(1, sizeof(JjfsDir_t));
  xer_fprint(stdout, &asn_DEF_JjfsDir, asn_dir);
  asn_DEF_JjfsDir.free_struct(&asn_DEF_JjfsDir, asn_dir, 0);

  return 0;
}

#ifdef JJFS_CACHE_XML
static void jjfs_cache_decode_xml() {

}
#else
static void jjfs_cache_decode_ber() {

}
#endif

#define LEVEL_PADDING(lvl) \
  do {                     \
    unsigned i;                                         \
    for (i = 0; i < (lvl); i++) fprintf(stderr, "  ");  \
  } while(0)

#ifdef DEBUG
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

static inline void jjfs_cache_debug_print() {
  jjfs_cache_debug_print_dir(0, &top);
}
#endif

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
static int jjfs_build_cache(sftp_session sftp, const char *path,
                            jjfs_cache_dir *parent) {
  
  sftp_dir dir = sftp_opendir(sftp, path);  
  sftp_attributes attr;
  while ((attr = sftp_readdir(sftp, dir))) {
    if (attr->type == SSH_FILEXFER_TYPE_DIRECTORY) {
      jjfs_cache_dir *d = (jjfs_cache_dir*)calloc(1, sizeof(jjfs_cache_dir));
      d->name = strdup(attr->name);
      d->size = attr->size;
      d->next = parent->subdirs;
      parent->subdirs = d;
      if (!strncmp(d->name, ".", 2) || !strncmp(d->name, "..", 3)) continue;
      char *subpath = (char*)malloc(JJFS_SCRATCH_SIZE);
#ifdef __OpenBSD__
      strlcpy(subpath, path, JJFS_SCRATCH_SIZE);
      strlcat(subpath, "/", 2);
      strlcat(subpath, attr->name, JJFS_SCRATCH_SIZE);
#else
      strncpy(subpath, path, JJFS_SCRATCH_SIZE);
      strcat(subpath, "/");
      strncat(subpath, attr->name, JJFS_SCRATCH_SIZE);
#endif
      if (jjfs_recurse_dir(sftp, subpath, d) == -1) return -1;
      free(subpath);
    } else if (attr->type == SSH_FILEXFER_TYPE_REGULAR) {
      jjfs_cache_file *f = (jjfs_cache_file*)calloc(1, sizeof(jjfs_cache_file));
      f->name = strdup(attr->name);
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
  const char *topdir = jjfs_get_top_dir();
  top.name = topdir;
  top.size = 11;
  jjfs_conn();
  jjfs_build_cache(jjfs_sftp(), topdir, &top);
  jjfs_disconn();

#ifdef DEBUG
  jjfs_cache_debug_print();
#endif

  jjfs_cache_write();
  return 0;
}

static void jjfs_cache_read() {
  const char *cache_file = jjfs_get_cache_file();  
  
}

int jjfs_cache_init() {
  jjfs_cache_rebuild();

  
  return 0;
}

void jjfs_cache_free_dir(jjfs_cache_dir *dir) {
  // First free the files
  jjfs_cache_file f = dir->files;
  while(f) {
    jjfs_cache_file *tmp = f;
    f = f->next;
    free(tmp);
  }

  jjfs_cache_dir d = dir->subdirs;
  while(d) {
    jjfs_cache_free_dir(d);
    jjfs_cache_dir *tmp = d;
    d = d->next;
    free(tmp);
  }
}

void jjfs_cache_free() {
  jjfs_cache_free_dir(&top);
}

jjfs_cache_entry *jjfs_cache_lookup_path(const char *path) {


  return &ret;
}

