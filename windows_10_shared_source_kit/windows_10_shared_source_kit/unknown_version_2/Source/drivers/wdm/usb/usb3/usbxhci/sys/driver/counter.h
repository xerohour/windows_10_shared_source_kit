/*++
 *
Copyright (c) Microsoft Corporation

Module Name:

    counter.h

Abstract:

    Functions for exposing counters.

--*/

#pragma once

//
// Public interface.
//

VOID
__drv_requiresIRQL(PASSIVE_LEVEL)
Counter_Register();

VOID
__drv_requiresIRQL(PASSIVE_LEVEL)
Counter_Unregister();

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Counter_CreateInterrupterInstance(
    __in
        PINTERRUPTER_DATA   InterrupterData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Counter_CloseInterrupterInstance(
    __in
        PINTERRUPTER_DATA   InterrupterData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Counter_CreateCommonBufferInstance(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Counter_CloseCommonBufferInstance(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Counter_CreateTransferRingInstance(
    __in
        PENDPOINT_DATA         EndpointData,
    __in
        ULONG                  StreamId,
    __in
        PTRANSFERRING_COUNTER  TransferRingCounter
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Counter_CloseTransferRingInstance(
    __in
        PTRANSFERRING_COUNTER TransferRingCounter
    );
