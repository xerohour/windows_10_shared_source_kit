/*++
Copyright (c) Microsoft Corporation

Module Name:

    crashdumpcommand.h

Abstract:

    USB XHCI Crashdump write module.

--*/

#define CRASHDUMP_COMMANDRING_STOP_WAIT_COUNT   80      // 16ms
#define CRASHDUMP_COMMANDRING_STOP_WAIT_TIME    200     // 200us

//
// 5.4.5 - Command Ring is 64-byte aligned.
//
#define CRASHDUMP_COMMANDRING_SIZE              (2 * sizeof(TRB))

typedef struct _CRASHDUMP_COMMAND_DATA {

    HANDLE                          DumperHandle;
    HANDLE                          RegisterHandle;

    PCRCR                           CommandRingControl; 

    CRASHDUMP_BUFFER_DATA           CommandRingBuffer;
    PTRB                            TRBArray;

    ULONG                           CycleState;

    COMMAND_COMPLETION_EVENT_TRB    LastCompletionEvent;

} CRASHDUMP_COMMAND_DATA, *PCRASHDUMP_COMMAND_DATA;

//
// Public functions.
//
_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Crashdump_Command_Initialize(
    __inout
        PCRASHDUMP_COMMAND_DATA CommandData,
    __in
        HANDLE                  DumperHandle
    );

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Command_PrepareForDump(
    __in
        PCRASHDUMP_COMMAND_DATA CommandData
    );

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Command_TestCommandRingOperation(
    __in
        PCRASHDUMP_COMMAND_DATA CommandData
    );

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
    );

//
// Private functions.
//
_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Crashdump_Command_IsRunning(
    __in
        PCRASHDUMP_COMMAND_DATA CommandData
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Command_Stop(
    __in
        PCRASHDUMP_COMMAND_DATA CommandData,
    __in
        BOOLEAN                 WaitForCompletion
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Crashdump_Command_InstallRing(
    __in
        PCRASHDUMP_COMMAND_DATA CommandData
    );

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
    );

