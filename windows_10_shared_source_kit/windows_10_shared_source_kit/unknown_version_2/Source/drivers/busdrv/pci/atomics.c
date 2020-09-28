/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    atomicop.c

Abstract:

    This module contains routines implementing the PCI interface required
    for AtomicOp functionality.

--*/

// ------------------------------------------------------------------- Includes

#include "pcip.h"

// ---------------------------------------------------------------------- Types

// ----------------------------------------------------------------- Prototypes

NTSTATUS
PciWalkDevicePathCheckAtomicOpCallback(
    _In_ PPCI_BUS Bus,
    _Inout_ PVOID Context
    );

// ------------------------------------------------------- Function Definitions

NTSTATUS
PciDeviceQueryAtomics (
    __in PEXPRESS_PORT ExpressPort,
    __out PBOOLEAN Supported
    )

/*++

Routine Description:

    This routine checks whether atomic op is supported.

Arguments:

    ExpressPort - Supplies the pointer to the express structure.

    Supported - Stores the truth value.

Return Value:

    NT Status value.

--*/

{

    PCI_FEATURE_STATE FeatureState;
    PPCI_BUS RootPort;
    NTSTATUS Status;

    *Supported = FALSE;

    //
    // For now, only allow type-0 devices to enable atomic op.
    //
    if (ExpressPort->DeviceType != PciExpressEndpoint &&
        ExpressPort->DeviceType != PciExpressRootComplexIntegratedEndpoint) {
        return STATUS_SUCCESS;
    }

    //
    // By default, atomics are disabled.  Query the registry to determine
    // if the FDO opts-in to enable this feature.
    //

    FeatureState = PciIsDeviceFeatureEnabled(ExpressPort->Device,
                                             PCI_FEATURE_ATOMICS_OPT_IN);

    if (FeatureState != PciFeatureStateEnabled) {
        return STATUS_SUCCESS;
    }

    //
    // Assume atomics should be supported for RC integrated Endpoints.
    //

    if (ExpressPort->DeviceType != PciExpressRootComplexIntegratedEndpoint) {
        *Supported = TRUE;
        return STATUS_SUCCESS;
    }

    //
    // Ensure that all the routing elements in the device path
    // supports atomic op routing.
    //

    *Supported = TRUE;
    Status = PciWalkDevicePath(ExpressPort->Device,
                               PciWalkDevicePathCheckAtomicOpCallback,
                               (PVOID)Supported,
                               &RootPort);

    if (!NT_SUCCESS(Status)) {
        *Supported = FALSE;
        return Status;
    }

    if (RootPort == NULL) {

        //
        // RootPort == NULL means trying to enable atomic op for the
        // root port itself, which is currently not allowed.
        //

        *Supported = FALSE;
        return STATUS_SUCCESS;
    }

    //
    // Ensure the root port supports 32-bit and 64-bit atomic op.
    //

    PCI_ASSERT(RootPort->PciBridge != NULL);

    if (RootPort->PciBridge->Atomics32BitCompleterSupported == 0 ||
        RootPort->PciBridge->Atomics64BitCompleterSupported == 0) {

        *Supported = FALSE;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciDeviceEnableAtomics (
    __in PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine enables atomic op. This function is intented to be called
    after the Bus Master Enable bit is set.

Arguments:

    ExpressPort - Supplies the pointer to the express structure.

Return Value:

    NT Status value.

--*/

{

    USHORT Command;
    PPCI_DEVICE Device;
    PCI_EXPRESS_DEVICE_CONTROL_2_REGISTER DeviceControl2;
    NTSTATUS Status;
    BOOLEAN Supported;

    Device = ExpressPort->Device;

    //
    // Ensure atomic op is supported in the hierarchy.
    //

    Status = PciDeviceQueryAtomics(ExpressPort, &Supported);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    if (Supported == FALSE) {
        return STATUS_NOT_SUPPORTED;
    }

    //
    // Ensure the Bus Master Enable bit is set.
    //

    PciReadConfigRegister(Device, Command, &Command);
    if ((Command & PCI_ENABLE_BUS_MASTER) == 0) {

        PCI_ASSERT(FALSE);

        return STATUS_NOT_SUPPORTED;
    }

    //
    // Enable the atomic op in the device control 2 register.
    //

    PciReadExpressRegisterEx(Device, DeviceControl2, &DeviceControl2);
    DeviceControl2.AtomicOpRequesterEnable = 1;
    PciWriteExpressRegisterEx(Device, DeviceControl2, &DeviceControl2);
    return STATUS_SUCCESS;
}

NTSTATUS
PciDeviceDisableAtomics (
    __in PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine disables atomic op.

Arguments:

    ExpressPort - Supplies the pointer to the express structure.

Return Value:

    NT Status value.

--*/

{
    PPCI_DEVICE Device;
    PCI_EXPRESS_DEVICE_CONTROL_2_REGISTER DeviceControl2;

    Device = ExpressPort->Device;

    //
    // Disable the atomic op in the device control 2 register.
    //

    PciReadExpressRegisterEx(Device, DeviceControl2, &DeviceControl2);
    DeviceControl2.AtomicOpRequesterEnable = 0;
    PciWriteExpressRegisterEx(Device, DeviceControl2, &DeviceControl2);
    return STATUS_SUCCESS;
}

NTSTATUS
PciWalkDevicePathCheckAtomicOpCallback(
    _In_ PPCI_BUS Bus,
    _Inout_ PVOID Context
    )

/*++

Routine Description:

    This is the callback function that checks whether the component
    on the path has support for AtomicOp.

Arguments:

    Bus - Supplies the current routing element.

    Context - Supplies the storage of the truth value.

Return Value:

    NT Status value.

--*/

{

    PBOOLEAN Supported;

    PCI_ASSERT(Bus->PciBridge != NULL);

    Supported = (PBOOLEAN)Context;
    if (Bus->PciBridge->AtomicsRoutingSupported == 0) {
        *Supported = FALSE;
    }

    return STATUS_SUCCESS;
}