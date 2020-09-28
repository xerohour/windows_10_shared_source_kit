/*++
Copyright (c) Microsoft Corporation

Module Name:

    crashdumpcommand.c

Abstract:

    USB XHCI Crashdump write module.

--*/

#include "pch.h"

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Crashdump_Command_Initialize(
    __inout
        PCRASHDUMP_COMMAND_DATA CommandData,
    __in
        HANDLE                  DumperHandle
    )
{
    NTSTATUS    status;

    TRY {

        CommandData->DumperHandle = DumperHandle;
        CommandData->RegisterHandle = Crashdump_GetRegisterHandle(DumperHandle);

        CommandData->CommandRingControl =
            Crashdump_Register_GetCommandRingControlRegister(CommandData->RegisterHandle);

        status = Crashdump_CommonBufferAcquire(CommandData->DumperHandle,
                                               CRASHDUMP_COMMANDRING_SIZE,
                                               &CommandData->CommandRingBuffer);

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Command_PrepareForDump(
    __in
        PCRASHDUMP_COMMAND_DATA CommandData
    )
/*++

Routine Description:

    This routine will stop the Command Ring if it is not already stopped,
    and install a Command Ring buffer comprising two TRBs. Since commands
    will be submitted sequentially, and because one Command Descriptor (CD)
    takes no more than one TRB, only one TRB is needed for command, and the
    other TRB is used as a Link TRB that will point back to the beginning
    of the Command Ring.

Reference:

    4.9.3       - Command Ring Management.

--*/
{
    NTSTATUS    status;

    TRY {

        RtlZeroMemory(CommandData->CommandRingBuffer.VirtualAddress,
                      CommandData->CommandRingBuffer.Size);

        CommandData->TRBArray = (PTRB)CommandData->CommandRingBuffer.VirtualAddress;
        CommandData->CycleState = 1;

        if (Crashdump_Command_IsRunning(CommandData)) {

            CrashdumpLogWarning("Command ring is not stopped, stopping now");

            status = Crashdump_Command_Stop(CommandData, FALSE);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        //
        // Create a two-element command ring. Element 0 will hold the command
        // TRB, Element 1 is the link TRB.
        //
        CommandData->TRBArray[1].Link.Type = TRB_TYPE_LINK;
        CommandData->TRBArray[1].Link.ToggleCycle = TRUE;
        CommandData->TRBArray[1].Link.RingSegmentPointer =
            CommandData->CommandRingBuffer.LogicalAddress.QuadPart;

        Crashdump_Command_InstallRing(CommandData);

        CrashdumpLogInfo("Installed new Command Ring");

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Command_TestCommandRingOperation(
    __in
        PCRASHDUMP_COMMAND_DATA CommandData
    )
/*++

Routine Description:

    This routine will verify the operation of Command and Event Rings
    by ensuring that a no-op command completes successfully.

Reference:

    4.6.2       - No Op.

--*/
{
    NTSTATUS    status;
    TRB         trb;

    CrashdumpLogInfo("Crashdump_Command_TestCommandRingOperation: begin");

    RtlZeroMemory(&trb, sizeof(TRB));

    trb.NoOpCommand.Type = TRB_TYPE_NO_OP_COMMAND;

    status = Crashdump_Command_SendCommand(CommandData, &trb, NULL);

    CrashdumpLogInfo("Crashdump_Command_TestCommandRingOperation: end 0x%X", status);

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Command_SendCommand(
    __in
        PCRASHDUMP_COMMAND_DATA         CommandData,
    __in
        PTRB                            CommandTrb,
    __out_opt
        PCOMMAND_COMPLETION_EVENT_TRB   CompletionEvent
    )
/*++

Routine Description:

    This routine will execute a single Command and ensure completion by
    polling on the Event Ring. Unlike the Transfer Ring, the Cycle bit
    in TRBs is utilized and toggled after executing every Command.

Reference:

    4.6.1       - Command Ring Operation.

--*/
{
    NTSTATUS    status;

    TRY {

        CrashdumpLogInfo("Crashdump_Command_SendCommand: begin");

        //
        // Make sure CycleBit in the source and target TRBs is invalid.
        //
        CommandTrb->NoOpCommand.CycleBit = 
            (CommandData->CycleState == 1) ? 0 : 1;
        RtlCopyMemory(&CommandData->TRBArray[0], CommandTrb, sizeof(TRB));
        
        CommandData->TRBArray[1].Link.CycleBit = CommandData->CycleState;

        //
        // Commit the CycleBit last.
        //
        Register_ToggleTrbCycleBit(&CommandData->TRBArray[0]);
        TOGGLE(CommandData->CycleState);

        Register_WriteUlong(
            &Crashdump_Register_GetDoorbellRegister(CommandData->RegisterHandle, 0)->AsUlong32,
            0);

        status = Crashdump_EventRing_Poll(Crashdump_GetEventRingHandle(CommandData->DumperHandle),
                                          TRB_TYPE_COMMAND_COMPLETION_EVENT,
                                          20000,
                                          CommandData,
                                          Crashdump_Command_EventRingCallback);

        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("Did not receive Event TRB, status 0x%X", status);
            LEAVE;
        }

        if (CommandData->LastCompletionEvent.CompletionCode != TRB_COMPLETION_CODE_SUCCESS) {
            CrashdumpLogError("Command failed, completion code: %u, slotId: %u",
                              CommandData->LastCompletionEvent.CompletionCode,
                              CommandData->LastCompletionEvent.SlotId);
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        CrashdumpLogInfo("Command Completion event: Completion code: %u, slotId: %u",
                         CommandData->LastCompletionEvent.CompletionCode,
                         CommandData->LastCompletionEvent.SlotId);

        if (CompletionEvent != NULL) {

            RtlCopyMemory(CompletionEvent,
                          &CommandData->LastCompletionEvent,
                          sizeof(COMMAND_COMPLETION_EVENT_TRB));
        }

    } FINALLY {

        CrashdumpLogInfo("Crashdump_Command_SendCommand: end 0x%X", status);
    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Command_EventRingCallback(
    __inout
        PCRASHDUMP_COMMAND_DATA CommandData,
    __in
        PTRB                    Trb,
    __out
        BOOLEAN                 *Continue
    )
/*++

Routine Description:

    When polling for events, the event ring will call this function for
    every Command Completion Event TRB. Ideally, only one such TRB should
    end up on the event ring since Commands are issued sequentially, but
    in the beginning, we may receive an command completion event from
    stopping of the Command Ring while the xHC was halted.

--*/
{
    if (Trb->CommandCompletionEvent.CommandPointer !=
        (ULONG64)CommandData->CommandRingBuffer.LogicalAddress.QuadPart) {

        *Continue = TRUE;
        return STATUS_SUCCESS;
    }

    RtlCopyMemory(&CommandData->LastCompletionEvent, Trb, sizeof(TRB));

    *Continue = FALSE;

    return STATUS_SUCCESS;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Command_Stop(
    __in
        PCRASHDUMP_COMMAND_DATA CommandData,
    __in
        BOOLEAN                 WaitForCompletion
    )
/*++

Routine Description:

    This routine will get the xHC to abort any currently scheduled Commands
    and stop the Command Ring. It will verify that the Command Ring is stopped
    by polling on the CommandRingRunning bit.
    If the caller needs it, this routine will clear the stop event from the
    event ring

Reference:

    5.4.5            - Command Ring Control Register (CRCR)

--*/
{
    NTSTATUS    status;
    ULONG       retryCount;
    CRCR        commandRingControl;

    TRY {

        CrashdumpLogInfo("Crashdump_Command_Stop: begin");

        commandRingControl.AsUlong64 =
            Register_ReadUlong64(&CommandData->CommandRingControl->AsUlong64,
                                 (Crashdump_Register_GetDeviceFlags(CommandData->RegisterHandle).Use32BitRegisterAccess == 1));

        commandRingControl.CommandAbort = 1;

        Register_WriteUlong64(&CommandData->CommandRingControl->AsUlong64,
                              commandRingControl.AsUlong64,
                              (Crashdump_Register_GetDeviceFlags(CommandData->RegisterHandle).Use32BitRegisterAccess == 1));

        //
        // Setting the CommandAbort bit is supposed to generate a Command
        // Completion Event, but since we may attempt to stop the Command
        // Ring while the xHC is halted, the event will not be generated
        // until the xHC is restarted. So poll on the CRR bit instead.
        //
        for (retryCount = 0; retryCount < CRASHDUMP_COMMANDRING_STOP_WAIT_COUNT; retryCount++) {

            KeStallExecutionProcessor(CRASHDUMP_COMMANDRING_STOP_WAIT_TIME);

            commandRingControl.AsUlong64 =
                Register_ReadUlong64(&CommandData->CommandRingControl->AsUlong64,
                                     (Crashdump_Register_GetDeviceFlags(CommandData->RegisterHandle).Use32BitRegisterAccess == 1));

            if (commandRingControl.CommandRingRunning == 0) {

                CrashdumpLogInfo("Command ring took %u us to stop",
                                 (retryCount + 1) * CRASHDUMP_COMMANDRING_STOP_WAIT_TIME);

                status = STATUS_SUCCESS;

                //
                // If the caller wants us to drain this event from the Event Ring
                // wait for it to show up
                //
                if (WaitForCompletion) {
                    
                    status = Crashdump_EventRing_Poll(Crashdump_GetEventRingHandle(CommandData->DumperHandle),
                                                      TRB_TYPE_COMMAND_COMPLETION_EVENT,
                                                      20000,
                                                      CommandData,
                                                      Crashdump_Command_EventRingCallback);
                    
                    if (!NT_SUCCESS(status)) {
                        CrashdumpLogError("Did not receive Event TRB, status 0x%X", status);
                        LEAVE;
                    }
                } 
                LEAVE;
            }
        }

        CrashdumpLogError("Command ring is not stopped after %u us",
                          (retryCount * CRASHDUMP_COMMANDRING_STOP_WAIT_TIME));

        status = STATUS_UNSUCCESSFUL;

    } FINALLY {

        CrashdumpLogInfo("Crashdump_Command_Stop: end 0x%X", status);
    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Crashdump_Command_IsRunning(
    __in
        PCRASHDUMP_COMMAND_DATA CommandData
    )
{
    CRCR    commandRingControl;

    commandRingControl.AsUlong64 =
        Register_ReadUlong64(&CommandData->CommandRingControl->AsUlong64,
                             (Crashdump_Register_GetDeviceFlags(CommandData->RegisterHandle).Use32BitRegisterAccess == 1));

    return (commandRingControl.CommandRingRunning != 0);
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Crashdump_Command_InstallRing(
    __in
        PCRASHDUMP_COMMAND_DATA CommandData
    )
{
    CRCR    commandRingControl;

    commandRingControl.AsUlong64 = CommandData->CommandRingBuffer.LogicalAddress.QuadPart;

    CommandData->CycleState = 1;
    commandRingControl.RingCycleState = 1;

    Register_WriteUlong64(&CommandData->CommandRingControl->AsUlong64,
                          commandRingControl.AsUlong64,
                          (Crashdump_Register_GetDeviceFlags(CommandData->RegisterHandle).Use32BitRegisterAccess == 1));
}

