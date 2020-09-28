/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    usbfn.h

Abstract:

    This header file is inspired by the SimpleWinphoneIo interface. 
    Exposes supported functionality for initializing the USBFn interface which 
    will be consumed by the USB subsystem of the Boot Charging application
    
Environment:

    Boot    
    
--*/


#pragma once

#include "chargelib.h"
#include <windef.h>

//
// Timeout before we can decide for sure what our connection
// status is
//
#define     USB_CONNECT_TIMEOUT         ( 7 * SECONDS )
#define     USB_ENUMERATION_TIMEOUT     ( 5 * SECONDS )

NTSTATUS UsbFnProtocolInit();

NTSTATUS UsbFnEnumerate();

NTSTATUS GetUsbFnConnectionToHost(BOOLEAN *ConnectedToHost);

NTSTATUS UsbFnEnumerateIfConnected(BOOLEAN *ConnectedToHost);

NTSTATUS GetUsbConnectedToCharger(BOOLEAN *ConnectedToHost);

NTSTATUS GetCurrentConnectionStatus(BOOLEAN *CurrentConnectionStatus);

NTSTATUS DetectPort();
