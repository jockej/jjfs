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
#include <limits.h>
#define MAX_PATH_LEN 1024
#include "jjfs_conf.h"
#include "jjfs_misc.h"

#define JJFS_MAX_CONF_ELEM_LEN 64
#define JJFS_MAX_CONF_PATH_LEN (4*JJFS_MAX_CONF_ELEM_LEN + 5)


#ifndef JJFS_DEFAULT_CONF_FILE
#define JJFS_DEFAULT_CONF_FILE "~/.jjfs/jjfs.conf"
#endif

#ifndef JJFS_DEFAULT_PREFETCH_SIZE
#define JJFS_DEFAULT_PREFETCH_SIZE 10485760
#endif

static const char *server, *top_dir, *user, *sshconfig;

static int port, prefetch_bytes;

#define JJFS_STR_VAR(name)                      \
  const char *jjfs_get_##name() {               \
    return name;                                \
  }

JJFS_STR_VAR(server);
JJFS_STR_VAR(user);
JJFS_STR_VAR(top_dir);
JJFS_STR_VAR(sshconfig);

const int * const jjfs_get_port() {
  return &port;
}

int jjfs_prefetch_bytes() {
  return prefetch_bytes;
}

static char *jjfs_wordexp(const char *str) {
  if (str[0] == '~' && str[1] == '/') {
    char *homedir = getenv("HOME");
    if (!homedir) JJFS_DIE("Can't expand \"~\", $HOME not set.\n");
    char *ret = (char*)malloc(MAX_PATH_LEN);
    strncpy(ret, homedir, 200);
    strncat(ret, str + 2, 200);
    return ret;
  } else {
    return (char*)str;
  }
}

#define JJFS_READ_VAR_OR_DIE_LINO_MP(lino, mp, var, config_type)        \
  do {                                                                  \
    char path[MAX_PATH_LEN] = "mountpoints.";                           \
    strncat(path, mp, JJFS_MAX_CONF_ELEM_LEN);                          \
    strncat(path, ".", 1);                                              \
    strncat(path, #var, JJFS_MAX_CONF_ELEM_LEN);                        \
    if (config_lookup_##config_type(&cfg, path, &var) != CONFIG_TRUE)   \
      JJFS_DIE_LINO_FILE(lino, __FILE__, "Could not read var \"%s\"\n", path); \
  } while(0)

#define JJFS_READ_VAR_OR_DIE_LINO(lino, var, config_type)               \
  do {                                                                  \
    if (config_lookup_##config_type(&cfg, #var, &var) != CONFIG_TRUE)   \
      JJFS_DIE_LINO_FILE(lino, __FILE__, "Could not read var \"%s\"\n", #var); \
  } while(0)

#define JJFS_READ_VAR_OR_DIE_MP(mp, var, config_type)           \
  JJFS_READ_VAR_OR_DIE_LINO_MP(__LINE__, mp, var, config_type)

#define JJFS_READ_VAR_OR_DIE(var, config_type)          \
  JJFS_READ_VAR_OR_DIE_LINO(__LINE__, var, config_type)

#define JJFS_READ_VAR_OR_DEFAULT_MP(mp, var, config_type, dflt)         \
  do {                                                                  \
    char path[MAX_PATH_LEN] = "mountpoints.";                           \
    strncat(path, mp, JJFS_MAX_CONF_ELEM_LEN);                          \
    strncat(path, ".", 1);                                              \
    strncat(path, #var, JJFS_MAX_CONF_ELEM_LEN);                        \
    if (config_lookup_##config_type(&cfg, path, &var) != CONFIG_TRUE)   \
      var = dflt;                                                       \
  } while(0)

#define JJFS_READ_VAR_OR_DEFAULT(var, config_type, dflt)                \
  do {                                                                  \
    if (config_lookup_##config_type(&cfg, #var, &var) != CONFIG_TRUE)   \
      var = dflt;                                                       \
  } while(0)


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

  JJFS_READ_VAR_OR_DEFAULT(prefetch_bytes, int, JJFS_DEFAULT_PREFETCH_SIZE);
  JJFS_READ_VAR_OR_DIE_MP(mountpoint, server, string);
  JJFS_READ_VAR_OR_DIE_MP(mountpoint, port, int);
  JJFS_READ_VAR_OR_DIE_MP(mountpoint, top_dir, string);
  JJFS_READ_VAR_OR_DEFAULT_MP(mountpoint, user, string, NULL);
  JJFS_READ_VAR_OR_DEFAULT(sshconfig, string, NULL);
}
