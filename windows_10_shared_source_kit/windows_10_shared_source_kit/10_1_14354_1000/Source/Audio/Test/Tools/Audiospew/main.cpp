// Windows
#include <objbase.h>
#include <initguid.h>
#include <ntassert.h>
#define ASSERT(x) NT_ASSERT(x)
#if DBG
#define VERIFY(x) ASSERT(x)
#else
#define VERIFY(x) (x)
#endif

#include <atlbase.h>
#include <atlstr.h>
#include <atlsync.h>
#include <strsafe.h>
#include <propidl.h>

#include <audioclient.h>

#if (NTDDI_VERSION >= NTDDI_WIN8)
#define USE_XAUDIO2 1
#else
#define USE_XAUDIO2 0
#endif

#if USE_XAUDIO2
#include <xaudio2.h>
#else
#endif

// CRT
#include <stdio.h>
#include <math.h>

// SDKTOOLS
#include <basiclog.h>

// AVCORE
#include <mmdeviceapi.h>
#include <mmsystem.h>
#include <DevPKey.h>
#include <DSound.h>
#include <functiondiscoverykeys_devpkey.h>
#include <signal-source.h>
#include <dithering.h>
#include <sine-signal-source.h>
#include <AudioClient.h>





// Renderlib
#include <audiostreaming.h>

// MMtest
#include <basicprintf.h>
#include <coinitialize.h>
#include <BasicLogHelper.h>
#include <formattedstr.h>
#include <helperclasses/wfx.h>
#include <helperclasses/cptr.h>
#include <waveio.h>
#include <doonexit.h>
#include <kslib.h>

#include "endpoint-id.h"
#include "log.h"
#include "parse.h"

#define MILLISECONDS_PER_SECOND 1000
#define HNS_PER_MILLISECOND 10000ll

IBasicLog *g_pLog = NULL;
BOOL g_fWorkaroundWindowsBlueBugs518047 = FALSE;

struct Category {
    LPCWSTR name;
    AUDIO_STREAM_CATEGORY value;
};
#define CATEGORY(x) { L ## #x, AudioCategory_ ## x }
Category g_AudioCategories[] = {
    CATEGORY(Other),
    CATEGORY(Movie),
    CATEGORY(Media),
    CATEGORY(Communications),
    CATEGORY(Alerts),
    CATEGORY(SoundEffects),
    CATEGORY(GameEffects),
    CATEGORY(GameMedia),
    CATEGORY(GameChat),
    CATEGORY(Speech),

    // deprecated
    CATEGORY(ForegroundOnlyMedia),
    CATEGORY(BackgroundCapableMedia),
};
































struct ArgStruct {
    ArgStruct() :
    flow(eAll),
    what(none),
    api(api_Unspecified),
    maxBufferSize(false),
    minBufferSize(false),
    wasapiTimerDriven(false),
    wasapiLoopback(false),
    wasapiOffload(false),
    streamCategory((AUDIO_STREAM_CATEGORY)(-1)),



    wasapiRaw(false),
    wasapiMatchFormat(false),
    wasapiFallBackToNonRawOffload(false),
    wasapiWaitIndefinitely(false),
    queryXAudio2Performance(false),
    timems(0),
    sinfreq(0),
    sinamp(0),
    sindither(Method_NoDithering),
    sweep(false),
    sweepfreq2(0.0),
    sweepseconds(0.0),
    sweeprepeat(false),
    sweeplinear(false),
    pinFactoryId((UINT)-1),
    msBufferSize(0),
    streamMethod(eStreamMethodUnspecified),
    modeSpecified(false),
    minPeriodicity(false),
    periodicityInFrames(0),
    useAudioClient3(false),
    signalProcessingMode(GUID_NULL),





    wfx(NULL)
    {}
    ~ArgStruct() { if (NULL != wfx) { CoTaskMemFree(wfx); } }
    
    EDataFlow flow;
    enum { sine, file, none } what;
    ApiEnum api;
    bool maxBufferSize;
    bool minBufferSize;
    bool wasapiTimerDriven;
    bool wasapiLoopback;
    bool wasapiOffload;
    AUDIO_STREAM_CATEGORY streamCategory;



    bool wasapiRaw;
    bool wasapiMatchFormat;
    bool wasapiFallBackToNonRawOffload;
    bool wasapiWaitIndefinitely;
    bool queryXAudio2Performance;
    CString endpoint;
    CString filename;
    double timems;
    float sinfreq;
    float sinamp;
    bool   sweep;
    double sweepfreq2;
    double sweepseconds;
    bool   sweeprepeat;
    bool   sweeplinear;
    DitheringMethod sindither;
    unsigned pinFactoryId;
    unsigned msBufferSize;
    StreamMethodEnum streamMethod;
    bool modeSpecified;
    GUID signalProcessingMode;
    WAVEFORMATEX *wfx;
    bool minPeriodicity;
    UINT32 periodicityInFrames;
    bool useAudioClient3;





};

void PrintUsage(LPCWSTR progname);
bool ParseArgs(int argc, _In_reads_(argc) LPCWSTR argv[], _Inout_ ArgStruct *args);

typedef enum
{
    PeriodicityAction_ListNone = 0,
    PeriodicityAction_List,



} PeriodicityAction;

void ListDevices(PeriodicityAction action);

HRESULT SetWasapiOptions(IUnknown *pAudioStreaming, const ArgStruct &options);
HRESULT SetKernelStreamingOptions(IUnknown *pAudioStreaming, const ArgStruct &options);
HRESULT SetXAudio2Options(IUnknown *pAudioStreaming, const ArgStruct &options);
HRESULT WasEnterPressed(HANDLE hStdIn, bool &bEnterPressed);
void LogXAudio2Performance(IUnknown *pStream);

class CApp {
public:
    CApp(const ArgStruct& opt) : options(opt) {}
    ~CApp() {}

    int RunRenderMode();
    int RunCaptureMode();

    int PlaySineWave(IAudioStreamingRender*);
    int PlayTimedSineWave(IAudioStreamingRender*);
    int PlaySineSweep(IAudioStreamingRender*);
    int PlayFileOnce(IAudioStreamingRender*);
    int PlayFileLooped(IAudioStreamingRender*);
    int CaptureFile(IAudioStreamingCapture*);
    int CaptureFileTimed(IAudioStreamingCapture*);
    int CaptureNull(IAudioStreamingCapture*, double);

private:
    const ArgStruct& options;
    
    void WaitForEnterOrEvent(IUnknown *pStream, HANDLE hEvent);
};

int CApp::RunRenderMode() {

    CComPtr<IAudioStreamingRender> pRender;
    HRESULT hr = S_OK;

    // If we didn't specify a device use default console
    if (options.endpoint.IsEmpty() || (options.endpoint.CompareNoCase(L"console") == 0)) {
        hr = CreateAudioStreamingRenderOnDefaultDevice(options.api, eConsole, g_pLog, &pRender);
        if (FAILED(hr)) {
            ERR(L"CreateAudioStreamingRenderOnDefaultDevice(%d, eConsole, Logger, IMMDevice): 0x%08x", options.api, hr);
            return -__LINE__;
        }

    } else if (options.endpoint.CompareNoCase(L"communications") == 0) {
        hr = CreateAudioStreamingRenderOnDefaultDevice(options.api, eCommunications, g_pLog, &pRender);
        if (FAILED(hr)) {
            ERR(L"CreateAudioStreamingRenderOnDefaultDevice(%d, eCommunications, Logger, IMMDevice): 0x%08x", options.api, hr);
            return -__LINE__;
        }
    } else {
        // Create library from endpoint ID
        CComHeapPtr<WCHAR> wasapiEndpointId;
        hr = FindWasapiEndpointId(eRender, options.endpoint, &wasapiEndpointId);
        if (FAILED(hr)) {
            return -__LINE__;
        }
        
        hr = CreateAudioStreamingRenderOnEndpointId(options.api, wasapiEndpointId, g_pLog, &pRender);
        if (FAILED(hr)) {
            ERR(L"CreateAudioStreamingRenderOnEndpointId(%d, %ls, Logger, *): 0x%08x",
                options.api, static_cast<LPCWSTR>(wasapiEndpointId), hr);
            return -__LINE__;
        }
    }

    switch (options.api) {
        case api_WASAPI:
        case api_WASAPIExclusive: {
            hr = SetWasapiOptions(pRender, options);
            break;
        }
        
        case api_KernelStreaming: {
            hr = SetKernelStreamingOptions(pRender, options);
            break;
        }
        
        case api_XAudio2: {
            hr = SetXAudio2Options(pRender, options);
            break;
        }
    }

    if (FAILED(hr)) {
        return -__LINE__;
    }

    switch (options.what) {
        case ArgStruct::sine:
            if (options.sweep) {
                return PlaySineSweep(pRender);
            } else if (0 == options.timems) {
                return PlaySineWave(pRender);
            } else {
                return PlayTimedSineWave(pRender);
            }

        case ArgStruct::file:
            if (0 == options.timems) {
                return PlayFileOnce(pRender);
            } else {
                return PlayFileLooped(pRender);
            }
    }
    
    ERR(L"Unexpected option: %u", options.what);
    return -__LINE__;
}

class CCallbackLogger : public IAudioStreamingAsyncCallback {
public:
    explicit CCallbackLogger(IBasicLog* pLog, HANDLE hEvent = NULL)
        : m_pLog(pLog)
        , m_hEvent(hEvent)
        , m_cRef(1)
    { 
        m_pLog->AddRef();
    }
    
    // IAudioStreamingAsyncCallback
    HRESULT STDMETHODCALLTYPE Callback(HRESULT hrRet) {
        if (FAILED(hrRet)) {
            ERR(L"Callback fired with hr = 0x%08x", hrRet);
        }
        
        if (NULL != m_hEvent) {
            SetEvent(m_hEvent);
        }
        
        return hrRet;
    }
    
    // IUnknown
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_cRef); }

    ULONG STDMETHODCALLTYPE Release() override {
        LONG retval = InterlockedDecrement(&m_cRef);

        if (0 >= retval) {
            delete this;
        }
        return retval;
    }

    STDMETHOD(QueryInterface)(REFIID riid, void** pp) override {
        if (NULL == pp) return E_POINTER;
        *pp = NULL;
        if (__uuidof(IUnknown) == riid || __uuidof(IAudioStreamingAsyncCallback) == riid) {
            *pp = (IAudioStreamingAsyncCallback*)this;
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
private:
    ~CCallbackLogger() {
        m_pLog->Release();
    }

    LONG m_cRef;
    IBasicLog* m_pLog;
    HANDLE m_hEvent;
};


int CApp::RunCaptureMode() {

    CComPtr<IAudioStreamingCapture> pCapture;
    HRESULT hr = S_OK;
    EDataFlow flow = eCapture;
    
    if (api_WASAPI == options.api && options.wasapiLoopback) {
        flow = eRender;
    }
    
    bool bUseDefault = true;
    ERole role = eConsole;
    
    if (options.endpoint.IsEmpty() || (options.endpoint.CompareNoCase(L"console") == 0)) {
        // normal case
    } else if (options.endpoint.CompareNoCase(L"communications") == 0) {
        role = eCommunications;
    } else {
        bUseDefault = false;
    }

    if (bUseDefault) {
        if (eRender == flow) {
            hr = CreateWasapiAudioStreamingCaptureOnDefaultRenderDevice(role, g_pLog, &pCapture);
            if (FAILED(hr)) {
                ERR(L"CreateWasapiAudioStreamingCaptureOnDefaultRenderDevice(%u, IMMDevice, *): 0x%08x", role, hr);
                return -__LINE__;
            }
        } else {
            hr = CreateAudioStreamingCaptureOnDefaultDevice(options.api, role, g_pLog, &pCapture);
            if (FAILED(hr)) {
                ERR(L"CreateAudioStreamingCaptureOnDefaultDevice(%d, %u, IMMDevice, *): 0x%08x", options.api, role, hr);
                return -__LINE__;
            }
        }
    } else {
        // Create library from endpoint ID
        CComHeapPtr<WCHAR> wasapiEndpointId;
        hr = FindWasapiEndpointId(flow, options.endpoint, &wasapiEndpointId);
        if (FAILED(hr)) {
            return -__LINE__;
        }
        
        hr = CreateAudioStreamingCaptureOnEndpointId(options.api, wasapiEndpointId, g_pLog, &pCapture);
        if (FAILED(hr)) {
            ERR(L"CreateAudioStreamingCaptureOnEndpointId(%d, %ls, NULL, *): 0x%08x",
                options.api, static_cast<LPCWSTR>(wasapiEndpointId), hr);
            return -__LINE__;
        }
    }

    switch (options.api) {
        case api_WASAPI:
        case api_WASAPIExclusive:
            hr = SetWasapiOptions(pCapture, options);
            break;
        
        case api_KernelStreaming:
            hr = SetKernelStreamingOptions(pCapture, options);
            break;
        
        case api_XAudio2:
            hr = SetXAudio2Options(pCapture, options);
            break;
    }

    if (FAILED(hr)) {
        return -__LINE__;
    }

    switch (options.what) {
        case ArgStruct::file:
            if (0 == options.timems) {
                return CaptureFile(pCapture);
            } else {
                return CaptureFileTimed(pCapture);
            }

        case ArgStruct::none:
            return CaptureNull(pCapture, options.timems);
    }

    ERR(L"Unexpected option: %u", options.what);
    return -__LINE__;
}


int CApp::PlaySineWave(IAudioStreamingRender* pRender) {

    HRESULT hr = pRender->Initialize(options.wfx);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingRender::Initialize(wfx): 0x%08x", hr);
        return -__LINE__;
    }

    hr = pRender->PlaySineWaveAsync(options.sinfreq, options.sinamp, options.sindither);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingRender::PlaySineWaveAsync(%f, %f, %d): 0x%08x",
            options.sinfreq, options.sinamp, options.sindither, hr);
        return -__LINE__;
    }

    WaitForEnterOrEvent(pRender, NULL);
    
    hr = pRender->Stop();
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingRender::Stop(): 0x%08x", hr);
        return -__LINE__;
    }

    return 0;
}

int CApp::PlayTimedSineWave(IAudioStreamingRender* pRender) {
    HRESULT hr = pRender->Initialize(options.wfx);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingRender::Initialize(wfx): 0x%08x", hr);
        return -__LINE__;
    }

    UINT32 ms = (UINT32)options.timems;
    hr = pRender->PlaySineWave(options.sinfreq, options.sinamp, options.sindither, ms);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingRender::PlaySineWave(%f, %f, %d, %d): 0x%08x",
            options.sinfreq, options.sinamp, options.sindither, ms, hr);
        return -__LINE__;
    }

    return 0;
}

int CApp::PlaySineSweep(IAudioStreamingRender* pRender) {
    HRESULT hr = pRender->Initialize(options.wfx);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingRender::Initialize(wfx): 0x%08x", hr);
        return -__LINE__;
    }
    
    CComHeapPtr<WAVEFORMATEX> pWfx;
    hr = pRender->GetWaveFormat(&pWfx);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingRender::GetWaveFormat: 0x%08x", hr);
        return -__LINE__;
    }

    CComPtr<ISignalSource> pSignalSource;    
    hr = CreateSineSignalSource(
        g_pLog, XFAIL,
        pWfx,
        options.sinamp,
        options.sinfreq,
        0.0, // initial phase
        0.0, // dc
        options.sindither,
        &pSignalSource
    );
    if (FAILED(hr)) {
        ERR(L"CreateSineSignalSource: 0x%08x", hr);
        return -__LINE__;
    }
    
    CComPtr<ISineSweepControl> pSineSweepControl;
    hr = pSignalSource->QueryInterface(IID_PPV_ARGS(&pSineSweepControl));
    if (FAILED(hr)) {
        ERR(L"Sine signal source did not QI to ISineSweepControl");
        return -__LINE__;
    }
    
    SineSweepControlFlags sweepFlags = SineSweepControlFlags_Default;
    if (options.sweeprepeat) {
        sweepFlags |= SineSweepControlFlags_Repeat;
    }
    
    if (options.sweeplinear) {
        sweepFlags |= SineSweepControlFlags_Linear;
    }
    
    hr = pSineSweepControl->SweepTo(
        options.sweepfreq2,
        options.sweepseconds,
        sweepFlags
    );
    if (FAILED(hr)) {
        ERR(L"ISineSweepControl::SweepTo: 0x%08x", hr);
        return -__LINE__;
    }
    
    if (0 == options.timems) {
        hr = pRender->PlaySignalSourceAsync(pSignalSource);
        if (FAILED(hr)) {
            ERR(L"IAudioStreamingRender::PlaySignalSourceAsync: 0x%08x", hr);
            return -__LINE__;
        }

        WaitForEnterOrEvent(pRender, NULL);

        hr = pRender->Stop();
        if (FAILED(hr)) {
            ERR(L"IAudioStreamingRender::Stop: 0x%08x", hr);
            return -__LINE__;
        }
    } else {
        hr = pRender->PlaySignalSource(pSignalSource, (DWORD)options.timems);
        if (FAILED(hr)) {
            ERR(L"IAudioStreamingRender::PlaySignalSource: 0x%08x", hr);
            return -__LINE__;
        }
    }
 
    return 0;
}

int CApp::PlayFileOnce(IAudioStreamingRender* pRender) {

    CComPtr<IWaveFileSource> pWfs;
    HRESULT hr = CreateWaveFileSource(g_pLog, XFAIL, options.filename, WAVEFILESOURCE_ALLOWMISSINGFACTCHUNK, &pWfs);
    if (FAILED(hr)) {
        ERR(L"CreateWaveFileSource(%ls): 0x%08x", static_cast<LPCWSTR>(options.filename), hr);
        return -__LINE__;
    }

    WAVEFORMATEX* pwfxTmp;
    hr = pWfs->GetWaveFormat(&pwfxTmp);
    if (FAILED(hr)) {
        ERR(L"IWaveFileSource::GetWaveFormat(): 0x%08x", hr);
        return -__LINE__;
    }

    CAutoWaveFormatEx wfx(pwfxTmp);
    CoTaskMemFree(pwfxTmp);

    hr = pRender->Initialize(wfx);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingRender::Initialize(wfx): 0x%08x", hr);
        return -__LINE__;
    }

    UINT32 nFrames = 0, nBytes = 0;
    hr = pWfs->GetFrameCount(&nFrames, &nBytes);
    if (FAILED(hr)) {
        ERR(L"IWaveFileSource::GetFrameCount(): 0x%08x", hr);
        return -__LINE__;
    }

    if (0 == nBytes) {
        ERR(L"IWaveFileSource::GetFrameCount() returned 0 bytes");
        return -__LINE__;
    }
    
    CPtrArray<BYTE> buffer(nBytes);
    if (NULL == buffer) {
        ERR(L"Attempt to allocate %u bytes failed", nBytes);
        return -__LINE__;
    }
    
    hr = pWfs->FillBuffer(buffer, nFrames, nBytes);
    if (FAILED(hr)) {
        ERR(L"IWaveFileSource::FillBuffer(*, %d, %d): 0x%08x", nFrames, nBytes, hr);
        return -__LINE__;
    }

    CHandle shCompletionEvent(CreateEvent(NULL, FALSE, FALSE, NULL));
    if (shCompletionEvent == NULL)
    {
        ERR(L"Unable to create event! (%d)", GetLastError());
        return -__LINE__;
    }

    CCallbackLogger* pCallback = new CCallbackLogger(g_pLog, shCompletionEvent);
    CComPtr<IAudioStreamingAsyncCallback> spCallback;
    if (NULL != pCallback)
    {
        spCallback.Attach(pCallback);
        pCallback = NULL;
    }
    else
    {
        ERR(L"Out of memory!");
        return -__LINE__;
    }

    // play all the way through...
    hr = pRender->PlayBufferAsync(buffer, nBytes, spCallback);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingRender::PlayBuffer(*, %d): 0x%08x", nBytes, hr);
        return -__LINE__;
    }
    
    WaitForEnterOrEvent(pRender, shCompletionEvent);

    // just for the error
    hr = pRender->Stop();
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingRender::Stop(): 0x%08x", hr);
        return -__LINE__;
    }
    
    return 0;
}


int CApp::PlayFileLooped(IAudioStreamingRender* pRender) {

    CComPtr<IWaveFileSource> pWfs;
    HRESULT hr = CreateWaveFileSource(g_pLog, XFAIL, options.filename, WAVEFILESOURCE_ALLOWMISSINGFACTCHUNK, &pWfs);
    if (FAILED(hr)) {
        ERR(L"CreateWaveFileSource(%ls): 0x%08x", static_cast<LPCWSTR>(options.filename), hr);
        return -__LINE__;
    }

    WAVEFORMATEX* pwfxTmp;
    hr = pWfs->GetWaveFormat(&pwfxTmp);
    if (FAILED(hr)) {
        ERR(L"IWaveFileSource::GetWaveFormat(): 0x%08x", hr);
        return -__LINE__;
    }

    CAutoWaveFormatEx wfx(pwfxTmp);
    CoTaskMemFree(pwfxTmp);

    hr = pRender->Initialize(wfx);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingRender::Initialize(wfx): 0x%08x", hr);
        return -__LINE__;
    }

    UINT32 nFrames = 0;
    hr = pWfs->GetFrameCount(&nFrames, NULL);
    if (FAILED(hr)) {
        ERR(L"IWaveFileSource::GetFrameCount() (returned %d frames): 0x%08x", nFrames, hr);
        return -__LINE__;
    }

    UINT32 nBytes = nFrames * wfx->nBlockAlign;

    CPtrArray<BYTE> buffer(nBytes);
    if (NULL == buffer) {
        ERR(L"Could not allocate %u bytes", nBytes);
        return -__LINE__;
    }

    hr = pWfs->FillBuffer(buffer, nFrames, nBytes);
    if (FAILED(hr)) {
        ERR(L"IWaveFileSource::FillBuffer(*, %d, %d): 0x%08x", nFrames, nBytes, hr);
        return -__LINE__;
    }

    // start it playing...
    hr = pRender->PlayBufferLooped(buffer, nBytes);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingRender::PlayBufferLooped(*, %d): 0x%08x", nBytes, hr);
        return -__LINE__;
    }

    // wait for the alloted time...
    Sleep((DWORD)options.timems);

    // ok, time to stop.
    hr = pRender->Stop();
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingRender::Stop(): 0x%08x", hr);
        return -__LINE__;
    }

    return 0;
}

int CApp::CaptureFile(IAudioStreamingCapture* pCapture) {

    HRESULT hr = S_OK;

    hr = pCapture->Initialize(options.wfx);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingCapture::Initialize(wfx): 0x%08x", hr);
        return -__LINE__;
    }

    WAVEFORMATEX* pwfxTmp;
    hr = pCapture->GetWaveFormat(&pwfxTmp);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingCapture::GetWaveFormat(): 0x%08x", hr);
        return -__LINE__;
    }

    CAutoWaveFormatEx wfx(pwfxTmp);
    CoTaskMemFree(pwfxTmp);

    LOG(L"Capturing %lg milliseconds to file %ls", options.timems, static_cast<LPCWSTR>(options.filename));

    CComPtr<IWaveFileSink> pWfs;
    hr = CreateWaveFileSink(g_pLog, XFAIL, options.filename, wfx, &pWfs);
    if (FAILED(hr)) {
        ERR(L"CreateWaveFileSink(%ls, wfx, **): 0x%08x", static_cast<LPCWSTR>(options.filename), hr);
        return -__LINE__;
    }

    hr = pCapture->CaptureToSinkAsync(pWfs);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingCapture::CaptureToSink(*): 0x%08x", hr);
        return -__LINE__;
    }

    WaitForEnterOrEvent(pCapture, NULL);

    hr = pCapture->Stop();
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingCapture::Stop(): 0x%08x", hr);
        return -__LINE__;
    }

    hr = pWfs->Finalize();
    if (FAILED(hr)) {
        ERR(L"IWaveFileSink::Finalize(): 0x%08x", hr);
        return -__LINE__;
    }

    return 0;
}

int CApp::CaptureFileTimed(IAudioStreamingCapture* pCapture) {

    HRESULT hr = S_OK;

    hr = pCapture->Initialize(options.wfx);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingCapture::Initialize(wfx): 0x%08x", hr);
        return -__LINE__;
    }

    WAVEFORMATEX* pwfxTmp;
    hr = pCapture->GetWaveFormat(&pwfxTmp);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingCapture::GetWaveFormat(): 0x%08x", hr);
        return -__LINE__;
    }

    CAutoWaveFormatEx wfx(pwfxTmp);
    CoTaskMemFree(pwfxTmp);

    LOG(L"Capturing %lg milliseconds to file %ls", options.timems, static_cast<LPCWSTR>(options.filename));

    CComPtr<IWaveFileSink> pWfs;
    hr = CreateWaveFileSink(g_pLog, XFAIL, options.filename, wfx, &pWfs);
    if (FAILED(hr)) {
        ERR(L"CreateWaveFileSink(%ls, wfx, **): 0x%08x", static_cast<LPCWSTR>(options.filename), hr);
        return -__LINE__;
    }

    hr = pCapture->CaptureToSink(pWfs, options.timems);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingCapture::CaptureToSink(*, %lg): 0x%08x", options.timems, hr);
        return -__LINE__;
    }

    hr = pWfs->Finalize();
    if (FAILED(hr)) {
        ERR(L"IWaveFileSink::Finalize(): 0x%08x", hr);
        return -__LINE__;
    }

    return 0;
}

int CApp::CaptureNull(IAudioStreamingCapture* pCapture, double milliseconds) {

    HRESULT hr = S_OK;
    int retval = 0;

    CHandle shTimedEvent(CreateWaitableTimerEx(nullptr, nullptr, 0, SYNCHRONIZE | TIMER_MODIFY_STATE));
    if (nullptr == shTimedEvent) {
        ERR(L"CreateWaitableTimerEx returned null: GetLastError() = %u", GetLastError());
        return -__LINE__;
    }

    hr = pCapture->Initialize(options.wfx);
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingCapture::Initialize(wfx): 0x%08x", hr);
        return -__LINE__;
    }

    hr = pCapture->CaptureWithoutBufferAsync();
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingCapture::CaptureWithoutBufferAsync(): 0x%08x", hr);
        return -__LINE__;
    }

    if (milliseconds > 0) {
        LARGE_INTEGER dueTime = {};
        // negative indicates relative time
        dueTime.QuadPart = -(LONGLONG)(milliseconds * HNS_PER_MILLISECOND);

        if (!SetWaitableTimer(
            shTimedEvent,
            &dueTime,
            0, // don't recur
            nullptr, // no completion routine
            nullptr, // no arguments
            FALSE // don't wake up the system
        )) {
            ERR(L"SetWaitableTimer returned FALSE: GetLastError() = %u", GetLastError());
            retval = -__LINE__;
            goto Stop;
        }
    }

    WaitForEnterOrEvent(pCapture, shTimedEvent);

Stop:
    hr = pCapture->Stop();
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingCapture::Stop(): 0x%08x", hr);
        return -__LINE__;
    }
    
    return retval;
}

HRESULT SetWasapiOptions(IUnknown *pAudioStreaming, const ArgStruct &options) {
    CComPtr<IAudioStreamingControl> pAudioStreamingControl;
    HRESULT hr = pAudioStreaming->QueryInterface(__uuidof(IAudioStreamingControl), (void**)&pAudioStreamingControl);
    if (FAILED(hr)) {
        ERR(L"IUnknown::QueryInterface(IAudioStreamingControl) failed: 0x%08x", hr);
        return hr;
    }

    CComPtr<IAudioStreamingWasapiControl> pAudioStreamingWasapiControl;
    hr = pAudioStreamingControl->GetService(
        __uuidof(IAudioStreamingWasapiControl),
        (void**)&pAudioStreamingWasapiControl
    );
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingControl::GetService(IAudioStreamingWasapiControl) failed: 0x%08x", hr);
        return hr;
    }
    
    DWORD dwFlags = 0;
    if (options.wasapiFallBackToNonRawOffload) {
        dwFlags |= WASAPI_FALL_BACK_TO_NONRAW_OFFLOAD;
    }
    
    if (options.wasapiWaitIndefinitely) {
        dwFlags |= WASAPI_WAIT_INDEFINITELY;
    }
    
    if (dwFlags) {
        hr = pAudioStreamingWasapiControl->SetFlags(dwFlags);
        if (FAILED(hr)) {
            ERR(L"IAudioStreamingControl::SetFlags failed: 0x%08x", hr);
            return hr;
        }
    }
    
    AudioClientProperties* pClientProperties = NULL;
   
    AudioClientProperties clientProperties = { 0 };
    clientProperties.cbSize = sizeof(AudioClientProperties);
    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = (-1 == options.streamCategory ? AudioCategory_Other : options.streamCategory); 
#if (NTDDI_VERSION >= NTDDI_WINBLUE)     
    clientProperties.Options = AUDCLNT_STREAMOPTIONS_NONE;
#endif
    pClientProperties = &clientProperties;
    
   














    DWORD dwStreamFlags;
    REFERENCE_TIME hnsBufferDuration;
    REFERENCE_TIME hnsPeriodicity;
    GUID guidAudioSession;
    
    // get the current parameters
    hr = pAudioStreamingWasapiControl->GetAudioClientInitializeParameters(
        &dwStreamFlags, &hnsBufferDuration, &hnsPeriodicity, &guidAudioSession
    );
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingWasapiControl::GetAudioClientInitializeParameters failed: 0x%08x", hr);
        return hr;
    }

    // set the event-driven flag as necessary
    if (options.wasapiTimerDriven) {
        dwStreamFlags &= ~AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
    } else {
        dwStreamFlags |= AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
    }
    
    // set the loopback flag as necessary
    if (options.wasapiLoopback) {
        dwStreamFlags |= AUDCLNT_STREAMFLAGS_LOOPBACK;
    }

    // max can only be done for offload
    if ((options.maxBufferSize) && !options.wasapiOffload) {
        ERR(L"buffer size of \"max\" can only be specified in offload mode");
        return E_INVALIDARG;
    }

    if ((options.minBufferSize) && !options.wasapiOffload) {
        ERR(L"buffer size of \"min\" can only be specified in offload mode");
        return E_INVALIDARG;
    }

    // for "min", pass 0; for "max", pass -1
    if (options.minBufferSize) {
        hnsBufferDuration = BUFFER_SIZE_MINIMUM;
    } else if (options.maxBufferSize) {
        hnsBufferDuration = BUFFER_SIZE_MAXIMUM;
    } else if (options.msBufferSize) {
        hnsBufferDuration = HNS_PER_MILLISECOND * options.msBufferSize;
    }

    // shared-mode does not allow specifying a periodicity
    // offload and exclusive mode require the periodicity to be the buffer size
    if (api_WASAPIExclusive == options.api || options.wasapiOffload) {
        hnsPeriodicity = hnsBufferDuration;
    }

    // AudioClient3 APIs cannot be used with exclusive and offload streams
    if (options.useAudioClient3 && (options.api == api_WASAPIExclusive || options.wasapiOffload || options.wasapiLoopback)) {
        ERR(L"AudioClient3 APIs only available for shared mode, non-offload, non-loopback streams.");
        return E_INVALIDARG;
    }

    if (options.useAudioClient3) {
        // zero out the hns buffer size and hns periodicity
        hnsBufferDuration = 0;
        hnsPeriodicity = 0;

        UINT32 PeriodicityInFrames = options.minPeriodicity ? BUFFER_SIZE_MINIMUM : options.periodicityInFrames;

        hr = pAudioStreamingWasapiControl->SetAudioClient3InitializeParameters(
            PeriodicityInFrames
            );
        if (FAILED(hr)) {
            ERR(L"IAudioStreamingWasapiControl::SetAudioClient3InitializeParameters: 0x%08x", hr);
            return hr;
        }
    }

























    // set the current parameters
    hr = pAudioStreamingWasapiControl->SetAudioClientInitializeParameters(
        dwStreamFlags, hnsBufferDuration, hnsPeriodicity, guidAudioSession
        );
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingWasapiControl::SetAudioClientInitializeParameters failed: 0x%08x", hr);
        return hr;
    }

    if (options.wasapiOffload) {
        pClientProperties->bIsOffload = TRUE;
        pClientProperties->eCategory = (-1 == options.streamCategory ? AudioCategory_Media : options.streamCategory);        
    }

#if (NTDDI_VERSION >= NTDDI_WINBLUE)
    if (options.wasapiRaw) {
        pClientProperties->Options |= AUDCLNT_STREAMOPTIONS_RAW;
    }
#endif

#if (NTDDI_VERSION >= NTDDI_WIN10)
    if (options.wasapiMatchFormat) {
        pClientProperties->Options |= AUDCLNT_STREAMOPTIONS_MATCH_FORMAT;
    }
#endif

    if (options.wasapiOffload || options.wasapiRaw || options.wasapiMatchFormat || -1 != options.streamCategory) {
        hr = pAudioStreamingWasapiControl->SetClientProperties(pClientProperties);
        if (FAILED(hr)) {
            ERR(L"IAudioStreamingWasapiControl::SetClientProperties failed: 0x%08x", hr);
            return hr;
        }
    }
        
    return S_OK;
}

HRESULT SetKernelStreamingOptions(IUnknown *pAudioStreaming, const ArgStruct &options) {
    CComPtr<IAudioStreamingControl> pAudioStreamingControl;
    HRESULT hr = pAudioStreaming->QueryInterface(__uuidof(IAudioStreamingControl), (void**)&pAudioStreamingControl);
    if (FAILED(hr)) {
        ERR(L"IUnknown::QueryInterface(IAudioStreamingControl) failed: 0x%08x", hr);
        return hr;
    }

    CComPtr<IAudioKernelStreamingControl> pAudioKernelStreamingControl;
    hr = pAudioStreamingControl->GetService( __uuidof(IAudioKernelStreamingControl), (void**)&pAudioKernelStreamingControl );
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingControl::GetService(IAudioKernelStreamingControl) failed: 0x%08x", hr);
        return hr;
    }

    if ((unsigned)-1 != options.pinFactoryId) {
        hr = pAudioKernelStreamingControl->SetStreamPinFactoryId( options.pinFactoryId );
        if (FAILED(hr)) {
            ERR(L"IAudioKernelStreamingControl::SetStreamPinFactoryId failed: 0x%08x", hr);
            return hr;
        }
    }

    if (options.maxBufferSize) {
        hr = pAudioKernelStreamingControl->UseMaxBufferSize();
        if (FAILED(hr)) {
            ERR(L"IAudioKernelStreamingControl::UseMaxBufferSize() failed: 0x%08x", hr);
            return hr;
        }
    }
    else if (options.minBufferSize) {
        hr = pAudioKernelStreamingControl->UseMinBufferSize();
        if (FAILED(hr)) {
            ERR(L"IAudioKernelStreamingControl::UseMinBufferSize() failed: 0x%08x", hr);
            return hr;
        }
    }
    else if ( 0 != options.msBufferSize ) {
        hr = pAudioKernelStreamingControl->SetMsBufferSize( options.msBufferSize );
        if (FAILED(hr)) {
            ERR(L"IAudioKernelStreamingControl::SetMsBufferSize failed: 0x%08x", hr);
            return hr;
        }
    }

    hr = pAudioKernelStreamingControl->SetStreamAlgorithm( options.streamMethod );
    if (FAILED(hr)) {
        ERR(L"IAudioKernelStreamingControl::SetStreamAlgorithm failed: 0x%08x", hr);
        return hr;
    }

    if (options.modeSpecified) {
        hr = pAudioKernelStreamingControl->SetSignalProcessingMode( options.signalProcessingMode );
        if (FAILED(hr)) {
            ERR(L"IAudioKernelStreamingControl::SetSignalProcessingMode failed: 0x%08x", hr);
            return hr;
        }
    }

    return S_OK;
}

HRESULT SetXAudio2Options(IUnknown *pAudioStreaming, const ArgStruct &options) {
    CComPtr<IAudioStreamingControl> pAudioStreamingControl;
    HRESULT hr = pAudioStreaming->QueryInterface(__uuidof(IAudioStreamingControl), (void**)&pAudioStreamingControl);
    if (FAILED(hr)) {
        ERR(L"IUnknown::QueryInterface(IAudioStreamingControl) failed: 0x%08x", hr);
        return hr;
    }
    
    CComPtr<IAudioStreamingXAudio2Control> pAudioStreamingXAudio2Control;
    hr = pAudioStreamingControl->GetService(IID_PPV_ARGS(&pAudioStreamingXAudio2Control));
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingControl::GetService(IAudioStreamingXAudio2Control) failed: 0x%08x", hr);
        return hr;
    }
    
    if (-1 != options.streamCategory) {
        hr = pAudioStreamingXAudio2Control->SetStreamCategory(options.streamCategory);
        if (FAILED(hr)) {
            ERR(L"IAudioStreamingXAudio2Control::SetStreamCategory failed: 0x%08x", hr);
            return hr;
        }
    }

    return S_OK;
}

bool ParseArgs(
    int argc,
    _In_reads_(argc) LPCWSTR argv[],
    _Inout_ ArgStruct *args
) {

    if (0 == argc - 1) {
        PrintUsage(argv[0]);
        return false;
    }

    if (
        1 == argc - 1 &&
        0 == _wcsicmp(argv[1], L"-devices")
    ) {
        ListDevices(PeriodicityAction_ListNone);
        return false;
    }

#if NTDDI_VERSION >= NTDDI_WIN10
    if (
        1 == argc - 1 &&
        0 == _wcsicmp(argv[1], L"-periods")
        ) {
        ListDevices(PeriodicityAction_List);
        return false;
    }
#endif












    bool bSeenAPI = false;
    bool bSeenDev = false;
    bool bSeenTime = false;
    bool bSeenAction = false;
    bool bSeenFile = false;
    bool bSeenFormat = false;
    bool bSeenBuffer = false;
    bool bSeenPeriodicity = false;
    
    bool bKsOpt_Pin = false;
    bool bKsOpt_Algorithm = false;
    bool bKsOpt_Notification = false;
    PWCHAR pszEnd = L"";

    for (int i = 1; i < argc; ++i) {
        CString szArg = argv[i];

        // -api
        if (0 == szArg.CompareNoCase(L"-api")) {
            if (bSeenAPI) {
                ERR(L"multiple -api arguments specified");
                return false;
            }

            bSeenAPI = true;

            if (++i >= argc) {
                ERR(L"-api requires an argument");
                return false;
            }

            CString szAPI = argv[i];
            if (0 == szAPI.CompareNoCase(L"dsound")) {
                args->api = api_DirectSound;
                continue;
            }
            if (0 == szAPI.CompareNoCase(L"winmm")) {
                args->api = api_Wave;
                continue;
            }
            if (0 == szAPI.CompareNoCase(L"ks")) {
                args->api = api_KernelStreaming;
                continue;
            }
            if (0 == szAPI.CompareNoCase(L"mf")) {
                args->api = api_MediaFoundation;
                continue;
            }
            if (0 == szAPI.CompareNoCase(L"wasapi")) {
                args->api = api_WASAPI;
                // we might change this to api_WASAPIExclusive later
                continue;
            }
            if (0 == szAPI.CompareNoCase(L"xaudio2")) {
                args->api = api_XAudio2;
                continue;
            }

            ERR(L"Unrecognized api \"%s\"", static_cast<LPCWSTR>(szAPI));
            return false;
        } // -api

        // -device
        if (0 == szArg.CompareNoCase(L"-device")) {
            if (bSeenDev) {
                ERR(L"multiple -device arguments specified");
                return false;                
            }

            if (bSeenFormat) {
                ERR(L"-device argument cannot come after -format argument");
                return false;
            }
            
            bSeenDev = true;

            if (++i >= argc) {
                ERR(L"-device requires an argument");
                return false;
            }

            args->endpoint = argv[i];
            continue;
        }

        // -time
        if (0 == szArg.CompareNoCase(L"-time")) {
            if (bSeenTime) {
                ERR(L"multiple -time arguments specified");
                return false;                
            }

            bSeenTime = true;

            if (++i >= argc) {
                ERR(L"-time requires an argument");
                return false;
            }

            args->timems = wcstod(argv[i], &pszEnd);
#pragma prefast(suppress: __WARNING_POTENTIAL_BUFFER_OVERFLOW_NULLTERMINATED, "pszEnd is guaranteed to point to something")
            if (0 != errno || NULL == pszEnd || L'\0' != *pszEnd) {
                ERR(L"Could not parse -time \"%s\"", argv[i]);
                return false;
            }
            continue;
        }

        // -play-sine
        if (0 == szArg.CompareNoCase(L"-play-sine")) {
            if (bSeenAction) {
                ERR(L"multiple actions specified");
                return false;                
            }        
        
            bSeenAction = true;

            args->flow = eRender;
            args->what = ArgStruct::sine;

            if (++i >= argc) {
                ERR(L"-play-sine requires an argument");
                return false;                
            }

            args->sinfreq = (float)wcstod(argv[i], &pszEnd);
#pragma prefast(suppress: __WARNING_POTENTIAL_BUFFER_OVERFLOW_NULLTERMINATED, "pszEnd is guaranteed to point to something")
            if (0 != errno || NULL == pszEnd || L'\0' != *pszEnd) {
                ERR(L"Could not parse frequency \"%s\"", argv[i]);
                return false;
            }

            if (++i >= argc) {
                ERR(L"-play-sine requires another argument");
                return false;                
            }
            
            args->sinamp = (float)wcstod(argv[i], &pszEnd);
#pragma prefast(suppress: __WARNING_POTENTIAL_BUFFER_OVERFLOW_NULLTERMINATED, "pszEnd is guaranteed to point to something")
            if (0 != errno || NULL == pszEnd || L'\0' != *pszEnd) {
                ERR(L"Could not parse amplitude \"%s\"", argv[i]);
                return false;
            }
            continue;
        }
        
        // -sweep <end-frequency> <seconds>
        if (0 == szArg.CompareNoCase(L"-sweep")) {
            if (args->sweep) {
                ERR(L"Only one -sweep supported");
                return false;
            }
            args->sweep = true;
            
            if (!bSeenAction || args->what != ArgStruct::sine) {
                ERR(L"-sweep requires -play-sine");
                return false;                
            }

            // end-frequency
            if (++i >= argc) {
                ERR(L"-sweep requires two arguments");
                return false;                
            }

            args->sweepfreq2 = wcstod(argv[i], &pszEnd);
#pragma prefast(suppress: __WARNING_POTENTIAL_BUFFER_OVERFLOW_NULLTERMINATED, "pszEnd is guaranteed to point to something")
            if (0 != errno || NULL == pszEnd || L'\0' != *pszEnd) {
                ERR(L"Could not parse frequency \"%s\"", argv[i]);
                return false;
            }
            
            if (++i >= argc) {
                ERR(L"-sweep requires two arguments");
                return false;                
            }

            args->sweepseconds = wcstod(argv[i], &pszEnd);
#pragma prefast(suppress: __WARNING_POTENTIAL_BUFFER_OVERFLOW_NULLTERMINATED, "pszEnd is guaranteed to point to something")
            if (0 != errno || NULL == pszEnd || L'\0' != *pszEnd) {
                ERR(L"Could not parse seconds \"%s\"", argv[i]);
                return false;
            }
            continue;
        }
        
        // -repeat
        if (0 == szArg.CompareNoCase(L"-repeat")) {
            if (args->sweeprepeat) {
                ERR(L"Only one -repeat supported");
                return false;
            }
            
            if (!args->sweep) {
                ERR(L"-repeat requires -sweep");
                return false;
            }
            args->sweeprepeat = true;
            
            continue;
        }

        // -linear
        if (0 == szArg.CompareNoCase(L"-linear")) {
            if (args->sweeplinear) {
                ERR(L"Only one -linear supported");
                return false;
            }
            
            if (!args->sweep) {
                ERR(L"-linear requires -sweep");
                return false;
            }
            args->sweeplinear = true;
            
            continue;
        }

        // -play-file
        if (0 == szArg.CompareNoCase(L"-play-file")) {
            if (bSeenAction) {
                ERR(L"multiple actions specified");
                return false;                
            }        
        
            bSeenAction = true;

            args->flow = eRender;
            args->what = ArgStruct::file;

            if (++i >= argc) {
                ERR(L"-play-file requires an argument");
                return false;                
            }
            
            args->filename = argv[i];
            continue;
        }

        // -record
        if (0 == szArg.CompareNoCase(L"-record")) {
            if (bSeenAction) {
                ERR(L"multiple actions specified");
                return false;                
            }        
        
            bSeenAction = true;

            args->flow = eCapture;
            continue;
        }

        // -file
        if (0 == szArg.CompareNoCase(L"-file")) {
            if (bSeenFile) {
                ERR(L"multiple files specified");
                return false;                
            }        
        
            bSeenFile = true;

            if (++i >= argc) {
                ERR(L"-file requires an argument");
                return false;                
            }
            
            args->what = ArgStruct::file;
            args->filename = argv[i];
            continue;
        }

        // there are optional arguments for kernel streaming

        // which pin to render/capture to/from
        if (0 == szArg.CompareNoCase(L"-pin")) {
            if (bKsOpt_Pin) {
                ERR(L"multiple ks pin factories specified");
                return false;
            }

            if (++i >= argc) {
                ERR(L"-pin requires an argument");
                return false;
            }

            bKsOpt_Pin = true;
            args->pinFactoryId = wcstoul(argv[i], &pszEnd, 0);
#pragma prefast(suppress: __WARNING_POTENTIAL_BUFFER_OVERFLOW_NULLTERMINATED, "pszEnd is guaranteed to point to something")
            if (0 != errno || NULL == pszEnd || L'\0' != *pszEnd) {
                ERR(L"Could not parse -pin \"%s\"", argv[i]);
                return false;
            }
            continue;
        }

        // buffer size
        if (0 == szArg.CompareNoCase(L"-buffer")) {
            if (bSeenBuffer) {
                ERR(L"multiple buffer sizes specified");
                return false;
            }

            if (bSeenPeriodicity) {
                ERR(L"-Periodicity and -buffer cannot be specified together");
                return false;
            }

            if (++i >= argc) {
                ERR(L"-buffer requires an argument");
                return false;
            }

            CString szBufferArg = argv[i];

            if (0 == szBufferArg.CompareNoCase(L"max")) {
                bSeenBuffer = true;

                args->maxBufferSize = true;
                continue;
            }
            else if (0 == szBufferArg.CompareNoCase(L"min")) {
                bSeenBuffer = true;

                args->minBufferSize = true;
                continue;
            }
            else {
                bSeenBuffer = true;

                args->msBufferSize = wcstoul(szBufferArg, &pszEnd, 0);
#pragma prefast(suppress: __WARNING_POTENTIAL_BUFFER_OVERFLOW_NULLTERMINATED, "pszEnd is guaranteed to point to something")
                if (0 != errno || NULL == pszEnd || L'\0' != *pszEnd) {
                    ERR(L"Could not parse -buffer \"%s\"", argv[i]);
                    return false;
                }
                continue;
            }
        }

        if (0 == szArg.CompareNoCase(L"-periodicity")) {
            if (bSeenBuffer) {
                ERR(L"-Buffer and -Periodidicty cannot be specified together");
                return false;
            }

            if (bSeenPeriodicity) {
                ERR(L"multiple values of periodidicty specified");
                return false;
            }

            if (++i >= argc) {
                ERR(L"-periodicity requires an argument");
                return false;
            }

            CString szBufferArg = argv[i];

            if (0 == szBufferArg.CompareNoCase(L"min")) {
                bSeenPeriodicity = true;

                args->minPeriodicity = true;
                args->useAudioClient3 = true;
                continue;
            }
            else {
                bSeenPeriodicity = true;

                args->periodicityInFrames = wcstoul(szBufferArg, &pszEnd, 0);
#pragma prefast(suppress: __WARNING_POTENTIAL_BUFFER_OVERFLOW_NULLTERMINATED, "pszEnd is guaranteed to point to something")
                if (0 != errno || NULL == pszEnd || L'\0' != *pszEnd) {
                    ERR(L"Could not parse -periodicity \"%s\"", argv[i]);
                    return false;
                }
                args->useAudioClient3 = true;
                continue;
            }
        }










































        // streaming algorithm
        if (0 == szArg.CompareNoCase(L"-algorithm")) {
            if (bKsOpt_Algorithm) {
                ERR(L"multiple ks streaming algorithms specified");
                return false;
            }

            if (++i >= argc) {
                ERR(L"-algorithm requires an argument");
                return false;
            }

            CString szKsAlgorithmArg = argv[i];

            if (0 == szKsAlgorithmArg.CompareNoCase(L"standard")) {
                bKsOpt_Algorithm = true;

                args->streamMethod = eStandardStream;
                continue;
            }
            else if (0 == szKsAlgorithmArg.CompareNoCase(L"standard-looped")) {
                bKsOpt_Algorithm = true;

                args->streamMethod = eStandardLoopedStream;
                continue;
            }
            else if (0 == szKsAlgorithmArg.CompareNoCase(L"wavert")) {
                bKsOpt_Algorithm = true;

                args->streamMethod = eRealTimeStream;
                continue;
            }
            else if (0 == szKsAlgorithmArg.CompareNoCase(L"enhanced-wavert")) {
                bKsOpt_Algorithm = true;

                args->streamMethod = eEnhancedRealTimeStream;
                continue;
            }
            else {
                ERR(L"Invalid -algorithm argument.  Only 'Standard', 'Standard-Looped', 'WaveRT' or 'Enhanced-WaveRT' are accepted.");
                return false;
            }
        }

        // streaming algorithm with notifications
        if (0 == szArg.CompareNoCase(L"-Notifications")) {
            if (bKsOpt_Notification) {
                ERR(L"multiple ks notification flags specified");
                return false;
            }

            bKsOpt_Notification = true;

            continue;
        }
        
        // signal processing mode
        if (0 == szArg.CompareNoCase(L"-Mode")) {
            if (api_KernelStreaming != args->api) {
                ERR(L"-Mode only applies to Kernel Streaming");
                return false;
            }

            if (args->modeSpecified) {
                ERR(L"multiple signal processing modes specified");
                return false;
            }
            
            args->modeSpecified = true;

            if (++i >= argc) {
                ERR(L"-mode requires an argument");
                return false;
            }

            CString szModeArg = argv[i];
            if (0 == szModeArg.CompareNoCase(L"default")) {
                args->signalProcessingMode = AUDIO_SIGNALPROCESSINGMODE_DEFAULT;
            } else if (0 == szModeArg.CompareNoCase(L"raw")) {
                args->signalProcessingMode = AUDIO_SIGNALPROCESSINGMODE_RAW;
            } else if (0 == szModeArg.CompareNoCase(L"none")) {
                args->signalProcessingMode = KSLIB_AUDIOPROCESSINGMODE_NONE;
            } else if (0 == szModeArg.CompareNoCase(L"auto")) {
                args->signalProcessingMode = KSLIB_AUDIOPROCESSINGMODE_AUTO;
            } else {
                HRESULT hr = CLSIDFromString(szModeArg, &args->signalProcessingMode);
                if (FAILED(hr)) {
                    ERR(L"unrecognized signal processing mode");
                    return false;
                }
            }

            continue;
        }

        // -exclusive
        if (0 == szArg.CompareNoCase(L"-Exclusive")) {
            if (api_WASAPI != args->api) {
                ERR(L"-Exclusive only applies to WASAPI");
                return false;
            }

            args->api = api_WASAPIExclusive;

            continue;
        }

        // -timer-driven
        if (0 == szArg.CompareNoCase(L"-Timer-Driven")) {
            if (args->wasapiTimerDriven) {
                ERR(L"-Timer-Driven specified twice");
                return false;
            }
        
            if (api_WASAPI != args->api && api_WASAPIExclusive != args->api) {
                ERR(L"-Timer-Driven only applies to WASAPI or WASAPI exclusive");
                return false;
            }

            args->wasapiTimerDriven = true;

            continue;
        }

        // -loopback
        if (0 == szArg.CompareNoCase(L"-Loopback")) {
            if (args->wasapiLoopback) {
                ERR(L"-Loopback specified twice");
                return false;
            }
        
            if (api_WASAPI != args->api) {
                ERR(L"-Loopback only applies to WASAPI");
                return false;
            }

            args->wasapiLoopback = true;

            continue;
        }

        // -offload
        if (0 == szArg.CompareNoCase(L"-Offload")) {
            if (args->wasapiOffload) {
                ERR(L"-Offload specified twice");
                return false;
            }
        
            if (api_WASAPI != args->api) {
                ERR(L"-Offload only applies to WASAPI");
                return false;
            }

            args->wasapiOffload = true;

            continue;
        }

        // -category
        if (0 == szArg.CompareNoCase(L"-Category")) {
            if (-1 != args->streamCategory) {
                ERR(L"-Category specified twice");
                return false;
            }
        
            if (
                api_WASAPI == args->api ||
                api_WASAPIExclusive == args->api ||
                api_XAudio2 == args->api
            ) {
                // fine
            } else {
                ERR(L"-Category only applies to WASAPI, WASAPI exclusive, or XAudio2");
                return false;
            }

            if (++i >= argc) {
                ERR(L"-category requires an argument");
                return false;
            }

            CString szCategoryArg = argv[i];
            bool found = false;
            
            //find the Audio Category (public)
            for (int i = 0; i < ARRAYSIZE(g_AudioCategories); i++) {
                if (0 == szCategoryArg.CompareNoCase(g_AudioCategories[i].name)) {
                    args->streamCategory = g_AudioCategories[i].value;
                    found = true;
                    break;
                }
            }
            if (!found) {
                // try to parse as a number
                args->streamCategory = (AUDIO_STREAM_CATEGORY)wcstol(szCategoryArg, &pszEnd, 0);
#pragma prefast(suppress: __WARNING_POTENTIAL_BUFFER_OVERFLOW_NULLTERMINATED, "pszEnd is guaranteed to point to something")
                if (0 != errno || NULL == pszEnd || '\0' != *pszEnd) {
                    ERR(L"Could not parse audio category");
                    return false;
                }
                
                if (-1 == args->streamCategory) {
                    ERR(L"Cannot specify a category of -1");
                    return false;
                }
            }           
            continue;
        }            





















































        // -raw
        if (0 == szArg.CompareNoCase(L"-Raw")) {
            if (args->wasapiRaw) {
                ERR(L"-Raw specified twice");
                return false;
            }
        
            if (api_WASAPI != args->api && api_WASAPIExclusive != args->api) {
                ERR(L"-Raw only applies to WASAPI or WASAPI exclusive");
                return false;
            }

            args->wasapiRaw = true;

            continue;
        }

        // -MatchFormat
        if (0 == szArg.CompareNoCase(L"-MatchFormat")) {
            if (args->wasapiMatchFormat) {
                ERR(L"-MatchFormat specified twice");
                return false;
            }

            if (api_WASAPI != args->api) {
                ERR(L"-MatchFormat only applies to shared mode WASAPI streams.");
                return false;
            }

            args->wasapiMatchFormat = true;

            continue;
        }

        // -FallBackToNonRawOffload
        if (0 == szArg.CompareNoCase(L"-FallBackToNonRawOffload")) {
            if (args->wasapiFallBackToNonRawOffload) {
                ERR(L"-FallBackToNonRawOffload specified twice");
                return false;
            }

            args->wasapiFallBackToNonRawOffload = true;

            continue;
        }

        // -WaitIndefinitely
        if (0 == szArg.CompareNoCase(L"-WaitIndefinitely")) {
            if (args->wasapiWaitIndefinitely) {
                ERR(L"-WaitIndefinitely specified twice");
                return false;
            }

            args->wasapiWaitIndefinitely = true;

            continue;
        }
        
        // -format
        if (0 == szArg.CompareNoCase(L"-format")) {
            if (bSeenFormat) {
                ERR(L"multiple -format options specified");
            }
            
            if (!ParseWaveFormat(args->flow, args->endpoint, &i, argc, argv, &args->wfx)) {
                // ParseWaveFormat does its own logging
                return false;
            }
            
            bSeenFormat = true;
            continue;
        }
        
        // -dither
        if (0 == szArg.CompareNoCase(L"-dither")) {
            if (ArgStruct::sine != args->what) {
                ERR(L"-dither only applies to sine waves");
                return false;
            }

            if (Method_NoDithering != args->sindither) {
                ERR(L"-dither specified twice");
                return false;
            }

            args->sindither = Method_TPD;

            continue;
        }
        
        // -performance
        if (0 == szArg.CompareNoCase(L"-performance")) {
            if (api_XAudio2 != args->api) {
                ERR(L"-performance only applies to XAudio2");
                return false;
            }

            if (args->queryXAudio2Performance) {
                ERR(L"Specify -performance only once");
                return false;
            }
            
            args->queryXAudio2Performance = true;
            
            continue;
        }

        // unexpected argument
        ERR(L"Unexpected argument %s", argv[i]);
        return false;
    } // for i

    // Everything made sense individually.  Put it all together.
    if( bKsOpt_Notification && !bKsOpt_Algorithm )
    {
        ERR(L"-Notifications was specified but -Algorithm was not");
        return false;
    }

    if( bKsOpt_Notification && bKsOpt_Algorithm )
    {
        switch( args->streamMethod )
        {
            case eRealTimeStream:
                args->streamMethod = eRealTimeWithNotificationStream;

                break;
            case eEnhancedRealTimeStream:
                args->streamMethod = eEnhancedRealTimeWithNotificationStream;

                break;
            default:
                ERR(L"-Notification is only valid with WaveRT or Enhanced-RT Algorithm");
                return false;
        }
    }
    
    if (args->wasapiLoopback && !args->wasapiTimerDriven) {
        WARN(L"WASAPI loopback may require timer-driven mode");
        // continue anyway
    }

    if (
        args->wasapiFallBackToNonRawOffload && !(
            api_WASAPI == args->api && args->wasapiRaw && args->wasapiOffload
        )
    ) {
        ERR(L"-FallBackToNonRawOffload only applies to WASAPI raw offload");
        return false;
    }
    
    if (args->wasapiWaitIndefinitely && api_WASAPI != args->api && api_WASAPIExclusive != args->api) {
        ERR(L"-WaitIndefinitely only applies to WASAPI or WASAPI exclusive");
        return false;
    }

    if (!bSeenAPI) {
        ERR(L"-api was not specified.");
        return false;                
    }

    if (!bSeenAction) {
        ERR(L"None of -play-sine, -play-file, or -record were specified.");
        return false;                
    }

    // playing a sine wave
    if (eRender == args->flow && ArgStruct::sine == args->what) {
        if (bSeenFile) {
            ERR(L"-file incompatible with -play-sine.");
            return false; 
        }

        // time can be specified, or not.
        return true;
    }
    
    // playing a file
    if (eRender == args->flow && ArgStruct::file == args->what) {
        if (bSeenFormat) {
            ERR(L"-play-file incompatible with -format");
            return false;
        }
    
        // time can be specified, or not
        return true;
    }

    // recording
    if (eCapture == args->flow) {
        // time can be specified, or not
        return true;
    }

    ERR(L"Unexpected combination of arguments.");
    return false;
}

void PrintUsage(LPCWSTR progname) {
    wprintf(
        L"%s -devices\n"
        L"    prints all active devices and their endpoint IDs\n"
#if NTDDI_VERSION >= NTDDI_WIN10
        L"%s -periods\n"
        L"    prints period information for active devices\n"
#endif
        L"%s\n"
        L"    -api api-args\n"
        L"    [ -device [ console | communications | <endpoint-id> ] ]\n"
        L"    [\n"
        L"        -play-sine <frequency> <amplitude> [ -dither ]\n"
        L"            [ -sweep <end-frequency> <seconds> [ -repeat ] [ -linear ] ]\n"
        L"            [ -time <milliseconds> ]\n"
        L"            [ -format <format-args> ]\n"
        L"        |\n"
        L"        -play-file <filename>\n"
        L"            [ -time <milliseconds> ]\n"
        L"        |\n"
        L"        -record\n"
        L"            [ -time <milliseconds> ] [ -file <filename> ]\n"
        L"            [ -format <format-args> ]\n"
        L"    ]\n"
        L"\n"
        L"api-args\n"
        L"    DSound | MF | WinMM\n"
        L"    XAudio2 [ -Category <audio-category> ] [ -performance ]\n"
        L"    WASAPI\n"
        L"        [ -Exclusive ] [ -Timer-Driven ] [ -Loopback ] [ -Offload ]\n"
        L"            [ -Category <audio-category>"



        L" ]\n"
        L"            [ -FallBackToNonRawOffload ]\n"
        L"            [ -WaitIndefinitely ]\n"
        L"            [ -Raw ] [ -Buffer [ <milliseconds> | \"Max\" | \"Min\" ] ]\n"
#if (NTDDI_VERSION >= NTDDI_WIN10)
        L"            [ -Periodicity [ <frames> | \"Min\" ] ]\n"
        L"            [ -MatchFormat ]\n"
#endif
        L"    KS\n"
        L"        [ -Pin <pin-factory-id> ] [ -Mode [ default | raw | none | auto | <guid> ] ]\n"
        L"        [ -Buffer [ <milliseconds> | \"Max\" | \"Min\" ] ]\n"
        L"        [ -Algorithm <ks-algorithm> [ -Notifications ] ]\n"
        L"\n"
        L"format-args\n"
        L"    software-mix | offload-mix | device |\n"
        L"    ex [ int | float ] <channels> <sample-rate> <bits> |\n"
        L"    ext [ int | float ] <channels> <sample-rate> <bits> <valid> <mask> |\n"
        L"    raw <hex-bytes>\n"
        L"\n"
        L"ks-algorithms\n"
        L"    Standard | Standard-Looped | WaveRT | Enhanced-WaveRT\n"
        L"\n"
        L"audio-categories\n"
        L"    Other | Movie | Media | ... | <number>\n"
        L"   (See the AUDIO_STREAM_CATEGORY enumeration in audiosessiontypes.h)\n"





        L"endpoint-id can be:\n"
        L"    a WASAPI endpoint ID\n"
        L"     a PKEY_AudioEndpoint_GUID\n"
        L"     an endpoint name\n"
        L"play-sine\n"
        L"    plays a sine tone of the given frequency (in Hz: say, 440.0)\n"
        L"    and amplitude - 0.0 (silent) to 1.0 (full scale)\n"
        L"    if a time argument is not given, plays until user hits Enter\n"
        L"play-file\n"
        L"    plays a given .wav file until the file runs out\n"
        L"    if a time argument is given, play, looping, until time runs out\n"
        L"    WARNING: slurps the WHOLE data chunk into memory, by design\n"
        L"record\n"
        L"    if a file is not given, drops data on the floor\n"
        L"    WARNING: -time requires -file (synchronous null capture not implemented)\n"
        ,
        progname,
#if NTDDI_VERSION >= NTDDI_WIN10
        progname,
#endif
        progname
    );
}

int __cdecl wmain(int argc, __ecount(argc) LPCWSTR argv[]) {

    HRESULT hr = S_OK;
    
    CComPtr<IBasicLog> pLog;
    hr = CreateStdOutBasicLog(&pLog);
    if (FAILED(hr)) {
        printf("CreateStdOutBasicLog failed: hr = 0x%08x\n", hr);
        return -__LINE__;
    }

    g_pLog = pLog; // pLog will ->Release() on main() exit, orphaning g_pLog, but that's OK

    CCoInitialize ci(0, COINIT_MULTITHREADED, hr);
    if (FAILED(hr)) {
        ERR(L"CoinitializeEx: 0x%08x\n", hr);
        return -__LINE__;
    }

    ArgStruct options;
    if (!ParseArgs(argc, argv, &options)) {
        return -__LINE__;
    }

    CApp app(options);

    switch (options.flow) {
        case eRender:
            return app.RunRenderMode();

        case eCapture:
            return app.RunCaptureMode();
    }

    ERR(L"unexpected flow: %u", options.flow);
    return -__LINE__;
}

void CApp::WaitForEnterOrEvent(IUnknown *pStream, HANDLE hEvent) {
    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    if (NULL == hStdIn || INVALID_HANDLE_VALUE == hStdIn) {
        DWORD dwErr = GetLastError();
        ERR(L"GetStdHandle failed - hStdIn = 0x%p, GetLastError() = %u", hStdIn, dwErr);
        return;
    }

    // see if we need to work around Windows Blue Bugs 518047
    // in Core SKUs (e.g., Mobile Core)
    // the STDIN handle is never signaled (so we need to poll)
    // and GetNumberOfConsoleInputEvents always fails with ERROR_INVALID_FUNCTION
    DWORD dwDummy;
    g_fWorkaroundWindowsBlueBugs518047 =
        !GetNumberOfConsoleInputEvents(hStdIn, &dwDummy) &&
        ERROR_INVALID_FUNCTION == GetLastError();

    bool loggedPerformance = false;
    ULONGLONG start = GetTickCount64();
    
    LOG(L"Press <ENTER> to stop...");

    HANDLE rhWaitFor[] = { hStdIn, hEvent };

    for (;;) {
        bool wakeQuickly = 
            g_fWorkaroundWindowsBlueBugs518047 || // need to poll to see if Enter was pressed
            (options.queryXAudio2Performance && !loggedPerformance);
    
        DWORD dwWaitResult = WaitForMultipleObjectsEx(
            hEvent ? 2 : 1,
            rhWaitFor,
            FALSE, // any
            wakeQuickly ? 200 : INFINITE,
            FALSE
        );
        
        switch (dwWaitResult) {
            case WAIT_OBJECT_0:
            case WAIT_TIMEOUT:
                {
                    bool bDone;
                    HRESULT hr = WasEnterPressed(hStdIn, bDone);
                    if (FAILED(hr)) {
                        return;
                    }
                    
                    if (bDone) {
                        return;
                    }

                    if (
                        options.queryXAudio2Performance &&
                        !loggedPerformance &&
                        GetTickCount() - start > 2000
                    ) {
                        LogXAudio2Performance(pStream);
                        loggedPerformance = true;
                    }
                }
                break;
                
            case WAIT_OBJECT_0 + 1:
                // stream ended of its own accord
                return;
                
            default:
                {
                    DWORD dwErr = GetLastError();
                    ERR(L"Unexpected return value 0x%x from WaitForMultipleObjectsEx: GetLastError() = %u", dwWaitResult, dwErr);
                    return;
                }
        }
    }
}

HRESULT WasEnterPressed(HANDLE hStdIn, bool &bEnterPressed) {
    bEnterPressed = false;

    for (;;) {
        DWORD nEvents = 0;
    
        // how many events are in the queue?
        if (g_fWorkaroundWindowsBlueBugs518047) {
            INPUT_RECORD rEvents[128] = {};
            if (!PeekConsoleInput(hStdIn, &rEvents[0], ARRAYSIZE(rEvents), &nEvents)) {
                DWORD dwErr = GetLastError();
                ERR(L"PeekConsoleInput failed: last error is %u", dwErr);
                return HRESULT_FROM_WIN32(dwErr);
            }
        } else if (!GetNumberOfConsoleInputEvents(hStdIn, &nEvents)) {
            DWORD dwErr = GetLastError();
            
            if (ERROR_INVALID_FUNCTION == dwErr) {
            } else {
                ERR(L"GetNumberOfConsoleInputEvents failed: last error is %u", dwErr);
                return HRESULT_FROM_WIN32(dwErr);
            }
        }

        // LOG(L"Number of console input events is: %u", nEvents);
    
        if (0 == nEvents) {
            // we're done
            return S_OK;
        }
    
        CComHeapPtr<INPUT_RECORD> pEvents;
        DWORD nRead;
        if (!pEvents.Allocate(nEvents)) {
            return E_OUTOFMEMORY;
        }
        
        if (!ReadConsoleInput(hStdIn, pEvents, nEvents, &nRead)) {
            DWORD dwErr = GetLastError();
            ERR(L"ReadConsoleInput failed: last error is %u", dwErr);
            return HRESULT_FROM_WIN32(dwErr);
        }

        if (nRead != nEvents) {
            ERR(L"Events expected: %u; events read: %u", nEvents, nRead);
            return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
        }
        
        for (DWORD i = 0; i < nEvents; i++) {
            if (
                KEY_EVENT == pEvents[i].EventType &&
                VK_RETURN == pEvents[i].Event.KeyEvent.wVirtualKeyCode &&
                pEvents[i].Event.KeyEvent.bKeyDown
                // we used to look for an Enter key-up event
                // but Core System populates the buffer with a spurious Enter key-up event
                // so now we look for Enter key-down
            ) {
                bEnterPressed = true;
                if (!FlushConsoleInputBuffer(hStdIn)) {
                    DWORD dwErr = GetLastError();
                    ERR(L"FlushConsoleInput failed: last error is %u", dwErr);
                    return HRESULT_FROM_WIN32(dwErr);
                }
            }
        }
    }

    return S_OK;
}

#if USE_XAUDIO2
void LogXAudio2Performance(IUnknown *pStream) {
    CComPtr<IAudioStreamingControl> pAudioStreamingControl;
    HRESULT hr = pStream->QueryInterface(IID_PPV_ARGS(&pAudioStreamingControl));
    if (FAILED(hr)) {
        ERR(L"IUnknown::QueryInterface(IAudioStreamingControl) failed: 0x%08x", hr);
        return;
    }
    
    CComPtr<IXAudio2> pXAudio2;
    hr = pAudioStreamingControl->GetService(IID_PPV_ARGS(&pXAudio2));
    if (FAILED(hr)) {
        ERR(L"IAudioStreamingControl::QueryInterface(IXAudio2) failed: 0x%08x", hr);
        return;
    }
    
    XAUDIO2_PERFORMANCE_DATA data = {};
    pXAudio2->GetPerformanceData(&data);
    
    LOG(
        L"AudioCyclesSinceLastQuery: %llu\n"
        L"TotalCyclesSinceLastQuery: %llu\n"
        L"MinimumCyclesPerQuantum: %u\n"
        L"MaximumCyclesPerQuantum: %u\n"
        L"MemoryUsageInBytes: %u\n"
        L"CurrentLatencyInSamples: %u\n"
        L"GlitchesSinceEngineStarted: %u\n"
        L"ActiveSourceVoiceCount: %u\n"
        L"TotalSourceVoiceCount: %u\n"
        L"ActiveSubmixVoiceCount: %u\n"
        L"ActiveResamplerCount: %u\n"
        L"ActiveMatrixMixCount: %u\n"
        L"ActiveXmaSourceVoices: %u\n"
        L"ActiveXmaStreams: %u\n",
        data.AudioCyclesSinceLastQuery,
        data.TotalCyclesSinceLastQuery,
        data.MinimumCyclesPerQuantum,
        data.MaximumCyclesPerQuantum,
        data.MemoryUsageInBytes,
        data.CurrentLatencyInSamples,
        data.GlitchesSinceEngineStarted,
        data.ActiveSourceVoiceCount,
        data.TotalSourceVoiceCount,
        data.ActiveSubmixVoiceCount,
        data.ActiveResamplerCount,
        data.ActiveMatrixMixCount,
        data.ActiveXmaSourceVoices,
        data.ActiveXmaStreams
    );
}
#else
void LogXAudio2Performance(IUnknown *) {}
#endif

void ListDevices(PeriodicityAction action) {
    CComPtr<IMMDeviceEnumerator> pEnumerator;
    HRESULT hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
    if (FAILED(hr)) {
        ERR(L"CoCreateInstance(MMDeviceEnumerator) failed: hr = 0x%08x", hr);
        return;
    }

    static_assert((UINT)eRender + 1 == (UINT)eCapture, "iterating over EDataFlow");
    for (UINT f = (UINT)eRender; f <= (UINT)eCapture; f++) {
        EDataFlow flow = (EDataFlow)f;
        LPCWSTR szFlow = (flow == eRender ? L"Render" : L"Capture");
        
        CComPtr<IMMDeviceCollection> pMMDeviceCollection;
        hr = pEnumerator->EnumAudioEndpoints(flow, DEVICE_STATE_ACTIVE, &pMMDeviceCollection);
        if (FAILED(hr)) {
            ERR(L"IMMDeviceEnumerator::EnumAudioEndpoints(%s) failed: hr = 0x%08x", szFlow, hr);
            continue;
        }
        
        UINT nCount = 0;
        hr = pMMDeviceCollection->GetCount(&nCount);
        if (FAILED(hr)) {
            ERR(L"IMMDeviceCollection(%s)::GetCount failed: hr = 0x%08x", szFlow, hr);
            continue;
        }
        
        LOG(L"-- %s devices: %u --", szFlow, nCount);
        for (UINT i = 0; i < nCount; i++) {
            CComPtr<IMMDevice> pMMDevice;
            hr = pMMDeviceCollection->Item(i, &pMMDevice);
            if (FAILED(hr)) {
                ERR(L"IMMDeviceCollection(%s)::Item(%u) failed: hr = 0x%08x", szFlow, i, hr);
                continue;
            }
            
            CComHeapPtr<WCHAR> szId;
            hr = pMMDevice->GetId(&szId);
            if (FAILED(hr)) {
                ERR(L"IMMDevice(%s, %u)::GetId failed: hr = 0x%08x", szFlow, i, hr);
                continue;
            }
            
            CComPtr<IPropertyStore> pPropertyStore;
            hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
            if (FAILED(hr)) {
                ERR(L"IMMDevice(%s, %u)::OpenPropertyStore failed: hr = 0x%08x", szFlow, i, hr);
                continue;
            }
            
            PROPVARIANT var; PropVariantInit(&var);
            DoOnExit<PROPVARIANT*> doeClearPropVariant(
                PropVariantClear, Success_NonNegative,
                &var, g_pLog, L"Clearing PropVariant"
            );
            hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &var);
            if (FAILED(hr)) {
                ERR(L"IPropertyStore(%s, %u)::GetValue(PKEY_Device_FriendlyName) failed: hr = 0x%08x", szFlow, i, hr);
                continue;
            }
            
            if (VT_LPWSTR != var.vt) {
                ERR(L"IPropertyStore(%s, %u)::GetValue(DEVPKEY_Device_FriendlyName) has unexpected vartype 0x%x", szFlow, i, var.vt);
                continue;
            }
            
            LOG(L"%s %s", static_cast<LPCWSTR>(szId), var.pwszVal);

            if (action == PeriodicityAction_ListNone) { continue;  }

#if (NTDDI_VERSION >= NTDDI_WIN10) 
            CComPtr<IAudioClient3> spAudioClient3;

            hr = pMMDevice->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, nullptr, (void**)&spAudioClient3);
            if (FAILED(hr)) {
                ERR(L"IMMDevice::Activate(IAudioClient3) failed: hr = 0x%08x", hr);
                continue;
            }

            CComHeapPtr<WAVEFORMATEX> spMixFormat;
            AudioClientProperties acProps = {};
            acProps.cbSize = sizeof(acProps);
            acProps.bIsOffload = FALSE;
            acProps.Options = AUDCLNT_STREAMOPTIONS_RAW;
            hr = spAudioClient3->SetClientProperties(&acProps);
            if (FAILED(hr)) {
                ERR(L"IAudioClient3::SetClientProperties() failed: hr = 0x%08x", hr);
                continue;
            }

            hr = spAudioClient3->GetMixFormat(&spMixFormat);
            if (FAILED(hr)) {
                ERR(L"IAudioClient3::GetMixFormat() failed: hr = 0x%08x", hr);
                continue;
            }

            LOG(L" Sample rate in frames/sec; block align in bytes/frame; periods in frames");

            UINT32 DefaultPeriodicityInFrames, FundamentalPeriodicityInFrames, MinPeriodicityInFrames, MaxPeriodicityInFrames;
            hr = spAudioClient3->GetSharedModeEnginePeriod(spMixFormat, &DefaultPeriodicityInFrames, &FundamentalPeriodicityInFrames, &MinPeriodicityInFrames, &MaxPeriodicityInFrames);
            if (FAILED(hr)) {
                ERR(L"IAudioClient3::GetSharedModeEnginePeriod() failed: hr = 0x%08x", hr);
                continue;
            }















            if (action == PeriodicityAction_List)
            {
                LOG(L" Sample rate Channels Fundamental Minimum Default Maximum Category");
                LOG(L" ----------- -------- ----------- ------- ------- ------- --------");
                LOG(L" %       11u %     8u %       11u %    7u %    7u %    7u Any/RAW",
                    spMixFormat->nSamplesPerSec,
                    spMixFormat->nChannels,
                    FundamentalPeriodicityInFrames,
                    MinPeriodicityInFrames,
                    DefaultPeriodicityInFrames,
                    MaxPeriodicityInFrames);

            }

















            for (int c = 0; c < ARRAYSIZE(g_AudioCategories); c++)
            {
                spAudioClient3.Release();

                hr = pMMDevice->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, nullptr, (void**)&spAudioClient3);
                if (FAILED(hr)) {
                    ERR(L"IMMDevice::Activate(IAudioClient3) failed (Are you running on an OS prior to Windows 10?): hr = 0x%08x", hr);
                    continue;
                }

                CComHeapPtr<WAVEFORMATEX> spMixFormat;
                AudioClientProperties acProps = {};
                acProps.bIsOffload = FALSE;
                acProps.eCategory = g_AudioCategories[c].value;
                acProps.cbSize = sizeof(acProps);

                hr = spAudioClient3->SetClientProperties(&acProps);
                if (FAILED(hr)) {
                    ERR(L"IAudioClient3::SetClientProperties() failed: hr = 0x%08x", hr);
                    continue;
                }

                hr = spAudioClient3->GetMixFormat(&spMixFormat);
                if (FAILED(hr)) {
                    ERR(L"IAudioClient3::GetMixFormat() failed: hr = 0x%08x", hr);
                    continue;
                }

                UINT32 DefaultPeriodicityInFrames, FundamentalPeriodicityInFrames, MinPeriodicityInFrames, MaxPeriodicityInFrames;
                hr = spAudioClient3->GetSharedModeEnginePeriod(spMixFormat, &DefaultPeriodicityInFrames, &FundamentalPeriodicityInFrames, &MinPeriodicityInFrames, &MaxPeriodicityInFrames);
                if (FAILED(hr)) {
                    ERR(L"IAudioClient3::GetSharedModeEnginePeriod() failed: hr = 0x%08x", hr);
                    continue;
                }
                    














                if (action == PeriodicityAction_List)
                {
                    //     Sample rate Channels Fundamental Minimum Default Maximum Category
                    //     ----------- -------- ----------- ------- ------- ------- --------
                    LOG(L" %       11u %     8u %       11u %    7u %    7u %    7u %s",
                        spMixFormat->nSamplesPerSec,
                        spMixFormat->nChannels,
                        FundamentalPeriodicityInFrames,
                        MinPeriodicityInFrames,
                        DefaultPeriodicityInFrames,
                        MaxPeriodicityInFrames,
                        g_AudioCategories[c].name);
                }

















            } // for each category
#endif // NTDDI_VERSION >= NTDDI_WIN10
        } // for each device
    } // for render and capture
} // ListDevices
