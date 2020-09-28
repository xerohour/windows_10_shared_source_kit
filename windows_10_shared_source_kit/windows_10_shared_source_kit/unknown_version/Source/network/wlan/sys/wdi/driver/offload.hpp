/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    offload.hpp

Abstract:

    Header file for the scan handlers

Environment:

    Kernel mode

Revision History:

--*/
#pragma once

//==============================================================================================

/*++

Protocol Offload GTK, ARP and NSLookup

OID_PM_ADD_PROTOCOL_OFFLOAD     S
OID_PM_GET_PROTOCOL_OFFLOAD     G
OID_PM_PROTOCOL_OFFLOAD_LIST            G does not come to Miniport
OID_PM_REMOVE_PROTOCOL_OFFLOAD      S

Using NDIS_PM_PROTOCOL_OFFLOAD

    IPv4ARP
    IPv6NS
    GTK (80211RSN_REKEY)

--*/

/*++

    Hanlde OID_PM_ADD_PROTOCOL_OFFLOAD

--*/
class CSetAddPmProtocolOffloadJob : public COidJobBase
{
public:
    CSetAddPmProtocolOffloadJob(
        _In_ WFC_ACTIVITY_ID ActivityId ):
    COidJobBase(ActivityId),
    m_DeviceCommand( ActivityId ) ,
    m_pbCommandBuffer (NULL)
    {};

    ~CSetAddPmProtocolOffloadJob()
    {
        if ( m_pbCommandBuffer )
        {
            FreeGenerated( m_pbCommandBuffer );
            m_pbCommandBuffer = NULL;
        }
    };

    // COidJobBase
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        );

    VOID OnOidJobStartedEvent(
        _In_ NDIS_STATUS NdisStatus
        );

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus
        );
    
    // CJobBase
    void OnJobStarted();
    
    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );
    
    void OnJobCancelled();

    NDIS_STATUS AddOffloadToPropertyCache();

protected:

NDIS_STATUS 
CSetAddPmProtocolOffloadJob::GenerateOffloadTlv(
    _In_ PNDIS_PM_PROTOCOL_OFFLOAD pPmProtocolOffload,
    _In_ ULONG ReserveHeaderSize,
    _Outptr_result_buffer_(* pcbPmProtocolOffload ) BYTE** ppbPmProtocolOffload,
    _Out_ ULONG * pcbPmProtocolOffload,
    _Out_ UINT32 * pcbNeeded );

private:

    NDIS_STATUS StartSetAddPmProtocolOffloadCommand();

    DeviceCommand           m_DeviceCommand;
    PVOID                   m_OidRequestId;
    NDIS_PORT_NUMBER        m_OidPortNumber;
    CPort *                 m_pPort;
    PNDIS_PM_PROTOCOL_OFFLOAD m_pPmProtocolOffload;
    ULONG                   m_cbCommandSize;
    BYTE*                   m_pbCommandBuffer;
};

/*++

    CResumeOffloadJob -for resume firmware context

--*/
class CResumeOffloadJob: public CSetAddPmProtocolOffloadJob
{
public:
    CResumeOffloadJob(
        _In_ WFC_ACTIVITY_ID ActivityId ):
    CSetAddPmProtocolOffloadJob(ActivityId)
    {};

    ~CResumeOffloadJob()
    {};

    NDIS_STATUS InitializeForResume( 
        _In_ CAdapter * pAdapter,
        _In_ NDIS_PORT_NUMBER NdisPortNumber,
        _In_ ULONG ulOidInfoSize,
        _In_ BYTE *pbOidInfoBuffer );

    void OnOidJobCompletedEvent()
    {
        // do nothing, don't complete the OID which is a dummy
        return;
    };

private:
    NDIS_OID_REQUEST m_OidRequest;
};


/*++
Handles set of OID_PM_REMOVE_PROTOCOL_OFFLOAD
--*/
class CSetRemovePmProtocolOffloadJob : public COidJobBase
{
public:
    CSetRemovePmProtocolOffloadJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        COidJobBase(ActivityId),
        m_DeviceCommand( ActivityId )
    {
    };

    ~CSetRemovePmProtocolOffloadJob()
    {
    };

    // COidJobBase
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        );

    VOID OnOidJobStartedEvent(
        _In_ NDIS_STATUS NdisStatus
        );

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus
        );
    
    // CJobBase
    void OnJobStarted();
    
    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );
    
    void OnJobCancelled();

    NDIS_STATUS RemoveOffloadFromPropertyCache();

private:

    NDIS_STATUS StartSetRemovePmProtocolOffloadCommand();

    DeviceCommand           m_DeviceCommand;
    PVOID                   m_OidRequestId;
    NDIS_PORT_NUMBER        m_OidPortNumber;
    CPort *                 m_pPort;
    UINT32                  m_OffloadId;
    static const ULONG      m_cbCommand = sizeof( WFC_MESSAGE_METADATA ) + 8; // TLV header + UINT32
    BYTE                    m_pbCommandBuffer[m_cbCommand];

};


/*++

    Hanlde OID_PM_GET_PROTOCOL_OFFLOAD

--*/
class CGetPmProtocolOffloadJob : public COidJobBase
{
public:
    CGetPmProtocolOffloadJob(
        _In_ WFC_ACTIVITY_ID ActivityId ):
    COidJobBase(ActivityId),
    m_DeviceCommand( ActivityId )
    {};

    ~CGetPmProtocolOffloadJob()
    {
    };

    // COidJobBase
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        );

    VOID OnOidJobStartedEvent(
        _In_ NDIS_STATUS NdisStatus
        );

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus
        );
    
    // CJobBase
    void OnJobStarted();
    
    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    void OnJobCancelled();

protected:

private:

    NDIS_STATUS StartGetPmProtocolOffloadCommand();

    DeviceCommand           m_DeviceCommand;
    PVOID                   m_OidRequestId;
    NDIS_PORT_NUMBER        m_OidPortNumber;
    CPort *                 m_pPort;
    PNDIS_PM_PROTOCOL_OFFLOAD m_pPmProtocolOffload;
    UINT32                  m_OffloadId;
    static const ULONG      m_cbCommand = sizeof( WFC_MESSAGE_METADATA ) + 8; // TLVhdr+ULONG Id
    BYTE                    m_pbCommandBuffer[m_cbCommand];
};



