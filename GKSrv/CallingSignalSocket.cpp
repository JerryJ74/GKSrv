// CallingSignalSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "InterGK.h"
#include "CallingSignalSocket.h"
#include "DBWorkThread.h"
//#include "MessageThread.h"

// CCallingSignalSocket

CCallingSignalSocket::CCallingSignalSocket(LPCALL_INFO call_info)
{
	m_call_info = call_info;
	m_rxTPKT = false;
	m_packetLength = 0;
	m_bActive = true;
	//memset(m_rxBuff, 0, sizeof(m_rxBuff));
}

CCallingSignalSocket::~CCallingSignalSocket()
{
}


// CCallingSignalSocket 멤버 함수입니다.
void CCallingSignalSocket::OnReceive(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	// rx tpkt(get packet length)
	
	
	//temp code
	//CWinThread * thd = ::AfxGetThread();
	//__APPMSG(APPMSG_ENDCALL, "CallingSignalSocket OnRecevie, thread : %d", thd->m_nThreadID);
	//end of temp code

	if (m_rxTPKT == false)
	{
		memset(m_TPKT, 0, sizeof(m_TPKT));
		int ll = Receive((char *)m_TPKT, sizeof(m_TPKT));

		m_packetLength = ((m_TPKT[2] << 8) | m_TPKT[3]) - 4;

		if (m_packetLength < 5) 
    		return;
		m_rxTPKT = true;
		return;
	}

	if (m_rxTPKT && (m_packetLength > 0))
	{
		memset(m_rxBuff, 0, sizeof(m_rxBuff));
		int rx_size = Receive((char *)m_rxBuff, m_packetLength);

		// TPKT의에서 Packet size 와 실제 rx 한 size가 다를 경우 Return 한다.
		if (rx_size != m_packetLength)
		{
			//__DEBUG("Calling..Socket::OnReceive(), WARNING!!!, rx size:%d, tpkt size:%d, Close Call Thread", rx_size, m_packetLength);
			::PostThreadMessage(m_nThread, WM_QUIT, 0, 0);					
			return CSocket::OnReceive(nErrorCode);
		}

		m_rxTPKT = false;
		m_q931.SetData(m_rxBuff, m_packetLength, m_TPKT);

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
				CDBWorkThread::Instance()->RxSetup(&m_nThread, (void *)m_call_info);
				//Sleep(1000);
				break;
			case Q931Packet::Q931PacketType::e_Q931Connect:
				break;
			case Q931Packet::Q931PacketType::e_Q931ReleaseComplete:
				{
					m_call_info->call_stat = e_Q931_ReleaseComplete_From_Calling;
					m_call_info->release_complete = m_q931.GetReleaseReason();
					m_call_info->release_cause = m_q931.GetCause();

					// 2004. 11. 24. 
					// CallSignal Thread의 종료와 함께 m_call_info는 메모리에서 사라진다.
					// 하여 DBWork Thread에서처리가 늦어지면 메모리 참조 오류가 발생한다.
					// 이를 막기 위하여 call_info를 새로 생성 하여 DBWorkThread로 보낸다.
					// 이때 DBWorkThread에서는 ReleaseComplete(..) 함수를 처리 하고 call_info를 삭제하여야한다.
					if (m_call_info->call_stat != e_Q931_ReleaseComplete_From_Called)
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
			case Q931Packet::Q931PacketType::e_Q931Unknow:
				break;
			case Q931Packet::Q931PacketType::e_Q931Facility:
				break;
			}

			if (m_q931.GetQ931Type() != Q931Packet::Q931PacketType::e_Q931Setup)
			{
				rx_size = m_called->Send(m_TPKT, sizeof(m_TPKT));
				//int ss = (int)strlen((const char *)m_rxBuff);
				m_called->Send(m_rxBuff, m_packetLength, 0);// + 4, 0);
				//rx_size = m_called->Send(m_rxBuff, m_packetLength, 0);// + 4);
			}
		}
		else // Q.931 Decode Fail
		{
			//__DEBUG("CallingSignalSocket::OnReceive(), WARNING Decode Fail");
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

bool CCallingSignalSocket::OnSetupApplyChangePrefix()
{
	// Setup 에서 Message 보내기 전에 addprefix 또는 delprefix를 적용 하여 보낸다.

	// 2004. 10. 21. 
	// 번호 변경 알고 리즘 추가
	/*
	 1. Add 만 있을 경우 - delprefix 는 없고 addprefix 만 있을 경우
	 2. Delete 하고 Add 할 경우 - delprefix 가 dest_num의 앞의 숫자와 일치 할경우 일치하는 부분까지 삭제
	 3. 모두 delete 하고 add 할 경우
	 4. 자리수로 delete 할때
	*/
	// parse del prefix

	char parsed_destnum[30];
	memset(parsed_destnum, 0, sizeof(parsed_destnum));

	StrTrim(m_call_info->delprefix, " ");
	StrTrim(m_call_info->addprefix, " ");

	// addprefix 와 delprefix 모두 있을 경우.
	if ( (strlen(m_call_info->delprefix) != 0) && (strlen(m_call_info->addprefix) != 0) )
	{
		char * temp = strstr(m_call_info->dest_num, m_call_info->delprefix);

		if ( (int)(temp - m_call_info->dest_num + 1) == 1 ) // 일치하는 만큼만 삭제 하고 add 한다.
		{
			strcpy(parsed_destnum, m_call_info->addprefix);
			strcat(parsed_destnum, m_call_info->dest_num + strlen(m_call_info->delprefix));
		} 
		else if (strcmp(m_call_info->delprefix, "*") == 0) // 모두 삭제 하고 addprefix로 보낸다.
		{
			strcpy(parsed_destnum, m_call_info->addprefix);
		}
		else // 자리수 별로 삭제 한다.
		{
			// ex) -00,3  = 00으로 시작하는 번호는 앞에 3자리를 삭제 한다.
			char * tmp1 = strstr(m_call_info->delprefix, "-");
			char * tmp2 = strstr(m_call_info->delprefix, ",");

			if ( (tmp1) && (tmp2) )
			{
				int mpos = (int)(tmp1 - m_call_info->delprefix + 1);
				int cpos = (int)(tmp2 - m_call_info->delprefix + 1);
				
				char del[36];
				memset(del, 0, sizeof(del));

				strncpy(del, m_call_info->delprefix, cpos);

				char * tmp3 = strstr(m_call_info->delprefix, del);

				if ( (int)(tmp3 - m_call_info->delprefix + 1) == 1)
				{
					char cdelsize[5];
					memset(cdelsize, 0, sizeof(cdelsize));

					strncpy(cdelsize, m_call_info->delprefix + cpos, 1);
					int delsize = atoi((const char *)cdelsize);
					//int delsize = atoi((const char *)m_call_info->delprefix[cpos]);
					strcpy(parsed_destnum, (const char *) m_call_info->addprefix);
					strcat(parsed_destnum, (const char *) m_call_info->dest_num + delsize);
				}
				else
					strcpy(parsed_destnum, m_call_info->dest_num);
			} 
			else
				strcpy(parsed_destnum, m_call_info->dest_num);
		}
	}
	// addprefix 는 있고 delprefix는 없을 경우.
	else if ( (strlen(m_call_info->addprefix) != 0) && (strlen(m_call_info->delprefix) == 0) ) 
	{
		strcpy(parsed_destnum, m_call_info->addprefix);
		strcat(parsed_destnum, m_call_info->dest_num);
	}
	// addprefix와 delprefix 모두 없을 경우
	else if ( (strlen(m_call_info->addprefix) == 0) && (strlen(m_call_info->delprefix) == 0) )
	{
		strcpy(parsed_destnum, (const char *)m_call_info->dest_num);
	}

	m_q931.SetData(m_rxBuff, m_packetLength, m_TPKT);

	int parsed_packetLength = m_q931.SetCalledPartyNum(parsed_destnum);

	if (parsed_packetLength)
	{
		m_packetLength = parsed_packetLength;
		return true;
	}
	else
		return false;
}


bool CCallingSignalSocket::OnSetupApplyChangeCallingNum()
{
	m_q931.SetData(m_rxBuff, m_packetLength, m_TPKT);
	
	m_packetLength = m_q931.SetCallingPartyNum("012345678");

	if (m_packetLength)
		return true;	
	else
		return false;
}


bool CCallingSignalSocket::OnSetupChangeCalledH323id()
{
	m_q931.SetCalledH323ID("temp");
	return true;
}


void CCallingSignalSocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//__DEBUG("Calling Socket Closed, m_bActive = false");
	m_bActive = false;
	CSocket::OnClose(nErrorCode);
}
