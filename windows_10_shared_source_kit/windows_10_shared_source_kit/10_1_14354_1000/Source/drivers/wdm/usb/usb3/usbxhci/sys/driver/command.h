/*++
Copyright (c) Microsoft Corporation

Module Name:

    command.h

Abstract:

    USB XHCI Driver

--*/

#pragma once
















#define COMMAND_ABORT_TIMEOUT_SECONDS                       5

#define COMMAND_TIMER_INITIAL_VALUE_SECONDS                 10
#define COMMAND_TIMER_FIRST_TIMEOUT_VALUE_SECONDS           5
#define COMMAND_TIMER_SECOND_TIMEOUT_VALUE_SECONDS          0































































//
// Public definitions.
//
#define ONE_SECOND_IN_MS        1000
#define ONE_HALF_SECOND_IN_MS    500

typedef struct _CRB *PCRB;

typedef enum _CRB_COMPLETION_STATUS {
    CrbCompletionSuccess = 0,
    CrbCompletionUnsuccessful,
    CrbCompletionTimeout,
    CrbCompletionControllerGone,
    CrbCompletionNotComplete
} CRB_COMPLETION_STATUS;

typedef enum _COMMAND_WAITLIST_REASON {
    CommandRingFull           = 1,
    CommandsSerialized        = 2,
    CommandAbortInProgress    = 3,
    ControllerResetInProgress = 4
} COMMAND_WAITLIST_REASON;

//
// Command Callback function pointer definitions.
//      Parameter EventTRB is used for logging 
//      Hardware verifier events
//
typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(COMMAND_CALLBACK) (
    __in PCRB                   Crb,
    __in CRB_COMPLETION_STATUS  CrbCompletionStatus,
    __in_opt PTRB               EventTRB
    );

typedef COMMAND_CALLBACK *PCOMMAND_CALLBACK;

typedef enum _CrbStates {
    CrbState_Waiting = 0,
    CrbState_Pending,
    CrbState_PendingCompletion
} CrbStates;


//
// This flag indicates whether or not the command is internally issued
// prior to completing the initial entry into D0.  If the command is
// internally issued prior to completing the initial entry into D0 then
// special case handling is needed if the command fails to complete
// before it is timed out.
//
#define CRB_FLAG_INTERNAL_COMMAND   0x0001

//
// This flag indicates that the command completion event TRB should be
// copied back to the TRB field of the CRB prior to calling the command
// completion callback.
//
#define CRB_FLAG_RETURN_EVENT_TRB   0x0002

typedef struct _CRB {

    LIST_ENTRY                  ListEntry;

    CrbStates                   State;

    ULONG                       Index;

    TRB                         Trb;

    PVOID                       InputContextVA;

    PCOMMAND_CALLBACK           CommandCallback;

    HANDLE                      ObjectHandle;

    LONG                        Timeout;

    UCHAR                       CompletionCode;

    UCHAR                       SlotId;

    USHORT                      CrbFlags;

    ULONG                       CompletionParameter;

} CRB;

typedef enum _CommandStates {
    CommandState_Uninitialized = 0,
    CommandState_Prepared,
    CommandState_Released
} CommandStates;

typedef enum _CommandResetStates {
    CommandResetState_Uninitialized = 0,
    CommandResetState_NotInProgress,
    CommandResetState_Pending,
    CommandResetState_InProgress,
    CommandResetState_PostReset,
    CommandResetState_Failed
} CommandResetStates;

typedef struct _COMMAND_DATA {

    WDFOBJECT                   WdfCommand;

    HANDLE                      ControllerHandle;

    RECORDER_LOG                RecorderLog;

    WDFTIMER                    WdfTimer;

    CommandStates               State;

    CommandResetStates          ResetState;

    PCRCR                       CommandRingControl;

    PDOORBELL_REGISTER          DoorbellRegister;

    ULONG64                     LinkTrbPointer;
    ULONG                       MaxTrbIndex;

    ULONG                       EnqueueIndex;
    ULONG                       DequeueIndex;

    ULONG                       CycleState;

    PTRB                        TrbArray;

    LIST_ENTRY                  PendingList;

    LIST_ENTRY                  WaitingList;

    PBUFFER_DATA                CommandRingBufferData;

    KSPIN_LOCK                  CommandRingLock;

    BOOLEAN                     OutOfOrderCompletionObserved;

    BOOLEAN                     AbortInProgress;
    ULONG                       AbortTimeout;

    BOOLEAN                     DrainCommandsFromTimerRoutine;

} COMMAND_DATA, *PCOMMAND_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(COMMAND_DATA, GetCommandData)

typedef struct _INTERNAL_COMMAND_CONTEXT {

    PCOMMAND_DATA               CommandData;

    CRB                         Crb;

    KEVENT                      CompletionEvent;

} INTERNAL_COMMAND_CONTEXT, *PINTERNAL_COMMAND_CONTEXT;

//
// Public functions.
//
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
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Command_PrepareHardware(
    __in
        PCOMMAND_DATA   CommandData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Command_ReleaseHardware(
    __in
        PCOMMAND_DATA   CommandData
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Command_D0Entry(
    __in
        PCOMMAND_DATA           CommandData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Command_D0EntryPostInterruptsEnabled(
    __in
        PCOMMAND_DATA           CommandData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Command_SendCommand(
    __in
        PCOMMAND_DATA   CommandData,
    __in
        PCRB            Crb
    );

//
// Private functions.
//
_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Command_CreateCommandTimeoutTimer(
    __in
        PCOMMAND_DATA   CommandData
    );

EVT_WDF_TIMER Command_WdfEvtTimerFunction;

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Command_HandleCommandCompletionEvent(
    __in
        PCOMMAND_DATA   CommandData,
    __in
        PTRB            Trb
    );

_Requires_lock_held_(CommandData->CommandRingLock)
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Command_InternalSendCommand(
    __in
        PCOMMAND_DATA   CommandData,
    __in
        PCRB            Crb
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Command_ControllerResetPreReset(
    __in
        PCOMMAND_DATA   CommandData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Command_ControllerResetPostReset(
    __in
        PCOMMAND_DATA   CommandData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Command_ControllerResetPostResetSuccess(
    __in
        PCOMMAND_DATA   CommandData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Command_ControllerResetPostResetFailure(
    __in
        PCOMMAND_DATA   CommandData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Command_ControllerGone(
    __in
        PCOMMAND_DATA   CommandData
    );
