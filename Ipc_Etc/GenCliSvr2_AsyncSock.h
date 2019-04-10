//
// GenCliSvr2_AsyncSock.h
//	See the .cpp file
//

#pragma once

#include "Winsock.h"

//#define MyAsyncSocket_PassthroughTo_CASyncSocket

#ifdef MyAsyncSocket_PassthroughTo_CASyncSocket
#include "AfxSock.h"
	class CMyAsyncSocket : public CAsyncSocket
	{
	public:
		CMyAsyncSocket(HWND parent);
		virtual ~CMyAsyncSocket();

	};

#else // MyAsyncSocket_PassthroughTo_CASyncSocket

class CMyAsyncSocket
{

public:
	CMyAsyncSocket(HWND parent);
	virtual ~CMyAsyncSocket();

	BOOL Create(UINT nSocketPort = 0, int nSocketType=SOCK_STREAM,
		long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE,
		LPCTSTR lpszSocketAddress = NULL);

	BOOL Socket(int nSocketType=SOCK_STREAM, long lEvent =
		FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE,
		int nProtocolType = 0, int nAddressFormat = PF_INET);

	BOOL SetSockOpt(int nOptionName, const void* lpOptionValue,
		int nOptionLen, int level = SOL_SOCKET);

	BOOL Listen(int nConnectionBacklog=5);
	
	virtual BOOL Accept(CMyAsyncSocket& rConnectedSocket,
		SOCKADDR* lpSockAddr = NULL, int* lpSockAddrLen = NULL);

	BOOL Connect(LPCTSTR lpszHostAddress, UINT nHostPort);

	BOOL Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL);
	BOOL Bind(const SOCKADDR* lpSockAddr, int nSockAddrLen);


	BOOL AsyncSelect(long lEvent);

	virtual void Close();

	virtual size_t Receive(void * lpBuf, size_t nBufLen, int nFlags = 0);

	virtual size_t Send(const void * lpBuf, size_t nBufLen, int nFlags = 0);

	SOCKET m_hSocket;
	HWND m_parent;
	
protected:

	virtual void OnConnect(int ecode) ;
		// App must override this too, I guess!

	virtual void OnClose(int ecode) = NULL;	
	virtual void OnAccept(int nErrorCode) ;	
	virtual void OnRcvLo(char *, size_t) = NULL;
	virtual void OnReceive(int ecode) = NULL;	
	virtual void OnSend(int ecode) = NULL;

	// Newbie, not sure its needed
	virtual void OnExcept(int ecode);

	virtual void OnTimer(unsigned uiNow, size_t sendNPending); // Derived class must call me!

private:

	int m_iMagic;
	int m_bInDestructor;
	int m_id;
	long m_lEvent;
	int m_holdoff;
	int m_bConnectInProgress;
	int m_bListenInProgress;
	int m_bInOnTimer;
	FD_SET m_fdsRead;
	FD_SET m_fdsWrite;
	FD_SET m_fdsExcept;

};

#endif // #else // MyAsyncSocket_PassthroughTo_CASyncSocket

// eof  GenCliSvr2_AsyncSockDotHInc