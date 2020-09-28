/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Coalesce.hpp

Abstract:

    Header file for the scan handlers

Environment:

    Kernel mode

Revision History:

--*/
#pragma once

//==============================================================================================

/*++

                                                                        Q  S  M
OID_PACKET_COALESCING_FILTER_MATCH_COUNT     Q 
OID_RECEIVE_FILTER_CLEAR_FILTER                           S
OID_RECEIVE_FILTER_CURRENT_CAPABILITIES           Q
OID_RECEIVE_FILTER_ENUM_FILTERS                              M
OID_RECEIVE_FILTER_HARDWARE_CAPABILITIES        Q
OID_RECEIVE_FILTER_PARAMETERS                                M
OID_RECEIVE_FILTER_SET_FILTER                                  M

--*/

/*++

    Hanlde OID_RECEIVE_FILTER_SET_FILTER =====

--*/
class CSetReceiveCoalescingJob : public COidJobBase
{
public:
    CSetReceiveCoalescingJob(
        _In_ WFC_ACTIVITY_ID ActivityId ):
    COidJobBase(ActivityId),
    m_DeviceCommand( ActivityId ) ,
    m_pbCommandBuffer(NULL)
    {};

    ~CSetReceiveCoalescingJob()
    {
        DtPrint( "dtor Coalesce free TLV cmd %p\n", m_pbCommandBuffer);
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

    NDIS_STATUS AddReceiveCoalesceToPropertyCache();

protected:

NDIS_STATUS 
    GenerateReceiveCoalescingTlv(
        _In_ PNDIS_RECEIVE_FILTER_PARAMETERS pReceiveFilterParams,
        _In_ UINT32 ReserveHeaderSize,
        _Outptr_result_buffer_(* pcbReceiveFilterParams ) BYTE** ppbReceiveFilterFields,
        _Out_ ULONG * pcbReceiveFilterParams,
        _Out_ UINT32 * pcbNeeded );

private:

    NDIS_STATUS StartSetReceiveCoalescingCommand();

    DeviceCommand           m_DeviceCommand;
    PVOID                   m_OidRequestId;
    NDIS_PORT_NUMBER        m_OidPortNumber;
    CPort *                 m_pPort;
    PNDIS_RECEIVE_FILTER_PARAMETERS m_pReceiveFilterParams;
    ULONG                   m_cbCommand;
    BYTE*                   m_pbCommandBuffer;
};


/*++

    CResumeCoalesceJob -for resume firmware context

--*/
class CResumeCoalesceJob: public CSetReceiveCoalescingJob
{
public:
    CResumeCoalesceJob(
        _In_ WFC_ACTIVITY_ID ActivityId ):
    CSetReceiveCoalescingJob(ActivityId)
    {};

    ~ CResumeCoalesceJob()
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

    Hanlde OID_RECEIVE_FILTER_CLEAR_FILTER =====

--*/
class CSetClearReceiveCoalescingJob : public COidJobBase
{
public:
    CSetClearReceiveCoalescingJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
    COidJobBase( ActivityId ),
    m_DeviceCommand( ActivityId )
    {};

    ~CSetClearReceiveCoalescingJob()
    {};

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

    NDIS_STATUS RemoveReceiveCoalesceFromPropertyCache();

private:

    NDIS_STATUS StartSetClearReceiveCoalescingCommand();

    DeviceCommand           m_DeviceCommand;
    PVOID                   m_OidRequestId;
    NDIS_PORT_NUMBER        m_OidPortNumber;
    CPort *                 m_pPort;
    UINT32                  m_FilterId;
    static const UINT32     m_cbCommand = sizeof(WFC_MESSAGE_METADATA)+ 8; // header + UINT32 id
    BYTE                    m_pbCommandBuffer[m_cbCommand];
};

/*++

    Hanlde OID_PACKET_COALESCING_FILTER_MATCH_COUNT =====

--*/
    class CGetReceiveCoalescingMatchCountJob : public COidJobBase
    {
    public:
        CGetReceiveCoalescingMatchCountJob(
            _In_ WFC_ACTIVITY_ID ActivityId ):
        COidJobBase(ActivityId),
        m_DeviceCommand( ActivityId )
        {};
    
        ~CGetReceiveCoalescingMatchCountJob()
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

    private:
    
        NDIS_STATUS StartGetReceiveCoalescingMatchCountCommand();
    
        DeviceCommand           m_DeviceCommand;
        PVOID                   m_OidRequestId;
        NDIS_PORT_NUMBER        m_OidPortNumber;
        CPort *                 m_pPort;
        UINT64 *                m_pOidOutput;
        static const UINT32     m_cbCommand = sizeof(WFC_MESSAGE_METADATA);
        BYTE                    m_pbCommandBuffer[m_cbCommand];
    };



