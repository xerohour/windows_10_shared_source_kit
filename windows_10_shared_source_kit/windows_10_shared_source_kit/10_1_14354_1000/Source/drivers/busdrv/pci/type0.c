/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    type0.c

Abstract:

    This module contains functions associated with enumerating
    ordinary (PCI Header type 0) devices.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciType0_GetRequirements(
    __inout PPCI_DEVICE Device,
    __out PPCI_DEVICE_REQUIREMENTS Requirements
    );

NTSTATUS
PciType0_GetResources(
    __in PPCI_DEVICE Device,
    __out PPCI_DEVICE_RESOURCES Resources
    );

NTSTATUS
PciType0_SetResources(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_RESOURCES Resources
    );

// --------------------------------------------------------------------- Pragmas
// --------------------------------------------------------------------- Globals

PCI_HEADER_OPERATIONS PciType0HeaderOperations =
{
    PciType0_GetRequirements,
    PciType0_GetResources,
    PciType0_SetResources
};

// ------------------------------------------------------------------- Functions

NTSTATUS
PciType0_GetRequirements(
    __inout PPCI_DEVICE Device,
    __out PPCI_DEVICE_REQUIREMENTS Requirements
    )
/*++

Routine Description:

    This routine gets the device requirements for a type 0 PCI device.
    These requirements include the BARs and the ROM BAR for the device.

Parameters:

    Device - The PCI device to get the requirements of.

    Requirements - Pointer to a structure that will contain the requirements
        on successful return from this routine.

Return Value:

    NT Status code.

--*/
{
    PCI_CONFIG_HEADER configHeader, probedHeader;
    PIO_RESOURCE_DESCRIPTOR requirement;
    NTSTATUS status = STATUS_SUCCESS;
    ULONG barIndex, upperBar;
    BOOLEAN bar64bit = FALSE;

    NON_PAGED_CODE();

    RtlZeroMemory(&Device->Requirements, sizeof(PCI_DEVICE_REQUIREMENTS));

    PciProbeConfigRegister(
        Device,
        type0.BaseAddresses,
        &configHeader.type0.BaseAddresses[0],
        &probedHeader.type0.BaseAddresses[0]
        );

    PciProbeConfigRegister(
        Device,
        type0.ROMBaseAddress,
        &configHeader.type0.ROMBaseAddress,
        &probedHeader.type0.ROMBaseAddress
        );

    //
    // PCI IDE controllers operating in legacy mode implement
    // the first 4 BARs but don't actually use them,... nor are
    // they initialized correctly, and sometimes, nothing will
    // change if we change them,... but we can't read them to determine
    // if they are implemented or not,... so,...
    //
    if (PCI_LEGACY_IDE_CONTROLLER(Device)) {

        barIndex = 4;
    } else {

        barIndex = 0;
    }

    //
    // Loop through the possible BARs, getting the requirement for each.
    //
    for (; barIndex < PCI_TYPE0_BAR_COUNT; barIndex++) {

        //
        // upperBar contains the (potential) upper 32 bits of a 64 bit
        // BAR.  only populate this if we're not on the last BAR, for
        // which there can be no upper 32 bits.
        //
        if (barIndex < (PCI_TYPE0_BAR_COUNT - 1)) {
#pragma prefast(suppress:__WARNING_INDEX_EXCEEDS_MAX)
            upperBar = probedHeader.type0.BaseAddresses[barIndex+1];
        } else {
            upperBar = 0;
        }

        requirement = &Requirements->Bars[barIndex];

        status = PciRequirementFromBar(probedHeader.type0.BaseAddresses[barIndex],
                                       upperBar,
                                       requirement,
                                       &bar64bit
                                       );
        if (!NT_SUCCESS(status)) {
            return status;
        }

        //
        // -- HACK --
        //
        if (PCI_S3_868(Device)) {

            //
            // Check for S3 868 and 968.  These cards report memory
            // requirements of 32MB but decode 64MB.
            //
            if ((requirement->Type == CmResourceTypeMemory) &&
                (requirement->u.Memory.Length == 0x2000000)) {

                requirement->u.Memory.Length = 0x4000000;
            }
        }

        //
        // -- HACK --
        //
        if (PCI_CIRRUS_54XX(Device)) {

            //
            // This device reports an IO requirement of 0x400 ports, in
            // the second BAR.   What it really wants is access to the VGA
            // registers (3b0 thru 3bb and 3c0 thru 3df).  Since this is
            // reported in a BAR, we're likely to NOT assign the actual VGA
            // registers, which will confuse the VGA driver.
            //
            // Solution?  If the second requirement is IO for length 0x400,
            // currently unassigned, don't report it at all.
            //
            if ((barIndex == 1) &&
                (requirement->Type == CmResourceTypePort) &&
                (requirement->u.Port.Length == 0x400)) {

                //
                // Delete the requirement if the device does not have a valid
                // current setting in this BAR.
                //
                if ((configHeader.type0.BaseAddresses[barIndex]
                     & PCI_ADDRESS_IO_ADDRESS_MASK) == 0) {

                    NullResourceRequirement(requirement);

                    TRACE(RESOURCE_TRACE_LEVEL,
                          "Ignored Cirrus GD54xx broken IO requirement (400 ports)\n"
                          );
                }
            }
        }

        if (bar64bit) {

            //
            // Seeing as 64bit bars occupy two indices make sure that we don't
            // run off the end of the bars.
            //
            if ( barIndex != (PCI_TYPE0_BAR_COUNT - 1)) {

                //
                // The 64 bit BAR eats up two BARs, so mark the upper half as
                // not in use.
                //
                barIndex++;
                NullResourceRequirement(&Requirements->Bars[barIndex]);

            } else {
                ERROR("PciType0_GetRequirements: Attempt to reference beyond end of 64-bit BAR.\n");
            }
        }
    }

    //
    // Generate a ROM requirement
    //
    status = PciRequirementFromRomBar(probedHeader.type0.ROMBaseAddress,
                                      configHeader.type0.ROMBaseAddress,
                                      &Requirements->type0.RomBar
                                      );
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = PciGetRequirementsForVfs(Device, Requirements);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    return status;
}

NTSTATUS
PciType0_GetResources(
    __in PPCI_DEVICE Device,
    __out PPCI_DEVICE_RESOURCES Resources
    )
/*++

Routine Description:

    This routine gets the currently decoded resources for a type 0 PCI device.
    These resources include the BARs and the ROM BAR for the device.

Parameters:

    Device - The device to get the resources of.

    Resources - Pointer to a structure that will contain the resources
        on successful return from this routine.

Return Value:

    NT Status code.

--*/
{
    PIO_RESOURCE_DESCRIPTOR barRequirement;
    NTSTATUS status = STATUS_SUCCESS;
    PCI_CONFIG_HEADER configHeader;
    ULONG barIndex, upperBar;

    RtlZeroMemory(Resources, sizeof(PCI_DEVICE_RESOURCES));

    //
    // If the I/O or memory enable bits are not set, the device has
    // no resources that it is currently decoding.
    //
    PciReadConfigRegister(Device, Command, &configHeader.Command);
    if (!(configHeader.Command & (PCI_ENABLE_MEMORY_SPACE | PCI_ENABLE_IO_SPACE))) {
        return STATUS_SUCCESS;
    }

    PciReadConfigRegister(
        Device,
        type0.BaseAddresses,
        &configHeader.type0.BaseAddresses[0]
        );

    PciReadConfigRegister(
        Device,
        type0.ROMBaseAddress,
        &configHeader.type0.ROMBaseAddress
        );

    //
    // Convert the BARs into resource descriptors
    //
    for (barIndex = 0; barIndex < PCI_TYPE0_BAR_COUNT; barIndex++) {

        barRequirement = &Device->Requirements.Bars[barIndex];

        if ((barRequirement->Type == CmResourceTypeNull) ||
            ( (barRequirement->Type == CmResourceTypeMemory ||
               (barRequirement->Type == CmResourceTypeMemoryLarge)) &&
               (!(configHeader.Command & PCI_ENABLE_MEMORY_SPACE))) ||
             ( (barRequirement->Type == CmResourceTypePort) &&
               (!(configHeader.Command & PCI_ENABLE_IO_SPACE)))) {

            //
            // If this BAR is not implemented or not currently decoding, continue
            //
            continue;
        }

        //
        // upperBar contains the (potential) upper 32 bits of a 64 bit
        // BAR.  only populate this if we're not on the last BAR, for
        // which there can be no upper 32 bits.
        //
        if (barIndex < (PCI_TYPE0_BAR_COUNT - 1)) {
#pragma prefast(suppress:__WARNING_INDEX_EXCEEDS_MAX)
            upperBar = configHeader.type0.BaseAddresses[barIndex+1];
        } else {
            upperBar = 0;
        }

        status = PciResourceFromBar(configHeader.type0.BaseAddresses[barIndex],
                                    upperBar,
                                    barRequirement,
                                    &Resources->Bars[barIndex]
                                    );
        if (!NT_SUCCESS(status)) {
            return status;
        }
    }

    if ((configHeader.Command & PCI_ENABLE_MEMORY_SPACE) &&
        (configHeader.type0.ROMBaseAddress & PCI_ROMADDRESS_ENABLED)) {

        status = PciResourceFromRomBar(configHeader.type0.ROMBaseAddress,
                                       &Device->Requirements.type0.RomBar,
                                       &Resources->type0.RomBar
                                       );
        if (!NT_SUCCESS(status)) {
            return status;
        }
    }

    status = PciGetResourcesForVfs(Device, Resources);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    return status;
}

NTSTATUS
PciType0_SetResources(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_RESOURCES Resources
    )
/*++

Routine Description:

    This routine sets the device resources for a type 0 PCI device.
    These resources include the BARs and the ROM BAR for the device.
    It sets these resources both into the hardware and into the given
    config header (so that the updated config header can be saved away).

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
    PCI_CONFIG_HEADER configHeader;
    ULONG barIndex;
    USHORT decode;
    ULONG romBar;
    NTSTATUS status;

    NON_PAGED_CODE();

    //
    // Extract the data from the BARs and write it out to hardware
    //
    for (barIndex = 0; barIndex < PCI_TYPE0_BAR_COUNT; barIndex++) {

        resource = &Resources->Bars[barIndex];
        requirement = &Device->Requirements.Bars[barIndex];

        //
        // Typically the resource type and requirement type need to match.
        // The only exception is if the resource has been filtered out, and
        // therefore is CmResourceTypeNull.  In this case, we better have
        // decided to turn off the decode for this resource type, because
        // otherwise the BAR will contain a bogus value, since none was assigned.
        //
        if (resource->Type != requirement->Type &&
            (!(resource->Type == CmResourceTypeMemoryLarge && requirement->Type == CmResourceTypeMemory)) &&
            (!(resource->Type == CmResourceTypeMemory && requirement->Type == CmResourceTypeMemoryLarge))) {

            if (requirement->Type == CmResourceTypePort) {
                decode = PCI_ENABLE_IO_SPACE;
            } else if ((requirement->Type == CmResourceTypeMemory) ||
                        (requirement->Type == CmResourceTypeMemoryLarge)) {
                decode = PCI_ENABLE_MEMORY_SPACE;
            } else {
                decode = 0;
            }

            //
            // N.B. Adding a check for VGA devices below for TH Bug 3762646:
            // As UEFI spec discourages the use of IO resources, the new AMD GOP
            // driver will disable IO decoding for the VGA device (setting the
            // command register to be 0x6). And this value (0x6) will be kept in
            // the PossibleDecodes field in the device extension and due to a hack
            // defined in hack.c line 1059, it will be preserved across S3/S4 transition
            // which causes IO to be disabled all the time. However, as there are
            // IO resources in the BAR, this causes a mismatch between the command
            // register and the actual resources probed from BAR. Then, when the
            // machine resumes from hibernate, the resource list of the VGA device
            // will be rebuilt, but as IO is turned off in the command register,
            // IO resources will be filtered out from the list. This leads to
            // the mismatch between the resource and requirement list thus causing
            // the code to go into this failure case. In order to handle this,
            // adding a check for VGA devices here so the rest of the BARs can
            // still be restored.
            //

            if ((resource->Type != CmResourceTypeNull) ||
                ((Device->ResourceDecodes & decode) &&
                 (PCI_VGA_CONTROLLER(Device) == FALSE))) {

                ERROR("PciType0_SetResources: Illegal resource filtering.\n");
                return STATUS_INVALID_PARAMETER;
            }
        }

        if (resource->Type == CmResourceTypeNull) {

            configHeader.type0.BaseAddresses[barIndex] = 0;
            continue;
        }

        if (resource->Type == CmResourceTypeMemory ||
            resource->Type == CmResourceTypeMemoryLarge) {

            status = PciResizableBarSetSize(Device, barIndex, requirement);
            if (!NT_SUCCESS(status)) {
                return status;
            }
        }

        configHeader.type0.BaseAddresses[barIndex] = resource->u.Generic.Start.LowPart;

        //
        // If this is a 64bit bar populate the next bar with the upper bits and
        // advance index
        //
        if ((ULONGLONG)requirement->u.Generic.MaximumAddress.QuadPart > MAXULONG) {
            barIndex++;

            if (barIndex == PCI_TYPE0_BAR_COUNT) {
                ERROR("PciType0_SetResources: Attempt to reference beyond end of 64-BIT BAR.\n");
                return STATUS_INVALID_PARAMETER;
            }

            configHeader.type0.BaseAddresses[barIndex] = resource->u.Generic.Start.HighPart;

        } else {

            if (resource->u.Generic.Start.HighPart != 0) {
                ERROR("PciType0_SetResources: invalid resource paramenter.\n");
                return STATUS_INVALID_PARAMETER;
            }
        }
    }

    PciWriteConfigRegister(Device, type0.BaseAddresses, configHeader.type0.BaseAddresses);

    //
    // Configure the ROM bar
    //
    resource = &Resources->type0.RomBar;

    if (resource->Type == CmResourceTypeNull) {

        romBar = 0;
    } else {
        romBar = resource->u.Generic.Start.LowPart;
    }

    PciWriteConfigRegister(Device, type0.ROMBaseAddress, &romBar);

    status = PciSetResourcesForVfs(Device, Resources);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    return STATUS_SUCCESS;
}
