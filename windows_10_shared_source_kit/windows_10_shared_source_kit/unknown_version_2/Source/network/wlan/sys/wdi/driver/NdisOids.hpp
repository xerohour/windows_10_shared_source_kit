/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    NdisOids.hpp

Abstract:

    Header file for the NDIS OID handlers

Environment:

    Kernel mode

Revision History:

--*/
#pragma once

//==============================================================================================

/*++
Handles query of OID_GEN_SUPPORTED_GUIDS
--*/
class CQuerySupportedGuidsJob : public CSimpleQueryOidJob
{
public:
    CQuerySupportedGuidsJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId)
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesWritten,
        _In_ PUINT  pBytesNeeded
        );
};

//==============================================================================================

/*++
Handles set of OID_GEN_CURRENT_PACKET_FILTER
--*/
class CSetPacketFilterJob : public CSimpleSetDeviceCommandOidJob
{
public:
    CSetPacketFilterJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleSetDeviceCommandOidJob(ActivityId)    
    {
    }

    NDIS_STATUS GetDeviceCommandData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesRead,
        _In_ PUINT  pBytesNeeded,
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
Handles query of OID_802_3_MULTICAST_LIST
--*/
class CQueryMulticastListJob : public CSimpleQueryOidJob
{
private:
    WFC_PROPERTY_NAME m_MulticastPropertyName;
    
public:
    CQueryMulticastListJob(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ WFC_ACTIVITY_ID ActivityId)
            : CSimpleQueryOidJob(ActivityId),
              m_MulticastPropertyName(PropertyName)
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesWritten,
        _In_ PUINT  pBytesNeeded
        );

};

//==============================================================================================

/*++
Handles set of OID_802_3_MULTICAST_LIST
--*/
class CSetMulticastListJob : public CSimpleSetDeviceCommandOidJob
{
protected:  // so the derived class can modify
    WFC_PROPERTY_NAME m_MulticastPropertyName;
    
public:
    CSetMulticastListJob(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ WFC_ACTIVITY_ID ActivityId)
            : CSimpleSetDeviceCommandOidJob(ActivityId),
              m_MulticastPropertyName(PropertyName)
    {
    }

    NDIS_STATUS GetDeviceCommandData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesRead,
        _In_ PUINT  pBytesNeeded,
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
Handles query of OID_GEN_VENDOR_DESCRIPTION
--*/
class CQueryVendorDescriptionJob : public CSimpleQueryOidJob
{
public:
    CQueryVendorDescriptionJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesWritten,
        _In_ PUINT  pBytesNeeded
        );
};

//==============================================================================================
/*++
Handles query of OID_GEN_VENDOR_DRIVER_VERSION
--*/
class CQueryVendorDriverVersionJob : public CSimpleQueryOidJob
{
public:
    CQueryVendorDriverVersionJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesWritten,
        _In_ PUINT  pBytesNeeded
        );
};

//==============================================================================================

/*++
Handles query of OID_GEN_VENDOR_ID
--*/
class CQueryVendorIDJob : public CSimpleQueryOidJob
{
public:
    CQueryVendorIDJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesWritten,
        _In_ PUINT  pBytesNeeded
        );
};


/*++
Handles OID_TCP_OFFLOAD_PARAMETERS
--*/
class CTcpOffloadSetParametersJob : public CSimpleSetDeviceCommandOidJob
{
public:
    CTcpOffloadSetParametersJob(
        _In_ WFC_ACTIVITY_ID ActivityId)
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
        _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer);
};

/*++
Handles OID_OFFLOAD_ENCAPSULATION
--*/
class CTcpEncapsulationOffloadParametersJob : public CSimpleSetDeviceCommandOidJob
{
public:
    CTcpEncapsulationOffloadParametersJob(
        _In_ WFC_ACTIVITY_ID ActivityId)
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
        _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer);
};

/*++
Handles OID_TCP_OFFLOAD_CURRENT_CONFIG
--*/
class CTcpOffloadCurrentConfigJob : public CSimpleQueryOidJob
{
public:
    CTcpOffloadCurrentConfigJob(
        _In_ WFC_ACTIVITY_ID ActivityId)
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
Handles OID_TCP_OFFLOAD_HARDWARE_CAPABILITIES
--*/
class CTcpOffloadHardwareCapsJob : public CSimpleQueryOidJob
{
public:
    CTcpOffloadHardwareCapsJob(
        _In_ WFC_ACTIVITY_ID ActivityId)
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
Handles OID_TCP_RSC_STATISTICS
--*/
class CTcpOffloadOffloadRscStatisticsJob : public CSimpleQueryDeviceCommandOidJob
{
public:
    CTcpOffloadOffloadRscStatisticsJob(
        _In_ WFC_ACTIVITY_ID ActivityId)
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

This is a job that handles OIDs that we dont handle and want to send 
down to the device.

--*/
class CUnrecognizedOidHandlerJob : public COidJobBase
{
public:
    CUnrecognizedOidHandlerJob(
        _In_ WFC_ACTIVITY_ID ActivityId);

    virtual ~CUnrecognizedOidHandlerJob();

    // COidJobBase
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        );
    
    virtual VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus
        );
    
    // CJobBase
    void OnJobStarted();
    
    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );
    
    void OnJobCancelled();

private:
    WFC_MESSAGE_METADATA  m_CommandHeader;
    DeviceCommand m_DeviceCommand;
    bool m_DirectOid;

};
