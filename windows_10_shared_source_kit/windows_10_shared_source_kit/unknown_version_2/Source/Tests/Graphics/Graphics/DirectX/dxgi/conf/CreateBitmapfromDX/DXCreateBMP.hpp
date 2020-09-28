//////////////////////////////////////////////////////////////////////
// File:  DXCreateBMP.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for GDI32.dll CreateBitMapfromDXSurf functional test.
//
// History:
// 13 May 2008  Mhusain  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include <D3D9CustomTestApp.hpp>
#include <CustomTest.hpp>
#include <D3D9CustomTest.hpp>
#include <D3D9ToGDIAdapterTopologyMap.hpp>

//
// Win32 headers
//

#include <setupapi.h>
#include <devguid.h>
#include <strsafe.h>
#include <windows.h>
#include <dwmapi.h>
#include <dwmapip.h>

/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

#define MODECHANGE 0
#define DWMONOFF   1
#define TDR        2
#define PNPSTOP    3
#define POWERDOWN  4
#define HYBRID     5

#define HW   0
#define SW   1
#define HWSW 2

#define THREAD_SLEEP_TIME           50

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
    UINT Event;
    UINT Acceleration;
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
    // Test Wide setup 
    TEST_RESULT Setup();
    
    // Test Wide setup 
    void Cleanup();

    // Initializes parameters.
    void InitTestParameters();

    // Per-test-case setup.
    TEST_RESULT SetupTestCase();

    // Runs a test case.
    TEST_RESULT ExecuteTestCase();

    // Per-test-case cleanup.
    void CleanupTestCase();

    static DWORD WINAPI     Helper_StaticThreadProc       // Static thread proc.
    (
        __in VOID * pContext
    );

    //
    // Test scenario methods
    //

protected:
    TEST_RESULT TC_EventsControlled_Pri0();

    TEST_RESULT TC_EventsRandom_Pri1();

    //
    // Helper methods
    //

protected:
    HRESULT LoadGDI();

    HRESULT UnloadGDI();

    HRESULT GDIRender
    (
        HDC hdc,
        HBITMAP hbm
    );

    HRESULT GDIRenderAgain
    (
        HDC hdc,
        HBITMAP hbm
    );
    
    HRESULT GDIRenderContinously
    (
        HDC hdc,
        HBITMAP hbm
    );

    void DrawClientAreaWithTextHW 
    (
        HDC hDC
    );

    void DrawClientAreaWithTextSW 
    (
        HDC hDC
    );

    void DrawClientAreaColorFillHW
    (
        HDC hDC
    );

    HRESULT CompareDCs( );      
    
    HRESULT TriggerEvent
    (
        IN UINT Event
    );

    void ChangeMode( );    

    HRESULT DWMDisableEnable( );    
    
    // Put System in Hibernate Mode
    HRESULT SuspendSystem
    (
        IN bool bHibernate
    );

    // Simulates a TDR event.
    HRESULT GenerateTDRViaKMTEscape( );
    
    // Simulates a PnP Stop.
    HRESULT GeneratePnPStop( );

    // Enumerates display devices.
    HRESULT EnumerateDisplayDevices();

    // Enables or disables all display devices.
    HRESULT EnableDisplayDevices
    (
        IN BOOL bEnable
    );

    // Enables or disables a single display device.
    HRESULT EnableDisplayDevice
    (
        IN HDEVINFO hDisplayDevicesInfo,
        IN SP_DEVINFO_DATA * pDisplayDeviceData,
        IN BOOL bEnable
    );
    
    // Boilerplate ShowWindow/UpdateWindow code.
    HRESULT DisplayWindow
    (
        IN UINT iWindow
    );

    HRESULT Helper_CreateThread( );

    DWORD Helper_ThreadProc( );

    void Helper_DisplayLastError
    (
        __in LPCTSTR    pszParentFunctionName,
        __in LPCTSTR    pszFunctionName
    );

    DWORD   VerifyAndLogTestCaseResult
    (
        HRESULT     hrExpectedCode,
        HRESULT     hrActualCode,
        TCHAR*      szOperation,
        HRESULT     hrAlternateAllowedCode = S_OK,
        BOOL		MustBeExact = TRUE
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
    HWND    m_phWindows[ 10 ];
    HDC     m_hDcWindow;

    HDC     m_hdc;
    HDC     m_hdcRef;
    HBITMAP m_hbm;
    HBITMAP m_hbmRef;

    HANDLE  m_hThread;
    HANDLE  m_hThreadExitEvent;

    // GDI32.dll handle and thunks.
    HMODULE m_hGDI32;
    PFND3DKMT_ESCAPE m_pD3DKMTEscape;
    PFND3DKMT_OPENADAPTERFROMHDC m_pD3DKMTOpenAdapterFromHDC;
    PFND3DKMT_OPENADAPTERFROMDEVICENAME m_pD3DKMTOpenAdapterFromDeviceName;
    PFND3DKMT_CLOSEADAPTER m_pD3DKMTCloseAdapter;

    BOOL m_bTDRTestMode;
    BOOL m_bPnPStop;

    HDEVINFO m_hDisplayDevicesInfo;
    int m_cDisplayDevices;
    SP_DEVINFO_DATA m_DisplayDeviceData[ 10 ];
    
    HDC     m_hScreenDC;
    RECT	m_rect;
    UINT	m_Width;
    UINT	m_Height;
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
    // Pri0 COntrolled Events Scenarios
    //
    //////////////////////////////////////////////////

    // HARDWARE Accelerated Rendering cases

    {    // Scenario 0
        _T( "HW Rendering with Event Mode Change" ), TRUE, MODECHANGE, HW
    },

    {    // Scenario 1
        _T( "HW Rendering with Event DWMON OFF" ), TRUE, DWMONOFF, HW
    },

    {    // Scenario 2
        _T( "HW Rendering with Event TDR" ), TRUE, TDR, HW
    },

    {    // Scenario 3
        _T( "HW Rendering with Event PNP Stop" ), TRUE, PNPSTOP, HW
    },

    {    // Scenario 4 
        _T( "HW Rendering with Event POWERDOWN" ), FALSE, POWERDOWN, HW
    },


    // SOFTWARE Rendering cases

    {    // Scenario 5
        _T( "SW Rendering with Event Mode Change" ), TRUE, MODECHANGE, SW
    },

    {    // Scenario 6
        _T( "SW Rendering with Event DWMON OFF" ), TRUE, DWMONOFF, SW
    },

    {    // Scenario 7
        _T( "SW Rendering with Event TDR" ), TRUE, TDR, SW
    },

    {    // Scenario 8
        _T( "SW Rendering with Event PNP Stop" ), TRUE, PNPSTOP, SW
    },

    {    // Scenario 9 
        _T( "SW Rendering with Event POWERDOWN" ), FALSE, POWERDOWN, SW
    },

    //////////////////////////////////////////////////
    //
    // Pri1 Random Event Scenarios
    //
    //////////////////////////////////////////////////

    {    // Scenario 10
        _T( "Rendering with Random Mode Change" ), TRUE, MODECHANGE, HW
    },

    {    // Scenario 11
        _T( "Rendering with Random DWMON OFF" ), TRUE, DWMONOFF, HW
    },

    {    // Scenario 12
        _T( "Rendering with Random PNP Stop" ), TRUE, PNPSTOP, HW
    },

    {    // Scenario 13
        _T( "Rendering with Random HYBRID" ), TRUE, HYBRID, HW
    },

    {    // Scenario 14 
        _T( "Rendering with Random .." ), FALSE, HYBRID, HW
    },

};


const int g_cTestScenarios = sizeof( g_TestScenarios ) / sizeof( g_TestScenarios[ 0 ] );


// Singleton application object.
extern CMyTestApp g_Application;
