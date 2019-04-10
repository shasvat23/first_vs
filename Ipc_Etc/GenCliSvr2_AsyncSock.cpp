//
// GenCliSvr2_AsyncSock.cpp
//
//	This is a replacement for the rather buggy (?)
//	CAsyncSocket() in mfc.
//	The implementation should be pretty easy to understand
//
//	The problem all Winsock's have is that BSD sockets
//	relied on signals to signify async operations, e.g. select().
//  CASyncSocket() in MFC tried to bridge this, and it
//	works most of the time; the problems occur
//	when there is heavy network activity and a socket goes down.
//	Why do sockets go down in heavy network activity?
//	Well, this *is* Windows, no?
//	Anyway, my app tried to reconnect, and CAsyncSocket ASSERTS right and left.
//	Internet chat rooms strongly suggest abandoning CAsyncSocket
//	So here's the life-raft....Worked at least five minutes for one of my apps...

//	Note you need to pass an HWND to the constructor.  This allows us to setup our timers "Properly"

// Note the derived class MUST call my OnTimer -- about every 10 ms

#include "Stdafx.h"
#include "StdSys.h"
#include "GenCliSvr2_AsyncSock.h"
#include "WinSockErrors.h"
#include "msg.h"

static int s_bInitDone = 0;

#define MyAsyncSocket_Magic 171211

#ifdef MyAsyncSocket_PassthroughTo_CASyncSocket // Enable this in the .h file prn

CMyAsyncSocket::CMyAsyncSocket(	HWND parent )
	: m_bInDestructor(0)
{
	m_iMagic = MyAsyncSocket_Magic;
	ASSERT(_CrtCheckMemory());
	
}
CMyAsyncSocket::~CMyAsyncSocket( )
{
	m_iMagic = 0;
	
}

#else // MyAsyncSocket_PassthroughTo_CASyncSocket

static void TellSockErr(const char *tag)
{
	DWORD dwErr = WSAGetLastError();
	CString aa = szWinSockErrName(dwErr);
	warn("Select failed:", aa);
	/*
	TRACE("%s: select failed: err=%s (%d)\n",tag, szWinSockErrName(dwErr), dwErr);
	ASSERT(0);
	*/
}

CMyAsyncSocket::CMyAsyncSocket(	HWND parent )
	: m_hSocket(INVALID_SOCKET)
	, m_iMagic(0)
	, m_lEvent(0)
	, m_holdoff(0)
	, m_bInDestructor(0)
	, m_bInOnTimer(0)

{

	ASSERT(_CrtCheckMemory());

	m_hSocket = INVALID_SOCKET;
	m_iMagic = MyAsyncSocket_Magic;

	m_parent = parent;
	m_lEvent = 0;
	m_id = -1;
	m_bListenInProgress = 0;
	m_bConnectInProgress = 0;
	 
	FD_ZERO(&m_fdsRead);
	FD_ZERO(&m_fdsWrite);
	FD_ZERO(&m_fdsExcept);

	if(!s_bInitDone)
	{
		s_bInitDone = 1;

		WSAData D;
		WORD ver = MAKEWORD(1,1);
		int iret = WSAStartup(ver,&D);
		if(iret != 0)
			TellSockErr("WsaStartup");
		ASSERT(iret == 0);
	}

	ASSERT(_CrtCheckMemory());
}

CMyAsyncSocket::~CMyAsyncSocket()
{
	m_bInDestructor=1;

	if (m_iMagic == MyAsyncSocket_Magic && m_hSocket != INVALID_SOCKET)
		Close();	

	m_iMagic = 0;
}


#if 1 // Re-write to block on select for rcv and except after send ?
void CMyAsyncSocket::OnTimer(unsigned uiNow, size_t sendNPending)
{
	// Could you put a timeout on connect pending ? // m_bConnectInProgress
	if(m_bInOnTimer)
		return;
	m_bInOnTimer=1;

	// Just poll select()
	if(m_iMagic != MyAsyncSocket_Magic || m_hSocket == INVALID_SOCKET)
	{
		m_bInOnTimer = 0;
		return;
	}

	if (! m_lEvent )
	{
		m_bInOnTimer = 0;
		return;
	}

	if(m_holdoff)
	{
		m_holdoff--;
		m_bInOnTimer=0;
		return;
	}

	int bNeedToCheckFdsWrite = (m_bConnectInProgress || sendNPending);

	while(1)
	{
		static struct timeval Timeout = {0,1000 };

		// Performance: 5000 -> 60
		// 3000 50/70
		// 2000 varies, more or less same as 3000

		// WARNING: If you set this to 1000, then after 20 seconds we get WSAECONNABORTED, 
		// which means the Tcp "ack" msgs are not getting through
		// If you set it to 2000, then if you have a PSD display active, we get WSAECONNABORTED

		FD_ZERO(&m_fdsRead);
		FD_ZERO(&m_fdsWrite);
		FD_ZERO(&m_fdsExcept);

		FD_SET(m_hSocket, &m_fdsExcept);
		FD_SET(m_hSocket, &m_fdsRead);

		if(bNeedToCheckFdsWrite)
			FD_SET(m_hSocket, &m_fdsWrite);

		int nfds = 1; // Ignored in windows, compatibility and all that
		int n =	select(nfds, &m_fdsRead, &m_fdsWrite, &m_fdsExcept, &Timeout);

		if(!n)
			break;

		if(n == SOCKET_ERROR)
		{
			TellSockErr("select()");
			Close();
			break;
		}

		if(FD_ISSET(m_hSocket, &m_fdsExcept))
		{
			if(m_bConnectInProgress)
			{
				m_bConnectInProgress = 0;
				OnConnect(-1);
			}else{
				OnExcept(0);
			}
		}

		if(FD_ISSET(m_hSocket, &m_fdsRead))
		{
			if(m_bListenInProgress)
			{
				m_holdoff = 10;
				OnAccept(0);
			}else{
				OnReceive(0);
			}
		}

		if(m_iMagic != MyAsyncSocket_Magic || m_hSocket == INVALID_SOCKET)
		{
			m_bInOnTimer = 0;
			return;
		}

		if(bNeedToCheckFdsWrite)
		{
			if(FD_ISSET(m_hSocket, &m_fdsWrite))
			{
				if(m_bConnectInProgress)
				{
					m_bConnectInProgress = 0;
					m_holdoff = 10;
					OnConnect(0);
				}else{
					OnSend(-2);		// Special flag to tell "OnSend" where he was called from
				}
			}
		}

		break;
	}	
	m_bInOnTimer = 0;
}

#endif 


BOOL CMyAsyncSocket::Socket(int nSocketType/*=SOCK_STREAM*/, long lEvent 
		/* = 	FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE*/,
		int nProtocolType/* = 0*/, int nAddressFormat /*= PF_INET*/)
{

	ASSERT(m_hSocket == INVALID_SOCKET);
	m_hSocket = socket(nAddressFormat,nSocketType,nProtocolType);

	if(m_hSocket == INVALID_SOCKET)
	{
		ASSERT(0);	// I'd like to know
		return FALSE;
	}
	u_long yes=1;
	int iret = ioctlsocket(m_hSocket,FIONBIO, &yes);


	return TRUE;
}

#if 0 // MFC
BOOL CAsyncSocket::Socket(int nSocketType, long lEvent,
						  int nProtocolType, int nAddressFormat)
{
	ASSERT(m_hSocket == INVALID_SOCKET);
	
	m_hSocket = socket(nAddressFormat,nSocketType,nProtocolType);
	if (m_hSocket != INVALID_SOCKET)
	{
		CAsyncSocket::AttachHandle(m_hSocket, this, FALSE);
		return AsyncSelect(lEvent);
	}
	return FALSE;
}
#endif

BOOL CMyAsyncSocket::Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress/* = NULL*/)
{
	ASSERT(sizeof(TCHAR) == sizeof(char));	// 6/5/2010 DSL: porting from CAsyncSocket, KISS

	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));
	
	LPSTR lpszAscii = (LPTSTR)lpszSocketAddress;	// Cast TSTR to STR
	sockAddr.sin_family = AF_INET;
	
	if (lpszAscii == NULL)
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
	{
		DWORD lResult = inet_addr(lpszAscii);
		if (lResult == INADDR_NONE)
		{
			WSASetLastError(WSAEINVAL);
			return FALSE;
		}
		sockAddr.sin_addr.s_addr = lResult;
	}
	
	sockAddr.sin_port = htons((u_short)nSocketPort);
	
//	return Bind((SOCKADDR*)&sockAddr, sizeof(sockAddr)); // bind()

	int iret = bind(m_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));

	return (iret == 0);
}

BOOL CMyAsyncSocket::Create(UINT nSocketPort/* = 0*/, int nSocketType/*=SOCK_STREAM*/,
			long lEvent /*= FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE*/,
			LPCTSTR lpszSocketAddress/* = NULL*/)
{
	if(m_iMagic != MyAsyncSocket_Magic)
	{
		TRACE("CMyAsyncSocket::Create: bad magic\n");
		return FALSE;
	}

	if(m_hSocket != INVALID_SOCKET)
	{
		//	ASSERT(0);
		Close();
	}

	/*
	 MSDN "Binding a Socket
	 iRes = bind(sock, result->ai_addr, (int)result->ai_addrlen)
	 // sockaddr struct returned from getaddrinfo
	 Once the bind function is called, the address information returned by the getaddrinfo function is no longer needed. The freeaddrinfo function is called to free the memory allocated by the getaddrinfo function for this address information.
	getaddrinfo
	https://msdn.microsoft.com/en-us/library/windows/desktop/ms738520(v=vs.85).aspx
	*/

	BOOL b = Socket(nSocketType, lEvent);
	if(!b)
	{
		warn("\"Socket\" failed -- network is very sick!");
		return FALSE;
	}

	b = Bind(nSocketPort,lpszSocketAddress);
	if(b)
		return TRUE;

	static int cnt=0;
	cnt++;
	DWORD dwErr = GetLastError(); // 10049 is WSAEADDRNOTAVAIL
	Close();
	WSASetLastError(dwErr);

	CString aa;
	aa.Format("\"Bind\" Failed (cnt=%d) (addr=%s) Err=%s", cnt, lpszSocketAddress, szWinSockErrName(dwErr));
	msg(aa);

#if 0
		PADDRINFOA result;
		int iret = getaddrinfo("DesertSolitaire", "1292", NULL, &result);
		int ires = Bind(nSocketPort, result->ai_addr);
		if(ires)
			return TRUE;
#endif 
	
	return FALSE;
}

BOOL CMyAsyncSocket::Listen(int nConnectionBacklog /* = 5 */)
{
	m_bListenInProgress = 1;
	int iret = listen(m_hSocket, nConnectionBacklog);
	return (iret == 0);
}

#if 0 // MFC 
BOOL CAsyncSocket::Accept(CAsyncSocket& rConnectedSocket,
						  SOCKADDR* lpSockAddr, int* lpSockAddrLen)
{
	ASSERT(rConnectedSocket.m_hSocket == INVALID_SOCKET);
	ASSERT(CAsyncSocket::FromHandle(INVALID_SOCKET) == NULL);
	
	CAsyncSocket::AttachHandle(INVALID_SOCKET, &rConnectedSocket);
	
	SOCKET hTemp = accept(m_hSocket, lpSockAddr, lpSockAddrLen);
	
	if (hTemp == INVALID_SOCKET)
	{
		DWORD dwProblem = GetLastError();
		CAsyncSocket::DetachHandle(rConnectedSocket.m_hSocket, FALSE);
		rConnectedSocket.m_hSocket = INVALID_SOCKET;
		SetLastError(dwProblem);
	}
	else if (CAsyncSocket::FromHandle(INVALID_SOCKET) != NULL)
	{
		rConnectedSocket.m_hSocket = hTemp;
		CAsyncSocket::DetachHandle(INVALID_SOCKET, FALSE);
		CAsyncSocket::AttachHandle(hTemp, &rConnectedSocket);
	}
	
	return (hTemp != INVALID_SOCKET);
}
#endif 

BOOL CMyAsyncSocket::Accept(CMyAsyncSocket& rConnectedSocket,
					SOCKADDR* lpSockAddr /*= NULL*/, int* lpSockAddrLen /*= NULL*/)
{
	ASSERT(rConnectedSocket.m_hSocket == INVALID_SOCKET);
	
	SOCKET hTemp = accept(m_hSocket, lpSockAddr, lpSockAddrLen);

	if (hTemp == INVALID_SOCKET)
		return FALSE;

	rConnectedSocket.m_hSocket = hTemp;
	return TRUE;
}

BOOL CMyAsyncSocket::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
	ASSERT(sizeof(TCHAR) == sizeof(char));	// 6/5/2010 DSL: porting from CAsyncSocket, KISS

	ASSERT(lpszHostAddress != NULL);
	
	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));
	
	LPSTR lpszAscii = (LPTSTR)lpszHostAddress;	// Cvt TSTR to STR

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(lpszAscii);
	
	
	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost;
		lphost = gethostbyname(lpszAscii);
		if (lphost != NULL)
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
		{
			WSASetLastError(WSAEINVAL);
			return FALSE;
		}
	}
	
	sockAddr.sin_port = htons((u_short)nHostPort);
	
	m_bConnectInProgress = 1;
	int iret = connect(m_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
	return (iret == 0);
}

BOOL CMyAsyncSocket::SetSockOpt(int nOptionName, const void* lpOptionValue,
				int nOptionLen, int level /* = SOL_SOCKET */)
{
	int iret = setsockopt(m_hSocket, level, nOptionName, (const char *)lpOptionValue,  nOptionLen);
	return (iret == 0);
}

BOOL CMyAsyncSocket::AsyncSelect(long lEvent)
{
	// Spawn a thread, to hang on select()
	// Hard part is getting it shut down properly
	// Need some object, then how to wait for both the select
	// and the object to tell it to shutdown?
	// perhaps a timeout on the select();
	m_lEvent = lEvent;
	return TRUE;
}

void CMyAsyncSocket::Close()
{
	FD_ZERO(&m_fdsRead);
	FD_ZERO(&m_fdsWrite);
	FD_ZERO(&m_fdsExcept);
	m_lEvent = 0;
	if (m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
		if(!m_bInDestructor)
			OnClose(0);
	}
}

size_t CMyAsyncSocket::Send(const void* lpBuf, size_t nBufLen, int nFlags)
{
	return (size_t) send(m_hSocket, (LPSTR)lpBuf, (int)nBufLen, nFlags);
}

size_t CMyAsyncSocket::Receive(void* lpBuf, size_t nBufLen, int nFlags)
{
	size_t iret = (size_t)recv(m_hSocket, (LPSTR)lpBuf, (int)nBufLen, nFlags);
	// If the connection has been gracefully closed, the return value is zero.
	// Otherwise, a value of SOCKET_ERROR is returned, and a specific error code can be retrieved by calling WSAGetLastError.

	// If the socket is connection oriented and the remote side has shut down the connection gracefully, 
	// and all data has been received, a recv will complete immediately with zero bytes received.
	



	return iret;
}

void CMyAsyncSocket::OnAccept(int ecode)
{
	
}

void CMyAsyncSocket::OnConnect(int ecode)
{
	
}


void CMyAsyncSocket::OnExcept(int ecode)
{
	// Probably not needed.
//	ASSERT(0); // ?? I'd like to know...
}

#endif // #else // MyAsyncSocket_PassthroughTo_CASyncSocket