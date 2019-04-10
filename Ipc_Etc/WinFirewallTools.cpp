//
// WinFirewallTools.cpp
// The first section of this file is
// a bit of sample code from MSDN,
// with "printf" changed to "msg" and "warn" prn.
//
// The next section is a C++ wrapper class
// See WinFirewallTools.h for the class definition.

//////////////////////////////////////////////////////////////////////////////////
// Section I: Demo code from MSDN
// From https://msdn.microsoft.com/en-us/library/aa364726(v=VS.85).aspx
//////////////////////////////////////////////////////////////////////////////////
/*
    Copyright (c) Microsoft Corporation

    SYNOPSIS

        Sample code for the Windows Firewall COM interface.
*/

#include "Stdafx.h"	// DSL
#include <windows.h>
#include <crtdbg.h>
#include <netfw.h>
#include <objbase.h>
#include <oleauto.h>

#include "msg.h"	// DSL
// #include <stdio.h>


#pragma comment( lib, "ole32.lib" )
#pragma comment( lib, "oleaut32.lib" )

#if 1 // DSL -- accumulate function prototypes

static  HRESULT WindowsFirewallInitialize(OUT INetFwProfile** fwProfile);

static void WindowsFirewallCleanup(IN INetFwProfile* fwProfile);

static HRESULT WindowsFirewallIsOn(IN INetFwProfile* fwProfile, OUT BOOL* fwOn);


static HRESULT WindowsFirewallAppIsEnabled(
	IN INetFwProfile* fwProfile,
	IN const wchar_t* fwProcessImageFileName,
	OUT BOOL* fwAppEnabled
	);


static HRESULT WindowsFirewallAddApp(
	IN INetFwProfile* fwProfile,
	IN const wchar_t* fwProcessImageFileName,
	IN const wchar_t* fwName
	);

static HRESULT WindowsFirewallDisableApp(
	IN INetFwProfile* fwProfile,
	IN const wchar_t* fwProcessImageFileName
	);

static HRESULT WindowsFirewallPortIsEnabled(
	IN INetFwProfile* fwProfile,
	IN LONG portNumber,
	IN NET_FW_IP_PROTOCOL ipProtocol,
	OUT BOOL* fwPortEnabled
	);

static HRESULT WindowsFirewallPortAdd(
	IN INetFwProfile* fwProfile,
	IN LONG portNumber,
	IN NET_FW_IP_PROTOCOL ipProtocol,
	IN const wchar_t* name
	);

static HRESULT WindowsFirewallPortDisable(
	IN INetFwProfile* fwProfile,
	IN LONG portNumber,
	IN NET_FW_IP_PROTOCOL ipProtocol
	);

#endif // DSL -- accumulate

static  HRESULT WindowsFirewallInitialize(OUT INetFwProfile** fwProfile)
{
    HRESULT hr = S_OK;
    INetFwMgr* fwMgr = NULL;
    INetFwPolicy* fwPolicy = NULL;

    _ASSERT(fwProfile != NULL);

    *fwProfile = NULL;

    // Create an instance of the firewall settings manager.
    hr = CoCreateInstance(
            __uuidof(NetFwMgr),
            NULL,
            CLSCTX_INPROC_SERVER,
            __uuidof(INetFwMgr),
            (void**)&fwMgr
            );
    if (FAILED(hr))
    {
        msg("WindowsFirewallInitialize: CoCreateInstance failed:", (DWORD)hr);
        goto error;
    }

    // Retrieve the local firewall policy.
    hr = fwMgr->get_LocalPolicy(&fwPolicy);
    if (FAILED(hr))
    {
        msg("WindowsFirewallInitialize: get_LocalPolicy failed: 0x%08lx\n", hr);
        goto error;
    }

    // Retrieve the firewall profile currently in effect.
    hr = fwPolicy->get_CurrentProfile(fwProfile);
    if (FAILED(hr))
    {
        msg("WindowsFirewallInitialize: get_CurrentProfile failed: 0x%08lx\n", hr);
        goto error;
    }

	   return hr;

error:

    // Release the local firewall policy.
    if (fwPolicy != NULL)
    {
        fwPolicy->Release();
    }

    // Release the firewall settings manager.
    if (fwMgr != NULL)
    {
        fwMgr->Release();
    }

    return hr;
}


static void WindowsFirewallCleanup(IN INetFwProfile* fwProfile)
{
    // Release the firewall profile.
    if (fwProfile != NULL)
    {
        fwProfile->Release();
    }
}


static HRESULT WindowsFirewallIsOn(IN INetFwProfile* fwProfile, OUT BOOL* fwOn)
{
    HRESULT hr = S_OK;
    VARIANT_BOOL fwEnabled;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwOn != NULL);

    *fwOn = FALSE;

    // Get the current state of the firewall.
    hr = fwProfile->get_FirewallEnabled(&fwEnabled);
    if (FAILED(hr))
    {
        warn("get_FirewallEnabled failed");
        goto error;
    }

    // Check to see if the firewall is on.
    if (fwEnabled != VARIANT_FALSE)
    {
        *fwOn = TRUE;
        warn("The firewall is on.\n");
    }
    else
    {
        warn("The firewall is off.\n");
    }

error:

    return hr;
}

#if 0 // DSL -- don't need this
static HRESULT WindowsFirewallTurnOn(IN INetFwProfile* fwProfile)
{
    HRESULT hr = S_OK;
    BOOL fwOn;

    _ASSERT(fwProfile != NULL);

    // Check to see if the firewall is off.
    hr = WindowsFirewallIsOn(fwProfile, &fwOn);
    if (FAILED(hr))
    {
        warn("WindowsFirewallIsOn failed: 0x%08lx\n", hr);
        goto error;
    }

    // If it is, turn it on.
    if (!fwOn)
    {
        // Turn the firewall on.
        hr = fwProfile->put_FirewallEnabled(VARIANT_TRUE);
        if (FAILED(hr))
        {
            warn("put_FirewallEnabled failed: 0x%08lx\n", hr);
            goto error;
        }

        warn("The firewall is now on.\n");
    }

error:

    return hr;
}
#endif // DSL -- dont need this

#if 0 // DSL -- don't need this
static HRESULT WindowsFirewallTurnOff(IN INetFwProfile* fwProfile)
{
    HRESULT hr = S_OK;
    BOOL fwOn;

    _ASSERT(fwProfile != NULL);

    // Check to see if the firewall is on.
    hr = WindowsFirewallIsOn(fwProfile, &fwOn);
    if (FAILED(hr))
    {
        warn("WindowsFirewallIsOn failed: 0x%08lx\n", hr);
        goto error;
    }

    // If it is, turn it off.
    if (fwOn)
    {
        // Turn the firewall off.
        hr = fwProfile->put_FirewallEnabled(VARIANT_FALSE);
        if (FAILED(hr))
        {
            warn("put_FirewallEnabled failed: 0x%08lx\n", hr);
            goto error;
        }

        warn("The firewall is now off.\n");
    }

error:

    return hr;
}
#endif // #if 0 // DSL -- don't need this



static HRESULT WindowsFirewallAppIsEnabled(
            IN INetFwProfile* fwProfile,
            IN const wchar_t* fwProcessImageFileName,
            OUT BOOL* fwAppEnabled
            )
{
    HRESULT hr = S_OK;
    BSTR fwBstrProcessImageFileName = NULL;
    VARIANT_BOOL fwEnabled;
    INetFwAuthorizedApplication* fwApp = NULL;
    INetFwAuthorizedApplications* fwApps = NULL;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwProcessImageFileName != NULL);
    _ASSERT(fwAppEnabled != NULL);

    *fwAppEnabled = FALSE;

    // Retrieve the authorized application collection.
    hr = fwProfile->get_AuthorizedApplications(&fwApps);
    if (FAILED(hr))
    {
        warn("get_AuthorizedApplications failed");
        goto error;
    }

    // Allocate a BSTR for the process image file name.
    fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
    if (fwBstrProcessImageFileName == NULL)
    {
        hr = E_OUTOFMEMORY;
        warn("SysAllocString failed");
        goto error;
    }

    // Attempt to retrieve the authorized application.
    hr = fwApps->Item(fwBstrProcessImageFileName, &fwApp);
    if (SUCCEEDED(hr))
    {
        // Find out if the authorized application is enabled.
        hr = fwApp->get_Enabled(&fwEnabled);
        if (FAILED(hr))
        {
            warn("get_Enabled failed: 0x%08lx\n", hr);
            goto error;
        }

        if (fwEnabled != VARIANT_FALSE)
        {
            // The authorized application is enabled.
            *fwAppEnabled = TRUE;
#if 0 // DSL, comment out
			CString aa;
			aa.Format("Authorized application %lS is enabled in the firewall.\n",
				fwProcessImageFileName
				);
            warn(aa);
#endif 
        }
        else
        {
#if 0 // DSL, comment out
			CString aa;
			aa.Format("Authorized application %lS is disabled in the firewall.\n",
				fwProcessImageFileName
				);
            warn(aa);
#endif 
        }
    }
    else
    {
        // The authorized application was not in the collection.
        hr = S_OK;
#if 0 // DSL, comment out
		CString aa;
        aa.Format(
            "Authorized application %lS is disabled in the firewall.\n",
            fwProcessImageFileName
            );
		warn(aa);
#endif 
    }

error:

    // Free the BSTR.
    SysFreeString(fwBstrProcessImageFileName);

    // Release the authorized application instance.
    if (fwApp != NULL)
    {
        fwApp->Release();
    }

    // Release the authorized application collection.
    if (fwApps != NULL)
    {
        fwApps->Release();
    }

    return hr;
}


#if 1 // DSL new

static HRESULT WindowsFirewallDisableApp(
	IN INetFwProfile* fwProfile,
	IN const wchar_t* fwProcessImageFileName
	)
{
	HRESULT hr = S_OK;
	BSTR fwBstrProcessImageFileName = NULL;
	VARIANT_BOOL fwEnabled;
	INetFwAuthorizedApplication* fwApp = NULL;
	INetFwAuthorizedApplications* fwApps = NULL;

	_ASSERT(fwProfile != NULL);
	_ASSERT(fwProcessImageFileName != NULL);
	
	// Retrieve the authorized application collection.
	hr = fwProfile->get_AuthorizedApplications(&fwApps);
	if (FAILED(hr))
	{
		warn("get_AuthorizedApplications failed: 0x%08lx\n", hr);
		goto error;
	}

	// Allocate a BSTR for the process image file name.
	fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
	if (fwBstrProcessImageFileName == NULL)
	{
		hr = E_OUTOFMEMORY;
		warn("SysAllocString failed: 0x%08lx\n", hr);
		goto error;
	}

	// Attempt to retrieve the authorized application.
	hr = fwApps->Item(fwBstrProcessImageFileName, &fwApp);
	if (SUCCEEDED(hr))
	{
		// Find out if the authorized application is enabled.
		hr = fwApp->get_Enabled(&fwEnabled);
		if (FAILED(hr))
		{
			warn("get_Enabled failed: 0x%08lx\n", hr);
			goto error;
		}

		if (fwEnabled != VARIANT_FALSE)
		{
			// The authorized application is enabled.
			hr = fwApp->put_Enabled(FALSE);
			if(FAILED(hr))
			{
				warn("put_ENabled failed");
				goto error;
			}else{
				// Success
			}
#if 0 // DSL, comment out
			CString aa;
			aa.Format("Authorized application %lS is enabled in the firewall.\n",
				fwProcessImageFileName
				);
			warn(aa);
#endif 
		}
		else
		{
#if 0 // DSL, comment out
			CString aa;
			aa.Format("Authorized application %lS is disabled in the firewall.\n",
				fwProcessImageFileName
				);
			warn(aa);
#endif 
		}
	}
	else
	{
		// The authorized application was not in the collection.
		hr = S_OK;
#if 0 // DSL, comment out
		CString aa;
		aa.Format(
			"Authorized application %lS is disabled in the firewall.\n",
			fwProcessImageFileName
			);
		warn(aa);
#endif 
	}

error:

	// Free the BSTR.
	SysFreeString(fwBstrProcessImageFileName);

	// Release the authorized application instance.
	if (fwApp != NULL)
	{
		fwApp->Release();
	}

	// Release the authorized application collection.
	if (fwApps != NULL)
	{
		fwApps->Release();
	}

	return hr;
}

#endif // DSL new

static HRESULT WindowsFirewallAddApp(
            IN INetFwProfile* fwProfile,
            IN const wchar_t* fwProcessImageFileName,
            IN const wchar_t* fwName
            )
{
    HRESULT hr = S_OK;
    BOOL fwAppEnabled;
    BSTR fwBstrName = NULL;
    BSTR fwBstrProcessImageFileName = NULL;
    INetFwAuthorizedApplication* fwApp = NULL;
    INetFwAuthorizedApplications* fwApps = NULL;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwProcessImageFileName != NULL);
    _ASSERT(fwName != NULL);

    // First check to see if the application is already authorized.
    hr = WindowsFirewallAppIsEnabled(
            fwProfile,
            fwProcessImageFileName,
            &fwAppEnabled
            );
    if (FAILED(hr))
    {
        warn("WindowsFirewallAppIsEnabled failed: 0x%08lx\n", hr);
        goto error;
    }

    // Only add the application if it isn't already authorized.
    if (!fwAppEnabled)
    {
        // Retrieve the authorized application collection.
        hr = fwProfile->get_AuthorizedApplications(&fwApps);
        if (FAILED(hr))
        {
            warn("get_AuthorizedApplications failed: 0x%08lx\n", hr);
            goto error;
        }

        // Create an instance of an authorized application.
        hr = CoCreateInstance(
                __uuidof(NetFwAuthorizedApplication),
                NULL,
                CLSCTX_INPROC_SERVER,
                __uuidof(INetFwAuthorizedApplication),
                (void**)&fwApp
                );
        if (FAILED(hr))
        {
            warn("CoCreateInstance failed: 0x%08lx\n", hr);
            goto error;
        }

        // Allocate a BSTR for the process image file name.
        fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
        if (fwBstrProcessImageFileName == NULL)
        {
            hr = E_OUTOFMEMORY;
            warn("SysAllocString failed: 0x%08lx\n", hr);
            goto error;
        }

        // Set the process image file name.
        hr = fwApp->put_ProcessImageFileName(fwBstrProcessImageFileName);
        if (FAILED(hr))
        {
            warn("put_ProcessImageFileName failed:", hr);
            goto error;
        }

        // Allocate a BSTR for the application friendly name.
        fwBstrName = SysAllocString(fwName);
        if (SysStringLen(fwBstrName) == 0)
        {
            hr = E_OUTOFMEMORY;
            warn("SysAllocString failed: 0x%08lx\n", hr);
            goto error;
        }

        // Set the application friendly name.
        hr = fwApp->put_Name(fwBstrName);
        if (FAILED(hr))
        {
            warn("put_Name failed: 0x%08lx\n", hr);
            goto error;
        }

        // Add the application to the collection.
        hr = fwApps->Add(fwApp);
        if (FAILED(hr))
        {
            warn("Add authorized app failed:", hr);
            goto error;
        }
#if 0 // DSL, comment out
		CString aa;
        aa.Format(
            "Authorized application %lS is now enabled in the firewall.\n",
            fwProcessImageFileName
            );
		warn(aa);
#endif 
    }

error:

    // Free the BSTRs.
    SysFreeString(fwBstrName);
    SysFreeString(fwBstrProcessImageFileName);

    // Release the authorized application instance.
    if (fwApp != NULL)
    {
        fwApp->Release();
    }

    // Release the authorized application collection.
    if (fwApps != NULL)
    {
        fwApps->Release();
    }

    return hr;
}


static HRESULT WindowsFirewallPortIsEnabled(
            IN INetFwProfile* fwProfile,
            IN LONG portNumber,
            IN NET_FW_IP_PROTOCOL ipProtocol,
            OUT BOOL* fwPortEnabled
            )
{
    HRESULT hr = S_OK;
    VARIANT_BOOL fwEnabled;
    INetFwOpenPort* fwOpenPort = NULL;
    INetFwOpenPorts* fwOpenPorts = NULL;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwPortEnabled != NULL);

    *fwPortEnabled = FALSE;

    // Retrieve the globally open ports collection.
    hr = fwProfile->get_GloballyOpenPorts(&fwOpenPorts);
    if (FAILED(hr))
    {
        warn("get_GloballyOpenPorts failed: 0x%08lx\n", hr);
        goto error;
    }

    // Attempt to retrieve the globally open port.
    hr = fwOpenPorts->Item(portNumber, ipProtocol, &fwOpenPort);
    if (SUCCEEDED(hr))
    {
        // Find out if the globally open port is enabled.
        hr = fwOpenPort->get_Enabled(&fwEnabled);
        if (FAILED(hr))
        {
            warn("get_Enabled failed: 0x%08lx\n", hr);
            goto error;
        }

        if (fwEnabled != VARIANT_FALSE)
        {
            // The globally open port is enabled.
            *fwPortEnabled = TRUE;

#if 0 // DSL, comment out
            warn("Port %ld is open in the firewall.\n", portNumber);
#endif 
        }
        else
        {
#if 0 // DSL, comment out
            warn("Port %ld is not open in the firewall.\n", portNumber);
#endif 
        }
    }
    else
    {
        // The globally open port was not in the collection.
        hr = S_OK;

#if 0 // DSL, comment out
        warn("Port %ld is not open in the firewall.\n", portNumber);
#endif 
    }

error:

    // Release the globally open port.
    if (fwOpenPort != NULL)
    {
        fwOpenPort->Release();
    }

    // Release the globally open ports collection.
    if (fwOpenPorts != NULL)
    {
        fwOpenPorts->Release();
    }

    return hr;
}

#if 1 // DSL new

static HRESULT WindowsFirewallPortDisable(
	IN INetFwProfile* fwProfile,
	IN LONG portNumber,
	IN NET_FW_IP_PROTOCOL ipProtocol
	)
{
	HRESULT hr = S_OK;
	VARIANT_BOOL fwEnabled;
	INetFwOpenPort* fwOpenPort = NULL;
	INetFwOpenPorts* fwOpenPorts = NULL;

	_ASSERT(fwProfile != NULL);


	// Retrieve the globally open ports collection.
	hr = fwProfile->get_GloballyOpenPorts(&fwOpenPorts);
	if (FAILED(hr))
	{
		warn("get_GloballyOpenPorts failed: 0x%08lx\n", hr);
		goto error;
	}

	// Attempt to retrieve the globally open port.
	hr = fwOpenPorts->Item(portNumber, ipProtocol, &fwOpenPort);
	if (SUCCEEDED(hr))
	{
		// Find out if the globally open port is enabled.
		hr = fwOpenPort->get_Enabled(&fwEnabled);
		if (FAILED(hr))
		{
			warn("get_Enabled failed: 0x%08lx\n", hr);
			goto error;
		}

		if (fwEnabled != VARIANT_FALSE)
		{
			hr = fwOpenPort->put_Enabled(FALSE);
			if( FAILED(hr))
			{
				warn("put_Enabled(FALSE) Failed");
				goto error;
			}else{
				hr = S_OK;
			}
			

#if 0 // DSL, comment out
			warn("Port %ld is open in the firewall.\n", portNumber);
#endif 
		}
		else
		{
#if 0 // DSL, comment out
			warn("Port %ld is not open in the firewall.\n", portNumber);
#endif 
		}
	}
	else
	{
		// The globally open port was not in the collection.
		hr = S_OK;

#if 0 // DSL, comment out
		warn("Port %ld is not open in the firewall.\n", portNumber);
#endif 
	}

error:

	// Release the globally open port.
	if (fwOpenPort != NULL)
	{
		fwOpenPort->Release();
	}

	// Release the globally open ports collection.
	if (fwOpenPorts != NULL)
	{
		fwOpenPorts->Release();
	}

	return hr;
}
#endif // DSL new


static HRESULT WindowsFirewallPortAdd(
            IN INetFwProfile* fwProfile,
            IN LONG portNumber,
            IN NET_FW_IP_PROTOCOL ipProtocol,
            IN const wchar_t* name
            )
{
    HRESULT hr = S_OK;
    BOOL fwPortEnabled;
    BSTR fwBstrName = NULL;
    INetFwOpenPort* fwOpenPort = NULL;
    INetFwOpenPorts* fwOpenPorts = NULL;

    _ASSERT(fwProfile != NULL);
    _ASSERT(name != NULL);

    // First check to see if the port is already added.
    hr = WindowsFirewallPortIsEnabled(
            fwProfile,
            portNumber,
            ipProtocol,
            &fwPortEnabled
            );
    if (FAILED(hr))
    {
        warn("WindowsFirewallPortIsEnabled failed: 0x%08lx\n", hr);
        goto error;
    }

    // Only add the port if it isn't already added.
    if (!fwPortEnabled)
    {
        // Retrieve the collection of globally open ports.
        hr = fwProfile->get_GloballyOpenPorts(&fwOpenPorts);
        if (FAILED(hr))
        {
            warn("get_GloballyOpenPorts failed: 0x%08lx\n", hr);
            goto error;
        }

        // Create an instance of an open port.
        hr = CoCreateInstance(
                __uuidof(NetFwOpenPort),
                NULL,
                CLSCTX_INPROC_SERVER,
                __uuidof(INetFwOpenPort),
                (void**)&fwOpenPort
                );
        if (FAILED(hr))
        {
            warn("CoCreateInstance failed: 0x%08lx\n", hr);
            goto error;
        }

        // Set the port number.
        hr = fwOpenPort->put_Port(portNumber);
        if (FAILED(hr))
        {
            warn("put_Port failed: 0x%08lx\n", hr);
            goto error;
        }

        // Set the IP protocol.
        hr = fwOpenPort->put_Protocol(ipProtocol);
        if (FAILED(hr))
        {
            warn("put_Protocol failed: 0x%08lx\n", hr);
            goto error;
        }

        // Allocate a BSTR for the friendly name of the port.
        fwBstrName = SysAllocString(name);
        if (SysStringLen(fwBstrName) == 0)
        {
            hr = E_OUTOFMEMORY;
            warn("SysAllocString failed: 0x%08lx\n", hr);
            goto error;
        }

        // Set the friendly name of the port.
        hr = fwOpenPort->put_Name(fwBstrName);
        if (FAILED(hr))
        {
            warn("put_Name failed: 0x%08lx\n", hr);
            goto error;
        }

        // Opens the port and adds it to the collection.
        hr = fwOpenPorts->Add(fwOpenPort);
        if (FAILED(hr))
        {
            warn("Add failed: 0x%08lx\n", hr);
            goto error;
        }

#if 0 // DSL, comment out
        warn("Port %ld is now open in the firewall.\n", portNumber);
#endif 
    }

error:

    // Free the BSTR.
    SysFreeString(fwBstrName);

    // Release the open port instance.
    if (fwOpenPort != NULL)
    {
        fwOpenPort->Release();
    }

    // Release the globally open ports collection.
    if (fwOpenPorts != NULL)
    {
        fwOpenPorts->Release();
    }

    return hr;
}

#if 0 // DSL -- don't need this 
int __cdecl wmain(int argc, wchar_t* argv[])
{
    HRESULT hr = S_OK;
    HRESULT comInit = E_FAIL;
    INetFwProfile* fwProfile = NULL;

    // Initialize COM.
    comInit = CoInitializeEx(
                0,
                COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
                );

   // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
   // initialized with a different mode. Since we don't care what the mode is,
   // we'll just use the existing mode.
   if (comInit != RPC_E_CHANGED_MODE)
   {
        hr = comInit;
        if (FAILED(hr))
        {
            warn("CoInitializeEx failed: 0x%08lx\n", hr);
            goto error;
        }
   }

    // Retrieve the firewall profile currently in effect.
    hr = WindowsFirewallInitialize(&fwProfile);
    if (FAILED(hr))
    {
        warn("WindowsFirewallInitialize failed: 0x%08lx\n", hr);
        goto error;
    }

    // Turn off the firewall.
    hr = WindowsFirewallTurnOff(fwProfile);
    if (FAILED(hr))
    {
        warn("WindowsFirewallTurnOff failed: 0x%08lx\n", hr);
        goto error;
    }

    // Turn on the firewall.
    hr = WindowsFirewallTurnOn(fwProfile);
    if (FAILED(hr))
    {
        printf("WindowsFirewallTurnOn failed: 0x%08lx\n", hr);
        goto error;
    }

    // Add Windows Messenger to the authorized application collection.
    hr = WindowsFirewallAddApp(
            fwProfile,
            L"%ProgramFiles%\\Messenger\\msmsgs.exe",
            L"Windows Messenger"
            );
    if (FAILED(hr))
    {
        warn("WindowsFirewallAddApp failed: 0x%08lx\n", hr);
        goto error;
    }

    // Add TCP::80 to list of globally open ports.
    hr = WindowsFirewallPortAdd(fwProfile, 80, NET_FW_IP_PROTOCOL_TCP, L"WWW");
    if (FAILED(hr))
    {
        warn("WindowsFirewallPortAdd failed: 0x%08lx\n", hr);
        goto error;
    }

error:

    // Release the firewall profile.
    WindowsFirewallCleanup(fwProfile);

    // Uninitialize COM.
    if (SUCCEEDED(comInit))
    {
        CoUninitialize();
    }

    return 0;
}
#endif // DSL dont need this

//////////////////////////////////////////////////////////////////////////////////
// Section II: C++ class to wrapper all this happy stuff
//////////////////////////////////////////////////////////////////////////////////

#include "WinFirewallTools.h"
INetFwProfile* fwProfile = NULL;

CWindowsFirewallCtl::CWindowsFirewallCtl()
	:
	m_fwProfile(0)
	, m_comInit (E_FAIL)
{
	
}

CWindowsFirewallCtl::~CWindowsFirewallCtl()
{
	cleanup();
}

BOOL CWindowsFirewallCtl::init()
{
	// Initialize COM.
	if( m_comInit != E_FAIL)
	{
		m_comInit = CoInitializeEx(
			0,
			COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
			);

		// Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
		// initialized with a different mode. Since we don't care what the mode is,
		// we'll just use the existing mode.
		if (m_comInit != RPC_E_CHANGED_MODE)
		{
		
			if (FAILED(m_comInit))
			{
				warn("CoInitializeEx failed:", m_comInit);
				return 0;
			}
		}
	}

	if(!m_fwProfile)
	{
		// Retrieve the firewall profile currently in effect.
		HRESULT hr = WindowsFirewallInitialize(&m_fwProfile);
		if (FAILED(hr))
		{
			msg("WindowsFirewallInitialize failed", hr);
			return 0;
		}
	}

	return 1;

}

BOOL CWindowsFirewallCtl::cleanup()
{
	// Release the firewall profile.
	if(m_fwProfile)
	{
		WindowsFirewallCleanup(m_fwProfile);
		m_fwProfile = NULL;
	}


	// Uninitialize COM.
	if (SUCCEEDED(m_comInit))
	{
		CoUninitialize();
		m_comInit = E_FAIL;
	}

	return 1;
}

static void Lpctstr_to_wchar(LPCTSTR src, wchar_t *dst, int max)
{
	int bGotTerm=0;
	for(int i=0;i<max;i++)
	{
		dst[i] = (wchar_t)src[i];
		if(!dst[i])
			return;
		
	}
	ASSERT(0);
	dst[max-1] = 0;
}

BOOL CWindowsFirewallCtl::IsAppEnabled(LPCTSTR szImagePath)
{
	BOOL b = init();
	if(!b)
		return 0;

	wchar_t wImagePath[MAX_PATH];
	Lpctstr_to_wchar(szImagePath, wImagePath, MAX_PATH);


	BOOL bEnabled=0;
	HRESULT hr = WindowsFirewallAppIsEnabled(
		m_fwProfile,
		wImagePath,
		&bEnabled
		);
	if (FAILED(hr))
	{
		warn("WindowsFirewallAppIsEnabled failed");
		return 0;
	}

	return bEnabled;


}

BOOL CWindowsFirewallCtl::AddApp(LPCTSTR szImagePath, LPCTSTR shortName)
{
	// First check to see if the application is already authorized.
	BOOL b = IsAppEnabled(szImagePath);
	if(b)
		return 1;

	b = init();
	if(!b)
		return 0;

	wchar_t wImagePath[MAX_PATH];
	wchar_t wShortName[MAX_PATH];
	Lpctstr_to_wchar(szImagePath, wImagePath, MAX_PATH);
	Lpctstr_to_wchar(shortName, wShortName, MAX_PATH);

	HRESULT hr = WindowsFirewallAddApp(
		m_fwProfile,
		wImagePath,
		wShortName
		);
	if (FAILED(hr))
	{
		warn("WindowsFirewallAddApp failed");
		return 0;
	}

	return 1;
}
#if 0
static HRESULT WindowsFirewallDisableApp(
	IN INetFwProfile* fwProfile,
	IN const wchar_t* fwProcessImageFileName
	);
#endif 


BOOL CWindowsFirewallCtl::DisableApp(LPCTSTR szImagePath)
{

	BOOL b = IsAppEnabled(szImagePath);
	if(!b)
		return 1;

	wchar_t wImagePath[MAX_PATH];
	Lpctstr_to_wchar(szImagePath, wImagePath, MAX_PATH);

	HRESULT hr = WindowsFirewallDisableApp(m_fwProfile, wImagePath);

	if( FAILED(hr))
		return 0;

	return 1;
}


BOOL CWindowsFirewallCtl::IsPortEnabled(long port, int bTcp)	//  If not bTcp, then Datagrams
{
	BOOL b = init();
	if(!b)
		return 0;

	IN NET_FW_IP_PROTOCOL ipProtocol = bTcp ? NET_FW_IP_PROTOCOL_TCP : NET_FW_IP_PROTOCOL_UDP;
	BOOL bEnb = 0;
	HRESULT hr = WindowsFirewallPortIsEnabled(m_fwProfile, port, ipProtocol, &bEnb);
	if (FAILED(hr))
		return 0;
	return bEnb;
}

BOOL CWindowsFirewallCtl::AddPort(long port, int bTcp, LPCTSTR portName)		//  If not bTcp, then Datagrams
{
	if(IsPortEnabled(port, bTcp))
		return 1;

	wchar_t wPortName[MAX_PATH];
	Lpctstr_to_wchar(portName, wPortName, MAX_PATH);

	IN NET_FW_IP_PROTOCOL ipProtocol = bTcp ? NET_FW_IP_PROTOCOL_TCP : NET_FW_IP_PROTOCOL_UDP;
	
	HRESULT hr = WindowsFirewallPortAdd(m_fwProfile, port, ipProtocol, wPortName);
	if (FAILED(hr))
		return 0;
	return 1;
	
}

BOOL CWindowsFirewallCtl::DisablePort(long port, int bTcp)
{
	BOOL b = IsPortEnabled(port, bTcp);
	if(!b)
		return 1;

	IN NET_FW_IP_PROTOCOL ipProtocol = bTcp ? NET_FW_IP_PROTOCOL_TCP : NET_FW_IP_PROTOCOL_UDP;

	HRESULT hr = WindowsFirewallPortDisable(m_fwProfile, port, ipProtocol);
	if (FAILED(hr))
		return 0;
	return 1;
}


void WindowsFirewall_Demo()
{
	// Document what you do, where it came from...
	// For instance, you want the original MSDN file mostly unmunged....
	CWindowsFirewallCtl Ctl;
	CString csImage = "c:\\DebugBin\\6Dof\\6Dof_DataSynthesis.exe";
	CString csShortName = "6Dof_DataSynthesis.exe";
	BOOL b = Ctl.IsAppEnabled(csImage);
	if(b)
		msg("App is enabled: ", csShortName);
	else
		msg("App is not enabled: ", csShortName);

	b = Ctl.AddApp(csImage, csShortName);
	if(b)
		msg("App is now newly enabled: ", csShortName);
	else
		msg("App could not be enabled: ", csShortName);

	b = Ctl.DisableApp(csImage);
	if(b)
		msg("App is now newly disabled: ", csShortName);
	else
		msg("App could not be disabled: ", csShortName);

	long port = 1292;
	BOOL bTcp = 1;
	b = Ctl.IsPortEnabled(port, bTcp);
	if(b)
		msg("Port is enabled:", port);
	else
		msg("Port is not enabled:", port);
	b = Ctl.AddPort(port, bTcp, "MyDemoPort");
	if(b)
		msg("Port is newly enabled:", port);
	else
		msg("Port could not be enabled:", port);

	b = Ctl.DisablePort(port, bTcp);
	if(b)
		msg("Port has been disabled:", port);
	else
		msg("Port could not be disabled:", port);


}


#include "fnsplit.h" // For GetMyImagePath()

BOOL enableMyPort(int port)
{
	// We need my image name
	// Can't find this in the WinBase.h
	// But it is in afxwin.h, 
	// CWinApp::m_pszExeName
	// So implement as 
	// AfxGetApp()->m_pszExeName()

	CString csExeName = GetMyImagePath();
	CString csExeShortName = AfxGetApp()->m_pszAppName;

	CWindowsFirewallCtl *f = new CWindowsFirewallCtl();
	f->init();
	BOOL b;
	b = f->IsAppEnabled(csExeName);
	if(!b)
	{
		b = f->AddApp(csExeName, csExeShortName);
		if(!b)
		{
			msg("Could not add myself to the allowed apps for firewall control");
			return 0;
		}
	}

	int bTcp = TRUE;
	b = f->IsPortEnabled(port, bTcp);

	CString csPortName = "MB Dynamics General-Purpose-Server";
	if(!b)
	{
		b = f->AddPort(port, bTcp, csPortName);
		if(!b)
		{
			msg("Could not add port to firewall control");
			return 0;
		}
	}

	f->cleanup();
	delete f;
	return 1;

}
