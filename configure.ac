# -*- Autoconf -*-

AC_PREREQ([2.69])
AC_INIT([jjfs], [0.1], [joakim.jalap@fastmail.com])
AC_USE_SYSTEM_EXTENSIONS
AM_INIT_AUTOMAKE([-Wall foreign])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AM_PROG_AR
AC_PROG_LIBTOOL

AC_SEARCH_LIBS([config_init], [config], [],
               [AC_MSG_ERROR([Couldn't find libconfig.])])

AC_SEARCH_LIBS([sftp_new], [ssh], [],
               [AC_MSG_ERROR([Couldn't find libssh.])])


dnl PKG_CHECK_MODULES([CONFIG], [config])
dnl PKG_CHECK_MODULES([SSH], [ssh])

PKG_CHECK_MODULES([FUSE], [fuse])

AC_SEARCH_LIBS([fuse_main], [fuse], [],
               [AC_MSG_ERROR([Couldn't find libfuse.])])

AC_SEARCH_LIBS([pthread_create], [pthread], [PTHREAD_LIBS += -lpthread],
               [AC_MSG_ERROR([Couldn't find pthreads library])])


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

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/asn1/Makefile
                 doc/Makefile])

AC_OUTPUT
