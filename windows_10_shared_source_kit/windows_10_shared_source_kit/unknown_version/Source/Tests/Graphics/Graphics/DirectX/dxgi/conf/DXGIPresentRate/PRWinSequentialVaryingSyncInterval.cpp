//
// Project headers
//

#include "DXGIPresentRate.h"
#include "ParameterHelper.h"
#include "TestApp.h"

////////////////////////////////////////////////////////////////////////////////
// CWinSequentialVaryingSyncInterval Methods

CWinSequentialVaryingSyncInterval::CWinSequentialVaryingSyncInterval()
{
	m_useQpcTimes = TRUE;

	//
	//  Make sure to use average verification. Matching varying frametimes in the actual vs expected array is not implemented
	//
	m_useAverages = TRUE;
};


CWinSequentialVaryingSyncInterval::~CWinSequentialVaryingSyncInterval()
{
};


void CWinSequentialVaryingSyncInterval::InitTestParameters()
{
	DebugOut.Write("Entering InitTestParameters.\n");


	//
	// Call the base class implementation.
	//

	CDXGITest::InitTestParameters();


	//
	// Initialize parameters.
	//

	tstring strPath = GetPath().ToString();


	if ( InitializeTestGroup() )
	{

		//
		// We need to maintain seperate test factor lists for Flip Model versus Blt Model swap effects.
		//

		//
		// Blt Model test factors
		//
		CUserValueSet< DXGI_SWAP_EFFECT > *pSwapEffect = new CUserValueSet< DXGI_SWAP_EFFECT >();
		pSwapEffect->AddValue(DXGI_SWAP_EFFECT_SEQUENTIAL); 
		RFactor BltModelSwapEffectFactors = AddParameter< DXGI_SWAP_EFFECT >( _T("SwapEffect") , &m_SwapEffect, pSwapEffect);

		// Blt model has no restrictions on buffer count
		CUserValueSet< int > *pBufferCount = new CUserValueSet< int >();
		pBufferCount->AddValue( 1 );
		pBufferCount->AddValue( 3 );
		pBufferCount->AddValue( DXGI_MAX_SWAP_CHAIN_BUFFERS);
		RFactor BltModelBufferCountFactors = AddParameter< int >(_T("BufferCount"), &iNumBuffersIndex, pBufferCount);

		// Blt model supports a fairly large number of back buffer formats.
		RFactor BltModelFormatFactors = AddParameter(_T("BBFormat"), &m_PixelFormat, 
			TableValueSet<DXGI_FORMAT>(&clsDXGIRenderTargetFormats::DXGI_Back_Buffer_Formats[0], 
			sizeof(clsDXGIRenderTargetFormats::DXGI_Back_Buffer_Formats[0]), 
			clsDXGIRenderTargetFormats::uiNoOfBBFormats) );

		//
		// Windowless (DCOMP Visual) Presents are NOT supported for Blt Model.  Test regular Windowed presents only.
		//
		CUserValueSet< bool > *pWindowlessBltModel = new CUserValueSet< bool >();
		pWindowlessBltModel->AddValue(false); 
		RFactor DCOMPBltModel = AddParameter< bool >( _T("UseDCOMP") , &m_tp_bWindowless, pWindowlessBltModel);


		//
		// Flip Model test factors
		//
		CUserValueSet< DXGI_SWAP_EFFECT > *pFlipSwapEffect = new CUserValueSet< DXGI_SWAP_EFFECT >();
		pFlipSwapEffect->AddValue(DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL); 
		RFactor FlipModelSwapEffectFactors = AddParameterValueSet< DXGI_SWAP_EFFECT >( _T("SwapEffect"), pFlipSwapEffect);

		// Flip Model Swap Effects require at least 2 buffers, but we're going
		// to use much more to simplify our validation.   With Flip Model, sync 
		// interval 0 Presents can stall until there is room in the queue.  
		// Also GPU scheduling can be delayed due to insufficient buffers, this 
		// could prevent DWM from processing a frame in it's queue.

		CUserValueSet< int > *pFlipModelBufferCount = new CUserValueSet< int >();
		pFlipModelBufferCount->AddValue( DXGI_MAX_SWAP_CHAIN_BUFFERS);
		RFactor FlipModelBufferCountFactors  = AddParameterValueSet< int >(_T("BufferCount"), pFlipModelBufferCount);

		// Flip model only supports a subset of the normal back buffer formats.
		RFactor FlipModelFormatFactors = AddParameterValueSet(_T("BBFormat"), 
			TableValueSet<DXGI_FORMAT>(&clsDXGIFlipModelRenderTargetFormats::DXGI_Back_Buffer_Formats[0], 
			sizeof(clsDXGIFlipModelRenderTargetFormats::DXGI_Back_Buffer_Formats[0]), 
			clsDXGIFlipModelRenderTargetFormats::uiNoOfBBFormats) );

		//
		// Test both Windowed and Windowless (DCOMP Visual) Presents for Flip Model.  (Note that "windowless" is somewhat misleading
		// since the visuals themselves are still contained with-in a Window.)
		//
		CUserValueSet< bool > *pWindowlessFlipModel = new CUserValueSet< bool >();
		pWindowlessFlipModel->AddValue(false); 
		pWindowlessFlipModel->AddValue(true); 
		RFactor DCOMPFlipModel = AddParameterValueSet< bool >( _T("UseDCOMP"), pWindowlessFlipModel);


		//
		// Common test factors for both Blt and Flip Model
		//

		CUserValueSet< bool > *pDwm = new CUserValueSet< bool >();
		pDwm->AddValue( true );
		RFactor DWMFactors = AddParameter< bool >(_T("DWM"), &m_bDWM, pDwm);        

		//
		// Common usage is to use 0 syncinterval to dump pre rendered frames so that is what we will test. More specifically,
		// the non-zero syncinterval frame and all zero syncinterval frames will be dumped and replaced with the last zero
		// syncinterval frame in the sequence. For example, if we had a queue that was already filled like the following
		//
		// Frame:          A  B  C  D
		// Sync Interval:  3  3  0  0
		//
		// processing the sync interval 0 frames will result in something like
		//
		// Frame:          A  D
		// Sync Interval:  3  0
		//
		RFactor SyncInterval1 = AddParameter(_T("SyncInterval1"), &iSyncInterval1, RangeValueSet(0, 0, 1)); 
		RFactor SyncInterval2 = AddParameter(_T("SyncInterval2"), &iSyncInterval2, RangeValueSet(3, 3, 1)); // Limiting this to sync interval 3.

		//
		// For the flip sequential case, if IntervalDivisor is 4, we will do 2 presents at SyncInterval1 and 2 at
		// SyncInterval2. This means that in the flip sequential case the first SyncInterval2 frame will be rendered
		// and the second SyncInterval2 frame will be thrown out due to the two SyncInterval1 (syncinterval = 0)
		// frames that follow. The last syncinterval 0 frame will be presented in its place. This implies that
		// we require InternalDivisor >= 3. This also requires the pattern be
		//
		// SyncInterval2, SyncInterval2, SyncInterval1, ..., SyncInterval1, where SyncInterval1 is always 0.
		// 
		// There is no need for a second non-zero present interval when testing bltmodel, so the bltmodel sequence
		// will look like
		//
		// SyncInterval2, SyncInterval1, ..., SyncInterval1, where SyncInterval1 is always 0.
		//
		// Should the desire arise to support more complex interval patterns, the test logic will need to be modified.
		//
		RFactor IntervalDivisor = AddParameter(_T("IntervalDivisor"), &iIntervalDivisor, RangeValueSet(4, 4, 1));

		// Cycle use of DXGI_PRESENT_DO_NOT_SEQUENCE along with the 
		// SyncInterval2 frames.   We want to make sure the sync interval of 
		// DO_NOT_SEQUENCE frames is treated the same as normal frames.  In 
		// real-world scenarios, DO_NOT_SEQUENCE frames will always be 
		// inter-mixed with normal frames, so including them in this test
		// is appropriate.  We'll present normal frames for interval1 and 
		// alternate the inclusion of DO_NOT_FLIP for interval2.
		CUserValueSet< bool > *pDoNotSequence = new CUserValueSet< bool >();
		pDoNotSequence ->AddValue( true );
		pDoNotSequence ->AddValue( false );
		RFactor DoNotSequenceFactors = AddParameter< bool >(_T("DONOTSEQUENCE"), &m_bDoNotSequence, pDoNotSequence);        

		AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("BBFormat"), DXGI_FORMAT_R8G8B8A8_UNORM), WeightFactorLarge);        // DXGI_FORMAT_R8G8B8A8_UNORM
		AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("BBFormat"), DXGI_FORMAT_B8G8R8A8_UNORM), WeightFactorLarge);        // DXGI_FORMAT_B8G8R8A8_UNORM
		AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("BBFormat"), DXGI_FORMAT_R16G16B16A16_FLOAT), WeightFactorSmall);    // DXGI_FORMAT_R16G16B16A16_FLOAT

		DeclarePriorityLevelByPriorityValue(1, 1.5f);
		DeclarePriorityLevelByPriorityValue(2, 1.0f);

		SetRootTestFactor( m_FeatureLevelFactor * DWMFactors * SyncInterval1 * SyncInterval2 * IntervalDivisor *
			DoNotSequenceFactors *
			((BltModelBufferCountFactors  * BltModelSwapEffectFactors  * BltModelFormatFactors * DCOMPBltModel) + 
			(FlipModelBufferCountFactors * FlipModelSwapEffectFactors * FlipModelFormatFactors * DCOMPFlipModel)) );

	};

	DebugOut.Write("Exiting InitTestParameters.\n");
};


TEST_RESULT CWinSequentialVaryingSyncInterval::ExecuteTestCase()
{
	bool bSetTimePeriod = false;

	TEST_RESULT SetupDeviceResult;

	//
	// Initialize locals.
	//

	TEST_RESULT test_result = RESULT_FAIL;

	//
	// Validate test parameter compatibility with Flip Model
	//
	if (IsPixelFormatCompatibleWithSwapEffect(m_SwapEffect,  m_PixelFormat, m_TestFeatureLevel) == false)
	{
		test_result = RESULT_SKIP;
		goto Cleanup;
	}

    if(g_Application.m_bDFlip)
        ShowCursor(FALSE);

	//
	// Create our device and swap chain.
	//

	SetupDeviceResult = CreateDeviceAndSwapChain
		(
		m_bDWM,
        m_tp_bWindowless,
		m_PixelFormat,
		m_SwapEffect,
		iNumBuffersIndex
		);

	if ( SetupDeviceResult != RESULT_PASS )
	{
		if ( SetupDeviceResult != RESULT_SKIP )
		{
			WriteToLog
				(
				"CreateDeviceAndSwapChain failed."
				);
		};

		test_result = SetupDeviceResult;
		goto Cleanup;
	};


	// To do for multiple-output / multimon scenarios:
	//
	// Infer the position of the output window from test case arguments and locate
	// the window appropriately.
	//
	// Note that the properties of the textures created by the CreateTextures method are
	// dependent on the properties and location of the output window, so these must be
	// established before the call to CreateTextures.

	//
	// Textures are loaded but test doesn't present anything or use them.  
	// This seems to be a test bug. Should either remove loading or make the test present something.
	//
	bool bCreateTexturesResult = CreateTextures();

	if ( bCreateTexturesResult != true )
	{
		WriteToLog
			(
			"CreateTextures failed."
			);

		goto Cleanup;
	};

	//tests currently only run on default output
	UINT refreshRate = 0;
	HRESULT hr = GetRefreshRateForDefault( &refreshRate );
	if ( FAILED( hr ) )
	{
		WriteToLog
			(
			_T("GetRefreshRate failed with error 0x%x."),
			hr
			);

		goto Cleanup;
	};

	// Use a high resolution timer to ensure the test's correctness for OS emulated VSYNCs (VMs, BasicDisplay.sys)
	bSetTimePeriod = true;
	timeBeginPeriod(2);

	UINT numberOfRetries = 1;

	//
	// Increase the Maximum Frame Latency to reduce stalling on FlipModel interval 0 frames.
	// This should have no affect on Blt Model.
	//
	IDXGIDevice1 *pDXGIDevice;
	hr = m_DevicePointer.pDevice->QueryInterface(IID_IDXGIDevice1, (void **)&pDXGIDevice);
	if (FAILED(hr))
	{
		WriteToLog(_T("GetRefreshRate failed with error 0x%x."),D3DHResultToString(hr), hr);
		goto Cleanup;
	}
	pDXGIDevice->SetMaximumFrameLatency(16);
	pDXGIDevice->Release();

	for(UINT i = 0; i <= numberOfRetries; i++)
	{

		hr = TestVaryingSyncInterval 
			(
			(UINT) iSyncInterval1,
			(UINT) iSyncInterval2,
			(UINT) iIntervalDivisor,
			refreshRate,
			m_SwapEffect,
			iNumBuffersIndex,
			m_bDoNotSequence,
			&test_result
			);

		if ( FAILED( hr ) )
		{
			WriteToLog
				(
				_T("TestVaryingSyncInterval failed with error 0x%x."),
				hr
				);

			test_result = RESULT_FAIL;
			goto Cleanup;
		};

		if ( ( i <  numberOfRetries ) && (test_result == RESULT_FAIL) )
		{

			UINT sleepTime = 5000; 
			WriteToLog
				(
				_T("Test Case Failed.  Sleep for %dms and re-running test "),
				sleepTime
				);    

			Sleep(sleepTime);
		}

		else
		{
			break;
		}

	}

Cleanup:

	if (bSetTimePeriod)
	{
		timeEndPeriod(2);
	}

	ReleaseTestCaseObjects();

    if(g_Application.m_bDFlip)
        ShowCursor(TRUE);

	return test_result;
};


void CWinSequentialVaryingSyncInterval::Cleanup()
{

	CleanupTestGroup();

};
