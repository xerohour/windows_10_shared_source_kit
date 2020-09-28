/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    resource.c

Abstract:

    This module contains functions dealing with resource requirements
    of PCI devices











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines

#define PCI_REQUIREMENT_INTERNAL 0x1000

#define PCI_MAXIMUM_DESCRIPTOR_COUNT 0x10000

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

ULONGLONG
PciGetLengthFromBar(
    __in ULONG UpperBar,
    __in ULONG ProbedBar
    );

BOOLEAN
PciIoSpaceNotRequired(
    __in PPCI_DEVICE Device
    );

BOOLEAN
PciMatchResourcesWithRequirements(
    __in PPCI_DEVICE_RESOURCES Resources,
    __in PPCI_DEVICE_REQUIREMENTS Requirements,
    __in BOOLEAN PruneMemory,
    __in BOOLEAN PruneIO
    );

VOID
PciPopulateMsiRequirements(
    __in PPCI_DEVICE Device,
    __in ULONG MessageCount,
    __in_ecount(MessageCount) PIO_RESOURCE_DESCRIPTOR Descriptor,
    __out PULONG Count
    );

NTSTATUS
PciGetInterruptConnectionData(
    __in PPCI_DEVICE Device,
    __deref_out PINTERRUPT_CONNECTION_DATA *ConnectionDataOut
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciBuildRequirementsList)
    #pragma alloc_text(PAGE, PciBuildCurrentResourceList)
    #pragma alloc_text(PAGE, PciProcessStartResources)
    #pragma alloc_text(PAGE, PciAllocateIoRequirementsList)
    #pragma alloc_text(PAGE, PciAllocateCmResourceList)
    #pragma alloc_text(PAGE, PciIsVgaResource)
    #pragma alloc_text(PAGE, PciRequirementFromPreferredResource)
    #pragma alloc_text(PAGE, PciGenericRequirementsAllowed)
    #pragma alloc_text(PAGE, PciIoSpaceNotRequired)
    #pragma alloc_text(PAGE, PciMatchResourcesWithRequirements)
    #pragma alloc_text(PAGE, PciPopulateMsiRequirements)
    #pragma alloc_text(PAGE, PciGetInterruptConnectionData)
#endif

// --------------------------------------------------------------------- Globals
// ------------------------------------------------------------------- Functions

NTSTATUS
PciBuildRequirementsList(
    __in PPCI_DEVICE Device,
    __out PIO_RESOURCE_REQUIREMENTS_LIST *RequirementsList
    )
/*++

Routine Description:

    This routine builds a requirements list for the given device that can be
    handed back to PnP in the context of a Query Resource Requirements IRP.

    The resource requirements list for a PCI device is reasonably
    straightforward as long as the device doesn't support Message-Signaled
    Interrupts.  If it does support them, there are two major cases:

    PCI 2.2 MSI:  In this case, the MSI requirement can be represented as a
    single resource descriptor.  The MSI request will be represented as a
    preferred resource descriptor.  If the device can generate multiple
    messages, that preferred descriptor will be followed by an alternate that
    requests only a single message.  If the device also supports line-based
    interrupts, the next descriptor will be another alternate, describing the
    LBI.

    PCI 3.0 MSI-X:  In this case, the MSI requirement can be represented as a
    single descriptor only if the device supports only a single message.
    Multiple message requests require one descriptor per message.  Since it is
    impossible to describe a single message alternative to a preferred request
    that uses multiple descriptors, there may need to be two lists generated
    in this case.

    SR-IOV adds more complications.  SR-IOV devices have two sets of BARs, those
    for the physical function and those for the virtual functions.  VF BARs are
    special because their required size is multiplied by the number of VFs.
    Furthermore, many BIOSes don't look for VF BARs when they compute the size
    of the bridge windows for upstream bridges.  So we'll start these buses with
    the BIOS's choices and then end up trying to rebalance if the driver
    supports SR-IOV.  This is bad because the device might not start if
    rebalance isn't possible, even in cases where the user had no intention to
    actually use SR-IOV.  So, when the device supports SR-IOV and the machine
    does too, we build the resource lists (within the requirements list) twice,
    once with VF BAR claims and once without.  If there isn't enough MMIO space
    to grant the VF BAR claims, the lists without those claims will be used.
    This will cause the device (usually a NIC) to start and be useful unless
    the user configures it to use SR-IOV, at which point enabling virtualization
    on the device will fail.

Parameters:

    Device - The PCI_DEVICE to generate the requirements of.

    RequirementsList - On exit, will point to memory allocated by this routine
    containing the requirements list.  It is PnP's responsibility to free this
    memory.

Return Value:

    NT Status code.

--*/
{
    BOOLEAN addLineBasedInterrupt;
    PIO_RESOURCE_REQUIREMENTS_LIST reqList, finalRequirements;
    PIO_RESOURCE_LIST list;
    PIO_RESOURCE_DESCRIPTOR requirement;
    PPCI_DEVICE_RESOURCES preferredResources;
    PCI_SAVED_CONFIGURATION savedConfig;
    BOOLEAN pruneMemory, pruneIO;
    BOOLEAN skipVfBars;
    ULONG messagesRequested;
    UCHAR listCount;
    UCHAR populateListCount;
    UCHAR listIndex;
    BOOLEAN match;
    BOOLEAN msiUsed;
    ULONG msiCount;
    NTSTATUS status;
    ULONG requirementCount;
    ULONG totalRequirementCount;
    ULONG maxDescriptors;
    USHORT bridgeControl;
    ULONG index;
    ULONG BarRequirementCount;
    ULONG BarRequirementUsed;

    PAGED_CODE();

    *RequirementsList = NULL;
    reqList = NULL;

    //
    // First get some supporting information.  This includes the saved BIOS
    // config, which is used to compute the preferred resources, and any
    // registry override for MSIs requested.
    //
    status = PciGetSavedDeviceConfiguration(Device, &savedConfig);
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    preferredResources = &savedConfig.Resources;
    messagesRequested = 0;
    if (Device->InterruptRequirement.Message.Type != PciNone) {
        status = PciGetMessageCount(Device, &messagesRequested);
        if (!NT_SUCCESS(status)) {
            messagesRequested = 0;
        }
    }

    pruneMemory = FALSE;

    //
    // If the driver does not support SR-IOV - do not show the requirements
    // for VF BARs.
    //
    skipVfBars = TRUE;
    if (Device->HeaderType == PCI_DEVICE_TYPE) {
        if (PciIsSriovSupported(Device) != FALSE) {
            skipVfBars = FALSE;
            Device->ExpressPort->EnableSriovDecodes = TRUE;
        }
    }

    //
    // Some devices with I/O BARs also have their registers mapped into memory.
    // PCI-X and PCI Express both require this.  In these cases, the drivers
    // can use the memory-mapped registers, making the I/O registers useless.
    // If the driver supports this, exclude the I/O requirements.
    //
    pruneIO = PciIoSpaceNotRequired(Device);

    //
    // Try and match preferred resources with default requirements.  If these
    // do not match, use the current resources as preferred.  This can happen
    // if a boot config was for a different device with similar IDs.
    //
    match = PciMatchResourcesWithRequirements(preferredResources,
                                              &Device->Requirements,
                                              pruneMemory,
                                              pruneIO
                                              );
    if (match == FALSE) {
        WARN("Investigate - Preferred resources do not match default "
             "requirements for device 0x%p\n", Device);

        preferredResources = &Device->Resources;
    }

    //
    // Allocate a list large enough for the maximum possible number
    // of descriptors required.  At the end we'll allocate a new list of
    // the appropriate size.
    //

    BarRequirementCount = PciGetBarRequirementCount(Device);
    maxDescriptors = BarRequirementCount           // Mem/IO BARs
                     + messagesRequested           // For MSI descriptors
                     + 1                           // For Level triggered interrupt
                     + 4;                          // For VGA resources

    //
    // If there are multiple MSI-X messages requested, two lists are required,
    // one with all the requested messages and one with only one requested
    // message (and a line-based interrupt.)
    //
    listCount = 1;
    if ((Device->InterruptRequirement.Message.Type == PciMsiX) &&
        (messagesRequested > 1)) {

        listCount = 2;
    }

    //
    // If SR-IOV is a goal, double the list count.
    //
    if (skipVfBars == FALSE) {

        listCount *= 2;
    }
    populateListCount = listCount;
    if (skipVfBars == FALSE && (Device->HackFlags & PCI_HACK_FORCE_RESOURCE_VF_BARS) != 0) {
        //
        // There are 2 sets of resource lists. The 1st set has both PF and VF bars.
        // The 2nd set has only PF bars.
        // Pnp picks the less resource which satisfy the PF to start. It would pick
        // the 2nd set.
        // Intel Niantec PF is prefetchable memory bar and VF is not prefetchable.
        // Dell bios has a bug to have memory window closed on bridge above Niantec.
        // It prevents VF to be assigned to VM.
        // Here we build only the 1st set of resource list which contains both PF
        // and VF bars. Pnp has no chance to pick the 2nd alternate resource.
        //
        populateListCount = listCount/2;
    }

    //
    // Check the descriptor count against an arbitrary upper bound that provides
    // plenty of head-room given the current hardware capabilities supported by
    // this routine but also provides plenty of slack space to protect against
    // a requirements list length that overflows a ULONG.
    //

    PCI_ASSERT(maxDescriptors <= PCI_MAXIMUM_DESCRIPTOR_COUNT);

    reqList = PciAllocateIoRequirementsList(
                    Device->BusNumber,
                    Device->Slot.u.AsULONG,
                    listCount,
                    maxDescriptors * listCount
                    );

    if (reqList == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }

    //
    // Now fill in the descriptors.
    //
    totalRequirementCount = 0;
    list = &reqList->List[0];
    for (listIndex = 0; listIndex < populateListCount; listIndex++) {
        list->Version = PCI_CM_RESOURCE_VERSION;
        list->Revision = PCI_CM_RESOURCE_REVISION;
        requirement = list->Descriptors;
        requirementCount = 0;

        //
        // First the memory and I/O descriptors
        //
        for (index = 0; index < PCI_DEVICE_RESOURCE_COUNT; index++) {

            if (Device->Requirements.ByIndex[index].Type == CmResourceTypeNull) {
                continue;
            }
            if (Device->Requirements.ByIndex[index].Flags & PCI_REQUIREMENT_INTERNAL) {
                continue;
            }

            if ((pruneMemory &&
                ((Device->Requirements.ByIndex[index].Type == CmResourceTypeMemory) ||
                 (Device->Requirements.ByIndex[index].Type == CmResourceTypeMemoryLarge))) ||
                (pruneIO &&
                (Device->Requirements.ByIndex[index].Type == CmResourceTypePort))) {

                NullResourceRequirement(requirement);
                requirement++;
                requirementCount++;
                continue;
            }

            //
            // Don't add the requirements if the driver is not expecting them
            // or if the resource list is in the second half.  I.e. put the
            // VF BARs in the first two lists list in a four-list device.
            //
            if ((Device->HeaderType == PCI_DEVICE_TYPE) &&
                (skipVfBars || (listIndex >= listCount / 2)) &&
                (&Device->Requirements.ByIndex[index] >=
                &Device->Requirements.type0.VfBars[0])) {

                continue;
            }

            BarRequirementUsed = PciPopulateBarRequirements(Device,
                                                            index,
                                                            preferredResources,
                                                            requirement);

            requirement += BarRequirementUsed;
            requirementCount += BarRequirementUsed;
        }

        //
        // Now message signaled interrupt(s).
        //
        msiUsed = FALSE;
        addLineBasedInterrupt = TRUE;
        if (messagesRequested > 0) {
            msiUsed = TRUE;

            //
            // In the first of two passes, populate multi-message MSI-X
            // requirements.
            //
            if ((Device->InterruptRequirement.Message.Type == PciMsiX) &&
                (listIndex % 2 == 0) &&
                (messagesRequested > 1)) {

                PciPopulateMsiRequirements(Device,
                                           messagesRequested,
                                           requirement,
                                           &msiCount
                                           );

                //
                // Avoid adding a line based alternate to the multiple
                // message resource list. The line based alternate will be
                // an alternate for the single message resource list.
                //
                addLineBasedInterrupt = FALSE;

                PCI_MARK_SAFE_ADDITION(requirementCount, msiCount);
                requirement += msiCount;
                requirementCount += msiCount;

            } else {

                //
                // This is either the second of two passes or the only pass.
                //
                switch (Device->InterruptRequirement.Message.Type) {

                case PciMsiX:

                    //
                    // If this is the second pass, the multi-message descriptors
                    // were generated in the first pass.  If this is the only
                    // pass, then the device only supports a single message.
                    // In either case, only a single message MSI-X is generated
                    // on this pass.
                    //

                    PciPopulateMsiRequirements(Device,
                                               1,
                                               requirement,
                                               &msiCount
                                               );

                    PCI_ASSERT(msiCount == 1);

                    //
                    // Only keep the "preferred" bit if there is a line-based
                    // interrupt which will be an alternative.
                    //
                    if (Device->InterruptRequirement.LineBased.Type !=
                        CmResourceTypeInterrupt) {

                        requirement->Option &= ~IO_RESOURCE_PREFERRED;
                    }

                    requirement++;
                    requirementCount++;

                    break;

                case PciMsi:

                    //
                    // Both the multi-message and single message MSI
                    // requirements can be generated in a single list.
                    //
                    if (messagesRequested > 1) {

                        PciPopulateMsiRequirements(Device,
                                                   messagesRequested,
                                                   requirement,
                                                   &msiCount
                                                   );

                        PCI_ASSERT(msiCount == 1);

                        requirement++;
                        requirementCount++;
                    }

                    PciPopulateMsiRequirements(Device,
                                               1,
                                               requirement,
                                               &msiCount
                                               );

                    PCI_ASSERT(msiCount == 1);

                    //
                    // This is an alternative requirement if there is a
                    // multi-message requirement just in front of it.
                    //
                    if (messagesRequested > 1) {
                        requirement->Option = IO_RESOURCE_ALTERNATIVE;
                    }

                    requirement++;
                    requirementCount++;
                    break;

                default:

                    //
                    // If we're here, the type is either None which we already
                    // checked for, or LineBased, which is illegal.
                    //
                    PCI_ASSERT(FALSE);
                    msiUsed = FALSE;
                    break;
                }
            }
        }

        //
        // Now line-based interrupts. This requirement is not influenced by the
        // command values.
        //
        if (addLineBasedInterrupt) {
            if (Device->InterruptRequirement.LineBased.Type ==
                CmResourceTypeInterrupt) {

                RtlCopyMemory(requirement,
                              &Device->InterruptRequirement.LineBased,
                              sizeof(IO_RESOURCE_DESCRIPTOR)
                              );

                //
                // If this device has message-signaled interrupts, the level-
                // triggered interrupt is an alternative.  Otherwise, it is its
                // own requirement.
                //
                if (msiUsed) {
                    requirement->Option = IO_RESOURCE_ALTERNATIVE;
                }

                requirement++;
                requirementCount++;
            }
        }

        //
        // Finally the VGA requirements for a bridge with the VGA bit set
        //
        bridgeControl = Device->SavedRegisters.bridge.BridgeControl;

        if (((Device->HeaderType == PCI_BRIDGE_TYPE) ||
             (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) &&
             !PciSubtractiveDecodeBridge(Device) &&
             (bridgeControl & PCI_ENABLE_BRIDGE_VGA)) {

            //
            // First, put in a device private.  This is no longer used,
            // but must be in the list for backwards compatibility.
            //
            requirement->Type = CmResourceTypeDevicePrivate;
            requirement->u.DevicePrivate.Data[0] = 3;
            requirement->u.DevicePrivate.Data[1] = 3;
            requirement++;
            requirementCount++;

            //
            // Add the memory descriptor.
            //
            requirement->Type = CmResourceTypeMemory;
            requirement->Flags = 0;
            requirement->u.Generic.MinimumAddress.QuadPart = 0xa0000;
            requirement->u.Generic.MaximumAddress.QuadPart = 0xbffff;
            requirement->u.Generic.Length = 0xbffff - 0xa0000 + 1;
            requirement->u.Generic.Alignment = 1;
            requirement++;
            requirementCount++;

            //
            // Add the two IO ranges, marking them 10 bit and positive decode.
            //
            requirement->Type = CmResourceTypePort;
            requirement->Flags = CM_RESOURCE_PORT_POSITIVE_DECODE;
            if (!(bridgeControl & PCI_ENABLE_BRIDGE_VGA_16BIT)) {
                requirement->Flags |= CM_RESOURCE_PORT_10_BIT_DECODE;
            }
            requirement->u.Generic.MinimumAddress.QuadPart = 0x3b0;
            requirement->u.Generic.MaximumAddress.QuadPart = 0x3bb;
            requirement->u.Generic.Length = 0x3bb - 0x3b0 + 1;
            requirement->u.Generic.Alignment = 1;
            requirement++;
            requirementCount++;

            requirement->Type = CmResourceTypePort;
            requirement->Flags = CM_RESOURCE_PORT_POSITIVE_DECODE;
            if (!(bridgeControl & PCI_ENABLE_BRIDGE_VGA_16BIT)) {
                requirement->Flags |= CM_RESOURCE_PORT_10_BIT_DECODE;
            }
            requirement->u.Generic.MinimumAddress.QuadPart = 0x3c0;
            requirement->u.Generic.MaximumAddress.QuadPart = 0x3df;
            requirement->u.Generic.Length = 0x3df - 0x3c0 + 1;
            requirement->u.Generic.Alignment = 1;
            requirement++;
            requirementCount++;
        }

        //
        // Now that we have a real count for the requirements in this list,
        // fill it in.
        //
        list->Count = requirementCount;
        totalRequirementCount += requirementCount;

        //
        // Move on to the next list.
        //
        list = (PIO_RESOURCE_LIST)requirement;
    }

    //
    // Record the list count and size.
    //

    reqList->AlternativeLists = populateListCount;
    reqList->ListSize = sizeof(IO_RESOURCE_REQUIREMENTS_LIST) +
                        (sizeof(IO_RESOURCE_LIST) * (populateListCount - 1)) +
                        (sizeof(IO_RESOURCE_DESCRIPTOR) * (totalRequirementCount - 1));


    //
    // Finally, allocate a new list of the appropriate size, instead of the
    // oversized temporary buffer we had before. Do this if we have at least
    // one requirement. In case of zero requirement, this will ensure a NULL
    // list instead of an empty list required for the device manager to display
    // correct information in the UI.
    //
    finalRequirements = NULL;
    if (totalRequirementCount > 0) {
        finalRequirements = PciAllocatePool(PagedPool, reqList->ListSize);
        if (!finalRequirements) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto cleanup;
        }

        RtlCopyMemory(finalRequirements, reqList, reqList->ListSize);
    }

    PciFreePool(reqList);
    *RequirementsList = finalRequirements;

    DebugDumpRequirementsList(RESOURCE_TRACE_LEVEL, finalRequirements);

    return STATUS_SUCCESS;

cleanup:

    if (reqList) {
        PciFreePool(reqList);
    }

    WARN("PciBuildRequirementsList for device 0x%p failed, status=0x%x\n",
         Device,
         status
         );
    return status;
}

NTSTATUS
PciBuildCurrentResourceList(
    __in PPCI_DEVICE Device,
    __out PCM_RESOURCE_LIST *ResourceList
    )
/*++

Routine Description:

    Given a pointer to a PCI PDO, this routine allocates and returns a pointer
    to the resource description of that PDO.

Arguments:

    PdoExtension - Our extension for the PCI-enumerated physical device object.

    ResourceList - Used to return a pointer to the resource list.

Return Value:

    NT status.

--*/
{
    PCM_PARTIAL_RESOURCE_DESCRIPTOR array, arrayElement;
    PCM_RESOURCE_LIST cmResourceList;
    PPCI_DEVICE_RESOURCES resources;
    PCI_DEVICE_ID_DATA idData;
    ULONG resourceCount;
    BOOLEAN lbiRequired;
    BOOLEAN vgaRequired;
    BOOLEAN vga10Bit;
    NTSTATUS status;
    USHORT command;
    ULONG index;

    PAGED_CODE();

    lbiRequired = FALSE;
    *ResourceList = NULL;
    RtlZeroMemory(&idData, sizeof(PCI_DEVICE_ID_DATA));

    //
    // If the device doesn't exist anymore or is a different device,
    // it is probably been surprise removed and we can't trust any
    // data returned from the hardware.
    //
    status = PciReadDeviceIdData(Device, &idData);
    if (!NT_SUCCESS(status) ||
        !PciIsSameDevice(Device, &idData)) {

        return STATUS_NO_SUCH_DEVICE;
    }

    //
    // First get the internal, header-type specific representation of
    // the currently decoded resources
    //
    status = Device->Operations->GetResources(Device,
                                              &Device->Resources
                                              );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    resources = &Device->Resources;

    PciReadConfigRegister(Device, Command, &command);

    //
    // Now get a count of the number of descriptors needed.
    //
    resourceCount = 0;
    vgaRequired = FALSE;
    vga10Bit = FALSE;
    for (index = 0; index < PCI_DEVICE_RESOURCE_COUNT; index++) {

        if (resources->ByIndex[index].Type != CmResourceTypeNull) {

            resourceCount++;
        }
    }

    //
    // The device should claim an interrupt resource as a boot resource
    // only when such resource is in active use. Since such information
    // is not always available on the device, we will use the command
    // values to derive this based on the premise that if the device is
    // decoding memory or I/O, then chances are that it is using interrupts
    // as well (if not disabled).
    //

    if ((Device->InterruptResource.Type == PciLineBased) &&
        !(command & PCI_DISABLE_LEVEL_INTERRUPT) &&
        ((command & PCI_ENABLE_MEMORY_SPACE) ||
         (command & PCI_ENABLE_IO_SPACE))) {

        lbiRequired = TRUE;
        resourceCount++;
    }

    if (((Device->HeaderType == PCI_BRIDGE_TYPE) ||
         (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) &&
        !PciSubtractiveDecodeBridge(Device)) {

        if (Device->SavedRegisters.bridge.BridgeControl & PCI_ENABLE_BRIDGE_VGA) {

            vgaRequired = TRUE;

            vga10Bit = TRUE;
            if (Device->SavedRegisters.bridge.BridgeControl & PCI_ENABLE_BRIDGE_VGA_16BIT) {
                vga10Bit = FALSE;
            }

            if (command & PCI_ENABLE_MEMORY_SPACE) {
                resourceCount++;
            }
            if (command & PCI_ENABLE_IO_SPACE) {
                resourceCount += 2;
            }
        }
    }

    if (resourceCount == 0) {

        //
        // Device has no resources currently enabled.
        //
        return STATUS_SUCCESS;
    }

    //
    // Allocate a CM Resource List large enough to handle this
    // device's resources.
    //
    cmResourceList = PciAllocateCmResourceList(Device, resourceCount);
    if (cmResourceList == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }

    //
    // Copy the resources from the internal resource array to
    // the output resource list
    //
    array = cmResourceList->List[0].PartialResourceList.PartialDescriptors;
    arrayElement = array;
    for (index = 0; index < PCI_DEVICE_RESOURCE_COUNT; index++) {

        if (resources->ByIndex[index].Type != CmResourceTypeNull) {

            PCI_ASSERT(arrayElement < (array + resourceCount));

            RtlCopyMemory(arrayElement,
                          &resources->ByIndex[index],
                          sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)
                          );
            arrayElement++;
        }
    }

    if (vgaRequired) {

        //
        // Add the memory descriptor.
        //
        if (command & PCI_ENABLE_MEMORY_SPACE) {
            arrayElement->Type = CmResourceTypeMemory;
            arrayElement->Flags = 0;
            arrayElement->u.Memory.Start.QuadPart = 0xa0000;
            arrayElement->u.Memory.Length = 0xbffff - 0xa0000 + 1;
            arrayElement++;
        }

        //
        // Add the two IO ranges, marking them 10 bit and positive decode.
        //
        if (command & PCI_ENABLE_IO_SPACE) {
            arrayElement->Type = CmResourceTypePort;
            arrayElement->Flags = CM_RESOURCE_PORT_POSITIVE_DECODE;
            if (vga10Bit) {
                arrayElement->Flags |= CM_RESOURCE_PORT_10_BIT_DECODE;
            }
            arrayElement->u.Port.Start.QuadPart = 0x3b0;
            arrayElement->u.Port.Length = 0x3bb - 0x3b0 + 1;
            arrayElement++;

            arrayElement->Type = CmResourceTypePort;
            arrayElement->Flags = CM_RESOURCE_PORT_POSITIVE_DECODE;
            if (vga10Bit) {
                arrayElement->Flags |= CM_RESOURCE_PORT_10_BIT_DECODE;
            }
            arrayElement->u.Port.Start.QuadPart = 0x3c0;
            arrayElement->u.Port.Length = 0x3df - 0x3c0 + 1;
            arrayElement++;
        }
    }

    if (lbiRequired != FALSE) {
        arrayElement->Type = CmResourceTypeInterrupt;
        arrayElement->ShareDisposition = CmResourceShareShared;
        arrayElement->Flags = CM_RESOURCE_INTERRUPT_LEVEL_SENSITIVE;;
        arrayElement->u.Interrupt.Level =
        arrayElement->u.Interrupt.Vector = Device->InterruptResource.InterruptLine;
        arrayElement->u.Interrupt.Affinity = MAXULONG;
    }

    DebugDumpResourceList(RESOURCE_TRACE_LEVEL, cmResourceList);
    *ResourceList = cmResourceList;
    return STATUS_SUCCESS;

cleanup:

    WARN("PciBuildCurrentResourceList for device 0x%p failed, status=0x%x\n",
         Device,
         status
         );

    return status;
}

NTSTATUS
PciProcessStartResources(
    __in PPCI_DEVICE Device,
    __in PCM_RESOURCE_LIST StartResourceList,
    __in PCM_RESOURCE_LIST TranslatedResourceList,
    __out PPCI_DEVICE_RESOURCES Resources,
    __out PPCI_DEVICE_INTERRUPT_RESOURCE InterruptResource
    )
/*++

Routine Description:

    This routine processes the incoming Start resource list, converting
    it into the internal PCI_DEVICE_RESOURCES representation.

Arguments:

    Device - The PCI_DEVICE whose Start resources are being processed.

    StartResourceList - The Start resource list.

    TranslatedResourceList - The translated Start resource list.

    Resources - A pointer to the PCI_DEVICE_RESOURCES structure that will
        be filled in by this routine.

    InterruptResource - A pointer to the PCI_DEVICE_INTERRUPT_RESOURCE
        structure that will be filled in by this routine.

Return Value:

    NT Status code.

--*/
{

    PHYSICAL_ADDRESS address;
    PINTERRUPT_CONNECTION_DATA connectionData;
    ULONG index;
    ULONG interruptCount;
    ULONG startingBarIndex;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR startResource;
    NTSTATUS status;
    ULONG tableSize;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR translatedResource;

    PAGED_CODE();

    tableSize = 0;

    //
    // Initialize the output buffer to 0's (CmResourceTypeNull)
    //
    RtlZeroMemory(Resources, sizeof(PCI_DEVICE_RESOURCES));
    if (StartResourceList == NULL) {
        return STATUS_SUCCESS;
    }

    RtlZeroMemory(InterruptResource, sizeof(PCI_DEVICE_INTERRUPT_RESOURCE));

    //
    // We should never get a Count of anything other that 1
    //
    if (StartResourceList->Count != 1) {
        ERROR("PciProcessStartResources: Invalid ResList 0x%p for device 0x%p.\n",
             StartResourceList,
             Device);

        return STATUS_INVALID_PARAMETER;
    }

    DebugDumpResourceList(RESOURCE_TRACE_LEVEL, StartResourceList);

    InterruptResource->Type = PciNone;
    index = 0;
    interruptCount = 0;

    //
    // First process interrupt descriptors.
    //

    connectionData = NULL;
    FOR_ALL_IN_RESOURCE_LIST(StartResourceList, startResource) {
        if (startResource->Type != CmResourceTypeInterrupt) {
            continue;
        }

        if (connectionData == NULL) {
            status = PciGetInterruptConnectionData(Device,
                                                   &connectionData);

            if (!NT_SUCCESS(status)) {
                goto exit;
            }
        }

        status = PciProcessInterruptDescriptor(Device,
                                               connectionData,
                                               startResource,
                                               interruptCount,
                                               InterruptResource
                                               );
        if (!NT_SUCCESS(status)) {
            goto exit;
        }

        interruptCount++;
    }

    //
    // The remaining resources in the StartResourceList will be in the same
    // order as the device's internal requirements.  Loop through the
    // requirements array to see which resources the device implements, to match
    // up the start resources with the internal requirements.
    //

    FOR_ALL_IN_PARALLEL_RESOURCE_LISTS(StartResourceList,
                                       TranslatedResourceList,
                                       startResource,
                                       translatedResource) {

        //
        // Skip over any resources we don't care about.
        //
        if ((startResource->Type != CmResourceTypeMemory) &&
            (startResource->Type != CmResourceTypeMemoryLarge) &&
            (startResource->Type != CmResourceTypePort) &&
            (startResource->Type != CmResourceTypeNull)) {

            continue;
        }

        if (PciIsVgaResource(startResource)) {
            continue;
        }

        //
        // This resource belongs in the PCI_DEVICE_RESOURCES.
        // The Start resources are in the same order as the elements of this
        // array, but some elements of the array do not correspond to start
        // resources (CmResourceTypeNull).  Skip over these.
        //
        for (; index < PCI_DEVICE_RESOURCE_COUNT; index++) {

            if ((Device->Requirements.ByIndex[index].Type !=
                 CmResourceTypeNull) &&
                ((Device->Requirements.ByIndex[index].Flags &
                  PCI_REQUIREMENT_INTERNAL) == 0)) {

                break;
            }
        }

        if (index < PCI_DEVICE_RESOURCE_COUNT) {

            //
            // Some resources may have been changed from CmResourceTypeMemory to
            // CmResourceTypeMemoryLarge or vice versa depending on the size required.
            // Thus to match up with the device requirements, both cases must be checked.
            //

            PCI_ASSERT((startResource->Type == CmResourceTypeNull) ||
                (startResource->Type == Device->Requirements.ByIndex[index].Type) ||
                ((startResource->Type == CmResourceTypeMemoryLarge) &&
                 (Device->Requirements.ByIndex[index].Type == CmResourceTypeMemory)) ||
                ((startResource->Type == CmResourceTypeMemory) &&
                 (Device->Requirements.ByIndex[index].Type == CmResourceTypeMemoryLarge)));

            RtlCopyMemory(&Resources->ByIndex[index],
                          startResource,
                          sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)
                          );

            if (startResource->u.Generic.Length == 0) {
                Resources->ByIndex[index].Type = CmResourceTypeNull;
            }

            //
            // If this resource contains the location of an interrupt message
            // table, record the translated location, since this must be used
            // to access the table.
            //

            startingBarIndex = FIELD_OFFSET(PCI_DEVICE_RESOURCES, Bars) /
                               sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR);

            if ((InterruptResource->Type == PciMsiX) &&
                ((startingBarIndex + InterruptResource->MsiX.BARIndex) == index)) {

                address.QuadPart =
                    translatedResource->u.Generic.Start.QuadPart +
                    InterruptResource->MsiX.TableOffset;

                tableSize =
                    Device->InterruptRequirement.Message.MessagesRequested *
                    sizeof(PCI_MSIX_TABLE_ENTRY);

                InterruptResource->MsiX.Table = MmMapIoSpaceEx(address,
                                                               tableSize,
                                                               (PAGE_NOCACHE | PAGE_READWRITE)
                                                               );

                if (InterruptResource->MsiX.Table == NULL) {
                    status = STATUS_INSUFFICIENT_RESOURCES;
                    goto exit;
                }
            }

            index++;
        }
    }

    status = STATUS_SUCCESS;

exit:
    if (!NT_SUCCESS(status)) {
        if ((InterruptResource->Type == PciMsiX) &&
            (InterruptResource->MsiX.Table != NULL)) {

            MmUnmapIoSpace(InterruptResource->MsiX.Table, tableSize);
        }
    }

    if (connectionData != NULL) {
        PciFreePool(connectionData);
    }

    return status;
}

NTSTATUS
PciRequirementFromBar(
    _In_ ULONG ProbedBar,
    _In_ ULONG UpperBar,
    _Inout_ PIO_RESOURCE_DESCRIPTOR Requirement,
    _Out_opt_ PBOOLEAN Bar64Bit
    )
/*++

Description:

    Generate an IO resource descriptor to describe the settings
    a Base Address Register can take.

Arguments:

    ProbedBar - the value read from a base address register
        immediately after writing all ones to it.

    UpperBar - The value read from the next higher BAR than the one
        represented by ProbedBar. If ProbedBar is the lower 32 bits
        of a 64 bit BAR, UpperBar is the upper 32 bits.

    Requirement - Pointer to the requirement that will be generated.

    Bar64Bit - If provided, points to a BOOLEAN that will be set to
        true if the BAR is 64 bits.

Return Value:

    NT Status code.

--*/
{
    BOOLEAN extendedBar = FALSE;
    ULONGLONG bar = 0;
    ULONGLONG length = 0;
    NTSTATUS status;

    NON_PAGED_CODE();

    if (ARGUMENT_PRESENT(Bar64Bit)) {
        *Bar64Bit = FALSE;
    }

    //
    // If the Base Address Register contains zero after being written
    // with all ones, it is not implemented..
    //
    // Note: We ignore the I/O bit in the BAR since in some unimplemented
    // BARs it is set.
    //
    if ((ProbedBar & ~PCI_ADDRESS_IO_SPACE) == 0) {

        NullResourceRequirement(Requirement);
        return STATUS_SUCCESS;
    }

    //
    // Set up type and flags.
    //
    if (ProbedBar & PCI_ADDRESS_IO_SPACE) {

        Requirement->Type = CmResourceTypePort;
        Requirement->Flags = CM_RESOURCE_PORT_IO
                             | CM_RESOURCE_PORT_POSITIVE_DECODE
                             | CM_RESOURCE_PORT_16_BIT_DECODE
                             | CM_RESOURCE_PORT_BAR;


    } else {

        //
        // We may change the type from CmResourceTypeMemory
        // to CmResourceTypeMemoryLarge after we find out
        // the size of the length required
        //

        Requirement->Type = CmResourceTypeMemory;
        Requirement->Flags = CM_RESOURCE_MEMORY_READ_WRITE | CM_RESOURCE_MEMORY_BAR;

        if (ProbedBar & PCI_ADDRESS_MEMORY_PREFETCHABLE) {
            Requirement->Flags |= CM_RESOURCE_MEMORY_PREFETCHABLE;
        }
    }

    Requirement->ShareDisposition = CmResourceShareDeviceExclusive;

    //
    // Ranges described by PCI Base Address Registers must be a
    // power of 2 in length and naturally aligned.  Get the length
    // and set the length and alignment in the descriptor.
    //

    if ((Requirement->Type == CmResourceTypeMemory) &&
        ((ProbedBar & PCI_ADDRESS_MEMORY_TYPE_MASK) == PCI_TYPE_64BIT)) {
        length = PciGetLengthFromBar (UpperBar,ProbedBar);

    } else {
        length = PciGetLengthFromBar (0,ProbedBar);
    }

    //
    // The maximum address is usually 0xffffffff for 32 bit
    // BARs and 0xffffffffffffffff for 64 bit BARs.  Some devices,
    // however, don't implement the top bits of the BAR.  When
    // probed, these BARs will read 0x00fff000, or something similar.
    // To get the maximum address of this BAR, the lower bits must be
    // or'ed into the probed BAR.  These lower bits can be easily obtained
    // because we've already computed the length, and the lower bits are
    // simply length-1.  This technique works even when the BAR is fully
    // implemented, so we use it in both cases.
    //
    bar = ProbedBar;
    if ((Requirement->Type == CmResourceTypeMemory) ||
        (Requirement->Type == CmResourceTypeMemoryLarge)) {

        if ((bar & PCI_ADDRESS_MEMORY_TYPE_MASK) == PCI_TYPE_64BIT) {

            //
            // This is a 64 bit PCI device.  Get the high 32 bits
            // from the next BAR.
            //
            bar |= (ULONGLONG)UpperBar << 32;
            extendedBar = TRUE;

        } else if ((bar & PCI_ADDRESS_MEMORY_TYPE_MASK) == PCI_TYPE_20BIT) {

            //
            // This device must locate below 1MB, the BAR shouldn't
            // have any top bits set but this isn't clear from the
            // spec.  Enforce it by clearing the top bits.  Also, the 20 bit
            // flag is reserved in the 2.2 spec.  By now people should be
            // following this.  Assert to make sure.
            //

            ERROR("PciRequirementFromBar: 20 BIT BAR encountered.\n");
            bar &= 0x000fffff;
        }
    }

    //
    // Fill in the descriptor.  Note that all BAR requirements have a minimum
    // address of 0.  BARs also have alignment equal to their length.
    //

    status = RtlIoEncodeMemIoResource(Requirement,
                                      Requirement->Type,
                                      length,
                                      length,
                                      0,
                                      bar | (length - 1));

    if (!NT_SUCCESS(status)) {
        return STATUS_UNSUCCESSFUL;
    }

    if (ARGUMENT_PRESENT(Bar64Bit)) {
        *Bar64Bit = extendedBar;
    }

#if defined(_ARM_) || defined(_ARM64_)

    //
    // On ARM platforms disallow all I/O BARs.
    //

    if (Requirement->Type == CmResourceTypePort) {
        NullResourceRequirement(Requirement);
    }

#endif

    return STATUS_SUCCESS;
}


NTSTATUS
PciRequirementFromRomBar(
    _In_ ULONG ProbedRomBar,
    _In_ ULONG CurrentRomBar,
    _Inout_ PIO_RESOURCE_DESCRIPTOR Requirement
    )
/*++

Description:

    Generate an IO resource descriptor to describe the settings
    a ROM BAR can take.

Arguments:

    ProbedRomBar - the value read from a ROM BAR
        immediately after writing all ones to it.

    CurrentRomBar - the value currently in the device's ROM BAR.

    Requirement - Pointer to the requirement that will be generated.

Return Value:

    NT Status code.

--*/
{
    ULONG length, bar;

    NON_PAGED_CODE();

    bar = ProbedRomBar & PCI_ADDRESS_ROM_ADDRESS_MASK;

    //
    // If the implementable bits of the ROM read 0, the ROM BAR is
    // not implemented.
    //
    if (bar == 0) {

        NullResourceRequirement(Requirement);
        return STATUS_SUCCESS;
    }

    Requirement->Type = CmResourceTypeMemory;
    Requirement->ShareDisposition = CmResourceShareDeviceExclusive;
    Requirement->Flags = CM_RESOURCE_MEMORY_READ_ONLY | CM_RESOURCE_MEMORY_BAR;
    Requirement->u.Memory.MinimumAddress.QuadPart = 0;
    Requirement->u.Memory.MaximumAddress.HighPart = 0;

    //
    // Ranges described by PCI Base Address Registers must be a
    // power of 2 in length and naturally aligned.  Get the length
    // and set the length and alignment in the descriptor.
    //
    length = (ULONG)PciGetLengthFromBar(0,ProbedRomBar); //0 for upper bar
    Requirement->u.Generic.Length = length;
    Requirement->u.Generic.Alignment = length;

    //
    // All BAR requirements have a minimum address of 0
    //
    Requirement->u.Generic.MinimumAddress.QuadPart = 0;

    //
    // The maximum address is usually 0xffffffff.  Some devices,
    // however, don't implement the top bits of the BAR.  When
    // probed, these BARs will read 0x00fff000, or something similar.
    // To get the maximum address of this BAR, the lower bits must be
    // or'ed into the probed BAR.  These lower bits can be easily obtained
    // because we've already computed the length, and the lower bits are
    // simply length-1.  This technique works even when the BAR is fully
    // implemented, so we use it in both cases.
    //
    Requirement->u.Generic.MaximumAddress.QuadPart = bar | (length - 1);

    //
    // If the ROM isn't currently enabled, set a flag that indicates that
    // this requirement shouldn't be placed in the requirements list
    //
    if (!(CurrentRomBar & PCI_ROMADDRESS_ENABLED)) {

        Requirement->Flags |= PCI_REQUIREMENT_INTERNAL;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciResourceFromBar(
    __in ULONG Bar,
    __in ULONG UpperBar,
    __in PIO_RESOURCE_DESCRIPTOR Requirement,
    __out PCM_PARTIAL_RESOURCE_DESCRIPTOR Resource
    )
/*++

Description:

    Generate a CM resource descriptor to describe the current
    setting of a Base Address Register.

Arguments:

    Bar - the value read from a base address register.

    UpperBar - The value read from the next higher BAR than the one
        represented by ProbedBar. If ProbedBar is the lower 32 bits
        of a 64 bit BAR, UpperBar is the upper 32 bits.

    Requirement - Pointer to the previously computed requirement for
        this BAR.

    Resource - Pointer to the resource descriptor that will be generated.

Return Value:

    NT Status code.

--*/
{

    //
    // Initialize the resource based on the requirement if we have one
    //
    Resource->Type = Requirement->Type;

    if (Requirement->Type == CmResourceTypeNull) {
        //
        // We have no requirement so we can't have a current setting
        //
        return STATUS_SUCCESS;
    }

    Resource->Flags = Requirement->Flags;
    Resource->ShareDisposition = Requirement->ShareDisposition;
    Resource->u.Generic.Length = Requirement->u.Generic.Length;
    Resource->u.Generic.Start.QuadPart = 0;

    //
    // Now decode the bar.
    //
    if (Resource->Type == CmResourceTypePort) {

        PCI_ASSERT(Bar & PCI_ADDRESS_IO_SPACE);

        Resource->u.Generic.Start.LowPart = Bar & PCI_ADDRESS_IO_ADDRESS_MASK;
    }

    if ((Resource->Type == CmResourceTypeMemory) ||
        (Resource->Type == CmResourceTypeMemoryLarge)) {

        PCI_ASSERT(!(Bar & PCI_ADDRESS_IO_SPACE));

        if ((Bar & PCI_ADDRESS_MEMORY_TYPE_MASK) == PCI_TYPE_64BIT) {

            //
            // This is a 64bit bar so the upper bits come from the next bar
            //
            Resource->u.Memory.Start.HighPart = UpperBar;

        } else if ((Bar & PCI_ADDRESS_MEMORY_TYPE_MASK) == PCI_TYPE_20BIT) {

            //
            // This is a 20bit bar, depricated in the latest PCI specs (>2.1)
            // Verify that the BAR is not decoding greater than 20 bits.
            //
            if ((Bar & 0xFFF00000) != 0) {
                VERIFIER("Invaid 20bit BAR with bits over 20bit set");
            }
        }

        Resource->u.Generic.Start.LowPart = Bar & PCI_ADDRESS_MEMORY_ADDRESS_MASK;
    }

    if (Resource->u.Generic.Start.QuadPart == 0) {

       //
       // No current setting if the value is current setting
       // is 0.
       //
       Resource->Type = CmResourceTypeNull;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciResourceFromRomBar(
    __in ULONG RomBar,
    __in PIO_RESOURCE_DESCRIPTOR Requirement,
    __out PCM_PARTIAL_RESOURCE_DESCRIPTOR Resource
    )
/*++

Description:

    Generate a CM resource descriptor to describe the current
    setting of a ROM BAR.

Arguments:

    RomBar - the value read from the ROM BAR.

    Requirement - Pointer to the previously computed requirement for
        this ROM BAR.

    Resource - Pointer to the resource descriptor that will be generated.

Return Value:

    NT Status code.

--*/
{

    //
    // Initialize the resource based on the requirement.  This will
    // either be CmResourceTypeMemory or CmResourceTypeNull;
    //
    Resource->Type = Requirement->Type;

    if (Requirement->Type == CmResourceTypeNull) {
        //
        // We have no requirement so we can't have a current setting
        //
        return STATUS_SUCCESS;
    }

    if (Requirement->Type != CmResourceTypeMemory) {
        ERROR("PciResourceFromRomBar: Requirement is not a memory resource.\n");
        return STATUS_INVALID_PARAMETER;
    }

    Resource->Flags = Requirement->Flags;
    Resource->ShareDisposition = Requirement->ShareDisposition;

    Resource->u.Generic.Start.QuadPart = RomBar & PCI_ADDRESS_ROM_ADDRESS_MASK;
    Resource->u.Generic.Length = Requirement->u.Generic.Length;

    if (((RomBar & PCI_ROMADDRESS_ENABLED) == 0) ||
        (Resource->u.Generic.Start.QuadPart == 0)) {

       //
       // No current setting if the value is current setting
       // is 0 or if the ROM enable bit is clear.
       //
       Resource->Type = CmResourceTypeNull;
    }

    return STATUS_SUCCESS;
}

PIO_RESOURCE_REQUIREMENTS_LIST
PciAllocateIoRequirementsList(
    __in ULONG BusNumber,
    __in ULONG SlotNumber,
    __in ULONG ListCount,
    __in ULONG ResourceCount
    )
/*++

Routine Description:

    This routine allocates and initializes a requirements list
    for use by the PCI driver.

Parameters:

    BusNumber - The bus number the device that generated these
        requirements is on.

    SlotNumber - The slot on that bus that the device is in.

    ListCount - The number of alternative lists in the
        requirements list.

    ResourceCount - The total number of resource descriptors
        needed in the requirements list.  If there are multiple
        alternative lists, each descriptor in each alternative
        list must be counted here.

Return Value:

    Pointer to the allocated requirements list, or NULL if
    the allocation failed.

--*/
{
    PIO_RESOURCE_REQUIREMENTS_LIST list;
    ULONG size;

    PAGED_CODE();

    //
    // There better be at least one resource per list.
    //
    if ((ResourceCount == 0) ||
        (ListCount == 0) ||
        (ResourceCount < ListCount)) {

        ERROR("PciAllocateIoRequirementsList: No resources!\n");
        return NULL;
    }

    //
    // Allocate space for (and zero) the resource requirements list.
    // An IO_RESOURCE_REQUIREMENTS_LIST comes with one free IO_RESOURCE_LIST,
    // so add space for the rest.  Each IO_RESOURCE_LIST comes with one
    // free IO_RESOURCE_DESCRIPTOR, so add the rest.
    //
    size = sizeof(IO_RESOURCE_REQUIREMENTS_LIST)
            + ((ListCount - 1) * sizeof(IO_RESOURCE_LIST))
            + ((ResourceCount - ListCount) * sizeof(IO_RESOURCE_DESCRIPTOR));


    list = PciAllocatePool(PagedPool | POOL_COLD_ALLOCATION, size);

    if (list != NULL) {

        RtlZeroMemory(list, size);

        //
        // Initialize the list structure header.
        //
        // Driver constant.
        //
        list->InterfaceType = PCIBus;
        list->AlternativeLists = ListCount;

        //
        // Call dependent.
        //
        list->BusNumber = BusNumber;
        list->SlotNumber = SlotNumber;
        list->ListSize = size;
    }
    return list;
}

PCM_RESOURCE_LIST
PciAllocateCmResourceList(
    __in PPCI_DEVICE Device,
    __in ULONG ResourceCount
    )
/*++

Routine Description:

    This routine allocates and initializes a resource list
    for use by the PCI driver.

Parameters:

    Device - The device for which this resource list is
        being generated.

    ResourceCount - The total number of resource descriptors
        needed in the resource list.

Return Value:

    Pointer to the allocated resource list, or NULL if
    the allocation failed.

--*/
{
    PCM_PARTIAL_RESOURCE_LIST partial;
    PCM_RESOURCE_LIST         list;
    ULONG                     size;

    PAGED_CODE();

    //
    // CM_RESOURCE_LIST includes space for one descriptor.  If there's
    // more than one (in the resource list) increase the allocation by
    // that amount.
    //
    size = sizeof(CM_RESOURCE_LIST);

    if (ResourceCount > 1) {
        size += (ResourceCount - 1) * sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR);
    }

    //
    // Get memory for the resource list.
    //
    list = PciAllocatePool(PagedPool | POOL_COLD_ALLOCATION, size);
    if (list != NULL) {

        RtlZeroMemory(list, size);

        //
        // Initialize the resource list.
        //
        list->Count = 1;
        list->List[0].InterfaceType = PCIBus;
        list->List[0].BusNumber = Device->BusNumber;

        partial = &list->List[0].PartialResourceList;
        partial->Version = PCI_CM_RESOURCE_VERSION;
        partial->Revision = PCI_CM_RESOURCE_REVISION;
        partial->Count = ResourceCount;
    }
    return list;
}

BOOLEAN
PciIsVgaResource(
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Resource
    )
/*++

Routine Description:

    This routine determines whether a resource descriptor
    is for a legacy VGA resource.

Parameters:

    Resource - The resource to test.

Return Value:

    TRUE if the resource is for a legacy VGA range.
    FALSE otherwise.

--*/
{
    PAGED_CODE();

    if ((Resource->Type == CmResourceTypeMemory) &&
        (Resource->u.Memory.Start.QuadPart == 0xa0000) &&
        (Resource->u.Memory.Length == 0x20000)) {

        return TRUE;
    }

    if ((Resource->Type == CmResourceTypePort) &&
        (Resource->u.Port.Start.QuadPart == 0x3b0) &&
        (Resource->u.Port.Length == 0xc)) {

        return TRUE;
    }


    if ((Resource->Type == CmResourceTypePort) &&
        (Resource->u.Port.Start.QuadPart == 0x3c0) &&
        (Resource->u.Port.Length == 0x20)) {

        return TRUE;
    }

    return FALSE;
}

// ------------------------------------------------------------------ Utilities

VOID
PciRequirementFromPreferredResource(
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Resource,
    __out PIO_RESOURCE_DESCRIPTOR Requirement
    )
/*++

Routine Description:

    This routine generates an IO_RESOURCE_DESCRIPTOR for
    the resource described by a CM_PARTIAL_RESOURCE_DESCRIPTOR.
    Since the IO_RESOURCE_DESCRIPTOR is for a fixed resource,
    its minimum and maximum addresses will be exactly described
    by the start and length of the resource, and alignment will be 1.

Parameters:

    Resource - The resource to base the requirement on.

    Requirement - The generated requirement

Return Value:

    VOID

--*/
{
    PAGED_CODE();

    Requirement->Option = IO_RESOURCE_PREFERRED;
    Requirement->Type = Resource->Type;
    Requirement->ShareDisposition = Resource->ShareDisposition;
    Requirement->Flags = Resource->Flags;
    Requirement->ShareDisposition = Resource->ShareDisposition;
    Requirement->u.Generic.Length = Resource->u.Generic.Length;
    Requirement->u.Generic.Alignment = 1; // alignment is meaningless in a fixed requirement.
    Requirement->u.Generic.MinimumAddress.QuadPart = Resource->u.Generic.Start.QuadPart;
    Requirement->u.Generic.MaximumAddress.QuadPart =
        Resource->u.Generic.Start.QuadPart + RtlCmDecodeMemIoResource(Resource, NULL) - 1;
}

BOOLEAN
PciGenericRequirementsAllowed(
    __in PPCI_DEVICE Device,
    __in ULONG Index
    )
/*++

Routine Description:

    This routine determines whether the resources for the
    given device should be "locked," that is, only the preferred
    config should be considered available.

Parameters:

    Device - The device to test.

    Index - Supplies an index into the requirements list of a device.

Return Value:

    FALSE if the devices resources should be locked.
    TRUE otherwise.

--*/
{
    PAGED_CODE();

    if ((PciLockDeviceResources == TRUE) ||
        Device->LegacyDriver ||
        (Device->Parent->HackFlags & PCI_BUS_HACK_LOCK_RESOURCES) ||
        (Device->HackFlags & PCI_HACK_LOCK_RESOURCES)) {

        return FALSE;
    }

    //
    // If a device is on debugging path and is boot configured for a resource
    // type then lock its resource requirement for that type. On the other hand
    // if the device isn't decoding a resource type then allow generic
    // requirements to be generated for the same.
    //

    if (Device->OnDebugPath) {
        if ((Device->SavedRegisters.Command & PCI_ENABLE_IO_SPACE) &&
            (Device->Requirements.ByIndex[Index].Type == CmResourceTypePort)) {

            return FALSE;
        }

        if ((Device->SavedRegisters.Command & PCI_ENABLE_MEMORY_SPACE) &&
            ((Device->Requirements.ByIndex[Index].Type ==
              CmResourceTypeMemory) ||
             (Device->Requirements.ByIndex[Index].Type ==
              CmResourceTypeMemoryLarge))) {

            return FALSE;
        }
    }

    return TRUE;
}

ULONGLONG
PciGetLengthFromBar(
    __in ULONG UpperBar,
    __in ULONG ProbedBar
    )
/*++

Routine Description:

    Given the contents of a PCI Base Address Register, after it
    has been written with all ones, this routine calculates the
    length (and alignment) requirement for this BAR.

    This method for determining requirements is described in
    section 6.2.5.1 of the PCI Specification (Rev 2.1).

Parameters:

    ProbedBar - The BAR to determine the length of, after writing
        all 1's to it.

Return Value:

    Returns the length of the resource requirement.  This will be a number
    in the range 0 thru 0x80000000.

--*/
{
    ULONGLONG length;
    ULONGLONG bar;
    ULONG OriginalProbedBar = ProbedBar;

    NON_PAGED_CODE();

    //
    // A number of least significant bits should be ignored in the
    // determination of the length.  These are flag bits, the number
    // of bits is dependent on the type of the resource.
    //
    if (ProbedBar & PCI_ADDRESS_IO_SPACE) {

        //
        // PCI IO space.
        //
        ProbedBar &= PCI_ADDRESS_IO_ADDRESS_MASK;

    } else {

        //
        // PCI Memory space.
        //
        ProbedBar &= PCI_ADDRESS_MEMORY_ADDRESS_MASK;
    }

    //
    // If the upper bar is given, all bits are relevant (no flag bits)
    //

    //
    // BaseAddressRegister now contains the maximum base address
    // this device can reside at and still exist below the top of
    // memory.
    //
    // The value 0xffffffff was written to the BAR.  The device will
    // have adjusted this value to the maximum it can really use.
    //
    // Length MUST be a power of 2.
    //
    // For most devices, h/w will simply have cleared bits from the
    // least significant bit positions so that the address 0xffffffff
    // is adjusted to accomodate the length.  eg: if the new value is
    // 0xffffff00, the device requires 256 bytes.
    //
    // The difference between the original and new values is the length (-1).
    //
    // For example, if the value fead back from the BAR is 0xffff0000,
    // the length of this resource is
    //
    //     0xffffffff - 0xffff0000 + 1
    //   = 0x0000ffff + 1
    //   = 0x00010000
    //
    //  ie 16KB.
    //
    // Some devices cannot reside at the top of PCI address space.  These
    // devices will have adjusted the value such that length bytes are
    // accomodated below the highest address.  For example, if a device
    // must reside below 1MB, and occupies 256 bytes, the value will now
    // be 0x000fff00.
    //
    // In the first case, length can be calculated as-
    //

    bar = (ULONGLONG)ProbedBar;
    if (UpperBar == 0) {
        length = (0xffffffff - ProbedBar) + 1;

    } else {

        bar |= (((ULONGLONG)UpperBar) << 32);
        length = (0xffffffffffffffff - bar) + 1;
    }

    if (((length - 1) & length) != 0) {

        //
        // We didn't end up with a power of two, must be the latter
        // case, we will have to scan for it.
        //
        length = 4;     // start with minimum possible

        while ((length | bar) != bar) {

            //
            // Length *= 2, note we will eventually drop out of this
            // loop for one of two reasons (a) because we found the
            // length, or (b) because Length left shifted off the end
            // and became 0.
            //
            length <<= 1;
        }
    }

    //
    // Check that if we have a 32 bit device, the length does not exceed
    // 4GB.
    //

    if ((OriginalProbedBar & PCI_ADDRESS_MEMORY_TYPE_MASK) != PCI_TYPE_64BIT) {
        PCI_ASSERT(length <= MAXULONG);
    }

    //
    // Check that we got something (i.e. not 0).
    //

    PCI_ASSERT(length);

    return length;
}

BOOLEAN
PciIoSpaceNotRequired(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine checks to see if the driver supports excluding
    of I/O requirements even if the device has I/O BARs.  A device
    might implement I/O BARs for driver compatibility but also
    implement memory BARs because the hardware standard (PCI-X
    or PCI Express) requires it or to move away from legacy I/O.
    In this case a properly written driver will not need the I/O BAR.
    Such a driver will set a registry value thru an INF to indicate
    this.

Arguments:

    Device - The device to check for I/O requirement.

Return Value:

    TRUE - If the driver supports exclusion of I/O requirements.
    FALSE - If the driver does not support this feature and I/O
        requirements must be included in the list.

--*/
{

    PAGED_CODE();

    //
    // Default to I/O being required unless otherwise specified.
    //

    if (PciIsDeviceFeatureEnabled(Device, PCI_FEATURE_IO_NOT_REQUIRED) != PciFeatureStateEnabled) {
        return FALSE;
    }

    return TRUE;
}

BOOLEAN
PciMatchResourcesWithRequirements (
    __in PPCI_DEVICE_RESOURCES Resources,
    __in PPCI_DEVICE_REQUIREMENTS Requirements,
    __in BOOLEAN PruneMemory,
    __in BOOLEAN PruneIO
    )

/*++

Routine Description:

    This routine matches the resource list with the requirements list
    for a device. The result determines which among the resource lists
    (saved config or device resources) will be used to compute the preferred
    requirements list.

    This is primarily to build the right set of requirements for fake
    PCI devices (read SoftPCI) that have different BAR types across
    different incarnations while retaining the same device IDs.

    Comparing resources with requirements in a rigorous way might lead
    to false-discarding of saved configs for real devices.

    The sequence for matching resources with requirements emulates the
    PciBuildRequirementsList routine.

    This routine ensures that the number and type of BARs in each of these
    lists are the same. It doesnt take into account the length and/or alignment
    for comparisons because certain HACKs applied during device enumeration
    might have updated those fields.

Parameters:

    Resources - Resource list for a device.

    Requirements - Requirements list for a device.

    PruneMemory - Indicates if the memory is pruned. In that case, we
        ignore the memory requirements.

    PruneIo - Indicates if the I/O is pruned. In that case we ignore the
        I/O requirements.

Return Value:

    TRUE if resources match requirements. FALSE otherwise.

--*/

{

    ULONG index;
    BOOLEAN match;
    PIO_RESOURCE_DESCRIPTOR requirementDesc;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR resourceDesc;

    PAGED_CODE();

    match = TRUE;
    resourceDesc = &Resources->ByIndex[0];
    requirementDesc = &Requirements->ByIndex[0];

    for (index = 0;
         index < PCI_DEVICE_RESOURCE_COUNT;
         index++, resourceDesc++, requirementDesc++) {

        if (requirementDesc->Type == CmResourceTypeNull) {
            continue;
        }

        if (requirementDesc->Flags & PCI_REQUIREMENT_INTERNAL) {
            continue;
        }

        if ((PruneMemory &&
             ((requirementDesc->Type == CmResourceTypeMemory) ||
              (requirementDesc->Type == CmResourceTypeMemoryLarge))) ||
            (PruneIO &&
             (requirementDesc->Type == CmResourceTypePort))) {

            continue;
        }

        //
        // if the types match, then ignore everything else.
        //

        if (requirementDesc->Type == resourceDesc->Type) {
            continue;
        }

        //
        // "Resources" is a list of boot configured resources for the device.
        // If a boot config resource descriptor is not found on the list,
        // i.e. the resource type is Null, then the device doesn't have any at
        // this index and so skip any comparisons with the corresponding
        // requirement descriptor.
        //

        if (resourceDesc->Type == CmResourceTypeNull) {
            continue;
        }

        //
        // The only condition for a mismatch is when the resource type is
        // different from the requirement descriptor type.
        //

        match = FALSE;
        break;
    }

    return match;
}

VOID
PciPopulateMsiRequirements(
    __in PPCI_DEVICE Device,
    __in ULONG MessageCount,
    __in_ecount(MessageCount) PIO_RESOURCE_DESCRIPTOR Descriptor,
    __out PULONG Count
    )
/*++

Routine Description:

    This routine fills in an array of requirements descriptors with data
    representing the MSI requirements of a device.  The type of MSI in use
    in the device and the number of messages affect the number of descriptors
    that get generated and their contents.

Arguments:

    Device - Supplies the device for which the requirements are to be generated.

    MessageCount - Supplies the number of messages that requirements are to
        be generated for.  Some multi-message requests can be expressed using
        a single descriptor, so this parameter does not indicate how many
        descriptors will be created.

    Descriptor - Supplies a pointer to an array of requirements descriptors
        to be filled in.  Enough descriptors to allow one descriptor to be
        created per message must be supplied in the array, although this
        routine may or may not populate all of them.

    Count - Supplies a pointer to a variable that receives the number of
        descriptors populated with the requested information.

Return Value:

    None.

--*/
{

    ULONG index;
    UCHAR iterator;
    BOOLEAN queryPolicy;
    PIO_RESOURCE_DESCRIPTOR requirement;
    ULONG requirementCount;
    NTSTATUS status;

    PAGED_CODE();

    requirement = Descriptor;
    requirementCount = 0;
    switch (Device->InterruptRequirement.Message.Type) {

        //
        // MSI-X case.  There is one descriptor per message.
        //

    case PciMsiX:

        queryPolicy = TRUE;
        iterator = 0;
        for (index = 0; index < MessageCount; index++) {
            RtlCopyMemory(requirement,
                          &Device->InterruptRequirement.Message.SingleMessage,
                          sizeof(IO_RESOURCE_DESCRIPTOR)
                          );

            if ((queryPolicy != FALSE) && (MessageCount != 1)) {

                //
                // The INF for this driver may have put policy in the
                // registry.  Now read the registry more extensively and
                // apply anything that might be there to this message.
                //
                // queryPolicy will be TRUE if the device policy keys
                // actually exist.  If this gets returned once, do not bother
                // with the check for subsequent messages.
                //
                status = PciGetMessagePolicy(
                            Device,
                            MessageCount,
                            index,
                            &iterator,
                            &queryPolicy,
                            &requirement->u.Interrupt.AffinityPolicy,
                            &requirement->u.Interrupt.PriorityPolicy,
                            &requirement->u.Interrupt.Group,
                            &requirement->u.Interrupt.TargetedProcessors
                            );

                if (NT_SUCCESS(status)) {
                    requirement->Flags |= CM_RESOURCE_INTERRUPT_POLICY_INCLUDED;
                }
            }

            //
            // Remove the "preferred" bit, as this is one of many messages.
            //
            if (MessageCount > 1) {
                requirement->Option &= ~IO_RESOURCE_PREFERRED;
            }

            requirement++;
        }

        requirementCount = MessageCount;
        break;

        //
        // MSI case.  There is one descriptor, and the message count is
        // encoded in the minimum vector field.
        //

    case PciMsi:

        RtlCopyMemory(requirement,
                      &Device->InterruptRequirement.Message.SingleMessage,
                      sizeof(IO_RESOURCE_DESCRIPTOR)
                      );

        requirementCount = 1;
        requirement->u.Interrupt.MinimumVector =
                CM_RESOURCE_INTERRUPT_MESSAGE_TOKEN - MessageCount + 1;

        break;

    default:

        //
        // If we're here, the type is either None which we already
        // checked for, or LineBased, which is illegal.
        //
        PCI_ASSERT(FALSE);

        break;
    }

    *Count = requirementCount;
    return;
}

NTSTATUS
PciGetInterruptConnectionData(
    __in PPCI_DEVICE Device,
    __deref_out PINTERRUPT_CONNECTION_DATA *ConnectionDataOut
    )
/*++

Routine Description:

    This routine retrieves the extended interrupt connection data associated
    with the given device.  This information is stored as a device property.

Arguments:

    Device - Supplies the device for which the connection data should be
        retrieved.

    ConnectionDataOut - Supplies a pointer to a variable that receives a pointer
        to the interrupt connection data for the device.

Return Value:

    STATUS_SUCCESS if the operation succeeds.
    STATUS_DATA_ERROR if the connection data could be retrieved, but is
        internally inconsistent.
    Another error status if returned from the property retrieval routine.

--*/
{

    PINTERRUPT_CONNECTION_DATA ConnectionData;
    ULONG ExpectedSize;
    ULONG RequiredSize;
    NTSTATUS Status;
    ULONG Type;

    PAGED_CODE();

    *ConnectionDataOut = NULL;
    ConnectionData = NULL;

    //
    // The extended interrupt connection data for a device is stored as a
    // device property.  This is an arbitrarily sized structure, so first
    // retrieve the necessary size of the data buffer.
    //

    Status = IoGetDevicePropertyData(Device->DeviceObject,
                                     &INTERRUPT_CONNECTION_DATA_PKEY,
                                     0, // Language ID
                                     0, // Flags
                                     0, // Size
                                     NULL, // Data
                                     &RequiredSize,
                                     &Type);

    if ((Status != STATUS_BUFFER_TOO_SMALL) ||
        (RequiredSize < sizeof(INTERRUPT_CONNECTION_DATA))) {
        return STATUS_UNSUCCESSFUL;
    }

    ConnectionData = PciAllocatePool(NonPagedPoolNx, RequiredSize);
    if (ConnectionData == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Call the property routine a second time to retrieve the data.
    //

    Status = IoGetDevicePropertyData(Device->DeviceObject,
                                     &INTERRUPT_CONNECTION_DATA_PKEY,
                                     0, // Language ID
                                     0, // Flags
                                     RequiredSize, // Size
                                     ConnectionData, // Data
                                     &RequiredSize,
                                     &Type);

    if (!NT_SUCCESS(Status)) {
        goto cleanup;
    }

    //
    // Validate the data.  The structure should have one vector description
    // for each message in the structure count.  If the buffer is smaller than
    // this, fail so that callers do not run off the end of the buffer.
    //

    ExpectedSize = sizeof(INTERRUPT_CONNECTION_DATA) +
                   ((ConnectionData->Count - 1) * sizeof(INTERRUPT_VECTOR_DATA));

    if (RequiredSize < ExpectedSize) {
        Status = STATUS_DATA_ERROR;
        goto cleanup;
    }

    *ConnectionDataOut = ConnectionData;
    return STATUS_SUCCESS;

cleanup:

    if (ConnectionData != NULL) {
        PciFreePool(ConnectionData);
    }

    return Status;
}

BOOLEAN
PciIsFixedRequirement(
    __in PIO_RESOURCE_DESCRIPTOR Requirement
    )

/*++

Routine Description:

    This routine determines whether a resource descriptor represents a
    fixed resource requirement.

Arguments:

    ResourceDescriptor - Supplies a pointer to a resource descriptor.

Return Value:

    TRUE if this is a fixed requirement, FALSE otherwise.

--*/

{

    ULONGLONG Length;
    ULONGLONG MaximumAddress;
    ULONGLONG MinimumAddress;
    ULONGLONG Range;

    if (Requirement->Type == CmResourceTypeNull) {
        return FALSE;
    }

    Length = RtlIoDecodeMemIoResource(Requirement,
                                      NULL,
                                      &MinimumAddress,
                                      &MaximumAddress);

    if (Length == 0) {
        return FALSE;
    }

    Range = MaximumAddress - MinimumAddress + 1;
    if (Range != Length) {
        return FALSE;
    }

    return TRUE;
}
