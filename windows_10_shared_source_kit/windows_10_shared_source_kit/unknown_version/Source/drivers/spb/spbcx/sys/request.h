/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name: 

    request.h

Abstract:

    This module contains the definitions for SPB requests

Environment:

    kernel-mode only

Revision History:

--*/

#pragma once
#pragma warning(disable:4201)

//
// The number of pages we preallocate MDL space for
//

#define SCX_PREALLOCATED_REQUEST_PAGE_COUNT 1

//
// The number of transfers we preallocate TD's for
//

#define SCX_PREALLOCATED_TD_COUNT 2

//
// Flags to add to the reserved field of a transfer descriptor.
//

typedef enum SCX_TRANSFER_DESCRIPTOR_FLAGS
{
    ScxTransferDescriptorFlagsMdlAllocated  = (1 << 0),
    ScxTransferDescriptorFlagsMdlLocked     = (1 << 1),
    ScxTransferDescriptorFlagsMdlFromCaller = (1 << 2)
}
SCX_TRANSFER_DESCRIPTOR_FLAGS, *PSCX_TRANSFER_DESCRIPTOR_FLAGS;

//
// Special case request flags
//

typedef enum SCX_REQUEST_FLAGS
{
    ScxRequestFlagsNone                   = 0,
    ScxRequestFlagsUseIoOtherTransferList = (1 << 0)
}
SCX_REQUEST_FLAGS, *PSCX_REQUEST_FLAGS;

#define DEFINE_ENUM_OPERATORS(ENUM, BASE)                                                           \
FORCEINLINE ENUM operator& (__in ENUM  A, __in ENUM B) {return (ENUM) (((BASE) A) & ((BASE) B));}   \
FORCEINLINE ENUM operator| (__in ENUM  A, __in ENUM B) {return (ENUM) (((BASE) A) | ((BASE) B));}   \
FORCEINLINE ENUM operator^ (__in ENUM  A, __in ENUM B) {return (ENUM) (((BASE) A) ^ ((BASE) B));}   \
FORCEINLINE ENUM operator~ (__in ENUM  A)              {return (ENUM) ~((BASE) A);}                 \
FORCEINLINE ENUM operator&=(__in ENUM &A, __in ENUM B) {return (A = (A & B));}                      \
FORCEINLINE ENUM operator|=(__in ENUM &A, __in ENUM B) {return (A = (A | B));}                      \
FORCEINLINE ENUM operator^=(__in ENUM &A, __in ENUM B) {return (A = (A ^ B));}                      \

DEFINE_ENUM_OPERATORS(SCX_TRANSFER_DESCRIPTOR_FLAGS, ULONG);

//
// Internal structure for tracking transfer descriptors.  Contains 
// extra members to store the current MDL.
//

typedef struct SCX_TRANSFER_DESCRIPTOR
{
    //
    // The TD we give to the controller driver
    //

    SPB_TRANSFER_DESCRIPTOR SpbDescriptor;

    //
    // Flags 
    //

    SCX_TRANSFER_DESCRIPTOR_FLAGS Flags;

    //
    // A location to store the MDL for this transfer's buffer.
    // Set to point to PreallocatedMdl when the transfer is smaller
    // than the predefined page count.
    //

    PMDL Mdl;

    //
    // Preallocated space for a single-page MDL.
    //

    struct 
    {
        MDL Mdl;
        PFN_NUMBER PfnArray[SCX_PREALLOCATED_REQUEST_PAGE_COUNT];
    } PreallocatedMdl;

public:

    VOID
    InitializeFromClientTd(
        __in PSPB_TRANSFER_LIST_ENTRY Source,
        __in ULONG                    BufferCb
        )
    {
        //
        // Fill in the destination TD
        //

        SpbDescriptor.Size = sizeof(SPB_TRANSFER_DESCRIPTOR);
        SpbDescriptor.Direction = Source->Direction;
        SpbDescriptor.TransferLength = BufferCb;
        SpbDescriptor.DelayInUs = Source->DelayInUs;
    }


    VOID
    CopyToDriverTd(
        __out SPB_TRANSFER_DESCRIPTOR *Destination
        )
    {
        *Destination = SpbDescriptor;
    }
}
SCX_TRANSFER_DESCRIPTOR, *PSCX_TRANSFER_DESCRIPTOR;

PCScxRequest
__forceinline
GetRequestFromFxRequest(
    __in WDFREQUEST o
    );

class CScxRequest : public CFxObject<WDFREQUEST,
                                     CScxRequest,
                                     GetRequestFromFxRequest,
                                     true>
{
    friend CFxObject;

private:

    //
    // The controller this request was issued to.
    //

    PCScxController m_Controller;

    //
    // The target this request was issued to.
    //

    PCScxTarget m_Target;

    //
    // Set to true if this is the active request for its target.
    // (e.g. the target queue is stopped due to this request)
    //

    bool m_IsTargetActiveRequest;

    //
    // Set to true if the completion of this request should restart the 
    // controller queue.
    //

    bool m_PostProcessUnlockOnCompletion;

    //
    // Set to the activity ID for the current request.
    //

    GUID m_ActivityId;

    //
    // True if the cleanup callback should log a stop event for the 
    // I/O task associated with this request.
    //

    bool m_LogStopEvent;

    //
    // The type of this request
    //

    SPB_REQUEST_TYPE m_RequestType;

    //
    // The position of this request within a sequence
    //

    SPB_REQUEST_SEQUENCE_POSITION m_Position;

    //
    // The direction of the previous transfer for this target
    //

    SPB_TRANSFER_DIRECTION m_LastDirection;

    //
    // The length of the entire transfer in bytes.
    //

    size_t m_TotalCb;

    //
    // The number of transfers in this request (1 for read/write, 
    // n for a sequence, 0 for lock/unlock)
    //

    ULONG m_TransferCount;

    //
    // A pointer to the array of transfer descriptors for this request
    //

    PSCX_TRANSFER_DESCRIPTOR m_TransferDescriptors;

    //
    // An array of TD's to use for the common I/O case.  If more 
    // are needed they are allocated from pool instead and freed
    // up in the destructor.
    //

    SCX_TRANSFER_DESCRIPTOR m_PreallocatedTDs[SCX_PREALLOCATED_TD_COUNT];

    //
    // Special case flags for this request
    //

    SCX_REQUEST_FLAGS m_RequestFlags;

    //
    // private constructor
    //

    CScxRequest(
        __in     PCScxController               Controller,
        __in     PCScxTarget                   Target,
        __in     WDFREQUEST                    FxRequest,
        __in     SCX_REQUEST_FLAGS             RequestFlags,
        __in_opt PGUID                         ActivityId
        ) : CFxObject(FxRequest),
            m_Controller(Controller),
            m_Target(Target),
            m_RequestFlags(RequestFlags),
            m_RequestType(SpbRequestTypeUndefined),
            m_Position(SpbRequestSequencePositionInvalid),
            m_TotalCb(0),
            m_TransferCount(0)
    {
        if (ARGUMENT_PRESENT(ActivityId))
        {
            m_ActivityId = *ActivityId;
            m_LogStopEvent = true;
        }

        return;
    }
    
    _Success_(return)
    bool
    static
    _WriteStartEvent(
        __in  WDFREQUEST    FxRequest,
        __in  WDFDEVICE     FxDevice,
        __in  WDFFILEOBJECT FxFile,
        __in  PCWSTR        TargetName,
        __in  PCWSTR        ScopeTag,
        __out GUID         *ActivityId
        );

    VOID
    static
    _WriteStopEvent(
        __in_opt PGUID        ActivityId,
        __in_opt PCScxRequest Request
        );

    static
    SPB_REQUEST_TYPE
    GetRequestType(
        __in      PWDF_REQUEST_PARAMETERS RequestParameters
        );

    NTSTATUS
    CaptureFxRequest(
        VOID
        );

    NTSTATUS
    CaptureTransferListIoctl(
        VOID
        );

    NTSTATUS
    CaptureTransferList(
        __in_ecount(DescriptorsCe) SPB_TRANSFER_LIST_ENTRY Descriptors[],
        __in                       ULONG                   DescriptorsCe,
        __in                       KPROCESSOR_MODE         CaptureMode
        );

    NTSTATUS
    CaptureTransferListEntry(
        __in    PSPB_TRANSFER_LIST_ENTRY  Source,
        __in    KPROCESSOR_MODE           CaptureMode,
        __in    ULONG                     Index,
        __inout size_t                   *TotalCb,
        __out   SCX_TRANSFER_DESCRIPTOR  *Destination
        );

    NTSTATUS
    ValidateClientTransferListEntry(
        __in PSPB_TRANSFER_LIST_ENTRY Source,
        __in ULONG                    Index
        );

    VOID
    CleanupTransferDescriptors(
        VOID
        );

    VOID
    CleanupTransferDescriptor(
        __inout PSCX_TRANSFER_DESCRIPTOR Desc
        );

    VOID
    CleanupMdlChain(
        __in_opt PMDL                          MdlChain,
        __in     SCX_TRANSFER_DESCRIPTOR_FLAGS Flags
        );

    NTSTATUS
    BuildMdlForBufferList(
        __in_ecount(ListCe) SPB_TRANSFER_BUFFER_LIST_ENTRY  List[],
        __in                ULONG                           ListCe,
        __in                KPROCESSOR_MODE                 CaptureMode,
        __in                SPB_TRANSFER_DIRECTION          Direction,
        __out               PMDL                           *Mdl,
        __out               SCX_TRANSFER_DESCRIPTOR_FLAGS  *Flags
        );

    NTSTATUS
    BuildMdlForBufferEntry(
        __in        PSPB_TRANSFER_BUFFER_LIST_ENTRY  Entry,
        __in        KPROCESSOR_MODE                  CaptureMode,
        __in        SPB_TRANSFER_DIRECTION           Direction,
        __inout_opt PMDL                             PreallocatedMdl,
        __in_opt    ULONG                            PreallocatedMdlPfnCount,
        __out       PMDL                            *Mdl,
        __out       SCX_TRANSFER_DESCRIPTOR_FLAGS   *Flags
        );

    ULONG
    GetTransferBufferLength(
        __in PSPB_TRANSFER_BUFFER Buffer
        );

    inline
    SCX_REQUEST_FLAGS
    GetRequestFlags(
        VOID
        )
    {
        return m_RequestFlags;
    }

protected:

    virtual
    ~CScxRequest(
        VOID
        );

    virtual
    VOID
    OnCleanup(
        VOID
        );

public:

    static
    NTSTATUS
    _CreateAndInitialize(
        __in  PCScxTarget        Target,
        __in  WDFREQUEST         FxRequest,
        __in  SCX_REQUEST_FLAGS  RequestFlags,
        __out PCScxRequest      *Request
        );

    SPBREQUEST
    GetSpbObject(
        VOID
        )
    {
        return (SPBREQUEST) GetFxObject();
    }

    inline
    PCScxController
    GetController(
        VOID
        )
    {
        return m_Controller;
    }

    inline
    PCScxTarget
    GetTarget(
        VOID
        )
    {
        return m_Target;
    }

    inline
    SPB_REQUEST_TYPE
    GetSpbIoType(
        VOID
        )
    {
        return m_RequestType;
    }

    inline 
    size_t
    GetTotalCb(
        VOID
        )
    {
        return m_TotalCb;
    }

    inline
    ULONG
    GetTransferCount(
        VOID
        )
    {
        return m_TransferCount;
    }

    VOID
    GetParameters(
        __inout SPB_REQUEST_PARAMETERS *Parameters
        );

    VOID
    GetTransferParameters(
        __in      ULONG Index,
        __out_opt SPB_TRANSFER_DESCRIPTOR *TransferDescriptor,
        __out_opt PMDL                    *TransferBuffer
        );

    VOID
    CompleteRequest(
        __in     NTSTATUS   Status,
        __in_opt ULONG_PTR *Information = NULL
        );

    bool 
    GetIsTargetActiveRequest(
        VOID
        )
    {
        return m_IsTargetActiveRequest;
    }

    VOID
    SetIsTargetActiveRequest(
        __in bool Value
        )
    {
        m_IsTargetActiveRequest = Value;
    }

    SPB_REQUEST_SEQUENCE_POSITION
    GetPosition(
        VOID
        )
    {
        return m_Position;
    }

    VOID
    SetPosition(
        __in SPB_REQUEST_SEQUENCE_POSITION Position
        )
    {
        m_Position = Position;
    }

    VOID
    SetLastDirection(
        __in SPB_TRANSFER_DIRECTION Direction
        )
    {
        m_LastDirection = Direction;
    }

    VOID
    SetPostProcessUnlockOnCompletion(
        __in bool Value
        )
    {
        m_PostProcessUnlockOnCompletion = Value;
    }

    bool
    GetPostProcessUnlockOnCompletion(
        VOID
        )
    {
        return m_PostProcessUnlockOnCompletion;
    }

    PGUID
    GetActivityId(
        VOID
        )
    {
        if (m_LogStopEvent) 
        {
            return &(m_ActivityId);
        }
        else
        {
            return NULL;
        }
    }

    BOOLEAN
    IsTransferListIo(
        VOID
        )
    {
        return (m_TransferCount > 0);
    }

    VOID
    WritePayloadEvents(
        VOID
        );
};

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(CScxRequest, _GetRequestFromFxRequest);

PCScxRequest
__forceinline
GetRequestFromFxRequest(
    __in WDFREQUEST o
    )
{
    return _GetRequestFromFxRequest((WDFOBJECT) o);
}

//
// Helper routine to get activity IDs from WDF requests
//

bool
inline
ScxRequestGetActivityId(
  __in  WDFREQUEST  FxRequest,
  __out GUID       *ActivityId
  )
{
    PIRP irp = WdfRequestWdmGetIrp(FxRequest);
    NTSTATUS status;

    status = IoGetActivityIdIrp(irp, ActivityId);

    return NT_SUCCESS(status);
}
