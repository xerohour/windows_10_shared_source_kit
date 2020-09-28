//--------------------------------------------------------------------------
//
//  File: PortClsEventLogger.h
//
// Copyright (C) Microsoft. All rights reserved.
//
//  History:
//      02/14/2012    CaWerner       Initial implementation.
//
//--------------------------------------------------------------------------
#pragma once

// {9502CBC6-AA74-4eff-BA91-D9329BCCE758}
static const GUID g_PortClsProviderGuid = 
{ 0x9502CBC6, 0xAA74, 0x4eff, { 0xBA, 0x91, 0xD9, 0x32, 0x9B, 0xCC, 0xE7, 0x58 } };

// {47C74B10-D1C0-4E50-ACD1-116E1BD44D3B}
static const GUID g_PortClsPowerPolicyEventGuid = 
{ 0x47C74B10, 0xD1C0, 0x4E50, { 0xAC, 0xD1, 0x11, 0x6E, 0x1B, 0xD4, 0x4D, 0x3B } };

// Power Policy Events
#define PC_ETYPE_PWR_POLICY_LPE_NOTIFICATION        0x70
#define PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE  0x71

enum CS_EVENT_STATE{
    EXITING_CS = 0,
    ENTERING_CS
};

enum EXIT_LATENCY {
    PcExitLatencyInstant = 0,
    PcExitLatencyFast,
    PcExitLatencyResponsive
};

struct PowerPolicyEvent
{
    ULONGLONG EventType;        // PC_ETYPE_PWR_POLICY_LPE_NOTIFICATION or PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE
    ULONGLONG DeviceObjectPtr;  // Pointer to the Device Object
    ULONGLONG Value;            // 1 = Enter, 0 = Exit or New D3 Exit Latency
    ULONGLONG Complete;         // 0 = Beginning, 1 = Completed
    ULONGLONG Reserved1;
    ULONGLONG Reserved2;
};

class PortClsEventLogger
{

public:

    PortClsEventLogger( IBasicLog* pBasicLog ) : m_spBasicLog(pBasicLog), m_isSessionStarted(false), m_isProviderEnabled(false),
        m_isMonitoring(false) {}
    ~PortClsEventLogger();
    HRESULT Initialize( LPCWSTR wsczSessionName );
    HRESULT Start();
    HRESULT Stop();
    HRESULT GetLostEventCount( UINT* pcLostEvents );
    HRESULT GetEventCount( UINT* pcEvents );
    HRESULT GetEvent( UINT iEvent, EtwLogger::IEtwEventHelper** ppEtwEventHelper );
    HRESULT GetPowerPolicyEvent( UINT iEvent, PowerPolicyEvent* pPowerPolicyEvent );

private:

    CComPtr<IBasicLog> m_spBasicLog;
    CComPtr<EtwLogger::IEtwEventLogger> m_spEtwEventLogger;
    bool m_isSessionStarted;
    bool m_isProviderEnabled;
    bool m_isMonitoring;

};
