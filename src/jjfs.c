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
  .releasedir = jjfs_releasedir,
  .init = jjfs_init,
  .destroy = jjfs_destroy,
  .access = jjfs_access,
};


struct jjfs_args {
  char *server;
  char *user;
  char *cache_file;
  char *conf_file;
  char *staging_dir;
  char *sshconfig;
  char *mount_on;
  int port;
  int prefetch_bytes;
  int rebuild;
};

enum {
  KEY_HELP,
  KEY_VERSION,
  KEY_REBUILD
};

#define JJFS_OPT(t, p, v)\
  {t, offsetof(struct jjfs_args, p), v}

#define JJFS_LONGOPT(l, fmt, var, v)         \
  JJFS_OPT("--" #l " " #fmt, var, v),        \
    JJFS_OPT(#l "=" #fmt, var, v)

#define JJFS_SHORTOPT(s, fmt, var, v)       \
  JJFS_OPT("-" #s " " #fmt, var, v)

#define JJFS_BOTHOPT(l, s, fmt, var, v)   \
  JJFS_LONGOPT(l, fmt, var, v),               \
    JJFS_SHORTOPT(s, fmt, var, v)


static struct fuse_opts jjfs_opts {
  JJFS_BOTHOPT("server", "s", "%s", server, 0),
    JJFS_BOTHOPT("user", "u", "%s", user, 0),
    JJFS_BOTHOPT("conf-file", "c", "%s", conf_file),
    JJFS_LONGOPT("cache-file", "%s", cache_file, 0),
    JJFS_BOTHOPT("port", "p", "%s", port, 0),
    JJFS_LONGOPT("staging-dir", "%s", staging_dir, 0),
    JJFS_LONGOPT("sshconfig", "%s", sshconfig, 0),
    JJFS_BOTHOPT("mount-on", "m", "%s", mount_on, 0),
    JJFS_BOTHOPT("rebuild", "r", "", rebuild, 1),
    FUSE_OPT_KEY("-V", KEY_VERSION),
    FUSE_OPT_KEY("--version", KEY_VERSION),
    FUSE_OPT_KEY("-h", KEY_HELP),
    FUSE_OPT_KEY("--help", KEY_HELP),
    FUSE_OPT_END
};


static void echo_usage(const char *argv0) {
  printf("Usage: %s {mountpoint | {-r | --rebuild}} [options]\n"
         "\n"
         "general options:\n"
         "    -o opt [,opt ...]  mount options\n"
         "    -V | --version     print version\n"
         "    -h | --help        print help\n"
         "\n"
         "jjfs options:\n"
         "    --server | -s      server\n"
         , argv0);
}

static void echo_version() {

  printf(PACKAGE ", version " PACKAGE_VERSION
         ", please send any bugreports to "
         PACKAGE_BUGREPORT ".\n");
}

static int jjfs_opt_proc(void* data, const char *arg, int key,
                         struct fuse_args *outargs) {
  switch(key) {
  case KEY_HELP:




  case KEY_VERSION:
    


  default:
    
  }


}


int main(int argc, char **argv) {

  if (argc < 2) JJFS_DIE("Too few arguments");

  struct fuse_args fargs = FUSE_ARGS_INIT(argc, argv);

  

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
