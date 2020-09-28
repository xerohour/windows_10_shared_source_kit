/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    internal.h

Abstract:

    This is the standard header for all of the source modules within the
    SPBCx driver.

Environment:

    kernel-mode only

Revision History:

--*/
#include <ntosp.h>
#include <ntstrsafe.h>
#include <ntintsafe.h>

#include "wdf.h"
#include "wdfcx.h"
#include "wdfldr.h"
#include "wdmsec.h"

#include "Scx.h"

#define RESHUB_USE_HELPER_ROUTINES
#include "reshub.h"
#include "gpio.h"

#define countof(x) (sizeof(x) / sizeof(x[0]))

typedef GUID* PGUID;

//
// Tag used for pool allocations.
//

#define SCX_POOL_TAG 'xBPS'

#ifdef _PREFAST_
#define PREFAST_ASSUME(a) __assume(a);
#else
#define PREFAST_ASSUME(a)
#endif

//
// Security descriptor macro for UMDF drivers.
//

DECLARE_CONST_UNICODE_STRING(
    SDDL_DEVOBJ_SPBCX_SYS_ALL_ADM_ALL_UMDF_ALL,
    L"D:P(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;UD)");

//
// Forward Declarations
//

typedef class CScxController CScxController, *PCScxController;
typedef class CScxTarget     CScxTarget,     *PCScxTarget;
typedef class CScxRequest    CScxRequest,    *PCScxRequest;


#include "driver.h"
#include "controller.h"
#include "target.h"
#include "request.h"
#include "verifier.h"

#define SCX_CONTROL_DEVICE_NAME L"\\Device\\SpbCx"
#define SCX_GLOBALS_SIG 'XbpS'

#define Add2Ptr(P,I) ((PVOID)((PUCHAR)(P) + (I)))

BOOLEAN
FORCEINLINE
SCX_IS_DESCRIPTOR_SHARED (
    _In_ PPNP_SERIAL_BUS_DESCRIPTOR Descriptor
    )

/*++

  Routine Description:

    This function/macro checks if the given SPB descriptor is marked as shared
    or not. The sharing/exclusive flag is only defined in V2 or above
    descriptors. If the descriptor is v1, then assume it supports sharing by
    default.

  Arguments:

    Descriptor - Supplies the descriptor to be checked.

  Return Value:

    TRUE if the descriptor is shared. FALSE otherwise.

--*/

{

    BOOLEAN Shared;

    if (Descriptor->RevisionId >= SERIAL_BUS_DESCRIPTOR_REVISION_V2)
    {
       Shared =
        ((Descriptor->GeneralFlags & SERIAL_BUS_FLAG_SHARED_DESCRIPTOR) != 0);

    } else {
        Shared = TRUE;
    }

    return Shared;
}

