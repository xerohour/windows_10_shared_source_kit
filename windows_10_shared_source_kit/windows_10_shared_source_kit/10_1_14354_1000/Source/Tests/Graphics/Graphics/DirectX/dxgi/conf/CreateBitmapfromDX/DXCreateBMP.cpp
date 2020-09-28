//-----------------------------------------------------
// File:  DXCreateBMP.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of DXcreateBitMap functional test. app.
//
// History:
// 13 May 2008  Mhusain  Created.
//-----------------------------------------------------

//-----------------------------------------------------
//
// Includes
//
//-----------------------------------------------------

//
// Project headers
//

#include <initguid.h>
#include "DXCreateBMP.hpp"

#include <CustomTest.inl>
#include <CustomTestApp.inl>
#include <D3D9CustomTest.inl>
#include <D3D9CustomTestApp.inl>

#if 0
#include "PowerManagement.h"
using namespace std;
using namespace PowerApi;
#endif

#define TEXTURE_SIZE 300

extern "C" {
    WINGDIAPI HBITMAP WINAPI CreateBitmapFromDxSurface(
                                                        __in HDC hdc, 
                                                        __in UINT uiWidth, 
                                                        __in UINT uiHeight, 
                                                        __in DWORD Format, 
                                                        __in HANDLE hDxSharedSurface
                                                      ); 
}

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp);
HRESULT CreateBMPFile(LPCTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC);

//----------------------------------------------------------------------------------
//
// Globals
//
//----------------------------------------------------------------------------------

// Singleton application object.
CMyTestApp g_Application;

//-----------------------------------------------------
// Function: IsWindows8orLater
//
// Purpose:  
//   Indicates whether the OS we are running on is at 
//   least Win8.
//
// Returns: bool : true or false 
//-----------------------------------------------------
bool IsWindows8orLater()
{
    OSVERSIONINFOEX osvi;
    ZeroMemory( &osvi, sizeof( osvi ) );
    osvi.dwOSVersionInfoSize = sizeof( osvi );

    GetVersionEx( reinterpret_cast< OSVERSIONINFO* >( &osvi ) );

    return (osvi.dwMajorVersion > 6 || (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion >= 2));
}

//-----------------------------------------------------
//
// Implementation
//
//-----------------------------------------------------

//-----------------------------------------------------
// Function:  CMyTestApp
//
// Purpose:
// Constructor.
//-----------------------------------------------------
CMyTestApp::CMyTestApp()
{
};

//-----------------------------------------------------
// Function:  GetTestType
//
// Purpose:
// Returns the default type for all test groups in the
// app.
//-----------------------------------------------------
TESTTYPE CMyTestApp::GetTestType() const
{
    return TESTTYPE_CONF;
};

//-----------------------------------------------------
// Function:  InitTestGroups
//
// Purpose:
// Registers test groups.
//-----------------------------------------------------
bool CMyTestApp::InitTestGroups()
{
    // Register test class with the group manager.
    if(!ADD_TEST(_T("DXCreateBMP"), CMyTest))
        return false;

    return true;
};

//-----------------------------------------------------
// Function:  CMyTest
//
// Purpose:
// Constructor.
//-----------------------------------------------------
CMyTest::CMyTest()
:
    CD3D9CustomTest< CMyTestApp >::CD3D9CustomTest( &g_Application ),
    m_iCurrentScenario( 0 ),
    m_hGDI32( NULL ),
    m_pD3DKMTEscape( NULL ),
    m_pD3DKMTOpenAdapterFromHDC( NULL ),
    m_pD3DKMTOpenAdapterFromDeviceName( NULL ),
    m_pD3DKMTCloseAdapter( NULL ),
    m_bPnPStop( FALSE ),
    m_bTDRTestMode( FALSE ),
    m_cDisplayDevices( 0 ),
    m_hDisplayDevicesInfo( NULL )
{
    ZeroMemory( m_phWindows, sizeof( m_phWindows ) );
    ZeroMemory( m_DisplayDeviceData, sizeof( m_DisplayDeviceData ) );

    for
    (
        UINT iCurrentDevice = 0;
        iCurrentDevice < ARRAYSIZE( m_DisplayDeviceData );
        ++iCurrentDevice
    )
    {
        m_DisplayDeviceData[ iCurrentDevice ].cbSize = sizeof( m_DisplayDeviceData[ 0 ] );
    };

};

//-----------------------------------------------------
// Function:  Setup
//
// Purpose:
// Performs app-wide initialization.
//-----------------------------------------------------

TEST_RESULT CMyTest::Setup()
{
    //
    // Load GDI32.dll.
    //

    HRESULT hLoadGDIResult = LoadGDI();

    if ( FAILED( hLoadGDIResult ) )
    {
        WriteToLog( _T( "Setup:  LoadGDI failed." ) );

        return RESULT_ABORT;
    };


    //
    // Enable the debug privilege.
    //

    HRESULT hEnableDebugPrivilegeResult = EnablePrivilege( SE_DEBUG_NAME );

    if ( FAILED( hEnableDebugPrivilegeResult ) )
    {
        WriteToLog
        (
            _T( "Setup:  EnablePrivilege returned %u (0x%x)." ),
            hEnableDebugPrivilegeResult,
            hEnableDebugPrivilegeResult
        );
    };


    //
    // Open (or create) the GraphicsDrivers registry key.
    //

    m_bTDRTestMode = FALSE;

    HKEY hKey = NULL;
    LONG OpenKeyResult = RegCreateKeyEx
    (
        HKEY_LOCAL_MACHINE,
        _T( "System\\CurrentControlSet\\Control\\GraphicsDrivers" ),
        0,
        NULL,
        0,
        KEY_READ | KEY_WRITE,
        NULL,
        &hKey,
        NULL
    );

    if ( OpenKeyResult != ERROR_SUCCESS )
    {
        WriteToLog
        (
            _T( "Setup:  RegCreateKeyEx on GraphicsDrivers failed with error %u." ),
            OpenKeyResult
        );

        return RESULT_ABORT;
    };


    //
    // Determine whether TDR test mode is set in the registry.
    //

    DWORD ValueType = REG_DWORD;
    DWORD ValueData = 0;
    DWORD ValueSize = sizeof( ValueData );
    LONG QueryValueResult = RegQueryValueEx
    (
        hKey,
        "TdrTestMode",
        NULL,
        &ValueType,
        (BYTE *) &ValueData,
        &ValueSize
    );

    if
    (
        QueryValueResult == ERROR_SUCCESS &&
        ValueData != 0
    )
    {
        WriteToLog( _T( "Setup:  TDR test mode already set." ) );

        m_bTDRTestMode = TRUE;
    }
    else
    {
        WriteToLog( _T( "Setup:  Setting TDR test mode." ) );

        m_bTDRTestMode = FALSE;

        ValueType = REG_DWORD;
        ValueData = 1;
        ValueSize = sizeof( ValueData );
        LONG SetValueResult = RegSetValueEx
        (
            hKey,
            _T( "TdrTestMode" ),
            0,
            ValueType,
            (BYTE *) &ValueData,
            ValueSize
        );

        if ( SetValueResult != ERROR_SUCCESS )
        {
            WriteToLog
            (
                _T( "Setup:  Failed to set TDR test mode." )
            );

            RegCloseKey( hKey );
            hKey = NULL;

            return RESULT_ABORT;
        };
    };

    //
    // Determine whether we're running under WOW64.
    //

    BOOL bIsWOW64Process = FALSE;
    BOOL bCheckWOW64ProcessResult = IsWow64Process
    (
        GetCurrentProcess(),
        &bIsWOW64Process
    );

    if ( bCheckWOW64ProcessResult != TRUE )
    {
        DWORD CheckWOW64ProcessError = GetLastError();

        WriteToLog
        (
            _T( "Setup:  IsWow64Process failed with error %u." ),
            CheckWOW64ProcessError
        );
    };

    if ( bIsWOW64Process == TRUE )
    {
        WriteToLog
        (
            _T( "Setup:  Can't install 64-bit drivers from a 32-bit process." )
        );

        //return RESULT_SKIP;
    };

    WriteToLog( _T( "Setup:  IsWOW64Process returned FALSE." ) );


    //
    // Enumerate display hardware.
    //

    HRESULT hEnumerateDisplayHardwareResult = EnumerateDisplayDevices();

    if ( FAILED( hEnumerateDisplayHardwareResult ) )
    {
        WriteToLog
        (
            _T( "Setup:  EnumerateDisplayDevices failed with error 0x%x." ),
            hEnumerateDisplayHardwareResult
        );

        return RESULT_ABORT;
    };

    return RESULT_PASS;
};

//-----------------------------------------------------
// Function: Cleanup
//
// Purpose:
// Performs app-wide CleanUp.
//-----------------------------------------------------
void CMyTest::Cleanup()
{
    //
    // Clear our TDR test mode registry value if needed.
    //

    if ( m_bTDRTestMode != TRUE )
    {
        HKEY hKey = NULL;
        LONG OpenKeyResult = RegOpenKeyEx
        (
            HKEY_LOCAL_MACHINE,
            _T( "System\\CurrentControlSet\\Control\\GraphicsDrivers" ),
            0,
            KEY_READ | KEY_WRITE,
            &hKey
        );

        if
        (
            OpenKeyResult != ERROR_SUCCESS ||
            hKey == NULL
        )
        {
            WriteToLog
            (
                _T( "Cleanup:  Could not open GraphicsDrivers registry key.  RegOpenKeyEx returned error %u." ),
                OpenKeyResult
            );
        }
        else
        {
            LONG DeleteValueResult = RegDeleteValue
            (
                hKey,
                _T( "TdrTestMode" )
            );

            if
            (
                DeleteValueResult != ERROR_SUCCESS &&
                DeleteValueResult != ERROR_FILE_NOT_FOUND
            )
            {
                WriteToLog
                (
                    _T( "Cleanup:  Failed to clear TDR test mode." )
                );
            };

            RegCloseKey( hKey );
            hKey = NULL;
        };
    };

    //
    // Clean up the display device information.
    //

    if ( m_hDisplayDevicesInfo != INVALID_HANDLE_VALUE )
    {
        SetupDiDestroyDeviceInfoList( m_hDisplayDevicesInfo );
        m_hDisplayDevicesInfo = INVALID_HANDLE_VALUE;
    };
}

//-----------------------------------------------------
// Function:  InitTestParameters
//
// Purpose:
// Initializes the parameter axes along which the
// framework should iterate.
//-----------------------------------------------------
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


//-----------------------------------------------------
// Function:  SetupTestCase
//
// Purpose:
// Per-test-case initialization.
//-----------------------------------------------------
TEST_RESULT CMyTest::SetupTestCase()
{
    TEST_RESULT SetupResult = RESULT_ABORT;
    HRESULT     hr = S_OK;

    //
    // Skip inactive scenarios.
    //

    if ( g_TestScenarios[ m_iCurrentScenario ].bEnabled != TRUE )
    {
        WriteToLog
        (
            _T( "Scenario %i (%s) disabled." ),
            m_iCurrentScenario,
            g_TestScenarios[ m_iCurrentScenario ].Name
        );

        SetupResult = RESULT_SKIP;
        goto Cleanup;
    };

    // Log which scenario we're running.
    WriteToLog(_T( "**************%s*****************." ), 
        g_TestScenarios[ m_iCurrentScenario ].Name);

    // Run Only On WDDM version 1.1 
    //TODO:
    
    // Run Only with DWM ON
    BOOL bDWMEnabled = TRUE;
    DwmIsCompositionEnabled(&bDWMEnabled);    
    if( !bDWMEnabled )
    {           //turn it oN
        HRESULT hr = DwmEnableComposition(DWM_EC_ENABLECOMPOSITION );
        if( FAILED(hr) )
        {
            WriteToLog(_T( " DWM couldnt be enabled " ) );
            goto Cleanup;
        } 

        DwmIsCompositionEnabled(&bDWMEnabled);   
        if( !bDWMEnabled )
            goto Cleanup;        
    }    

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
    m_WindowClass.lpszClassName = _T( "DXGIinterOp" );


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
                _T( "SetupTestCase:  RegisterClassEx failed with error %u.\n" ),
                RegisterClassError
            );

            goto Cleanup;
        };
    };

    //
    // Create our window.
    //

    m_phWindows[ 0 ] = CreateWindowExA
    (
        0 | WS_EX_APPWINDOW,
        "DXGIinterOp",
        "Overlapped Window",
        WS_OVERLAPPED | WS_VISIBLE,
        0,
        0,
        TEXTURE_SIZE,
        TEXTURE_SIZE,
        NULL,
        NULL,
        m_WindowClass.hInstance,
        (PVOID) this
    );

    if ( m_phWindows[ 0 ] == NULL )
    {
        WriteToLog
        (
            _T( "SetupTestCase:  Could not create window ." )		    
        );

        goto Cleanup;
    }   
    
    DisplayWindow(0);

    // Obtain the window's client rectangle 
    GetClientRect(m_phWindows[ 0 ], &m_rect); 
    m_Width = m_rect.right - m_rect.left;
    m_Height = m_rect.bottom - m_rect.top;

    m_hDcWindow = ::GetDC( m_phWindows[ 0 ] );
    if( m_hDcWindow == NULL )
    {
        return RESULT_SKIP;
    }

    m_hdc = CreateCompatibleDC( m_hDcWindow );
    if (m_hdc == NULL)
    {
        return RESULT_SKIP;
    }

    m_hdcRef = CreateCompatibleDC( m_hDcWindow );
    if (m_hdcRef == NULL)
    {
        return RESULT_SKIP;
    }

    m_hScreenDC = CreateCompatibleDC( m_hDcWindow );
    if (m_hScreenDC == NULL)
    {
        return RESULT_SKIP;
    }

    m_hbmRef = CreateCompatibleBitmap(m_hDcWindow, TEXTURE_SIZE, TEXTURE_SIZE);
    if (m_hbmRef == NULL)
    {
        return RESULT_FAIL;
    }

#if 0
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader); 
    bmi.bmiHeader.biWidth  = m_Width; 
    bmi.bmiHeader.biHeight = -(LONG)m_Height; //top-down format
    bmi.bmiHeader.biPlanes = 1; 
    bmi.bmiHeader.biBitCount = 32; 
    bmi.bmiHeader.biCompression = BI_RGB; 
    bmi.bmiHeader.biSizeImage = 0; 
    bmi.bmiHeader.biXPelsPerMeter = 96; 
    bmi.bmiHeader.biYPelsPerMeter = 96; 
    bmi.bmiHeader.biClrUsed = 0; 
    bmi.bmiHeader.biClrImportant = 0; 

    VOID* pScreenBits = NULL;
    m_hbm = CreateDIBSection(m_hDcWindow, &bmi, DIB_RGB_COLORS, &pScreenBits, NULL, 0);
    if (m_hbm == NULL || pScreenBits == NULL)
    {
        OutputDebugString(_T("hScreenBitmap is NULL\n"));
        goto Cleanup;
    }
    //get the contents on the screen to ScreenBitmap
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(m_hScreenDC, m_hbm);
    m_hdc = m_hDcWindow;
#endif

    m_hbm = CreateBitmapFromDxSurface( m_hDcWindow, 
                                         TEXTURE_SIZE, 
                                         TEXTURE_SIZE, 
                                         DXGI_FORMAT_B8G8R8A8_UNORM,
                                         NULL );
    if (m_hbm == NULL)
    {
        return RESULT_FAIL;
    }        

    SelectObject( m_hdc, m_hbm );
    SelectObject( m_hdcRef, m_hbmRef );

    SetupResult = RESULT_PASS;

Cleanup:
    return SetupResult;
};


//-----------------------------------------------------
// Function:  ExecuteTestCase
//
// Purpose:
// Executes a single test case.
//-----------------------------------------------------
TEST_RESULT CMyTest::ExecuteTestCase()
{
    // Validate the scenario number.
    if ( m_iCurrentScenario < 0 )
    {
        WriteToLog(	_T( "Scenario not implemented." ));
        return RESULT_SKIP;
    };

    if	(m_iCurrentScenario >= ARRAYSIZE( g_TestScenarios ))
    {
        WriteToLog(	_T( "Scenario not implemented." ));
        return RESULT_SKIP;
    };
    
   
    // Skip scenarios that are marked as inactive.
    if ( g_TestScenarios[ m_iCurrentScenario ].bEnabled == FALSE )
        return RESULT_SKIP;	
    
    // Skip DWM Off->On scenarios on Win8 and later OSes.  (Disabling of DWM only supported on pre-win8 OSes)
    if ( g_TestScenarios[ m_iCurrentScenario ].Event == DWMONOFF && IsWindows8orLater())
    {
        WriteToLog(	_T( "This test case cycles DWM off and on which is not supported on Windows 8 and later Oses.  Skipping test case." ));
        return RESULT_SKIP;
    }

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
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        {
            return TC_EventsControlled_Pri0(  );
        }break;
        
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        {
            return TC_EventsRandom_Pri1(  );
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


//-----------------------------------------------------
// Function:  CleanupTestCase
//
// Purpose:
// Per-test-case cleanup.
//-----------------------------------------------------
void CMyTest::CleanupTestCase()
{
    //
    // Free our devices and windows.
    //
    
    if ( m_phWindows[ 0 ] != NULL )
    {
        DestroyWindow( m_phWindows[ 0 ] );
        m_phWindows[ 0 ] = NULL;
    };


    ReleaseDC(m_phWindows[ 0 ] , m_hdc);
    ReleaseDC(m_phWindows[ 0 ] , m_hdcRef);
    ReleaseDC(m_phWindows[ 0 ], m_hScreenDC);
    DeleteObject(m_hbm);
    DeleteObject(m_hbmRef);

    //
    // Re-enable devices.
    //
    if(m_bPnPStop)
    {
        EnableDisplayDevices( TRUE );
        m_bPnPStop = FALSE;
    }
};


//-----------------------------------------------------
// Function:  TC_EventsControlled_Pri0
//
// Purpose:
// Tests the scenario of a System Event while a GDI
// rendering.
//
// Called by:
// ExecuteTestCase
//-----------------------------------------------------
TEST_RESULT CMyTest::TC_EventsControlled_Pri0
(	
)
{
    TEST_RESULT TestResult = RESULT_FAIL;
    HRESULT hr = S_OK;

    hr = GDIRender(m_hdc, m_hbm);
    if( FAILED (VerifyAndLogTestCaseResult(S_OK, hr, _T("GDIRender, Native"))) )
        goto Cleanup;

    hr = GDIRender(m_hdcRef, m_hbmRef);
    if( FAILED (VerifyAndLogTestCaseResult(S_OK, hr, _T("GDIRender, Ref"))) )
        goto Cleanup;

    hr = CompareDCs( );
    if( FAILED (VerifyAndLogTestCaseResult(S_OK, hr, _T("Compare the Initial Rendering"))) )
        goto Cleanup;

    hr = TriggerEvent(g_TestScenarios[ m_iCurrentScenario ].Event);
    if( FAILED (VerifyAndLogTestCaseResult(S_OK, hr, _T("TriggerEvent"))) )
        goto Cleanup;   

    hr = GDIRenderAgain(m_hdc, m_hbm);
    if( FAILED (VerifyAndLogTestCaseResult(S_OK, hr, _T("GDIRenderAgain, Native"))) )
        goto Cleanup;

    hr = GDIRenderAgain(m_hdcRef, m_hbmRef);
    if( FAILED (VerifyAndLogTestCaseResult(S_OK, hr, _T("GDIRenderAgain, Ref"))) )
        goto Cleanup;

    hr = CompareDCs( );
    if( FAILED (VerifyAndLogTestCaseResult(S_OK, hr, _T("Final Comparison"))) )
        goto Cleanup;
    
    //
    // If we made it this far, then we're good to go.
    //
    TestResult = RESULT_PASS;

Cleanup:

    //
    // Pump messages resulting from the display mode change.
    //

    WaitAndPumpMessages( 1000 );


    return TestResult;
};

//-----------------------------------------------------
// Function:  TC_EventsControlled_Pri0
//
// Purpose:
// Tests the scenario of a System Event while a GDI
// rendering.
//
// Called by:
// ExecuteTestCase
//-----------------------------------------------------
TEST_RESULT CMyTest::TC_EventsRandom_Pri1
(	
)
{
    TEST_RESULT TestResult = RESULT_FAIL;
    HRESULT hr = S_OK;

    hr = GDIRenderContinously(m_hdcRef, m_hbmRef);
    if( FAILED (VerifyAndLogTestCaseResult(S_OK, hr, _T("GDIRender, Ref"))) )
        goto Cleanup;
    //Dump the DC
    PBITMAPINFO bmInfo = CreateBitmapInfoStruct(m_hbmRef);    
    TCHAR pszFile[100];
    StringCchPrintf(pszFile, 100, TEXT("Ref_Pri1TC%d.bmp"),m_iCurrentScenario);
    CreateBMPFile(pszFile, bmInfo, m_hbmRef, m_hdcRef);

    // Create a thread to Trigger System Events During GDI Rendering
    hr = Helper_CreateThread();
    if( FAILED (VerifyAndLogTestCaseResult(S_OK, hr, _T("Helper_CreateThread()"))) )
        goto Cleanup;

    hr = GDIRenderContinously(m_hdc, m_hbm);
    if( FAILED (VerifyAndLogTestCaseResult(S_OK, hr, _T("GDIRender, Native"))) )
        goto Cleanup;
    
    if (!SetEvent(m_hThreadExitEvent))
    {
        Helper_DisplayLastError(_T("Cleanup()"), _T("SetEvent()"));
    }

    //Dump the DC
    bmInfo = CreateBitmapInfoStruct(m_hbm);    
    StringCchPrintf(pszFile, 100, TEXT("Native_Pri1TC%d.bmp"),m_iCurrentScenario);
    CreateBMPFile(pszFile, bmInfo, m_hbm, m_hdc);

    hr = CompareDCs( );
    if( FAILED (VerifyAndLogTestCaseResult(S_OK, hr, _T("Interim Comparison"))) )
        goto Cleanup;

    hr = GDIRenderAgain(m_hdc, m_hbm);
    if( FAILED (VerifyAndLogTestCaseResult(S_OK, hr, _T("GDIRenderAgain"))) )
        goto Cleanup;

    hr = GDIRenderAgain(m_hdcRef, m_hbmRef);
    if( FAILED (VerifyAndLogTestCaseResult(S_OK, hr, _T("GDIRenderAgain"))) )
        goto Cleanup;

    hr = CompareDCs( );
    if( FAILED (VerifyAndLogTestCaseResult(S_OK, hr, _T("Final Comparison"))) )
        goto Cleanup;
    
    //
    // If we made it this far, then we're good to go.
    //
    TestResult = RESULT_PASS;

Cleanup:

    //
    // Pump messages resulting from the display mode change.
    //

    WaitAndPumpMessages( 1000 );


    return TestResult;
};

//-----------------------------------------------------
// Function:  GDIRender()
//
// Purpose:
// Helper method to render an empty scene on the
// specified device.  
//-----------------------------------------------------
HRESULT CMyTest::GDIRender
(
    HDC hdc, 
    HBITMAP hbm
)
{
    static int c = 0 ; 
    c++;

    if(g_TestScenarios[ m_iCurrentScenario ].Acceleration == HW)
    {
        //DrawClientAreaWithTextHW(hdc);
        DrawClientAreaColorFillHW(hdc);
    }
    else 
    {
        DrawClientAreaWithTextSW(hdc);
    }

    //Dump the DC
    PBITMAPINFO bmInfo = CreateBitmapInfoStruct(hbm);    
    TCHAR pszFile[100];
    StringCchPrintf(pszFile, 100, TEXT("GDIRender_TC%d.%d.bmp"),m_iCurrentScenario, c);
    CreateBMPFile(pszFile, bmInfo, hbm, hdc);

    return S_OK;
};

//-----------------------------------------------------
// Function:  GDIRender()
//
// Purpose:
// Helper method to render an empty scene on the
// specified device.  
//-----------------------------------------------------
HRESULT CMyTest::GDIRenderContinously
(
    HDC hdc, 
    HBITMAP hbm
)
{
    for (UINT i = 0 ; i < 50; i++)
    {
        DrawClientAreaWithTextHW(hdc);     
        Sleep(100);
        DrawClientAreaColorFillHW(hdc);
        Sleep(100);
        DrawClientAreaWithTextSW(hdc);
        Sleep(100);
    }   

    return S_OK;
};

//-----------------------------------------------------
// Function:  DrawClientAreaWithTextHW()
//
// Purpose: HW accelerated GDI rendering, 
//			TextOut Cleartype
//-----------------------------------------------------
void CMyTest::DrawClientAreaWithTextHW 
(
    HDC hDC
)
{
    // Obtain the window's client rectangle 
    RECT r;
    GetClientRect(m_phWindows[ 0 ], &r); 
    UINT Width = r.right - r.left;
    UINT Height = r.bottom - r.top;
 
    HBRUSH hBrush = CreateSolidBrush(0x00805060);
    if (hBrush == NULL)
    {
        OutputDebugString(_T("hBrush is NULL\n"));
        goto EXIT;
    }

    HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
    // Fill the background
    if (!BitBlt(hDC, r.left, r.top, Width, Height, NULL, 0, 0, PATCOPY))
    {
        OutputDebugString(_T("BitBlt failed\n"));
    }
    SelectObject(hDC, hOldBrush);

    TEXTMETRIC tm;
    GetTextMetrics(hDC, &tm);
    UINT NumLines = Height / tm.tmHeight;
    UINT i,n,y;

#if 1
    HFONT fAntialias = CreateFont(-20, 0, 0, 0, FW_NORMAL, 0, 0, 0,
                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                  ANTIALIASED_QUALITY, DEFAULT_PITCH, TEXT("Tahoma"));

    //TextOut with Transparent mode with anti-alias font
    {
      SetTextColor(hDC, RGB(0,255,0));
      SetBkColor(hDC, RGB(0,0,0));
      SetBkMode(hDC, TRANSPARENT);
      SelectObject(hDC, fAntialias);
      {
          //TextOut with Transparent mode and antialiased font
          {
              y = r.top;
              for (n=0; n < NumLines/3; n++)
              {
                  TCHAR text[ ] = _T("Using DeviceBitmaps 0123456789"); 
                  TextOut(hDC, r.left, y, text, ARRAYSIZE(text) - 1);
                  y += 3*tm.tmHeight;
              }
          }
      }
    }
#endif

#if 0
    HFONT fClearType = CreateFont(-20, 0, 0, 0, FW_NORMAL, 0, 0, 0,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, TEXT("Tahoma"));

    //TextOut with Transparent and OPAQUE modes with ClearType font
    {
      SetTextColor(hDC, RGB(0,255,0));
      SetBkColor(hDC, RGB(0,0,0));
      SetBkMode(hDC, TRANSPARENT);
      SelectObject(hDC, fClearType);
      {
          //TextOut with Transparent mode and ClearType font
          {
              y = r.top;
              for (n=0; n < NumLines/3; n++)
              {
                  TCHAR text[ ] = _T("Using DeviceBitmaps 0123456789"); 
                  TextOut(hDC, r.left, y, text, ARRAYSIZE(text) - 1);
                  y += 3*tm.tmHeight;
              }
          }
      }
    }
#endif

EXIT:
    if (fAntialias)
      DeleteObject(fAntialias);
    //if (fClearType) 
      //DeleteObject(fClearType);

    DeleteObject(hBrush);
}

//-----------------------------------------------------
// Function: DrawClientAreaWithTextSW()
//
// Purpose:	 SW Only GDI rendering, Simple TextOut.
//-----------------------------------------------------
void CMyTest::DrawClientAreaWithTextSW 
(
    HDC hDC
)
{ 
    HBRUSH hBrush = CreateSolidBrush(RGB(25,50,60));
    if (hBrush == NULL)
    {
        OutputDebugString(_T("hBrush is NULL\n"));
    }

    HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
    // Fill the background
    if (!BitBlt(hDC, m_rect.left, m_rect.top, m_Width, m_Height, NULL, 0, 0, PATCOPY))
    {
        OutputDebugString(_T("BitBlt failed\n"));
    }
    SelectObject(hDC, hOldBrush);

    TEXTMETRIC tm;
    GetTextMetrics(hDC, &tm);
    UINT NumLines = m_Height / tm.tmHeight;
    UINT n,y;

    {
      SetTextColor(hDC, RGB(255,0,0));
      SetBkColor(hDC, RGB(0,0,0));
      SetBkMode(hDC, TRANSPARENT);	  
      HFONT hFont, hOldFont; 

      // Retrieve a handle to the variable stock font. 
      hFont = (HFONT)GetStockObject(ANSI_VAR_FONT); 
      
      // Select the variable stock font into the specified device context.
      if (hOldFont = (HFONT)SelectObject(hDC, hFont)) 
      {
          y = m_rect.top;
          for (n=0; n < NumLines/3; n++)
          {
              TCHAR text[ ] = _T("Sample ANSI_VAR_FONT text"); 
              TextOut(hDC, m_rect.left, y, text, ARRAYSIZE(text) - 1);
              y += 3*tm.tmHeight;
          }

        // Restore the original font.	
        SelectObject(hDC, hOldFont);
      }  
    }

    //Draw Lines
    {
        int x = 50 , y = 150;
        for (int i = 0 ; i < 20; i++ )
        {
            MoveToEx(hDC, (int) x - 10, (int) y, (LPPOINT) NULL); 
            LineTo(hDC, (int) x + 10, (int) y); 
            MoveToEx(hDC, (int) x, (int) y - 10, (LPPOINT) NULL); 
            LineTo(hDC, (int) x, (int) y + 10); 
            
            x+=5;
            y+=5;
        }
    }

    DeleteObject(hBrush);
}

//-----------------------------------------------------
// Function: DrawClientAreaWithTextSW()
//
// Purpose:	 HW Accelerated GDI Colorfill rendering.
//-----------------------------------------------------
void CMyTest::DrawClientAreaColorFillHW 
(
    HDC hDC
)
{
    UINT Blocks = TEXTURE_SIZE/10 ;
    
    HBRUSH hBrush1 = CreateSolidBrush(RGB(0, 255, 0));
    if (hBrush1 == NULL)
    {
        OutputDebugString(_T("hBrush1 is NULL\n"));
    }

    HBRUSH hBrush2 = CreateSolidBrush(RGB(255, 0, 0));
    if (hBrush2 == NULL)
    {
        OutputDebugString(_T("hBrush2 is NULL\n"));
    }

    for (UINT i = 0 ; i < Blocks ; i++) 
    {

        HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush1);            
        BitBlt(hDC, 
               Blocks*i,  // x-coord of destination upper-left corner
               0,  // y-coord of destination upper-left corner
               Blocks / 2,  // width of destination rectangle
               TEXTURE_SIZE, // height of destination rectangle
               NULL,  // handle to source DC
               0,   // x-coordinate of source upper-left corner
               0,   // y-coordinate of source upper-left corner
               PATCOPY);
        SelectObject(hDC, hOldBrush);

        HBRUSH hOldBrush2 = (HBRUSH)SelectObject(hDC, hBrush2);
        BitBlt(hDC, 
               (Blocks/2)+(Blocks*i),  // x-coord of destination upper-left corner
               0,  // y-coord of destination upper-left corner
               Blocks / 2,  // width of destination rectangle
               TEXTURE_SIZE, // height of destination rectangle
               NULL,  // handle to source DC
               0,   // x-coordinate of source upper-left corner
               0,   // y-coordinate of source upper-left corner
               PATCOPY);
        SelectObject(hDC, hOldBrush2);
    }

    DeleteObject(hBrush1);
    DeleteObject(hBrush2);
}

//-----------------------------------------------------
// Function:  GDIRenderAgain
//
// Purpose:
// Helper method to render an empty scene on the
// specified device.  
//-----------------------------------------------------
HRESULT CMyTest::GDIRenderAgain
(
    HDC hDC,
    HBITMAP hbm
)
{    
    static int c2 = 0 ; 
    c2++;
    
    // half white HW operation
    BitBlt(hDC, 
           TEXTURE_SIZE / 2,  // x-coord of destination upper-left corner
           0,  // y-coord of destination upper-left corner
           TEXTURE_SIZE / 2,  // width of destination rectangle
           TEXTURE_SIZE, // height of destination rectangle
           NULL,  // handle to source DC
           0,   // x-coordinate of source upper-left corner
           0,   // y-coordinate of source upper-left corner
           WHITENESS);

    TEXTMETRIC tm;
    GetTextMetrics(hDC, &tm);
    UINT NumLines = m_Height / tm.tmHeight;
    UINT n,y;

    // Text Software Operation
    {
        SetTextColor(hDC, RGB(255,255,0));
        SetBkColor(hDC, RGB(0,0,0));
        SetBkMode(hDC, TRANSPARENT);	  
        HFONT hFont, hOldFont; 

        // Retrieve a handle to the variable stock font. 
        hFont = (HFONT)GetStockObject(ANSI_VAR_FONT); 

        // Select the variable stock font into the specified device context.
        if (hOldFont = (HFONT)SelectObject(hDC, hFont)) 
        {
          y = m_rect.top;
          for (n=0; n < NumLines/4; n++)
          {
              TCHAR text[ ] = _T("Sample ANSI_VAR_FONT text"); 
              TextOut(hDC, m_rect.left, y, text, ARRAYSIZE(text) - 1);
              y += 2*tm.tmHeight;
          }
          // Restore the original font.	
          SelectObject(hDC, hOldFont);
        }  
    }

    //Draw Lines
    {
        int x = 10 , y = 100;
        for (int i = 0 ; i < 20; i++ )
        {
            MoveToEx(hDC, (int) x - 10, (int) y, (LPPOINT) NULL); 
            LineTo(hDC, (int) x + 10, (int) y); 
            MoveToEx(hDC, (int) x, (int) y - 10, (LPPOINT) NULL); 
            LineTo(hDC, (int) x, (int) y + 10); 
            
            x+=5;
            y+=5;
        }
    }

    //Dump the screen captured
    PBITMAPINFO bmInfo = CreateBitmapInfoStruct(hbm);    
    TCHAR pszFile[100];
    StringCchPrintf(pszFile, 100, TEXT("GDIRender2_TC%d.%d.bmp"),m_iCurrentScenario, c2);
    CreateBMPFile(pszFile, bmInfo, hbm, hDC);

    return S_OK;
};

//-----------------------------------------------------
// Function:  CompareDCs()
//
// Purpose: Comparing two Bitmaps 
//
//-----------------------------------------------------
HRESULT CMyTest::CompareDCs
(	
)
{
        for (UINT i = 0; i < TEXTURE_SIZE; i++) //(UINT)m_rect.right
        {
            for (UINT j = 0; j < TEXTURE_SIZE; j++) //(UINT)m_rect.bottom
            {
                COLORREF Color = GetPixel(m_hdc, i, j);
                COLORREF RefColor = GetPixel(m_hdcRef, i, j);

                if (Color == CLR_INVALID)
                {
                    WriteToLog( _T( "ERR: CLR_INVALID Native=0x%x, Ref=0x%x." ), Color, RefColor);
                    WriteToLog( _T( "Pixel Location x=%d, y=%d." ), Color, RefColor);
                    return RESULT_FAIL;
                }
                
                if (Color != RefColor)
                {
                    WriteToLog( _T( "ERR:Color Mismatch Native=0x%x, Ref=0x%x." ), Color, RefColor);
                    WriteToLog( _T( "Pixel Location x=%d, y=%d." ), Color, RefColor);
                    return RESULT_FAIL;
                }               
            }
        }

    return S_OK;
};

//-----------------------------------------------------
// Function:  TriggerEvent
//
// Purpose:
// Helper routine to create a device in the specified
// device slot.
//-----------------------------------------------------
HRESULT CMyTest::TriggerEvent
(
    UINT Event
)
{
    HRESULT hr = S_OK;

    switch(Event)
    {

        case MODECHANGE: 
            {
                ChangeMode();
            }
            break;

        case DWMONOFF: 
            {
                hr = DWMDisableEnable();
            }
            break ;

        case TDR: 
            {
                hr = GenerateTDRViaKMTEscape();
            }
            break;

        case PNPSTOP: 
            {
                hr = GeneratePnPStop();
            }
            break ;

        case POWERDOWN: 
            {
                hr = SuspendSystem(TRUE);
            }
            break ;
    }

    return hr;

};

//-----------------------------------------------------
// Function: GeneratePnPStop
//
// Purpose: Generate a plug-and-play stop, and then Start
//
//-----------------------------------------------------
HRESULT
CMyTest::GeneratePnPStop()
{
    WriteToLog( _T( "Attempting to stop display devices." ) );

    HRESULT hGeneratePnPStopResult = EnableDisplayDevices( FALSE );

    if ( hGeneratePnPStopResult == (HRESULT) ERROR_IN_WOW64 )
    {
        WriteToLog
        (
            _T( "We can't stop 64-bit drivers from a 32-bit process." )
        );

        return E_FAIL;
    }
    else if ( FAILED( hGeneratePnPStopResult ) )
    {
        WriteToLog
        (
            _T( "EnableDisplayDevices failed with error 0x%x." ),
            hGeneratePnPStopResult
        );

        return E_FAIL;
    };

    WriteToLog( _T( "Display devices stopped successfully." ) );
    m_bPnPStop = TRUE;

    return S_OK;
}

//-----------------------------------------------------
// Function: ChangeMode
//
// Purpose: Cause a Mode Change
//
//-----------------------------------------------------
void
CMyTest::ChangeMode()
{
    // Get the device name of the primary monitor

    DISPLAY_DEVICEA dd;
    ZeroMemory(&dd, sizeof(dd));
    dd.cb = sizeof(dd);

    for (UINT i = 0; EnumDisplayDevices(NULL, i, &dd, 0); i++)
    {
        if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
        {
            break;
        }
    }

    DEVMODE OrigMode;

    memset (&OrigMode, 0, sizeof(OrigMode));
    OrigMode.dmSize = sizeof(OrigMode);

    EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &OrigMode);

    DEVMODE NewMode = OrigMode;

    switch (NewMode.dmDisplayOrientation)
    {
    case DMDO_90:
        NewMode.dmDisplayOrientation = DMDO_270;
        break;

    case DMDO_180:
        NewMode.dmDisplayOrientation = DMDO_DEFAULT;
        break;

    case DMDO_270:
        NewMode.dmDisplayOrientation = DMDO_90;
        break;

    default:
        NewMode.dmDisplayOrientation = DMDO_180;
        break;
    }

    NewMode.dmFields |= DM_DISPLAYORIENTATION;

    // Now do the mode change
    ChangeDisplaySettingsEx(dd.DeviceName, &NewMode, NULL, 0x51, 0);

    // That should have worked, now change it back
    ChangeDisplaySettingsEx(dd.DeviceName, &OrigMode, NULL, 0x51, 0);
}

//-----------------------------------------------------
// Function:  DWMDisableEnable
//
// Purpose: Disabl Enable DWM compose
//
//-----------------------------------------------------

HRESULT CMyTest::DWMDisableEnable
(	
)
{
    BOOL compositionEnabled;
    HRESULT hr = S_OK;

    // Disable DWM Compose
    hr = DwmIsCompositionEnabled(&compositionEnabled);
    if (FAILED(hr))
    {
        WriteToLog(_T("Call DwmIsCompositionEnabled failed. \n"));
        return hr;
    }

    if(compositionEnabled)
    {
        hr = DwmEnableComposition(DWM_EC_DISABLECOMPOSITION);
        if (FAILED(hr))
        {
            WriteToLog(_T("Call DwmDisableComposition failed. \n"));
            return hr;
        }
    }    

    // Turn On DWM Compose
    hr = DwmEnableComposition(DWM_EC_ENABLECOMPOSITION);
    if (FAILED(hr))
    {
        WriteToLog(_T("Call DwmdisableComposition failed. \n"));
        return hr;
    }

    return hr;
};

//----------------------------------------------------------------------------------
// Function:  GenerateTDRViaKMTEscape
//
// Purpose:
// Simulates a TDR using a kernel-mode thunk.
//
// Note:
// The TDR that is generated can't be tied to the
// specific monitor or device specified.
//----------------------------------------------------------------------------------
HRESULT CMyTest::GenerateTDRViaKMTEscape
(
)
{
    HRESULT hOverallResult = E_FAIL;

    //
    // Get a device context handle for the adapter.
    //
    HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL); 

    if ( hDC == NULL )
    {
        WriteToLog
        (
            _T( "GenerateTDR:  CreateDC failed." )
        );

        goto Cleanup;
    };

    //
    // Open the adapter, and get an adapter handle.
    //

    D3DKMT_OPENADAPTERFROMHDC OpenAdapterData;
    ZeroMemory( &OpenAdapterData, sizeof( OpenAdapterData ) );
    OpenAdapterData.hDc = hDC;

    assert( m_pD3DKMTOpenAdapterFromHDC != NULL );
    NTSTATUS OpenAdapterResult = (*m_pD3DKMTOpenAdapterFromHDC)( &OpenAdapterData );

    if ( !NT_SUCCESS( OpenAdapterResult ) )
    {
        WriteToLog
        (
            _T( "GenerateTDR:  D3DKMTOpenAdapterFromHdc failed with NTSTATUS %u (0x%x)." ),
            OpenAdapterResult,
            OpenAdapterResult
        );

        goto Cleanup;
    };

    if ( OpenAdapterData.hAdapter == NULL )
    {
        WriteToLog
        (
            _T( "GenerateTDR:  D3DKMTOpenAdapterFromHdc succeeded, but hAdapter == NULL." )
        );

        goto Cleanup;
    };


    //
    // Trigger the timeout.
    //

    D3DKMT_TDRDBGCTRL_ESCAPE tdrEscapeData = {};
    tdrEscapeData.TdrControl = D3DKMT_TDRDBGCTRLTYPE_FORCETDR;

    D3DKMT_ESCAPE EscapeData;
    ZeroMemory( &EscapeData, sizeof( EscapeData ) );
    EscapeData.hAdapter = OpenAdapterData.hAdapter;
    EscapeData.hDevice = NULL;
    EscapeData.Type = D3DKMT_ESCAPE_TDRDBGCTRL;
    EscapeData.pPrivateDriverData = &tdrEscapeData;
    EscapeData.PrivateDriverDataSize = sizeof( tdrEscapeData );

    assert( m_pD3DKMTEscape != NULL );
    NTSTATUS EscapeResult = (*m_pD3DKMTEscape)( &EscapeData );

    if ( NT_SUCCESS( EscapeResult) )
    {
        WriteToLog( TEXT( "TDR triggered successfully." ) );
    }
    else
    {
        WriteToLog
        (
            TEXT( "Failed to trigger TDR timeout.  D3DKMTEscape failed with NTSTATUS %u (0x%x)." ),
            EscapeResult,
            EscapeResult
        );

        goto Cleanup;
    };


    hOverallResult = S_OK;


Cleanup:
    //
    // Close the device context handle.
    //

    if ( hDC != NULL )
    {
        DeleteDC( hDC );
        hDC = NULL;
    };


    //
    // Close the adapter handle.
    //

    if ( OpenAdapterData.hAdapter != NULL )
    {
        D3DKMT_CLOSEADAPTER CloseAdapterData;
        CloseAdapterData.hAdapter = OpenAdapterData.hAdapter;

        assert( m_pD3DKMTCloseAdapter != NULL );
        (*m_pD3DKMTCloseAdapter)( &CloseAdapterData );

        OpenAdapterData.hAdapter = NULL;
    };

    return hOverallResult;
};

//----------------------------------------------------------------------------------
// Function:  EnableDisplayDevice
//
// Purpose:
// Enables or disables the specified device.
//----------------------------------------------------------------------------------
HRESULT CMyTest::EnableDisplayDevice
(
    IN HDEVINFO hDisplayDevicesInfo,
    __in SP_DEVINFO_DATA * pDisplayDeviceData,
    IN BOOL bEnable
)
{
    HRESULT hOverallResult = E_FAIL;


    //
    // Initialize installation parameters.
    //

    SP_PROPCHANGE_PARAMS pcp;
    ZeroMemory( &pcp, sizeof( pcp ) );
    pcp.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
    pcp.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
    pcp.StateChange = ( bEnable == TRUE ) ? DICS_ENABLE : DICS_DISABLE;
    pcp.Scope = DICS_FLAG_CONFIGSPECIFIC;
    pcp.HwProfile = 0;


    //
    // Set the installation parameters.
    //

    SetLastError( ERROR_SUCCESS );
    BOOL bSetInstallParamsResult = SetupDiSetClassInstallParams
    (
        hDisplayDevicesInfo,
        pDisplayDeviceData,
        &pcp.ClassInstallHeader,
        sizeof(pcp)
    );

    if ( bSetInstallParamsResult != TRUE )
    {
        DWORD SetInstallParamsError = GetLastError();

        WriteToLog
        (
            _T( "EnableDisplayDevice:  SetupDiSetClassInstallParams failed with error %u." ),
            SetInstallParamsError
        );

        //
        // N.B. [SEdmison]:
        // We don't do an HRESULT_FROM_WIN32 here, because these
        // arent traditional Win32 error codes.  They're some
        // bizarre error construct invented by setupapi that look
        // more-or-less like HRESULTs already.  See setupapi.h for
        // details.
        //

        hOverallResult = SetInstallParamsError;
        goto Cleanup;
    };


    //
    // Call the installer to perform the change.
    //

    SetLastError( ERROR_SUCCESS );
    BOOL bCallClassInstallerResult = SetupDiCallClassInstaller
    (
        DIF_PROPERTYCHANGE,
        hDisplayDevicesInfo,
        pDisplayDeviceData
    );

    if ( bCallClassInstallerResult != TRUE )
    {
        DWORD CallClassInstallerError = GetLastError();

        WriteToLog
        (
            _T( "EnableDisplayDevice:  SetupDiCallClassInstaller failed with error %u." ),
            CallClassInstallerError
        );

        hOverallResult = CallClassInstallerError;
        goto Cleanup;
    };


    //
    // Determine whether the system needs to be rebooted
    // in order to complete the enable/disable.
    //

    BOOL bNeedReboot = FALSE;

    SP_DEVINSTALL_PARAMS DisplayDeviceInstallParams;
    DisplayDeviceInstallParams.cbSize = sizeof(DisplayDeviceInstallParams);

    BOOL bGetInstallParamsResult = SetupDiGetDeviceInstallParams
    (
        hDisplayDevicesInfo,
        pDisplayDeviceData,
        &DisplayDeviceInstallParams
    );

    if ( bGetInstallParamsResult == TRUE )
    {
        if
        (
            (DisplayDeviceInstallParams.Flags & (DI_NEEDRESTART|DI_NEEDREBOOT))
        )
        {
            WriteToLog
            (
                _T( "EnableDisplayDevice:  Reboot needed." )
            );

            bNeedReboot = TRUE;
        };
    };


    if ( bNeedReboot == FALSE )
    {
        hOverallResult = S_OK;
    };


Cleanup:
    return hOverallResult;
};


//----------------------------------------------------------------------------------
// Function:  EnableDisplayDevices
//
// Purpose:
// Enables or disables all enumerated display hardware
// devices.
//----------------------------------------------------------------------------------
HRESULT CMyTest::EnableDisplayDevices
(
    IN BOOL bEnable
)
{
    HRESULT hOverallResult = E_FAIL;


    //
    // Enable/disable each device.
    //

    // For each device...
    for
    (
        int i = 0;
        i < m_cDisplayDevices;
        ++i
    )
    {
        // ...disable the device...
        HRESULT hDisableDeviceResult = EnableDisplayDevice
        (
            m_hDisplayDevicesInfo,
            &m_DisplayDeviceData[ i ],
            bEnable
        );

        if ( hDisableDeviceResult == ERROR_IN_WOW64 )
        {
            hOverallResult = hDisableDeviceResult;
            goto Cleanup;
        }
        else if ( FAILED( hDisableDeviceResult ) )
        {
            WriteToLog
            (
                _T( "EnableDisplayDevices:  Failed to enable/disable device %i.  EnableDisplayDevice returned error 0x%x." ),
                i,
                hDisableDeviceResult
            );

            hOverallResult = hDisableDeviceResult;
            goto Cleanup;
        };
    };


    //
    // If we made it this far, then we're good to go.
    //

    hOverallResult = S_OK;


Cleanup:
    return hOverallResult;
};

//----------------------------------------------------------------------------------
// Function:  EnumerateDisplayDevices
//
// Purpose:
// Enumerates display hardware devices.
//----------------------------------------------------------------------------------
HRESULT CMyTest::EnumerateDisplayDevices()
{
    HRESULT hOverallResult = E_FAIL;


    //
    // Get device information for all installed display
    // adapters.
    //

    assert( m_hDisplayDevicesInfo == NULL );
    GUID DisplayClassGUID = GUID_DEVCLASS_DISPLAY;
    m_hDisplayDevicesInfo = SetupDiGetClassDevs
    (
        &DisplayClassGUID,
        NULL,
        NULL,
        DIGCF_PRESENT
    );


    if
    (
        m_hDisplayDevicesInfo == INVALID_HANDLE_VALUE ||
        m_hDisplayDevicesInfo == NULL
    )
    {
        DWORD GetClassDeviceInfoError = GetLastError();

        WriteToLog( _T( "EnumerateDisplayDevices:  Unable to retrieve class definitions set for GUID_DEVCLASS_DISPLAY!" ) );

        hOverallResult = GetClassDeviceInfoError;
        goto Cleanup;
    };


    //
    // Enumerate each device.
    //

    // For each device...
    assert( m_cDisplayDevices == 0 );
    for
    (
        int i = 0;
        // ...enumerate the available drivers for the device...
        i < 100 &&
            m_cDisplayDevices < ARRAYSIZE( m_DisplayDeviceData );
        ++i
    )
    {
        m_DisplayDeviceData[ m_cDisplayDevices ].cbSize = sizeof( m_DisplayDeviceData[ m_cDisplayDevices ] );
        BOOL bEnumerateDisplayDeviceResult = SetupDiEnumDeviceInfo
        (
            m_hDisplayDevicesInfo,
            i,
            &m_DisplayDeviceData[ m_cDisplayDevices ]
        );

        if ( bEnumerateDisplayDeviceResult != TRUE )
        {
            DWORD EnumerateDisplayDeviceError = GetLastError();

            if ( EnumerateDisplayDeviceError == ERROR_NO_MORE_ITEMS )
            {
                break;
            }
            else
            {
                WriteToLog
                (
                    _T( "SetupDiEnumDeviceInfo failed with error %u." ),
                    EnumerateDisplayDeviceError
                );

                break;
            };
        };

        ++m_cDisplayDevices;
    };


    //
    // Check whether we found any devices.
    //

    if ( m_cDisplayDevices == 0 )
    {
        WriteToLog
        (
            _T( "EnumerateDisplayDevices:  No display devices found." )
        );

        hOverallResult = E_FAIL;
        goto Cleanup;
    };


    //
    // If we made it this far, then we're good to go.
    //

    hOverallResult = S_OK;


Cleanup:

    return hOverallResult;
};

//----------------------------------------------------------------------------------
// Function:  SuspendSystem
//
// Purpose:
// Creates a waitable timer, puts the machine into standby or
// hibernate, and then waits for the timer to be signalled.
//
//----------------------------------------------------------------------------------
HRESULT CMyTest::SuspendSystem
(
	IN bool bHibernate
)
{
    //
	// Assume by default that we've failed until we determine otherwise.
	//
    HRESULT hOverallResult = E_FAIL;

#if 0

    if(PowerManagement::SupportsPowerState(PowerState::S3))
    {
        PowerManagement::SleepComputerInS3AndAutoLogonOnResume(PowerManagement::MinimumAllowedSleepTimeInSeconds);
    }
#endif

	//
	// Create a waitable timer to wake us.
	//

	HANDLE hTimer = CreateWaitableTimer
	(
		NULL,
		FALSE,
		NULL
	);

	if
	(
		hTimer == INVALID_HANDLE_VALUE ||
		hTimer == NULL
	)
	{
		WriteToLog
		(			
			_T("SuspendSystem:  Could not create timer.\n")
		);

		goto CLEANUP;
	};


	//
	// Wake the system in Interval number of seconds.
	//

	LONG Interval = 20;
	LARGE_INTEGER WakeTime;
	WakeTime.QuadPart = Interval * -10000000;


	//
	// Set the timer to wake the system.
	//

	BOOL bSetTimerResult = SetWaitableTimer
	(
		hTimer,
		&WakeTime,
		500, // half-second interval, in case we miss it
		NULL,
		NULL,
		TRUE
	);

	if ( bSetTimerResult != TRUE )
	{
		WriteToLog
		(			
			_T("SuspendSystem:  Could not set waitable timer.\n")
		);

		goto CLEANUP;
	};


	//
	// Suspend the system.
	//

	BOOLEAN bForceCritical = 0;
	BOOLEAN bSuspendResult = SetSuspendState
	(
		(BOOLEAN) bHibernate,
		bForceCritical,
		0
	);

	if ( bSuspendResult == 0 )
	{
		WriteToLog
		(			
			_T("SuspendSystem:  Could not suspend.\n")
		);

		goto CLEANUP;
	};


	//
	// Wait for the timer to be signalled or our timeout to elapse.
	//

	DWORD WaitResult = WaitForSingleObject
	(
		hTimer,
		Interval * 2 * 1000
	);

	if ( WaitResult != WAIT_OBJECT_0 )
	{
		WriteToLog
		(			
			_T("SuspendSystem:  Wait for wake-up timer timed out.\n")
		);

		goto CLEANUP;
	};


	//
	// If we got this far, then we should be good to go.
	//

	hOverallResult = S_OK;


CLEANUP:
	if
	(
		hTimer != INVALID_HANDLE_VALUE &&
		hTimer != NULL
	)
	{
		//
		// Cancel the timer.
		//

		CancelWaitableTimer( hTimer );


		//
		// Release our timer.
		//

		CloseHandle( hTimer );
		hTimer = NULL;
	};


	//
	// Reset the input-idle counters to make sure that the system
	// and monitor(s) come back on.
	//

	SetThreadExecutionState( ES_SYSTEM_REQUIRED );
	SetThreadExecutionState( ES_DISPLAY_REQUIRED );
	SetThreadExecutionState( ES_USER_PRESENT );


	if ( hOverallResult != S_OK )
	{
		WriteToLog( _T("SuspendSystem() failed.\n") );
		//DebugBreak();
	};


	return hOverallResult;
}

//-----------------------------------------------------
// Function:  DisplayWindow
//
// Purpose:
// Displays the specified window.
//-----------------------------------------------------
HRESULT CMyTest::DisplayWindow
(
    IN UINT iWindow
)
{
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

    return S_OK;
};

//*************************************************************************
// Helper functions
//*************************************************************************
//----------------------------------------------------------------------------------
// Function: Helper_DisplayLastError()
// Purpose:  Display last error message
//----------------------------------------------------------------------------------
void 
CMyTest::Helper_DisplayLastError
(
    __in LPCTSTR    pszParentFunctionName,
    __in LPCTSTR    pszFunctionName
)
{
    DWORD       dwLastError = GetLastError();
    LPVOID      lpMsgBuf;

    assert(pszParentFunctionName != NULL);
    assert(pszFunctionName != NULL);

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL, 
                  dwLastError, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&lpMsgBuf, 
                  0, 
                  NULL);
    WriteToLog(_T("Error[%s]: %s failed with error %u (%s)."),
               pszParentFunctionName,
               pszFunctionName,
               dwLastError,
               lpMsgBuf);
    LocalFree(lpMsgBuf);
}

//-----------------------------------------------------
// Function:  VerifyAndLogTestCaseResult
//
// Purpose:
// Verify result.
//-----------------------------------------------------
DWORD
CMyTest::VerifyAndLogTestCaseResult
(
    HRESULT     hrExpectedCode,
    HRESULT     hrActualCode,
    TCHAR*      szOperation,
    HRESULT     hrAlternateAllowedCode,// = S_OK
    BOOL		MustBeExact // = TRUE
)
{
    TCHAR       szMsg[MAX_PATH] = _T("");
    DWORD       dwRetCode = 0;

    if (hrExpectedCode == hrActualCode)
    {
        //  pass
        StringCchPrintf(szMsg, MAX_PATH, _T("%s: succeeded "), szOperation);
        dwRetCode = S_OK;
    }
    else
    {        
        if(!MustBeExact && S_OK != hrActualCode && S_OK != hrExpectedCode)
        {

            StringCchPrintf(szMsg, MAX_PATH, _T("All error codes are accepted by the test. "), szOperation);
            StringCchPrintf(szMsg, MAX_PATH, _T("%s: succeeded "), szOperation);
            dwRetCode = S_OK;
        }
        else
        {
            //  fail
            if (FAILED(hrExpectedCode))
            {
                StringCchPrintf(szMsg, MAX_PATH, _T("%s: failed (expected = %s, returned = %s)"),
                                szOperation, HResultToString( hrExpectedCode), HResultToString( hrActualCode));
            }
            else
            {
                StringCchPrintf(szMsg, MAX_PATH, _T("%s: failed (returned = 0x%x, %s)"),
                                szOperation, hrActualCode, HResultToString( hrActualCode));
            }
            dwRetCode = E_FAIL;
        }
    }

    //  log
    WriteToLog( szMsg );

    return dwRetCode;
}   //  VerifyAndLogTestCaseResult

//----------------------------------------------------------------------------------
// Function: Helper_StaticThreadProc()
// Purpose:  Thread For Triggering System Events.
//----------------------------------------------------------------------------------
HRESULT 
CMyTest::Helper_CreateThread
(
)
{
    HRESULT     hrResult = S_OK;

    m_hThreadExitEvent = CreateEvent(NULL, TRUE, FALSE,	NULL);
    if (m_hThreadExitEvent == NULL)
    {
        Helper_DisplayLastError(_T("Helper_CreateThread()"), _T("CreateEvent()"));
        hrResult = E_FAIL;
        goto Exit;
    }

    // Create a thread to Trigger System Events 
    DWORD   ThreadID = 0;
    m_hThread = CreateThread(NULL,
                             0,
                             Helper_StaticThreadProc, // lpThreadFunc,
                             this,
                             0,                       // run the thread immediately
                             &ThreadID
                            );
    if (m_hThread == NULL)
    {
        Helper_DisplayLastError(_T("Helper_CreateThread()"), _T("CreateThread()"));
        hrResult = E_FAIL;
    };

Exit:
    return hrResult;
}

//----------------------------------------------------------------------------------
// Function: Helper_StaticThreadProc()
// Purpose:  Thread For Triggering System Events.
//----------------------------------------------------------------------------------
DWORD WINAPI 
CMyTest::Helper_StaticThreadProc
(
    __in VOID * pContext
)
{
    DWORD       dwResult = ERROR_SUCCESS; 

    assert(pContext != NULL);
    return((CMyTest *)pContext)->Helper_ThreadProc();
};


//----------------------------------------------------------------------------------
// Function: Helper_ThreadProc()
// Purpose:  Used to check DWM is always on during the test.
//----------------------------------------------------------------------------------
DWORD
CMyTest::Helper_ThreadProc
(
)
{
    DWORD       dwResult = ERROR_SUCCESS;
    HRESULT     hrResult = S_OK;

    // ThreadExitEvent must be created before this thread proc starts to run
    if (m_hThreadExitEvent == NULL)
    {
        WriteToLog(_T("Error[Helper_ThreadProc()]: m_hThreadExitEvent is NULL."));
        dwResult = ERROR_INVALID_HANDLE;
        goto Exit;
    }    

    while (1)
    {
        if(g_TestScenarios[ m_iCurrentScenario ].Event  == HYBRID)
        {
            if (!IsWindows8orLater()) // Only cycle DWM off on pre windows8 OSes.
                TriggerEvent(DWMONOFF);
            TriggerEvent(MODECHANGE);
            TriggerEvent(TDR);
            Sleep(100);
            TriggerEvent(PNPSTOP);
        }
        else
        {
            TriggerEvent(g_TestScenarios[ m_iCurrentScenario ].Event);
        }

        // For PnPSTop, Enable Devices again.
        if(m_bPnPStop)
        {
            Sleep(50);
            EnableDisplayDevices( TRUE );
            m_bPnPStop = FALSE;
        }

        // Exit fronm the thread when m_hThreadExitEvent is signaled
        if (WaitForSingleObject(m_hThreadExitEvent, 0) == WAIT_OBJECT_0)
        {
            break;
        };        

        Sleep((DWORD)THREAD_SLEEP_TIME);
    };

Exit:
    return dwResult;
};

//----------------------------------------------------------------------------------
// Function:  LoadGDI
//
// Purpose:
// Loads GDI32.dll and gets pointers to kernel-mode
// thunks.
//----------------------------------------------------------------------------------
HRESULT CMyTest::LoadGDI()
{
    //
    // Load GDI32.dll.
    //

    m_hGDI32 = LoadLibrary( "gdi32.dll" );

    if ( m_hGDI32 == NULL )
    {
        DWORD LoadLibraryError = GetLastError();


        if ( LoadLibraryError != ERROR_SUCCESS )
        {
            WriteToLog
            (
                _T( "LoadGDI:  LoadLibrary( GDI32 ) failed with error %u." ),
                LoadLibraryError
            );

            return HRESULT_FROM_WIN32( LoadLibraryError );
        };

        WriteToLog( _T( "LoadGDI:  LoadLibrary( GDI32 ) failed." ) );

        return E_FAIL;
    };


    //
    // Get pointers to our thunks.
    //

    m_pD3DKMTEscape = (PFND3DKMT_ESCAPE) GetProcAddress
    (
        m_hGDI32,
        "D3DKMTEscape"
    );

    if ( m_pD3DKMTEscape == NULL )
    {
        DWORD GetProcAddressError = GetLastError();

        if ( GetProcAddressError != ERROR_SUCCESS )
        {
            WriteToLog
            (
                _T( "LoadGDI:  GetProcAddress failed with error %u." ),
                GetProcAddressError
            );

            return HRESULT_FROM_WIN32( GetProcAddressError );
        };


        WriteToLog
        (
            _T( "LoadGDI:  GetProcAddress failed." )
        );

        return E_FAIL;
    };


    m_pD3DKMTOpenAdapterFromHDC = (PFND3DKMT_OPENADAPTERFROMHDC) GetProcAddress
    (
        m_hGDI32,
        "D3DKMTOpenAdapterFromHdc"
    );

    if ( m_pD3DKMTOpenAdapterFromHDC == NULL )
    {
        DWORD GetProcAddressError = GetLastError();

        if ( GetProcAddressError != ERROR_SUCCESS )
        {
            WriteToLog
            (
                _T( "LoadGDI:  GetProcAddress failed with error %u." ),
                GetProcAddressError
            );

            return HRESULT_FROM_WIN32( GetProcAddressError );
        };


        WriteToLog
        (
            _T( "LoadGDI:  GetProcAddress failed." )
        );

        return E_FAIL;
    };


    m_pD3DKMTOpenAdapterFromDeviceName = (PFND3DKMT_OPENADAPTERFROMDEVICENAME) GetProcAddress
    (
        m_hGDI32,
        "D3DKMTOpenAdapterFromDeviceName"
    );

    if ( m_pD3DKMTOpenAdapterFromDeviceName == NULL )
    {
        DWORD GetProcAddressError = GetLastError();

        if ( GetProcAddressError != ERROR_SUCCESS )
        {
            WriteToLog
            (
                _T( "LoadGDI:  GetProcAddress failed with error %u." ),
                GetProcAddressError
            );

            return HRESULT_FROM_WIN32( GetProcAddressError );
        };


        WriteToLog
        (
            _T( "LoadGDI:  GetProcAddress failed." )
        );

        return E_FAIL;
    };


    m_pD3DKMTCloseAdapter = (PFND3DKMT_CLOSEADAPTER) GetProcAddress
    (
        m_hGDI32,
        "D3DKMTCloseAdapter"
    );

    if ( m_pD3DKMTCloseAdapter == NULL )
    {
        DWORD GetProcAddressError = GetLastError();

        if ( GetProcAddressError != ERROR_SUCCESS )
        {
            WriteToLog
            (
                _T( "LoadGDI:  GetProcAddress failed with error %u." ),
                GetProcAddressError
            );

            return HRESULT_FROM_WIN32( GetProcAddressError );
        };


        WriteToLog
        (
            _T( "LoadGDI:  GetProcAddress failed." )
        );

        return E_FAIL;
    };


    return S_OK;
};

//-----------------------------------------------------
// Function:  MsgProc
//
// Purpose:
// Message handler routine.
//
// Called by:
// StaticMsgProc
//-----------------------------------------------------
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

//----------------------------------------------------------------------------------
// Function: CreateBitmapInfoStruct()
// Purpose:  Create the bitmap in memory
//----------------------------------------------------------------------------------
PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{ 
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 

    // Retrieve the bitmap color format, width, and height. 
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) 
    {
        wprintf(L"Unable to load the bitmap\n");
        return NULL;
    }

    // Convert the color format to a count of bits. 
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1)
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure 
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
    // data structures.)

    unsigned long lClrBitsReqd = 1;
    
    int iPow = cClrBits;
    
    while(iPow > 0)
    {
        lClrBitsReqd *= 2;

        // Rollover
        if (lClrBitsReqd == 0)
            lClrBitsReqd = 1;

        iPow--;
    }

    if (lClrBitsReqd <= 0)
    {
        wprintf(L"Error in calculating bits required for color pallete. Size is %ld for %d\n", lClrBitsReqd, cClrBits);
        return NULL;
    }

     if (cClrBits != 24) 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER) + 
                    sizeof(RGBQUAD) * lClrBitsReqd); 

     // There is no RGBQUAD array for the 24-bit-per-pixel format. 

     else 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER)); 

    // Initialize the fields in the BITMAPINFO structure. 

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = lClrBitsReqd; 

    // If the bitmap is not compressed, set the BI_RGB flag. 
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color 
    // indices and store the result in biSizeImage. 
    // For Windows NT, the width must be DWORD aligned unless 
    // the bitmap is RLE compressed. This example shows this. 
    // For Windows 95/98/Me, the width must be WORD aligned unless the 
    // bitmap is RLE compressed.
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
                                  * pbmi->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the 
    // device colors are important. 
     pbmi->bmiHeader.biClrImportant = 0; 

     return pbmi; 
 } 

//----------------------------------------------------------------------------------
// Function: CreateBMPFile()
// Purpose:  Create the bitmap file
//----------------------------------------------------------------------------------
HRESULT CreateBMPFile(LPCTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC)
 { 
    HANDLE hf;                  // file handle 
    BITMAPFILEHEADER hdr;       // bitmap file-header 
    PBITMAPINFOHEADER pbih;     // bitmap info-header 
    LPBYTE lpBits;              // memory pointer 
    DWORD dwTotal;              // total count of bytes 
    DWORD cb;                   // incremental count of bytes 
    BYTE *hp;                   // byte pointer 
    DWORD dwTmp;
    HRESULT hr = E_ABORT;

    pbih = (PBITMAPINFOHEADER) pbi; 
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!lpBits)
    {
        wprintf(L"CreateBMPFile: Couldn't allocate memory\n");
        goto CLEANUP;
    }

    // Retrieve the color table (RGBQUAD array) and the bits 
    // (array of palette indices) from the DIB. 
    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS)) 
    {
        wprintf(L"CreateBMPFile: Couldn't get the color table\n");
        goto CLEANUP;
    }

    // Create the .BMP file. 
    hf = CreateFile(pszFile, 
                   GENERIC_READ | GENERIC_WRITE, 
                   (DWORD) 0, 
                    NULL, 
                   CREATE_ALWAYS, 
                   FILE_ATTRIBUTE_NORMAL, 
                   (HANDLE) NULL);

    if (hf == INVALID_HANDLE_VALUE) 
    {
        wprintf(L"CreateBMPFile: Couldn't create output file\n");
        goto CLEANUP;
    }

    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
    // Compute the size of the entire file. 
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
                 pbih->biSize + pbih->biClrUsed 
                 * sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 

    // Compute the offset to the array of color indices. 
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
                    pbih->biSize + pbih->biClrUsed 
                    * sizeof (RGBQUAD); 

    // Copy the BITMAPFILEHEADER into the .BMP file. 
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp,  NULL))
    {
        wprintf(L"CreateBMPFile: Couldn't write BITMAPFILEHEADER into file\n");
        goto CLEANUP;
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
    if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
                  + pbih->biClrUsed * sizeof (RGBQUAD), 
                  (LPDWORD) &dwTmp, NULL))
    {
        wprintf(L"CreateBMPFile: Couldn't write BITMAPINFOHEADER into file\n");
        goto CLEANUP;
    }

    // Copy the array of color indices into the .BMP file. 
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) 
    {
        wprintf(L"CreateBMPFile: Couldn't write color indices into file");
        goto CLEANUP;
    }

    // Close the .BMP file. 
    if (!CloseHandle(hf)) 
    {
        wprintf(L"CreateBMPFile: Couldn't close the file\n");
        goto CLEANUP;
    }

    hr = S_OK;

CLEANUP:
    // Free memory. 
    if (lpBits != NULL)
    {
        GlobalFree((HGLOBAL)lpBits);
    }

    return hr;
}
