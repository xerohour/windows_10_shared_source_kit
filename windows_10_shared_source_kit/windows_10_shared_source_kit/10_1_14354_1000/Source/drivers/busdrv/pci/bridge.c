/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    bridge.c

Abstract:

    This module contains functions for configuring PCI-PCI bridges











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines

#define PCI_INITIALIZE_BRIDGE_REQUIREMENTS(_Reqs)        \
{                                                        \
    PPCI_BRIDGE_REQUIREMENTS _localReqs = (_Reqs);       \
    _localReqs->IoWindow.Disabled = FALSE;               \
    _localReqs->MemoryWindow.Disabled = FALSE;           \
    _localReqs->PrefetchWindow.Disabled = FALSE;         \
    _localReqs->IoWindow.Size = 0;                       \
    _localReqs->MemoryWindow.Size = 0;                   \
    _localReqs->PrefetchWindow.Size = 0;                 \
    _localReqs->IoWindow.Alignment = 1;                  \
    _localReqs->MemoryWindow.Alignment = 1;              \
    _localReqs->PrefetchWindow.Alignment = 1;            \
    _localReqs->IoWindow.LengthBelow4GB = 0;             \
    _localReqs->MemoryWindow.LengthBelow4GB = 0;         \
    _localReqs->PrefetchWindow.LengthBelow4GB = 0;       \
}

#define PCI_EXPAND_WINDOW_FOR_ISA_BIT       0x1

#define PCI_ALIGN_AT_GIGABYTE_BOUNDARY      0x40000000

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciQueryBridgeInterface(
    __in PPCI_BRIDGE Bridge,
    __out PPCI_BRIDGE_INTERFACE Interface
    );

VOID
PciBridgeGetDefaultRequirements(
    __in PPCI_BRIDGE Bridge,
    __out PPCI_BRIDGE_REQUIREMENTS PreferredRequirements,
    __out PPCI_BRIDGE_REQUIREMENTS FallbackRequirements
    );

VOID
PciBridgeAddRequirements(
    __in PPCI_BRIDGE Bridge,
    __inout PPCI_BRIDGE_REQUIREMENTS Destination,
    __in PPCI_BRIDGE_REQUIREMENTS Source,
    __in ULONG Flags
    );

VOID
PciBridgeFilterRequirementsListInterrupts(
    __in PPCI_BRIDGE Bridge,
    __inout PIO_RESOURCE_REQUIREMENTS_LIST Requirements
    );

VOID
PciBridgeModifyInterruptAffinity (
    _Inout_ PIO_RESOURCE_DESCRIPTOR InputDesc
    );

VOID
PciBridgeFilterRequirement(
    _In_ PPCI_BRIDGE Bridge,
    _Inout_ PIO_RESOURCE_DESCRIPTOR Fixed,
    _Inout_ PIO_RESOURCE_DESCRIPTOR Flexible,
    _Inout_ PIO_RESOURCE_DESCRIPTOR Alternate
    );

ULONGLONG
PciBridgeMaximumWindowAlignment(
    __in ULONGLONG Length
    );

NTSTATUS
PciBridgeGetRequirementsForDevice(
    __in PPCI_BRIDGE Bridge,
    __in PPCI_DEVICE Device,
    __out PPCI_BRIDGE_REQUIREMENTS Requirements,
    __out PPCI_BRIDGE_REQUIREMENTS Subtractive
    );

PCI_INTERFACE_CONSTRUCTOR PciBridgeInterface_Constructor;
BRIDGE_GET_POWER_PARAMETERS PciBridgeInterface_GetPowerParameters;

NTSTATUS
PciDeviceResourcesToBridgeResources(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_RESOURCES DeviceResources,
    __out PPCI_BRIDGE_RESOURCES BridgeResources
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciProcessNewBridgedBus)
    #pragma alloc_text(PAGE, PciQueryBridgeInterface)
    #pragma alloc_text(PAGE, PciBridgeStart)
    #pragma alloc_text(PAGE, PciBridgeProcessStartResources)
    #pragma alloc_text(PAGE, PciBridgeGetDefaultRequirements)
    #pragma alloc_text(PAGE, PciBridgeAddRequirements)
    #pragma alloc_text(PAGE, PciBridgeCheckLocationCompatibility)
    #pragma alloc_text(PAGE, PciBridgeFilterRequirementsList)
    #pragma alloc_text(PAGE, PciBridgeFilterRequirementsListInterrupts)
    #pragma alloc_text(PAGE, PciBridgeModifyInterruptAffinity)
    #pragma alloc_text(PAGE, PciBridgeComputeRequirements)
    #pragma alloc_text(PAGE, PciBridgeFilterRequirement)
    #pragma alloc_text(PAGE, PciBridgeMaximumWindowAlignment)
    #pragma alloc_text(PAGE, PciBridgeGetRequirementsForDevice)
    #pragma alloc_text(PAGE, PciBridgeInterface_Constructor)
    #pragma alloc_text(PAGE, PciBridgeInterface_GetPowerParameters)
    #pragma alloc_text(PAGE, PciBridgeReevaluateWindows)
    #pragma alloc_text(PAGE, PciDeviceResourcesToBridgeResources)
    #pragma alloc_text(PAGE, PciDeviceRequirementsToBridgeRequirements)
#endif

// --------------------------------------------------------------------- Globals

const PCI_INTERFACE PciBridgeInterface = {
    &GUID_PCI_BRIDGE_INTERFACE,         // InterfaceType
    sizeof(PCI_BRIDGE_INTERFACE),       // MinSize
    1,                                  // MinVersion
    2,                                  // MaxVersion
    PCIIF_PDO,                          // Flags
    PciInterface_Bridge,                // Signature
    PciBridgeInterface_Constructor      // Constructor
};

// ------------------------------------------------------------------- Functions

NTSTATUS
PciProcessNewBridgedBus(
    __inout PPCI_BUS Bus,
    __in PCI_SLOT_NUMBER Slot
    )
/*++

Routine Description:

    This routine allocates and initializes a PCI_BRIDGE structure
    for a bridged PCI bus (either a PCI-PCI bridge or cardbus bridge).

Arguments:

    Bus - The bus extension for the bridge.

    Slot - The slot where the bridge sits on its primary bus.

Return Value:

    NT Status code.

--*/
{

    UCHAR baseClass, subClass, headerType;
    PPCI_BRIDGE bridge;
    PCI_EXPRESS_CAPABILITIES_REGISTER expressCapabilities;
    PCI_BRIDGE_INTERFACE interface;
    USHORT segmentNumber;
    NTSTATUS status;

    PAGED_CODE();

    RtlZeroMemory(&interface, sizeof(PCI_BRIDGE_INTERFACE));

    bridge = NULL;
    Bus->PciBridge = PciAllocatePool(NonPagedPoolNx, sizeof(PCI_BRIDGE));
    if (!Bus->PciBridge) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }
    bridge = Bus->PciBridge;

    RtlZeroMemory(bridge, sizeof(PCI_BRIDGE));

    bridge->Bus = Bus;
    bridge->Slot.u.AsULONG = Slot.u.AsULONG;

    //
    // Verify that the device actually looks like a PCI-PCI or cardbus bridge.
    //
    PciReadBridgeConfigRegister(bridge, BaseClass, &baseClass);
    PciReadBridgeConfigRegister(bridge, SubClass, &subClass);
    PciReadBridgeConfigRegister(bridge, HeaderType, &headerType);

    headerType &= ~PCI_MULTIFUNCTION;

    if ((headerType != PCI_BRIDGE_TYPE) &&
        (headerType != PCI_CARDBUS_BRIDGE_TYPE)) {

        PCI_ASSERT((headerType == PCI_BRIDGE_TYPE) ||
                   (headerType == PCI_CARDBUS_BRIDGE_TYPE));

        ERROR("AddDevice for Non-Root/Non-PCI-PCI bridge,\n"
              "    HeaderType %02x will not add.\n",
              headerType
              );

        status = STATUS_INVALID_DEVICE_REQUEST;
        goto cleanup;
    }

    bridge->HeaderType = headerType;

    //
    // Now process the bridge interface.  This gets the data out of the
    // PDO that is required to understand the constraints of this bridge.
    //
    status = PciQueryBridgeInterface(Bus->PciBridge, &interface);
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    bridge->InterfaceContext = interface.Context;
    bridge->Attributes.Subtractive = interface.Subtractive;
    bridge->Attributes.IsaBitSet = interface.IsaBitSet;
    bridge->Attributes.LegacyVgaBitSet = interface.LegacyVgaBitSet;
    if (interface.IgnoreBootConfig == 1) {
        PCI_BUS_SET_FLAG(Bus, IgnoreBootConfig);
    } else {
        PCI_BUS_CLEAR_FLAG(Bus, IgnoreBootConfig);
    }

    if (interface.BusmasterDisabledOnBoot == 0) {
        PCI_BUS_CLEAR_FLAG(Bus, BusmasterDisabledOnBoot);
    } else {
        PCI_BUS_SET_FLAG(Bus, BusmasterDisabledOnBoot);
    }

    RtlCopyMemory(&bridge->MinimumRequirements,
                  &interface.MinimumRequirements,
                  sizeof(PCI_BRIDGE_REQUIREMENTS)
                  );

    TRACE(RESOURCE_TRACE_LEVEL,
          "Bridge 0x%p has attributes 0x%x\n",
          bridge,
          bridge->AttributesAsULONG
          );

    RtlCopyMemory(&bridge->BiosConfiguration,
                  &interface.BiosConfiguration,
                  sizeof(PCI_BRIDGE_RESOURCES)
                  );

    bridge->PciXCapability = interface.PciXCapability;
    bridge->ExpressCapability = interface.ExpressCapability;
    bridge->DownstreamDeviceNotRemovable =
        interface.DownstreamDeviceNotRemovable;

    //
    // The interface bus number represents the legacy bus number read/saved
    // off the hardware. Update this with segment information and stash away
    // the fully encoded bus number in the PCI_BUS extension.
    //

    if (interface.BusNumbersConfigured) {
        segmentNumber =
            SEGMENT_NUMBER_FOR_BUS(Bus->ParentBus->SecondaryBusNumber);

        Bus->SecondaryBusNumber =
            FULL_BUS_NUMBER(segmentNumber, interface.SecondaryBusNumber);

        Bus->SubordinateBusNumber =
            FULL_BUS_NUMBER(segmentNumber, interface.SubordinateBusNumber);

    } else {

        //
        // Bus numbers couldn't be configured in the PDO.  This
        // is fatal - fail add device.
        //
        status = STATUS_INVALID_DEVICE_REQUEST;
        goto cleanup;
    }

    //
    // Determine whether devices downstream of this bridge need to pass data
    // over hypertransport to get it to the root of the machine.  This is true
    // either if the bridge itself is a hypertransport tunnel or if some
    // bridge upstream of it is.  Also determine whether downstream devices
    // can send PCI MSI and expect them to be converted to HT interrupts by
    // a conversion element somewhere on the path between the device and the
    // root.
    //
    if (interface.HyperTransport ||
        (Bus->ParentBus->PciBridge &&
         Bus->ParentBus->PciBridge->HyperTransportPath)) {

        bridge->HyperTransportPath = 1;
    }

    if (interface.MsiHtConverter ||
        (Bus->ParentBus->PciBridge &&
         Bus->ParentBus->PciBridge->MsiHtConversionPath)) {

        bridge->MsiHtConversionPath = 1;
    }

    bridge->D3Causes66MHzB2 = interface.D3Causes66MHzB2;
    bridge->AvoidChildD1D2ForSld = interface.AvoidChildD1D2ForSld;
    bridge->DisableChildPowerManagement = interface.DisableChildPowerManagement;
    if (interface.ExtendedConfigAvailable == 1) {
        PCI_BUS_SET_FLAG(Bus, ExtendedConfigAvailable);
    } else {
        PCI_BUS_CLEAR_FLAG(Bus, ExtendedConfigAvailable);
    }

    bridge->EnableAllBridgeInterrupts = interface.EnableAllBridgeInterrupts;
    PciBridgeGetDefaultRequirements(bridge,
                                    &bridge->PreferredRequirements,
                                    &bridge->FallbackRequirements
                                    );

    PCI_INITIALIZE_BRIDGE_REQUIREMENTS(&Bus->PciBridge->SubtractiveRequirements);

    //
    // Determine whether the path upstream of this bridge is entirely Express,
    // and whether the bridge is or is somewhere beneath an Express root port.
    // These properties affect the RIDs seen at an IOMMU for transactions from
    // devices downstream of the bridge.
    //

    if (PCI_ROOT_BUS(Bus->ParentBus) ||
        (Bus->ParentBus->PciBridge->PathIsExpress &&
         (Bus->ParentBus->SecondaryInterface == PciExpress))) {

        bridge->PathIsExpress = 1;
        if (!PCI_ROOT_BUS(Bus->ParentBus) &&
            Bus->ParentBus->PciBridge->RootPortDomain) {

            bridge->RootPortDomain = 1;

        } else if (bridge->ExpressCapability != 0) {
            PciReadExpressBridgeRegisterEx(bridge,
                                           ExpressCapabilities,
                                           &expressCapabilities);

            if (expressCapabilities.DeviceType == PciExpressRootPort) {
                bridge->RootPortDomain = 1;
            }
        }
    }

    if ((bridge->ExpressCapability != 0) &&
        ((Bus->Root->HardwareCapability.OscControlGranted.u.AsULONG != 0) ||
         (PciSystemWideHackFlags & PCI_SYS_HACK_CONTROL_EXPRESS_FEATURES))) {

        bridge->DisableLinkPowerManagement =
            interface.DisableLinkPowerManagement;

        bridge->ExpressBaseVersion11OrGreater =
            interface.ExpressBaseVersion11OrGreater;

        bridge->SecondaryBusResetOnLinkStateChange =
            interface.SecondaryBusResetOnLinkStateChange;

        bridge->LinkDisableOnSlotPowerDown =
            interface.LinkDisableOnSlotPowerDown;

        bridge->E2ETLPPrefixSupported = interface.E2ETLPPrefixSupported;

        bridge->AtomicsRoutingSupported = interface.AtomicsRoutingSupported;
        bridge->Atomics32BitCompleterSupported = interface.Atomics32BitCompleterSupported;
        bridge->Atomics64BitCompleterSupported = interface.Atomics64BitCompleterSupported;
        bridge->Atomics128BitCompleterSupported = interface.Atomics128BitCompleterSupported;

        status = ExpressProcessNewVirtualBridge(bridge,
                                                &interface,
                                                &bridge->ExpressBridge);
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }
    }

    bridge->AcsCapIncorrectFormat = interface.AcsCapIncorrectFormat;
    bridge->DelayPassive = interface.DelayPassive;
    bridge->StallDispatch = interface.StallDispatch;

    interface.InterfaceDereference(interface.Context);
    return status;

cleanup:

    if (bridge) {
        PciFreePool(bridge);
        bridge = NULL;
        Bus->PciBridge = NULL;
    }

    if (interface.Context) {
        interface.InterfaceDereference(interface.Context);
    }

    return status;
}

NTSTATUS
PciQueryBridgeInterface(
    __in PPCI_BRIDGE Bridge,
    __out PPCI_BRIDGE_INTERFACE Interface
    )
/*++

Routine Description:

    This routine queries for a PCI_BRIDGE_INTERFACE from the device stack.
    The PCI_BRIDGE_INTERFACE describes the structure and resource limitations of
    the bridge to allow the FDO to make intelligent decisions about the resources
    that should be assigned.  This interface is exposed by the PDO.

Arguments:

    Bridge - The bridge to get the interface for.

    Interface - A pointer to a buffer to fill in with the interface.

Return Value:

    NT Status code

--*/
{
    NTSTATUS status;
    IO_STACK_LOCATION irpStack;

    PAGED_CODE();

    irpStack.MajorFunction = IRP_MJ_PNP;
    irpStack.MinorFunction = IRP_MN_QUERY_INTERFACE;

    irpStack.Parameters.QueryInterface.InterfaceType = (LPGUID)&GUID_PCI_BRIDGE_INTERFACE;
    irpStack.Parameters.QueryInterface.Version = 2;
    irpStack.Parameters.QueryInterface.Size = sizeof(PCI_BRIDGE_INTERFACE);
    irpStack.Parameters.QueryInterface.Interface = (PINTERFACE)Interface;
    irpStack.Parameters.QueryInterface.InterfaceSpecificData = NULL;

    status = PciSendPnpIrp(Bridge->Bus->PhysicalDeviceObject,
                           &irpStack,
                           NULL
                           );
    return status;
}

NTSTATUS
PciBridgeStart(
    __inout PPCI_BRIDGE Bridge,
    __in PCM_RESOURCE_LIST RawResources,
    __in PCM_RESOURCE_LIST TranslatedResources
    )
/*++

Routine Description:

    This routine is the start device routine for a PCI-PCI bridge.
    It parses the interesting resources from the incoming resource list
    and configures the arbiters with these resources.  It also sets some
    flags based on how the BridgeControl register has been configured.

Parameters:

    Bridge - The PCI-PCI bridge to configure.

    Resources - The incoming start resources.

Return Value:

    NT Status code.

--*/
{
    NTSTATUS status;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(TranslatedResources);

    //
    // If the bridge has resources, convert the PnP resource list into
    // the internal structure describing the bridge windows.  If there
    // is no PnP resource list for the bridge, there are no bridge windows.
    // In this case the internal representation of the bridge windows must
    // contain the type CmResourceTypeNull.
    //

    RtlZeroMemory(&Bridge->Resources, sizeof(PCI_BRIDGE_RESOURCES));
    if (RawResources != NULL) {
        status = PciBridgeProcessStartResources(
                    Bridge,
                    RawResources->List[0].PartialResourceList.PartialDescriptors,
                    RawResources->List[0].PartialResourceList.Count,
                    &Bridge->Resources
                    );

        if (!NT_SUCCESS(status)) {
            TRACE(RESOURCE_TRACE_LEVEL,
                  "PciBridgeStart: Failure to process start resources for bridge 0x%p\n",
                  Bridge
                  );
            return status;
        }
    }

    if (RtlEqualMemory(&Bridge->BiosConfiguration.IoWindow,
                       &Bridge->Resources.IoWindow,
                       sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)
                       )) {

        Bridge->IoWindowOnBootConfig = TRUE;

    } else {

        Bridge->IoWindowOnBootConfig = FALSE;
    }

    if (Bridge->ExpressBridge != NULL) {
        status = ExpressBridgeStart(Bridge->ExpressBridge,
                                    RawResources,
                                    TranslatedResources);

        if (!NT_SUCCESS(status)) {
            return status;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciBridgePowerUp (
    __in PPCI_BRIDGE Bridge
    )
{

    NTSTATUS Status;

    NON_PAGED_CODE();

    if (Bridge->ExpressBridge != NULL) {
        Status = ExpressBridgePowerUp(Bridge->ExpressBridge);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciBridgePowerDown (
    __in PPCI_BRIDGE Bridge
    )
{

    NTSTATUS Status;

    NON_PAGED_CODE();

    if (Bridge->ExpressBridge != NULL) {
        Status = ExpressBridgePowerDown(Bridge->ExpressBridge);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciBridgeProcessStartResources(
    __in PPCI_BRIDGE Bridge,
    __in_ecount(StartResourceCount) PCM_PARTIAL_RESOURCE_DESCRIPTOR StartResources,
    __in ULONG StartResourceCount,
    __out PPCI_BRIDGE_RESOURCES BridgeResources
    )
/*++

Routine Description:

    This routine converts a Start resource list into the
    internal representation of PCI_BRIDGE_RESOURCES.

Parameters:

    Bridge - The PCI-PCI bridge to configure.

    StartResources - The array of CM_PARTIAL_RESOURCE_DESCRIPTORs
        that came in the start IRP.

    StartResourceCount - The number of elements in StartResources.

    BridgeResources - Pointer to a PCI_BRIDGE_RESOURCES structure
        that will contain the processed resources on successful exit.

Return Value:

    NT Status code.

--*/
{
    PCM_PARTIAL_RESOURCE_DESCRIPTOR resource;
    ULONG vgaIndex = 0;

    PAGED_CODE();

    RtlZeroMemory(BridgeResources, sizeof(PCI_BRIDGE_RESOURCES));

    FOR_ALL_IN_ARRAY(StartResources, StartResourceCount, resource) {

        //
        // Skip resources we don't care about.
        //
        if (((resource->Type != CmResourceTypeMemory) &&
             (resource->Type != CmResourceTypeMemoryLarge) &&
             (resource->Type != CmResourceTypePort)) ||
            (resource->u.Generic.Length == 0)) {

            continue;
        }

        if (((resource->Type == CmResourceTypeMemory) ||
             (resource->Type == CmResourceTypeMemoryLarge)) &&
            (resource->Flags & CM_RESOURCE_MEMORY_WINDOW_DECODE)) {

            if (resource->Flags & CM_RESOURCE_MEMORY_PREFETCHABLE) {

                PCI_ASSERT(BridgeResources->PrefetchWindow.Type == CmResourceTypeNull);
                RtlCopyMemory(&BridgeResources->PrefetchWindow,
                              resource,
                              sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)
                              );
            } else {

                PCI_ASSERT(BridgeResources->MemoryWindow.Type == CmResourceTypeNull);
                RtlCopyMemory(&BridgeResources->MemoryWindow,
                              resource,
                              sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)
                              );
            }
        } else if ((resource->Type == CmResourceTypePort) &&
                   (resource->Flags & CM_RESOURCE_PORT_WINDOW_DECODE)) {

            PCI_ASSERT(BridgeResources->IoWindow.Type == CmResourceTypeNull);
            RtlCopyMemory(&BridgeResources->IoWindow,
                          resource,
                          sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)
                          );

        } else if (PciIsVgaResource(resource)) {

            PCI_ASSERT(vgaIndex < PCI_BRIDGE_VGA_RESOURCE_COUNT);
            if (vgaIndex < PCI_BRIDGE_VGA_RESOURCE_COUNT) {
                RtlCopyMemory(&BridgeResources->VgaRange[vgaIndex],
                              resource,
                              sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)
                              );
                vgaIndex++;
            }

        }
    }

    TRACE(RESOURCE_TRACE_LEVEL, "PciBridgeProcessStartResources for 0x%p:\n", Bridge);
    TRACE(RESOURCE_TRACE_LEVEL, "MemoryWindow\n");
    DebugDumpCmResource(RESOURCE_TRACE_LEVEL, &BridgeResources->MemoryWindow);
    TRACE(RESOURCE_TRACE_LEVEL, "PrefetchWindow\n");
    DebugDumpCmResource(RESOURCE_TRACE_LEVEL, &BridgeResources->PrefetchWindow);
    TRACE(RESOURCE_TRACE_LEVEL, "IoWindow\n");
    DebugDumpCmResource(RESOURCE_TRACE_LEVEL, &BridgeResources->IoWindow);
    TRACE(RESOURCE_TRACE_LEVEL, "VGA Ranges\n");
    for (vgaIndex=0;vgaIndex<PCI_BRIDGE_VGA_RESOURCE_COUNT;vgaIndex++) {
        DebugDumpCmResource(RESOURCE_TRACE_LEVEL, &BridgeResources->VgaRange[vgaIndex]);
    }

    return STATUS_SUCCESS;
}

VOID
PciBridgeGetDefaultRequirements(
    __in PPCI_BRIDGE Bridge,
    __out PPCI_BRIDGE_REQUIREMENTS PreferredRequirements,
    __out PPCI_BRIDGE_REQUIREMENTS FallbackRequirements
    )
/*++

Routine Description:

    This routine selects the default requirements for a bridge.  These
    requirements are selected without knowledge of the devices on the
    bus underneath the bridge.  They respect BIOS-configs, and otherwise
    use best guesses.

Arguments:

    Bridge - The bridge to compute the requirements for.

    PreferredRequirements - These are the resources that the bridge would
        like to get if the machine has space to grant them.

    FallbackRequirements - These are the minimum resources that the bridge
        will be able to start with.

Return Value:

    VOID

--*/
{
    PPCI_BRIDGE_WINDOW_REQUIREMENT minimum, preferred, fallback;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR biosConfig;
    ULONGLONG defaultSize, fallbackSize;
    ULONG window;

    PAGED_CODE();

    for (window = 0; window < 3; window++) {

        switch (window) {

        case 0:

            //
            // I/O
            //
            preferred = &PreferredRequirements->IoWindow;
            fallback = &FallbackRequirements->IoWindow;
            biosConfig = &Bridge->BiosConfiguration.IoWindow;
            minimum = &Bridge->MinimumRequirements.IoWindow;

            //
            // We want to try to give a bridge I/O if possible to minimize
            // the number of rebalances that might occur.  But if it isn't
            // available, this shouldn't prevent the bridge from starting.
            //

            defaultSize = minimum->Size;
            fallbackSize = 0;
            break;

        case 1:

            //
            // Memory
            //
            preferred = &PreferredRequirements->MemoryWindow;
            fallback = &FallbackRequirements->MemoryWindow;
            biosConfig = &Bridge->BiosConfiguration.MemoryWindow;
            minimum = &Bridge->MinimumRequirements.MemoryWindow;
            defaultSize = minimum->Size;
            fallbackSize = 0;
            break;

        case 2:

            //
            // Prefetch
            //
            preferred = &PreferredRequirements->PrefetchWindow;
            fallback = &FallbackRequirements->PrefetchWindow;
            biosConfig = &Bridge->BiosConfiguration.PrefetchWindow;
            minimum = &Bridge->MinimumRequirements.PrefetchWindow;
            defaultSize = minimum->Size;
            fallbackSize = 0;
            break;

        default:

            PCI_ASSERT(FALSE);
            return;;
        }

        //
        // If the window is disabled, respect that.
        // Otherwise, the BIOS config is present, use it.
        //
        if (minimum->Disabled) {

            preferred->Disabled = TRUE;
            fallback->Disabled = TRUE;

        } else if (biosConfig->Type != CmResourceTypeNull) {

            preferred->Disabled = FALSE;
            fallback->Disabled = FALSE;

            preferred->Alignment = minimum->Alignment;
            fallback->Alignment = minimum->Alignment;

            preferred->Size = RtlCmDecodeMemIoResource(biosConfig, NULL);
            fallback->Size = preferred->Size;

        } else {

            preferred->Disabled = FALSE;
            fallback->Disabled = FALSE;

            preferred->Size = defaultSize;
            preferred->Alignment = PciBridgeMaximumWindowAlignment(defaultSize);
            fallback->Size = fallbackSize;
            fallback->Alignment = PciBridgeMaximumWindowAlignment(fallbackSize);
        }
    }

    PciOverrideBridgeRequirements(Bridge,
                                  PreferredRequirements,
                                  FallbackRequirements);

}

NTSTATUS
PciBridgeComputeRequirements(
    __inout PPCI_BRIDGE Bridge,
    __inout PPCI_BRIDGE_REQUIREMENTS PreferredRequirements,
    __inout PPCI_BRIDGE_REQUIREMENTS FallbackRequirements,
    __inout PPCI_BRIDGE_REQUIREMENTS SubtractiveRequirements
    )
/*++

Routine Description:

    This function computes the requirements of devices on the
    secondary bus of a bridge, including all BARs and windows.

    NB - this first draft ignores prefetchable memory and 64 bit windows.

Parameters:

    Bridge - The bridge for whom these requirements were generated.

    PreferredRequirements - A buffer to be filled in with the preferred
        bridge requirements.

    FallbackRequirements - A buffer to be filled in with the fallback
        bridge requirements.

    SubtractiveRequirements - A buffer to be filled in with the requirements
        of a subtractive bridge that do not belong inside the bridge windows.

Return Value:

    NT Status code.

--*/
{
    PPCI_BRIDGE_WINDOW_REQUIREMENT preferred, fallback, minimum, oldPreferred;
    PCI_BRIDGE_REQUIREMENTS localPreferred, localFallback, localSubtractive;
    PCI_BRIDGE_REQUIREMENTS deviceReqs, deviceSubtractive;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR biosConfig;
    PPCI_BUS bus, childBus;
    PPCI_DEVICE device;
    NTSTATUS status;
    ULONG window;
    ULONG requirementCount;

    PAGED_CODE();

    PCI_INITIALIZE_BRIDGE_REQUIREMENTS(&localPreferred);
    PCI_INITIALIZE_BRIDGE_REQUIREMENTS(&localFallback);
    PCI_INITIALIZE_BRIDGE_REQUIREMENTS(&localSubtractive);

    //
    // The default initialization (above) is to enable all the window
    // requirements. If a prefetchable memory BAR is not implemented by a
    // bridge, then the prefetchable requirements will be added to the memory
    // requirements.
    //

    if (Bridge->MinimumRequirements.PrefetchWindow.Disabled != FALSE) {
        localPreferred.PrefetchWindow.Disabled = TRUE;
        localFallback.PrefetchWindow.Disabled = TRUE;

        //
        // localSubtractive is a cumulation of requirements that are not
        // arbitrated by this bridge. So don't bother setting the Disabled
        // field for the localSubtractive.
        //

    }

    //
    // If this is a subtractive bridge, leave the I/O windows closed.  The
    // possible performance improvement that could come from making a cycle
    // going to a child go down a positive-decode window isn't worth the
    // headache of opening up a bridge window on I/O space, which is such a
    // scarce resource.
    //

    if (Bridge->Attributes.Subtractive == 1) {
        localPreferred.IoWindow.Disabled = TRUE;
        localFallback.IoWindow.Disabled = TRUE;
        localSubtractive.IoWindow.Disabled = TRUE;
    }

    bus = Bridge->Bus;
    requirementCount = 0;

    TRACE(RESOURCE_TRACE_LEVEL, "PciBridgeComputeRequirements for bridge 0x%p\n", Bridge);

    PciAcquirePassiveLock(&bus->ChildDeviceLock);
    for (device = bus->ChildDevices; device != NULL; device = device->Sibling) {

        //
        // First get the window requirements for the device.
        //
        status = PciBridgeGetRequirementsForDevice(Bridge,
                                                   device,
                                                   &deviceReqs,
                                                   &deviceSubtractive
                                                   );
        if (!NT_SUCCESS(status)) {
            PciReleasePassiveLock(&bus->ChildDeviceLock);
            return status;
        }

        //
        // Force memory/prefetch windows to open up in case the device is an
        // SR-IOV device and requires memory/prefetch resources.
        // This is because some BIOS'es accidentally disable these windows by
        // not considering VF BAR requirements.
        //

        if (device->ExpressPort != NULL) {
            if (device->ExpressPort->SriovCapability != 0) {
                if(Bridge->MinimumRequirements.MemoryWindow.Disabled &&
                   deviceReqs.MemoryWindow.Size > 0) {

                    Bridge->MinimumRequirements.MemoryWindow.Disabled = FALSE;

                    Bridge->MinimumRequirements.MemoryWindow.Size =
                        PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;

                    Bridge->MinimumRequirements.MemoryWindow.Alignment =
                        PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
                }

                if(Bridge->MinimumRequirements.PrefetchWindow.Disabled &&
                   deviceReqs.PrefetchWindow.Size > 0 ) {

                    Bridge->MinimumRequirements.PrefetchWindow.Disabled = FALSE;

                    Bridge->MinimumRequirements.PrefetchWindow.Size =
                        PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;

                    Bridge->MinimumRequirements.PrefetchWindow.Alignment =
                        PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
                }
            }
        }

        requirementCount += 1;
        PciBridgeAddRequirements(Bridge,
                                 &localPreferred,
                                 &deviceReqs,
                                 PCI_EXPAND_WINDOW_FOR_ISA_BIT);

        PciBridgeAddRequirements(Bridge,
                                 &localSubtractive,
                                 &deviceSubtractive,
                                 PCI_EXPAND_WINDOW_FOR_ISA_BIT);

        TRACE(RESOURCE_TRACE_LEVEL,
              "PciComputeChildResources: Processed Device 0x%p\n",
              device
              );
        DebugDumpBridgeRequirements(RESOURCE_TRACE_LEVEL,
                                    &deviceReqs
                                    );
        DebugDumpBridgeRequirements(RESOURCE_TRACE_LEVEL,
                                    &deviceSubtractive
                                    );
    }

    PciReleasePassiveLock(&bus->ChildDeviceLock);

    //
    // Child *devices* have fixed requirements, so the preferred and fallback
    // requirements are the same.  This will change when we process bridges below.
    //
    RtlCopyMemory(&localFallback, &localPreferred, sizeof(PCI_BRIDGE_REQUIREMENTS));

    //
    // In addition to the child device resources, now look at the child
    // bus resources for the required windows of child busses.  Requirements are
    // computed in QueryStop, which is sent by PnP from the bottom of the tree up.
    // As a result, this routine can just grab the updated values from its
    // child busses.  It gets recursion for free.
    //
    PciAcquirePassiveLock(&PciGlobalLock);
    for (childBus = bus->ChildBuses; childBus != NULL; childBus = childBus->SiblingBus) {

        PCI_ASSERT(childBus->PciBridge);
        if (childBus->PciBridge) {

            //
            // Open up memory/prefetch windows if a child bridge has these windows enabled
            // and the current bridge does not.
            //

            if(Bridge->MinimumRequirements.MemoryWindow.Disabled &&
               !childBus->PciBridge->MinimumRequirements.MemoryWindow.Disabled) {

                Bridge->MinimumRequirements.MemoryWindow.Disabled = FALSE;
            }

            if(Bridge->MinimumRequirements.PrefetchWindow.Disabled &&
               !childBus->PciBridge->MinimumRequirements.PrefetchWindow.Disabled) {

                Bridge->MinimumRequirements.PrefetchWindow.Disabled = FALSE;
            }

            //
            // Only open the I/O window if the child needs more space *and* the
            // parent is doing positive decode.
            //

            if((Bridge->Attributes.Subtractive == 0) &&
               Bridge->MinimumRequirements.IoWindow.Disabled &&
               !childBus->PciBridge->MinimumRequirements.IoWindow.Disabled) {

                Bridge->MinimumRequirements.IoWindow.Disabled = FALSE;
            }

            requirementCount += 1;
            PciBridgeAddRequirements(Bridge,
                                     &localPreferred,
                                     &childBus->PciBridge->PreferredRequirements,
                                     0
                                     );

            PciBridgeAddRequirements(Bridge,
                                     &localFallback,
                                     &childBus->PciBridge->FallbackRequirements,
                                     0
                                     );

            if (Bridge->Attributes.Subtractive) {

                PciBridgeAddRequirements(Bridge,
                                         &localSubtractive,
                                         &childBus->PciBridge->SubtractiveRequirements,
                                         0
                                         );
            } else {

                PciBridgeAddRequirements(Bridge,
                                         &localPreferred,
                                         &childBus->PciBridge->SubtractiveRequirements,
                                         0
                                         );

                PciBridgeAddRequirements(Bridge,
                                         &localFallback,
                                         &childBus->PciBridge->SubtractiveRequirements,
                                         0
                                         );
            }


            TRACE(RESOURCE_TRACE_LEVEL,
                  "PciComputeChildResources: Processed Child Bridge 0x%p\n",
                  childBus->PciBridge
                  );
            TRACE(RESOURCE_TRACE_LEVEL, "\tPreferred:\n");
            DebugDumpBridgeRequirements(RESOURCE_TRACE_LEVEL,
                                        &localPreferred
                                        );
            TRACE(RESOURCE_TRACE_LEVEL, "\tFallback:\n");
            DebugDumpBridgeRequirements(RESOURCE_TRACE_LEVEL,
                                        &localFallback
                                        );
            TRACE(RESOURCE_TRACE_LEVEL, "\tSubtractive:\n");
            DebugDumpBridgeRequirements(RESOURCE_TRACE_LEVEL,
                                        &localSubtractive
                                        );
        }
    }

    PciReleasePassiveLock(&PciGlobalLock);

    //
    // Now that we have the requirements, sanitize them against the BIOS
    // config, the hardware minimums, and the old requirements.
    // Note that we don't have to do this for the subtractive requirements,
    // since these are only computed in order to be rolled up into the
    // actual requirements of a higher-level bridge.  When that occurs, the
    // code below will kick in.
    //
    for (window = 0; window < 3; window++) {

        switch (window) {
        case 0:
            preferred = &localPreferred.IoWindow;
            fallback = &localFallback.IoWindow;
            oldPreferred = &PreferredRequirements->IoWindow;
            biosConfig = &Bridge->BiosConfiguration.IoWindow;
            minimum = &Bridge->MinimumRequirements.IoWindow;
            break;
        case 1:
            preferred = &localPreferred.MemoryWindow;
            fallback = &localFallback.MemoryWindow;
            oldPreferred = &PreferredRequirements->MemoryWindow;
            biosConfig = &Bridge->BiosConfiguration.MemoryWindow;
            minimum = &Bridge->MinimumRequirements.MemoryWindow;
            break;
        case 2:
            preferred = &localPreferred.PrefetchWindow;
            fallback = &localFallback.PrefetchWindow;
            oldPreferred = &PreferredRequirements->PrefetchWindow;
            biosConfig = &Bridge->BiosConfiguration.PrefetchWindow;
            minimum = &Bridge->MinimumRequirements.PrefetchWindow;
            break;
        default:
            PCI_ASSERT(FALSE);
            return STATUS_UNSUCCESSFUL;
        }

        //
        // If windows are disabled, keep them that way.
        //
        if (minimum->Disabled) {
            preferred->Disabled = TRUE;
            fallback->Disabled = TRUE;
            continue;
        }

        preferred->Disabled = FALSE;
        fallback->Disabled = FALSE;

        //
        // Never shrink a window past the BIOS config.
        // Keep the computed alignment, though.
        //
        if (biosConfig->Type != CmResourceTypeNull) {

            if (biosConfig->u.Generic.Length == 0) {
                preferred->Size = 0;
                fallback->Size = 0;
            } else {
                ULONGLONG biosLength;

                biosLength = RtlCmDecodeMemIoResource(biosConfig, NULL);
                preferred->Size = max(preferred->Size, biosLength);
                fallback->Size = max(fallback->Size, biosLength);
            }
        }

        //
        // Respect the alignments we just computed, but make sure they're
        // at least the minimums.
        //
        preferred->Alignment = max(preferred->Alignment, minimum->Alignment);
        fallback->Alignment = max(fallback->Alignment, minimum->Alignment);

        //
        // Now make sure that the sizes are at least the minimum,
        // and are rounded up to the right alignments.  A zero length
        // window may be smaller than the minimum, but allow that.
        //
        if ((preferred->Size != 0) || (requirementCount == 0)) {
            preferred->Size = max(preferred->Size, minimum->Size);

            if(minimum->Alignment) {
                preferred->Size = ALIGN_RANGE_UP(preferred->Size, minimum->Alignment);
            } else {

                //
                // If the minimum alignment was not set
                // (because this window was disabled originally),
                // use the preferred alignment to round up the size.
                //

                preferred->Size = ALIGN_RANGE_UP(preferred->Size, preferred->Alignment);
            }
        }

        if ((fallback->Size != 0) || (requirementCount == 0)) {
            fallback->Size = max(fallback->Size, minimum->Size);

            if(minimum->Alignment) {
                fallback->Size = ALIGN_RANGE_UP(fallback->Size, minimum->Alignment);
            } else {

                //
                // If the minimum alignment was not set
                // (because this window was disabled originally),
                // use the fallback alignment to round up the size.
                //

                fallback->Size = ALIGN_RANGE_UP(fallback->Size, fallback->Alignment);
            }
        }

        //
        // Finally, only shrink windows when really necessary.  If the newly
        // computed preferred size is smaller than the old one, keep the old
        // preferred size around and "fallback" to the new size if necessary.
        // In this case, the fallback requirements aren't used.
        //
        if ((preferred->Size < oldPreferred->Size) &&
            (preferred->Alignment <= oldPreferred->Alignment)) {

            *fallback = *preferred;
            *preferred = *oldPreferred;
        }

        //
        // Update the LengthBelow4GB field in case the Size/Alignment has changed.
        //

        if ((preferred->Size != 0) && (preferred->LengthBelow4GB != 0)) {
            preferred->LengthBelow4GB =
                ALIGN_RANGE_UP(preferred->LengthBelow4GB,
                               (ULONGLONG)preferred->Alignment);

            if (preferred->LengthBelow4GB > preferred->Size) {
                preferred->LengthBelow4GB = preferred->Size;
            }
        }

        if ((fallback->Size != 0) && (fallback->LengthBelow4GB != 0)) {
            fallback->LengthBelow4GB =
                ALIGN_RANGE_UP(fallback->LengthBelow4GB,
                               (ULONGLONG)fallback->Alignment);

            if (fallback->LengthBelow4GB > fallback->Size) {
                fallback->LengthBelow4GB = fallback->Size;
            }
        }
    }

    RtlCopyMemory(PreferredRequirements, &localPreferred, sizeof(PCI_BRIDGE_REQUIREMENTS));
    RtlCopyMemory(FallbackRequirements, &localFallback, sizeof(PCI_BRIDGE_REQUIREMENTS));
    RtlCopyMemory(SubtractiveRequirements, &localSubtractive, sizeof(PCI_BRIDGE_REQUIREMENTS));

    Bridge->GotRequirements = TRUE;

    return STATUS_SUCCESS;
}

VOID
PciBridgeFilterRequirementsListInterrupts(
        __in PPCI_BRIDGE Bridge,
        __inout PIO_RESOURCE_REQUIREMENTS_LIST Requirements
        )
/*++

Routine Description:

    This function filters interrupt requirements for a bridge. When the PCI
    driver is loaded as the FDO on a bridge, it connects at most one interrupt,
    and in many cases doesn't connect any interrupts. This function filters
    the number of interrupts requested to the minimum number required.

Parameters:

    Bridge - The bridge for whom these requirements were generated.

    Requirements - The requirements list that came in the
        IRP_MN_FILTER_RESOURCE_REQUIREMENTS IRP.

Return Value:

    None.

--*/

{

    PEXPRESS_BRIDGE ExpressBridge;
    ULONG Index;
    PIO_RESOURCE_DESCRIPTOR InputDesc;
    PIO_RESOURCE_LIST InputList;
    ULONG InputOffset;
    ULONG InterruptsDesired;
    ULONG InterruptsLeft;
    ULONG MsiCount;
    BOOLEAN RetainAlternatives;

    PAGED_CODE();

    //
    // If a third-party driver has installed itself as a filter it may
    // invoke the PciEnableAllBridgeInterrupts section from machine.inf
    // to disable filtering of PCI Bridge interrupts.
    //

    if (Bridge->EnableAllBridgeInterrupts) {
        return;
    }

    //
    // Calculate the number of interrupts the PCI driver will need to connect
    // on start.
    //

    InterruptsDesired = 0;
    RetainAlternatives = FALSE;
    ExpressBridge = Bridge->ExpressBridge;
    if (ExpressBridge != NULL) {
        switch (ExpressBridge->BridgeType) {
        case PciExpressRootPort:
            InterruptsDesired = 1;
            break;

        case PciExpressDownstreamSwitchPort:
            if (ExpressBridge->Downstream.HotPlugCapable != FALSE) {
                InterruptsDesired = 1;
            }

            break;
        }

    } else if (Bridge->HeaderType == PCI_CARDBUS_BRIDGE_TYPE) {
        InterruptsDesired = 1;
    }

    //
    // Alternative descriptors are only retained if the number of desired
    // interrupts is non-zero.
    //

    if (InterruptsDesired != 0) {
        RetainAlternatives = TRUE;
    }

    //
    // Iterate through each alternative list, removing interrupts beyond the
    // number determined above.
    //

    for (Index = 0,
         InputList = &Requirements->List[0];
         Index < Requirements->AlternativeLists;
         Index++,
         InputList = (PIO_RESOURCE_LIST)&InputList->Descriptors[InputList->Count]) {


        //
        // Iterate through each entry in this alternative, counting the number
        // of interrupts used and removing those beyond the number desired.
        // beyond the number.
        //

        InterruptsLeft = InterruptsDesired;
        for (InputOffset = 0; InputOffset < InputList->Count; InputOffset++) {
            InputDesc = &InputList->Descriptors[InputOffset];
            if (InputDesc->Type != CmResourceTypeInterrupt) {
                continue;
            }

            //
            // Do not remove alternative interrupt resource descriptors if any
            // interrupts were desired.
            // 

            if ((RetainAlternatives != FALSE) &&
                (InputDesc->Option & IO_RESOURCE_ALTERNATIVE)) {

                continue;
            }

            //
            // If no more interrupts are needed, just null out any interrupt
            // descriptors remaining in this list.
            //

            if (InterruptsLeft == 0) {
                NullResourceRequirement(InputDesc);
                continue;
            }

            PciBridgeModifyInterruptAffinity(InputDesc);

            //
            // If this is a line-based entry, it represents a single vector
            // so just decrement the number left and continue.
            //

            if (!(InputDesc->Flags & CM_RESOURCE_INTERRUPT_MESSAGE)) {
                InterruptsLeft--;
                continue;
            }

            //
            // If this is an MSI entry, it may represent more than one vectors.
            // Determined how many by looking at the maximum and minimum
            // vector values, and adjust the minimum to limit the number used.
            //

            MsiCount = InputDesc->u.Interrupt.MaximumVector -
                       InputDesc->u.Interrupt.MinimumVector + 1;

            if (MsiCount > InterruptsLeft) {
                InputDesc->u.Interrupt.MinimumVector =
                    InputDesc->u.Interrupt.MaximumVector - InterruptsLeft + 1;

                InterruptsLeft = 0;

            } else {
                InterruptsLeft -= MsiCount;
            }
        }
    }

    return;
}

VOID
PciBridgeModifyInterruptAffinity (
    _Inout_ PIO_RESOURCE_DESCRIPTOR Resource
    )

/*++

Routine Description:

    This routine modifies the affinity policy of an interrupt resource,
    depending on the system configuration.

Arguments:

    Resource - Supplies an interrupt resource to modify.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PCI_ASSERT(Resource->Type == CmResourceTypeInterrupt);

#if defined(_AMD64_) || defined(_X86_)

    if (PciInterruptSteeringEnabled == FALSE) {

        //
        // Set the AffinityPolicy to only target one processor to
        // reduce interrupt vector usage.
        //

        Resource->Flags |= CM_RESOURCE_INTERRUPT_POLICY_INCLUDED;
        Resource->u.Interrupt.AffinityPolicy = IrqPolicyOneCloseProcessor;
    }

#else

    UNREFERENCED_PARAMETER(Resource);

#endif

    return;
}
NTSTATUS
PciBridgeFilterRequirementsList(
    __in PPCI_BRIDGE Bridge,
    __in PIO_RESOURCE_REQUIREMENTS_LIST InputRequirements,
    __out PIO_RESOURCE_REQUIREMENTS_LIST *FilteredRequirements
    )
/*++

Routine Description:

    This function filters a requirements list generated by the PDO
    to comprehend the knowledge of the secondary bus that the
    FDO has.  It loops through the initial requirements list, looking
    for requirements that correspond to bridge windows.  It then
    calls PciBridgeFilterRequirement to update the requirement.  This
    may involve stripping the preferred config or the requirement
    altogether.  If this happens for any requirements, this routine
    allocates a new list and copies over the non-deleted requirements
    to the new list.

Parameters:

    Bridge - The bridge for whom these requirements were generated.

    InputRequirements - The requirements list that came in the
        IRP_MN_FILTER_RESOURCE_REQUIREMENTS IRP.

    FilteredRequirements - On successful exit, will point to a
        IO_RESOURCE_REQUIREMENTS_LIST containing the new, filtered
        requirements.  It is the caller's responsibility to free
        this memory.

Return Value:

    NT Status code.

--*/
{
    IO_RESOURCE_DESCRIPTOR fixedBuffer, flexibleBuffer, alternateBuffer;
    PIO_RESOURCE_DESCRIPTOR inputDesc, flexible, fixed, first;
    PIO_RESOURCE_LIST alternativeList, inputList;
    PIO_RESOURCE_REQUIREMENTS_LIST requirements;
    ULONG index, offset, inputOffset;
    ULONG totalDescriptors;
    NTSTATUS status;
    ULONG size;

    PAGED_CODE();

    PciBridgeFilterRequirementsListInterrupts(Bridge, InputRequirements);

    *FilteredRequirements = NULL;

    TRACE(RESOURCE_TRACE_LEVEL,
          "PciBridgeFilterRequirementsList for bridge 0x%p\n",
          Bridge
          );
    TRACE(RESOURCE_TRACE_LEVEL, "Initial List:\n");
    DebugDumpRequirementsList(RESOURCE_TRACE_LEVEL, InputRequirements);

    //
    // Allocate a new buffer for the requirements to be copied into.
    // If there is an error and this routine returns without filtering
    // the requirements, the original InputRequirements must be intact.
    // in this scratch pad, leave enough space for a few extra requirements
    // that may or may not get added.  There could be two per bridge window,
    // meaning six per list.
    //
    size = InputRequirements->ListSize
           + (InputRequirements->AlternativeLists
              * 6
              * sizeof(IO_RESOURCE_DESCRIPTOR)
              );
    if ((size < InputRequirements->ListSize) ||
        (size < (InputRequirements->AlternativeLists
                 * 6
                 * sizeof(IO_RESOURCE_DESCRIPTOR)))) {
        return STATUS_UNSUCCESSFUL;
    }
    requirements = PciAllocatePool(PagedPool, size);
    if (!requirements) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Copy the input list into the new buffer, starting with the list
    // headers and then going through alternative list by alternative list.
    //
    RtlCopyMemory(requirements,
                  InputRequirements,
                  FIELD_OFFSET(IO_RESOURCE_REQUIREMENTS_LIST, List)
                  );
    requirements->ListSize = size;
    requirements->AlternativeLists = 0;

    totalDescriptors = 0;

    inputList = &InputRequirements->List[0];
    alternativeList = &requirements->List[0];

    for (index = 0; index < InputRequirements->AlternativeLists; index++) {

        alternativeList->Version = inputList->Version;
        alternativeList->Revision = inputList->Revision;

        offset = 0;
        inputOffset = 0;
        while (inputOffset < inputList->Count) {

            inputDesc = &inputList->Descriptors[inputOffset];

            //
            // Bridge windows are identified by the WINDOW_DECODE flag.  Any
            // requirements that are not marked in this way don't get filtered.
            // Just copy them over wholesale.
            //
            if (!((((inputDesc->Type == CmResourceTypeMemory) ||
                    (inputDesc->Type == CmResourceTypeMemoryLarge)) &&
                   (inputDesc->Flags & CM_RESOURCE_MEMORY_WINDOW_DECODE)) ||
                  ((inputDesc->Type == CmResourceTypePort) &&
                   (inputDesc->Flags & CM_RESOURCE_PORT_WINDOW_DECODE)))) {

                //
                // If it's not a window descriptor, copy it over unmodified.
                //
                RtlCopyMemory(&alternativeList->Descriptors[offset],
                              inputDesc,
                              sizeof(IO_RESOURCE_DESCRIPTOR)
                              );
                inputOffset++;
                offset++;
                continue;
            }

            //
            // This is a requirement for a bridge window.  The input list
            // could have a fixed requirement for a specific range, and a generic
            // requirement for a specific size of window that can live anywhere.
            // Based on this, and the requirements that have already been computed,
            // turn these into some combination of a fixed, generic and alternate
            // (fallback) requirement.
            //
            PCI_ASSERT(inputDesc->Option != IO_RESOURCE_ALTERNATIVE);
            if (inputDesc->Option == IO_RESOURCE_ALTERNATIVE) {

                RtlCopyMemory(&alternativeList->Descriptors[offset],
                              inputDesc,
                              sizeof(IO_RESOURCE_DESCRIPTOR)
                              );
                inputOffset++;
                offset++;
                continue;
            }

            //
            // Figure out whether we have just a flexible config for this
            // requirement, or also a fixed requirement.  Copy these window
            // requirements into scratch buffers that can be modified
            // by the filtering routine.
            //
            flexible = inputDesc;
            fixed = NULL;
            inputOffset++;
            if (inputOffset != (inputList->Count - 1)) {

                if ((inputDesc + 1)->Option == IO_RESOURCE_ALTERNATIVE) {
                    fixed = inputDesc;
                    flexible = inputDesc + 1;
                    inputOffset++;
                }
            }

            NullResourceRequirement(&fixedBuffer);
            if (fixed) {
                RtlCopyMemory(&fixedBuffer,
                              fixed,
                              sizeof(IO_RESOURCE_DESCRIPTOR)
                              );
            }
            fixedBuffer.Option = 0;

            NullResourceRequirement(&flexibleBuffer);
            if (flexible) {
                RtlCopyMemory(&flexibleBuffer,
                              flexible,
                              sizeof(IO_RESOURCE_DESCRIPTOR)
                              );
            }
            flexibleBuffer.Option = 0;

            NullResourceRequirement(&alternateBuffer);

            //
            // Now do the actual filtering.  The filter routine marks requirements
            // CmResourceTypeNull if they are not needed.  Based on the result of
            // the filter, copy the right requirements into the output buffer.
            //
            PciBridgeFilterRequirement(Bridge,
                                       &fixedBuffer,
                                       &flexibleBuffer,
                                       &alternateBuffer
                                       );

            if ((fixedBuffer.Type == CmResourceTypeNull) &&
                (flexibleBuffer.Type == CmResourceTypeNull) &&
                (alternateBuffer.Type == CmResourceTypeNull)) {

                //
                // There's no requirement.  Just leave a NULL placeholder
                // in the list.
                //
                NullResourceRequirement(&alternativeList->Descriptors[offset]);
                offset++;

            } else {

                first = NULL;
                if (fixedBuffer.Type != CmResourceTypeNull) {

                    RtlCopyMemory(&alternativeList->Descriptors[offset],
                                  &fixedBuffer,
                                  sizeof(IO_RESOURCE_DESCRIPTOR)
                                  );

                    first = &alternativeList->Descriptors[offset];

                    offset++;
                }
                if (flexibleBuffer.Type != CmResourceTypeNull) {

                    if (first != NULL) {
                        first->Option = IO_RESOURCE_PREFERRED;
                        flexibleBuffer.Option = IO_RESOURCE_ALTERNATIVE;
                    } else {
                        first = &alternativeList->Descriptors[offset];
                    }

                    RtlCopyMemory(&alternativeList->Descriptors[offset],
                                  &flexibleBuffer,
                                  sizeof(IO_RESOURCE_DESCRIPTOR)
                                  );
                    offset++;
                }
                if (alternateBuffer.Type != CmResourceTypeNull) {

                    if (first != NULL) {
                        first->Option = IO_RESOURCE_PREFERRED;
                        alternateBuffer.Option = IO_RESOURCE_ALTERNATIVE;
                    }
                    RtlCopyMemory(&alternativeList->Descriptors[offset],
                                  &alternateBuffer,
                                  sizeof(IO_RESOURCE_DESCRIPTOR)
                                  );
                    offset++;
                }
            }
        }

        alternativeList->Count = offset;
        alternativeList = (PIO_RESOURCE_LIST)
                          &alternativeList->Descriptors[alternativeList->Count];

        requirements->AlternativeLists++;
        totalDescriptors += offset;

        inputList = (PIO_RESOURCE_LIST)&inputList->Descriptors[inputList->Count];
    }

    *FilteredRequirements = requirements;

    //
    // Finally, try to save some memory by re-allocating a buffer of the right size.
    //
    if (requirements->AlternativeLists != 0) {

        //
        // Compute the size the buffer would have to be to hold the new requirements.
        // This could be somewhat less than or exactly equal to the size that was
        // originally allocated.  If the required size is smaller, allocate a new
        // buffer and copy the old one over.
        //
        if (totalDescriptors == 0) {
            size = sizeof(IO_RESOURCE_REQUIREMENTS_LIST);
        } else {
            size = sizeof(IO_RESOURCE_REQUIREMENTS_LIST)
                   + (sizeof(IO_RESOURCE_LIST) * (requirements->AlternativeLists - 1))
                   + ((totalDescriptors - requirements->AlternativeLists)
                      * sizeof(IO_RESOURCE_DESCRIPTOR)
                      );
        }

        if (size != requirements->ListSize) {

            *FilteredRequirements = PciAllocatePool(PagedPool, size);
            if (!*FilteredRequirements) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                goto cleanup;
            }

            RtlCopyMemory(*FilteredRequirements, requirements, size);
            (*FilteredRequirements)->ListSize = size;
            PciFreePool(requirements);
        }
    }

    TRACE(RESOURCE_TRACE_LEVEL, "Filtered List:\n");
    DebugDumpRequirementsList(RESOURCE_TRACE_LEVEL, *FilteredRequirements);
    return STATUS_SUCCESS;


cleanup:

    if (requirements) {
        PciFreePool(requirements);
    }

    return status;
}

VOID
PciBridgeAddRequirements(
    __in PPCI_BRIDGE Bridge,
    __inout PPCI_BRIDGE_REQUIREMENTS Destination,
    __in PPCI_BRIDGE_REQUIREMENTS Source,
    __in ULONG Flags
    )
/*++

Routine Description:

    This routine "adds" the two provided bridge requirements.  For
    each window in the requirements structure, the sum of the requirements
    is the sum of the window sizes and the maximum of the window alignments.

Arguments:

    Bridge - The bridge that the destination requirements are for.

    Destination - One of the two requirements operands, and also holds
        the result of the operation.

    Source - The second operand.

Return Value:

    VOID

--*/
{
    PAGED_CODE();

    //
    // And add them to the total window size.  Size is cumulative, but the
    // required alignment is just the maximum of all the required alignments.
    // If the bridge doesn't have a prefetchable window, put the child prefetchable
    // requirements into the normal memory window.
    // If the prefetch window is still disabled at this point, add prefetch
    // window requirements to memory window requirements.
    //

    if (Bridge->MinimumRequirements.PrefetchWindow.Disabled != FALSE) {

        Destination->MemoryWindow.Size += Source->PrefetchWindow.Size;
        Destination->MemoryWindow.Alignment = max(Destination->MemoryWindow.Alignment,
                                                  Source->PrefetchWindow.Alignment
                                                  );

        Destination->MemoryWindow.LengthBelow4GB +=
            Source->PrefetchWindow.LengthBelow4GB;

    } else {

        Destination->PrefetchWindow.Size += Source->PrefetchWindow.Size;
        Destination->PrefetchWindow.Alignment =
            max(Destination->PrefetchWindow.Alignment,
                Source->PrefetchWindow.Alignment
                );

        Destination->PrefetchWindow.LengthBelow4GB +=
            Source->PrefetchWindow.LengthBelow4GB;

    }

    Destination->MemoryWindow.Size += Source->MemoryWindow.Size;
    Destination->MemoryWindow.Alignment =
        max(Destination->MemoryWindow.Alignment,
            Source->MemoryWindow.Alignment
            );

    Destination->MemoryWindow.LengthBelow4GB +=
        Source->MemoryWindow.LengthBelow4GB;

    //
    // The ISA bit eats up 3/4 of every I/O window, so if it is set, the I/O
    // requirements quadruple.
    //
    if (Bridge->Attributes.IsaBitSet &&
        (Flags & PCI_EXPAND_WINDOW_FOR_ISA_BIT)) {
        Destination->IoWindow.Size += (Source->IoWindow.Size * 4);
        Destination->IoWindow.LengthBelow4GB +=
            (Source->IoWindow.LengthBelow4GB * 4);

    } else {

        //
        // If the bridge is subtractive, the increase in I/O size should be to
        // the size of a bridge window.  (This will probably never matter,
        // though.)
        //

        if (Bridge->Attributes.Subtractive == 1) {
            Destination->IoWindow.Size =
                max(PCI_TYPE1_IO_WINDOW_ALIGNMENT,
                    Source->IoWindow.Size);
            Destination->IoWindow.LengthBelow4GB =
                max(PCI_TYPE1_IO_WINDOW_ALIGNMENT,
                    Source->IoWindow.LengthBelow4GB);
        } else {
            Destination->IoWindow.Size += Source->IoWindow.Size;
            Destination->IoWindow.LengthBelow4GB += Source->IoWindow.LengthBelow4GB;
        }
    }

    Destination->IoWindow.Alignment =
        max(Destination->IoWindow.Alignment,
            Source->IoWindow.Alignment
            );

    return;
}

BOOLEAN
PciBridgeCheckLocationCompatibility (
    __in PPCI_BRIDGE Bridge,
    __in PPCI_BRIDGE_RESOURCES Resources,
    __in PPCI_BRIDGE_REQUIREMENTS Requirements
    )

/*++

Routine Description:

    This routine will test the location compatibility between the current
    resources for the bridge and the newly computed requirements.

    There are three types of resources that are examined here.
    Port or I/O, Memory and Prefetchable memory.

    The location compatibility ensures that the location of a bridge window
    is not deterimental to the assigment of the new set of resources (if any)
    from within the bridge window for the device.

    For illustrative purposes, consider a bridge with a prefetchable window
    above the 4GB boundary and a PCI device behind this bridge that only
    supports 32-bit BARS and requires a prefetchable resource. This is a case
    of incompatibility and in order to support this device, the bridge window
    or certain portions of it has to be moved to a region below the 4GB boundary.

Arguments:

    Bridge - The bridge whose current resources and newly computed requirements
        is tested for compatibility.

    Resources - current resource assignment for the bridge.

    Requirements - newly computed requirements for the bridge.

Return Value:

    TRUE if compatible, FALSE otherwise.

--*/

{

    UNREFERENCED_PARAMETER(Bridge);

    PAGED_CODE();

    //
    // I/O. This will never ever be incompatible.
    //

    if ((Resources->IoWindow.u.Port.Length != 0) &&
        (!ADDRESS_BELOW_4GB(Resources->IoWindow.u.Port.Start.QuadPart)) &&
        (Requirements->IoWindow.LengthBelow4GB != 0)) {

        PCI_ASSERT(FALSE);

        return FALSE;
    }

    //
    // Memory Window. This will never be incompatible at the time of writing.
    //

    if ((Resources->MemoryWindow.u.Memory.Length != 0) &&
        (!ADDRESS_BELOW_4GB(Resources->MemoryWindow.u.Memory.Start.QuadPart)) &&
        (Requirements->MemoryWindow.LengthBelow4GB != 0)) {

        PCI_ASSERT(FALSE);

        return FALSE;
    }

    //
    // Prefetch Memory Window.
    //

    if ((Resources->PrefetchWindow.u.Memory.Length != 0) &&
        (!ADDRESS_BELOW_4GB(Resources->PrefetchWindow.u.Memory.Start.QuadPart)) &&
        (Requirements->PrefetchWindow.LengthBelow4GB != 0)) {

        return FALSE;
    }

    return TRUE;
}

VOID
PciBridgeUpdateD3Availability (
    __in PPCI_BRIDGE Bridge
    )

/*++

Routine Description:

    This routine updates the availability of D3 for a bridge based on the
    devices on the secondary bus of the bridge.  If this bus will be placed into
    B2 when this bridge is placed into D3, scan the child devices for any that
    cannot tolerate B2 while on a 66MHz bus segment.  If such a device exists
    and the bus is in 66MHz mode, disable D3 for this bridge.  Detect a 66MHz
    bus segment by looking at the 66MHz capability of every device on the bus.
    If they all support 66MHz, assume that the segment is operating in 66MHz
    mode.

Arguments:

    Bridge - Supplies the bridge to check for D3 availability.

Return Value:

    None.

--*/

{

    BOOLEAN childD3Illegal;
    PPCI_DEVICE current;
    PPCI_DEVICE device;
    BOOLEAN found;
    BOOLEAN illegal;
    BOOLEAN operating66MHz;

    if (Bridge->D3Causes66MHzB2 == 0) {
        return;
    }

    PciAcquirePassiveLock(&Bridge->Bus->ChildDeviceLock);
    found = FALSE;
    operating66MHz = TRUE;
    childD3Illegal = FALSE;
    for (current = Bridge->Bus->ChildDevices;
         current != NULL;
         current = current->Sibling) {

        if (current->B2On66MHzSegmentIllegal == 1) {
            found = TRUE;
        }

        if (current->Device66MHzCapable == 0) {
            operating66MHz = FALSE;
        }

        if ((current->HeaderType == PCI_BRIDGE_TYPE) &&
            (current->BridgeFlags.D3Illegal == 1)) {

            childD3Illegal = TRUE;
        }
    }

    PciReleasePassiveLock(&Bridge->Bus->ChildDeviceLock);

    //
    // Update the PDO flag indicating whether D3 is allowed for this bridge
    // with the result of the scan.
    //
    // N.B. This requires reaching into the PDO pointer of the bridge stack
    //      and assuming that it is a PCI PDO.  This breaks the distinction
    //      between the FDO and PDO sections of the driver and is to be
    //      avoided whenever possible.
    //

    device = (PPCI_DEVICE)Bridge->Bus->PhysicalDeviceObject->DeviceExtension;

    PCI_ASSERT(PCI_DEVICE_EXTENSION(device));

    illegal = (device->BridgeFlags.D3Illegal != 0);

    //
    // If the segment is not operating in 66MHz mode, override any found devices
    // that cannot support B2.  This restriction only applies to 66MHz mode.
    // However, if there are any child bridges that have been prevented from
    // entering D3 because their bus segments are 66MHz and contain affected
    // devices, disable D3 on this bridge as well.  This is to prevent the
    // clock being stopped on the primary bus of the child bridge which might
    // result in the clock being stopped on the secondary bus of the child
    // bridge, causing the child bus to effectively enter B2.
    //

    if (operating66MHz == FALSE) {
        found = FALSE;
    }

    if (childD3Illegal != FALSE) {
        found = TRUE;
    }

    if (found != FALSE) {
        device->BridgeFlags.D3Illegal = 1;

    } else {
        device->BridgeFlags.D3Illegal = 0;
    }

    //
    // If this results in a change of the D3 legality for this bridge,
    // recursively call the parent to update itself.  The comment above
    // describes the reason for this.
    //

    if (((found == FALSE) && (illegal != FALSE)) ||
        ((found != FALSE) && (illegal == FALSE))) {

        if (device->Parent->PciBridge != NULL) {
            PciBridgeUpdateD3Availability(device->Parent->PciBridge);
        }
    }

    return;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBridgeReevaluateWindows (
    _In_ PPCI_BRIDGE Bridge
    )

/*++

Routine Description:

    When a machine boots, if the BIOS leaves a bridge's windows closed, this
    driver marks them as "disabled."  If the bus was empty at boot, a BIOS may
    do this even if the windows work fine and might need to be opened when a
    device is hot-plugged.  This routine marks a bridge's windows as a candidate
    for opening, in response to a hot-plug event, even if that event is really
    just plugging in a device while the whole machine has been hibernated.

    Access to the fields involved is implicitly synchronized by the PnP manager.

Parameters:

    Bridge - The bridge in question.

Return Value:

    VOID

--*/
{
    BOOLEAN reqsChanged;

    PAGED_CODE();

    reqsChanged = FALSE;

    //
    // For I/O windows, go with preferred and fallback but not minimum, so the
    // bridge still works even if there is no I/O space available.  If there is
    // no more I/O space left in the machine, this bridge will start with its
    // minimum requirement, which may or may not allow the device below to
    // work.  This can't be helped.
    //

    if (Bridge->PreferredRequirements.IoWindow.Disabled != FALSE) {
        Bridge->PreferredRequirements.IoWindow.Disabled = FALSE;
        Bridge->PreferredRequirements.IoWindow.Size = PCI_TYPE1_IO_WINDOW_ALIGNMENT;
        Bridge->PreferredRequirements.IoWindow.Alignment = PCI_TYPE1_IO_WINDOW_ALIGNMENT;
        reqsChanged = TRUE;
    }

    if (Bridge->FallbackRequirements.IoWindow.Disabled != FALSE) {
        Bridge->FallbackRequirements.IoWindow.Disabled = FALSE;
        Bridge->FallbackRequirements.IoWindow.Size = PCI_TYPE1_IO_WINDOW_ALIGNMENT;
        Bridge->FallbackRequirements.IoWindow.Alignment = PCI_TYPE1_IO_WINDOW_ALIGNMENT;
        reqsChanged = TRUE;
    }

    //
    // Memory is always required, in preferred, fallback and minimum.  Set the
    // size to the default 1MB.  If the device(s) below require more than 1MB,
    // this will trigger yet another rebalance, and the bridge windows will be
    // opened further, if MMIO space is plentiful enough to satisfy the request.
    //

    if (Bridge->PreferredRequirements.MemoryWindow.Disabled != FALSE) {
        Bridge->PreferredRequirements.MemoryWindow.Disabled = FALSE;
        Bridge->PreferredRequirements.MemoryWindow.Size = PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
        Bridge->PreferredRequirements.MemoryWindow.Alignment = PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
        reqsChanged = TRUE;
    }

    if (Bridge->FallbackRequirements.MemoryWindow.Disabled != FALSE) {
        Bridge->FallbackRequirements.MemoryWindow.Disabled = FALSE;
        Bridge->FallbackRequirements.MemoryWindow.Size = PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
        Bridge->FallbackRequirements.MemoryWindow.Alignment = PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
        reqsChanged = TRUE;
    }

    //
    // Like I/O, prefetchable memory can be done without, so do only preferred
    // and fallback.
    //

    if (Bridge->PreferredRequirements.PrefetchWindow.Disabled != FALSE) {
        Bridge->PreferredRequirements.PrefetchWindow.Disabled = FALSE;
        Bridge->PreferredRequirements.PrefetchWindow.Size = PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
        Bridge->PreferredRequirements.PrefetchWindow.Alignment = PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
        reqsChanged = TRUE;
    }

    if (Bridge->FallbackRequirements.PrefetchWindow.Disabled != FALSE) {
        Bridge->FallbackRequirements.PrefetchWindow.Disabled = FALSE;
        Bridge->FallbackRequirements.PrefetchWindow.Size = PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
        Bridge->FallbackRequirements.PrefetchWindow.Alignment = PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
        reqsChanged = TRUE;
    }

    //
    // Now kick the I/O manager and tell it that the resource requirements for
    // this bus have changed.
    //

    if (reqsChanged != FALSE) {

        PCI_BUS_SET_FLAG(Bridge->Bus, RequirementsChanged);
        IoInvalidateDeviceState(Bridge->Bus->PhysicalDeviceObject);
    }
}


// --------------------------------------------------------------- Internal Utilities

VOID
PciBridgeFilterRequirement(
    _In_ PPCI_BRIDGE Bridge,
    _Inout_ PIO_RESOURCE_DESCRIPTOR Fixed,
    _Inout_ PIO_RESOURCE_DESCRIPTOR Flexible,
    _Inout_ PIO_RESOURCE_DESCRIPTOR Alternate
    )
/*++

Routine Description:

    The PDO's requirements list will have a preferred config
    of the current setting for every bridge window (if there is one),
    and an alternate config of the minimum possible setting for
    every bridge window.  If a window has been explicitly disabled,
    the alternate config has a length of 0 (and there is no preferred
    config).

    This function updates the generic configs to be large enough to
    fulfill the requirements of any device on the secondary bus, comprehending
    the preferred config as well.

Parameters:

    Bridge - The bridge these requirements are for.

    Fixed - The fixed requirement from the input list (if it exists).  It will
        contain the filtered fixed requirement on exit.

    Flexible - The generic requirement.  This will be populated with a flexible
        requirement of the preferred size.

    Alternate - The alternate (fallback) requirement.  This won't be present
        in the input list, but if we have a fallback size/alignment from
        the flexible config, it should go here.

Return Value:

    VOID

--*/
{
    PPCI_BRIDGE_WINDOW_REQUIREMENT preferred, fallback, origPreferred;
    ULONGLONG decodedLength;
    BOOLEAN prefetchWindow;
    NTSTATUS status;

    PAGED_CODE();

    if (!PCI_IO_WINDOW_REQUIREMENT(Flexible) &&
        !PCI_MEMORY_WINDOW_REQUIREMENT(Flexible) &&
        !PCI_PREFETCH_WINDOW_REQUIREMENT(Flexible)) {

        //
        // Not a window descriptor.  Shouldn't be here, and don't filter.
        //
        PCI_ASSERT(FALSE);
        return;
    }

    //
    // Usually, the alternate requirement will be flexible, and we
    // want to filter it to support the window size/alignment we want.
    // But sometimes the PDO will generate only fixed requirements for
    // a window - for instance when the device is on the debugging path.
    // In this case, we want to respect the fixed nature of the generic
    // config and not alter it.
    //

    if (PciIsFixedRequirement(Flexible) != FALSE) {

        PCI_ASSERT(Fixed->Type == CmResourceTypeNull);
        return;
    }

    TRACE(RESOURCE_TRACE_LEVEL, "PciBridgeFilterRequirement for bridge 0x%p\n", Bridge);
    TRACE(RESOURCE_TRACE_LEVEL, "Fixed:\n");
    DebugDumpIoResource(RESOURCE_TRACE_LEVEL, Fixed);
    TRACE(RESOURCE_TRACE_LEVEL, "Flexible:\n");
    DebugDumpIoResource(RESOURCE_TRACE_LEVEL, Flexible);

    //
    // Make a local copy of preferred and fallback requirements so that it can
    // be modified at will.
    //

    preferred = NULL;
    fallback = NULL;
    preferred = PciAllocatePool(PagedPool,
                                sizeof(PCI_BRIDGE_WINDOW_REQUIREMENT));

    if (preferred == NULL) {
        return;
    }

    fallback = PciAllocatePool(PagedPool,
                               sizeof(PCI_BRIDGE_WINDOW_REQUIREMENT));

    if (fallback == NULL) {
        PciFreePool(preferred);
        return;
    }

    prefetchWindow = FALSE;
    if (PCI_IO_WINDOW_REQUIREMENT(Flexible)) {
        origPreferred = &Bridge->PreferredRequirements.IoWindow;
        RtlCopyMemory(preferred,
                      &Bridge->PreferredRequirements.IoWindow,
                      sizeof(PCI_BRIDGE_WINDOW_REQUIREMENT));

        RtlCopyMemory(fallback,
                      &Bridge->FallbackRequirements.IoWindow,
                      sizeof(PCI_BRIDGE_WINDOW_REQUIREMENT));

    } else if (PCI_MEMORY_WINDOW_REQUIREMENT(Flexible)) {
        origPreferred = &Bridge->PreferredRequirements.MemoryWindow;
        RtlCopyMemory(preferred,
                      &Bridge->PreferredRequirements.MemoryWindow,
                      sizeof(PCI_BRIDGE_WINDOW_REQUIREMENT));

        RtlCopyMemory(fallback,
                      &Bridge->FallbackRequirements.MemoryWindow,
                      sizeof(PCI_BRIDGE_WINDOW_REQUIREMENT));

    } else {

        //
        // Prefetch window
        //

        prefetchWindow = TRUE;
        origPreferred = &Bridge->PreferredRequirements.PrefetchWindow;
        RtlCopyMemory(preferred,
                      &Bridge->PreferredRequirements.PrefetchWindow,
                      sizeof(PCI_BRIDGE_WINDOW_REQUIREMENT));

        RtlCopyMemory(fallback,
                      &Bridge->FallbackRequirements.PrefetchWindow,
                      sizeof(PCI_BRIDGE_WINDOW_REQUIREMENT));

    }

    if (preferred->Disabled == TRUE) {

        //
        // There is no requirement.  Strip it out.
        //
        NullResourceRequirement(Fixed);
        NullResourceRequirement(Flexible);
        NullResourceRequirement(Alternate);
        goto exit;
    }

    //
    // Ensure that the preferred requirement does not exceed the max address
    // as programmed by the PDO. This applies to both memory and IO.
    //

    if (preferred->Size >
        (ULONGLONG)Flexible->u.Generic.MaximumAddress.QuadPart) {

        preferred->Size =
            (ULONGLONG)Flexible->u.Generic.MaximumAddress.QuadPart;

        preferred->Alignment = PCI_ALIGN_AT_GIGABYTE_BOUNDARY;
    }

    if (preferred->LengthBelow4GB > MAXULONG) {
        preferred->LengthBelow4GB = MAXULONG;
    }

    //
    // The preferred window size gets programmed into the Generic requirement.
    // If the fallback window size is different from this, this gets programmed
    // into the Alternate requirement
    //

    //
    // If considering the prefetch window, there may be lengths
    // greater than MAXULONG, in which case the size and alignment
    // will need to be encoded into the descriptor.
    //

    //
    // Check if considering the prefetch window
    //

    if (prefetchWindow != FALSE) {

        status = RtlIoEncodeMemIoResource(
                     Flexible,
                     Flexible->Type,
                     preferred->Size,
                     preferred->Alignment,
                     Flexible->u.Generic.MinimumAddress.QuadPart,
                     Flexible->u.Generic.MaximumAddress.QuadPart);

        //
        // If the encoding has failed, the following attempts will be made
        // to make the window fit into the resource descriptor.
        //
        // 1. The size of the window will be shifted over until it can be
        // encoded into the resource descriptor.  This will fail if the
        // size is above the maximum value that can be encoded.
        //
        // 2. It should be very rare to get a window size so large, but if
        // so, the size will default to the largest encodable value.  This
        // may result in some devices behind the bridge not starting.
        //
        // 3. The alignment is not recoverable when encoded with the length.
        // This would only occur if the alignment was greater than the length
        // and since the alignment for a bridge should be the maximum of the
        // alignments of its children, the bridge length should always be
        // greater than its alignment.  In this case, fail since something
        // has gone wrong.
        //

        if (!NT_SUCCESS(status)) {
            ULONGLONG newSize;

            newSize = preferred->Size;

            //
            // Case 1 - Failed to encode flexible requirement. Attempt to
            // round up to closest acceptable length.
            //

            status = RtlFindClosestEncodableLength(preferred->Size, &newSize);

            if (!NT_SUCCESS(status)) {

                //
                // Case 2 - Length is above maximum encodable length, so
                // default to the largest acceptable length.
                //

                newSize = CM_RESOURCE_MEMORY_LARGE_64_MAXLEN;
            }

            //
            // Encode length into flexible requirement
            //

            status = RtlIoEncodeMemIoResource(
                         Flexible,
                         Flexible->Type,
                         newSize,
                         preferred->Alignment,
                         Flexible->u.Generic.MinimumAddress.QuadPart,
                         Flexible->u.Generic.MaximumAddress.QuadPart);

            if (!NT_SUCCESS(status)) {

                //
                // Case 3 - The alignment is not recoverable with the required
                // shifting of the length.  This should not be possible.
                //

                PCI_ASSERT(FALSE);
            }

            //
            // The size and alignment may have changed.  Copy them back to the
            // preferred requirement to be consistent.
            //

            preferred->Size = RtlIoDecodeMemIoResource(Flexible,
                                                       &preferred->Alignment,
                                                       NULL,
                                                       NULL);
        }

    } else {

        PCI_ASSERT((preferred->Size <= MAXULONG) &&
                (preferred->Alignment <= MAXULONG));

        Flexible->u.Generic.Length = (ULONG)preferred->Size;
        Flexible->u.Generic.Alignment = (ULONG)preferred->Alignment;
    }

    if ((origPreferred->Size == fallback->Size) &&
        (origPreferred->Alignment == fallback->Alignment)) {

        NullResourceRequirement(Alternate);

    } else {

        RtlCopyMemory(Alternate, Flexible, sizeof(IO_RESOURCE_DESCRIPTOR));

        //
        // Ensure that the fallback requirement does not exceed the max address
        // as programmed by the PDO. This applies to both memory and IO.
        //

        if (fallback->Size >
            (ULONGLONG)Alternate->u.Generic.MaximumAddress.QuadPart) {

            fallback->Size =
                (ULONGLONG)Alternate->u.Generic.MaximumAddress.QuadPart;

            fallback->Alignment = PCI_ALIGN_AT_GIGABYTE_BOUNDARY;
        }

        if (fallback->LengthBelow4GB > MAXULONG) {
            fallback->LengthBelow4GB = MAXULONG;
        }

        //
        // If considering the prefetch window, there may be lengths
        // greater than MAXULONG, in which case the length and alignment
        // may need to be encoded into the descriptor.
        //

        //
        // Check if considering the prefetch window.
        //

        if (prefetchWindow != FALSE) {

            status = RtlIoEncodeMemIoResource(
                         Alternate,
                         Alternate->Type,
                         fallback->Size,
                         fallback->Alignment,
                         Alternate->u.Generic.MinimumAddress.QuadPart,
                         Alternate->u.Generic.MaximumAddress.QuadPart);

            //
            // If the encoding has failed, the following attempts will be made
            // to make the window fit into the resource descriptor.
            //
            // 1. The size of the window will be shifted over until it can be
            // encoded into the resource descriptor.  This will fail if the
            // size is above the maximum value that can be encoded.
            //
            // 2. It should be very rare to get a window size so large, but if
            // so, the size will default to the largest encodable value.  This
            // may result in some devices behind the bridge not starting.
            //
            // 3. The alignment is not recoverable when encoded with the length.
            // This would only occur if the alignment was greater than the length
            // and since the alignment for a bridge should be the maximum of the
            // alignments of its children, the bridge length should always be
            // greater than its alignment.  In this case, fail since something
            // has gone wrong.
            //

            if (!NT_SUCCESS(status)) {
                ULONGLONG newSize;

                newSize = fallback->Size;

                //
                // Case 1 - Failed to encode alternate requirement. Attempt to
                // round up to closest acceptable length.
                //

                status = RtlFindClosestEncodableLength(fallback->Size, &newSize);

                if (!NT_SUCCESS(status)) {

                    //
                    // Case 2 - Length is above maximum encodable length, so
                    // default to the largest acceptable length.
                    //

                    newSize = CM_RESOURCE_MEMORY_LARGE_64_MAXLEN;
                }

                //
                // Encode expanded length into alternate requirement
                //
                status = RtlIoEncodeMemIoResource(
                             Alternate,
                             Alternate->Type,
                             newSize,
                             fallback->Alignment,
                             Alternate->u.Generic.MinimumAddress.QuadPart,
                             Alternate->u.Generic.MaximumAddress.QuadPart);

                if (!NT_SUCCESS(status)) {

                    //
                    // Case 3 - The alignment is not recoverable with the required
                    // shifting of the length.  This should not be possible.
                    //

                    PCI_ASSERT(FALSE);
                }

                //
                // The size and alignment may have changed.  Copy them back to the
                // preffered requirement to be consistent.
                //

                fallback->Size = RtlIoDecodeMemIoResource(Alternate,
                                                          &fallback->Alignment,
                                                          NULL,
                                                          NULL);
            }

        } else {

            PCI_ASSERT((fallback->Size <= MAXULONG) &&
                (fallback->Alignment <= MAXULONG));

            Alternate->u.Generic.Length = (ULONG)fallback->Size;
            Alternate->u.Generic.Alignment = (ULONG)fallback->Alignment;
        }
    }

    //
    // There is a requirement, so test to see if the fixed config
    // should remain.  If the fixed config is too small, or not
    // aligned correctly, strip it out.
    //
    if (Fixed->Type != CmResourceTypeNull) {

        if ((Fixed->u.Generic.Length != Flexible->u.Generic.Length) ||
            ((Fixed->u.Generic.Length % Flexible->u.Generic.Alignment) != 0)) {

            NullResourceRequirement(Fixed);
        }
    }

    //
    // Tweak the Fixed, Flexible and Alternate descriptors for location
    // compatibility with the new requirements.
    // If a bridge has a device that requires a window below the 4GB boundary,
    // then limit the bridge requirement descriptor from 0 to 0xFFFFFFFF. Allow
    // additional bytes above the 4GB boundary only if certain devices behind
    // this bridge can be assigned those ranges.
    //

    if (preferred->LengthBelow4GB != 0) {

        PCI_ASSERT(Flexible->Type != CmResourceTypeNull);

        if (!ADDRESS_BELOW_4GB(Flexible->u.Generic.MaximumAddress.QuadPart)) {
            Flexible->u.Generic.MinimumAddress.QuadPart = 0;
            Flexible->u.Generic.MaximumAddress.QuadPart = 0xFFFFFFFF;
            decodedLength = RtlIoDecodeMemIoResource(Flexible,
                                                     NULL,
                                                     NULL,
                                                     NULL);
            if (preferred->LengthBelow4GB < decodedLength) {
                Flexible->u.Generic.MaximumAddress.QuadPart +=
                    decodedLength - preferred->LengthBelow4GB;
            }

            NullResourceRequirement(Fixed);
        }
    }

    if (fallback->LengthBelow4GB != 0) {
        if (Alternate->Type == CmResourceTypeNull) {
            goto exit;
        }

        if (!ADDRESS_BELOW_4GB(Alternate->u.Generic.MaximumAddress.QuadPart)) {
            Alternate->u.Generic.MinimumAddress.QuadPart = 0;
            Alternate->u.Generic.MaximumAddress.QuadPart = 0xFFFFFFFF;
            decodedLength = RtlIoDecodeMemIoResource(Alternate,
                                                     NULL,
                                                     NULL,
                                                     NULL);
            if (fallback->LengthBelow4GB < decodedLength) {
                Alternate->u.Generic.MaximumAddress.QuadPart +=
                    decodedLength - fallback->LengthBelow4GB;
            }

        }

    }

exit:

    if (preferred != NULL) {
        PciFreePool(preferred);
    }

    if (fallback != NULL) {
        PciFreePool(fallback);
    }

    TRACE(RESOURCE_TRACE_LEVEL, "PciBridgeFilterRequirement: After filtering %p\n", Bridge);
    TRACE(RESOURCE_TRACE_LEVEL, "Fixed:\n");
    DebugDumpIoResource(RESOURCE_TRACE_LEVEL, Fixed);
    TRACE(RESOURCE_TRACE_LEVEL, "Flexible:\n");
    DebugDumpIoResource(RESOURCE_TRACE_LEVEL, Flexible);
    TRACE(RESOURCE_TRACE_LEVEL, "Alternate:\n");
    DebugDumpIoResource(RESOURCE_TRACE_LEVEL, Alternate);
}

ULONGLONG
PciBridgeMaximumWindowAlignment(
    __in ULONGLONG Length
    )
/*++

Routine Description:

    This function calculates the maximum alignment a device can have if it is
    behind a bridge with a window of Length.  This turns out to be finding
    the top bit set in the length.

Parameters:

    Length - the size of the window

Return Value:

    The alignment

--*/
{
    ULONGLONG alignment = 0x8000000000000000;

    PAGED_CODE();

    if (Length == 0) {
        return 1;
    }

    while (!(Length & alignment)) {
        alignment >>= 1;
    }

    return alignment;
}

NTSTATUS
PciBridgeGetRequirementsForDevice(
    __in PPCI_BRIDGE Bridge,
    __in PPCI_DEVICE Device,
    __out PPCI_BRIDGE_REQUIREMENTS Requirements,
    __out PPCI_BRIDGE_REQUIREMENTS Subtractive
    )
/*++

Routine Description:

    This routine computes the impact that the given device will have
    on the requirements of the bridge windows above it.  This is computed
    by getting the resource requirements from PnP and computing the total
    size and alignment of all the requirements.  We do this using PnP's
    requirements list instead of our internal representation because
    our internal representation does not comprehend filtering, either in or out.

Arguments:

    Bridge - The bridge whose requirements will be affected by the requiremetns
        of this device.

    Device - The device to compute the requirements for.

    Requirements - A bridge requirements structure which should be filled in
        with the computed impact on bridge windows that the device has.

    Subtractive - A bridge requirements structure which should be filled in
        with the requirements that this device has that will be serviced
        outside the bridge window.

Return Value:

    STATUS_SUCCESS if the routine succeeded.
    STATUS_INSUFFICIENT_RESOURCES if there was a memory allocation failure
    STATUS_DATA_ERROR if the requirements list from PnP has a bogus format
    A failure status code returned from IoGetDeviceProperty if that call failed.

--*/
{
    PIO_RESOURCE_DESCRIPTOR ioDescriptor, firstAlternative;
    PCI_BRIDGE_REQUIREMENTS listRequirements, listSubtractive;
    PPCI_BRIDGE_REQUIREMENTS listWindows, bridgeWindows;
    PHYSICAL_ADDRESS maxAddress;
    PCI_BRIDGE_WINDOW_REQUIREMENT window;
    PIO_RESOURCE_REQUIREMENTS_LIST reqList;
    PIO_RESOURCE_LIST list;
    BOOLEAN alternativeCount;
    ULONG currentType;
    NTSTATUS status;
    ULONG i, j, pass;
    ULONGLONG decodedLength;
    ULONGLONG decodedAlignment;
    ULONGLONG decodedMaxAddress;

    PAGED_CODE();

    PCI_INITIALIZE_BRIDGE_REQUIREMENTS(Requirements);
    PCI_INITIALIZE_BRIDGE_REQUIREMENTS(Subtractive);

    status = PciGetDeviceProperty(Device->DeviceObject,
                                  DevicePropertyResourceRequirements,
                                  &reqList
                                  );
    if (!NT_SUCCESS(status) || (reqList == NULL)) {
        return status;
    }

    //
    // The requirements could have multiple alternative lists.  Compute the window
    // requirements separately for each, and take the maximum.
    //
    list = &reqList->List[0];
    for (i = 0; i < reqList->AlternativeLists; i++) {

        PCI_INITIALIZE_BRIDGE_REQUIREMENTS(&listRequirements);
        PCI_INITIALIZE_BRIDGE_REQUIREMENTS(&listSubtractive);

        for (j = 0; j < list->Count; j++) {

            ioDescriptor = &list->Descriptors[j];

            //
            // Skip uninteresting descriptors.
            //

            if ((ioDescriptor->Type != CmResourceTypePort) &&
                (ioDescriptor->Type != CmResourceTypeMemory) &&
                (ioDescriptor->Type != CmResourceTypeMemoryLarge)) {

                continue;
            }

            PCI_ASSERT(ioDescriptor->Option != IO_RESOURCE_ALTERNATIVE);

            if (ioDescriptor->Option == IO_RESOURCE_ALTERNATIVE) {
                status = STATUS_DATA_ERROR;
                goto exit;
            }

            //
            // PCI-PCI bridge windows are handled separately.
            // See PciBridgeComputeRequirements.
            //
            if (PCI_WINDOW_REQUIREMENT(ioDescriptor)) {

                while ((j < (list->Count - 1)) &&
                       (list->Descriptors[j+1].Option == IO_RESOURCE_ALTERNATIVE)) {
                    j++;
                }
                continue;
            }

            //
            // Each requirement consists of at least one preferred requirement
            // and then may many alternates.  The total impact of all of these
            // is the maximum impact of a single one, since only one alternative
            // will ever be allocated.
            //

            alternativeCount = 1;
            firstAlternative = ioDescriptor;
            currentType = ioDescriptor->Type;
            window.Size = RtlIoDecodeMemIoResource(ioDescriptor,
                                                   &window.Alignment,
                                                   NULL,
                                                   &maxAddress.QuadPart);

            while ((j < (list->Count - 1)) &&
                   (list->Descriptors[j+1].Option == IO_RESOURCE_ALTERNATIVE)) {

                j++;
                ioDescriptor = &list->Descriptors[j];
                alternativeCount++;

                PCI_ASSERT(currentType == ioDescriptor->Type);
                if (currentType != ioDescriptor->Type) {
                    status = STATUS_DATA_ERROR;
                    goto exit;
                }

                decodedLength = RtlIoDecodeMemIoResource(ioDescriptor,
                                                         &decodedAlignment,
                                                         NULL,
                                                         &decodedMaxAddress);

                window.Size = max(window.Size, decodedLength);
                window.Alignment = max(window.Alignment, decodedAlignment);

                maxAddress.QuadPart = max((ULONGLONG)maxAddress.QuadPart,
                                          decodedMaxAddress);
            }

            if (PciArbitrateRequirementInsideBridgeWindow(Bridge->Bus,
                                                          Device->DeviceObject,
                                                          firstAlternative,
                                                          alternativeCount)) {
                listWindows = &listRequirements;

            } else {
                listWindows = &listSubtractive;
            }

            //
            // Now that we have computed the impact of the various alternatives
            // of the requirement, add it to the list impact based on type.
            //
            switch (currentType) {
            case CmResourceTypePort:

                listWindows->IoWindow.Size += window.Size;
                listWindows->IoWindow.Alignment = max(listWindows->IoWindow.Alignment,
                                                      window.Alignment
                                                      );

                //
                // Port ranges are always below 4GB.
                // This is just a customary check.
                //

                if (ADDRESS_BELOW_4GB(maxAddress.QuadPart)) {

                    listWindows->IoWindow.LengthBelow4GB += window.Size;
                }

                break;

            case CmResourceTypeMemory:
            case CmResourceTypeMemoryLarge:

                if (ioDescriptor->Flags & CM_RESOURCE_MEMORY_PREFETCHABLE) {
                    listWindows->PrefetchWindow.Size += window.Size;
                    listWindows->PrefetchWindow.Alignment =
                        max(listWindows->PrefetchWindow.Alignment,
                            window.Alignment
                            );

                    if (ADDRESS_BELOW_4GB(maxAddress.QuadPart)) {

                        //
                        // Sanity Check.  If the window size is above 4GB,
                        // then there is an error in the maxAddress being
                        // below 4GB.
                        //

                        PCI_ASSERT(window.Size < MAXULONG);

                        listWindows->PrefetchWindow.LengthBelow4GB +=
                            window.Size;

                    }

                } else {
                    listWindows->MemoryWindow.Size += window.Size;
                    listWindows->MemoryWindow.Alignment =
                        max(listWindows->MemoryWindow.Alignment,
                            window.Alignment
                            );

                    if (ADDRESS_BELOW_4GB(maxAddress.QuadPart)) {

                        //
                        // Sanity Check.  If the window size is above 4GB,
                        // then there is an error in the maxAddress being
                        // below 4GB.
                        //

                        PCI_ASSERT(window.Size < MAXULONG);
                        listWindows->MemoryWindow.LengthBelow4GB +=
                            window.Size;

                    }
                }
                break;
            default:
                break;
            }
        }

        //
        // Finally, the total impact is the maximum of each of the list impacts.
        //
        for (pass = 0; pass < 2; pass++) {

            if (pass == 0) {
                bridgeWindows = Requirements;
                listWindows = &listRequirements;
            } else {
                bridgeWindows = Subtractive;
                listWindows = &listSubtractive;
            }

            bridgeWindows->IoWindow.Size = max(bridgeWindows->IoWindow.Size,
                                               listWindows->IoWindow.Size
                                               );

            bridgeWindows->MemoryWindow.Size = max(bridgeWindows->MemoryWindow.Size,
                                                   listWindows->MemoryWindow.Size
                                                   );


            bridgeWindows->PrefetchWindow.Size = max(bridgeWindows->PrefetchWindow.Size,
                                                     listWindows->PrefetchWindow.Size
                                                     );

            bridgeWindows->IoWindow.Alignment
                    = max(listWindows->IoWindow.Alignment,
                          bridgeWindows->IoWindow.Alignment
                          );

            bridgeWindows->MemoryWindow.Alignment
                    = max(listWindows->MemoryWindow.Alignment,
                          bridgeWindows->MemoryWindow.Alignment
                          );

            bridgeWindows->PrefetchWindow.Alignment
                    = max(listWindows->PrefetchWindow.Alignment,
                          bridgeWindows->PrefetchWindow.Alignment
                          );

            bridgeWindows->IoWindow.LengthBelow4GB
                = max(listWindows->IoWindow.LengthBelow4GB,
                      bridgeWindows->IoWindow.LengthBelow4GB
                      );

            bridgeWindows->MemoryWindow.LengthBelow4GB
                = max(listWindows->MemoryWindow.LengthBelow4GB,
                      bridgeWindows->MemoryWindow.LengthBelow4GB
                      );

            bridgeWindows->PrefetchWindow.LengthBelow4GB
                = max(listWindows->PrefetchWindow.LengthBelow4GB,
                      bridgeWindows->PrefetchWindow.LengthBelow4GB
                      );

        }

        list = (PIO_RESOURCE_LIST)((PUCHAR)list
                                   + sizeof(IO_RESOURCE_LIST)
                                   + ((list->Count - 1) * sizeof(IO_RESOURCE_DESCRIPTOR)));
    }

    status = STATUS_SUCCESS;

exit:

    PciFreePool(reqList);
    return status;
}

// ---------------------------------------------------------------- Bridge Interface

_Function_class_(BRIDGE_ARI_FORWARDING_ENABLE)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBridgeInterface_AriForwardingEnable (
    _In_ PVOID Context,
    _In_ BOOLEAN AriForwardingEnable
    )
/*++

Routine Description:

    This routine exposes the ARI Forwarding Enable bit through the bridge
    interface so that the FDO-layer code can safely manipulate it without
    coliding with other stuff in the DeviceControl2 register.

Arguments:

    Context - This is the "Context" pointer exposed in the bridge interface.
        It stores the PDO's device extension.

    AriForwardingEnable - Indicates whether ARI forwarding should be turned on
        or off.

Return Value:

    none

--*/
{
    PPCI_DEVICE Device;

    Device = (PPCI_DEVICE)Context;

    PCI_ASSERT(Device->ExpressPort != NULL);
    PCI_ASSERT(Device->ExpressPort->ExpressCapabilities.CapabilityVersion >= 2);

    //
    // Merge the bridge-controlled fields of DeviceControl2 with the PDO-
    // controlled fields.
    //

    Device->ExpressPort->DeviceControl2.AriForwardingEnable =
        (AriForwardingEnable != FALSE) ? 1 : 0;

    PciWriteExpressRegister(Device->ExpressPort,
                            DeviceControl2,
                            &Device->ExpressPort->DeviceControl2);
}

_Function_class_(BRIDGE_OBFF_CONTROL)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBridgeInterface_ObffControl (
    _In_ PVOID Context,
    _In_ EXPRESS_PORT_OBFF_CONTROL ObffControl
    )
/*++

Routine Description:

    This routine exposes the OBFF control bits through the bridge
    interface so that the FDO-layer code can safely manipulate them without
    coliding with other stuff in the DeviceControl2 register.

Arguments:

    Context - This is the "Context" pointer exposed in the bridge interface.
        It stores the PDO's device extension.

    ObffControl - Indicates whether OBFF should be enabled and, if so, how.

Return Value:

    none

--*/
{
    PPCI_DEVICE Device;

    Device = (PPCI_DEVICE)Context;

    PCI_ASSERT(Device->ExpressPort != NULL);
    PCI_ASSERT(Device->ExpressPort->ExpressCapabilities.CapabilityVersion >= 2);
    PCI_ASSERT(ObffControl <= ObffEnabledUsingWake);

    //
    // Merge the bridge-controlled fields of DeviceControl2 with the PDO-
    // controlled fields.
    //

    Device->ExpressPort->DeviceControl2.OBFFEnable = ObffControl;

    PciWriteExpressRegister(Device->ExpressPort,
                            DeviceControl2,
                            &Device->ExpressPort->DeviceControl2);
}

_Function_class_(BRIDGE_GET_POWER_PARAMETERS)
_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBridgeInterface_GetPowerParameters (
    _In_ PVOID Context,
    _Out_ PPCI_EXPRESS_LTR_MAX_LATENCY_REGISTER UpstreamLatency,
    _Out_ PEXPRESS_PORT_OBFF_CONTROL ObffConfiguration
    )
/*++

Routine Description:

    This routine exposes configuration values to the FDO layer of a bridge
    related to PCIe power management.  These parameters can't just be a part
    of the bridge interface structure because they can't be known until after
    ACPI has had a chance to really get set up on the stack (because they're
    ultimately derived from, among other things, _DSM methods.)

Arguments:

    Context - This is the "Context" pointer exposed in the bridge interface.
        It stores the PDO's device extension.

    UpstreamLatency - Pointer to be filled in with the latency for an upstream
        port plugged in below a downstream port.

    ObffConfiguration - Pointer to be filled in with the OBFF method which
        should be used below this express bridge.

Return Value:

    none

--*/
{
    PPCI_DEVICE Device;

    PAGED_CODE();

    Device = (PPCI_DEVICE)Context;

    PCI_ASSERT(Device->DeviceState != PciNotStarted);
    PCI_ASSERT(Device->ExpressPort != NULL);
    PCI_ASSERT(Device->ExpressPort->ExpressCapabilities.CapabilityVersion >= 2);

    ExpressPortGetLtrValueForExpressBridge(Device->ExpressPort, UpstreamLatency);
    ExpressPortGetObffValueForExpressBridge(Device->ExpressPort, ObffConfiguration);
}

_Function_class_(BRIDGE_DELAY)
_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
PciBridgeInterface_Delay (
    _In_ PVOID Context,
    _In_ PCI_DELAY_INFO Delay
    )

/*++

Routine Description:

    This routine implements a passive delay for a bridge.

Arguments:

    Context - This is the "Context" pointer exposed in the bridge interface.
        It stores the PDO's device extension.

    Delay - An enum which identifies the type of delay requested.

Return Value:

    none

--*/
{
    PPCI_DEVICE Device;

    PAGED_CODE();

    Device = (PPCI_DEVICE)Context;

    //
    // Call the common delay routine
    //

    PciDevice_Delay(Device, Delay);
}

_Function_class_(BRIDGE_STALL)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBridgeInterface_Stall (
    _In_ PVOID Context,
    _In_ ULONG MicroSeconds
    )

/*++

Routine Description:

    This routine implements a stall, which can be called at DISPATCH.

Arguments:

    Context - This is the "Context" pointer exposed in the bridge interface.
        It stores the PDO's device extension.

    MicroSeconds - The stall duration.

Return Value:

    none

--*/
{
    PPCI_DEVICE Device;

    Device = (PPCI_DEVICE)Context;

    //
    // Call the common stall routine
    //

    PciDevice_Stall(Device, MicroSeconds);
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciBridgeInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    This routine returns a PCI_BRIDGE_INTERFACE.

Arguments:

    DeviceExtension - Pointer to the device extension of the bridge in question.

    InterfaceSpecificData - Unused.

    Version - Interface version.

    Size - Size of the PCI_BRIDGE_INTERFACE interface object.

    InterfaceReturn - The interface object pointer to be filled in.

    Filtered - Not supported for this interface.

Return Value:

    NT Status code.

--*/
{
    PPCI_BRIDGE_INTERFACE bridgeInterface;
    PPCI_DEVICE device;
    PCI_EXPRESS_DEVICE_CAPABILITIES_2_REGISTER deviceCap2;
    PCI_SAVED_CONFIGURATION savedConfig;
    PCI_EXPRESS_SLOT_CAPABILITIES_REGISTER slotCap;
    NTSTATUS status;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < FIELD_OFFSET(PCI_BRIDGE_INTERFACE, GetPowerParameters)) {
        return STATUS_UNSUCCESSFUL;
    }

    if (Size > sizeof(PCI_BRIDGE_INTERFACE)) {
        Size = sizeof(PCI_BRIDGE_INTERFACE);
    }

    device = (PPCI_DEVICE)DeviceExtension;

    if ((device->HeaderType != PCI_BRIDGE_TYPE) &&
        (device->HeaderType != PCI_CARDBUS_BRIDGE_TYPE)) {

        return STATUS_INVALID_PARAMETER;
    }

    bridgeInterface = (PPCI_BRIDGE_INTERFACE)InterfaceReturn;
    RtlZeroMemory(bridgeInterface, Size);

    bridgeInterface->Size = sizeof(PciBridgeInterface);
    bridgeInterface->Version = 2;
    bridgeInterface->Context = DeviceExtension;
    bridgeInterface->InterfaceReference = PciRefDereferenceNoop;
    bridgeInterface->InterfaceDereference = PciRefDereferenceNoop;

    //
    // Report the hardware interface capabilities.
    //
    bridgeInterface->PciXCapability = device->PciXCapability;
    bridgeInterface->ExpressCapability = device->ExpressCapability;
    bridgeInterface->BusNumbersConfigured = (device->BusNumbersConfigured? 1 : 0);
    bridgeInterface->HyperTransport = (device->HyperTransport? 1 : 0);
    bridgeInterface->MsiHtConverter = (device->MsiHtConverter? 1 : 0);
    bridgeInterface->ExtendedConfigAvailable = (device->ExtendedConfigAvailable? 1:  0);
    if ((device->HackFlags & PCI_HACK_AVOID_D1D2_FOR_SLD) != 0) {
        bridgeInterface->AvoidChildD1D2ForSld = 1;
    }

    if ((device->HackFlags & PCI_HACK_DISABLE_CHILD_PM) != 0) {
        bridgeInterface->DisableChildPowerManagement = 1;
    }

    bridgeInterface->D3Causes66MHzB2 = device->BridgeFlags.D3Causes66MHzB2;
    if (device->ExpressPort != NULL) {
        if ((device->HackFlags & PCI_HACK_NO_ASPM_FOR_EXPRESS_LINK) != 0) {
            bridgeInterface->DisableLinkPowerManagement = 1;
        }

        bridgeInterface->ExpressBaseVersion11OrGreater =
            device->ExpressPort->BaseVersion11OrGreater;

        if ((device->HackFlags & PCI_HACK_SBR_ON_LINK_STATE_CHANGE) != 0) {
            bridgeInterface->SecondaryBusResetOnLinkStateChange = 1;
        }

        if ((device->HackFlags & PCI_HACK_LINK_DISABLE_ON_SLOT_PWRDN) != 0) {
            bridgeInterface->LinkDisableOnSlotPowerDown = 1;
        }

        bridgeInterface->AerCapability =
            device->ExpressPort->AerCapability;

        bridgeInterface->AcsCapability =
            device->ExpressPort->AcsCapability;

        bridgeInterface->AtsCapability =
            device->ExpressPort->AtsCapability;

        bridgeInterface->AcsNotNeeded =
            PciIsDeviceFeatureEnabled(device, PCI_FEATURE_ACS_NOT_REQUIRED) ==
                PciFeatureStateEnabled;

        if (PciIsDeviceFeatureEnabled(device,
                                      PCI_FEATURE_ACS_DO_NOT_USE) == PciFeatureStateEnabled) {

            bridgeInterface->AcsCapability = 0;
        }

        if ((device->HackFlags & PCI_HACK_ACS_CAPABILITY_INCORRECT_FORMAT) != 0) {
            bridgeInterface->AcsCapIncorrectFormat = 1;
        }

        RtlCopyMemory(&bridgeInterface->ErrorSource,
                      &device->ExpressPort->ErrorSource,
                      sizeof(WHEA_ERROR_SOURCE_DESCRIPTOR));

        if ((Size == sizeof(PCI_BRIDGE_INTERFACE)) &&
            (device->ExpressPort->ExpressCapabilities.CapabilityVersion >= 2)) {
            bridgeInterface->GetPowerParameters = PciBridgeInterface_GetPowerParameters;
            bridgeInterface->AriForwardingEnable = PciBridgeInterface_AriForwardingEnable;
            bridgeInterface->ObffControl = PciBridgeInterface_ObffControl;
        }


        bridgeInterface->ThunderboltCapability =
            device->ExpressPort->ThunderboltCapability;
    }

    bridgeInterface->DelayPassive = PciBridgeInterface_Delay;
    bridgeInterface->StallDispatch = PciBridgeInterface_Stall;

    if (device->BusNumbersConfigured) {
        bridgeInterface->SecondaryBusNumber
            = device->SavedRegisters.bridge.SecondaryBusNumber;
        bridgeInterface->SubordinateBusNumber
            = device->SavedRegisters.bridge.SubordinateBusNumber;
    }

    bridgeInterface->IgnoreBootConfig = (device->IgnoreBootConfig? 1 : 0);
    status = PciGetSavedDeviceConfiguration(device, &savedConfig);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    if (PciIsDeviceFeatureEnabled(device, PCI_FEATURE_ENABLE_BRIDGE_INTERRUPTS)
            == PciFeatureStateEnabled) {

        bridgeInterface->EnableAllBridgeInterrupts = 1;
    }

    status = PciDeviceRequirementsToBridgeRequirements(
                    device,
                    &device->Requirements,
                    &bridgeInterface->MinimumRequirements
                    );
    if (!NT_SUCCESS(status)) {
        return status;
    }

    if (savedConfig.Bridge.IoWindowDisabled) {
        bridgeInterface->MinimumRequirements.IoWindow.Disabled = TRUE;
    }
    if (savedConfig.Bridge.MemoryWindowDisabled) {
        bridgeInterface->MinimumRequirements.MemoryWindow.Disabled = TRUE;
    }
    if (savedConfig.Bridge.PrefetchWindowDisabled) {
        bridgeInterface->MinimumRequirements.PrefetchWindow.Disabled = TRUE;
    }

    status = PciDeviceResourcesToBridgeResources(
                device,
                &savedConfig.Resources,
                &bridgeInterface->BiosConfiguration
                );
    if (!NT_SUCCESS(status)) {
        return status;
    }

    if (PciSubtractiveDecodeBridge(device)) {
        bridgeInterface->Subtractive = TRUE;

        //
        // If a subtractive bridge has ignore boot config flag set and
        // a debugging device behind it then set the disable fields for
        // each of the requirements that it is currently decoding. This
        // is because the ignore boot config flag will kill boot resources
        // and presence of debugging device will not allow creating generic
        // requirements for an active resource. Disabling resource type
        // will allow these resources to be arbitrated from outside the
        // bridge windows for the subtractive bridge.
        //

        if ((device->HackFlags & PCI_HACK_IGNORE_BOOT_CONFIG) &&
            (device->OnDebugPath)) {

            if (device->SavedRegisters.Command & PCI_ENABLE_IO_SPACE) {
                bridgeInterface->MinimumRequirements.IoWindow.Disabled = TRUE;
            }

            if (device->SavedRegisters.Command & PCI_ENABLE_MEMORY_SPACE) {
                bridgeInterface->MinimumRequirements.MemoryWindow.Disabled = TRUE;
                bridgeInterface->MinimumRequirements.PrefetchWindow.Disabled = TRUE;
            }
        }
    }

    //
    // Query the express capability register to find out whether End-End TLP prefix
    // is supported, whether AtomicOp routing is supported and whether 32/64/128 bit
    // completer is supported. Also cache the physical slot number.
    //

    if (device->ExpressCapability != 0) {
        PciReadExpressRegisterEx(device, DeviceCapabilities2, &deviceCap2);
        if (deviceCap2.EndEndTLPPrefixSupported != 0) {
            bridgeInterface->E2ETLPPrefixSupported = 1;
        }

        if (deviceCap2.AtomicOpRoutingSupported != 0) {
            bridgeInterface->AtomicsRoutingSupported = 1;
        }

        if (deviceCap2.AtomicOpCompleterSupported32Bit != 0) {
            bridgeInterface->Atomics32BitCompleterSupported = 1;
        }

        if (deviceCap2.AtomicOpCompleterSupported64Bit != 0) {
            bridgeInterface->Atomics64BitCompleterSupported = 1;
        }

        if (deviceCap2.CASCompleterSupported128Bit != 0) {
            bridgeInterface->Atomics128BitCompleterSupported = 1;
        }

        PciReadExpressRegisterEx(device, SlotCapabilities, &slotCap);
        bridgeInterface->PhysicalSlotNumber = (USHORT) slotCap.PhysicalSlotNumber;
    }

    //
    // And finally some bit settings
    //
    if (device->SavedRegisters.bridge.BridgeControl & PCI_ENABLE_BRIDGE_ISA) {
        bridgeInterface->IsaBitSet = TRUE;
    }

    if ((device->SavedRegisters.bridge.BridgeControl & PCI_ENABLE_BRIDGE_VGA) &&
        !(device->SavedRegisters.bridge.BridgeControl & PCI_ENABLE_BRIDGE_VGA_16BIT)) {
        bridgeInterface->LegacyVgaBitSet = TRUE;
    }

    if ((device->HackFlags & PCI_HACK_DOWNSTREAM_DEVICE_NOT_REMOVABLE) != 0) {
        bridgeInterface->DownstreamDeviceNotRemovable = TRUE;
    }

    bridgeInterface->BusmasterDisabledOnBoot = (device->BusmasterDisabledOnBoot? 1 : 0);
    return STATUS_SUCCESS;
}

NTSTATUS
PciDeviceResourcesToBridgeResources(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_RESOURCES DeviceResources,
    __out PPCI_BRIDGE_RESOURCES BridgeResources
    )
/*++

Routine Description:

    This routine converts the PCI_DEVICE_RESOURCES of a type1 or type2 device
    into the generic PCI_BRIDGE_RESOURCES used by the FDO code to modify
    bridge windows.

Arguments:

    Device - The PCI_DEVICE extension for the bridge in question.

    DeviceResources - The device resources to convert.

    BridgeResources - A buffer to contain the computed bridge resources.

Return Value:

    STATUS_SUCCESS if the routine completes successfully.
    STATUS_INVALID_PARAMETER if the device provided is not a type1 or type2 device.

--*/
{
    PCM_PARTIAL_RESOURCE_DESCRIPTOR ioWindow, memWindow, prefetchWindow;

    PAGED_CODE();

    switch (Device->HeaderType) {

    case PCI_BRIDGE_TYPE:

        ioWindow = &DeviceResources->type1.IoWindow;
        memWindow = &DeviceResources->type1.MemoryWindow;
        prefetchWindow = &DeviceResources->type1.PrefetchWindow;
        break;

    case PCI_CARDBUS_BRIDGE_TYPE:

        ioWindow = &DeviceResources->type2.IoWindows[0];
        PCI_ASSERT(DeviceResources->type2.IoWindows[1].Type == CmResourceTypeNull);

        if (Device->Requirements.type2.MemoryWindows[1].Flags & CM_RESOURCE_MEMORY_PREFETCHABLE) {
            memWindow = &DeviceResources->type2.MemoryWindows[0];
            prefetchWindow = &DeviceResources->type2.MemoryWindows[1];
        } else {
            prefetchWindow = &DeviceResources->type2.MemoryWindows[0];
            memWindow = &DeviceResources->type2.MemoryWindows[1];
        }
        break;

    default:
        PCI_ASSERT((Device->HeaderType == PCI_BRIDGE_TYPE) ||
                   (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE));
        return STATUS_INVALID_PARAMETER;
    }

    RtlCopyMemory(&BridgeResources->IoWindow,
                  ioWindow,
                  sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)
                  );
    RtlCopyMemory(&BridgeResources->MemoryWindow,
                  memWindow,
                  sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)
                  );
    RtlCopyMemory(&BridgeResources->PrefetchWindow,
                  prefetchWindow,
                  sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)
                  );

    return STATUS_SUCCESS;
}

NTSTATUS
PciDeviceRequirementsToBridgeRequirements(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_REQUIREMENTS DeviceRequirements,
    __out PPCI_BRIDGE_REQUIREMENTS BridgeRequirements
    )
/*++

Routine Description:

    This routine converts the PCI_DEVICE_REQUIREMENTS of a type1 or type2 device
    into the generic PCI_BRIDGE_REQUIREMENTS used by the FDO code to modify
    bridge windows.

Arguments:

    Device - The PCI_DEVICE extension for the bridge in question.

    DeviceRequirements - The device requirements to convert.

    BridgeRequirements - A buffer to contain the computed bridge requirements.

Return Value:

    STATUS_SUCCESS if the routine completes successfully.
    STATUS_INVALID_PARAMETER if the device provided is not a type1 or type2 device.

--*/
{
    PIO_RESOURCE_DESCRIPTOR ioWindow, memWindow, prefetchWindow;

    PAGED_CODE();

    switch (Device->HeaderType) {

    case PCI_BRIDGE_TYPE:

        ioWindow = &DeviceRequirements->type1.IoWindow;
        memWindow = &DeviceRequirements->type1.MemoryWindow;
        prefetchWindow = &DeviceRequirements->type1.PrefetchWindow;
        break;

    case PCI_CARDBUS_BRIDGE_TYPE:

        ioWindow = &DeviceRequirements->type2.IoWindows[0];
        PCI_ASSERT(DeviceRequirements->type2.IoWindows[1].Type == CmResourceTypeNull);

        if (DeviceRequirements->type2.MemoryWindows[1].Flags & CM_RESOURCE_MEMORY_PREFETCHABLE) {
            memWindow = &DeviceRequirements->type2.MemoryWindows[0];
            prefetchWindow = &DeviceRequirements->type2.MemoryWindows[1];
        } else {
            prefetchWindow = &DeviceRequirements->type2.MemoryWindows[0];
            memWindow = &DeviceRequirements->type2.MemoryWindows[1];
        }
        break;

    default:
        PCI_ASSERT((Device->HeaderType == PCI_BRIDGE_TYPE) ||
                   (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE));
        return STATUS_INVALID_PARAMETER;
    }

    if (ioWindow->Type == CmResourceTypeNull) {
        BridgeRequirements->IoWindow.Disabled = TRUE;
    } else {
        BridgeRequirements->IoWindow.Disabled = FALSE;
        BridgeRequirements->IoWindow.Size = (ULONGLONG)ioWindow->u.Port.Length;
        BridgeRequirements->IoWindow.Alignment = (ULONGLONG)ioWindow->u.Port.Alignment;
    }

    if (memWindow->Type == CmResourceTypeNull) {
        BridgeRequirements->MemoryWindow.Disabled = TRUE;
    } else {
        BridgeRequirements->MemoryWindow.Disabled = FALSE;
        BridgeRequirements->MemoryWindow.Size = (ULONGLONG)memWindow->u.Memory.Length;
        BridgeRequirements->MemoryWindow.Alignment = (ULONGLONG)memWindow->u.Memory.Alignment;
    }

    if (prefetchWindow->Type == CmResourceTypeNull) {
        BridgeRequirements->PrefetchWindow.Disabled = TRUE;
    } else {
        BridgeRequirements->PrefetchWindow.Disabled = FALSE;
        BridgeRequirements->PrefetchWindow.Size = RtlIoDecodeMemIoResource(prefetchWindow,
            &BridgeRequirements->PrefetchWindow.Alignment,
            NULL,
            NULL);
    }

    return STATUS_SUCCESS;
}

VOID
PciBridgeAssertSecondaryReset (
    __inout PPCI_BRIDGE Bridge
    )

/*++

Routine Description:

    This routine is invoked to assert a secondary bus reset.

Arguments:

    Bridge - Supplies a pointer to a PCI bridge.

Return Value:

    None.

--*/

{

    USHORT BridgeControl;

    NON_PAGED_CODE();

    PciReadBridgeConfigRegister(Bridge,
                                type1.BridgeControl,
                                &BridgeControl);

    BridgeControl |= PCI_ASSERT_BRIDGE_RESET;
    PciWriteBridgeConfigRegister(Bridge,
                                 type1.BridgeControl,
                                 &BridgeControl);

    Bridge->StallDispatch(Bridge->InterfaceContext, 100);
    return;
}

VOID
PciBridgeDeassertSecondaryReset (
    __inout PPCI_BRIDGE Bridge
    )

/*++

Routine Description:

    This routine is invoked to de-assert a secondary bus reset.

Arguments:

    Bridge - Supplies a pointer to a PCI bridge.

Return Value:

    None.

--*/

{

    USHORT BridgeControl;

    NON_PAGED_CODE();

    PciReadBridgeConfigRegister(Bridge,
                                type1.BridgeControl,
                                &BridgeControl);

    BridgeControl &= ~PCI_ASSERT_BRIDGE_RESET;
    PciWriteBridgeConfigRegister(Bridge,
                                 type1.BridgeControl,
                                 &BridgeControl);

    Bridge->StallDispatch(Bridge->InterfaceContext, 100);

    //
    // If this is an Express bridge, clear the Data Link Layer State Changed
    // bit that will now be set because of the reset.
    //

    if (Bridge->ExpressBridge != NULL) {

        PCI_EXPRESS_SLOT_STATUS_REGISTER ExpressSlotStatusRegister;

        Bridge->StallDispatch(Bridge->InterfaceContext, 200);

        ExpressSlotStatusRegister.AsUSHORT = 0;
        ExpressSlotStatusRegister.DataLinkStateChanged = 1;
        PciWriteExpressBridgeRegister(Bridge->ExpressBridge,
                                      SlotStatus,
                                      &ExpressSlotStatusRegister);

        Bridge->StallDispatch(Bridge->InterfaceContext, 200);
    }

    return;
}

