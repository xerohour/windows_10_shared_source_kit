/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    target.h

Abstract:

    This module contains the internal type definitions for the SPBCx
    extension's target object

Environment:

    kernel-mode only

Revision History:

--*/
#pragma once


//
// The core target object
//

PCScxTarget
FORCEINLINE
GetTargetFromFxFile(
    __in WDFFILEOBJECT FxFile
    );

PCScxTarget
FORCEINLINE
GetTargetFromTargetQueue(
    __in WDFQUEUE FxQueue
    );

class CScxTarget : public CFxObject<WDFFILEOBJECT,
                                    CScxTarget,
                                    GetTargetFromFxFile,
                                    false>
{
    friend CFxObject;

public:

    //
    // Map of request type to transfer direction.
    //

    const static SPB_TRANSFER_DIRECTION s_TypeToDirectionMap[];

private:
    //
    // Pointer to the parent controller object.
    //

    const PCScxController m_Controller;

    //
    // The ConnectionID provided by the Resource Hub for this target connection
    //

    LARGE_INTEGER m_ConnectionId;

    //
    // String that represents the ConnectionID
    //

    PWCHAR m_ConnectionTag;

    //
    // Scope tag supplied in the file handle
    //

    LPWSTR m_ScopeTag;

    //
    // List of acceptable scope tags.
    //

    const static LPCWSTR s_AcceptableScopeTags[];

    //
    // The connection properties for this target connection
    //

    PRH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER m_ConnectionProperties;

    //
    // The pointer to the next target in the list
    // of targets.  This list is protected by the controller's
    // TargetListLock
    //

    LIST_ENTRY m_TargetListEntry;

    //
    // The target queue
    //

    WDFQUEUE m_TargetQueue;

    //
    // Pointer to the active request for this target.  Set when
    // presented on the target queue and cleared when completed
    // while restarting the target queue.
    //

    PCScxRequest m_ActiveRequest;

    //
    // True when this file has exclusive ownership of the controller
    //

    bool m_IsExclusiveControllerOwner;

    //
    // True when this file has exclusive ownership of the connection.
    //

    bool m_IsExclusiveConnectionOwner;

    //
    // A count of the number of I/O operations executed since the
    // controller was locked.  This is used to determine first/last
    // position within the sequence.
    //

    ULONG m_LockedIoCount;

    //
    // The direction of the last transfer.  Used within a sequence to
    // give the controller a hint about whether it needs to restart.
    //

    SPB_TRANSFER_DIRECTION m_LastDirection;

    //
    // True if this file was opened by a user-mode client, false if by
    // a kernel-mode client.  For kernel-mode clients we ignore cleanup
    // requests.
    //

    bool m_HasUserModeClient;

    //
    // Stat information tracking total IO counts, including
    // number of cancellations and failures.
    //

    struct
    {
        ULONGLONG TotalIoCount;
        ULONGLONG CancelledIoCount;
        ULONGLONG FailedIoCount;
        ULONGLONG IncompleteIoCount;
    } m_IoStats;

protected:
    virtual
    ~CScxTarget(
        VOID
        );

public:

    static
    NTSTATUS
    _CreateAndInitialize(
        __in  PCScxController  Controller,
        __in  WDFFILEOBJECT    FxFile,
        __in  PLARGE_INTEGER   ConnectionId,
        __in  LPWSTR           ScopeTag,
        __in  bool             UserModeClient,
        __in_opt PRH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER SpbConnection,
        __out PCScxTarget     *TargetObject
        );

    static
    NTSTATUS
    _ParseFileObjectName(
        __in  WDFFILEOBJECT   FxFile,
        __out PLARGE_INTEGER  ConnectionId,
        __out LPWSTR         *ScopeTag
        );

    static
    PCScxTarget
    _FromListEntry(
        __in PLIST_ENTRY Entry
        )
    {
        return CONTAINING_RECORD(Entry, CScxTarget, m_TargetListEntry);
    }

    NTSTATUS
    OnCreate(
        VOID
        );

    VOID
    OnCleanup(
        VOID
        );

    VOID
    OnClose(
        VOID
        );

    VOID
    GetConnectionParameters(
        __inout SPB_CONNECTION_PARAMETERS *ConnectionParameters
        );

    SPBTARGET
    GetSpbObject(
        VOID
        )
    {
        return (SPBTARGET) GetFxObject();
    }

    LARGE_INTEGER
    GetConnectionId(
        VOID
        )
    {
        return m_ConnectionId;
    }

    PCWSTR
    GetConnectionTag(
        VOID
        )
    {
        return m_ConnectionTag;
    }

    LPWSTR
    GetScopeTag(
        VOID
        )
    {
        return m_ScopeTag;
    }

    PRH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER
    GetConnectionProperties(
        VOID
        )
    {
        return m_ConnectionProperties;
    }

    PCScxController
    GetController(
        VOID
        )
    {
        return m_Controller;
    }

    PLIST_ENTRY
    GetTargetListEntry(
        VOID
        )
    {
        return &m_TargetListEntry;
    }

    VOID
    SetIsExclusiveControllerOwner(
        __in bool Value
        )
    {
        m_IsExclusiveControllerOwner = Value;

        //
        // Clear the locked I/O count.
        //

        m_LockedIoCount = 0;
    }

    bool
    IsExclusiveConnectionOwner(
        VOID
        )
    {
        return m_IsExclusiveConnectionOwner;
    }

    VOID
    SetIsExclusiveConnectionOwner(
        __in bool Value
        )
    {
        m_IsExclusiveConnectionOwner = Value;
    }

    VOID
    ForwardFxRequestToTargetQueue(
        __in WDFREQUEST FxRequest
        );

    NTSTATUS
    ForwardIrpToTargetQueue(
        __in PIRP Irp
        );

    VOID
    OnActiveRequestCompletion(
        __in PCScxRequest Request
        );

    static
    NTSTATUS
    QueryDescriptorFromResourceHub(
        _In_ PCScxController Controller,
        _In_ LARGE_INTEGER connectionId,
        _Out_ PRH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER *connectionProperties
       );

    VOID
    LogIoCompletionStatus(
        __in NTSTATUS Status,
        __in BOOLEAN Incomplete
        )
    {
        if (Status == STATUS_CANCELLED)
        {
            m_IoStats.CancelledIoCount += 1;
        }
        else if (!NT_SUCCESS(Status))
        {
            m_IoStats.FailedIoCount += 1;
        }
        else if (Incomplete)
        {
            m_IoStats.IncompleteIoCount += 1;
        }
    }

private:

    CScxTarget(
        __in PCScxController   Controller,
        __in WDFFILEOBJECT     FxObject,
        __in bool              UserModeClient
        ) : CFxObject(FxObject),
            m_Controller(Controller),
            m_HasUserModeClient(UserModeClient),
            m_IsExclusiveConnectionOwner(false),
            m_IsExclusiveControllerOwner(false),
            m_ActiveRequest(NULL)
    {
        RtlZeroMemory(&m_IoStats, sizeof(m_IoStats));
        return;
    }

    VOID
    ProcessTargetRequest(
        __in WDFREQUEST FxRequest,
        __in bool       StopTargetQueue
        );


    NTSTATUS
    SendIoctlToSelf(
        __in ULONG IoControlCode
        );

    //
    // Callback methods
    //

    static EVT_WDF_IO_QUEUE_IO_DEFAULT _OnTargetIoDefault;
};

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(CScxTarget,  _GetTargetFromFxFile);
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(PCScxTarget, _GetTargetFromFxQueue);

PCScxTarget
FORCEINLINE
GetTargetFromFxFile(
    __in WDFFILEOBJECT FxFile
    )
{
    return _GetTargetFromFxFile(FxFile);
}

PCScxTarget
FORCEINLINE
GetTargetFromTargetQueue(
    __in WDFQUEUE FxQueue
    )
{
    PCScxTarget* target = _GetTargetFromFxQueue(FxQueue);
    NT_ASSERTMSG("_FromFxQueue invoked on non-target queue", target != NULL);
    return *target;
}
