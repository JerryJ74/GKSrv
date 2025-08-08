// IGKSigSrvDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "IGKSigSrv.h"
#include "IGKSigSrvDlg.h"

#include "DBWorkThread.h"
#include "CallSignalServerThread.h"
#include "CDR.h"
#include "MessageThread.h"
#include "GKConst.h"
#include ".\igksigsrvdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CIGKSigSrvDlg ��ȭ ����



CIGKSigSrvDlg::CIGKSigSrvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIGKSigSrvDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIGKSigSrvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIGKSigSrvDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


// CIGKSigSrvDlg �޽��� ó����

BOOL CIGKSigSrvDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	if (__argv[1] == NULL)
	{
		AfxMessageBox("FATAL ERROR!!! Check Option");
		CDialog::OnCancel();
		return FALSE;
	}

	if (atoi(__argv[1]) == 0)
	{
		AfxMessageBox("FATAL ERROR!!! Option 0 is Invalid Value");
		CDialog::OnCancel();
		return FALSE;
	}

	InitializeCriticalSection(&m_critical);

	m_cfg = new Config();
	m_cfg->ReloadConfig();

	// GKConfig Create
	//CGKConfig * config = new CGKConfig();

	// gk signal server�� ���� ���ڰ� ���ڰ� �ƴϸ� ���� ���� �ƴ� �Ѵ�.
	// �̶� ���� ���ڴ� 1720 ~ 1729 ������ �Ѵ�.
	if (!m_cfg->SetSignalPort(__argv[1]))
	{
		AfxMessageBox("GK SignalServer run under GKService Manager!");
		CDialog::OnCancel();
		return FALSE;
	}

	char windowtext[30];
	sprintf(windowtext, "IGKSigSrv_%d", SIGNAL_PORT);
	this->SetWindowText(LPCTSTR(windowtext));

	m_endecoder = new H323EnDecoder;

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_db = (CWinThread *)AfxBeginThread(RUNTIME_CLASS(CDBWorkThread), THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED);
	m_db->m_bAutoDelete = true;
	m_db->ResumeThread();

	// 2003. 11. 03 
	// CallSignalThread�� ��Ȱ�� �������� �ʴ� ��찡 �߻� �Ͽ� Thread�� Priority �� ���� ������
	m_callsignal = (CWinThread *)AfxBeginThread(RUNTIME_CLASS(CCallSignalServerThread), THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED);
	m_callsignal->m_bAutoDelete = true;
	m_callsignal->ResumeThread();	

	m_cdr = (CWinThread *)AfxBeginThread(RUNTIME_CLASS(CCDR), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	m_cdr->m_bAutoDelete = true;
	m_cdr->ResumeThread();

	m_msg = (CWinThread *)AfxBeginThread(RUNTIME_CLASS(CMessageThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	m_msg->m_bAutoDelete = true;
	m_msg->ResumeThread();


	// DISPLAY DIALOG GK INFO
	char info[64] = "";
	sprintf(info, "SERVICE IP:%s , PORT:%d", LOCAL_IPADDRESS, SIGNAL_PORT);
	SetDlgItemText(ID_IP_INFO, info);

	sprintf(info, "GK SIGNAL %s %s", GK_ID, BUILDSTR);
	SetDlgItemText(IDC_GK_INFO2, info);

	// 2004. 11. 25. �۾�ǥ���ٿ��� ������.
	this->ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);   

	//char arg[20];
	//char * arg = GetCommandLine();
	//AfxMessageBox(arg);

	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

void CIGKSigSrvDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CIGKSigSrvDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// ���߱�
        //::ShowWindow(GetSafeHwnd(), FALSE);  // <--- ��� �߰��Ͻø� �˴ϴ�...

		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
HCURSOR CIGKSigSrvDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//BOOL CIGKSigSrvDlg::PreTranslateMessage(MSG* pMsg)
//{
//	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
//
//	return CDialog::PreTranslateMessage(pMsg);
//}

void CIGKSigSrvDlg::OnTimer(UINT nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	// 2004. 11. 24 �ּ� 
	// �� Signal Thread�� (IRR_INTERVAL * 1000) milli second ���� Timer �̺�Ʈ�� �߻� �Ѵ�.
	// �� �̺�Ʈ�� Call Record�� �ִ��� �˻� �Ͽ� Call Record �� ���� �Ǿ��� ���
	// ���� ���� �Ѵ�.
	::PostThreadMessage(nIDEvent, WM_TIMER, 0, 0);
	//this->ShowWindow(SW_HIDE);

	CDialog::OnTimer(nIDEvent);
}

void CIGKSigSrvDlg::OnCancel()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	return;

	CDialog::OnCancel();
}

void CIGKSigSrvDlg::OnBnClickedButton1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CGKConfig::Instance()->ReloadConfig();
	m_cfg->ReloadConfig();
}


//BOOL CIGKSigSrvDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
//{
//	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
//
//	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
//}


BOOL CIGKSigSrvDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if (message == WM_CONFIG_RELOAD)
	{
		::EnterCriticalSection(&m_critical);
		m_cfg->ReloadConfig();
		::LeaveCriticalSection(&m_critical);
	}

	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

BOOL CIGKSigSrvDlg::DestroyWindow()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	::DeleteCriticalSection(&m_critical);
	delete m_cfg;

	return CDialog::DestroyWindow();
}
