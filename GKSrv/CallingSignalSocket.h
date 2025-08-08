#pragma once

//#include "CalledSignalSocket.h"
#include "util.h"
#include "GKConst.h"
#include "Q931.h"

// CCallingSignalSocket 명령 대상입니다.

class CCallingSignalSocket : public CSocket
{
public:
	CCallingSignalSocket(LPCALL_INFO call_info);
	virtual ~CCallingSignalSocket();
	void SetCalledSignalSocket(CSocket * called) { m_called = called; }
	bool SendToCalled(Q931Packet * q931);
	void SetThread(DWORD thread) { m_nThread = thread; }
	BYTE * GetPacketHeader() { return m_TPKT; }
	char * GetPacketBody() { return (char *)m_rxBuff; }
	UINT GetPacketLength() { return m_packetLength; }
	bool GetActive() { return m_bActive; }
	
	bool OnSetupApplyChangePrefix();
	bool OnSetupApplyChangeCallingNum();

	bool OnSetupChangeCalledH323id();
	WORD GetPacketSize() { return m_packetLength; }
	//Q931Packet GetPacketHeader() { return m_rx
protected:
	CSocket * m_called;
	bool m_rxTPKT;
	BYTE m_TPKT[4];
	WORD m_packetLength;
	Q931Packet m_q931;
	DWORD m_nThread;
	LPCALL_INFO m_call_info;
	OCTET m_rxBuff[1024];
	bool m_bActive;

	//void OnQ931Message();
public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
};
