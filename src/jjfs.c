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
#include "config.h"
#include "jjfs_conf.h"
#include "jjfs_misc.h"
#include "jjfs_sftp.h"
#include "jjfs_cache.h"
#include "jjfs_fuse.h"

struct fuse_operations jjfs_oper = {
  .getattr = jjfs_getattr,
  /* .fgetattr = jjfs_fgetattr, */
  /* .read = jjfs_read, */
  /* .readbuf = jjfs_read_buf, */
  .open = jjfs_open,
  /* .release = jjfs_release, */
  .opendir = jjfs_opendir,
  .readdir = jjfs_readdir,
  /* .releasedir = jjfs_releasedir, */
  .init = jjfs_init,
  .destroy = jjfs_destroy,
  /* .access = jjfs_access */
};


int main(int argc, char **argv) {

  if (argc < 2) JJFS_DIE("Too few arguments");
  
  jjfs_read_conf(argc, argv);


  if (jjfs_is_rebuild()) {
    jjfs_cache_rebuild();
    exit(EXIT_SUCCESS);
  }

  char **fargv = calloc(6, sizeof(char*));
  fargv[0] = strdup("jjfs");
  fargv[1] = strdup(jjfs_get_mountpoint());
  fargv[2] = "-d";
  fargv[3] = "-s";
  fargv[4] = "-f";
  
  return fuse_main(5, fargv, &jjfs_oper, NULL);
}
