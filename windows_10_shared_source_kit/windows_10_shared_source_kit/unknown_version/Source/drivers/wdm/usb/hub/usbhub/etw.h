//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: etw.c */

#include <PSHPACK1.H>

typedef struct _ETW_USB_HC {
    ULONG           PciBus;
    USHORT          PciDevice;
    USHORT          PciFunction;
    USHORT          PciVendorId;
    USHORT          PciDeviceId;
} ETW_USB_HC, *PETW_USB_HC;

typedef struct _ETW_USB_HUB {
    USHORT              idVendor;
    USHORT              idProduct;
    USB_PORT_PATH       PortPath;
    USB_DEVICE_SPEED    DeviceSpeed;
} ETW_USB_HUB, *PETW_USB_HUB;

typedef struct _ETW_USB_HUB_STATE {
    DEVICE_POWER_STATE  DevicePowerState;
    SSH_HUB_STATE       HubSuspendState;
} ETW_USB_HUB_STATE, *PETW_USB_HUB_STATE;

typedef struct _ETW_USB_DEVICE {
    PVOID               DeviceHandle;
    USHORT              idVendor;
    USHORT              idProduct;
    USB_PORT_PATH       PortPath;
    USB_DEVICE_SPEED    DeviceSpeed;
} ETW_USB_DEVICE, *PETW_USB_DEVICE;

typedef struct _ETW_USB_DEVICE_STATE {
    DEVICE_POWER_STATE  DevicePowerState;
} ETW_USB_DEVICE_STATE, *PETW_USB_DEVICE_STATE;

#define MAX_ETW_DESCRIPTORS 10


#include <POPPACK.H>

typedef union _ETW_PCI_VALUE {
    ULONG   Ulong;
    USHORT  UShort[2];
} ETW_PCI_VALUE, PETW_PCI_VALUE;


VOID 
__inline
UsbhEtwGetHcInfo(
    __in  PDEVICE_EXTENSION_HUB hubExt,
    __out PETW_USB_HC EtwUsbHc
    )
{
    ETW_PCI_VALUE PciValue;

    EtwUsbHc->PciBus = hubExt->TopologyAddress.PciBusNumber;                   
        
    // need to avoid the "loss of data" warning for conversion of ULONG to USHORT
    PciValue.Ulong = hubExt->TopologyAddress.PciDeviceNumber;
    EtwUsbHc->PciDevice = PciValue.UShort[0];
    PciValue.Ulong = hubExt->TopologyAddress.PciFunctionNumber;
    EtwUsbHc->PciFunction = PciValue.UShort[0];
        
    EtwUsbHc->PciVendorId = hubExt->HcPciVendorId;
    EtwUsbHc->PciDeviceId = hubExt->HcPciDeviceId;
}        

VOID
__inline
UsbhEtwGetHubInfo(
    __in  PDEVICE_EXTENSION_HUB hubExt,
    __out PETW_USB_HUB EtwUsbHub
    )
{
    ULONG count;

    EtwUsbHub->idVendor = hubExt->DeviceDescriptor.idVendor;
    EtwUsbHub->idProduct = hubExt->DeviceDescriptor.idProduct;
        
    EtwUsbHub->PortPath.PortPathDepth = 0;
    EtwUsbHub->PortPath.PortPath[0] = hubExt->TopologyAddress.RootHubPortNumber;

    if (hubExt->TopologyAddress.RootHubPortNumber) {
        EtwUsbHub->PortPath.PortPathDepth++;
    }

    for(count=0; count < 5; count++){
        EtwUsbHub->PortPath.PortPath[count+1] = hubExt->TopologyAddress.HubPortNumber[count];
        if (EtwUsbHub->PortPath.PortPath[count+1]) {
            EtwUsbHub->PortPath.PortPathDepth++;
        }
    }

    EtwUsbHub->DeviceSpeed = hubExt->DeviceSpeed;
}

VOID
__inline
UsbhEtwGetDeviceInfo(
    __in  PDEVICE_EXTENSION_PDO pdoExt,
    __out PETW_USB_DEVICE EtwUsbDevice
    )
{
    EtwUsbDevice->DeviceHandle = pdoExt->DeviceHandle;
    EtwUsbDevice->idVendor = pdoExt->DeviceDescriptor.idVendor;
    EtwUsbDevice->idProduct = pdoExt->DeviceDescriptor.idProduct;
    EtwUsbDevice->PortPath.PortPathDepth = pdoExt->PortPath.PortPathDepth;
    EtwUsbDevice->PortPath.PortPath[0] = pdoExt->PortPath.PortPath[0];
    EtwUsbDevice->PortPath.PortPath[1] = pdoExt->PortPath.PortPath[1];
    EtwUsbDevice->PortPath.PortPath[2] = pdoExt->PortPath.PortPath[2];
    EtwUsbDevice->PortPath.PortPath[3] = pdoExt->PortPath.PortPath[3];
    EtwUsbDevice->PortPath.PortPath[4] = pdoExt->PortPath.PortPath[4];
    EtwUsbDevice->PortPath.PortPath[5] = pdoExt->PortPath.PortPath[5];
    EtwUsbDevice->DeviceSpeed = pdoExt->Speed;
}

NTSTATUS
UsbhEtwDriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING UniRegistryPath
    );

VOID
UsbhEtwUnload(
    PDRIVER_OBJECT DriverObject
    );

VOID
UsbhEtwEnableCallback (
    __in LPCGUID SourceId,
    __in ULONG IsEnabled,
    __in UCHAR Level,
    __in ULONGLONG MatchAnyKeyword,
    __in ULONGLONG MatchAllKeyword,
    __in_opt PEVENT_FILTER_DESCRIPTOR FilterData,
    __in_opt PVOID CallbackContext
    );

VOID
UsbhEtwRundown (
    VOID
    );

VOID
UsbhEtwLogHubInformation(
    __in PDEVICE_EXTENSION_HUB hubExt,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in BOOLEAN LogHubDescriptor
    );

VOID
UsbhEtwLogPortInformation(
    __in PDEVICE_OBJECT HubFdo
    );

VOID
UsbhEtwLogHubPastExceptions(
    __in PDEVICE_EXTENSION_HUB  hubExt
    );

VOID
UsbhEtwLogDeviceDescription(
    __in PDEVICE_EXTENSION_PDO pdoExt,
    __in PCEVENT_DESCRIPTOR EventDescriptor
    );

VOID
UsbhEtwLogDeviceInformation(
    __in PDEVICE_EXTENSION_PDO pdoExt,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in BOOLEAN LogDeviceDescriptor
    );

VOID
UsbhEtwLogHubEvent(
    __in PDEVICE_OBJECT HubFdo,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in ULONG PortNumber,
    __in NTSTATUS NtStatus
    );

VOID
UsbhEtwLogHubIrpEvent(
    __in PDEVICE_OBJECT HubFdo,
    __in_opt PIRP Irp,
    __in_opt LPGUID ActivityId,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in ULONG PortNumber,
    __in NTSTATUS NtStatus
    );

NTSTATUS
UsbhEtwGetActivityId(
    __in PIRP Irp,
    __out GUID *ActivityId
    );

VOID
UsbhEtwLogHubPowerEvent(
    __in PDEVICE_OBJECT HubFdo,
    __in_opt PIRP Irp,
    __in_opt LPGUID ActivityId,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in POWER_STATE PowerState,
    __in NTSTATUS NtStatus
    );

VOID
UsbhEtwLogDeviceEvent(
    __in PDEVICE_EXTENSION_PDO pdoExt,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in NTSTATUS NtStatus
    );

VOID
UsbhEtwLogDeviceIrpEvent(
    __in PDEVICE_EXTENSION_PDO pdoExt,
    __in_opt PIRP Irp,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in NTSTATUS NtStatus
    );

VOID
UsbhEtwLogDevicePowerEvent(
    __in PDEVICE_EXTENSION_PDO pdoExt,
    __in_opt PIRP Irp,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in POWER_STATE PowerState,
    __in NTSTATUS NtStatus
    );

VOID
UsbhEtwLogHubException(
    __in PDEVICE_OBJECT HubFdo,
    __in ULONG PortNumber,
    __in HUB_EXCEPTION_CLASS Class,
    __in NTSTATUS NtStatus,
    __in USBD_STATUS UsbdStatus,
    __in PCSTR DebugText,
    __in ULONG DebugTextSize
    );

VOID
UsbhEtwLogHubEventWithExtraData(
    __in PDEVICE_OBJECT HubFdo,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in ULONG PortNumber,
    __in_bcount(DataSize) PVOID Data,
    __in ULONG DataSize
    );

NTSTATUS
UsbhEtwWrite(
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in_opt LPCGUID ActivityId,
    ...
    );
