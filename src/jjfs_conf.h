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
 * Read the config from conf_file.
 *
 * @param conf_file Path to the config file.
 * @param mountp The mountpoint to read config for.
 * @param cmdl_args An array of strings containing the command line arguments.
 */
void jjfs_read_conf(const char *conf_file, const char *mountp,
                    char **cmdl_args);

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
