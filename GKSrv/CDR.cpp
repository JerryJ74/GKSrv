// CDR.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "InterGK.h"
#include "CDR.h"
//#include "SockDebug.h"
#include "GKConst.h"

// CCDR

IMPLEMENT_DYNCREATE(CCDR, CWinThread)

CCDR * CCDR::m_instance = NULL;

CCDR::CCDR()
{
	::CreateDirectory("CDR\\", NULL);
	m_active = false;
	m_active = CreateNewFile();


	//if (!(m_active = CreateNewFile()))
		//__DEBUG("CCDR::CCDR(), CDR File Create fail..");


	if (!(m_cdrsock.Create(0, SOCK_DGRAM)))
	{
		//__DEBUG("CCDR::CCDR(), Can't Create cdr socket.. ");
	}
	m_instance = this;
}

CCDR::~CCDR()
{
}

CCDR * CCDR::Instance()
{
	return m_instance;
}


BOOL CCDR::InitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	return TRUE;
}

int CCDR::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
	m_cdrfile.Flush();
	m_cdrfile.Close();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCDR, CWinThread)
END_MESSAGE_MAP()


// CCDR 메시지 처리기입니다.

BOOL CCDR::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	
	switch(pMsg->message)
	{
		case WM_CDR: 
		{
			WriteCDRToText(LPCDR(pMsg->lParam));
		}
	}

	return CWinThread::PreTranslateMessage(pMsg);
}

void CCDR::WriteCDRToText(LPCDR cdr)
{
	CTime t = CTime::GetCurrentTime();

	if (m_day != t.GetDay())
		m_active = CreateNewFile();

	/*
	StrTrim(cdr->calling, " ");
	StrTrim(cdr->calling_ip, " ");
	StrTrim(cdr->src_num, " ");
	StrTrim(cdr->called, " ");
	StrTrim(cdr->called_ip, " ");
	StrTrim(cdr->dest_num, " ");
	*/

	// 
	//if (cdr->src_num[0] == "")
	//	strcpy(cdr->src_num, "NULL");

	char c[256] = "";
	sprintf(c, "%s|%s|%s|%s|%s|%s|%s|%s|%d|;\r\n", 
		cdr->calling, cdr->calling_ip, cdr->src_num,
		cdr->called, cdr->called_ip, cdr->dest_num, 
		cdr->stime, cdr->etime, cdr->release_complete);

	m_cdrsock.SendTo(c, (int)strlen(c), CDR_PORT, CDR_ADDRESS);


	// 2004. 04. 14
	// CDR 2개 전송 하는 부분.
	// TNI, BIT 일때 2개 전송 한다.

	// 수정 2004. 2. 22 
	// CDR을 2개 보낼 수 있도록 옵션화 하여 수정 한다.
	//#define DUAL_CDR					Config::Instance()->GET_ENABLEDUALCDR()
	//#define DUAL_CDR_ADDR				Config::Instance()->GET_DUAL_CDRADDR()
	if (DUAL_CDR)
	{
		sprintf(c, "%s|%s|%s|%s|%s|%s|%s|%s|%d|%s|;\r\n", 
			cdr->calling, cdr->calling_ip, cdr->src_num,
			cdr->called, cdr->called_ip, cdr->dest_num, 
			cdr->stime, cdr->etime, cdr->release_complete, GK_ID);

		m_cdrsock.SendTo(c, (int)strlen(c), CDR_PORT, DUAL_CDR_ADDR);
	}

	if (m_active)
		m_cdrfile.Write(c, (UINT)strlen(c));
	//else
	//	__DEBUG("CCDR::WriteCDRToText(), WARNING!! :: CDR write FAIL, Can't Create new file");

	delete cdr;
}


bool CCDR::CreateNewFile()
{
	CTime t = CTime::GetCurrentTime();

	m_year = t.GetYear();
	m_day = t.GetDay();
	m_month = t.GetMonth();
	m_hour = t.GetHour();
	m_minute = t.GetMinute();
	m_second = t.GetSecond();

	char filename[50] = "";

	if (m_active)
	{
		m_cdrfile.Flush();
		m_cdrfile.Close();
	}

	sprintf(filename, "CDR\\CDR_%d_%04d%02d%02d_%02d%02d%02d.txt", SIGNAL_PORT, m_year, m_month, m_day, m_hour, m_minute, m_second);
	if (m_cdrfile.Open(filename, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
		return true;
	else
		return false;
}

void CCDR::SendCDR(LPCDR cdr)
{
	PostThreadMessage(WM_CDR, 0, LPARAM(cdr));
}
