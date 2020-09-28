// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// Module Name:
//
//  tcommon.cpp
//
// Abstract:
//
//  Implementation file for CKsPosTst::TestCommon
//




// -------------------------------------------------------------------------------

#include "PreComp.h"
#include <setupapi.h>

#include <DevTrans.h>
#include <CPropvariant.h>
#include <avendpointkeys.h>

using namespace DevTrans;
using namespace AudioDevNode;

#define S_W static_cast<LPCWSTR>

extern USHORT ChannelsFromMask(DWORD);

using namespace WEX::Logging;
using namespace WEX::TestExecution;

WORD BitDepthFromMask(DWORD dw)
{
    switch(dw & FMTM_BITDEPTH)
    {
        case FMT_8BIT    : return 8;
        case FMT_16BIT   : return 16;
        case FMT_20BIT   : return 20;
        case FMT_24BIT   : return 24;
        case FMT_32BIT   : return 32;
    }

    return 0;
}

ULONG SampleRateFromMask(DWORD dw)
{
    switch (dw & FMTM_SAMPLERATE)
    {
        case FMT_8000HZ  : return  8000;
        case FMT_11025HZ : return 11025;
        case FMT_16000HZ : return 16000;
        case FMT_22050HZ : return 22050;
        case FMT_32000HZ : return 32000;
        case FMT_44100HZ : return 44100;
        case FMT_48000HZ : return 48000;
    }
    return 0;
}

ULONG SpeakerMaskFromMask(DWORD dw)
{
    switch (dw & FMTM_SPEAKCONFIG)
    {
        case FMT_MONO    :  return KSAUDIO_SPEAKER_MONO;
        case FMT_STEREO  :  return KSAUDIO_SPEAKER_STEREO;
        case FMT_QUAD    :  return KSAUDIO_SPEAKER_QUAD;
        case FMT_5_1     :  return KSAUDIO_SPEAKER_5POINT1;
    }
    return 0;
}

// ----------------------------------------------------------------------------------
// BitsSetInDWORD    - Finding no of Bits set into DWORD
// ----------------------------------------------------------------------------------
USHORT
BitsSetInDWORD
(
    DWORD   dw
)
{
    USHORT u;
    USHORT c = 0;

    for(u = 0; u < 32; u++)
    {
        if(dw & 0x00000001)
            c++;

        dw >>= 1;
    }

    return c;
}

BOOL 
InstantiatePinWfx
(
    CPCMAudioPin   *pPin,
    WAVEFORMATEX   *pWfx,
    BOOL            fLooped,
    OPTIONAL BOOL   bRawSupport,
    OPTIONAL BOOL   bDefaultSupport
)
{
    GUID mode =
    (
        bRawSupport     ? AUDIO_SIGNALPROCESSINGMODE_RAW     :
        bDefaultSupport ? AUDIO_SIGNALPROCESSINGMODE_DEFAULT :
        KSLIB_AUDIOPROCESSINGMODE_AUTO
    );

    if (pWfx)
    {
        // we've been handed a format; use that
        if(!pPin->SetFormat(pWfx, mode))
        {
            XLOG(XMSG, XFAIL, "Setting the specified format on the pin failed");
            return FALSE;
        }
    }
    else
    {
        // let the pin choose the format
        if(!pPin->SetViablePCMWfxt(NULL, mode))
        {
            XLOG(XFAIL, eError, "Unable to find a viable format");
            return FALSE;
        }
    }


    XLOG(XMSG, eInfo1, "Wave Format used for instantiation:");
    CComPtr<IBasicLog> pShellBasicLog;

    LogWaveFormat(g_pBasicLog, pPin->GetFormat());

    return pPin->Instantiate(fLooped);
}

// ----------------------------------------------------------------------------------
// PreparePin
// ----------------------------------------------------------------------------------
BOOL
CHalfApp::PreparePin
(
    WAVEFORMATEX *pWfxIn,
    BOOL fLooped,
    OPTIONAL ULONG ulBufferTime,
    OPTIONAL BOOL bRawSupport,
    OPTIONAL ULONG ulNotificationCount
)
{
    CComPtr<IBasicLog> pBasicLog;
    HRESULT hr = S_OK;
    
    // make sure we have enough parts to make some noise
    if (NULL == m_pPin)
    {
        SLOG(eWarn1, "PreparePin: Pin passed in is NULL");
        goto cleanup;
    }

    if (!InstantiatePinWfx (m_pPin, pWfxIn, fLooped, bRawSupport, false))
    {
        SLOG(eWarn1, "PreparePin: Unable to instantiate pin");
        goto cleanup;
    }

    // Does the pin support KSPROPERTY_RTAUDIO_GETREADPACKET?
    if (IsWaveRTPin())
    {
        DWORD dwSupport;
        if (m_pPin->PropertyBasicSupport(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_GETREADPACKET, &dwSupport))
        {
            m_fIsWaveRTPinWithReadPacketSupport = ((dwSupport && KSPROPERTY_TYPE_GET) != 0);
        }
        else
        {
            // Ensure correct error codes
            VERIFY_IS_TRUE((GetLastError() == ERROR_SET_NOT_FOUND) || (GetLastError() == ERROR_NOT_FOUND));
        }
    }
    
    // Does the pin support KSPROPERTY_RTAUDIO_SETWRITEPACKET?
    if (IsWaveRTPin())
    {
        DWORD dwSupport;
        if (m_pPin->PropertyBasicSupport(KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_SETWRITEPACKET, &dwSupport))
        {
            m_fIsWaveRTPinWithWritePacketSupport = ((dwSupport && KSPROPERTY_TYPE_SET) != 0);
        }
        else
        {
            // Ensure correct error codes
            VERIFY_IS_TRUE((GetLastError() == ERROR_SET_NOT_FOUND) || (GetLastError() == ERROR_NOT_FOUND));
        }
    }

    if (!m_pPin->CreateDataBuffer(ulBufferTime, pWfxIn->nBlockAlign, 0, ulNotificationCount))
    {
        SLOG(eWarn1, "PreparePin: Unable to create a data buffer");
        goto cleanup;
    }

    if (m_flow == render)
    {

        pBasicLog = g_pBasicLog;

        hr = FillBufferWithSineSignal(
            pBasicLog, XFAIL,
            pWfxIn,
            1.0,
            200.0,
            0.0, // initial phase,
            0.0, // dc
            Method_NoDithering,
            m_pPin->m_pbStreamData,
            m_pPin->m_cbStreamData / pWfxIn->nBlockAlign,
            m_pPin->m_cbStreamData
        );

        if (S_OK != hr)
        {
            XLOG(XFAIL,
                eError,
                "FAIL: Error filling buffer with sine: 0x%08x", hr);
            goto cleanup;
        }
    }
    
    return TRUE;

cleanup:
    if (NULL != m_pPin)
    {
        m_pPin->SetState(KSSTATE_STOP);
        m_pPin->ClosePin();
    }
    return FALSE;
}

BOOL CHalfApp::PreparePinConsiderUSB(WAVEFORMATEX *pwfx, BOOL fLooped, BOOL bRawSupport)
{
    if (IsUSBPin())    
    {
        SLOG(eInfo2, "Device is USB, preparing pin with %d(ms) buffer", USB_BUFLEN_MS);
        //USB buffer must be less than 250 ms per buffer

        return PreparePin(pwfx, fLooped, USB_BUFLEN_MS, bRawSupport);
    }
    else
    {
        return PreparePin(pwfx, fLooped, BUFLEN_MS, bRawSupport);
    }
}

bool CHalfApp::IsUSBPin() 
{ 
    if (NULL != m_pFilter)
    {
        return (0 == _tcsnicmp(m_pFilter->m_szHardwareID, _T("USB"), 3));
    }
    return false;
}

bool CHalfApp::IsKWDPin()
{
    if (NULL != m_pFilter)
    {
        GUID pinCategory;

        if (m_pFilter->GetPinPropertySimple(m_pPin->m_nId, KSPROPSETID_Pin, KSPROPERTY_PIN_CATEGORY, &pinCategory, sizeof(pinCategory)))
        {
            const CComBSTR guidBstr(pinCategory);
            XLOG(XMSG, eInfo1, "pin category is %S", guidBstr);

            if (pinCategory == KSNODETYPE_AUDIO_KEYWORDDETECTOR)
            {
                return true;
            }
        }
    }
    return false;
}

HRESULT CHalfApp::FindInstanceIDInDeviceSet(ITestDeviceSet *pDeviceSet, LPCWSTR szInstanceID, bool &fFound, bool &isBth)
{
    HRESULT hr = S_OK;
    if (NULL == pDeviceSet)
    {
        return E_POINTER;
    }

    UINT cDevices = 0;
    hr = pDeviceSet->GetCount(&cDevices);
    if (FAILED(hr)) { return hr; }

    fFound = false;
    isBth = false;
    for (DWORD dwIdx = 0; dwIdx < cDevices; dwIdx++)
    {
        CComPtr<ITestDevice> pDevice;
        hr = pDeviceSet->GetAt(dwIdx, &pDevice);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "Failed getting device at index %d (hr = %s)", dwIdx, GetHRString(hr));
            return hr;
        }

        CComHeapPtr<WCHAR> szCurrentInstanceID;
        hr = pDevice->GetId(&szCurrentInstanceID);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "Failed getting device ID at index %d (hr = %s)", dwIdx, GetHRString(hr));
            return hr;
        }

        if(0 == wcscmp(szCurrentInstanceID, szInstanceID))
        {
            fFound = true;

            // Determine if device is Bluetooth...
            CComPtr<ITestDevNode> pAudioDevNode;
            hr = pDevice->QueryInterface(__uuidof(ITestDevNode), (void**)&pAudioDevNode);
            if (FAILED(hr))
            {
                XLOG(XFAIL, eError, "Failed QIing for AudioDevice with hr=0x%08x", hr);
                return hr;
            }

            GUID guidAudioDevNodeType;
            hr = pAudioDevNode->GetType(&guidAudioDevNodeType);
            if (FAILED(hr))
            {
                XLOG(XFAIL, eError, "Failed Getting DevNode type with hr=0x%08x", hr);
                return hr;
            }

            if (GUID_AUDDEVNODE_BLUETOOTH == guidAudioDevNodeType)
            {
                XLOG(XMSG, eBlab2, "The Bluetooth device: \"%s\" is found", szInstanceID);
                isBth = true;
            }

            break;
        }
    }
    return S_OK;
}

// Get the collection of endpoints associated with the current device (pin) under test
HRESULT CHalfApp::GetCurrentEndpointsUnderTest(IMMDeviceCollection **ppEndpointCollection)
{
    HRESULT hr = S_OK;

    if (NULL == ppEndpointCollection) { return E_POINTER; }

    CPCMAudioPin *pPin = m_pPin; // current pin under test
    CKsFilter *pFilter = m_pFilter; // current filter under test
    CComPtr<IMMDevice> pDevInterface;
    
    hr = DevicePathToInterfaceW(pFilter->m_szFilterName, &pDevInterface);
    if (FAILED(hr))
    {
        SLOG(eWarn1, "Failed to get Interface from device path %s (hr = 0x%08x)", pFilter->m_szFilterName, hr);
        return hr;
    }

    // - We should NOT enumerate endpoints that are removed, i.e. DEVICE_STATE_NOTPRESENT. Those are not valid for testing.
    // - We should enumerate disabled devices such as Stereo Mix, CD In, Aux, etc.
    // - We theoretically should not see unplugged endpoint on the system because wave test would have failed earlier before
    // loading a test case. But it doesn't hurt to add it here.
    // - We are always interested in active endpoints. 
    hr = KSStreamPinToEndpoints(pDevInterface, pPin->m_nId, DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED | DEVICE_STATE_DISABLED, ppEndpointCollection);
    if (FAILED(hr))
    {
        XLOG(XWARN, eError, "Failed to get endpoints from pin %d on device path %s (hr = 0x%08x)", pPin->m_nId, pFilter->m_szFilterName, hr);
        return hr;
    }

    return S_OK;
}

bool CHalfApp::IsWaveRTPacketStreamingSupported()
{
    return (m_flow == render) ? m_fIsWaveRTPinWithWritePacketSupport : m_fIsWaveRTPinWithReadPacketSupport;
}

bool CHalfApp::IsBTHPin()
{
    HRESULT hr;
    bool bBTHFound = false;
    UINT cEndpoints = 0;
    CComPtr<IMMDeviceCollection> pEndpointCollection;
    CComPtr<IBasicLog> pLog = g_pBasicLog;

    // Find endpoint collection related to current testing pin
    hr = GetCurrentEndpointsUnderTest(&pEndpointCollection);
    if (FAILED(hr))
    {
        return bBTHFound;
    }

    // Get the count of endpoint collection
    hr = pEndpointCollection->GetCount(&cEndpoints);
    if (FAILED(hr))
    {
        return bBTHFound;
    }

    XLOG(XMSG, eInfo1, "There is a total of %d endpoints for the current pin", cEndpoints);
    if (0 == cEndpoints)
    {
        // Didn't find any endpoint. Just exit.
        return bBTHFound;
    }

    for (UINT idxEP = 0; idxEP < cEndpoints; idxEP++)
    {
        CComPtr<IMMDevice> pEndpoint;

        hr = pEndpointCollection->Item(idxEP, &pEndpoint);
        if (FAILED(hr))
        {
            XLOG(XMSG, eInfo1, "Failed to get endpoint at index %d", idxEP);
            continue;
        }

        CComHeapPtr<WCHAR> szId; // use the ID for logging purpose
        hr = pEndpoint->GetId(&szId);
        if (FAILED(hr))
        {
            XLOG(XMSG, eInfo1, "Failed to get endpoint id at index %d", idxEP);
            continue;
        }

        XLOG(XMSG, eInfo2, "Audio endpoint device under test has ID: %ls", S_W(szId));

        CComPtr<IPropertyStore> pPropertyStore;
        hr = pEndpoint->OpenPropertyStore(STGM_READ, &pPropertyStore);
        if (FAILED(hr))
        {
            XLOG(XMSG, eInfo1, "Failed to open property store on endpoint (%ls) to read (IMMDevice::OpenPropertyStore) (hr = %s)", S_W(szId), GetHRString(hr));
            continue;
        }

        CPropVariant varIsBTH;
        hr = pPropertyStore->GetValue(PKEY_Endpoint_IsBluetooth, &varIsBTH);
        if (FAILED(hr))
        {
            XLOG(XMSG, eInfo1, "Failed to get PKEY_Endpoint_IsBluetooth on endpoint (%ls) (IPropertyStore::GetValue) (hr = %s)", S_W(szId), GetHRString(hr));
            continue;
        }

        if (VT_EMPTY == varIsBTH.vt)
        {
            XLOG(XMSG, eInfo1, "PKEY_Endpoint_IsBluetooth not defined on endpoint (%ls).", S_W(szId));
            continue;
        }

        bBTHFound = (varIsBTH.ulVal ? true : false);
        if (true == bBTHFound)
        {
            XLOG(XMSG, eInfo1, "Current device is bluetooth.");
            return bBTHFound;
        }
    }

    return bBTHFound;
}

bool CHalfApp::IsTestSupportedForDeviceType(BOOL fTestingLoopedDevice) 
{ 
    return 
    // If the current test is for looped streaming device 
    // and the current device supports looped streaming;
        ((fTestingLoopedDevice && m_fIsStdLoopedStrmPin)
    // or, if the current test is for standard streaming device 
    // and the current device supports standard streaming.
        || (!fTestingLoopedDevice && m_fIsStdStrmPin));  
}

OACR_WARNING_POP
