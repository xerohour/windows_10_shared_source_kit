// Copyright(C) Microsoft.All rights reserved.

// loopbackprotect.cpp

#include <atlbase.h>
#include <atlstr.h>
#include <audioclient.h>
#include <mmeapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <devicetopology.h>
#include <strsafe.h>
#include <math.h>
#include <mfidl.h>
#include <attributesbase.h>
#include <audioenginebaseapo.h>

#include <basiclog.h>
#include <BasicLogHelper.h>
#include <basicprintf.h>

#include <audiostreaming.h>
#include <sine-signal-source.h>
#include <soundanalyze.h>
#include <waveio.h>
#include <offloadpath.h>

#include <WexTestClass.h>

#include "haptest.h"
#include "log.h"
#include "outputpolicy.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;

#define FREQUENCY_HALF_WIDTH 50

#define SHARED  0
#define OFFLOAD 1

double SecondsFromFrames(UINT32 nFrames, const WAVEFORMATEX *pWfx) {
    return (double)nFrames / pWfx->nSamplesPerSec;
}

UINT32 FramesFromSeconds(double seconds, const WAVEFORMATEX *pWfx) {
    return (UINT32)(seconds * pWfx->nSamplesPerSec + 0.5);
}

void LoopbackProtect(GUID guidProtectionScheme, DWORD dwConfigData);

void CHapTest::LoopbackProtectMute() {
    // none of the other constriction options are supported, by design
    // (well, "no constriction" is)
    LoopbackProtect(MFPROTECTION_CONSTRICTAUDIO, MAKE_MFPROTECTIONDATA_CONSTRICTAUDIO(eAudioConstrictionMute));
}

void CHapTest::LoopbackProtectDisable() {
    LoopbackProtect(MFPROTECTION_DISABLE, MAKE_MFPROTECTIONDATA_DISABLE(1));
}

//
// Approximate timeline
//
// T = -1.0 Start playback
// T =  0.0 Start loopback capture
// T =  1.0 Set policy
// T =  2.0 Release policy
// T =  3.0 Stop loopback capture
//
// Verify that T = 0.3 to 0.7 has signal;
// Verify that T = 1.3 to 1.7 has silence;
// Verify that T = 2.3 to 2.7 has signal again.
//
void LoopbackProtect(GUID guidProtectionScheme, DWORD dwConfigData) {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    CComPtr<IBasicLog> pLogger;
    if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return; }
    
    CComPtr<IMMDevice> pMMDevice;
    bool bSysvad = false;
    if (!DeviceUnderTest(pLogger, &pMMDevice, &bSysvad)) { return; }
    if (bSysvad)
    {
        Log::Result(TestResults::Result::Skipped, L"Virtual Audio Device doesn't support loopback");
        return;
    }

    CNormalizeVolumeAndRestoreOnExit normalizeVolume(pMMDevice);

    //
    // T = -1.0
    //
    // stream in both offload and shared mode
    // grab the "extract" output trust authority for each stream while you're at it
    float freq[] = { 281.372f, 632.388f };
    double dbOriginalPower = -40.0;
    
    LOG(L"Playing %g Hz tone in shared mode and %g Hz tone in offload mode", freq[0], freq[1]);
    CComPtr<IAudioStreamingRender> pAudioStreamingRenders[2];
    CComPtr<IMFOutputTrustAuthority> pMFOutputTrustAuthorities[2];
    
    for (int stream = SHARED; stream <= OFFLOAD; stream++) {
        if (!VERIFY_SUCCEEDED(
            CreateAudioStreamingRender(
                api_WASAPI,
                pMMDevice,
                pLogger,
                &pAudioStreamingRenders[stream]
        ))) { return; }
        
        CComPtr<IAudioStreamingControl> pAudioStreamingControl;
        if (!VERIFY_SUCCEEDED(
            pAudioStreamingRenders[stream]->QueryInterface(
                __uuidof(IAudioStreamingControl),
                reinterpret_cast<void**>(&pAudioStreamingControl)
        ))) { return; }
        
        CComPtr<IAudioStreamingWasapiControl> pAudioStreamingWasapiControl;
        if (!VERIFY_SUCCEEDED(
            pAudioStreamingControl->GetService(
                __uuidof(IAudioStreamingWasapiControl),
                reinterpret_cast<void**>(&pAudioStreamingWasapiControl)
        ))) { return; }

        if (OFFLOAD == stream) {
            // force offload mode
            AudioClientPropertiesWin8 clientProperties = {0};
            clientProperties.bIsOffload                = TRUE;
            clientProperties.eCategory                 = AudioCategory_Media;
            clientProperties.cbSize                    = sizeof(clientProperties);

#pragma prefast(suppress: __WARNING_BUFFER_OVERFLOW, "client properties is variable-sized")
            if (!VERIFY_SUCCEEDED(
                pAudioStreamingWasapiControl->SetClientProperties(
                    reinterpret_cast<AudioClientProperties*>(&clientProperties)
                )
            )) { return; }

            if (!VERIFY_SUCCEEDED(
                pAudioStreamingWasapiControl->SetAudioClientInitializeParameters(
                    AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                    200000,
                    200000,
                    GUID_NULL
                )
            )) { return; }
        }

        if (!VERIFY_SUCCEEDED(pAudioStreamingRenders[stream]->Initialize(nullptr))) { return; }

        // start playing
        if (!VERIFY_SUCCEEDED(
            pAudioStreamingRenders[stream]->PlaySineWaveAsync(
                freq[stream],
                static_cast<float>(SineAmpFromDb(dbOriginalPower)),
                Method_TPD
            )
        )) { return; }
        
        // get the "extract" output trust authority
        CComPtr<IAudioClient> pAudioClient;
        if (!VERIFY_SUCCEEDED(
            pAudioStreamingControl->GetService(IID_PPV_ARGS(&pAudioClient))
        )) { return; }
        
        CComPtr<IMFTrustedOutput> pMFTrustedOutput;
        if (!VERIFY_SUCCEEDED(
            pAudioClient->GetService(IID_PPV_ARGS(&pMFTrustedOutput))
        )) { return; }

        DWORD dwCountOfOTAs;
        if (!VERIFY_SUCCEEDED(pMFTrustedOutput->GetOutputTrustAuthorityCount(&dwCountOfOTAs))) { return; }
        if (!VERIFY_IS_GREATER_THAN(dwCountOfOTAs, (DWORD)0)) { return; }

        bool bSawExtract = false;
        for (DWORD ota = 0; ota < dwCountOfOTAs; ota++) {
            CComPtr<IMFOutputTrustAuthority> pMFOutputTrustAuthority;
            if (!VERIFY_SUCCEEDED(pMFTrustedOutput->GetOutputTrustAuthorityByIndex(ota, &pMFOutputTrustAuthority))) {
                // fail but continue on anyway
                continue;
            }

            MFPOLICYMANAGER_ACTION action;
            if (!VERIFY_SUCCEEDED(pMFOutputTrustAuthority->GetAction(&action))) {
                // fail but continue on anyway
                continue;
            }

            if (PEACTION_EXTRACT != action) {
                // we're only interested in the extraction output trust authority
                continue;
            }
            
            if (!VERIFY_IS_FALSE(bSawExtract)) {
                ERR(L"We already saw at least one extraction output trust authority.  There should only be one per trusted output.");
                continue;
            }
            
            bSawExtract = true;
            pMFOutputTrustAuthorities[stream] = pMFOutputTrustAuthority;
        } // for each output trust authority

        if (!VERIFY_IS_TRUE(bSawExtract)) {
            ERR(L"There were no extraction policies on this trusted output.");
            return;
        }
    }
    
    // stop the streams on exit
    CStopOnExit<IAudioStreamingRender> stopSharedRender(pAudioStreamingRenders[0]);
    CStopOnExit<IAudioStreamingRender> stopOffloadRender(pAudioStreamingRenders[1]);
    
    LOG(L"T = -1; render streams are started.");
    Sleep(1000);
    
    //
    // T = 0.0
    //
    // now start the loopback capture stream
    CComPtr<IAudioStreamingCapture> pLoopback;
    if (!VERIFY_SUCCEEDED(
        CreateAudioStreamingCapture(
            api_WASAPI,
            pMMDevice,
            pLogger,
            &pLoopback
    ))) { return; }
    
    // set the loopback capture object to loopback mode
    CComPtr<IAudioStreamingControl> pAudioStreamingControl;
    if (!VERIFY_SUCCEEDED(
        pLoopback->QueryInterface(
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

    if (!VERIFY_SUCCEEDED(pLoopback->Initialize(NULL))) { return; }
    CComHeapPtr<WAVEFORMATEX> pWfxLoopback;
    if (!VERIFY_SUCCEEDED(pLoopback->GetWaveFormat(&pWfxLoopback))) { return; }
    
    // dump the captured data to a temp file for future reference
    WCHAR szFilename[MAX_PATH] = {0};
    if (!VERIFY_WIN32_BOOL_SUCCEEDED(
        GetTempFileName(
            L".",
            L"HAP",
            0,
            szFilename
    ))) { return; }
    
    CComPtr<IWaveFileSink> pWaveFileSink;
    if (!VERIFY_SUCCEEDED(
        CreateWaveFileSink(
            pLogger,
            XFAIL,
            szFilename,
            pWfxLoopback,
            &pWaveFileSink
    ))) { return; }

    if (!VERIFY_SUCCEEDED(
        pLoopback->CaptureToSinkAsync(pWaveFileSink)
    )) { return; }
    
    // we'll stop it when the time comes
    CStopOnExit<IAudioStreamingCapture> stopCapture(pLoopback);

    LOG(L"T = 0; capturing loopback data to %s", szFilename);
    Sleep(1000);

    //
    // T = 1.0
    //
    // set the "disable" or "mute" output protection policy on the "extract" output trust authorities
    CComPtr<IMFOutputPolicy> pOutputPolicy;
    if (!VERIFY_SUCCEEDED(
        CreateOutputPolicy(
            pLogger,
            XFAIL,
            guidProtectionScheme,
            dwConfigData,
            &pOutputPolicy
        )
    )) {
        return;
    }
                
    IMFOutputPolicy *rExtractPolicies[] = { pOutputPolicy };
    
    for (int stream = SHARED; stream <= OFFLOAD; stream++) {
        CComHeapPtr<BYTE> pbTicket;
        DWORD cbTicket = 0;

        if (!VERIFY_SUCCEEDED(
            pMFOutputTrustAuthorities[stream]->SetPolicy(
                rExtractPolicies,
                ARRAYSIZE(rExtractPolicies),
                &pbTicket,
                &cbTicket
            )
        )) {
            return;
        }
    }
    
    LOG(L"T = 1: Output protection policies are set.");
    Sleep(1000);
    
    //
    // T = 2.0
    //
    // now clear the policies
    for (int stream = SHARED; stream <= OFFLOAD; stream++) {
        CComHeapPtr<BYTE> pbTicket;
        DWORD cbTicket = 0;

        if (!VERIFY_SUCCEEDED(
            pMFOutputTrustAuthorities[stream]->SetPolicy(
                NULL,
                0,
                &pbTicket,
                &cbTicket
            )
        )) {
            return;
        }
    }

    pOutputPolicy.Release();
    
    LOG(L"T = 2: Output protection policies are released.");
    Sleep(1000);

    //
    // T = 3.0
    //
    // stop the loopback capture
    stopCapture.Update(nullptr);
    if (!VERIFY_SUCCEEDED(pLoopback->Stop())) { return; }
    pLoopback.Release();
    if (!VERIFY_SUCCEEDED(pWaveFileSink->Finalize())) { return; }
    pWaveFileSink.Release(); // close the file
    
    LOG(L"T = 3: Loopback capture is complete.");

    // now analyze the loopback stream that we captured
    CComPtr<IWaveFileSource> pWaveFileSource;
    if (!VERIFY_SUCCEEDED(
        CreateWaveFileSource(
            pLogger,
            XFAIL,
            szFilename,
            0,
            &pWaveFileSource
        )
    )) { return; }
    
    UINT32 nFrames = 0;
    UINT32 nBytes = 0;
    
    if (!VERIFY_SUCCEEDED(
        pWaveFileSource->GetFrameCount(&nFrames, &nBytes)
    )) { return; }
    
    double seconds = SecondsFromFrames(nFrames, pWfxLoopback);
    LOG(L"Frames: %u; bytes: %u; seconds: %lg", nFrames, nBytes, seconds);
    
    // we should have gotten about three seconds' worth of data
    if (!VERIFY_ASCENDING_INCLUSIVE(2.8, seconds, 3.5)) { return; }
    
    CComHeapPtr<BYTE> pLoopbackData;
    if (!VERIFY_IS_TRUE(pLoopbackData.Allocate(nBytes))) { return; }
    
    if (!VERIFY_SUCCEEDED(
        pWaveFileSource->FillBuffer(pLoopbackData, nFrames, nBytes)
    )) { return; }
    pWaveFileSource.Release(); // close the file here; we may want to delete it
    
    bool pass = true;

    struct Expectation {
        double timeStart;
        double timeEnd;
        bool   signalExpected;
    };
    
    Expectation expected[] = {
        { 0.3, 0.7, true  },
        { 1.3, 1.7, false },
        { 2.3, 2.7, true  },
    };

    for (int window = 0; window < ARRAYSIZE(expected); window++) {
        CComPtr<ISoundAnalyze> pSoundAnalyze;
        if (!VERIFY_SUCCEEDED(CreateSoundAnalyze(&pSoundAnalyze))) { return; }
        
        UINT32 nStartingFrame = FramesFromSeconds(expected[window].timeStart, pWfxLoopback);
        UINT32 nFrameCount = FramesFromSeconds(expected[window].timeEnd - expected[window].timeStart, pWfxLoopback);
        
        if (!VERIFY_SUCCEEDED(
            pSoundAnalyze->InitializeSoundAnalyzer(
                pWfxLoopback,
                &pLoopbackData[nStartingFrame * pWfxLoopback->nBlockAlign],
                nFrameCount,
                BLACKMAN_HARRIS,
                false
        ))) { return; }
        
        double magAudible;
        double freqLow = 20.0;
        double freqHigh = min(20000.0, pWfxLoopback->nSamplesPerSec / 2.0);
            
        for (int channel = 1; channel <= pWfxLoopback->nChannels; channel++) {
            if (!VERIFY_SUCCEEDED(
                pSoundAnalyze->GetPowerBetween(
                    channel,
                    freqLow,
                    freqHigh,
                    magAudible
            ))) { return; }
            LOG(
                L"Time %lg to %lg; channel %d; power between %g Hz and %g Hz: %g (%g dB FS)",
                expected[window].timeStart,
                expected[window].timeEnd,
                channel,
                freqLow, freqHigh,
                magAudible, DbFromMagnitude(magAudible)
            );
        }
        
        for (int stream = 0; stream < ARRAYSIZE(pAudioStreamingRenders); stream++) {
            double magSignal;
            freqLow = freq[stream] - FREQUENCY_HALF_WIDTH;
            freqHigh = freq[stream] + FREQUENCY_HALF_WIDTH;

            for (int channel = 1; channel <= pWfxLoopback->nChannels; channel++) {
                if (!VERIFY_SUCCEEDED(
                    pSoundAnalyze->GetPowerBetween(
                        channel,
                        freqLow,
                        freqHigh,
                        magSignal
                ))) { return; }
                LOG(
                    L"Time %lg to %lg; channel %d; power between %g Hz and %g Hz: %g (%g dB FS)",
                    expected[window].timeStart,
                    expected[window].timeEnd,
                    channel,
                    freqLow, freqHigh,
                    magSignal, DbFromMagnitude(magSignal)
                );

                if (expected[window].signalExpected) {
                    // we allow for processing to attenuate the signal
                    // or amplify it
                    // but either way the signal should be discernible (> -50.0 dB FS, say)
                    if (!VERIFY_IS_GREATER_THAN_OR_EQUAL(DbFromMagnitude(magSignal), -50.0)) { pass = false; }
                } else {
                    // signal should be totally silent
                    if (!VERIFY_IS_LESS_THAN_OR_EQUAL(DbFromMagnitude(magSignal), -90.0)) { pass = false; }
                }
            }
        }
    }
    
    // if we passed, clean up the file
    if (pass) {
        VERIFY_WIN32_BOOL_SUCCEEDED(DeleteFile(szFilename));
    }
}
