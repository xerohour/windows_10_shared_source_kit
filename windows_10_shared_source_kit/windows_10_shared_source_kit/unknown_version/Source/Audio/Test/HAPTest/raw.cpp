// Copyright(C) Microsoft.All rights reserved.

// raw.cpp
// we want to instantiate two streams
// one raw stream with a sinusoid with frequency 1000hz
// one default stream with a sinusoid with frequency 1500hz
// we should end up with a mix stream containing both signals in equal proportion

// Options:
// To save the data use /p:SaveData=true

#include <windows.h>
#include <versionhelpers.h>
#include <atlstr.h>
#include <mmdeviceapip.h>
#include <endpointvolume.h>
#include <propkey.h>
#include <math.h>

#include <strsafe.h>
#include <basiclog.h>
#include <BasicLogHelper.h>
#include <basicprintf.h>

#include <audiostreaming.h>
#include <soundanalyze.h>
#include <waveio.h>
#include <WexTestClass.h>
#include <initguid.h>

#include <debug.h>
#include <doonexit.h>

#include "haptest.h"
#include "log.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;

#define SECONDS_TO_CAPTURE 1.0

void CHapTest::Raw() {

    if (!IsWindows8Point1OrGreater()) {
        Log::Result(TestResults::Result::Skipped, L"Test applies only to Windows 8.1 and up");
        return;
    }

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
    
    // see if RAW processing is supported
    CComPtr<IPropertyStore> pPropertyStore;
    if (!VERIFY_SUCCEEDED(pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore))) { return; }

    PROPVARIANT var; PropVariantInit(&var);
    DoOnExit<PROPVARIANT *> clearVar(PropVariantClear, Success_NonNegative, &var, NULL, L"Clearing raw property");

    if (!VERIFY_SUCCEEDED(
        pPropertyStore->GetValue(PKEY_Devices_AudioDevice_RawProcessingSupported, &var)
    )) { return; }
    
    switch (var.vt) {
    
        case VT_EMPTY:
            ERR(L"PKEY_Devices_AudioDevice_RawProcessingSupported missing");
            return;

        case VT_BOOL:
            if (!var.boolVal) {
                Log::Result(TestResults::Result::Skipped, L"Endpoint does not support raw processing.");
                return;
            }
            
            // RAW processing is supported; carry on.
            break;

        default:
            ERR("Unexpected vartype %u for PKEY_Devices_AudioDevice_RawProcessingSupported", var.vt); 
            return;
    }
    
    CNormalizeVolumeAndRestoreOnExit normalizeVolume(pMMDevice);

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
    const float rawFreq = 1048.28398f;    
    const float defaultFreq = 2573.49827f;

    // Default and Raw streams can both go through some processing
    // but we expect the signal to still be recognizable at the other end
    //
    // An example of processing that might affect both streams is "speaker compensation"
    // This will boost some frequencies and attenuate others
    // Based on device hardware, required boost may be large even for raw streams.
    //
    // An example of processing that might be applied on default streams only is "loudness equalization",
    // also known as dynamic range compression
    // which is to say, it reduces the effect of "dynamics" (soft vs. loud)
    //
    // Since the audio signal we're playing is quiet, we allow a much higher tolerance
    const double dbExpectedPower = -40;
    const float amp = static_cast<float>(SineAmpFromDb(dbExpectedPower));

    const Tolerance rawTolerance(-10.0, +40.0);
    const Tolerance defaultTolerance(-10.0, +40.0);

    CComHeapPtr<BYTE> pCaptureBuffer;
    CComHeapPtr<WAVEFORMATEX> pWfxLoopback;   
    DWORD nFrames;
    DWORD nBytes;

    // naked scope to force "stop" objects
    {
        CComPtr<IAudioStreamingRender> pRawAudioStreamingRender;
        if (!VERIFY_SUCCEEDED(
            CreateAudioStreamingRender(
            api_WASAPI,
            pMMDevice,
            pLogger,
            &pRawAudioStreamingRender
        ))) { return; }

        // create the raw stream
        AudioClientPropertiesWin81 rawClientProperties = {0};
        rawClientProperties.cbSize = sizeof(rawClientProperties);
        // Use Other here, instead of Communications (on phone, 
        // Communications will mute all other audio)
        rawClientProperties.eCategory = AudioCategory_Other;
        rawClientProperties.bIsOffload = false;
        rawClientProperties.Options = AUDCLNT_STREAMOPTIONS_RAW;

        CComPtr<IAudioStreamingControl> pRawAudioStreamingControl;
        if (!VERIFY_SUCCEEDED(
        pRawAudioStreamingRender->QueryInterface(
            __uuidof(IAudioStreamingControl),
            reinterpret_cast<void**>(&pRawAudioStreamingControl)
        ))) { return; }
        
        CComPtr<IAudioStreamingWasapiControl> pRawAudioStreamingWasapiControl;
        if (!VERIFY_SUCCEEDED(
        pRawAudioStreamingControl->GetService(
            __uuidof(IAudioStreamingWasapiControl),
            reinterpret_cast<void**>(&pRawAudioStreamingWasapiControl)
        ))) { return; }
        
#pragma prefast(suppress: __WARNING_BUFFER_OVERFLOW, "client properties is variable-sized")
        if (!VERIFY_SUCCEEDED(
            pRawAudioStreamingWasapiControl->SetClientProperties(
                reinterpret_cast<AudioClientProperties*>(&rawClientProperties)
            )
        )) { return; }

        if (!VERIFY_SUCCEEDED(pRawAudioStreamingRender->Initialize(NULL))) { return; }
    
        CComPtr<IAudioStreamingRender> pDefaultAudioStreamingRender;
        if (!VERIFY_SUCCEEDED(
        CreateAudioStreamingRender(
        api_WASAPI,
        pMMDevice,
        pLogger,
        &pDefaultAudioStreamingRender
        ))) { return; }

        if (!VERIFY_SUCCEEDED(pDefaultAudioStreamingRender->Initialize(NULL))) { return; }
    
        // start playing both streams
        if (!VERIFY_SUCCEEDED(
            pRawAudioStreamingRender->PlaySineWaveAsync(
            rawFreq,
            amp,
            Method_TPD
        ))) { return; }
        CStopOnExit<IAudioStreamingRender> pSoeRawAudioStreamingRender(pRawAudioStreamingRender);

        if (!VERIFY_SUCCEEDED(
            pDefaultAudioStreamingRender->PlaySineWaveAsync(
            defaultFreq,
            amp,
            Method_TPD
        ))) { return; }
        CStopOnExit<IAudioStreamingRender> pSoeDefaultAudioStreamingRender(pDefaultAudioStreamingRender);

        // now start the loopback capture stream
        CComPtr<IAudioStreamingCapture> pAudioStreamingLoopback;    
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
    }

    // OK let's analyze the wave data now
    CComPtr<ISoundAnalyze> pSoundAnalyze;
    if (!VERIFY_SUCCEEDED(CreateSoundAnalyze(&pSoundAnalyze))) { return; }
    
    if (!VERIFY_SUCCEEDED(
        pSoundAnalyze->InitializeSoundAnalyzer(
            pWfxLoopback,
            pCaptureBuffer,
            nFrames,
            BLACKMAN,
        true  
    ))) { return; }
    
    double magRaw;
    double freqRawLow = rawFreq - 50;
    double freqRawHigh = rawFreq + 50;
    if (!VERIFY_SUCCEEDED(
        pSoundAnalyze->GetPowerBetween(
            1, // just the left channel
            freqRawLow,
            freqRawHigh,
            magRaw
    ))) { return; }
    bool saveDataToFile = false;
    LOG(L"Power between %g Hz and %g Hz: %g (%g dB FS)", freqRawLow, freqRawHigh, magRaw, DbFromMagnitude(magRaw));
    
    if (!VERIFY_ASCENDING_INCLUSIVE(
        dbExpectedPower + rawTolerance.Below,
        DbFromMagnitude(magRaw),
        dbExpectedPower + rawTolerance.Above
    ))
    { saveDataToFile = true; }

    double magDefault;
    double freqDefaultLow = defaultFreq - 50;
    double freqDefaultHigh = defaultFreq + 50;
    if (!VERIFY_SUCCEEDED(
        pSoundAnalyze->GetPowerBetween(
            1, // just the left channel
            freqDefaultLow,
            freqDefaultHigh,
            magDefault
    ))) { return; }

    LOG(L"Power between %g Hz and %g Hz: %g (%g dB FS)", freqDefaultLow, freqDefaultHigh, magDefault, DbFromMagnitude(magDefault));
    
    if (!VERIFY_ASCENDING_INCLUSIVE(
        dbExpectedPower + defaultTolerance.Below,
        DbFromMagnitude(magDefault),
        dbExpectedPower + defaultTolerance.Above
    ))
    { saveDataToFile = true; }

    double magAudible;
    double freqLow = 20;
    double freqHigh = min(20000.0, pWfxLoopback->nSamplesPerSec / 2.0);
    if (!VERIFY_SUCCEEDED(
        pSoundAnalyze->GetPowerBetween(
            1, // just the left channel
            freqLow,
            freqHigh,
            magAudible
    ))) { return; }
    LOG(L"Power between %g Hz and %g Hz: %g (%g dB FS)", freqLow, freqHigh, magAudible, DbFromMagnitude(magAudible));

    double powSquaredSignal = ((magDefault * magDefault) + (magRaw * magRaw));
    double magSignal = sqrt(powSquaredSignal);
    double magNoise = sqrt((magAudible * magAudible) - powSquaredSignal);

    LOG(
        L"Signal: %g (%g dB FS); Noise: %g (%g dB FS); Signal to noise ratio: %g (%g dB)",
        magSignal, DbFromMagnitude(magSignal),
        magNoise, DbFromMagnitude(magNoise),
        magSignal / magNoise, DbFromMagnitude(magSignal / magNoise)
    );

    if (!VERIFY_IS_GREATER_THAN(DbFromMagnitude(magSignal / magNoise), (pWfxLoopback->nSamplesPerSec >= 44100 ? 40.0 : 25.0)))
    { saveDataToFile = true; }

    if (!saveDataToFile) {
    RuntimeParameters::TryGetValue(L"SaveData", saveDataToFile);
    }

    // only save the data if we explicitly want or if we have an error
    if ( saveDataToFile ) {
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
    }
}
