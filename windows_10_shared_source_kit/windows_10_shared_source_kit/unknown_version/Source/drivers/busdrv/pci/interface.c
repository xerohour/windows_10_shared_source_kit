/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    interface.c

Abstract:

    IRP_MN_QUERY_INTERFACE lives here.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "interface.tmh"

// --------------------------------------------------------------------- Defines

#define PciCompareGuid(a,b)                                         \
    (RtlEqualMemory((PVOID)(a), (PVOID)(b), sizeof(GUID)))

#define LOCATION_INTERFACE_VERSION 1

//
// The length - in characters - of the Multi-Sz strings returned from the
// location interface. count one extra character for the MultiSz second terminator
//
#define LOCATION_INTERFACE_DEVICE_STRING_COUNT (sizeof "PCI(XXXX)" + 1)
#define LOCATION_INTERFACE_ROOT_STRING_COUNT (sizeof "PCIROOT(XXXXXX)" + 1)

#define DEVICE_PRESENT_INTERFACE_MINSIZE     FIELD_OFFSET(PCI_DEVICE_PRESENT_INTERFACE, IsDevicePresentEx)

#define PCI_TRANSLATOR_INTERFACE_VERSION 0
#define TR_IRQ_INVALID_VECTOR 0xffffffff

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

PCI_INTERFACE_CONSTRUCTOR PciLocationInterface_Constructor;

VOID
PciLocationInterface_Reference(
    __inout PVOID Context
    );

VOID
PciLocationInterface_Dereference(
    __inout PVOID Context
    );

NTSTATUS
PciLocationInterface_GetLocationStrings(
    __inout PVOID Context,
    __out PWCHAR *LocationStrings
    );

PCI_INTERFACE_CONSTRUCTOR PciDevicePresentInterface_Constructor;

VOID
PciDevicePresentInterface_Reference(
    __inout PVOID Context
    );

VOID
PciDevicePresentInterface_Dereference(
    __inout PVOID Context
    );

PCI_IS_DEVICE_PRESENT PciDevicePresentInterface_DevicePresent;
PCI_IS_DEVICE_PRESENT_EX PciDevicePresentInterface_DevicePresentEx;

BOOLEAN
PciCheckDevicePresence(
    __in_opt PPCI_DEVICE Device,
    __in PPCI_DEVICE_PRESENCE_PARAMETERS Parameters
    );

BOOLEAN
PciDevicePresentOnBus(
    __inout PPCI_BUS Bus,
    __in_opt PPCI_DEVICE Device,
    __in PPCI_DEVICE_PRESENCE_PARAMETERS Parameters
    );

PCI_INTERFACE_CONSTRUCTOR PciCardbusInterface_Constructor;

VOID
PciCardbusInterface_Reference(
    __inout PVOID Context
    );

VOID
PciCardbusInterface_Dereference(
    __inout PVOID Context
    );

NTSTATUS
PciCardbusInterface_GetLocation(
    __in PDEVICE_OBJECT Pdo,
    __out PULONG Bus,
    __out PUCHAR DeviceNumber,
    __out PUCHAR FunctionNumber,
    __out BOOLEAN *OnDebugPath
    );

PCI_INTERFACE_CONSTRUCTOR PciTranslatorInterface_Constructor;

//
// Routines describing crashdump functions interface.
//

PCI_INTERFACE_CONSTRUCTOR PciCrashdumpFunctions_Constructor;

VOID
PciCrashdumpFunctions_Reference(
    __inout PVOID Context
    );

VOID
PciCrashdumpFunctions_Dereference(
    __inout PVOID Context
    );

NTSTATUS
PciCrashdumpFunctions_PowerOn (
    _In_opt_ PVOID Context
    );

_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR PciReenumerateSelfInterface_Constructor;

_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR PciDeviceResetInterface_Constructor;

PCI_INTERFACE_CONSTRUCTOR PciSecurityInterface_Constructor;
PCI_SET_ACS PciSecurityInterface_SetAcs;

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciQueryInterface)
    #pragma alloc_text(PAGE, PciRefDereferenceNoop)
    #pragma alloc_text(PAGE, PciLocationInterface_Constructor)
    #pragma alloc_text(PAGE, PciLocationInterface_GetLocationStrings)
    #pragma alloc_text(PAGE, PciDevicePresentInterface_Constructor)
    #pragma alloc_text(PAGE, PciDevicePresentInterface_DevicePresent)
    #pragma alloc_text(PAGE, PciDevicePresentInterface_DevicePresentEx)
    #pragma alloc_text(PAGE, PciCheckDevicePresence)
    #pragma alloc_text(PAGE, PciDevicePresentOnBus)
    #pragma alloc_text(PAGE, PciCardbusInterface_Constructor)
    #pragma alloc_text(PAGE, PciCardbusInterface_GetLocation)
    #pragma alloc_text(PAGE, PciTranslatorInterface_Constructor)
    #pragma alloc_text(PAGE, PciCrashdumpFunctions_Constructor)
    #pragma alloc_text(PAGE, PciReenumerateSelfInterface_Constructor)
    #pragma alloc_text(PAGE, PciDeviceResetInterface_Constructor)
    #pragma alloc_text(PAGE, PciSecurityInterface_Constructor)
    #pragma alloc_text(PAGE, PciSecurityInterface_SetAcs)
#endif

// --------------------------------------------------------------------- Globals

extern const PCI_INTERFACE PciArbiterInterface;
extern const PCI_INTERFACE PciBusHandlerInterface;
extern const PCI_INTERFACE PciAgpTargetInterface;
extern const PCI_INTERFACE PciRoutingInterface;
extern const PCI_INTERFACE PciLegacyDeviceDetectionInterface;
extern const PCI_INTERFACE PciPmeInterface;
extern const PCI_INTERFACE PciDevicePresentInterface;
extern const PCI_INTERFACE PciNativeIdeInterface;
extern const PCI_INTERFACE PciBridgeInterface;
extern const PCI_INTERFACE ExpressLinkQuiescentInterface;
extern const PCI_INTERFACE ExpressRootPortInterface;
extern const PCI_INTERFACE PciMsiXTableConfigInterface;
extern const PCI_INTERFACE PciVirtualizationInterface;
extern const PCI_INTERFACE PciD3ColdSupportInterface;
extern const PCI_INTERFACE PciIommuBusInterfaceInterface;

const PCI_INTERFACE PciLocationInterface = {
    &GUID_PNP_LOCATION_INTERFACE,           // InterfaceType
    sizeof(PNP_LOCATION_INTERFACE),         // MinSize
    LOCATION_INTERFACE_VERSION,             // MinVersion
    LOCATION_INTERFACE_VERSION,             // MaxVersion
    PCIIF_PDO | PCIIF_FDO | PCIIF_ROOT,     // Flags - supported on PDOs and root FDOs
    PciInterface_Location,                  // Signature
    PciLocationInterface_Constructor        // Constructor
};

const PCI_INTERFACE PciDevicePresentInterface = {
    &GUID_PCI_DEVICE_PRESENT_INTERFACE,     // InterfaceType
    DEVICE_PRESENT_INTERFACE_MINSIZE,       // MinSize
    PCI_DEVICE_PRESENT_INTERFACE_VERSION,   // MinVersion
    PCI_DEVICE_PRESENT_INTERFACE_VERSION,   // MaxVersion
    PCIIF_PDO,                              // Flags
    PciInterface_DevicePresent,             // Signature
    PciDevicePresentInterface_Constructor   // Constructor
};

const PCI_INTERFACE PciCardbusPrivateInterface = {
    &GUID_PCI_CARDBUS_INTERFACE_PRIVATE,    // InterfaceType
    sizeof(PCI_CARDBUS_INTERFACE_PRIVATE),  // MinSize
    PCI_CB_INTRF_VERSION,                   // MinVersion
    PCI_CB_INTRF_VERSION,                   // MaxVersion
    PCIIF_PDO,                              // Flags
    PciInterface_PciCb,                     // Signature
    PciCardbusInterface_Constructor         // Constructor
};

const PCI_INTERFACE PciTranslatorInterface = {
    &GUID_TRANSLATOR_INTERFACE_STANDARD,    // InterfaceType
    sizeof(TRANSLATOR_INTERFACE),           // MinSize
    PCI_TRANSLATOR_INTERFACE_VERSION,       // MinVersion
    PCI_TRANSLATOR_INTERFACE_VERSION,       // MaxVersion
    PCIIF_FDO,                              // Flags
    PciInterface_Translator,                // Signature
    PciTranslatorInterface_Constructor      // Constructor
};

const PCI_INTERFACE PciCrashdumpFunctionsInterface = {
    &GUID_QUERY_CRASHDUMP_FUNCTIONS,        // InterfaceType
    sizeof(CRASHDUMP_FUNCTIONS_INTERFACE),  // MinSize
    1,                                      // MinVersion
    1,                                      // MaxVersion
    PCIIF_PDO | PCIIF_FDO | PCIIF_ROOT,     // Flags
    PciInterface_CrashdumpFunctions,        // Signature
    PciCrashdumpFunctions_Constructor       // Constructor
};

const PCI_INTERFACE PciReenumerateSelfInterface = {
    &GUID_REENUMERATE_SELF_INTERFACE_STANDARD,      // InterfaceType
    sizeof(REENUMERATE_SELF_INTERFACE_STANDARD),    // MinSize
    1,                                              // MinVersion
    1,                                              // MaxVersion
    PCIIF_PDO,                                      // Flags
    PciInterface_ReenumerateSelf,                   // Signature
    PciReenumerateSelfInterface_Constructor         // Constructor
};

const PCI_INTERFACE PciDeviceResetStandardInterface = {
    &GUID_DEVICE_RESET_INTERFACE_STANDARD,          // InterfaceType
    sizeof(DEVICE_RESET_INTERFACE_STANDARD),        // MinSize
    DEVICE_RESET_INTERFACE_VERSION,                 // MinVersion
    DEVICE_RESET_INTERFACE_VERSION,                 // MaxVersion
    PCIIF_PDO,                                      // Flags
    PciInterface_DeviceResetStandard,               // Signature
    PciDeviceResetInterface_Constructor             // Constructor
};

const PCI_INTERFACE PciSecurityInterface = {
    &GUID_PCI_SECURITY_INTERFACE,                   // InterfaceType
    sizeof(PCI_SECURITY_INTERFACE),                 // MinSize
    PCI_SECURITY_INTERFACE_VERSION,                 // MinVersion
    PCI_SECURITY_INTERFACE_VERSION,                 // MaxVersion
    PCIIF_PDO,                                      // Flags
    PciInterface_SecurityInterface,                 // Signature
    PciSecurityInterface_Constructor                // Constructor
};

const PCI_INTERFACE * const PciInterfaces[] = {
    &PciArbiterInterface,
    &PciBusHandlerInterface,
    &PciAgpTargetInterface,
    &PciRoutingInterface,
    &PciCardbusPrivateInterface,
    &PciLegacyDeviceDetectionInterface,
    &PciPmeInterface,
    &PciDevicePresentInterface,
    &PciNativeIdeInterface,
    &PciLocationInterface,
    &PciBridgeInterface,
    &ExpressLinkQuiescentInterface,
    &ExpressRootPortInterface,
    &PciMsiXTableConfigInterface,
    &PciVirtualizationInterface,
    &PciD3ColdSupportInterface,
    &PciCrashdumpFunctionsInterface,
    &PciReenumerateSelfInterface,
    &PciIommuBusInterfaceInterface,
    &PciDeviceResetStandardInterface,
    &PciSecurityInterface,
    NULL
};

//
// Reference count for the PCI global crashdump functions interface.
//

volatile LONG PciCrashdumpFunctionsReferenceCount;

//
// These are the interfaces we supply only if nobody underneath
// us (the HAL) does.
//
const PCPCI_INTERFACE PciInterfacesLastResort[] = {
    &PciTranslatorInterface,
    NULL
};

// -------------------------------------------------------------- General Interface Handler

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciQueryInterface(
    __in PVOID DeviceExtension,
    __in LPCGUID InterfaceType,
    __in USHORT Size,
    __in USHORT Version,
    __in PVOID InterfaceSpecificData,
    __out_bcount(Size) PINTERFACE InterfaceReturn,
    __in BOOLEAN LastChance,
    __in NTSTATUS CurrentIrpStatus
    )
/*++

Routine Description:

    This routine searches through the PciInterfaces table looking for
    an interface entry that matches the parameters.

Arguments:

    DeviceExtension - The device extension that received the interface request.
        Some interfaces are allowable only from the PDO level, and some only
        from the FDO level.

    InterfaceType - A GUID representing the requested interface.

    Size - The size of the buffer provided by the caller to fill
        in with the interface.

    Version - The version of the interface requested.

    InterfaceSpecificData - Interface-specific data provided by the caller.

    InterfaceReturn - The buffer (of size Size) to fill in with the matching
        interface.

    LastChance - Whether to provide "last chance" interfaces.  Last chance
        interfaces should only be provided by the FDO after passing the
        request down to the PDO level and seeing that the PDO does not
        provide the interface itself.  Currently this is only used for
        interrupt translators.

    CurrentIrpStatus - This status indicates whether another driver in the stack
        has already filled in the interface.

Return Value:

    NT Status code.

--*/
{
    UNICODE_STRING guidString;
    const PCPCI_INTERFACE *interfaceEntry;
    const PCPCI_INTERFACE *interfaceTable;
    PCPCI_INTERFACE interface;
    NTSTATUS status;
    PPCI_BUS bus;

    PAGED_CODE();

    status = DebugStringFromGuid(InterfaceType, &guidString);
    if (NT_SUCCESS(status)) {

        TRACE(INTERFACE_TRACE_LEVEL,
              "PciQueryInterface TYPE = %wZ\n",
              &guidString
              );

        TRACE(INTERFACE_TRACE_LEVEL,
              "Size = %d, Version = %d, InterfaceData = %p, LastChance = %s\n",
              Size,
              Version,
              InterfaceSpecificData,
              LastChance ? "TRUE" : "FALSE"
              );

        RtlFreeUnicodeString(&guidString);
    }

    //
    // Try to locate the requested interface in the PCI driver's set
    // of exported interfaces.
    //
    // Note - we do not allow last chance interfaces (ie mock translators) for
    // machines where we assign bus numbers
    //
    if (LastChance) {

        interfaceTable = PciInterfacesLastResort;
    } else {
        interfaceTable = PciInterfaces;
    }

    for (interfaceEntry = interfaceTable; *interfaceEntry; interfaceEntry++) {

        interface = *interfaceEntry;

        //
        // Check if this interface is allowed to be used from this
        // device object type.  PCIIF_PDO means the interface is only
        // allowed at the PDO level, PCIIF_FDO means only at the FDO level.
        // PCIIF_ROOT must be used in conjunction with PCIIF_FDO, but
        // means that the interface can only be used from roots.
        //
        if (PCI_DEVICE_EXTENSION(DeviceExtension)) {

            if ((interface->Flags & PCIIF_PDO) == 0) {

                status = DebugStringFromGuid(interface->InterfaceType, &guidString);
                if (NT_SUCCESS(status)) {
                    TRACE(INTERFACE_TRACE_LEVEL,
                          "\tguid = %wZ only for PDOs\n",
                          &guidString
                          );
                    RtlFreeUnicodeString(&guidString);
                }

                continue;
            }

        } else {

            if ((interface->Flags & PCIIF_FDO) == 0) {

                status = DebugStringFromGuid(interface->InterfaceType, &guidString);
                if (NT_SUCCESS(status)) {
                    TRACE(INTERFACE_TRACE_LEVEL,
                          "\tguid = %wZ only for FDOs\n",
                          &guidString
                          );
                    RtlFreeUnicodeString(&guidString);
                }

                continue;
            }

            if (interface->Flags & PCIIF_ROOT) {

                bus = (PPCI_BUS)DeviceExtension;

                if (!PCI_ROOT_BUS(bus)) {

                    status = DebugStringFromGuid(interface->InterfaceType, &guidString);
                    if (NT_SUCCESS(status)) {
                        TRACE(INTERFACE_TRACE_LEVEL,
                              "\tguid = %wZ only for ROOT\n",
                              &guidString
                              );
                        RtlFreeUnicodeString(&guidString);
                    }

                    continue;
                }
            }
        }

        status = DebugStringFromGuid(interface->InterfaceType, &guidString);
        if (NT_SUCCESS(status)) {
            TRACE(INTERFACE_TRACE_LEVEL,
                  "\tguid = %wZ okay for device type\n",
                  &guidString
                  );
            RtlFreeUnicodeString(&guidString);
        }

        //
        // Check for the appropriate GUID then verify version numbers
        // and size.
        //
        if ((PciCompareGuid(InterfaceType, interface->InterfaceType)) &&
            (Version >= interface->MinVersion) &&
            (Version <= interface->MaxVersion) &&
            (Size >= interface->MinSize)) {

            //
            // We have a possible hit.  Check to see if the interface
            // itself agrees.
            //
            status = interface->Constructor(
                DeviceExtension,
                InterfaceSpecificData,
                Version,
                Size,
                InterfaceReturn,
                CurrentIrpStatus == STATUS_SUCCESS ? TRUE : FALSE
                );
            if (NT_SUCCESS(status)) {

                //
                // We found and allocated an interface, reference it
                // and get out of the loop.
                //
                InterfaceReturn->InterfaceReference(InterfaceReturn->Context);

                TRACE(INTERFACE_TRACE_LEVEL, "-- returning SUCCESS\n");

                return status;

            } else {

                TRACE(INTERFACE_TRACE_LEVEL, "\t\tContructor returned %08lx\n", status);
            }
        } else {

            TRACE(INTERFACE_TRACE_LEVEL, "\t\tguid doesn't match.\n");
        }
    }

    //
    // Failed to find the requested interface.
    //
    TRACE(INTERFACE_TRACE_LEVEL, "-- FAILED TO FIND INTERFACE\n");

    return STATUS_NOT_SUPPORTED;
}

VOID
PciRefDereferenceNoop(
    __in PVOID Context
    )
{
    UNREFERENCED_PARAMETER(Context);

    PAGED_CODE();
    return;
}

// -------------------------------------------------------------- Location Interface

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciLocationInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    This routine constructs a PNP_LOCATION_INTERFACE.

Arguments:

    DeviceExtension - An extension pointer.

    InterfaceSpecificData - Unused.

    Version - Interface version.

    Size - Size of the PNP_LOCATION_INTERFACE interface object.

    InterfaceReturn - The interface object pointer to be filled in.

    Filtered - Not supported for this interface.

Return Value:

    Returns NTSTATUS.

--*/
{
    PPNP_LOCATION_INTERFACE interface;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(PNP_LOCATION_INTERFACE)) {
        return STATUS_UNSUCCESSFUL;
    }

    interface = (PPNP_LOCATION_INTERFACE)InterfaceReturn;
    interface->Size = sizeof(PNP_LOCATION_INTERFACE);
    interface->Version = LOCATION_INTERFACE_VERSION;
    interface->Context = DeviceExtension;
    interface->InterfaceReference = PciLocationInterface_Reference;
    interface->InterfaceDereference = PciLocationInterface_Dereference;
    interface->GetLocationString = PciLocationInterface_GetLocationStrings;

    return STATUS_SUCCESS;
}

VOID
PciLocationInterface_Reference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine increments outstanding reference count for this interface

Arguments:

    PCI_DEVICE or PCI_BUS

Return Value:

    None

--*/
{
    PPCI_COMMON_EXTENSION extension = (PPCI_COMMON_EXTENSION)Context;

    if (PCI_DEVICE_EXTENSION(extension)) {

        PPCI_DEVICE device = (PPCI_DEVICE)extension;
        InterlockedIncrement(&device->PciLocationInterfaceCount);

    } else {

        PPCI_BUS bus = (PPCI_BUS)extension;
        InterlockedIncrement(&bus->PciLocationInterfaceCount);
    }

    return;
}

VOID
PciLocationInterface_Dereference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine decrements outstanding reference count for this interface

Arguments:

    PCI_DEVICE or PCI_BUS

Return Value:

    None

--*/
{
    PPCI_COMMON_EXTENSION extension = (PPCI_COMMON_EXTENSION)Context;

    if (PCI_DEVICE_EXTENSION(extension)) {

        PPCI_DEVICE device = (PPCI_DEVICE)extension;
        InterlockedDecrement(&device->PciLocationInterfaceCount);

    } else {

        PPCI_BUS bus = (PPCI_BUS)extension;
        InterlockedDecrement(&bus->PciLocationInterfaceCount);
    }

    return;
}

NTSTATUS
PciLocationInterface_GetLocationStrings(
    __inout PVOID Context,
    __out PWCHAR *LocationStrings
    )
/*++

Routine Description:

    This routine allocates, fills in, and returns a Multi-Sz string
    containing the bus-relative location identifier string for the
    given device.

    For a PCI device, this is "PCI(XXYY)", where XX is the device
    number of the device, and YY is the function number of the device.

    For a PCI root bus, this is PCIROOT(XX), where XX is the bus number
    of the root bus.  This relies on the fact that bus numbers of root
    buses will not change, which is believed to be a safe assumption
    for some time to come.

    This interface is permitted to return a Multi-Sz containing
    multiple strings describing the same device, but in this
    first implementation, only the single strings listed above
    will be returned from the interface.  The string must still
    be in the format of a Multi-Sz, however, meaning a double-NULL
    terminator is required.

Arguments:

    Context - extension pointer.

Return Value:

    NTSTATUS code.

--*/
{
    PPCI_COMMON_EXTENSION extension;
    PPCI_DEVICE device;
    PPCI_BUS bus;
    SIZE_T remainingChars;
    PWCHAR stringBuffer;
    NTSTATUS status;

    PAGED_CODE();

    *LocationStrings = NULL;

    extension = (PPCI_COMMON_EXTENSION)Context;

    if (PCI_DEVICE_EXTENSION(extension)) {

        device = (PPCI_DEVICE)extension;

        stringBuffer = PciAllocatePool(
                            PagedPool,
                            LOCATION_INTERFACE_DEVICE_STRING_COUNT*sizeof(WCHAR)
                            );
        if (!stringBuffer) {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        //
        // The location string for a PCI device is "PCI(XXYY)"
        // where XX is the device number and YY is the function number
        // We use the STRSAFE_FILL_BEHIND_NULL flag to ensure the unused portion
        // of the buffer is filled with 0s which null terminates the multsz
        //
        status = RtlStringCchPrintfExW(stringBuffer,
                                       LOCATION_INTERFACE_DEVICE_STRING_COUNT,
                                       NULL,
                                       &remainingChars,
                                       STRSAFE_FILL_BEHIND_NULL,
                                       L"PCI(%.2X%.2X)",
                                       device->Slot.u.bits.DeviceNumber,
                                       device->Slot.u.bits.FunctionNumber
                                       );
        PCI_ASSERT(NT_SUCCESS(status));
        UNREFERENCED_PARAMETER(status);

        //
        // Make sure there was room for the multisz termination NUL
        // N.B. remainingChars counts the NUL termination of the regular string
        // as being available so we need to ensure 2 chars are left for the 2 NULS
        //
        PCI_ASSERT(remainingChars >= 2);

        *LocationStrings = stringBuffer;
        return STATUS_SUCCESS;

    } else {

        bus = (PPCI_BUS)extension;

        if (PCI_ROOT_BUS(bus)) {

            stringBuffer = PciAllocatePool(
                                PagedPool,
                                LOCATION_INTERFACE_ROOT_STRING_COUNT*sizeof(WCHAR)
                                );
            if (!stringBuffer) {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            //
            // The location string for a PCI root is "PCIROOT(X)"
            // where X is the bus number of root bus.
            // We use the STRSAFE_FILL_BEHIND_NULL flag to ensure the unused portion
            // of the buffer is filled with 0s which null terminates the multsz
            //
            status = RtlStringCchPrintfExW(stringBuffer,
                                           LOCATION_INTERFACE_ROOT_STRING_COUNT,
                                           NULL,
                                           &remainingChars,
                                           STRSAFE_FILL_BEHIND_NULL,
                                           L"PCIROOT(%X)",
                                           bus->SecondaryBusNumber
                                           );
            PCI_ASSERT(NT_SUCCESS(status));
            UNREFERENCED_PARAMETER(status);

            //
            // Make sure there was room for the multisz termination NUL
            // N.B. remainingChars counts the NUL terminatiun of the regular string
            // as being available so we need to ensure 2 chars are left for the 2 NULS
            //
            PCI_ASSERT(remainingChars >= 2);

            *LocationStrings = stringBuffer;

            //
            // returning STATUS_TRANSLATION_COMPLETE indicates that PnP shouldn't
            // query for this interface any further up the tree.  Stop here.
            //
            return STATUS_TRANSLATION_COMPLETE;

        } else {

            //
            // In the interface constructor, we specified that this interface
            // is only valid for root FDOs.  If we get here, we've been asked
            // to fill in this interface for a P-P bridge FDO, which is illegal.
            //
            ERROR("PciLocationInterface_GetLocationStrings: not a root bus.\n");
            return STATUS_INVALID_PARAMETER;
        }
    }
}

// -------------------------------------------------------------- Device Present Interface

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciDevicePresentInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    This routine constructs a PCI_DEVICE_PRESENT_INTERFACE.

Arguments:

    DeviceExtension - An extension pointer.

    InterfaceSpecificData - Unused.

    Version - Interface version.

    Size - Size of the PCI_DEVICE_PRESENT_INTERFACE interface object.

    InterfaceReturn - The interface object pointer to be filled in.

    Filtered - Not supported for this interface.

Return Value:

    Returns NTSTATUS.

--*/
{
    PPCI_DEVICE_PRESENT_INTERFACE interface;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < DEVICE_PRESENT_INTERFACE_MINSIZE) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Have already verified that the InterfaceReturn variable
    // points to an area in memory large enough to contain a
    // PCI_DEVICE_PRESENT_INTERFACE.  Fill it in for the caller.
    //
    interface = (PPCI_DEVICE_PRESENT_INTERFACE)InterfaceReturn;

    interface->Version              = PCI_DEVICE_PRESENT_INTERFACE_VERSION;
    interface->InterfaceReference   = PciDevicePresentInterface_Reference;
    interface->InterfaceDereference = PciDevicePresentInterface_Dereference;
    interface->Context              = DeviceExtension;
    interface->IsDevicePresent      = PciDevicePresentInterface_DevicePresent;

    //
    // This interface has been extended from the base interface (what was
    // filled in above), to a larger interface.  If the buffer provided
    // is large enough to hold the whole thing, fill in the rest.  Otherwise
    // don't.
    //
    if (Size >= sizeof(PCI_DEVICE_PRESENT_INTERFACE)) {

        interface->IsDevicePresentEx = PciDevicePresentInterface_DevicePresentEx;
        interface->Size = sizeof(PCI_DEVICE_PRESENT_INTERFACE);

    } else {

        interface->Size = DEVICE_PRESENT_INTERFACE_MINSIZE;
    }

    return STATUS_SUCCESS;
}

VOID
PciDevicePresentInterface_Reference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine increments outstanding reference count for this interface

Arguments:

    PCI_DEVICE

Return Value:

    None

--*/
{
    PPCI_DEVICE device = (PPCI_DEVICE)Context;

    InterlockedIncrement(&device->PciDevicePresentInterfaceCount);

    return;
}

VOID
PciDevicePresentInterface_Dereference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine decrements outstanding reference count for this interface

Arguments:

    PCI_DEVICE

Return Value:

    None

--*/
{
    PPCI_DEVICE device = (PPCI_DEVICE)Context;

    InterlockedDecrement(&device->PciDevicePresentInterfaceCount);

    return;
}

BOOLEAN
PciDevicePresentInterface_DevicePresent(
    __in USHORT VendorID,
    __in USHORT DeviceID,
    __in UCHAR RevisionID,
    __in USHORT SubVendorID,
    __in USHORT SubSystemID,
    __in ULONG Flags
    )
/*++

Routine Description:

    This routine searches the PCI device tree to see if the specific device
    is present in the system.  Not devices that are explicitly not enumerated
    (such as PIIX4 power management function) are considered absent.

Arguments:

    VendorID - Required VendorID of the device

    DeviceID - Required DeviceID of the device

    RevisionID - Optional Revision ID

    SubVendorID - Optional Subsystem Vendor ID

    SubSystemID - Optional Subsystem ID

    Flags - Bitfield which indicates if Revision and Sub* ID's should be used:

        PCI_USE_SUBSYSTEM_IDS, PCI_USE_REVISION_ID are valid all other bits
        should be 0


Return Value:

    TRUE if the device is present in the system, FALSE otherwise.

--*/
{
    PCI_DEVICE_PRESENCE_PARAMETERS parameters;

    PAGED_CODE();

    parameters.Size = sizeof(PCI_DEVICE_PRESENCE_PARAMETERS);
    parameters.VendorID = VendorID;
    parameters.DeviceID = DeviceID;
    parameters.RevisionID = RevisionID;
    parameters.SubVendorID = SubVendorID;
    parameters.SubSystemID = SubSystemID;

    //
    // Clear out flags that this version of the interface didn't use,
    //
    parameters.Flags = Flags & (PCI_USE_SUBSYSTEM_IDS | PCI_USE_REVISION);

    //
    // This original version of the interface required vendor/device ID
    // matching.  The new version doesn't, so set the flag indicating
    // that we do in fact want to do a vendor/device ID match.
    //
    parameters.Flags |= PCI_USE_VENDEV_IDS;

    return PciCheckDevicePresence(NULL, &parameters);
}

BOOLEAN
PciDevicePresentInterface_DevicePresentEx(
    __in PVOID Context,
    __in PPCI_DEVICE_PRESENCE_PARAMETERS Parameters
    )
/*++

Routine Description:

    This routine searches the PCI device tree to see if the specific device
    is present in the system.  Note devices that are explicitly not enumerated
    (such as PIIX4 power management function) are considered absent.

Arguments:

    Context - The device extension of the device requesting the search.

    Parameters - Pointer to a structure containing the parameters of the device search,
                 including VendorID, SubSystemID and ClassCode, among others.

Return Value:

    TRUE if the device is present in the system, FALSE otherwise.

--*/
{
    PAGED_CODE();

    return PciCheckDevicePresence((PPCI_DEVICE)Context, Parameters);
}

BOOLEAN
PciCheckDevicePresence(
    __in_opt PPCI_DEVICE Device,
    __in PPCI_DEVICE_PRESENCE_PARAMETERS Parameters
    )
/*++

Routine Description:

    This routine searches the PCI device tree to see if the specific device
    is present in the system.  Note devices that are explicitly not enumerated
    (such as PIIX4 power management function) are considered absent.

Arguments:

    Device - The device extension of the device requesting the search.

    Parameters - Pointer to a structure containing the parameters of the device search,
                 including VendorID, SubSystemID and ClassCode, among others.

Return Value:

    TRUE if the device is present in the system, FALSE otherwise.

--*/
{
    PPCI_SEGMENT segment;
    PPCI_BUS bus;
    BOOLEAN found;
    ULONG flags;

    PAGED_CODE();

    found = FALSE;

    //
    // Validate the parameters.
    //
    if (!ARGUMENT_PRESENT(Parameters)) {
        ERROR("PciDevicePresentInterface_DevicePresentEx: no parameters supplied.\n");
        return FALSE;
    }

    if (Parameters->Size < sizeof(PCI_DEVICE_PRESENCE_PARAMETERS)) {
        ERROR("PciDevicePresentInterface_DevicePresentEx: Invalid parameter supplied - size is bad.\n");
        return FALSE;
    }

    flags = Parameters->Flags;

    //
    // We can either do a Vendor/Device ID match, or a Class/SubClass
    // match.  If neither of these flags are present, fail.
    //
    if (!(flags & (PCI_USE_VENDEV_IDS | PCI_USE_CLASS_SUBCLASS))) {
        ERROR("PciDevicePresentInterface_DevicePresentEx: Invalid parameter supplied - match type not specified.\n");
        return FALSE;
    }

    //
    // RevisionID, SubVendorID and SubSystemID are more precise flags.
    // They are only valid if we're doing a Vendor/Device ID match.
    //
    if (flags & (PCI_USE_REVISION | PCI_USE_SUBSYSTEM_IDS)) {
        if (!(flags & PCI_USE_VENDEV_IDS)) {
            ERROR("PciDevicePresentInterface_DevicePresentEx: Invalid parameter supplied - SUBSYSTEM_IDS only valid on VEN/DEV ID match.\n");
            return FALSE;
        }
    }

    //
    // Programming Interface is also a more precise flag.
    // It is only valid if we're doing a class code match.
    //
    if (flags & PCI_USE_PROGIF) {
        if (!(flags & PCI_USE_CLASS_SUBCLASS)) {
            ERROR("PciDevicePresentInterface_DevicePresentEx: Invalid parameter supplied - PROGIF only valid on class code match.\n");
            return FALSE;
        }
    }

    //
    // Okay, validation complete.  Do the search.
    //
    PciAcquirePassiveLock(&PciGlobalLock);

    if (flags & (PCI_USE_LOCAL_BUS | PCI_USE_LOCAL_DEVICE)) {

        //
        // Limit the search to the same bus as the device that requested
        // the search.  This requires the device requesting the search.
        //
        if (Device == NULL) {
            WARN("PciDevicePresentInterface_DevicePresentEx: device is NULL.\n");
            goto cleanup;
        }

        found = PciDevicePresentOnBus(Device->Parent, Device, Parameters);

    } else {

        //
        // We have not been told to limit the search to
        // the bus on which a particular device lives.
        // Do a global search, iterating over all the buses.
        //
        for (segment = PciSegmentList; segment != NULL; segment = segment->Next) {
            bus = segment->PciRootBusList;
            while (bus) {

                found = PciDevicePresentOnBus(bus,
                                            NULL,
                                            Parameters
                                            );
                if (found) {
                    goto cleanup;
                }

                bus = PciNextBusInSegment(bus);
            }
        }
    }

cleanup:

    PciReleasePassiveLock(&PciGlobalLock);

    return found;

}

BOOLEAN
PciDevicePresentOnBus(
    __inout PPCI_BUS Bus,
    __in_opt PPCI_DEVICE Device,
    __in PPCI_DEVICE_PRESENCE_PARAMETERS Parameters
    )
/*++

Routine Description:

    This routine searches the PCI device tree for a given device
    on the bus represented by the given FdoExtension.

Arguments:

    Bus - A pointer to the device extension of a PCI FDO.
        This represents the bus to be searched for the given device.

    Device - A pointer to the device extension of the PCI PDO that requested
        the search.  Some device searches are limited to the same bus/device number
        as the requesting device, and this is used to get those numbers.

    Parameters - The parameters of the search.

    Flags - A bitfield indicating which fields of the Parameters
        structure to use for the search.

Return Value:

    TRUE if the requested device is found.
    FALSE if it is not.

--*/
{
    PPCI_DEVICE current;
    BOOLEAN found = FALSE;
    ULONG flags = Parameters->Flags;

    PAGED_CODE();

    found = FALSE;

    PciAcquirePassiveLock(&Bus->ChildDeviceLock);

    for (current = Bus->ChildDevices; current != NULL; current = current->Sibling) {

        //
        // If we're limiting the search to devices with the same
        // device number as the requesting device, make sure this
        // current PDO qualifies.
        //
        if (ARGUMENT_PRESENT(Device) && (flags & PCI_USE_LOCAL_DEVICE)) {

            if (Device->Slot.u.bits.DeviceNumber !=
                current->Slot.u.bits.DeviceNumber) {

                continue;
            }
        }

        if (flags & PCI_USE_VENDEV_IDS) {

            if ((current->VendorID != Parameters->VendorID) ||
                (current->DeviceID != Parameters->DeviceID)) {

                continue;
            }

            if ((flags & PCI_USE_SUBSYSTEM_IDS) &&
                ((current->SubVendorID != Parameters->SubVendorID) ||
                 (current->SubSystemID != Parameters->SubSystemID))) {

                continue;
            }

            if ((flags & PCI_USE_REVISION) &&
                (current->RevisionID != Parameters->RevisionID)) {

                continue;
            }
        }

        if (flags & PCI_USE_CLASS_SUBCLASS) {

            if ((current->BaseClass != Parameters->BaseClass) ||
                (current->SubClass != Parameters->SubClass)) {

                continue;
            }

            if ((flags & PCI_USE_PROGIF) &&
                (current->ProgIf != Parameters->ProgIf)) {

                continue;
            }
        }

        found = TRUE;
        break;
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

    return found;
}

// --------------------------------------------------------------------- Cardbus Interface

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciCardbusInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    This routine constructs a PCI_CARDBUS_PRIVATE_INTERFACE.

Arguments:

    DeviceExtension - An extension pointer.

    InterfaceSpecificData - Unused.

    Version - Interface version.

    Size - Size of the PCI_CARDBUS_INTERFACE_PRIVATE interface object.

    InterfaceReturn - The interface object pointer to be filled in.

    Filtered - Not supported for this interface.

Return Value:

    Returns NTSTATUS.

--*/
{
    PPCI_CARDBUS_INTERFACE_PRIVATE interface;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    //
    // Make sure that no one is asking for the old, larger interface
    //
    if (Size > sizeof(PCI_CARDBUS_INTERFACE_PRIVATE)) {
        PCI_ASSERT(Size == sizeof(PCI_CARDBUS_INTERFACE_PRIVATE));
        return STATUS_NOT_SUPPORTED;
    }

    if (Size < sizeof(PCI_CARDBUS_INTERFACE_PRIVATE)) {
        return STATUS_UNSUCCESSFUL;
    }

    interface = (PPCI_CARDBUS_INTERFACE_PRIVATE)InterfaceReturn;

    //
    // Standard interface stuff
    //
    interface->Size = sizeof(PCI_CARDBUS_INTERFACE_PRIVATE);
    interface->Version = PCI_CB_INTRF_VERSION;
    interface->Context = DeviceExtension;
    interface->InterfaceReference = PciCardbusInterface_Reference;
    interface->InterfaceDereference = PciCardbusInterface_Dereference;

    //
    // PCI-CardBus private
    //
    interface->DriverObject = PciDriverObject;
    interface->GetLocation  = PciCardbusInterface_GetLocation;

    return STATUS_SUCCESS;
}

VOID
PciCardbusInterface_Reference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine increments outstanding reference count for this interface

Arguments:

    PCI_DEVICE

Return Value:

    None

--*/
{
    PPCI_DEVICE device = (PPCI_DEVICE)Context;

    InterlockedIncrement(&device->PciCardBusInterfaceCount);

    return;
}

VOID
PciCardbusInterface_Dereference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine decrements outstanding reference count for this interface

Arguments:

    PCI_DEVICE

Return Value:

    None

--*/
{
    PPCI_DEVICE device = (PPCI_DEVICE)Context;

    InterlockedDecrement(&device->PciCardBusInterfaceCount);

    return;
}

NTSTATUS
PciCardbusInterface_GetLocation(
    __in PDEVICE_OBJECT Pdo,
    __out PULONG Bus,
    __out PUCHAR DeviceNumber,
    __out PUCHAR FunctionNumber,
    __out BOOLEAN *OnDebugPath
    )
/*++

Routine Description:

    This routine provides location information about the given PDO.
    It provides the bus/device/function of the PDO for legacy purposes,
    even though this information is available through IoGetDeviceProperty.
    It also indicates whether the device is on the debugging path.  This
    information is not available through any other means.

Arguments:

    Pdo - The device object to get the location information of.

    Bus - On success, will be filled in with the segment/bus
        number of the device.

    DeviceNumber - will be filled in with the device number of the device.

    FunctinNumber - will be filled in with the function number of the device.

    DebugPath - will be filled in with TRUE if the device is on the debug
        path, or FALSE otherwise.

Return Value:

    NT Status code.

--*/
{
    PPCI_DEVICE device;

    PAGED_CODE();

    PCI_ASSERT(Bus);
    PCI_ASSERT(DeviceNumber);
    PCI_ASSERT(FunctionNumber);
    PCI_ASSERT(OnDebugPath);

    device = (PPCI_DEVICE)Pdo->DeviceExtension;

    //
    // Verify that it is actually a PDO.
    //
    if ((device == NULL) ||
        (device->Signature != PCI_DEVICE_SIGNATURE)) {
        return STATUS_NOT_FOUND;
    }

    *Bus            = device->Parent->SecondaryBusNumber;
    *DeviceNumber   = (UCHAR)device->Slot.u.bits.DeviceNumber;
    *FunctionNumber = (UCHAR)device->Slot.u.bits.FunctionNumber;
    *OnDebugPath    = (BOOLEAN)(device->OnDebugPath);

    return STATUS_SUCCESS;
}

//------------------------------------------------------------------- Translator Interface

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciTranslatorInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    This routine fills in a TRANSLATOR_INTERFACE.  PCI does not
    have translators itself.  The HAL does, but the HAL does not
    have device objects in the correct place in the tree to
    translate the correct resources.  So this routine gets the
    HAL translator that is appropriate for this device object.

Arguments:

    DeviceExtension - An extension pointer.

    InterfaceSpecificData - The CmResourceType of the translator request.

    Version - Interface version.

    Size - Size of the PCI_CARDBUS_INTERFACE_PRIVATE interface object.

    InterfaceReturn - The interface object pointer to be filled in.

    Filtered - Not supported for this interface.

Return Value:

    Returns NTSTATUS.

--*/
{
    ULONG secondaryBusNumber, parentBusNumber;
    INTERFACE_TYPE parentInterface;
    PPCI_BUS bus;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(TRANSLATOR_INTERFACE)) {
        return STATUS_UNSUCCESSFUL;
    }
    //
    // We only translate interrupts, is that what they want?
    //
    if ((CM_RESOURCE_TYPE)(ULONG_PTR)InterfaceSpecificData != CmResourceTypeInterrupt) {

        TRACE(ARBITER_TRACE_LEVEL,
              "IRQ trans constructor doesn't like %p in InterfaceSpecificData\n",
              InterfaceSpecificData
              );

        return STATUS_INVALID_PARAMETER_3;
    }

    bus = (PPCI_BUS)DeviceExtension;
    PCI_ASSERT(PCI_BUS_EXTENSION(bus));

    //
    // Give the HAL a shot at providing this translator.
    //
    if (PCI_ROOT_BUS(bus)) {

        parentInterface = Internal;
        secondaryBusNumber = bus->SecondaryBusNumber;
        parentBusNumber = 0;

        TRACE(ARBITER_TRACE_LEVEL, "      Is root FDO\n");

    } else {

        parentInterface = PCIBus;
        secondaryBusNumber = bus->SecondaryBusNumber;

        parentBusNumber = bus->ParentBus->SecondaryBusNumber;

        TRACE(ARBITER_TRACE_LEVEL,
              "      Is bridge FDO, parent bus %x, secondary bus %x\n",
              parentBusNumber,
              secondaryBusNumber
              );
    }

    return HalGetInterruptTranslator(parentInterface,
                                     parentBusNumber,
                                     PCIBus,
                                     sizeof(TRANSLATOR_INTERFACE),
                                     PCI_TRANSLATOR_INTERFACE_VERSION,
                                     (PTRANSLATOR_INTERFACE)InterfaceReturn,
                                     &secondaryBusNumber
                                     );
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciCrashdumpFunctions_Constructor (
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    This routine constructs a CRASHDUMP_FUNCTIONS_INTERFACE.

Arguments:

    DeviceExtension - An extension pointer.

    InterfaceSpecificData - Unused.

    Version - Interface version.

    Size - Size of the PNP_LOCATION_INTERFACE interface object.

    InterfaceReturn - The interface object pointer to be filled in.

    Filtered - Not supported for this interface.

Return Value:

    Returns NTSTATUS.

--*/

{

    PCRASHDUMP_FUNCTIONS_INTERFACE Interface;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(DeviceExtension);
    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(CRASHDUMP_FUNCTIONS_INTERFACE)) {
        return STATUS_UNSUCCESSFUL;
    }

    Interface = (PCRASHDUMP_FUNCTIONS_INTERFACE)InterfaceReturn;

    //
    // Standard interface stuff.
    //

    Interface->Size = sizeof(CRASHDUMP_FUNCTIONS_INTERFACE);
    Interface->Version = 1;
    Interface->Context = NULL;
    Interface->InterfaceReference = PciCrashdumpFunctions_Reference;
    Interface->InterfaceDereference = PciCrashdumpFunctions_Dereference;

    //
    // Crashdump functions interface private.
    //

    Interface->PowerOn = PciCrashdumpFunctions_PowerOn;
    return STATUS_SUCCESS;
}

VOID
PciCrashdumpFunctions_Reference (
    __inout PVOID Context
    )

{

    LONG Result;

    UNREFERENCED_PARAMETER(Context);

    Result = InterlockedIncrement(&PciCrashdumpFunctionsReferenceCount);

    PCI_ASSERT(Result > 0);

    return;
}

VOID
PciCrashdumpFunctions_Dereference (
    __inout PVOID Context
    )

{

    LONG Result;

    UNREFERENCED_PARAMETER(Context);

    Result = InterlockedDecrement(&PciCrashdumpFunctionsReferenceCount);

    PCI_ASSERT(Result >= 0);

    return;
}

NTSTATUS
PciCrashdumpFunctions_PowerOn (
    _In_opt_ PVOID Context
    )

{

    UNREFERENCED_PARAMETER(Context);

    return PciEarlyRestoreResourcesInternal(PCI_EARLY_RESTORE_CRASHDUMP,
                                            FALSE);
}

NTSTATUS
PciInterface_Noop(
    __in PVOID Context
    )

/*++

Routine Description:

    This routine is a no-op interface reference/dereference routine for
    ReenumerateSelf and DeviceReset interfaces.

Arguments:

    Context - Unused.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Context);

    return STATUS_SUCCESS;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciReenumerateSelfInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )

/*++

Routine Description:

    This routine initializes the ReenumerateSelf interface.

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

    PREENUMERATE_SELF_INTERFACE_STANDARD Interface;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(REENUMERATE_SELF_INTERFACE_STANDARD)) {
        return STATUS_INVALID_PARAMETER;
    }

    Interface = (PREENUMERATE_SELF_INTERFACE_STANDARD)InterfaceReturn;
    Interface->Size = sizeof(REENUMERATE_SELF_INTERFACE_STANDARD);
    Interface->Context = DeviceExtension;
    Interface->InterfaceReference = PciInterface_Noop;
    Interface->InterfaceDereference = PciInterface_Noop;
    Interface->SurpriseRemoveAndReenumerateSelf = PciSurpriseRemoveAndReenumerateSelf;

    return STATUS_SUCCESS;
}

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
PciDeviceResetInterface_DeviceReset(
    _In_ PVOID Context,
    _In_ DEVICE_RESET_TYPE ResetType,
    _In_ ULONG Flags,
    _In_opt_ PVOID Parameters
    )

/*++

Routine Description:

    This routine handles device reset requests and calls appropriate handler
    for specified reset type.

Arguments:

    Context - Supplies pointer to PCI_DEVICE stored in the
                   interface returned by QueryInterface IRP.

    ResetType - Supplies the type of the reset to perform.

    Flags - Not used. Must be zero.

    Parameters - Supplies additional reset type dependent parameters.

Return Value:

    NTSTATUS

--*/

{

    PPCI_DEVICE Device;
    IO_STACK_LOCATION IrpStack;
    DEVICE_RESET_INTERFACE_STANDARD ResetInterface;
    PFUNCTION_LEVEL_DEVICE_RESET_PARAMETERS ResetParameters;
    USHORT Size;
    NTSTATUS Status;

    Device = (PPCI_DEVICE)Context;

    PCI_ASSERT(PCI_DEVICE_EXTENSION(Device));

    if (ResetType == FunctionLevelDeviceReset) {
        Status = PciAttemptDeviceReset(Device);

        NT_ASSERT(Status != STATUS_PENDING);

        ResetParameters = (PFUNCTION_LEVEL_DEVICE_RESET_PARAMETERS)Parameters;
        if ((ResetParameters != NULL) &&
            (ResetParameters->Size >= sizeof(*ResetParameters))) {

            ResetParameters->DeviceResetCompletion(
                Status,
                ResetParameters->CompletionContext
                );
        }

    //
    // For platform-level device reset (PLDR) requests, query for the PLDR
    // interface from the parent and issue PLDR on the parent. PLDR requests
    // should only be forwarded one level up to the parent port/bridge (to
    // avoid PLDR going up to the root).
    //

    } else if ((ResetType == PlatformLevelDeviceReset) &&
               (Device->HeaderType == PCI_DEVICE_TYPE)) {

        Size = sizeof(DEVICE_RESET_INTERFACE_STANDARD);
        RtlZeroMemory(&ResetInterface, Size);
        IrpStack.MajorFunction = IRP_MJ_PNP;
        IrpStack.MinorFunction = IRP_MN_QUERY_INTERFACE;
        IrpStack.Parameters.QueryInterface.InterfaceType =
            (LPGUID)&GUID_DEVICE_RESET_INTERFACE_STANDARD;

        IrpStack.Parameters.QueryInterface.Version =
            DEVICE_RESET_INTERFACE_VERSION;

        IrpStack.Parameters.QueryInterface.Size = Size;
        IrpStack.Parameters.QueryInterface.Interface =
            (PINTERFACE)&ResetInterface;

        IrpStack.Parameters.QueryInterface.InterfaceSpecificData = NULL;
        Status = PciSendPnpIrp(Device->Parent->DeviceObject, &IrpStack, NULL);
        if (NT_SUCCESS(Status) && (ResetInterface.DeviceReset != NULL)) {
            Status = ResetInterface.DeviceReset(
                         ResetInterface.Context,
                         PlatformLevelDeviceReset,
                         Flags,
                         NULL
                         );

            if (ResetInterface.InterfaceDereference != NULL) {
                ResetInterface.InterfaceDereference(ResetInterface.Context);
            }
        }

    } else {

        Status = STATUS_NOT_SUPPORTED;
    }

    return Status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciDeviceResetInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )

/*++

Routine Description:

    This routine initializes the device reset interface.

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

    PDEVICE_RESET_INTERFACE_STANDARD Interface;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(DEVICE_RESET_INTERFACE_STANDARD)) {
        return STATUS_INVALID_PARAMETER;
    }

    Interface = (PDEVICE_RESET_INTERFACE_STANDARD)InterfaceReturn;
    Interface->Size = sizeof(DEVICE_RESET_INTERFACE_STANDARD);
    Interface->Context = DeviceExtension;
    Interface->InterfaceReference = PciInterface_Noop;
    Interface->InterfaceDereference = PciInterface_Noop;
    Interface->DeviceReset = PciDeviceResetInterface_DeviceReset;
    Interface->SupportedResetTypes =
        PciQuerySupportedResetTypes(DeviceExtension);

    return STATUS_SUCCESS;
}

_Function_class_(PCI_SET_ACS)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
PciSecurityInterface_SetAcs (
    _Inout_ PVOID Context,
    _In_ PCI_ACS_BIT EnableSourceValidation,
    _In_ PCI_ACS_BIT EnableTranslationBlocking,
    _In_ PCI_ACS_BIT EnableP2PRequestRedirect,
    _In_ PCI_ACS_BIT EnableCompletionRedirect,
    _In_ PCI_ACS_BIT EnableUpstreamForwarding,
    _In_ PCI_ACS_BIT EnableEgressControl,
    _In_ PCI_ACS_BIT EnableDirectTranslatedP2P
    )

/*++

Routine Description:

    This routine is a wrapper, in Interface Normal Form, for
    PciVirtualizationConfigureAcs.

Arguments:

    Context - Supplies pointer to PCI_DEVICE stored in the
                   interface returned by QueryInterface IRP.

    The rest of the parameters are values of bits described in the ACS part
        of the PCI Express specification.

Return Value:

    NT Status value.

--*/

{
    PPCI_DEVICE Device;
    PCI_EXPRESS_ACS_CONTROL BitsToModify = {0};
    PCI_EXPRESS_ACS_CONTROL BitState = {0};

    PAGED_CODE();

    Device = (PPCI_DEVICE)Context;

    if (EnableSourceValidation != PciAcsBitDontCare) {
        BitsToModify.SourceValidation = 1;
        if (EnableSourceValidation == PciAcsBitEnable) {
            BitState.SourceValidation = 1;
        }
    }

    if (EnableTranslationBlocking != PciAcsBitDontCare) {
        BitsToModify.TranslationBlocking = 1;
        if (EnableTranslationBlocking == PciAcsBitEnable) {
            BitState.TranslationBlocking = 1;
        }
    }

    if (EnableP2PRequestRedirect != PciAcsBitDontCare) {
        BitsToModify.RequestRedirect = 1;
        if (EnableP2PRequestRedirect == PciAcsBitEnable) {
            BitState.RequestRedirect = 1;
        }
    }

    if (EnableCompletionRedirect != PciAcsBitDontCare) {
        BitsToModify.CompletionRedirect = 1;
        if (EnableCompletionRedirect == PciAcsBitEnable) {
            BitState.CompletionRedirect = 1;
        }
    }

    if (EnableUpstreamForwarding != PciAcsBitDontCare) {
        BitsToModify.UpstreamForwarding = 1;
        if (EnableUpstreamForwarding == PciAcsBitEnable) {
            BitState.UpstreamForwarding = 1;
        }
    }

    if (EnableEgressControl != PciAcsBitDontCare) {
        BitsToModify.EgressControl = 1;
        if (EnableEgressControl == PciAcsBitEnable) {
            BitState.EgressControl = 1;
        }
    }

    if (EnableDirectTranslatedP2P != PciAcsBitDontCare) {
        BitsToModify.DirectTranslation = 1;
        if (EnableDirectTranslatedP2P == PciAcsBitEnable) {
            BitState.DirectTranslation = 1;
        }
    }

    PciVirtualizationConfigureAcs(Device, BitsToModify, BitState);

    return STATUS_SUCCESS;
}


_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciSecurityInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )

/*++

Routine Description:

    This routine initializes the security interface.

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

    PPCI_SECURITY_INTERFACE Interface;
    NTSTATUS Status;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(PCI_SECURITY_INTERFACE)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // As long as there is only an ACS-related function in the interface, check
    // for ACS support before returning the interface.
    //

    Status = PciVirtualizationCheckAcsSupport((PPCI_DEVICE)DeviceExtension);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Interface = (PPCI_SECURITY_INTERFACE)InterfaceReturn;
    Interface->Size = sizeof(PCI_SECURITY_INTERFACE);
    Interface->Context = DeviceExtension;
    Interface->InterfaceReference = PciInterface_Noop;
    Interface->InterfaceDereference = PciInterface_Noop;
    Interface->SetAccessControlServices = PciSecurityInterface_SetAcs;

    return STATUS_SUCCESS;
}
