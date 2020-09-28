/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name: 

    target.cpp

Abstract:

    This module contains the code for managing targets.

Environment:

    kernel-mode only

Revision History:

--*/

//
// Generate the function table when inporting scxdynamics.h thorugh internal.h
//

#include "internal.h"

#include "targetAPI.tmh"


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
SPBEXPORT(SpbTargetGetConnectionParameters)(
    __in PSPB_DRIVER_GLOBALS          DriverGlobals,
    __in  SPBTARGET                   Target,
    __out SPB_CONNECTION_PARAMETERS * ConnectionParameters
    )
{
    PCScxTarget target;

    //
    // Verify that we are at the correct IRQL
    //

    if (!NT_SUCCESS(ScxVerifyIrqlLessThanOrEqual(DISPATCH_LEVEL)))
    {
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Validate globals
    //

    if (!NT_SUCCESS(ScxVerifyPrivateGlobals(GetPrivateGlobals(DriverGlobals))))
    {
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Validate parameters
    //

    if (!NT_SUCCESS(ScxVerifyNotNull(Target)))
    {
        _Analysis_assume_(FALSE);
        goto exit;
    }

#pragma prefast(suppress:__WARNING_USING_UNINIT_VAR, "ConnectionParameters is initialized by caller")
    if (!NT_SUCCESS(ScxVerifyNotNull(ConnectionParameters)))
    {
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Validate the size of the connection parameters
    // the caller gave us
    //

    if (ConnectionParameters->Size != sizeof(SPB_CONNECTION_PARAMETERS))
    {
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "ConnectionParameters %p Size %d incorrect, expected %d", 
            ConnectionParameters,
            ConnectionParameters->Size,
            sizeof(SPB_CONNECTION_PARAMETERS)
            );
        WdfVerifierDbgBreakPoint();
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Validate that we were handed an actual target object.
    //

    target = GetTargetFromFxFile((WDFFILEOBJECT) Target);

    if (target == NULL)
    {
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "Target %p is not a real SPB target object",
            Target
            );
        WdfVerifierDbgBreakPoint();
        _Analysis_assume_(FALSE);
        goto exit;
    }

    //
    // Get the connection parameters
    //

    target->GetConnectionParameters(ConnectionParameters);

exit:

    return ;
}

__drv_maxIRQL(DISPATCH_LEVEL)
WDFFILEOBJECT
SPBEXPORT(SpbTargetGetFileObject)(
    __in PSPB_DRIVER_GLOBALS DriverGlobals,
    __in SPBTARGET           Target
    )
{
    PCScxTarget target;
    WDFFILEOBJECT fileObject = NULL;

    //
    // Verify that we are at the correct IRQL
    //

    if (!NT_SUCCESS(ScxVerifyIrqlLessThanOrEqual(DISPATCH_LEVEL)))
    {
        goto exit;
    }

    //
    // Validate globals
    //

    if (!NT_SUCCESS(ScxVerifyPrivateGlobals(GetPrivateGlobals(DriverGlobals))))
    {
        goto exit;
    }

    //
    // Validate parameters
    //

    if (!NT_SUCCESS(ScxVerifyNotNull(Target)))
    {
        goto exit;
    }

    //
    // Validate that we were handed an actual target object.
    //

    target = GetTargetFromFxFile(Target);

    if (target == NULL)
    {
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_APIERROR,
            "Target %p is not a real SPB target object",
            Target
            );
        WdfVerifierDbgBreakPoint();
        goto exit;
    }

    //
    // Return the corresponding FX file object.
    //

    fileObject = target->GetFxObject();

exit:

    return fileObject;
}

