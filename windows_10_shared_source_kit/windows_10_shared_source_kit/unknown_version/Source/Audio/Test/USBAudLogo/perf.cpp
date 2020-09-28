// Copyright (C) Microsoft. All rights reserved.
#include "stdafx.h"
#include <common.h>
#include <cptr.h>

#include <HidConsts.h>
#include <HidCollection.h>
#include <TestDevice.h>
#include <TestDevnode.h>
#include <TestSetupDevInfo.h>
#include <TestHidDevice.h>
#include <HidReport.h>
#include <SRDeviceFromITestDevice.h>
#include <AudioPlastic.h>
#include <TestHidReport.h>
#include <StressSafeFormattedString.h>
#include <latencystats.h>
#include "audiohidtestclient.h"
#include "tests.h"

static const UINT TEST_SAMPLE_COUNT = 10;
static const float TEST_TARGET_AVERAGE = 100; //ms
static const float TEST_TARGET_UPPER_BOUND = 300; //ms


DWORD CUSBHidTest::TestPerf(EHidControl hidCtrl, UINT cSamples)
{
    HRESULT hr = S_OK;
    
    CLatencyDataset startDataset;
    CLatencyDataset endDataset;
        
    for (UINT idxSample = 0; idxSample < cSamples; idxSample++)
    {
        m_pIBasicLog->Log(XMSG, 10, FormatStr(L"=============== Test Sample %d ==================", idxSample + 1));
        CComPtr<IAudioHidPerfTestClient> pClient;
        hr = CreateAudioHidPerfTestClient(m_pIBasicLog, &startDataset, &endDataset, &pClient);
        IF_FAILED_RETURN(m_pIBasicLog, hr, L"Failed CreateAudioHidPerfTestClient", TEST_RESULT_FAIL);

        hr = pClient->Test(hidCtrl);
        IF_FAILED_RETURN(m_pIBasicLog, hr, L"Failed IAudioHidPerfTestClient::Test", TEST_RESULT_FAIL);
    }

    // let's do some math and calculate the result here
    CLatencyStats stats(startDataset, endDataset, hr);
    IF_FAILED_RETURN(m_pIBasicLog, hr, L"Failed CLatencyStats::CLatencyStats", TEST_RESULT_FAIL);

    m_pIBasicLog->Log(XMSG, 1, L"Printing raw dataset collected...");
    stats.PrintDataSet(m_pIBasicLog, 10); 
    m_pIBasicLog->Log(XMSG, 1, FormatStr(L"Average = %0.2fms", stats.GetAverage()));
    m_pIBasicLog->Log(XMSG, 1, FormatStr(L"Maximum = %0.2fms", stats.GetMaximum()));
    m_pIBasicLog->Log(XMSG, 1, FormatStr(L"Minimum = %0.2fms", stats.GetMinimum()));
    m_pIBasicLog->Log(XMSG, 1, FormatStr(L"Standard Deviation = %0.2fms", stats.GetStandardDeviation()));

    if (stats.GetAverage() > TEST_TARGET_AVERAGE) 
    {
        m_pIBasicLog->Log(XFAIL, 1, FormatStr(L"Average greater than target (%0.2fms)", TEST_TARGET_AVERAGE));
    }
    if (stats.GetMaximum() > TEST_TARGET_UPPER_BOUND) 
    {
        m_pIBasicLog->Log(XFAIL, 1, FormatStr(L"Maximum greater than target (%0.2fms)", TEST_TARGET_UPPER_BOUND));
    }
    
    return TEST_RESULT_PASS;
}

DWORD CUSBHidTest::TC_Perf_Mute(ISRDevice * /*pDevice*/)
{
    return TestPerf(EHidControl_Mute, TEST_SAMPLE_COUNT);
}

DWORD CUSBHidTest::TC_Perf_VolumeUp(ISRDevice * /*pDevice*/)
{
    return TestPerf(EHidControl_VolUp, TEST_SAMPLE_COUNT);
}

DWORD CUSBHidTest::TC_Perf_VolumeDown(ISRDevice * /*pDevice*/)
{
    return TestPerf(EHidControl_VolDown, TEST_SAMPLE_COUNT);
}
