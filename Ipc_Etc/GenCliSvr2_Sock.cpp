//
// GenCliSvr2_Sock.cpp
//

#include "stdafx.h"
#include "StdSys.h"
#include "Direct.h"
#include "msg.h"
#include "GenCliSvr2_Sock.h"
#include "GenCliSvr2_Proto.h"
#include "WinSockErrors.h"
#include <stdio.h>
#define _PROMPT_RCV
#define _PROMPT_SEND
#define _ONTIMER_SEND
#define _ONTIMER_RCV

static BOOL bChatty = 0; // Beware "static"

#define _MyTimerInterval_ms 0xA	// This is the minimum timer interval, see documentation for SetTimer

LPCTSTR getHostAddr(LPCTSTR hostName)
{
	struct hostent *h = gethostbyname(hostName);
	if(!h)
	{
		warn("Can not find hostaddr for hostName=",hostName);
		return NULL;
	}
	ASSERT(h);
	static char buf[16];

	unsigned char * addr = (unsigned char *)h->h_addr_list[0];
	if(!addr)
	{
		ASSERT(0);
		return NULL;
	}

	unsigned u[4];
	for(int i=0;i<4;i++)
		u[i] = (unsigned)addr[i];
	sprintf(buf,"%d.%d.%d.%d",
		u[0],u[1],u[2],u[3]);

	//TRACE(_T("HostAddr: %s\n"),buf);
	return buf;

	// If you do not wish to hardcode the port numbers into the application, 
	// your code can add the port numbers it uses to the SERVICES file 
	// (under Windows NT, that file would typically be in the %system_root%\drivers\etc directory) 
	// and use the getservbyname function to retrieve the port number at run time.	
}

LPCSTR getLocalhostAddr(void)
{
	return getHostAddr("localhost");
}

static void CALLBACK GenCliSvr2Sock_OnTimer(HWND h, UINT a, UINT_PTR b, DWORD c)
{
	CGenCliSvr2_Sock *This = (CGenCliSvr2_Sock *)(b - 191);
	This->OnTimer();
}
 

CGenCliSvr2_Sock::CGenCliSvr2_Sock(HWND parent)
: CMyAsyncSocket(parent)
, m_BurstModeOptimization(0)
, m_bInOnTimer(0)
, m_bAutoDeleteReq(0)
{
	ASSERT(_CrtCheckMemory());
	m_sendBuf = NULL;
	m_rcvBuf = NULL;

	m_sendBufSz = 0;
	m_rcvBufSz = 0;
	m_sendNSent = 0;
	m_sendNPending = 0;
	m_sendNFree = 0;
	m_rcvNGot = 0;
	m_rcvNPending = 0;
	m_bRcvingHdr = 0;
	m_ptrSend[0] = 0;
	m_ptrSend[1] = 0;
	m_ptrRcv = 0;

	m_sendNFree = -1;
	m_iProto = 0;
	m_b_I_Am_So_Outta_Here = FALSE;
	m_bConnected = FALSE;
	BOOL iTrue = TRUE;
	BOOL iFalse = FALSE;

	SetSockOpt(TCP_NODELAY,   &iTrue, sizeof(iTrue), IPPROTO_TCP);
	SetSockOpt(SO_REUSEADDR,  &iFalse, sizeof(iTrue));
	SetSockOpt(SO_EXCLUSIVEADDRUSE,  &iTrue, sizeof(iTrue));

	SetSockOpt(SO_DONTLINGER, &iTrue, sizeof(iTrue));
	SetSockOpt(SO_KEEPALIVE,  &iTrue, sizeof(iTrue));

	InitializeCriticalSection(&m_sendQueueCritSec);
	
	// If uElapse is less than USER_TIMER_MINIMUM (0x0000000A), the timeout is set to USER_TIMER_MINIMUM. 
	UINT_PTR This = (UINT_PTR)this;
	UINT_PTR Parent = (UINT_PTR)parent;

#if 0
	CString aa;
	aa.Format("CGenCliSvr2_Sock: This=0x%x Parent=0x%x", (UINT)This, (UINT)Parent);
	TRACE("\r\n%s\r\n\r\n",aa);
#endif

	m_uiTimerId = ::SetTimer(parent, ((UINT_PTR)this) + 191 , _MyTimerInterval_ms, GenCliSvr2Sock_OnTimer );
	int kkk=0;

	ASSERT(_CrtCheckMemory());
}

CGenCliSvr2_Sock::~CGenCliSvr2_Sock()
{
	::KillTimer(m_parent, m_uiTimerId);

	m_b_I_Am_So_Outta_Here = TRUE;	
	Close();
	if(m_sendBuf)
		free(m_sendBuf);
	if(m_rcvBuf)
		free(m_rcvBuf);
}

#include "MMSystem.h"


void CGenCliSvr2_Sock::OnTimer(void)
{
	if(m_bInOnTimer)
		return;
	m_bInOnTimer=1;
	static int first=1;  // BEWARE "static" many apps have two or more instances of this object
	if(first)
	{
		first=0;
		timeBeginPeriod(1); // Sets resolution of "Sleep()" to "1"
	}
	
	unsigned t0 = Millitime();
	for(int k=0; k < 1000 * _MyTimerInterval_ms ; k++)
	{
		if(m_b_I_Am_So_Outta_Here)
		{
			m_bInOnTimer=0;
			return;
		}

		::CMyAsyncSocket::OnTimer(t0, m_sendNPending); // This should block for 1 ms, well, "should" -- I think so, but I'm not certain
		
		unsigned t1 = Millitime();
		unsigned dt = t1 - t0;
		if(dt >= _MyTimerInterval_ms)
			break;
		if(m_bAutoDeleteReq)
			break;

	}
//	timeEndPeriod(10);	// Restore resolution of "Sleep()"
	if(m_bAutoDeleteReq)
	{
		m_bAutoDeleteReq = 0;
		ASSERT(_CrtCheckMemory()); // In OnDisconLo

		delete this;
		ASSERT(_CrtCheckMemory()); // In OnDisconLo
		return; // do NOT set m_bInOnTimer=0, 
	
	}
	m_bInOnTimer=0;
}


void CGenCliSvr2_Sock::OnConnectionSuccess(BOOL bOk)
{
	m_bConnected = bOk;
}

void CGenCliSvr2_Sock::OnClose(int ecode)
{
	// Application must override this
	// m_parent->OnDisconLo(m_iParentIdx, m_dwTag, ecode);
	m_b_I_Am_So_Outta_Here = TRUE;
}

void CGenCliSvr2_Sock::OnRcvLo(char * buf, size_t sz)
{
	// Application must override this
	// m_parent->OnDisconLo(m_iParentIdx, m_dwTag, ecode);
}

void CGenCliSvr2_Sock::OnReceive(int ecode)
{
	// Virtual override of CAsyncSocket; this is the notification
	// that data can be read.
#ifdef _PROMPT_RCV
	DoRcv(ecode);
#endif
}

void CGenCliSvr2_Sock::DoRcv(int ecode)
{
#if 0 // Silly GOOSE! You can't call msg in DoRcv -- you will recurse recurse
	if(bChatty)
		msg("SERVER: OnReceive: ENTRY: ecode=",ecode);
#endif 

	if(ecode) // Ecode is 0, WSAENETDOWN
	{
		// Slow down the diagnostics
		static int s_iDiagCnt = 0;	// BEWARE of "static" many apps have multiple instances of this object
		static UINT s_uiLastDiagTime = 0;
		if(!s_iDiagCnt++)
		{
			s_uiLastDiagTime = _time32(NULL);
		}else{
			UINT uiNow = _time32(NULL);
			UINT dt = uiNow - s_uiLastDiagTime;
			if(dt > 1)
			{
				s_uiLastDiagTime = uiNow;
				// msg("SERVER: OnReceive: ecode=",szWinSockErrName(ecode)); // Silly GOOSE! You can't call msg in DoRcv -- you will recurse recurse
			}
		}
		return;
	}
	
	//while(1)
	for(int uu=0;uu<1;uu++)
	{
		if(m_b_I_Am_So_Outta_Here)
			return;

		if(bChatty && 0) // Silly GOOSE! You can't call msg in DoRcv -- you will recurse recurse
		{
			if(m_bRcvingHdr)
				msg("SERVER: OnReceive: READING HDR: nBytesToRead=", (int) m_rcvNPending);
			else
				msg("SERVER: OnReceive: READING DATA: nBytesToRead=",(int) m_rcvNPending);
		}

		char temp[100];
		//size_t n = Receive(m_ptrRcv, 1999);
		size_t n = Receive(m_ptrRcv, m_rcvNPending);
		for (int i = 0; i < m_rcvNPending; i++)
		{
			temp[i] = m_ptrRcv[i];
		}
		//size_t n = Receive(temp, sizeof(temp));
		CString bb;
		bb.Format(m_ptrRcv);
		msg(bb);


		if(bChatty)
			msg("SERVER: Return from Receive: nread=",(int)n);

		if(!n)
		{
			ASSERT(_CrtCheckMemory()); 

			Close();
			//OnClose(ecode);

			ASSERT(_CrtCheckMemory());

			break;	// Either no data or socket has been closed
		}
		if((int)n == SOCKET_ERROR)
		{
			DWORD dwErr = GetLastError();
			CString csErrName = szWinSockErrName(dwErr);
			if(dwErr == WSAENOTSOCK || dwErr == WSAECONNRESET)
			{
				// Oh We are Sooo Gone!
				// This is apparently the "normal" operation if the server's application code 
				// calls "Close(iClientId)"
#if 0 
				//
				// MessageId: WSAECONNRESET
				//
				// MessageText:
				//
				// An existing connection was forcibly closed by the remote host.
				//
#define WSAECONNRESET                    10054L

#endif 

				ASSERT(_CrtCheckMemory());

				OnClose(ecode); // Ooooh  this routine may have deleted me!

				ASSERT(_CrtCheckMemory());

				break;
			}

			if(dwErr == WSAEWOULDBLOCK || dwErr == 0)
			{
				n = 0;
			}else{
				
				if(bChatty && 0	) // Silly GOOSE! You can't call msg in DoRcv -- you will recurse recurse
					msg("SERVER: OnReceive: error is NOT WSAEWOULDBLOCK: err=",csErrName);
				//voops("OnReceive: Receive");
			}
			break;
		}

		m_ptrRcv += n;
		m_rcvNPending -= n;
		m_rcvNGot += n;

		if(!m_rcvNPending)
		{
			CliSvr2Hdr *h = (CliSvr2Hdr *)m_rcvBuf;
			if(m_bRcvingHdr)
			{
				if(bChatty)
				{
					msg("SERVER: Reading Hdr: Got Hdr: It Looks Like:");
					msg("SERVER:   Magic=", (h->magic==m_uiMagic ? "GOOD" : "BAD MAGIC"));
					msg(_T("SERVER:   h->sz="), (int)h->ttlSz);
				}

				unsigned uiMagic = LOWORD(h->magic);
				DWORD flags = HIWORD(h->magic);

				
				if(flags == _CliSvrSendFlag_DisconNotice)
				{
					// This is a close notification
					Close();
					return;
				}
				ASSERT(uiMagic == m_uiMagic);

				ASSERT(h->ttlSz > 0 && h->ttlSz <= m_rcvBufSz);
				// An industrial strength implementation would disconnect the client at these error
				if(uiMagic != m_uiMagic || h->ttlSz <= 0 || h->ttlSz > m_rcvBufSz)
				{
					if(bChatty)
						msg("SERVER: Magic wrong or sz wrong; closing the socket");

					Close();
					return;
				}
				m_rcvNPending = h->ttlSz - sizeof(CliSvr2Hdr);
				m_rcvNGot = 0;
				m_bRcvingHdr = FALSE;
			}else{
				if(m_iProto == 0)
				{
					OnRcvLo(m_rcvBuf + sizeof(*h), h->msgSz);
				}
				else if(m_iProto == 1)
				{
					ASSERT(0); // Not supported
					if(bChatty)
						msg(_T("SERVER: Have rcvd all data for this msg, passing it to next layer, sz="),(int) h->ttlSz);

					// Here's the error:
					// The application blocked via AppMessageBox() in the dispatch of the message;
					// this happens within "OnRcvLo", ff
					// and while in MessageBox of course we process messages,
					// including "OnRecieveDataAvailable"
					// but we have m_rcvNPending==0!
					// So suppose I enqueue a dozen messages? Will that help?

					// Also, suppose I add the "discard policy" for the JCM messages? Will that help?
					OnRcvLo(m_rcvBuf, h->ttlSz);
				}else{
					ASSERT(0);
				}
				setupForRcv();
			}
		}
	}
}

size_t CGenCliSvr2_Sock::getSendNPending(void)
{
	if( ! m_bConnected || m_b_I_Am_So_Outta_Here)
	{
		return (size_t)(-1);
	}
	return m_sendNPending;
}

size_t CGenCliSvr2_Sock::getSendNFree(void)
{
	if( ! m_bConnected || m_b_I_Am_So_Outta_Here)
	{
		return 0;
	}
	return m_sendNFree;
}


size_t CGenCliSvr2_Sock::send(const void *buf1, size_t sz1, const void *buf2, size_t sz2, DWORD dwFlags)
{
	CliSvr2Hdr Hdr;
	CliSvr2Trailer Trailer;

	size_t iret = send3a(&Hdr, sizeof(Hdr), buf1, sz1, buf2, sz2, &Trailer, sizeof(Trailer), dwFlags);
	if(iret <= 0)
		return iret;

	return (sz1 + sz2);
}


size_t CGenCliSvr2_Sock::send3a(CliSvr2Hdr *hdr, size_t hdrSz, const void *buf1, size_t sz1, const void *buf2, size_t sz2, CliSvr2Trailer *trailer, size_t trlSz, DWORD dwFlags)
{
	ASSERT(hdrSz == sizeof(CliSvr2Hdr));
	ASSERT(trlSz == sizeof(CliSvr2Trailer));

	if(!m_bConnected)
	{
		ASSERT(0);
		TRACE("CGenCliSvr2_Sock::send3(): not connected yet!"); // This will recurse in some implementations, msg calls send..
		return -1;
	}
//	OnSend(0);
	size_t msgSz = sz1 + sz2;
	size_t ttlSz = hdrSz + msgSz + trlSz;

	ASSERT(m_sendBufSz);

	if(ttlSz > m_sendBufSz)
	{
		// This is clearly not going to work with this architecture!
		ASSERT(0);
		// This would recurse on the stack		warn("Client/Server: attempt to send message whose size is greater than the max msg size declared at startup");
		return 0;
	}
	
	if(ttlSz > m_sendNFree)
	{
		//	ASSERT(0);	// Tell me, it's not fatal, but I'd like to know
	//	OnSend(0);	// This looks redundandant, but it's an easy place to debug the OnSend() routine
		return -1;
	}
	
	CliSvr2Hdr *h = (CliSvr2Hdr *)hdr;
	h->magic = MAKELONG( m_uiMagic, dwFlags);

	h->ttlSz = (unsigned)ttlSz;
	h->hdrSz = (unsigned)hdrSz;
	h->msgSz = (unsigned)msgSz;
	
	CliSvr2Trailer *t = (CliSvr2Trailer *)trailer;
	t->magic = m_uiMagic + 1;

	// Now enqueue the data
	EnterCriticalSection(&m_sendQueueCritSec);
		ASSERT(m_sendNFree >= hdrSz);
		memcpy(m_ptrSend[1], hdr, hdrSz);
		m_ptrSend[1]   += hdrSz;
		m_sendNPending += hdrSz;
		m_sendNFree    -= hdrSz;



		if(sz1)
		{
			ASSERT(m_sendNFree >= sz1);
			memcpy(m_ptrSend[1], buf1, sz1);
			m_ptrSend[1]   += sz1;
			m_sendNPending += sz1;	
			m_sendNFree    -= sz1;
		}

		if(sz2)
		{
			ASSERT(m_sendNFree >= sz2);
			memcpy(m_ptrSend[1], buf2, sz2);
			m_ptrSend[1]   += sz2;
			m_sendNPending += sz2;

			m_sendNFree    -= sz2;
		}

		ASSERT(m_sendNFree >= trlSz);
		memcpy(m_ptrSend[1], trailer, trlSz);
		m_ptrSend[1]   += trlSz;
		m_sendNPending += trlSz;
		m_sendNFree    -= trlSz;

	

	LeaveCriticalSection(&m_sendQueueCritSec);
	OnSend(0);
	return ttlSz;
}


void CGenCliSvr2_Sock::OnSend(int ecode)
{
	// virtual: Override of base class CASyncSock OnSend.  
	// This is our notification method that data can be sent

#ifdef _PROMPT_SEND
	DoSend(ecode);
#endif
}

void CGenCliSvr2_Sock::DoSend(int ecode)
{
	if(ecode == 0)
	{
		// we are called by user after sending data.  This is high-throughput for burst mode only; 
		// we cannot allow this normally, else we'll get WSAECONNABORTED 
		if(! m_BurstModeOptimization)
			return;
	}
	if(ecode == -2)
	{
		// We are called from CMyAsyncSocket::OnTimer after select -- this is the only reliable context
		ecode = 0;
	}
	if(ecode)
	{
		msg("OnSend: ecode=",szWinSockErrName(ecode));
		return;
	}

	if(m_b_I_Am_So_Outta_Here)
		return;	// Sorry

	if(!m_sendNPending)
		return;

	while(m_sendNPending)
	{
#if 0 // Silly goose! You can't call msg in DoSend -- you will recurse
		if(bChatty)
		{
			int *p  = (int *)m_ptrSend[0];
			CString a;
			a.Format(_T("SERVER: OnSend(after hdr set): magic= [0x%x] sz=[%d] msgId=[%d]    sz=%d"), p[0],p[1],p[2],m_sendNPending);
			msg(a);
		}
#endif 
		EnterCriticalSection(&m_sendQueueCritSec);
		  size_t n = Send(m_ptrSend[0], m_sendNPending);
		
			if( n && n != SOCKET_ERROR)
			{
				m_ptrSend[0] += n;
				m_sendNPending -= n;
				m_sendNSent++; // Never gets reset -- good for debugging!
				if(!m_sendNPending)
					setupForSend();
			}

		LeaveCriticalSection(&m_sendQueueCritSec);
		if(!n)
			break;
		if(n == SOCKET_ERROR)
		{
			DWORD dwErr = GetLastError();
			if(dwErr == WSAEWOULDBLOCK)
			{
				break;
			}else if(dwErr == WSAECONNABORTED)
			{
				// He bailed on me!
				Close();
				break;
			
			}else{
				m_b_I_Am_So_Outta_Here = TRUE;
				// warn("OnSend: got error from send:",szWinSockErrName(dwErr));
				{
					static int cnt=0; // Beware "static"
					if(cnt++ > 4)
						ASSERT(0);
				}
				Close();
				break;
			}
		}
	}
}

void CGenCliSvr2_Sock::alloc(size_t maxSzSend, size_t maxSzRcv)
{
	maxSzSend +=  sizeof(CliSvr2Hdr) + sizeof(CliSvr2Trailer);
	maxSzRcv  +=  sizeof(CliSvr2Hdr) + sizeof(CliSvr2Trailer);

	m_sendBuf = (char *)malloc(maxSzSend);
	m_rcvBuf  = (char *)malloc(maxSzRcv);
	m_sendBufSz = maxSzSend;
	m_rcvBufSz  = maxSzRcv;
	
	setupForSend();
	setupForRcv();
}

void CGenCliSvr2_Sock::setupForSend(void)
{
	m_sendNSent = 0;
	m_sendNPending = 0;
	m_ptrSend[0] = m_ptrSend[1] = m_sendBuf;
	m_sendNFree = (int) m_sendBufSz;	
}

void CGenCliSvr2_Sock::setupForRcv(void)
{
	// PRIVATE:
	//	Synopsis: Reset pointers to begin another rcv cycle
	if(m_b_I_Am_So_Outta_Here)
		return;
	m_rcvNGot = 0;
	m_rcvNPending = sizeof(CliSvr2Hdr);
	m_bRcvingHdr = TRUE;
	m_ptrRcv = m_rcvBuf;	
	CliSvr2Hdr *h = (CliSvr2Hdr *)m_rcvBuf;
	h->magic = 0;
	h->ttlSz = 0;
}

void CGenCliSvr2_Sock::voops(char const *a)
{
	// "void oops"
	DWORD x = GetLastError();
	CString aa = a;
	msg(aa,_T("ERR="), szWinSockErrName(x));
}

int CGenCliSvr2_Sock::oops(char const *a)
{
	// "oops" and return -1, for code convenience
	voops(a);
	return -1;
}

// EOF