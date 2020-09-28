//
//    Copyright (C) Microsoft.  All rights reserved.
//
/*++

Module Name:

    device.h

Abstract:

    This file contains the declarations for the frameworks callbacks
    on the device object

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel-mode Driver Framework

--*/

#ifndef _DEVICE_H_
#define _DEVICE_H_

//
// WDF Device callback events
//

EVT_WDF_DEVICE_PREPARE_HARDWARE                     OnPrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE                     OnReleaseHardware;
EVT_WDF_DEVICE_D0_ENTRY                             OnD0Entry;
EXTERN_C EVT_WDF_DEVICE_D0_EXIT                     OnD0Exit;
EVT_WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED     OnPostInterruptsEnabled;

EXTERN_C
VOID
DeviceReportPnpProblem(
    _In_ WDFDEVICE  Device,
    _In_ ULONG      MsgId
    );

#endif
