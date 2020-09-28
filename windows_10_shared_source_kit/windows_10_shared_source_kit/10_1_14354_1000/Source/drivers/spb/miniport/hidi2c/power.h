/*++

Module Name:

    power.h

Abstract:

    This file contains the declarations for Power specific callbacks
    and function declarations

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel-mode Driver Framework

--*/

#ifndef _POWER_H_
#define _POWER_H_

VOID
PowerIdleIrpCompletion(
            _In_ PDEVICE_CONTEXT    FxDeviceContext
            );

EVT_WDF_WORKITEM PowerIdleIrpWorkitem;

#endif _POWER_H_