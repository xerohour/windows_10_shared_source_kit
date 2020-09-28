/*++

Copyright (c) Microsoft Corporation

Module Name:

    hsmmux.c

Abstract:

    Support functions for HSM's "middle layer" which multiplexes all of the
    events to and from the different DSMs and PSMs.

Author:



Environment:

    Kernel mode only.

--*/


#pragma once


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HSMMUX_InitializeHSMMuxContext (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBMUX_PropogateDeviceProgrammingLostFlagsToAllDSMs(
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    );

VOID
HUBMUX_QueueSurpriseRemoveToAllSMs(
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    );


__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBMUX_CheckAndQueueIfResetHubIsEnabled (
    __in
        PHUB_FDO_CONTEXT            HubFdoContext
    );
