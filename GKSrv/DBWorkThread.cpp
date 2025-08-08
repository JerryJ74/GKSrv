// DBWorkThread.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "InterGK.h"
#include "DBWorkThread.h"
//#include "SockDebug.h"
#include "GKConst.h"
//#include "GKConfig.h"
#include "MessageThread.h"

// CDBWorkThread

CDBWorkThread * CDBWorkThread::m_instance = NULL;

IMPLEMENT_DYNCREATE(CDBWorkThread, CWinThread)

CDBWorkThread::CDBWorkThread()// : m_propset(DBPROPSET_ROWSET)
{
	m_instance = this;


	/*
	CoInitialize(NULL);
	
	m_propset.AddProperty(DBPROP_IRowsetScroll, true);
	m_propset.AddProperty(DBPROP_IRowsetChange, true, DBPROPOPTIONS_REQUIRED); // | VARIANT_TRUE);
	m_propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_INSERT | DBPROPVAL_UP_CHANGE | DBPROPVAL_UP_DELETE | VARIANT_TRUE);
	m_propset.AddProperty(DBPROP_CANFETCHBACKWARDS, true, DBPROPOPTIONS_OPTIONAL);
	m_propset.AddProperty(DBPROP_CANSCROLLBACKWARDS, true, DBPROPOPTIONS_OPTIONAL);

	HRESULT hr = m_reg.OpenDataSource();

	hr = m_auth.OpenDataSource();

	hr = m_prefix.OpenDataSource();

	hr = m_call.OpenDataSource();

	//hr = m_currentcall.OpenDataSource();

	m_reg.m_dwh323idLength = 15;
	m_reg.m_dwipaddrLength = 15;
	m_reg.m_dwcallsignaladdrLength = 15;
	m_reg.m_dwrasportLength = 4;
	m_reg.m_dwcallsignalportLength = 4;
	m_reg.m_dwlast_signalLength = 8;
	m_reg.m_dwlast_signal_timeLength = 8;
	//m_reg.m_dwcurrent_rxLength = 2;
	//m_reg.m_dwcurrent_txLength = 2;
	m_reg.m_dwdebugLength = 2;
	m_reg.m_dwaddprefixLength = 5;
	m_reg.m_dwdelprefixLength = 5;
	m_reg.m_dwcapacityLength = 2;
	m_reg.m_dwregistered_gkLength = 12;
	m_reg.m_dwe164Length = 104;

	m_reg.m_dwh323idStatus = DBSTATUS_S_OK;
	m_reg.m_dwipaddrStatus = DBSTATUS_S_OK;
	m_reg.m_dwcallsignaladdrStatus = DBSTATUS_S_OK;
	m_reg.m_dwrasportStatus = DBSTATUS_S_OK;
	m_reg.m_dwcallsignalportStatus = DBSTATUS_S_OK;
	m_reg.m_dwlast_signalStatus = DBSTATUS_S_OK;
	m_reg.m_dwlast_signal_timeStatus = DBSTATUS_S_OK;
	//m_reg.m_dwcurrent_rxStatus = DBSTATUS_S_OK;
	//m_reg.m_dwcurrent_txStatus = DBSTATUS_S_OK;
	m_reg.m_dwdebugStatus = DBSTATUS_S_OK;
	m_reg.m_dwaddprefixStatus = DBSTATUS_S_OK;
	m_reg.m_dwdelprefixStatus = DBSTATUS_S_OK;
	m_reg.m_dwcapacityStatus = DBSTATUS_S_OK;
	m_reg.m_dwregistered_gkStatus = DBSTATUS_S_OK;
	m_reg.m_dwe164Status = DBSTATUS_S_OK;

	m_prefix.m_dwh323idLength = 15;
	m_prefix.m_dwprefixLength = 12;
	m_prefix.m_dwh323idStatus = DBSTATUS_S_OK;
	m_prefix.m_dwprefixStatus = DBSTATUS_S_OK;

	m_call.m_dwcall_idLength = 32;
	m_call.m_dwcrvLength = 4;
	m_call.m_dwthread_idLength = 4;
	m_call.m_dwcallingLength = 15;
	m_call.m_dwcalling_ipLength = 15;
	m_call.m_dwcalling_numLength = 15;
	m_call.m_dwcalledLength = 15;
	m_call.m_dwcalled_ipLength = 15;
	m_call.m_dwcalled_numLength = 18;
	//m_call.m_dwstatsLength = 2;
	m_call.m_dwconnect_timeLength = 8;
	m_call.m_dwdisconnect_timeLength = 8;
	m_call.m_dwarq_timeLength = 8;
	m_call.m_dwaddprefixLength = 5;
	m_call.m_dwdelprefixLength = 5;
	m_call.m_dwtimeoutLength = 2;
	m_call.m_dwlrq_seq_numLength = 4;
	m_call.m_dwarq_rx_portLength = 4;

	m_call.m_dwcall_idStatus = DBSTATUS_S_OK;
	m_call.m_dwcrvStatus = DBSTATUS_S_OK;
	m_call.m_dwthread_idStatus = DBSTATUS_S_OK;
	m_call.m_dwcallingStatus = DBSTATUS_S_OK;
	m_call.m_dwcalling_ipStatus = DBSTATUS_S_OK;
	m_call.m_dwcalling_numStatus = DBSTATUS_S_OK;
	m_call.m_dwcalledStatus = DBSTATUS_S_OK;
	m_call.m_dwcalled_ipStatus = DBSTATUS_S_OK;
	m_call.m_dwcalled_numStatus = DBSTATUS_S_OK;
	//m_call.m_dwstatsStatus = DBSTATUS_S_OK;
	m_call.m_dwconnect_timeStatus = DBSTATUS_S_OK;
	m_call.m_dwdisconnect_timeStatus = DBSTATUS_S_OK;
	m_call.m_dwarq_timeStatus = DBSTATUS_S_OK;
	m_call.m_dwaddprefixStatus = DBSTATUS_S_OK;
	m_call.m_dwdelprefixStatus = DBSTATUS_S_OK;
	m_call.m_dwtimeoutStatus = DBSTATUS_S_OK;
	m_call.m_dwlrq_seq_numStatus = DBSTATUS_S_OK;
	m_call.m_dwarq_rx_portStatus = DBSTATUS_S_OK;
	//m_call.m_spRowsetChange
	*/

	// LRQ in Request Sequence Number Init;
	// 2003. 10. 17
	// lrq request seq num 을 3000 이상 부터로 하고 
	m_LRQRequestSeqNum = 3001;

	// log 작성 line 수
	m_debugline = 0;
	sprintf(m_logpath, "LOG_DBSIGNAL_%d\\", SIGNAL_PORT);

	::CreateDirectory(m_logpath, NULL); 
}

CDBWorkThread::~CDBWorkThread()
{
}

BOOL CDBWorkThread::InitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	//::SetTimer(m_pMainWnd->m_hWnd, m_nThreadID, (IRR_INTERVAL * 1000), (TIMERPROC) NULL);
	//#define CONST_DBIP					"211.189.4.231"
	//#define CONST_DBNAME				"intergk"
	//#define CONST_ID					"intergk"
	//#define CONST_PASS					"123456"

	m_db.OpenDatabase(CONST_DBIP, CONST_DBNAME, CONST_DBID, CONST_DBPASS, CONST_DBPORT);

	return TRUE;
}

int CDBWorkThread::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
	//m_auth.CloseDataSource();
	//m_reg.CloseDataSource();
	//m_prefix.CloseAll();
	//m_call.CloseAll();
	//CoUninitialize();

	return CWinThread::ExitInstance();
}

/*
void CDBWorkThread::RegistrationRequest(DWORD * srcHnd, void * pData)
{
	PostThreadMessage(WM_RAS_REGRQST, WPARAM(srcHnd), LPARAM(pData));
}

void CDBWorkThread::AdmissionRequest(DWORD * srcHnd, void * pData)
{
	PostThreadMessage(WM_RAS_ADMRQST, WPARAM(srcHnd), LPARAM(pData));
}

void CDBWorkThread::InformationRequest(LPRAS_IRR irr_info)
{
	PostThreadMessage(WM_RAS_INFRQST, 0, LPARAM(irr_info));
}

*/

void CDBWorkThread::RxSetup(DWORD * srcHnd, void * pData)
{
	PostThreadMessage(WM_Q931_SETUP, WPARAM(srcHnd), LPARAM(pData));
}

void CDBWorkThread::RxReleaseComplete(DWORD * srcHnd, void * pData)
{
	PostThreadMessage(WM_Q931_RELEASECOMPLETE, WPARAM(srcHnd), LPARAM(pData));
}

void CDBWorkThread::RxConnect(DWORD * srcHnd, void * pData)
{
	PostThreadMessage(WM_Q931_CONNECT, WPARAM(srcHnd), LPARAM(pData));
}

void CDBWorkThread::CheckCall(DWORD * srcHnd, void * pData)
{
	PostThreadMessage(WM_CALLINFO, WPARAM(srcHnd), LPARAM(pData));
}

/*
void CDBWorkThread::LocationConfirm(DWORD * srcHnd, void * pData)
{
	PostThreadMessage(WM_RAS_LOCCFRM, WPARAM(srcHnd), LPARAM(pData));
}

void CDBWorkThread::UnregistrationRequest(void * pData)
{dd
	PostThreadMessage(WM_RAS_URGRQST, 0, LPARAM(pData));
}

void CDBWorkThread::LocationReject(DWORD * srcHnd, void * pData)
{
	PostThreadMessage(WM_RAS_LOCRJCT, WPARAM(srcHnd), LPARAM(pData));
}
*/

CDBWorkThread * CDBWorkThread::Instance()
{
	return m_instance;
}

BEGIN_MESSAGE_MAP(CDBWorkThread, CWinThread)
END_MESSAGE_MAP()

// CDBWorkThread 메시지 처리기입니다.

BOOL CDBWorkThread::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	_DD("RxMsg, Type:%ld", pMsg->message);
	DWORD st = GetTickCount();
	DWORD buffered_msg = GetQueueStatus(QS_POSTMESSAGE);
	WORD hi  = HIWORD(buffered_msg);//(DWORD)(HIBYTE(LOWORD(buffered_msg))); //(DWORD)(LOBYTE(LOWORD(dwVersion)));
	WORD low = LOWORD(buffered_msg);//(DWORD)(LOBYTE(LOWORD(buffered_msg))); //(DWORD)(HIBYTE(LOWORD(dwVersion)));

	switch(pMsg->message)
	{
		//case WM_RAS_REGRQST:
		//	RegistrationRequest_DB((DWORD *)pMsg->wParam, (LPRAS_RRQRCF)pMsg->lParam);
		//	break;
		//case WM_RAS_ADMRQST:
		//	AdmissionRequest_DB((DWORD *)pMsg->wParam, (LPRAS_ARQACF)pMsg->lParam);
		//	break;
		case WM_Q931_SETUP:
			RxSetup_DB((DWORD *)pMsg->wParam, (LPCALL_INFO)pMsg->lParam);
			break;
		case WM_Q931_RELEASECOMPLETE:
			RxReleaseComplete_DB((DWORD *)pMsg->wParam, (LPCALL_INFO)pMsg->lParam);
			break;
		case WM_Q931_CONNECT:
			RxConnect_DB((DWORD *)pMsg->wParam, (LPCALL_INFO)pMsg->lParam);
			break;
		case WM_CALLINFO:
			CheckCall_DB((DWORD *)pMsg->wParam, (LPRAS_IRR)pMsg->lParam);
			break;
		//case WM_RAS_INFRQST:
		//	InformationRequest_DB((LPRAS_IRR)pMsg->lParam);
		//	break;
		//case WM_TIMER:
		//	TimeOutCheck_ALLDB();
		//	break;
		//case WM_RAS_URGRQST:
		//	UnregistrationRequest_DB((LPRAS_URQUCF)pMsg->lParam);
		//	break;
		//case WM_RAS_LOCCFRM:
		//	LocationConfirm_DB((DWORD *)pMsg->wParam, (LPRAS_ARQACF)pMsg->lParam);
		//	break;
		//case WM_RAS_LOCRJCT:
		//	LocationReject_DB((DWORD *)pMsg->wParam, (LPRAS_LRJ)pMsg->lParam);
		//	break;
		default :
			break;
	}
	
	DWORD ed = GetTickCount();
	_DD("End RxMsg, dur:%ld, MsgQueue:%u, hi:%d, lo:%d\r\n", ed - st, buffered_msg, hi, low);

	return CWinThread::PreTranslateMessage(pMsg);
}

/*
void CDBWorkThread::RegistrationRequest_DB(DWORD * srcHnd, RAS_RRQRCF * Data)
{
	//_DD("RegistrationRequest_DB, srcHnd:%ld, h323id:%s, ip:%s, start..", *srcHnd, Data->h323id, Data->rasaddr);
	char sql[512] = "";
	sprintf(sql, "select * from Authentication where h323id = '%s'", Data->h323id);
	HRESULT hr;
	
	//_DD("Reg_DB(), m_auth.Open, sql:%s", sql);	
	if (m_auth.Open(m_auth, sql) != S_OK)
	{
		m_auth.Close();
		_DD("Reg_DB(), m_auth.Open ERROR, result is NOT S_OK");
		//__DEBUG("CDBWorkThread::RegistrationRequest_DB(), m_auth open fail");
		delete Data;
		return;
	}

	hr = m_auth.MoveFirst();
	if (hr == DB_S_ENDOFROWSET)
	{
		_DD("Reg_DB(), m_auth.MoveFirst() hr:DB_S_ENDOFROWSET, hr:%lX", hr);
		m_auth.Close();
		Data->Confirm = false;
		::PostThreadMessage(*srcHnd, WM_RAS_REGRQST_RESULT, 0, LPARAM(Data));
		return;
	}

	StrTrim(m_auth.m_h323id, " ");
	StrTrim(m_auth.m_e164, " ");
	StrTrim(m_auth.m_ipaddress, " ");

	
	// Authentication
	bool bAuthed = false;

	switch(CGKConfig::Instance()->GetAuthMode())
	{
	case AUTH_E164 :
		if (strcmp(Data->e164, m_auth.m_e164) == 0)
			bAuthed = true;
		else
			bAuthed = false;
		break;
	case AUTH_IP_H323ID :
		if ( (strcmp(Data->h323id, m_auth.m_h323id) == 0) && 
			 (strcmp(Data->callsignaladdr, m_auth.m_ipaddress) == 0) )
			bAuthed = true;
		else
			bAuthed = false;
		break;
	case AUTH_H323ID :
		if (strcmp(Data->h323id, m_auth.m_h323id) == 0)
			bAuthed = true;
		else
			bAuthed = false;
		break;
	case AUTH_E164_H323ID :
		if ( (strcmp(Data->h323id, m_auth.m_h323id) == 0) && (strcmp(Data->e164, m_auth.m_e164) == 0) )
			bAuthed = true;
		else
			bAuthed = false;
		break;
	case AUTH_E164_H323ID_IP :
		bAuthed = false;
		break;
	}

	m_auth.Close();
	//_DD("Reg_DB(), m_auth.Close()");

	if (bAuthed)
	{
		// 2003. 11. 27. callsignalport 를 update 항목에 추가 함.
		DBROWCOUNT ef_count = 0;		
		sprintf(sql, "update registration_info set e164='%s', ipaddr='%s', callsignaladdr='%s', registered_gk='%s', last_signal_time=getdate(), last_signal='LWRRQ', callsignalport=%ld where h323id='%s'",
			Data->e164, Data->callsignaladdr, Data->callsignaladdr, GK_ID, Data->callsignalport, Data->h323id);
		hr = m_reg.Open(m_reg, sql, 0, &ef_count, DBGUID_DBSQL, false, 0);
		m_reg.Close();

		//_DD("Reg_DB(), Open, sql:%s, eff_row:%ld, hr:%lX", sql, ef_count, hr); 
		Data->Confirm = true;
		// Update 했는데 영향을 받은 Rec가 없다면 Insert 해야 한다.
		if (ef_count == 0)
		{
			// 2003. 10. 9. insert 시 current_tx, current_rx field 제거함으로 query 수정
			sprintf(sql, "insert into registration_info values ('%s', '%s', '%s', %d,  %d, 'RRQ', getdate(), 0, '', '', 0, '%s', '%s')",
			  Data->h323id, Data->rxaddr, Data->callsignaladdr, Data->rasport, Data->callsignalport, GK_ID, Data->e164); 
			hr = m_reg.Open(m_reg, sql, 0, &ef_count, DBGUID_DBSQL, false, 0);
			m_reg.Close();
			//_DD("Reg_DB(), Insert, sql:%s, eff_row:%ld, hr:%lX", sql, ef_count, hr);

			if (ef_count != 1)
				Data->Confirm = false;				
		}
	}
	else
		Data->Confirm = false;

	//_DD("RegistrationRequest_DB, srcHnd:%ld, h323id:%s, ip:%s, end...", *srcHnd, Data->h323id, Data->rasaddr);
	//_DD("RegistrationRequest_DB, srcHnd:%ld, end...", *srcHnd);
	::PostThreadMessage(*srcHnd, WM_RAS_REGRQST_RESULT, 0, LPARAM(Data));
}
*/

/*
void CDBWorkThread::AdmissionRequest_DB(DWORD * srcHnd, RAS_ARQACF * Data)
{
	//_DD("Adm_DB, srcHnd:%ld, clling:%s, destnum:%s, start..", *srcHnd, Data->calling, Data->dest_num);

	//E.164 검사
	char addprefix[6] = "";
	char delprefix[6] = "";
	HRESULT hr;
	char sql[512] = "";

	Data->Confirm = false;
	bool bNeighboring = false;
	bool bAuthed = false;

	// Need Calling Authentication
	// 2003. 11. 21. RCF 받지 못한 CPE가 ARQ를 발신 하였을 경우 이에 대한 체크가 없음
	// 때문에 Authentication Table을 한번더 검색 함.
	sprintf(sql, "select * from authentication where h323id = '%s'", Data->calling);
	hr = m_auth.Open(m_auth, sql);
	if ( (SUCCEEDED(hr)) && (m_auth.MoveFirst() != DB_S_ENDOFROWSET) )
		bAuthed = true;
	else
	{
		_DD("Adm_DB, m_reg.Open, %s, Authentication Fail", Data->calling);
		strcpy(Data->called_ip, "Auth Fail");
		bAuthed = false;
	}
	m_auth.Close();


	if (bAuthed)
	{
		// Find E.164
		//sprintf(sql, "select * from registration_info where e164 like '\%@%s'
		memset(sql, 0, sizeof(sql));
		strcpy(sql, "select * from Registration_info where e164 like '%@");
		strcat(sql, Data->dest_num);
		strcat(sql, "%' order by h323id");
		//_DD("Adm_DB, m_reg.Open, sql:%s", sql);
		hr = m_reg.Open(m_reg, sql);
		// Find E.164
		while (m_reg.MoveNext() != DB_S_ENDOFROWSET) 
		{
			char * tmp1 = strstr(m_reg.m_e164, Data->dest_num);
			char * tmp2 = strstr(tmp1, "@");
			if (!tmp2)
				Data->Confirm = true;
			else
				if ( (strlen(tmp1) - strlen(tmp2)) == strlen(Data->dest_num) )
					Data->Confirm = true;

			if (Data->Confirm)
			{
				strcpy(Data->called, m_reg.m_h323id);
				strcpy(Data->called_ip, m_reg.m_callsignaladdr);
				Data->called_port = (WORD)m_reg.m_callsignalport;
				strcpy(addprefix, _T(m_reg.m_addprefix));
				strcpy(delprefix, _T(m_reg.m_delprefix));
				break;
			}
		}
		m_reg.Close();
		//_DD("Adm_DB, m_reg.Close(), result:%lx", hr);
	}
	
	// Find Prefix 
	if ( (Data->Confirm == false) && (bAuthed) )
	{
		memset(sql, 0, sizeof(sql));
		strcpy(sql, "select * from prefixes order by len(prefix) desc");
		
		//_DD("Adm_DB, m_prefix.Open(), sql:%s", sql);
		hr = m_prefix.Open(m_prefix, sql, 0);
		
		size_t maxlength = 0;
		size_t length = 0;
		int fcount = 0;
		char find_called[5][16];
		memset(find_called, 0, sizeof(find_called));
		while (m_prefix.MoveNext() != DB_S_ENDOFROWSET)
		{
			StrTrim(m_prefix.m_prefix, " ");
			StrTrim(m_prefix.m_h323id, " ");
			
			// Prefix table을 순차적으로 Count 하다가 현재 Count가 이전에 검색한
			// maxlength 보다 작을 경우 더이상 검색 필요가 없음으로 while문을 종료 한다.
			if (strlen(m_prefix.m_prefix) < maxlength)
				break;

			length = IsMatchedPrefix(Data->dest_num, m_prefix.m_prefix);
			if ( (length >= maxlength) && length )
			{
				// 로드 발란스 라우팅을 위하여 찾은 prefix의 수를 증가 한다.
				maxlength = length;
				strcpy(find_called[fcount], m_prefix.m_h323id);
				fcount++;
				// Max load balance gw size = 5
				if (fcount >= 5) break;
			}
		}
		m_prefix.Close();
		//_DD("Adm_DB, m_prefix.Close()");

		if (maxlength > 0) 
		{
			memset(sql, 0, sizeof(sql));
			strcpy(sql, "select * from ConCurrentCall where h323id ='");
			// 로드 발란싱 할 gw 의 수 많큼 or 하여 select 한다.
			for (int i = 0; i < fcount; i++)
			{
				if (i == 0)
				{
					strcat(sql, find_called[i]);
					strcat(sql, "'");
				}
				else
				{
					strcat(sql, " or h323id = '");
					strcat(sql, find_called[i]);
					strcat(sql, "'");
				}
			}
			hr = m_currentcall.Open(m_currentcall, sql);
			//_DD("Adm_DB, m_currentcall.Open(), sql:%s, hr:%lX", sql, hr);

			if (m_currentcall.MoveFirst() != DB_S_ENDOFROWSET)
			{
				StrTrim(m_currentcall.m_callsignaladdr, " ");
				StrTrim(m_currentcall.m_h323id, " ");
				strcpy(Data->called, m_currentcall.m_h323id);
				strcpy(Data->called_ip, m_currentcall.m_callsignaladdr);
				Data->called_port = (WORD)m_currentcall.m_callsignalport;
				strcpy(addprefix, _T(m_currentcall.m_addprefix));
				if (strcmp(m_currentcall.m_registered_gk, "NEIGHBORING ") == 0)
					bNeighboring = true;

				Data->Confirm = true;
			}
			else
			{
				Data->Confirm = false;
				strcpy(Data->called_ip, "No Route");
			}
			m_currentcall.Close();
			//_DD("Adm_DB, m_currentcall.CloseAll()");

		}
		else // not found prefix
		{
			strcpy(Data->called_ip, "No Route");
			Data->Confirm = false;
		}
	} // end of find preifx (if (Data->Con...))

	// confirm
	if ( (Data->Confirm) && (strcmp("0.0.0.0", Data->called_ip) != 0) )
	{
		// Neighboring의 경우 DB의 thread_id 에 request sequence number를 넣는다.
		// thread_id field 임시 활용
		WORD rsn = 0;
		WORD lrq_rsn = 0;
		DBROWCOUNT insert_cnt = 0;
		if (bNeighboring)
		{
			rsn = Data->request_number;
			lrq_rsn = GetLRQRequestSeqNum();
			Data->request_number = lrq_rsn;
		}

		// 2003. 11. 27. 
		// Data->called_port를 Call Table 마지막 필드 signal_port로 추가
		// setup 메시지를 calling으로 부터 받은 이후 connection 성립을 called_port로 성립 후 setup을 전달하도록 수정함

		// Insert Call Rec;
		memset(sql, 0, sizeof(sql));
		sprintf(sql, "insert into call values('%s', %d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '', %s, '%s', '%s', '%s', 0, %d, %d, %d)",
					Data->call_id, Data->crv, rsn, Data->calling, Data->calling_ip, Data->src_num,
					Data->called,  Data->called_ip, Data->dest_num,
					"getdate()", addprefix, delprefix, GK_ID, lrq_rsn, Data->rxport, Data->called_port);
		
		hr = m_call.Open(m_call, sql, 0, &insert_cnt, DBGUID_DBSQL, false, 0);
		//_DD("Adm_DB, m_call.Open(), sql:%s, insert_cnt:%ld, hr:%lX", sql, insert_cnt, hr);
		m_call.Close();
 		//_DD("Adm_DB, m_call.Close()");

		if (FAILED(hr)) 
		{
			Data->Confirm = false;
			strcpy(Data->called_ip, "Call Reject");
			//__DEBUG("Adm..DB(), Call Insert FAIL, clling:%s, clled:%s, dest:%s, hr:%lX", Data->calling, Data->called, Data->dest_num, hr);
		}
	}
	else // reject
	{
		Data->Confirm = false;
	}

	//_DD("Adm_DB, srcThread:%ld, clling:%s, clled:%s, destnum:%s end.....", *srcHnd, Data->calling, Data->dest_num, Data->called);

	if (!(bNeighboring))
		::PostThreadMessage(*srcHnd, WM_RAS_ADMRQST_RESULT, 0, LPARAM(Data));
	else
		::PostThreadMessage(*srcHnd, WM_RAS_NEEDLRQ, 0, LPARAM(Data));
}

void CDBWorkThread::InformationRequest_DB(LPRAS_IRR irr_info)
{
	//_DD("InforRequest_DB,  clling:%s, crv:%d, callid:%s start.....", irr_info->calling, irr_info->crv, irr_info->call_id);

	char sql[128] = "";
	//m_call.Close();
	DBROWCOUNT update_cnt = 0;
	sprintf(sql, "update call set timeout = 0 where calling = '%s' and crv = %d and call_id = '%s'", 
		irr_info->calling, irr_info->crv, irr_info->call_id);
	HRESULT hr = m_call.Open(m_call, sql, 0, &update_cnt, DBGUID_DBSQL, false, 0);
	
	if (update_cnt != 1)
		//__DEBUG("CDBWorkThread::Infor..DB(), not found call, clling:%s, call_id:%s", irr_info->calling, irr_info->call_id);

	m_call.Close();

	delete irr_info;
	//_DD("InforRequest_DB, end..........");
}  */

void CDBWorkThread::RxSetup_DB(DWORD * srcHnd, LPCALL_INFO call_info)
{
	//_DD("RxSetup_DB, srcHnd:%ld, calling:%s, crv:%d, callid:%s, start..", *srcHnd, call_info->calling, call_info->crv, call_info->call_id);
	char sql[256] = "";
	
	// 2004. 01. 27
	// calling 까지 검색 조건에 넣으면 netmeeting에서 h323id를 다르게 넣어서 setup을 보냄으로 잠시 
	// 주석 처리함
	//sprintf(sql, "select * from call where call_id='%s' and crv=%d and calling='%s'", call_info->call_id, call_info->crv, call_info->calling);
	
	sprintf(sql, "select * from call where call_id='%s' and crv=%d", call_info->call_id, call_info->crv);

	m_db.OpenSQL(sql);
	//HRESULT hr = m_call.Open(m_call, sql, &m_propset);

	if (m_db.MoveFirst() != DB_S_ENDOFROWSET)
	{

		m_db.GetChar("called", call_info->called);
		m_db.GetChar("called_ip", call_info->called_ip);
		m_db.GetChar("addprefix", call_info->addprefix);
		m_db.GetChar("delprefix", call_info->delprefix);
		m_db.GetChar("calling_ip", call_info->calling_ip);

		//strcpy(call_info->called, m_call.m_called);
		//strcpy(call_info->called_ip, m_call.m_called_ip);
		//strcpy(call_info->addprefix, m_call.m_addprefix);
		//strcpy(call_info->delprefix, m_call.m_delprefix);
		//strcpy(call_info->calling_ip, m_call.m_calling_ip);
		
		call_info->called_port = m_db.GetInt("signal_port");//(WORD)m_call.m_signal_port;

		DBTIMESTAMP dbtime;
		m_db.GetDate("arq_time", dbtime);
		
		sprintf(call_info->arq_time, "%d-%d-%d %d:%d:%d", 
			dbtime.year, dbtime.month, dbtime.day, dbtime.hour, dbtime.minute, dbtime.second);
		// m_call.m_thread_id = *srcHnd;
		//HRESULT hr = m_call.SetData();
		
		// |Hanaro  |  -->  |Hanaro|
		StrTrim(call_info->called, " ");
		::PostThreadMessage(*srcHnd, WM_Q931_SETUP_RESULT, 0, LPARAM(call_info));
	}
	else
	{
		//__DEBUG("CDBWorkThread::RxSetupDB(), not found call, clling:%s, call_id:%s", call_info->calling, call_info->call_id);
		// TEMP
		::PostThreadMessage(*srcHnd, WM_CALLINFO_NOT_FOUND, 0, LPARAM(call_info));
	}
	
	m_db.CloseSQL();

	sprintf(sql, "update call set thread_id = %d where call_id = '%s' and crv = %d", *srcHnd, call_info->call_id, call_info->crv);
	m_db.ExecSQL(sql);

	// m_call.Close();
	//_DD("RxSetup_DB, srcHnd:%ld, calling:%s, crv:%d, called:%s, end..", *srcHnd, call_info->calling, call_info->crv, call_info->called);
}

void CDBWorkThread::RxReleaseComplete_DB(DWORD * srcHnd, LPCALL_INFO call_info)
{
	// 2003. 9. 15일 수정
	// Call Count Calc는 성공 하나 이후 Call delete procedure 가 실패 함.
	// 이에 call rec delete 이후 call count calc를 한다.
	
	//_DD("RxReleaseComplete_DB, srcHnd:%ld, calling:%s, crv:%d, called:%s, start..", *srcHnd, call_info->calling, call_info->crv, call_info->called);

	char sql[256] = "";
	
	if (strcmp(call_info->delprefix, "__LRQ") == 0)
		sprintf(sql, "select * from call where call_id='%s'", call_info->call_id);
	else
		sprintf(sql, "select * from call where call_id='%s' and crv=%d and calling='%s'", call_info->call_id, call_info->crv, call_info->calling);
	
	DBROWCOUNT del_cnt = 0;

	m_db.OpenSQL(sql);

	// 2005. 05. 25.
	// call_info 구조체를 delete 하지 않고 return 할 경우 memory leak 발생
	// delete call_info 구조체 삭제 코드 추가.
	if (m_db.MoveFirst() == DB_S_ENDOFROWSET)
	{
		m_db.CloseSQL();
		delete call_info;
		return;
	}
	
	// TODO 2004. 11. 24. 
	if (call_info->connect)
	{
		DBTIMESTAMP ctime;
		m_db.GetDate("connect_time", ctime);
		sprintf(call_info->cnt_time, "%d-%d-%d %d:%d:%d", ctime.year, 
			ctime.month, ctime.day, ctime.hour, ctime.minute, ctime.second);

		CDRProcess(call_info);
	}
	//	CDRProcess(&m_call, call_info->release_cause);

/*
	2004. 08. 27.
	M3Telecom(미국)에서 실패 CDR을 남기기 위해서 아래 코드를 추가 한다.
*/
#if defined(GK_FOR_M3TEL)

	// TODO 2004. 11. 24. 
	// CDR 생성 해야 한다.
	if (call_info->connect == false)
		CDRProcess(call_info);
		//CDRProcess(&m_call, call_info->release_cause);
#endif

	m_db.CloseSQL();

	// Send ENDCALL message to GKTool
	if (strlen(call_info->called) != 0)
	{
		char cnt[5] = "";
		int dur = 0;
		div_t dur_t;

		dur_t.quot = 0;
		dur_t.rem = 0;

		if (call_info->connect)
		{
			dur = GetTickCount() - call_info->connect_t;
			dur_t = div(dur, 1000);
		}
		
		if (call_info->connect) strcpy(cnt, "OK"); else strcpy(cnt, "NOK");

		__APPMSG(APPMSG_ENDCALL, "CALL|%d|%s|%s|%s|%s|%s|%d|%d|%ld|;", dur_t.quot, cnt, 
			call_info->calling, call_info->src_num, call_info->called, call_info->dest_num,
			call_info->release_complete, call_info->release_cause, call_info->crv);
	} 

	sprintf(sql, "delete call where call_id='%s' and crv=%d and calling='%s'", call_info->call_id, call_info->crv, call_info->calling);

	m_db.ExecSQL(sql);

	delete call_info;

	//hr = m_call.Open(m_call, sql, 0, &del_cnt, DBGUID_DBSQL, false, 0);
	//m_call.Close();
	
	//if (del_cnt != 1)
		//__DEBUG("CDBWork..::RxRelease..DB(), WARNING, CALL RECORD Delete fail, clling:%s, clled:%s", call_info->calling, call_info->called);

	//_DD("RxReleaseComplete_DB, srcHnd:%ld, calling:%s, crv:%d, called:%s, del_cnt:%ld end..", *srcHnd, call_info->calling, call_info->crv, call_info->called, del_cnt);
}

void CDBWorkThread::RxConnect_DB(DWORD * srcHnd, LPCALL_INFO call_info)
{
	//_DD("RxConnect_DB, srcHnd:%ld, clling:%s, crv:%d, start..", *srcHnd, call_info->calling, call_info->crv); 
	// 2004. 08. 05 
	// getdate를 사용 했으나 connect time은 remote에서 받아오고
	// release complete는 local 에서 받아오는 문제로 시간에 오차가 생긴다
	// 이로 인한 버그를 없애기 위하여 connect time도 local에서 받아오도록 수정한다.

	char sql[256] = "";
	char ctime[128] = "";
	DBTIMESTAMP db_ctime;
	GetTimeDB(db_ctime);
	sprintf(ctime, "%d-%d-%d %d:%d:%d", db_ctime.year, db_ctime.month, db_ctime.day, db_ctime.hour, db_ctime.minute, db_ctime.second);

	//sprintf(sql, "update call set connect_time = getdate() where call_id='%s' and crv=%d and calling='%s'", call_info->call_id, call_info->crv, call_info->calling);
	sprintf(sql, "update call set connect_time = '%s' where call_id='%s' and crv=%d and calling='%s'", ctime, call_info->call_id, call_info->crv, call_info->calling);

	// HRESULT hr = m_call.Open(m_call, sql, 0, 0, DBGUID_DBSQL, false, 0);
	BOOL r = m_db.ExecSQL(sql, true);

	if (r)
	{
		//2003. 10.08 stats field 삭제
		//m_call.m_stats = call_info->call_stat;
		DBTIMESTAMP t;
		GetTimeDB(t);
		sprintf(call_info->cnt_time, "%d-%d-%d %d:%d:%d", t.year, t.month, t.day, t.hour, t.minute, t.second);
		call_info->connect = true;
		call_info->connect_t = GetTickCount();
	}
	//else
		//__DEBUG("DBWork..::RxConnect_DB(), connect db, not found call rec, clling:%s, clled:%s",  call_info->calling, call_info->called);
	
	// m_call.Close();

	// _DD("RxConnect_DB, end."); 
}

/*
void CDBWorkThread::TimeOutCheck_ALLDB()
{
	//_DD("TimeoutCheck_DB, start");
	WORD delcount = 0;

	char sql[128] = "";
	sprintf(sql, "select * from call where registered_gk='%s'", GK_ID);

	WORD select_cnt = 0;
	DBROWCOUNT update_cnt = 0;
	DBROWCOUNT del_cnt = 0;

	HRESULT hr = m_call.Open(m_call, sql);

	// error check
	if (FAILED(hr))
	{
		//__DEBUG("CDBWorkThread::TimeoutCheck_ALLDB(), m_call table open fail");
		m_call.Close(); //All();
		return;
	}

	// call timeout check
	while (m_call.MoveNext() != DB_S_ENDOFROWSET) 
	{
		if (m_call.m_timeout >= IRR_INTERVAL * 3)
		{
			// Send CDR ///////////////////////////////////////////////////////////////////////
			if (m_call.m_connect_time.year != 1900)
			{
				CDRProcess(&m_call);

			//__DEBUG("CDBWork..::Time..ALLDB(), Remove clling:%s, clled:%s", m_call.m_calling, m_call.m_called);

			// Send ENDCALL message to GKTool
				char cnt[5] = "";
				DWORD duration = 0;
				if (m_call.m_connect_time.year != 1900)
				{
					duration = GetTimeDiff(m_call.m_connect_time);
					strcpy(cnt, "OK");
				}
				else
					strcpy(cnt, "NOK");

				StrTrim(m_call.m_calling, " ");
				StrTrim(m_call.m_calling_num, " ");
				StrTrim(m_call.m_called, " ");
				StrTrim(m_call.m_called_num, " ");

				__APPMSG(APPMSG_ENDCALL, "CALL|%ld|%s|%s|%s|%s|%s|255|255|%ld|;", duration, cnt, 
					m_call.m_calling, m_call.m_calling_num, m_call.m_called, m_call.m_called_num, m_call.m_crv);
			} 
			del_cnt++;
		}
		select_cnt++;
	} 
	m_call.Close();

	if (select_cnt > 0) 
	{
		if (del_cnt > 0)
		{
			del_cnt = 0;
			sprintf(sql, "delete call where timeout >= %d and registered_gk='%s'", IRR_INTERVAL * 3, GK_ID);
			hr = m_call.Open(m_call, sql, 0, &del_cnt, DBGUID_DBSQL, false, 0);
			m_call.Close();
		}

		sprintf(sql, "update call set timeout = timeout + %d where registered_gk='%s'", IRR_INTERVAL, GK_ID);
		hr = m_call.Open(m_call, sql, 0, &update_cnt, DBGUID_DBSQL, false, 0);
	}
	m_call.Close();

	//_DD("TimeoutCheck_DB, select_cnt:%d, update_cnt:%d, del_cnt:%d, end........", select_cnt, update_cnt, del_cnt);
}
*/

/*
void CDBWorkThread::LocationConfirm_DB(DWORD * srcHnd, LPRAS_ARQACF call_info)
{
	//_DD("LocationConfirm_DB, start");
	
	char sql[128] = "";
	DBROWCOUNT update_cnt = 0;
	
	sprintf(sql, "update call set called_ip='%s', signal_port=%ld where lrq_seq_num = %d and registered_gk = '%s'", 
		call_info->called_ip, call_info->called_port, call_info->request_number, GK_ID);
	HRESULT hr = m_call.Open(m_call, sql, 0, &update_cnt, DBGUID_DBSQL, false, 0);
	m_call.Close();	

	//_DD("LocationConfirm_DB, update seq:%ld, row:%d, hr:%lX", call_info->request_number, update_cnt, hr);

	sprintf(sql, "select * from call where lrq_seq_num = %d and registered_gk = '%s'", call_info->request_number, GK_ID);
	hr = m_call.Open(m_call, sql);
	if (m_call.MoveFirst() != DB_S_ENDOFROWSET)
	{
		StrTrim(m_call.m_called, " ");
		StrTrim(m_call.m_called_ip, " ");
		StrTrim(m_call.m_called_num, " ");
		StrTrim(m_call.m_calling, " ");
		StrTrim(m_call.m_calling_ip, " ");
		StrTrim(m_call.m_calling_num, " ");

		call_info->request_number = (WORD)m_call.m_thread_id;
		strcpy(call_info->called, m_call.m_called);
		strcpy(call_info->called_ip, m_call.m_called_ip);
		strcpy(call_info->src_num, m_call.m_calling_num);
		strcpy(call_info->calling, m_call.m_calling);
		strcpy(call_info->calling_ip, m_call.m_calling_ip);
		strcpy(call_info->dest_num, m_call.m_called_num);
		strcpy(call_info->rxaddr, m_call.m_calling_ip);
		call_info->rxport = m_call.m_arq_rx_port;
		call_info->Confirm = true;
		call_info->crv = (WORD)m_call.m_crv;
		strcpy(call_info->call_id, m_call.m_call_id);
		call_info->process_id = _UI16_MAX;
	}
	else
	{
		call_info->Confirm = false;
		//__DEBUG("CDBWork..::LocationConfirm_DB(), WARNING, not found call rec, clling:%s", call_info->calling);
	}

	m_call.Close();

	call_info->answer_call = false;

	::PostThreadMessage(*srcHnd, WM_RAS_LCCCFRM_RESULT, 0, LPARAM(call_info));

	//_DD("LocationConfirm_DB, end...");

	return;
}
*/

void CDBWorkThread::CheckCall_DB(DWORD * srcHnd, LPRAS_IRR irr_info)
{
	//_DD("CheckCall_DB, srcHnd:%ld, crv:%d, clling:%d, start", *srcHnd, irr_info->crv, irr_info->calling);

	char sql[128] = "";

	sprintf(sql, "select * from call where calling='%s' and crv=%d and call_id='%s'", 
		irr_info->calling, irr_info->crv, irr_info->call_id);
	
	// HRESULT hr = m_call.Open(m_call, sql);
	BOOL r = m_db.OpenSQL(sql);

	if (m_db.MoveFirst() == DB_S_ENDOFROWSET)
	{
		//__DEBUG("CDBWorkThread::TimeOutCheck_DB(), Rx CheckCall from CallThread and NOT FOUND CALLREC");
		::PostThreadMessage(*srcHnd, WM_CALLINFO_NOT_FOUND, 0, 0);
	}

	delete irr_info;

	m_db.CloseSQL();

	// m_call.Close();
	//_DD("CheckCall_DB, srcHnd:%ld, hr:%lX, end..", *srcHnd, irr_info->crv, irr_info->calling, hr);
}


void CDBWorkThread::CDRProcess(LPCALL_INFO call_info)
{
	//_DD("CDRProcess, start..");

	DBTIMESTAMP t;
	LPCDR cdr = new CDR;
	GetTimeDB(t);

	memset(cdr, 0, sizeof(CDR));
	strcpy(cdr->call_id, call_info->call_id);
	cdr->crv = call_info->crv;

// 2004. 06. 08. 트렁크 연동 국제 이동 무료전화 수정
// calling h323id 가 CONST_TID 일 경우 billing에서 요금 계산 가능 하도록
// h323id + calling_num 형태로 변환 한다.

// 2004. 08. 16 trve 트렁크GW 추가
#if defined(GK_FOR_MH161) || defined(GK_FOR_MH162) || defined(GK_FOR_M3TEL)
	StrTrim(call_info->calling, " ");
	if (strcmp(call_info->calling, CONST_TID) == 0)
	{
		strcpy(cdr->calling, CONST_TID);
		strcat(cdr->calling, call_info->src_num);
	}
	else if (strcmp(call_info->calling, CONST_TID2) == 0)
	{
		strcpy(cdr->calling, CONST_TID2);
		strcat(cdr->calling, call_info->src_num);
	}
	else
		strcpy(cdr->calling, call_info->calling);
#else
	strcpy(cdr->calling, call_info->calling);
#endif


	strcpy(cdr->calling_ip, call_info->calling_ip);
	strcpy(cdr->src_num, call_info->src_num);
	strcpy(cdr->called, call_info->called);
	strcpy(cdr->called_ip, call_info->called_ip);
	strcpy(cdr->dest_num, call_info->dest_num);
	cdr->release_complete = call_info->release_cause;

	strcpy(cdr->stime, call_info->cnt_time);
	sprintf(cdr->etime, "%d-%d-%d %d:%d:%d", t.year, t.month, t.day, 
		t.hour, t.minute, t.second);

	//_DD("CDRProcess, LPCALL_INFO, clling:%s, clled:%s, destnum:%s, end..", cdr->calling, cdr->called, cdr->dest_num);

	CCDR::Instance()->SendCDR(cdr);

	return;
}


bool CDBWorkThread::GetTimeDB(DBTIMESTAMP & dbts)
{
	CTime c = CTime::GetCurrentTime();
	dbts.year = c.GetYear();
	dbts.month = c.GetMonth();
	dbts.day = c.GetDay();
	dbts.hour = c.GetHour();
	dbts.minute = c.GetMinute();
	dbts.second = c.GetSecond();
	dbts.fraction = 0;

	return true;
}

DWORD CDBWorkThread::GetTimeDiff(DBTIMESTAMP dbtime)
{
	time_t e_time;
	time(&e_time);

	struct tm temp_time;
	temp_time.tm_year = dbtime.year - 1900;
	temp_time.tm_mon = dbtime.month - 1;
	temp_time.tm_mday = dbtime.day;
	temp_time.tm_hour = dbtime.hour;
	temp_time.tm_min = dbtime.minute;
	temp_time.tm_sec = dbtime.second;

	temp_time.tm_yday = 0;
	temp_time.tm_wday = 0;
	temp_time.tm_isdst = 0;
	
	time_t s_time = mktime(&temp_time);

	DWORD diff = (DWORD)difftime(e_time, s_time);
	
	return diff;
}

WORD CDBWorkThread::GetLRQRequestSeqNum()
{
	if (_UI16_MAX == m_LRQRequestSeqNum)
		m_LRQRequestSeqNum = 3000;
	else
		m_LRQRequestSeqNum++;

	return m_LRQRequestSeqNum;
}

void CDBWorkThread::TEST_DB()
{
	/*
	char sql[128] = "";
	sprintf(sql, "select * from call where crv = %d", 5840);
	HRESULT hr = m_call.Open(m_call, sql);

	hr = m_call.MoveFirst();

	DWORD diff = GetTimeDiff(m_call.m_connect_time);

	sprintf(sql, "time:%lu", diff);

	m_call.Close();

	*/
	//AfxMessageBox(sql);

	//m_reg.OpenDataSource();
	//m_reg.CloseAll();
	//int b = 0;
	
	//HRESULT hr 
}

void CDBWorkThread::_DD(char * m, ...)
{
	char buff[512] = "";
	va_list va; 

	va_start(va, m);
	vsprintf(buff, m, (va_list)va);
	va_end(va);

	if (m_debugline == 0) 
	{
		DWORD b = GetTickCount();
		CString f;
		f.Format("%sDBLOG_%lu.txt", m_logpath, b);
		m_file.Open(f, CFile::modeWrite | CFile::modeCreate | CFile::shareDenyNone);
	}

	strcat(buff, "\r\n");
	CString t;
	t.Format("%lu::", GetTickCount());
	m_file.Write(t, t.GetLength());
	m_file.Write(buff, (UINT)strlen(buff));
	m_debugline++;

	if (m_debugline > 30000) 
	{
		m_debugline = 0;
		m_file.Close();
	}
}
