//--------------------------------------------------------------------------;
//
//  File: PortClsEventLogger.cpp
//
// Copyright (C) Microsoft. All rights reserved.
//
//  History:
//      02/14/2012    CaWerner       Initial implementation.
//
//--------------------------------------------------------------------------;

#include "common.h"
#include <evntcons.h>
#include <ETWLogger.h>

#include "PortClsEventLogger.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;
using namespace EtwLogger;

PortClsEventLogger::~PortClsEventLogger()
{
    if( m_isMonitoring )
    {
        (void) m_spEtwEventLogger->StopMonitoring();
    }

    if( m_isProviderEnabled )
    {
        (void) m_spEtwEventLogger->DisableProvider( g_PortClsProviderGuid );
    }

    if( m_isSessionStarted )
    {
        (void) m_spEtwEventLogger->StopSession();
    }
}

HRESULT PortClsEventLogger::Initialize( LPCWSTR wsczSessionName )
{
    HRESULT hr = S_OK;

    CComPtr<IEtwEventLogger> spEtwEventLogger;
    if( !VERIFY_SUCCEEDED( hr = CreateEtwEventLogger( wsczSessionName, 1024, &spEtwEventLogger ) ) ) { return hr; }

    if( !VERIFY_SUCCEEDED( hr = spEtwEventLogger->StartSession() ) ) { return hr; }

    m_isSessionStarted = true;

    if( !VERIFY_SUCCEEDED( hr = spEtwEventLogger->EnableProvider( g_PortClsProviderGuid, 0, 0, 0 ) ) ) { return hr; }

    m_isProviderEnabled = true;

    if( !VERIFY_SUCCEEDED( hr = spEtwEventLogger->AddEventFilter( g_PortClsPowerPolicyEventGuid ) ) ) { return hr; }

    m_spEtwEventLogger = spEtwEventLogger;

    return S_OK;
}

HRESULT PortClsEventLogger::Start()
{
    HRESULT hr = S_OK;

    if( !m_spEtwEventLogger || m_isMonitoring || !m_isProviderEnabled || !m_isSessionStarted )
    {
        LOG( m_spBasicLog, L"Invalid State to Start HW Audio Engine Event Logger: m_spEtwEventLogger(0x%p), m_isMonitoring(%u), m_isProviderEnabled(%u), m_isSessionStarted(%u)",
            m_spEtwEventLogger.p, m_isMonitoring, m_isProviderEnabled, m_isSessionStarted );

        return E_UNEXPECTED;
    }

    if( !VERIFY_SUCCEEDED( hr = m_spEtwEventLogger->StartMonitoring() ) ) { return hr; }

    m_isMonitoring = true;

    return S_OK;
}

HRESULT PortClsEventLogger::Stop()
{
    HRESULT hr = S_OK;

    if( !m_spEtwEventLogger || !m_isMonitoring || !m_isProviderEnabled || !m_isSessionStarted )
    {
        LOG( m_spBasicLog, L"Invalid State to Stop HW Audio Engine Event Logger: m_spEtwEventLogger(0x%p), m_isMonitoring(%u), m_isProviderEnabled(%u), m_isSessionStarted(%u)",
            m_spEtwEventLogger.p, m_isMonitoring, m_isProviderEnabled, m_isSessionStarted );

        return E_UNEXPECTED;
    }

    if( !VERIFY_SUCCEEDED( hr = m_spEtwEventLogger->StopMonitoring() ) ) { return hr; }

    m_isMonitoring = false;

    if( !VERIFY_SUCCEEDED( hr = m_spEtwEventLogger->DisableProvider( g_PortClsProviderGuid ) ) ) { return hr; }

    m_isProviderEnabled = false;

    if( !VERIFY_SUCCEEDED( hr = m_spEtwEventLogger->StopSession() ) ) { return hr; }

    m_isSessionStarted = false;

    return S_OK;
}

HRESULT PortClsEventLogger::GetLostEventCount( UINT* pcLostEvents )
{
    HRESULT hr = S_OK;

    if( !VERIFY_IS_NOT_NULL( pcLostEvents ) ) { return E_POINTER; }

    *pcLostEvents = 0;

    if( !m_spEtwEventLogger || m_isMonitoring || m_isProviderEnabled || m_isSessionStarted )
    {
        LOG( m_spBasicLog, L"Invalid State to Get the Lost Event Count of the HW Audio Engine Event Logger: m_spEtwEventLogger(0x%px), m_isMonitoring(%u), m_isProviderEnabled(%u), m_isSessionStarted(%u)",
            m_spEtwEventLogger.p, m_isMonitoring, m_isProviderEnabled, m_isSessionStarted );

        return E_UNEXPECTED;
    }

    if( !VERIFY_SUCCEEDED( hr = m_spEtwEventLogger->GetLostEventCount( pcLostEvents ) ) ) { return hr; }

    return S_OK;
}

HRESULT PortClsEventLogger::GetEventCount( UINT* pcEvents )
{
    HRESULT hr = S_OK;

    if( !VERIFY_IS_NOT_NULL( pcEvents ) ) { return E_POINTER; }

    *pcEvents = 0;

    if( !m_spEtwEventLogger || m_isMonitoring || m_isProviderEnabled || m_isSessionStarted )
    {
        LOG( m_spBasicLog, L"Invalid State to Get the Event Count of the HW Audio Engine Event Logger: m_spEtwEventLogger(0x%px), m_isMonitoring(%u), m_isProviderEnabled(%u), m_isSessionStarted(%u)",
            m_spEtwEventLogger.p, m_isMonitoring, m_isProviderEnabled, m_isSessionStarted );

        return E_UNEXPECTED;
    }

    if( !VERIFY_SUCCEEDED( hr = m_spEtwEventLogger->GetEventCount( pcEvents ) ) ) { return hr; }

    return S_OK;
}

HRESULT PortClsEventLogger::GetEvent( UINT iEvent, EtwLogger::IEtwEventHelper** ppEtwEventHelper )
{
    HRESULT hr = S_OK;

    if( !VERIFY_IS_NOT_NULL( ppEtwEventHelper ) ) { return E_POINTER; }

    *ppEtwEventHelper = nullptr;

    if( !m_spEtwEventLogger || m_isMonitoring || m_isProviderEnabled || m_isSessionStarted )
    {
        LOG( m_spBasicLog, L"Invalid State to Get Event from the HW Audio Engine Event Logger: m_spEtwEventLogger(0x%px), m_isMonitoring(%u), m_isProviderEnabled(%u), m_isSessionStarted(%u)",
            m_spEtwEventLogger.p, m_isMonitoring, m_isProviderEnabled, m_isSessionStarted );

        return E_UNEXPECTED;
    }

    if( !VERIFY_SUCCEEDED( hr = m_spEtwEventLogger->GetEvent( iEvent, ppEtwEventHelper ) ) ) { return hr; }

    return S_OK;
}

HRESULT PortClsEventLogger::GetPowerPolicyEvent( UINT iEvent, PowerPolicyEvent* pPowerPolicyEvent )
{
    HRESULT hr = S_OK;
    CComPtr<EtwLogger::IEtwEventHelper> spEtwEventHelper;
    GUID guidProvider = GUID_NULL;
    USHORT cbUserData = 0;

    pPowerPolicyEvent->EventType = (ULONGLONG)-1;
    pPowerPolicyEvent->DeviceObjectPtr = (ULONGLONG)-1;
    pPowerPolicyEvent->Value = (ULONGLONG)-1;
    pPowerPolicyEvent->Complete = (ULONGLONG)-1;

    if( !VERIFY_SUCCEEDED( hr = GetEvent( iEvent, &spEtwEventHelper ) ) ) { return hr; }

    if( !VERIFY_SUCCEEDED( hr = spEtwEventHelper->GetHeaderProviderId( &guidProvider ) ) ) { return hr; }
    if( !VERIFY_IS_TRUE( FALSE != IsEqualGUID( guidProvider, g_PortClsPowerPolicyEventGuid ) ) )
    {
        WCHAR wszExpectedGUID[40];
        WCHAR wszActualGUID[40];

        StringFromGUID2( g_PortClsPowerPolicyEventGuid, wszExpectedGUID, ARRAYSIZE(wszExpectedGUID) );
        StringFromGUID2( guidProvider, wszActualGUID, ARRAYSIZE(wszActualGUID) );

        LOG( m_spBasicLog, L"Only Expecting PortCls Power Policy Events (GUID: %s).  Received GUID: %s",
            wszExpectedGUID, wszActualGUID );

        return E_UNEXPECTED;
    }

    if( !VERIFY_SUCCEEDED( hr = spEtwEventHelper->GetUserDataSize( &cbUserData ) ) ) { return hr; }
    if( !VERIFY_ARE_EQUAL( (USHORT)sizeof(PowerPolicyEvent), cbUserData ) ) { return E_UNEXPECTED; }

    if( !VERIFY_SUCCEEDED( hr = spEtwEventHelper->GetUserData( (LPBYTE)pPowerPolicyEvent, sizeof(PowerPolicyEvent) ) ) ) { return hr; }

    return S_OK;
}
