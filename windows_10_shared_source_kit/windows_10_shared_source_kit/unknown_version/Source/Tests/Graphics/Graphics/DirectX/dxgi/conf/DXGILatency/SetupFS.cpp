//
// Project headers
//

#include "TestApp.h"

#define MINIMUM(a,b)  (a<b ? a : b)

DXGI_MODE_DESC TempFSModes[] =
{
    {800, 600, {60, 1}, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_UNSPECIFIED},
    {1024, 768, {60, 1}, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_UNSPECIFIED}
};

const UINT cuiNoOfTempFSModes = ARRAYSIZE( TempFSModes );


CFSTestCases::CFSTestCases()
{
};


CFSTestCases::~CFSTestCases()
{
};

TEST_RESULT CFSTestCases::CreateDeviceAndSwapChain
(
    int iModeIndexArg,
    DXGI_SWAP_EFFECT swapEffect,
    int iNumBuffersIndexArg
)
{
    //
    // Initialize locals.
    //

    TEST_RESULT OverallResult = RESULT_FAIL;
    bool bReturn = false;
    HRESULT hresult = E_FAIL;

    ReleaseTestCaseObjects();

    hresult = m_PrimaryAdapterDevice.InitializeDevice
    (
        NULL,
        g_Application.m_D3DOptions.DriverType,
        NULL,
        D3D10_CREATE_DEVICE_SINGLETHREADED,
        0,
        m_TestFeatureLevel,
        g_Application.m_pD3D10CreateDevice1     // if not NULL create device 1 
    );

    if
    (
        hresult == DXGI_ERROR_UNSUPPORTED ||
        hresult == E_NOINTERFACE ||
        hresult == E_NOTIMPL
    )
    {
        WriteToLog
        (
            "Could not create device."
        );

        OverallResult = RESULT_SKIP;
        goto Cleanup;
    }
    else if ( FAILED( hresult ) )
    {
        WriteToLog
        (
            "CFSTestCases::CreateDeviceAndSwapChain:  InitializeDevice failed with error 0x%x.",
            hresult
        );

        goto Cleanup;
    };

    if(!m_PrimaryAdapterDevice.GetDevicePtr( &m_pD3D10Device ))
    {
        WriteToLog("CFSTestCases::CreateDeviceAndSwapChain: m_PrimaryAdapterDevice.GetDevicePtr failed.");
        goto Cleanup;
    }

    if ( m_pD3D10Device == NULL )
    {
        WriteToLog
        (
            "CFSTestCases::CreateDeviceAndSwapChain:  GetDevicePtr returned NULL device pointer."
        );

        goto Cleanup;
    }

    if(!m_PrimaryAdapterDevice.GetFactory( &m_pFactory ))
    {
        WriteToLog("CFSTestCases::CreateDeviceAndSwapChain: m_PrimaryAdapterDevice.GetFactory failed.");
        goto Cleanup;  
    }

    if (m_pFactory == NULL)
    {
        WriteToLog
        (
            "CFSTestCases::CreateDeviceAndSwapChain:  GetFactory returned NULL factory pointer."
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
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        m_hProgInst,
        NULL
    );

    if ( m_hwndFocusWindow == NULL )
    {
        WriteToLog
        (
            "CFSTestCases::CreateDeviceAndSwapChain:  CreateWindow failed."
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


    m_hwndDeviceWindow = m_hwndFocusWindow;


    //
    // Associate the focus window with the DXGI factory.
    //

    DebugOut.Write("Calling MakeWindowAssociation in ExecuteTestCase.\n");

    hresult = m_pFactory->MakeWindowAssociation
    (
        m_hwndFocusWindow,
        0
    );

    if ( FAILED( hresult ) )
    {
        WriteToLog
        (
            "CFSTestCases::CreateDeviceAndSwapChain:  MakeWindowAssociation failed with error 0x%x.",
            hresult
        );

        goto Cleanup;
    };

    DebugOut.Write("CFSTestCases::CreateDeviceAndSwapChain:  MakeWindowAssociation succeeded.\n");


    //
    // Specify the swap chain properties.
    //

    //m_SCDesc.BufferDesc = TestedModes[ iModeIndexArg ];
    m_SCDesc.BufferDesc = TempFSModes[ iModeIndexArg ];
    
    // Skip unsupported formats.
    UINT FormatSupport = 0;
    hresult = m_pD3D10Device->CheckFormatSupport
    (
        m_SCDesc.BufferDesc.Format,
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
            "CFSTestCases::CreateDeviceAndSwapChain:  CheckFormatSupport failed with error 0x%x.",
            hresult
        );

        goto Cleanup;
    };

    //C_ASSERT( D3D10_FORMAT_SUPPORT_TEXTURE2D == D3D11_FORMAT_SUPPORT_TEXTURE2D );
    //C_ASSERT( D3D10_FORMAT_SUPPORT_RENDER_TARGET == D3D11_FORMAT_SUPPORT_RENDER_TARGET );
    //C_ASSERT( D3D10_FORMAT_SUPPORT_CPU_LOCKABLE == D3D11_FORMAT_SUPPORT_CPU_LOCKABLE );
    //C_ASSERT( D3D10_FORMAT_SUPPORT_DISPLAY == D3D11_FORMAT_SUPPORT_DISPLAY );
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

    m_SCDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    m_SCDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    m_SCDesc.SampleDesc.Count = 1;
    m_SCDesc.SampleDesc.Quality = 0;

    m_SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    m_SCDesc.BufferCount = iNumBuffersIndexArg;

    //m_SCDesc.MaxFrameLatency = 1;

    m_SCDesc.OutputWindow = m_hwndDeviceWindow;

    //m_SCDesc.DegradationPreference = DXGI_DEGRADE_TIMELINESS;

    m_SCDesc.Windowed = FALSE;

    m_SCDesc.SwapEffect = swapEffect;

    //m_SCDesc.BufferRotation = DXGI_MODE_ROTATION_IDENTITY;

    //m_SCDesc.RefreshRateInHz = 0.f;

    m_SCDesc.Flags = 0;


    //
    // Attempt the swap chain creation.
    //

    DebugOut.Write("CFSTestCases::CreateDeviceAndSwapChain:  Initializing swap chain.\n");

    if (!m_TestedSwapChain.InitializeSwapChain( &m_SCDesc ) )
    {
        WriteToLog
        (
            "CFSTestCases::CreateDeviceAndSwapChain:  InitializeSwapChain failed."
        );

        goto Cleanup;
    };


    //
    // Get a reference to our swap chain.
    //

    assert( m_pSwapChain == NULL );
    m_TestedSwapChain.GetSwapChainPtr( &m_pSwapChain );

    if ( m_pSwapChain == NULL )
    {
        WriteToLog
        (
            "CFSTestCases::CreateDeviceAndSwapChain:  GetSwapChainPtr returned NULL swap chain pointer."
        );

        goto Cleanup;
    };


    //
    // Dump details about the swap chain.
    //

    DebugOut.Write("***********************************\n");
    DebugOut.Write("CreateSwapChain succeeded!\n");

    {
        const DXGIFormatInfo *pBBFormatInfo = NULL;

        if ( pBBFormatInfo = CFormatInfoMap::Find( m_SCDesc.BufferDesc.Format ) )
        {
            DebugOut.Write(pBBFormatInfo->szDesc);
            DebugOut.Write("\n");
        }
    }

    DebugOut.Write("***********************************\n");


    //
    // Get a reference to the containing output.
    //

    IDXGIOutput * pContainingOutput = NULL;
    hresult = m_pSwapChain->GetContainingOutput( &pContainingOutput );

    if ( FAILED( hresult ) )
    {
        WriteToLog
        (
            "CFSTestCases::CreateDeviceAndSwapChain:  GetContainingOutput failed with error 0x%x.",
            hresult
        );

        goto Cleanup;
    };


    //
    // Make the swap chain full-screen on the output.
    //

    hresult = m_pSwapChain->SetFullscreenState
    (
        TRUE,
        pContainingOutput
    );


    pContainingOutput->Release();
    pContainingOutput = NULL;

    if ( FAILED( hresult ) )
    {
        WriteToLog
        (
            "CFSTestCases::CreateDeviceAndSwapChain:  SetFullscreenState failed with error 0x%x.",
            hresult
        );

        goto Cleanup;
    };

    OverallResult = RESULT_PASS;


Cleanup:
    return OverallResult;
};


TEST_RESULT CFSTestCases::TestLatency(UINT uiMaxLatency)
{
 
    unsigned int    uInitialPresentCount; // GetLastPresentCount may not start at zero.
    unsigned int    uPreviousPresentCount; // Tracks present count from previous iteration.
    unsigned int    uCurrentPresentCount;
    unsigned int    uRetiredFrames; 
    unsigned int    uQueuedFrames;
    unsigned int    uFrameLimit;
    unsigned int    uHighestRecordedQueuedFrames = 0;
    unsigned int    uFailCount = 0;

    bool            bFirstPassingCallToGetFrameStatsAfterDisjoint = false;
    bool            bGetFramesStatsSucceededAtLeastOnce = false;

    TEST_RESULT     OverallResult = RESULT_PASS; // Start out assuming we'll pass.
    HRESULT         hresult = E_FAIL;

    DXGI_FRAME_STATISTICS CurrentStats;
    DXGI_FRAME_STATISTICS PreviousStats;

    ID3D10RenderTargetView *pRTView=NULL; // Need a Rendertarget to perform Clear operations on our back buffers.

    //
    // Validate members and state.
    //
    if ( m_pSwapChain == NULL )
    {
        WriteToLog("Swap chain pointer is NULL.");
        OverallResult = RESULT_FAIL;
        goto Cleanup;
    };
    
    if ( uiMaxLatency == 0 )
    {
        WriteToLog("Max latency must be greater than zero.");
        OverallResult = RESULT_FAIL;
        goto Cleanup;
    };

    //
    // Create a render target view so we can perform a Clear operation before each present.
    //

    // Get backbuffer
    ID3D10Texture2D* pDXGIBackBuffer = NULL;
	hresult = m_pSwapChain->GetBuffer(0, __uuidof( ID3D10Texture2D ), (void**)&pDXGIBackBuffer);

    // Create render target view of backbuffer
    D3D10_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory( &rtvd, sizeof( rtvd ) );
	rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvd.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;

    hresult = m_pD3D10Device->CreateRenderTargetView(pDXGIBackBuffer, NULL, &pRTView);
    SAFE_RELEASE(pDXGIBackBuffer); // Done with backbuffer

    // Set render target view
    m_pD3D10Device->OMSetRenderTargets(1, &pRTView, NULL);
    
    // In fullscreen mode, Maximum latency is really only limited by the value 
    // passed to SetMaximumFrameLatency, though number of back buffers could
    // play a roll as well on some HW.
    //
    uFrameLimit = uiMaxLatency;

    WriteToLog("MaxFrameLatency: %u", uiMaxLatency);
    WriteToLog("SwapChain buffers: %d", m_iNumBuffersIndex);
    WriteToLog("Frame queueing should be limited by MaxFrameLatency, in this case we expect the limit to be %u frames.", uFrameLimit);

    //
    // Record the initial present count and use it to initialize the "Previous"
    // present count.  We will look to see that this number increases with 
    // additional Present calls.
    //
    hresult = m_pSwapChain->GetLastPresentCount(&uInitialPresentCount);
    if (FAILED(hresult))
    {
        WriteToLog("GetLastPresentCount failed with error 0x%x while trying to determine the intial present count.  Can't continue with test.",hresult);
        OverallResult = RESULT_FAIL;
        goto Cleanup;
    }

    WriteToLog("Initial Present Count: %u", uInitialPresentCount);

    uPreviousPresentCount = uInitialPresentCount;

    // Flush out any DISJOINT errors associated with first GetFrameStatistics call
    m_pSwapChain->GetFrameStatistics( &CurrentStats ); // Throw away call.

    // Zero out stats structures so we know if something isn't being updated.
    ZeroMemory( &CurrentStats, sizeof( CurrentStats ) );
    ZeroMemory( &PreviousStats, sizeof( PreviousStats ) );


    //
    // Perform MAX_NUMBER_OF_PRESENTS presents and monitor frame stats.
    //
    //      We will fail this test if the Delta between number of frames 
    //      retired and number of presents is greater than MaxFrameLatency.  
    //      If this happens, it suggests MaxFrameLatency is being ignored 
    //      and excessive frames are being queued.
    //
    //      We will fail if insufficient frames are queued.  Increasing the 
    //      frame latency should have some impact on the number of queued 
    //      frames.  If we only appear to be queuing the default of 3 (or 
    ///     less), then we'll fail the test.  Note that we will not enforce
    //      that the full MaxFrameLatency must be queueable.  It's always 
    //      possible that frames could be retired more quickly than we can 
    //      queue them up.  This is the most "squishy" of our validations, 
    //      and may need tweaking based on empirical data.
    //
    //      We will fail if GetLastPresentCount does not increment consistently 
    //      by 1.  We expect it to increment by 1 (and only 1) for each present
    //      call we make.   We do not expect it to start from 0.
    //
    //      Since this test runs in a controlled environment, we shouldn't 
    //      get unexpected DISJOINT errors from GetFrameStatistics, that 
    //      and other errors will also trigger a failure.
    //
    const unsigned int MAX_NUMBER_OF_PRESENTS = 100;   // this will result in a worse test case run time of about 6.5 seconds for 60 Hz display mode with a presentational interval of 4
    const unsigned int MAX_QUEUE_LIMIT_FAILURES = 10;  // Limit the log size if the queue limit is exceeded frequently.

    WriteToLog("Performing %u Present calls (frames) monitoring frame stats.", MAX_NUMBER_OF_PRESENTS);

    // Start our loop from 1.
    for (unsigned int uLoopCounter=1; uLoopCounter < MAX_NUMBER_OF_PRESENTS+1; uLoopCounter++)
    {

        // Clear render target view
        FLOAT fRGBA[4] = { (float)uLoopCounter/(float)MAX_NUMBER_OF_PRESENTS, 0.0f, 0.0f, 1.0f };
        m_pD3D10Device->ClearRenderTargetView(pRTView, fRGBA);

        // Present a frame.
        HRESULT hresult = m_pSwapChain->Present( m_nSyncInterval, 0 );
        if (FAILED( hresult ) )
        {
            WriteToLog("Frame %u: Present failed with error 0x%x.",uLoopCounter, hresult);
            OverallResult = RESULT_FAIL;
            goto Cleanup;
        };

        // Get the presented frame count.
        hresult = m_pSwapChain->GetLastPresentCount(&uCurrentPresentCount);
        if (FAILED(hresult))
        {
            WriteToLog("Frame %u: GetLastPresentCount failed with error %s(0x%x).", uLoopCounter, HResultToString(hresult), hresult);
            OverallResult = RESULT_FAIL;
            goto Cleanup;
        }

        // Verify that Present Count is incrementing correctly.
        if (uCurrentPresentCount != uPreviousPresentCount+1)
        {
            WriteToLog("Frame %u: GetLastPresentCount returned %u, we expected %u after % presents. It isn't incrementing correctly.",
                        uLoopCounter, uCurrentPresentCount, uPreviousPresentCount+1, uLoopCounter);
            OverallResult = RESULT_FAIL;
            goto Cleanup;
        }

        hresult = m_pSwapChain->GetFrameStatistics( &CurrentStats );
        if ( FAILED( hresult ) )
        {
            if ( hresult == DXGI_ERROR_FRAME_STATISTICS_DISJOINT )
            {
                WriteToLog("Frame %u: GetFrameStatistics failed with acceptable error of DXGI_ERROR_FRAME_STATISTICS_DISJOINT.", uLoopCounter);
                bFirstPassingCallToGetFrameStatsAfterDisjoint = false;  // Previous frame stats are voided.
            }
            else
            {
                WriteToLog("Frame %u: GetFrameStatistics failed with %s(0x%x) after %u presents.", uLoopCounter, HResultToString(hresult), hresult, uLoopCounter);
                OverallResult = RESULT_FAIL;
                goto Cleanup;
            }
        }
        else  // GetFrameStatistics succeeded!
        {
            bGetFramesStatsSucceededAtLeastOnce = true;

            // Is this our first successful call to GetFrameStats
            if (bFirstPassingCallToGetFrameStatsAfterDisjoint) 
            {
            // For our first passing call to GetFrameStats after a DISJOINT error 
            // we'll skip validation and just store stats for our next iteration.
                bFirstPassingCallToGetFrameStatsAfterDisjoint = true;
            }
            else // Verify that returned frame stats look good.
            {
                // Verify the returned PresentCount isn't higher than GetLastPresentCount.
                if (CurrentStats.PresentCount > uCurrentPresentCount)
                {
                    WriteToLog("Frame %u: GetFrameStatistics returned a PresentCount of %u frames, yet GetLastPresentCount returned %u!   PresentStats Framecount is higher than presented frames?", uLoopCounter, CurrentStats.PresentCount, uCurrentPresentCount);
                    OverallResult = RESULT_FAIL;
                    goto Cleanup;
                }

                uQueuedFrames = uCurrentPresentCount - CurrentStats.PresentCount;
                uRetiredFrames = CurrentStats.PresentCount - uInitialPresentCount;

                // Record the highest number of queued frames.
                if (uQueuedFrames > uHighestRecordedQueuedFrames) uHighestRecordedQueuedFrames = uQueuedFrames;

                // Verify that we aren't queuing too many presents.
                if (uQueuedFrames > uFrameLimit)
                {
                    //
                    // Bad!  We queued more frames than allowed by current frame latency settings.  Or present stats are completely broken.
                    //
                    WriteToLog("Frame %u: GetFrameStatistics indicates that only %u frames have been retired, yet we called present %u times.  %u frames are being queued.  Queue Limit is %u frames!", uLoopCounter, uRetiredFrames, uLoopCounter, uQueuedFrames, uFrameLimit);

                    OverallResult = RESULT_FAIL;

                    if (uFailCount++ > MAX_QUEUE_LIMIT_FAILURES)
                    {
                        WriteToLog("Too many queue limit failures.   Terminating test.");
                        goto Cleanup;
                    }		    
                }

                // Verify that if a new frame was retired since the last call,then stats should have increased by at least some small amount.
                if (CurrentStats.PresentCount > PreviousStats.PresentCount)
                {
                    // PresentCount increased, I expect other values to go up as well.  
                    // Did PresentRefreshCount increase?  If not, fail!
                    if (CurrentStats.PresentRefreshCount <= PreviousStats.PresentRefreshCount)
                    {
                        WriteToLog("Frame %u: GetFrameStatistics indicates a new frame was retired (PresentCount increased), but PresentRefreshCount did not increase.", uLoopCounter);
                        WriteToLog("          Previous PresentCount: %u, Current PresentCount: %u, Previous PresentRefreshCount: %u, Current PresentRefreshCount: %u.",
                                   PreviousStats.PresentCount,  CurrentStats.PresentCount, PreviousStats.PresentRefreshCount, CurrentStats.PresentRefreshCount );
                        WriteToLog("          Error Occurred after calling present %u times.",uLoopCounter );

                        OverallResult = RESULT_FAIL;    
                        goto Cleanup;
                    }

                }
            }

            // update the previous stats with the current stats in prep for the next iteration
            PreviousStats = CurrentStats;

            // Re-bind render targets after flip model present
            if( m_SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL )
                m_pD3D10Device->OMSetRenderTargets(1, &pRTView, NULL);
        }

         // Update the previous count with the new value in preparation for next iteration.
         uPreviousPresentCount = uCurrentPresentCount;
    } // End of Present loop

    WriteToLog("Completed %u frames.", MAX_NUMBER_OF_PRESENTS);

    // Did GetPresentStats succeed at least once without a DISJOINT error?  If not, fail the test.
    if (bGetFramesStatsSucceededAtLeastOnce == false)
    {
        WriteToLog("GetFrameStatistics never succeeded!  Always returned DXGI_ERROR_FRAME_STATISTICS_DISJOINT.");
        OverallResult = RESULT_FAIL;    
        goto Cleanup;
    }

    WriteToLog("The highest number of frames queued was %u", uHighestRecordedQueuedFrames);

    if (uHighestRecordedQueuedFrames < uFrameLimit)
    {
        //
        // If MaxFrameLatency is greater than 3, then lets make sure we could queue more than 3 frames.
        //  Ideally we should have consistently pegged the number of queued frames at MaxFrameLatency, but 
        // it's possible something is legitimately slowing us down and preventing us from filling the 
        // queue fast enough, or something is legitimately causing a flush.
        if ( (uFrameLimit >= 3) && (uHighestRecordedQueuedFrames <= 3) )
        {
            WriteToLog("With a queue limit of %u we expected a higher number for max queued frames.  Ideally it should have matched the queue limit.", uFrameLimit, uHighestRecordedQueuedFrames);     
        }
        else  // If Max latency at least had some affect, we'll just log a warning.
        {
                   WriteToLog("Warning:  We did not queue the maximum number of frames!  This is not necessarily a failure, but it's suspicious. Queue limit was %u frames", uFrameLimit);
        }

    } // End of uHighestRecordedQueuedFrames evaluation.


Cleanup:

    SAFE_RELEASE(pRTView);

    return OverallResult;
};


HRESULT CFSTestCases::SetMaximumFrameLatency(
    ID3D10Device* pD3D10Device,
    UINT value)
{
    TEST_RESULT testResult = RESULT_FAIL;

    IDXGIDevice1* pDXGIDevice1 = NULL;

    HRESULT hresult = pD3D10Device->QueryInterface(&pDXGIDevice1);
    if ( FAILED( hresult ) )
    {
        if ( hresult == E_NOINTERFACE )
        {
            WriteToLog("IDXGIDevice1 not supported, hresult = 0x%x. Skip the test",
                       hresult);
        }
        else
        {
            WriteToLog("QueryInterface for IDXGIDevice1 failed. It is required for Get\\Set MaximumFrameLatency tests. hresult = 0x%x",
                       hresult);
        }
        goto Cleanup;
    }

    hresult = pDXGIDevice1->SetMaximumFrameLatency(value);
    if ( FAILED( hresult ) )
    {
        WriteToLog
            (
            "SetMaximumFrameLatency failed. hresult = 0x%x",
            hresult
            );
        goto Cleanup;
    }

Cleanup:
    if ( pDXGIDevice1 != NULL )
    {
        pDXGIDevice1->Release();
        pDXGIDevice1 = NULL;
    };


    return hresult;
}

TEST_RESULT CFSTestCases::VerifyGetMaximumFrameLatency(
    ID3D10Device* pD3D10Device,
    UINT expectedValue)
{
    TEST_RESULT testResult = RESULT_FAIL;
    IDXGIDevice1* pDXGIDevice1 = NULL;

    HRESULT hresult = pD3D10Device->QueryInterface(&pDXGIDevice1);
    if ( FAILED( hresult ) )
    {
        WriteToLog
            (
            "QueryInterface for IDXGIDevice1 failed. It is required for Get\\Set MaximumFrameLatency tests. hresult = 0x%x",
            hresult
            );

        testResult = RESULT_FAIL;
        goto Cleanup;
    }

    UINT maxFrameLatency = 0;
    hresult = pDXGIDevice1->GetMaximumFrameLatency(&maxFrameLatency);
    if ( FAILED( hresult ) )
    {
        WriteToLog
            (
            "GetMaximumFrameLatency failed. hresult = 0x%x",
            hresult
            );

        testResult = RESULT_FAIL;
        goto Cleanup;
    }

    //
    //  m_iMaxFrameLatencyIndex == 0 means GetMaximumFrameLatency should return default
    //
    if ( (maxFrameLatency != expectedValue && expectedValue != 0 ) || 
        (expectedValue == 0 && maxFrameLatency != DEFAULT_FRAME_LATENCY))
    {
        WriteToLog
            (
            "maxFrameLatency[%u](returned by GetMaximumFrameLatency) != expectedValue[%u](set by SetMaximumFrameLatency) when expectedValue != 0"
            "maxFrameLatency[%u](returned by GetMaximumFrameLatency) != DEFAULT_FRAME_LATENCY[%u] when expectedValue == 0",
            maxFrameLatency,
            expectedValue,
            maxFrameLatency,
            (UINT)DEFAULT_FRAME_LATENCY
            );

        testResult = RESULT_FAIL;        
        goto Cleanup;
    }
    
    testResult = RESULT_PASS;

Cleanup:
    if ( pDXGIDevice1 != NULL )
    {
        pDXGIDevice1->Release();
        pDXGIDevice1 = NULL;
    };

    return testResult;
}

TEST_RESULT CFSTestCases::ExecuteTestCase()
{
    DebugOut.Write("Entering ExecuteTestCase.\n");


    //
    // Initialize locals.
    //
    TEST_RESULT test_result = RESULT_FAIL;
    TEST_RESULT temp_test_result = RESULT_FAIL;
    HRESULT hresult = S_OK;
    ID3D10Device* pD3D10Device = NULL;

    clsTestableDevice extraDevice( (CTest *) this);

    //
    // Validate members and state.
    //

    if ( m_iModeIndex < 0 )
    {
        WriteToLog
            (
            "Invalid mode index %d.", m_iModeIndex
            );

        goto Cleanup;
    };

    //
    // Create our device and swap chain.
    //

    temp_test_result = CreateDeviceAndSwapChain
        (
        m_iModeIndex,
        m_SwapEffect,
        m_iNumBuffersIndex
        );

    if ( temp_test_result != RESULT_PASS )
    {
        if ( temp_test_result != RESULT_SKIP )
        {
            WriteToLog
                (
                "CreateDeviceAndSwapChain failed."
                );
        };

        test_result = temp_test_result;
        goto Cleanup;
    };

    if(m_iModeIndex != m_iLastModeIndex)
    {
        Sleep(5000);
    }

    m_iLastModeIndex = m_iModeIndex;


    if (m_TestType == SECOND_DEVICE_CREATED_BEFORE_SET_TESTS)
    {
        hresult = extraDevice.InitializeDevice
        (
            NULL,
            g_Application.m_D3DOptions.DriverType,
            NULL,
            D3D10_CREATE_DEVICE_SINGLETHREADED,
            0,
            m_TestFeatureLevel,
            g_Application.m_pD3D10CreateDevice1
        );

        if
        (
            hresult == DXGI_ERROR_UNSUPPORTED ||
            hresult == E_NOINTERFACE ||
            hresult == E_NOTIMPL
        )
        {
            WriteToLog
                (
                "Could not create device 0x%x.",
                hresult
                );

            test_result = RESULT_SKIP;
            goto Cleanup;
        }
        else if ( FAILED( hresult ) )
        {
            WriteToLog
                (
                "InitializeDevice for extra device failed with error 0x%x.",
                hresult
                );
            test_result = RESULT_SKIP;
            goto Cleanup;
        };
    }

    if (m_TestType != DEFAULT_TESTS)
    {
        assert( g_Application.m_pCreateDXGIFactory1 != NULL );

        HRESULT hr = SetMaximumFrameLatency(
            m_pD3D10Device,
            m_iMaxFrameLatencyIndex);
        
        if(FAILED(hr))
        {
            if (hr == E_NOINTERFACE || hr == DXGI_ERROR_UNSUPPORTED )
            {
                test_result = RESULT_SKIP;
            }
            else
            {
                WriteToLog(_T("SetMaximumFrameLatency failed"));  
                test_result = RESULT_FAIL;
            }
            goto Cleanup;
        }

        if(m_iMaxFrameLatencyIndex == 0)
        {
            WriteToLog(_T("Setting m_iMaxFrameLatencyIndex to default %d because SetMaximumFrameLatency(0)"), DEFAULT_FRAME_LATENCY);
            m_iMaxFrameLatencyIndex = DEFAULT_FRAME_LATENCY;
        }

        temp_test_result = VerifyGetMaximumFrameLatency(
            m_pD3D10Device,
            m_iMaxFrameLatencyIndex); 

        if ( temp_test_result != RESULT_PASS )
        {
            WriteToLog(_T("VerifyGetMaximumFrameLatency failed"));  
            test_result = temp_test_result;
            goto Cleanup;
        }

    }

    if (m_TestType == SECOND_DEVICE_CREATED_AFTER_SET_TESTS)
    {
        hresult = extraDevice.InitializeDevice
        (
            NULL,
            g_Application.m_D3DOptions.DriverType,
            NULL,
            D3D10_CREATE_DEVICE_SINGLETHREADED,
            0,
            m_TestFeatureLevel,
            g_Application.m_pD3D10CreateDevice1
        );

        if
        (
            hresult == DXGI_ERROR_UNSUPPORTED || 
            hresult == E_NOINTERFACE ||
            hresult == E_NOTIMPL
        )
        {
            WriteToLog
                (
                "Could not create device 0x%x.",
                hresult
                );

            test_result = RESULT_SKIP;
            goto Cleanup;
        }
        else if ( FAILED( hresult ) )
        {
            WriteToLog
                (
                "InitializeDevice for extra device failed with error 0x%x.",
                hresult
                );
            test_result = RESULT_SKIP;
            goto Cleanup;
        };
    }

    if (m_TestType == SECOND_DEVICE_CREATED_BEFORE_SET_TESTS ||
        m_TestType == SECOND_DEVICE_CREATED_AFTER_SET_TESTS)
    {
        //
        // Just checking the latency on the new device dont actually need to verify other than
        // making sure GetMaximumFrameLatency is default
        //
        if (!extraDevice.GetDevicePtr( &pD3D10Device ))
        {
            WriteToLog("extraDevice.GetDevicePtr failed.");
            goto Cleanup;
        }

        if (pD3D10Device == NULL)
        {
            WriteToLog("extraDevice.GetDevicePtr:  GetDevicePtr returned NULL device pointer.");
            goto Cleanup;
        }

        test_result = VerifyGetMaximumFrameLatency(
            pD3D10Device,
            DEFAULT_FRAME_LATENCY);

        if ( pD3D10Device != NULL )
        {
            pD3D10Device->Release();
            pD3D10Device = NULL;
        }

        if ( test_result != RESULT_PASS )
        {
            WriteToLog(_T("VerifyGetMaximumFrameLatency failed"));
            test_result = temp_test_result;
            goto Cleanup;
        }
    }
    else
    {
         test_result = TestLatency(m_iMaxFrameLatencyIndex);
    }

Cleanup:
    ReleaseTestCaseObjects();

    DebugOut.Write("Exiting ExecuteTestCase.\n");

    return test_result;
};


