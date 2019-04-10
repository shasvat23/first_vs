//
// VibCtl_Proto_Dbg.cpp
//
//	Debug aids for VibCtl_Proto.h structs
//
#include "stdafx.h"
#include "VibCtl_Proto_Etc.h"


const char * VibCtlMsgIdName(int id)
{
	return VibCtlMsgIdName((eVibCtlMsgId)id);
}


const char * VibCtlMsgIdName(const void *buf)
{
	const VibCtlStdMsg *h = (const VibCtlStdMsg *)buf;
	return VibCtlMsgIdName((eVibCtlMsgId)h->iMsgId);
}


const char * VibCtlUnitsName(eVibCtlUnits u)
{
	const char *a = NULL;
	static char aa[60];
	switch(u)
	{
	default: sprintf(aa,"VibCtlUnits=%d",(int)u); a = aa; break;
		
	case VibCtlUnits_Unspecified: a = "(Unspecified)";	break;
	case VibCtlUnits_Dimensionless: a = "(Dimensionless)";	break;
	case VibCtlUnits_dB: a = "dB";	break;

	case VibCtlUnits_mpss: a = "mpss";	break;

	case VibCtlUnits_mps: a = "mps";	break;
	case VibCtlUnits_Newton: a = "Newton";	break;
		
	case VibCtlUnits_mm: a = "mm";	break;
	case VibCtlUnits_Volts: a = "Volts";	break;
	case VibCtlUnits_Hz: a = "Hz";	break;
	case VibCtlUnits_last: a = "VibCtlUnits_last";	break;
	}
	return a;

}

const char * VibCtlMsgIdName(eVibCtlMsgId id)
{
	const char *a = NULL;
	static char aa[60];
	switch(id)
	{
	default: sprintf(aa,"VibCtlMsgId=%d",(int)id); a = aa; break;


	case VibCtlMsgId_GeneralControlFirst: a = "VibCtlMsgId_GeneralControlFirst";	break;
	case VibCtlMsgId_PwdReq: a = "VibCtlMsgId_PwdReq";	break;
	case VibCtlMsgId_DisconReq: a = "VibCtlMsgId_DisconReq";	break;
	case VibCtlMsgId_Terminate: a = "VibCtlMsgId_Terminate";	break;
	case VibCtlMsgId_Quit: a = "VibCtlMsgId_Quit";	break;

	case VibCtlMsgId_HideAndFreeze: a = "VibCtlMsgId_HideAndFreeze";	break;
	case VibCtlMsgId_Minimize: a = "VibCtlMsgId_Minimize";	break;
	case VibCtlMsgId_Maximize: a = "VibCtlMsgId_Maximize";	break;
	case VibCtlMsgId_ShowNormal: a = "VibCtlMsgId_ShowNormal";	break;
	case VibCtlMsgId_GetWindowHandle: a = "VibCtlMsgId_GetWindowHandle";	break;
		
	case VibCtlMsgId_ResendLastMsg: a = "VibCtlMsgId_ResendLastMsg";	break;
	case VibCtlMsgId_GeneralControlLast: a = "VibCtlMsgId_GeneralControlLast";	break;
	
	case VibCtlMsgId_logon: a = "VibCtlMsgId_logon";	break;
	case VibCtlMsgId_logoff: a = "VibCtlMsgId_logoff";	break;
	case VibCtlMsgId_LoadTestFirst: a = "VibCtlMsgId_LoadTestFirst";	break;
		
	case VibCtlMsgId_LoadTest: a = "VibCtlMsgId_LoadTest";	break;
	case VibCtlMsgId_LoadProfile: a = "VibCtlMsgId_LoadProfile";	break;
	case VibCtlMsgId_LoadSchedule: a = "VibCtlMsgId_LoadSchedule";	break;
	case VibCtlMsgId_EchoReq: a = "VibCtlMsgId_EchoReq";	break;
	case VibCtlMsgId_LoadTestLast: a = "VibCtlMsgId_LoadTestLast";	break;
		
	case VibCtlMsgId_TestCtlFirst: a = "VibCtlMsgId_TestCtlFirst";	break;
	case VibCtlMsgId_Start: a = "VibCtlMsgId_Start";	break;
	case VibCtlMsgId_Stop: a = "VibCtlMsgId_Stop";	break;
	case VibCtlMsgId_Pause: a = "VibCtlMsgId_Pause";	break;
	case VibCtlMsgId_Continue2: a = "VibCtlMsgId_Continue";	break;
		
	case VibCtlMsgId_Resume2:		a = "VibCtlMsgId_Resume";	break;
	case VibCtlMsgId_SetLevel:		a = "VibCtlMsgId_SetLevel";	break;
	case VibCtlMsgId_SetFreq:		a = "VibCtlMsgId_SetFreq";	break;
	case VibCtlMsgId_IncrementLevel:a = "VibCtlMsgId_IncrementLevel";	break;
	case VibCtlMsgId_IncrementFreq: a = "VibCtlMsgId_IncrementFreq";	break;
		
	case VibCtlMsgId_SendSingle_TimeHistory: a = "VibCtlMsgId_SendSingle_TimeHistory";	break;
	case VicCtlMsgId_SendSingle_Random: a = "VibCtlMsgId_SendSingle_Random";	break;
	case VibCtlMsgId_Pretest: a = "VibCtlMsgId_Pretest";	break;
	case VibCtlMsgId_LoadXfer: a = "VibCtlMsgId_LoadXfer";	break;
	case VibCtlMsgId_RunContinuityTest: a = "VibCtlMsgId_RunContinuityTest";	break;
	case VibCtlMsgId_SetScheduleIdx: a = "SetScheduleIdx";	break;
	case VibCtlMsgId_TestCtlLast: a = "VibCtlMsgId_TestCtlLast";	break;
		
	case VibCtlMsgId_AlarmReq: a = "VibCtlMsgId_AlarmReq";	break;
	case VibCtlMsgId_SystemStatusReq: a = "VibCtlMsgId_SystemStatusReq";	break;
	case VibCtlMsgId_SystemStatusReqEx: a = "VibCtlMsgId_SystemStatusReqEx (Deprecated)";	break;

	case VibCtlMsgId_QueryTest: a = "VibCtlMsgId_QueryTest"; break;
	case VibCtlMsgId_QueryProfile: a = "VibCtlMsgId_QueryProfile"; break;
	case VibCtlMsgId_QuerySchedule: a = "VibCtlMsgId_QuerySchedule"; break;
	
	case VibCtlMsgId_ServerToClient_First: a = "VibCtlMsgId_ServerToClient_First";	break;
	case VibCtlMsgId_CmdAck: a = "VibCtlMsgId_CmdAck";	break;
		
	case VibCtlMsgId_LoadTestAck: a = "VibCtlMsgId_LoadTestAck";	break;
	case VibCtlMsgId_LoadTestAck_LG: a = "VibCtlMsgId_LoadTestAck_LG";	break;

	case VibCtlMsgId_TestCtlAck: a = "VibCtlMsgId_TestCtlAck";	break;
	case VibCtlMsgId_Alarm: a = "VibCtlMsgId_Alarm";	break;
	case VibCtlMsgId_Alarm_LG: a = "VibCtlMsgId_Alarm_LG";	break;


	case VibCtlMsgId_SystemStatus: a = "VibCtlMsgId_SystemStatus";	break;
	case VibCtlMsgId_SystemStatus_LG: a = "VibCtlMsgId_SystemStatus_LG";	break;
	case VibCtlMsgId_SystemStatusEx: a = "VibCtlMsgId_SystemStatusEx";	break;

	case VibCtlMsgId_diag: a = "VibCtlMsgId_diag";	break;
	case VibCtlMsgId_diag_LG: a = "VibCtlMsgId_diag_LG";	break;

	case VibCtlStateChange: a = "VibCtlStateChange";	break;
		
	case VibCtlMajorEvent: a = "VibCtlMajorEvent";	break;
	case VibCtlMajorEvent_LG: a = "VibCtlMajorEvent_LG";	break;

	case VibCtlMsgId_TestEndSynopsis: a = "VibCtlMsgId_TestEndSynopsis";	break;
	case VibCtlMsgId_TestEndSynopsis_LG: a = "VibCtlMsgId_TestEndSynopsis_LG";	break;

	case VibCtlMsgId_Last: a = "VibCtlMsgId_Last";	break;

	case VibCtlMsgId_EchoReq2: a = "lvCmdId_EchoReq";	break;

#if defined _MIMO6 && defined _EAGLE
	case VibCtlMsgId_OutputRelayOn  : a = "VibCtlMsgId_OutputRelayOn";	break;
	case VibCtlMsgId_OutputRelayOff : a = "VibCtlMsgId_OutputRelayOff";	break;
#endif 

	}

	return a;
}

const char * VibCtlErrName(int err)
{
	return VibCtlErrName((eVibCtlErr)err);
}

const char * VibCtlErrName(eVibCtlErr err)
{
	const char *a = NULL;
	static char aa[60];
	switch(err)
	{
	default: sprintf(aa,"eVibCtlErr=%d",(int)err); a = aa; break;

	case VibCtlErr_Nostat: a = "VibCtlErr_Nostat";	break;
	case VibCtlErr_Success: a = "VibCtlErr_Success";	break;
	case VibCtlErr_BadCmdId: a = "VibCtlErr_BadCmdId";	break;
	case VibCtlErr_UnimplementedCmd: a = "VibCtlErr_UnimplementedCmd";	break;
	case VibCtlErr_Badstate: a = "VibCtlErr_Badstate";	break;
	case VibCtlErr_InvalPar: a = "VibCtlErr_InvalPar";	break;
	case VibCtlErr_FileNotFound: a = "VibCtlErr_FileNotFound";	break;
	case VibCtlErr_FileOpenError: a = "VibCtlErr_FileOpenError";	break;
	case VibCtlErr_FileFormatError: a = "VibCtlErr_FileFormatError";	break;
	case VibCtlErr_NeedFileSave: a = "VibCtlErr_NeedFileSave";	break;
	case VibCtlErr_AmRunning: a = "VibCtlErr_AmRunning";	break;
	case VibCtlErr_NotAuthorized: a = "Not licenced or otherwise not authorized"; break;

	case VibCtlErr_OperationInProgress: a = "Operaion in progress"; break;		

	case VibCtlErr_ProtocolError: a = " VibCtlErr_ProtocolError "; break;
	case VibCtlErr_SocketError: a = " VibCtlErr_SocketError "; break;
	case VibCtlErr_SendError: a = " VibCtlErr_SendError "; break;
	case VibCtlErr_NotConnected: a = " VibCtlErr_NotConnected "; break;
 	case VibCtlErr_Timedout: a = " VibCtlErr_Timedout "; break;
// 	case xx: a = " xx "; break;
// 	case xx: a = " xx "; break;

		// 	case xx: a = " xx "; break;
		// 	case xx: a = " xx "; break;
		// 	case xx: a = " xx "; break;
		// 	case xx: a = " xx "; break;




	case VibCtlErr_LastError: a = "VibCtlErr_LastError";	break;
	}

	return a;
}


const char *  VibCtlDiagLevelName(eVibCtlDiagLevel l)
{
	char *a = NULL;
	
	switch(l)
	{
	case VibCtlDiagLevel_Log:		a = "Log";	break;
	case VibCtlDiagLevel_Info:		a = "Info";	break;
	case VibCtlDiagLevel_Warn:		a = "Warn";	break;
	case VibCtlDiagLevel_Error:		a = "Error";	break;
	case VibCtlDiagLevel_Fatal:		a = "Fatal";	break;
	}
	
	return a;
}


const char * VibCtlMajorStateName(eVibCtlMajorState s)
{
	static char aa[60];
	char *a;
	switch(s)
	{
	default: sprintf(aa,"MajorState=%d",(int)s);a = aa; break;
	case VibCtlState_Initializing: a = "VibCtlState_Initializing"; break;
	case VibCtlState_DspNotFound: a = "VibCtlState_DspNotFound"; break;
	case VibCtlState_DspFaulted: a = "VibCtlState_DspFaulted"; break;
	case VibCtlState_LicenseFailed: a = "VibCtlState_LicenseFailed"; break;
	case VibCtlState_Exiting: a = "VibCtlState_Exiting"; break;
	case VibCtlState_Faulted: a = "VibCtlState_Faulted"; break;
	case VibCtlState_PostAn: a = "VibCtlState_PostAn"; break;
	case VibCtlState_NoTest: a = "VibCtlState_NoTest"; break;
	case VibCtlState_LoadingTest: a = "VibCtlState_LoadingTest"; break;
	case VibCtlState_IdleNoXfer: a = "VibCtlState_IdleNoXfer"; break;

	case VibCtlState_IdleWithXfer: a = "VibCtlState_IdleWithXfer"; break;
	case VibCtlState_RunningPretest: a = "VibCtlState_RunningPretest"; break;
	case VibCtlState_RunningTest: a = "VibCtlState_RunningTest"; break;
	case VibCtlState_Stopping: a = "VibCtlState_Stopping"; break;
	
	case VibCtlState_Paused: a = "VibCtlState_Paused";	break;
	case VibCtlState_Rampup: a = "VibCtlState_Rampup";	break;
	case VibCtlState_Holding: a = "VibCtlState_Holding";	break;

	case VibCtlState_Last: a = "VibCtlState_Last"; break;
	}
	return a;
}

const char * VibCtl_SineMajorStateName(VibCtl_SineMajorState s)
{
	static char aa[60];
	char *a;
	switch(s)
	{
	default: sprintf(aa,"SineState=%d",(int)s);a = aa; break;
	case Vcs_noState: a = "Vcs_noState"; break;
	case Vcs_deviceStopped: a = "Vcs_deviceStopped"; break;
	case Vcs_deviceFaulted: a = "Vcs_deviceFaulted"; break;
	case Vcs_idle: a = "Vcs_idle"; break;
	case Vcs_openLoopChanging: a = "Vcs_openLoopChanging"; break;
	case Vcs_openLoopHolding: a = "Vcs_openLoopHolding" ;break;
	case Vcs_rampup0: a = "Vcs_rampup0" ;break;
	case Vcs_rampup1: a = "Vcs_rampup1" ;break;
	case Vcs_holding: a = "Vcs_holding"; break;
	case Vcs_levelChange: a = "Vcs_levelChange"; break;
	case Vcs_sweeping: a = "Vcs_sweeping"; break;
	case Vcs_rampdown: a = "Vcs_rampdown" ;break;
	case Vcs_fault2: a = "Vcs_fault2" ;break;
	case Vcs_last: a = "Vcs_last"; break;


	}
	return a;
}

const char *VibCtlMajorEventName(eVibCtlMajorEvent e)
{
	static char aa[60];
	char *a;
	switch(e)
	{
	default: sprintf(aa,"MajorEvent=%d",(int)e);a = aa; break;
	case VibCtlEvent_AdacStart:		a = "VibCtlEvent_AdacStart";	break;
	case VibCtlEvent_pretestStart:	a = "VibCtlEvent_pretestStart";	break;
	case VibCtlEvent_pretestPass:	a = "VibCtlEvent_pretestPass";	break;
	case VibCtlEvent_pretestFail:	a = "VibCtlEvent_pretestFail";	break;
	case VibCtlEvent_stop:			a = "VibCtlEvent_Stop";	break;
	case VibCtlEvent_testStart:		a = "VibCtlEvent_testStart";	break;

	case VibCtlEvent_pause:				a = "VibCtlEvent_pause";	break;
	case VibCtlEvent_resume:			a = "VibCtlEvent_resume";	break;
	case VibCtlEvent_AtFullLevel:		a = "VibCtlEvent_AtFullLevel";	break;
	case VibCtlEvent_AtPartialLevel:	a = "VibCtlEvent_AtPartialLevel";	break;
	case VibCtlEvent_alarm:				a = "VibCtlEvent_alarm";	break;
	case VibCtlEvent_fault:				a = "VibCtlEvent_fault";	break;
	case VibCtlEvent_hostExit:			a = "VibCtlEvent_hostExit";	break;
	case VibCtlEvent_advisory:			a = "VibCtlEvent_advisory";	break;
	case VibCtlEvent_unclassified:		a = "VibCtlEvent_unclassified";	break;
	
	case VibCtlEvent_AlignmentBegin:	a = "Adc/Dac Aligning";				break;
	case VibCtlEvent_AlignmentSuccess:	a = "Adc/Dac Alignment Success";	break;
	case VibCtlEvent_AlignmentFailed:	a = "Adc/Dac Alignment FAILED";		break;

	
	case VibCtlEvent_WaitingToSettleBegin:		a = "Adc Waiting to Settle";	break;
	case VibCtlEvent_WaitingToSettleSuccess:	a = "Adc Settle Complete";		break;
	case VibCtlEvent_WaitingToSettleFailed:		a = "Adc Offeset Trim FAILED";	break;


	case VibCtlEvent_last: a = "VibCtlEvent_last";	break;
	}
	return a;
}

const char * StdAckMsgDescription(const VibCtlStdAckMsg *m)
{
	static char aa[120];
	sprintf(aa,"VibCtlStdAckMsg: cmdCode=%s ecode=%s",
		VibCtlMsgIdName(m->cmdCode),
		VibCtlErrName(m->ecode));
	return aa;
}

const char * TestCtlAckMsgDescription(const VibCtlTestCtlAckMsg *m)
{
	static char aa[240];
	char bb[120];
	switch(m->ecode)
	{
	default:
		bb[0] = 0;
		break;

	case VibCtlErr_InvalPar:
		sprintf(bb,"BadVal=%.4f MinAllowed=%.4f MaxAllowed=%.4f",
			m->fVal, m->fMin, m->fMax);
		break;

	}
	sprintf(aa,"VibCtlTestCtlAckMsg: cmdCode=%s ecode=%s %s",
		VibCtlMsgIdName(m->cmdCode),
		VibCtlErrName(m->ecode),
		bb);
	return aa;

}

const char * TestLoadAckMsgDescription(const VibCtlTestLoadAckMsg *m)
{
	static char aa[MAX_PATH + 120];
	sprintf(aa,"VibCtlTestLoadAckMsg: ackCode=%s ecode=%s path=%s",
		VibCtlTestAckCodeName(m->ackCode),
		VibCtlErrName(m->ecode),
		m->szTestSetupPath);
	return aa;
}

const char * TestCtlMsgDescription(const VibCtlTestCtlMsg *m)
{
	static char aa[240];

	sprintf(aa,"VibCtlTestCtlMsg: msgId=%s fLevel=%.4f",
		VibCtlMsgIdName(m->iMsgId), m->fLevel);
	return aa;
}


const char * VibCtlTestAckCodeName(eLoadAckCode code)
{
	static char aa[60];
	char *a;
	switch(code)
	{
	default: sprintf(aa,"LoadAckCode=%d",(int)code);a = aa; break;
	case loadAck_TestSetupPrompt: a = "loadAck_TestSetupPrompt";	break;
	case loadAck_TestSetupComplete: a = "loadAck_TestSetupComplete";	break;
	case loadAck_ProfilePrompt: a = "loadAck_ProfilePrompt";	break;
	case loadAck_ProfileComplete: a = "loadAck_ProfileComplete";	break;
	case loadAck_SchedulePrompt: a = "loadAck_SchedulePrompt";	break;
	case loadAck_ScheduleComplete: a = "loadAck_ScheduleComplete";	break;
	case loadAck_Pwd: a = "loadAck_Pwd";	break;
	case loadAck_Echo: a = "loadAck_Echo";	break;
	case loadAck_Last: a = "loadAck_Last";	break;
	case loadAck_TestSetupQuery: a = "loadAck_TestSetupQuery"; break;
	case loadAck_ProfileQuery: a = "loadAck_ProfileQuery"; break;
	case loadAck_ScheduleQuery: a = "loadAck_ScheduleQuery"; break;
	}
	return a;
}

const char * VibCtlAlarmFormatter(const VibCtlAlarmMsg *m)
{
	// This is the do-it-yourself part.
	// You could add hi/lo, limit info, bAborted, iChanType, etc.
	static CString z;
	z = VibCtlAlarmName(m->type);

	if(m->bChanTypeEtcValid)
	{
		ASSERT(0); // E_INCOMPLETE
	}
	if(m->dscr[0])
	{
		z += _T("\r\n\t");
		z += m->dscr;
	}
	return z;
}

const char * VibCtlAlarmName(eVibCtlAlarm code)
{
	static char aa[60];
	char *a;
	switch(code)
	{
	default: sprintf(aa,"AlarmCode=%d",(int)code);	a = aa; break;
	case VibCtlAlarm_allClear:			a = "VibCtlAlarm_AllClear";		break;
	case VibCtlAlarm_none:				a = "VibCtlAlarm_none";			break;
	case VibCtlAlarm_DspFailure:		a = "VibCtlAlarm_DspFailure";	break;
	case VibCtlAlarm_ControlError:		a = "VibCtlAlarm_ControlError";	break;
	case VibCtlAlarm_HighAlarm:			a = "VibCtlAlarm_HighAlarm";	break;
	case VibCtlAlarm_HighAbort:			a = "VibCtlAlarm_HighAbort";	break;
	case VibCtlAlarm_LowAlarm:			a = "VibCtlAlarm_LowAlarm";		break;
	case VibCtlAlarm_LowAbort:			a = "VibCtlAlarm_LowAbort";		break;
	case VibCtlAlarm_DriveVoltsHighAlarm:	a = "VibCtlAlarm_DriveVoltsHighAlarm";	break;
	case VibCtlAlarm_DriveVoltsHighAbort:	a = "VibCtlAlarm_DriveVoltsHighAbort";	break;
	case VibCtlAlarm_Unspecified:			a = "VibCtlAlarm_Unspecified";			break;
	case VibCtlAlarm_OperatorAbort:			a = "VibCtlAlarm_OperatorAbort";		break;
	case VibCtlAlarm_ShakerLimits:			a = "VibCtlAlarm_ShakerLimits";			break;
	case VibCtlAlarm_last:					a = "VibCtlAlarm_last";					break;
		
	}
	return a;
}

eVibCtlErr vibCtlErrFromEcode(Ecode er)
{	
	eVibCtlErr err = VibCtlErr_Badstate;

	switch(er)
	{
	default:
		ASSERT(0);	// Tell me, then fake it
		err = VibCtlErr_Badstate;
		break;

	case E_BAD_SAMPRATE:
	case E_SAMPRATE_MIN:
	case E_SAMPRATE_MAX:
		err = VibCtlErr_InvalPar;
		break;

	case E_INVAL:
		err = VibCtlErr_InvalPar;
		break;

	case E_SUCCESS:
		err = VibCtlErr_Success;
		break;

	case E_WOULDBLOCK:	// IsModified()
		err = VibCtlErr_NeedFileSave;
		break;

	case E_BADSTATE:
		err = VibCtlErr_Badstate;
		break;

	case E_FILEACCESS:
	case E_FILEOPEN:
	case E_EXIST:
		err = VibCtlErr_FileNotFound;
		break;
	case E_FILEREAD:
		err = VibCtlErr_FileOpenError;
		break;

	case E_FILECOMPAT:
	case E_FILEFORMAT:
	case E_FILEOPTIONS:

		err = VibCtlErr_FileFormatError;
		break;
	}

	return err;
}

// EOF
