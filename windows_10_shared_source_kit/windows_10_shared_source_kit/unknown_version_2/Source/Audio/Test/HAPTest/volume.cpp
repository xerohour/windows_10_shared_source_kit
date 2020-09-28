// Copyright(C) Microsoft.All rights reserved.

// volume.cpp

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
const Tolerance g_DBTHRESHOLD(-10.0, +40.0);

class CAudioStreamingAsyncCallback : public IAudioStreamingAsyncCallback {
public:

    // IUnknown
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_cRef); }
    ULONG STDMETHODCALLTYPE Release() override {
        LONG retval = InterlockedDecrement(&m_cRef);

        if (0 >= retval) 
        {
            delete this;
        }
        
        return retval;
    }

    STDMETHOD(QueryInterface)(REFIID riid, void** pp) override {
        if (NULL == pp) { return E_POINTER; }
        *pp = NULL;
        
        if (__uuidof(IUnknown) == riid || __uuidof(IAudioStreamingAsyncCallback) == riid) {
            *pp = static_cast<IAudioStreamingAsyncCallback*>(this);
            AddRef();
            return S_OK;
        }
        
        return E_NOINTERFACE;
    }

    // IAudioStreamingAsyncCallback
    STDMETHOD(Callback)(HRESULT hrRet) {
        VERIFY_SUCCEEDED(hrRet);
        *m_pHr = hrRet;
        return S_OK;
    }

   CAudioStreamingAsyncCallback(HRESULT *pHr) : m_cRef(1), m_pHr(pHr) {} 
    
private:
    ~CAudioStreamingAsyncCallback() {}
    
    LONG m_cRef;
    HRESULT *m_pHr;
};

void Volume(AUDIO_CURVE_TYPE curve);
void ValidateSignalLevel(
    IBasicLog *pLogger,
    const WAVEFORMATEX *pWfx,
    const BYTE *pBuffer,
    UINT32 nBytes,
    double secStart,
    double secEnd,
    double hzExpected,
    double dbMin,
    double dbMax
);

// for a ramp from dbStart to dbEnd that lasts for secDuration,
// find the expected value at time secT (0 <= secT <= secDuration)
double ExpectedRampValue(double dbStart, double dbEnd, double secDuration, double secT);

// CHapTest::VolumeImmediate
//
// This test case validates setting the Session volume
// while streaming
//
//                 1          2           3
//    0 +--------*-|-*------*-|-*---------|--------> time (seconds)
//      |
//      |
//  -20 +--------*-+          +-*-----------> (until end of stream)
//      |          |          |          
//      |          |          |          
//  -40 |          +-*------*-+
//      |
//      v
//
// signal strength (dB FS)
//
// we play a signal mastered at -20 dB FS for three seconds
// the measured signal strength should be -20 dB FS
//
// at one second in, we set the volume on the stream to -20 dB FS
// this should result in the measured signal strength dropping to -40 dB FS
//
// at two seconds in, we set the volume on the stream back to 0 dB FS
// this should result in the measured signal strength coming back up to -20 dB FS
//
// we validate four measurements as marked above
// from t = 0.8 to 0.9 (should be -20 dB FS)
// from t = 1.1 to 1.2 (should be -40 dB FS)
//
// from t = 1.8 to 1.9 (should be -40 dB FS)
// from t = 2.1 to 2.2 (should be -20 dB FS)
//
// our time intervals are fairly small; only 100 ms
// and up to 50 ms can be thrown away by the base-2 Fourier transform that is sndanlyz.dll
//
// so how high of a frequency should we use?
// a 20 Hz sine wave would complete one cycle in 50 ms
// a 200 Hz sine wave would complete ten cycles
// so we'll use that - or an irrational number reasonably close to that
void CHapTest::VolumeImmediate() {
    Volume(AUDIO_CURVE_TYPE_NONE);
}

void Volume(AUDIO_CURVE_TYPE curve) {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    const float dbSignal      = -20.0f;
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
    const float hzSignal      = 1274.94737f;
    const float dbAttenuation = -20.0f;
    const float secCapture    = 3.0f;

    if (!VERIFY_ARE_EQUAL(AUDIO_CURVE_TYPE_NONE, curve))
    {
        return;
    }
    
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

    // need the engine node so we can send the volume property change request
    UINT idAudioEngineNode;
    if (!VERIFY_SUCCEEDED(pOffloadPath->GetKsNodeId(&idAudioEngineNode))) { return; }
    
    LOG(L"Audio engine node: id %u", idAudioEngineNode);

    CNormalizeVolumeAndRestoreOnExit normalizeVolume(pMMDevice);
    
    CComHeapPtr<WAVEFORMATEX> pWfxLoopback;
    
    CComHeapPtr<BYTE> pCaptureBuffer;
    DWORD             nFrames;
    BUFFER_SIZE       nBytes;

    HRESULT hrCallback = E_UNEXPECTED;
    
    // naked scope to force "stop" objects
    {
        AudioClientPropertiesWin8           clientProperties = {0};
        CComPtr<IAudioStreamingRender>      pAudioStreaming;
        CStopOnExit<IAudioStreamingRender>  soe;

        CComPtr<IAudioStreamingCapture>     pAudioStreamingLoopback;
        CStopOnExit<IAudioStreamingCapture> soeLoopback;

        clientProperties.bIsOffload = TRUE;
        clientProperties.eCategory  = AudioCategory_Media;
        clientProperties.cbSize     = sizeof(clientProperties);

        // instantiate the offload stream
        if (!VERIFY_SUCCEEDED(
            CreateAudioStreamingRender(
                api_WASAPI,
                pMMDevice,
                pLogger,
                &pAudioStreaming
        ))) { return; }
        
        // force offload mode
        CComPtr<IAudioStreamingControl> pAudioStreamingControl;
        if (!VERIFY_SUCCEEDED(
            pAudioStreaming->QueryInterface(
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

        // let the object decide the format
        if (!VERIFY_SUCCEEDED(pAudioStreaming->Initialize(NULL))) { return; }

        // get a stream-handled IKsControl so we can set the stream volume
        // first we need to coax the IAudioClient out of IAudioStreamingRender
        CComPtr<IAudioClient> pAudioClient;
        if (!VERIFY_SUCCEEDED(
            pAudioStreamingControl->GetService(
                __uuidof(IAudioClient),
                reinterpret_cast<void**>(&pAudioClient)
        ))) { return; }
        
        // now we can get an ISimpleAudioVolume
        CComPtr<ISimpleAudioVolume> pSimpleAudioVolume;
        if (!VERIFY_SUCCEEDED(
            pAudioClient->GetService(
                __uuidof(ISimpleAudioVolume),
                reinterpret_cast<void**>(&pSimpleAudioVolume)
        ))) { return; }

        if (!VERIFY_SUCCEEDED(
            pAudioStreaming->PlaySineWaveAsync(
                hzSignal,
                (float)SineAmpFromDb(dbSignal),
                Method_TPD
        ))) { return; }
        soe.Update(pAudioStreaming); // stop streaming on exit
            
        LOG(L"Offload stream: %g Hz sine wave at amplitude %g (expected power %g dB FS)", hzSignal, SineAmpFromDb(dbSignal), dbSignal);
        
        // set up the loopback capture stream
        if (!VERIFY_SUCCEEDED(
            CreateAudioStreamingCapture(
                api_WASAPI,
                pMMDevice,
                pLogger,
                &pAudioStreamingLoopback
        ))) { return; }

        // set the loopback capture object to loopback mode
        pAudioStreamingControl.Release();
        if (!VERIFY_SUCCEEDED(
            pAudioStreamingLoopback->QueryInterface(
                __uuidof(IAudioStreamingControl),
                reinterpret_cast<void**>(&pAudioStreamingControl)
        ))) { return; }
        
        pAudioStreamingWasapiControl.Release();
        if (!VERIFY_SUCCEEDED(
            pAudioStreamingControl->GetService(
                __uuidof(IAudioStreamingWasapiControl),
                reinterpret_cast<void**>(&pAudioStreamingWasapiControl)
        ))) { return; }
        
        // Use a different session for the loopback stream; otherwise ISimpleAudioVolume::SetMasterVolume will also attenuate Loopback
        GUID loopbackSession = { 0 };
        if (!VERIFY_SUCCEEDED(CoCreateGuid(&loopbackSession))) { return; }

        if (!VERIFY_SUCCEEDED(
            pAudioStreamingWasapiControl->SetAudioClientInitializeParameters(
                AUDCLNT_STREAMFLAGS_LOOPBACK,
                0, // buffer duration
                0, // periodicity
                loopbackSession // session
        ))) { return; }

        if (!VERIFY_SUCCEEDED(pAudioStreamingLoopback->Initialize(NULL))) { return; }
        if (!VERIFY_SUCCEEDED(pAudioStreamingLoopback->GetWaveFormat(&pWfxLoopback))) { return; }
        
        // allocate a buffer that can hold three second's worth of data
        nFrames = (DWORD)(secCapture * pWfxLoopback->nSamplesPerSec + 0.5);
        nBytes = nFrames * pWfxLoopback->nBlockAlign;
        if (!VERIFY_IS_TRUE(pCaptureBuffer.Allocate(nBytes))) { return; }
        
        CComPtr<IAudioStreamingAsyncCallback> pAsyncCallback;
        pAsyncCallback.Attach(new CAudioStreamingAsyncCallback(&hrCallback));
        if (!VERIFY_IS_NOT_NULL(
            pAsyncCallback
        )) { return; }
        if (!VERIFY_SUCCEEDED(
            pAudioStreamingLoopback->CaptureBufferAsync(
                pCaptureBuffer,
                &nBytes,
                pAsyncCallback
        ))) { return; }
        soeLoopback.Update(pAudioStreamingLoopback); // stop on exit
        pAsyncCallback.Release(); // we don't need our reference anymore

        LogTime(pLogger, L"All streams started");
        Sleep(1000);
        
        // set the volume to -20 dB FS. Note that this will apply to all streams in the session, including the Loopback stream
        LogTime(pLogger, L"Lowering the volume...");
        if (FAILED(
            pSimpleAudioVolume->SetMasterVolume((float)MagnitudeFromDb(dbAttenuation), NULL)
        )) { return; } // SetVolume does its own VERIFY
        LogTime(pLogger, L"Lowering the volume is complete.");
            
        Sleep(1000);
            
        // set the volume back to 0 dB FS
        LogTime(pLogger, L"Setting the volume back to 0 dB FS...");
        if (FAILED(
            pSimpleAudioVolume->SetMasterVolume((float)MagnitudeFromDb(0.0), NULL)
        )) { return; } // SetVolume does its own VERIFY
        LogTime(pLogger, L"Setting the volume back to 0 dB FS complete.");
            
        Sleep(1000);

        LogTime(pLogger, L"Stopping streams");
    } // end naked scope; all playback and capture should stop
    
    LOG(L"Loopback capture callback HRESULT: 0x%08x", hrCallback);
    if (!VERIFY_SUCCEEDED(hrCallback)) { return; }
    
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

    // from t = 0.8 to 0.9 (should be -20 dB FS)
    ValidateSignalLevel(pLogger, pWfxLoopback, pCaptureBuffer, nBytes, 0.8, 0.9, hzSignal, dbSignal + g_DBTHRESHOLD.Below, dbSignal + g_DBTHRESHOLD.Above);
        
    // from t = 1.1 to 1.2 (should be -40 dB FS)
    ValidateSignalLevel(pLogger, pWfxLoopback, pCaptureBuffer, nBytes, 1.1, 1.2, hzSignal, dbSignal + dbAttenuation + g_DBTHRESHOLD.Below, dbSignal + dbAttenuation + g_DBTHRESHOLD.Above);
        
    // from t = 1.8 to 1.9 (should be -40 dB FS)
    ValidateSignalLevel(pLogger, pWfxLoopback, pCaptureBuffer, nBytes, 1.8, 1.9, hzSignal, dbSignal + dbAttenuation + g_DBTHRESHOLD.Below, dbSignal + dbAttenuation + g_DBTHRESHOLD.Above);

    // from t = 2.1 to 2.2 (should be -20 dB FS)
    ValidateSignalLevel(pLogger, pWfxLoopback, pCaptureBuffer, nBytes, 2.1, 2.2, hzSignal, dbSignal + g_DBTHRESHOLD.Below, dbSignal + g_DBTHRESHOLD.Above);
}

void ValidateSignalLevel(
    IBasicLog *pLogger,
    const WAVEFORMATEX *pWfx,
    const BYTE *pBuffer,
    UINT32 nBytes,
    double secStart,
    double secEnd,
    double hzExpected,
    double dbMin,
    double dbMax
) {
    CComPtr<ISoundAnalyze> pSoundAnalyze;
    if (!VERIFY_SUCCEEDED(CreateSoundAnalyze(&pSoundAnalyze))) { return; }

    UINT32 nByteStart = (UINT32)(secStart * pWfx->nAvgBytesPerSec + 0.5);
    nByteStart -= (nByteStart % pWfx->nBlockAlign); // start at the beginning of a frame
    UINT32 nByteEnd = (UINT32)(secEnd * pWfx->nAvgBytesPerSec + 0.5); // actually one past the end
    nByteEnd -= (nByteEnd % pWfx->nBlockAlign);
    
    if (!VERIFY_IS_LESS_THAN_OR_EQUAL(nByteEnd, nBytes)) {
        // don't overflow the buffer
        return;
    }
    
    LOG(L"Calculating signal level from byte %u to byte %u", nByteStart, nByteEnd);

    if (!VERIFY_SUCCEEDED(
        pSoundAnalyze->InitializeSoundAnalyzer(
            pWfx,
            pBuffer + nByteStart,
            (nByteEnd - nByteStart) / pWfx->nBlockAlign,
            BLACKMAN_HARRIS,
            false
    ))) { return; }
    
    for (UINT32 channel = 1; channel <= pWfx->nChannels; channel++) {
        double magMeasured;
        if (!VERIFY_SUCCEEDED(
            pSoundAnalyze->GetPowerBetween(
                channel,
                hzExpected - 50.0,
                hzExpected + 50.0,
                magMeasured
        ))) { return; }
        
        double dbMeasured = DbFromMagnitude(magMeasured);
        LOG(
            L"Measured signal on channel %u from time %g to %g (byte %u to byte %u) is %lg; expected between %g and %g",
            channel,
            secStart, secEnd,
            nByteStart, nByteEnd,
            dbMeasured, dbMin, dbMax
        );
        
        VERIFY_ASCENDING_INCLUSIVE(dbMin, dbMeasured, dbMax);
    }
}

double ExpectedRampValue(double dbStart, double dbEnd, double secDuration, double secT) {
    // formula from "Exposing Hardware-Offloaded Audio Processing in Windows" whitepaper
    // http://msdn.microsoft.com/en-us/library/windows/hardware/br259116.aspx
    
    double ampStart = MagnitudeFromDb(dbStart);
    double ampEnd = MagnitudeFromDb(dbEnd);
    
    // first, are we ramping up or down?
    double amp;
    if (dbStart > dbEnd) {
        // we're ramping down
        amp = (ampStart - ampEnd) * pow(1.0 - (secT / secDuration), 1.75) + ampEnd;
    } else {
        // we're ramping up
        amp = ampStart + (ampEnd - ampStart) * pow(secT / secDuration, 1.75);
    }
    
    return DbFromMagnitude(amp);
}
