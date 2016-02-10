#ifndef JJFS_CONF_H
#define JJFS_CONF_H
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

/**
 * This module handles retrieving and  setting of configuration variables.
 */

#include <fuse.h>

struct jjfs_args {
  char *server;
  char *user;
  char *staging_dir;
  char *conf_file;
  char *cache_file;
  char *sshconfig;
  char *entry;
  char *top_dir;
  int port;
  int prefetch_bytes;
  int rebuild;
};




/**
 * Read the config and set config parameters.
 *
 * This function will read the config file, wither the one given with a command
 * line option or the default one, and also the arguments gathered from the
 * command line. It will then set the various parameters.
 *
 * Options given on the command line takes precedence over those in the config
 * file.
 *
 * For most parameters there are defaults which will be set if the parameter is
 * not set in the config file or given on the command line. Some parameters have
 * no defaults, these are `server' and `top_dir' as we cannot possibly guess the
 * value of those.
 *
 * @param args Arguments from the command line.
 */
void jjfs_read_conf(struct jjfs_args *args);

/**
 * Free the config memory.
 */
void jjfs_conf_free();

/**
 * Return the server.
 */
const char *jjfs_get_server();

/**
 * Return the top directory.
 */
const char *jjfs_get_top_dir();

/**
 * Return the user to connect as.
 */
const char *jjfs_get_user();

/**
 * Return the path to the ssh config file to use.
 */
const char *jjfs_get_sshconfig();

/**
 * Return the path to the staging dir.
 */
const char *jjfs_get_staging_dir();

/**
 * Return the path to the cache file.
 */
const char *jjfs_get_cache_file();

/**
 * Return the number of bytes to prefetch before returning from `jjfs_open'.
 */
int jjfs_get_prefetch_bytes();

/**
 * Return the port to connect to.
 *
 * This returns a pointer because that's what libssh wants.
 */
const int * jjfs_get_port();

/**
 * Return 1 if this is a call to rebuild, otherwise 0.
 */
int jjfs_is_rebuild();

#endif /* ifndef JJFS_CONF_H */
