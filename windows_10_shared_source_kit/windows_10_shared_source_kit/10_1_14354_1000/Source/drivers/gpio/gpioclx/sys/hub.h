/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    hub.h

Abstract:

    This header file contains definitions and protoypes used by the hub module.


Environment:

    Kernel mode

--*/

#pragma once

//
// -------------------------------------------------------------------- Defines
//


#define GPIO_IS_HUB_DEVICE(Device) (Device == GpioHubDevice)

#define STATUS_GPIO_INSTANCE_NOT_REGISTERED                                 \
    STATUS_SECONDARY_IC_PROVIDER_NOT_REGISTERED

//
// ---------------------------------------------------------------------- Types
//

//
// Interfaces exported by the hub driver to the class extension.
//

typedef struct _GPIO_HUB_REGISTRATION_DATA {

    //
    // The name of the target device.
    //
    // N.B. This should be treated as a read-only string and should not be
    //      freed.
    //

    PCUNICODE_STRING TargetName;

    //
    // Reference to the BIOS name of the target device
    //
    // N.B. This should be treated as a read-only string and should not be
    //      freed.
    //

    PCUNICODE_STRING BiosName;

    //
    // The context to be supplied when calling GPIO client driver handlers.
    //

    PVOID Context;
} GPIO_HUB_REGISTRATION_DATA, *PGPIO_HUB_REGISTRATION_DATA;

//
// The type of input value specified for search and other routines.
//

typedef enum _INPUT_VALUE_TYPE {
    InputTypeVirq = 1,
    InputTypeBiosName
} INPUT_VALUE_TYPE, *PINPUT_VALUE_TYPE;

//
// -------------------------------------------------------------------- Globals
//

extern WDFDEVICE GpioHubDevice;

extern KEVENT GpioHubEmergencyTerminateEvent;

extern PETHREAD GpioHubEmergencyThread;

//
// ----------------------------------------------------------------- Prototypes
//

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpInitialize (
    __in ULONG Phase,
    __in PDRIVER_OBJECT WdmDriverObject,
    __in WDFDRIVER WdfDriver
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpRegisterGpioDevice (
    __in PGPIO_HUB_REGISTRATION_DATA RegistrationData
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpInitializeSecondaryICInterface (
    __in PDRIVER_OBJECT DriverObject
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
__drv_sameIRQL
VOID
GpioHubpUninitialize (
    VOID
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpUnregisterGpioDevice (
    __in PGPIO_HUB_REGISTRATION_DATA RegistrationData
    );

_Must_inspect_result_
BOOLEAN
GpioHubpIsSecondaryInterrupt (
    __in ULONG Virq
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
GpioHubQueueEmergencyWorkItem (
    __in PWORK_QUEUE_ITEM WorkItem
    );

//
// Interfaces exported by the hub driver externally (to the HAL).
//

SECONDARY_INTERRUPT_ENABLE GpioHubEnableInterrupt;
SECONDARY_INTERRUPT_DISABLE GpioHubDisableInterrupt;
SECONDARY_INTERRUPT_MASK GpioHubMaskInterrupt;
SECONDARY_INTERRUPT_UNMASK GpioHubUnmaskInterrupt;
SECONDARY_INTERRUPT_QUERY_PRIMARY_INTERRUPT_INFORMATION
                    GpioHubQueryPrimaryInterruptInformation;

//
// Interfaces exported to WDF.
//

EVT_WDF_DEVICE_PROCESS_QUERY_INTERFACE_REQUEST
    GpioHubEvtProcessQueryInterfaceRequest;


