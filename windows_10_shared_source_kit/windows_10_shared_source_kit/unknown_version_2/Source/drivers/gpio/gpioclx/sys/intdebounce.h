/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    intdebounce.h

Abstract:

    This header contains all the definitions and prototypes required for
    interrupt debouncing module.


Environment:

    Kernel mode

--*/

#pragma once

//
// Define the debounce state history size. On CHK builds builds maintain
// a larger history to aid diagnosis.
//

#if defined(DBG)

#define GPIO_DEBOUNCE_HISTORY_SIZE (32)

#else

#define GPIO_DEBOUNCE_HISTORY_SIZE (8)

#endif

//
// The number of reconfiguration reattempts the debounce engine will perform.
//
// N.B. The GPIO class extension will attempt
//      RECONFIGURE_FAILURE_COUNT_THRESHOLD times to reconfigure on each
//      request. Thus the total number of attempts is multiplied.
//

#define GPIO_DEBOUNCE_RECONFIGURE_FAILURE_THRESHOLD (5)

//
// Define states within the interrupt debounce state machine.
//


typedef enum _GPIO_DEBOUNCE_STATE {
    DebounceStateInvalid = 0x0,

    //
    // Starting state
    //
    // The state machine always returns back to this, except in the
    // GPIO_DEBOUNCE_RECONFIGURE_FAILURE_THRESHOLD case.
    //

    DebounceStateOnInitialInterrupt,
    DebounceStateMaskInterrupt,
    DebounceStateScheduleDebounceTimer,
    DebounceStateOnDebounceTimerFire,
    DebounceStateScheduleNoiseTimer,
    DebounceStateOnNoiseTimerFire,
    DebounceStateCancelNoiseTimer,
    DebounceStateInvokeTargetIsr,
    DebounceStateReconfigureInterrupt,
    DebounceStateUnmaskInterrupt,
    DebounceStateWaitForUnmaskCompletion,
    DebounceStateUpdateEpochOnInitialInterrupt,
    DebounceStateUpdateEpochOnNoiseTimerInterrupt,
    DebounceStateQueryInterruptEpoch,
    DebounceStateCheckEpochMatch,
    DebounceStateLogSpuriousInterrupt,
    DebounceStateUpdateDebounceModel,
    DebounceStateCheckRestorePolarity,
    DebounceStateMaskRestorePolarity,
    DebounceStateReconfigureRestorePolarity,
    DebounceStateUnmaskRestorePolarity,

    //
    // N.B. The state machine can get stuck at this (see
    //      GPIO_DEBOUNCE_RECONFIGURE_FAILURE_THRESHOLD).
    //

    DebounceStateReconfigureFailureReattempt,
    DebounceStateRestoreReconfigureFailureReattempt,
    DebounceStateMaximum
} GPIO_DEBOUNCE_STATE, *PGPIO_DEBOUNCE_STATE;

//
// Define input events for interrupt debouncing.
//

typedef enum _GPIO_DEBOUNCE_EVENT_TYPE {

    //
    // Secondary event types begin here.
    //
    //

    DebounceEventNone = 0x0,
    DebounceEventRequestCompletion,
    DebounceEventRequestPending,

    //
    // Primary event types begin here
    //

    DebounceEventOnInterrupt,
    DebounceEventOnDebounceTimerFire,
    DebounceEventOnNoiseTimerFire,
    DebounceEventMaximum
} GPIO_DEBOUNCE_EVENT_TYPE, *PGPIO_DEBOUNCE_EVENT_TYPE;

//
// Define output actions for interrupt debouncing.
//

typedef enum _GPIO_DEBOUNCE_ACTION_TYPE {
    DebounceActionNone = 0x0,
    DebounceActionScheduleDebounceTimer,
    DebounceActionScheduleNoiseTimer,
    DebounceActionInvokeTargetIsr,
    DebounceActionCancelNoiseTimer,
    DebounceActionMaskInterrupt,
    DebounceActionUnmaskInterrupt,
    DebounceActionReconfigureInterrupt,
    DebounceActionLogSpuriousInterrupt,
    DebounceActionQueryNoiseFilterEpoch,
    DebounceActionClearStatusRegister,
    DebounceActionQueryEmulatedActiveBothInterrupt,
    DebounceActionMaximum
} GPIO_DEBOUNCE_ACTION_TYPE, *PGPIO_DEBOUNCE_ACTION_TYPE;


//
// Defines the various types of debouncing logic that can be applied.
//
// DebounceModelNone - This type means the pin doesn't need any debouncing.
//
// DebounceModelLegacy - This type means each interrupt will result in
//      the target ISR invocation. This is applied in cases where enhanced
//      algorithm cannot be applied.
//
// DebounceModelPossiblyEnhanced - This type means that the debounce model
//      could possibly be enhanced. The value can only be confirmed after
//      a reconfigure attempt. If the reconfigure succeeds, then the debounce
//      model will be converted to enhanced. Otherwise, it will be set to
//      legacy.
//
// DebounceModelEnhanced - This type means each interrupt will be examined
//      after the debounce interval to determine if it is spurious or not.
//      The target ISR will only be invoked if the interrupt is deemed
//      genuine (as determined by the state of the physical line post
//      debounce interval expiry).
//

typedef enum _GPIO_DEBOUNCE_MODEL {
    DebounceModelNone = 0x0,
    DebounceModelLegacy,
    DebounceModelPossiblyEnhanced,
    DebounceModelEnhanced,
    DebounceModelMaximum
} GPIO_DEBOUNCE_MODEL, *PGPIO_DEBOUNCE_MODEL;

//
// Define various flags used by the debouncing logic.
//
// MemoryMapped - This flag is set if the controller is memory-mapped.
//
// DebounceModel - This flag takes a GPIO_DEBOUNCE_MODEL value.
//
// NoRevertToOriginalPolarity - This flag is set if the configuration
//      need not be reverted to original polarity after a valid interrupt
//      is detected. Reconfiguration will be done if spurious/invalid
//      interrupts are detected.
//
// ReconfigureFailed - This flag indicates that the reconfiguration request
//      has failed.
//

typedef union _GPIO_DEBOUNCE_FLAGS {
    struct {
        ULONG MemoryMapped: 1;
        ULONG DebounceModel: 3;
        ULONG NoReconfigurePostValidInterrupt: 1;
        ULONG ReconfigureFailed: 1;
        ULONG Reserved: 26;
    };

    ULONG AsULONG;

} GPIO_DEBOUNCE_FLAGS, *PGPIO_DEBOUNCE_FLAGS;

C_ASSERT(DebounceModelMaximum < 8); // only 3-bits for debounce model

//
// Define the per-pin interrupt debounce context.
//

typedef struct _GPIO_DEBOUNCE_PIN_CONTEXT {

    //
    // Tracks the current state within the debounce state machine for the
    // pin. The state is protected using the debounce lock.
    //

    GPIO_DEBOUNCE_STATE State;
    GPIO_DEBOUNCE_FLAGS Flags;

    //
    // Tracks the epoch number associated with the interrupt. This value is
    // incremented each time an interrupt request is presented to the
    // debounce engine.
    //

    volatile ULONG InterruptEpoch;

    ULONG NoiseFilterEpoch;

    //
    // Tracks the debounce state history state and corresponding primary events
    // for the pin. These buffers are protected using the same lock as the
    // state.
    //

    UCHAR StateIndex;
    GPIO_DEBOUNCE_STATE PreviousState[GPIO_DEBOUNCE_HISTORY_SIZE];
    GPIO_DEBOUNCE_EVENT_TYPE PreviousEvent[GPIO_DEBOUNCE_HISTORY_SIZE];

    //
    // The consecutive number of times a reconfiguration request failed for
    // this pin.
    //

    UCHAR ReconfigureFailureCount;

    //
    // If non-NULL, "CompletionEvent" is signalled some time after the
    // debounce state machine has completed.
    //

    PKEVENT CompletionEvent;

} GPIO_DEBOUNCE_PIN_CONTEXT, *PGPIO_DEBOUNCE_PIN_CONTEXT;

C_ASSERT((GPIO_DEBOUNCE_HISTORY_SIZE <= MAX_UCHAR) &&
         (GPIO_DEBOUNCE_RECONFIGURE_FAILURE_THRESHOLD <= MAX_UCHAR));

typedef struct _GPIO_DEBOUNCE_OUTPUT_ACTION {
    GPIO_DEBOUNCE_ACTION_TYPE ActionType;
    BOOLEAN StateChangeValid;
    GPIO_DEBOUNCE_STATE ExpectedState;
    ULONG AssociatedEpoch;
} GPIO_DEBOUNCE_OUTPUT_ACTION, *PGPIO_DEBOUNCE_OUTPUT_ACTION;

typedef struct _GPIO_DEBOUNCE_INPUT_EVENT {
    GPIO_DEBOUNCE_EVENT_TYPE EventType;
    NTSTATUS EventStatus;
    GPIO_DEBOUNCE_OUTPUT_ACTION AssociatedOutputAction;
} GPIO_DEBOUNCE_INPUT_EVENT, *PGPIO_DEBOUNCE_INPUT_EVENT;

//
// ------------------------------------------------------------------- Exports
//

_IRQL_requires_(PASSIVE_LEVEL)
VOID
GpiopDebounceInitializeContext (
    __in BOOLEAN MemoryMappedController,
    __in BOOLEAN EmulatedDebounce,
    __in BOOLEAN EmulatedActiveBoth,
    __in WDF_TRI_STATE LevelReconfigurationSupported,
    __in GPIO_DEBOUNCE_MODEL DebounceModelOverride,
    __out PGPIO_DEBOUNCE_PIN_CONTEXT DebounceContext
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
GpiopDebounceDestroyContext (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebounceContext
    );

NTSTATUS
GpiopDebounceOnInterrupt (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebounceContext,
    __in PVOID ControllerContext,
    __out PBOOLEAN DebounceStateMachineCompleted
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
GpiopDebounceOnDebounceTimer (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebounceContext,
    __in PVOID ControllerContext,
    __out PBOOLEAN DebounceStateMachineCompleted
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
GpiopDebounceOnNoiseTimer (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebounceContext,
    __in PVOID ControllerContext,
    __in PGPIO_DEBOUNCE_OUTPUT_ACTION AssociatedOutputAction,
    __out PBOOLEAN DebounceStateMachineCompleted
    );

ULONG
GpiopDebounceQueryInterruptEpoch (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebounceContext
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
GpiopDebounceWaitForStateMachineCompletionOfMaskedPin (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebounceContext,
    __in PVOID ControllerContext,
    __in BOOLEAN WaitForTimerDpcCompletion
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
GpiopDebounceSignalStateMachineCompletion (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebounceContext,
    __in PVOID ControllerContext
    );

//
// -------------------------------------------------------------------- Globals
//

extern GPIO_DEBOUNCE_MODEL GpioGlobalDebounceModelOverride;


