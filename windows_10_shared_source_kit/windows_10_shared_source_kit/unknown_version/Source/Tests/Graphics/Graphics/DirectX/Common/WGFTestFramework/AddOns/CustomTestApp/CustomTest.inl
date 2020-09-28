//////////////////////////////////////////////////////////////////////
// File:  CustomTest.inl
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Custom D3D/DXGI test group base class.
//
// History:
// 10 Mar 2006  SEdmison  Factored from other tests into a shared lib.
// 15 Nov 2006  SEdmison  Move code from CCustomTestApp into
//                        CCustomTest.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CCustomTest
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
CCustomTest< AppClass, ParentClass >::CCustomTest
(
	AppClass * pApplication
)
:
	m_pApplication( pApplication ),
	m_OriginalForegroundWindowLockTimeout( 0 ),
	m_bSetDefaultMode( true ),
	m_bRestoreOriginalMode( true ),
	m_bCompositionUIValueFound( FALSE ),
	m_CompositionUIValue( 0 )
{
	m_OriginalGDITopology.AttachLogger( m_pApplication->GetLogWrapper() );
};


///////////////////////////////////////////////////////
// Function:  Setup
//
// Purpose:
// Performs app-wide initialization.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
// warning 28251: Inconsistent annotation for 'Setup': this instance has no annotations. 
// See e:\fbl_dgt_dev2\testsrc\windowstest\graphics\directx\common\wgftestframework\addons\customtestapp\customtest.hpp
TEST_RESULT CCustomTest< AppClass, ParentClass >::Setup()
{
	//
	// Initialize locals.
	//

	TEST_RESULT OverallResult = RESULT_FAIL;

	//GUID SubgroupGuid = GUID_VIDEO_SUBGROUP;
	//GUID SettingGuid = GUID_VIDEO_POWERDOWN_TIMEOUT;


	//
	// Enumerate the original GDI adapter topology.
	//

	HRESULT hEnumerateAdaptersResult = m_OriginalGDITopology.EnumerateAdaptersAndOutputs();

	if ( FAILED( hEnumerateAdaptersResult ) )
	{
		WriteToLog
		(
			_T( "Setup:  EnumerateAdaptersAndOutputs failed with error 0x%x (%s)." ),
			hEnumerateAdaptersResult,
			HResultToString( hEnumerateAdaptersResult )
		);

		goto Cleanup;
	};


	//
	// Turn off DWM composition UI, which otherwise would jump up
	// at inopportune times, thus stealing the foreground from the
	// windows created by our tests.
	//

	HRESULT hToggleCompositionUIResult = ToggleDWMCompositionUI( FALSE );

	if ( FAILED( hToggleCompositionUIResult ) )
	{
		// No worries.  We could be running on an old OS.
	};


	//
	// Enable the system shutdown privilege.
	//

	HRESULT hEnableShutdownResult = EnablePrivilege( SE_SHUTDOWN_NAME );

	if ( FAILED( hEnableShutdownResult ) )
	{
		WriteToLog
		(
			_T( "Setup:  EnablePrivilege( SE_SHUTDOWN_NAME ) failed." )
		);
	};


	//
	// Retrieve the original foreground lock timeout.
	//

	SystemParametersInfo
	(
		SPI_GETFOREGROUNDLOCKTIMEOUT,
		0,
		&m_OriginalForegroundWindowLockTimeout,
		0
	);


	//
	// Override that timeout with zero to unlock the foreground
	// window (meaning that we can freely SetForegroundWindow).
	//

	BOOL bOverrideForegroundTimeoutResult = SystemParametersInfo
	(
		SPI_SETFOREGROUNDLOCKTIMEOUT,
		0,
		NULL,
		SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE
	);

	if ( bOverrideForegroundTimeoutResult != TRUE )
	{
		DWORD OverrideForegroundTimeoutResult = GetLastError();

		WriteToLog
		(
			_T( "Setup:  SystemParametersInfo( SPI_SETFOREGROUNDLOCKTIMEOUT ) ")
			_T( "failed with error %u.  This may mean that the test is " )
			_T( "occluded by another window, and may cause failures." ),
			OverrideForegroundTimeoutResult
		);

		//goto Cleanup;
	};


	//
	// Prevent the OS from deciding that our test app is not
	// responding and displaying "ghost" windows over our UI.
	//

	HMODULE hUser32 = GetModuleHandleA( "user32.dll" );
	if ( hUser32 != NULL )
	{
		typedef void (WINAPI *PDISABLEPROCESSWINDOWSGHOSTING)();
		PDISABLEPROCESSWINDOWSGHOSTING pfnNoGhost = (PDISABLEPROCESSWINDOWSGHOSTING) GetProcAddress
		(
			hUser32,
			"DisableProcessWindowsGhosting"
		);

		if ( pfnNoGhost != NULL )
		{
			(*pfnNoGhost)();
			pfnNoGhost = NULL;
		};

		hUser32 = NULL;
	};


	//
	// Adjust display modes on all connected monitors to a reasonable default.
	//

	if
	(
		m_bSetDefaultMode == true &&
		m_pApplication->IsTerminalServices() == false &&
		GetSystemMetrics( SM_REMOTESESSION ) == 0
	)
	{
		HRESULT hSetModeResult = m_OriginalGDITopology.CauseModeChange
		(
			MONITOR_UNSPECIFIED,
			1024,
			768,
			32,
			60,
			DMDO_DEFAULT,
			CDS_UPDATEREGISTRY
		);

		if ( FAILED( hSetModeResult ) )
		{
			WriteToLog
			(
				_T( "Setup:  CauseModeChange failed with error 0x%x (%s)." ),
				hSetModeResult,
				HResultToString( hSetModeResult )
			);

			OverallResult = RESULT_SKIP;
			goto Cleanup;
		};
	};


	//
	// Check for adapter topology changes.
	//

	HRESULT hCheckForAdapterTopologyChangesResult = CheckForAdapterTopologyChanges();

	if ( FAILED( hCheckForAdapterTopologyChangesResult ) )
	{
		WriteToLog
		(
			_T( "Setup:  CheckForAdapterTopologyChanges failed with error 0x%x (%s)." ),
			hCheckForAdapterTopologyChangesResult,
			HResultToString( hCheckForAdapterTopologyChangesResult )
		);

		OverallResult = RESULT_ABORT;
		goto Cleanup;
	}
	else if ( hCheckForAdapterTopologyChangesResult == S_FALSE )
	{
		WriteToLog
		(
			_T( "Setup:  CheckForAdapterTopologyChanges reports that adapter topology has changed during this function." )
		);

		OverallResult = RESULT_ABORT;
		goto Cleanup;
	};


	OverallResult = RESULT_PASS;


Cleanup:
	return OverallResult;
};


///////////////////////////////////////////////////////
// Function:  Cleanup
//
// Purpose:
// Application-wide cleanup.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
// warning 28251: Inconsistent annotation for 'Cleanup': this instance has no annotations. 
// See e:\fbl_dgt_dev2\testsrc\windowstest\graphics\directx\common\wgftestframework\addons\customtestapp\customtest.hpp
void CCustomTest< AppClass, ParentClass >::Cleanup()
{
	//
	// Restore display modes if necessary.
	//

	if
	(
		m_bRestoreOriginalMode == true &&
		m_pApplication->IsTerminalServices() == false &&
		GetSystemMetrics( SM_REMOTESESSION ) == 0
	)
	{
		m_OriginalGDITopology.RestoreOriginalMode();
	};


	//
	// Restore the original foreground window lock timeout.
	//

	SystemParametersInfo
	(
		SPI_SETFOREGROUNDLOCKTIMEOUT,
		0,
		(PVOID) (DWORD_PTR) m_OriginalForegroundWindowLockTimeout,
		SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE
	);


	//
	// Restore prior DWM composition UI setting.
	//

	HRESULT hToggleCompositionUIResult = ToggleDWMCompositionUI( TRUE );

	if ( FAILED( hToggleCompositionUIResult ) )
	{
		// No worries.  We could be running on an old OS.
	};
};


///////////////////////////////////////////////////////
// Function:  CauseModeChange
//
// Purpose:
// Causes a mode change for all devices attached to
// the desktop (that is, all adapters onto which the
// desktop extends).
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
HRESULT CCustomTest< AppClass, ParentClass >::CauseModeChange
(
	IN UINT Monitor,
	IN DWORD Width,
	IN DWORD Height,
	IN DWORD BitDepth,
	IN DWORD RefreshRate,
	IN DWORD Orientation,
	IN DWORD Flags
)
{
	//
	// Make sure that the display is powered on.  Otherwise,
	// ChangeDisplaySettingsEx will fail with DISP_CHANGE_BADPARAM.
	//

	SetMonitorPowerStateViaWindowMessage( MONITOR_ON );


	//
	// Walk the list of adapters that we built previously,
	// causing a mode change on each.
	//

	return m_OriginalGDITopology.CauseModeChange
	(
		Monitor,
		Width,
		Height,
		BitDepth,
		RefreshRate,
		Orientation,
		Flags
	);
};


///////////////////////////////////////////////////////
// Function:  CheckForAdapterTopologyChanges
//
// Purpose:
// Checks to see whether the topology of User/GDI
// display devices has changed from what was originally
// enumerated.
//
// Returns:
// S_OK     If the adapter topologies match.
// S_FALSE  If the adapter topologies differ.
// E_xxxx   If an error was encountered.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
HRESULT CCustomTest< AppClass, ParentClass >::CheckForAdapterTopologyChanges
(
	BOOL bIncludeDetachedDevices
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	CGDIAdapterTopology * pCurrentGDITopology = new CGDIAdapterTopology();

	if ( pCurrentGDITopology == NULL )
	{
		hOverallResult = E_OUTOFMEMORY;
		goto Cleanup;
	};


	//
	// Enumerate the current topology.
	//

	HRESULT hEnumerateAdaptersResult = pCurrentGDITopology->EnumerateAdaptersAndOutputs
	(
		bIncludeDetachedDevices
	);

	if ( FAILED( hEnumerateAdaptersResult ) )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges:  EnumerateAdaptersAndOutputs failed with error 0x%x (%s)." ),
			hEnumerateAdaptersResult,
			HResultToString( hEnumerateAdaptersResult )
		);

		hOverallResult = hEnumerateAdaptersResult;
		goto Cleanup;
	};


	//
	// Compare the result of this enumeration with what we
	// found the first time.
	//

	if ( pCurrentGDITopology->m_cDevices != m_OriginalGDITopology.m_cDevices )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges:  Original topology had %u devices, but this topology has %u devices." ),
			m_OriginalGDITopology.m_cDevices,
			pCurrentGDITopology->m_cDevices
		);

		hOverallResult = S_FALSE;
		goto Cleanup;
	};


	//
	// Compare key aspects of the device descriptors.
	//
    // bogus warning 26018: Potential read overflow of null terminated buffer using 
    // expression '(wchar_t *)(& (pCurrentGDITopology->m_DeviceDescriptors[iCurrentDevice]).DeviceName)'
    // m_OriginalGDITopology.m_cDevices <= 30, see CGDIAdapterTopology::EnumerateAdaptersAndOutputs
#pragma warning( push )
#pragma warning( disable: 26018 )
	for
	(
		UINT iCurrentDevice = 0;
		iCurrentDevice < m_OriginalGDITopology.m_cDevices;
		++iCurrentDevice
	)
	{
		// Check the device name.
		if
		(
			wcscmp
			(
				pCurrentGDITopology->m_DeviceDescriptors[ iCurrentDevice ].DeviceName,
				m_OriginalGDITopology.m_DeviceDescriptors[ iCurrentDevice ].DeviceName
			) != 0 ||
			wcscmp
			(
				pCurrentGDITopology->m_DeviceDescriptors[ iCurrentDevice ].DeviceString,
				m_OriginalGDITopology.m_DeviceDescriptors[ iCurrentDevice ].DeviceString
			)
		)
		{
			WriteToLog
			(
				L"CheckForAdapterTopologyChanges:  Original topology had device %s (%s) at slot %u, but this topology has device %s (%s) at slot %u.",
				m_OriginalGDITopology.m_DeviceDescriptors[ iCurrentDevice ].DeviceName,
				m_OriginalGDITopology.m_DeviceDescriptors[ iCurrentDevice ].DeviceString,
				iCurrentDevice,
				pCurrentGDITopology->m_DeviceDescriptors[ iCurrentDevice ].DeviceString,
				pCurrentGDITopology->m_DeviceDescriptors[ iCurrentDevice ].DeviceName,
				iCurrentDevice
			);

			hOverallResult = S_FALSE;
			goto Cleanup;
		};


		// Check the state flags.
		if
		(
			pCurrentGDITopology->m_DeviceDescriptors[ iCurrentDevice ].StateFlags !=
			m_OriginalGDITopology.m_DeviceDescriptors[ iCurrentDevice ].StateFlags
		)
		{
			WCHAR OriginalStateFlags[ 1024 ] = L"";
			DecodeGDIDeviceStateFlags
			(
				m_OriginalGDITopology.m_DeviceDescriptors[ iCurrentDevice ].StateFlags,
				L"",
				L"|",
				OriginalStateFlags,
				ARRAYSIZE( OriginalStateFlags )
			);

			WCHAR CurrentStateFlags[ 1024 ] = L"";
			DecodeGDIDeviceStateFlags
			(
				pCurrentGDITopology->m_DeviceDescriptors[ iCurrentDevice ].StateFlags,
				L"",
				L"|",
				CurrentStateFlags,
				ARRAYSIZE( CurrentStateFlags )
			);

			WriteToLog
			(
				L"CheckForAdapterTopologyChanges:  Device %u in original topology had state flags %s, but in this topology has state flags %s.",
				iCurrentDevice,
				OriginalStateFlags,
				CurrentStateFlags
			);

			hOverallResult = S_FALSE;
			goto Cleanup;
		};
	};
#pragma warning( pop )


	hOverallResult = S_OK;


Cleanup:
	if ( pCurrentGDITopology != NULL )
	{
		delete pCurrentGDITopology;
		pCurrentGDITopology = NULL;
	};


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  GetAngleFromGDIOrientation
//
// Purpose:
// Returns the angle in degrees corresponding to a
// DEVMODE orientation mode.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
int CCustomTest< AppClass, ParentClass >::GetAngleFromGDIOrientation
(
	IN DWORD DisplayOrientation
)
{
	switch ( DisplayOrientation )
	{
		case DMDO_DEFAULT:
		default:
		{
			return 0;
		};

		case DMDO_90:
		{
			return 90;
		};

		case DMDO_180:
		{
			return 180;
		};

		case DMDO_270:
		{
			return 270;
		};
	};
};


///////////////////////////////////////////////////////
// Function:  IsDWMCompositionEnabled
//
// Purpose:
// Determines whether DWM composition is enabled.
//
// Returns:
// S_OK     DWM is running, and composition is enabled.
// S_FALSE  DWM is not running or composition is
//          disabled.
// E_xxxx   An error was encountered.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
HRESULT CCustomTest< AppClass, ParentClass >::IsDWMCompositionEnabled()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	typedef HRESULT (WINAPI * PDWMISCOMPOSITIONENABLED)
	(
		BOOL * pbCompositionEnabled
	);


	//
	// Load DWMAPI.dll.
	//

	HMODULE hDWMAPI = LoadLibraryExW( L"dwmapi.dll", NULL, 0 );

	if ( hDWMAPI == NULL )
	{
/*
		WriteToLog
		(
			"DWMAPI.dll not found."
		);
*/

		hOverallResult = S_FALSE;
		goto Cleanup;
	};


	//
	// Find the DwmIsCompositionEnabled entry point.
	//

	PDWMISCOMPOSITIONENABLED pDwmIsCompositionEnabled = (PDWMISCOMPOSITIONENABLED) GetProcAddress
	(
		hDWMAPI,
		"DwmIsCompositionEnabled"
	);

	if ( pDwmIsCompositionEnabled == NULL )
	{
/*
		WriteToLog
		(
			"DwmIsCompositionEnabled entry point not found in DWMAPI.dll."
		);
*/

		hOverallResult = S_FALSE;
		goto Cleanup;
	};


	//
	// Call the entry point.
	//

	BOOL bCompositionEnabled = FALSE;
	HRESULT hIsCompositionEnabledResult = (*pDwmIsCompositionEnabled)
	(
		&bCompositionEnabled
	);

	if ( FAILED( hIsCompositionEnabledResult ) )
	{
		WriteToLog
		(
			_T( "IsDWMCompositionEnabled:  DwmIsCompositionEnabled failed with error code 0x%x (%s)." ),
			hIsCompositionEnabledResult,
			HResultToString( hIsCompositionEnabledResult )
		);

		hOverallResult = hIsCompositionEnabledResult;
		goto Cleanup;
	};


	if ( bCompositionEnabled == TRUE )
	{
		hOverallResult = S_OK;
	}
	else
	{
		hOverallResult = S_FALSE;
	};


Cleanup:
	pDwmIsCompositionEnabled = NULL;

	if ( hDWMAPI != NULL )
	{
		FreeLibrary( hDWMAPI );
		hDWMAPI = NULL;
	};


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  LogDiagnosticsStart
//
// Purpose:
// Begins the failure diagnostics section in the log.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
HRESULT CCustomTest< AppClass, ParentClass >::LogDiagnosticsStart()
{
	WriteToLog
	(
		_T( "----------------------------------------------------------------------" )
	);

	WriteToLog
	(
		_T( "Troubleshooting information:" )
	);


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  LogDiagnosticsEnd
//
// Purpose:
// Ends the failure diagnostics section in the log.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
HRESULT CCustomTest< AppClass, ParentClass >::LogDiagnosticsEnd()
{
	WriteToLog
	(
		_T( "----------------------------------------------------------------------" )
	);


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  LogDiagnostics
//
// Purpose:
// Logs information that might be handy in trying to
// troubleshoot a failure.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
HRESULT CCustomTest< AppClass, ParentClass >::LogDiagnostics()
{
	//
	// Log whether composition is enabled.
	//

	LogDWMCompositionState();


	//
	// Log TS information.
	//

	LogSessionInformation();


	//
	// Log the GDI adapter topology.
	//

	LogGDIAdapterTopology();


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  LogDWMCompositionState
//
// Purpose:
// Logs whether DWM composition is enabled.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
HRESULT CCustomTest< AppClass, ParentClass >::LogDWMCompositionState()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = IsDWMCompositionEnabled();

	if ( FAILED( hOverallResult ) )
	{
		WriteToLog
		(
			_T( "IsDWMCompositionEnabled failed with error 0x%x (%s)." ),
			hOverallResult,
			HResultToString( hOverallResult )
		);

		goto Cleanup;
	}
	else if ( hOverallResult == S_OK )
	{
		WriteToLog
		(
			_T( "DWM composition is enabled." )
		);
	}
	else if ( hOverallResult == S_FALSE )
	{
		WriteToLog
		(
			_T( "DWM composition is disabled." )
		);
	}
	else
	{
		WriteToLog
		(
			_T( "IsDWMCompositionEnabled returned unexpected result 0x%x (%s)." ),
			hOverallResult,
			HResultToString( hOverallResult )
		);

		assert( FALSE );
	};


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  LogGDIAdapterTopology
//
// Purpose:
// Logs the User/GDI display device topology.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
HRESULT CCustomTest< AppClass, ParentClass >::LogGDIAdapterTopology()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Enumerate User/GDI devices.
	//

	WriteToLog
	(
		_T( "GDI adapter topology:" )
	);


	DWORD iCurrentDevice = 0;
	for
	(
		iCurrentDevice = 0;
		;
		++iCurrentDevice
	)
	{
		DISPLAY_DEVICEW DisplayDevice;
		ZeroMemory( &DisplayDevice, sizeof( DisplayDevice ) );
		DisplayDevice.cb = sizeof( DisplayDevice );

		BOOL bEnumerateDeviceResult = EnumDisplayDevicesW
		(
			NULL,
			iCurrentDevice,
			&DisplayDevice,
			0
		);

		if ( bEnumerateDeviceResult != TRUE )
		{
			break;
		};


		//
		// Dump information about the device.
		//

		WriteToLog
		(
			L"- Device %u \"%s\" (\"%s\").\n",
			iCurrentDevice,
			DisplayDevice.DeviceName,
			DisplayDevice.DeviceString
		);

		if ( DisplayDevice.StateFlags == 0 )
		{
			WriteToLog
			(
				L"  State flags:\n    [None.]\n"
			);
		}
		else
		{
			WCHAR StateFlags[ 256 ] = L"";
			DecodeGDIDeviceStateFlags
			(
				DisplayDevice.StateFlags,
				L"    ",
				L"|",
				StateFlags,
				ARRAYSIZE( StateFlags )
			);

			WriteToLog
			(
				L"  State flags:\n%s.\n",
				StateFlags
			);
		};


		if ( ( DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) != 0 )
		{
			DEVMODEW CurrentMode;
			ZeroMemory( &CurrentMode, sizeof( CurrentMode ) );
			CurrentMode.dmSpecVersion = DM_SPECVERSION;
			CurrentMode.dmSize = sizeof( CurrentMode );

			BOOL bEnumerateCurrentModeResult = EnumDisplaySettingsExW
			(
				DisplayDevice.DeviceName,
				ENUM_CURRENT_SETTINGS,
				&CurrentMode,
				0
			);

			if ( bEnumerateCurrentModeResult != TRUE )
			{
				WriteToLog
				(
					L"  Mode:  [Indeterminate.  EnumDisplaySettingsEx failed.]\n"
				);
			}
			else
			{
				WriteToLog
				(
					L"  Mode:  %u x %u x %u bpp @ %u Hz.\n",
					CurrentMode.dmPelsWidth,
					CurrentMode.dmPelsHeight,
					CurrentMode.dmBitsPerPel,
					CurrentMode.dmDisplayFrequency
				);
			};
		};


		//
		// Enumerate monitors connected to the device.
		//

		WriteToLog
		(
			L"  Monitors:\n"
		);

		DWORD iCurrentMonitor = 0;
		for
		(
			iCurrentMonitor = 0;
			;
			++iCurrentMonitor
		)
		{
			DISPLAY_DEVICEW Monitor;
			ZeroMemory( &Monitor, sizeof( Monitor ) );
			Monitor.cb = sizeof( Monitor );

			BOOL bEnumerateMonitorResult = EnumDisplayDevicesW
			(
				DisplayDevice.DeviceName,
				iCurrentMonitor,
				&Monitor,
				0
			);

			if ( bEnumerateMonitorResult != TRUE )
			{
				break;
			};


			//
			// Dump information about the monitor.
			//

			WriteToLog
			(
				L"    Monitor %u \"%s\".\n",
				iCurrentMonitor,
				Monitor.DeviceName
			);

			WriteToLog
			(
				L"      Device string:  \"%s\"\n",
				Monitor.DeviceString
			);


			if ( Monitor.StateFlags == 0 )
			{
				WriteToLog
				(
					L"      State flags:\n        [None.]\n"
				);
			}
			else
			{
				WCHAR MonitorStateFlags[ 256 ] = L"";
				DecodeGDIDeviceStateFlags
				(
					Monitor.StateFlags,
					L"        ",
					L"|",
					MonitorStateFlags,
					ARRAYSIZE( MonitorStateFlags )
				);

				WriteToLog
				(
					L"      State flags:\n%s.\n",
					MonitorStateFlags
				);
			};
		};

		WriteToLog
		(
			L"  %u monitor(s) found.\n",
			iCurrentMonitor
		);
	};

	WriteToLog
	(
		L"%u GDI device(s) found.\n",
		iCurrentDevice
	);


	hOverallResult = S_OK;

	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  LogSessionInformation
//
// Purpose:
// Logs information about the session under which the
// application is running.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
HRESULT CCustomTest< AppClass, ParentClass >::LogSessionInformation()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	WTS_SESSION_INFOW * pLogonIDs = NULL;


	//
	// Enumerate logon IDs.
	//

	ULONG cLogonIDs = 0;
	BOOL bEnumerateLogonIDsResult = WTSEnumerateSessionsW
	(
		WTS_CURRENT_SERVER_HANDLE,
		0,
		1,
		&pLogonIDs,
		&cLogonIDs
	);

	if ( !bEnumerateLogonIDsResult )
	{
		DWORD EnumerateLogonIDsError = GetLastError();

		WriteToLog
		(
			_T( "LogSessionInformation:  WTSEnumerate failed with error %u." ),
			EnumerateLogonIDsError
		);

		if ( EnumerateLogonIDsError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( EnumerateLogonIDsError );
		};

		goto Cleanup;
	};


	//
	// Log the logon IDs.
	//

	WriteToLog
	(
		_T( "Machine sessions:" )
	);

	DWORD RemoteConsoleSessionID = 0xFFFFFFFF;
	for
	(
		ULONG iCurrentSession = 0;
		iCurrentSession < cLogonIDs;
		++iCurrentSession
	)
	{
		WriteToLog
		(
			"Session:  %u; state:  %s; name:  %S",
			pLogonIDs[ iCurrentSession ].SessionId,
			GetWTSStateName( pLogonIDs[ iCurrentSession ].State ),
			pLogonIDs[ iCurrentSession ].pWinStationName
		);

		if
		(
			_wcsicmp
			(
				pLogonIDs[ iCurrentSession ].pWinStationName,
				L"Console"
			) == 0
		)
		{
			RemoteConsoleSessionID = pLogonIDs[ iCurrentSession ].SessionId;
		};
	};

	WriteToLog
	(
		_T( "WTSEnumerateSessions says that console session is session ID %u." ),
		RemoteConsoleSessionID
	);


	//
	// Determine the current console session ID.
	//

	DWORD ConsoleSessionID = WTSGetActiveConsoleSessionId();

	WriteToLog
	(
		_T( "WTSGetActiveConsoleSessionId says that console session is session ID %u." ),
		ConsoleSessionID
	);


	//
	// Determine the session ID under which the process is running.
	//

	DWORD SessionID = 0;
	ProcessIdToSessionId( GetCurrentProcessId(), &SessionID );

	WriteToLog
	(
		_T( "ProcessIdToSessionId reports that the process is running under session ID %u." ),
		SessionID
	);


	WriteToLog
	(
		_T( "GetSystemMetrics( SM_REMOTESESSION ) reports %u." ),
		GetSystemMetrics( SM_REMOTESESSION )
	);


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  SetMonitorPowerStateViaWindowMessage
//
// Purpose:
// Broadcasts a system command message to change the
// monitor's power state.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
HRESULT CCustomTest< AppClass, ParentClass >::SetMonitorPowerStateViaWindowMessage
(
	IN LPARAM MonitorPowerState
)
{
	switch ( MonitorPowerState )
	{
		case MONITOR_ON:
		{
			WriteToLog( _T( "SetMonitorPowerStateViaWindowMessage:  Turning monitor on." ) );
		} break;


		case MONITOR_STANDBY:
		{
			WriteToLog( _T( "SetMonitorPowerStateViaWindowMessage:  Setting monitor to standby." ) );

			MonitorPowerState = MONITOR_OFF;
		} break;


		case MONITOR_OFF:
		{
			WriteToLog( _T( "SetMonitorPowerStateViaWindowMessage:  Turning monitor off." ) );
		} break;


		default:
		{
			WriteToLog
			(
				_T( "SetMonitorPowerStateViaWindowMessage:  Invalid monitor power state %d." ),
				MonitorPowerState
			);

			return E_INVALIDARG;
		} break;
	};


	for
	(
		int i = 0;
		i < 3;
		++i
	)
	{
		//
		// Send the monitor power message.
		//

		/* LRESULT SendMessageResult = */ SendMessage
		(
			GetShell()->GetMainHWnd(),
			WM_SYSCOMMAND,
			SC_MONITORPOWER,
			MonitorPowerState
		);

		WaitAndPumpMessages( 100 );
	};


	switch ( MonitorPowerState )
	{
		case MONITOR_ON:
		{
			SetThreadExecutionState( ES_DISPLAY_REQUIRED );
		} break;
	};


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  SetMonitorPowerStateViaNtApi
//
// Purpose:
// Sets the monitor power state using the native API
// NtPowerInformation.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
HRESULT CCustomTest< AppClass, ParentClass >::SetMonitorPowerStateViaNtApi
(
	IN LPARAM MonitorPowerState
)
{
	WriteToLog( _T( "SetMonitorPowerStateViaNtApi:  Setting monitor power state." ) );


	DEVICE_POWER_STATE NewMonitorPowerState = PowerDeviceD3;
	DEVICE_POWER_STATE AdjustedPowerState = PowerDeviceD3;

	if ( MonitorPowerState == MONITOR_ON )
	{
		NewMonitorPowerState = PowerDeviceD0;
	};

	NTSTATUS PowerInfoResult = NtPowerInformation
	(
		SystemVideoState,
		&NewMonitorPowerState,
		sizeof(NewMonitorPowerState),
		&AdjustedPowerState,
		sizeof(AdjustedPowerState)
	);

	if ( !NT_SUCCESS( PowerInfoResult ) )
	{
		WriteToLogF
		(
			_T( "SetMonitorPowerStateViaNtApi:  NtPowerInformation failed with NTSTATUS %u (0x%x)." ),
			PowerInfoResult,
			PowerInfoResult
		);

		return E_FAIL;
	};

	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  SetMonitorPowerState
//
// Purpose:
// Turns the monitor on or off.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
HRESULT CCustomTest< AppClass, ParentClass >::SetMonitorPowerState
(
	IN LPARAM MonitorPowerState
)
{
	//
	// Initialize our overall result.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Set the monitor power state using window messages.
	//

	HRESULT hSetMonitorPowerStateResult = SetMonitorPowerStateViaWindowMessage( MonitorPowerState );

	if ( FAILED( hSetMonitorPowerStateResult ) )
	{
		WriteToLog
		(
			_T( "SetMonitorPowerState:  SetMonitorPowerStateViaWindowMessage failed with error 0x%x (%s)." ),
			hSetMonitorPowerStateResult,
			HResultToString( hSetMonitorPowerStateResult )
		);

		hOverallResult = hSetMonitorPowerStateResult;
		goto Cleanup;
	};


	//
	// If we made it this far, then we're good to go.
	//

	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  WaitAndPumpMessages
//
// Purpose:
// A Sleep command that pumps messages.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
HRESULT CCustomTest< AppClass, ParentClass >::WaitAndPumpMessages
(
	IN DWORD Duration
)
{
	DWORD StartTime = timeGetTime();
	for
	(
		DWORD CurrentTime = StartTime;
		CurrentTime >= StartTime &&
			CurrentTime <= StartTime + Duration;
		CurrentTime = timeGetTime()
	)
	{
		//
		// Process messages or render the scene.
		//

		MSG msg;
		if
		(
			PeekMessage
			(
				&msg,
				NULL,
				0,
				0,
				PM_REMOVE
			)
		)
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		};
	};


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  HResultToString
//
// Purpose:
// Returns a stringized HRESULT.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
LPCTSTR CCustomTest< AppClass, ParentClass >::HResultToString
(
	IN HRESULT hResult
) const
{
	switch ( hResult )
	{
		//
		// Win32/COM status codes
		//

		HANDLE_HRESULT( S_OK );
		HANDLE_HRESULT( S_FALSE );

		HANDLE_HRESULT( E_ACCESSDENIED );
		HANDLE_HRESULT( E_FAIL );
		HANDLE_HRESULT( E_INVALIDARG );
		HANDLE_HRESULT( E_NOINTERFACE );
		HANDLE_HRESULT( E_NOTIMPL );
		HANDLE_HRESULT( E_OUTOFMEMORY );
		HANDLE_HRESULT( E_POINTER );
		HANDLE_WIN32_HRESULT( ERROR_BAD_NETPATH );
		HANDLE_WIN32_HRESULT( ERROR_BAD_PIPE );
		HANDLE_WIN32_HRESULT( ERROR_BROKEN_PIPE );
		HANDLE_WIN32_HRESULT( ERROR_FILE_NOT_FOUND );
		HANDLE_WIN32_HRESULT( ERROR_INVALID_HANDLE );
		HANDLE_WIN32_HRESULT( ERROR_MORE_DATA );
		HANDLE_WIN32_HRESULT( ERROR_NO_DATA );
		HANDLE_WIN32_HRESULT( ERROR_PATH_NOT_FOUND );
		HANDLE_WIN32_HRESULT( ERROR_PIPE_BUSY );
		HANDLE_WIN32_HRESULT( ERROR_VC_DISCONNECTED );


		//
		// Unknown status codes
		//

		default:
		{
			return _T( "Unknown" );
		} break;
	};
};


///////////////////////////////////////////////////////
// Function:  EnablePrivilege
//
// Purpose:
// Asserts the specified security privilege for the
// process.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
HRESULT CCustomTest< AppClass, ParentClass >::EnablePrivilege
(
	IN LPCTSTR PrivilegeName
)
{
	//
	// Validate parameters.
	//

	if ( PrivilegeName == NULL )
	{
		return E_INVALIDARG;
	};


	//
	// Open the process token.
	//

	HANDLE hToken = NULL;
	BOOL bOpenTokenResult = OpenProcessToken
	(
		GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken
	);

	if
	(
		bOpenTokenResult != TRUE ||
		hToken == NULL ||
		hToken == INVALID_HANDLE_VALUE
	)
	{
		return HRESULT_FROM_WIN32( GetLastError() );
	};


	TOKEN_PRIVILEGES tkp; // pointer to token structure
	ZeroMemory( &tkp, sizeof( tkp ) );


	//
	// Lookup the shutdown privilege.
	//

	LookupPrivilegeValue
	(
		NULL,
		PrivilegeName,
		&tkp.Privileges[0].Luid
	);


	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;   


	//
	// Enable the privilege for the process.
	//

	BOOL bAdjustPrivilegesResult = AdjustTokenPrivileges
	(
		hToken,
		FALSE,
		&tkp,
		sizeof( tkp ),
		(PTOKEN_PRIVILEGES) NULL,
		0
	);

	if ( bAdjustPrivilegesResult != TRUE )
	{
		DWORD AdjustPrivilegesError = GetLastError();

		if ( AdjustPrivilegesError != ERROR_SUCCESS )
		{
			return HRESULT_FROM_WIN32( AdjustPrivilegesError );
		};
	};


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  ToggleDWMCompositionUI
//
// Purpose:
// Disables or restores DWM composition UI via a
// registry hack.
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
HRESULT CCustomTest< AppClass, ParentClass >::ToggleDWMCompositionUI
(
	IN BOOL bRestore
)
{
	HRESULT hOverallResult = E_FAIL;


	//
	// Open or create the parent registry key.
	//

	HKEY hParentKey = NULL;
	LONG CreateParentKeyResult = RegCreateKeyEx
	(
		HKEY_CURRENT_USER,
		_T( "Software\\Microsoft\\Windows\\DWM" ),
		0,
		NULL,
		0,
		KEY_READ | KEY_WRITE,
		NULL,
		&hParentKey,
		NULL
	);

	if ( CreateParentKeyResult != ERROR_SUCCESS )
	{
		hOverallResult = HRESULT_FROM_WIN32( CreateParentKeyResult );
		goto Cleanup;
	};


	//
	// Set the value thus suppressing the behavior, or restore
	// the system to its prior state.
	//

	// If we're restoring the state of the system...
	if ( bRestore == TRUE )
	{
		// ...and the value wasn't set originally...
		if ( m_bCompositionUIValueFound == FALSE )
		{
			// ...then delete it...
/*
			WriteToLog
			(
				_T( "ToggleDWMCompositionUI:  Removing SuppressDisableCompositionUI value from the registry." )
			);
*/

			LONG DeleteValueResult = RegDeleteValue
			(
				hParentKey,
				_T( "SuppressDisableCompositionUI" )
			);

			if ( DeleteValueResult != ERROR_SUCCESS )
			{
				hOverallResult = HRESULT_FROM_WIN32( DeleteValueResult );
				goto Cleanup;
			};
		}
		// ...otherwise, it was set, ...
		else
		{
			// ...so restore it to its original value...
/*
			WriteToLogF
			(
				_T( "ToggleDWMCompositionUI:  Restoring SuppressDisableCompositionUI value to %u in the registry." ),
				m_CompositionUIValue
			);
*/

			LONG SetValueResult = RegSetValueEx
			(
				hParentKey,
				_T( "SuppressDisableCompositionUI" ),
				0,
				REG_DWORD,
				(LPBYTE) &m_CompositionUIValue,
				sizeof( m_CompositionUIValue )
			);

			if ( SetValueResult != ERROR_SUCCESS )
			{
				hOverallResult = HRESULT_FROM_WIN32( SetValueResult );
				goto Cleanup;
			};
		};
	}
	// ...otherwise, if we're overriding the state of the system...
	else
	{
		// ...then query the value...
		DWORD ValueType = REG_DWORD;
		DWORD ValueSize = sizeof( DWORD );
		LONG QueryValueResult = RegQueryValueEx
		(
			hParentKey,
			_T( "SuppressDisableCompositionUI" ),
			NULL,
			&ValueType,
			(LPBYTE) &m_CompositionUIValue,
			&ValueSize
		);

		// ...and if we found it...
		if ( QueryValueResult == ERROR_SUCCESS )
		{
			// ...then sanity-check it...
			assert( ValueType == REG_DWORD );
			assert( ValueSize == sizeof( DWORD ) );

			// ...and stash it for restoration at cleanup time...
			m_bCompositionUIValueFound = TRUE;
		}
		else
		{
			// ...otherwise, make a note that we didn't find it.
			m_bCompositionUIValueFound = FALSE;
		};


		//
		// Override the value.
		//

/*
		WriteToLog
		(
			_T( "ToggleDWMCompositionUI:  Overriding SuppressDisableCompositionUI value in the registry." )
		);
*/

		DWORD OverrideData = 1;
		LONG SetValueResult = RegSetValueEx
		(
			hParentKey,
			_T( "SuppressDisableCompositionUI" ),
			0,
			REG_DWORD,
			(LPBYTE) &OverrideData,
			sizeof( OverrideData )
		);

		if ( SetValueResult != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( SetValueResult );
			goto Cleanup;
		};
	};

			
	//
	// If we made it this far, then we're good to go.
	//

	hOverallResult = S_OK;


Cleanup:
	//
	// Close the parent key.
	//

	if ( hParentKey != NULL )
	{
		RegCloseKey( hParentKey );
		hParentKey = NULL;
	};

	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  DetectDriverModelForDevice
//
// Purpose:
// Determines whether the specified device supports the
// Longhorn device driver model (LDDM).
//
// Returns:
// DRIVERMODEL_XPDM  for Windows XP devices
// DRIVERMODEL_LDDM  for Longhorn (LDDM) devices
///////////////////////////////////////////////////////
template < typename AppClass, typename ParentClass >
DRIVERMODEL CCustomTest< AppClass, ParentClass >::DetectDriverModelForDevice
(
	IN LPCWSTR pDeviceName
)
{
	DRIVERMODEL DriverModel = DRIVERMODEL_XPDM;

	HMODULE hGDI = NULL;

	HDC hDC = NULL;
	if ( pDeviceName != NULL )
	{
		hDC = CreateDCW
		(
			pDeviceName,
			NULL,
			NULL,
			NULL
		);
	}
	else
	{
		hDC = CreateDCW
		(
			L"DISPLAY",
			NULL,
			NULL,
			NULL
		);
	};

	if ( hDC == NULL )
	{
		goto Cleanup;
	};


	//
	// Load GDI.
	//

	hGDI = LoadLibraryExW( L"gdi32.dll", NULL, 0 );
	if ( hGDI == NULL )
	{
		goto Cleanup;
	};


	//
	// Get pointers to our GDI kernel-mode thunks.
	//

	PFND3DKMT_OPENADAPTERFROMHDC pfnOsThunkDDIOpenAdapterFromHdc = (PFND3DKMT_OPENADAPTERFROMHDC)GetProcAddress
	(
		hGDI,
		"D3DKMTOpenAdapterFromHdc"
	);
	if (pfnOsThunkDDIOpenAdapterFromHdc == NULL)
	{
		goto Cleanup;
	};


	PFND3DKMT_CLOSEADAPTER pfnOsThunkDDICloseAdapter = (PFND3DKMT_CLOSEADAPTER)GetProcAddress
	(
		hGDI,
		"D3DKMTCloseAdapter"
	);
	if (pfnOsThunkDDICloseAdapter == NULL)
	{
		goto Cleanup;
	};


	//
	// Open the adapter.
	//

	D3DKMT_OPENADAPTERFROMHDC OpenAdapterFromHdc;
	OpenAdapterFromHdc.hDc = hDC;
	if
	(
		(*pfnOsThunkDDIOpenAdapterFromHdc)(&OpenAdapterFromHdc) != STATUS_SUCCESS
	)
	{
		goto Cleanup;
	};


	//
	// Close the adapter.
	//

	D3DKMT_CLOSEADAPTER CloseAdapter;

	CloseAdapter.hAdapter = OpenAdapterFromHdc.hAdapter;
	(*pfnOsThunkDDICloseAdapter)(&CloseAdapter);


	DriverModel = DRIVERMODEL_LDDM;


Cleanup:
	if ( hDC != NULL )
	{
		DeleteDC( hDC );
		hDC = NULL;
	};

	if ( hGDI != NULL )
	{
		FreeLibrary( hGDI );
		hGDI = NULL;
	};


	return DriverModel;
};
