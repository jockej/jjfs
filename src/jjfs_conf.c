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
#include "jjfs_conf.h"
#include "jjfs_misc.h"

#define JJFS_MAX_CONF_ELEM_LEN 64
#define JJFS_SCRATCH_SIZE 2096

#ifndef JJFS_DEFAULT_CONF_FILE
#define JJFS_DEFAULT_CONF_FILE "~/.jjfs/jjfs.conf"
#endif

#ifndef JJFS_DEFAULT_PREFETCH_SIZE
#define JJFS_DEFAULT_PREFETCH_SIZE 10485760
#endif

#ifndef JJFS_CACHE_SUFFIX
#define JJFS_CACHE_SUFFIX "-cache"
#endif

#ifndef JJFS_DIR
#define JJFS_DIR "/.jjfs/"
#endif


static int port, prefetch_bytes;

#define JJFS_STR_VAR(name)                            \
  const static char *name;                            \
  const char *jjfs_get_##name() {                     \
    return name;                                      \
  }

JJFS_STR_VAR(server);
JJFS_STR_VAR(user);
JJFS_STR_VAR(top_dir);
JJFS_STR_VAR(sshconfig);
JJFS_STR_VAR(mountp);
JJFS_STR_VAR(cache_file);
JJFS_STR_VAR(staging_dir);

const int * const jjfs_get_port() {
  return &port;
}

int jjfs_prefetch_bytes() {
  return prefetch_bytes;
}

#define JJFS_PATH(mp, var) \
  char path[JJFS_SCRATCH_SIZE];\
  



#define JJFS_READ_STR_OR_DIE_LINO(lino, mp, var)        \
  do {                                                                  \
  
    if (config_lookup_string(&cfg, path, &var) != CONFIG_TRUE)   \
      JJFS_DIE_LINO_FILE(lino, __FILE__, "Could not read var \"%s\"\n", path); \
  } while(0)

#define JJFS_READ_STR_OR_DIE(mp, var)           \
  JJFS_READ_STR_OR_DIE_LINO_MP(__LINE__, mp, var, config_type)

#define JJFS_READ_STR_OR_DEFAULT(mp, var, dflt)         \
  do {                                                                  \
    var = dflt;                                                         \
    config_lookup_##config_type(&cfg, path, &var);                      \
  } while(0)

#define JJFS_READ_STR_OR_DEFAULT(var, config_type, dflt)                \
  do {                                                                  \
    var = dflt;                                                         \
    config_lookup_##config_type(&cfg, #var, &var);                      \
  } while(0)

#define JJFS_READ_INT_OR_DEFAULT_MP(mp, var, config_type, dflt)         \
  do {                                                                  \
    var = dflt;                                                         \
    config_lookup_##config_type(&cfg, path, &var);                      \
  } while(0)

#define JJFS_READ_INT_OR_DEFAULT(var, config_type, dflt)                \
  do {                                                                  \
    var = dflt;                                                         \
    config_lookup_##config_type(&cfg, #var, &var);                      \
  } while(0)


static void jjfs_tilde_expand(const char *path) {
  if (!(path != NULL && strnlen(path, 3) > 1 &&
        path[0] == '~' && path[1] == '/')) return;
  char *homedir = getenv("HOME");
  if (!homedir) JJFS_DIE("Can't expand '~', $HOME not set\n");  
  char *scratch = (char*)calloc(JJFS_SCRATCH_SIZE, 1);
  strcpy(scratch, homedir);
  strcat(scratch, path);
  free((void*)path);
  path = scratch;
}

void jjfs_read_conf(const char *conf_file, const char *mountpoint) {

  config_t cfg;

  const char *cf = conf_file != NULL ? conf_file : JJFS_DEFAULT_CONF_FILE;

  config_init(&cfg);

  if (config_read_file(&cfg, cf) != CONFIG_TRUE) {
    fprintf(stderr, "libconfig error: %s on line %d\n",
            config_error_text(&cfg),
            config_error_line(&cfg));
    JJFS_DIE("Couldn't read conf file: %s!\n", cf);
  }

  /* These must be set */
  JJFS_READ_VAR_OR_DIE_MP(mountpoint, server, string);
  JJFS_READ_VAR_OR_DIE_MP(mountpoint, top_dir, string);

  JJFS_READ_VAR_OR_DEFAULT(prefetch_bytes, int, JJFS_DEFAULT_PREFETCH_SIZE);
  JJFS_READ_VAR_OR_DEFAULT_MP(mountpoint, port, int, 22);
  JJFS_READ_VAR_OR_DEFAULT_MP(mountpoint, user, string, NULL);
  JJFS_READ_VAR_OR_DEFAULT(sshconfig, string, NULL);
  JJFS_READ_VAR_OR_DEFAULT(staging_dir, string, "~" JJFS_DIR "staging");

  JJFS_READ_VAR_OR_DEFAULT_MP(mountpoint, mountp, string, NULL);
  JJFS_READ_VAR_OR_DEFAULT_MP(mountpoint, cache_file, string, NULL);

  /* Move all strings to memory we control */
  FOR_ALL_STR_VARS(jjfs_realloc_string);
  /* Then free the config object */
  config_destroy(&cfg);
  
  if (cache_file == NULL) {
    /* Construct default cache file name */
    char *scratch = (char*)calloc(JJFS_SCRATCH_SIZE, 1);
    strcpy(scratch, "~");
    strncat(scratch, JJFS_DIR, sizeof(JJFS_DIR));
    strncat(scratch, mountpoint, JJFS_MAX_CONF_ELEM_LEN);
    strncat(scratch, JJFS_CACHE_SUFFIX, sizeof(JJFS_CACHE_SUFFIX));
    cache_file = scratch;
  }

  if (mountp == NULL) {
    /* Construct default mountp path */
    char *scratch = (char*)calloc(JJFS_SCRATCH_SIZE, 1);
    strcpy(scratch, "~/");
    strncat(scratch, mountpoint, JJFS_MAX_CONF_ELEM_LEN);
    mountp = scratch;
  }

  /* Expand all '~/' to '$HOME/' */
  FOR_ALL_STR_VARS(jjfs_tilde_expand);
}
