#pragma once

#include "GKConst.h"

// CCDR

class CCDR : public CWinThread
{
	DECLARE_DYNCREATE(CCDR)

protected:
	CCDR();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CCDR();
	
	UINT m_year;
	UINT m_day;
	UINT m_month;
	UINT m_hour;
	UINT m_minute;
	UINT m_second;

	CFile m_cdrfile;

	void WriteCDRToText(LPCDR cdr);
	bool CreateNewFile();
	bool m_active;
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	static CCDR * Instance();
	void SendCDR(LPCDR cdr);
protected:
	DECLARE_MESSAGE_MAP()
	static CCDR * m_instance;
	CAsyncSocket m_cdrsock;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


