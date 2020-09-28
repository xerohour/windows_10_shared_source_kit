/*++

Copyright (c) Microsoft Corporation

Module Name:

    cpu.c

Abstract:

    This module implements processor specific utility routines.

Environment:

    Kernel mode only.

--*/

#include "ki.h"

BOOLEAN
KeAreInterruptsEnabled (
    VOID
    )

/*++

Routine Description:

    This routine indicates the current state of the interrupt flag.

Arguments:

    None.

Return Value:

    TRUE is returned if interrupts are currently enabled. Otherwise, FALSE
     is returned.

--*/

{
    UINT32 Status;

    Status = _ReadStatusReg(0);

    //
    // N.B. CPSRC_INT and CPSRC_FIQ are interrupt *disable* bits, If
    //      CPSRC_INT is clear, then interrupts were enabled.  We
    //      don't test for FIQ as the bit may be protected by TrustZone.
    //

    return (Status & CPSRC_INT) == 0;
}

