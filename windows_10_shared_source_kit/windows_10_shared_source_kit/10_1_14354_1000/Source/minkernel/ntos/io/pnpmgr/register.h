/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    register.h

Abstract:

    This module implements device registration.

Author:

    Santosh Jodh (santoshj) 08-10-2004

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

NTKERNELAPI
NTSTATUS
PpDeviceRegistration(
    __in PUNICODE_STRING DeviceInstancePath,
    __in BOOLEAN Add,
    __in_opt PUNICODE_STRING ServiceKeyName,
    __in BOOLEAN LockHeld
    );

NTSTATUS
PiDeviceRegistration(
    __in PUNICODE_STRING DeviceInstancePath,
    __in BOOLEAN Add,
    __in_opt PUNICODE_STRING ServiceKeyName
    );

