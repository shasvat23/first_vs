//
// SampleAppSvr.cpp
//
#include "stdafx.h"
#include "SampleAppSvr.h"
#include "msg.h"
#include "VibCtlPorts.h"
#include "TcmGlobals.h"

CSampleAppSvr::CSampleAppSvr(HWND parent)
	: CGenSvr2(parent)
{

}

CSampleAppSvr::~CSampleAppSvr()
{

}

int CSampleAppSvr::OnConnection(LPCTSTR szPlaintextAddr, SOCKADDR remoteClientAddr, 
	int addrsz, int iClientId, DWORD_PTR *pClientData)
{
	msg("Got client connection from:", szPlaintextAddr);
	*pClientData = NULL;	// You could allocate per-client data here.... it would be made avail in "onRcv()" and "OnDisconnect()"

	return 0;
}


void CSampleAppSvr::OnRcv(int iClientId, DWORD_PTR dwApplicationLevelClientData, void *buf, size_t sz)
{
	::dispatchMsgFromClient(iClientId, dwApplicationLevelClientData, buf, sz);
}

void CSampleAppSvr::OnDisconnect(int iClientId, DWORD_PTR dwApplicationLevelClientData, int ecode)
{
	msg("He's gone!");
}



