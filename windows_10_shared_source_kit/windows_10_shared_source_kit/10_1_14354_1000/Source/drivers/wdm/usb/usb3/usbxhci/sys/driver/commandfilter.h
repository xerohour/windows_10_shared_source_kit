/*++
Copyright (c) Microsoft Corporation

Module Name:

    commandfilter.h

Abstract:

    USB XHCI Driver    

--*/

#pragma once

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
CRB_COMPLETION_STATUS
(FN_FILTER_COMMAND)(
    __in
        HANDLE                  FilterHandle,
    __inout
        PCRB                    Crb
    );

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(FN_FILTER_COMMAND_COMPLETION)(
    __in
        HANDLE                  FilterHandle,
    __inout
        PCRB                    Crb
    );

typedef FN_FILTER_COMMAND *PFN_FILTER_COMMAND;
typedef FN_FILTER_COMMAND_COMPLETION *PFN_FILTER_COMMAND_COMPLETION;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(FN_FILTER_POST_RESET)(
    __in
        HANDLE                  FilterHandle
    );

typedef FN_FILTER_POST_RESET *PFN_FILTER_POST_RESET;

typedef struct _COMMANDFILTER_DATA_HEADER {

    WDFOBJECT                       WdfCommandFilter;

    HANDLE                          ControllerHandle;

    PFN_FILTER_COMMAND              FilterCommand;
    PFN_FILTER_COMMAND_COMPLETION   FilterCommandCompletion;
    PFN_FILTER_POST_RESET           FilterPostReset;

} COMMANDFILTER_DATA_HEADER, *PCOMMANDFILTER_DATA_HEADER;


//
// Public functions.
//
_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
CommandFilter_Create(
    __in
        WDFDEVICE   WdfDevice,
    __in
        HANDLE      ControllerHandle,
    __inout
        PHANDLE     ObjectHandle
    );

__drv_maxIRQL(DISPATCH_LEVEL)
CRB_COMPLETION_STATUS
FORCEINLINE
CommandFilter_FilterCommand(
    __in
        PCOMMANDFILTER_DATA_HEADER  CommandFilterDataHeader,
    __inout
        PCRB                        Crb
    ) 
{
    CRB_COMPLETION_STATUS   result;

    if ((CommandFilterDataHeader == NULL) ||
        (CommandFilterDataHeader->FilterCommand == NULL)) {
        return CrbCompletionNotComplete;
    }

    result = CommandFilterDataHeader->FilterCommand(CommandFilterDataHeader,
                                                    Crb);

    return result;

}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
FORCEINLINE
CommandFilter_FilterCommandCompletion(
    __in
        PCOMMANDFILTER_DATA_HEADER      CommandFilterDataHeader,
    __inout
        PCRB                            Crb
    )
{
    if ((CommandFilterDataHeader == NULL) ||
        (CommandFilterDataHeader->FilterCommandCompletion == NULL)) {
        return;
    }

    CommandFilterDataHeader->FilterCommandCompletion(CommandFilterDataHeader,
                                                     Crb);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
FORCEINLINE
CommandFilter_ControllerResetPostReset(
    __in
        PCOMMANDFILTER_DATA_HEADER      CommandFilterDataHeader
    )
{
    if ((CommandFilterDataHeader == NULL) ||
        (CommandFilterDataHeader->FilterPostReset == NULL)){
        return;
    }

    CommandFilterDataHeader->FilterPostReset(CommandFilterDataHeader);
}


