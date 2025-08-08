#include "StdAfx.h"
#include "q931.h"
#include "q931common.h"
#include "cppAsnHeader.h"
//#include "SockDebug.h"
#include "MessageThread.h"
#include "H323EnDecoder.h"


Q931Packet::Q931Packet()
{	
	//m_Data = pData;
	m_UserInfo = NULL;
	memset(m_callingparty_num, 0, sizeof(m_callingparty_num));
	memset(m_calledparty_num, 0, sizeof(m_calledparty_num));
	memset(m_calling, 0, sizeof(m_calling));
	memset(m_callid, 0, sizeof(m_callid));
	m_calledparty_offset = 0;
	m_callingparty_offset = 0;
	m_release_reason = 0;
	m_crv = 0;
	//__DEBUG("Q931Packet Class Create\n");
	//CSockDebug::Instance()->RDEBUG("Create Q931 Decoder \n");
}

Q931Packet::~Q931Packet(void)
{
}

void Q931Packet::SetData(OCTET * Data, UINT packetLength, BYTE * TPKT)
{
	m_Data = Data;
	m_packetLength = packetLength;
	m_TPKT = TPKT;
}

bool Q931Packet::Decode()
{
	WORD offset = Q931PACKETHEADERLENGTH;
    H323_UserInformation * pUU = NULL;
	DWORD dwBandwidth = 0;
	Q931PacketHeader * pHeader = (Q931PacketHeader *)m_Data;
	BIGENDIANCOPYWORD(&(m_crv),&(pHeader->wCrv));
	
	//RDEBUG("Start Decode.. \n");
	//__DEBUG("Q931Packet::Decode() Header message type : %d ",pHeader->oMessageType);

	if ( !(pHeader->oMessageType == 5) )
		offset -= 4;

	//__DEBUG("Q931Packet::Decode(), MessageType:%d", pHeader->oMessageType);

	while (offset < m_packetLength) {
			
		//CSockDebug::Instance()->RDEBUG("Q931Packet::Decode() Header offset : %d, m_packetLength : %d", offset, m_packetLength);
		//__DEBUG("Q931Packet::Decode() Header offset : %d, m_packetLength : %d", offset, m_packetLength);	
		OCTET oIE = *(m_Data + offset);

		if ((oIE & 0x80) != 0)
      		offset++;
		else 
		{
      		if (oIE == Q931IE_UUIE) 
			{
    			WORD wUUIELength;
    			Q931UUIE * pIE = (Q931UUIE *)(m_Data + offset);
      			if (pIE->oAsn1ProtDiscr == Q931_ASN1PROTDISCR) 
				{
					DWORD ulAsn1Size;

					// ASN1 decode
					BIGENDIANCOPYWORD(&wUUIELength, &(pIE->wLength));//m_packetLengthgth));
					//ulAsn1Size = Q931Asn1DecodeBuf(H323_USERINFORMATION, (void**)&pUU, m_Data + offset + SIZEOFQ931UUIE, wUUIELength - 1);
					//2003. 10. 01. Q931Asn1DeodeBuf 함수 사용시 여러 Thread에서 동시 호출 함으로인한 문제 발생을 없애기 위해
					//H323EnDecoder 에 캡슐화 하여 호출함
					m_calledh323id_offset = offset + SIZEOFQ931UUIE;

					// I need New off set from uuie('cause i need change the setup in destnation address)
					m_uuie_start_offset = offset;

					ulAsn1Size = H323EnDecoder::Instance()->Q931Decode((void **)&pUU, m_Data + offset + SIZEOFQ931UUIE, wUUIELength - 1);
					// invalidate the decode
			        if ( ((wUUIELength - 1) < (WORD)ulAsn1Size) || (pUU == NULL) ) 
					{
						//__DEBUG("Q931Packet::Decode(), WARNING!! Invalid ASN1 UUIE(msg size:%d, asn size:%d), crv:%d", wUUIELength, ulAsn1Size, m_crv);
						if (pUU != NULL)  
						{
							//__DEBUG("Q931Packet::Decode(), pUU is not NULL, memory leak");
							Asn1Free(pUU);
						}
						return false;
					}
  				}
      			break;
			} 
			else 
			{
    			OCTET oIELength = *(m_Data + offset + 1);
				switch(oIE) {
					case Q931IE_CALLINGPARTYNUMBER:
					{ 
						//CSockDebug::Instance()->RDEBUG("Q931Packet::Decode()\t CALLING PARTY DECODE\r\n");
						BOOL bOctet3a = FALSE;
						Q931CallingPartyNumberIE * pIE = (Q931CallingPartyNumberIE *)(m_Data + offset);
						if (!(pIE->oNumberPlan & 0x80))
							bOctet3a=TRUE;
					
						if (pIE->oLength > sizeof(m_callingparty_num)) {
							pIE->oLength = sizeof(m_callingparty_num);
						}
						memcpy(m_callingparty_num, ((OCTET*)(pIE+1))+(bOctet3a?1:0), pIE->oLength-(bOctet3a?2:1));
						m_callingparty_offset = offset + 1;
						break;
					}
			    
					case Q931IE_CALLEDPARTYNUMBER:
					{ 
						//CSockDebug::Instance()->RDEBUG("Q931Packet::Decode()\t CALLED PARTY DECODE\r\n");
						BOOL bOctet3a = FALSE;
						Q931CalledPartyNumberIE * pIE= (Q931CalledPartyNumberIE *)(m_Data + offset);
						if (!(pIE->oNumberPlan & 0x80))
							bOctet3a=TRUE;

						if (pIE->oLength > sizeof(m_calledparty_num)) 
							pIE->oLength = sizeof(m_calledparty_num);
						//memcpy(m_calledparty_num, pIE+1, pIE->oLength - 1);
						memcpy(m_calledparty_num, ((OCTET*)(pIE+1))+(bOctet3a?1:0), pIE->oLength-(bOctet3a?2:1));
						m_calledparty_offset = offset + 1;
						// + 2 + oIELength;
						break;
					}
			    
					case Q932IE_FACILITY:
						//__DEBUG("Q931Packet::Decode()\t FACILITY\r\n");
						break;
					case Q931IE_CAUSE:
					{ 
						//__DEBUG("Q931Packet::Decode() Q931IE_CAUSE");
						Q931CauseIE * pIE = (Q931CauseIE *)(m_Data + offset);
						m_release_cause = pIE->oCause - 128;
						//int b;
						break;
					}
				} // switch
				offset += 2 + oIELength;
			} // if
		} // if
	} // while
	
	CallIdentifier * callid = NULL;
	//m_calledh323id_offset = offset + SIZEOFQ931UUIE;

	//__DEBUG("Q931Packet::Decode()\t END Decode\r\n");
	//char appmsg[256] = "";


	switch(pHeader->oMessageType)
	{
	
	case Q931MSG_SETUP:
	{
		if (pUU->h323.h323.oChoice == H323_STP)
			callid = &(pUU->h323.h323._stp.callid);

		AliasAddress * addr = pUU->h323.h323._stp.srcadrs.palsadrs;
		DecodeCalledInfo(addr, pUU->h323.h323._stp.srcadrs.wSize);

		m_packetType = Q931PacketType::e_Q931Setup;

		__APPMSG(APPMSG_SETUP, "SETUP|%s|%s|%s|%d|;", m_calling, m_callingparty_num, m_calledparty_num, m_crv);

		break;
	}
	case Q931MSG_ALERTING:
		//__DEBUG("Q931Packet::Decode()\t Q931MSG_ALERTING\r\n");
		m_packetType = Q931PacketType::e_Q931Alerting;
		break;
	case Q931MSG_CALLPROCEEDING:
		//__DEBUG("Q931Packet::Decode()\t Q931MSG_CALLPROCEEDING\r\n");
		m_packetType = Q931PacketType::e_Q931Callproceeding;
		break;
	case Q931MSG_CONNECT:
		//__DEBUG("Q931Packet::Decode()\t Q931MSG_CONNECT\r\n");
		m_crv = m_crv - 0x8000;
		if (pUU && pUU->h323.h323.oChoice == H323_CNCT) 
			callid = &(pUU->h323.h323._cnct.callid);
		m_packetType = Q931PacketType::e_Q931Connect;

		__APPMSG(APPMSG_CONNECT, "CONNECT|%d|;", m_crv);
		break;
	case Q931MSG_RELEASECOMPLETE:
	{
		//__DEBUG("Q931Packet::Decode()\t Q931MSG_RELEASECOMPLETE\r\n");
		if (pUU->h323.h323.oChoice == H323_RLSCOMPL)
			callid = &(pUU->h323.h323._rlscompl.callid);
		m_packetType = Q931PacketType::e_Q931ReleaseComplete;
		if (pUU->h323.h323._rlscompl.bRsnOptional)
		{
			m_release_reason = pUU->h323.h323._rlscompl.rsn.oChoice;
			//pUU->h323.h323._rlscompl.rsn._
		}
		break;
	}
	case Q931MSG_STATUSENQUIRY:
		break;
	case Q931MSG_INFORMATION:
		m_packetType = e_Q931Information;
		break;
	case Q931MSG_PROGRESS:
		break;
	case Q932MSG_FACILITY:
		/* facility message내부에 h.245 주소를 강제로 할당함
		if (pUU->h323.h323._fac.bH245aOptional)
		{
			DWORD t_ip = inet_addr("211.240.106.232");
			memcpy(m_Data + 43, &t_ip, sizeof(t_ip));
		}
		*/
		m_packetType = Q931PacketType::e_Q931Facility;
		break;
	case 0x20 : 
		// 0x20 코드는 UserInfo 코드 이다. 하지만 넷터지 스택이 UserInfo에 대한 정의가 없다
		// 그래서 0x20 코드를 강제로 등록 하여 UserInfo가 오면 그대로 상대측 GW에 전송한다.
		m_packetType = e_Q931Information;
		break;
	case Q931MSG_STATUS :
	{
		m_packetType = e_Q931Status;
		break;
	}
	default:
		//__DEBUG("Q931Packet::Decode(), Q931MSG_UNKNONW");
		m_packetType = Q931PacketType::e_Q931Unknow;
	}

	if (callid)
	{
		sprintf(m_callid, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
        callid->guid.po[0], callid->guid.po[1], callid->guid.po[2], callid->guid.po[3], 
		callid->guid.po[4], callid->guid.po[5], callid->guid.po[6], callid->guid.po[7], 
		callid->guid.po[8], callid->guid.po[9], callid->guid.po[10], callid->guid.po[11], 
		callid->guid.po[12], callid->guid.po[13], callid->guid.po[14], callid->guid.po[15]);
	}

	if (pUU)
		Asn1Free(pUU);
	
	if (m_packetType != Q931PacketType::e_Q931Unknow)
		return true;
	else
	{
		//__DEBUG("Q931Packet::Decode(), Decode FAIL!! High Priority WARNING!");
		return false;
	}
}

int Q931Packet::SetCalledPartyNum(char * parsed_destnum)
{
	if (m_calledparty_offset == 0)
		return 0;

	// memory move 
	// a0               a1        a2      a3
	// +----------------+--(000)----+--------+
    // a1 부터 a2 까지 setup 메시지의 calling num 부분이다.

	// b0               b1        b2      b3
	// +----------------+--(1111)----+--------+
    // b1 부터 b2 까지가 바뀌어질 메모리 부분이다.
    
	// a1 에서 a2 구간의 메모리를 b1 에서 b2 까지의 새로운 값으로 변경 해야 한다.
	// tmpBuff 가 b구역의 메모리 이고 m_Data가 a구역의 메모리 이다.

	int src_offset = m_calledparty_offset;
	OCTET tmpBuff[1024];
	memset(tmpBuff, 0, sizeof(tmpBuff));
	memcpy(tmpBuff, m_Data, m_calledparty_offset);
	
	// size
	int length = (int)*(m_Data + m_calledparty_offset);
	int destnum_length =  (int)strlen(parsed_destnum);
	*(tmpBuff + m_calledparty_offset) = destnum_length + 1;//length + 3;
	m_calledparty_offset += 1;

	// 1 byte copy
	*(tmpBuff + m_calledparty_offset) = 0xA1;
	m_calledparty_offset++;

	// copy parsed_destnum
	memcpy(tmpBuff + m_calledparty_offset, parsed_destnum, destnum_length);
	m_calledparty_offset = m_calledparty_offset + destnum_length;

	memcpy(tmpBuff + m_calledparty_offset, m_Data + src_offset + length + 1, m_packetLength - src_offset - length);

	m_TPKT[3] = m_TPKT[3] + (destnum_length - length) + 1;

	m_packetLength = m_packetLength + (destnum_length - length) + 1;
	memset(m_Data, 0, 1024);
	memcpy(m_Data, tmpBuff, m_packetLength);

	m_calledparty_offset = 0;


	return m_packetLength;
	// New Code from MCube Telecom. ******************************************************************
	// 2006. 04. 6.
	/*
	int called_party_num_size = (int)strlen(m_calledparty_num); 

	if (called_party_num_size != destnum_length)
		m_uuie_start_offset = m_uuie_start_offset - (called_party_num_size - destnum_length);

	Q931UUIE * pIE = (Q931UUIE *)(m_Data + m_uuie_start_offset);

	WORD wUUIELength;
	DWORD ulAsn1Size;

	BIGENDIANCOPYWORD(&wUUIELength, &(pIE->wLength));//m_packetLengthgth));

	//pUU->h323.h323._stp.destadrs.wSize
	//m_calledh323id_offset = offset + SIZEOFQ931UUIE;
    H323_UserInformation * pUU = NULL;
	ulAsn1Size = H323EnDecoder::Instance()->Q931Decode((void **)&pUU, m_Data + m_uuie_start_offset + SIZEOFQ931UUIE, wUUIELength - 1);

	Asn1Free(pUU->h323.h323._stp.destadrs.palsadrs->_dlddgts.po);
	//pUU->h323.h323._stp.destadrs.palsadrs->_dlddgts.po = (OCTET *)malloc(destnum_length);
	pUU->h323.h323._stp.destadrs.palsadrs->_dlddgts.po = (OCTET *)Asn1Malloc(destnum_length)  ;

	pUU->h323.h323._stp.destadrs.palsadrs->_dlddgts.wSize = destnum_length;
	CopyToE164AliasAddress((char *)(pUU->h323.h323._stp.destadrs.palsadrs->_dlddgts.po), parsed_destnum); 

	OCTET m_newData[1024];
	memset(m_newData, 0, sizeof(m_newData));

	int new_size = H323EnDecoder::Instance()->Q931Encode((void **)pUU, m_newData);
	int header_size = m_packetLength - ulAsn1Size;

	pIE->wLength2 = new_size;

	memset(tmpBuff, 0, sizeof(tmpBuff));
	
	// copy 
	memcpy(tmpBuff, m_Data, header_size);
	memcpy(tmpBuff + header_size, m_newData, new_size);
	
	memset(m_Data, 0, sizeof(m_Data));
	memcpy(m_Data, tmpBuff, sizeof(tmpBuff));

	Asn1Free(pUU->h323.h323._stp.destadrs.palsadrs->_dlddgts.po);

	if (pUU)
		Asn1Free(pUU);

	m_packetLength = new_size + header_size;
	//m_packetLength = m_packetLength - (called_party_num_size - destnum_length + 1);
	//***********************************************************************************************
	*/
}



int Q931Packet::SetCallingPartyNum(char * parsed_destnum)
{

	size_t b = strlen(m_callingparty_num); 
	
	if (7 < (b-1))
		return m_packetLength;

	// memory move 
	// a0               a1        a2      a3
	// +----------------+--(000)----+--------+
    // a1 부터 a2 까지 setup 메시지의 calling num 부분이다.

	// b0               b1        b2      b3
	// +----------------+--(1111)----+--------+
    // b1 부터 b2 까지가 바뀌어질 메모리 부분이다.
    
	// a1 에서 a2 구간의 메모리를 b1 에서 b2 까지의 새로운 값으로 변경 해야 한다.
	// tmpBuff 가 b구역의 메모리 이고 m_Data가 a구역의 메모리 이다.


	// calling party number가 없을 경우..
	
	
	int src_offset = m_callingparty_offset;
	OCTET tmpBuff[1024];
	memset(tmpBuff, 0, sizeof(tmpBuff));
	memcpy(tmpBuff, m_Data, m_callingparty_offset);

	// size
	int length = (int)*(m_Data + m_callingparty_offset);
	int destnum_length =  (int)strlen(parsed_destnum);
	*(tmpBuff + m_callingparty_offset) = destnum_length + 1;//length + 3;
	m_callingparty_offset += 1;
	/* */
	

	// 1 byte copy
	*(tmpBuff + m_callingparty_offset) = 0xA1;
	m_callingparty_offset++;

	// copy parsed_destnum
	memcpy(tmpBuff + m_callingparty_offset, parsed_destnum, destnum_length);
	m_callingparty_offset = m_callingparty_offset + destnum_length;

	memcpy(tmpBuff + m_callingparty_offset, m_Data + src_offset + length + 1, m_packetLength - src_offset - length);

	if ( (destnum_length + 1) < length )
	{
		m_packetLength = m_packetLength + ( length - (destnum_length + 1 ) );
	}
	else
	{
		m_packetLength = m_packetLength - ( length - (destnum_length + 1) );
	}
	
	m_TPKT[3] = m_packetLength; //m_TPKT[3] + (destnum_length - length) + 1;

	//m_packetLength = m_packetLength + (destnum_length - length) + 1;
	memset(m_Data, 0, 1024);
	memcpy(m_Data, tmpBuff, m_packetLength);

	m_callingparty_offset = 0;

	return m_packetLength;
}


int Q931Packet::SetCalledH323ID(char * KTString)
{
	if (m_calledh323id_offset == 0)
		return 0;
	
	OCTET tmpBuff[1024] = "";
	OCTET headBuff[1024] = "";
	OCTET tailBuff[1024] = "";
	OCTET replaceBuff[1024] = "";

	// KT 의 경우 ADD Prefix가 없기 때문에 offset이 218(보안프릭스 3자리) 가
	// 붙어서 들어온다. 이를 해결 하기 위해서 KT 연동일 경우 오프셋 -3 을 한다.
	m_calledh323id_offset = m_calledh323id_offset + 10;

	// source address가 있기 이전 가지의 값을 가지고 있는데.
	memcpy(headBuff, m_Data, m_calledh323id_offset);
	int head_length = m_calledh323id_offset;
	headBuff[head_length - 1] = 0x01;

	int head_start_offset = m_calledh323id_offset + 1;

	// Setup의 UUIE Field 첫번째 E.164 or H323ID Size
	// 만약 5byte 이면 message 내부에 size는 -1 된값 4가 넘어 오기 때문에 ++을 해 준다.
	int size_1, size_2 = 0;
	int temp_1, temp_2 = 0;

	temp_1 = (int)*(m_Data + head_start_offset - 1);
	temp_2 = (int)*(m_Data + head_start_offset + 0);
	if ( (temp_1 == 0x40) || (temp_1 == 0x80) )
	{
		size_1 = temp_2 + 1;
		if (temp_1 == 0x40) size_1 = (temp_2 + 1) * 2;
	}
	else
	{
		size_1 = temp_1 + 1;
		if (temp_2 == 0x40) size_1 = (temp_1 + 1) * 2;
	}

    temp_1 = (int)*(m_Data + head_start_offset + size_1 + 1);
	temp_2 = (int)*(m_Data + head_start_offset + size_1 + 2);
	if ( (temp_1 == 0x40) || (temp_1 == 0x80) )
	{
		size_2 = temp_2 + 1;
		if (temp_1 == 0x40) size_2 = (temp_2 + 1) * 2;
	}
	else
	{
		size_2 = temp_1 + 1;
		if (temp_2 == 0x40) size_2 = (temp_1 + 1) * 2;
	}

	int del_length = size_1 + size_2 + 4;
	if (temp_2 == 0x40) del_length = del_length - 2;

	int tail_length = m_packetLength - (head_length + del_length);
	memcpy(tailBuff, m_Data + head_length + del_length, tail_length);

	//ktstr[0] 는 H323ID 임을 나타내는 16진수 0x40이 들어가고
	//ktstr[1] 은 H323ID string 길이에 -1 을 한다.
    OCTET ktstr[39] = "";
	ktstr[0] = 0x40;
	ktstr[1] = 0x11;
	memcpy((char *)(ktstr+2), "\0i\0n\0t\0e\0r\0g\0k\0@\0m\0o\0o\0h\0a\0n\0.\0n\0e\0t", 36);

	int ktstr_len = 38;
	//tmpBuff에다 Head, ktstr, Tail을 연결해 Setup 메시지를 완성한다.
	memcpy(tmpBuff, headBuff, head_length); 
	memcpy(tmpBuff + head_length, ktstr, ktstr_len);
	memcpy(tmpBuff + head_length + ktstr_len, tailBuff, tail_length);

	memset(m_Data, 0, 1024);
	memcpy(m_Data, tmpBuff, head_length + ktstr_len + tail_length);

	m_packetLength = head_length + tail_length + ktstr_len;

	return m_packetLength;
}

void Q931Packet::DecodeCalledInfo(AliasAddress * addrs, WORD size)
{
	for(int i = 0; i < size; i++)
	{
		switch(addrs->oChoice)
		{
			case ALSADRS_DLDDGTS:
				memset(m_callingparty_num, 0, sizeof(m_callingparty_num));
				if (addrs->_dlddgts.wSize > 0)
					ExtractE164FromAliasAddress(m_callingparty_num, addrs->_dlddgts.po, addrs->_dlddgts.wSize);
				break;
			case ALSADRS_H323ID:
			{
				memset(m_calling, 0, sizeof(m_calling));
				UINT c = 0;
				while(c < addrs->_h323id.wSize)
				{	
					if (*addrs->_h323id.pw != 0)
					{
						m_calling[c] = (char )*addrs->_h323id.pw;
						c++;	
					}
					addrs->_h323id.pw++;
				}
			}
			break;
		}
		addrs++;
	}
}
