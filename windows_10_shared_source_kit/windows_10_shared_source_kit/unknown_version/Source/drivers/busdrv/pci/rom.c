/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    rom.c

Abstract:

    This module contains the code required to obtain a copy of a
    device's ROM (Read Only Memory).

    The PCI spec allows a device to share address decoding logic
    between the ROM BAR (Base Address Registers) and other BARs.
    Effectively, this means the ROM cannot be accessed at the same
    time as the device is otherwise operating.

    The ROM is accesible when both the ROM enabled bit is set and
    memory decoding is enabled.









Revision History:

--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines

#define PCI_ROM_HEADER_SIGNATURE            0xaa55
#define PCI_ROM_DATA_STRUCTURE_SIGNATURE    'RICP'  // LE PCIR

// ----------------------------------------------------------------------- Types

typedef struct _PCI_ROM_HEADER {
    USHORT  Signature;
    UCHAR   RsvdArchitectureUnique[0x16];
    USHORT  DataStructureOffset;
} PCI_ROM_HEADER, *PPCI_ROM_HEADER;

typedef struct _PCI_DATA_STRUCTURE {
    ULONG   Signature;
    USHORT  VendorId;
    USHORT  DeviceId;
    USHORT  VitalProductDataOffset;
    USHORT  DataStructureLength;
    UCHAR   DataStructureRevision;
    UCHAR   ClassCode[3];
    USHORT  ImageLength;
    USHORT  ImageRevision;
    UCHAR   CodeType;
    UCHAR   Indicator;
    USHORT  Reserved;
} PCI_DATA_STRUCTURE, *PPCI_DATA_STRUCTURE;

typedef struct _PCI_ROM_ACCESS_PARAMETERS {
    PVOID   Buffer;
    ULONG   Length;
    ULONG   RomAddress;
    PUCHAR  MappedRomAddress;
    ULONG   DisplacedBarAddress;
    ULONG   DisplacedBarIndex;
    BOOLEAN RomDisabled;    
} PCI_ROM_ACCESS_PARAMETERS, *PPCI_ROM_ACCESS_PARAMETERS;

// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciFindAddressForRom(
    __in PPCI_BUS Bus,
    __in PPCI_DEVICE Device,
    __out PPHYSICAL_ADDRESS RomAddress,
    __out PUCHAR DisplacedBarIndex,
    __out PULONG DisplacedBarAddress
    );

NTSTATUS
PciTranslateRomAddress(
    __in PPCI_DEVICE Device,
    __in PHYSICAL_ADDRESS RomAddress,
    __out PPHYSICAL_ADDRESS TranslatedAddress
    );

NTSTATUS
PciAccessRom(
    __inout PPCI_DEVICE Device,
    __inout PPCI_ROM_ACCESS_PARAMETERS RomAccessParameters
    );

NTSTATUS
PciRomTestWriteAccessToBuffer(
    _Inout_updates_opt_(Length) PUCHAR Buffer,
    _In_ ULONG Length
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciReadRomImage)
    #pragma alloc_text(PAGE, PciFindAddressForRom)
    #pragma alloc_text(PAGE, PciTranslateRomAddress)
    #pragma alloc_text(PAGE, PciRomTestWriteAccessToBuffer)
#endif

// --------------------------------------------------------------------- Globals

extern pHalTranslateBusAddress PcipSavedTranslateBusAddress;

// ------------------------------------------------------------------- Functions

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciReadRomImage(
    _In_ PPCI_DEVICE Device,
    _In_ ULONG WhichSpace,
    _Inout_updates_(*Length) PVOID Buffer,
    _In_ ULONG Offset,
    _Inout_ PULONG Length
    )
/*++

Routine Description:

    Copy a portion of the device ROM to the caller's Buffer.

Arguments:

    Device - The device in question.
    
    WhichSpace - Indicates which part of the ROM image is required.
        (Currently only the x86 BIOS image is supported, can be 
        expanded to pass back the Open FW image if needed).
    
    Buffer - Address of the caller's data area.
    
    Offset - Offset from the start of the ROM image data should
        be returned from.   Currently not used, can be used
        in the future to stage data.
    
    Length - Pointer to a ULONG containing the length of the
        Buffer (requested length).   The value is modified
        to the actual data length.


Return Value:

    Status of this operation.

--*/
{
    PCI_CRITICAL_ROUTINE_CONTEXT criticalContext;
    PCI_ROM_ACCESS_PARAMETERS romParameters;
    PIO_RESOURCE_DESCRIPTOR requirement;
    PHYSICAL_ADDRESS translatedAddress;
    PHYSICAL_ADDRESS romAddress;
    ULONG displacedBarAddress;
    PVOID mappedAddress;
    PARBITER_INSTANCE arbiter;
    UCHAR displacedBarIndex;
    ULONG transferLength;
    PUCHAR doubleBuffer;
    BOOLEAN romDisabled;
    NTSTATUS status;
    PPCI_BUS bus;
    
    PAGED_CODE();

    UNREFERENCED_PARAMETER(Offset);
    UNREFERENCED_PARAMETER(WhichSpace);

    TRACE(ROM_TRACE_LEVEL,
          "PCI ROM entered for pdox %p (buffer @ %p %08x bytes)\n",
           Device,
           Buffer,
           *Length
           );

    transferLength = *Length;
    *Length = 0;
    
    arbiter = NULL;
    mappedAddress = NULL;

    //
    // Currently not very flexible, assert we can do what the
    // caller wants.  Bridges can have ROMs, but they are not supported.
    //
    PCI_ASSERT(Offset == 0);
    PCI_ASSERT(WhichSpace == PCI_WHICHSPACE_ROM);
    
    if (Device->HeaderType != PCI_DEVICE_TYPE) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // Get the device's ROM requirement, if it has one.
    //
    requirement = &Device->Requirements.type0.RomBar;
    
    if (requirement->Type == CmResourceTypeNull) {
        return STATUS_SUCCESS;
    }
    PCI_ASSERT(requirement->Type == CmResourceTypeMemory);
    PCI_ASSERT(requirement->Flags & CM_RESOURCE_MEMORY_READ_ONLY);
    PCI_ASSERT((requirement->u.Generic.Length & 0x1ff) == 0);

    //
    // Special case.  If Length == 0 on entry, caller wants to know
    // what the length should be.
    //
    if (transferLength == 0) {
        *Length = requirement->u.Generic.Length;
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Trim length to device maximum.
    //
    if (requirement->u.Generic.Length < transferLength) {
        transferLength = requirement->u.Generic.Length;
    }

    //
    // Paranoia1:  This device is probably video.  If the system
    // bugchecks while we have the device' memory access in limbo,
    // the system will appear to hung.  Reduce the possibility of
    // bugcheck by ensuring we have (write) access to the caller's
    // buffer.
    //
    status = PciRomTestWriteAccessToBuffer(Buffer, transferLength);
    if (!NT_SUCCESS(status)) {
        ERROR("PciReadRomImage: Could not write to buffer.\n");
        return status;
    }

    //
    // If access to the ROM is already enabled, and memory is
    // currently enabled, we already have access to the image.
    // Otherwise, we need to get PnP to allocate the range.
    //
    if (Device->Resources.type0.RomBar.Type == CmResourceTypeMemory) {

        romDisabled = FALSE;
    
    } else {
        PCI_ASSERT(Device->Resources.type0.RomBar.Type == CmResourceTypeNull);
        
        romDisabled = TRUE;
    }

    //
    // If required, allocate a memory resource to access the ROM with.
    //
    displacedBarIndex = 0xFF;
    displacedBarAddress = 0;

    if (romDisabled) {

        //
        // First get the arbiter that arbitrates this device's memory resources.
        //
        bus = Device->Parent;
        do {
            
            if (!bus->MemoryArbiter) {
    
                if (bus->PciBridge && bus->PciBridge->Attributes.Subtractive) {
                        
                    //
                    // This is subtractive so we want to find the guy who
                    // arbitrates our resources (so we move on up the tree)
                    //
                    bus = bus->ParentBus;
                
                } else {
    
                    //
                    // We have a non-subtractive bridge without an arbiter -
                    // something is wrong...
                    //
                    PCI_ASSERT(bus->MemoryArbiter);
                    return STATUS_UNSUCCESSFUL;
                }
            }
        } while (!bus->MemoryArbiter);
    
        arbiter = bus->MemoryArbiter;
    
        ArbAcquireArbiterLock(arbiter);
        
        status = PciFindAddressForRom(bus,
                                      Device, 
                                      &romAddress,
                                      &displacedBarIndex,
                                      &displacedBarAddress
                                      );
        if (!NT_SUCCESS(status)) {
            goto exit;
        }

    } else {

        //
        // The ROM is currently enabled on this device, translate and
        // map the current setting.
        //
        romAddress = Device->Resources.type0.RomBar.u.Memory.Start;        
    }

    //
    // The following needs to be done regardless of whether
    // or not we had to go acquire resources.
    //
    status = PciTranslateRomAddress(Device, romAddress, &translatedAddress);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }
    
    mappedAddress = MmMapIoSpaceEx(translatedAddress,
                                   requirement->u.Generic.Length,
                                   (PAGE_NOCACHE | PAGE_READWRITE)
                                   );

    if (!mappedAddress) {
        ERROR("PciReadRomImage: Failed to map buffer.\n");
        status = STATUS_UNSUCCESSFUL;
        goto exit;
    }

    TRACE(ROM_TRACE_LEVEL,
          "ROM mapped b %08x t %08x to %p length %x bytes\n",
          romAddress.LowPart,
          translatedAddress.LowPart,
          mappedAddress,
          requirement->u.Generic.Length
          );

    //
    // Now we have the mapped address to use to access the rom. Do it.
    // This involves shutting off the device decodes.  If required,
    // do this within KeIpiGenericCall.
    //
    romParameters.RomDisabled = romDisabled;
    romParameters.RomAddress = romAddress.LowPart;
    romParameters.MappedRomAddress = (PUCHAR)mappedAddress;
    romParameters.DisplacedBarAddress = displacedBarAddress;
    romParameters.DisplacedBarIndex = displacedBarIndex;
    romParameters.Length = transferLength;

    if (romDisabled && (Device->HackFlags & PCI_HACK_CRITICAL_DEVICE)) {
            
        //
        // The caller buffer might be pageable, since this routine
        // is callable at PASSIVE_LEVEL.  So allocate a double buffer
        // to write into, and then copy the data over after the
        // KeIpiGenericCall completes.
        //
        doubleBuffer = (PUCHAR)PciAllocatePool(NonPagedPoolNx, transferLength);
        if (!doubleBuffer) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto exit;
        }
        romParameters.Buffer = doubleBuffer;
        
        criticalContext.Gate = 1;
        criticalContext.Barrier = 1;
        criticalContext.Routine = PciAccessRom;
        criticalContext.Extension = Device;
        criticalContext.Context = &romParameters;
        KeIpiGenericCall(PciExecuteCriticalSystemRoutine,
                         (ULONG_PTR)&criticalContext
                         );

        status = criticalContext.Status;

        if (romParameters.Length != 0) {
            RtlCopyMemory(Buffer, doubleBuffer, romParameters.Length);
        }
        PciFreePool(doubleBuffer);

    } else {

        romParameters.Buffer = Buffer;
        status = PciAccessRom(Device, &romParameters);
    }

    *Length = romParameters.Length;
        
exit:

    //
    // Release the arbiter lock (we're no longer using extraneous
    // resources so it should be safe to let someone else allocate
    // them.
    //
    if (arbiter) {
        ArbReleaseArbiterLock(arbiter);
    }
    
    if (mappedAddress) {
        MmUnmapIoSpace(mappedAddress, requirement->u.Generic.Length);
    }
    TRACE(ROM_TRACE_LEVEL,
          "ROM leaving pdox %p (buffer @ %p %08x bytes, status %08x)\n",
          Device,
          (PUCHAR)Buffer,
          *Length,
          status
          );
    return status;
}

NTSTATUS
PciFindAddressForRom(
    __in PPCI_BUS Bus,
    __in PPCI_DEVICE Device,
    __out PPHYSICAL_ADDRESS RomAddress,
    __out PUCHAR DisplacedBarIndex,
    __out PULONG DisplacedBarAddress
    )
/*++

Routine Description:

    This routine finds a suitable memory address to program
    into the ROM BAR of the given device for ROM access.  It
    does this by looking for a large enough hole in the memory
    window of the bus to hold the ROM BAR.  If this fails,
    it "steals" a normal memory BAR from the device, placing
    this BAR in a location outside the bus for the duration
    of the ROM access.  The lock on the bus's memory arbiter
    must be held on entry to this routine.
    
Arguments:

    Bus - The bus on which this device's resources are arbitrated.
        If the device is underneath a subtractive bridge, this
        bus may not be the device's immediate parent.
        
    Device - The device to find space for the ROM of.
    
    RomAddress - A buffer to contain the address allocated for the ROM.
    
    DisplacedBarIndex - If a memory BAR must have its range stolen
        to find space for the ROM, this is the zero-indexed number
        of the displaced BAR.
        
    DisplacedBarAddress - The location that the BAR with DisplacedBarIndex
        should be moved to in order to make space for the ROM.
        
Return Value:

    NT Status code.
    
--*/
{
    PIO_RESOURCE_DESCRIPTOR romRequirement, requirement;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR resource;
    ULONGLONG rangeMin, rangeMax, address;
    PHYSICAL_ADDRESS displacedAddress;
    PARBITER_INSTANCE arbiter;
    UCHAR displacedIndex;
    NTSTATUS status;
    UCHAR i;

    PAGED_CODE();

    arbiter = Bus->MemoryArbiter;
    romRequirement = &Device->Requirements.type0.RomBar;

    //
    // Attempt to get this resource as an additional resource
    // within the ranges supported by this bridge.
    //
    // If this is a PCI-PCI bridge then restrict this to the
    // non-prefetchable memory.
    //
    // Note: ROM BARs are 32 bit only so limit to low 4GB).
    // Note: Is is not clear that we really need to limit to
    // non-prefetchable memory.
    //
    if (Bus->PciBridge) {

        resource = &Bus->PciBridge->Resources.MemoryWindow;
        if (resource->Type == CmResourceTypeNull) {

            //
            // Bridge isn't passing memory,.... so reading
            // ROMs isn't really an option.
            //
            TRACE(ROM_TRACE_LEVEL,
                  "ROM pdo %p parent %p has no memory aperture.\n",
                  Device,
                  Bus
                  );
            return STATUS_UNSUCCESSFUL;
        }

        PCI_ASSERT(resource->Type == CmResourceTypeMemory);
        rangeMin = resource->u.Memory.Start.QuadPart;
        rangeMax = rangeMin + (resource->u.Memory.Length - 1);
    
    } else {

        rangeMin = (ULONGLONG)romRequirement->u.Memory.MinimumAddress.QuadPart;
        rangeMax = (ULONGLONG)romRequirement->u.Memory.MaximumAddress.QuadPart;
    }

    status = RtlFindRange(
                 arbiter->Allocation,
                 rangeMin,
                 rangeMax,
                 (ULONGLONG)romRequirement->u.Memory.Length,
                 romRequirement->u.Memory.Alignment,
                 0,
                 0,
                 NULL,
                 NULL,
                 &address
                 );
    if (NT_SUCCESS(status)) {

        //
        // Found a range.  We're done.
        //
        RomAddress->QuadPart = address;
        *DisplacedBarIndex = 0xFF;
        *DisplacedBarAddress = 0;

        return STATUS_SUCCESS;
    }

    //
    // If allocating a range on the bus failed, we have to
    // try to move one of the device's BARs to a range off the
    // bus and use the BAR's range for the ROM.
    //
    if (PCI_CARDBUS_BRIDGE(Bus)) {
        
        //
        // If this is a cardbus controller then game over as stealing BARS
        // is not something we encourage.
        //
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Run down the list of memory resources
    // already assigned to this device and try to find
    // one which is large enough to cover the ROM and
    // appropriate aligned.   (Note: look for the smallest
    // one meeting these requirements).
    //
    // Note: ROM BARs are only 32 bits so we cannot steal
    // a 64 bit BAR that has been assigned an address > 4GB-1.
    // We could allow the replacement range to be > 4GB-1 if
    // the BAR supports it but not on the first  pass.
    //
    displacedIndex = 0xFF;
    for (i = 0; i < PCI_TYPE0_BAR_COUNT; i++) {

        requirement = &Device->Requirements.Bars[i];
        resource = &Device->Resources.Bars[i];

        if ((requirement->Type == CmResourceTypeMemory) &&
            (requirement->u.Memory.Length >= romRequirement->u.Memory.Length) &&
            (resource->u.Memory.Start.HighPart == 0)) {
            
            if ((displacedIndex == 0xFF) ||
                (requirement->u.Memory.Length <
                 Device->Requirements.Bars[displacedIndex].u.Memory.Length)) {
                    
                displacedIndex = i;
            }
        }
    }

    if (displacedIndex == 0xFF) {
        TRACE(ROM_TRACE_LEVEL,
              "ROM pdo %p could not get MEM resource len 0x%x.\n",
              Device,
              romRequirement->u.Memory.Length
              );
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Ok, we found a suitable candidate to move.   Let's see
    // if we can find somewhere to put it that's out of the
    // way.   We do this by allowing a conflict with ranges
    // not owned by this bus.  We know the driver isn't
    // using this range at this instant so we can put it
    // somewhere where there's no way to use it then use
    // the space it occupied for the ROM.
    //
    status = RtlFindRange(arbiter->Allocation,
                          0,
                          0xffffffff,
                          (ULONGLONG)Device->Requirements.Bars[displacedIndex].u.Memory.Length,
                          Device->Requirements.Bars[displacedIndex].u.Memory.Alignment,
                          RTL_RANGE_LIST_NULL_CONFLICT_OK,
                          0,
                          NULL,
                          NULL,
                          &displacedAddress.QuadPart
                          );
    if (!NT_SUCCESS(status)) {

        //
        // We were unable to find somewhere to move the
        // memory aperture to even allowing conflicts with
        // ranges not on this bus.   This can't happen
        // unless the requirement is just plain bogus.
        //
        TRACE(ROM_TRACE_LEVEL,
              "ROM could find range to disable %x memory window.\n",
              Device->Requirements.Bars[displacedIndex].u.Memory.Length
              );
        return STATUS_UNSUCCESSFUL;
    }

    PCI_ASSERT(displacedAddress.HighPart == 0);

    TRACE(ROM_TRACE_LEVEL,
          "ROM Moving existing memory resource from %I64x to %I64x\n",
          Device->Resources.Bars[displacedIndex].u.Memory.Start.QuadPart,
          displacedAddress.QuadPart
          );
    
    *RomAddress = Device->Resources.Bars[displacedIndex].u.Memory.Start;
    *DisplacedBarIndex = displacedIndex;
    *DisplacedBarAddress = displacedAddress.LowPart;

    return STATUS_SUCCESS;
}

NTSTATUS
PciTranslateRomAddress(
    __in PPCI_DEVICE Device,
    __in PHYSICAL_ADDRESS RomAddress,
    __out PPHYSICAL_ADDRESS TranslatedAddress
    )
/*++

Routine Description:

    This routine translates a bus-relative physical address
    into a system-relative physical address using the legacy
    HAL API for doing so.  It makes two calls to the API, 
    the first passing in the bus number of the bus the device
    sits on, and the second passing in the bus number of the
    root bus the device sits under, in case the HAL doesn't
    understand the bus the device sits on and fails the first
    call.
    
Arguments:

    Device - The device for whom the ROM is being translated.
    
    RomAddress - Bus-relative physical address of the ROM.
    
    TranslatedAddress - Buffer to be filled in with the translated
        address.
        
Return Value:

    NT Status code.
    
--*/
{
    PHYSICAL_ADDRESS physicalAddress;
    BOOLEAN translated;
    ULONG ioSpace;

    PAGED_CODE();

    TranslatedAddress->QuadPart = 0;
    
    ioSpace = 0;

    //
    // Note: HalTranslateBusAddress has been hooked to call back
    // into the PCI driver which will then attempt to acquire the
    // arbiter lock on this bus.  We can't release the lock as we
    // haven't really acquired this resource we're about to use.
    // We could trick PciTranslateBusAddress into not acquiring
    // the lock by calling it at dispatch level, or, we could
    // just call the saved (prehook) HAL function which is what
    // that routine ends up doing anyway.
    //
    PCI_ASSERT(PcipSavedTranslateBusAddress);

    translated = PcipSavedTranslateBusAddress(
                     PCIBus,
                     Device->BusNumber,
                     RomAddress,
                     &ioSpace,
                     &physicalAddress
                     );

    //
    // If the resource won't translate it may be because the HAL doesn't
    // know about this bus.  Try the translation of the root bus this is 
    // under instead
    //
    if (!translated) {
        
        translated = PcipSavedTranslateBusAddress(
                         PCIBus,
                         Device->Root->Bus->SecondaryBusNumber,
                         RomAddress,
                         &ioSpace,
                         &physicalAddress
                         );
    }
    
    if (!translated) {
        ERROR("PciTranslateRomAddress: Failed to translate resource.\n");
        return STATUS_UNSUCCESSFUL;
    }

    //
    // N.B. HalTranslateBusAddress allows for this memory address
    // to be translated into an I/O address.  This is not supported
    // on any platform Windows runs on, and is not supported here.
    //
    if (ioSpace) {
        ERROR("PciTranslateRomAddress: Resource is in I/O space.\n");
        return STATUS_UNSUCCESSFUL;
    }

    TRACE(ROM_TRACE_LEVEL,
          "ROM range at %I64x translated to %I64x\n",
          RomAddress.QuadPart,
          physicalAddress.QuadPart
          );

    *TranslatedAddress = physicalAddress;

    return STATUS_SUCCESS;
}

NTSTATUS
PciAccessRom(
    __inout PPCI_DEVICE Device,
    __inout PPCI_ROM_ACCESS_PARAMETERS RomAccessParameters
    )
/*++

Routine Description:

    Does the work of programming the hardware and transferring
    the data to the caller's buffer after all the values that
    should be programmed have already been configured above.

Arguments:

    PdoExtension - Device Extension of the device in question.
    
    RomAccessParameters - A structure containing the parameters
        for the call.  These are packed into this structure format
        to fit into the calling convention required by
        PciExecuteCriticalSystemRoutine.  The parameters are:
        
        Buffer - The buffer to write the ROM data into
        Length - On input, the length of the buffer.  On output,
            contains the length of the data transferred.
        OriginalRomAddress - The address programmed into the ROM BAR
            when we started.
        NewRomAddress - The new address to program into the ROM BAR
            to transfer the data.
        MappedRomAddress - A mapping of the ROM BAR for data transfer.
        DisplacedBarIndex - If space for the ROM was found by stealing
            space from a BAR, the index of the BAR that was displaced.
            Otherwise, MAXULONG.
        NewBarAddress - If DisplacedBarIndex is valid, the address
            to write into the displaced BAR to prevent it from decoding.
        AcquiredResources - TRUE if we had to find space for the ROM somewhere.
            FALSE if the ROM was already active and decoding when we found it.
        OriginalStatusCommand - The value of the status and command registers
            when we found the device.


Return Value:

    STATUS_SUCCESS.  The routine only returns a value to match
    the calling convention of PciExecuteCriticalSystemRoutine.

--*/
{
    ULONG lengthTransferred, transferLength;
    PCI_DATA_STRUCTURE dataStructure;
    PCI_ROM_HEADER header;
    ULONG romBar, oldRomBar;
    PUCHAR bufferPointer;
    PUCHAR imageBase;
    ULONG imageLength;
    ULONG barIndex;
    ULONG command;

    NON_PAGED_CODE();

    barIndex = RomAccessParameters->DisplacedBarIndex;

    if (RomAccessParameters->RomDisabled) {
        
        PciDisconnectDevice(Device);
    
        //
        // If we have to move a memory aperture to access the ROM
        // do so now.
        //
        if (barIndex < PCI_TYPE0_BAR_COUNT) {
    
            PciWriteConfigRegister(Device,
                                   type0.BaseAddresses[barIndex],
                                   &RomAccessParameters->DisplacedBarAddress
                                   );
        }
    
        //
        // Save the current value in the ROM BAR before whacking it.
        //
        PciReadConfigRegister(Device, type0.ROMBaseAddress, &oldRomBar);
        
        romBar = RomAccessParameters->RomAddress | PCI_ROMADDRESS_ENABLED;
    
        PciWriteConfigRegister(Device, type0.ROMBaseAddress, &romBar);
    
        //
        // Enable Memory access to this device.
        //
        command = PCI_ENABLE_MEMORY_SPACE;
    
        PciWriteConfigRegister(Device, Command, &command);
    }
    
    //
    // Copy the ROM to the caller's buffer.
    //
    imageBase = RomAccessParameters->MappedRomAddress;
    bufferPointer = (PUCHAR)RomAccessParameters->Buffer;
    transferLength = RomAccessParameters->Length;
    lengthTransferred = 0;

    do {
    
        //
        // Get the header, check signature.
        //
        RtlCopyMemory(&header, imageBase, sizeof(header));
    
        if (header.Signature != PCI_ROM_HEADER_SIGNATURE) {
    
            //
            // Not a valid ROM image, don't transfer anything.
            //
            TRACE(ROM_TRACE_LEVEL,
                  "ROM invalid signature, offset %X, expected %04x, got %04x\n",
                  (ULONG)(imageBase - (PUCHAR)RomAccessParameters->MappedRomAddress),
                  PCI_ROM_HEADER_SIGNATURE,
                  header.Signature
                  );
            break;
        }
    
        //
        // Get image data structure, check its signature and
        // get actual length.
        //
        RtlCopyMemory(&dataStructure,
                      imageBase + header.DataStructureOffset,
                      sizeof(dataStructure)
                      );
    
        if (dataStructure.Signature != PCI_ROM_DATA_STRUCTURE_SIGNATURE) {
    
            //
            // Invalid data structure, bail.
            //
            TRACE(ROM_TRACE_LEVEL,
                  "ROM invalid signature, offset %x, expected %08x, got %08x\n",
                  (ULONG)(imageBase - 
                          (PUCHAR)RomAccessParameters->MappedRomAddress
                          + header.DataStructureOffset),
                  PCI_ROM_DATA_STRUCTURE_SIGNATURE,
                  dataStructure.Signature
                  );
            break;
        }
    
        //
        // Image length is in units of 512 bytes.   We presume
        // it's from the start of this image, ie imageBase, not
        // from the start of the code because that wouldn't make
        // sense.
        //
        imageLength = dataStructure.ImageLength * 512;
    
        if (imageLength > transferLength) {
    
            //
            // Truncate to available buffer space.
            //
            imageLength = transferLength;
        }
    
        //
        // Transfer this image to the caller's buffer.
        //
        RtlCopyMemory(bufferPointer, imageBase, imageLength);
    
        //
        // Update pointers etc
        //
        bufferPointer += imageLength;
        lengthTransferred += imageLength;
        imageBase += imageLength;
        transferLength -= imageLength;
    
        if (dataStructure.Indicator & 0x80) {
    
            //
            // Indicator bit 7 == 1 means this was the last image.
            //
            break;
        }
    } while (transferLength);

    
    if (RomAccessParameters->RomDisabled) {
    
        //
        // Disable memory decoding.
        //
        command = 0;

        PciWriteConfigRegister(Device, Command, &command);

        //
        // If we moved someone to make room for the ROM, put them
        // back where they started off.  Also put the ROM itself back.
        //
        PciWriteConfigRegister(Device, 
                               type0.ROMBaseAddress, 
                               &oldRomBar
                               );

        if (barIndex < PCI_TYPE0_BAR_COUNT) {
    
            PciWriteConfigRegister(Device,
                                   type0.BaseAddresses[barIndex],
                                   &Device->Resources.Bars[barIndex].u.Memory.Start.LowPart
                                   );
        }
        
        PciReconnectDevice(Device);        
    }

    RomAccessParameters->Length = lengthTransferred;

    return STATUS_SUCCESS;
}

NTSTATUS
PciRomTestWriteAccessToBuffer(
    _Inout_updates_opt_(Length) PUCHAR Buffer,
    _In_ ULONG Length
    )
/*++

Routine Description:

    Complete Paranoia.  Make sure we can write every page in the
    caller's buffer (assumes 4096 bytes per page) by writing to
    every page.

    We do this in a try block to avoid killing the system.  The
    hope is to avoid anything that might bugcheck the system while
    we have changed the operating characteristics of the device.

Arguments:

    Buffer - Address of start of buffer.
    
    Length - Number of bytes in buffer.

Return Value:

    Status.

--*/
{
    PUCHAR endAddress;

    PAGED_CODE();

    endAddress = Buffer + Length - 1;

    try {

        while (Buffer <= endAddress) {
            *Buffer = 0;
            Buffer += 0x1000;
        }
        *endAddress = 0;

#pragma prefast(suppress:__WARNING_UNREACHABLE_CODE)
    } except (EXCEPTION_EXECUTE_HANDLER) {
        return GetExceptionCode();
    }
    return STATUS_SUCCESS;
}
