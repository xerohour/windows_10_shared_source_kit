//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once

#define USB_VERSION_11 0x0110
#define USB_VERSION_20 0x0200

BOOLEAN
Usbd_IsDeviceAtleastHighSpeed(
    __in 
        PVOID Context
    );

NTSTATUS
Usbd_BusQueryBusInformation(
    __in      
        PVOID       Context,
    __in      
        ULONG       Level,
    __in      
        PVOID       BusInformationBuffer,
    __inout   
        PULONG      BusInformationBufferLength,
    __out_opt 
        PULONG      BusInformationActualLength
    );

VOID
Usbd_BusGetUSBDIVersion(
    __in_opt 
        PVOID                         Context,
    __out_opt 
        PUSBD_VERSION_INFORMATION     VersionInformation,
    __out_opt 
        PULONG                        HcdCapabilities
    );


NTSTATUS
Usbd_BusQueryBusTime(
    __in
        PVOID     Context,
    __out_opt 
        PULONG    CurrentFrame
    );


NTSTATUS
Usbd_BusQueryBusTimeEx(
    __in_opt 
        PVOID     Context,
    __out_opt 
        PULONG    CurrentMicroFrame
    );

NTSTATUS
Usbd_QueryControllerType(
    __in_opt 
        PVOID     Context,
    __out_opt 
        PULONG    HcdiOptionFlags,
    __out_opt 
        PUSHORT   PciVendorId,
    __out_opt 
        PUSHORT   PciDeviceId,
    __out_opt 
        PUCHAR    PciClass,
    __out_opt 
        PUCHAR    PciSubClass,
    __out_opt 
        PUCHAR    PciRevisionId,
    __out_opt 
        PUCHAR    PciProgIf
    );

NTSTATUS
Usbd_BusEnumLogEntry(
    PVOID Context,
    ULONG DriverTag,
    ULONG EnumTag,
    ULONG P1,
    ULONG P2
    );

NTSTATUS
Usbd_BusSubmitIsoOutUrb(
    PVOID Context,
    PURB  Urb
    );

