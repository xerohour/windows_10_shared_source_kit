// Copyright (C) Microsoft. All rights reserved.
#include "stdafx.h"
#include <common.h>
#include <shlflags.h>
#include <cptr.h>

#include <HidConsts.h>
#include <HidCollection.h>
#include <TestDevice.h>
#include <TestDevnode.h>
#include <TestSetupDevInfo.h>
#include <TestHidDevice.h>
#include <HidReport.h>
#include <devpkey.h>
#include <SRDeviceFromITestDevice.h>
#include <AudioPlastic.h>

#include <TestHidReport.h>
#include <latencystats.h>
#include "audiohidtestclient.h"
#include "tests.h"
#include <ShellRunnerTestGroupHelper.h>

// {A02F268B-D663-411e-A5B6-5E80B45B9053}
static const GUID DLLGUID = 
{ 0xa02f268b, 0xd663, 0x411e, { 0xa5, 0xb6, 0x5e, 0x80, 0xb4, 0x5b, 0x90, 0x53 } };


static SRTestCaseData<CUSBHidTest> g_rgUSBHidTest[] =
{
    {L"Telephony Hook",             L"{0AFBC083-4F16-4B47-9B09-919F71EE4F9B}", DEV_OR, TESTFLAG_MANUAL, &CUSBHidTest::TC_Telephony_Hook},
    {L"Telephony Speaker Phone",    L"{20EAFD89-22BF-4A3F-8909-B7EBF6BBFA84}", DEV_OR, TESTFLAG_MANUAL, &CUSBHidTest::TC_Telephony_SpeakerPhone},
    {L"Telephony Phone Mute",       L"{0C58F729-0C03-4DEB-91CA-2179B432EAF9}", DEV_OR, TESTFLAG_MANUAL, &CUSBHidTest::TC_Telephony_PhoneMute},
    {L"Consumer Volume",            L"{2166AEFB-4ED1-4BD6-9583-B080BEA3AB52}", DEV_OR, TESTFLAG_MANUAL, &CUSBHidTest::TC_Consumer_Volume},
};

static SRTestCaseData<CUSBHidTest> g_rgUSBHidPerfTest[] =
{
    {L"Mute",                       L"{075DBBF6-B00C-4C49-8922-4C929FE54C7E}", DEV_OR, 0, &CUSBHidTest::TC_Perf_Mute},
    {L"Volume Up",                  L"{40B0D9FD-D8FA-4F1E-A4D5-60433FC8C4A7}", DEV_OR, 0, &CUSBHidTest::TC_Perf_VolumeUp},
    {L"Volume Down",                L"{8EFC5433-3E82-47A7-A8E8-9389DD810CB7}", DEV_OR, 0, &CUSBHidTest::TC_Perf_VolumeDown}
};

// Create USB HID test group
bool CreateUSBHidTestGroup(ITestRunner *pTestRunner)
{
    bool bSucceeded = true;

    ITestGroup* pCommunicationLogoTestGroup = pTestRunner->CreateGroup( L"Communication Logo Test Group" );
        
    CSRTestGroupHelper<CUSBHidTest> usbHidTestGroup
    (
        DLLGUID,
        pTestRunner, 
        pTestRunner->CreateGroup(L"Hid Test", pCommunicationLogoTestGroup),
        g_rgUSBHidTest,
        sizeof(g_rgUSBHidTest)/sizeof(g_rgUSBHidTest[0]),
        bSucceeded
    );
    
    return bSucceeded;
}

bool CreateUSBHidPerfTestGroup(ITestRunner *pTestRunner)
{
    bool bSucceeded = true;

    CSRTestGroupHelper<CUSBHidTest> usbHidTestGroup
    (
        DLLGUID,
        pTestRunner, 
        pTestRunner->CreateGroup( L"HID Performance Test Group" ),
        g_rgUSBHidPerfTest,
        sizeof(g_rgUSBHidPerfTest)/sizeof(g_rgUSBHidPerfTest[0]),
        bSucceeded
    );
    
    return bSucceeded;
}

bool EnumerateDevices(ITestRunner *pTestRunner)
{
    bool bRes = true;
    
    // get the logger
    CComPtr<IBasicLog> pLog;
    HRESULT hr = pTestRunner->GetBasicLog(&pLog);
    if (FAILED(hr)) return FALSE;
    
    CComPtr<ITestDeviceSet> pBthPlasticSet;
    hr = CreateUSBAudioPlasticSet(pLog, &pBthPlasticSet);
    IF_FAILED_RETURN(pLog, hr, L"Failed CreateBthAudioPlasticSet", false);
    
    UINT cBthDevice = 0;
    hr = pBthPlasticSet->GetCount(&cBthDevice);
    IF_FAILED_RETURN(pLog, hr, L"Failed to get count of audio plastics", false);

    for (UINT idxDevice = 0; idxDevice < cBthDevice; idxDevice++)
    {
        CComPtr<ITestDevice> pTestDevice;
        hr = pBthPlasticSet->GetAt(idxDevice, &pTestDevice);
        IF_FAILED_RETURN(pLog, hr, CFormattedString(L"Failed ITestDeviceSet::GetAt(%d)", idxDevice), false);
        
        // printing device information
        CPtr<WCHAR, CoTaskMemFree> szName;
        hr = pTestDevice->GetName(&szName);
        IF_FAILED_RETURN(pLog, hr, L"Failed CBthPlastic::GetName", false);
        
        CPtr<WCHAR, CoTaskMemFree> szId;
        hr = pTestDevice->GetId(&szId);
        IF_FAILED_RETURN(pLog, hr, L"Failed CBthPlastic::GetId", false);
        
        pLog->Log(XMSG, 5, CFormattedString(L"USB Audio Plastic (%d) Name = %s, Id = %s", idxDevice, szName, szId));
        
        CComPtr<ITestDeviceSet> pDevNodeSet;
        hr = pTestDevice->QueryInterface(__uuidof(ITestDeviceSet), (void**) &pDevNodeSet);
        IF_FAILED_RETURN(pLog, hr, L"Failed ITestDevice::QueryInterface(ITestDeviceSet)", false);
        
        UINT deviceCount = 0;
        hr = pDevNodeSet->GetCount(&deviceCount);
        IF_FAILED_RETURN(pLog, hr, L"Failed to get count of audio devnodes", false);
        
        for (UINT idxNode = 0; idxNode < deviceCount; idxNode++)
        {
            CComPtr<ITestDevice> pTestDevice;
            hr = pDevNodeSet->GetAt(idxNode, &pTestDevice);
            IF_FAILED_RETURN(pLog, hr, CFormattedString(L"Failed ITestDeviceSet::GetAt(%d)", idxNode), false);
            
            // printing device information
            CPtr<WCHAR, CoTaskMemFree> szName;
            hr = pTestDevice->GetName(&szName);
            IF_FAILED_RETURN(pLog, hr, L"Failed ITestDevice::GetName", false);
            
            CPtr<WCHAR, CoTaskMemFree> szId;
            hr = pTestDevice->GetId(&szId);
            IF_FAILED_RETURN(pLog, hr, L"Failed ITestDevice::GetId", false);
            
            pLog->Log(XMSG, 5, CFormattedString(L"    DevNode (%d) Name = %s, Id = %s", idxNode, szName, szId));
        
        }
                
        // adding device to shellrunner
        ISRDevice *pTempSRDev;
        hr = CSRDeviceFromITestDevice::Create<CSRAudioPlastic>(pLog, DLLGUID, pTestDevice, &pTempSRDev);
        IF_FAILED_RETURN(pLog, hr, L"Failed CSRDeviceFromITestDevice::Create<CSRAudioPlastic>", false);
        
        pTestRunner->AddDevice(pTempSRDev, true);
    }
    return bRes;
}

// shellrunner function to initialize test module
HRESULT __stdcall myInitializeTestModule(ITestRunner* pTestRunner)
{
    HRESULT hr = S_OK;
    
    bool bRes = true;

    bRes = CreateUSBHidTestGroup(pTestRunner);
    if (!bRes) { return E_FAIL; }
    
// internal only tests
#ifndef WHQL
    bRes = CreateUSBHidPerfTestGroup(pTestRunner);
    if (!bRes) { return E_FAIL; }
#endif

    if (!pTestRunner->ParseGroups())
    {
        return E_FAIL;
    }

    if (!EnumerateDevices(pTestRunner))
    {
        return E_FAIL;
    }
    return hr;
}
