/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    locks.h

Abstract:

    This module implements the global PNP synchronization objects.

Author:

    Santosh Jodh (santoshj) 08-02-2004

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

//
// Call this macro to block other resource allocations and releases in the
// system.
//
#define IopLockResourceManager() {      \
    KeEnterCriticalRegion();            \
    KeWaitForSingleObject(              \
        &PpRegistrySemaphore,           \
        DelayExecution,                 \
        KernelMode,                     \
        FALSE,                          \
        NULL);                          \
}
//
// Unblock other resource allocations and releases in the system.
//
#define IopUnlockResourceManager() {    \
    KeReleaseSemaphore(                 \
        &PpRegistrySemaphore,           \
        0,                              \
        1,                              \
        FALSE);                         \
    KeLeaveCriticalRegion();            \
}

//
// PnpSpinLock - spinlock for Pnp code.
//
extern KSPIN_LOCK PnpSpinLock;

