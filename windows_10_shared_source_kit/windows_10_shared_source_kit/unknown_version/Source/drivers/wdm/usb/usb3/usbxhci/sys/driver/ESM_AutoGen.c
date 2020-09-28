
/*++
Copyright (c) Microsoft Corporation

Module Name:

ESM_AutoGen.c

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/


#include <pch.h>

ESM_EVENT
ESM_AcknowledgingControllerResetComplete(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_AcknowledgingControllerResetComplete is called when the
    state machine enters the AcknowledgingControllerResetComplete State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_AcknowledgeControllerResetComplete(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_AcknowledgingControllerResetComplete


ESM_EVENT
ESM_AcknowledgingControllerResetCompleteInDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_AcknowledgingControllerResetCompleteInDisabled is called when the
    state machine enters the AcknowledgingControllerResetCompleteInDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_AcknowledgeControllerResetComplete(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_AcknowledgingControllerResetCompleteInDisabled


ESM_EVENT
ESM_CheckingIfControllerHasKnownIssuesInDeliveringFSE(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_CheckingIfControllerHasKnownIssuesInDeliveringFSE is called when the
    state machine enters the CheckingIfControllerHasKnownIssuesInDeliveringFSE State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        syncEvent = Endpoint_SM_IsUseSingleInterrupterFlagSet(EndpointData);

    } FINALLY {

    }

    return syncEvent;

} // ESM_CheckingIfControllerHasKnownIssuesInDeliveringFSE


ESM_EVENT
ESM_CheckingIfEndpointCanBeResetAfterHalt(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_CheckingIfEndpointCanBeResetAfterHalt is called when the
    state machine enters the CheckingIfEndpointCanBeResetAfterHalt State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        syncEvent = Endpoint_SM_ShouldResetEndpointAfterHalt(EndpointData);

    } FINALLY {

    }

    return syncEvent;

} // ESM_CheckingIfEndpointCanBeResetAfterHalt


ESM_EVENT
ESM_CheckingIfEndpointShouldBeAutoReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_CheckingIfEndpointShouldBeAutoReset is called when the
    state machine enters the CheckingIfEndpointShouldBeAutoReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        syncEvent = Endpoint_SM_IsEndpointTypeControl(EndpointData);

    } FINALLY {

    }

    return syncEvent;

} // ESM_CheckingIfEndpointShouldBeAutoReset


ESM_EVENT
ESM_CheckingIfEndpointShouldBeReconfiguredAfterStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_CheckingIfEndpointShouldBeReconfiguredAfterStop is called when the
    state machine enters the CheckingIfEndpointShouldBeReconfiguredAfterStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        syncEvent = Endpoint_SM_ShouldReconfigureEndpointAfterStop(EndpointData);

    } FINALLY {

    }

    return syncEvent;

} // ESM_CheckingIfEndpointShouldBeReconfiguredAfterStop


ESM_EVENT
ESM_CheckingIfEndpointShouldBeReconfiguredOnResetWhenNotHalted(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_CheckingIfEndpointShouldBeReconfiguredOnResetWhenNotHalted is called when the
    state machine enters the CheckingIfEndpointShouldBeReconfiguredOnResetWhenNotHalted State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        syncEvent = Endpoint_SM_ShouldReconfigureEndpointOnResetWhenNotHalted(EndpointData);

    } FINALLY {

    }

    return syncEvent;

} // ESM_CheckingIfEndpointShouldBeReconfiguredOnResetWhenNotHalted


ESM_EVENT
ESM_CheckingIfEndpointShouldBeStopped(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_CheckingIfEndpointShouldBeStopped is called when the
    state machine enters the CheckingIfEndpointShouldBeStopped State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        syncEvent = Endpoint_SM_WasDoorbellRungSinceMappingStart(EndpointData);

    } FINALLY {

    }

    return syncEvent;

} // ESM_CheckingIfEndpointShouldBeStopped


ESM_EVENT
ESM_CheckingIfIgnoreContextErrorOnEPStopFlagIsSet(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_CheckingIfIgnoreContextErrorOnEPStopFlagIsSet is called when the
    state machine enters the CheckingIfIgnoreContextErrorOnEPStopFlagIsSet State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        syncEvent = Endpoint_SM_IsIgnoreContextErrorOnEPStopFlagSet(EndpointData);

    } FINALLY {

    }

    return syncEvent;

} // ESM_CheckingIfIgnoreContextErrorOnEPStopFlagIsSet


ESM_EVENT
ESM_CheckingIfStopEndpointCommandShouldBeDelayed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_CheckingIfStopEndpointCommandShouldBeDelayed is called when the
    state machine enters the CheckingIfStopEndpointCommandShouldBeDelayed State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_ResetStopEndpointReissueCount(EndpointData);

        syncEvent = Endpoint_SM_IsDelayInitialEndpointStopCommandFlagSet(EndpointData);

    } FINALLY {

    }

    return syncEvent;

} // ESM_CheckingIfStopEndpointCommandShouldBeDelayed


ESM_EVENT
ESM_CheckingIfStopEndpontReissueCountReached(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_CheckingIfStopEndpontReissueCountReached is called when the
    state machine enters the CheckingIfStopEndpontReissueCountReached State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        syncEvent = Endpoint_SM_IsStopEndpointReissueCountLimitReached(EndpointData);

    } FINALLY {

    }

    return syncEvent;

} // ESM_CheckingIfStopEndpontReissueCountReached


ESM_EVENT
ESM_CompletingClientResetRequest(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_CompletingClientResetRequest is called when the
    state machine enters the CompletingClientResetRequest State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_CompleteResetRequestFromClient(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_CompletingClientResetRequest


ESM_EVENT
ESM_CompletingClientResetRequestAfterEndpointReconfigure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_CompletingClientResetRequestAfterEndpointReconfigure is called when the
    state machine enters the CompletingClientResetRequestAfterEndpointReconfigure State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_CompleteResetRequestFromClient(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_CompletingClientResetRequestAfterEndpointReconfigure


ESM_EVENT
ESM_CompletingClientResetRequestAfterEndpointReconfigureFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_CompletingClientResetRequestAfterEndpointReconfigureFailure is called when the
    state machine enters the CompletingClientResetRequestAfterEndpointReconfigureFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_CompleteResetRequestFromClient(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_CompletingClientResetRequestAfterEndpointReconfigureFailure


ESM_EVENT
ESM_CompletingClientResetRequestAfterResetFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_CompletingClientResetRequestAfterResetFailure is called when the
    state machine enters the CompletingClientResetRequestAfterResetFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_CompleteResetRequestFromClient(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_CompletingClientResetRequestAfterResetFailure


ESM_EVENT
ESM_CompletingClientResetRequestDueToOutstandingTransfers(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_CompletingClientResetRequestDueToOutstandingTransfers is called when the
    state machine enters the CompletingClientResetRequestDueToOutstandingTransfers State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_CompleteResetRequestFromClient(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_CompletingClientResetRequestDueToOutstandingTransfers


ESM_EVENT
ESM_CompletingResetRequestWhenEndpointNotHalted(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_CompletingResetRequestWhenEndpointNotHalted is called when the
    state machine enters the CompletingResetRequestWhenEndpointNotHalted State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_CompleteResetRequestFromClient(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_CompletingResetRequestWhenEndpointNotHalted


ESM_EVENT
ESM_ControllerResetInEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_ControllerResetInEnabled is called when the
    state machine enters the ControllerResetInEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return EsmEventNull;

} // ESM_ControllerResetInEnabled


ESM_EVENT
ESM_Disabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_Disabled is called when the
    state machine enters the Disabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return EsmEventNull;

} // ESM_Disabled


ESM_EVENT
ESM_DroppingFSEAfterStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_DroppingFSEAfterStop is called when the
    state machine enters the DroppingFSEAfterStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_DropFSE(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_DroppingFSEAfterStop


ESM_EVENT
ESM_EnabledAndMapping(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_EnabledAndMapping is called when the
    state machine enters the EnabledAndMapping State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return EsmEventNull;

} // ESM_EnabledAndMapping


ESM_EVENT
ESM_NotifyingTransferRingsEndpointHalted(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_NotifyingTransferRingsEndpointHalted is called when the
    state machine enters the NotifyingTransferRingsEndpointHalted State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_NotifyTransferRingsEndpointHalted(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_NotifyingTransferRingsEndpointHalted


ESM_EVENT
ESM_NotifyingTransferRingsEndpointResetFromClient(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_NotifyingTransferRingsEndpointResetFromClient is called when the
    state machine enters the NotifyingTransferRingsEndpointResetFromClient State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_NotifyTransferRingsEndpointResetFromClient(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_NotifyingTransferRingsEndpointResetFromClient


ESM_EVENT
ESM_NotifyingTransferRingsFSEReceived(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_NotifyingTransferRingsFSEReceived is called when the
    state machine enters the NotifyingTransferRingsFSEReceived State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_NotifyTransferRingsFSEReceived(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_NotifyingTransferRingsFSEReceived


ESM_EVENT
ESM_NotifyingTransferRingsOkToReclaimTransfersOnCancel(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_NotifyingTransferRingsOkToReclaimTransfersOnCancel is called when the
    state machine enters the NotifyingTransferRingsOkToReclaimTransfersOnCancel State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_NotifyTransferRingsOkToReclaimTransfersOnCancel(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_NotifyingTransferRingsOkToReclaimTransfersOnCancel


ESM_EVENT
ESM_NotifyingUCXAboutTransferCancellation(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_NotifyingUCXAboutTransferCancellation is called when the
    state machine enters the NotifyingUCXAboutTransferCancellation State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_NotifyUCXCancelTransfers(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_NotifyingUCXAboutTransferCancellation


ESM_EVENT
ESM_ProcessingExpectedEventTRBsAfterEndpointHalt(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_ProcessingExpectedEventTRBsAfterEndpointHalt is called when the
    state machine enters the ProcessingExpectedEventTRBsAfterEndpointHalt State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_NotifyTransferRingsProcessExpectedEventTRBs(EndpointData);

        Endpoint_SM_Start2SecondTimer(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_ProcessingExpectedEventTRBsAfterEndpointHalt


ESM_EVENT
ESM_ProcessingExpectedEventTRBsAfterEndpointStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_ProcessingExpectedEventTRBsAfterEndpointStop is called when the
    state machine enters the ProcessingExpectedEventTRBsAfterEndpointStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_NotifyTransferRingsProcessExpectedEventTRBs(EndpointData);

        Endpoint_SM_Start2SecondTimer(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_ProcessingExpectedEventTRBsAfterEndpointStop


ESM_EVENT
ESM_PurgingTransferRingQueuesIfNeeded(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_PurgingTransferRingQueuesIfNeeded is called when the
    state machine enters the PurgingTransferRingQueuesIfNeeded State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_PurgeTransferRingQueuesIfNeeded(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_PurgingTransferRingQueuesIfNeeded


ESM_EVENT
ESM_ReclaimingCanceledTransfersAfterControllerSurpriseRemoval(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_ReclaimingCanceledTransfersAfterControllerSurpriseRemoval is called when the
    state machine enters the ReclaimingCanceledTransfersAfterControllerSurpriseRemoval State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_NotifyTransferRingsOkToReclaimTransfersOnCancel(EndpointData);

        Endpoint_SM_OkToCompleteEndpointResetRequest(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_ReclaimingCanceledTransfersAfterControllerSurpriseRemoval


ESM_EVENT
ESM_ReclaimingTransfersAfterControlEndpointAutoReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_ReclaimingTransfersAfterControlEndpointAutoReset is called when the
    state machine enters the ReclaimingTransfersAfterControlEndpointAutoReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_NotifyTransferRingsReclaimTransfers(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_ReclaimingTransfersAfterControlEndpointAutoReset


ESM_EVENT
ESM_ReclaimingTransfersAfterControllerReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_ReclaimingTransfersAfterControllerReset is called when the
    state machine enters the ReclaimingTransfersAfterControllerReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_NotifyTransferRingsReclaimTransfers(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_ReclaimingTransfersAfterControllerReset


ESM_EVENT
ESM_ReclaimingTransfersAfterEndpointStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_ReclaimingTransfersAfterEndpointStop is called when the
    state machine enters the ReclaimingTransfersAfterEndpointStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_NotifyTransferRingsReclaimTransfers(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_ReclaimingTransfersAfterEndpointStop


ESM_EVENT
ESM_ReconfiguringEndpointAfterStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_ReconfiguringEndpointAfterStop is called when the
    state machine enters the ReconfiguringEndpointAfterStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_ReconfigureEndpointAfterStop(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_ReconfiguringEndpointAfterStop


ESM_EVENT
ESM_ReconfiguringEndpointOnReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_ReconfiguringEndpointOnReset is called when the
    state machine enters the ReconfiguringEndpointOnReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_ReconfigureEndpoint(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_ReconfiguringEndpointOnReset


ESM_EVENT
ESM_RequeuingPendingTransfersAfterHaltClear(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_RequeuingPendingTransfersAfterHaltClear is called when the
    state machine enters the RequeuingPendingTransfersAfterHaltClear State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_NotifyTransferRingsReclaimTransfers(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_RequeuingPendingTransfersAfterHaltClear


ESM_EVENT
ESM_ResettingControlEndpoint(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_ResettingControlEndpoint is called when the
    state machine enters the ResettingControlEndpoint State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_ResetControlEndpoint(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_ResettingControlEndpoint


ESM_EVENT
ESM_ResettingControllerDueToRepeatedStopEndpointFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_ResettingControllerDueToRepeatedStopEndpointFailure is called when the
    state machine enters the ResettingControllerDueToRepeatedStopEndpointFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_RequestControllerResetDueToRepeatedStopEndpointFailure(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_ResettingControllerDueToRepeatedStopEndpointFailure


ESM_EVENT
ESM_ResettingEndpoint(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_ResettingEndpoint is called when the
    state machine enters the ResettingEndpoint State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_ResetEndpoint(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_ResettingEndpoint


ESM_EVENT
ESM_RestartingTransferRingQueuesIfNeeded(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_RestartingTransferRingQueuesIfNeeded is called when the
    state machine enters the RestartingTransferRingQueuesIfNeeded State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_RestartTransferRingQueuesIfNeeded(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_RestartingTransferRingQueuesIfNeeded


ESM_EVENT
ESM_SimulatingEndpointHaltReceivedOnTimeout(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_SimulatingEndpointHaltReceivedOnTimeout is called when the
    state machine enters the SimulatingEndpointHaltReceivedOnTimeout State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        syncEvent = Endpoint_SM_AssertAndSimulateEndpointHaltReceived(EndpointData);

    } FINALLY {

    }

    return syncEvent;

} // ESM_SimulatingEndpointHaltReceivedOnTimeout


ESM_EVENT
ESM_SimulatingExpectedEventTRBsProcessedAfterHaltOnTimeout(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_SimulatingExpectedEventTRBsProcessedAfterHaltOnTimeout is called when the
    state machine enters the SimulatingExpectedEventTRBsProcessedAfterHaltOnTimeout State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        syncEvent = Endpoint_SM_AssertAndSimulateExpectedEventTRBsProcessedAfterHalt(EndpointData);

    } FINALLY {

    }

    return syncEvent;

} // ESM_SimulatingExpectedEventTRBsProcessedAfterHaltOnTimeout


ESM_EVENT
ESM_SimulatingExpectedEventTRBsProcessedOnTimeout(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_SimulatingExpectedEventTRBsProcessedOnTimeout is called when the
    state machine enters the SimulatingExpectedEventTRBsProcessedOnTimeout State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        syncEvent = Endpoint_SM_AssertAndSimulateExpectedEventTRBsProcessedAfterStop(EndpointData);

    } FINALLY {

    }

    return syncEvent;

} // ESM_SimulatingExpectedEventTRBsProcessedOnTimeout


ESM_EVENT
ESM_SimulatingFSEReceivedOnTimeout(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_SimulatingFSEReceivedOnTimeout is called when the
    state machine enters the SimulatingFSEReceivedOnTimeout State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        syncEvent = Endpoint_SM_AssertAndSimulateFSEReceived(EndpointData);

    } FINALLY {

    }

    return syncEvent;

} // ESM_SimulatingFSEReceivedOnTimeout


ESM_EVENT
ESM_StartingMapping(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_StartingMapping is called when the
    state machine enters the StartingMapping State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    ESM_EVENT                       syncEvent;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_StartMapping(EndpointData);

        syncEvent = EsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // ESM_StartingMapping


ESM_EVENT
ESM_StoppingEndpoint(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_StoppingEndpoint is called when the
    state machine enters the StoppingEndpoint State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_IncrementStopEndpointReissueCount(EndpointData);

        Endpoint_SM_SendStopEndpointCommand(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_StoppingEndpoint


ESM_EVENT
ESM_StoppingMappingAfterControllerSurpriseRemovalInEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_StoppingMappingAfterControllerSurpriseRemovalInEnabled is called when the
    state machine enters the StoppingMappingAfterControllerSurpriseRemovalInEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_StopMapping(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_StoppingMappingAfterControllerSurpriseRemovalInEnabled


ESM_EVENT
ESM_StoppingMappingBeforeEndpointReconfigure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_StoppingMappingBeforeEndpointReconfigure is called when the
    state machine enters the StoppingMappingBeforeEndpointReconfigure State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_StopMapping(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_StoppingMappingBeforeEndpointReconfigure


ESM_EVENT
ESM_StoppingMappingOnCancelStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_StoppingMappingOnCancelStart is called when the
    state machine enters the StoppingMappingOnCancelStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_StopMapping(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_StoppingMappingOnCancelStart


ESM_EVENT
ESM_StoppingMappingOnHalt(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_StoppingMappingOnHalt is called when the
    state machine enters the StoppingMappingOnHalt State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_StopMapping(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_StoppingMappingOnHalt


ESM_EVENT
ESM_UpdatingTransferRingDequeuePointers(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_UpdatingTransferRingDequeuePointers is called when the
    state machine enters the UpdatingTransferRingDequeuePointers State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_UpdateTransferRingDequeuePointers(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_UpdatingTransferRingDequeuePointers


ESM_EVENT
ESM_WaitingForControllerResetCompletion(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_WaitingForControllerResetCompletion is called when the
    state machine enters the WaitingForControllerResetCompletion State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_OkToCompleteEndpointResetRequest(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_WaitingForControllerResetCompletion


ESM_EVENT
ESM_WaitingForControllerResetOrSurpriseRemovalNotification(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_WaitingForControllerResetOrSurpriseRemovalNotification is called when the
    state machine enters the WaitingForControllerResetOrSurpriseRemovalNotification State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return EsmEventNull;

} // ESM_WaitingForControllerResetOrSurpriseRemovalNotification


ESM_EVENT
ESM_WaitingForEndpointDisable(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_WaitingForEndpointDisable is called when the
    state machine enters the WaitingForEndpointDisable State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return EsmEventNull;

} // ESM_WaitingForEndpointDisable


ESM_EVENT
ESM_WaitingForFSEAfterStopEndpointCommandCompletion(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_WaitingForFSEAfterStopEndpointCommandCompletion is called when the
    state machine enters the WaitingForFSEAfterStopEndpointCommandCompletion State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_ReportFSE(EndpointData);

        Endpoint_SM_Start2SecondTimer(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_WaitingForFSEAfterStopEndpointCommandCompletion


ESM_EVENT
ESM_WaitingForHaltedCompletionCode(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_WaitingForHaltedCompletionCode is called when the
    state machine enters the WaitingForHaltedCompletionCode State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_Start2SecondTimer(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_WaitingForHaltedCompletionCode


ESM_EVENT
ESM_WaitingForResetRequestFromClient(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_WaitingForResetRequestFromClient is called when the
    state machine enters the WaitingForResetRequestFromClient State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return EsmEventNull;

} // ESM_WaitingForResetRequestFromClient


ESM_EVENT
ESM_WaitingToReissueStopEndpointCommand1(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_WaitingToReissueStopEndpointCommand1 is called when the
    state machine enters the WaitingToReissueStopEndpointCommand1 State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_Start200MSTimer(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_WaitingToReissueStopEndpointCommand1


ESM_EVENT
ESM_WaitingToReissueStopEndpointCommand2(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    ESM_WaitingToReissueStopEndpointCommand2 is called when the
    state machine enters the WaitingToReissueStopEndpointCommand2 State

Arguments:

    SmContext - State Machine Context


Return Value:

    ESM_EVENT

--*/
{

    PENDPOINT_DATA                  EndpointData;
    
    TRY {

        EndpointData = SmContext->ParentContext;
        Endpoint_SM_Start500MSTimer(EndpointData);

    } FINALLY {

    }

    return EsmEventNull;

} // ESM_WaitingToReissueStopEndpointCommand2





