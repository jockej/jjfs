SUBDIRS = asn1

AM_LDFLAGS = -L./asn1
AM_CFLAGS = -Wall 

JJFS_SRCFILES = jjfs.c jjfs_sftp.c jjfs_cache.c jjfs_conf.c jjfs_misc.h
# JJFS_HDRFILES = jjfs_sftp.h jjfs_cache.h jjfs_conf.h

bin_PROGRAMS = jjfs

jjfs_SOURCES = $(JJFS_SRCFILES)
jjfs_LDADD = ./asn1/libjjfsasn1.la
