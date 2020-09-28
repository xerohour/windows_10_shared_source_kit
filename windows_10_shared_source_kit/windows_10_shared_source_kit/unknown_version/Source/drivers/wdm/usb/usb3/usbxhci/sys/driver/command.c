/*++
Copyright (c) Microsoft Corporation

Module Name:

    command.c

Abstract:

    USB XHCI Driver

--*/

#include "pch.h"

#include "command.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, Command_Create)
#pragma alloc_text(PAGE, Command_CreateCommandTimeoutTimer)
#pragma alloc_text(PAGE, Command_PrepareHardware)
#pragma alloc_text(PAGE, Command_ReleaseHardware)
#endif

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Command_Create(
    __in
        WDFDEVICE   WdfDevice,
    __in
        HANDLE      ControllerHandle,
    __inout
        PHANDLE     ObjectHandle
    )
/*++

Routine Description:

    This function creates an commmand object and initializes the data
    associated with the command ring.

--*/
{
    NTSTATUS                status;
    WDF_OBJECT_ATTRIBUTES   wdfAttributes;
    WDFOBJECT               wdfCommand;
    PCOMMAND_DATA           commandData;

    UNREFERENCED_PARAMETER(WdfDevice);

    PAGED_CODE();

    TRY {

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, COMMAND_DATA);
        wdfAttributes.ParentObject = WdfDevice;

        status = WdfObjectCreate(&wdfAttributes, &wdfCommand);

        if (NT_SUCCESS(status)) {

            commandData = GetCommandData(wdfCommand);

            commandData->WdfCommand = wdfCommand;

            LogVerbose(Controller_GetRecorderLog(ControllerHandle), Command,
                       "wdfCommand 0x%p", wdfCommand);

            commandData->ControllerHandle = ControllerHandle;

            KeInitializeSpinLock(&commandData->CommandRingLock);

            commandData->ResetState = CommandResetState_NotInProgress;

            //
            // Return command data as the handle to the command object.
            //
            *ObjectHandle = (HANDLE)commandData;

            status = Command_CreateCommandTimeoutTimer(commandData);

        } else {

            LogError(Controller_GetRecorderLog(ControllerHandle), Command,
                     "WdfObjectCreate Failed %!STATUS!", status);
        }

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Command_CreateCommandTimeoutTimer(
    __in
        PCOMMAND_DATA   CommandData
    )
/*++

Routine Description:

    This routine creates the one-second Command Ring timeout timer.

--*/
{
    WDF_TIMER_CONFIG        timerConfig;
    WDF_OBJECT_ATTRIBUTES   timerAttributes;
    NTSTATUS                status;

    PAGED_CODE();

    TRY {

        WDF_TIMER_CONFIG_INIT_PERIODIC(&timerConfig,
                                       Command_WdfEvtTimerFunction,
                                       ONE_SECOND_IN_MS);

        timerConfig.TolerableDelay = ONE_HALF_SECOND_IN_MS;

        WDF_OBJECT_ATTRIBUTES_INIT(&timerAttributes);
        timerAttributes.ParentObject = CommandData->WdfCommand;

        status = WdfTimerCreate(&timerConfig,
                                &timerAttributes,
                                &CommandData->WdfTimer);

        if (!NT_SUCCESS(status)) {

            LogError(Controller_GetRecorderLog(CommandData->ControllerHandle), Command,
                     "WdfTimerCreate Failed %!STATUS!", status);
            LEAVE;
        }

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Command_PrepareHardware(
    __in
        PCOMMAND_DATA   CommandData
    )
/*++

Routine Description:

    This function is called on a prepare hardware event and allocates any
    resources required by the command ring.

--*/
{
    NTSTATUS                    status;
    HANDLE                      registerHandle;
    HANDLE                      commonBufferHandle;
    RECORDER_LOG_CREATE_PARAMS  recorderLogCreateParams;

    PAGED_CODE();

    TRY {

        registerHandle = Controller_GetRegisterHandle(CommandData->ControllerHandle);
        commonBufferHandle = Controller_GetCommonBufferHandle(CommandData->ControllerHandle);

        //
        // Acquire the common buffer for the command ring.
        //
        CommandData->CommandRingBufferData = CommonBuffer_AcquireBuffer(commonBufferHandle,
                                                                        SMALL_SEGMENT_SIZE,
                                                                        CommandData,
                                                                        COMMAND_ALLOC_TAG1);

        if (CommandData->CommandRingBufferData == NULL) {

            LogError(Controller_GetRecorderLog(CommandData->ControllerHandle), Command,
                     "CommonBuffer_AcquireBuffer returned NULL");

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        InitializeListHead(&CommandData->PendingList);
        InitializeListHead(&CommandData->WaitingList);

        CommandData->CommandRingControl = Register_GetCommandRingControlRegister(registerHandle);
        CommandData->DoorbellRegister   = Register_GetDoorbellRegister(registerHandle, 0);

        NT_ASSERT((CommandData->CommandRingBufferData->LogicalAddress.QuadPart & 0x3F) == FALSE);
        NT_ASSERT((SMALL_SEGMENT_SIZE % sizeof(TRB)) == 0);

        CommandData->MaxTrbIndex = (SMALL_SEGMENT_SIZE / sizeof(TRB)) - 1;
        CommandData->TrbArray    = (PTRB)CommandData->CommandRingBufferData->VirtualAddress;

        CommandData->LinkTrbPointer = (ULONG64)CommandData->CommandRingBufferData->LogicalAddress.QuadPart +
                                      CommandData->MaxTrbIndex * sizeof(TRB);

        LogInfo(Controller_GetRecorderLog(CommandData->ControllerHandle), Command,
                "MaxTrbIndex %d TrbArray 0x%p", CommandData->MaxTrbIndex, CommandData->TrbArray);

        //
        // Create a separate log for Command Ring. In case of failure to create the
        // log, fallback to using the controller's log. Only use this log after
        // PrepareHardware and before ReleaseHardware.
        //
        RECORDER_LOG_CREATE_PARAMS_INIT(&recorderLogCreateParams, NULL);
#if defined (DBG)
        recorderLogCreateParams.TotalBufferSize = 8192;
        recorderLogCreateParams.ErrorPartitionSize = 200;
#else
        recorderLogCreateParams.TotalBufferSize = 1024;
        recorderLogCreateParams.ErrorPartitionSize = 200;
#endif

        RtlStringCchPrintfA(recorderLogCreateParams.LogIdentifier,
                            sizeof(recorderLogCreateParams.LogIdentifier),
                            "%02d CMD",
                            Controller_GetInstanceNumber(CommandData->ControllerHandle));

        if (!NT_SUCCESS(WppRecorderLogCreate(&recorderLogCreateParams,
                                             &CommandData->RecorderLog))) {

            CommandData->RecorderLog = Controller_GetRecorderLog(CommandData->ControllerHandle);
        }

        CommandData->State = CommandState_Prepared;

        status = STATUS_SUCCESS;

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (CommandData->CommandRingBufferData != NULL) {

                CommonBuffer_ReleaseBuffer(commonBufferHandle, CommandData->CommandRingBufferData);
                CommandData->CommandRingBufferData = NULL;
            }
        }
    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Command_ReleaseHardware(
    __in
        PCOMMAND_DATA   CommandData
    )
/*++

Routine Description:

    This function is called on a release hardware event and frees any
    resources used by the command ring.

--*/
{
    HANDLE  commonBufferHandle;

    PAGED_CODE();

    TRY {

        commonBufferHandle = Controller_GetCommonBufferHandle(CommandData->ControllerHandle);

        if (CommandData->State == CommandState_Prepared) {

            Register_WriteUlong64(&CommandData->CommandRingControl->AsUlong64,
                                  0,
                                  (Controller_GetDeviceFlags(CommandData->ControllerHandle).Use32BitRegisterAccess == 1));

            if (IsListEmpty(&CommandData->WaitingList) == FALSE) {

                LogWarning(Controller_GetRecorderLog(CommandData->ControllerHandle), Command,
                           "WaitingList not empty");
                NT_ASSERTMSG("WaitingList not empty", 0);
            }

            if (IsListEmpty(&CommandData->PendingList) == FALSE) {

                LogWarning(Controller_GetRecorderLog(CommandData->ControllerHandle), Command,
                           "PendingList not empty");
                NT_ASSERTMSG("PendingList not empty", 0);
            }

            if (CommandData->CommandRingBufferData != NULL) {

                CommonBuffer_ReleaseBuffer(commonBufferHandle, CommandData->CommandRingBufferData);
                CommandData->CommandRingBufferData = NULL;
            }

            if ((CommandData->RecorderLog != NULL) &&
                (CommandData->RecorderLog != Controller_GetRecorderLog(CommandData->ControllerHandle))) {

                WppRecorderLogDelete(CommandData->RecorderLog);
                CommandData->RecorderLog = NULL;
            }
        }

        CommandData->CommandRingControl = NULL;
        CommandData->DoorbellRegister   = NULL;

        CommandData->State = CommandState_Released;

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Command_Initialize(
    __in
        PCOMMAND_DATA   CommandData
    )
/*++

Routine Description:

    This routine initializes (or re-initializes) the following XHCI host
    controller registers:

    5.4.5 Command Ring Control Register (CRCR)

    It also zero initializes the command ring buffer and initializes the
    command ring state variables.

    This routine is called during D0 entry and also during non-PnP reset
    recovery after the host controller has been reset.

    The host controller should be halted when this routine is called.

--*/
{
    CRCR    commandRingControl;
    TRB     localTrb;

    TRY {

        NT_ASSERT(IsListEmpty(&CommandData->PendingList));
        NT_ASSERT(IsListEmpty(&CommandData->WaitingList));

        if (Controller_IsControllerAccessible(CommandData->ControllerHandle) == FALSE) {
            LEAVE;
        }

        CommandData->EnqueueIndex = 0;
        CommandData->DequeueIndex = 0;
        CommandData->CycleState   = TRUE;

        //
        // Zero initialize the command ring buffer.
        //
        RtlZeroMemory(CommandData->CommandRingBufferData->VirtualAddress,
                      CommandData->CommandRingBufferData->Size);

        //
        // Be consistent with the general practice of fully initializing a local
        // copy of the TRB and then copying it to CommonBuffer in one-shot, to
        // reduce nummber of direct CommonBuffer accesses.
        //
        RtlZeroMemory(&localTrb, sizeof(TRB));

        localTrb.Link.Type               = TRB_TYPE_LINK;
        localTrb.Link.ToggleCycle        = 1;
        localTrb.Link.RingSegmentPointer = CommandData->CommandRingBufferData->LogicalAddress.QuadPart;

        RtlCopyMemory(&CommandData->TrbArray[CommandData->MaxTrbIndex],
                      &localTrb,
                      sizeof(TRB));

        //
        // Initialize (or re-initialize) the following XHCI host
        // controller registers:
        //
        // 5.4.5 Command Ring Control Register (CRCR)
        //
        commandRingControl.AsUlong64 = CommandData->CommandRingBufferData->LogicalAddress.QuadPart;
        commandRingControl.RingCycleState = CommandData->CycleState;

        Register_WriteUlong64(&CommandData->CommandRingControl->AsUlong64,
                              commandRingControl.AsUlong64,
                              (Controller_GetDeviceFlags(CommandData->ControllerHandle).Use32BitRegisterAccess == 1));

    } FINALLY {

    }

    return;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Command_D0Entry(
    __in
        PCOMMAND_DATA           CommandData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    )
/*++

Routine Description:

    This function is called when the controller enters D0.

    This routine initializes (or re-initializes) the following XHCI host
    controller registers:

    5.4.5 Command Ring Control Register (CRCR)

    It also zero initializes the command ring buffer and initializes
    the command ring state variables.

--*/
{
    TRY {

        LogInfo(CommandData->RecorderLog, Command,
                "CommandData 0x%p WdfPreviousState %d", CommandData, WdfPreviousState);

        Command_Initialize(CommandData);

    } FINALLY {

    }

    return STATUS_SUCCESS;
}

















































































































































































































































_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Command_D0EntryPostInterruptsEnabled(
    __in
        PCOMMAND_DATA           CommandData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    )
/*++

Routine Description:

    This function is called when the controller enters D0 after interrupters are enabled.

    It attempts to retrieve any information from the controller that requires sending
    an xHCI command, such as a vendor specific request.

--*/
{
    PINTERNAL_COMMAND_CONTEXT   commandContext;

    TRY {

        commandContext = NULL;







































































































































    } FINALLY {

        if (commandContext != NULL) {
            ExFreePoolWithTag(commandContext, DriverAllocationTag);
        }
    }

    return STATUS_SUCCESS;
}

VOID
Command_WdfEvtTimerFunction(
    __in
        WDFTIMER    WdfTimer
    )
/*++

Routine Description:

    This routine is the one-second Command Ring timeout time event
    callback.

    It decrements the timeout value of the current command which is
    pending on the Command Ring.  If the timeout value is decremented
    down to zero then the Command Ring Control Register (CRCR) Command
    Abort bit is set to abort the execution of the command.

    This routine also checks to see if appears that the host controller
    has been removed and if so all currently pending and waiting
    commands are immediately completed.

--*/
{
    PCOMMAND_DATA   commandData;
    KIRQL           irql;
    PCRB            crb;
    CRCR            commandRingControl;
    BOOLEAN         controllerAccessible;
    BOOLEAN         controllerNewlyGone;
    LIST_ENTRY      completionList;
    LIST_ENTRY      requeueList;
    PLIST_ENTRY     listEntry;
    BOOLEAN         ResetNeeded;
    BOOLEAN         InternalResetNeeded;

    TRY {

        commandData = GetCommandData(WdfTimerGetParentObject(WdfTimer));

        controllerAccessible = Controller_IsControllerAccessible(commandData->ControllerHandle);
        controllerNewlyGone = FALSE;
        InitializeListHead(&completionList);

        ResetNeeded = FALSE;
        InternalResetNeeded = FALSE;

        KeAcquireSpinLock(&commandData->CommandRingLock, &irql);

        if ((controllerAccessible != FALSE) &&
            (commandData->ResetState == CommandResetState_NotInProgress) &&
            (IsListEmpty(&commandData->PendingList) == FALSE)) {

            //
            // Check the timeout count down timer of the first command
            // on the pending command list.
            //
            listEntry = commandData->PendingList.Flink;

            crb = CONTAINING_RECORD(listEntry, CRB, ListEntry);

            commandRingControl.AsUlong64 = Register_ReadUlong64(
                &commandData->CommandRingControl->AsUlong64,
                (Controller_GetDeviceFlags(commandData->ControllerHandle).Use32BitRegisterAccess == 1));

            if (crb->Timeout > COMMAND_TIMER_SECOND_TIMEOUT_VALUE_SECONDS) {
                LogInfo(Controller_GetRecorderLog(commandData->ControllerHandle), Command,
                        "Crb 0x%p Timeout %d CRCR 0x%I64x",
                        crb, crb->Timeout, commandRingControl.AsUlong64);
            }

            //
            // Check to see if the controller appears to be gone.
            //
            if (commandRingControl.AsUlong64 == HARDWARE_GONE_ULONG64_REGISTER_VALUE) {

                controllerAccessible = FALSE;
                controllerNewlyGone = TRUE;

            } else if (commandData->AbortInProgress) {

                NT_ASSERT(commandData->AbortTimeout != 0);

                commandData->AbortTimeout -= 1;

                if (commandData->AbortTimeout == 0) {

                    if (commandRingControl.CommandRingRunning == 1) {

                        //
                        // Give up and trigger controller reset recovery at this point.
                        // Clear the AbortInProgress flag because we will start fresh
                        // after the reset recovery.
                        //
                        LogError(Controller_GetRecorderLog(commandData->ControllerHandle), Command,
                                 "HW_COMPLIANCE: Command abort timed out, command ring still running, resetting the controller");

                        Controller_HwVerifierBreakIfEnabled(
                            commandData->ControllerHandle,
                            NULL,
                            NULL,
                            ControllerHwVerifierCommandAbortTimeoutCommandRingNotStopped,
                            "Command abort timed out, command ring still running, resetting the controller",
                            &(commandData->TrbArray[commandData->DequeueIndex]),
                            NULL);

                        commandData->AbortInProgress = FALSE;

                        WdfTimerStop(commandData->WdfTimer, FALSE);

                        commandData->ResetState = CommandResetState_Pending;

                        ResetNeeded = TRUE;
                        if ((crb->CrbFlags & CRB_FLAG_INTERNAL_COMMAND) != 0) {
                            InternalResetNeeded = TRUE;
                        }

                    } else {

                        //
                        // In response to command abort, the controller is supposed to post
                        // a COMMAND_RING_STOPPED event when it stops the command ring. We
                        // have seen cases where a COMMAND_RING_STOPPED event is not posted,
                        // but the command ring is still stopped. If that is the case, consider
                        // the abort finished.
                        //
                        LogError(Controller_GetRecorderLog(commandData->ControllerHandle), Command,
                                 "HW_COMPLIANCE: Command abort timed out; stopped event not received, but command ring is stopped; Assuming abort finished.");

                        Controller_HwVerifierBreakIfEnabled(
                            commandData->ControllerHandle,
                            NULL,
                            NULL,
                            ControllerHwVerifierCommandAbortTimeoutCommandRingStopped,
                            "Command abort timed out; stopped event not received, but command ring is stopped; Assuming abort finished.",
                            &(commandData->TrbArray[commandData->DequeueIndex]),
                            NULL);

                        commandData->AbortInProgress = FALSE;

                        switch (crb->Timeout) {

                        case COMMAND_TIMER_INITIAL_VALUE_SECONDS:
                            //
                            // The command that was aborted has been completed. Now we are pointing to
                            // the next command.
                            //
                            LogInfo(Controller_GetRecorderLog(commandData->ControllerHandle), Command,
                                     "The command that was aborted has been completed (crb->Timeout = %d)",
                                     crb->Timeout);
                            crb = NULL;
                            break;

                        case COMMAND_TIMER_FIRST_TIMEOUT_VALUE_SECONDS:
                            //
                            // The command that was aborted has not been completed by the controller.
                            // We will re-program it and give the controller one more chance to execute
                            // it, in case the controller missed the doorbell ring the first time around.
                            //
                            LogError(Controller_GetRecorderLog(commandData->ControllerHandle), Command,
                                     "WARNING: The command that was aborted has not been completed by the controller after first timeout (crb->Timeout = %d), re-program it and give the controller one more chance to execute it",
                                     crb->Timeout);
                            crb = NULL;
                            break;

                        case COMMAND_TIMER_SECOND_TIMEOUT_VALUE_SECONDS:
                            //
                            // A command got timed out and aborted twice. But the controller did not
                            // finish executing it. Fail this command and move on to the next one, to
                            // avoid getting in a loop of executing and aborting the same command.
                            //
                            LogError(Controller_GetRecorderLog(commandData->ControllerHandle), Command,
                                     "ERROR: The command got timed out and aborted twice (crb->Timeout = %d)",
                                     crb->Timeout);

                            //
                            // Remove the aborted command from the pending list, to be completed after
                            // releasing the lock.
                            //
                            RemoveEntryList(&crb->ListEntry);

                            //
                            // Point the controller to the next command.
                            //
                            commandData->DequeueIndex += 1;
                            if (commandData->DequeueIndex == commandData->MaxTrbIndex) {
                                commandData->DequeueIndex = 0;
                            }

                            commandRingControl.AsUlong64 =
                                commandData->CommandRingBufferData->LogicalAddress.QuadPart +
                                commandData->DequeueIndex * sizeof(TRB);
                            commandRingControl.RingCycleState = commandData->CycleState;

                            Register_WriteUlong64(&commandData->CommandRingControl->AsUlong64,
                                                  commandRingControl.AsUlong64,
                                                  (Controller_GetDeviceFlags(commandData->ControllerHandle).Use32BitRegisterAccess == 1));
                            break;

                        default:
                            LogError(Controller_GetRecorderLog(commandData->ControllerHandle), Command,
                                     "ERROR_ASSERT: Unexpected timeout value for oldest command after abort finish (crb->Timeout = %d)",
                                     crb->Timeout);

                            NT_ASSERTMSG("Unexpected timeout value for oldest command after abort finish", FALSE);
                            break;
                        }

                        //
                        // If there are pending transfers, restart the command ring by
                        // ringing the doorbell.
                        //
                        if (IsListEmpty(&commandData->PendingList) == FALSE) {
                            LogInfo(commandData->RecorderLog, Command, "Command Ring restart due to pending transfers");
                            Register_WriteUlong(&commandData->DoorbellRegister->AsUlong32, 0);
                        }

                        //
                        // Requeue the commands that were on the waiting list. They could
                        // have been put on the waiting list for various reasons. So, if
                        // it is appropriate, the SendCommand function may put them back
                        // on the waiting list.
                        //
                        TRANSFER_LIST(&requeueList, &commandData->WaitingList);
                        while (IsListEmpty(&requeueList) == FALSE) {
                            Command_InternalSendCommand(commandData, (PCRB)RemoveHeadList(&requeueList));
                        }

                        //
                        // Complete the aborted command with timeout failure. Note that
                        // for some command types, the client object may invoke controller
                        // reset.
                        //
                        if (crb) {

                            KeReleaseSpinLock(&commandData->CommandRingLock, irql);
                            crb->CompletionCode = TRB_COMPLETION_CODE_COMMAND_ABORTED;
                            Etw_CommandCompleteError(NULL, commandData, crb, CrbCompletionTimeout);
                            (crb->CommandCallback)(crb, CrbCompletionTimeout, NULL);
                            KeAcquireSpinLock(&commandData->CommandRingLock, &irql);
                        }
                    }
                }

            } else {

                NT_ASSERT(crb->Timeout > COMMAND_TIMER_SECOND_TIMEOUT_VALUE_SECONDS);
                NT_ASSERT(crb->Timeout != 0);
                crb->Timeout--;

                if ((crb->Timeout == COMMAND_TIMER_FIRST_TIMEOUT_VALUE_SECONDS) ||
                    (crb->Timeout == COMMAND_TIMER_SECOND_TIMEOUT_VALUE_SECONDS)) {

                    //
                    // Check to see whether or not the command ring is
                    // currently running before attempting to abort the
                    // current command.
                    //
                    if (commandRingControl.CommandRingRunning == 1) {

                        //
                        // The command ring is currently running.
                        // Attempt to abort the current command.
                        //
                        LogError(Controller_GetRecorderLog(commandData->ControllerHandle), Command,
                                 "Command ring is running. Aborting Crb 0x%p %!TrbType!, Timeout = %d",
                                 crb, (TRB_TYPE)crb->Trb.NoOpCommand.Type, crb->Timeout);

                        //
                        // Address Device Command with BSR=0 goes out on the wire and may hand.
                        // Do not trigger hardware verifier break because it is not necessarily
                        // a controller issue.
                        //
                        if ((crb->Trb.AddressDeviceCommand.Type != TRB_TYPE_ADDRESS_DEVICE_COMMAND) ||
                            (crb->Trb.AddressDeviceCommand.BlockSetAddressRequest == 1)) {

                            LogError(Controller_GetRecorderLog(commandData->ControllerHandle), Command,
                                     "HW_COMPLIANCE: A command timed out, aborting command next (will use %d sec timeout). BSR = %d, Type = %!TrbType!",
                                     COMMAND_ABORT_TIMEOUT_SECONDS, crb->Trb.AddressDeviceCommand.BlockSetAddressRequest, (TRB_TYPE)crb->Trb.AddressDeviceCommand.Type);

                            Controller_HwVerifierBreakIfEnabled(
                                commandData->ControllerHandle,
                                NULL,
                                NULL,
                                ControllerHwVerifierCommandTimeout,
                                "A command timed out, aborting command next",
                                &crb->Trb,
                                NULL);
                        }

                        commandData->AbortInProgress = TRUE;
                        commandData->AbortTimeout = COMMAND_ABORT_TIMEOUT_SECONDS;

                        commandRingControl.CommandAbort = 1;

                        Register_WriteUlong64(&commandData->CommandRingControl->AsUlong64,
                                              commandRingControl.AsUlong64,
                                              (Controller_GetDeviceFlags(commandData->ControllerHandle).Use32BitRegisterAccess == 1));
                    } else {

                        //
                        // Why isn't the Command Ring currently running?
                        //
                        LogError(Controller_GetRecorderLog(commandData->ControllerHandle), Command,
                                 "ASSERT: Command Ring not running");

                        NT_ASSERTMSG("Command Ring not running", 0);

                        WdfTimerStop(commandData->WdfTimer, FALSE);

                        commandData->ResetState = CommandResetState_Pending;

                        ResetNeeded = TRUE;
                        if ((crb->CrbFlags & CRB_FLAG_INTERNAL_COMMAND) != 0) {
                            InternalResetNeeded = TRUE;
                        }
                    }
                }
            }
        }

        if (controllerAccessible == FALSE) {

            if (commandData->DrainCommandsFromTimerRoutine != FALSE) {

                //
                // Move all commands on the pending command list to the
                // completion list.
                //
                while (IsListEmpty(&commandData->PendingList) == FALSE) {

                    listEntry = RemoveHeadList(&commandData->PendingList);

                    InsertTailList(&completionList, listEntry);
                }

                //
                // Move all commands on the waiting command list to the
                // completion list.
                //
                while (IsListEmpty(&commandData->WaitingList) == FALSE) {

                    listEntry = RemoveHeadList(&commandData->WaitingList);

                    InsertTailList(&completionList, listEntry);
                }
            }

            //
            // The pending command list is now empty so stop the command
            // timeout timer.
            //
            WdfTimerStop(commandData->WdfTimer, FALSE);
        }

        KeReleaseSpinLock(&commandData->CommandRingLock, irql);

        if (controllerAccessible == FALSE) {

            if (controllerNewlyGone == TRUE) {
                Controller_SetControllerGone(commandData->ControllerHandle, TRUE);
            }

            //
            // Complete all of the commands that were moved to the
            // completion list.
            //
            while (IsListEmpty(&completionList) == FALSE) {

                listEntry = RemoveHeadList(&completionList);

                crb = CONTAINING_RECORD(listEntry, CRB, ListEntry);

                LogInfo(Controller_GetRecorderLog(commandData->ControllerHandle), Command,
                        "Controller gone - completing Crb 0x%p: %!TrbType!",
                        crb, (TRB_TYPE)crb->Trb.NoOpCommand.Type);

                Etw_CommandCompleteError(NULL, commandData, crb, CrbCompletionControllerGone);

                (crb->CommandCallback)(crb, CrbCompletionControllerGone, NULL);
            }
        }

        //
        // Initiate non-PnP error recovery if necessary.
        //
        if (ResetNeeded == TRUE) {
            Controller_ReportFatalError(commandData->ControllerHandle,
                                        (InternalResetNeeded == TRUE) ?
                                        Controller_FatalErrorInternalReset : Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_COMMAND_ABORT_FAILED,
                                        NULL,
                                        NULL,
                                        NULL);
        }

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Command_HandleCommandRingStoppedEvent(
    __in
        PCOMMAND_DATA   CommandData,
    __in
        PTRB            EventTrb
    )
/*++

Routine Description:

    This routine processes the Command Ring Stopped event which is expected
    to be delivered after software issues a command abort. It signals the
    end of the abort along with stopping of the command ring.

--*/
{
    CRCR        commandRingControl;
    PCRB        crb;
    KIRQL       irql;
    ULONG64     softwareDequeuePointer;
    ULONG64     hardwareDequeuePointer;
    LIST_ENTRY  requeueList;

    TRY {

        crb = NULL;

        KeAcquireSpinLock(&CommandData->CommandRingLock, &irql);

        CommandData->AbortInProgress = FALSE;

        softwareDequeuePointer =
            (ULONG64)CommandData->CommandRingBufferData->LogicalAddress.QuadPart +
            CommandData->DequeueIndex * sizeof(TRB);

        hardwareDequeuePointer = EventTrb->CommandCompletionEvent.CommandPointer;

        LogInfo(CommandData->RecorderLog, Command,
                "Command Ring Stopped - Dequeue Pointer : Software - 0x%I64x, Hardware - 0x%I64x",
                softwareDequeuePointer, hardwareDequeuePointer);

        //
        // The Command Completion Event TRB pointer does not point to the address of a
        // completed Command TRB. Instead it is the current value of the xHCI controller
        // Command Ring Dequeue Pointer at the point where the Command Ring execution
        // stopped. If the current command was aborted the Command Ring Dequeue Pointer
        // was advanced past the command which was aborted. It may point to the Link TRB,
        // in which case the software dequeue pointer value should be that of the first
        // TRB on command ring.
        //
        if ((hardwareDequeuePointer == softwareDequeuePointer) ||
            ((hardwareDequeuePointer == CommandData->LinkTrbPointer) &&
             (CommandData->DequeueIndex == 0))) {

            if (IsListEmpty(&CommandData->PendingList) == FALSE) {
 
                crb = CONTAINING_RECORD(CommandData->PendingList.Flink, CRB, ListEntry);

                switch (crb->Timeout) {

                case COMMAND_TIMER_INITIAL_VALUE_SECONDS:
                    //
                    // The command that was aborted has been completed. Now we are pointing to
                    // the next command.
                    //
                    crb = NULL;
                    break;

                case COMMAND_TIMER_FIRST_TIMEOUT_VALUE_SECONDS:
                    //
                    // The command that was aborted has not been completed by the controller.
                    // We will re-program it and give the controller one more chance to execute
                    // it, in case the controller missed the doorbell ring the first time around.
                    //
                    crb = NULL;
                    break;

                case COMMAND_TIMER_SECOND_TIMEOUT_VALUE_SECONDS:
                    //
                    // A command got timed out and aborted twice. But the controller did not
                    // finish executing it. Fail this command and move on to the next one, to
                    // avoid getting in a loop of executing and aborting the same command.
                    //

                    //
                    // Remove the aborted command from the pending list, to be completed after
                    // releasing the lock.
                    //
                    RemoveEntryList(&crb->ListEntry);

                    //
                    // Point the controller to the next command.
                    //
                    CommandData->DequeueIndex += 1;
                    if (CommandData->DequeueIndex == CommandData->MaxTrbIndex) {
                        CommandData->DequeueIndex = 0;
                    }

                    commandRingControl.AsUlong64 =
                        CommandData->CommandRingBufferData->LogicalAddress.QuadPart +
                        CommandData->DequeueIndex * sizeof(TRB);
                    commandRingControl.RingCycleState = CommandData->CycleState;

                    Register_WriteUlong64(&CommandData->CommandRingControl->AsUlong64,
                                          commandRingControl.AsUlong64,
                                          (Controller_GetDeviceFlags(CommandData->ControllerHandle).Use32BitRegisterAccess == 1));
                    break;

                default:
                    NT_ASSERTMSG("Unexpected timeout value for oldest command after abort finish", FALSE);
                    break;
                }
            }

            //
            // If there are pending transfers, restart the command ring by
            // ringing the doorbell.
            //
            if (IsListEmpty(&CommandData->PendingList) == FALSE) {
                LogInfo(CommandData->RecorderLog, Command, "Command Ring restart due to pending transfers");
                Register_WriteUlong(&CommandData->DoorbellRegister->AsUlong32, 0);
            }

            //
            // Requeue the commands that were on the waiting list. They could
            // have been put on the waiting list for various reasons. So, if
            // it is appropriate, the SendCommand function may put them back
            // on the waiting list.
            //
            TRANSFER_LIST(&requeueList, &CommandData->WaitingList);
            while (IsListEmpty(&requeueList) == FALSE) {
                Command_InternalSendCommand(CommandData, (PCRB)RemoveHeadList(&requeueList));
            }

        } else {

            LogInfo(CommandData->RecorderLog, Command,
                    "Dequeue Pointer mismatch after command abort completion");

            Controller_HwVerifierBreakIfEnabled(
                CommandData->ControllerHandle,
                NULL,
                NULL,
                ControllerHwVerifierDequeuePointerMismatchAfterCommandAbort,
                "After command abort completion, software and hardware dequeue pointers do not match",
                &(CommandData->TrbArray[CommandData->DequeueIndex]),
                EventTrb);

            Controller_ReportFatalError(CommandData->ControllerHandle,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_DEQUEUEPOINTER_MISMATCH_AFTER_COMMAND_ABORT,
                                        NULL,
                                        NULL,
                                        NULL);

            CommandData->ResetState = CommandResetState_Pending;
        }

    } FINALLY {

        KeReleaseSpinLock(&CommandData->CommandRingLock, irql);

        //
        // Complete the aborted command with timeout failure. Note that
        // for some command types, the client object may invoke controller
        // reset.
        //
        if (crb) {

            crb->CompletionCode = TRB_COMPLETION_CODE_COMMAND_ABORTED;
            Etw_CommandCompleteError(NULL, CommandData, crb, CrbCompletionTimeout);
            (crb->CommandCallback)(crb, CrbCompletionTimeout, NULL);
        }
    }

    return;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Command_HandleCommandCompletionEvent(
    __in
        PCOMMAND_DATA   CommandData,
    __in
        PTRB            EventTrb
    )
/*++

Routine Description:

    This function is called by the interrupter when a command completion
    event is received.

--*/
{
    KIRQL       irql;
    PCRB        Crb;
    PCRB        WaitingCrb;
    ULONG64     CrbAddress;
    LIST_ENTRY  CompletionList;
    PLIST_ENTRY listEntry;

    NT_ASSERT(EventTrb != NULL);

    TRY {

        InitializeListHead(&CompletionList);

        if (EventTrb->CommandCompletionEvent.CompletionCode == TRB_COMPLETION_CODE_COMMAND_RING_STOPPED) {
            Command_HandleCommandRingStoppedEvent(CommandData, EventTrb);
            LEAVE;
        }

        KeAcquireSpinLock(&CommandData->CommandRingLock, &irql);

        //
        // Go through the list of pending commands to find the one that completed.
        // Once it is found, mark it as pending completion. This is done to
        // guarantee that the commands are completed in order.
        //
        listEntry = CommandData->PendingList.Flink;

        while (listEntry != &CommandData->PendingList) {

            Crb = CONTAINING_RECORD(listEntry, CRB, ListEntry);

            CrbAddress = CommandData->CommandRingBufferData->LogicalAddress.QuadPart;
            CrbAddress += (Crb->Index * sizeof(TRB));

            LogVerbose(CommandData->RecorderLog, Command,
                       "Crb 0x%p CrbAddress 0x%I64x EventTrb.CommandPointer 0x%I64x",
                       Crb, CrbAddress, EventTrb->CommandCompletionEvent.CommandPointer);

            LogVerbose(CommandData->RecorderLog, Command,
                       "DequeueIndex %d EnqueueIndex %d", CommandData->DequeueIndex,
                       CommandData->EnqueueIndex);

            if (EventTrb->CommandCompletionEvent.CommandPointer == CrbAddress) {

                Crb->State = CrbState_PendingCompletion;

                Crb->CompletionCode = (UCHAR)EventTrb->CommandCompletionEvent.CompletionCode;

                Crb->CompletionParameter = (ULONG)EventTrb->CommandCompletionEvent.CompletionParameter;

                if (EventTrb->CommandCompletionEvent.Type == TRB_TYPE_COMMAND_COMPLETION_EVENT) {

                    Crb->SlotId = (UCHAR)EventTrb->CommandCompletionEvent.SlotId;
                }

                //
                // If requested the command completion event TRB
                // should be copied back to the TRB field of the CRB
                // prior to calling the command completion callback.
                //
                if ((Crb->CrbFlags & CRB_FLAG_RETURN_EVENT_TRB) != 0) {

                    RtlCopyMemory(&Crb->Trb, EventTrb, sizeof(*EventTrb));
                }

                break;
            }

            listEntry = listEntry->Flink;
        }

        //
        // Go through the list again, putting commands that are pending completion
        // on a completion list, which is then handled after the lock is released.
        // Note that the first Crb on the pending list must be marked for pending
        // completion, or else it will wait until further command completion events
        // are received. This is done to guarantee that completion of CRBs occurs
        // serially.
        //
        listEntry = CommandData->PendingList.Flink;

        while (listEntry != &CommandData->PendingList) {

            Crb = CONTAINING_RECORD(listEntry, CRB, ListEntry);

            if (Crb->State == CrbState_PendingCompletion) {

                NT_ASSERT(CommandData->DequeueIndex == Crb->Index);

                CommandData->DequeueIndex++;

                if (CommandData->DequeueIndex == CommandData->MaxTrbIndex) {
                    CommandData->DequeueIndex = 0;
                }

                LogVerbose(CommandData->RecorderLog, Command,
                           "DequeueIndex %d EnqueueIndex %d", CommandData->DequeueIndex,
                           CommandData->EnqueueIndex);

                LogInfo(CommandData->RecorderLog, Command,
                        "Completed Crb 0x%p found: %!TrbType! %!TrbCompletionCode! CycleBit %d SlotId %d",
                        Crb, (TRB_TYPE)Crb->Trb.NoOpCommand.Type,
                        (TRB_COMPLETION_CODE)EventTrb->CommandCompletionEvent.CompletionCode,
                        EventTrb->CommandCompletionEvent.CycleBit,
                        EventTrb->CommandCompletionEvent.SlotId);

                RemoveEntryList(&Crb->ListEntry);
                InsertTailList(&CompletionList, &Crb->ListEntry);

                if (IsListEmpty(&CommandData->WaitingList) == FALSE) {

                    WaitingCrb = (PCRB)RemoveHeadList(&CommandData->WaitingList);

                    Command_InternalSendCommand(CommandData, WaitingCrb);
                }

                listEntry = CommandData->PendingList.Flink;

            } else {

                //
                // If we are here, this is an out of order command completion. This is
                // not spec compliant. If this is the first time it has been observed
                // for the lifetime of this device object, then trigger a live dump.
                //
                if (IsListEmpty(&CompletionList) &&
                    (CommandData->OutOfOrderCompletionObserved == FALSE)) {

                    Controller_HwVerifierBreakIfEnabled(
                        CommandData->ControllerHandle,
                        NULL,
                        NULL,
                        ControllerHwVerifierOutOfOrderCommandCompletion,
                        "Controller completed a command out of order",
                        &(CommandData->TrbArray[CommandData->DequeueIndex]),
                        EventTrb);

                    Controller_ReportFatalError(CommandData->ControllerHandle,
                                                Controller_FatalErrorNoAction,
                                                USB3_WER_BUGCODE_USBXHCI_OUT_OF_ORDER_COMMAND_COMPLETION,
                                                NULL,
                                                NULL,
                                                NULL);

                    CommandData->OutOfOrderCompletionObserved = TRUE;
                }

                break;
            }
        }

        if (IsListEmpty(&CommandData->PendingList) != FALSE) {

            if (CommandData->AbortInProgress == FALSE) {

                //
                // The timer is used to timeout commands, or to timeout command abort operations.
                // Stop it when there are no more commands and there is no abort in progress.
                //
                LogVerbose(CommandData->RecorderLog, Command,
                           "PendingList empty - stopping timeout timer");

                WdfTimerStop(CommandData->WdfTimer, FALSE);

            } else {

                LogInfo(CommandData->RecorderLog, Command,
                        "No more pending commands. Abort in progress, waiting for "
                        "Command Ring Stopped event with timeout");
            }
        }

        KeReleaseSpinLock(&CommandData->CommandRingLock, irql);

        while (IsListEmpty(&CompletionList) == FALSE) {

            CRB_COMPLETION_STATUS   crbCompletionStatus;

            listEntry = RemoveHeadList(&CompletionList);

            Crb = CONTAINING_RECORD(listEntry, CRB, ListEntry);

            CommandFilter_FilterCommandCompletion(
                        Controller_GetCommandFilterHandle(CommandData->ControllerHandle),
                        Crb);

            switch (Crb->CompletionCode) {

            case TRB_COMPLETION_CODE_SUCCESS:
                crbCompletionStatus = CrbCompletionSuccess;
                break;

            case TRB_COMPLETION_CODE_COMMAND_ABORTED:
                LogInfo(CommandData->RecorderLog, Command,
                        "Crb 0x%p %!TrbType! aborted due to timeout",
                        Crb, (TRB_TYPE)Crb->Trb.NoOpCommand.Type);
                crbCompletionStatus = CrbCompletionTimeout;
                break;

            default:
                crbCompletionStatus = CrbCompletionUnsuccessful;
                break;
            }

            //
            // Depending on the completion code, this could be a new error of interest in the log.
            //
            if (Crb->CompletionCode == TRB_COMPLETION_CODE_SUCCESS) {

                Etw_CommandComplete(NULL, CommandData, Crb, crbCompletionStatus);

            } else {

                Etw_CommandCompleteError(NULL, CommandData, Crb, crbCompletionStatus);
            }

            (Crb->CommandCallback)(Crb, crbCompletionStatus, EventTrb);
        }

    } FINALLY {

    }

    return;
}

_Requires_lock_held_(CommandData->CommandRingLock)
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Command_InternalSendCommand(
    __in
        PCOMMAND_DATA   CommandData,
    __in
        PCRB            Crb
    )
/*++

Routine Description:

    The function inserts a command request into the command ring.

    NOTE: the command ring lock must be held when this function is called.

--*/
{
    ULONG   ringFullCompareIndex;

    NT_ASSERT(CommandData != NULL);
    NT_ASSERT(Crb != NULL);
    NT_ASSERT(Crb->CommandCallback != NULL);

    TRY {

        if ((CommandData->ResetState == CommandResetState_Pending) ||
            (CommandData->ResetState == CommandResetState_InProgress)) {

            //
            // If a controller reset is pending or in progress just
            // queue the command on the waiting list.
            //

            Etw_CommandWaitlisted(NULL, CommandData, Crb, ControllerResetInProgress);
            InsertTailList(&CommandData->WaitingList, &Crb->ListEntry);
            LEAVE;
        }

        //
        // Queueing commands while abort is in progress will incorrectly
        // ring the doorbell and set the command ring to running again,
        // thus confusing the abort logic.
        //
        if (CommandData->AbortInProgress) {

            LogInfo(CommandData->RecorderLog, Command,
                    "Abort in progress: queueing Crb 0x%p %!TrbType! ObjectHandle 0x%p to the WaitingList",
                    Crb, (TRB_TYPE)Crb->Trb.NoOpCommand.Type, Crb->ObjectHandle);

            Etw_CommandWaitlisted(NULL, CommandData, Crb, CommandAbortInProgress);
            InsertTailList(&CommandData->WaitingList, &Crb->ListEntry);
            LEAVE;
        }

        if (CommandData->ResetState == CommandResetState_PostReset) {

            //
            // TODO: PostWin8: Use new code other than CrbCompletionControllerGone,
            // or rename CrbCompletionControllerGone to be more generic?
            //
            (Crb->CommandCallback)(Crb, CrbCompletionControllerGone, NULL);
            LEAVE;
        }

        NT_ASSERT(CommandData->ResetState == CommandResetState_NotInProgress);

        //
        // If the flag is set, do not program more than one command at
        // a time. The next command on the waiting list will be programmed
        // when the current command completes.
        //
        if ((IsListEmpty(&CommandData->PendingList) == FALSE) &&
            Controller_GetDeviceFlags(CommandData->ControllerHandle).SerializeControllerCommands) {

            LogInfo(CommandData->RecorderLog, Command,
                    "Serialized command execution: queueing Crb 0x%p %!TrbType! ObjectHandle 0x%p to the WaitingList",
                    Crb, (TRB_TYPE)Crb->Trb.NoOpCommand.Type, Crb->ObjectHandle);

            Etw_CommandWaitlisted(NULL, CommandData, Crb, CommandsSerialized);
            InsertTailList(&CommandData->WaitingList, &Crb->ListEntry);
            LEAVE;
        }

        LogInfo(CommandData->RecorderLog, Command,
                "Sending Crb 0x%p: %!TrbType! ObjectHandle 0x%p",
                Crb, (TRB_TYPE)Crb->Trb.NoOpCommand.Type, Crb->ObjectHandle);

        //
        // This is the countdown timer that will timeout the command after 5 seconds.
        //
        Crb->Timeout = COMMAND_TIMER_INITIAL_VALUE_SECONDS;

        //
        // Check to see if the command ring is full. The ring is full if enqueue
        // index plus one equals the dequeue pointer. Make sure to take into
        // account when enqueue index value wraps (exceeds max trb index).
        //
        ringFullCompareIndex = CommandData->EnqueueIndex + 1;

        if (ringFullCompareIndex == CommandData->MaxTrbIndex) {
            ringFullCompareIndex = 0;
        }

        if (ringFullCompareIndex == CommandData->DequeueIndex) {

            //
            // The command ring is full.
            //
            LogVerbose(CommandData->RecorderLog, Command,
                       "Queue Crb 0x%p: EnqueueIndex %d DequeueIndex %d ringFullCompareIndex %d",
                       Crb, CommandData->EnqueueIndex, CommandData->DequeueIndex, ringFullCompareIndex);

            Crb->State = CrbState_Waiting;

            Etw_CommandWaitlisted(NULL, CommandData, Crb, CommandRingFull);
            InsertTailList(&CommandData->WaitingList, &Crb->ListEntry);

        } else {

            BOOLEAN pendingListWasEmpty;

            //
            // Insert command request into the command ring.
            //
            LogVerbose(CommandData->RecorderLog, Command,
                       "Insert Crb 0x%p: TrbArray[%d] 0x%p  DequeueIndex %d CycleState %d ringFullCompareIndex %d",
                       Crb, CommandData->EnqueueIndex,
                       &CommandData->TrbArray[CommandData->EnqueueIndex],
                       CommandData->DequeueIndex, CommandData->CycleState, ringFullCompareIndex);

            Etw_CommandSend(NULL, CommandData, Crb);

            //
            // Ensure that the CycleBit stays invalid until the target TRB is fully
            // formed.
            //
            Crb->Trb.NoOpCommand.CycleBit = (CommandData->CycleState == 1) ? 0 : 1;

            RtlCopyMemory(&CommandData->TrbArray[CommandData->EnqueueIndex],
                          &Crb->Trb,
                          sizeof(TRB));

            Register_ToggleTrbCycleBit(&CommandData->TrbArray[CommandData->EnqueueIndex]);

            Crb->State = CrbState_Pending;
            Crb->Index = CommandData->EnqueueIndex;

            pendingListWasEmpty = IsListEmpty(&CommandData->PendingList);

            InsertTailList(&CommandData->PendingList, &Crb->ListEntry);

            CommandData->EnqueueIndex++;

            //
            // The last trb in the command ring is a link trb that points to the
            // beginning of the command ring. Take this into account when determining
            // if we are at the end of the command ring.
            //
            if (CommandData->EnqueueIndex == CommandData->MaxTrbIndex) {

                Register_ToggleTrbCycleBit(&CommandData->TrbArray[CommandData->MaxTrbIndex]);

                CommandData->CycleState = (CommandData->CycleState == TRUE) ? FALSE : TRUE;

                LogVerbose(CommandData->RecorderLog, Command,
                           "Setting EnqueueIndex=0. EnqueueIndex %d MaxTrbIndex %d, CommandData->CycleState %d",
                           CommandData->EnqueueIndex, CommandData->MaxTrbIndex,
                           CommandData->CycleState);

                CommandData->EnqueueIndex = 0;
            }

            Register_WriteUlong(&CommandData->DoorbellRegister->AsUlong32, 0);

            if (pendingListWasEmpty != FALSE) {

                //
                // The pending command list was empty.  Now it is not
                // empty.  Start the command timeout timer.
                //
                LogVerbose(CommandData->RecorderLog, Command,
                           "PendingList not empty - starting timeout timer");

                WdfTimerStart(CommandData->WdfTimer, WDF_REL_TIMEOUT_IN_MS(ONE_SECOND_IN_MS));
            }
        }

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Command_SendCommand(
    __in
        PCOMMAND_DATA   CommandData,
    __in
        PCRB            Crb
    )
/*++

Routine Description:

    This function is called to send an xHCI command.

--*/
{
    KIRQL   irql;

    NT_ASSERT(CommandData != NULL);
    NT_ASSERT(Crb != NULL);
    NT_ASSERT(Crb->CommandCallback != NULL);

    TRY {

        //
        // Note: We don't currently log to ETW commands that the hardware never sees. Therefore this
        // function does not log its command completions.
        //
        if (Controller_IsControllerAccessible(CommandData->ControllerHandle) == FALSE) {

            //
            // If the controller is gone complete the command
            // immediately.
            //
            LogInfo(CommandData->RecorderLog, Command,
                    "Controller gone - completing Crb 0x%p: %!TrbType!",
                    Crb, (TRB_TYPE)Crb->Trb.NoOpCommand.Type);

            Crb->CompletionCode = TRB_COMPLETION_CODE_INVALID;
            (Crb->CommandCallback)(Crb, CrbCompletionControllerGone, NULL);

        } else {

            CRB_COMPLETION_STATUS   completionStatus;
            HANDLE                  commandFilterHandle;

            KeAcquireSpinLock(&CommandData->CommandRingLock, &irql);
            //
            // This needs to be done with the lock held to ensure that
            // the commands get filtered in the exact order that they
            // get programmed onto the controller
            //
            commandFilterHandle = Controller_GetCommandFilterHandle(CommandData->ControllerHandle);

            completionStatus = CommandFilter_FilterCommand(commandFilterHandle,
                                                           Crb);
            if (completionStatus != CrbCompletionNotComplete) {
                KeReleaseSpinLock(&CommandData->CommandRingLock, irql);
                (Crb->CommandCallback)(Crb, completionStatus, NULL);
                LEAVE;
            }

            if (CommandData->ResetState != CommandResetState_Failed) {

                Command_InternalSendCommand(CommandData, Crb);

                KeReleaseSpinLock(&CommandData->CommandRingLock, irql);

            } else {

                KeReleaseSpinLock(&CommandData->CommandRingLock, irql);

                //
                // TODO: PostWin8: Use new code other than CrbCompletionControllerGone,
                // or rename CrbCompletionControllerGone to be more generic?
                //
                (Crb->CommandCallback)(Crb, CrbCompletionControllerGone, NULL);
            }
        }

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Command_ControllerResetPreReset(
    __in
        PCOMMAND_DATA   CommandData
    )
/*++

Routine Description:

    This routine is called when the non-PnP controller reset process is
    in progress. The controller may still be running and the controller
    has not yet been reset.

    After this routine is called the DoorbellRegister will not be
    written again until after the controller reset process has
    completed.

    After this routine is called all new cammands sent through
    Command_SendCommand() will be queued on the WaitingList.

--*/
{
    KIRQL   irql;

    TRY {

        KeAcquireSpinLock(&CommandData->CommandRingLock, &irql);

        CommandData->ResetState = CommandResetState_InProgress;

        //
        // If reset was triggered while command abort was in progress,
        // consider the abort finished.
        //
        CommandData->AbortInProgress = FALSE;

        KeReleaseSpinLock(&CommandData->CommandRingLock, irql);

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Command_ControllerResetPostReset(
    __in
        PCOMMAND_DATA   CommandData
    )
/*++

Routine Description:

    This routine is called when the non-PnP controller reset process is
    in progress. The controller has been reset and the controller has
    not yet been started again.

    At this time all commands queued on the PendingList and the
    WaitingList will be completed with a failure status and the command
    ring will be re-initialized.

--*/
{
    KIRQL           irql;
    PCRB            crb;
    LIST_ENTRY      completionList;
    PLIST_ENTRY     listEntry;

    TRY {

        InitializeListHead(&completionList);

        KeAcquireSpinLock(&CommandData->CommandRingLock, &irql);

        //
        // Move all of the commands on the pending command list to the
        // local completion list.
        //
        while (IsListEmpty(&CommandData->PendingList) == FALSE) {

            listEntry = RemoveHeadList(&CommandData->PendingList);

            InsertTailList(&completionList, listEntry);
        }

        //
        // Move all of the commands on the waiting command list to the
        // local completion list.
        //
        while (IsListEmpty(&CommandData->WaitingList) == FALSE) {

            listEntry = RemoveHeadList(&CommandData->WaitingList);

            InsertTailList(&completionList, listEntry);
        }

        //
        // The pending command list is now empty so stop the command
        // timeout timer.
        //
        WdfTimerStop(CommandData->WdfTimer, FALSE);

        CommandData->ResetState = CommandResetState_PostReset;

        KeReleaseSpinLock(&CommandData->CommandRingLock, irql);

        //
        // Complete all of the commands that were moved to the local
        // completion list.
        //
        while (IsListEmpty(&completionList) == FALSE) {

            listEntry = RemoveHeadList(&completionList);

            crb = CONTAINING_RECORD(listEntry, CRB, ListEntry);

            LogInfo(CommandData->RecorderLog, Command,
                    "Completing Crb 0x%p: %!TrbType!",
                    crb, (TRB_TYPE)crb->Trb.NoOpCommand.Type);

            //
            // TODO: PostWin8: Use new code other than CrbCompletionControllerGone,
            // or rename CrbCompletionControllerGone to be more generic?
            //
            Etw_CommandCompleteError(NULL, CommandData, crb, CrbCompletionControllerGone);
            (crb->CommandCallback)(crb, CrbCompletionControllerGone, NULL);
        }

        //
        // Re-initialize the following XHCI host controller registers:
        //
        // 5.4.5 Command Ring Control Register (CRCR)
        //
        // Also zero initialize the command ring buffer and initialize
        // the command ring state variables.
        //
        Command_Initialize(CommandData);

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Command_ControllerResetPostResetSuccess(
    __in
        PCOMMAND_DATA   CommandData
    )
/*++

Routine Description:

    This routine is called when the non-PnP controller reset process is
    in progress. The controller has been reset and the controller has
    been successfully started again.

    Allow commands to be queued on the controller again.

--*/
{
    KIRQL           irql;
    LIST_ENTRY      waitingList;
    PLIST_ENTRY     listEntry;
    PCRB            crb;

    TRY {

        InitializeListHead(&waitingList);

        KeAcquireSpinLock(&CommandData->CommandRingLock, &irql);

        CommandData->ResetState = CommandResetState_NotInProgress;

        NT_ASSERT(IsListEmpty(&CommandData->PendingList));

        //
        // Move all of the commands on the waiting command list to the
        // local waiting list.
        //
        while (IsListEmpty(&CommandData->WaitingList) == FALSE) {

            listEntry = RemoveHeadList(&CommandData->WaitingList);

            InsertTailList(&waitingList, listEntry);
        }

        //
        // Requeue all of the commands that were moved to the local
        // waiting list.
        //
        while (IsListEmpty(&waitingList) == FALSE) {

            listEntry = RemoveHeadList(&waitingList);

            crb = CONTAINING_RECORD(listEntry, CRB, ListEntry);

            Command_InternalSendCommand(CommandData, crb);
        }

        KeReleaseSpinLock(&CommandData->CommandRingLock, irql);

    } FINALLY {

    }

    return;
}


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Command_FailAllCommands(
    __in
        PCOMMAND_DATA   CommandData
    )
/*++

Routine Description:

    This routine is called when the non-PnP controller reset process is
    in progress.  The controller reset or restart has fatally failed.
    
    This is also called if the controller is removed to fail all pending
    commands. 

    Immediately fail all commands.

--*/
{
    KIRQL           irql;
    LIST_ENTRY      completionList;
    PLIST_ENTRY     listEntry;
    PCRB            crb;

    TRY {

        InitializeListHead(&completionList);

        KeAcquireSpinLock(&CommandData->CommandRingLock, &irql);

        CommandData->ResetState = CommandResetState_Failed;

        APPEND_LIST(&completionList, &CommandData->PendingList);
        APPEND_LIST(&completionList, &CommandData->WaitingList);

        //
        // Command_SendCommand may end up queueing a command after the lock
        // is dropped here. For that case, let the timer routine finish the
        // outstanding commands, after this routine runs.
        //
        CommandData->DrainCommandsFromTimerRoutine = TRUE;

        KeReleaseSpinLock(&CommandData->CommandRingLock, irql);

        //
        // Complete all of the commands that were moved to the local
        // completion list.
        //
        while (IsListEmpty(&completionList) == FALSE) {

            listEntry = RemoveHeadList(&completionList);

            crb = CONTAINING_RECORD(listEntry, CRB, ListEntry);

            LogInfo(CommandData->RecorderLog, Command,
                    "Completing Crb 0x%p: %!TrbType!",
                    crb, (TRB_TYPE)crb->Trb.NoOpCommand.Type);

            //
            // TODO: PostWin8: Use new code other than CrbCompletionControllerGone,
            // for the Command_ControllerResetPostResetFailure case
            // or rename CrbCompletionControllerGone to be more generic?
            //
            Etw_CommandCompleteError(NULL, CommandData, crb, CrbCompletionControllerGone);
            (crb->CommandCallback)(crb, CrbCompletionControllerGone, NULL);
        }

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Command_ControllerGone(
    __in
        PCOMMAND_DATA   CommandData
    )
/*++

Routine Description:

    This routine is called when the controller is gone.
    
    Immediately fail all commands.

--*/
{
    Command_FailAllCommands(CommandData);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Command_ControllerResetPostResetFailure(
    __in
        PCOMMAND_DATA   CommandData
    )
/*++

Routine Description:

    This routine is called when the non-PnP controller reset process is
    in progress.  The controller reset or restart has fatally failed.
    
    Immediately fail all commands.

--*/
{
    Command_FailAllCommands(CommandData);
}

