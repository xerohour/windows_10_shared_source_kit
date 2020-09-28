/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Request.hpp

Abstract:

    Header file for the generic request handlers

Environment:

    Kernel mode

Revision History:

--*/
#pragma once


//==============================================================================================

/*++
Holds information about each OID that the driver would handle. This would be used to convert from the OID
to a job that would do the real work
--*/
typedef struct _OID_HANDLER_ENTRY {
    NDIS_OID                    Oid;                // Oid value
    
    BOOLEAN                     Queryable:1;        // Queryable in Init/Op state
    BOOLEAN                     Settable:1;         // Settable in Init/Op state
    BOOLEAN                     Method:1;           // Method request
    
    BOOLEAN                     QuerySerialized:1;  // Serialized query
    BOOLEAN                     SetMethodSerialized:1;    // Serialized set
    
    BOOLEAN                     IsPreemptable:1;        
    BOOLEAN                     CanPreempt:1;

    BOOLEAN                     IsDirectOid:1;

    WFC_SERIALIZED_JOB_PRIORITY_SCOPE  PriorityScope;
    WFC_SERIALIZED_JOB_PRIORITY        Priority;
    
    WFC_JOB_TYPE               QueryJobType;       // Job that handles the query request
    WFC_JOB_TYPE               SetMethodJobType;   // Job that handles the Set or Method request

    ULONG                       MinBufferSize;      // Minimum size required for the buffer (query & set)

    WFC_PORT_TYPE               AllowedPortTypes; // Flags for which port types allowed to issue this command
} OID_HANDLER_ENTRY, * POID_HANDLER_ENTRY;

//==============================================================================================

/*++
This is held by the adapter object and handles completion of OID 
--*/
class COidJobNotificationHandler : public IOperationCompletionCallback, public INotifyOperationStarted
{
public:
    COidJobNotificationHandler();

    /*++
        Implementation of INotifyOperationStarted

        SenderObject points to the CJobBase of a COidJobBase
    --*/
    void OnOperationStarted(
        _In_ NDIS_STATUS OperationStatus,
        _In_opt_ WFC_SENDER_OBJECT SenderObject );

    /*++
        Implementation of IOperationCompletionCallback

        SenderObject points to the CJobBase of a COidJobBase
    --*/
    void OnOperationCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_opt_ WFC_SENDER_OBJECT SenderObject );

};

//
// Macros for assigning and verifying NDIS_OBJECT_HEADER
//
#define ASSIGN_NDIS_OBJECT_HEADER(_header, _type, _revision, _size) \
    (_header).Type = _type; \
    (_header).Revision = _revision; \
    (_header).Size = _size; 


#define ASSIGN_NDIS_OBJECT_HEADER_PTR(_header, _type, _revision, _size) \
    (_header)->Type = _type; \
    (_header)->Revision = _revision; \
    (_header)->Size = _size; 

// 
// With NDIS 6.0 header versioning, the driver should allow higher versions
// of structures to be set. This macro verifies that for sets the version is atleast
// the expected one and size is greater or equal to required
//
#define VERIFY_NDIS_OBJECT_HEADER_DEFAULT(_header, _type, _revision, _size) \
    (((_header).Type == _type) && \
     ((_header).Revision >= _revision) && \
     ((_header).Size >= _size))

#define VERIFY_NDIS_OBJECT_HEADER_DEFAULT_PTR(_header, _type, _revision, _size) \
    (((_header)->Type == _type) && \
     ((_header)->Revision >= _revision) && \
     ((_header)->Size >= _size))

//
// This macro performs exact matching of the NDIS_OBJECT_HEADER
//
#define VERIFY_NDIS_OBJECT_HEADER_PERFECT(_header, _type, _revision, _size) \
    (((_header).Type == _type) && \
     ((_header).Revision == _revision) && \
     ((_header).Size == _size))

#define VERIFY_NDIS_OBJECT_HEADER_PERFECT_PTR(_header, _type, _revision, _size) \
    (((_header)->Type == _type) && \
     ((_header)->Revision == _revision) && \
     ((_header)->Size == _size))

#define QUERY_OID_VALUE(_pOid) ((_pOid)->DATA.QUERY_INFORMATION.Oid)
#define QUERY_OID_BUFFER(_pOid) ((_pOid)->DATA.QUERY_INFORMATION.InformationBuffer)
#define QUERY_OID_BUFFER_LENGTH(_pOid) ((_pOid)->DATA.QUERY_INFORMATION.InformationBufferLength)
#define QUERY_OID_BYTES_NEEDED(_pOid) ((_pOid)->DATA.QUERY_INFORMATION.BytesNeeded)
#define QUERY_OID_BYTES_WRITTEN(_pOid) ((_pOid)->DATA.QUERY_INFORMATION.BytesWritten)

#define SET_OID_VALUE(_pOid)   ((_pOid)->DATA.SET_INFORMATION.Oid)
#define SET_OID_BUFFER(_pOid)   ((_pOid)->DATA.SET_INFORMATION.InformationBuffer)
#define SET_OID_BUFFER_LENGTH(_pOid) ((_pOid)->DATA.SET_INFORMATION.InformationBufferLength)
#define SET_OID_BYTES_NEEDED(_pOid) ((_pOid)->DATA.SET_INFORMATION.BytesNeeded)
#define SET_OID_BYTES_READ(_pOid) ((_pOid)->DATA.SET_INFORMATION.BytesRead)

#define METHOD_OID_VALUE(_pOid)   ((_pOid)->DATA.METHOD_INFORMATION.Oid)
#define METHOD_OID_BUFFER(_pOid)   ((_pOid)->DATA.METHOD_INFORMATION.InformationBuffer)
#define METHOD_OID_INPUT_LENGTH(_pOid) ((_pOid)->DATA.METHOD_INFORMATION.InputBufferLength)
#define METHOD_OID_OUTPUT_LENGTH(_pOid) ((_pOid)->DATA.METHOD_INFORMATION.OutputBufferLength)
#define METHOD_OID_BYTES_WRITTEN(_pOid) ((_pOid)->DATA.METHOD_INFORMATION.BytesWritten)
#define METHOD_OID_BYTES_READ(_pOid) ((_pOid)->DATA.METHOD_INFORMATION.BytesRead)
#define METHOD_OID_BYTES_NEEDED(_pOid) ((_pOid)->DATA.METHOD_INFORMATION.BytesNeeded)


/*++
This is the abstract base class for all OID jobs. It provides some basic parser functions, but
the real work is done by the child classes
--*/
class COidJobBase : public CJobBase
{
public:
    COidJobBase(
        _In_ WFC_ACTIVITY_ID ActivityId );

    virtual ~COidJobBase();


    /*++
        Should be called by the OID handler to prepare the object. This looks at
        the contents of the OID and does some pre-checking. 

        Its called outside the event queue, so cannot trust the current adapter state
    --*/
    NDIS_STATUS InitializeForOid(
        _In_ WFC_JOB_TYPE JobType,
        _In_ CAdapter*  pAdapter,
        _In_ POID_HANDLER_ENTRY pHandlerInfo,
        _In_ PNDIS_OID_REQUEST pOidRequest
        );


    /*++
        Used to parse the IE blob that came from the OS. This is 
        called from InitializeOidJob

        Its called outside the event queue, so cannot trust the current adapter state  
    --*/
    virtual NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        ) = 0;

    /*++
        Used to handle the Job Started event fired by the job
    --*/
    virtual VOID OnOidJobStartedEvent(
        _In_ NDIS_STATUS NdisStatus
        );

    /*++
        Used to handle the Job Completed event fired by the job
    --*/
    virtual VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus
        ) = 0;
    
    /*++
        Factory method to create the job for a handler
    --*/
    _Success_( return == NDIS_STATUS_SUCCESS )
    static NDIS_STATUS s_CreateOidJob(
        _In_ WFC_JOB_TYPE JobType,
        _In_ CAdapter* pAdapter,
        _In_ POID_HANDLER_ENTRY pHandlerInfo,
        _In_ PNDIS_OID_REQUEST pOidRequest,
        _Out_ COidJobBase** ppOidJobBase
        );

    static POID_HANDLER_ENTRY s_FindOidHandlerEntry(
        _In_ NDIS_OID NdisOid
        );


    // Method to query the supported OIDs
    static NDIS_STATUS s_GetSupportedOidsList(
        _In_opt_count_(NonWdiOidCount) UINT32 * pNonWdiOidList,
        _In_ ULONG NonWdiOidCount,
        _Outptr_result_buffer_(*pSupportedOidsCount) PNDIS_OID *ppSupportedOidsList, 
        _Out_ ULONG *pSupportedOidsCount
        );
    

    POID_HANDLER_ENTRY GetOidHandlerEntry()
    {
        return m_pHandlerInfo;
    }

    CAdapter* GetAdapter()
    {
        return m_pAdapter;
    }

    PNDIS_OID_REQUEST GetOidRequest()
    {
        return m_pOidRequest;
    }

protected:
    /*++
        Can be called outside the OID handler to prepare the object

        Its called outside the event queue, so cannot trust the current adapter state

        Keep this protected so that the job cannot be inadvertently initialized from outside
    --*/
    NDIS_STATUS InitializeWithoutOid(
        _In_ WFC_JOB_TYPE JobType,
        _In_ CAdapter*  pAdapter,
        _In_ bool       IsPreemptable,
        _In_ bool       CanPreempt,
        _In_ WFC_SERIALIZED_JOB_PRIORITY_SCOPE PriorityScope,
        _In_ WFC_SERIALIZED_JOB_PRIORITY       JobPriority,
        _In_ NDIS_PORT_NUMBER PortNumber,
        _In_opt_ bool IsDeferrable = false
        );

    NDIS_PORT_NUMBER GetNdisPortNumber()
    {
        return m_NdisPortNumber;
    }        

    CPortPropertyCache* GetPortPropertyCache();

    bool IsActiveP2PPort()
    {
        bool isP2P = false;
        ULONG currentOpMode;

        if ( GetPortPropertyCache()->GetPropertyULong(
                 WfcPortCurrentOperationalMode,
                 &currentOpMode ) != NDIS_STATUS_SUCCESS )
        {
            goto exit;
        }

        isP2P = (currentOpMode & 
                 (WDI_OPERATION_MODE_P2P_DEVICE |
                  WDI_OPERATION_MODE_P2P_CLIENT |
                  WDI_OPERATION_MODE_P2P_GO)) != 0;

    exit:
        return isP2P;
    }
    
protected:
    CAdapter*   m_pAdapter;
    POID_HANDLER_ENTRY m_pHandlerInfo;
    PNDIS_OID_REQUEST m_pOidRequest;
    NDIS_PORT_NUMBER m_NdisPortNumber;
    CPortPropertyCache* m_pPortPropertyCache;

};



//==============================================================================================

/*++

This is a job that always succeeds the OID
--*/
class CStubOidJob : public COidJobBase
{
public:
    CStubOidJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) ;

    ~CStubOidJob ();

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
        _In_ WFC_SENDER_OBJECT SenderObject );
    
    void OnJobCancelled();
};


//==============================================================================================

/*++

This is a job that handles query OIDs that don't need to go down to the device
--*/
class CSimpleQueryOidJob : public COidJobBase
{
public:
    CSimpleQueryOidJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) ;

    virtual ~CSimpleQueryOidJob();

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
        _In_ WFC_SENDER_OBJECT SenderObject );
    
    void OnJobCancelled();

private:
    NDIS_OID    m_Oid;
    PVOID       m_pInformationBuffer;
    UINT        m_InformationBufferLength;
    PUINT       m_pBytesWritten;
    PUINT       m_pBytesNeeded;

protected:
    // The virtual function that handles the OID query
    virtual NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        ) = 0;
};



//==============================================================================================

/*++

This is a job that handles set OIDs that don't need to go down to the device
--*/
class CSimpleSetOidJob : public COidJobBase
{
public:
    CSimpleSetOidJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) ;

    virtual ~CSimpleSetOidJob();

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
        _In_ WFC_SENDER_OBJECT SenderObject );
    
    void OnJobCancelled();

private:
    NDIS_OID    m_Oid;
    PVOID       m_pInformationBuffer;
    UINT        m_InformationBufferLength;
    PUINT       m_pBytesRead;
    PUINT       m_pBytesNeeded;

protected:

    // The virtual function that handles the OID query
    virtual NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        ) = 0;
};

//==============================================================================================

/*++
Handles query of OIDs that just need to read a list from the property store
--*/
class CSimplePropertyCacheListQueryOidJob : public CSimpleQueryOidJob
{
private:
    WFC_PROPERTY_NAME m_PropertyName;
    UCHAR m_ObjectType;
    UCHAR m_ObjectRevision;
    USHORT m_ObjectSize;
    ULONG m_OffsetToTotalNumberOfEntries;
    ULONG m_OffsetToNumberOfEntries;
    ULONG m_OffsetToEntriesList;
    ULONG m_SizeOfEachEntry;

public:
    CSimplePropertyCacheListQueryOidJob(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ UCHAR ObjectType,
        _In_ UCHAR ObjectRevision,
        _In_ size_t ObjectSize,
        _In_ ULONG OffsetToTotalNumberOfEntries,
        _In_ ULONG OffsetToNumberOfEntries,
        _In_ ULONG OffsetToEntriesList,
        _In_ ULONG SizeOfEachEntry,
        _In_ WFC_ACTIVITY_ID ActivityId );

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};

//==============================================================================================

/*++
Handles set of OIDs that just need to save a list in the property store
--*/
class CSimplePropertyCacheListSetOidJob : public CSimpleSetOidJob
{
private:
    WFC_PROPERTY_NAME m_PropertyName;
    UCHAR m_ObjectType;
    UCHAR m_ObjectRevision;
    USHORT m_ObjectSize;
    ULONG m_OffsetToTotalNumberOfEntries;
    ULONG m_OffsetToNumberOfEntries;
    ULONG m_OffsetToEntriesList;
    ULONG m_MinimumRequiredEntries;
    ULONG m_MaximumAllowedEntries;
    ULONG m_SizeOfEachEntry;


public:
    CSimplePropertyCacheListSetOidJob(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ UCHAR ObjectType,
        _In_ UCHAR ObjectRevision,
        _In_ size_t ObjectSize,
        _In_ ULONG OffsetToTotalNumberOfEntries,
        _In_ ULONG OffsetToNumberOfEntries,
        _In_ ULONG OffsetToEntriesList,
        _In_ ULONG SizeOfEachEntry,
        _In_ ULONG MinimumRequiredEntries,
        _In_ ULONG MaximumAllowedEntries,
        _In_ WFC_ACTIVITY_ID ActivityId );

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        );
};
//==============================================================================================

/*++
Handles query of OIDs that need to read a ULONG from the property store
--*/
class CSimplePropertyCacheULongQueryOidJob : public CSimpleQueryOidJob
{
private:
    WFC_PROPERTY_NAME m_PropertyName;

public:
    CSimplePropertyCacheULongQueryOidJob(
        _In_ WFC_PROPERTY_NAME PropertyName,    
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
        m_PropertyName = PropertyName;
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};


class CSimplePropertyCacheULongSetOidJob : public CSimpleSetOidJob
{
private:
    WFC_PROPERTY_NAME m_PropertyName;
    ULONG m_MinimumRequiredValue;
    ULONG m_MaximumAllowedValue;

public:
    CSimplePropertyCacheULongSetOidJob(
        _In_ WFC_PROPERTY_NAME PropertyName,    
        _In_ ULONG MinimumRequiredValue,
        _In_ ULONG MaximumAllowedValue,
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleSetOidJob(ActivityId) 
    {
        m_PropertyName = PropertyName;
        m_MinimumRequiredValue = MinimumRequiredValue;
        m_MaximumAllowedValue = MaximumAllowedValue;
    }

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        );
};
//==============================================================================================

/*++
Handles query of OIDs that need to read a ULONG with header from the property store
--*/
class CSimplePropertyCacheUCharWithHeaderQueryOidJob : public CSimpleQueryOidJob
{
private:
    WFC_PROPERTY_NAME m_PropertyName;
    UCHAR m_ObjectType;
    UCHAR m_ObjectRevision;
    USHORT m_ObjectSize;
    ULONG m_Value;
public:
    CSimplePropertyCacheUCharWithHeaderQueryOidJob(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ UCHAR ObjectType,
        _In_ UCHAR ObjectRevision,
        _In_ size_t ObjectSize,
        _In_ WFC_ACTIVITY_ID ActivityId );

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};


class CSimplePropertyCacheUCharWithHeaderSetOidJob : public CSimpleSetOidJob
{
private:
    WFC_PROPERTY_NAME m_PropertyName;
    UCHAR m_ObjectType;
    UCHAR m_ObjectRevision;
    USHORT m_ObjectSize;
public:
    CSimplePropertyCacheUCharWithHeaderSetOidJob(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ UCHAR ObjectType,
        _In_ UCHAR ObjectRevision,
        _In_ size_t ObjectSize,
        _In_ WFC_ACTIVITY_ID ActivityId );

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded);
};


//==============================================================================================

/*++
Handles query of OIDs that need to read a BOOLEAN from the property store
--*/
class CSimplePropertyCacheBooleanQueryOidJob : public CSimpleQueryOidJob
{
private:
    WFC_PROPERTY_NAME m_PropertyName;

public:
    CSimplePropertyCacheBooleanQueryOidJob(
        _In_ WFC_PROPERTY_NAME PropertyName,    
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleQueryOidJob(ActivityId) 
    {
        m_PropertyName = PropertyName;
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );
};

class CSimplePropertyCacheBooleanSetOidJob : public CSimpleSetOidJob
{
private:
    WFC_PROPERTY_NAME m_PropertyName;

public:
    CSimplePropertyCacheBooleanSetOidJob(
        _In_ WFC_PROPERTY_NAME PropertyName,    
        _In_ WFC_ACTIVITY_ID ActivityId ) 
            : CSimpleSetOidJob(ActivityId) 
    {
        m_PropertyName = PropertyName;
    }

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        );
};

// ================================

/*++
  Handles set of OIDs that need to write a binary blob to the property store
--*/
class CPropertyCacheSetBufferOidJob : public CSimpleSetOidJob
{
private:
    WFC_PROPERTY_NAME m_PropertyName;
    ULONG m_DataOffset;

public:
    CPropertyCacheSetBufferOidJob(
        _In_ WFC_PROPERTY_NAME PropertyName,
        _In_ WFC_ACTIVITY_ID ActivityId,
        _In_opt_ ULONG DataOffset = 0 ) 
            : CSimpleSetOidJob(ActivityId) 
    {
        m_PropertyName = PropertyName;
        m_DataOffset = DataOffset;
    }

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded )
    {
        NDIS_STATUS status = NDIS_STATUS_SUCCESS;

        UNREFERENCED_PARAMETER(pBytesNeeded);

        NT_ASSERT(pInformationBuffer != NULL);

        if ( m_DataOffset >= InformationBufferLength )
        {
            NT_ASSERT(FALSE);
            status = NDIS_STATUS_INVALID_DATA;
            goto exit;
        }

        status = GetPortPropertyCache()->SetPropertyBuffer(
            m_PropertyName,
            InformationBufferLength - m_DataOffset,
            (BYTE*)pInformationBuffer + m_DataOffset );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            goto exit;
        }

        *pBytesRead = InformationBufferLength;

    exit:
        return status;
    }
};

// ================================

/*++

This is a job that handles set OIDs that only need to send 1 command
down to the device.

--*/
class CSimpleSetDeviceCommandOidJob : public COidJobBase
{
public:
    CSimpleSetDeviceCommandOidJob(
        _In_ WFC_ACTIVITY_ID ActivityId,
        _In_ bool UseTaskCommand = false );

    virtual ~CSimpleSetDeviceCommandOidJob();

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

protected:
    NDIS_OID    m_Oid;
    PVOID       m_pInformationBuffer;
    UINT        m_InformationBufferLength;
private:
    bool        m_UseTask;
    PUINT       m_pBytesRead;
    PUINT       m_pBytesNeeded;
    DeviceCommand* m_pDeviceCommand;
    Task        m_TaskCommand;
    PVOID       m_pCommandDataBuffer;  
    UINT        m_CommandBufferLength;

protected:
    //
    // The virtual function converts OID Data from NDIS to a WABI
    // Device Command.
    //
    // This function can optionally cache the input. 
    // The caller owns the pCommandbuffer and would free it
    //
    virtual NDIS_STATUS GetDeviceCommandData(
        _In_ PVOID             pInformationBuffer,
        _In_ UINT              InformationBufferLength,
        _In_ PUINT             pBytesRead,
        _Inout_ PUINT             pBytesNeeded,
        _In_ UINT              ReservedHeaderLength,
        _Out_ ULONG*           pCommandId,
        _Out_ ULONG*           pCommandBufferLength,
        _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer) = 0;

    //
    // If implementation needs to inspect the command result to cache
    // data/etc, it can override this function.
    //
    virtual NDIS_STATUS OnDeviceCommandCompletion(
        _In_ NDIS_STATUS OperationStatus,
        _In_ ULONG MessageLength,
        _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader );

};

// ================================

/*++
    This is a job that handles query OIDs that only need to send 
    one command down to the device
--*/

class CSimpleQueryDeviceCommandOidJob : public COidJobBase
{
public:
    CSimpleQueryDeviceCommandOidJob(
        _In_ WFC_ACTIVITY_ID ActivityId );

    virtual ~CSimpleQueryDeviceCommandOidJob();
    
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

protected:
    NDIS_OID    m_Oid;
    PVOID       m_pInformationBuffer;
    UINT        m_InformationBufferLength;
private:
    PUINT       m_pBytesWritten;
    PUINT       m_pBytesNeeded;
    DeviceCommand m_DeviceCommand;
    PVOID       m_pCommandDataBuffer;  
    UINT        m_CommandBufferLength;

protected:
    virtual NDIS_STATUS GetDeviceCommandId(
        _Out_ ULONG* pCommandId) = 0;

    virtual NDIS_STATUS OnDeviceCommandCompletion(
        _In_ NDIS_STATUS OperationStatus,
        _In_ ULONG CommandBufferLength,
        _In_reads_bytes_(CommandBufferLength) BYTE* pCommandBuffer,
        _In_ PVOID pInformationBuffer,
        _In_ UINT InformationBufferLength,
        _Inout_ PUINT pBytesWritten,
        _Inout_ PUINT pBytesNeeded) = 0;
};


// ================================

/*++
    This is a job that handles method OIDs that only need to send 
    one command down to the device
--*/
class CSimpleMethodDeviceCommandOidJob : public COidJobBase
{
public:
    CSimpleMethodDeviceCommandOidJob(
        _In_ WFC_ACTIVITY_ID ActivityId );

    virtual ~CSimpleMethodDeviceCommandOidJob();

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

protected:
    NDIS_OID    m_Oid;
    PVOID       m_pInformationBuffer;
    UINT        m_InputBufferLength;
    UINT        m_OutputBufferLength;
private:
    PUINT       m_pBytesRead;
    PUINT       m_pBytesNeeded;
    PUINT       m_pBytesWritten;
    DeviceCommand m_DeviceCommand;
    PVOID       m_pCommandDataBuffer;  
    UINT        m_CommandBufferLength;

protected:
    //
    // The virtual function converts OID Data from NDIS to a WABI
    // Device Command.
    //
    // This function can optionally cache the input. 
    // The caller owns the pCommandbuffer and would free it
    //
    virtual NDIS_STATUS GetDeviceCommandData(
        _In_reads_bytes_(InputBufferLength) PVOID             pInputBuffer,
        _In_ UINT              InputBufferLength,
        _Inout_ PUINT          pBytesRead,
        _Inout_ PUINT          pBytesNeeded,
        _In_ UINT              ReservedHeaderLength,
        _Out_ ULONG*           pCommandId,
        _Out_ ULONG*           pCommandBufferLength,
        _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer) = 0;

    //
    // If implementation needs to inspect the command result to cache
    // data/etc, it can override this function.
    //
    virtual NDIS_STATUS OnDeviceCommandCompletion(
        _Out_writes_bytes_(OutputBufferLength) PVOID             pOutputBuffer,
        _In_ UINT              OutputBufferLength,
        _Inout_ PUINT          pBytesWritten,
        _Inout_ PUINT          pBytesNeeded,    
        _In_ NDIS_STATUS       OperationStatus,
        _In_ ULONG             MessageLength,
        _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader ) = 0;

};
