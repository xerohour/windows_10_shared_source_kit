/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    locks.c

Abstract:

    This module implements the global PNP synchronization objects.

Author:

    Santosh Jodh (santoshj) 08-02-2004

Environment:

    KernelMode only.

Revision History:

--*/

#include "pnpmgrp.h"
#pragma hdrstop

//
// Regular data segment
//
#ifdef ALLOC_DATA_PRAGMA

#pragma  data_seg()

#endif // ALLOC_DATA_PRAGMA

//
// PnpSpinLock - spinlock for Pnp code.
//
KSPIN_LOCK PnpSpinLock;

