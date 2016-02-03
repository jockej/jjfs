SUBDIRS = asn1

AM_LDFLAGS = -L./asn1
AM_CFLAGS = -Wall
AM_CPPFLAGS = -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=29

bin_PROGRAMS = jjfs

jjfs_SOURCES = jjfs.c jjfs_sftp.c jjfs_cache.c jjfs_conf.c jjfs_misc.h jjfs_fuse.c

jjfs_LDADD = ./asn1/libjjfsasn1.la
