/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    radio.hpp

Abstract:

    Header file for the scan handlers

Environment:

    Kernel mode

Revision History:

--*/
#pragma once

//========================================================

/*++

                                                                        Q  S  M
OID_DOT11_NIC_POWER_STATE                      Q  S

--*/


/*++

    Hanlde SET OID_DOT11_NIC_POWER_STATE =====

--*/

typedef enum _JOB_STATE_SET_RADIO {
    JobStateUnspecified,
    JobStateAwaitResetComplete,
    JobStateAwaitSetCommandComplete
} JOB_STATE_SET_RADIO;

class CPort;

class CSetRadioStateJob : public COidJobBase
{
public:
    CSetRadioStateJob(
        _In_ WFC_ACTIVITY_ID ActivityId ):
    COidJobBase(ActivityId),
    m_CDot11ResetJob( ActivityId ),
    m_Task( ActivityId ),
    m_pTlvBuffer(NULL)
    {};

    ~CSetRadioStateJob()
    {
        if (m_pTlvBuffer)
            delete[] m_pTlvBuffer;
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

     NDIS_STATUS InitializeFromJob(
        _In_ CAdapter *pAdapter,
        _In_ NDIS_PORT_NUMBER PortNumber,
        _In_ BOOLEAN RadioState );

protected:

private:
    NDIS_STATUS StartDot11ResetSubJob(
        _In_ CPort * pPort );

    CPort * GetNextPortToReset();

    NDIS_STATUS StartSetRadioStateCommand();

    Task                    m_Task;
    CDot11ResetJob          m_CDot11ResetJob;
    PVOID                   m_OidRequestId;
    NDIS_PORT_NUMBER        m_OidPortNumber;
    CPort *                 m_pPort;
    BOOLEAN                 m_OidRadioState;
    JOB_STATE_SET_RADIO     m_JobState;
    BYTE*                   m_pTlvBuffer;
    ULONG                   m_TlvLength;
    
    int                     m_RadioOffResetLoop;
};

/*++

    Handle GET OID_DOT11_NIC_POWER_STATE or OID_DOT11_HARDWARE_PHY_STATE =====

    We will answer the query from our cashe without asking the firmware

--*/
class CGetRadioStateJob : public CSimpleQueryOidJob
{
public:
    CGetRadioStateJob(
        _In_ WFC_ACTIVITY_ID ActivityId,
        _In_ BOOLEAN bHardwareQuery) 
            : CSimpleQueryOidJob(ActivityId) 
    {
        m_bHardwareQuery = bHardwareQuery;
    };

    ~CGetRadioStateJob()
    {
    };

protected:    
    NDIS_STATUS QueryData(
            _In_ PVOID  pInformationBuffer,
            _In_ UINT   InformationBufferLength,
            _In_ PUINT  pBytesWritten,
            _In_ PUINT  pBytesNeeded );

private:
    BOOLEAN m_bHardwareQuery;
};

