// CallSignalThread.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
//#include "InterGK.h"
#include "CallSignalThread.h"
#include "DBWorkThread.h"
//#include "DBLogThread.h"

// CCallSignalThread

IMPLEMENT_DYNCREATE(CCallSignalThread, CWinThread)

CCallSignalThread::CCallSignalThread():m_called(&m_call_info), 
									   m_calling(&m_call_info)
{
	AfxSocketInit();
	m_calling.SetCalledSignalSocket((CSocket *)(&m_called));
	m_called.SetCallingSignalSocket((CSocket *)(&m_calling));
	memset(&m_call_info, 0, sizeof(m_call_info));
	m_delTime = 0;
	m_call_info.connect = false;
	// m_call_info.confirm = false;
}

CCallSignalThread::~CCallSignalThread()
{

}

BOOL CCallSignalThread::InitInstance()
{
	// TODO: ���⿡�� �� �����忡 ���� �ʱ�ȭ�� �����մϴ�.
	m_calling.Attach(m_socket);
	m_calling.SetThread(m_nThreadID);
	m_called.SetThread(m_nThreadID);
	
	m_timeout = 0;
	CWinApp * app = AfxGetApp();

	::SetTimer(m_pMainWnd->m_hWnd, m_nThreadID, (IRR_INTERVAL * 1000), (TIMERPROC) NULL);
	return TRUE;
}

int CCallSignalThread::ExitInstance()
{
	// TODO: ���⿡�� �� �����忡 ���� ������ �����մϴ�.
	m_calling.ShutDown();
	m_called.ShutDown();
	::KillTimer(m_pMainWnd->m_hWnd, m_nThreadID);

	::WaitForSingleObject(m_hThread, 1000);

	return CWinThread::ExitInstance();
}


BEGIN_MESSAGE_MAP(CCallSignalThread, CWinThread)
END_MESSAGE_MAP()


// CCallSignalThread �޽��� ó�����Դϴ�.

BOOL CCallSignalThread::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	switch(pMsg->message)
	{
		case WM_Q931_SETUP_RESULT:
			OnSetup();
			break;
		case WM_TIMER:
			OnTimer();
			break;
		case WM_CALLINFO_NOT_FOUND:
			OnTimeOut();
			break;
	}
	return CWinThread::PreTranslateMessage(pMsg);
}


void CCallSignalThread::OnSetup()
{
	int tx_size = 0;
	if (m_call_info.called_ip != "")
	{
		StrTrim(m_call_info.addprefix, " ");
		StrTrim(m_call_info.delprefix, " ");
		//parsing called num = addprefix, delprefix
		if ((strlen(m_call_info.addprefix) != 0) || (strlen(m_call_info.delprefix) != 0))
		{
			m_calling.OnSetupApplyChangePrefix();
		}
		
		// called�� �������� kt.co.�� �߰ߵǸ� kt�� ������ �ʿ��� �޽����� replace �Ѵ�.
		/*
		if (strstr(m_call_info.called, ".kt.co") != NULL)
			int m_pack_length = m_calling.OnSetupChangeCalledH323id();
		*/

/*
	���ѳ� GK 161, 162�� ���� �ڵ��.
	VNC�� 8���� ���� �ڸ����� ��� ���� ��� "012345678"�� �ٲ��ش�.
#if defined(GK_FOR_MH161) || defined(GK_FOR_MH162)
		m_calling.OnSetupApplyChangeCallingNum();
#endif
*/

		m_called.Create();
		if (m_called.Connect(LPCTSTR(m_call_info.called_ip), m_call_info.called_port))
		{
			m_called.SetActive(true);
			BYTE TPKT[4];
			TPKT[3] = BYTE(m_calling.GetPacketSize() + 4);
			TPKT[2] = BYTE((m_calling.GetPacketSize() + 4) >> 8);
			TPKT[1] = 0;
			TPKT[0] = 3;
			tx_size = m_called.Send((char *)TPKT, 4);
			tx_size = m_called.Send(m_calling.GetPacketBody(), m_calling.GetPacketLength());
		}
		else
		{
			m_called.SetActive(false);
		}
	}
	else
	{
		// DBWork Thread ���� Call �� ���� ���� ã�� ���� ���ߴ�.
		//__DEBUG("CCallSignalThread::OnSetup(), Can't find call rec from setup msg");
		m_called.ShutDown();
		m_calling.ShutDown();
		PostThreadMessage(WM_QUIT, 0, 0);
	}
}

void CCallSignalThread::OnTimer()
{
	// 60�� ����  Calling �� Called Socket�� Active ��Ȳ�� üũ �Ѵ�.
	if ( (m_calling.GetActive() == false) || (m_called.GetActive() == false))
	{
		//__DEBUG("CCallSignalThread::OnTimer(), SocketClosed, clling:%s, clled:%s", 
		//u	m_call_info.calling, m_call_info.called);
		OnTimeOut();
		return;
	}

	// Timer Event �� 3���̻� �߻� �Ͽ��� Setup�� ���� �ʾҴ�.
	// Setup�� ���� ������ m_call_info�� ������ ������ �Ǵ� ���� �ϴ�.
	// Timer Event�� 3�� �̻� �̰� Setup �� ���� �ʾҴٸ� Call Thread �� Close �Ѵ�.
	if (m_timeout >= 2)
	{
		if (m_call_info.calling == "") 
		{
			OnTimeOut();
			return;
		}
	}

	// DBWorkThread�� ���� CallThread�� Call ������ ������ 
	// Call Record�� �ִ��� üũ �Ѵ�.
	if (m_timeout >= 4) //4
	{
		// Request Check Call
		LPRAS_IRR irr = new RAS_IRR;
		memset(irr, 0, sizeof(RAS_IRR));
		strcpy(irr->call_id, m_call_info.call_id);
		strcpy(irr->calling, m_call_info.calling);
		irr->crv = m_call_info.crv;
		CDBWorkThread::Instance()->CheckCall(&m_nThreadID, (void *)irr);
		//__DEBUG("CCallSignalThead::OnTimer(), CheckCall(), clling:%s, dest:%s", m_call_info.calling, m_call_info.dest_num);
		m_timeout = 0;
	}

	m_timeout++;
}

void CCallSignalThread::OnTimeOut()
{
	//__DEBUG("CCallSignalThead::OnTimeOut(), calling : %s, dest no : %s", m_call_info.calling, m_call_info.dest_num);
	m_call_info.call_stat = e_TimeOut;
	m_call_info.release_complete = 255;
	m_called.ShutDown();
	m_calling.ShutDown();
	PostThreadMessage(WM_QUIT, 0, 0);
}

