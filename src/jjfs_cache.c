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

int jjfs_cache_rebuild() {
  const char *cache_file = jjfs_get_cache_file();


  
}

static void jjfs_cache_read() {
  const char *cache_file = jjfs_get_cache_file();  
  
}

int jjfs_cache_init() {

}

void jjfs_cache_free() {

}

jjfs_cache_entry *jjfs_cache_lookup_path(const char *path) {


  return &ret;
}

