//
// GenCliSvr2_Client.h -- Simple sockets client.  We hope
//
//	To use this, derive a class from this and
//	override OnConnection() and OnRcv() and OnSocketClose
//
//	

#pragma once

#include "GenCliSvr2_Proto.h"
#include "GenCliSvr2_Sock.h"


class CGenClient2 : public CGenCliSvr2_Sock
{
public:
	CGenClient2(HWND parent);
	virtual ~CGenClient2();

	// EVERYTHING is ALWAYS non-blocking.
	void SetProtocolOpt(int iProto);
	// If iProto is 0 (normal) then we deliver messages without headers;
	// If iProto is 1 then we deliver message with headers and the
	// sender must include the headers.  
	// This is really useful only for debugging

	int BeginConnection(LPCTSTR host, unsigned port, 
		size_t maxSendSz, size_t maxRcvSz, unsigned uiMagic,int timeoutSeconds);
		// You can take errors here, but most likely
		// you will take the result status in OnConnection

	BOOL bIsConnected(void);
		// Tells you if you are currently connected.

	virtual void OnConnection(int istat);
		// Derived classes should always override this AND ALSO call the base class
		// Unless you override this your class is probably worthless.
		// The base class sets a flag to start the polling for recv 

	virtual void OnSocketClose(int istat) = NULL;
		// Derived class should always override this

	virtual void OnRcv(char *buf, size_t sz) = NULL;
		// Derived classes should always override this
		// This will have one message from host in buf.  sz must be 
		// .le. maxRcvSz given to "BeginConnection"

	// size_t send(const void *b1, size_t sz1, const void *b2=0, size_t sz2=0, DWORD dwFlags=0x0);	 // In the base class
		// Will not block.  In proper operation
		// it just enqueues the buf (sz must be .le. maxSendSz given to "BeginConnection" )
		// Return is sz sent

protected:

	virtual void OnRcvLo(char *, size_t);

	virtual void OnClose(int ecode);
	// Override of CAsyncSocket::OnClose();

	virtual void OnConnect(int ecode);
	// Override of CAsyncSocket::OnConnect();
	
private:
	BOOL m_bConnected;
	int m_iPort;
	char m_host[MAX_PATH];
	time_t m_tConnectionBegin;
	int m_tConnectionTimeout;
	int m_bInDestructor;

};

// eof 
