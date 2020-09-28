/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    bus.h

Abstract:

   Contains declarations for SPB function and callbacks

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel-mode Driver Framework

--*/

#ifndef _BUS_H_
#define _BUS_H_

// Default timeout for a Request sent to a IO Target 1 second
const ULONGLONG HIDI2C_REQUEST_DEFAULT_TIMEOUT = 1;

//
// Public Functions
//

NTSTATUS
SpbInitialize(
    _In_ PDEVICE_CONTEXT    FxDeviceContext,
    _In_ WDFCMRESLIST       FxResourcesRaw,
    _In_ WDFCMRESLIST       FxResourcesTranslated
    );

VOID
SpbDestroy(
    _In_ PDEVICE_CONTEXT  FxDeviceContext
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
SpbWrite(
    _In_                    WDFIOTARGET FxIoTarget,
    _In_                    USHORT      RegisterAddress,
    _In_reads_(DataLength)  PBYTE       Data,
    _In_                    ULONG       DataLength,
    _In_                    ULONG       Timeout = HIDI2C_REQUEST_DEFAULT_TIMEOUT
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
SpbWritelessRead(
    _In_                        WDFIOTARGET FxIoTarget,
    _In_                        WDFREQUEST  FxRequest,
    _Out_writes_(DataLength)    PBYTE       Data,
    _In_                        ULONG       DataLength,
    _In_opt_                    LPGUID      ActivityId
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
SpbRead(
    _In_                        WDFIOTARGET FxIoTarget,
    _In_                        USHORT      RegisterAddress,
    _Out_writes_(DataLength)    PBYTE       Data,
    _In_                        ULONG       DataLength,
    _In_                        ULONG       DelayUs,
    _In_                        ULONG       Timeout = HIDI2C_REQUEST_DEFAULT_TIMEOUT
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
SpbWriteWrite(
    _In_                            WDFIOTARGET FxIoTarget,
    _In_                            USHORT      RegisterAddressFirst,
    _In_reads_(DataLengthFirst)     PBYTE       DataFirst,
    _In_                            USHORT      DataLengthFirst,
    _In_                            USHORT      RegisterAddressSecond,
    _In_reads_(DataLengthSecond)    PBYTE       DataSecond,
    _In_                            USHORT      DataLengthSecond,
    _In_                            ULONG       /* DelayUs */
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
SpbWriteRead(
    _In_                            WDFIOTARGET     FxIoTarget,
    _In_                            USHORT          RegisterAddressFirst,
    _In_reads_(DataLengthFirst)     PBYTE           DataFirst,
    _In_                            USHORT          DataLengthFirst,
    _In_                            USHORT          RegisterAddressSecond,
    _Out_writes_(DataLengthSecond)  PBYTE           DataSecond,
    _In_                            USHORT          DataLengthSecond,
    _In_                            ULONG           DelayUs
    );

//
// Private Functions
//

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
_SpbSequence(
    _In_                        WDFIOTARGET     FxIoTarget,
    _In_reads_(SequenceLength)  PVOID           Sequence,
    _In_                        SIZE_T          SequenceLength,
    _Out_                       PULONG          BytesReturned,
    _In_                        ULONG           Timeout = HIDI2C_REQUEST_DEFAULT_TIMEOUT
    );

#endif
