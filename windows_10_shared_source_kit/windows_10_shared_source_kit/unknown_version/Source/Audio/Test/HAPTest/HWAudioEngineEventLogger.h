// Copyright(C) Microsoft.All rights reserved.

// HWAudioEngineEventLogger.h

// {9502CBC6-AA74-4eff-BA91-D9329BCCE758}
static const GUID g_PortClsProviderGuid = 
{ 0x9502CBC6, 0xAA74, 0x4eff, { 0xBA, 0x91, 0xD9, 0x32, 0x9B, 0xCC, 0xE7, 0x58 } };

// {97BCD203-6286-4974-801D-A293B921F84B}
static const GUID g_MiniportAudioEngineEventGuid = 
{ 0x97BCD203, 0x6286, 0x4974, { 0x80, 0x1D, 0xA2, 0x93, 0xB9, 0x21, 0xF8, 0x4B } };


// MFCore ETW provider id 
// from ..\avcore\mf\setup\manifests\MF\MFPlat.man
// {B20E65AC-C905-4014-8F78-1B6A508142EB}
static const GUID g_MFCoreProvider = 
{ 0xB20E65AC, 0xC905, 0x4014, { 0x8F, 0x78, 0x1B, 0x6A, 0x50, 0x81, 0x42, 0xEB } };

#define ETW_TRACE_LEVEL_PROVIDER_SPECIFIC 0
#define ETW_TRACE_LEVEL_CRITICAL          1
#define ETW_TRACE_LEVEL_ERROR             2
#define ETW_TRACE_LEVEL_WARNING           3
#define ETW_TRACE_LEVEL_INFORMATION       4
#define ETW_TRACE_LEVEL_VERBOSE           5

class HwAudioEngineEventLogger {
public:
    HwAudioEngineEventLogger(IBasicLog* logger ) : pLogger(logger), m_isSessionStarted(false), m_isProviderEnabled(false),
        m_isMonitoring(false), m_gETWProvider(GUID_NULL), m_gMiniportAudioEngineEvent(GUID_NULL) {}
    ~HwAudioEngineEventLogger();
    HRESULT Initialize(_In_ LPCWSTR wsczSessionName, _In_opt_ GUID pETWProviderGuid = g_PortClsProviderGuid, _In_opt_ UCHAR iTraceLevel = ETW_TRACE_LEVEL_VERBOSE);
    HRESULT Start();
    HRESULT Stop();
    HRESULT GetLostEventCount(_Out_ UINT* pcLostEvents);
    HRESULT GetEventCount(_Out_ UINT* pcEvents);
    HRESULT GetEvent(_In_ UINT iEvent, _Outptr_ EtwLogger::IEtwEventHelper** ppEtwEventHelper);

private:

    CComPtr<IBasicLog> pLogger;
    CComPtr<EtwLogger::IEtwEventLogger> m_spEtwEventLogger;
    CComHeapPtr<WCHAR> m_strSessionName;
    bool m_isSessionStarted;
    bool m_isProviderEnabled;
    bool m_isMonitoring;
    GUID m_gETWProvider;
    GUID m_gMiniportAudioEngineEvent;
};

