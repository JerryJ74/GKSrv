// CallingSignalSocket.cpp : ���� �����Դϴ�.
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


// CCallingSignalSocket ��� �Լ��Դϴ�.
void CCallingSignalSocket::OnReceive(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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

		// TPKT�ǿ��� Packet size �� ���� rx �� size�� �ٸ� ��� Return �Ѵ�.
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
					// CallSignal Thread�� ����� �Բ� m_call_info�� �޸𸮿��� �������.
					// �Ͽ� DBWork Thread����ó���� �ʾ����� �޸� ���� ������ �߻��Ѵ�.
					// �̸� ���� ���Ͽ� call_info�� ���� ���� �Ͽ� DBWorkThread�� ������.
					// �̶� DBWorkThread������ ReleaseComplete(..) �Լ��� ó�� �ϰ� call_info�� �����Ͽ����Ѵ�.
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
	// Setup ���� Message ������ ���� addprefix �Ǵ� delprefix�� ���� �Ͽ� ������.

	// 2004. 10. 21. 
	// ��ȣ ���� �˰� ���� �߰�
	/*
	 1. Add �� ���� ��� - delprefix �� ���� addprefix �� ���� ���
	 2. Delete �ϰ� Add �� ��� - delprefix �� dest_num�� ���� ���ڿ� ��ġ �Ұ�� ��ġ�ϴ� �κб��� ����
	 3. ��� delete �ϰ� add �� ���
	 4. �ڸ����� delete �Ҷ�
	*/
	// parse del prefix

	char parsed_destnum[30];
	memset(parsed_destnum, 0, sizeof(parsed_destnum));

	StrTrim(m_call_info->delprefix, " ");
	StrTrim(m_call_info->addprefix, " ");

	// addprefix �� delprefix ��� ���� ���.
	if ( (strlen(m_call_info->delprefix) != 0) && (strlen(m_call_info->addprefix) != 0) )
	{
		char * temp = strstr(m_call_info->dest_num, m_call_info->delprefix);

		if ( (int)(temp - m_call_info->dest_num + 1) == 1 ) // ��ġ�ϴ� ��ŭ�� ���� �ϰ� add �Ѵ�.
		{
			strcpy(parsed_destnum, m_call_info->addprefix);
			strcat(parsed_destnum, m_call_info->dest_num + strlen(m_call_info->delprefix));
		} 
		else if (strcmp(m_call_info->delprefix, "*") == 0) // ��� ���� �ϰ� addprefix�� ������.
		{
			strcpy(parsed_destnum, m_call_info->addprefix);
		}
		else // �ڸ��� ���� ���� �Ѵ�.
		{
			// ex) -00,3  = 00���� �����ϴ� ��ȣ�� �տ� 3�ڸ��� ���� �Ѵ�.
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
	// addprefix �� �ְ� delprefix�� ���� ���.
	else if ( (strlen(m_call_info->addprefix) != 0) && (strlen(m_call_info->delprefix) == 0) ) 
	{
		strcpy(parsed_destnum, m_call_info->addprefix);
		strcat(parsed_destnum, m_call_info->dest_num);
	}
	// addprefix�� delprefix ��� ���� ���
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	//__DEBUG("Calling Socket Closed, m_bActive = false");
	m_bActive = false;
	CSocket::OnClose(nErrorCode);
}
