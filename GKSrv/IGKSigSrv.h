// IGKSigSrv.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// �� ��ȣ


// CIGKSigSrvApp:
// �� Ŭ������ ������ ���ؼ��� IGKSigSrv.cpp�� �����Ͻʽÿ�.
//

class CIGKSigSrvApp : public CWinApp
{
public:
	CIGKSigSrvApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

extern CIGKSigSrvApp theApp;
