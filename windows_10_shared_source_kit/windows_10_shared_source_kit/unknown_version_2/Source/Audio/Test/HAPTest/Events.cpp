// Copyright(C) Microsoft.All rights reserved.

// events.cpp

#pragma once
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <shell98.h>

#include <module98.h>
#include <tchar.h>

#include <dvmodule.h>

// Audio Core Headers
#include <mmsystem.h>

#include <versionhelpers.h>

#include <atlbase.h>
#include <atlstr.h>
#include <ks.h>
#include <ksmedia.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <strsafe.h>

#include <basiclog.h>
#include <BasicLogHelper.h>
#include <basicprintf.h>

#include <audiostreaming.h>
#include <OffloadPath.h>
#include <evntcons.h>
#include <etwlogger.h>
#include <mfcoreevents.h>

#include <WexTestClass.h>

#include "QueryMachineInfo.h"

#include "haptest.h"
#include "log.h"
#include "CSHelperClass.h"

#include "HWAudioEngineEventLogger.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;

//  Internal PortCls definitions from portcls/inc/perf.h
typedef struct _PORTCLS_WMI_DATA
{
    ULONGLONG EventType;
    ULONGLONG Data1;
    ULONGLONG Data2;
    ULONGLONG Data3;
    ULONGLONG Data4;
    ULONGLONG Data5;
} PORTCLS_WMI_DATA, *PPORTCLS_WMI_DATA;

// MFCore ETW provider id 
// from ..\avcore\mf\setup\manifests\MF\MFPlat.man
// {B20E65AC-C905-4014-8F78-1B6A508142EB}
static const GUID MFPlat_Core_Provider  = { 0xB20E65AC, 0xC905, 0x4014, { 0x8F, 0x78, 0x1B, 0x6A, 0x50, 0x81, 0x42, 0xEB } };

// {97BCD203-6286-4974-801D-A293B921F84B}
const GUID Portcls_AudioEngine_Provider = { 0x97BCD203, 0x6286, 0x4974, { 0x80, 0x1D, 0xA2, 0x93, 0xB9, 0x21, 0xF8, 0x4B } };

// defined in file \\depot\drivers\wdm\audio\backpln\portcls\inc\perf.h
#define PC_ETYPE_MINIPORT_MINIPORT_DEFINED          0x60
#define PC_ETYPE_MINIPORT_BUFFER_COMPLETE_EVENT     0x61
#define PC_ETYPE_MINIPORT_PIN_STATE                 0x62
#define PC_ETYPE_MINIPORT_GET_STREAM_POS            0x63
#define PC_ETYPE_MINIPORT_SET_WAVERT_WRITE_POS      0x64
#define PC_ETYPE_MINIPORT_GET_PRESENTATION_POS      0x65
#define PC_ETYPE_MINIPORT_PROGRAM_DMA               0x66
#define PC_ETYPE_MINIPORT_GLITCH_REPORT             0x67
#define PC_ETYPE_MINIPORT_LAST_BUFFER_RENDERED      0x68

enum eGlitchType {
    Glitch_WaveRTBufferUnderrun = 1,
    Glitch_DecoderError = 2,
    Glitch_DuplicateWaveRTWritePos = 3
};

double MsFromQpc(LARGE_INTEGER qpf, LARGE_INTEGER qpcStart, LARGE_INTEGER qpcEnd) {
    return 1000.0 * (qpcEnd.QuadPart - qpcStart.QuadPart) / qpf.QuadPart;
}

#define STRINGCASE(a) case a: return L#a

LPCWSTR GetGlitchTypeString(UINT64 type) {
    switch (type) {
        STRINGCASE(Glitch_WaveRTBufferUnderrun);
        STRINGCASE(Glitch_DecoderError);
        STRINGCASE(Glitch_DuplicateWaveRTWritePos);
        default:
            return (L"Undefined");
    }
}

LPCWSTR GetEventTypeString (UINT64   ulEventType) {
    switch (ulEventType) {
        STRINGCASE(PC_ETYPE_MINIPORT_MINIPORT_DEFINED);
        STRINGCASE(PC_ETYPE_MINIPORT_BUFFER_COMPLETE_EVENT);
        STRINGCASE(PC_ETYPE_MINIPORT_PIN_STATE);
        STRINGCASE(PC_ETYPE_MINIPORT_GET_STREAM_POS);
        STRINGCASE(PC_ETYPE_MINIPORT_SET_WAVERT_WRITE_POS);
        STRINGCASE(PC_ETYPE_MINIPORT_GET_PRESENTATION_POS);
        STRINGCASE(PC_ETYPE_MINIPORT_PROGRAM_DMA);
        STRINGCASE(PC_ETYPE_MINIPORT_GLITCH_REPORT);
        default:
            return (L"Undefined");
    }
}

LPCWSTR GetKsStateString(UINT64 state) {
    switch (state) {
        STRINGCASE(KSSTATE_STOP);
        STRINGCASE(KSSTATE_ACQUIRE);
        STRINGCASE(KSSTATE_PAUSE);
        STRINGCASE(KSSTATE_RUN);
        default:
            return (L"Undefined");
    }
}

void LogAudioEngineEvent(IBasicLog *pLogger, double ms, PORTCLS_WMI_DATA event) {

    // Event meanings and syntax are defined in the whitepaper
    // Exposing Hardware-Offloaded Audio Processing in Windows
    // http://msdn.microsoft.com/en-us/library/windows/hardware/br259116.aspx
    //
    // There is also a sample implementation in AEVad with verbose comments

    switch (event.EventType) {
        case PC_ETYPE_MINIPORT_MINIPORT_DEFINED:
            LOG(
                L"%g ms: Miniport-defined event: "
                L"%I64u, %I64u, %I64u, %I64u",
                ms, event.Data1, event.Data2, event.Data3, event.Data4
            );
            break;
        case PC_ETYPE_MINIPORT_BUFFER_COMPLETE_EVENT:
            LOG(
                L"%g ms: Buffer complete: "
                L"linear buffer position %I64u, "
                L"WaveRtBufferWritePosition %I64u, "
                L"data length completed %I64u",
                ms, event.Data1, event.Data2, event.Data3
            );
            break;
        case PC_ETYPE_MINIPORT_PIN_STATE:
            LOG(
                L"%g ms: Pin state: "
                L"linear buffer position %I64u, "
                L"WaveRtBufferWritePosition %I64u, "
                L"pin state %s (%I64u)",
                ms, event.Data1, event.Data2, GetKsStateString(event.Data3), event.Data3
            );
            break;
        case PC_ETYPE_MINIPORT_GET_STREAM_POS:
            LOG(
                L"%g ms: Get stream position: "
                L"linear buffer position %I64u, "
                L"WaveRtBufferWritePosition %I64u",
                ms, event.Data1, event.Data2
            );
            break;
        case PC_ETYPE_MINIPORT_SET_WAVERT_WRITE_POS:
            LOG(
                L"%g ms: Set write position: "
                L"linear buffer position %I64u, "
                L"old WaveRtBufferWritePosition %I64u, "
                L"new WaveRtBufferWritePosition %I64u",
                ms, event.Data1, event.Data2, event.Data3
            );
            break;
        case PC_ETYPE_MINIPORT_GET_PRESENTATION_POS:
            LOG(
                L"%g ms: Get stream position: "
                L"linear buffer position %I64u, "
                L"WaveRtBufferWritePosition %I64u, "
                L"presentation position %I64u",
                ms, event.Data1, event.Data2, event.Data3
            );
            break;
        case PC_ETYPE_MINIPORT_PROGRAM_DMA:
            LOG(
                L"%g ms: Program DMA: "
                L"linear buffer position %I64u, "
                L"WaveRtBufferWritePosition %I64u, "
                L"starting offset %I64u, "
                L"data length %I64u",
                ms, event.Data1, event.Data2, event.Data3, event.Data4
            );
            break;
        case PC_ETYPE_MINIPORT_GLITCH_REPORT:
            if (Glitch_DuplicateWaveRTWritePos == event.Data3) {
                LOG(
                    L"%g ms: Glitch report: "
                    L"linear buffer position %I64u, "
                    L"WaveRtBufferWritePosition %I64u, "
                    L"glitch type %s (%I64u), "
                    L"duplicate write position %I64u",
                    ms, event.Data1, event.Data2, GetGlitchTypeString(event.Data3), event.Data3, event.Data4
                );
            } else {
                LOG(
                    L"%g ms: Glitch report: "
                    L"linear buffer position %I64u, "
                    L"WaveRtBufferWritePosition %I64u, "
                    L"glitch type %s (%I64u)",
                    ms, event.Data1, event.Data2, GetGlitchTypeString(event.Data3), event.Data3
                );
            }
            break;
        case PC_ETYPE_MINIPORT_LAST_BUFFER_RENDERED:
            LOG(L"!!! LastBufferRendered Event Fired !!!");               
            break;
        default:
            LOG(
                L"%g ms: Unrecognized audio engine event type %I64u: "
                L"%I64u, %I64u, %I64u, %I64u",
                ms, event.EventType, event.Data1, event.Data2, event.Data3, event.Data4
            );
            break;
    }
}

void CHapTest::UnderrunGlitchReport () {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    CComPtr<IBasicLog> pLogger;
    if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return; }

    CComPtr<IMMDevice> pMMDevice;
    bool bSysvad = false;
    if (!DeviceUnderTest(pLogger, &pMMDevice, &bSysvad)) { return; }
    if (bSysvad)
    {
        Log::Result(TestResults::Result::Skipped, L"Virtual Audio Device doesn't support glitch events");
        return;
    }

    // Get the offload pin factory id
    CComPtr<IOffloadPath> spOffloadPath;
    if (!VERIFY_SUCCEEDED(GetOffloadPath(pLogger, pMMDevice, &spOffloadPath))) { return; }

    UINT idOffloadPinFactory;
    if (!SUCCEEDED(spOffloadPath->GetOffloadPinFactoryId(&idOffloadPinFactory))) { return; }

    // Setup for Kernel Streaming through the offload pin
    CComPtr<IAudioStreamingRender> spAudioStreamingRender;
    if (!VERIFY_SUCCEEDED(
        CreateAudioStreamingRender(
            api_KernelStreaming,
            pMMDevice,pLogger,
            &spAudioStreamingRender
    ))) { return; }

    CComPtr<IAudioStreamingControl> spAudioStreamingControl;
    if (!VERIFY_SUCCEEDED(spAudioStreamingRender->QueryInterface(&spAudioStreamingControl))) { return; }

    CComPtr<IAudioKernelStreamingControl> spAudioKernelStreamingControl;
    if (!VERIFY_SUCCEEDED(
        spAudioStreamingControl->GetService(
            __uuidof(IAudioKernelStreamingControl),
            reinterpret_cast<void**>(&spAudioKernelStreamingControl)
    ))) { return; }

    if (!VERIFY_SUCCEEDED(spAudioKernelStreamingControl->SetStreamPinFactoryId(idOffloadPinFactory))) { return; }

    if (!VERIFY_SUCCEEDED(spAudioStreamingRender->Initialize(NULL))) { return; }

    // get access to the test hook control
    CComPtr<IAudioKernelStreamingTestHookControl> spAudioKernelStreamingTestHookControl;
    if (!VERIFY_SUCCEEDED(
        spAudioStreamingControl->GetService(
            __uuidof(IAudioKernelStreamingTestHookControl),
            reinterpret_cast<void**>(&spAudioKernelStreamingTestHookControl)
    ))) { return; }

    LARGE_INTEGER qpf = {};
    if (!VERIFY_WIN32_BOOL_SUCCEEDED(QueryPerformanceFrequency(&qpf))) { return; }
    
    LARGE_INTEGER qpcStart = {};
    if (!VERIFY_WIN32_BOOL_SUCCEEDED(QueryPerformanceCounter(&qpcStart))) { return; }
    
    // setup for logging portcls ETW for miniport audio engine
    HwAudioEngineEventLogger hwAudioEngineEventLogger(pLogger);
    if (!VERIFY_SUCCEEDED(hwAudioEngineEventLogger.Initialize(L"UnderrunGlitchReport"))) { return; }

    // start streaming audio data to the offload endpoint
    if (!VERIFY_SUCCEEDED(
        spAudioStreamingRender->PlaySineWaveAsync(
            220.0f,
            0.1f,
            Method_NoDithering
    ))) { return; }

    if (!VERIFY_SUCCEEDED(hwAudioEngineEventLogger.Start())) { return; }

    Sleep( 100 );

    if (!VERIFY_SUCCEEDED(spAudioKernelStreamingTestHookControl->TestBufferUnderrun(TRUE))) { return; }

    Sleep( 10000 );

    // stop the rendering audio data
    if (!VERIFY_SUCCEEDED(spAudioStreamingRender->Stop())) { return; }

    // stop collecting events
    if (!VERIFY_SUCCEEDED(hwAudioEngineEventLogger.Stop())) { return; }

    UINT countEvents = 0;
    if (!VERIFY_SUCCEEDED(hwAudioEngineEventLogger.GetEventCount(&countEvents))) { return; }

    LOG( L"Logged %u Miniport Audio Engine Events", countEvents );

    bool foundGlitchEvent = false;
    for (UINT i = 0; i < countEvents; i++) {
        CComPtr<EtwLogger::IEtwEventHelper> spEtwEventHelper;
        if (!VERIFY_SUCCEEDED(hwAudioEngineEventLogger.GetEvent(i, &spEtwEventHelper))) { return; }

        GUID gProviderId(GUID_NULL);
        if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetHeaderProviderId(&gProviderId))) { return; }

        if (Portcls_AudioEngine_Provider != gProviderId) {
            continue;
        }
        
        CComHeapPtr<WCHAR> wszProviderGuid;
        if (!VERIFY_IS_TRUE(wszProviderGuid.Allocate( 39 ))) { return; }
        if (!VERIFY_SUCCEEDED(StringFromGUID2(gProviderId, wszProviderGuid.m_pData, 39))) { return; }

        LARGE_INTEGER qpcTimeStamp = {0};
        if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetHeaderTimeStamp(&qpcTimeStamp))) { return; }
        
        USHORT cbUserData = 0;
        if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetUserDataSize(&cbUserData))) { return; }

        if (sizeof(PORTCLS_WMI_DATA) != cbUserData) { continue; }

        PORTCLS_WMI_DATA audioEngineEvent = { 0, 0, 0, 0, 0, 0 };
        if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetUserData((LPBYTE)&audioEngineEvent, sizeof(PORTCLS_WMI_DATA)))) { return; }
        
        LogAudioEngineEvent(pLogger, MsFromQpc(qpf, qpcStart, qpcTimeStamp), audioEngineEvent);
        
        if (
            PC_ETYPE_MINIPORT_GLITCH_REPORT == audioEngineEvent.EventType &&
            Glitch_WaveRTBufferUnderrun == audioEngineEvent.Data3
        ) {
                Log::Comment( L"*** FOUND THE EXPECTED GLITCH UNDERRUN EVENT ***" );
                foundGlitchEvent = true;
        }
    } // for each event

    if (0 == countEvents) {
        Log::Result( TestResults::Failed, L"No Miniport Audio Engine Events were captured including "
                                          L"\"WaveRT Buffer Underrun\" Glitch Events.  Drivers that support "
                                          L"Audio Offload must support the IPortClsEtwHelper interface and generate "
                                          L"Glitch Events under the specified circumstances." );
    } else if (!foundGlitchEvent) {
        Log::Result( TestResults::Failed, L"No \"WaveRT Buffer Underrun\" Glitch Events were captured.  "
                                          L"Drivers that support Audio Offload must support the IPortClsEtwHelper "
                                          L"interface and generate Glitch Events under the specified circumstances." );
    }
}

void CHapTest::DuplicateWritePositionGlitchReport() {
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    CComPtr<IBasicLog> pLogger;
    if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return; }

    CComPtr<IMMDevice> pMMDevice;
    bool bSysvad = false;
    if (!DeviceUnderTest(pLogger, &pMMDevice, &bSysvad)) { return; }
    if (bSysvad)
    {
        Log::Result(TestResults::Result::Skipped, L"Virtual Audio Device doesn't support glitch events");
        return;
    }

    // Get the offload pin factory id
    CComPtr<IOffloadPath> spOffloadPath;
    if (!VERIFY_SUCCEEDED(GetOffloadPath(pLogger, pMMDevice, &spOffloadPath))) { return; }

    UINT idOffloadPinFactory;
    if (!SUCCEEDED(spOffloadPath->GetOffloadPinFactoryId(&idOffloadPinFactory))) { return; }

    // Setup for Kernel Streaming through the offload pin
    CComPtr<IAudioStreamingRender> spAudioStreamingRender;
    if (!VERIFY_SUCCEEDED(
        CreateAudioStreamingRender(
            api_KernelStreaming,
            pMMDevice,pLogger,
            &spAudioStreamingRender
    ))) { return; }

    CComPtr<IAudioStreamingControl> spAudioStreamingControl;
    if (!VERIFY_SUCCEEDED(spAudioStreamingRender->QueryInterface(&spAudioStreamingControl))) { return; }

    CComPtr<IAudioKernelStreamingControl> spAudioKernelStreamingControl;
    if (!VERIFY_SUCCEEDED(
        spAudioStreamingControl->GetService(
            __uuidof(IAudioKernelStreamingControl),
            reinterpret_cast<void**>(&spAudioKernelStreamingControl)
    ))) { return; }

    if (!VERIFY_SUCCEEDED(spAudioKernelStreamingControl->SetStreamPinFactoryId(idOffloadPinFactory))) { return; }

    if (!VERIFY_SUCCEEDED(spAudioStreamingRender->Initialize(NULL))) { return; }

    // get access to the test hook control
    CComPtr<IAudioKernelStreamingTestHookControl> spAudioKernelStreamingTestHookControl;
    if (!VERIFY_SUCCEEDED(
        spAudioStreamingControl->GetService(
            __uuidof(IAudioKernelStreamingTestHookControl),
            reinterpret_cast<void**>(&spAudioKernelStreamingTestHookControl)
    ))) { return; }

    LARGE_INTEGER qpf = {};
    if (!VERIFY_WIN32_BOOL_SUCCEEDED(QueryPerformanceFrequency(&qpf))) { return; }
    
    LARGE_INTEGER qpcStart = {};
    if (!VERIFY_WIN32_BOOL_SUCCEEDED(QueryPerformanceCounter(&qpcStart))) { return; }
    
    // setup for logging portcls ETW for miniport audio engine
    HwAudioEngineEventLogger hwAudioEngineEventLogger(pLogger);
    if (!VERIFY_SUCCEEDED(hwAudioEngineEventLogger.Initialize(L"DuplicateWritePositionGlitchReport"))) { return; }

    // start streaming audio data to the offload endpoint
    if (!VERIFY_SUCCEEDED(
        spAudioStreamingRender->PlaySineWaveAsync(
            220.0f,
            0.1f,
            Method_NoDithering
    ))) { return; }

    if (!VERIFY_SUCCEEDED(hwAudioEngineEventLogger.Start())) { return; }

    Sleep( 100 );

    if (!VERIFY_SUCCEEDED(spAudioKernelStreamingTestHookControl->TestDuplicateCurrentWaveRTPosition(TRUE))) { return; }

    Sleep( 10000 );

    // stop the rendering audio data
    if (!VERIFY_SUCCEEDED(spAudioStreamingRender->Stop())) { return; }

    // stop collecting events
    if (!VERIFY_SUCCEEDED(hwAudioEngineEventLogger.Stop())) { return; }

    UINT countEvents = 0;
    if (!VERIFY_SUCCEEDED(hwAudioEngineEventLogger.GetEventCount(&countEvents))) { return; }

    LOG( L"Logged %u Miniport Audio Engine Events", countEvents );

    bool foundGlitchEvent = false;
    for (UINT i = 0; i < countEvents; i++) {
        CComPtr<EtwLogger::IEtwEventHelper> spEtwEventHelper;
        if (!VERIFY_SUCCEEDED(hwAudioEngineEventLogger.GetEvent(i, &spEtwEventHelper))) { return; }

        GUID gProviderId(GUID_NULL);
        if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetHeaderProviderId(&gProviderId))) { return; }

        if (Portcls_AudioEngine_Provider != gProviderId) {
            continue;
        }

        CComHeapPtr<WCHAR> wszProviderGuid;
        if (!VERIFY_IS_TRUE(wszProviderGuid.Allocate( 39 ))) { return; }
        if (!VERIFY_SUCCEEDED(StringFromGUID2(gProviderId, wszProviderGuid.m_pData, 39))) { return; }

        LARGE_INTEGER qpcTimeStamp = {0};
        if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetHeaderTimeStamp(&qpcTimeStamp))) { return; }

        USHORT cbUserData = 0;
        if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetUserDataSize(&cbUserData))) { return; }

        if (sizeof(PORTCLS_WMI_DATA) != cbUserData) { continue; }

        PORTCLS_WMI_DATA audioEngineEvent = { 0, 0, 0, 0, 0, 0 };
        if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetUserData((LPBYTE)&audioEngineEvent, sizeof(PORTCLS_WMI_DATA)))) { return; }

        LogAudioEngineEvent(pLogger, MsFromQpc(qpf, qpcStart, qpcTimeStamp), audioEngineEvent);

        if (
            PC_ETYPE_MINIPORT_GLITCH_REPORT == audioEngineEvent.EventType &&
            Glitch_DuplicateWaveRTWritePos == audioEngineEvent.Data3
        ) {
                Log::Comment( L"*** FOUND THE EXPECTED DUPLICATE WRITE POSITION EVENT ***" );
                foundGlitchEvent = true;
        }
    } // for each event

    if (0 == countEvents) {
        Log::Result( TestResults::Failed, L"No Miniport Audio Engine Events were captured including "
                                          L"\"Duplicate WaveRT Write Position\" Glitch Events.  Drivers that support "
                                          L"Audio Offload must support the IPortClsEtwHelper interface and generate "
                                          L"Glitch Events under the specified circumstances." );
    } else if (!foundGlitchEvent) {
        Log::Result( TestResults::Failed, L"No \"Duplicate WaveRT Write Position\" Glitch Events were captured.  "
                                          L"Drivers that support Audio Offload must support the IPortClsEtwHelper "
                                          L"interface and generate Glitch Events under the specified circumstances." );
    }
}

void CHapTest::EOSEvent() {
 
   if(!IsWindows8Point1OrGreater()) {
      Log::Result(TestResults::Result::Skipped, L"Test applies to Windows 8.1 and up.");
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
       Log::Result(TestResults::Result::Skipped, L"Virtual Audio Device doesn't support EOS event");
       return;
   }

   CComPtr<IOffloadPath> pOffloadPath;
   if (!SUCCEEDED(GetOffloadPath(pLogger, pMMDevice, &pOffloadPath))) { return; } // GetOffloadPath does its own VERIFY
   
   // get info from the offload path
   UINT idOffloadPinFactory;
   if (!VERIFY_SUCCEEDED(pOffloadPath->GetOffloadPinFactoryId(&idOffloadPinFactory))) { return; }
   
   ULONG nOffloadPossible;
   ULONG nOffloadCurrent;

   // ~five seconds for the source length
   UINT32 sndSourceLengh = 4139; 

   if (!VERIFY_SUCCEEDED(
       pOffloadPath->GetPinInstanceCounts(
           idOffloadPinFactory,
           &nOffloadPossible,
           &nOffloadCurrent
   ))) { return; }
           
   LOG(
       L"Offload pin factory: id %u, %u of %u possible instances\n",
       idOffloadPinFactory, nOffloadCurrent, nOffloadPossible
   );
   
   // offload pin needs to support at least two instances
   if (!VERIFY_IS_GREATER_THAN_OR_EQUAL(nOffloadPossible, 2u)) { return; }
   
   // system should be quiescent at the start of the test
   if (!VERIFY_ARE_EQUAL(0u, nOffloadCurrent)) { return; }
   
   CNormalizeVolumeAndRestoreOnExit normalizeVolume(pMMDevice);
   
   double dbExpectedPower  = -40.0;
   float  amp              = static_cast<float>(SineAmpFromDb(dbExpectedPower));
   float  rFreqsOffload    = 873.38f;

   // verify if test machine is CS capabile
   bool bCSSupported = false;

   if (!VERIFY_SUCCEEDED(QueryMachineInfo::IsAOACCapable(pLogger, bCSSupported))) { return; }
   if (bCSSupported)
   {
       // we are a CS test and we support AOAC, but we need to not test CS if the endpoint is a digital display since they 
       // cant stream audio without a video signal being present too
       BOOL bIsEndpointDigitalDisplay = FALSE;
       if (!VERIFY_SUCCEEDED(IsEndpointUnderTestDigitalDisplay(pLogger, pMMDevice, bIsEndpointDigitalDisplay))) { return; }

       if (bIsEndpointDigitalDisplay == TRUE)
       {
           LOG(L"Endpoint under test is a digital output.  Not testing CS variation.");
           bCSSupported = false;
       }
   }
   else
       LOG(L"Machine does not support AOAC.  Not testing CS variation.");

   // loop variant
   UINT32 csLoopFlag = 0;
   do
   {
       CSHelperClass csClass(pLogger);  
       csLoopFlag++;
       if (bCSSupported && csLoopFlag == 1)
       {
           LOG(L"First testing EOS in Connected Standby.");
           csClass.EnterConnectedStandby(pLogger);
       }

       CComPtr<IAudioStreamingRender>     rpAudioStreamingOffload;
       CStopOnExit<IAudioStreamingRender> rSoeOffload;

       LARGE_INTEGER qpf = {};
       if (!VERIFY_WIN32_BOOL_SUCCEEDED(QueryPerformanceFrequency(&qpf))) { return; }

       // setup for logging portcls ETW for MFPlatCore
       HwAudioEngineEventLogger mfCoreEventLogger(pLogger);
       if (!VERIFY_SUCCEEDED(mfCoreEventLogger.Initialize(L"EndOfStreamMarker", g_MFCoreProvider, ETW_TRACE_LEVEL_VERBOSE))) { return; }

       // setup for logging portcls ETW for PortCls events
       HwAudioEngineEventLogger portClsEventLogger(pLogger);
       if (!VERIFY_SUCCEEDED(portClsEventLogger.Initialize(L"LastBufferRendered"))) { return; }

       // start gathering ETW events
       if (!VERIFY_SUCCEEDED(mfCoreEventLogger.Start())) { return; }
       if (!VERIFY_SUCCEEDED(portClsEventLogger.Start())) { return; }       
       
   
       // instantiate the offload stream
       if (!VERIFY_SUCCEEDED(
           CreateAudioStreamingRender(
               api_MediaFoundation,
               pMMDevice,
               pLogger,
               &rpAudioStreamingOffload
       ))) { return; }

       // let the object decide the format
       if (!VERIFY_SUCCEEDED(rpAudioStreamingOffload->Initialize(NULL))) { return; }

       // play the sine wave for 4.139 seconds - no matter what the offloaded buffer is, it should never be a factor of a prime number.
       if (!VERIFY_SUCCEEDED(
           rpAudioStreamingOffload->PlaySineWave(
               rFreqsOffload,
               amp,
               Method_TPD,
               sndSourceLengh
           ))) { return; }
  
       rSoeOffload.Update(rpAudioStreamingOffload);
       LOG(L"Offload stream : %g Hz sine wave at amplitude %g (expected power %g dB FS)", rFreqsOffload, amp, DbFromSineAmp(amp));     

       if (bCSSupported && csLoopFlag == 1)
           csClass.ExitConnectedStandby(pLogger);

       // stop collecting events
       if (!VERIFY_SUCCEEDED(mfCoreEventLogger.Stop())) { return; }      
       if (!VERIFY_SUCCEEDED(portClsEventLogger.Stop())) { return; }
     
       UINT countEvents = 0;
       UINT lostEvents  = 0;
       if (!VERIFY_SUCCEEDED(mfCoreEventLogger.GetEventCount(&countEvents))) { return; }
       if (!VERIFY_SUCCEEDED(mfCoreEventLogger.GetLostEventCount(&lostEvents))) { return; }
       
       LOG( L"Logged %u MFCore Events", countEvents );
       LOG( L"Lost events %u", lostEvents );

       if (!countEvents)
           ERR(L"No events found");

       bool          bEOSMarkerFiredEvent       = false;
       bool          bFillRenderBufferEvent     = false;
       bool          bCMFPumpDoWorkEvent        = false; 
       LARGE_INTEGER pEosMarkerTimeStamp        = {0};
       LARGE_INTEGER pCMFPumpTimeStamp          = {0};
       LARGE_INTEGER pFillRenderBufferTimeStamp = {0};


       for (UINT i = 0; i < countEvents; i++) 
       {
           CComPtr<EtwLogger::IEtwEventHelper> spEtwEventHelper;
           if (!VERIFY_SUCCEEDED(mfCoreEventLogger.GetEvent(i, &spEtwEventHelper))) { return; }
       
           GUID gProviderId(GUID_NULL);
           if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetHeaderProviderId(&gProviderId))) { return; }
       
           if (MFPlat_Core_Provider != gProviderId) {
               continue;
           }

           USHORT szUserData = 0;
           if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetUserDataSize(&szUserData))) { return; }
           
           USHORT etwEventId = 0;
           if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetHeaderEventDescriptorId(&etwEventId))) { return; }

           // only want info from the CMFPumpDoWorkEnter event BEFORE all other events are generated.
           if (etwEventId == AudEngineStream_CMFPump_DoWork_Enter_value && !bCMFPumpDoWorkEvent)
           {              
              LOG(L"!!! First CMFPumpDoWorkEnter event found !!!");

              bCMFPumpDoWorkEvent = true;
              if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetHeaderTimeStamp(&pCMFPumpTimeStamp))) { return; }            
           }

           if (etwEventId == AudEngineStream_FillRenderBuffer_LastBufferFrameCount_value)
           {
               LOG(L"!!! LastBufferFrameCount event found !!!");
               if (bCMFPumpDoWorkEvent)
                   bFillRenderBufferEvent = true;

               if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetHeaderTimeStamp(&pFillRenderBufferTimeStamp))) { return; }
           }

           if (etwEventId == AudStreamSink_EOSMarkerFired_value)
           {
              LOG(L"!!! EOSMarkerFired event found !!!");
              if(bFillRenderBufferEvent)
                 bEOSMarkerFiredEvent = true;

              if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetHeaderTimeStamp(&pEosMarkerTimeStamp))) { return; }
           }
       } // for each event               

       LARGE_INTEGER pLastBufferRenderedTimeStamp = {0};
       bool          bLastBufferRendered          = false;

       if (!VERIFY_SUCCEEDED(portClsEventLogger.GetEventCount(&countEvents))) { return; }
       if (!VERIFY_SUCCEEDED(portClsEventLogger.GetLostEventCount(&lostEvents))) { return; }

       LOG( L"Logged %u PortCls Events", countEvents );
       LOG( L"Lost events %u", lostEvents );

       if (!countEvents)
           ERR(L"No PortCls events found");

       for (UINT i = 0; i < countEvents; i++) 
       {
           CComPtr<EtwLogger::IEtwEventHelper> spEtwEventHelper;
           if (!VERIFY_SUCCEEDED(portClsEventLogger.GetEvent(i, &spEtwEventHelper))) { return; }
       
           GUID gProviderId(GUID_NULL);
           if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetHeaderProviderId(&gProviderId))) { return; }
       
           if (Portcls_AudioEngine_Provider != gProviderId) {
               continue;
           }

           USHORT szUserData = 0;
           if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetUserDataSize(&szUserData))) { return; }
           
           USHORT etwEventId = 0;
           if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetHeaderEventDescriptorId(&etwEventId))) { return; }


           PORTCLS_WMI_DATA audioEngineEvent = { 0, 0, 0, 0, 0, 0 };
           if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetUserData((LPBYTE)&audioEngineEvent, sizeof(PORTCLS_WMI_DATA)))) { return; }
           
           if (!VERIFY_SUCCEEDED(spEtwEventHelper->GetHeaderTimeStamp(&pLastBufferRenderedTimeStamp))) { return; }
           
           if (bEOSMarkerFiredEvent)
           {         
              if (PC_ETYPE_MINIPORT_LAST_BUFFER_RENDERED == audioEngineEvent.EventType)
              {
                 bLastBufferRendered = true;

                 Log::Comment( L"!!! LAST_BUFFER_RENDERED event found !!!" );

                 double delta = MsFromQpc(qpf,
                                          pEosMarkerTimeStamp,
                                          pLastBufferRenderedTimeStamp);
                 LOG(L"Difference in time is %g milliseconds", delta);

                 // TODO: need to determine what the proper threshold for failure is here...
                 if (delta >= sndSourceLengh + 250)
                     LOG(L"Time difference between begining the EOSMarkerFired and processing of buffer exceeds 250ms - [%g ms]", delta);

              }               
           }
       } // for each event               

       if(bCMFPumpDoWorkEvent)
          LOG(L"AudEngineStream_CMFPump_DoWork_Enter event fired properly.");
       if(bFillRenderBufferEvent)
          LOG(L"AudStreamSink_FillRenderBuffer_LastBufferFrameCount event fired properly.");
       if(bEOSMarkerFiredEvent)
          LOG(L"AudStreamSink_EOSMarkerFired event fired properly.");
       if(bLastBufferRendered)
          LOG(L"PC_ETYPE_MINIPORT_LAST_BUFFER_RENDERED event fired properly.");

       if (!bFillRenderBufferEvent || !bEOSMarkerFiredEvent || !bCMFPumpDoWorkEvent || !bLastBufferRendered)
       {
           ERR(L"ERROR: One or more expected ETW events was not properly generated. "
               L"\nExpected events: "
               L"\nAudEngineStream_CMFPump_DoWork_Enter"
               L"\nAudStreamSink_FillRenderBuffer_LastBufferFrameCount"
               L"\nAudStreamSink_EOSMarkerFired"
               L"\nPC_ETYPE_MINIPORT_LAST_BUFFER_RENDERED");
       }

   }while (bCSSupported && csLoopFlag <= 1); // all playback and capture should stop

}

