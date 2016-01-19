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
#include <stdlib.h>
#include "jjfs_sftp.h"
#include "jjfs_misc.h"
#include "jjfs_conf.h"

ssh_session ssh;
sftp_session sftp;

ssh_session jjfs_ssh() {
  return ssh;
}

sftp_session jjfs_sftp() {
  return sftp;
}

#define JJFS_TRY_EQ(call, exp, ...)             \
  do {                                          \
    if ((call) == exp) {                        \
      fprintf(stderr, ##__VA_ARGS__);           \
      return -1;                                \
    }                                           \
  } while(0)

#define JJFS_TRY_NE(call, exp, ...)             \
  do {                                          \
    if ((call) != exp) {                        \
      fprintf(stderr, ##__VA_ARGS__);           \
      return -1;                                \
    }                                           \
  } while(0)


int jjfs_conn() {
  ssh = ssh_new();

  JJFS_TRY_EQ(ssh, NULL, "Couldn't allocate ssh session\n");

  ssh_options_set(ssh, SSH_OPTIONS_HOST, jjfs_get_server());
  ssh_options_set(ssh, SSH_OPTIONS_PORT, jjfs_get_port());
  ssh_options_set(ssh, SSH_OPTIONS_USER, jjfs_get_user());
  ssh_options_parse_config(ssh, jjfs_get_sshconfig());


  JJFS_TRY_NE(ssh_connect(ssh), SSH_OK, "Error connecting: %s\n",\
              ssh_get_error(ssh));

  JJFS_TRY_NE(ssh_is_server_known(ssh), SSH_SERVER_KNOWN_OK,\
              "Server is not known\n");
  JJFS_TRY_NE(ssh_userauth_autopubkey(ssh, NULL), SSH_AUTH_SUCCESS,
              "Failed to authenticate!\n");

  sftp = sftp_new(ssh);
  JJFS_TRY_EQ(sftp, NULL, "Couldn't allocate sftp session\n");

  JJFS_TRY_NE(sftp_init(sftp), SSH_OK,
              "Sftp init error: %d\n", sftp_get_error(sftp));
  return 0;
}

int jjfs_disconn() {
  ssh_disconnect(ssh);
  ssh_free(ssh);
  sftp_free(sftp);
  return 0;
}
