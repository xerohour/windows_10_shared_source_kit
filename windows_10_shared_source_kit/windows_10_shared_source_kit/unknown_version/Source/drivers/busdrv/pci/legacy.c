/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    legacy.c

Abstract:

    This module contains functions dealing with the PCI driver's
    interaction with the HAL and legacy devices.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines

#define LEGACY_DEVICE_INTERFACE_VERSION 0

//
// Translatable resources
//
#define ADDRESS_SPACE_MEMORY                0x0
#define ADDRESS_SPACE_PORT                  0x1
#define ADDRESS_SPACE_USER_MEMORY           0x2
#define ADDRESS_SPACE_USER_PORT             0x3
#define ADDRESS_SPACE_DENSE_MEMORY          0x4
#define ADDRESS_SPACE_USER_DENSE_MEMORY     0x6

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

_Success_(return != FALSE)
BOOLEAN
PciTranslateBusAddress(
    _In_ INTERFACE_TYPE InterfaceType,
    _In_ ULONG BusNumber,
    _In_ PHYSICAL_ADDRESS BusAddress,
    _Inout_ PULONG AddressSpace,
    _Out_ PPHYSICAL_ADDRESS TranslatedAddress
    );

NTSTATUS
PciAssignSlotResources (
    __in PUNICODE_STRING RegistryPath,
    __in_opt PUNICODE_STRING DriverClassName,
    __in PDRIVER_OBJECT DriverObject,
    __in_opt PDEVICE_OBJECT DeviceObject,
    __in INTERFACE_TYPE BusType,
    __in ULONG BusNumber,
    __in ULONG Slot,
    __out PCM_RESOURCE_LIST *AllocatedResources
    );

_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR PciLegacyDeviceInterface_Constructor;

VOID
PciLegacyDeviceInterface_Reference(
    __inout PVOID Context
    );

VOID
PciLegacyDeviceInterface_Dereference(
    __inout PVOID Context
    );

NTSTATUS
PciLegacyDeviceInterface_GetLegacyDevice(
    __inout PVOID Context,
    __in INTERFACE_TYPE LegacyBusType,
    __in ULONG BusNumber,
    __in ULONG SlotNumber,
    __out PDEVICE_OBJECT *PhysicalDeviceObject
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(INIT, PciHookHal)
    #pragma alloc_text(PAGE, PciUnhookHal)
    #pragma alloc_text(PAGE, PciAssignSlotResources)
    #pragma alloc_text(PAGE, PciLegacyDeviceInterface_Constructor)
    #pragma alloc_text(PAGE, PciLegacyDeviceInterface_GetLegacyDevice)
#endif

// --------------------------------------------------------------------- Globals

pHalAssignSlotResources PcipSavedAssignSlotResources = NULL;
pHalTranslateBusAddress PcipSavedTranslateBusAddress = NULL;

const PCI_INTERFACE PciLegacyDeviceDetectionInterface = {
    &GUID_LEGACY_DEVICE_DETECTION_STANDARD,    // InterfaceType
    sizeof(LEGACY_DEVICE_DETECTION_INTERFACE), // MinSize
    LEGACY_DEVICE_INTERFACE_VERSION,           // MinVersion
    LEGACY_DEVICE_INTERFACE_VERSION,           // MaxVersion
    PCIIF_FDO,                                 // Flags
    PciInterface_LegacyDeviceDetection,        // Signature
    PciLegacyDeviceInterface_Constructor       // Constructor
};

// ------------------------------------------------------------------- Functions

VOID
PciHookHal(
    VOID
    )
/*++

Routine Description:

    This is called when the PCI driver is loaded and it takes over the functions
    that have traditionally been in the HAL.

Arguments:

    None

Return Value:

    None

--*/
{
    PAGED_CODE();

    PCI_ASSERT(PcipSavedAssignSlotResources == NULL);
    PCI_ASSERT(PcipSavedTranslateBusAddress == NULL);

    //
    // Override the handlers for AssignSlotResources and
    // TranslateBusAddress.  (But only modify the HAL dispatch
    // table once.)
    //

    PcipSavedAssignSlotResources = HALPDISPATCH->HalPciAssignSlotResources;
    HALPDISPATCH->HalPciAssignSlotResources = PciAssignSlotResources;
    PcipSavedTranslateBusAddress = HALPDISPATCH->HalPciTranslateBusAddress;
    HALPDISPATCH->HalPciTranslateBusAddress = PciTranslateBusAddress;
    HalPciEarlyRestore = PciEarlyRestoreResources;
    HalPciMarkHiberPhase = PciMarkHiberPhase;
    HalPciMultiStageResumeCapable = PciMultiStageResumeCapable;
    HalPciLateRestore = PciLateRestoreResources;
}

VOID
PciUnhookHal(
    VOID
    )
/*++

Routine Description:

    This reverses the changed made by PciHookHal.  It is called as part of
    unloading the PCI driver which seems like a really bad idea...

Arguments:

    None

Return Value:

    None

--*/
{
    PAGED_CODE();
    
    PCI_ASSERT(PcipSavedAssignSlotResources != NULL);
    PCI_ASSERT(PcipSavedTranslateBusAddress != NULL);

    //
    // Override the handlers for AssignSlotResources and
    // TranslateBusAddress.  (But only modify the HAL dispatch
    // table once.)
    //

    HALPDISPATCH->HalPciAssignSlotResources = PcipSavedAssignSlotResources;
    HALPDISPATCH->HalPciTranslateBusAddress = PcipSavedTranslateBusAddress;

    PcipSavedAssignSlotResources = NULL;
    PcipSavedTranslateBusAddress = NULL;
}

NTSTATUS
PciAssignSlotResources(
    __in PUNICODE_STRING RegistryPath,
    __in_opt PUNICODE_STRING DriverClassName,
    __in PDRIVER_OBJECT DriverObject,
    __in_opt PDEVICE_OBJECT DeviceObject,
    __in INTERFACE_TYPE BusType,
    __in ULONG BusNumber,
    __in ULONG Slot,
    __out PCM_RESOURCE_LIST *AllocatedResources
    )
/*++

Routine Description:

    This subsumes the the functinality of HalAssignSlotResources for PCI devices.

    This function builds some bookkeeping information about legacy
    PCI device so that we know how to route interrupts for these
    PCI devices.  We build this here because this is the only place
    we see the legacy device object associated with proper bus, slot,
    function information.

Arguments:

    As HalAssignSlotResources

Return Value:

    STATUS_SUCCESS or error

--*/
{
    PIO_RESOURCE_REQUIREMENTS_LIST requirements;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptors;
    PCM_PARTIAL_RESOURCE_LIST partialList;
    PCI_DEVICE_RESOURCES deviceResources;
    PCI_CONFIG_HEADER configHeader;
    PCM_RESOURCE_LIST resources;
    ULONG readIndex, writeIndex;
    PCI_SLOT_NUMBER slotNumber;
    ULONG descriptorCount;
    PDEVICE_OBJECT oldDO;
    PPCI_DEVICE device;
    NTSTATUS status;

    PAGED_CODE();
    
    PCI_ASSERT(PcipSavedAssignSlotResources);
    
    *AllocatedResources = NULL;

    requirements = NULL;
    resources = NULL;
    
    //
    // We should only ever get here if this is a request for a PCI device
    //
    if (BusType != PCIBus) {
        ERROR("PciAssignSlotResources: Invalid BusType supplied.\n");
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Look for the PCI enumerated PDO for this bus/slot number.
    // It should be *different* from the legacy DO passed into
    // this call.
    //
    slotNumber.u.AsULONG = Slot;
    
    device = PciFindDeviceByLocation(BusNumber, slotNumber, FALSE);
    if (!device) {
        return STATUS_DEVICE_DOES_NOT_EXIST;
    }
    
    PCI_ASSERT(DeviceObject != device->DeviceObject);

    //
    // Only allow legacy resource allocation on this device
    // if we don't have a PnP driver for the device.
    //
    if (device->DeviceState != PciNotStarted) {
        return STATUS_INVALID_OWNER;
    }

    //
    // Grab the PciGlobalLock since we will modify the legacy cache.
    //
    PciAcquirePassiveLock(&PciGlobalLock);

    PciReadConfigHeader(device, &configHeader);

    //
    // Cache everything we have now learned about this
    // device object provided that they gave us one so that we can regurgitate
    // it when the IRQ arbiter needs to know.
    //












    status = PciCacheLegacyDeviceRouting(
                DeviceObject,
                BusNumber,
                Slot,
                configHeader.type0.InterruptLine,
                configHeader.type0.InterruptPin,
                configHeader.BaseClass,
                configHeader.SubClass,
                device->Parent->PhysicalDeviceObject,
                device,
                &oldDO
                );
    if (!NT_SUCCESS(status)) {
        goto ExitWithoutUpdatingCache;
    }

    PCI_DEVICE_SET_FLAG(device, LegacyDriver);

    //
    // Build a requirements list for this device
    //
    status = PciBuildRequirementsList(device, &requirements);
    if (!NT_SUCCESS(status)) {
        goto ExitWithCacheRestoreOnFailure;
    }

    //
    // Call the legacy API to get the resources
    //
#pragma prefast(suppress:__WARNING_USE_OTHER_FUNCTION, "This warning exists to get other drivers to invoke this wrapper") 
    status = IoAssignResources(RegistryPath,
                               DriverClassName,
                               DriverObject,
                               DeviceObject,
                               requirements,
                               &resources
                               );
    if (!NT_SUCCESS(status)) {
        PCI_ASSERT(resources == NULL);
        goto ExitWithCacheRestoreOnFailure;
    }

    //
    // Extract the PDO Resources (PCI driver internal style)
    // from the incoming resource list.
    //
    status = PciProcessStartResources(device, 
                                      resources, 
                                      resources,
                                      &deviceResources,
                                      &device->InterruptResource
                                      );
    if (!NT_SUCCESS(status)) {
        goto ExitWithCacheRestoreOnFailure;  
    }

    //
    // If the new settings are different than the old, program the new resources.
    //
    if (!RtlEqualMemory(&device->Resources, &deviceResources, sizeof(PCI_DEVICE_RESOURCES))) {
        
        RtlCopyMemory(&device->Resources, 
                      &deviceResources, 
                      sizeof(PCI_DEVICE_RESOURCES)
                      );
    }

    status = PciUpdateHardware(device, NULL);
    if (!NT_SUCCESS(status)) {
        goto ExitWithCacheRestoreOnFailure;  
    }

    status = PciProgramInterruptResource(device);
    if (!NT_SUCCESS(status)) {
        goto ExitWithCacheRestoreOnFailure;
    }

    //
    // Remove the device privates from the list - yes this means that we will
    // have allocated a little more pool than required.
    //
    PCI_ASSERT(resources->Count == 1);

    partialList = &resources->List[0].PartialResourceList;
    descriptorCount = resources->List[0].PartialResourceList.Count;
    descriptors = &resources->List[0].PartialResourceList.PartialDescriptors[0];

    readIndex = 0;
    writeIndex = 0;

    while (readIndex < descriptorCount) {
        if (descriptors[readIndex].Type != CmResourceTypeDevicePrivate) {

            if (writeIndex < readIndex) {

                //
                // Shuffle the descriptor up
                //
                RtlCopyMemory(&descriptors[writeIndex],
                              &descriptors[readIndex],
                              sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)
                              );
            }

            writeIndex++;

        } else {

            //
            // Skip the device private, don't increment writeCount so we will
            // overwrite it
            //
            PCI_ASSERT(partialList->Count > 0);
            partialList->Count--;

        }
        readIndex++;
    }

    PCI_ASSERT(partialList->Count > 0);

    *AllocatedResources = resources;
    resources = NULL;
    status = STATUS_SUCCESS;

ExitWithCacheRestoreOnFailure:
    
    //
    // On failure, restore the old legacy DO in our cache.
    //
    if (!NT_SUCCESS(status)) {

        PciCacheLegacyDeviceRouting(
        oldDO,
        BusNumber,
        Slot,
        configHeader.type0.InterruptLine,
        configHeader.type0.InterruptPin,
        configHeader.BaseClass,
        configHeader.SubClass,
        device->Parent->PhysicalDeviceObject,
        device,
        NULL
        );

    }

ExitWithoutUpdatingCache:
    
    PciReleasePassiveLock(&PciGlobalLock);

    if (requirements) {
        PciFreePool(requirements);
    }

    if (resources) {
        PciFreeExternalPool(resources);
    }
    return status;
}

_Success_(return != FALSE)
BOOLEAN
PciTranslateBusAddress(
    _In_ INTERFACE_TYPE InterfaceType,
    _In_ ULONG BusNumber,
    _In_ PHYSICAL_ADDRESS BusAddress,
    _Inout_ PULONG AddressSpace,
    _Out_ PPHYSICAL_ADDRESS TranslatedAddress
    )
/*++

Routine Description:

    This subsumes the functionality of HalTranslateBusAddress for PCI devices.

Arguments:

    As HalTranslateBusAddress

Return Value:

    TRUE if translation succeeded, FALSE otherwise.

--*/
{
    PPCI_BUS bus;
    BOOLEAN translatesOk;
    PARBITER_INSTANCE arbiter;
    RTL_RANGE_LIST_ITERATOR iterator;
    PRTL_RANGE current;
    ULONGLONG address;
    BOOLEAN found;
    PPCI_SEGMENT segment;
    NTSTATUS status;
#if defined(_X86_)
    ULONG savedAddressSpace;
#endif

    NON_PAGED_CODE();
    
    translatesOk = TRUE;
    address = (ULONGLONG)BusAddress.QuadPart;

    //
    // HalTranslateBusAddress can be called at high IRQL (the DDK says
    // <= DISPATCH_LEVEL) but crash dump seems to be at HIGH_LEVEL.  Either way
    // touching pageable data and code is a no no.  If we are calling at high
    // IRQL then just skip the validation that the range is on the bus as we are
    // crashing/hibernating at the time anyway...  We still need to call the
    // original hal function to perform the translation magic.
    //
    if (KeGetCurrentIrql() < DISPATCH_LEVEL) {

        // Try to determine whether this range is available at all on the
        // requested bus.  Do this by walking up the tree until the first
        // positive decode bridge is found, and checking its arbiter.
        // Subtractive bridges are ignored since all ranges can pass through
        // them.
        //
        PciAcquirePassiveLock(&PciGlobalLock);
        found = FALSE;
        bus = NULL;
        for (segment = PciSegmentList; segment != NULL; segment = segment->Next) {
            bus = segment->PciRootBusList;

            while (bus) {
                
                if (bus->SecondaryBusNumber == BusNumber) {
                    found = TRUE;
                    break;
                }

                bus = PciNextBusInSegment(bus);
            }

            if (found == TRUE) {
                break;
            }
        }
        
        if (bus == NULL) {

            // 
            // Couldn't find an FDO for the requested bus, because we haven't
            // enumerated one yet.  As a result we can't translate.  This usually
            // occurs because of legacy drivers (vga) trying to translate ranges
            // on a whole host of busses without regard for where the device
            // actually is.
            //
            WARN("PciTranslateBusAddress: Could not find bus FDO. Bus Number = 0x%x\n", 
                 BusNumber
                 );
            PciReleasePassiveLock(&PciGlobalLock);
            return FALSE;
        }

        for (;;) {

            //
            // Ignore subtractive bridges, since all ranges pass through them.
            //
            if (bus->PciBridge) {

                if (bus->PciBridge->Attributes.Subtractive) {

                    bus = bus->ParentBus;
                    continue;
                }
            }
            break;
        }

        status = IoAcquireRemoveLock(&bus->RemoveLock, NULL);

        PciReleasePassiveLock(&PciGlobalLock);

        PCI_ASSERT(bus);
        
        if (!NT_SUCCESS(status)) {
            return FALSE;
        }
        
        if (bus->DeviceState == PciStarted) {
            
            //
            // Find the appropriate arbiter
            //
            switch (*AddressSpace) {
                case 0: // Memory space
                case 2: // UserMode view of memory space (Alpha)
                case 4: // Dense memory space (Alpha)
                case 6: // UserMode view of dense memory space (Alpha)
                    arbiter = bus->MemoryArbiter;
                    break;
        
                case 1: // Port space
                case 3: // UserMode view of port space (Alpha)
                    arbiter = bus->IoArbiter;
                    break;
        
                default:
        
                    PCI_ASSERT(FALSE);
                    IoReleaseRemoveLock(&bus->RemoveLock, NULL);
                    return FALSE;
            }
    
            if (!arbiter) {
                IoReleaseRemoveLock(&bus->RemoveLock, NULL);
                return FALSE;
            }
    
            ArbAcquireArbiterLock(arbiter);
    
            //
            // If the range is owned by NULL, it does not make it down to
            // this bus.  Therefore, it cannot be translated.
            //
            FOR_ALL_RANGES(arbiter->Allocation, &iterator, current) {
    
                if (address < current->Start) {
                    //
                    // We have passed all possible intersections
                    //
                    break;
                }
    
#pragma prefast(suppress:__WARNING_REDUNDANTTEST, "by design")
                if (INTERSECT(current->Start, current->End, address, address) &&  
                    (current->Owner == NULL)) {
    
                    translatesOk = FALSE;
                    break;
                }
            }
    
            ArbReleaseArbiterLock(arbiter);
        }
        
        IoReleaseRemoveLock(&bus->RemoveLock, NULL);
    }
        
    //
    // If the range makes it onto the bus, it should translate fine.
    // Call the original HAL function to perform the translation magic
    //
#if defined(_X86_)
    savedAddressSpace = *AddressSpace;
#endif

    if (translatesOk) {

        translatesOk = PcipSavedTranslateBusAddress(
                            InterfaceType,
                            BusNumber,
                            BusAddress,
                            AddressSpace,
                            TranslatedAddress
                            );
    }

#if defined(_X86_)

    if (!translatesOk) {

        //
        // HalTranslateBusAddress failed, figure out if we want to
        // pretend it succeeded.
        //

        //
        // GROSS HACK:  If we failed to translate in the range 0xa0000
        // thru 0xbffff on an X86 machine, just go ahead and allow it.
        // It is probably because the BIOS is buggy.
        //
        // Same for 0x400 thru 0x4ff
        //

        if (BusAddress.HighPart == 0) {

            ULONG lowPart = BusAddress.LowPart; // improve code generation

            if (((savedAddressSpace == ADDRESS_SPACE_MEMORY) &&
                    (((lowPart >= 0xa0000) &&     // HACK broken MPS BIOS
                      (lowPart <= 0xbffff)) ||    //
                     ((lowPart >= 0x400)   &&     // HACK MGA
                      (lowPart <= 0x4ff))   ||    //
                     (lowPart == 0x70)      )) || // HACK Trident
                 ((savedAddressSpace == ADDRESS_SPACE_PORT) &&
                     ((lowPart >= 0xcf8) &&       // HACK MGA
                      (lowPart <= 0xcff)))) {

                translatesOk = TRUE;
                *TranslatedAddress = BusAddress;
                *AddressSpace = savedAddressSpace;
            }
        }
    }

#endif

    return translatesOk;
}

// ------------------------------------------------------------------- Utilities

PPCI_DEVICE
PciFindDeviceByLocation(
    __in ULONG BusNumber,
    __in PCI_SLOT_NUMBER Slot,
    __in BOOLEAN NoLocking
    )
/*++

Routine Description:

    This routine gets the device extension of the PCI enumerated
    device at the given bus and slot numbers.

Arguments:

    BusNumber - the bus number of the bus the device is on

    Slot - the device/function of the device

    NoLocking - Supplies a boolean which indicates that no locking is 
                necessary or should be done.

Return Value:

    The device extension or NULL if one can not be found

--*/
{
    PPCI_BUS bus;
    BOOLEAN found;
    PPCI_DEVICE device;
    PPCI_SEGMENT segment;

    device = NULL;

    if (NoLocking == FALSE) {
        PciAcquirePassiveLock(&PciGlobalLock);
    }

    //
    // Find the bus FDO.
    //
    found = FALSE;
    bus = NULL;
    for (segment = PciSegmentList; segment != NULL; segment = segment->Next) {
        bus = segment->PciRootBusList;
        while (bus) {

            if (bus->SecondaryBusNumber == BusNumber) {
                found = TRUE;
                break;
            }

            bus = PciNextBusInSegment(bus);
        }

        if (found == TRUE) {
            break;
        }
    }

    if (NoLocking == FALSE) {
        PciReleasePassiveLock(&PciGlobalLock);
    }

    if (bus == NULL) {

        //
        // This is bad.
        //
        if (NoLocking == FALSE) {
            ERROR("PciFindDeviceByLocation: Could not find bus FDO. Bus Number = 0x%x\n", BusNumber);
        }
        goto cleanup;
    }

    //
    // Now find the pdo for the device in this slot
    //
    if (NoLocking == FALSE) {
        PciAcquirePassiveLock(&bus->ChildDeviceLock);
    }

    device = bus->ChildDevices;
    while (device) {
        
        //
        // People probably don't clear the unused bits in a PCI_SLOT_NUMBER so
        // ignore them in the main build but assert checked so we can get this
        // fixed
        //
        if ((device->Slot.u.bits.DeviceNumber == Slot.u.bits.DeviceNumber) &&  
            (device->Slot.u.bits.FunctionNumber == Slot.u.bits.FunctionNumber)) {

            PCI_ASSERT(device->Slot.u.AsULONG == Slot.u.AsULONG);

            //
            // This is our guy!
            //
            break;
        }

        device = device->Sibling;
    }

    if (NoLocking == FALSE) {
        PciReleasePassiveLock(&bus->ChildDeviceLock);
    }

    if (device == NULL) {

        //
        // This is bad.
        //

        if (NoLocking == FALSE) {
            ERROR("PciFindDeviceByLocation: Could not find PDO for device @ %x.%x.%x\n",
                  BusNumber,
                  Slot.u.bits.DeviceNumber,
                  Slot.u.bits.FunctionNumber
                  );
        }

        goto cleanup;
    }
    return device;

cleanup:

    return NULL;
}

// -------------------------------------------------------------- Legacy Device Interface

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciLegacyDeviceInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    This routine constructs a LEGACY_DEVICE_DETECTION_INTERFACE.

Arguments:

    DeviceExtension - An FDO extenion pointer.

    InterfaceSpecificData - Unused.

    Version - Interface version.

    Size - Size of the LEGACY_DEVICE_DETECTION interface object.

    InterfaceReturn - The interface object pointer.

    Filtered - Not supported for this interface.
 
Return Value:

    Returns NTSTATUS.

--*/
{
    PLEGACY_DEVICE_DETECTION_INTERFACE standard;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(LEGACY_DEVICE_DETECTION_INTERFACE)) {
        return STATUS_UNSUCCESSFUL;
    }
        
    standard = (PLEGACY_DEVICE_DETECTION_INTERFACE)InterfaceReturn;
    
    standard->Size = sizeof(LEGACY_DEVICE_DETECTION_INTERFACE);
    standard->Version = LEGACY_DEVICE_INTERFACE_VERSION;
    standard->Context = DeviceExtension;
    standard->InterfaceReference = PciLegacyDeviceInterface_Reference;
    standard->InterfaceDereference = PciLegacyDeviceInterface_Dereference;
    standard->LegacyDeviceDetection = PciLegacyDeviceInterface_GetLegacyDevice;

    return STATUS_SUCCESS;
}

VOID
PciLegacyDeviceInterface_Reference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine increments outstanding reference count for this interface

Arguments:

    PCI_BUS

Return Value:

    None

--*/
{
    PPCI_BUS bus = (PPCI_BUS)Context;

    InterlockedIncrement(&bus->PciLegacyDeviceInterfaceCount);

    return;
}

VOID
PciLegacyDeviceInterface_Dereference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine decrements outstanding reference count for this interface

Arguments:

    PCI_BUS

Return Value:

    None

--*/
{
    PPCI_BUS bus = (PPCI_BUS)Context;

    InterlockedDecrement(&bus->PciLegacyDeviceInterfaceCount);

    return;
}

NTSTATUS
PciLegacyDeviceInterface_GetLegacyDevice(
    __inout PVOID Context,
    __in INTERFACE_TYPE LegacyBusType,
    __in ULONG BusNumber,
    __in ULONG SlotNumber,
    __out PDEVICE_OBJECT *PhysicalDeviceObject
    )
/*++

Routine Description:

    This function searches for a legacy device, specified by LegacyBusType,
    BusNumber and SlotNumber, and returns a referenced physical device object
    as an output argument.

Arguments:

    Context - Supplies a pointer to the interface context.  This is actually
        the FDO for the given bus.

    LegacyBusType - PCIBus.

    BusNumber - The legacy device's bus number.

    SlotNumber - The legacy device's slot number.

    PhysicalDeviceObject - The return argument i.e. a reference physical
        device object if the corresponding legacy device is found.

Return Value:

    Returns NTSTATUS.

--*/
{
    PCI_SLOT_NUMBER slotNumber;
    PPCI_BUS bus;
    PPCI_DEVICE device;
    NTSTATUS status;

    PAGED_CODE();

    status = STATUS_UNSUCCESSFUL;

    bus = (PPCI_BUS)Context;
    PCI_ASSERT(PCI_BUS_EXTENSION(bus));

    if (LegacyBusType != PCIBus) {
        return STATUS_UNSUCCESSFUL;
    }

    if (bus->SecondaryBusNumber != BusNumber) {
        return STATUS_UNSUCCESSFUL;
    }

    slotNumber.u.AsULONG = SlotNumber;

    PciAcquirePassiveLock(&bus->ChildDeviceLock);
    
    device = bus->ChildDevices;
    while (device) {
        
        if ((device->Slot.u.bits.DeviceNumber == slotNumber.u.bits.DeviceNumber) &&
            (device->Slot.u.bits.FunctionNumber == slotNumber.u.bits.FunctionNumber)) {

            if (device->DeviceState != PciNotStarted) {
                break;
            }

            *PhysicalDeviceObject = device->DeviceObject;
            ObReferenceObject(device->DeviceObject);
            status = STATUS_SUCCESS;
            break;
        }

        device = device->Sibling;
    }

    PciReleasePassiveLock(&bus->ChildDeviceLock);

    return status;
}

