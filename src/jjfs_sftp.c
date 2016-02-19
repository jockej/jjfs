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

#define JJFS_XFER_BUF_SIZE 16384

ssh_session ssh;
sftp_session sftp;

ssh_session jjfs_ssh() {
  return ssh;
}

sftp_session jjfs_sftp() {
  return sftp;
}

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
  //sftp_free(sftp);
  //ssh_free(ssh);
  return 0;
}

struct jjfs_read_args {
  int cache_fd;
  sftp_file remote;
  int ret;
};

static struct jjfs_read_args read_args;

static void *jjfs_async_read(void *raw_args) {
  char buf[JJFS_XFER_BUF_SIZE];

  struct jjfs_read_args *args = (struct jjfs_read_args*)raw_args;
  
  sftp_file remote = args->remote;
  // TODO: check return status here
  FILE* local = fdopen(dup(args->cache_fd), "a");

  int n;  
  while(1) {
    if ((n = sftp_read(remote, buf, sizeof(buf))) < 0) {
      JJFS_DEBUG_PRINT(1, "Read failed\n");
      args->ret = -1;
      pthread_exit(NULL);
    }
    if (n == 0) break;
    if (fwrite(buf, 1, n, local) != n) {
      JJFS_DEBUG_PRINT(1, "Failed to write to cache file\n");
      args->ret = -1;
      pthread_exit(NULL);
    }
  }

  sftp_close(remote);
  jjfs_disconn();
  fclose(local);
  args->ret = 0;
  pthread_exit(NULL);
}

pthread_t jjfs_start_async_read(const char *path, int fd) {

  char remote_path[JJFS_SCRATCH_SIZE];

  /* FIXME: fix the strncat and stuff */
#ifdef __OpenBSD__
  strlcpy(remote_path, jjfs_get_top_dir(), JJFS_SCRATCH_SIZE);
  strlcat(remote_path, path, JJFS_SCRATCH_SIZE);
#else
  strncpy(remote_path, jjfs_get_top_dir(), 256); // whatever...
  strncat(remote_path, path, 1024);
#endif

  jjfs_conn();
  
  sftp_file file = sftp_open(sftp, remote_path, O_RDONLY, 0);
  if (!file) {
    JJFS_DEBUG_PRINT(1, "Failed to open remote file %s\n", remote_path);
    return 0;
  }

  read_args.remote = file;
  read_args.cache_fd = fd;
  pthread_t t;
  if (pthread_create(&t, NULL, jjfs_async_read, &read_args) != 0) {
    JJFS_DEBUG_PRINT(1,"Failed to create read thread\n");
    return 0;
  }
  
  return t;
}
