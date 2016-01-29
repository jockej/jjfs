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

/* Size used for various allocations of strings */
#define JJFS_SCRATCH_SIZE 2048

#ifdef DEBUG
#define JJFS_DEBUG_PRINT(lvl, ...)              \
  if (lvl >= DEBUG)                             \
    fprintf(stderr, ##__VA_ARGS__)
#else
#define JJFS_DEBUG_PRINT(lvl, ...)
#endif

#define JJFS_DIE_LINO_FILE(lino, file, uncond, ...)                     \
  do {                                                                  \
    if (errno) fprintf(stderr, "%s, %d: %s\n", file, lino, strerror(errno)); \
    else if (uncond) fprintf(stderr, "%s, %d: ", file, lino);           \
    fprintf(stderr, ##__VA_ARGS__);                                     \
    exit(EXIT_FAILURE);                                                 \
  } while(0)

#define JJFS_DIE(...) \
  JJFS_DIE_LINO_FILE(__LINE__, __FILE__, 0,  __VA_ARGS__)

#endif /* ifndef JJFS_MISC_H */
