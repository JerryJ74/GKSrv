// CallSiganlServerThread.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "InterGK.h"
#include "CallSignalServerThread.h"


// CCallSiganlServerThread

IMPLEMENT_DYNCREATE(CCallSignalServerThread, CWinThread)

CCallSignalServerThread::CCallSignalServerThread()
{
}

CCallSignalServerThread::~CCallSignalServerThread()
{
	//if (m_serversocket)
}

BOOL CCallSignalServerThread::InitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	return TRUE;
}

int CCallSignalServerThread::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
	//m_serversocket.Close();
	m_serversocket.ShutDown();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCallSignalServerThread, CWinThread)
END_MESSAGE_MAP()


// CCallSiganlServerThread 메시지 처리기입니다.
