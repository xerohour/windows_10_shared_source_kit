/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Dot11Oids.hpp

Abstract:

    Header file for the Dot11 OID request handlers

Environment:

    Kernel mode

Revision History:

--*/
#pragma once

//TODO: Remove this when all OIDs are implemented
class CDummySetJob : public CSimpleSetOidJob
{
public:
    CDummySetJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CSimpleSetOidJob(ActivityId)
    {
    }

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        );
};

class CDummyQueryReturnTrueJob : public CSimpleQueryOidJob
{
public:
    CDummyQueryReturnTrueJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};


//==============================================================================================

/*++
Handles query of OID_DOT11_CURRENT_OPERATION_MODE
--*/
class CQueryOperationModeJob : public CSimpleQueryOidJob
{
public:
    CQueryOperationModeJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};


//==============================================================================================

/*++
Handles set of OID_DOT11_CURRENT_OPERATION_MODE
--*/
class CSetOperationModeJob : public COidJobBase
{
public:
    CSetOperationModeJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : COidJobBase(ActivityId),
              m_Task(ActivityId),
              m_pTlvBuffer(NULL)
    {
    }

    ~CSetOperationModeJob()
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

protected:

private:
    WDI_OPERATION_MODE m_OpmodeMask;
    Task                m_Task;
    BYTE*               m_pTlvBuffer;
    ULONG               m_TlvLength;
};

//==============================================================================================

/*++
    Handles set of OID_DOT11_DESIRED_BSS_TYPE
--*/
class CSetDesirerdBssTypeJob : public CSimplePropertyCacheULongSetOidJob
{
public:
    CSetDesirerdBssTypeJob(
        _In_ WFC_ACTIVITY_ID ActivityId )
        : CSimplePropertyCacheULongSetOidJob( WfcPortBssType, dot11_BSS_type_infrastructure, dot11_BSS_type_independent, ActivityId )
    {
    }

    virtual NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        ) override;
};

/*++
Handles set of OID_DOT11_SET_FT_PARAMETERS
--*/
class CSetFTParametersJob : public CSimpleSetOidJob
{
public:
    CSetFTParametersJob(
        _In_ WFC_ACTIVITY_ID ActivityId)
        : CSimpleSetOidJob(ActivityId)
    {
    }

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        );
};

//==============================================================================================

/*++
Handles query of OID_DOT11_CURRENT_PHY_ID
--*/
class CQueryPhyIdJob : public CSimpleQueryOidJob
{
public:
    CQueryPhyIdJob(
        _In_ WFC_ACTIVITY_ID ActivityId) 
            : CSimpleQueryOidJob(ActivityId)
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded);
};

//==============================================================================================

/*++
Handles set of OID_DOT11_CURRENT_PHY_ID
--*/
class CSetPhyIdJob : public CSimpleSetOidJob
{
public:
    CSetPhyIdJob(
        _In_ WFC_ACTIVITY_ID ActivityId) 
            : CSimpleSetOidJob(ActivityId) 
    {
    }

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        )
    {
        NDIS_STATUS ndisStatus;
        ULONG phyID;

        UNREFERENCED_PARAMETER(pInformationBuffer);
        UNREFERENCED_PARAMETER(InformationBufferLength);
        UNREFERENCED_PARAMETER(pBytesRead);
        UNREFERENCED_PARAMETER(pBytesNeeded);
        
        //
        // informationLength is checked in the table
        //
        phyID = *(ULONG*) pInformationBuffer;

        ndisStatus = GetPortPropertyCache()->SetPropertyULong((WFC_PROPERTY_NAME)WfcPortOSConfiguredPhyID,
                                                            phyID);

        return ndisStatus;
    }


};

//==============================================================================================

class CQueryOperationalRateSet : public CSimpleQueryOidJob
{
public:
    CQueryOperationalRateSet(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};

//==============================================================================================

/*++
Handles query of OID_DOT11_PRIVACY_EXEMPTION_LIST
--*/
class CQueryPrivacyExemptionListJob : public CSimpleQueryOidJob
{
public:
    CQueryPrivacyExemptionListJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};

/*++
Handles set of OID_DOT11_PRIVACY_EXEMPTION_LIST
--*/
class CSetPrivacyExemptionListJob : public CSimpleSetDeviceCommandOidJob
{
public:
    CSetPrivacyExemptionListJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleSetDeviceCommandOidJob(ActivityId)    
    {
    }

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
        
};

//==============================================================================================

/*++
Handles set of OID_DOT11_CIPHER_DEFAULT_KEY
--*/
class CSetDefaultKeyJob : public CSimpleSetDeviceCommandOidJob
{
public:
    CSetDefaultKeyJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleSetDeviceCommandOidJob(ActivityId)    
    {
    }

    // Override function COidJobBase
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        );    

    NDIS_STATUS GetDeviceCommandData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded,
        _In_ UINT   ReservedHeaderLength,
        _Out_ ULONG* pCommandId,
        _Out_ ULONG* pCommandBufferLength,
        _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer );

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus );

};


//==============================================================================================

/*++
Handles query of OID_DOT11_CIPHER_DEFAULT_KEY_ID
--*/
class CQueryCipherDefaultKeyIDJob : public CSimpleQueryOidJob
{
public:
    CQueryCipherDefaultKeyIDJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};
/*++
Handles set of OID_DOT11_CIPHER_DEFAULT_KEY_ID
--*/
class CSetDefaultKeyIdJob : public CSimpleSetDeviceCommandOidJob
{
public:
    CSetDefaultKeyIdJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleSetDeviceCommandOidJob(ActivityId)    
    {
    }

    NDIS_STATUS GetDeviceCommandData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded,
        _In_ UINT   ReservedHeaderLength,
        _Out_ ULONG* pCommandId,
        _Out_ ULONG* pCommandBufferLength,
        _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer );
};


//==============================================================================================

/*++
Handles set of OID_DOT11_CIPHER_KEY_MAPPING_KEY
--*/
class CSetKeyMappingKeyJob : public CSimpleSetDeviceCommandOidJob
{
public:
    CSetKeyMappingKeyJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleSetDeviceCommandOidJob(ActivityId),
              m_IsKeyAdded(false)
    {
    }

    // Override function COidJobBase
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        );    

    NDIS_STATUS GetDeviceCommandData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded,
        _In_ UINT   ReservedHeaderLength,
        _Out_ ULONG* pCommandId,
        _Out_ ULONG* pCommandBufferLength,
        _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer );

    // Override OnJobStarted so that we can do some additional work when key gets added
    void OnJobStarted();

protected:
    bool m_IsKeyAdded;
};

//==============================================================================================

/*++
Handles set of OID_DOT11_DESIRED_BSSID_LIST
--*/
class CSetDesiredBSSIDListJob : public CSimpleSetOidJob
{
public:
    CSetDesiredBSSIDListJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleSetOidJob(ActivityId) 
    {
    }

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        );
};

//==============================================================================================

/*++
Handles set of OID_DOT11_ASSOCIATION_PARAMS
--*/
class CSetStaAssociationIEsJob : public CSimpleSetOidJob
{
public:
    CSetStaAssociationIEsJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleSetOidJob(ActivityId) 
    {
    }

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        );
};

//==============================================================================================

/*++
Handles query of OID_DOT11_ENUM_ASSOCIATION_INFO
--*/
class CEnumAssociationInfoJob : public CSimpleQueryOidJob
{
public:
    CEnumAssociationInfoJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};

//==============================================================================================
#if WDI_DISABLED

/*++
Handles set of OID_DOT11_NIC_POWER_STATE
--*/
class CSetNicPowerStateJob : public CSimpleSetOidJob
{
public:
    CSetNicPowerStateJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleSetOidJob(ActivityId) 
    {
    }

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        );
};

/*++
Handles query of OID_DOT11_NIC_POWER_STATE
--*/
class CQueryNicPowerStateJob : public CSimpleQueryOidJob
{
public:
    CQueryNicPowerStateJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};

#endif // WDI_DISABLED

//==============================================================================================

/*++
Handles query of OID_DOT11_OPERATION_MODE_CAPABILITY
--*/
class CQueryOperationModeCapabilityJob : public CSimpleQueryOidJob
{
public:
    CQueryOperationModeCapabilityJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};

//==============================================================================================

/*++
Handles query of OID_802_3_CURRENT_MAC_ADDRESS, OID_DOT11_CURRENT_MAC_ADDRESS
--*/
class CQueryMacAddressJob : public CSimpleQueryOidJob
{
public:
    CQueryMacAddressJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};

//==============================================================================================

/*++
Handles query of OID_802_3_PERMANENT_MAC_ADDRESS, OID_DOT11_PERMANENT_MAC_ADDRESS
--*/
class CQueryHardwareAddressJob : public CSimpleQueryOidJob
{
public:
    CQueryHardwareAddressJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};

//==============================================================================================
/*++
Handles set of OID_DOT11_EXCLUDED_MAC_ADDRESS_LIST
--*/
class CSetExcludedMacAddressListJob : public CSimpleSetOidJob
{
public:
    CSetExcludedMacAddressListJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleSetOidJob(ActivityId) 
    {
    }

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        );
};

//==============================================================================================

/*++
base class for query of OID_DOT11_SUPPORTED_MULTICAST_ALGORITHM_PAIR / OID_DOT11_SUPPORTED_UNICAST_ALGORITHM_PAIR oids
--*/
class CQuerySupportedAlgorithmPairJobBase : public CSimpleQueryOidJob
{
public:
    CQuerySupportedAlgorithmPairJobBase(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QuerySupportedAlgorithmPairData(
        _In_ WFC_ADAPTER_PROPERTY_NAME AlgoPropertyName,
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded);
};

/*++
Handles query of OID_DOT11_SUPPORTED_MULTICAST_ALGORITHM_PAIR
--*/
class CQuerySupportedMulticastAlgorithmPairJob : public CQuerySupportedAlgorithmPairJobBase
{
public:
    CQuerySupportedMulticastAlgorithmPairJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CQuerySupportedAlgorithmPairJobBase(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded);
};
//==============================================================================================
/*++
Handles query of OID_DOT11_SUPPORTED_UNICAST_ALGORITHM_PAIR
--*/
class CQuerySupportedUnicastAlgorithmPairJob : public CQuerySupportedAlgorithmPairJobBase
{
public:
    CQuerySupportedUnicastAlgorithmPairJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CQuerySupportedAlgorithmPairJobBase(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded);
};

//==============================================================================================
/*++
Handles query of OID_DOT11_EXTSTA_CAPABILITY
--*/
class CQueryExtSTACapabilityJob : public CSimpleQueryOidJob
{
public:
    CQueryExtSTACapabilityJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};

/*++
Handles query of OID_DOT11_STATISTICS
--*/
class CQueryStatisticsJob : public CSimpleQueryDeviceCommandOidJob
{
public:
    CQueryStatisticsJob(
        _In_ WFC_ACTIVITY_ID ActivityId )
        : CSimpleQueryDeviceCommandOidJob(ActivityId)
    {
    }

    NDIS_STATUS GetDeviceCommandId(
        _Out_ ULONG* pCommandId);

    NDIS_STATUS OnDeviceCommandCompletion(
        _In_ NDIS_STATUS OperationStatus,
        _In_ ULONG CommandBufferLength,
        _In_reads_bytes_(CommandBufferLength) BYTE* pCommandBuffer,
        _In_ PVOID pInformationBuffer,
        _In_ UINT InformationBufferLength,
        _Inout_ PUINT pBytesWritten,
        _Inout_ PUINT pBytesNeeded);
};

//==============================================================================================
/*++
Handles query of OID_DOT11_REG_DOMAINS_SUPPORT_VALUE
--*/
class CQueryRegDomainsSupportJob : public CSimpleQueryOidJob
{
public:
    CQueryRegDomainsSupportJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};

//==============================================================================================

/*++
Handles query of OID_DOT11_CURRENT_REG_DOMAIN
--*/
class CQueryCurrentRegDomainJob : public CSimpleQueryOidJob
{
public:
    CQueryCurrentRegDomainJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};


//==============================================================================================

/*++
Handles set of OID_DOT11_CURRENT_OPERATION_MODE
--*/
class CP2PDiscoverCancelJob : public CSimpleSetOidJob
{
public:
    CP2PDiscoverCancelJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleSetOidJob(ActivityId) 
    {
    }

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        );
};

//==============================================================================================


/*++
Handles connection quality update requests
--*/
class CUpdateConnectionQualityJob : public CJobBase
{
public:
    CUpdateConnectionQualityJob(
        _In_ WFC_ACTIVITY_ID ActivityId);

    ~CUpdateConnectionQualityJob();

    NDIS_STATUS InitializeFromJob(
        _In_ CAdapter* pAdapter,
        _In_ NDIS_PORT_NUMBER PortNumber,
        _In_ BOOLEAN   ForceUpdate,
        _In_ WFC_PORT_ID UpdatingWfcPortId,  // Port that triggered the update
        _In_ WDI_CONNECTION_QUALITY_HINT ConnectionQualityTrigger,  // Connection Quality being triggere
        _In_ BOOLEAN   EnablingTrigger
        );

    // CJobBase
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject);

    void OnJobCancelled();

private:
    void FinishJob(
        _In_ NDIS_STATUS OperationStatus);

    NDIS_STATUS DetermineConnectionQuality(
        _In_ WFC_PORT_ID WfcPortId,
        _Out_ WDI_CONNECTION_QUALITY_HINT *pConnectionQuality
        );

    NDIS_STATUS StartConnectionQualityUpdateOnPort(
        _In_ WFC_PORT_ID WfcPortId,
        _In_ WDI_CONNECTION_QUALITY_HINT ConnectionQuality,
        _Out_ bool* pbUpdateStarted
        );

    NDIS_STATUS CompleteConnectionQualityUpdateOnPort(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_PORT_ID WfcPortId,
        _In_ WDI_CONNECTION_QUALITY_HINT ConnectionQuality
        );

private:
    CAdapter*   m_pAdapter;
    WDI_CONNECTION_QUALITY_HINT m_ParentConnectionQualityTrigger;
    BOOLEAN     m_ParentEnablingTrigger;
    WFC_PORT_ID m_ParentPortId;
    BOOLEAN     m_ForceUpdate;
    
    DeviceCommand m_DeviceCommand;
    ULONG       m_CommandSize;
    BYTE*       m_pCommandBuffer;
    WFC_PORT_ID  m_CommandPortId;
    WDI_CONNECTION_QUALITY_HINT       m_CommandConnectionQuality;

    
};

//==============================================================================================

class CQueryMediaStreamingEnabledOidJob : public CSimplePropertyCacheBooleanQueryOidJob
{
public:
    CQueryMediaStreamingEnabledOidJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CSimplePropertyCacheBooleanQueryOidJob(
            WfcPortPropertyMediaStreamingEnabled,
            ActivityId )
    {

    }
};

class CSetMediaStreamingEnabledOidJob : public COidJobBase
{
public:
    CSetMediaStreamingEnabledOidJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        COidJobBase(ActivityId),
        m_ConnectionQualityJob(ActivityId)

    {

    }

    ~CSetMediaStreamingEnabledOidJob()
    {

    }

public:
    //
    // CJobBase Implementation
    //
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    void OnJobCancelled() { }

public:
    //
    // COidJobBase Implementation
    //
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        );

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus
        );

private:
    CUpdateConnectionQualityJob m_ConnectionQualityJob;
    BOOLEAN m_bEnableMediaStreaming;

};

//==============================================================================================
/*++
Handles set of OID_DOT11_PMKID_LIST
--*/
class CSetPMKIDList : public CSimplePropertyCacheListSetOidJob
{
public:
    CSetPMKIDList(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimplePropertyCacheListSetOidJob(
                WfcPortPropertyPMKIDList,
                NDIS_OBJECT_TYPE_DEFAULT,
                DOT11_PMKID_LIST_REVISION_1,
                sizeof(DOT11_PMKID_LIST),
                FIELD_OFFSET(DOT11_PMKID_LIST, uTotalNumOfEntries),
                FIELD_OFFSET(DOT11_PMKID_LIST, uNumOfEntries),
                FIELD_OFFSET(DOT11_PMKID_LIST, PMKIDs),
                sizeof(DOT11_PMKID_ENTRY),
                0,
                WFC_CONSTANT::PMKIDCacheSize,
                ActivityId )
    {
    }

public:
    //
    // COidJobBase Implementation
    //

    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus
        );
        
};

//==============================================================================================

/*++
Handles set of OID_DOT11_NIC_SPECIFIC_EXTENSION
--*/

class CNicSpecificExtensionJob : public CSimpleMethodDeviceCommandOidJob
{
public:
    CNicSpecificExtensionJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleMethodDeviceCommandOidJob(ActivityId)    
    {
    }

protected:
    virtual NDIS_STATUS GetDeviceCommandData(
        _In_reads_bytes_(InputBufferLength) PVOID             pInputBuffer,
        _In_ UINT              InputBufferLength,
        _Inout_ PUINT          pBytesRead,
        _Inout_ PUINT          pBytesNeeded,
        _In_ UINT              ReservedHeaderLength,
        _Out_ ULONG*           pCommandId,
        _Out_ ULONG*           pCommandBufferLength,
        _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer);

    virtual NDIS_STATUS OnDeviceCommandCompletion(
        _Out_writes_bytes_(OutputBufferLength) PVOID             pOutputBuffer,
        _In_ UINT              OutputBufferLength,
        _Inout_ PUINT          pBytesWritten,
        _Inout_ PUINT          pBytesNeeded,    
        _In_ NDIS_STATUS       OperationStatus,
        _In_ ULONG             MessageLength,
        _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader );
};

//==============================================================================================

/*
  Handles the following jobs:
    Query of OID_DOT11_ADAPTER_ENHANCED_CAPABILITIES
    Query of OID_DOT11_ADAPTER_BAND_CAPABILITIES
CGetAdapterEnhancedCapabilitiesJob
CGetAdapterBandCapabilitiesJob
*/

class CGetAdapterCapabilitiesJob : public CSimpleQueryOidJob
{
public:
    CGetAdapterCapabilitiesJob(
        _In_ WFC_ACTIVITY_ID ActivityId )
            : CSimpleQueryOidJob(ActivityId)
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};


//==============================================================================================


/*++
Handles IHV task requests
--*/
class CIhvTaskRequestJob : public CJobBase
{
public:
    CIhvTaskRequestJob(
        _In_ WFC_ACTIVITY_ID ActivityId);

    ~CIhvTaskRequestJob();

    NDIS_STATUS InitializeFromJob(
        _In_ CAdapter* pAdapter,
        _In_ NDIS_PORT_NUMBER PortNumber,
        _In_ WFC_PORT_ID ParentWfcPortId,  // Port/Adapter that triggered the update
        _In_ ULONG RequestBufferLength,
        _In_reads_bytes_(RequestBufferLength) BYTE* RequestBuffer
        );

    // CJobBase
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject);

    void OnJobCancelled();

private:
    void FinishJob(
        _In_ NDIS_STATUS OperationStatus);

    NDIS_STATUS StartIhvTask();

    NDIS_STATUS CompleteIhvTask(
        _In_ NDIS_STATUS OperationStatus
        );

private:
    CAdapter*   m_pAdapter;
    WFC_PORT_ID m_ParentPortId;
    WDI_IHV_TASK_PRIORITY   m_RequestedPriority;
    bool        m_IhvIsPreemptable;
    bool        m_IhvCanPreempt;
    WFC_SERIALIZED_JOB_PRIORITY_SCOPE m_IhvPriorityScope;
    WFC_SERIALIZED_JOB_PRIORITY m_IhvJobPriority;
    bool        m_IsCancelled;

    Task        m_Task;
    ULONG       m_CommandSize;
    BYTE*       m_pCommandBuffer;
};

