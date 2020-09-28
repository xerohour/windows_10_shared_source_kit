//
// Tests.hpp
//
// Class declarations for D3D9 Mode Enumeration tests.
//
// Class declarations:  CTests
//

#pragma once

#include <d3d9.h>
#include "Common.hpp"
#include "DXGIWndClass.h"
#include "DXGISurfUtils.h"
#include "D3D10RefUtil.h"


// #defines converted to constants for easy debugging
static const UINT  c_nBACK_BUFFER_COUNT = 2;
static const UINT  c_nFORMAT_MAX = 4;
static const UINT  c_nMODE_MAX = 256;     // max modes per format for each adaptor

//

class CTests
:
	public CTest,
	public clsBasicWindowClass
{
	public:
		CTests();
		~CTests();


	private:
		// Framework combinatorial vars
		int             m_nPrimaryOutputNum;
		int             m_nModeIndex;
		int             m_nDeviceGrouping;
		bool            m_bUseFullscreen;

		// Variables
		int             m_nNumMonitors;
		BOOL            m_fGotOriginalDevMode[MONITORS_MAX];
		DISPLAY_DEVICE  m_originalDispDev[MONITORS_MAX];
		DEVMODE         m_originalDevMode[MONITORS_MAX];
		int             m_nModeRotation;
		IDirect3D9Ex   *m_pD3D9L;
		UINT            m_D3D9LDeviceCount;
		D3DCAPS9        m_d3dCaps9[MONITORS_MAX];
		UINT            m_nLargestModeCount;

		// When specifying an adapter group, use this array of identical modes
		// to pass to CreateDeviceEx and ResetEx.
		D3DDISPLAYMODEEX  m_groupModeToTest[MONITORS_MAX][MONITORS_MAX];

		// There shouldn't be more adapters or devices than monitors.
		IDirect3DDevice9Ex  *m_pD3D9LDevice[MONITORS_MAX];

		// Store modes for each attached monitor.
		D3DDISPLAYMODEEX  m_D3D9LModeToTest[MONITORS_MAX][c_nFORMAT_MAX * c_nMODE_MAX];

		HWND            m_hwndD3D9LDevWnd[MONITORS_MAX];

		D3DPRESENT_PARAMETERS   m_D3D9Lpp[MONITORS_MAX][MONITORS_MAX];


		// WGFramework overrides
		//
		bool  InitializeTestGroup();
		void  CleanupTestGroup();
		void  InitTestParameters();
		TEST_RESULT  Setup();
		TEST_RESULT  ExecuteTestCase();
		BOOL  ReleaseTestCaseObjects();
		BOOL  ReleaseD3D9LTestCaseObjects();
		void  Cleanup();

		// Internal Methods
		//

		// Enumerate all the display devices
		TEST_RESULT
		EnumerateDisplayDevices(
			__in    IDirect3D9Ex*   pTempD3D9L
		   );

		// For each display device and each display format, enumerate all the display modes 
		TEST_RESULT
		EnumerateAndStoreSelectedModes(
			__in    IDirect3D9Ex*   pTempD3D9L,
			__in    UINT            nDeviceNum
		   );

		// Before running a test case, create the D3D9 object
		TEST_RESULT  CreateInitialDXObjects();

		// Extract display device name and mode info
		TEST_RESULT
		GetMiscDeviceData(
			__in        int             nPrimaryOutputNum,
			__inout_ecount(nDevNameLen)  PCHAR  pszDevName,
			__in        UINT            nDevNameLen,
			__inout     DEVMODE&        devMode         // by ref
		   );

		// Set up the swap chain for the device
		TEST_RESULT
		SetupSwapChain(
			__in    UINT            nDeviceNum,
			__in    BOOL            fMasterAdapter
		   );
};

