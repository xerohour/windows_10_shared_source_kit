//
//    Copyright (C) Microsoft.  All rights reserved.
//
/*++

Module Name:

    driver.h

Abstract:

    This file contains the declarations for the frameworks callbacks
    on the driver object

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel-mode Driver Framework

--*/

#ifndef _DRIVER_H_
#define _DRIVER_H_

//
// WDF Driver callback events
//

EXTERN_C DRIVER_INITIALIZE                  DriverEntry;
EXTERN_C EVT_WDF_DRIVER_DEVICE_ADD          OnDeviceAdd;
EXTERN_C EVT_WDF_DRIVER_UNLOAD              OnDriverUnload;

#endif