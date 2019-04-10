//
// GuiDlg.cpp
//

#include "stdafx.h"
#include "Gui.h"
#include "GuiDlg.h"
#include "afxdialogex.h"

#include "stdsys.h"
#include "msg.h"
#include "..\App_Spt\AppSpt.h"
#include "..\App_Spt\AppMsgs.h"
#include "SampleAppClient.h"

#include "GuiGlobals.h"
#include "VibCtlPorts.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static CGuiDlg *me = 0;

CSampleAppClient *g_client = 0;

int g_bConnectionInProgress=0;	// Managed by GUI
int g_bConnected = 0;			// Managed by SampleAppClient
int g_bConnectionFailed = 0;	// Managed by SampleAppClient


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
	BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}


BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString aa = AppSpt_getHelpAboutDisplayString();

	SetDlgItemText(IDC_LBL_ABOUT, aa);

	aa = AppSpt_getProductShortName();
	SetWindowText(aa);


	return TRUE;
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


CGuiDlg::CGuiDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGuiDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGuiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit1);
}

BEGIN_MESSAGE_MAP(CGuiDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CGuiDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CGuiDlg::OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT1, &CGuiDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CGuiDlg message handlers

BOOL CGuiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//////////////////////////////////////////////////////////////////////////
	// Add "About..." menu item to system menu.
	//////////////////////////////////////////////////////////////////////////
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX); 	// IDM_ABOUTBOX must be in the system command range.
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString csShortName =  AppSpt_getProductShortName();
		CString space = " ";
		CString aa = "About";
		aa += space + csShortName;
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, aa);
	}

	//////////////////////////////////////////////////////////////////////////
	// Icon
	//////////////////////////////////////////////////////////////////////////
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//////////////////////////////////////////////////////////////////////////
	// Generic Sonnet Features: msgSetStdout, main title
	//////////////////////////////////////////////////////////////////////////
	msgSetStdout(&m_edit1);

	CString aa = AppSpt_getMainTitle();
	SetWindowText(aa);

	me = this;	// This is me!

	//////////////////////////////////////////////////////////////////////////
	// App-dependent initialization here
	//////////////////////////////////////////////////////////////////////////
	static int xxx=0;
	SetTimer((UINT_PTR)&xxx, 100, NULL);
	beginConnectionToTcm();


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGuiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGuiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGuiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGuiDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(g_bConnectionInProgress && ! g_bConnectionFailed)
	{
		int b = g_client->bIsConnected();
		if(b)
		{
			g_bConnectionInProgress = 0;
			msg("Got connection to TCM");
		}
	}
	
}

void CGuiDlg::beginConnectionToTcm(void)
{
	int physUnit=0;	
		// Physical unit number -- will be 0 almost always (99.9% of the time)
		// But if you had TWO Eagles connected on your LAN, this could specify the second eagle,
		// via "physUnit=1"


	int lun=0;	
	// Logical unit number -- will be 0 almost always (99.9% of the time)
	// But if you had TWO sets of standard hardware on your Eagle
	// this could specify the second set of hardware, via its IoDevicesEagle.ini file
	

	// Create "g_client" -- the communications "client" object
	// It will communicate with "g_server" in the TCM
	g_client = new CSampleAppClient(m_hWnd);

	// Get host and port -- these are from VibControlCommon/Sys/VibCtlPorts.ini
	// Use standard accessors from VibCtlPorts.h
	CString host = getVibCtlHost(physUnit);
	unsigned port = getVibControllerPort(lun);

	CString aa;
	aa.Format("Begin connection to host=%s port=%d", host,port);
	msg(aa);

	// The communications scheme needs to know the max size of the messages
	// So we define these in the "AppMsgs.h" file

	size_t maxSendSz = sizeof(uMaxMsgGuiToTcm);
	size_t maxRcvSz =  sizeof(uMaxMsgTcmToGui);

	// The communications scheme has a "magic number" in the hdr of all messages
	// and we use this for debugging.  Both client and server must specify this number

	unsigned uiMagic = _EagleSampleAppCliSvrMagicNumber;

	int timeoutSeconds = 12;

	g_bConnected = 0;
	g_bConnectionFailed = 0;
	g_bConnectionInProgress = 1;
		// Set this so we can check for connectivity in OnTimer
	int istat = g_client->BeginConnection(host,port, maxSendSz, maxRcvSz, uiMagic, timeoutSeconds);
	if(istat < 0)
	{
		warn("Error from \"BeginConnection()\"");
		return; // No recovery, sorry
	}

	msg("\"BeginConnection()\" started...");


}

#include "StringEx.h"
void CGuiDlg::OnBnClickedOk()
{
	
	CString echo_msg = "Mary had a little lamb.... and who knows what the lamb had";
	EchoMsg M, *m = &M;
	memset(m,0,sizeof(*m));
	m->msgId = MsgId_EchoReq;
	strNcpy(m->buf, echo_msg, ARRAYSIZE(m->buf));
	if(g_client)
		g_client->send(m, sizeof(*m));
}


void CGuiDlg::OnBnClickedCancel()
{
	
	delete g_client;
	g_client = 0;
	CDialogEx::OnCancel();
}


void CGuiDlg::OnClose()
{
	OnBnClickedCancel();
}


void CGuiDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}



void dispatchMsgFromTcm(char *buf, size_t sz)
{
	if(me)
		me->dispatchMsgFromTcm(buf, sz);
}

void CGuiDlg::dispatchMsgFromTcm(char *buf, size_t sz)
{
	int *ptr = (int *)buf;
	int msgId = *ptr;

	switch(msgId)
	{
	case MsgId_EchoAck:
		{
			EchoMsg *m = (EchoMsg *)buf;
			msg("Got Echo Ack: buf=", m->buf);
		}
		break;
	}
}