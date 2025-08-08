#include "util.h"
#include "stdafx.h"
#include "cppAsnHeader.h"
#include "asn1v4.h"
#include "GKConst.h"

WORD * CopyCharToWord(RasMessage * ras, char * pchSrc, size_t wLen)
{
	WORD * pwDest = NULL;
	if (wLen > 0 && pchSrc)
	{
		pwDest = (WORD *)RasAsn1Attach(ras, RasAsn1Malloc((int)wLen * 2));
	 	for (int i = 0; i < (int)wLen; i++)
		{
		  pwDest[i] = (WORD)(pchSrc[i]);
		}
	}
	return pwDest;
};

int CopyCharToWord(WORD * pwDest, char * pchSrc, size_t wLen)
{
	if (wLen > 0 && pchSrc)
	{
	 	for (int i = 0; i < (int)wLen; i++)
		{
		  pwDest[i] = (WORD)(pchSrc[i]);
		}
	}
	return 0;
	// n_itoa(
};

void AllocateProtocolID(ProtocolIdentifier * prid)
{
	//
	prid->po = (OCTET *)malloc(6);
	prid->wSize = 6;
	memcpy(prid->po, PROTOCOL_ID, prid->wSize);

}

//////////////////////////////////////////////////////////////////////////////////////////////
// Fills in the IP address into Transport Address data structure
//////////////////////////////////////////////////////////////////////////////////////////////
void FillTransportAddr(TransportAddress * pTransAddr, DWORD dwIPAddr, WORD wPort)
{
	pTransAddr->oChoice = TRADRS_IPADRS;
	pTransAddr->_ipadrs.ip.wSize = (WORD)4;

	COPY_H_IPV4ADDR_TO_N_CHS(pTransAddr->_ipadrs.ip.po, dwIPAddr);

	//*(pTransAddr->_ipadrs.ip.po)	   = 218;//(OCTET)dwIPAddr >> 24;
	//*((pTransAddr->_ipadrs.ip.po) + 1) = 38;//(OCTET)(0xFF & (dwIPAddr >> 16));
	//*((pTransAddr->_ipadrs.ip.po) + 2) = 24;//(OCTET)(0xFF & (dwIPAddr >> 8));
	//*((pTransAddr->_ipadrs.ip.po) + 3) = 81;//(OCTET)(0xFF & dwIPAddr);
	pTransAddr->_ipadrs.wPort = wPort;
}


unsigned short IsMatchedPrefix(char * e164, char * prefix)
{
	char * d = strstr(e164, prefix);

	int p = (int)(d - e164 + 1);
	
	if (p == 1) 
		return (unsigned short)strlen(prefix);
	else
		return 0;
};



void ExtractE164FromAliasAddress(char * pchDest, OCTET * poSourceE164, WORD wLength)
{
	WORD wCnt;
	for (wCnt = 0; wCnt<wLength;wCnt++) {
		if (poSourceE164[wCnt] == 0) {
			pchDest[wCnt] = '#';
		} else if (poSourceE164[wCnt] == 1) {
			pchDest[wCnt] = '*';
		} else if (poSourceE164[wCnt] == 2) {
			pchDest[wCnt] = ',';
		} else if (poSourceE164[wCnt] >= 3) {
			pchDest[wCnt] = '0' + poSourceE164[wCnt] - 3;
		} 
	}
}


void CopyToE164AliasAddress(char * pchDest, char * pchSource)
{
	BYTE iLen = (BYTE)strlen(pchSource);

	for (size_t i = 0; i < iLen; i++) {
		if (pchSource[i] == '#') {
			pchDest[i] = 0;
		} else if (pchSource[i] == '*') {
			pchDest[i] = 1;
		} else if (pchSource[i] == ',') {
			pchDest[i] = 2;
		} else if (pchSource[i] >= '0' && pchSource[i] <= '9') {
			pchDest[i] = pchSource[i] - '0' + 3;
		} else {
			pchDest[i] = 0;
		}
	}
}
