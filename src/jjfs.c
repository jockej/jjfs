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
#include "jjfs_conf.h"
#include "jjfs_misc.h"
#include "jjfs_sftp.h"
#include "jjfs_cache.h"


int main(int argc, char **argv) {

  const char *cf = argv[1];

  const char *mp = argv[2];

  printf("Conf file: %s, mountpoint: %s\n", cf, mp);

  char *full_cf = realpath(cf, NULL);
  printf("Full path of conf file: %s\n", full_cf);
  
  jjfs_read_conf(full_cf, mp);

  free(full_cf);

  JJFS_DEBUG_PRINT("Server: %s\n", jjfs_get_server());
  JJFS_DEBUG_PRINT("Port: %d\n", *jjfs_get_port());
  JJFS_DEBUG_PRINT("Top_dir: %s\n", jjfs_get_top_dir());
  JJFS_DEBUG_PRINT("Cache file: %s\n", jjfs_get_cache_file());
  JJFS_DEBUG_PRINT("Mountpoint: %s\n", jjfs_get_mountpoint());
  JJFS_DEBUG_PRINT("User: %s\n", jjfs_get_user());
  JJFS_DEBUG_PRINT("Staging dir: %s\n", jjfs_get_staging_dir());  

  jjfs_cache_rebuild();
  
  return 0;
}
