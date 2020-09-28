//////////////////////////////////////////////////////////////////////
// File:  Occlusion.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for Direct3D 9.L occlusion functional test.
//
// History:
// 11 Oct 2005  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Test infrastructure headers
//

#include <ForegroundWindowHandler.hpp>
#include <D3D9CustomTestApp.hpp>
#include <CustomTest.hpp>
#include <D3D9CustomTest.hpp>
#include <D3D9ToGDIAdapterTopologyMap.hpp>


//
// Win32/DirectX headers
//

#include <ddraw.h>


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Struct:  DeviceDescriptor
//
// Purpose:
// Describes the set of devices to be created for a
// test scenario.
///////////////////////////////////////////////////////
struct DeviceDescriptor
{
	UINT Left;
	UINT Top;
	UINT Width;
	UINT Height;
	BOOL bVisible;
	BOOL bWindowed;
	UINT Monitor;
};


///////////////////////////////////////////////////////
// Struct:  ExpectedAPIResult
//
// Purpose:
// Contains the expected HRESULT to be returned from a
// given function call, assuming that the version of
// the DLL under test is within the specified version
// range.
///////////////////////////////////////////////////////
struct ExpectedAPIResult
{
	DWORD MinimumDLLVersionMS;
	DWORD MinimumDLLVersionLS;

	DWORD MaximumDLLVersionMS;
	DWORD MaximumDLLVersionLS;

	HRESULT hExpectedResult;
};


///////////////////////////////////////////////////////
// Struct:  TestScenario
//
// Purpose:
// Wraps information about a test scenario.
///////////////////////////////////////////////////////
struct TestScenario
{
	LPCTSTR Name;
	BOOL bEnabled;

	UINT cDevices;
	DeviceDescriptor Devices[ 5 ];

	UINT cExpectedAPIResults;
	ExpectedAPIResult ExpectedAPIResults[ 5 ];

	UINT cRelatedBugs;
	UINT RelatedBugs[ 5 ];
};


///////////////////////////////////////////////////////
// Class:  CMyTestApp
//
// Purpose:
// Test application object.
//
// Notes:
// Singleton application object derived from
// CWGFTestFramework.  Required by the WGF framework.
///////////////////////////////////////////////////////
class CMyTestApp
:
	public CD3D9CustomTestApp
{
	//
	// Construction/destruction
	//

	public:
		CMyTestApp();

		//virtual ~CMyTestApp();


	//
	// Public framework method overrides
	//

	public:
		// Returns the default type for all test groups in the app.
		virtual TESTTYPE GetTestType() const;

		// Registers test groups.
		virtual bool InitTestGroups();
};


///////////////////////////////////////////////////////
// Class:  CMyTest
//
// Purpose:
// Test group object.
///////////////////////////////////////////////////////
class CMyTest
:
	public CD3D9CustomTest< CMyTestApp >
{
	//
	// Construction/destruction.
	//

	public:
		CMyTest();

		//virtual ~CMyTest();


	//
	// Public framework method overrides
	//

	public:
		// Initializes parameters.
		void InitTestParameters();

		// Per-test-case setup.
		TEST_RESULT SetupTestCase();

		// Runs a test case.
		TEST_RESULT ExecuteTestCase();

		// Per-test-case cleanup.
		void CleanupTestCase();


	//
	// Test scenario methods
	//

	protected:
		TEST_RESULT TestOcclusionOfDevice0ByDevice1
		(
			IN BOOL bDoModeChange,
			IN BOOL bTolerateOcclusionFailureOnWDDM_1_0   // Allow Present to return S_OK intead of occluded on WDDM1.0.  (Work around Windows7 bug #618143)
		);

		TEST_RESULT TestOcclusionOfDevice0ByMonitorSuspend
		(
			IN LPARAM MonitorPowerState
		);

		TEST_RESULT TestOcclusionOfDevice0ByMinimization
		(
			IN BOOL bRestoreWindow
		);

		TEST_RESULT TestModeChangeDevice0
		(
			IN BOOL bValidateWindowPosition
		);

		TEST_RESULT TestCreationOfDevice0WhileMonitorSuspended
		(
			IN LPARAM MonitorPowerState
		);

		TEST_RESULT TestCreationOfDDrawWhileMonitorSuspended
		(
			IN LPARAM MonitorPowerState
		);


	//
	// Helper methods
	//

	protected:
		// Creates a Direct3D 9.L device in the specified device slot.
		HRESULT CreateDevice
		(
			IN UINT CurrentDevice
		);

		// Boilerplate ShowWindow/UpdateWindow code.
		HRESULT DisplayWindow
		(
			IN UINT iWindow
		);

		// Helper function to log info about potentially related bugs.
		HRESULT LogRelatedBugs();

		// Boilerplate BeginScene/EndScene code.
		HRESULT RenderScene
		(
			IN UINT CurrentDevice
		);


	//
	// Static helper methods
	//

	public:
		// Window message proc.
		static LRESULT MsgProc
		(
			HWND hWindow,
			UINT Message,
			WPARAM wParam,
			LPARAM lParam
		);


	//
	// Data members
	//

	protected:
		// Scenario we're working on.
		int m_iCurrentScenario;

		// Window class.
		WNDCLASSEX m_WindowClass;

		// Window handles.
		HWND m_phWindows[ 10 ];

		// Direct3D device objects.
		IDirect3DDevice9Ex * m_ppDevices[ 10 ];

		DWORD m_D3D9VersionMS;
		DWORD m_D3D9VersionLS;

		CGDIAdapterTopology m_GDITopology;
		CD3D9AdapterTopology m_D3D9Topology;
		CD3D9ToGDIAdapterTopologyMap m_AdapterTopologyMap;

		CForegroundWindowHandler m_WindowHandler;
};


/////////////////////////////////////////////////////////////////
//
// Globals
//
/////////////////////////////////////////////////////////////////

//
// Test scenario data
//

const TestScenario g_TestScenarios[] =
{
	//////////////////////////////////////////////////
	//
	// Occlusion scenarios
	//
	//////////////////////////////////////////////////
	{
		// Scenario 0
		_T( "Occlusion of windowed device by visible full-screen device" ),
		TRUE,
		2,
		{
			{ 0, 0,  640, 480, TRUE, TRUE, MONITOR_CENTER },
			{ 0, 0, 1024, 768, TRUE, FALSE, MONITOR_CENTER }
		},
		1,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_OCCLUDED }
		},
		2,
		{
			1533717,
			1435439
		}
	},
	{
		// Scenario 1
		_T( "Occlusion of windowed device by hidden full-screen device" ),
		FALSE,
		2,
		{
			{ 0, 0,  640, 480, TRUE, TRUE, MONITOR_CENTER },
			{ 0, 0, 1024, 768, FALSE, FALSE, MONITOR_CENTER }
		},
		1,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_OCCLUDED }
		},
		2,
		{
			1533717,
			1435439
		}
	},
	{
		// Scenario 2
		_T( "Occlusion of windowed device by visible full-screen device after mode change" ),
		TRUE,
		2,
		{
			{ 0, 0,  640, 480, TRUE, TRUE, MONITOR_CENTER },
			{ 0, 0, 1024, 768, TRUE, FALSE, MONITOR_CENTER }
		},
		2,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_OCCLUDED },
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_MODE_CHANGED }
		},
		0,
		{
		}
	},
	{
		// Scenario 3
		_T( "Occlusion of windowed device by windowed device" ),
		TRUE,
		2,
		{
			{ 50, 50, 320, 240, TRUE, TRUE, MONITOR_CENTER },
			{  0,  0, 640, 480, TRUE, TRUE, MONITOR_CENTER }
		},
		1,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_OK }
		},
		1,
		{
			1533717,
		}
	},
	{
		// Scenario 4
		_T( "Occlusion of windowed device by minimization" ),
		TRUE,
		1,
		{
			{ 0, 0, 640, 480, TRUE, TRUE, MONITOR_CENTER }
		},
		2,
		{
			{ 0, 0, MAKELONG( 1, 6 ), MAKELONG( 0, 7045 ), S_OK },
			{ MAKELONG( 1, 6 ), MAKELONG( 0, 7046 ), 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_OCCLUDED }
		},
		1,
		{
			1533717,
		}
	},
	{
		// Scenario 5
		_T( "Occlusion of windowed device by monitor standby" ),
		TRUE,
		1,
		{
			{ 0, 0, 640, 480, TRUE, TRUE, MONITOR_CENTER }
		},
		1,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_OCCLUDED }
		},
		1,
		{
			1533717,
		}
	},
	{
		// Scenario 6
		_T( "Occlusion of windowed device by monitor shutdown" ),
		TRUE,
		1,
		{
			{ 0, 0,  640,  480, TRUE, TRUE, MONITOR_CENTER }
		},
		2,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_OCCLUDED },
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_OK }
		},
		1,
		{
			1533717,
		}
	},
	{
		// Scenario 7
		_T( "Occlusion of full-screen device by full-screen device" ),
		FALSE,
		2,
		{
			{ 0, 0, 1024, 768, TRUE, FALSE, MONITOR_CENTER },
			{ 0, 0, 1024, 768, TRUE, FALSE, MONITOR_CENTER }
		},
		1,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_OCCLUDED }
		},
		1,
		{
			1533717,
		}
	},
	{
		// Scenario 8
		_T( "Occlusion of full-screen device by minimization" ),
		TRUE,
		1,
		{
			{ 0, 0, 1024, 768, TRUE, FALSE, MONITOR_CENTER }
		},
		1,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_OCCLUDED }
		},
		1,
		{
			1533717,
		}
	},
	{
		// Scenario 9
		_T( "Occlusion of full-screen device by monitor standby" ),
		TRUE,
		1,
		{
			{ 0, 0, 1024, 768, TRUE, FALSE, MONITOR_CENTER }
		},
		1,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_OCCLUDED }
		},
		1,
		{
			1533717,
		}
	},
	{
		// Scenario 10
		_T( "Occlusion of full-screen device by monitor shutdown" ),
		TRUE,
		1,
		{
			{ 0, 0, 1024, 768, TRUE, FALSE, MONITOR_CENTER }
		},
		2,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_OCCLUDED },
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_OK }
		},
		1,
		{
			1533717,
		}
	},
	{
		// Scenario 11
		_T( "Occlusion of windowed device on center monitor by full-screen device on right monitor" ),
		TRUE,
		2,
		{
			{ 0, 0,  640, 480, TRUE, TRUE, MONITOR_CENTER },
			{ 1024, 0, 1024, 768, TRUE, FALSE, MONITOR_RIGHT }
		},
		1,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_OK }
		},
		1,
		{
			1533717,
		}
	},
	{
		// Scenario 12
		_T( "Occlusion of windowed device created on center adapter but displayed on right monitor by full-screen device on right monitor" ),
		TRUE,
		2,
		{
			{ 1200, 100,  640, 480, TRUE, TRUE, MONITOR_CENTER },
			{ 1024,   0, 1024, 768, TRUE, FALSE, MONITOR_RIGHT }
		},
		1,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_OCCLUDED }
		},
		1,
		{
			1533717,
		}
	},
	{
		// Scenario 13
		_T( "Occlusion of windowed device created on center adapter but displayed on left monitor by full-screen device on left monitor" ),
		TRUE,
		2,
		{
			{  -900, 100,  640, 480, TRUE, TRUE, MONITOR_CENTER },
			{ -1024,   0, 1024, 768, TRUE, FALSE, MONITOR_LEFT }
		},
		1,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_OCCLUDED }
		},
		1,
		{
			1533717,
		}
	},
	{
		// Scenario 14
		_T( "Occlusion of windowed device straddling center and right monitors by full-screen device on right monitor" ),
		TRUE,
		2,
		{
			{ 800, 80, 640, 480, TRUE, TRUE, MONITOR_CENTER },
			{ 1024, 0, 1024, 768, TRUE, FALSE, MONITOR_RIGHT }
		},
		2,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_OK },
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_MODE_CHANGED }
		},
		1,
		{
			1533717,
		}
	},
	{
		// Scenario 15
		_T( "Occlusion of windowed device straddling center and left monitors by full-screen device on left monitor" ),
		TRUE,
		2,
		{
			{ -400, 80, 640, 480, TRUE, TRUE, MONITOR_CENTER },
			{ -1024, 0, 1024, 768, TRUE, FALSE, MONITOR_LEFT }
		},
		2,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_OK },
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_MODE_CHANGED }
		},
		1,
		{
			1533717,
		}
	},

	//////////////////////////////////////////////////
	//
	// Mode change scenarios
	//
	//////////////////////////////////////////////////
	{
		// Scenario 16
		_T( "Mode change on windowed device" ),
		TRUE,
		1,
		{
			{ 0, 0, 640, 480, TRUE, TRUE, MONITOR_CENTER }
		},
		1,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_MODE_CHANGED }
		},
		2,
		{
			1533717,
			1248319
		}
	},
	{
		// Scenario 17
		_T( "Mode change on windowed device, validating window position" ),
		TRUE,
		1,
		{
			{ 400, 80, 640, 480, TRUE, TRUE, MONITOR_CENTER }
		},
		1,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_MODE_CHANGED }
		},
		3,
		{
			1533717,
			1248319,
			1296293
		}
	},
	{
		// Scenario 18
		_T( "Mode change on full-screen device" ),
		TRUE,
		1,
		{
			{ 0, 0, 1024, 768, TRUE, FALSE, MONITOR_CENTER }
		},
		1,
		{
			{ 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, S_PRESENT_MODE_CHANGED }
		},
		2,
		{
			1533717,
			1248319
		}
	},

	//////////////////////////////////////////////////
	//
	// Direct3D device creation scenarios
	//
	//////////////////////////////////////////////////
	{
		// Scenario 19
		_T( "Creation of windowed device while monitor is in standby" ),
		TRUE,
		1,
		{
			{ 0, 0, 640, 480, TRUE, TRUE, MONITOR_CENTER }
		},
		0,
		{
		},
		1,
		{
			1361666
		}
	},
	{
		// Scenario 20
		_T( "Creation of windowed device while monitor is shut down" ),
		TRUE,
		1,
		{
			{ 0, 0, 640, 480, TRUE, TRUE, MONITOR_CENTER }
		},
		0,
		{
		},
		1,
		{
			1361666
		}
	},
	{
		// Scenario 21
		_T( "Creation of full-screen device while monitor is in standby" ),
		TRUE,
		1,
		{
			{ 0, 0, 1024, 768, TRUE, FALSE, MONITOR_CENTER }
		},
		0,
		{
		},
		1,
		{
			1361666
		}
	},
	{
		// Scenario 22
		_T( "Creation of full-screen device while monitor is shut down" ),
		TRUE,
		1,
		{
			{ 0, 0, 1024, 768, TRUE, FALSE, MONITOR_CENTER }
		},
		0,
		{
		},
		1,
		{
			1361666
		}
	},

	//////////////////////////////////////////////////
	//
	// DirectDraw creation scenarios
	//
	//////////////////////////////////////////////////
	{
		// Scenario 23
		_T( "Creation of DirectDraw while monitor is in standby" ),
		TRUE,
		0,
		{
		},
		0,
		{
		},
		0,
		{
		}
	},
	{
		// Scenario 24
		_T( "Creation of DirectDraw while monitor is powered down" ),
		TRUE,
		0,
		{
		},
		0,
		{
		},
		0,
		{
		}
	},
};


const int g_cTestScenarios = sizeof( g_TestScenarios ) / sizeof( g_TestScenarios[ 0 ] );


// Singleton application object.
extern CMyTestApp g_Application;
