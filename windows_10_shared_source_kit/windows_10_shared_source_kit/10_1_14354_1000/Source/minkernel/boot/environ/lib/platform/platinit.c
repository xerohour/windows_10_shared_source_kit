/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    platinit.c

Abstract:

    This module implements initialization of the platform library.

Environment:

    Boot Environment.

--*/

// ------------------------------------------------------------------- Includes

#include "platform.h"

// ------------------------------------------------------------------ Functions

VOID
BlpPltDestroy (
    VOID
    )

/*++

Routine Description:

    This routine destroys the platform library.

Arguments:

    None.

Return Value:

    None.

--*/

{

    //
    // Destroy PCI configuration space.
    //

    PltDestroyPciConfiguration();
    return;
}

NTSTATUS
BlpPltInitialize (
    VOID
    )

/*++

Routine Description:

    This routine initializes the platform library.

Arguments:

    None.

Return Value:

    NT Status code.

--*/

{

    //
    // Initialize PCI configuration space access.
    //

    return PltInitializePciConfiguration();
}

