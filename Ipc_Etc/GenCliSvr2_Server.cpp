//
// GenCliSvr2_Server.cpp
//
#include "stdafx.h"
#include "Direct.h"
#include "msg.h"
#include "GenCliSvr2_Server.h"
#include "GenCliSvr2_Proto.h"
#include "GenCliSvr2_Sock.h"
#include "WinSockErrors.h"
#include "AfxSock.h"
#include "StringEx.h"
#include "StdSys.h"

//////////////////////////////////////////////////////////////////////////
// CClientSock: : Public CGenCliSvr2_Sock
//////////////////////////////////////////////////////////////////////////
class CClientSock : public CGenCliSvr2_Sock
{
public:
	CClientSock(HWND parent);
	virtual ~CClientSock();
	
protected:
	virtual void OnClose(int ecode);
	virtual void OnRcvLo( char *, size_t);
	
public:
	CGenSvr2 * m_parent;
	int   m_iParentIdx;
	DWORD_PTR m_dwTag;	
};


CClientSock::CClientSock(HWND parent)
	: CGenCliSvr2_Sock(parent)
	, m_parent(NULL)
	, m_iParentIdx(0)
	, m_dwTag(0)
{
	
}

CClientSock::~CClientSock()
{
	
}

void CClientSock::OnClose(int ecode)
{
	// Virtual override of CGenCliSvr2_Sock::OnClose, which is a NOP
	m_parent->OnDisconLo(m_iParentIdx, m_dwTag, ecode);
}

void CClientSock::OnRcvLo(char * buf, size_t sz)
{
	// Virtual override of CGenCliSvr2_Sock::OnRcvLo, which is a NOP
	m_parent->OnRcv(m_iParentIdx, m_dwTag, buf,sz);
}

//////////////////////////////////////////////////////////////////////////
// CGenSvr2 : Public CAsyncSock
//////////////////////////////////////////////////////////////////////////

// typedef VOID (CALLBACK* TIMERPROC)(HWND, UINT, UINT_PTR, DWORD);
static void CALLBACK GenServer_OnTimer(HWND hw, UINT a, UINT_PTR b, DWORD c)
{
	CGenSvr2 *This = (CGenSvr2 *)(b - 277);
	This->OnTimer();
}

CGenSvr2::CGenSvr2(HWND parent)
: CMyAsyncSocket(parent)
{
	
	m_nClients = 0;
	m_iProto = 0;

	for(int i=0;i<MaxClients;i++)
		m_C[i] = NULL;
	m_parent = parent;

	int myTimerInterval = 10;

	UINT_PTR This = (UINT_PTR)this;
	UINT_PTR Parent = (UINT_PTR)parent;

	CString aa;
	aa.Format("CGenSvr2: This=0x%x Parent=0x%x", (UINT)This, (UINT)Parent);
	TRACE("\r\n%s\r\n\r\n",aa);

	m_uiTimerId = ::SetTimer(parent, ((UINT_PTR)this) + 277, myTimerInterval, ::GenServer_OnTimer);
}
 
CGenSvr2::~CGenSvr2()
{
	ASSERT(_CrtCheckMemory());

	if(m_uiTimerId)
		::KillTimer(m_parent, m_uiTimerId);

	for(int i=0;i<MaxClients;i++)
	{
		if(m_C[i])
		{
			// ???? m_C[i]->Close();
			delete m_C[i];
			m_C[i] = NULL;
		}
	}

	ASSERT(_CrtCheckMemory());

}

void CGenSvr2::OnTimer(void)
{
	ASSERT(_CrtCheckMemory());

	CMyAsyncSocket::OnTimer(0,0);

	ASSERT(_CrtCheckMemory());

}


void CGenSvr2::voops(char *a)
{
	// "void oops"
	DWORD x = GetLastError();
	CString aa = a;
	msg(aa,_T("ERR="), szWinSockErrName(x));
}

int CGenSvr2::oops(char *a)
{
	// "oops" and return -1, for code convenience
	voops(a);
	return -1;
}

void CGenSvr2::SetProtocolOpt(int iProto)
{
	ASSERT(iProto==0);
	m_iProto = iProto;
}


BOOL  CGenSvr2::StartService(int port,  const char *hostAddr, size_t maxSendSz, size_t maxRcvSz, unsigned uiMagic )
{
	long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT  | FD_CLOSE;
	//long lEvent = FD_ACCEPT;

	CString szSockAddr;

	if(!hostAddr)
	{
		// Call getHostName
		char localHostName[MAX_PATH];
		gethostname(localHostName, ARRAYSIZE(localHostName));
	}

	if(!strcmp(hostAddr, "localhost"))
	{
		szSockAddr = getLocalhostAddr(); // This returns a special entity that is NOT visible outside this machine
	}

	else 
	{
		szSockAddr = getHostAddr(hostAddr);
		if(szSockAddr.IsEmpty())
			return FALSE;	// getHostAddr() should be chatty enough
	}

	
	m_maxSzSend = maxSendSz;
	m_maxSzRcv = maxRcvSz;
	m_uiMagic = uiMagic;

	AfxSocketInit();
	
	BOOL b = Create(port, SOCK_STREAM, lEvent, szSockAddr);
	if(!b)
		return b; // "Create" is chatty in the event of error

	b = AsyncSelect(lEvent);
	if(!b)
	{
		warn("StartService: Select failed");
		return 0;
	}

	b = Listen();
	if(!b)
	{
		warn("StartService: Listen failed");
		return 0;
	}

	return b;
}


void CGenSvr2::OnClose(int ecode)
{
	int kkk=0;
}

void CGenSvr2::OnRcvLo(char *, size_t)
{
	int kkk=0;
}

void CGenSvr2::OnReceive(int ecode)
{
	int kkk=0;
}
	
void CGenSvr2::OnSend(int ecode)
{
	int kkk=0;
	
}


size_t CGenSvr2::send(int iClientId, const void *b1, size_t sz1, const void *b2, size_t sz2, DWORD flags)
{
	size_t sz = sz1 + sz2;
	if(iClientId == ALL_CLIENTS)
	{
		int cnt=0;
		for(int i=0;i<MaxClients;i++)
		{
			if(m_C[i] && m_C[i]->m_bConnected)
			{
				m_C[i]->send(b1, sz1, b2, sz2, flags);
				cnt++;
			}
		}
		return cnt;
	}else{
		if(m_C[iClientId])
			return m_C[iClientId]->send(b1, sz1, b2, sz2, flags);
		else
			return 0;
	}
}

size_t CGenSvr2::get_send_freespace(int iClientId)
{
	if(iClientId == ALL_CLIENTS)
	{
		size_t minFreespace = 0;
		int cnt=0;
		for(int i=0;i<MaxClients;i++)
		{
			if(m_C[i])
			{
				size_t x = m_C[i]->getSendNFree();
				if(!cnt++)
					minFreespace = x;
				else
					minFreespace = MIN(minFreespace, x);
			}
		}
		return minFreespace;
	}else{
		size_t x = m_C[iClientId]->getSendNFree();
		return x;
	}
}

size_t CGenSvr2::send3a(int iClientId, CliSvr2Hdr *hdr, size_t hdrSz, const void *buf, size_t bufSz, const void *b2, size_t sz2, CliSvr2Trailer *trailer, size_t trlSz, DWORD flags)
{
	if(iClientId == ALL_CLIENTS)
	{
		for(int i=0;i<MaxClients;i++)
		{
			if(m_C[i])
				m_C[i]->send3a(hdr,hdrSz,buf,bufSz, b2,sz2, trailer, trlSz, flags);
		}
		return bufSz;
	}else{
		if(m_C[iClientId])
			return m_C[iClientId]->send3a(hdr,hdrSz,buf,bufSz,b2,sz2, trailer, trlSz, flags);
		else
			return 0;
	}
}

int CGenSvr2::getNumConnections(void)
{
	return m_nClients;
}

#include "Mstcpip.h"
void CGenSvr2::OnAccept(int ecode)
{	
	// virtual, an override of CAsyncSocket
	// Called by the framework to notify a listening socket 
	//	that it can accept pending connection requests by 
	//	calling the Accept member function

	
	ASSERT(_CrtCheckMemory());

	CString badNews;
	if(ecode)
	{
		msg("OnAccept: ecode=",szWinSockErrName(ecode));
		return;
	}

	// Get Slot
	CClientSock *c = NULL;
	BOOL bGotIt = FALSE;
	int iClientIdx = 0;
	for(iClientIdx=0; iClientIdx < MaxClients; iClientIdx++)
	{
		if(!m_C[iClientIdx])
		{
			bGotIt = TRUE;
			break;
		}
	}
	if(!bGotIt)
	{
		msg("OnAccept: NO ROOM FOR MORE CLIENTS");
		return;
	}
	
	ASSERT(_CrtCheckMemory());

	// Get Client Socket object
	c = m_C[iClientIdx] = new CClientSock(m_parent);

	ASSERT(_CrtCheckMemory());


	// Now the Accept
	SOCKADDR clientAddr;
	int addrsz = sizeof(clientAddr);
	BOOL b = Accept(*c, &clientAddr, &addrsz);
	if(!b)
	{
		if(m_C[iClientIdx])	{ delete m_C[iClientIdx];m_C[iClientIdx] = NULL;	}
		voops("OnAccept::Accept");
		return;
	}
	
	// Decode the addr!
	unsigned char u[4];
	for(int kk=0;kk<4;kk++)
		u[kk] = (unsigned char)clientAddr.sa_data[kk];

	CString csAddr;
	csAddr.Format(_T("%03d.%03d.%03d.%03d"), u[2],u[3],u[0],u[1]);

	TRACE(_T("Got Accept: addr=%s\n"),csAddr);

	// Condition the new socket (created by Accept) for the async operations we care about
	// BTW: Here is your stack:
	/*

  CGenSvr2::OnAccept(int 0) line 307
  CAsyncSocket::DoCallBack(unsigned int 1700, long 8) line 539
  CSocket::ProcessAuxQueue() line 823
  CSocketWnd::OnSocketNotify(unsigned int 1700, long 8) line 1127
  CWnd::OnWndMsg(unsigned int 883, unsigned int 1700, long 8, long * 0x0012fcc4) line 1826 + 17 bytes
  CWnd::WindowProc(unsigned int 883, unsigned int 1700, long 8) line 1596 + 30 bytes
  AfxCallWndProc(CWnd * 0x108a6d40 {CSocketWnd hWnd=0x001c101a}, HWND__ * 0x001c101a, unsigned int 883, unsigned int 1700, long 8) line 215 + 26 bytes
  AfxWndProc(HWND__ * 0x001c101a, unsigned int 883, unsigned int 1700, long 8) line 379
  AfxWndProcBase(HWND__ * 0x001c101a, unsigned int 883, unsigned int 1700, long 8) line 220 + 21 bytes
  USER32! 7e418734()
  USER32! 7e418816()
  USER32! 7e4189cd()
  USER32! 7e4196c7()
  CWinThread::PumpMessage() line 853
  CWinThread::Run() line 487 + 11 bytes
  CWinApp::Run() line 400
  AfxWinMain(HINSTANCE__ * 0x00400000, HINSTANCE__ * 0x00000000, char * 0x00141f0b, int 1) line 49 + 11 bytes
  WinMain(HINSTANCE__ * 0x00400000, HINSTANCE__ * 0x00000000, char * 0x00141f0b, int 1) line 30
  WinMainCRTStartup() line 330 + 54 bytes
  KERNEL32! 7c817067()

  */


#if 1 // Set the keepalive timeout to 10 seconds
	{
		/* Argument structure for SIO_KEEPALIVE_VALS */
#if 0
		struct tcp_keepalive {
			u_long  onoff;
			u_long  keepalivetime;
			u_long  keepaliveinterval;
		};
#endif

	DWORD iTrue=1;
	SetSockOpt(SO_KEEPALIVE,  &iTrue, sizeof(iTrue));

	struct tcp_keepalive K;
	memset(&K,0,sizeof(K));
	K.onoff = 1;
	K.keepalivetime = 10 * 1000;
	K.keepaliveinterval = 1 * 1000;

	DWORD dwBytesReturned=0;
	int iret = WSAIoctl(
		m_hSocket,
		SIO_KEEPALIVE_VALS,
		&K,
		sizeof(K),
		NULL,
		0,
		&dwBytesReturned,

		NULL,
		NULL);

	

	if(iret != 0)
	{
		DWORD dwLasterr = WSAGetLastError();
		CString aa = szWinSockErrName(dwLasterr);
		warn("KEEPALIVE options setting failed:", aa);
	}
	}
#endif 
	


	long lEvent = FD_READ | FD_WRITE | FD_CLOSE;
	b = c->AsyncSelect(lEvent);
	if(!b)
	{
		if(m_C[iClientIdx])	{ delete m_C[iClientIdx];m_C[iClientIdx] = NULL;	}
		voops("OnAccept::AsyncSelect");
		return;
	}


	// Call the application-level "OnConnection" to retrieve application-level "dwTag"
	DWORD_PTR dwTag = 0;
	int istat = OnConnection(csAddr,clientAddr, addrsz, iClientIdx, &dwTag);
	if(istat < 0)
	{
		if(m_C[iClientIdx])	{ delete m_C[iClientIdx];m_C[iClientIdx] = NULL;	}
		voops("OnAccept::OnConnection");
		return;
	}
	c->OnConnectionSuccess(TRUE);
	
	// Save the info in "c" that we'll need for future processing

	c->m_parent = this;
	c->m_dwTag = dwTag;
	c->m_iParentIdx = iClientIdx;
	c->m_csAddr = csAddr; // Nice for debugging ?? Maybe??
	c->m_uiMagic = m_uiMagic;
	c->m_iProto = m_iProto;

	// Finally, tell the po' bloke to allocate his iobuffers
	ASSERT(m_maxSzSend);
	ASSERT(m_maxSzRcv);

	c->alloc(m_maxSzSend, m_maxSzRcv);

	m_nClients++;

}

void CGenSvr2::OnDisconLo(int iClientIdx, DWORD_PTR dwTag, int ecode)
{
	if(iClientIdx < 0)
	{
		ASSERT(0);
		return;
	}
	if(! m_C[iClientIdx]  )
	{
		//ASSERT(0);  // Already bn done
		return;
	}

	if(m_C[iClientIdx]->m_iParentIdx != iClientIdx)
	{
		ASSERT(0);
		return;
	}

	if(m_C[iClientIdx]->m_dwTag != dwTag)
	{
		ASSERT(0);
		return;
	}

	ASSERT(_CrtCheckMemory()); // In OnDisconLo
	if(m_nClients > 0)
		m_nClients--;
	OnDisconnect(iClientIdx, dwTag, ecode);

	ASSERT(_CrtCheckMemory()); // In OnDisconLo

#if 1 // You can't delete me! I'm in an OnTimer routine right now -- disconnect dispatch is from my OnTimer
	m_C[iClientIdx]->m_bAutoDeleteReq++;
#else 
	delete(m_C[iClientIdx]); // You can't delete me! I'm in an OnTimer routine right now -- disconnect dispatch is from my OnTimer
#endif 
	m_C[iClientIdx] = NULL;
	
}

DWORD_PTR CGenSvr2::getClientData(int iClientIdx)
{
	if(! m_C[iClientIdx]) 
		return 0;

	if ( m_C[iClientIdx]->m_iParentIdx != iClientIdx)
	{
		ASSERT(0);
		return 0;
	}

	return m_C[iClientIdx]->m_dwTag;
}

int CGenSvr2::disconnect(int iClientId)
{
	// Just disconnects the poor bloke.
	// dwClientId is the tag you gave to OnConnection.

	// Is this going to trigger the OnClose of the client, which
	// will go back to our OnDisconLo, which
	// will fail the assert(m_C[iClientIdx])
	// MUST TEST THIS
	if(m_C[iClientId])
	{
		send(iClientId, NULL, 0, NULL, 0, _CliSvrSendFlag_DisconNotice);
		if(m_C[iClientId]) // Might get closed when we do the send...
			m_C[iClientId]->Close();
	}
	else
	{
		// Keep clam	ASSERT(0);
	}


	// Should we call OnDisconnect? Only if the OnClose doesn't trigger.
	return 0;
}

void CGenSvr2::DisconnectAllClients(void)
{
	for(int i=0;i<MaxClients;i++)
	{
		if(m_C[i])
		{
			disconnect(i);
		}
	}
}




// EOF