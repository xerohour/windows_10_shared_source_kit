// Copyright(C) Microsoft.All rights reserved.

// buffer.cpp

#include <atlbase.h>
#include <atlstr.h>
#include <ks.h>
#include <ksmedia.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <endpointvolume.h>
#include <strsafe.h>

#include <basiclog.h>
#include <BasicLogHelper.h>
#include <basicprintf.h>

#include <WexTestClass.h>

#include "haptest.h"
#include "log.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;

void CHapTest::BufferSize() {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    CComPtr<IBasicLog> pLogger;
    if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return; }
    
    CComPtr<IMMDevice> pMMDevice;
    if (!DeviceUnderTest(pLogger, &pMMDevice)) { return; }
    
    CComPtr<IAudioClient2> pAudioClient2;
    if (!VERIFY_SUCCEEDED(
        pMMDevice->Activate(
            __uuidof(IAudioClient2),
            CLSCTX_ALL,
            NULL,
            reinterpret_cast<void**>(&pAudioClient2)
    ))) { return; }
    
    AudioClientPropertiesWin8 clientProperties = {0};
    clientProperties.bIsOffload = TRUE;
    clientProperties.eCategory = AudioCategory_Media;
    clientProperties.cbSize = sizeof(clientProperties);
#pragma prefast(suppress: __WARNING_BUFFER_OVERFLOW, "client properties is variable-sized")
    if (!VERIFY_SUCCEEDED(
        pAudioClient2->SetClientProperties(
            reinterpret_cast<AudioClientProperties *>(&clientProperties)
        )
    )) { return; }
 
    // buffer size limits assume 48 kHz 16-bit stereo PCM
    WAVEFORMATEXTENSIBLE wfx = { 0 };
    wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    wfx.Format.nChannels = 2;
    wfx.Format.nSamplesPerSec = 48000;
    wfx.Format.wBitsPerSample = 16;
    wfx.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
    wfx.Format.nBlockAlign = wfx.Format.nChannels * wfx.Format.wBitsPerSample / BITS_PER_BYTE;
    wfx.Format.nAvgBytesPerSec = wfx.Format.nBlockAlign * wfx.Format.nSamplesPerSec;
    wfx.Samples.wValidBitsPerSample = wfx.Format.wBitsPerSample;
    wfx.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
    wfx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
    
    // let's make sure this format is supported
    CComHeapPtr<WAVEFORMATEX> pClosest;
    HRESULT hr = pAudioClient2->IsFormatSupported(
            AUDCLNT_SHAREMODE_SHARED,
            &wfx.Format,
            &pClosest // necessary or IsFormatSupported returns E_POINTER
    );
    
    switch (hr) {
        case S_OK:
            // great
            break;
        case AUDCLNT_E_UNSUPPORTED_FORMAT:
            WARN(
                L"Offload path doesn't support 48 kHz 16-bit stereo PCM in WAVE_FORMAT_EXTENSIBLE; "
                L"trying WAVE_FORMAT_PCM"
            );
            wfx.Format.wFormatTag = WAVE_FORMAT_PCM;
            wfx.Format.cbSize = 0;
            pClosest.Free();
            if (!VERIFY_SUCCEEDED(
                pAudioClient2->IsFormatSupported(
                    AUDCLNT_SHAREMODE_SHARED,
                    &wfx.Format,
                    &pClosest // necessary or IsFormatSupported returns E_POINTER
            ))) { return; }
            break;
        default:
            ERR(
                L"Unexpected HRESULT 0x%08x returned from IAudioClient2::IsFormatSupported "
                L"with 48 kHz 16-bit stereo PCM in WAVE_FORMAT_EXTENSIBLE",
                hr
            );
            return;
    }
    
    // now let's get the buffer size limits
    REFERENCE_TIME hnsMin;
    REFERENCE_TIME hnsMax;
    
    if (!VERIFY_SUCCEEDED(
        pAudioClient2->GetBufferSizeLimits(
            &wfx.Format,
            TRUE, // event-driven
            &hnsMin,
            &hnsMax
    ))) { return; }
    
    LOG(
        L"Min/max buffer sizes at stereo 16-bit 48 kHz PCM: "
        L"%I64u hns (%g ms), "
        L"%I64u hns (%g ms)",
        hnsMin, 1.0 * hnsMin / HNS_PER_MS,
        hnsMax, 1.0 * hnsMax / HNS_PER_MS
    );
    
    // min must be no more than 10 ms
    VERIFY_IS_LESS_THAN_OR_EQUAL(hnsMin, (REFERENCE_TIME)10 * HNS_PER_MS);
    
    // max must be no less than 1 full second
    VERIFY_IS_GREATER_THAN_OR_EQUAL(hnsMax, (REFERENCE_TIME)MS_PER_SECOND * HNS_PER_MS);
}
