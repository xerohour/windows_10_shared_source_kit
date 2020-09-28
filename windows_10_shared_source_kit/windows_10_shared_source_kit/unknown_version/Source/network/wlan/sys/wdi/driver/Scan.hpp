/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

Scan.hpp

Abstract:

Header file for the scan handlers

Environment:

Kernel mode

Revision History:

--*/
#pragma once


//==============================================================================================

/*++
Handles bss list update
--*/
class CBSSListUpdateJob : public CJobBase
{
public:
    CBSSListUpdateJob(
        _In_ WFC_ACTIVITY_ID ActivityId);

    ~CBSSListUpdateJob();

    NDIS_STATUS Initialize(
        _In_ CAdapter* pAdapter,
        _In_ NDIS_PORT_NUMBER PortNumber,
        _In_ CBSSListManager *pListManager
        );

    // CJobBase
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject);

    void OnJobCancelled();

private:
    CAdapter*   m_pAdapter;
    DeviceCommand m_DeviceCommand;
    WFC_MESSAGE_METADATA m_UpdateCommandData;
    CBSSListManager *m_pListManager;
    
};


//==============================================================================================

// Structure that consumers of ScanJob would populate
typedef struct _WFC_SCAN_JOB_PARAMETERS
{
    void InitializeDefaults()
    {
        WDI_MAC_ADDRESS broadcastMac = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    
        // Yes this is a totally bogus and somewhat arbitrary assumption, but
        // it is here to keep SAL/PREFIX/PREFAST from complaining.  And yes,
        // somehow telling it that a value is explicitly out of range helps it
        // realize that pSSIDList is NULL, and thus none of it's fields are
        // valid anyway.
        _Analysis_assume_( this->_WFC_SCAN_JOB_PARAMETERS::pSSIDList->uSSIDLength == 100 );

        SSIDListCount = 0;
        pSSIDList = NULL;

        VendorIELength = 0;
        pVendorIEs = NULL;

        ChannelList24GHzCount = 0;
        pChannelList24GHz = NULL;

        ChannelList5GHzCount = 0;
        pChannelList5GHz = NULL;

        RtlZeroMemory(
            &WdiScanParameters,
            sizeof(WDI_SCAN_PARAMETERS));
        WdiScanParameters.ScanModeParameters.ScanRepeatCount = 1;
        WdiScanParameters.DwellTime.ActiveChannelDwellTime = WFC_CONSTANT::DefaultActiveChannelDwellTime;
        WdiScanParameters.DwellTime.PassiveChannelDwellTime = WFC_CONSTANT::DefaultPassiveChannelDwellTime;
        WdiScanParameters.DwellTime.MaximumScanTime = 4000;
        WdiScanParameters.ScanModeParameters.AllowedScanType = WDI_SCAN_TYPE_AUTO;
        WdiScanParameters.ScanModeParameters.ScanTrigger = WDI_SCAN_TRIGGER_BACKGROUND; 

        // Set the broadcast address as the BSSID
        RtlCopyMemory(
            WdiScanParameters.BSSID.Address,
            broadcastMac.Address,
            sizeof(WDI_MAC_ADDRESS));

        // Dont update the BSS list cache on scan complete
        UpdateBSSListCache = false;
        BypassScanThrottling = false;
    }

    PDOT11_SSID pSSIDList;
    ULONG       SSIDListCount;

    PUCHAR      pVendorIEs;
    ULONG       VendorIELength;
    
    WDI_CHANNEL_NUMBER * pChannelList24GHz;
    ULONG ChannelList24GHzCount;

    WDI_CHANNEL_NUMBER * pChannelList5GHz;
    ULONG ChannelList5GHzCount;

    WDI_SCAN_PARAMETERS WdiScanParameters;

    bool        BypassScanThrottling;
    bool        UpdateBSSListCache;

} WFC_SCAN_JOB_PARAMETERS, *PWFC_SCAN_JOB_PARAMETERS;

typedef enum _SCAN_JOB_STATE
{
    WiFiScanJobStateInit,
    WiFiScanJobStateWaitingForScanTaskCompletion,
    WiFiScanJobStateWaitingForBSSCacheUpdate,
    WiFiScanJobStateCompleted
} SCAN_JOB_STATE;

/*++
Handles scan request
--*/
class CScanJob : public CJobBase
{
public:
    CScanJob(
        _In_ WFC_ACTIVITY_ID ActivityId);

    ~CScanJob();

    NDIS_STATUS InitializeFromScanParams(
        _In_ CAdapter* pAdapter,
        _In_ NDIS_PORT_NUMBER PortNumber,
        _In_ WFC_SCAN_JOB_PARAMETERS* pScanParams
        );

    // CJobBase
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject);

    void OnJobCancelled();

    bool IsCancelled()
    {
        return m_IsCancelled;
    }
    
private:
    NDIS_STATUS StartScanTask();

    NDIS_STATUS CompleteScanTask(
        _In_ NDIS_STATUS OperationStatus);

    void FinishJob(
        _In_ NDIS_STATUS JobCompleteStatus);

    bool CScanJob::IsScanAllowed();

private:
    CAdapter*   m_pAdapter;
    NDIS_PORT_NUMBER m_NdisPortNumber;

    Task        m_ScanTask;

    WDI_SCAN_PARAMETERS    m_ScanParameters;

    BYTE*       m_pScanParametersTlvBuffer;
    ULONG       m_ScanParametersTlvLength;
    
    CBSSListManager*        m_pBSSListManager;
    CBSSListUpdateJob       m_UpdateBSSListCacheJob;
    bool        m_UpdateBSSListCache;
    bool        m_BypassScanThrottling;

    bool        m_IsCancelled;
    SCAN_JOB_STATE  m_State;

    ULONGLONG   m_ScanStartTime;
};

//==============================================================================================

/*++
Handles scan request of OID_DOT11_SCAN_REQUEST
--*/
class CScanOidJob : public COidJobBase,
    public ITimerCallback
{
public:
    CScanOidJob(
        _In_ WFC_ACTIVITY_ID ActivityId);

    ~CScanOidJob();

    // COidJobBase
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        );

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus
        );

    // CJobBase
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject);

    void OnJobCancelled();

    //
    // ITimerCallback Implementation
    //
    void OnTimerCallback(
        _In_opt_ WFC_TIMER_CONTEXT TimerContext);

private:
    NDIS_STATUS ValidateScanRequest(
        _In_ PDOT11_SCAN_REQUEST_V2  Dot11ScanRequest,
        _In_ ULONG RequestLength
        );

    NDIS_STATUS StartScanJob();

    NDIS_STATUS CompleteScanJob(
        _In_ NDIS_STATUS OperationStatus,
        _Out_ bool* WasCancelled);

    void FinishJob(
        _In_ NDIS_STATUS JobCompleteStatus);

private:
    PVOID       m_OidRequestId;
    NDIS_PORT_NUMBER m_OidPortNumber;

    WFC_TIMER_REGISTRATION_TOKEN m_TimerToken;
    bool        m_IsCancelled;
    bool        m_IndicateCompletion;

    CScanJob*   m_pScanJob;
    WFC_SCAN_JOB_PARAMETERS  m_ScanJobParameters;

    ULONGLONG   m_ScanOidSubmitTime;
};
//==============================================================================================

/*++
Handles method request of OID_DOT11_ENUM_BSS_LIST
--*/
class CEnumBSSListJob : public COidJobBase
{
public:
    CEnumBSSListJob(
        _In_ WFC_ACTIVITY_ID ActivityId);

    virtual ~CEnumBSSListJob();

    // COidJobBase
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        );

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus
        );

    // CJobBase
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject);

    void OnJobCancelled();

private:
    void CompleteBSSListEnum(
        _In_ NDIS_STATUS NdisStatus
        );

private:
    PVOID       m_pInformationBuffer;
    UINT        m_InputLength;
    UINT        m_OutputLength;
    PUINT       m_pBytesWritten;
    PUINT       m_pBytesNeeded;
    PUINT       m_pBytesRead;

    CBSSListManager *m_pListManager;

    CBSSListUpdateJob m_BSSListUpdateJob;
};

//==============================================================================================

/*++
Handles method request of OID_DOT11_FLUSH_BSS_LIST
--*/
class CFlushBSSListJob : public COidJobBase
{
public:
    CFlushBSSListJob(
        _In_ WFC_ACTIVITY_ID ActivityId);

    virtual ~CFlushBSSListJob();

    // COidJobBase
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        );

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus
        );

    // CJobBase
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject);

    void OnJobCancelled();


private:

    DeviceCommand m_DeviceCommand;
    WFC_MESSAGE_METADATA m_UpdateCommandData;
};


