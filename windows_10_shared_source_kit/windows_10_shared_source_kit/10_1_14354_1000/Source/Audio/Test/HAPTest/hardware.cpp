// Copyright(C) Microsoft.All rights reserved.

// hardware.cpp

#include <atlbase.h>
#include <atlstr.h>
#include <mmeapi.h>
#include <mmsystem.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <devicetopology.h>
#include <strsafe.h>

#include <basiclog.h>
#include <BasicLogHelper.h>
#include <basicprintf.h>

#include <devtrans.h>

#include <WexTestClass.h>

#include "haptest.h"
#include "log.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;

enum HapTest_Hardware {
    HapTest_HardwareVolume,
    HapTest_HardwareMute,
    HapTest_HardwarePeakMeter,
};

void HardwareSupport(HapTest_Hardware what);

void CHapTest::HardwareVolume()    { HardwareSupport(HapTest_HardwareVolume);    }
void CHapTest::HardwareMute()      { HardwareSupport(HapTest_HardwareMute);      }
void CHapTest::HardwarePeakMeter() { HardwareSupport(HapTest_HardwarePeakMeter); }

void HardwareSupport(HapTest_Hardware what) {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    CComPtr<IBasicLog> pLogger;
    if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return; }
    
    CComPtr<IMMDevice> pEndpoint;
    if (!DeviceUnderTest(pLogger, &pEndpoint)) { return; }
    
    // get a list of all streaming pins associated with this endpoint
    // this might include the loopback pin
    // the loopback pin shouldn't support modes, so that's fine
    CComHeapPtr<UINT> pPinIds;
    UINT nPinCount;
    CComPtr<IMMDevice> pFilter;
    if (!VERIFY_SUCCEEDED(
        DevTrans::EndpointToKSStreamPins(pEndpoint, &pPinIds, &nPinCount, &pFilter)
    )) { return; }
    
    if (!VERIFY_IS_GREATER_THAN(nPinCount, 0ul)) { return; }
    
    // see what modes are supported by the various streaming pins
    CComPtr<IKsControl> pKsControl;
    if (!VERIFY_SUCCEEDED(
        pFilter->Activate(
            __uuidof(IKsControl),
            CLSCTX_ALL,
            NULL,
            reinterpret_cast<void**>(&pKsControl)
    ))) { return; }
    
    CSimpleArray<GUID> modes;
    for (UINT i = 0; i < nPinCount; i++) {
        UINT id = pPinIds[i];
        
        CComHeapPtr<GUID> pModes;
        UINT nModes = 0;
        
        if (!VERIFY_SUCCEEDED(
            GetModes(pKsControl, id, &pModes, &nModes)
        )) { return; }
        
        LOG(L"Modes supported by pin %u: %u", id, nModes);
        
        // add to the global list of modes
        for (UINT m = 0; m < nModes; m++) {
            if (-1 == modes.Find(pModes[m])) {
                modes.Add(pModes[m]);
            }
        }
    }
    
    LOG(L"Total number of modes supported across all pins: %d", modes.GetSize());
    
    DWORD requiredSupport = 0;
    if (modes.GetSize() > 1) {
        // multi-mode drivers are required to support the following
        // render multi-mode drivers are also required to support loopback;
        // we don't test that explicitly here
        //
        // compressed-output-capable drivers are also required to support mute;
        // we don't test that explicitly here either
        switch (what) {
            case HapTest_HardwareVolume:
                requiredSupport = ENDPOINT_HARDWARE_SUPPORT_VOLUME;
                break;
                
            case HapTest_HardwareMute:
                requiredSupport = ENDPOINT_HARDWARE_SUPPORT_MUTE;
                break;
            
            case HapTest_HardwarePeakMeter:
                requiredSupport = ENDPOINT_HARDWARE_SUPPORT_METER;
                break;
        }
    }
    
    DWORD actualSupport = 0;
    switch (what) {
        case HapTest_HardwareVolume:
        case HapTest_HardwareMute: {
            CComPtr<IAudioEndpointVolume> pAudioEndpointVolume;
            if (!VERIFY_SUCCEEDED(
                pEndpoint->Activate(
                    __uuidof(IAudioEndpointVolume),
                    CLSCTX_ALL,
                    NULL,
                    reinterpret_cast<void**>(&pAudioEndpointVolume)
                )
            )) { return; }
            
            if (!VERIFY_SUCCEEDED(
                pAudioEndpointVolume->QueryHardwareSupport(&actualSupport)
            )) { return; }
        }
        
        case HapTest_HardwarePeakMeter: {
            CComPtr<IAudioMeterInformation> pAudioMeterInformation;
            if (!VERIFY_SUCCEEDED(
                pEndpoint->Activate(
                    __uuidof(IAudioMeterInformation),
                    CLSCTX_ALL,
                    NULL,
                    reinterpret_cast<void**>(&pAudioMeterInformation)
                )
            )) { return; }
            
            if (!VERIFY_SUCCEEDED(
                pAudioMeterInformation->QueryHardwareSupport(&actualSupport)
            )) { return; }
            break;
        }
            
        default:
            ERR(L"Unexpected value %u passed to HardwareSupport", what);
    }
    
    LOG(L"ENDPOINT_HARDWARE_SUPPORT_XXX required support: 0x%x; actual support 0x%x", requiredSupport, actualSupport);

    // enforce that the required support is actually supported
    VERIFY_ARE_EQUAL(actualSupport & requiredSupport, requiredSupport);
}
