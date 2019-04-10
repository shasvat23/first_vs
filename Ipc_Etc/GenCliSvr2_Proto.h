//
// GenCliSvr2_Proto.h
//
//	This is the protocol used privately by GenCliSvr2_Client and GenCliSvr2_Server
//	This should be hidden from everyone else.

#pragma once


typedef struct CliSvr2HdrTag
{
	unsigned magic;
	unsigned ttlSz;		// size of entire message, including this hdr

	unsigned hdrSz;
	unsigned msgSz;

	// Note: Should be quadword sized!

} CliSvr2Hdr;

typedef struct CliSvr2TrailerTag
{
	unsigned magic;
	unsigned pad;  // Note: Should be quadword sized!
} CliSvr2Trailer;

#define _GenCliSvrMagic 27

// eof


