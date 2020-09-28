//
// Tests.cpp
//
// Perform D3D9 Mode Enumeration tests
// Devices.
//
// Class definitions:   CTests
// 


#include <d3dx9.h>
#include <StrSafe.h>
#include "ModeEnumeration.hpp"
#include "Tests.hpp"


// D3D9 Formats
D3DFORMAT ModeFormats[] =
{
    D3DFMT_A1R5G5B5,
    D3DFMT_R5G6B5  ,
    D3DFMT_R8G8B8  ,
    D3DFMT_X8R8G8B8,
};

const int c_nNumFormats = (sizeof(ModeFormats) / sizeof(D3DFORMAT));

//

CTests::CTests() :
    m_nNumMonitors(0),
    m_nPrimaryOutputNum(0),
    m_nDeviceGrouping(0),
    m_bUseFullscreen(true),
    m_pD3D9L(NULL),
    m_D3D9LDeviceCount(0),
    m_nLargestModeCount(0)
{

    memset( &m_fGotOriginalDevMode[0], 0, sizeof(m_fGotOriginalDevMode) );
    memset( &m_originalDispDev[0], 0, sizeof(m_originalDispDev) );
    memset( &m_originalDevMode[0], 0, sizeof(m_originalDevMode) );
    memset( &m_pD3D9LDevice[0], 0, sizeof(m_pD3D9LDevice) );
    memset( &m_d3dCaps9[0], 0, sizeof(m_d3dCaps9) );
    memset( &m_D3D9LModeToTest[0][0], 0, sizeof(m_D3D9LModeToTest) );
    memset( &m_groupModeToTest[0][0], 0, sizeof(m_groupModeToTest) );
    memset( &m_hwndD3D9LDevWnd[0], 0, sizeof(m_hwndD3D9LDevWnd) );
    memset( &m_D3D9Lpp[0][0], 0, sizeof(m_D3D9Lpp) );
}

CTests::~CTests()
{
}

bool  CTests::InitializeTestGroup()
{
    HRESULT         hr = S_OK;
    ULONG           ulRefCt = 0;
    char            szMsg[c_nMSG_LEN] = {0};    
    UINT            i;

    //
    //  Log the name, path and moddate of the test app.
    //
    CCommon::LogTestAppInfo();
 
    DebugOut.Write("Entering InitializeTestGroup.\n");

    // Get the adapter and modes counts
    //
    IDirect3D9Ex   *pTempD3D9L = NULL;

    m_nLargestModeCount = 0;

    if ( FAILED( hr = Direct3DCreate9Ex( D3D_SDK_VERSION,
                                         &pTempD3D9L
                                       ) ) )
    {
        StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: Direct3DCreate9Ex returned hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
        CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
        return false;
    }

    m_D3D9LDeviceCount = pTempD3D9L->GetAdapterCount();

    for ( i = 0; i < m_D3D9LDeviceCount; i++ )
    {
        for ( UINT uiFormatIndex = 0; uiFormatIndex < c_nNumFormats; ++uiFormatIndex )
        {
            UINT    uiModeCount = 0;

            // Get a count of the available modes for this adapter in the specified format.
            uiModeCount = pTempD3D9L->GetAdapterModeCount( i, ModeFormats[uiFormatIndex] );

            if ( uiModeCount > m_nLargestModeCount )
            {
                m_nLargestModeCount = uiModeCount;
            }
        }
    }

    if ( ( ulRefCt = pTempD3D9L->Release() ) != 0 )
    {
        StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: IDirect3D9Ex::Release() left RefCount = %d\n", __FUNCTION__, __FILE__, __LINE__, ulRefCt );
        CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
        return false;
    }

    if ( !InitializeWindowClass() )
    {
        DebugOut.Write("Exiting InitializeTestGroup (Failed).\n");

        return false;
    }

    DebugOut.Write("Exiting InitializeTestGroup (Succeeded).\n");

    return true;
}

void  CTests::CleanupTestGroup()
{
    DebugOut.Write("Entering CleanupTestGroup.\n");

    DebugOut.Write("Exiting CleanupTestGroup.\n");
}

void  CTests::InitTestParameters()
{
    LONG            lReturn = 0;
    TCHAR           szMsg[c_nMSG_LEN] = {0};    
    DISPLAY_DEVICE  dispDev = {0};
    DEVMODE         devMode = {0};


    DebugOut.Write("Entering InitTestParameters.\n");

    //
    // Attach any detached screens and set all rotations to 0.
    //  
    CCommon::AttachScreensAndSetRotationsToZero();

    //
    // Get the current modes and save them for restoration when done testing.
    //    
    for ( DWORD dwDevNum = 0; (int)dwDevNum < MONITORS_MAX; dwDevNum++ )
    { 
        if ( CCommon::GetGDIDevMode( dwDevNum, dispDev/* by ref */, devMode/* by ref */ ) )
        {
            m_nNumMonitors++;    
            m_originalDispDev[dwDevNum] = dispDev;
            m_originalDevMode[dwDevNum] = devMode;
            m_fGotOriginalDevMode[dwDevNum] = TRUE;
        }
    }

    if ( InitializeTestGroup() )
    {
        AddParameter( _T("PrimaryOutputNum"), &m_nPrimaryOutputNum, RangeValueSet( 0, (m_nNumMonitors - 1), 1 ) );  

        AddParameter( _T("ModeIndex"), &m_nModeIndex, RangeValueSet( 0, (int)(m_nLargestModeCount - 1), 1 ) );  

        AddParameter( _T("DeviceGrouping"), &m_nDeviceGrouping, RangeValueSet( 1, 2, 1 ) );  // 1 = D3D9L Groups, 2 = D3D9L Devices

        AddParameter( _T("Rotation"), &m_nModeRotation, RangeValueSet( (int)D3DDISPLAYROTATION_IDENTITY, (int)D3DDISPLAYROTATION_270, 1 ) );
        
        AddParameter( _T("UseFullscreen"), &m_bUseFullscreen, new CBoolValueSet() );
    }

// Testing
//m_nPrimaryOutputNum = 0;
//m_nModeIndex = 0;
//m_nDeviceGrouping = 1;
//m_nModeRotation = D3DDISPLAYROTATION_IDENTITY;
//m_bUseFullscreen = true;


    DebugOut.Write("Exiting InitTestParameters.\n");
}


TEST_RESULT  CTests::Setup()
{
    HRESULT         hr = S_OK;
    TEST_RESULT     test_result = RESULT_FAIL;   
    char            szMsg[c_nMSG_LEN] = {0};    
    ULONG           ulRefCt = 0;

    // Is D3D9L supported?
    IDirect3D9Ex   *pTempD3D9L = NULL;

    if ( FAILED( hr = Direct3DCreate9Ex( D3D_SDK_VERSION,
                                         &pTempD3D9L
                                       ) ) )
    {
        test_result = RESULT_SKIP;
        StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "Driver does not support D3D9L - test is skipped\n" );
        CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
        StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  SKIPPED: Direct3DCreate9Ex returned hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
        CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
        goto Exit;
    }

    if ( m_nLargestModeCount > c_nMODE_MAX )
    {
        test_result = RESULT_FAIL;
        StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: Unexpected number of Modes = %d\n", __FUNCTION__, __FILE__, __LINE__, m_nLargestModeCount );
        CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
        goto Exit;
    }

    test_result = EnumerateDisplayDevices( pTempD3D9L );

    // Release D3D9
    if ( ( ulRefCt = pTempD3D9L->Release() ) != 0 )
    {
        test_result = RESULT_FAIL;
        StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: IDirect3D9Ex::Release() left RefCount = %d\n", __FUNCTION__, __FILE__, __LINE__, ulRefCt );
        CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
        goto Exit;
    }

    if ( !InitializeWindowClass() )
    {
        test_result = RESULT_FAIL;
        goto Exit;
    }

Exit:
    return test_result;
}

//

TEST_RESULT
CTests::EnumerateDisplayDevices(
    __in    IDirect3D9Ex*   pTempD3D9L
   )
{
    TEST_RESULT     test_result = RESULT_FAIL;   
    UINT            nDeviceNum = 0;


    // Enumerate and store D3D9L Devices and their modes
    //
    m_D3D9LDeviceCount = pTempD3D9L->GetAdapterCount();

    for ( nDeviceNum = 0; nDeviceNum < m_D3D9LDeviceCount; nDeviceNum++ )
    {
        if ( ( test_result = EnumerateAndStoreSelectedModes( pTempD3D9L, nDeviceNum ) ) != RESULT_PASS )
        {
            goto Exit;
        }
    }

    test_result = RESULT_PASS;

Exit:

    return test_result;
}


TEST_RESULT
CTests::EnumerateAndStoreSelectedModes(
    __in    IDirect3D9Ex*   pTempD3D9L,
    __in    UINT            nDeviceNum
   )
{
    TEST_RESULT     test_result = RESULT_FAIL;   
    HRESULT         hr = S_OK;
    char            szMsg[c_nMSG_LEN] = {0};    
    int             nStoredMode;


    // Store the Caps so we can determine later on if this adapter is a master adapter.
    if ( FAILED( hr = pTempD3D9L->GetDeviceCaps( nDeviceNum, g_Application.m_nD3D9DriverType, &m_d3dCaps9[nDeviceNum] ) ) )
    {
        StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: IDirect3D9Ex::GetDeviceCaps returned hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
        CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
        goto Exit;
    }

    // Index for the modes that are stored
    nStoredMode = -1;

    // For each adapter, we filter on the formats and store matching modes.
    for ( UINT uiFormatIndex = 0; uiFormatIndex < c_nNumFormats; ++uiFormatIndex )
    {
        UINT        uiModeCount = 0;

        // Get a count of the available modes for this adapter in the specified format.
        uiModeCount = pTempD3D9L->GetAdapterModeCount( nDeviceNum, ModeFormats[uiFormatIndex] );

        // No modes found for the specified format
        if ( uiModeCount == 0 )
        {
            continue;
        }

        // Get the modes for this adapter.
        D3DDISPLAYMODEFILTER    filter;

        for ( UINT nEnumeratedMode = 0; nEnumeratedMode < (int)uiModeCount; nEnumeratedMode++ )
        {
            memset( &filter, 0, sizeof(filter) );
            filter.Size = sizeof(filter);
            filter.Format = ModeFormats[uiFormatIndex];
            filter.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;

            nStoredMode++;
            memset( &m_D3D9LModeToTest[nDeviceNum][nStoredMode], 0, sizeof(D3DDISPLAYMODEEX) );
            m_D3D9LModeToTest[nDeviceNum][nStoredMode].Size = sizeof(D3DDISPLAYMODEEX);

            // Enumerate and store the mode
            if ( FAILED( hr = pTempD3D9L->EnumAdapterModesEx( nDeviceNum, &filter, nEnumeratedMode, &m_D3D9LModeToTest[nDeviceNum][nStoredMode] ) ) )
            {
                StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: IDirect3D9Ex::EnumAdapterModes failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
                CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
                goto Exit;
            }
        }

        StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "Matching format - contains %d modes.\n", (int)uiModeCount );
        CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
    }

    test_result = RESULT_PASS;

Exit:

    return test_result;
}

//

TEST_RESULT  CTests::ExecuteTestCase()
{
    HRESULT         hr = S_OK;
    TEST_RESULT     test_result = RESULT_FAIL;   
    char            szMsg[c_nMSG_LEN] = {0};    

    if ( ( test_result = CreateInitialDXObjects() ) != RESULT_PASS )
    {
        goto Exit;
    }

    CHAR     szDevName[c_nDEV_NAME_LEN] = {0};
    DEVMODE  devMode;

    memset( &devMode, 0, sizeof(devMode) );

    if ( ( test_result = GetMiscDeviceData( m_nPrimaryOutputNum, &szDevName[0], sizeof(szDevName), devMode ) ) != RESULT_PASS )
    {
        goto Exit;
    }

    if ( !CCommon::SetPrimaryMonitor( &szDevName[0], sizeof(szDevName), devMode ) )
    {
        test_result = RESULT_FAIL;
        goto Exit;
    }

    UINT    nDeviceNum = 0;
    int     nAction = 0;
    BOOL    fFullscreen = FALSE;
    BOOL    fMasterAdapter = ( m_nDeviceGrouping == 1 ? TRUE : FALSE );

    //
    // D3D9L
    //
    
    // Adapter Groups:  Set up the swapchains for each adapter group.  Fullscreen is required.
    // Device List:     Set up the swapchains for every device.  Windowed or fullscreen.
    if ( ( m_nDeviceGrouping == 1 && m_bUseFullscreen )
        || m_nDeviceGrouping == 2 )
    {
        for ( nDeviceNum = 0; nDeviceNum < (int)m_D3D9LDeviceCount; nDeviceNum++ )
        {
            // If doing Adapter Groups, check for the Master Ordinal.  Also check we have a valid mode. 
            if ( ( ( m_nDeviceGrouping == 1 && m_d3dCaps9[nDeviceNum].AdapterOrdinal == m_d3dCaps9[nDeviceNum].MasterAdapterOrdinal )
                    || m_nDeviceGrouping == 2 )
                && m_D3D9LModeToTest[nDeviceNum][m_nModeIndex].Width > 0 )
            {
                if ( ( test_result = SetupSwapChain( nDeviceNum,
                                                     fMasterAdapter
                                                   ) ) != RESULT_PASS )
                {
                    goto Exit;
                }

                if ( m_bUseFullscreen )
                {
                    for ( nAction = 0; nAction < 3; nAction++ )
                    {
                        // Check for the Master Ordinal for both Adapter Groups and Device Lists. 
                        if ( m_d3dCaps9[nDeviceNum].AdapterOrdinal == m_d3dCaps9[nDeviceNum].MasterAdapterOrdinal )
                        {
                            if ( FAILED( hr = m_pD3D9LDevice[nDeviceNum]->ResetEx( &m_D3D9Lpp[nDeviceNum][0], &m_groupModeToTest[nDeviceNum][0] ) ) )
                            {
                                test_result = RESULT_FAIL;
                                StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: IDirect3DDevice9Ex::ResetEx failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
                                CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
                                goto Exit;
                            }

                            UINT i;
                            for ( i = 0; i < 8; i++ )
                            {
                                // Check the screen mode
                                fFullscreen = CCommon::IsFullscreen( m_hwndD3D9LDevWnd[nDeviceNum], m_D3D9LModeToTest[nDeviceNum][m_nModeIndex] );
                                if (fFullscreen)
                                {
                                    break;
                                }

                                Sleep(500);
                            }

                            if (i == 8)
                            {
                                StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  Not full screen\n", __FUNCTION__, __FILE__, __LINE__ );
                                CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
                                goto Exit;
                                test_result = RESULT_FAIL;
                            }
                        }
                    }
                }
            }
        }
    }

Exit:

    if ( !ReleaseTestCaseObjects() )
    {
        test_result = RESULT_FAIL;
        StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: ReleaseTestCaseObjects() returned FALSE.\n", __FUNCTION__, __FILE__, __LINE__ );
        CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
    }  

    DebugOut.Write("Exiting ExecuteTestCase.\n");

    return test_result;
}


TEST_RESULT  CTests::CreateInitialDXObjects()
{
    TEST_RESULT     test_result = RESULT_FAIL;   
    HRESULT         hr = S_OK;
    char            szMsg[c_nMSG_LEN] = {0};    


    // Create D3D9 object
    //
    if ( FAILED( hr = Direct3DCreate9Ex( D3D_SDK_VERSION,
                                         &m_pD3D9L
                                       ) ) )
    {
        StringCchPrintf( szMsg, sizeof(szMsg), "%s in %s at line %d.  SKIPPED: Direct3DCreate9Ex returned hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
        CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
        goto Exit;
    }

    test_result = RESULT_PASS;

Exit:

    return test_result;
}

//
TEST_RESULT
CTests::GetMiscDeviceData(
    __in        int             nPrimaryOutputNum,
    __inout_ecount(nDevNameLen)  PCHAR  pszDevName,
    __in        UINT            nDevNameLen,
    __inout     DEVMODE&        devMode         // by ref
   )
{
    TEST_RESULT         test_result = RESULT_FAIL;
    HRESULT             hr = S_OK;
    char                szMsg[c_nMSG_LEN] = {0};    
    BOOL                bOK = FALSE;
    DISPLAY_DEVICE      displayDevice = {0};


    displayDevice.cb = sizeof(displayDevice);

    if ( ( bOK = EnumDisplayDevices( NULL, nPrimaryOutputNum, &displayDevice, 0 ) ) == FALSE )
    {
        StringCchPrintf( szMsg, sizeof(szMsg), "%s in %s at line %d.  FAILED: EnumDisplayDevices failed - iDevNum = %d\n", __FUNCTION__, __FILE__, __LINE__, nPrimaryOutputNum );
        CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
        goto Exit;
    }

    StringCchCopy( pszDevName, nDevNameLen, &displayDevice.DeviceName[0] );
    
    // Ensure that string is NULL-terminated - PREfast request. 
    pszDevName[nDevNameLen - 1] = 0;

    if ( !CCommon::GetGDIDevMode( pszDevName, nDevNameLen, devMode ) )
    {
        goto Exit;
    }
       
    test_result = RESULT_PASS;

Exit:
    return test_result;
}
 
//
TEST_RESULT
CTests::SetupSwapChain(
    __in    UINT            nDeviceNum,
    __in    BOOL            fMasterAdapter
   )
{
    TEST_RESULT         test_result = RESULT_FAIL;
    BOOL                bReturn = FALSE;
    LONG                lReturn = 0;
    HRESULT             hr = S_OK;
    CHAR                szDevName[32] = {0};
    DEVMODE             devMode = {0};
    DISPLAY_DEVICE      dispDev = {0};
    char                szMsg[c_nMSG_LEN] = {0};    
    char                szTempMsg[c_nMSG_LEN] = {0};    
    int                 nRotationDifference = 0;
    BOOL                bFullScreen = FALSE;
    int                 nMax = 0;
    UINT                uiRefreshRate = 0;      // Calculated integer Refresh Rate


    if ( fMasterAdapter )
    {
        nMax = m_d3dCaps9[nDeviceNum].NumberOfAdaptersInGroup;
    }
    else
    {
        nMax = 1;
    }

    HWND  hwndParent = GetFramework()->GetShell()->GetMainHWnd();

    for ( int j = 0; j < nMax; j++ )
    {
        if ( !CCommon::GetGDIDevMode( (DWORD)nDeviceNum + j, dispDev/* by ref */, devMode/* by ref */ ) )
        {
            test_result = RESULT_FAIL;
            goto Exit;
        }

        StringCchCopy( &szDevName[0], sizeof(szDevName), (STRSAFE_LPSTR)&devMode.dmDeviceName[0] );

        devMode.dmFields = 0;

        if ( m_nModeRotation != (devMode.dmDisplayOrientation + 1) )
        {
            nRotationDifference = __max((int)devMode.dmDisplayOrientation, (m_nModeRotation - 1)) 
                                  - __min((int)devMode.dmDisplayOrientation, (m_nModeRotation - 1)); 
            devMode.dmDisplayOrientation = (SHORT)(m_nModeRotation - 1);
            devMode.dmFields |= DM_DISPLAYORIENTATION;

            if ( nRotationDifference != 2 )     // If the difference is not 2, the screen is not rotated by 180 deg. from before
            {                                   // and the width/height values need to be swopped.
                DWORD  dwTemp = devMode.dmPelsWidth;
                devMode.dmPelsWidth = devMode.dmPelsHeight;
                devMode.dmPelsHeight = dwTemp;
                devMode.dmFields |= DM_PELSWIDTH;
                devMode.dmFields |= DM_PELSHEIGHT;
            }
        }

        devMode.dmDisplayFrequency = 0;
        devMode.dmFields |= DM_DISPLAYFREQUENCY;

        if ( ( lReturn = ChangeDisplaySettingsEx( &szDevName[0], &devMode, NULL, CDS_UPDATEREGISTRY, NULL ) ) != DISP_CHANGE_SUCCESSFUL )
        {
            StringCchPrintf( szMsg, sizeof(szMsg), "%s in %s at line %d.  FAILED: ChangeDisplaySettingsEx() returned %d: width[%u], height[%u], refreshrate[%u], devMode.dmDisplayOrientation[%u]\n", 
                             __FUNCTION__, __FILE__, __LINE__, lReturn, devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmDisplayFrequency, devMode.dmDisplayOrientation);
            CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
            goto Exit;
        }

        // Create a device window.
        if ( m_hwndD3D9LDevWnd[nDeviceNum + j] = CreateWindowEx( WS_EX_TOPMOST,
                                                                 m_szFocusWindowClassName,
                                                                 "Device Window - D3D9L",
                                                                 WS_OVERLAPPEDWINDOW,
                                                                 (int)( devMode.dmPosition.x + ( devMode.dmPelsWidth / 16 ) ),   // Position window on secondary desktop if secondary
                                                                 (int)( devMode.dmPosition.y + ( devMode.dmPelsHeight / 16 ) ),  // is being tested.
                                                                 (int)( devMode.dmPelsWidth / 2 ),   
                                                                 (int)( devMode.dmPelsHeight / 2 ),
                                                                 hwndParent,
                                                                 NULL,
                                                                 m_hProgInst,
                                                                 NULL
                                                               ) )
        {
            ShowWindow( m_hwndD3D9LDevWnd[nDeviceNum + j], SW_SHOWNORMAL );
        }
    }

    if ( m_hwndD3D9LDevWnd[nDeviceNum] )
    {
        DWORD                   dwBehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        RECT                    rc = {0};

        memset( &m_groupModeToTest[nDeviceNum][0], 0, sizeof(m_groupModeToTest[0]) );

        if ( g_Application.m_nD3D9DriverType == D3DDEVTYPE_HAL )
        {
            dwBehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
        }

        if ( fMasterAdapter )
        {
            dwBehaviorFlags |= D3DCREATE_ADAPTERGROUP_DEVICE;

            for ( int j = 0; j < nMax; j++ )
            {
                memset( &m_D3D9Lpp[nDeviceNum][j], 0, sizeof(m_D3D9Lpp[nDeviceNum][j]) );
                m_D3D9Lpp[nDeviceNum][j].Windowed = (BOOL)!m_bUseFullscreen;
                m_D3D9Lpp[nDeviceNum][j].BackBufferFormat = m_D3D9LModeToTest[nDeviceNum][m_nModeIndex].Format;
                m_D3D9Lpp[nDeviceNum][j].BackBufferCount = c_nBACK_BUFFER_COUNT;
                m_D3D9Lpp[nDeviceNum][j].SwapEffect = D3DSWAPEFFECT_DISCARD;
                m_D3D9Lpp[nDeviceNum][j].hDeviceWindow = m_hwndD3D9LDevWnd[nDeviceNum + j];
                m_D3D9Lpp[nDeviceNum][j].PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
                m_D3D9Lpp[nDeviceNum][j].BackBufferWidth = m_D3D9LModeToTest[nDeviceNum][m_nModeIndex].Width;
                m_D3D9Lpp[nDeviceNum][j].BackBufferHeight = m_D3D9LModeToTest[nDeviceNum][m_nModeIndex].Height;
                m_groupModeToTest[nDeviceNum][j] = m_D3D9LModeToTest[nDeviceNum][m_nModeIndex];

                if ( m_D3D9Lpp[nDeviceNum][j].Windowed == TRUE )
                {
                    GetClientRect( m_hwndD3D9LDevWnd[nDeviceNum], &rc );
                    m_D3D9Lpp[nDeviceNum][j].BackBufferWidth = rc.right;
                    m_D3D9Lpp[nDeviceNum][j].BackBufferHeight = rc.bottom;
                }
                else
                {
                    m_D3D9Lpp[nDeviceNum][j].FullScreen_RefreshRateInHz = m_D3D9LModeToTest[nDeviceNum][m_nModeIndex].RefreshRate;
                }
            }
        }
        else
        {
            memset( &m_D3D9Lpp[nDeviceNum][0], 0, sizeof(m_D3D9Lpp[nDeviceNum][0]) );
            m_D3D9Lpp[nDeviceNum][0].Windowed = (BOOL)!m_bUseFullscreen;
            m_D3D9Lpp[nDeviceNum][0].BackBufferFormat = m_D3D9LModeToTest[nDeviceNum][m_nModeIndex].Format;
            m_D3D9Lpp[nDeviceNum][0].BackBufferCount = 1;
            m_D3D9Lpp[nDeviceNum][0].SwapEffect = D3DSWAPEFFECT_DISCARD;
            m_D3D9Lpp[nDeviceNum][0].hDeviceWindow = m_hwndD3D9LDevWnd[nDeviceNum];
            m_D3D9Lpp[nDeviceNum][0].PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
            m_D3D9Lpp[nDeviceNum][0].BackBufferWidth = m_D3D9LModeToTest[nDeviceNum][m_nModeIndex].Width;
            m_D3D9Lpp[nDeviceNum][0].BackBufferHeight = m_D3D9LModeToTest[nDeviceNum][m_nModeIndex].Height;
            m_groupModeToTest[nDeviceNum][0] = m_D3D9LModeToTest[nDeviceNum][m_nModeIndex];

            if ( m_D3D9Lpp[nDeviceNum][0].Windowed == TRUE )
            {
                GetClientRect( m_hwndD3D9LDevWnd[nDeviceNum], &rc );
                m_D3D9Lpp[nDeviceNum][0].BackBufferWidth = rc.right;
                m_D3D9Lpp[nDeviceNum][0].BackBufferHeight = rc.bottom;
            }
            else
            {
                m_D3D9Lpp[nDeviceNum][0].FullScreen_RefreshRateInHz = m_D3D9LModeToTest[nDeviceNum][m_nModeIndex].RefreshRate;
            }
        }
    
        D3DDISPLAYMODEEX   *pFullscreenDisplayMode = NULL;

        if ( m_D3D9Lpp[nDeviceNum][0].Windowed == TRUE )
        {
            pFullscreenDisplayMode = NULL;
        }
        else
        {
            pFullscreenDisplayMode = &m_groupModeToTest[nDeviceNum][0];
        }

        CCommon::GetFocusForFullscreen( m_hwndD3D9LDevWnd[nDeviceNum] );

        if ( FAILED( hr = m_pD3D9L->CreateDeviceEx( nDeviceNum, 
                                                    g_Application.m_nD3D9DriverType, 
                                                    hwndParent,  
                                                    dwBehaviorFlags | D3DCREATE_FPU_PRESERVE, 
                                                    &m_D3D9Lpp[nDeviceNum][0],
                                                    pFullscreenDisplayMode,
                                                    &m_pD3D9LDevice[nDeviceNum]
                                                  ) ) )
	    {
            StringCchPrintf( szMsg, sizeof(szMsg), "%s in %s at line %d.  FAILED: IDirect3D9Ex::CreateDeviceEx returned hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
            CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
            goto Exit;
	    }

        StringCchPrintf(
            szMsg,
            sizeof(szMsg),
            "%d x %d @ %dHz, %d deg.\n",
            m_D3D9Lpp[nDeviceNum][0].BackBufferWidth,
            m_D3D9Lpp[nDeviceNum][0].BackBufferHeight,
            m_D3D9Lpp[nDeviceNum][0].FullScreen_RefreshRateInHz,
            (m_nModeRotation - 1) * 90
            );
        CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
    }

    test_result = RESULT_PASS;

Exit:
    return test_result;
}


BOOL  CTests::ReleaseTestCaseObjects( )
{
    BOOL                fReturn = FALSE;

    if ( !ReleaseD3D9LTestCaseObjects() )
    {
        goto Exit;
    }  

    fReturn = TRUE;

Exit:
    return fReturn;
}

BOOL  CTests::ReleaseD3D9LTestCaseObjects()
{
    BOOL                fReturn = FALSE;
    ULONG               ulRefCt = 0;
    char                szMsg[c_nMSG_LEN] = {0};    


    for ( int i = 0; i < MONITORS_MAX; i++ )
    {
        if ( m_pD3D9LDevice[i] != NULL )
        {
            ulRefCt = m_pD3D9LDevice[i]->Release();
            m_pD3D9LDevice[i] = NULL;

            if ( ulRefCt != 0 )
            {
                StringCchPrintf( szMsg, sizeof(szMsg), "%s in %s at line %d.  FAILED: IDirect3DDevice9Ex::Release() left RefCount = %d\n", __FUNCTION__, __FILE__, __LINE__, ulRefCt );
                CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
                goto Exit;
            }
        }

        if ( m_hwndD3D9LDevWnd[i] )
        {
            DestroyWindow( m_hwndD3D9LDevWnd[i] );
            m_hwndD3D9LDevWnd[i] = NULL;
        }
    }

    if ( m_pD3D9L )
    {
        if ( ( ulRefCt = m_pD3D9L->Release() ) != 0 )
        {
            StringCchPrintf( szMsg, sizeof(szMsg), "%s in %s at line %d.  FAILED: IDirect3D9Ex::Release() left RefCount = %d\n", __FUNCTION__, __FILE__, __LINE__, ulRefCt );
            CCommon::WriteMsgs( szMsg, sizeof(szMsg) );
            goto Exit;
        }

        m_pD3D9L = NULL;
    }

    fReturn = TRUE;

Exit:
    return fReturn;
}

void  CTests::Cleanup()
{

    for ( int i = 0; i < MONITORS_MAX; i++ )
    {
        if ( m_fGotOriginalDevMode[i] )
        { 
            CCommon::SetGDIDevMode( m_originalDispDev[i]/* by ref */, m_originalDevMode[i]/* by ref */ );
        }
    }

    return;
}
