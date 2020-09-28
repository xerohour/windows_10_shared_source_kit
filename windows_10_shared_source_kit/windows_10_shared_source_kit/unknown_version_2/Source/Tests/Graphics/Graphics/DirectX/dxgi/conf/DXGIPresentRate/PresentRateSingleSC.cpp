//
// Project headers
//

#include "DXGIPresentRate.h"
#include "testapp.h"

#pragma warning(disable: 4355)

//const DXGI_FORMAT CDXGIPresentRateSingleSC::dxgifmtBBInitTexture = DXGI_FORMAT_R16G16B16A16_FLOAT;
const DXGI_FORMAT CDXGIPresentRateSingleSC::dxgifmtBBInitTexture = DXGI_FORMAT_R8G8B8A8_UNORM;
const UINT CDXGIPresentRateSingleSC::cuiBBInitTextureWidth = 64;
const UINT CDXGIPresentRateSingleSC::cuiBBInitTextureHeight = 64;
const UINT CDXGIPresentRateSingleSC::cuiNoOfPresentOperations = 60;
const UINT CDXGIPresentRateSingleSC::cuiSkipPresentsAtStart = 16; // Need to fill the queue for Flip Model before analyzing times.
const float CDXGIPresentRateSingleSC::cfMaxFractionForImmediate = 0.5f;
const float CDXGIPresentRateSingleSC::cfMaxFractionalDeviationForNonImmediate = 0.25f; // tolerance needs to be fairly high since interval zero frame times vary from system to system.
const float CDXGIPresentRateSingleSC::cfPrecentLateFrames = 0.15f;


//-----------------------------------------------------------------------------
//
//  Constructor
//
//-----------------------------------------------------------------------------
CDXGIPresentRateSingleSC::CDXGIPresentRateSingleSC()
    :
m_PrimaryAdapterDevice( (CTest *) this ),
    m_TestedSwapChain( (CTest *) this, &m_PrimaryAdapterDevice ),
    m_hwndFocusWindow( NULL ),
    m_hwndDeviceWindow( NULL ),
    m_pSwapChain( NULL ),
    m_pDXGIDevice( NULL ),
    m_useQpcTimes( FALSE ),
    m_useAverages( TRUE )
{
    ZeroMemory( &m_FactoryPointer, sizeof( m_FactoryPointer ) );
    ZeroMemory( &m_AdapterPointer, sizeof( m_AdapterPointer ) );
    ZeroMemory( &m_DevicePointer, sizeof( m_DevicePointer ) );

    for
        (
        UINT uiIndex = 0;
    uiIndex < MAX_BUFFERS;
    ++uiIndex
        )
    {
        pBBInitTextureSys[ uiIndex ] = NULL;
        pBBInitTextureVid[ uiIndex ] = NULL;
        pBBInitSRV[ uiIndex ] = NULL;
    };

    m_DCOMPIsolation.SetLogger(GetFramework()->GetLogWrapper());
};


//-----------------------------------------------------------------------------
//
//  Destructor
//
//-----------------------------------------------------------------------------
CDXGIPresentRateSingleSC::~CDXGIPresentRateSingleSC()
{
};


//-----------------------------------------------------------------------------
//
//  InitializeTexturePair
//
//-----------------------------------------------------------------------------
bool CDXGIPresentRateSingleSC::InitializeTexturePair
    (
    UINT uiIndex,
    const char * const szResourceName
    )
{
    //
    // Initialize locals.
    //

    bool bOverallResult = false;
    HRESULT hresult = E_FAIL;


    //
    // Create the system memory texture, and initialize it
    // with an image from a dds file.
    //

    // Initialize a structure describing the texture.
    D3DX10_IMAGE_LOAD_INFO d3dx_texdesc_Sys;
    d3dx_texdesc_Sys.Width = cuiBBInitTextureWidth;
    d3dx_texdesc_Sys.Height = cuiBBInitTextureHeight;
    d3dx_texdesc_Sys.Depth = 1;
    d3dx_texdesc_Sys.FirstMipLevel = 0;
    d3dx_texdesc_Sys.MipLevels = 1;
    d3dx_texdesc_Sys.Usage = D3D10_USAGE_STAGING;
    d3dx_texdesc_Sys.BindFlags = 0;
    d3dx_texdesc_Sys.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
    d3dx_texdesc_Sys.MiscFlags = 0;
    d3dx_texdesc_Sys.Format = dxgifmtBBInitTexture;
    d3dx_texdesc_Sys.Filter = D3DX10_DEFAULT;
    d3dx_texdesc_Sys.MipFilter = D3DX10_DEFAULT;
    d3dx_texdesc_Sys.pSrcInfo = NULL;

    /*
    WriteToLog
    (
    "CDXGIPresentRateSingleSC::InitializeTexturePair:  System memory texture:  %d x %d.",
    d3dx_texdesc_Sys.Width,
    d3dx_texdesc_Sys.Height
    );
    */

    hresult = D3DX10CreateTextureFromResource
        (
        m_DevicePointer.pDevice,
        GetModuleHandle(NULL),
        szResourceName,
        &d3dx_texdesc_Sys,
        NULL,
        (ID3DResource**) &pBBInitTextureSys[ uiIndex ],
        NULL
        );

    if ( FAILED( hresult ) )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::CreateTexture:  D3DX10CreateTextureFromResource on system memory texture failed with error 0x%x.",
            hresult
            );

        goto Cleanup;
    };

    if ( pBBInitTextureSys[ uiIndex ] == NULL )
    {
        WriteToLog
            (
            "CDXGIPresentRateSingleSC::InitializeTexturePair:  D3DX10CreateTextureFromFile succeeded but returned a NULL texture pointer."
            );

        goto Cleanup;
    };


    //
    // Create the video memory texture.
    //

    D3D_TEXTURE2D_DESC d3d_texdesc_Vid;
    d3d_texdesc_Vid.Width = cuiBBInitTextureWidth;
    d3d_texdesc_Vid.Height = cuiBBInitTextureHeight;
    d3d_texdesc_Vid.MipLevels = 1;
    d3d_texdesc_Vid.ArraySize = 1;
    d3d_texdesc_Vid.Format = dxgifmtBBInitTexture;
    d3d_texdesc_Vid.SampleDesc.Count = 1;
    d3d_texdesc_Vid.SampleDesc.Quality = 0;

    C_ASSERT( D3D10_USAGE_DEFAULT == D3D11_USAGE_DEFAULT );
    d3d_texdesc_Vid.Usage = (D3D_USAGE) D3D10_USAGE_DEFAULT;
    d3d_texdesc_Vid.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
    d3d_texdesc_Vid.CPUAccessFlags = 0;
    d3d_texdesc_Vid.MiscFlags = 0;

    /*
    WriteToLog
    (
    "CDXGIPresentRateSingleSC::InitializeTexturePair:  Video memory texture:  %d x %d.",
    d3d_texdesc_Vid.Width,
    d3d_texdesc_Vid.Height
    );
    */

    hresult = m_DevicePointer.pDevice->CreateTexture2D
        (
        &d3d_texdesc_Vid,
        NULL,
        &pBBInitTextureVid[ uiIndex ]
    );

    if ( FAILED( hresult ) )
    {
        WriteToLog
            (
            "CDXGIPresentRateSingleSC::InitializeTexturePair:  CreateTexture2D failed with error 0x%x.",
            hresult
            );

        goto Cleanup;
    };


    if ( pBBInitTextureVid[ uiIndex ] == NULL )
    {
        WriteToLog
            (
            "CDXGIPresentRateSingleSC::InitializeTexturePair:  CreateTexture2D succeeded but returned a NULL texture pointer."
            );

        goto Cleanup;
    };


    //
    // Use the system memory texture to update the video memory texture.
    //

    m_DevicePointer.pDevice->CopyResource
        (
        pBBInitTextureVid[ uiIndex ],
        pBBInitTextureSys[ uiIndex ]
    );


    //
    // Create a shader resource view of the video memory texture.
    //

    D3D_SHADER_RESOURCE_VIEW_DESC d3dsrvdesc;
    d3dsrvdesc.Format = dxgifmtBBInitTexture;

    C_ASSERT( D3D10_SRV_DIMENSION_TEXTURE2D == D3D11_SRV_DIMENSION_TEXTURE2D );
    d3dsrvdesc.ViewDimension = (D3D_SRV_DIMENSION) D3D10_SRV_DIMENSION_TEXTURE2D;
    d3dsrvdesc.Texture2D.MostDetailedMip = 0;
    d3dsrvdesc.Texture2D.MipLevels = 1;
    //d3dsrvdesc.Texture2D.FirstArraySlice = 0;
    //d3dsrvdesc.Texture2D.ArraySize = 1;

    hresult = m_DevicePointer.pDevice->CreateShaderResourceView
        (
        pBBInitTextureVid[ uiIndex ],
        &d3dsrvdesc,
        &pBBInitSRV[ uiIndex ]
    );

    if ( FAILED( hresult ) )
    {
        WriteToLog
            (
            "CDXGIPresentRateSingleSC::InitializeTexturePair:  CreateShaderResourceView on video memory texture failed with error 0x%x.",
            hresult
            );

        goto Cleanup;
    };


    if ( pBBInitSRV[ uiIndex ] == NULL )
    {
        WriteToLog
            (
            "CDXGIPresentRateSingleSC::InitializeTexturePair:  CreateShaderResourceView succeeded but returned a NULL shader resource view pointer."
            );

        goto Cleanup;
    };


    bOverallResult = true;


Cleanup:
    return bOverallResult;
};


//-----------------------------------------------------------------------------
//
//  CreateTextures
//
//-----------------------------------------------------------------------------
bool CDXGIPresentRateSingleSC::CreateTextures()
{
    //
    // Initialize locals.
    //

    bool bOverallResult = false;


    ReleaseTextures();


    //
    // Validate members and state.
    //

    if ( m_DevicePointer.pDevice == NULL )
    {
        WriteToLog
            (
            "CDXGIPresentRateSingleSC::CreateTextures:  D3D10 device pointer is NULL."
            );

        goto Cleanup;
    };

    if ( m_pSwapChain == NULL )
    {
        WriteToLog
            (
            "CDXGIPresentRateSingleSC::CreateTextures:  Swap chain pointer is NULL."
            );

        goto Cleanup;
    };


    if
        (
        (!InitializeTexturePair(0, "TestTex00.dds")) ||
        (!InitializeTexturePair(1, "TestTex01.dds")) ||
        (!InitializeTexturePair(2, "TestTex02.dds"))
        )
    {
        WriteToLog( "Unable to initialize all textures." );

        goto Cleanup;
    };


    bOverallResult = true;


Cleanup:
    return bOverallResult;
};


//-----------------------------------------------------------------------------
//
//  ReleaseTextures
//
//-----------------------------------------------------------------------------
void CDXGIPresentRateSingleSC::ReleaseTextures()
{
    for
        (
        UINT uiTexIndex = 0;
    uiTexIndex < MAX_BUFFERS;
    ++uiTexIndex
        )
    {
        if ( pBBInitSRV[ uiTexIndex ] != NULL )
        {
            pBBInitSRV[ uiTexIndex ]->Release();
            pBBInitSRV[ uiTexIndex ] = NULL;
        }

        if ( pBBInitTextureVid[ uiTexIndex ] != NULL )
        {
            pBBInitTextureVid[ uiTexIndex ]->Release();
            pBBInitTextureVid[ uiTexIndex ] = NULL;
        }

        if ( pBBInitTextureSys[ uiTexIndex ] != NULL )
        {
            pBBInitTextureSys[ uiTexIndex ]->Release();
            pBBInitTextureSys[ uiTexIndex ] = NULL;
        }
    }
};


//-----------------------------------------------------------------------------
//
//  ReleaseTestCaseObjects
//
//-----------------------------------------------------------------------------
void CDXGIPresentRateSingleSC::ReleaseTestCaseObjects()
{
    DebugOut.Write("Entering ReleaseTestCaseObjects.\n");


    // Clean up DirectComposition if necessary
    m_DCOMPIsolation.DestroyDCompDeviceAndVisual();

    ReleaseTextures();

    //
    // Release our swap chains.
    //

    if ( m_pSwapChain != NULL )
    {
        m_pSwapChain->SetFullscreenState
            (
            FALSE,
            NULL
            );

        m_pSwapChain->Release();
        m_pSwapChain = NULL;
    };
    SAFE_RELEASE( m_pDXGIDevice );

    m_TestedSwapChain.ReleaseAllObjects();

    m_PrimaryAdapterDevice.ReleaseTexRenderResources();


    //
    // Release device objects and device.
    //

    if ( m_DevicePointer.pDevice != NULL )
    {
        // @DX11PORT [SEdmison]:
        // Replace this.
        //D3DX10UnsetAllDeviceObjects( m_DevicePointer.pDevice );

        m_DevicePointer.pDevice->Release();
        m_DevicePointer.pDevice = NULL;
    };

    m_PrimaryAdapterDevice.ReleaseAllObjects();



    //
    // Release our adapter pointer.
    //

    if ( m_AdapterPointer.pAdapter != NULL )
    {
        m_AdapterPointer.pAdapter->Release();
        m_AdapterPointer.pAdapter = NULL;
    };


    //
    // Break focus window association and release factory.
    //

    if ( m_FactoryPointer.pFactory != NULL )
    {
        HWND hwndCurrentFocus = NULL;

        m_FactoryPointer.pFactory->GetWindowAssociation( &hwndCurrentFocus );
        if ( hwndCurrentFocus != NULL )
        {
            m_FactoryPointer.pFactory->MakeWindowAssociation
                (
                NULL,
                0
                );
        };

        m_FactoryPointer.pFactory->Release();
        m_FactoryPointer.pFactory = NULL;
    };


    if ( m_hwndDeviceWindow != NULL )
    {
        if ( m_hwndDeviceWindow != m_hwndFocusWindow )
        {
            DestroyWindow( m_hwndDeviceWindow );
        };

        m_hwndDeviceWindow = NULL;
    };


    if ( m_hwndFocusWindow != NULL )
    {
        DestroyWindow( m_hwndFocusWindow );
        m_hwndFocusWindow = NULL;
    };


    DebugOut.Write("Exiting ReleaseTestCaseObjects.\n");
};


//-----------------------------------------------------------------------------
//
//  GetRefreshRateForDefault.  Gets the default monitor refresh rate
//
//-----------------------------------------------------------------------------
HRESULT CDXGIPresentRateSingleSC::GetRefreshRateForDefault
    (
    UINT * pRefreshRate
    )
{
    //
    // Initialize locals.
    //

    HRESULT hr = E_FAIL;


    //
    // Validate parameters.
    //

    if ( pRefreshRate == NULL )
    {
        WriteToLog
            (
            "CDXGIPresentRateSingleSC::GetRefreshRateForDefault:  pRefreshRate is NULL."
            );

        hr = E_POINTER;
        goto Cleanup;
    };


    //
    // Get the current mode for the default display device.
    //

    DEVMODE currentDisplayMode;
    ZeroMemory( &currentDisplayMode, sizeof( currentDisplayMode ) );
    currentDisplayMode.dmSize = sizeof( currentDisplayMode );
    currentDisplayMode.dmSpecVersion = DM_SPECVERSION;

    BOOL bGetCurrentModeResult = EnumDisplaySettings
        (
        NULL,
        ENUM_CURRENT_SETTINGS,
        &currentDisplayMode
        );

    if ( bGetCurrentModeResult != TRUE )
    {
        DWORD GetCurrentModeError = GetLastError();

        WriteToLog
            (
            "CDXGIPresentRateSingleSC::GetRefreshRateForDefault:  EnumDisplaySettings failed with error %u.",
            GetCurrentModeError
            );

        if ( GetCurrentModeError != ERROR_SUCCESS )
        {
            hr = HRESULT_FROM_WIN32( GetCurrentModeError );
        };

        goto Cleanup; 
    };


    //
    // This is an interlaced mode, then double the refresh rate.
    //

    if ( ( currentDisplayMode.dmDisplayFlags & DM_INTERLACED ) != 0 )
    {
        *pRefreshRate = currentDisplayMode.dmDisplayFrequency * 2;
    }
    else
    {
        *pRefreshRate = currentDisplayMode.dmDisplayFrequency;
    };


    hr = S_OK;


Cleanup:
    return hr;
};


//-----------------------------------------------------------------------------
//
//  GetRefreshRate
//
//-----------------------------------------------------------------------------
HRESULT CDXGIPresentRateSingleSC::GetRefreshRate
    (
    UINT * pRefreshRate,
    UINT uiDXGIAdapterOrdinal,
    UINT uiDXGIOutputOrdinal
    )
{
    //
    // Initialize locals.
    //

    HRESULT hr = E_FAIL;
    DXGI_FACTORY_POINTER  FactoryPointer = {NULL};
    IDXGIAdapter * pDXGIAdapterEnumMode = NULL;
    IDXGIOutput * pDXGIOutputEnumMode = NULL;


    //
    // Create a DXGI factory.
    //

    hr = g_Application.CreateDXGIFactory
        (
        &FactoryPointer
        );

    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "CDXGIPresentRateSingleSC::GetRefreshRate:  CreateDXGIFactory failed with error 0x%x.",
            hr
            );

        goto Cleanup;
    };


    //
    // Enumerate the specfied adapter.
    //

    hr = FactoryPointer.pFactory->EnumAdapters
        (
        uiDXGIAdapterOrdinal,
        &pDXGIAdapterEnumMode
        );

    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "CDXGIPresentRateSingleSC::GetRefreshRate:  EnumAdapters on adapter %u failed with error 0x%x.",
            uiDXGIAdapterOrdinal,
            hr
            );

        goto Cleanup;
    };


    //
    // Enumerate the specified output.
    //

    hr = pDXGIAdapterEnumMode->EnumOutputs
        (
        uiDXGIOutputOrdinal,
        &pDXGIOutputEnumMode
        );

    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "CDXGIPresentRateSingleSC::GetRefreshRate:  EnumOutputs on output %u failed with error 0x%x.",
            uiDXGIOutputOrdinal,
            hr
            );

        goto Cleanup;
    };


    //
    // Get a descriptor for the output.
    //

    DXGI_OUTPUT_DESC outputDesc;
    hr = pDXGIOutputEnumMode->GetDesc( &outputDesc);
    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "CDXGIPresentRateSingleSC::GetRefreshRate:  GetDesc on output %u failed with error 0x%x.",
            uiDXGIOutputOrdinal,
            hr
            );

        goto Cleanup; 
    };


    //
    // Determine the current mode for the output.
    //

    DEVMODEW currentDisplayMode;
    ZeroMemory( &currentDisplayMode, sizeof( currentDisplayMode ) );
    currentDisplayMode.dmSize = sizeof( currentDisplayMode );
    currentDisplayMode.dmSpecVersion = DM_SPECVERSION;

    BOOL bGetCurrentModeResult = EnumDisplaySettingsW
        (
        outputDesc.DeviceName,
        ENUM_CURRENT_SETTINGS,
        &currentDisplayMode
        );

    if ( bGetCurrentModeResult != TRUE )
    {
        DWORD GetCurrentModeError = GetLastError();

        WriteToLog
            (
            "CDXGIPresentRateSingleSC::GetRefreshRate:  EnumDisplaySettings on output %u failed with error %u.",
            uiDXGIOutputOrdinal,
            GetCurrentModeError
            );

        if ( GetCurrentModeError != ERROR_SUCCESS )
        {
            hr = HRESULT_FROM_WIN32( GetCurrentModeError );
        };

        goto Cleanup;
    };


    //
    // This is an interlaced mode, then double the refresh rate.
    //

    if ( ( currentDisplayMode.dmDisplayFlags & DM_INTERLACED ) != 0 )
    {
        *pRefreshRate = currentDisplayMode.dmDisplayFrequency * 2;
    }
    else
    {
        *pRefreshRate = currentDisplayMode.dmDisplayFrequency;
    };


    hr = S_OK;


Cleanup:
    if ( pDXGIOutputEnumMode != NULL )
    {
        pDXGIOutputEnumMode->Release();
        pDXGIOutputEnumMode = NULL;
    };

    if ( pDXGIAdapterEnumMode != NULL )
    {
        pDXGIAdapterEnumMode->Release();
        pDXGIAdapterEnumMode = NULL;
    };

    if ( FactoryPointer.pFactory != NULL )
    {
        FactoryPointer.pFactory->Release();
        FactoryPointer.pFactory = NULL;
    };


    return hr;
};


//-----------------------------------------------------------------------------
//
//  GetPerformanceCounterTime.  Gets the time in Milliseconds
//
//-----------------------------------------------------------------------------
HRESULT CDXGIPresentRateSingleSC::GetPerformanceCounterTime
    (
    __out __int64 * pTime
    )
{
    HRESULT hr = S_OK;
    LARGE_INTEGER count = {0};
    if (!QueryPerformanceCounter(&count))
    {
        DWORD e = GetLastError();
        hr = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
    };


    //
    // convert to ms
    //
    if (SUCCEEDED(hr))
    {
        hr = ConvertQpcToMs
            (
            count,
            pTime
            );
    };


    return hr;
};


//-----------------------------------------------------------------------------
//
//  ConvertQpcToMs.  Converts microseconds to  Milliseconds
//
//-----------------------------------------------------------------------------
HRESULT CDXGIPresentRateSingleSC::ConvertQpcToMs
    (
    __in LARGE_INTEGER &qpcTime,
    __out __int64* pTime
    )
{
    HRESULT hr = S_OK;
    static LARGE_INTEGER frequency = {0};

    if (frequency.QuadPart == 0)
    {
        if (!QueryPerformanceFrequency(&frequency))
        {
            DWORD e = GetLastError();
            hr = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
        }
    }

    if (SUCCEEDED(hr))
    {
        //
        // convert to ms
        //
        qpcTime.QuadPart = qpcTime.QuadPart * 1000000 / frequency.QuadPart;

        *pTime  = qpcTime.LowPart / 1000;
    }

    return hr;
};


//-----------------------------------------------------------------------------
//
//  TestWindowedSwapChain.  Calls present and uses the time it takes for present
//  to return to calculate frametime.  Adds up each frame time for total time.
//
//-----------------------------------------------------------------------------
HRESULT CDXGIPresentRateSingleSC::TestWindowedSwapChain
    (
    UINT syncInterval,
    UINT refreshRateInHz,
    DXGI_SWAP_EFFECT swapeffect,
    UINT bufferCount,
    DXGI_PRESENT_PARAMETERS* pPresentParameters,
    TEST_RESULT * pTestresult
    )
{
    //
    // Initialize locals.
    //

    DXGI_RESULT hr(m_pSwapChain, S_OK);
    *pTestresult = RESULT_FAIL;
    __int64 endTime = 0;
    __int64 start = 0;
    __int64 stop = 0;
    __int64 startBackUp = 0;
    __int64 stopBackUp = 0;
    __int64 frametimes[cuiNoOfPresentOperations];
    ID3D10RenderTargetView *pRTV=NULL; 
    ID3D10Texture2D *pDXGIBackBuffer = NULL;

    //
    // We need a back buffer to establish a render target view.
    //
    if (FAILED(hr = m_pSwapChain->GetBuffer(0, __uuidof( ID3D10Texture2D ), (void**)&pDXGIBackBuffer)))
    {
        WriteToLog("TestWindowedSwapChain: GetPerformanceCounterTime failed with error %s (0x%x).",D3DHResultToString(hr).c_str(), hr);
        goto Cleanup;
    }

    //
    // Create a rendertargetview so we can perform simple clear operations between present calls.  
    // We need to do some work on the GPU to ensure that the kernel inserts periodic GPU syncronization 
    // fences.  Otherwise sync-interval zero flip-model presents are completely unthrottled except 
    // for frame latency.  (Testing Present without any GPU work is deemed a lower-priority test case 
    // that we don't cover).
    // Our swapchain dimensions are small to keep the GPU work from adversely affecting this timing 
    // critical test.
    //
    // The addition of Clear calls also makes this test slightly more realistic since we actually have
    // content to present.
    //
    if (FAILED(hr = m_DevicePointer.pDevice->CreateRenderTargetView(pDXGIBackBuffer, NULL, &pRTV)))
    {
        WriteToLog("TestWindowedSwapChain: CreateRenderTargetView failed with error %s (0x%x).",D3DHResultToString(hr).c_str(), hr);
        goto Cleanup;    
    }

    for
        (
        UINT uiPresentCount = 0;
    uiPresentCount < cuiNoOfPresentOperations;
    ++uiPresentCount
        )
    {
        hr = GetPerformanceCounterTime( &start );
        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                "GetPerformanceCounterTime failed with error 0x%x.",
                hr
                );

            goto Cleanup;
        };

        startBackUp = timeGetTime();

        // Clear render target view (fade-in Red)
        FLOAT fRGBA[4] = { (float)uiPresentCount/(float)(cuiNoOfPresentOperations), 0.0f, 0.0f, 1.0f };
        m_DevicePointer.pDevice->ClearRenderTargetView(pRTV, fRGBA);

        if (pPresentParameters)
        {
            hr = m_pSwapChain->Present1
                (
                syncInterval,
                0,
                pPresentParameters
                );
        }
        else
        {
            hr = m_pSwapChain->Present
                (
                syncInterval,
                0
                );
        }

        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                "Present failed with error 0x%x.",
                hr
                );

            goto Cleanup;
        };


        hr = GetPerformanceCounterTime(&stop);        
        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                "GetPerformanceCounterTime failed with error 0x%x.",
                hr
                );

            goto Cleanup;
        };

        stopBackUp = timeGetTime( );

        if ( stop >= start )
        {
            frametimes[ uiPresentCount ] = stop - start;
        }
        else
        {
            WriteToLog
                (
                _T("QPC Timer rolled over at frame %u.  (Start:  %I64u, stop:  %I64u). Using time from timeGetTime"),
                uiPresentCount,
                start,
                stop
                );

            frametimes[ uiPresentCount ] = stopBackUp - startBackUp;
        };
    };


    for
        (
        UINT uiPresentCount = 0;
    uiPresentCount < cuiNoOfPresentOperations;
    ++uiPresentCount
        )
    {
        endTime += frametimes[ uiPresentCount ];

        WriteToLog
            (
            _T("Frame time %u:  %I64u"),
            uiPresentCount,
            frametimes[ uiPresentCount ]
        );
    };


    *pTestresult = VerifyPresentAverageTime
        (
        frametimes,
        cuiNoOfPresentOperations,
        cuiSkipPresentsAtStart,
        syncInterval,
        refreshRateInHz,
        swapeffect,    // DXGI_SWAP_EFFECT
        bufferCount,   // swap chain buffer count
        false          // not fullscreen
        );


    hr = S_OK;


Cleanup:
    SAFE_RELEASE(pRTV);
    SAFE_RELEASE(pDXGIBackBuffer);
    return hr;
};


//-----------------------------------------------------------------------------
//
//  TestWindowedBltSwapChainPartialPresentation.  Calls present and uses the time it takes for present
//  to return to calculate frametime.  Adds up each frame time for total time.
//
//-----------------------------------------------------------------------------
HRESULT CDXGIPresentRateSingleSC::TestWindowedBltSwapChainPartialPresentation
    (
    UINT refreshRateInHz,
    UINT bufferCount,
    DXGI_PRESENT_PARAMETERS* pPresentParameters,
    TEST_RESULT * pTestresult
    )
{
    //
    // Initialize locals.
    //
    DXGI_RESULT hr(m_pSwapChain, S_OK);
    *pTestresult = RESULT_FAIL;
    __int64 endTimelatency1 = 0;
    __int64 endTimelatencymax = 0;
    __int64 start = 0;
    __int64 stop = 0;
    __int64 startBackUp = 0;
    __int64 stopBackUp = 0;
    const UINT maxlatency = 16;
    __int64 frametimeslatency1[maxlatency];
    __int64 frametimeslatencymax[maxlatency];
    UINT OldMaxFrameLatency;
    ID3DTexture2D *pBB0 = NULL;
    ID3DTexture2D *pLargeRT = NULL;
    ID3DRenderTargetView *pLargeRTV = NULL;

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    DXGI_PRESENT_PARAMETERS PresentParameters = *pPresentParameters;
    PresentParameters.DirtyRectsCount = 2;

    D3D_TEXTURE2D_DESC LargeTex;
    LargeTex.Width = 8096;
    LargeTex.Height = 8096;
    LargeTex.MipLevels = 1;
    LargeTex.ArraySize = 1;
    LargeTex.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    LargeTex.SampleDesc.Count = 1;
    LargeTex.SampleDesc.Quality = 0;
    LargeTex.Usage = (D3D_USAGE) D3D10_USAGE_DEFAULT;
    LargeTex.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
    LargeTex.CPUAccessFlags = 0;
    LargeTex.MiscFlags = 0;

    const FLOAT ClearRGBA[] = { 0.f, 0.f, 1.f, 0.f };

    D3D10_BOX SingleTexel = { 0, 0, 0, 1, 1, 1 };

    hr = m_pSwapChain->GetBuffer( 0, __uuidof( *pBB0 ), reinterpret_cast< void** >( &pBB0 ) );
    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "TestWindowedBltSwapChainPartialPresentation:  GetBuffer 0 failed with error 0x%x.",
            hr
            );

        goto Cleanup;
    };

    hr = m_DevicePointer.pDevice->CreateTexture2D
        (
        &LargeTex,
        NULL,
        &pLargeRT
        );

    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "TestWindowedBltSwapChainPartialPresentation:  Large CreateTexture2D failed with error 0x%x.",
            hr
            );

        goto Cleanup;
    };

    hr = m_DevicePointer.pDevice->CreateRenderTargetView
        (
        pLargeRT,
        NULL,
        &pLargeRTV
        );

    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "TestWindowedBltSwapChainPartialPresentation:  Large CreateShaderResourceView failed with error 0x%x.",
            hr
            );

        goto Cleanup;
    };

    if (FAILED( hr = m_pDXGIDevice->GetMaximumFrameLatency( &OldMaxFrameLatency ) ))
    {
        WriteToLog
            (
            "GetMaximumFrameLatency failed with error 0x%x.",
            hr
            );
        goto Cleanup;
    }


    // Prime the pump:
    m_DevicePointer.pDevice->ClearRenderTargetView( pLargeRTV, ClearRGBA );
    m_DevicePointer.pDevice->CopySubresourceRegion( pBB0, 0, 0, 0, 0, pLargeRT, 0, &SingleTexel );

    hr = m_pSwapChain->Present1
        (
        0,
        0,
        &PresentParameters
        );

    for
        (
        UINT uiScenario = 0;
    uiScenario < 2;
    ++uiScenario
        )
    {
        Sleep( 100 ); // Sleep to let previous presents cleanup.

        __int64* frametimes = (uiScenario ? frametimeslatencymax : frametimeslatency1);

        if (FAILED( hr = m_pDXGIDevice->SetMaximumFrameLatency( uiScenario ? maxlatency : 1 ) ))
        {
            WriteToLog
                (
                "SetMaximumFrameLatency failed with error 0x%x.",
                hr
                );
            goto Cleanup;
        }

        for
            (
            UINT uiPresentCount = 0;
        uiPresentCount < maxlatency;
        ++uiPresentCount
            )
        {
            m_DevicePointer.pDevice->ClearRenderTargetView( pLargeRTV, ClearRGBA );
            m_DevicePointer.pDevice->CopySubresourceRegion( pBB0, 0, 0, 0, 0, pLargeRT, 0, &SingleTexel );

            QueryPerformanceCounter( reinterpret_cast< LARGE_INTEGER* >( &start ) );

            hr = m_pSwapChain->Present1
                (
                0,
                0,
                &PresentParameters
                );

            if ( FAILED( hr ) )
            {
                WriteToLog
                    (
                    "Present failed with error 0x%x.",
                    hr
                    );

                goto Cleanup;
            };


            QueryPerformanceCounter( reinterpret_cast< LARGE_INTEGER* >( &stop ) );

            if ( stop >= start )
            {
                frametimes[ uiPresentCount ] = stop - start;
            }
            else
            {
                WriteToLog
                    (
                    _T("QPC Timer rolled over at frame %u.  (Start:  %I64u, stop:  %I64u). Using time from timeGetTime"),
                    uiPresentCount,
                    start,
                    stop
                    );

                frametimes[ uiPresentCount ] = stopBackUp - startBackUp;
            };

        };

        for
            (
            UINT uiPresentCount = 0;
        uiPresentCount < maxlatency;
        ++uiPresentCount
            )
        {
            (uiScenario ? endTimelatencymax : endTimelatency1) += frametimes[ uiPresentCount ];

            WriteToLog
                (
                _T("Frame time %u:	%I64u"),
                uiPresentCount,
                frametimes[ uiPresentCount ]
            );
        };

    }

    DebugBreak();
#if 0
    *pTestresult = VerifyPresentAverageTime
        (
        frametimes,
        cuiNoOfPresentOperations,
        cuiSkipPresentsAtStart,
        syncInterval,
        refreshRateInHz,
        swapeffect,    // DXGI_SWAP_EFFECT
        bufferCount,   // swap chain buffer count
        false          // not fullscreen
        );
#endif

    hr = S_OK;


Cleanup:
    SAFE_RELEASE( pLargeRTV );
    SAFE_RELEASE( pLargeRT );
    SAFE_RELEASE( pBB0 );
    (void)m_pDXGIDevice->SetMaximumFrameLatency( OldMaxFrameLatency );
    return hr;
};

//-----------------------------------------------------------------------------
//
//  TestFullScreenSwapChain.  Uses GetFrameStatistics to determine when actual
//  first present occurs.  Uses QueryPerformanceCounter to get the frame time 
//  (the time before present and after).  Totals all frame times.
//
//-----------------------------------------------------------------------------
HRESULT CDXGIPresentRateSingleSC::TestFullScreenSwapChain
    (
    UINT syncInterval,
    UINT refreshRateInHz,
    DXGI_SWAP_EFFECT swapEffect,
    UINT bufferCount,
    TEST_RESULT * pTestresult
    )
{
    //
    // Initialize locals.
    //

    DXGI_RESULT hr(m_pSwapChain, E_FAIL);

    __int64 endTime = 0;
    __int64 start = 0;
    __int64 stop = 0;
    __int64 startBackUp = 0;
    __int64 stopBackUp = 0;


    DXGI_FRAME_STATISTICS baseLine;
    ZeroMemory( &baseLine, sizeof( baseLine ) );

    __int64 frametimesQpc[ cuiNoOfPresentOperations ];
    __int64 frametimesFrameStats[ cuiNoOfPresentOperations ];
    DXGI_FRAME_STATISTICS frametimesDxgi[ cuiNoOfPresentOperations ];


    //
    // Validate and initialize parameters.
    //

    if ( pTestresult == NULL )
    {
        WriteToLog
            (
            "CDXGIPresentRateSingleSC::TestFullScreenSwapChain:  pTestresult is NULL."
            );

        hr = E_POINTER;
        goto Cleanup;
    };

    *pTestresult = RESULT_FAIL;


    //
    // Initialize GetFrameStatistics with a present and get the baseline data
    //
    hr = m_pSwapChain->Present(1, 0);
    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "Present failed with error 0x%x.",
            hr
            );

        goto Cleanup;
    };

    //
    // Try this a few times
    //

    UINT numberOfRetries = 500;
    UINT sleepTime = 10;
    UINT count = 0;

    do
    {              
        hr = m_pSwapChain->GetFrameStatistics( &baseLine );
        if
            (
            hr == DXGI_ERROR_WAS_STILL_DRAWING ||
            hr == DXGI_ERROR_FRAME_STATISTICS_DISJOINT
            )
        {
            //
            //  Eat these errors and try again
            //
        }
        else if ( hr == DXGI_ERROR_UNSUPPORTED )
        {
            //
            // One of the causes of this error is that the window is not fullscreen
            //

            BOOL fullscreen = TRUE;
            IDXGIOutput * pTarget = NULL;
            HRESULT hGetFullscreenStateResult = m_pSwapChain->GetFullscreenState
                (
                &fullscreen,
                &pTarget
                );

            if ( FAILED( hGetFullscreenStateResult ) )
            {
                WriteToLog
                    (
                    "GetFullscreenState failed with error 0x%x.",
                    hGetFullscreenStateResult
                    );

                // Dump DXGI Journal to get clues as to why FullScreen swapchain is windowed
                if( g_Application.m_bLogDXGIJounrnal )
                    g_Application.InitiateJournalDump( true );

                goto Cleanup;
            };


            if ( pTarget != NULL )
            {
                pTarget->Release();
                pTarget = NULL;
            };


            if ( fullscreen == FALSE )
            {
                WriteToLog
                    (
                    _T("GetFrameStatistics failed with error DXGI_ERROR_UNSUPPORTED because swap chain is not full-screen.")
                    );

                // Dump DXGI Journal to get clues as to why FullScreen swapchain is windowed
                if( g_Application.m_bLogDXGIJounrnal )
                    g_Application.InitiateJournalDump( true );

                goto Cleanup;
            }
            else
            {
                WriteToLog
                    (
                    _T("GetFrameStatistics failed with error DXGI_ERROR_UNSUPPORTED.")
                    );

                goto Cleanup;
            };
        }
        else if ( FAILED( hr ) )
        {
            WriteToLog
                (
                _T("GetFrameStatistics failed with error 0x%x."),
                hr
                );

            goto Cleanup;
        };


        Sleep( sleepTime );

        ++count;
    } while (baseLine.PresentRefreshCount == 0 && count <= numberOfRetries);

    //
    // Call present and gather data
    //

    __int64 previousTime = 0;
    for
        (
        UINT uiPresentCount = 0;
    uiPresentCount < cuiNoOfPresentOperations;
    ++uiPresentCount
        )
    {
        hr = GetPerformanceCounterTime( &start );
        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                _T("GetPerformanceCounterTime failed with error 0x%x."),
                hr
                );

            goto Cleanup;
        };

        startBackUp = timeGetTime();

        hr = m_pSwapChain->Present
            (
            syncInterval,
            0
            );

        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                _T("Present failed with error 0x%x."),
                hr
                );

            goto Cleanup;
        };


        hr = GetPerformanceCounterTime( &stop );        

        if ( FAILED( hr ) )
        {         
            WriteToLog
                (
                _T("GetPerformanceCounterTime failed with error 0x%x."),
                hr
                );

            goto Cleanup;
        };

        hr = m_pSwapChain->GetFrameStatistics
            (
            &frametimesDxgi[ uiPresentCount ]
        );

        if ( hr == DXGI_ERROR_UNSUPPORTED )
        {
            BOOL fullscreen = TRUE;
            IDXGIOutput * pTarget = NULL;
            HRESULT hGetFullscreenStateResult = m_pSwapChain->GetFullscreenState
                (
                &fullscreen,
                &pTarget
                );

            if ( FAILED( hGetFullscreenStateResult ) )
            {
                WriteToLog
                    (
                    "GetFullscreenState failed with error 0x%x.",
                    hGetFullscreenStateResult
                    );

                goto Cleanup;
            };


            if ( pTarget != NULL )
            {
                pTarget->Release();
                pTarget = NULL;
            };


            if ( fullscreen != TRUE )
            {
                WriteToLog
                    (
                    _T("GetFrameStatistics failed with error DXGI_ERROR_UNSUPPORTED because swap chain is not full-screen.")
                    );

                // Dump DXGI Journal to get clues as to why FullScreen swapchain is windowed
                if( g_Application.m_bLogDXGIJounrnal )
                    g_Application.InitiateJournalDump( true );

                goto Cleanup;
            }
            else
            {
                WriteToLog
                    (
                    _T("GetFrameStatistics failed with error DXGI_ERROR_UNSUPPORTED.")
                    );

                goto Cleanup;
            };
        }
        else if ( FAILED( hr ) )
        {
            WriteToLog
                (
                _T("GetFrameStatistics failed with error 0x%x."),
                hr
                );

            goto Cleanup;
        };


        stopBackUp = timeGetTime( );


        if ( stop >= start )
        {
            frametimesQpc[ uiPresentCount ] = stop - start;
        }
        else
        {
            WriteToLog
                (
                _T("QPC Timer rolled over at frame %u.  (Start:  %I64u, stop:  %I64u). Using time from timeGetTime"),
                uiPresentCount,
                start,
                stop
                );

            frametimesQpc[ uiPresentCount ] = stopBackUp - startBackUp;

        };


        __int64 time = 0;
        hr = ConvertQpcToMs
            (
            frametimesDxgi[ uiPresentCount ].SyncQPCTime,
            &time
            );

        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                "ConvertQpcToMs failed with error 0x%x.",
                hr
                );

            goto Cleanup;
        };

        if ( time >= previousTime )
        {
            frametimesFrameStats[ uiPresentCount ] = time - previousTime;
            previousTime  = time;
        }
        else
        {
            WriteToLog
                (
                _T("QPC Timer rolled over at frame %u.  (Start:  %I64u, stop:  %I64u). Using time from timeGetTime"),
                uiPresentCount,
                previousTime,
                time
                );

            frametimesFrameStats[ uiPresentCount ] = stopBackUp - startBackUp;
            previousTime  = frametimesFrameStats[ uiPresentCount ];

        };       

    };


    if ( m_useQpcTimes )
    {
        WriteToLog(_T("Using frame times from QueryPerformanceCounter for verification."));
    }
    else
    {
        WriteToLog(_T("Using frame times from GetFrameStatistics.SyncQPCTime for verification."));
    };

    if
        (
        syncInterval == 0 ||
        m_useAverages
        )
    {
        //
        // For syncinterval = 0 use Average since it wont have a predictable frametime
        //

        *pTestresult = VerifyPresentAverageTime
            (
            m_useQpcTimes ? frametimesQpc : frametimesFrameStats,
            cuiNoOfPresentOperations,
            cuiSkipPresentsAtStart,
            syncInterval,
            refreshRateInHz,
            swapEffect,
            bufferCount,
            true  // Fullscreen
            );
    }
    else
    {
        UINT frameTimeInMilliseconds = 1000 * (syncInterval ? syncInterval: 1) / refreshRateInHz;

        *pTestresult = VerifyPresentOnTimeFrames
            (
            m_useQpcTimes ? frametimesQpc : frametimesFrameStats,
            cuiNoOfPresentOperations,
            cuiSkipPresentsAtStart,
            frameTimeInMilliseconds,
            2,//plus or minus
            cfPrecentLateFrames
            );
    };

    WriteToLog(_T("******************************* frame stats *******************************"));

    for
        (
        UINT uiPresentCount = 0;
    uiPresentCount < cuiNoOfPresentOperations;
    ++uiPresentCount
        )
    {
        if ( uiPresentCount < cuiSkipPresentsAtStart )
        {
            WriteToLog
                (
                _T("Skipped frametime from QPC[%u]=%I64u PresentCount %u,  PresentRefreshCount %u, SyncRefreshCount %u, CurrentPresent %u, frametime from GetFrameStats %I64u"),
                uiPresentCount,
                frametimesQpc[ uiPresentCount ],
                frametimesDxgi[ uiPresentCount ].PresentCount - baseLine.PresentCount,
                frametimesDxgi[ uiPresentCount ].PresentRefreshCount - baseLine.PresentRefreshCount,
                frametimesDxgi[ uiPresentCount ].SyncRefreshCount - baseLine.SyncRefreshCount,
                uiPresentCount,
                frametimesFrameStats[ uiPresentCount ]
            );
        }
        else
        {
            WriteToLog
                (
                _T("        frametime from QPC[%u]=%I64u PresentCount %u,  PresentRefreshCount %u, SyncRefreshCount %u, CurrentPresent %u, frametime from GetFrameStats %I64u"),
                uiPresentCount,
                frametimesQpc[ uiPresentCount ],
                frametimesDxgi[ uiPresentCount ].PresentCount - baseLine.PresentCount,
                frametimesDxgi[ uiPresentCount ].PresentRefreshCount - baseLine.PresentRefreshCount,
                frametimesDxgi[ uiPresentCount ].SyncRefreshCount - baseLine.SyncRefreshCount,
                uiPresentCount,
                frametimesFrameStats[ uiPresentCount ]
            );
        };
    };


    hr = S_OK;


Cleanup:
    return hr;
};

//-----------------------------------------------------------------------------
//
//  TestVaryingSyncInterval.
//
//-----------------------------------------------------------------------------
HRESULT CDXGIPresentRateSingleSC::TestVaryingSyncInterval 
    (
    UINT syncInterval1,
    UINT syncInterval2,
    UINT intervalDivisor,
    UINT refreshRateInHz,
    DXGI_SWAP_EFFECT swapEffect,
    UINT bufferCount,
    bool bDoNotSequence,
    TEST_RESULT * pTestresult
    )
{

    //
    // getframestats not yet working for windowed tests
    //
    assert(m_useQpcTimes == TRUE);

    //
    // Initialize locals.
    //

    DXGI_RESULT hr(m_pSwapChain, E_FAIL);

    __int64 endTime = 0;
    __int64 start = 0;
    __int64 stop = 0;
    __int64 startBackUp = 0;
    __int64 stopBackUp = 0;

    __int64 frametimesQpc[ cuiNoOfPresentOperations ];
    __int64 frametimesFrameStats[ cuiNoOfPresentOperations ];
    __int64 expectedFrameTimes[ cuiNoOfPresentOperations ];
    UINT frameSyncInterval[ cuiNoOfPresentOperations ]; // Record the sync interval used per frame for later logging.

    ZeroMemory( &frametimesFrameStats, sizeof( frametimesFrameStats ) );
    ZeroMemory( &frametimesQpc, sizeof( frametimesQpc ) );
    ZeroMemory( &expectedFrameTimes, sizeof( expectedFrameTimes ) );



    //
    // Validate and initialize parameters.
    //

    if ( pTestresult == NULL )
    {
        WriteToLog
            (
            "CDXGIPresentRateSingleSC::TestFullScreenSwapChain:  pTestresult is NULL."
            );

        hr = E_POINTER;
        goto Cleanup;
    };

    *pTestresult = RESULT_FAIL;


    //
    // Initialize GetFrameStatistics with a present and get the baseline data
    //
    hr = m_pSwapChain->Present(1, 0);
    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "Present failed with error 0x%x.",
            hr
            );

        goto Cleanup;
    };


    //
    // Try this a few times
    //

    UINT numberOfRetries = 500;
    UINT sleepTime = 10;
    UINT count = 0;


    //
    // Call present and gather data
    //

    __int64 previousTime = 0;
    __int64 syncIntervalTimeMs = 1000 / refreshRateInHz;

    for
        (
        UINT uiPresentCount = 0;
    uiPresentCount < cuiNoOfPresentOperations;
    ++uiPresentCount
        )
    {

        hr = GetPerformanceCounterTime( &start );
        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                _T("GetPerformanceCounterTime failed with error 0x%x."),
                hr
                );

            goto Cleanup;
        };

        startBackUp = timeGetTime();

        //
        // The present construction is assumed to be of the form
        //
        // SyncInterval2, SyncInterval2, SyncInterval1, ..., SyncInterval1 when flip sequential and
        // SyncInterval2, SyncInterval1, ..., SyncInterval1 when not flip sequential
        //
        // where SyncInterval1 is always 0. This is in order to make sure that in the flip
        // sequential case we are both getting a present of a non-zero syncinterval and a non-zero
        // syncinterval present that gets dropped. For the frame that gests dropped, the final zero
        // syncinterval frame will be drawn in its place, which means we need to treat the second and
        // final frames of the sequence differently in the flip sequential case.
        //
        USHORT subInterval = uiPresentCount % intervalDivisor;
        if( subInterval == 0 )
        {
            // record sync interval for logging later.
            frameSyncInterval[uiPresentCount] = syncInterval2;

            DWORD dwFlags = (bDoNotSequence ? DXGI_PRESENT_DO_NOT_SEQUENCE : 0);
            hr = m_pSwapChain->Present( syncInterval2, dwFlags );

            expectedFrameTimes[ uiPresentCount ] = syncInterval2 * syncIntervalTimeMs;
        }
        else if( subInterval == 1 && swapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL )
        {
            // record sync interval for logging later.
            frameSyncInterval[uiPresentCount] = syncInterval2;

            DWORD dwFlags = (bDoNotSequence ? DXGI_PRESENT_DO_NOT_SEQUENCE : 0);
            hr = m_pSwapChain->Present( syncInterval2, dwFlags );

            // 
            // In the FLIP_SEQUENTIAL case, use the timing for SyncInterval1 since this frame
            // will be dropped.
            //
            expectedFrameTimes[ uiPresentCount ] = 1;
        }
        else if( subInterval == (intervalDivisor - 1) && swapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL )
        {
            // record sync interval for logging later.
            frameSyncInterval[uiPresentCount] = syncInterval1;

            hr = m_pSwapChain->Present( syncInterval1, 0 );

            // 
            // In the FLIP_SEQUENTIAL case, use the timing for a syncinterval of 1 since the final
            // zero syncinterval frame will be presented.
            //
            expectedFrameTimes[ uiPresentCount ] = syncIntervalTimeMs;
        }
        else
        {
            // record sync interval for logging later.
            frameSyncInterval[uiPresentCount] = syncInterval1;

            hr = m_pSwapChain->Present( syncInterval1, 0 );

            //
            // If its syncInterval = 0 make it 1ms, for verification purposes 
            //
            // Note: Occasionally FlipModel Presents will take longer to return
            // if the queue is full.   This test sets the framelatency and 
            // number of buffers high enough so that these occasionally delays
            // do not significantly affect our validation.
            //
            expectedFrameTimes[ uiPresentCount ] = 1;
        }

        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                _T("Present failed with error 0x%x."),
                hr
                );

            goto Cleanup;
        };


        hr = GetPerformanceCounterTime( &stop );        

        if ( FAILED( hr ) )
        {         
            WriteToLog
                (
                _T("GetPerformanceCounterTime failed with error 0x%x."),
                hr
                );

            goto Cleanup;
        };

        stopBackUp = timeGetTime();

        //
        // Check for QPC rollover.  (happens once every hundred years or so depending on CPU frequency)
        //
        if ( stop >= start )
        {
            frametimesQpc[ uiPresentCount ] = stop - start;
        }
        else
        {
            WriteToLog
                (
                _T("QPC Timer rolled over at frame %u.  (Start:  %I64u, stop:  %I64u). Using time from timeGetTime"),
                uiPresentCount,
                start,
                stop
                );

            frametimesQpc[ uiPresentCount ] = stopBackUp - startBackUp;
        };

    };

    UINT numberPresentOperations1 = cuiNoOfPresentOperations;
    UINT numberPresentOperations2 = 0;

    *pTestresult = VerifyPresentAverageTime
        (
        frametimesQpc,
        expectedFrameTimes,
        cuiNoOfPresentOperations,
        cuiSkipPresentsAtStart,
        refreshRateInHz
        );

    WriteToLog(_T("******************************* Detailed Presentation call times *******************************"));
    if( swapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL)
    {
        WriteToLog(_T("============================================================================================================================================="));
        WriteToLog(_T("WARNING: Testing DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL.  Individual Present times and expected times will be out of sync and not match!  This is OK."));  
        WriteToLog(_T("The total/average times are still expected to match."));
        WriteToLog(_T("============================================================================================================================================="));
    }

    for
        (
        UINT uiPresentCount = 0;
    uiPresentCount < cuiNoOfPresentOperations;
    ++uiPresentCount
        )
    {
        if ( uiPresentCount < cuiSkipPresentsAtStart )
        {
            WriteToLog
                (
                _T("Skipped Present call time from QPC[%3u]=%3d, Expected: %3d, Sync Interval %u"),
                uiPresentCount,
                (int)frametimesQpc[ uiPresentCount ],
                (int)expectedFrameTimes[uiPresentCount ],
                frameSyncInterval[uiPresentCount]
            );
        }
        else
        {
            WriteToLog
                (
                _T("        Present call time from QPC[%3u]=%3d, Expected: %3d, Sync Interval %u"),
                uiPresentCount,
                (int)frametimesQpc[ uiPresentCount ],
                (int)expectedFrameTimes[uiPresentCount ],
                frameSyncInterval[uiPresentCount]
            );
        };
    };


    hr = S_OK;


Cleanup:
    return hr;
};

////////////////////////////////////////////////////////////////////////////////
//
// VerifyPresentOnTimeFrames
//
////////////////////////////////////////////////////////////////////////////////
TEST_RESULT CDXGIPresentRateSingleSC::VerifyPresentOnTimeFrames
    (
    __in_ecount(presentCount) __int64 *pFrametimes,
    __in UINT presentCount,
    __in UINT skipPresentsAtStart,
    __in UINT expectedFrameTime,
    __in UINT timeBuffer,
    __in FLOAT fractionalDeviation
    )
{
    TEST_RESULT testresult = RESULT_FAIL;

    if ( skipPresentsAtStart >= presentCount )
    {
        WriteToLog
            (
            _T("Invalid Parmeter: skipPresentsAtStart %d >= presentCount = %d"),
            skipPresentsAtStart,
            presentCount
            );

        return RESULT_FAIL;    
    };


    std::map< UINT, UINT >::iterator it;

    std::map< UINT, UINT > modeCounts;
    UINT countedFrames = (presentCount - skipPresentsAtStart);

    for(UINT i = skipPresentsAtStart; i < presentCount; i++)
    {
        it = modeCounts.find( static_cast<UINT>(pFrametimes[i]) );
        if ( it == modeCounts.end( ) )
        {
            //
            //  Item not found, insert it
            //
            modeCounts.insert(std::map< UINT, UINT >::value_type(static_cast<UINT>(pFrametimes[i]),1));
        }
        else
        {
            modeCounts[it -> first] = it -> second + 1;
        }
    }

    it = modeCounts.begin();
    UINT count = 0;

    while (it != modeCounts.end()) 
    {
        if (it -> first >= ((timeBuffer >= expectedFrameTime) ? 0 :(expectedFrameTime - timeBuffer)) && 
            it -> first <= (expectedFrameTime + timeBuffer))
        {
            count += it -> second;
        }

        it++;       
    }

    if ( count >= countedFrames - ( countedFrames * fractionalDeviation ) )
    {
        testresult = RESULT_PASS;    
    }
    else
    {
        testresult = RESULT_FAIL;
    }

    WriteToLog
        (
        _T("%d (%.2f%%) frames of %d frames were within %d ms of target time %d ms.  Expected at least %d (%.2f%%)."), 
        count,
        (float) count / (float) countedFrames * 100.0f,
        countedFrames,
        timeBuffer,
        expectedFrameTime,
        (UINT) ( countedFrames - ( countedFrames * fractionalDeviation ) ),
        (1.0f - fractionalDeviation) * 100.0f
        );

    return testresult;
};


////////////////////////////////////////////////////////////////////////////////
//
// VerifyPresentAverageTime
//
////////////////////////////////////////////////////////////////////////////////
TEST_RESULT CDXGIPresentRateSingleSC::VerifyPresentAverageTime
    (
    __in_ecount(presentCount) __int64 *pFrametimes,
    __in UINT presentCount,
    __in UINT skipPresentsAtStart,
    __in UINT syncInterval,
    __in UINT refreshRateInHz,
    __in DXGI_SWAP_EFFECT swapeffect,
    __in UINT buffercount,
    __in bool Fullscreen
    )
{
    TEST_RESULT testresult = RESULT_PASS;
    UINT countedFrames = (presentCount - skipPresentsAtStart);

    __int64 elapsedTimeInMilliseconds = 0;

    if ( skipPresentsAtStart >= presentCount )
    {
        WriteToLog
            (
            _T("Invalid Parameter: skipPresentsAtStart %d >= presentCount = %d"),
            skipPresentsAtStart,
            presentCount
            );

        return RESULT_FAIL;    
    }

    for (UINT uiPresentCount = skipPresentsAtStart; uiPresentCount < presentCount; ++uiPresentCount)
    {
        elapsedTimeInMilliseconds+=pFrametimes[uiPresentCount];
    }

    __int64 compareTimeInMilliseconds;

    // Compute the period with which to compare the elapsed time.
    compareTimeInMilliseconds = 1000 * countedFrames *
        (syncInterval ? syncInterval: 1) / refreshRateInHz;


    if (!syncInterval)
        // PresentationInterval is D3DPRESENT_INTERVAL_IMMEDIATE.
        // Verify that the duration of the Present operations is significantly smaller than
        // the duration expected for an equivalent set of Present operations synchronized
        // with the vertical retrace.
    {    

        //
        // Sync intervals are interpreted differently for different swap 
        // effects (at least for windowed mode).
        //
        // FLIP_SEQUENTIAL and FLIP_DISCARD are Flip Model swap 
        // effects.  For flip model, sync intervals tell DXGI how long to 
        // display a frame in vsyncs.
        //
        // Original SEQUENTIAL and DISCARD are Blt model swap effects.
        // for blt model, sync intervals tell DXGI how long to delay
        // (in vsyncs) before displaying a frame.
        //
        // Since this test just crudely validates the average framerate, the
        // validation mechanism is the same for both Blt and Flip Model
        // except for sync interval 0 which has subtle different behaviors on
        // Flip Model.  Normally sync interval 0 presents as fast as possible
        // with no vsync syncronization, but with Flip Model, DWM only 
        // processes frames on every vsync.  As a result, it's possible to
        // fill the queue and introduce a delay.  Basically with flip model
        // you can present N frames per vsync, where N is maximum frame 
        // latency.  With the Win8 GPU Syncronization work we are not 
        // as limited by number of swapchain buffers, but it is still a factor
        // as well.   DWM can not skip interval zero frames until the GPU is 
        // finished with them.  This is mostly a problem when using a small 
        // number of buffers relative to how deep you are queuing, or 
        // presenting interval zero frames exclusively.		
        // 
        // Note: older comments in this source code refer to sync interval 0 by 
        // it's D3D9 name PRESENT_INTERVAL_IMMEDIATE.  PRESENT_INTERVAL_IMMEDIATE
        // is not actually defined in DXGI.   DXGI just takes an integer value
        // in the present call specifying the sync interval.
        //
        WriteToLog(_T("Verifying Elapsed Time of all presents: %I64ums. Number of Presents=%u, SyncInterval=%u, RefreshRate=%u\n"),
            elapsedTimeInMilliseconds, 
            countedFrames,
            syncInterval,
            refreshRateInHz);

        if ( (swapeffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL /* || swapeffect == DXGI_SWAP_EFFECT_FLIP_DISCARD*/ ) &&
            (Fullscreen == false) )
        {
            //
            // Windowed Flip Model validation for sync interval 0
            //

            // Tweak the comparison time to account for queue limits on flip 
            // model sync-interval-zero frame rate.   Basically we can present 
            // N number of sync-interval-zero frames per vsync, where N is the
            // lessor of :
            //  * Number of buffers in our swapchain minus 1.  
            //	* Max Frame Latency 
            // Since the original calculation of compareTimeInMilliseconds assumed 
            // 1 present per vsync, let's simply divide it by our limiting factor.

            // Max Frame Latency defaults to 3.
            if (buffercount <= 3)
                compareTimeInMilliseconds/=(buffercount-1);
            else 
                compareTimeInMilliseconds/=3;

            // We'll use the deviation allowance normally used for non immediate (non sync-interval zero).

            UINT uiMaxDeviationInMilliseconds = (UINT)(cfMaxFractionalDeviationForNonImmediate *
                (float)compareTimeInMilliseconds);

            if (elapsedTimeInMilliseconds <
                (compareTimeInMilliseconds - uiMaxDeviationInMilliseconds))
            {
                WriteToLog("*** FAILURE: The elapsed time for all measured Presents was substantially smaller than the expected time of %I64ums (%I64u - %u) ***",compareTimeInMilliseconds - uiMaxDeviationInMilliseconds, compareTimeInMilliseconds, uiMaxDeviationInMilliseconds);
                testresult = RESULT_FAIL;
            }
            else if (elapsedTimeInMilliseconds >=
                (compareTimeInMilliseconds + uiMaxDeviationInMilliseconds))
            {
                WriteToLog("*** FAILURE: The elapsed time for all measured Presents was substantially greater than the expected time of %I64ums (%I64u + %u)***", compareTimeInMilliseconds + uiMaxDeviationInMilliseconds, compareTimeInMilliseconds, uiMaxDeviationInMilliseconds);
                testresult = RESULT_FAIL;
            }
            else
            {
                WriteToLog("The elapsed time for all measured Presents (%I64ums) is within tolerance of %I64ums +- %u", elapsedTimeInMilliseconds, compareTimeInMilliseconds, uiMaxDeviationInMilliseconds);
            }



        }
        else // Blt model validation for sync interval 0 (and all swap effects in fullscreen)
        {
            if (elapsedTimeInMilliseconds >= (UINT)(cfMaxFractionForImmediate * (float)compareTimeInMilliseconds))
            {
                //WriteToLog("The measured Present() rate with no synchronization was not significantly less than the rate of %ums (%d * %f) expected with synchronization.\n", (UINT)(cfMaxFractionForImmediate * (float)compareTimeInMilliseconds),compareTimeInMilliseconds,99,98,97 );
                WriteToLog("*** FAILURE: The measured Present() rate with no synchronization was not significantly less than the rate of %u (%I64u * %f) expected with synchronization. ***", ((UINT)(cfMaxFractionForImmediate * (float)compareTimeInMilliseconds)), compareTimeInMilliseconds,cfMaxFractionForImmediate,98,97 );
                testresult = RESULT_FAIL;
            }
        } // end Blt versus Flip model check
    } // End of sync interval 0 handling
    else
        // PresentationInterval is not D3DPRESENT_INTERVAL_IMMEDIATE.
        // Verify that the duration of the Present operations compares favourably with
        // the expected duration, as computed based on PresentationInterval and the refresh
        // rate.
    {

        WriteToLog(_T("Verifying elapsed time of all presents: %I64ums. Calculated Expected Time: %I64ums. Number of Presents=%u, SyncInterval=%u, RefreshRate=%u"),
            elapsedTimeInMilliseconds, 
            compareTimeInMilliseconds,
            countedFrames,
            syncInterval,
            refreshRateInHz);

        UINT uiMaxDeviationInMilliseconds = (UINT)(cfMaxFractionalDeviationForNonImmediate *
            (float)compareTimeInMilliseconds);

        if (elapsedTimeInMilliseconds <
            (compareTimeInMilliseconds - uiMaxDeviationInMilliseconds))
        {
            WriteToLog("*** FAILURE:The measured Present() rate was substantially smaller than the expected rate of %I64ums (%I64u - %u) ***",compareTimeInMilliseconds - uiMaxDeviationInMilliseconds, compareTimeInMilliseconds, uiMaxDeviationInMilliseconds);
            testresult = RESULT_FAIL;
        }
        else
            if (elapsedTimeInMilliseconds >=
                (compareTimeInMilliseconds + uiMaxDeviationInMilliseconds))
            {
                WriteToLog("*** FAILURE: The measured Present() rate was substantially greater than the expected rate of %I64ums (%I64u + %u) ***", compareTimeInMilliseconds + uiMaxDeviationInMilliseconds, compareTimeInMilliseconds, uiMaxDeviationInMilliseconds);
                testresult = RESULT_FAIL;
            }
    }

    return testresult;
}

////////////////////////////////////////////////////////////////////////////////
//
// VerifyPresentAverageTime
//
////////////////////////////////////////////////////////////////////////////////
TEST_RESULT CDXGIPresentRateSingleSC::VerifyPresentAverageTime
    (
    __in_ecount(presentCount) __int64 *pFrametimes,
    __in_ecount(presentCount) __int64 *pExpectedFrametimes,
    __in UINT presentCount,
    __in UINT skipPresentsAtStart,
    __in UINT refreshRateInHz
    )
{
    TEST_RESULT testresult = RESULT_PASS;

    __int64 elapsedTimeInMilliseconds = 0;
    __int64 expectedTimeInMilliseconds = 0;

    if ( skipPresentsAtStart >= presentCount)
    {
        WriteToLog
            (
            _T("Invalid Parameter: skipPresentsAtStart %d >= presentCount = %d"),
            skipPresentsAtStart,
            presentCount
            );

        return RESULT_FAIL;    
    }

    for (UINT uiPresentCount = skipPresentsAtStart; uiPresentCount < presentCount; ++uiPresentCount)
    {
        elapsedTimeInMilliseconds+=pFrametimes[uiPresentCount];
        expectedTimeInMilliseconds+=pExpectedFrametimes[uiPresentCount];
    }


    UINT uiMaxDeviationInMilliseconds = (UINT)(cfMaxFractionalDeviationForNonImmediate *
        (float)expectedTimeInMilliseconds);

    WriteToLog(_T("Verifying Elapsed Time of all presents: %I64ums. Expected Time: %I64ums (+-%ums). RefreshRate=%u\n"),
        elapsedTimeInMilliseconds, 
        expectedTimeInMilliseconds,
        uiMaxDeviationInMilliseconds,
        refreshRateInHz);

    if (elapsedTimeInMilliseconds <
        (expectedTimeInMilliseconds - uiMaxDeviationInMilliseconds))
    {
        WriteToLog("*** FAILURE: The measured Present() rate was substantially smaller than the expected rate of %I64ums (%I64u expected  - %u deviation ) ***", expectedTimeInMilliseconds - uiMaxDeviationInMilliseconds, expectedTimeInMilliseconds, uiMaxDeviationInMilliseconds);
        testresult = RESULT_FAIL;
    }
    else
        if (elapsedTimeInMilliseconds >=
            (expectedTimeInMilliseconds + uiMaxDeviationInMilliseconds))
        {
            WriteToLog("*** FAILURE: The measured Present() rate was substantially greater than the expected rate of %I64ums (%I64u expected + %u deviation) ***", expectedTimeInMilliseconds + uiMaxDeviationInMilliseconds, expectedTimeInMilliseconds, uiMaxDeviationInMilliseconds);
            testresult = RESULT_FAIL;
        }


        return testresult;
}

////////////////////////////////////////////////////////////////////////////////
//
// IsPixelFormatCompatibleWithSwapEffect()
//
// Validates that backbuffer format is compatible with Flip model swapeffects
// this varies based on feature level we are testing.
//
// Placed in CDXGIPresentRateSingleSC to enable sharing by all derived classes.
//
////////////////////////////////////////////////////////////////////////////////
bool CDXGIPresentRateSingleSC::IsPixelFormatCompatibleWithSwapEffect
    (__in DXGI_SWAP_EFFECT swapeffect, 
    __in DXGI_FORMAT pixelformat, 
    __in TEST_FEATURE_LEVEL level)
{
    //
    // Validate Flip Model compatible parameters
    //
    if ((swapeffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) 
        //        ||(swapeffect == DXGI_SWAP_EFFECT_FLIP_DISCARD) // uncomment when FLIP_DISCARD is available.
        )
    {
        switch (level)
        {
            // All levels except feature level 9 should
            // be in this list.  Please add them as they
            // are created.
        case TEST_FEATURE_10_0_LEVEL_10_0:
        case TEST_FEATURE_10_1_LEVEL_10_0:
        case TEST_FEATURE_10_1_LEVEL_10_1:
        case TEST_FEATURE_11_0_LEVEL_10_0:
        case TEST_FEATURE_11_0_LEVEL_10_1:
        case TEST_FEATURE_11_0_LEVEL_11_0:
            //uncomment these when DX11.1 support is added to DXGIFramework helpers.
            //            case TEST_FEATURE_11_1_LEVEL_10_0:  
            //            case TEST_FEATURE_11_1_LEVEL_10_1:
            //            case TEST_FEATURE_11_1_LEVEL_11_0:
            //            case TEST_FEATURE_11_1_LEVEL_11_1:
            // Validate back buffer formats compatible with FLIP_SEQUENTIAL on Feature level 10 or greater.
            switch (pixelformat)
            {
            case DXGI_FORMAT_R16G16B16A16_FLOAT:
            case DXGI_FORMAT_B8G8R8X8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_UNORM:
                break;
            default:
                WriteToLog("*** Flip Model Not supported with this pixel format. ***");                           
                return false;
            }

            break; // End of DX10 or higher switch case block.

        default:
            // These are the only formats available on feature level 9.
            switch (pixelformat)
            {
            case DXGI_FORMAT_B8G8R8X8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_UNORM:
                break;
            default:
                WriteToLog("*** Flip Model Not supported with this pixel format. ***");
                return false;
            }
        }

    } // End of Flip Model compatibility checks

    return true;
};
