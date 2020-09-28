//
// Project headers
//

#include "DXGIPresentRate.h"
#include "TestApp.h"


CWindowedTestCases::CWindowedTestCases() :
	m_tp_bPartialPresentation( false )
{
	ZeroMemory( &m_PresentParams, sizeof( m_PresentParams ) );
	m_PresentParams.pDirtyRects = m_DirtyRects;
	m_PresentParams.DirtyRectsCount = ARRAYSIZE( m_DirtyRects );

	// Fill out dirty rects to be a compact portion of the resource.
	// 9 dirty rects should build the follow dirty rects each a pixel wide:
	// +-+-+-+-+-+
	// |0| |1| |4|
	// +-+-+-+-+-+
	// | | | | | |
	// +-+-+-+-+-+
	// |3| |2| |5|
	// +-+-+-+-+-+
	// | | | | | |
	// +-+-+-+-+-+
	// |7| |8| |6|
	// +-+-+-+-+-+
	for (UINT d = 0, t = 0; t < ARRAYSIZE( m_DirtyRects ); d += 2)
	{
		// Fill up the next column:
		for (UINT y = 0; y <= d && t < ARRAYSIZE( m_DirtyRects ); y += 2, ++t)
		{
			const RECT rc = { d, y, d + 1, y + 1 };
			m_DirtyRects[ t ] = rc;
		}

		// Fill up the next row:
		for (UINT x = 0; x < d && t < ARRAYSIZE( m_DirtyRects ); x += 2, ++t)
		{
			const RECT rc = { x, d, x + 1, d + 1 };
			m_DirtyRects[ t ] = rc;
		}
	}
};


CWindowedTestCases::~CWindowedTestCases()
{
};


TEST_RESULT CWindowedTestCases::CreateDeviceAndSwapChain
(
    bool bDWM,
    bool bUseDCOMP,
    DXGI_FORMAT PixelFormat,
    DXGI_SWAP_EFFECT SwapEffectArg,
    int iNumBuffersIndexArg
)
{
    //
    // Initialize locals.
    //

    TEST_RESULT OverallResult = RESULT_FAIL;
    HRESULT hresult = E_FAIL;
    IDXGISwapChain* pSwapChain = NULL;


    ReleaseTestCaseObjects();

    if(g_Application.m_bDFlip)
    {        
        if(bUseDCOMP || SwapEffectArg != DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL)
        {
            WriteToLog("Parameteres Not Applicable to DirectFLIP.");
            OverallResult = RESULT_SKIP;
            goto Cleanup;
        }
    }

    //
    // Turn DWM on or off if necessary.
    //

    BOOL bDWMState = FALSE;
    DwmIsCompositionEnabled( &bDWMState );
    if
    (
        bDWM == true && bDWMState == FALSE ||
        bDWM == false && bDWMState == TRUE
    )
    {
        DwmEnableComposition
        (
            bDWM == true ? TRUE : FALSE
        );

        Sleep( 1000 );
    };


    UINT adapterID = 0;
    
    if(g_Application.m_DXGIOptions.AdapterIDSrc == ADAPTER_UNDEFINED)
    {
        adapterID = 0;
    }
    else
    {
        adapterID = g_Application.m_DXGIOptions.AdapterIDSrc; 
    }
    
    //
    // Create our device.
    //

       assert( m_DevicePointer.pDevice == NULL );
    hresult = g_Application.CreateFactoryAdapterDevice
    (
        &m_FactoryPointer,
        adapterID,
        &m_AdapterPointer,
        g_Application.m_D3DOptions.SrcDriverType,
        D3D10_CREATE_DEVICE_SINGLETHREADED,
        m_TestFeatureLevel,
        0,
        &m_DevicePointer
    );

    if
    (
        hresult == DXGI_ERROR_NOT_FOUND
    )
    {
        WriteToLog
        (
            "Could not find an adapter at ordinal 0."
        );

        OverallResult = RESULT_SKIP;
        goto Cleanup;
    }
    else if
    (
        hresult == DXGI_ERROR_UNSUPPORTED ||
        hresult == E_NOINTERFACE ||
        hresult == E_NOTIMPL ||
        hresult == E_FAIL &&
        (
            m_TestFeatureLevel == TEST_FEATURE_10_1_LEVEL_9_1 ||
            m_TestFeatureLevel == TEST_FEATURE_10_1_LEVEL_9_2 ||
            m_TestFeatureLevel == TEST_FEATURE_10_1_LEVEL_9_3 ||
            m_TestFeatureLevel == TEST_FEATURE_11_0_LEVEL_9_1 ||
            m_TestFeatureLevel == TEST_FEATURE_11_0_LEVEL_9_2 ||
            m_TestFeatureLevel == TEST_FEATURE_11_0_LEVEL_9_3
        )
    )
    {
        WriteToLog
        (
            "Could not create device. Skipping testcases"
        );

        OverallResult = RESULT_SKIP;
        goto Cleanup;
    }
    else if ( FAILED( hresult ) )
    {
        WriteToLog
        (
            "CWindowedTestCases::CreateDeviceAndSwapChain:  CreateFactoryAdapterDevice failed with error 0x%x.",
            hresult
        );

        goto Cleanup;
    };

    if ( m_DevicePointer.pDevice == NULL )
    {
        WriteToLog
        (
            "CWindowedTestCases::CreateDeviceAndSwapChain:  CreateFactoryAdapterDevice succeeded but returned NULL device pointer."
        );

        goto Cleanup;
    };

    m_PrimaryAdapterDevice.WrapDevice( m_DevicePointer.pDevice );

    hresult = m_DevicePointer.pDevice->QueryInterface( __uuidof( *m_pDXGIDevice ), reinterpret_cast< void** >( &m_pDXGIDevice ) );
    if ( FAILED( hresult ) )
    {
        WriteToLog
        (
            "CWindowedTestCases::CreateDeviceAndSwapChain:  QueryInterface for IDXGIDevice failed."
        );

        goto Cleanup;
    }

    //
    // Create a focus / device window.
    //

    m_hwndFocusWindow = CreateWindow
    (
        m_szFocusWindowClassName,
        "Focus Window",
        WS_OVERLAPPEDWINDOW,
        0,
        0,
        64,
        64,
        NULL,
        NULL,
        m_hProgInst,
        NULL
    );

    if ( m_hwndFocusWindow == NULL )
    {
        WriteToLog
        (
            "CreateWindow failed."
        );

        goto Cleanup;
    };


    //
    // Display the window.
    //

    ShowWindow
    (
        m_hwndFocusWindow,
        SW_SHOWNORMAL
    );

    UpdateWindow( m_hwndFocusWindow );


    SetForegroundWindow( m_hwndFocusWindow );


    m_hwndDeviceWindow = m_hwndFocusWindow;


    //
    // Associate the focus window with the DXGI factory.
    //

    DebugOut.Write("CWindowedTestCases::CreateDeviceAndSwapChain:  Calling MakeWindowAssociation.\n");

    hresult = m_FactoryPointer.pFactory->MakeWindowAssociation
    (
        m_hwndFocusWindow,
        0
    );

    if ( FAILED( hresult ) )
    {
        WriteToLog
        (
            "CWindowedTestCases::CreateDeviceAndSwapChain:  MakeWindowAssociation failed with error 0x%x.",
            hresult
        );

        goto Cleanup;
    };

    if(g_Application.m_bDFlip)
    {
        // NOTE: In order for DirectFlip to work, satisfy-
        // 1) Test Window covers Entire Display Area
        // 2) Swapchain Buffers need to match the window size.
        // Resize window to match entire Display area here since
        // Swapchain description is built off of window placement.
        SetWindowLongPtr(m_hwndDeviceWindow, GWL_STYLE, WS_POPUP);
        SetWindowPos(m_hwndDeviceWindow, 
                        HWND_TOPMOST, 
                        0, 
                        0, 
                        GetSystemMetrics(SM_CXSCREEN),// Desktop Width 
                        GetSystemMetrics(SM_CYSCREEN),// Desktop Height
                        SWP_SHOWWINDOW | SWP_FRAMECHANGED);
    }

    //
    // Get the window's client rect.
    //

    RECT rectWindowClient;

    if
    (
        !GetClientRect
        (
            m_hwndDeviceWindow,
            &rectWindowClient
        )
    )
    {
        WriteToLog
        (
            "CWindowedTestCases::CreateDeviceAndSwapChain:  GetClientRect failed."
        );

        goto Cleanup;
    };


    //
    // Specify the swap chain properties.
    //

    SCDesc.BufferDesc.Width =
        rectWindowClient.right - rectWindowClient.left;
    SCDesc.BufferDesc.Height =
        rectWindowClient.bottom - rectWindowClient.top;

    SCDesc.BufferDesc.RefreshRate.Numerator = 0;
    SCDesc.BufferDesc.RefreshRate.Denominator = 1;
    //SCDesc.BufferDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
    SCDesc.BufferDesc.Format = PixelFormat;

    // Skip unsupported formats
    // Skip unsupported formats
    UINT FormatSupport = 0;
    hresult = m_DevicePointer.pDevice->CheckFormatSupport
    (
        SCDesc.BufferDesc.Format,
        &FormatSupport
    );

    if ( hresult == E_FAIL )
    {
        WriteToLog
        (
            "Device does not support this format."
        );

        OverallResult = RESULT_SKIP;
        goto Cleanup;
    }
    else if ( FAILED( hresult ) )
    {
        WriteToLog
        (
            "CWindowedTestCases::CreateDeviceAndSwapChain:  CheckFormatSupport failed with error 0x%x.",
            hresult
        );

        goto Cleanup;
    };

    C_ASSERT( D3D10_FORMAT_SUPPORT_TEXTURE2D == D3D11_FORMAT_SUPPORT_TEXTURE2D );
    C_ASSERT( D3D10_FORMAT_SUPPORT_RENDER_TARGET == D3D11_FORMAT_SUPPORT_RENDER_TARGET );
    C_ASSERT( D3D10_FORMAT_SUPPORT_CPU_LOCKABLE == D3D11_FORMAT_SUPPORT_CPU_LOCKABLE );
    C_ASSERT( D3D10_FORMAT_SUPPORT_DISPLAY == D3D11_FORMAT_SUPPORT_DISPLAY );
    UINT RequiredFormatSupport =
        D3D10_FORMAT_SUPPORT_TEXTURE2D |
        D3D10_FORMAT_SUPPORT_RENDER_TARGET |
        D3D10_FORMAT_SUPPORT_CPU_LOCKABLE |
        D3D10_FORMAT_SUPPORT_DISPLAY;

    if ( ( FormatSupport & RequiredFormatSupport ) != RequiredFormatSupport )
    {
        WriteToLog
        (
            "Device does not support capabilities with this format needed to test it."
        );

        OverallResult = RESULT_SKIP;
        goto Cleanup;
    };


    // DXGI_FORMAT_R8G8B8A8_UNORM
    // DXGI_FORMAT_R8G8B8A8_TYPELESS;
    SCDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    SCDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    //SCDesc.BufferDesc = pModeDesc -> DXGIModeDesc;

    SCDesc.SampleDesc.Count = 1;
    SCDesc.SampleDesc.Quality = 0;

    SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    SCDesc.BufferCount = iNumBuffersIndexArg;

    //SCDesc.MaxFrameLatency = 1;

    SCDesc.OutputWindow =  bUseDCOMP ? NULL : m_hwndDeviceWindow; // Don't specify a DeviceWindow when using DCOMP (Windowless) Presentation

    //SCDesc.DegradationPreference = DXGI_DEGRADE_TIMELINESS;

    SCDesc.Windowed = TRUE;

    SCDesc.SwapEffect = SwapEffectArg;

    //SCDesc.BufferRotation = DXGI_MODE_ROTATION_UNSPECIFIED;

    //SCDesc.RefreshRateInHz = 0.f;

    SCDesc.Flags = 0 ;


    //
    // Attempt the swap chain creation.
    //

    DebugOut.Write("CWindowedTestCases::CreateDeviceAndSwapChain:  Initializing swap chain.\n");

    BOOL bInitializeSwapChainResult = m_TestedSwapChain.InitializeSwapChain( &SCDesc );

    if ( bInitializeSwapChainResult != TRUE )
    {
        WriteToLog
        (
            "CWindowedTestCases::CreateDeviceAndSwapChain:  InitializeSwapChain failed."
        );

        goto Cleanup;
    };


    //
    // Get a reference to our swap chain.
    //

    assert( m_pSwapChain == NULL );
    m_TestedSwapChain.GetSwapChainPtr( &pSwapChain );

    if ( pSwapChain == NULL )
    {
        WriteToLog
        (
            "CWindowedTestCases::CreateDeviceAndSwapChain:  GetSwapChainPtr returned NULL swap chain pointer."
        );

        goto Cleanup;
    };

    hresult = pSwapChain->QueryInterface( __uuidof( *m_pSwapChain ), reinterpret_cast< void** >( &m_pSwapChain ) );
    if ( FAILED( hresult ) )
    {
        WriteToLog
        (
            "CWindowedTestCases::CreateDeviceAndSwapChain:  QueryInterface for IDXGISwapChain1 failed."
        );

        goto Cleanup;
    };


    //
    // Dump details about the swap chain.
    //

    DebugOut.Write("***********************************\n");
    DebugOut.Write("CWindowedTestCases::CreateDeviceAndSwapChain:  CreateSwapChain succeeded!\n");

    {
        const DXGIFormatInfo *pBBFormatInfo = NULL;

        if (pBBFormatInfo = CFormatInfoMap::Find((SCDesc.BufferDesc).Format))
        {
            DebugOut.Write(pBBFormatInfo -> szDesc);
            DebugOut.Write("\n");
        }
    }

    DebugOut.Write("***********************************\n");

    //
    // Initialize the D3D10Device-owned resources needed
    // for texture rendering.
    //

    bool bCreateTexRenderResourcesResult = false;
    switch ( m_TestFeatureLevel )
    {
        case TEST_FEATURE_11_0_LEVEL_9_1:
        case TEST_FEATURE_11_0_LEVEL_9_2:
        case TEST_FEATURE_11_0_LEVEL_9_3:
        case TEST_FEATURE_10_1_LEVEL_9_1:
        case TEST_FEATURE_10_1_LEVEL_9_2:
        case TEST_FEATURE_10_1_LEVEL_9_3:
        {
            bCreateTexRenderResourcesResult = m_PrimaryAdapterDevice.CreateTexRenderResources
            (
                "vs_4_0_level_9_1",
                "ps_4_0_level_9_1"
            );
        } break;


        default:
        {
            bCreateTexRenderResourcesResult = m_PrimaryAdapterDevice.CreateTexRenderResources
            (
                "vs_4_0",
                "ps_4_0"
            );
        } break;
    };

    if ( bCreateTexRenderResourcesResult != true )
    {
        WriteToLog
        (
            "CreateTexRenderResources failed."
        );

        goto Cleanup;
    };


   	// Instantiate DCOMP if necessary
	if (bUseDCOMP)
	{
		if (FAILED(m_DCOMPIsolation.CreateAndAssociateDCOMPVisual(m_pSwapChain, m_hwndDeviceWindow)))
			goto Cleanup;
	}

    OverallResult = RESULT_PASS;


Cleanup:
    SAFE_RELEASE( pSwapChain );
    return OverallResult;
};
