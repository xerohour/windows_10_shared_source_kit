//*****************************************************************************
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// File:         EffectsDiscoveryTestCore.cpp
//
//*****************************************************************************
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <atlbase.h>
#include <objbase.h>
#include <initguid.h>
#include <comdef.h>

#include "EffectsDiscoveryCPPTestCore.h"
static const PROPERTYKEY MFPKEY_CORR_LOUDNESS_EQUALIZATION_ON    = { { 0xfc52a749, 0x4be9, 0x4510, { 0x89, 0x6e, 0x96, 0x6b, 0xa6, 0x52, 0x59, 0x80 } }, PID_FIRST_USABLE }; 
//
// TAEF
//

#include <WexTestClass.h>
#include <wexstring.h>
#include <collection.h>

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;
#define CASE_RETURN(prefix, token) case prefix ## token: return L ## #token




// Constructor/Destructor/Initializer
CEffectsDiscoveryCPPTestCore::CEffectsDiscoveryCPPTestCore()
{
    m_presentCategories = ref new Platform::Collections::Vector<Windows::Media::Render::AudioRenderCategory>();
}

CEffectsDiscoveryCPPTestCore::~CEffectsDiscoveryCPPTestCore()
{}

bool CEffectsDiscoveryCPPTestCore::NameContainsHF(const wchar_t* pDeviceName)
{
    if (NULL == wcsstr(pDeviceName,L"Hands-Free"))
    {
        return false;
    }
    return true;
}


void CEffectsDiscoveryCPPTestCore::EnumerateCaptureDevices(BOOL bVideo, DeviceInformationCollection^* ppList)
{
    Platform::String^ deviceSelectorString;
    task<DeviceInformationCollection^> enumerateTask;
    VERIFY_IS_NOT_NULL(ppList, L"Location to fill DeviceInformationCollection");
    try
    {   
        auto propertiesToRetrieve =  ref new Platform::Collections::Vector<Platform::String^>();                   
        propertiesToRetrieve->Append("System.Devices.AudioDevice.RawProcessingSupported");

        //Retrieve value for PKEY_AudioEndpoint_GUID
        //This will help map Software Device (WinRT) to IMMDevice Endpoint
        propertiesToRetrieve->Append(L"{1da5d803-d492-4edd-8c23-e0c0ffee7f0e} 4");

        deviceSelectorString = (bVideo) ? MediaDevice::GetVideoCaptureSelector() : MediaDevice::GetAudioCaptureSelector();        
        enumerateTask = create_task(DeviceInformation::FindAllAsync(deviceSelectorString, propertiesToRetrieve));  
        enumerateTask.then([this, &ppList] (task<DeviceInformationCollection^> t)
        {
            try
            {
                *ppList = t.get();
            }
            catch(Platform::Exception^ e)
            {
                LogException(e);
            }
        }).wait();
    }
    catch(Platform::Exception^ e)
    {
        LogException(e);
    }
}

void CEffectsDiscoveryCPPTestCore::EnumerateRenderDevices( DeviceInformationCollection^* ppList)
{
    Platform::String^ deviceSelectorString;
    task<DeviceInformationCollection^> enumerateTask;
    VERIFY_IS_NOT_NULL(ppList, L"Location to fill DeviceInformationCollection");
    try
    {   
        auto propertiesToRetrieve =  ref new Platform::Collections::Vector<Platform::String^>();                   
        propertiesToRetrieve->Append("System.Devices.AudioDevice.RawProcessingSupported");

        //Retrieve value for PKEY_AudioEndpoint_GUID
        //This will help map Software Device (WinRT) to IMMDevice Endpoint
        propertiesToRetrieve->Append(L"{1da5d803-d492-4edd-8c23-e0c0ffee7f0e}4");


        deviceSelectorString =  MediaDevice::GetAudioRenderSelector();        
        enumerateTask = create_task(DeviceInformation::FindAllAsync(deviceSelectorString, propertiesToRetrieve));  
        enumerateTask.then([this, &ppList] (task<DeviceInformationCollection^> t)
        {
            try
            {
                *ppList = t.get();
            }
            catch(Platform::Exception^ e)
            {
                LogException(e);
            }
        }).wait();
    }
    catch(Platform::Exception^ e)
    {
        LogException(e);
    }
}

bool CEffectsDiscoveryCPPTestCore::ReadRawSupportSettings(Platform::String ^audioDeviceID)
{
    bool bDeviceSupportsRaw = false;
    try
    {   
        if (audioDeviceID != nullptr)
        {
            auto propertiesToRetrieve =  ref new Platform::Collections::Vector<Platform::String^>();           

            propertiesToRetrieve->Append("System.Devices.AudioDevice.RawProcessingSupported");            
            // read property store to see if the device supports a RAW processing mode and start audio recording accordingly            
            auto testRaw = create_task(Windows::Devices::Enumeration::DeviceInformation::CreateFromIdAsync(audioDeviceID, propertiesToRetrieve));
            testRaw.then([this, &bDeviceSupportsRaw] (Windows::Devices::Enumeration::DeviceInformation^ deviceInformation)
            {
                auto obj = deviceInformation->Properties->Lookup("System.Devices.AudioDevice.RawProcessingSupported");            
                if (obj)
                {                    
                    bDeviceSupportsRaw = obj->Equals(true);              
                }

            }).wait();  
            return bDeviceSupportsRaw;
        }
    }
    catch (Platform::Exception^ e)
    {
        LogException(e);      
    }
    return bDeviceSupportsRaw;
}

IVectorView<AudioEffect^>^ CEffectsDiscoveryCPPTestCore::ListEffectsCapture(
    Windows::Media::Effects::AudioCaptureEffectsManager ^manager,
    Windows::Devices::Enumeration::DeviceInformation ^device,
    Windows::Media::Capture::MediaCategory category,
    Windows::Media::AudioProcessing mode    
    ) {
        IVectorView<AudioEffect^>^ list = nullptr;
        try
        {    
            list = manager->GetAudioCaptureEffects();           
            Platform::String ^listString = L"{";      
            for (UINT fx = 0; fx < list->Size; fx++) {
                auto effect = list->GetAt(fx);

                if (fx > 0) {
                    listString += ref new Platform::String(L", ");
                }

                listString += ref new Platform::String(StringFromAudioEffectType(effect->AudioEffectType)) ;
            } // for each effect    

            listString += ref new Platform::String(L"}");

            Log::Comment(WEX::Common::String().Format(L"%s %s %s: %s", 
                device->Name->Data(),
                StringFromCaptureMediaCategory(category),
                StringFromAudioProcessing(mode),
                listString->Data()));
            return list; 
        }
        catch (Platform::Exception^ e)
        {
            LogException(e);      
        }
        return list; 
}

IVectorView<AudioEffect^>^ CEffectsDiscoveryCPPTestCore::ListEffectsRender(
    Windows::Media::Effects::AudioRenderEffectsManager ^manager,
    Windows::Devices::Enumeration::DeviceInformation ^device,
    Windows::Media::Render::AudioRenderCategory category,
    Windows::Media::AudioProcessing mode    
    ) {
        IVectorView<AudioEffect^>^ list = nullptr;
        try
        {
            list = manager->GetAudioRenderEffects();    
            Platform::String ^listString = L"{";      
            for (UINT fx = 0; fx < list->Size; fx++) {
                auto effect = list->GetAt(fx);

                if (fx > 0) {
                    listString += ref new Platform::String(L", ");
                }        
                listString += ref new Platform::String(StringFromAudioEffectType(effect->AudioEffectType)) ;
            } // for each effect    

            listString += ref new Platform::String(L"}");

            Log::Comment(WEX::Common::String().Format(L"%s %s %s: %s", 
                device->Name->Data(),
                StringFromAudioRenderCategory(category),
                StringFromAudioProcessing(mode),
                listString->Data()));
            return list; 
        }
        catch (Platform::Exception^ e)
        {
            LogException(e);      
        }
        return list; 
}



void CEffectsDiscoveryCPPTestCore::VerifyEffectListReturned(
    IVectorView<AudioEffect^> ^list,
    Windows::Media::AudioProcessing mode
    ){    
        try
        {        
            for (UINT fx = 0; fx < list->Size; fx++) {
                auto effect = list->GetAt(fx);
                if ( mode == Windows::Media::AudioProcessing::Raw  && (effect->AudioEffectType != Windows::Media::Effects::AudioEffectType::SpeakerCompensation && effect->AudioEffectType != Windows::Media::Effects::AudioEffectType::SpeakerProtection))
                {
                    LOG_ERROR(L"In Raw mode effects list contains an enhancement of unacceptable type: %s",StringFromAudioEffectType(effect->AudioEffectType));                  
                }

            }
        }
        catch (Platform::Exception^ e)
        {
            LogException(e);       
        }
}

void CEffectsDiscoveryCPPTestCore::VerifyEffectListOnDisable(
    Platform::Collections::Vector<AudioEffectType> ^list    
    ){
        try
        {        
            for (UINT fx = 0; fx < list->Size; fx++) {
                auto effect = list->GetAt(fx);
                if ( effect != Windows::Media::Effects::AudioEffectType::SpeakerCompensation && effect != Windows::Media::Effects::AudioEffectType::SpeakerProtection )
                {                
                    LOG_ERROR(L"The effects list contains an enhancement of unacceptable type: %s. All enhancements other than endpoint effects should be disabled when enhancements are disabled from UI.",StringFromAudioEffectType(effect));               
                }
            }
        }
        catch (Platform::Exception^ e)
        {
            LogException(e);       
        }
}

void CEffectsDiscoveryCPPTestCore::VerifyEffectListOnDisable(
    IVectorView<AudioEffect^> ^list    
    ){
        try
        {        
            for (UINT fx = 0; fx < list->Size; fx++) {
                auto effect = list->GetAt(fx);
                if ( effect->AudioEffectType != Windows::Media::Effects::AudioEffectType::SpeakerCompensation && effect->AudioEffectType != Windows::Media::Effects::AudioEffectType::SpeakerProtection )
                {                
                    LOG_ERROR(L"The effects list contains an enhancement of unacceptable type: %s. All enhancements other than endpoint effects should be disabled when enhancements are disabled from UI.",StringFromAudioEffectType(effect->AudioEffectType));               
                }
            }
        }
        catch (Platform::Exception^ e)
        {
            LogException(e);       
        }
}

void CEffectsDiscoveryCPPTestCore::VerifyRawSupport(DeviceInformation ^pDevice)
{
    bool bDeviceSupportsRaw = ReadRawSupportSettings(pDevice->Id);
    if (bDeviceSupportsRaw)
    {
        Log::Comment(L"Device Supports Raw");        
    }
    else
    {
#ifndef BUILD_FOR_MOBILECORE 
        if (!NameContainsHF(pDevice->Name->Data()))
        {
            Log::Error(L"Device does not support raw");   
        }
#else
        // Phone is expected to not support raw
        if (NameContainsHF(pDevice->Name->Data()))
        {   
            Log::Error(L"Device supports raw and is not expected to"); 
        }
#endif
    }
}

void CEffectsDiscoveryCPPTestCore::LogException(Platform::Exception^ e)
{      
    Log::Error((LPCWSTR)e->Message->Data());    
    Log::Error(WEX::Common::String().Format(L"HR = 0x%.8X", e->HResult));   

}


bool CEffectsDiscoveryCPPTestCore::SetDisableSystemEffectsSetting( bool bDisableSysFx, EDataFlow flow, LPCWSTR  deviceId, CPropVariant *storevar)
{
    //
    //In this method VERIFY will not throw.
    //
    DisableVerifyExceptions disable;

    HRESULT hr = S_OK;
    CComPtr<IMMEndpointInternal> spMMEndpointInternal;
    CComPtr<IPropertyStore> spFXPropertyStore;
    CComPtr<IPropertyStore> spEndpointStore;
    CPropVariant devicevar;    
    CPropVariant var;
    CComPtr<IMMDeviceCollection> pIMMEndpoints;
    CComPtr<IMMDeviceEnumerator> spEnumerator;
    CComPtr<IMMDevice> pIMMDevice;
    hr = spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
    UINT nEndpoints = 0;
    LPWSTR pstrId = NULL;
    CComPtr<IPolicyConfig>       spConfig;

    if (!VERIFY_SUCCEEDED(hr = spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator))))
    {
        return false;
    }

    if(!VERIFY_SUCCEEDED(hr = spEnumerator->EnumAudioEndpoints(
        flow,
        DEVICE_STATE_ACTIVE,
        &pIMMEndpoints)
        ))
    {
        return false;
    }

    if(!VERIFY_SUCCEEDED(hr = pIMMEndpoints->GetCount(&nEndpoints)))
    {
        return false;
    }

    bool bMMDeviceFound = false;
    for (UINT i = 0; i < nEndpoints; i++)
    {

        if(!VERIFY_SUCCEEDED(hr = pIMMEndpoints->Item(i, &pIMMDevice)))
        {
            return false;
        }

        if(!VERIFY_SUCCEEDED(hr = pIMMDevice->OpenPropertyStore(STGM_READWRITE, &spEndpointStore)))
        {
            return false;
        }

        if(!VERIFY_SUCCEEDED(hr = spEndpointStore->GetValue(PKEY_AudioEndpoint_GUID, &devicevar)))
        {
            return false;
        }

        switch(devicevar.vt)
        {
        case VT_LPWSTR:
            if (wcscmp(devicevar.pwszVal, deviceId) == 0)
            {
                bMMDeviceFound = true;
            }   
            break;
        default:
            LOG_ERROR(L"Type of PKEY_AudioEndpoint_GUID in PropertyStore is not one of accepable types: VT_LPWSTR. The type is:%u",devicevar.vt);
            break;                  

        }     
        if(bMMDeviceFound == true)
        {
            break;
        }
        else
        {
            pIMMDevice.Release();
        }
    }

    if (!VERIFY_IS_NOT_NULL(pIMMDevice) )
    {

        return false;
    }

    if (!VERIFY_SUCCEEDED(hr = spConfig.CoCreateInstance(__uuidof(PolicyConfig))))
    {
        return false;
    }

    if(!VERIFY_SUCCEEDED(hr = pIMMDevice->GetId(&pstrId)))
    {
        return false;
    }
    VERIFY_SUCCEEDED(hr = spConfig->GetPropertyValue(pstrId, true, PKEY_AudioEndpoint_Disable_SysFx, storevar));            
    CPropVariant storevarlocal = *storevar;
    switch(storevarlocal.vt)
    {
    case VT_EMPTY:
        break;
    case VT_UI4:
        switch(storevarlocal.ulVal)
        {
        case ENDPOINT_SYSFX_ENABLED:
        case ENDPOINT_SYSFX_DISABLED:
            break;
        default:
            LOG_ERROR(L"Value of PKEY_AudioEndpoint_Disable_SysFx is not acceptable.Acceptable values are  ENDPOINT_SYSFX_ENABLED or ENDPOINT_SYSFX_DISABLED.The value is:%u",storevarlocal.ulVal);
            break;
        }                     
        break;
    default:
        LOG_ERROR(L"Type in PropertyStore for PKEY_AudioEndpoint_Disable_SysFx is not one of acceptable values:VT_EMPTY,VT_UI4. The type is:%u",storevarlocal.vt);
        break;                  

    }      

    if (storevarlocal.ulVal == ENDPOINT_SYSFX_DISABLED)
    {
        LOG_OUTPUT(L"The enhancements are already disabled. No notifications will be triggered.");
        return true;
    }
    var.vt = VT_UI4;
    var.ulVal = ( bDisableSysFx ? ENDPOINT_SYSFX_DISABLED : ENDPOINT_SYSFX_ENABLED );
    VERIFY_SUCCEEDED(hr = spConfig->SetPropertyValue(pstrId, true, PKEY_AudioEndpoint_Disable_SysFx, &var));       

    LOG_OUTPUT(L"Set PKEY_AudioEndpoint_Disable_SysFx to %u", var.ulVal );   

    return true;      
}

bool CEffectsDiscoveryCPPTestCore::RestoreSystemEffectsSetting(CPropVariant storevar, EDataFlow flow, LPCWSTR  deviceId)
{   
    //
    //In this method VERIFY will not throw.
    //
    DisableVerifyExceptions disable;

    HRESULT hr = S_OK;
    CComPtr<IMMEndpointInternal> spMMEndpointInternal;
    CComPtr<IPropertyStore> spFXPropertyStore;
    CComPtr<IPropertyStore> spEndpointStore;
    CPropVariant devicevar;    
    CComPtr<IMMDeviceCollection> pIMMEndpoints;
    CComPtr<IMMDeviceEnumerator> spEnumerator;
    CComPtr<IMMDevice> pIMMDevice;
    hr = spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
    UINT nEndpoints = 0;
    LPWSTR pstrId = NULL;
    CComPtr<IPolicyConfig>       spConfig;

    if (!VERIFY_SUCCEEDED(hr = spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator))))
    {
        return false;
    }


    if(!VERIFY_SUCCEEDED(hr = spEnumerator->EnumAudioEndpoints(
        flow,
        DEVICE_STATE_ACTIVE,
        &pIMMEndpoints)
        ))
    {
        return false;
    }

    if(!VERIFY_SUCCEEDED(hr = pIMMEndpoints->GetCount(&nEndpoints)))
    {
        return false;
    }    
    bool bMMDeviceFound = false;
    for (UINT i = 0; i < nEndpoints; i++)
    {

        if(!VERIFY_SUCCEEDED(hr = pIMMEndpoints->Item(i, &pIMMDevice)))
        {
            return false;
        }

        if(!VERIFY_SUCCEEDED(hr = pIMMDevice->OpenPropertyStore(STGM_READ, &spEndpointStore)))
        {
            return false;
        }

        if(!VERIFY_SUCCEEDED(hr = spEndpointStore->GetValue(PKEY_AudioEndpoint_GUID, &devicevar)))
        {
            return false;
        }

        switch(devicevar.vt)
        {
        case VT_EMPTY:
            break;
        case VT_LPWSTR:
            if (wcscmp(devicevar.pwszVal, deviceId) == 0)
            {
                bMMDeviceFound = true;
            }   
            break;
        default:
            LOG_ERROR(L"Type of PKEY_AudioEndpoint_GUID in PropertyStore is not one of accepable types:VT_EMPTY, VT_LPWSTR. The type is:%u",devicevar.vt);
            break;                  

        }     
        if(bMMDeviceFound == true)
        {
            break;
        }
        else
        {
            pIMMDevice.Release();
        }
    }
    if (!VERIFY_IS_NOT_NULL(pIMMDevice) )
    {
        return false;
    }

    if (!VERIFY_SUCCEEDED(hr = spConfig.CoCreateInstance(__uuidof(PolicyConfig))))
    {
        return false;
    }
    if(!VERIFY_SUCCEEDED(hr = pIMMDevice->GetId(&pstrId)))
    {
        return false;
    }
    VERIFY_SUCCEEDED(hr = spConfig->SetPropertyValue(pstrId, true, PKEY_AudioEndpoint_Disable_SysFx, &storevar));       

    LOG_OUTPUT(L"Set PKEY_AudioEndpoint_Disable_SysFx to %u", storevar.ulVal );

    return true;
}


void  CEffectsDiscoveryCPPTestCore::ListEffectsAndVerifyCapture(
    Windows::Media::Effects::AudioCaptureEffectsManager ^manager,
    Windows::Devices::Enumeration::DeviceInformation ^device,
    Windows::Media::Capture::MediaCategory category,
    Windows::Media::AudioProcessing mode 
    )
{   
    IVectorView<AudioEffect^> ^list = ListEffectsCapture(manager, device, category, mode);    
    VerifyEffectListReturned(list, mode);      
}

void  CEffectsDiscoveryCPPTestCore::ListEffectsAndVerifyRender(
    Windows::Media::Effects::AudioRenderEffectsManager ^manager,
    Windows::Devices::Enumeration::DeviceInformation ^device,
    Windows::Media::Render::AudioRenderCategory category,
    Windows::Media::AudioProcessing mode 
    )
{   
    IVectorView<AudioEffect^> ^list = ListEffectsRender(manager, device, category, mode);    
    VerifyEffectListReturned(list, mode);
}


bool CEffectsDiscoveryCPPTestCore::CheckforPresenceOfWMALFXGFXDSP(EDataFlow flow, LPCWSTR  deviceId)
{

    //
    //In this method VERIFY will not throw.
    //
    DisableVerifyExceptions disable;    

    CComPtr<IMMDevice> pIMMDevice;     
    LPWSTR pstrId = NULL;      

    if (!GetIMMDevice(deviceId, flow, &pIMMDevice))
    {
        return false;
    }

    if(!VERIFY_SUCCEEDED(pIMMDevice->GetId(&pstrId)))
    {
        return false;
    }

    bool bWMALFXGFXUsedInStream = LookForWMALFXGFX(pstrId, PKEY_FX_StreamEffectClsid);
    bool bWMALFXGFXUsedInMode   = LookForWMALFXGFX(pstrId, PKEY_FX_ModeEffectClsid);
    bool bWMALFXGFXUsedInEndPoint = LookForWMALFXGFX(pstrId, PKEY_FX_EndpointEffectClsid);

    if ( bWMALFXGFXUsedInStream || bWMALFXGFXUsedInEndPoint  || bWMALFXGFXUsedInMode)
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool CEffectsDiscoveryCPPTestCore::LookForWMALFXGFX(LPWSTR pstrId, REFPROPERTYKEY key)
{    
    CComPtr<IPolicyConfig> spConfig;
    bool bWMALFXGFXUsed = false;
    CPropVariant storevar;

    if (!VERIFY_SUCCEEDED(spConfig.CoCreateInstance(__uuidof(PolicyConfig))))
    {
        return false;
    }

    VERIFY_SUCCEEDED(spConfig->GetPropertyValue(pstrId, true, key, &storevar));  

    switch(storevar.vt)
    {
    case VT_EMPTY:
        break;
    case VT_LPWSTR:
        if ((_wcsicmp(storevar.pwszVal, L"{62dc1a93-ae24-464c-a43e-452f824c4250}") == 0) || (_wcsicmp(storevar.pwszVal, L"{637c490d-eee3-4c0a-973f-371958802da2}") == 0))
        {				
            bWMALFXGFXUsed = true;
        }     
        break;
    default:
        LOG_ERROR(L"Type of key in PropertyStore is not one of accepable types:VT_EMPTY, VT_LPWSTR. The type is:%u",storevar.vt);
        break;                 

    } 
    return bWMALFXGFXUsed;
}

bool CEffectsDiscoveryCPPTestCore::ChangeLoudnessEq(EDataFlow flow, LPCWSTR  deviceId, CPropVariant *storevar)
{
    //
    //In this method VERIFY will not throw.
    //
    DisableVerifyExceptions disable;


    CComPtr<IPropertyStore> spEndpointStore;   
    CComPtr<IPolicyConfig> spConfig;
    CComPtr<IMMDevice> pIMMDevice;
    LPWSTR pstrId = NULL;    
    PROPERTYKEY keyLoudness;
    DWORD dwChannelMask = 0;
    CPropVariant devicevar;


    if (!GetIMMDevice(deviceId, flow, &pIMMDevice))
    {
        return false;
    }


    if (!VERIFY_SUCCEEDED(spConfig.CoCreateInstance(__uuidof(PolicyConfig))))
    {
        return false;
    }

    if(!VERIFY_SUCCEEDED(pIMMDevice->GetId(&pstrId)))
    {
        return false;
    }

    if(!VERIFY_SUCCEEDED(pIMMDevice->OpenPropertyStore(STGM_READWRITE, &spEndpointStore)))
    {
        return false;
    }

    if(!VERIFY_SUCCEEDED(spEndpointStore->GetValue(PKEY_AudioEngine_DeviceFormat,  &devicevar)))
    {
        return false;
    }                
    switch(devicevar.vt)
    {
    case VT_BLOB:
        if (devicevar.blob.cbSize >= sizeof(WAVEFORMATEXTENSIBLE))
        {
            dwChannelMask = ((WAVEFORMATEXTENSIBLE*)devicevar.blob.pBlobData)->dwChannelMask;
        }   
        break;
    default:
        LOG_ERROR(L"Type of PKEY_AudioEngine_DeviceFormat in PropertyStore is not one of accepable types: VT_BLOB. The type is:%u",devicevar.vt);
        break;                  

    } 

    keyLoudness.fmtid = MFPKEY_CORR_LOUDNESS_EQUALIZATION_ON.fmtid;
    keyLoudness.pid = dwChannelMask;

    VERIFY_SUCCEEDED(spConfig->GetPropertyValue(pstrId, true, keyLoudness, storevar));  
    CPropVariant tempvar = *storevar;
    CPropVariant var;
    var.vt = VT_BOOL;
    var.boolVal = VARIANT_TRUE;

    switch(tempvar.vt)
    {
    case VT_EMPTY:
        break;
    case VT_BOOL:
        if (tempvar.boolVal == VARIANT_TRUE)
        {
            LOG_OUTPUT(L"Loudness Eq is currently on will set it to false");
            var.boolVal =  VARIANT_FALSE;
        }
        else if (tempvar.boolVal == VARIANT_FALSE)
        {
            LOG_OUTPUT(L"Loudness Eq is currently off will set it to true");
            var.boolVal = VARIANT_TRUE;
        }
        else
        {
            LOG_ERROR(L"Loudness Eq is currently set to %u", tempvar.boolVal);                
        }
        break;
    default:
        LOG_ERROR(L"Type of LoudnessEq in PropertyStore is not one of accepable types:VT_EMPTY, VT_BOOL. The type is:%u",tempvar.vt);
        break;  

    }     

    VERIFY_SUCCEEDED(spConfig->SetPropertyValue(pstrId, true, keyLoudness, &var));   

    LOG_OUTPUT(L"Set LoudnessEq set to to %u", var.boolVal );   

    return true; 

}
bool CEffectsDiscoveryCPPTestCore::RestoreLoudnessEq(CPropVariant storevar, EDataFlow flow, LPCWSTR  deviceId)
{
    //
    //In this method VERIFY will not throw.
    //
    DisableVerifyExceptions disable;



    CComPtr<IPropertyStore> spEndpointStore;
    CComPtr<IMMDevice> pIMMDevice;            
    LPWSTR pstrId = NULL;    
    PROPERTYKEY keyLoudness;
    CComPtr<IPolicyConfig> spConfig;
    DWORD dwChannelMask = 0;   
    CPropVariant devicevar;


    if (!GetIMMDevice(deviceId, flow, &pIMMDevice))
    {
        return false;
    }

    if (!VERIFY_SUCCEEDED(spConfig.CoCreateInstance(__uuidof(PolicyConfig))))
    {
        return false;
    }

    if(!VERIFY_SUCCEEDED(pIMMDevice->GetId(&pstrId)))
    {
        return false;
    }


    if(!VERIFY_SUCCEEDED(pIMMDevice->OpenPropertyStore(STGM_READWRITE, &spEndpointStore)))
    {
        return false;
    }

    if(!VERIFY_SUCCEEDED(spEndpointStore->GetValue(PKEY_AudioEngine_DeviceFormat,  &devicevar)))
    {
        return false;
    }                
    switch(devicevar.vt)
    {
    case VT_BLOB:
        if (devicevar.blob.cbSize >= sizeof(WAVEFORMATEXTENSIBLE))
        {
            if (((WAVEFORMATEXTENSIBLE*)devicevar.blob.pBlobData)->Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
            {
                dwChannelMask = ((WAVEFORMATEXTENSIBLE*)devicevar.blob.pBlobData)->dwChannelMask;
            }
        }   
        break;
    default:
        LOG_ERROR(L"Type of PKEY_AudioEngine_DeviceFormat in PropertyStore is not one of accepable types: VT_BLOB. The type is:%u",devicevar.vt);
        break;                  

    } 

    keyLoudness.fmtid = MFPKEY_CORR_LOUDNESS_EQUALIZATION_ON.fmtid;
    keyLoudness.pid = dwChannelMask;

    VERIFY_SUCCEEDED(spConfig->SetPropertyValue(pstrId, true, keyLoudness, &storevar));   

    LOG_OUTPUT(L"Restoring LoudnessEq set to to %u", storevar.boolVal );   

    return true; 

}

bool CEffectsDiscoveryCPPTestCore::GetIMMDevice(LPCWSTR  deviceId, EDataFlow flow, IMMDevice **ppIMMDevice)
{
    //
    //In this method VERIFY will not throw.
    //
    DisableVerifyExceptions disable;

    HRESULT hr = S_OK;
    CComPtr<IMMEndpointInternal> spMMEndpointInternal;
    CComPtr<IPropertyStore> spFXPropertyStore;
    CComPtr<IPropertyStore> spEndpointStore;
    CPropVariant devicevar;    
    CPropVariant var;
    CComPtr<IMMDeviceCollection> pIMMEndpoints;
    CComPtr<IMMDeviceEnumerator> spEnumerator;
    UINT i;

    hr = spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
    UINT nEndpoints = 0;

    if (!VERIFY_SUCCEEDED(hr = spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator))))
    {
        return false;
    }

    if(!VERIFY_SUCCEEDED(hr = spEnumerator->EnumAudioEndpoints(
        flow,
        DEVICE_STATE_ACTIVE,
        &pIMMEndpoints)
        ))
    {
        return false;
    }

    if(!VERIFY_SUCCEEDED(hr = pIMMEndpoints->GetCount(&nEndpoints)))
    {
        return false;
    }

    bool bMMDeviceFound = false;
    for (i = 0; i < nEndpoints; i++)
    {

        if(!VERIFY_SUCCEEDED(hr = pIMMEndpoints->Item(i, ppIMMDevice)))
        {
            return false;
        }

        if(!VERIFY_SUCCEEDED(hr = (*ppIMMDevice)->OpenPropertyStore(STGM_READWRITE, &spEndpointStore)))
        {
            return false;
        }

        if(!VERIFY_SUCCEEDED(hr = spEndpointStore->GetValue(PKEY_AudioEndpoint_GUID, &devicevar)))
        {
            return false;
        }

        switch(devicevar.vt)
        {
        case VT_LPWSTR:
            if (wcscmp(devicevar.pwszVal, deviceId) == 0)
            {
                bMMDeviceFound = true;
            }   
            break;
        default:
            LOG_ERROR(L"Type of PKEY_AudioEndpoint_GUID in PropertyStore is not one of accepable types: VT_LPWSTR. The type is:%u",devicevar.vt);
            break;                  

        }     
        if(bMMDeviceFound == true)
        {
            break;
        }
        else
        {
            (*ppIMMDevice)->Release();
        }
    }

    if (!VERIFY_IS_NOT_NULL(*ppIMMDevice) )
    {

        return false;
    }   

    return true;
}


void CEffectsDiscoveryCPPTestCore::ProcessRenderEffectsList(const wchar_t* effectsList, CPropVariant var, Verification eVerType)
{
    Platform::Collections::Vector<AudioEffectType> ^effects=ref new Platform::Collections::Vector<AudioEffectType>();
    Windows::Media::Render::AudioRenderCategory category = Windows::Media::Render::AudioRenderCategory::Other;
    Windows::Media::AudioProcessing mode = Windows::Media::AudioProcessing::Default;


    LPWSTR localEffectsList = new wchar_t [ wcslen(effectsList) + 1 ];
    LPWSTR token = NULL;		
    LPWSTR tokenEffect = NULL;

    wcscpy(localEffectsList, effectsList);
    token = wcstok( localEffectsList, L",");
    int tokenIndex = 0;
    while( token != NULL )	
    {
        switch(tokenIndex)
        {
        case 0:						
            LOG_OUTPUT(L" audioDevice: %s\n", token );
            break;
        case 1:			
            category = AudioRenderCategoryFromString(token);
            LOG_OUTPUT(L" category: %s\n", StringFromAudioRenderCategory(category) );
            break;
        case 2:
            mode = AudioProcessingFromString(token);
            LOG_OUTPUT(L" mode: %s\n", StringFromAudioProcessing(mode) );
            break;
        case 3:						
            tokenEffect = wcstok( token, L":");
            while(tokenEffect != NULL)
            {
                LOG_OUTPUT(L" effects: %s\n", tokenEffect );
                if (wcscmp(tokenEffect, L"no effects") != 0)
                {
                    effects->Append(AudioEffectTypeFromString(tokenEffect));
                }
                tokenEffect = wcstok( NULL, L":" );				
            }
            if (eVerType == VerifyLoudnessEq)
            {
                VerifyNotificationsLoudnessEq(effects, category, mode, var); 
                if (mode == Windows::Media::AudioProcessing::Default)
                {
                    AddtoNotificationRenderCategory(category);
                }
            }
            else
            {
                VerifyEffectListOnDisable(effects);
            }
            break;
        default:
            LOG_OUTPUT(L" Unexpected string: %s\n", token );

        }
        /* Get next token: */
        tokenIndex++;
        token = wcstok( NULL, L"," );
        //
    }

}

void CEffectsDiscoveryCPPTestCore::AddtoNotificationRenderCategory(Windows::Media::Render::AudioRenderCategory category)
{
    m_presentCategories->Append(category);
}

void CEffectsDiscoveryCPPTestCore::VerifyNotificationsReceived()
{
    //In a received set of notifications, check if all audio categories expected were present
    //For example toggling loudness eq isn't expected to raise notifications for Communication streams
    //All other categories are expected to be present.
    const int expectedCategories = 247;
    int presentCategories = 0;


    //All categories except Communications is expected to raise notifications
    for (UINT fx = 0; fx < m_presentCategories->Size; fx++) 
    {
        auto category = m_presentCategories->GetAt(fx); 
        switch(category)
        {
        case Windows::Media::Render::AudioRenderCategory::Other:
            presentCategories |= 1;
            break;
        case Windows::Media::Render::AudioRenderCategory::Movie:
            presentCategories |= 2;
            break;
        case Windows::Media::Render::AudioRenderCategory::Media:
            presentCategories |= 4;
            break;
        case Windows::Media::Render::AudioRenderCategory::Alerts:
            presentCategories |= 16;
            break;
        case Windows::Media::Render::AudioRenderCategory::SoundEffects:
            presentCategories |= 32;
            break;
        case Windows::Media::Render::AudioRenderCategory::GameEffects:
            presentCategories |= 64;
            break;
        case Windows::Media::Render::AudioRenderCategory::GameMedia:
            presentCategories |= 128;
            break;         
        case Windows::Media::Render::AudioRenderCategory::GameChat:
            presentCategories |= 256;
            break; 
		
        }
    }

    if (presentCategories != expectedCategories)
    {
        LOG_ERROR(L"Unexpected render watchers expected notifications and raised notifications don't match. Present Categories is:%u",presentCategories);
    }
    //clean up the list after validation.
    ClearPresentCategories();

}

void CEffectsDiscoveryCPPTestCore::VerifyNotificationsLoudnessEq(
    Platform::Collections::Vector<AudioEffectType> ^list,    
    Windows::Media::Render::AudioRenderCategory category,
    Windows::Media::AudioProcessing mode,
    CPropVariant var
    ){

        bool bLoudnessEqFound = false;
        try
        {
            if (mode == Windows::Media::AudioProcessing::Raw)
            {
                LOG_ERROR(L"Toggling Loudness Eq wasn't expected to trigger a change event in raw mode");
            }
            if(category == Windows::Media::Render::AudioRenderCategory::Communications)
            {
                LOG_ERROR(L"Communication render,raw/default is not expected to trigger a change event due to toggling loudnessEq");
            }


            for (UINT fx = 0; fx < list->Size; fx++) 
            {
                auto effect = list->GetAt(fx); 
                //means loudness eq was on to begin with and was turned off
                if ( var.boolVal == VARIANT_TRUE && effect == Windows::Media::Effects::AudioEffectType::LoudnessEqualizer )
                {
                    LOG_ERROR(L"Loudness Eq is currently off. Unexpected to see it in list of effects");
                }
                //means loudness eq was off to begin with and was turned on
                else if (var.boolVal == VARIANT_FALSE && effect == Windows::Media::Effects::AudioEffectType::LoudnessEqualizer )
                {
                    bLoudnessEqFound = true;
                }
            }
            if (var.boolVal == VARIANT_FALSE  && bLoudnessEqFound == false)
            {
                LOG_ERROR(L"LoudnessEq was not found in the list of effects.It is expected to be present");
            }
        }
        catch (Platform::Exception^ e)
        { 
            LogException(e);       
        }
}


void CEffectsDiscoveryCPPTestCore::ProcessCaptureEffectsList(const wchar_t* effectsList)
{
    Platform::Collections::Vector<AudioEffectType>^ effects=ref new Platform::Collections::Vector<AudioEffectType>();
    const wchar_t seps[] = L",";
    wchar_t *token;
    Windows::Media::Capture::MediaCategory category;
    Windows::Media::AudioProcessing mode;
    wchar_t *localEffectsList = new wchar_t [ wcslen(effectsList)+ 1 ];

    wcscpy(localEffectsList, effectsList);

    token = wcstok( localEffectsList, seps );

    category = MediaCaptureCategoryFromString(token);
    token = wcstok( NULL, seps );
    mode = AudioProcessingFromString(token);
    while( token != NULL )
    {
        /* While there are tokens in "string" */
        printf( " %s\n", token );
        /* Get next token: */
        token = wcstok( NULL, seps );
        effects->Append(AudioEffectTypeFromString(token));
    }

}

bool CEffectsDiscoveryCPPTestCore::SetupCommunications(EDataFlow flow)
{
    UINT nClients=0;

    SetVerifyOutput verifySettings( VerifyOutputSettings::LogOnlyFailures );
    VERIFY_SUCCEEDED(Windows::Foundation::Initialize( RO_INIT_MULTITHREADED ) ); // Async objects work better in MTAs         

    VERIFY_SUCCEEDED(m_spCommunicationsFramework.CoCreateInstance(__uuidof(ModernAppCommunications)));
    VERIFY_SUCCEEDED(CreateWexBasicLog(&m_spLog));

    VERIFY_SUCCEEDED(m_spLog->QueryInterface(__uuidof(IRpcBasicLog), (void**)(&m_spRemoteLogger)));
    VERIFY_SUCCEEDED(m_spCommunicationsFramework->CreateMACServer(m_spRemoteLogger, &m_spCommServer));  

    //
    //  Subscribe to client arrival notifications
    //                    

    m_pNotificationHandler = new CArrivalNotificationHandler();

    VERIFY_IS_NOT_NULL(m_pNotificationHandler);

    m_spClientNotify.Attach(reinterpret_cast<IMACClientActivityNotify*>(m_pNotificationHandler));
    VERIFY_IS_NOT_NULL(m_spClientNotify);       
    VERIFY_SUCCEEDED(m_spCommServer->RegisterClientActivityNotify(m_spClientNotify));                    

    VERIFY_IS_NOT_NULL(m_pLauncher = new CClientLauncher());
    VERIFY_SUCCEEDED(m_pLauncher->LaunchApp(L"effectsdiscoverywwa_8wekyb3d8bbwe!genericpbmwwa1.App"));                    

    //Wait for app to launch
    Sleep(10000);

    VERIFY_SUCCEEDED(m_spCommServer->GetActiveClientCount(&nClients));
    LOG_OUTPUT(L"Number of clients %u", nClients );  

    if (flow == eRender)
    {
        VERIFY_SUCCEEDED(m_spCommServer->ReportServerEventToClient( L"EffectsDiscoveryWWA", L"ActivateRenderEffectsWatchers", L""));
    }
    else
    {
        VERIFY_SUCCEEDED(m_spCommServer->ReportServerEventToClient( L"EffectsDiscoveryWWA", L"ActivateCaptureEffectsWatchers", L""));
    }
    m_hHandle = m_pNotificationHandler->GetWatcherHandle();
    VERIFY_IS_NOT_NULL(m_hHandle);

    if( WAIT_OBJECT_0 != WaitForSingleObject(m_hHandle,INFINITE))
    {
        LOG_ERROR(L"WaitForSingleObject failed for Watcher handle");
    }

    return true;
}


void CEffectsDiscoveryCPPTestCore::StopListening(EDataFlow flow)
{
    if (flow == eRender)
    {
        VERIFY_SUCCEEDED(m_spCommServer->ReportServerEventToClient( L"EffectsDiscoveryWWA", L"StopRenderEffectsWatchers", L""));
    }
    else
    {
        VERIFY_SUCCEEDED(m_spCommServer->ReportServerEventToClient( L"EffectsDiscoveryWWA", L"StopCaptureEffectsWatchers", L""));
    }

}

CArrivalNotificationHandler* CEffectsDiscoveryCPPTestCore::GetNotificationHandler()
{
    return m_pNotificationHandler;
}


bool CEffectsDiscoveryCPPTestCore::TeardownCommunications()
{
    if (m_spCommServer != NULL)
    {
        CloseHandle(m_hHandle);	
        if (m_pLauncher != NULL) m_pLauncher->TerminateAp(L"effectsdiscoverywwa_1.0.0.0_neutral_neutral_8wekyb3d8bbwe");	
        m_spCommServer->UnRegisterClientActivityNotify(m_spClientNotify);     
        m_spLog.Release();
        m_spRemoteLogger.Release();  
        RestartMACService();
        Windows::Foundation::Uninitialize(); 
    }
    return true;
}


HRESULT CEffectsDiscoveryCPPTestCore::RestartMACService()
{
    //get service handle to ModernAppCommunications service
    SC_HANDLE hServiceControlManager = NULL;
    SC_HANDLE hMACServiceHandle = NULL;
    hServiceControlManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == hServiceControlManager)
    {
        LOG_ERROR(L"Failed obtaining handle to service control manager");
        return E_FAIL;
    }

    hMACServiceHandle = OpenServiceW(hServiceControlManager, L"ModernAppCommunications", SC_MANAGER_ALL_ACCESS);
    if (NULL == hMACServiceHandle)
    {
        LOG_ERROR(L"Failed obtaining handle to MAC Service");
        return E_FAIL;
    }

    //stop MAC Service
    SERVICE_STATUS sServiceStatus;
    if(!ControlService(hMACServiceHandle, SERVICE_CONTROL_STOP, &sServiceStatus))
    {
        LOG_ERROR(L"Unable to shut down service (ec: %d)", GetLastError());
        return HRESULT_FROM_WIN32(GetLastError());
    }

    BOOL bServiceStopped = FALSE;
    // wait for the service to shut down
    for(int i=0;i<60;i++)
    {
        if(!QueryServiceStatus(hMACServiceHandle, &sServiceStatus))
        {
            LOG_ERROR(L"Unable to obtain status for MAC ");
            return E_FAIL;
        }

        if(SERVICE_STOPPED == sServiceStatus.dwCurrentState)
        {
            bServiceStopped = TRUE;
            break;
        }
        Sleep(1000);
    }

    if(FALSE == bServiceStopped)
    {
        LOG_ERROR(L"TIMEOUT while waiting for MAC to stop");
        return E_FAIL;
    }

    if(!StartService(hMACServiceHandle, NULL, NULL))
    {
        LOG_ERROR(L"Unable to start MAC (ec: %d)", GetLastError());
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // wait for the service to start
    BOOL bServiceStarted = FALSE;
    for(int i=0;i<60;i++)
    {
        if(!QueryServiceStatus(hMACServiceHandle, &sServiceStatus))
        {
            LOG_ERROR(L"Unable to obtain status for MAC");
            return E_FAIL;
        }

        if(SERVICE_RUNNING == sServiceStatus.dwCurrentState)
        {
            bServiceStarted = TRUE;
            break;
        }
        Sleep(1000);
    }

    if(FALSE == bServiceStarted)
    {
        LOG_ERROR(L"TIMEOUT while waiting for MAC to start");
        return E_FAIL;
    }
    return S_OK;
}


//
// IMACClientActivityNotify
//
HRESULT STDMETHODCALLTYPE CArrivalNotificationHandler::OnClientArrival(LPCWSTR wszArrivedClient, UINT nActiveClients)
{
    wprintf(L"\n[Client Arrival] (%ws) (count: %d)\n", wszArrivedClient, nActiveClients);
    return S_OK;
}


HRESULT STDMETHODCALLTYPE CArrivalNotificationHandler::OnClientRemoval(LPCWSTR wszRemovedClient, UINT nActiveClients)
{
    wprintf(L"\n[Client Removal] (%ws) (count: %d)\n", wszRemovedClient, nActiveClients);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CArrivalNotificationHandler::OnClientNotification(LPCWSTR wszSourceClient, LPCWSTR wszNotificationDescription, LPCWSTR wszNotificationContents)
{    
    wprintf(L"\n[Generic Client Notification] (%ws) (desciption: \"%ws\") (contents: \"%ws\")",
        wszSourceClient,
        wszNotificationDescription,
        wszNotificationContents);

    if (wcscmp(wszNotificationDescription,  L"WatcherReady") == 0)
    {          
        if(SetEvent(m_hWatcherReady) == 0)
        {
            LOG_ERROR(L"SetEvent failed");
        }
    }

    if (wcscmp(wszNotificationDescription,  L"EffectsList") == 0)
    { 
        m_effectList->Append(ref new Platform::String(wszNotificationContents));		
    }

    if (wcscmp(wszNotificationDescription,  L"WatcherRemoved") == 0)
    {     
        LOG_OUTPUT(L"Watcher Removed");

    }

    return S_OK;
}

HANDLE CArrivalNotificationHandler::GetWatcherHandle()
{
    return m_hWatcherReady;
}

Vector<Platform::String^>^ CArrivalNotificationHandler::GetEffectsListInNotification()
{   
    return m_effectList;
}


CClientLauncher::CClientLauncher()
{ 
    HRESULT hr = S_OK;
    VERIFY_SUCCEEDED( CoCreateInstance(CLSID_ApplicationActivationManager, 
        nullptr, 
        CLSCTX_LOCAL_SERVER , 
        IID_IApplicationActivationManager,
        (LPVOID*)&m_pActivationManager));
    //this returns ACCESS DENIED when we launch again in same taef process
    //This is because the app is already allowed foreground
#ifndef BUILD_FOR_MOBILECORE
    hr = CoAllowSetForegroundWindow(m_pActivationManager.Get(), NULL);
    LOG_OUTPUT(L"CoAllowSetForegroundWindow returned 0x%08x",hr);
#else
    hr = E_FAIL;
#endif
}

HRESULT CClientLauncher::LaunchApp(LPWSTR pwszAppId)
{
    HRESULT hr = S_OK;             

    VERIFY_SUCCEEDED(hr = m_pActivationManager->ActivateApplication(pwszAppId, NULL, AO_NONE, &m_pid));  
    return hr;
}   

void CClientLauncher::TerminateAp(LPWSTR pwszAppId)
{
    CComPtr<IPackageDebugSettings> spPackageDebugSettings;
    HRESULT hrResult =S_OK;
    VERIFY_SUCCEEDED(hrResult = CoCreateInstance(CLSID_PackageDebugSettings,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&spPackageDebugSettings)));


    VERIFY_SUCCEEDED(hrResult = spPackageDebugSettings->TerminateAllProcesses(pwszAppId));

}

