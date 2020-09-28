// Copyright(C) Microsoft.All rights reserved.

// haptest.cpp

#include <atlbase.h>
#include <atlstr.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <devicetopology.h>
#include <endpointvolume.h>
#include <strsafe.h>
#include <math.h>

#include <basiclog.h>
#include <BasicLogHelper.h>
#include <basicprintf.h>

#include <ITestResource.h>
#include <ResourceList.h>
#include <WexTestClass.h>

#include <offloadpath.h>

#include "haptest.h"
#include "log.h"

BEGIN_MODULE()
    MODULE_PROPERTY(L"TestResourceDependent", L"true")
END_MODULE()

using namespace WEX::Logging;
using namespace WEX::TestExecution;

#define REGKEY_AUDIOSERVICE_KEY L"Software\\Microsoft\\Windows\\CurrentVersion\\Audio"
#define REGKEY_MF_SAR_KEY L"Software\\Microsoft\\Windows Media Foundation\\Platform\\SAR"

DWORD g_EnableOffloadRegKeyValue = 0;
DWORD g_UseMediaStreamCategory = 0;

bool CHapTest::SetupClass()
{
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    CComPtr<IBasicLog> pLogger;
    if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return false; }

    DWORD dwStatus = 0;
    DWORD dwValue = 0;
    DWORD cbData = sizeof(dwValue);
    HKEY  hkResultOffload = 0;
    HKEY  hkResultStreamCategory = 0;
    bool  returnResult = true;
 
    /// Get original Registry values
    dwStatus = RegGetValue(HKEY_LOCAL_MACHINE,
                           REGKEY_AUDIOSERVICE_KEY,
                           L"AllowClassicOffload",
                           RRF_RT_DWORD, 
                           NULL,
                           &dwValue,
                           &cbData);

    if (dwStatus == ERROR_FILE_NOT_FOUND)
    {
       LOG(L"Classic Offload regkey does not exist.");
       g_EnableOffloadRegKeyValue = 0;
    }
    else if (dwStatus != ERROR_SUCCESS)
    {
       ERR(L"Failed to query the AllowClassicOffload regkey. [0x%x]", 
           HRESULT_FROM_WIN32(dwStatus));
       returnResult = false;
       goto CLEANUP;
    }
    else if (cbData != sizeof(dwValue))
    {
       ERR(L"AllowClassicOffload data of unexpected size. [%u]", 
           cbData);
       returnResult = false;
       goto CLEANUP;
    }
    else
    {
        g_EnableOffloadRegKeyValue = dwValue;
    }

    cbData = sizeof(dwValue);
    dwStatus = RegGetValue(HKEY_LOCAL_MACHINE,
                           REGKEY_MF_SAR_KEY,
                           L"UseMediaStreamCategory",
                           RRF_RT_DWORD, 
                           NULL,
                           &dwValue,
                           &cbData);

    if (dwStatus == ERROR_FILE_NOT_FOUND)
    {
       LOG(L"Use Media Stream Category regkey does not exist.");
       g_UseMediaStreamCategory = 0;
    }
    else if (dwStatus != ERROR_SUCCESS)
    {
       ERR(L"Failed to query the UseMediaStreamCategory regkey. [0x%x]", 
           HRESULT_FROM_WIN32(dwStatus));
       returnResult = false;
       goto CLEANUP;
    }
    else if (cbData != sizeof(dwValue))
    {
       ERR(L"UseMediaStreamCategory data of unexpected size. [%u]", 
           cbData);
       returnResult = false;
       goto CLEANUP;
    }
    else
    {
        g_UseMediaStreamCategory = dwValue;
    }

    //turn regkeys on
    dwValue = 1; 
 
    /// Set AllowClassicOffload regkey
    dwStatus = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                              REGKEY_AUDIOSERVICE_KEY,
                              0,
                              NULL,
                              0,
                              KEY_WRITE,
                              NULL,
                              &hkResultOffload,
                              NULL);
    if (dwStatus == ERROR_ACCESS_DENIED)
    {
       ERR(L"Please run HAPTest in an elevated cmd prompt. [0x%x]", 
           HRESULT_FROM_WIN32(dwStatus));
       returnResult = false;
       goto CLEANUP;
    }
    else if (dwStatus != ERROR_SUCCESS)
    {
        ERR(L"Failed to create the Classic Offload regkey. [0x%x]", 
            HRESULT_FROM_WIN32(dwStatus));
        returnResult = false;
        goto CLEANUP;
    }
    
    dwStatus = RegSetValueEx(hkResultOffload,
                             L"AllowClassicOffload",
                             0,
                             REG_DWORD,
                             (LPBYTE)&dwValue,
                             sizeof(dwValue));
    if (dwStatus == ERROR_ACCESS_DENIED)
    {
       ERR(L"Please run HAPTest in an elevated cmd prompt. [0x%x]", 
           HRESULT_FROM_WIN32(dwStatus));
       returnResult = false;
       goto CLEANUP;
    }
    else if (dwStatus != ERROR_SUCCESS)
    {
       ERR(L"Failed to set the Classic Offload regkey. [0x%x]", 
           HRESULT_FROM_WIN32(dwStatus));
       returnResult = false;
       goto CLEANUP;
    }

    /// Set UseMediaStreamCategory regkey
    dwStatus = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                              REGKEY_MF_SAR_KEY,
                              0,
                              NULL,
                              0,
                              KEY_WRITE,
                              NULL,
                              &hkResultStreamCategory,
                              NULL);
    if (dwStatus == ERROR_ACCESS_DENIED)
    {
       ERR(L"Please run HAPTest in an elevated cmd prompt. [0x%x]", 
           HRESULT_FROM_WIN32(dwStatus));
       returnResult = false;
       goto CLEANUP;
    }
    else if (dwStatus != ERROR_SUCCESS)
    {
        ERR(L"Failed to create the UseMediaStreamCategory regkey. [0x%x]", 
            HRESULT_FROM_WIN32(dwStatus));
        returnResult = false;
        goto CLEANUP;
    }

    dwStatus = RegSetValueEx(hkResultStreamCategory,
                             L"UseMediaStreamCategory",
                             0,
                             REG_DWORD,
                             (LPBYTE)&dwValue,
                             sizeof(dwValue));
    if (dwStatus == ERROR_ACCESS_DENIED)
    {
       ERR(L"Please run HAPTest in an elevated cmd prompt. [0x%x]", 
           HRESULT_FROM_WIN32(dwStatus));
       returnResult = false;
       goto CLEANUP;
    }
    else if (dwStatus != ERROR_SUCCESS)
    {
       ERR(L"Failed to set the UseMediaStreamCategory regkey. [0x%x]", 
           HRESULT_FROM_WIN32(dwStatus));       
       returnResult = false;
       goto CLEANUP;
    }


CLEANUP:

    if (hkResultOffload)
    {
       dwStatus = RegFlushKey(hkResultOffload);
       if (dwStatus != ERROR_SUCCESS)
       {
          ERR(L"Failed to flush the Classic Offload regkey. [0x%x]", 
              HRESULT_FROM_WIN32(dwStatus));
          returnResult = false;
       }
       
       dwStatus = RegCloseKey(hkResultOffload);
       if (dwStatus != ERROR_SUCCESS)
       {
          ERR(L"Failed to close the Classic Offload regkey. [0x%x]", 
              HRESULT_FROM_WIN32(dwStatus));
          returnResult = false;
       }
    }

    if (hkResultStreamCategory)
    {
        dwStatus = RegFlushKey(hkResultStreamCategory);
        if (dwStatus != ERROR_SUCCESS)
        {
           ERR(L"Failed to flush the UseMediaStreamCategory regkey. [0x%x]", 
               HRESULT_FROM_WIN32(dwStatus));
           returnResult = false;
        }
    
        dwStatus = RegCloseKey(hkResultStreamCategory);
        if (dwStatus != ERROR_SUCCESS)
        {
           ERR(L"Failed to close the UseMediaStreamCategory regkey. [0x%x]", 
               HRESULT_FROM_WIN32(dwStatus));
           returnResult = false;
        }
    }

    return returnResult;
}

bool CHapTest::CleanupClass()
{
   SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

   CComPtr<IBasicLog> pLogger;
   if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return false; }

   DWORD dwStatus = 0;
   DWORD dwValue = 0;
   HKEY  hkResultOffload = 0;
   HKEY  hkResultStreamCategory = 0;
   bool  returnResult = true;

   /// Set AllowClassicOffload regkey
   dwStatus = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                             REGKEY_AUDIOSERVICE_KEY,
                             0,
                             NULL,
                             0,
                             KEY_WRITE,
                             NULL,
                             &hkResultOffload,
                             NULL);
   if (dwStatus != ERROR_SUCCESS)
   {
       ERR(L"Failed to create the Classic Offload regkey. [0x%x]", 
           HRESULT_FROM_WIN32(dwStatus));
       returnResult = false;
       goto CLEANUP;
   }

   /// Retrieve original value
   dwValue = g_EnableOffloadRegKeyValue;
      
   dwStatus = RegSetValueEx(hkResultOffload,
                            L"AllowClassicOffload",
                            0,
                            REG_DWORD,
                            (LPBYTE)&dwValue,
                            sizeof(dwValue));
   if (dwStatus != ERROR_SUCCESS)
   {
      ERR(L"Failed to set the Classic Offload regkey. [0x%x]", 
          HRESULT_FROM_WIN32(dwStatus));
      returnResult = false;
      goto CLEANUP;
   }

   /// Set UseMediaStreamCategory regkey
   dwStatus = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                             REGKEY_MF_SAR_KEY,
                             0,
                             NULL,
                             0,
                             KEY_WRITE,
                             NULL,
                             &hkResultStreamCategory,
                             NULL);
   if (dwStatus != ERROR_SUCCESS)
   {
       ERR(L"Failed to create the UseMediaStreamCategory regkey. [0x%x]", 
           HRESULT_FROM_WIN32(dwStatus));
       returnResult = false;
       goto CLEANUP;
   }

   /// Retrieve original value
   dwValue = g_UseMediaStreamCategory;
   
   dwStatus = RegSetValueEx(hkResultStreamCategory,
                            L"UseMediaStreamCategory",
                            0,
                            REG_DWORD,
                            (LPBYTE)&dwValue,
                            sizeof(dwValue));
   if (dwStatus != ERROR_SUCCESS)
   {
      ERR(L"Failed to set the UseMediaStreamCategory regkey. [0x%x]", 
          HRESULT_FROM_WIN32(dwStatus));       
      returnResult = false;
      goto CLEANUP;
   }
   

CLEANUP:
   
   dwStatus = RegFlushKey(hkResultOffload);
   if (dwStatus != ERROR_SUCCESS)
   {
      ERR(L"Failed to flush the Classic Offload regkey. [0x%x]", 
          HRESULT_FROM_WIN32(dwStatus));
      returnResult = false;
   }
   
   dwStatus = RegCloseKey(hkResultOffload);
   if (dwStatus != ERROR_SUCCESS)
   {
      ERR(L"Failed to close the Classic Offload regkey. [0x%x]", 
          HRESULT_FROM_WIN32(dwStatus));
      returnResult = false;
   }
   
   dwStatus = RegFlushKey(hkResultStreamCategory);
   if (dwStatus != ERROR_SUCCESS)
   {
      ERR(L"Failed to flush the UseMediaStreamCategory regkey. [0x%x]", 
          HRESULT_FROM_WIN32(dwStatus));
      returnResult = false;
   }
   
   dwStatus = RegCloseKey(hkResultStreamCategory);
   if (dwStatus != ERROR_SUCCESS)
   {
      ERR(L"Failed to close the UseMediaStreamCategory regkey. [0x%x]", 
          HRESULT_FROM_WIN32(dwStatus));
      returnResult = false;
   }

   return returnResult;
}

bool
IgnoreDeviceState(_In_ IMMDevice *pDevice, DWORD deviceState)
{
    bool bIgnoreDevice = false;
    CComPtr<IDeviceTopology> spDevTopology;
    CComPtr<IConnector> spDevConnector;
    CComPtr<IConnector> spDevTopologyConnector;
    CComPtr<IPart> spDevTopologyConnectorPart;
    GUID guidKsNodeType = GUID_NULL;

    if (!VERIFY_SUCCEEDED(pDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_ALL, NULL, (void**)&spDevTopology))) { return bIgnoreDevice; }
    if (!VERIFY_SUCCEEDED(spDevTopology->GetConnector(0, &spDevConnector))) { return bIgnoreDevice; }
    if (!VERIFY_SUCCEEDED(spDevConnector->GetConnectedTo(&spDevTopologyConnector))) { return bIgnoreDevice; }
    if (!VERIFY_SUCCEEDED(spDevTopologyConnector->QueryInterface(__uuidof(IPart), (void**)&spDevTopologyConnectorPart))) { return bIgnoreDevice; }
    if (!VERIFY_SUCCEEDED(spDevTopologyConnectorPart->GetSubType(&guidKsNodeType))) { return bIgnoreDevice; }

    // Cellular and FM endpoints can be unplugged during the test run so we would not run tests on these endpoints
    if ((IsEqualGUID(guidKsNodeType, KSNODETYPE_TELEPHONY_BIDI) || IsEqualGUID(guidKsNodeType, KSNODETYPE_FM_RX)) && (DEVICE_STATE_UNPLUGGED == deviceState))
    {
        bIgnoreDevice = true;
    }

    return bIgnoreDevice;
}


_Success_(return != 0)
bool DeviceUnderTest(_In_ IBasicLog *pLogger, _COM_Outptr_ IMMDevice **ppMMDevice, _Outptr_opt_ bool *pbSysvad) {
    const WCHAR SYSVAD_PREFIX[] = L"ROOT\\SYSVAD";
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    *ppMMDevice = NULL;
    if (!VERIFY_IS_NOT_NULL(pLogger)) { return false; }
    if (pbSysvad) 
    { 
        *pbSysvad = false; 
    }

    // We're using the simplest form of TAEF device selection
    // where we tell TAEF "call this function once for each device"
    // it's possible to use a more advanced form of TAEF device selection
    // where we tell TAEF "call this function once for each PAIR of devices"
    // in the more advanced form, Resources::Count() may be > 1
    // but we're keeping it simple in this function
    if (!VERIFY_ARE_EQUAL((size_t)1, Resources::Count())) { return false; }
    
    CComPtr<ITestResource> pTestResource;
    if (!VERIFY_SUCCEEDED(Resources::Item(0, &pTestResource))) { return false; }
    
    CComBSTR szId;
    if (!VERIFY_SUCCEEDED(
        pTestResource->GetValue(
            CComBSTR(TestResourceProperty::c_szId),
            &szId
    ))) { return false; }

    // this is necessary because if TAEF finds a test case with no matching devices
    // then it will log a Blocked result
    // this rolls up as a WTT failure (a Skip result is more appropriate)
    //
    // The device enumeration logic should be such that if no genuine devices of a class are found
    // then a faux device with id = NO_SUCH_DEVICE should be added
    // for the sole benefit of allowing us to manually log a Skip instead of letting TAEF log a Block
    if (0 == wcscmp(szId, NO_SUCH_DEVICE)) {
        Log::Result(TestResults::Result::Skipped, L"No applicable devices in the system.");
        return false;
    }
    
    CComBSTR szName;
    if (!VERIFY_SUCCEEDED(
        pTestResource->GetValue(
            CComBSTR(TestResourceProperty::c_szName),
            &szName
    ))) { return false; }

    CComBSTR szInstanceId;
    if (!VERIFY_SUCCEEDED(
        pTestResource->GetValue(
            CComBSTR(L"InstanceId"),
            &szInstanceId
    ))) { return false; }
    LOG(L"Running on %s", static_cast<LPCWSTR>(szName));
    LOG(L"Instance ID: %s", static_cast<LPCWSTR>(szInstanceId));

    if (pbSysvad)
    {
        *pbSysvad = (0 == _wcsnicmp(static_cast<LPCWSTR>(szInstanceId), SYSVAD_PREFIX, wcslen(SYSVAD_PREFIX)));
    }

    CComPtr<IMMDeviceEnumerator> pMMDeviceEnumerator;
    if (!VERIFY_SUCCEEDED(pMMDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)))) { return false; }

    CComPtr<IMMDevice> pMMDevice;
    if (!VERIFY_SUCCEEDED(pMMDeviceEnumerator->GetDevice(szId, &pMMDevice))) {
        *ppMMDevice = NULL;
        return false;
    }

    DWORD deviceState = DEVICE_STATE_NOTPRESENT;
    if (!VERIFY_SUCCEEDED(pMMDevice->GetState(&deviceState))) { return false; }

    if (DEVICE_STATE_ACTIVE != deviceState) {
        // If the device state is unplugged and this device does not have physical jack, ignore the device and do not fail.
        if (IgnoreDeviceState(pMMDevice, deviceState)) {
            LOG(L"Ignore Device Under Test since it is not active. Device State: 0x%x (%s)", deviceState, GetDeviceState(deviceState));
            return false;
        }
        else
        {
            ERR(L"Device Under Test is not active. Device State: 0x%x (%s)", deviceState, GetDeviceState(deviceState));
            return false;
        }
    }

    *ppMMDevice = pMMDevice.Detach();

    return true;
}

HRESULT GetOffloadPath(_In_ IBasicLog *pLogger, _In_ IMMDevice *pMMDevice, _COM_Outptr_ IOffloadPath **ppOffloadPath) {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    *ppOffloadPath = NULL;
    if (!VERIFY_IS_NOT_NULL(pLogger)) { return E_POINTER; }
    if (!VERIFY_IS_NOT_NULL(pMMDevice)) { return E_POINTER; }
    
    HRESULT hr;
    
    CComPtr<IOffloadPathCollection> pOffloadPathCollection;
    if (!VERIFY_SUCCEEDED(hr =
        OffloadEnumerator::GetOffloadPath(
            pLogger,
            pMMDevice,
            &pOffloadPathCollection
    ))) { return hr; }

    UINT nPaths;
    if (!VERIFY_SUCCEEDED(hr = pOffloadPathCollection->GetCount(nPaths))) { return hr; }
    
    if (!VERIFY_IS_GREATER_THAN_OR_EQUAL(nPaths, 1u)) { return HRESULT_FROM_WIN32(ERROR_NOT_FOUND); }
    
    if (nPaths >= 2u) {
        WARN(
            L"%u offload paths found for this device. "
            L"This test operates under the assumption that "
            L"there is only one audio offload path per endpoint.",
            nPaths
        );
        // we'll just pass back the first offload path and hope for the best
    }
    
    if (!VERIFY_SUCCEEDED(hr = pOffloadPathCollection->Item(0, ppOffloadPath))) { return hr; }
    
    return S_OK;
}

HRESULT GetAdapterKsControl(_In_ IBasicLog * /*pLogger*/, _In_ IOffloadPath *pOffloadPath, _COM_Outptr_ IKsControl **ppKsControl) {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    *ppKsControl = NULL;
    if (!VERIFY_IS_NOT_NULL(pOffloadPath)) { return E_POINTER; }

    HRESULT hr;
    
    CComHeapPtr<WCHAR> szAdapterId;
    if (!VERIFY_SUCCEEDED(hr = pOffloadPath->GetMMDeviceId(&szAdapterId))) { return hr; }
    
    CComPtr<IMMDeviceEnumerator> pMMDeviceEnumerator;
    if (!VERIFY_SUCCEEDED(hr =
        CoCreateInstance(
            __uuidof(MMDeviceEnumerator),
            NULL,
            CLSCTX_ALL,
            __uuidof(IMMDeviceEnumerator),
            reinterpret_cast<void**>(&pMMDeviceEnumerator)
    ))) { return hr; }
    
    CComPtr<IMMDevice> pMMDeviceAdapter;
    if (!VERIFY_SUCCEEDED(hr =
        pMMDeviceEnumerator->GetDevice(
            szAdapterId,
            &pMMDeviceAdapter
    ))) { return hr; }
    
    if (!VERIFY_SUCCEEDED(hr = 
        pMMDeviceAdapter->Activate(
            __uuidof(IKsControl),
            CLSCTX_ALL,
            NULL,
            reinterpret_cast<void**>(ppKsControl)
    ))) {
        *ppKsControl = NULL;
        return hr;
    }

    return S_OK;
}

// power is magnitude squared
double DbFromMagnitude(double m) {
    double power = m * m;
    return 10 * log10(power); // = 20 * log10(m)
}

double MagnitudeFromDb(double dB) {
    double power = pow(10.0, dB / 10.0);
    return sqrt(power); // = 10 ^ (dB / 20)
}

// a sine wave of amplitude A
// has an RMS value of A * sqrt(1/2)
//
// for example a full-scale sine wave (-1.0 to 1.0) has an RMS value of 1 * sqrt(1/2) = 0.707...
double DbFromSineAmp(double a) {
    return DbFromMagnitude(a * sqrt(0.5)); // = DbFromMagnitude(a) - 3.01... dB
}

// we're given the power dB, not the peak-to-peak dB
double SineAmpFromDb(double dB) {
    return MagnitudeFromDb(dB) * sqrt(2.0);
}

// convert a dB measurement to a KSPROPERTY_*_VOLUMELEVEL LONG
LONG KsVolumeFromDb(double dB) {
    return
        dB >= 0.0 ?
        
            // we can use the standard rounding technique for positive numbers
            // 10.0 dB =>
            //     (LONG)(10.0 * 65536 + 0.5) =>
            //     (LONG)(655360.0 + 0.5) =>
            //     (LONG)655360.5 => 655360
            (LONG)(dB * 65536.0 + 0.5) :
            
            // for negative numbers we need to round differently
            // the standard technique fails:
            // -10.0 dB =>
            //     (LONG)(-10.0 * 65536 + 0.5) =>
            //     (LONG)(-655360.0 + 0.5) =>
            //     (LONG)(-655359.5)
            // in some cases this rounds up to -655359
            //
            // so we convert to a positive number and do the standard rounding technique
            // and then convert the rounded number back to a negative
            // -10.0 dB =>
            //     -(LONG)(-(-10.0) * 65536 + 0.5) =>
            //     -(LONG)(655360.0 + 0.5) =>
            //     -(LONG)(655360.5) =>
            //     -655360
            -(LONG)(-dB * 65536.0 + 0.5)
        ;
}

HRESULT GetModes(IKsControl *pKsControl, UINT pin, GUID **ppModes, UINT *pnModes) {
    *ppModes = nullptr;
    *pnModes = 0;

    KSP_PIN ksp = {};
    
    ksp.Property.Set = KSPROPSETID_AudioSignalProcessing;
    ksp.Property.Id = KSPROPERTY_AUDIOSIGNALPROCESSING_MODES;
    ksp.Property.Flags = KSPROPERTY_TYPE_GET;
    ksp.PinId = pin;
    
    // first, get the buffer size
    ULONG bufferSize = 0;
    HRESULT hr = pKsControl->KsProperty(
        &ksp.Property,
        sizeof(ksp),
        NULL, 0,
        &bufferSize
    );
    
    if (FAILED(hr)) {
        // no mode support; that's fine
        return S_OK;
    }
    
    if (
        // buffer size must be at least sizeof(KSMULTIPLE_ITEM)
        !VERIFY_IS_GREATER_THAN_OR_EQUAL((size_t)bufferSize, sizeof(KSMULTIPLE_ITEM)) ||
        
        // leftover must be a multiple of sizeof(GUID)
        !VERIFY_ARE_EQUAL((size_t)0, (bufferSize - sizeof(KSMULTIPLE_ITEM)) % sizeof(GUID))
    ) {
        return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
    }
    
    UINT nModes = (UINT)((bufferSize - sizeof(KSMULTIPLE_ITEM)) / sizeof(GUID));

    CComHeapPtr<KSMULTIPLE_ITEM> pProperty;
    if(!VERIFY_IS_TRUE(pProperty.AllocateBytes(bufferSize))) { return E_OUTOFMEMORY; }
    
    ULONG written = 0;
    if (!VERIFY_SUCCEEDED(hr = pKsControl->KsProperty(
        &ksp.Property,
        sizeof(ksp),
        (KSMULTIPLE_ITEM*)pProperty, bufferSize,
        &written
    ))) { return hr; }
    
    if (
        // buffer size must not have changed
        !VERIFY_ARE_EQUAL(bufferSize, written) ||
        
        // KSMULTIPLE_ITEM.Size must be equal to buffer size
        !VERIFY_ARE_EQUAL(bufferSize, pProperty->Size) ||
        
        // KSMULTIPLE_ITEM.Count must be equal to number of modes
        !VERIFY_ARE_EQUAL(nModes, pProperty->Count)
        
    ) { return HRESULT_FROM_WIN32(ERROR_INVALID_DATA); }
    
    // copy the modes into the output buffer
    CComHeapPtr<GUID> pModes;
    if(!VERIFY_IS_TRUE(pModes.Allocate(nModes))) { return E_OUTOFMEMORY; }
    
    GUID *pGuids = (GUID*)((KSMULTIPLE_ITEM*)pProperty + 1);
    for (UINT i = 0; i < nModes; i++) {
        pModes[i] = pGuids[i];
    }
    
    *ppModes = pModes.Detach();
    *pnModes = nModes;
    
    return S_OK;
}

HRESULT SetVolume(IKsControl *pKsControl, UINT node, double dB) {
    KSNODEPROPERTY_AUDIO_CHANNEL ks = {0};
    ks.NodeProperty.Property.Set = KSPROPSETID_AudioEngine;
    ks.NodeProperty.Property.Id = KSPROPERTY_AUDIOENGINE_VOLUMELEVEL;
    ks.NodeProperty.Property.Flags = KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_TOPOLOGY;
    ks.NodeProperty.NodeId = node;
    ks.Channel = -1; // all channels

    KSAUDIOENGINE_VOLUMELEVEL vol = {0};
    vol.TargetVolume = KsVolumeFromDb(dB);
    vol.CurveType = AUDIO_CURVE_TYPE_NONE;
    vol.CurveDuration = 0;
    
    HRESULT hr;
    if (!VERIFY_SUCCEEDED(
        hr = pKsControl->KsProperty(
            &ks.NodeProperty.Property,
            sizeof(ks),
            &vol,
            sizeof(vol),
            NULL
    ))) { return hr; }
    
    return S_OK;
}

HRESULT SetVolumeRamp(IKsControl *pKsControl, UINT node, double dB, double seconds) {
    KSNODEPROPERTY_AUDIO_CHANNEL ks = {0};
    ks.NodeProperty.Property.Set = KSPROPSETID_AudioEngine;
    ks.NodeProperty.Property.Id = KSPROPERTY_AUDIOENGINE_VOLUMELEVEL;
    ks.NodeProperty.Property.Flags = KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_TOPOLOGY;
    ks.NodeProperty.NodeId = node;
    ks.Channel = -1; // all channels

    KSAUDIOENGINE_VOLUMELEVEL vol = {0};
    vol.TargetVolume = KsVolumeFromDb(dB);
    vol.CurveType = AUDIO_CURVE_TYPE_WINDOWS_FADE;
    vol.CurveDuration = (REFERENCE_TIME)(seconds * MS_PER_SECOND * HNS_PER_MS + 0.5);
    
    HRESULT hr;
    if (!VERIFY_SUCCEEDED(
        hr = pKsControl->KsProperty(
            &ks.NodeProperty.Property,
            sizeof(ks),
            &vol,
            sizeof(vol),
            NULL
    ))) { return hr; }
    
    return S_OK;
}

void LogTime(IBasicLog *pLogger, LPCWSTR szMessage) {
    SYSTEMTIME now = {0};
    GetLocalTime(&now);
    LOG(L"%02d:%02d:%02d.%03d: %s", now.wHour, now.wMinute, now.wSecond, now.wMilliseconds, szMessage);
}

#define HANDLE_CASE(x) case x: return L ## #x

LPCWSTR GetDeviceState(DWORD dwDeviceState) {
    switch (dwDeviceState) {
        HANDLE_CASE(DEVICE_STATE_ACTIVE);
        HANDLE_CASE(DEVICE_STATE_DISABLED);
        HANDLE_CASE(DEVICE_STATE_NOTPRESENT);
        HANDLE_CASE(DEVICE_STATE_UNPLUGGED);
        default: return L"Unknown device state";
    }
}

CString StringSampleRate(int r) {
    CString s;
    if (0 == r % 1000) {
        // 48000 Hz => 48 kHz
        s.Format(L"%u kHz", r / 1000);
    } else if (0 == r % 100) {
        // 44100 Hz => 44.1 kHz
        s.Format(L"%u.%u kHz", r / 1000, (r / 100) % 10);
    } else if (0 == r % 10) {
        // 22050 Hz => 22.05 kHz
        s.Format(L"%u.%02u kHz", r / 1000, (r / 10) % 100);
    } else {
        // 11025 Hz => 11.025 kHz
        s.Format(L"%u.%03u kHz", r / 1000, r % 1000);
    }
    
    return s;
}

bool VerifyAscendingInclusive(
    IBasicLog *pLogger,
    double x_value, LPCWSTR x_label,
    double y_value, LPCWSTR y_label,
    double z_value, LPCWSTR z_label
) {
    if (x_value <= y_value && y_value <= z_value) {
        LOG(
            L"VERIFY_ASCENDING_INCLUSIVE: ((%s) = %g) <= ((%s) = %g) <= ((%s) = %g) holds",
            x_label, x_value, y_label, y_value, z_label, z_value
        );
        
        return true;
    }
    
    double yx_diff = y_value - x_value;
    double zy_diff = z_value - y_value;
    double zx_diff = z_value - x_value;
    
    ERR(
        L"VERIFY_ASCENDING_INCLUSIVE: ((%s) = %g) <= ((%s) = %g) <= ((%s) = %g) fails to hold: "
        L"(%s) is %g more than (%s) (%s); "
        L"(%s) is %g more than (%s) (%s); "
        L"(%s) is %g more than (%s) (%s)",
        x_label, x_value, y_label, y_value, z_label, z_value,
        (yx_diff >= 0 ? y_label : x_label),
            fabs(yx_diff),
            (yx_diff >= 0 ? x_label : y_label),
            (yx_diff >= 0 ? L"fine" : L"problem"),
        (zy_diff >= 0 ? z_label : y_label),
            fabs(zy_diff),
            (zy_diff >= 0 ? y_label : z_label),
            (zy_diff >= 0 ? L"fine" : L"problem"),
        (zx_diff >= 0 ? z_label : x_label),
            fabs(zx_diff),
            (zx_diff >= 0 ? x_label : z_label),
            (zx_diff >= 0 ? L"fine" : L"problem")
    );

    return false;
}

CNormalizeVolumeAndRestoreOnExit::CNormalizeVolumeAndRestoreOnExit(IMMDevice *pMMDevice)
: m_pAudioEndpointVolume(nullptr)
, m_volume(0.0f)
, m_restore(false)
{
    if (!VERIFY_SUCCEEDED(
        pMMDevice->Activate(
            __uuidof(IAudioEndpointVolume),
            CLSCTX_INPROC_SERVER,
            nullptr,
            (LPVOID*)&m_pAudioEndpointVolume
        )
    )) { return; }
    
    if (!VERIFY_SUCCEEDED(
        m_pAudioEndpointVolume->GetMasterVolumeLevelScalar(&m_volume)
    )) { return; }

    if (!VERIFY_SUCCEEDED(
        m_pAudioEndpointVolume->SetMasterVolumeLevelScalar(1.0f, NULL)
    )) { return; }

    m_restore = true;
}

CNormalizeVolumeAndRestoreOnExit::~CNormalizeVolumeAndRestoreOnExit() {
    if (m_restore) {
        VERIFY_SUCCEEDED(m_pAudioEndpointVolume->SetMasterVolumeLevelScalar(m_volume, NULL));
    }
}
