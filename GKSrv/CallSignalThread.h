#pragma once

#include "util.h"
#include "GKConst.h"
#include "CallingSignalSocket.h"
#include "CalledSignalSocket.h"

// CCallSignalThread

class CCallSignalThread : public CWinThread
{
	DECLARE_DYNCREATE(CCallSignalThread)

protected:
	CCallSignalThread();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CCallSignalThread();

	CALL_STATS m_callstat;

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void SetCallingSocket(SOCKET s) { m_socket = s; }


protected:
	CCallingSignalSocket m_calling;
	CCalledSignalSocket m_called;

	SOCKET m_socket;
	CALL_INFO m_call_info;

	void OnSetup();
	void OnTimer();
	void OnTimeOut();
	DWORD m_delTime;
	BYTE m_timeout;
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG * pMsg);
};
