// IGKSigSrvDlg.h : ��� ����
//

#pragma once

#include "H323EnDecoder.h"
#include "..\\CommonFiles\\Config.h"

// CIGKSigSrvDlg ��ȭ ����
class CIGKSigSrvDlg : public CDialog
{
// ����
public:
	CIGKSigSrvDlg(CWnd* pParent = NULL);	// ǥ�� ������

// ��ȭ ���� ������
	enum { IDD = IDD_IGKSigSrv_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����


// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:
	CWinThread * m_ras;
	CWinThread * m_db;
	CWinThread * m_callsignal;
	CWinThread * m_cdr;
	CWinThread * m_log;
	CWinThread * m_msg;
	
	//CWinThread * m_config;
	Config * m_cfg;

	H323EnDecoder * m_endecoder;
	CRITICAL_SECTION m_critical;
public:
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT nIDEvent);
protected:
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedButton1();
//	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	virtual BOOL DestroyWindow();
};
