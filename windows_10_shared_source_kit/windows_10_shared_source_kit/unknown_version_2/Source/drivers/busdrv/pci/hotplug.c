/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    hotplug.c

Abstract:

    This module contains functions for hot plugging of PCI devices.









--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines

#define EVENT_MAP_INDEX(_Type, _ElementState) \
    ((_Type * 2) + ((_ElementState) ? 1 : 0))

#define PCI_MAXIMUM_HOTPLUG_COMMAND_COUNT       6

// ----------------------------------------------------------------------- Types

//
// Slot events.
//

typedef
VOID
(*PPCI_HOTPLUG_ACTION_CALLBACK)(
    __in PPCI_HOTPLUG_SLOT Slot,
    __in PVOID Context
    );

typedef struct _PCI_HOTPLUG_EVENT {

    LIST_ENTRY ListEntry;
    PCI_HOTPLUG_EVENT_TYPE Type;
    PPCI_HOTPLUG_ACTION_CALLBACK Callback;
    PVOID CallbackContext;

} PCI_HOTPLUG_EVENT, *PPCI_HOTPLUG_EVENT;

//
// Slot Commands.
//

typedef struct _PCI_HOTPLUG_COMMAND {

    PCI_HOTPLUG_COMMAND_TYPE Type;
    ULONG_PTR Context;

    BOOLEAN InProgress;

} PCI_HOTPLUG_COMMAND, *PPCI_HOTPLUG_COMMAND;

//
// Controller actions.
//

typedef enum {

    ActionFailureDisable,
    ActionFailureEnable,
    ActionFailureIgnore

} PCI_HOTPLUG_ACTION_FAILURE_TYPE;

typedef struct _PCI_HOTPLUG_ACTION {

    LIST_ENTRY ListEntry;

    PPCI_HOTPLUG_CONTROLLER Controller;
    PPCI_HOTPLUG_SLOT Slot;
    PCI_HOTPLUG_ACTION_FAILURE_TYPE FailureType;
    BOOLEAN Failed;

    PPCI_HOTPLUG_ACTION_CALLBACK Callback;
    PVOID CallbackContext;

    ULONG CurrentCommand;
    PCI_HOTPLUG_COMMAND Commands[PCI_MAXIMUM_HOTPLUG_COMMAND_COUNT];

} PCI_HOTPLUG_ACTION, *PPCI_HOTPLUG_ACTION;

// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciProcessCommandCompleteInterrupt (
    __inout PPCI_HOTPLUG_CONTROLLER Controller
    );

NTSTATUS
PciProcessSlotInterrupts (
    __inout PPCI_HOTPLUG_CONTROLLER Controller,
    __in PPCI_HOTPLUG_INTERRUPTS HotPlugInterrupts
    );

VOID
PciProcessHotPlugQueues (
    __inout PPCI_HOTPLUG_CONTROLLER Controller
    );

BOOLEAN
PciProcessSlotEventQueue (
    __inout PPCI_HOTPLUG_SLOT Slot
    );

BOOLEAN
PciProcessControllerActionQueue (
    __inout PPCI_HOTPLUG_CONTROLLER Controller
    );

NTSTATUS
PciQueueHotPlugEvent (
    __in PPCI_HOTPLUG_SLOT Slot,
    __in PCI_HOTPLUG_EVENT_TYPE EventType,
    __in_opt PPCI_HOTPLUG_ACTION_CALLBACK Callback,
    __in_opt PVOID CallbackContext
    );

BOOLEAN
PciProcessHotPlugEvent (
    __in PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent
    );

VOID
PciProcessStateAgnosticEvent (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction,
    __out PBOOLEAN ProcessingDone
    );

VOID
PciProcessEventInNotReadyState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    );

VOID
PciProcessEventInDisabledState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    );

VOID
PciProcessEventInWaitingWhileDisabledState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    );

VOID
PciProcessEventInPoweredState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    );

VOID
PciProcessEventInLinkActiveTimerQueuedState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    );

VOID
PciProcessEventInSettleTimerQueuedState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    );

VOID
PciProcessEventInFullyFunctionalState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    );

VOID
PciProcessEventInWaitingWhileEnabledState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    );

VOID
PciProcessEventInPowerOffTimerQueuedState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    );

NTSTATUS
PciCreateHotPlugActionFromEvent (
    __in PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT Event,
    __out PPCI_HOTPLUG_ACTION *Action
    );

NTSTATUS
PciCreateHotPlugAction (
    __in PPCI_HOTPLUG_CONTROLLER Controller,
    __in_opt PPCI_HOTPLUG_SLOT Slot,
    __in PCI_HOTPLUG_ACTION_FAILURE_TYPE FailureType,
    __in_opt PPCI_HOTPLUG_ACTION_CALLBACK Callback,
    __in_opt PVOID CallbackContext,
    __out PPCI_HOTPLUG_ACTION *Action
    );

VOID
PciAddCommandToAction (
    __inout PPCI_HOTPLUG_ACTION Action,
    __in PCI_HOTPLUG_COMMAND_TYPE Type,
    __in ULONG_PTR Context
    );

BOOLEAN
PciProcessHotPlugCommand (
    __inout PPCI_HOTPLUG_CONTROLLER Controller,
    __inout PPCI_HOTPLUG_SLOT Slot,
    __inout PPCI_HOTPLUG_ACTION Action,
    __in PPCI_HOTPLUG_COMMAND Command
    );

NTSTATUS
PciCreateControllerInterruptAction (
    __in PPCI_HOTPLUG_CONTROLLER Controller,
    __in BOOLEAN Enable,
    __out PPCI_HOTPLUG_ACTION *Action
    );

NTSTATUS
PciCreateSlotInterruptsAction (
    __in PPCI_HOTPLUG_SLOT Slot,
    __in BOOLEAN Enable,
    __out PPCI_HOTPLUG_ACTION *Action
    );

VOID
PciHandleHotPlugCommandFailure (
    __inout PPCI_HOTPLUG_ACTION Action,
    __in PPCI_HOTPLUG_COMMAND Command
    );

KDEFERRED_ROUTINE PciHotPlugCommandTimer;
KDEFERRED_ROUTINE PciHotPlugSlotTimer;

PCI_HOTPLUG_INDICATOR_STATE
PciGetNextAttentionIndicatorState (
    __in PPCI_HOTPLUG_SLOT Slot,
    __in PCI_ATTENTION_INDICATOR_EVENT_TYPE Reason
    );

VOID
PciDeviceEjectCallback (
    __in NTSTATUS Status,
    __inout_opt PVOID Context
    );

VOID
PciSetHotPlugEventCallback (
    __in PPCI_HOTPLUG_SLOT Slot,
    __inout PVOID Context
    );

#if DBG

VOID
PciDebugDumpHotPlugEvent (
    __in PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT Event
    );

VOID
PciDebugDumpHotPlugAction (
    __in PPCI_HOTPLUG_ACTION Action
    );

VOID
PciDebugDumpHotPlugSlotState (
    __in PPCI_HOTPLUG_SLOT Slot
    );

#endif

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciCreateHotPlugController)
    #pragma alloc_text(PAGE, PciInitializeHotPlugEventMap)
    #pragma alloc_text(PAGE, PciHandleHotPlugDriverRemoval)
#endif

// --------------------------------------------------------------------- Globals
// ------------------------------------------------------------------- Functions

NTSTATUS
PciCreateHotPlugController (
    __in PPCI_BRIDGE PciBridge,
    __in ULONG SlotCount,
    __out PPCI_HOTPLUG_CONTROLLER *HotPlugController
    )

/*++

Routine Description:

    This routine creates a PCI_HOTPLUG_CONTROLLER object and initializes the
    common fields in the structure. It also initializes the common fields for
    each slot structures created for the controller.

Arguments:

    PciBridge - Supplies a pointer to a PCI bridge.

    SlotCount - Supplies the count of slots in a hot plug controller.

    HotPlugController - Supplies a place holder for a hot plug controller.

Return Value:

    STATUS_INVALID_PARAMETER if the slot count exceeds the maximum number of
        PCI devices on a PCI bus (32).

    STATUS_INSUFFICIENT_RESOURCES on failure to allocate memory for a hot plug
        controller.

    STATUS_SUCCESS on success.

--*/

{

    ULONG ControllerSize;
    PPCI_HOTPLUG_CONTROLLER NewController;
    PPCI_HOTPLUG_SLOT Slot;
    ULONG SlotIndex;

    PAGED_CODE();

    if (SlotCount > PCI_MAX_DEVICES) {
        return STATUS_INVALID_PARAMETER;
    }

    ControllerSize = sizeof(PCI_HOTPLUG_CONTROLLER) +
                     (sizeof(PCI_HOTPLUG_SLOT) * (SlotCount - 1));

    NewController = PciAllocatePool(NonPagedPoolNx, ControllerSize);
    if (NewController == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(NewController, ControllerSize);

    //
    // Initialize the common controller fields including list heads, locks,
    // timers and dpcs.
    //

    NewController->PciBridge = PciBridge;
    NewController->SlotCount = SlotCount;
    InitializeListHead(&NewController->ActionQueue);
    KeInitializeSpinLock(&NewController->QueueLock);
    KeInitializeSpinLock(&NewController->TimerLock);
    NewController->TimersDisabled = FALSE;
    KeInitializeTimer(&NewController->CommandTimer);
    KeInitializeDpc(&NewController->CommandTimerDpc,
                    PciHotPlugCommandTimer,
                    NewController);

    //
    // Initialize each of the slot objects including list heads, locks, timers
    // and dpcs.
    //

    for (SlotIndex = 0; SlotIndex < SlotCount; SlotIndex++) {
        Slot = &NewController->Slots[SlotIndex];
        Slot->Controller = NewController;
        KeInitializeSpinLock(&Slot->DeviceLock);
        InitializeListHead(&Slot->EventQueue);
        KeInitializeTimer(&Slot->Timer);
        KeInitializeDpc(&Slot->TimerDpc, PciHotPlugSlotTimer, Slot);
    }

    *HotPlugController = NewController;
    return STATUS_SUCCESS;
}

NTSTATUS
PciStartHotPlugController (
    __inout PPCI_HOTPLUG_CONTROLLER Controller
    )

/*++

Routine Description:

    This routine is invoked to kick-start the engine for a hot plug controller.
    This is invoked once the hot plug interrupts are connected to and enabled.

Arguments:

    Controller - Supplies a pointer to a hot plug controller.

Return Value:

    STATUS_SUCCESS on success or an appropriate error code returned from a
        function call.

--*/

{

    PPCI_HOTPLUG_ACTION Action;
    NTSTATUS FinalStatus;
    KLOCK_QUEUE_HANDLE LockHandle;
    PPCI_HOTPLUG_SLOT Slot;
    ULONG SlotNumber;
    NTSTATUS Status;

    NON_PAGED_CODE();

    FinalStatus = STATUS_SUCCESS;
    KeAcquireInStackQueuedSpinLock(&Controller->QueueLock, &LockHandle);

    //
    // Initialize each slot.
    //

    for (SlotNumber = 0; SlotNumber < Controller->SlotCount; SlotNumber++) {
        Slot = &Controller->Slots[SlotNumber];

        //
        // Collect telemetry data.
        //

        TraceLoggingWrite(
            PciTraceLoggingProvider, 
            "PciHotplugSlotInformation",
            TraceLoggingString((Slot->Removable)?
                               "Yes": "No", "Removable"),

            TraceLoggingString((Slot->AttentionButtonPresent)?
                               "Yes": "No", "Attention Button Present"),

            TraceLoggingString((Slot->PowerControllerPresent)?
                               "Yes": "No", "Power Controller Present"),

            TraceLoggingString((Slot->MRLSensorPresent)?
                               "Yes": "No", "MRL Sensor Present"),

            TraceLoggingString((Slot->AttentionIndicatorPresent)?
                               "Yes": "No", "Attention Indicator Present"),

            TraceLoggingString((Slot->PowerIndicatorPresent)?
                               "Yes": "No", "Power Indicator Present"),

            TraceLoggingString((Slot->ElectromechanicalLockPresent)?
                               "Yes": "No", "Electromechanical Lock Present"),

            TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES)
        );

        //
        // If the slot is in any of the steady states, make sure its indicators
        // accurately reflect this. Turn off the attention indicator and set
        // the power indicator to the right state.
        //

        if ((Slot->CurrentState == SlotStateNotReady) ||
            (Slot->CurrentState == SlotStateDisabled) ||
            (Slot->CurrentState == SlotStateFullyFunctional)) {

            Status = PciCreateHotPlugAction(Controller,
                                            Slot,
                                            ActionFailureIgnore,
                                            NULL,
                                            NULL,
                                            &Action);

            if (!NT_SUCCESS(Status)) {
                FinalStatus = Status;
                continue;
            }

            if (Slot->CurrentState == SlotStateFullyFunctional) {
                PciAddCommandToAction(Action,
                                      SlotCommandSetPowerIndicator,
                                      SlotIndicatorOn);

            } else {
                PciAddCommandToAction(Action,
                                      SlotCommandSetPowerIndicator,
                                      SlotIndicatorOff);

            }

            PciAddCommandToAction(Action,
                                  SlotCommandSetAttentionIndicator,
                                  SlotIndicatorOff);

            InsertTailList(&Controller->ActionQueue, &Action->ListEntry);

            TRACE(HOTPLUG_TRACE_LEVEL,
                  "Queueing hotplug action: set initial indicator state\n");
            DebugDumpHotplugAction(Action);
            TRACE(HOTPLUG_TRACE_LEVEL, "\n");
        }
    }

    KeReleaseInStackQueuedSpinLock(&LockHandle);

    //
    // Make sure these new actions get processed.
    //

    PciProcessHotPlugQueues(Controller);
    return FinalStatus;
}

VOID
PciInitializeHotPlugEventMap (
    __inout PPCI_HOTPLUG_SLOT Slot
    )

/*++

Routine Description:

    This routine sets up an interrupt to event mapping for a given slot. The
    various types of hotplug slot interrupts will cause different events to
    be sent to the state machine for a slot depending on the capabilities that
    the slot supports.

    Some interrupt types have two possible events it can generate. For example,
    the presence detect interrupts generate different events based on whether
    the interrupt signals a device arrival or removal. As a result, the event
    map contains two entries for each interrupt.

Arguments:

    Slot - Supplies a pointer to a slot in a controller.

Return Value:

    None.

--*/

{

    ULONG AbsentIndex;
    ULONG ClosedIndex;
    BOOLEAN EnableEventSet;
    ULONG Index;
    ULONG OpenIndex;
    ULONG PresentIndex;
    BOOLEAN PrimeEventSet;

    PAGED_CODE();

    //
    // Each event map entry corresponds to a particular type of interrupt that
    // can occur and maps to the slot event that the interrupt represents. To
    // begin with, initialize the array so that all interrupts don't cause any
    // events at all.
    //

    for (Index = 0; Index < PCI_SLOT_EVENT_MAP_COUNT; Index++) {
        Slot->EventMap[Index] = SlotEventNone;
    }

    EnableEventSet = FALSE;
    PrimeEventSet = FALSE;

    //
    // Set up the mapping for Attention Button interrupts.
    //

    if (Slot->AttentionButtonPresent != FALSE) {
        Index = EVENT_MAP_INDEX(SlotInterruptAttentionButton, FALSE);
        if (Slot->PowerControllerPresent != FALSE) {

            //
            // Attention buttons are generally used by an operator to indicate
            // that a device should be brought online or taken offline and is
            // modelled by an AttentionRequest event. If software controls the
            // slot power, set up this model.
            //

            Slot->EventMap[Index] = SlotEventAttentionRequest;
            EnableEventSet = TRUE;

        } else {

            //
            // However, if the software can't control the power to a slot
            // explicitly, this model doesn't make sense. Instead, the power
            // is brought online automatically when a device is plugged in,
            // and the attention button is only used by an operator to request
            // a device removal.
            //

            Slot->EventMap[Index] = SlotEventDisableRequest;
        }
    }

    //
    // Set up the mapping for Manual Retention Latch (MRL) interrupts.
    //

    if (Slot->MRLSensorPresent != FALSE) {
        ClosedIndex = EVENT_MAP_INDEX(SlotInterruptLatchChange, TRUE);
        OpenIndex = EVENT_MAP_INDEX(SlotInterruptLatchChange, FALSE);
        if (EnableEventSet != FALSE) {

            //
            // If an attention button is used for requesting an enable/disable
            // of the slot, the MRL is used to indicate whether or not an
            // enable/disable request should be honored. If the MRL is open,
            // the device is not fully inserted, so an enable request shouldn't
            // be honored.
            //

            Slot->EventMap[OpenIndex] = SlotEventUnprimed;
            Slot->EventMap[ClosedIndex] = SlotEventPrimed;
            PrimeEventSet = TRUE;

        } else {

            //
            // If an attention button is not used for this, closing the latch
            // should bring the device online. Since opening the latch removes
            // power from the slot, this is a surprise disable.
            //

            Slot->EventMap[OpenIndex] = SlotEventSurpriseDisableRequest;
            Slot->EventMap[ClosedIndex] = SlotEventEnableRequest;
            EnableEventSet = TRUE;
        }
    }

    //
    // Set up the mapping for presence detect interrupts.
    //

    if (PrimeEventSet == FALSE) {
        PresentIndex = EVENT_MAP_INDEX(SlotInterruptPresenceDetect, TRUE);
        AbsentIndex = EVENT_MAP_INDEX(SlotInterruptPresenceDetect, FALSE);
        if (EnableEventSet != FALSE) {

            //
            // If an attention button or latch is used to enable the slot,
            // presence detect is used to indicate whether to allow the enable.
            //

            Slot->EventMap[AbsentIndex] = SlotEventUnprimed;
            Slot->EventMap[PresentIndex] = SlotEventPrimed;

        } else {

            //
            // If there is no event that causes an enable, make presence detect
            // itself cause this.
            //

            Slot->EventMap[AbsentIndex] = SlotEventSurpriseDisableRequest;
            Slot->EventMap[PresentIndex] = SlotEventEnableRequest;
            EnableEventSet = TRUE;
        }
    }

    //
    // Finally map the data link layer state change and power fault interrupt.
    //

    if (Slot->Controller->LinkActiveReportingCapable != FALSE) {
        Slot->EventMap[EVENT_MAP_INDEX(SlotInterruptLinkStateChange, FALSE)] =
            SlotEventLinkStateNotActive;

        Slot->EventMap[EVENT_MAP_INDEX(SlotInterruptLinkStateChange, TRUE)] =
            SlotEventLinkStateActive;
    }

    Slot->EventMap[EVENT_MAP_INDEX(SlotInterruptPowerFault, FALSE)] =
        SlotEventPowerFault;

    //
    // Paranoia - Currently this has to be true, but keep the assert in
    // case new code is added that violates this rule.
    //

    PCI_ASSERT(EnableEventSet != FALSE);

    return;
}

NTSTATUS
PciEnableDisableHotPlugInterrupts (
    __inout PPCI_HOTPLUG_CONTROLLER Controller,
    __in BOOLEAN EnableInterrupts
    )

/*++

Routine Description:

    This routine is invoked to enable/disable hot plug interrupts for a given
    hot plug controller.

Arguments:

    Controller - Supplies a pointer to a hot plug controller.

    Enable - Supplies a boolean to indicate if interrupts are to be enabled or
        disabled.

Return Value:

    STATUS_SUCCESS on success or an appropriate error code returned from a
    function call.

--*/

{

    PPCI_HOTPLUG_ACTION Action;
    ULONG Index;
    KLOCK_QUEUE_HANDLE LockHandle;
    NTSTATUS Status;

    NON_PAGED_CODE();

    TRACE(HOTPLUG_TRACE_LEVEL,
          "Queuing hotplug actions to %s interrupts...\n",
          EnableInterrupts ? "enable" : "disable");

    KeAcquireInStackQueuedSpinLock(&Controller->QueueLock, &LockHandle);

    //
    // Create actions to enable or disable controller and slot interrupts.
    //

    Status = PciCreateControllerInterruptAction(Controller,
                                                EnableInterrupts,
                                                &Action);

    if (NT_SUCCESS(Status)) {
        InsertTailList(&Controller->ActionQueue, &Action->ListEntry);
        DebugDumpHotplugAction(Action);
    }

    for (Index = 0; Index < Controller->SlotCount; Index++) {
        Status = PciCreateSlotInterruptsAction(&Controller->Slots[Index],
                                               EnableInterrupts,
                                               &Action);

        if (NT_SUCCESS(Status)) {
            InsertTailList(&Controller->ActionQueue, &Action->ListEntry);
            DebugDumpHotplugAction(Action);
        }
    }

    KeReleaseInStackQueuedSpinLock(&LockHandle);

    TRACE(HOTPLUG_TRACE_LEVEL, "\n");

    //
    // Make sure the action queue is processed.
    //

    PciProcessHotPlugQueues(Controller);
    return STATUS_SUCCESS;
}

VOID
PciProcessHotPlugInterrupts (
    __inout PPCI_HOTPLUG_CONTROLLER Controller,
    __in PPCI_HOTPLUG_INTERRUPTS HotPlugInterrupts
    )

/*++

Routine Description:

    This routine is invoked to process a set of hot plug interrupts for the
    controller and its slots. This can be invoked from the DPC for an ISR or
    as a result of expiration of the controller command timer.

Arguments:

    Controller - Supplies a pointer to a hot plug controller.

    HotPlugInterrupts - Supplies a pointer for a set of hot plug interrupts to
        handle.

Return Value:

    None.

--*/

{

    NON_PAGED_CODE();

    //
    // Process a command completion interrupt.
    //

    if (HotPlugInterrupts->CommandCompleteInterrupt != FALSE) {
        PciProcessCommandCompleteInterrupt(Controller);
    }

    //
    // Process slot interrupts. Each slot interrupt is transformed into an
    // event and placed on the appropriate slot event queue.
    //

    if (HotPlugInterrupts->SlotMask != 0) {
        PciProcessSlotInterrupts(Controller, HotPlugInterrupts);
    }

    //
    // Now process hot plug queues.
    //

    PciProcessHotPlugQueues(Controller);
    return;
}

NTSTATUS
PciProcessCommandCompleteInterrupt (
    __inout PPCI_HOTPLUG_CONTROLLER Controller
    )

/*++

Routine Description:

    This routine is invoked to process the command complete interrupt. It
    unblocks the controller action queue and advances to the following command.

Arguments:

    Controller - Supplies a pointer to a hot plug controller.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PPCI_HOTPLUG_ACTION HotPlugAction;
    PPCI_HOTPLUG_COMMAND HotPlugCommand;
    KLOCK_QUEUE_HANDLE LockHandle;

    NON_PAGED_CODE();

    KeAcquireInStackQueuedSpinLock(&Controller->QueueLock, &LockHandle);
    if (!IsListEmpty(&Controller->ActionQueue)) {
        KeCancelTimer(&Controller->CommandTimer);

        TRACE(HOTPLUG_TRACE_LEVEL,
              "Hotplug Command Completed - Controller 0x%p\n\n",
              Controller);

        HotPlugAction = CONTAINING_RECORD(Controller->ActionQueue.Flink,
                                          PCI_HOTPLUG_ACTION,
                                          ListEntry);

        HotPlugCommand =
            &HotPlugAction->Commands[HotPlugAction->CurrentCommand];


        //
        // The command inside the first hot plug action on the queue should be
        // the one in progress. If not, we have a spurious command complete
        // event.
        //

        PCI_ASSERT(HotPlugCommand->InProgress != FALSE);

        //
        // We are done processing the current command. Advance to the next
        // command in the hot plug action queue.
        //

        HotPlugCommand->InProgress = FALSE;
        HotPlugAction->CurrentCommand += 1;

        PCI_ASSERT(Controller->ActionQueueBlocked != FALSE);

        Controller->ActionQueueBlocked = FALSE;
    }

    KeReleaseInStackQueuedSpinLock(&LockHandle);
    return STATUS_SUCCESS;
}

NTSTATUS
PciProcessSlotInterrupts (
    __inout PPCI_HOTPLUG_CONTROLLER Controller,
    __in PPCI_HOTPLUG_INTERRUPTS HotPlugInterrupts
    )

/*++

Routine Description:

    This routine is invoked to process a slot interrupt. It determines the slot
    event type based on the slot interrupt and the current state of the hardware.
    It will then queue a hot plug event to the slot event queue for further
    processing.

Arguments:

    Controller - Supplies a pointer to a hot plug controller.

    HotPlugInterrupts - Supplies a pointer for a set of hot plug interrupts to
        handle.

Return Value:

    STATUS_SUCCESS on success or an appropriate error code returned from a
    function call.

--*/

{

    ULONG CurrentSlot;
    PCI_HOTPLUG_EVENT_TYPE EventType;
    PPCI_HOTPLUG_SLOT Slot;
    ULONG SlotInterruptBitPosition;
    ULONG SlotInterrupts;
    BOOLEAN State;
    NTSTATUS Status;

    NON_PAGED_CODE();

    PCI_ASSERT(HotPlugInterrupts->SlotMask != 0);

    for (CurrentSlot = 0; CurrentSlot < Controller->SlotCount; CurrentSlot++) {
        if ((HotPlugInterrupts->SlotMask & (1 << CurrentSlot)) == 0) {
            continue;
        }

        Slot = &Controller->Slots[CurrentSlot];
        SlotInterrupts = HotPlugInterrupts->SlotInterrupts[CurrentSlot];
        SlotInterruptBitPosition = 0;
        while (SlotInterrupts != 0) {
            if ((SlotInterrupts & (1 << SlotInterruptBitPosition)) != 0) {
                SlotInterrupts &= ~(1 << SlotInterruptBitPosition);

                //
                // Determine the event type for a slot interrupt based on slot
                // event mapping. The MRL sensor change and presence detect
                // change interrupts will be examined in conjunction with the
                // current MRL and device presence state.
                //

                State = FALSE;
                switch (SlotInterruptBitPosition) {
                case SlotInterruptPresenceDetect:
                    State = Controller->GetDevicePresenceState(Slot);
                    break;

                case SlotInterruptLatchChange:
                    State = Controller->GetLatchState(Slot);
                    break;

                case SlotInterruptLinkStateChange:
                    State = Controller->GetLinkState(Slot);
                    break;

                default:
                    break;

                }

                EventType =
                    Slot->EventMap[EVENT_MAP_INDEX(SlotInterruptBitPosition,
                                                   State)];

                Status = PciQueueHotPlugEvent(Slot, EventType, NULL, NULL);
                if (!NT_SUCCESS(Status)) {
                    continue;
                }
            }

            SlotInterruptBitPosition += 1;
        }
    }

    return STATUS_SUCCESS;
}

// ---------------------------------------------------------------------- Queues

VOID
PciProcessHotPlugQueues (
    __inout PPCI_HOTPLUG_CONTROLLER Controller
    )

/*++

Routine Description:

    This routine drains the slot event queues for each slot and the controller
    action queue until all are blocked, or empty. The event queues will be
    blocked on pending actions, and the action queue will be blocked on pending
    commands that need to receive command complete interrupts.

Arguments:

    Controller - Supplies a pointer to a hot plug controller.

Return Value:

    None.

--*/

{

    KLOCK_QUEUE_HANDLE ControllerHandle;
    BOOLEAN Result;
    ULONG SlotNumber;
    BOOLEAN WorkDone;

    NON_PAGED_CODE();

    TRACE(HOTPLUG_TRACE_LEVEL, "Processing hotplug queues\n");

    KeAcquireInStackQueuedSpinLock(&Controller->QueueLock, &ControllerHandle);
    do {

        WorkDone = FALSE;

        //
        // Run through the event and action queues until they are all blocked.
        //

        for (SlotNumber = 0; SlotNumber < Controller->SlotCount; SlotNumber++) {
            Result = PciProcessSlotEventQueue(&Controller->Slots[SlotNumber]);
            if (Result != FALSE) {
                WorkDone = TRUE;
            }
        }

        //
        // Now process the action queue. There is only one such queue per hot plug
        // controller since commands and command complete interrupts occur at the
        // controller granularity, not slot granularity. All slot event queues that
        // cause actions to be queued feed into this queue.
        //

        Result = PciProcessControllerActionQueue(Controller);
        if (Result != FALSE) {
            WorkDone = TRUE;
        }

    } while (WorkDone != FALSE);
    KeReleaseInStackQueuedSpinLock(&ControllerHandle);

    TRACE(HOTPLUG_TRACE_LEVEL, "\n");

    return;
}

BOOLEAN
PciProcessSlotEventQueue (
    __inout PPCI_HOTPLUG_SLOT Slot
    )

/*++

Routine Description:

    This routine is invoked to process a hot plug event queued at the slot
    event queue.

Arguments:

    Slot - Supplies a pointer to a slot in a hot plug controller.

Return Value:

    TRUE if any hot plug event is processed. FALSE otherwise.

--*/

{

    PPCI_HOTPLUG_EVENT HotPlugEvent;
    PLIST_ENTRY ListEntry;
    BOOLEAN Queued;
    KLOCK_QUEUE_HANDLE SlotHandle;
    BOOLEAN WorkDone;

    NON_PAGED_CODE();

    WorkDone = FALSE;
    KeAcquireInStackQueuedSpinLock(&Slot->QueueLock, &SlotHandle);
    while ((IsListEmpty(&Slot->EventQueue) == FALSE) &&
           (Slot->EventQueueBlocked == FALSE)) {

        WorkDone = TRUE;

        //
        // Pop the first event off of the queue.
        //

        ListEntry = RemoveHeadList(&Slot->EventQueue);
        HotPlugEvent = CONTAINING_RECORD(ListEntry,
                                         PCI_HOTPLUG_EVENT,
                                         ListEntry);

        PCI_ASSERT(HotPlugEvent != NULL);

        TRACE(HOTPLUG_TRACE_LEVEL, "Processing slot event\n");
        DebugDumpHotplugEvent(Slot, HotPlugEvent);

        //
        // Handle the de-queued event by submitting it to a state machine. This
        // can cause a new action to be queued. In that case the action queue
        // needs to run to drain the action, so the event queue is blocked.
        //

        Queued = PciProcessHotPlugEvent(Slot, HotPlugEvent);
        if (Queued != FALSE) {
            Slot->EventQueueBlocked = TRUE;

        } else {

            //
            // If there is a callback associated with this event, it needs to
            // be called. Typically this is done after the action associated
            // with the event is completed, but in this case there is no such
            // action.
            //

            if (HotPlugEvent->Callback != NULL) {
                HotPlugEvent->Callback(Slot, HotPlugEvent->CallbackContext);
            }
        }

        PciFreePool(HotPlugEvent);
    }

    KeReleaseInStackQueuedSpinLock(&SlotHandle);
    return WorkDone;
}

BOOLEAN
PciProcessControllerActionQueue (
    __inout PPCI_HOTPLUG_CONTROLLER Controller
    )

/*++

Routine Description:

    This routine is invoked to process some command in the controller action
    queue. If the command execution requires command completion interrupt then
    the controller action queue is blocked.

Arguments:

    Controller - Supplies a pointer to a hot plug controller.

Return Value:

    TRUE if any action is processed. FALSE otherwise.

--*/

{

    PPCI_HOTPLUG_ACTION Action;
    ULONG CommandIndex;
    BOOLEAN Queued;
    KLOCK_QUEUE_HANDLE SlotHandle;
    BOOLEAN WorkDone;

    NON_PAGED_CODE();

    WorkDone = FALSE;
    while ((IsListEmpty(&Controller->ActionQueue) == FALSE) &&
           (Controller->ActionQueueBlocked == FALSE)) {

        WorkDone = TRUE;

        //
        // Get the first action but do not pop it off the queue yet. Actions
        // contain multiple commands that must be processed in sequence, so
        // this routine may not be able to complete the processing of this
        // action.
        //

        Action = CONTAINING_RECORD(Controller->ActionQueue.Flink,
                                   PCI_HOTPLUG_ACTION,
                                   ListEntry);

        TRACE(HOTPLUG_TRACE_LEVEL, "Processing hotplug action...\n");
        DebugDumpHotplugAction(Action);

        while (TRUE) {

            //
            // Actions are composed of one or more individual commands that
            // must be submitted. Process each in turn.
            //

            CommandIndex = Action->CurrentCommand;
            if ((Action->Failed != FALSE) ||
                (CommandIndex >= PCI_MAXIMUM_HOTPLUG_COMMAND_COUNT) ||
                (Action->Commands[CommandIndex].Type == SlotCommandNoop)) {

                //
                // We've either reached the end of a command list or hit a no-
                // op command entry. In either case, this action is complete.
                //

                RemoveEntryList(&Action->ListEntry);
                if (Action->Failed != FALSE) {

                    TRACE(HOTPLUG_TRACE_LEVEL, "\tAction failed!\n");

                    PciHandleHotPlugCommandFailure(
                        Action,
                        &Action->Commands[CommandIndex]);

                } else {

                    TRACE(HOTPLUG_TRACE_LEVEL, "\tAction completed.\n");

                }

                //
                // Completing the action means that the slot that queued it can
                // now continue processing events.
                //

                if (Action->Slot != NULL) {
                    KeAcquireInStackQueuedSpinLock(&Action->Slot->QueueLock,
                                                   &SlotHandle);

                    Action->Slot->EventQueueBlocked = FALSE;
                    KeReleaseInStackQueuedSpinLock(&SlotHandle);
                }

                //
                // Call the action callback, if present.
                //

                if (Action->Callback != NULL) {
                    Action->Callback(Action->Slot,
                                     Action->CallbackContext);

                }

                PciFreePool(Action);
                break;

            }

            //
            // There is a command to execute. If the command will complete when
            // an asychronous event like a command complete interrupt occurs,
            // "queued" will be TRUE and no more work can be done on the action
            // queue until the asynchronous event un-freezes the queue.
            //

            TRACE(HOTPLUG_TRACE_LEVEL, "\tProcessing current command\n");

            Queued = PciProcessHotPlugCommand(Controller,
                                              Action->Slot,
                                              Action,
                                              &Action->Commands[CommandIndex]);

            if (Queued != FALSE) {

                TRACE(HOTPLUG_TRACE_LEVEL,
                        "\tCommand queued - Action Queue blocked\n");

                Action->Commands[CommandIndex].InProgress = TRUE;
                Controller->ActionQueueBlocked = TRUE;
                break;
            }

            Action->CurrentCommand++;
        }
    }

    return WorkDone;
}

//--------------------------------------------------------- Slot Event Functions

NTSTATUS
PciQueueHotPlugEvent (
    __in PPCI_HOTPLUG_SLOT Slot,
    __in PCI_HOTPLUG_EVENT_TYPE EventType,
    __in_opt PPCI_HOTPLUG_ACTION_CALLBACK Callback,
    __in_opt PVOID CallbackContext
    )

/*++

Routine Description:

    This routine queues an event onto a slot's event queue.

Arguments:

    Slot - Supplies a pointer to a hot plug slot.

    EventType - Supplies the type of hot plug event to be queued.

    Callback - Supplies a callback routine to be invoked for a hot plug event.

    CallbackContext - Supplies a context for a callback routine associated with
        the hot plug event.

Return Value:

    STATUS_INSUFFICIENT_RESOURCES on failure to allocate memory for a slot
        event.

    STATUS_SUCCESS on success.

--*/

{

    KLOCK_QUEUE_HANDLE LockHandle;
    PPCI_HOTPLUG_EVENT HotPlugEvent;

    NON_PAGED_CODE();

    //
    // Create the event structure.
    //

    HotPlugEvent = PciAllocatePool(NonPagedPoolNx, sizeof(PCI_HOTPLUG_EVENT));
    if (HotPlugEvent == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    HotPlugEvent->Type = EventType;
    HotPlugEvent->Callback = Callback;
    HotPlugEvent->CallbackContext = CallbackContext;

    //
    // And add it to the queue
    //

    KeAcquireInStackQueuedSpinLock(&Slot->QueueLock, &LockHandle);
    InsertTailList(&Slot->EventQueue, &HotPlugEvent->ListEntry);
    KeReleaseInStackQueuedSpinLock(&LockHandle);

    TRACE(HOTPLUG_TRACE_LEVEL, "Queuing new hotplug event...\n");
    DebugDumpHotplugEvent(Slot, HotPlugEvent);
    TRACE(HOTPLUG_TRACE_LEVEL, "\n");

    return STATUS_SUCCESS;
}

BOOLEAN
PciProcessHotPlugEvent (
    __in PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent
    )

/*++

Routine Description:

    This routine is invoked to process a hot plug event for a slot based on its
    current state. Successful processing of an event can result in queueing of
    a new action for the controller as well as transitioning of the slot to a
    new state.

Parameters:

    Slot - Supplies a pointer to a hot plug slot.

    HotPlugEvent - Supplies a pointer to hot plug event.

Return Value:

    TRUE if a hot plug action was queued while processing the event.

    FALSE otherwise.

--*/

{

    PPCI_HOTPLUG_ACTION HotPlugAction;
    BOOLEAN Pending;
    BOOLEAN ProcessingDone;

    NON_PAGED_CODE();

    HotPlugAction = NULL;
    Pending = FALSE;

    //
    // If this is a spurious hot plug event then return immediately.
    //

    if (HotPlugEvent->Type == SlotEventNone) {
        return FALSE;
    }

    PciProcessStateAgnosticEvent(Slot,
                                 HotPlugEvent,
                                 &HotPlugAction,
                                 &ProcessingDone);

    if (ProcessingDone != FALSE) {
        goto Done;
    }

    //
    // Examine the current slot state and a hot plug event type to determine
    // the next state for a slot.
    //

    switch (Slot->CurrentState) {
    case SlotStateNotReady:
        PciProcessEventInNotReadyState(Slot, HotPlugEvent, &HotPlugAction);
        break;

    case SlotStateDisabled:
        PciProcessEventInDisabledState(Slot, HotPlugEvent, &HotPlugAction);
        break;

    case SlotStateWaitingWhileDisabled:
        PciProcessEventInWaitingWhileDisabledState(Slot,
                                                   HotPlugEvent,
                                                   &HotPlugAction);

        break;

    case SlotStatePowered:
        PciProcessEventInPoweredState(Slot, HotPlugEvent, &HotPlugAction);
        break;

    case SlotStateLinkActiveTimerQueued:
        PciProcessEventInLinkActiveTimerQueuedState(Slot,
                                                    HotPlugEvent,
                                                    &HotPlugAction);

        break;

    case SlotStateSettleTimerQueued:
        PciProcessEventInSettleTimerQueuedState(Slot,
                                                HotPlugEvent,
                                                &HotPlugAction);

        break;

    case SlotStateFullyFunctional:
        PciProcessEventInFullyFunctionalState(Slot,
                                              HotPlugEvent,
                                              &HotPlugAction);

        break;

    case SlotStateWaitingWhileEnabled:
        PciProcessEventInWaitingWhileEnabledState(Slot,
                                                  HotPlugEvent,
                                                  &HotPlugAction);

        break;

    case SlotStatePowerOffTimerQueued:
        PciProcessEventInPowerOffTimerQueuedState(Slot,
                                                  HotPlugEvent,
                                                  &HotPlugAction);

        break;

    default:

        PCI_ASSERT(FALSE);

        break;
    }

Done:

    if (HotPlugAction != NULL) {

        PCI_ASSERT(HotPlugAction->Commands[0].Type != SlotCommandNoop);

        InsertTailList(&Slot->Controller->ActionQueue, &HotPlugAction->ListEntry);

        TRACE(HOTPLUG_TRACE_LEVEL, "Queuing action for state transition...\n");
        DebugDumpHotplugAction(HotPlugAction);
        TRACE(HOTPLUG_TRACE_LEVEL, "\n");

        Pending = TRUE;
    }

    return Pending;
}

VOID
PciProcessStateAgnosticEvent (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction,
    __out PBOOLEAN ProcessingDone
    )

/*++

Routine Description:

    This routine is invoked to handle certain hot plug events that are agnostic
    of the current state of the slot.

Parameters:

    Slot - Supplies a pointer to a hot plug slot.

    HotPlugEvent - Supplies a pointer to a hot plug event.

    HotPlugAction - Supplies a place holder for a hot plug action.

    ProcessingDone - Supplies a place holder for a value indicating that the
        hot plug event processing is done.

Return Value:

    None.

--*/

{

    NTSTATUS Status;
    ULONG TimerFlags;

    NON_PAGED_CODE();

    *HotPlugAction = NULL;
    *ProcessingDone = TRUE;
    switch (HotPlugEvent->Type) {
    case SlotEventUnprimed:
    case SlotEventPowerFault:
    case SlotEventSurpriseDisableRequest:
        Status = PciCreateHotPlugActionFromEvent(Slot,
                                                 HotPlugEvent,
                                                 HotPlugAction);

        if (!NT_SUCCESS(Status)) {
            break;
        }

        (*HotPlugAction)->FailureType = ActionFailureIgnore;
        if (Slot->Controller->PciBridge->LinkDisableOnSlotPowerDown != 0) {
            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandSetLinkState,
                                  LinkStateDisable);
        }

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetSlotPower,
                              SlotPowerOff);

        TimerFlags = PCI_HOTPLUG_SLOT_INVALIDATE_DEVICE;
        if (HotPlugEvent->Type != SlotEventSurpriseDisableRequest) {
            TimerFlags |= PCI_HOTPLUG_SLOT_UNPRIMED;
        }

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandQueuePowerOffTimer,
                              TimerFlags);

        Slot->CurrentState = SlotStatePowerOffTimerQueued;
        break;

    case SlotEventStackTornDown:

        //
        // If we receive an IRP_MN_EJECT IRP, power needs to be removed from
        // the slot. This is accomplished by transitioning to the disabled
        // state, unless the slot is already in the not ready state, in which
        // case power is already off and the slot should remain in its current
        // state.
        //

        if ((Slot->CurrentState != SlotStateNotReady) &&
            (Slot->CurrentState != SlotStateDisabled)) {

            Status = PciCreateHotPlugActionFromEvent(Slot,
                                                     HotPlugEvent,
                                                     HotPlugAction);

            if (!NT_SUCCESS(Status)) {
                break;
            }

            if (Slot->Controller->PciBridge->LinkDisableOnSlotPowerDown != 0) {
                PciAddCommandToAction(*HotPlugAction,
                                      SlotCommandSetLinkState,
                                      LinkStateDisable);
            }

            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandSetSlotPower,
                                  SlotPowerOff);

            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandQueuePowerOffTimer,
                                  0);

            Slot->CurrentState = SlotStatePowerOffTimerQueued;
        }

        break;

    case SlotEventForceEnable:

        //
        // If we are asked to forcibly enable a slot (probably because some
        // command to move the slot out of the enabled state failed), we will
        // move to the fully functional state regardless of the current state.
        // If this fails, continue on anyway.
        //
        // N.B. This event should be used with incredible care since it skips
        //      bus speed checks and settle times.
        //

        Status = PciCreateHotPlugActionFromEvent(Slot,
                                                 HotPlugEvent,
                                                 HotPlugAction);

        if (!NT_SUCCESS(Status)) {
            break;
        }

        (*HotPlugAction)->FailureType = ActionFailureIgnore;
        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetLinkState,
                              LinkStateEnable);

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetSlotPower,
                              SlotPowerOn);

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetPowerIndicator,
                              SlotIndicatorOn);

        Slot->CurrentState = SlotStateFullyFunctional;
        break;

    case SlotEventForceDisable:

        //
        // If we are asked to forcibly disable a slot (probably because some
        // command failed execution), move to the disabled state regardless of
        // the current state. Since this is as a result of a failure, move to
        // this state even if the commands to get there fail, so that at least
        // the state machine will be in a consistent state, even if the hardware
        // is not.
        //

        Status = PciCreateHotPlugActionFromEvent(Slot,
                                                 HotPlugEvent,
                                                 HotPlugAction);

        if (!NT_SUCCESS(Status)) {
            break;
        }

        (*HotPlugAction)->FailureType = ActionFailureIgnore;
        if (Slot->Controller->PciBridge->LinkDisableOnSlotPowerDown != 0) {
            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandSetLinkState,
                                  LinkStateDisable);
        }

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetSlotPower,
                              SlotPowerOff);

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandQueuePowerOffTimer,
                              0);

        Slot->CurrentState = SlotStatePowerOffTimerQueued;
        break;

    default:
        *ProcessingDone = FALSE;
        break;
    }

    return;
}

VOID
PciProcessEventInNotReadyState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    )

/*++

Routine Description:

    This routine is invoked to handle a hot plug event for a slot when it is in
    the 'not ready' state.

Parameters:

    Slot - Supplies a pointer to a hot plug slot.

    HotPlugEvent - Supplies a pointer to a hot plug event.

    HotPlugAction - Supplies a place holder for a hot plug action.

Return Value:

    None.

--*/

{

    NON_PAGED_CODE();

    *HotPlugAction = NULL;

    //
    // State: SlotStateNotReady.
    //
    // A slot is in a 'Not Ready' state when it is not ready to be enabled.
    // Either its MRL is open or a power fault has occurred.
    //
    // Properties:
    //      Slot Power is off.
    //      Power Indicator is off.
    //      Enable requests are ignored.
    //

    if (HotPlugEvent->Type == SlotEventPrimed) {

        //
        // No changes are required to get to the disabled state. The disabled
        // state simply changes the events that will be processed and those
        // that will be ignored.
        //

        Slot->CurrentState = SlotStateDisabled;
    }

    return;
}

VOID
PciProcessEventInDisabledState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    )

/*++

Routine Description:

    This routine is invoked to handle a hot plug event for a slot when it is in
    the 'disabled' state.

Parameters:

    Slot - Supplies a pointer to a hot plug slot.

    HotPlugEvent - Supplies a pointer to a hot plug event.

    HotPlugAction - Supplies a place holder for a hot plug action.

Return Value:

    None.

--*/

{

    PCI_HOTPLUG_INDICATOR_STATE AttentionIndicator;
    NTSTATUS Status;

    NON_PAGED_CODE();

    *HotPlugAction = NULL;

    //
    // State: SlotStateDisabled.
    //
    // A slot is in this state when it is disabled, but ready to be enabled.
    //
    // Properties:
    //      Slot Power is off.
    //      Power Indicator is off.
    //

    switch (HotPlugEvent->Type) {
    case SlotEventAttentionRequest:

        //
        // An attention event request in a disabled state is only valid when
        // a power controller is present.
        //

        PCI_ASSERT(Slot->PowerControllerPresent != FALSE);

        if (Slot->Controller->GetDevicePresenceState(Slot) == FALSE) {
            break;
        }

        //
        // Check if there is a power indicator. In that case, blink it and let
        // the user have 5 seconds to cancel the attention request by sending
        // another.
        //

        if (Slot->PowerIndicatorPresent != FALSE) {

            Status = PciCreateHotPlugActionFromEvent(Slot,
                                                     HotPlugEvent,
                                                     HotPlugAction);

            if (!NT_SUCCESS(Status)) {
                break;
            }

            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandSetPowerIndicator,
                                  SlotIndicatorBlinking);

            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandQueueAbortTimer,
                                  0);

            Slot->CurrentState = SlotStateWaitingWhileDisabled;
            break;
        }

        //
        // If there is no power indicator then there is no way for a user to
        // cancel the previous attention button press. Fall through to proceed
        // with powering on the slot.
        //

    case SlotEventEnableRequest:

        //
        // An Enable request differs from an attention request in that there is
        // no opportunity for cancellation. Simply turn on the slot.
        //

        if (Slot->Controller->GetDevicePresenceState(Slot) == FALSE) {
            break;
        }

        Status = PciCreateHotPlugActionFromEvent(Slot,
                                                 HotPlugEvent,
                                                 HotPlugAction);

        if (!NT_SUCCESS(Status)) {
            break;
        }

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetLinkState,
                              LinkStateEnable);

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetSlotPower,
                              SlotPowerOn);

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandQueueLinkActiveTimer,
                              0);

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetPowerIndicator,
                              SlotIndicatorBlinking);

        //
        // Leaving the disabled state causes operational problems to be
        // cleared.
        //

        AttentionIndicator =
            PciGetNextAttentionIndicatorState(Slot,
                                              LeavingDisabled);

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetAttentionIndicator,
                              AttentionIndicator);

        Slot->CurrentState = SlotStateLinkActiveTimerQueued;
        break;

    case SlotEventTimerExpired:

        //
        // Ignore timer expired events.
        //

        break;

    default:

        PCI_ASSERT(FALSE);

        break;
    }

    return;
}

VOID
PciProcessEventInWaitingWhileDisabledState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    )

/*++

Routine Description:

    This routine is invoked to handle a hot plug event for a slot when it is in
    the 'waiting while disabled' state.

Parameters:

    Slot - Supplies a pointer to a hot plug slot.

    HotPlugEvent - Supplies a pointer to a hot plug event.

    HotPlugAction - Supplies a place holder for a hot plug action.

Return Value:

    None.

--*/

{

    PCI_HOTPLUG_INDICATOR_STATE AttentionIndicator;
    NTSTATUS Status;

    NON_PAGED_CODE();

    *HotPlugAction = NULL;

    //
    // State: WaitingWhileDisabled.
    //
    // A slot is in this state when a user has requested that the slot be
    // enabled, and the 5 second period during which the user can cancel this
    // request has not expired.
    //
    // Properties:
    //      Slot State is Disabled.
    //      Power Indicator is blinking.
    //      Attention Button interrupts are unmasked.
    //      Abort Timer has been queued.
    //

    switch (HotPlugEvent->Type) {
    case SlotEventAttentionRequest:

        //
        // A second attention request arrived, so cancel the first one and
        // don't enable the slot.
        //

        KeCancelTimer(&Slot->Timer);
        Status = PciCreateHotPlugActionFromEvent(Slot,
                                                 HotPlugEvent,
                                                 HotPlugAction);

        if (!NT_SUCCESS(Status)) {
            break;
        }

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetPowerIndicator,
                              SlotIndicatorOff);

        Slot->CurrentState = SlotStateDisabled;
        break;

    case SlotEventTimerExpired:

        //
        // No attention request appeared in the 5 second interval, so proceed
        // with enabling the slot.
        //
        // N. B. The SHPC specification defines interim steps to check for the
        //       device/slot compatibility. For the time being, this is not
        //       being implemented.
        //

        Status = PciCreateHotPlugActionFromEvent(Slot,
                                                 HotPlugEvent,
                                                 HotPlugAction);

        if (!NT_SUCCESS(Status)) {
            break;
        }

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetLinkState,
                              LinkStateEnable);

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetSlotPower,
                              SlotPowerOn);

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandQueueLinkActiveTimer,
                              0);

        Slot->CurrentState = SlotStateLinkActiveTimerQueued;

        //
        // Leaving the disabled state causes operational problems to be
        // cleared.
        //

        AttentionIndicator =
            PciGetNextAttentionIndicatorState(Slot,
                                              LeavingDisabled);

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetAttentionIndicator,
                              AttentionIndicator);

        break;

    default:

        PCI_ASSERT(FALSE);

        break;
    }

    return;
}

VOID
PciProcessEventInPoweredState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    )

/*++

Routine Description:

    This routine is invoked to handle a hot plug event for a slot when it is in
    the 'powered' state.

Parameters:

    Slot - Supplies a pointer to a hot plug slot.

    HotPlugEvent - Supplies a pointer to a hot plug event.

    HotPlugAction - Supplies a place holder for a hot plug action.

Return Value:

    None.

--*/

{

    NON_PAGED_CODE();

    UNREFERENCED_PARAMETER(Slot);

    *HotPlugAction = NULL;

    //
    // State: SlotStatePowered.
    //
    // A slot is in this state when a controller command to power the slot has
    // been issued. Any abort time that might have been observed has ended.
    //
    // Properties:
    //      Slot Power is on.
    //      Power Indicator is blinking.
    //

    switch (HotPlugEvent->Type) {
    default:

        PCI_ASSERT(FALSE);

        break;
    }

    return;
}

VOID
PciProcessEventInLinkActiveTimerQueuedState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    )

/*++

Routine Description:

    This routine is invoked to handle a hot plug event for a slot when it is in
    the 'link active timer queued' state.

Parameters:

    Slot - Supplies a pointer to a hot plug slot.

    HotPlugEvent - Supplies a pointer to a hot plug event.

    HotPlugAction - Supplies a place holder for a hot plug action.

Return Value:

    None.

--*/

{

    NTSTATUS Status;
    BOOLEAN TimerDequeued;

    NON_PAGED_CODE();

    *HotPlugAction = NULL;

    //
    // State: SlotStateLinkActiveTimerQueued.
    //
    // A slot is in this state when the slot has been enabled but has not
    // waited for the data link layer to become active
    //
    // Properties:
    //      Slot is powered.
    //      Power Indicator is blinking.
    //      Link active timer has been queued.
    //

    switch (HotPlugEvent->Type) {
    case SlotEventLinkStateActive:
        TimerDequeued = KeCancelTimer(&Slot->Timer);
        if (TimerDequeued == FALSE) {

            //
            // The timer DPC has begun execution. Bail out as a timer expired
            // event will be queued shortly. Any action for a link state active
            // event will be handled in the context of this timer event.
            //

            break;
        }

        //
        // Fall through.
        //

    case SlotEventTimerExpired:

        //
        // The data link layer is active or assumed to be so. Queue a settle
        // timer before any config access to the device. A settle timer is
        // a 100 ms delay mandated by the spec. The delay is after the data
        // link layer becomes active but before any config access request can
        // be sent to a device.
        //

        Status = PciCreateHotPlugActionFromEvent(Slot,
                                                 HotPlugEvent,
                                                 HotPlugAction);

        if (!NT_SUCCESS(Status)) {
            break;
        }

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandQueueSettleTimer,
                              0);

        Slot->CurrentState = SlotStateSettleTimerQueued;
        break;

    default:

        PCI_ASSERT(FALSE);

        break;
    }

    return;
}

VOID
PciProcessEventInSettleTimerQueuedState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    )

/*++

Routine Description:

    This routine is invoked to handle a hot plug event for a slot when it is in
    the 'settle timer queued' state.

Parameters:

    Slot - Supplies a pointer to a hot plug slot.

    HotPlugEvent - Supplies a pointer to a hot plug event.

    HotPlugAction - Supplies a place holder for a hot plug action.

Return Value:

    None.

--*/

{

    NTSTATUS Status;

    NON_PAGED_CODE();

    *HotPlugAction = NULL;

    //
    // State: SlotStateSettleTimerQueued.
    //
    // A slot is in this state when the slot has been enabled but has not
    // waited the required settle time before it can be considered fully
    // functional
    //
    // Properties:
    //      Slot is powered and bus is connected .
    //      Power Indicator is blinking.
    //      Settle timer has been queued.
    //

    switch (HotPlugEvent->Type) {
    case SlotEventTimerExpired:

        //
        // The device has settled and is ready to go. Tell PnP about it and
        // turn on the light to indicate that the enabling process is complete.
        //

        Status = PciCreateHotPlugActionFromEvent(Slot,
                                                 HotPlugEvent,
                                                 HotPlugAction);

        if (!NT_SUCCESS(Status)) {
            break;
        }

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandInvalidateBusRelations,
                              0);

        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetPowerIndicator,
                              SlotIndicatorOn);

        Slot->CurrentState = SlotStateFullyFunctional;
        break;

    default:

        PCI_ASSERT(FALSE);

        break;
    }

    return;
}

VOID
PciProcessEventInFullyFunctionalState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    )

/*++

Routine Description:

    This routine is invoked to handle a hot plug event for a slot when it is in
    the 'fully functional' state.

Parameters:

    Slot - Supplies a pointer to a hot plug slot.

    HotPlugEvent - Supplies a pointer to a hot plug event.

    HotPlugAction - Supplies a place holder for a hot plug action.

Return Value:

    None.

--*/

{

    PDEVICE_OBJECT DeviceObject;
    KLOCK_QUEUE_HANDLE LockHandle;
    NTSTATUS Status;

    NON_PAGED_CODE();

    *HotPlugAction = NULL;

    //
    // State: SlotStateFullyFunctional.
    //
    // A slot is in this state when it is fully enabled, and PnP has been
    // notified of the new device that has just appeared in the slot.
    //
    // Properties:
    //      Slot is powered and bus connected.
    //      Power Indicator is on.
    //

    switch (HotPlugEvent->Type) {
    case SlotEventAttentionRequest:
        if (Slot->PowerIndicatorPresent != FALSE) {

            //
            // Since there's a power indicator, blink it and let the user have
            // 5 seconds to cancel the attention request by sending another.
            //

            Status = PciCreateHotPlugActionFromEvent(Slot,
                                                     HotPlugEvent,
                                                     HotPlugAction);

            if (!NT_SUCCESS(Status)) {
                break;
            }

            (*HotPlugAction)->FailureType = ActionFailureEnable;
            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandSetPowerIndicator,
                                  SlotIndicatorBlinking);

            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandQueueAbortTimer,
                                  0);

            Slot->CurrentState = SlotStateWaitingWhileEnabled;
            break;
        }

        //
        // If there is no power indicator present, don't do a 5 second wait
        // because it is not possible for the user to cancel this operation.
        // Fall through.
        //

    case SlotEventDisableRequest:

        //
        // A disable request differs from an attention request in that there
        // is no opportunity for cancellation.
        //

        Status = PciCreateHotPlugActionFromEvent(Slot,
                                                 HotPlugEvent,
                                                 HotPlugAction);

        if (!NT_SUCCESS(Status)) {
            break;
        }

        DeviceObject = NULL;
        KeAcquireInStackQueuedSpinLock(&Slot->DeviceLock, &LockHandle);
        if (Slot->Device != NULL) {
            DeviceObject = Slot->Device->DeviceObject;
            ObReferenceObject(DeviceObject);
        }

        KeReleaseInStackQueuedSpinLock(&LockHandle);
        if (DeviceObject != NULL) {

            //
            // Request that this device be ejected from the system.
            //

            (*HotPlugAction)->FailureType = ActionFailureIgnore;
            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandSetPowerIndicator,
                                  SlotIndicatorBlinking);

            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandRequestDeviceEject,
                                  (ULONG_PTR)DeviceObject);

            //
            // Since a device eject command has been queued, we need to ensure
            // that the device will be around when the command runs so that the
            // pointer passed to IoRequestDeviceEjectEx will be valid. Continue
            // to hold a reference to the device object.
            //

            Slot->CurrentState = SlotStateEjectRequested;

        } else {

            //
            // There's no device enumerated for this slot. Nothing to report
            // to PnP.
            //

            if (Slot->Controller->PciBridge->LinkDisableOnSlotPowerDown != 0) {
                PciAddCommandToAction(*HotPlugAction,
                                      SlotCommandSetLinkState,
                                      LinkStateDisable);
            }

            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandSetSlotPower,
                                  SlotPowerOff);

            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandQueuePowerOffTimer,
                                  0);

            Slot->CurrentState = SlotStatePowerOffTimerQueued;
        }

        break;

    case SlotEventEnableRequest:

        //
        // An 'enable' request will put a slot in a 'fully functional' state.
        // As this slot is already in a functional state, this event is being
        // ignored.
        //

        break;

    default:

        PCI_ASSERT(FALSE);

        break;
    }

    return;
}

VOID
PciProcessEventInWaitingWhileEnabledState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    )

/*++

Routine Description:

    This routine is invoked to handle a hot plug event for a slot when it is in
    the 'waiting while enabled' state.

Parameters:

    Slot - Supplies a pointer to a hot plug slot.

    HotPlugEvent - Supplies a pointer to a hot plug event.

    HotPlugAction - Supplies a place holder for a hot plug action.

Return Value:

    None.

--*/

{

    PDEVICE_OBJECT DeviceObject;
    KLOCK_QUEUE_HANDLE LockHandle;
    NTSTATUS Status;

    NON_PAGED_CODE();

    *HotPlugAction = NULL;

    //
    // State: WaitingWhileEnabled.
    //
    // A slot is in this state when a user has requested that the slot be
    // disabled, and the 5 second period during which the user can cancel
    // this request has not expired.
    //
    // Properties:
    //      Slot State is Enabled.
    //      Power Indicator is blinking.
    //      Attention Button interrupts are unmasked.
    //      Abort Timer has been queued.
    //

    switch (HotPlugEvent->Type) {
    case SlotEventAttentionRequest:

        //
        // A second attention request arrived, cancelling the first. Return
        // back to the fully functional state.
        //

        Status = PciCreateHotPlugActionFromEvent(Slot,
                                                 HotPlugEvent,
                                                 HotPlugAction);

        if (!NT_SUCCESS(Status)) {
            break;
        }

        KeCancelTimer(&Slot->Timer);
        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetPowerIndicator,
                              SlotIndicatorOn);

        Slot->CurrentState = SlotStateFullyFunctional;
        break;

    case SlotEventTimerExpired:

        //
        // No cancellation occurred. Process the eject.
        //

        Status = PciCreateHotPlugActionFromEvent(Slot,
                                                 HotPlugEvent,
                                                 HotPlugAction);

        if (!NT_SUCCESS(Status)) {
            break;
        }

        DeviceObject = NULL;
        KeAcquireInStackQueuedSpinLock(&Slot->DeviceLock, &LockHandle);
        if (Slot->Device != NULL) {
            DeviceObject = Slot->Device->DeviceObject;
            ObReferenceObject(DeviceObject);
        }

        KeReleaseInStackQueuedSpinLock(&LockHandle);
        if (DeviceObject != NULL) {

            //
            // There is a device in the slot, so report the ejection request
            // to PnP.  Set an abort timer so that if the request fails we
            // don't hang (there is no failure notification)
            //

            (*HotPlugAction)->FailureType = ActionFailureIgnore;
            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandRequestDeviceEject,
                                  (ULONG_PTR)DeviceObject);

            //
            // Since a DeviceEject command has been queued, we need to ensure
            // that the device will be around when the command runs so that
            // its remove lock will still be resident in memory.
            //

            Slot->CurrentState = SlotStateEjectRequested;

        } else {

            //
            // There is no device in the slot. Just turn it off.
            //

            if (Slot->Controller->PciBridge->LinkDisableOnSlotPowerDown != 0) {
                PciAddCommandToAction(*HotPlugAction,
                                      SlotCommandSetLinkState,
                                      LinkStateDisable);
            }

            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandSetSlotPower,
                                  SlotPowerOff);

            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandQueuePowerOffTimer,
                                  0);

            Slot->CurrentState = SlotStatePowerOffTimerQueued;
        }

        break;

    default:

        PCI_ASSERT(FALSE);

        break;
    }

    return;
}

VOID
PciProcessEventInPowerOffTimerQueuedState (
    __inout PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT HotPlugEvent,
    __out PPCI_HOTPLUG_ACTION *HotPlugAction
    )

/*++

Routine Description:

    This routine is invoked to handle a hot plug event for a slot when it is in
    the 'power off timer queued' state.

Parameters:

    Slot - Supplies a pointer to a hot plug slot.

    HotPlugEvent - Supplies a pointer to a hot plug event.

    HotPlugAction - Supplies a place holder for a hot plug action.

Return Value:

    None.

--*/

{

    NTSTATUS Status;

    NON_PAGED_CODE();

    *HotPlugAction = NULL;

    //
    // State: PowerOffTimerQueued.
    //
    // A slot is in this state after a command to power it off has
    // executed and a timer for power off has fired.
    //

    switch (HotPlugEvent->Type) {
    case SlotEventTimerExpired:
        Status = PciCreateHotPlugActionFromEvent(Slot,
                                                 HotPlugEvent,
                                                 HotPlugAction);

        if (!NT_SUCCESS(Status)) {
            break;
        }

        (*HotPlugAction)->FailureType = ActionFailureIgnore;
        PciAddCommandToAction(*HotPlugAction,
                              SlotCommandSetPowerIndicator,
                              SlotIndicatorOff);

        if (((Slot->TimerFlags & PCI_HOTPLUG_SLOT_INVALIDATE_DEVICE) != 0) &&
            (Slot->Device != NULL)) {

            PciAddCommandToAction(*HotPlugAction,
                                  SlotCommandInvalidateBusRelations,
                                  0);

        }

        if (((Slot->TimerFlags & PCI_HOTPLUG_SLOT_UNPRIMED) != 0) ||
            ((Slot->MRLSensorPresent != FALSE) &&
             (Slot->Controller->GetDevicePresenceState(Slot) == FALSE))) {

            Slot->CurrentState = SlotStateNotReady;

        } else {
            Slot->CurrentState = SlotStateDisabled;
        }

        Slot->TimerFlags = 0;
        break;

    case SlotEventLinkStateNotActive:

        //
        // This event indicates that the link went down after it was powered
        // off. There is no change in the slot state in response to this event.
        //

        break;

    case SlotEventLinkStateActive:
    case SlotEventEnableRequest:

        //
        // Transient events which may have been queued while the device was
        // being powered off.
        //

        break;

    default:

        PCI_ASSERT(FALSE);

        break;
    }

    return;
}

//------------------------------------------ Controller Action/Command Functions

NTSTATUS
PciCreateHotPlugActionFromEvent (
    __in PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT Event,
    __out PPCI_HOTPLUG_ACTION *Action
    )

/*++

Routine Description:

    This routine allocates and initializes a new hotplug action for placement
    on the hotplug controller's action queue. Actions created by this routine
    occur as a result of slot state machine transitions, which are initiated
    by events.

Arguments:

    Slot - Supplies a pointer to a hot plug slot.

    Event - Supplies the hot plug event that has triggered an action.

    Action - Supplies a place holder for a new action in response to the hot
        plug event.

Return Value:

    Same as the value returned from a function call.

--*/

{

    NON_PAGED_CODE();

    //
    // Call the base routine for creating actions.
    //
    // ActionFailureDisable - By default, if the action fails either because a
    // hardware command failed or there was an internal processing error, the slot
    // will be disabled to prevent it from getting into an unpredictable state.
    // This can be overridden by the caller if necessary.
    //
    // Event->Callback - The event isn't complete until the action that gets
    // queued on its behalf is complete.  Copy over the callback so that it
    // can be executed at action completion time.
    //

    return PciCreateHotPlugAction(Slot->Controller,
                                  Slot,
                                  ActionFailureDisable,
                                  Event->Callback,
                                  Event->CallbackContext,
                                  Action);

}

NTSTATUS
PciCreateHotPlugAction (
    __in PPCI_HOTPLUG_CONTROLLER Controller,
    __in_opt PPCI_HOTPLUG_SLOT Slot,
    __in PCI_HOTPLUG_ACTION_FAILURE_TYPE FailureType,
    __in_opt PPCI_HOTPLUG_ACTION_CALLBACK Callback,
    __in_opt PVOID CallbackContext,
    __out PPCI_HOTPLUG_ACTION *Action
    )

/*++

Routine Description:

    This routine creates and initializes a hotplug action.

Arguments:

    Controller - Supplies a pointer to a hot plug controller.

    Slot - Supplies a pointer to a hot plug slot. If present, this action
        corresponds to a hot plug slot otherwise it corresponds to a hot plug
        controller.

    FailureType - Supplies the severity grade on failure to execute an action.

    Callback - Supplies a callback routine to invoke when the action completes.

    CallbackContext - Supplies a context for the callback routine.

    Action - Supplies a place holder for a new hot plug action.

Return Value:

    STATUS_INSUFFICIENT_RESOURCE on failure to allocate memory for a hot plug
        action.

    STATUS_SUCCESS on success.

--*/

{

    ULONG Index;
    PPCI_HOTPLUG_ACTION NewAction;

    NON_PAGED_CODE();

    *Action = NULL;
    NewAction = PciAllocatePool(NonPagedPoolNx, sizeof(PCI_HOTPLUG_ACTION));
    if (NewAction == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(NewAction, sizeof(PCI_HOTPLUG_ACTION));
    NewAction->Controller = Controller;
    NewAction->Slot = Slot;
    NewAction->FailureType = FailureType;
    NewAction->Callback = Callback;
    NewAction->CallbackContext = CallbackContext;

    for (Index = 0; Index < PCI_MAXIMUM_HOTPLUG_COMMAND_COUNT; Index++) {
        NewAction->Commands[Index].Type = SlotCommandNoop;
    }

    *Action = NewAction;
    return STATUS_SUCCESS;
}

VOID
PciAddCommandToAction (
    __inout PPCI_HOTPLUG_ACTION Action,
    __in PCI_HOTPLUG_COMMAND_TYPE Type,
    __in ULONG_PTR Context
    )

/*++

Routine Description:

    This routine adds a new command at the end of a given action. Actions are
    comprised of a number of commands that must all be executed in sequence for
    the action to be complete.

Arguments:

    Action - Supplies an action for the command.

    Type - Supplies the type of command to append to an action.

    Context - Supplies a context for the command.

Return Value:

    None.

--*/

{

    ULONG Index;

    NON_PAGED_CODE();

    for (Index = 0; Index < PCI_MAXIMUM_HOTPLUG_COMMAND_COUNT; Index++) {
        if (Action->Commands[Index].Type == SlotCommandNoop) {
            Action->Commands[Index].Type = Type;
            Action->Commands[Index].Context = Context;
            return;
        }
    }

    PCI_ASSERT(Index != PCI_MAXIMUM_HOTPLUG_COMMAND_COUNT);

    return;
}

BOOLEAN
PciProcessHotPlugCommand (
    __inout PPCI_HOTPLUG_CONTROLLER Controller,
    __inout PPCI_HOTPLUG_SLOT Slot,
    __inout PPCI_HOTPLUG_ACTION Action,
    __in PPCI_HOTPLUG_COMMAND Command
    )

/*++

Routine Description:

    This routine executes a hotplug command as part of a given action.
    The specifics of the command processing is highly dependent on the
    command type.

Arguments:

    Controller - Supplies a pointer to a hot plug controller.

    Slot - Supplies a pointer to a hot plug slot.

    Action - Supplies a pointer to a hot plug action.

    Command - Supplies a pointer to a hot plug command.

Return Value:

    TRUE if the processing of the command will not complete until a command
        complete interrupt occurs. The command is therefore pending.

    FALSE if the command completes instantaneously.

--*/

{

    PDEVICE_OBJECT DeviceObject;
    LARGE_INTEGER DueTime;
    KIRQL OldIrql;
    BOOLEAN Pending;
    NTSTATUS Status;

    NON_PAGED_CODE();

    UNREFERENCED_PARAMETER(Action);

    switch (Command->Type) {
    case SlotCommandSetSlotPower:
    case SlotCommandSetPowerIndicator:
    case SlotCommandSetAttentionIndicator:

        //
        // These commands are handled in a controller-specific way, unless they
        // are spurious commands that can be completed immediately.
        //

        if (((Command->Type == SlotCommandSetSlotPower) &&
             (Slot->PowerControllerPresent == FALSE)) ||
            ((Command->Type == SlotCommandSetPowerIndicator) &&
             (Slot->PowerIndicatorPresent == FALSE)) ||
            ((Command->Type == SlotCommandSetAttentionIndicator) &&
             (Slot->AttentionIndicatorPresent == FALSE))) {

            Pending = FALSE;

        } else {
            Pending = Controller->ExecuteHotPlugCommands(Slot,
                                                         Command->Type,
                                                         Command->Context);

        }

        //
        // If a command is pending, set a 1 second timer. If we don't get a
        // command complete interrupt in a second, move on.  If the command
        // is a slot power command, a worker has been queued which will be
        // responsible for setting a command complete timeout.
        //

        if ((Pending != FALSE) && (Command->Type != SlotCommandSetSlotPower)) {
            KeAcquireSpinLock(&Controller->TimerLock, &OldIrql);
            if (Controller->TimersDisabled == FALSE) {
                DueTime.QuadPart = -10 * 1000 * 1000;
                KeSetTimer(&Controller->CommandTimer,
                        DueTime,
                        &Controller->CommandTimerDpc);

            }

            KeReleaseSpinLock(&Controller->TimerLock, OldIrql);

        }

        break;

    case SlotCommandEnableDisableInterrupts:

        //
        // The EnableDisableInterrupts command can either refer to the global
        // controller interrupts or slot interrupts.
        //

        if (Slot != NULL) {

            //
            // Enable/disable slot interrupts. The specific bits to enable/
            // disable are indicated by the Context.
            //

            Pending = Controller->ExecuteHotPlugCommands(Slot,
                                                         Command->Type,
                                                         Command->Context);

        } else {

            //
            // Enable/disable global interrupts. The Context indicates whether
            // this is an enable or disable operation.
            //

            Pending = Controller->EnableDisableControllerInterrupts(
                            Controller,
                            (BOOLEAN)Command->Context);

        }

        //
        // If a command is pending, set a 1 second timer. If we don't get a
        // command complete interrupt in a second, move on.
        //

        if (Pending != FALSE) {
            KeAcquireSpinLock(&Controller->TimerLock, &OldIrql);
            if (Controller->TimersDisabled == FALSE) {
                DueTime.QuadPart = -10 * 1000 * 1000;
                KeSetTimer(&Controller->CommandTimer,
                        DueTime,
                        &Controller->CommandTimerDpc);

            }

            KeReleaseSpinLock(&Controller->TimerLock, OldIrql);

        }

        break;

    case SlotCommandSetLinkState:
        Pending = Controller->ExecuteHotPlugCommands(Slot,
                                                     Command->Type,
                                                     Command->Context);

        //
        // This command queues a worker that is responsible for setting
        // the timer which will unblock the action queue.  Therefore, we
        // do not set a timer here.
        //

        break;

    case SlotCommandQueueAbortTimer:

        //
        // The abort timer is a 5 second timer.
        //

        KeAcquireSpinLock(&Controller->TimerLock, &OldIrql);
        if (Controller->TimersDisabled == FALSE) {
            DueTime.QuadPart = -5 * 10* 1000 * 1000;
            KeSetTimer(&Slot->Timer, DueTime, &Slot->TimerDpc);

        }

        KeReleaseSpinLock(&Controller->TimerLock, OldIrql);
        Pending = FALSE;
        break;

    case SlotCommandQueueLinkActiveTimer:

        //
        // The link active timer is a 1 second timer.
        //

        KeAcquireSpinLock(&Controller->TimerLock, &OldIrql);
        if (Controller->TimersDisabled == FALSE) {
            DueTime.QuadPart = -1 * 10 * 1000 * 1000;
            KeSetTimer(&Slot->Timer, DueTime, &Slot->TimerDpc);

        }

        KeReleaseSpinLock(&Controller->TimerLock, OldIrql);
        Pending = FALSE;
        break;

    case SlotCommandQueueSettleTimer:

        //
        // The settle timer is a 100 ms timer.
        //

        KeAcquireSpinLock(&Controller->TimerLock, &OldIrql);
        if (Controller->TimersDisabled == FALSE) {
            DueTime.QuadPart = -1 * 1000 * 1000;
            KeSetTimer(&Slot->Timer, DueTime, &Slot->TimerDpc);

        }

        KeReleaseSpinLock(&Controller->TimerLock, OldIrql);
        Pending = FALSE;
        break;

    case SlotCommandQueuePowerOffTimer:

        //
        // The power off timer is a 1 second timer. Queue only when a power
        // controller is present.
        //

        Slot->TimerFlags = (ULONG)Command->Context;
        if (Slot->PowerControllerPresent != FALSE) {
            KeAcquireSpinLock(&Controller->TimerLock, &OldIrql);
            if (Controller->TimersDisabled == FALSE) {
                DueTime.QuadPart = -1 * 10 * 1000 * 1000;
                KeSetTimer(&Slot->Timer, DueTime, &Slot->TimerDpc);

            }

            KeReleaseSpinLock(&Controller->TimerLock, OldIrql);

        } else {
            PciQueueHotPlugEvent(Slot, SlotEventTimerExpired, NULL, NULL);
        }

        Pending = FALSE;
        break;

    case SlotCommandSetEvent:
        KeSetEvent((PKEVENT)Command->Context, IO_NO_INCREMENT, FALSE);
        Pending = FALSE;
        break;

    case SlotCommandInvalidateBusRelations:
        IoInvalidateDeviceRelations(
            Slot->Controller->PciBridge->Bus->PhysicalDeviceObject,
            BusRelations);

        Pending = FALSE;
        break;

    case SlotCommandRequestDeviceEject:

        DeviceObject = (PDEVICE_OBJECT)Command->Context;

        //
        // Holding a reference on a device prevents the device from being
        // deleted.
        //

        Status = IoRequestDeviceEjectEx(DeviceObject,
                                        PciDeviceEjectCallback,
                                        Slot,
                                        NULL);


        if (!NT_SUCCESS(Status)) {

            //
            // On failure to request a device eject, queue a new event to the
            // slot that will trigger the same functionality as a device eject
            // Irp.
            //

            PciQueueHotPlugEvent(Slot, SlotEventStackTornDown, NULL, NULL);
        }

        ObDereferenceObject(DeviceObject);
        Pending = FALSE;
        break;

    case SlotCommandNoop:
    default:

        Pending = FALSE;
        break;
    }

    return Pending;
}

NTSTATUS
PciCreateControllerInterruptAction (
    __in PPCI_HOTPLUG_CONTROLLER Controller,
    __in BOOLEAN Enable,
    __out PPCI_HOTPLUG_ACTION *Action
    )

/*++

Routine Description:

    This routine creates a command action to enable/disable interrupts for a
    given hotplug controller.

Arguments:

    Controller - Supplies a pointer to a hot plug controller.

    Enable - Supplies a boolean indicating whether the interrupts are to be
        enabled or disabled.

    Action - Supplies a place holder for a hot plug action.

Return Value:

--*/

{

    NTSTATUS Status;

    NON_PAGED_CODE();

    Status = PciCreateHotPlugAction(Controller,
                                    NULL,
                                    ActionFailureIgnore,
                                    NULL,
                                    NULL,
                                    Action);

    if (NT_SUCCESS(Status)) {
        PciAddCommandToAction(*Action,
                              SlotCommandEnableDisableInterrupts,
                              Enable);

    }

    return Status;
}

NTSTATUS
PciCreateSlotInterruptsAction (
    __in PPCI_HOTPLUG_SLOT Slot,
    __in BOOLEAN Enable,
    __out PPCI_HOTPLUG_ACTION *Action
    )

/*++

Routine Description:

    This routine creates a command action to enable/disable interrupts for a
    given slot.

Arguments:

    Slot - Supplies a pointer to a hot plug slot.

    Enable - Supplies a boolean indicating whether the slot interrupts are to
        be enabled or disabled.

    Action - Supplies a place holder for a hot plug action.

Return Value:

--*/

{

    ULONG EventMapIndex;
    ULONG Index;
    UCHAR SlotInterrupts;
    NTSTATUS Status;

    NON_PAGED_CODE();

    SlotInterrupts = 0;
    if (Enable != FALSE) {

        //
        // Determine which interrupts should be enabled. Only enable those
        // interrupts that have valid mappings and will be useful to the slot
        // state machine.
        //

        for (Index = SlotInterruptPresenceDetect;
             Index < SlotInterruptMaximum;
             Index++) {

            EventMapIndex = EVENT_MAP_INDEX(Index, FALSE);
            if (Slot->EventMap[EventMapIndex] != SlotEventNone) {
                SlotInterrupts |= (1 << Index);
            }
        }
    }

    Status = PciCreateHotPlugAction(Slot->Controller,
                                    Slot,
                                    ActionFailureIgnore,
                                    NULL,
                                    NULL,
                                    Action);
    if (NT_SUCCESS(Status)) {
        PciAddCommandToAction(*Action,
                              SlotCommandEnableDisableInterrupts,
                              SlotInterrupts);

    }

    return Status;
}

VOID
PciHandleHotPlugCommandFailure (
    __inout PPCI_HOTPLUG_ACTION Action,
    __in PPCI_HOTPLUG_COMMAND Command
    )

/*++

Routine Description:

    This routine handles failure of a hotplug command. The handling of this
    failure is different based on the setting of the FailureType in the action.

Arguments:

    Action - Supplies a pointer to a hotplug action.

    Command - Supplies a pointer to a command contained in an action.

Return Value:

    None.

--*/

{

    PPCI_HOTPLUG_EVENT HotPlugEvent;
    KLOCK_QUEUE_HANDLE SlotHandle;

    NON_PAGED_CODE();

    UNREFERENCED_PARAMETER(Command);

    //
    // For the ignore failure type, pretend the failure didn't happen.
    //

    if (Action->FailureType == ActionFailureIgnore) {
        return;
    }

    //
    // Create the event structure.
    //

    HotPlugEvent = PciAllocatePool(NonPagedPoolNx, sizeof(PCI_HOTPLUG_EVENT));
    if (HotPlugEvent == NULL) {
        return;
    }

    RtlZeroMemory(HotPlugEvent, sizeof(PCI_HOTPLUG_EVENT));
    switch (Action->FailureType) {
    case ActionFailureDisable:

        //
        // Disable the slot, but don't go through the normal sequence. Simply
        // turn it off.
        //

        HotPlugEvent->Type = SlotEventForceDisable;
        break;

    case ActionFailureEnable:

        //
        // Enable the slot, but don't go through the normal sequence. Simply
        // turn it on. Note that this failure type should be used judiciously,
        // as it can potentially result in bus speed incompatibilities if used
        // inappropriately.
        //

        HotPlugEvent->Type = SlotEventForceEnable;
        break;

    default:

        PCI_ASSERT(FALSE);
    }

    //
    // Add a hot plug event to the slot event queue.
    //

    PCI_ASSERT(Action->Slot != NULL);

    KeAcquireInStackQueuedSpinLock(&Action->Slot->QueueLock, &SlotHandle);
    InsertTailList(&Action->Slot->EventQueue, &HotPlugEvent->ListEntry);
    KeReleaseInStackQueuedSpinLock(&SlotHandle);
    return;
}

_Function_class_(KDEFERRED_ROUTINE)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_min_(DISPATCH_LEVEL)
_IRQL_requires_(DISPATCH_LEVEL)
_IRQL_requires_same_
VOID
PciHotPlugCommandTimer (
    _In_ struct _KDPC *Dpc,
    _In_opt_ PVOID DeferredContext,
    _In_opt_ PVOID SystemArgument1,
    _In_opt_ PVOID SystemArgument2
    )

/*++

Routine Description:

    This routine is a hot plug controller command complete timer. It simulates
    the arrival of a command complete interrupt and thus enables progress in
    the action queue processing.

Arguments:

    Dpc - Supplies a pointer to a command timer dpc.

    DeferredContext - Supplies a pointer to a hot plug controller.

    SystemArgument1 - Unused.

    SystemArgument2 - Unused.

Return Value:

    None.

--*/

{

    PPCI_BUS Bus;
    PPCI_HOTPLUG_CONTROLLER HotPlugController;
    PCI_HOTPLUG_INTERRUPTS HotPlugInterrupts;
    NTSTATUS Status;

    NON_PAGED_CODE();

    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);

    HotPlugController = (PPCI_HOTPLUG_CONTROLLER)DeferredContext;
    _Analysis_assume_(HotPlugController != NULL);

    Bus = HotPlugController->PciBridge->Bus;
    Status = IoAcquireRemoveLock(&Bus->RemoveLock, NULL);
    if (!NT_SUCCESS(Status)) {
        return;
    }

    HotPlugInterrupts.CommandCompleteInterrupt = 1;
    HotPlugInterrupts.SlotMask = 0;
    PciProcessHotPlugInterrupts(HotPlugController, &HotPlugInterrupts);
    IoReleaseRemoveLock(&Bus->RemoveLock, NULL);
    return;
}

_Function_class_(KDEFERRED_ROUTINE)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_min_(DISPATCH_LEVEL)
_IRQL_requires_(DISPATCH_LEVEL)
_IRQL_requires_same_
VOID
PciHotPlugSlotTimer (
    _In_ struct _KDPC *Dpc,
    _In_opt_ PVOID DeferredContext,
    _In_opt_ PVOID SystemArgument1,
    _In_opt_ PVOID SystemArgument2
    )

/*++

Routine Description:

    This is a timer dpc routine queued for some of the slot events.

Arguments:

    Dpc - Supplies a pointer to a slot timer dpc.

    DeferredContext - Supplies a pointer to a hot plug slot.

    SystemArgument1 - Unused.

    SystemArgument2 - Unused.

Return Value:

    None.

--*/

{

    PPCI_BUS Bus;
    PPCI_HOTPLUG_SLOT HotPlugSlot;
    NTSTATUS Status;

    NON_PAGED_CODE();

    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);

    HotPlugSlot = (PPCI_HOTPLUG_SLOT)DeferredContext;
    _Analysis_assume_(HotPlugSlot != NULL);

    Bus = HotPlugSlot->Controller->PciBridge->Bus;
    Status = IoAcquireRemoveLock(&Bus->RemoveLock, NULL);
    if (!NT_SUCCESS(Status)) {
        return;
    }

    Status = PciQueueHotPlugEvent(HotPlugSlot,
                                  SlotEventTimerExpired,
                                  NULL,
                                  NULL);

    if (!NT_SUCCESS(Status)) {
        return;
    }

    PciProcessHotPlugQueues(HotPlugSlot->Controller);
    IoReleaseRemoveLock(&Bus->RemoveLock, NULL);
    return;
}

//---------------------------------------------------------------------- Miscell

NTSTATUS
PciHandleHotPlugDriverRemoval (
    __inout PPCI_HOTPLUG_SLOT Slot
    )

/*++

Routine Description:

    This routine is invoked when the driver stack for a device in a given hot
    plug slot has been torn down. Depending on the device capabilities, this
    can be either after a Remove or Eject IRP.

    This routine feeds this event into the state machine of the slot.

Arguments:

    Slot - Supplies a pointer to a hot plug slot.

Return Value:

    STATUS_SUCCESS on success

    Otherwise a failure code returned from one of the function calls in this
        routine.

--*/

{

    PPCI_BUS Bus;
    KEVENT Event;
    NTSTATUS Status;

    PAGED_CODE();

    Bus = Slot->Controller->PciBridge->Bus;
    Status = IoAcquireRemoveLock(&Bus->RemoveLock, NULL);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    KeInitializeEvent(&Event, SynchronizationEvent, FALSE);

    //
    // Create a new event for this and then process the hot plug queues to
    // ensure the event is handled.
    //

    Status = PciQueueHotPlugEvent(Slot,
                                  SlotEventStackTornDown,
                                  PciSetHotPlugEventCallback,
                                  &Event);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    PciProcessHotPlugQueues(Slot->Controller);

    //
    // Handle this event synchronously.
    //

    KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
    IoReleaseRemoveLock(&Bus->RemoveLock, NULL);
    return STATUS_SUCCESS;
}

VOID
PciDestroyHotPlugController (
    __inout PPCI_HOTPLUG_CONTROLLER *HotPlugController
    )

/*++

Routine Description:

    This routine is invoked to flush the controller and slot queues as well as
    release the memory for a hot plug controller.

Arguments:

    HotPlugController - Supplies a placeholder that contains a pointer to the
        hot plug controller.

Return Value:

--*/

{

    PPCI_HOTPLUG_CONTROLLER Controller;
    KLOCK_QUEUE_HANDLE ControllerLockHandle;
    PPCI_HOTPLUG_ACTION HotPlugAction;
    PPCI_HOTPLUG_EVENT HotPlugEvent;
    PLIST_ENTRY ListHead;
    KIRQL OldIrql;
    PPCI_HOTPLUG_SLOT Slot;
    ULONG SlotIndex;
    KLOCK_QUEUE_HANDLE SlotLockHandle;

    NON_PAGED_CODE();

    Controller = *HotPlugController;

    //
    // Prevent timers from being queued again.
    //

    KeAcquireSpinLock(&Controller->TimerLock, &OldIrql);
    Controller->TimersDisabled = TRUE;
    KeReleaseSpinLock(&Controller->TimerLock, OldIrql);

    //
    // Cancel all timers.
    //

    for (SlotIndex = 0; SlotIndex < Controller->SlotCount; SlotIndex++) {
        Slot = &Controller->Slots[SlotIndex];
        KeCancelTimer(&Slot->Timer);
    }

    KeCancelTimer(&Controller->CommandTimer);

    //
    // Allow all DPCs queued by these timers to complete as the DPCs will
    // operate on structures about to be deallocated.  Once DPCs are flushed,
    // it is guaranteed that the timers are not queued, they will not be
    // queued again, and that any DPCs they have queued are finished running.
    //

    KeFlushQueuedDpcs();

    //
    // Flush all the slot event and controller action queues.
    //

    KeAcquireInStackQueuedSpinLock(&Controller->QueueLock,
                                   &ControllerLockHandle);

    for (SlotIndex = 0; SlotIndex < Controller->SlotCount; SlotIndex++) {
        Slot = &Controller->Slots[SlotIndex];
        KeAcquireInStackQueuedSpinLock(&Slot->QueueLock, &SlotLockHandle);
        while (IsListEmpty(&Slot->EventQueue) == FALSE) {
            ListHead = RemoveHeadList(&Slot->EventQueue);
            HotPlugEvent = CONTAINING_RECORD(ListHead,
                                             PCI_HOTPLUG_EVENT,
                                             ListEntry);

            PCI_ASSERT(HotPlugEvent != NULL);

            TRACE(HOTPLUG_TRACE_LEVEL, "Flushing slot event\n");
            DebugDumpHotplugEvent(Slot, HotPlugEvent);

            PciFreePool(HotPlugEvent);
        }

        KeReleaseInStackQueuedSpinLock(&SlotLockHandle);
    }

    while (IsListEmpty(&Controller->ActionQueue) == FALSE) {
        ListHead = RemoveHeadList(&Controller->ActionQueue);
        HotPlugAction = CONTAINING_RECORD(ListHead,
                                          PCI_HOTPLUG_ACTION,
                                          ListEntry);

        PCI_ASSERT(HotPlugAction != NULL);

        TRACE(HOTPLUG_TRACE_LEVEL, "Flushing hotplug action\n");
        DebugDumpHotplugAction(HotPlugAction);

        PciFreePool(HotPlugAction);
    }

    KeReleaseInStackQueuedSpinLock(&ControllerLockHandle);

    //
    // Release the memory for a hot plug controller.
    //

    PciFreePool(Controller);
    *HotPlugController = NULL;
    return;
}

PCI_HOTPLUG_INDICATOR_STATE
PciGetNextAttentionIndicatorState (
    __in PPCI_HOTPLUG_SLOT Slot,
    __in PCI_ATTENTION_INDICATOR_EVENT_TYPE Reason
    )

/*++

Routine Description:

    This routine provides the Attention Indicator state for a slot based on the
    event for a state transition.

Arguments:

    Slot - Supplies a pointer to a hot plug slot.

    Reason - Supplies the reason that guides the following state for an
        attention indicator.

Return Value:

    Enum for an attention indicator state.

--*/

{

    NON_PAGED_CODE();

    //
    // There are two internal flags that determine the state of the attention
    // indicator. Set them appropriately based on the update we've been given.
    //

    switch (Reason) {
    case LocateRequested:
        Slot->AttentionIndicatorFlags |= PCI_HOTPLUG_SLOT_LOCATE_REQUESTED;
        break;

    case LocateRequestCleared:
        Slot->AttentionIndicatorFlags &= ~PCI_HOTPLUG_SLOT_LOCATE_REQUESTED;
        break;

    case ProblemDetected:
        Slot->AttentionIndicatorFlags |= PCI_HOTPLUG_SLOT_PROBLEM_DETECTED;
        break;

    case LeavingDisabled:
    case ProblemCleared:
        Slot->AttentionIndicatorFlags &= ~PCI_HOTPLUG_SLOT_PROBLEM_DETECTED;
        break;

    default:

        PCI_ASSERT(FALSE);

        break;
    }

    if ((Slot->AttentionIndicatorFlags &
         PCI_HOTPLUG_SLOT_LOCATE_REQUESTED) != 0) {

        //
        // Location takes precedence over a problem, so if we have been asked
        // to locate the slot, make sure the attention indicator is blinking.
        //

        return SlotIndicatorBlinking;

    } else if ((Slot->AttentionIndicatorFlags &
                PCI_HOTPLUG_SLOT_PROBLEM_DETECTED) != 0) {

        //
        // If we are not being asked to locate but there is a problem with the
        // slot, turn the indicator on.
        //

        return SlotIndicatorOn;

    } else {

        //
        // There is no reason to locate the slot. Turn off the indicator.
        //

        return SlotIndicatorOff;
    }
}

VOID
PciDeviceEjectCallback (
    __in NTSTATUS Status,
    __inout_opt PVOID Context
    )

/*++

Routine Description:

    This routine is an eject callback registered with the PnP Mgr.

Arguments:

    Status - Supplies a status value for an eject operation.

    Context - Supplies a pointer to a hot plug slot.

Return Value:

    None.

--*/

{

    PPCI_BUS Bus;
    KLOCK_QUEUE_HANDLE ControllerLockHandle;
    PPCI_HOTPLUG_ACTION HotPlugAction;
    PPCI_HOTPLUG_SLOT HotPlugSlot;
    KLOCK_QUEUE_HANDLE SlotLockHandle;

    NON_PAGED_CODE();

    if (!NT_SUCCESS(Status)) {
        HotPlugSlot = (PPCI_HOTPLUG_SLOT)Context;
        Bus = HotPlugSlot->Controller->PciBridge->Bus;
        Status = IoAcquireRemoveLock(&Bus->RemoveLock, NULL);
        if (!NT_SUCCESS(Status)) {
            return;
        }

        //
        // The eject never came through, so assume it was vetoed and return to
        // the fully functional state.
        //

        Status = PciCreateHotPlugAction(HotPlugSlot->Controller,
                                        HotPlugSlot,
                                        ActionFailureDisable,
                                        NULL,
                                        NULL,
                                        &HotPlugAction);

        if (!NT_SUCCESS(Status)) {
            return;
        }

        PciAddCommandToAction(HotPlugAction,
                              SlotCommandSetPowerIndicator,
                              SlotIndicatorOn);

        KeAcquireInStackQueuedSpinLock(&HotPlugSlot->Controller->QueueLock,
                                       &ControllerLockHandle);

        KeAcquireInStackQueuedSpinLock(&HotPlugSlot->QueueLock,
                                       &SlotLockHandle);

        HotPlugSlot->CurrentState = SlotStateFullyFunctional;
        InsertTailList(&HotPlugSlot->Controller->ActionQueue,
                       &HotPlugAction->ListEntry);

        KeReleaseInStackQueuedSpinLock(&SlotLockHandle);
        KeReleaseInStackQueuedSpinLock(&ControllerLockHandle);
        PciProcessHotPlugQueues(HotPlugSlot->Controller);
        IoReleaseRemoveLock(&Bus->RemoveLock, NULL);
    }

    return;
}

VOID
PciSetHotPlugEventCallback (
    __in PPCI_HOTPLUG_SLOT Slot,
    __inout PVOID Context
    )

/*++

Routine Description:

    This routine is a simple hot plug event completion callback that signals an
    event. This allows the queuing process to wait synchronously for the task
    to conclude.

Arguments:

    Slot - Supplies a pointer to a hot plug slot.

    Context - Supplies an event to signal.

Return Value:

    None.

--*/

{

    PKEVENT Event;

    NON_PAGED_CODE();

    UNREFERENCED_PARAMETER(Slot);

    Event = (PKEVENT)Context;
    KeSetEvent(Event, IO_NO_INCREMENT, FALSE);
    return;
}

//----------------------------------------------------------- Debugger Utilities

#if DBG

static PUCHAR HotplugEventTypeText[] = {

    "SlotEventNone",
    "SlotEventPrimed",
    "SlotEventUnprimed",
    "SlotEventEnableRequest",
    "SlotEventDisableRequest",
    "SlotEventSurpriseDisableRequest",
    "SlotEventAttentionRequest",
    "SlotEventLinkStateActive",
    "SlotEventLinkStateNotActive",
    "SlotEventPowerFault",
    "SlotEventStackTornDown",
    "SlotEventStackTearDownFailed",
    "SlotEventTimerExpired",
    "SlotEventActionCompleted",
    "SlotEventForceEnable",
    "SlotEventForceDisable"
};

static PUCHAR HotplugCommandTypeText[] = {

    "SlotCommandNoop",
    "SlotCommandSetSlotPower",
    "SlotCommandSetPowerIndicator",
    "SlotCommandSetAttentionIndicator",
    "SlotCommandEnableDisableInterrupts",
    "SlotCommandSetLinkState",
    "SlotCommandQueueAbortTimer",
    "SlotCommandQueueSettleTimer",
    "SlotCommandQueueLinkActiveTimer",
    "SlotCommandQueuePowerOffTimer",
    "SlotCommandSetEvent",
    "SlotCommandInvalidateBusRelations",
    "SlotCommandSetBusSpeed",
    "SlotCommandRequestDeviceEject",
    "SlotCommandQueueHotPlugEvent"
};

static PUCHAR HotplugSlotStateText[] = {

    "SlotStateNotReady",
    "SlotStateDisabled",
    "SlotStateWaitingWhileDisabled",
    "SlotStatePowered",
    "SlotStateBusSpeedSet",
    "SlotStateLinkActiveTimerQueued",
    "SlotStateSettleTimerQueued",
    "SlotStateFullyFunctional",
    "SlotStateWaitingWhileEnabled",
    "SlotStatePowerOffTimerQueued",
    "SlotStateEjectRequested"
};

VOID
PciDebugDumpHotPlugEvent (
    __in PPCI_HOTPLUG_SLOT Slot,
    __in PPCI_HOTPLUG_EVENT Event
    )

{

    NON_PAGED_CODE();

    PciDebugTrace(HOTPLUG_TRACE_LEVEL,
                  "\tSlot 0x%p - Number %x - %s\n",
                  Slot,
                  Slot->PhysicalSlotNumber,
                  HotplugEventTypeText[Event->Type]);

}

VOID
PciDebugDumpHotPlugAction (
    __in PPCI_HOTPLUG_ACTION Action
    )

{

    ULONG Index;

    NON_PAGED_CODE();

    if (Action->Slot != NULL) {
        PciDebugTrace(HOTPLUG_TRACE_LEVEL,
                      "\tHotplug Action 0x%p - Slot 0x%p - Number %x\n",
                      Action,
                      Action->Slot,
                      Action->Slot->PhysicalSlotNumber);

    } else {
        PciDebugTrace(HOTPLUG_TRACE_LEVEL,
                      "\tHotplug Action 0x%p - No slot\n",
                      Action);

    }

    PciDebugTrace(HOTPLUG_TRACE_LEVEL, "\tCommands:\n");
    for (Index = 0; Index < PCI_MAXIMUM_HOTPLUG_COMMAND_COUNT; Index++) {
        if (Action->Commands[Index].Type == SlotCommandNoop) {
            break;
        }

        PciDebugTrace(HOTPLUG_TRACE_LEVEL,
                      "\t\t%s%d%s - %s Context=%p %s\n",
                      (Index == Action->CurrentCommand) ? "<" : " ",
                      Index,
                      (Index == Action->CurrentCommand) ? ">" : " ",
                      HotplugCommandTypeText[Action->Commands[Index].Type],
                      (PVOID)Action->Commands[Index].Context,
                      Action->Commands[Index].InProgress ? "IN PROGRESS" : "");

    }

    if ((Index == PCI_MAXIMUM_HOTPLUG_COMMAND_COUNT) ||
        (Action->Commands[Index].Type == SlotCommandNoop)) {

        if (Action->CurrentCommand == Index) {
            PciDebugTrace(HOTPLUG_TRACE_LEVEL,
                          "\t\t<%d> All commands have been executed\n",
                          Index);

        }
    }
}

VOID
PciDebugDumpHotPlugSlotState (
    __in PPCI_HOTPLUG_SLOT Slot
    )

{

    NON_PAGED_CODE();

    PciDebugTrace(HOTPLUG_TRACE_LEVEL,
                  "\tSlot 0x%p - Number %x - %s\n",
                  Slot,
                  Slot->PhysicalSlotNumber,
                  HotplugSlotStateText[Slot->CurrentState]);

}

#endif
