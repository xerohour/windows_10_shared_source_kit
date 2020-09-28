/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    type2.c

Abstract:

    This module contains functions for munging type 2 config headers.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciType2_GetRequirements(
    __in PPCI_DEVICE Device,
    __out PPCI_DEVICE_REQUIREMENTS Requirements
    );

NTSTATUS
PciType2_GetResources(
    __in PPCI_DEVICE Device,
    __out PPCI_DEVICE_RESOURCES Resources
    );

NTSTATUS
PciType2_SetResources(
    __inout PPCI_DEVICE Device,
    __in PPCI_DEVICE_RESOURCES Resources
    );

// --------------------------------------------------------------------- Pragmas
// --------------------------------------------------------------------- Globals

PCI_HEADER_OPERATIONS PciType2HeaderOperations =
{
    PciType2_GetRequirements,
    PciType2_GetResources,
    PciType2_SetResources
};

// ------------------------------------------------------------------- Functions

NTSTATUS
PciType2_GetRequirements(
    __in PPCI_DEVICE Device,
    __out PPCI_DEVICE_REQUIREMENTS Requirements
    )
/*++

Routine Description:

    This routine gets the device requirements for a type 2 PCI device.
    These requirements include the BARs and windows for the device.

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
    ULONG windowBase, windowLimit, probedLimit;
    ULONG windowIndex;
    ULONG flags;

    NON_PAGED_CODE();

    RtlZeroMemory(Requirements, sizeof(PCI_DEVICE_REQUIREMENTS));

    PciProbeConfigRegister(
        Device,
        type2.SocketRegistersBaseAddress,
        &configHeader.type2.SocketRegistersBaseAddress,
        &probedHeader.type2.SocketRegistersBaseAddress
        );

    PciProbeDeviceConfig(
        Device,
        &configHeader.type2.MemoryBase0,
        &probedHeader.type2.MemoryBase0,
        CONFIG_FIELD_OFFSET(type2.MemoryBase0),
        CONFIG_FIELD_LENGTH(type2.MemoryBase0)
         + CONFIG_FIELD_LENGTH(type2.MemoryLimit0)
         + CONFIG_FIELD_LENGTH(type2.MemoryBase1)
         + CONFIG_FIELD_LENGTH(type2.MemoryLimit1)
         + CONFIG_FIELD_LENGTH(type2.IoBase0)
         + CONFIG_FIELD_LENGTH(type2.IoLimit0)
         + CONFIG_FIELD_LENGTH(type2.IoBase1)
         + CONFIG_FIELD_LENGTH(type2.IoLimit1)
        );

    //
    // Get the BAR requirement
    //
    status = PciRequirementFromBar(probedHeader.type2.SocketRegistersBaseAddress,
                                   0, // no upper 32 bits of a socket BAR
                                   &Requirements->type2.SocketBar,
                                   NULL  // can't be 64 bit
                                   );
    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Get the requirements for the bridge windows.
    //

    //
    // There are two memory windows in a cardbus controller
    //
    for (windowIndex = 0; windowIndex < 2; windowIndex++) {

        if (windowIndex == 0) {

            requirement = &Requirements->type2.MemoryWindows[0];
            windowBase = configHeader.type2.MemoryBase0;
            windowLimit = configHeader.type2.MemoryLimit0;
            probedLimit = probedHeader.type2.MemoryLimit0;

        } else {

            requirement = &Requirements->type2.MemoryWindows[1];
            windowBase = configHeader.type2.MemoryBase1;
            windowLimit = configHeader.type2.MemoryLimit1;
            probedLimit = probedHeader.type2.MemoryLimit1;
        }

        //
        // The bottom bits of a memory window are required to be 0.  Verify this.
        //
        PCI_ASSERT((windowBase & PCI_TYPE2_MEMORY_WINDOW_ADDRESS_MASK) == windowBase);
        PCI_ASSERT((windowLimit & PCI_TYPE2_MEMORY_WINDOW_ADDRESS_MASK) == windowLimit);

        if (probedLimit == 0) {

            //
            // The window is unimplemented.  Unimplemented memory windows
            // are illegal.
            //
            ERROR("PciType2_GetRequirements: Unimplemented memory windows.\n");
            NullResourceRequirement(requirement);

        } else {

            requirement->Type = CmResourceTypeMemory;
            requirement->ShareDisposition = CmResourceShareDeviceExclusive;
            requirement->Flags = CM_RESOURCE_MEMORY_READ_WRITE
                                  | CM_RESOURCE_MEMORY_WINDOW_DECODE;
            requirement->u.Memory.Alignment = PCI_TYPE2_MEMORY_WINDOW_ALIGNMENT;
            requirement->u.Memory.MinimumAddress.QuadPart = 0;
            requirement->u.Memory.MaximumAddress.QuadPart = MAXULONG;

            if ((windowBase > windowLimit) &&
                !(PciSystemWideHackFlags &
                  PCI_SYS_HACK_IGNORE_CB_MEMORY_WINDOW_LIMITS) &&
                !(Device->HackFlags & PCI_HACK_DEFAULT_CARDBUS_WINDOWS)) {

                requirement->u.Memory.Length = 0;

            } else {

                //
                // Create a default requirement of the minimum window size
                // allowed.  This can be overridden by the FDO.
                //
                requirement->u.Memory.Length = PCI_TYPE2_MEMORY_WINDOW_ALIGNMENT;
            }

            //
            // Cardbus controller memory windows can be configured
            // prefetchable or not.  Always make the second one
            // prefetchable and the first not, for simplicity.
            //
            if (windowIndex == 1) {
                requirement->Flags |= CM_RESOURCE_MEMORY_PREFETCHABLE;
            }
        }
    }

    //
    // There are two I/O windows in a cardbus controller
    //
    for (windowIndex = 0; windowIndex < 2; windowIndex++) {

        if (windowIndex == 0) {

            requirement = &Requirements->type2.IoWindows[0];
            windowBase = configHeader.type2.IoBase0;
            windowLimit = configHeader.type2.IoLimit0;
            probedLimit = probedHeader.type2.IoLimit0;

        } else {

            requirement = &Requirements->type2.IoWindows[1];
            windowBase = configHeader.type2.IoBase1;
            windowLimit = configHeader.type2.IoLimit1;
            probedLimit = probedHeader.type2.IoLimit1;
        }

        //
        // Flags are stored in the lower bits of the base/limit registers.
        // They should be the same for both base and limit.  If the flags are
        // inconsistent, throw away the flags.
        //
        if (Device->HackFlags & PCI_HACK_16BIT_CB_IO_WINDOW) {

            flags = 0;

        } else {

            if ((windowBase & ~PCI_TYPE2_IO_WINDOW_ADDRESS_MASK) !=
                (windowLimit & ~PCI_TYPE2_IO_WINDOW_ADDRESS_MASK)) {

                ERROR("PciType2_GetRequirements: Inconsistent IO Base and Limit flags for (%x,%x,%x)\n",
                      Device->BusNumber,
                      Device->Slot.u.bits.DeviceNumber,
                      Device->Slot.u.bits.FunctionNumber);

                flags = 0;

            } else {
                flags = windowBase & ~PCI_TYPE2_IO_WINDOW_ADDRESS_MASK;
            }
        }

        windowBase &= PCI_TYPE2_IO_WINDOW_ADDRESS_MASK;
        windowLimit |= ~PCI_TYPE2_IO_WINDOW_ADDRESS_MASK;

        if (((probedLimit & PCI_TYPE2_IO_WINDOW_ADDRESS_MASK) == 0) ||
            (windowIndex == 1)) {

            //
            // The window isn't implemented. For now always consider
            // the second I/O window unimplemented, for simplicity.
            //
            NullResourceRequirement(requirement);

        } else {

            requirement->Type = CmResourceTypePort;
            requirement->ShareDisposition = CmResourceShareDeviceExclusive;
            requirement->Flags = CM_RESOURCE_PORT_IO
                                  | CM_RESOURCE_PORT_POSITIVE_DECODE
                                  | CM_RESOURCE_PORT_WINDOW_DECODE;
            requirement->u.Port.Alignment = PCI_TYPE2_IO_WINDOW_ALIGNMENT;
            requirement->u.Port.MinimumAddress.QuadPart = 0;

            if (flags == 0x1) {

                //
                // The window dows 32 bit decode.
                //
                requirement->u.Port.MaximumAddress.QuadPart = 0xffffffff;

            } else {

                //
                // 16 bit decode.
                //
                PCI_ASSERT(flags == 0);

                requirement->u.Port.MaximumAddress.QuadPart = 0xffff;
            }

            if ((windowBase > windowLimit) &&
                !(Device->HackFlags & PCI_HACK_DEFAULT_CARDBUS_WINDOWS)) {

                //
                // The window has been explicitly disabled by the BIOS.
                // Create a default 0 length requirement.
                //
                requirement->u.Port.Length = 0;

            } else {

                //
                // Create a default requirement of the minimum window size
                // allowed.  This can be overridden by the FDO.
                //
                requirement->u.Port.Length = PCI_TYPE2_IO_WINDOW_ALIGNMENT;
            }
        }
    }

    return status;
}

NTSTATUS
PciType2_GetResources(
    __in PPCI_DEVICE Device,
    __out PPCI_DEVICE_RESOURCES Resources
    )
/*++

Routine Description:

    This routine gets the currently decoded resources for a type 2 PCI device.
    These resources include the BARs and windows for the device.

Parameters:

    Device - The PCI device to get the resources of.

    Resources - Pointer to a structure that will contain the resources
        on successful return from this routine.

Return Value:

    NT Status code.

--*/
{
    PCM_PARTIAL_RESOURCE_DESCRIPTOR resource;
    PIO_RESOURCE_DESCRIPTOR requirement;
    PCI_CONFIG_HEADER configHeader;
    NTSTATUS status = STATUS_SUCCESS;
    ULONG windowBase, windowLimit, probedLimit;
    ULONG windowIndex;

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
        type2.SocketRegistersBaseAddress,
        &configHeader.type2.SocketRegistersBaseAddress
        );

    PciReadDeviceConfig(
        Device,
        &configHeader.type2.MemoryBase0,
        CONFIG_FIELD_OFFSET(type2.MemoryBase0),
        CONFIG_FIELD_LENGTH(type2.MemoryBase0)
         + CONFIG_FIELD_LENGTH(type2.MemoryLimit0)
         + CONFIG_FIELD_LENGTH(type2.MemoryBase1)
         + CONFIG_FIELD_LENGTH(type2.MemoryLimit1)
         + CONFIG_FIELD_LENGTH(type2.IoBase0)
         + CONFIG_FIELD_LENGTH(type2.IoLimit0)
         + CONFIG_FIELD_LENGTH(type2.IoBase1)
         + CONFIG_FIELD_LENGTH(type2.IoLimit1)
        );

    //
    // Get the BAR resource
    //
    status = PciResourceFromBar(configHeader.type2.SocketRegistersBaseAddress,
                                0, // no upper 32 bits of a socket BAR
                                &Device->Requirements.type2.SocketBar,
                                &Resources->type2.SocketBar
                                );
    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // There are two memory windows in a cardbus controller
    //
    for (windowIndex = 0; windowIndex < 2; windowIndex++) {

        if (windowIndex == 0) {

            requirement = &Device->Requirements.type2.MemoryWindows[0];
            resource = &Resources->type2.MemoryWindows[0];
            windowBase = configHeader.type2.MemoryBase0;
            windowLimit = configHeader.type2.MemoryLimit0;

        } else {

            requirement = &Device->Requirements.type2.MemoryWindows[1];
            resource = &Resources->type2.MemoryWindows[1];
            windowBase = configHeader.type2.MemoryBase1;
            windowLimit = configHeader.type2.MemoryLimit1;
        }

        //
        // The bottom bits of a memory window are required to be 0.  Verify this.
        //
        PCI_ASSERT((windowBase & PCI_TYPE2_MEMORY_WINDOW_ADDRESS_MASK) == windowBase);
        PCI_ASSERT((windowLimit & PCI_TYPE2_MEMORY_WINDOW_ADDRESS_MASK) == windowLimit);

        if ((requirement->Type == CmResourceTypeNull) ||
            (!(configHeader.Command & PCI_ENABLE_MEMORY_SPACE)) ||
            (windowBase == 0) ||
            (Device->HackFlags & PCI_HACK_DEFAULT_CARDBUS_WINDOWS) ||
            ((windowBase > windowLimit) &&
             !(PciSystemWideHackFlags &
               PCI_SYS_HACK_IGNORE_CB_MEMORY_WINDOW_LIMITS))) {

            //
            // The window doesn't have a current resource if it is unimplemented,
            // the memory decodes are off, the base address is 0, a hack flag
            // is defined to ignore boot configs or the window is explicitly
            // disabled (base > limit).
            //
            resource->Type = CmResourceTypeNull;

        } else {

            PCI_ASSERT(requirement->Type == CmResourceTypeMemory);

            resource->Type = CmResourceTypeMemory;
            resource->ShareDisposition = requirement->ShareDisposition;
            resource->Flags = requirement->Flags;
            windowBase &= PCI_TYPE2_MEMORY_WINDOW_ADDRESS_MASK;
            resource->u.Port.Start.QuadPart = windowBase;
            if ((windowBase > windowLimit) &&
                (PciSystemWideHackFlags & PCI_SYS_HACK_IGNORE_CB_MEMORY_WINDOW_LIMITS)) {

                if (windowIndex == 0) {
                    PciProbeDeviceConfig(
                        Device,
                        &windowLimit,
                        &probedLimit,
                        CONFIG_FIELD_OFFSET(type2.MemoryLimit0),
                        CONFIG_FIELD_LENGTH(type2.MemoryLimit0));

                } else {
                    PciProbeDeviceConfig(
                        Device,
                        &windowLimit,
                        &probedLimit,
                        CONFIG_FIELD_OFFSET(type2.MemoryLimit1),
                        CONFIG_FIELD_LENGTH(type2.MemoryLimit1));

                }

                probedLimit |= ~PCI_TYPE2_MEMORY_WINDOW_ADDRESS_MASK;
                resource->u.Port.Length = probedLimit - windowBase + 1;

            } else {
                windowLimit |= ~PCI_TYPE2_MEMORY_WINDOW_ADDRESS_MASK;
                resource->u.Port.Length = windowLimit - windowBase + 1;
            }
        }
    }

    //
    // There are two I/O windows in a cardbus controller
    //
    for (windowIndex = 0; windowIndex < 2; windowIndex++) {

        if (windowIndex == 0) {

            requirement = &Device->Requirements.type2.IoWindows[0];
            resource = &Resources->type2.IoWindows[0];
            windowBase = configHeader.type2.IoBase0;
            windowLimit = configHeader.type2.IoLimit0;

        } else {

            requirement = &Device->Requirements.type2.IoWindows[1];
            resource = &Resources->type2.IoWindows[1];
            windowBase = configHeader.type2.IoBase1;
            windowLimit = configHeader.type2.IoLimit1;
        }

        windowBase &= PCI_TYPE2_IO_WINDOW_ADDRESS_MASK;
        windowLimit |= ~PCI_TYPE2_IO_WINDOW_ADDRESS_MASK;

        if ((requirement->Type == CmResourceTypeNull) ||
            ((configHeader.Command & PCI_ENABLE_IO_SPACE) == 0) ||
            (windowBase == 0) ||
            (Device->HackFlags & PCI_HACK_DEFAULT_CARDBUS_WINDOWS) ||
            (windowBase > windowLimit)) {

            //
            // The window doesn't have a current resource if it is unimplemented,
            // the i/o decodes are off, the base address is 0, or the window
            // has been explicitly disabled (base > limit).
            //
            resource->Type = CmResourceTypeNull;

        } else {

            resource->Type = CmResourceTypePort;
            resource->ShareDisposition = requirement->ShareDisposition;
            resource->Flags = requirement->Flags;

            resource->u.Port.Start.QuadPart = windowBase;
            resource->u.Port.Length = windowLimit - windowBase + 1;
        }
    }

    return status;
}

NTSTATUS
PciType2_SetResources(
    __inout PPCI_DEVICE Device,
    __in PPCI_DEVICE_RESOURCES Resources
    )
/*++

Routine Description:

    This routine sets the device resources for a type 2 PCI device.
    These resources include the BARs and windows for the device.
    It sets these resources both into the hardware and into the given
    config header (so that the updated config header can be saved away).

Parameters:

    Device - The PCI device to set the resources of.

    Resources - Pointer to a structure that contains the resources
        to set.

Return Value:

    NT Status code.

--*/
#pragma push
#pragma prefast(disable: __WARNING_UNUSED_POINTER_ASSIGNMENT, "asserts")
{
    ULONG windowBase, windowLimit;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR resource;
    PIO_RESOURCE_DESCRIPTOR requirement;
    PCI_CONFIG_HEADER configHeader;
    USHORT decode;

    NON_PAGED_CODE();

    //
    // Extract the data from the socket BAR and write it out to hardware
    //
    resource = &Resources->type2.SocketBar;
    requirement = &Device->Requirements.type2.SocketBar;

    //
    // Typically the resource type and requirement type need to match.
    // The only exception is if the resource has been filtered out, and
    // therefore is CmResourceTypeNull.  In this case, we better have
    // decided to turn off the decode for this resource type, because
    // otherwise the BAR will contain a bogus value, since none was assigned.
    //
    if (resource->Type != requirement->Type) {

        if (requirement->Type == CmResourceTypePort) {
            decode = PCI_ENABLE_IO_SPACE;
        } else if (requirement->Type == CmResourceTypeMemory) {
            decode = PCI_ENABLE_MEMORY_SPACE;
        } else {
            decode = 0;
        }

        if ((resource->Type != CmResourceTypeNull) ||
            (Device->ResourceDecodes & decode)) {

            return STATUS_INVALID_PARAMETER;
        }
    }

    if (resource->Type == CmResourceTypeNull) {

        configHeader.type2.SocketRegistersBaseAddress = 0;

    } else {

        if (resource->u.Generic.Start.HighPart != 0) {
            ERROR("PciType2_SetResources: invalid generic resource.\n");
            return STATUS_INVALID_PARAMETER;
        }
        configHeader.type2.SocketRegistersBaseAddress = resource->u.Generic.Start.LowPart;
    }

    PciWriteConfigRegister(Device,
                           type2.SocketRegistersBaseAddress,
                           &configHeader.type2.SocketRegistersBaseAddress
                           );

    //
    // The legacy mode BAR is always zero.
    //
    configHeader.type2.LegacyModeBaseAddress = 0;
    PciWriteConfigRegister(Device,
                           type2.LegacyModeBaseAddress,
                           &configHeader.type2.LegacyModeBaseAddress
                           );

    //
    // Initialize the windows to be closed.  If we have a window
    // resource for a given window, this will be overridden below.
    //
    configHeader.type2.MemoryBase0 = 0xffffffff;
    configHeader.type2.MemoryBase1 = 0xffffffff;
    configHeader.type2.IoBase0 = 0xffffffff;
    configHeader.type2.IoBase1 = 0xffffffff;
    configHeader.type2.MemoryLimit0 = 0;
    configHeader.type2.MemoryLimit1 = 0;
    configHeader.type2.IoLimit0 = 0;
    configHeader.type2.IoLimit1 = 0;

    //
    // There are two memory windows in a cardbus controller
    //
    requirement = &Device->Requirements.type2.MemoryWindows[0];
    resource = &Resources->type2.MemoryWindows[0];

    if (resource->Type != CmResourceTypeNull) {

        windowBase = resource->u.Generic.Start.LowPart & PCI_TYPE2_MEMORY_WINDOW_ADDRESS_MASK;
        windowLimit = windowBase + resource->u.Generic.Length - 1;

        PCI_ASSERT(windowBase == resource->u.Generic.Start.LowPart);
        PCI_ASSERT(resource->u.Generic.Start.HighPart == 0);

        configHeader.type2.MemoryBase0 = windowBase;
        configHeader.type2.MemoryLimit0 = windowLimit;

        if (resource->Flags & CM_RESOURCE_MEMORY_PREFETCHABLE) {

            Device->SavedRegisters.bridge.BridgeControl |= PCI_ENABLE_CARDBUS_MEM0_PREFETCH;

        } else {

            Device->SavedRegisters.bridge.BridgeControl &= ~PCI_ENABLE_CARDBUS_MEM0_PREFETCH;
        }
    }

    requirement = &Device->Requirements.type2.MemoryWindows[1];
    resource = &Resources->type2.MemoryWindows[1];

    if (resource->Type != CmResourceTypeNull) {

        windowBase = resource->u.Generic.Start.LowPart & PCI_TYPE2_MEMORY_WINDOW_ADDRESS_MASK;
        windowLimit = windowBase + resource->u.Generic.Length - 1;

        PCI_ASSERT(windowBase == resource->u.Generic.Start.LowPart);
        PCI_ASSERT(resource->u.Generic.Start.HighPart == 0);

        configHeader.type2.MemoryBase1 = windowBase;
        configHeader.type2.MemoryLimit1 = windowLimit;

        if (resource->Flags & CM_RESOURCE_MEMORY_PREFETCHABLE) {

            Device->SavedRegisters.bridge.BridgeControl |= PCI_ENABLE_CARDBUS_MEM1_PREFETCH;

        } else {

            Device->SavedRegisters.bridge.BridgeControl &= ~PCI_ENABLE_CARDBUS_MEM1_PREFETCH;
        }
    }

    //
    // There are two I/O windows in a cardbus controller
    //
    requirement = &Device->Requirements.type2.IoWindows[0];
    resource = &Resources->type2.IoWindows[0];

    if (resource->Type != CmResourceTypeNull) {

        windowBase = resource->u.Generic.Start.LowPart & PCI_TYPE2_IO_WINDOW_ADDRESS_MASK;
        windowLimit = windowBase + resource->u.Generic.Length - 1;

        PCI_ASSERT(windowBase == resource->u.Generic.Start.LowPart);
        PCI_ASSERT(resource->u.Generic.Start.HighPart == 0);
        PCI_ASSERT(resource->u.Generic.Start.LowPart <
                   (ULONGLONG)requirement->u.Generic.MaximumAddress.QuadPart);

        configHeader.type2.IoBase0 = windowBase;
        configHeader.type2.IoLimit0 = windowLimit;
    }

    requirement = &Device->Requirements.type2.IoWindows[1];
    resource = &Resources->type2.IoWindows[1];

    if (resource->Type != CmResourceTypeNull) {

        windowBase = resource->u.Generic.Start.LowPart & PCI_TYPE2_IO_WINDOW_ADDRESS_MASK;
        windowLimit = windowBase + resource->u.Generic.Length - 1;

        PCI_ASSERT(windowBase == resource->u.Generic.Start.LowPart);
        PCI_ASSERT(resource->u.Generic.Start.HighPart == 0);
        PCI_ASSERT(resource->u.Generic.Start.LowPart <
                   (ULONGLONG)requirement->u.Generic.MaximumAddress.QuadPart);

        configHeader.type2.IoBase1 = windowBase;
        configHeader.type2.IoLimit1 = windowLimit;
    }

    PciWriteDeviceConfig(Device,
                         &configHeader.type2.MemoryBase0,
                         CONFIG_FIELD_OFFSET(type2.MemoryBase0),
                         CONFIG_FIELD_LENGTH(type2.MemoryBase0)
                          + CONFIG_FIELD_LENGTH(type2.MemoryLimit0)
                          + CONFIG_FIELD_LENGTH(type2.MemoryBase1)
                          + CONFIG_FIELD_LENGTH(type2.MemoryLimit1)
                          + CONFIG_FIELD_LENGTH(type2.IoBase0)
                          + CONFIG_FIELD_LENGTH(type2.IoLimit0)
                          + CONFIG_FIELD_LENGTH(type2.IoBase1)
                          + CONFIG_FIELD_LENGTH(type2.IoLimit1)
                         );

    return STATUS_SUCCESS;
}
#pragma pop
