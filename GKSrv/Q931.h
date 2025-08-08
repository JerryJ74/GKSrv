#pragma once

#include "util.h"
#include "GKConst.h"
#include "asn1v4.h"
#include "cppAsnHeader.h"


class Q931Packet
{
public:
	Q931Packet();
	~Q931Packet(void);

	void SetData(OCTET * Data, UINT packetLength, BYTE * TPKT);
	bool Decode();
	WORD GetCRV() { return m_crv; }
	char * GetCalledNum() { return m_calledparty_num; }
	char * GetCallingNum() { return m_callingparty_num; }
	char * GetCallID() { return m_callid; }
	char * GetCalling() { return m_calling; }
	BYTE GetCause() { return m_release_cause; }
	WORD GetReleaseReason() { return m_release_reason; }

	typedef struct Q931ReceivePacketH245Data {};

	enum Q931PacketType
	{
		e_Q931Setup,
		e_Q931Alerting,
		e_Q931Callproceeding,
		e_Q931Connect,
		e_Q931ReleaseComplete,
		e_Q931Facility,
		e_Q931H245Tunnulling,
		e_Q931Information,
		e_Q931Status,
		e_Q931Unknow
	};

	Q931PacketType GetQ931Type() { return m_packetType; }
	Q931PacketType m_packetType;
	int SetCalledPartyNum(char * parsed_destnum);
	int SetCallingPartyNum(char * parsed_destnum);
	int SetCalledH323ID(char * KTString);

protected:
	BYTE m_release_cause;
	WORD m_release_reason;
	WORD m_crv;
	char m_callingparty_num[18];
	char m_calledparty_num[18];
	char m_calling[16];
	char m_callid[32];
	OCTET * m_Data;
	H323_UserInformation * m_UserInfo;
	bool CheckIEType() { return true; }
	void DecodeCalledInfo(AliasAddress * addrs, WORD size);
	UINT m_packetLength;
	BYTE * m_TPKT;
	int m_calledparty_offset;
	int m_callingparty_offset;
	int m_calledh323id_offset;
	int m_uuie_start_offset;
};
