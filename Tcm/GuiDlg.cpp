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
#include "SampleAppSvr.h"

#include "TcmGlobals.h"
#include "VibCtlPorts.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static CGuiDlg *me = 0;

CSampleAppSvr *g_svr = 0;

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
	ON_WM_QUERYENDSESSION()
	ON_WM_ENDSESSION()

	ON_EN_CHANGE(IDC_EDIT1, &CGuiDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CGuiDlg message handlers

/*
 * From MSDN OnQueryEndSession

  return value is Nonzero if an application can be conveniently shut down; otherwise 0.

  remarks:
    If any application returns 0, the Windows session is not ended.
	Windows stops calling OnQueryEndSession as soon as one application returns 0 
	and sends the WM_ENDSESSION message with a parameter value of FALSE 
	for any application that has already returned nonzero.
 */

BOOL CGuiDlg::OnQueryEndSession(void)
{
	// This is the FIRST STEP in windows shutdown -- we ask all tasks if it's ok to shut down
	// Return 1 if it's ok to shut down...  
	// That doesn't mean we'll be shutting down -- some other task might return 0
	// and the shutdown might be canceled
	return 1;
}



/*
 * From Msdn OnEndSession
 afx_msg void OnEndSession( BOOL bEnding );
 Parameters
	 bEnding
	Specifies whether or not the session is being ended. It is TRUE if the session is being ended; otherwise FALSE.
 Remarks
	 The framework calls this member function after the CWnd 
	 object has returned a nonzero value from a 
	 OnQueryEndSession member function call. 
	 The OnEndSession call informs the CWnd object whether the session is actually ending.
	 If bEnding is TRUE, Windows can terminate any time after
	 all applications have returned from processing this call. 
	 Consequently, have an application perform all tasks required for termination within OnEndSession.

 */
void CGuiDlg::OnEndSession(BOOL bEndingNowFolks)
{
	// This is the SECOND Step in windows shutdown
	// If you previously got "OnQueryEndSession" and you returned 1
	// and then someone else returned 0, then "bEndingNowFolks" will be FALSE;
	// that is your notification that the shutdown has been canceled.

	// Otherwise, bEndingNowFolks is .true., and this is your last chance to shut down.
	// You do not need to return immediately -- take some time if necessary to shut down,
	// but note this function is void -- no one cares what you might think about these things; we ARE going down.
}


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
	beginAcceptingConnections();


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
	if(g_svr)
		g_svr->OnTimer();	// This is the context in which he accepts connections
}

void CGuiDlg::beginAcceptingConnections(void)
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
	g_svr = new CSampleAppSvr(m_hWnd);

	// Get host and port -- these are from VibControlCommon/Sys/VibCtlPorts.ini
	// Use standard accessors from VibCtlPorts.h
	CString host = getVibCtlHost(physUnit);
	unsigned port = getVibControllerPort(lun);

	CString aa;
	aa.Format("Begin connection to host=%s port=%d", host,port);
	msg(aa);

	// The communications scheme needs to know the max size of the messages
	// So we define these in the "AppMsgs.h" file

	size_t maxRcvSz  = sizeof(uMaxMsgGuiToTcm); 
	size_t maxSendSz =  sizeof(uMaxMsgTcmToGui);

	// The communications scheme has a "magic number" in the hdr of all messages
	// and we use this for debugging.  Both client and server must specify this number

	unsigned uiMagic = _EagleSampleAppCliSvrMagicNumber;

	int timeoutSeconds = 12;

	g_bConnected = 0;
	g_bConnectionFailed = 0;
		// Set this so we can check for connectivity in OnTimer

	BOOL b = g_svr->StartService(port, host, maxSendSz, maxRcvSz, uiMagic);
	if( !b )
	{
		warn("Error from \"StartService()\"");
		return; // No recovery, sorry
	}

	msg("\"StartService()\" started...");


}

#include "StringEx.h"
void CGuiDlg::OnBnClickedOk()
{
	msg("Nop?");
}


void CGuiDlg::OnBnClickedCancel()
{
	
	delete g_svr;
	g_svr = 0;
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


void dispatchMsgFromClient(int iClientId, DWORD_PTR dwApplicationLevelClientData, void *buf, size_t sz)
{
	if(me)
		me->dispatchMsgFromClient(iClientId, dwApplicationLevelClientData, buf, sz);
}

void CGuiDlg::dispatchMsgFromClient(int iClientId, DWORD_PTR dwApplicationLevelClientData, void *buf, size_t sz)
{
		int *ptr = (int *)buf;
		int msgId = *ptr;
		switch(msgId)
		{
		case MsgId_EchoReq:
			{
				EchoMsg *m = (EchoMsg *)buf;
				m->msgId = MsgId_EchoAck;
				if(g_svr)
					g_svr->send(iClientId, buf,sz);
			}
			break;
		}
}