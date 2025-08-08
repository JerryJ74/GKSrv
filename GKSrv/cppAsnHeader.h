////////////////////////////////////////////////////////////////////////
//
// asn1api.h
//
// This is the external interface for the ASN1 library. This is designed
// to be a full multi-instance library to avoid having multiple copies
// of ASN1 encoders/decoders in a system
//
// Copyright 2000-2001 Netergy Microelectronics
//
////////////////////////////////////////////////////////////////////////
#ifndef __ASN1API_H__
#define __ASN1API_H__

#include <dblk.h>


#define NEXTENSION

/* Type definitions */
struct String {
  OCTET *po;
  WORD  wSize;
};

struct WString {
  WORD *pw;
  WORD wSize;
};


#define COPY_H_IPV4ADDR_TO_N_CHS(po, dwHostIpAddr) do {			\
	  *(po) = (OCTET)dwHostIpAddr;								\
	  *((po) + 1) = (OCTET)(0xFF & (dwHostIpAddr >> 8));		\
	  *((po) + 2) = (OCTET)(0xFF & (dwHostIpAddr >> 16));		\
	  *((po) + 3) = (OCTET)(0xFF & (dwHostIpAddr >> 24));		\
	} while (0)


#define COPY_N_CHS_TO_H_IPV4ADDR(dwHostIpAddr, po) do {			\
	  dwHostIpAddr = *(po);										\
	  dwHostIpAddr |= *((po) + 1) << 8;							\
	  dwHostIpAddr |= *((po) + 2) << 16;						\
	  dwHostIpAddr |= *((po) + 3) << 24;						\
	} while (0)

/*
#define COPY_N_CHS_TO_H_IPV4ADDR(dwHostIpAddr, po) do {			\
	  dwHostIpAddr = *(po) << 24;				\
	  dwHostIpAddr |= *((po) + 1) << 16;			\
	  dwHostIpAddr |= *((po) + 2) << 8;			\
	  dwHostIpAddr |= *((po) + 3);				\
	} while (0)
*/

/* Prototypes */

extern "C" void *Asn1Attach(void *, void *);
extern "C" void *Asn1Detach(void *);

extern "C" void *Asn1Malloc(unsigned);
extern "C" unsigned Asn1Size(const WORD*,unsigned uRef, void **ppData);
extern "C" int Asn1EncodeBuf(const WORD*,int, void *, OCTET *, int);
extern "C" int Asn1DecodeBuf(const WORD*,int, void **, OCTET *, int);
extern "C" const WORD awRasAsn1Table[2476];
extern "C" const WORD awQ931Asn1Table[1216];
//const WORD awQ931Asn1Table[1216]
extern "C" void Asn1Print(const WORD*, int nMsg, void *pMsg);
extern "C" void Asn1Free(void *);
//#ifdef NAMES
//void Asn1Print(const WORD*,int, void *);
//#endif

// the following functions are also available (and used internally). Their
// use is NOT recommended as Asn1Encode/DecodeBuf provide a simpler 
// interface without needing to use dblk.h
int   _Asn1Encode(const WORD*,int, void *, DATA_BLK *);
int   _Asn1Decode(const WORD*,int, void **, DATA_BLK *);


#endif /* __ASN1API_H__ */
