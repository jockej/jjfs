SUBDIRS = asn1

AM_LDFLAGS += -L./asn1
AM_CFLAGS = -Wall 

JJFS_MODULE_SOURCES = jjfs.c jjfs_sftp.c jjfs_cache.c jjfs_conf.c jjfs_misc.h
JJFS_MODULE_HEADERS = jjfs_sftp.h jjfs_cache.h jjfs_conf.h

bin_PROGRAMS = jjfs

jjfs_SOURCES = $(JJFS_MODULE_HEADERS) $(JJFS_MODULE_SOURCES)
jjfs_LDADD = libjjfsasn1.la
