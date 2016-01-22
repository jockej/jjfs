# -*- Autoconf -*-

AC_PREREQ([2.69])
AC_INIT([jjfs], [0.1], [joakim.jalap@fastmail.com])
AM_INIT_AUTOMAKE([-Wall foreign])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/asn1/Makefile
                 doc/Makefile])

# Checks for programs.
AC_PROG_CC
AM_PROG_AR
AC_PROG_LIBTOOL

# Checks for libraries.
AC_SEARCH_LIBS([config_init], [config], [],
               [AC_MSG_ERROR([Couldn't find libconfig.])])

AC_SEARCH_LIBS([sftp_new], [ssh], [],
               [AC_MSG_ERROR([Couldn't find libssh.])])

# AC_SEARCH_LIBS(

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([float.h inttypes.h limits.h malloc.h netinet/in.h stddef.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STRNLEN
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memchr memmove memset realpath strdup strerror])





















AC_OUTPUT
