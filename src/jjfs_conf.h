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
void jjfs_read_conf(const char *conf_file, const char *mountpoint);

const char *jjfs_get_server();

const char *jjfs_get_top_dir();

const char *jjfs_get_user();

const char *jjfs_get_sshconfig();

const char *jjfs_get_cache_file();

const int * const jjfs_get_port();
