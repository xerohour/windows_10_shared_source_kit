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
#include <latencystats.h>
#include "audiohidtestclient.h"
#include "tests.h"

DWORD CUSBHidTest::TC_Telephony_Hook(ISRDevice *pDevice)
{
    bool bTestPassed = false;
    CComPtr<ITestHidDevice> pHidDev;
    HRESULT hr = ((CSRAudioPlastic*)pDevice)->GetHidDevice(GUID_HID_TELEPHONY, &pHidDev);
    if (E_NOTFOUND == hr) 
    {
        m_pIBasicLog->Log(XSKIP, hr, L"HID telephony device not found, skipping...");
        return TEST_RESULT_SKIPPED;
    }
    IF_FAILED_RETURN(m_pIBasicLog, hr, L"Failed CSRAudioPlastic::GetHidDevice", TEST_RESULT_FAIL);
    
    // reports expected
    CTestHidReport report1(TELEPHONY_PAGE, TELEPHONY_HOOK_SWITCH);
    CTestHidReport report2(TELEPHONY_PAGE, BUTTONS_ALL_OFF);
    PCTestHidReport rgpTestReports[2] = {&report1, &report2};

    // run test
    hr = InputReportManualTest
    (
        m_pIBasicLog,                                           // logger
        pHidDev,                                                // device under test
        L"Press HOOK SWITCH",                                   // prompt for user
        rgpTestReports,                                         // first of the series of usages to be expected
        2,                                                      // total number of usages
        false,                                                  // is testing on this input report required?
        bTestPassed                                             // all expected results met
    );
    IF_FAILED_RETURN(m_pIBasicLog, hr, L"Failed InputReportManualTest", TEST_RESULT_FAIL);

    if (!bTestPassed)
    {
        m_pIBasicLog->Log(XFAIL, 1, L"Failed test case");
        return TEST_RESULT_FAIL;
    }
    
    return TEST_RESULT_PASS;
}

DWORD CUSBHidTest::TC_Telephony_SpeakerPhone(ISRDevice *pDevice)
{
    bool bTestPassed = false;
    CComPtr<ITestHidDevice> pHidDev;
    HRESULT hr = ((CSRAudioPlastic*)pDevice)->GetHidDevice(GUID_HID_TELEPHONY, &pHidDev);
    if (E_NOTFOUND == hr) 
    {
        m_pIBasicLog->Log(XSKIP, hr, L"HID telephony device not found, skipping...");
        return TEST_RESULT_SKIPPED;
    }
    IF_FAILED_RETURN(m_pIBasicLog, hr, L"Failed CSRAudioPlastic::GetHidDevice", TEST_RESULT_FAIL);
    
    // reports expected
    CTestHidReport report1(TELEPHONY_PAGE, TELEPHONY_SPEAKER_PHONE);
    CTestHidReport report2(TELEPHONY_PAGE, BUTTONS_ALL_OFF);
    PCTestHidReport rgpTestReports[2] = {&report1, &report2};

    // run test
    hr = InputReportManualTest
    (
        m_pIBasicLog,                                           // logger
        pHidDev,                                                // device under test
        L"Press SPEAKER PHONE",                                 // prompt for user
        rgpTestReports,                                         // first of the series of usages to be expected
        2,                                                      // total number of usages
        false,                                                  // is testing on this input report required?
        bTestPassed                                             // all expected results met
    );
    IF_FAILED_RETURN(m_pIBasicLog, hr, L"Failed InputReportManualTest", TEST_RESULT_FAIL);

    if (!bTestPassed)
    {
        m_pIBasicLog->Log(XFAIL, 1, L"Failed test case");
        return TEST_RESULT_FAIL;
    }
    
    return TEST_RESULT_PASS;
}

DWORD CUSBHidTest::TC_Telephony_PhoneMute(ISRDevice *pDevice)
{
    bool bTestPassed = false;
    CComPtr<ITestHidDevice> pHidDev;
    HRESULT hr = ((CSRAudioPlastic*)pDevice)->GetHidDevice(GUID_HID_TELEPHONY, &pHidDev);
    if (E_NOTFOUND == hr) 
    {
        m_pIBasicLog->Log(XSKIP, hr, L"HID telephony device not found, skipping...");
        return TEST_RESULT_SKIPPED;
    }
    IF_FAILED_RETURN(m_pIBasicLog, hr, L"Failed CSRAudioPlastic::GetHidDevice", TEST_RESULT_FAIL);
    
    // reports expected
    CTestHidReport report1(TELEPHONY_PAGE, TELEPHONY_PHONE_MUTE);
    CTestHidReport report2(TELEPHONY_PAGE, BUTTONS_ALL_OFF);
    PCTestHidReport rgpTestReports[2] = {&report1, &report2};

    // run test
    hr = InputReportManualTest
    (
        m_pIBasicLog,                                           // logger
        pHidDev,                                                // device under test
        L"Press PHONE MUTE",                                    // prompt for user
        rgpTestReports,                                         // first of the series of usages to be expected
        2,                                                      // total number of usages
        false,                                                  // is testing on this input report required?
        bTestPassed                                             // all expected results met
    );
    IF_FAILED_RETURN(m_pIBasicLog, hr, L"Failed InputReportManualTest", TEST_RESULT_FAIL);

    if (!bTestPassed)
    {
        m_pIBasicLog->Log(XFAIL, 1, L"Failed test case");
        return TEST_RESULT_FAIL;
    }
    
    return TEST_RESULT_PASS;
}

DWORD CUSBHidTest::TC_Consumer_Volume(ISRDevice *pDevice)
{    
    bool bTestPassed = false;
    CComPtr<ITestHidDevice> pHidDev;
    HRESULT hr = ((CSRAudioPlastic*)pDevice)->GetHidDevice(GUID_HID_CONSUMER, &pHidDev);
    if (E_NOTFOUND == hr) 
    {
        m_pIBasicLog->Log(XSKIP, hr, L"HID telephony device not found, skipping...");
        return TEST_RESULT_SKIPPED;
    }
    IF_FAILED_RETURN(m_pIBasicLog, hr, L"Failed CSRAudioPlastic::GetHidDevice", TEST_RESULT_FAIL);
    
    // reports expected
    CTestHidReport report1(CONSUMER_PAGE, CONSUMER_VOLUME_INCREMENT);
    CTestHidReport report2(CONSUMER_PAGE, BUTTONS_ALL_OFF);
    CTestHidReport report3(CONSUMER_PAGE, CONSUMER_VOLUME_DECREMENT);
    CTestHidReport report4(CONSUMER_PAGE, BUTTONS_ALL_OFF);
    PCTestHidReport rgpTestReports[4] = {&report1, &report2, &report3, &report4};

    // run test
    hr = InputReportManualTest
        (
        m_pIBasicLog,                                           // logger
        pHidDev,                                                // device under test
        L"Volume Up\n Volume Down",                              // prompt for user
        rgpTestReports,                                         // first of the series of usages to be expected
        4,                                                      // total number of usages
        false,                                                  // is testing on this input report required?
        bTestPassed                                             // all expected results met
        );
    IF_FAILED_RETURN(m_pIBasicLog, hr, L"Failed InputReportManualTest", TEST_RESULT_FAIL);

    if (!bTestPassed)
    {
        m_pIBasicLog->Log(XFAIL, 1, L"Failed test case");
        return TEST_RESULT_FAIL;
    }

    return TEST_RESULT_PASS;
}
