// CallSiganlServerThread.cpp : ���� �����Դϴ�.
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
	// TODO: ���⿡�� �� �����忡 ���� �ʱ�ȭ�� �����մϴ�.
	return TRUE;
}

int CCallSignalServerThread::ExitInstance()
{
	// TODO: ���⿡�� �� �����忡 ���� ������ �����մϴ�.
	//m_serversocket.Close();
	m_serversocket.ShutDown();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCallSignalServerThread, CWinThread)
END_MESSAGE_MAP()


// CCallSiganlServerThread �޽��� ó�����Դϴ�.
