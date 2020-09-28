//
// Project headers
//

#include "DXGIPresentRate.h"
#include "ParameterHelper.h"

#pragma warning(disable: 4355)


////////////////////////////////////////////////////////////////////////////////
// CWinDiscard Methods

CWinDiscard::CWinDiscard()
{
};


CWinDiscard::~CWinDiscard()
{
};


void CWinDiscard::InitTestParameters()
{
    DebugOut.Write("Entering InitTestParameters.\n");


    //
    // Call the base class implementation.
    //

    CDXGITest::InitTestParameters();


    //
    // Initialize locals.
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
        pSwapEffect->AddValue(DXGI_SWAP_EFFECT_DISCARD); 
        RFactor BltModelSwapEffectFactors = AddParameter< DXGI_SWAP_EFFECT >( _T("SwapEffect") , &m_SwapEffect, pSwapEffect);

        // Blt model has no restrictions on buffer count
        RFactor BltModelBufferCountFactors = AddParameter(_T("BufferCount"), &iNumBuffersIndex, RangeValueSet(1, MAX_BUFFERS, 1));

        // Blt model supports a fairly large number of back buffer formats.
        RFactor BltModelFormatFactors = AddParameter(_T("BBFormat"), &m_PixelFormat, 
                                            TableValueSet<DXGI_FORMAT>(&clsDXGIRenderTargetFormats::DXGI_Back_Buffer_Formats[0], 
                                                sizeof(clsDXGIRenderTargetFormats::DXGI_Back_Buffer_Formats[0]), 
                                                clsDXGIRenderTargetFormats::uiNoOfBBFormats) );

        //
        // Partial Presentation (aka. dirty-rects) is not supported for SWAP_EFFECT_DISCARD for Win8.          
        //
        CUserValueSet< bool > *pNoPartialPresentation = new CUserValueSet< bool >();
        pNoPartialPresentation->AddValue(false); 
        RFactor NoPartialPresentation = AddParameter< bool >( _T("PartialPresentation"), &m_tp_bPartialPresentation,
            pNoPartialPresentation);

        //// Commented this out for now.  Support may be re-introduced in a future OS release. 
        //CUserValueSet< bool > *pPartialPresentation = new CUserValueSet< bool >();
        //pPartialPresentation->AddValue(true); 
        //RFactor PartialPresentation = AddParameterValueSet< bool >( _T("PartialPresentation"),
        //    pPartialPresentation);
 
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
        //pFlipSwapEffect->AddValue(DXGI_SWAP_EFFECT_FLIP_DISCARD); 
        RFactor FlipModelSwapEffectFactors = AddParameterValueSet< DXGI_SWAP_EFFECT >( _T("SwapEffect"), pFlipSwapEffect);

        // Flip Model Swap Effects require at least 2 buffers.
        RFactor FlipModelBufferCountFactors  = AddParameterValueSet(_T("BufferCount"), RangeValueSet(2, MAX_BUFFERS, 1));

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

        RFactor SyncIntervalFactors = AddParameter(_T("SyncInterval"), &iSyncInterval, RangeValueSet(0, 4, 1)); // Limiting this to sync interval of 1 for WDK/logo testing.
        //RFactor SyncInterval0 = AddParameterValueSet(_T("SyncInterval"), RangeValueSet(0, 1, 1));


        AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("BBFormat"), DXGI_FORMAT_R8G8B8A8_UNORM), WeightFactorLarge);        // DXGI_FORMAT_R8G8B8A8_UNORM
        AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("BBFormat"), DXGI_FORMAT_B8G8R8A8_UNORM), WeightFactorLarge);        // DXGI_FORMAT_B8G8R8A8_UNORM
        AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("BBFormat"), DXGI_FORMAT_R16G16B16A16_FLOAT), WeightFactorSmall);    // DXGI_FORMAT_R16G16B16A16_FLOAT

        AddPriorityPatternFilter(FilterLess<int>(_T("SyncInterval"), 2), WeightFactorLarge);

        DeclarePriorityLevelByPriorityValue(1, 2.1f);
        DeclarePriorityLevelByPriorityValue(2, 0.9f);

        SetRootTestFactor( m_FeatureLevelFactor * DWMFactors * 
            ((BltModelBufferCountFactors * BltModelSwapEffectFactors * BltModelFormatFactors * DCOMPBltModel *
                (NoPartialPresentation * SyncIntervalFactors/* +
                PartialPresentation * SyncInterval0*/)) + 
            (FlipModelBufferCountFactors * FlipModelSwapEffectFactors * FlipModelFormatFactors * DCOMPFlipModel *
                NoPartialPresentation * SyncIntervalFactors)) );

    }

    DebugOut.Write("Exiting InitTestParameters.\n");
}


TEST_RESULT CWinDiscard::ExecuteTestCase()
{
    DebugOut.Write("Entering ExecuteTestCase.\n");


    //
    // Initialize locals.
    //

    TEST_RESULT test_result = RESULT_FAIL;


    //
    // Create our device and swap chain.
    //

    TEST_RESULT SetupDeviceResult = CreateDeviceAndSwapChain
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
        }

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


    //tests currently only run on default outputs
    UINT refreshRate = 0;
    HRESULT hr = GetRefreshRateForDefault( &refreshRate );
    if ( FAILED( hr ) )
    {
        WriteToLog
        (
            _T("GetRefreshRateForDefault failed with error 0x%x."),
            hr
        );

        goto Cleanup;
    };

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

Cleanup:
    ReleaseTestCaseObjects();

    DebugOut.Write("Exiting ExecuteTestCase.\n");

    return test_result;
};


void CWinDiscard::Cleanup()
{
    DebugOut.Write("Entering Cleanup.\n");

    CleanupTestGroup();
    //CleanupWindowClass();

    DebugOut.Write("Exiting Cleanup.\n");
};
