#pragma once

/*
#include "..\CommonHeader\DB_Authentication.h"
#include "..\CommonHeader\DB_Registration.h"
//#include "ConCurrentCall.h"
#include "..\CommonHeader\Call.h"
#include "..\CommonHeader\DB_Prefixes.h"
#include "..\CommonHeader\GKConst.h"
*/

//#include <oledb.h>
#include "util.h"
#include "CDR.h"
#include "MyOleDB.h"
#include "GKConst.h"

// CDBWorkThread

class CDBWorkThread : public CWinThread
{
	DECLARE_DYNCREATE(CDBWorkThread)

protected:
	CDBWorkThread();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CDBWorkThread();
	
	//CDB_Registration m_reg;
	//CDB_Authentication m_auth;
	//CDB_Prefix m_prefix;
	//CDB_Call m_call;

	//CConCurrentCall m_currentcall;
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	
	//void RegistrationRequest(DWORD * srcHnd, void * pData);
	//void AdmissionRequest(DWORD * srcHnd, void * pData);
	//void InformationRequest(LPRAS_IRR irr_info);
	//void UnregistrationRequest(void * pData);
	//void LocationConfirm(DWORD * srcHnd, void * pData);
	//void LocationReject(DWORD * srcHnd, void * pData);

	void RxSetup(DWORD * srcHnd, void * pData);
	void RxReleaseComplete(DWORD * srcHnd, void * pData);
	void RxConnect(DWORD * srcHnd, void * pData);

	void CheckCall(DWORD * srcHnd, void * pData);

	static CDBWorkThread * Instance();

protected:
	//void RegistrationRequest_DB(DWORD * srcHnd, RAS_RRQRCF * Data);
	//void AdmissionRequest_DB(DWORD * srcHnd, RAS_ARQACF * Data);
	//void LocationConfirm_DB(DWORD * srcHnd, LPRAS_ARQACF call_info);
	//void LocationReject_DB(DWORD * srcHnd, LPRAS_LRJ lrj);
	//void InformationRequest_DB(LPRAS_IRR irr_info);
	//void UnregistrationRequest_DB(LPRAS_URQUCF urq);

	void RxSetup_DB(DWORD * srcHnd, LPCALL_INFO call_info);
	void RxReleaseComplete_DB(DWORD * srcHnd, LPCALL_INFO call_info);
	void RxConnect_DB(DWORD * srcHnd, LPCALL_INFO call_info);
	void TEST_DB();

	void CheckCall_DB(DWORD * srcHnd, LPRAS_IRR irr_info);
	// void TimeOutCheck_ALLDB();

	// Default Release Complete reason timeout
	// void CDRProcess(CDB_Call * call, BYTE release = 255);

	void CDRProcess(LPCALL_INFO call_info);

	//CDBPropSet m_propset;

	static CDBWorkThread * m_instance;

	bool GetTimeDB(DBTIMESTAMP & dbts);
	DWORD GetTimeDiff(DBTIMESTAMP dbtime);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	WORD m_LRQRequestSeqNum;
	WORD GetLRQRequestSeqNum();
	void _DD(char * m, ...);
	CFile m_file;
	DWORD m_debugline;

	MyOleDB m_db;

	char m_logpath[20];
};


