// Copyright(C) Microsoft.All rights reserved.

// HWAudioEngineEventLogger.cpp

#include <atlbase.h>
#include <atlstr.h>
#include <strsafe.h>

#include <basiclog.h>
#include <BasicLogHelper.h>
#include <basicprintf.h>

#include <WexTestClass.h>

#include <evntcons.h>
#include <ETWLogger.h>

#include "log.h"
#include "HWAudioEngineEventLogger.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;
using namespace EtwLogger;

HwAudioEngineEventLogger::~HwAudioEngineEventLogger() {
    HRESULT hr = S_OK;

    if (m_isMonitoring) {
        hr = m_spEtwEventLogger->StopMonitoring();
        if (FAILED(hr)) { return; }
    }

    if (m_isProviderEnabled) {
        hr = m_spEtwEventLogger->DisableProvider(m_gETWProvider);
        if (FAILED(hr)) { return; }
    }

    if (m_isSessionStarted) {
        hr = m_spEtwEventLogger->StopSession();
        if (FAILED(hr)) { return; }
    }
}

HRESULT HwAudioEngineEventLogger::Initialize(_In_ LPCWSTR wsczSessionName, _In_opt_ GUID pETWProviderGuid /* = g_PortClsProviderGuid*/, _In_opt_ UCHAR iTraceLevel /* = ETW_TRACE_LEVEL_VERBOSE*/) {
    HRESULT hr = S_OK;

    CComPtr<IEtwEventLogger> spEtwEventLogger;
    if (!VERIFY_SUCCEEDED(hr = CreateEtwEventLogger(wsczSessionName, 1024, &spEtwEventLogger))) { return hr; }

    if (!VERIFY_SUCCEEDED(hr = spEtwEventLogger->StartSession())) { return hr; }

    m_isSessionStarted = true;

    if (!VERIFY_SUCCEEDED(hr = spEtwEventLogger->EnableProvider(pETWProviderGuid, iTraceLevel, 0, 0))) { return hr; }

    m_isProviderEnabled = true;

    if (IsEqualGUID(pETWProviderGuid, (GUID)g_PortClsProviderGuid)) {
        if (!VERIFY_SUCCEEDED(hr = spEtwEventLogger->AddEventFilter(g_MiniportAudioEngineEventGuid))) { return hr; }
    } 
    else {
        if (!VERIFY_SUCCEEDED(hr = spEtwEventLogger->AddEventFilter(pETWProviderGuid))) { return hr; }
    }

    m_spEtwEventLogger = spEtwEventLogger;
    m_gETWProvider     = pETWProviderGuid;

    return S_OK;
}

HRESULT HwAudioEngineEventLogger::Start() {
    HRESULT hr = S_OK;

    if (!m_spEtwEventLogger || m_isMonitoring || !m_isProviderEnabled || !m_isSessionStarted) {
        LOG( L"Invalid State to Start HW Audio Engine Event Logger: m_spEtwEventLogger(0x%px), m_isMonitoring(%u), m_isProviderEnabled(%u), m_isSessionStarted(%u)",
            m_spEtwEventLogger.p, m_isMonitoring, m_isProviderEnabled, m_isSessionStarted );

        return E_UNEXPECTED;
    }

    if (!VERIFY_SUCCEEDED(hr = m_spEtwEventLogger->StartMonitoring())) { return hr; }

    m_isMonitoring = true;

    return S_OK;
}

HRESULT HwAudioEngineEventLogger::Stop() {
    HRESULT hr = S_OK;

    if (!m_spEtwEventLogger || !m_isMonitoring || !m_isProviderEnabled || !m_isSessionStarted) {
        LOG( L"Invalid State to Stop HW Audio Engine Event Logger: m_spEtwEventLogger(0x%px), m_isMonitoring(%u), m_isProviderEnabled(%u), m_isSessionStarted(%u)",
            m_spEtwEventLogger.p, m_isMonitoring, m_isProviderEnabled, m_isSessionStarted );

        return E_UNEXPECTED;
    }

    if (!VERIFY_SUCCEEDED(hr = m_spEtwEventLogger->StopMonitoring())) { return hr; }

    m_isMonitoring = false;

    if (!VERIFY_SUCCEEDED(hr = m_spEtwEventLogger->DisableProvider(m_gETWProvider))) { return hr; }

    m_isProviderEnabled = false;

    if (!VERIFY_SUCCEEDED(hr = m_spEtwEventLogger->StopSession())) { return hr; }

    m_isSessionStarted = false;

    return S_OK;
}

HRESULT HwAudioEngineEventLogger::GetLostEventCount(_Out_ UINT* pcLostEvents)
{
    HRESULT hr = S_OK;

    if (!VERIFY_IS_NOT_NULL(pcLostEvents)) { return E_POINTER; }

    *pcLostEvents = 0;

    if (!m_spEtwEventLogger || m_isMonitoring || m_isProviderEnabled || m_isSessionStarted) {
        LOG( L"Invalid State to Get the Lost Event Count of the HW Audio Engine Event Logger: m_spEtwEventLogger(0x%px), m_isMonitoring(%u), m_isProviderEnabled(%u), m_isSessionStarted(%u)",
            m_spEtwEventLogger.p, m_isMonitoring, m_isProviderEnabled, m_isSessionStarted );

        return E_UNEXPECTED;
    }

    if (!VERIFY_SUCCEEDED(hr = m_spEtwEventLogger->GetLostEventCount( pcLostEvents ))) { return hr; }

    return S_OK;
}

HRESULT HwAudioEngineEventLogger::GetEventCount(_Out_ UINT* pcEvents)
{
    HRESULT hr = S_OK;

    if (!VERIFY_IS_NOT_NULL(pcEvents)) { return E_POINTER; }

    *pcEvents = 0;

    if (!m_spEtwEventLogger || m_isMonitoring || m_isProviderEnabled || m_isSessionStarted) {
        LOG( L"Invalid State to Get the Event Count of the HW Audio Engine Event Logger: m_spEtwEventLogger(0x%px), m_isMonitoring(%u), m_isProviderEnabled(%u), m_isSessionStarted(%u)",
            m_spEtwEventLogger.p, m_isMonitoring, m_isProviderEnabled, m_isSessionStarted );

        return E_UNEXPECTED;
    }

    if (!VERIFY_SUCCEEDED(hr = m_spEtwEventLogger->GetEventCount( pcEvents ))) { return hr; }

    return S_OK;
}

HRESULT HwAudioEngineEventLogger::GetEvent(_In_ UINT iEvent, _Outptr_ EtwLogger::IEtwEventHelper** ppEtwEventHelper) {
    HRESULT hr = S_OK;

    if (!VERIFY_IS_NOT_NULL(ppEtwEventHelper)) { return E_POINTER; }

    *ppEtwEventHelper = nullptr;

    if (!m_spEtwEventLogger || m_isMonitoring || m_isProviderEnabled || m_isSessionStarted) {
        LOG( L"Invalid State to Get Event from the HW Audio Engine Event Logger: m_spEtwEventLogger(0x%px), m_isMonitoring(%u), m_isProviderEnabled(%u), m_isSessionStarted(%u)",
            m_spEtwEventLogger.p, m_isMonitoring, m_isProviderEnabled, m_isSessionStarted );

        return E_UNEXPECTED;
    }

    if (!VERIFY_SUCCEEDED(hr = m_spEtwEventLogger->GetEvent(iEvent, ppEtwEventHelper))) { return hr; }

    return S_OK;
}

