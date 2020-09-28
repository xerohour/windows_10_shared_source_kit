/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Power.hpp

Abstract:

    Header file for the scan handlers

Environment:

    Kernel mode

Revision History:

--*/
#pragma once

/*++

Short hand to implement an OID handler Job

1. Implement the OID job for each required methods
    1.1 add TLVs as appro
    1.2 add IOCTL as appro
2. Add an enum of WFC_JOB_TYPE enum in job.hpp
3. Add a switch for the job type in FactoryCreateJob
4. Add OID handler to the table requiest.cpp

--*/

/*++

    Hanlde Auto power save OID
    OID_DOT11_POWER_MGMT_MODE_AUTO_ENABLED

--*/
class CSetAutoPowerSaveJob : public COidJobBase
{
public:
    CSetAutoPowerSaveJob(
        _In_ WFC_ACTIVITY_ID ActivityId ):
    COidJobBase(ActivityId),
    m_ConnectionQualityJob( ActivityId )
    {};

    ~CSetAutoPowerSaveJob()
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

    NDIS_STATUS StartSetAutoPowerSaveCommand();

    PVOID                   m_OidRequestId;
    NDIS_PORT_NUMBER        m_OidPortNumber;
    CPort *                 m_pCPort;
    BOOLEAN                 m_bOidAutoPowerSave;
    CUpdateConnectionQualityJob m_ConnectionQualityJob;

///    static const int        c_CommandSize = sizeof(WFC_MESSAGE_METADATA)+
///                                sizeof(WDI_SET_AUTO_POWER_SAVE_TLV_STRUCT);
///    BYTE                    m_pCommandBuffer[c_CommandSize];
};



/*++

    CReseumAutoPsmJob - subclass of SetAutoPowerSaveJob for resume firmware

--*/
class CResumeAutoPsmJob: public CSetAutoPowerSaveJob
{
public:
    CResumeAutoPsmJob(
        _In_ WFC_ACTIVITY_ID ActivityId ):
        CSetAutoPowerSaveJob(ActivityId)
    {};

    NDIS_STATUS InitializeForResume( 
        _In_ CAdapter * pAdapter,
        _In_ NDIS_PORT_NUMBER NdisPortNumber );

    void OnOidJobCompletedEvent()
    {
        // do nothing, don't complete the OID which is a dummy
        return;
    };

private:
    CAdapter *      m_pAdapter;
    NDIS_PORT_NUMBER    m_NdisPortNumber;
    static const int    c_OidRequestSize = sizeof(NDIS_OID_REQUEST)+
                            sizeof(DOT11_POWER_MGMT_AUTO_MODE_ENABLED_INFO);
    BYTE                m_pbOidAudoPsm[c_OidRequestSize];
};


/*++

Handles request of Get Auto Power Save
how about  OID_DOT11_POWER_MGMT_MODE_STATUS

--*/
class CGetAutoPowerSaveJob : public COidJobBase
{
public:
    CGetAutoPowerSaveJob(
        _In_ WFC_ACTIVITY_ID ActivityId ):
    COidJobBase(ActivityId),
    m_DeviceCommand( ActivityId )
    {};

    ~CGetAutoPowerSaveJob()
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
    
private:

    NDIS_STATUS StartGetAutoPowerSaveCommand();

    DeviceCommand           m_DeviceCommand;
    PVOID                   m_OidRequestId;
    NDIS_PORT_NUMBER        m_OidPortNumber;
    CPort *               m_pCPort;
    BOOLEAN                 m_bOidAutoPowerSave;
    BOOLEAN                 m_OldPortAutoPowerSave;
    static const int        c_CommandSize = sizeof(WFC_MESSAGE_METADATA);
    BYTE                    m_pCommandBuffer[c_CommandSize];
};


/*++

Handle  Set and Get OID_DOT11_POWER_MGMT_REQUEST

If we are in AutoPSM, we ignore all these.
If we are not in AutoPSM, we are in MAX_PERF. Just settle with FW to do MAX_PERF in this case.
No need to do anything to firmware for this.
 
-- */
class CSetPowerMgmtRequestJob : public CSimpleSetOidJob
{
public:
    CSetPowerMgmtRequestJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleSetOidJob(ActivityId) 
    {
    }

    ~CSetPowerMgmtRequestJob()
    {
    };

protected:
    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesRead,
        _In_ PUINT  pBytesNeeded )
    {
        NDIS_STATUS ndisStatus = STATUS_SUCCESS;

        UNREFERENCED_PARAMETER(pInformationBuffer);
        UNREFERENCED_PARAMETER(InformationBufferLength);
        UNREFERENCED_PARAMETER(pBytesRead);
        UNREFERENCED_PARAMETER(pBytesNeeded);


        if ( m_NdisPortNumber != NDIS_DEFAULT_PORT_NUMBER )
        {
            ndisStatus = STATUS_NOT_SUPPORTED;
        }
        else 
        {
            ndisStatus = STATUS_SUCCESS;
        }

        return ndisStatus;
    }
};

class CGetPowerMgmtRequestJob : public CSimpleQueryOidJob
{
public:
    CGetPowerMgmtRequestJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    };

    ~CGetPowerMgmtRequestJob()
    {
    };

protected:
    NDIS_STATUS QueryData(
            _In_ PVOID  pInformationBuffer,
            _In_ UINT   InformationBufferLength,
            _In_ PUINT  pBytesWritten,
            _In_ PUINT  pBytesNeeded )
    {
        NDIS_STATUS  ndisStatus;
        CPort *pPort = NULL;

        if ( m_NdisPortNumber != NDIS_DEFAULT_PORT_NUMBER )
        {
            ndisStatus = STATUS_NOT_SUPPORTED;
            goto exit;
        }

        pPort = m_pAdapter->GetPortFromNdisPortNumber( m_NdisPortNumber );
        NT_ASSERT( pPort );
        
        *pBytesNeeded = sizeof( ULONG );

        if ( InformationBufferLength >= sizeof( ULONG ) )
        {
            if ( pPort->GetPropertyCache()->GetPropertyBooleanOrDefault(WfcPortPropertyAutoPsm , FALSE))
            {
                *(ULONG*)pInformationBuffer = DOT11_POWER_SAVING_MAX_PSP;
            }
            else
            {
                *(ULONG*)pInformationBuffer = DOT11_POWER_SAVING_NO_POWER_SAVING;
            }
            *pBytesWritten = sizeof( ULONG );
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        else 
        {
            //ndisStatus = NTSTATUS2NDISSTATUS(STATUS_BUFFER_TOO_SMALL);
            ndisStatus = STATUS_BUFFER_TOO_SMALL;
        }
    exit:
        return ndisStatus;
    };
};

/*++

    Handle NLO
    OID_DOT11_OFFLOAD_NETWORK_LIST

--*/
class CSetNloJob : public COidJobBase
{
public:
    CSetNloJob(
        _In_ WFC_ACTIVITY_ID ActivityId ):
    COidJobBase(ActivityId),
    m_DeviceCommand( ActivityId )
    {};

    ~CSetNloJob()
    {
        if ( m_pbTlv )
        {
            delete [] m_pbTlv;
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

private:

    NDIS_STATUS StartSetNloCommand();
    NDIS_STATUS SetupInfoForTlvGenerator();

    DeviceCommand           m_DeviceCommand;
    PVOID                   m_OidRequestId;
    NDIS_PORT_NUMBER        m_OidPortNumber;
    CPort *                 m_pCPort;
    PDOT11_OFFLOAD_NETWORK_LIST_INFO m_pOffloadNetworkListInfo;
    //UINT32                  m_CommandSize;
    //BYTE*                   m_pCommandBuffer;

    ULONG                   m_cbTlv; // replace the above pairs
    BYTE*                   m_pbTlv;
};



/*++
Handles set of OID_PM_ADD_WOL_PATTERN
--*/
class CSetAddWolPatternJob : public COidJobBase
{
public:
    CSetAddWolPatternJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
    COidJobBase(ActivityId),
    m_DeviceCommand( ActivityId ),
    m_pbTlv(NULL)
    {
    }

    ~CSetAddWolPatternJob()
    {
        if ( m_pbTlv )
        {
            delete [] m_pbTlv;
            m_pbTlv = NULL;
        }
    };

    NDIS_STATUS SetupInfoForTlvGenerator();

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

    NDIS_STATUS AddWolPatternToPropertyCache();

private:

    NDIS_STATUS StartSetAddWolPatternCommand();

    DeviceCommand           m_DeviceCommand;
    PVOID                   m_OidRequestId;
    NDIS_PORT_NUMBER        m_OidPortNumber;
    CPort *                 m_pCPort;
    PNDIS_PM_WOL_PATTERN    m_pWolPattern;
    //UINT32                  m_CommandSize;
    //BYTE*                   m_pCommandBuffer;;
    ULONG                   m_cbTlv;
    BYTE *                  m_pbTlv;

};


/*++

    CResumeWolPatternJob  for resume firmware context

--*/
class CResumeWolPatternJob: public CSetAddWolPatternJob
{
public:
    CResumeWolPatternJob(
        _In_ WFC_ACTIVITY_ID ActivityId ):
    CSetAddWolPatternJob(ActivityId)
    {};

    ~CResumeWolPatternJob()
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
Handles set of OID_PM_REMOVE_WOL_PATTERN
--*/
class CSetRemoveWolPatternJob : public COidJobBase
{
public:
    CSetRemoveWolPatternJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        COidJobBase(ActivityId),
        m_DeviceCommand( ActivityId )
    {
    };

    ~CSetRemoveWolPatternJob()
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

    NDIS_STATUS RemoveWolPatternFromPropertyCache();

    private:
    NDIS_STATUS StartSetRemoveWolPatternCommand();

    DeviceCommand           m_DeviceCommand;
    PVOID                   m_OidRequestId;
    NDIS_PORT_NUMBER        m_OidPortNumber;
    CPort *                 m_pCPort;
    UINT32                  m_PatternId;
    static const ULONG      m_cbCommand = sizeof( WFC_MESSAGE_METADATA ) + 8; // tlv hdr + UINT32
    BYTE                    m_pbCommandBuffer[m_cbCommand];

};



typedef enum _JOB_STATE_SET_POWER
{
    JobStateNotStarted,
    JobStateAwaitDot11ResetJobComplete,
    JobStateDot11ResetJobCompleted,
    JobStateAwaitReplumbNloCmdComplete,
    JobStateAwaitDisconnectTaskComplete, // if not armed oto wake in Dx disconnect
    JobStateAwaitSetPowerCommandComplete,
    JobStateAwaitPauseCompleteForDx,
    JobStateAwaitCloseFirmwareCompleteForD3,
    JobStateAwaitOpenFirmwareCompleteForD0,
    JobStateAwaitD0ReinitComplete,
    JobStateAwaitD0RoamComplete
} JOB_STATE_SET_POWER;


/*++

    Model after CScanJob without INotifyDeviceIndicationCallback

--*/
class CResumeP2pPortJob: public CJobBase
{
public:
    CResumeP2pPortJob(
        _In_ WFC_ACTIVITY_ID ActivityId ):
        CJobBase( ActivityId ),
        m_CreateMacOidJob ( ActivityId ),
        m_ResumeAutoPsm( ActivityId ),
        m_ResumeAdvSettingJob( ActivityId )
    {
        m_AdvertisementNames[0] = WfcPortPropertyP2PDeviceCaps;
        m_AdvertisementNames[1] = WfcPortPropertyP2PAdditionalIEs;
        m_AdvertisementNames[2] = WfcPortPropertyP2PDeviceInfo;
        m_AdvertisementNames[3] = WfcPortPropertyP2PGroupCaps;
        m_AdvertisementNames[4] = WfcPortPropertyP2PSecondaryDeviceTypeList;
        m_AdvertisementNames[5] = WfcPortPropertyP2PAdvertisementList;
        m_AdvertisementNames[6] = WfcPortPropertyP2PBackgroundDiscoverySettings;
    };

    ~CResumeP2pPortJob()
    {};

    NDIS_STATUS InitializeForResume(
        _In_ CAdapter* pAdapter,
        _In_ NDIS_PORT_NUMBER PortNumber
        );

    // CJobBase
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    void OnJobCancelled()
    {};

    NDIS_STATUS 
    CResumeP2pPortJob::GetNextAdvertisement(
        _Out_ NDIS_OID* pOid,
        _Out_ ULONG* pulInfoBuffer,
        _Out_ BYTE ** ppbInfoBuffer );

protected:
    enum
    {
        ResumeP2pPortStepInitialized,
        ResumeP2pPortStepAwaitCreateMacCompletion,
        ResumeP2pPortStepAwaitAutoPsmCompletion,
        ResumeP2pPortStepAwaitSetAdvCompletion
    } m_ResumeP2pPortStep;

    NDIS_STATUS GetNextAdvertisementName( 
        _Out_ NDIS_OID_REQUEST *pOid,
        _Out_ ULONG *pulInfoBuffer,
        _Out_ BYTE ** ppbInfoBuffer );

    static const ULONG m_cAdvertisementNames = 7;

    WFC_PORT_PROPERTY_NAME m_AdvertisementNames[m_cAdvertisementNames];

private:
    CAdapter*   m_pAdapter;
    NDIS_PORT_NUMBER m_NdisPortNumber;
    CPort *m_pPort;
    ULONG m_ulPortLoop;
    ULONG m_AdvertisementSettingLoop;
    NDIS_OID_REQUEST m_OidRequest;
    CCreateMacOidJob m_CreateMacOidJob;
    CResumeAutoPsmJob m_ResumeAutoPsm;
    CResumeAdvSettingJob m_ResumeAdvSettingJob;
};


//
// Resume privacy exemption list from property cache
//
class CResumePrivacyExemptionJob: public CSimpleSetDeviceCommandOidJob
{
    public:
    CResumePrivacyExemptionJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleSetDeviceCommandOidJob(ActivityId)
    {
    }

    NDIS_STATUS InitializeForResume( 
        _In_ CAdapter * pAdapter,
        _In_ NDIS_PORT_NUMBER NdisPortNumber,
        _In_ ULONG ulSize,
        _In_ UINT16 usEntries,
        _In_ PDOT11_PRIVACY_EXEMPTION pPrivacyExemptions);

    NDIS_STATUS GetDeviceCommandData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded,
        _In_ UINT   ReservedHeaderLength,
        _Out_ ULONG* pCommandId,
        _Out_ ULONG* pCommandBufferLength,
        _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer );

    NDIS_STATUS OnDeviceCommandCompletion(
        _In_ NDIS_STATUS OperationStatus,
        _In_ ULONG MessageLength,
        _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader );

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus )
    {
        NdisStatus;
        //No compltion of OID, since our OID is a make-up
    }

private:
    NDIS_OID_REQUEST m_OidRequest;  // internal makeup oid
};


class CResumeDefaultKeyJob: public CSetDefaultKeyJob
{
    public:
    CResumeDefaultKeyJob( 
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CSetDefaultKeyJob( ActivityId )
    {}

    NDIS_STATUS InitializeForResume(
        _In_ CAdapter * pAdapter,
        _In_ NDIS_PORT_NUMBER NdisPortNumber,
        _In_ ULONG ulKeyIndex);

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus )
    {
        //
        //overwrite to not  complete the OID, since the OID is internal
        //
        NdisStatus;
        ASSERT( NdisStatus == NDIS_STATUS_SUCCESS );
    }

    ULONG GetNextKeyIndex( 
        _In_ CAdapter * pAdapter );

    LONG m_NextKeyIndex;
private:
    NDIS_OID_REQUEST m_OidRequest;
};

class CResumeDefaultKeyIdJob: public CSetDefaultKeyIdJob
{
public:
    CResumeDefaultKeyIdJob( _In_ WFC_ACTIVITY_ID ActivityId ):
        CSetDefaultKeyIdJob( ActivityId )
    {};

    ~CResumeDefaultKeyIdJob()
    {};

    NDIS_STATUS InitializeForResume(
        _In_ CAdapter * pAdapter,
        _In_ NDIS_PORT_NUMBER NdisPortNumber );

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus )
    {
        //
        //overwrite to not  complete the OID, since the OID is internal
        //
        NdisStatus;
        ASSERT( NdisStatus == NDIS_STATUS_SUCCESS );
    }

private:
    NDIS_OID_REQUEST m_OidRequest;
};


//
// Substate of MiniportInitialize job state, currently under 
// MiniportInitializeJobWaitingForSetRadioStateComplete
// Sliptted here so we don't need to mess around with CMiniportInitializeJob
//
typedef enum _JOB_STATE_RESUME_FIRMWARE_CONTEXT
{
    JobStateSubNotStarted,
    JobStateResumeFwAwaitResumePacketFilterComplete,
    JobStateResumeFwAwaitResumeMulticastList8023Complete,
    JobStateResumeFwAwaitResumeMulticastListDot11Complete,
    JobStateResumeFwAwaitResumeAutoPsmComplete,
    JobStateResumeFwAwaitResumeOffloadsStart,
    JobStateResumeFwAwaitResumeOffloadsComplete,
    JobStateResumeFwAwaitResumeWolPatternsComplete,
    JobStateResumeFwAwaitResumeCoalescingComplete,
    JobStateResumeFwAwaitResumePrivacyExemptionComplete,
    JobStateResumeFwAwaitResumeDefaultKeyComplete,
    JobStateResumeFwAwaitResumeDefaultKeyIdComplete,
    JobStateResumeFwAwaitResumeP2pPortComplete,
    JobStateResumeFwAwaitComplete
} JOB_STATE_RESUME_FIRMWARE_CONTEXT;


/*++

    CResumeFirmwareJob - when resume to D0, if firmware was dead (D3Cold),
    We need to re-open the firmware and resume its context. This is in the context
    of D0 OID.

--*/
class CResumeFirmwareJob : public CMiniportInitializeJob
{
public:
    CResumeFirmwareJob(
        _In_ WFC_ACTIVITY_ID ActivityId ):
    CMiniportInitializeJob(ActivityId),
    m_CreatePortJob( ActivityId ),
    m_ResumePacketFilterJob( ActivityId ),
    m_ResumeMulticastListJob( WfcPortProperty8023MulticastList, ActivityId ),
    m_ResumeAutoPsmJob ( ActivityId ),
    m_ResumeOffloadJob ( ActivityId ),
    m_ResumeWolPatternJob( ActivityId ),
    m_ResumeCoalesceJob( ActivityId ),
    m_ResumeP2pPortJob( ActivityId ),
    m_ResumePrivacyExemptionJob( ActivityId ),
    m_ResumeDefaultKeyJob( ActivityId ),
    m_ResumeDefaultKeyIdJob( ActivityId )
    {};

    ~CResumeFirmwareJob()
    {};

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    // Use to initialize from a parent job
    NDIS_STATUS InitializeExplicit(
        _In_ CAdapter *pAdapter );

    //
    // Override StartOpenTask in CMiniportInitializeJob since it has been revised to
    // use callback instead of an OID. But we still need to use an OID to reopen
    // firmware
    //
    NDIS_STATUS StartOpenTask();

    CPort * GetNextP2pPort();

private:
    NDIS_STATUS ResumeFirmwareContext( _Out_ BOOLEAN* pbCompleteJob );

private:
    JOB_STATE_RESUME_FIRMWARE_CONTEXT m_JobStateSub;
    CPropertyCache *  m_pCurrPropertyCache;
    CCreatePortJob      m_CreatePortJob;
    ULONG m_PortLoop;
    CResumePacketFilterJob m_ResumePacketFilterJob;
    CResumeMulticastListJob m_ResumeMulticastListJob;
    CResumeAutoPsmJob m_ResumeAutoPsmJob;
    CResumeOffloadJob m_ResumeOffloadJob;
    CResumeWolPatternJob m_ResumeWolPatternJob;
    CResumeCoalesceJob m_ResumeCoalesceJob;
    CResumePrivacyExemptionJob m_ResumePrivacyExemptionJob;
    CResumeDefaultKeyJob m_ResumeDefaultKeyJob;
    CResumeDefaultKeyIdJob m_ResumeDefaultKeyIdJob;
    CResumeP2pPortJob m_ResumeP2pPortJob;
};


/*++
Handles scan request of OID_PNP_SET_POWER
--*/
class CSetPowerJob : public COidJobBase
{
public:
    CSetPowerJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
    COidJobBase(ActivityId),
    m_CDot11ResetJob( ActivityId ),
    m_PauseJobForD3( ActivityId ),
    m_TaskOpenCloseFirmware( ActivityId ),
    m_DeviceCommand( ActivityId ),
    m_DisconnectJob( ActivityId ),
    //m_RoamJob ( ActivityId ),
    m_ResumeFirmwareJob( ActivityId )
    {};
     
    ~CSetPowerJob()
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

    CPort * GetNextNon0Port();
    NDIS_STATUS StartSetPowerCommand();
    NDIS_STATUS StartDot11ResetSubJob( _In_ CPort *pPort );
    NDIS_STATUS StartPauseJobForD3();
    NDIS_STATUS StartOpenCloseFirmwareTask(_In_ BOOLEAN bToOpen);
    NDIS_STATUS StartReplumbNloCmd( _In_ CPort *pPort);

    void CSetPowerJob::D3DefaultPortArmedStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    void CSetPowerJob::D3DefaultPortNoArmStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    JOB_STATE_SET_POWER      m_JobState;
    CDot11ResetJob      m_CDot11ResetJob;
    CMiniportPauseJob  m_PauseJobForD3;
    Task                    m_TaskOpenCloseFirmware;
    CDisconnectJob       m_DisconnectJob;
    //CRoamReconnectJob                m_RoamJob;
    CResumeFirmwareJob  m_ResumeFirmwareJob;
    WFC_MESSAGE_METADATA   m_CommandBufferOpenCloseFwForD3;
    DeviceCommand        m_DeviceCommand;
    PVOID                   m_OidRequestId;
    NDIS_PORT_NUMBER        m_OidPortNumber;
    CPort *                 m_pCPort;
    NDIS_DEVICE_POWER_STATE m_OidPowerState;
    NDIS_DEVICE_POWER_STATE m_OldPortPowerState;

    // If more fields are added, then you need to update c_CommandSize
    C_ASSERT( sizeof( WDI_SET_POWER_PARAMETERS ) == 
                        sizeof( UINT32 ) + 
                        sizeof( UINT32_CONTAINER ) + 
                        sizeof( WDI_WAKE_EVENT_MASK_CONTAINER ) +
                        sizeof( UINT32 ) );
    static const ULONG      c_CommandSize = sizeof( WFC_MESSAGE_METADATA ) +
                                            WDI_TLV_HEADER_SIZE + sizeof( UINT32_CONTAINER ) + // PowerState
                                            WDI_TLV_HEADER_SIZE + sizeof( WDI_WAKE_EVENT_MASK_CONTAINER ) + // WakeEventMask
                                            WDI_TLV_HEADER_SIZE + sizeof( UINT32_CONTAINER ); // SetPowerDxReason
    BYTE                    m_pCommandBuffer[c_CommandSize];
    ULONG                   m_D2Dot11ResetLoop; // on D2,for port0  we reset all non-port0
};

/*++
Handles scan request of OID_PNP_QUERY_POWER
--*/
class CQueryPowerJob : public COidJobBase
{
public:
    CQueryPowerJob(
        _In_ WFC_ACTIVITY_ID ActivityId ):
    COidJobBase(ActivityId),
    m_DeviceCommand( ActivityId )
    {};

    ~CQueryPowerJob()
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
    
private:

    NDIS_STATUS StartQueryPowerCommand();

    DeviceCommand           m_DeviceCommand;
    PVOID                   m_OidRequestId;
    NDIS_PORT_NUMBER        m_OidPortNumber;
    CPort *                 m_pCPort;
    NDIS_DEVICE_POWER_STATE m_OidPowerState;
    NDIS_DEVICE_POWER_STATE m_OldPortPowerState;
    static const int        c_CommandSize = sizeof(WFC_MESSAGE_METADATA);
    BYTE                    m_pCommandBuffer[c_CommandSize];
};


