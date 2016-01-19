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
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <stdlib.h>
#include "jjfs_sftp.h"
#include "jjfs_misc.h"


ssh_session ssh;
sftp_session sftp;

int jjfs_conn() {
  ssh = ssh_new();
  if (!ssh) JJFS_DIE("Couldn't allocate ssh session\n");

  ssh_options_set(ssh, SSH_OPTIONS_HOST, jjfs_get_server());
  ssh_options_set(ssh, SSH_OPTIONS_PORT, jjfs_get_port());
  
  ssh_options_set(ssh, SSH_OPTIONS_PORT, jjfs_get_port());  
  
}

int jjfs_disconn() {

}
