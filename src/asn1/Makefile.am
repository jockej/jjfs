ASN_SRCFILES=	\
	JjfsFile.c	\
	JjfsDir.c

ASN_HDRFILES=	\
	JjfsFile.h	\
	JjfsDir.h

ASN_HDRFILES+=UTF8String.h
ASN_HDRFILES+=INTEGER.h
ASN_HDRFILES+=NativeEnumerated.h
ASN_SRCFILES+=INTEGER.c
ASN_SRCFILES+=NativeEnumerated.c
ASN_HDRFILES+=NativeInteger.h
ASN_SRCFILES+=NativeInteger.c
ASN_SRCFILES+=UTF8String.c
ASN_HDRFILES+=asn_SEQUENCE_OF.h
ASN_SRCFILES+=asn_SEQUENCE_OF.c
ASN_HDRFILES+=asn_SET_OF.h
ASN_SRCFILES+=asn_SET_OF.c
ASN_HDRFILES+=constr_SEQUENCE.h
ASN_SRCFILES+=constr_SEQUENCE.c
ASN_HDRFILES+=constr_SEQUENCE_OF.h
ASN_SRCFILES+=constr_SEQUENCE_OF.c
ASN_HDRFILES+=constr_SET_OF.h
ASN_SRCFILES+=constr_SET_OF.c
ASN_HDRFILES+=asn_application.h
ASN_HDRFILES+=asn_system.h
ASN_HDRFILES+=asn_codecs.h
ASN_HDRFILES+=asn_internal.h
ASN_HDRFILES+=OCTET_STRING.h
ASN_SRCFILES+=OCTET_STRING.c
ASN_HDRFILES+=BIT_STRING.h
ASN_SRCFILES+=BIT_STRING.c
ASN_SRCFILES+=asn_codecs_prim.c
ASN_HDRFILES+=asn_codecs_prim.h
ASN_HDRFILES+=ber_tlv_length.h
ASN_SRCFILES+=ber_tlv_length.c
ASN_HDRFILES+=ber_tlv_tag.h
ASN_SRCFILES+=ber_tlv_tag.c
ASN_HDRFILES+=ber_decoder.h
ASN_SRCFILES+=ber_decoder.c
ASN_HDRFILES+=der_encoder.h
ASN_SRCFILES+=der_encoder.c
ASN_HDRFILES+=constr_TYPE.h
ASN_SRCFILES+=constr_TYPE.c
ASN_HDRFILES+=constraints.h
ASN_SRCFILES+=constraints.c
ASN_HDRFILES+=xer_support.h
ASN_SRCFILES+=xer_support.c
ASN_HDRFILES+=xer_decoder.h
ASN_SRCFILES+=xer_decoder.c
ASN_HDRFILES+=xer_encoder.h
ASN_SRCFILES+=xer_encoder.c
ASN_HDRFILES+=per_support.h
ASN_SRCFILES+=per_support.c
ASN_HDRFILES+=per_decoder.h
ASN_SRCFILES+=per_decoder.c
ASN_HDRFILES+=per_encoder.h
ASN_SRCFILES+=per_encoder.c
ASN_HDRFILES+=per_opentype.h
ASN_SRCFILES+=per_opentype.c
#ASN_CONVERTER_SOURCES+=converter-sample.c


lib_LTLIBRARIES=libjjfsasn1.la
libjjfsasn1_la_SOURCES=$(ASN_SRCFILES) $(ASN_HDRFILES)
libjjfsasn1_la_DEPENDENCIES = ../jjfs_cache.asn1
