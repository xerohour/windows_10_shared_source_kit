/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    CtlPlane.hpp

Abstract:

This header file defines the service for the client of control transfer on the upper
 side. The control trasnfer manager facilitates commands and indications 
 between the upper class driver and lower.filter driver

Environment:

    Kernel mode

Revision History:

--*/

#pragma once

typedef VOID (*PFN_SEND_CMD_COMPLETE)(
    _In_ PVOID  Context,
    _In_ ULONG  TotalResponseLength,
    _In_reads_bytes_(TotalResponseLength) PWFC_MESSAGE_METADATA pResponse, // Caller owns memory
    _In_ IWdiMessageMemoryManager *pBufferManager
    );

typedef VOID (*PFN_INDICATION_CALLBACK)(
    _In_ PVOID Context,
    _In_ ULONG  TotalIndicationLength,
    _In_reads_bytes_(TotalIndicationLength) PWFC_MESSAGE_METADATA pIndication, // Caller owns memory
    _In_ IWdiMessageMemoryManager *pBufferManager
    );

// Forward declaration
class CAdapter;

class CCtlPlane : public IWdiMessageMemoryManager
{
public:
    CCtlPlane();
    
    ~CCtlPlane();

    NDIS_STATUS Initialize(
        _In_ CAdapter* pAdapter
        );

    void FreeMemory(
        _In_ BYTE* pOutputMessageBuffer
        );
        
    NDIS_STATUS Diagnose();

    VOID CancelCommand();

    NDIS_STATUS SendCommand(
        _In_ PWFC_MESSAGE_METADATA pCommand,
        _In_ PFN_SEND_CMD_COMPLETE SendCmdCompletionRoutine,
        _In_ PVOID pCompletionContext );

    NDIS_STATUS RegisterIndicationCallback(
        _In_ PFN_INDICATION_CALLBACK IndicationCallback,
        _In_ PVOID CallbackContext );

    NDIS_STATUS DeregisterIndicationCallback(
        _In_ PFN_INDICATION_CALLBACK IndicationCallback,
        _In_ PVOID CallbackContext );


    VOID
    OnStatusIndicationFromMiniport(
        _In_ PNDIS_STATUS_INDICATION  StatusIndication
        );

    VOID
    OnOidRequestCompleteFromMiniport(
        _In_  PNDIS_OID_REQUEST       Request,
        _In_  NDIS_STATUS             CompletionStatus
        );

private:
    NDIS_STATUS SendWdiCommand(
        _In_ PWFC_MESSAGE_METADATA pCommand,
        _In_ PFN_SEND_CMD_COMPLETE SendCmdCompletionRoutine,
        _In_ PVOID pCompletionContext );

    VOID
    OnWdiOidRequestCompleteFromMiniport(
        _In_  PNDIS_OID_REQUEST       Request,
        _In_  NDIS_STATUS             CompletionStatus
        );

    NDIS_STATUS ForwardRawOid(
        _In_ PWFC_MESSAGE_METADATA pCommand,
        _In_ PFN_SEND_CMD_COMPLETE SendCmdCompletionRoutine,
        _In_ PVOID pCompletionContext );

    VOID
    OnRawOidRequestCompleteFromMiniport(
        _In_opt_  PNDIS_OID_REQUEST       pOriginalRequest,
        _In_opt_  PNDIS_OID_REQUEST       pClonedRequest,
        _In_  NDIS_STATUS             CompletionStatus
        );


private: 
    CAdapter* m_pAdapter;
    PNDIS_MINIPORT_DRIVER_CHARACTERISTICS m_pMPDriverChars;
    PNDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS m_pMPWdiChars;

    // There can only be a single command pending
    bool m_bRequestPending;
    
    //
    // This will contain a pointer to the last OID that was sent down to LE.
    // If a WDI OID was sent last, it'll contain a pointer to either
    // m_OidRequest (for OIDs sent before reset/recovery) or 
    // m_OidAlternative (for OIDs sent after reset/recovery).
    // If a non-WDI OID was forwarded to LE last, it will contain a pointer to  
    // original NDIS OID request that was forwarded (or the cloned OID
    // in case it is supported by NDIS in the future).
    //
    PNDIS_OID_REQUEST m_pPendingOidRequest;

    bool m_bRequestUsingAllocatedMemory;

    //
    // when cmd timeout, we fail it back to ndis. 
    // start to use alternative oid and alternative buffer
    // to achive full double buffering
    //
    NDIS_OID_REQUEST   m_OidRequest;
    NDIS_OID_REQUEST   m_OidAlternative;
    PVOID m_pCompletionContext;
    PFN_SEND_CMD_COMPLETE m_SendCmdCompletionRoutine;
    PUCHAR m_pDefaultInformationBuffer;

    PVOID  m_pIndicationCallbackContext;
    PFN_INDICATION_CALLBACK m_IndicationCallback;

    WFC_MESSAGE_METADATA m_PendingWdiCommandMetadata;

    //
    // Saved reference to the hung command and its completion routine in case 
    // reset recovery was triggered due to M3 timeout.
    //
    PNDIS_OID_REQUEST      m_pHungOidRequest;
    PFN_SEND_CMD_COMPLETE  m_HungSendCmdCompletionRoutine;

    static const ULONG   MaxInformationBufferLength = 2046;
    
};

