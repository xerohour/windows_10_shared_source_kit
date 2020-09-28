/*++
Copyright (c) Microsoft Corporation

Module Name:

    telemetry.h

Abstract:

    Telemetry Library for creating WER live dumps

Author:

    Graham McIntyre (grahamm), 27-Mar-2011

Revision History:

--*/

#pragma once


//
// Prototypes
//

VOID
TelemetryData_CloseHandle(
    __in
        HANDLE  TelemetryHandle
    );

HANDLE
TelemetryData_CreateReport(
    __in
        PWCHAR      ReportType,
    __in
        ULONG       BugCheckCode,
    __in
        ULONG_PTR   BugCheckParam1,
    __in
        ULONG_PTR   BugCheckParam2,
    __in
        ULONG_PTR   BugCheckParam3,
    __in
        ULONG_PTR   BugCheckParam4
    );

NTSTATUS
TelemetryData_SetSecondaryData(
    __in
        HANDLE  TelemetryHandle,
    __in
        LPCGUID SecondaryDataGuid,
    __in
        ULONG   SecondaryDataSize,
    __in_bcount(SecondaryDataSize)
        PVOID   SecondaryDataBuffer
    );

NTSTATUS
TelemetryData_SubmitReport(
    __in
        HANDLE  TelemetryHandle
    );

NTSTATUS
TelemetryData_InsertTriageDataBlock (
    __in 
        HANDLE                  TelemetryHandle,
    __in_bcount(TriageDataSize)
        PVOID                   TriageData,
    __in
        ULONG                   TriageDataSize
    );
