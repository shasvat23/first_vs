//
// GuiGlobals.h
//
#pragma once

#include "..\App_Spt\AppMsgs.h"
#include "SampleAppClient.h"


extern  CSampleAppClient *g_client ;

extern  int g_bConnectionInProgress;	// Managed by GUI
extern  int g_bConnected ;			// Managed by SampleAppClient
extern  int g_bConnectionFailed ;	// Managed by SampleAppClient

void dispatchMsgFromTcm(char *buf, size_t sz);

// eof 
