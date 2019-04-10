//
// SampleAppSvr.h
//
#pragma once
#include "GenCliSvr2_Server.h"

class CSampleAppSvr : public CGenSvr2
{
public:
	CSampleAppSvr(HWND parent);
	virtual ~CSampleAppSvr();

	virtual int OnConnection(LPCTSTR szPlaintextAddr, SOCKADDR remoteClientAddr, 
		int addrsz, int iClientId, DWORD_PTR *pClientData);
	// Pure-virtual in base class
	// Return -1 if you wish to refuse a connection from this host
	// Otherwise return 0 and set *pClientData to the (newly allocated) per-client-data;
	// This tag will be presented on future calls to OnRcv
	// and will be used in future calls to send.

	virtual void OnRcv(int iClientId, DWORD_PTR dwApplicationLevelClientData, void *buf, size_t sz);
	// Pure-virtual in base class
	// The "iClientId" may be used for calls to Disconnect or Send
	// The "dwApplicationLevelClientId" is the tag you returned from OnConnection

	virtual void OnDisconnect(int iClientId, DWORD_PTR dwApplicationLevelClientData, int ecode);
	// Pure-virtual in base class
	// He's gone, folks.  Gone.

};

