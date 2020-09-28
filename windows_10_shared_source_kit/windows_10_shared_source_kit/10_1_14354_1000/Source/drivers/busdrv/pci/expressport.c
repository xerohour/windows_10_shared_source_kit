/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    expressport.c

Abstract:

    This module contains functions associated with configuring the PCI
    Express-specific features of Express devices.









--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include <initguid.h>
#include "ntpoapi.h"
#include "pciprop.h"

// --------------------------------------------------------------------- Defines

#define EXPRESS_LINK_QUIESCENT_CONTEXT_SIGNATURE        'suQL'
#define EXPRESS_ROOT_PORT_CONTEXT_SIGNATURE             'CpRE'

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
ExpressCompatibilitySaveExtendedCapabilities (
    __inout PEXPRESS_COMPATIBILITY_PORT CompatibilityPort
    );

NTSTATUS
ExpressValidateFabricTopology (
    __in PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressUpdatePortLineage (
    __inout PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressProcessPortRegisters (
    __inout PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressProcessExtendedPortCapabilities (
    __inout PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressSaveExtendedPortCapabilityPointers (
    __inout PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressProcessSerialNumberCapability (
    __inout PEXPRESS_PORT ExpressPort
    );

VOID
ExpressProcessAriCapability (
    __inout PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressProcessSriovCapability (
    __inout PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressProcessAerCapability (
    __inout PEXPRESS_PORT ExpressPort
    );

BOOLEAN
ExpressVerifyAcsCapability(
    _In_ PCI_EXPRESS_DEVICE_TYPE                DeviceType,
    _In_ PCI_EXPRESS_ACS_CAPABILITY_REGISTER    AcsCapabilities,
    _In_ USHORT                                 AtsCapability
    );

ULONG
ExpressCheckAcsHierarchyCompatibility (
    _In_ PEXPRESS_PORT ExpressPort
    );

VOID
ExpressAddIntegratedPortToRootComplex (
    __inout PROOT_COMPLEX RootComplex,
    __inout PEXPRESS_PORT ExpressPort
    );

_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR ExpressLinkQuiescentInterface_Constructor;

PCI_EXPRESS_ENTER_LINK_QUIESCENT_MODE ExpressPortEnterLinkQuiescentMode;
PCI_EXPRESS_EXIT_LINK_QUIESCENT_MODE ExpressPortExitLinkQuiescentMode;

VOID
ExpressLinkQuiescentReference (
    __inout PVOID Context
    );

VOID
ExpressLinkQuiescentDereference (
    __inout PVOID Context
    );

_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR ExpressRootPortInterface_Constructor;

ULONG
ExpressRootPortReadConfigSpace (
    __in PVOID Context,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    );

ULONG
ExpressRootPortWriteConfigSpace (
    __in PVOID Context,
    __in_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    );

VOID
ExpressRootPortInterfaceReference (
    __inout PVOID Context
    );

VOID
ExpressRootPortInterfaceDereference (
    __inout PVOID Context
    );

NTSTATUS
ExpressCheckDeviceOnRootPort (
    __in PPCI_DEVICE Device
    );

VOID
ExpressInitErrorSourceInfo (
    __inout PEXPRESS_PORT ExpressPort
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, ExpressAddIntegratedPortToRootComplex)
    #pragma alloc_text(PAGE, ExpressCheckDeviceOnRootPort)
    #pragma alloc_text(PAGE, ExpressCompatibilitySaveExtendedCapabilities)
    #pragma alloc_text(PAGE, ExpressDetermineSlotNumber)
    #pragma alloc_text(PAGE, ExpressInitErrorSourceInfo)
    #pragma alloc_text(PAGE, ExpressInitializeNewCompatibilityPort)
    #pragma alloc_text(PAGE, ExpressLinkQuiescentDereference)
    #pragma alloc_text(PAGE, ExpressLinkQuiescentInterface_Constructor)
    #pragma alloc_text(PAGE, ExpressLinkQuiescentReference)
    #pragma alloc_text(PAGE, ExpressPortEnterLinkQuiescentMode)
    #pragma alloc_text(PAGE, ExpressPortExitLinkQuiescentMode)
    #pragma alloc_text(PAGE, ExpressPortInitializeAERSupport)
    #pragma alloc_text(PAGE, ExpressProcessAerCapability)
    #pragma alloc_text(PAGE, ExpressProcessAriCapability)
    #pragma alloc_text(PAGE, ExpressVerifyAcsCapability)
    #pragma alloc_text(PAGE, ExpressCheckAcsHierarchyCompatibility)
    #pragma alloc_text(PAGE, ExpressProcessExtendedPortCapabilities)
    #pragma alloc_text(PAGE, ExpressProcessNewPort)
    #pragma alloc_text(PAGE, ExpressProcessPortRegisters)
    #pragma alloc_text(PAGE, ExpressProcessSerialNumberCapability)
    #pragma alloc_text(PAGE, ExpressProcessSriovCapability)
    #pragma alloc_text(PAGE, ExpressRemoveIntegratedPortFromRootComplex)
    #pragma alloc_text(PAGE, ExpressRootPortInterface_Constructor)
    #pragma alloc_text(PAGE, ExpressRootPortInterfaceDereference)
    #pragma alloc_text(PAGE, ExpressRootPortInterfaceReference)
    #pragma alloc_text(PAGE, ExpressSaveExtendedPortCapabilityPointers)
    #pragma alloc_text(PAGE, ExpressUpdatePortLineage)
    #pragma alloc_text(PAGE, ExpressValidateFabricTopology)
#endif

// --------------------------------------------------------------------- Globals

const PCI_INTERFACE ExpressLinkQuiescentInterface = {
    &GUID_PCI_EXPRESS_LINK_QUIESCENT_INTERFACE,       // InterfaceType
    sizeof(PCI_EXPRESS_LINK_QUIESCENT_INTERFACE),     // MinSize
    PCI_EXPRESS_LINK_QUIESCENT_INTERFACE_VERSION,     // MinVersion
    PCI_EXPRESS_LINK_QUIESCENT_INTERFACE_VERSION,     // MaxVersion
    PCIIF_PDO,                                        // Flags - supported on PDOs
    PciInterface_ExpressLinkQuiescent,                // Signature
    ExpressLinkQuiescentInterface_Constructor         // Constructor
    };

const PCI_INTERFACE ExpressRootPortInterface = {
    &GUID_PCI_EXPRESS_ROOT_PORT_INTERFACE,            // InterfaceType
    sizeof(PCI_EXPRESS_ROOT_PORT_INTERFACE),          // MinSize
    PCI_EXPRESS_ROOT_PORT_INTERFACE_VERSION,          // MinVersion
    PCI_EXPRESS_ROOT_PORT_INTERFACE_VERSION,          // MaxVersion
    PCIIF_PDO,                                        // Flags
    PciInterface_ExpressRootPortAccess,               // Signature
    ExpressRootPortInterface_Constructor              // Constructor
    };


volatile LONG PciDebugExpressCompatibilitySaveDone;
volatile LONG PciDebugExpressCompatibilitySaveNotDone;
volatile LONG PciDebugExpressNativeSaveDone;
volatile LONG PciDebugExpressNativeSaveNotDone;

// ------------------------------------------------------------------- Functions

NTSTATUS
ExpressInitializeNewCompatibilityPort (
    __in PPCI_DEVICE Device
    )

/*++

Routine description:

    This routine initializes a new PCI Express device which is under control
    of the system BIOS.  It creates a structure which contains
    read-only information which is exposed through PCI custom properties.

Arguments:

    Device - Supplies the PCI device whose capabilities are processed.

    NewExpressCompatibilityPort - Supplies a pointer to hold a new structure
        that contains PCI Express properties for this device.

Return Value:

    STATUS_INSUFFICIENT_RESOURCES on memory allocation failure.

    STATUS_SUCCESS if the routine completes successfully.

    Otherwise failure code returned from other function calls.

--*/

{

    PEXPRESS_COMPATIBILITY_PORT CompatibilityPort;
    PCI_EXPRESS_CAPABILITIES_REGISTER ExpressCapabilitiesRegister;

    PAGED_CODE();

    PCI_ASSERT(Device->ExpressCapability != 0);

    CompatibilityPort = PciAllocatePool(NonPagedPoolNx,
                                        sizeof(EXPRESS_COMPATIBILITY_PORT));

    if (CompatibilityPort == NULL) {
        PciSetEnumFailureCode(Device, EXPRESS_ALLOCATION_FAILED);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(CompatibilityPort, sizeof(EXPRESS_COMPATIBILITY_PORT));
    CompatibilityPort->Device = Device;
    Device->ExpressCompatibilityPort = CompatibilityPort;
    PciReadExpressRegisterEx(Device,
                             ExpressCapabilities,
                             &ExpressCapabilitiesRegister);

    if (ExpressCapabilitiesRegister.CapabilityVersion == 0) {
        ExpressDestroyCompatibilityPort(CompatibilityPort);
        return STATUS_DEVICE_DATA_ERROR;
    }

    CompatibilityPort->DeviceType = ExpressCapabilitiesRegister.DeviceType;
    PciReadExpressRegisterEx(Device,
                             DeviceControl,
                             &CompatibilityPort->DeviceControl);

    PciReadExpressRegisterEx(Device,
                             DeviceCapabilities,
                             &CompatibilityPort->DeviceCapabilities);

    //
    // Find the base version of the device.  There is a role based error
    // reporting field in the device capabilities register in the express
    // register blob. This bit was introduced with the last ECN that was
    // added to the PCI Express Base Spec 1.1. So this field will be used
    // to determine if the device conforms to the PCI Express Base 1.1 spec.
    //

    CompatibilityPort->BaseVersion11OrGreater =
        CompatibilityPort->DeviceCapabilities.RoleBasedErrorReporting;

    //
    // The link registers are in the domain of the port if this is an ingress
    // link port (it's an endpoint device, PCI Express to PCI/PCI-X bridge or
    // upstream ports of switches and is further away from the root port than
    // its terminal link).  Otherwise, they're in the domain of the express
    // bridge at the FDO.
    //

    CompatibilityPort->ExpressLinkValid = FALSE;
    if (EXPRESS_COMPATIBILITY_INGRESS_LINK_PORT(CompatibilityPort) != FALSE) {
        CompatibilityPort->ExpressLinkValid = TRUE;
        PciReadExpressRegisterEx(Device,
                                 LinkCapabilities,
                                 &CompatibilityPort->LinkCapabilities);

        PciReadExpressRegisterEx(Device,
                                 LinkControl,
                                 &CompatibilityPort->LinkControl);

        PciReadExpressRegisterEx(Device,
                                 LinkStatus,
                                 &CompatibilityPort->LinkStatus);
    }

    //
    // Check availability of capabilities in the extended configuration space
    // such as Advanced Error Reporting.
    //

    if (CompatibilityPort->Device->ExtendedConfigAvailable != FALSE) {

        //
        // Errors in retrieving capabilities can be ignored because the default
        // is to indicate that these capabilites are unavailable.
        //

        ExpressCompatibilitySaveExtendedCapabilities(CompatibilityPort);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
ExpressProcessNewPort (
    __in PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine initializes a new PCI Express device.  It creates an
    EXPRESS_PORT structure, initializes it, and links it to the generic
    PCI_DEVICE structure.

Arguments:

    Device - Supplies the Pci device whose capabilities are processed.

    NewExpressPort - Supplies a placeholder for a new structure called
        EXPRESS_PORT that is created in this routine on a successful exit.

Return Value:

    STATUS_INSUFFICIENT_RESOURCES on memory allocation failure.

    STATUS_DEVICE_DATA_ERROR if the express capabilities are invalid.

    STATUS_SUCCESS if the routine completes successfully.

    Otherwise failure code returned from other function calls.

--*/

{

    PEXPRESS_PORT ExpressPort;
    PPCI_BUS ParentBus;
    NTSTATUS Status;

    PAGED_CODE();

    PCI_ASSERT(Device->ExpressCapability != 0);

    ExpressPort = PciAllocatePool(NonPagedPoolNx, sizeof(EXPRESS_PORT));
    if (ExpressPort == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(ExpressPort, sizeof(EXPRESS_PORT));
    ExpressPort->Device = Device;
    Device->ExpressPort = ExpressPort;
    PciReadExpressRegister(ExpressPort,
                           ExpressCapabilities,
                           &ExpressPort->ExpressCapabilities);

    if (ExpressPort->ExpressCapabilities.CapabilityVersion == 0) {
        PciSetEnumFailureCode(Device, EXPRESS_INVALID_CAPABILITY_VERSION);
        Status = STATUS_DEVICE_DATA_ERROR;
        goto Cleanup;
    }

    if ((ExpressPort->Device->HackFlags & PCI_HACK_EXPRESS_VERSION_IS_2) != 0) {
        ExpressPort->ExpressCapabilities.CapabilityVersion = 2;
    }

    //
    // Save device type.
    //

    ExpressPort->DeviceType = ExpressPort->ExpressCapabilities.DeviceType;

    //
    // Indulge in some Express hierarchy validation.
    //

    Status = ExpressValidateFabricTopology(ExpressPort);
    if (!NT_SUCCESS(Status)) {

        //
        // Allow a failure in topology validation if the device is an RC device
        // and the appropriate hack flag is set.  Illegal root complex
        // topologies might work fine, but illegal topologies deeper within the
        // hierarchy will cause inconsistencies and are not allowed.
        //

        if ((Device->HackFlags & PCI_HACK_IGNORE_ROOT_TOPOLOGY) &&
            ((ExpressPort->DeviceType == PciExpressRootPort) ||
             EXPRESS_RC_INTEGRATED_DEVICE(ExpressPort))) {

            //
            // Fall through.
            //

            NOTHING;

        } else {
            PciSetEnumFailureCode(Device, EXPRESS_INVALID_HIERARCHY);
            goto Cleanup;
        }
    }

    //
    // Now update the lineage for a newly discovered PCI Express device.
    //

    Status = ExpressUpdatePortLineage(ExpressPort);
    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    //
    // Process express port registers required for base functionality.
    // This includes allocation of certain structures, verification of
    // capabilities and caching of certain register values.
    //

    Status = ExpressProcessPortRegisters(ExpressPort);
    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    if (Device->ExtendedConfigAvailable != FALSE) {
        Status = ExpressProcessExtendedPortCapabilities(ExpressPort);
        if (!NT_SUCCESS(Status)) {
            PciSetEnumFailureCode(ExpressPort->Device,
                                  EXPRESS_INVALID_EXTENDED_CAPABILITY);

            goto Cleanup;
        }
    }

    //
    // Link the RC Integrated EP or RC Event Collector to the Root Complex.
    //

    if (EXPRESS_RC_INTEGRATED_DEVICE(ExpressPort)) {
        ExpressAddIntegratedPortToRootComplex(ExpressPort->RootComplex,
                                              ExpressPort);
    }

    return STATUS_SUCCESS;

Cleanup:

    if (ExpressPort != NULL) {
        ParentBus = Device->Parent;
        PciAcquirePassiveLock(&ParentBus->ChildDeviceLock);
        ExpressDestroyPort(ExpressPort);
        PciReleasePassiveLock(&ParentBus->ChildDeviceLock);
    }

    return Status;
}

NTSTATUS
ExpressValidateFabricTopology (
    __in PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine validates the Express hierarchy in accordance to the
    PCI Express Base Specification. Failure from this routine will cause
    the device to fail enumeration.

Arguments:

    ExpressPort - Supplies a pointer to the EXPRESS_PORT structure
        for the device.

Return Value:

    STATUS_DEVICE_DATA_ERROR on illegal composition of Express hierarchies.

    STATUS_SUCCESS on success.

--*/

{

    PPCI_DEVICE Device;
    NTSTATUS Status;

    PAGED_CODE();

    Device = ExpressPort->Device;

    //
    // Start with an assumption that the hierarchy is ill-composed.
    //

    Status = STATUS_DEVICE_DATA_ERROR;

    //
    // Validate hierarchy based on Express device type.
    //

    switch (ExpressPort->DeviceType) {
    case PciExpressRootPort:
    case PciExpressRootComplexEventCollector:
    case PciExpressRootComplexIntegratedEndpoint:

        //
        // Root Ports, RC Event Collectors and RC Integrated
        // Endpoints can only be found in a root complex.
        //

        if (!PCI_ROOT_BUS(Device->Parent)) {
            break;
        }

        Status = STATUS_SUCCESS;
        break;

    case PciExpressEndpoint:
    case PciExpressLegacyEndpoint:
    case PciExpressUpstreamSwitchPort:
    case PciExpressToPciXBridge:

        //
        // Other than the Root Ports, RC Event Collectors and RC Integrated
        // Endpoints, none of the Express devices should be found in the root
        // complex. So bail out if the immediate parent is a root and not an
        // Express (Virtual PCI-PCI) bridge.
        //

        if (PCI_ROOT_BUS(Device->Parent)) {
            break;
        }

        if (Device->Parent->PciBridge->ExpressBridge == NULL) {
            break;
        }

        //
        // None of these devices should be in a switch complex. So bail out
        // if any of these are downstream of an upstream Switch port.
        //

        if (Device->Parent->PciBridge->ExpressBridge->BridgeType ==
            PciExpressUpstreamSwitchPort) {

            break;
        }

        //
        // Unless ARI is enabled on the parent bridge, PCI Express
        // endpoints and switches must be on device 0 as express links
        // are represented as buses with only one device on them.
        //

        if (Device->Slot.u.bits.DeviceNumber != 0 &&
            Device->Parent->PciBridge->ExpressBridge->AriForwardingEnabled == FALSE) {

            break;
        }

        Status = STATUS_SUCCESS;
        break;

    case PciExpressDownstreamSwitchPort:

        //
        // The Express device representing a downstream switch port is a virtual
        // PCI-PCI bridge below an upstream Switch port. Validate this and bail
        // out if this requirement is not satisfied.
        //

        if (PCI_ROOT_BUS(Device->Parent)) {
            break;
        }

        if (Device->Parent->PciBridge->ExpressBridge == NULL) {
            break;
        }

        if (Device->Parent->PciBridge->ExpressBridge->BridgeType !=
            PciExpressUpstreamSwitchPort) {

            break;
        }

        Status = STATUS_SUCCESS;
        break;

    case PciXToExpressBridge:

        //
        // A 'PCI/PCI-X to PCI Express Bridge' is a supported device type but
        // there is no specification to describe its functionality. So, fail
        // its validation for a problem code 10 to be assigned to this device.
        //

        break;

    default:

        //
        // If the PCI Express Logical Device type is none of
        // the above, then it is an error.
        //

        break;
    }

    return Status;
}

NTSTATUS
ExpressUpdatePortLineage (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine is invoked to update the lineage for a newly discovered
    Express device.

    If an Express device is a RC Integrated Endpoint, RC Event Collector
    or a Root Port, then a structure representing a Root Complex is allocated
    and linked to the root bus if required.

Arguments:

    ExpressPort - Supplies a pointer to the EXPRES_PORT structure
        for the device.

Return Value:

    STATUS_SUCCESS - on success or failure returned from a function call.

--*/

{

    PPCI_DEVICE Device;
    PEXPRESS_BRIDGE Parent;
    NTSTATUS Status;

    PAGED_CODE();

    Device = ExpressPort->Device;
    Status = STATUS_SUCCESS;

    switch (ExpressPort->DeviceType) {
    case PciExpressRootPort:
    case PciExpressRootComplexEventCollector:
    case PciExpressRootComplexIntegratedEndpoint:

        //
        // Based on the root bus capabilities provided by ACPI, a structure
        // representing an Express Root Complex was allocated and linked
        // to the root bus. If the reported capabilities were incorrect for
        // some reason, then such Root Complex will not be found. Ignore this
        // and proceed with allocating a new one.
        //

        if (Device->Parent->Root->RootComplex == NULL) {
            Status = ExpressProcessNewRootComplex(Device->Parent);
            if (!NT_SUCCESS(Status)) {
                break;
            }
        }

        ExpressPort->RootComplex = Device->Parent->Root->RootComplex;
        break;

    default:

        Parent = Device->Parent->PciBridge->ExpressBridge;
        ExpressPort->Parent = Parent;
        ExpressPort->RootComplex = Parent->RootComplex;
        switch (Parent->BridgeType) {
        case PciExpressRootPort:
            ExpressPort->RootPortBridge = Parent;
            if (Parent->Downstream.HotPlugCapable != FALSE) {

                PCI_ASSERT(Parent->Downstream.HotPlugController != NULL);

                Device->HotPlugSlot =
                    &Parent->Downstream.HotPlugController->Slots[0];
            }

            break;

        case PciExpressUpstreamSwitchPort:
            ExpressPort->RootPortBridge =
                Parent->UpstreamSwitchPort.RootPortBridge;

            break;

        case PciExpressDownstreamSwitchPort:
            ExpressPort->RootPortBridge =
                Parent->Downstream.u.SwitchPort.RootPortBridge;

            if (Parent->Downstream.HotPlugCapable != FALSE) {

                PCI_ASSERT(Parent->Downstream.HotPlugController != NULL);

                Device->HotPlugSlot =
                    &Parent->Downstream.HotPlugController->Slots[0];
            }

            break;

        default:

            PCI_ASSERT(FALSE);

            break;
        }

        break;
    }

    return Status;
}

NTSTATUS
ExpressProcessPortRegisters (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine is invoked for processing the Express registers
    on the device for base functionality. This includes verification
    of capabilities and caching of some register values so that they
    can be saved and restored across PnP and Power events.

Arguments:

    ExpressPort - Supplies a pointer to the EXPRES_PORT structure
        for the device.

Return Value:

    STATUS_INSUFFICIENT_RESOURCE - on memory allocation failure.

    STATUS_SUCCESS - on success.

--*/

{

    BOOLEAN ClearNoSnoop;
    PEXPRESS_LINK ExpressLink;
    PEXPRESS_BRIDGE Parent;
    PPCI_BUS ParentBus;

    PAGED_CODE();

    //
    // Device Control Register is found on all Express devices and
    // needs to be saved/restored across PnP and Power events.
    //

    PciReadExpressRegister(ExpressPort,
                           DeviceControl,
                           &ExpressPort->DeviceControl);

    //
    // Cache the device capabilities register for determining the capabilities
    // of an express device.
    //

    PciReadExpressRegister(ExpressPort,
                           DeviceCapabilities,
                           &ExpressPort->DeviceCapabilities);

    if (ExpressPort->Device->MpsSizeOverrideSet != 0) {
        ExpressPort->DeviceCapabilities.MaxPayloadSizeSupported =
            ExpressPort->Device->MpsSizeOverride;
    }

    if (ExpressPort->ExpressCapabilities.CapabilityVersion >= 2) {

        //
        // Save the Device Control 2 Register. This is restored
        // on PnP/Power on events.
        //

        PciReadExpressRegister(ExpressPort,
                               DeviceControl2,
                               &ExpressPort->DeviceControl2);
        //
        // Save the Device Capabilties 2 Register.
        //

        PciReadExpressRegister(ExpressPort,
                               DeviceCapabilities2,
                               &ExpressPort->DeviceCapabilities2);
    }

    //
    // Find the base version of the device. There is a role based error
    // reporting field in the device capabilities register in the express
    // register blob. This bit was introduced with the last ECN that was
    // added to the PCI Express Base Spec 1.1. So this field will be used
    // to determine if the device conforms to the PCI Express Base 1.1 spec.
    //

    ExpressPort->BaseVersion11OrGreater =
        ExpressPort->DeviceCapabilities.RoleBasedErrorReporting;

    //
    // Check if the express port supports an extended tag field and if so then
    // enable it.
    //

    if (ExpressPort->DeviceCapabilities.ExtendedTagSupported != 0) {
        ExpressPort->DeviceControl.ExtendedTagEnable = 1;
    }

    //
    // Clear the 'Enable No Snoop' bit in the device control register.
    // This will prevent devices from inadvertently using this feature.
    // For performance gains, this bit can be reset by the driver.
    //

    ClearNoSnoop = TRUE;
    if (ExpressPort->Device->OnPostPath != FALSE) {
        ClearNoSnoop = FALSE;
    }

    if (ClearNoSnoop != FALSE) {
        ExpressPort->DeviceControl.NoSnoopEnable = 0;
    }

    //
    // The link registers are in the domain of the Port if this is an ingress
    // link port (it's an Endpoint device, PCI Express to PCI/PCI-X bridges or
    // Upstream Ports of Switches and is further away from the root port than
    // its terminal link. Otherwise, they're in the domain of the Express bridge
    // at the FDO.
    //

    if (EXPRESS_INGRESS_LINK_PORT(ExpressPort)) {
        ExpressLink = PciAllocatePool(NonPagedPoolNx, sizeof(EXPRESS_LINK));
        if (ExpressLink == NULL) {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        RtlZeroMemory(ExpressLink, sizeof(EXPRESS_LINK));

        //
        // Save the link capabilities for an ingress link port.
        //

        PciReadExpressRegister(ExpressPort,
                               LinkCapabilities,
                               &ExpressLink->LinkCapabilities);

        //
        // Now, fetch the link control register and modify certain fields as
        // appropriate.
        //

        PciReadExpressRegister(ExpressPort,
                               LinkControl,
                               &ExpressLink->LinkControl);

        //
        // The Read Completion Boundary (RCB) parameter determines the naturally
        // aligned address boundaries for read requests. For Endpoints, this will
        // be set to the RCB value of the Root Port upstream from the Endpoint.
        //

        if (ExpressPort->DeviceType != PciExpressUpstreamSwitchPort) {

            PCI_ASSERT(ExpressPort->RootPortBridge != NULL);

            ExpressLink->LinkControl.ReadCompletionBoundary =
                ExpressPort->RootPortBridge->Downstream.u.RootPort.ReadCompletionBoundary;

        }

        //
        // This is a Hack for a specific device that doesn't support ClockPowerManagement.
        // TODO: Replace this with a general hackflag mechanism
        // See TravM or JasonGos for details.
        //

        if (ExpressPort->DeviceType == PciExpressEndpoint && 
            ExpressPort->Device->VendorID == 0x1414 && 
            ExpressPort->Device->DeviceID == 0x90) {

            ExpressLink->LinkControl.EnableClockPowerManagement = FALSE;
        }

        //
        // Make a copy of link status register.
        //

        PciReadExpressRegister(ExpressPort,
                               LinkStatus,
                               &ExpressLink->LinkStatus);

        //
        // The Express Link represents a Point-Point connection. The TargetLink
        // field in the ExpressLink structure points to the terminal link.
        // Update this field for both the parent and the child express device.
        // For a multi-function device this points to the Express Link on
        // function 0.
        //

        Parent = ExpressPort->Parent;
        ParentBus = Parent->PciBridge->Bus;
        if (DOWNSTREAM_PORT(Parent)) {

            PciAcquirePassiveLock(&ParentBus->ChildDeviceLock);
            ExpressLink->TargetLink = &Parent->Downstream.ExpressLink;
            if (Parent->Downstream.ExpressLink.TargetLink == NULL) {
                Parent->Downstream.ExpressLink.TargetLink = ExpressLink;
            }

            PciReleasePassiveLock(&ParentBus->ChildDeviceLock);

        } else {
            PCI_ASSERT(FALSE);
        }

        ExpressPort->ExpressLink = ExpressLink;
    }

    if (ExpressPort->ExpressCapabilities.SlotImplemented != 0) {

        //
        // Save the Slot Capabilities register.
        //

        PciReadExpressRegister(ExpressPort,
                               SlotCapabilities,
                               &ExpressPort->SlotCapabilities);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
ExpressProcessExtendedPortCapabilities (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine processes the capability list that lives in the extended
    configuration space.

Arguments:

    ExpressPort - Supplies the express device whose capabilities are to be
        processed.

Return Value:

    STATUS_SUCCESS - on success.

    Otherwise a failure code as returned from one of the function calls.

--*/

{

    PPCI_DEVICE Device;
    NTSTATUS Status;

    PAGED_CODE();

    PCI_ASSERT(ExpressPort->Device->ExtendedConfigAvailable != FALSE);

    //
    // Save the capabilities pointer for interesting ones.
    //

    Status = ExpressSaveExtendedPortCapabilityPointers(ExpressPort);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Process the serial number capability if present.
    //

    if (((PciSystemWideHackFlags & PCI_SYS_HACK_DISABLE_EXPRESS_SERIAL_NUMBER_SUPPORT) == 0) &&
        ExpressPort->SerialNumberCapability != 0) {
        ExpressProcessSerialNumberCapability(ExpressPort);
    }

    //
    // Process the AER capability if present.
    //

    Device = ExpressPort->Device;
    if ((ExpressPort->AerCapability != 0) &&
        (PciIsAerGranted(Device->Root)) &&
        ((Device->HackFlags & PCI_HACK_IGNORE_AER_CAPABILITY) == 0)) {

        //
        // Initialize this express device's error source information. First we
        // initialize using default values. Then we call the PSHED, which may
        // override the default values. This is where we pick up settings
        // supplied by a PSHED plug-in or through the ACPI error source
        // enumeration interface (i.e. HEST table).
        //

        ExpressInitErrorSourceInfo(ExpressPort);
        PshedGetErrorSourceInfo(WheaErrSrcTypePCIe,
                                &ExpressPort->ErrorSource);

        //
        // If this PCIe device is not configured for firmware-first error
        // handling, then we apply the settings in the error source descriptor
        // to the device's AER control fields. And, if the device is a root
        // port, we add an error source to WHEA's list of error sources.
        //

        if ((ExpressPort->ErrorSource.Flags &
            WHEA_ERROR_SOURCE_FLAG_FIRMWAREFIRST) == 0) {

            ExpressProcessAerCapability(ExpressPort);
        }
    }

    //
    // Process the ARI capability if present.
    //

    if (ExpressPort->AriCapability != 0) {
        ExpressProcessAriCapability(ExpressPort);
    }

    //
    // Process the SR-IOV capability if present.
    //

    if (ExpressPort->SriovCapability != 0) {
        ExpressProcessSriovCapability(ExpressPort);
    }

    return STATUS_SUCCESS;
}

VOID
ExpressInitErrorSourceInfo (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine fills in the express port's default error source information.

Arguments:

    ExpressPort - Supplies the express device whose capabilities are to be
        processed.

Return Value:

    None.

--*/

{

    PWHEA_AER_BRIDGE_DESCRIPTOR AerBridgeDescr;
    PWHEA_AER_ENDPOINT_DESCRIPTOR AerEndpointDescr;
    PWHEA_AER_ROOTPORT_DESCRIPTOR AerRootportDescr;

    PAGED_CODE();

    //
    // Initialize the fields that are common for all PCIe devices.
    //

    ExpressPort->ErrorSource.Length = sizeof(WHEA_ERROR_SOURCE_DESCRIPTOR);
    ExpressPort->ErrorSource.Version = WHEA_ERROR_SOURCE_DESCRIPTOR_VERSION_10;
    ExpressPort->ErrorSource.Type = WheaErrSrcTypePCIe;
    ExpressPort->ErrorSource.State = WheaErrSrcStateStopped;
    ExpressPort->ErrorSource.NumRecordsToPreallocate = PciWHEARecordPreallocationCount;
    ExpressPort->ErrorSource.MaxSectionsPerRecord = 2;
    ExpressPort->ErrorSource.ErrorSourceId = 0;
    ExpressPort->ErrorSource.Flags = 0;
    ExpressPort->ErrorSource.MaxRawDataLength = sizeof(WHEA_PCIEXPRESS_ERROR_SECTION);

    //
    // Initialize the fields depends on the DeviceType.
    //

    switch (ExpressPort->DeviceType) {

        //
        // Root port fields.
        //

        case PciExpressRootPort:
            AerRootportDescr =
                &ExpressPort->ErrorSource.Info.AerRootportDescriptor;

            AerRootportDescr->Enabled = 1;
            AerRootportDescr->Reserved = 0;
            AerRootportDescr->BusNumber = ExpressPort->Device->BusNumber;
            AerRootportDescr->Slot.u.AsULONG =
                ExpressPort->Device->Slot.u.AsULONG;

            //
            // For PCIe 1.0a devices - those that do not support role-based
            // error reporting per the Error Reporting ECN - UR error
            // propagation must be disabled to ensure PCI/PCI-X compatibility.
            //

            AerRootportDescr->UncorrectableErrorMask = 0x00000000;
            if (ExpressPort->DeviceCapabilities.RoleBasedErrorReporting != 1) {
                AerRootportDescr->UncorrectableErrorMask = 0x00100000;
            }

            AerRootportDescr->DeviceControl = 0x07;
            AerRootportDescr->UncorrectableErrorSeverity = 0x00062011;
            AerRootportDescr->CorrectableErrorMask = 0x00000000;
            AerRootportDescr->AdvancedCapsAndControl = 0x00000000;
            AerRootportDescr->RootErrorCommand = 0x00000007;
            AerRootportDescr->Type =
                WHEA_ERROR_SOURCE_DESCRIPTOR_TYPE_AERROOTPORT;

            break;

        //
        // Bridge fields.
        //

        case PciExpressToPciXBridge:
        case PciXToExpressBridge:
            AerBridgeDescr =
                &ExpressPort->ErrorSource.Info.AerBridgeDescriptor;

            AerBridgeDescr->Enabled = 1;
            AerBridgeDescr->Reserved = 0;
            AerBridgeDescr->BusNumber = ExpressPort->Device->BusNumber;
            AerBridgeDescr->Slot.u.AsULONG =
                ExpressPort->Device->Slot.u.AsULONG;

            //
            // For PCIe 1.0a devices - those that do not support role-based
            // error reporting per the Error Reporting ECN - UR error
            // propagation must be disabled to ensure PCI/PCI-X compatibility.
            //

            AerBridgeDescr->UncorrectableErrorMask = 0x00000000;
            if (ExpressPort->DeviceCapabilities.RoleBasedErrorReporting != 1) {
                AerBridgeDescr->UncorrectableErrorMask = 0x00100000;
            }

            AerBridgeDescr->DeviceControl = 0x07;
            AerBridgeDescr->UncorrectableErrorSeverity = 0x00062011;
            AerBridgeDescr->CorrectableErrorMask = 0x00000000;
            AerBridgeDescr->AdvancedCapsAndControl = 0x00000000;
            AerBridgeDescr->SecondaryUncorrectableErrorMask = 0x000017a8;
            AerBridgeDescr->SecondaryUncorrectableErrorSev = 0x00001340;
            AerBridgeDescr->SecondaryCapsAndControl = 0x00000000;
            AerBridgeDescr->Type =
                WHEA_ERROR_SOURCE_DESCRIPTOR_TYPE_AERBRIDGE;

            break;

        //
        // Endpoint fields.
        //

        case PciExpressEndpoint:
        case PciExpressLegacyEndpoint:
        case PciExpressUpstreamSwitchPort:
        case PciExpressDownstreamSwitchPort:
        case PciExpressRootComplexIntegratedEndpoint:
        case PciExpressRootComplexEventCollector:
        default:
            AerEndpointDescr =
                &ExpressPort->ErrorSource.Info.AerEndpointDescriptor;

            AerEndpointDescr->Enabled = 1;
            AerEndpointDescr->Reserved = 0;
            AerEndpointDescr->BusNumber = ExpressPort->Device->BusNumber;
            AerEndpointDescr->Slot.u.AsULONG =
                ExpressPort->Device->Slot.u.AsULONG;

            //
            // For PCIe 1.0a devices - those that do not support role-based
            // error reporting per the Error Reporting ECN - UR error
            // propagation must be disabled to ensure PCI/PCI-X compatibility.
            //

            AerEndpointDescr->UncorrectableErrorMask = 0x00000000;
            if (ExpressPort->DeviceCapabilities.RoleBasedErrorReporting != 1) {
                AerEndpointDescr->UncorrectableErrorMask = 0x00100000;
            }

            AerEndpointDescr->DeviceControl = 0x07;
            AerEndpointDescr->UncorrectableErrorSeverity = 0x00062011;
            AerEndpointDescr->CorrectableErrorMask = 0x00000000;
            AerEndpointDescr->AdvancedCapsAndControl = 0x00000000;
            AerEndpointDescr->Type =
                WHEA_ERROR_SOURCE_DESCRIPTOR_TYPE_AERENDPOINT;

            break;
    }

    return;
}

NTSTATUS
ExpressSaveExtendedPortCapabilityPointers (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine processes the capability list that lives in "enhanced"
    configuration space, the config space beginning at offset 0x100.

Arguments:

    ExpressPort - Supplies the Express device whose extended capabilities
        will be processed.

Return Value:

    STATUS_DEVICE_DATA_ERROR - if the extended capabilities are incorrectly
        laid out for the device.

    STATUS_SUCCESS - on success.

--*/

{

    USHORT CurrentCap;
    PPCI_DEVICE Device;
    PCI_EXPRESS_ENHANCED_CAPABILITY_HEADER Header;
    PCI_EXPRESS_VENDOR_SPECIFIC_CAPABILITY VsecHeader;
    VIRTUAL_CHANNEL_CAPABILITIES1 VcCap1;
    VIRTUAL_CHANNEL_CAPABILITIES2 VcCap2;
    VIRTUAL_RESOURCE VcResource;
    ULONG VcCapSize;
    ULONG VcCapTableEnd;
    ULONG ResIndex;

    PAGED_CODE();

    PCI_ASSERT(ExpressPort->Device->ExtendedConfigAvailable != FALSE);

    //
    // Extended Capabilities in device configuration space always begin at
    // offset 100h with a PCI Express Enhanced Capability header.
    //

    CurrentCap = 0x100;
    while (CurrentCap) {
        if (CurrentCap < 0x100) {

            VERIFIER("Extended capability pointer in the non-extended space");

            return STATUS_DEVICE_DATA_ERROR;
        }

        if ((CurrentCap & 0x3) != 0) {

            VERIFIER("Extended capability pointer not DWORD aligned");

            return STATUS_DEVICE_DATA_ERROR;
        }

        if (CurrentCap >
            (PCI_EXTENDED_CONFIG_LENGTH -
             sizeof(PCI_EXPRESS_ENHANCED_CAPABILITY_HEADER))) {

            VERIFIER("Extended capability pointer extends "
                     "beyond end of config space");

            return STATUS_DEVICE_DATA_ERROR;
        }

        Device = ExpressPort->Device;
        PciReadDeviceConfig(Device,
                            &Header,
                            CurrentCap,
                            sizeof(PCI_EXPRESS_ENHANCED_CAPABILITY_HEADER));

        switch (Header.CapabilityID) {
        case PCI_EXPRESS_INVALID_ENHANCED_CAP_ID:
            if (CurrentCap == 0x100) {

                //
                // If a premier enhanced capability header returns 0xFFFF,
                // then the express device does not have such capability.
                // Bail out with success.
                //

                return STATUS_SUCCESS;

            } else {

                //
                // An enhanced capability header returned 0xFFFF. Bail out with
                // error.
                //

                return STATUS_DEVICE_DATA_ERROR;
            }

            break;

        case PCI_EXPRESS_DEVICE_SERIAL_NUMBER_CAP_ID:
            ExpressPort->SerialNumberCapability = CurrentCap;
            break;

        case PCI_EXPRESS_ADVANCED_ERROR_REPORTING_CAP_ID:
            ExpressPort->AerCapability = CurrentCap;
            break;

        case PCI_EXPRESS_ARI_CAP_ID:
            ExpressPort->AriCapability = CurrentCap;
            break;

        case PCI_EXPRESS_ACCESS_CONTROL_SERVICES_CAP_ID:
            ExpressPort->AcsCapability = CurrentCap;
            break;

        case PCI_EXPRESS_ATS_CAP_ID:
            ExpressPort->AtsCapability = CurrentCap;
            break;

        case PCI_EXPRESS_PASID_CAP_ID:
            ExpressPort->PasidCapability = CurrentCap;
            break;

        case PCI_EXPRESS_PAGE_REQUEST_CAP_ID:
            ExpressPort->PriCapability = CurrentCap;
            break;

        case PCI_EXPRESS_SINGLE_ROOT_IO_VIRTUALIZATION_CAP_ID:
            ExpressPort->SriovCapability = CurrentCap;
            break;

        case PCI_EXPRESS_TPH_REQUESTER_CAP_ID:
            ExpressPort->TphRequesterCapability = CurrentCap;
            break;

        case PCI_EXPRESS_L1_PM_SS_CAP_ID:
            ExpressPort->L1PmSsCapability = CurrentCap;
            break;

        case PCI_EXPRESS_LTR_CAP_ID:
            if (EXPRESS_INGRESS_LINK_PORT(ExpressPort) ||
                EXPRESS_RC_INTEGRATED_DEVICE(ExpressPort)) {
                ExpressPort->LtrCapability = CurrentCap;
            } else {
                PCI_ASSERTMSG("LTR caps shouldn't exist on egress ports", FALSE);
            }
            break;

        case PCI_EXPRESS_VENDOR_SPECIFIC_CAP_ID:

            //
            // Read the capability ID and parse it if recognized.
            //

            PciReadDeviceConfig(Device,
                                &VsecHeader,
                                CurrentCap,
                                sizeof(PCI_EXPRESS_VENDOR_SPECIFIC_CAPABILITY));

            //
            // Look at the Vsec ID to determine if this is a thunderbolt
            // controller. This is the only way to recognize this controller
            // until a standard capability ID is introduced in the PCI SIG.
            // Currently only Intel specific IDs are recognized.
            //

            if ((Device->VendorID == PCI_VENDOR_INTEL) &&
                (VsecHeader.VsecId == PCI_EXPRESS_THUNDERBOLT_VSEC_ID)) {

                if (ExpressPort->ThunderboltCapability == 0) {
                    ExpressPort->ThunderboltCapability = CurrentCap;
                }
            }

            break;

        case PCI_EXPRESS_RESIZABLE_BAR_CAP_ID:
            PciResizableBarInitialize(Device, CurrentCap);
            break;

        case PCI_EXPRESS_VIRTUAL_CHANNEL_CAP_ID:
            // This one is so nice they named it twice.
        case PCI_EXPRESS_VC_AND_MFVC_CAP_ID:
            ExpressPort->VcCapability = CurrentCap;

            PciReadExpressVcRegister(ExpressPort, Capabilities1, &VcCap1);
            PciReadExpressVcRegister(ExpressPort, Capabilities2, &VcCap2);

            //
            // ExtededVCCount refers to all the VCs beyond the default VC0.
            // (See table 7-45 in PCIe version 3.1 dated October 8, 2014, and
            // in particular, the little asterisk at the bottom.)
            //
            VcCapSize = FIELD_OFFSET(PCI_EXPRESS_VIRTUAL_CHANNEL_CAPABILITY, 
                                     Resource) +
                        ((VcCap1.ExtendedVCCount + 1) * 
                            sizeof(VIRTUAL_RESOURCE));

            //
            // Check for a VC Arbitration Table and increase the cap size
            // accordingly.  The offset is stored in terms of 0x10-byte units.
            // It is 0x10 bytes long.
            //

            if (VcCap2.VCArbitrationTableOffset != 0) {

                VcCapTableEnd = (VcCap2.VCArbitrationTableOffset * 0x10) + 0x10;

                if (VcCapTableEnd > VcCapSize) {
                    VcCapSize = VcCapTableEnd;
                }
            }

            //
            // Now check for port arbitration tables.  Again, they're expressed
            // as offsets stored in terms of 0x10-byte units.  The size of the
            // table entries is determined by the Port Arbitration Table Entry
            // Size in Cap1.  The potential length is determined by the Port
            // Arbitration Capability field.
            //

            for (ResIndex = 0; ResIndex <= VcCap1.ExtendedVCCount; ResIndex++) {

                ULONG ArbitrationPhases;

                PciReadDeviceConfig(Device, 
                                    (PUCHAR)&VcResource,
                                    ExpressPort->VcCapability +
                                       FIELD_OFFSET(PCI_EXPRESS_VIRTUAL_CHANNEL_CAPABILITY,
                                                    Resource) +
                                       (sizeof(VcResource) * ResIndex),
                                    sizeof(VcResource));

                if (VcResource.Capability.PortArbitrationTableOffset == 0) {
                    continue;
                }

                //
                // See the definition of the Port Arbitration Capability field
                // for an explanation of this switch statement.
                //

                switch (VcResource.Capability.PortArbitrationCapability) {
                case 0: ArbitrationPhases = 0;   break;
                case 1: ArbitrationPhases = 32;  break;
                case 2: ArbitrationPhases = 64;  break;
                case 3: ArbitrationPhases = 128; break;
                case 4: ArbitrationPhases = 128; break;
                case 5: ArbitrationPhases = 256; break;
                default:
                    PCI_ASSERTMSG("Value not allowed in PCIe 3.0", FALSE);
                    ArbitrationPhases = 0;
                    break;
                }

                VcCapTableEnd = 
                    (VcResource.Capability.PortArbitrationTableOffset * 0x10) +
                    ((1 << VcCap1.PortArbitrationTableEntrySize) * ArbitrationPhases / 8);

                if (VcCapTableEnd > VcCapSize) {
                    VcCapSize = VcCapTableEnd;
                }
            }

            //
            // Allocate enough memory to save the entire VC capability,
            // minus the read-only header.
            //

            ExpressPort->VcSaveSize = VcCapSize -
                FIELD_OFFSET(PCI_EXPRESS_VIRTUAL_CHANNEL_CAPABILITY, Control);

            ExpressPort->VcSaveArea = 
                PciAllocatePool(NonPagedPoolNx, ExpressPort->VcSaveSize);

            if (ExpressPort->VcSaveArea == NULL) {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            RtlZeroMemory(ExpressPort->VcSaveArea, ExpressPort->VcSaveSize);
            break; 

        default:
            break;
        }

        CurrentCap = Header.Next;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
ExpressCompatibilitySaveExtendedCapabilities (
    __inout PEXPRESS_COMPATIBILITY_PORT CompatibilityPort
    )

/*++
Routine Description:

    This routine processes the capability list that lives in the extended
        configuration space.

Arguments:

    CompatibilityPort - Supplies placeholder which describes which device
        to query and provides place to store result of query.

Return Value:

    Returns STATUS_SUCCESS on success.
    Returns STATUS_DEVICE_DATA_ERROR on failure.

--*/

{

    USHORT CurrentCap;
    PPCI_DEVICE Device;
    PCI_EXPRESS_ENHANCED_CAPABILITY_HEADER Header;
    PCI_EXPRESS_VENDOR_SPECIFIC_CAPABILITY VsecHeader;

    PAGED_CODE();

    PCI_ASSERT(CompatibilityPort->Device->ExtendedConfigAvailable != FALSE);

    //
    // Extended Capabilities in device configuration space always begin at
    // offset 100h with a PCI Express Enhanced Capability header.
    //

    CurrentCap = 0x100;
    while (CurrentCap != 0) {
        if (CurrentCap < 0x100) {

            VERIFIER("Extended capability pointer in the non-extended space");

            return STATUS_DEVICE_DATA_ERROR;
        }

        if ((CurrentCap & 0x3) != 0) {

            VERIFIER("Extended capability pointer not DWORD aligned");

            return STATUS_DEVICE_DATA_ERROR;
        }

        if (CurrentCap >
            (PCI_EXTENDED_CONFIG_LENGTH -
             sizeof(PCI_EXPRESS_ENHANCED_CAPABILITY_HEADER))) {

            VERIFIER("Extended capability pointer extends "
                     "beyond end of config space");

            return STATUS_DEVICE_DATA_ERROR;
        }

        Device = CompatibilityPort->Device;
        PciReadDeviceConfig(Device,
                            &Header,
                            CurrentCap,
                            sizeof(PCI_EXPRESS_ENHANCED_CAPABILITY_HEADER));

        switch(Header.CapabilityID) {
        case PCI_EXPRESS_INVALID_ENHANCED_CAP_ID:
            if (CurrentCap == 0x100) {

                //
                // If a premier enhanced capability header returns 0xFFFF,
                // then the express device does not have such capability.
                // Bail out with success.
                //

                return STATUS_SUCCESS;

            } else {

                //
                // An enhanced capability header returned 0xFFFF. Bail out with
                // error.
                //

                return STATUS_DEVICE_DATA_ERROR;
            }

            break;

        case PCI_EXPRESS_ADVANCED_ERROR_REPORTING_CAP_ID:
            CompatibilityPort->AerCapability = CurrentCap;
            break;

        case PCI_EXPRESS_VENDOR_SPECIFIC_CAP_ID:

            //
            // Read the capability ID and parse it if recognized.
            //

            PciReadDeviceConfig(Device,
                                &VsecHeader,
                                CurrentCap,
                                sizeof(PCI_EXPRESS_VENDOR_SPECIFIC_CAPABILITY));

            //
            // Look at the Vsec ID to determine if this is a thunderbolt
            // controller. This is the only way to recognize this controller
            // until a standard capability ID is introduced in the PCI SIG.
            // Currently only Intel specific IDs are recognized.
            //

            if ((Device->VendorID == PCI_VENDOR_INTEL) &&
                (VsecHeader.VsecId == PCI_EXPRESS_THUNDERBOLT_VSEC_ID)) {

                if (CompatibilityPort->ThunderboltCapability == 0) {
                    CompatibilityPort->ThunderboltCapability = CurrentCap;
                }
            }

            break;

        case PCI_EXPRESS_RESIZABLE_BAR_CAP_ID:
            PciResizableBarInitialize(Device, CurrentCap);
            break;

        default:
            break;
        }

        CurrentCap = Header.Next;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
ExpressProcessAerCapability (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine processes the PCI Express device's AER capabilities.  Here, we
    configure the AER settings for the device.

Arguments:

    ExpressPort - Supplies the Express device whose AER capability will
        be processed.

Return Value:

    STATUS_SUCCESS if the device's AER capabilities are successfully processed.

    STATUS_UNSUCCESSFUL if the AER capabilities cannot be processed.

--*/

{

    PPCI_EXPRESS_AER_CAPABILITY AerCaps;
    PWHEA_ERROR_SOURCE_DESCRIPTOR ErrorSource;

    PAGED_CODE();

    ErrorSource = &ExpressPort->ErrorSource;

    //
    // Clear uncorrectable error status register.
    //

    AerCaps = &ExpressPort->AerCapabilityBlock;
    AerCaps->UncorrectableErrorStatus.AsULONG = 0xffffffff;
    PciWriteExpressAerRegister(
        ExpressPort,
        UncorrectableErrorStatus,
        &AerCaps->UncorrectableErrorStatus.AsULONG);

    //
    // Initialize the uncorrectable error mask register.
    //

    AerCaps->UncorrectableErrorMask.AsULONG =
        ErrorSource->Info.AerEndpointDescriptor.UncorrectableErrorMask;

    PciWriteExpressAerRegister(
        ExpressPort,
        UncorrectableErrorMask,
        &AerCaps->UncorrectableErrorMask.AsULONG);

    //
    // Initialize the uncorrectable error severity bits alone.
    //

    AerCaps->UncorrectableErrorSeverity.AsULONG =
        ErrorSource->Info.AerEndpointDescriptor.UncorrectableErrorSeverity;

    PciWriteExpressAerRegister(
        ExpressPort,
        UncorrectableErrorSeverity,
        &AerCaps->UncorrectableErrorSeverity.AsULONG);

    //
    // Clear correctable error status register.
    //

    AerCaps->CorrectableErrorStatus.AsULONG = 0xffffffff;
    PciWriteExpressAerRegister(
        ExpressPort,
        CorrectableErrorStatus,
        &AerCaps->CorrectableErrorStatus.AsULONG);

    //
    // Clear correctable error mask register.
    //

    AerCaps->CorrectableErrorMask.AsULONG =
        ErrorSource->Info.AerEndpointDescriptor.CorrectableErrorMask;

    PciWriteExpressAerRegister(
        ExpressPort,
        CorrectableErrorMask,
        &AerCaps->CorrectableErrorMask.AsULONG);

    //
    // Leave the Advanced Error Capabilities and Control Register alone.
    //

    AerCaps->CapabilitiesAndControl.AsULONG =
        ErrorSource->Info.AerEndpointDescriptor.AdvancedCapsAndControl;

    PciWriteExpressAerRegister(
        ExpressPort,
        CapabilitiesAndControl,
        &AerCaps->CapabilitiesAndControl.AsULONG);

    //
    // Read the AER capability information for the device and cache the the
    // information in the EXPRESS_PORT's AerCapabilityBlock.
    //

    PciReadExpressAerCapabilities(ExpressPort, AerCaps);

    //
    // Configure the device to route error messages to the root complex.
    //

    ExpressPortInitializeAERSupport(ExpressPort);
    return STATUS_SUCCESS;
}

VOID
ExpressPortInitializeAERSupport (
    __in PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine enables the supplied PCI Express device to generate AER
    error messages.

Arguments:

    ExpressPort - Supplies an Express device whose error reporting registers
        are to be enabled.

Return Value:

    None.

--*/

{

    USHORT DeviceStatus;

    PAGED_CODE();

    //
    // The DeviceControl register is cached by the driver and initialized
    // when the device is started.  Rather than writing to hardware now,
    // just set the cached value as appropriate and it will be written to
    // hardware later when the device is started. Set the AER bits in the
    // DeviceControl register using either default values or values supplied
    // by the platform.
    //

    ExpressPort->DeviceControl.AsUSHORT &= ~PCI_EXPRESS_AER_DEVICE_CONTROL_MASK;
    ExpressPort->DeviceControl.AsUSHORT |=
        ExpressPort->ErrorSource.Info.AerEndpointDescriptor.DeviceControl;

    //
    // Clear error status bits from the Express Capability Structure's
    // Device Status Register.
    //

    DeviceStatus = PCI_EXPRESS_AER_DEVICE_STATUS_MASK;
    PciWriteExpressRegister(ExpressPort,
                            DeviceStatus,
                            &DeviceStatus);

    return;
}

NTSTATUS
ExpressProcessSerialNumberCapability (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine retrieves the serial number for the Express device.

Arguments:

    ExpressPort - Supplies the Express device whose serial number will
        be retrieved.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PCI_EXPRESS_SERIAL_NUMBER_CAPABILITY SerialNumber;

    PAGED_CODE();

    PCI_ASSERT(ExpressPort->SerialNumberCapability != 0);

    PciReadDeviceConfig(ExpressPort->Device,
                        &SerialNumber,
                        ExpressPort->SerialNumberCapability,
                        sizeof(PCI_EXPRESS_SERIAL_NUMBER_CAPABILITY));

    ExpressPort->SerialNumber = SerialNumber.HighSerialNumber;
    ExpressPort->SerialNumber <<= 32;
    ExpressPort->SerialNumber |= SerialNumber.LowSerialNumber;

    //
    // If a serial number for this device hasn't been seen before
    // and a cached value wasn't found in the registry, update the
    // value to be used for Instance ID generation here.  This
    // will subsequently be persisted in the registry for future
    // use.
    //
    if (ExpressPort->Device->InstanceIdSerialNumber == 0){
        ExpressPort->Device->InstanceIdSerialNumber = ExpressPort->SerialNumber;
    }

    return STATUS_SUCCESS;
}

VOID
ExpressProcessAriCapability (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine processes the PCI Express device's ARI capabilities.

Arguments:

    ExpressPort - Supplies the Express device whose ARI capability will
        be processed.

Return Value:

    None.

--*/

{

    PCI_EXPRESS_ARI_CAPABILITY Ari;

    PAGED_CODE();

    PCI_ASSERT(ExpressPort->AriCapability != 0);

    //
    // This capability only happens if the bridge that got us
    // here is ARI-capable.  If not, there's not even any reason
    // to read the capability, we're just done.
    //

    if ((ExpressPort->Parent == NULL) ||
        (ExpressPort->Parent->AriForwardingSupported == FALSE)) {

        return;
    }

    PciReadDeviceConfig(ExpressPort->Device,
                        &Ari,
                        ExpressPort->AriCapability,
                        sizeof(PCI_EXPRESS_ARI_CAPABILITY));

    //
    // Ok, we're good to go.  Enable ARI in the bridge, mark the
    // device, and remember the next function number.
    //

    if (ExpressBridgeSetAriEnable(ExpressPort->Parent, TRUE) == FALSE) {
        return;
    }

    PCI_DEVICE_SET_FLAG(ExpressPort->Device, AriIsEnabled);
    ExpressPort->NextAriFunctionNumber = Ari.Capability.NextFunctionNumber;
    return;
}

NTSTATUS
ExpressProcessSriovCapability (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine processes the PCI Express device's SR-IOV capabilities.

Arguments:

    ExpressPort - Supplies the Express device whose SR-IOV capability will
        be processed.

Return Value:

    STATUS_SUCCESS

--*/

{

    PCI_EXPRESS_SRIOV_CONTROL Control;
    USHORT InitialVFs;
    USHORT NumVFs;
    USHORT TotalVFs;

    PAGED_CODE();

    PCI_ASSERT(ExpressPort->SriovCapability != 0);

    //
    // Enable the ARI on the device If ARI was enabled on the path to
    // this device
    //
    if (ExpressPort->Device->AriIsEnabled != FALSE) {
        PciReadSriovRegister(ExpressPort, SRIOVControl, &Control);
        Control.ARICapableHierarchy = TRUE;
        PciWriteSriovRegister(ExpressPort, SRIOVControl, &Control);
    }

    //
    // Setup the initial, total and current  nubmber of Virtual Functions
    //
    PciReadSriovRegister(ExpressPort, InitialVFs, &InitialVFs);
    PciReadSriovRegister(ExpressPort, TotalVFs, &TotalVFs);

    NumVFs = InitialVFs;
    PciWriteSriovRegister(ExpressPort, NumVFs, &NumVFs);

    ExpressPort->NumVFs = NumVFs;
    ExpressPort->InitialVFs = InitialVFs;
    ExpressPort->TotalVFs = TotalVFs;

    //
    // Read the VF offset and stride
    //
    PciReadSriovRegister(ExpressPort,
                         FirstVFOffset,
                         &ExpressPort->FirstVFOffset);

    PciReadSriovRegister(ExpressPort,
                         VFStride,
                         &ExpressPort->VFStride);

    return STATUS_SUCCESS;
}

BOOLEAN
ExpressVerifyAcsCapability(
    _In_ PCI_EXPRESS_DEVICE_TYPE                DeviceType,
    _In_ PCI_EXPRESS_ACS_CAPABILITY_REGISTER    AcsCapabilities,
    _In_ USHORT                                 AtsCapability
)

/*++

Routine Description:

   This routine verifies that the ACS (Access Control Services - see PCIe 3.0)
   Capability register conforms to the PCI Express standard for certain device
   types. If the verification fails, this routine returns false.

Arguments:

    DeviceType - Supplies the type of device being verified. If it's not a
        downstream port, this routine assumes the device is a multi function
        device.

    AcsCapabilities - Supplies the Acs Capability Register read from the given
        device.

    AtsCapability - Location of the ATS capability struct in config to space to
        check if the device implements ATS.

Return Value:

    Boolean stating if Acs Capability is valid.

--*/

{

    PAGED_CODE();

    // NOTE: Via Pci Express Spec r3.0

    if (DeviceType == PciExpressRootPort){
        // Root ports must implement:
        //      ACS Source Validation       (bit 0)
        //      ACS Translation Blocking    (bit 1)

        // Root ports may implement:
        //      ACS P2P Request Redirect    (bit 2)
        //      ACS P2P Completion Redirect (bit 3)
        //      ACS Upstream Forwarding     (bit 4)
        //      ACS P2P Egress Control      (bit 5)
        //      ACS Direct Translated P2P   (bit 6) (ATS)

        // Verify mandatory properties
        if (!(AcsCapabilities.SourceValidation &&
            AcsCapabilities.TranslationBlocking)){
            return FALSE;
        }

        // Verify that if P2P Request Redirect are supported,
        // P2P Completion Redirect is also supported
        if (AcsCapabilities.RequestRedirect){
            if (!AcsCapabilities.CompletionRedirect){
                return FALSE;
            }
        }

        // If ATS is supported on the device, verify
        // it implements Direct Translation
        if (AtsCapability != 0 &&
            !AcsCapabilities.DirectTranslation) {
            return FALSE;
        }

    } else if (DeviceType == PciExpressDownstreamSwitchPort) {
        // Downstream Switch Ports must implement:
        //      ACS Source Validation       (bit 0)
        //      ACS Translation Blocking    (bit 1)
        //      ACS P2P Request Redirect    (bit 2)
        //      ACS P2P Completion Redirect (bit 3)
        //      ACS Upstream Forwarding     (bit 4)
        //      ACS Direct Translated P2P   (bit 6)

        // Downstream Switch Ports may implement:
        //      ACS P2P Egress Control      (bit 5)

        if (!(AcsCapabilities.SourceValidation &&
            AcsCapabilities.TranslationBlocking &&
            AcsCapabilities.RequestRedirect &&
            AcsCapabilities.CompletionRedirect &&
            AcsCapabilities.UpstreamForwarding &&
            AcsCapabilities.DirectTranslation)){
            return FALSE;
        }

    } else {
        // Other Pci Express device types (Multifunction devices)
        // must NOT implement:
        //      ACS Source Validation       (bit 0)
        //      ACS Translation Blocking    (bit 1)
        //      ACS Upstream Forwarding     (bit 4)

        // Devices that support P2P must implement:
        //      ACS P2P Request Redirect    (bit 2)
        //      ACS P2P Completion Redirect (bit 3)
        //      ACS Direct Translated P2P   (bit 6) (ATS & P2P)

        // Devices may implement:
        //      ACS P2P Egress Control      (bit 5)

        // Verify it doesn't implement the required things
        if (AcsCapabilities.SourceValidation ||
            AcsCapabilities.TranslationBlocking ||
            AcsCapabilities.UpstreamForwarding){
            return FALSE;
        }

        // Assume that for now, all multifunction devices support P2P
        if (!(AcsCapabilities.RequestRedirect &&
            AcsCapabilities.CompletionRedirect)){
            return FALSE;
        }

        // If ATS is supported on the device, verify
        // it implements Direct Translation
        if (AtsCapability != 0 &&
            !AcsCapabilities.DirectTranslation) {
            return FALSE;
        }

    }

    return TRUE;
}

ULONG
ExpressCheckAcsHierarchyCompatibility(
    _In_ PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine checks support for ACS of the passed in device, and
    ACS support up the hierarchy.

Arguments:

    ExpressPort - Supplies the Express device whose ACS compatibility will
        be verified.

Return Value:

    Value identifying the level of ACS support in the hierarchy.

--*/

{
    PEXPRESS_BRIDGE ParentExpressBridge;
    PPCI_DEVICE PciDevice;
    BOOLEAN AcsValid;
    ULONG AcsCompatability;
    PCI_EXPRESS_ACS_CAPABILITY_REGISTER AcsCapabilities;

    PAGED_CODE();

    //
    // Default to ACS being not supported
    //

    AcsCompatability = DevProp_PciDevice_AcsCompatibleUpHierarchy_NotSupported;
    AcsValid = FALSE;

    //
    // Check the current device type to see if it's a root port
    //

    if (ExpressPort->DeviceType == PciExpressRootPort) {

        AcsCapabilities.AsUSHORT = 0;
        if (ExpressPort->AcsCapability){
            PciReadAcsRegister(ExpressPort, Capability, &AcsCapabilities);
        }

        if (ExpressVerifyAcsCapability(ExpressPort->DeviceType,
            AcsCapabilities,
            ExpressPort->AtsCapability)) {
            AcsCompatability = DevProp_PciDevice_AcsCompatibleUpHierarchy_Supported;
        }

        return AcsCompatability;
    }

    //
    // Check all the way up to the Root Port to verify every device along the way
    // supports ACS.
    //
    for (ParentExpressBridge = ExpressPort->Parent;
        ParentExpressBridge != NULL;
        ParentExpressBridge = ParentExpressBridge->Parent) {

        if (ParentExpressBridge->AcsCapability){
            PciReadExpressBridgeAcsRegister(ParentExpressBridge, Capability, &AcsCapabilities);
            AcsValid = ExpressVerifyAcsCapability(ParentExpressBridge->BridgeType,
                AcsCapabilities,
                ParentExpressBridge->AtsCapability);
        }
        else {
            AcsValid = FALSE;
        }

        switch (ParentExpressBridge->BridgeType){

        //
        // Root Ports that support peer-to-peer must implement ACS,
        // but Root Ports that do not support peer-to-peer must indicate
        // that ACS is unneeded.
        //
        case PciExpressRootPort:
            if (!AcsValid &&
                ParentExpressBridge->AcsNotNeeded == FALSE) {
                return AcsCompatability;
            }
            break;

        //
        // Switch downstream ports must implement ACS.
        //
        case PciExpressDownstreamSwitchPort:
            if (!AcsValid){
                return AcsCompatability;
            }
            break;

        //
        // Upstream ports do not have to implement ACS.
        //
        case PciExpressUpstreamSwitchPort:
            break;

        //
        // All other bridge types are not supported with ACS.
        //
        default:
            return AcsCompatability;
        }

    }

    PciDevice = ExpressPort->Device;

    //
    // Check if the current device is single function or multi function
    //

    if (PciDevice->Sibling == NULL &&
        PciDevice->Parent->ChildDevices == PciDevice){

        //
        // This device is a single function device, it should not have implemented an ACS struct
        //

        if (ExpressPort->AcsCapability == 0) {
            AcsCompatability = DevProp_PciDevice_AcsCompatibleUpHierarchy_SingleFunctionSupported;
        }

    } else {

        //
        // This device is a multi function device, verify the config register is correct
        //
        AcsCapabilities.AsUSHORT = 0;
        if (ExpressPort->AcsCapability) {
            PciReadAcsRegister(ExpressPort, Capability, &AcsCapabilities);
        }

        if (ExpressVerifyAcsCapability(ExpressPort->DeviceType,
            AcsCapabilities,
            ExpressPort->AtsCapability)){
            AcsCompatability = DevProp_PciDevice_AcsCompatibleUpHierarchy_Supported;
        }
    }

    return AcsCompatability;
}

VOID
ExpressAddIntegratedPortToRootComplex (
    __inout PROOT_COMPLEX RootComplex,
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine is invoked for RC Integrated Endpoints and RC Event
    Collectors. This routine links the ExpressPort representing the
    above mentioned logical devices to the root complex.

Arguments:

    RootComplex - Supplies the root complex that originates the PCI
        Express hierarchies.

    ExpressPort - Supplies the placeholder for an Express specific
        portions of the device.

Return Value:

    None.

--*/

{

    PEXPRESS_PORT *Link;

    PAGED_CODE();

    PciAcquirePassiveLock(&RootComplex->ListLock);
    switch (ExpressPort->DeviceType) {
    case PciExpressRootComplexIntegratedEndpoint:
        Link = &RootComplex->IntegratedEndpointList;
        break;

    case PciExpressRootComplexEventCollector:
        Link = &RootComplex->EventCollectorList;
        break;

    default:

        PCI_ASSERT(FALSE);

        PciReleasePassiveLock(&RootComplex->ListLock);
        return;
    }

    ExpressPort->Sibling = *Link;
    *Link = ExpressPort;
    PciReleasePassiveLock(&RootComplex->ListLock);
    return;
}

VOID
ExpressRemoveIntegratedPortFromRootComplex (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine is invoked for RC Integrated Endpoints and RC Event
    Collectors. This routine unlinks the ExpressPort representing the
    above mentioned logical devices from the root complex.

    This routine is invoked with the PciGlobalLock held.

Arguments:

    ExpressPort - Supplies the placeholder for an Express specific
        portions of the device.

Return Value:

    None.

--*/

{

    PEXPRESS_PORT Current;
    PEXPRESS_PORT *Head;
    PEXPRESS_PORT Previous;
    PROOT_COMPLEX RootComplex;

    PAGED_CODE();

    RootComplex = ExpressPort->RootComplex;

    PCI_ASSERT(RootComplex != NULL);

    PciAcquirePassiveLock(&RootComplex->ListLock);
    switch (ExpressPort->DeviceType) {
    case PciExpressRootComplexIntegratedEndpoint:
        Head = &RootComplex->IntegratedEndpointList;
        break;

    case PciExpressRootComplexEventCollector:
        Head = &RootComplex->EventCollectorList;
        break;

    default:

        PCI_ASSERT(FALSE);

        PciReleasePassiveLock(&RootComplex->ListLock);
        return;
    }

    if (*Head != NULL) {
        if (*Head == ExpressPort) {
            *Head = ExpressPort->Sibling;

        } else {
            Current = (*Head)->Sibling;
            Previous = *Head;
            while (Current != NULL) {
                if (Current == ExpressPort) {
                    Previous->Sibling = ExpressPort->Sibling;
                    break;
                }

                Previous = Current;
                Current = Current->Sibling;
            }
        }
    }

    PciReleasePassiveLock(&RootComplex->ListLock);
    return;
}

_IRQL_requires_max_(APC_LEVEL)
VOID
ExpressWaitForPendingTransactions (
    _In_ PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine waits for any pending PCI Express transactions for this
    device to complete, making sure that it is completely off of the bus.
    This should be done before reprogramming the device.

    Note that the Stop/Remove/Surprise Remove/Power Down IRPs are non failable
    IRPs, so if the transactions don't drain, there is no action that PCI can
    take to prevent the reprogramming of this device. As a result, this routine
    doesn't return a status.

    This routine must not be pageable, as it will be called as part of a power-
    down sequence, potentially after the paging device is no longer functioning.

Arguments:

    ExpressPort - Supplies the PCI Express port to drain.

Return Value:

    None.

--*/

{

    ULONG Pass;

    Pass = 1;
    while ((ExpressCheckForPendingTransactions(ExpressPort)) && (Pass < 10)) {
        PciDevice_Delay(ExpressPort->Device, ExpressTransactionTime);
        Pass++;
    }

    return;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
BOOLEAN
ExpressCheckForPendingTransactions (
    _In_ PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine checks for any pending PCI Express transactions for this
    device to complete, making sure that it is completely off of the bus.
    This should be done before reprogramming the device.

Arguments:

    ExpressPort - Supplies the PCI Express port to drain.

Return Value:

    TRUE - Transactions are still pending
    FALSE - Not so much

--*/
{

    PCI_EXPRESS_DEVICE_STATUS_REGISTER DeviceStatusRegister;

    NON_PAGED_CODE();

    PciReadExpressRegister(ExpressPort, DeviceStatus, &DeviceStatusRegister);
    return DeviceStatusRegister.TransactionsPending != 0;
}

NTSTATUS
ExpressSavePortRegisters (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine reads and saves the registers associated with an Express
    device. The specific registers that are saved are controlled by the
    PDO.

Arguments:

    ExpressPort - Supplies EXPRESS_PORT that represents an Express portion
        of the device.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PCI_EXPRESS_ACS_CONTROL AcsControlTemp;
    PCI_EXPRESS_LTR_CAPABILITY CapabilityTemp;
    PPCI_DEVICE Device;
    PCI_EXPRESS_DEVICE_CONTROL_REGISTER DeviceControlTemp;
    PCI_EXPRESS_DEVICE_CONTROL_2_REGISTER DeviceControl2Temp;
    PCI_DEVICE_ID_DATA IdData;
    PCI_EXPRESS_L1_PM_SS_CAPABILITY L1PmSsCapabilityTemp;
    PCI_EXPRESS_LINK_CONTROL_REGISTER LinkControlTemp;
    PVOID VcSaveAreaTemp;

    VcSaveAreaTemp = NULL;
    Device = ExpressPort->Device;
    PciReadExpressRegister(ExpressPort,
                           DeviceControl,
                           &DeviceControlTemp);

    LinkControlTemp.AsUSHORT = 0;
    if (EXPRESS_INGRESS_LINK_PORT(ExpressPort)) {

        PCI_ASSERT(ExpressPort->ExpressLink != NULL);

        PciReadExpressRegister(ExpressPort,
                               LinkControl,
                               &LinkControlTemp);

    }

    //
    // N.B. The Device Control 2 register and TPH/LTR capabilities were
    //  previously cached in ExpressProcessPortRegisters and
    //  ExpressProcessExtendedPortCapabilities, respectively. This was
    //  necessary because when the system first powers up the device it
    //  "restores" the previous values, so the saved values need to be
    //  initially valid. If no other code on the system touches these
    //  values, there is no need to save them in this function. Consider
    //  removing this saving code to improve performance in the future.
    //

    DeviceControl2Temp.AsUSHORT = 0;
    if (ExpressPort->ExpressCapabilities.CapabilityVersion >= 2) {

        //
        // Save the Device Control 2 Register. This is restored
        // on PnP/Power on events.
        //

        PciReadExpressRegister(ExpressPort,
                               DeviceControl2,
                               &DeviceControl2Temp);
    }

    //
    // Save the L1PmSs capability if present.
    //

    RtlZeroMemory(&L1PmSsCapabilityTemp,
                  sizeof(PCI_EXPRESS_L1_PM_SS_CAPABILITY));

    if (ExpressPort->L1PmSsCapability != 0) {

        //
        // Save everything in the L1PmSs Capability block after the Control1
        // register.
        //

        PciReadDeviceConfig(
            ExpressPort->Device,
            &L1PmSsCapabilityTemp.L1PmSsControl1,
            ExpressPort->L1PmSsCapability +
                FIELD_OFFSET(PCI_EXPRESS_L1_PM_SS_CAPABILITY,L1PmSsControl1),
            sizeof(PCI_EXPRESS_L1_PM_SS_CAPABILITY) -
                FIELD_OFFSET(PCI_EXPRESS_L1_PM_SS_CAPABILITY,L1PmSsControl1));
    }

    //
    // Save the LTR capability if present.
    //

    RtlZeroMemory(&CapabilityTemp, sizeof(PCI_EXPRESS_LTR_CAPABILITY));
    if ((EXPRESS_INGRESS_LINK_PORT(ExpressPort) ||
         EXPRESS_RC_INTEGRATED_DEVICE(ExpressPort)) &&
        (ExpressPort->LtrCapability != 0)) {

        ExpressSaveLtrCapability(ExpressPort, &CapabilityTemp, FALSE);
    }

    //
    // Save the VC capability, if present.
    //

    if (ExpressPort->VcCapability != 0) {

        PCI_ASSERT(ExpressPort->VcSaveArea != NULL);

        VcSaveAreaTemp = _alloca(ExpressPort->VcSaveSize);
        PciReadDeviceConfig(ExpressPort->Device,
                            VcSaveAreaTemp,
                            ExpressPort->VcCapability + 
                               FIELD_OFFSET(PCI_EXPRESS_VIRTUAL_CHANNEL_CAPABILITY,
                                            Control),
                            ExpressPort->VcSaveSize);
    }

    //
    // Save the ACS control register, if present
    //

    AcsControlTemp.AsUSHORT = 0;
    if (ExpressPort->AcsCapability != 0) {
        PciReadAcsRegister(ExpressPort, Control, &AcsControlTemp);
    }

    //
    // There exists a race condition where a device is hot unplugged
    // concurrently with an S3/S4 transition. In this case, it's
    // possible to read all registers as 0xffs (since the device is
    // gone). When the device is re-added before resume, those 0xffs
    // would be restored to the device. Only update the saved registers
    // if the same device is in place.
    //

    if (NT_SUCCESS(PciReadDeviceIdData(Device, &IdData)) &&
        PciIsSameDevice(Device, &IdData)) {

        ExpressPort->DeviceControl = DeviceControlTemp;
        if (EXPRESS_INGRESS_LINK_PORT(ExpressPort)) {
            ExpressPort->ExpressLink->LinkControl = LinkControlTemp;
        }

        if (ExpressPort->ExpressCapabilities.CapabilityVersion >= 2) {
            ExpressPort->DeviceControl2 = DeviceControl2Temp;
        }

        //
        // Save the TPH Requester capability if present.
        //
        // Note:  TPH enabled devices are not expected to
        //        encounter the eject/hibernate race condition,
        //        therefore, it is possible that the saved data 
        //        may be flagged as invalid and the capabilitity's 
        //        boot configuration may be lost.
        //
        //        Telemetry has been added to ExpressSaveTphRequesterCapability
        //        to report when the config data would be lost.  Post
        //        Redstone, Windows TPH support will be enhanced, and
        //        the save/restore path will be hardened against this
        //        scenario.
        //

        if (ExpressPort->TphRequesterCapability != 0) {
            ExpressSaveTphRequesterCapability(ExpressPort);
        }

        if (ExpressPort->L1PmSsCapability != 0) {
            ExpressPort->L1PmSsCapabilitySaved = TRUE;
            RtlCopyMemory(&ExpressPort->L1PmSsCapabilityBlock.L1PmSsControl1,
                          &L1PmSsCapabilityTemp.L1PmSsControl1,
                          (sizeof(PCI_EXPRESS_L1_PM_SS_CAPABILITY) -
                           FIELD_OFFSET(PCI_EXPRESS_L1_PM_SS_CAPABILITY,
                                        L1PmSsControl1)));
        }

        if ((EXPRESS_INGRESS_LINK_PORT(ExpressPort) ||
             EXPRESS_RC_INTEGRATED_DEVICE(ExpressPort)) &&
            (ExpressPort->LtrCapability != 0)) {
            ExpressSaveLtrCapability(ExpressPort, &CapabilityTemp, TRUE);
        }

        if (ExpressPort->VcCapability != 0) {
            RtlCopyMemory(ExpressPort->VcSaveArea,
                          VcSaveAreaTemp,
                          ExpressPort->VcSaveSize);
        }

        if (ExpressPort->AcsCapability != 0) {
            ExpressPort->AcsControl = AcsControlTemp;
        }

        InterlockedIncrement(&PciDebugExpressNativeSaveDone);

    } else {
        InterlockedIncrement(&PciDebugExpressNativeSaveNotDone);
        TraceLoggingWrite(PciTraceLoggingProvider,
                          "PciDebugExpressNativeSave",
                          TraceLoggingInt32(PciDebugExpressNativeSaveNotDone, "NotDone"),
                          TraceLoggingInt32(PciDebugExpressNativeSaveDone, "Done"),
                          TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES));
    }

    return STATUS_SUCCESS;
}

NTSTATUS
ExpressCompatibilitySavePortRegisters (
    __inout PEXPRESS_COMPATIBILITY_PORT CompatibilityPort
    )

/*++

Routine Description:

    This routine reads and saves the registers associated with an Express
    device that is operating in compatibility mode.

Arguments:

    CompatibilityPort - Supplies compatibility port where registers will be
        saved.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PPCI_DEVICE Device;
    PCI_EXPRESS_DEVICE_CONTROL_REGISTER DeviceControlTemp;
    PCI_DEVICE_ID_DATA IdData;
    PCI_EXPRESS_LINK_CONTROL_REGISTER LinkControlTemp;

    Device = CompatibilityPort->Device;
    PciReadExpressRegisterEx(Device,
                             DeviceControl,
                             &DeviceControlTemp);

    LinkControlTemp.AsUSHORT = 0;
    if (EXPRESS_COMPATIBILITY_INGRESS_LINK_PORT(CompatibilityPort)) {
        PciReadExpressRegisterEx(Device,
                                 LinkControl,
                                 &LinkControlTemp);
    }

    //
    // There exists a race condition where a device is hot unplugged
    // concurrently with an S3/S4 transition. In this case, it's
    // possible to read all registers as 0xffs (since the device is
    // gone). When the device is re-added before resume, those 0xffs
    // would be restored to the device. Only update the saved registers
    // if the same device is in place.
    //

    if (NT_SUCCESS(PciReadDeviceIdData(Device, &IdData)) &&
        PciIsSameDevice(Device, &IdData)) {

        CompatibilityPort->DeviceControl = DeviceControlTemp;
        if (EXPRESS_COMPATIBILITY_INGRESS_LINK_PORT(CompatibilityPort)) {
            CompatibilityPort->LinkControl = LinkControlTemp;
        }

        InterlockedIncrement(&PciDebugExpressCompatibilitySaveDone);

    } else {
        InterlockedIncrement(&PciDebugExpressCompatibilitySaveNotDone);
        TraceLoggingWrite(PciTraceLoggingProvider,
                          "PciDebugExpressCompatibilitySave",
                          TraceLoggingInt32(PciDebugExpressCompatibilitySaveNotDone, "NotDone"),
                          TraceLoggingInt32(PciDebugExpressCompatibilitySaveDone, "Done"),
                          TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES));
    }

    return STATUS_SUCCESS;
}

NTSTATUS
ExpressRestorePortRegisters (
    __in PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine restores the registers associated with an Express device.
    The specific registers that will be restored in here are in the realms
    of the PDO.

Arguments:

    ExpressPort - Supplies EXPRESS_PORT that represents an Express portion
        of the device.

Return Value:

    STATUS_SUCCESS.

--*/

{
    PciWriteExpressRegister(ExpressPort,
                            DeviceControl,
                            &ExpressPort->DeviceControl);

    if (EXPRESS_INGRESS_LINK_PORT(ExpressPort)) {

        PCI_ASSERT(ExpressPort->ExpressLink != NULL);

        PciWriteExpressRegister(ExpressPort,
                                LinkControl,
                                &ExpressPort->ExpressLink->LinkControl);

    }

    if ((EXPRESS_INGRESS_LINK_PORT(ExpressPort) ||
         EXPRESS_RC_INTEGRATED_DEVICE(ExpressPort)) &&
        (ExpressPort->LtrCapability != 0)) {
        ExpressRestoreLtrCapability(ExpressPort);
    }

    if ((ExpressPort->L1PmSsCapability != 0) &&
        (ExpressPort->L1PmSsCapabilitySaved != FALSE)) {

        //
        // Restore everything in the L1PmSs Capability block after the Control1
        // register.
        //

        PciWriteDeviceConfig(
            ExpressPort->Device,
            &ExpressPort->L1PmSsCapabilityBlock.L1PmSsControl1,
            ExpressPort->L1PmSsCapability +
                FIELD_OFFSET(PCI_EXPRESS_L1_PM_SS_CAPABILITY,L1PmSsControl1),
            sizeof(PCI_EXPRESS_L1_PM_SS_CAPABILITY) -
                FIELD_OFFSET(PCI_EXPRESS_L1_PM_SS_CAPABILITY,L1PmSsControl1));
    }

    if (ExpressPort->ExpressCapabilities.CapabilityVersion >= 2) {

        //
        // Restore DeviceControl2 register.
        //

        PciWriteExpressRegister(ExpressPort,
                                DeviceControl2,
                                &ExpressPort->DeviceControl2);
    }

    //
    // Restore the VC capability, if present.
    //

    if (ExpressPort->VcCapability != 0) {

        PCI_ASSERT(ExpressPort->VcSaveArea != NULL);
        PciWriteDeviceConfig(ExpressPort->Device,
                             ExpressPort->VcSaveArea,
                             ExpressPort->VcCapability + 
                                FIELD_OFFSET(PCI_EXPRESS_VIRTUAL_CHANNEL_CAPABILITY,
                                             Control),
                             ExpressPort->VcSaveSize);
    }

    //
    // Restore the ACS capability, if present.
    //

    if (ExpressPort->AcsCapability != 0) {
        PciWriteAcsRegister(ExpressPort, Control, &ExpressPort->AcsControl.AsUSHORT);
    }

    //
    // Restore _HPX related registers
    //

    PciApplyHPX(ExpressPort->Device);

    return STATUS_SUCCESS;
}

NTSTATUS
ExpressCompatibilityRestorePortRegisters (
    __in PEXPRESS_COMPATIBILITY_PORT CompatibilityPort
    )

/*++

Routine Description:

    This routine restores registers to express devices which are operating
    in compatibility mode.  Currently, only max payload size is restored
    because devices operating in compatibility mode are controlled by the
    sytem BIOS.  In this case, max payload size is restored because the BIOS
    will be unable to restore non-default values.

Arguments:

    CompatibilityPort - Supplies values to restore to device.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PPCI_DEVICE Device;
    PCI_EXPRESS_DEVICE_CONTROL_REGISTER DeviceControlRegister;

    Device = CompatibilityPort->Device;
    PciReadExpressRegisterEx(Device, DeviceControl, &DeviceControlRegister);
    DeviceControlRegister.MaxPayloadSize =
        CompatibilityPort->DeviceControl.MaxPayloadSize;

    PciWriteExpressRegisterEx(Device,
                              DeviceControl,
                              &DeviceControlRegister);

    return STATUS_SUCCESS;
}

VOID
ExpressDestroyCompatibilityPort (
    __in __drv_in(__drv_freesMem(Pool)) PEXPRESS_COMPATIBILITY_PORT CompatPort
    )

/*++

Routine Description:

    This routine is invoked to free an express compatibility port and its
    associated allocation.

Arguments:

    CompatPort - Supplies a pointer to an express compatibility port.

Return Value:

    None.

--*/

{

    PPCI_DEVICE Device;

    NON_PAGED_CODE();

    PCI_ASSERT(CompatPort != NULL);

    Device = CompatPort->Device;
    if (Device != NULL) {
        Device->ExpressCompatibilityPort = NULL;
    }

    PciFreePool(CompatPort);
    return;
}

VOID
ExpressDestroyPort (
    __in __drv_in(__drv_freesMem(Pool)) PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine is invoked to free an express port and its associated
    allocation.

Arguments:

    ExpressPort - Supplies a pointer to an express port.

Return Value:

    None.

--*/

{

    PPCI_DEVICE Device;
    PEXPRESS_LINK ParentLink;

    NON_PAGED_CODE();

    PCI_ASSERT(ExpressPort != NULL);

    if (ExpressPort->ExpressLink != NULL) {
        ParentLink = ExpressPort->ExpressLink->TargetLink;
        if (ParentLink->TargetLink == ExpressPort->ExpressLink) {
            ParentLink->TargetLink = NULL;
        }

        PciFreePool(ExpressPort->ExpressLink);
    }

    Device = ExpressPort->Device;
    if (Device != NULL) {
        Device->ExpressPort = NULL;
    }

    if (ExpressPort->TphSteeringTags) {
        PciFreePool(ExpressPort->TphSteeringTags);
    }

    if (ExpressPort->VcSaveArea)
    {
        PciFreePool(ExpressPort->VcSaveArea);
    }

    PciFreePool(ExpressPort);
    return;
}

// --------------------------------------------------------- Interface Functions

//
// These macros allow accesses to express register space even if the
// device is enumerated in a conventional mode. Hence these macros
// will be undefed at the end of this section so that it is not available
// to other routines.
//

#define PciDeviceReadExpressRegister(_Device_, RegisterName, Buffer)           \
{                                                                              \
    PCI_ASSERT(_Device_->ExpressCapability != 0);                              \
    PciReadDeviceConfig(_Device_,                                              \
                        (PUCHAR)Buffer,                                        \
                        _Device_->ExpressCapability +                          \
                            FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, RegisterName),\
                        RTL_FIELD_SIZE(PCI_EXPRESS_CAPABILITY, RegisterName)   \
                        );                                                     \
}

#define PciBridgeReadExpressRegister(_Bridge_, RegisterName, Buffer)           \
{                                                                              \
    PCI_ASSERT(_Bridge_->ExpressCapability != 0);                              \
    PciReadBridgeConfig(_Bridge_,                                              \
                        (PUCHAR)Buffer,                                        \
                        _Bridge_->ExpressCapability +                          \
                            FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, RegisterName),\
                        RTL_FIELD_SIZE(PCI_EXPRESS_CAPABILITY, RegisterName)   \
                        );                                                     \
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
ExpressLinkQuiescentInterface_Constructor (
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )

/*++

Routine Description:

    This routine returns a bus interface to quiescent an express link that
    connects an express root port to a video device.

Arguments:

    DeviceExtension - Supplies a pointer to pci device.

    InterfaceSpecificData - Supplies an interface specific data.

    Version - Supplies a version number.

    Size - Supplies a maximum size for a field that will hold the interface.

    InterfaceReturn - Supplies a field for an interface to quiescent an express
        link.

    Filtered - Not supported for this interface.

Return Value:

    STATUS_UNSUCCESSFUL if the device is not an express video controller
        connected to a root port.

    STATUS_INSUFFICIENT_RESOURCES on failure to allocate resources.

    STATUS_SUCCESS on success.

--*/

{

    PPCI_DEVICE Device;
    PEXPRESS_LINK_QUIESCENT_CONTEXT LinkQuiescentContext;
    PPCI_EXPRESS_LINK_QUIESCENT_INTERFACE LinkQuiescentInterface;
    NTSTATUS Status;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Size);
    UNREFERENCED_PARAMETER(Filtered);

    __analysis_assume(Size >= sizeof(PCI_EXPRESS_LINK_QUIESCENT_INTERFACE));

    //
    // Ensure that the device is an express video controller.
    //

    Device = (PPCI_DEVICE)DeviceExtension;
    if (!PCI_VGA_CONTROLLER(Device)) {
        return STATUS_UNSUCCESSFUL;
    }

    Status = ExpressCheckDeviceOnRootPort(Device);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    LinkQuiescentInterface =
        (PPCI_EXPRESS_LINK_QUIESCENT_INTERFACE)InterfaceReturn;

    RtlZeroMemory(LinkQuiescentInterface,
                  sizeof(PCI_EXPRESS_LINK_QUIESCENT_INTERFACE));

    LinkQuiescentInterface->Size = sizeof(PCI_EXPRESS_LINK_QUIESCENT_INTERFACE);
    LinkQuiescentInterface->Version =
        PCI_EXPRESS_LINK_QUIESCENT_INTERFACE_VERSION;

    //
    // Allocate a link quiescent context. This context is passed along during
    // invocation of every routine defined in this interface. This context is
    // opaque to the caller and subject to change without any notice.
    //

    LinkQuiescentContext =
        PciAllocatePool(PagedPool, sizeof(EXPRESS_LINK_QUIESCENT_CONTEXT));

    if (LinkQuiescentContext == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(LinkQuiescentContext, sizeof(EXPRESS_LINK_QUIESCENT_CONTEXT));
    LinkQuiescentContext->Signature =
        (ULONG)EXPRESS_LINK_QUIESCENT_CONTEXT_SIGNATURE;

    LinkQuiescentContext->Device = DeviceExtension;
    LinkQuiescentInterface->Context = (PVOID)LinkQuiescentContext;
    LinkQuiescentInterface->InterfaceReference = ExpressLinkQuiescentReference;
    LinkQuiescentInterface->InterfaceDereference =
        ExpressLinkQuiescentDereference;

    LinkQuiescentInterface->PciExpressEnterLinkQuiescentMode =
        ExpressPortEnterLinkQuiescentMode;

    LinkQuiescentInterface->PciExpressExitLinkQuiescentMode =
        ExpressPortExitLinkQuiescentMode;

    return STATUS_SUCCESS;
}

NTSTATUS
ExpressPortEnterLinkQuiescentMode (
    __inout PVOID Context
    )

/*++

Routine Description:

    This routine is invoked to place the link in a quiescent mode.

Arguments:

    Context - Supplies a pointer to the context defined for this interface.

Return Value:

    STATUS_UNSUCCESSFUL if this routine is not invoked at PASSIVE level or
        is invoked recursively.

    STATUS_INVALID_PARAMETER if the format for the context has changed.

    STATUS_INVALID_DEVICE_STATE if the device is in some inconsistent PnP or
        power state.

    STATUS_SUCCESS on success or an appropriate error status returned from
        a function call in this routine.

--*/

{

    PPCI_BUS Bus;
    PPCI_DEVICE CurrentDevice;
    PPCI_DEVICE Device;
    PEXPRESS_LINK_QUIESCENT_CONTEXT LinkQuiescentContext;
    NTSTATUS Status;
    PPCI_DEVICE UnlockDevice;

    PAGED_CODE();

    if (KeGetCurrentIrql() != PASSIVE_LEVEL) {
        return STATUS_UNSUCCESSFUL;
    }

    LinkQuiescentContext = (PEXPRESS_LINK_QUIESCENT_CONTEXT)Context;

    //
    // Sanity check.
    //

    if (LinkQuiescentContext->Signature !=
        EXPRESS_LINK_QUIESCENT_CONTEXT_SIGNATURE) {

        return STATUS_INVALID_PARAMETER;
    }

    if (LinkQuiescentContext->InProgress != FALSE) {
        return STATUS_UNSUCCESSFUL;
    }

    Device = LinkQuiescentContext->Device;
    if (!PCI_DEVICE_EXTENSION(Device)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Validate PnP and power states for the device.
    //

    if (Device->DeviceState != PciStarted) {
        return STATUS_INVALID_DEVICE_STATE;
    }

    if (Device->LogicalPowerState != PowerDeviceD0) {
        return STATUS_INVALID_DEVICE_STATE;
    }

    Status = STATUS_SUCCESS;

    //
    // Stop the parent from scanning its secondary bus in response to the
    // bus relations Irp.
    //

    Bus = Device->Parent;
    KeWaitForSingleObject(&Bus->ExpressLinkQuiescentEvent,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);

    //
    // Hold the remove locks for all devices on the parent so that none of them
    // vanish away for the link quiescent interval.
    //

    PciAcquirePassiveLock(&Bus->ChildDeviceLock);
    for (CurrentDevice = Bus->ChildDevices;
         CurrentDevice != NULL;
         CurrentDevice = CurrentDevice->Sibling) {

        Status = IoAcquireRemoveLock(&CurrentDevice->RemoveLock, NULL);
        if (!NT_SUCCESS(Status)) {
            break;
        }
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

    //
    // If everything succeeded then update the progress indicator.
    //

    if (NT_SUCCESS(Status)) {
        LinkQuiescentContext->InProgress = TRUE;

    } else {

        //
        // In case of error release the remove lock that was successfully
        // acquired on each of the device.
        //

        for (UnlockDevice = Bus->ChildDevices;
             UnlockDevice != CurrentDevice;
             UnlockDevice = UnlockDevice->Sibling) {

            IoReleaseRemoveLock(&UnlockDevice->RemoveLock, NULL);
        }
    }

    return Status;
}

NTSTATUS
ExpressPortExitLinkQuiescentMode (
    __inout PVOID Context
    )

/*++

Routine Description:

    This routine exits the link quiescent mode.

Arguments:

    Context - Supplies a pointer to the context defined for this interface.

Return Value:

    STATUS_INVALID_PARAMETER if the format for the context has changed.

    STATUS_SUCCESS on success.

    Otherwise an error code returned from one of the function calls.

--*/

{

    PPCI_BUS Bus;
    PPCI_DEVICE CurrentDevice;
    PPCI_DEVICE Device;
    PEXPRESS_LINK_QUIESCENT_CONTEXT LinkQuiescentContext;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    PAGED_CODE();

    PCI_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    LinkQuiescentContext = (PEXPRESS_LINK_QUIESCENT_CONTEXT)Context;

    if (LinkQuiescentContext->Signature !=
        EXPRESS_LINK_QUIESCENT_CONTEXT_SIGNATURE) {

        return STATUS_INVALID_PARAMETER;
    }

    PCI_ASSERT(LinkQuiescentContext->InProgress != FALSE);

    Device = LinkQuiescentContext->Device;

    //
    // Restore the state of the device and release its remove lock.
    //

    Bus = Device->Parent;
    ReturnStatus = STATUS_SUCCESS;
    PciAcquirePassiveLock(&Bus->ChildDeviceLock);
    for (CurrentDevice = Bus->ChildDevices;
         CurrentDevice != NULL;
         CurrentDevice = CurrentDevice->Sibling) {

        //
        // Update the hardware. Raise IRQL and rendevous the processors to do
        // this if the device is critical to system operation and might be in
        // use while this operation is occurring. Also do this for devices
        // marked as being on the debugger path. Disabling the debugger around
        // the reprogramming operation is not sufficient to stop the debugger
        // device from being used, since devices that are marked as being on
        // the debug path are sometimes not actually debugger devices -
        // headless console redirection devices are marked in this way as well.
        //

        Status = PciDeviceCallCritical(CurrentDevice,
                                       PciUpdateHardware,
                                       (PVOID)(ULONG_PTR)TRUE,
                                       (CurrentDevice->OnDebugPath != FALSE));

        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }

        Status = PciProgramInterruptResource(CurrentDevice);
        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }

        IoReleaseRemoveLock(&CurrentDevice->RemoveLock, NULL);
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

    //
    // Unblock the parent so that it can process its bus relations Irp.
    //

    KeSetEvent(&Bus->ExpressLinkQuiescentEvent, IO_NO_INCREMENT, FALSE);
    LinkQuiescentContext->InProgress = FALSE;
    return ReturnStatus;
}

VOID
ExpressLinkQuiescentReference (
    __inout PVOID Context
    )

/*++

Routine Description:

    This routine is invoked to reference an interface context. This helps
    manage its lifetime.

Arguments:

    Context - Supplies a pointer to the context defined for this interface.

Return Value:

    None.

--*/

{

    PEXPRESS_LINK_QUIESCENT_CONTEXT LinkQuiescentContext;

    PAGED_CODE();

    LinkQuiescentContext = (PEXPRESS_LINK_QUIESCENT_CONTEXT)Context;

    PCI_ASSERT(LinkQuiescentContext->Signature ==
               EXPRESS_LINK_QUIESCENT_CONTEXT_SIGNATURE);

    InterlockedIncrement(&LinkQuiescentContext->ReferenceCount);
    return;
}

VOID
ExpressLinkQuiescentDereference (
    __inout PVOID Context
    )

/*++

Routine Description:

    This routine is invoked to de-reference an interface context. When the
    count drops to 0, the context is de-allocated.

Arguments:

    Context - Supplies a pointer to the context defined for this interface.

Return Value:

    None.

--*/

{

    PEXPRESS_LINK_QUIESCENT_CONTEXT LinkQuiescentContext;
    LONG Count;

    PAGED_CODE();

    LinkQuiescentContext = (PEXPRESS_LINK_QUIESCENT_CONTEXT)Context;

    PCI_ASSERT(LinkQuiescentContext->Signature ==
               EXPRESS_LINK_QUIESCENT_CONTEXT_SIGNATURE);

    Count = InterlockedDecrement(&LinkQuiescentContext->ReferenceCount);
    if (Count == 0) {
        PciFreePool(LinkQuiescentContext);
    }

    return;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
ExpressRootPortInterface_Constructor (
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )

/*++

Routine Description:

    This routine exports an interface for config space access to a PCI express
    root port.

Arguments:

    DeviceExtension - Supplies a pointer to a device extension.

    InterfaceSpecificData - Supplies an interface specific data.

    Version - Supplies an interface version number.

    Size - Supplies a maximum size for a buffer that will hold an interface.

    InterfaceReturn - Supplies a field for an interface that will provide
        routines to access config space on PCI express root port.

    Filtered - Not supported for this interface.

Return Value:

    STATUS_INSUFFICIENT_RESOURCES on failure to allocate resources.

    STATUS_SUCCESS on success.

    Otherwise an error code returned from a function call.

--*/

{

    PPCI_DEVICE Device;
    PEXPRESS_ROOT_PORT_CONTEXT ExpressRootPortContext;
    PPCI_EXPRESS_ROOT_PORT_INTERFACE ExpressRootPortInterface;
    PPCI_BRIDGE RootPortBridge;
    NTSTATUS Status;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Size);
    UNREFERENCED_PARAMETER(Filtered);

    __analysis_assume(Size >= sizeof(PCI_EXPRESS_ROOT_PORT_INTERFACE));

    Device = (PPCI_DEVICE)DeviceExtension;
    Status = ExpressCheckDeviceOnRootPort(Device);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Allocate a context for root port access.
    //

    ExpressRootPortContext = PciAllocatePool(PagedPool,
                                             sizeof(EXPRESS_ROOT_PORT_CONTEXT));

    if (ExpressRootPortContext == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(ExpressRootPortContext, sizeof(EXPRESS_ROOT_PORT_CONTEXT));
    ExpressRootPortContext->Signature = EXPRESS_ROOT_PORT_CONTEXT_SIGNATURE;
    RootPortBridge = Device->Parent->PciBridge;
    ExpressRootPortContext->BusNumber =
        RootPortBridge->Bus->ParentBus->SecondaryBusNumber;

    ExpressRootPortContext->SlotNumber = RootPortBridge->Slot;
    ExpressRootPortContext->PrivateInterface =
        RootPortBridge->Bus->Root->PciBusInterface;

    //
    // Fill in the root port access interface.
    //

    ExpressRootPortInterface = (PPCI_EXPRESS_ROOT_PORT_INTERFACE)InterfaceReturn;
    RtlZeroMemory(ExpressRootPortInterface, sizeof(PCI_EXPRESS_ROOT_PORT_INTERFACE));
    ExpressRootPortInterface->Size = sizeof(PCI_EXPRESS_ROOT_PORT_INTERFACE);
    ExpressRootPortInterface->Version = PCI_EXPRESS_ROOT_PORT_INTERFACE_VERSION;
    ExpressRootPortInterface->Context = (PVOID)ExpressRootPortContext;
    ExpressRootPortInterface->InterfaceReference =
        ExpressRootPortInterfaceReference;

    ExpressRootPortInterface->InterfaceDereference =
        ExpressRootPortInterfaceDereference;

    ExpressRootPortInterface->ReadConfigSpace = ExpressRootPortReadConfigSpace;
    ExpressRootPortInterface->WriteConfigSpace =
        ExpressRootPortWriteConfigSpace;

    return STATUS_SUCCESS;
}

ULONG
ExpressRootPortReadConfigSpace (
    __in PVOID Context,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    )

/*++

Routine Description:

    This routine is invoked to read config space of a chipset device.

Arguments:

    Context - Supplies a context for a chipset device whose config space is to
        be read.

    Buffer - Supplies a buffer for read data.

    Offset - Supplies an offset in the config space to read data from.

    Length - Supplies the length of data to read from a given offset.

Return Value:

    Number of bytes successfully read from a given offset.

--*/

{

    PEXPRESS_ROOT_PORT_CONTEXT ExpressRootPortContext;
    KIRQL Irql;
    ULONG LengthRead;
    PPCI_BUS_INTERFACE_STANDARD PciBusInterface;
    PUCHAR ReadBuffer;

    //
    // Sanity checks.
    //

    ExpressRootPortContext = (PEXPRESS_ROOT_PORT_CONTEXT)Context;
    if (ExpressRootPortContext->Signature !=
        EXPRESS_ROOT_PORT_CONTEXT_SIGNATURE) {

        RtlFillMemory(Buffer, Length, 0xFF);
        return 0;
    }

    PCI_MARK_SAFE_ADDITION(Offset, Length);
    if ((Offset + Length) > PCI_EXTENDED_CONFIG_LENGTH) {
        RtlFillMemory(Buffer, Length, 0xFF);
        return 0;
    }

    Irql = KeGetCurrentIrql();
    if (Irql < DISPATCH_LEVEL) {
        PciAcquirePassiveLock(&PciConfigDoubleBufferLock);
        ReadBuffer = &PciConfigDoubleBuffer[Offset];

    } else {
        ReadBuffer = Buffer;
    }

    PciBusInterface =
        (PPCI_BUS_INTERFACE_STANDARD)ExpressRootPortContext->PrivateInterface;

    LengthRead = PciBusInterface->ReadConfig(
                    PciBusInterface->Context,
                    ExpressRootPortContext->BusNumber,
                    ExpressRootPortContext->SlotNumber.u.AsULONG,
                    ReadBuffer,
                    Offset,
                    Length);

    if (Irql < DISPATCH_LEVEL) {
        RtlCopyMemory(Buffer, &PciConfigDoubleBuffer[Offset], Length);
        PciReleasePassiveLock(&PciConfigDoubleBufferLock);
    }

    PCI_ASSERT(LengthRead == Length);

    return LengthRead;
}

ULONG
ExpressRootPortWriteConfigSpace (
    __in PVOID Context,
    __in_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    )

/*++

Routine Description:

    This routine is invoked to write config space of a chipset device.

Arguments:

    Context - Supplies a context for a chipset device whose config space is to
        be written.

    Buffer - Supplies a buffer for write data.

    Offset - Supplies an offset in the config space to write data from.

    Length - Supplies the length of data to write from a given offset.

Return Value:

    Number of bytes successfully written at a given offset.

--*/

{

    PEXPRESS_ROOT_PORT_CONTEXT ExpressRootPortContext;
    KIRQL Irql;
    ULONG LengthWritten;
    PPCI_BUS_INTERFACE_STANDARD PciBusInterface;
    PUCHAR WriteBuffer;

    //
    // Sanity checks.
    //

    ExpressRootPortContext = (PEXPRESS_ROOT_PORT_CONTEXT)Context;
    if (ExpressRootPortContext->Signature !=
        EXPRESS_ROOT_PORT_CONTEXT_SIGNATURE) {

        return 0;
    }

    PCI_MARK_SAFE_ADDITION(Offset, Length);
    if ((Offset + Length) > PCI_EXTENDED_CONFIG_LENGTH) {
        return 0;
    }

    Irql = KeGetCurrentIrql();
    if (Irql < DISPATCH_LEVEL) {
        PciAcquirePassiveLock(&PciConfigDoubleBufferLock);
        RtlCopyMemory(&PciConfigDoubleBuffer[Offset], Buffer, Length);
        WriteBuffer = &PciConfigDoubleBuffer[Offset];

    } else {
        WriteBuffer = Buffer;
    }

    PciBusInterface =
        (PPCI_BUS_INTERFACE_STANDARD)ExpressRootPortContext->PrivateInterface;

    LengthWritten = PciBusInterface->WriteConfig(
                        PciBusInterface->Context,
                        ExpressRootPortContext->BusNumber,
                        ExpressRootPortContext->SlotNumber.u.AsULONG,
                        WriteBuffer,
                        Offset,
                        Length);

    if (Irql < DISPATCH_LEVEL) {
        PciReleasePassiveLock(&PciConfigDoubleBufferLock);
    }

    PCI_ASSERT(LengthWritten == Length);

    return LengthWritten;
}

VOID
ExpressRootPortInterfaceReference (
    __inout PVOID Context
    )

/*++

Routine Description:

    This routine is invoked to reference an interface context. This helps
    manage its lifetime.

Arguments:

    Context - Supplies a pointer to the context defined for this interface.

Return Value:

    None.

--*/

{

    PEXPRESS_ROOT_PORT_CONTEXT ExpressRootPortContext;

    PAGED_CODE();

    ExpressRootPortContext = (PEXPRESS_ROOT_PORT_CONTEXT)Context;

    PCI_ASSERT(ExpressRootPortContext->Signature ==
               EXPRESS_ROOT_PORT_CONTEXT_SIGNATURE);

    InterlockedIncrement(&ExpressRootPortContext->ReferenceCount);
    return;
}

VOID
ExpressRootPortInterfaceDereference (
    __inout PVOID Context
    )

/*++

Routine Description:

    This routine is invoked to de-reference an interface context. When the
    count drops to 0, the context is de-allocated.

Arguments:

    Context - Supplies a pointer to the context defined for this interface.

Return Value:

    None.

--*/

{

    PEXPRESS_ROOT_PORT_CONTEXT ExpressRootPortContext;
    LONG Count;

    PAGED_CODE();

    ExpressRootPortContext = (PEXPRESS_ROOT_PORT_CONTEXT)Context;

    PCI_ASSERT(ExpressRootPortContext->Signature ==
               EXPRESS_ROOT_PORT_CONTEXT_SIGNATURE);

    Count = InterlockedDecrement(&ExpressRootPortContext->ReferenceCount);
    if (Count == 0) {
        PciFreePool(ExpressRootPortContext);
    }

    return;
}

NTSTATUS
ExpressCheckDeviceOnRootPort (
    __in PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine is invoked to check if the device is on an express root port.

Arguments:

    Context - Supplies a pointer to the device.

Return Value:

    STATUS_SUCCESS if the device is an express device on root port.

    STATUS_UNSUCCESSFUL otherwise.

--*/

{

    PPCI_BRIDGE Bridge;
    PCI_EXPRESS_DEVICE_TYPE BridgeType;
    PCI_EXPRESS_DEVICE_TYPE DeviceType;
    PCI_EXPRESS_CAPABILITIES_REGISTER ExpressCapabilitiesRegister;
    PEXPRESS_PORT ExpressPort;

    PAGED_CODE();

    ExpressPort = Device->ExpressPort;
    if ((ExpressPort == NULL) && (Device->ExpressCapability == 0)) {
        return STATUS_UNSUCCESSFUL;
    }

    if (ExpressPort != NULL) {
        DeviceType = ExpressPort->DeviceType;

    } else {
        RtlZeroMemory(&ExpressCapabilitiesRegister,
                      sizeof(PCI_EXPRESS_CAPABILITIES_REGISTER));

        PciDeviceReadExpressRegister(Device,
                                     ExpressCapabilities,
                                     &ExpressCapabilitiesRegister);

        DeviceType = ExpressCapabilitiesRegister.DeviceType;
    }

    if ((DeviceType != PciExpressEndpoint) &&
        (DeviceType != PciExpressLegacyEndpoint)) {

        return STATUS_UNSUCCESSFUL;
    }

    //
    // For an express device check that the parent is a root port. If
    // otherwise then return failure. The link quiescent interface is
    // implemented to support the PCIe lane width change. At the time
    // of development, there is no standard way of doing PCIe lane width
    // change and the proprietary mechanism requires access to the NB
    // register to do this reliably. Hence it is essential to have an
    // express device directly below a root port.
    //

    if (ExpressPort != NULL) {
        if (ExpressPort->Parent == NULL) {
            return STATUS_UNSUCCESSFUL;
        }

        BridgeType = ExpressPort->Parent->BridgeType;

    } else {
        Bridge = Device->Parent->PciBridge;
        if ((Bridge == NULL) || (Bridge->ExpressCapability == 0)) {
            return STATUS_UNSUCCESSFUL;
        }

        RtlZeroMemory(&ExpressCapabilitiesRegister,
                      sizeof(PCI_EXPRESS_CAPABILITIES_REGISTER));

        PciBridgeReadExpressRegister(Bridge,
                                     ExpressCapabilities,
                                     &ExpressCapabilitiesRegister);

        BridgeType = ExpressCapabilitiesRegister.DeviceType;
    }

    if (BridgeType != PciExpressRootPort) {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

#undef PciDeviceReadExpressRegister
#undef PciBridgeReadExpressRegister

// ----------------------------------------------------------- Miscell Functions

NTSTATUS
ExpressDetermineSlotNumber (
    __in PEXPRESS_PORT ExpressPort,
    __out PULONG SlotNumber
    )

/*++

Routine Description:

    This routine is invoked to find the physical slot number of a PCI express
    device. The slot number for an express device is found if the parent is
    a PCI express root port or a downstream switch port, implements slots and
    the slot capabilities register contains a non-zero physical slot number.

Arguments:

    ExpressPort - Supplies a pointer to an express port for a device.

    SlotNumber - Supplies a place holder for the physical slot number.

Return Value:

    STATUS_SUCCESS if the slot number is found.

    STATUS_NOT_FOUND otherwise.

--*/

{

    PEXPRESS_BRIDGE ExpressBridge;

    PAGED_CODE();

    //
    // If this is a root port or a root complex integrated endpoint then
    // bail out.
    //

    if (ExpressPort->Parent == NULL) {
        return STATUS_NOT_FOUND;
    }

    //
    // If the parent is neither an express root port nor a downstream switch
    // port then bail out.
    //

    ExpressBridge = ExpressPort->Parent;
    if ((ExpressBridge->BridgeType != PciExpressRootPort) &&
        (ExpressBridge->BridgeType != PciExpressDownstreamSwitchPort)) {

        return STATUS_NOT_FOUND;
    }

    //
    // If the parent does not implement a slot then bail out.
    //

    if (ExpressBridge->ExpressCapabilities.SlotImplemented == 0) {
        return STATUS_NOT_FOUND;
    }

    if (ExpressBridge->PhysicalSlotNumber == 0) {
        return STATUS_NOT_FOUND;
    }

    *SlotNumber = ExpressBridge->PhysicalSlotNumber;
    return STATUS_SUCCESS;
}

NTSTATUS
ExpressApplyHPXSettings(
    __in PEXPRESS_PORT ExpressPort,
    __in PPCI_HPX_INFO HPX
    )

/*++

Routine Description:

    This 

Arguments:

    ExpressPort - Supplies a pointer to an express port for a device.


Return Value:

    NTSTATUS

--*/
{

    ULONG scratch;

    //
    // An PCIe device must be running in native mode
    // for _HPX to be valid.
    //

    if (ExpressPort == NULL) {
        return STATUS_NOT_SUPPORTED;
    }

    //
    // The HPX info must include the type2 (PCIe) information
    //

    if ((HPX->ValidTypes & HPX_TYPE_2_VALID) == 0) {
        return STATUS_NOT_SUPPORTED;
    }

    //
    // If PCI is natively handling AER and the FIRMWAREFIRST
    // flag is clear, _HPX should not be applied
    //

    if (PCI_DEVICE_CONFIGURED_FOR_AER(ExpressPort->Device)) {
        return STATUS_NOT_SUPPORTED;
    }

    //
    // Devices must have an AER Capability for _HPX to be
    // meaningful.
    //
    
    if ((ExpressPort->AerCapability == 0) ||
        (ExpressPort->Device->HackFlags & PCI_HACK_IGNORE_AER_CAPABILITY)) {

        return STATUS_SUCCESS;
    }

    //
    // Update each _HPX defined register using the 4 step process defined
    // by the ACPI 5.0 spec:
    //
    //    1. Read registers value.
    //    2. AND value with "AND mask" provided by _HPX.
    //    3. OR value with "OR mask" provided by _HPX.
    //    4. Write result.
    //

    // AER Cap: UncorrectableErrorMask
    PciReadExpressAerRegister(ExpressPort, 
                              UncorrectableErrorMask,
                              &scratch);

    scratch &= HPX->type2.UncorrectableErrorMask[PCI_HPX_AND_MASK];
    scratch |= HPX->type2.UncorrectableErrorMask[PCI_HPX_OR_MASK];
    PciWriteExpressAerRegister(ExpressPort, 
                               UncorrectableErrorMask,
                               &scratch);

    // AER Cap: UncorrectableErrorSeverity
    PciReadExpressAerRegister(ExpressPort, 
                              UncorrectableErrorSeverity,
                              &scratch);

    scratch &= HPX->type2.UncorrectableErrorSeverity[PCI_HPX_AND_MASK];
    scratch |= HPX->type2.UncorrectableErrorSeverity[PCI_HPX_OR_MASK];
    PciWriteExpressAerRegister(ExpressPort, 
                               UncorrectableErrorSeverity,
                               &scratch);

    // AER Cap: CorrectableErrorMask
    PciReadExpressAerRegister(ExpressPort, 
                              CorrectableErrorMask,
                              &scratch);

    scratch &= HPX->type2.CorrectableErrorMask[PCI_HPX_AND_MASK];
    scratch |= HPX->type2.CorrectableErrorMask[PCI_HPX_OR_MASK];
    PciWriteExpressAerRegister(ExpressPort, 
                               CorrectableErrorMask,
                               &scratch);

    // AER Cap: CapabilitiesAndControl
    PciReadExpressAerRegister(ExpressPort, 
                              CapabilitiesAndControl,
                              &scratch);

    scratch &= HPX->type2.CapabilitiesAndControl[PCI_HPX_AND_MASK];
    scratch |= HPX->type2.CapabilitiesAndControl[PCI_HPX_OR_MASK];
    PciWriteExpressAerRegister(ExpressPort, 
                               CapabilitiesAndControl,
                               &scratch);

    //
    // SecUncorrectableErrorSeverity and SecUncorrectableErrorMask only
    // apply to PciExpressToPciXBridge device types.
    //

    if (ExpressPort->DeviceType == PciExpressToPciXBridge) {

        // AER Cap: SecUncorrectableErrorSeverity
        PciReadExpressAerRegister(ExpressPort, 
                                  SecUncorrectableErrorSeverity,
                                  &scratch);

        scratch &= HPX->type2.SecUncorrectableErrorSeverity[PCI_HPX_AND_MASK];
        scratch |= HPX->type2.SecUncorrectableErrorSeverity[PCI_HPX_OR_MASK];
        PciWriteExpressAerRegister(ExpressPort, 
                                   SecUncorrectableErrorSeverity,
                                   &scratch);

        // AER Cap: SecUncorrectableErrorMask
        PciReadExpressAerRegister(ExpressPort, 
                                  SecUncorrectableErrorMask,
                                  &scratch);

        scratch &= HPX->type2.SecUncorrectableErrorMask[PCI_HPX_AND_MASK];
        scratch |= HPX->type2.SecUncorrectableErrorMask[PCI_HPX_OR_MASK];
        PciWriteExpressAerRegister(ExpressPort, 
                                   SecUncorrectableErrorMask,
                                   &scratch);
    }

    // Express Cap: DeviceControl
    PciReadExpressRegister(ExpressPort,
                           DeviceControl,
                           &scratch);

    scratch &= HPX->type2.DeviceControl[PCI_HPX_AND_MASK];
    scratch |= HPX->type2.DeviceControl[PCI_HPX_OR_MASK];
    PciWriteExpressRegister(ExpressPort, 
                            DeviceControl,
                            &scratch);

    // Express Cap: LinkControl
    PciReadExpressRegister(ExpressPort,
                           LinkControl,
                           &scratch);

    scratch &= HPX->type2.LinkControl[PCI_HPX_AND_MASK];
    scratch |= HPX->type2.LinkControl[PCI_HPX_OR_MASK];
    PciWriteExpressRegister(ExpressPort, 
                            LinkControl,
                            &scratch);

    return STATUS_SUCCESS;
}
