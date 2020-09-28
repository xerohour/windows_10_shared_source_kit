////////////////////////////////////////////////////////////////////////////////
// TimingData.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "MarkerValidation.h"

////////////////////////////////////////////////////////////////////////////////
// CMarkerValidationTest
////////////////////////////////////////////////////////////////////////////////

CMarkerValidationTest::CMarkerValidationTest(bool fEnableETW) : CTimingDataTest(fEnableETW)
{
}

TEST_RESULT CMarkerValidationTest::SetupTestCase()
{
    TEST_RESULT tr = CTimingDataTest::SetupTestCase();
    HRESULT hr = S_OK;

    if(tr != RESULT_PASS)
    {
        return tr;
    }

    tr = SetupPipeline();

    return tr;
}

void CMarkerValidationTest::CleanupTestCase()
{
    CTimingDataTest::CleanupTestCase();

    CleanupPipeline();
}

////////////////////////////////////////////////////////////////////////////////
// CMarkerValidationTest_CommandList
////////////////////////////////////////////////////////////////////////////////

TEST_RESULT CMarkerValidationTest_CommandList::Setup()
{
    TEST_RESULT tr = CMarkerValidationTest::Setup();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    if(GetFramework()->IsDeferredContextUsed() == false)
    {
        WriteToLog(_T("Skipping test group since the test is not running on a deferred context."));
        return RESULT_SKIP;
    }

    return RESULT_PASS;
}

TEST_RESULT CMarkerValidationTest_CommandList::ExecuteTestCase()
{
    HRESULT hr = E_FAIL;
    TEST_RESULT tr = StartETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    m_pDeviceContext->Draw(4, 0);
    m_pDeviceContext->Draw(4, 0);

    if(FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
                hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute); 
        return RESULT_FAIL;
    }

    // Finish/Execute so that APISequenceNumber resets

    m_pDeviceContext->Draw(4, 0);

    if(FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
                hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute); 
        return RESULT_FAIL;
    }

    tr = StopETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    // ensure command list event appears and that it matches our deferred context pointer
    vector<CEvent_CommandList*> commandListList;
    tr = FilterCommandListEvents(&commandListList);
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    bool foundContext = false;

    for(UINT i = 0; i < commandListList.size(); ++i)
    {
        if(commandListList[i]->ID3D11DeviceContext() == (UINT32)m_pDeviceContext)
        {
            foundContext = true;
            break;
        }
    }

    if(foundContext == false)
    {
        LogError(__FILEW__, __LINE__, L"Command list event for test's deferred context was not found",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
        tr = RESULT_FAIL;
    }

    // validate API sequence number resets
    vector<CEvent_CommandBufferSubmission*> cmdBufferList;
    tr = FilterCommandBufferEvents(&cmdBufferList);
    if(tr != RESULT_PASS || cmdBufferList.size() == 0)
    {
        LogError(__FILEW__, __LINE__, L"Expected a command buffer submission event",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
        return RESULT_FAIL;
    }

    UINT32 zeroesFound = 0;

    for(UINT i = 0; i < cmdBufferList.size(); ++i)
    {
        vector<UINT64> APISeqNums;  

        tr = FormSequenceNumbers(&APISeqNums,
                cmdBufferList[i]->FirstAPISequenceNumberHigh(), 
                cmdBufferList[i]->BegunAPISequenceNumberLow0(),
                cmdBufferList[i]->BegunAPISequenceNumberLow1());

        if(tr != RESULT_PASS)
        {
            return tr;
        }

        for(UINT j = 0; j < APISeqNums.size(); ++j)
        {
            if(APISeqNums[j] == 0)
            {
                zeroesFound++;
            }
        }
    }

    if(zeroesFound < 2)
    {
        LogError(__FILEW__, __LINE__, L"APISequenceNumber did not reset after FinishCommandList",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
        tr = RESULT_FAIL;
    }

    return tr;
}

////////////////////////////////////////////////////////////////////////////////
// CMarkerValidationTest_APISequence
////////////////////////////////////////////////////////////////////////////////

#define NUM_RENDER_OPS_API  10

TEST_RESULT CMarkerValidationTest_APISequence::SetupTestCase()
{
    return CMarkerValidationTest::SetupTestCase();
}

TEST_RESULT CMarkerValidationTest_APISequence::ExecuteTestCase()
{
    vector<UINT64> begunSeqNums;
    vector<UINT64> completedSeqNums;
    vector<UINT32> renderCBSequenceNumbers;
    vector<CEvent_HistoryBuffer*> FilteredEvents;

    vector<UINT64> completedRenderCbs;
    UINT64 lastBegun = 0;
    UINT64 lastCompleted = 0;
    UINT64 lastRenderCB = 0;
    vector<CEvent_CommandBufferSubmission*> cmdBufferList;
    vector<CEvent_HistoryBuffer*> historyBufferList;
    vector<UINT64> timestamps;
    vector<UINT64> ElapsedTime;

    HRESULT hr = E_FAIL;
    bool fFailure = false;

    //
    // Start ETW tracing so we're sure we start with API #0 as real work.
    //
    TEST_RESULT tr = StartETWTracing(true); //flag : start with Draw before SetMarker 
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    
    //
    // Add some work
    //
    for(UINT32 i = 0; i < NUM_RENDER_OPS_API; ++i)
    {
        m_pDeviceContext->Draw(4, 0);
        m_pDeviceContext->SetMarkerInt(L"ZeroTime", 0);
        m_pDeviceContext->Draw(4, 0);
        m_pDeviceContext->Flush();
        TogglePS();
    }

    if(FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
                hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute); 
        goto Fail;
    }

    tr = StopETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    // validation
    FilterCommandBufferEvents(&cmdBufferList);
    if(cmdBufferList.size() == 0)
    {
        LogError(__FILEW__, __LINE__, L"Expected command buffer submission events but none appeared.",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
        return RESULT_FAIL;
    }

    // accumulate API and RenderCB sequence numbers
    completedRenderCbs.clear();
    UINT64 TotalApiSeqNums = 0;
    UINT64 DxgContext = 0;
    for(UINT i = 0; i < cmdBufferList.size(); ++i)
    {
        if(cmdBufferList[i]->RenderCBSequenceNumber() == 1)
        {
            DxgContext = cmdBufferList[i]->Context();
            break;
        }
    }
    for(vector<CEvent_CommandBufferSubmission *>::iterator It = cmdBufferList.begin(); It != cmdBufferList.end();)
    {
        if(DxgContext != (*It)->Context())
        {
            cmdBufferList.erase(It);
        }
        else
        {
            It++;
        }
    }
    
    UINT64 BroadcastContexts = 1; // The number of broadcast contexts
    CEvent_CommandBufferSubmission* It = cmdBufferList.front();
    for(UINT32 Context : It->BroadcastContexts())
    {
        if(Context != It->Context())
        {
            BroadcastContexts++;
        }
    }
    for(UINT i = 0; i < cmdBufferList.size(); ++i)
    {
        vector<UINT64> APISeqNums;

        tr = FormSequenceNumbers(&APISeqNums,
                cmdBufferList[i]->FirstAPISequenceNumberHigh(), 
                cmdBufferList[i]->BegunAPISequenceNumberLow0(),
                cmdBufferList[i]->BegunAPISequenceNumberLow1());

        if(tr != RESULT_PASS)
        {
            goto Fail;
        }
        
        for(UINT seqNum = 0; seqNum < APISeqNums.size(); ++seqNum)
        {
            if(find(begunSeqNums.begin(), begunSeqNums.end(), APISeqNums[seqNum]) == begunSeqNums.end())
            {
                begunSeqNums.push_back(APISeqNums[seqNum]);
            }
        }
        
        APISeqNums.clear();
        TotalApiSeqNums += cmdBufferList[i]->CompletedAPISequenceNumberLow0().size() + cmdBufferList[i]->CompletedAPISequenceNumberLow1().size();
        tr = FormSequenceNumbers(&APISeqNums,
                cmdBufferList[i]->FirstAPISequenceNumberHigh(), 
                cmdBufferList[i]->CompletedAPISequenceNumberLow0(),
                cmdBufferList[i]->CompletedAPISequenceNumberLow1());

        if(tr != RESULT_PASS)
        {
            goto Fail;
        }
        
        if (APISeqNums.size() != 0)
        {
            completedRenderCbs.push_back(cmdBufferList[i]->RenderCBSequenceNumber());
        }

        for(UINT seqNum = 0; seqNum < APISeqNums.size(); ++seqNum)
        {
            if(find(completedSeqNums.begin(), completedSeqNums.end(), APISeqNums[seqNum]) == completedSeqNums.end())
            {
                completedSeqNums.push_back(APISeqNums[seqNum]);
            }
        }

        renderCBSequenceNumbers.push_back(cmdBufferList[i]->RenderCBSequenceNumber());
    }

    // test that API sequence numbers begin and are completed in order
    for(UINT i = 0; i < begunSeqNums.size(); ++i)
    {
        if(begunSeqNums[i] < lastBegun)
        {
            WriteToLog(_T("APISequenceNumber %I64d appeared after %I64d in Begun array."), begunSeqNums[i], lastBegun);
            LogError(__FILEW__, __LINE__, L"APISequenceNumbers did not begin in order.",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
            goto Fail;
        }

        lastBegun = begunSeqNums[i];
    }
    
    for(UINT i = 0; i < completedSeqNums.size(); ++i)
    {
        if(completedSeqNums[i] < lastCompleted)
        {
            WriteToLog(_T("APISequenceNumber %I64d appeared after %I64d in Completed array."), completedSeqNums[i], lastBegun);
            LogError(__FILEW__, __LINE__, L"APISequenceNumbers did not complete in order.",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
            goto Fail;
        }

        lastCompleted = completedSeqNums[i];
    }

    // verify that RenderCBSequence monotonically increments
    for(UINT i = 0; i < renderCBSequenceNumbers.size(); ++i)
    {
        if (!fFailure && (renderCBSequenceNumbers[i] < lastRenderCB))
        {
            WriteToLog(_T("RenderCBSequenceNumber %I64d appeared after %I64d."), renderCBSequenceNumbers[i], lastRenderCB);
            LogError(__FILEW__, __LINE__, L"RenderCBSequenceNumbers did not monotonically increment.",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
            fFailure = true;
        }
        lastRenderCB = renderCBSequenceNumbers[i];
    }   

    tr = FilterHistoryBufferEvents(&historyBufferList, true);   //verify order of timestamps as well.
    if (tr != RESULT_PASS)
    {
        fFailure = true;
    }
    if (historyBufferList.size() == 0)
    {
        LogError(__FILEW__, __LINE__, L"Expected history buffer events but none appeared.",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
        goto Fail;
    }
    tr = CorrelateTimestampsToApiSequenceNumbers(&historyBufferList, &timestamps, &ElapsedTime, completedRenderCbs, nullptr);
    if(tr != RESULT_PASS)
    {
        goto Fail;
    }
    UINT64 ValidationNumber = 0;
    if(GetFramework()->Is11On12())
    {
        ValidationNumber = 1;
    }
    if (completedSeqNums.size() > 0)
    {
        //
        // Verify that the first Completed Api was 0. This will be true because it is a Draw
        //
        if (completedSeqNums[0] != ValidationNumber)
        {
            WriteToLog(_T("Completed APISequence Number must begin with %d"), ValidationNumber);
            LogError(__FILEW__, __LINE__, L"Completed APISequence Number must begin with zero.",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
            fFailure = true;
        }
    }

    // verify that the number of timestamps match the number of completed APISequenceNumbers (+2 per DMA buffer)
    ValidationNumber = (completedRenderCbs.size() * 2 + completedSeqNums.size()) * BroadcastContexts;
    if(GetFramework()->Is11On12())
    {
        ValidationNumber = TotalApiSeqNums;
    }
    if (timestamps.size() != (ValidationNumber))
    {
        WriteToLog(_T("CompletedRenderCbs: %d\nCompleted APISequenceNumbers: %d\nNumber of timestamps: %d"), completedRenderCbs.size(), completedSeqNums.size(), timestamps.size());
        LogError(__FILEW__, __LINE__, L"Number of timestamps must be equal to the number of completed API sequence numbers (plus 2 per DMA buffer).",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
        goto Fail;
    }
    if (fFailure)
    {
        goto Fail;
    }
    goto Done;
Fail:
    tr = RESULT_FAIL;
    StopETWTracing();

Done:
    return tr;
}

////////////////////////////////////////////////////////////////////////////////
// CMarkerValidationTest_Optimized
////////////////////////////////////////////////////////////////////////////////

#define NUM_RT_CLEARS   30

TEST_RESULT CMarkerValidationTest_Optimized::ExecuteTestCase()
{
    HRESULT hr = E_FAIL;
    TEST_RESULT tr = StartETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    const FLOAT clearColor [] = { 1.0f, 0.0f, 0.0f, 1.0f };

    // all but one draw should be optimized out by the driver
    for(UINT32 i = 0; i < NUM_RT_CLEARS; ++i)
    {
        m_pDeviceContext->Draw(4, 0);

        m_pDeviceContext->Flush();
    }

    // clear call to log a new API sequence number
    m_pDeviceContext->ClearRenderTargetView(m_pRTView, clearColor); 

    if(FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
                hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute); 
        return RESULT_FAIL;
    }

    tr = StopETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    // check number of dropped API calls
    vector<CEvent_RuntimeDroppedCall*> droppedCallList;
    tr = FilterDroppedCallEvents(&droppedCallList);
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    UINT droppedAPICalls = droppedCallList.size();

    // validate API sequence numbers
    vector<CEvent_CommandBufferSubmission*> cmdBufferList;
    tr = FilterCommandBufferEvents(&cmdBufferList);
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    if(cmdBufferList.size() == 0)
    {
        LogError(__FILEW__, __LINE__, L"Expected at least one command buffer submission event",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
        return RESULT_FAIL;
    }

    vector<UINT64> APISeqNums;  

    for(UINT i = 0; i < cmdBufferList.size(); ++i)
    {
        vector<UINT64> eventAPINums;
        if(!GetFramework()->Is11On12())
        {
            tr = FormSequenceNumbers(&eventAPINums,
                cmdBufferList[i]->FirstAPISequenceNumberHigh(), 
                cmdBufferList[i]->BegunAPISequenceNumberLow0(),
                cmdBufferList[i]->BegunAPISequenceNumberLow1());        
        }
        else
        {
            // Need to check completed instead for 12 because there is no Begun
            tr = FormSequenceNumbers(&eventAPINums,
                cmdBufferList[i]->FirstAPISequenceNumberHigh(), 
                cmdBufferList[i]->CompletedAPISequenceNumberLow0(),
                cmdBufferList[i]->CompletedAPISequenceNumberLow1());
        }

        if(tr != RESULT_PASS)
        {
            return tr;
        }

        for(UINT j = 0; j < eventAPINums.size(); ++j)
        {
            APISeqNums.push_back(eventAPINums[j]);
        }
    }

    if(APISeqNums.size() < 2)
    {
        WriteToLog(_T("APISequenceNumbers found: %d"), APISeqNums.size());
        LogError(__FILEW__, __LINE__, L"Expected at least two APISequenceNumbers",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
        return RESULT_FAIL;
    }

    UINT64 firstNum = APISeqNums[APISeqNums.size() - 1];
    UINT64 lastNum = APISeqNums[APISeqNums.size() - 2];

    UINT32 difference = (UINT32)(lastNum - firstNum);
    if(difference < (droppedAPICalls))
    {
        WriteToLog(_T("First APISequenceNumber: %I64d\nNext APISequenceNumber: %I64d\tDropped API calls: %d"),
                firstNum, lastNum, droppedAPICalls);
        LogError(__FILEW__, __LINE__, L"Insufficient APISequenceNumbers were skipped on dropped API calls",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
        return RESULT_FAIL;
    }

    return RESULT_PASS;
}

////////////////////////////////////////////////////////////////////////////////
// CMarkerValidationTest_Flush
////////////////////////////////////////////////////////////////////////////////

TEST_RESULT CMarkerValidationTest_Flush::SetupTestCase()
{
    return CMarkerValidationTest::SetupTestCase();
}

TEST_RESULT CMarkerValidationTest_Flush::ExecuteTestCase()
{
    HRESULT hr = E_FAIL;
    TEST_RESULT tr = StartETWTracing();
    if (tr != RESULT_PASS)
    {
        return tr;
    }

    m_pDeviceContext->Draw(4, 0);

    if (FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
            hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute);
        return RESULT_FAIL;
    }

    m_pDeviceContext->Flush();

    TogglePS();

    m_pDeviceContext->Draw(4, 0);

    if (FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
            hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute);
        return RESULT_FAIL;
    }

    tr = StopETWTracing();
    if (tr != RESULT_PASS)
    {
        return tr;
    }

    // validate API sequence number goes from begun to completed after flush
    vector<CEvent_CommandBufferSubmission*> cmdBufferList;
    tr = FilterCommandBufferEvents(&cmdBufferList);
    if (tr != RESULT_PASS)
    {
        return tr;
    }

    if (cmdBufferList.size() < 2)
    {
        WriteToLog(_T("Number of events found: %d"), cmdBufferList.size());
        LogError(__FILEW__, __LINE__, L"Expected at least two command buffer submission events",
            false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
        return RESULT_FAIL;
    }

    for (unsigned u = 0; u < cmdBufferList.size(); u++)
    {
        vector<UINT64> firstBegunAPISeqNums;
        tr = FormSequenceNumbers(&firstBegunAPISeqNums,
            cmdBufferList[u]->FirstAPISequenceNumberHigh(),
            cmdBufferList[u]->BegunAPISequenceNumberLow0(),
            cmdBufferList[u]->BegunAPISequenceNumberLow1());

        if (tr != RESULT_PASS)
        {
            return tr;
        }
        for (unsigned v = u + 1; v < cmdBufferList.size(); v++)
        {
            if (cmdBufferList[v]->Context() != cmdBufferList[u]->Context())
            {
                continue;
            }
            vector<UINT64> lastBegunAPISeqNums;

            tr = FormSequenceNumbers(&lastBegunAPISeqNums,
                cmdBufferList[v]->FirstAPISequenceNumberHigh(),
                cmdBufferList[v]->BegunAPISequenceNumberLow0(),
                cmdBufferList[v]->BegunAPISequenceNumberLow1());

            //
            // Verify that we don't see the same begun twice
            //

            for (unsigned w = 0; w < firstBegunAPISeqNums.size(); w++)
            {
                for (unsigned x = 0; x < lastBegunAPISeqNums.size(); x++)
                {
                    if (lastBegunAPISeqNums[x] == firstBegunAPISeqNums[w])
                    {
                        LogError(__FILEW__, __LINE__, L"APISequenceNumber for Draw operation appears in Begun array after flush when it should be absent",
                            false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
                        tr = RESULT_FAIL;
                    }
                }
            }

        }
    }
    return tr;
}

////////////////////////////////////////////////////////////////////////////////
// CMarkerValidationTest_CommandBufferLimit
////////////////////////////////////////////////////////////////////////////////

#define MAX_BUFFER_SIZE_PLUS_1  8173

TEST_RESULT CMarkerValidationTest_CommandBufferLimit::ExecuteTestCase()
{
    if(GetFramework()->Is11On12())
    {
        // This limitation does not apply for D3D12 so skip if it is 11 on 12
        return RESULT_SKIP;
    }
    HRESULT hr = E_FAIL;
    TEST_RESULT tr = StartETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    for(UINT32 i = 0; i < MAX_BUFFER_SIZE_PLUS_1; ++i)
    {
        m_pDeviceContext->Draw(4, 0);

        TogglePS();
    }

    if(FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
                hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute); 
        return RESULT_FAIL;
    }

    tr = StopETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    // validate API sequence numbers don't exceed 8172
    vector<CEvent_CommandBufferSubmission*> cmdBufferList;
    tr = FilterCommandBufferEvents(&cmdBufferList);
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    if(cmdBufferList.size() == 0)
    {
        LogError(__FILEW__, __LINE__, L"Expected at least one command buffer submission event",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
        return RESULT_FAIL;
    }

    WriteToLog(_T("APISequenceNumbers issued: %d"), MAX_BUFFER_SIZE_PLUS_1 + 1);

    struct ApiCounts
    {
        UINT64 hContext;
        UINT64 Size;
    };
    std::vector<ApiCounts> counts;

    for (unsigned u = 0; u < cmdBufferList.size(); u++)
    {
        vector<UINT64> BegunNumbers;
        vector<UINT64> CompletedNumbers;

        tr = FormSequenceNumbers(&BegunNumbers,
            cmdBufferList[u]->FirstAPISequenceNumberHigh(),
            cmdBufferList[u]->BegunAPISequenceNumberLow0(),
            cmdBufferList[u]->BegunAPISequenceNumberLow1());

        if (tr != RESULT_PASS)
        {
            return tr;
        }

        tr = FormSequenceNumbers(&CompletedNumbers,
            cmdBufferList[u]->FirstAPISequenceNumberHigh(),
            cmdBufferList[u]->CompletedAPISequenceNumberLow0(),
            cmdBufferList[u]->CompletedAPISequenceNumberLow1());

        if (tr != RESULT_PASS)
        {
            return tr;
        }

        if ((BegunNumbers.size() + CompletedNumbers.size()) >= MAX_BUFFER_SIZE_PLUS_1)
        {
            WriteToLog(_T("APISequenceNumbers found in this command buffer: %d"), (BegunNumbers.size() + CompletedNumbers.size()));
            LogError(__FILEW__, __LINE__, L"Operations per command buffer exceeded the limit of 8172",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
            return RESULT_FAIL;
        }
        unsigned v = 0;
        for (; v < counts.size(); v++)
        {
            if (counts[v].hContext == cmdBufferList[u]->Context())
            {
                break;
            }
        }
        if (v >= counts.size())
        {
            ApiCounts Data = {};
            Data.hContext = cmdBufferList[u]->Context();
            counts.push_back(Data);
        }
        counts[v].Size += CompletedNumbers.size();
    }
    //
    // validate...
    //
    if (!GetFramework()->IsMobile())
    {
        if (m_pXperfHelper == NULL)
        {
            m_pXperfHelper = new XperfHelper(NORMAL, true, LARGE, FALSE);
            m_pXperfHelper->SetMergedFile(ETLFileName());
        }
        hr = m_pXperfHelper->Action(L"gputiming -test 2048 -noheader", RESULTS_FILE, L"perf_gputiming.dll");
        hr = S_OK;              //return code from Action() currently unreliable - todo
        if (!SUCCEEDED(hr))
        {
            tr = RESULT_FAIL;
        }
        if (tr == RESULT_PASS)
        {
            tr = ReadResultFile();
        }
    }
    if (tr == RESULT_FAIL)
    {
        LogError(__FILEW__, __LINE__, L"Failure CommandBufferLimit", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
        return RESULT_FAIL;
    }

    return RESULT_PASS;
}

////////////////////////////////////////////////////////////////////////////////
// CMarkerValidationTest_Range
////////////////////////////////////////////////////////////////////////////////

TEST_RESULT CMarkerValidationTest_Range::ExecuteTestCase()
{
    HRESULT hr = E_FAIL;
    TEST_RESULT tr = StartETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    LARGE_INTEGER qpcTimestamps[2];
    LARGE_INTEGER cpuFrequency;
    vector<CEvent_HistoryBuffer*> timestampEvents;
    vector<CEvent_HistoryBuffer*> filteredTimestampEvents;
    vector<UINT64> timestamps;
    
    if(QueryPerformanceCounter(&qpcTimestamps[0]) == FALSE)
    {
        LogError(__FILEW__, __LINE__, L"QueryPerformanceCounter() failed", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCQueryPerformanceCounter); 
        goto Fail;
    }

    m_pDeviceContext->Draw(4, 0);

    if(FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
                hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute); 
        goto Fail;
    }

    if(QueryPerformanceCounter(&qpcTimestamps[1]) == FALSE)
    {
        LogError(__FILEW__, __LINE__, L"QueryPerformanceCounter() failed", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCQueryPerformanceCounter); 
        goto Fail;
    }

    tr = StopETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    // validation
    tr = FilterHistoryBufferEvents(&timestampEvents);
    if(tr != RESULT_PASS)
    {
        goto Fail;
    }

    tr = CorrelateTimestamps(&timestampEvents, &timestamps, &filteredTimestampEvents);
    if(tr != RESULT_PASS)
    {
        goto Fail;
    }

    if(filteredTimestampEvents.size() < 1)
    {
        WriteToLog(_T("Number of events found: %d"), filteredTimestampEvents.size());
        LogError(__FILEW__, __LINE__, L"Expected at least one history buffer event", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCRequiredCapabilityNotSupported); 
        goto Fail;
    }

    if(QueryPerformanceFrequency(&cpuFrequency) == FALSE)
    {
        LogError(__FILEW__, __LINE__, L"QueryPerformanceFrequency() failed", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCQueryPerformanceCounter); 
        goto Fail;
    }

    //
    // EventTime is time in 100ns from the start of the trace.
    // Range check dt = t(0) - t(e); x < dt;
    //
    qpcTimestamps[0].QuadPart -= qpcTimestamps[1].QuadPart;
    qpcTimestamps[0].QuadPart /= (cpuFrequency.QuadPart * 10000000ULL);
    if(filteredTimestampEvents[0]->EventTime() < (UINT64)qpcTimestamps[0].QuadPart)
    {
        LogError(__FILEW__, __LINE__, L"GPU timestamp event doesnt fall between DMA buffer timestamps", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
        WriteToLog(_T("GPU Timestamp: %I64d\nDMA begin timestamp: %I64d\nDMA end timestamp: %I64d"),
            filteredTimestampEvents[0]->EventTime(), qpcTimestamps[0].QuadPart, qpcTimestamps[1].QuadPart);
        goto Fail;
    }

    goto Done;

Fail:
    tr = RESULT_FAIL;
    StopETWTracing();

Done:
    return tr;
}