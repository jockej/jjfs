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
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <getopt.h>
#include "config.h"
#include "jjfs_conf.h"
#include "jjfs_misc.h"
#include "jjfs_sftp.h"
#include "jjfs_cache.h"


int main(int argc, char **argv) {

  if (argc < 2) JJFS_DIE("Too few arguments");
  
  jjfs_read_conf(argc, argv);


  if (jjfs_is_rebuild()) {
    jjfs_cache_rebuild();
    exit(EXIT_SUCCESS);
  }

  
  jjfs_cache_init();

  jjfs_cache_entry *c;
  char s[2048];
  while(1) {
    fgets(s, 2048, stdin);
    c = jjfs_cache_lookup_path(s);
    if (JJFS_IS_DIR(c))
      printf("Dir: name: %s, size %lu\n", c->dir->name, c->dir->size);
    else
      printf("File: name: %s, size %lu\n", c->file->name, c->file->size);      
  }
  
  jjfs_cache_free();
  jjfs_conf_free();
  return 0;
}
