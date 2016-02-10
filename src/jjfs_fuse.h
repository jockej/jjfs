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

/**
 * This file declares the fuse callbacks for jjfs
 */

int jjfs_getattr(const char *path, struct stat *st);

int jjfs_fgetattr(const char *path, struct stat *st, struct fuse_file_info *fi);

int jjfs_read(const char *path, char *buf, size_t size, off_t offs,
              struct fuse_file_info *fi);

/* int jjfs_read_buf(const char *path, struct fuse_bufvec *bufp, */
                  /* size_t size, off_t offs, struct fuse_file_info *fi); */

int jjfs_open(const char *path, struct fuse_file_info *fi);

int jjfs_release(const char *path, struct fuse_file_info *fi);

int jjfs_opendir(const char *path, struct fuse_file_info *fi);

int jjfs_readdir(const char *path, void *, fuse_fill_dir_t filler, off_t offs,
                 struct fuse_file_info *fi);

int jjfs_releasedir(const char *path, struct fuse_file_info *fi);

void *jjfs_init (struct fuse_conn_info *conn);

void jjfs_destroy(void *v);

int jjfs_access(const char *path, int );
