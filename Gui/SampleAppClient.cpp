//
// SampleAppClient.cpp
//

#include "Stdafx.h"

#include "SampleAppClient.h"
#include "msg.h"
#include "GuiGlobals.h"


CSampleAppClient::CSampleAppClient(HWND parent)
	: CGenClient2(parent)
{

}

CSampleAppClient::~CSampleAppClient()
{

}


void CSampleAppClient::OnConnection(int istat)
{
	if(istat < 0)	// Connection refused
	{

		msg("Connection timed-out or refused");
		g_bConnected = 0;
		g_bConnectionFailed = 1;
	}else{
		g_bConnected = 1;
		g_bConnectionFailed = 0;
	}
}

void CSampleAppClient::OnSocketClose(int istat)
{
	msg("Server has CLOSED my socket");
	g_bConnected = 0;
	g_bConnectionFailed = 1;

}


void CSampleAppClient:: OnRcv(char *buf, size_t sz)
{
	int *ptr = (int *)buf;
	int msgId = *ptr;
	dispatchMsgFromTcm(buf, sz);
}


// EOF 
