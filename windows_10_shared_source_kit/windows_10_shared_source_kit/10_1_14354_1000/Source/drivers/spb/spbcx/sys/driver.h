/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name: 

    driver.h

Abstract:

    This module contains the internal type definitions for the SPBCx 
    class extension.

Environment:

    kernel-mode only

Revision History:

--*/
#pragma once

typedef struct _SCX_CLIENT_GLOBALS {
    //
    // Equal to GLOBALS_SIG
    //
    ULONG Signature;

    //
    // Public part of the globals
    //
    SPB_DRIVER_GLOBALS Public;

} SCX_CLIENT_GLOBALS, *PSCX_CLIENT_GLOBALS;

PSCX_CLIENT_GLOBALS
FORCEINLINE
GetPrivateGlobals(
    PSPB_DRIVER_GLOBALS PublicGlobals
    )
{
    return CONTAINING_RECORD(PublicGlobals,
                             SCX_CLIENT_GLOBALS,
                             Public);
}
