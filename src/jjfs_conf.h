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
#include <fuse.h>

struct jjfs_args {
  char *server;
  char *user;
  char *cache_file;
  char *conf_file;
  char *staging_dir;
  char *sshconfig;
  char *entry;
  char *top_dir;
  int port;
  int prefetch_bytes;
  int rebuild;
};




/**
 * Read the config from conf_file.
 *
 */
void jjfs_read_conf(struct jjfs_args *args);

/**
 * Free the config memory.
 */
void jjfs_conf_free();

const char *jjfs_get_server();

const char *jjfs_get_top_dir();

const char *jjfs_get_user();

const char *jjfs_get_sshconfig();

const char *jjfs_get_cache_file();

const int * const jjfs_get_port();

const char *jjfs_get_mountpoint();

const char *jjfs_get_staging_dir();

int jjfs_is_rebuild();

#endif /* ifndef JJFS_CONF_H */
