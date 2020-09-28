/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    type1.c

Abstract:

    This module contains functions associated with enumerating
    PCI to PCI bridges.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines

#define PCI_BUGCODE_MEMORY_REGION_TOO_LARGE     0xbaddeed1

#define PCI_IGNORE_DISABLED_BRIDGE_WINDOW(_Device_)                 \
    (((_Device_)->ExpressCapability != 0) &&                        \
     ((PciSystemWideHackFlags &                                     \
      PCI_SYS_HACK_IGNORE_DISABLED_EXPRESS_BRIDGE_WINDOW) != 0))


// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciType1_GetRequirements(
    __inout PPCI_DEVICE Device,
    __out PPCI_DEVICE_REQUIREMENTS Requirements
    );

NTSTATUS
PciType1_GetResources(
    __in PPCI_DEVICE Device,
    __out PPCI_DEVICE_RESOURCES Resources
    );

NTSTATUS
PciType1_SetResources(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_RESOURCES Resources
    );

ULONG
PciType1_IoBase(
    __in PPCI_CONFIG_HEADER ConfigHeader
    );

ULONG
PciType1_IoLimit(
    __in PPCI_CONFIG_HEADER ConfigHeader
    );

ULONG
PciType1_MemoryBase(
    __in PPCI_CONFIG_HEADER ConfigHeader
    );

ULONG
PciType1_MemoryLimit(
    __in PPCI_CONFIG_HEADER ConfigHeader
    );

PHYSICAL_ADDRESS
PciType1_PrefetchMemoryBase(
    __in PPCI_CONFIG_HEADER ConfigHeader
    );

PHYSICAL_ADDRESS
PciType1_PrefetchMemoryLimit(
    __in PPCI_CONFIG_HEADER ConfigHeader
    );

// --------------------------------------------------------------------- Pragmas
// --------------------------------------------------------------------- Globals

PCI_HEADER_OPERATIONS PciType1HeaderOperations =
{
    PciType1_GetRequirements,
    PciType1_GetResources,
    PciType1_SetResources
};

// ------------------------------------------------------------------- Functions

NTSTATUS
PciType1_GetRequirements(
    __inout PPCI_DEVICE Device,
    __out PPCI_DEVICE_REQUIREMENTS Requirements
    )
/*++

Routine Description:

    This routine gets the device requirements for a type 1 PCI device.
    These requirements include the BARs, windows and ROM BAR for the device.

Parameters:

    Device - The PCI device to get the requirements of.

    Requirements - Pointer to a structure that will contain the requirements
        on successful return from this routine.

Return Value:

    NT Status code.

--*/
{
    PHYSICAL_ADDRESS prefetchBase, prefetchLimit, probedPrefetchLimit;
    PCI_CONFIG_HEADER configHeader, probedHeader;
    ULONG windowBase, windowLimit, probedLimit;
    PIO_RESOURCE_DESCRIPTOR requirement;
    ULONG barIndex, upperBar;
    NTSTATUS status;
    BOOLEAN bar64bit;

    NON_PAGED_CODE();

    RtlZeroMemory(&Device->Requirements, sizeof(PCI_DEVICE_REQUIREMENTS));

    status = STATUS_SUCCESS;

    PciProbeConfigRegister(
        Device,
        type1.BaseAddresses,
        &configHeader.type1.BaseAddresses[0],
        &probedHeader.type1.BaseAddresses[0]
        );

    PciProbeDeviceConfig(
        Device,
        &configHeader.type1.IOBase,
        &probedHeader.type1.IOBase,
        CONFIG_FIELD_OFFSET(type1.IOBase),
        CONFIG_FIELD_LENGTH(type1.IOBase) + CONFIG_FIELD_LENGTH(type1.IOLimit)
        );

    PciProbeDeviceConfig(
        Device,
        &configHeader.type1.MemoryBase,
        &probedHeader.type1.MemoryBase,
        CONFIG_FIELD_OFFSET(type1.MemoryBase),
        CONFIG_FIELD_LENGTH(type1.MemoryBase)
         + CONFIG_FIELD_LENGTH(type1.MemoryLimit)
         + CONFIG_FIELD_LENGTH(type1.PrefetchBase)
         + CONFIG_FIELD_LENGTH(type1.PrefetchLimit)
         + CONFIG_FIELD_LENGTH(type1.PrefetchBaseUpper32)
         + CONFIG_FIELD_LENGTH(type1.PrefetchLimitUpper32)
         + CONFIG_FIELD_LENGTH(type1.IOBaseUpper16)
         + CONFIG_FIELD_LENGTH(type1.IOLimitUpper16)
        );

    PciProbeConfigRegister(
        Device,
        type1.ROMBaseAddress,
        &configHeader.type1.ROMBaseAddress,
        &probedHeader.type1.ROMBaseAddress
        );

    //
    // Get the BAR requirements
    //
    for (barIndex = 0; barIndex < PCI_TYPE1_BAR_COUNT; barIndex++) {

        if (barIndex < (PCI_TYPE1_BAR_COUNT - 1)) {
            upperBar = probedHeader.type0.BaseAddresses[barIndex+1];
        } else {
            upperBar = 0;
        }

        status = PciRequirementFromBar(probedHeader.type1.BaseAddresses[barIndex],
                                       upperBar,
                                       &Requirements->Bars[barIndex],
                                       &bar64bit
                                       );
        if (!NT_SUCCESS(status)) {
            return status;
        }

        if (bar64bit) {

            //
            // Seeing as 64bit bars occupy two indices make sure that we don't
            // run off the end of the bars.
            //
            PCI_ASSERT(barIndex != (PCI_TYPE1_BAR_COUNT - 1));

            barIndex++;
            NullResourceRequirement(&Requirements->Bars[barIndex]);
        }
    }

    //
    // PCI_HACK_BRIDGE_WINDOW_ALIGNMENT - This hack flag is defined to
    // set the alignment for a bridge window equal to the preferred length.
    // This is to workaround a BIOS issue for VGA path (that cannot be
    // rebalanced yet). This hack flag provides a one time opportunity
    // to move the bridge window such that it will be able to arbitrate
    // requirements for a device beneath it. This workaround is applicable
    // only if the BIOS misconfigured both the bridge window and the VGA
    // device.
    //
    // N.B. If a rebalance of VGA path is correctly implemented, then this
    //      hack will no longer be required.
    //

    //
    // I/O window
    //

    requirement = &Requirements->type1.IoWindow;

    windowBase = PciType1_IoBase(&configHeader);
    windowLimit = PciType1_IoLimit(&configHeader);
    probedLimit = PciType1_IoLimit(&probedHeader);

#if defined(_ARM_) || defined(_ARM64_)

    //
    // On ARM platforms disallow all I/O windows.
    //

    probedLimit = 0;

#endif

    if ((probedLimit & PCI_TYPE1_IO_WINDOW_ADDRESS_MASK) == 0) {

        //
        // The window is not implemented.
        //
        NullResourceRequirement(requirement);

    } else {

        requirement->Type = CmResourceTypePort;
        requirement->ShareDisposition = CmResourceShareDeviceExclusive;
        requirement->Flags = CM_RESOURCE_PORT_IO
                              | CM_RESOURCE_PORT_POSITIVE_DECODE
                              | CM_RESOURCE_PORT_WINDOW_DECODE;

        requirement->u.Port.Alignment = PCI_TYPE1_IO_WINDOW_ALIGNMENT;
        requirement->u.Port.MinimumAddress.QuadPart = 0;
        requirement->u.Port.MaximumAddress.QuadPart = probedLimit;

        if ((windowBase > windowLimit) &&
            (PCI_IGNORE_DISABLED_BRIDGE_WINDOW(Device) == FALSE)) {

            //
            // The window is disabled, because base > length.
            // Respect this configuration, and create a 0 length
            // requirement.
            //
            requirement->u.Port.Length = 0;

        } else {

            //
            // Create a default requirement of the minimum size of the
            // window.  This can be overridden by the FDO.
            //
            requirement->u.Port.Length = 0;
            if (((Device->HackFlags & PCI_HACK_BRIDGE_WINDOW_ALIGNMENT) != 0) &&
                (windowLimit > windowBase)) {

                requirement->u.Port.Alignment = (windowLimit - windowBase + 1);
            }
        }
    }

    //
    // Memory window
    //
    requirement = &Requirements->type1.MemoryWindow;
    windowBase = PciType1_MemoryBase(&configHeader);
    windowLimit = PciType1_MemoryLimit(&configHeader);
    probedLimit = PciType1_MemoryLimit(&probedHeader);

    if ((probedLimit & PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK) == 0) {

        //
        // The window is not implemented.  This is not
        // legal for the memory window, but handle it gracefully.
        //
        NullResourceRequirement(requirement);

    } else {

        requirement->Type = CmResourceTypeMemory;
        requirement->ShareDisposition = CmResourceShareDeviceExclusive;
        requirement->Flags = CM_RESOURCE_MEMORY_READ_WRITE
                              | CM_RESOURCE_MEMORY_WINDOW_DECODE;

        requirement->u.Memory.Alignment = PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
        requirement->u.Memory.MinimumAddress.QuadPart = 0;
        requirement->u.Memory.MaximumAddress.QuadPart = probedLimit;

        if ((windowBase > windowLimit) &&
            (PCI_IGNORE_DISABLED_BRIDGE_WINDOW(Device) == FALSE)) {

            //
            // The window is disabled, because base > length.
            // Respect this configuration, and create a 0 length
            // requirement.
            //
            requirement->u.Memory.Length = 0;

        } else {

            //
            // Create a default requirement of the minimum window size
            // allowed.  This can be overridden by the FDO.
            //

            requirement->u.Memory.Length = 0;

            if (((Device->HackFlags & PCI_HACK_BRIDGE_WINDOW_ALIGNMENT) != 0) &&
                (windowLimit > windowBase)) {

                requirement->u.Memory.Alignment = (windowLimit - windowBase + 1);
            }
        }
    }

    //
    // Prefetchable Memory window
    //
    requirement = &Requirements->type1.PrefetchWindow;
    prefetchBase = PciType1_PrefetchMemoryBase(&configHeader);
    prefetchLimit = PciType1_PrefetchMemoryLimit(&configHeader);
    probedPrefetchLimit = PciType1_PrefetchMemoryLimit(&probedHeader);

    if ((probedPrefetchLimit.QuadPart & PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK) == 0) {

        //
        // The window is not implemented.
        //
        NullResourceRequirement(requirement);

    } else {

        requirement->Type = CmResourceTypeMemory;
        requirement->ShareDisposition = CmResourceShareDeviceExclusive;
        requirement->Flags = CM_RESOURCE_MEMORY_READ_WRITE
                              | CM_RESOURCE_MEMORY_PREFETCHABLE
                              | CM_RESOURCE_MEMORY_WINDOW_DECODE;

        requirement->u.Memory.Alignment = PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
        requirement->u.Memory.MinimumAddress.QuadPart = 0;
        requirement->u.Memory.MaximumAddress.QuadPart = probedPrefetchLimit.QuadPart;

        if (((ULONGLONG)prefetchBase.QuadPart >
             (ULONGLONG)prefetchLimit.QuadPart) &&
            (PCI_IGNORE_DISABLED_BRIDGE_WINDOW(Device) == FALSE)) {

            //
            // The window is disabled, because base > length.
            // Respect this configuration, and create a 0 length
            // requirement.
            //
            requirement->u.Memory.Length = 0;

        } else {

            //
            // Create a default requirement of the minimum window size
            // allowed.  This can be overridden by the FDO.
            //
            requirement->u.Memory.Length = 0;
            if (((Device->HackFlags & PCI_HACK_BRIDGE_WINDOW_ALIGNMENT) != 0) &&
                ((ULONGLONG)prefetchLimit.QuadPart >
                 (ULONGLONG)prefetchBase.QuadPart)) {

                status = RtlIoEncodeMemIoResource(
                            requirement,
                            requirement->Type,
                            PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT,
                            ((ULONGLONG)prefetchLimit.QuadPart -
                             (ULONGLONG)prefetchBase.QuadPart +
                             1),
                            requirement->u.Memory.MinimumAddress.QuadPart,
                            requirement->u.Memory.MaximumAddress.QuadPart);

                if (!NT_SUCCESS(status)) {
                    NullResourceRequirement(requirement);
                }
            }
        }
    }

    //
    // Generate a ROM requirement
    //
    status = PciRequirementFromRomBar(probedHeader.type1.ROMBaseAddress,
                                      configHeader.type1.ROMBaseAddress,
                                      &Requirements->type1.RomBar
                                      );

    return status;
}

NTSTATUS
PciType1_GetResources(
    __in PPCI_DEVICE Device,
    __out PPCI_DEVICE_RESOURCES Resources
    )
/*++

Routine Description:

    This routine gets the currently decoded resources for a type 1 PCI device.
    These resources include the BARs, windows and ROM BAR for the device.

Parameters:

    Device - The PCI device to get the resources of.

    Resources - Pointer to a structure that will contain the resources
        on successful return from this routine.

Return Value:

    NT Status code.

--*/
{
    PHYSICAL_ADDRESS prefetchBase, prefetchLimit;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR resource;
    PIO_RESOURCE_DESCRIPTOR requirement;
    PCI_CONFIG_HEADER configHeader;
    NTSTATUS status = STATUS_SUCCESS;
    ULONG windowBase, windowLimit;
    ULONG barIndex, upperBar;
    ULONGLONG prefetchLength;

    RtlZeroMemory(Resources, sizeof(PCI_DEVICE_RESOURCES));

    //
    // If the I/O or memory enable bits are not set, the device has
    // no resources that it is currently decoding.
    //
    PciReadConfigRegister(Device, Command, &configHeader.Command);
    if ((configHeader.Command & (PCI_ENABLE_MEMORY_SPACE | PCI_ENABLE_IO_SPACE)) == 0) {
        return STATUS_SUCCESS;
    }

    PciReadConfigRegister(
        Device,
        type1.BaseAddresses,
        &configHeader.type1.BaseAddresses[0]
        );

    PciReadDeviceConfig(
        Device,
        &configHeader.type1.IOBase,
        CONFIG_FIELD_OFFSET(type1.IOBase),
        CONFIG_FIELD_LENGTH(type1.IOBase) + CONFIG_FIELD_LENGTH(type1.IOLimit)
        );

    PciReadDeviceConfig(
        Device,
        &configHeader.type1.MemoryBase,
        CONFIG_FIELD_OFFSET(type1.MemoryBase),
        CONFIG_FIELD_LENGTH(type1.MemoryBase)
         + CONFIG_FIELD_LENGTH(type1.MemoryLimit)
         + CONFIG_FIELD_LENGTH(type1.PrefetchBase)
         + CONFIG_FIELD_LENGTH(type1.PrefetchLimit)
         + CONFIG_FIELD_LENGTH(type1.PrefetchBaseUpper32)
         + CONFIG_FIELD_LENGTH(type1.PrefetchLimitUpper32)
         + CONFIG_FIELD_LENGTH(type1.IOBaseUpper16)
         + CONFIG_FIELD_LENGTH(type1.IOLimitUpper16)
        );

    PciReadConfigRegister(
        Device,
        type1.ROMBaseAddress,
        &configHeader.type1.ROMBaseAddress
        );

    //
    // BARs
    //
    for (barIndex = 0; barIndex < PCI_TYPE1_BAR_COUNT; barIndex++) {

        resource = &Resources->Bars[barIndex];
        requirement = &Device->Requirements.Bars[barIndex];

        if ( (requirement->Type == CmResourceTypeNull) ||
             ( ((requirement->Type == CmResourceTypeMemory) ||
                (requirement->Type == CmResourceTypeMemoryLarge)) &&
               ((configHeader.Command & PCI_ENABLE_MEMORY_SPACE)) == 0) ||
             ( (requirement->Type == CmResourceTypePort) &&
               ((configHeader.Command & PCI_ENABLE_IO_SPACE)) == 0)) {

            //
            // There is no current setting because the BAR is unimplemented
            // or the appropriate enable bit isn't set.
            //
            resource->Type = CmResourceTypeNull;
            continue;
        }

        //
        // upperBar contains the (potential) upper 32 bits of a 64 bit
        // BAR.  only populate this if we're not on the last BAR, for
        // which there can be no upper 32 bits.
        //
        if (barIndex < (PCI_TYPE1_BAR_COUNT - 1)) {
#pragma prefast(suppress:__WARNING_INDEX_EXCEEDS_MAX)
            upperBar = configHeader.type1.BaseAddresses[barIndex+1];

        } else {
            upperBar = 0;
        }

        status = PciResourceFromBar(configHeader.type1.BaseAddresses[barIndex],
                                    upperBar,
                                    requirement,
                                    resource
                                    );
        if (!NT_SUCCESS(status)) {
            return status;
        }
    }

    //
    // I/O window
    //
    resource = &Resources->type1.IoWindow;
    requirement = &Device->Requirements.type1.IoWindow;

    if ((requirement->Type == CmResourceTypeNull) ||
        ((configHeader.Command & PCI_ENABLE_IO_SPACE) == 0)) {

        resource->Type = CmResourceTypeNull;

    } else {

        PCI_ASSERT(requirement->Type == CmResourceTypePort);

        windowBase = PciType1_IoBase(&configHeader);
        windowLimit = PciType1_IoLimit(&configHeader);

        if (windowBase > windowLimit) {

            //
            // If base > limit, the window is disabled
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

    //
    // Memory window
    //
    resource = &Resources->type1.MemoryWindow;
    requirement = &Device->Requirements.type1.MemoryWindow;

    if ((requirement->Type == CmResourceTypeNull) ||
        ((configHeader.Command & PCI_ENABLE_MEMORY_SPACE) == 0)) {

        resource->Type = CmResourceTypeNull;

    } else {

        PCI_ASSERT(requirement->Type == CmResourceTypeMemory);

        windowBase = PciType1_MemoryBase(&configHeader);
        windowLimit = PciType1_MemoryLimit(&configHeader);

        if ((windowBase > windowLimit) || (windowBase == 0)) {

            //
            // The window is "configured" at 0, which can't happen for
            // memory windows, or it's been explicitly disabled (limit
            // less than base).  It's not currently decoding.
            //
            resource->Type = CmResourceTypeNull;

        } else {

            resource->Type = CmResourceTypeMemory;
            resource->ShareDisposition =  requirement->ShareDisposition;
            resource->Flags = requirement->Flags;

            resource->u.Memory.Start.QuadPart = windowBase;
            resource->u.Memory.Length = windowLimit - windowBase + 1;
        }

    }

    //
    // Prefetchable memory window
    //
    resource = &Resources->type1.PrefetchWindow;
    requirement = &Device->Requirements.type1.PrefetchWindow;

    if ((requirement->Type == CmResourceTypeNull) ||
        ((configHeader.Command & PCI_ENABLE_MEMORY_SPACE) == 0)) {

        resource->Type = CmResourceTypeNull;

    } else {

        PCI_ASSERT((requirement->Type == CmResourceTypeMemory) ||
                   (requirement->Type == CmResourceTypeMemoryLarge));

        prefetchBase = PciType1_PrefetchMemoryBase(&configHeader);
        prefetchLimit = PciType1_PrefetchMemoryLimit(&configHeader);

        if (((ULONGLONG)prefetchBase.QuadPart >
             (ULONGLONG)prefetchLimit.QuadPart) ||
            (prefetchBase.QuadPart == 0)) {

            //
            // The window is "configured" at 0, which can't happen for
            // memory windows, or it's been explicitly disabled (limit
            // less than base).  It's not currently decoding.
            //
            resource->Type = CmResourceTypeNull;

        } else {


            prefetchLength = (ULONGLONG)prefetchLimit.QuadPart -
                             (ULONGLONG)prefetchBase.QuadPart
                             + 1;

            //
            // Hack to workaround badly behaved firmware which sets the
            // prefetch limit highpart to a garbage value. This would not
            // be encodeable and RtlCmEncodeMemIoResource would otherwise fail.
            //

            if ((prefetchLength > CM_RESOURCE_MEMORY_LARGE_48_MAXLEN) &&
                (prefetchLimit.HighPart > prefetchBase.HighPart) &&
                (prefetchLimit.LowPart > prefetchBase.LowPart)) {

                ERROR("PciType1_GetResources: Illegal prefetch window configuration.\n");

                prefetchLimit.HighPart = prefetchBase.HighPart;
                prefetchLength = (ULONGLONG)prefetchLimit.QuadPart -
                                 (ULONGLONG)prefetchBase.QuadPart
                                 + 1;
            }

            resource->Flags = requirement->Flags;
            resource->ShareDisposition =  requirement->ShareDisposition;

            status = RtlCmEncodeMemIoResource(resource,
                                              requirement->Type,
                                              prefetchLength,
                                              prefetchBase.QuadPart);

            if (!NT_SUCCESS(status)) {

                //
                // The window cannot be encoded correctly into the
                // memory descriptor.
                //

                resource->Type = CmResourceTypeNull;
            }
        }
    }

    //
    // ROM BAR
    //
    if ((configHeader.Command & PCI_ENABLE_MEMORY_SPACE) &&
        (configHeader.type1.ROMBaseAddress & PCI_ROMADDRESS_ENABLED)) {

        status = PciResourceFromRomBar(configHeader.type1.ROMBaseAddress,
                                       &Device->Requirements.type1.RomBar,
                                       &Resources->type1.RomBar
                                       );
    }

    return status;
}

NTSTATUS
PciType1_SetResources(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_RESOURCES Resources
    )
/*++

Routine Description:

    This routine sets the device resources for a type 1 PCI device.
    These resources include the BARs, windows and ROM BAR for the device.
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
    ULONG barIndex;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR resource;
    PIO_RESOURCE_DESCRIPTOR requirement;
    ULONGLONG windowBase, windowLimit, windowLength;
    ULONGLONG prefetchBase, prefetchLimit, prefetchLength;
    PCI_CONFIG_HEADER configHeader;
    USHORT decode;
    ULONG romBar;

    NON_PAGED_CODE();

    //
    // Extract the data from the BARs and write it out to hardware
    //
    for (barIndex = 0; barIndex < PCI_TYPE1_BAR_COUNT; barIndex++) {

        resource = &Resources->Bars[barIndex];
        requirement = &Device->Requirements.Bars[barIndex];

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
            } else if ((requirement->Type == CmResourceTypeMemory) ||
                       (requirement->Type == CmResourceTypeMemoryLarge)) {
                decode = PCI_ENABLE_MEMORY_SPACE;
            } else {
                decode = 0;
            }

            if ((resource->Type != CmResourceTypeNull) ||
                (Device->ResourceDecodes & decode)) {

                ERROR("PciType1_SetResources: Illegal resource filtering.\n");
                return STATUS_INVALID_PARAMETER;
            }
        }

        if (resource->Type == CmResourceTypeNull) {
            configHeader.type1.BaseAddresses[barIndex] = 0;
            continue;
        }

        configHeader.type1.BaseAddresses[barIndex] = resource->u.Generic.Start.LowPart;

        //
        // If this is a 64bit bar populate the next bar with the upper bits and
        // advance index
        //
        if ((ULONGLONG)requirement->u.Generic.MaximumAddress.QuadPart > MAXULONG) {

            if (barIndex < (PCI_TYPE1_BAR_COUNT - 1)) {

                barIndex++;
                configHeader.type1.BaseAddresses[barIndex] = resource->u.Generic.Start.HighPart;
            }

        } else {

            if (resource->u.Generic.Start.HighPart != 0) {
                ERROR("PciType1_SetResources: Invalid generic resource supplied.\n");
                return STATUS_INVALID_PARAMETER;
            }
        }
    }

    PciWriteConfigRegister(Device, type1.BaseAddresses, configHeader.type1.BaseAddresses);

    //
    // Initialize the config header to have closed bridge windows.
    // If we have allocated windows for the bridge, we'll overwrite this
    // below.
    //
    configHeader.type1.IOBase = 0xFF;
    configHeader.type1.IOLimit = 0;
    configHeader.type1.MemoryBase = 0xFFFF;
    configHeader.type1.MemoryLimit = 0;
    configHeader.type1.PrefetchBase = 0xFFFF;
    configHeader.type1.PrefetchLimit = 0;
    configHeader.type1.PrefetchBaseUpper32 = 0;
    configHeader.type1.PrefetchLimitUpper32 = 0;
    configHeader.type1.IOBaseUpper16 = 0;
    configHeader.type1.IOLimitUpper16 = 0;

    //
    // I/O window
    //
    resource = &Resources->type1.IoWindow;

    if (resource->Type != CmResourceTypeNull) {

        if (resource->u.Port.Start.HighPart != 0) {
            ERROR("PciType1_SetResources: invalid port resource supplied.\n");
            return STATUS_INVALID_PARAMETER;
        }

        windowLength = RtlCmDecodeMemIoResource(resource, &windowBase);
        if (windowLength==0) {
            windowBase = PCI_TYPE1_IO_WINDOW_ALIGNMENT;
            windowLimit = 0;

        } else {
            windowLimit = windowBase + windowLength - 1;
        }


        //
        // Check the alignment and length
        //
        PCI_ASSERT(((windowBase & ~PCI_TYPE1_IO_WINDOW_ADDRESS_MASK) == 0) &&
                   ((windowLimit & ~PCI_TYPE1_IO_WINDOW_ADDRESS_MASK)
                    == ~PCI_TYPE1_IO_WINDOW_ADDRESS_MASK));

        windowBase &= PCI_TYPE1_IO_WINDOW_ADDRESS_MASK;
        configHeader.type1.IOBase = (UCHAR)(windowBase >> 8);
        configHeader.type1.IOBaseUpper16 = (USHORT)(windowBase >> 16);

        windowLimit &= PCI_TYPE1_IO_WINDOW_ADDRESS_MASK;
        configHeader.type1.IOLimit = (UCHAR)(windowLimit >> 8);
        configHeader.type1.IOLimitUpper16 = (USHORT)(windowLimit >> 16);
    }



    //
    // Memory window
    //
    resource = &Resources->type1.MemoryWindow;

    if (resource->Type != CmResourceTypeNull) {

        //
        // Only 32 bit memory is supported for this window.
        //
        if (resource->u.Memory.Start.HighPart != 0) {
            ERROR("PciType1_SetResources: invalid memory resource supplied.\n");
            return STATUS_INVALID_PARAMETER;
        }

        windowLength = RtlCmDecodeMemIoResource(resource, &windowBase);
        if (windowLength==0) {
            windowBase = PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
            windowLimit = 0;

        } else {
            windowLimit = windowBase + windowLength - 1;
        }

        //
        // Memory windows are aligned at 0x100000 (1MB boundaries) and have length of
        // a multiple of 0x100000.
        //
        PCI_ASSERT(((windowBase & ~PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK) == 0) &&
                   ((windowLimit & ~PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK)
                    == ~PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK));

        //
        // Only the upper three nibbles are valid (0x100000 alignment).  For the
        // window base, the rest are assumed to be 0, and for the limit, the
        // rest are assumed to be 0xf.  Clear off the low bits and then shift
        // the values to fit in the registers.
        //
        configHeader.type1.MemoryBase
            = (USHORT)((windowBase & PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK) >> 16);
        configHeader.type1.MemoryLimit
            = (USHORT)((windowLimit & PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK) >> 16);
    }

    //
    // Prefetchable memory window
    //
    resource = &Resources->type1.PrefetchWindow;

    if (resource->Type != CmResourceTypeNull) {

        prefetchLength = RtlCmDecodeMemIoResource(resource, &prefetchBase);
        if (prefetchLength == 0) {
            prefetchBase = PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
            prefetchLimit = 0;

        } else {
            prefetchLimit = prefetchBase + prefetchLength - 1;
        }

        //
        // Memory windows are aligned at 0x100000 (1MB boundaries) and have length of
        // a multiple of 0x100000.
        //
        PCI_ASSERT(((prefetchBase & ~PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK) == 0) &&
                   ((prefetchLimit & ~PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK)
                    == ~PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK));

        //
        // Only the upper three nibbles of the LowPart are valid (0x100000 alignment).  For the
        // window base, the rest are assumed to be 0, and for the limit, the
        // rest are assumed to be 0xf.  All the bits of the upper DWORD are valid.
        // Clear off the low bits and then shift the values to fit in the registers.
        //
        prefetchBase &= PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK;
        prefetchLimit &= PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK;

        configHeader.type1.PrefetchBase = (USHORT)(prefetchBase >> 16);
        configHeader.type1.PrefetchBaseUpper32 = (ULONG)(prefetchBase >> 32);

        configHeader.type1.PrefetchLimit = (USHORT)(prefetchLimit >> 16);
        configHeader.type1.PrefetchLimitUpper32 = (ULONG)(prefetchLimit >> 32);
    }

    PciWriteDeviceConfig(Device,
                         &configHeader.type1.IOBase,
                         CONFIG_FIELD_OFFSET(type1.IOBase),
                         CONFIG_FIELD_LENGTH(type1.IOBase)
                          + CONFIG_FIELD_LENGTH(type1.IOLimit)
                         );

    PciWriteDeviceConfig(Device,
                         &configHeader.type1.MemoryBase,
                         CONFIG_FIELD_OFFSET(type1.MemoryBase),
                         CONFIG_FIELD_LENGTH(type1.MemoryBase)
                          + CONFIG_FIELD_LENGTH(type1.MemoryLimit)
                          + CONFIG_FIELD_LENGTH(type1.PrefetchBase)
                          + CONFIG_FIELD_LENGTH(type1.PrefetchLimit)
                          + CONFIG_FIELD_LENGTH(type1.PrefetchBaseUpper32)
                          + CONFIG_FIELD_LENGTH(type1.PrefetchLimitUpper32)
                          + CONFIG_FIELD_LENGTH(type1.IOBaseUpper16)
                          + CONFIG_FIELD_LENGTH(type1.IOLimitUpper16)
                         );

    //
    // Configure the ROM bar
    //
    resource = &Resources->type1.RomBar;

    if (resource->Type == CmResourceTypeNull) {

        romBar = 0;
    } else {
        romBar = resource->u.Generic.Start.LowPart;
    }

    PciWriteConfigRegister(Device, type1.ROMBaseAddress, &romBar);

    return STATUS_SUCCESS;
}

// ------------------------------------------------------------------ Utilities

ULONG
PciType1_IoBase(
    __in PPCI_CONFIG_HEADER ConfigHeader
    )
/*++

Routine Description:

    Compute the 32 bit base IO address being passed by the bridge
    whose config space is at Config.

    The IO base address is always 4KB aligned.  If only 64KB IO
    address space is supported, this is represented in the upper
    nibble of Config->u.type1.IOBase giving the range 0 to 0xf000
    for the base address.  The low nibble of Config->u.type1.IOBase
    contains flags.  If the least significant bit is set, then the
    bridge supports IO addressing to 4GB and Config->u.type1.IOBaseUpper16
    contains the upper 16 bits of the base address.

Arguments:

    ConfigHeader - Pointer to a buffer containing the device's
        configuration header.

Return Value:

    ULONG containing the IO Base address.

--*/
{
    BOOLEAN io32Bit;
    ULONG   base;

    NON_PAGED_CODE();

    io32Bit = ConfigHeader->type1.IOBase & PCI_TYPE1_IO_WINDOW_DECODE_32BIT;
    base = ConfigHeader->type1.IOBase << 8;

    if (io32Bit) {

        //
        // Check h/w (base and limit must be the same bit width).
        //
        PCI_ASSERT(ConfigHeader->type1.IOLimit & PCI_TYPE1_IO_WINDOW_DECODE_32BIT);

        base |= ConfigHeader->type1.IOBaseUpper16 << 16;
    }

    base &= PCI_TYPE1_IO_WINDOW_ADDRESS_MASK;

    return base;
}

ULONG
PciType1_IoLimit(
    __in PPCI_CONFIG_HEADER ConfigHeader
    )
/*++

Routine Description:

    Compute the 32 bit IO address limit being passed by the bridge
    whose config space is at Config.

    The range of IO addresses being passed is always a multiple of 4KB
    therefore the least significant 12 bits of the address limit are
    always 0xfff.  The upper nibble of Config->u.type1.IOLimit provides
    the next significant 4 bits.  The lower nibble of this byte contains
    flags.  If the least significant bit is set, the bridge is capable of
    passing 32 bit IO addresses and the next 16 significant bits are
    obtained from Config->u.type1.IOLimitUpper16.

Arguments:

    ConfigHeader - Pointer to a buffer containing the device's
        configuration header.

Return Value:

    ULONG containing the IO address limit.

--*/
{
    BOOLEAN io32Bit;
    ULONG   limit;

    NON_PAGED_CODE();

    io32Bit = ConfigHeader->type1.IOLimit & PCI_TYPE1_IO_WINDOW_DECODE_32BIT;
    limit = ConfigHeader->type1.IOLimit << 8;

    if (io32Bit) {

        //
        // Check h/w (base and limit must be the same bit width).
        //
        PCI_ASSERT(ConfigHeader->type1.IOBase & PCI_TYPE1_IO_WINDOW_DECODE_32BIT);

        limit |= ConfigHeader->type1.IOLimitUpper16 << 16;
    }

    limit |= ~PCI_TYPE1_IO_WINDOW_ADDRESS_MASK;

    return limit;
}

ULONG
PciType1_MemoryBase(
    __in PPCI_CONFIG_HEADER ConfigHeader
    )
/*++

Routine Description:

    Compute the 32 bit base Memory address being passed by the bridge
    whose config space is at Config.

    The Memory base address is always 1MB aligned.

Arguments:

    ConfigHeader - Pointer to a buffer containing the device's
        configuration header.

Return Value:

    ULONG containing the Memory Base address.

--*/
{
    ULONG base;

    NON_PAGED_CODE();

    //
    // The upper 12 bits of the memory base address are contained in
    // the upper 12 bits of the USHORT ConfigHeader->type1.MemoryBase.
    //
    base = ConfigHeader->type1.MemoryBase << 16;
    base &= PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK;

    return base;
}

ULONG
PciType1_MemoryLimit(
    __in PPCI_CONFIG_HEADER ConfigHeader
    )
/*++

Routine Description:

    Compute the 32 bit Memory address limit being passed by the bridge
    whose config space is at Config.

    The memory limit is always at the byte preceeding a 1MB boundary.
    The upper 12 bits of the limit address are contained in the upper
    12 bits of Config->u.type1.MemoryLimit, the lower 20 bits are all
    ones.

Arguments:

    ConfigHeader - Pointer to a buffer containing the device's
    configuration header.

Return Value:

    ULONG containing the Memory limit.

--*/
{
    ULONG limit;

    NON_PAGED_CODE();

    //
    // The upper 12 bits of the memory limit address are contained in
    // the upper 12 bits of the USHORT ConfigHeader->type1.MemoryLimit.
    //
    limit = ConfigHeader->type1.MemoryLimit << 16;
    limit |= ~PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK;

    return limit;
}

PHYSICAL_ADDRESS
PciType1_PrefetchMemoryBase(
    __in PPCI_CONFIG_HEADER ConfigHeader
    )
/*++

Routine Description:

    Compute the 64 bit base Prefetchable Memory address being passed
    by the bridge whose config space is at Config.

    The Prefetchable Memory base address is always 1MB aligned.

Arguments:

    ConfigHeader - Pointer to a buffer containing the device's
        configuration header.

Return Value:

    PHYSICAL_ADDRESS containing the Prefetchable Memory Base address.

--*/
{
    BOOLEAN prefetch64Bit;
    PHYSICAL_ADDRESS base;

    NON_PAGED_CODE();

    prefetch64Bit = (BOOLEAN)(ConfigHeader->type1.PrefetchBase
                              & PCI_TYPE1_MEMORY_WINDOW_DECODE_64BIT
                              );

    base.QuadPart = 0;

    base.LowPart = ConfigHeader->type1.PrefetchBase << 16;
    base.LowPart &= PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK;

    if (prefetch64Bit) {
        base.HighPart = ConfigHeader->type1.PrefetchBaseUpper32;
    }

    return base;
}

PHYSICAL_ADDRESS
PciType1_PrefetchMemoryLimit(
    __in PPCI_CONFIG_HEADER ConfigHeader
    )
/*++

Routine Description:

    Compute the 64 bit Prefetchable Memory address limit being passed
    by the bridge whose config space is at Config.

    The prefetchable memory limit is always at the byte preceeding a
    1MB boundary, that is, the least significant 20 bits are all ones.
    The next 12 bits are obtained from the upper 12 bits of
    Config->type1.PrefetchLimit.  The botton 4 bits of that field
    provide a flag indicating whether the upper 32 bits should be obtained
    from Config->u.type1.PrefetchLimitUpper32 or should be 0.

Arguments:

    ConfigHeader - Pointer to a buffer containing the device's
        configuration header.

Return Value:

    PHYSICAL_ADDRESS containing the prefetchable memory limit.

--*/
{
    BOOLEAN prefetch64Bit;
    PHYSICAL_ADDRESS limit;

    NON_PAGED_CODE();

    prefetch64Bit = (BOOLEAN)(ConfigHeader->type1.PrefetchLimit
                              & PCI_TYPE1_MEMORY_WINDOW_DECODE_64BIT
                              );

    limit.QuadPart = 0;

    limit.LowPart = ConfigHeader->type1.PrefetchLimit << 16;
    limit.LowPart |= ~PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK;

    if (prefetch64Bit) {
        limit.HighPart = ConfigHeader->type1.PrefetchLimitUpper32;
    }

    return limit;
}
