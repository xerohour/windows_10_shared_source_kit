//
// Project headers
//

#include "DXGIPresentRate.h"


#pragma warning(disable: 4355)


CFSDiscard::CFSDiscard()
{
    m_bIgnoreRefresh = TRUE;
};


CFSDiscard::~CFSDiscard()
{
};


void CFSDiscard::InitTestParameters()
{
    DebugOut.Write("Entering InitTestParameters.\n");


    //
    // Call the base class implementation and initialize FeatureLevelFactor
    //

    CDXGITest::InitTestParameters();
    if( !m_FeatureLevelFactor.Valid())
    {
        WriteToLog
            (
            "Invalid Test Feature Level Factor."
            );
        return;
    }


    //
    // Initialize locals.
    //

    tstring strPath = GetPath().ToString();


    if (InitializeTestGroup())
    {
        m_hEnumerateModesResult = EnumerateModes();
        RFactor rMode = InitModeParameters();

        RFactor rBufferCount = AddParameter(_T("BufferCount"), &iNumBuffersIndex, RangeValueSet(1, MAX_BUFFERS, 1));
        RFactor rSyncInternval = AddParameter(_T("SyncInterval"), &iSyncInterval, RangeValueSet(0, 4, 1));

        if( rMode.Valid() )
        {
            SetRootTestFactor( m_FeatureLevelFactor * rMode * rBufferCount * rSyncInternval );
        }
        else
        {
            WriteToLog
                (
                "Invalid Mode TestFactor generated.  EnumerateModes may have failed."
                );
            m_iTotalModes = 0;

        }

        AddPriorityPatternFilter(FilterLess<int>(_T("SyncInterval"), 2), WeightFactorLarge);

        DeclarePriorityLevelByPriorityValue(1, 5.0f);
        DeclarePriorityLevelByPriorityValue(2, 3.0f);

    };

    DebugOut.Write("Exiting InitTestParameters.\n");
};


TEST_RESULT CFSDiscard::ExecuteTestCase()
{
    DebugOut.Write("Entering ExecuteTestCase.\n");


    //
    // Initialize locals.
    //

    TEST_RESULT test_result = RESULT_FAIL;
    TEST_RESULT SetupDeviceResult = RESULT_FAIL;
    HRESULT hr = S_OK;


    //
    // Validate members and state.
    //

    if ( m_iTotalModes == 0 )
    {
        WriteToLog
            (
            "No supported modes found."
            );

        test_result = RESULT_SKIP;
        goto Cleanup;
    };

    //
    // Create our device and swap chain.
    //

    SetupDeviceResult = CreateDeviceAndSwapChain
        (
        0,
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


    //
    // Create our textures.
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


    UINT refreshRate = 0;
    hr = GetRefreshRate
        (
        &refreshRate,
        m_FSModeInTest.uiDXGIAdapterOrdinal,
        m_FSModeInTest.uiDXGIOutputOrdinal
        );

    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            _T("GetRefreshRate failed with error 0x%x."),
            hr
            );

        goto Cleanup;
    };

    UINT numberOfRetries = 1;

    for(UINT i = 0; i <= numberOfRetries; i++)
    {

        hr = TestFullScreenSwapChain
            (
            (UINT) iSyncInterval,
            refreshRate,
            DXGI_SWAP_EFFECT_DISCARD,      // In other parts of the code we refer to the swapeffect 
                                           // by a hard coded index value of 0 which is an index into 
                                           // the DXGISwapEffects table.  I find that very unreadable
                                           // and am using the actual swap effect instead.   For 
                                           // consistency all the code should be updated to work this
                                           // way, but I'm lazy.
            iNumBuffersIndex,
            &test_result
            );

        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                _T("TestFullScreenSwapChain failed with error 0x%x."),
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
                _T("Test Case Failed.  Sleep for %d ms and re-running test "),
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


void CFSDiscard::Cleanup()
{
    DebugOut.Write("Entering Cleanup.\n");

    CleanupTestGroup();
    //CleanupWindowClass();

    DebugOut.Write("Exiting Cleanup.\n");
};
