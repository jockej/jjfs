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
#include <stdlib.h>

typedef struct jjfs_cache_file {
  const char * name;
  size_t size;
  jjfs_cache_file *next;
} jjfs_cache_file;

typedef struct jjfs_cache_dir {
  const char *name;
  const size_t size;
  const jjfs_cache_file *files;
  const jjfs_cache_dir *next, *subdirs;
} jjfs_cache_dir;

typedef enum {
  JJFS_CACHE_DIR,
  JJFS_CACHE_FILE
} jjfs_cache_type;

typedef struct {
  jjfs_cache_type tag;
  union {
    jjfs_cache_file file;
    jjfs_cache_dir dir;
  };
} jjfs_cache_entry;

#define JJFS_IS_DIR(entry) (entry->tag == JJFS_CACHE_DIR)
#define JJFS_IS_FILE(entry) (entry->tag == JJFS_CACHE_FILE)

int jjfs_cache_rebuild();

int jjfs_cache_init();

void jjfs_cache_free();
  
jjfs_cache_entry *jjfs_cache_lookup_path(const char *path);
