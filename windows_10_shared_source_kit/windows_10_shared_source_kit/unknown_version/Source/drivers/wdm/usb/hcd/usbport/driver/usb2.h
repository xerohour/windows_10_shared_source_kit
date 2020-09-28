//
//    Copyright (C) Microsoft.  All rights reserved.
//
NTSTATUS
USBPORT_AllocateBandwidthUSB20(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_FreeBandwidthUSB20(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_Rebalance(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __in PLIST_ENTRY ReblanceListHead
    );

VOID
USBPORT_RebalanceEndpoints(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __inout PLIST_ENTRY EndpointList
    );

ULONG
USBPORT_SelectOrdinal(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __in PHCD_ENDPOINT Endpoint
    );