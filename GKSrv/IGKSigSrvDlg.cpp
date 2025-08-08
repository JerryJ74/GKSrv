// IGKSigSrvDlg.cpp : 구현 파일
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


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
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


// CIGKSigSrvDlg 대화 상자



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


// CIGKSigSrvDlg 메시지 처리기

BOOL CIGKSigSrvDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

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

	// gk signal server의 실행 인자가 숫자가 아니면 실행 하지 아니 한다.
	// 이때 실행 인자는 1720 ~ 1729 까지로 한다.
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

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_db = (CWinThread *)AfxBeginThread(RUNTIME_CLASS(CDBWorkThread), THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED);
	m_db->m_bAutoDelete = true;
	m_db->ResumeThread();

	// 2003. 11. 03 
	// CallSignalThread가 원활이 생성되지 않는 경우가 발생 하여 Thread의 Priority 를 높게 설정함
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

	// 2004. 11. 25. 작업표시줄에서 가리기.
	this->ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);   

	//char arg[20];
	//char * arg = GetCommandLine();
	//AfxMessageBox(arg);

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CIGKSigSrvDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// 감추기
        //::ShowWindow(GetSafeHwnd(), FALSE);  // <--- 요놈만 추가하시면 됩니다...

		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CIGKSigSrvDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//BOOL CIGKSigSrvDlg::PreTranslateMessage(MSG* pMsg)
//{
//	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//
//	return CDialog::PreTranslateMessage(pMsg);
//}

void CIGKSigSrvDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// 2004. 11. 24 주석 
	// 각 Signal Thread는 (IRR_INTERVAL * 1000) milli second 마다 Timer 이벤트가 발생 한다.
	// 이 이벤트는 Call Record가 있는지 검사 하여 Call Record 가 삭제 되었을 경우
	// 콜을 해제 한다.
	::PostThreadMessage(nIDEvent, WM_TIMER, 0, 0);
	//this->ShowWindow(SW_HIDE);

	CDialog::OnTimer(nIDEvent);
}

void CIGKSigSrvDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	return;

	CDialog::OnCancel();
}

void CIGKSigSrvDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CGKConfig::Instance()->ReloadConfig();
	m_cfg->ReloadConfig();
}


//BOOL CIGKSigSrvDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
//{
//	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//
//	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
//}


BOOL CIGKSigSrvDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	::DeleteCriticalSection(&m_critical);
	delete m_cfg;

	return CDialog::DestroyWindow();
}
