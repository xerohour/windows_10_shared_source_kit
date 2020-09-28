
/*++
Copyright (c) Microsoft Corporation

Module Name:

ISM_AutoGen.c

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/


#include <pch.h>

ISM_EVENT
HUBISM_CheckingIfSSIsEnabled(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_CheckingIfSSIsEnabled is called when the
    state machine enters the CheckingIfSSIsEnabled State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        syncEvent = HUBIDLE_CheckIfSelectiveSuspendIsEnabled(IdleContext, IdleIrp);

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_CheckingIfSSIsEnabled


ISM_EVENT
HUBISM_CompletingIdleIrpWithStatusCancelled(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_CompletingIdleIrpWithStatusCancelled is called when the
    state machine enters the CompletingIdleIrpWithStatusCancelled State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        UNREFERENCED_PARAMETER(IdleIrp);

        IdleContext->PostLockAction = IsmPostLockActionCompleteIrpWithStatusCancelled;

        syncEvent = IsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_CompletingIdleIrpWithStatusCancelled


ISM_EVENT
HUBISM_CompletingIdleIrpWithStatusCancelledInSSDisabled(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_CompletingIdleIrpWithStatusCancelledInSSDisabled is called when the
    state machine enters the CompletingIdleIrpWithStatusCancelledInSSDisabled State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        UNREFERENCED_PARAMETER(IdleIrp);

        IdleContext->PostLockAction = IsmPostLockActionCompleteIrpWithStatusCancelled;

        syncEvent = IsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_CompletingIdleIrpWithStatusCancelledInSSDisabled


ISM_EVENT
HUBISM_CompletingIdleIrpWithStatusCancelledReturnPending(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_CompletingIdleIrpWithStatusCancelledReturnPending is called when the
    state machine enters the CompletingIdleIrpWithStatusCancelledReturnPending State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        UNREFERENCED_PARAMETER(IdleIrp);

        IdleContext->PostLockAction = IsmPostLockActionCompleteIrpWithStatusCancelledAndReturnPending;

        syncEvent = IsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_CompletingIdleIrpWithStatusCancelledReturnPending


ISM_EVENT
HUBISM_CompletingIdleIrpWithStatusInvalidDeviceState(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_CompletingIdleIrpWithStatusInvalidDeviceState is called when the
    state machine enters the CompletingIdleIrpWithStatusInvalidDeviceState State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        UNREFERENCED_PARAMETER(IdleIrp);

        IdleContext->PostLockAction = IsmPostLockActionCompleteIrpWithStatusInvalidDeviceState;

        syncEvent = IsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_CompletingIdleIrpWithStatusInvalidDeviceState


ISM_EVENT
HUBISM_CompletingIdleIrpWithStatusSuccess(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_CompletingIdleIrpWithStatusSuccess is called when the
    state machine enters the CompletingIdleIrpWithStatusSuccess State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        UNREFERENCED_PARAMETER(IdleIrp);

        IdleContext->PostLockAction = IsmPostLockActionCompleteIrpWithStatusSuccess;

        syncEvent = IsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_CompletingIdleIrpWithStatusSuccess


ISM_EVENT
HUBISM_CompletingRequestWithStatusDeviceGone(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_CompletingRequestWithStatusDeviceGone is called when the
    state machine enters the CompletingRequestWithStatusDeviceGone State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        UNREFERENCED_PARAMETER(IdleIrp);

        IdleContext->PostLockAction = IsmPostLockActionCompleteIrpWithStatusNoSuchDevice;

        syncEvent = IsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_CompletingRequestWithStatusDeviceGone


ISM_EVENT
HUBISM_InvokingClientCallback(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_InvokingClientCallback is called when the
    state machine enters the InvokingClientCallback State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        UNREFERENCED_PARAMETER(IdleIrp);

        IdleContext->PostLockAction = IsmPostLockActionInvokeClientCallback;

        syncEvent = IsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_InvokingClientCallback


ISM_EVENT
HUBISM_MarkingRequestCancellable(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_MarkingRequestCancellable is called when the
    state machine enters the MarkingRequestCancellable State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        syncEvent = HUBIDLE_MarkIrpAsCancellable(IdleContext, IdleIrp);

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_MarkingRequestCancellable


ISM_EVENT
HUBISM_MarkingRequestCancellableInSSDisabled(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_MarkingRequestCancellableInSSDisabled is called when the
    state machine enters the MarkingRequestCancellableInSSDisabled State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        syncEvent = HUBIDLE_MarkIrpAsCancellable(IdleContext, IdleIrp);

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_MarkingRequestCancellableInSSDisabled


ISM_EVENT
HUBISM_QueuingWorkerItem(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_QueuingWorkerItem is called when the
    state machine enters the QueuingWorkerItem State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        UNREFERENCED_PARAMETER(IdleIrp);

        IdleContext->PostLockAction = IsmPostLockActionQueueWorkItem;

        syncEvent = IsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_QueuingWorkerItem


ISM_EVENT
HUBISM_StoringRequestInSSDisabled(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_StoringRequestInSSDisabled is called when the
    state machine enters the StoringRequestInSSDisabled State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        HUBIDLE_StoreRequest(IdleContext, IdleIrp);

        syncEvent = IsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_StoringRequestInSSDisabled


ISM_EVENT
HUBISM_StoringRequestInSSEnabled(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_StoringRequestInSSEnabled is called when the
    state machine enters the StoringRequestInSSEnabled State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        HUBIDLE_StoreRequest(IdleContext, IdleIrp);

        syncEvent = IsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_StoringRequestInSSEnabled


ISM_EVENT
HUBISM_UnMarkingRequestAsCancelable(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_UnMarkingRequestAsCancelable is called when the
    state machine enters the UnMarkingRequestAsCancelable State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        syncEvent = HUBIDLE_UnMarkIrpAsCancellable(IdleContext, IdleIrp);

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_UnMarkingRequestAsCancelable


ISM_EVENT
HUBISM_UnMarkingRequestCancellable(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_UnMarkingRequestCancellable is called when the
    state machine enters the UnMarkingRequestCancellable State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        syncEvent = HUBIDLE_UnMarkIrpAsCancellable(IdleContext, IdleIrp);

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_UnMarkingRequestCancellable


ISM_EVENT
HUBISM_UnMarkingRequestCancellableOnD0(
    PISM_CONTEXT              IdleContext,
    PIRP                      IdleIrp
    )
/*++
Routine Description:

    HUBISM_UnMarkingRequestCancellableOnD0 is called when the
    state machine enters the UnMarkingRequestCancellableOnD0 State

Arguments:

    IdleContext - Pdo Context
    IdleIrp - IRP for the Idle notification


Return Value:

    ISM_EVENT

--*/
{

    ISM_EVENT                       syncEvent;
    
    TRY {

        syncEvent = HUBIDLE_UnMarkIrpAsCancellable(IdleContext, IdleIrp);

    } FINALLY {

    }

    return syncEvent;

} // HUBISM_UnMarkingRequestCancellableOnD0





