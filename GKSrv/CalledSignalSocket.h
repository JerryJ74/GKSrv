#pragma once

//#include "CallingSignalSocket.h"
#include "GKConst.h"
#include "util.h"
#include "Q931.h"
// CCalledSignalSocket 명령 대상입니다.

class CCalledSignalSocket : public CSocket
{
public:
	CCalledSignalSocket(LPCALL_INFO call_info);
	virtual ~CCalledSignalSocket();
	void SetCallingSignalSocket(CSocket * calling) { m_calling = calling; }
	void SetThread(DWORD thread) { m_nThread = thread; }
	bool GetActive() { return m_bActive; }
	void SetActive(bool stats) { m_bActive = stats; };
protected:
	CSocket * m_calling;
	DWORD m_nThread;
	bool m_rxTPKT;
	BYTE m_TPKT[4];
	UINT m_packetLength;
	Q931Packet m_q931;
	LPCALL_INFO m_call_info;
	OCTET m_rxBuff[1024];
	bool m_bActive;

public:
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
};