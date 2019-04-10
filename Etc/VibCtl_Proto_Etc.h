//
// VibCtl_Proto_Etc.h
//
//	Debug aids for VibCtl_Proto.h structs
//
#pragma once

#include "\Src\Src_Common\Inc\VibCtl_Proto.h"

#include "Ecode.h"

#ifdef __cplusplus	// Sometimes the numerical 'C' files get here, and they do not need this stuff, and some of these are overloaded

const char * VibCtlTestAckCodeName(eLoadAckCode code);

const char * VibCtlMsgIdName(int id);
const char * VibCtlMsgIdName(eVibCtlMsgId id);
const char * VibCtlMsgIdName(const void *buf);
const char * VibCtlUnitsName(eVibCtlUnits u);

const char * VibCtlErrName(int err);
const char * VibCtlErrName(eVibCtlErr err);

const char * VibCtlDiagLevelName(eVibCtlDiagLevel l);

const char * VibCtlMajorStateName(eVibCtlMajorState s);
const char * VibCtlMajorEventName(eVibCtlMajorEvent e);
const char * VibCtl_SineMajorStateName(VibCtl_SineMajorState s);

const char * StdAckMsgDescription(const VibCtlStdAckMsg *m);
const char * TestCtlAckMsgDescription(const VibCtlTestCtlAckMsg *m);
const char * TestLoadAckMsgDescription(const VibCtlTestLoadAckMsg *m);

const char * TestCtlMsgDescription(const VibCtlTestCtlMsg *m);

const char * VibCtlAlarmFormatter(const VibCtlAlarmMsg *m);
const char * VibCtlAlarmName(eVibCtlAlarm a);

eVibCtlErr vibCtlErrFromEcode(Ecode er);

#endif // __cplusplus

// EOF
