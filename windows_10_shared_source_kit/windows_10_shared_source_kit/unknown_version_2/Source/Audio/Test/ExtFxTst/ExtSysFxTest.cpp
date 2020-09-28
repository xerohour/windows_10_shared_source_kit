//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft. All rights reserved.
//
//  File:       ExtSysFxTest.cpp
//
//  Created:    Fwong
//
//--------------------------------------------------------------------------

#include <initguid.h>
#include "common.h"
#include "propkey.h"
#include <AudioClient.h>
#include <functiondiscoverykeys.h>
#include <DevPKey.h>

// ------------------------------------------------------------------------------
// App static name
#ifndef TESTAPOS
#define SZSHELLAPP          "External SysFx Test - 1.0"
#else
#define SZSHELLAPP          "APO Test - 1.0"
#endif

#ifndef BUILD_TAEF
static  LPSTR szModuleName = SZSHELLAPP;
ITestShell* g_IShell;
#else
CTaefShellStub g_IShellObject;
CTaefShellStub * g_IShell = &g_IShellObject;
IBasicLog * g_pBasicLog = NULL;

using namespace WEX::Common;
using namespace WEX::TestExecution;
#endif
CAPODeviceTestModule *g_pAPODeviceTestModule = NULL;

#define APO_REGISTRY_KEY L"AudioEngine\\AudioProcessingObjects"
#define kRegKeyMaxChars 512

//  Default values for Initialize...
APOLIMITS gAPOLimits =
{
    //  RateConvert
    44100.0,                //  Rate_fInputFrameRate
    44100.0,                //  Rate_fOutputFrameRate
    8000.0,                 //  Rate_fMinInputFrameRate
    96000.0,                //  Rate_fMaxInputFrameRate
    8000.0,                 //  Rate_fMinOutputFrameRate
    96000.0,                //  Rate_fMaxOutputFrameRate
    SRC_QUALITY_LINEAR,     //  Rate_eAlgorithmType
    AUDIO_FLOW_PUSH,        //  Rate_eFlowType
    2,                      //  Rate_u32SamplesPerFrame
    0L,                     //  Rate_u32HighQualityTaplength
    
    //  Volume
    2,                      //  Volume_u32SamplesPerFrame
    48000.0,                //  Volume_f32FrameRate
    
    //  Matrix
    2,                      //  Matrix_u32InputSamplesPerFrame
    1,                      //  Matrix_u32OutputSamplesPerFrame
    48000.0,                //  Matrix_f32FrameRate
    
    //  Meter
    2,                      //  Meter_u32SamplesPerFrame
    10,                     //  Meter_u32FramesPerAverage
    10,                     //  Meter_u32NumAverages
    10,                     //  Meter_u32FramesPerQuantum
    
    //  Limiter
    2,                      //  Limiter_u32SamplesPerFrame
    48000.0,                //  Limiter_f32FrameRate
    FALSE,                  //  Limiter_bInDevicePipe
    
//    //  General
//    10.0,                   //  f32MinInFramesPerSecond
//    384000.0,               //  f32MaxInFramesPerSecond
//    10.0,                   //  f32MinOutFramesPerSecond
//    384000.0,               //  f32MaxOutFramesPerSecond
//    
//    1,                      //  u32MinInSamplesPerSecond;
//    4096,                   //  u32MaxInSamplesPerSecond;
//    1,                      //  u32MinOutSamplesPerSecond;
//    4096                    //  u32MaxOutSamplesPerSecond;
};

struct APO_PROPERTIES { 
    PROPERTYKEY key; 
    LPARAM apoType;
    CHAR apoTypeName[5];
    DWORD dwMajorVersion; 
    DWORD dwMinorVersion; 
    } PKEY_FX_EFFECTS[] = {
        {PKEY_FX_PostMixEffectClsid,  DT_GFX, "GFX " ,6, 0},
        {PKEY_FX_PreMixEffectClsid,   DT_LFX, "LFX ", 6, 0},
        {PKEY_FX_StreamEffectClsid,   DT_SFX, "SFX ", 6, 3},
        {PKEY_FX_ModeEffectClsid,     DT_MFX, "MFX ", 6, 3},
        {PKEY_FX_EndpointEffectClsid, DT_EFX, "EFX ", 6, 3},
    }; 

DWORD WINAPI TC_DisplayDeviceAssociation()
{
    HRESULT                         hr;
    UINT                            iDevice;
    CComPtr<IMMDeviceEnumerator>    pIDeviceEnumerator = NULL;
    CComPtr<IMMDeviceCollection>    pDevices = NULL;
    CAPODevice                     *pCurrentDevice = g_pAPODeviceTestModule->GetDevice();
    DWORD                           dwDeviceType = pCurrentDevice->GetDeviceType();
    WCHAR                           szClsIDDevice[40];
    
    {
        GUID    guidClassId;
        
        pCurrentDevice->GetClsID(&guidClassId);
        SAL_TERMINATE(szClsIDDevice);
        StringFromGUID2(guidClassId, szClsIDDevice, ARRAYSIZE(szClsIDDevice));
    }
    
    if (S_OK != (hr = pIDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator))))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: CoCreateInstance returned error (0x%08lx).",
            hr);
            
        return (FNS_PASS);
    }
    
    if (S_OK != (hr = pIDeviceEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATEMASK_INTERNAL, &pDevices)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IMMDeviceEnumerator::EnumAudioEndpoints returned error (0x%08lx).",
            hr);
            
        return (FNS_PASS);
    }
    
    if (S_OK != (hr = pDevices->GetCount(&iDevice)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IMMDeviceCollection::GetCount returned error (0x%08lx).", hr);
            
        return (FNS_PASS);
    }
    
    SLOG(eInfo1, "Found %d endpoints.", iDevice);
    
    for (; iDevice; iDevice--)
    {
        CComPtr<IMMDevice>              pIMMDevice = NULL;
        CComPtr<IMMEndpoint>            pIMMEndpoint = NULL;
        CComPtr<IMMEndpointInternal>    pIMMEndpointInt = NULL;
        CComPtr<IPropertyStore>         pIFXPropertyStore = NULL;
        CComPtr<IPropertyStore>         pIEPPropertyStore = NULL;
        char                            szEndpoint[256];
        PROPVARIANT                     pv;
        
        if (S_OK != (hr = pDevices->Item(iDevice - 1, &pIMMDevice)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: IMMDeviceCollection::Item returned error (0x%08lx).",
                hr);
                
            return (FNS_PASS);
        }
        
        GetEndpointFriendlyNameA(pIMMDevice, szEndpoint, ARRAYSIZE(szEndpoint));
        
        if (S_OK != (hr = pIMMDevice->QueryInterface(__uuidof(IMMEndpoint), (void**)&pIMMEndpoint)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: IMMDevice::QueryInterface returned error (0x%08lx).",
                hr);
                
            return (FNS_PASS);
        }
        
        if (S_OK != (hr = pIMMDevice->OpenPropertyStore(STGM_READ, &pIEPPropertyStore)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: IMMDevice::OpenPropertyStore returned error (0x%08lx).",
                hr);
                
            return (FNS_PASS);
        }
        
        if (S_OK == (hr = pIMMEndpoint->QueryInterface(__uuidof(IMMEndpointInternal), (void**)&pIMMEndpointInt)))
        {
            // fine
        }
#ifdef WHQL
        else if (E_NOINTERFACE == hr)
        {
            // ... could still be fine...
            // ... in particular, we could be running a Win7 ExtSysFX Test on Vista
            // If that's the case, query for the old IMMEndpointInternal IID,
            // and make sure not to call into any of the new methods
            // (which won't exist.)
            SLOG(eInfo1, "IMMEndpoint::QueryInterface(IMMEndpointInternal) failed; trying Vista-era interface ID");

            // pulled from vista_rtm mmdeviceapip.h
            // {84087C38-1B58-422b-BF7E-55D2E3364199}
            IID IID_IMMEndpointInternalVista = {
                0x84087C38, 0x1B58, 0x422B, 0xBF, 0x7E, 0x55, 0xD2, 0xE3, 0x36, 0x41, 0x99
            };

            if (S_OK != (hr = pIMMEndpoint->QueryInterface(IID_IMMEndpointInternalVista, (void**)&pIMMEndpointInt)))
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: IMMEndpoint::QueryInterface(IMMEndpointInternal) failed and Vista-era interface ID returned error (0x%08lx).",
                    hr);
                    
                return (FNS_PASS);
            }
            
        }
#endif
        else
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: IMMEndpoint::QueryInterface returned error (0x%08lx).",
                hr);
                
            return (FNS_PASS);
        }
        
        if (S_OK != (hr = pIMMEndpointInt->OpenFXPropertyStore(STGM_READ, &pIFXPropertyStore)))
        {
            if (HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) == hr)
            {
                //  Plausible...  This device probably doesn't have any SysFx...
                //  Just skip!
            
                continue;
            }
        
            XLOG(
                XFAIL,
                eError,
                "FAIL: IMMEndpointInternal::OpenFXPropertyStore returned error "
                    "(0x%08lx).",
                hr);
                
            return (FNS_PASS);
        }
        
        PropVariantInit(&pv);

        OSVERSIONINFO ver = {0};
        ver.dwOSVersionInfoSize = sizeof(ver);

        if (!GetVersionEx(&ver))
        {
            XLOG(
                XFAIL, 
                eFatalError, 
                "FAIL: GetVersionEx() failed: GetLastError() = %u", GetLastError());
        }

        SLOG(eInfo1, "Windows version %u.%u", ver.dwMajorVersion, ver.dwMinorVersion);

        for(int i = 0; i < ARRAYSIZE(PKEY_FX_EFFECTS); i++)
        {
            if (ver.dwMajorVersion  < PKEY_FX_EFFECTS[i].dwMajorVersion || 
               (ver.dwMajorVersion == PKEY_FX_EFFECTS[i].dwMajorVersion &&
                ver.dwMinorVersion  < PKEY_FX_EFFECTS[i].dwMinorVersion))
            {
	        continue;
            }

            if (PKEY_FX_EFFECTS[i].apoType == (LPARAM)dwDeviceType)
            {
                if (S_OK == (hr = pIFXPropertyStore->GetValue(PKEY_FX_EFFECTS[i].key, &pv)))
                {
                    if (VT_LPWSTR == V_VT(&pv))
                    {
                        if (0 == lstrcmpiW(szClsIDDevice, pv.pwszVal))
                        {
                            PROPVARIANT     pvDisabled;
                            UINT32          u32Disabled;
                        
                            PropVariantInit(&pvDisabled);
                        
                            if (S_OK != (hr = pIFXPropertyStore->GetValue(PKEY_AudioEndpoint_Disable_SysFx, &pvDisabled)))
                            {
                                XLOG(
                                    XFAIL,
                                    eError,
                                    "FAIL: IPropertyStore::GetValue returned error "
                                    "(0x%08lx).",
                                    hr);
                            
                                return (FNS_PASS);
                            }
                        
                            u32Disabled = V_UI4(&pvDisabled);

                            if (u32Disabled)
                            {
                                SLOG(eInfo1, "%s is DISABLED on endpoint: [%s].", PKEY_FX_EFFECTS[i].apoTypeName, szEndpoint);
                            }
                            else
                            {
                                SLOG(eInfo1, "%s is ENABLED on endpoint: [%s].", PKEY_FX_EFFECTS[i].apoTypeName, szEndpoint);
                            }
                        }
                    }
                }
            }
        }
        PropVariantClear(&pv);
    }
    
    return (FNS_PASS);
}

//
// Test Case Info Structure
//

// Unit/ Invalid Test Cases for DeviceTopology set of API's. Each test case maps to a method in DeviceTopology. 
// However one method may have one or more test cases associated with it depending on various scenarious that
// we are testing the method for.

BEGIN_TEST_CASE_INFO(SysFxTaef, CAPODeviceTestModule)
//                      [TEST ID]     [Test Case Name]                                      [TEST CASE FLAGS]    [DEVICE TYPE FLAGS]   [NA]   [N/A]  [TEST CASE FUNCTION POINTER]                                  [TEST CASE GUID]
    DEVICE_TEST_PARENT( "",           "SysFx",                                              0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{2413F780-77D3-4275-BAAE-E72C32670FB9}")
    DEVICE_TEST_PARENT( "",           " General",                                           0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{CBE6FF6F-A6C5-4023-87B1-BD1783AF24BE}")
    DEVICE_TEST_METHOD( "1.0.0",      "  Verify interfaces tests",                          0,                   DT_ALL | DEV_OR,      0,     0,     TC_General_VerifyInterfaceSupport,                            "{2946300B-F036-4007-BEDE-CF4A901C7725}")
    DEVICE_TEST_METHOD( "1.0.1",      "  Device Association",                               0,                   DT_SYSFX | DEV_OR,    0,     0,     TC_DisplayDeviceAssociation,                                  "{C7375A9A-D6A5-48E1-94BD-FFD139633FDC}")
    DEVICE_TEST_PARENT( "",           " IAudioProcessingObject",                            0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{93427B6D-17B5-4C9E-B08E-C28CB3D5975F}")

#ifdef TESTAPOS
    //  Keep test case disabled until Win7 bugs 170077-170079 are fixed.
    DEVICE_TEST_METHOD( "1.1.1",      "  GetLatency tests",                                 TESTFLAG_DISABLED,   DT_ALL | DEV_OR,      0,     0,     TC_IAudioProcessingObject_GetLatency,                         "{00BD8FD9-B006-454D-BFFE-0D4E59BD53F4}")
#else
    DEVICE_TEST_METHOD( "1.1.1",      "  GetLatency tests",                                 0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioProcessingObject_GetLatency,                         "{00BD8FD9-B006-454D-BFFE-0D4E59BD53F4}")
#endif

    DEVICE_TEST_METHOD( "1.1.2",      "  GetRegistrationProperties tests",                  0,                   DT_LGFX| DEV_OR,      0,     0,     TC_IAudioProcessingObject_GetRegistrationProperties,          "{51610066-B510-4051-B960-5442ED1BFB71}")
    DEVICE_TEST_METHOD( "1.1.3",      "  Verify InputFormatSupport tests",                  0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioProcessingObject_VerifyInputFormatSupport,           "{B6995138-8EC7-4F95-AB6E-EA0A84C569CE}")
    DEVICE_TEST_METHOD( "1.1.4",      "  Verify OutputFormatSupport tests",                 0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioProcessingObject_VerifyOutputFormatSupport,          "{A5292415-F190-4313-91AB-9FCB06004606}")
    DEVICE_TEST_METHOD( "1.1.5",      "  Verify no sample rate converter",                  0,                   DT_LFX,               0,     0,     TC_IAudioProcessingObject_VerifyNoSRC,                        "{CB815BEB-6E1B-47EA-8028-F50E4908AF06}")
    DEVICE_TEST_PARENT( "",           " IAudioProcessingObjectRT",                          0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{1A5B7E78-2C83-401C-83FD-1C6305142083}")

#ifdef TESTAPOS
    DEVICE_TEST_METHOD( "1.2.0",      "  Verify APOProcess tests",                          TESTFLAG_DISABLED,   DT_ALL | DEV_OR,      0,     0,     TC_IAudioProcessingObject_VerifyProcess,                      "{BA810C2E-16FD-4833-A1D8-1ECCDA5C4FD7}")
#else
    DEVICE_TEST_METHOD( "1.2.0",      "  Verify APOProcess tests",                          0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioProcessingObject_VerifyProcess,                      "{BA810C2E-16FD-4833-A1D8-1ECCDA5C4FD7}")
#endif

    DEVICE_TEST_PARENT( "",           " Certification",                                     0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{82F4BC2C-02B7-419F-B061-CA83E361B0E2}")
#ifndef BUILD_TAEF    
    DEVICE_TEST_METHOD( "1.3.0",      "  Verify signing and certificate",                   0,                   DT_ALL | DEV_OR,      0,     0,     TC_VerifySignature,                                           "{B1DA44E6-0EC7-4445-85D1-D426EB520523}")
#endif
    DEVICE_TEST_METHOD( "1.3.1",      "  Display driver files",                             0,                   DT_ALL | DEV_OR,      0,     0,     TC_DisplayFilelist,                                           "{5C7A0FED-A8A0-4209-AE62-5C7A73D1BC81}")
    DEVICE_TEST_PARENT( "",           " Device Specific",                                   0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{8DABA8FD-1240-4F27-B298-7F425A34F549}")
    DEVICE_TEST_METHOD( "1.4.0",      "  Verify MicArray unbeamed mode support",            0,                   DT_SYSFX | DEV_OR,    0,     0,     TC_VerifyMicArrayRaw,                                         "{20480324-74CC-4EAD-BE82-F24BDE842736}")
    DEVICE_TEST_PARENT( "",           " IAudioSystemEffectsCustomFormats",                  0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{79BC9531-D75C-44B8-AF6F-D3A92904B8A1}")
    DEVICE_TEST_METHOD( "1.5.0",      "  Verify GetFormat tests",                           0,                   DT_SYSFX | DEV_OR,    0,     0,     TC_VerifyCustomFormats,                                       "{A731D2E9-4086-4C68-8745-85600BF63F0B}")
    DEVICE_TEST_METHOD( "1.5.1",      "  Verify custom format support",                     0,                   DT_GFX,               0,     0,     TC_VerifyCustomFormatSupport,                                 "{6E28ECB3-D61C-42BB-97B4-6A6B0BF2D20C}")
    DEVICE_TEST_PARENT( "",           " External",                                          0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{27618b0d-dd81-4da4-babc-159185796675}")
    DEVICE_TEST_METHOD( "2.0.0",      "  Condition: (NULL == pDeviceCollection)",           0,                   DT_SYSFX | DEV_OR,    0,     0,     TC_External_NULL_DeviceCollection,                            "{e92fa37b-a328-47f4-b4f6-4bfde1a2158c}")
    DEVICE_TEST_METHOD( "2.0.1",      "  Condition: (NULL == pAPOEndpointProperties)",      0,                   DT_SYSFX | DEV_OR,    0,     0,     TC_External_NULL_EndpointProperties,                          "{EF37BAF8-7158-41D4-AB5A-C2A2231C8D44}")
    
#ifdef TESTAPOS    
    DEVICE_TEST_PARENT( "",           " By APO",                                            0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{ea6411ed-bacd-4858-a58b-41801320d857}")
    DEVICE_TEST_PARENT( "",           "  CAudioVolume",                                     0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{80688172-aead-44f0-bd5c-9298c4f04743}")
    DEVICE_TEST_METHOD( "3.0.0",      "   Verify Registration Properties",                  0,                   DT_ALL | DEV_OR,      0,     0,     TC_CAudioVolume_VerifyRegistration,                           "{4499eded-c166-4948-8908-a91cd2a3636d}")
    DEVICE_TEST_METHOD( "3.0.1",      "   Initialize test",                                 0,                   DT_ALL | DEV_OR,      0,     0,     TC_CAudioVolume_Initialize,                                   "{03b8cfca-d583-45be-a32a-c2c6b1ba8d8d}")
    DEVICE_TEST_METHOD( "3.0.2",      "   Verify States",                                   0,                   DT_ALL | DEV_OR,      0,     0,     TC_CAudioVolume_States,                                       "{fab6a010-b86e-4b85-8a8c-e576f993a24e}")
    DEVICE_TEST_PARENT( "",           "  CAudioRateConvertCMPT",                            0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{8ad1fe66-4f90-4fbe-ae72-c7b31e05fc57}")
    DEVICE_TEST_METHOD( "3.1.0",      "   Verify Registration Properties",                  0,                   DT_ALL | DEV_OR,      0,     0,     TC_CAudioRateConvertCMPT_VerifyRegistration,                  "{25d5a0d0-05c4-4866-ad2f-0c3dbcdc4af6}")
    DEVICE_TEST_METHOD( "3.1.1",      "   Initialize test",                                 0,                   DT_ALL | DEV_OR,      0,     0,     TC_CAudioRateConvertCMPT_Initialize,                          "{a3f8915e-1dd2-4dfe-9aaa-b2f57d05220c}") 
    DEVICE_TEST_METHOD( "3.1.2",      "   Process, no FR change, SRC_QUALITY_LINEAR",       0,                   DT_ALL | DEV_OR,      0,     0,     TC_CAudioRateConvertCMPT_Functional_NoChange_Linear,          "{3cbefd20-feaa-4f80-8b66-71aaff807f76}") 
    DEVICE_TEST_METHOD( "3.1.3",      "   Process, increase FR by 1%, SRC_QUALITY_LINEAR",  0,                   DT_ALL | DEV_OR,      0,     0,     TC_CAudioRateConvertCMPT_Functional_Increase1Percent_Linear,  "{a9b22661-f1ac-492e-bc5f-36c97e2a5022}") 
    DEVICE_TEST_METHOD( "3.1.4",      "   Process, increase FR by 20%, SRC_QUALITY_LINEAR", 0,                   DT_ALL | DEV_OR,      0,     0,     TC_CAudioRateConvertCMPT_Functional_Increase20Percent_Linear, "{a4bded39-c8ee-4478-bb6d-122cc73f808b}") 
    DEVICE_TEST_METHOD( "3.1.5",      "   Process, decrease FR by 1%, SRC_QUALITY_LINEAR",  0,                   DT_ALL | DEV_OR,      0,     0,     TC_CAudioRateConvertCMPT_Functional_Decrease1Percent_Linear,  "{26d5035d-c71c-4be7-a40d-bf8bc5efc4d5}") 
    DEVICE_TEST_METHOD( "3.1.6",      "   Process, decrease FR by 20%, SRC_QUALITY_LINEAR", 0,                   DT_ALL | DEV_OR,      0,     0,     TC_CAudioRateConvertCMPT_Functional_Decrease20Percent_Linear, "{f876f03f-75d6-4c6b-aa3c-723cf13aa38b}")
    DEVICE_TEST_METHOD( "3.1.7",      "   Process, no FR change, SRC_QUALITY_SINC3",        0,                   DT_ALL | DEV_OR,      0,     0,     TC_CAudioRateConvertCMPT_Functional_NoChange_SINC3,           "{CE790DE6-12EC-47C8-B153-8C9670980C49}") 
    DEVICE_TEST_METHOD( "3.1.8",      "   Process, increase FR by 1%, SRC_QUALITY_SINC3",   0,                   DT_ALL | DEV_OR,      0,     0,     TC_CAudioRateConvertCMPT_Functional_Increase1Percent_SINC3,   "{39050AE0-5976-4069-B4F6-44FD770E2113}") 
    DEVICE_TEST_METHOD( "3.1.9",      "   Process, increase FR by 20%, SRC_QUALITY_SINC3",  0,                   DT_ALL | DEV_OR,      0,     0,     TC_CAudioRateConvertCMPT_Functional_Increase20Percent_SINC3,  "{F7AE79E6-E7A5-4AA2-937C-0E68B28501C9}") 
    DEVICE_TEST_METHOD( "3.1.10",     "   Process, decrease FR by 1%, SRC_QUALITY_SINC3",   0,                   DT_ALL | DEV_OR,      0,     0,     TC_CAudioRateConvertCMPT_Functional_Decrease1Percent_SINC3,   "{750D12C3-B219-4558-88E4-053A4BDD18F4}") 
    DEVICE_TEST_METHOD( "3.1.11",     "   Process, decrease FR by 20%, SRC_QUALITY_SINC3",  0,                   DT_ALL | DEV_OR,      0,     0,     TC_CAudioRateConvertCMPT_Functional_Decrease20Percent_SINC3,  "{F1F1A132-9550-4797-97A3-A97F0BE965B9}") 
    DEVICE_TEST_PARENT( "",           " By Interface",                                      0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{ea4d1ef6-e249-4f67-be4d-b0daf32fab28}")
    DEVICE_TEST_PARENT( "",           "  IAudioVolume",                                     0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{90fd176b-6d8d-4627-9e1b-0d046f15fda9}")
    DEVICE_TEST_METHOD( "4.0.0",      "   SetVolumeCoefficients test",                      0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioVolume_SetVolumeCoefficients,                        "{2d9d43b2-e668-40f6-9c1b-40dc9f6d397a}")
    DEVICE_TEST_METHOD( "4.0.1",      "   GetVolumeCoefficients test",                      0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioVolume_GetVolumeCoefficients,                        "{e891592f-b924-4e47-8483-e8a7a52e89da}")
    DEVICE_TEST_METHOD( "4.0.2",      "   Verify AUDIO_CURVE_TYPE_NONE test",               0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioVolume_CURVE_NONE,                                   "{0abcf0ca-bd26-4051-a7f2-041d9260b3c6}")
    DEVICE_TEST_METHOD( "4.0.3",      "   Verify AUDIO_CURVE_TYPE_WINDOWS_FADE test",       0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioVolume_CURVE_WINDOWS_FADE,                           "{2999c923-ba57-48c3-a5cd-1bcad4da3ebe}")
    DEVICE_TEST_METHOD( "4.0.5",      "   Verify no Initial Ramp test",                     0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioVolume_NoInitialRamp,                                "{5b52a579-2eab-4e36-be43-3ec3c3242a52}")
    DEVICE_TEST_PARENT( "",           "  IAudioMuteAPO",                                    0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{3c969cb9-dc75-4c0f-a342-42e8d698fb6f}")
    DEVICE_TEST_METHOD( "4.1.0",      "   SetMute test",                                    0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioMuteAPO_SetMute,                                     "{cb14f0cb-0fa1-468f-8f15-9717747ea10a}")
    DEVICE_TEST_METHOD( "4.1.1",      "   GetMute test",                                    0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioMuteAPO_GetMute,                                     "{c4fc8603-011c-498f-8088-175172de9ce6}")
    DEVICE_TEST_METHOD( "4.1.2",      "   Verify AUDIO_CURVE_TYPE_NONE test",               0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioMuteAPO_CURVE_NONE,                                  "{d2105054-2d1e-4a50-97eb-63331ac87863}")
    DEVICE_TEST_METHOD( "4.1.3",      "   Verify AUDIO_CURVE_TYPE_WINDOWS_FADE test",       0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioMuteAPO_CURVE_WINDOWS_FADE,                          "{4bf0f7f6-9116-4615-b854-a3b6d8d4b82f}")

    //  Keep test case disabled until Win7 bug 22622 is resolved.
    DEVICE_TEST_METHOD( "4.1.5",      "   Verify no Initial Ramp test",                     TESTFLAG_DISABLED,   DT_ALL | DEV_OR,      0,     0,     TC_IAudioMuteAPO_NoInitialRamp,                               "{99795b81-69cc-48e1-874f-b5ea66376e5a}")

    DEVICE_TEST_PARENT( "",           "  IAudioRateConvert",                                0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{d956e06c-4dd9-491e-8219-0bbb5163366a}")
    DEVICE_TEST_METHOD( "4.2.0",      "   SetInputFrameRate test",                          0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioRateConvert_SetInputFrameRate,                       "{bfb6e6a5-edb5-4668-bfb2-42714b3030c4}")
    DEVICE_TEST_METHOD( "4.2.1",      "   SetOutputFrameRate test",                         0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioRateConvert_SetOutputFrameRate,                      "{4c3fc58c-c7c5-488c-9f58-1ce82ac1197b}")
    DEVICE_TEST_PARENT( "",           "  IAudioRateConvertRT",                              0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{c484bb87-e810-4fb2-9cfe-e1dada09c57e}")
    DEVICE_TEST_METHOD( "4.3.0",      "   GetInputFrameRate test",                          0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioRateConvertRT_GetInputFrameRate,                     "{9c26d889-fffb-4e57-8336-de7a743e280b}")
    DEVICE_TEST_METHOD( "4.3.1",      "   GetOutputFrameRate test",                         0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioRateConvertRT_GetOutputFrameRateRT,                  "{3cbb72fd-54ee-435c-bf92-ecb7b07ebb24}")
    DEVICE_TEST_PARENT( "",           "  IAudioProcessingObjectVBR",                        0,                   DT_ALL | DEV_OR,      0,     0,     NULL,                                                         "{cd4d5a5f-edac-4a16-adba-7c8f10063332}")
    DEVICE_TEST_METHOD( "4.4.0",      "   CalcMaxInputFrames test",                         0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioProcessingObjectVBR_CalcMaxInputFrames,              "{54b89f0e-b18f-43e3-b79a-55a7ab2b3952}")
    DEVICE_TEST_METHOD( "4.4.1",      "   CalcMaxOutputFrames test",                        0,                   DT_ALL | DEV_OR,      0,     0,     TC_IAudioProcessingObjectVBR_CalcMaxOutputFrames,             "{c03deea9-818f-430e-887d-c420ddfe0ae1}")
#endif
END_TEST_CASE_INFO(SysFxTaef)

#ifndef BUILD_TAEF
// ------------------------------------------------------------------------------
// Shell98 extern
CTestModule* WINAPI NewTestModule
(
    CTestShell* pShell,
    HINSTANCE   hInstDLL
)
{
    g_IShell               = (ITestShell*)pShell;
    g_pAPODeviceTestModule = new CAPODeviceTestModule(pShell, hInstDLL);
    return (CTestModule*)g_pAPODeviceTestModule;
}
#endif

// @@BEGIN_NOTPUBLICCODE
/*
BOOL CAPODevice::GetRegistrationProperties(LPCWSTR pszClassId)
{
    LONG        lRet;
    WCHAR       strRegKey[sizeof(APO_REGISTRY_KEY) + sizeof(GUID)*2 + 8] = {0}; // Enough to hold the reg key and GUID
    DWORD       dwLen;
    DWORD       dwVal = 0;

    StringCchPrintfW(strRegKey, sizeof(strRegKey), L"%s\\%s", APO_REGISTRY_KEY, pszClassId);
        dwLen = sizeof(dwVal);
        lRet = RegGetValueW(HKEY_CLASSES_ROOT, strRegKey, L"NumAPOInterfaces", RRF_RT_DWORD, NULL, &dwVal, &dwLen);
        if (ERROR_SUCCESS != lRet) goto Exit;

        m_pRegProps = (PAPO_REG_PROPERTIES)CoTaskMemAlloc(sizeof(APO_REG_PROPERTIES) + sizeof(IID)*(dwVal-1));
        if (m_pRegProps == NULL) goto Exit;

        m_pRegProps->u32NumAPOInterfaces = dwVal;
        for (UINT curAPOInterface = 0; curAPOInterface < m_pRegProps->u32NumAPOInterfaces; curAPOInterface++)
        {
            WCHAR strName[15];
            WCHAR strVal[sizeof(GUID)*2 + 8]; // Enough to hold a GUID
            StringCchPrintfW(strName, sizeof(strName), L"APOInterface%d", curAPOInterface);
            dwLen = sizeof(strVal);
            lRet = RegGetValueW(HKEY_CLASSES_ROOT, strRegKey, strName, RRF_RT_REG_SZ, NULL, strVal, &dwLen);
            if (ERROR_SUCCESS != lRet) goto Exit;
            if (FAILED(CLSIDFromString(strVal, m_pRegProps->iidAPOInterfaceList + curAPOInterface))) goto Exit;
        }

        m_pRegProps->clsid = m_gClsID;

        dwLen = sizeof(m_pRegProps->szFriendlyName);
        lRet = RegGetValueW(HKEY_CLASSES_ROOT, strRegKey, L"FriendlyName", RRF_RT_REG_SZ, NULL, m_pRegProps->szFriendlyName, &dwLen);
        if (ERROR_SUCCESS != lRet) goto Exit;

        dwLen = sizeof(m_pRegProps->szCopyrightInfo);
        lRet = RegGetValueW(HKEY_CLASSES_ROOT, strRegKey, L"Copyright", RRF_RT_REG_SZ, NULL, m_pRegProps->szCopyrightInfo, &dwLen);
        if (ERROR_SUCCESS != lRet) goto Exit;

        dwLen = sizeof(dwVal);
        lRet = RegGetValueW(HKEY_CLASSES_ROOT, strRegKey, L"MajorVersion", RRF_RT_DWORD, NULL, &dwVal, &dwLen);
        if (ERROR_SUCCESS != lRet) goto Exit;
        m_pRegProps->u32MajorVersion = dwVal;

        dwLen = sizeof(dwVal);
        lRet = RegGetValueW(HKEY_CLASSES_ROOT, strRegKey, L"MinorVersion", RRF_RT_DWORD, NULL, &dwVal, &dwLen);
        if (ERROR_SUCCESS != lRet) goto Exit;
        m_pRegProps->u32MinorVersion = dwVal;

        dwLen = sizeof(dwVal);
        lRet = RegGetValueW(HKEY_CLASSES_ROOT, strRegKey, L"Flags", RRF_RT_DWORD, NULL, &dwVal, &dwLen);
        if (ERROR_SUCCESS != lRet) goto Exit;
        m_pRegProps->Flags = static_cast<APO_FLAG>(dwVal);

        dwLen = sizeof(dwVal);
        lRet = RegGetValueW(HKEY_CLASSES_ROOT, strRegKey, L"MinInputConnections", RRF_RT_DWORD, NULL, &dwVal, &dwLen);
        if (ERROR_SUCCESS != lRet) goto Exit;
        m_pRegProps->u32MinInputConnections = dwVal;

        dwLen = sizeof(dwVal);
        lRet = RegGetValueW(HKEY_CLASSES_ROOT, strRegKey, L"MaxInputConnections", RRF_RT_DWORD, NULL, &dwVal, &dwLen);
        if (ERROR_SUCCESS != lRet) goto Exit;
        m_pRegProps->u32MaxInputConnections = dwVal;

        dwLen = sizeof(dwVal);
        lRet = RegGetValueW(HKEY_CLASSES_ROOT, strRegKey, L"MinOutputConnections", RRF_RT_DWORD, NULL, &dwVal, &dwLen);
        if (ERROR_SUCCESS != lRet) goto Exit;
        m_pRegProps->u32MinOutputConnections = dwVal;

        dwLen = sizeof(dwVal);
        lRet = RegGetValueW(HKEY_CLASSES_ROOT, strRegKey, L"MaxOutputConnections", RRF_RT_DWORD, NULL, &dwVal, &dwLen);
        if (ERROR_SUCCESS != lRet) goto Exit;
        m_pRegProps->u32MaxOutputConnections = dwVal;

        dwLen = sizeof(dwVal);
        lRet = RegGetValueW(HKEY_CLASSES_ROOT, strRegKey, L"MaxInstances", RRF_RT_DWORD, NULL, &dwVal, &dwLen);
        if (ERROR_SUCCESS != lRet) goto Exit;
        m_pRegProps->u32MaxInstances = dwVal;

        return TRUE;

Exit:
    return FALSE;
}
*/
// @@END_NOTPUBLICCODE

//--------------------------------------------------------------------------
//  CKsDevice constructor
//CKsDevice::CKsDevice
//(   
//    CDevice    *pDevice
//)
//{
//    m_pDevice = pDevice;
//
//    lstrcpyA(m_szRep, pDevice->GetFriendlyName());
//    lstrcpyA(m_szPnPId, pDevice->GetInterfaceId());
//    m_lpType = DT_GENERAL;
//}

CAPODevice::CAPODevice
(
    IMMDevice          *pIEndpoint,
    LPCWSTR             pszClassId,
    LPCWSTR             pszEndpoint,
    IPropertyStore     *pIStoreDevice,
    IPropertyStore     *pIStoreFx,
    LPARAM              apoType,
    LPCWSTR             pszAttachedDevice,
    BOOL                bProxyAPO
)
: m_pIUnknown(NULL), m_pIEndpoint(NULL), m_pIAPO(NULL), m_pRegProps(NULL), m_fValid(FALSE), m_pIEPStore(NULL), m_pIFXStore(NULL), m_pIDevCollection(NULL) //, m_pIAPORT(NULL), m_pIAPOConfig(NULL), m_pISysFx(NULL)
{
    LPSTR       pszFriendly;
    HRESULT     hr;
    UINT        cchRemain;

    {
        //  Getting PnpId from property store...
    
        PROPVARIANT                     pv;
        CComPtr<IMMDeviceEnumerator>    pIDeviceEnumerator = NULL;
        CComPtr<IMMDevice>              pIMMDevNode = NULL;
        CComPtr<IPropertyStore>         pIDNPropStore = NULL;
    
        if (S_OK != (hr = pIDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator))))
        {
            return;
        }
           
        PropVariantInit(&pv);
    
        if (S_OK != (hr = pIStoreDevice->GetValue(PKEY_Endpoint_Devnode, &pv)))
        {
            //  Any cleanup required for PropVariantInit?
            return;
        }
    
        if (VT_LPWSTR != pv.vt)
        {
            PropVariantClear(&pv);
            return;
        }
        
        if (S_OK != (hr = pIDeviceEnumerator->GetDevice(pv.pwszVal, &pIMMDevNode)))
        {
            PropVariantClear(&pv);
            return;
        }
        
        PropVariantClear(&pv);
        
        if (S_OK != (hr = pIMMDevNode->OpenPropertyStore(STGM_READ, &pIDNPropStore)))
        {
            return;
        }
        
        PropVariantInit(&pv);
        
        if (S_OK != (hr = pIDNPropStore->GetValue((PROPERTYKEY&)DEVPKEY_Device_InstanceId, &pv)))
        {
            //  Any cleanup required for PropVariantInit?
            return;
        }
        
        if (VT_LPWSTR != pv.vt)
        {
            PropVariantClear(&pv);
            return;
        }
        
        wcstombs(m_szPnPId, pv.pwszVal, sizeof(m_szPnPId));
        PropVariantClear(&pv);
    }
    
    if (NOERROR != CLSIDFromString((LPOLESTR)pszClassId, &m_gClsID))
    {
        SLOG(eError, "Eh?!  CLSIDFromString returned error.");
        return;
    }
    
    wcstombs(m_szAttachedDevice, pszAttachedDevice, sizeof(m_szAttachedDevice));
    
    if (0 == pIEndpoint->AddRef())
    {
        SLOG(eError, "Eh?!  IMMDevice::AddRef() returned zero.");
        return;
    }
    m_pIEndpoint = pIEndpoint;
    
    if (0 == pIStoreDevice->AddRef())
    {
        SLOG(eError, "Eh?!  IPropertyStore::AddRef() returned zero.");
        return;
    }
    m_pIEPStore = pIStoreDevice;
    
    if (0 == pIStoreFx->AddRef())
    {
        SLOG(eError, "Eh?!  IPropertyStore::AddRef() returned zero.");
        return;
    }
    m_pIFXStore = pIStoreFx;
    
    if (S_OK != (hr = m_pIUnknown.CoCreateInstance(m_gClsID)))
    {
        SLOG(eError, "Eh?!  CoCreateInstance for IUnknown returned error (0x%08lx).", hr);
        return;
    }

    if (S_OK != (hr = m_pIUnknown->QueryInterface(__uuidof(IAudioProcessingObject), (void**)&m_pIAPO)))
    {
        SLOG(eError, "Eh?!  CoCreateInstance for IAudioProcessingObject returned error (0x%08lx).", hr);
        return;
    }

    //  Getting DeviceCollection
    {
        PROPVARIANT                 pvFormat;
        
        // the standard KSDATAFORMAT_WAVEFORMATEX only contains a 
        // WAVEFORMATEX, but we need the extensible format so
        // we define this type here:
        typedef struct {
            KSDATAFORMAT            DataFormat;
            WAVEFORMATEXTENSIBLE    WFXtensible;
        } KS_WFExtensible;
        
        KS_WFExtensible             ksWfx;
        
        // initialize the structure
        PropVariantInit(&pvFormat);
        
        hr = pIStoreDevice->GetValue(PKEY_AudioEngine_DeviceFormat, &pvFormat);
        
        if (S_OK == hr)
        {
            if (pvFormat.vt == VT_BLOB)
            {
                if (pvFormat.blob.cbSize != sizeof(WAVEFORMATEXTENSIBLE))
                {
                    SLOG(eError, "pvFormat.blob.cbSize (%zu) != sizeof(WAVEFORMATEXTENSIBLE) (%zu)", pvFormat.blob.cbSize, sizeof(WAVEFORMATEXTENSIBLE));
                    return;
                }
            }
            else
            {
                SLOG(eError, "Format is not VT_BLOB, Q != %c", VARTYPE(pvFormat.vt));
                return;
            }
        }
        else
        {
            SLOG(eInfo1, "Get PKEY_AudioEngine_Device format returned (0x%08lx).", hr);
        }

        if ((S_OK == hr) && (pvFormat.vt == VT_BLOB) && (pvFormat.blob.cbSize == sizeof(WAVEFORMATEXTENSIBLE)))
        {
            LPWAVEFORMATEX              pbWfxDevice = NULL;
            ULONG                       cbWfxDevice = 0;
            CComPtr<IEndpointUtility2>  pIEPUtil = NULL;
            
            pbWfxDevice = (WAVEFORMATEX*)pvFormat.blob.pBlobData;
            cbWfxDevice = pvFormat.blob.cbSize;
    
            ksWfx.DataFormat.FormatSize  = sizeof(KSDATAFORMAT) + cbWfxDevice;
            ksWfx.DataFormat.Flags       = 0;
            ksWfx.DataFormat.SampleSize  = 0;
            ksWfx.DataFormat.Reserved    = 0;
            ksWfx.DataFormat.MajorFormat = KSDATAFORMAT_TYPE_AUDIO;
            ksWfx.DataFormat.SubFormat   = KSDATAFORMAT_SUBTYPE_PCM;
            ksWfx.DataFormat.Specifier   = KSDATAFORMAT_SPECIFIER_WAVEFORMATEX;
            CopyMemory( &(ksWfx.WFXtensible), pbWfxDevice, cbWfxDevice );
            
            hr = pIEndpoint->Activate(__uuidof(IEndpointUtility2), CLSCTX_ALL, NULL, (void**)&pIEPUtil);
            
            if (S_OK != hr)
            {
                SLOG(eError, "Eh?!  pIMMDevice->Activate returned error (0x%08lx).", hr);
                PropVariantClear(&pvFormat);
                return;
            }
            
            hr = pIEPUtil->GetHostConnectorDeviceCollection2((PKSDATAFORMAT)&ksWfx, sizeof(ksWfx), TRUE, eHostProcessConnector, &m_pIDevCollection, &m_nSoftwareIoDeviceInCollection, &m_nSoftwareIoConnectorIndex);
            
            if (S_OK != hr)
            {
                SLOG(eError, "Eh?!  Getting device collection returned error (0x%08lx).", hr);
                PropVariantClear(&pvFormat);
                return;
            }
        }
        
        PropVariantClear(&pvFormat);
        //  Note:  If not set, NULL == m_pIDevCollection!
    }

    if(NULL == m_pIDevCollection)
    {
        SLOG(eError, "We did not populate the device collection.", hr);
        m_fValid = FALSE;
        return;
    }
    
    //  Saved off for InitializeAPO()
    m_pIEndpoint = pIEndpoint;
    m_pIEPStore  = pIStoreDevice;
    m_pIFXStore  = pIStoreFx;
    m_lpType     = apoType;
    m_bProxyAPO  = bProxyAPO;
    
    //  Initialize SysFx!!
    if (S_OK != (hr = InitializeAPO(m_pIAPO)))
    {
        SLOG(eError, "Eh?!  Label00 pIAPO->Initialize returned error (0x%08lx).", hr);
        return;
    }

    // @@BEGIN_NOTPUBLICCODE
    // Remove this when ready to use APO registration registry code
    // @@END_NOTPUBLICCODE
    for (int i = 0; i < ARRAYSIZE(PKEY_FX_EFFECTS); i++)
    {
        if(apoType & PKEY_FX_EFFECTS[i].apoType)
        {
            lstrcpy(m_szRep, PKEY_FX_EFFECTS[i].apoTypeName);
        }
    }


    if (m_bProxyAPO) 
    {
        strcat_s(m_szRep, ARRAYSIZE(m_szRep), " Proxy APO"); 
    }

    pszFriendly = (&(m_szRep[0])) + lstrlenA(m_szRep);
    cchRemain   = (sizeof(m_szRep) / sizeof(m_szRep[0])) - lstrlenA(m_szRep);
    wcstombs(pszFriendly, pszClassId, cchRemain);

    lstrcatA(pszFriendly, " On ");
    cchRemain   = (sizeof(m_szRep) / sizeof(m_szRep[0])) - lstrlenA(m_szRep);
    pszFriendly = (&(m_szRep[0])) + lstrlenA(m_szRep);
    wcstombs(pszFriendly, pszEndpoint, cchRemain);
    // @@BEGIN_NOTPUBLICCODE
    // Remove to here...

    // Get APO registration info from the registry since the old API is deprecated
    // Enable this later once skipped/failing tests can be addressed
    /*
    if (!GetRegistrationProperties(pszClassId))
    {
        SLOG(eError, "Eh?!  Couldn't get APO registration properties from the registry.");
    }
    else
    {
        SAL_TERMINATE(m_pRegProps->szFriendlyName);
        wcstombs(m_szRep, m_pRegProps->szFriendlyName, sizeof(m_szRep) / sizeof(m_szRep[0]));
    }
    */
    // @@END_NOTPUBLICCODE

    m_fValid = TRUE;
}

CAPODevice::CAPODevice
(
    IAudioProcessingObject     *pIAPO,
    LPCWSTR                     pszClassId
)
: m_pIUnknown(NULL), m_pIEndpoint(NULL), m_pIAPO(NULL), m_pRegProps(NULL), m_fValid(FALSE), m_pIEPStore(NULL), m_pIFXStore(NULL), m_pIDevCollection(NULL)
{
    HRESULT         hr;

    m_pIAPO               = pIAPO;
    m_szAttachedDevice[0] = 0;
    
    if (S_OK != (hr = pIAPO->QueryInterface(__uuidof(IUnknown), (void**)&m_pIUnknown)))
    {
        return;
    }
    
    if (NOERROR != CLSIDFromString((LPOLESTR)pszClassId, &m_gClsID))
    {
        SLOG(eError, "Eh?!  CLSIDFromString returned error.");
        return;
    }
    
    m_lpType = DT_APO;
    
    if (S_OK != (hr = InitializeAPO(pIAPO)))
    {
        SLOG(eError, "Eh?!  pIAPO->Initialize returned error (0x%08lx).", hr);
        return;
    }
    
    if (S_OK != (hr = pIAPO->GetRegistrationProperties(&m_pRegProps)))
    {
        SLOG(eError, "pIAudioProcessingObject::GetRegistrationProperties is deprecated and might not be supported. (0x%08lx).", hr);
    }
    else
    {
        SAL_TERMINATE(m_pRegProps->szFriendlyName);
        wcstombs(m_szRep, m_pRegProps->szFriendlyName, sizeof(m_szRep)/sizeof(m_szRep[0]));
    }
    
    wcstombs(m_szPnPId, pszClassId, sizeof(m_szPnPId)/sizeof(m_szPnPId[0]));
    
    m_fValid = TRUE;
}

CAPODevice::~CAPODevice(void)
{
    if (NULL != m_pRegProps)
    {
        CoTaskMemFree(m_pRegProps);
        m_pRegProps = NULL;
    }
}

HRESULT CAPODevice::GetMixFormat
(
    IAudioMediaType   **ppAMTFormat
)
{
    CComPtr<IAudioClient>       pIAudioClient = NULL;
    HRESULT                     hr;
    IAudioMediaType            *pAMTReturn = NULL;
    
    if (NULL == ppAMTFormat)
    {
        return (E_POINTER);
    }
    
    *ppAMTFormat = NULL;
    
    if (DT_APO == m_lpType)
    {
        WAVEFORMATEXTENSIBLE        wfext;
        UINT32                      cbFormat;
        
        wfext.Format.wFormatTag           = WAVE_FORMAT_EXTENSIBLE;
        wfext.Format.nChannels            = 2;
        wfext.Format.nSamplesPerSec       = 48000;
        wfext.Format.wBitsPerSample       = 32;
        wfext.Format.nBlockAlign          = wfext.Format.nChannels
                                            * wfext.Format.wBitsPerSample
                                            / 8;
        wfext.Format.nAvgBytesPerSec      = wfext.Format.nBlockAlign
                                            * wfext.Format.nSamplesPerSec;
        wfext.Format.cbSize               = sizeof(WAVEFORMATEXTENSIBLE)
                                            - sizeof(WAVEFORMATEX);
        wfext.dwChannelMask               = SPEAKER_FRONT_LEFT
                                            | SPEAKER_FRONT_RIGHT;
        wfext.SubFormat                   = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
        wfext.Samples.wValidBitsPerSample = 32;
        cbFormat                          = sizeof(wfext);
        
        hr = CreateAudioMediaType((LPWAVEFORMATEX)&wfext, cbFormat, &pAMTReturn);
    }
    else
    {
        LPWAVEFORMATEX              pwfx = NULL;
        UINT32                      cbFormat;
        
        hr = m_pIEndpoint->Activate(
            __uuidof(IAudioClient),
            CLSCTX_ALL,
            NULL,
            (void**)&pIAudioClient);
    
        if (S_OK != hr)
        {
            return (hr);
        }
    
        if (S_OK != (hr = pIAudioClient->GetMixFormat(&pwfx)))
        {
            return (hr);
        }
    
        if (WAVE_FORMAT_PCM == pwfx->wFormatTag)
        {
            cbFormat = sizeof(PCMWAVEFORMAT);
        }
        else
        {
            cbFormat = sizeof(WAVEFORMATEX) + pwfx->cbSize;
        }
    
        hr = CreateAudioMediaType(pwfx, cbFormat, &pAMTReturn);
    
        CoTaskMemFree(pwfx);
    }
    
    if (S_OK != hr)
    {
        return (hr);
    }
        
    *ppAMTFormat = pAMTReturn;
    
    return (S_OK);
}

BOOL CAPODevice::NegotiateFormatsFromInput()
{
    CComPtr<IMMEndpoint>        pIEndpoint = NULL;
    EDataFlow                   df;
    HRESULT                     hr;
    
    if (DT_APO == m_lpType)
    {
        //  Doesn't matter for APO's.
        
        return (TRUE);
    }
    
    if (S_OK != (hr = m_pIEndpoint->QueryInterface(__uuidof(IMMEndpoint), (void**)&pIEndpoint)))
    {
        return (TRUE);
    }

    if (S_OK != (hr = pIEndpoint->GetDataFlow(&df)))
    {
        return (TRUE);
    }

    if (eRender == df)
    {
        if (DT_GFX == m_lpType)
        {
            //  Input side of the render GFX is the one that faces the mixer...
            return (TRUE);
        }
        else
        {
            //  Output side of the render LFX is the one that faces the mixer...
            return (FALSE);
        }
    }
    else
    {
        //  It's capture, assuming that it's an LFX since we don't allow
        //  capture GFX's...
        
        //  Input side of the capture LFX is the one that faces the mixer...
        return (TRUE);
    }
}

HRESULT CAPODevice::InitializeAPO
(
    IAudioProcessingObject     *pIAPO
)
{
    if(DT_SMEFX & GetDeviceType())
    {
        APOInitSystemEffects2                        sysfxInitParams2 = {0};
        // Setup initialization struct
        sysfxInitParams2.APOInit.cbSize                = sizeof(APOInitSystemEffects2);
        sysfxInitParams2.APOInit.clsid                 = m_gClsID;
        sysfxInitParams2.pReserved                     = NULL;
        sysfxInitParams2.pDeviceCollection             = m_pIDevCollection;
        sysfxInitParams2.pAPOEndpointProperties        = m_pIEPStore;
        sysfxInitParams2.pAPOSystemEffectsProperties   = m_pIFXStore;
        sysfxInitParams2.InitializeForDiscoveryOnly    = TRUE;
        sysfxInitParams2.AudioProcessingMode           = AUDIO_SIGNALPROCESSINGMODE_DEFAULT;
        sysfxInitParams2.nSoftwareIoDeviceInCollection = m_nSoftwareIoDeviceInCollection;
        sysfxInitParams2.nSoftwareIoConnectorIndex     = m_nSoftwareIoConnectorIndex;
        
        return pIAPO->Initialize(sysfxInitParams2.APOInit.cbSize, (BYTE*)(&sysfxInitParams2));
    }
    else if (DT_LGFX & GetDeviceType())
    {
        APOInitSystemEffects                        sysfxInitParams = {0};
        // Setup initialization struct
        sysfxInitParams.APOInit.cbSize              = sizeof(sysfxInitParams);
        sysfxInitParams.APOInit.clsid               = m_gClsID;
        sysfxInitParams.pAPOEndpointProperties      = m_pIEPStore;
        sysfxInitParams.pAPOSystemEffectsProperties = m_pIFXStore;
        sysfxInitParams.pDeviceCollection           = m_pIDevCollection;
        sysfxInitParams.pReserved                   = NULL;
        
        return (pIAPO->Initialize(sizeof(sysfxInitParams), (BYTE*)&sysfxInitParams));
    }
    else if (IsEqualGUID(m_gClsID, __uuidof(AudioVolume)))
    {
        APO_VOLUME_PARAMS           APOInitParams = {0};
        
        APOInitParams.APOInit.cbSize     = sizeof(APOInitParams);
        APOInitParams.APOInit.clsid      = m_gClsID;
        APOInitParams.u32SamplesPerFrame = gAPOLimits.Volume_u32SamplesPerFrame;
        APOInitParams.f32FrameRate       = gAPOLimits.Volume_f32FrameRate;
        
        return (pIAPO->Initialize(sizeof(APOInitParams), (BYTE*)&APOInitParams));
    }
    else if (IsEqualGUID(m_gClsID, __uuidof(AudioMatrix)))
    {
        APO_MATRIX_PARAMS           APOInitParams = {0};

        APOInitParams.APOInit.cbSize           = sizeof(APOInitParams);
        APOInitParams.APOInit.clsid            = m_gClsID;
        APOInitParams.u32InputSamplesPerFrame  = gAPOLimits.Matrix_u32InputSamplesPerFrame;
        APOInitParams.u32OutputSamplesPerFrame = gAPOLimits.Matrix_u32OutputSamplesPerFrame;
        APOInitParams.f32FrameRate             = gAPOLimits.Matrix_f32FrameRate;
        
        return (pIAPO->Initialize(sizeof(APOInitParams), (BYTE*)&APOInitParams));
    }
    else if (IsEqualGUID(m_gClsID, __uuidof(AudioRateConvert)))
    {
        AUDIO_RATE_CONVERT_PARAMS   APOInitParams = {0};

        APOInitParams.APOInit.cbSize          = sizeof(APOInitParams);
        APOInitParams.APOInit.clsid           = m_gClsID;
        APOInitParams.fInputFrameRate         = gAPOLimits.Rate_fInputFrameRate;
        APOInitParams.fOutputFrameRate        = gAPOLimits.Rate_fOutputFrameRate;
        APOInitParams.fMinInputFrameRate      = gAPOLimits.Rate_fMinInputFrameRate;
        APOInitParams.fMaxInputFrameRate      = gAPOLimits.Rate_fMaxInputFrameRate;
        APOInitParams.fMinOutputFrameRate     = gAPOLimits.Rate_fMinOutputFrameRate;
        APOInitParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fMaxOutputFrameRate;
        APOInitParams.eAlgorithmType          = gAPOLimits.Rate_eAlgorithmType;
        APOInitParams.eFlowType               = gAPOLimits.Rate_eFlowType;
        APOInitParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
        APOInitParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;
        
        return (pIAPO->Initialize(sizeof(APOInitParams), (BYTE*)&APOInitParams));
    }
    else if (IsEqualGUID(m_gClsID, __uuidof(AudioMeter)))
    {
        APO_METER_PARAMS            APOInitParams = {0};

        APOInitParams.APOInit.cbSize      = sizeof(APOInitParams);
        APOInitParams.APOInit.clsid       = m_gClsID;
        APOInitParams.u32SamplesPerFrame  = gAPOLimits.Meter_u32SamplesPerFrame;
        APOInitParams.u32FramesPerAverage = gAPOLimits.Meter_u32FramesPerAverage;
        APOInitParams.u32NumAverages      = gAPOLimits.Meter_u32NumAverages;
        APOInitParams.u32FramesPerQuantum = gAPOLimits.Meter_u32FramesPerQuantum;
        
        return (pIAPO->Initialize(sizeof(APOInitParams), (BYTE*)&APOInitParams));
    }
    else if (IsEqualGUID(m_gClsID, __uuidof(AudioRateConvertCMPT)))
    {
        AUDIO_RATE_CONVERT_PARAMS   APOInitParams = {0};

        APOInitParams.APOInit.cbSize          = sizeof(APOInitParams);
        APOInitParams.APOInit.clsid           = m_gClsID;
        APOInitParams.fInputFrameRate         = gAPOLimits.Rate_fInputFrameRate;
        APOInitParams.fOutputFrameRate        = gAPOLimits.Rate_fOutputFrameRate;
        APOInitParams.fMinInputFrameRate      = gAPOLimits.Rate_fMinInputFrameRate;
        APOInitParams.fMaxInputFrameRate      = gAPOLimits.Rate_fMaxInputFrameRate;
        APOInitParams.fMinOutputFrameRate     = gAPOLimits.Rate_fMinOutputFrameRate;
        APOInitParams.fMaxOutputFrameRate     = gAPOLimits.Rate_fMaxOutputFrameRate;
        APOInitParams.eAlgorithmType          = gAPOLimits.Rate_eAlgorithmType;
        APOInitParams.eFlowType               = gAPOLimits.Rate_eFlowType;
        APOInitParams.u32SamplesPerFrame      = gAPOLimits.Rate_u32SamplesPerFrame;
        APOInitParams.u32HighQualityTaplength = gAPOLimits.Rate_u32HighQualityTaplength;
        
        return (pIAPO->Initialize(sizeof(APOInitParams), (BYTE*)&APOInitParams));
    }
    else if (IsEqualGUID(m_gClsID, __uuidof(AudioLimiter)))
    {
        APO_LIMITER_PARAMS          APOInitParams = {0};

        APOInitParams.APOInit.cbSize     = sizeof(APOInitParams);
        APOInitParams.APOInit.clsid      = m_gClsID;
        APOInitParams.u32SamplesPerFrame = gAPOLimits.Limiter_u32SamplesPerFrame;
        APOInitParams.f32FrameRate       = gAPOLimits.Limiter_f32FrameRate;
        APOInitParams.bInDevicePipe      = gAPOLimits.Limiter_bInDevicePipe;
        
        return (pIAPO->Initialize(sizeof(APOInitParams), (BYTE*)&APOInitParams));
    }
    else
    {
        //  All other APO's...  This includes:
        //    AudioFormatConvert
        //    AudioMixer
        //    AudioCopy
        //    AudioConstrictor
        APOInitBaseStruct           APOInitParams = {0};
        
        //  Setup initiazliation struct
        APOInitParams.cbSize = sizeof(APOInitParams);
        APOInitParams.clsid  = m_gClsID;

        return (pIAPO->Initialize(sizeof(APOInitParams), (BYTE*)&APOInitParams));
    }

    //  We should never get here...    
}

BOOL CAPODevice::GetAPOInterfaces
(
    IAudioProcessingObject                **ppIAPO,
    IAudioProcessingObjectRT              **ppIAPORT,
    IAudioProcessingObjectConfiguration   **ppIAPOConfig
)
{
    BOOL        fReturn = TRUE;
    HRESULT     hr;

    if (NULL != ppIAPO)
    {
        if (S_OK != (hr = m_pIUnknown->QueryInterface(__uuidof(IAudioProcessingObject), (void**)ppIAPO)))
        {
            SLOG(eError, "Eh?!  CoCreateInstance for IAudioProcessingObject returned error (0x%08lx).", hr);
            fReturn = FALSE;
        }
    }
    
    if (NULL != ppIAPORT)
    {
        if (S_OK != (hr = m_pIUnknown->QueryInterface(__uuidof(IAudioProcessingObjectRT), (void**)ppIAPORT)))
        {
            SLOG(eError, "Eh?!  CoCreateInstance for IAudioProcessingObjectRT returned error (0x%08lx).", hr);
            fReturn = FALSE;
        }
    }
    
    if (NULL != ppIAPOConfig)
    {
        if (S_OK != (hr = m_pIUnknown->QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), (void**)ppIAPOConfig)))
        {
            SLOG(eError, "Eh?!  CoCreateInstance for IAudioProcessingObjectConfiguration returned error (0x%08lx).", hr);
            fReturn = FALSE;
        }
    }
    
    return (fReturn);
}


//--------------------------------------------------------------------------
//  CAPODeviceTestModule constructor
#ifndef BUILD_TAEF
CAPODeviceTestModule::CAPODeviceTestModule(CTestShell *pShell, HINSTANCE hInstDll)
: CDeviceTestModule(pShell, hInstDll), m_pDevice(NULL)
{
    m_dwAppID        = 1013;
    m_hInstance      = hInstDll;
    m_pstrModuleName = szModuleName;
    m_nIconID        = APPICON;
    
    m_rgTestCaseInfo = rgTestCaseInfo;
    m_cTestCases     = ARRAY_ELEMENTS(rgTestCaseInfo);
    m_dwWHQLChapter  = WHQL_CHAPTER_AUDIO;
    (void)CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
}
#else
CAPODeviceTestModule::CAPODeviceTestModule ( DV_TCINFO * ptcInfo )
: CDeviceTestModule(), m_pDevice(NULL)
{
    m_rgTestCaseInfo = ptcInfo;
    m_cTestCases     = 1;
    
    // Do we have a TAEF parameter to select the device?
    String targetDevice;
    if (SUCCEEDED(RuntimeParameters::TryGetValue(L"InstanceId", targetDevice)))
    {
        g_IShell->SetTestDevice(targetDevice);
    }
}
#endif

//--------------------------------------------------------------------------
//  ~CAPODeviceTestModule destructor

CAPODeviceTestModule::~CAPODeviceTestModule(VOID)
{
    //  Grr...  Sloppy, but calling CoUninitialize() will blow up b/c the
    //  devices use COM objects, and they're not deleted before this destructor
    //  is called.
//    CoUninitialize();
}

BOOL CAPODeviceTestModule::AddSysFxDevices()
{
    UINT                            iDevice;
    CComPtr<IMMDeviceEnumerator>    pIDeviceEnumerator = NULL;
    CComPtr<IMMDeviceCollection>    pDevices = NULL;
    CAPODevice                     *pSysFxDevice = NULL;
    HRESULT                         hr;
    
    if (S_OK != (hr = pIDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator))))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: CoCreateInstance returned error (0x%08lx).",
            hr);
            
        return (FNS_FAIL);
    }
    
    //  Since we're querying the endpoint for information now, we're enumerating only active endpoints.
    if (S_OK != (hr = pIDeviceEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &pDevices)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IMMDeviceEnumerator::EnumAudioEndpoints returned error (0x%08lx).",
            hr);
            
        return (FNS_FAIL);
    }
    
    if (S_OK != (hr = pDevices->GetCount(&iDevice)))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: IMMDeviceCollection::GetCount returned error (0x%08lx).",
            hr);
            
        return (FNS_FAIL);
    }
    
    for (; iDevice; iDevice--)
    {
        CComPtr<IMMDevice>              pIMMDevice = NULL;
        CComPtr<IMMEndpoint>            pIMMEndpoint = NULL;
        CComPtr<IMMEndpointInternal>    pIMMEndpointInt = NULL;
        CComPtr<IPropertyStore>         pIPropertyStoreDevice = NULL;
        CComPtr<IPropertyStore>         pIPropertyStoreFX = NULL;
        CComPtr<IDeviceTopology>        pIDeviceTopology = NULL;
        CComPtr<IConnector>             pIConnector = NULL;
        CCTMString                      strConnectedDevice = NULL;
        PROPVARIANT                     pvName;
        PROPVARIANT                     pv;
        PROPVARIANT                     pvProxy;
        BOOL                            bAPOProxy;

        
        if (S_OK != (hr = pDevices->Item(iDevice - 1, &pIMMDevice)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: IMMDeviceCollection::Item returned error (0x%08lx).",
                hr);
                
            return (FNS_FAIL);
        }
        
        if (S_OK != (hr = pIMMDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (void**)&pIDeviceTopology)))
        {
            if (E_NOINTERFACE == hr)
            {
                //  There's the possibility that this legit if device is not active.
                continue;
            }
            
            XLOG(
                XFAIL,
                eError,
                "FAIL: IMMDevice::Activate returned error (0x%08lx).",
                hr);
                
            return (FNS_FAIL);
        }
        
        if (S_OK != (hr = pIDeviceTopology->GetConnector(0, &pIConnector)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: IDeviceTopology::GetConnector returned error (0x%08lx).",
                hr);
                
            return (FNS_FAIL);
        }
        
        if (S_OK != (hr = pIConnector->GetDeviceIdConnectedTo(&strConnectedDevice)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: IDeviceTopology::GetDeviceIdConnectedTo returned error (0x%08lx).",
                hr);
                
            return (FNS_FAIL);
        }
        
        if (S_OK != (hr = pIMMDevice->QueryInterface(__uuidof(IMMEndpoint), (void**)&pIMMEndpoint)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: IMMDevice::QueryInterface returned error (0x%08lx).", hr);
                
            return (FNS_FAIL);
        }
        
        if (S_OK == (hr = pIMMEndpoint->QueryInterface(__uuidof(IMMEndpointInternal), (void**)&pIMMEndpointInt)))
        {
            // fine
        }
#ifdef WHQL
        else if (E_NOINTERFACE == hr)
        {
            // ... could still be fine...
            // ... in particular, we could be running a Win7 ExtSysFX Test on Vista
            // If that's the case, query for the old IMMEndpointInternal IID,
            // and make sure not to call into any of the new methods
            // (which won't exist.)
            SLOG(eInfo1, "IMMEndpoint::QueryInterface(IMMEndpointInternal) failed; trying Vista-era interface ID");

            // pulled from vista_rtm mmdeviceapip.h
            // {84087C38-1B58-422b-BF7E-55D2E3364199}
            IID IID_IMMEndpointInternalVista = {
                0x84087C38, 0x1B58, 0x422B, 0xBF, 0x7E, 0x55, 0xD2, 0xE3, 0x36, 0x41, 0x99
            };

            if (S_OK != (hr = pIMMEndpoint->QueryInterface(IID_IMMEndpointInternalVista, (void**)&pIMMEndpointInt)))
            {
                XLOG(
                    XFAIL,
                    eError,
                    "FAIL: IMMEndpoint::QueryInterface(IMMEndpointInternal) failed and Vista-era interface ID returned error (0x%08lx).",
                    hr);
                    
                return (FNS_FAIL);
            }
            
        }
#endif
        else
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: IMMEndpoint::QueryInterface returned error (0x%08lx).",
                hr);
                
            return (FNS_FAIL);
        }
        
        if (S_OK != (hr = pIMMDevice->OpenPropertyStore(STGM_READ, &pIPropertyStoreDevice)))
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: IMMDevice::OpenPropertyStore returned error (0x%08lx).",
                hr);
                
            return (FNS_FAIL);
        }
        
        if (S_OK != (hr = pIMMEndpointInt->OpenFXPropertyStore(STGM_READ, &pIPropertyStoreFX)))
        {
            if (HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) == hr)
            {
                //  Plausible...  This device probably doesn't have any SysFx...
                //  Just skip!
                
                continue;
            }
        
            XLOG(
                XFAIL,
                eError,
                "FAIL: IMMEndpointInternal::OpenFXPropertyStore returned error "
                    "(0x%08lx).",
                hr);
                
            return (FNS_FAIL);
        }
        
        PropVariantInit(&pvName);
        if (S_OK == (hr = pIPropertyStoreDevice->GetValue(PKEY_Device_FriendlyName, &pvName)))
        {
            if (VT_LPWSTR != V_VT(&pvName))
            {
                SLOG(eError, "FriendlyName value is not a string.");
            }
        }
        else
        {
            SLOG(eError, "IPropertyStore::GetValue returned error.");
        }
        

        OSVERSIONINFO ver = {0};
        ver.dwOSVersionInfoSize = sizeof(ver);

        if (!GetVersionEx(&ver))
        {
            XLOG(
                XFAIL, 
                eFatalError, 
                "FAIL: GetVersionEx() failed: GetLastError() = %u", GetLastError());
        }

        for(int i = 0; i < ARRAYSIZE(PKEY_FX_EFFECTS); i++)
        {
            if (ver.dwMajorVersion < PKEY_FX_EFFECTS[i].dwMajorVersion || 
               (ver.dwMajorVersion == PKEY_FX_EFFECTS[i].dwMajorVersion &&
                ver.dwMinorVersion < PKEY_FX_EFFECTS[i].dwMinorVersion))
            {
	            continue;
            }

            PropVariantInit(&pv);
            if (S_OK == (hr = pIPropertyStoreFX->GetValue(PKEY_FX_EFFECTS[i].key, &pv)))
            {
                if (VT_LPWSTR == V_VT(&pv))
                {
                    bAPOProxy = FALSE;
                    PropVariantInit(&pvProxy);
                    
                    // Check if mode effect is proxy
                    if (PKEY_FX_EFFECTS[i].apoType & DT_MFX)
                    {
                        if (S_OK == (hr = pIPropertyStoreFX->GetValue(PKEY_MFX_ProcessingModes_Supported_For_Streaming, &pvProxy)))
                        {
                            bAPOProxy = (pvProxy.vt != (VT_VECTOR | VT_LPWSTR)) ? TRUE : FALSE;
                        }
                    }
                    
                    // Check if endpoint effect is proxy
                    if (PKEY_FX_EFFECTS[i].apoType & DT_EFX)
                    {
                        if (S_OK == (hr = pIPropertyStoreFX->GetValue(PKEY_EFX_ProcessingModes_Supported_For_Streaming, &pvProxy)))
                        {
                            bAPOProxy = (pvProxy.vt != (VT_VECTOR | VT_LPWSTR)) ? TRUE : FALSE;
                        }
                    }

                    pSysFxDevice = new CAPODevice(pIMMDevice, pv.pwszVal, pvName.pwszVal, pIPropertyStoreDevice, pIPropertyStoreFX, PKEY_FX_EFFECTS[i].apoType, strConnectedDevice, bAPOProxy);

                            
                    if (NULL != pSysFxDevice)
                    {
                        if (pSysFxDevice->IsValid())
                        {
                            pSysFxDevice->m_fSelected = TRUE;
                            g_IShell->AddDevice(pSysFxDevice);
                        }
                        else
                        {
                            delete pSysFxDevice;
                            pSysFxDevice = NULL;
                        }
                    }
                }
                PropVariantClear(&pvProxy);
            }
            PropVariantClear(&pv);
        }
        PropVariantClear(&pvName);
    }
    
    return (TRUE);
}

BOOL CAPODeviceTestModule::AddAPODevices()
{
    HKEY                hKeyAPO = NULL;
    LONG                lRet;
    DWORD               cSubKeys;
    DWORD               cchSubKey;
    DWORD               ii;
    WCHAR               szwGuid[80];  //  40 should be sufficient, being safe.
    DWORD               cchGuid;
    FILETIME            ft;
    BOOL                fRes = TRUE;
    
    lRet = RegOpenKeyEx(
            HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Classes\\AudioEngine\\AudioProcessingObjects",
            0,
            KEY_READ,
            &hKeyAPO);
    
    if (ERROR_FILE_NOT_FOUND == lRet)
    {
        //  This registry key does not exist, let's use our private APO
        //  list instead.
        return (AddAPODevicesWithInternalList());
    }
    
    if (ERROR_SUCCESS != lRet)
    {
        XLOG(
            XFAIL,
            eError,
            "RegOpenKeyEx failed with %d.",
            lRet);
        fRes = FALSE;
        goto CleanUp;
    }
    
    lRet = RegQueryInfoKey(
            hKeyAPO,
            NULL,           //  Class
            NULL,           //  Class size
            NULL,           //  Reserved
            &cSubKeys,      //  Number of subkeys
            &cchSubKey,     //  Max key length
            NULL,           //  Max class length
            NULL,           //  Number of values
            NULL,           //  Max value name length
            NULL,           //  Max value length
            NULL,           //  Security descriptor
            NULL);          //  Last write time

    if (ERROR_SUCCESS != lRet)
    {
        XLOG(
            XFAIL,
            eError,
            "RegQueryInfoKey failed with %d.",
            lRet);
        fRes = FALSE;
        goto CleanUp;
    }
    
    if (cchSubKey > ARRAYSIZE(szwGuid))
    {
        XLOG(
            XFAIL,
            eError,
            "Subkey length (%u) is longer than a GUID.",
            cchSubKey);
        fRes = FALSE;
        goto CleanUp;
    }
    
    for (ii = 0; ii < cSubKeys; ++ii)
    {
        CComPtr<IAudioProcessingObject>   pIAPO;
        CAPODevice                       *pAPODevice = NULL;
        
        cchGuid = (sizeof(szwGuid)/sizeof(szwGuid[0]));
        
        lRet = RegEnumKeyExW(
                hKeyAPO,
                ii,
                szwGuid,
                &cchGuid,
                NULL,       //  Reserved
                NULL,       //  Class
                NULL,       //  size of Class
                &ft);
            
        if (ERROR_SUCCESS != lRet)
        {
            XLOG(
                XFAIL,
                eError,
                "RegEnumKeyExW(..., %u, ...) failed with %d.",
                ii,
                lRet);
            fRes = FALSE;
            // soldier on
            continue;
        }
                
        if (AddAPODevice(szwGuid, &pIAPO))
        {
            // AddAPODevice succeeded
            // check pIAPO for NULL
            if (NULL != pIAPO)
            {
                pAPODevice = new CAPODevice(pIAPO, szwGuid);
                    
                if (NULL != pAPODevice)
                {
                    if (pAPODevice->IsValid())
                    {
                        pAPODevice->m_fSelected = TRUE;
                        g_IShell->AddDevice(pAPODevice);
                    }
                    else
                    {
                        delete pAPODevice;
                        pAPODevice = NULL;
                    }
                }
            }
        }
        else
        {
            fRes = FALSE;
        }
    }

CleanUp:

    if (NULL != hKeyAPO)
    {
        RegCloseKey(hKeyAPO);
    }
    
    return fRes;
}

BOOL CAPODeviceTestModule::AddAPODevicesWithInternalList()
{
    BOOL    fRes = TRUE;
    LPWSTR  aszAPOClassIds[] =
    {
        L"{06587E71-F043-403A-BF49-CB591BA6E103}",  //  CAudioVolume
        L"{07252659-BB6B-4B79-B78B-623F6699A579}",  //  CAudioConstrictor
        L"{12DD4DBB-532B-4FCE-8653-74CDB9C8FE5A}",  //  CAudioMixer
        L"{27C98999-2895-4829-B080-5A8B65BD3DB0}",  //  CAudioRateConvertCMPT
        L"{3DC09436-7D83-4BA0-ADDC-CD47F996C5BA}",  //  CAudioMeter
        L"{3FD7F233-A716-472E-8F2F-C25954F34E96}",  //  CAudioFormatConvert
        L"{541987EE-0E02-411E-9A85-1FC6156E7F4B}",  //  CAudioMatrix
        L"{C58BD103-E87F-4B78-A0FA-7A5C95970EE2}",  //  CAudioRateConvert
        L"{D69E0717-DD4B-4B25-997A-DA813833B8AC}",  //  CAudioLimiter
        L"{E916B6B2-22BD-4AFC-B337-D3D9FB27670E}",  //  CAudioCopy
    };

    for (UINT ii = 0; ii < ARRAYSIZE(aszAPOClassIds); ++ii)
    {
        CComPtr<IAudioProcessingObject>   pIAPO;
        CAPODevice                       *pAPODevice = NULL;
        
        if (AddAPODevice(aszAPOClassIds[ii], &pIAPO))
        {
            // AddAPODevice succeeded
            // check pIAPO for NULL
            if (NULL != pIAPO)
            {
                pAPODevice = new CAPODevice(pIAPO, aszAPOClassIds[ii]);
                    
                if (NULL != pAPODevice)
                {
                    if (pAPODevice->IsValid())
                    {
                        pAPODevice->m_fSelected = TRUE;
                        g_IShell->AddDevice(pAPODevice);
                    }
                    else
                    {
                        delete pAPODevice;
                        pAPODevice = NULL;
                    }
                }
            }
        }
        else
        {
            fRes = FALSE;
        }
    }

    return (fRes);
}

// can't use destructors and __try / __catch in the same function
BOOL CAPODeviceTestModule::AddAPODevice(LPCWSTR szAPOGuid, IAudioProcessingObject **ppIAPO)
{
    HRESULT                 hr = S_OK;
    BOOL                    fRes = TRUE;
    IID                     iidAPO;
    IAudioProcessingObject *pIAPO = NULL;
    IAudioSystemEffects    *pISysFx = NULL;
    DWORD                   dwExcept = 0;
    
    SLOG(eInfo1, "Looking at Audio Processing Object %ls", szAPOGuid);

    if (S_OK != (hr = IIDFromString(szAPOGuid, &iidAPO)))
    {
        XLOG(
            XFAIL,
            eError,
            "IIDFromString(%ls) failed with 0x%08x.",
            szAPOGuid,
            hr);
        fRes = FALSE;
        goto Cleanup;
    }

    // APOs have a nasty habit of blowing up if you look at them funny
    // good thing we took them out of kernel mode!
    __try
    {
        hr = CoCreateInstance(
            iidAPO,
            NULL,
            CLSCTX_INPROC_SERVER,
            __uuidof(IAudioProcessingObject),
            (void**)&pIAPO);
    }
    __except (
        (EXCEPTION_ACCESS_VIOLATION == (dwExcept = GetExceptionCode())) ?
        EXCEPTION_EXECUTE_HANDLER :
        EXCEPTION_CONTINUE_SEARCH
    )
    {
        XLOG(
            XFAIL, eError,
            "FAIL: CoCreateInstance on Audio Processing Object "
            "with Interface ID %ls threw exception 0x%08x. "
            "Please follow up with the vendor for this Audio Processing Object; "
            "the COM implementation needs to be fixed.",
            szAPOGuid,
            dwExcept);
        fRes = FALSE;
        goto Cleanup;
    }
    
    if (S_OK != hr)
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: CoCreateInstance on Audio Processing Object "
            "with Interface ID %ls failed with HRESULT 0x%08x. "
            "Please follow up with the vendor for this Audio Processing Object; "
            "the COM implementation needs to be fixed.",
            szAPOGuid,
            hr);
        fRes = FALSE;
        goto Cleanup;
    }
    
    __try
    {
        hr = pIAPO->QueryInterface(__uuidof(IAudioSystemEffects), (void**)&pISysFx);
    }
    __except (
        (EXCEPTION_ACCESS_VIOLATION == (dwExcept = GetExceptionCode())) ?
        EXCEPTION_EXECUTE_HANDLER :
        EXCEPTION_CONTINUE_SEARCH
    )
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: After instantiating the Audio Processing Object with Interface ID %ls "
            "IAudioProcessingObject::QueryInterface(IAudioSystemEffects) threw exception 0x%08x. "
            "Please follow up with the vendor for this Audio Processing Object; "
            "the COM implementation needs to be fixed.",
            szAPOGuid,
            dwExcept);
        fRes = FALSE;
        goto Cleanup;
    }

    if (S_OK == hr)
    {
        // this is a SysFX
        // don't bubble it up because it got enumerated already
        goto Cleanup;
    }
    
    if (E_NOINTERFACE != hr) {
        XLOG(
            XFAIL, // relying on this to bubble up the failure
            eError,
            "FAIL: After instantiating the Audio Processing Object with Interface ID %ls "
            "IAudioProcessingObject::QueryInterface(IAudioSystemEffects) failed "
            "with unexpected HRESULT 0x%08x (expected S_OK or E_NOINTERFACE.) "
            "Please follow up with the vendor for this Audio Processing Object; "
            "the COM implementation needs to be fixed.",
            szAPOGuid,
            hr);
        // this should not block further testing of the APO.
        // fRes NOT = FALSE
        // DON'T goto Cleanup;
    }
    
    //  If the QI for IAudioSystemEffects failed, this is just an APO, not a SysFx...
    //  Install it as such.
    __try
    {
        pIAPO->AddRef();
    }
    __except (
        (EXCEPTION_ACCESS_VIOLATION == (dwExcept = GetExceptionCode())) ?
        EXCEPTION_EXECUTE_HANDLER :
        EXCEPTION_CONTINUE_SEARCH
    )
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: After instantiating the Audio Processing Object with Interface ID %ls "
            "IAudioProcessingObject::AddRef() threw exception 0x%08x."
            "Please follow up with the vendor for this Audio Processing Object; "
            "the COM implementation needs to be fixed.",
            szAPOGuid,
            dwExcept);
        fRes = FALSE;
        goto Cleanup;
    }
    
    *ppIAPO = pIAPO;

Cleanup:

    if (NULL != pIAPO)
    {
        __try
        {
            pIAPO->Release();
        }
        __except (
            (EXCEPTION_ACCESS_VIOLATION == (dwExcept = GetExceptionCode())) ?
            EXCEPTION_EXECUTE_HANDLER :
            EXCEPTION_CONTINUE_SEARCH
        )
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: After instantiating the Audio Processing Object with Interface ID %ls "
                "IAudioProcessingObject::Release() threw exception 0x%08x. "
                "Please follow up with the vendor for this Audio Processing Object; "
                "the COM implementation needs to be fixed.",
                szAPOGuid,
                dwExcept);
            fRes = FALSE;
        }
    }
    
    if (NULL != pISysFx)
    {
        __try
        {
            pISysFx->Release();
        }
        __except (
            (EXCEPTION_ACCESS_VIOLATION == (dwExcept = GetExceptionCode())) ?
            EXCEPTION_EXECUTE_HANDLER :
            EXCEPTION_CONTINUE_SEARCH
        )
        {
            XLOG(
                XFAIL,
                eError,
                "FAIL: After instantiating the Audio Processing Object with Interface ID %ls "
                "IAudioSystemEffects::Release() threw exception 0x%08x. "
                "Please follow up with the vendor for this Audio Processing Object; "
                "the COM implementation needs to be fixed.",
                szAPOGuid,
                dwExcept);
            fRes = FALSE;
        }
    }
        
    return fRes;
}

DWORD CAPODeviceTestModule::OnInitialUpdate(VOID)
{
    if (!AddSysFxDevices())
    {
        return (FNS_FAIL);
    }
    
#ifdef TESTAPOS
    if (!AddAPODevices())
    {
        return (FNS_FAIL);
    }
#endif    
    
    return (FNS_PASS);
} // OnInitialUpdate


//--------------------------------------------------------------------------
//  CAPODeviceTestModule::SetTestDevice handler

VOID CAPODeviceTestModule::SetTestDevice(CDeviceDescriptor* pDevice)
{
    m_pDevice = (CAPODevice*)pDevice;
}

//--------------------------------------------------------------------------
//  CAPODeviceTestModule::RunTest

DWORD CAPODeviceTestModule::RunTest(DWORD dwTestCase)
{
    DWORD dwRes = FNS_PASS;
    dwRes = ((PDV_TCINFO)m_rgTestCaseInfo)[dwTestCase].pfnTest();
    return dwRes;
}
