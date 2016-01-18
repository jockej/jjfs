#ifndef JJFS_MISC_H
#define JJFS_MISC_H
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

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define JJFS_DIE_LINO_FILE(lino, file, format, ...)                     \
  do {                                                                  \
    if (errno != 0)                                                     \
      fprintf(stderr, "%s, %d: %s\n", file, lino, strerror(errno));     \
    fprintf(stderr, format, ##__VA_ARGS__);                             \
    exit(EXIT_FAILURE);                                                 \
  } while(0)

#define JJFS_DIE(format, ...) \
  JJFS_DIE_LINO_FILE(__LINE__, __FILE__, format, __VA_ARGS__)

#endif /* ifndef JJFS_MISC_H */
