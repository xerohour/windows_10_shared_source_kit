/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    state.c

Abstract:

    This module the state manipulation engine for PCI















--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "state.tmh"

// --------------------------------------------------------------------- Defines

#define ST_OK     STATUS_SUCCESS
#define ST_NOTOK  STATUS_INVALID_DEVICE_STATE
#define ST_ERROR  STATUS_FAIL_CHECK
#define ST_NTBUG  STATUS_INVALID_DEVICE_REQUEST
#define ST_OSBUG  STATUS_INVALID_DEVICE_REQUEST

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes
// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciInitializeState)
    #pragma alloc_text(PAGE, PciBeginStateTransition)
    #pragma alloc_text(PAGE, PciCommitStateTransition)
    #pragma alloc_text(PAGE, PciCancelStateTransition)
    #pragma alloc_text(PAGE, PciIsInTransitionToState)
#endif

// --------------------------------------------------------------------- Globals

//
// This array marks Allowed, Disallowed, and Illegal state transitions.
//
// The horizontal axis represents the current state.
// The vertical axis represents the state we are being asked to transition into.
//
//    There are four values in the table:
//       ST_OK             - State transition is possible
//       ST_NOTOK          - We legally cannot do the state transition
//       ST_NTBUG/ST_OSBUG - Illegal transition, but known OS bug.
//       ST_ERROR          - Consistancy problem. We should ASSERT!
//
// Count is PciMaxObjectState of
//
// PciNotStarted, PciStarted, PciDeleted, PciStopped, PciSurpriseRemoved, and
// PciSynchronizedOperation
//
// The final state is used to initiate operations that synchronously with
// respect to it and other state transitions. Commiting PciSynchronizedOperation
// is strictly illegal, and we are never strickly "in" that state.
//
const NTSTATUS PnpStateTransitionArray[PciMaxObjectState][PciMaxObjectState] = {
// at NotStarted, Started, Deleted, Stopped, SurpriseRemoved, SynchronizedOperation
   { ST_OK,    ST_OK,    ST_ERROR, ST_OK,    ST_ERROR, ST_ERROR }, // entering PciNotStarted
   { ST_OK,    ST_OK,    ST_ERROR, ST_OK,    ST_ERROR, ST_ERROR }, // entering PciStarted
   { ST_OK,    ST_OK,    ST_ERROR, ST_ERROR, ST_OK,    ST_ERROR }, // entering PciDeleted
   { ST_OSBUG, ST_OK,    ST_ERROR, ST_ERROR, ST_ERROR, ST_ERROR }, // entering PciStopped
   { ST_OK,    ST_OK,    ST_ERROR, ST_OK,    ST_ERROR, ST_ERROR }, // entering PciSurpriseRemoved
   { ST_OK,    ST_OK,    ST_NOTOK, ST_OK,    ST_NOTOK, ST_ERROR }  // entering PciSynchronizedOperation
};

//
// This array is used in debug to restrict which state transitions can be
// spuriously cancelled. We restrict this to Stops and Removes, which come
// through all the time due to the inability of PnP to differentiate which
// device in a stack failed a query.
//
// Cancelling NotStarted, Started, Removed, Stopped, SurpriseRemoved, SynchronizedOperation
const NTSTATUS PnpStateCancelArray[PciMaxObjectState] =
   { ST_NTBUG, ST_ERROR, ST_NOTOK, ST_NOTOK, ST_ERROR, ST_ERROR };

// ------------------------------------------------------------------- Functions

VOID
PciInitializeState(
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
/*++

Routine Description:

    This routine initializes the state variables in a device extension.
    
Arguments:

    DeviceExtension - The device extension to initialize.  This routine,
        like all the other state management functions, use a common
        extension since both bus and device objects have the state
        variables.
        
Return Value:

    VOID

--*/
{
    PAGED_CODE();
    
    DeviceExtension->DeviceState        = PciNotStarted;
    DeviceExtension->TentativeNextState = PciNotStarted;
}

NTSTATUS
PciBeginStateTransition(
    __inout PPCI_COMMON_EXTENSION DeviceExtension,
    __in PCI_OBJECT_STATE NewState
    )
/*++

Routine Description:

    This routine starts a state transition.  This transition is considered
    to be in-flight until a subsequent call to PciCommitStateTransition.
    
Arguments:

    DeviceExtension - The device to transition.
    
    NewState - The new state to start a transition into.
        
Return Value:

    NT Status code.  This indicates whether the requested state transition
    is valid.  See the top of this file for the transition table and
    potential return values.

--*/
{
    RECORDER_LOG blackBoxHandle;
    PCI_OBJECT_STATE currentState;
    NTSTATUS status;

    PAGED_CODE();

    PCI_ASSERT(NewState >= PciNotStarted && NewState < PciMaxObjectState);

    if (PCI_BUS_EXTENSION(DeviceExtension)) {
        blackBoxHandle = ((PPCI_BUS)DeviceExtension)->BlackBoxHandle;
    } else {
        blackBoxHandle = WppRecorderLogGetDefault();
    }

    TraceEvents(blackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PNP,
                "Request to begin transition of Extension %p to %!PNP_STATE!",
                DeviceExtension,
                NewState
                );

    //
    // Our "next" device state should be the same as our current device state.
    //
    PCI_ASSERT(DeviceExtension->TentativeNextState == DeviceExtension->DeviceState);
    currentState = DeviceExtension->DeviceState;

    PCI_ASSERT(currentState >= PciNotStarted && currentState < PciMaxObjectState);

    //
    // One of three returns will wind their way out of this code:
    // STATUS_SUCCESS              - State transition is possible
    // STATUS_INVALID_DEVICE_STATE - We legally cannot do the state transition
    // STATUS_FAIL_CHECK           - Consistancy problem. We should ASSERT!
    //
    status = PnpStateTransitionArray[NewState][currentState];

    if (NT_SUCCESS(status)) {
        
        //
        // Assert if someone tried to transition from A to A, and it's not a 
        // back-to-back start, as there are no other known cases where this is
        // legal.
        //
        PCI_ASSERT((NewState != DeviceExtension->DeviceState) ||
                   ((NewState == PciStarted && 
                     (DeviceExtension->DeviceState == PciStarted))));

        DeviceExtension->TentativeNextState = (UCHAR)NewState;
    }
    
    //
    // State bug in PnP or driver. Investigation required.
    //
    if (status == ST_ERROR) {

        TraceEvents(blackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_PNP,
                    "ERROR trying to enter %!PNP_STATE! from %!PNP_STATE!",
                    NewState,
                    currentState
                    );

        KdBreakPoint();

#pragma prefast(suppress:__WARNING_REDUNDANTTEST, "redundant but informative")
    } else if ((status == ST_NTBUG) || (status == ST_OSBUG)) {

        TraceEvents(blackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_PNP,
                    "ERROR: Illegal request to try to enter %!PNP_STATE! from state %!PNP_STATE!, rejecting",
                    NewState,
                    currentState
                    );
    }

    TraceEvents(blackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PNP,
                "-> %!STATUS!",
                status
                );

    return status;
}

VOID
PciCommitStateTransition(
    __inout PPCI_COMMON_EXTENSION DeviceExtension,
    __in PCI_OBJECT_STATE NewState
    )
/*++

Routine Description:

    This routine commits a state transition that was previously started
    with PciBeginStateTransition.
    
Arguments:

    DeviceExtension - The device to transition.
    
    NewState - The new state to commit a transition into.
        
Return Value:

    VOID

--*/
{
    RECORDER_LOG blackBoxHandle;

    PAGED_CODE();

    PCI_ASSERT(NewState >= PciNotStarted && NewState < PciMaxObjectState);

    if (PCI_BUS_EXTENSION(DeviceExtension)) {
        blackBoxHandle = ((PPCI_BUS)DeviceExtension)->BlackBoxHandle;
    } else {
        blackBoxHandle = WppRecorderLogGetDefault();
    }

    TraceEvents(blackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PNP,
                "Commit transition of Extension %p to %!PNP_STATE!",
                DeviceExtension,
                NewState
                );

    //
    // This state is illegal.
    //
    PCI_ASSERT(NewState != PciSynchronizedOperation);

    //
    // verify commit properly pairs with previous PciBeginStateTransition.
    //
    PCI_ASSERT(DeviceExtension->TentativeNextState == NewState);

    DeviceExtension->DeviceState = (UCHAR)NewState;
}

NTSTATUS
PciCancelStateTransition(
    __inout PPCI_COMMON_EXTENSION DeviceExtension,
    __in PCI_OBJECT_STATE StateNotEntered
    )
/*++

Routine Description:

    This routine cancels a state transition.  that was previously started
    with PciBeginStateTransition.
    
Arguments:

    DeviceExtension - The device to transition.
    
    StateNotEntered - The state to cancel the transition into.
        
Return Value:

    NT Status code.  This indicates whether cancelling the requested 
    state transition is valid.  See the top of this file for the 
    transition table and potential return values.

--*/
{
    RECORDER_LOG blackBoxHandle;

    PAGED_CODE();

    PCI_ASSERT(StateNotEntered >= PciNotStarted && StateNotEntered < PciMaxObjectState);

    if (PCI_BUS_EXTENSION(DeviceExtension)) {
        blackBoxHandle = ((PPCI_BUS)DeviceExtension)->BlackBoxHandle;
    } else {
        blackBoxHandle = WppRecorderLogGetDefault();
    }

    TraceEvents(blackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PNP,
                "Request to cancel transition of Extension %p to %!PNP_STATE!",
                DeviceExtension,
                StateNotEntered
                );

    //
    // Spurious Cancels are allowed in specific states. This is allowed
    // because PnP can't help but send them.
    //
    if (DeviceExtension->TentativeNextState == DeviceExtension->DeviceState) {

        PCI_ASSERT(PnpStateCancelArray[StateNotEntered] != STATUS_FAIL_CHECK);
        
        TraceEvents(blackBoxHandle,
                    TRACE_LEVEL_INFORMATION,
                    DBG_PNP,
                    "-> ST_NOTOK"
                    );
        
        return ST_NOTOK;
    }

    //
    // Verify cancel properly pairs with previous PciBeginStateTransition.
    //
    PCI_ASSERT(DeviceExtension->TentativeNextState == StateNotEntered);

    DeviceExtension->TentativeNextState = DeviceExtension->DeviceState;

    TraceEvents(blackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PNP,
                "-> ST_OK"
                );

    return ST_OK;
}

BOOLEAN
PciIsInTransitionToState(
    __in PPCI_COMMON_EXTENSION DeviceExtension,
    __in PCI_OBJECT_STATE NextState
    )
/*++

Routine Description:

    This routine indicates whether the given device is in the
    middle of a transition to the given state.
    
Arguments:

    DeviceExtension - The device to check.
    
    NextState - The state to check if the device is in a transition into.
        
Return Value:

    TRUE if the device is in a transition to the given state.
    FALSE otherwise.

--*/
{
    PAGED_CODE();
    
    PCI_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    PCI_ASSERT(NextState < PciMaxObjectState);

    //
    // If we're not in the middle of any state transition, we can't
    // possibly be in the middle of a transition to the requested state.
    //
    if (DeviceExtension->TentativeNextState == DeviceExtension->DeviceState) {

        return FALSE;
    }

    return (BOOLEAN)(DeviceExtension->TentativeNextState == NextState);
}
