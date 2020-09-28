/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    pciconfig.c

Abstract:

    This module implements PCI configuration space access routines.

Environment:

    Boot Environment.

--*/

// ------------------------------------------------------------------- Includes

#include "platform.h"

// -------------------------------------------------------------------- Globals

ULONG PltPciConfigTableEntries;
PPCI_CONFIG_ENTRY *PltPciConfigTable = NULL;

// ----------------------------------------------------------------- Prototypes

NTSTATUS
PltpAccessPciConfig (
    __in PPCI_CONFIG_ADDRESS DeviceAddress,
    _When_(ReadAccess != FALSE, __out_bcount(Length))
    _When_(ReadAccess == FALSE, __in_bcount(Length))
         PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length,
    __in BOOLEAN ReadAccess
    );

NTSTATUS
PltpCheckDeviceExistence (
    __in PPCI_CONFIG_ADDRESS DeviceAddress
    );

// ------------------------------------------------------------------ Functions

NTSTATUS
BlPltReadPciConfig (
    __in PPCI_CONFIG_ADDRESS DeviceAddress,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    )

/*++

Routine Description:

    This routine reads the PCI configuration space of the given device.

Arguments:

    DeviceAddress - Supplies the PCI bus, device and function number of the
        device to be accessed.

    Buffer - Supplies a pointer to the buffer which receives the configuration
        data.

    Offset - Supplies the offset into the device's configuration space that is
        to be accessed.

    Length - Supplies the number of bytes to be read.

Return Value:

    STATUS_SUCCESS if the access succeeds.

    STATUS_INVALID_PARAMETER if the device address is badly formed.

    STATUS_NO_SUCH_DEVICE if the device does not exist.

    Another error status code if the access fails.

--*/

{

    NTSTATUS Status;

    //
    // Make sure the device and function numbers are within bounds, and that
    // the device being accessed exists.
    //

    if ((DeviceAddress->Device > 0x1F) || (DeviceAddress->Function > 7)) {
        return STATUS_INVALID_PARAMETER;
    }

    Status = PltpCheckDeviceExistence(DeviceAddress);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Perform the access.
    //

    return PltpAccessPciConfig(DeviceAddress, Buffer, Offset, Length, TRUE);
}

NTSTATUS
BlPltWritePciConfig (
    __in PPCI_CONFIG_ADDRESS DeviceAddress,
    __in_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    )

/*++

Routine Description:

    This routine writes the PCI configuration space of the given device.

Arguments:

    DeviceAddress - Supplies the PCI bus, device and function number of the
        device to be accessed.

    Buffer - Supplies a pointer to the buffer containing the configuration
        data to write.

    Offset - Supplies the offset into the device's configuration space that is
        to be accessed.

    Length - Supplies the number of bytes to be written.

Return Value:

    STATUS_SUCCESS if the access succeeds.

    STATUS_INVALID_PARAMETER if the device address is badly formed.

    STATUS_NO_SUCH_DEVICE if the device does not exist.

    Another error status code if the access fails.

--*/

{

    NTSTATUS Status;

    //
    // Make sure the device and function numbers are within bounds, and that
    // the device being accessed exists.
    //

    if ((DeviceAddress->Device > 0x1F) || (DeviceAddress->Function > 7)) {
        return STATUS_INVALID_PARAMETER;
    }

    Status = PltpCheckDeviceExistence(DeviceAddress);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Perform the access.
    //

    return PltpAccessPciConfig(DeviceAddress, Buffer, Offset, Length, FALSE);
}

BOOLEAN
PltPciConfigTableMatch (
    __in PVOID Entry,
    __in PVOID Arg1,
    __in PVOID Arg2,
    __in PVOID Arg3,
    __in PVOID Arg4
    )

/*++

Routine Description:

    Compares an existing config access table entry to see if it refers to the
    requested bus.

Arguments:

    Entry - Supplies the entry in the table to compare.

    Arg1 - Supplies the segment number to compare to the table entry.

    Arg2 - Supplies a BOOLEAN indicating whether a specific bus number is to be
        matched.

    Arg3 - Supplies the bus number to match.

    Arg4 - Unused.

Return Value:

    TRUE if the existing entry matches the given bus and segment numbers.

    FALSE otherwise.

--*/

{

    UCHAR BusNumber;
    BOOLEAN MatchBusNumber;
    PPCI_CONFIG_ENTRY SegmentEntry;
    ULONG SegmentNumber;

    UNREFERENCED_PARAMETER(Arg4);

    //
    // Convert the parameters, which are typeless to meet the calling convention
    // of the match routine, to those that can be used internally here.
    //

    SegmentEntry = (PPCI_CONFIG_ENTRY)Entry;
    SegmentNumber = (ULONG)(ULONG_PTR)Arg1;
    MatchBusNumber = (BOOLEAN)(ULONG_PTR)Arg2;
    BusNumber = (UCHAR)(ULONG_PTR)Arg3;

    //
    // Perform the match.
    //

    if (SegmentEntry->SegmentNumber != SegmentNumber) {
        return FALSE;
    }

    if (MatchBusNumber != FALSE) {
        if (RtlTestBit(&SegmentEntry->BusNumberMap, BusNumber) == FALSE) {
            return FALSE;
        }
    }

    return TRUE;
}

// --------------------------------------------------------- Internal Functions

NTSTATUS
PltpAccessPciConfig (
    __in PPCI_CONFIG_ADDRESS DeviceAddress,
    _When_(ReadAccess != FALSE, __out_bcount(Length))
    _When_(ReadAccess == FALSE, __in_bcount(Length))
         PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length,
    __in BOOLEAN ReadAccess
    )

/*++

Routine Description:

    This routine converts a large PCI configuration space access request into
    naturally aligned accesses of 1, 2 or 4 bytes.

Arguments:

    DeviceAddress - Supplies the PCI bus, device and function number of the
        device to be accessed.

    Buffer - Supplies a pointer to the buffer containing the configuration
        data to write.

    Offset - Supplies the offset into the device's configuration space that is
        to be accessed.

    Length - Supplies the number of bytes to be written.

Return Value:

    STATUS_SUCCESS if the access succeeds.
    
    STATUS_NOT_SUPPORTED if configuration space access is not initialized.
    
    STATUS_INVALID_PARAMETER if configuration space cannot be accessed on the
        given segment and bus.
        
    Otherwise, a status code returned from the routine performing the access.

--*/

{
    
    ULONG AccessLength;
    ULONG AccessOffset;
    PUCHAR CurrentBuffer;
    ULONG CurrentOffset;
    ULONG EntryId;
    ULONG RemainingLength;
    PPCI_CONFIG_ENTRY SegmentEntry;
    NTSTATUS Status;

    //
    // Retrieve the config access table entry for the requested segment and bus
    // number.  This contains the routine that must be used to access config
    // space for this bus.
    //

    if (PltPciConfigTable == NULL) {
        return STATUS_NOT_SUPPORTED;
    }

    SegmentEntry = BlTblFindEntry(PltPciConfigTable,
                                  PltPciConfigTableEntries,
                                  &EntryId,
                                  PltPciConfigTableMatch,
                                  (PVOID)(ULONG_PTR)DeviceAddress->Segment,
                                  (PVOID)(ULONG_PTR)TRUE,
                                  (PVOID)(ULONG_PTR)DeviceAddress->Bus,
                                  NULL);

    if (SegmentEntry == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    CurrentOffset = Offset;
    CurrentBuffer = Buffer;
    RemainingLength = Length;
    while (RemainingLength > 0) {

        //
        // Get the offset within the DWORD register being accessed, and the
        // number of bytes that remain in this DWORD.  Accesses can never
        // cross a DWORD boundary.
        //

        AccessOffset = CurrentOffset % sizeof(ULONG);
        AccessLength = min(sizeof(ULONG) - AccessOffset, RemainingLength);

        //
        // In addition to preventing DWORD boundaries from being crossed, make
        // sure natural alignment is maintained.
        //

        if (AccessOffset == 1) {
            AccessLength = 1;
        }

        if (AccessLength == 3) {
            AccessLength = 2;
        }

        //
        // Use the access routine specified in the table to perform the access.
        //

        Status = SegmentEntry->AccessRoutine(SegmentEntry->AccessContext,
                                             DeviceAddress,
                                             CurrentBuffer,
                                             CurrentOffset,
                                             AccessLength,
                                             ReadAccess);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        CurrentOffset += AccessLength;
        CurrentBuffer = Add2Ptr(CurrentBuffer, AccessLength);
        RemainingLength -= AccessLength;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PltpCheckDeviceExistence (
    __in PPCI_CONFIG_ADDRESS DeviceAddress
    )

/*++

Routine Description:

    This routine verifies that the given device exists.  This is done since some
    accesses to devices that do not exist may cause system crashes.

Arguments:

    DeviceAddress - Supplies the device whose existence is to be checked.

Return Value:

    STATUS_SUCCESS if the existence check succeeds.

    STATUS_NO_SUCH_DEVICE if the device does not exist.

    An error status code if a configuration access failed.  This is to be
    interpreted as the device not existing.

--*/

{

    PCI_CONFIG_ADDRESS FunctionZeroAddress;
    UCHAR HeaderType;
    NTSTATUS Status;
    USHORT Vendor;

    //
    // For a non-zero function number, first check that function 0 exists
    // before attempting to access the function in question.
    //

    if (DeviceAddress->Function != 0) {
        RtlCopyMemory(&FunctionZeroAddress,
                      DeviceAddress,
                      sizeof(PCI_CONFIG_ADDRESS));

        FunctionZeroAddress.Function = 0;
        Status = PltpAccessPciConfig(&FunctionZeroAddress,
                                     &Vendor,
                                     0,
                                     sizeof(USHORT),
                                     TRUE);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        if ((Vendor == 0) || (Vendor == 0xFFFF)) {
            return STATUS_NO_SUCH_DEVICE;
        }

        Status = PltpAccessPciConfig(&FunctionZeroAddress,
                                     &HeaderType,
                                     FIELD_OFFSET(PCI_COMMON_CONFIG, HeaderType),
                                     sizeof(UCHAR),
                                     TRUE);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        if (CHECK_FLAG(HeaderType, PCI_MULTIFUNCTION) == FALSE) {
            return STATUS_NO_SUCH_DEVICE;
        }
    }

    //
    // Read the vendor ID of the function itself to verify that it exists.
    //

    Status = PltpAccessPciConfig(DeviceAddress,
                                 &Vendor,
                                 0,
                                 sizeof(USHORT),
                                 TRUE);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    if ((Vendor == 0) || (Vendor == 0xFFFF)) {
        return STATUS_NO_SUCH_DEVICE;
    }

    return STATUS_SUCCESS;
}

