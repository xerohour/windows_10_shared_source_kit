/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Miniport.cpp

Abstract:

    Implementation the object representing the IHV miniport driver object
    
Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "Miniport.tmh"
#include <wdfminiport.h>

// Declaration using typedef so we can pickup the SAL _Function_class_ annotation
extern "C" MINIPORT_INITIALIZE MPWrapperInitializeEx;
extern "C" MINIPORT_HALT MPWrapperHalt;
extern "C" MINIPORT_SET_OPTIONS MPWrapperSetOptions;
MINIPORT_PAUSE MPWrapperPause;
MINIPORT_RESTART MPWrapperRestart;
MINIPORT_OID_REQUEST MPWrapperOidRequest;
MINIPORT_SEND_NET_BUFFER_LISTS MPWrapperSendNetBufferLists;
MINIPORT_RETURN_NET_BUFFER_LISTS MPWrapperReturnNetBufferLists;
MINIPORT_CANCEL_SEND MPWrapperCancelSendNetBufferLists;
MINIPORT_DEVICE_PNP_EVENT_NOTIFY MPWrapperPnPEventNotify;
MINIPORT_SHUTDOWN MPWrapperShutdown;
MINIPORT_RESET MPWrapperReset;
MINIPORT_CANCEL_OID_REQUEST MPWrapperCancelOidRequest;
MINIPORT_DIRECT_OID_REQUEST MPWrapperDirectOidRequest;
MINIPORT_CANCEL_DIRECT_OID_REQUEST MPWrapperCancelDirectOidRequest;
NDIS_WDI_OPEN_ADAPTER_COMPLETE WDIOpenAdapterCompleteHandler;
NDIS_WDI_CLOSE_ADAPTER_COMPLETE WDICloseAdapterCompleteHandler;

MINIPORT_IDLE_NOTIFICATION        MiniportIdleNotificationHandler;
MINIPORT_CANCEL_IDLE_NOTIFICATION MiniportCancelIdleNotificationHandler;

NDIS_WDI_IDLE_NOTIFICATION_CONFIRM  UeIdleNotificationConfirmHandler;
NDIS_WDI_IDLE_NOTIFICATION_COMPLETE UeIdleNotificationCompleteHandler;

extern "C"
NDIS_STATUS
MPWrapperInitializeEx(
    IN  NDIS_HANDLE                        MiniportAdapterHandle,
    IN  NDIS_HANDLE                        MiniportDriverContext,
    IN  PNDIS_MINIPORT_INIT_PARAMETERS     MiniportInitParameters
    )
/*++
Routine Description:

Arguments:

Return Value:

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;    
    CMiniportDriver *pMiniportDriver = NULL;
    
    TraceEntry();

    pMiniportDriver = g_pWdiDriver->
        GetMiniportDriverFromDriverContext(MiniportDriverContext);
    NT_ASSERT(pMiniportDriver);
    
    ndisStatus = pMiniportDriver->MPInitializeEx(
                    MiniportAdapterHandle, 
                    MiniportInitParameters
                    );
    
    TraceExit(ndisStatus);
    return ndisStatus;
}


extern "C"
VOID
MPWrapperHalt(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  NDIS_HALT_ACTION        HaltAction
    )
/*++
Routine Description:

Arguments:

Return Value:

--*/
{
    CMiniportDriver *pMiniportDriver = NULL;
    CAdapter *pAdapter = NULL;

    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterContext(MiniportAdapterContext);
    NT_ASSERT(pAdapter);
    
    pMiniportDriver = pAdapter->GetMiniportDriver();
    NT_ASSERT(pMiniportDriver);
    
    pMiniportDriver->MPHalt(pAdapter, HaltAction);
    
    TraceExit(NDIS_STATUS_SUCCESS);
}


MP_NDIS_REEUMERATE_HANDLER g_NdisMReenumerateFailedAdapter = NULL;

extern "C"
VOID
MPReenumerateFailedAdapter(
    _In_ NDIS_HANDLE        MiniportAdapterContext
    )
/*++
Routine Description:
    This routine is invoked by NDIS if it has to reset and reenumerate the adapter after detecting a failure.
    This should tell the device to reset itself if necessary (note that I/Os don't have to be paused) 
    and call NdisMReenumerateFailedAdapter. The latter API will cause a surprise removal of the device.

Arguments:
    MiniportAdapterContext - Adapter context

Return Value:

--*/
{
    TraceEntry();
    CAdapter *pAdapter;

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterContext(MiniportAdapterContext);
    NT_ASSERT(pAdapter);

    g_NdisMReenumerateFailedAdapter(pAdapter->GetMiniportAdapterHandle());
    TraceExit(NDIS_STATUS_SUCCESS);
}

NTSTATUS MPLiveDumpCallback(
    _In_ HANDLE ReportHandle,
    _In_ PDBGK_LIVEDUMP_ADDSECONDARYDATA_ROUTINE AddSecondaryDataRoutine,
    _In_ ULONG BugCheckCode,
    _In_opt_ ULONG_PTR P1,  
    _In_opt_ ULONG_PTR P2,  
    _In_opt_ ULONG_PTR P3,  
    _In_opt_ ULONG_PTR P4,  
    _In_opt_ PVOID CallbackContext
    )
{
    NTSTATUS status = NDIS_STATUS_SUCCESS;

    ReportHandle,
    AddSecondaryDataRoutine,
    BugCheckCode,
    P1, P2, P3, P4,
    CallbackContext;

    CAdapter* pAdapter;

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterContext(CallbackContext);
    NT_ASSERT(pAdapter);
    if (NULL == pAdapter)
    {
        //
        // Racing with a halt or shutdown might result in the adapter getting
        // cleaned up before we get called, so just bail rather than AV
        //
        WFCError("MPLiveDumpCallback CallbackContext(%p) does not map to an adapter, not adding any secondary data", CallbackContext);
        goto exit;
    }

    //
    // Added the firmware info to the dump; reuse our wpp control GUID
    //
    AddSecondaryDataRoutine(ReportHandle,
                    &WPP_ThisDir_CTLGUID_WdiLibraryCtlGuid,
                    (PVOID)&pAdapter->m_DebugFirmwareBuffer[0],
                    MAX_DIAGNOSTIC_FIRMWARE_BUFFER_SIZE); // should use the actualsize

    #if 0 // undef when WPP recorder is enabled.
    RECORDER_LOG log;
    PVOID logBuffer;
    ULONG logBufferLength;
    GUID  wppGuid;

    UNREFERENCED_PARAMETER(BugCheckCode);
    UNREFERENCED_PARAMETER(P1);
    UNREFERENCED_PARAMETER(P2);
    UNREFERENCED_PARAMETER(P3);
    UNREFERENCED_PARAMETER(P4);
    UNREFERENCED_PARAMETER(CallbackContext);

    log = WppRecorderLogGetDefault();
    status = WppRecorderLogDumpLiveData(log, &logBuffer, &logBufferLength, &wppGuid);
    if (NT_SUCCESS(status))
    {
        AddSecondaryDataRoutine(ReportHandle, &wppGuid, logBuffer, logBufferLength);
    }

    status = WppRecorderDumpLiveDriverData(&logBuffer, &logBufferLength, &wppGuid);
    if (NT_SUCCESS(status))
    {
        AddSecondaryDataRoutine(ReportHandle, &wppGuid, logBuffer, logBufferLength);
    }
    #endif

exit:

    return status;
}


DBGK_LIVEDUMP_CALLBACK_ROUTINE MPLiveDumpCallback;

NDIS_MINIPORT_REENUMERATE_CHARACTERISTICS g_ReenumerateCharacteristics = 
{
    {
        NDIS_OBJECT_TYPE_MINIPORT_REENUMERATE,
        NDIS_MINIPORT_REENUMERATE_CHARACTERISTICS_REVISION_1,
        NDIS_SIZEOF_MINIPORT_REENUMERATE_CHARACTERISTICS_REVISION_1
    },
    0,
    MPReenumerateFailedAdapter,
    MPLiveDumpCallback
};


NDIS_MINIPORT_SS_CHARACTERISTICS g_UsbSsCharacteristics = 
{
    {
        NDIS_OBJECT_TYPE_MINIPORT_SS_CHARACTERISTICS,
        NDIS_MINIPORT_SS_CHARACTERISTICS_REVISION_1,
        NDIS_SIZEOF_MINIPORT_SS_CHARACTERISTICS_REVISION_1
    },
    0,
    MiniportIdleNotificationHandler,
    MiniportCancelIdleNotificationHandler
};


extern "C"
NDIS_STATUS
MPWrapperSetOptions(
    IN NDIS_HANDLE  NdisDriverHandle,
    IN NDIS_HANDLE  MiniportAdapterContext
    )
/*++
Routine Description:

Arguments:

Return Value:

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CMiniportDriver *pMpDriver = NULL;
    UINT ndisVersion = NdisGetVersion();

    TraceEntry();

    pMpDriver = g_pWdiDriver->GetMiniportDriverFromDriverHandle(NdisDriverHandle);

    if (ndisVersion >= NDIS_RUNTIME_VERSION_640)
    {
        ndisStatus = NdisSetOptionalHandlers(NdisDriverHandle, 
            (PNDIS_DRIVER_OPTIONAL_HANDLERS)&g_ReenumerateCharacteristics);
        if (ndisStatus == NDIS_STATUS_NOT_SUPPORTED)
        {
            WFCError("Fake HCK success NdisSetOptionalHandlers 0x%x", ndisStatus);
            ///ndisStatus = NDIS_STATUS_SUCCESS;
        }

        ndisStatus = NdisSetOptionalHandlers(NdisDriverHandle, 
            (PNDIS_DRIVER_OPTIONAL_HANDLERS)&g_ReenumerateCharacteristics);
        if (ndisStatus == NDIS_STATUS_NOT_SUPPORTED)
        {
            WFCError("Fake HCK success NdisSetOptionalHandlers 0x%x", ndisStatus);
            ///ndisStatus = NDIS_STATUS_SUCCESS;
        }

        ndisStatus = NdisSetOptionalHandlers(NdisDriverHandle, 
            (PNDIS_DRIVER_OPTIONAL_HANDLERS)&g_UsbSsCharacteristics);
        if (ndisStatus == NDIS_STATUS_NOT_SUPPORTED)
        {
            WFCError("NdisSetOptionalHandlers for USB SS 0x%x", ndisStatus);
        }

        //
        // Done ours, pass it down.
        //
        if (pMpDriver->GetNdisCharacteristics()->SetOptionsHandler)
        {
            ndisStatus = pMpDriver->GetNdisCharacteristics()->SetOptionsHandler(
                        NdisDriverHandle,
                        MiniportAdapterContext );
        }
    }

    TraceExit(ndisStatus);
    return ndisStatus;
}


NDIS_STATUS
MPWrapperPause(
    IN  NDIS_HANDLE                         MiniportAdapterContext,
    IN  PNDIS_MINIPORT_PAUSE_PARAMETERS     MiniportPauseParameters
    )
{  
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;    
    CAdapter *pAdapter = NULL;

    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterContext(MiniportAdapterContext);
    NT_ASSERT(pAdapter);

    // Pause the adapter
    ndisStatus = pAdapter->Pause( MiniportPauseParameters );

    TraceExit(ndisStatus);

    return ndisStatus;
}

NDIS_STATUS
MPWrapperRestart(
    IN  NDIS_HANDLE                         MiniportAdapterContext,
    IN  PNDIS_MINIPORT_RESTART_PARAMETERS   MiniportRestartParameters
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;    
    CAdapter *pAdapter = NULL;

    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterContext(MiniportAdapterContext);
    NT_ASSERT(pAdapter);


    // Restart the adapter
    ndisStatus = pAdapter->Restart(MiniportRestartParameters);

    TraceExit(ndisStatus);

    return ndisStatus;
}


VOID
MPWrapperPnPEventNotify(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PNET_DEVICE_PNP_EVENT   NetDevicePnPEvent
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;    
    CAdapter *pAdapter = NULL;
    
    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterContext(MiniportAdapterContext);
    NT_ASSERT(pAdapter);
    
    ndisStatus = pAdapter->HandlePnPEventNotify(NetDevicePnPEvent);
    
    TraceExit(ndisStatus);
}

VOID
MPWrapperShutdown(
    IN NDIS_HANDLE MiniportAdapterContext,
    IN  NDIS_SHUTDOWN_ACTION    ShutdownAction
    )
{
    MINIPORT_SHUTDOWN_HANDLER pShutdownHandler;
    CAdapter *pAdapter = NULL;
    
    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterContext(MiniportAdapterContext);
    NT_ASSERT(pAdapter);

    //
    // Ndis makes this cold call without sync to anything. Nor does ndis change anything operations accrodigly.
    // We, at least, use this to stop sending further wdi commads to LE which some IHV complains after this call.
    // We can't stop commands inflight which would requires queuing shutdown call which risks haning shutdown.
    // It would be lots of work and risk for little benefit because ndis/system will pull the power underneath soon.
    //
    InterlockedIncrement( &pAdapter->m_lShutdown );

    pShutdownHandler = pAdapter->GetMiniportDriver()->GetNdisCharacteristics()->ShutdownHandlerEx;
    if (pShutdownHandler )
    {
        WFCTrace("Invoking IHV's ShutdownHandlerEx handler for %d", ShutdownAction);    
    
        pShutdownHandler(
            pAdapter->GetMiniportAdapterContext(),
            ShutdownAction
            );
    }

    TraceExit(NDIS_STATUS_SUCCESS);
}

NDIS_STATUS
MPWrapperReset(
    IN  NDIS_HANDLE     MiniportAdapterContext,
    OUT PBOOLEAN        AddressingReset
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    MINIPORT_RESET_HANDLER pResetHandler;
    CAdapter *pAdapter = NULL;
    
    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterContext(MiniportAdapterContext);
    NT_ASSERT(pAdapter);

    pResetHandler = pAdapter->GetMiniportDriver()->GetNdisCharacteristics()->ResetHandlerEx;
    if (pResetHandler )
    {
        WFCTrace("Invoking IHV's ResetHandlerEx handler");

        ndisStatus = pResetHandler(
            pAdapter->GetMiniportAdapterContext(),
            AddressingReset
            );
    }
    else
    {
        *AddressingReset = TRUE;
    }

    TraceExit(ndisStatus);

    return ndisStatus;
}

NDIS_STATUS
MPWrapperOidRequest(
    IN  NDIS_HANDLE        MiniportAdapterContext,
    IN  PNDIS_OID_REQUEST  NdisRequest
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;    
    CAdapter *pAdapter = NULL;
    
    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterContext(MiniportAdapterContext);
    NT_ASSERT(pAdapter);
    
    ndisStatus = pAdapter->HandleOidRequest(NdisRequest, false);
    
    TraceExit(ndisStatus);

    return ndisStatus;
}

VOID
MPWrapperCancelOidRequest(
    IN  NDIS_HANDLE            MiniportAdapterContext,
    IN  PVOID                  RequestId
    )
{
    TraceEntry();

    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    UNREFERENCED_PARAMETER(RequestId);

    TraceExit(NDIS_STATUS_SUCCESS);
}

NDIS_STATUS
MPWrapperDirectOidRequest(
    IN  NDIS_HANDLE        MiniportAdapterContext,
    IN  PNDIS_OID_REQUEST  NdisRequest
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;    
    CAdapter *pAdapter = NULL;

    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    UNREFERENCED_PARAMETER(NdisRequest);
   
    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterContext(MiniportAdapterContext);
    NT_ASSERT(pAdapter);
    
    ndisStatus = pAdapter->HandleOidRequest(NdisRequest, true);
    
    TraceExit(ndisStatus);

    return ndisStatus;
}

VOID
MPWrapperCancelDirectOidRequest(
    IN  NDIS_HANDLE            MiniportAdapterContext,
    IN  PVOID                  RequestId
    )
{
    TraceEntry();

    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    UNREFERENCED_PARAMETER(RequestId);

    TraceExit(NDIS_STATUS_SUCCESS);
}

VOID
MPWrapperSendNetBufferLists(
    IN  NDIS_HANDLE         MiniportAdapterContext,
    IN  PNET_BUFFER_LIST    NetBufferList,
    IN  NDIS_PORT_NUMBER    PortNumber,
    IN  ULONG               SendFlags
    )
{
    CAdapter *pAdapter = NULL;
    
    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterContext(MiniportAdapterContext);
    NT_ASSERT(pAdapter);
    
    pAdapter->SendNetBufferLists(NetBufferList,PortNumber,SendFlags);

    TraceExit(NDIS_STATUS_SUCCESS);
}

VOID
MPWrapperReturnNetBufferLists(
    IN  NDIS_HANDLE         MiniportAdapterContext,
    IN  PNET_BUFFER_LIST    NetBufferLists,
    IN  ULONG               ReturnFlags
    )
{
    TraceEntry();

    CAdapter *pAdapter = NULL;
    pAdapter = g_pWdiDriver->GetAdapterFromAdapterContext(MiniportAdapterContext);
    NT_ASSERT( pAdapter );

    pAdapter->ReturnNetBufferLists( NetBufferLists, ReturnFlags );

    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    UNREFERENCED_PARAMETER(NetBufferLists);
    UNREFERENCED_PARAMETER(ReturnFlags);

    TraceExit(NDIS_STATUS_SUCCESS);
}

VOID
MPWrapperCancelSendNetBufferLists(
    IN  NDIS_HANDLE     MiniportAdapterContext,
    IN  PVOID           CancelId
    )
{
    TraceEntry();
    
    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    UNREFERENCED_PARAMETER(CancelId);

    TraceExit(NDIS_STATUS_SUCCESS);
}

void WDIOpenAdapterCompleteHandler(
    _In_ NDIS_HANDLE              MiniportAdapterHandle,
    _In_ NDIS_STATUS              CompletionStatus
    )
{
    TraceEntry();

    CAdapter *pAdapter = NULL;
    pAdapter = g_pWdiDriver->GetAdapterFromAdapterHandle(MiniportAdapterHandle);
    NT_ASSERT( pAdapter );

    pAdapter->OnOpenAdapterCompleted( CompletionStatus );

    TraceExit(CompletionStatus);

}

void WDICloseAdapterCompleteHandler(
    _In_ NDIS_HANDLE              MiniportAdapterHandle,
    _In_ NDIS_STATUS              CompletionStatus
    )
{
    TraceEntry();

    CAdapter *pAdapter = NULL;
    pAdapter = g_pWdiDriver->GetAdapterFromAdapterHandle(MiniportAdapterHandle);
    NT_ASSERT( pAdapter );

    pAdapter->OnCloseAdapterCompleted( CompletionStatus );

    TraceExit(CompletionStatus);
}


NDIS_STATUS
MiniportIdleNotificationHandler(
    _In_ NDIS_HANDLE        MiniportAdapterContext,
    _In_ BOOLEAN            ForceIdle )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_BUSY;
    CAdapter *pAdapter = NULL;
    NDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS* pMiniportWdiCharacteristics;
    
    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterContext(MiniportAdapterContext);
    NT_ASSERT(pAdapter);

    //
    // if we are busy internally, dont idle
    //
    if ( pAdapter->Busy())
    {
        WFCInfo("WDI is busy interally, veto IdleNotification");
        goto exit;
    }

    pMiniportWdiCharacteristics = pAdapter->GetMiniportDriver()->GetWdiCharacteristics();

    if ( pMiniportWdiCharacteristics->LeIdleNotificationHandler )
    {
        pAdapter->SetUsbIdleState( WdiUsbIdleStatePending );

        ndisStatus = pMiniportWdiCharacteristics->LeIdleNotificationHandler(
                        MiniportAdapterContext,
                        ForceIdle);

        if ( ndisStatus != STATUS_SUCCESS && ndisStatus != STATUS_PENDING )
        {
            pAdapter->SetUsbIdleState( WdiUsbIdleStateInit );
        }
    }

exit:
    TraceExit(ndisStatus);

    return ndisStatus;
}

VOID
MiniportCancelIdleNotificationHandler(
    _In_ NDIS_HANDLE        MiniportAdapterContext )
{
    CAdapter *pAdapter = NULL;
    NDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS* pMiniportWdiCharacteristics;

    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterContext(MiniportAdapterContext);
    NT_ASSERT(pAdapter);
    pMiniportWdiCharacteristics = pAdapter->GetMiniportDriver()->GetWdiCharacteristics();

    if ( pMiniportWdiCharacteristics->LeCancelIdleNotificationHandler )
    {
        pMiniportWdiCharacteristics->LeCancelIdleNotificationHandler(
                        MiniportAdapterContext );
    }

    pAdapter->SetUsbIdleState(WdiUsbIdleStateInit);

    TraceExit(0);
}


void
UeIdleNotificationConfirmHandler(
    _In_ NDIS_HANDLE                MiniportAdapterHandle,
    _In_ NDIS_DEVICE_POWER_STATE    DeviceIdlePowerState) // This must be D2 for USB
{
    CAdapter *pAdapter = NULL;
    WDI_USB_IDLE_STATE oldState;

    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterHandle(MiniportAdapterHandle);
    NT_ASSERT( pAdapter );

    NT_ASSERT( DeviceIdlePowerState == NdisDeviceStateD2);

    oldState = pAdapter->GetUsbIdleState();
    if ( oldState == WdiUsbIdleStatePending )
    {
        pAdapter->SetUsbIdleState( WdiUsbIdleStateConfirm );
    }
    NdisMIdleNotificationConfirm( pAdapter->GetMiniportAdapterHandle() ,DeviceIdlePowerState);

    TraceExit(0);
}


void
UeIdleNotificationCompleteHandler(
    _In_ NDIS_HANDLE    MiniportAdapterHandle)
{
    TraceEntry();

    CAdapter *pAdapter = NULL;
    pAdapter = g_pWdiDriver->GetAdapterFromAdapterHandle(MiniportAdapterHandle);
    NT_ASSERT( pAdapter );

    pAdapter->SetUsbIdleState( WdiUsbIdleStateInit );

    NdisMIdleNotificationComplete( pAdapter->GetMiniportAdapterHandle() );
    TraceExit(0);
}

//
// Make the version easy to access in kd and kdext.
//
volatile ULONG g_WdiVersion;

CMiniportDriver::CMiniportDriver()
{
    m_DebugLastCreateAdapter = NULL;
    m_NdisMiniportDriverHandle = NULL;
    m_DriverObject = NULL;
    m_bFakeMiniportDriverContext = false;
    RtlZeroMemory(&m_MiniportDriverCharacteristics, sizeof(m_MiniportDriverCharacteristics));
    RtlZeroMemory(&m_MiniportWdiCharacteristics, sizeof(m_MiniportWdiCharacteristics));
    InitializeCppListEntry(this, &m_Link);
    g_WdiVersion = WDI_VERSION_LATEST;
}

CMiniportDriver::~CMiniportDriver()
{
    // Nothing
}

NDIS_STATUS CMiniportDriver::Initialize()
{
    // Nothing since we are not tracking lists of objects
    return NDIS_STATUS_SUCCESS;
}

typedef struct _NON_VERSIONED_NDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS
{
    NDIS_OBJECT_HEADER                       Header;
    MINIPORT_WDI_ALLOCATE_ADAPTER_HANDLER    AllocateAdapterHandler;
    MINIPORT_WDI_FREE_ADAPTER_HANDLER        FreeAdapterHandler;
    MINIPORT_WDI_START_OPERATION_HANDLER     StartOperationHandler;
    MINIPORT_WDI_STOP_OPERATION_HANDLER      StopOperationHandler;
    MINIPORT_WDI_POST_PAUSE_HANDLER          PostPauseHandler;
    MINIPORT_WDI_POST_RESTART_HANDLER        PostRestartHandler;

    MINIPORT_WDI_HANG_DIAGNOSE_HANDLER       HangDiagnoseHandler;
    MINIPORT_WDI_TAL_TXRX_INITIALIZE_HANDLER     TalTxRxInitializeHandler;
    MINIPORT_WDI_TAL_TXRX_DEINITIALIZE_HANDLER   TalTxRxDeinitializeHandler;
} NON_VERSIONED_NDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS, *PNON_VERSIONED_NDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS;


NDIS_STATUS
CMiniportDriver::RegisterWdiMiniportDriver(
	_In_     PDRIVER_OBJECT                              DriverObject,
	_In_     PCUNICODE_STRING                            RegistryPath,
	_In_opt_ NDIS_HANDLE                                 MiniportDriverContext,
	_In_     NDIS_MINIPORT_DRIVER_CHARACTERISTICS const *OriginalMiniportDriverCharacteristics,
	_In_     PNDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS   MiniportWdiCharacteristics,
	_Out_    PNDIS_HANDLE                                NdisMiniportDriverHandle
	)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    NDIS_MINIPORT_DRIVER_CHARACTERISTICS updatedMiniportDriverCharacteristics;

    TraceEntry();
    *NdisMiniportDriverHandle = NULL;

    if (MiniportWdiCharacteristics->WdiVersion < WDI_VERSION_1_0)
    {
        WFCError( "IHV driver WDI Version 0x%08x is less than minimum supported version 0x%08x. Failing load",
            MiniportWdiCharacteristics->WdiVersion, WDI_VERSION_1_0 );
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    // Save the IHV's characteristics (only to the point that we understand)
    RtlCopyMemory(&m_MiniportDriverCharacteristics, 
        OriginalMiniportDriverCharacteristics,
        MIN(OriginalMiniportDriverCharacteristics->Header.Size, sizeof(NDIS_MINIPORT_DRIVER_CHARACTERISTICS)));

    RtlCopyMemory(&m_MiniportWdiCharacteristics, 
        MiniportWdiCharacteristics,
        MIN(MiniportWdiCharacteristics->Header.Size, sizeof(NDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS)));

    // We cannot directly call into these handlers, since Driver Verifier expects
    // these calls to go through NDIS.  Instead, use CNdisHook::InvokeXxx methods.
    m_MiniportDriverCharacteristics.OidRequestHandler = NULL;
    m_MiniportDriverCharacteristics.CancelOidRequestHandler = NULL;
    m_MiniportDriverCharacteristics.DirectOidRequestHandler = NULL;
    m_MiniportDriverCharacteristics.CancelDirectOidRequestHandler = NULL;

    m_DriverObject = DriverObject;
    m_MiniportDriverContext = MiniportDriverContext;
    
    if (m_MiniportDriverContext == NULL)
    {
        WFCInfo("IHV driver did not provide MiniportDriverContext. Using our own");
        m_bFakeMiniportDriverContext = true;
        m_MiniportDriverContext = this;
    }

    RtlZeroMemory(&updatedMiniportDriverCharacteristics,
        sizeof(NDIS_MINIPORT_DRIVER_CHARACTERISTICS));

    // Make a copy such that we are not modifying the miniport's original 
    // structure directly
    RtlCopyMemory(&updatedMiniportDriverCharacteristics,
        OriginalMiniportDriverCharacteristics,
        MIN(OriginalMiniportDriverCharacteristics->Header.Size, sizeof(NDIS_MINIPORT_DRIVER_CHARACTERISTICS)));

    // Populate our init parameters
    WFC_ASSIGN_NDIS_OBJECT_HEADER(m_NdisWdiInitParameters.Header,
        NDIS_OBJECT_TYPE_WDI_INIT_PARAMETERS,
        NDIS_OBJECT_TYPE_WDI_INIT_PARAMETERS_REVISION_1,
        NDIS_SIZEOF_WDI_INIT_PARAMETERS_REVISION_1);

    m_NdisWdiInitParameters.WdiVersion = WDI_VERSION_LATEST;
    m_NdisWdiInitParameters.OpenAdapterCompleteHandler = WDIOpenAdapterCompleteHandler;
    m_NdisWdiInitParameters.CloseAdapterCompleteHandler = WDICloseAdapterCompleteHandler;
    m_NdisWdiInitParameters.UeIdleNotificationConfirm = UeIdleNotificationConfirmHandler;
    m_NdisWdiInitParameters.UeIdleNotificationComplete = UeIdleNotificationCompleteHandler;

    // Modify the handlers that we care about to point to our handlers
    updatedMiniportDriverCharacteristics.InitializeHandlerEx          = MPWrapperInitializeEx;
    updatedMiniportDriverCharacteristics.HaltHandlerEx                = MPWrapperHalt;

    // Not wrapped updatedMiniportDriverCharacteristics.UnloadHandler

    updatedMiniportDriverCharacteristics.SetOptionsHandler            = MPWrapperSetOptions;
    updatedMiniportDriverCharacteristics.PauseHandler                 = MPWrapperPause;
    updatedMiniportDriverCharacteristics.RestartHandler               = MPWrapperRestart;
    updatedMiniportDriverCharacteristics.OidRequestHandler            = MPWrapperOidRequest;
    updatedMiniportDriverCharacteristics.SendNetBufferListsHandler    = MPWrapperSendNetBufferLists;
    updatedMiniportDriverCharacteristics.ReturnNetBufferListsHandler  = MPWrapperReturnNetBufferLists;
    updatedMiniportDriverCharacteristics.CancelSendHandler            = MPWrapperCancelSendNetBufferLists;
    updatedMiniportDriverCharacteristics.DevicePnPEventNotifyHandler  = MPWrapperPnPEventNotify;
    updatedMiniportDriverCharacteristics.ShutdownHandlerEx            = MPWrapperShutdown;
    updatedMiniportDriverCharacteristics.CheckForHangHandlerEx        = NULL; // No check for hang handler
    updatedMiniportDriverCharacteristics.ResetHandlerEx               = MPWrapperReset;
    updatedMiniportDriverCharacteristics.CancelOidRequestHandler      = MPWrapperCancelOidRequest;
    updatedMiniportDriverCharacteristics.DirectOidRequestHandler      = MPWrapperDirectOidRequest;
    updatedMiniportDriverCharacteristics.CancelDirectOidRequestHandler= MPWrapperCancelDirectOidRequest;


    if (!IS_DRIVER_VERSION_650_OR_ABOVE(m_MiniportDriverCharacteristics.MajorNdisVersion,
                                            m_MiniportDriverCharacteristics.MinorNdisVersion)     )
    {
        // Fail loading drivers which are not at least NDIS 6.50
        WFCError("IHV driver is not at least NDIS version 6.50.  Fail loading.  IHV NDIS version %d.%d.  IHV WDI version 0x%0x.",
            m_MiniportDriverCharacteristics.MajorNdisVersion,
            m_MiniportDriverCharacteristics.MinorNdisVersion,
            m_MiniportWdiCharacteristics.WdiVersion);
        goto exit;
    }

    // Call NDIS. Since NDIS would call us back, ensure that we use some driver context
    
    ndisStatus = NdisMRegisterMiniportDriver(DriverObject,
                                         (PUNICODE_STRING)RegistryPath,
                                         m_MiniportDriverContext,
                                         &updatedMiniportDriverCharacteristics,
                                         &m_NdisMiniportDriverHandle);
    if (ndisStatus != NDIS_STATUS_SUCCESS) 
    {
        WFCError("NdisMRegisterMiniportDriver failed. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    *NdisMiniportDriverHandle = m_NdisMiniportDriverHandle;

    NDIS_STRING reenumerateFailedAdapterRoutineName;
    NdisInitUnicodeString(&reenumerateFailedAdapterRoutineName, 
            L"NdisMReenumerateFailedAdapter");
    g_NdisMReenumerateFailedAdapter = (MP_NDIS_REEUMERATE_HANDLER)
            NdisGetRoutineAddress(&reenumerateFailedAdapterRoutineName);

exit:
    TraceExit(ndisStatus);

    return ndisStatus;
}


VOID
CMiniportDriver::DeregisterWdiMiniportDriver()
{
    TraceEntry();
    
    NdisMDeregisterMiniportDriver(m_NdisMiniportDriverHandle);
    m_NdisMiniportDriverHandle = NULL;

    TraceExit(NDIS_STATUS_SUCCESS);
}



NDIS_STATUS
CMiniportDriver::MPInitializeEx(
    _In_  NDIS_HANDLE                        MiniportAdapterHandle,
    _In_  PNDIS_MINIPORT_INIT_PARAMETERS     MiniportInitParameters
    )
/*++
Routine Description:

Arguments:

Return Value:

--*/
{
    NDIS_STATUS            ndisStatus = NDIS_STATUS_SUCCESS;
    CAdapter               *pAdapter = NULL;
    BOOLEAN                adapterInitialized = false;

    TraceEntry();

    // Allocate the adapter
    ndisStatus = g_pWdiDriver->AllocateAdapter(this, &pAdapter);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("CMiniportDriver::AllocateAdapter failed (0x%x)", ndisStatus);
        goto Exit;
    }

    // Let the adapter object do all the initialization
    ndisStatus = pAdapter->Initialize(MiniportAdapterHandle, 
                    MiniportInitParameters, 
                    &m_NdisWdiInitParameters);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("pAdapter->Initialize failed (0x%x)", ndisStatus);
        goto Exit;
    }
    adapterInitialized = true;
    
Exit:

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (pAdapter != NULL) 
        {       
            if (adapterInitialized)
            {
                pAdapter->Halt(NdisHaltDeviceInitializationFailed);
            }
            else
            {
                g_pWdiDriver->ReleaseAdapter(pAdapter);
            }            
        }
    }

    TraceExit(ndisStatus);

    return ndisStatus;
}



VOID
CMiniportDriver::MPHalt(
    _In_  CAdapter*               pAdapter,
    _In_  NDIS_HALT_ACTION        HaltAction
    )
/*++
Routine Description:

Arguments:

Return Value:

--*/
{
    TraceEntry();
   
    pAdapter->Halt(HaltAction);
    
    // Cleanup the adapter
    g_pWdiDriver->ReleaseAdapter(pAdapter);

    TraceExit(NDIS_STATUS_SUCCESS);
}
