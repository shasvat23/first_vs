//
// TcmGlobals.h
//
#pragma once

#include "..\App_Spt\AppMsgs.h"
#include "SampleAppSvr.h"


extern  CSampleAppSvr *g_svr ;

extern  int g_bConnectionInProgress;	// Managed by GUI
extern  int g_bConnected ;			// Managed by SampleAppClient
extern  int g_bConnectionFailed ;	// Managed by SampleAppClient

void dispatchMsgFromClient(int iClientId, DWORD_PTR dwApplicationLevelClientData, void *buf, size_t sz);

// eof 
