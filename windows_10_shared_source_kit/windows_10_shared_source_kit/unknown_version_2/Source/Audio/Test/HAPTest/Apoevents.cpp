// Copyright(C) Microsoft.All rights reserved.

// Apoevents.cpp

#include <windows.h>
#include <versionhelpers.h>

#include <atlbase.h>
#include <atlstr.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <strsafe.h>
#include <propkey.h>

#include <basiclog.h>
#include <BasicLogHelper.h>
#include <basicprintf.h>

#include <audiostreaming.h>
#include <evntcons.h>
#include <etwlogger.h>
#include <mfcoreevents.h>

#include <WexTestClass.h>

#include "haptest.h"
#include "log.h"

#include "HWAudioEngineEventLogger.h"

#include "initguid.h"
#include <audioenginebaseapo.h>
#include <functiondiscoverykeys_devpkey.h>
#include <AudioPolicyP.h>

#include <debug.h>
#include <doonexit.h>

using namespace WEX::Logging;
using namespace WEX::TestExecution;
using namespace WEX::Common;

// {AE4BD3BE-F36F-45B6-8D21-BDD6FB832853}
static const GUID Microsoft_Windows_Audio = { 0xAE4BD3BE, 0xF36F, 0x45B6, { 0x8D, 0x21, 0xBD, 0xD6, 0xFB, 0x83, 0x28, 0x53 } };
// Event id of APO Init
static const USHORT APOEventId = 50;
//  IPolicyConfigWin10Th - Win 10 Threshold version of IPolicyConfig
static const GUID IID_IPolicyConfigWin10Th = { 0xCA286FC3, 0x91FD, 0x42C3, { 0x8E, 0x9B, 0xCA, 0xAF, 0xA6, 0x62, 0x42, 0xE3 } };

typedef struct _SYSTEM_EFFECT_APO_INITIALIZED
{
    GUID APOCLSID;
    GUID AudioSignalProcessingMode;
    BOOL InitializeForDiscoveryOnly;
}SYSTEM_EFFECT_APO_INITIALIZED;

// Some helper functions
UINT ExpectedOfApoInitEvents(_In_ IMMDevice *pMMDevice, _In_ BOOL israw);
UINT FilterApoEvents(_In_ HwAudioEngineEventLogger mfCoreEventLogger);
BOOL HasClsID(_In_ IMMDevice *pMMDevice, _In_ const PROPERTYKEY pkey);
BOOL IsNotProxyApo(_In_ IMMDevice *pMMDevice, _In_ const PROPERTYKEY pkey);
CStringW GetFriendlyName(_In_ IMMDevice* pMMDevice);


void CHapTest::APORegistration() {

   if (!IsWindows8Point1OrGreater()) {
       Log::Result(TestResults::Result::Skipped, L"Test applies only to Windows 8.1 and up");
       return;
   }

    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    CComPtr<IBasicLog> pLogger;
    if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return; }

    CComPtr<IMMDevice> pMMDevice;
    if (!DeviceUnderTest(pLogger, &pMMDevice)) { return; }

    bool hasLFXOrGFX = false;

    // Check if legacy effects is available
    if (HasClsID( pMMDevice, PKEY_FX_PreMixEffectClsid)) {
        Log::Comment(L"Local effect (LFX) is registered");
        hasLFXOrGFX = true;
    }

    if (HasClsID( pMMDevice, PKEY_FX_PostMixEffectClsid)) {
        Log::Comment(L"Global effect (GFX) is registered");
        hasLFXOrGFX = true;
    }
    
    // If LFX or GFX is there make sure one at least one of SME is
    if (hasLFXOrGFX) {
        bool hasSMEFX = false;
        Log::Comment(L"As either LFX or GFX is registered SFX, MFX or EFX needs to be supported");

        if (HasClsID( pMMDevice, PKEY_FX_StreamEffectClsid)) {
            Log::Comment(L"Stream effect (SFX) is registered");
            hasSMEFX = true;
        }
        
        if (HasClsID( pMMDevice, PKEY_FX_ModeEffectClsid)) {
            Log::Comment(L"Mode effect (MFX) is registered");
            hasSMEFX = true;
        }
    
        if (HasClsID( pMMDevice, PKEY_FX_EndpointEffectClsid)) {
            Log::Comment(L"Endpoint effect (EFX) is registered");
            hasSMEFX = true;

        }
        if (!VERIFY_IS_TRUE(hasSMEFX, L"When having LFX or GFX you also need to support one of SFX, MFX or EFX")) { return; }
    } 
        
    return;
}

void CHapTest::CommunicationCaptureDefault() {

   if (!IsWindows8Point1OrGreater()) {
       Log::Result(TestResults::Result::Skipped, L"Test applies only to Windows 8.1 and up");
       return;
   }

    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    CComPtr<IBasicLog> pLogger;
    if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return; }
    
    CComPtr<IMMDevice> pMMDevice;
    if (!DeviceUnderTest(pLogger, &pMMDevice)) { return; }

    UINT eventsExpected = ExpectedOfApoInitEvents(pMMDevice, false);

    CComPtr<IAudioStreamingCapture> pAudioStreamingCapture;

    if (!VERIFY_SUCCEEDED(
        CreateAudioStreamingCapture(
        api_WASAPI,
        pMMDevice,
        pLogger,
        &pAudioStreamingCapture
        ))) { return; }

    AudioClientPropertiesWin8 clientProperties = {0};
    clientProperties.eCategory = AudioCategory_Communications;
    clientProperties.bIsOffload = false;
    clientProperties.cbSize = sizeof(clientProperties);

    CComPtr<IAudioStreamingControl> pAudioStreamingControl;
    if (!VERIFY_SUCCEEDED(
        pAudioStreamingCapture->QueryInterface(
        __uuidof(IAudioStreamingControl),
        reinterpret_cast<void**>(&pAudioStreamingControl)
        ))) { return; }
            
    CComPtr<IAudioStreamingWasapiControl> pAudioStreamingWasapiControl;
    if (!VERIFY_SUCCEEDED(
         pAudioStreamingControl->GetService(
         __uuidof(IAudioStreamingWasapiControl),
         reinterpret_cast<void**>(&pAudioStreamingWasapiControl)
         ))) { return; }
            
#pragma prefast(suppress: __WARNING_BUFFER_OVERFLOW, "client properties is variable-sized")
    if (!VERIFY_SUCCEEDED(
        pAudioStreamingWasapiControl->SetClientProperties(
        reinterpret_cast<AudioClientProperties*>(&clientProperties)
        ))) { return; }

    HwAudioEngineEventLogger mfCoreEventLogger(pLogger);
    if (!VERIFY_SUCCEEDED(mfCoreEventLogger.Initialize(L"APOInit", Microsoft_Windows_Audio, ETW_TRACE_LEVEL_INFORMATION))) { return; }
    // start gathering ETW events
    if (!VERIFY_SUCCEEDED(mfCoreEventLogger.Start())) { return; }

    if (!VERIFY_SUCCEEDED(pAudioStreamingCapture->Initialize(NULL))) { return; }
    
    CComHeapPtr<WAVEFORMATEX> pWfxCapture;   
    if (!VERIFY_SUCCEEDED(pAudioStreamingCapture->GetWaveFormat(&pWfxCapture))) { return; }

    // allocate a buffer that can hold a second's worth of data
    DWORD nFrames = (DWORD)(pWfxCapture->nSamplesPerSec + 0.5);
    DWORD nBytes = nFrames * pWfxCapture->nBlockAlign;

    CComHeapPtr<BYTE> pCaptureBuffer;
    if (!VERIFY_IS_TRUE(pCaptureBuffer.Allocate(nBytes))) { return; }
        
    if (!VERIFY_SUCCEEDED(
        pAudioStreamingCapture->CaptureBuffer(
        pCaptureBuffer,
        nBytes
         ))) { return; }

    if (!VERIFY_SUCCEEDED(mfCoreEventLogger.Stop())) { return; }

    UINT events = FilterApoEvents(mfCoreEventLogger);

    VERIFY_ARE_EQUAL(eventsExpected, events);

    return;
}

void CHapTest::CommunicationRenderRaw() {

    if (!IsWindows8Point1OrGreater()) {
        Log::Result(TestResults::Result::Skipped, L"Test applies only to Windows 8.1 and up");
        return;
    }

    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    CComPtr<IBasicLog> pLogger;
    if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return; }
   
    CComPtr<IMMDevice> pMMDevice;
    if (!DeviceUnderTest(pLogger, &pMMDevice)) { return; }

    CStringW szDevice = GetFriendlyName(pMMDevice);
    LPCWSTR name = static_cast<LPCWSTR>(szDevice);
    
    CComPtr<IPropertyStore> pPropertyStore;
    if (!VERIFY_SUCCEEDED(pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore))) { return; }
    PROPVARIANT var; PropVariantInit(&var);
    DoOnExit<PROPVARIANT *> clearVar(PropVariantClear, Success_NonNegative, &var, NULL, L"");
    
    if (!VERIFY_SUCCEEDED(pPropertyStore->GetValue(
        PKEY_Devices_AudioDevice_RawProcessingSupported, &var))) { return; }

    BOOL bRawProcessingSupported = FALSE;
    if (var.vt == VT_BOOL) {
        bRawProcessingSupported = ( var.boolVal ? TRUE : FALSE );
    }
    else if(var.vt == VT_EMPTY) {
        bRawProcessingSupported = FALSE;
    }
    else {
        ERR("Unexpected vartype %u", var.vt);
    }
    
    String supportsRaw = bRawProcessingSupported ? "True" : "False";
    Log::Comment(String().Format(L"[%s] has raw mode: ", name) + supportsRaw);

    UINT eventsExpected = ExpectedOfApoInitEvents(pMMDevice, bRawProcessingSupported);

    CComPtr<IAudioStreamingRender> pAudioStreamingRender;

    if (!VERIFY_SUCCEEDED(
        CreateAudioStreamingRender(
        api_WASAPI,
        pMMDevice,
        pLogger,
        &pAudioStreamingRender
        ))) { return; }

    AudioClientProperties clientProperties = {0};
    clientProperties.eCategory = AudioCategory_Communications;
    clientProperties.bIsOffload = false;
    if (bRawProcessingSupported)
    {
        clientProperties.Options = AUDCLNT_STREAMOPTIONS_RAW;
    }
    clientProperties.cbSize = sizeof(clientProperties);
    
    CComPtr<IAudioStreamingControl> pAudioStreamingControl;
    if (!VERIFY_SUCCEEDED(
        pAudioStreamingRender->QueryInterface(
        __uuidof(IAudioStreamingControl),
        reinterpret_cast<void**>(&pAudioStreamingControl)
        ))) { return; }
            
    CComPtr<IAudioStreamingWasapiControl> pAudioStreamingWasapiControl;
    if (!VERIFY_SUCCEEDED(
         pAudioStreamingControl->GetService(
         __uuidof(IAudioStreamingWasapiControl),
         reinterpret_cast<void**>(&pAudioStreamingWasapiControl)
         ))) { return; }
            
#pragma prefast(suppress: __WARNING_BUFFER_OVERFLOW, "client properties is variable-sized")
    if (!VERIFY_SUCCEEDED(
        pAudioStreamingWasapiControl->SetClientProperties(
        reinterpret_cast<AudioClientProperties*>(&clientProperties)
        ))) { return; }
        
    HwAudioEngineEventLogger mfCoreEventLogger(pLogger);
    if (!VERIFY_SUCCEEDED(mfCoreEventLogger.Initialize(L"APOInit", Microsoft_Windows_Audio, ETW_TRACE_LEVEL_INFORMATION))) { return; }
    // start gathering ETW events
    if (!VERIFY_SUCCEEDED(mfCoreEventLogger.Start())) { return; }
    if (!VERIFY_SUCCEEDED(pAudioStreamingRender->Initialize(NULL))) { return; }
    
    if (!VERIFY_SUCCEEDED(
        pAudioStreamingRender->PlaySineWave(
        1000.0f,
        0.001f,
        Method_TPD,
        1000
    ))) { return; }

    if (!VERIFY_SUCCEEDED(mfCoreEventLogger.Stop())) { return; }

    UINT events = FilterApoEvents(mfCoreEventLogger);

    VERIFY_ARE_EQUAL(eventsExpected, events);
} 

// Added due to previous bug where APO's where not 
// loaded if loopback stream was started before render
void CHapTest::LoopbackBeforeRender() {

    if (!IsWindows8Point1OrGreater()) {
        Log::Result(TestResults::Result::Skipped, L"Test applies only to Windows 8.1 and up");
        return;
    }

    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    CComPtr<IBasicLog> pLogger;
    if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return; }
   
    CComPtr<IMMDevice> pMMDevice;
    if (!DeviceUnderTest(pLogger, &pMMDevice)) { return; }

    UINT eventsExpected = ExpectedOfApoInitEvents(pMMDevice, FALSE);

    // Let's setup the loopback stream
    CComPtr<IAudioStreamingCapture> pAudioStreamingLoopback;	
    if (!VERIFY_SUCCEEDED(
        CreateAudioStreamingCapture(
            api_WASAPI,
            pMMDevice,
            pLogger,
            &pAudioStreamingLoopback
    ))) { return; }

    CComPtr<IAudioStreamingControl> pAudioStreamingControl;
    if (!VERIFY_SUCCEEDED(
        pAudioStreamingLoopback->QueryInterface(
            __uuidof(IAudioStreamingControl),
            reinterpret_cast<void**>(&pAudioStreamingControl)
    ))) { return; }

    CComPtr<IAudioStreamingWasapiControl> pAudioStreamingWasapiControl;
    if (!VERIFY_SUCCEEDED(
        pAudioStreamingControl->GetService(
            __uuidof(IAudioStreamingWasapiControl),
            reinterpret_cast<void**>(&pAudioStreamingWasapiControl)
    ))) { return; }

    if (!VERIFY_SUCCEEDED(
        pAudioStreamingWasapiControl->SetAudioClientInitializeParameters(
            AUDCLNT_STREAMFLAGS_LOOPBACK,
            0, // buffer duration
            0, // periodicity
            GUID_NULL // session
    ))) { return; }

    // Let's setup the render stream
    CComPtr<IAudioStreamingRender> pAudioStreamingRender;
    if (!VERIFY_SUCCEEDED(
        CreateAudioStreamingRender(
        api_WASAPI,
        pMMDevice,
        pLogger,
        &pAudioStreamingRender
        ))) { return; }

    HwAudioEngineEventLogger mfCoreEventLogger(pLogger);
    if (!VERIFY_SUCCEEDED(mfCoreEventLogger.Initialize(L"APOInit", Microsoft_Windows_Audio, ETW_TRACE_LEVEL_INFORMATION))) { return; }
    // start gathering ETW events
    if (!VERIFY_SUCCEEDED(mfCoreEventLogger.Start())) { return; }
    
    // Kick off the loopback capture
    if (!VERIFY_SUCCEEDED(pAudioStreamingLoopback->Initialize(NULL))) { return; }
    if (!VERIFY_SUCCEEDED(pAudioStreamingLoopback->CaptureWithoutBufferAsync())) { return; }

    // Kick of the render
    if (!VERIFY_SUCCEEDED(pAudioStreamingRender->Initialize(NULL))) { return; }
    if (!VERIFY_SUCCEEDED(
        pAudioStreamingRender->PlaySineWave(
        1000.0f,
        0.001f,
        Method_TPD,
        1000
    ))) { return; }

    if (!VERIFY_SUCCEEDED(mfCoreEventLogger.Stop())) { return; }

    // Verify that the apo that was supposed to be loaded got loaded
    UINT events = FilterApoEvents(mfCoreEventLogger);

    VERIFY_ARE_EQUAL(eventsExpected, events);
} 

BOOL HasClsID(_In_ IMMDevice *pMMDevice,
              _In_ const PROPERTYKEY pkey) {

    HRESULT hr;
    CComHeapPtr<WCHAR> pstrId;
    CComPtr<IPolicyConfig> spConfig;
    PROPVARIANT var; PropVariantInit(&var);
    DoOnExit<PROPVARIANT *> clearVar(PropVariantClear, Success_NonNegative, &var, NULL, L"");

    hr = spConfig.CoCreateInstance(__uuidof(PolicyConfig));
    if (hr == E_NOINTERFACE)
    {
        // See if the older interface is supported.  We can assign this to the new interface pointer
        // because we're not using any of the newer functions.  IPolicyConfig is purposely designed
        // this way for backwards compatability.
        Log::Comment(L"Unable to CoCreateInstance() for IPolicyConfig.  Trying Win 10 Threshold IPolicyConfig");
        if (!VERIFY_SUCCEEDED(CoCreateInstance(__uuidof(PolicyConfig), NULL, CLSCTX_ALL, IID_IPolicyConfigWin10Th, (void**)&spConfig)))  { return FALSE; }
        Log::Comment(L"Successfully got Win 10 Threshold IPolicyConfig");
    }
    else if (!VERIFY_SUCCEEDED(hr, L"Could not CoCreateInstance() any supported versions of IPolicyConfig")) { return FALSE; }
    if (!VERIFY_SUCCEEDED(pMMDevice->GetId(&pstrId))) { return FALSE; }
    if (!VERIFY_SUCCEEDED(spConfig->GetPropertyValue(pstrId, true, pkey, &var))) { return FALSE; }

    return (var.vt == VT_LPWSTR);
}

BOOL IsProxyApo(_In_ IMMDevice *pMMDevice,
                _In_ const PROPERTYKEY pkey) {

    HRESULT hr;
    CComHeapPtr<WCHAR> pstrId;
    CComPtr<IPolicyConfig> spConfig;
    PROPVARIANT var; PropVariantInit(&var);
    DoOnExit<PROPVARIANT *> clearVar(PropVariantClear, Success_NonNegative, &var, NULL, L"");

    hr = spConfig.CoCreateInstance(__uuidof(PolicyConfig));
    if (hr == E_NOINTERFACE)
    {
        // See if the older interface is supported.  We can assign this to the new interface pointer
        // because we're not using any of the newer functions.  IPolicyConfig is purposely designed
        // this way for backwards compatability.
        Log::Comment(L"Unable to CoCreateInstance() for IPolicyConfig.  Trying Win 10 Threshold IPolicyConfig");
        if (!VERIFY_SUCCEEDED(CoCreateInstance(__uuidof(PolicyConfig), NULL, CLSCTX_ALL, IID_IPolicyConfigWin10Th, (void**)&spConfig)))  { return FALSE; }
        Log::Comment(L"Successfully got Win 10 Threshold IPolicyConfig");
    }
    else if (!VERIFY_SUCCEEDED(hr, L"Could not CoCreateInstance() any supported versions of IPolicyConfig")) { return FALSE; }
    if (!VERIFY_SUCCEEDED(pMMDevice->GetId(&pstrId))) { return FALSE; }
    if (!VERIFY_SUCCEEDED(spConfig->GetPropertyValue(pstrId, TRUE, pkey, &var))) { return FALSE; }

    return (var.vt != (VT_VECTOR | VT_LPWSTR));
}

CStringW GetFriendlyName(_In_ IMMDevice* pMMDevice) {

    CComPtr<IPropertyStore> pPropertyStore;
    if (!VERIFY_SUCCEEDED(pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore))) { return L"(error)"; }

    PROPVARIANT var; PropVariantInit(&var);
    DoOnExit<PROPVARIANT *> clearVar(PropVariantClear, Success_NonNegative, &var, NULL, L"");
    
    if (!VERIFY_SUCCEEDED(pPropertyStore->GetValue(PKEY_Device_FriendlyName, &var))) { return L"(error)"; }
    if (!VERIFY_ARE_EQUAL(var.vt, VT_LPWSTR)) { return L"(error)"; }

    return var.pwszVal;
}

UINT ExpectedOfApoInitEvents(_In_ IMMDevice *pMMDevice, _In_ BOOL israw) {

    UINT numberofevents = 0; 
    BOOL hasProxyAPO = FALSE;
   
    CStringW szDevice = GetFriendlyName(pMMDevice);
    LPCWSTR name = static_cast<LPCWSTR>(szDevice);

    // only endpoint effects is expected on raw and default
    
    if(HasClsID( pMMDevice, PKEY_FX_EndpointEffectClsid))
    {
        // even that it has a clsid it can be proxy apo
        if(IsProxyApo( pMMDevice, PKEY_EFX_ProcessingModes_Supported_For_Streaming)) {
            Log::Comment(String().Format(L"[%s] has endpoint effect (EFX) but is a proxy APO", name));
            hasProxyAPO = TRUE;
        }
        else { // APO is not a proxy
            Log::Comment(String().Format(L"[%s] has usermode endpoint effect (EFX)", name));
            numberofevents++;
        }
    }
    else {
        Log::Comment(String().Format(L"[%s] has no endpoint effect (EFX)", name));
    }

    // if not raw then check stream and mode effects also
    if(!israw) {    
         // check for mode effects
         if (HasClsID(pMMDevice, PKEY_FX_ModeEffectClsid)) {
             // even that it has a clsid it can be proxy apo
             if(IsProxyApo( pMMDevice, PKEY_MFX_ProcessingModes_Supported_For_Streaming)) {
                 Log::Comment(String().Format(L"[%s] has mode effect (MFX) but is a proxy APO", name));
                 hasProxyAPO = TRUE;
             }
             else { // APO is not a proxy
                 Log::Comment(String().Format(L"[%s] has usermode mode effect (MFX)", name));
                 numberofevents++;
             }
         }
         else {
             Log::Comment(String().Format(L"[%s] has no mode effect (MFX)", name));
         }

         // check for stream effects
         if(HasClsID( pMMDevice, PKEY_FX_StreamEffectClsid)) {
             // even that it has a clsid it can be proxy apo
             if(IsProxyApo( pMMDevice, PKEY_SFX_ProcessingModes_Supported_For_Streaming)) {
                 Log::Result(TestResults::Result::Failed, L"Stream effect must support streaming");
             }
             else { // APO is not a proxy
                  Log::Comment(String().Format(L"[%s] has usermode stream effect (SFX)", name));
                  numberofevents++;
             }
         }
         else {
             Log::Comment(String().Format(L"[%s] has no stream effect (SFX)", name));
         }

         // if no S/M/E effects and there is no proxy APOS found let's check for LFX/GFX
         if (numberofevents == 0 && !hasProxyAPO) {
             // check for LFX
             if (HasClsID( pMMDevice, PKEY_FX_PreMixEffectClsid)) {
                 Log::Comment(String().Format(L"[%s] has a local effect (LFX)", name));
                 numberofevents++;
             }
             else {
                 Log::Comment(String().Format(L"[%s] has no local effect (LFX)", name));
             }
             // check for GFX
             if (HasClsID( pMMDevice, PKEY_FX_PostMixEffectClsid)) {
                 Log::Comment(String().Format(L"[%s] has a global effect (GFX)", name));
                 numberofevents++;
             }
             else {
                 Log::Comment(String().Format(L"[%s] has no global effect (GFX)", name));
             }

         }
    }
    
    return numberofevents;
}

UINT FilterApoEvents(_In_ HwAudioEngineEventLogger mfCoreEventLogger) {

    UINT events = 0;
    UINT countEvents = 0;

    if (!VERIFY_SUCCEEDED(mfCoreEventLogger.GetEventCount(&countEvents))) { return 0; }

    USHORT cbUserData = 0;

    for (UINT i = 0; i < countEvents; i++) 
    {
        CComPtr<EtwLogger::IEtwEventHelper> spEtwEventHelper;
        if (!VERIFY_SUCCEEDED(mfCoreEventLogger.GetEvent(i, &spEtwEventHelper))) { continue; }

        GUID gProviderId(GUID_NULL);
        if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetHeaderProviderId(&gProviderId))) { continue; }

        USHORT etwEventId = 0;
        if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetHeaderEventDescriptorId(&etwEventId))) { continue; }

        if (IsEqualGUID(gProviderId, Microsoft_Windows_Audio) && etwEventId == APOEventId)
        {
            LPOLESTR guid;
            if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetUserDataSize(&cbUserData))) { continue; }
            if (!VERIFY_ARE_EQUAL(sizeof(SYSTEM_EFFECT_APO_INITIALIZED), cbUserData)) { continue; }
            SYSTEM_EFFECT_APO_INITIALIZED apoEvent = {0, 0, 0};
            if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetUserData((LPBYTE)&apoEvent, cbUserData))) { continue; }

            if (!VERIFY_SUCCEEDED(StringFromCLSID(apoEvent.APOCLSID, &guid))) { continue; }
            Log::Comment(String().Format(L"APO with APOCLSID: %s is loaded", guid));

            if (IsEqualGUID(apoEvent.AudioSignalProcessingMode, AUDIO_SIGNALPROCESSINGMODE_DEFAULT)) {
                Log::Comment(String().Format(L"AudioSignalProcessingMode = AUDIO_SIGNALPROCESSINGMODE_DEFAULT"));
            }
            else {
                Log::Comment(String().Format(L"AudioSignalProcessingMode is not AUDIO_SIGNALPROCESSINGMODE_DEFAULT"));
            }

            String discoveryOnly = apoEvent.InitializeForDiscoveryOnly ? "True" : "False";
            Log::Comment(String().Format(L"Initialize For Discovery Only: " + discoveryOnly));

            events++;
            CoTaskMemFree(guid);
        }
    }
    
    return events;
}

