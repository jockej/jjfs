SUBDIRS = asn1

AM_LDFLAGS = -L./asn1
AM_CFLAGS = -Wall

bin_PROGRAMS = jjfs

jjfs_SOURCES = jjfs.c jjfs_sftp.c jjfs_cache.c jjfs_conf.c jjfs_misc.h

jjfs_LDADD = ./asn1/libjjfsasn1.la
