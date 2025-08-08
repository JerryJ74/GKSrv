#pragma once

#include "CallSignalServerSocket.h"



// CCallSiganlServerThread

class CCallSignalServerThread : public CWinThread
{
	DECLARE_DYNCREATE(CCallSignalServerThread)

protected:
	CCallSignalServerThread();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CCallSignalServerThread();
	CCallSignalServerSocket m_serversocket;
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
};


