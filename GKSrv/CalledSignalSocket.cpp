// CalledSignalSocket.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
//#include "InterGK.h"
#include "CalledSignalSocket.h"
#include "DBWorkThread.h"
//#include "MessageThread.h"

// CCalledSignalSocket

CCalledSignalSocket::CCalledSignalSocket(LPCALL_INFO call_info)
{
	m_call_info = call_info;
	m_rxTPKT = false;
	m_packetLength = 0;
	m_bActive = false;
}

CCalledSignalSocket::~CCalledSignalSocket()
{
}

// CCalledSignalSocket ��� �Լ��Դϴ�.

void CCalledSignalSocket::OnConnect(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	// __DEBUG("Called Socket Connected, m_bActive = true");
	m_bActive = true;
	CSocket::OnConnect(nErrorCode);
}

void CCalledSignalSocket::OnClose(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	// __DEBUG("Called Socket Closed, m_bActive = false");
	m_bActive = false;
	CSocket::OnClose(nErrorCode);
}

void CCalledSignalSocket::OnReceive(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	//temp code
	//CWinThread * thd = ::AfxGetThread();
	//__APPMSG(APPMSG_ENDCALL, "CalledSignalSocket OnRecevie, thread : %d", thd->m_nThreadID);
	//end of temp code

	if (m_rxTPKT == false)
	{
		memset(m_TPKT, 0, sizeof(m_TPKT));
		int ll = Receive((char *)m_TPKT, sizeof(m_TPKT));
		
		//__APPMSG(APPMSG_ENDCALL, "CalledSignalSocket::OnReceive(), TPKT rx size : %d", ll);
		m_packetLength = ((m_TPKT[2] << 8)|m_TPKT[3]) - 4;

		if (m_packetLength < 5) 
    		return;
		m_rxTPKT = true;
		return;
	}

	if (m_rxTPKT && (m_packetLength > 0))
	{
		memset(m_rxBuff, 0, sizeof(m_rxBuff));
		int rx_size = Receive((char *)m_rxBuff, m_packetLength);
		//__APPMSG(APPMSG_ENDCALL, "CalledSignalSocket::OnReceive(), rx size : %d", rx_size);

		// TPKT�ǿ��� Packet size �� ���� rx �� size�� �ٸ� ��� Return �Ѵ�.
		if (rx_size != m_packetLength)
		{
			//__DEBUG("Called..Socket::OnReceive(), WARNING!!!, rx size:%d, tpkt size:%d, Close Call Thread", rx_size, m_packetLength);
			::PostThreadMessage(m_nThread, WM_QUIT, 0, 0);					
			return CSocket::OnReceive(nErrorCode);
		}

		m_q931.SetData(m_rxBuff, m_packetLength, m_TPKT);
		m_rxTPKT = false;

		if (m_q931.Decode())
		{
			switch(m_q931.GetQ931Type())
			{
			case Q931Packet::Q931PacketType::e_Q931Setup:
				m_call_info->crv = m_q931.GetCRV();
				strcpy(m_call_info->call_id, m_q931.GetCallID());
				strcpy(m_call_info->dest_num, m_q931.GetCalledNum());
				strcpy(m_call_info->src_num, m_q931.GetCallingNum());
				strcpy(m_call_info->calling, m_q931.GetCalling());
				m_call_info->call_stat = e_Q931_Setup_ToCalled;
				CDBWorkThread::Instance()->RxSetup(&m_nThread, (void *)m_call_info);
				break;
			case Q931Packet::Q931PacketType::e_Q931Connect:
				m_call_info->connect = true;
				m_call_info->call_stat = e_Q931_Connect;
				
				//m_call_info->connect_t

				CDBWorkThread::Instance()->RxConnect(&m_nThread, (void *)m_call_info);
				break;
			case Q931Packet::Q931PacketType::e_Q931ReleaseComplete:
				{
					m_call_info->call_stat = e_Q931_ReleaseComplete_From_Called;
					m_call_info->release_complete = m_q931.GetReleaseReason();
					m_call_info->release_cause = m_q931.GetCause();

					// 2004. 11. 24. 
					// CallSignal Thread�� ����� �Բ� m_call_info�� �޸𸮿��� �������.
					// �Ͽ� DBWork Thread����ó���� �ʾ����� �޸� ���� ������ �߻��Ѵ�.
					// �̸� ���� ���Ͽ� call_info�� ���� ���� �Ͽ� DBWorkThread�� ������.
					// �̶� DBWorkThread������ ReleaseComplete(..) �Լ��� ó�� �ϰ� call_info�� �����Ͽ����Ѵ�.
					if (m_call_info->call_stat != e_Q931_ReleaseComplete_From_Calling)
					{
						LPCALL_INFO call_info;
						call_info = new CALL_INFO;
						memcpy(call_info, (const void *)m_call_info, sizeof(CALL_INFO));
						CDBWorkThread::Instance()->RxReleaseComplete(&m_nThread, (void *)call_info);
					}
					break;
				}
			case Q931Packet::Q931PacketType::e_Q931Alerting:
				break;
			case Q931Packet::Q931PacketType::e_Q931Callproceeding:
				break;
			case Q931Packet::Q931PacketType::e_Q931Facility:
				break;
			case Q931Packet::Q931PacketType::e_Q931Unknow:
				break;
			}

			if (m_q931.GetQ931Type() != Q931Packet::Q931PacketType::e_Q931Setup)
			{
				// 2004. 04. 09. 
				// Connect �޽����� �޾Ƽ� Calling Socket���� �����Ҷ� 
				// Delay���� �ٷ� ���۽� Connect�� ������ ������ �߻���.
				// �̿� Connect �޽��� �϶��� 500ms�� �ð��� ������.
				int s_size = m_calling->Send(m_TPKT, 4, 0);
				if ( (m_q931.GetQ931Type() == Q931Packet::e_Q931Connect) || (m_q931.GetQ931Type() == Q931Packet::e_Q931Callproceeding) )
					Sleep(300);
				s_size = m_calling->Send(m_rxBuff, m_packetLength, 0);// + 4, 0);
			}
		}
		else // Q.931 Decode Fail
		{
			//__DEBUG("CalledSignalSocket::OnReceive(), WARNING!! Decode Fail");
			::PostThreadMessage(m_nThread, WM_QUIT, 0, 0);
			return CSocket::OnReceive(nErrorCode);
		}
	}

	if (m_q931.GetQ931Type() == Q931Packet::Q931PacketType::e_Q931ReleaseComplete)
	{
		::PostThreadMessage(m_nThread, WM_QUIT, 0, 0);
	}

	CSocket::OnReceive(nErrorCode);
}
