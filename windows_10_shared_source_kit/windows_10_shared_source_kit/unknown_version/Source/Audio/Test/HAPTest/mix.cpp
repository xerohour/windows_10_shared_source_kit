// Copyright(C) Microsoft.All rights reserved.

// mix.cpp

#include <atlbase.h>
#include <atlstr.h>
#include <audioclient.h>
#include <mmeapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <devicetopology.h>
#include <strsafe.h>
#include <math.h>

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

using namespace WEX::Logging;
using namespace WEX::TestExecution;

#define OFFLOAD_STREAMS 2ul
#define FREQUENCY_HALF_WIDTH 50
#define SECONDS_TO_CAPTURE 1.0

void CHapTest::RunMixTest(BOOL bDifferentFormats /* = FALSE */) {
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

    CComPtr<IOffloadPath> pOffloadPath;
    if (!SUCCEEDED(GetOffloadPath(pLogger, pMMDevice, &pOffloadPath))) { return; } // GetOffloadPath does its own VERIFY

    // get info from the offload path
    UINT idOffloadPinFactory;
    if (!VERIFY_SUCCEEDED(pOffloadPath->GetOffloadPinFactoryId(&idOffloadPinFactory))) { return; }

    ULONG nOffloadPossible;
    ULONG nOffloadCurrent;
    if (!VERIFY_SUCCEEDED(
        pOffloadPath->GetPinInstanceCounts(
            idOffloadPinFactory,
            &nOffloadPossible,
            &nOffloadCurrent
    ))) { return; }
    
    // get info from the software path
    UINT idHostPinFactory;
    if (!VERIFY_SUCCEEDED(pOffloadPath->GetHostPinFactoryId(&idHostPinFactory))) { return; }

    ULONG nHostPossible;
    ULONG nHostCurrent;
    if (!VERIFY_SUCCEEDED(
        pOffloadPath->GetPinInstanceCounts(
            idHostPinFactory,
            &nHostPossible,
            &nHostCurrent
    ))) { return; }

    // get info from the loopback path
    UINT idLoopbackPinFactory;
    if (!VERIFY_SUCCEEDED(pOffloadPath->GetLoopbackPinFactoryId(&idLoopbackPinFactory))) { return; }

    ULONG nLoopbackPossible;
    ULONG nLoopbackCurrent;
    if (!VERIFY_SUCCEEDED(
        pOffloadPath->GetPinInstanceCounts(
            idLoopbackPinFactory,
            &nLoopbackPossible,
            &nLoopbackCurrent
    ))) { return; }

    UINT idAudioEngineNode;
    if (!VERIFY_SUCCEEDED(pOffloadPath->GetKsNodeId(&idAudioEngineNode))) { return; }
    
    LOG(
        L"Offload pin factory: id %u, %u of %u possible instances\n"
        L"Host pin factory: id %u, %u of %u possible instances\n"
        L"Loopback pin factory: id %u, %u of %u possible instances\n"
        L"Audio engine node: id %u",
        idOffloadPinFactory, nOffloadCurrent, nOffloadPossible,
        idHostPinFactory, nHostCurrent, nHostPossible,
        idLoopbackPinFactory, nLoopbackCurrent, nLoopbackPossible,
        idAudioEngineNode
    );

    // offload pin needs to support at least two instances
    if (!VERIFY_IS_GREATER_THAN_OR_EQUAL(nOffloadPossible, 2u)) { return; }
    
    // system should be quiescent at the start of the test
    if (!VERIFY_ARE_EQUAL(0u, nOffloadCurrent)) { return; }
    if (!VERIFY_ARE_EQUAL(0u, nHostCurrent)) { return; }
    if (!VERIFY_ARE_EQUAL(0u, nLoopbackCurrent)) { return; }
    
    CNormalizeVolumeAndRestoreOnExit normalizeVolume(pMMDevice);

    // we want to instantiate four streams
    // two in offload mode
    // one in shared mode
    // one loopback capture stream
    //
    // all should run concurrently
    //
    // we should end up with a mixed stream containing all three signals in equal proportion
    //
    // these sounds go directly to speakers so we need them to be reasonably quiet
    //
    // we need to be careful when choosing our test tones
    //
    // 1. the sine wave period should not a multiple of the buffer length
    //    which is usually 10 ms
    // 2. the frequency should fall within the peak audible range of about 1 kHz to about 6 kHz
    //    to be resistant to DSP
    // 3. they frequency should fall well below the Nyquist frequency for even low-bitrate device formats
    //    such as Bluetooth Hands-Free for which the sample rate can be as low as 8 kHz
    //    implying that the Nyquist frequency (sample rate / 2) can be as low as 4 kHz
    // 4. they should not be harmonics of each other, or be close to being harmonics of each other
    const float  rFreqsOffload[OFFLOAD_STREAMS] = { 1382.832342f, 1873.38441f };
    static_assert(OFFLOAD_STREAMS == 2, "update array above to match number of offload streams");
    const float  freqShared                     = 2917.293263f;

    // Default and Raw streams can both go through some processing
    // Default streams might go through (much) more processing
    // but we expect the signal to still be recognizable at the other end
    //
    // An example of processing that might affect both streams is "speaker compensation"
    // This will boost some frequencies and attenuate others
    // So we allow +/- 10 dB even on raw streams
    //
    // An example of processing that might be applied on default streams only is "loudness equalization",
    // also known as dynamic range compression
    // which is to say, it reduces the effect of "dynamics" (soft vs. loud)
    //
    // Since the audio signal we're playing is quiet, we allow a much higher tolerance
    // on the default stream: +40 dB
    const double    dbExpectedPower             = -40.0;
    const float     amp                         = static_cast<float>(SineAmpFromDb(dbExpectedPower));
    const Tolerance tolerance                   (-10.0, +40.0);

    CComHeapPtr<BYTE>         pCaptureBuffer;
    CComHeapPtr<WAVEFORMATEX> pWfxLoopback;   
    DWORD nFrames;
    DWORD nBytes;

    // naked scope to force "stop" objects
    {
        AudioClientPropertiesWin8           clientProperties = {0};
        CComPtr<IAudioStreamingRender>      rpAudioStreamingOffload[OFFLOAD_STREAMS];
        CStopOnExit<IAudioStreamingRender>  rSoeOffload[OFFLOAD_STREAMS];
        
        CComPtr<IAudioStreamingRender>      pAudioStreamingShared;
        CStopOnExit<IAudioStreamingRender>  soeShared;
        CComPtr<IAudioStreamingCapture>     pAudioStreamingLoopback;
        CStopOnExit<IAudioStreamingCapture> soeLoopback;
        CComHeapPtr<WAVEFORMATEX>           pwfxSavedFormat;

        clientProperties.bIsOffload = TRUE;
        clientProperties.eCategory  = AudioCategory_Media;
        clientProperties.cbSize     = sizeof(clientProperties);
        
        // instantiate the offload streams
        for (int i = 0; i < OFFLOAD_STREAMS; i++) {
            if (!VERIFY_SUCCEEDED(
                CreateAudioStreamingRender(
                    api_WASAPI,
                    pMMDevice,
                    pLogger,
                    &rpAudioStreamingOffload[i]
            ))) { return; }
            
            // force offload mode
            CComPtr<IAudioStreamingControl> pAudioStreamingControl;
            if (!VERIFY_SUCCEEDED(
                rpAudioStreamingOffload[i]->QueryInterface(
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

            // initialize the stream
            if (!VERIFY_SUCCEEDED(
                rpAudioStreamingOffload[i]->Initialize(
                    (bDifferentFormats && i > 0) ?
                        static_cast<WAVEFORMATEX*>(pwfxSavedFormat) :
                        nullptr
                )
            )) { return; }

            // if we're intentionally using different formats, and this is the first stream,
            // save the format and tweak it for future streams
            if (bDifferentFormats && 0 == i) {
                if (!VERIFY_SUCCEEDED(
                    rpAudioStreamingOffload[i]->GetWaveFormat(
                        &pwfxSavedFormat
                ))) { return; }

                DWORD nOriginalSampleRate = pwfxSavedFormat->nSamplesPerSec;
                
                pwfxSavedFormat->nSamplesPerSec = (48000 == nOriginalSampleRate ? 44100 : 48000);
                pwfxSavedFormat->nAvgBytesPerSec = pwfxSavedFormat->nBlockAlign * pwfxSavedFormat->nSamplesPerSec;
                
                LOG(L"First offload stream sample rate was %g kHz; further offload streams will use %g kHz", nOriginalSampleRate / 1000.0, pwfxSavedFormat->nSamplesPerSec / 1000.0);
            }

            if (!VERIFY_SUCCEEDED(
                rpAudioStreamingOffload[i]->PlaySineWaveAsync(
                    rFreqsOffload[i],
                    amp,
                    Method_TPD
            ))) { return; }
            rSoeOffload[i].Update(rpAudioStreamingOffload[i]);
            
            LOG(L"Offload stream #%u: %g Hz sine wave at amplitude %g (expected power %g dB FS)", i + 1, rFreqsOffload[i], amp, DbFromSineAmp(amp));
        }
        
        // instantiate the shared mode stream
        if (!VERIFY_SUCCEEDED(
            CreateAudioStreamingRender(
                api_WASAPI,
                pMMDevice,
                pLogger,
                &pAudioStreamingShared
        ))) { return; }

        // let the object decide the format
        if (!VERIFY_SUCCEEDED(pAudioStreamingShared->Initialize(NULL))) { return; }

        if (!VERIFY_SUCCEEDED(
            pAudioStreamingShared->PlaySineWaveAsync(
                freqShared,
                amp,
                Method_TPD
        ))) { return; }
        soeShared.Update(pAudioStreamingShared);
        LOG(L"Shared stream: %g Hz sine wave at amplitude %g (expected power %g dB FS)", freqShared, amp, DbFromSineAmp(amp));
        
        // everything's started
        // sleep for a second to let everything flush
        Sleep(1000);
        
        // now start the loopback capture stream
        if (!VERIFY_SUCCEEDED(
            CreateAudioStreamingCapture(
                api_WASAPI,
                pMMDevice,
                pLogger,
                &pAudioStreamingLoopback
        ))) { return; }
        
        // set the loopback capture object to loopback mode
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

        if (!VERIFY_SUCCEEDED(pAudioStreamingLoopback->Initialize(NULL))) { return; }
        if (!VERIFY_SUCCEEDED(pAudioStreamingLoopback->GetWaveFormat(&pWfxLoopback))) { return; }
        
        // allocate a buffer that can hold a second's worth of data
        nFrames = (DWORD)(SECONDS_TO_CAPTURE * pWfxLoopback->nSamplesPerSec + 0.5);
        nBytes = nFrames * pWfxLoopback->nBlockAlign;
        if (!VERIFY_IS_TRUE(pCaptureBuffer.Allocate(nBytes))) { return; }
        
        if (!VERIFY_SUCCEEDED(
            pAudioStreamingLoopback->CaptureBuffer(
                pCaptureBuffer,
                nBytes
        ))) { return; }
        soeLoopback.Update(pAudioStreamingLoopback);

        // verify all the streams are where we expect them
        if (!VERIFY_SUCCEEDED(
            pOffloadPath->GetPinInstanceCounts(
                idOffloadPinFactory,
                &nOffloadPossible,
                &nOffloadCurrent
        ))) { return; }
        
        if (!VERIFY_SUCCEEDED(
            pOffloadPath->GetPinInstanceCounts(
                idHostPinFactory,
                &nHostPossible,
                &nHostCurrent
        ))) { return; }

        if (!VERIFY_SUCCEEDED(
            pOffloadPath->GetPinInstanceCounts(
                idLoopbackPinFactory,
                &nLoopbackPossible,
                &nLoopbackCurrent
        ))) { return; }

        if (!VERIFY_ARE_EQUAL(OFFLOAD_STREAMS, nOffloadCurrent)) { return; }
        if (!VERIFY_ARE_EQUAL(1u, nHostCurrent)) { return; }
        if (!VERIFY_ARE_EQUAL(1u, nLoopbackCurrent)) { return; }

    } // end naked scope; all playback and capture should stop
    
    // dump the captured data to a temp file for future reference
    WCHAR szFilename[MAX_PATH] = {0};
    if (!VERIFY_WIN32_BOOL_SUCCEEDED(
        GetTempFileName(
            L".",
            L"HAP",
            0,
            szFilename
    ))) { return; }
    
    LOG(L"Dumping data to %s", szFilename);
    
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
        pWaveFileSink->ConsumeBuffer(
            pCaptureBuffer,
            nFrames,
            nBytes
    ))) { return; }
    
    if (!VERIFY_SUCCEEDED(pWaveFileSink->Finalize())) { return; }
    
    // OK let's analyze the wave data now
    CComPtr<ISoundAnalyze> pSoundAnalyze;
    if (!VERIFY_SUCCEEDED(CreateSoundAnalyze(&pSoundAnalyze))) { return; }
    
    if (!VERIFY_SUCCEEDED(
        pSoundAnalyze->InitializeSoundAnalyzer(
            pWfxLoopback,
            pCaptureBuffer,
            nFrames,
            BLACKMAN_HARRIS,
            false
    ))) { return; }
    
    double magAudible;
    double freqLow = 20.0;
    double freqHigh = min(20000.0, pWfxLoopback->nSamplesPerSec / 2.0);
    if (!VERIFY_SUCCEEDED(
        pSoundAnalyze->GetPowerBetween(
            1, // just the left channel
            freqLow,
            freqHigh,
            magAudible
    ))) { return; }
    LOG(L"Power between %g Hz and %g Hz: %g (%g dB FS)", freqLow, freqHigh, magAudible, DbFromMagnitude(magAudible));

    double magShared;
    freqLow = freqShared - FREQUENCY_HALF_WIDTH;
    freqHigh = freqShared + FREQUENCY_HALF_WIDTH;
    if (!VERIFY_SUCCEEDED(
        pSoundAnalyze->GetPowerBetween(
            1, // just the left channel
            freqLow,
            freqHigh,
            magShared
    ))) { return; }
    LOG(L"Power between %g Hz and %g Hz (shared mode stream:) %g (%g dB FS)", freqLow, freqHigh, magShared, DbFromMagnitude(magShared));

    VERIFY_ASCENDING_INCLUSIVE(
        dbExpectedPower + tolerance.Below,
        DbFromMagnitude(magShared),
        dbExpectedPower + tolerance.Above
    );
    
    double rMagOffload[OFFLOAD_STREAMS];
    for (int i = 0; i < ARRAYSIZE(rMagOffload); i++) {
        freqLow = rFreqsOffload[i] - FREQUENCY_HALF_WIDTH;
        freqHigh = rFreqsOffload[i] + FREQUENCY_HALF_WIDTH;
        if (!VERIFY_SUCCEEDED(
            pSoundAnalyze->GetPowerBetween(
                1, // just the left channel
                freqLow,
                freqHigh,
                rMagOffload[i]
        ))) { return; }
        LOG(L"Power between %g Hz and %g Hz (offload stream #%u:) %g (%g dB FS)", freqLow, freqHigh, i + 1, rMagOffload[i], DbFromMagnitude(rMagOffload[i]));

        VERIFY_ASCENDING_INCLUSIVE(
            dbExpectedPower + tolerance.Below,
            DbFromMagnitude(rMagOffload[i]),
            dbExpectedPower + tolerance.Above
        );
    }
    
    double powSquaredSignal = magShared * magShared;
    for (int i = 0; i < ARRAYSIZE(rMagOffload); i++) {
        powSquaredSignal += rMagOffload[i] * rMagOffload[i];
    }
    
    double magSignal = sqrt(powSquaredSignal);
    double magNoise = sqrt(magAudible * magAudible - powSquaredSignal);

    LOG(
        L"Signal: %g (%g dB FS); Noise: %g (%g dB FS); Signal to noise ratio: %g (%g dB)",
        magSignal, DbFromMagnitude(magSignal),
        magNoise, DbFromMagnitude(magNoise),
        magSignal / magNoise, DbFromMagnitude(magSignal / magNoise)
    );
    
    // enforce a signal-to-noise ratio of at least 25 dB for communications formats
    // (e.g., Hands-Free devices)
    // at least 40 dB for music formats
    VERIFY_IS_GREATER_THAN(DbFromMagnitude(magSignal / magNoise), (pWfxLoopback->nSamplesPerSec >= 44100 ? 40.0 : 25.0));
}

void CHapTest::Mix(){
    return RunMixTest();
}

void CHapTest::MixOffloadDifferentFormat(){
    return RunMixTest(TRUE);
}
