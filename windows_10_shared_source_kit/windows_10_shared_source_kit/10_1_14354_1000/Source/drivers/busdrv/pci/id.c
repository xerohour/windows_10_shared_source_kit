/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    id.c

Abstract:

    This module contains functions used in the generation of responses
    to a IRP_MN_QUERY_ID IRP.  The IDs returned have one of two prefixes.  When
    the PCI driver is exposing the device to an unvirtualized OS, or a Hyper-V
    "parent" OS, the prefix is "PCI\" which is what all the IHVs have always
    used in their INFs to match their devices.  When a device has been marked
    (through the control device object interface) as "offline" or, in
    PowerShell language, "dismounted," then it is exposed with the prefix
    "PCIP\".  This prefix is meant to be used by INFs which bind a driver stack
    that acts as a proxy in the PnP system of this operating system while the
    device is actually in use by another (guest) operating system.  We change
    the prefix here rather than the root PnP IDs because we want the ability
    to filter or otherwise form vendor-specific matches on the device.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines

#define PCI_SLOT_ID(_Slot)     \
    (UCHAR)(((_Slot).u.bits.DeviceNumber << 3) | (_Slot).u.bits.FunctionNumber)

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciConstructMessageString (
    __in ULONG MessageNumber,
    __in_opt PWSTR Arguments,
    __deref_out PWSTR *MessageString
    );

PWSTR
PciGetDescriptionMessage(
    __in ULONG MessageNumber,
    __out_opt PUSHORT MessageLength
    );

PWSTR
PciGetDeviceDescriptionMessage(
    __in UCHAR BaseClass,
    __in UCHAR SubClass
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciQueryDeviceID)
    #pragma alloc_text(PAGE, PciQueryInstanceID)
    #pragma alloc_text(PAGE, PciQueryHardwareIDs)
    #pragma alloc_text(PAGE, PciQueryCompatibleIDs)
    #pragma alloc_text(PAGE, PciQueryDeviceText)
    #pragma alloc_text(PAGE, PciConstructMessageString)
#endif

// --------------------------------------------------------------------- Globals
// ------------------------------------------------------------------- Functions
 
NTSTATUS
PciQueryDeviceID(
    __in PPCI_DEVICE Device,
    __out_bcount(*BufferLength) PWCHAR Buffer,
    __inout PULONG BufferLength
    )
{
    SIZE_T bufferSize, stringSize, bufferRemaining;
    NTSTATUS status;
    ULONG subsystem;

    PAGED_CODE();

    bufferSize = (SIZE_T)*BufferLength;

    subsystem = (Device->SubSystemID << 16) | Device->SubVendorID;

    //
    // The Device ID is the most specific hardware ID, which is:
    //
    // PCI\VEN_vvvv&DEV_dddd&SUBSYS_ssssssss&REV_rr
    //
    status = RtlStringCbPrintfExW(
                Buffer, bufferSize,
                NULL, &bufferRemaining, 0,
                L"%s\\VEN_%04X&DEV_%04X&SUBSYS_%08X&REV_%02X",
                Device->PciOwned ? s_PciId : s_VmProxyId,
                Device->VendorID,
                Device->DeviceID,
                subsystem,
                Device->RevisionID
                );

    if (!NT_SUCCESS(status)) {
        ERROR("PciQueryDeviceID: failed to create device ID string (0x%08X).\n",
              status);

        return status;
    }

    //
    // Figure out how large the string is.
    // Add a sizeof(WCHAR) for the UNICODE_NULL at the end.
    //
    stringSize = bufferSize - bufferRemaining + sizeof(WCHAR);
    PCI_ASSERT(stringSize <= MAXULONG);

    *BufferLength = (ULONG)stringSize;

    return STATUS_SUCCESS;
}

NTSTATUS
PciQueryInstanceID(
    __in PPCI_DEVICE Device,
    __out_bcount(*BufferLength) PWCHAR Buffer,
    __inout PULONG BufferLength
    )
{
    SIZE_T bufferSize, bufferRemaining;
    SIZE_T stringLength;
    PWCHAR bufferPtr;
    PPCI_BUS parentBus;
    UCHAR devFunc;
    NTSTATUS status;

    PAGED_CODE();

    bufferPtr = Buffer;
    bufferSize = bufferRemaining = *BufferLength;
    status = STATUS_SUCCESS;

    if ((Device->ExpressPort != NULL) &&
    (Device->InstanceIdSerialNumber != 0)) {

        //
        // Certain Express devices have serial number capability.
        // As serial numbers are unique, these can be used as an Instance ID
        // for the device. Windows requires the combination of the Device ID
        // and the Instance ID to be unique on the system.
        //
        // N. B. The PCI Express Base Specification requires that all multi-
        //       function devices that implement the serial number capability
        //       must return the same Device Serial Number value as that
        //       reported by function 0.
        //       In addition, a PCI Express multi-device component such as a
        //       PCI Express Switch that implements this capability must return
        //       the same device serial number for each device. It is also
        //       legal for each of these devices to use the same Device ID.
        //       So, in order to make an Instance ID a truely unique field for
        //       a given Device ID, append the Slot information for the device
        //       after the serial number.
        //
        //       Upstream switchports commonly share a slot ID with one of
        //       their associated downstream switchports.  In this case
        //       the ID would not be truly unique.  'U' is prepended to
        //       upstream switchport IDs so that they remain unique.
        //

        if (Device->ExpressPort->DeviceType == PciExpressUpstreamSwitchPort) {
            status = RtlStringCbPrintfExW(bufferPtr,
                                          bufferRemaining,
                                          &bufferPtr,
                                          &bufferRemaining,
                                          0,
                                          L"U");

            if (!NT_SUCCESS(status)) {
                goto cleanup;
            }
        }

        status = RtlStringCbPrintfExW(bufferPtr,
                                      bufferRemaining,
                                      &bufferPtr,
                                      &bufferRemaining,
                                      0,
                                      L"%016llX",
                                      Device->InstanceIdSerialNumber);

        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        devFunc = PCI_SLOT_ID(Device->Slot);
        status = RtlStringCbPrintfExW(bufferPtr,
                                      bufferRemaining,
                                      &bufferPtr,
                                      &bufferRemaining,
                                      0,
                                      L"%02X",
                                      devFunc);

        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

    } else {

        //
        // The instance ID is of the form-
        //
        //  AABBCCDDEEFF...XXYYZZ
        //
        // Where AA is the slot number (device/function) of the device
        // on the bus, BB, CC,... XX, YY, ZZ are the slot number of the
        // PCI-to-PCI bridges on their parent busses all the way up to
        // the root.   A device on the root bus will have only one entry,
        // AA.
        //
        // The PCI_SLOT_NUMBER structure is a ULONG, so make sure that
        // only the lowest byte (which contains the useful information
        // anyway) is the only one printed.
        //

        devFunc = PCI_SLOT_ID(Device->Slot);
        parentBus = Device->Parent;

        while (TRUE) {

            status = RtlStringCbPrintfExW(
                        bufferPtr,
                        bufferRemaining,
                        &bufferPtr,
                        &bufferRemaining,
                        0,
                        L"%02X",
                        devFunc
                        );

            if (!NT_SUCCESS(status)) {
                goto cleanup;
            }

            if (!parentBus->PciBridge) {
                break;
            }

            devFunc = PCI_SLOT_ID(parentBus->PciBridge->Slot);
            parentBus = parentBus->ParentBus;
        }
    }

    //
    // Figure out how large the buffer is.
    // Add a sizeof(WCHAR) for the UNICODE_NULL at the end.
    //
    stringLength = bufferSize - bufferRemaining + sizeof(WCHAR);
    PCI_ASSERT(stringLength <= MAXULONG);

    *BufferLength = (ULONG)stringLength;

    return STATUS_SUCCESS;

cleanup:

    PCI_ASSERT(NT_SUCCESS(status));
    *BufferLength = 0;

    return status;
}

NTSTATUS
PciQueryHardwareIDs(
    __in PPCI_DEVICE Device,
    __out_bcount(*BufferLength) PWCHAR Buffer,
    __inout PULONG BufferLength
    )
{
    SIZE_T bufferSize, bufferRemaining;
    SIZE_T stringLength;
    PWCHAR bufferPtr;
    NTSTATUS status;
    ULONG subsystem;

    PAGED_CODE();

    bufferPtr = Buffer;
    bufferSize = bufferRemaining = *BufferLength;

    status = STATUS_SUCCESS;

    subsystem = (Device->SubSystemID << 16) | Device->SubVendorID;

    //
    // Hardware and Compatible IDs are generated as specified
    // in the ACPI spec (section 6.1.2 in version 0.9).
    //
    // Hardware IDs are a list of identifiers of the form
    //
    //  PCI\VEN_vvvv&DEV_dddd&SUBSYS_ssssssss&REV_rr
    //  PCI\VEN_vvvv&DEV_dddd&SUBSYS_ssssssss
    //
    // Where vvvv is the Vendor ID from config space,
    //       dddd is the Device ID,
    //       ssssssss is the Subsystem ID/Subsystem Vendor ID, and
    //       rr   is the Revision ID.
    // VmProxy Device Id uses PCIP instead
    //
    status = RtlStringCbPrintfExW(
                bufferPtr, bufferRemaining,
                &bufferPtr, &bufferRemaining, 0,
                L"%s\\VEN_%04X&DEV_%04X&SUBSYS_%08X&REV_%02X",
                Device->PciOwned ? s_PciId : s_VmProxyId,
                Device->VendorID,
                Device->DeviceID,
                subsystem,
                Device->RevisionID
                );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }
    //
    // Skip over the NULL, since we're building a Multi-SZ
    //
    bufferPtr++;
    bufferRemaining -= sizeof(WCHAR);

    status = RtlStringCbPrintfExW(
                bufferPtr,
                bufferRemaining,
                &bufferPtr,
                &bufferRemaining,
                0,
                L"%s\\VEN_%04X&DEV_%04X&SUBSYS_%08X",
                Device->PciOwned ? s_PciId : s_VmProxyId,
                Device->VendorID,
                Device->DeviceID,
                subsystem
                );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    bufferPtr++;
    bufferRemaining -= sizeof(WCHAR);

    //
    // If there is a subsystem ID, plain old ven/dev matches
    // are compatible IDs.  If there is no subsystem ID,
    // these are hardware IDs.
    //
    //  PCI\VEN_vvvv&DEV_dddd&REV_rr
    //  PCI\VEN_vvvv&DEV_dddd
    //  VmProxy device uses PCIP instead
    //
    if (subsystem == 0) {

        status = RtlStringCbPrintfExW(
                    bufferPtr,
                    bufferRemaining,
                    &bufferPtr,
                    &bufferRemaining,
                    0,
                    L"%s\\VEN_%04X&DEV_%04X&REV_%02X",
                    Device->PciOwned ? s_PciId : s_VmProxyId,
                    Device->VendorID,
                    Device->DeviceID,
                    Device->RevisionID
                    );
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        bufferPtr++;
        bufferRemaining -= sizeof(WCHAR);

        status = RtlStringCbPrintfExW(
                    bufferPtr,
                    bufferRemaining,
                    &bufferPtr,
                    &bufferRemaining,
                    0,
                    L"%s\\VEN_%04X&DEV_%04X",
                    Device->PciOwned ? s_PciId : s_VmProxyId,
                    Device->VendorID,
                    Device->DeviceID
                    );
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        bufferPtr++;
        bufferRemaining -= sizeof(WCHAR);
    }

    //
    // The comment in the Memphis code says "Add
    // special Intel entry".  Odd that these entries
    // are absent from the spec.  They are added for
    // PIIX4 which has the same vendor and device IDs
    // for two different sub class codes.
    //
    // These two entries are
    //
    //  PCI\VEN_vvvv&DEV_dddd&CC_ccsspp
    //  PCI\VEN_vvvv&DEV_dddd&CC_ccss
    //  VmProxy Device Id uses PCIP instead
    //
    // (See below for cc, ss and pp explanaitions).
    //
    status = RtlStringCbPrintfExW(
                  bufferPtr,
                  bufferRemaining,
                  &bufferPtr,
                  &bufferRemaining,
                  0,
                  L"%s\\VEN_%04X&DEV_%04X&CC_%02X%02X%02X",
                  Device->PciOwned ? s_PciId : s_VmProxyId,
                  Device->VendorID,
                  Device->DeviceID,
                  Device->BaseClass,
                  Device->SubClass,
                  Device->ProgIf
                  );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    bufferPtr++;
    bufferRemaining -= sizeof(WCHAR);

    status = RtlStringCbPrintfExW(
                bufferPtr,
                bufferRemaining,
                &bufferPtr,
                &bufferRemaining,
                0,
                L"%s\\VEN_%04X&DEV_%04X&CC_%02X%02X",
                Device->PciOwned ? s_PciId : s_VmProxyId,
                Device->VendorID,
                Device->DeviceID,
                Device->BaseClass,
                Device->SubClass
                );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    bufferPtr++;
    bufferRemaining -= sizeof(WCHAR);

    status = RtlStringCbPrintfExW(
                bufferPtr,
                bufferRemaining,
                &bufferPtr,
                &bufferRemaining,
                0,
                L""
                );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    bufferPtr++;
    bufferRemaining -= sizeof(WCHAR);

    //
    // Figure out how large the buffer is.
    // No extra sizeof(WCHAR) for the UNICODE_NULL at the end, since
    // we've been calculating those in the whole time.
    //
    stringLength = bufferSize - bufferRemaining;
    PCI_ASSERT(stringLength <= MAXULONG);

    *BufferLength = (ULONG)stringLength;

    return STATUS_SUCCESS;

cleanup:

    PCI_ASSERT(NT_SUCCESS(status));
    *BufferLength = 0;

    return status;
}

NTSTATUS
PciQueryCompatibleIDs(
    __in PPCI_DEVICE Device,
    __out_bcount(*BufferLength) PWCHAR Buffer,
    __inout PULONG BufferLength
    )
{
    SIZE_T bufferSize, bufferRemaining;
    SIZE_T stringLength;
    PWCHAR bufferPtr;
    ULONG subsystem;
    NTSTATUS status;
    BOOLEAN thunderboltCompatible;

    PAGED_CODE();

    bufferPtr = Buffer;
    bufferSize = bufferRemaining = *BufferLength;

    status = STATUS_SUCCESS;

    subsystem = (Device->SubSystemID << 16) | Device->SubVendorID;

    //
    // If there is a subsystem ID, plain old ven/dev matches
    // are compatible IDs.  If there is no subsystem ID,
    // these are hardware IDs.
    //
    //  PCI\VEN_vvvv&DEV_dddd&REV_rr
    //  PCI\VEN_vvvv&DEV_dddd
    //  VmProxy Device uses PCIP instead
    //
    if (subsystem != 0) {

        status = RtlStringCbPrintfExW(
                    bufferPtr,
                    bufferRemaining,
                    &bufferPtr,
                    &bufferRemaining,
                    0,
                    L"%s\\VEN_%04X&DEV_%04X&REV_%02X",
                    Device->PciOwned ? s_PciId : s_VmProxyId,
                    Device->VendorID,
                    Device->DeviceID,
                    Device->RevisionID
                    );
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        //
        // Skip over the terminating NULL since we're building a MULTI_SZ
        //
        bufferPtr++;
        bufferRemaining -= sizeof(WCHAR);

        status = RtlStringCbPrintfExW(
                    bufferPtr,
                    bufferRemaining,
                    &bufferPtr,
                    &bufferRemaining,
                    0,
                    L"%s\\VEN_%04X&DEV_%04X",
                    Device->PciOwned ? s_PciId : s_VmProxyId,
                    Device->VendorID,
                    Device->DeviceID
                    );
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        bufferPtr++;
        bufferRemaining -= sizeof(WCHAR);
    }

    //
    // The Compatible IDs list, consists of the above plus
    //
    //  PCI\VEN_vvvv&CC_ccsspp
    //  PCI\VEN_vvvv&CC_ccss
    //  PCI\VEN_vvvv
    //  PCI\CC_ccsspp&DT_d  (PCI Express only)
    //  PCI\CC_ccsspp
    //  PCI\CC_ccss&DT_d  (PCI Express only)
    //  PCI\CC_ccss
    //
    // Where cc is the Class Code from config space,
    //       ss is the Sub-Class Code,
    //       pp is the programming interface, and
    //        d is the PCIe device type.
    //
    // WARNING: Revise the size of the buffer if you increase
    //          the above list.
    // VmProxy Device uses PCIP instead
    //
    status = RtlStringCbPrintfExW(
                bufferPtr,
                bufferRemaining,
                &bufferPtr,
                &bufferRemaining,
                0,
                L"%s\\VEN_%04X&CC_%02X%02X%02X",
                Device->PciOwned ? s_PciId : s_VmProxyId,
                Device->VendorID,
                Device->BaseClass,
                Device->SubClass,
                Device->ProgIf
                );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    bufferPtr++;
    bufferRemaining -= sizeof(WCHAR);

    status = RtlStringCbPrintfExW(
                bufferPtr,
                bufferRemaining,
                &bufferPtr,
                &bufferRemaining,
                0,
                L"%s\\VEN_%04X&CC_%02X%02X",
                Device->PciOwned ? s_PciId : s_VmProxyId,
                Device->VendorID,
                Device->BaseClass,
                Device->SubClass
                );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    bufferPtr++;
    bufferRemaining -= sizeof(WCHAR);

    status = RtlStringCbPrintfExW(
                bufferPtr,
                bufferRemaining,
                &bufferPtr,
                &bufferRemaining,
                0,
                L"%s\\VEN_%04X",
                Device->PciOwned ? s_PciId : s_VmProxyId,
                Device->VendorID
                );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    bufferPtr++;
    bufferRemaining -= sizeof(WCHAR);

    if (Device->ExpressPort != NULL) {
        status = RtlStringCbPrintfExW(bufferPtr,
                                      bufferRemaining,
                                      &bufferPtr,
                                      &bufferRemaining,
                                      0,
                                      L"%s\\CC_%02X%02X%02X&DT_%01X",
                                      Device->PciOwned ? s_PciId : s_VmProxyId,
                                      Device->BaseClass,
                                      Device->SubClass,
                                      Device->ProgIf,
                                      Device->ExpressPort->DeviceType);

        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        bufferPtr++;
        bufferRemaining -= sizeof(WCHAR);
    }

    status = RtlStringCbPrintfExW(
                bufferPtr,
                bufferRemaining,
                &bufferPtr,
                &bufferRemaining,
                0,
                L"%s\\CC_%02X%02X%02X",
                Device->PciOwned ? s_PciId : s_VmProxyId,
                Device->BaseClass,
                Device->SubClass,
                Device->ProgIf
                );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    bufferPtr++;
    bufferRemaining -= sizeof(WCHAR);

    if (Device->ExpressPort != NULL) {
        status = RtlStringCbPrintfExW(bufferPtr,
                                      bufferRemaining,
                                      &bufferPtr,
                                      &bufferRemaining,
                                      0,
                                      L"%s\\CC_%02X%02X&DT_%01X",
                                      Device->PciOwned ? s_PciId : s_VmProxyId,
                                      Device->BaseClass,
                                      Device->SubClass,
                                      Device->ExpressPort->DeviceType);

        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        bufferPtr++;
        bufferRemaining -= sizeof(WCHAR);
    }

    status = RtlStringCbPrintfExW(
                bufferPtr,
                bufferRemaining,
                &bufferPtr,
                &bufferRemaining,
                0,
                L"%s\\CC_%02X%02X",
                Device->PciOwned ? s_PciId : s_VmProxyId,
                Device->BaseClass,
                Device->SubClass
                );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    bufferPtr++;
    bufferRemaining -= sizeof(WCHAR);

    //
    // If the device is a bridge and contains thunderbolt capability, add the
    // corresponding compatible class codes.
    //

    thunderboltCompatible = FALSE;
    if  ((Device->BaseClass == PCI_CLASS_BRIDGE_DEV) &&
         (Device->SubClass == PCI_SUBCLASS_BR_PCI_TO_PCI)) {

        if ((Device->ExpressPort != NULL) &&
            (Device->ExpressPort->ThunderboltCapability != 0)) {
            thunderboltCompatible = TRUE;

        } else if ((Device->ExpressCompatibilityPort != NULL) &&
             (Device->ExpressCompatibilityPort->ThunderboltCapability != 0)) {

            thunderboltCompatible = TRUE;
        }
    }

    if (thunderboltCompatible != FALSE) {
        status = RtlStringCbPrintfExW(bufferPtr,
                                      bufferRemaining,
                                      &bufferPtr,
                                      &bufferRemaining,
                                      0,
                                      L"%s\\CC_%02X%02X",
                                      Device->PciOwned ? s_PciId : s_VmProxyId,
                                      PCI_CLASS_SERIAL_BUS_CTLR,
                                      PCI_SUBCLASS_SB_THUNDERBOLT);

        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        bufferPtr++;
        bufferRemaining -= sizeof(WCHAR);
    }

    //
    // Add a generic match for discrete device assignment, so that the INF
    // for the driver has something to match on if all other more specific
    // matches fail.
    //

    if (Device->PciOwned == FALSE) {
        status = RtlStringCbPrintfExW(bufferPtr,
                                      bufferRemaining,
                                      &bufferPtr,
                                      &bufferRemaining,
                                      0,
                                      L"PCIP\\Generic");

        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        bufferPtr++;
        bufferRemaining -= sizeof(WCHAR);
    }

    status = RtlStringCbPrintfExW(
                bufferPtr,
                bufferRemaining,
                &bufferPtr,
                &bufferRemaining,
                0,
                L""
                );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    bufferPtr++;
    bufferRemaining -= sizeof(WCHAR);

    //
    // Figure out how large the buffer is.
    // No extra sizeof(WCHAR) for the UNICODE_NULL at the end, since
    // we've been calculating those in the whole time.
    //
    stringLength = bufferSize - bufferRemaining;
    PCI_ASSERT(stringLength <= MAXULONG);

    *BufferLength = (ULONG)stringLength;

    return STATUS_SUCCESS;

cleanup:

    PCI_ASSERT(NT_SUCCESS(status));
    *BufferLength = 0;

    return status;
}

NTSTATUS
PciQueryDeviceText(
    __in PPCI_DEVICE Device,
    __in DEVICE_TEXT_TYPE TextType,
    __in LCID LocaleId,
    __deref_out PWSTR *DeviceText
    )
/*++

Routine Description:

    This routine gets the descriptive device text for the given device.
    The requested text will either be for a description of the device,
    or it will be for the location of the device.  In both cases the returned
    string will not be the device text itself, but rather a string identifying
    PCI as the provider of the text and the location of the device text in
    PCI's message table resource.  This allows a localized version of the text
    to be loaded based on the current system locale.

Arguments:

    Device - The device to get device text for.

    TextType - The type of text to return.  Supported values are:
        DeviceTextDescription and DeviceTextLocationInformation

    LocaleId - The information about which localized version should
        be returned.

    DeviceText - On successful exit, will contain a pointer to the
        memory allocated to contain the device text.

Return Value:

    NT Status code.

--*/
{

    PWCHAR arguments;
    SIZE_T textLength;
    PMESSAGE_RESOURCE_ENTRY messageEntry;
    ULONG messageNumber;
    BOOLEAN passSegmentInfo;
    NTSTATUS status;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(LocaleId);

    switch (TextType) {

        //
        // Device description case.  In this case look up the message associated
        // with the device class code.  If there is no description for this
        // class code, use a default "PCI Device" description for the device.
        //

    case DeviceTextDescription:
        messageNumber = (Device->BaseClass << 8) | Device->SubClass;
#pragma prefast(suppress:__WARNING_INACCESSIBLE_MEMBER: "looking at resources")
        status = RtlFindMessage(PciDriverObject->DriverStart,
                                11, // RT_MESSAGETABLE
                                LANG_NEUTRAL,
                                messageNumber,
                                &messageEntry);

        if (!NT_SUCCESS(status)) {
            messageNumber = PCI_DEFAULT_DESCRIPTION;
        }

        status = PciConstructMessageString(messageNumber, NULL, DeviceText);
        return status;

        //
        // Location information case.  In this case the message to be
        // constructed contains the segment, bus, device and function numbers
        // of the device.  The message resource contains the text identifying
        // each portion of the location string, but this must be modified with
        // the specific numerical values for this particular device, which are
        // not in the resource.
        //

    case DeviceTextLocationInformation:
        PciAcquirePassiveLock(&PciGlobalLock);
        if ((PciSegmentList->Next == NULL) &&
            (PciSegmentList->SegmentNumber == 0)) {

            passSegmentInfo = FALSE;

        } else {
            passSegmentInfo = TRUE;
        }

        PciReleasePassiveLock(&PciGlobalLock);

        //
        // Construct a string containing the arguments to modify the format
        // string.  Space is required for up to 5 digits for segments
        // 0-65535, up to 3 digits for bus number 0-255, up to 2 digits
        // for device 0-32 and up to 1 digit for function 0-7.  Note we
        // assume standard arabic numbers.  Write these arguments into a
        // string with the following format:
        //
        // ,Segment,Bus,Device,Function
        //

        if (passSegmentInfo == FALSE) {
            messageNumber = PCI_LOCATION_TEXT;
            textLength = 3 // Bus number, max 255
                         + 2 // Device number, max 32
                         + 1 // Function number, max 8
                         + 3 // commas before and between the three args
                         + 1; // Null

            arguments = PciAllocatePool(PagedPool,
                                        textLength * sizeof(WCHAR));

            if (arguments == NULL) {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            status = RtlStringCchPrintfW(
                         arguments,
                         textLength,
                         L",%u,%u,%u",
                         LOCAL_BUS_NUMBER(Device->BusNumber),
                         (ULONG)Device->Slot.u.bits.DeviceNumber,
                         (ULONG)Device->Slot.u.bits.FunctionNumber
                         );

            if (!NT_SUCCESS(status)) {
                PciFreePool(arguments);
                return status;
            }

        } else {
            messageNumber = PCI_SEGMENT_LOCATION_TEXT;
            textLength = 5 // Segment, max 65535
                         + 3 // Bus number, max 255
                         + 2 // Device number, max 32
                         + 1 // Function number, max 8
                         + 4 // commas before and between the four args
                         + 1; // Null

            arguments = PciAllocatePool(PagedPool,
                                        textLength * sizeof(WCHAR));

            if (arguments == NULL) {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            status = RtlStringCchPrintfW(
                         arguments,
                         textLength,
                         L",%u,%u,%u,%u",
                         SEGMENT_NUMBER_FOR_BUS(Device->BusNumber),
                         LOCAL_BUS_NUMBER(Device->BusNumber),
                         (ULONG)Device->Slot.u.bits.DeviceNumber,
                         (ULONG)Device->Slot.u.bits.FunctionNumber
                         );

            if (!NT_SUCCESS(status)) {
                PciFreePool(arguments);
                return status;
            }
        }

        status = PciConstructMessageString(messageNumber,
                                           arguments,
                                           DeviceText);

        PciFreePool(arguments);
        return status;

    default:
        return STATUS_NOT_SUPPORTED;
    }
}

NTSTATUS
PciConstructMessageString (
    __in ULONG MessageNumber,
    __in_opt PWSTR Arguments,
    __deref_out PWSTR *MessageString
    )

/*++

Routine Description:

    This routine constructs a string to be returned to the PnP manager that
    points to a message table entry containing device text.  The format of the
    string is DriverRegistryPath,MessageNumber,Arguments.

Arguments:

    MessageNumber - Supplies the message number containing the device text
        string.

    Arguments - Supplies a comma-delimited set of arguments that will be used as
        replacements for %1, %2, ... argument specifiers in the message string.
        These arguments cannot contain commas themselves.  This string must
        contain a leading comma and each argument must be surrounded by double
        quotes as follows: ,"Arg1","Arg2"

    MessageString - Supplies a pointer to a variable that receives the pool
        allocated message pointer string in the format described above.

Return Value:

    NT Status code.

--*/

{

    SIZE_T size;
    NTSTATUS status;
    PWCHAR subString;
    PWCHAR text;

    PAGED_CODE();

    //
    // Allocate space for the string.  The message number is printed in decimal,
    // and can be 32 bits, which requires 10 digits.  Add another character for
    // the comma separating the registry path from the message number, and
    // another for NULL.
    //

    text = NULL;
    size = PciRegistryPath.Length + (12 * sizeof(WCHAR));
    if (ARGUMENT_PRESENT(Arguments)) {
        size += (wcslen(Arguments) * sizeof(WCHAR));
    }

    text = PciAllocatePool(PagedPool, size);
    if (text == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto ConstructMessageStringEnd;
    }

    //
    // Print the registry path and message number, comma separated.  If the
    // arguments string is specified, add it to the end.  Assume that it
    // contains a leading comma to separate it from the message number.
    //

    status = RtlStringCbCopyNExW(text,
                                 size,
                                 PciRegistryPath.Buffer,
                                 PciRegistryPath.Length,
                                 &subString,
                                 &size,
                                 0);

    if (!NT_SUCCESS(status)) {
        goto ConstructMessageStringEnd;
    }

    status = RtlStringCbPrintfW(subString, size, L",%d", MessageNumber);
    if (!NT_SUCCESS(status)) {
        goto ConstructMessageStringEnd;
    }

    if (Arguments != NULL) {
        status = RtlStringCbCatW(subString, size, Arguments);
        if (!NT_SUCCESS(status)) {
            goto ConstructMessageStringEnd;
        }
    }

    *MessageString = text;
    status = STATUS_SUCCESS;

ConstructMessageStringEnd:
    if (!NT_SUCCESS(status)) {
        if (text != NULL) {
            PciFreePool(text);
        }
    }

    return status;
}
