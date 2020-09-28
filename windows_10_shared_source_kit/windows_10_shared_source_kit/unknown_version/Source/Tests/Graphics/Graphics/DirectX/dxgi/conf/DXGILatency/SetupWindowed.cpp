//****************************************************************************
// SetupWindowed.cpp
//
// Contains class definition for CWinTestCases.   Creates and manages 
// swapchain and performs core test execution for Windowed Mode cases.
// Based on (but not derived from) CFSTestCases.
//
// With Windows 8, we introduced "Flip Model" swap effects which work
// with DWM to allow for Windowed mode frame latency control.
// Prior to Windows 8, SetMaxFrameLatency only worked in fullscreen
// modes.
//
//  See derived classes:
//      CSetMaximumFrameLatencyWindowed (SetMaxFrameLatencyWindowed.cpp)
//      CDefaultLatencyWindowed (DefaultLatencyWindowed.cpp)
//
// Class Hierarchy:   
//  CWinTestCases  (SetupWindowed.cpp)
//      CDXGILatencySingleSC
//          CDXGILatency
//              CDXGITest
//                  CTest
//                  CVariationGenerator
//	            clsBasicWindowClass
//	            CTestDXGISurfaceUtils
//	            clsD3D10Ref
//
// History:
//  6/21/2010 - olanh - Initial creation
//****************************************************************************

// Project headers
//

#include "TestApp.h"
#include <ParameterHelper.h>

//*****************************************************************************
// Constructor
//*****************************************************************************
CWinTestCases::CWinTestCases() 

{

    // Give our DCOMP isolation object a means to log.
    m_DCOMPIsolation.SetLogger(GetFramework()->GetLogWrapper());
};

//*****************************************************************************
// Destructor
//*****************************************************************************
CWinTestCases::~CWinTestCases()
{
};

//*****************************************************************************
// IsWindows8
// 
// Returns true if we are running on Windows8 or later OS.
//
// With Windows 8 M2 we are no longer queue limited by the number of back 
// buffers in our swapchain.  We are limited only by Max Frame latency.
// We need to detect when we're running Win8 and later and behave differently.
//*****************************************************************************
bool CWinTestCases::IsWindows8()
{
   OSVERSIONINFOEX osvi;
   DWORDLONG dwlConditionMask = 0;
   BYTE op=VER_GREATER_EQUAL;

   // Initialize the OSVERSIONINFOEX structure.

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
   osvi.dwMajorVersion = 6;
   osvi.dwMinorVersion = 1;
   osvi.dwBuildNumber = 7850;// Roughly associated with the start of Win8 M2 when this feature was introduced.

   // Initialize the condition mask.

   VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, op );
   VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, op );
   VER_SET_CONDITION( dwlConditionMask, VER_BUILDNUMBER, op );

   // Perform the test.

   return VerifyVersionInfo(
      &osvi, 
      VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER,
      dwlConditionMask) != FALSE;

}


//*****************************************************************************
// CreateDeviceAndSwapChain
//*****************************************************************************
TEST_RESULT CWinTestCases::CreateDeviceAndSwapChain
(
    DXGI_FORMAT PixelFormat,
    DXGI_SWAP_EFFECT swapEffect,
    unsigned int uNumBuffers
)
{
    //
    // Initialize locals.
    //

    TEST_RESULT OverallResult = RESULT_FAIL;
    bool bReturn = false;
    HRESULT hresult = E_FAIL;

    ReleaseTestCaseObjects();

    if(g_Application.m_bDFlip)
    {
        if(m_tp_bWindowless || swapEffect != DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL)
        {
            WriteToLog("Parameteres Not Applicable to DirectFLIP.");
            OverallResult = RESULT_SKIP;
            goto Cleanup;
        }
    }

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
            "CWinTestCases::CreateDeviceAndSwapChain:  InitializeDevice failed with error 0x%x.",
            hresult
        );

        goto Cleanup;
    };

    if(!m_PrimaryAdapterDevice.GetDevicePtr( &m_pD3D10Device ))
    {
        WriteToLog("CWinTestCases::CreateDeviceAndSwapChain: m_PrimaryAdapterDevice.GetDevicePtr failed.");
        goto Cleanup;
    }

    if ( m_pD3D10Device == NULL )
    {
        WriteToLog
        (
            "CWinTestCases::CreateDeviceAndSwapChain:  GetDevicePtr returned NULL device pointer."
        );

        goto Cleanup;
    }

    //
    // Now that we have a device, let's verify that our pixel format is supported.
    //
    UINT uSupportBitFeild=0;
    hresult = m_pD3D10Device->CheckFormatSupport(m_PixelFormat, &uSupportBitFeild);
    if ( FAILED( hresult ) )
    {
        WriteToLog( "CWinTestCases::CreateDeviceAndSwapChain:  CheckFormatSupport failed with error 0x%x.  This is unexpected!", hresult );
        goto Cleanup;
    }
    
    // Do we support this format as a rendertarget. (Closest thing to a swapchain)
    if ( (uSupportBitFeild & D3D10_FORMAT_SUPPORT_RENDER_TARGET) == 0)
    {
        // If not, let's skip the test.
        OverallResult = RESULT_SKIP; 
        WriteToLog( "Pixel format %s is not supported.  Skipping test case.", ToString(m_PixelFormat) );
        goto Cleanup;
    }

    //
    // Get our DXGI Factory
    //
    if(!m_PrimaryAdapterDevice.GetFactory( &m_pFactory ))
    {
        WriteToLog("CWinTestCases::CreateDeviceAndSwapChain: m_PrimaryAdapterDevice.GetFactory failed.");
        goto Cleanup;  
    }

    if (m_pFactory == NULL)
    {
        WriteToLog
        (
            "CWinTestCases::CreateDeviceAndSwapChain:  GetFactory returned NULL factory pointer."
        );

        goto Cleanup;
    }

    //
    // Create a focus / device window.
    //

    // Create a small window to help ensure we can keep the present queue full
    m_hwndFocusWindow = CreateWindow
    (
        m_szFocusWindowClassName,
        "DXGILatency Test - Focus Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        100,
        100,
        NULL,
        NULL,
        m_hProgInst,
        NULL
    );

    if ( m_hwndFocusWindow == NULL )
    {
        WriteToLog
        (
            "CWinTestCases::CreateDeviceAndSwapChain:  CreateWindow failed."
        );

        goto Cleanup;
    };

    const BOOL bDisable = TRUE;
    (void)DwmSetWindowAttribute( m_hwndFocusWindow, DWMWA_TRANSITIONS_FORCEDISABLED, &bDisable, sizeof( bDisable ) );

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
            "CWinTestCases::CreateDeviceAndSwapChain:  MakeWindowAssociation failed with error 0x%x.",
            hresult
        );

        goto Cleanup;
    };

    DebugOut.Write("CWinTestCases::CreateDeviceAndSwapChain:  MakeWindowAssociation succeeded.\n");

    if(g_Application.m_bDFlip)
    {
        // NOTE: In order for DirectFlip to work, satisfy-
        // 1) Test Window covers Entire Display Area
        // 2) Swapchain Buffers need to match the window size.
        // Resize window to match entire Display area here.        
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
    // Specify the swap chain properties.
    //

    // These are the parameters that we cycle on.
    m_SCDesc.SwapEffect = swapEffect;
    m_SCDesc.BufferDesc.Format = m_PixelFormat;
    m_SCDesc.BufferCount = uNumBuffers;

    // Fill out the rest of the struct
    m_SCDesc.BufferDesc.Width = 501;  // Width and Height are abitrary
    m_SCDesc.BufferDesc.Height = 301;
    if(g_Application.m_bDFlip)
    {   //Swapchain Buffers need to match the window size for DirectFlip
        m_SCDesc.BufferDesc.Width = GetSystemMetrics(SM_CXSCREEN);
        m_SCDesc.BufferDesc.Height = GetSystemMetrics(SM_CYSCREEN);
    }
    m_SCDesc.BufferDesc.RefreshRate.Numerator = 0;  // Not running fullscreen, so refreshrate doesn't matter.
    m_SCDesc.BufferDesc.RefreshRate.Denominator = 1;
    m_SCDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // Doesn't matter for windowed mode.
    m_SCDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;// Doesn't matter for windowed mode.

    m_SCDesc.SampleDesc.Count = 1;
    m_SCDesc.SampleDesc.Quality = 0;

    m_SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    //m_SCDesc.MaxFrameLatency = 1;

    m_SCDesc.OutputWindow = m_tp_bWindowless ? NULL : m_hwndDeviceWindow;

    //m_SCDesc.DegradationPreference = DXGI_DEGRADE_TIMELINESS;

    m_SCDesc.Windowed = TRUE;

    //m_SCDesc.BufferRotation = DXGI_MODE_ROTATION_IDENTITY;

    //m_SCDesc.RefreshRateInHz = 0.f;

    m_SCDesc.Flags = 0 ;


    //
    // Attempt the swap chain creation.
    //

    DebugOut.Write("CWinTestCases::CreateDeviceAndSwapChain:  Initializing swap chain.\n");

    if (!m_TestedSwapChain.InitializeSwapChain1( &m_SCDesc ) )
    {
        WriteToLog
        (
            "CWinTestCases::CreateDeviceAndSwapChain:  InitializeSwapChain failed."
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
            "CWinTestCases::CreateDeviceAndSwapChain:  GetSwapChainPtr returned NULL swap chain pointer."
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

    // Instantiate DCOMP if necessary
    if (m_tp_bWindowless)
    {
        if (FAILED(m_DCOMPIsolation.CreateAndAssociateDCOMPVisual(m_pSwapChain, m_hwndDeviceWindow)))
            goto Cleanup;
    }

    OverallResult = RESULT_PASS;

Cleanup:
    return OverallResult;
};


//****************************************************************************
// TestLatency
// Called by ExecuteTest to perform latency validation.  This fuction performs 
// a series of Presents followed by GetFrameStatistics.  It looks to see if 
// the delta between the PresentCount in GetFrameStats and it's own count is
// ever higher than the max frame latency. If so, it fails.
//
// Note that as of Windows 8 M1, frame latency can be limited by both the 
// SetMaxFrameLatency call and the number of back buffers in a swapchain.
//****************************************************************************
TEST_RESULT CWinTestCases::TestLatency(UINT uiMaxLatency)
{

    unsigned int    uInitialPresentCount = 0; // GetLastPresentCount may not start at zero.
    unsigned int    uPreviousPresentCount = 0; // Tracks present count from previous iteration.
    unsigned int    uCurrentPresentCount = 0;
    unsigned int    uRetiredFrames = 0; 
    unsigned int    uQueuedFrames = 0;
    unsigned int    uFrameLimit = 0;
    unsigned int    uFlipLatency = 0; // Flip has an extra frame of inherent latency
    unsigned int    uCombinedLimit = 0;
    unsigned int    uHighestRecordedQueuedFrames = 0;
    unsigned int    uFailCount = 0;

    bool            bFirstPassingCallToGetFrameStatsAfterDisjoint = false;
    bool            bGetFramesStatsSucceededAtLeastOnce = false;

    TEST_RESULT     OverallResult = RESULT_PASS; // Start out assuming we'll pass.
    HRESULT         hresult = E_FAIL;

    DXGI_FRAME_STATISTICS CurrentStats;
    DXGI_FRAME_STATISTICS PreviousStats;

    ID3D10RenderTargetView* pRTView = NULL; // Need a Rendertarget to perform Clear operations on our back buffers.

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
    
    // Maximum latency is limited be two factors.   
    //      * Value passed to SetMaximumFrameLatency
    //      * Number of buffers in the swapchain.
    // We're limited by the smaller of the two.
    if (IsWindows8())
    {
        WriteToLog("Windows8 or later OS detected.");
        uFrameLimit = uiMaxLatency;

    }
    else
    {
        WriteToLog("Windows7 OS (or early Win8 build) detected.");
        uFrameLimit = MINIMUM((unsigned int)m_iNumBuffersIndex, uiMaxLatency);
    }

    WriteToLog("MaxFrameLatency: %u", uiMaxLatency);
    WriteToLog("SwapChain buffers: %d", m_iNumBuffersIndex);
    WriteToLog("For Windows7 (With the interop pack), frame queueing should be limited by the lessor of SwapChain buffers or MaxFrameLatency.");
    WriteToLog("With Windows 8 and later, we are limited only by the MaxFrameLatency setting.");
    WriteToLog("In this case we expect the limit to be %u frames.", uFrameLimit);

    // In the flip model case, there is an extra frame of latency inherent in the way DWM processes events. Using
    // the uiMaxLatency = 1 case as an example, the first queued frame won't present to screen until the second
    // v-blank. After the second v-blank is when we will be unblocked from our third present. So, our current present
    // is always two greater than the present that just went to screen. So, when we are processing flip model presents,
    // we need to take another frame of latency into account.
    if (m_SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL)
    {
        uFlipLatency = 1;
    }

    // Did the test runner request that we loosen test tolerances?
    if (g_Application.m_bLoosenTolerances == true)
    {       
        uFrameLimit += 4;
        WriteToLog(" \"-LoosenTolerances\" was specified on the command line.  Increasing frame limit by 4 frames to %u frames.", uFrameLimit);
    }

    // Store away the allowable limit
    uCombinedLimit = uFrameLimit + uFlipLatency;

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
    //      retired and number of presents is greater than MaxFrameLatency and
    //      an inherent frame of latency in flip model.  If this happens, it
    //      suggests MaxFrameLatency is being ignored  and excessive frames are
    //      being queued.
    //
    //      We will fail if insufficient frames are queued.
    //
    //      We will fail if GetLastPresentCount does not increment consistently 
    //      by 1.  We expect it to increment by 1 (and only 1) for each present
    //      call we make.   We do not expect it to start from 0.
    //
    //      Since this test runs in a controlled environment, we shouldn't 
    //      get unexpected DISJOINT errors from GetFrameStatistics, that 
    //      and other errors will also trigger a failure.
    //
    const unsigned int MAX_NUMBER_OF_PRESENTS = 40;    // this will result in a worst test case run time of about 2.6 seconds for 60 Hz display mode with a presentational interval of 4
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
                if (uQueuedFrames > uCombinedLimit)
                {
                    //
                    // Bad!  We queued more frames than allowed by current frame latency settings, or present stats are completely broken.
                    //
                    WriteToLog(
                        "Frame %u: GetFrameStatistics indicates queue limit was exceeded by %u.",
                        uLoopCounter,
                        uQueuedFrames - uCombinedLimit
                        );
                    WriteToLog(
                        "Retired Frames: %u, Present Calls: %u, Queued Frames: %u, Queue Limit: %u, Flip Latency: %u",
                        uRetiredFrames,
                        uLoopCounter,
                        uQueuedFrames,
                        uFrameLimit,
                        uFlipLatency
                        );

                    OverallResult = RESULT_FAIL;

                    if (uFailCount++ > MAX_QUEUE_LIMIT_FAILURES)
                    {
                        WriteToLog("Too many queue limit failures.   Terminating test.");
                        goto Cleanup;
                    }
                }

                // Verify that we aren't queuing too few presents; however, wait until we know the queue is filled
                if (uHighestRecordedQueuedFrames >= uCombinedLimit && uQueuedFrames < uCombinedLimit)
                {
                    //
                    // We queued fewer frames than expected by current frame latency settings, test is unable to fill the queue,
                    // or present stats are completely broken.
                    //
                    WriteToLog(
                        "Frame %u: GetFrameStatistics indicates we were under the expected queue limit by %u.",
                        uLoopCounter,
                        uCombinedLimit - uQueuedFrames
                        );
                    WriteToLog(
                        "Retired Frames: %u, Present Calls: %u, Queued Frames: %u, Queue Limit: %u, Flip Latency: %u",
                        uRetiredFrames,
                        uLoopCounter,
                        uQueuedFrames,
                        uFrameLimit,
                        uFlipLatency
                        );

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
        }

        // Update the previous count with the new value in preparation for next iteration.
        uPreviousPresentCount = uCurrentPresentCount;
        // Re-bind render targets after flip model present
        if( m_SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL )
            m_pD3D10Device->OMSetRenderTargets(1, &pRTView, NULL);

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

    if (uHighestRecordedQueuedFrames < uCombinedLimit)
    {
        //
        // If the test is unable to keep the queue full, we are unable to validate frame latency behavior. In a user
        // application this may be okay or expected; however, if it happens during this test, it can hide latency issues.
        WriteToLog("Test was unable to reach the expected queue limit (sum of queue limit and flip latency).");
        WriteToLog(
            "Queue Limit: %u, Flip Latency: %u, Max Recorded: %u",
            uFrameLimit,
            uFlipLatency,
            uHighestRecordedQueuedFrames
            );

        OverallResult = RESULT_FAIL; 
    } // End of uHighestRecordedQueuedFrames evaluation.


Cleanup:

    SAFE_RELEASE(pRTView);

    return OverallResult;
};

//****************************************************************************
// SetMaximumFrameLatency
// Helper function for setting frame latency.
//****************************************************************************
HRESULT CWinTestCases::SetMaximumFrameLatency(
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


//*****************************************************************************
// VerifyGetMaximumFrameLatency
//
//*****************************************************************************
TEST_RESULT CWinTestCases::VerifyGetMaximumFrameLatency(
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

//*****************************************************************************
// ExecuteTestCase
//
// Main entry point for test cases
//
//*****************************************************************************
TEST_RESULT CWinTestCases::ExecuteTestCase()
{
    DebugOut.Write("Entering ExecuteTestCase.\n");

    //At this point, the framework has set the following member variables with important test factors.
    //  m_iNumBuffersIndex       - Number of swapchain buffers
    //  m_iMaxFrameLatencyIndex  - Value to set for maximum Frame latency (expected is stored in effectiveMaxFrameLatency)
    //  m_TestType               - DEFAULT_TEST versus SET_TEST.  (Basically tells us whether to call SetMaximumFrameLatency or use the default of 3)
    //  m_PixelFormat            - SwapChain buffer pixel format
    //  m_nSyncInternal          - Sync interval to use for Present calls.

    //
    // Initialize locals.
    //
    TEST_RESULT test_result = RESULT_FAIL;
    TEST_RESULT temp_test_result = RESULT_FAIL;
    HRESULT hresult = S_OK;
    ID3D10Device* pD3D10Device = NULL;

    // Keep track of what the max frame latency in order to take default into account
    int effectiveMaxFrameLatency = m_iMaxFrameLatencyIndex;

    clsTestableDevice extraDevice((CTest *) this);

    if (g_Application.m_bDFlip)
        ShowCursor(FALSE);
	
	//Win8: [TestBug]320791
	// check the featurelevel is 9.x AND the format is DXGI_FORMAT_B8G8R8A8_UNORM
	// Otherwise, skip the test execution 
	
	if (
			(
				m_TestFeatureLevel == TEST_FEATURE_10_1_LEVEL_9_1 ||
				m_TestFeatureLevel == TEST_FEATURE_10_1_LEVEL_9_2 ||
				m_TestFeatureLevel == TEST_FEATURE_10_1_LEVEL_9_3 || 
				m_TestFeatureLevel == TEST_FEATURE_11_0_LEVEL_9_1 ||
				m_TestFeatureLevel == TEST_FEATURE_11_0_LEVEL_9_2 ||
				m_TestFeatureLevel == TEST_FEATURE_11_0_LEVEL_9_3 
			) &&
			(m_PixelFormat != DXGI_FORMAT_B8G8R8A8_UNORM)
		)
	{
		WriteToLog
				(
                "Skip un-supported pixel format for FeatureLevel9."
                );
        test_result = RESULT_SKIP;
        goto Cleanup;
	}

	//
    // Validate members and state.
    //

    
    //
    // Create our device and swap chain.
    //

    temp_test_result = CreateDeviceAndSwapChain
        (
        m_PixelFormat,
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

    //
    // The m_TestType can be either DEFAULT_TEST or SET_TEST indicating whether
    // we should use the default frame latency or set a custom value.
    //
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
            effectiveMaxFrameLatency = DEFAULT_FRAME_LATENCY;
        }

        temp_test_result = VerifyGetMaximumFrameLatency(
            m_pD3D10Device,
            effectiveMaxFrameLatency); 

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
         test_result = TestLatency(effectiveMaxFrameLatency);
    }

Cleanup:
    ReleaseTestCaseObjects();

    // Clean up DirectComposition if necessary
    m_DCOMPIsolation.DestroyDCompDeviceAndVisual();
    
    if(g_Application.m_bDFlip)
        ShowCursor(TRUE);

    DebugOut.Write("Exiting ExecuteTestCase.\n");

    return test_result;
};


//*****************************************************************************
// InitCommonTestParameters
//
// Sets up common test factors shared by all windowed mode Latency tests.
//
//*****************************************************************************
void CWinTestCases::InitCommonTestParameters()
{
        CUserValueSet< DXGI_SWAP_EFFECT > *pSwapEffect = new CUserValueSet< DXGI_SWAP_EFFECT >();
        pSwapEffect->AddValue((DXGI_SWAP_EFFECT)DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL); 
//        pSwapEffect->AddValue(DXGI_SWAP_EFFECT_FLIP_DISCARD); 
        AddParameter< DXGI_SWAP_EFFECT >( _T("SwapEffect") , &m_SwapEffect, pSwapEffect);

        //
        // Test both Windowed and Windowless (DCOMP Visual) Presents for Flip Model.  (Note that "windowless" is somewhat misleading
        // since the visuals themselves are still contained with-in a Window.)
        //
        CUserValueSet< bool > *pWindowlessFlipModel = new CUserValueSet< bool >();
        pWindowlessFlipModel->AddValue(false); 
        pWindowlessFlipModel->AddValue(true); 
        AddParameter< bool >( _T("UseDCOMP") , &m_tp_bWindowless, pWindowlessFlipModel);

        //
        // We want to cycle on all windowed mode pixel formats relevant to 
        // DXGI Flip Model. Unfortunately the list is smaller for Featurelevel 9.
        // We will cycle on all these formats, but inside ExecuteTest we’ll skip those not supported by FL9
		//
        CUserValueSet< DXGI_FORMAT> *pPixelFormat = new CUserValueSet< DXGI_FORMAT >();
		
		pPixelFormat->AddValue(DXGI_FORMAT_R8G8B8A8_UNORM); 
        pPixelFormat->AddValue(DXGI_FORMAT_R16G16B16A16_FLOAT);
		pPixelFormat->AddValue(DXGI_FORMAT_B8G8R8A8_UNORM); 		

        AddParameter< DXGI_FORMAT >( _T("Format") , &m_PixelFormat, pPixelFormat);

        AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>("Format", DXGI_FORMAT_B8G8R8A8_UNORM), WeightFactorLarge);
        AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>("Format", DXGI_FORMAT_R8G8B8A8_UNORM), WeightFactorLarge);

        //AddParameter(_T("BufferCount"), &m_iNumBuffersIndex, RangeValueSet(2, MAX_BUFFERS, 1));

}
