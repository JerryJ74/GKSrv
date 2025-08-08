#include "StdAfx.h"
#include "h323endecoder.h"

H323EnDecoder * H323EnDecoder::m_instance = NULL;

H323EnDecoder::H323EnDecoder(void)
{
	m_instance = this;
	InitializeCriticalSection(&m_critical);
	// m_debugline = 0;
	// CreateDirectory("H323Debugging\\", NULL);
}

H323EnDecoder::~H323EnDecoder(void)
{
	DeleteCriticalSection(&m_critical);
}

H323EnDecoder * H323EnDecoder::Instance()
{
	return m_instance;
}

int H323EnDecoder::RASDecode(void ** pMsg, OCTET * rawBuff, DWORD size)
{
	EnterCriticalSection(&m_critical);

	//_DD("RASDecode, %d[%X], %d[%X], %d[%X]", rawBuff[0], rawBuff[0], rawBuff[1], rawBuff[1], rawBuff[2], rawBuff[2]);

	int result = 0;
	try
	{
		result = RasAsn1DecodeBuf(RASMESSAGE, pMsg, rawBuff, size);
		//_DD("RASDecode, End.., R:%d, HEAP:%d\r\n", result, _heapchk());
		//_DD("RASDecode, End.., R:%d\r\n", result);
	}
	catch (CMemoryException * e)
	{
		e->Delete();
		//_DD("RasDecode, Memory Exception");
	}

	LeaveCriticalSection(&m_critical);

	if (pMsg == NULL) 
		return -1;
	else
		return result;
}

int H323EnDecoder::RASEncode(void * pMsg, OCTET * rawBuff)
{
	EnterCriticalSection(&m_critical);
	//_DD("RASEncode, Start"); 
	int result = 0;
	try
	{
		result = RasAsn1EncodeBuf(RASMESSAGE, pMsg, rawBuff, MAX_RAS_BUFSIZE);
		//_DD("RASEncode, End.., %d[%X], %d[%X], %d[%X], R:%d\r\n", rawBuff[0], rawBuff[0], rawBuff[1], rawBuff[1], rawBuff[2], rawBuff[2], result);
	}
	catch (CMemoryException * e)
	{
		e->Delete();
		//_DD("RasEncode, Memory Exception\r\n");
	}

	//_DD("RASEncode, End.., %d[%X], %d[%X], %d[%X], R:%d\r\n", rawBuff[0], rawBuff[0], rawBuff[1], rawBuff[1], rawBuff[2], rawBuff[2], result);
	LeaveCriticalSection(&m_critical);
	return result;
}

int H323EnDecoder::Q931Decode(void ** pMsg, OCTET * rawBuff, DWORD size)
{
	EnterCriticalSection(&m_critical);

	//_DD("Q931Decode, %d[%X], %d[%X], %d[%X]", rawBuff[0], rawBuff[0], rawBuff[1], rawBuff[1], rawBuff[2], rawBuff[2]);

	int result = Q931Asn1DecodeBuf(H323_USERINFORMATION, pMsg, rawBuff, size);
	
	//_DD("Q931Decode, End.., R:%d\r\n", result);
	
	LeaveCriticalSection(&m_critical);
	return result;
}


int H323EnDecoder::Q931Encode(void ** pMsg, OCTET * rawBuff)
{
	EnterCriticalSection(&m_critical);
	int result = Q931Asn1EncodeBuf(H323_USERINFORMATION, pMsg, rawBuff, MAX_Q931_BUFSIZE);
	LeaveCriticalSection(&m_critical);
	return result;
}

/*
void H323EnDecoder::_DD(char * m, ...)
{
	char buff[512] = "";
	char cnt[50] = "";

	va_list va; 

	va_start(va, m);
	vsprintf(buff, m, (va_list)va);
	va_end(va);

	if (m_debugline == 0) 
	{
		DWORD b = GetTickCount();
		CString f;
		f.Format("H323Debugging\\H323Log%lu.txt", b);
		m_file.Open(f, CFile::modeWrite | CFile::modeCreate | CFile::shareDenyNone);
	}

	strcat(buff, "\r\n");

	sprintf(cnt, "%lu::", GetTickCount());
	m_file.Write(cnt, (UINT)strlen(cnt));
	m_file.Write(buff, (UINT)strlen(buff));
	m_debugline++;

	if (m_debugline > 30000) 
	{
		m_debugline = 0;
		m_file.Close();
	}
}
*/
