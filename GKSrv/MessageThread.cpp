// MessageThread.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
//#include "InterGK.h"
#include "MessageThread.h"

//#include "GKConfig.h"


// CMessageThread

IMPLEMENT_DYNCREATE(CMessageThread, CWinThread)

/************************************************************************
-----------------------------------------------------------------------
message definition
-----------------------------------------------------------------------
ARQ->ACF|calling_id|calling_num|called_id|called_num|Answer or Request|
ARQ->ARJ|calling_id|calling_num|called_id|called_num|Reject Reason|
RRQ->RCF|h323id|ipaddress|ep type|
RRQ->RRJ|h323id|ipaddress|reject reason|
DRQ->DCF|calling_id|calling_num|called_id+called_num|
IRR|calling_id|called_num|
CONNECT|calling_id|calling_num|called_id|called_num|
RELECMP|calling_id|calling_num|called_id|called_num|reason|
SETUP|calling_id|calling_num|called_id|called_num|
************************************************************************/

CMessageThread * CMessageThread::m_instance = NULL;


CMessageThread::CMessageThread()
{
	m_instance = this;
	m_msgsock = new CAsyncSocket();
	//m_msgsock->Create(2020, SOCK_DGRAM, FD_WRITE , NULL);
	m_msgsock->Create(0, SOCK_DGRAM, FD_WRITE, NULL);
}

CMessageThread::~CMessageThread()
{
	//m_msgsock->ShutDown();
	//delete m_msgsock;
}

/*
void CMessageThread::SendMsg(LPAPP_MESSAGE msg)
{
	PostThreadMessage(WM_MSG_APPSERVER, NULL, LPARAM(msg));
}
*/

BOOL CMessageThread::InitInstance()
{
	// TODO: ���⿡�� �� �����忡 ���� �ʱ�ȭ�� �����մϴ�.
	return TRUE;
}

int CMessageThread::ExitInstance()
{
	// TODO: ���⿡�� �� �����忡 ���� ������ �����մϴ�.
	m_msgsock->ShutDown();
	delete m_msgsock;
	m_msgsock = NULL;
	return CWinThread::ExitInstance();
}

CMessageThread * CMessageThread::Instance()
{
	return m_instance;
}

BEGIN_MESSAGE_MAP(CMessageThread, CWinThread)
END_MESSAGE_MAP()


// CMessageThread �޽��� ó�����Դϴ�.


BOOL CMessageThread::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if (pMsg->message == WM_MSG_APPSERVER)
	{
		LPAPP_MESSAGE msg = (LPAPP_MESSAGE)(pMsg->lParam);
		SendMsg(msg);
		delete msg;
	}
	return CWinThread::PreTranslateMessage(pMsg);
}

void CMessageThread::SendAppMessage(LPAPP_MESSAGE msg)
{
	PostThreadMessage(WM_MSG_APPSERVER, NULL, LPARAM(msg));
}

void CMessageThread::SendMsg(LPAPP_MESSAGE msg)
{
	bool bSend = false;

	switch(msg->type)
	{
		case APPMSG_ARQ :
			if (APPMSG_ARQ_SEND) bSend = true;	
			break;
		case APPMSG_LRQ	:
			break;
		case APPMSG_URQ	:
			break;
		case APPMSG_IRR	:
			if (APPMSG_IRR_SEND) bSend = true;
			break;
		case APPMSG_RRQ	:
			if (APPMSG_RRQ_SEND) bSend = true;
			break;
		case APPMSG_SETUP :
			if (APPMSG_SETUP_SEND) bSend = true;
			break;
		case APPMSG_CONNECT :
			if (APPMSG_CNT_SEND) bSend = true;
			break;
		case APPMSG_RELECOMP :
			break;
		case APPMSG_ENDCALL	:
			bSend = true;
	}

	if (bSend)
	{
		int len = (int)strlen(msg->message);
		m_msgsock->SendTo(msg->message, len, MESSAGE_PORT, MESSAGE_ADDR, 0);
	}
}
