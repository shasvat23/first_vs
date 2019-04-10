//
// GenSvr2.h
//
//	Our motto: KISS;
//	
//		We do NOT impose protocols on the client-server application layer.
//	
//		Always async.  Neverblock.
//
//	To use this: 
//		Derive a class from this and override OnConnection(), OnRcv(), and OnDisconnect()
//		From the OnRcv() method you can call send() or disconnect()
//
//		The base class implementations of OnConnection(), OnRcv() and OnDisconnect()
//		are suitable for a quick demo; rtfc
/*

Server logic
1.  Create a class derived from CGenSvr2
	Must implement the pure-virtual methods:
	OnConnection()
	OnRcv()
	OnDisconnect()

2.  It gets the host addr and port somehow (I have routines for this) and also determine
    the maximum message sizes, and also a "magic number" used for debugging

3.  It calls "StartService(....)"

4.  The app must regularly call the "OnTimer()" method (Very important!)
	The server does NOT do this all by itself -- I did it this way
	so that I could have just one timer in my app and it would call all the
	relevant "OnTimer()" methods for all its relevant objects

5.  When a client app makes a connection, the server app will invoke its OnConnection
    virtual method (this is pure-virtual in the base class, your derived class must implement it)

6.  The "OnConnection" method will be called in the context of "OnTimer"

7.  The "OnConnection" method must allocate any required 
    per-client data and return a pointer thereto,
    at "DWORD_PTR *pdwApplicationLevelClientData"

8.  Thereafter when a client message has been recvd, it gets delivered
    to the OnRcv() virtual mehod (pure-virtual in the base class) 
	This is invoked in the context of OnTimer()

9.  The OnRcv() method sends info back to the client via the "send()" method -- don't use "send3a"
    
10. You can also call send() from your code in the context of any other windows message,
    but it is NOT thread-safe, so you can NOT call send() from a worker thread

10.  If/when the client disconnects, the server-object gets "OnDisconnect()", also in the
     "OnTimer()" context -- it should free the data allocated in "OnConnection"


The Client, on the other hand, does pretty much the same stuff, only upside-down:

1. Create a class derived from CGenClient2

2.  It gets the host addr and port somehow (I have routines for this)
   Also it must know the exact same max message sizes and magic number as 
   did the server.

3. It calls "BeginConnection()"

4. The GenClient will set up its own "on timer" logic, using the window handle
   you gave it when you created it -- THIS MAY NOT BE NULL -- the world will end
   so the "OnConnection, OnSocketClose, and OnRcv methods will be called in the 
   context of a windows timer event for the window handle given on creation.

5. When the connection to the server is accepted, the "OnConnection()" method
   is called -- please call the base class first, then do your own stuff

6. Thereafter you can send messages to the server via 
   "size_t send(const void *b1, size_t sz1, const void *b2=0, size_t sz2=0, DWORD dwFlags=0x0);"
   This is a public method in the CGenClient2 base class CGenCliSvr2_Sock
   You can invoke this from any Windows event handler WM_TIMER, etc, but not 
   from a worker thread.

7.  The "onRcv()" method (pure-virtual in base class) is invoked in the context
    of a windows timer, as is the OnConnection()

8.  You also can get "OnSocketClose()" if the server shuts you down.

9.  Note that Tcp/IP is very persistent -- if you disconnect the Cat-5 cable
then Tcp/Ip will assume this is just a temporary outage and will keep the 
sockets open for -- for how long?  The windows default is several hours --
I've tried to set it to a much lower value -- ten seconds or so -- but I haven't tested it.
because 10 seconds is too long, in my line of work -- if the cable gets disconnected
we need to shut down the controller in four seconds.

*/

#pragma once


#include "GenCliSvr2_Proto.h"
#include "GenCliSvr2_Sock.h"
#include "GenCliSvr2_AsyncSock.h"

#define ALL_CLIENTS (-1)
#define GenSvrErr_inval			-1
#define GenSvrErr_disconnected	-2
#define GenSvrErr_wouldblock	-3


class CClientSock;

class CGenSvr2 : public CMyAsyncSocket
{
public:
	CGenSvr2(HWND parent);
	virtual ~CGenSvr2();

	BOOL  StartService(int port, const char *hostName, size_t maxSendSz, size_t maxRcvSz, unsigned uiMagic);
	// bExternal:  If 0, then service is on "localhost" and is visible only to clients on this machine; "hostAddr" is ignored
	//			   If 1, then service is visible to clients on the network, and "hostAddr" is possibly used
	// hostName:   "localhost" or "Tetonia" or "192.168.0.1" etc

	void SetProtocolOpt(int iProto);
		// If iProto is 0 (normal) then we deliver messages without headers;
		// If iProto is 1 then we deliver message with headers and the
		// sender must include the headers
		// This is really useful only for debugging

	virtual int OnConnection(LPCTSTR szPlaintextAddr, SOCKADDR remoteClientAddr, 
		int addrsz, int iClientId, DWORD_PTR *pClientData) = NULL;
	// Application should override this ALWAYS.
	// Return -1 if you wish to refuse a connection from this host
	// Otherwise return 0 and set *pClientData to the (newly allocated) per-client-data;
	// This tag will be presented on future calls to OnRcv
	// and will be used in future calls to send.

	virtual void OnRcv(int iClientId, DWORD_PTR dwApplicationLevelClientData, void *buf, size_t sz) = NULL;
		// Application should override this ALWAYS.
		// The "iClientId" may be used for calls to Disconnect or Send
		// The "dwApplicationLevelClientId" is the tag you returned from OnConnection
		
	virtual void OnDisconnect(int iClientId, DWORD_PTR dwApplicationLevelClientData, int ecode) = NULL;
		// Application should override this ALWAYS.
		// He's gone, folks.  Gone.

	int disconnect(int iClientId);
		// You may call this from OnRcv
		// Just disconnects the poor bloke.
		// iClientId is a parameter you get from OnRcv

	void DisconnectAllClients(void);

	size_t get_send_freespace(int iClientId);

	size_t send(int iClientId, const void *b1, size_t sz1, const void *b2=0, size_t sz2=0, DWORD flags=0x0);
		// You may call these from OnRcv
		// iClientId is a parameter you get from OnRcv

	size_t send3a(int iClientId, CliSvr2Hdr *hdr, size_t hdrSz, const void *buf, size_t bufSz, const void *b2, size_t sz2, CliSvr2Trailer *trailer, size_t trlSz, DWORD flags=0x0);
	// You may call this from OnRcv
	// iClientId is a parameter you get from OnRcv

	int getNumConnections(void);
		// Just return the number of connections.
	virtual void OnClose(int ecode);	
	virtual void OnRcvLo(char *, size_t);
	virtual void OnReceive(int ecode);	
	virtual void OnSend(int ecode);

	void OnTimer(void); // I think the app must call this regularly -- it is used to check for pending client connection requests
	
	DWORD_PTR getClientData(int iClientId);

protected:
	virtual void OnAccept(int ecode);
		// This is an override of CAsyncSocket::OnAccept and applications
		// should NOT override it; or if they do they should call the base class first... 

private:
	friend CClientSock;
	friend CGenCliSvr2_Sock;

	int oops(char *a);
	void voops(char *a);
	void OnDisconLo(int iClientIdx, DWORD_PTR dwTag, int ecode);

	
	int m_nClients;
	size_t m_maxSzSend;
	size_t m_maxSzRcv;
	UINT_PTR m_uiTimerId;

	#define MaxClients 32
	CClientSock * m_C[MaxClients];

	unsigned m_uiMagic;
	int m_iProto;

	HWND m_parent;
};

// _GEN_SVR2__HH_H_H_H_