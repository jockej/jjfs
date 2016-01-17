/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "JjfsCache"
 * 	found in "jjfs_cache.asn1"
 */

#ifndef	_JjfsDir_H_
#define	_JjfsDir_H_


#include <asn_application.h>

/* Including external dependencies */
#include <PrintableString.h>
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct JjfsFile;
struct JjfsDir;

/* JjfsDir */
typedef struct JjfsDir {
	PrintableString_t	 name;
	struct files {
		A_SEQUENCE_OF(struct JjfsFile) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} files;
	struct subdirs {
		A_SEQUENCE_OF(struct JjfsDir) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} subdirs;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} JjfsDir_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_JjfsDir;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "JjfsFile.h"
#include "JjfsDir.h"

#endif	/* _JjfsDir_H_ */
#include <asn_internal.h>
