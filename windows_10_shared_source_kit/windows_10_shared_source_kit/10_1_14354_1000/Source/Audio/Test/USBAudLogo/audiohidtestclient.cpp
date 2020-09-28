// Copyright (C) Microsoft. All rights reserved.
#include "stdafx.h"
#include <common.h>
//#include <utils.h>
//#include <cptr.h>

//#include <HidConsts.h>
//#include <HidCollection.h>
//#include <TestDevices.h>
//#include <TestDevnode.h>
//#include <TestHidDevice.h>
//#include <HidReport.h>
//#include <SRDeviceFromITestDevice.h>
//#include <AudioPlastic.h>

//#include <helperclasses.h>

//#include <TestHidReport.h>
#include <latencyStats.h>
#include <StressSafeFormattedString.h>
#include <EndpointVolume.h>
#include <MMDeviceAPI.h>
#include <AudioHidTestControl.h>

#include <audioclient.h>
#include <volumesetting.h>
#include "audiohidtestclient.h"
using namespace AudioVolume;

// found in mixer.h for sndvol SSO
static const GUID _SystemMixerEventContext = { 0x2c01ab9a, 0x43a2, 0x4aed, { 0xa4, 0x51, 0xe7, 0x0c, 0x4f, 0x97, 0x57, 0x62 } };
// {7047FB02-333A-4d31-811D-93AC429CF3E1} 
static const GUID USBAudioHidTestPerfTestContext = { 0x7047fb02, 0x333a, 0x4d31, { 0x81, 0x1d, 0x93, 0xac, 0x42, 0x9c, 0xf3, 0xe1 } };

class CUnregisterOnExit
{
public:
    CUnregisterOnExit(IBasicLog *pLog, IAudioHidPerfTestClient *pClient)
    {
        // if any input parameter is NULL, the test would AV
        m_pClient = pClient;
        m_pLog = pLog;
    }
    ~CUnregisterOnExit() 
    { 
        HRESULT hr = m_pClient->UnregisterNotification();
        IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioHidPerfTestClient::UnregisterNotification", /*void*/);
    }

private:
    CComPtr<IAudioHidPerfTestClient> m_pClient;
    CComPtr<IBasicLog> m_pLog;
};

class CAudioHidPerfTestClient : public IAudioEndpointVolumeCallback, public IAudioHidPerfTestClient
{
public:
    CAudioHidPerfTestClient(IBasicLog *pLog, CLatencyDataset *pStartSet, CLatencyDataset *pEndSet, HRESULT &hr);
    HRESULT RegisterNotification() override;
    HRESULT UnregisterNotification() override;
    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
    // IUnknown implementation
    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **pp) override;
    HRESULT Test(EHidControl hidCtrl);

private:

    struct AudioVolumeTestData 
    {
        BOOL bMuted;
        float fMasterVolume;
    };

    // cache the data before hid controls were received
    // this allows us to verify the endpoint notification
    // and volume change after test
    AudioVolumeTestData m_dataBeforeTest;
    EHidControl m_currHidCtrlTest;

    // volume data when constructor is called
    // restore when desctructor is called so that the state is not changed after test
    AudioVolumeTestData m_dataAtCtor; 
    bool m_bdataAtCtorStored;
    
    ~CAudioHidPerfTestClient();

    CComPtr<IBasicLog> m_pLog;
    CComPtr<IMMDevice> m_pDevice;
    CComPtr<IAudioEndpointVolume> m_pEptVol;
    LONG m_cRef;
    HANDLE m_hNotified;
    CLatencyDataset *m_pStartSet;
    CLatencyDataset *m_pEndSet;
};

CAudioHidPerfTestClient::CAudioHidPerfTestClient
(
    IBasicLog *pLog, 
    CLatencyDataset *pStartSet, 
    CLatencyDataset *pEndSet, 
    HRESULT &hr
)
: m_cRef(1)
, m_pLog(NULL)
, m_pStartSet(pStartSet)
, m_pEndSet(pEndSet)
, m_hNotified(NULL)
, m_currHidCtrlTest(EHidControl_Undefined)
, m_bdataAtCtorStored(false)
{
    if (FAILED(hr)) { return; }
    if (NULL == pLog || NULL == pStartSet || NULL == pEndSet) { hr = E_POINTER; return; }
    
    m_pLog = pLog;

    CComPtr<IMMDeviceEnumerator> pEnumerator;
    hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
    IF_FAILED_RETURN(m_pLog, hr, L"Failed CoCreateInstance(MMDeviceEnumerator)", /*void*/);

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_pDevice);
    IF_FAILED_RETURN(m_pLog, hr, L"Failed IMMDeviceEnumerator::GetDefaultAudioEndpoint(eRender, eConsole)", /*void*/);

    hr = m_pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&m_pEptVol);
    IF_FAILED_RETURN(m_pLog, hr, L"Failed IMMDevice::Activate(IAudioEndpointVolume)", /*void*/);

    m_hNotified = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == m_hNotified)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        IF_FAILED_RETURN(m_pLog, hr, L"Failed CreateEvent", /*void*/);
    }

    // cache the endpoint volume setting and restore them at destructor
    hr = m_pEptVol->GetMute(&m_dataAtCtor.bMuted);
    IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioEndpointVolume::GetMute: cache mute state in ctor", /*void*/);

    hr = m_pEptVol->GetMasterVolumeLevelScalar(&m_dataAtCtor.fMasterVolume);
    IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioEndpointVolume::GetMasterVolumeLevelScalar: cache volume level in ctor", /*void*/);

    m_bdataAtCtorStored = true;
}

CAudioHidPerfTestClient::~CAudioHidPerfTestClient()
{
    if (NULL != m_hNotified)
    {
        CloseHandle(m_hNotified);
    }

    if (m_bdataAtCtorStored)
    {
        // restore the endpoint volume setting
        HRESULT hr = m_pEptVol->SetMute(m_dataAtCtor.bMuted, &USBAudioHidTestPerfTestContext);
        IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioEndpointVolume::SetMute: restore mute state", /*void*/);
        ASSERT(SUCCEEDED(hr));

        hr = m_pEptVol->SetMasterVolumeLevelScalar(m_dataAtCtor.fMasterVolume, &USBAudioHidTestPerfTestContext);
        IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioEndpointVolume::SetMasterVolumeLevelScalar: restore volume level", /*void*/);
        ASSERT(SUCCEEDED(hr));
    }
}

HRESULT CAudioHidPerfTestClient::RegisterNotification()
{
    HRESULT hr = m_pEptVol->RegisterControlChangeNotify(this);
    IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioEndpointVolume::RegisterControlChangeNotify", hr);

    return S_OK;
}

HRESULT CAudioHidPerfTestClient::UnregisterNotification()
{
    HRESULT hr = m_pEptVol->UnregisterControlChangeNotify(this);
    IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioEndpointVolume::UnregisterControlChangeNotify", hr);
    
    return S_OK;
}

HRESULT CAudioHidPerfTestClient::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify)
{
    // make sure this is the notification we expected
    // compare the current state with what was before the test started
    bool bExpectedNotification = false;
    
    if (NULL == pNotify) { return E_POINTER; }

    if (pNotify->guidEventContext != _SystemMixerEventContext)
    {
        m_pLog->Log(XWARN, 1, L"IAudioEndpointVolumeCallback::OnNotify received notificaiton of non-system guid. Another client is changing volume on the system.");
        return S_OK;
    }

    m_pLog->Log(XMSG, 10, FormatStr(L"Volume control on notify: Mute(%s), Volume(%0.2f)", 
        pNotify->bMuted?L"true":L"false",
        pNotify->fMasterVolume));
        
    switch (m_currHidCtrlTest)
    {
    case EHidControl_Mute:
        // make sure our mute state changed and volume did not change
        bExpectedNotification = (pNotify->bMuted != m_dataBeforeTest.bMuted) 
                                && (pNotify->fMasterVolume == m_dataBeforeTest.fMasterVolume);
        break;
    case EHidControl_VolUp:
        // make sure endpoint is unmuted (volume change through HID unmutes endpoint) and volume went up
        bExpectedNotification = !pNotify->bMuted && (pNotify->fMasterVolume > m_dataBeforeTest.fMasterVolume);
        break;
    case EHidControl_VolDown:
        // make sure endpoint is unmuted (volume change through HID unmutes endpoint) and volume went down
        bExpectedNotification = !pNotify->bMuted && (pNotify->fMasterVolume < m_dataBeforeTest.fMasterVolume);
        break;
    case EHidControl_Undefined:
        return S_OK; // no-op, this notification is received before our test starts
    default:
        return E_UNEXPECTED;
    }

    if (bExpectedNotification)
    {
        m_pLog->Log(XMSG, 10, L"Expected Notification Received...");

        // update the dataset for the timestamp of the notification
        HRESULT hr = m_pEndSet->Add();
        IF_FAILED_RETURN(m_pLog, hr, L"Failed CLatencyDataset::Add: adding new end time (last received IAudioEndpointVolume notification)", hr);
        
        // notify the client we've got the event
        BOOL bRes = SetEvent(m_hNotified);
        if (FALSE == bRes)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            IF_FAILED_RETURN(m_pLog, hr, L"Failed SetEvent: notification received from IAudioEndpointVolume", hr);
        }
    }
    else
    {
        m_pLog->Log(XFAIL, 1, L"IAudioEndpointVolumeCallback::OnNotify received notificaiton on unexpected volume change.");
        return E_UNEXPECTED;
    }
    return S_OK;
}

    // IUnknown implementation
ULONG CAudioHidPerfTestClient::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

ULONG CAudioHidPerfTestClient::Release()
{
    LONG retval = InterlockedDecrement(&m_cRef);

    if (0 >= retval) 
    {
        delete this;
    }
    return retval;
}

HRESULT CAudioHidPerfTestClient::QueryInterface(REFIID riid, void **pp)
{
    if (NULL == pp) return E_POINTER;
    *pp = NULL;
    if (__uuidof(IUnknown) == riid || 
        __uuidof(IAudioEndpointVolumeCallback) == riid)
    {
        *pp = (IAudioEndpointVolumeCallback*)this;
        AddRef();
        return S_OK;
    }
    else if (__uuidof(IAudioHidPerfTestClient) == riid)
    {
        *pp = (IAudioHidPerfTestClient*)this;
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

HRESULT CAudioHidPerfTestClient::Test(EHidControl hidCtrl)
{
    HRESULT hr = S_OK;

    // cache the volume setting for this test
    // restores the current setting when leaving the function
    CEndpointVolumeSetting volSet(m_pEptVol, hr);
    IF_FAILED_RETURN(m_pLog, hr, L"Failed CEndpointVolumeSetting::CEndpointVolumeSetting: cache endpoint volume setting", hr);

    // set existing volume to mid-way so that we have room to do volume up and down
    hr = m_pEptVol->SetMasterVolumeLevelScalar(0.5f, &USBAudioHidTestPerfTestContext);
    IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioEndpointVolume::SetMasterVolumeLevelScalar(0.5): preset volume level to mid-level", hr);

    // cache the test info so that OnNotify can verify the the data it receives
    m_currHidCtrlTest = hidCtrl;
    hr = m_pEptVol->GetMute(&m_dataBeforeTest.bMuted);
    IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioEndpointVolume::GetMute: cache mute state", hr);
    hr = m_pEptVol->GetMasterVolumeLevelScalar(&m_dataBeforeTest.fMasterVolume);
    IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioEndpointVolume::GetMasterVolumeLevelScalar: cache volume level", hr);

    m_pLog->Log(XMSG, 10, FormatStr(L"Volume setting at test start: Mute(%s), Volume(%0.2f)", 
        m_dataBeforeTest.bMuted?L"true":L"false",
        m_dataBeforeTest.fMasterVolume));
    
    // Notification is async, let's wait for a bit for the above set volume to act before we register for another notification
    Sleep(1000);

    // get an IAudioHidTestControl
    CComPtr<IAudioHidTestControl> pAudioHidTestControl;
    hr = CreateAudioHidTestControl(m_pLog, &pAudioHidTestControl);
    IF_FAILED_RETURN(m_pLog, hr, L"Failed CreateAudioHidTestControl", hr);

    // register for volume change notification
    hr = RegisterNotification();
    IF_FAILED_RETURN(m_pLog, hr, L"Failed RegisterNotification", hr);
    // raii to unregister the notification for endpoint volume change
    CUnregisterOnExit unregister(m_pLog, (IAudioHidPerfTestClient*)this);

    // get a starting value before we send the HID report to generate a test
    LARGE_INTEGER qpcStart = {0};
    
    BOOL bOK = QueryPerformanceCounter(&qpcStart);
    if (!bOK) 
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        IF_FAILED_RETURN(m_pLog, hr, L"Failed QueryPerformanceCounter: query start time", hr);
    }
    m_pLog->Log(XMSG, 10, FormatStr(L"QueryPerformanceCounter at start: %I64d", qpcStart.QuadPart));

    m_pLog->Log(XMSG, 10, L"Writing output report for test..."); 
    switch (hidCtrl)
    {
    case EHidControl_Mute:
        hr = pAudioHidTestControl->tapMute();
        IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioHidtestControl::tapMute", hr);
        hr = pAudioHidTestControl->updateQPC();
        IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioHidtestControl::updateQPC", hr);
        break;
    case EHidControl_VolUp:
        hr = pAudioHidTestControl->tapVolumeUp();
        IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioHidtestControl::tapVolumeUp", hr);
        hr = pAudioHidTestControl->updateQPC();
        IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioHidtestControl::updateQPC", hr);
        break;

    case EHidControl_VolDown:
        hr = pAudioHidTestControl->tapVolumeDown();
        IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioHidtestControl::tapVolumeDown", hr);
        hr = pAudioHidTestControl->updateQPC();
        IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioHidtestControl::updateQPC", hr);
        break;
    default:
        return E_UNEXPECTED;
    }
    
    // wait for we receive the endpoint notification
    // we assume no other volume change or mute state change is happening
    // except for this test
    m_pLog->Log(XMSG, 10, L"Waiting for IAudioEndpointVolume notification..."); 
    DWORD dwRes = WaitForSingleObject(m_hNotified, 10000);
    if (WAIT_OBJECT_0 != dwRes)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        m_pLog->Log(XFAIL, 1, FormatStr(L"Failed WaitForSingleObject(returned %d): waiting for last IAudioEndpointVolumeCallback::OnNotify call (hr = %s)", dwRes, GetHRString(hr)));
        return E_FAIL;
    }

    // we now check to see find the report that has the start qpc info
    // by now we should have that report in the hid ring buffer already
    m_pLog->Log(XMSG, 10, L"Reading input report from test driver..."); 
    UINT cLoops = 0;
    while (true)
    {
        bool bVolumeUp, bVolumeDown;
        LARGE_INTEGER qpcInput;
        hr = pAudioHidTestControl->readVolumeUp(&bVolumeUp);
        IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioHidTestControl::readVolumeUp", hr);
        
        hr = pAudioHidTestControl->readVolumeDown(&bVolumeDown);
        IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioHidTestControl::readVolumeDown", hr);
        
        hr = pAudioHidTestControl->readQPC(&qpcInput);
        IF_FAILED_RETURN(m_pLog, hr, L"Failed IAudioHidTestControl::readQPC", hr);
        
        m_pLog->Log(XMSG, 10, FormatStr(L"Volume control at input: VolumeUp(%s), VolumeDown(%s)", 
            bVolumeUp?L"true":L"false",
            bVolumeDown?L"true":L"false"));
        m_pLog->Log(XMSG, 10, FormatStr(L"QueryPerformanceCounter at input: %I64d", qpcInput.QuadPart));
            
        // we haven't seen the QPC we triggered yet...
        // or more accurately,
        // we haven't seen any QPCs yet
        // which happened after we triggered one
        if (qpcStart.QuadPart - qpcInput.QuadPart <= 0) 
        {
            // make sure the right control is reported
            // ylsuen-2009/02/19: Uncomment the following when 524235 is fixed
            /*
            bool bReportExpected = true;
            // uncomment the following code once 524235 is fixed
            switch (hidCtrl)
            {
            case EHidControl_Mute:
                bReportExpected = bMuteRead && !bVolumeUp && !bVolumeDown;
                break;
            case EHidControl_VolUp:
                bReportExpected = !bMuteRead && bVolumeUp && !bVolumeDown;
                break;
            case EHidControl_VolDown:
                bReportExpected = !bMuteRead && !bVolumeUp && bVolumeDown;
                break;
            default:
                return E_UNEXPECTED;
            }
            if (bReportExpected)
            {
            */
            hr = m_pStartSet->Add(qpcInput);
            IF_FAILED_RETURN(m_pLog, hr, L"Failed CLatencyDataset::Add: adding new end time (last received IAudioEndpointVolume notification)", hr);
            m_pLog->Log(XMSG, 10, L"Expected Input Report Received...");
            break;
            /*
            } 
            else
            {
                m_pLog->Log(XFAIL, 1, L"Read input report on unexpected volume change.");
                return E_UNEXPECTED;
            }
            */
        }
        if (++cLoops == 10) { return E_FAIL; }
        Sleep(50);
    }

    // reset the test data
    m_currHidCtrlTest = EHidControl_Undefined;
    return S_OK;
}

HRESULT CreateAudioHidPerfTestClient
(
    IBasicLog *pLog, 
    CLatencyDataset *pStartSet, 
    CLatencyDataset *pEndSet, 
    IAudioHidPerfTestClient **ppClient
)
{
    if (NULL == ppClient)
    {
        return E_POINTER;
    }
    *ppClient = NULL;
    
    HRESULT hr = S_OK;
    CAudioHidPerfTestClient *pClient = new CAudioHidPerfTestClient(pLog, pStartSet, pEndSet, hr);
    if (NULL == pClient) { return E_OUTOFMEMORY; }
    if (FAILED(hr))
    {
        pClient->Release();
        return hr;
    }
    
    *ppClient = pClient;
    
    return S_OK;
}
