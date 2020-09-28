//
// Project headers
//

#include "DXGIPresentRate.h"
#include "ParameterHelper.h"
#include "TestApp.h"

#pragma warning(disable: 4355)


////////////////////////////////////////////////////////////////////////////////
// CWinSequential Methods

CWinSequential::CWinSequential()
{
    // Give our DCOMP isolation object a means to log.
    m_DCOMPIsolation.SetLogger(GetFramework()->GetLogWrapper());
};


CWinSequential::~CWinSequential()
{
};


void CWinSequential::InitTestParameters()
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
        // if (FindRenderTargetFormats(0, RTFormatsVector))
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

        // With BltModel, the number of buffers has no affect on presentation rate.  To keep runtime down, well only
        // test the 2-buffer case.   We'll choose 2, because it's compatible with flip-model, and that allows us
        // to prioritize just the 2-buffer case higher than other buffer counts.
        // See the AddPriorityPatternFilter calls further down.
        RFactor BltModelBufferCountFactors = AddParameter(_T("BufferCount"), &iNumBuffersIndex, RangeValueSet(2, 2, 1));

        //
        // Partial Presentation (aka. dirty-rects) is only enabled for Blt Model now. 
        //
        CUserValueSet< bool > *pPartialPresentation = new CUserValueSet< bool >();
        pPartialPresentation->AddValue(true); 
        RFactor PartialPresentation = AddParameter< bool >( _T("PartialPresentation"), &m_tp_bPartialPresentation,
            pPartialPresentation);
        CUserValueSet< bool > *pNoPartialPresentation = new CUserValueSet< bool >();
        pNoPartialPresentation->AddValue(false); 
        RFactor NoPartialPresentation = AddParameterValueSet< bool >( _T("PartialPresentation"), pNoPartialPresentation);

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

        //
        // Flip Model Swap Effects require at least 2 buffers.  We want to test a variety of buffer counts for FlipSquential
        // primarily for sync interval zero.  The frame rate for sync interval zero is capped by both max frame latency and 
        // buffer count.  We need to explore both limits.
        RFactor FlipModelBufferCountFactors  = AddParameterValueSet(_T("BufferCount"), RangeValueSet(2, MAX_BUFFERS, 1));

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

        RFactor SyncIntervalFactors = AddParameter(_T("SyncInterval"), &iSyncInterval, RangeValueSet(0, 4, 1)); 
        RFactor SyncInterval0 = AddParameterValueSet(_T("SyncInterval"), RangeValueSet(0, 0, 1));

        //
        // We want to test a couple broadly supported pixel formats.  Historically we've tested all pixel formats
        // but as the test has grown we need to find ways to reduce runtime.   I'm not aware of any 
        // PresentationRate failures that were pixel format specific.  Reducing the set of tested formats
        // to R8G8B8A8 and B8G8R8A8 seems like an acceptable compromise.
        //
        CUserValueSet <DXGI_FORMAT> *pPixelFormatsValueSet = new CUserValueSet<DXGI_FORMAT>();
        pPixelFormatsValueSet->AddValue(DXGI_FORMAT_R8G8B8A8_UNORM); 
        pPixelFormatsValueSet->AddValue(DXGI_FORMAT_B8G8R8A8_UNORM); 
        RFactor PixelFormatFactors= AddParameter< DXGI_FORMAT>( _T("BBFormat"), &m_PixelFormat, pPixelFormatsValueSet);

        SetRootTestFactor( m_FeatureLevelFactor * DWMFactors * PixelFormatFactors *
            ((BltModelBufferCountFactors * BltModelSwapEffectFactors  * DCOMPBltModel *
            (NoPartialPresentation * SyncIntervalFactors + 
            PartialPresentation * SyncInterval0)) +
            (FlipModelBufferCountFactors * FlipModelSwapEffectFactors * DCOMPFlipModel *
            NoPartialPresentation * SyncIntervalFactors)) );

        AddPriorityPatternFilter(FilterEqual<int>(_T("SyncInterval"), 1), WeightFactorLarge);
        AddPriorityPatternFilter(FilterEqual<int>(_T("BufferCount"), 2), WeightFactorLarge);

        DeclarePriorityLevelByPriorityValue(1, 2.0f);
        DeclarePriorityLevelByPriorityValue(2, 1.4f);
    };

    DebugOut.Write("Exiting InitTestParameters.\n");
};


TEST_RESULT CWinSequential::ExecuteTestCase()
{
    bool bSetTimePeriod = false;

    DebugOut.Write("Entering ExecuteTestCase.\n");

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

    bool bCreateTexturesResult = CreateTextures();

    if ( bCreateTexturesResult != true )
    {
        WriteToLog
            (
            "CreateTextures failed."
            );

        goto Cleanup;
    };


    //tests currently only run on default ouput
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

    // Partial Presentation doesn't work unless a single non-dirty frame is submitted:
    if (m_tp_bPartialPresentation)
    {
        (void)m_pSwapChain->Present( 0, 0 );
    }

    UINT numberOfRetries = 1;

    for(UINT i = 0; i <= numberOfRetries; i++)
    {

        hr = TestWindowedSwapChain
            (
            (UINT) iSyncInterval,
            refreshRate,
            m_SwapEffect,
            iNumBuffersIndex,
            (m_tp_bPartialPresentation ? &m_PresentParams : NULL),
            &test_result
            );

        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                _T("TestWindowedSwapChain failed with error 0x%x."),
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

#if 0
    if (m_tp_bPartialPresentation && RESULT_PASS == test_result)
    {
        numberOfRetries = 1;

        for(UINT i = 0; i <= numberOfRetries; i++)
        {

            hr = TestWindowedBltSwapChainPartialPresentation
                (
                refreshRate,
                iNumBuffersIndex,
                &m_PresentParams,
                &test_result
                );

            if ( FAILED( hr ) )
            {
                WriteToLog
                    (
                    _T("TestWindowedBltSwapChainPartialPresentation failed with error 0x%x."),
                    hr
                    );

                test_result = RESULT_FAIL;
                goto Cleanup;
            };

            if ( ( i <	numberOfRetries ) && (test_result == RESULT_FAIL) )
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
    }
#endif

Cleanup:

    if (bSetTimePeriod)
    {
        timeEndPeriod(2);
    }

    ReleaseTestCaseObjects();

    if(g_Application.m_bDFlip)
        ShowCursor(TRUE);

    DebugOut.Write("Exiting ExecuteTestCase.\n");

    return test_result;
};


void CWinSequential::Cleanup()
{
    DebugOut.Write("Entering Cleanup.\n");

    CleanupTestGroup();
    //CleanupWindowClass();

    DebugOut.Write("Exiting Cleanup.\n");
};
