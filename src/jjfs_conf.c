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

#include "config.h"
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <libconfig.h>
#include "jjfs_conf.h"
#include "jjfs_misc.h"

#define JJFS_MAX_CONF_ELEM_LEN 128

#ifndef JJFS_DEFAULT_PORT
#define JJFS_DEFAULT_PORT 22
#endif

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


static int rebuild = 0;

static const char *entry;

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
JJFS_STR_VAR(staging_dir);
JJFS_STR_VAR(cache_file);

const int * jjfs_get_port() {
  return &port;
}

int jjfs_get_prefetch_bytes() {
  return prefetch_bytes;
}

int jjfs_is_rebuild() {
  return rebuild;
}

/* Construct the libconfig path */
#ifdef __OpenBSD__
#define JJFS_PATH(mp, var)                              \
  char path[JJFS_SCRATCH_SIZE] = {0};                   \
  if (strnlen(mp, 2) > 0) {                             \
    strlcpy(path, mp, JJFS_SCRATCH_SIZE);               \
    strlcat(path, ".", JJFS_SCRATCH_SIZE);              \
    strlcat(path, #var, JJFS_SCRATCH_SIZE);             \
  } else {                                              \
    strlcpy(path, #var, JJFS_MAX_CONF_ELEM_LEN);        \
  }
#else
#define JJFS_PATH(mp, var)                      \
  char path[JJFS_SCRATCH_SIZE] = {0};           \
  if (strnlen(mp, 2) > 0) {                     \
    strcpy(path, mp);                           \
    strcat(path, ".");                          \
    strcat(path, #var);                         \
  } else {                                      \
    strcpy(path, #var);                         \
  }
#endif

#define JJFS_READ_STR_OR_DIE_LINO(lino, mp, var)                        \
  do {                                                                  \
    char *c;                                                            \
    if ((c = args->var)) {                                              \
      var = strdup(c);                                                  \
    } else {                                                            \
      JJFS_PATH(mp, var);                                               \
      const char *tmp_##var;                                            \
      if (config_lookup_string(&cfg, path, &tmp_##var) != CONFIG_TRUE) { \
        JJFS_DIE_LINO_FILE(lino, __FILE__, 1,                           \
                           "Could not read var \"%s\"\n", path);        \
      } else {                                                          \
        var = strdup(tmp_##var);                                        \
      }                                                                 \
    }                                                                   \
  } while(0)

#define JJFS_READ_STR_OR_DIE(mp, var)           \
  JJFS_READ_STR_OR_DIE_LINO(__LINE__, mp, var)

#define JJFS_READ_STR_OR_DEFAULT(mp, var, dflt)                         \
  do {                                                                  \
    char *c;                                                            \
    if ((c = args->var)) {                                              \
      var = strdup(c);                                                  \
    } else if (mp) {                                                    \
      JJFS_PATH(mp, var);                                               \
      const char *tmp_##var = dflt;                                     \
      config_lookup_string(&cfg, path, &tmp_##var);                     \
      var = tmp_##var ? strdup(tmp_##var) : NULL;                       \
    }                                                                   \
  } while(0)

#define JJFS_READ_INT_OR_DEFAULT(mp, var, dflt)                         \
    do {                                                                \
      if (args->var) {                                                  \
        var = args->var;                                                \
      } else if (mp) {                                                  \
        JJFS_PATH(mp, var);                                             \
        var = dflt;                                                     \
        config_lookup_int(&cfg, path, &var);                            \
      }                                                                 \
    } while(0)    

static void jjfs_tilde_expand(const char **path) {
  if (!(*path != NULL && strnlen(*path, 3) > 1 &&
        (*path)[0] == '~' && (*path)[1] == '/')) return;
  char *homedir = getenv("HOME");
  if (!homedir) JJFS_DIE("Can't expand '~', $HOME not set\n");  
  char *scratch = (char*)calloc(JJFS_SCRATCH_SIZE, 1);
#ifdef __OpenBSD__
  strlcpy(scratch, homedir, JJFS_SCRATCH_SIZE);
  strlcat(scratch, *path + 1, JJFS_SCRATCH_SIZE);
#else
  strcpy(scratch, homedir);
  strcat(scratch, *path + 1);
#endif
  free((void*)(*path));
  *path = scratch;
}

void jjfs_read_conf(struct jjfs_args *args) {

  config_t cfg;
  const char *cf = args->conf_file ? args->conf_file : JJFS_DEFAULT_CONF_FILE;

  config_init(&cfg);

  if (!args->entry) {
    JJFS_DIE("Have to give an entry!\n");
  } else {
    entry = args->entry;
  }
  
  if (config_read_file(&cfg, cf) != CONFIG_TRUE) {
    fprintf(stderr, "libconfig error: %s on line %d\n",
            config_error_text(&cfg),
            config_error_line(&cfg));
    JJFS_DIE("Couldn't read conf file: %s!\n", cf);
  }

  /* These must be set */
  JJFS_READ_STR_OR_DIE(entry, server);
  JJFS_READ_STR_OR_DIE(entry, top_dir);

  /* Per entry options */
  JJFS_READ_INT_OR_DEFAULT(entry, port, 22);
  JJFS_READ_INT_OR_DEFAULT(entry, rebuild, 0);
  JJFS_READ_STR_OR_DEFAULT(entry, user, NULL);
  JJFS_READ_STR_OR_DEFAULT(entry, cache_file, NULL);

  /* Global options */
  JJFS_READ_INT_OR_DEFAULT("", prefetch_bytes, JJFS_DEFAULT_PREFETCH_SIZE);  
  JJFS_READ_STR_OR_DEFAULT("", sshconfig, NULL);
  JJFS_READ_STR_OR_DEFAULT("", staging_dir, "~" JJFS_DIR "staging");

  /* Then free the config object */
  config_destroy(&cfg);

  if (cache_file == NULL) {
    /* Construct default cache file name */
    char *scratch = (char*)calloc(JJFS_SCRATCH_SIZE, 1);
#ifdef __OpenBSD__
    strlcpy(scratch, "~", JJFS_SCRATCH_SIZE);
    strlcat(scratch, JJFS_DIR, JJFS_SCRATCH_SIZE);
    strlcat(scratch, entry, JJFS_SCRATCH_SIZE);
    strlcat(scratch, JJFS_CACHE_SUFFIX, JJFS_SCRATCH_SIZE);
#else
    strcpy(scratch, "~");
    strncat(scratch, JJFS_DIR, sizeof(JJFS_DIR));
    strncat(scratch, entry, JJFS_MAX_CONF_ELEM_LEN);
    strncat(scratch, JJFS_CACHE_SUFFIX, sizeof(JJFS_CACHE_SUFFIX));
#endif
    free((void*)cache_file);
    cache_file = scratch;
  }

  /* Expand all '~/' to '$HOME/' */
  jjfs_tilde_expand(&staging_dir);
  jjfs_tilde_expand(&cache_file);
  jjfs_tilde_expand(&sshconfig);

  JJFS_DEBUG_PRINT(1, "Done parsing conf\n");
  JJFS_DEBUG_PRINT(1, "Entry: %s\n", entry);
  JJFS_DEBUG_PRINT(1, "Server: %s\n", jjfs_get_server());
  JJFS_DEBUG_PRINT(1, "Port: %d\n", *jjfs_get_port());
  JJFS_DEBUG_PRINT(1, "Top_dir: %s\n", jjfs_get_top_dir());
  JJFS_DEBUG_PRINT(1, "User: %s\n", jjfs_get_user());
  JJFS_DEBUG_PRINT(1, "Staging dir: %s\n", jjfs_get_staging_dir());
  JJFS_DEBUG_PRINT(1, "Cache file: %s\n", jjfs_get_cache_file());  
}

void jjfs_conf_free() {
  free((void*)server);
  free((void*)user);
  free((void*)staging_dir);
  free((void*)top_dir);
  free((void*)sshconfig);              
}
