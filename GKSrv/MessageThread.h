#pragma once


#include "GKConst.h"
// CMessageThread

class CMessageThread : public CWinThread
{
	DECLARE_DYNCREATE(CMessageThread)

protected:
	CMessageThread();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CMessageThread();
	
	static CMessageThread * m_instance;
	CAsyncSocket * m_msgsock;

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	static CMessageThread * Instance();
	//void SendMsg(LPAPP_MESSAGE msg);
	void SendAppMessage(LPAPP_MESSAGE msg);

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	void SendMsg(LPAPP_MESSAGE msg);
};


inline void __APPMSG(int msgType, char * x, ...)
{
	char buff[256] = "";

	LPAPP_MESSAGE msg = new APP_MESSAGE;
	memset(msg, 0, sizeof(APP_MESSAGE));

	va_list va;
	va_start(va, x);
	vsprintf(buff, x, (va_list)va);
	va_end(va);

	msg->type = msgType;
	strcpy(msg->gk_name, GK_ID);
	strcpy(msg->message, buff);

	CMessageThread::Instance()->SendAppMessage(msg);
} 


