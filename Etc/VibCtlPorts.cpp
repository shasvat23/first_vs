//
// VIbCtlPorts.h
//

//////////////////////////////////////////////////////////////////////////
// In general, how to select a "port" ?
// Go to www.iana.org and look for unassigned
// 	Here's iana "unassigned" as of today, 01/04/2017
// http://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml?&page=131
/*
Port numbers are assigned in various ways, based on three ranges: System
Ports (0-1023), User Ports (1024-49151), and the Dynamic and/or Private
Ports (49152-65535); the difference uses of these ranges is described in

*/
// So note the range of ports is unsigned short (65535)
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "VibCtlPorts.h"
#include "PrivateProfileStuff.h"
#include "fnsplit.h"
#include "\Src\Src_Common\Inc\Eagle_Paths.h"

// Jan 2017, common code
#ifndef VibCtlPort_iniFilePath_local
#error "This should be derined in "Eagle_Paths.h"
#endif 


#define VibCtlPort_iniFileSection "PORTS"
#define VibCtlHost_iniFileSection "HOSTS"

//////////////////////////////////////////////////////////////////////////
// Host addr for the Eagle Embedded Controller
//	Goal:
//		If cmd args are given, they have precedence
//		getVibCtlHostFromCmdLine()
//			/Localhost
//			/Hostname
//			/Host="Fred"
//			/Host=192.168.xx.xxx
//	If cmd args are not gvien, then we go to this .ini file
//  
//  We have up to 4 Eagle embedded controllers on the private local-area-net,
//  they are referenced by "Unit".  This is distinct from the possibility
//  that a single Eagle has multiple NI Board-clusters, each of which are called "lun"
//  So "unit" references a host addr, whil "lun" references a port.

//////////////////////////////////////////////////////////////////////////


#define _VibCtlHost_MaxControllerUnits 4

#define _Default_Eagle_Addr_Unit_0	"192.168.2.200"
#define _Default_Eagle_Addr_Unit_1	"192.168.2.300"
#define _Default_Eagle_Addr_Unit_2	"192.168.2.400"
#define _Default_Eagle_Addr_Unit_3	"192.168.2.500"

//////////////////////////////////////////////////////////////////////////
// Ports for communicating with the Eagle File Server
// Each Eagle "Unit" has two instances of the Eagle File Server.
// If you don't know his port, you cannot connect
// So although these could be changed in this .ini file
// I think (personally) it would have made more sense to leave them hard-coded 
//////////////////////////////////////////////////////////////////////////

#define _EagleFileServerBasePort	48991	
#define _EagleFileServer_PortA	  ( _EagleFileServerBasePort	)					
#define _EagleFileServer_PortB    ( _EagleFileServerBasePort + 1) 
	// 48991 is prime, and is largest prime in "Unassigned" range.  Sounds lucky!


#define _EagleRemoteLaunch_Port (_EagleFileServer_PortA -1  )
	// April 24, 2017 -- used for dll that provides remote launch service, it connects to the launcher
	// But why is this NOT _Dll_For_LaunchControl_BasePort ?


//////////////////////////////////////////////////////////////////////////
// Ports for communication TCM->Gui
// We have up to 4 LUN's in an Eagle, so we need up to 4 ports
// We do not care about which app it is (currently)
// We do not care about whether it is emulator
//////////////////////////////////////////////////////////////////////////
#define _GuiToTcm_BasePort (_EagleFileServerBasePort + 4) 
	// 48991 and 48992 are used by GeneralPurposeServer;
	// the next two are reserved for future use by the Server
	// How many of these will we need?
	// Perhaps x (num lun = 4)
	// Perhaps x (num control modes = 4)
	// So as many as 16 ports can easily be used.
	// Let's reserve 64 of them -- maybe more -- see LabViewDll_For_LaunchControl_BasePort

//////////////////////////////////////////////////////////////////////////
// Ports for communication GUI to LabView Dll's
//  One is for control of the controller (start/stop/load-test etc)
//  The other is for launching the controller
//////////////////////////////////////////////////////////////////////////
#define _Dll_For_LaunchControl_BasePort		   (_EagleFileServerBasePort + 100)
#define _Dll_For_ControllerStartStop_BasePort  (_EagleFileServerBasePort + 120)

//////////////////////////////////////////////////////////////////////////
// Ports for communicating special apps -- in this case the 
// 6Dof Static Control system
#define _6DofStaticCtlSystem_BasePort	 (_EagleFileServerBasePort + 130)


//////////////////////////////////////////////////////////////////////////
// Keywords for the .ini file; "Host addrs"
//////////////////////////////////////////////////////////////////////////
#define _Launcher_And_Gui_Host_Addr_Keyword		"Launcher_And_Gui_Host_Addr"
#define _Eagle_HostAddr_Keyword_0		"Eagle_HostAddr_0"
#define _Eagle_HostAddr_Keyword_1		"Eagle_HostAddr_1"
#define _Eagle_HostAddr_Keyword_2		"Eagle_HostAddr_2"
#define _Eagle_HostAddr_Keyword_3		"Eagle_HostAddr_3"

//////////////////////////////////////////////////////////////////////////
// Keywords for the .ini file; "Ports"
//////////////////////////////////////////////////////////////////////////
#define _EagleFileServerPort_Keyword_A		 "EagleFileServerPort_A"
#define _EagleFileServerPort_Keyword_B		 "EagleFileServerPort_B"
#define _GuiToTcm_Keyword					 "Gui_To_Tcm_Base_Port"
#define _Remote_Control_Dll_BasePort_Keyword "Remote_Control_Dll_Base_Port"



static LPCTSTR VibCtlHost_Defaults[_VibCtlHost_MaxControllerUnits] = {
	_Default_Eagle_Addr_Unit_0,
	_Default_Eagle_Addr_Unit_1,
	_Default_Eagle_Addr_Unit_2,
	_Default_Eagle_Addr_Unit_3
};

static LPCTSTR VibCtlHost_Keyword_per_controller[_VibCtlHost_MaxControllerUnits] = {
	_Eagle_HostAddr_Keyword_0, 
	_Eagle_HostAddr_Keyword_1,
	_Eagle_HostAddr_Keyword_2,
	_Eagle_HostAddr_Keyword_3
};

static void accPortAny(LPCTSTR keyword, int &port, int bSet)
{
	makeDirAndUpperDirs(VibCtlPort_iniFilePath_local);
	setPrivateProfIniFilePath(VibCtlPort_iniFilePath_local, PrivateProfOpt_AddMissingEntries);
	setPrivateProfIniFileSection(VibCtlPort_iniFileSection);
	accPrivateProfInt(keyword, &port, FALSE); 
}

static int getPortAny(LPCTSTR keyword, int defaultValue)
{
	int port = defaultValue;
	accPortAny(keyword, port, FALSE);
	return port;
}

static void setPortAny(LPCTSTR keyword, int port)
{
	accPortAny(keyword, port, TRUE);
}

int getPortForRemoteGuiControlDll(int lun)
{
	int x = getPortAny(_Remote_Control_Dll_BasePort_Keyword, _Dll_For_ControllerStartStop_BasePort);
	x += lun;
	return x;
}

LPCTSTR getHostForRemoteGuiControlDll(int iUnit)
{
	return getEagleRemoteLaunch_HostAddr(iUnit);
}

int getVibControllerBasePort(void)
{
	int x = getPortAny(_GuiToTcm_Keyword, _GuiToTcm_BasePort);
	return x;
}

void setVibControllerBasePort(int port)
{
	setPortAny(_GuiToTcm_Keyword, port);
}

int getVibControllerPort(int lun)
{
	int x = getVibControllerBasePort();
	x += lun;
	return x;
}

int getVibControllerPortFromCmdLine()
{
	// Get from iArgGiven("/Port=") or else from getVibControllerPort
	int port=0;
	int b = iArgGiven("/Port=", &port);
	if(!b)
		port=0;
	return port;
}

int getVibControllerPortEx(int lun)
{
#ifdef _DEBUG_no_no_keep_clam
	static int cnt=0;
	ASSERT(!cnt++);
#endif 

	// Get from iArgGiven("/Port=") or else from getVibControllerPort
	int port=getVibControllerPortFromCmdLine();
	if(!port)
		port = getVibControllerPort(lun);
	return port;
}

int getGeneralPurposeServerPort_A(void)
{
	int x = getPortAny(_EagleFileServerPort_Keyword_A, _EagleFileServer_PortA);
	return x;
}

int getGeneralPurposeServerPort_B(void)
{
	int x = getPortAny(_EagleFileServerPort_Keyword_B, _EagleFileServer_PortB);
	return x;
}

void setGeneralPurposeServerPort_A(int port)
{
	setPortAny(_EagleFileServerPort_Keyword_A, port);
}

void setGeneralPurposeServerPort_B(int port)
{
	setPortAny(_EagleFileServerPort_Keyword_B, port);
}

static void accVibCtlHost(int ControllerUnit, CString & csHost, int bSet)
{
	if(ControllerUnit < 0)
	{
		ASSERT(0);
		ControllerUnit = 0;
	}
	if(ControllerUnit >= _VibCtlHost_MaxControllerUnits)
	{
		ASSERT(0);
		ControllerUnit = _VibCtlHost_MaxControllerUnits-1;
	}

	LPCTSTR keyword =  VibCtlHost_Keyword_per_controller[ ControllerUnit ];

	makeDirAndUpperDirs(VibCtlPort_iniFilePath_local);
	setPrivateProfIniFilePath(VibCtlPort_iniFilePath_local, PrivateProfOpt_AddMissingEntries);
	setPrivateProfIniFileSection(VibCtlHost_iniFileSection);

	CString csDefault = VibCtlHost_Defaults[ControllerUnit];

	if(!bSet)
		csHost = csDefault;
	accPrivateProfStr(keyword, csHost, bSet);
}

LPCTSTR getVibCtlHost(int ControllerUnit)
{
	static CString aa;
	aa.Empty();
	accVibCtlHost(ControllerUnit, aa, FALSE);
	return aa;
}

LPCTSTR getVibCtlHostEx_keyword() // Returns /Host=  or whatever the keyword is after alpha review
{
	return "/Host=";
}

LPCTSTR getVibCtlHostFromCmdLine(void)
{
	// Gets from strArgGiven(/Host=<xx> or /localhost
	// Or of those are not given, from "getVibCtlHost()"
	static CString aa;
	aa.Empty();

	CString key = getVibCtlHostEx_keyword();
	int b = strArgGiven(key, aa);		// strArgGiven("/Host=", aa);
	if(!b)
	{
		b = ArgGiven("/Localhost");	// ArgGiven is case-insensitive
		if(b)
			aa = "localhost";	// This string IS case-sensitive
	}

	if(!b)
	{
		b = ArgGiven("/Hostname");
		if(b)
		{
			char name[MAX_PATH];
			int iret = gethostname(name, ARRAYSIZE(name));
			if(iret == 0)
				aa = name;
		}
	}
	
	if(!b)
		aa.Empty();

	return aa;
}

LPCTSTR getVibCtlHostEx(int ControllerUnit)
{
	// Gets from strArgGiven(/Host=<xx> or /localhost
	// Or of those are not given, from "getVibCtlHost()"

#ifdef _DEBUG_nomore
	static int cnt=0;
	ASSERT(!cnt++);
#endif 

	if(ArgGiven("/Hawk"))
		return "localhost";

	static CString aa;
	aa.Empty();

	aa = getVibCtlHostFromCmdLine();
	if( aa.IsEmpty())
		aa = getVibCtlHost(ControllerUnit);
	return aa;
}

void setVibCtlHost(int ControllerUnit, LPCTSTR szHost)
{
	CString aa = szHost;
	accVibCtlHost(ControllerUnit, aa, TRUE);
}

int getEagleRemoteLaunch_Port(int  lun)
{
	return ( _EagleRemoteLaunch_Port ); // April 20, 2017 this was _EagleFileServer_PortA -1, why is we want to get the launcher, not the server
}

LPCTSTR getEagleRemoteLaunch_HostAddr(int iUnit)
{
	//return "localhost";	// Kiss again.

	int bSet = 0;
	makeDirAndUpperDirs(VibCtlPort_iniFilePath_local);
	setPrivateProfIniFilePath(VibCtlPort_iniFilePath_local, PrivateProfOpt_AddMissingEntries);
	setPrivateProfIniFileSection(VibCtlHost_iniFileSection);

	CString csDefault = "localhost";
	static CString csHost;
	csHost = csDefault;

	accPrivateProfStr(_Launcher_And_Gui_Host_Addr_Keyword, csHost, bSet);

	return csHost;
}

int	get6DofStaticCtl_Port(void)
{
	return _6DofStaticCtlSystem_BasePort;
}

LPCTSTR get6DofStaticCtl_Host(void)
{
	return getVibCtlHostEx();
}

#ifdef _HAWK_Dep // Old JCM and early Eugen stuff
#define VibCtlPort_BaseAddr_Default 1270
#define VibCtlPort_iniFilePath	"c:\\users\\public\\documents\\MB Dynamics\\VibControlCommon\\Sys\\VibCtlPorts.ini"
#define VibCtlPort_iniFileSection "PORTS"
#define VibCtlPort_Keyword	"ControllerPort_BaseAddr"

int getVibCtlPort_KISS(int iUnit, int bEmulator)
{
	makeDirAndUpperDirs(VibCtlPort_iniFilePath);
	setPrivateProfIniFilePath(VibCtlPort_iniFilePath, PrivateProfOpt_AddMissingEntries);
	setPrivateProfIniFileSection(VibCtlPort_iniFileSection);
	int x = VibCtlPort_BaseAddr_Default;
	accPrivateProfInt(VibCtlPort_Keyword, &x, FALSE); // This sets the default
	x += iUnit;
	if(bEmulator)
		x += 20;
	return x;
}
#endif 
