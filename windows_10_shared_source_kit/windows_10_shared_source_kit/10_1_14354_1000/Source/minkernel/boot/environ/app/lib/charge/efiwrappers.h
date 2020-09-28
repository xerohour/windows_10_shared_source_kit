/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efiwrappers.h 

--*/

#pragma once

#include "chargelib.h"
#include "usbfnsupport.h"
#include <blctxp.h>
#include <blmm.h>

//-----------------------------------------------------------------------------
// General Wrappers
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// EFI UsbFn Protocol Wrappers
//-----------------------------------------------------------------------------

NTSTATUS EfiUsbFnEventHandler( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    EFI_USBFN_MESSAGE    *Message,
    UINTN *PayloadSize,
    EFI_USBFN_MESSAGE_PAYLOAD *Payload
    );
    
NTSTATUS EfiUsbFnConfigureEnableEndpoints( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    EFI_USB_DEVICE_INFO    *DeviceInfo
    );

NTSTATUS EfiUsbFnDetectPort( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    EFI_USBFN_PORT_TYPE    *PortType
    );

NTSTATUS EfiUsbFnAllocateTransferBuffer( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    UINTN    Size,
    VOID    **Buffer
    );

NTSTATUS EfiUsbFnFreeTransferBuffer( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    VOID *Buffer 
    );

NTSTATUS EfiUsbFnGetEndpointMaxPacketSize( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol,
    EFI_USB_ENDPOINT_TYPE EndpointType,
    EFI_USB_BUS_SPEED BusSpeed,
    UINT16 *MaxPacketSize 
    );

NTSTATUS EfiUsbFnGetMaxTransferSize( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    UINTN *MaxTransferSize 
    );

NTSTATUS EfiUsbFnGetDeviceInfo( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    EFI_USBFN_DEVICE_INFO_ID Id,
    UINTN *BufferSize,
    VOID *Buffer
    );

NTSTATUS EfiUsbFnGetVendorIdProductId( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    UINT16 *Vid,
    UINT16 *Pid
    );

NTSTATUS EfiUsbFnAbortTransfer( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    UINT8 EndpointIndex,
    EFI_USBFN_ENDPOINT_DIRECTION Direction
    );

NTSTATUS EfiUsbFnTransfer( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    UINT8 EndpointIndex,
    EFI_USBFN_ENDPOINT_DIRECTION Direction,
    UINTN *BufferSize,
    VOID *Buffer
    );

NTSTATUS EfiUsbFnGetEndpointStallState( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    UINT8 EndpointIndex,
    EFI_USBFN_ENDPOINT_DIRECTION Direction,
    BOOLEAN *State
    );

NTSTATUS EfiUsbFnSetEndpointStallState( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    UINT8 EndpointIndex,
    EFI_USBFN_ENDPOINT_DIRECTION Direction,
    BOOLEAN State
    );

//-----------------------------------------------------------------------------
// EFI Battery Charging Protocol wrappers
//-----------------------------------------------------------------------------

NTSTATUS EfiBatteryChargingGetBatteryStatus( 
    EFI_BATTERY_CHARGING_PROTOCOL   *EfiBatteryChargingProtocol, 
    UINT32                          *StateOfCharge,
    UINT32                          *RatedCapacity,
    INT32                           *CurrentIntoBattery
    );

NTSTATUS EfiBatteryChargingChargeBattery( 
    EFI_BATTERY_CHARGING_PROTOCOL           *EfiBatteryChargingProtocol, 
    UINT32                                  MaximumCurrent,
    UINT32                                  TargetStateOfCharge,
    EFI_BATTERY_CHARGING_COMPLETION_TOKEN   *CompletionToken
    );

NTSTATUS EfiBatteryChargingGetBatteryInformation( 
    EFI_BATTERY_CHARGING_PROTOCOL   *EfiBatteryChargingProtocol, 
    UINT32                          *StateOfCharge,
    INT32                           *CurrentIntoBattery,
    UINT32                          *BatteryTerminalVoltage,
    INT32                           *BatteryTemperature,
    UINT32                          *USBCableVoltage,
    UINT32                          *USBCableCurrent
    );
