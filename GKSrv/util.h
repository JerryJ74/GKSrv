#pragma once
// Util header file


#ifndef __UTIL_H
#define __UTIL_H

#include "GKConst.h"

int CopyCharToWord(WORD * pwDest, char * pchSrc, size_t wLen);
WORD * CopyCharToWord(RasMessage * ras, char * pchSrc, size_t wLen);
void FillTransportAddr(TransportAddress * pTransAddr, DWORD dwIPAddr, WORD wPort);
void AllocateProtocolID(ProtocolIdentifier * prid);
void ExtractE164FromAliasAddress(char * pchDest, OCTET * poSourceE164, WORD wLength);
void CopyToE164AliasAddress(char * pchDest, char * pchSource);

// return value 
// 0 : not matched
// => 1 : matched length
unsigned short IsMatchedPrefix(char * e164, char * prefix);

#endif