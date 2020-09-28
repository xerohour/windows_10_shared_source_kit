/*++
Copyright (c) Microsoft Corporation

Module Name:

    interrupter.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

#define MAX_SECONDARY_INTERRUPTERS                      16
#define EVENT_RING_SEGMENT_COUNT_DEFAULT                4
#define EVENT_RING_SEGMENT_COUNT_TO_AVOID_FULL          8

#define DPC_WATCHDOG_THRESHOLD_PERCENTAGE               25
#define DPC_TIME_THRESHOLD_PERCENTAGE                   95

//
// Number of events to process before checking for watchdog timer expiration.
//
#define INTERRUPTER_MAX_EVENTS_HANDLED_PER_DPC_TIMEOUT_CHECK    64

//
// Number of events to process before updating the event ring dequeue pointer.
//
#define INTERRUPTER_MAX_EVENTS_PROCESSED_PER_ERDP_UPDATE        128

#define DEFAULT_MODERATION_INTERVAL                             200

typedef
VOID
(*PEVENT_HANDLER) (
    __in HANDLE ObjectHandle,
    __in PTRB   EventTrb
    );

typedef enum _InterrupterStates {
    InterrupterState_Uninitialized = 0,
    InterrupterState_Prepared,
    InterrupterState_Released
} InterrupterStates;

typedef enum _InterruptMechanism {
    InterruptMechanism_LineBased = 0,
    InterruptMechanism_MultiMessageMSI,
    InterruptMechanism_MSIX
} InterruptMechanism;

typedef struct _INTERRUPTER_COUNTER {
    ULONG                       IsrCount;
    ULONG                       DpcCount;
    ULONG                       EventCount;
    ULONG                       WorkItemCount;
    ULONG                       RequeueCount;
    ULONG                       QueuedWorkItem;
    ULONG                       QueuedDpc;
    ULONG                       EventRingBusy;
    ULONG                       EventRingFull;
    ULONG                       DpcTimeExpired;
    ULONG                       DpcWatchdogExpired;
    PPCW_INSTANCE               PcwInstanceHandle;
} INTERRUPTER_COUNTER, *PINTERRUPTER_COUNTER;

typedef struct _INTERRUPTER_FLAGS {
    ULONG   MessageSignaled:1;
    ULONG   PendingDisable:1;
    ULONG   Enabled:1;
    ULONG   DpcRunning:1;
} INTERRUPTER_FLAGS, *PINTERRUPTER_FLAGS;
C_ASSERT(sizeof(INTERRUPTER_FLAGS) == sizeof(ULONG));

typedef struct _INTERRUPTER_DATA {

    WDFINTERRUPT                WdfInterrupt;
    HANDLE                      ControllerHandle;
    RECORDER_LOG                RecorderLog;
    PINTERRUPTER_REGISTER_SET   InterrupterRegisterSet;

    ULONG                       MessageNumber;
    USHORT                      Group;
    KAFFINITY                   Affinity;

    INTERRUPTER_COUNTER         Counter;
    INTERRUPTER_FLAGS           Flags;
    InterrupterStates           State;

    ULONG                       SegmentSize;
    ULONG                       SegmentCount;
    ULONG                       UsedEventRingSegments;
    ULONG                       TRBsPerSegment;

    ULONG                       CycleState;
    ULONG                       DequeueIndex;
    ULONG                       DequeueSegment;

    PTRB                        CurrentRingSegment;
    PBUFFER_DATA                CurrentBufferData;
    PBUFFER_DATA                EventRingTableBufferData;
    LIST_ENTRY                  BufferDataList;

    KEVENT                      WorkItemEvent;
    PIO_WORKITEM                IoWorkItem;

    KSPIN_LOCK                  EventRingLock;

} INTERRUPTER_DATA, *PINTERRUPTER_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(INTERRUPTER_DATA, GetInterrupterData)

typedef struct _PRIMARY_INTERRUPTER_DATA {

    WDFDEVICE                           WdfDevice;
    HANDLE                              ControllerHandle;

    InterruptMechanism                  Mechanism;

    ULONG                               SecondaryInterrupterCount;
    ULONG                               InterrupterCount;

    PINTERRUPTER_DATA*                  InterrupterDataArray;
    PINTERRUPTER_DATA                   PrimaryInterrupter;

    ULONG                               ActiveProcessorCount;
    PUSHORT                             InterrupterLookup;

} PRIMARY_INTERRUPTER_DATA, *PPRIMARY_INTERRUPTER_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(PRIMARY_INTERRUPTER_DATA, GetPrimaryInterrupterData)

//
// Public functions.
//

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_Create(
    __in
        WDFDEVICE   WdfDevice,
    __in
        HANDLE      ControllerHandle,
    __inout
        PHANDLE     ObjectHandle
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_PrepareHardware(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDFCMRESLIST                WdfResourcesRaw,
    __in
        WDFCMRESLIST                WdfResourcesTranslated
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_ReleaseHardware(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Interrupter_Initialize(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Interrupter_InterrupterRegisterIntialize(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_D0Entry(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDF_POWER_DEVICE_STATE      WdfPreviousState,
    __in
        BOOLEAN                     RestoreState
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_D0ExitPreInterruptsDisabled(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDF_POWER_DEVICE_STATE      WdfTargetState
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_D0ExitStopped(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDF_POWER_DEVICE_STATE      WdfTargetState
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_FilterRemoveResourceRequirements(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDFIORESREQLIST             IoResourceRequirementsList
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_FilterAddResourceRequirements(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDFIORESREQLIST             IoResourceRequirementsList
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_FilterAllMSIResources(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDFIORESREQLIST             IoResourceRequirementsList
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_IoResList_GetMessageCountAndType(
    __in
        WDFIORESREQLIST IoResourceRequirementsList,
    __out
        PULONG          ListIndex,
    __out
        PULONG          MessageCount,
    __out
        PBOOLEAN        IsMSIX
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_CmResList_GetInterrupterCountAndType(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDFCMRESLIST                WdfResourcesRaw,
    __out
        PULONG                      InterruptCount,
    __out
        InterruptMechanism*         Mechanism
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_FilterMessageDescriptors(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDFIORESREQLIST             IoResourceRequirementsList,
    __in
        ULONG                       ListIndex,
    __in
        ULONG                       MessagesDesired
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_AffinitizeMSIXDescriptors(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDFIORESREQLIST             IoResourceRequirementsList,
    __in
        ULONG                       ListIndex
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_PopulateInterrupterLookupTable(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Interrupter_FlushEvent(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        USHORT                      InterrupterTarget
    );

EVT_WDF_OBJECT_CONTEXT_CLEANUP Interrupter_WdfEvtCleanupCallback;

EVT_WDF_INTERRUPT_ENABLE Interrupter_WdfEvtInterruptEnable;

EVT_WDF_INTERRUPT_DISABLE Interrupter_WdfEvtInterruptDisable;

EVT_WDF_INTERRUPT_ISR Interrupter_WdfEvtInterruptIsr;

EVT_WDF_INTERRUPT_DPC Interrupter_WdfEvtInterruptDpc;

//
// Private functions.
//

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_CreateInterrupter(
    __in
        WDFDEVICE                       WdfDevice,
    __in
        HANDLE                          ControllerHandle,
    __in
        ULONG                           MessageNumber,
    __in_opt
        PCM_PARTIAL_RESOURCE_DESCRIPTOR InterruptRaw,
    __in_opt
        PCM_PARTIAL_RESOURCE_DESCRIPTOR InterruptTranslated,
    __out
        PINTERRUPTER_DATA               *InterrupterData,
    __out_opt
        PPRIMARY_INTERRUPTER_DATA       *PrimaryInterrupterData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_DetermineSecondaryInterrupterCount(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        ULONG                       SupportedInterrupterCount
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_DetermineSegmentSizeAndCount(
    __in
        PINTERRUPTER_DATA   InterrupterData
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_PrepareInterrupter(
    __in
        PINTERRUPTER_DATA   InterrupterData,
    __in
        BOOLEAN             IsMessageSignaled
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_ReleaseInterrupter(
    __in
        PINTERRUPTER_DATA   InterrupterData
    );

IO_WORKITEM_ROUTINE_EX  Interrupter_IoWorkItemRequeueDpc;

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_GetMsiCapabilityData(
    __in
        HANDLE                      ControllerHandle,
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
PTRB
Interrupter_GetCurrentRingSegment(
    __in
        PINTERRUPTER_DATA   InterrupterData
    )
/*++

Routine Description:


--*/
{
    return InterrupterData->CurrentRingSegment;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
ULONG
Interrupter_GetTRBsPerSegment(
    __in
        PINTERRUPTER_DATA   InterrupterData
    )
/*++

Routine Description:


--*/
{
    return InterrupterData->TRBsPerSegment;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
ULONG
Interrupter_GetUsedEventRingSegments(
    __in
        PINTERRUPTER_DATA   InterrupterData
    )
/*++

Routine Description:


--*/
{
    return InterrupterData->UsedEventRingSegments;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
ULONG
Interrupter_GetDequeueIndex(
    __in
        PINTERRUPTER_DATA   InterrupterData
    )
/*++

Routine Description:


--*/
{
    return InterrupterData->DequeueIndex;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
ULONG
Interrupter_GetDequeueSegment(
    __in
        PINTERRUPTER_DATA   InterrupterData
    )
/*++

Routine Description:


--*/
{
    return InterrupterData->DequeueSegment;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
ULONG
Interrupter_GetCycleState(
    __in
        PINTERRUPTER_DATA   InterrupterData
    )
/*++

Routine Description:


--*/
{
    return InterrupterData->CycleState;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
PBUFFER_DATA
Interrupter_GetCurrentBufferData(
    __in
        PINTERRUPTER_DATA   InterrupterData
    )
/*++

Routine Description:


--*/
{
    return InterrupterData->CurrentBufferData;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
LIST_ENTRY
Interrupter_GetBufferDataList(
    __in
        PINTERRUPTER_DATA   InterrupterData
    )
/*++

Routine Description:


--*/
{
    return InterrupterData->BufferDataList;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
HANDLE
Interrupter_GetInterrupterData(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        ULONG                       Index
    )
/*++

Routine Description:


--*/
{
    return PrimaryInterrupterData->InterrupterDataArray[Index];
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
ULONG
Interrupter_GetInterrupterCount(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData
    )
/*++

Routine Description:


--*/
{
    return PrimaryInterrupterData->InterrupterCount;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
VOID
Interrupter_SetInterrupterCount(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        ULONG                       InterrupterCount
    )
{
    PrimaryInterrupterData->InterrupterCount = InterrupterCount;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
USHORT
Interrupter_GetInterrupterTarget(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        ULONG                       ProcessorIndex
    )
/*++

Routine Description:

    Given a ProcessorIndex return the id of the Interrupter that is affinitized
    to the processor.

    N.B: If a CPU is hot-added, there is a time window where it is possible that
    the Interrupter lookup table is not populated for the new processor count. In
    this case, the ProcessorIndex can be outside the range of known processors.
    When this happens, the id of the primary interrupter (0) is returned. Eventually,
    stop/start device will happen and a new lookup table will be populated.

--*/
{
    if (ProcessorIndex < PrimaryInterrupterData->ActiveProcessorCount) {
        return PrimaryInterrupterData->InterrupterLookup[ProcessorIndex];
    } else {
        return 0;
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Interrupter_ControllerResetPostReset(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
VOID
Interrupter_SetSecondaryInterrupterCount(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        ULONG                       SecondaryInterrupterCount
    )
{
    PrimaryInterrupterData->SecondaryInterrupterCount = SecondaryInterrupterCount;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
ULONG
Interrupter_GetSecondaryInterrupterCount(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData
    )
{
    return PrimaryInterrupterData->SecondaryInterrupterCount;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
BOOLEAN
Interrupter_IsMSIEnabled(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData
    )
{
    if (PrimaryInterrupterData->Mechanism == InterruptMechanism_LineBased) {
        return FALSE;
    } else {
        return TRUE;
    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
BOOLEAN
IsIoResourceDescriptorMSI(
    __in
        PIO_RESOURCE_DESCRIPTOR descriptor
    )
{
    return ((descriptor->Type == CmResourceTypeInterrupt) &&
            (descriptor->Flags & CM_RESOURCE_INTERRUPT_LATCHED) &&
            (descriptor->Flags & CM_RESOURCE_INTERRUPT_MESSAGE));
}

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
BOOLEAN
IsCmResourceDescriptorMSI(
    __in
        PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor
    )
{
    return ((descriptor->Type == CmResourceTypeInterrupt) &&
            (descriptor->Flags & CM_RESOURCE_INTERRUPT_LATCHED) &&
            (descriptor->Flags & CM_RESOURCE_INTERRUPT_MESSAGE));
}


__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Interrupter_SetCurrentRingSegment(
    __in
        PINTERRUPTER_DATA   InterrupterData,
    __in
        PTRB                CurrentRingSegment
    )
{
    InterrupterData->CurrentRingSegment = CurrentRingSegment;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Interrupter_SetTRBsPerSegment(
    __in
        PINTERRUPTER_DATA   InterrupterData,
    __in
        ULONG               TRBsPerSegment
    )
{
    InterrupterData->TRBsPerSegment = TRBsPerSegment;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Interrupter_SetUsedEventRingSegments(
    __in
        PINTERRUPTER_DATA   InterrupterData,
    __in
        ULONG               UsedEventRingSegments
    )
{
    InterrupterData->UsedEventRingSegments = UsedEventRingSegments;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Interrupter_SetDequeueIndex(
    __in
        PINTERRUPTER_DATA   InterrupterData,
    __in
        ULONG               DequeueIndex
    )
{
    InterrupterData->DequeueIndex = DequeueIndex;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Interrupter_SetDequeueSegment(
    __in
        PINTERRUPTER_DATA   InterrupterData,
    __in
        ULONG               DequeueSegment
    )
{
    InterrupterData->DequeueSegment = DequeueSegment;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Interrupter_SetCycleState(
    __in
        PINTERRUPTER_DATA   InterrupterData,
    __in
        ULONG               CycleState
    )
{
    InterrupterData->CycleState = CycleState;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Interrupter_SetCurrentBufferData(
    __in
        PINTERRUPTER_DATA   InterrupterData,
    __in
        PBUFFER_DATA        CurrentBufferData
    )
{
    InterrupterData->CurrentBufferData = CurrentBufferData;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Interrupter_SetBufferDataList(
    __in
        PINTERRUPTER_DATA   InterrupterData,
    __in
        LIST_ENTRY          BufferDataList
    )
{
    InterrupterData->BufferDataList = BufferDataList;
}

