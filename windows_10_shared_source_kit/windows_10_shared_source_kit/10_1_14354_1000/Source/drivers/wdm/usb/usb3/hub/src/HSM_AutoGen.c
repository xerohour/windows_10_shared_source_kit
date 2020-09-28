
/*++
Copyright (c) Microsoft Corporation

Module Name:

HSM_AutoGen.c

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/


#include <pch.h>

HSM_EVENT
HUBHSM_AcquiringWdfPowerReferenceInitialD0Entry(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_AcquiringWdfPowerReferenceInitialD0Entry is called when the
    state machine enters the AcquiringWdfPowerReferenceInitialD0Entry State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_AcquireWdfPowerReference(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_AcquiringWdfPowerReferenceInitialD0Entry


HSM_EVENT
HUBHSM_AcquiringWdfPowerReferenceOnInterrupt(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_AcquiringWdfPowerReferenceOnInterrupt is called when the
    state machine enters the AcquiringWdfPowerReferenceOnInterrupt State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_AcquireWdfPowerReference(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_AcquiringWdfPowerReferenceOnInterrupt


HSM_EVENT
HUBHSM_AcquiringWdfPowerReferenceOnInterruptFailureAndCheckingIfRootHub(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_AcquiringWdfPowerReferenceOnInterruptFailureAndCheckingIfRootHub is called when the
    state machine enters the AcquiringWdfPowerReferenceOnInterruptFailureAndCheckingIfRootHub State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_AcquireWdfPowerReference(hubContext);

        syncEvent = HUBMISC_CheckIfHubDepthIsZero(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_AcquiringWdfPowerReferenceOnInterruptFailureAndCheckingIfRootHub


HSM_EVENT
HUBHSM_AcquiringWdfPowerReferenceOnReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_AcquiringWdfPowerReferenceOnReset is called when the
    state machine enters the AcquiringWdfPowerReferenceOnReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_AcquireWdfPowerReference(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_AcquiringWdfPowerReferenceOnReset


HSM_EVENT
HUBHSM_AcquiringWdfPowerReferenceOnResumeInSX(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_AcquiringWdfPowerReferenceOnResumeInSX is called when the
    state machine enters the AcquiringWdfPowerReferenceOnResumeInSX State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_AcquireWdfPowerReference(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_AcquiringWdfPowerReferenceOnResumeInSX


HSM_EVENT
HUBHSM_AcquiringWdfReferenceOnResumeInS0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_AcquiringWdfReferenceOnResumeInS0 is called when the
    state machine enters the AcquiringWdfReferenceOnResumeInS0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_AcquireWdfPowerReference(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_AcquiringWdfReferenceOnResumeInS0


HSM_EVENT
HUBHSM_BugcheckingSystemOnExcessiveResetsForHubInBootPath(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_BugcheckingSystemOnExcessiveResetsForHubInBootPath is called when the
    state machine enters the BugcheckingSystemOnExcessiveResetsForHubInBootPath State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
//
// Temperorily disable the warning for unreachable code
// We need to return an event from this function because
// it is a state machine function
//
#pragma warning(push)
#pragma warning(disable:4702)
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_BugcheckSystemOnBootHubFailure(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_BugcheckingSystemOnExcessiveResetsForHubInBootPath


#pragma warning(pop)
HSM_EVENT
HUBHSM_CancellingInterruptTransferOnReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CancellingInterruptTransferOnReset is called when the
    state machine enters the CancellingInterruptTransferOnReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_CancelInterruptTransfer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_CancellingInterruptTransferOnReset


HSM_EVENT
HUBHSM_CancellingInterruptTransferOnStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CancellingInterruptTransferOnStop is called when the
    state machine enters the CancellingInterruptTransferOnStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_CancelInterruptTransfer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_CancellingInterruptTransferOnStop


HSM_EVENT
HUBHSM_CancellingInterruptTransferOnSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CancellingInterruptTransferOnSuspend is called when the
    state machine enters the CancellingInterruptTransferOnSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_CancelInterruptTransfer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_CancellingInterruptTransferOnSuspend


HSM_EVENT
HUBHSM_CheckIfThereIsAValidHubChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckIfThereIsAValidHubChange is called when the
    state machine enters the CheckIfThereIsAValidHubChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBHTX_CheckAndSelectIfAnyHubChangeSet(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CheckIfThereIsAValidHubChange


HSM_EVENT
HUBHSM_CheckingForUnexpectedPowerLoss(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckingForUnexpectedPowerLoss is called when the
    state machine enters the CheckingForUnexpectedPowerLoss State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_CheckingForUnexpectedPowerLoss


HSM_EVENT
HUBHSM_CheckingIfHubIsInBootPath(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckingIfHubIsInBootPath is called when the
    state machine enters the CheckingIfHubIsInBootPath State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBFDO_CheckIfHubIsInBootPath(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CheckingIfHubIsInBootPath


HSM_EVENT
HUBHSM_CheckingIfResetByParent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckingIfResetByParent is called when the
    state machine enters the CheckingIfResetByParent State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBPARENT_QueryParentIfDeviceWasReset(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CheckingIfResetByParent


HSM_EVENT
HUBHSM_CheckingIfResetByParentWithResetPending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckingIfResetByParentWithResetPending is called when the
    state machine enters the CheckingIfResetByParentWithResetPending State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBPARENT_QueryParentIfDeviceWasReset(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CheckingIfResetByParentWithResetPending


HSM_EVENT
HUBHSM_CheckingIfResetRecoveryHasBeenInvokedTooManyTimes(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckingIfResetRecoveryHasBeenInvokedTooManyTimes is called when the
    state machine enters the CheckingIfResetRecoveryHasBeenInvokedTooManyTimes State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IncrementAndCheckIfRecoveryCountExceededLimit(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CheckingIfResetRecoveryHasBeenInvokedTooManyTimes


HSM_EVENT
HUBHSM_CheckingPSMInterruptReferencesOnReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckingPSMInterruptReferencesOnReset is called when the
    state machine enters the CheckingPSMInterruptReferencesOnReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBMUX_CheckIfAllPSMInterruptReferencesAreReleased(hubContext);

        HUBMUX_QueueSurpriseRemovalToAllPSMs(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CheckingPSMInterruptReferencesOnReset


HSM_EVENT
HUBHSM_CheckingPSMInterruptReferencesOnResetInNoInterrupt(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckingPSMInterruptReferencesOnResetInNoInterrupt is called when the
    state machine enters the CheckingPSMInterruptReferencesOnResetInNoInterrupt State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueSurpriseRemovalToAllPSMs(hubContext);

        syncEvent = HUBMUX_CheckIfAllPSMInterruptReferencesAreReleased(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CheckingPSMInterruptReferencesOnResetInNoInterrupt


HSM_EVENT
HUBHSM_CheckingPSMInterruptReferencesOnResetInNoInterruptOnFatalError(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckingPSMInterruptReferencesOnResetInNoInterruptOnFatalError is called when the
    state machine enters the CheckingPSMInterruptReferencesOnResetInNoInterruptOnFatalError State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueSurpriseRemovalToAllPSMs(hubContext);

        syncEvent = HUBMUX_CheckIfAllPSMInterruptReferencesAreReleased(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CheckingPSMInterruptReferencesOnResetInNoInterruptOnFatalError


HSM_EVENT
HUBHSM_CheckingPSMInterruptReferencesOnStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckingPSMInterruptReferencesOnStop is called when the
    state machine enters the CheckingPSMInterruptReferencesOnStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBMUX_CheckIfAllPSMInterruptReferencesAreReleased(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CheckingPSMInterruptReferencesOnStop


HSM_EVENT
HUBHSM_CheckingPSMInterruptReferencesOnStopInNoInterrupt(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckingPSMInterruptReferencesOnStopInNoInterrupt is called when the
    state machine enters the CheckingPSMInterruptReferencesOnStopInNoInterrupt State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBMUX_CheckIfAllPSMInterruptReferencesAreReleased(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CheckingPSMInterruptReferencesOnStopInNoInterrupt


HSM_EVENT
HUBHSM_CheckingPSMInterruptReferencesOnSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckingPSMInterruptReferencesOnSuspend is called when the
    state machine enters the CheckingPSMInterruptReferencesOnSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBMUX_CheckIfAllPSMInterruptReferencesAreReleased(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CheckingPSMInterruptReferencesOnSuspend


HSM_EVENT
HUBHSM_CheckingPSMInterruptReferencesOnSxInNoInterrupt(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckingPSMInterruptReferencesOnSxInNoInterrupt is called when the
    state machine enters the CheckingPSMInterruptReferencesOnSxInNoInterrupt State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBMUX_CheckIfAllPSMInterruptReferencesAreReleased(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CheckingPSMInterruptReferencesOnSxInNoInterrupt


HSM_EVENT
HUBHSM_CheckOverCurrentBit(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckOverCurrentBit is called when the
    state machine enters the CheckOverCurrentBit State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBHTX_IsOverCurrentBitCleared(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CheckOverCurrentBit


HSM_EVENT
HUBHSM_CompletingPDORequestForGetPortStatus(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CompletingPDORequestForGetPortStatus is called when the
    state machine enters the CompletingPDORequestForGetPortStatus State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_CompleteGetPortStatusWithPortStatusResult(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CompletingPDORequestForGetPortStatus


HSM_EVENT
HUBHSM_CompletingPDORequestForGetPortStatusFromConfigWithIntTransfer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CompletingPDORequestForGetPortStatusFromConfigWithIntTransfer is called when the
    state machine enters the CompletingPDORequestForGetPortStatusFromConfigWithIntTransfer State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_CompleteGetPortStatusWithPortStatusResult(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CompletingPDORequestForGetPortStatusFromConfigWithIntTransfer


HSM_EVENT
HUBHSM_CompletingPDORequestForGetPortStatusFromConfigWithIntTransferWithNoSuchDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CompletingPDORequestForGetPortStatusFromConfigWithIntTransferWithNoSuchDevice is called when the
    state machine enters the CompletingPDORequestForGetPortStatusFromConfigWithIntTransferWithNoSuchDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_CompleteGetPortStatusWithNoSuchDevice(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CompletingPDORequestForGetPortStatusFromConfigWithIntTransferWithNoSuchDevice


HSM_EVENT
HUBHSM_CompletingPDORequestForGetPortStatusInSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CompletingPDORequestForGetPortStatusInSuspended is called when the
    state machine enters the CompletingPDORequestForGetPortStatusInSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_CompleteGetPortStatusWithPortStatusResult(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CompletingPDORequestForGetPortStatusInSuspended


HSM_EVENT
HUBHSM_CompletingPDORequestForGetPortStatusWithNoSuchDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CompletingPDORequestForGetPortStatusWithNoSuchDevice is called when the
    state machine enters the CompletingPDORequestForGetPortStatusWithNoSuchDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_CompleteGetPortStatusWithNoSuchDevice(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CompletingPDORequestForGetPortStatusWithNoSuchDevice


HSM_EVENT
HUBHSM_ConfiguredWithIntTransfer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ConfiguredWithIntTransfer is called when the
    state machine enters the ConfiguredWithIntTransfer State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_ConfiguredWithIntTransfer


HSM_EVENT
HUBHSM_ConfiguredWithoutInterruptTransfer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ConfiguredWithoutInterruptTransfer is called when the
    state machine enters the ConfiguredWithoutInterruptTransfer State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_ConfiguredWithoutInterruptTransfer


HSM_EVENT
HUBHSM_Configuring(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_Configuring is called when the
    state machine enters the Configuring State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBUCX_GetControllerInfo(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_Configuring


HSM_EVENT
HUBHSM_ConfiguringHubAfterReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ConfiguringHubAfterReset is called when the
    state machine enters the ConfiguringHubAfterReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_LogHubResetRecoveryWasInvoked(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_ConfiguringHubAfterReset


HSM_EVENT
HUBHSM_ConfiguringHubAfterResetOnSystemResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ConfiguringHubAfterResetOnSystemResume is called when the
    state machine enters the ConfiguringHubAfterResetOnSystemResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_ConfiguringHubAfterResetOnSystemResume


HSM_EVENT
HUBHSM_CreatingChildPSMs(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CreatingChildPSMs is called when the
    state machine enters the CreatingChildPSMs State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBMUX_CreateChildPSMs(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CreatingChildPSMs


HSM_EVENT
HUBHSM_ErrorOnWaitingForOvercurrentClear(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ErrorOnWaitingForOvercurrentClear is called when the
    state machine enters the ErrorOnWaitingForOvercurrentClear State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBHTX_GetErrorResponseEventOnBadStatusChange(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_ErrorOnWaitingForOvercurrentClear


HSM_EVENT
HUBHSM_GettingAdditionalConfigInfo(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_GettingAdditionalConfigInfo is called when the
    state machine enters the GettingAdditionalConfigInfo State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_GettingAdditionalConfigInfo


HSM_EVENT
HUBHSM_GettingHubPortStatus(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_GettingHubPortStatus is called when the
    state machine enters the GettingHubPortStatus State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_GetHubStatusUsingControlTransfer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_GettingHubPortStatus


HSM_EVENT
HUBHSM_GettingHubPortStatusForLostChanges(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_GettingHubPortStatusForLostChanges is called when the
    state machine enters the GettingHubPortStatusForLostChanges State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_GetHubStatusUsingControlTransfer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_GettingHubPortStatusForLostChanges


HSM_EVENT
HUBHSM_GettingOverCurrentBitStatus(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_GettingOverCurrentBitStatus is called when the
    state machine enters the GettingOverCurrentBitStatus State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_GetHubStatusUsingControlTransfer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_GettingOverCurrentBitStatus


HSM_EVENT
HUBHSM_GettingPortStatusOnPDORequest(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_GettingPortStatusOnPDORequest is called when the
    state machine enters the GettingPortStatusOnPDORequest State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_GetRequestedPortStatusUsingControlTransfer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_GettingPortStatusOnPDORequest


HSM_EVENT
HUBHSM_GettingPortStatusOnPDORequestFromConfigWithIntTransfer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_GettingPortStatusOnPDORequestFromConfigWithIntTransfer is called when the
    state machine enters the GettingPortStatusOnPDORequestFromConfigWithIntTransfer State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_GetRequestedPortStatusUsingControlTransfer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_GettingPortStatusOnPDORequestFromConfigWithIntTransfer


HSM_EVENT
HUBHSM_HasResetCountReachedMaximum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_HasResetCountReachedMaximum is called when the
    state machine enters the HasResetCountReachedMaximum State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IncrementAndCheckIfResetCountExceededMaximum(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_HasResetCountReachedMaximum


HSM_EVENT
HUBHSM_HasResetPipeCountReachedMaximum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_HasResetPipeCountReachedMaximum is called when the
    state machine enters the HasResetPipeCountReachedMaximum State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IncrementAndCheckIfInterruptPipeResetCountExceededMaximum(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_HasResetPipeCountReachedMaximum


HSM_EVENT
HUBHSM_HubStatusError(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_HubStatusError is called when the
    state machine enters the HubStatusError State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBHTX_GetErrorResponseEventOnBadStatusChange(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_HubStatusError


HSM_EVENT
HUBHSM_InitializingResetCount(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_InitializingResetCount is called when the
    state machine enters the InitializingResetCount State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMISC_InitializeHubResetCount(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_InitializingResetCount


HSM_EVENT
HUBHSM_IsItHubChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_IsItHubChange is called when the
    state machine enters the IsItHubChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBHTX_IsHubChangeBitSet(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_IsItHubChange


HSM_EVENT
HUBHSM_LoggingHubWasReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_LoggingHubWasReset is called when the
    state machine enters the LoggingHubWasReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_LogHubResetRecoveryWasInvoked(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_LoggingHubWasReset


HSM_EVENT
HUBHSM_NotifyingHubResetToPortsOnReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_NotifyingHubResetToPortsOnReset is called when the
    state machine enters the NotifyingHubResetToPortsOnReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueResetToAllPSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_NotifyingHubResetToPortsOnReset


HSM_EVENT
HUBHSM_NotifyingHubResetToPortsOnResetOnFatalError(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_NotifyingHubResetToPortsOnResetOnFatalError is called when the
    state machine enters the NotifyingHubResetToPortsOnResetOnFatalError State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueResetToAllPSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_NotifyingHubResetToPortsOnResetOnFatalError


HSM_EVENT
HUBHSM_NotifyingHubResetToPortsOnResetOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_NotifyingHubResetToPortsOnResetOnResume is called when the
    state machine enters the NotifyingHubResetToPortsOnResetOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueResetToAllPSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_NotifyingHubResetToPortsOnResetOnResume


HSM_EVENT
HUBHSM_NotifyingHubResumeInS0ToDevices(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_NotifyingHubResumeInS0ToDevices is called when the
    state machine enters the NotifyingHubResumeInS0ToDevices State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueResumeInS0ToSuspendedDSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_NotifyingHubResumeInS0ToDevices


HSM_EVENT
HUBHSM_NotifyingHubResumeToDevices(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_NotifyingHubResumeToDevices is called when the
    state machine enters the NotifyingHubResumeToDevices State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueResumeToSuspendedDSMs(hubContext);

        HUBFDO_SignalPnpPowerEvent(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_NotifyingHubResumeToDevices


HSM_EVENT
HUBHSM_NotifyingHubResumeWithResetToDevices(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_NotifyingHubResumeWithResetToDevices is called when the
    state machine enters the NotifyingHubResumeWithResetToDevices State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueResumeWithResetToSuspendedDSMs(hubContext);

        HUBFDO_SignalPnpPowerEvent(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_NotifyingHubResumeWithResetToDevices


HSM_EVENT
HUBHSM_NotifyingHubResumeWithResetToDevicesOnFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_NotifyingHubResumeWithResetToDevicesOnFailure is called when the
    state machine enters the NotifyingHubResumeWithResetToDevicesOnFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_LogHubWasResetOnResume(hubContext);

        HUBMUX_QueueResumeWithResetToSuspendedDSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_NotifyingHubResumeWithResetToDevicesOnFailure


HSM_EVENT
HUBHSM_NotifyingHubStopToDevices(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_NotifyingHubStopToDevices is called when the
    state machine enters the NotifyingHubStopToDevices State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueStopToAllDSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_NotifyingHubStopToDevices


HSM_EVENT
HUBHSM_NotifyingHubStopToDevicesOnSuspriseRemove(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_NotifyingHubStopToDevicesOnSuspriseRemove is called when the
    state machine enters the NotifyingHubStopToDevicesOnSuspriseRemove State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueStopAfterSuspendToAllDSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_NotifyingHubStopToDevicesOnSuspriseRemove


HSM_EVENT
HUBHSM_NotifyingHubStopToPorts(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_NotifyingHubStopToPorts is called when the
    state machine enters the NotifyingHubStopToPorts State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueStopToAllPSMs(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_NotifyingHubStopToPorts


HSM_EVENT
HUBHSM_NotifyingHubStopToPortsOnFatalError(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_NotifyingHubStopToPortsOnFatalError is called when the
    state machine enters the NotifyingHubStopToPortsOnFatalError State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueStopToAllPSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_NotifyingHubStopToPortsOnFatalError


HSM_EVENT
HUBHSM_NotifyingHubStopToPortsOnReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_NotifyingHubStopToPortsOnReset is called when the
    state machine enters the NotifyingHubStopToPortsOnReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueStopToAllPSMs(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_NotifyingHubStopToPortsOnReset


HSM_EVENT
HUBHSM_QueryingForHubHackFlags(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_QueryingForHubHackFlags is called when the
    state machine enters the QueryingForHubHackFlags State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBFDO_QueryHubErrataFlags(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_QueryingForHubHackFlags


HSM_EVENT
HUBHSM_QueueingFakeStatusChangeOnResumeInS0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_QueueingFakeStatusChangeOnResumeInS0 is called when the
    state machine enters the QueueingFakeStatusChangeOnResumeInS0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueFakeStatusChangeToAllPSMs(hubContext);

        HUBFDO_SignalPnpPowerEvent(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_QueueingFakeStatusChangeOnResumeInS0


HSM_EVENT
HUBHSM_QueueingHubStatusForLostChanges(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_QueueingHubStatusForLostChanges is called when the
    state machine enters the QueueingHubStatusForLostChanges State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBHTX_GetHubLostChangeEvent(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_QueueingHubStatusForLostChanges


HSM_EVENT
HUBHSM_QueueingPortStatusChangeEvents(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_QueueingPortStatusChangeEvents is called when the
    state machine enters the QueueingPortStatusChangeEvents State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueStatusChangeToPSMs(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_QueueingPortStatusChangeEvents


HSM_EVENT
HUBHSM_QueuingHubChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_QueuingHubChange is called when the
    state machine enters the QueuingHubChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBHTX_GetHubChangeEvent(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_QueuingHubChange


HSM_EVENT
HUBHSM_ReConfiguring(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ReConfiguring is called when the
    state machine enters the ReConfiguring State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_ReConfiguring


HSM_EVENT
HUBHSM_ReportingErrorToPnp(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ReportingErrorToPnp is called when the
    state machine enters the ReportingErrorToPnp State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_ReportDeviceFailureToPnp(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_ReportingErrorToPnp


HSM_EVENT
HUBHSM_ReportingReStartFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ReportingReStartFailure is called when the
    state machine enters the ReportingReStartFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_SignalPnpPowerFailureEvent(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_ReportingReStartFailure


HSM_EVENT
HUBHSM_ReportingStartFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ReportingStartFailure is called when the
    state machine enters the ReportingStartFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_SignalPnpPowerFailureEvent(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_ReportingStartFailure


HSM_EVENT
HUBHSM_ResettingHub(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ResettingHub is called when the
    state machine enters the ResettingHub State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBPARENT_ResetHub(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_ResettingHub


HSM_EVENT
HUBHSM_ResettingHubOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ResettingHubOnResume is called when the
    state machine enters the ResettingHubOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBPARENT_ResetHub(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_ResettingHubOnResume


HSM_EVENT
HUBHSM_ResettingInterruptPipeOnFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ResettingInterruptPipeOnFailure is called when the
    state machine enters the ResettingInterruptPipeOnFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBPARENT_ResetInterruptPipe(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_ResettingInterruptPipeOnFailure


HSM_EVENT
HUBHSM_SendingAckForHubChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_SendingAckForHubChange is called when the
    state machine enters the SendingAckForHubChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_AckHubChangeUsingControlTransfer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_SendingAckForHubChange


HSM_EVENT
HUBHSM_SendingInterruptTransfer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_SendingInterruptTransfer is called when the
    state machine enters the SendingInterruptTransfer State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_ReleaseWdfPowerReference(hubContext);

        HUBHTX_SendInterruptTransfer(hubContext);

        HUBMISC_ResetCountForResetInterruptPipe(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_SendingInterruptTransfer


HSM_EVENT
HUBHSM_SendingInterruptTransferAfterResettingPipe(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_SendingInterruptTransferAfterResettingPipe is called when the
    state machine enters the SendingInterruptTransferAfterResettingPipe State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_ReleaseWdfPowerReference(hubContext);

        HUBHTX_SendInterruptTransfer(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_SendingInterruptTransferAfterResettingPipe


HSM_EVENT
HUBHSM_SignalingPnPPowerEventOnPendingStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_SignalingPnPPowerEventOnPendingStop is called when the
    state machine enters the SignalingPnPPowerEventOnPendingStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_ReleaseWdfPowerReference(hubContext);

        HUBFDO_SignalPnpPowerEvent(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_SignalingPnPPowerEventOnPendingStop


HSM_EVENT
HUBHSM_SignalingPnPPowerEventOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_SignalingPnPPowerEventOnStart is called when the
    state machine enters the SignalingPnPPowerEventOnStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_SignalPnpPowerEvent(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_SignalingPnPPowerEventOnStart


HSM_EVENT
HUBHSM_SignalingPnPPowerEventOnStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_SignalingPnPPowerEventOnStop is called when the
    state machine enters the SignalingPnPPowerEventOnStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_SignalPnpPowerEvent(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_SignalingPnPPowerEventOnStop


HSM_EVENT
HUBHSM_SignallingPnpPowerEventForSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_SignallingPnpPowerEventForSuspended is called when the
    state machine enters the SignallingPnpPowerEventForSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_SignalPnpPowerEvent(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_SignallingPnpPowerEventForSuspended


HSM_EVENT
HUBHSM_SignallingPnpPowerEventInUnconfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_SignallingPnpPowerEventInUnconfigured is called when the
    state machine enters the SignallingPnpPowerEventInUnconfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_SignalPnpPowerEvent(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_SignallingPnpPowerEventInUnconfigured


HSM_EVENT
HUBHSM_StartingTimerForResetRetry(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_StartingTimerForResetRetry is called when the
    state machine enters the StartingTimerForResetRetry State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMISC_StartResetRetryTimer(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_StartingTimerForResetRetry


HSM_EVENT
HUBHSM_Stopped(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_Stopped is called when the
    state machine enters the Stopped State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_Stopped


HSM_EVENT
HUBHSM_SupriseRemoved(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_SupriseRemoved is called when the
    state machine enters the SupriseRemoved State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_SignalPnpPowerEvent(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_SupriseRemoved


HSM_EVENT
HUBHSM_Suspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_Suspended is called when the
    state machine enters the Suspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_Suspended


HSM_EVENT
HUBHSM_SuspendedInRecycle(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_SuspendedInRecycle is called when the
    state machine enters the SuspendedInRecycle State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_SuspendedInRecycle


HSM_EVENT
HUBHSM_SuspendedWithPendingReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_SuspendedWithPendingReset is called when the
    state machine enters the SuspendedWithPendingReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_AcquireWdfPowerReference(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_SuspendedWithPendingReset


HSM_EVENT
HUBHSM_UnblockingResumeOnHubResetFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_UnblockingResumeOnHubResetFailure is called when the
    state machine enters the UnblockingResumeOnHubResetFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_SignalPnpPowerEvent(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_UnblockingResumeOnHubResetFailure


HSM_EVENT
HUBHSM_UnblockResumeWaitingForStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_UnblockResumeWaitingForStop is called when the
    state machine enters the UnblockResumeWaitingForStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_AcquireWdfPowerReference(hubContext);

        HUBFDO_SignalPnpPowerEvent(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_UnblockResumeWaitingForStop


HSM_EVENT
HUBHSM_Unconfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_Unconfigured is called when the
    state machine enters the Unconfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_Unconfigured


HSM_EVENT
HUBHSM_WaitingForAddDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForAddDevice is called when the
    state machine enters the WaitingForAddDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_WaitingForAddDevice


HSM_EVENT
HUBHSM_WaitingForD0Entry(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForD0Entry is called when the
    state machine enters the WaitingForD0Entry State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_SignalPnpPowerEvent(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForD0Entry


HSM_EVENT
HUBHSM_WaitingForDevicesToAcquireReferenceOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForDevicesToAcquireReferenceOnStart is called when the
    state machine enters the WaitingForDevicesToAcquireReferenceOnStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueStartToStoppedDSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForDevicesToAcquireReferenceOnStart


HSM_EVENT
HUBHSM_WaitingForDevicesToReleaseReferenceOnD0ExitFinalFromReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForDevicesToReleaseReferenceOnD0ExitFinalFromReset is called when the
    state machine enters the WaitingForDevicesToReleaseReferenceOnD0ExitFinalFromReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_ReleaseWdfPowerReference(hubContext);

        HUBMUX_QueueStopToAllDSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForDevicesToReleaseReferenceOnD0ExitFinalFromReset


HSM_EVENT
HUBHSM_WaitingForDevicesToReleaseReferenceOnSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForDevicesToReleaseReferenceOnSuspend is called when the
    state machine enters the WaitingForDevicesToReleaseReferenceOnSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueSuspendToAllDSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForDevicesToReleaseReferenceOnSuspend


HSM_EVENT
HUBHSM_WaitingForEnableInterruptsOnReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForEnableInterruptsOnReset is called when the
    state machine enters the WaitingForEnableInterruptsOnReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_WaitingForEnableInterruptsOnReset


HSM_EVENT
HUBHSM_WaitingForEnableInterruptsOnStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForEnableInterruptsOnStop is called when the
    state machine enters the WaitingForEnableInterruptsOnStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_ReleaseWdfPowerReference(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForEnableInterruptsOnStop


HSM_EVENT
HUBHSM_WaitingForEnableInterruptsOnSx(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForEnableInterruptsOnSx is called when the
    state machine enters the WaitingForEnableInterruptsOnSx State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_ReleaseWdfPowerReference(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForEnableInterruptsOnSx


HSM_EVENT
HUBHSM_WaitingForInterruptReferencesReleasedOnFatalError(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForInterruptReferencesReleasedOnFatalError is called when the
    state machine enters the WaitingForInterruptReferencesReleasedOnFatalError State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_WaitingForInterruptReferencesReleasedOnFatalError


HSM_EVENT
HUBHSM_WaitingForInterruptReferencesReleasedOnReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForInterruptReferencesReleasedOnReset is called when the
    state machine enters the WaitingForInterruptReferencesReleasedOnReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_WaitingForInterruptReferencesReleasedOnReset


HSM_EVENT
HUBHSM_WaitingForInterruptReferencesReleasedOnStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForInterruptReferencesReleasedOnStop is called when the
    state machine enters the WaitingForInterruptReferencesReleasedOnStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_WaitingForInterruptReferencesReleasedOnStop


HSM_EVENT
HUBHSM_WaitingForInterruptReferencesReleasedOnSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForInterruptReferencesReleasedOnSuspend is called when the
    state machine enters the WaitingForInterruptReferencesReleasedOnSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_WaitingForInterruptReferencesReleasedOnSuspend


HSM_EVENT
HUBHSM_WaitingForOvercurrentToClear(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForOvercurrentToClear is called when the
    state machine enters the WaitingForOvercurrentToClear State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMISC_StartHubOverCurrentTimer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForOvercurrentToClear


HSM_EVENT
HUBHSM_WaitingForPortsToAcquireReferenceOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForPortsToAcquireReferenceOnResume is called when the
    state machine enters the WaitingForPortsToAcquireReferenceOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueResumeToAllPSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForPortsToAcquireReferenceOnResume


HSM_EVENT
HUBHSM_WaitingForPortsToAcquireReferenceOnResumeInS0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForPortsToAcquireReferenceOnResumeInS0 is called when the
    state machine enters the WaitingForPortsToAcquireReferenceOnResumeInS0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueResumeInS0ToAllPSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForPortsToAcquireReferenceOnResumeInS0


HSM_EVENT
HUBHSM_WaitingForPortsToAcquireReferenceOnResumeWithReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForPortsToAcquireReferenceOnResumeWithReset is called when the
    state machine enters the WaitingForPortsToAcquireReferenceOnResumeWithReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_LogHubWasResetOnResume(hubContext);

        HUBMUX_QueueResumeWithResetToAllPSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForPortsToAcquireReferenceOnResumeWithReset


HSM_EVENT
HUBHSM_WaitingForPortsToAcquireReferenceOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForPortsToAcquireReferenceOnStart is called when the
    state machine enters the WaitingForPortsToAcquireReferenceOnStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_EnableResetHub(hubContext);

        HUBMUX_QueueStartToAllPSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForPortsToAcquireReferenceOnStart


HSM_EVENT
HUBHSM_WaitingForPortsToAcquireReferencePostReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForPortsToAcquireReferencePostReset is called when the
    state machine enters the WaitingForPortsToAcquireReferencePostReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_EnableResetHub(hubContext);

        HUBMUX_QueueStartToAllPSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForPortsToAcquireReferencePostReset


HSM_EVENT
HUBHSM_WaitingForPortsToReleaseReferenceOnReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForPortsToReleaseReferenceOnReset is called when the
    state machine enters the WaitingForPortsToReleaseReferenceOnReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_WaitingForPortsToReleaseReferenceOnReset


HSM_EVENT
HUBHSM_WaitingForPortsToReleaseReferenceOnResetOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForPortsToReleaseReferenceOnResetOnResume is called when the
    state machine enters the WaitingForPortsToReleaseReferenceOnResetOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueStopToAllPSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForPortsToReleaseReferenceOnResetOnResume


HSM_EVENT
HUBHSM_WaitingForPortsToReleaseReferenceOnStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForPortsToReleaseReferenceOnStop is called when the
    state machine enters the WaitingForPortsToReleaseReferenceOnStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_WaitingForPortsToReleaseReferenceOnStop


HSM_EVENT
HUBHSM_WaitingForPortsToReleaseReferenceOnSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForPortsToReleaseReferenceOnSuspend is called when the
    state machine enters the WaitingForPortsToReleaseReferenceOnSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMUX_QueueSuspendToAllPSMs(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForPortsToReleaseReferenceOnSuspend


HSM_EVENT
HUBHSM_WaitingForPSMInterruptReferencesOnReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForPSMInterruptReferencesOnReset is called when the
    state machine enters the WaitingForPSMInterruptReferencesOnReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_WaitingForPSMInterruptReferencesOnReset


HSM_EVENT
HUBHSM_WaitingForPSMInterruptReferencesOnStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForPSMInterruptReferencesOnStop is called when the
    state machine enters the WaitingForPSMInterruptReferencesOnStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_WaitingForPSMInterruptReferencesOnStop


HSM_EVENT
HUBHSM_WaitingForPSMInterruptReferencesOnSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForPSMInterruptReferencesOnSuspend is called when the
    state machine enters the WaitingForPSMInterruptReferencesOnSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_WaitingForPSMInterruptReferencesOnSuspend


HSM_EVENT
HUBHSM_WaitingForReleaseHardware(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForReleaseHardware is called when the
    state machine enters the WaitingForReleaseHardware State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_SignalPnpPowerEvent(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForReleaseHardware


HSM_EVENT
HUBHSM_WaitingForReleaseHardwareOnReStartFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForReleaseHardwareOnReStartFailure is called when the
    state machine enters the WaitingForReleaseHardwareOnReStartFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_WaitingForReleaseHardwareOnReStartFailure


HSM_EVENT
HUBHSM_WaitingForReleaseHardwareOnStartFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForReleaseHardwareOnStartFailure is called when the
    state machine enters the WaitingForReleaseHardwareOnStartFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_WaitingForReleaseHardwareOnStartFailure


HSM_EVENT
HUBHSM_WaitingForResetRetryTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForResetRetryTimer is called when the
    state machine enters the WaitingForResetRetryTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_WaitingForResetRetryTimer


HSM_EVENT
HUBHSM_WaitingForStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForStop is called when the
    state machine enters the WaitingForStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_WaitingForStop


HSM_EVENT
HUBHSM_WaitingToBeDeleted(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingToBeDeleted is called when the
    state machine enters the WaitingToBeDeleted State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBFDO_SignalPnpPowerEvent(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingToBeDeleted


HSM_EVENT
HUBHSM_CheckingBytesReturnedInHubConfigDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckingBytesReturnedInHubConfigDescriptor is called when the
    state machine enters the CheckingBytesReturnedInHubConfigDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBHTX_IsBytesReturnedLessThanTotalLengthForHubConfigDescriptor(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CheckingBytesReturnedInHubConfigDescriptor


HSM_EVENT
HUBHSM_CheckingIfHubDescriptorRetryCountExceededMax(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_CheckingIfHubDescriptorRetryCountExceededMax is called when the
    state machine enters the CheckingIfHubDescriptorRetryCountExceededMax State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IncrementAndCheckIfHubDescriptorRetryCountExceededMax(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_CheckingIfHubDescriptorRetryCountExceededMax


HSM_EVENT
HUBHSM_ConfiguringAfterSetConfig(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ConfiguringAfterSetConfig is called when the
    state machine enters the ConfiguringAfterSetConfig State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return HsmEventNull;

} // HUBHSM_ConfiguringAfterSetConfig


HSM_EVENT
HUBHSM_GettingHubConfigurationDescriptorWithDefaultLength(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_GettingHubConfigurationDescriptorWithDefaultLength is called when the
    state machine enters the GettingHubConfigurationDescriptorWithDefaultLength State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_GetHubConfigDescriptorWithDefaultSizeUsingControlTransfer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_GettingHubConfigurationDescriptorWithDefaultLength


HSM_EVENT
HUBHSM_GettingHubConfigurationDescriptorWithReturnedLength(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_GettingHubConfigurationDescriptorWithReturnedLength is called when the
    state machine enters the GettingHubConfigurationDescriptorWithReturnedLength State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_GetHubConfigDescriptorWithReturnedLengthUsingControlTransfer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_GettingHubConfigurationDescriptorWithReturnedLength


HSM_EVENT
HUBHSM_GettingHubDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_GettingHubDescriptor is called when the
    state machine enters the GettingHubDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_GetHubDescriptorUsingControlTransfer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_GettingHubDescriptor


HSM_EVENT
HUBHSM_GettingHubStatus(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_GettingHubStatus is called when the
    state machine enters the GettingHubStatus State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_GetHubStandardStatusUsingControlTransfer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_GettingHubStatus


HSM_EVENT
HUBHSM_InitializingHubDescriptorRetryCount(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_InitializingHubDescriptorRetryCount is called when the
    state machine enters the InitializingHubDescriptorRetryCount State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMISC_InitializeHubDescriptorRetryCount(hubContext);

        syncEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_InitializingHubDescriptorRetryCount


HSM_EVENT
HUBHSM_ReturningConfigurationFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ReturningConfigurationFailure is called when the
    state machine enters the ReturningConfigurationFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return HsmEventHubConfigurationFailure;

} // HUBHSM_ReturningConfigurationFailure


HSM_EVENT
HUBHSM_ReturningConfigurationSuccess(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ReturningConfigurationSuccess is called when the
    state machine enters the ReturningConfigurationSuccess State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return HsmEventHubConfigurationSuccess;

} // HUBHSM_ReturningConfigurationSuccess


HSM_EVENT
HUBHSM_SettingHubConfiguration(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_SettingHubConfiguration is called when the
    state machine enters the SettingHubConfiguration State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBPARENT_SetHubConfiguration(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_SettingHubConfiguration


HSM_EVENT
HUBHSM_UpdatingHubInfoInUCX(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_UpdatingHubInfoInUCX is called when the
    state machine enters the UpdatingHubInfoInUCX State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBUCX_UpdateHubInformationUsingUCXIoctl(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_UpdatingHubInfoInUCX


HSM_EVENT
HUBHSM_ValidatingAndParsingHubDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ValidatingAndParsingHubDescriptor is called when the
    state machine enters the ValidatingAndParsingHubDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBHTX_ValidateAndParseHubDescriptor(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_ValidatingAndParsingHubDescriptor


HSM_EVENT
HUBHSM_ValidatingAndParsingHubPowerStatus(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ValidatingAndParsingHubPowerStatus is called when the
    state machine enters the ValidatingAndParsingHubPowerStatus State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBHTX_ValidateAndParseDevicePowerStatus(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_ValidatingAndParsingHubPowerStatus


HSM_EVENT
HUBHSM_ValidatingHubConfigurationDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ValidatingHubConfigurationDescriptor is called when the
    state machine enters the ValidatingHubConfigurationDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    HSM_EVENT                       syncEvent;
    
    TRY {

        hubContext = SmContext->ParentContext;
        syncEvent = HUBHTX_ValidateAndCacheHubConfigDescriptor(hubContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBHSM_ValidatingHubConfigurationDescriptor


HSM_EVENT
HUBHSM_WaitingForHubDescriptorRetryTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_WaitingForHubDescriptorRetryTimer is called when the
    state machine enters the WaitingForHubDescriptorRetryTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBMISC_StartHubDescriptorRetryTimer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_WaitingForHubDescriptorRetryTimer


HSM_EVENT
HUBHSM_GettingRootHub20PortsInfo(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_GettingRootHub20PortsInfo is called when the
    state machine enters the GettingRootHub20PortsInfo State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBUCX_GetRootHub20PortsInfoUsingUCXIoctl(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_GettingRootHub20PortsInfo


HSM_EVENT
HUBHSM_GettingRootHub30PortsInfo(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_GettingRootHub30PortsInfo is called when the
    state machine enters the GettingRootHub30PortsInfo State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBUCX_GetRootHub30PortsInfoUsingUCXIoctl(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_GettingRootHub30PortsInfo


HSM_EVENT
HUBHSM_GettingRootHubInfo(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_GettingRootHubInfo is called when the
    state machine enters the GettingRootHubInfo State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBUCX_GetControllerInfo(hubContext);

        HUBUCX_GetRootHubInfoUsingUCXIoctl(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_GettingRootHubInfo


HSM_EVENT
HUBHSM_ReturningConfigurationFailureForRootHub(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ReturningConfigurationFailureForRootHub is called when the
    state machine enters the ReturningConfigurationFailureForRootHub State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return HsmEventHubConfigurationFailure;

} // HUBHSM_ReturningConfigurationFailureForRootHub


HSM_EVENT
HUBHSM_ReturningConfigurationSuccessForRootHub(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ReturningConfigurationSuccessForRootHub is called when the
    state machine enters the ReturningConfigurationSuccessForRootHub State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return HsmEventHubConfigurationSuccess;

} // HUBHSM_ReturningConfigurationSuccessForRootHub


HSM_EVENT
HUBHSM_GettingAdditionalInfoFromParent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_GettingAdditionalInfoFromParent is called when the
    state machine enters the GettingAdditionalInfoFromParent State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBPARENT_GetInfoFromParentUsingParentIoctl(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_GettingAdditionalInfoFromParent


HSM_EVENT
HUBHSM_ReturningOperationFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ReturningOperationFailure is called when the
    state machine enters the ReturningOperationFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return HsmEventOperationFailure;

} // HUBHSM_ReturningOperationFailure


HSM_EVENT
HUBHSM_ReturningOperationSuccess(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ReturningOperationSuccess is called when the
    state machine enters the ReturningOperationSuccess State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return HsmEventOperationSuccess;

} // HUBHSM_ReturningOperationSuccess


HSM_EVENT
HUBHSM_GettingHubDescriptorToCheckForPowerLoss(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_GettingHubDescriptorToCheckForPowerLoss is called when the
    state machine enters the GettingHubDescriptorToCheckForPowerLoss State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_GetHubDescriptorUsingControlTransfer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_GettingHubDescriptorToCheckForPowerLoss


HSM_EVENT
HUBHSM_ReturningOperationFailureInCheckingForPowerLoss(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ReturningOperationFailureInCheckingForPowerLoss is called when the
    state machine enters the ReturningOperationFailureInCheckingForPowerLoss State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return HsmEventOperationFailure;

} // HUBHSM_ReturningOperationFailureInCheckingForPowerLoss


HSM_EVENT
HUBHSM_ReturningOperationSuccessInCheckingForPowerLoss(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ReturningOperationSuccessInCheckingForPowerLoss is called when the
    state machine enters the ReturningOperationSuccessInCheckingForPowerLoss State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return HsmEventOperationSuccess;

} // HUBHSM_ReturningOperationSuccessInCheckingForPowerLoss


HSM_EVENT
HUBHSM_ReturningOperationFailureInSettingHubDepth(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ReturningOperationFailureInSettingHubDepth is called when the
    state machine enters the ReturningOperationFailureInSettingHubDepth State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return HsmEventOperationFailure;

} // HUBHSM_ReturningOperationFailureInSettingHubDepth


HSM_EVENT
HUBHSM_ReturningOperationSuccessInSettingHubDepth(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_ReturningOperationSuccessInSettingHubDepth is called when the
    state machine enters the ReturningOperationSuccessInSettingHubDepth State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return HsmEventOperationSuccess;

} // HUBHSM_ReturningOperationSuccessInSettingHubDepth


HSM_EVENT
HUBHSM_SettingHubDepth(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBHSM_SettingHubDepth is called when the
    state machine enters the SettingHubDepth State

Arguments:

    SmContext - State Machine Context


Return Value:

    HSM_EVENT

--*/
{

    PHUB_FDO_CONTEXT                hubContext;
    
    TRY {

        hubContext = SmContext->ParentContext;
        HUBHTX_SetHubDepthUsingControlTransfer(hubContext);

    } FINALLY {

    }

    return HsmEventNull;

} // HUBHSM_SettingHubDepth





