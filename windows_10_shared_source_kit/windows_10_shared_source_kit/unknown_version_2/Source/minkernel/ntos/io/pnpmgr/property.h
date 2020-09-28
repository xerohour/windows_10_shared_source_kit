/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    property.h

Abstract:

    This module contains custom property routines.

Author:

    Santosh Jodh (santoshj) February 04, 2005


Revision History:


--*/

#pragma once

NTSTATUS
PnpSetDevicePropertyData (
    __in PDEVICE_OBJECT     Pdo,
    __in CONST DEVPROPKEY   *PropertyKey,
    __in LCID               Lcid,
    __in ULONG              Flags,
    __in DEVPROPTYPE        Type,
    __in ULONG              Size,
    __in_bcount_opt(Size)
         PVOID              Data
    );

NTSTATUS
PnpGetDevicePropertyData (
    __in PDEVICE_OBJECT     Pdo,
    __in CONST DEVPROPKEY   *PropertyKey,
    __in LCID               Lcid,
    __reserved ULONG        Flags,
    __in ULONG              Size,
    __out_bcount_part(Size, *RequiredSize)
          PVOID             Data,
    __out PULONG            RequiredSize,
    __out PDEVPROPTYPE      Type
    );

NTSTATUS
PnpSetDeviceInterfacePropertyData (
    __in PUNICODE_STRING    SymbolicLinkName,
    __in CONST DEVPROPKEY   *PropertyKey,
    __in LCID               Lcid,
    __in ULONG              Flags,
    __in DEVPROPTYPE        Type,
    __in ULONG              Size,
    __in_bcount_opt(Size)
         PVOID              Data
    );

NTSTATUS
PnpGetDeviceInterfacePropertyData (
    __in PUNICODE_STRING    SymbolicLinkName,
    __in CONST DEVPROPKEY   *PropertyKey,
    __in LCID               Lcid,
    __reserved ULONG        Flags,
    __in ULONG              Size,
    __out_bcount_part(Size, *RequiredSize)
          PVOID             Data,
    __out PULONG            RequiredSize,
    __out PDEVPROPTYPE      Type
    );

//
// Convenient macros to serialize access to device property data.
//

#define PnpLockDeviceProperty(Exclusive) {                              \
    KeEnterCriticalRegion();                                            \
    if (Exclusive) {                                                    \
        ExAcquireResourceExclusiveLite(&PnpDevicePropertyLock, TRUE);   \
    } else {                                                            \
        ExAcquireResourceSharedLite(&PnpDevicePropertyLock, TRUE);      \
    }                                                                   \
}

#define PnpUnlockDeviceProperty() {                     \
    ExReleaseResourceLite(&PnpDevicePropertyLock);      \
    KeLeaveCriticalRegion();                            \
}

extern ERESOURCE PnpDevicePropertyLock;

NTSTATUS
PnpSetInterruptInformation (
    __in PDEVICE_OBJECT Pdo,
    __in PVOID Buffer,
    __in ULONG Size
    );

NTSTATUS
PnpGetInterruptInformation (
    __in PDEVICE_OBJECT Pdo,
    __in PVOID Buffer,
    __in ULONG Size,
    __out PULONG RequiredSize
    );

VOID
PnpCompareInterruptInformation (
    __in PDEVICE_OBJECT Pdo,
    __in PVOID Buffer,
    __in ULONG Size
    );

typedef struct _PNP_INTERRUPT_CONTEXT {
    ULONG Size;
    CHAR Data[ANYSIZE_ARRAY];
} PNP_INTERRUPT_CONTEXT, *PPNP_INTERRUPT_CONTEXT;