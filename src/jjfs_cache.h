#ifndef JJFS_CACHE_H
#define JJFS_CACHE_H
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
 * This module handles writing of and lookup in the cache.
 */

#include <stdlib.h>

/**
 * An entry in the cache for a file.
 */
typedef struct jjfs_cache_file {
  /**
   * Name of the file.
   */
  const char * name;
  /**
   * Size of the file in bytes.
   */
  size_t size;
  /**
   * Pointer to the next file in the same directory.
   * NULL if there are no more files.
   */
  struct jjfs_cache_file *next;
} jjfs_cache_file;

/**
 * An entry in the cache for a directory.
 */
typedef struct jjfs_cache_dir {
  /**
   * Name of this directory.
   */
  const char *name;
  /**
   * Size of this directory (thu number of things it)
   */
  size_t size;
  /**
   * Pointer to NULL terminated linked list of files in this derectory.
   */
  struct jjfs_cache_file *files;
  /**
   * Pointer to next directory on this level, or NULL if no more.
   */
  struct jjfs_cache_dir *next;
  /**
   * Pointer to a NULL terminated linked list of subdirectories
   */
  struct jjfs_cache_dir *subdirs;
} jjfs_cache_dir;

/**
 * Enum for the tag of `jjfs_cache_entry'.
 */
typedef enum {
  JJFS_CACHE_DIR = 0,
  JJFS_CACHE_FILE = 1
} jjfs_cache_type;

/**
 * An entry in the cache.
 */
typedef struct {
  /**
   * Contents.
   */
  union {
    jjfs_cache_file *file;
    jjfs_cache_dir *dir;
  };
  /**
   * Tag to determine wheteher this is a directory or a file.
   */
  jjfs_cache_type tag:1;
} jjfs_cache_entry;

#define JJFS_IS_DIR(entry) (entry->tag == JJFS_CACHE_DIR)
#define JJFS_IS_FILE(entry) (entry->tag == JJFS_CACHE_FILE)

/**
 * Rebuild the cache.
 *
 * @return 0 on success,  -1 if there was an error.
 */
int jjfs_cache_rebuild();

/**
 * Initialize the cache.
 *
 * Reads from `cache_file' if it exists, otherwise rebuilds the cache.
 *
 * @return 0 on success, -1 otherwise.
 */
int jjfs_cache_init();

/**
 * Free the memory of the cache structure.
 */
void jjfs_cache_free();

/**
 * Lookup an entry in the cache.
 *
 * @param path The path to lookup.
 *
 * @return NULL if the lookup failed, otherwise a pointer to a jjfs_cache_entry
 *structure. Note that this structure will be reused by the next call to this
 *function, so if you want to keep it, you have to memcpy it or something.
 */
jjfs_cache_entry *jjfs_cache_lookup_path(const char *path);

#endif /* ifndef JJFS_CACHE_H */
