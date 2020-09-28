/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    interrupt.c

Abstract:

    This module implements the "Pci Interrupt Routing" interfaces supported
    by the PCI driver.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "interrupt.tmh"

// --------------------------------------------------------------------- Defines

#define PCI_ON_HYPERTRANSPORT_PATH(_Device)                 \
    (((_Device)->Parent->PciBridge &&                       \
      (_Device)->Parent->PciBridge->HyperTransportPath) ||  \
     ((_Device)->HyperTransport))

#define PCI_HYPERTRANSPORT_MSI_CONVERTED(_Device)                   \
    (((_Device)->Parent->PciBridge &&                               \
      (_Device)->Parent->PciBridge->MsiHtConversionPath) ||    \
     ((_Device)->MsiHtConverter))

#define PCI_MESSAGE_POLICY_MAXIMUM_KEY_COUNT 10000
#define PCI_MESSAGE_POLICY_MAXIMUM_KEY_NAME_LENGTH sizeof("9999")

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciProcessInterruptMessageData(
    __in PPCI_DEVICE Device,
    __in PINTERRUPT_CONNECTION_DATA ConnectionData,
    __in ULONG MessageIndex,
    __out PPHYSICAL_ADDRESS Address,
    __out PULONG DataPayload
    );

BOOLEAN
PciValidateMsiXMemorySpaceStructures (
    __in PPCI_DEVICE Device
    );

NTSTATUS
PciProgramMsiXInterrupt(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_INTERRUPT_RESOURCE Resource
    );

NTSTATUS
PciProgramMsiInterrupt(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_INTERRUPT_RESOURCE Resource
    );

VOID
PciGetAssignmentSetOverride(
    __in HANDLE AffinityHandle,
    __out PKAFFINITY AssignmentSet
    );

NTSTATUS
PciGetMessageRangeElements (
    __in PPCI_DEVICE Device,
    __in ULONG MessageNumber,
    __out PULONG DevicePolicy,
    __out PULONG DevicePriority,
    __out PULONG StartingMessage,
    __out PULONG EndingMessage,
    __out PULONG MessagesPerProcessor,
    __out PKAFFINITY AssignmentSet
    );

NTSTATUS
PciGetMessageDevicePolicy (
    __in PPCI_DEVICE Device,
    __out PULONG DevicePolicy,
    __out PULONG DevicePriority,
    __out PULONG GroupPolicy,
    __out PKAFFINITY AssignmentSet
    );

NTSTATUS
PciGetMessageAffinityMask (
    __in ULONG MessageNumber,
    __in ULONG StartingMessage,
    __in ULONG EndingMessage,
    __in ULONG MessagesPerProcessor,
    __in ULONG GroupPolicy,
    __inout PUCHAR Iterator,
    __out PUSHORT Group,
    __out PKAFFINITY ProcessorAffinity
    );

_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR PciRoutingInterface_Constructor;

VOID
PciRoutingInterface_Reference(
    __inout PVOID Context
    );

VOID
PciRoutingInterface_Dereference(
    __inout PVOID Context
    );

NTSTATUS
PciRoutingInterface_GetRoutingInfo(
    __in PDEVICE_OBJECT Pdo,
    __inout ULONG *Bus,
    __inout ULONG *PciSlot,
    __out_opt UCHAR *InterruptLine,
    __out_opt UCHAR *InterruptPin,
    __out_opt UCHAR *ClassCode,
    __out_opt UCHAR *SubClassCode,
    __out_opt PDEVICE_OBJECT *ParentPdo,
    __out_opt UCHAR *Flags,
    __out_opt UCHAR *PhantomFunctionBits,
    __out_opt PCI_BUSMASTER_DESCRIPTOR *MsiDeliveryDescriptor
    );

VOID
PciRoutingInterface_UpdateInterruptLine(
    __in PDEVICE_OBJECT Pdo,
    __in UCHAR Line
    );

_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR PciMsiXInterface_Constructor;

NTSTATUS
PciMsiXInterface_RefDeref (
    __in PVOID Context
    );

PCI_MSIX_SET_ENTRY PciMsiXInterface_SetTableEntry;
PCI_MSIX_MASKUNMASK_ENTRY PciMsiXInterface_MaskTableEntry;
PCI_MSIX_MASKUNMASK_ENTRY PciMsiXInterface_UnmaskTableEntry;
PCI_MSIX_GET_ENTRY PciMsiXInterface_GetTableEntry;
PCI_MSIX_GET_TABLE_SIZE PciMsiXInterface_GetTableSize;

NTSTATUS
PciGetLegacyDeviceRoutingInfo(
    __in PDEVICE_OBJECT LegacyDO,
    __inout ULONG *Bus,
    __inout ULONG *PciSlot,
    __out_opt UCHAR *InterruptLine,
    __out_opt UCHAR *InterruptPin,
    __out_opt UCHAR *ClassCode,
    __out_opt UCHAR *SubClassCode,
    __out_opt PDEVICE_OBJECT *ParentPdo
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciGetInterruptRequirement)
    #pragma alloc_text(PAGE, PciProcessInterruptDescriptor)
    #pragma alloc_text(PAGE, PciGetHalReportedInterruptLine)
    #pragma alloc_text(PAGE, PciValidateMsiXMemorySpaceStructures)
    #pragma alloc_text(PAGE, PciGetAssignmentSetOverride)
    #pragma alloc_text(PAGE, PciGetMessageCount)
    #pragma alloc_text(PAGE, PciGetMessagePolicy)
    #pragma alloc_text(PAGE, PciGetMessageRangeElements)
    #pragma alloc_text(PAGE, PciGetMessageDevicePolicy)
    #pragma alloc_text(PAGE, PciGetMessageAffinityMask)
    #pragma alloc_text(PAGE, PciRoutingInterface_Constructor)
    #pragma alloc_text(PAGE, PciRoutingInterface_GetRoutingInfo)
    #pragma alloc_text(PAGE, PciRoutingInterface_UpdateInterruptLine)
    #pragma alloc_text(PAGE, PciMsiXInterface_Constructor)
    #pragma alloc_text(PAGE, PciCacheLegacyDeviceRouting)
    #pragma alloc_text(PAGE, PciGetLegacyDeviceRoutingInfo)
    #pragma alloc_text(PAGE, PciDeleteVolatileInterruptRoutingData)
#endif

// --------------------------------------------------------------------- Globals

const PCI_INTERFACE PciRoutingInterface = {
    &GUID_INT_ROUTE_INTERFACE_STANDARD,     // InterfaceType
    sizeof(INT_ROUTE_INTERFACE_STANDARD),   // MinSize
    PCI_INT_ROUTE_INTRF_STANDARD_VER,       // MinVersion
    PCI_INT_ROUTE_INTRF_STANDARD_VER,       // MaxVersion
    PCIIF_FDO,                              // Flags
    PciInterface_IntRouteHandler,           // Signature
    PciRoutingInterface_Constructor         // Constructor
};

const PCI_INTERFACE PciMsiXTableConfigInterface = {
    &GUID_MSIX_TABLE_CONFIG_INTERFACE,       // InterfaceType
    PCI_MSIX_TABLE_CONFIG_MINIMUM_SIZE,      // MinSize
    PCI_MSIX_TABLE_CONFIG_INTERFACE_VERSION, // MinVersion
    PCI_MSIX_TABLE_CONFIG_INTERFACE_VERSION, // MaxVersion
    PCIIF_PDO,                               // Flags
    PciInterface_MsiXTableConfig,            // Signature
    PciMsiXInterface_Constructor             // Constructor
};

PLEGACY_DEVICE PciLegacyDeviceHead = NULL;
UCHAR PciRoundRobinIndex = 0;

const PWSTR PciRegMsiProperties =  L"Interrupt Management\\MessageSignaledInterruptProperties";
const PWSTR PciRegAffinityPolicy = L"Interrupt Management\\Affinity Policy";
const PWSTR PciRegMsiRange =  L"Range";
const PWSTR PciRegAssignmentSetOverride = L"AssignmentSetOverride";
const PWSTR PciRegDevicePolicy = L"DevicePolicy";
const PWSTR PciRegDevicePriority = L"DevicePriority";
const PWSTR PciRegGroupPolicy = L"GroupPolicy";
const PWSTR PciRegStartingMessage = L"StartingMessage";
const PWSTR PciRegEndingMessage = L"EndingMessage";
const PWSTR PciRegMessagesPerProcessor = L"MessagesPerProcessor";

// ------------------------------------------------------------------- Functions

NTSTATUS
PciGetInterruptRequirement(
    _In_ PPCI_DEVICE Device,
    _Inout_ PPCI_DEVICE_INTERRUPT_REQUIREMENT Requirement
    )
/*++

Routine Description:

   This routine probes the given device for its interrupt requirements,
   including any level-triggered or message-signaled interrupts it may have.

Arguments:

    Device - The device to get the interrupt requirement of.

    Requirement - Buffer to store the device's interrupt requirement in.

Return Value:

    NT Status code.

--*/
{
    PIO_RESOURCE_DESCRIPTOR descriptor;
    PCI_MSI_CAPABILITY msi;
    PCI_MSIX_CAPABILITY msix;
    BOOLEAN valid;

    PAGED_CODE();

    //
    // Legacy IDE controllers don't use PCI interrupts, even
    // though they might have interrupt pins.
    //
    if (PCI_LEGACY_IDE_CONTROLLER(Device)) {
        NullResourceRequirement(&Requirement->LineBased);
        return STATUS_SUCCESS;
    }

    //
    // Generate a line-based interrupt requirement if the hardware supports it.
    //
    if (Device->InterruptPin != 0) {
        Requirement->LineBased.Type = CmResourceTypeInterrupt;
        Requirement->LineBased.ShareDisposition = CmResourceShareShared;
        Requirement->LineBased.Option = 0;
        Requirement->LineBased.Flags = CM_RESOURCE_INTERRUPT_LEVEL_SENSITIVE;
        Requirement->LineBased.u.Interrupt.MinimumVector = 0;
        Requirement->LineBased.u.Interrupt.MaximumVector = MAXULONG;

        //
        // If the device is a PCI Express fabric device, the interrupts are
        // handled inside this driver, so add default policy indicating that
        // the interrupt can be handled across processor groups.
        //
        if ((Device->ExpressPort != NULL) &&
            ((Device->ExpressPort->DeviceType == PciExpressRootPort) ||
             (Device->ExpressPort->DeviceType == PciExpressDownstreamSwitchPort))) {

            Requirement->LineBased.u.Interrupt.AffinityPolicy =
                IrqPolicyMachineDefault;

            Requirement->LineBased.u.Interrupt.PriorityPolicy = 0;
            Requirement->LineBased.u.Interrupt.TargetedProcessors = 0;
            Requirement->LineBased.u.Interrupt.Group = ALL_PROCESSOR_GROUPS;
            Requirement->LineBased.Flags |=
                CM_RESOURCE_INTERRUPT_POLICY_INCLUDED;
        }

    } else {
        NullResourceRequirement(&Requirement->LineBased);
    }

    //
    // A hypertransport device can only signal an MSI if some device on the
    // path from it to the CPU can translate the PCI MSI into a hypertransport
    // interrupt message.  If this is not the case, exit without creating a
    // MSI requirement for the device.
    //
    if (PCI_ON_HYPERTRANSPORT_PATH(Device) &&
        (PCI_HYPERTRANSPORT_MSI_CONVERTED(Device) == FALSE)) {

        return STATUS_SUCCESS;
    }

    //
    // There are certain chipsets that are not capable of forwarding MSI
    // messages from AGP devices to the front side bus on the CPU. In
    // those chipsets, MSI cannot be used for AGP devices. There are two
    // ways to workaround this problem. First is to disable MSI for all
    // AGP devices and second is to compend a list of known good vs bad
    // chipsets and enable/disable MSI support for AGP devices based on
    // the chipset info. For Vista, it is the former approach that is
    // implemented i.e. MSI is not enabled for all AGP devices.
    //

    if (Device->AgpCapability != 0) {
        return STATUS_SUCCESS;
    }

    //
    // There are two kinds of message-signaled interrupts, but we will
    // only ever generate a requirement for one on each device, even if
    // multiple types are supported.  Use the MSI-X capability if available,
    // and use vanilla MSI if MSI-X is not available.
    //
    // N.B. If the MSI-X table structure or the pending bit array (PBA)
    //      structure is not compliant to the spec, then we will use the
    //      MSI capability (if present) to generate message requirements
    //      for a device.
    //

    if (Device->MsiXCapability) {
        valid = PciValidateMsiXMemorySpaceStructures(Device);
        if (valid != FALSE) {
            PciReadMsiXCapabilityRegister(Device,
                                          MessageControl,
                                          &msix.MessageControl);

            Requirement->Message.Type = PciMsiX;
            Requirement->Message.MessagesRequested =
                msix.MessageControl.TableSize;

            //
            // Compliant devices are encoded as "value from hardware plus one
            // equals the number of table entries."  Non-compliant devices
            // tend to encode the actual number of table entries in the
            // hardware. All such devices to date actually have an even number
            // of entries (typically 256), but incorrectly report an odd
            // number of entries. To support potential fixes to these devices
            // which would correct the off-by-one error, the OFF_BY_ONE hack
            // flag rounds down to the nearest even number (i.e. clear bit 0).
            //

            Requirement->Message.MessagesRequested += 1;
            if ((Device->HackFlags & PCI_HACK_MSIX_TABLE_OFF_BY_ONE) != 0) {
                Requirement->Message.MessagesRequested &= ~0x1;
            }

            //
            // The mapping table from MSI-X table entries to message numbers
            // uses the high bit to represent a mask.  With the current hardware
            // definition there are not enough messages to conflict with this
            // use of this bit.  Assert that this remains the case.
            //

            PCI_ASSERT((Requirement->Message.MessagesRequested &
                        PCI_MSIX_ENTRY_MASKED) == 0);

            descriptor = &Requirement->Message.SingleMessage;
            descriptor->Type = CmResourceTypeInterrupt;
            descriptor->ShareDisposition = CmResourceShareDeviceExclusive;
            descriptor->Option = IO_RESOURCE_PREFERRED;
            descriptor->Flags = CM_RESOURCE_INTERRUPT_LATCHED |
                                CM_RESOURCE_INTERRUPT_MESSAGE;

            descriptor->u.Interrupt.MinimumVector =
                CM_RESOURCE_INTERRUPT_MESSAGE_TOKEN;

            descriptor->u.Interrupt.MaximumVector =
                CM_RESOURCE_INTERRUPT_MESSAGE_TOKEN;

            //
            // If the device is a PCI Express fabric device, the interrupts are
            // handled inside this driver, so add default policy indicating that
            // the interrupt can be handled across processor groups.
            //
            if ((Device->ExpressPort != NULL) &&
                ((Device->ExpressPort->DeviceType == PciExpressRootPort) ||
                 (Device->ExpressPort->DeviceType == PciExpressDownstreamSwitchPort))) {

                descriptor->u.Interrupt.AffinityPolicy = IrqPolicyMachineDefault;
                descriptor->u.Interrupt.PriorityPolicy = 0;
                descriptor->u.Interrupt.TargetedProcessors = 0;
                descriptor->u.Interrupt.Group = ALL_PROCESSOR_GROUPS;
                descriptor->Flags |= CM_RESOURCE_INTERRUPT_POLICY_INCLUDED;
            }

            return STATUS_SUCCESS;
        }
    }

    if (Device->MsiCapability) {
        Requirement->Message.Type = PciMsi;
        PciReadMsiCapabilityRegister(Device,
                                     MessageControl,
                                     &msi.MessageControl);

        //
        // The MultipleMessageCapable value is the exponent base 2 of the
        // number of requested messages.
        //
        Requirement->Message.MessagesRequested =
            1 << (UCHAR)msi.MessageControl.MultipleMessageCapable;

        descriptor = &Requirement->Message.SingleMessage;
        descriptor->Type = CmResourceTypeInterrupt;
        descriptor->ShareDisposition = CmResourceShareDeviceExclusive;
        descriptor->Option = IO_RESOURCE_PREFERRED;
        descriptor->Flags = CM_RESOURCE_INTERRUPT_LATCHED |
                            CM_RESOURCE_INTERRUPT_MESSAGE;

        descriptor->u.Interrupt.MinimumVector =
            CM_RESOURCE_INTERRUPT_MESSAGE_TOKEN;

        descriptor->u.Interrupt.MaximumVector =
            CM_RESOURCE_INTERRUPT_MESSAGE_TOKEN;

        //
        // If the device is a PCI Express fabric device, the interrupts are
        // handled inside this driver, so add default policy indicating that
        // the interrupt can be handled across processor groups.
        //
        if ((Device->ExpressPort != NULL) &&
            ((Device->ExpressPort->DeviceType == PciExpressRootPort) ||
             (Device->ExpressPort->DeviceType == PciExpressDownstreamSwitchPort))) {

            descriptor->u.Interrupt.AffinityPolicy = IrqPolicyMachineDefault;
            descriptor->u.Interrupt.PriorityPolicy = 0;
            descriptor->u.Interrupt.TargetedProcessors = 0;
            descriptor->u.Interrupt.Group = ALL_PROCESSOR_GROUPS;
            descriptor->Flags |= CM_RESOURCE_INTERRUPT_POLICY_INCLUDED;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciProcessInterruptDescriptor(
    __inout PPCI_DEVICE Device,
    __in PINTERRUPT_CONNECTION_DATA ConnectionData,
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptor,
    __in ULONG DescriptorNumber,
    __inout PPCI_DEVICE_INTERRUPT_RESOURCE InterruptResource
    )
/*++

Routine Description:

    This routine processes an interrupt resource that came as
    part of a start resource list and translates it into the
    driver-internal form, determining which kind of interrupt
    it refers to as well as the parameters for the interrupt.

Arguments:

    Device - The device this descriptor describes the interrupt for.

    ConnectionData - The device property describing interrupt connection
        information not stored in the resource descriptor.

    Descriptor - The element of the start resource list to process.

    DescriptorNumber - The index of the interrupt descriptor being processed,
        if there are multiple interrupt descriptors.

    InterruptResource - The structure to process the descriptor into.

Return Value:

    NT Status code.

--*/
{

    PULONG entryMap;
    ULONG messageCount;
    PINTERRUPT_CONNECTION_DATA messageRequest;
    PCI_MSIX_CAPABILITY msix;
    PPCI_DEVICE_INTERRUPT_REQUIREMENT requirement;
    ULONG size;

    PAGED_CODE();

    requirement = &Device->InterruptRequirement;

    //
    // If the descriptor is for a message-signaled interrupt, process it based
    // on the type of MSI in use on the device.
    //
    if (Descriptor->Flags & CM_RESOURCE_INTERRUPT_MESSAGE) {
        if (ConnectionData->Count < (DescriptorNumber + 1)) {
            return STATUS_TOO_MANY_ADDRESSES;
        }

        if (ConnectionData->Vectors[DescriptorNumber].Type !=
            InterruptTypeMessageRequest) {

            return STATUS_INVALID_PARAMETER;
        }

        if (Descriptor->Flags & CM_RESOURCE_INTERRUPT_POLICY_INCLUDED) {
            InterruptResource->PolicySpecified = TRUE;
        }

        switch (requirement->Message.Type) {

            //
            // MSI-X case.  Each message is conveyed in a separate interrupt
            // descriptor, and the index of the descriptor currently being
            // processed is passed into this routine as the descriptor number.
            // The message information is stored in a message table that will
            // eventually be written to the hardware and saved in the device's
            // internal interrupt resource.
            //
        case PciMsiX:
            if (DescriptorNumber >= ConnectionData->Count) {
                return STATUS_TOO_MANY_ADDRESSES;
            }

            InterruptResource->Type = PciMsiX;
            if (DescriptorNumber == 0) {

                //
                // For the first descriptor, initialize the message table,
                // allocating it if necessary or simply zeroing it.
                //
                if (InterruptResource->MsiX.EntryMap == NULL) {
                    entryMap = PciAllocatePool(
                                    NonPagedPoolNx,
                                    sizeof(ULONG) *
                                     requirement->Message.MessagesRequested
                                    );

                    if (entryMap == NULL) {
                        return STATUS_INSUFFICIENT_RESOURCES;
                    }

                    InterruptResource->MsiX.EntryMap = entryMap;
                }

                RtlZeroMemory(
                    InterruptResource->MsiX.EntryMap,
                    sizeof(ULONG) * requirement->Message.MessagesRequested
                    );

                //
                // Also, allocate a copy of the connection data for all of the
                // messages.
                //
                if (InterruptResource->MsiX.ConnectionData != NULL) {
                    PciFreePool(InterruptResource->MsiX.ConnectionData);
                    InterruptResource->MsiX.ConnectionData = NULL;
                }

                size = sizeof(INTERRUPT_CONNECTION_DATA) +
                       (sizeof(INTERRUPT_VECTOR_DATA) *
                        (ConnectionData->Count - 1));

                messageRequest = PciAllocatePool(NonPagedPoolNx, size);
                if (messageRequest == NULL) {
                    return STATUS_INSUFFICIENT_RESOURCES;
                }

                RtlCopyMemory(messageRequest, ConnectionData, size);
                InterruptResource->MsiX.ConnectionData = messageRequest;

                //
                // Finally save the information about the location of the
                // message table.
                //
                PciReadDeviceCapability(Device, MsiXCapability, &msix);
                InterruptResource->MsiX.TableOffset =
                    msix.MessageTable.TableOffset & ~0x7;

                InterruptResource->MsiX.BARIndex =
                    (UCHAR)msix.MessageTable.BaseIndexRegister;
            }

            //
            // Record the number of messages granted.
            //
            InterruptResource->MsiX.MessagesGranted = DescriptorNumber + 1;

            //
            // Map this message number to the corresponding table index.  Table
            // entries above the number of allocated messages are not mapped,
            // leaving them assigned to message 0.  A device may have more
            // granted messages than hardware-requested messages in order to
            // keep some messages in reserve for runtime reprogramming.  Do not
            // map these messages to a table entry at this time.
            //
            if (DescriptorNumber < requirement->Message.MessagesRequested) {
                InterruptResource->MsiX.EntryMap[DescriptorNumber] = DescriptorNumber;

                PCI_ASSERT(Descriptor->u.MessageInterrupt.Raw.MessageCount == 1);
            }

            break;

            //
            // MSI case.  The resource descriptor indicates the number of
            // allocated messages.  Store this data in the device's internal
            // interrupt resource.
            //
        case PciMsi:

            InterruptResource->Type = PciMsi;

            PCI_ASSERT(DescriptorNumber == 0);

            messageCount = (UCHAR)Descriptor->u.MessageInterrupt.Raw.MessageCount;

            PCI_ASSERT(messageCount == (messageCount & ~(messageCount - 1)));
            PCI_ASSERT(messageCount <= 32);

            InterruptResource->Msi.ConnectionData = ConnectionData->Vectors[0];
            InterruptResource->Msi.GrantedBits =
                RtlFindMostSignificantBit(messageCount);

            break;

        default:

            PCI_ASSERT(FALSE);
            return STATUS_INVALID_PARAMETER;
        }

    } else {

        //
        // We got a plain-old level-triggered interrupt.
        //
        InterruptResource->Type = PciLineBased;

        PCI_ASSERT(Descriptor->u.Interrupt.Level ==
                   Descriptor->u.Interrupt.Vector);

        //
        // The interrupt line register is only 8 bits wide, but some
        // machines have more than 256 interrupt inputs.  If the interrupt
        // input assigned to the device is small enough to fit in the
        // interrupt line register, write it out.  If the interrupt input
        // assigned to the device is too large, just write 0 to the interrupt
        // line register.
        //
        if (Descriptor->u.Interrupt.Vector > 0xFF) {
            Device->InterruptResource.InterruptLine = 0;

        } else {
            Device->InterruptResource.InterruptLine =
                    (UCHAR)Descriptor->u.Interrupt.Vector;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciProcessInterruptMessageData(
    __in PPCI_DEVICE Device,
    __in PINTERRUPT_CONNECTION_DATA ConnectionData,
    __in ULONG MessageIndex,
    __out PPHYSICAL_ADDRESS Address,
    __out PULONG DataPayload
    )
/*++

Routine Description:

    This routine determines the address and data that are to be used to
    program MSI or MSI-X hardware based on the extended interrupt connection
    data provided.  The connection data describes the way the device would
    trigger its assigned processor interrupt vector using the signaling
    mechanism that is native to the system.  In general, the bus-master memory
    write supported natively by PCI MSI is also supported natively by the
    system, so the address and data payload are described directly in the
    connection data.  If the system does not natively support bus-master
    memory writes, this routine must see if there is any conversion logic in
    the system, and if so determine the appropriate PCI MSI parameters that
    must be written into the hardware to perform the appropriate conversion.

Arguments:

    Device - Supplies the device the connection data refers to.

    ConnectionData -  Supplies the extended connection information for this
        interrupt resource.

    MessageIndex - Supplies the index of the message currently being processed.

    Address - Supplies a pointer to a variable that receives the message
        address to be used for this message.

    DataPayload - Supplies a pointer to a variable that receives the message
        data payload to be used for this message.

Return Value:

    STATUS_SUCCESS if the operation succeeded.
    STATUS_INVALID_PARAMETER if the connection data contained unrecognized
        data.
    STATUS_NOT_SUPPORTED if the connection data contains information about an
        interrupt connection that this driver cannot program into hardware.

--*/
{

    INTERRUPT_CONNECTION_DATA converted;
    UCHAR destination;
    ULONG xapicAddress;
    ULONG xapicData;
    INTERRUPT_HT_INTR_INFO intrInfo;
    PINTERRUPT_VECTOR_DATA messageData;
    HAL_MESSAGE_TARGET_REQUEST messageRequest;
    NTSTATUS status;

    //
    // Verify that this connection data supplies information about the
    // message being processed.  There are two types of message-based
    // interrupts currently supported.
    //

    __analysis_assume(MessageIndex < (MessageIndex + 1));
    if (ConnectionData->Count < (MessageIndex + 1)) {
        return STATUS_INVALID_PARAMETER;
    }

    messageData = &ConnectionData->Vectors[MessageIndex];
    if ((messageData->Type != InterruptTypeXapicMessage) &&
        (messageData->Type != InterruptTypeHypertransport) &&
        (messageData->Type != InterruptTypeMessageRequest)) {

        return STATUS_INVALID_PARAMETER;
    }

    switch (messageData->Type) {

        //
        // Intel X-APIC message case.  This is the format natively supported
        // by PCI MSI, so extract the message address and data directly from
        // the connection information.
        //

    case InterruptTypeXapicMessage:
        Address->QuadPart = messageData->XapicMessage.Address.QuadPart;
        *DataPayload = messageData->XapicMessage.DataPayload;
        break;

        //
        // AMD Hyper-transport case.  This message format cannot be directly
        // produced by a PCI bus-master write, so there must be a converter
        // in the middle.  This converter applies a conversion from the address
        // and data it receives on PCI into an HT interrupt.  Perform the
        // reverse of this conversion, given the necessary format of this
        // device's interrupt on HT.
        //

    case InterruptTypeHypertransport:

        //
        // This device should only have been given an MSI interrupt requirement
        // if there is a converter of the kind described above on the path
        // from the device to the processor.  If this is not the case, something
        // is wrong.
        //

        if ((PCI_ON_HYPERTRANSPORT_PATH(Device) == FALSE) ||
            (PCI_HYPERTRANSPORT_MSI_CONVERTED(Device) == FALSE)) {

            return STATUS_NOT_SUPPORTED;
        }

        //
        // HT MSI converters perform a conversion from the Intel X-APIC message
        // format to HT's format.  The conversion is specified in appendix
        // B.5 of the Hypertransport specification.
        //
        intrInfo = messageData->Hypertransport.IntrInfo;

        //
        // The address field is laid out as follows.  Only 8 bit destination IDs
        // are supported at this time, so all remaining destination ID bits
        // are set to 0.
        //
        //  31-20: FEEh - This indicates the base address of the memory region
        //         that is converted to HT interrupts.  The code to configure
        //         mapping capabilities always sets the base address to the
        //         Intel compatible address of FEEh.
        //  19-12: Destination ID bits 7-0
        //   11-4: Destination ID bits 15-8
        //      3: Destination ID bit 27
        //      2: Destination Mode (DM) - set to 1 for logical mode,
        //         0 for physical mode.
        //    1-0: unused.
        //
        xapicAddress = 0xFEE00000;
        destination = (UCHAR)intrInfo.LowPart.bits.Destination;
        xapicAddress |= destination << 12;
        if (intrInfo.LowPart.bits.DestinationMode != 0) {
            xapicAddress |= (1 << 2);
        }

        //
        // The data payload is laid out as follows:
        //
        //    15: Trigger Mode - set to 1 for level triggered or 0 for edge.
        // 14-11: Destination ID bits 31-28
        //  10-8: Delivery Mode - indicates the mode (i.e. lowest priority)
        //       in which the interrupt is delivered.
        //   7-0: Vector
        //
        xapicData = intrInfo.LowPart.bits.Vector;
        xapicData |= (intrInfo.LowPart.bits.MessageType << 8);
        xapicData |= (intrInfo.LowPart.bits.RequestEOI << 15);

        //
        // Return the computed address and data to the caller.
        //
        Address->QuadPart = xapicAddress;
        *DataPayload = xapicData;
        break;

        //
        // Message request case.  This connection data contains the fields that
        // the HAL can use to convert the request to a native message.
        //
    case InterruptTypeMessageRequest:
        messageRequest.Type = InterruptTargetTypeApic;
        messageRequest.Apic.Vector = messageData->Vector;
        messageRequest.Apic.TargetProcessors = messageData->TargetProcessors;
        messageRequest.Apic.IntRemapInfo = messageData->IntRemapInfo;
        messageRequest.Apic.DestinationMode =
            messageData->MessageRequest.DestinationMode;

        status = HalGetMessageRoutingInfo(&messageRequest, &converted);
        if (!NT_SUCCESS(status)) {
            return status;
        }

        //
        // The converted routing information should be in native format, and
        // not in request format.
        //
        if (converted.Vectors[0].Type == InterruptTypeMessageRequest) {
            return STATUS_UNSUCCESSFUL;
        }

        //
        // Recursively call to convert the native message to an address and
        // data.
        //
        status = PciProcessInterruptMessageData(Device,
                                                &converted,
                                                0,
                                                Address,
                                                DataPayload);

        if (!NT_SUCCESS(status)) {
            return status;
        }

        break;

    default:
        return STATUS_NOT_SUPPORTED;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciProgramInterruptResource(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine programs the given device to be able to trigger
    its interrupts.  The work that needs to be done to do this
    differs based on the type of interrupt the device uses.

Arguments:

    Device - The device to program.

Return Value:

    NT Status code.

--*/
{
    NTSTATUS status;
    USHORT command;

    NON_PAGED_CODE();

    PciDisableInterrupts(Device);

    switch (Device->InterruptResource.Type) {

        case PciMsiX:

            status = PciProgramMsiXInterrupt(Device,
                                             &Device->InterruptResource);
            break;

        case PciMsi:

            status = PciProgramMsiInterrupt(Device, &Device->InterruptResource);
            break;

        case PciLineBased:

            PciReadConfigRegister(Device, Command, &command);
            command &= ~PCI_DISABLE_LEVEL_INTERRUPT;
            PciWriteConfigRegister(Device, Command, &command);
            status = STATUS_SUCCESS;
            break;

        default:

            //
            // Legacy mode IDE controllers have legacy interrupts that may be
            // masked by the interrupt disable bit, but aren't PCI interrupts,
            // so we do have an interrupt resource for them.
            // Unmask even without a resource for these.
            //
            if (PCI_LEGACY_IDE_CONTROLLER(Device)) {
                PciReadConfigRegister(Device, Command, &command);
                command &= ~PCI_DISABLE_LEVEL_INTERRUPT;
                PciWriteConfigRegister(Device, Command, &command);
            }

            status = STATUS_SUCCESS;
            break;
    }

    if (!NT_SUCCESS(status)) {
        TraceEvents(Device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_DEVPOWER,
                    "(%x:%x) %!STATUS!",
                    Device->Slot.u.bits.DeviceNumber,
                    Device->Slot.u.bits.FunctionNumber,
                    status);
    }

    return status;
}

BOOLEAN
PciValidateMsiXMemorySpaceStructures (
    __in PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine is invoked to validate the MSI-X table and PBA structures
    in the memory space behind one of the device BARs.

Arguments:

    Device - Supplies a pointer to a PCI device.

Return Value:

    TRUE if the structures are valid.

    FALSE otherwise.

--*/

{

    ULONG MessagesRequested;
    PIO_RESOURCE_DESCRIPTOR PbaBarRequirement;
    ULONG PbaIndex;
    ULONG PbaLength;
    ULONG PbaStartOffset;
    PCI_MSIX_CAPABILITY PciMsiXCapability;
    PIO_RESOURCE_DESCRIPTOR TableBarRequirement;
    ULONG TableIndex;
    ULONG TableLength;
    ULONG TableStartOffset;

    PAGED_CODE();

    PCI_ASSERT(Device->MsiXCapability != 0);

    //
    // Read the MSI-X capability on a PCI device.
    //

    PciReadDeviceCapability(Device, MsiXCapability, &PciMsiXCapability);
    TableIndex = PciMsiXCapability.MessageTable.BaseIndexRegister;
    PbaIndex = PciMsiXCapability.PBATable.BaseIndexRegister;

    //
    // Check that the BAR index for MSI-X table structure and PBA structure
    // are within bounds.
    //

    if ((TableIndex >= PCI_MAX_BAR_COUNT) || (PbaIndex >= PCI_MAX_BAR_COUNT)) {
        return FALSE;
    }

    //
    // Check that the BAR requirements for MSI-X table and PBA structures
    // correspond to a read-write memory range.
    //

    TableBarRequirement = &Device->Requirements.Bars[TableIndex];
    PbaBarRequirement = &Device->Requirements.Bars[PbaIndex];
    if ((TableBarRequirement->Type != CmResourceTypeMemory) ||
        (PbaBarRequirement->Type != CmResourceTypeMemory)) {

        return FALSE;
    }

    if ((TableBarRequirement->Flags &
         (CM_RESOURCE_MEMORY_READ_WRITE | CM_RESOURCE_MEMORY_BAR)) !=
        (CM_RESOURCE_MEMORY_READ_WRITE | CM_RESOURCE_MEMORY_BAR)) {

        return FALSE;
    }

    if ((PbaBarRequirement->Flags &
         (CM_RESOURCE_MEMORY_READ_WRITE | CM_RESOURCE_MEMORY_BAR)) !=
        (CM_RESOURCE_MEMORY_READ_WRITE | CM_RESOURCE_MEMORY_BAR)) {

        return FALSE;
    }

    //
    // Check that the memory requirement is minimum of (offset + length).
    //

    MessagesRequested = (PciMsiXCapability.MessageControl.TableSize);

    //
    // Compliant devices are encoded as "value from hardware plus one
    // equals the number of table entries."  Non-compliant devices
    // tend to encode the actual number of table entries in the
    // hardware. All such devices to date actually have an even number
    // of entries (typically 256), but incorrectly report an odd
    // number of entries. To support potential fixes to these devices
    // which would correct the off-by-one error, the OFF_BY_ONE hack
    // flag rounds down to the nearest even number (i.e. clear bit 0).
    //

    MessagesRequested += 1;
    if ((Device->HackFlags & PCI_HACK_MSIX_TABLE_OFF_BY_ONE) != 0) {
        MessagesRequested &= ~0x1;
    }

    PCI_ASSERT(MessagesRequested > 0);

    TableStartOffset = PciMsiXCapability.MessageTable.TableOffset & ~0x7;
    TableLength = MessagesRequested * sizeof(PCI_MSIX_TABLE_ENTRY);
    if ((TableStartOffset + TableLength) >
        TableBarRequirement->u.Generic.Length) {

        return FALSE;
    }

    PbaStartOffset = PciMsiXCapability.PBATable.TableOffset & ~0x7;
    PbaLength = ((MessagesRequested / MSIX_PENDING_BITS_IN_PBA_TABLE_ENTRY) *
                 MSIX_PBA_TABLE_ENTRY_SIZE);

    if ((MessagesRequested % MSIX_PENDING_BITS_IN_PBA_TABLE_ENTRY) != 0) {
        PbaLength += MSIX_PBA_TABLE_ENTRY_SIZE;
    }

    if ((PbaStartOffset + PbaLength) > PbaBarRequirement->u.Generic.Length) {
        return FALSE;
    }

    //
    // If the MSI-X table and PBA structures are mapped behind the same BAR
    // (i.e. these co-reside), then ensure that they do not overlap.
    //

    if (TableIndex == PbaIndex) {
        if (INTERSECT(TableStartOffset,
                      (TableStartOffset + TableLength - 1),
                      PbaStartOffset,
                      (PbaStartOffset + PbaLength - 1)) != FALSE) {

            return FALSE;
        }
    }

    return TRUE;
}

VOID
PciDisableInterrupts(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine prevents the given device from being able to trigger
    its interrupts.  The work that needs to be done to do this
    differs based on the type of interrupt the device uses.  Since a
    device can potentially support multiple methods of interrupt
    generation, this routine disables all interrupt types, to be safe.

Arguments:

    Device - The device to disable the interrupts of.

Return Value:

    VOID

--*/
{
    USHORT command;

    NON_PAGED_CODE();

    if (Device->InterruptPin) {
        PciReadConfigRegister(Device, Command, &command);
        command |= PCI_DISABLE_LEVEL_INTERRUPT;
        PciWriteConfigRegister(Device, Command, &command);
    }

    if (Device->MsiXCapability) {
        PciMaskMsiXInterrupt(Device);
    }

    if (Device->MsiCapability) {
        PciMaskMsiInterrupt(Device);
    }
}

UCHAR
PciGetHalReportedInterruptLine(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

   This retrieves the interrupt line the HAL would like the world to see - this
   may or may not be different than what is actually in the InterruptLine
   register.

   If adjusted, the interrupt line register will be adjusted with a value
   that contains routing information about the device. This is done for
   those HALs (HALMPS, most notably) that don't have interrupt arbiters,
   so all the interrupt line information is retrieved through this call.
   Even on these HALs, this would not be necessary, except to support those
   drivers that do not use proper PnP APIs to allocate their resources and
   instead read the Interrupt Line register to determine their interrupt
   routing.  It may be possible to phase this out over time.

Arguments:

    Device - The device who we want to check for an adjusted interrupt line

Return Value:

    The HAL reported interrupt line

--*/
{
    ULONG lengthRead;
    UCHAR line;

    PAGED_CODE();

    line = 0;

    if (Device->InterruptPin != 0) {

        //
        // The legacy HAL config space reading API retrieves the
        // adjusted value.
        //
#pragma prefast(suppress:__WARNING_USE_OTHER_FUNCTION, "This warning exists to get other drivers to invoke this wrapper")
        lengthRead = HalGetBusDataByOffset(
                        PCIConfiguration,
                        Device->BusNumber,
                        Device->Slot.u.AsULONG,
                        &line,
                        CONFIG_FIELD_OFFSET(common.InterruptLine),
                        sizeof(line)
                        );

        if (lengthRead != sizeof(line)) {

            line = Device->SavedRegisters.InterruptLine;
        }
    }

    return line;
}

// ---------------------------------------------------- MSI Programming Routines

VOID
PciMaskMsiXInterrupt(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine masks a MSI-X message signaled interrupt.

Arguments:

    Device - The device to disable the interrupt of.

Return Value:

    None.

--*/
{
    PCI_MSIX_CAPABILITY msix;

    NON_PAGED_CODE();

    PCI_ASSERT(Device->MsiXCapability);

    PciReadMsiXCapabilityRegister(Device, MessageControl, &msix.MessageControl);
    msix.MessageControl.MSIXEnable = 0;
    PciWriteMsiXCapabilityRegister(Device,
                                   MessageControl,
                                   &msix.MessageControl);
}

NTSTATUS
PciProgramMsiXInterrupt(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_INTERRUPT_RESOURCE Resource
    )
/*++

Routine Description:

    This routine programs a MSI-X capable device with the given interrupt
    resource.

    N.B. - This routine determines the address and data for the request instead
           of caching them during interrupt descriptor processing.  This allows
           the HAL to update the address and data corresponding to a request
           across power transitions.

Arguments:

    Device - The device to program the interrupt of.

    Resource - The interrupt resource to program the device with.

Return Value:

    NT Status code.

--*/
{

    PHYSICAL_ADDRESS address;
    USHORT command;
    ULONG dataPayload;
    PCI_MSIX_TABLE_ENTRY entry;
    ULONG index;
    ULONG messageNumber;
    ULONG messagesRequested;
    PCI_MSIX_CAPABILITY msix;
    NTSTATUS status;
    PPCI_MSIX_TABLE_ENTRY table;
    ULONG ulongCountForTableEntry;

    NON_PAGED_CODE();

    PCI_ASSERT(Device->MsiXCapability);
    PCI_ASSERT(Resource->Type == PciMsiX);
    PCI_ASSERT(Resource->MsiX.EntryMap != NULL);
    PCI_ASSERT(Device->Connected);

    //
    // The spec requires full DWORD or QWORD aligned transactions to the MSI-X
    // table. Count the number of ulong values for a single MSI-X table entry.
    //

    ulongCountForTableEntry = sizeof(PCI_MSIX_TABLE_ENTRY) / sizeof(ULONG);

    //
    // Loop through the table, retrieving the address and data corresponding to
    // the message that is assigned to the table entry.  Then set the entry into
    // the table.
    //

    table = Resource->MsiX.Table;
    messagesRequested = Device->InterruptRequirement.Message.MessagesRequested;
    for (index = 0; index < messagesRequested; index += 1) {
        messageNumber = Resource->MsiX.EntryMap[index] & ~PCI_MSIX_ENTRY_MASKED;
        status = PciProcessInterruptMessageData(Device,
                                                Resource->MsiX.ConnectionData,
                                                messageNumber,
                                                &address,
                                                &dataPayload);

        if (!NT_SUCCESS(status)) {
            return status;
        }

        READ_REGISTER_BUFFER_ULONG((PULONG)&table[index],
                                    (PULONG)&entry,
                                    ulongCountForTableEntry);

        entry.MessageAddress.QuadPart = address.QuadPart;
        entry.MessageData = dataPayload;
        if (Resource->MsiX.EntryMap[index] & PCI_MSIX_ENTRY_MASKED) {
            entry.VectorControl.Mask = 1;
        } else {
            entry.VectorControl.Mask = 0;
        }

        WRITE_REGISTER_BUFFER_ULONG((PULONG)&table[index],
                                    (PULONG)&entry,
                                    ulongCountForTableEntry);
    }

    //
    // Finally, do the global enable.
    //
    PciReadMsiXCapabilityRegister(Device, MessageControl, &msix.MessageControl);
    msix.MessageControl.MSIXEnable = 1;
    PciWriteMsiXCapabilityRegister(Device,
                                   MessageControl,
                                   &msix.MessageControl);

    //
    // Some devices require interrupt disable bit cleared for Msi and Msi-X.
    //

    if ((Device->HackFlags & PCI_HACK_CLEAR_INT_DISABLE_FOR_MSI) != 0) {
        PciReadConfigRegister(Device, Command, &command);
        command &= ~PCI_DISABLE_LEVEL_INTERRUPT;
        PciWriteConfigRegister(Device, Command, &command);
    }

    return STATUS_SUCCESS;
}

VOID
PciMaskMsiInterrupt(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine masks a MSI message signaled interrupt.

Arguments:

    Device - The device to disable the interrupt of.

Return Value:

    VOID

--*/
{
    PCI_MSI_CAPABILITY msi;

    NON_PAGED_CODE();

    PciReadMsiCapabilityRegister(Device, MessageControl, &msi.MessageControl);
    msi.MessageControl.MSIEnable = 0;
    PciWriteMsiCapabilityRegister(Device, MessageControl, &msi.MessageControl);
}

NTSTATUS
PciProgramMsiInterrupt(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_INTERRUPT_RESOURCE Resource
    )
/*++

Routine Description:

    This routine programs a MSI-capable device with the given
    interrupt resource.

Arguments:

    Device - The device to program the interrupt of.

    Resource - The interrupt resource to program the device with.

Return Value:

    NT Status code.

--*/
{
    PHYSICAL_ADDRESS address;
    USHORT command;
    INTERRUPT_CONNECTION_DATA connectionData;
    ULONG dataPayload;
    ULONG maskValue;
    ULONG messageCount;
    PCI_MSI_CAPABILITY msi;
    NTSTATUS status;

    NON_PAGED_CODE();

    PCI_ASSERT(Device->MsiCapability);
    PCI_ASSERT(Resource->Type == PciMsi);

    //
    // Convert the message request to the address and data to program into the
    // device.
    //
    connectionData.Count = 1;
    connectionData.Vectors[0] = Resource->Msi.ConnectionData;
    status = PciProcessInterruptMessageData(Device,
                                            &connectionData,
                                            0,
                                            &address,
                                            &dataPayload);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    PCI_ASSERT(dataPayload <= MAXUSHORT);

    //
    // Read the control register for the capability bits therein.
    //
    PciReadMsiCapabilityRegister(Device, MessageControl, &msi.MessageControl);

    //
    // Set the proper resources in the MSI capability structure.
    //
    // First write the message address.  This must be DWORD aligned address.
    //
    msi.MessageAddressLower.Raw = address.LowPart;

    PCI_ASSERT(msi.MessageAddressLower.Register.Reserved == 0);

    PciWriteMsiCapabilityRegister(Device,
                                  MessageAddressLower,
                                  &msi.MessageAddressLower);

    if (msi.MessageControl.CapableOf64Bits) {

        //
        // This is a 64 bit MSI capability.  Write the upper 32 bits of the
        // address and then the data.
        //
        msi.Option64Bit.MessageAddressUpper = address.HighPart;
        PciWriteMsiCapabilityRegister(Device,
                                      Option64Bit.MessageAddressUpper,
                                      &msi.Option64Bit.MessageAddressUpper
                                      );

        msi.Option64Bit.MessageData = (USHORT)dataPayload;
        PciWriteMsiCapabilityRegister(Device,
                                      Option64Bit.MessageData,
                                      &msi.Option64Bit.MessageData
                                      );

    } else {

        //
        // This is a 32 bit MSI capability.  The address has already been
        // written, just write the data.
        //

        msi.Option32Bit.MessageData = (USHORT)dataPayload;
        PciWriteMsiCapabilityRegister(Device,
                                      Option32Bit.MessageData,
                                      &msi.Option32Bit.MessageData
                                      );
    }

    //
    // If this function supports vector masking then clear the message bits to
    // unmask them.
    //

    if (msi.MessageControl.PerVectorMaskCapable != 0) {
        messageCount = (1 << Resource->Msi.GrantedBits);
        maskValue = ~((1 << messageCount) - 1) ;
        if (msi.MessageControl.CapableOf64Bits != 0) {
            msi.Option64Bit.MaskBits = maskValue;
            PciWriteMsiCapabilityRegister(Device,
                                          Option64Bit.MaskBits,
                                          &msi.Option64Bit.MaskBits);

        } else {
            msi.Option32Bit.MaskBits = maskValue;
            PciWriteMsiCapabilityRegister(Device,
                                          Option32Bit.MaskBits,
                                          &msi.Option32Bit.MaskBits);
        }
    }

    //
    // Finally write the number of granted messages and the enable bit in the
    // MessageControl register.
    //
    msi.MessageControl.MultipleMessageEnable = Resource->Msi.GrantedBits;
    msi.MessageControl.MSIEnable = 1;
    PciWriteMsiCapabilityRegister(Device, MessageControl, &msi.MessageControl);

    //
    // Some devices require interrupt disable bit cleared for Msi and Msi-X.
    //

    if ((Device->HackFlags & PCI_HACK_CLEAR_INT_DISABLE_FOR_MSI) != 0) {
        PciReadConfigRegister(Device, Command, &command);
        command &= ~PCI_DISABLE_LEVEL_INTERRUPT;
        PciWriteConfigRegister(Device, Command, &command);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciProgramHtMsiMapCapability (
    __in PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine programs a hyper-transport based MSI mapping capability.

Arguments:

    Device - Supplies a pointer to a device extension that has the hyper
        transport based MSI mapping capability.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PCI_HT_CAPABILITY HtCapability;

    NON_PAGED_CODE();

    PCI_ASSERT(Device->HtMsiMapCapability != 0);

    PciReadDeviceConfig(Device,
                        &HtCapability,
                        Device->HtMsiMapCapability,
                        PCI_HT_CAPABILITY_COMMON_SIZE);

    //
    // The MSI mapping capability will convert memory writes to a particular
    // address range into HT interrupts.  It expects the format of the
    // memory write address and data to comply with the Intel XAPIC spec,
    // with the exception of the base address of the memory range, which
    // may be configurable.  Later versions of the MSI mapping capability
    // fix the base address to the Intel-compatible range, making the
    // address and data completely Intel-compatible.  For versions that are
    // more flexible than this, reduce the complexity by programming the
    // base address to be the Intel-compatible range of 00000000'FEE00000.
    //

    if (HtCapability.Command.MsiMapping.Fixed == FALSE) {
        PciReadDeviceConfig(Device,
                            &HtCapability,
                            Device->HtMsiMapCapability,
                            PCI_HT_MSI_MAPPING_CAPABILITY_SIZE);

        if ((HtCapability.Offset04.MsiMapping.AddressLow != 0xFEE) ||
            (HtCapability.Offset08.MsiMapping.AddressUpper != 0)) {

            HtCapability.Offset04.MsiMapping.AddressLow = 0xFEE;
            HtCapability.Offset08.MsiMapping.AddressUpper = 0;
            PciWriteHtMsiCapabilityRegister(Device,
                                            Offset04,
                                            &HtCapability.Offset04);

            PciWriteHtMsiCapabilityRegister(Device,
                                            Offset08.MsiMapping,
                                            &HtCapability.Offset08.MsiMapping);
        }
    }

    //
    // Enable the capability if it is not.
    //

    if (HtCapability.Command.MsiMapping.Enable == FALSE) {
        HtCapability.Command.MsiMapping.Enable = 1;
        PciWriteHtMsiCapabilityRegister(Device, Command, &HtCapability.Command);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciGetMessageCount(
    __in PPCI_DEVICE Device,
    __out PULONG MessageCount
    )
/*++

Routine Description:

    This routine looks under the device's entry in the Enum branch to
    see whether the INF set MSI-related values.

Arguments:

    Device - Identifies the device.

    MessageCount - Pointer to be filled in with the number of messages.
        If this value doesn't exist in the registry, then the value
        will be set to zero, which means that the MessageCount isn't
        meaningful.

Return Value:

    STATUS_SUCCESS if all went well, and if MessageCount was filled in.

    An appropriate error status code if the registry could not be accessed.

--*/
{
    ULONG messagesRequested;
    NTSTATUS status;
    HANDLE rootHandle;
    PULONG valueBuffer;
    ULONG valueLength;

    PAGED_CODE();

    *MessageCount = 0;
    rootHandle = NULL;
    valueBuffer = NULL;

    PCI_ASSERT(Device->InterruptRequirement.Message.Type != PciNone);

    //
    // When MSI is not enabled in the system and a PCIe bridge is enumerated
    // in non-express mode then do not allocate any MSI requirement. This
    // will allow MSI only express bridges to start when MSI is not an option.
    //

    if ((Device->ExpressPort == NULL) &&
        (Device->ExpressCapability != 0) &&
        (Device->HeaderType == PCI_BRIDGE_TYPE) &&
        (PciSystemMsiEnabled == FALSE)) {

        return STATUS_SUCCESS;
    }

    //
    // Find the key in the Enum branch that corresponds to this device.
    //
    status = IoOpenDeviceRegistryKey(Device->DeviceObject,
                                     PLUGPLAY_REGKEY_DEVICE,
                                     KEY_ALL_ACCESS,
                                     &rootHandle);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // If the  device supports level triggered interrupts, we assume the
    // driver is not MSI aware, unless it has an "MSI Supported" value.
    // There is another exception to this. If the device is a PCI Express
    // root port or a downstream switch port, then we will always generate
    // MSI requirement for it.
    // For devices that don't have level triggered interrupts, this check
    // isn't necessary - we assume a driver for an MSI-only device is
    // MSI aware.
    //
    if (Device->InterruptRequirement.LineBased.Type ==
        CmResourceTypeInterrupt) {

        if ((Device->ExpressPort == NULL) ||
            ((Device->ExpressPort->DeviceType != PciExpressRootPort) &&
             (Device->ExpressPort->DeviceType !=
              PciExpressDownstreamSwitchPort))) {

            //
            // Under the Interrupt Management key under this PDO's device registry
            // key, MSISupported is a REG_DWORD.
            //
            status = PciGetRegistryValue(L"MSISupported",
                                        PciRegMsiProperties,
                                        rootHandle,
                                        REG_DWORD,
                                        &valueBuffer,
                                        &valueLength
                                        );

            if (!NT_SUCCESS(status)) {
                goto exit;
            }

            if (valueLength < sizeof(ULONG)) {
                status = STATUS_BUFFER_TOO_SMALL;
                goto exit;
            }

            if (*valueBuffer == 0) {

                //
                // MSI is not supported - keep the message count at 0.
                //
                status = STATUS_SUCCESS;
                goto exit;
            }
        }
    }

    messagesRequested = Device->InterruptRequirement.Message.MessagesRequested;

    //
    // Under the Interrupt Management key under this PDO's device registry key,
    // the message limit is a REG_DWORD.
    //
    status = PciGetRegistryValue(L"MessageNumberLimit",
                                 PciRegMsiProperties,
                                 rootHandle,
                                 REG_DWORD,
                                 &valueBuffer,
                                 &valueLength
                                 );

    if (NT_SUCCESS(status)) {
        if (valueLength < sizeof(ULONG)) {
            status = STATUS_BUFFER_TOO_SMALL;
            goto exit;
        }

        //
        // Now find the number of requested messages.  This is the
        // minimum of the number requested by the hardware and
        // (optionally) the number specified in the registry.
        //
        messagesRequested = min(*valueBuffer, messagesRequested);
    }

    //
    // With PCI 2.2 MSI, the ACPI interrupt arbiter will only allocate 16
    // messages per request, so that each request will always fall into a
    // single IRQL.  Avoid requests that will never be satisfied by restricting
    // the message count to 16 in this case.
    //
    if (Device->InterruptRequirement.Message.Type == PciMsi) {
        if (messagesRequested > 16) {
            messagesRequested = 16;
        }
    }

    *MessageCount = messagesRequested;
    status = STATUS_SUCCESS;

exit:

    if (valueBuffer) {
        PciFreePool(valueBuffer);
    }

    if (rootHandle) {
        ZwClose(rootHandle);
    }

    return status;
}

NTSTATUS
PciGetMessagePolicy (
    __in PPCI_DEVICE Device,
    __in ULONG MessageCount,
    __in ULONG MessageNumber,
    __inout PUCHAR Iterator,
    __out PBOOLEAN DevicePolicyExists,
    __out PIRQ_DEVICE_POLICY AffinityPolicy,
    __out PIRQ_PRIORITY PriorityPolicy,
    __out PUSHORT Group,
    __out PKAFFINITY TargetProcessors
    )

/*++

Routine Description:

    This routine provides MSI-X policy settings for a given message.

Arguments:

    Device - Supplies a pointer to a PCI device.

    MessageCount - Supplies a count of MSI-X messages for a device.

    MessageNumber - Supplies the message number whose policy is being fetched.

    Iterator - Supplies a place holder for a context maintained through
        multiple calls to this routine to get the policy for different
        messages for the same device.

    DevicePolicyExists - Supplies a place holder for a boolean whose value is
        set to TRUE when an MSI interrupt policy exists.

    AffinityPolicy - Supplies a place holder for an affinity policy of a given
        message.

    PriorityPolicy - Supplies a place holder for a priority policy of a given
        message.

    Group - Supplies a place holder for a processor group number.

    TargetProcessors - Supplies a place holder for a processor affinity mask.

Return Value:

    NT status value.

--*/

{

    KAFFINITY AssignmentSet;
    ULONG DevicePolicy;
    ULONG DevicePriority;
    ULONG EndingMessage;
    ULONG GroupPolicy;
    ULONG MessagesPerProcessor;
    ULONG StartingMessage;
    NTSTATUS Status;
    PVOID Unused;

    PAGED_CODE();

    *DevicePolicyExists = FALSE;

    //
    // Fetch a range subkey that contains policy settings for a given message
    // index.
    //

    Status = PciGetMessageRangeElements(Device,
                                        MessageNumber,
                                        &DevicePolicy,
                                        &DevicePriority,
                                        &StartingMessage,
                                        &EndingMessage,
                                        &MessagesPerProcessor,
                                        &AssignmentSet);

    if (NT_SUCCESS(Status)) {

        //
        // Get the per-driver group affinity policy.
        //

        Status = PciGetMessageDevicePolicy(Device,
                                           (PULONG)&Unused,
                                           (PULONG)&Unused,
                                           &GroupPolicy,
                                           (PKAFFINITY)&Unused);

    } else {

        //
        // If range subkey could not be read, then process device policy
        // for MSI.
        //

        StartingMessage = 0;
        EndingMessage = MessageCount - 1;
        MessagesPerProcessor = 1;
        Status = PciGetMessageDevicePolicy(Device,
                                           &DevicePolicy,
                                           &DevicePriority,
                                           &GroupPolicy,
                                           &AssignmentSet);
    }

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    *DevicePolicyExists = TRUE;
    if (DevicePolicy == IrqPolicySpreadMessagesAcrossAllProcessors) {

        //
        // For this policy, ignore the processor assignment set read from the
        // registry and generate a new affinity set based on the number of NUMA
        // nodes and active processors in the system. Correspondingly, change
        // the affinity policy to 'specified' processors.
        //

        *AffinityPolicy = IrqPolicySpecifiedProcessors;
        *PriorityPolicy = DevicePriority;
        *Group = 0;
        *TargetProcessors = 0;
        Status = PciGetMessageAffinityMask(MessageNumber,
                                           StartingMessage,
                                           EndingMessage,
                                           MessagesPerProcessor,
                                           GroupPolicy,
                                           Iterator,
                                           Group,
                                           TargetProcessors);

    } else {
        *AffinityPolicy = (IRQ_DEVICE_POLICY)DevicePolicy;
        *PriorityPolicy = DevicePriority;
        *TargetProcessors = AssignmentSet;
        if ((GroupPolicy == GroupAffinityAllGroupZero) ||
            (DevicePolicy == IrqPolicySpecifiedProcessors)) {

            *Group = 0;
        } else {
            *Group = ALL_PROCESSOR_GROUPS;
        }

        Status = STATUS_SUCCESS;
    }

Exit:
    return Status;
}

VOID
PciGetAssignmentSetOverride(
    __in HANDLE AffinityHandle,
    __out PKAFFINITY AssignmentSet
    )

/*++

Routine Description:

    This routine fetches the policy request for processor affinity associated
    with a device or message.

Arguments:

    AffinityHandle - Supplies a registry handle to the affinity key.

    AssignmentSet - Supplies pointer to be filled with the requested affinity
        mask.

Return Value:

    None.

--*/

{

    NTSTATUS Status;
    PUCHAR ValueBuffer;
    ULONG ValueLength;

    PAGED_CODE();

    //
    // Retrieve the requested processor affinity mask from the device's policy
    // keys.  For backwards compatibility handle several types.  In the case
    // where multi-byte binary data is found, treat the input byte order as
    // little endian.
    //

    *AssignmentSet = 0;
    ValueBuffer = NULL;
    ValueLength = 0;
    Status = PciGetRegistryValue(PciRegAssignmentSetOverride,
                                 NULL,
                                 AffinityHandle,
                                 REG_DWORD,
                                 &ValueBuffer,
                                 &ValueLength);

    if (NT_SUCCESS(Status)) {
        *AssignmentSet = (KAFFINITY)(*((ULONG*)ValueBuffer));
        PciFreePool(ValueBuffer);
        return;
    }

    ValueBuffer = NULL;
    ValueLength = 0;
    Status = PciGetRegistryValue(PciRegAssignmentSetOverride,
                                 NULL,
                                 AffinityHandle,
                                 REG_QWORD,
                                 &ValueBuffer,
                                 &ValueLength);

    if (NT_SUCCESS(Status)) {
        *AssignmentSet = (KAFFINITY)(*((ULONG64*)ValueBuffer));
        PciFreePool(ValueBuffer);
        return;
    }

    ValueBuffer = NULL;
    ValueLength = 0;
    Status = PciGetRegistryValue(PciRegAssignmentSetOverride,
                                 NULL,
                                 AffinityHandle,
                                 REG_BINARY,
                                 &ValueBuffer,
                                 &ValueLength);

    if (NT_SUCCESS(Status)) {
        if ((ValueLength > 0) &&
            (ValueLength <= sizeof(KAFFINITY))) {

            while (ValueLength != 0) {
                *AssignmentSet <<= 8;
                ValueLength -= 1;
                *AssignmentSet |= ValueBuffer[ValueLength];
            }
        }

        PciFreePool(ValueBuffer);
        return;
    }
}

NTSTATUS
PciGetMessageRangeElements (
    __in PPCI_DEVICE Device,
    __in ULONG MessageNumber,
    __out PULONG DevicePolicy,
    __out PULONG DevicePriority,
    __out PULONG StartingMessage,
    __out PULONG EndingMessage,
    __out PULONG MessagesPerProcessor,
    __out PKAFFINITY AssignmentSet
    )

/*++

Routine Description:

    This routine provides the policy settings read from a range subkey (if
    present) for a given message.

Arguments:

    Device - Supplies a pointer to a PCI device.

    MessageNumber - Supplies the message number for range elements that
        comprise of policy settings.

    DevicePolicy - Supplies a place holder for a device policy of a given
        message number.

    DevicePriority - Supplies a place holder for a device priority of a given
        message number.

    StartingMessage - Supplies a place holder for a starting message in a range.

    EndingMessage - Supplies a place holder for an ending message in a range.

    MessagesPerProcessor - Supplies a place holder for the number of messages
        in a range targeted at a processor.

    AssignmentSet - Supplies a place holder for a processor mask for a given
        message.

Return Value:

    NT status value.

--*/

{

    ULONG Index;
    HANDLE MessageHandle;
    HANDLE MsiHandle;
    BOOLEAN Opened;
    WCHAR Ordinal[PCI_MESSAGE_POLICY_MAXIMUM_KEY_NAME_LENGTH];
    RTL_QUERY_REGISTRY_TABLE QueryTable[6] = {0};
    HANDLE RangeHandle;
    HANDLE RootHandle;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // Find the key in the Enum branch that corresponds to this device.
    // Failure to open the reg keys is okay, as it's fine for there to
    // be no policy associated with this device.
    //

    Status = IoOpenDeviceRegistryKey(Device->DeviceObject,
                                     PLUGPLAY_REGKEY_DEVICE,
                                     KEY_ALL_ACCESS,
                                     &RootHandle);

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    Opened = PciOpenKey(PciRegMsiProperties,
                        RootHandle,
                        KEY_ALL_ACCESS,
                        &MsiHandle,
                        NULL);

    ZwClose(RootHandle);
    if (Opened == FALSE) {
        Status = STATUS_NOT_FOUND;
        goto Exit;
    }

    //
    // Attempt to open a sub-key called "Range."
    //

    Opened = PciOpenKey(PciRegMsiRange,
                        MsiHandle,
                        KEY_ALL_ACCESS,
                        &RangeHandle,
                        NULL);

    ZwClose(MsiHandle);
    if (Opened == FALSE) {
        Status = STATUS_NOT_FOUND;
        goto Exit;
    }

    //
    // Cycle through all the sub-keys that have names "0", "1", "2", etc. For
    // each that exists, get a bunch of values.  If they match the message
    // asked about, then return the data. If not, keep searching.
    //

    for (Index = 0; Index < PCI_MESSAGE_POLICY_MAXIMUM_KEY_COUNT; Index++) {
        Status = RtlStringCchPrintfW(Ordinal,
                                     PCI_MESSAGE_POLICY_MAXIMUM_KEY_NAME_LENGTH,
                                     L"%d",
                                     Index);
        PCI_ASSERT(NT_SUCCESS(Status));
        UNREFERENCED_PARAMETER(Status);

        Opened = PciOpenKey(Ordinal,
                            RangeHandle,
                            KEY_ALL_ACCESS,
                            &MessageHandle,
                            NULL);

        if (Opened == FALSE) {

            //
            // No more sub-keys.
            //

            Status = STATUS_UNSUCCESSFUL;
            break;
        }

        //
        // Build a table of values. Each will be filled in only if it exists.
        //

        //
        // Read a reg value: "AssignmentSetOverride".
        //

        *AssignmentSet = 0;
        PciGetAssignmentSetOverride(MessageHandle, AssignmentSet);

        //
        // Prepare to read a reg value: "DevicePolicy".
        //

        *DevicePolicy = 0;
        QueryTable[0].Flags = RTL_QUERY_REGISTRY_DIRECT |
                              RTL_QUERY_REGISTRY_TYPECHECK;

        QueryTable[0].Name = PciRegDevicePolicy;
        QueryTable[0].EntryContext = DevicePolicy;
        QueryTable[0].DefaultType = (REG_DWORD << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT) | REG_DWORD;
        QueryTable[0].DefaultData = DevicePolicy;
        QueryTable[0].DefaultLength = 0;

        //
        // Prepare to read a reg value: "DevicePriority".
        //

        *DevicePriority = 0;
        QueryTable[1].Flags = RTL_QUERY_REGISTRY_DIRECT |
                              RTL_QUERY_REGISTRY_TYPECHECK;

        QueryTable[1].Name = PciRegDevicePriority;
        QueryTable[1].EntryContext = DevicePriority;
        QueryTable[1].DefaultType = (REG_DWORD << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT) | REG_DWORD;
        QueryTable[1].DefaultData = DevicePriority;
        QueryTable[1].DefaultLength = 0;

        //
        // Prepare to read a reg value: "StartingMessage".
        //

        *StartingMessage = 0;
        QueryTable[2].Flags = RTL_QUERY_REGISTRY_DIRECT |
                              RTL_QUERY_REGISTRY_TYPECHECK;

        QueryTable[2].Name = PciRegStartingMessage;
        QueryTable[2].EntryContext = StartingMessage;
        QueryTable[2].DefaultType = (REG_DWORD << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT) | REG_DWORD;
        QueryTable[2].DefaultData = StartingMessage;
        QueryTable[2].DefaultLength = 0;

        //
        // Prepare to read a reg value: "EndingMessage".
        //

        *EndingMessage = 0;
        QueryTable[3].Flags = RTL_QUERY_REGISTRY_DIRECT |
                              RTL_QUERY_REGISTRY_TYPECHECK;

        QueryTable[3].Name = PciRegEndingMessage;
        QueryTable[3].EntryContext = EndingMessage;
        QueryTable[3].DefaultType = (REG_DWORD << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT) | REG_DWORD;
        QueryTable[3].DefaultData = EndingMessage;
        QueryTable[3].DefaultLength = 0;

        //
        // Prepare to read a reg value: "MessagesPerProcessor"
        //

        *MessagesPerProcessor = 0;
        QueryTable[4].Flags = RTL_QUERY_REGISTRY_DIRECT |
                              RTL_QUERY_REGISTRY_TYPECHECK;

        QueryTable[4].Name = PciRegMessagesPerProcessor;
        QueryTable[4].EntryContext = MessagesPerProcessor;
        QueryTable[4].DefaultType = (REG_DWORD << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT) | REG_DWORD;
        QueryTable[4].DefaultData = MessagesPerProcessor;
        QueryTable[4].DefaultLength = 0;

        Status = RtlQueryRegistryValues(RTL_REGISTRY_HANDLE,
                                        (PWSTR)MessageHandle,
                                        &QueryTable[0],
                                        NULL,
                                        NULL);

        ZwClose(MessageHandle);
        if (!NT_SUCCESS(Status)) {
            break;
        }

        PCI_ASSERT(*StartingMessage <= *EndingMessage);

        if ((MessageNumber >= *StartingMessage) &&
            (MessageNumber <= *EndingMessage)) {

            break;
        }

        Status = STATUS_NOT_FOUND;
    }

    ZwClose(RangeHandle);

Exit:
    return Status;
}

NTSTATUS
PciGetMessageDevicePolicy (
    __in PPCI_DEVICE Device,
    __out PULONG DevicePolicy,
    __out PULONG DevicePriority,
    __out PULONG GroupPolicy,
    __out PKAFFINITY AssignmentSet
    )

/*++

Routine Description:

    This routine is invoked to fetch device policies for MSI-X messages. These
    policies are applicable to all messages except in the case where range
    subkeys are defined.

Arguments:

    Device - Supplies a pointer to a PCI device.

    DevicePolicy - Supplies a place holder for device policy.

    DevicePriority - Supplies a place holder for device priority.

    GroupPolicy - Supplies a place holder for group policy.

    AssignmentSet - Supplies a place holder for a processor mask.

Return Value:

    NT status value.

--*/

{

    BOOLEAN Opened;
    HANDLE PolicyHandle;
    RTL_QUERY_REGISTRY_TABLE QueryTable[4] = {0};
    HANDLE RootHandle;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // Find the key in the Enum branch that corresponds to this device.
    // Failure to open the reg keys is okay, as it's fine for there to
    // be no policy associated with this device.
    //

    Status = IoOpenDeviceRegistryKey(Device->DeviceObject,
                                     PLUGPLAY_REGKEY_DEVICE,
                                     KEY_ALL_ACCESS,
                                     &RootHandle);

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    Opened = PciOpenKey(PciRegAffinityPolicy,
                        RootHandle,
                        KEY_ALL_ACCESS,
                        &PolicyHandle,
                        NULL);

    ZwClose(RootHandle);
    if (Opened == FALSE) {
        Status = STATUS_NOT_FOUND;
        goto Exit;
    }

    //
    // Read a reg value: "AssignmentSetOverride".
    //

    *AssignmentSet = 0;
    PciGetAssignmentSetOverride(PolicyHandle, AssignmentSet);

    //
    // Prepare to read a reg value: "DevicePolicy".
    //

    *DevicePolicy = 0;
    QueryTable[0].Flags = RTL_QUERY_REGISTRY_DIRECT |
                          RTL_QUERY_REGISTRY_TYPECHECK;

    QueryTable[0].Name = PciRegDevicePolicy;;
    QueryTable[0].EntryContext = DevicePolicy;
    QueryTable[0].DefaultType = (REG_DWORD << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT) | REG_DWORD;
    QueryTable[0].DefaultData = DevicePolicy;
    QueryTable[0].DefaultLength = 0;

    //
    // Prepare to read a reg value: "DevicePriority".
    //

    *DevicePriority = 0;
    QueryTable[1].Flags = RTL_QUERY_REGISTRY_DIRECT |
                          RTL_QUERY_REGISTRY_TYPECHECK;

    QueryTable[1].Name = PciRegDevicePriority;
    QueryTable[1].EntryContext = DevicePriority;
    QueryTable[1].DefaultType = (REG_DWORD << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT) | REG_DWORD;
    QueryTable[1].DefaultData = DevicePriority;
    QueryTable[1].DefaultLength = 0;

    //
    // Prepare to read a reg value: "GroupPolicy".
    //

    *GroupPolicy = GroupAffinityAllGroupZero;
    QueryTable[2].Flags = RTL_QUERY_REGISTRY_DIRECT |
                          RTL_QUERY_REGISTRY_TYPECHECK;

    QueryTable[2].Name = PciRegGroupPolicy;
    QueryTable[2].EntryContext = GroupPolicy;
    QueryTable[2].DefaultType = (REG_DWORD << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT) | REG_DWORD;
    QueryTable[2].DefaultData = GroupPolicy;
    QueryTable[2].DefaultLength = 0;

    Status = RtlQueryRegistryValues(RTL_REGISTRY_HANDLE,
                                    (PWSTR)PolicyHandle,
                                    &QueryTable[0],
                                    NULL,
                                    NULL);

    ZwClose(PolicyHandle);

Exit:
    return Status;
}

NTSTATUS
PciGetMessageAffinityMask (
    __in ULONG MessageNumber,
    __in ULONG StartingMessage,
    __in ULONG EndingMessage,
    __in ULONG MessagesPerProcessor,
    __in ULONG GroupPolicy,
    __inout PUCHAR Iterator,
    __out PUSHORT Group,
    __out PKAFFINITY ProcessorAffinity
    )

/*++

Routine Description:

    This routine is invoked to provide a processor affinity mask for a given
    message. This is for the policy of spread messages across all processors.
    The affinity mask describes a single processor for a message.

Arguments:

    MessageNumber - Supplies a message number whose affinity mask is to be
        formed.

    StartingMessage - Supplies a starting message number for a range of
        messages that this policy applies to.

    EndingMessage - Supplies an ending message number for a range of messages
        that this policy applies to.

    MessagesPerProcessor - Supplies the number of messages targeted at a single
        processor.

    GroupPolicy - Supplies the group affinity policy.

    Iterator - Supplies a place holder for a context maintained through multiple
        calls to this routine to get the policy for different messages for the
        same device.

    Group - Supplies a place holder for a processor group number.

    ProcessorAffinity - Supplies a place holder for an affinity mask for a
        given message as computed on succesful exit from this routine.

Return Value:

    NT status value.

--*/

{

    KAFFINITY Affinity;
    UCHAR CurrentProcessor;
    UCHAR NextProcessor;
    ULONG NodeIndex;
    ULONG ProcCount;
    ULONG ProcessorNodeIndex;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(EndingMessage);
    PAGED_CODE();

    //
    // Get a count and active set of processors in the system. Refetch the NUMA
    // information if the topology has changed.
    //

    ProcCount = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
    PciAcquirePassiveLock(&PciNumaInformationLock);
    if (ProcCount != PciProcessorCount) {
        PciFreeNumaInformation();
        Status = PciGetNumaInformation();
        if (!NT_SUCCESS(Status)) {
            goto Exit;
        }
    }

    //
    // Compute a 0-based processor-node index for a given message.
    //

    PCI_ASSERT((MessageNumber >= StartingMessage) &&
               (MessageNumber <= EndingMessage));

    if (MessagesPerProcessor == 0) {
        MessagesPerProcessor = 1;
    }

    ProcessorNodeIndex =
        ((MessageNumber - StartingMessage) / MessagesPerProcessor);

    if (MessageNumber == 0) {
        *Iterator = PciRoundRobinIndex;
    }

    ProcessorNodeIndex += *Iterator;

    PCI_ASSERT(PciNumaNodeCount >= 1);
    PCI_ASSERT(PciGroupZeroNumaNodeCount >= 1);

    __analysis_assume(PciNumaNodeCount >= 1);
    __analysis_assume(PciGroupZeroNumaNodeCount >= 1);

    //
    // All NUMA nodes in group 0 appear first in processor mask and
    // current processor arrays.
    //

    if (GroupPolicy == GroupAffinityAllGroupZero) {
        NodeIndex = ProcessorNodeIndex % PciGroupZeroNumaNodeCount;

    } else {
        NodeIndex = ProcessorNodeIndex % PciNumaNodeCount;
    }

    CurrentProcessor = PciRoundRobinCurrentProcessor[NodeIndex];
    *Group = PciNumaNodesProcessorMask[NodeIndex].Group;
    *ProcessorAffinity = AFFINITY_MASK(CurrentProcessor);

    //
    // Update the current processor on this node.
    //

    Affinity = PciNumaNodesProcessorMask[NodeIndex].Mask;
    Affinity &= ~((2 * AFFINITY_MASK(CurrentProcessor)) - 1);
    if (Affinity == 0) {
        Affinity = PciNumaNodesProcessorMask[NodeIndex].Mask;
    }

    PCI_ASSERT(Affinity != 0);
#pragma prefast(suppress: __WARNING_MUST_USE, "This can't fail because Affinity can't be 0")
    NextProcessor = RtlFindLeastSignificantBit(Affinity);
    PciRoundRobinCurrentProcessor[NodeIndex] = NextProcessor;
    PciRoundRobinIndex += 1;
    if (PciRoundRobinIndex == PciNumaNodeCount) {
        PciRoundRobinIndex = 0;
    }

    Status = STATUS_SUCCESS;

Exit:
    PciReleasePassiveLock(&PciNumaInformationLock);
    return Status;
}

// ------------------------------------------------- Interrupt Routing Interface

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciRoutingInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    Initialize the INT_ROUTE_INTERFACE_STANDARD fields.

Arguments:

    DeviceExtension - An extension pointer.

    InterfaceSpecificData - Not used.

    Version - Interface version.

    Size - Size of the INT_ROUTE_INTERFACE_STANDARD interface object.

    InterfaceReturn - The interface object pointer to be filled in.

    Filtered - Not supported for this interface.

Return Value:

    Returns NTSTATUS.

--*/
{
    PINT_ROUTE_INTERFACE_STANDARD standard;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(INT_ROUTE_INTERFACE_STANDARD)) {
        return STATUS_UNSUCCESSFUL;
    }

    standard = (PINT_ROUTE_INTERFACE_STANDARD)InterfaceReturn;

    standard->Size = sizeof(INT_ROUTE_INTERFACE_STANDARD);
    standard->Version = PCI_INT_ROUTE_INTRF_STANDARD_VER;
    standard->Context = DeviceExtension;
    standard->InterfaceReference = PciRoutingInterface_Reference;
    standard->InterfaceDereference = PciRoutingInterface_Dereference;

    standard->GetInterruptRouting = PciRoutingInterface_GetRoutingInfo;
    standard->UpdateInterruptLine = PciRoutingInterface_UpdateInterruptLine;


    return STATUS_SUCCESS;
}

VOID
PciRoutingInterface_Reference(
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

    InterlockedIncrement(&bus->PciRoutingInterfaceCount);

    return;
}

VOID
PciRoutingInterface_Dereference(
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

    InterlockedDecrement(&bus->PciRoutingInterfaceCount);

    return;
}

NTSTATUS
PciRoutingInterface_GetRoutingInfo(
    __in PDEVICE_OBJECT Pdo,
    __inout ULONG *Bus,
    __inout ULONG *PciSlot,
    __out_opt UCHAR *InterruptLine,
    __out_opt UCHAR *InterruptPin,
    __out_opt UCHAR *ClassCode,
    __out_opt UCHAR *SubClassCode,
    __out_opt PDEVICE_OBJECT *ParentPdo,
    __out_opt UCHAR *Flags,
    __out_opt UCHAR *PhantomFunctionBits,
    __out_opt PCI_BUSMASTER_DESCRIPTOR *InitiatingRidDescriptor
    )
/*++

Routine Description:

    Each PCI device in the system is connected to some
    interrupt pin.  And in order to figure out which interrupt
    that device may trigger, an IRQ arbiter must have this
    information.  This interface gathers all such information
    for the arbiter.

Arguments:

    Pdo - Device object that the arbiter needs to inquire about

    Bus - Number of the PCI bus in question

    PciSlot - Slot/Function that corresponds to this device

    InterruptLine - Contents of the device's interrupt line register

    InterruptPin - Contents of the device's interrupt pin register

    ClassCode - type of device

    SubClassCode - sub-type of device

    ParentPdo - PDO of parent PCI bus

    Flags - Various flags

    PhantomFunctionBits - Number of bits which a PCI Express device may vary in
        its own Requester ID (RID) when putting transactions on the bus

    InitiatingRidDescriptor - Describes the set of RIDs which may be in use
        by a device, as observed at the IOMMU.  This is complicated because
        conversions between PCI Express and PCI-X may involve changing the RID
        as transactions flow through the fabric.

Return Value:

    STATUS_SUCCESS   - this is a PCI device and all the fields have been filled
    STATUS_NOT_FOUND - to the knowledge of the PCI driver, this is not a PCI
        device

--*/
{
    PPCI_DEVICE device;
    PPCI_BUS parent;
    PCI_HARDWARE_INTERFACE secondaryInterface;
    NTSTATUS status;

    PAGED_CODE();

    //
    // Flags are not currently used.
    //
    if (ARGUMENT_PRESENT(Flags)) {
        *Flags = 0;
    }

    //
    // Check to see if this is a legacy PCI device that
    // we are tracking. If so, PciGetLegacyDeviceRoutingInfo fills
    // in the fields correctly.
    //
    status = PciGetLegacyDeviceRoutingInfo(
                Pdo,
                Bus,
                PciSlot,
                InterruptLine,
                InterruptPin,
                ClassCode,
                SubClassCode,
                ParentPdo
                );
    if (NT_SUCCESS(status)) {
        return status;
    }

    //
    // Verify that this PDO actually belongs to us.
    //

    if (PCI_DEVICE_OBJECT(Pdo) == FALSE) {
        return STATUS_NOT_FOUND;
    }

    device = (PPCI_DEVICE)Pdo->DeviceExtension;
    if (!PCI_DEVICE_EXTENSION(device)) {
        return STATUS_NOT_FOUND;
    }

    if (ARGUMENT_PRESENT(Bus)) {
        *Bus = device->BusNumber;
    }

    if (ARGUMENT_PRESENT(PciSlot)) {
        *PciSlot = device->Slot.u.AsULONG;
    }

    if (ARGUMENT_PRESENT(InterruptLine)) {
        *InterruptLine = device->SavedRegisters.InterruptLine;
    }

    if (ARGUMENT_PRESENT(InterruptPin)) {
        *InterruptPin = device->InterruptPin;
    }

    if (ARGUMENT_PRESENT(ClassCode)) {
        *ClassCode = device->BaseClass;
    }

    if (ARGUMENT_PRESENT(SubClassCode)) {
        *SubClassCode = device->SubClass;
    }

    if (ARGUMENT_PRESENT(ParentPdo)) {
        *ParentPdo = device->Parent->PhysicalDeviceObject;
    }

    if (ARGUMENT_PRESENT(PhantomFunctionBits)) {

        if (device->ExpressPort != NULL) {
            *PhantomFunctionBits =
                (UCHAR)device->ExpressPort->DeviceCapabilities.PhantomFunctionsSupported;
        } else {
            *PhantomFunctionBits = 0;
        }
    }

    if (ARGUMENT_PRESENT(InitiatingRidDescriptor)) {

        //
        // PCI devices don't support split transactions, and thus they need
        // no "tag" on the bus to specify where the completion goes.  PCI-X
        // devices sometimes operate in a mode where they do support split
        // transactions, and thus there is a tag, with the originating Requester
        // ID (RID.)  PCI Express devices also support split transactions,
        // with the originating RID.  This allows PCI-X and PCIe devices to
        // go through translation at an IOMMU.
        //
        // A problem comes up when PCI-X or PCI devices have some parent or
        // grandparent that is a PCIe to PCI-X bridge.  PCI-X is allowed to
        // burst essentially unlimited amounts of data, at least until the bus
        // arbiter takes away the device's right to master.  PCIe devices are
        // limited to 4K, page aligned.  Thus it's possible that a PCI(-X)
        // device may try to burst across a 4K page boundary and the upstream
        // bridge needs to buffer that up and initiate multiple upstream
        // transactions that correspond to a single downstream transaction.
        //
        // When this occurs, the PCIe to PCI-X bridge is allowed to substitute
        // its own RID in place of the downstream device's RID.  In fact, it's
        // allowed to substitute its own RID any time it feels like it.  So,
        // if there is a PCIe to PCI-X bridge in the path to the IOMMU, there
        // could be a number of RIDs associated with the actual endpoint device
        // by the time the transaction reaches the IOMMU, particularly if there
        // are multiple such bridges.
        //
        // As usual, the "root" bus is special.  In systems with an IOMMU, the
        // root bus gets tagged even if the devices themselves seem to be legacy
        // PCI.  Such devices are actually internal.  On machines that are old
        // enough to have real physical PCI devices on bus 0, it doesn't matter
        // if we report them as tagged, as there is no IOMMU.
        //
        // Devices which are behind legacy PCI bridges (again in systems with
        // an IOMMU) will be tagged with the RID of the bridge, no exceptions.
        //
        // The InitiatingRidDescriptor packages up enough information to
        // program the IOMMU correctly.
        //
        // Walk up the chain of upstream bridges until the point where it joins
        // the Express hierarchy.  Everything further upstream of that bridge
        // is Express and will pass RIDs through.  The type of that bridge (-E,
        // -C, or -X) determines the RID(s) that will be sent to the IOMMU.
        //

        parent = device->Parent;
        if (PCI_ROOT_BUS(parent)) {
            secondaryInterface = PciExpress;

        } else {
            while (!parent->PciBridge->PathIsExpress) {
                parent = parent->ParentBus;
            }

            secondaryInterface = parent->SecondaryInterface;
        }

        RtlZeroMemory(InitiatingRidDescriptor,
                      sizeof(*InitiatingRidDescriptor));

        InitiatingRidDescriptor->Segment =
            SEGMENT_NUMBER_FOR_BUS(device->BusNumber);

        if (secondaryInterface == PciExpress) {

            //
            // The entire path to the device is Express or it is on the root
            // bus.  Fill in a fully qualified descriptor, indicating that the
            // device's RID will always be used.
            //

            InitiatingRidDescriptor->Type = BusmasterRidFromDeviceRid;
            InitiatingRidDescriptor->DeviceRid.Bus =
                LOCAL_BUS_NUMBER(device->BusNumber);

            InitiatingRidDescriptor->DeviceRid.Device =
                (UCHAR)device->Slot.u.bits.DeviceNumber;

            InitiatingRidDescriptor->DeviceRid.Function =
                (UCHAR)device->Slot.u.bits.FunctionNumber;

        } else if (secondaryInterface == PciConventional) {

            //
            // A -C bridge.  If the bridge is an actual E-C bridge, then it
            // follows PCIe bridge rules and the RID it uses for all
            // transactions forwarded upstream is {B=bridge's secondary bus
            // number,D=0,F=0}.  This is the case if the bridge is somewhere
            // beneath a root port.  Otherwise, consider the bridge to be a
            // RC-integrated -C bridge, for which the RID is the BDF of the
            // bridge - this comes from Intel/AMD IOMMU specs which both happen
            // to require the same behavior.
            //

            InitiatingRidDescriptor->Type = BusmasterRidFromBridgeRid;
            if (parent->PciBridge->RootPortDomain) {
                InitiatingRidDescriptor->BridgeRid.Bus =
                    LOCAL_BUS_NUMBER(parent->SecondaryBusNumber);

                InitiatingRidDescriptor->BridgeRid.Device = 0;
                InitiatingRidDescriptor->BridgeRid.Function = 0;

            } else {
                InitiatingRidDescriptor->BridgeRid.Bus =
                    LOCAL_BUS_NUMBER(parent->ParentBus->SecondaryBusNumber);

                InitiatingRidDescriptor->BridgeRid.Device =
                    (UCHAR)parent->PciBridge->Slot.u.bits.DeviceNumber;

                InitiatingRidDescriptor->BridgeRid.Function =
                    (UCHAR)parent->PciBridge->Slot.u.bits.FunctionNumber;
            }

        } else {

            //
            // An -X bridge.  Fill in the bus range, indicating all the RIDs
            // that might appear under that bridge.  This assumes RC-integrated
            // bridges behave the same as actual E-X bridges (Intel/AMD don't
            // document behavior for RC-integrated -X bridges, and they are
            // unlikely to even exist on machines with an IOMMU).
            //

            InitiatingRidDescriptor->Type = BusmasterRidFromMultipleBridges;
            InitiatingRidDescriptor->MultipleBridges.SecondaryBus =
                (UCHAR)parent->SecondaryBusNumber;

            InitiatingRidDescriptor->MultipleBridges.SubordinateBus =
                (UCHAR)parent->SubordinateBusNumber;
        }
    }

    return STATUS_SUCCESS;
}

VOID
PciRoutingInterface_UpdateInterruptLine(
    __in PDEVICE_OBJECT Pdo,
    __in UCHAR Line
    )
/*++

Routine Description:

    This routine updates the interrupt assignment of the given PDO.
    Since there is a register in PCI config space that is supposed
    to reflect the device's interrupt assignment (and since some
    device's rely on the register being correct), this routine
    writes the updated value out to hardware.

Arguments:

    Pdo - Device object that the IRQ arbiter is working with

    Line - The updated interrupt line.

Return Value:

    VOID

--*/
{
    PPCI_DEVICE device;
    PLEGACY_DEVICE legacyDev;

    PAGED_CODE();

    //
    // Work out if this is a legacy PDO or not
    //
    device = NULL;
    legacyDev = PciLegacyDeviceHead;
    while (legacyDev) {

        if (legacyDev->LegacyDeviceObject == Pdo) {

            device = legacyDev->DeviceExtension;
            break;
        }

        legacyDev = (PLEGACY_DEVICE)legacyDev->List.Next;
    }

    if (device == NULL) {

        //
        // Ensure that this Pdo belongs to PCI.
        //

        device = Pdo->DeviceExtension;
        if (!PCI_DEVICE_EXTENSION(device)) {
            return;
        }
    }

    //
    // Now we can update the hardware and our internal state!
    //
    device->SavedRegisters.InterruptLine = Line;
    device->InterruptResource.InterruptLine = Line;

    //
    // Touching config space while a device is in D3Cold
    // is illegal. The InterruptLine register will be restored
    // properly when the device is repowered on.
    //

    if (PCI_DEVICE_IN_D3COLD(device) == FALSE) {
        PciWriteConfigRegister(device, common.InterruptLine, &Line);
    }

    return;
}

// --------------------------------------------------- MSI-X Table Modification

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciMsiXInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    This routine initializes the MSI-X table configuration interface.

Arguments:

    DeviceExtension - Supplies the device extension pointer.

    InterfaceSpecificData - Not used.

    Version - Supplies the interface version.

    Size - Supplies the size of the provided interface buffer.

    InterfaceReturn - Supplies a pointer to a variable that receives the
        interface.

    Filtered - Not supported for this interface.

Return Value:

    NT Status value.

--*/

{

    PPCI_MSIX_TABLE_CONFIG_INTERFACE Interface;
    USHORT OutputSize;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < PCI_MSIX_TABLE_CONFIG_MINIMUM_SIZE) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Fill in the required portion of the interface.
    //

    Interface = (PPCI_MSIX_TABLE_CONFIG_INTERFACE)InterfaceReturn;
    Interface->Version = PCI_MSIX_TABLE_CONFIG_INTERFACE_VERSION;
    Interface->Context = DeviceExtension;
    Interface->InterfaceReference = PciMsiXInterface_RefDeref;
    Interface->InterfaceDereference = PciMsiXInterface_RefDeref;
    Interface->SetTableEntry = PciMsiXInterface_SetTableEntry;
    Interface->MaskTableEntry = PciMsiXInterface_MaskTableEntry;
    Interface->UnmaskTableEntry = PciMsiXInterface_UnmaskTableEntry;
    OutputSize = PCI_MSIX_TABLE_CONFIG_MINIMUM_SIZE;

    //
    // If enough buffer space is supplied, fill in additional interface
    // routines.
    //

    if (Size >= RTL_SIZEOF_THROUGH_FIELD(PCI_MSIX_TABLE_CONFIG_INTERFACE,
                                         GetTableEntry)) {

        Interface->GetTableEntry = PciMsiXInterface_GetTableEntry;
        OutputSize = RTL_SIZEOF_THROUGH_FIELD(PCI_MSIX_TABLE_CONFIG_INTERFACE,
                                              GetTableEntry);
    }

    if (Size >= RTL_SIZEOF_THROUGH_FIELD(PCI_MSIX_TABLE_CONFIG_INTERFACE,
                                         GetTableSize)) {

        Interface->GetTableSize = PciMsiXInterface_GetTableSize;
        OutputSize = RTL_SIZEOF_THROUGH_FIELD(PCI_MSIX_TABLE_CONFIG_INTERFACE,
                                              GetTableSize);
    }

    Interface->Size = OutputSize;
    return STATUS_SUCCESS;
}

NTSTATUS
PciMsiXInterface_RefDeref (
    __in PVOID Context
    )

/*++

Routine Description:

    This routine is a no-op interface reference/dereference routine for the
    MSI-X table configuration interface.

Arguments:

    Context - Unused.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Context);

    return STATUS_SUCCESS;
}

NTSTATUS
PciMsiXInterface_SetTableEntry (
    __in PVOID Context,
    __in ULONG TableEntry,
    __in ULONG MessageNumber
    )

/*++

Routine Description:

    This routine sets an MSI-X table entry.

Arguments:

    Context - Supplies the interface context.  This is the relevant device
        extension.

    TableEntry - Supplies the index of the table entry to set.

    MessageNumber - Supplies the index in the device's allocated message array
        to assign to this entry.

Return Value:

    STATUS_SUCCESS if the set is successful.

    STATUS_INVALID_PARAMETER if the device is invalid, or the table entry is
        out of bounds.

--*/

{

    PHYSICAL_ADDRESS Address;
    ULONG DataPayload;
    PPCI_DEVICE Device;
    PCI_MSIX_TABLE_ENTRY Entry;
    ULONG EntryMask;
    ULONG HardwareMessages;
    ULONG MessagesGranted;
    NTSTATUS Status;
    PPCI_MSIX_TABLE_ENTRY Table;
    ULONG UlongCount;

    Table = NULL;

    //
    // Validate parameters to ensure that the device is using MSI-X and the
    // entry numbers are valid message numbers for this device.
    //

    Device = (PPCI_DEVICE)Context;
    if (Device->Signature != PCI_DEVICE_SIGNATURE) {
        return STATUS_INVALID_PARAMETER;
    }

    if (Device->InterruptResource.Type != PciMsiX) {
        return STATUS_INVALID_PARAMETER;
    }

    HardwareMessages = Device->InterruptRequirement.Message.MessagesRequested;
    if (TableEntry >= HardwareMessages) {
        return STATUS_INVALID_PARAMETER;
    }

    MessagesGranted = Device->InterruptResource.MsiX.MessagesGranted;
    if (MessageNumber >= MessagesGranted) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Get the address and data corresponding to the message number to set.
    //

    Status = PciProcessInterruptMessageData(
                 Device,
                 Device->InterruptResource.MsiX.ConnectionData,
                 MessageNumber,
                 &Address,
                 &DataPayload);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Store the mapping between the table entry and message number in the
    // device extension.
    //

    EntryMask = Device->InterruptResource.MsiX.EntryMap[TableEntry] &
                PCI_MSIX_ENTRY_MASKED;

    Device->InterruptResource.MsiX.EntryMap[TableEntry] =
        MessageNumber | EntryMask;

    //
    // Read the table entry and set in the address and data corresponding to the
    // new message.
    //

    UlongCount = sizeof(PCI_MSIX_TABLE_ENTRY) / sizeof(ULONG);
    Table = Device->InterruptResource.MsiX.Table;
    READ_REGISTER_BUFFER_ULONG((PULONG)&Table[TableEntry],
                               (PULONG)&Entry,
                               UlongCount);

    Entry.MessageAddress.QuadPart = Address.QuadPart;
    Entry.MessageData = DataPayload;

    //
    // Devices are allowed to cache message address and data as long as a
    // message is unmasked.  If the entry is unmasked, set the mask bit and
    // write the entry before rewriting it with the mask bit clear.
    //

    if ((EntryMask & PCI_MSIX_ENTRY_MASKED) == 0) {
        Entry.VectorControl.Mask = 1;
        WRITE_REGISTER_BUFFER_ULONG((PULONG)&Table[TableEntry],
                                    (PULONG)&Entry,
                                    UlongCount);

        Entry.VectorControl.Mask = 0;
    }

    WRITE_REGISTER_BUFFER_ULONG((PULONG)&Table[TableEntry],
                                (PULONG)&Entry,
                                UlongCount);

    return STATUS_SUCCESS;
}

NTSTATUS
PciMsiXInterface_MaskTableEntry (
    __in PVOID Context,
    __in ULONG TableEntry
    )

/*++

Routine Description:

    This routine masks an MSI-X table entry.

Arguments:

    Context - Supplies the interface context.  This is the relevant device
        extension.

    TableEntry - Supplies the index of the table entry to mask.

Return Value:

    STATUS_SUCCESS if the mask is successful.

    STATUS_INVALID_PARAMETER if the device is invalid, or the table entry is
        out of bounds.

--*/

{

    PPCI_DEVICE Device;
    PCI_MSIX_TABLE_ENTRY Entry;
    ULONG HardwareMessages;
    PPCI_MSIX_TABLE_ENTRY Table;
    ULONG UlongCount;

    //
    // Validate parameters to ensure that the device is using MSI-X and the
    // entry numbers are valid message numbers for this device.
    //

    Device = (PPCI_DEVICE)Context;
    if (Device->Signature != PCI_DEVICE_SIGNATURE) {
        return STATUS_INVALID_PARAMETER;
    }

    if (Device->InterruptResource.Type != PciMsiX) {
        return STATUS_INVALID_PARAMETER;
    }

    HardwareMessages = Device->InterruptRequirement.Message.MessagesRequested;
    if (TableEntry >= HardwareMessages) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Store the mask state in the device extension entry map.
    //

    Device->InterruptResource.MsiX.EntryMap[TableEntry] |= PCI_MSIX_ENTRY_MASKED;

    //
    // Read the table entry and set in the address and data corresponding to the
    // new message.
    //

    UlongCount = sizeof(PCI_MSIX_TABLE_ENTRY) / sizeof(ULONG);
    Table = Device->InterruptResource.MsiX.Table;
    READ_REGISTER_BUFFER_ULONG((PULONG)&Table[TableEntry],
                               (PULONG)&Entry,
                               UlongCount);

    Entry.VectorControl.Mask = 1;
    WRITE_REGISTER_BUFFER_ULONG((PULONG)&Table[TableEntry],
                                (PULONG)&Entry,
                                UlongCount);
    return STATUS_SUCCESS;
}

NTSTATUS
PciMsiXInterface_UnmaskTableEntry (
    __in PVOID Context,
    __in ULONG TableEntry
    )

/*++

Routine Description:

    This routine unmasks an MSI-X table entry.

Arguments:

    Context - Supplies the interface context.  This is the relevant device
        extension.

    TableEntry - Supplies the index of the table entry to unmask.

Return Value:

    STATUS_SUCCESS if the unmask is successful.

    STATUS_INVALID_PARAMETER if the device is invalid, or the table entry is
        out of bounds.

--*/

{

    PPCI_DEVICE Device;
    PCI_MSIX_TABLE_ENTRY Entry;
    ULONG HardwareMessages;
    PPCI_MSIX_TABLE_ENTRY Table;
    ULONG UlongCount;

    //
    // Validate parameters to ensure that the device is using MSI-X and the
    // entry numbers are valid message numbers for this device.
    //

    Device = (PPCI_DEVICE)Context;
    if (Device->Signature != PCI_DEVICE_SIGNATURE) {
        return STATUS_INVALID_PARAMETER;
    }

    if (Device->InterruptResource.Type != PciMsiX) {
        return STATUS_INVALID_PARAMETER;
    }

    HardwareMessages = Device->InterruptRequirement.Message.MessagesRequested;
    if (TableEntry >= HardwareMessages) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Store the mask state in the device extension entry map.
    //

    Device->InterruptResource.MsiX.EntryMap[TableEntry] &=
        ~PCI_MSIX_ENTRY_MASKED;

    //
    // Read the table entry and set in the address and data corresponding to the
    // new message.
    //

    UlongCount = sizeof(PCI_MSIX_TABLE_ENTRY) / sizeof(ULONG);
    Table = Device->InterruptResource.MsiX.Table;
    READ_REGISTER_BUFFER_ULONG((PULONG)&Table[TableEntry],
                               (PULONG)&Entry,
                               UlongCount);

    Entry.VectorControl.Mask = 0;
    WRITE_REGISTER_BUFFER_ULONG((PULONG)&Table[TableEntry],
                                (PULONG)&Entry,
                                UlongCount);

    return STATUS_SUCCESS;
}

NTSTATUS
PciMsiXInterface_GetTableEntry (
    __in PVOID Context,
    __in ULONG TableEntry,
    __out PULONG MessageNumber,
    __out PBOOLEAN Masked
    )

/*++

Routine Description:

    This routine returns the message number currently mapped to the given table
    entry.

Arguments:

    Context - Supplies the interface context.  This is the relevant device
        extension.

    TableEntry - Supplies the index of the table entry to query.

    MessageNumber - Supplies a pointer to a variable that receives the index in
        the device's allocated message array that is assigned to this entry.

    Masked - Supplies a pointer to a boolean that receives TRUE if the entry is
        currently masked, and FALSE if it is unmasked.

Return Value:

    STATUS_SUCCESS if the query is successful.

    STATUS_INVALID_PARAMETER if the device is invalid, or the table entry is
        out of bounds.

--*/

{

    PPCI_DEVICE Device;
    ULONG Entry;
    ULONG HardwareMessages;

    //
    // Validate parameters to ensure that the device is using MSI-X and the
    // entry numbers are valid message numbers for this device.
    //

    Device = (PPCI_DEVICE)Context;
    if (Device->Signature != PCI_DEVICE_SIGNATURE) {
        return STATUS_INVALID_PARAMETER;
    }

    if (Device->InterruptResource.Type != PciMsiX) {
        return STATUS_INVALID_PARAMETER;
    }

    HardwareMessages = Device->InterruptRequirement.Message.MessagesRequested;
    if (TableEntry >= HardwareMessages) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Retrieve the mapping between the table entry and message number from the
    // device extension.
    //

    Entry = Device->InterruptResource.MsiX.EntryMap[TableEntry];
    *MessageNumber = Entry & ~PCI_MSIX_ENTRY_MASKED;
    if ((Entry & PCI_MSIX_ENTRY_MASKED) != 0) {
        *Masked = TRUE;

    } else {
        *Masked = FALSE;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciMsiXInterface_GetTableSize (
    __in PVOID Context,
    __out PULONG TableSize
    )

/*++

Routine Description:

    This routine returns the size of the MSI-X hardware table.

Arguments:

    Context - Supplies the interface context.  This is the relevant device
        extension.

    TableSize - Supplies a pointer to a variable that receives the size of the
        MSI-X table in the device.

Return Value:

    STATUS_SUCCESS if the routine is successful.

    STATUS_INVALID_PARAMETER if the device is invalid.

--*/

{

    PPCI_DEVICE Device;

    //
    // Validate parameters to ensure that the device is using MSI-X and the
    // entry numbers are valid message numbers for this device.
    //

    Device = (PPCI_DEVICE)Context;
    if (Device->Signature != PCI_DEVICE_SIGNATURE) {
        return STATUS_INVALID_PARAMETER;
    }

    if (Device->InterruptResource.Type != PciMsiX) {
        return STATUS_INVALID_PARAMETER;
    }

    *TableSize = Device->InterruptRequirement.Message.MessagesRequested;
    return STATUS_SUCCESS;

}

// ------------------------------------------------------ Legacy Device Routing

//
//  NT 5.0 has to support non-PnP 4.0-style device drivers.  And
//  this driver doesn't create the device objects associated with
//  a PCI device when its driver is 4.0-style.  It does, however
//  get a chance to look at those objects when the driver calls
//  HalAssignSlotResources.  This collection of functions tracks
//  these foreign device objects for the purposes of interrupt routing.
//

NTSTATUS
PciCacheLegacyDeviceRouting(
    __in PDEVICE_OBJECT LegacyDO,
    __in ULONG Bus,
    __in ULONG PciSlot,
    __in UCHAR InterruptLine,
    __in UCHAR InterruptPin,
    __in UCHAR ClassCode,
    __in UCHAR SubClassCode,
    __in PDEVICE_OBJECT ParentPdo,
    __in PPCI_DEVICE DeviceExtension,
    __out_opt PDEVICE_OBJECT *OldLegacyDO
    )
/*++

Routine Description:

    This function stores all the routing data for a legacy
    device object, except the RoutingToken. Caller needs to acquire
    PciGlobalLock before calling this function.

Arguments:

    LegacyDO - Device object that the arbiter needs to inquire about

    Bus - Number of the PCI bus in question

    PciSlot - Slot/Function that corresponds to this device

    InterruptLine - Contents of the device's interrupt line register

    InterruptPin - Contents of the device's interrupt pin register

    ClassCode - type of device

    SubClassCode - sub-type of device

    ParentPdo - PDO of parent PCI bus

    DeviceExtension - The PCI device extension corresponding to the PDO
        (note that this is not the same as LegacyDO->DeviceExtension, since
        PCI did not create the legacy DO itself.)

    OldLegacyDO - Any device object that was previously cached with the
        same bus and slot numbers.  this is returned so that if there's
        an error in some later part of the caching process, the cache
        can be rolled back.

Return Value:

    Returns the status of this operation.

--*/
{
    PLEGACY_DEVICE legacyDev;
    PLEGACY_DEVICE lastLegacyDev;

    PAGED_CODE();

    if (ARGUMENT_PRESENT(OldLegacyDO)) {
        *OldLegacyDO = NULL;
    }

    legacyDev = PciLegacyDeviceHead;
    lastLegacyDev = (PLEGACY_DEVICE)&PciLegacyDeviceHead;
    while (legacyDev) {

        //
        // If the bus and slot match, save away the old device object
        // cached here and update the structure with the new device object.
        //
        if ((Bus == legacyDev->Bus) && (PciSlot == legacyDev->PciSlot)) {

            if (ARGUMENT_PRESENT(OldLegacyDO)) {

                *OldLegacyDO = legacyDev->LegacyDeviceObject;
            }

            legacyDev->LegacyDeviceObject = LegacyDO;

            return STATUS_SUCCESS;

        } else if (legacyDev->LegacyDeviceObject == LegacyDO) {
            //
            // This is a device on the list with different
            // PCI Bus/Dev values and the same DO pointer.
            // This must be stale.  Delete it.
            //

            lastLegacyDev->List.Next = legacyDev->List.Next;
            PciFreePool(legacyDev);
            legacyDev = (PLEGACY_DEVICE)lastLegacyDev->List.Next;
            continue;
        }

        lastLegacyDev = legacyDev;
        legacyDev = (PLEGACY_DEVICE)legacyDev->List.Next;
    }

    //
    // The device isn't here.  Allocate a new entry for it.
    //
    legacyDev = PciAllocatePool(PagedPool, sizeof(LEGACY_DEVICE));
    if (!legacyDev) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(legacyDev, sizeof(LEGACY_DEVICE));

    legacyDev->LegacyDeviceObject   = LegacyDO;
    legacyDev->Bus                  = Bus;
    legacyDev->PciSlot              = PciSlot;
    legacyDev->InterruptLine        = InterruptLine;
    legacyDev->InterruptPin         = InterruptPin;
    legacyDev->ClassCode            = ClassCode;
    legacyDev->SubClassCode         = SubClassCode;
    legacyDev->ParentPdo            = ParentPdo;
    legacyDev->DeviceExtension      = DeviceExtension;

    //
    // Push this one onto the list.
    //
    legacyDev->List.Next = (PSINGLE_LIST_ENTRY)PciLegacyDeviceHead;
    PciLegacyDeviceHead = legacyDev;

    if (ARGUMENT_PRESENT(OldLegacyDO)) {

        *OldLegacyDO = LegacyDO;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciGetLegacyDeviceRoutingInfo(
    __in PDEVICE_OBJECT LegacyDO,
    __inout ULONG *Bus,
    __inout ULONG *PciSlot,
    __out_opt UCHAR *InterruptLine,
    __out_opt UCHAR *InterruptPin,
    __out_opt UCHAR *ClassCode,
    __out_opt UCHAR *SubClassCode,
    __out_opt PDEVICE_OBJECT *ParentPdo
    )
/*++

Routine Description:

    This function returns all the routing data for a legacy device object.

Arguments:

    LegacyDO - Device object that the arbiter needs to inquire about

    Bus - Number of the PCI bus in question

    PciSlot - Slot/Function that corresponds to this device

    InterruptLine - Contents of the device's interrupt line register

    InterruptPin - Contents of the device's interrupt pin register

    ClassCode - type of device

    SubClassCode - sub-type of device

    ParentPdo - PDO of parent PCI bus

Return Value:

    Returns the status of this operation.

--*/
{
    PLEGACY_DEVICE legacyDev;
    NTSTATUS status;

    PAGED_CODE();

    status = STATUS_NOT_FOUND;

    legacyDev = PciLegacyDeviceHead;
    while (legacyDev) {

        if (legacyDev->LegacyDeviceObject == LegacyDO) {
            break;
        }

        if ((legacyDev->Bus == *Bus) && (legacyDev->PciSlot == *PciSlot)) {

            if (legacyDev->LegacyDeviceObject == NULL) {

                //
                // Cache the LegacyDO in case we matched on the bus and slot info.
                //
                legacyDev->LegacyDeviceObject = LegacyDO;
                break;

            } else {

                ERROR("Two PDOs (Legacy = %p, Pnp = %p) for device on bus %08x, slot %08x\n",
                      legacyDev->LegacyDeviceObject, LegacyDO, *Bus, *PciSlot
                      );
                PCI_ASSERT(legacyDev->LegacyDeviceObject != NULL);
                legacyDev = NULL;
                break;
            }
        }

        legacyDev = (PLEGACY_DEVICE)legacyDev->List.Next;
    }

    if (legacyDev) {

        if (ARGUMENT_PRESENT(Bus)) {
            *Bus = legacyDev->Bus;
        }

        if (ARGUMENT_PRESENT(PciSlot)) {
            *PciSlot = legacyDev->PciSlot;
        }

        if (ARGUMENT_PRESENT(InterruptLine)) {
            *InterruptLine = legacyDev->InterruptLine;
        }

        if (ARGUMENT_PRESENT(InterruptPin)) {
            *InterruptPin = legacyDev->InterruptPin;
        }

        if (ARGUMENT_PRESENT(ClassCode)) {
            *ClassCode = legacyDev->ClassCode;
        }

        if (ARGUMENT_PRESENT(SubClassCode)) {
            *SubClassCode = legacyDev->SubClassCode;
        }

        if (ARGUMENT_PRESENT(ParentPdo)) {
            *ParentPdo = legacyDev->ParentPdo;
        }

        status = STATUS_SUCCESS;
    }

    return status;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciDeleteVolatileInterruptRoutingData (
    _In_ PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine deletes any interrupt routing data that the interrupt arbiter
    has cached (for performance reasons) about this device.  That way the
    interrupt arbiter will be forced to re-work-out the interrupt routing if
    the device reappears in the machine.  This might be necessary in several
    situations, most notably moving an Express device with a serial number from
    one slot to another.  (Serial numbers are used for the device instance path
    and thus don't change when the device is moved.)

Arguments:

    Device - The device extension for this PDO.

Return Value:

    Returns the status of this operation.

--*/

{
    HANDLE IntHandle;
    OBJECT_ATTRIBUTES ObjectAttributes;
    HANDLE RootHandle;
    HANDLE RoutingHandle;
    NTSTATUS Status;
    UNICODE_STRING UnicodeString;

    PAGED_CODE();
    PCI_ASSERT(PCI_DEVICE_EXTENSION(Device));

    Status = IoOpenDeviceRegistryKey(Device->DeviceObject,
                                     PLUGPLAY_REGKEY_DEVICE,
                                     KEY_ALL_ACCESS,
                                     &RootHandle);

    if (!NT_SUCCESS(Status)) {
        return;
    }

    RtlInitUnicodeString(&UnicodeString, L"Interrupt Management");
    InitializeObjectAttributes(&ObjectAttributes,
                               &UnicodeString,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               RootHandle,
                               (PSECURITY_DESCRIPTOR)NULL);

    Status = ZwCreateKey(&IntHandle,
                         KEY_ALL_ACCESS,
                         &ObjectAttributes,
                         0,
                         (PUNICODE_STRING)NULL,
                         REG_OPTION_NON_VOLATILE,
                         NULL);

    ZwClose(RootHandle);
    if (!NT_SUCCESS(Status)) {
        return;
    }

    RtlInitUnicodeString(&UnicodeString, L"Routing Info");
    InitializeObjectAttributes(&ObjectAttributes,
                               &UnicodeString,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               IntHandle,
                               (PSECURITY_DESCRIPTOR)NULL);

    Status = ZwOpenKey(&RoutingHandle,
                       KEY_ALL_ACCESS,
                       &ObjectAttributes);

    ZwClose(IntHandle);

    if (NT_SUCCESS(Status)) {
        ZwDeleteKey(RoutingHandle);
        ZwClose(RoutingHandle);
    }

    return;
}
