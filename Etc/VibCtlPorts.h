//
// VibCtlPorts.h
//
#pragma once

#ifdef _HAWK_Dep
int getVibCtlPort_KISS(int iUnit, int bEmulator);
	// This for JCM, and Eugen's old stuff
#endif 

// July 2014, this is how a remote executive gets control of the Random GUI
// Used for John Morrison's Material Compatibility System
// Used for Eugen Reh's "Task Scheduler" 
int     getPortForRemoteGuiControlDll(int lun); 
LPCTSTR getHostForRemoteGuiControlDll(int iUnit);

// January, 2017, this is a port and addr for the remote launch dll (new for Eagle)
int     getEagleRemoteLaunch_Port(int iUnit);
LPCTSTR getEagleRemoteLaunch_HostAddr(int iUnit);

// April 24, 2017, need to run 6Dof static control system with Eagle architecture
int	    get6DofStaticCtl_Port(void);
LPCTSTR get6DofStaticCtl_Host(void);

// January 2017 for Win10 Project
// This is how a standard GUI communicates to the TCM module on the embedded system
int getVibControllerBasePort(void);
int getVibControllerPort(int lun);
int getVibControllerPortEx(int lun);
int getVibControllerPortFromCmdLine();
	// Get from iArgGiven("/Port=") or else from getVibControllerPort

// This is how the launcher and maintenance utilities communicate with the "GeneralPurposeServer"
// It has two ports
int getGeneralPurposeServerPort_A(void);
int getGeneralPurposeServerPort_B(void);

void setGeneralPurposeServerPort_A(int port);
void setGeneralPurposeServerPort_B(int port);

// "Host" -- either a plaintext name, such as "Tetonia" or "DesertSolitaire"
// or "localhost", which means the service is only available on the local host,
// or an Ip4 dot addr, such as "192.168.0.1"
//
// Client: interpret these directly
// Server: if a plaintext name, e.g. first char is not a digit and name is not "localhost",
// then use "HostName()"

LPCTSTR getVibCtlHost(int iControllerPhysicalUnit=0);
void setVibCtlHost(int iControllerPhysicalUnit,  LPCTSTR szHost);

LPCTSTR getVibCtlHostEx(int iControllerPhysicalUnit=0);
	// Gets from strArgGiven(/Host=<xx> /Port=<xxx)
	// Or of those are not given, from "getVibCtlHost()"

LPCTSTR getVibCtlHostFromCmdLine();	// Without regard to lun or bEmulator

LPCTSTR getVibCtlHostEx_keyword(); // Returns /Host=  or whatever the keyword is after alpha review

// EOF