//
// GenCliSvr2_Sock.h
//
//	This class extends CAsyncSocket
//	by providing iobuffers and
//	message oriented async send/rcv.
//	Remember CAsyncSocket has no message
//	concept and the async read / write
//	may return partial results.
//	Here we buffer up whole messages.
//	The message protocol is defined in __Proto.h
//
#pragma once

#include "GenCliSvr2_AsyncSock.h"
#include "GenCliSvr2_Proto.h"

LPCSTR getLocalhostAddr(void);
LPCSTR getHostAddr(LPCSTR);

//#define _SOCK_CHATTY

#define _CliSvrSendFlag_DisconNotice 0x1

class CGenCliSvr2_Sock : public CMyAsyncSocket
{
public:
	CGenCliSvr2_Sock(HWND parent);
	virtual ~CGenCliSvr2_Sock();
	
protected:
	virtual void OnClose(int ecode);
	// Application must override this.

	virtual void OnRcvLo(char *, size_t);
	// Application must override this.

	virtual void OnReceive(int ecode);
	// Override of CASyncSocket
	
	virtual void OnSend(int ecode);
	// Override of CASyncSocket

	friend void CALLBACK GenCliSvr2Sock_OnTimer(void *, BOOLEAN);
	friend void CALLBACK GenCliSvr2Sock_OnTimer(HWND,UINT,UINT_PTR,DWORD);
	void OnTimer(void);
	virtual void voops(char const *a);
	virtual int   oops(char const *a);
	
public:
	void alloc(size_t maxSzSend, size_t maxSzRcv);
	void OnConnectionSuccess(BOOL bOk);	
	size_t getSendNPending(void);
	size_t getSendNFree(void);
	size_t send(const void *b1, size_t sz1, const void *b2=0, size_t sz2=0, DWORD dwFlags=0x0);	
	size_t send3a(CliSvr2Hdr *hdr, size_t hdrSz, const void *buf, size_t bufSz, const void *b2, size_t sz2, CliSvr2Trailer *trailer, size_t trlSz, DWORD dwFlags=0x0);
	CString  m_csAddr;
	unsigned m_uiMagic;
	int m_iProto;
	int m_bAutoDeleteReq;
	BOOL m_bConnected;
private:
	void DoSend(int ecode);
	void DoRcv(int ecode);
	void setupForRcv(void);
	void setupForSend(void);
	
	char * m_sendBuf;
	char * m_rcvBuf;
	size_t m_sendBufSz;
	size_t m_rcvBufSz;
	size_t m_sendNSent;
	size_t m_sendNPending;
	size_t m_sendNFree;
	size_t m_rcvNGot;
	size_t m_rcvNPending;
	
	BOOL m_bRcvingHdr;
	char *m_ptrSend[2];
	char *m_ptrRcv;

	BOOL m_b_I_Am_So_Outta_Here;
	
	UINT_PTR m_uiTimerId;
	BOOL m_BurstModeOptimization;

	CRITICAL_SECTION m_sendQueueCritSec;

	BOOL m_bInOnTimer;
};

// eof
