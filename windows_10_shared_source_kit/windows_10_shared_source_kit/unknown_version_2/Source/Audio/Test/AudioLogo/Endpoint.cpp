//--------------------------------------------------------------------------
//
//  File: Endpoint.cpp
//
//  Copyright (C) Microsoft. All rights reserved.
//
//  Abstract:
//      Verifies endpoint availability.
//
//  Contents:
//      TC_VerifyAllEndpointsAvailable()
//      TC_VerifyMicArrayRawFormat()
//
//--------------------------------------------------------------------------
#include "Common.h"
#include <initguid.h>
#include <MMDeviceAPIP.h>
#include <DeviceTopology.h>
#include <DeviceTopologyP.h>
#include <AudioStreaming.h>
#include <MMSystem.h>
#include <DevTrans.h>
#include <CPropvariant.h>
#include <HRString.h>
#include "DetectBluetooth.h"

using namespace DevTrans;

HRESULT MixFormat(IMMDevice *pMMDevice, bool raw, WAVEFORMATEX **ppWfx);

//--------------------------------------------------------------------------;
//
//  DWORD TC_VerifyAllEndpointsAvailable
//
//  Description:
//      TestCase: Verify all endpoints are available for testing.  No
//          unplugged endpoints.
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Unused.  Failures propagated through XLOG.
//
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_VerifyAllEndpointsAvailable()
{
    CComPtr<IMMDeviceEnumerator>    pDevEnum = NULL;
    CComPtr<IMMDeviceCollection>    pEndpoints = NULL;
    UINT                            iEndpoint;
    HRESULT                         hr;

    if (S_OK != (hr = pDevEnum.CoCreateInstance(__uuidof(MMDeviceEnumerator))))
    {
        XLOG(
                    XFAIL,
                    eError,
                    L"FAIL: CoCreateInstance(IMMDeviceEnumerator) returned "
                        L"error (0x%08lx).",
                    hr);

        return (FNS_PASS);
    }

    //  Enumerating all endpoints...
    hr = pDevEnum->EnumAudioEndpoints(eAll, DEVICE_STATEMASK_ALL, &pEndpoints);

    if (S_OK != hr)
    {
        XLOG(
                    XFAIL,
                    eError,
                    L"FAIL: EnumAudioEndpoints returned error (0x%08lx).",
                    hr);

        return (FNS_PASS);
    }

    if (S_OK != (hr = pEndpoints->GetCount(&iEndpoint)))
    {
        XLOG(
                    XFAIL,
                    eError,
                    L"FAIL: IMMDeviceCollection::GetCount() returned "
                        L"error (0x%08lx).",
                    hr);

        return (FNS_PASS);
    }

    XLOG(XMSG, eInfo2, L"Enumerating endpoints...");

    for (; iEndpoint; iEndpoint--)
    {
        CComPtr<IMMDevice>      pIEndpoint = NULL;
        CComHeapPtr<WCHAR>      pszName;
        CComHeapPtr<WCHAR>      pszDesc;
        DWORD                   dwState;

        if (S_OK != (hr = pEndpoints->Item(iEndpoint - 1, &pIEndpoint)))
        {
            XLOG(
                        XFAIL,
                        eError,
                        L"FAIL: IMMDeviceCollection::Item() returned "
                            L"error (0x%08lx).",
                        hr);

            continue;
        }

        //  Get Display name and description...
        if (S_OK != (hr = GetEndpointDisplayNameW(&pszName, pIEndpoint)))
        {
            XLOG(
                        XFAIL,
                        eError,
                        L"FAIL: GetEndpointDisplayName returned error (0x%08lx).",
                        hr);

            continue;
        }

        if (S_OK != (hr = GetEndpointDescriptionW(&pszDesc, pIEndpoint)))
        {
            XLOG(
                        XFAIL,
                        eError,
                        L"FAIL: GetEndpointDescription returned error (0x%08lx).",
                        hr);

            continue;
        }

        if (S_OK != (hr = pIEndpoint->GetState(&dwState)))
        {
            XLOG(
                        XFAIL,
                        eError,
                        L"FAIL: IMMDevice::GetState returned error (0x%08lx).",
                        hr);

            continue;
        }

        XLOG(
                    XMSG,
                    eInfo2,
                    L"  Endpoint (%ls (%ls)) has state of 0x%08lx.",
                        (LPCWSTR)pszDesc,
                        (LPCWSTR)pszName,
                        dwState);

        if (0 != (dwState & DEVICE_STATE_ACTIVE))
        {
            XLOG(
                        XMSG,
                        eInfo2,
                        L"   State: DEVICE_STATE_ACTIVE (0x%08lx)",
                        DEVICE_STATE_ACTIVE);
        }

        if (0 != (dwState & DEVICE_STATE_DISABLED))
        {
            XLOG(
                        XMSG,
                        eInfo2,
                        L"   State: DEVICE_STATE_DISABLED (0x%08lx)",
                        DEVICE_STATE_DISABLED);
        }

        if (0 != (dwState & DEVICE_STATE_NOTPRESENT))
        {
            XLOG(
                        XMSG,
                        eInfo2,
                        L"   State: DEVICE_STATE_NOTPRESENT (0x%08lx)",
                        DEVICE_STATE_NOTPRESENT);
        }

        //  Check for unplugged...
        if (0 != (dwState & DEVICE_STATE_UNPLUGGED))
        {
            XLOG(
                        XMSG,
                        eInfo2,
                        L"   State: DEVICE_STATE_UNPLUGGED (0x%08lx)",
                        DEVICE_STATE_UNPLUGGED);

            XLOG(
                        XFAIL,
                        eError,
                        L"    FAIL: Endpoint (%ls (%ls)) is unplugged (0x%08lx)",
                            (LPCWSTR)pszDesc,
                            (LPCWSTR)pszName,
                            DEVICE_STATE_UNPLUGGED);
        }
    }

    return (FNS_PASS);
}   //   TC_VerifyAllEndpointsAvailable()

DWORD WINAPI TC_Verify_BTH()
{
    //this should only run in the system tests
#ifndef BUILD_TAEF
    if(NULL != g_pAudioLogoDeviceTestModule->m_pShell->m_pszHwIdTestDeviceSelect &&
        0 != lstrcmp(L"", g_pAudioLogoDeviceTestModule->m_pShell->m_pszHwIdTestDeviceSelect))
#else
    if (NULL != g_IShell->m_pszHwIdTestDeviceSelect &&
        0 != lstrcmp(L"", g_IShell->m_pszHwIdTestDeviceSelect))
#endif
    {
        SKIP(L"Bluetooth presence test is run only in the system kit. Skipping...");
        return FNS_SKIPPED;
    }

    //
    // This test only applies to Windows 6.3 and above
    //
    OSVERSIONINFO ver = {0};
    ver.dwOSVersionInfoSize = sizeof(ver);
    if (!GetVersionEx(&ver))
    {
        ERR(L"FAIL: GetVersionEx() failed: GetLastError() = %u", GetLastError());
        return (FNS_PASS);
    }

    LOG(L"INFO: OS version is %u.%u", ver.dwMajorVersion, ver.dwMinorVersion);

    if (ver.dwMajorVersion < 6 || (ver.dwMajorVersion == 6 && ver.dwMinorVersion < 3))
    {
        XLOG(XSKIP, eInfo1, L"INFO: This test case is only applicable to Windows versions 6.3 and above.");
        return (FNS_PASS);
    }

    BOOL bBthRunnging = FALSE;
    UINT cDevices = 0;

    CComPtr<IBasicLog> pLogger;
    HRESULT hr = g_IShell->GetBasicLog(&pLogger);
    if(FAILED(hr))
    {
        ERR(L"FAIL: Unable to create IBasicLog!.");
        return FNS_PASS;
    }

    //see if there is a bth service running
    hr = DetectBluetooth::IsBthServiceRunning(pLogger, bBthRunnging);
    if(FAILED(hr))
    {
        ERR(L"FAIL: Unable to Query Bth Services (0x%08lx).", hr);
        return FNS_PASS;
    }

    if (FALSE == bBthRunnging)
    {
        XLOG(XSKIP, eError, L"Bluetooth is not running on the system");
        return FNS_SKIPPED;
    }

    //see how many devices are present
    hr = DetectBluetooth::BluetoothAudioDeviceCount(pLogger, &cDevices);
    if(FAILED(hr))
    {
        ERR(L"FAIL: Unable to Query Bth device count (0x%08lx).", hr);
        return FNS_PASS;
    }

    if (0 == cDevices)
    {
        ERR(L"FAIL: BLuetooth is present on the system but there are no Bluetooth Audio Devices attached."
            L"\nPlease attach one and rerun these tests.");
        return FNS_PASS;
    }
    else
    {
        XLOG(XMSG, eError, L"PASS: Bluetooth is present on the system and there is/are %d Bluetooth Audio Device(s)", cDevices);
    }

    return FNS_PASS;
}

//--------------------------------------------------------------------------;
//
//  DWORD TC_VerifyMicArrayRawFormat
//
//  Description:
//      TestCase: Verify microphone arrays have a RAW mix format
//                with a number of channels equal to
//                the number of elements in the array
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Unused.  Failures propagated through XLOG.
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_VerifyMicArrayRawFormat()
{
    // This test only applies to NT 6.3 and above
    OSVERSIONINFO ver = {0};
    ver.dwOSVersionInfoSize = sizeof(ver);
    if (!GetVersionEx(&ver))
    {
        ERR(L"FAIL: GetVersionEx() failed: GetLastError() = %u", GetLastError());
        return (FNS_PASS);
    }

    XLOG(XMSG, eInfo1, L"INFO: OS version is %u.%u", ver.dwMajorVersion, ver.dwMinorVersion);

    if (ver.dwMajorVersion < 6 || (ver.dwMajorVersion == 6 && ver.dwMinorVersion < 3))
    {
        SKIP(L"INFO: This test case is only applicable to NT 6.3 and above.");
        return (FNS_PASS);
    }

    // get the endpoint under test
    CAudioLogoDevice *pCurrentDevice = NULL;
    if (NULL == (pCurrentDevice = g_pAudioLogoDeviceTestModule->GetDevice()))
    {
        ERR(L"FAIL: GetDevice encountered error.");
        return (FNS_PASS);
    }

    IMMDevice *pEndpoint = NULL;
    if (NULL == (pEndpoint = pCurrentDevice->GetEndpoint()))
    {
        ERR(L"FAIL: GetEndpoint encountered error.");
        return (FNS_PASS);
    }
    
    // check to see if this is a microphone array
    CComPtr<IPropertyStore> pPropertyStore;
    HRESULT hr = pEndpoint->OpenPropertyStore(STGM_READ, &pPropertyStore);
    if (FAILED(hr))
    {
        ERR(L"FAIL: IMMDevice::OpenPropertyStore failed (%s)", GetHRString(hr));
        return (FNS_PASS);
    }
    
    CPropVariant categoryString;
    hr = pPropertyStore->GetValue(PKEY_AudioEndpoint_JackSubType, &categoryString);
    if (FAILED(hr))
    {
        ERR(L"FAIL: IPropertyStore::GetValue(PKEY_AudioEndpoint_JackSubType) failed (%s)", GetHRString(hr));
        return (FNS_PASS);
    }
    
    if (VT_LPWSTR != categoryString.vt)
    {
        ERR(L"FAIL: IPropertyStore::GetValue(PKEY_AudioEndpoint_JackSubType) has unexpected vartype 0x%x", categoryString.vt);
        return (FNS_PASS);
    }
    
    LOG(L"INFO: endpoint pin category is %s", categoryString.pwszVal);
    
    GUID category;
    hr = CLSIDFromString(categoryString.pwszVal, &category);
    if (FAILED(hr))
    {
        ERR(L"FAIL: CLSIDFromString(%s) failed (%s)", categoryString.pwszVal, GetHRString(hr));
        return (FNS_PASS);
    }

    if (
        KSNODETYPE_MICROPHONE_ARRAY == category ||
        KSNODETYPE_PROCESSING_MICROPHONE_ARRAY == category
    )
    {
        // continue on; this is a microphone array
    }
    else if (KSNODETYPE_MICROPHONE_ARRAY_PROCESSOR == category)
    {
        ERR(L"FAIL: endpoint uses KSNODETYPE_MICROPHONE_ARRAY_PROCESSOR which is a node type, not a pin category");
        // assume they meant KSNODETYPE_MICROPHONE_ARRAY and continue on
    }
    else
    {
        SKIP(L"Device is not a microphone array");
        return FNS_PASS;
    }
    
    // check whether the "supports RAW mode" property is set to true
    CPropVariant rawBool;
    hr = pPropertyStore->GetValue(PKEY_Devices_AudioDevice_RawProcessingSupported, &rawBool);
    if (FAILED(hr))
    {
        ERR(L"FAIL: IPropertyStore::GetValue(PKEY_Devices_AudioDevice_RawProcessingSupported) failed (%s)", GetHRString(hr));
        return (FNS_PASS);
    }
    
    if (VT_BOOL != rawBool.vt)
    {
        ERR(L"FAIL: IPropertyStore::GetValue(PKEY_Devices_AudioDevice_RawProcessingSupported) has unexpected vartype 0x%x", rawBool.vt);
        return (FNS_PASS);
    }
    
    bool raw = (rawBool.boolVal != VARIANT_FALSE);
    LOG(L"INFO: supports raw mode: %s", (raw ? L"yes" : L"no"));
    
    if (!raw)
    {
        ERR(L"FAIL: Microphone array does not support RAW mode (PKEY_Devices_AudioDevice_RawProcessingSupported is VARIANT_FALSE.)");
        return (FNS_PASS);
    }
    
    // get the mix formats in both normal and RAW mode; not the channel count
    CComHeapPtr<WAVEFORMATEX> wfxNormal;
    hr = MixFormat(pEndpoint, false, &wfxNormal);
    if (FAILED(hr))
    {
        // MixFormat logs failures
        return (FNS_PASS);
    }
    LOG(L"INFO: mix format in normal mode has channel count %u", wfxNormal->nChannels);
    
    CComHeapPtr<WAVEFORMATEX> wfxRaw;
    hr = MixFormat(pEndpoint, true, &wfxRaw);
    if (FAILED(hr))
    {
        // MixFormat logs failures
        return (FNS_PASS);
    }
    LOG(L"INFO: mix format in raw mode has channel count %u", wfxRaw->nChannels);
    
    // compare the normal and RAW mode channel counts; normal should be <= RAW
    // that is, beam forming should not introduce *more* channels
    if (wfxNormal->nChannels > wfxRaw->nChannels)
    {
        ERR(L"FAIL: Microphone array mix format has more channels in normal mode (%u) than in RAW mode (%u)", wfxNormal->nChannels, wfxRaw->nChannels);
    }
    
    // get the mic array geometry property
    CPropVariant geometryBuffer;
    hr = pPropertyStore->GetValue(PKEY_Devices_MicrophoneArray_Geometry, &geometryBuffer);
    if (FAILED(hr))
    {
        ERR(L"FAIL: IPropertyStore::GetValue(PKEY_Devices_MicrophoneArray_Geometry) failed (%s)", GetHRString(hr));
        return (FNS_PASS);
    }
    
    if ((VT_VECTOR | VT_UI1) != geometryBuffer.vt)
    {
        ERR(L"FAIL: IPropertyStore::GetValue(PKEY_Devices_MicrophoneArray_Geometry) has unexpected vartype 0x%x", geometryBuffer.vt);
        return (FNS_PASS);
    }

    if (geometryBuffer.caub.cElems < sizeof(KSAUDIO_MIC_ARRAY_GEOMETRY))
    {
        ERR(L"FAIL: PKEY_Devices_MicrophoneArray_Geometry is too small (%u) for a KSAUDIO_MIC_ARRAY_GEOMETRY", geometryBuffer.caub.cElems);
        return (FNS_PASS);
    }
    
    PKSAUDIO_MIC_ARRAY_GEOMETRY geometry = (PKSAUDIO_MIC_ARRAY_GEOMETRY)geometryBuffer.caub.pElems;

    if (geometry->usNumberOfMicrophones == 0)
    {
        ERR(L"FAIL: PKEY_Devices_MicrophoneArray_Geometry has usNumberOfMicrophones = 0");
        return (FNS_PASS);
    }
    LOG(L"INFO: microphone array has element count %u", geometry->usNumberOfMicrophones);
    
    if (
        geometryBuffer.caub.cElems !=
            sizeof(KSAUDIO_MIC_ARRAY_GEOMETRY) +
                (geometry->usNumberOfMicrophones - 1) * sizeof(KSAUDIO_MICROPHONE_COORDINATES)
    )
    {
        ERR(L"FAIL: PKEY_Devices_MicrophoneArray_Geometry size (%u) is inconsistent with the number of microphone elements (%u)", geometryBuffer.caub.cElems, geometry->usNumberOfMicrophones);
        return (FNS_PASS);
    }
        
    // compare the mic array geometry element count to the RAW mode channel count; they should be equal
    if (geometry->usNumberOfMicrophones != wfxRaw->nChannels)
    {
        ERR(L"FAIL: The mic array has an element count of %u but the RAW format has a channel count of %u", geometry->usNumberOfMicrophones, wfxRaw->nChannels);
    }
    
    return (FNS_PASS);
}   //   TC_VerifyMicArrayRawFormat()

HRESULT MixFormat(IMMDevice *pMMDevice, bool raw, WAVEFORMATEX **ppWfx)
{
    CComPtr<IAudioClient2> pAudioClient2;
    
    HRESULT hr = pMMDevice->Activate(__uuidof(IAudioClient2), CLSCTX_ALL, NULL, (void**)&pAudioClient2);
    if (FAILED(hr))
    {
        ERR(L"IMMDevice::Activate(IAudioClient2) failed (%s)", GetHRString(hr));
        return hr;
    }
    
    if (raw)
    {
        struct AudioClientPropertiesBlue
        {
            UINT32 cbSize;
            BOOL bIsOffload;
            AUDIO_STREAM_CATEGORY eCategory;
            AUDCLNT_STREAMOPTIONS Options;
        };
        
        AudioClientPropertiesBlue props = {};
        props.cbSize = sizeof(props);
        props.bIsOffload = false;
        props.eCategory = AudioCategory_Other;
        props.Options = AUDCLNT_STREAMOPTIONS_RAW;

        hr = pAudioClient2->SetClientProperties(reinterpret_cast<AudioClientProperties*>(&props));
        if (FAILED(hr))
        {
            ERR(L"IAudioClient2::SetClientProperties failed (%s)", GetHRString(hr));
            return hr;
        }
    }
    
    hr = pAudioClient2->GetMixFormat(ppWfx);
    if (FAILED(hr))
    {
        ERR(L"IAudioClient2::GetMixFormat failed (%s)", GetHRString(hr));
        return hr;
    }
    
    return S_OK;
}