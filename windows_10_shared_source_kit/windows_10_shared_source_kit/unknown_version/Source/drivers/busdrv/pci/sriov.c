/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    sriov.c

Abstract:

    This module implements support for Single-Root I/O Virtualization also
    known as SR-IOV.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "sriov.tmh"

// --------------------------------------------------------------------- Defines

#define VIRTINTRF_VERSION 1

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes


_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR PciVirtualization_Constructor;

VOID
PciVirtualization_Reference(
    __inout PVOID Context
    );

VOID
PciVirtualization_Dereference(
    __inout PVOID Context
    );

GET_VIRTUAL_DEVICE_DATA PciVirtualization_GetFunctionData;
SET_VIRTUAL_DEVICE_DATA PciVirtualization_SetFunctionData;
GET_VIRTUAL_DEVICE_LOCATION PciVirtualization_GetLocation;
GET_VIRTUAL_DEVICE_RESOURCES PciVirtualization_GetResources;
ENABLE_VIRTUALIZATION PciVirtualization_EnableVirtualization;
GET_VIRTUAL_FUNCTION_PROBED_BARS PciVirtualization_GetVirtualFunctionProbedBars;


// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE,    PciVirtualization_Constructor)
    #pragma alloc_text(PAGE,    PciVirtualization_GetVirtualFunctionProbedBars)
    #pragma alloc_text(PAGE,    PciIsSriovSupported)
	#pragma alloc_text(PAGE,    PciIsSriovDriverAvailable)
    #pragma alloc_text(PAGE,    PciVirtualization_GetLocation)
    #pragma alloc_text(PAGE,    PciVirtualization_GetResources)
    #pragma alloc_text(PAGE,    PciVirtualizationCheckAcsSupport)
    #pragma alloc_text(PAGE,    PciVirtualizationConfigureAcs)
    #pragma alloc_text(PAGE,    PciVirtualization_EnableVirtualization)
#endif

// --------------------------------------------------------------------- Globals

const PCI_INTERFACE PciVirtualizationInterface = {
    &GUID_PCI_VIRTUALIZATION_INTERFACE,     // InterfaceType
    sizeof(PCI_VIRTUALIZATION_INTERFACE),   // MinSize
    VIRTINTRF_VERSION,                      // MinVersion
    VIRTINTRF_VERSION,                      // MaxVersion
    PCIIF_PDO,                              // Flags
    PciInterface_Virtualization,            // Signature
    PciVirtualization_Constructor           // Constructor
};

//
// UpstreamForwarding | CompletionRedirect | RequestRedirect
//
const PCI_EXPRESS_ACS_CONTROL PciAcsBitsForIov = {
    0x1c
};

// ------------------------------------------------------------------- Functions

NTSTATUS
PciGetRequirementsForVfs(
    __inout PPCI_DEVICE Device,
    __inout PPCI_DEVICE_REQUIREMENTS Requirements
    )
/*++

Routine Description:

    This routine gets the device requirements for a type 0 PCI device that
    is SR-IOV capable.

    These requirements are a set of additional BARs on top of the regular
    type 0 BARs.

Parameters:

    Device - The PCI device to get the requirements of.

    Requirements - Pointer to a structure that will contain the requirements
        on successful return from this routine.

Return Value:

    NT Status code.

--*/
{
    PCI_EXPRESS_SRIOV_CAPABILITY configHeader, probedHeader;
    PIO_RESOURCE_DESCRIPTOR requirement;
    NTSTATUS status = STATUS_SUCCESS;
    ULONG barIndex, upperBar;
    BOOLEAN bar64bit = FALSE;
    PEXPRESS_PORT port;

    NON_PAGED_CODE();


    port = Device->ExpressPort;
    if (port == NULL || port->SriovCapability == 0 || 
        (PciSystemWideHackFlags & PCI_SYS_HACK_NO_SRIOV) != 0) {
        return STATUS_SUCCESS;
    }

    PciProbeDeviceConfig(
        Device,
        &configHeader.BaseAddresses[0],
        &probedHeader.BaseAddresses[0],
        SRIOV_FIELD_OFFSET(Device, BaseAddresses),
        SRIOV_FIELD_LENGTH(Device, BaseAddresses)
        );


    //
    // Save the probed BAR values so that the PF can simply query for
    // the probed values instead of calculating indirectly from the
    // resource requirements.
    //
    RtlCopyMemory(port->ProbedVfBars,
                  probedHeader.BaseAddresses,
                  sizeof(ULONG) * PCI_TYPE0_BAR_COUNT);


    //
    // Loop through the possible BARs, getting the requirement for each.
    //
    for (barIndex = 0; barIndex < PCI_TYPE0_BAR_COUNT; barIndex++) {

        //
        // upperBar contains the (potential) upper 32 bits of a 64 bit
        // BAR.  only populate this if we're not on the last BAR, for
        // which there can be no upper 32 bits.
        //
        if (barIndex < (PCI_TYPE0_BAR_COUNT - 1)) {
#pragma prefast(suppress:__WARNING_INDEX_EXCEEDS_MAX, "barIndex < 5")
            upperBar = probedHeader.BaseAddresses[barIndex + 1];
        } else {
            upperBar = 0;
        }

        requirement = &Requirements->type0.VfBars[barIndex];

        status = PciRequirementFromBar(probedHeader.BaseAddresses[barIndex],
                                       upperBar,
                                       requirement,
                                       &bar64bit
                                       );
        if (!NT_SUCCESS(status)) {
            return status;
        }

        //
        // PCI I/O space is not supported in VFs.
        //
        if  (requirement->Type == CmResourceTypePort) {
            ERROR("I/O Ports not supported for virtual functions");
            return STATUS_DEVICE_PROTOCOL_ERROR;
        }

        //
        // The length of the VF bar is the value determined according to
        // the PCI 3.0 spec multiplied by the number set for NumVFs.
        //
        if (requirement->Type == CmResourceTypeMemory ||
            requirement->Type == CmResourceTypeMemoryLarge) {


            //
            // PF is required to align all BAR resources on a System
            // Page Size boundary.  Verify that it does.
            //
            if (requirement->u.Memory.Alignment < PAGE_SIZE) {
                ERROR("VF BAR not aligned on a page boundary");
                return STATUS_DEVICE_PROTOCOL_ERROR;
            }

            status = RtlULongMult(requirement->u.Memory.Length,
                                  port->NumVFs,
                                  &requirement->u.Memory.Length);

            if (!NT_SUCCESS(status)) {
                ERROR("VF BAR too large");
                return STATUS_DEVICE_PROTOCOL_ERROR;
            }
        }

        if (bar64bit) {

            //
            // Seeing as 64bit bars occupy two indices make sure that we don't
            // run off the end of the bars.
            //
            if (barIndex != (PCI_TYPE0_BAR_COUNT - 1)) {

                //
                // The 64 bit BAR eats up two BARs, so mark the upper half as
                // not in use.
                //
                barIndex++;
                Requirements->type0.VfBars[barIndex].Type = CmResourceTypeNull;

            } else {
                ERROR("PciGetRequirementsForVfs: Attempt to reference beyond end of 64-bit BAR.\n");
            }
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciGetResourcesForVfs(
    __in PPCI_DEVICE Device,
    __inout PPCI_DEVICE_RESOURCES Resources
    )
/*++

Routine Description:

    This routine gets the currently decoded resources for Virtual
    functions belonging to a type 0 PCI device.  These resources
    include the BARs in the SR-IOV capability header.

Parameters:

    Device - The device to get the resources of.

    Resources - Pointer to a structure that will contain the VF resources
        on successful return from this routine.

Return Value:

    NT Status code.

--*/
{
    PIO_RESOURCE_DESCRIPTOR barRequirement;
    NTSTATUS status = STATUS_SUCCESS;
    PCI_EXPRESS_SRIOV_CAPABILITY sriovHeader;
    ULONG barIndex, upperBar;
    PEXPRESS_PORT port;

    port = Device->ExpressPort;
    if (port == NULL || port->SriovCapability == 0 || 
        (PciSystemWideHackFlags & PCI_SYS_HACK_NO_SRIOV) != 0) {
        return STATUS_SUCCESS;
    }

    //
    // If the VF MSE and VF Enable bits are not set, the device has
    // no resources that it is currently decoding.
    //
    PciReadSriovRegister(Device->ExpressPort,
                         SRIOVControl,
                         &sriovHeader.SRIOVControl);

    if (sriovHeader.SRIOVControl.VFEnable == FALSE ||
        sriovHeader.SRIOVControl.VFMemorySpaceEnable == FALSE) {

        return STATUS_SUCCESS;
    }

    PciReadSriovRegister(
        Device->ExpressPort,
        BaseAddresses,
        &sriovHeader.BaseAddresses[0]
        );

    //
    // Convert the BARs into resource descriptors
    //
    for (barIndex = 0; barIndex < PCI_TYPE0_BAR_COUNT; barIndex++) {

        barRequirement = &Device->Requirements.type0.VfBars[barIndex];

        if (barRequirement->Type == CmResourceTypeNull) {
            //
            // If this BAR is not implemented, continue
            //
            continue;
        }

        //
        // upperBar contains the (potential) upper 32 bits of a 64 bit
        // BAR.  only populate this if we're not on the last BAR, for
        // which there can be no upper 32 bits.
        //
        if (barIndex < (PCI_TYPE0_BAR_COUNT - 1)) {
#pragma prefast(suppress:__WARNING_INDEX_EXCEEDS_MAX, "barIndex < 5")
            upperBar = sriovHeader.BaseAddresses[barIndex+1];
        } else {
            upperBar = 0;
        }

        status = PciResourceFromBar(sriovHeader.BaseAddresses[barIndex],
                                    upperBar,
                                    barRequirement,
                                    &Resources->type0.VfBars[barIndex]
                                    );
        if (!NT_SUCCESS(status)) {
            return status;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciSetResourcesForVfs(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_RESOURCES Resources
    )
/*++

Routine Description:

    This routine sets the device resources for virtual functions of a
    type 0 PCI device which is SR-IOV capable.  It sets these
    resources both into the hardware and into the given config header
    (so that the updated config header can be saved away).

Parameters:

    Device - The PCI device to set the resources of.

    Resources - Pointer to a structure that contains the resources
        to set.

Return Value:

    NT Status code.

--*/
{
    PCM_PARTIAL_RESOURCE_DESCRIPTOR resource;
    PIO_RESOURCE_DESCRIPTOR requirement;
    PCI_EXPRESS_SRIOV_CAPABILITY sriovHeader;
    ULONG barIndex;
    PEXPRESS_PORT port;

    NON_PAGED_CODE();

    port = Device->ExpressPort;
    if (port == NULL || port->SriovCapability == 0 ||
        (PciSystemWideHackFlags & PCI_SYS_HACK_NO_SRIOV) != 0) {
        return STATUS_SUCCESS;
    }

    PCI_DEVICE_SET_FLAG(Device, SriovDeviceDidntGetVfBarSpace);

    //
    // Extract the data from the BARs and write it out to hardware
    //
    for (barIndex = 0; barIndex < PCI_TYPE0_BAR_COUNT; barIndex++) {

        resource = &Resources->type0.VfBars[barIndex];
        requirement = &Device->Requirements.type0.VfBars[barIndex];

        //
        // Typically the resource type and requirement type need to
        // match.  The only exception is if the resource has been
        // filtered out, and therefore is CmResourceTypeNull.  In this
        // case, we better have decided to turn off the decode for
        // this resource type, because otherwise the BAR will contain
        // a bogus value, since none was assigned.
        //
        if (resource->Type != requirement->Type) {

            if (requirement->Type == CmResourceTypePort) {
                ERROR("PciSetResourcesForVfs: invalid resource parameter.\n");
                return STATUS_INVALID_PARAMETER;

            } else if ((requirement->Type == CmResourceTypeMemory) ||
                        (requirement->Type == CmResourceTypeMemoryLarge)) {

                //
                // Return with SriovDeviceDidntGetVfBarSpace set, as a VF BAR
                // didn't get satisfied.
                //
                INFO("PciSetResourcesForVfs: SR-IOV device starting without VF BAR resources.\n");
                return STATUS_SUCCESS;

            } else {
                //
                // Someting is wrong - the requirements can be either
                // memory or null.
                //
                ERROR("PciSetResourcesForVfs: corrupted internal state.\n");
                return STATUS_INTERNAL_ERROR;
            }
        }

        if (resource->Type == CmResourceTypeNull) {

            sriovHeader.BaseAddresses[barIndex] = 0;
            continue;
        }

        sriovHeader.BaseAddresses[barIndex] = resource->u.Generic.Start.LowPart;

        //
        // If this is a 64bit bar populate the next bar with the upper bits and
        // advance index
        //
        if ((ULONGLONG)requirement->u.Generic.MaximumAddress.QuadPart > MAXULONG) {
            barIndex++;

            if (barIndex == PCI_TYPE0_BAR_COUNT) {
                ERROR("PciSetResourcesForVfs: Attempt to reference beyond end of 64-BIT BAR.\n");
                return STATUS_INVALID_PARAMETER;
            }

            sriovHeader.BaseAddresses[barIndex] = resource->u.Generic.Start.HighPart;

        } else {

            if (resource->u.Generic.Start.HighPart != 0) {
                ERROR("PciSetResourcesForVfs: invalid resource paramenter.\n");
                return STATUS_INVALID_PARAMETER;
            }
        }
    }

    PCI_DEVICE_CLEAR_FLAG(Device, SriovDeviceDidntGetVfBarSpace);

    PciWriteSriovRegister(Device->ExpressPort,
                          BaseAddresses,
                          sriovHeader.BaseAddresses);

    return STATUS_SUCCESS;
}

UCHAR
PciMaximumBusNumbersToCapture(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This function calculates the maximum number of bus numbers to
    reserve for an SR-IOV device.  The device may report different
    values depending on the status of ARI capability so this function
    should run after processing the ARI capability.

Arguments:

    Device - Identifies the device.

Return Value:

    The maximum number of bus numbers the device can consume.

--*/
{
    PEXPRESS_PORT ExpressPort;
    USHORT RoutingIdDelta;

    ExpressPort = Device->ExpressPort;
    if (ExpressPort == NULL || ExpressPort->SriovCapability == 0) {
        return 0;
    }

    if (ExpressPort->TotalVFs == 0) {
        return 0;
    }

    RoutingIdDelta = ExpressPort->FirstVFOffset +
        (ExpressPort->TotalVFs - 1) * ExpressPort->VFStride;

    return RoutingIdDelta / 256;
}


// ---------------------------------------------------- Virtualization Interface

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciVirtualization_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    This routine returns a PCI_VIRTUALIZATION_INTERFACE

Arguments:

    DeviceExtension - Pointer to the device extension of the PDO in question.

    InterfaceSpecificData - Unused.

    Version - Interface version.

    Size - Size of the PCI_VIRTUALIZATION_INTERFACE interface object.

    InterfaceReturn - The interface object pointer to be filled in.

    Filtered - Not supported for this interface.
 
Return Value:

    NT Status code.

--*/
{
    PPCI_VIRTUALIZATION_INTERFACE interface;
    PPCI_DEVICE device;
    NTSTATUS status;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(PCI_VIRTUALIZATION_INTERFACE)) {
        return STATUS_UNSUCCESSFUL;
    }

    device = (PPCI_DEVICE)DeviceExtension;

    if (device->ExpressPort == NULL ||
        device->ExpressPort->SriovCapability == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    status = PciVirtualizationCheckAcsSupport(device);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    interface = (PPCI_VIRTUALIZATION_INTERFACE)InterfaceReturn;
    RtlZeroMemory(interface, sizeof(PCI_VIRTUALIZATION_INTERFACE));

    interface->Size = sizeof(PCI_VIRTUALIZATION_INTERFACE);
    interface->Version = VIRTINTRF_VERSION;
    interface->Context = DeviceExtension;

    interface->InterfaceReference = PciVirtualization_Reference;
    interface->InterfaceDereference = PciVirtualization_Dereference;
    interface->GetVirtualFunctionData = PciVirtualization_GetFunctionData;
    interface->SetVirtualFunctionData = PciVirtualization_SetFunctionData;
    interface->GetLocation = PciVirtualization_GetLocation;
    interface->GetResources = PciVirtualization_GetResources;
    interface->EnableVirtualization = PciVirtualization_EnableVirtualization;
    interface->GetVirtualFunctionProbedBars = PciVirtualization_GetVirtualFunctionProbedBars;

    return STATUS_SUCCESS;
}

VOID
PciVirtualization_Reference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine increments outstanding reference count for this interface

Arguments:

    PCI_DEVICE

Return Value:

    None

--*/
{
    PPCI_DEVICE device = (PPCI_DEVICE)Context;

    InterlockedIncrement(&device->PciBusInterfaceCount);

    return;
}

VOID
PciVirtualization_Dereference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine decrements outstanding reference count for this interface

Arguments:

    PCI_DEVICE

Return Value:

    None

--*/
{
    PPCI_DEVICE device = (PPCI_DEVICE)Context;

    InterlockedDecrement(&device->PciBusInterfaceCount);

    return;
}

__drv_functionClass(GET_VIRTUAL_DEVICE_DATA)
_IRQL_requires_same_
__drv_maxIRQL(DISPATCH_LEVEL)
ULONG
PciVirtualization_GetFunctionData(
    _Inout_ PVOID Context,
    _In_ USHORT VirtualFunction,
    _Out_writes_bytes_(Length) PVOID Buffer,
    _In_ ULONG Offset,
    _In_ ULONG Length
    )
/*++

Routine Description:

    This function reads the PCI configuration space of a virtual function of
    an SR-IOV compliant device.

Arguments:

    VirtualFunction - Indicates which VF to use.

    Context - Supplies a pointer  to the interface context.  This is actually
        the PDO extension for the device's PF.

    Buffer - Supplies a pointer to where the data is to be returned

    Offset - Indicates the offset into the space where the reading should begin.

    Length - Indicates the count of bytes which should be read.

Return Value:

    Returns the number of bytes actually read.

--*/
{
    ULONG lengthRead = 0;
    USHORT functionOffset;
    PPCI_DEVICE device;
    PEXPRESS_PORT port;

    NON_PAGED_CODE();

    device = (PPCI_DEVICE)Context;
    port = device->ExpressPort;
    if (port == NULL || VirtualFunction >= port->NumVFs) {
        return 0;
    }

    functionOffset = port->FirstVFOffset + VirtualFunction * port->VFStride;

    PciReadDeviceSpace(
        device,
        functionOffset,
        PCI_WHICHSPACE_CONFIG,
        Buffer,
        Offset,
        Length,
        &lengthRead
        );

    return lengthRead;
}

__drv_functionClass(SET_VIRTUAL_DEVICE_DATA)
_IRQL_requires_same_
__drv_maxIRQL(DISPATCH_LEVEL)
ULONG
PciVirtualization_SetFunctionData(
    _Inout_ PVOID Context,
    _In_ USHORT VirtualFunction,
    _In_reads_bytes_(Length) PVOID Buffer,
    _In_ ULONG Offset,
    _In_ ULONG Length
    )
/*++

Routine Description:

    This function writes the PCI configuration space of a virtual function of
    an SR-IOV compliant device.

Arguments:

    VirtualFunction - Indicates which VF to use.

    Context - Supplies a pointer  to the interface context.  This is actually
        the PDO extension for the device's PF.

    Buffer - Supplies a pointer to the data.

    Offset - Indicates the offset into the space where the writing should begin.

    Length - Indicates the count of bytes which should be written.

Return Value:

    Returns the number of bytes actually written.

--*/
{
    ULONG lengthWritten = 0;
    USHORT functionOffset;
    PPCI_DEVICE device;
    PEXPRESS_PORT port;

    NON_PAGED_CODE();

    device = (PPCI_DEVICE)Context;
    port = device->ExpressPort;
    if (port == NULL || VirtualFunction >= port->NumVFs) {
        return 0;
    }

    functionOffset = port->FirstVFOffset + VirtualFunction * port->VFStride;

    PciWriteDeviceSpace(
        device,
        functionOffset,
        PCI_WHICHSPACE_CONFIG,
        Buffer,
        Offset,
        Length,
        &lengthWritten
        );

    return lengthWritten;
}

__drv_functionClass(GET_VIRTUAL_DEVICE_LOCATION)
_IRQL_requires_same_
__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
PciVirtualization_GetLocation(
    _Inout_ PVOID Context,
    _In_ USHORT VirtualFunction,
    _Out_ PUINT16 SegmentNumber,
    _Out_ PUINT8 BusNumber,
    _Out_ PUINT8 FunctionNumber
    )
/*++

Routine Description:

    This routine gets the device location of the physical function
    (segment, bus, PCI device number and function number).  PF drivers
    need to expose these properties via WMI

Parameters:

    Context - Supplies a pointer  to the interface context.  This is actually
        the PDO extension for the device's PF.

    VirtualFunction - Indicates which VF to use.

    SegmentNumber - Pointer to location that will receive the segment number

    BusNumber - Pointer to location that will receive the bus number

    FunctionNumber - Poiner to location that will receive the function number,
        as a flat 0-255 value.

Return Value:

    None.

--*/
{
    ULONG busNumber;
    PPCI_DEVICE device;
    USHORT functionOffset;
    PEXPRESS_PORT port;
    PCI_SLOT_NUMBER slotNumber;

    PAGED_CODE();

    device = (PPCI_DEVICE)Context;
    port = device->ExpressPort;

    if (VirtualFunction >= port->NumVFs) {
        return STATUS_INVALID_PARAMETER;
    }

    functionOffset = port->FirstVFOffset + VirtualFunction * port->VFStride;
    PciGetConfigSlotAndBus(device, functionOffset, &slotNumber, &busNumber);

    *SegmentNumber = SEGMENT_NUMBER_FOR_BUS(device->BusNumber);
    *BusNumber = (UINT8)busNumber;

    //
    // PCIe devices don't really have a device number.  So FunctionNumber
    // contains the function on the bus, as a single 8-bit value.  This is a
    // better, less ambiguous, representation for PCIe virtual functions.
    //

    *FunctionNumber = ((UINT8)slotNumber.u.bits.DeviceNumber << 3) | 
        (UINT8)slotNumber.u.bits.FunctionNumber;

    return STATUS_SUCCESS;
}


__drv_functionClass(GET_VIRTUAL_DEVICE_RESOURCES)
_IRQL_requires_same_
__drv_maxIRQL(PASSIVE_LEVEL)
VOID
PciVirtualization_GetResources (
    _Inout_ PVOID Context,
    _Out_ PUINT8 CapturedBusNumbers
    )
/*++

Routine Description:

    This routine retrieves information about the resources available to this
    device related to virtualization.

Parameters:

    Context - Supplies a pointer  to the interface context.  This is actually
        the PDO extension for the device's PF.

    CapturedBusNumbers - Number of busses which can validly be used for Virtual
        Functions.

Return Value:

    None.

--*/
{
    PPCI_DEVICE device;

    PAGED_CODE();

    device = (PPCI_DEVICE)Context;

    *CapturedBusNumbers = device->Parent->CapturedBusNumberCount;

    return;
}


__drv_functionClass(ENABLE_VIRTUALIZATION)
_IRQL_requires_same_
__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
PciVirtualization_EnableVirtualization(
    _Inout_ PVOID Context,
    _In_ UINT16 NumVFs,
    _In_ BOOLEAN EnableVfMigration,
    _In_ BOOLEAN EnableMigrationInterrupt,
    _In_ BOOLEAN EnableVirtualization
    )
/*++

Routine Description:

    This routine turns on virtualization support for a device on or off

    The calling driver is responsible for making sure that dependent functions
    are configured correctly.

Parameters:

    Context - Supplies a pointer  to the interface context.  This is actually
        the PDO extension for the device's PF.

    NumVFs - The number of virtual functions that should be enabled.

    EnableVfMigration - The future state of the VF Migration Enable bit in
        the SR-IOV Control register.  It controls whether an MR-IOV compliant
        device can move resources in and out of this Physical Function.

    EnableMigrationInterrupt - The future state of the VF Migration Interrupt
        Enable bit in the SR-IOV Control register.  It controls whether an
        MR-IOV compliant device will interrupt if the Base Function changes the
        resources allocated to this Physical Function.

    EnableVirtualization - TRUE if virtualization is to be turned on and FALSE
        if it is to be turned off.

Return Value:

    NTSTATUS

--*/
{
    
    PPCI_DEVICE device;
    PEXPRESS_PORT port;
    PCI_EXPRESS_ACS_CONTROL bitState;
    PCI_EXPRESS_SRIOV_CONTROL control;

    PAGED_CODE();

    device = (PPCI_DEVICE)Context;
    port = device->ExpressPort;

    PCI_ASSERT(port->SriovCapability != 0);

    if (device->SriovDeviceDidntGetVfBarSpace != 0) {
        return STATUS_DEVICE_CONFIGURATION_ERROR;
    }

    //
    // Do not allow enabling if not disabled, or disabling if not
    // enabled.  Not only would that mess the enable count for ACS,
    // this also may not work.  E.g. a second enable with a different
    // NumVFs will be ignored by the hardware.
    //
    if (port->SriovEnabled == EnableVirtualization) {
        return STATUS_INVALID_DEVICE_STATE;
    }

    if (EnableVirtualization != FALSE) {

        //
        // Do not allow enabling virtualization with no virtual
        // functions, or with more virtual functions than the device
        // reports.
        //
        if (NumVFs == 0 || NumVFs > port->TotalVFs) {
            return STATUS_INVALID_PARAMETER;
        }

        port->NumVFs = NumVFs;
        PciWriteSriovRegister(port, NumVFs, &(port->NumVFs));

        //
        // Refresh the values of FirstVFOffset and VFStride.  The device may
        // change these value as a result of a change in NumVFs.
        //
        PciReadSriovRegister(port, FirstVFOffset, &port->FirstVFOffset);
        PciReadSriovRegister(port, VFStride, &port->VFStride);

        //
        // Make sure there are enough bus numbers captured to satisfy
        // the last VF.
        //
        if ((port->FirstVFOffset + (NumVFs - 1) * port->VFStride) / 256 >
            device->Parent->CapturedBusNumberCount) {

            return STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    if (EnableVirtualization != FALSE) {
        bitState.AsUSHORT = PciAcsBitsForIov.AsUSHORT; 
    } else {
        bitState.AsUSHORT = 0; 
    }
    PciVirtualizationConfigureAcs(device, PciAcsBitsForIov, bitState);

    PciReadSriovRegister(port, SRIOVControl, &control);

    control.VFEnable = EnableVirtualization ? 1 : 0;
    control.VFMigrationEnable = EnableVfMigration ? 1 : 0;
    control.VFMigrationInterruptEnable = EnableMigrationInterrupt ? 1 : 0;
    PciWriteSriovRegister(port, SRIOVControl, &control);

    if (EnableVirtualization != FALSE) {
        //
        // "To allow components to perform internal initialization,
        // system software must wait for 100 ms after changing
        // the VF Enable bit from a 0 to a 1".  A _DSM can reduce 
        // this delay.
        //

        PciDevice_Delay(device, VFEnableTime);

        //
        // ISSUE:  After the above 100 ms, the device is still allowed
        //         to return a CRS status for additional 900ms.
        //         PCI.SYS currently does not enable CRS software visibility
        //         which means that if a device actually takes that long to
        //         initialize and the device driver does not wait before
        //         attempting to access - the CPU may stall for a long time.
        //
    }

    port->SriovEnabled = EnableVirtualization;

    return STATUS_SUCCESS;
}


NTSTATUS
__drv_functionClass(GET_VIRTUAL_FUNCTION_PROBED_BARS)
__drv_maxIRQL(PASSIVE_LEVEL)
PciVirtualization_GetVirtualFunctionProbedBars(
    _Inout_ PVOID Context,
    _Out_writes_(PCI_TYPE0_ADDRESSES) PULONG BaseRegisterValues
    )
/*++

Routine Description:

    This routine returns the required required resources for virtual
    functions supported by this physical function in the form of
    "probed bars".


    The driver is not allowed to actually probe the
    bars (this is reserved for the PCI bus driver).  Instead the
    values can be deduced from the resources assigned to the device.


Arguments:

    Context - Pointer the interface context.

    BaseRegisterValues - Pointer to location that receives the 6 probed
       BAR values.

Return Value:

    NTSTATUS

--*/
{
    PPCI_DEVICE device;
    PEXPRESS_PORT port;

    PAGED_CODE();

    device = (PPCI_DEVICE)Context;
    port = device->ExpressPort;

    if (port->NumVFs == 0 || port->SriovCapability == 0) {
        return STATUS_INVALID_DEVICE_STATE;
    }

    if (port->EnableSriovDecodes == FALSE) {
        return STATUS_INVALID_DEVICE_STATE;
    }

    RtlCopyMemory(BaseRegisterValues,
                  port->ProbedVfBars,
                  sizeof(ULONG) * PCI_TYPE0_ADDRESSES);

    return STATUS_SUCCESS;
}


LOGICAL
PciIsSriovSupported(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine checks if Sriov is supported by checking if the 
    machine is actually capable of SR-IOV and this is a Hyper-V
    partition and the Sr-IOV drivers are available.
    
Arguments:

    Device - Identifies the device.

Return Value:

    TRUE if Sriov is Supported, FALSE otherwise.

--*/
{
    LOGICAL sriovSupported;

    PAGED_CODE();

    sriovSupported = FALSE;

    //
    // In order to reduce the MMIO space used by devices, and to make it more
    // likely that the device will start with old BIOSes in situations where
    // SR-IOV isn't possible anyway, skip VF BAR claims when the machine isn't
    // actually capable of SR-IOV and/or this isn't a Hyper-V "parent"
    // partition.
    //

    if ((PciSystemWideHackFlags & PCI_SYS_HACK_NO_SRIOV) != 0) {
        goto exit;
    }

    sriovSupported = PciIsSriovDriverAvailable(Device);

exit:

    return sriovSupported;

}

LOGICAL
PciIsSriovDriverAvailable(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine looks under the device's entry in the Enum branch to
    see whether the driver set up registry to signal that it supports
    SR-IOV

Arguments:

    Device - Identifies the device.

Return Value:

    TRUE if the value was found and is non zero, FALSE otherwise.

--*/
{
    NTSTATUS status;
    HANDLE rootHandle;
    PULONG valueBuffer;
    ULONG valueLength;
    LOGICAL sriovDriversAvailable;

    PAGED_CODE();

    rootHandle = NULL;
    valueBuffer = NULL;
    sriovDriversAvailable = FALSE;

    //
    // If the device is not plugged into a PCIe slot, or the device
    // does not have the capability then SR-IOV is not enabled.
    //

    if ((Device->ExpressPort == NULL) ||
        (Device->ExpressPort->SriovCapability == 0)) {

        goto exit;
    }

    //
    // Find the key in the Enum branch that corresponds to this device.
    //
    status = IoOpenDeviceRegistryKey(Device->DeviceObject,
                                     PLUGPLAY_REGKEY_DEVICE,
                                     KEY_ALL_ACCESS,
                                     &rootHandle);

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Under the Pci feature key under this PDO's device registry key,
    // the value should be a REG_DWORD.
    //
    status = PciGetRegistryValue(L"SriovSupported",
                                 PCI_DEVICE_FEATURE_BITS,
                                 rootHandle,
                                 REG_DWORD,
                                 &valueBuffer,
                                 &valueLength
                                 );

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    if (valueLength < sizeof(ULONG)) {
        goto exit;
    }

    if (valueBuffer != NULL && *valueBuffer != 0) {
        sriovDriversAvailable = TRUE;
    }

exit:

    if (valueBuffer) {
        PciFreePool(valueBuffer);
    }

    if (rootHandle) {
        ZwClose(rootHandle);
    }

    return sriovDriversAvailable;
}


VOID
PciEnableSriovDecodes(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine sets the VF MSE bit (VF memory space enable) bit for
    SR-IOV devices.

Arguments:

    Device - Identifies the device.

Return Value:

    None.

--*/
{
    PEXPRESS_PORT port;
    PCI_EXPRESS_SRIOV_CONTROL control;

    NON_PAGED_CODE();

    port = Device->ExpressPort;

    PciReadSriovRegister(port, SRIOVControl, &control);
    control.VFMemorySpaceEnable = 1;
    PciWriteSriovRegister(port, SRIOVControl, &control);

    return;
}


VOID
PciDisableSriovDecodes(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine clears the VF MSE bit (VF memory space enable) bit
    for SR-IOV devices.

Arguments:

    Device - Identifies the device.

Return Value:

    None.

--*/
{
    PEXPRESS_PORT port;
    PCI_EXPRESS_SRIOV_CONTROL control;

    NON_PAGED_CODE();

    port = Device->ExpressPort;

    PciReadSriovRegister(port, SRIOVControl, &control);
    control.VFMemorySpaceEnable = 0;
    PciWriteSriovRegister(port, SRIOVControl, &control);

    return;
}


NTSTATUS
PciVirtualizationCheckAcsSupport(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This function verifies that all downstream switche ports in the path
    to the root port, and the root port itself support ACS.

Arguments:

    Device - Identifies the device for which support is verified.

Return Value:

    Success code if ACS is supported all the way, error code otherwise

--*/
{
    PPCI_BRIDGE Bridge;
    PPCI_BUS Bus;
    PEXPRESS_BRIDGE ExpressBridge;
    NTSTATUS Status;

    PAGED_CODE();

    Status = STATUS_SUCCESS;

    PciAcquirePassiveLock(&PciGlobalLock);

    for (Bus = Device->Parent; Bus != Bus->Root->Bus; Bus = Bus->ParentBus) {

        //
        // Parent busses, including the Root Port are types of Bridges.
        //
        Bridge = Bus->PciBridge;
        PCI_ASSERT(Bridge != NULL);
        if (Bridge == NULL) {
            Status = STATUS_DRIVER_INTERNAL_ERROR;
            break;
        }

        //
        // A bridge that is not an Express bridge will not have ACS
        // and could substiture RIDs which makes it unsuitable for
        // device assignments.
        //
        if (Bridge->ExpressBridge == NULL) {
            TraceEvents(Bus->BlackBoxHandle,
                        TRACE_LEVEL_ERROR,
                        DBG_INIT,
                        "ACS failure because a non-express bridge was an ancestor");
            Status = STATUS_UNSUCCESSFUL;
            break;
        }

        ExpressBridge = Bridge->ExpressBridge;

        //
        // Switch downstream ports must implement ACS
        //
        if (ExpressBridge->BridgeType == PciExpressDownstreamSwitchPort) {
            if (ExpressBridge->AcsCapability == 0) {
                TraceEvents(Bus->BlackBoxHandle,
                            TRACE_LEVEL_ERROR,
                            DBG_INIT,
                            "ACS failure because a downstream port didn't implement ACS");
                Status = STATUS_UNSUCCESSFUL;
                break;
            }

            continue;
        }

        //
        // Root Ports that support peer-to-peer must implement ACS,
        // Root Ports that do not support peer-to-peer have to
        // indicate that ACS is not needed.
        //
        if (ExpressBridge->BridgeType == PciExpressRootPort) {
            if (ExpressBridge->AcsCapability == 0 &&
                ExpressBridge->AcsNotNeeded == FALSE) {

                TraceEvents(Bus->BlackBoxHandle,
                            TRACE_LEVEL_ERROR,
                            DBG_INIT,
                            "ACS failure because a root port didn't implement ACS");
                Status = STATUS_UNSUCCESSFUL;
                break;
            }

            continue;
        }

        //
        // Upstream Switch Ports do not implement ACS.
        //
        if (ExpressBridge->BridgeType == PciExpressUpstreamSwitchPort) {
            continue;
        }

        //
        // Other types of Express bridge (e.g. PciXToExpressBridge)
        // are not supported for virtualization.
        //
        TraceEvents(Bus->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_INIT,
                    "ACS failure because there was an ancestor was an E->X bridge or something similar");
        Status = STATUS_UNSUCCESSFUL;
        break;
    }

    PciReleasePassiveLock(&PciGlobalLock);

    return Status;
}


_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciVirtualizationConfigureAcs(
    _In_ PPCI_DEVICE Device,
    _In_ PCI_EXPRESS_ACS_CONTROL BitsToModify,
    _In_ PCI_EXPRESS_ACS_CONTROL BitState
    )
/*++

Routine Description:

    This function configures ACS on the path from the device to the
    Root Port so that security policies are respected, typically this would
    force all traffic from the device to be routed all the way upstream to an
    I/O MMU.

Arguments:

    Device - Identifies the device.
 
    BitsToModify - Identifies the aspects of ACS which are changed by this
        invocation of this function.

    BitState - The state of the bits that are being changed (as identified by
        BitsToModify).

Return Value:

    None.

--*/
{
    PPCI_BRIDGE Bridge;
    PPCI_BUS Bus;
    PCI_EXPRESS_ACS_CONTROL OldControl, NewControl;
    PEXPRESS_BRIDGE ExpressBridge;
    ULONG BitNum;

    PAGED_CODE();

    if (BitsToModify.AsUSHORT >= 1 << PCI_TRACKED_ACS_BITS)
    {
        TraceEvents(Device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_WARNING,
                    DBG_INIT,
                    "Caller requesting that undefined ACS bits be set: %04x",
                    BitsToModify.AsUSHORT);
        BitsToModify.AsUSHORT &= (1 << PCI_TRACKED_ACS_BITS) - 1;
    }

    PciAcquirePassiveLock(&PciGlobalLock);

    for (Bus = Device->Parent; Bus != Bus->Root->Bus; Bus = Bus->ParentBus) {

        //
        // This function is only called for devices that passed
        // PciVirtualizationCheckAcsSupport.  This means that all
        // bridges on the path are Express Bridges.
        //
        Bridge = Bus->PciBridge;
        PCI_ASSERT(Bridge != NULL);
        PCI_ASSERT(Bridge->ExpressBridge != NULL);
        ExpressBridge = Bridge->ExpressBridge;

        //
        // Upstream Switch Ports do not implement ACS.
        //
        if (ExpressBridge->BridgeType == PciExpressUpstreamSwitchPort) {
            continue;
        }

        //
        // Skip Root Ports if the driver INF indicated that ACS is not
        // needed for that Root Port
        //
        if (ExpressBridge->BridgeType == PciExpressRootPort) {
            if (ExpressBridge->AcsNotNeeded != FALSE) {
                continue;
            }
        }

        //
        // It's tempting to optimize away this register read, since the values
        // that we're reading are inferable from the AcsEnableCounts.  But,
        // just in case the reserved bits end up being used in the future, 
        // read the hardware.
        //
        PciReadExpressBridgeAcsRegister(ExpressBridge,
                                        Control,
                                        &OldControl);

        NewControl = OldControl;

        for (BitNum = 0; BitNum < PCI_TRACKED_ACS_BITS; BitNum++)
        {
            if (BitsToModify.AsUSHORT & 1 << BitNum)
            {
                if (BitState.AsUSHORT & 1 << BitNum) {
                    ++ExpressBridge->AcsEnableCount[BitNum];
                } else {
                    --ExpressBridge->AcsEnableCount[BitNum];
                }

                if (ExpressBridge->AcsEnableCount[BitNum] == 0) {
                    NewControl.AsUSHORT &= ~(1 << BitNum);
                } else {
                    NewControl.AsUSHORT |= (1 << BitNum);
                }
            }
        }

        if (NewControl.AsUSHORT != OldControl.AsUSHORT)
        {
            PciWriteExpressBridgeAcsRegister(ExpressBridge,
                                             Control,
                                             &NewControl);
        }
    }

    PciReleasePassiveLock(&PciGlobalLock);

    return;
}
