// CallSignalServerSocket.cpp : 구현 파일입니다.
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


// CCallSignalServerSocket 멤버 함수입니다.

void CCallSignalServerSocket::OnAccept(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	// 2003. 10. 24
	// CSocket가 동기화 소켓이여서 Accept 하는 과정에서 timeout이 걸려 
	// Signalserver 소켓이 동작 하지 않는 상황으로 추측..
	// CSocket 에서 CAsyncSocket 으로 수정 함.

	//temp code
	//CWinThread * thd = ::AfxGetThread();
	//__APPMSG(APPMSG_ENDCALL, "CallSignalServerSocket OnAccept, thread : %d", thd->m_nThreadID);
	//end of temp code

	//CSocket socket;
	CAsyncSocket socket;
	Accept(socket);

	// 2003. 10. 24
	// Thread의 Priority를 BELOW NORMAL에서 NORMAL로 수정 함.
	CCallSignalThread * call = 
			(CCallSignalThread *)AfxBeginThread(RUNTIME_CLASS(CCallSignalThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	SOCKET s = socket.Detach();
	call->SetCallingSocket(s);
	call->m_bAutoDelete = true;
	call->ResumeThread();

	CAsyncSocket::OnAccept(nErrorCode);
}
