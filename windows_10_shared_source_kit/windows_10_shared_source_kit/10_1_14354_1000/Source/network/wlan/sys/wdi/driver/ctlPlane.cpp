/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    ctlPlane.cpp

Abstract:

This  file implement Control transfer subsystem on the upper side.
The control trasnfer manager facilitates commands and indications 
 between the upper class driver and lower.filter driver

Environment:

    Kernel mode

Revision History:

--*/
#include "precomp.hpp"

#include <wdfminiport.h>
#include "ctlPlane.hpp"
#include "ctlPlane.tmh"

CCtlPlane::CCtlPlane() :
    m_pAdapter(NULL),
    m_pMPDriverChars(NULL),
    m_pMPWdiChars(NULL),
    m_bRequestPending(false),
    m_pPendingOidRequest(NULL),
    m_bRequestUsingAllocatedMemory(false),
    m_IndicationCallback(NULL),
    m_pDefaultInformationBuffer(NULL),
    m_pHungOidRequest(NULL)
{

}

CCtlPlane::~CCtlPlane()
{
    if (m_pDefaultInformationBuffer)
    {
        delete[] m_pDefaultInformationBuffer;
    }
}

NDIS_STATUS CCtlPlane::Initialize(
    _In_ CAdapter* pAdapter
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();
    
    WFCTrace("[MSG Initialize] Adapter %p", pAdapter);
   
    m_pAdapter = pAdapter;
    m_pMPDriverChars = pAdapter->GetMiniportDriver()->GetNdisCharacteristics();
    m_pMPWdiChars = pAdapter->GetMiniportDriver()->GetWdiCharacteristics();

    // TODO: This has to work more efficiently
    m_pDefaultInformationBuffer = new UCHAR[CCtlPlane::MaxInformationBufferLength];
    if (m_pDefaultInformationBuffer == NULL)
    {
        WFCError("Failed to allocate memory for information buffer" );        
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }

exit:    
    TraceExit(ndisStatus);
    
    return ndisStatus;
}

void CCtlPlane::FreeMemory(
    _In_ BYTE* pOutputMessageBuffer
    )
{
    delete[] pOutputMessageBuffer;
}

NDIS_STATUS CCtlPlane::RegisterIndicationCallback(
    __in PFN_INDICATION_CALLBACK IndicationCallback,
    __in PVOID CallbackContext )
/*++

    The client register to callback on idnication. There is only one client. The latest one wins.

--*/
{
    m_IndicationCallback = IndicationCallback;
    m_pIndicationCallbackContext = CallbackContext;
    
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS CCtlPlane::DeregisterIndicationCallback(
    __in PFN_INDICATION_CALLBACK IndicationCallback,
    __in PVOID CallbackContext )
/*++

    The client deregister to callback on idnication. There is only one client. The latest one wins.

--*/
{
    UNREFERENCED_PARAMETER(IndicationCallback);
    UNREFERENCED_PARAMETER(CallbackContext);
    
    m_IndicationCallback = NULL;
    m_pIndicationCallbackContext = NULL;

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS CCtlPlane::Diagnose()
{
    return NDIS_STATUS_FAILURE;
}

VOID CCtlPlane::CancelCommand()
{
    TraceEntry();
    
    if (m_bRequestPending)
    {
        g_pWdiDriver->GetNdisHook()->InvokeMiniportCancelOidRequest(
            m_pAdapter->GetMiniportAdapterHandle(),
            m_OidRequest.RequestId
            );
    }

    TraceExit(NDIS_STATUS_SUCCESS);
}

NDIS_STATUS CCtlPlane::SendWdiCommand(
    _In_ PWFC_MESSAGE_METADATA pCommand,
    _In_ PFN_SEND_CMD_COMPLETE SendCmdCompletionRoutine,
    _In_ PVOID pCompletionContext )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PUCHAR pInformationBuffer = NULL;
    ULONG neededOutputLength = 0;
    

    TraceEntry();

    // TODO: We cannot handle dispatch level
    NT_ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);
    //
    //Requests are serialized, there should be no Request pending except when RR
    //
    NT_ASSERT( !m_bRequestPending || m_pAdapter->m_lResetRecovery );
    
    m_bRequestUsingAllocatedMemory = false;

    //
    // if system shutdown (we have recevied MPShutdown), stop requests in the track
    // NDIS calls MPShutdown w/o any sync or stopping calling us. 
    // We could handle in-flight requests, by queue shutdown. But it could hang shutdown
    // even with substantial work. We only stop sending further requests.
    //
    if ( m_pAdapter->m_lShutdown )
    {
        WFCError("Failing message %!MESSAGE_ID! (%d) because of Shutdown (%d)",
            pCommand->MessageId, pCommand->MessageId, m_pAdapter->m_lShutdown);
    
        ndisStatus = NTSTATUS2NDISSTATUS( STATUS_SYSTEM_SHUTDOWN );
        goto exit;
    }

    //
    // Handle special-case Oids
    //
    switch ( pCommand->MessageId )
    {
        //
        // block some commands once we trigger reset recovery
        // this list might expand or shrink
        //
        case WDI_TASK_SCAN:
        case WDI_TASK_CONNECT:
        case WDI_SET_CONNECTION_QUALITY:
        case WDI_TASK_CREATE_PORT:
        case WDI_GET_BSS_ENTRY_LIST:
        {
            if ( m_pAdapter->m_lResetRecovery || m_pAdapter->m_lSurpriseRemove )
            {            
                WFCError("Failing message %!MESSAGE_ID! (%d) because of Reset Recovery (%d) or Surprise Removal (%d)",
                    pCommand->MessageId, pCommand->MessageId, m_pAdapter->m_lResetRecovery, m_pAdapter->m_lSurpriseRemove);
                ndisStatus = NDIS_STATUS_DEVICE_FAILED;
                goto exit;
            }
            break;
        }

        //
        // Check for Direct Oid requests
        //
        case WDI_TASK_P2P_SEND_RESPONSE_ACTION_FRAME:
        case WDI_TASK_SEND_AP_ASSOCIATION_RESPONSE:
        case WDI_SET_FAST_BSS_TRANSITION_PARAMETERS:
        {
            pCommand->UseDirectOidCall = TRUE;
            break;
        }

        default:
            break;
    }

    // Figure out how much output length to make available
    //
    // if Reset Recovery occurs, we may have the wdi OID outstanding but
    // we complete it anyway to unwind RR. The m_OidRequest should not
    // be reused. We will use the alternative oid and always allocate buffer
    //
    NDIS_OID_REQUEST *pOidRequest = &m_OidRequest;
    if ( m_pAdapter->m_lResetRecovery )
    {
        pOidRequest = &m_OidAlternative;
    }

    // Start with whatever is in the command
    neededOutputLength = pCommand->MessageLength;  

    // Did caller ask for more space
    if (pCommand->Command()->MinimumOutputLength > neededOutputLength)
    {
        neededOutputLength = pCommand->Command()->MinimumOutputLength;
    }

    // Use conservative size if caller didnt specify anything
    if (neededOutputLength < CCtlPlane::MaxInformationBufferLength)
        neededOutputLength = CCtlPlane::MaxInformationBufferLength;
    
    // Determine which buffer to use here
    if (neededOutputLength > CCtlPlane::MaxInformationBufferLength ||
        m_pAdapter->m_lResetRecovery )
    {
        m_bRequestUsingAllocatedMemory = true;
        pInformationBuffer = new UCHAR[neededOutputLength];
        if (pInformationBuffer == NULL)
        {
            WFCError("Failed to allocate memory for OID request's information buffer" );
            ndisStatus = NDIS_STATUS_RESOURCES;
            goto exit;
        }        
    }
    else
    {
        pInformationBuffer = m_pDefaultInformationBuffer;
    }

    // If a previous request is still pending and RR has been triggered,
    // the previous OID must be the hung OID. Save its reference in case
    // LE completes the OID late.
    if (m_pAdapter->m_lResetRecovery &&
        m_bRequestPending)
    {
        m_pHungOidRequest = m_pPendingOidRequest;
        m_HungSendCmdCompletionRoutine = m_SendCmdCompletionRoutine;
    }

    m_SendCmdCompletionRoutine = SendCmdCompletionRoutine;
    m_pCompletionContext = pCompletionContext;

    // Get the OID for this command. Save this so that if the task
    // needs to be cancelled, it can be worked on off the DeviceCommand (without
    // needing the Response)
    pCommand->Command()->MappedWdiOid = WDI_DEFINE_OID(pCommand->MessageId);
    
    CMessageHelper::LogMessage(
        1,
        pCommand->MessageId,
        m_pAdapter->GetTlvContext()->PeerVersion,
        pCommand->MessageLength,
        &(pCommand->Message)
        );

    RtlCopyMemory(pInformationBuffer, 
        &(pCommand->Message),
        pCommand->MessageLength
        );

    RtlZeroMemory(pOidRequest, sizeof(NDIS_OID_REQUEST));

    if (IS_DRIVER_VERSION_650_OR_ABOVE(m_pAdapter->GetMiniportDriver()->GetNdisCharacteristics()->MajorNdisVersion,
                                       m_pAdapter->GetMiniportDriver()->GetNdisCharacteristics()->MinorNdisVersion))
    {
        ASSIGN_NDIS_OBJECT_HEADER( pOidRequest->Header,
            NDIS_OBJECT_TYPE_OID_REQUEST,
            NDIS_OID_REQUEST_REVISION_2,
            NDIS_SIZEOF_OID_REQUEST_REVISION_2
            );
    }
    else
    {
        ASSIGN_NDIS_OBJECT_HEADER( pOidRequest->Header,
            NDIS_OBJECT_TYPE_OID_REQUEST,
            NDIS_OID_REQUEST_REVISION_1,
            NDIS_SIZEOF_OID_REQUEST_REVISION_1
            );
    }

    pOidRequest->RequestType = NdisRequestMethod;

    pOidRequest->DATA.METHOD_INFORMATION.Oid = pCommand->Command()->MappedWdiOid;

    pOidRequest->DATA.METHOD_INFORMATION.InformationBuffer = pInformationBuffer;
    pOidRequest->DATA.METHOD_INFORMATION.InputBufferLength = pCommand->MessageLength;
    pOidRequest->DATA.METHOD_INFORMATION.OutputBufferLength = neededOutputLength;
    pOidRequest->DATA.METHOD_INFORMATION.MethodId = 0;
    pOidRequest->DATA.METHOD_INFORMATION.BytesWritten = 0;
    pOidRequest->DATA.METHOD_INFORMATION.BytesRead = 0;
    pOidRequest->DATA.METHOD_INFORMATION.BytesNeeded = 0;
    
    // TODO: Need to fill the parameters of the OID request correctly

    
    m_bRequestPending = true;    
    m_pPendingOidRequest = pOidRequest;

    // Save this object
    RtlCopyMemory(&m_PendingWdiCommandMetadata,
        pCommand,
        sizeof(WFC_MESSAGE_METADATA)
        );

    WFCInfo("[WDIControl] WDI OID_%!MESSAGE_ID! (0x%08x) Submitted", pCommand->MessageId, pCommand->Command()->MappedWdiOid);

    // If we get an error after this point, we dont fail the
    // call, we call the completion with the appropriate status
    if (pCommand->UseDirectOidCall)
    {
        ndisStatus = g_pWdiDriver->GetNdisHook()->InvokeMiniportDirectOidRequest(
            m_pAdapter->GetMiniportAdapterHandle(),
            pOidRequest
            );
    }
    else
    {
        ndisStatus = g_pWdiDriver->GetNdisHook()->InvokeMiniportOidRequest(
            m_pAdapter->GetMiniportAdapterHandle(),
            pOidRequest
            );
    }

    if (ndisStatus != NDIS_STATUS_PENDING)
    {
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("OID request failed inline. Status = 0x%08x", ndisStatus );
        }
        this->OnOidRequestCompleteFromMiniport(
            pOidRequest,
            ndisStatus
            );
        
        // Fix the status to 
        ndisStatus = NDIS_STATUS_SUCCESS;
    }
    else
    {
        // Return status is success
        ndisStatus = NDIS_STATUS_SUCCESS;
    }

exit:

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        //
        // If we have allocated pInformationBuffer AND not called OnOidRequestCompleteFromMiniport yet
        // cleanup
        //
        if ((pInformationBuffer != NULL) && 
            (pInformationBuffer != m_pDefaultInformationBuffer))
        {
            NT_ASSERT( m_bRequestUsingAllocatedMemory || m_pAdapter->m_lResetRecovery );
            delete[] pInformationBuffer;
            m_bRequestUsingAllocatedMemory = false;
        }
    }
    TraceExit(ndisStatus);
    
    return ndisStatus;
}



VOID
CCtlPlane::OnStatusIndicationFromMiniport(
    _In_ PNDIS_STATUS_INDICATION  StatusIndication
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_MESSAGE_METADATA pIndicationData = NULL;
    ULONG indicationDataLength = 0;
    ULONG messageId;

    TraceEntry();

    NT_ASSERT((StatusIndication->StatusCode & WDI_INDICATION_PREFIX) == WDI_INDICATION_PREFIX);
    
    messageId = StatusIndication->StatusCode & 0x0000FFFF;
    
    WFCInfo("[WDIControl] WDI Indication NDIS_STATUS_%!MESSAGE_ID! (0x%08x) Received (Size = %d)", 
        messageId, StatusIndication->StatusCode, StatusIndication->StatusBufferSize);

    if (StatusIndication->StatusBufferSize < sizeof(WDI_MESSAGE_HEADER))
    {
        NT_ASSERT(FALSE);
        WFCError("Indication buffer size %d is less than %d (sizeof(WDI_MESSAGE_HEADER))", 
            StatusIndication->StatusBufferSize, sizeof(WDI_MESSAGE_HEADER));
        return;
    }
    
    CMessageHelper::LogMessage(
        4,
        messageId,
        m_pAdapter->GetTlvContext()->PeerVersion,
        StatusIndication->StatusBufferSize,
        (PWDI_MESSAGE_HEADER)StatusIndication->StatusBuffer
        );

    if (!NT_SUCCESS(RtlULongAdd(
        StatusIndication->StatusBufferSize,
        FIELD_OFFSET(WFC_MESSAGE_METADATA, Message),
        &indicationDataLength)))
    {
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        WFCError("Integer overflow calcuating size of Completion Buffer from NDIS_STATUS_INDICATION StatusBufferSize (%u)", 
            StatusIndication->StatusBufferSize);
        
        NT_ASSERTMSG("Invalid StatusBufferSize specified in Indication", FALSE);
        // There isnt a good way to recover here. So we will bail out and let R&R handle any hang
        goto exit;
    }

    pIndicationData = (PWFC_MESSAGE_METADATA)new UCHAR[indicationDataLength];
    if (pIndicationData == NULL)
    {
        WFCError("Failed to allocate memory for indication buffer" );
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }        

    pIndicationData->Initialize(WfcMessageIndication);

    pIndicationData->MessageId = messageId;
    pIndicationData->MessageLength = StatusIndication->StatusBufferSize;
    pIndicationData->Indication()->MappedWdiIndicationCode = StatusIndication->StatusCode;

    RtlCopyMemory(&pIndicationData->Message, 
        StatusIndication->StatusBuffer,
        StatusIndication->StatusBufferSize
        );
        
    m_IndicationCallback(
        m_pIndicationCallbackContext,
        indicationDataLength,
        pIndicationData,
        (IWdiMessageMemoryManager *)this
        );
        
exit:
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (pIndicationData)
        {
            delete[] pIndicationData;
        }
    }

    TraceExit(ndisStatus);
}


VOID
CCtlPlane::OnWdiOidRequestCompleteFromMiniport(
    _In_  PNDIS_OID_REQUEST       Request,
    _In_  NDIS_STATUS             CompletionStatus
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_MESSAGE_METADATA pCompletionData = NULL;
    ULONG completionDataLength = 0;
    ULONG completionBufferLength = 0;
    ULONG messageId;
    PVOID pCompletionContext;
    PFN_SEND_CMD_COMPLETE pSendCmdCompletionRoutine;
    PVOID pAllocatedOidInformationBuffer = NULL;;

    TraceEntry();

    UNREFERENCED_PARAMETER(CompletionStatus);
    NT_ASSERT(Request == &m_OidRequest || Request == &m_OidAlternative );

    NT_ASSERT((Request->DATA.METHOD_INFORMATION.Oid & WDI_OID_PREFIX) == WDI_OID_PREFIX);
    messageId = Request->DATA.METHOD_INFORMATION.Oid & 0x0000FFFF;

    WFCInfo("[WDIControl] WDI OID_%!MESSAGE_ID! (0x%08x) Completion with Status 0x%08x", 
        messageId,
        Request->DATA.METHOD_INFORMATION.Oid, CompletionStatus);

    m_bRequestPending = false;

    //
    // cases
    // 1. RR uses alternative oid. and always allocates pInfomationBuffer
    // 2. Normal WDI OID allocates infobuffer iff size > max_size
    // 3. We dont allocates info buffer for non-wdi OID 
    //
    // This check is not suffecient with additional RR  feature
    // if (m_bRequestUsingAllocatedMemory)
    //
    if ( Request == &m_OidAlternative ||  // OID after RR, it was always allocated
        ( Request == &m_OidRequest &&     // normal WDI OID m_pDefault... unless different
          Request->DATA.METHOD_INFORMATION.InformationBuffer != m_pDefaultInformationBuffer ))
    {
        // OID call had allocated new memory. Remember the pointer so that when the OID completes, we 
        // can clean this pointer
        pAllocatedOidInformationBuffer = Request->DATA.METHOD_INFORMATION.InformationBuffer;
        m_bRequestUsingAllocatedMemory = false;
    }

    if (CompletionStatus == NDIS_STATUS_SUCCESS)
    {
        NT_ASSERT(Request->DATA.METHOD_INFORMATION.BytesWritten >= sizeof(WDI_MESSAGE_HEADER));
        completionDataLength = Request->DATA.METHOD_INFORMATION.BytesWritten;
    }
    else
    {
        // We dont look at the rest of the data
        NT_ASSERT(Request->DATA.METHOD_INFORMATION.OutputBufferLength >= sizeof(WDI_MESSAGE_HEADER));
        // We update the output status in the headerm
        ((PWDI_MESSAGE_HEADER)Request->DATA.METHOD_INFORMATION.InformationBuffer)->Status = CompletionStatus;
        completionDataLength = sizeof(WDI_MESSAGE_HEADER);
    }
    
    CMessageHelper::LogMessage(
        3,
        messageId,
        m_pAdapter->GetTlvContext()->PeerVersion,
        completionDataLength,
        (PWDI_MESSAGE_HEADER)Request->DATA.METHOD_INFORMATION.InformationBuffer
        );

    if (!NT_SUCCESS(RtlULongAdd(
        completionDataLength,
        FIELD_OFFSET(WFC_MESSAGE_METADATA, Message),
        &completionBufferLength)))
    {
        CompletionStatus = NDIS_STATUS_INVALID_DATA;
        WFCError("Integer overflow calcuating size of Completion Buffer from OID BytesWritten (%u)", 
            Request->DATA.METHOD_INFORMATION.BytesWritten);
        
        NT_ASSERTMSG("Invalid BytesWritten specified in OID", FALSE);
        
        // We can handle this better than the indication path by completing the OID but with a failure status
        completionDataLength = sizeof(WDI_MESSAGE_HEADER);
        ((PWDI_MESSAGE_HEADER)Request->DATA.METHOD_INFORMATION.InformationBuffer)->Status = CompletionStatus;        
        completionBufferLength = sizeof(WDI_MESSAGE_HEADER) + FIELD_OFFSET(WFC_MESSAGE_METADATA, Message);
    }
    
    pCompletionData = (PWFC_MESSAGE_METADATA)new UCHAR[completionBufferLength];
    if (pCompletionData == NULL)
    {
        WFCError("Failed to allocate memory for OID response buffer" );
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }        

    pCompletionData->Initialize(WfcMessageResponse);
    
    // Copy all the flags from the command into the response
    RtlCopyMemory(pCompletionData, 
        &m_PendingWdiCommandMetadata,
        FIELD_OFFSET(WFC_MESSAGE_METADATA, MessageType));

    pCompletionData->Response()->MappedWdiOid = Request->DATA.METHOD_INFORMATION.Oid;
    pCompletionData->MessageId = messageId;
    pCompletionData->MessageLength = completionDataLength;

    // If we failed with status too short, etc, fill that
    if ((CompletionStatus == NDIS_STATUS_BUFFER_OVERFLOW) ||
        (CompletionStatus == NDIS_STATUS_BUFFER_TOO_SHORT) ||
        (CompletionStatus == NDIS_STATUS_INVALID_LENGTH))
    {
        CompletionStatus = NDIS_STATUS_BUFFER_OVERFLOW;
        pCompletionData->Response()->OidBytesNeeded = Request->DATA.METHOD_INFORMATION.BytesNeeded;
    }

    pCompletionData->Response()->OidCompletionStatus = CompletionStatus;

    // Copy the response whatever response we have
    RtlCopyMemory(&pCompletionData->Message, 
        Request->DATA.METHOD_INFORMATION.InformationBuffer,
        completionDataLength
        );

    // Update completion length to incorporate the metadata
    completionDataLength += FIELD_OFFSET(WFC_MESSAGE_METADATA, Message);

    pCompletionContext = m_pCompletionContext;

    if (Request == m_pHungOidRequest) // Hung command completed late by LE
    {
        WFCInfo("[WDIControl] Calling cached hung command completion routine for OID 0x%08x", messageId);
        pSendCmdCompletionRoutine = m_HungSendCmdCompletionRoutine;
        m_HungSendCmdCompletionRoutine = NULL;
    }
    else
    {
        pSendCmdCompletionRoutine = m_SendCmdCompletionRoutine;
        m_SendCmdCompletionRoutine = NULL;
    }

    // Call the originator of the request    
    pSendCmdCompletionRoutine(
        pCompletionContext,
        completionDataLength,
        pCompletionData,
        (IWdiMessageMemoryManager *)this
        );
        
exit:
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (pCompletionData)
        {
            delete[] pCompletionData;
        }
    }

    if (pAllocatedOidInformationBuffer)
    {
        // Needed to free information buffer
        delete[] pAllocatedOidInformationBuffer;
        
    }

    TraceExit(ndisStatus);
}


NDIS_STATUS CCtlPlane::SendCommand(
    _In_ PWFC_MESSAGE_METADATA pCommand,
    _In_ PFN_SEND_CMD_COMPLETE SendCmdCompletionRoutine,
    _In_ PVOID pCompletionContext )
{
    if ( pCommand->NonWdiOid)
    {
        return ForwardRawOid(pCommand,
            SendCmdCompletionRoutine,
            pCompletionContext);
    }
    else
    {
        return SendWdiCommand(pCommand,
            SendCmdCompletionRoutine,
            pCompletionContext);
    }            
}

VOID
CCtlPlane::OnOidRequestCompleteFromMiniport(
    _In_  PNDIS_OID_REQUEST       Request,
    _In_  NDIS_STATUS             CompletionStatus
    )
{
    if (Request == &m_OidRequest || Request == &m_OidAlternative )
    {
        // Wdi request
        OnWdiOidRequestCompleteFromMiniport(Request, 
            CompletionStatus);
    }
    else
    {
        OnRawOidRequestCompleteFromMiniport(Request, 
            NULL,   // The clone couldnt be allocated, so its always NULL
            CompletionStatus);
    }
}
NDIS_STATUS CCtlPlane::ForwardRawOid(
    _In_ PWFC_MESSAGE_METADATA pCommand,
    _In_ PFN_SEND_CMD_COMPLETE SendCmdCompletionRoutine,
    _In_ PVOID pCompletionContext )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;  
    PNDIS_OID_REQUEST pClonedOidRequest = NULL;
    PNDIS_OID_REQUEST pOidRequestToForward = NULL;

    TraceEntry();

    // TODO: We cannot handle dispatch level
    NT_ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);
    NT_ASSERT( !m_bRequestPending || m_pAdapter->m_lResetRecovery );
    
    m_bRequestUsingAllocatedMemory = false;

    // We dont block Raw OIDs during reset recover

    // If a previous request is still pending and RR has been triggered,
    // the previous OID must be the hung OID. Save its reference in case
    // LE completes the OID late.
    if (m_pAdapter->m_lResetRecovery &&
        m_bRequestPending)
    {
        m_pHungOidRequest = m_pPendingOidRequest;
        m_HungSendCmdCompletionRoutine = m_SendCmdCompletionRoutine;
    }

    m_SendCmdCompletionRoutine = SendCmdCompletionRoutine;
    m_pCompletionContext = pCompletionContext;

    WFCTrace("[MSG 1] OID Request 0x%08x", pCommand->Command()->pOriginalOidRequest->DATA.METHOD_INFORMATION.Oid);
    WFCTrace("[MSG]   NDIS Port       = %d", pCommand->Command()->pOriginalOidRequest->PortNumber);

    pOidRequestToForward = pCommand->Command()->pOriginalOidRequest;

    m_bRequestPending = true;
    m_pPendingOidRequest = pOidRequestToForward;
#if 0
    // Clone the original request
    ndisStatus = NdisAllocateCloneOidRequest(
        m_pAdapter->GetMiniportAdapterHandle(),
        pCommand->pOriginalOidRequest,
        WDI_POOL_TAG,
        &pClonedOidRequest
        );
    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {    
        // Save the WFC_MESSAGE_METADATA in the SourceHandle
        RtlCopyMemory(pClonedOidRequest->SourceReserved, pCommand, sizeof(PVOID));

        pClonedOrOriginalRequest = pClonedOidRequest;
    }
    else
    {
        // NDIS doesnt currently allow miniports to call NdisAllocateCloneOidRequest
        // Forward the original OID request down
        pClonedOrOriginalRequest = pCommand->pOriginalOidRequest;
    }
#endif
    
    WFCInfo("[WDIControl] Non WDI OID 0x%08x Submitted", pOidRequestToForward->DATA.METHOD_INFORMATION.Oid);

    // If we get an error after this point, we dont fail the
    // call, we call the completion with the appropriate status
    if (pCommand->UseDirectOidCall)
    {
        ndisStatus = g_pWdiDriver->GetNdisHook()->InvokeMiniportDirectOidRequest(
            m_pAdapter->GetMiniportAdapterHandle(),
            pOidRequestToForward
            );
    }
    else
    {
        ndisStatus = g_pWdiDriver->GetNdisHook()->InvokeMiniportOidRequest(
            m_pAdapter->GetMiniportAdapterHandle(),
            pOidRequestToForward
            );
    }

    if (ndisStatus != NDIS_STATUS_PENDING)
    {
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("OID request failed inline. Status = 0x%08x", ndisStatus );
        }
        this->OnRawOidRequestCompleteFromMiniport(
            pCommand->Command()->pOriginalOidRequest,
            pClonedOidRequest,
            ndisStatus
            );
        
        // Fix the status to 
        ndisStatus = NDIS_STATUS_SUCCESS;
    }
    else
    {
        // Return status is success
        ndisStatus = NDIS_STATUS_SUCCESS;
    }

    TraceExit(ndisStatus);
    
    return ndisStatus;
}

VOID
CCtlPlane::OnRawOidRequestCompleteFromMiniport(
    _In_opt_  PNDIS_OID_REQUEST       pOriginalRequest,
    _In_opt_  PNDIS_OID_REQUEST       pClonedRequest,
    _In_  NDIS_STATUS             CompletionStatus
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_MESSAGE_METADATA pResponseHeader = NULL;
    PVOID pCompletionContext;
    PFN_SEND_CMD_COMPLETE pSendCmdCompletionRoutine;
    PNDIS_OID_REQUEST pClonedOrOriginalRequest = pOriginalRequest ? pOriginalRequest : pClonedRequest;

    TraceEntry();

    WFCInfo("[WDIControl] Non WDI OID 0x%08x Completion with Status 0x%08x", pClonedOrOriginalRequest->DATA.METHOD_INFORMATION.Oid, CompletionStatus);

    m_bRequestPending = false;

    WFCTrace("[MSG 3] OID Response 0x%08x", pClonedOrOriginalRequest->DATA.METHOD_INFORMATION.Oid);
    WFCTrace("[MSG]   NDIS Port       = %d", pClonedOrOriginalRequest->PortNumber);
    WFCTrace("[MSG]   Status          = 0x%x", CompletionStatus);

    pResponseHeader = (PWFC_MESSAGE_METADATA)new UCHAR[sizeof(WFC_MESSAGE_METADATA)];
    if (pResponseHeader == NULL)
    {
        WFCError("Failed to allocate memory for OID response headers" );
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    pResponseHeader->Initialize(WfcMessageResponse);

    pResponseHeader->Response()->OidCompletionStatus = CompletionStatus;
        
    // Fill a dummy WDI_MESSAGE_HEADER
    pResponseHeader->MessageLength = sizeof(WFC_MESSAGE_METADATA) -  FIELD_OFFSET(WFC_MESSAGE_METADATA, Message);

    pCompletionContext = m_pCompletionContext;

    if (pClonedOrOriginalRequest == m_pHungOidRequest) // Hung command completed late by LE
    {
        WFCInfo("[WDIControl] Calling cached hung command completion routine for non WDI OID 0x%08x", 
            pClonedOrOriginalRequest->DATA.METHOD_INFORMATION.Oid);
        pSendCmdCompletionRoutine = m_HungSendCmdCompletionRoutine;
        m_HungSendCmdCompletionRoutine = NULL;
    }
    else
    {
        pSendCmdCompletionRoutine = m_SendCmdCompletionRoutine;
        m_SendCmdCompletionRoutine = NULL;
    }
    
    // Call the originator of the request    
    pSendCmdCompletionRoutine(
        pCompletionContext,
        sizeof(WFC_MESSAGE_METADATA),
        pResponseHeader,
        (IWdiMessageMemoryManager *)this
        );

exit:
    if (pClonedRequest)
    {
        // If we had created the clone, free it
        NdisFreeCloneOidRequest(
            m_pAdapter->GetMiniportAdapterHandle(),            
            pClonedRequest
            );        
    }
    
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (pResponseHeader)
        {
            delete[] pResponseHeader;
        }
    }
        
    TraceExit(ndisStatus);    
}
