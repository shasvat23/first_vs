//
// WinFirewallTools.h
//
// Here's a C++ class to enable/disable apps or ports in the Windows Firewall.
// 


#pragma once

#include "netfw.h"

class CWindowsFirewallCtl
{
public:
	CWindowsFirewallCtl();
	virtual ~CWindowsFirewallCtl();
	BOOL init();
	BOOL cleanup();
	BOOL IsAppEnabled(LPCTSTR szImagePath);
	BOOL AddApp(LPCTSTR szImagePath, LPCTSTR shortName);
	BOOL DisableApp(LPCTSTR szImagePath);

	BOOL IsPortEnabled(long port, int bTcp);	//  If not bTcp, then Datagrams
	BOOL AddPort(long port, int bTcp, LPCTSTR portName);			//  If not bTcp, then Datagrams
	BOOL DisablePort(long port, int bTcp);

private:
	INetFwProfile* m_fwProfile;
	HRESULT m_comInit;


};

BOOL enableMyPort(int port);


void WindowsFirewall_Demo();

