//
// SampleAppClient.h
//
#pragma once
#include "GenCliSvr2_CLient.h"


class CSampleAppClient : public CGenClient2
{
public:
	CSampleAppClient(HWND parent);
	virtual ~CSampleAppClient();

	void OnSocketClose(int istat);
	// Pure-virtual in base-class
	

	virtual void OnRcv(char *buf, size_t sz);
	// Pure-virtual in base-class
	// This will have one message from host in buf.  sz must be 
	// .le. maxRcvSz given to "BeginConnection"

	// size_t send(const void *b1, size_t sz1, const void *b2=0, size_t sz2=0, DWORD dwFlags=0x0);	 // In the base class
	// Will not block.  In proper operation
	// it just enqueues the buf (sz must be .le. maxSendSz given to "BeginConnection" )
	// Return is sz sent


	virtual void OnConnection(int istat);
	// Derived classes should always override this AND ALSO call the base class
	// Unless you override this your class is probably worthless.
	// The base class sets a flag to start the polling for recv 

};
// EOF 
