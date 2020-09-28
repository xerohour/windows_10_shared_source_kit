//****************************************************************************
// File: DoNotWait.cpp
// Description:
//
//  Contains test cases for DXGI Present flag DXGI_PRESENT_DO_NOT_WAIT.
//  See ExecuteTest function comments for methodolgy.
//
//  Class Hierarchy:   
//  CDoNotWait
//      CDefaultLatencyWindowed (DefaultLatencyWindowed.cpp)
//          CWinTestCases  (SetupWindowed.cpp)
//              CDXGILatencySingleSC
//                  CDXGILatency
//                      CDXGITest
//                          CTest
//                          CVariationGenerator
//	                    clsBasicWindowClass
//	                    CTestDXGISurfaceUtils
//	                    clsD3D10Ref
//
// History:
//      7/22/2010 - Initial Creation - OlanH
//****************************************************************************
//
// Project headers
//

#include "TestApp.h"
#include <ParameterHelper.h>

#define MAX_WINDOWED_LATENCY 8  // Limit on the range of MaxFrameLatencies we test.

//****************************************************************************
//****************************************************************************
CDoNotWaitTest::CDoNotWaitTest()
{
};


//****************************************************************************
// Destructor
//****************************************************************************
CDoNotWaitTest::~CDoNotWaitTest()
{
};

//****************************************************************************
// Constructor
//****************************************************************************
TEST_RESULT CDoNotWaitTest::Setup()
{
	if ( g_Application.m_pCreateDXGIFactory1 == NULL )
	{
		WriteToLog
			(
			_T( "Testing Get\\SetMaximumFrameLatency requires DXGIFactory1" )
			);

		return RESULT_SKIP;    
	}

	return RESULT_PASS;
};


//****************************************************************************
// InitTestParameters
//****************************************************************************
void CDoNotWaitTest::InitTestParameters()
{
	DebugOut.Write("Entering InitTestParameters.\n");


	//
	// Call the base class implementation.
	//

	CDXGITest::InitTestParameters();


	if (InitializeTestGroup())
	{

		InitCommonTestParameters();        

		//
		// Experiment with a few different back buffer counts.  We want to trigger cases where 
		// both frame latency and back buffer count limit how many frames can be queued.
		//
		CUserValueSet< int > *pBufferCounts = new CUserValueSet< int>();
		pBufferCounts->AddValue(2);
		pBufferCounts->AddValue(5);
		pBufferCounts->AddValue(8);
		AddParameter<int>(_T("BufferCount"), &m_iNumBuffersIndex, pBufferCounts);

		//
		// Experiment with several different frame latencies.
		//
		CUserValueSet< int > *pFrameLatencies = new CUserValueSet< int>();
		pFrameLatencies->AddValue(1);
		pFrameLatencies->AddValue(3);
		pFrameLatencies->AddValue(16);
		AddParameter<int>(_T("FrameLatency"), &m_iMaxFrameLatencyIndex, pFrameLatencies);

		//
		// Adding this causes SetMaximumFrameLatency to be called.
		//
		CUserValueSet< TestType > *pTestType = new CUserValueSet< TestType >();
		pTestType->AddValue(SET_TESTS);
		AddParameter< TestType >( _T("TestType") , &m_TestType, pTestType);

		//
		// Testing a couple different sync internals.
		//
		CUserValueSet< int > *pnSyncIntervals= new CUserValueSet< int>();
		pnSyncIntervals->AddValue(0);
		pnSyncIntervals->AddValue(1);
		AddParameter<int>(_T("SyncInterval"), &m_nSyncInterval, pnSyncIntervals);

		AddPriorityPatternFilter(FilterEqual<int>("FrameLatency", 3), WeightFactorLarge);
		AddPriorityPatternFilter(FilterEqual<int>("BufferCount", 5), WeightFactorLarge);
		AddPriorityPatternFilter(FilterEqual<int>("SyncInterval", 1), WeightFactorLarge);

		DeclarePriorityLevelByPriorityValue(1, 2.0);
		DeclarePriorityLevelByPriorityValue(2, 1.5);
	}

	DebugOut.Write("Exiting InitTestParameters.\n");

};

//*****************************************************************************
// ExecuteTestCase
//
// Main entry point for test cases.
// 
// Test verifies that DXGI Present returns DXGI_ERROR_WAS_STILL_DRAWING when
// the queue is full.  (Queue is limited by Max Frame Latency setting or number 
// of back buffers in swapchain.)
// 
// Test works by calling Present in a loop until WAS_STILL_DRAWING is returned.
// We then compare the frame number against the Queue limit.   If the numbers
// are close, the test passes.
// 
// Because this test has timing dependencies and runs in a multitasking OS, 
// we'll repeat the test several times (several trials) and only fail if we a 
// certain percentage of the trials fail.
//
// Note the special handling for the 2-buffer swapchain case.   The way the 
// DXGI-DWM buffer exchange works, prevents DXGI (and D3D9) from returning
// WAS_STILL_DRAWING when using a 2-buffer swapchain, we must stall.
//*****************************************************************************
TEST_RESULT CDoNotWaitTest::ExecuteTestCase()
{
	//At this point, the framework has set the following member variables with important test factors.
	//  m_iNumBuffersIndex      - Number of swapchain buffers (Actual value.  Not an index!  Variable is misnamed)
	//  m_iMaxFrameLatencyIndex - Value to set (or expect) for maximum Frame latency
	//  m_TestType              - DEFAULT_TEST versus SET_TEST.  (Basically tells us whether to call SetMaximumFrameLatency or use the default of 3)
	//  m_PixelFormat           - SwapChain buffer pixel format
	//  m_nSyncInternal         - Sync interval to use for Present calls.

	DebugOut.Write("Entering ExecuteTestCase.\n");

	const unsigned int NUMBER_OF_TRIALS = 4;      // Repeat each test case to allow for random timing related failures.
	const unsigned int ACCEPTABLE_PERCENTAGE_OF_FAILING_TRIALS = 25; // Allow for 25% of the trials to fail, yet still pass the test.

	unsigned int    uNumberOfPresentsPerTrial = MAXIMUM(m_iNumBuffersIndex, m_iMaxFrameLatencyIndex) * 3;  //Perform enough presents per trial to ensure we find any limits.
	unsigned int    uFrameLimit;
	unsigned int    uSteadyStateFrameLimit; // This is the steady state frame limit (This many frames can normally be presented before WAS_STILL_DRAWING is returned)
	unsigned int    uLoopCounter;
	unsigned int    uFailureCount;

	bool            bFirstPassingCallToGetFrameStatsAfterDisjoint = false;
	bool            bGetFramesStatsSucceededAtLeastOnce = false;

	TEST_RESULT     TestResultFinal = RESULT_PASS; // Start out assuming we'll pass.

	DXGI_FRAME_STATISTICS CurrentStats;
	DXGI_FRAME_STATISTICS PreviousStats;

	ID3D10RenderTargetView *pRTView=NULL; // Need a Rendertarget to perform Clear operations on our back buffers.

	TEST_RESULT temp_test_result = RESULT_FAIL;

	HRESULT hresult = S_OK;
	ID3D10Device* pD3D10Device = NULL;


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

		TestResultFinal = temp_test_result;
		goto Cleanup;
	};


	//
	// Set the Maximum frame latency base on test parameter value.
	//
	assert( g_Application.m_pCreateDXGIFactory1 != NULL );

	HRESULT hr = SetMaximumFrameLatency(
		m_pD3D10Device,
		m_iMaxFrameLatencyIndex);

	if(FAILED(hr))
	{
		if (hr == E_NOINTERFACE || hr == DXGI_ERROR_UNSUPPORTED )
		{
			TestResultFinal = RESULT_SKIP;
		}
		else
		{
			WriteToLog(_T("SetMaximumFrameLatency failed"));  
			TestResultFinal = RESULT_FAIL;
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
		TestResultFinal = temp_test_result;
		goto Cleanup;
	}

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

	// For windowed mode, Maximum latency is limited be two factors.   
	//      * Value passed to SetMaximumFrameLatency
	//      * Number of buffers in the swapchain (minus 1).
	// We're limited by the smaller of the two.
	// Note this may change in Win8 M2.  GPU syncronization may enable us to work around limits based on number of back buffers.

	WriteToLog("MaxFrameLatency: %u", m_iMaxFrameLatencyIndex);
	WriteToLog("SwapChain buffers: %d", m_iNumBuffersIndex);

	// Determine when we should expect to get WAS_STILL_DRAWING.  For the first trial,
	// there are no buffers in DWMs queue, so we are allowed to present one extra frame.
	//
	// Queue limits vary based on OS version.   For Windows 7 (And early Win8 builds) 
	// the number of buffers in our swapchain is one possible limit.   For Windows 8
	// and later, only MaxFrameLatency limits us.
	//
	// NOTE: For Flip model, the minimum number of back buffers is 2, and 
	// that is treated special on Windows 7 (With interop pack).  For 2-buffer flip model 
	// swap chains you'll never get WAS_STILL_DRAWING.
	//
	if (IsWindows8())
	{
		WriteToLog("Windows 8 OS or later detected.");
		uSteadyStateFrameLimit = m_iMaxFrameLatencyIndex;
		WriteToLog("    On Windows 8, frame queueing is limited only by MaxFrameLatency, we expect the limit to be %u frame(s).", uSteadyStateFrameLimit);
		WriteToLog("    Present should return DXGI_ERROR_WAS_STILL_DRAWING on frame %u.", uSteadyStateFrameLimit+1);
	}
	else // Windows 7 or early Win8 build.
	{
		WriteToLog("Windows 7 OS (or early Windows 8 build) detected.");

		if (m_iNumBuffersIndex == 2)
		{
			uSteadyStateFrameLimit=0;
			WriteToLog("    For a 2-buffer swapchain on Windows 7, DXGI should never return WAS_STILL_DRAWING.");
		}
		else
		{
			unsigned int uBufferLimit = m_iNumBuffersIndex-2;
			uSteadyStateFrameLimit = MINIMUM(uBufferLimit, (unsigned int) m_iMaxFrameLatencyIndex);
			WriteToLog("    On Windows 7 frame queueing is limited by SwapChain buffers and MaxFrameLatency, we expect the limit to be %u frame(s).", uSteadyStateFrameLimit);
			WriteToLog("    Present should return DXGI_ERROR_WAS_STILL_DRAWING on frame %u.", uSteadyStateFrameLimit+1);
		}
	}

	// Did the test runner request that we loosen test tolerances?
	if (g_Application.m_bLoosenTolerances == true)
	{       
		WriteToLog(" \"-LoosenTolerances\" was specified on the command line.  We'll allow WASSTILLDRAWING to occur a few frames late.");
	}

	uFailureCount=0; // Track failures across all trials.

	WriteToLog("Performing rapid Present(DO_NOT_WAIT) calls to trigger DXGI_ERROR_WAS_STILL_DRAWING.  We'll repeat up to %d times to ensure consistency.", NUMBER_OF_TRIALS);

	//
	// Repeat the test several times to weed out noise.
	//
	for (unsigned int uTrial = 1; uTrial <= NUMBER_OF_TRIALS; uTrial++)
	{
		WriteToLog("Trial #%u:", uTrial);

		//
		// Check to see if this is the first trial for a Win7 run where we are
		// limited by back buffer count, not Max Frame Latency setting.
		//
		if (uSteadyStateFrameLimit && (uSteadyStateFrameLimit < (UINT)m_iMaxFrameLatencyIndex) && (uTrial == 1))
		{
			WriteToLog("    For this first trial where we are limited by back buffer count, we expect to be able to present one additional frame before getting WAS_STILL_DRAWING.");
			uFrameLimit = uSteadyStateFrameLimit + 1;
		}
		else 
			uFrameLimit = uSteadyStateFrameLimit;
		//
		// Call Present quickly in a loop until we get WAS_STILL_DRAWING.
		// Ideally we should get WAS_STILL_DRAWING right at the queue limit.
		//
		uLoopCounter=1;   // Start our count from 1, not zero.
        do
        {
            // Clear render target view
            FLOAT fRGBA[4] = { (float)uLoopCounter/(float)(2*uNumberOfPresentsPerTrial), 0.0f, 0.0f, 1.0f };
            m_pD3D10Device->ClearRenderTargetView(pRTView, fRGBA);

            // Present a frame.
            hresult = m_pSwapChain->Present( m_nSyncInterval, DXGI_PRESENT_DO_NOT_WAIT );
            if( m_SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL )
                m_pD3D10Device->OMSetRenderTargets(1, &pRTView, NULL);

        } // End of Present loop
		while((hresult == S_OK) && (++uLoopCounter < uNumberOfPresentsPerTrial));  // Continue presenting until we get WAS_STILL_DRAWING or we exceed our present count.

		//
		// Evaluate loop exit conditions to determine pass/fail.
		//
		if (hresult == DXGI_ERROR_WAS_STILL_DRAWING)
		{
			//
			// Got the WAS_STILL_DRAWING ERROR we hoped for, but was it returned when we expected it?
			// 
			// Did we get WAS_STILL_DRAWING the very first frame?   Not good.  We should be able to present frames up to the queue limit.
			if (uLoopCounter == 1)
			{
				WriteToLog("   *** Failure: Present returned WAS_STILL_DRAWING on the first frame! We can't present any frames! Failing this test case! ***");
				TestResultFinal = RESULT_FAIL;
				goto Cleanup;
			}

			// If we get WAS_STILL_DRAWING early, log a failure.  
			// Note for the first trial, we expect to be able to Present 
			// uFrameLimit+1 frames, but after that there will already be
			// one frame in DWM's queue, so we'll expect to get 
			// WAS_STILL_DRAWING on the uFrameLimit frame.
			else if (uLoopCounter < uFrameLimit+1) 
			{
				WriteToLog("   *** Failure: Present returned WAS_STILL_DRAWING on frame %u.  We didn't expect to get it until frame %u.  Failing this test case!. ***", uLoopCounter, uFrameLimit+1);
				TestResultFinal = RESULT_FAIL;
				goto Cleanup;
			}

			// If we get it too late, log a failure.
			// Allow for a few extra frames if the user specified the "-LoosenTolerances" command line parameter
			else if ( uLoopCounter > (g_Application.m_bLoosenTolerances ?  uFrameLimit+4: uFrameLimit+1) )
			{
				WriteToLog("   *** Failure: Present returned WAS_STILL_DRAWING on frame %u.  We should have gotten it on frame %u.  It's possible something delayed our process.   Failing trial #%u, but not necessarily the test case.***", uLoopCounter, uFrameLimit+1, uTrial);
				uFailureCount++;
			}
			else // Passed!
			{
				WriteToLog("   Present returned WAS_STILL_DRAWING on frame %u.  Passed!", uLoopCounter);
			}
		}
		else if (FAILED( hresult ) )
		{
			WriteToLog("   *** Failure: Frame %u: Present failed with an unexpected error of %s (0x%x). ***",uLoopCounter, HResultToString(hresult), hresult);
			// Treat this as a hard failure.  Don't regardless of how additional trials behave.
			TestResultFinal = RESULT_FAIL;
			goto Cleanup;
		}
		else // Present never failed with WAS_STILL_DRAWING.
		{
			// We presented enough frames to more than fill the queue.  We 
			// either aren't filling the queue, or WAS_STILL_DRAWING is not
			// implemented.
			//
			// Special check for the 2 buffer swapchain case.   
			// For 2 buffers, DXGI cannot return WAS_STILL_DRAWING due to 
			// implementation details.
			//
			if (m_iNumBuffersIndex == 2)
			{
				WriteToLog("   Called Present %u times and never got WAS_STILL_DRAWING.  This is expected for two-buffer swapchains. ", uLoopCounter);
			}
			else // FAIL!!!! We never got WAS_STILL_DRAWING and we were expecting it!
			{
				WriteToLog("   *** Failure: Called Present %u times and never got WAS_STILL_DRAWING.  Should have hit on frame %u.  Failing trial.***",uLoopCounter, uFrameLimit+1);
				uFailureCount++;
			}
		}

		Sleep (1000); // Sleep to allow all outstanding presents to complete before next trial.

	} // End of repeated Trials loop

	// Fail the overall test if too many of the trials failed.
	if (uFailureCount > (NUMBER_OF_TRIALS * ACCEPTABLE_PERCENTAGE_OF_FAILING_TRIALS / 100))
	{
		WriteToLog("Failure: Over %d percent of the test trials failed.  Failing the test case!", ACCEPTABLE_PERCENTAGE_OF_FAILING_TRIALS);
		TestResultFinal = RESULT_FAIL;
	}

Cleanup:

	SAFE_RELEASE(pRTView);

	ReleaseTestCaseObjects();

	DebugOut.Write("Exiting ExecuteTestCase.\n");

	return TestResultFinal;
};



