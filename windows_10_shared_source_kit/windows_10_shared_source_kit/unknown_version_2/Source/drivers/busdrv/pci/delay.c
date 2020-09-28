/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    delay.c

Abstract:

    This module implements delay/stall support for PCI.  All delays/stalls
    are expected to route thru functions defined in this module.


--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes
// --------------------------------------------------------------------- Pragmas
// --------------------------------------------------------------------- Globals
// ------------------------------------------------------------------- Functions

NTSTATUS
PciDevice_Delay(
    __in PPCI_DEVICE Device,
    __in PCI_DELAY_INFO DelayType
    )

/*++

Routine Description:

    This routine implements a relative delay, which must be called at PASSIVE level.

Arguments:

    Context - This is the "Context" pointer exposed in the bridge interface.
        It stores the PDO's device extension.

    DelayType - An enum which identifies the type of delay requested

Return Value:

    NTSTATUS

--*/

{
    LARGE_INTEGER DelayPeriod;

    PCI_ASSERT(DelayType < DelayMax);
    
    if ((DelayType < 0) || (DelayType >= DelayMax)) {
        TRACE(DELAY_TRACE_LEVEL,
              "Device 0x%p: Invalid DelayType 0x%x\n",
              Device,
              DelayType
              );

        return STATUS_INVALID_PARAMETER;
    }

    TRACE(DELAY_TRACE_LEVEL,
          "Device 0x%p delaying for 0x%llx\n",
          Device,
          Device->DelayInfo[DelayType]
          );

    //  
    // Convert the microsecond delay into 100ns units and 
    // mark the delay as relative.
    //

    DelayPeriod.QuadPart = -10 * Device->DelayInfo[DelayType];
    KeDelayExecutionThread(KernelMode,
                           FALSE,
                           &DelayPeriod
                           );

    return STATUS_SUCCESS;
}

NTSTATUS
PciDevice_Stall(
    __in PPCI_DEVICE Device,
    __in ULONG MicroSeconds
    )
/*++

Routine Description:

    This routine implements a stall, which can be called at DISPATCH.

Arguments:

    Device - the PDO's device extension.

    MicroSeconds - The stall duration in microseconds.

Return Value:

    NTSTATUS

--*/
{

    TRACE(DELAY_TRACE_LEVEL,
          "Device 0x%p stalling for 0x%x\n",
          Device,
          MicroSeconds
          );

    KeStallExecutionProcessor(MicroSeconds);
    return STATUS_SUCCESS;
}

