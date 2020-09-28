/*++

Copyright (c) Microsoft Corporation

Module Name:

    verifier.cpp

Abstract:

    This module contains the private verifier function definitions
    for the SPB class extension.

Environment:

    kernel mode only

Revision History:


--*/

#include "internal.h"

#include "verifier.tmh"

NTSTATUS
ScxVerifyNotNull(
    _In_ PVOID Parameter
    )
{
    NTSTATUS status = STATUS_SUCCESS;

    if (Parameter == NULL)
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "NULL required parameter passed to a SpbCx DDI - %!STATUS!",
            status);
        WdfVerifierDbgBreakPoint();
    }

    return status;
}

NTSTATUS
ScxVerifyIrqlLessThanOrEqual(
    _In_ KIRQL ExpectedIrql
    )
{
    KIRQL currentIrql = KeGetCurrentIrql();
    NTSTATUS status = STATUS_SUCCESS;
    
    if (currentIrql > ExpectedIrql)
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "Called at wrong IRQL, should be less than or equal to %d - "
            "%!STATUS!",
            ExpectedIrql,
            status
            );
        WdfVerifierDbgBreakPoint();
    }
    
    return status;
}

NTSTATUS
ScxVerifyIrqlEquals(
    _In_ KIRQL ExpectedIrql
    )
{
    KIRQL currentIrql = KeGetCurrentIrql();
    NTSTATUS status = STATUS_SUCCESS;
    
    if (currentIrql != ExpectedIrql)
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "Called at wrong IRQL, should be %d - %!STATUS!",
            ExpectedIrql,
            status
            );
        WdfVerifierDbgBreakPoint();
    }
    
    return status;
}

NTSTATUS
ScxVerifyPrivateGlobals(
    _In_ PSCX_CLIENT_GLOBALS  Globals
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    
    if ((Globals == NULL) ||
        (Globals->Signature != SCX_GLOBALS_SIG))
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "Private globals %p are invalid - %!STATUS!",
            Globals,
            status
            );
        WdfVerifierDbgBreakPoint();
    }
    
    return status;
}