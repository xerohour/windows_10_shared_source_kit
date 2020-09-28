// Copyright (C) Microsoft. All rights reserved.

#include "Common.h"
#include <initguid.h>
#include <MMDeviceAPIP.h>
#include <DeviceTopology.h>
#include <DeviceTopologyP.h>
#include <AudioStreaming.h>
#include <BasicLogHelper.h>
#include <MMSystem.h>
#include <DevTrans.h>
#include <HRString.h>
#include <audiooffload.h>

#pragma warning (push, 3)
#include <ks.h>
#pragma warning (pop)
// we are using the old header
// so we need ksmedia.h
#include <ksmedia.h>
#include <devioctl.h>
#include "AudioGlitchTracer.h"


//
//  The number of streams specified by HCK requirements
//  to stream simultaneously in shared render mode without
//  inducing any audio glitch ETW events.
//
const static UINT32 REQUIRED_SIMUL_GF_RENDER_STREAMS = 12;
const static UINT32 REQUIRED_SIMUL_GF_CAPTURE_STREAMS = 5;
const static UINT32 REQUIRED_STRESS_GF_RENDER_STREAMS = 3;
const static UINT32 REQUIRED_STRESS_GF_CAPTURE_STREAMS = 2;

const static UINT32 TEST_DURATION_MILLIS = 10000;

const UINT32 MAX_TRACENAME_LENGTH = 80;

const static UINT32 WIN_8_MINOR_VER =  2;
const static UINT32 WIN_BLUE_MINOR_VER =  3;
//--------------------------------------------------------------------------;
//
//  HRESULT Run100CPUStress
//
//  Description:
//      Create the RWS Process for stressing the CPU
//
//  Arguments:
//      UINT msDuration: Duration to run the tool for
//      PROCESS_INFORMATION* pProcInfo: Pointer to Process info 
//
//  Return (HRESULT):
//      S_OK if pass, appropriate HRESTULT otherwise.
//
//
//--------------------------------------------------------------------------;
HRESULT Run100CPUStress(UINT msDuration, PROCESS_INFORMATION* pProcInfo)
{
    if (NULL == pProcInfo)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;
    STARTUPINFO si = {0};

    si.cb = sizeof(si);

    WCHAR wszCommandLine[MAX_PATH];

    hr = StringCchPrintf(wszCommandLine, MAX_PATH, L"rws.exe -i \"CPU100All.xml\" -p CPU_pri7_100percent -h %u", msDuration);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError,L"FAIL: Building RWS String(%s).", GetHRString(hr));
        return hr;
    }
    
    BOOL bOK = CreateProcessW(
            L"rws.exe", // app name
            wszCommandLine, // command line
            NULL, // process security attributes
            NULL, // thread security attributes
            FALSE, // inherit handles
            CREATE_NEW_CONSOLE, // create-process flags
            NULL, // no environment changes
            NULL, // same working directory
            &si,
            pProcInfo);

    if (FALSE == bOK) 
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        XLOG(XFAIL, eError,L"FAIL: CreateProcessW failed (%s).", GetHRString(hr));
        return hr;
    }

    return hr;
}

//--------------------------------------------------------------------------;
//
//  HRESULT VerifyStressCompleted
//
//  Description:
//      Verify that the process shutdown properly
//
//  Arguments:
//      UINT msDuration: Duration to run the tool for
//      PROCESS_INFORMATION* pProcInfo: Pointer to Process info 
//
//  Return (HRESULT):
//      S_OK if pass, appropriate HRESTULT otherwise.
//
//
//--------------------------------------------------------------------------;
HRESULT VerifyStressCompleted(UINT msDuration, PROCESS_INFORMATION* pProcInfo)
{
    if (NULL == pProcInfo)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;

    if (NULL != pProcInfo->hProcess)
    {
        XLOG(XMSG, LogLevel_Info, L"Waiting for process to end.");

        //wait 3x Duration + 2 minutes
        DWORD retWait = WaitForSingleObject(pProcInfo->hProcess, 3 * msDuration + 120 * 1000);
        if (WAIT_OBJECT_0 != retWait)
        {
            XLOG(XFAIL, eError, L"WaitForSingleObject returned %u while waiting for process to finish; "
                L"GetLastError() == %u", retWait, GetLastError());
            if(FALSE == TerminateProcess(pProcInfo->hProcess, 0))
            {
                XLOG(XFAIL, eError, L"Could not terminate process (%s)", 
                    GetHRString(HRESULT_FROM_WIN32(GetLastError())));
            }
            return E_UNEXPECTED;
        }

        DWORD dwExitCode = 0;
        BOOL bOk = GetExitCodeProcess(pProcInfo->hProcess, &dwExitCode);
        if (FALSE == bOk)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            XLOG(XFAIL, eError, L"Could not get process exit code (%s)", GetHRString(hr));
            return hr;
        }

        if (0 != dwExitCode)
        {
            XLOG(XFAIL, eError, L"Process returned error code (%d)", dwExitCode);
        }
    }

    return hr;

}

//--------------------------------------------------------------------------;
//
//  DWORD RunRenderScenario
//
//  Description:
//      Run the glitch scenario for render endpoints.
//
//  Arguments:
//      const UINT nTotalStreams: Stream count to use for testing
//      bool bCPULoaded: Turn on RWS to stress CPU for testing
//      unit nMinVersionForCheck: min version in OS check to determine if test should run
//
//  Return (DWORD):
//      Unused.  Failures propagated through XLOG.
//
//
//--------------------------------------------------------------------------;
DWORD RunRenderScenario(const UINT nTotalStreams, bool bCPULoaded, UINT nMinVersionForCheck)
{
    CAudioLogoDevice                    *pCurrentDevice = NULL;
    IMMDevice                           *pEndpoint = NULL;
    HRESULT                              hr = S_OK;
    CComPtr<IBasicLog>                   spLogger;
    CSimpleArray<IAudioStreamingRender*> aspRenderEngines;
    PROCESS_INFORMATION                  pi = {0};
    CComHeapPtr<WCHAR>                   pwszTraceName;
    bool                                 audioGlitchTracingInitialized = false;

    if (S_OK != (hr = g_IShell->GetBasicLog(&spLogger)))
    {
        XLOG(XFAIL, eError,L"FAIL: GetBasicLog returned error (%s).",GetHRString(hr));
        return (FNS_PASS);
    }

    //
    // This test only applies to Windows 8 and above
    //
    OSVERSIONINFO ver = {0};
    ver.dwOSVersionInfoSize = sizeof(ver);
    if (!GetVersionEx(&ver))
    {
        XLOG(XFAIL, eFatalError, L"FAIL: GetVersionEx() failed: GetLastError() = %u", GetLastError());
        return (FNS_PASS);
    }

    XLOG(XMSG, eInfo1, L"INFO: OS version is %u.%u", ver.dwMajorVersion, ver.dwMinorVersion);

    if (ver.dwMajorVersion < 6 || (ver.dwMajorVersion == 6 && ver.dwMinorVersion < nMinVersionForCheck))
    {
        XLOG(XSKIP, eInfo1, L"INFO: This test case is only applicable to Windows versions 6.%d and above.", nMinVersionForCheck);
        return (FNS_PASS);
    }

    // tell the user what we're doing
    XLOG(XMSG, eWarn1, L"<<Preparing to render %d simultaneous streams and check for audio glitches>>", 
        nTotalStreams);
    // get the endpoint under test
    if (NULL == (pCurrentDevice = g_pAudioLogoDeviceTestModule->GetDevice()))
    {
        XLOG(XFAIL, eError, L"FAIL: GetDevice encountered error.");
        return (FNS_PASS);
    }

    if (NULL == (pEndpoint = pCurrentDevice->GetEndpoint()))
    {
        XLOG(XFAIL, eError, L"FAIL: GetEndpoint encountered error.");
        return (FNS_PASS);
    }

    // check whether the endpoint supports raw mode
    // if it does, we know we're on Blue or higher
    bool rawModeSupported = false;
    if (S_OK != (hr = AudioOffload::DoesDeviceSupportRawMode(spLogger, pEndpoint, rawModeSupported)))
    {
        XLOG(XFAIL, eError,L"FAIL: AudioOffload::DoesDeviceSupportRawMode returned failure (%s).", GetHRString(hr));
        return (FNS_PASS);
    }

    // initialize the renderers
    for (UINT32 i=0; i<nTotalStreams; i++)
    {
        IAudioStreamingRender* pCurrRenderer;
        hr = CreateAudioStreamingRender(api_WASAPI,
            pEndpoint,
            spLogger,
            &(pCurrRenderer));
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, L"FAIL: CreateAudioStreamingRender returned error (%s) for stream %d.",
                GetHRString(hr), i);
            goto CLEANUP;
        }

        if (! aspRenderEngines.Add(pCurrRenderer))
        {
            XLOG(XFAIL, eError, L"FAIL: adding renderer to array for stream %d.", i);
            pCurrRenderer->Release();
            goto CLEANUP;
        }
        
        // if the endpoint supports raw, make every other stream raw
        if (rawModeSupported && (i % 2 == 1))
        {
            CComPtr<IAudioStreamingControl> spAudioStreamingControl;
            CComPtr<IAudioStreamingWasapiControl> spAudioStreamingWasapiControl;
        
            struct AudioClientPropertiesBlue
            {
                UINT32                  cbSize;
                BOOL                    bIsOffload;
                AUDIO_STREAM_CATEGORY   eCategory;
                AUDCLNT_STREAMOPTIONS   Options;
            };
            
            AudioClientPropertiesBlue raw = {};
            raw.cbSize = sizeof(raw);
            raw.bIsOffload = FALSE;
            raw.eCategory = AudioCategory_Other;
            raw.Options = AUDCLNT_STREAMOPTIONS_RAW;
            
            if (S_OK != (hr = pCurrRenderer->QueryInterface(
                    __uuidof(IAudioStreamingControl),
                    reinterpret_cast<void**>(&spAudioStreamingControl)
            )))
            {
                XLOG(XFAIL, eError, L"FAIL: IAudioStreamingRender::QueryInterface(IAudioStreamingControl) returned error %s (0x%08x)", GetHRString(hr), hr);
                goto CLEANUP;
            }
            
            if (S_OK != (hr = spAudioStreamingControl->GetService(
                    __uuidof(IAudioStreamingWasapiControl),
                    reinterpret_cast<void**>(&spAudioStreamingWasapiControl)
            )))
            {
                XLOG(XFAIL, eError, L"FAIL: IAudioStreamingControl::GetService(IAudioStreamingWasapiControl) returned error %s (0x%08x)", GetHRString(hr), hr);
                goto CLEANUP;
            }
            
            if (S_OK != (hr = spAudioStreamingWasapiControl->SetClientProperties(
                reinterpret_cast<AudioClientProperties *>(&raw)
            )))
            {
                XLOG(XFAIL, eError, L"FAIL: IAudioStreamingWasapiControl::SetClientProperties returned error %s (0x%08x)", GetHRString(hr), hr);
                goto CLEANUP;
            }
        }
    }

    for (UINT32 i=0; i<nTotalStreams; i++)
    {
        if (S_OK != (hr = aspRenderEngines[i]->Initialize(NULL)))
        {
            XLOG(XFAIL, eError, L"FAIL: IAudioStreamingRender::Initialize returned error (%s).",GetHRString(hr));
            goto CLEANUP;
        }
    }

    hr = AudioGlitchTracer::Initialize(spLogger);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Initializing Tracer Library failed (%s).", GetHRString(hr));
        goto CLEANUP;
    }
    audioGlitchTracingInitialized = true;

    hr = AudioGlitchTracer::GenerateTraceName(&pwszTraceName);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Generating Trace Name failed (%s).", GetHRString(hr));
        goto CLEANUP;
    }

    if (bCPULoaded)
    {
        //turn on cpu load tool
        hr = Run100CPUStress(TEST_DURATION_MILLIS * 2, &pi);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, L"FAIL: Error starting CPU Stress (%s).", GetHRString(hr));
            goto CLEANUP;
        }
    }

    // start capturing ETW trace info
    hr = AudioGlitchTracer::StartETW(pwszTraceName);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Error starting ETW tracing (%s).", GetHRString(hr));
        goto CLEANUP;
    }

    // start the streams
    // Render a series of sine tones ranging from 230 Hz to (230 + 100 num streams) Hz
    for (UINT32 i=0; i<nTotalStreams;i++)
    {
        float frequency = 230.0f + (i * 100.0f);
        if (S_OK != (hr = aspRenderEngines[i]->PlaySineWaveAsync(frequency, TEST_AMPLITUDE, Method_NoDithering)))
        {
            XLOG(XFAIL, eError, L"FAIL: IAudioStreamingRender::PlaySineWaveAsync returned error (%s).",
                GetHRString(hr));
            // don't short circuit
        }
    }

    // stream for roughly TEST_DURATION_MILLIS ms
    Sleep(TEST_DURATION_MILLIS);

    // stop streaming
    for (UINT32 i=0; i<nTotalStreams;i++)
    {
        if (S_OK != (hr = aspRenderEngines[i]->Stop()))
        {
            XLOG(XFAIL, eError, L"FAIL: IAudioStreamingRender::Stop returned error (%s).", GetHRString(hr));
            // don't short circuit
        }
    }

    // Stop capturing ETW trace info
    hr = AudioGlitchTracer::StopETW(pwszTraceName);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Error stopping ETW tracing (%s).", GetHRString(hr));
        goto CLEANUP;
    }

    if (bCPULoaded)
    {
        //verify cpu stress ended
        hr = VerifyStressCompleted(TEST_DURATION_MILLIS * 2, &pi);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, L"FAIL: Error Verifying CPU Stress Terminated(%s).", GetHRString(hr));
            goto CLEANUP;
        }
    }

    UINT64 nGlitchEvents = 0;

    // analyze the trace info to determine the number of glitches
    hr = AudioGlitchTracer::ProcessETWTrace(pwszTraceName, &nGlitchEvents);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Error analyzing ETL file (%s)", GetHRString(hr));
        goto CLEANUP;
    }

    XLOG(XMSG, eInfo1, L">>>> Trace contains %I64u audio glitches <<<<", nGlitchEvents);

    if (0 == nGlitchEvents)
    {
        XLOG(XMSG, eInfo1, L"[PASS]");

        // delete the etw logs
        hr = AudioGlitchTracer::DeleteETWTraceFiles(pwszTraceName);
        if (FAILED(hr))
        {
            // log a warning but don't fail the test
            XLOG(XWARN, eError, L"WARNING: Error deleting ETL files for trace\"%s\" (%s)",
                static_cast<LPCWSTR>(pwszTraceName), GetHRString(hr));
        }
    }
    else
    {
        // Display an error message to the user explaining the failure.
        // Display the name of the trace file that was generated for potential analysis.  Note that the kit job(s)
        // associated with this content will need to copy the trace on failure
        XLOG(XFAIL, eInfo1, L"FAIL: No audio glitches are allowed!");
        XLOG(XFAIL, eInfo1, L"FAIL: ETW trace information for this test can be found in %s_merged.etl", static_cast<LPCWSTR>(pwszTraceName));
    }

CLEANUP:

    if (audioGlitchTracingInitialized)
    {
        hr = AudioGlitchTracer::Cleanup();
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, L"FAIL: Cleaning up Tracer Lib failed (%s)", GetHRString(hr));
        }
    }

    for (INT i = 0; i < aspRenderEngines.GetSize(); i++)
    {
        if(nullptr != aspRenderEngines[i])
        {
            (aspRenderEngines[i])->Release();
        }
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return (FNS_PASS);
}

//--------------------------------------------------------------------------;
//
//  DWORD RunCaptureScenario
//
//  Description:
//      Run the glitch scenario for capture endpoints.
//
//  Arguments:
//      const UINT nTotalStreams: Stream count to use for testing
//      bool bCPULoaded: Turn on RWS to stress CPU for testing
//
//  Return (DWORD):
//      Unused.  Failures propagated through XLOG.
//
//
//--------------------------------------------------------------------------;
DWORD RunCaptureScenario(const UINT nTotalStreams, bool bCPULoaded)
{
    CAudioLogoDevice                     *pCurrentDevice = NULL;
    IMMDevice                            *pEndpoint = NULL;
    HRESULT                               hr = S_OK;
    CComPtr<IBasicLog>                    spLogger;
    CSimpleArray<IAudioStreamingCapture*> aspCaptureEngines;
    PROCESS_INFORMATION                   pi = {0};
    CComHeapPtr<WCHAR>                    pwszTraceName;
    bool                                  audioGlitchTracingInitialized = false;

    if (S_OK != (hr = g_IShell->GetBasicLog(&spLogger)))
    {
        XLOG(XFAIL, eError,L"FAIL: GetBasicLog returned error (%s).",GetHRString(hr));
        return (FNS_PASS);
    }

    // This test only applies to Windows 8 and above
    OSVERSIONINFO ver = {0};
    ver.dwOSVersionInfoSize = sizeof(ver);
    if (!GetVersionEx(&ver))
    {
        XLOG(XFAIL, eFatalError, L"FAIL: GetVersionEx() failed: GetLastError() = %u", GetLastError());
        return (FNS_PASS);
    }

    XLOG(XMSG, eInfo1, L"INFO: OS version is %u.%u", ver.dwMajorVersion, ver.dwMinorVersion);

    if (ver.dwMajorVersion <  6 || (ver.dwMajorVersion == 6 && ver.dwMinorVersion < 3))
    {
        XLOG(XSKIP, eInfo1, L"INFO: This test case is only applicable to Windows 6.3 and above.");
        return (FNS_PASS);
    }

    // tell the user what we're doing
    XLOG(XMSG, eWarn1, L"<<Preparing to capture %d simultaneous streams and check for audio glitches>>", 
        nTotalStreams);
    // get the endpoint under test
    if (NULL == (pCurrentDevice = g_pAudioLogoDeviceTestModule->GetDevice()))
    {
        XLOG(XFAIL, eError, L"FAIL: GetDevice encountered error.");
        return (FNS_PASS);
    }

    if (NULL == (pEndpoint = pCurrentDevice->GetEndpoint()))
    {
        XLOG(XFAIL, eError, L"FAIL: GetEndpoint encountered error.");
        return (FNS_PASS);
    }
    
    // check whether the endpoint supports raw mode
    // if it does, we know we're on Blue or higher
    bool rawModeSupported = false;
    if (S_OK != (hr = AudioOffload::DoesDeviceSupportRawMode(spLogger, pEndpoint, rawModeSupported)))
    {
        XLOG(XFAIL, eError,L"FAIL: AudioOffload::DoesDeviceSupportRawMode returned failure (%s).", GetHRString(hr));
        return (FNS_PASS);
    }

    // initialize the capturers
    for (UINT32 i=0; i<nTotalStreams; i++)
    {
        IAudioStreamingCapture* pCurrCapture;
        hr = CreateAudioStreamingCapture(api_WASAPI,
            pEndpoint, 
            spLogger, 
            &pCurrCapture);
        if (S_OK != hr)
        {
            XLOG(XFAIL, eError, L"FAIL: CreateAudioStreamingCapture returned error (%s) for stream %d.",
                GetHRString(hr), i);
            goto CLEANUP;
        }

        if (! aspCaptureEngines.Add(pCurrCapture))
        {
            XLOG(XFAIL, eError, L"FAIL: adding capturer to array for stream %d.", i);
            pCurrCapture->Release();
            goto CLEANUP;
        }
        
        // if the endpoint supports raw, make every other stream raw
        if (rawModeSupported && (i % 2 == 1))
        {
            CComPtr<IAudioStreamingControl> spAudioStreamingControl;
            CComPtr<IAudioStreamingWasapiControl> spAudioStreamingWasapiControl;
        
            struct AudioClientPropertiesBlue
            {
                UINT32                  cbSize;
                BOOL                    bIsOffload;
                AUDIO_STREAM_CATEGORY   eCategory;
                AUDCLNT_STREAMOPTIONS   Options;
            };
            
            AudioClientPropertiesBlue raw = {};
            raw.cbSize = sizeof(raw);
            raw.bIsOffload = FALSE;
            raw.eCategory = AudioCategory_Other;
            raw.Options = AUDCLNT_STREAMOPTIONS_RAW;
            
            if (S_OK != (hr = pCurrCapture->QueryInterface(
                    __uuidof(IAudioStreamingControl),
                    reinterpret_cast<void**>(&spAudioStreamingControl)
            )))
            {
                XLOG(XFAIL, eError, L"FAIL: IAudioStreamingCapture::QueryInterface(IAudioStreamingControl) returned error %s (0x%08x)", GetHRString(hr), hr);
                goto CLEANUP;
            }
            
            if (S_OK != (hr = spAudioStreamingControl->GetService(
                    __uuidof(IAudioStreamingWasapiControl),
                    reinterpret_cast<void**>(&spAudioStreamingWasapiControl)
            )))
            {
                XLOG(XFAIL, eError, L"FAIL: IAudioStreamingControl::GetService(IAudioStreamingWasapiControl) returned error %s (0x%08x)", GetHRString(hr), hr);
                goto CLEANUP;
            }
            
            if (S_OK != (hr = spAudioStreamingWasapiControl->SetClientProperties(
                reinterpret_cast<AudioClientProperties *>(&raw)
            )))
            {
                XLOG(XFAIL, eError, L"FAIL: IAudioStreamingWasapiControl::SetClientProperties returned error %s (0x%08x)", GetHRString(hr), hr);
                goto CLEANUP;
            }
        }
    }

    for (UINT32 i=0; i<nTotalStreams; i++)
    {
        if (S_OK != (hr = aspCaptureEngines[i]->Initialize(NULL)))
        {
            XLOG(XFAIL, eError, L"FAIL: IAudioStreamingCapture::Initialize returned error (%s).",GetHRString(hr));
            goto CLEANUP;
        }
    }

    hr = AudioGlitchTracer::Initialize(spLogger);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Initializing Tracer Library failed (%s).", GetHRString(hr));
        goto CLEANUP;
    }
    audioGlitchTracingInitialized = true;

    hr = AudioGlitchTracer::GenerateTraceName(&pwszTraceName);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Generating Trace Name failed (%s).", GetHRString(hr));
        goto CLEANUP;
    }

    if (bCPULoaded)
    {
        //turn on cpu load tool
        hr = Run100CPUStress(TEST_DURATION_MILLIS * 2, &pi);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, L"FAIL: Error starting CPU Stress (%s).", GetHRString(hr));
            goto CLEANUP;
        }
    }

    // start capturing ETW trace info
    hr = AudioGlitchTracer::StartETW(pwszTraceName);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Error starting ETW tracing (%s).", GetHRString(hr));
        goto CLEANUP;
    }

    // start the streams
    for (UINT32 i=0; i<nTotalStreams;i++)
    {

        if (S_OK != (hr = aspCaptureEngines[i]->CaptureWithoutBufferAsync()))
        {
            XLOG(XFAIL, eError, L"FAIL: IAudioStreamingCapture::CaptureWithoutBufferAsync returned error (%s).",
                GetHRString(hr));
            // don't short circuit
        }
    }

    // stream for roughly TEST_DURATION_MILLIS ms
    Sleep(TEST_DURATION_MILLIS);

    // stop streaming
    for (UINT32 i=0; i<nTotalStreams;i++)
    {
        if (S_OK != (hr = aspCaptureEngines[i]->Stop()))
        {
            XLOG(XFAIL, eError, L"FAIL: IAudioStreamingCapture::Stop returned error (%s).", GetHRString(hr));
            // don't short circuit
        }
    }

    // Stop capturing ETW trace info
    hr = AudioGlitchTracer::StopETW(pwszTraceName);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Error stopping ETW tracing (%s).", GetHRString(hr));
        goto CLEANUP;
    }

    if (bCPULoaded)
    {
        //verify cpu stress ended
        hr = VerifyStressCompleted(TEST_DURATION_MILLIS * 2, &pi);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, L"FAIL: Error Verifying CPU Stress Terminated(%s).", GetHRString(hr));
            goto CLEANUP;
        }
    }

    UINT64 nGlitchEvents = 0;

    // analyze the trace info to determine the number of glitches
    hr = AudioGlitchTracer::ProcessETWTrace(pwszTraceName, &nGlitchEvents);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Error analyzing ETL file (%s)", GetHRString(hr));
        goto CLEANUP;
    }

    XLOG(XMSG, eInfo1, L">>>> Trace contains %I64u audio glitches <<<<", nGlitchEvents);

    if (0 == nGlitchEvents)
    {
        XLOG(XMSG, eInfo1, L"[PASS]");

        // delete the etw logs
        hr = AudioGlitchTracer::DeleteETWTraceFiles(pwszTraceName);
        if (FAILED(hr))
        {
            // log a warning but don't fail the test
            XLOG(XWARN, eError, L"WARNING: Error deleting ETL files for trace\"%s\" (%s)",
                static_cast<LPCWSTR>(pwszTraceName), GetHRString(hr));
        }
    }
    else
    {
        // Display an error message to the user explaining the failure.
        // Display the name of the trace file that was generated for potential analysis.  Note that the kit job(s)
        // associated with this content will need to copy the trace on failure
        XLOG(XFAIL, eInfo1, L"FAIL: No audio glitches are allowed!");
        XLOG(XFAIL, eInfo1, L"FAIL: ETW trace information for this test can be found in %s_merged.etl", static_cast<LPCWSTR>(pwszTraceName));
    }

CLEANUP:
    if (audioGlitchTracingInitialized)
    {
        hr = AudioGlitchTracer::Cleanup();
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, L"FAIL: Cleaning up Tracer Lib failed (%s)", GetHRString(hr));
        }
    }

    for (INT i = 0; i < aspCaptureEngines.GetSize(); i++)
    {
        if (nullptr != aspCaptureEngines[i])
        {
            (aspCaptureEngines[i])->Release();
        }
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return (FNS_PASS);
}

//--------------------------------------------------------------------------;
//
//  DWORD TC_VerifyMultiRenderGlitchFree
//
//  Description:
//      TestCase: On render endpoints, verifies that n-simultaneous shared-
//      mode render streams to not result in glitching.
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Unused.  Failures propagated through XLOG.
//
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_VerifyMultiRenderGlitchFree()
{
    return RunRenderScenario(REQUIRED_SIMUL_GF_RENDER_STREAMS, false, WIN_8_MINOR_VER);
}

//--------------------------------------------------------------------------;
//
//  DWORD TC_VerifyMultiCaptureGlitchFree
//
//  Description:
//      TestCase: On capture endpoints, verifies that n-simultaneous 
//      capture streams do not result in glitching.
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Unused.  Failures propagated through XLOG.
//
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_VerifyMultiCaptureGlitchFree()
{
    return RunCaptureScenario(REQUIRED_SIMUL_GF_CAPTURE_STREAMS, false);
}

//--------------------------------------------------------------------------;
//
//  DWORD TC_VerifyStressedCaptureGlitchFree
//
//  Description:
//      TestCase: On render endpoints, verifies that n-simultaneous shared-
//      mode render streams to not result in glitching while stressing the CPU.
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Unused.  Failures propagated through XLOG.
//
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_VerifyStressedCaptureGlitchFree()
{
    return RunCaptureScenario(REQUIRED_STRESS_GF_CAPTURE_STREAMS, true);
}

//--------------------------------------------------------------------------;
//
//  DWORD TC_VerifyStressedRenderGlitchFree
//
//  Description:
//      TestCase: On render endpoints, verifies that n-simultaneous shared-
//      mode render streams to not result in glitching while stressing the CPU.
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Unused.  Failures propagated through XLOG.
//
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_VerifyStressedRenderGlitchFree()
{
    return RunRenderScenario(REQUIRED_STRESS_GF_RENDER_STREAMS, true, WIN_BLUE_MINOR_VER);
}
