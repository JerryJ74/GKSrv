// CalledSignalSocket.cpp : 구현 파일입니다.
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

// CCalledSignalSocket 멤버 함수입니다.

void CCalledSignalSocket::OnConnect(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	// __DEBUG("Called Socket Connected, m_bActive = true");
	m_bActive = true;
	CSocket::OnConnect(nErrorCode);
}

void CCalledSignalSocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	// __DEBUG("Called Socket Closed, m_bActive = false");
	m_bActive = false;
	CSocket::OnClose(nErrorCode);
}

void CCalledSignalSocket::OnReceive(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

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

		// TPKT의에서 Packet size 와 실제 rx 한 size가 다를 경우 Return 한다.
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
					// CallSignal Thread의 종료와 함께 m_call_info는 메모리에서 사라진다.
					// 하여 DBWork Thread에서처리가 늦어지면 메모리 참조 오류가 발생한다.
					// 이를 막기 위하여 call_info를 새로 생성 하여 DBWorkThread로 보낸다.
					// 이때 DBWorkThread에서는 ReleaseComplete(..) 함수를 처리 하고 call_info를 삭제하여야한다.
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
				// Connect 메시지를 받아서 Calling Socket으로 전송할때 
				// Delay없이 바로 전송시 Connect가 깨지는 현상이 발생함.
				// 이에 Connect 메시지 일때만 500ms의 시간을 지연함.
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
