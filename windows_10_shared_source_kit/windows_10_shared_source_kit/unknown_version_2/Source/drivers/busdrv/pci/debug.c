/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    debug.c

Abstract:

    This module contains debug functions for PCI.SYS.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines

//
// This file is only compiled if debugging is enabled
//
// Disable warning 4206: nonstandard extension used : translation unit is empty
// used to make this file compile fre (even though it's completely empty)
//
#pragma warning(disable:4206)

#if DBG

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

VOID
PciDebugMsg(
    __in ULONG Level,
    __in __format_string PSTR Format,
    ...
    );

PUCHAR
PciDebugInterfaceTypeToText(
    __in ULONG InterfaceType
    );

PUCHAR
PciDebugCmResourceTypeToText(
    __in UCHAR Type
    );

BOOLEAN
PciDebugEqualResources(
    __in PPCI_DEVICE_RESOURCES First,
    __in PPCI_DEVICE_RESOURCES Second
    );

// --------------------------------------------------------------------- Pragmas
// --------------------------------------------------------------------- Globals

ULONG DebugBreakOnPnp = 0;
ULONG DebugBreakOnPower = 0;
ULONG_PTR DebugBreakOnDeviceObject = 0;

static UCHAR PnpIrpUnknownText[] = "** UNKNOWN PNP IRP Minor Code **";
static UCHAR PoIrpUnknownText[]  = "** UNKNOWN PO IRP Minor Code **";

//
// The following really begin with "IRP_MN_"
//

static PUCHAR PnpIrpTypeStrings[] = {
    
    "START_DEVICE",                // 0x00
    "QUERY_REMOVE_DEVICE",         // 0x01
    "REMOVE_DEVICE",               // 0x02
    "CANCEL_REMOVE_DEVICE",        // 0x03
    "STOP_DEVICE",                 // 0x04
    "QUERY_STOP_DEVICE",           // 0x05
    "CANCEL_STOP_DEVICE",          // 0x06

    "QUERY_DEVICE_RELATIONS",      // 0x07
    "QUERY_INTERFACE",             // 0x08
    "QUERY_CAPABILITIES",          // 0x09
    "QUERY_RESOURCES",             // 0x0A
    "QUERY_RESOURCE_REQUIREMENTS", // 0x0B
    "QUERY_DEVICE_TEXT",           // 0x0C
    "FILTER_RESOURCE_REQUIREMENTS",// 0x0D
    PnpIrpUnknownText,             // 0x0E

    "READ_CONFIG",                 // 0x0F
    "WRITE_CONFIG",                // 0x10
    "EJECT",                       // 0x11
    "SET_LOCK",                    // 0x12
    "QUERY_ID",                    // 0x13
    "QUERY_PNP_DEVICE_STATE",      // 0x14
    "QUERY_BUS_INFORMATION",       // 0x15
    "DEVICE_USAGE_NOTIFICATION",   // 0x16
    "SURPRISE_REMOVAL",            // 0x17
    PnpIrpUnknownText,             // 0x18
    "DEVICE_ENUMERATED"            // 0x19

    };

static PUCHAR PoIrpTypeStrings[] = {
    
    "WAIT_WAKE",                   // 0x00
    "POWER_SEQUENCE",              // 0x01
    "SET_POWER",                   // 0x02
    "QUERY_POWER"                  // 0x03
    };

static PUCHAR SystemPowerStateStrings[] = {
    
    "Unspecified",
    "Working",
    "Sleeping1",
    "Sleeping2",
    "Sleeping3",
    "Hibernate",
    "Shutdown"
};

static PUCHAR DevicePowerStateStrings[] = {
    "Unspecified",
    "D0",
    "D1",
    "D2",
    "D3",
    "Invalid"
};

static UCHAR InterfaceTypeUnknownText[]  = "** Unknown interface type **";

static PUCHAR InterfaceTypeText[] = {
    "InterfaceTypeUndefined",   // -1
    "Internal",                 // 0
    "Isa",                      // 1
    "Eisa",                     // 2
    "MicroChannel",             // 3
    "TurboChannel",             // 4
    "PCIBus",                   // 5
    "VMEBus",                   // 6
    "NuBus",                    // 7
    "PCMCIABus",                // 8
    "CBus",                     // 9
    "MPIBus",                   // 10
    "MPSABus",                  // 11
    "ProcessorInternal",        // 12
    "InternalPowerBus",         // 13
    "PNPISABus",                // 14
    "PNPBus",                   // 15
    "Vmcs",                     // 16
    "ACPIBus"                   // 17
    };

// ------------------------------------------------------------------- Functions

VOID
PciDebugError(
    __in __format_string PSTR Format,
    ...
    )
/*++

Routine Description:

    Prints an error message to the debugger.  This message
    will be visible if KD_PCI_MASK has its first bit set,
    which is true by default.

Arguments:

    Format - The format string to print.
  
    ... - The variable arguments that modify the format string.
                   
Return Value:

    VOID
    
--*/
{
    va_list argList;

    NON_PAGED_CODE();

    va_start(argList, Format);
    vDbgPrintExWithPrefix("PCI: ",
                          DPFLTR_PCI_ID,
                          ERROR_LEVEL,
                          Format,
                          argList
                          );

    va_end(argList);
}

VOID
PciDebugWarn(
    __in __format_string PSTR Format,
    ...
    )
/*++

Routine Description:

    Prints a warning message to the debugger.  This message
    will be visible if KD_PCI_MASK has its second bit set,
    which is not true by default.

Arguments:

    Format - The format string to print.
  
    ... - The variable arguments that modify the format string.
                   
Return Value:

    VOID
    
--*/
{
    va_list argList;

    NON_PAGED_CODE();

    va_start(argList, Format);
    vDbgPrintExWithPrefix("PCI: ",
                          DPFLTR_PCI_ID,
                          WARNING_LEVEL,
                          Format,
                          argList
                          );

    va_end(argList);
}

VOID
PciDebugInfo(
    __in __format_string PSTR Format,
    ...
    )
/*++

Routine Description:

    Prints an informational message to the debugger.  This message
    will be visible if KD_PCI_MASK has its fourth bit set,
    which is not true by default.

Arguments:

    Format - The format string to print.
  
    ... - The variable arguments that modify the format string.
                   
Return Value:

    VOID
    
--*/
{
    va_list argList;

    NON_PAGED_CODE();

    va_start(argList, Format);
    vDbgPrintExWithPrefix("PCI: ",
                          DPFLTR_PCI_ID,
                          INFO_LEVEL,
                          Format,
                          argList
                          );

    va_end(argList);
}

VOID
PciDebugTrace(
    __in ULONG Level,
    __in __format_string PSTR Format,
    ...
    )
/*++

Routine Description:

    Prints a trace message to the debugger.  This message
    will be visible if KD_PCI_MASK has either its third bit set
    or if it has the bit specified by the Level parameter set.
    Neither of these conditions are true by default.

Arguments:

    Level - A bitmask that must match KD_PCI_MASK in order for the
        message to be displayed.
        
    Format - The format string to print.
  
    ... - The variable arguments that modify the format string.
                   
Return Value:

    VOID
    
--*/
{
    va_list argList;

    NON_PAGED_CODE();

    va_start(argList, Format);
    vDbgPrintExWithPrefix("PCI: ",
                          DPFLTR_PCI_ID,
                          TRACE_LEVEL | Level,
                          Format,
                          argList
                          );
    va_end(argList);
}

VOID
PciDebugMsg(
    __in ULONG Level,
    __in __format_string PSTR Format,
    ...
    )
/*++

Routine Description:

    Prints a message to the debugger.  This message
    will be visible if KD_PCI_MASK has either its third bit set
    or if it has the bit specified by the Level parameter set.
    Neither of these conditions are true by default.
    
    This function does not prepend "PCI:" to the beginning of
    its prints, which makes it useful for functions which make
    multiple calls to it to build up one debug string.

Arguments:

    Level - A bitmask that must match KD_PCI_MASK in order for the
        message to be displayed.
        
    Format - The format string to print.
  
    ... - The variable arguments that modify the format string.
                   
Return Value:

    VOID
    
--*/
{
    va_list argList;

    NON_PAGED_CODE();

    va_start(argList, Format);
    vDbgPrintEx(DPFLTR_PCI_ID,
                TRACE_LEVEL | Level,
                Format,
                argList
                );
    va_end(argList);
}

VOID
PciDebugDumpConfigHeader(
    __in ULONG Level,
    __in PPCI_CONFIG_HEADER ConfigHeader
    )
/*++

Routine Description:

    Dumps a config header.

Arguments:

    Level - A bitmask that must match KD_PCI_MASK in order for the
        header to be dumped.
        
    ConfigHeader - The header to dump.
                   
Return Value:

    VOID
    
--*/
{
    PULONG dw;
    ULONG  i, size;

    NON_PAGED_CODE();

    dw = (PULONG)ConfigHeader;

    if (ConfigHeader->HeaderType == 2) {
        size = PCI_TYPE2_HEADER_LENGTH;    
    } else {
        size = PCI_TYPE0_OR_1_HEADER_LENGTH;
    }

    for (i = 0; i < size; i += sizeof(ULONG)) {
        PciDebugTrace(Level,"%02x - %08x\n", i, *dw);
        dw++;
    }
}

VOID
PciDebugDumpDeviceCapabilities(
    __in ULONG Level,
    __in PDEVICE_CAPABILITIES Capabilities
    )
/*++

Routine Description:

    Dumps device capabilities.

Arguments:

    Level - A bitmask that must match KD_PCI_MASK in order for the
        capabilities to be dumped.
        
    Capabilities - The capabilities to dump.
                   
Return Value:

    VOID
    
--*/
{
    SYSTEM_POWER_STATE systemWake = Capabilities->SystemWake;
    DEVICE_POWER_STATE deviceWake = Capabilities->DeviceWake;
    ULONG i;

    NON_PAGED_CODE();

    PciDebugTrace(
        Level,
        "Capabilities\n  Lock:%d, Eject:%d, Remove:%d, Dock:%d, UniqueId:%d\n",
        Capabilities->LockSupported,
        Capabilities->EjectSupported,
        Capabilities->Removable,
        Capabilities->DockDevice,
        Capabilities->UniqueID
        );
    PciDebugTrace(
        Level,
        "  SilentInstall:%d, RawOk:%d, SurpriseOk:%d\n",
        Capabilities->SilentInstall,
        Capabilities->RawDeviceOK,
        Capabilities->SurpriseRemovalOK
        );
    PciDebugTrace(
        Level,
        "  Address %08x, UINumber %08x, Latencies D1 %d, D2 %d, D3 %d\n",
        Capabilities->Address,
        Capabilities->UINumber,
        Capabilities->D1Latency,
        Capabilities->D2Latency,
        Capabilities->D3Latency
        );

    if (systemWake < PowerSystemUnspecified) {
        systemWake = PowerSystemUnspecified;
    }

    if (systemWake > PowerSystemMaximum) {
        systemWake = PowerSystemMaximum;
    }

    if (deviceWake < PowerDeviceUnspecified) {
        deviceWake = PowerDeviceUnspecified;
    }

    if (deviceWake > PowerDeviceMaximum) {
        deviceWake = PowerDeviceMaximum;
    }

    PciDebugTrace(
        Level,
        "  System Wake: %s, Device Wake: %s\n  DeviceState[PowerState] [",
        SystemPowerStateStrings[systemWake],
        DevicePowerStateStrings[deviceWake]
        );
    for (i = PowerSystemWorking;
         i < (sizeof(Capabilities->DeviceState) / sizeof(Capabilities->DeviceState[0]));
         i++) {
        deviceWake = Capabilities->DeviceState[i];
        if (deviceWake < PowerDeviceUnspecified) {
            deviceWake = PowerDeviceUnspecified;
        }
        if (deviceWake > PowerDeviceMaximum) {
            deviceWake = PowerDeviceMaximum;
        }
        PciDebugMsg(Level," %s", DevicePowerStateStrings[deviceWake]);
    }
    PciDebugMsg(Level," ]\n");
}


VOID
PciDebugDumpIoResource(
    __in ULONG Level,
    __in PIO_RESOURCE_DESCRIPTOR Descriptor
    )
/*++

Routine Description:

    Dumps an IO_RESOURCE_DESCRIPTOR.

Arguments:

    Level - A bitmask that must match KD_PCI_MASK in order for the
        capabilities to be dumped.
        
    Descriptor - The descriptor to dump.
                   
Return Value:

    VOID
    
--*/
{
    PUCHAR typeText;

    NON_PAGED_CODE();

    typeText = PciDebugCmResourceTypeToText(Descriptor->Type);
    PciDebugTrace(Level, "     IoResource Descriptor dump:  Descriptor @0x%p\n", Descriptor);
    PciDebugTrace(Level, "        Option           = 0x%x\n", Descriptor->Option);
    PciDebugTrace(Level, "        Type             = %d (%s)\n", Descriptor->Type, typeText);
    PciDebugTrace(Level, "        ShareDisposition = %d\n", Descriptor->ShareDisposition);
    PciDebugTrace(Level, "        Flags            = 0x%04X\n", Descriptor->Flags);

    PciDebugTrace(Level, "        Data = %08x  %08x  %08x\n",
                  Descriptor->u.DevicePrivate.Data[0],
                  Descriptor->u.DevicePrivate.Data[1],
                  Descriptor->u.DevicePrivate.Data[2]
                  );    
}


VOID
PciDebugDumpRequirementsList(
    __in ULONG Level,
    __in PIO_RESOURCE_REQUIREMENTS_LIST RequirementsList
    )
/*++

Routine Description:

    Dumps an IO_RESOURCE_REQUIREMENTS_LIST.

Arguments:

    Level - A bitmask that must match KD_PCI_MASK in order for the
        capabilities to be dumped.
        
    RequirementsList - The list to dump.
                   
Return Value:

    VOID
    
--*/
{
    ULONG listSize, listCount, resourceCount;
    PIO_RESOURCE_LIST list;
    PIO_RESOURCE_DESCRIPTOR resource;

    NON_PAGED_CODE();

    if (RequirementsList == NULL) {
        return;
    }

    listSize = RequirementsList->ListSize;
    listCount = RequirementsList->AlternativeLists;
    list = RequirementsList->List;

    if (listSize < sizeof(IO_RESOURCE_REQUIREMENTS_LIST)) {
        return;
    }

    listSize -= sizeof(IO_RESOURCE_REQUIREMENTS_LIST);

    PciDebugTrace(Level, "  IO_RESOURCE_REQUIREMENTS_LIST\n");
    PciDebugTrace(Level, "     InterfaceType        %d\n", RequirementsList->InterfaceType);
    PciDebugTrace(Level, "     BusNumber            0x%x\n", RequirementsList->BusNumber);
    PciDebugTrace(Level, "     SlotNumber           %d (0x%x), (d/f = 0x%x/0x%x)\n",
             RequirementsList->SlotNumber,              // in decimal
             RequirementsList->SlotNumber,              // in hex
             RequirementsList->SlotNumber & 0x1f,       // device number
             (RequirementsList->SlotNumber >> 5) & 0x7  // function
            );
    PciDebugTrace(Level, "     AlternativeLists     %d\n", listCount);

    if (listCount != 0) {
        if (listSize < ((listCount - 1) * sizeof(IO_RESOURCE_LIST))) {
            return;
        }
        listSize -= ((listCount -1) * sizeof(IO_RESOURCE_LIST));
    }

    while (listCount) {

        resource = list->Descriptors;
        resourceCount = list->Count;

        if (resourceCount != 0) {
            if (listSize < ((resourceCount -1) * sizeof(IO_RESOURCE_DESCRIPTOR))) {
                return;
            }
            listSize -= ((resourceCount -1) * sizeof(IO_RESOURCE_DESCRIPTOR));
        }

        PciDebugTrace(Level, "\n");
        PciDebugTrace(Level, "     List[%d].Count = %d\n", listCount, resourceCount);
        while (resourceCount) {
            PciDebugDumpIoResource(Level, resource++);
            resourceCount--;
        }

        list = (PIO_RESOURCE_LIST)resource;
        listCount--;
    }
    PciDebugTrace(Level, "\n");
}


VOID
PciDebugDumpCmResource(
    __in ULONG Level,
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptor
    )
/*++

Routine Description:

    Dumps a CM_PARTIAL_RESOURCE_DESCRIPTOR.

Arguments:

    Level - A bitmask that must match KD_PCI_MASK in order for the
        capabilities to be dumped.
        
    Descriptor - The descriptor to dump.
                   
Return Value:

    VOID
    
--*/
{
    ULONG  i;
    PUCHAR typeText;

    NON_PAGED_CODE();

    typeText = PciDebugCmResourceTypeToText(Descriptor->Type);
    PciDebugTrace(Level, "     Partial Resource Descriptor @0x%p\n", Descriptor);
    PciDebugTrace(Level, "        Type             = %d (%s)\n", Descriptor->Type, typeText);
    PciDebugTrace(Level, "        ShareDisposition = %d\n", Descriptor->ShareDisposition);
    PciDebugTrace(Level, "        Flags            = 0x%04X\n", Descriptor->Flags);

    for ( i = 0; i < 3; i+=3 ) {
        PciDebugTrace(Level, 
                      "        Data[%d] = %08x  %08x  %08x\n",
                      i,
                      Descriptor->u.DevicePrivate.Data[i],
                      Descriptor->u.DevicePrivate.Data[i+1],
                      Descriptor->u.DevicePrivate.Data[i+2]
                      );
    }
}

VOID
PciDebugDumpResourceList(
    __in ULONG Level,
    __in PCM_RESOURCE_LIST ResourceList
    )
/*++

Routine Description:

    Dumps a CM_RESOURCE_LIST.

Arguments:

    Level - A bitmask that must match KD_PCI_MASK in order for the
        capabilities to be dumped.
        
    ResourceList - The list to dump.
                   
Return Value:

    VOID
    
--*/
{
    ULONG descriptorCount;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor;

    NON_PAGED_CODE();
    
    descriptor = ResourceList->List[0].PartialResourceList.PartialDescriptors;
    descriptorCount = ResourceList->List[0].PartialResourceList.Count;
    
    PciDebugDumpResourceArray(Level, descriptor, descriptorCount);
}

VOID
PciDebugDumpResourceArray(
    __in ULONG Level,
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR ResourceArray,
    __in ULONG Count
    )
/*++

Routine Description:

    Dumps an array of resource descriptors
    
Arguments:

    Level - A bitmask that must match KD_PCI_MASK in order for the
        capabilities to be dumped.
        
    ResourceArray - the array of resource descriptors to dump.
    
    Count - The number of descriptors in the array
    
Return Value:

    VOID
    
--*/
{
    ULONG descriptorCount;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor;

    NON_PAGED_CODE();

    descriptorCount = Count;
    descriptor = ResourceArray;

    PciDebugTrace(Level, "  CM_RESOURCE_LIST\n");
    
    while (descriptorCount--) {
        PciDebugDumpCmResource(Level, descriptor);
        descriptor++;   
    }

    PciDebugTrace(Level, "\n");
}

VOID
PciDebugDumpBridgeRequirements(
    __in ULONG Level,
    __in PPCI_BRIDGE_REQUIREMENTS Requirements
    )
{
    PciDebugTrace(Level, 
                  "\t\tMemoryWindow: Size=0x%I64x, Alignment=0x%I64x\n",
                  Requirements->MemoryWindow.Size,
                  Requirements->MemoryWindow.Alignment
                  );
    PciDebugTrace(Level, 
                  "\t\tPrefetchWindow: Size=0x%I64x, Alignment=0x%I64x\n",
                  Requirements->PrefetchWindow.Size,
                  Requirements->PrefetchWindow.Alignment
                  );
    PciDebugTrace(Level, 
                  "\t\tIoWindow: Size=0x%I64x, Alignment=0x%I64x\n",
                  Requirements->IoWindow.Size,
                  Requirements->IoWindow.Alignment
                  );
}

VOID
PciDebugDumpArbiterAllocationState(
    __in ULONG Level,
    __in PARBITER_ALLOCATION_STATE State
    )
{
    PARBITER_ALTERNATIVE alternative;
    PCI_SLOT_NUMBER Slot;

    Slot.u.AsULONG = State->Entry->SlotNumber;

    PciDebugTrace(Level, 
                  "Arbiter Allocation State for PDO 0x%p (b:%x d:%x f:%x)\n", 
                  State->Entry->PhysicalDeviceObject, 
                  State->Entry->BusNumber, 
                  Slot.u.bits.DeviceNumber,
                  Slot.u.bits.FunctionNumber
                  );

    if (State->CurrentAlternative) {
        alternative = State->CurrentAlternative;
    } else {
        alternative = State->Alternatives;
    }

    PciDebugTrace(Level, "Current Alternative:\n");
    PciDebugTrace(Level, "\tType - %d\n", alternative->Descriptor->Type);
    PciDebugTrace(Level, "\tMinimum - 0x%I64x  Maximum - 0x%I64x\n", 
                  alternative->Minimum, alternative->Maximum);
    PciDebugTrace(Level, "\tLength - 0x%I64x  Alignment - 0x%I64x\n", 
                  alternative->Length, alternative->Alignment);
    PciDebugTrace(Level, "\tFlags - 0x%x\n", alternative->Flags);
    
    PciDebugTrace(Level, "Current State:\n");
    PciDebugTrace(Level, "\tStart - 0x%I64x  End - 0x%I64x\n", State->Start, State->End);
    PciDebugTrace(Level, "\tMin - 0x%I64x  Max - 0x%I64x\n", 
                  State->CurrentMinimum, State->CurrentMaximum);
    PciDebugTrace(Level, "\tFlags - 0x%x  RangeAttr - 0x%x  RangeAvailAttr - 0x%x\n",
                  State->Flags, State->RangeAttributes, State->RangeAvailableAttributes);
}

VOID
PciDebugCheckRegisterSettings(
    __inout PPCI_DEVICE Device
    )
{
    PCI_DEVICE_RESOURCES resources;
    PCI_SAVE_RESTORE_DATA saveRestore;
    USHORT command;

    Device->Operations->GetResources(Device, &resources);
    if (!PciDebugEqualResources(&Device->Resources, &resources)) {
        
        PciDebugWarn("Device resource settings did not stick for device 0x%p\n",Device);
    }

    PciUpdateSaveRestoreData(Device, &saveRestore);
    saveRestore.Command &= ~PCI_CONTROLLED_COMMAND_BITS;
    
    command = Device->SavedRegisters.Command;
    Device->SavedRegisters.Command &= ~PCI_CONTROLLED_COMMAND_BITS;
    if (!RtlEqualMemory(&Device->SavedRegisters, 
                        &saveRestore, 
                        sizeof(PCI_SAVE_RESTORE_DATA))) {
        
        PciDebugWarn("Device register settings did not stick for device 0x%p\n",Device);
    }
    Device->SavedRegisters.Command = command;
}

BOOLEAN
PciDebugEqualResources(
    __in PPCI_DEVICE_RESOURCES First,
    __in PPCI_DEVICE_RESOURCES Second
    )
{
    PCM_PARTIAL_RESOURCE_DESCRIPTOR firstRes, secondRes;
    ULONG i;

    for (i = 0; i < PCI_DEVICE_RESOURCE_COUNT; i++) {
        
        firstRes = &First->ByIndex[i];
        secondRes = &Second->ByIndex[i];

        //
        // Resources are equal if they have the same type and the same settings.
        // CmResourceTypeNull and 0 length resources are considered equivalent.
        //
        if (firstRes->Type == secondRes->Type) {
            
            if ((firstRes->Type != CmResourceTypeNull) &&
                !RtlEqualMemory(firstRes, secondRes, sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR))) {
                
                return FALSE;
            }
        } else if (firstRes->Type == CmResourceTypeNull) {
            
            if (((secondRes->Type != CmResourceTypePort) &&
                 (secondRes->Type != CmResourceTypeMemory) &&
                 (secondRes->Type != CmResourceTypeMemoryLarge)) ||
                (secondRes->u.Generic.Length != 0)) {
                
                return FALSE;
            }
        } else if (secondRes->Type == CmResourceTypeNull) {
            
            if (((firstRes->Type != CmResourceTypePort) &&
                 (firstRes->Type != CmResourceTypeMemory) &&
                 (firstRes->Type != CmResourceTypeMemoryLarge)) ||
                (firstRes->u.Generic.Length != 0)) {
                
                return FALSE;
            }
        } else {

            return FALSE;
        }
    }

    return TRUE;
}

VOID
PciDebugDumpCapabilitiesList(
    __in ULONG Level,
    __in PPCI_DEVICE Device
    )
{
    union _cap_buffer {
        PCI_CAPABILITIES_HEADER header;
        PCI_PM_CAPABILITY       pm;
        PCI_AGP_CAPABILITY      agp;

    } cap;

    UCHAR   capOffset;
    PUCHAR  capStr;
    ULONG   nshort;
    PUSHORT capData;

    capOffset = Device->FirstCapability;

    //
    // Run the list.
    //

    while ((capOffset != 0) && (capOffset != 0xff)) {

        PciReadDeviceConfig(
            Device,
            &cap,
            capOffset,
            sizeof(cap.header)
            );

        //
        // Depending on the Capability ID, the amount
        // of data varies.
        //

        switch (cap.header.CapabilityID) {
        case PCI_CAPABILITY_ID_POWER_MANAGEMENT:

            capStr = "POWER";
            nshort = 3;
            PciReadDeviceConfig(
                Device,
                &cap,
                capOffset,
                sizeof(cap.pm)
                );
            break;

        case PCI_CAPABILITY_ID_AGP:

            capStr = "AGP";
            nshort = 5;
            PciReadDeviceConfig(
                Device,
                &cap,
                capOffset,
                sizeof(cap.agp)
                );
            break;

        default:

            capStr = "UNKNOWN CAPABILITY";
            nshort = 0;
            break;
        }

        PciDebugTrace(
            Level,
            "CAP @%02x ID %02x (%s)",
            capOffset,
            cap.header.CapabilityID,
            capStr
            );

        capData = ((PUSHORT)&cap) + 1;

        while (nshort--) {

            PciDebugMsg(
                Level,
                "  %04x",
                *capData++
                );
        }
        PciDebugMsg(Level, "\n");

        //
        // Advance to the next entry in the list.
        //

        capOffset = cap.header.Next;
    }
}

VOID
PciDebugDumpIrpStack(
    __in PIO_STACK_LOCATION    IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension,
    __in ULONG                 MinorTableMax
    )
{
    ULONG  debugLevel;
    PUCHAR debugIrpText;

    NON_PAGED_CODE();

    //
    // Pick up the irpBreakMasks
    //
    switch(IrpSp->MajorFunction) {

        case IRP_MJ_PNP:

            debugIrpText = PciDebugPnpIrpTypeToText(IrpSp->MinorFunction);
            debugLevel = PNP_TRACE_LEVEL;
            break;

        case IRP_MJ_POWER:

            debugIrpText = PciDebugPoIrpTypeToText(IrpSp->MinorFunction);
            debugLevel = PO_TRACE_LEVEL;
            break;

        default:

            debugIrpText = "";
            debugLevel = 0;
            break;
    }

    if (PCI_DEVICE_EXTENSION(DeviceExtension)) {

        PPCI_DEVICE device = (PPCI_DEVICE)DeviceExtension;

        PciDebugTrace(
            debugLevel,
            "PDO(b=0x%x, d=0x%x, f=0x%x)<-%s\n",
            device->Parent->SecondaryBusNumber,
            device->Slot.u.bits.DeviceNumber,
            device->Slot.u.bits.FunctionNumber,
            debugIrpText
            );

    } else {

        PciDebugTrace(
            debugLevel,
            "FDO(%p)<-%s\n",
            DeviceExtension,
            debugIrpText
            );
    }

    //
    // If it's an unknown minor IRP, squirt some text to the debugger...
    //
    if (IrpSp->MinorFunction > MinorTableMax) {

        PciDebugTrace(debugLevel | INFO_LEVEL,
                      "Unknown IRP, minor = 0x%x\n",
                      IrpSp->MinorFunction
                      );
    }

    return;
}

VOID
PciDebugDumpIrpStatus(
    __in ULONG MajorFunction,
    __in NTSTATUS Status
    )
{
    ULONG debugLevel;

    NON_PAGED_CODE();

    switch(MajorFunction) {

        case IRP_MJ_PNP:

            debugLevel = PNP_TRACE_LEVEL;
            break;

        case IRP_MJ_POWER:

            debugLevel = PO_TRACE_LEVEL;
            break;

        default:

            debugLevel = 0;
            break;
    }
    
    PciDebugTrace(debugLevel, "0x%x\n", Status);
}

BOOLEAN
PciDebugBreakOnIrp(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIO_STACK_LOCATION IrpStack
    )
{
    ULONG mask;

    if (!PciDebuggingAllowed()) {
        return FALSE;
    }

    switch (IrpStack->MajorFunction) {
        
        case IRP_MJ_PNP:
            
            mask = DebugBreakOnPnp;
            break;

        case IRP_MJ_POWER:

            mask = DebugBreakOnPower;
            break;
                    
        default:
            return FALSE;
                
    }

    if ((DebugBreakOnDeviceObject == (ULONG_PTR)DeviceObject) ||
        (mask & (1 << IrpStack->MinorFunction))) {
        
        return TRUE;
    }

    return FALSE;    
}

VOID
PciDebugDumpBitMap(
    __in ULONG Level,
    __in PRTL_BITMAP BitMap
    )
{
    ULONG i;
    BOOLEAN AllZeros, AllOnes;

    PciDebugTrace(Level, " BitMap:%p", BitMap);

    PciDebugMsg(Level, " (%08x)", BitMap->SizeOfBitMap);
    PciDebugMsg(Level, " %p\n", BitMap->Buffer);

    AllZeros = FALSE;
    AllOnes = FALSE;

    for (i = 0; i < ((BitMap->SizeOfBitMap + 31) / 32); i += 1) {

        if (BitMap->Buffer[i] == 0) {

            if (AllZeros) {

                NOTHING;

            } else {

                PciDebugTrace(Level, "%4d:", i);
                PciDebugMsg(Level, " %08lx\n", BitMap->Buffer[i]);
            }

            AllZeros = TRUE;
            AllOnes = FALSE;

        } else if (BitMap->Buffer[i] == 0xFFFFFFFF) {

            if (AllOnes) {

                NOTHING;

            } else {

                PciDebugTrace(Level, "%4d:", i);
                PciDebugMsg(Level, " %08lx\n", BitMap->Buffer[i]);
            }

            AllZeros = FALSE;
            AllOnes = TRUE;

        } else {

            AllZeros = FALSE;
            AllOnes = FALSE;

            PciDebugTrace(Level, "%4d:", i);
            PciDebugMsg(Level, " %08lx\n", BitMap->Buffer[i]);
        }
    }
}

VOID
PciDebugDumpInvalidObject(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp,
    __in PUCHAR MajorString
    )
{
    NON_PAGED_CODE();

    PciDebugInfo(
        "%s IRP ( 0x%p ) for unknown or corrupted Device Object.\n",
        MajorString,
        Irp
        );

    PciDebugInfo(
        "      Device Object            0x%p\n",
        DeviceObject
        );

    PciDebugInfo(
        "      Device Object Extension  0x%p\n",
        DeviceObject->DeviceExtension
        );

    PciDebugInfo(
        "      Extension Signature      0x%08x\n",
        ((PPCI_COMMON_EXTENSION)DeviceObject->DeviceExtension)->Signature
        );
}

PUCHAR
PciDebugInterfaceTypeToText(
    __in ULONG InterfaceType
    )
{
    NON_PAGED_CODE();

    if (InterfaceType < MaximumInterfaceType) {
        PCI_ASSERT(InterfaceType + 1 < sizeof(InterfaceTypeText) / sizeof(PUCHAR));
        return InterfaceTypeText[InterfaceType + 1];
    }
    return InterfaceTypeUnknownText;
}

PUCHAR
PciDebugCmResourceTypeToText(
    __in UCHAR Type
    )
{
    NON_PAGED_CODE();

    switch (Type) {
        case CmResourceTypePort:
            return "CmResourceTypePort";
        case CmResourceTypeInterrupt:
            return "CmResourceTypeInterrupt";
        case CmResourceTypeMemory:
            return "CmResourceTypeMemory";
        case CmResourceTypeMemoryLarge:
            return "CmResourceTypeMemoryLarge";
        case CmResourceTypeDma:
            return "CmResourceTypeDma";
        case CmResourceTypeDeviceSpecific:
            return "CmResourceTypeDeviceSpecific";
        case CmResourceTypeBusNumber:
            return "CmResourceTypeBusNumber";
        case CmResourceTypeConfigData:
            return "CmResourceTypeConfigData";
        case CmResourceTypeDevicePrivate:
            return "CmResourceTypeDevicePrivate";
        case CmResourceTypePcCardConfig:
            return "CmResourceTypePcCardConfig";
        default:
            return "*** INVALID RESOURCE TYPE ***";
    }
}

PUCHAR
PciDebugPnpIrpTypeToText(
    __in ULONG IrpMinorCode
    )
{
    NON_PAGED_CODE();

    if (IrpMinorCode < (sizeof(PnpIrpTypeStrings)/sizeof(PUCHAR))) {
        return PnpIrpTypeStrings[IrpMinorCode];
    }
    return PnpIrpUnknownText;
}

PUCHAR
PciDebugPoIrpTypeToText(
    __in ULONG IrpMinorCode
    )
{
    NON_PAGED_CODE();

    if (IrpMinorCode < (sizeof(PoIrpTypeStrings)/sizeof(PUCHAR))) {
        return PoIrpTypeStrings[IrpMinorCode];
    }
    return PoIrpUnknownText;
}

#endif
