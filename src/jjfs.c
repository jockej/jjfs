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
#include <stddef.h>
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

enum {
  KEY_HELP,
  KEY_VERSION,
};

#define JJFS_OPT(t, p, v)                \
  {t, offsetof(struct jjfs_args, p), v}

static struct fuse_opt jjfs_opts[] = {
  JJFS_OPT("--entry %s", entry, 0),
  JJFS_OPT("--server %s", server, 0),  
  JJFS_OPT("server=%s", server, 0),  
  JJFS_OPT("--user %s", user, 0),
  JJFS_OPT("user=%s", user, 0),  
  JJFS_OPT("-c %s", conf_file, 0),
  JJFS_OPT("--conf-file %s", conf_file, 0),
  JJFS_OPT("conf-file=%s", conf_file, 0),    
  JJFS_OPT("cache-file=%s", cache_file, 0),
  JJFS_OPT("--port %i", port, 0),
  JJFS_OPT("port=%i", port, 0),
  JJFS_OPT("top-dir=%s", top_dir, 0),
  JJFS_OPT("--top-dir %s", top_dir, 0),  
  JJFS_OPT("staging-dir=%s", staging_dir, 0),
  JJFS_OPT("sshconfig=%s", sshconfig, 0),
  JJFS_OPT("prefetch-bytes=%i", prefetch_bytes, 0),  
  JJFS_OPT("--rebuild", rebuild, 1),
  JJFS_OPT("r", rebuild, 1),  
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

/**
 * This functions only handles those options which were declared with
 * FUSE_OPT_KEY
 *
 */
static int jjfs_opt_proc(void* data, const char *arg, int key,
                         struct fuse_args *outargs) {
  switch(key) {
  case KEY_HELP:
    echo_usage(outargs->argv[0]);
    fuse_opt_add_arg(outargs, "-ho");
    fuse_main(outargs->argc, outargs->argv, &jjfs_oper, NULL);
    exit(EXIT_SUCCESS);
  case KEY_VERSION:
    echo_version();
    fuse_opt_add_arg(outargs, "--version");
    fuse_main(outargs->argc, outargs->argv, &jjfs_oper, NULL);
    exit(EXIT_SUCCESS);
  }
  return 1;
}


int main(int argc, char **argv) {

  if (argc < 2) JJFS_DIE("Too few arguments");

  struct fuse_args fargs = FUSE_ARGS_INIT(argc, argv);

  struct jjfs_args jjfsargs;

  memset(&jjfsargs, 0, sizeof(jjfsargs));

  fuse_opt_parse(&fargs, &jjfsargs, jjfs_opts, jjfs_opt_proc);

  jjfs_read_conf(&jjfsargs);

  if (jjfs_is_rebuild()) {
    jjfs_cache_rebuild();
    
    exit(EXIT_SUCCESS);
  }

  return fuse_main(fargs.argc, fargs.argv, &jjfs_oper, NULL);
}
