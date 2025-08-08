#pragma once

#include "cppAsnHeader.h"
#include "asn1v4.h"
#include "GKConst.h"

class H323EnDecoder
{
public:
	H323EnDecoder(void);
	~H323EnDecoder(void);
	static H323EnDecoder * Instance();
protected:
	static H323EnDecoder * m_instance;
    CRITICAL_SECTION m_critical;
	//CFile m_file;
	//void _DD(char * m, ...);
	//DWORD m_debugline;
public:
	int RASDecode(void ** pMsg, OCTET * rawBuff, DWORD size);
	int RASEncode(void * pMsg, OCTET * rawBuff);
	int Q931Decode(void ** pMsg, OCTET * rawBuff, DWORD size);
	int Q931Encode(void ** pMsg, OCTET * rawBuff);
	//ULONG asnsize = RasAsn1DecodeBuf(RASMESSAGE, (void **)&ras, rxBuff, rxSize);
};
