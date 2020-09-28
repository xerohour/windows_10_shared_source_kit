//
// Tests.cpp
//
// Test IDXGIOutput::GetDisplaySurfaceData() to ensure that pixel format conversion is done correctly.
//
// Class definitions:   CScanOutFormats
//                      CTests


//
// Project headers
//

#include "Tests.hpp"
#include "DXGIScanoutFormats.h"

static const UINT   c_nLO_REFRESH = 59;
static const UINT   c_nHI_REFRESH = 74;

extern D3D10_INPUT_ELEMENT_DESC  c_inputElemDescForCheckeredSquare[];
extern UINT       c_nNumElements;
extern VERTEX  c_verticesForCheckeredSquare[];
extern UINT  c_nNumVerticesForCheckeredSquare;

extern const char * const szScanLineNames[] =
{
    "DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED",
    "DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE",
    "DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST",
    "DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST"  
};

extern const char * const szScalingNames[] =
{
    "DXGI_MODE_SCALING_UNSPECIFIED",
    "DXGI_MODE_SCALING_CENTERED",
    "DXGI_MODE_SCALING_STRETCHED"  
};


//  namespace Texture_ALU in TextureCode.cpp
//
namespace Texture_ALU
{

    void 
        WriteData(const TEST_VEC4F* pInputColor, 
        DXGI_FORMAT DataType, 
        BYTE* pOutput);

    void
        ReadDataWithDefaults(const void* pInput, 
        UINT uOffset, 
        DXGI_FORMAT DataType, 
        TEST_VEC4F* pOutputColor);

}

tstring __stdcall ToString( const IID &iid )
{
    if( iid == __uuidof( ID3D10Device ) ) return _T( "ID3D10Device" ) ;
    if( iid == __uuidof( ID3D10Device1 ) ) return _T( "ID3D10Device1" ) ;
    if( iid == __uuidof( ID3D11Device ) ) return _T( "ID3D11Device" ) ;
    return _T( "IUnknown" );
}
bool __stdcall FromString( IID *piid, const tstring &iid )
{
    if ( tcistring(iid) == "ID3D10Device" ) 
    {
        *piid =  __uuidof( ID3D10Device );
        return true;
    }
    if ( tcistring(iid) == "ID3D10Device1" )	
    {
        *piid =  __uuidof( ID3D10Device1 );
        return true;
    }
    if ( tcistring(iid) == "ID3D11Device" )
    {
        *piid =  __uuidof( ID3D11Device );
        return true;
    }
    return false;
}

BEGIN_NAMED_VALUES( TEST_DXGI_SWAP_CASE )
    NAMED_VALUE( _T( "DXGI_SWAP_CASE_PRESENT" ), TEST_DXGI_SWAP_CASE_PRESENT )
    NAMED_VALUE( _T( "DXGI_SWAP_CASE_GETDISPSURFDATA" ), TEST_DXGI_SWAP_CASE_GETDISPSURFDATA )
    NAMED_VALUE( _T( "DXGI_SWAP_CASE_MAP" ), TEST_DXGI_SWAP_CASE_MAP )
    NAMED_VALUE( _T( "DXGI_SWAP_CASE_DEFAULT" ), TEST_DXGI_SWAP_CASE_DEFAULT )
    END_NAMED_VALUES( TEST_DXGI_SWAP_CASE )

    //

    CTests::CTests() :
m_nNumMonitors(0),
    m_nPrimaryOutputNum(0),
    m_nModeIndex(0),
    m_bUseFullscreen(true),
    m_nNoOfDriverSupportedFormats(0),
    m_nWarningsCount(0)
{
    memset( &m_driverSupportedFormats, 0, sizeof(m_driverSupportedFormats) );
    memset( &m_fGotOriginalDevMode[0], 0, sizeof(m_fGotOriginalDevMode) );
    memset( &m_originalDispDev[0], 0, sizeof(m_originalDispDev) );
    memset( &m_originalDevMode[0], 0, sizeof(m_originalDevMode) );
    memset( &m_pD3D10Scene[0][0], 0, sizeof(m_pD3D10Scene) );
    memset( &m_pD3D10VidSurface[0][0], 0, sizeof(m_pD3D10VidSurface) );

    memset( &m_pAdapter[0], 0, sizeof(m_pAdapter) );
    memset( &m_pOutput[0][0], 0, sizeof(m_pOutput) );
    memset( &m_location[0], 0, sizeof(m_location) );
    memset( &m_ModeToTest[0][0][0], 0, sizeof(m_ModeToTest) );
    memset( &m_hwndDevWnd[0][0], 0, sizeof(m_hwndDevWnd) );
    memset( &m_swapChainDesc[0][0], 0, sizeof(m_swapChainDesc) );
    memset( &m_pD3D10Device[0][0], 0, sizeof(m_pD3D10Device) );
    memset( &m_pDXGIDevice[0][0], 0, sizeof(m_pDXGIDevice) );
    memset( &m_pSwapChain[0][0], 0, sizeof(m_pSwapChain) );

    memset( (BYTE *)&m_FactoryPointer, 0, sizeof(m_FactoryPointer) ); 
}

CTests::~CTests()
{
}


bool  CTests::InitializeTestGroup()
{
    //
    //  Log the name, path and moddate of the test app.
    //
    CCommon::LogTestAppInfo();

    DebugOut.Write("Entering InitializeTestGroup.\n");

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
    DebugOut.Write("Entering InitTestParameters.\n");


    //
    // Call the base class implementation.
    //

    CDXGITest::InitTestParameters();


    //
    // Initialize locals.
    //

    LONG            lReturn = 0;
    TCHAR           szMsg[c_nMSG_LEN] = {0};    
    DISPLAY_DEVICE  dispDev = {0};
    DEVMODE         devMode = {0};


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
        AddParameter( _T("PrimaryOutputNum"), 
            &m_nPrimaryOutputNum, 
            RangeValueSet( 0, (m_nNumMonitors - 1), 
            1 ) );  
        AddParameter( _T("ModeIndex"), 
            &m_nModeIndex, 
            RangeValueSet( 0, (int)(c_nFORMAT_MAX) - 1, 
            1 ) );  

        // AddParameter( _T("Fullscreen"), &m_bUseFullscreen, new CBoolValueSet() );
        m_bUseFullscreen = true;    // As of 5/07/07, IDXGIOutput::GetDisplaySurfaceData() only works for Fullscreen, by design.
    }

    CTestCaseParameter< IID > *pEmulatedInterface;
    CUserValueSet< IID > *pInterfaceValueSet = new CUserValueSet< IID >() ;
    pInterfaceValueSet->AddValue( __uuidof( ID3D10Device ) );
    pInterfaceValueSet->AddValue( __uuidof( ID3D11Device ) );

    pEmulatedInterface = AddParameter<IID>( _T("EmulatedInterface"), &(m_EmulatedInterface) );
    AddParameterValueSet< IID >( pEmulatedInterface, pInterfaceValueSet );

    CTestCaseParameter< TEST_DXGI_SWAP_CASE > *pCase;
    CUserValueSet< TEST_DXGI_SWAP_CASE > *pCaseValueSet = new CUserValueSet< TEST_DXGI_SWAP_CASE >() ;
    pCaseValueSet->AddValue( TEST_DXGI_SWAP_CASE_PRESENT );
    pCaseValueSet->AddValue( TEST_DXGI_SWAP_CASE_GETDISPSURFDATA );
    pCaseValueSet->AddValue( TEST_DXGI_SWAP_CASE_MAP );
    pCaseValueSet->AddValue( TEST_DXGI_SWAP_CASE_DEFAULT );
    pCase = AddParameter< TEST_DXGI_SWAP_CASE >( _T("DXGI_SWAP_CASE"), &m_case );
    AddParameterValueSet< TEST_DXGI_SWAP_CASE >( pCase, pCaseValueSet );

    DebugOut.Write("Exiting InitTestParameters.\n");
}


TEST_RESULT  CTests::Setup()
{
    HRESULT         hr = S_OK;
    TEST_RESULT     test_result = RESULT_FAIL;   
    char            szMsg[c_nMSG_LEN] = {0};    
    ULONG           ulRefCt = 0;
    DXGI_MODE_DESC  emptyModeDesc = {0};


    // Is DX10 supported?
    ID3D10Device   *pTempD3D10Device = NULL;
    hr = g_Application.CreateDevice
        (
        NULL,
        g_Application.m_D3DOptions.DriverType,
        NULL,
        D3D10_CREATE_DEVICE_SINGLETHREADED,
        m_TestFeatureLevel,
        0,
        &pTempD3D10Device
        );

    if ( FAILED( hr ) )
    {
        if
            (
            hr == DXGI_ERROR_UNSUPPORTED || 
            hr == E_NOINTERFACE ||
            hr == E_NOTIMPL
            )
        {
            StringCchPrintf
                (
                szMsg,
                ARRAYSIZE( szMsg ),
                "%s at line %d.  The D3D device is not supported. Skipping this test group. hr = 0x%08x\n",
                __FILE__,
                __LINE__,
                hr
                );

            CCommon::WriteMsgs
                (
                szMsg,
                ARRAYSIZE(szMsg)
                );

            test_result = RESULT_SKIP;
        }
        else 
        {
            StringCchPrintf
                (
                szMsg,
                ARRAYSIZE( szMsg ),
                "%s at line %d.  CreateDevice returned hr = 0x%08x\n",
                __FILE__,
                __LINE__,
                hr
                );

            CCommon::WriteMsgs
                (
                szMsg,
                ARRAYSIZE(szMsg)
                );
        }

        goto Exit;
    }

    test_result = EnumerateDisplayDevices();


    // Release DX10
    if ( pTempD3D10Device != NULL )
    {
        if ( ( ulRefCt = pTempD3D10Device->Release() ) != 0 )
        {
            test_result = RESULT_FAIL;
            StringCchPrintf
                (
                szMsg,
                ARRAYSIZE( szMsg ),
                "%s at line %d.  FAILED: ID3D10Device::Release() left RefCount = %d\n",
                __FILE__,
                __LINE__,
                ulRefCt
                );

            CCommon::WriteMsgs
                (
                szMsg,
                ARRAYSIZE(szMsg)
                );

            goto Exit;
        }
    }

    if ( !InitializeWindowClass() )
    {
        test_result = RESULT_FAIL;
        goto Exit;
    }

Exit:
    return test_result;
}


TEST_RESULT  CTests::EnumerateDisplayDevices()
{
    TEST_RESULT     test_result = RESULT_FAIL;   
    HRESULT         hr = S_OK;
    char            szMsg[c_nMSG_LEN] = {0};    
    DXGI_FACTORY_POINTER  FactoryPointer = {NULL};
    IDXGIAdapter   *pTempDXGIAdapter[MONITORS_MAX] = {0};
    IDXGIOutput    *pTempDXGIOutput[MONITORS_MAX][MONITORS_MAX] = {0};
    UINT            nAdapter = 0, nMonitor = 0, k;
    ULONG           ulRefCt = 0;


    //
    // Enumerate and store DX10 Adapters and Outputs
    //

    // Create the DXGI Factory to do some work.
    if ( FAILED( hr = g_Application.CreateDXGIFactory( &FactoryPointer ) ) )
    {
        StringCchPrintf
            (
            szMsg,
            ARRAYSIZE( szMsg ),
            "%s at line %d.  FAILED: g_Application.CreateDXGIFactory failed - hr = 0x%08x\n",
            __FILE__,
            __LINE__,
            hr
            );

        CCommon::WriteMsgs
            (
            szMsg,
            ARRAYSIZE(szMsg)
            );

        goto Exit;
    }

    // Use the factory to enumerate the adapters and save the pointers.
    // There should not be more adapters than there are monitors.
    for ( nAdapter = 0; nAdapter < MONITORS_MAX; nAdapter++ )
    {
        if ( FAILED( hr = FactoryPointer.pFactory->EnumAdapters( nAdapter, &pTempDXGIAdapter[nAdapter] ) ) )
        {
            // If we have found at least one adapter, exit loop successfully.
            if ( nAdapter > 0 )
            {
                break;
            }
            else
            {
                StringCchPrintf
                    (
                    szMsg,
                    ARRAYSIZE( szMsg ),
                    "%s at line %d.  FAILED: IDXGIFactory::EnumAdapters failed - hr = 0x%08x\n",
                    __FILE__,
                    __LINE__,
                    hr
                    );

                CCommon::WriteMsgs
                    (
                    szMsg,
                    ARRAYSIZE(szMsg)
                    );

                goto Exit;
            }
        }
    }

    for ( nAdapter = 0; nAdapter < MONITORS_MAX && pTempDXGIAdapter[nAdapter] != NULL; nAdapter++ )
    {
        for ( nMonitor = 0; nMonitor < MONITORS_MAX; nMonitor++ )
        {
            if ( SUCCEEDED( hr = pTempDXGIAdapter[nAdapter]->EnumOutputs( nMonitor, &pTempDXGIOutput[nAdapter][nMonitor] ) ) )
            {
                if ( ( test_result = DXGIStoreSelectedModes( pTempDXGIOutput[nAdapter][nMonitor], nAdapter, nMonitor ) ) != RESULT_PASS )
                {
                    goto Exit;
                }
            }
            else
            {
                // If we have come to the end of the Outputs or there are none to be found,
                // exit loop successfully.
                if ( hr == DXGI_ERROR_NOT_FOUND )
                {
                    break;
                }
                else
                {
                    StringCchPrintf
                        (
                        szMsg,
                        ARRAYSIZE( szMsg ),
                        "%s at line %d.  FAILED: IDXGIAdapter::EnumOutputs failed - hr = 0x%08x\n",
                        __FILE__,
                        __LINE__,
                        hr
                        );

                    CCommon::WriteMsgs
                        (
                        szMsg,
                        ARRAYSIZE(szMsg)
                        );

                    goto Exit;
                }
            }
        }
    }

    for ( nAdapter = 0; nAdapter < MONITORS_MAX; nAdapter++ )
    {
        for ( nMonitor = 0; nMonitor < MONITORS_MAX; nMonitor++ )
        {
            if ( pTempDXGIOutput[nAdapter][nMonitor] != NULL )
            {
                if ( (ulRefCt = pTempDXGIOutput[nAdapter][nMonitor]->Release() ) != 0 )
                {
                    StringCchPrintf
                        (
                        szMsg,
                        ARRAYSIZE( szMsg ),
                        "%s at line %d.  FAILED: IDXGIAdapter::Release() left RefCount = %d\n",
                        __FILE__,
                        __LINE__,
                        ulRefCt
                        );

                    CCommon::WriteMsgs
                        (
                        szMsg,
                        ARRAYSIZE(szMsg)
                        );

                    goto Exit;
                }
            }
        }

        if ( pTempDXGIAdapter[nAdapter] != NULL )
        {
            if ( (ulRefCt = pTempDXGIAdapter[nAdapter]->Release() ) != 0 ) 
            {
                StringCchPrintf
                    (
                    szMsg,
                    ARRAYSIZE( szMsg ),
                    "%s at line %d.  FAILED: IDXGIAdapter::Release() left RefCount = %d\n",
                    __FILE__,
                    __LINE__,
                    ulRefCt
                    );

                CCommon::WriteMsgs
                    (
                    szMsg,
                    ARRAYSIZE(szMsg)
                    );

                goto Exit;
            }
        }
    }

    if ( ( ulRefCt = FactoryPointer.pFactory->Release() ) != 0 ) 
    {
        StringCchPrintf
            (
            szMsg,
            ARRAYSIZE( szMsg ),
            "%s at line %d.  FAILED: IDXGIFactory::Release() left RefCount = %d\n",
            __FILE__,
            __LINE__,
            ulRefCt
            );

        CCommon::WriteMsgs
            (
            szMsg,
            ARRAYSIZE(szMsg)
            );

        goto Exit;
    }

    test_result = RESULT_PASS;

Exit:

    return test_result;
}


TEST_RESULT
    CTests::DXGIStoreSelectedModes(
    __in    IDXGIOutput*    pTempDXGIOutput,
    __in    UINT            nAdapter,
    __in    UINT            nMonitor
    )
{
    TEST_RESULT     test_result = RESULT_FAIL;   
    HRESULT         hr = S_OK;
    char            szMsg[c_nMSG_LEN] = {0};    
    DXGI_MODE_DESC* pModeDescArray = NULL;
    int             nMode = 0;

    m_nNoOfDriverSupportedFormats = 0;
    memset( &m_driverSupportedFormats, 0, sizeof(m_driverSupportedFormats) );
    for ( UINT uiFormatIndex = 0; uiFormatIndex < clsScanOutFormats::cuiNoOfScanOutFormats; ++uiFormatIndex )
    {
        DXGI_FORMAT             nMatchFormat = clsScanOutFormats::DXGIScanOutFormats[uiFormatIndex];
        UINT                    uiModeCount = 0;

        // Get a count of the available modes for this Output.
        if ( FAILED( hr = pTempDXGIOutput->GetDisplayModeList( nMatchFormat,
            DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING,
            &uiModeCount,
            NULL
            ) ) )
        {
            StringCchPrintf
                (
                szMsg,
                ARRAYSIZE( szMsg ),
                "%s at line %d.  FAILED: IDXGIOutput::GetDisplayModeList failed - hr = 0x%08x\n",
                __FILE__,
                __LINE__,
                hr
                );

            CCommon::WriteMsgs
                (
                szMsg,
                ARRAYSIZE(szMsg)
                );

            goto Exit;
        }

        // No modes found for the current format
        if ( uiModeCount == 0 )
        {
            StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  no supported mode for the color format m_DXGIScanOutFormats[%u].\n", 
                __FILE__, 
                __LINE__,  
                uiFormatIndex);
            CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
            continue;
        }
        else
        {
            StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  %u supported mode(s) for the color format m_DXGIScanOutFormats[%u].\n", 
                __FILE__, 
                __LINE__,  
                uiModeCount, 
                uiFormatIndex);
            CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
        }

        m_driverSupportedFormats[m_nNoOfDriverSupportedFormats] = nMatchFormat;
        m_nNoOfDriverSupportedFormats++;

        if ( !( pModeDescArray = new DXGI_MODE_DESC[uiModeCount] ) )
        {
            StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: Out of memory.  Failed to allocate array.\n", __FILE__, __LINE__ );
            CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
            goto Exit;
        }

        // Get the modes for this Output.
        if ( FAILED( hr = pTempDXGIOutput->GetDisplayModeList( nMatchFormat,
            DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING,
            &uiModeCount,
            pModeDescArray
            ) ) )
        {
            StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGIOutput::GetDisplayModeList failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
            CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
            goto Exit;
        }

        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "Matching format - contains %d modes.\n", (int)uiModeCount );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );

        // Select some edge-case modes
        int   k = 0;  
        UINT  nRefreshRate = 0;

        // Find out the lowest testable resolution:
        UINT    leastPixels = 0;
        UINT    currPixels = 0;
        for ( k = 0; k < (int)uiModeCount; k++ )
        {
            nRefreshRate = pModeDescArray[k].RefreshRate.Numerator / pModeDescArray[k].RefreshRate.Denominator;
            currPixels = pModeDescArray[k].Width * pModeDescArray[k].Height;

            if ( ( leastPixels == 0 || currPixels < leastPixels )
                && nRefreshRate > c_nLO_REFRESH && nRefreshRate < c_nHI_REFRESH )
            {
                m_ModeToTest[nAdapter][nMonitor][nMode] = pModeDescArray[k];
                leastPixels = currPixels;
            }
        }

        nMode++;

        // Find out the highest testable resolution:
        UINT    mostPixels = 0;
        for ( k = 0; k < (int)uiModeCount; k++ )
        {
            nRefreshRate = pModeDescArray[k].RefreshRate.Numerator / pModeDescArray[k].RefreshRate.Denominator;
            currPixels = pModeDescArray[k].Width * pModeDescArray[k].Height;

            if ( currPixels > mostPixels 
                && nRefreshRate > c_nLO_REFRESH && nRefreshRate < c_nHI_REFRESH )
            {
                m_ModeToTest[nAdapter][nMonitor][nMode] = pModeDescArray[k];
                mostPixels = currPixels;
            }
        }

        delete [] pModeDescArray;
        pModeDescArray = NULL;

        nMode++;
    }

    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "DXGIStoreSelectedModes: total modes = %u.\n", (int)nMode );
    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );

    test_result = RESULT_PASS;

Exit:
    if ( pModeDescArray != NULL )
    {
        delete [] pModeDescArray;
    }

    return test_result;
}

//

TEST_RESULT  CTests::ExecuteTestCase()
{
    HRESULT         hr = S_OK;
    TEST_RESULT     test_result = RESULT_FAIL;   
    char            szMsg[c_nMSG_LEN] = {0};    
    IDXGIAdapter   *pNewPrimaryAdapter = NULL;
    IDXGIOutput    *pNewPrimaryOutput = NULL;

    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ),
        "Entering ExecuteTestCase - nPrimaryOutputNum = %d, bUseFullscreen = %d.\n",
        m_nPrimaryOutputNum,
        (UINT)m_bUseFullscreen
        );
    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );


    if ( ( test_result = CreateInitialDXObjects() ) != RESULT_PASS )
    {
        goto Exit;
    }

    if ( ( test_result = GetDXGIAdapterPtr( &pNewPrimaryAdapter ) ) != RESULT_PASS )
    {
        goto Exit;
    }

    if ( ( test_result = GetDXGIOutputPtr( &pNewPrimaryOutput ) ) != RESULT_PASS )
    {
        goto Exit;
    }

    DXGI_OUTPUT_DESC  outputDesc;
    CHAR     szDevName[c_nDEV_NAME_LEN];
    DEVMODE  devMode;

    memset( &outputDesc, 0, sizeof(outputDesc) );
    memset( &szDevName[0], 0, sizeof(szDevName) );
    memset( &devMode, 0, sizeof(devMode) );

    if ( ( test_result = GetMiscOutputData( pNewPrimaryOutput, outputDesc, &szDevName[0], sizeof(szDevName), devMode ) ) != RESULT_PASS )
    {
        goto Exit;
    }

    if ( !CCommon::SetPrimaryMonitor( &szDevName[0], sizeof(szDevName), devMode ) )
    {
        test_result = RESULT_FAIL;
        goto Exit;
    }

    // Changing the Primary Monitor has invalidated the Output objects, so release and recreate them
    if ( !ReleaseTestCaseObjects() )
    {
        test_result = RESULT_FAIL;
        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ReleaseTestCaseObjects() returned FALSE.\n", __FILE__, __LINE__ );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
        goto Exit;
    }  

    if ( ( test_result = CreateInitialDXObjects() ) != RESULT_PASS )
    {
        goto Exit;
    }


    UINT    nAdapter;
    UINT    nMonitor, nMaxMonitors;
    ULONG   ulRefCt = 0;

    // Set up swapchains for each output monitor. no multi-mon for non-HW adapter.
    if (g_Application.m_D3DOptions.DriverType == D3D10_DRIVER_TYPE_SOFTWARE
        ||  g_Application.m_D3DOptions.DriverType == D3D10_DRIVER_TYPE_REFERENCE
        ||  g_Application.m_D3DOptions.DriverType == D3D10_DRIVER_TYPE_WARP)
    {
        nMaxMonitors = 1;
    }
    else
    {
        nMaxMonitors = MONITORS_MAX;
    }

    for ( nAdapter = 0; nAdapter < MONITORS_MAX && m_pAdapter[nAdapter] != NULL; nAdapter++ )
    {
        for ( nMonitor = 0; nMonitor < nMaxMonitors; nMonitor++ )
        {
            if ( m_pOutput[nAdapter][nMonitor] != NULL )
            {
                if ( ( test_result = GetMiscOutputData( m_pOutput[nAdapter][nMonitor], outputDesc, &szDevName[0], sizeof(szDevName), devMode ) ) != RESULT_PASS )
                {
                    goto Exit;
                }

                // Release this IDXGIOutput object and pick up the one created as part of CreateSwapChain().
                if ( (ulRefCt = m_pOutput[nAdapter][nMonitor]->Release() ) != 0 )
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGIAdapter::Release() left RefCount = %d\n", __FILE__, __LINE__, ulRefCt );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                }

                m_pOutput[nAdapter][nMonitor] = NULL;

                if ( ( test_result = DXGISetupSwapChain( nAdapter, nMonitor, devMode, &szDevName[0] ) ) != RESULT_PASS )
                {
                    goto Exit;
                }

                if ( ( m_pD3D10Scene[nAdapter][nMonitor] = new D3D10Scene() ) == NULL )
                {
                    test_result = RESULT_FAIL;
                    StringCchPrintfA( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: Memory allocation for D3D10Scene object failed\n", __FUNCTION__, __FILE__, __LINE__ );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                }

                if ( ( m_pD3D10VidSurface[nAdapter][nMonitor] = new D3D10VidSurface() ) == NULL )
                {
                    test_result = RESULT_FAIL;
                    StringCchPrintfA( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: Memory allocation for D3D10VidSurface object failed\n", __FUNCTION__, __FILE__, __LINE__ );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                } 
            }
        }
    }

    // Present to all the swapchains.
    for ( nAdapter = 0; nAdapter < MONITORS_MAX && m_pAdapter[nAdapter] != NULL; nAdapter++ )
    {
        for ( nMonitor = 0; nMonitor < nMaxMonitors; nMonitor++ )
        {
            if ( m_pOutput[nAdapter][nMonitor] != NULL )
            {
                if ( ( test_result = PresentAndValidate( nAdapter, nMonitor ) ) == RESULT_FAIL )
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: PresentAndValidate() failed\n", __FILE__, __LINE__ );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                }
            }
        }
    }

Exit:

    DeleteD3D10SceneObjects();

    if ( !ReleaseTestCaseObjects() )
    {
        test_result = RESULT_FAIL;
        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ReleaseTestCaseObjects() returned FALSE.\n", __FILE__, __LINE__ );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
    }  

    DebugOut.Write("Exiting ExecuteTestCase.\n");

    return test_result;
}


TEST_RESULT  CTests::CreateInitialDXObjects()
{
    TEST_RESULT     test_result = RESULT_FAIL;   
    HRESULT         hr = S_OK;
    char            szMsg[c_nMSG_LEN] = {0};    
    UINT            i, j, k;

    //
    // Enumerate DX10 Adapters and Outputs
    // Always load this so DXGI can help with changing the Primary monitor.
    //
    // Create the DXGI Factory to do some work.
    if ( FAILED( hr = g_Application.CreateDXGIFactory( &m_FactoryPointer ) ) )
    {
        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: g_Application.CreateDXGIFactory failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
        goto Exit;
    }

    // Use the factory to enumerate the adapters and save the pointers.
    // There should not be more adapters than there are monitors.
    for ( i = 0; i < MONITORS_MAX; i++ )
    {
        if ( FAILED( hr = m_FactoryPointer.pFactory->EnumAdapters( i, &m_pAdapter[i] ) ) )
        {
            // If we have found at least one adapter, exit loop successfully.
            if ( i > 0 )
            {
                break;
            }
            else
            {
                StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGIFactory::EnumAdapters failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
                CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                goto Exit;
            }
        }
    }

    for ( i = 0, k = 0; i < MONITORS_MAX && m_pAdapter[i] != NULL; i++ )
    {
        for ( j = 0; j < MONITORS_MAX; j++ )
        {
            if ( SUCCEEDED( hr = m_pAdapter[i]->EnumOutputs( j, &m_pOutput[i][j] ) ) )
            {
                // Cache the location of the Output pointer.
                if ( k < MONITORS_MAX )
                {
                    m_location[k].i = i;
                    m_location[k].j = j;
                    k++;
                }
                else
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: More than %d Outputs\n", __FILE__, __LINE__, MONITORS_MAX );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                }
            }
            else
            {
                // If we have come to the end of the Outputs or there are none to be found,
                // exit loop successfully.
                if ( hr == DXGI_ERROR_NOT_FOUND )
                {
                    break;
                }
                else
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGIAdapter::EnumOutputs failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                }
            }
        }
    }

    test_result = RESULT_PASS;

Exit:

    return test_result;
}


//
TEST_RESULT
    CTests::GetDXGIAdapterPtr( 
    __inout     IDXGIAdapter**   ppAdapter
    )
{
    TEST_RESULT     test_result = RESULT_FAIL;   
    char            szMsg[c_nMSG_LEN] = {0};    

    *ppAdapter = m_pAdapter[m_location[m_nPrimaryOutputNum].i];

    if ( *ppAdapter == NULL )
    {
        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: Adapter # %d is NULL\n", __FILE__, __LINE__, m_nPrimaryOutputNum );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
        goto Exit;
    }

    test_result = RESULT_PASS;

Exit:
    return test_result;
}

//
TEST_RESULT
    CTests::GetDXGIOutputPtr( 
    __inout     IDXGIOutput**   ppOutput
    )
{
    TEST_RESULT     test_result = RESULT_FAIL;   
    char            szMsg[c_nMSG_LEN] = {0};    


    *ppOutput = m_pOutput[m_location[m_nPrimaryOutputNum].i][m_location[m_nPrimaryOutputNum].j];

    if ( *ppOutput == NULL )
    {
        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: Output # %d is NULL\n", __FILE__, __LINE__, m_nPrimaryOutputNum );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
        goto Exit;
    }

    test_result = RESULT_PASS;

Exit:
    return test_result;
}


//
TEST_RESULT
    CTests::GetMiscOutputData(
    __in        IDXGIOutput*    pOutput,
    __inout     DXGI_OUTPUT_DESC&  outputDesc,  // by ref
    __inout_ecount(nDevNameLen)  PCHAR  pszDevName,
    __in        UINT            nDevNameLen,
    __inout     DEVMODE&        devMode         // by ref
    )
{
    TEST_RESULT         test_result = RESULT_FAIL;
    HRESULT             hr = S_OK;
    char                szMsg[c_nMSG_LEN] = {0};    


    if ( FAILED( hr = pOutput->GetDesc( &outputDesc ) ) )
    {
        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGIOutput::GetDesc failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
        goto Exit;
    }

    WideCharToMultiByte(CP_ACP, 0, &outputDesc.DeviceName[0], ARRAYSIZE(outputDesc.DeviceName), pszDevName, nDevNameLen, 0, 0);

    if ( !CCommon::GetGDIDevMode( pszDevName, c_nDEV_NAME_LEN, devMode ) )
    {
        goto Exit;
    }

    test_result = RESULT_PASS;

Exit:
    return test_result;
}


TEST_RESULT
    CTests::DXGISetupSwapChain(
    __in    UINT            nAdapter,
    __in    UINT            nMonitor,
    __in    DEVMODE&        devMode,            // by ref
    __in __nullterminated  PCHAR  pszDevName
    )
{
    TEST_RESULT         test_result = RESULT_FAIL;
    BOOL                bReturn = FALSE;
    LONG                lReturn = 0;
    HRESULT             hr = S_OK;
    char                szMsg[c_nMSG_LEN] = {0};    
    char                szTempMsg[c_nMSG_LEN] = {0};    
    BOOL                bFullScreen = FALSE;
    UINT                uiRefreshRate = 0;      // Calculated integer Refresh Rate
    const DXGIFormatInfo *pEnumeratedModeFormatRecord = 0;

    if ( m_ModeToTest[nAdapter][nMonitor][m_nModeIndex].Width == 0 
        || m_ModeToTest[nAdapter][nMonitor][m_nModeIndex].Height == 0 )
    {
        // don't need to display message here. the skipped color formats are displayed in DXGIStoreSelectedModes()
        test_result = RESULT_SKIP;
        goto Exit;
    }

    devMode.dmFields = 0;

    // If the DXGI mode is interlaced (as opposed to the default progressive), set dmDisplayFlags.
    if ( m_ModeToTest[nAdapter][nMonitor][m_nModeIndex].ScanlineOrdering == DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST
        || m_ModeToTest[nAdapter][nMonitor][m_nModeIndex].ScanlineOrdering == DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST )
    {
        devMode.dmDisplayFlags = DM_INTERLACED;
    }
    else
    {
        if ( m_ModeToTest[nAdapter][nMonitor][m_nModeIndex].ScanlineOrdering == DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE )
        {
            devMode.dmDisplayFlags = 0;
        }
    }

    devMode.dmDisplayFrequency = 0;

    devMode.dmFields |= DM_DISPLAYORIENTATION;
    devMode.dmFields |= DM_PELSWIDTH;
    devMode.dmFields |= DM_PELSHEIGHT;
    devMode.dmFields |= DM_DISPLAYFLAGS;
    devMode.dmFields |= DM_DISPLAYFREQUENCY;

    if ( ( lReturn = ChangeDisplaySettingsEx(pszDevName, 
        &devMode, 
        NULL, 
        CDS_UPDATEREGISTRY, 
        NULL ) ) != DISP_CHANGE_SUCCESSFUL )
    {
        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ChangeDisplaySettingsEx() returned %d\n", __FILE__, __LINE__, lReturn );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
        goto Exit;
    }

    // Create a focus / device window.
    if ( m_hwndDevWnd[nAdapter][nMonitor] = CreateWindowEx( WS_EX_TOPMOST,
        m_szFocusWindowClassName,
        "DXGI Device Window",
        WS_OVERLAPPEDWINDOW,
        ( devMode.dmPosition.x + ( devMode.dmPelsWidth / 16 ) ),   // Position window on secondary desktop if secondary
        ( devMode.dmPosition.y + ( devMode.dmPelsHeight / 16 ) ),  // is being tested.
        ( devMode.dmPelsWidth / 2 ),   
        ( devMode.dmPelsHeight / 2 ),
        NULL,
        NULL,
        m_hProgInst,
        NULL
        ) )
    {
        ShowWindow( m_hwndDevWnd[nAdapter][nMonitor], SW_SHOWNORMAL );

        hr = g_Application.CreateDevice
            (
            (g_Application.m_D3DOptions.DriverType != D3D10_DRIVER_TYPE_HARDWARE)? NULL: m_pAdapter[nAdapter],
            g_Application.m_D3DOptions.DriverType,
            NULL,
            0,
            m_TestFeatureLevel,
            0,
            &m_pD3D10Device[nAdapter][nMonitor]
        );

        if ( FAILED( hr ) )
        {
            if ( hr == DXGI_ERROR_UNSUPPORTED || hr == E_NOINTERFACE )
            {
                StringCchPrintf
                    (
                    szMsg,
                    ARRAYSIZE( szMsg ),
                    "%s at line %d.  The D3D device is not supported. Skipping this test group. hr = 0x%08x\n",
                    __FILE__,
                    __LINE__,
                    hr
                    );

                CCommon::WriteMsgs
                    (
                    szMsg,
                    ARRAYSIZE(szMsg)
                    );

                test_result = RESULT_SKIP;
            }
            else 
            {   
                StringCchPrintf
                    (
                    szMsg,
                    ARRAYSIZE( szMsg ),
                    "%s at line %d.  CreateDevice returned hr = 0x%08x\n",
                    __FILE__,
                    __LINE__,
                    hr
                    );

                CCommon::WriteMsgs
                    (
                    szMsg,
                    ARRAYSIZE(szMsg)
                    );
            }
            goto Exit;
        }

        // Associate the focus window with the DXGI factory.
        if ( FAILED( hr = m_FactoryPointer.pFactory->MakeWindowAssociation( m_hwndDevWnd[nAdapter][nMonitor], 0 ) ) )
        {
            StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGIFactory::MakeWindowAssociation failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
            CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
            goto Exit;
        }

        // check if the color format supported
        memset( &m_swapChainDesc[nAdapter][nMonitor], 0, sizeof(m_swapChainDesc[nAdapter][nMonitor]) );
        m_swapChainDesc[nAdapter][nMonitor].BufferDesc = m_ModeToTest[nAdapter][nMonitor][m_nModeIndex];
        UINT FormatSupport = 0;
        hr = m_pD3D10Device[nAdapter][nMonitor]->CheckFormatSupport(m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Format,
            &FormatSupport);

        if ( hr == E_FAIL )
        {
            StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  The device is not supported - hr = 0x%08x\n", __FILE__, __LINE__, hr );
            CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );

            test_result = RESULT_SKIP;
            goto Exit;
        }
        else if ( FAILED( hr ) )
        {
            StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  Failed: CheckFormatSupport() failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
            CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
            goto Exit;
        };

        UINT RequiredFormatSupport = D3D10_FORMAT_SUPPORT_TEXTURE2D |
            D3D10_FORMAT_SUPPORT_RENDER_TARGET |
            D3D10_FORMAT_SUPPORT_DISPLAY;

        if ( ( FormatSupport & RequiredFormatSupport ) != RequiredFormatSupport )
        {
            StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  The color format is not supported - hr = 0x%08x\n", __FILE__, __LINE__, hr );
            CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );

            test_result = RESULT_SKIP;
            goto Exit;
        };

        uiRefreshRate = 0;   // Calculated integer Refresh Rate.

        // Specify the swap chain properties.
        memset( &m_swapChainDesc[nAdapter][nMonitor], 0, sizeof(m_swapChainDesc[nAdapter][nMonitor]) );
        m_swapChainDesc[nAdapter][nMonitor].BufferDesc = m_ModeToTest[nAdapter][nMonitor][m_nModeIndex];
        uiRefreshRate = (UINT)( (float)m_swapChainDesc[nAdapter][nMonitor].BufferDesc.RefreshRate.Numerator / (float)m_swapChainDesc[nAdapter][nMonitor].BufferDesc.RefreshRate.Denominator );
        m_swapChainDesc[nAdapter][nMonitor].Windowed = (BOOL)!m_bUseFullscreen;

        RECT    rc;

        if ( m_swapChainDesc[nAdapter][nMonitor].Windowed == TRUE )
        {
            GetClientRect( m_hwndDevWnd[nAdapter][nMonitor], &rc );
            m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Width = rc.right;
            m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Height = rc.bottom;
        }

        pEnumeratedModeFormatRecord = CFormatInfoMap::Find( m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Format );

        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ),
            "Adapter %d, Monitor %d: CreateSwapChain() on %s specifying %d x %d @ %dHz (%s); %s; %s\n",
            nAdapter,
            nMonitor,
            pszDevName,
            m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Width,
            m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Height,
            uiRefreshRate,
            pEnumeratedModeFormatRecord ? pEnumeratedModeFormatRecord->szDesc : "Unrecognized format",
            szScanLineNames[(UINT)(m_swapChainDesc[nAdapter][nMonitor].BufferDesc.ScanlineOrdering)],
            szScalingNames[(UINT)(m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Scaling)]
        );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );

        m_swapChainDesc[nAdapter][nMonitor].SampleDesc.Count = 1;
        m_swapChainDesc[nAdapter][nMonitor].SampleDesc.Quality = 0;

        m_swapChainDesc[nAdapter][nMonitor].BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        m_swapChainDesc[nAdapter][nMonitor].BufferCount = c_nBACK_BUFFER_COUNT;
        m_swapChainDesc[nAdapter][nMonitor].OutputWindow = m_hwndDevWnd[nAdapter][nMonitor];
        m_swapChainDesc[nAdapter][nMonitor].SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        m_swapChainDesc[nAdapter][nMonitor].Flags = 0;

        // Retrieve the equivalent IDXGIDevice object.
        if ( FAILED(hr = m_pD3D10Device[nAdapter][nMonitor]->QueryInterface( __uuidof(IDXGIDevice), (PVOID*)&m_pDXGIDevice[nAdapter][nMonitor] ) ) )
        {
            StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ID3D10Device::QueryInterface failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
            CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
            goto Exit;
        }

        CCommon::GetFocusForFullscreen( m_hwndDevWnd[nAdapter][nMonitor] );
        CCommon::GetGDIDevMode( pszDevName, c_nDEV_NAME_LEN, devMode );

        // Create the swap chain.
        hr = m_FactoryPointer.pFactory->CreateSwapChain(m_pDXGIDevice[nAdapter][nMonitor],
            &m_swapChainDesc[nAdapter][nMonitor],
            &m_pSwapChain[nAdapter][nMonitor]);
        if (FAILED(hr))
        {
            if ( hr == DXGI_ERROR_UNSUPPORTED || hr == E_INVALIDARG )
            {
                StringCchPrintf(szMsg, 
                    ARRAYSIZE( szMsg ), 
                    "%s at line %d.  m_FactoryPointer.pFactory->CreateSwapChain doesn't support the color format 08%08x.\n",
                    __FILE__,
                    __LINE__,
                    m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Format);

                CCommon::WriteMsgs(szMsg, ARRAYSIZE(szMsg));
                test_result = RESULT_SKIP;
            }
            else
            {
                StringCchPrintf(szMsg, 
                    ARRAYSIZE( szMsg ), 
                    "%s at line %d.  FAILED: IDXGIFactory::CreateSwapChain() failed - hr = 0x%08x\n", 
                    __FILE__, 
                    __LINE__, 
                    hr);
                CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
            }
            goto Exit;
        }

        if ( FAILED( hr = m_pSwapChain[nAdapter][nMonitor]->GetContainingOutput( &m_pOutput[nAdapter][nMonitor] ) ) )
        {
            StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGISwapChain::GetContainingOutput failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
            CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
            goto Exit;
        }

        CCommon::PumpWindowMsgs( m_hwndDevWnd[nAdapter][nMonitor], TRUE, 70 );
        CCommon::GetGDIDevMode( pszDevName, c_nDEV_NAME_LEN, devMode );

        if ( FAILED( hr = m_pSwapChain[nAdapter][nMonitor]->GetFullscreenState( &bFullScreen, NULL )) )
        {
            StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGISwapChain::GetFullscreenState() failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
            CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
            goto Exit;
        }
        else
            if ( bFullScreen != (BOOL)m_bUseFullscreen )
            {
                StringCchPrintf(
                    szTempMsg,
                    ARRAYSIZE(szTempMsg),
                    "%d x %d @ %dHz (%s); %s; %s\n",
                    m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Width,
                    m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Height,
                    uiRefreshRate,
                    pEnumeratedModeFormatRecord ? pEnumeratedModeFormatRecord->szDesc : "Unrecognized format",
                    szScanLineNames[(UINT)(m_swapChainDesc[nAdapter][nMonitor].BufferDesc.ScanlineOrdering)],
                    szScalingNames[(UINT)(m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Scaling)]
                );

                if ( bFullScreen == FALSE )
                { 
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ),
                        "%s at line %d.  FAILED: Mode is not Full-Screen - %s.\n",
                        __FILE__, __LINE__,
                        szTempMsg
                        );
                }
                else
                { 
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ),
                        "%s at line %d.  FAILED: Mode is not Windowed - %s.\n",
                        __FILE__, __LINE__,
                        szTempMsg
                        );
                }

                CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                goto Exit;
            }
    }

    test_result = RESULT_PASS;

Exit:
    return test_result;
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


TEST_RESULT
    CTests::PresentAndValidate(
    __in    UINT            nAdapter,
    __in    UINT            nMonitor
    )
{
    HRESULT                 hr = S_OK;
    TEST_RESULT             test_result = RESULT_PASS;
    int						iTestFailCount = 0;
    BOOL                    fOK = FALSE;
    char                    szMsg[c_nMSG_LEN] = {0};    
    int                     n8BPPColorChannelValue = 0;
    float                   flColorIntensity = 0.0f;

    // test loop local objects
    IDXGISurface           *pTestSurface = NULL;
    DXGI_SURFACE_DESC       surfDesc;
    ID3D10Texture2D        *pValidationVidMemTex = NULL;
    ID3D10Texture2D        *pValidationSysMemTex = NULL;
    ID3D10Texture2D        *pManualValidationSysMemTex = NULL;
    D3D10_TEXTURE2D_DESC    tex2DSysDesc;
    ID3D10Resource         *pValidationVidMemRes = NULL;
    ID3D10Resource         *pValidationSysMemRes = NULL;	

    D3DWrappedDevice* pWrappedD3DDevice = new D3DWrappedDevice( m_pD3D10Device[nAdapter][nMonitor] );
    if ( FAILED ( hr = pWrappedD3DDevice->Initialize( m_EmulatedInterface ) ) )
        goto Exit;

    // Render a range of checkerboard test colors to the backbuffer
    //
    if ( FAILED ( hr = m_pD3D10Scene[nAdapter][nMonitor]->Initialize( pWrappedD3DDevice,
        m_pSwapChain[nAdapter][nMonitor],
        m_swapChainDesc[nAdapter][nMonitor] ) ) )
	{
		goto Exit;
	}

	if ( FAILED ( hr = m_pD3D10VidSurface[nAdapter][nMonitor]->Initialize( pWrappedD3DDevice,
        m_pSwapChain[nAdapter][nMonitor],
        m_swapChainDesc[nAdapter][nMonitor] ) ) )
	{
		goto Exit;
	}


    // Testing only the max color intensity for each channel for now
    //
    for ( n8BPPColorChannelValue = 256; n8BPPColorChannelValue > 0; n8BPPColorChannelValue -= 256 )  
    {
        flColorIntensity = (float)n8BPPColorChannelValue / 256.0f;

        // Render to the back buffer and present it
        //
        if ( FAILED ( hr = m_pD3D10Scene[nAdapter][nMonitor]->Create3DState( flColorIntensity ) ) )
        {
            goto Exit;
        }

        if ( FAILED ( hr = m_pD3D10Scene[nAdapter][nMonitor]->RenderScene() ) )
        {
            goto Exit;
        }

        if( m_case == TEST_DXGI_SWAP_CASE_PRESENT )
        {
            if( FAILED( hr = pWrappedD3DDevice->SwapToDeviceContextState_OtherMode() ) )
            {
                StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: D3DWrappedDevice::SwapToDeviceContextState_OtherMode failed", __FILE__, __LINE__ );
                CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                goto Exit;
            }
        }
        if ( FAILED ( hr = m_pD3D10Scene[nAdapter][nMonitor]->PresentScene() ) )
        {
            goto Exit;
        }
        if( m_case == TEST_DXGI_SWAP_CASE_PRESENT )
        {
            if( FAILED( hr = pWrappedD3DDevice->SwapToEmulatedDeviceContextState() ) )
            {
                StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: D3DWrappedDevice::SwapToEmulatedDeviceContextState failed", __FILE__, __LINE__ );
                CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                goto Exit;
            }
        }

        // Test GetDisplaySurfaceData()
        //
        DXGI_MAPPED_RECT        mappedTestBits;
        D3D10_MAPPED_TEXTURE2D  mappedSysMemTex2D;


        // Verify the test case passes
        DXGI_FORMAT     nFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        int             w = 0, h = 0;           // for iterating width and height
        int             w_iter = 0, h_iter = 0; // for tracking how many iterations we have done

        // Test on a variety of sizes
        // Start with existing Width, then on 2nd iteration, continue with a number which is a multiple of 2
        for ( w = m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Width,   w_iter = 0;
            w <= (int)m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Width || w_iter < 3;
            w *= 2,  w_iter++ )
        {
            if ( w_iter == 1 )
            {
                w = 256;
            }

            // Ensure that dimension is multiple of 2 so a simple 4-block checker pattern can be generated.
            //
            if ( w % 2 > 0 )
            {
                w = w / 2 * 2;
            }

            for ( h = m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Height,  h_iter = 0;
                h <= (int)m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Height || h_iter < 3;
                h *= 2,  h_iter++ )
            {
                if ( h_iter == 1 )
                {
                    h = 256;
                }

                // Ensure that dimension is multiple of 2 so a simple 4-block checker pattern can be generated.
                //
                if ( h % 2 > 0 )
                {
                    h = h / 2 * 2;
                }

                // Create temporary DXGI Test surface 
                //
                memset( &surfDesc, 0, sizeof(surfDesc) );
                surfDesc.Width = w;
                surfDesc.Height = h;
                surfDesc.Format = nFormat;                                  
                surfDesc.SampleDesc = m_swapChainDesc[nAdapter][nMonitor].SampleDesc; 
                pTestSurface = NULL;

                if ( FAILED(hr = m_pDXGIDevice[nAdapter][nMonitor]->CreateSurface( &surfDesc, 1, DXGI_CPU_ACCESS_SCRATCH, NULL, &pTestSurface ) ) )
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGIDevice::CreateSurface failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                }

                // Create temporary D3D10 Validation texture #1 in vidmem
                // and render to the Validation surface.  #1 is a 3D-rendered validation texture.
                //
                if ( FAILED ( hr = m_pD3D10VidSurface[nAdapter][nMonitor]->Create3DState( m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Width, m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Height, nFormat, flColorIntensity ) ) )
                {
                    goto Exit;
                }

                if ( FAILED ( hr = m_pD3D10VidSurface[nAdapter][nMonitor]->RenderScene( &pValidationVidMemTex ) ) )
                {
                    goto Exit;
                }  

                // Create temporary D3D10 Validation texture #1 in sysmem
                //
                memset( &tex2DSysDesc, 0, sizeof(tex2DSysDesc) );
                tex2DSysDesc.Width = w;
                tex2DSysDesc.Height = h;
                tex2DSysDesc.MipLevels = 1;
                tex2DSysDesc.ArraySize = 1;
                tex2DSysDesc.Format = nFormat;
                tex2DSysDesc.SampleDesc.Count = 1;
                tex2DSysDesc.Usage = D3D10_USAGE_STAGING;
                tex2DSysDesc.BindFlags = 0;
                tex2DSysDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;

                if ( FAILED( hr = m_pD3D10Device[nAdapter][nMonitor]->CreateTexture2D( &tex2DSysDesc, NULL, &pValidationSysMemTex ) ) )
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ID3D10Device::CreateTexture2D failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                }

                // Get the ID3D10Resource interfaces for the two textures, then do a CopyResource()
                //
                if ( FAILED( hr = pValidationVidMemTex->QueryInterface( __uuidof(ID3D10Resource), (PVOID*)&pValidationVidMemRes ) ) )
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ID3D10Device::QueryInterface failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                }

                if ( FAILED( hr = pValidationSysMemTex->QueryInterface( __uuidof(ID3D10Resource), (PVOID*)&pValidationSysMemRes ) ) )
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ID3D10Device::QueryInterface failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                }

                D3DX10_TEXTURE_LOAD_INFO tlinfo;
                tlinfo.pSrcBox 		= 0;
                tlinfo.pDstBox 		= 0;
                tlinfo.SrcFirstMip 	= 0;
                tlinfo.DstFirstMip 	= 0;
                tlinfo.NumMips 		= D3DX10_DEFAULT;
                tlinfo.SrcFirstElement = 0;
                tlinfo.DstFirstElement = 0;
                tlinfo.NumElements 	= D3DX10_DEFAULT;
                tlinfo.Filter  		= D3DX10_FILTER_LINEAR | D3DX10_FILTER_MIRROR_U | D3DX10_FILTER_MIRROR_V ;	// Don't wrap
                tlinfo.MipFilter   	= D3DX10_FILTER_LINEAR | D3DX10_FILTER_MIRROR_U | D3DX10_FILTER_MIRROR_V;

                if( FAILED( hr = pWrappedD3DDevice->CopyTextureToTexture( &pValidationSysMemTex, pValidationVidMemTex, tlinfo ) ) )
                {
                    goto Exit;
                }


                // Create temporary D3D10 Validation texture #2 in sysmem, looping thru all pixels and setting them manually. 
                //
                if ( FAILED( hr = CreateManualSysmemTexture( nAdapter,
                    nMonitor,
                    w,
                    h,
                    nFormat,
                    flColorIntensity,
                    &pManualValidationSysMemTex
                    ) ) )
                {
                    goto Exit;
                }

                // API TEST: GETDISPLAYSURFACEDATA
                //
                // Copy the data from the Test back buffer to the Test surface, converting if needed to a different format
                //
                if( m_case == TEST_DXGI_SWAP_CASE_GETDISPSURFDATA )
                {
                    if( FAILED( hr = pWrappedD3DDevice->SwapToDeviceContextState_OtherMode() ) )
                    {
                        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: D3DWrappedDevice::SwapToDeviceContextState_OtherMode failed", __FILE__, __LINE__ );
                        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                        test_result = RESULT_FAIL;
                        goto Exit;
                    }
                }
                if ( FAILED( hr = m_pOutput[nAdapter][nMonitor]->GetDisplaySurfaceData( pTestSurface ) ) )
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGIOutput::GetDisplaySurfaceData failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    test_result = RESULT_FAIL;
                    goto Exit;
                }
                if( m_case == TEST_DXGI_SWAP_CASE_GETDISPSURFDATA )
                {
                    if( FAILED( hr = pWrappedD3DDevice->SwapToEmulatedDeviceContextState() ) )
                    {
                        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: D3DWrappedDevice::SwapToEmulatedDeviceContextState failed", __FILE__, __LINE__ );
                        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                        test_result = RESULT_FAIL;
                        goto Exit;
                    }
                }

                // Map the Test surface
                memset( &mappedTestBits, 0, sizeof(mappedTestBits) );
                if( m_case == TEST_DXGI_SWAP_CASE_MAP )
                {
                    if( FAILED( hr = pWrappedD3DDevice->SwapToDeviceContextState_OtherMode() ) )
                    {
                        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: D3DWrappedDevice::SwapToDeviceContextState_OtherMode failed", __FILE__, __LINE__ );
                        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                        test_result = RESULT_FAIL;
                        goto Exit;
                    }
                }				
                if ( FAILED( hr = pTestSurface->Map( &mappedTestBits, DXGI_MAP_READ | DXGI_MAP_WRITE ) ) )
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGISurface::Map failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    test_result = RESULT_FAIL;
                    goto Exit;
                }
                if( m_case == TEST_DXGI_SWAP_CASE_MAP )
                {
                    if( FAILED( hr = pWrappedD3DDevice->SwapToEmulatedDeviceContextState() ) )
                    {
                        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: D3DWrappedDevice::SwapToEmulatedDeviceContextState failed", __FILE__, __LINE__ );
                        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                        test_result = RESULT_FAIL;
                        goto Exit;
                    }
                }

                // Map the Validation texture #1
                memset( &mappedSysMemTex2D, 0, sizeof(mappedSysMemTex2D) );

                if ( FAILED( hr = pValidationSysMemTex->Map( 0, D3D10_MAP_READ, 0, &mappedSysMemTex2D ) ) )
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ID3D10Texture2D::Map failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    test_result = RESULT_FAIL;
                    goto Exit;
                }

                // Compare the GetDisplaySurfaceData() Test surface and the Validation texture surface #1
                //
                if ( memcmp( mappedTestBits.pBits, mappedSysMemTex2D.pData, mappedTestBits.Pitch * surfDesc.Height ) )
                {
                    if ( ( test_result = FuzzyCompareDXGISurfaces( nFormat,
                        surfDesc,
                        m_swapChainDesc[nAdapter][nMonitor],
                        mappedTestBits,
                        mappedSysMemTex2D
                        ) ) != RESULT_PASS ) 
                    {
                        if ( test_result == RESULT_SKIP && m_nWarningsCount > 1 )
                        {
                            ;   // do nothing
                        }
                        else
                        {
                            iTestFailCount++;
                            const DXGIFormatInfo *pEnumeratedModeFormatRecord = CFormatInfoMap::Find( nFormat );

                            StringCchPrintf( szMsg, ARRAYSIZE( szMsg ),
                                "3D-generated Texture - Format: %s at %d x %d w:%d w_iter:%d h:%d h_iter:%d ColorChannelValue:%d \n",
                                pEnumeratedModeFormatRecord ? pEnumeratedModeFormatRecord->szDesc : "Unrecognized format",
                                m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Width,
                                m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Height,
                                w, w_iter, h, h_iter, n8BPPColorChannelValue
                                );
                            CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );

                            TCHAR DumpFileName[ MAX_PATH ];
                            StringCchPrintf
                                (
                                DumpFileName,
                                ARRAYSIZE( DumpFileName ),
                                _T( "DXGIGetDispSurf_testcase%u_ValidationSysMemTex_%u_%u_%u_%u_%u_Test.bmp" ),
                                g_Application.GetCurrentTestCase(), w, w_iter, h, h_iter, n8BPPColorChannelValue
                                );

                            if( SUCCEEDED( pWrappedD3DDevice->SwapToOriginalDeviceContextState() ) )
                            {
                                D3DX10SaveTextureToFile
                                    (
                                    (ID3D10Resource *) pValidationSysMemTex,
                                    D3DX10_IFF_BMP,
                                    DumpFileName
                                    );
                                pWrappedD3DDevice->SwapToEmulatedDeviceContextState();
                            }
                            else
                            {						
                                StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: D3DWrappedDevice::SwapToOriginalDeviceContextState failed", __FILE__, __LINE__ );
                                CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                            }

                            ID3D10Texture2D *pTempTex;
                            HRESULT hrQI;

                            if( SUCCEEDED( hrQI = pTestSurface->QueryInterface( __uuidof(ID3D10Resource), (void**)&pTempTex ) ))
                            {
                                // Test Surface
                                StringCchPrintf
                                    (
                                    DumpFileName,
                                    ARRAYSIZE( DumpFileName ),
                                    _T( "DXGIGetDispSurf_testcase%u_TestSurface_%u_%u_%u_%u_%u_Test.bmp" ),
                                    g_Application.GetCurrentTestCase(), w, w_iter, h, h_iter, n8BPPColorChannelValue
                                    );
                                if( SUCCEEDED( pWrappedD3DDevice->SwapToOriginalDeviceContextState() ) )
                                {
                                    D3DX10SaveTextureToFile
                                        (
                                        (ID3D10Resource *) pTempTex,
                                        D3DX10_IFF_BMP,
                                        DumpFileName
                                        );
                                    pWrappedD3DDevice->SwapToEmulatedDeviceContextState();
                                }
                                else
                                {						
                                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: D3DWrappedDevice::SwapToOriginalDeviceContextState failed", __FILE__, __LINE__ );
                                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                                }
                                SAFE_RELEASE( pTempTex );
                            }
                            else
                            {
                                WriteToLog
                                    (
                                    "QueryInterface for ID3D10Texture2D from IDXGISurface failed with error 0x%x.",
                                    hrQI
                                    );
                            }
                        }
                    }
                }

                pValidationSysMemTex->Unmap( 0 );


                // Map the Validation texture #2
                memset( &mappedSysMemTex2D, 0, sizeof(mappedSysMemTex2D) );

                if ( FAILED( hr = pManualValidationSysMemTex->Map( 0, D3D10_MAP_READ, 0, &mappedSysMemTex2D ) ) )
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ID3D10Texture2D::Map failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                }

                // Compare the GetDisplaySurfaceData() Test surface and the Validation texture surface #2
                //
                if ( memcmp( mappedTestBits.pBits, mappedSysMemTex2D.pData, mappedTestBits.Pitch * surfDesc.Height ) )
                {
                    if ( ( test_result = FuzzyCompareDXGISurfaces( nFormat,
                        surfDesc,
                        m_swapChainDesc[nAdapter][nMonitor],
                        mappedTestBits,
                        mappedSysMemTex2D
                        ) ) != RESULT_PASS ) 
                    {
                        if ( test_result == RESULT_SKIP && m_nWarningsCount > 1 )
                        {
                            ;   // do nothing
                        }
                        else
                        {
                            iTestFailCount++;
                            const DXGIFormatInfo *pEnumeratedModeFormatRecord = CFormatInfoMap::Find( nFormat );

                            StringCchPrintf( szMsg, ARRAYSIZE( szMsg ),
                                "Loop-generated Texture - Format: %s at %d x %d\n",
                                pEnumeratedModeFormatRecord ? pEnumeratedModeFormatRecord->szDesc : "Unrecognized format",
                                m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Width,
                                m_swapChainDesc[nAdapter][nMonitor].BufferDesc.Height
                                );
                            CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );

                            TCHAR DumpFileName[ MAX_PATH ];
                            StringCchPrintf
                                (
                                DumpFileName,
                                ARRAYSIZE( DumpFileName ),
                                _T( "DXGIGetDispSurf_testcase%u_ManualValidationSysMemTex_%u_%u_%u_%u_%u_Test.bmp" ),
                                g_Application.GetCurrentTestCase(), w, w_iter, h, h_iter, n8BPPColorChannelValue
                                );

                            if( SUCCEEDED( pWrappedD3DDevice->SwapToOriginalDeviceContextState() ) )
                            {
                                D3DX10SaveTextureToFile
                                    (
                                    (ID3D10Resource *) pManualValidationSysMemTex,
                                    D3DX10_IFF_BMP,
                                    DumpFileName
                                    );
                                pWrappedD3DDevice->SwapToEmulatedDeviceContextState();
                            }
                            else
                            {						
                                StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: D3DWrappedDevice::SwapToOriginalDeviceContextState failed", __FILE__, __LINE__ );
                                CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                            }

                            ID3D10Texture2D *pTempTex;
                            HRESULT hrQI;

                            if( SUCCEEDED( hrQI = pTestSurface->QueryInterface( __uuidof(ID3D10Resource), (void**)&pTempTex ) ))
                            {
                                // Test Surface
                                StringCchPrintf
                                    (
                                    DumpFileName,
                                    ARRAYSIZE( DumpFileName ),
                                    _T( "DXGIGetDispSurf_testcase%u_TestSurface_%u_%u_%u_%u_%u_Test.bmp" ),
                                    g_Application.GetCurrentTestCase(), w, w_iter, h, h_iter, n8BPPColorChannelValue
                                    );
                                if( SUCCEEDED( pWrappedD3DDevice->SwapToOriginalDeviceContextState() ) )
                                {
                                    D3DX10SaveTextureToFile
                                        (
                                        (ID3D10Resource *) pTempTex,
                                        D3DX10_IFF_BMP,
                                        DumpFileName
                                        );
                                    pWrappedD3DDevice->SwapToEmulatedDeviceContextState();
                                }
                                else
                                {						
                                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: D3DWrappedDevice::SwapToOriginalDeviceContextState failed", __FILE__, __LINE__ );
                                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                                }
                                SAFE_RELEASE( pTempTex );
                            }
                            else
                            {
                                WriteToLog
                                    (
                                    "QueryInterface for ID3D10Texture2D from IDXGISurface failed with error 0x%x.",
                                    hrQI
                                    );
                            }
                        }
                    }
                }

                pManualValidationSysMemTex->Unmap( 0 );

                if ( FAILED( hr = pTestSurface->Unmap() ) )
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGISurface::Unmap failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                }

                // Release the local objects
                //
                m_pD3D10VidSurface[nAdapter][nMonitor]->ReleaseViews();
                m_pD3D10VidSurface[nAdapter][nMonitor]->Uninitialize();

                if ( !ReleaseLocalObjects( pTestSurface,
                    pValidationVidMemRes,
                    pValidationSysMemRes,
                    pValidationVidMemTex, 
                    pValidationSysMemTex,
                    pManualValidationSysMemTex
                    ) )
                {
                    goto Exit;
                }
            }
        }

        m_pD3D10Scene[nAdapter][nMonitor]->ReleaseViews();
        m_pD3D10Scene[nAdapter][nMonitor]->Uninitialize();
    }

Exit:
    // Clean up for early-out cases
    SAFE_RELEASE( pTestSurface );
    SAFE_RELEASE( pValidationVidMemRes );
    SAFE_RELEASE( pValidationSysMemRes );
    SAFE_RELEASE( pValidationVidMemTex );
    SAFE_RELEASE( pValidationSysMemTex );
    SAFE_RELEASE( pManualValidationSysMemTex );

    m_pD3D10VidSurface[nAdapter][nMonitor]->ReleaseViews();
    m_pD3D10VidSurface[nAdapter][nMonitor]->Uninitialize();

    m_pD3D10Scene[nAdapter][nMonitor]->ReleaseViews();
    m_pD3D10Scene[nAdapter][nMonitor]->Uninitialize();

    if( pWrappedD3DDevice )
        pWrappedD3DDevice->Uninitialize();
    SAFE_DELETE( pWrappedD3DDevice );

    if( iTestFailCount > 0 )
        test_result = RESULT_FAIL;

    return test_result;
}

//
// Create temporary D3D10 Validation texture #2 in sysmem, looping thru all pixels and setting them manually. 
//
HRESULT
    CTests::CreateManualSysmemTexture(
    __in    UINT        nAdapter,
    __in    UINT        nMonitor,
    __in    UINT        width,
    __in    UINT        height,
    __in    DXGI_FORMAT nFormat,
    __in    float       flColorIntensity,
    __inout ID3D10Texture2D **ppManualValidationSysMemTex
    )
{
    HRESULT                 hr = E_FAIL;
    char                    szMsg[c_nMSG_LEN] = {0};    
    D3D10_TEXTURE2D_DESC    tex2DSysDesc;
    D3D10_MAPPED_TEXTURE2D  mappedSysMemTex2D;
    TEST_VEC4F              outputColor = {0};


    memset( &tex2DSysDesc, 0, sizeof(tex2DSysDesc) );
    tex2DSysDesc.Width = width;
    tex2DSysDesc.Height = height;
    tex2DSysDesc.MipLevels = 1;
    tex2DSysDesc.ArraySize = 1;
    tex2DSysDesc.Format = nFormat;
    tex2DSysDesc.SampleDesc.Count = 1;
    tex2DSysDesc.Usage = D3D10_USAGE_STAGING;
    tex2DSysDesc.BindFlags = 0;
    tex2DSysDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;

    if ( FAILED( hr = m_pD3D10Device[nAdapter][nMonitor]->CreateTexture2D( &tex2DSysDesc, NULL, ppManualValidationSysMemTex ) ) )
    {
        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s in %s at line %d.  FAILED: ID3D10Device::CreateTexture2D failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
        goto Exit;
    }

    // Map the Manual Validation texture
    memset( &mappedSysMemTex2D, 0, sizeof(mappedSysMemTex2D) );

    if ( FAILED( hr = ((ID3D10Texture2D*)*ppManualValidationSysMemTex)->Map( 0, D3D10_MAP_WRITE, 0, &mappedSysMemTex2D ) ) )
    {
        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ID3D10Texture2D::Map failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
        goto Exit;
    }

    // Create a 4-block Checker pattern
    int     nHalfWidth = width / 2;
    int     nHalfHeight = height / 2;

    if ( nFormat == DXGI_FORMAT_R16G16B16A16_FLOAT )
    {
        __int64    *p64BitColor[4] = {0};

        p64BitColor[0] = (__int64*)mappedSysMemTex2D.pData;
        p64BitColor[1] = (__int64*)mappedSysMemTex2D.pData + nHalfWidth;
        p64BitColor[2] = (__int64*)mappedSysMemTex2D.pData + (mappedSysMemTex2D.RowPitch / 8 * nHalfHeight);
        p64BitColor[3] = (__int64*)mappedSysMemTex2D.pData + (mappedSysMemTex2D.RowPitch / 8 * nHalfHeight) + nHalfWidth;

        for ( int nLine = 0; nLine < nHalfHeight; nLine++ )
        {
            for ( int nPixel = 0; nPixel < nHalfWidth; nPixel++ )
            {
                outputColor.f[0] = flColorIntensity;    // Red
                outputColor.f[1] = 0.0f;                // Green
                outputColor.f[2] = 0.0f;                // Blue
                outputColor.f[3] = 1.0f;                // Alpha
                Texture_ALU::WriteData( &outputColor, nFormat, (PBYTE)p64BitColor[0] );

                outputColor.f[0] = 0.0f;                // Red
                outputColor.f[1] = flColorIntensity;    // Green
                outputColor.f[2] = 0.0f;                // Blue
                outputColor.f[3] = 1.0f;                // Alpha
                Texture_ALU::WriteData( &outputColor, nFormat, (PBYTE)p64BitColor[1] );

                outputColor.f[0] = 0.0f;                // Red
                outputColor.f[1] = 0.0f;                // Green
                outputColor.f[2] = flColorIntensity;    // Blue
                outputColor.f[3] = 1.0f;                // Alpha
                Texture_ALU::WriteData( &outputColor, nFormat, (PBYTE)p64BitColor[2] );

                outputColor.f[0] = flColorIntensity;    // Red
                outputColor.f[1] = flColorIntensity;    // Green
                outputColor.f[2] = flColorIntensity;    // Blue
                outputColor.f[3] = 1.0f;                // Alpha
                Texture_ALU::WriteData( &outputColor, nFormat, (PBYTE)p64BitColor[3] );

                p64BitColor[0]++;
                p64BitColor[1]++;
                p64BitColor[2]++;
                p64BitColor[3]++;
            }

            p64BitColor[0] += ( mappedSysMemTex2D.RowPitch / 8 ) - nHalfWidth; // 8-Bytes per element. Adding remainder of the row
            p64BitColor[1] += ( mappedSysMemTex2D.RowPitch / 8 ) - nHalfWidth;
            p64BitColor[2] += ( mappedSysMemTex2D.RowPitch / 8 ) - nHalfWidth;
            p64BitColor[3] += ( mappedSysMemTex2D.RowPitch / 8 ) - nHalfWidth;
        }
    }
    else
    {
        __int32    *p32BitColor[4] = {0};

        p32BitColor[0] = (__int32*)mappedSysMemTex2D.pData;
        p32BitColor[1] = (__int32*)mappedSysMemTex2D.pData + nHalfWidth;
        p32BitColor[2] = (__int32*)mappedSysMemTex2D.pData + (mappedSysMemTex2D.RowPitch / 4 * nHalfHeight);
        p32BitColor[3] = (__int32*)mappedSysMemTex2D.pData + (mappedSysMemTex2D.RowPitch / 4 * nHalfHeight) + nHalfWidth;

        for ( int nLine = 0; nLine < nHalfHeight; nLine++ )
        {
            for ( int nPixel = 0; nPixel < nHalfWidth; nPixel++ )
            {
                outputColor.f[0] = flColorIntensity;    // Red
                outputColor.f[1] = 0.0f;                // Green
                outputColor.f[2] = 0.0f;                // Blue
                outputColor.f[3] = 1.0f;                // Alpha
                Texture_ALU::WriteData( &outputColor, nFormat, (PBYTE)p32BitColor[0] );

                outputColor.f[0] = 0.0f;                // Red
                outputColor.f[1] = flColorIntensity;    // Green
                outputColor.f[2] = 0.0f;                // Blue
                outputColor.f[3] = 1.0f;                // Alpha
                Texture_ALU::WriteData( &outputColor, nFormat, (PBYTE)p32BitColor[1] );

                outputColor.f[0] = 0.0f;                // Red
                outputColor.f[1] = 0.0f;                // Green
                outputColor.f[2] = flColorIntensity;    // Blue
                outputColor.f[3] = 1.0f;                // Alpha
                Texture_ALU::WriteData( &outputColor, nFormat, (PBYTE)p32BitColor[2] );

                outputColor.f[0] = flColorIntensity;    // Red
                outputColor.f[1] = flColorIntensity;    // Green
                outputColor.f[2] = flColorIntensity;    // Blue
                outputColor.f[3] = 1.0f;                // Alpha
                Texture_ALU::WriteData( &outputColor, nFormat, (PBYTE)p32BitColor[3] );

                p32BitColor[0]++;
                p32BitColor[1]++;
                p32BitColor[2]++;
                p32BitColor[3]++;
            }

            p32BitColor[0] += ( mappedSysMemTex2D.RowPitch / 4 ) - nHalfWidth; // 4-Bytes per element. Adding remainder of the row
            p32BitColor[1] += ( mappedSysMemTex2D.RowPitch / 4 ) - nHalfWidth;
            p32BitColor[2] += ( mappedSysMemTex2D.RowPitch / 4 ) - nHalfWidth;
            p32BitColor[3] += ( mappedSysMemTex2D.RowPitch / 4 ) - nHalfWidth;
        }
    }

    ((ID3D10Texture2D*)*ppManualValidationSysMemTex)->Unmap( 0 );

    hr = S_OK;

Exit:

    return hr;
}

//
TEST_RESULT
    CTests::FuzzyCompareDXGISurfaces(
    __in    const DXGI_FORMAT   nFormat,
    __in    const DXGI_SURFACE_DESC&  testSurfDesc,
    __in	const DXGI_SWAP_CHAIN_DESC& scDesc,
    __in	const DXGI_MAPPED_RECT mappedTestBits,
    __in	const D3D10_MAPPED_TEXTURE2D mappedSysMemTex2D
    )
{
    TEST_RESULT         test_result = RESULT_FAIL;
    UINT                nDifferences = 0;
    char                szMsg[5][c_nMSG_LEN] = {0};  
    char                szMsg_Debug[c_nMSG_LEN] = {0};
    const UINT nSurfaceLen_Test = mappedTestBits.Pitch * testSurfDesc.Height;
    __in_ecount(nSurfaceLen_Test) const PBYTE pTestBits = (const PBYTE)mappedTestBits.pBits;

    const UINT nSurfaceLen_Validation = mappedSysMemTex2D.RowPitch * testSurfDesc.Height;
    __in_ecount(nSurfaceLen_Validation) const PBYTE pValidationBits = (const PBYTE)mappedSysMemTex2D.pData;
    UINT bytesPerElement = GetBitsPerElement( nFormat ) / 8;

    BYTE    *pByteTest = NULL;
    BYTE    *pByteVal = NULL;
    BYTE    *pByteValNearby = NULL;
    BYTE    *pByteTestEnd = pTestBits + nSurfaceLen_Test;
    BYTE    *pByteValEnd = pValidationBits + nSurfaceLen_Validation;
    __int64	pitch_elements = mappedTestBits.Pitch / bytesPerElement ; // Pitch is in bytes

    // Tolerate stretching ( DXGI calls GDI stretchBlt ): from sdk documentation on GetDisplaySurfaceData:
    // "This method will modify the surface data to fit the destination surface (stretch, shrink, convert format, rotate). The stretch and shrink is performed with point-sampling."
    // tolerate pixels around the center criss-cross
    UINT width = testSurfDesc.Width;
    UINT height = testSurfDesc.Height; 
    UINT horiz_tol = ( scDesc.BufferDesc.Width + width -1 ) / scDesc.BufferDesc.Width;		// take upper bounds. represents how many pixels around the center might have interpolated color values
    UINT vert_tol = ( scDesc.BufferDesc.Height + height -1 ) / scDesc.BufferDesc.Height;
    UINT horiz_tol_begin = ( width - horiz_tol ) / 2;	// lower bounds
    UINT vert_tol_begin = ( height - vert_tol ) / 2;
    UINT horiz_tol_end = ( width + 1 + horiz_tol ) / 2;	// upper bounds
    UINT vert_tol_end = ( height + 1 + vert_tol ) / 2;


    StringCchPrintf( szMsg_Debug, ARRAYSIZE( szMsg_Debug ),
        "FuzzyCompareDXGISurfaces: testSurfDesc width=%d, height=%d, horiz_tol=%d , vert_tol=%d\n, horiz_begin=%d, horiz_end=%d, vert_begin=%d, vert_end=%d",
        width, height, horiz_tol, vert_tol, horiz_tol_begin, horiz_tol_end, vert_tol_begin, vert_tol_end
        );
    CCommon::WriteMsgs( szMsg_Debug, ARRAYSIZE(szMsg_Debug) );

    for ( pByteTest = pTestBits, pByteVal = pValidationBits;
        pByteTest < pByteTestEnd;
        pByteTest += bytesPerElement, pByteVal += bytesPerElement )
    {
        unsigned __int64 diff = ( pByteTest - pTestBits ) / bytesPerElement;
        unsigned __int64 current_texel[2] = { 0 };
        current_texel[0] = diff % pitch_elements;	// horiz
        current_texel[1] = diff / pitch_elements;	// vert

        if ( current_texel[0] == testSurfDesc.Width )	
        {
            // skip over any padded bits
            pByteTest += mappedTestBits.Pitch - testSurfDesc.Width * bytesPerElement;
            pByteVal += mappedSysMemTex2D.RowPitch - testSurfDesc.Width * bytesPerElement;
            continue;
        }

        if ( ( test_result = CompareDXGIPixel( EXACT, pByteTest, pByteVal, nFormat, testSurfDesc.Format, szMsg ) ) != RESULT_PASS )
        {

            // this part of "fuzzy" comparison can take very long long time(Win7 bug #591035), remove it.
#if 0
            pByteValNearby = ( pByteVal - 4 ) >= (__int32*)pValidationBits ?  ( pByteVal - 4 ) : (__int32*)pValidationBits; 

            for ( ; pByteValNearby < pByteValEnd; pByteValNearby++ )
            {
                if ( ( test_result = CompareDXGIPixel( NEARBY, pByteTest, pByteValNearby, nFormat, modeDescBackBuffer.Format, szMsg ) ) == RESULT_PASS )
                {
                    break;
                }
            }

            if ( test_result == RESULT_PASS )
            {
                continue;
            }
#endif
            // if this pixel falls on the middle crisscross, do not count the error
            if ( ( current_texel[0] >= horiz_tol_begin  && current_texel[0] <= horiz_tol_end ) || // near vertical center line: starting from diff == 0
                ( current_texel[1] >= vert_tol_begin  && current_texel[1] <= vert_tol_end ) // near horizontal center line
                )
            {
                // These pixels all map to the center texels on the src surface, which is the swapchain backbuffer 
                // map test surface coordinates to source swapchain backbuffer cordinates and find 4 pixels from the validation surface for interpolation
                const BYTE* pByteVal_gather4[4] = {NULL};
                UINT		nNumPixels = 0; 

                float		unit_texel_sc[2] = { 0 };
                unit_texel_sc[0] =  (float)width / (float)scDesc.BufferDesc.Width;
                unit_texel_sc[1] = (float)height / (float)scDesc.BufferDesc.Height;

                // At most 4 pixels on the Validation surface 
                __int64 left = current_texel[0] - horiz_tol;
                __int64 right = current_texel[0] + horiz_tol;
                __int64 top = current_texel[1] - vert_tol;
                __int64 bottom = current_texel[1] + vert_tol;

                // top left / right
                if( top >= 0 )
                {
                    if( left >= 0 )
                        pByteVal_gather4[nNumPixels++] = pValidationBits + left * bytesPerElement + top * mappedSysMemTex2D.RowPitch; 
                    if( right < width )
                        pByteVal_gather4[nNumPixels++] = pValidationBits + right * bytesPerElement + top * mappedSysMemTex2D.RowPitch; 
                }

                // bottom left / right
                if( bottom < height )
                {
                    if( left >= 0 )
                        pByteVal_gather4[nNumPixels++] = pValidationBits + left * bytesPerElement + bottom * mappedSysMemTex2D.RowPitch; 
                    if( right < width )
                        pByteVal_gather4[nNumPixels++] = pValidationBits + right * bytesPerElement + bottom * mappedSysMemTex2D.RowPitch; 
                }
                if ( ( test_result = CompareDXGIPixel( EXACT, pByteTest, pByteVal_gather4, bytesPerElement, nNumPixels, nFormat, testSurfDesc.Format, szMsg ) ) != RESULT_PASS )
                {
                    StringCchPrintf(
                        szMsg[4],
                        ARRAYSIZE( szMsg[4] ),
                        "FuzzyCompareDXGISurfaces: last failed pixel: horiz=%I64d, vert=%I64d. ValidationPixel Indexes: left = %I64d, right = %I64d, top = %I64d, bottom = %I64d\n",
                        current_texel[0],
                        current_texel[1],
                        left,
                        right,
                        top,
                        bottom
                        );
                    nDifferences++;
                }
            }
            else
            {

                StringCchPrintf(
                    szMsg[4],
                    ARRAYSIZE( szMsg[4] ),
                    "FuzzyCompareDXGISurfaces: last failed pixel: horiz=%I64d, vert=%I64d.",
                    current_texel[0],
                    current_texel[1]
                );
                nDifferences++;
            }

            if (nDifferences > c_nMAX_PIXEL_ERRORS)
            {

                CCommon::WriteMsgs( szMsg[4], ARRAYSIZE(szMsg[4]) );

                CCommon::WriteMsgs( szMsg[0], ARRAYSIZE(szMsg[0]) );
                CCommon::WriteMsgs( szMsg[1], ARRAYSIZE(szMsg[1]) );
                CCommon::WriteMsgs( szMsg[2], ARRAYSIZE(szMsg[2]) );
                CCommon::WriteMsgs( szMsg[3], ARRAYSIZE(szMsg[3]) );

                break;
            }				
        }							

    }

    return test_result;
}


TEST_RESULT
    CTests::CompareDXGIPixel(
    __in    PIXEL_POSITION  npixelPos,
    __in    const void*     pTestPixel,
    __in    const void*     pValidationPixel,
    __in    DXGI_FORMAT     nFormat,
    __in    DXGI_FORMAT     nBackBufferFormat,
    __inout_ecount(5) char  szMsg[5][c_nMSG_LEN]
)
{
    TEST_RESULT         test_result = RESULT_FAIL;
    TEST_VEC4F          tvTestPixel = {0,0,0,0};        // TEST_VEC4F is a struct of floats
    TEST_VEC4F          tvValidationPixel = {0,0,0,0};
    TEST_COLOR          tcTestPixel = {0,0,0,0};        // TEST_COLOR is a struct of doubles
    TEST_COLOR          tcValidationPixel = {0,0,0,0};
    const float         flMaxULP = 1.1f;                // ULP = Unit Last Place  Maximum accuracy: 0.6
    TEST_COLOR          tcULPDiffs = {0,0,0,0};    


    Texture_ALU::ReadDataWithDefaults( pTestPixel, 0, nFormat, &tvTestPixel );
    Texture_ALU::ReadDataWithDefaults( pValidationPixel, 0, nFormat, &tvValidationPixel );

    tcTestPixel.r = tvTestPixel.a;
    tcTestPixel.g = tvTestPixel.b;
    tcTestPixel.b = tvTestPixel.c;
    tcTestPixel.a = tvTestPixel.d;
    tcValidationPixel.r = tvValidationPixel.a;
    tcValidationPixel.g = tvValidationPixel.b;
    tcValidationPixel.b = tvValidationPixel.c;
    tcValidationPixel.a = tvValidationPixel.d;

    if ( !TestColorsSame( tcTestPixel, tcValidationPixel, nFormat, flMaxULP, tcULPDiffs ) )
    {
        if ( npixelPos == EXACT )
        {
            StringCchPrintf( szMsg[0], c_nMSG_LEN,
                "%s in %s at line %d.  FAILED: Color mismatch in Test and Validation surfaces\n",
                __FUNCTION__, __FILE__, __LINE__
                );

            StringCchPrintf( szMsg[1], c_nMSG_LEN,
                "Expected: R = %.04g, G = %.04g, B = %.04g, A = %.04g\n",
                tcValidationPixel.r, tcValidationPixel.g, tcValidationPixel.b, tcValidationPixel.a
                );

            StringCchPrintf( szMsg[2], c_nMSG_LEN,
                "Got     : R = %.04g, G = %.04g, B = %.04g, A = %.04g\n",
                tcTestPixel.r, tcTestPixel.g, tcTestPixel.b, tcTestPixel.a
                );

            StringCchPrintf( szMsg[3], c_nMSG_LEN,
                "ULP diff: R = %.04g, G = %.04g, B = %.04g, A = %.04g\n",
                tcULPDiffs.r, tcULPDiffs.g, tcULPDiffs.b, tcULPDiffs.a
                );
            goto Exit;
        }
        else  //  otherwise npixelPos == NEARBY and we didn't get a match, so FAIL. 
        {
            goto Exit;
        }
    }

    test_result = RESULT_PASS;

Exit:
    return test_result;
}

// CompareDXGIPixel : Validate if testPixel is within the Max/Min ranges of validationPixels
TEST_RESULT
    CTests::CompareDXGIPixel(
    __in    PIXEL_POSITION  npixelPos,
    __in    const void*     pTestPixel,
    __in	const BYTE**	pValidationPixels,
    __in	UINT			nBytesPerPixel,
    __in	UINT			nNumValidationPixels,
    __in    DXGI_FORMAT     nFormat,
    __in    DXGI_FORMAT     nBackBufferFormat,
    __inout_ecount(5) char  szMsg[5][c_nMSG_LEN]
)
{
    TEST_RESULT         test_result = RESULT_PASS;
    TEST_VEC4F          tvTestPixel = {0,0,0,0};        // TEST_VEC4F is a struct of floats
    TEST_COLOR          tcTestPixel = {0,0,0,0};        // TEST_COLOR is a struct of doubles

    TEST_COLOR          tcValidationPixel[2] = {{1.0,1.0,1.0,1.0}, {0.0,0.0,0.0,0.0}};
    const float         flMaxULP = 1.1f;                // ULP = Unit Last Place  Maximum accuracy: 0.6
    TEST_COLOR          tcULPDiffs = {0,0,0,0};    


    Texture_ALU::ReadDataWithDefaults( pTestPixel, 0, nFormat, &tvTestPixel );

    tcTestPixel.r = tvTestPixel.a;
    tcTestPixel.g = tvTestPixel.b;
    tcTestPixel.b = tvTestPixel.c;
    tcTestPixel.a = tvTestPixel.d;

    // Test pixel should be in between max and min of validation pixels for all channels
    for ( UINT index = 0; index < nNumValidationPixels; ++index )
    {
        TEST_VEC4F tvValidationPixel = {0,0,0,0};
        Texture_ALU::ReadDataWithDefaults( pValidationPixels[index], 0, nFormat, &tvValidationPixel );

        // min
        if( tcValidationPixel[0].r > tvValidationPixel.a ) tcValidationPixel[0].r = tvValidationPixel.a;
        if( tcValidationPixel[0].g > tvValidationPixel.b ) tcValidationPixel[0].g = tvValidationPixel.b;
        if( tcValidationPixel[0].b > tvValidationPixel.c ) tcValidationPixel[0].b = tvValidationPixel.c;
        if( tcValidationPixel[0].a > tvValidationPixel.d ) tcValidationPixel[0].a = tvValidationPixel.d;

        // max
        if( tcValidationPixel[1].r < tvValidationPixel.a ) tcValidationPixel[1].r = tvValidationPixel.a;
        if( tcValidationPixel[1].g < tvValidationPixel.b ) tcValidationPixel[1].g = tvValidationPixel.b;
        if( tcValidationPixel[1].b < tvValidationPixel.c ) tcValidationPixel[1].b = tvValidationPixel.c;
        if( tcValidationPixel[1].a < tvValidationPixel.d ) tcValidationPixel[1].a = tvValidationPixel.d;

    }

    TEST_COLOR tcValidationPixel_channel = tcTestPixel; 
	
	for ( int i =0; i < 4;  ++i )
    {
		// test if exceeding max/min by allowable error flMaxULP
        if( tcTestPixel.f[i] < tcValidationPixel[0].f[i] || tcTestPixel.f[i] > tcValidationPixel[1].f[i] )
        {
            tcValidationPixel_channel.f[i] = tcTestPixel.f[i] < tcValidationPixel[0].f[i] ? tcValidationPixel[0].f[i] : tcValidationPixel[1].f[i];
		}
	}
	
    if( !TestColorsSame( tcTestPixel, tcValidationPixel_channel, nFormat, flMaxULP, tcULPDiffs ) )
	{
        test_result = RESULT_FAIL;
		StringCchPrintf( szMsg[0], c_nMSG_LEN,
			"%s in %s at line %d.  FAILED: Color Test surfaces not within range of tolerable values.\n",
			__FUNCTION__, __FILE__, __LINE__
			);

		StringCchPrintf( szMsg[1], c_nMSG_LEN,
			"Expected color range: Max: R = %.04g, G = %.04g, B = %.04g, A = %.04g, Min: R = %.04g, G = %.04g, B = %.04g, A = %.04g\n",
			tcValidationPixel[1].r, tcValidationPixel[1].g, tcValidationPixel[1].b, tcValidationPixel[1].a,
			tcValidationPixel[0].r, tcValidationPixel[0].g, tcValidationPixel[0].b, tcValidationPixel[0].a
			);

		StringCchPrintf( szMsg[2], c_nMSG_LEN,
			"Got     : R = %.04g, G = %.04g, B = %.04g, A = %.04g\n",
			tcTestPixel.r, tcTestPixel.g, tcTestPixel.b, tcTestPixel.a
			);

		StringCchPrintf( szMsg[3], c_nMSG_LEN,
			"ULP diff: R = %.04g, G = %.04g, B = %.04g, A = %.04g\n",
			tcULPDiffs.r, tcULPDiffs.g, tcULPDiffs.b, tcULPDiffs.a
			);      
    }

    return test_result;
}


BOOL
    CTests::ReleaseLocalObjects(
    __inout  IDXGISurface*&             pTestSurface,
    __inout  ID3D10Resource*&           pValidationVidMemRes,
    __inout  ID3D10Resource*&           pValidationSysMemRes,
    __inout  ID3D10Texture2D*&          pValidationVidMemTex, 
    __inout  ID3D10Texture2D*&          pValidationSysMemTex,
    __inout  ID3D10Texture2D*&          pManualValidationSysMemTex
    )
{
    BOOL                fReturn = FALSE;
    ULONG               ulRefCt = 0;
    char                szMsg[c_nMSG_LEN] = {0};    


    if ( (ulRefCt = pTestSurface->Release()) != 0 )
    {
        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGISurface::Release() left RefCount = %d\n", __FILE__, __LINE__, ulRefCt );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
        goto Exit;
    }

    pTestSurface = NULL;

    if ( (ulRefCt = pValidationVidMemRes->Release()) != 1 )
    {
        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ID3D10Resource::Release() left RefCount = %d\n", __FILE__, __LINE__, ulRefCt );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
        goto Exit;
    }

    pValidationVidMemRes = NULL;

    if ( (ulRefCt = pValidationSysMemRes->Release()) != 1 )
    {
        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ID3D10Resource::Release() left RefCount = %d\n", __FILE__, __LINE__, ulRefCt );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
        goto Exit;
    }

    pValidationSysMemRes = NULL;

    if ( (ulRefCt = pValidationVidMemTex->Release()) != 0 )
    {
        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ID3D10Texture2D::Release() left RefCount = %d\n", __FILE__, __LINE__, ulRefCt );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
        goto Exit;
    }

    pValidationVidMemTex = NULL;

    if ( (ulRefCt = pValidationSysMemTex->Release()) != 0 )
    {
        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ID3D10Texture2D::Release() left RefCount = %d\n", __FILE__, __LINE__, ulRefCt );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
        goto Exit;
    }

    pValidationSysMemTex = NULL;

    if ( (ulRefCt = pManualValidationSysMemTex->Release()) != 0 )
    {
        StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ID3D10Texture2D::Release() left RefCount = %d\n", __FILE__, __LINE__, ulRefCt );
        CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
        goto Exit;
    }

    pManualValidationSysMemTex = NULL;

    fReturn = TRUE;

Exit:
    return fReturn;
}

//
void  CTests::DeleteD3D10SceneObjects()
{
    UINT                nAdapter = 0, nMonitor = 0;


    for ( nAdapter = 0; nAdapter < MONITORS_MAX && m_pAdapter[nAdapter] != NULL; nAdapter++ )
    {
        for ( nMonitor = 0; nMonitor < MONITORS_MAX; nMonitor++ )
        {
            if ( m_pOutput[nAdapter][nMonitor] != NULL )
            {
                if ( m_pD3D10Scene[nAdapter][nMonitor] != NULL )
                {
                    delete m_pD3D10Scene[nAdapter][nMonitor];
                    m_pD3D10Scene[nAdapter][nMonitor] = NULL;
                }

                if ( m_pD3D10VidSurface[nAdapter][nMonitor] != NULL )
                {
                    delete m_pD3D10VidSurface[nAdapter][nMonitor];
                    m_pD3D10VidSurface[nAdapter][nMonitor] = NULL;
                }  
            }
        }
    }

    return;
}

BOOL  CTests::ReleaseTestCaseObjects( )
{
    BOOL                fReturn = FALSE;

    if ( !ReleaseDXGITestCaseObjects() )
    {
        goto Exit;
    }  

    fReturn = TRUE;

Exit:
    return fReturn;
}

BOOL  CTests::ReleaseDXGITestCaseObjects()
{
    BOOL                fReturn = FALSE;
    ULONG               ulRefCt = 0;
    char                szMsg[c_nMSG_LEN] = {0};    


    for ( int i = 0; i < MONITORS_MAX; i++ )
    {
        for ( int j = 0; j < MONITORS_MAX; j++ )
        {
            if ( m_pSwapChain[i][j] != NULL )
            {
                // It's necessary to drop out of full-screen mode before 
                // calling release on the Swap Chain.
                m_pSwapChain[i][j]->SetFullscreenState( FALSE, NULL );

                ulRefCt = m_pSwapChain[i][j]->Release();
                m_pSwapChain[i][j] = NULL;

                if ( ulRefCt != 0 )
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGISwapChain::Release() left RefCount = %d\n", __FILE__, __LINE__, ulRefCt );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                }
            }  

            if ( m_pDXGIDevice[i][j] != NULL )
            {
                ulRefCt = m_pDXGIDevice[i][j]->Release();
                m_pDXGIDevice[i][j] = NULL;

                if ( ulRefCt != 1 )
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGIDevice::Release() left RefCount = %d\n", __FILE__, __LINE__, ulRefCt );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                }
            }

            if ( m_pD3D10Device[i][j] != NULL )
            {
                ulRefCt = m_pD3D10Device[i][j]->Release();
                m_pD3D10Device[i][j] = NULL;

                if ( ulRefCt != 0 )
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: ID3D10Device::Release() left RefCount = %d\n", __FILE__, __LINE__, ulRefCt );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                }
            }

            if ( m_pOutput[i][j] != NULL )
            {
                if ( (ulRefCt = m_pOutput[i][j]->Release() ) != 0 )
                {
                    StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGIOutput::Release() left RefCount = %d\n", __FILE__, __LINE__, ulRefCt );
                    CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                    goto Exit;
                }

                m_pOutput[i][j] = NULL;
            }

            if ( m_hwndDevWnd[i][j] )
            {
                DestroyWindow( m_hwndDevWnd[i][j] );
                m_hwndDevWnd[i][j] = NULL;
            }
        }

        if ( m_pAdapter[i] != NULL )
        {
            if ( (ulRefCt = m_pAdapter[i]->Release() ) != 0 ) 
            {
                StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGIAdapter::Release() left RefCount = %d\n", __FILE__, __LINE__, ulRefCt );
                CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                goto Exit;
            }

            m_pAdapter[i] = NULL;
        }
    }

    if ( m_FactoryPointer.pFactory )
    {
        if ( ( ulRefCt = m_FactoryPointer.pFactory->Release() ) != 0 ) 
        {
            StringCchPrintf( szMsg, ARRAYSIZE( szMsg ), "%s at line %d.  FAILED: IDXGIFactory::Release() left RefCount = %d\n", __FILE__, __LINE__, ulRefCt );
            CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
            goto Exit;
        }

        m_FactoryPointer.pFactory = NULL;
    }  

    fReturn = TRUE;

Exit:
    return fReturn;
}
