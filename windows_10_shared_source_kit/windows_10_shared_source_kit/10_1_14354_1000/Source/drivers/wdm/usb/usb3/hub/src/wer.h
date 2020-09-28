/*++
Copyright (c) Microsoft Corporation

Module Name:

    wer.h

Abstract:

    WER Live Kernel Dump

--*/

#pragma once

#define WER_NO_SUBREASON 0

NTSTATUS
WER_CreateReport(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in
        PPORT_CONTEXT       PortContext,
    __in
        ULONG               BugCheckParam1,
    __in
        ULONG               BugCheckSubReason
    );
