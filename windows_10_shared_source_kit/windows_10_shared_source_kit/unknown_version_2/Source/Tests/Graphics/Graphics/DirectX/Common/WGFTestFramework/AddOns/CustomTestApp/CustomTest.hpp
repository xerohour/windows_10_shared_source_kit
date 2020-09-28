//////////////////////////////////////////////////////////////////////
// File:  CustomTest.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a custom D3D/DXGI test group base class.
//
// History:
// 10 Mar 2006  SEdmison  Factored from other tests into a shared lib.
// 15 Nov 2006  SEdmison  Move code from CCustomTestApp into
//                        CCustomTest.
//////////////////////////////////////////////////////////////////////

#pragma once


/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Win32 headers
//

#include <windows.h>
#include <basetsd.h>


//
// Direct3D headers
//

#include <d3d10_1.h>
#include <dxgi.h>
#include <d3dx10.h>


//
// Project headers
//

#include "GDIAdapterTopology.hpp"


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CCustomTest
//
// Purpose:
// Custom test group object.
//
// Notes:
// Parameterized by the framework (app) class type name
// and the parent class type name.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
class CCustomTest
:
	public ParentClass
{
	//
	// Construction/destruction
	//

	public:
		CCustomTest
		(
			AppClass * pApplication
		);

		//virtual ~CCustomTest();


	//
	// Public framework method overrides
	//

	public:
		// App-wide setup.
		virtual TEST_RESULT Setup();

		// App-wide cleanup.
		virtual void Cleanup();


	//
	// Public methods
	//

	public:
		// Causes a temporary mode change for the specified device(s).
		HRESULT CauseModeChange
		(
			IN UINT Monitor = MONITOR_UNSPECIFIED,
			IN DWORD Width = 0,
			IN DWORD Height = 0,
			IN DWORD BitDepth = 0,
			IN DWORD RefreshRate = 0,
			IN DWORD Orientation = DMDO_DEFAULT,
			IN DWORD Flags = CDS_FULLSCREEN
		);


		// Check whether the User/GDI display device topology
		// has changed during a test case.
		HRESULT CheckForAdapterTopologyChanges
		(
			BOOL bIncludeDetachedDevices = FALSE
		);


		// Driver model detection code
		DRIVERMODEL DetectDriverModelForDevice
		(
			IN LPCWSTR pDeviceName
		);


		// Enables the specified privilege for the process.
		HRESULT EnablePrivilege
		(
			IN LPCTSTR PrivilegeName
		);

		int GetAngleFromGDIOrientation
		(
			IN DWORD DisplayOrientation
		);


		// Maps an HRESULT to a string.
		virtual LPCTSTR HResultToString
		(
			IN HRESULT hResult
		) const;


		// Determines whether DWM is present and composition
		// is enabled.
		HRESULT IsDWMCompositionEnabled();


		// Log troubleshooting information.
		HRESULT LogDiagnosticsStart();
		HRESULT LogDiagnostics();
		HRESULT LogDiagnosticsEnd();

		// Log whether DWM composition is enabled.
		HRESULT LogDWMCompositionState();

		// Log the User/GDI adapter topology.
		HRESULT LogGDIAdapterTopology();

		// Log information about machine sessions.
		HRESULT LogSessionInformation();

		// Broadcasts a message to suspend the monitor.
		HRESULT SetMonitorPowerState
		(
			IN LPARAM MonitorPowerState
		);

		// Sends a WM_SYSCOMMAND(SC_MONITORPOWER) message.
		HRESULT SetMonitorPowerStateViaWindowMessage
		(
			IN LPARAM MonitorPowerState
		);

		// Calls NtPowerInformation.
		HRESULT SetMonitorPowerStateViaNtApi
		(
			IN LPARAM MonitorPowerState
		);

		// Enable/disable DWM composition UI dialogs (which
		// have a tendency of popping up at inopportune
		// times during a test run)
		HRESULT ToggleDWMCompositionUI
		(
			IN BOOL bRestore
		);

		// A version of Sleep that pumps messages.
		HRESULT WaitAndPumpMessages
		(
			IN DWORD Duration
		);


	//
	// Data members
	//

	public:
		//
		// Framework stuff
		//

		// Pointer to the app object.
		// This is just here to keep us from referring to it
		// via g_Application, the type of which can vary from
		// test to test that uses this library.
		AppClass * m_pApplication;


		//
		// GDI stuff
		//

		// Original foreground window lock timeout.
		DWORD m_OriginalForegroundWindowLockTimeout;

		CGDIAdapterTopology m_OriginalGDITopology;

		bool m_bSetDefaultMode;
		bool m_bRestoreOriginalMode;


		//
		// DWM stuff
		//

		// DWM composition registry settings stuff.
		BOOL m_bCompositionUIValueFound;
		DWORD m_CompositionUIValue;
};
