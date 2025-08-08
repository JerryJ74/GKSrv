// CallSignalServerSocket.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "CallSignalServerSocket.h"
#include "CallSignalThread.h"
#include "GKConst.h"
//#include "MessageThread.h"


// CCallSignalServerSocket

CCallSignalServerSocket::CCallSignalServerSocket()
{
	if (Create(SIGNAL_PORT, SOCK_STREAM, FD_ACCEPT, NULL))
		Listen();
	//else
	//	__DEBUG("CCallSignalServerSocket::CCallSignal....(), TCP Server socket initialize fail!!");
}

CCallSignalServerSocket::~CCallSignalServerSocket()
{
	Close();
	ShutDown();
}


// CCallSignalServerSocket ��� �Լ��Դϴ�.

void CCallSignalServerSocket::OnAccept(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	// 2003. 10. 24
	// CSocket�� ����ȭ �����̿��� Accept �ϴ� �������� timeout�� �ɷ� 
	// Signalserver ������ ���� ���� �ʴ� ��Ȳ���� ����..
	// CSocket ���� CAsyncSocket ���� ���� ��.

	//temp code
	//CWinThread * thd = ::AfxGetThread();
	//__APPMSG(APPMSG_ENDCALL, "CallSignalServerSocket OnAccept, thread : %d", thd->m_nThreadID);
	//end of temp code

	//CSocket socket;
	CAsyncSocket socket;
	Accept(socket);

	// 2003. 10. 24
	// Thread�� Priority�� BELOW NORMAL���� NORMAL�� ���� ��.
	CCallSignalThread * call = 
			(CCallSignalThread *)AfxBeginThread(RUNTIME_CLASS(CCallSignalThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	SOCKET s = socket.Detach();
	call->SetCallingSocket(s);
	call->m_bAutoDelete = true;
	call->ResumeThread();

	CAsyncSocket::OnAccept(nErrorCode);
}
