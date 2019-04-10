//
// AppMsgs.h
//	This defines the messages exchanged between GUI and TCM

#pragma once

typedef enum _eGuiToTcmMsgId
{
	MsgId_EchoReq = 1 // EchoMsg

} eGuiToTcmMsgId;



typedef enum _eTcmToGuiMsgId
{
	MsgId_EchoAck = 1	// EchoMsg

} eTcmToGuiMsgId;

typedef struct _EchoMsg
{
	int msgId;		// MsgId_EchoReq or MsgId_EchoAck
	char buf[100];

} EchoMsg;

typedef union _uMaxMsgGuiToTcm
{
	EchoMsg echoMsg;


} uMaxMsgGuiToTcm;

typedef union _uMaxMsgTcmToGui
{
	EchoMsg echoMsg;


} uMaxMsgTcmToGui;

#define _EagleSampleAppCliSvrMagicNumber 17




