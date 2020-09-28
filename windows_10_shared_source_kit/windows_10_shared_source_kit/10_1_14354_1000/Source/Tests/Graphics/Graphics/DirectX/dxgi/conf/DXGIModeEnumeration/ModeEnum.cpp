//
// ModeEnum.cpp
//
// Enumerate and Set DXGI Modes
//

//
// Project headers
//

#include "DXGIModeEnum.h"
#include "TestApp.h"


#pragma warning(disable: 4355)


////////////////////////////////////////////////////////////////////////////////
// clsTestableDevice Methods

clsTestableDevice::~clsTestableDevice()
{
};

bool clsTestableDevice::GetDevicePtr
    (
    ID3D10Device **pDevicePointer
    )
{
    if ( pD3D10Device != NULL )
    {
        pD3D10Device->AddRef();
    };

    *pDevicePointer = pD3D10Device;

    return true;
};

////////////////////////////////////////////////////////////////////////////////
// clsTestableDXGISwapChain Methods

clsTestableDXGISwapChain::~clsTestableDXGISwapChain()
{
};


bool clsTestableDXGISwapChain::GetSwapChainPtr
    (
    IDXGISwapChain * * pSwapChainPointer
    )
{
    if ( pSwapChain != NULL )
    {
        pSwapChain->AddRef();
    };

    *pSwapChainPointer = pSwapChain;

    return true;
};


CDXGIModeEnum::CDXGIModeEnum()
    :
m_pFactory( NULL ),
    m_pAdapter( NULL ),
    PrimaryAdapterDevice( (CTest *) this ),
    m_pD3D10Device( NULL ),
    TestedSwapChain( (CTest *) this, &PrimaryAdapterDevice ),
    pSwapChain( NULL ),
    pSwapChain1( NULL ),
    hwndFocusWindow( NULL ),
    hwndDeviceWindow( NULL ),
    pDeviceWindowOutput( NULL ),
    uiDeviceOutputWidth( 0 ),
    uiDeviceOutputHeight( 0 ),
    iModeIndex( 0 ),
    iSwapEffectIndex( 0 ),
    iDSDFmtIndex( 0 ),
    iDSDDimIndex( 0 )
{
};


CDXGIModeEnum::~CDXGIModeEnum()
{
};


void CDXGIModeEnum::InitTestParameters()
{
    DebugOut.Write("Entering InitTestParameters.\n");


    //
    // Call the base class implementation
    //

    CDXGITest::InitTestParameters();


    tstring strPath = GetPath().ToString();


    EnumerateModes();

    if ( TestedModes.size() > 0 )
    {
        AddParameter
            (
            _T("Mode"),
            &iModeIndex,
            RangeValueSet
            (
            0,
            (int) TestedModes.size() - 1,
            1
            )
            );
    }
    else
    {
        iModeIndex = 0;
    };

    DebugOut.Write("Exiting InitTestParameters.\n");
};


TEST_RESULT CDXGIModeEnum::Setup()
{
    //
    // Initialize locals.
    //

    TEST_RESULT test_result = RESULT_FAIL;


    test_result = RESULT_PASS;

    return test_result;
};


HRESULT CDXGIModeEnum::SetDisplayMode()
{
    //
    // Initialize locals.
    //

    HRESULT hresult;

    IDXGIDevice * pDevice = NULL;
    IDXGIAdapter * pAdapter = NULL;
    IDXGIOutput * pOutput = NULL;


    //
    // QueryInterface on the device for an IDXGIDevice interface pointer.
    //

    hresult = m_pD3D10Device->QueryInterface
        (
        IID_IDXGIDevice,
        (void **) &pDevice
        );

    if ( FAILED( hresult ) )
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetDisplayMode:  QueryInterface for IDXGIDevice failed with error 0x%x.",
            hresult
            );

        goto Cleanup;
    };

    if ( pDevice == NULL )
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetDisplayMode:  QueryInterface for IDXGIDevice succeeded but returned a NULL IDXGIDevice interface pointer."
            );

        goto Cleanup;
    };


    //
    // Query the device for its parent adapter.
    //

    hresult = pDevice->GetAdapter
        (
        &pAdapter
        );

    if ( FAILED( hresult ) )
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetDisplayMode:  GetAdapter failed with error 0x%x.",
            hresult
            );

        goto Cleanup;
    };

    if ( pAdapter == NULL )
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetDisplayMode:  GetAdapter succeeded but returned a NULL adapter pointer."
            );

        hresult = E_FAIL;
        goto Cleanup;
    };


    //
    // Enumerate the target output.
    //

    hresult = pAdapter->EnumOutputs
        (
        TestedModes[ iModeIndex ].OutputOrdinal,
        &pOutput
        );

    if ( FAILED( hresult ) )
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetDisplayMode:  EnumOutputs on output %u failed with error 0x%x.",
            hresult
            );

        goto Cleanup;
    };

    if ( pOutput == NULL )
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetDisplayMode:  EnumOutputs on output %u succeeded but returned a NULL output pointer.",
            TestedModes[ iModeIndex ].OutputOrdinal
            );
        hresult = E_FAIL;
        goto Cleanup;
    };


    //
    // Get a descriptor for the output.
    //

    DXGI_OUTPUT_DESC outputDesc;
    hresult = pOutput->GetDesc( &outputDesc );

    if ( FAILED( hresult ) )
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetDisplayMode: GetDesc on target output failed with error 0x%x.",
            hresult
            );

        goto Cleanup;
    };


    //
    // Rotate the display if necessary.
    //

    if ( outputDesc.Rotation != TestedModes[ iModeIndex ].Rotation )
    {
        //
        // Initialize our mode structure.
        //

        DEVMODEW Mode;
        Mode.dmSize = sizeof( DEVMODEW );
        Mode.dmSpecVersion = DM_SPECVERSION;


        //
        // Get the current desktop mode.
        //

        BOOL bGetCurrentModeResult = EnumDisplaySettingsW
            (
            outputDesc.DeviceName,
            ENUM_CURRENT_SETTINGS,
            &Mode
            );

        if ( bGetCurrentModeResult != TRUE )
        {
            DWORD GetCurrentModeError = GetLastError();

            WriteToLog
                (
                "CDXGIModeEnum::SetDisplayMode:  EnumDisplaySettingsW failed with error %u.",
                GetCurrentModeError
                );

            if ( GetCurrentModeError != ERROR_SUCCESS )
            {
                hresult = HRESULT_FROM_WIN32( GetCurrentModeError );
            };

            goto Cleanup;
        };


        //
        // Override the height and width.
        //


        Mode.dmPelsWidth = TestedModes[ iModeIndex ].Mode.Width;
        Mode.dmPelsHeight = TestedModes[ iModeIndex ].Mode.Height;

        Mode.dmDisplayFrequency = 0;

        Mode.dmSpecVersion = DM_SPECVERSION;
        Mode.dmFields |= DM_DISPLAYORIENTATION;
        Mode.dmFields |= DM_PELSWIDTH;
        Mode.dmFields |= DM_PELSHEIGHT;
        Mode.dmFields |= DM_DISPLAYFLAGS;
        Mode.dmFields |= DM_DISPLAYFREQUENCY;
        // allow the default scaling mode 
        Mode.dmFields |= DM_DISPLAYFIXEDOUTPUT;
        Mode.dmDisplayFixedOutput = DMDFO_DEFAULT;


        //
        // Override the rotation.
        //

        switch ( TestedModes[ iModeIndex ].Rotation )
        {
        case DXGI_MODE_ROTATION_IDENTITY:
            Mode.dmDisplayOrientation = DMDO_DEFAULT;
            break;

        case DXGI_MODE_ROTATION_ROTATE90:
            Mode.dmDisplayOrientation = DMDO_90;
            break;

        case DXGI_MODE_ROTATION_ROTATE180:
            Mode.dmDisplayOrientation = DMDO_180;
            break;

        case DXGI_MODE_ROTATION_ROTATE270:
            Mode.dmDisplayOrientation = DMDO_270;
            break;

        default:
            WriteToLog
                (
                "CDXGIModeEnum::SetDisplayMode: modeRotation[%u] returned from EnumDisplaySettings() is unknown.",
                TestedModes[ iModeIndex ].Rotation
                );
            goto Cleanup;
            break;
        };

        WriteToLog("Changing desktop orientation to %s",  (Mode.dmDisplayOrientation == DMDO_270 ? "DMDO_270" : (Mode.dmDisplayOrientation == DMDO_180 ? "DMDO_180" : (Mode.dmDisplayOrientation == DMDO_90 ? "DMDO_90": "DMDO_DEFAULT" ) )) );

        //
        // Override the scanline ordering.
        //
        if (TestedModes[ iModeIndex ].Mode.ScanlineOrdering == DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST
            ||  TestedModes[ iModeIndex ].Mode.ScanlineOrdering == DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST)
        {
            Mode.dmDisplayFlags |= DM_INTERLACED;
            Mode.dmFields |= DM_DISPLAYFLAGS;
        }
        else
        {
            Mode.dmDisplayFlags &= ~DM_INTERLACED;
        }


        //
        // Attempt the mode switch.
        //

        LONG    lResult;
        lResult = ChangeDisplayMode
            (
            &Mode, 
            outputDesc.DeviceName
            );

        if (lResult != DISP_CHANGE_SUCCESSFUL)
        {
            WriteToLog
                (
                "CDXGIModeEnum::SetDisplayMode: ChangeDisplayMode failed."
                );
            hresult = E_FAIL;
        }
    };


Cleanup:
    if ( pOutput != NULL )
    {
        pOutput->Release();
        pOutput = NULL;
    };


    if ( pAdapter != NULL )
    {
        pAdapter->Release();
        pAdapter = NULL;
    };


    if ( pDevice != NULL )
    {
        pDevice->Release();
        pDevice = NULL;
    };


    return hresult;
};


TEST_RESULT CDXGIModeEnum::SetupTestCase()
{
    InitializeWindowClass();

    return RESULT_PASS;
};


void CDXGIModeEnum::CleanupTestCase()
{
    CleanupWindowClass();
};


TEST_RESULT CDXGIModeEnum::ExecuteTestCase()
{
    //
    // Initialize locals.
    //

    TEST_RESULT test_result = RESULT_FAIL;   
    HRESULT hr = S_OK;
    int i;

    if ( TestedModes.size() == 0 )
    {
        WriteToLog
            (
            "CDXGIModeEnum::ExecuteTestCase: No %smodes detected."
            "\nIf there is no any video adapter in the test machine, it is not an error;"
            "\nOtherwise the reason shall be checked.",
            m_bTestStereoModes ? _T("STEREO ") : _T("")
            );

        // No stereo modes is valid since it's an optional feature, just skip
        if( m_bTestStereoModes )
        {
            test_result = RESULT_SKIP;
        }

        goto Cleanup;
    };

    //
    // Dump test mode parameters.
    //

    const DXGIFormatInfo * pFormatRecord = CFormatInfoMap::Find
        (
        TestedModes[ iModeIndex ].Mode.Format
        );

    WriteToLog
        (
        _T( "CDXGIModeEnum::ExecuteTestCase: Adapter %u, output %u, mode %u x %u @ %f Hz @ %u degrees, %s, %s, %s, Stereo: %s" ),
        TestedModes[ iModeIndex ].AdapterOrdinal,
        TestedModes[ iModeIndex ].OutputOrdinal,
        TestedModes[ iModeIndex ].Mode.Width,
        TestedModes[ iModeIndex ].Mode.Height,
        (float) TestedModes[ iModeIndex ].Mode.RefreshRate.Numerator /
        (float) TestedModes[ iModeIndex ].Mode.RefreshRate.Denominator,
        ( TestedModes[ iModeIndex ].Rotation - DXGI_MODE_ROTATION_IDENTITY ) * 90,
        pFormatRecord != NULL ? pFormatRecord->szDesc : _T( "[Unknown]" ),
        ToString( TestedModes[ iModeIndex ].Mode.ScanlineOrdering ).c_str(),
        ToString( TestedModes[ iModeIndex ].Mode.Scaling ).c_str(),
        m_bTestStereoModes ? _T( "On" ) : _T( "Off" )
        );


    //
    // We assume all scanout format Skip formats that are deprecated on Direct3D 10.
    //

    if
        (
        (
        TestedModes[ iModeIndex ].Mode.Format != DXGI_FORMAT_B8G8R8A8_UNORM &&
        TestedModes[ iModeIndex ].Mode.Format != DXGI_FORMAT_B8G8R8X8_UNORM
        ) ||
        (
        m_TestFeatureLevel != TEST_FEATURE_10_1_LEVEL_9_1 &&
        m_TestFeatureLevel != TEST_FEATURE_10_1_LEVEL_9_2 &&
        m_TestFeatureLevel != TEST_FEATURE_10_1_LEVEL_9_3
        )
        )
    {
        bool bModeHasScanoutFormat = CFormatsToTest::IsFormatInList
            (
            TestedModes[ iModeIndex ].Mode.Format
            );

        if ( bModeHasScanoutFormat != true )
        {
            WriteToLog
                (
                "CDXGIModeEnum::ExecuteTestCase: Skipping deprecated format."
                );

            test_result = RESULT_SKIP;

            goto Cleanup;
        };
    };



    //
    // Create a swap chain.
    //

    test_result = SetupFullScreenSwapChain();

    if ( test_result != RESULT_PASS )
    {
        if ( test_result != RESULT_SKIP )
        {
            WriteToLog
                (
                "CDXGIModeEnum::ExecuteTestCase: SetupFullScreenSwapChain failed."
                );
        };

        goto Cleanup;
    };


    if ( !UpdateOutputWindowInformation() )
    {
        test_result = RESULT_FAIL;

        WriteToLog
            (
            "CDXGIModeEnum::ExecuteTestCase: UpdateOutputWindowInformation failed."
            );

        goto Cleanup;
    };


    //
    // Present on the swap chain.
    //

    test_result = PresentAndValidate();

    if ( test_result != RESULT_PASS )
    {
        if ( test_result != RESULT_SKIP )
        {
            WriteToLog
                (
                "CDXGIModeEnum::ExecuteTestCase: PresentAndValidate failed."
                );
        };

        goto Cleanup;
    };


    //
    // Make sure that the mode has the expected properties.
    //

    test_result = ValidateMode();
    if ( test_result == RESULT_FAIL)
    {
        WriteToLog
            (
            "CDXGIModeEnum::ExecuteTestCase: ValidateMode failed. Give it a bit more time and check again."
            );

        Sleep (TRY_INTERVAL);
        test_result = ValidateMode();
        if ( test_result == RESULT_FAIL)
        {
            WriteToLog
                (
                "CDXGIModeEnum::ExecuteTestCase: ValidateMode failed a second time.   Failing test case."
                );

            goto Cleanup;
        }
    }

    test_result = RESULT_PASS;


Cleanup:
    if ( !ReleaseTestCaseObjects() )
    {
        WriteToLog
            (
            "CDXGIModeEnum::ExecuteTestCase: ReleaseTestCaseObjects failed."
            );

        test_result = RESULT_FAIL;
        goto Cleanup;
    };

    if ( m_pD3D10Device != NULL )
    {
        ULONG refcount = m_pD3D10Device->Release();
        m_pD3D10Device = NULL;
    };


    DebugOut.Write("CDXGIModeEnum::ExecuteTestCase: Exit.\n");

    return test_result;
};


TEST_RESULT CDXGIModeEnum::SetupFullScreenSwapChain()
{
    //
    // Initialize locals.
    //

    TEST_RESULT test_result = RESULT_FAIL;
    HRESULT hr = S_OK;
    BOOL bReturn = FALSE;
    LONG lReturn = 0;
    int nDXGIDisplayFrequency1 = 0;
    int nDXGIDisplayFrequency2 = 0;
    int nRotationDifference = 0;
    BOOL bFullScreen = FALSE;
    const DXGIFormatInfo * pEnumeratedModeFormatRecord = 0;
    IDXGIOutput *pOutput = NULL;
        ULONG uref ;

    if ( !ReleaseTestCaseObjects() )
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetupFullScreenSwapChain: ReleaseTestCaseObjects failed."
            );

        test_result = RESULT_FAIL;

        goto Cleanup;
    };

    // Create DXGI Factory
    hr = CreateDXGIFactory1(IID_IDXGIFactory1, (void **)&m_pFactory);
    if (FAILED(hr))
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetupFullScreenSwapChain: CreateDXGIFactory1 failed. %s (0x%x)",
            D3DHResultToString(hr).c_str(), 
            hr
            );

        test_result = RESULT_FAIL;

        goto Cleanup;
    };

    // Create Adapter we intend to test.
    hr =  m_pFactory->EnumAdapters(TestedModes[ iModeIndex ].AdapterOrdinal, &m_pAdapter);
    if (FAILED(hr))
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetupFullScreenSwapChain: EnumAdapter(index = %d) failed. %s (0x%x)",
            TestedModes[ iModeIndex ].AdapterOrdinal,
            D3DHResultToString(hr).c_str(), 
            hr
            );

        test_result = RESULT_FAIL;

        goto Cleanup;
    };

    //
    // Create a device.
    //

    hr = PrimaryAdapterDevice.InitializeDevice
        (
        m_pAdapter,
        g_Application.m_D3DOptions.DriverType,
        NULL,
        D3D10_CREATE_DEVICE_SINGLETHREADED,
        0,
        m_TestFeatureLevel,
        g_Application.m_pD3D10CreateDevice1
        );

    if ( FAILED( hr ) )
    {
        // E_NOTIMPL is also used by some derivers as "Unsupported". 
        if 
            ( 
            hr == DXGI_ERROR_UNSUPPORTED || 
            hr == E_NOINTERFACE ||
            hr == E_NOTIMPL
            )
        {
            WriteToLog("CDXGIModeEnum::SetupFullScreenSwapChain: Could not create device. Return Code: %s (0x%x) Skipping Test.", D3DHResultToString(hr).c_str(), hr);
            test_result = RESULT_SKIP;
            goto Cleanup;
        }
        else
        {
            WriteToLog("CDXGIModeEnum::SetupFullScreenSwapChain: InitializeDevice failed with error %s 0x%x.", D3DHResultToString(hr).c_str(), hr);
            goto Cleanup;
        }
    }

    if (!PrimaryAdapterDevice.GetDevicePtr( &m_pD3D10Device ))
    {
        WriteToLog("CDXGIModeEnum::SetupFullScreenSwapChain: GetDevicePtr returned a NULL device pointer.");
        goto Cleanup;
    }

    //
    // Create a focus / device window.
    //

    hwndFocusWindow = CreateWindowEx
        (
        WS_EX_TOPMOST,
        m_szFocusWindowClassName,
        "Focus Window",
        WS_OVERLAPPEDWINDOW,
        TestedModes[ iModeIndex ].OutputDescriptor.DesktopCoordinates.left,
        TestedModes[ iModeIndex ].OutputDescriptor.DesktopCoordinates.top,
        ( TestedModes[ iModeIndex ].OutputDescriptor.DesktopCoordinates.right - TestedModes[ iModeIndex ].OutputDescriptor.DesktopCoordinates.left ) / 2,
        ( TestedModes[ iModeIndex ].OutputDescriptor.DesktopCoordinates.bottom - TestedModes[ iModeIndex ].OutputDescriptor.DesktopCoordinates.top ) / 2,
        NULL,
        NULL,
        m_hProgInst,
        NULL
        );


    if ( hwndFocusWindow == NULL )
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetupFullScreenSwapChain: CreateWindow failed."
            );

        goto Cleanup;
    };


    //
    // Display the window.
    //

    ShowWindow
        (
        hwndFocusWindow,
        SW_SHOWNORMAL
        );

    UpdateWindow( hwndFocusWindow );

    hwndDeviceWindow = hwndFocusWindow;

    //
    // Associate the focus window with the DXGI factory.
    //

    hr = m_pFactory->MakeWindowAssociation
        (
        hwndFocusWindow,
        0
        );

    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetupFullScreenSwapChain: MakeWindowAssociation failed with error 0x%x.",
            hr
            );

        goto Cleanup;
    };

    pEnumeratedModeFormatRecord = CFormatInfoMap::Find( swapChainDesc.BufferDesc.Format );


    //
    // Skip unsupported formats.
    //

    UINT FormatSupport = 0;
    hr = m_pD3D10Device->CheckFormatSupport
        (
        TestedModes[ iModeIndex ].Mode.Format,
        &FormatSupport
        );

    if ( hr == E_FAIL )
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetupFullScreenSwapChain: Device does not support this format."
            );

        test_result = RESULT_SKIP;
        goto Cleanup;
    }
    else if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetupFullScreenSwapChain: CheckFormatSupport failed with error 0x%x.",
            hr
            );

        goto Cleanup;
    };

    //C_ASSERT( D3D10_FORMAT_SUPPORT_TEXTURE2D == D3D11_FORMAT_SUPPORT_TEXTURE2D );
    //C_ASSERT( D3D10_FORMAT_SUPPORT_RENDER_TARGET == D3D11_FORMAT_SUPPORT_RENDER_TARGET );
    //C_ASSERT( D3D10_FORMAT_SUPPORT_DISPLAY == D3D11_FORMAT_SUPPORT_DISPLAY );
    UINT RequiredFormatSupport =
        D3D10_FORMAT_SUPPORT_TEXTURE2D |
        D3D10_FORMAT_SUPPORT_RENDER_TARGET |
        D3D10_FORMAT_SUPPORT_DISPLAY;

    if ( ( FormatSupport & RequiredFormatSupport ) != RequiredFormatSupport )
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetupFullScreenSwapChain: Device does not support capabilities with this format needed to test it."
            );
        WriteToLog("CheckFormatSupport indicates the following caps are supported: 0x%x.  The test is looking for the following: 0x%x (D3D10_FORMAT_SUPPORT_TEXTURE2D|D3D10_FORMAT_SUPPORT_RENDER_TARGET|D3D10_FORMAT_SUPPORT_DISPLAY", FormatSupport, RequiredFormatSupport);

        test_result = RESULT_SKIP;
        goto Cleanup;
    };

    // #132505 DXGI Mode Enumeration test is not designed to test clone mode
    // For clone mode we can enum only one output, this approach is reasonable.
    if (FAILED(SetDisplayMode()))
    {
        test_result = RESULT_SKIP;
        // continue to test
    }

    //
    // Set focus to our device window.
    //

    GetFocusForFullscreen();


    //
    // Specify the swap chain properties.
    //

    ZeroMemory( &swapChainDesc, sizeof( swapChainDesc ) );
    swapChainDesc.BufferDesc = TestedModes[ iModeIndex ].Mode;

    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = BACK_BUFFER_COUNT;
    swapChainDesc.OutputWindow = hwndDeviceWindow;
    swapChainDesc.Windowed = FALSE;
    swapChainDesc.SwapEffect = DXGISwapEffects[ iSwapEffectIndex ].dxgiSwapEffect;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    BOOL bSuccess = FALSE;
    if( m_bTestStereoModes )
    {
        swapChainDesc1 = clsDXGISwapChain::CopyToSwapChainDesc1(swapChainDesc, &fullscreenDesc);
        swapChainDesc1.Stereo = true;	
        swapChainDesc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

        bSuccess = TestedSwapChain.InitializeFullscreenSwapChain1
            (
            swapChainDesc.OutputWindow,
            &swapChainDesc1,
            swapChainDesc.Windowed ? NULL : &fullscreenDesc
            );
    }
    else
    {
        bSuccess = TestedSwapChain.InitializeFullscreenSwapChain
            (
            &swapChainDesc
            );
    }

    if ( !bSuccess )
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetupFullScreenSwapChain: InitializeFullscreenSwapChain failed."
            );

        goto Cleanup;
    };


    //
    // Get a reference to our swap chain.
    //

    assert( pSwapChain == NULL );
    TestedSwapChain.GetSwapChainPtr( &pSwapChain );

    if ( pSwapChain == NULL )
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetupFullScreenSwapChain: GetSwapChainPtr returned NULL swap chain pointer."
            );

        goto Cleanup;
    };

    hr = m_pAdapter->EnumOutputs(TestedModes[ iModeIndex ].OutputOrdinal, &pOutput);
    if (FAILED(hr))
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetupFullScreenSwapChain: EnumOutputa(index = %d) failed. %s (0x%x)",
            TestedModes[ iModeIndex ].OutputOrdinal,
            D3DHResultToString(hr).c_str(), 
            hr
            );

        test_result = RESULT_FAIL;

        goto Cleanup;
    };

    hr = pSwapChain->SetFullscreenState(TRUE, pOutput);
    if (FAILED(hr))
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetupFullScreenSwapChain: SetFullscreenState failed. %s (0x%x)",
            D3DHResultToString(hr).c_str(), 
            hr
            );

        test_result = RESULT_FAIL;

        goto Cleanup;
    };


    // 
    // QI up to the IDXGISwapChain1 interface if we're testing stereo modes
    //
    if( m_bTestStereoModes )
    {
        hr = pSwapChain->QueryInterface(__uuidof(IDXGISwapChain1), (void**)&pSwapChain1);
        if( FAILED(hr) )
        {
            WriteToLog
                (
                "CDXGIModeEnum::SetupFullScreenSwapChain: Failed to QI up to IDXGISwapChain1. hr=0x%x",
                hr
                );

            goto Cleanup;
        }
    }

    //
    // Determine whether the swap chain is full-screen.
    //
    int i;
    for (i=0; i<TRY_TIMES; i++)
    {
        hr = pSwapChain->GetFullscreenState
            (
            &bFullScreen,
            NULL
            );

        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                "CDXGIModeEnum::SetupFullScreenSwapChain: GetFullscreenState failed with error 0x%x.",
                hr
                );

            goto Cleanup;
        };

        if ( bFullScreen == TRUE )
        {
            break;
        }

        Sleep(TRY_INTERVAL);
    };

    if (i == TRY_TIMES)
    {
        WriteToLog
            (
            "CDXGIModeEnum::SetupFullScreenSwapChain: Swap chain was created, but is not full-screen."
            );

        goto Cleanup;
    };


    //
    // Check the swap chain descriptor against what we asked for.
    //

    if ( GetFramework()->GetTestMode() == WGFTEST_MODE_WHQL )
    {
        DXGI_SWAP_CHAIN_DESC SwapChainDescriptor;
        DXGI_SWAP_CHAIN_DESC1 SwapChainDescriptor1;
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC SwapChainFullscreenDesc;

        if( m_bTestStereoModes )
        {
            hr = pSwapChain1->GetDesc1
                (
                &SwapChainDescriptor1
                );

            if ( SUCCEEDED( hr ) )
            {
                hr = pSwapChain1->GetFullscreenDesc
                    (
                    &SwapChainFullscreenDesc
                    );
            }
        }
        else
        {
            hr = pSwapChain->GetDesc
                (
                &SwapChainDescriptor
                );
        }

        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                "CDXGIModeEnum::SetupFullScreenSwapChain: GetDesc on swap chain failed with error 0x%x.",
                hr
                );

            goto Cleanup;
        };

        if( m_bTestStereoModes )
        {
            bSuccess = CompareSwapChainDesc1
                (
                swapChainDesc1,
                SwapChainDescriptor1
                );

            if (bSuccess)
            {
                bSuccess = CompareFullscreenDesc
                    (
                    fullscreenDesc,
                    SwapChainFullscreenDesc
                    );
            }
        }
        else
        {
            bSuccess = CompareSwapChainDesc
                (
                swapChainDesc,
                SwapChainDescriptor
                );
        }

        if
            (
            !bSuccess
            )
        {
            WriteToLog
                (
                "CDXGIModeEnum::SetupFullScreenSwapChain: Swap chain descriptor from GetDesc is different than what was passed to CreateSwapChain."
                );

            goto Cleanup;
        };
    };


    test_result = RESULT_PASS;


Cleanup:
    SAFE_RELEASE (pOutput);
    return test_result;
};


void CDXGIModeEnum::Cleanup()
{
    ReleaseTestCaseObjects();

    return;
};


void CDXGIModeEnum::GetFocusForFullscreen()
{
    RECT    rc = {0};
    POINT   pt = {0};
    INPUT   input = {0};
    MSG     msg = {0};

    // Find location of top-left corner of main window in mouse coordinates.
    HWND hwnd = GetFramework()->GetShell()->GetMainHWnd();
    ::GetWindowRect( hwnd, &rc );
    pt.x = ( rc.left + 10 ) * 65535 / GetSystemMetrics( SM_CXSCREEN );
    pt.y = ( rc.top + 10 ) * 65535 / GetSystemMetrics( SM_CYSCREEN );

    input.type = INPUT_MOUSE;
    input.mi.dx = pt.x;
    input.mi.dy = pt.y;
    input.mi.mouseData = 0;
    input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

    // Move the mouse close to the top-left corner of the main window.
    SendInput
        (
        1,
        &input,
        sizeof( input )
        );

    // Process outstanding messages.
    while (PeekMessage( &msg, hwnd, 0, 0, PM_REMOVE ))
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    };

    return;
};


BOOL CDXGIModeEnum::CompareSwapChainDesc
    (
    __in DXGI_SWAP_CHAIN_DESC & swapChainDesc1,
    __in DXGI_SWAP_CHAIN_DESC & swapChainDesc2
    )
{
    if
        (
        swapChainDesc1.BufferDesc.RefreshRate.Numerator == swapChainDesc2.BufferDesc.RefreshRate.Numerator &&
        swapChainDesc1.BufferDesc.RefreshRate.Denominator == swapChainDesc2.BufferDesc.RefreshRate.Denominator &&
        swapChainDesc1.BufferDesc.Format == swapChainDesc2.BufferDesc.Format &&
        swapChainDesc1.BufferDesc.ScanlineOrdering == swapChainDesc2.BufferDesc.ScanlineOrdering &&
        swapChainDesc1.BufferDesc.Scaling == swapChainDesc2.BufferDesc.Scaling &&
        swapChainDesc1.SampleDesc.Count == swapChainDesc2.SampleDesc.Count &&
        swapChainDesc1.SampleDesc.Quality == swapChainDesc2.SampleDesc.Quality &&
        swapChainDesc1.BufferUsage == swapChainDesc2.BufferUsage &&
        swapChainDesc1.BufferCount == swapChainDesc2.BufferCount &&
        swapChainDesc1.OutputWindow == swapChainDesc2.OutputWindow &&
        swapChainDesc1.Windowed == swapChainDesc2.Windowed &&
        swapChainDesc1.SwapEffect == swapChainDesc2.SwapEffect &&
        swapChainDesc1.Flags == swapChainDesc2.Flags
        )
    {
        return TRUE;
    };


    return FALSE;
};


BOOL CDXGIModeEnum::CompareSwapChainDesc1
    (
    __in DXGI_SWAP_CHAIN_DESC1 & swapChainDesc1,
    __in DXGI_SWAP_CHAIN_DESC1 & swapChainDesc2
    )
{
    if
        (
        swapChainDesc1.Width == swapChainDesc2.Width &&
        swapChainDesc1.Height == swapChainDesc2.Height &&
        swapChainDesc1.Format == swapChainDesc2.Format &&
        swapChainDesc1.Stereo == swapChainDesc2.Stereo &&
        swapChainDesc1.SampleDesc.Count == swapChainDesc2.SampleDesc.Count &&
        swapChainDesc1.SampleDesc.Quality == swapChainDesc2.SampleDesc.Quality &&
        swapChainDesc1.BufferUsage == swapChainDesc2.BufferUsage &&
        swapChainDesc1.BufferCount == swapChainDesc2.BufferCount &&
        swapChainDesc1.SwapEffect == swapChainDesc2.SwapEffect &&
        swapChainDesc1.Flags == swapChainDesc2.Flags
        )
    {
        return TRUE;
    };


    return FALSE;
};

BOOL CDXGIModeEnum::CompareFullscreenDesc
    (
    __in DXGI_SWAP_CHAIN_FULLSCREEN_DESC & fullscreenDesc1,
    __in DXGI_SWAP_CHAIN_FULLSCREEN_DESC & fullscreenDesc2
    )
{
    if 
        (
        fullscreenDesc1.RefreshRate.Numerator == fullscreenDesc2.RefreshRate.Numerator &&
        fullscreenDesc1.RefreshRate.Denominator == fullscreenDesc2.RefreshRate.Denominator &&
        fullscreenDesc1.Scaling == fullscreenDesc2.Scaling &&
        fullscreenDesc1.ScanlineOrdering == fullscreenDesc2.ScanlineOrdering
        )
    {
        return TRUE;
    };

    return FALSE;
};


bool CDXGIModeEnum::UpdateOutputWindowInformation()
{
    SAFE_RELEASE(pDeviceWindowOutput);

    if ( !pSwapChain )
    {
        WriteToLog("CDXGIModeEnum::UpdateOutputWindowInformation: aborting, swap chain not initialized.\n");
    }
    else
    {
        MONITORINFO     MonitorInfoDeviceWindow = {0};
        BOOL            bMonitorInfoInitialized = FALSE;

        {
            HMONITOR  hmonitorDeviceWindow = NULL;

            if ( !(hmonitorDeviceWindow = MonitorFromWindow(hwndDeviceWindow, MONITOR_DEFAULTTONEAREST)))
                WriteToLog("CDXGIModeEnum::UpdateOutputWindowInformation: MonitorFromWindow failed.\n");
            else
            {
                MonitorInfoDeviceWindow.cbSize = sizeof(MONITORINFO);

                if ( !(bMonitorInfoInitialized = GetMonitorInfo(hmonitorDeviceWindow, &MonitorInfoDeviceWindow)) )
                {
                    WriteToLog("CDXGIModeEnum::UpdateOutputWindowInformation: GetMonitorInfo failed.\n");
                }
            }
        }

        if ( !bMonitorInfoInitialized )
        {
            WriteToLog("CDXGIModeEnum::UpdateOutputWindowInformation: aborting, Monitor info unavailable.\n");
        }
        else
        {
            BOOL bWindowInfoInitialized = FALSE;

            uiDeviceOutputWidth = (UINT)((MonitorInfoDeviceWindow.rcMonitor).right - (MonitorInfoDeviceWindow.rcMonitor).left);
            uiDeviceOutputHeight = (UINT)((MonitorInfoDeviceWindow.rcMonitor).bottom - (MonitorInfoDeviceWindow.rcMonitor).top);

            {
                WINDOWINFO DeviceWindowInfo = {0};

                if (!(bWindowInfoInitialized = GetWindowInfo(hwndDeviceWindow, &DeviceWindowInfo)))
                    WriteToLog("CDXGIModeEnum::UpdateOutputWindowInformation: GetWindowInfo failed.\n");
                else
                {
                    rectDeviceWindow.left = (DeviceWindowInfo.rcClient).left - (MonitorInfoDeviceWindow.rcMonitor).left;
                    rectDeviceWindow.top = (DeviceWindowInfo.rcClient).top - (MonitorInfoDeviceWindow.rcMonitor).top;
                    rectDeviceWindow.right = (DeviceWindowInfo.rcClient).right - (MonitorInfoDeviceWindow.rcMonitor).left;
                    rectDeviceWindow.bottom = (DeviceWindowInfo.rcClient).bottom - (MonitorInfoDeviceWindow.rcMonitor).top;
                }
            }

            if (!bWindowInfoInitialized)
                WriteToLog("CDXGIModeEnum::UpdateOutputWindowInformation: aborting, Window info unavailable.\n");
            else
            {
                HRESULT hresult;

                if (FAILED((hresult = pSwapChain -> GetContainingOutput(&pDeviceWindowOutput))))
                    WriteToLog("CDXGIModeEnum::UpdateOutputWindowInformation: aborting, IDXGISwapChain::GetContainingOutput failed.\n");
                else
                    return true;
            }
        }
    }

    return false;
};


BOOL CDXGIModeEnum::ReleaseTestCaseObjects()
{
    SAFE_RELEASE(pDeviceWindowOutput);

    SAFE_RELEASE(pSwapChain);
    SAFE_RELEASE(pSwapChain1);
    TestedSwapChain.ReleaseAllObjects();

    if ( m_pD3D10Device != NULL )
    {
        ULONG uRef = m_pD3D10Device->Release();
        m_pD3D10Device = NULL;
    };

    SAFE_RELEASE(m_pAdapter);

    PrimaryAdapterDevice.ReleaseAllObjects();

    if ( m_pFactory != NULL )
    {
        HWND hwndCurrentFocus = NULL;

        m_pFactory->GetWindowAssociation( &hwndCurrentFocus );

        if ( hwndCurrentFocus != NULL )
        {
            m_pFactory->MakeWindowAssociation
                (
                NULL,
                0
                );
        };

        ULONG uref = m_pFactory->Release();
        if (uref)
            WriteToLog("WARNING: Factory ref count after final release is %u.  Expected it to be zero!", uref);
        m_pFactory = NULL;
    };


    if ( hwndDeviceWindow != NULL )
    {
        if ( hwndDeviceWindow != hwndFocusWindow )
        {
            DestroyWindow( hwndDeviceWindow );
        };

        hwndDeviceWindow = NULL;
    };


    if ( hwndFocusWindow != NULL )
    {
        DestroyWindow( hwndFocusWindow );
        hwndFocusWindow = NULL;
    };


    return TRUE;
};


TEST_RESULT CDXGIModeEnum::PresentAndValidate()
{
    //
    // Initialize locals.
    //

    TEST_RESULT OverallResult = RESULT_FAIL;
    HRESULT hr = S_OK;


    //
    // Present to the swap chain.
    //

    hr = pSwapChain->Present
        (
        1,
        0
        );

    // For Mode enumeration test it is not needed to care about OCCLUDED return. 
    // If it is an error, it will be reported by Pesentation tests
    if (FAILED( hr ))
    {
        if ( hr != DXGI_STATUS_OCCLUDED )
        {
            WriteToLog
                (
                "CDXGIModeEnum::PresentAndValidate: Present failed with error 0x%x.",
                hr
                );
        }
        else
        {
            WriteToLog
                (
                "[Warning]CDXGIModeEnum::PresentAndValidate: Present returned DXGI_STATUS_OCCLUDED."
                "Check if some application, dialog box, message box, e.g. Optimal Resolution Notification," 
                "poped up during the test. If yes, it is not an error; otherwise the reason should be checcked."
                );
        }

        goto Cleanup;
    };

    OverallResult = RESULT_PASS;


Cleanup:
    return OverallResult;
};


TEST_RESULT CDXGIModeEnum::ValidateMode()
{
    //
    // At this point we expect to have a swap chain that's
    // full-screen in the test mode.  So now we want to
    // independently confirm that we are in fact in the
    // mode that we expected to be in.
    //

    //
    // Initialize locals.
    //

    TEST_RESULT OverallResult = RESULT_PASS;


    //
    // Let's ask User32 what mode the monitor is in.
    //
    // @TODO [chrisza]: EDS/User32 is not stereo aware, need a work item to use kernel to validate stereo-ness 

    DEVMODEW CurrentMode;
    ZeroMemory( &CurrentMode, sizeof( CurrentMode ) );
    CurrentMode.dmSize = sizeof( CurrentMode );
    CurrentMode.dmSpecVersion = DM_SPECVERSION;

    BOOL bGetDisplaySettingsResult = EnumDisplaySettingsW
        (
        TestedModes[ iModeIndex ].OutputDescriptor.DeviceName,
        ENUM_CURRENT_SETTINGS,
        &CurrentMode
        );

    if ( bGetDisplaySettingsResult != TRUE )
    {
        WriteToLog
            (
            "CDXGIModeEnum::ValidateMode: EnumDisplaySettings failed."
            );
        OverallResult = RESULT_FAIL;
        goto Cleanup;
    };


    //
    // Check the width and height.
    //

    WriteToLog
        (
        "CDXGIModeEnum::ValidateMode: Checking mode height and width."
        );

    switch ( TestedModes[ iModeIndex ].Rotation )
    {
    case DXGI_MODE_ROTATION_IDENTITY:
    case DXGI_MODE_ROTATION_ROTATE180:
    case DXGI_MODE_ROTATION_ROTATE90:
    case DXGI_MODE_ROTATION_ROTATE270:
        if ( CurrentMode.dmPelsWidth != TestedModes[ iModeIndex ].Mode.Width 
            ||   CurrentMode.dmPelsHeight != TestedModes[ iModeIndex ].Mode.Height)
        {
            WriteToLog
                (
                "CDXGIModeEnum::ValidateMode: Width %u differs from expected width %u, or Height %u differs from expected height %u.",
                CurrentMode.dmPelsWidth,
                TestedModes[ iModeIndex ].Mode.Width,
                CurrentMode.dmPelsHeight,
                TestedModes[ iModeIndex ].Mode.Height
                );

            OverallResult = RESULT_FAIL;
        };
        break;

    default:
        WriteToLog
            (
            "CDXGIModeEnum::ValidateMode:  modeRotation[%u] returned from EnumDisplaySettings() is unknown.",
            TestedModes[ iModeIndex ].Rotation
            );
        OverallResult = RESULT_FAIL;
        break;
    };


    //
    // Check the bit depth?
    //

    WriteToLog
        (
        "CDXGIModeEnum::ValidateMode: Checking mode bit depth."
        );

    switch ( TestedModes[ iModeIndex ].Mode.Format )
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        {
            if ( CurrentMode.dmBitsPerPel != 32 )
            {
                WriteToLog
                    (
                    "CDXGIModeEnum::ValidateMode: Bit depth of %u does not match expected bit depth of 32.",
                    CurrentMode.dmBitsPerPel
                    );
                OverallResult = RESULT_FAIL;
            };
        } break;


    case DXGI_FORMAT_B5G6R5_UNORM:
        {
            if ( CurrentMode.dmBitsPerPel != 16 )
            {
                WriteToLog
                    (
                    "CDXGIModeEnum::ValidateMode: Bit depth of %u does not match expected bit depth of 16.",
                    CurrentMode.dmBitsPerPel
                    );
                OverallResult = RESULT_FAIL;
            };
        } break;
    };


    //
    // Check the refresh rate.
    //

    WriteToLog
        (
        "CDXGIModeEnum::ValidateMode: Checking mode refresh rate."
        );

    float ExpectedRefreshRate = 
        (float) TestedModes[ iModeIndex ].Mode.RefreshRate.Numerator /
        (float) TestedModes[ iModeIndex ].Mode.RefreshRate.Denominator;

    if ((CurrentMode.dmDisplayFlags & DM_INTERLACED) != 0)
    {
        ExpectedRefreshRate /= 2;
    }

    float User32RefreshRate = (float) CurrentMode.dmDisplayFrequency;

    if
        (
        User32RefreshRate - ExpectedRefreshRate > 1.0f ||
        User32RefreshRate - ExpectedRefreshRate < -1.0f
        )
    {
        // Log a warning, but do not fail.  Monitors in clone mode can apparently 
        // cause a descrepency between the requested refreshrate specified through DXGI, and 
        // what get's enumerated through GDI. This probably warrants further investigation
        // but apparently that's the way it was for Win7.
        WriteToLog
            (
            "CDXGIModeEnum::ValidateMode: Refresh rate %f differs from expected rate of %f.  Not failing for this.",
            User32RefreshRate,
            ExpectedRefreshRate
            );

    };


Cleanup:
    return OverallResult;
};
