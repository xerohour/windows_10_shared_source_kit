//////////////////////////////////////////////////////////////////////
// File:  Occlusion.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of Direct3D 9.L occlusion test app.
//
// History:
// 15 Aug 2005  SEdmison  Created.
// 2 March 2010 OlanH	  Crippled select test cases on WDDM1.0.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include <initguid.h>
#include "Occlusion.hpp"
#include <CustomTest.inl>
#include <D3D9CustomTest.inl>
#include <CustomTestApp.inl>
#include <D3D9CustomTestApp.inl>
#include <HelperFunctions.h>

/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CMyTest
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CMyTest::CMyTest()
:
	CD3D9CustomTest< CMyTestApp >::CD3D9CustomTest( &g_Application ),
	m_D3D9VersionMS( 0 ),
	m_D3D9VersionLS( 0 )
{
	m_WindowHandler.AttachLogger( g_Application.GetLogWrapper() );

	ZeroMemory( m_phWindows, sizeof( m_phWindows ) );
	ZeroMemory( m_ppDevices, sizeof( m_ppDevices ) );
};


///////////////////////////////////////////////////////
// Function:  InitTestParameters
//
// Purpose:
// Initializes the parameter axes along which the
// framework should iterate.
///////////////////////////////////////////////////////
void CMyTest::InitTestParameters()
{
	// Set up a one-dimensional list of scenarios.
	AddParameter
	(
		_T( "Scenario" ),
		&m_iCurrentScenario,
		RangeValueSet
		(
			0,
			g_cTestScenarios - 1,
			1
		)
	);
};


///////////////////////////////////////////////////////
// Function:  SetupTestCase
//
// Purpose:
// Per-test-case initialization.
///////////////////////////////////////////////////////
TEST_RESULT CMyTest::SetupTestCase()
{
	//
	// Initialize locals.
	//

	TEST_RESULT SetupResult = RESULT_ABORT;


	//
	// Enumerate GDI devices.
	//

	HRESULT hEnumerateGDIDevicesResult = m_GDITopology.EnumerateAdaptersAndOutputs();

	if ( FAILED( hEnumerateGDIDevicesResult ) )
	{
		WriteToLog
		(
			_T( "SetupTestCase:  EnumerateAdaptersAndOutputs failed with error 0x%x (%s)." ),
			hEnumerateGDIDevicesResult,
			HResultToString( hEnumerateGDIDevicesResult )
		);

		goto Cleanup;
	};


	//
	// Skip inactive scenarios.
	//

	if ( g_TestScenarios[ m_iCurrentScenario ].bEnabled != TRUE )
	{
		WriteToLog
		(
			_T( "SetupTestCase:  Scenario %i (%s) is disabled." ),
			m_iCurrentScenario,
			g_TestScenarios[ m_iCurrentScenario ].Name
		);

		SetupResult = RESULT_SKIP;
		goto Cleanup;
	};


	//
	// Bail if we found no hardware to test.
	//

	if
	(
		m_GDITopology.m_cDevices == 0
	)
	{
		WriteToLog
		(
			_T( "SetupTestCase:  Found no GDI devices with which to test." )
		);

		SetupResult = RESULT_SKIP;
		goto Cleanup;
	};


	//
	// Skip scenarios that require monitors that the machine lacks.
	//

	for
	(
		UINT iCurrentDevice = 0;
		iCurrentDevice < g_TestScenarios[ m_iCurrentScenario ].cDevices;
		++iCurrentDevice
	)
	{
		if
		(
			g_TestScenarios[ m_iCurrentScenario ].Devices[ iCurrentDevice ].Monitor != MONITOR_CENTER &&
			m_GDITopology.m_DeviceSlotForMonitorSlot[ g_TestScenarios[ m_iCurrentScenario ].Devices[ iCurrentDevice ].Monitor ] == 0xFFFFFFFF
		)
		{
			WriteToLog
			(
				_T( "SetupTestCase:  Scenario %i (%s) requires a %s monitor." ),
				m_iCurrentScenario,
				g_TestScenarios[ m_iCurrentScenario ].Name,
				g_MonitorDescriptions[ g_TestScenarios[ m_iCurrentScenario ].Devices[ iCurrentDevice ].Monitor ]
			);

			SetupResult = RESULT_SKIP;
			goto Cleanup;
		};
	};


	//
	// Skip scenarios requiring powering down a monitor if we're on
	// an "always-on always connected" (AOAC) machine.
	//

	if ( g_Application.GetSysInfo()->PowerInf.AoAc == TRUE )
	{
		switch ( m_iCurrentScenario )
		{
			case 5:
			case 6:
			case 9:
			case 10:
			case 19:
			case 20:
			case 21:
			case 22:
			case 23:
			case 24:
			{
				WriteToLog
				(
					_T( "This scenario is being skipped because it requires suspending or turning off the monitor, which would put the machine into connected standby." )
				);

				SetupResult = RESULT_SKIP;
				goto Cleanup;
			} break;
		};
	};


	//
	// Handle pesky dialogs.
	//

	m_WindowHandler.HandleDialogs();


	//
	// Switch all monitors to the expected mode.
	//

	WriteToLog
	(
		_T( "SetupTestCase:  Changing mode to 1024 x 768 x 32 @ 60 Hz." )
	);

	HRESULT hCauseModeChangeResult = CauseModeChange
	(
		MONITOR_UNSPECIFIED,
		1024,
		768,
		32,
		60,
		DMDO_DEFAULT,
		CDS_UPDATEREGISTRY
	);

	if ( FAILED( hCauseModeChangeResult ) )
	{
		WriteToLog
		(
			_T( "SetupTestCase:  CauseModeChange failed with error 0x%x (%s)." ),
			hCauseModeChangeResult,
			HResultToString( hCauseModeChangeResult )
		);

		SetupResult = RESULT_SKIP;
		goto Cleanup;
	};


	//
	// Fill our our window class struct.
	//

	ZeroMemory( &m_WindowClass, sizeof( m_WindowClass ) );
	m_WindowClass.cbSize = sizeof( m_WindowClass );
	m_WindowClass.style = CS_DBLCLKS;
	m_WindowClass.lpfnWndProc = MsgProc;
	m_WindowClass.cbClsExtra = 0;
	m_WindowClass.cbWndExtra = sizeof( PVOID );
	m_WindowClass.hInstance = GetModuleHandle( NULL );
	m_WindowClass.lpszClassName = _T( "Occlusion" );


	//
	// Register our window class.
	//

	ATOM ClassAtom = RegisterClassEx( &m_WindowClass );

	if ( ClassAtom == 0 )
	{
		DWORD RegisterClassError = GetLastError();

		if ( RegisterClassError != ERROR_CLASS_ALREADY_EXISTS )
		{
			WriteToLog
			(
				_T( "SetupTestCase:  RegisterClassEx failed with error %u." ),
				RegisterClassError
			);

			goto Cleanup;
		};
	};


	//
	// Create our windows.
	//

	for
	(
		UINT iCurrentWindow = 0;
		iCurrentWindow < g_TestScenarios[ m_iCurrentScenario ].cDevices;
		++iCurrentWindow
	)
	{
		if ( g_TestScenarios[ m_iCurrentScenario ].Devices[ iCurrentWindow ].bWindowed == TRUE )
		{
			m_phWindows[ iCurrentWindow ] = CreateWindowExA
			(
				0,
				"Occlusion",
				"Overlapped Window",
				WS_OVERLAPPEDWINDOW,
				g_TestScenarios[ m_iCurrentScenario ].Devices[ iCurrentWindow ].Left,
				g_TestScenarios[ m_iCurrentScenario ].Devices[ iCurrentWindow ].Top,
				g_TestScenarios[ m_iCurrentScenario ].Devices[ iCurrentWindow ].Width,
				g_TestScenarios[ m_iCurrentScenario ].Devices[ iCurrentWindow ].Height,
				NULL,
				NULL,
				m_WindowClass.hInstance,
				(PVOID) this
			);
		}
		else
		{
			m_phWindows[ iCurrentWindow ] = CreateWindowExA
			(
				WS_EX_TOPMOST,
				"Occlusion",
				"Full-screen Window",
				WS_POPUP,
				g_TestScenarios[ m_iCurrentScenario ].Devices[ iCurrentWindow ].Left,
				g_TestScenarios[ m_iCurrentScenario ].Devices[ iCurrentWindow ].Top,
				g_TestScenarios[ m_iCurrentScenario ].Devices[ iCurrentWindow ].Width,
				g_TestScenarios[ m_iCurrentScenario ].Devices[ iCurrentWindow ].Height,
				NULL,
				NULL,
				m_WindowClass.hInstance,
				(PVOID) this
			);
		};

		if ( m_phWindows[ iCurrentWindow ] == NULL )
		{
			WriteToLog
			(
				_T( "SetupTestCase:  Could not create window %u." ),
				iCurrentWindow
			);

			goto Cleanup;
		};
	};


	//
	// Load D3D 9.
	//

	HRESULT hLoadD3D9Result = LoadD3D9();

	if ( FAILED( hLoadD3D9Result ) )
	{
		WriteToLog
		(
			_T( "SetupTestCase:  LoadD3D9 failed with error 0x%x (%s)." ),
			hLoadD3D9Result,
			HResultToString( hLoadD3D9Result )
		);

		goto Cleanup;
	};


	//
	// Build the fully-qualified filename to D3D9.dll.
	//

	WCHAR FullD3D9FileName[ MAX_PATH ] = L"";
	DWORD nFullD3D9FileName = GetModuleFileNameW
	(
		m_hD3D9,
		FullD3D9FileName,
		ARRAYSIZE( FullD3D9FileName )
	);

	if ( nFullD3D9FileName < 5 )
	{
		WriteToLog
		(
			_T( "SetupTestCase:  GetModuleFileName on D3D9.dll failed." )
		);

		goto Cleanup;
	};


	//
	// Get the D3D9.dll version.
	//

	// Log the fully-qualified DLL name.
	WriteToLog
	(
		L"Getting version for %s.",
		FullD3D9FileName
	);


	// We just need the numeric part of the version info.
	HRESULT hGetD3D9VersionResult = GetFileInfoEx
	(
		FullD3D9FileName,
		0,
		NULL,
		&m_D3D9VersionMS,
		&m_D3D9VersionLS,
		NULL,
		NULL
	);

	if ( FAILED( hGetD3D9VersionResult ) )
	{
		WriteToLog
		(
			_T( "GetFileInfoEx on D3D9.dll failed with error 0x%x (%s)." ),
			hGetD3D9VersionResult,
			HResultToString( hGetD3D9VersionResult )
		);

		goto Cleanup;
	};


	//
	// Log the D3D9.dll version.
	//

	WriteToLog
	(
		_T( "D3D9.dll version is %u.%u.%u.%u." ),
		HIWORD( m_D3D9VersionMS ),
		LOWORD( m_D3D9VersionMS ),
		HIWORD( m_D3D9VersionLS ),
		LOWORD( m_D3D9VersionLS )
	);


	//
	// Initialize D3D.
	//
	// This is a workaround for Windows OS Bugs bug 1788039.
	//

	HRESULT hReInitializeD3D9Result = InitializeD3D9
	(
		g_Application.m_bDebugHelper
	);

	if ( FAILED( hReInitializeD3D9Result ) )
	{
		WriteToLog
		(
			_T( "SetupTestCase:  InitializeD3D9 failed with error 0x%x (%s)." ),
			hReInitializeD3D9Result,
			HResultToString( hReInitializeD3D9Result )
		);

		goto Cleanup;
	};


	//
	// Enumerate Direct3D 9 devices.
	//

	HRESULT hEnumerateAdaptersResult = m_D3D9Topology.EnumerateAdaptersAndOutputs
	(
		m_bDoD3D9L == TRUE ? m_pD3D9Ex : m_pD3D9,
		g_Application.m_DeviceType
	);

	if ( FAILED( hEnumerateAdaptersResult ) )
	{
		WriteToLog
		(
			_T( "SetupTestCase:  EnumerateAdaptersAndOutputs failed with error 0x%x (%s)." ),
			hEnumerateAdaptersResult,
			HResultToString( hEnumerateAdaptersResult )
		);

		goto Cleanup;
	};


	//
	// Map Direct3D 9 devices to GDI devices.
	//

	HRESULT hMapAdapterTopologiesResult = m_AdapterTopologyMap.MapAdapterTopologies
	(
		m_GDITopology,
		m_D3D9Topology
	);

	if ( FAILED( hMapAdapterTopologiesResult ) )
	{
		WriteToLog
		(
			_T( "SetupTestCase:  MapAdapterTopologies failed with error 0x%x (%s)." ),
			hMapAdapterTopologiesResult,
			HResultToString( hMapAdapterTopologiesResult )
		);

		goto Cleanup;
	};


	SetupResult = RESULT_PASS;


Cleanup:
	return SetupResult;
};


///////////////////////////////////////////////////////
// Function:  ExecuteTestCase
//
// Purpose:
// Executes a single test case.
///////////////////////////////////////////////////////
TEST_RESULT CMyTest::ExecuteTestCase()
{
	//
	// Validate the scenario number.
	//

	assert( m_iCurrentScenario >= 0 );
	if ( m_iCurrentScenario < 0 )
	{
		WriteToLog
		(
			_T( "Scenario not implemented." )
		);

		return RESULT_SKIP;
	};


	assert( m_iCurrentScenario < g_cTestScenarios );
	if ( m_iCurrentScenario >= g_cTestScenarios )
	{
		WriteToLog
		(
			_T( "Scenario not implemented." )
		);

		return RESULT_SKIP;
	};


	assert( m_iCurrentScenario < ARRAYSIZE( g_TestScenarios ) );
	if
	(
		m_iCurrentScenario >= ARRAYSIZE( g_TestScenarios )
	)
	{
		WriteToLog
		(
			_T( "Scenario not implemented." )
		);

		return RESULT_SKIP;
	};


	//
	// Log which scenario we're running.
	//

	WriteToLog
	(
		_T( "Scenario:  %s." ),
		g_TestScenarios[ m_iCurrentScenario ].Name
	);


	//
	// Skip scenarios that are marked as inactive.
	//

	if ( g_TestScenarios[ m_iCurrentScenario ].bEnabled == FALSE )
	{
		return RESULT_SKIP;
	};


	//
	// Run the scenario.
	//

	switch ( m_iCurrentScenario )
	{
		//////////////////////////////////////////////////
		//
		// Occlusion scenarios.
		//
		//////////////////////////////////////////////////

		case 0:
		{
			return TestOcclusionOfDevice0ByDevice1( FALSE, TRUE );  // work around Windows 7 bug #618143.
		} break;

		case 1:
		case 3:
		case 7:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		{
			return TestOcclusionOfDevice0ByDevice1( FALSE, FALSE );
		} break;


		case 2:
		{
			return TestOcclusionOfDevice0ByDevice1( TRUE , FALSE);
		} break;


		case 5:
		case 9:
		{
			return TestOcclusionOfDevice0ByMonitorSuspend( MONITOR_STANDBY );
		} break;


		case 6:
		case 10:
		{
			return TestOcclusionOfDevice0ByMonitorSuspend( MONITOR_OFF );
		} break;


		case 4:
		case 8:
		{
			return TestOcclusionOfDevice0ByMinimization( TRUE );
		} break;


		//////////////////////////////////////////////////
		//
		// Mode change scenarios
		//
		//////////////////////////////////////////////////

		case 16:
		case 17:
		{
			return TestModeChangeDevice0( FALSE );
		} break;


		case 18:
		{
			return TestModeChangeDevice0( FALSE );
		} break;


		//////////////////////////////////////////////////
		//
		// Direct3D device creation scenarios
		//
		//////////////////////////////////////////////////

		case 19:
		case 21:
		{
			return TestCreationOfDevice0WhileMonitorSuspended( MONITOR_STANDBY );
		} break;


		case 20:
		case 22:
		{
			return TestCreationOfDevice0WhileMonitorSuspended( MONITOR_OFF );
		} break;


		//////////////////////////////////////////////////
		//
		// DirectDraw creation scenarios
		//
		//////////////////////////////////////////////////

		case 23:
		{
			return TestCreationOfDDrawWhileMonitorSuspended( MONITOR_STANDBY );
		} break;

		case 24:
		{
			return TestCreationOfDDrawWhileMonitorSuspended( MONITOR_OFF );
		} break;


		//////////////////////////////////////////////////
		//
		// Unimplemented scenarios
		//
		//////////////////////////////////////////////////

		default:
		{
			WriteToLog( _T( "Scenario not implemented." ) );

			return RESULT_SKIP;
		} break;
	};


	return RESULT_FAIL;
}


///////////////////////////////////////////////////////
// Function:  CleanupTestCase
//
// Purpose:
// Per-test-case cleanup.
///////////////////////////////////////////////////////
void CMyTest::CleanupTestCase()
{
	//
	// Free our devices and windows.
	//

	for
	(
		UINT iCurrentDevice = 0;
		iCurrentDevice < g_TestScenarios[ m_iCurrentScenario ].cDevices;
		++iCurrentDevice
	)
	{
		if ( m_ppDevices[ iCurrentDevice ] != NULL )
		{
			m_ppDevices[ iCurrentDevice ]->Release();
			m_ppDevices[ iCurrentDevice ] = NULL;
		};

		if ( m_phWindows[ iCurrentDevice ] != NULL )
		{
			DestroyWindow( m_phWindows[ iCurrentDevice ] );
			m_phWindows[ iCurrentDevice ] = NULL;
		};
	};


	//
	// Clean up D3D.
	//

	CleanupD3D9();


	//
	// Unload D3D 9.
	//

	UnloadD3D9();
};


///////////////////////////////////////////////////////
// Function:  TestOcclusionOfDevice0ByDevice1
//
// Purpose:
// Tests occlusion of a device by another device.
///////////////////////////////////////////////////////
TEST_RESULT CMyTest::TestOcclusionOfDevice0ByDevice1
(
	IN BOOL bDoModeChange,
	IN BOOL bTolerateOcclusionFailureOnWDDM_1_0   // Allow Present to return S_OK intead of occluded on WDDM1.0.  (Work around bug 618143)
)
{
	//
	// Initialize locals.
	//

	TEST_RESULT TestResult = RESULT_FAIL;


	//
	// Check for adapter topology changes.
	//

	HRESULT hCheckForAdapterTopologyChangesResult = CheckForAdapterTopologyChanges();

	if ( FAILED( hCheckForAdapterTopologyChangesResult ) )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges failed with error 0x%x (%s)." ),
			hCheckForAdapterTopologyChangesResult,
			HResultToString( hCheckForAdapterTopologyChangesResult )
		);

		TestResult = RESULT_ABORT;
		goto Cleanup;
	}
	else if ( hCheckForAdapterTopologyChangesResult == S_FALSE )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges reports that adapter topology has changed." )
		);

		TestResult = RESULT_ABORT;
		goto Cleanup;
	};


	//
	// Show the background window.
	//

	assert( m_phWindows[ 0 ] != NULL );
	DisplayWindow( 0 );


	//
	// Create device 0.
	//

	HRESULT hCreateDevice0Result = CreateDevice( 0 );

	if ( FAILED( hCreateDevice0Result ) )
	{
		WriteToLog
		(
			_T( "CreateDevice on device 0 failed with error 0x%x (%s)." ),
			hCreateDevice0Result,
			HResultToString( hCreateDevice0Result )
		);

		goto Cleanup;
	};


	//
	// Render an empty scene with device 0.
	//

	WriteToLog
	(
		_T( "Rendering with device 0." )
	);

	assert( m_ppDevices[ 0 ] != NULL );
	HRESULT hRenderOriginalSceneResult = RenderScene( 0 );

	if ( FAILED( hRenderOriginalSceneResult ) )
	{
		WriteToLog
		(
			_T( "RenderScene on device 0 failed with error 0x%x (%s)." ),
			hRenderOriginalSceneResult,
			HResultToString( hRenderOriginalSceneResult )
		);

		goto Cleanup;
	};


	//
	// Present the scene on device 0.
	//

	WriteToLog
	(
		_T( "Presenting with device 0." )
	);

	HRESULT hPresentOriginalSceneResult = m_ppDevices[ 0 ]->Present
	(
		NULL,
		NULL,
		NULL,
		NULL
	);

	if ( FAILED( hPresentOriginalSceneResult ) )
	{
		WriteToLog
		(
			_T( "Present on device 0 failed with error 0x%x (%s)." ),
			hPresentOriginalSceneResult,
			HResultToString( hPresentOriginalSceneResult )	
		);

		goto Cleanup;
	}
	else if ( hPresentOriginalSceneResult == S_PRESENT_OCCLUDED )
	{
		WriteToLog
		(
			_T( "Present on device 0 returned unexpected result S_PRESENT_OCCLUDED." )
		);

		goto Cleanup;
	};


	//
	// Pump messages resulting from work done on device 0.
	//

	WaitAndPumpMessages( 100 );


	//
	// Show our foreground window.
	//

	assert( m_phWindows[ 1 ] != NULL );

	if ( g_TestScenarios[ m_iCurrentScenario ].Devices[ 1 ].bVisible == TRUE )
	{
		DisplayWindow( 1 );

		HWND hForegroundWindow = GetForegroundWindow();

		if ( hForegroundWindow != m_phWindows[ 1 ] )
		{
			m_WindowHandler.LogDetailsAboutWindow( GetForegroundWindow() );
		};


		//
		// Pump messages resulting from work done on device 0.
		//

		WaitAndPumpMessages( 100 );
	};


	//
	// Create device 1.
	//

	HRESULT hCreateDevice1Result = CreateDevice( 1 );
	if ( FAILED( hCreateDevice1Result ) )
	{
		WriteToLog
		(
			_T( "CreateDevice on device 1 failed with error 0x%x (%s)." ),
			hCreateDevice1Result,
			HResultToString( hCreateDevice1Result )
		);

		goto Cleanup;
	};


	//
	// Render an empty scene with device 1.
	//

	WriteToLog
	(
		_T( "Rendering with device 1." )
	);

	assert( m_ppDevices[ 1 ] != NULL );
	HRESULT hRenderForegroundSceneResult = RenderScene( 1 );

	if ( FAILED( hRenderForegroundSceneResult ) )
	{
		WriteToLog
		(
			_T( "RenderScene on device 1 failed with error 0x%x (%s)." ),
			hRenderForegroundSceneResult,
			HResultToString( hRenderForegroundSceneResult )
		);

		goto Cleanup;
	};


	//
	// Present the scene on our foreground device.
	//

	WriteToLog
	(
		_T( "Presenting with device 1." )
	);

	HRESULT hPresentForegroundSceneResult = m_ppDevices[ 1 ]->Present
	(
		NULL,
		NULL,
		NULL,
		NULL
	);

	if ( FAILED( hPresentForegroundSceneResult ) )
	{
		WriteToLog
		(
			_T( "Present on device 1 failed with error 0x%x (%s)." ),
			hPresentForegroundSceneResult,
			HResultToString( hPresentForegroundSceneResult )	
		);

		goto Cleanup;
	}
	else if ( hPresentForegroundSceneResult != S_OK )
	{
		WriteToLog
		(
			_T( "Present on foreground device returned unexpected result 0x%x (%s)." ),
			hPresentForegroundSceneResult,
			HResultToString( hPresentForegroundSceneResult )
		);

		goto Cleanup;
	};


	//
	// Pump messages resulting from work done on device 1.
	//

	WaitAndPumpMessages( 100 );


	if ( bDoModeChange == TRUE )
	{
		//
		// Cause a mode change.
		//

		WriteToLog
		(
			_T( "Causing mode change." )
		);

		HRESULT hModeChangeResult = CauseModeChange
		(
			MONITOR_CENTER
		);

		if ( FAILED( hModeChangeResult ) )
		{
			WriteToLog( _T( "CauseModeChange failed." ) );

			goto Cleanup;
		};


		//
		// Wait for the messages resulting from the mode change to ripple through.
		//

		WaitAndPumpMessages( 2000 );
	};


	HRESULT hRenderOccludedSceneResult = E_FAIL;
	HRESULT hPresentOccludedSceneResult = E_FAIL;
	for
	(
		UINT iCurrentAttempt = 0;
		iCurrentAttempt < 3;
		++iCurrentAttempt
	)
	{
		//
		// Render again with the background device.
		//

		WriteToLog
		(
			_T( "Rendering with device 0 again." )
		);

		hRenderOccludedSceneResult = RenderScene( 0 );

		if ( FAILED( hRenderOccludedSceneResult ) )
		{
			WriteToLog
			(
				_T( "RenderScene on device 0 failed with error 0x%x (%s)." ),
				hRenderOccludedSceneResult,
				HResultToString( hRenderOccludedSceneResult )
			);

			goto Cleanup;
		};


		//
		// Present again with the background device.
		//

		WriteToLog
		(
			_T( "Presenting with device 0 again." )
		);

		hPresentOccludedSceneResult = m_ppDevices[ 0 ]->Present
		(
			NULL,
			NULL,
			NULL,
			NULL
		);

		if ( FAILED( hPresentOccludedSceneResult ) )
		{
			WriteToLog
			(
				_T( "Present on device 0 failed with error 0x%x (%s)." ),
				hPresentOccludedSceneResult,
				HResultToString( hPresentOccludedSceneResult )	
			);

			goto Cleanup;
		};


		//
		// Pump messages.
		//

		WaitAndPumpMessages( 0 );
	};


#if CHECK_FOR_WINDOWS_7_BUG_408340
	//
	// Call CheckDeviceState, and note any cases where CheckDeviceState
	// returns a different result than Present.
	//

	HRESULT hDeviceState = m_ppDevices[ 0 ]->CheckDeviceState( m_phWindows[ 0 ] );

	if ( hDeviceState != hPresentOccludedSceneResult )
	{
		WriteToLog
		(
			_T( "CheckDeviceState on device 0 returned 0x%x (%s), whereas Present returned 0x%x (%s)." ),
			hDeviceState,
			HResultToString( hDeviceState ),
			hPresentOccludedSceneResult,
			HResultToString( hPresentOccludedSceneResult )
		);

		goto Cleanup;
	};
#endif


	//
	// Verify that Present returned what we expected.
	//

	for
	(
		UINT iCurrentResult = 0;
		iCurrentResult < g_TestScenarios[ m_iCurrentScenario ].cExpectedAPIResults;
		++iCurrentResult
	)
	{
		if
		(
			(
				m_D3D9VersionMS > g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MinimumDLLVersionMS ||
				m_D3D9VersionMS == g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MinimumDLLVersionMS &&
					m_D3D9VersionLS >= g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MinimumDLLVersionLS
			) &&
			(
				m_D3D9VersionMS < g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MaximumDLLVersionMS ||
				m_D3D9VersionMS == g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MaximumDLLVersionMS &&
					m_D3D9VersionLS <= g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MaximumDLLVersionLS
			) &&
			hPresentOccludedSceneResult == g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].hExpectedResult
		)
		{
			WriteToLog
			(
				_T( "Present returned %s, as expected." ),
				HResultToString( hPresentOccludedSceneResult )
			);

			TestResult = RESULT_PASS;
			goto Cleanup;
		};
	};


	//
	// Always allow S_OK from Present if
	// - We're running on Windows 7.
	// - There are multiple monitors.
	// - Composition is enabled.
	// This is because the occlusion detection logic was
	// updated to take into account that even though
	// something might appear to be occluded on a given
	// monitor, if there is another monitor, thumbnails
	// may be visible on that monitor.
	//
	// Also when input BOOL bTolerateOcclusionFailureOnWDDM_1_0 is 
	// set to TRUE, we need to tolerate S_OK on WDDM1.0 
	// parts on Windows7
	// See Windows 7 Bug #618143 for details.


	if ( hPresentOccludedSceneResult == S_OK )
	{
		OSVERSIONINFOEXW VersionInfo;
		ZeroMemory( &VersionInfo, sizeof( VersionInfo ) );
		VersionInfo.dwOSVersionInfoSize = sizeof( VersionInfo );

		BOOL bGetVersionInfoResult = GetVersionExW
		(
			(OSVERSIONINFOW *) &VersionInfo
		);

		if ( bGetVersionInfoResult != TRUE )
		{
			DWORD GetVersionInfoError = GetLastError();

			WriteToLog
			(
				_T( "GetVersionInfoEx failed with error %u." ),
				GetVersionInfoError
			);

			goto Cleanup;
		};

		if
		(
			VersionInfo.dwMajorVersion == 6 &&
			VersionInfo.dwMinorVersion >= 1 ||

			VersionInfo.dwMajorVersion > 6
		)
		{
			if
			(
				IsDWMCompositionEnabled() == S_OK &&
				m_OriginalGDITopology.m_cDevices > 1
			)
			{
				WriteToLog
				(
					_T( "Present returned S_OK, which is expected on a multiple-monitor Windows 7 machine with composition enabled." )
				);

				TestResult = RESULT_PASS;
				goto Cleanup;
			}
			else if ((bTolerateOcclusionFailureOnWDDM_1_0 == TRUE) && (GetFramework()->GetSysInfo()->GetGraphicsDriverModel() == GRAPHICS_DRIVER_MODEL_WDDM_1_0))
			{
				//
			    // For some test cases we need to tolerate S_OK to work around Windows 7 bug #618143.
				//
				WriteToLog
				(
					_T( "Present returned S_OK, which is acceptable on WDDM1.0 Windows 7 machines." )
				);

				TestResult = RESULT_PASS;
				goto Cleanup;
			}
		};
	};


	WriteToLog
	(
		_T( "Present returned unexpected result 0x%x (%s)." ),
		hPresentOccludedSceneResult,
		HResultToString( hPresentOccludedSceneResult )
	);


Cleanup:
	//
	// Log troubleshooting information.
	//

	if
	(
		TestResult != RESULT_PASS &&
		TestResult != RESULT_SKIP
	)
	{
		//
		// Start the failure diagnostics section.
		//

		LogDiagnosticsStart();


		//
		// Log details about the foreground window.
		//

		m_WindowHandler.LogDetailsAboutWindow( GetForegroundWindow() );


		//
		// Log additional failure probes.
		//

		LogDiagnostics();


		//
		// Log related bugs.
		//

		LogRelatedBugs();


		//
		// End the failure diagnostics section.
		//

		LogDiagnosticsEnd();
	};


	if ( bDoModeChange == TRUE )
	{
		WriteToLog
		(
			_T( "Restoring display mode." )
		);


		//
		// Restore the display mode.
		//

		CauseModeChange
		(
			MONITOR_CENTER,
			1024,
			768,
			32,
			60,
			DMDO_DEFAULT,
			CDS_UPDATEREGISTRY
		);


		//
		// Pump messages resulting from the display mode change.
		//

		WaitAndPumpMessages( 1000 );
	};


	//
	// Check for adapter topology changes.
	//

	hCheckForAdapterTopologyChangesResult = CheckForAdapterTopologyChanges();

	if ( FAILED( hCheckForAdapterTopologyChangesResult ) )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges failed with error 0x%x (%s)." ),
			hCheckForAdapterTopologyChangesResult,
			HResultToString( hCheckForAdapterTopologyChangesResult )
		);

		TestResult = RESULT_ABORT;
	}
	else if ( hCheckForAdapterTopologyChangesResult == S_FALSE )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges reports that adapter topology has changed." )
		);

		TestResult = RESULT_ABORT;
	};


	return TestResult;
};


///////////////////////////////////////////////////////
// Function:  TestOcclusionOfDevice0ByMonitorSuspend
//
// Purpose:
// Tests the scenario of a device's being occluded
// the monitor shuts down or goes into standby.
//
// Called by:
// ExecuteTestCase
///////////////////////////////////////////////////////
TEST_RESULT CMyTest::TestOcclusionOfDevice0ByMonitorSuspend
(
	IN LPARAM MonitorPowerState
)
{
	//
	// Initialize locals.
	//

	TEST_RESULT TestResult = RESULT_FAIL;


	//
	// Check for adapter topology changes.
	//

	HRESULT hCheckForAdapterTopologyChangesResult = CheckForAdapterTopologyChanges();

	if ( FAILED( hCheckForAdapterTopologyChangesResult ) )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges failed with error 0x%x (%s)." ),
			hCheckForAdapterTopologyChangesResult,
			HResultToString( hCheckForAdapterTopologyChangesResult )
		);

		TestResult = RESULT_ABORT;
		goto Cleanup;
	}
	else if ( hCheckForAdapterTopologyChangesResult == S_FALSE )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges reports that adapter topology has changed." )
		);

		TestResult = RESULT_ABORT;
		goto Cleanup;
	};


	//
	// Show the window to be occluded.
	//

	assert( m_phWindows[ 0 ] != NULL );
	DisplayWindow( 0 );


	//
	// Create device 0.
	//

	HRESULT hCreateDevice0Result = CreateDevice( 0 );

	if ( FAILED( hCreateDevice0Result ) )
	{
		WriteToLog
		(
			_T( "CreateDevice on device 0 failed with error 0x%x (%s)." ),
			hCreateDevice0Result,
			HResultToString( hCreateDevice0Result )
		);

		goto Cleanup;
	};


	//
	// Render an empty scene with this device.
	//

	assert( m_ppDevices[ 0 ] != NULL );
	HRESULT hRenderOriginalSceneResult = RenderScene( 0 );

	if ( FAILED( hRenderOriginalSceneResult ) )
	{
		WriteToLog( _T( "Failed to render scene on device 0." ) );

		goto Cleanup;
	};


	//
	// Present the scene on the device.
	//

	HRESULT hPresentOriginalSceneResult = m_ppDevices[ 0 ]->Present
	(
		NULL,
		NULL,
		NULL,
		NULL
	);

	if ( FAILED( hPresentOriginalSceneResult ) )
	{
		WriteToLog
		(
			_T( "Present of original scene failed with error 0x%x (%s)." ),
			hPresentOriginalSceneResult,
			HResultToString( hPresentOriginalSceneResult )
		);

		goto Cleanup;
	};


	//
	// Pump messages resulting from work done on device 0.
	//

	WaitAndPumpMessages( 500 );


	//
	// Suspend the monitor.
	//

	HRESULT hSetMonitorPowerStateResult = SetMonitorPowerState( MonitorPowerState );

	if ( FAILED( hSetMonitorPowerStateResult ) )
	{
		WriteToLog
		(
			_T( "SetMonitorPowerState failed with error 0x%x (%s)." ),
			hSetMonitorPowerStateResult,
			HResultToString( hSetMonitorPowerStateResult )
		);

		goto Cleanup;
	};


	//
	// Wait for the monitor to go off.
	//

	WaitAndPumpMessages( 2000 );


	HRESULT hRenderOccludedSceneResult = E_FAIL;
	HRESULT hPresentOccludedSceneResult = E_FAIL;
	for
	(
		UINT iCurrentAttempt = 0;
		iCurrentAttempt < 3;
		++iCurrentAttempt
	)
	{
		//
		// Render again with the device.
		//

		hRenderOccludedSceneResult = RenderScene( 0 );

		if ( FAILED( hRenderOccludedSceneResult ) )
		{
			WriteToLog
			(
				_T( "RenderScene on device 0 failed with error 0x%x (%s)." ),
				hRenderOccludedSceneResult,
				HResultToString( hRenderOccludedSceneResult )
			);

			goto Cleanup;
		};


		//
		// Present again with the device.
		//

		hPresentOccludedSceneResult = m_ppDevices[ 0 ]->Present
		(
			NULL,
			NULL,
			NULL,
			NULL
		);

		if ( FAILED( hPresentOccludedSceneResult ) )
		{
			WriteToLog
			(
				_T( "Present on device 0 failed with error 0x%x (%s)." ),
				hPresentOccludedSceneResult,
				HResultToString( hPresentOccludedSceneResult )	
			);

			goto Cleanup;
		};


		//
		// Pump messages.
		//

		WaitAndPumpMessages( 0 );
	};


#if CHECK_FOR_WINDOWS_7_BUG_408340
	//
	// Call CheckDeviceState, and note any cases where CheckDeviceState
	// returns a different result than Present.
	//

	HRESULT hDeviceState = m_ppDevices[ 0 ]->CheckDeviceState( m_phWindows[ 0 ] );

	if ( hDeviceState != hPresentOccludedSceneResult )
	{
		WriteToLog
		(
			_T( "CheckDeviceState on device 0 returned 0x%x (%s), whereas Present returned 0x%x (%s)." ),
			hDeviceState,
			HResultToString( hDeviceState ),
			hPresentOccludedSceneResult,
			HResultToString( hPresentOccludedSceneResult )
		);

		goto Cleanup;
	};
#endif


	//
	// Verify that Present returned what we expected.
	//

	for
	(
		UINT iCurrentResult = 0;
		iCurrentResult < g_TestScenarios[ m_iCurrentScenario ].cExpectedAPIResults;
		++iCurrentResult
	)
	{
		if
		(
			(
				m_D3D9VersionMS > g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MinimumDLLVersionMS ||
				m_D3D9VersionMS == g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MinimumDLLVersionMS &&
					m_D3D9VersionLS >= g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MinimumDLLVersionLS
			) &&
			(
				m_D3D9VersionMS < g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MaximumDLLVersionMS ||
				m_D3D9VersionMS == g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MaximumDLLVersionMS &&
					m_D3D9VersionLS <= g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MaximumDLLVersionLS
			) &&
			hPresentOccludedSceneResult == g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].hExpectedResult
		)
		{
			WriteToLog
			(
				_T( "Present returned %s, as expected." ),
				HResultToString( hPresentOccludedSceneResult )
			);

			TestResult = RESULT_PASS;
			goto Cleanup;
		};
	};


	//
	// Always allow S_OK from Present if
	// - We're running on Windows 7.
	// - There are multiple monitors.
	// - Composition is enabled.
	// This is because the occlusion detection logic was
	// updated to take into account that even though
	// something might appear to be occluded on a given
	// monitor, if there is another monitor, thumbnails
	// may be visible on that monitor.
	//
	// Also tolerate S_OK on WDDM1.0 parts on Windows7
	// see Windows 7 Bug #618146 for details.

	if ( hPresentOccludedSceneResult == S_OK )
	{
		OSVERSIONINFOEXW VersionInfo;
		ZeroMemory( &VersionInfo, sizeof( VersionInfo ) );
		VersionInfo.dwOSVersionInfoSize = sizeof( VersionInfo );

		BOOL bGetVersionInfoResult = GetVersionExW
		(
			(OSVERSIONINFOW *) &VersionInfo
		);

		if ( bGetVersionInfoResult != TRUE )
		{
			DWORD GetVersionInfoError = GetLastError();

			WriteToLog
			(
				_T( "GetVersionInfoEx failed with error %u." ),
				GetVersionInfoError
			);

			goto Cleanup;
		};

		if
		(
			VersionInfo.dwMajorVersion == 6 &&
			VersionInfo.dwMinorVersion >= 1 ||

			VersionInfo.dwMajorVersion > 6
		)
		{
			if
			(
				IsDWMCompositionEnabled() == S_OK &&
				m_OriginalGDITopology.m_cDevices > 1
			)
			{
				WriteToLog
				(
					_T( "Present returned S_OK, which is expected on a multiple-monitor Windows 7 machine with composition enabled." )
				);

				TestResult = RESULT_PASS;
				goto Cleanup;
			}
			else if (GetFramework()->GetSysInfo()->GetGraphicsDriverModel() == GRAPHICS_DRIVER_MODEL_WDDM_1_0)
			{
				 // We're testing WDDM1.0.  Need to work around Windows 7 bug #618146.
				WriteToLog
				(
					_T( "Present returned S_OK, which is expected WDDM1.0 Windows 7 machines." )
				);

				TestResult = RESULT_PASS;
				goto Cleanup;
			}
		};
	};


	WriteToLog
	(
		_T( "Present returned unexpected result 0x%x (%s)." ),
		hPresentOccludedSceneResult,
		HResultToString( hPresentOccludedSceneResult )
	);


Cleanup:
	//
	// Log troubleshooting information.
	//

	if
	(
		TestResult != RESULT_PASS &&
		TestResult != RESULT_SKIP
	)
	{
		//
		// Start the failure diagnostics section.
		//

		LogDiagnosticsStart();


		//
		// Log details about the foreground window.
		//

		m_WindowHandler.LogDetailsAboutWindow( GetForegroundWindow() );


		//
		// Log additional failure probes.
		//

		LogDiagnostics();


		//
		// Log related bugs.
		//

		LogRelatedBugs();


		//
		// End the failure diagnostics section.
		//

		LogDiagnosticsEnd();
	};


	//
	// Restore the monitor power state.
	//

	SetMonitorPowerState( MONITOR_ON );


	//
	// Wait for the monitor to come back on.
	//

	WaitAndPumpMessages( 100 );


	//
	// Check for adapter topology changes.
	//

	hCheckForAdapterTopologyChangesResult = CheckForAdapterTopologyChanges();

	if ( FAILED( hCheckForAdapterTopologyChangesResult ) )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges failed with error 0x%x (%s)." ),
			hCheckForAdapterTopologyChangesResult,
			HResultToString( hCheckForAdapterTopologyChangesResult )
		);

		TestResult = RESULT_ABORT;
	}
	else if ( hCheckForAdapterTopologyChangesResult == S_FALSE )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges reports that adapter topology has changed." )
		);

		TestResult = RESULT_ABORT;
	};


	return TestResult;
};


///////////////////////////////////////////////////////
// Function:  TestOcclusionOfDevice0ByMinimization
//
// Purpose:
// Tests the scenario of a device's being occluded
// by being minimized.
//
// Called by:
// ExecuteTestCase
///////////////////////////////////////////////////////
TEST_RESULT CMyTest::TestOcclusionOfDevice0ByMinimization
(
	IN BOOL bRestoreWindow
)
{
	//
	// Initialize locals.
	//

	TEST_RESULT TestResult = RESULT_FAIL;


	//
	// Check for adapter topology changes.
	//

	HRESULT hCheckForAdapterTopologyChangesResult = CheckForAdapterTopologyChanges();

	if ( FAILED( hCheckForAdapterTopologyChangesResult ) )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges failed with error 0x%x (%s)." ),
			hCheckForAdapterTopologyChangesResult,
			HResultToString( hCheckForAdapterTopologyChangesResult )
		);

		TestResult = RESULT_ABORT;
		goto Cleanup;
	}
	else if ( hCheckForAdapterTopologyChangesResult == S_FALSE )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges reports that adapter topology has changed." )
		);

		TestResult = RESULT_ABORT;
		goto Cleanup;
	};


	//
	// Show the window to be occluded.
	//

	assert( m_phWindows[ 0 ] != NULL );
	DisplayWindow( 0 );


	//
	// Create device 0.
	//

	WriteToLog( _T( "Creating device 0." ) );

	HRESULT hCreateDevice0Result = CreateDevice( 0 );

	if ( FAILED( hCreateDevice0Result ) )
	{
		WriteToLog
		(
			_T( "CreateDevice on device 0 failed with error 0x%x (%s)." ),
			hCreateDevice0Result,
			HResultToString( hCreateDevice0Result )
		);

		goto Cleanup;
	};


	//
	// Render an empty scene with this device.
	//

	WriteToLog( _T( "Rendering with device 0." ) );

	assert( m_ppDevices[ 0 ] != NULL );
	HRESULT hRenderOriginalSceneResult = RenderScene( 0 );

	if ( FAILED( hRenderOriginalSceneResult ) )
	{
		WriteToLog
		(
			_T( "RenderScene on device 0 failed with error 0x%x (%s)." ),
			hRenderOriginalSceneResult,
			HResultToString( hRenderOriginalSceneResult )
		);

		goto Cleanup;
	};


	//
	// Present the scene on the device.
	//

	WriteToLog( _T( "Presenting with device 0." ) );

	HRESULT hPresentOriginalSceneResult = m_ppDevices[ 0 ]->Present
	(
		NULL,
		NULL,
		NULL,
		NULL
	);

	if ( FAILED( hPresentOriginalSceneResult ) )
	{
		WriteToLog
		(
			_T( "Present of original scene on device 0 returned unexpected result 0x%x (%s)." ),
			hPresentOriginalSceneResult,
			HResultToString( hPresentOriginalSceneResult )
		);

		goto Cleanup;
	};


	//
	// Pump messages resulting from work done on device 0.
	//

	WaitAndPumpMessages( 100 );


	//
	// Minimize the device window.
	//
	// N.B. [SEdmison]:
	// This doesn't seem to work for full-screen device windows.
	// Need to find another way to minimize the window.
	//

	if ( g_TestScenarios[ m_iCurrentScenario ].Devices[ 0 ].bWindowed == TRUE )
	{
		WriteToLog( _T( "Minimizing the swap chain 0 window." ) );

		ShowWindow
		(
			m_phWindows[ 0 ],
			SW_MINIMIZE
		);
	}
	else
	{
		WriteToLog( _T( "Generating Alt-Tab." ) );

		keybd_event( VK_MENU, 0, 0, 0 );
		WaitAndPumpMessages( 50 );
		keybd_event( VK_TAB, 0, 0, 0 );
		WaitAndPumpMessages( 50 );
		keybd_event( VK_TAB, 0, KEYEVENTF_KEYUP, 0 );
		WaitAndPumpMessages( 50 );
		keybd_event( VK_MENU, 0, KEYEVENTF_KEYUP, 0 );
	};


	//
	// Wait for the messages resulting from the minimization to ripple through.
	//

	WaitAndPumpMessages( 1000 );


	HRESULT hRenderOccludedSceneResult = E_FAIL;
	HRESULT hPresentOccludedSceneResult = E_FAIL;
	for
	(
		UINT iCurrentAttempt = 0;
		iCurrentAttempt < 3;
		++iCurrentAttempt
	)
	{
		//
		// Render again with the device.
		//

		WriteToLog( _T( "Rendering again with device 0." ) );

		hRenderOccludedSceneResult = RenderScene( 0 );

		if ( FAILED( hRenderOccludedSceneResult ) )
		{
			WriteToLog
			(
				_T( "RenderScene on device 0 failed with error 0x%x (%s)." ),
				hRenderOccludedSceneResult,
				HResultToString( hRenderOccludedSceneResult )
			);

			goto Cleanup;
		};


		//
		// Present again with the device.
		//

		WriteToLog( _T( "Presenting again with device 0." ) );

		assert( m_ppDevices[ 0 ] != NULL );
		hPresentOccludedSceneResult = m_ppDevices[ 0 ]->Present
		(
			NULL,
			NULL,
			NULL,
			NULL
		);

		if ( FAILED( hPresentOccludedSceneResult ) )
		{
			WriteToLog
			(
				_T( "Present on device 0 failed with error 0x%x (%s)." ),
				hPresentOccludedSceneResult,
				HResultToString( hPresentOccludedSceneResult )	
			);

			goto Cleanup;
		};


		//
		// Pump messages.
		//

		WaitAndPumpMessages( 0 );
	};


#if CHECK_FOR_WINDOWS_7_BUG_408340
	//
	// Call CheckDeviceState, and note any cases where CheckDeviceState
	// returns a different result than Present.
	//

	WriteToLog( _T( "Calling CheckDeviceState on device 0." ) );

	HRESULT hDeviceState = m_ppDevices[ 0 ]->CheckDeviceState( m_phWindows[ 0 ] );

	if ( hDeviceState != hPresentOccludedSceneResult )
	{
		WriteToLog
		(
			_T( "CheckDeviceState on device 0 returned 0x%x (%s), whereas Present returned 0x%x (%s)." ),
			hDeviceState,
			HResultToString( hDeviceState ),
			hPresentOccludedSceneResult,
			HResultToString( hPresentOccludedSceneResult )
		);

		goto Cleanup;
	};
#endif


	//
	// Verify that Present returned what we expected.
	//

	for
	(
		UINT iCurrentResult = 0;
		iCurrentResult < g_TestScenarios[ m_iCurrentScenario ].cExpectedAPIResults;
		++iCurrentResult
	)
	{
		if
		(
			(
				m_D3D9VersionMS > g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MinimumDLLVersionMS ||
				m_D3D9VersionMS == g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MinimumDLLVersionMS &&
					m_D3D9VersionLS >= g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MinimumDLLVersionLS
			) &&
			(
				m_D3D9VersionMS < g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MaximumDLLVersionMS ||
				m_D3D9VersionMS == g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MaximumDLLVersionMS &&
					m_D3D9VersionLS <= g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MaximumDLLVersionLS
			) &&
			hPresentOccludedSceneResult == g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].hExpectedResult
		)
		{
			WriteToLog
			(
				_T( "Present on device 0 returned %s, as expected." ),
				HResultToString( hPresentOccludedSceneResult )
			);

			TestResult = RESULT_PASS;
			break;
		};
	};


	//
	// Always allow S_OK from Present if
	// - We're running on Windows 7.
	// - There are multiple monitors.
	// - Composition is enabled.
	// This is because the occlusion detection logic was
	// updated to take into account that even though
	// something might appear to be occluded on a given
	// monitor, if there is another monitor, thumbnails
	// may be visible on that monitor.
	//

	if ( hPresentOccludedSceneResult == S_OK )
	{
		OSVERSIONINFOEXW VersionInfo;
		ZeroMemory( &VersionInfo, sizeof( VersionInfo ) );
		VersionInfo.dwOSVersionInfoSize = sizeof( VersionInfo );

		BOOL bGetVersionInfoResult = GetVersionExW
		(
			(OSVERSIONINFOW *) &VersionInfo
		);

		if ( bGetVersionInfoResult != TRUE )
		{
			DWORD GetVersionInfoError = GetLastError();

			WriteToLog
			(
				_T( "GetVersionInfoEx failed with error %u." ),
				GetVersionInfoError
			);

			goto Cleanup;
		};

		if
		(
			VersionInfo.dwMajorVersion == 6 &&
			VersionInfo.dwMinorVersion >= 1 ||

			VersionInfo.dwMajorVersion > 6
		)
		{
			if
			(
				IsDWMCompositionEnabled() == S_OK &&
				m_OriginalGDITopology.m_cDevices > 1
			)
			{
				WriteToLog
				(
					_T( "Present on device 0 returned S_OK, which is expected on a multiple-monitor Windows 7 machine with composition enabled." )
				);

				TestResult = RESULT_PASS;
				goto Cleanup;
			};
		};
	};


	if ( TestResult != RESULT_PASS )
	{
		WriteToLog
		(
			_T( "Present on device 0 returned unexpected result 0x%x (%s)." ),
			hPresentOccludedSceneResult,
			HResultToString( hPresentOccludedSceneResult )
		);
	};


	if ( bRestoreWindow == TRUE )
	{
		WriteToLog( _T( "Restoring swap chain 0 window." ) );

		ShowWindow
		(
			m_phWindows[ 0 ],
			SW_RESTORE
		);

		SetForegroundWindow
		(
			m_phWindows[ 0 ]
		);


		WaitAndPumpMessages( 500 );


		//
		// Render again with the device.
		//

		WriteToLog( _T( "Rendering again with device 0." ) );

		hRenderOccludedSceneResult = RenderScene( 0 );

		if ( FAILED( hRenderOccludedSceneResult ) )
		{
			WriteToLog
			(
				_T( "RenderScene on device 0 failed with error 0x%x (%s)." ),
				hRenderOccludedSceneResult,
				HResultToString( hRenderOccludedSceneResult )
			);

			goto Cleanup;
		};


		//
		// Try to Present again with the device.
		//

		WriteToLog( _T( "Presenting again with device 0." ) );

		hPresentOccludedSceneResult = m_ppDevices[ 0 ]->Present
		(
			NULL,
			NULL,
			NULL,
			NULL
		);

		if ( FAILED( hPresentOccludedSceneResult ) )
		{
			WriteToLog
			(
				_T( "Present on device 0 failed with error 0x%x (%s)." ),
				hPresentOccludedSceneResult,
				HResultToString( hPresentOccludedSceneResult )	
			);

			goto Cleanup;
		}
		else if ( hPresentOccludedSceneResult == S_PRESENT_OCCLUDED )
		{
			WriteToLog( _T( "Present on device 0 returned unexpected result S_PRESENT_OCCLUDED." ) );

			goto Cleanup;
		};	
	};


Cleanup:
	//
	// Log troubleshooting information.
	//

	if
	(
		TestResult != RESULT_PASS &&
		TestResult != RESULT_SKIP
	)
	{
		//
		// Start the failure diagnostics section.
		//

		LogDiagnosticsStart();


		//
		// Log details about the foreground window.
		//

		m_WindowHandler.LogDetailsAboutWindow( GetForegroundWindow() );


		//
		// Log additional failure probes.
		//

		LogDiagnostics();


		//
		// Log related bugs.
		//

		LogRelatedBugs();


		//
		// End the failure diagnostics section.
		//

		LogDiagnosticsEnd();
	};


	//
	// Check for adapter topology changes.
	//

	hCheckForAdapterTopologyChangesResult = CheckForAdapterTopologyChanges();

	if ( FAILED( hCheckForAdapterTopologyChangesResult ) )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges failed with error 0x%x (%s)." ),
			hCheckForAdapterTopologyChangesResult,
			HResultToString( hCheckForAdapterTopologyChangesResult )
		);

		TestResult = RESULT_ABORT;
	}
	else if ( hCheckForAdapterTopologyChangesResult == S_FALSE )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges reports that adapter topology has changed." )
		);

		TestResult = RESULT_ABORT;
	};


	return TestResult;
};


///////////////////////////////////////////////////////
// Function:  TestModeChangeDevice0
//
// Purpose:
// Tests the scenario of a mode change while a device
// is rendering.
//
// Called by:
// ExecuteTestCase
///////////////////////////////////////////////////////
TEST_RESULT CMyTest::TestModeChangeDevice0
(
	IN BOOL bValidateWindowPosition
)
{
	//
	// Initialize locals.
	//

	TEST_RESULT TestResult = RESULT_FAIL;


	//
	// Check for adapter topology changes.
	//

	HRESULT hCheckForAdapterTopologyChangesResult = CheckForAdapterTopologyChanges();

	if ( FAILED( hCheckForAdapterTopologyChangesResult ) )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges failed with error 0x%x (%s)." ),
			hCheckForAdapterTopologyChangesResult,
			HResultToString( hCheckForAdapterTopologyChangesResult )
		);

		TestResult = RESULT_ABORT;
		goto Cleanup;
	}
	else if ( hCheckForAdapterTopologyChangesResult == S_FALSE )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges reports that adapter topology has changed." )
		);

		TestResult = RESULT_ABORT;
		goto Cleanup;
	};


	//
	// Show window 0.
	//

	assert( m_phWindows[ 0 ] != NULL );
	DisplayWindow( 0 );


	//
	// Create device 0.
	//

	HRESULT hCreateDevice0Result = CreateDevice( 0 );

	if ( FAILED( hCreateDevice0Result ) )
	{
		WriteToLog
		(
			_T( "CreateDevice on device 0 failed with error 0x%x (%s)." ),
			hCreateDevice0Result,
			HResultToString( hCreateDevice0Result )
		);

		goto Cleanup;
	};


	//
	// Render an empty scene with this device.
	//

	assert( m_ppDevices[ 0 ] != NULL );
	HRESULT hRenderOriginalSceneResult = RenderScene( 0 );

	if ( FAILED( hRenderOriginalSceneResult ) )
	{
		WriteToLog
		(
			_T( "RenderScene on device 0 failed with error 0x%x (%s)." ),
			hRenderOriginalSceneResult,
			HResultToString( hRenderOriginalSceneResult )
		);

		goto Cleanup;
	};


	//
	// Present the scene on the device.
	//

	HRESULT hPresentOriginalSceneResult = m_ppDevices[ 0 ]->Present
	(
		NULL,
		NULL,
		NULL,
		NULL
	);

	if ( FAILED( hPresentOriginalSceneResult ) )
	{
		WriteToLog
		(
			_T( "Present of original scene on device 0 returned unexpected result 0x%x (%s)." ),
			hPresentOriginalSceneResult,
			HResultToString( hPresentOriginalSceneResult )
		);

		goto Cleanup;
	};


	//
	// Pump messages resulting from work done on device 0.
	//

	WaitAndPumpMessages( 100 );


	//
	// Cause a mode change.
	//

	HRESULT hModeChangeResult = CauseModeChange
	(
		MONITOR_UNSPECIFIED
	);

	if ( FAILED( hModeChangeResult ) )
	{
		WriteToLog( _T( "CauseModeChange failed." ) );

		goto Cleanup;
	};


	//
	// Wait for the messages resulting from the mode change to ripple through.
	//

	WaitAndPumpMessages( 2000 );


	HRESULT hRenderOccludedSceneResult = E_FAIL;
	HRESULT hPresentOccludedSceneResult = E_FAIL;
	for
	(
		UINT iCurrentAttempt = 0;
		iCurrentAttempt < 3;
		++iCurrentAttempt
	)
	{
		//
		// Render again with the device.
		//

		hRenderOccludedSceneResult = RenderScene( 0 );

		if ( FAILED( hRenderOccludedSceneResult ) )
		{
			WriteToLog
			(
				_T( "RenderScene on device 0 failed with error 0x%x (%s)." ),
				hRenderOccludedSceneResult,
				HResultToString( hRenderOccludedSceneResult )
			);

			goto Cleanup;
		};


		//
		// Try to Present again with the device.
		//

		hPresentOccludedSceneResult = m_ppDevices[ 0 ]->Present
		(
			NULL,
			NULL,
			NULL,
			NULL
		);

		if ( FAILED( hPresentOccludedSceneResult ) )
		{
			WriteToLog
			(
				_T( "Present on device 0 failed with error 0x%x (%s)." ),
				hPresentOccludedSceneResult,
				HResultToString( hPresentOccludedSceneResult )	
			);

			goto Cleanup;
		};


		//
		// Pump messages.
		//

		WaitAndPumpMessages( 0 );
	};


#if CHECK_FOR_WINDOWS_7_BUG_408340
	//
	// Call CheckDeviceState, and note any cases where CheckDeviceState
	// returns a different result than Present.
	//

	HRESULT hDeviceState = m_ppDevices[ 0 ]->CheckDeviceState( m_phWindows[ 0 ] );

	if ( hDeviceState != hPresentOccludedSceneResult )
	{
		WriteToLog
		(
			_T( "CheckDeviceState on device 0 returned 0x%x (%s), whereas Present returned 0x%x (%s)." ),
			hDeviceState,
			HResultToString( hDeviceState ),
			hPresentOccludedSceneResult,
			HResultToString( hPresentOccludedSceneResult )
		);

		goto Cleanup;
	};
#endif


	//
	// Verify that Present returned what we expected.
	//

	for
	(
		UINT iCurrentResult = 0;
		iCurrentResult < g_TestScenarios[ m_iCurrentScenario ].cExpectedAPIResults;
		++iCurrentResult
	)
	{
		if
		(
			(
				m_D3D9VersionMS > g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MinimumDLLVersionMS ||
				m_D3D9VersionMS == g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MinimumDLLVersionMS &&
					m_D3D9VersionLS >= g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MinimumDLLVersionLS
			) &&
			(
				m_D3D9VersionMS < g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MaximumDLLVersionMS ||
				m_D3D9VersionMS == g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MaximumDLLVersionMS &&
					m_D3D9VersionLS <= g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].MaximumDLLVersionLS
			) &&
			hPresentOccludedSceneResult == g_TestScenarios[ m_iCurrentScenario ].ExpectedAPIResults[ iCurrentResult ].hExpectedResult
		)
		{
			WriteToLog
			(
				_T( "Present returned %s, as expected." ),
				HResultToString( hPresentOccludedSceneResult )
			);

			TestResult = RESULT_PASS;
			break;
		};
	};


	//
	// Always allow S_OK from Present if
	// - We're running on Windows 7.
	// - There are multiple monitors.
	// - Composition is enabled.
	// This is because the occlusion detection logic was
	// updated to take into account that even though
	// something might appear to be occluded on a given
	// monitor, if there is another monitor, thumbnails
	// may be visible on that monitor.
	//

	if ( hPresentOccludedSceneResult == S_OK )
	{
		OSVERSIONINFOEXW VersionInfo;
		ZeroMemory( &VersionInfo, sizeof( VersionInfo ) );
		VersionInfo.dwOSVersionInfoSize = sizeof( VersionInfo );

		BOOL bGetVersionInfoResult = GetVersionExW
		(
			(OSVERSIONINFOW *) &VersionInfo
		);

		if ( bGetVersionInfoResult != TRUE )
		{
			DWORD GetVersionInfoError = GetLastError();

			WriteToLog
			(
				_T( "GetVersionInfoEx failed with error %u." ),
				GetVersionInfoError
			);

			goto Cleanup;
		};

		if
		(
			VersionInfo.dwMajorVersion == 6 &&
			VersionInfo.dwMinorVersion >= 1 ||

			VersionInfo.dwMajorVersion > 6
		)
		{
			if
			(
				IsDWMCompositionEnabled() == S_OK &&
				m_OriginalGDITopology.m_cDevices > 1
			)
			{
				WriteToLog
				(
					_T( "Present returned S_OK, which is expected on a multiple-monitor Windows 7 machine with composition enabled." )
				);

				TestResult = RESULT_PASS;
				goto Cleanup;
			};
		};
	};


	if ( TestResult != RESULT_PASS )
	{
		WriteToLog
		(
			_T( "Present returned unexpected result 0x%x (%s)." ),
			hPresentOccludedSceneResult,
			HResultToString( hPresentOccludedSceneResult )
		);
	};


	//
	// Restore the display mode.
	//

	CauseModeChange
	(
		MONITOR_UNSPECIFIED,
		1024,
		768,
		32,
		60,
		DMDO_DEFAULT,
		CDS_UPDATEREGISTRY
	);


	//
	// Pump messages resulting from the display mode change.
	//

	WaitAndPumpMessages( 1000 );


	//
	// Validate the window's final position.
	//

	if ( bValidateWindowPosition == TRUE )
	{
		RECT FinalWindowPosition = { 0, 0, 0, 0 };
		GetWindowRect
		(
			m_phWindows[ 0 ],
			&FinalWindowPosition
		);

		if
		(
			FinalWindowPosition.left != g_TestScenarios[ m_iCurrentScenario ].Devices[ 0 ].Left ||
			FinalWindowPosition.top != g_TestScenarios[ m_iCurrentScenario ].Devices[ 0 ].Top ||
			FinalWindowPosition.right - FinalWindowPosition.left != g_TestScenarios[ m_iCurrentScenario ].Devices[ 0 ].Width ||
			FinalWindowPosition.bottom - FinalWindowPosition.top != g_TestScenarios[ m_iCurrentScenario ].Devices[ 0 ].Height
		)
		{
			TestResult = RESULT_FAIL;

			WriteToLog
			(
				_T( "Window moved or resized after mode change.  Expected:  (%li, %li) %li x %li.  Actual:  (%li, %li) %li x %li" ),
				g_TestScenarios[ m_iCurrentScenario ].Devices[ 0 ].Left,
				g_TestScenarios[ m_iCurrentScenario ].Devices[ 0 ].Top,
				g_TestScenarios[ m_iCurrentScenario ].Devices[ 0 ].Width,
				g_TestScenarios[ m_iCurrentScenario ].Devices[ 0 ].Height,
				FinalWindowPosition.left,
				FinalWindowPosition.top,
				FinalWindowPosition.right - FinalWindowPosition.left,
				FinalWindowPosition.bottom - FinalWindowPosition.top
			);
		}
		else
		{
			WriteToLog
			(
				_T( "Window found at expected position and size:  (%li, %li) %li x %li." ),
				FinalWindowPosition.left,
				FinalWindowPosition.top,
				FinalWindowPosition.right - FinalWindowPosition.left,
				FinalWindowPosition.bottom - FinalWindowPosition.top
			);
		};
	};


Cleanup:
	//
	// Log troubleshooting information.
	//

	if
	(
		TestResult != RESULT_PASS &&
		TestResult != RESULT_SKIP
	)
	{
		//
		// Start the failure diagnostics section.
		//

		LogDiagnosticsStart();


		//
		// Log details about the foreground window.
		//

		m_WindowHandler.LogDetailsAboutWindow( GetForegroundWindow() );


		//
		// Log additional failure probes.
		//

		LogDiagnostics();


		//
		// Log related bugs.
		//

		LogRelatedBugs();


		//
		// End the failure diagnostics section.
		//

		LogDiagnosticsEnd();
	};


	//
	// Restore the display mode.
	//

	CauseModeChange
	(
		MONITOR_UNSPECIFIED,
		1024,
		768,
		32,
		60,
		DMDO_DEFAULT,
		CDS_UPDATEREGISTRY
	);


	//
	// Pump messages resulting from the display mode change.
	//

	WaitAndPumpMessages( 1000 );


	//
	// Check for adapter topology changes.
	//

	hCheckForAdapterTopologyChangesResult = CheckForAdapterTopologyChanges();

	if ( FAILED( hCheckForAdapterTopologyChangesResult ) )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges failed with error 0x%x (%s)." ),
			hCheckForAdapterTopologyChangesResult,
			HResultToString( hCheckForAdapterTopologyChangesResult )
		);

		TestResult = RESULT_ABORT;
	}
	else if ( hCheckForAdapterTopologyChangesResult == S_FALSE )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges reports that adapter topology has changed." )
		);

		TestResult = RESULT_ABORT;
	};


	return TestResult;
};


///////////////////////////////////////////////////////
// Function:  TestCreationOfDevice0WhileMonitorSuspended
//
// Purpose:
// Tests the scenario of a device's being occluded
// the monitor shuts down or goes into standby.
//
// Called by:
// ExecuteTestCase
///////////////////////////////////////////////////////
TEST_RESULT CMyTest::TestCreationOfDevice0WhileMonitorSuspended
(
	IN LPARAM MonitorPowerState
)
{
	//
	// Initialize locals.
	//

	TEST_RESULT TestResult = RESULT_FAIL;


	//
	// Check for adapter topology changes.
	//

	HRESULT hCheckForAdapterTopologyChangesResult = CheckForAdapterTopologyChanges();

	if ( FAILED( hCheckForAdapterTopologyChangesResult ) )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges failed with error 0x%x (%s)." ),
			hCheckForAdapterTopologyChangesResult,
			HResultToString( hCheckForAdapterTopologyChangesResult )
		);

		TestResult = RESULT_ABORT;
		goto Cleanup;
	}
	else if ( hCheckForAdapterTopologyChangesResult == S_FALSE )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges reports that adapter topology has changed." )
		);

		TestResult = RESULT_ABORT;
		goto Cleanup;
	};


	//
	// Suspend the monitor.
	//

	HRESULT hSetMonitorPowerStateResult = SetMonitorPowerState( MonitorPowerState );

	if ( FAILED( hSetMonitorPowerStateResult ) )
	{
		WriteToLog
		(
			_T( "SetMonitorPowerState failed with error 0x%x (%s)." ),
			hSetMonitorPowerStateResult,
			HResultToString( hSetMonitorPowerStateResult )
		);

		goto Cleanup;
	};


	//
	// Wait for the monitor to go off.
	//

	WaitAndPumpMessages( 1000 );


	//
	// Create device 0.
	//

	HRESULT hCreateDeviceResult = CreateDevice( 0 );

	if ( FAILED( hCreateDeviceResult ) )
	{
		WriteToLog
		(
			_T( "CreateDevice on device 0 failed with error 0x%x (%s)." ),
			hCreateDeviceResult,
			HResultToString( hCreateDeviceResult )
		);

		goto Cleanup;
	};


	//
	// If we made it this far, then we're good to go.
	//

	TestResult = RESULT_PASS;


Cleanup:
	//
	// Log troubleshooting information.
	//

	if
	(
		TestResult != RESULT_PASS &&
		TestResult != RESULT_SKIP
	)
	{
		//
		// Start the failure diagnostics section.
		//

		LogDiagnosticsStart();


		//
		// Log details about the foreground window.
		//

		m_WindowHandler.LogDetailsAboutWindow( GetForegroundWindow() );


		//
		// Log additional failure probes.
		//

		LogDiagnostics();


		//
		// Log related bugs.
		//

		LogRelatedBugs();


		//
		// End the failure diagnostics section.
		//

		LogDiagnosticsEnd();
	};


	//
	// Restore the monitor power state.
	//

	SetMonitorPowerState( MONITOR_ON );


	//
	// Wait for the monitor to come back on.
	//

	WaitAndPumpMessages( 100 );


	//
	// Check for adapter topology changes.
	//

	hCheckForAdapterTopologyChangesResult = CheckForAdapterTopologyChanges();

	if ( FAILED( hCheckForAdapterTopologyChangesResult ) )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges failed with error 0x%x (%s)." ),
			hCheckForAdapterTopologyChangesResult,
			HResultToString( hCheckForAdapterTopologyChangesResult )
		);

		TestResult = RESULT_ABORT;
	}
	else if ( hCheckForAdapterTopologyChangesResult == S_FALSE )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges reports that adapter topology has changed." )
		);

		TestResult = RESULT_ABORT;
	};


	return TestResult;
};


///////////////////////////////////////////////////////
// Function:  TestCreationOfDDraw0WhileMonitorSuspended
//
// Purpose:
// Tests the scenario of creating DirectDraw while the
// the monitor is shut down or in standby.
//
// Called by:
// ExecuteTestCase
///////////////////////////////////////////////////////
TEST_RESULT CMyTest::TestCreationOfDDrawWhileMonitorSuspended
(
	IN LPARAM MonitorPowerState
)
{
	//
	// Initialize locals.
	//

	TEST_RESULT TestResult = RESULT_FAIL;
	IDirectDraw * pDDraw = NULL;

	//
	// Check for adapter topology changes.
	//

	HRESULT hCheckForAdapterTopologyChangesResult = CheckForAdapterTopologyChanges();

	if ( FAILED( hCheckForAdapterTopologyChangesResult ) )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges failed with error 0x%x (%s)." ),
			hCheckForAdapterTopologyChangesResult,
			HResultToString( hCheckForAdapterTopologyChangesResult )
		);

		TestResult = RESULT_ABORT;
		goto Cleanup;
	}
	else if ( hCheckForAdapterTopologyChangesResult == S_FALSE )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges reports that adapter topology has changed." )
		);

		TestResult = RESULT_ABORT;
		goto Cleanup;
	};


	//
	// Suspend the monitor.
	//

	HRESULT hSetMonitorPowerStateResult = SetMonitorPowerState( MonitorPowerState );

	if ( FAILED( hSetMonitorPowerStateResult ) )
	{
		WriteToLog
		(
			_T( "SetMonitorPowerState failed with error 0x%x (%s)." ),
			hSetMonitorPowerStateResult,
			HResultToString( hSetMonitorPowerStateResult )
		);

		goto Cleanup;
	};


	//
	// Wait for the monitor to go off.
	//

	WaitAndPumpMessages( 2000 );


	//
	// Try to create DirectDraw.
	//


	__try
	{
		HRESULT hCreateDDrawResult = DirectDrawCreate
		(
			NULL,
			&pDDraw,
			NULL
		);

		if ( FAILED( hCreateDDrawResult ) )
		{
			WriteToLog( _T( "DirectDrawCreate failed." ) );

			goto Cleanup;
		};
	}
	__except
	(
		ExceptionFilter
		(
			GetExceptionCode(),
			GetExceptionInformation()
		)
	)
	{
		WriteToLog( _T( "DirectDrawCreate caused an unhandled exception." ) );

		goto Cleanup;
	};


	//
	// If we made it this far, then we're good to go.
	//

	TestResult = RESULT_PASS;


Cleanup:
	//
	// Log troubleshooting information.
	//

	if
	(
		TestResult != RESULT_PASS &&
		TestResult != RESULT_SKIP
	)
	{
		//
		// Start the failure diagnostics section.
		//

		LogDiagnosticsStart();


		//
		// Log details about the foreground window.
		//

		m_WindowHandler.LogDetailsAboutWindow( GetForegroundWindow() );


		//
		// Log additional failure probes.
		//

		LogDiagnostics();


		//
		// Log related bugs.
		//

		LogRelatedBugs();


		//
		// End the failure diagnostics section.
		//

		LogDiagnosticsEnd();
	};


	//
	// Restore the monitor power state.
	//

	SetMonitorPowerState( MONITOR_ON );


	//
	// Wait for the monitor to come back on.
	//

	WaitAndPumpMessages( 100 );


	//
	// Release our DirectDraw object.
	//

	if ( pDDraw != NULL )
	{
		pDDraw->Release();
		pDDraw = NULL;
	};


	//
	// Check for adapter topology changes.
	//

	hCheckForAdapterTopologyChangesResult = CheckForAdapterTopologyChanges();

	if ( FAILED( hCheckForAdapterTopologyChangesResult ) )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges failed with error 0x%x (%s)." ),
			hCheckForAdapterTopologyChangesResult,
			HResultToString( hCheckForAdapterTopologyChangesResult )
		);

		TestResult = RESULT_ABORT;
	}
	else if ( hCheckForAdapterTopologyChangesResult == S_FALSE )
	{
		WriteToLog
		(
			_T( "CheckForAdapterTopologyChanges reports that adapter topology has changed." )
		);

		TestResult = RESULT_ABORT;
	};


	return TestResult;
};


///////////////////////////////////////////////////////
// Function:  CreateDevice
//
// Purpose:
// Helper routine to create a device in the specified
// device slot.
///////////////////////////////////////////////////////
HRESULT CMyTest::CreateDevice
(
	UINT CurrentDevice
)
{
	//
	// Determine whether the device supports hardware vertex processing.
	//

	DWORD fDeviceBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;


	// If hardware vertex processing isn't supported...
	if ( ( m_D3D9Topology.m_AdapterCaps[ m_AdapterTopologyMap.m_D3D9AdapterSlotForGDIDeviceSlot[ m_GDITopology.m_DeviceSlotForMonitorSlot[ g_TestScenarios[ m_iCurrentScenario ].Devices[ CurrentDevice ].Monitor ] ] ].DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 )
	{
		// ...then specify software vertex processing instead.
 		fDeviceBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	};

	//
	//  WGFFramework performs FPU State checks.  Need to preserve FPU state between tests.
	//
	fDeviceBehavior |= D3DCREATE_FPU_PRESERVE;

	//
	// Determine whether we're creating a windowed or full-screen device.
	//

	if ( g_TestScenarios[ m_iCurrentScenario ].Devices[ CurrentDevice ].bWindowed == TRUE )
	{
		D3DPRESENT_PARAMETERS WindowedPresentParameters;
		ZeroMemory( &WindowedPresentParameters, sizeof( WindowedPresentParameters ) );

		WindowedPresentParameters.Windowed = TRUE;
		WindowedPresentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
		WindowedPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		WindowedPresentParameters.EnableAutoDepthStencil = TRUE;
		WindowedPresentParameters.AutoDepthStencilFormat = D3DFMT_D16;
		WindowedPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;


		WriteToLog
		(
			_T( "Creating windowed device %u on adapter %u." ),
			CurrentDevice,
			m_D3D9Topology.m_AdapterOrdinalForAdapterSlot[ m_AdapterTopologyMap.m_D3D9AdapterSlotForGDIDeviceSlot[ m_GDITopology.m_DeviceSlotForMonitorSlot[ g_TestScenarios[ m_iCurrentScenario ].Devices[ CurrentDevice ].Monitor ] ] ]
		);

		HRESULT hCreateDeviceResult = m_pD3D9Ex->CreateDeviceEx
		(
			m_D3D9Topology.m_AdapterOrdinalForAdapterSlot[ m_AdapterTopologyMap.m_D3D9AdapterSlotForGDIDeviceSlot[ m_GDITopology.m_DeviceSlotForMonitorSlot[ g_TestScenarios[ m_iCurrentScenario ].Devices[ CurrentDevice ].Monitor ] ] ],
			g_Application.m_DeviceType,
			m_phWindows[ CurrentDevice ],
			fDeviceBehavior,
			&WindowedPresentParameters,
			NULL,
			&m_ppDevices[ CurrentDevice ]
		);

		if ( FAILED( hCreateDeviceResult ) )
		{
			WriteToLog
			(
				_T( "CreateDevice:  CreateDeviceEx failed with error 0x%x (%s) for windowed device %u." ),
				hCreateDeviceResult,
				HResultToString( hCreateDeviceResult ),
				CurrentDevice
			);

			return hCreateDeviceResult;
		};
	}
	else
	{
		D3DPRESENT_PARAMETERS FullscreenPresentParameters;
		ZeroMemory( &FullscreenPresentParameters, sizeof( FullscreenPresentParameters ) );

		FullscreenPresentParameters.Windowed = FALSE;
		FullscreenPresentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
		FullscreenPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		FullscreenPresentParameters.EnableAutoDepthStencil = TRUE;
		FullscreenPresentParameters.AutoDepthStencilFormat = D3DFMT_D16;
		FullscreenPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		FullscreenPresentParameters.BackBufferWidth = g_TestScenarios[ m_iCurrentScenario ].Devices[ CurrentDevice ].Width;
		FullscreenPresentParameters.BackBufferHeight = g_TestScenarios[ m_iCurrentScenario ].Devices[ CurrentDevice ].Height;
		FullscreenPresentParameters.FullScreen_RefreshRateInHz = 60;
		FullscreenPresentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;


		D3DDISPLAYMODEEX FullscreenMode;
		ZeroMemory( &FullscreenMode, sizeof( FullscreenMode ) );

		FullscreenMode.Size = sizeof( FullscreenMode );
		FullscreenMode.Width = g_TestScenarios[ m_iCurrentScenario ].Devices[ CurrentDevice ].Width;
		FullscreenMode.Height = g_TestScenarios[ m_iCurrentScenario ].Devices[ CurrentDevice ].Height;
		FullscreenMode.RefreshRate = 60;
		FullscreenMode.Format = D3DFMT_X8R8G8B8;
		FullscreenMode.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;


		WriteToLog
		(
			_T( "Creating full-screen device %u on adapter %u." ),
			CurrentDevice,
			m_D3D9Topology.m_AdapterOrdinalForAdapterSlot[ m_AdapterTopologyMap.m_D3D9AdapterSlotForGDIDeviceSlot[ m_GDITopology.m_DeviceSlotForMonitorSlot[ g_TestScenarios[ m_iCurrentScenario ].Devices[ CurrentDevice ].Monitor ] ] ]
		);

		HRESULT hCreateDeviceResult = m_pD3D9Ex->CreateDeviceEx
		(
			m_D3D9Topology.m_AdapterOrdinalForAdapterSlot[ m_AdapterTopologyMap.m_D3D9AdapterSlotForGDIDeviceSlot[ m_GDITopology.m_DeviceSlotForMonitorSlot[ g_TestScenarios[ m_iCurrentScenario ].Devices[ CurrentDevice ].Monitor ] ] ],
			g_Application.m_DeviceType,
			m_phWindows[ CurrentDevice ],
			fDeviceBehavior,
			&FullscreenPresentParameters,
			&FullscreenMode,
			&m_ppDevices[ CurrentDevice ]
		);


/*
		//
		// Debug spew.
		//

		BOOL bIsWindowVisible = IsWindowVisible( m_phWindows[ CurrentDevice ] );
		WriteToLog
		(
			_T( "IsWindowVisible after CreateDeviceEx reports:  %s." ),
			bIsWindowVisible == TRUE ? _T( "TRUE" ) : _T( "FALSE" )
		);
*/


		if ( FAILED( hCreateDeviceResult ) )
		{
			WriteToLog
			(
				_T( "CreateDevice:  CreateDeviceEx failed with error 0x%x (%s) for full-screen device %u." ),
				hCreateDeviceResult,
				HResultToString( hCreateDeviceResult ),
				CurrentDevice
			);

			return hCreateDeviceResult;
		}
		else if ( hCreateDeviceResult != S_OK )
		{
			WriteToLog
			(
				_T( "CreateDevice:  CreateDeviceEx returned unexpected result 0x%x (%s)." ),
				hCreateDeviceResult,
				HResultToString( hCreateDeviceResult )
			);
		};
	};


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  DisplayWindow
//
// Purpose:
// Displays the specified window.
///////////////////////////////////////////////////////
HRESULT CMyTest::DisplayWindow
(
	IN UINT iWindow
)
{
	//
	// Validate parameters.
	//

	if ( iWindow >= 10 )
	{
		return E_INVALIDARG;
	};

	assert( iWindow < g_TestScenarios[ m_iCurrentScenario ].cDevices );
	if ( iWindow >= g_TestScenarios[ m_iCurrentScenario ].cDevices )
	{
		return E_INVALIDARG;
	};


	if ( g_TestScenarios[ m_iCurrentScenario ].Devices[ iWindow ].bVisible == FALSE )
	{
		return S_FALSE;
	};


	assert( m_phWindows[ iWindow ] != NULL );
	if ( m_phWindows[ iWindow ] == NULL )
	{
		return E_INVALIDARG;
	};


	//
	// Show the window.
	//

	ShowWindow
	(
		m_phWindows[ iWindow ],
		SW_SHOWDEFAULT
	);

	UpdateWindow( m_phWindows[ iWindow ] );


	
	//
	// Move it to the foreground.
	//

	SetForegroundWindow( m_phWindows[ iWindow ] );


	//
	// Make it topmost if needed.
	//

	if ( g_TestScenarios[ m_iCurrentScenario ].Devices[ iWindow ].bWindowed == FALSE )
	{
		LONG_PTR WindowStyle = GetWindowLongPtr
		(
			m_phWindows[ iWindow ],
			GWL_STYLE
		);

		assert( (WindowStyle & WS_POPUP) != 0 );
		assert( (WindowStyle & WS_OVERLAPPED) == 0 );

		LONG_PTR WindowStyleEx = GetWindowLongPtr
		(
			m_phWindows[ iWindow ],
			GWL_EXSTYLE
		);

		assert( (WindowStyleEx & WS_EX_TOPMOST) != 0 );

		SetWindowPos
		(
			m_phWindows[ iWindow ],
			HWND_TOPMOST,
			0,
			0,
			0,
			0,
			SWP_NOSIZE | SWP_NOMOVE
		);
	};


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  LogRelatedBugs
//
// Purpose:
// Logs a line of debug spew mentioning the bugs that
// might be related to a failure.
///////////////////////////////////////////////////////
HRESULT CMyTest::LogRelatedBugs()
{
	HRESULT hOverallResult = E_FAIL;

	if ( g_TestScenarios[ m_iCurrentScenario ].cRelatedBugs == 0 )
	{
		hOverallResult = S_FALSE;

		goto Cleanup;
	};


	TCHAR SeeAlsoBuffer[ 256 ] = _T( "This failure could be related to the following bugs:  " );

	for
	(
		UINT iCurrentBug = 0;
		iCurrentBug < g_TestScenarios[ m_iCurrentScenario ].cRelatedBugs;
		++iCurrentBug
	)
	{
		if ( iCurrentBug > 0 )
		{
			StringCchCat
			(
				SeeAlsoBuffer,
				256,
				_T( ", " )
			);
		};

		TCHAR FormattingBuffer[ 10 ] = _T( "" );
		StringCchPrintf
		(
			FormattingBuffer,
			10,
			_T( "%u" ),
			g_TestScenarios[ m_iCurrentScenario ].RelatedBugs[ iCurrentBug ]
		);

		StringCchCat
		(
			SeeAlsoBuffer,
			256,
			FormattingBuffer
		);
	};


	WriteToLog
	(
		SeeAlsoBuffer
	);


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  RenderScene
//
// Purpose:
// Helper method to render an empty scene on the
// specified device.  That is, we Clear, BeginScene,
// and EndScene, with no actual rendering in between.
// (Why bother?)
///////////////////////////////////////////////////////
HRESULT CMyTest::RenderScene
(
	IN UINT CurrentDevice
)
{
	//
	// Validate parameters.
	//

	assert( m_ppDevices[ CurrentDevice ] != NULL );
	if ( m_ppDevices[ CurrentDevice ] == NULL )
	{
		return E_INVALIDARG;
	};


	//
	// Clear to a solid color (blue).
	//

	HRESULT hClearDeviceResult = m_ppDevices[ CurrentDevice ]->Clear
	(
		0,
		NULL,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0,0,255),
		1.0f,
		0
	);

	if ( FAILED( hClearDeviceResult ) )
	{
		WriteToLog( _T( "Clear failed." ) );

		return hClearDeviceResult;
	};


	//
	// Begin the scene.
	//

	HRESULT hBeginSceneResult = m_ppDevices[ CurrentDevice ]->BeginScene();

	if ( FAILED( hBeginSceneResult ) )
	{
		WriteToLog( _T( "BeginScene failed." ) );

		return hBeginSceneResult;
	};


	//
	// Render something.
	//
	// @TODO [SEdmison]:
	//


	//
	// End the scene.
	//

	HRESULT hEndSceneResult = m_ppDevices[ CurrentDevice ]->EndScene();

	if ( FAILED( hEndSceneResult ) )
	{
		WriteToLog( _T( "EndScene failed." ) );

		return hEndSceneResult;
	};


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  MsgProc
//
// Purpose:
// Message handler routine.
//
// Called by:
// StaticMsgProc
///////////////////////////////////////////////////////
LRESULT CMyTest::MsgProc
(
	HWND hWindow,
	UINT Message,
	WPARAM wParam,
	LPARAM lParam
)
{
	//
	// Do default handling of unrecognized messages.
	//

	return DefWindowProc
	(
		hWindow,
		Message,
		wParam,
		lParam
	);
};


///////////////////////////////////////////////////////
// Function:  CMyTestApp
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CMyTestApp::CMyTestApp()
{
};


///////////////////////////////////////////////////////
// Function:  GetTestType
//
// Purpose:
// Returns the default type for all test groups in the
// app.
///////////////////////////////////////////////////////
TESTTYPE CMyTestApp::GetTestType() const
{
	return TESTTYPE_CONF;
};


///////////////////////////////////////////////////////
// Function:  InitTestGroups
//
// Purpose:
// Registers test groups.
///////////////////////////////////////////////////////
bool CMyTestApp::InitTestGroups()
{
	// Register test class with the group manager.
	if(!ADD_TEST(_T("Occlusion"), CMyTest))
		return false;

	return true;
};


/////////////////////////////////////////////////////////////////
//
// Globals
//
/////////////////////////////////////////////////////////////////

// Singleton application object.
CMyTestApp g_Application;
