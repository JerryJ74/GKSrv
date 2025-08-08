#pragma once

#include "CallSignalServerSocket.h"



// CCallSiganlServerThread

class CCallSignalServerThread : public CWinThread
{
	DECLARE_DYNCREATE(CCallSignalServerThread)

protected:
	CCallSignalServerThread();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CCallSignalServerThread();
	CCallSignalServerSocket m_serversocket;
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
};


