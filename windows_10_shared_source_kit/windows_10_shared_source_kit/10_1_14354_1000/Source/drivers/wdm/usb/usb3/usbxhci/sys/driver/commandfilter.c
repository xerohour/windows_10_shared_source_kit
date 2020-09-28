
/*++
Copyright (c) Microsoft Corporation

Module Name:

    commandfilter.c

Abstract:

    USB XHCI Driver

    Contains workaround routines needed to deal with controller issues related to
    XHCI commands.

--*/

#include "pch.h"

#include "commandfilter.tmh"
#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, CommandFilter_Create)
#endif


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
    )
/*++

Routine Description:

    This function creates a commmand filter object and initializes the data
    associated with the command filter.

--*/
{
    PAGED_CODE();










    *ObjectHandle = NULL;
    return STATUS_SUCCESS;
}


