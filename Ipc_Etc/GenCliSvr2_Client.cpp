//
// GenCliSvr2_Client.cpp
//
#include "StdAfx.h"
#include "GenCliSvr2_Client.h"
#include "msg.h"
#include "AfxSock.h"
#include "WinSockErrors.h"

CGenClient2::CGenClient2(HWND parent)
	: CGenCliSvr2_Sock(parent)
	, m_bInDestructor(0)	
	, m_bConnected(0)
	, m_iPort(0)
	//, m_host[0](0)
	, m_tConnectionBegin(0)
	, m_tConnectionTimeout(0)

	
	
{
	ASSERT(_CrtCheckMemory());
}

CGenClient2::~CGenClient2()
{
	m_bInDestructor = 1;
	Close();
}

void CGenClient2::SetProtocolOpt(int iProto)
{
	m_iProto = iProto;
}

int CGenClient2::BeginConnection(LPCTSTR host, unsigned port, size_t maxSendSz, 
								 size_t maxRcvSz, unsigned uiMagic, int timeoutSeconds)
{
	m_uiMagic = uiMagic;

	if(!host)
		host = _T("localhost");

	// "Events" on which we will be async
	long lEvent = FD_READ | FD_WRITE  | FD_CONNECT | FD_CLOSE;
	
	AfxSocketInit();
	BOOL b = Create(0, SOCK_STREAM, lEvent, NULL);
	if(!b)
		return oops("BeginConnection: Create:");
	
	b = AsyncSelect(lEvent);
	if(!b)
		return oops("BeginConnection: AsyncSelect");
	
	b = Connect(host,port);
	if(!b)
	{
		if(GetLastError() == WSAEWOULDBLOCK)
			b = TRUE;
		if(!b)
			return oops("BeginConnection: Connect:");
	}

#if 1 // new code to arrange for persistent retries 

	strcpy(m_host,host);
	m_iPort = port;
	m_tConnectionBegin = time(NULL);
	m_tConnectionTimeout = timeoutSeconds;
	
#endif
	
	alloc(maxSendSz, maxRcvSz); 
	return 0;
}

BOOL CGenClient2::bIsConnected(void)
{
	// Tells you if you are currently connected.
	return m_bConnected;
}

void CGenClient2::OnConnection(int ecode)
{
	//  Virtual: Derived class should override, and also call the base class

	OnConnectionSuccess(ecode == 0);
}

void CGenClient2::OnClose(int ecode)
{
	// virtual override of CAsyncSocket
	// Derived classes should leave this alone and use "OnSocketClose()
	// Or if you must, be sure to call the base class

	// Ecode is usually 0, but sometimes WSAENETDOWN, WSAECONNRESET, WSAECONNABORTED
	m_bConnected = FALSE;
	if(m_bInDestructor)
		return;
	CGenCliSvr2_Sock::OnClose(ecode);
	if(m_bInDestructor)
		return;
	OnSocketClose(ecode);

}

void CGenClient2::OnConnect(int ecode)
{
	// virtual override of CAsyncSocket
	// Please leave me alone

	// Ecode is 0, WSAEADDRINUSE, etc
	// if WSAECONNREFUSED or -1, the server might not be up yet
	if(ecode)
	{
		if(ecode == WSAECONNREFUSED || ecode == -1)
		{
#ifdef _DEBUG_xx 
			msg("CGenClient2::OnConnect: WSAECONNREFUSED, Retrying");
#endif 
		}
		else
		{
			msg("CGenClient2::OnConnect: ecode=",szWinSockErrName(ecode));
		}
	}

#if 1 // New code to try again... 
	// If WSAECONNREFUSED, the server might not be up yet
	if(ecode == WSAECONNREFUSED || ecode == -1)
	{
		time_t tNow = time(NULL);
		if(tNow < m_tConnectionBegin + m_tConnectionTimeout)
		{
			BOOL b = Connect(m_host,m_iPort);
			if(b)
				return;

			if(!b)
			{
				if(GetLastError() == WSAEWOULDBLOCK)
					return;
			}
		}
	}
#endif

	CGenCliSvr2_Sock::OnConnectionSuccess(ecode==0);

	OnConnection(ecode);
	if(ecode == 0)
		m_bConnected = TRUE;
}

void CGenClient2::OnRcvLo(char *buf, size_t sz)
{
	// Virtual override of CGenCliSvr2_Sock::OnRcvLo
	OnRcv(buf,sz);
}





// EOF