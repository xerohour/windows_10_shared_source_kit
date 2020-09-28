/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    interface.h

Abstract:

    This module contains APIs and routines for handling device interfaces.

Author:

    Santosh Jodh (santoshj) 08-02-2004

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

typedef struct _PENDING_SET_INTERFACE_STATE {
    LIST_ENTRY      List;
    UNICODE_STRING  LinkName;
} PENDING_SET_INTERFACE_STATE, *PPENDING_SET_INTERFACE_STATE;

//
// Location of group policy key to prevent execute access on a storage
// stack.  This is checked when bringing up a device node and if set
// a flag is applied to the PDO that tells the I/O manager that this
// device does not allow execution (MM will fail any section creates
// for executables on the device).  We do *not* check when the
// interface is disk as volume manager will take care of this.
//
#define REG_DENY_EXECUTE TEXT("\\Registry\\Machine\\System\\CurrentControlSet\\Control\\Storage")
#define REG_DISK_KEY TEXT("{53f5630d-b6bf-11d0-94f2-00a0c91efb8b}")

NTSTATUS
IopRegisterDeviceInterface(
    __in PCWSTR DeviceInstanceName,
    __in CONST GUID *InterfaceClassGuid,
    __in_opt PCWSTR ReferenceString,
    __in BOOLEAN UserModeFormat,
    __deref_out PWSTR *SymbolicLinkName,
    __out_opt PULONG PdoFlags
    );

NTSTATUS
PnpDisableDeviceInterfaces(
    __in PUNICODE_STRING DeviceInstancePath
    );

NTSTATUS
PnpDeleteDeviceInterfaces(
    __in PUNICODE_STRING DeviceInstancePath
    );

NTSTATUS
IopGetDeviceInterfaces(
    __in CONST GUID *InterfaceClassGuid,
    __in_opt PUNICODE_STRING DevicePath,
    __in ULONG Flags,
    __in BOOLEAN UserModeFormat,
    __deref_out_bcount(*SymbolicLinkListSize) PWSTR *SymbolicLinkList,
    __out_opt PULONG SymbolicLinkListSize
    );

NTSTATUS
IopDoDeferredSetInterfaceState(
    __in PDEVICE_NODE DeviceNode
    );
