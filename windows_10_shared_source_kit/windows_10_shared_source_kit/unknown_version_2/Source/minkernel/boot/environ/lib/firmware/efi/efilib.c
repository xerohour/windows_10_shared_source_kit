/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efilib.c

Abstract:

    Various library routines for EFI firmware boot.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "bootlibp.h"

// ------------------------------------------------------------------ Functions

EFI_STATUS
EfiGetEfiStatusCode (
    __in NTSTATUS NtStatus
    )

/*++

Routine Description:

    This routine returns the closest equivalent EFI status code for the
    specified NT status code.

Arguments:

    NtStatus - Supplies a NT status code.

Return Value:

    EFI status code.

--*/

{

    switch (NtStatus) {
    case STATUS_SUCCESS:
        return EFI_SUCCESS;

    case STATUS_DRIVER_UNABLE_TO_LOAD:
        return EFI_LOAD_ERROR;

    case STATUS_INVALID_PARAMETER:
        return EFI_INVALID_PARAMETER;

    case STATUS_NOT_SUPPORTED:
        return EFI_UNSUPPORTED;

    case STATUS_INVALID_BUFFER_SIZE:
        return EFI_BAD_BUFFER_SIZE;

    case STATUS_BUFFER_TOO_SMALL:
        return EFI_BUFFER_TOO_SMALL;

    case STATUS_IO_DEVICE_ERROR:
        return EFI_DEVICE_ERROR;

    case STATUS_MEDIA_WRITE_PROTECTED:
        return EFI_WRITE_PROTECTED;

    case STATUS_INSUFFICIENT_RESOURCES:
        return EFI_OUT_OF_RESOURCES;

    case STATUS_DISK_CORRUPT_ERROR:
        return EFI_VOLUME_CORRUPTED;

    case STATUS_DISK_FULL:
        return EFI_VOLUME_FULL;

    case STATUS_NO_MEDIA:
        return EFI_NO_MEDIA;

    case STATUS_MEDIA_CHANGED:
        return EFI_MEDIA_CHANGED;

    case STATUS_NOT_FOUND:
        return EFI_NOT_FOUND;

    case STATUS_ACCESS_DENIED:
        return EFI_ACCESS_DENIED;

    case STATUS_NO_MATCH:
        return EFI_NO_MAPPING;

    case STATUS_TIMEOUT:
        return EFI_TIMEOUT;

    case STATUS_DEVICE_NOT_READY:
        return EFI_NOT_STARTED;

    case STATUS_DEVICE_ALREADY_ATTACHED:
        return EFI_ALREADY_STARTED;

    case STATUS_REQUEST_ABORTED:
        return EFI_ABORTED;

    default:
        return EFI_NO_MAPPING;
    }
}

NTSTATUS
EfiGetNtStatusCode (
    __in EFI_STATUS EfiStatus
    )

/*++

Routine Description:

    This routine returns the closest equivalent NT status code for the
    specified EFI status code.

Arguments:

    EfiStatus - Supplies an EFI status code.

Return Value:

    NT status code.

--*/

{

    switch (EfiStatus) {
    case EFI_SUCCESS:
        return STATUS_SUCCESS;

    case EFI_LOAD_ERROR:
        return STATUS_DRIVER_UNABLE_TO_LOAD;

    case EFI_INVALID_PARAMETER:
        return STATUS_INVALID_PARAMETER;

    case EFI_UNSUPPORTED:
        return STATUS_NOT_SUPPORTED;

    case EFI_BAD_BUFFER_SIZE:
        return STATUS_INVALID_BUFFER_SIZE;

    case EFI_BUFFER_TOO_SMALL:
        return STATUS_BUFFER_TOO_SMALL;

    case EFI_NOT_READY:
        return STATUS_NOT_FOUND;

    case EFI_DEVICE_ERROR:
        return STATUS_IO_DEVICE_ERROR;

    case EFI_WRITE_PROTECTED:
        return STATUS_MEDIA_WRITE_PROTECTED;

    case EFI_OUT_OF_RESOURCES:
        return STATUS_INSUFFICIENT_NVRAM_RESOURCES;

    case EFI_VOLUME_CORRUPTED:
        return STATUS_DISK_CORRUPT_ERROR;

    case EFI_VOLUME_FULL:
        return STATUS_DISK_FULL;

    case EFI_NO_MEDIA:
        return STATUS_NO_MEDIA;

    case EFI_MEDIA_CHANGED:
        return STATUS_MEDIA_CHANGED;

    case EFI_NOT_FOUND:
        return STATUS_NOT_FOUND;

    case EFI_ACCESS_DENIED:
    case EFI_SECURITY_VIOLATION:
        return STATUS_ACCESS_DENIED;

    case EFI_NO_RESPONSE:
        return STATUS_TIMEOUT;

    case EFI_NO_MAPPING:
        return STATUS_NO_MATCH;

    case EFI_TIMEOUT:
        return STATUS_TIMEOUT;

    case EFI_NOT_STARTED:
        return STATUS_DEVICE_NOT_READY;

    case EFI_ALREADY_STARTED:
        return STATUS_DEVICE_ALREADY_ATTACHED;

    case EFI_ABORTED:
        return STATUS_REQUEST_ABORTED;

    case EFI_ICMP_ERROR:
    case EFI_TFTP_ERROR:
    case EFI_PROTOCOL_ERROR:
    //case EFI_INCOMPATIBLE_VERSION:
    //case EFI_CRC_ERROR:
    default:
        return STATUS_UNSUCCESSFUL;
    }
}

VOID
EfiPrintf (
    __in PWSTR Format,
    ...
    )

/*++

Routine Description:

    Simple print routine.  Defined as ConsolePrint and used for debug purposes
    when console display does not work.

Arguments:

    Format - printf style format

    ... - arguments.

Return Value:

    none

--*/

{

    va_list Arglist;
    LONG Ret;
    ULONG Size;

    va_start(Arglist, Format);
    Size = GLOBAL_SCRATCH_BUFFER_SIZE/sizeof(WCHAR) - 1;
    Ret = vswprintf_s((PWCHAR)BlScratchBuffer, Size, Format, Arglist);
    if (Ret > 0) {
        ((PWCHAR)BlScratchBuffer)[Size] = L'\0';
        EfiPrint((PWCHAR)BlScratchBuffer);
    }
}

EFI_DEVICE_PATH UNALIGNED *
EfiGetLeafNode (
    __in EFI_DEVICE_PATH UNALIGNED *DevicePath
    )

/*++

Routine Description:

    Get the last node of an EFI device path.

Arguments:

    DevicePath - Efi device path.

Return Value:

    A pointer to the last node in the device path.

--*/

{

    EFI_DEVICE_PATH UNALIGNED *Dp;

    ASSERT(DevicePath != NULL);

    if (IsDevicePathEndType(DevicePath)) {
        return DevicePath;
    }

    //
    // Walk down the device path, have DevicePath lag by one node.
    //

    Dp = NextDevicePathNode(DevicePath);
    while (!IsDevicePathEndType(Dp)) {
        DevicePath = Dp;
        Dp = NextDevicePathNode(Dp);
    }

    return DevicePath;
}

HARDDRIVE_DEVICE_PATH UNALIGNED *
EfiGetFirstHardDriveNode (
    __in EFI_DEVICE_PATH UNALIGNED *DevicePath
    )

/*++

Routine Description:

    This routine returns the first hard drive node in an EFI device path.

Arguments:

    DevicePath - Supplies a pointer to the first node in an EFI device path.

Return Value:

    A pointer to the first hard drive node found. NULL if no such node was
    found.

--*/

{

    EFI_DEVICE_PATH UNALIGNED *CurrentNode;

    ASSERT(DevicePath != NULL);

    CurrentNode = DevicePath;
    while ((CurrentNode != NULL) &&
           (IsDevicePathEndType(CurrentNode) == FALSE)) {

        if ((CurrentNode->Type == MEDIA_DEVICE_PATH) &&
            (CurrentNode->SubType == MEDIA_HARDDRIVE_DP)) {

            return (HARDDRIVE_DEVICE_PATH UNALIGNED *)CurrentNode;
        }

        CurrentNode = NextDevicePathNode(CurrentNode);
    }

    return NULL;
}

EFI_DEVICE_PATH UNALIGNED *
EfiIsDevicePathParent (
    __in EFI_DEVICE_PATH UNALIGNED *DevicePath1,
    __in EFI_DEVICE_PATH UNALIGNED *DevicePath2
    )

/*++

Routine Description:

    Determines the relationship between DevicePath1 and DevicePath2.  In
    particular, this routine determines if either is the parent of the
    other.

Arguments:

    DevicePath1 - Efi device path.

    DevicePath2 - Efi device path.

Return Value:

    DevicePath1 if DevicePath2 is the child of DevicePath1.

    DevicePath2 if DevicePath1 is the child of DevicePath2.

    NULL otherwise.

--*/

{

    EFI_DEVICE_PATH UNALIGNED *Dp1;
    EFI_DEVICE_PATH UNALIGNED *Dp2;
    UINTN Num;

    Dp1 = DevicePath1;
    Dp2 = DevicePath2;

    //
    // Iterate through the device paths comparing each node.  Return from the
    // function if the nodes ever differ.  That way, if we ever break out of
    // the loop, one of the two paths is the parent of the other.
    //

    while (!IsDevicePathEndType(Dp1) && !IsDevicePathEndType(Dp2)) {

        //
        // Check to make sure this node is the same for both device paths.  If
        // not, return now.
        //

        if (DevicePathNodeLength(Dp1) != DevicePathNodeLength(Dp2)) {
            return NULL;
        }

        Num = RtlCompareMemory(Dp1, Dp2, DevicePathNodeLength(Dp1));
        if (Num != (UINTN)DevicePathNodeLength(Dp1)) {
            return NULL;
        }

        //
        // Try the next node.
        //

        Dp1 = NextDevicePathNode(Dp1);
        Dp2 = NextDevicePathNode(Dp2);
    }

    //
    // Catch the case where DevicePath1 == DevicePath2.
    //

    if (IsDevicePathEndType(Dp1) && IsDevicePathEndType(Dp2)) {
        return NULL;
    }

    //
    // We iterated through the device path's and the are equivalent.  The
    // non-null device path is the child.
    //

    return IsDevicePathEndType(Dp1) ? DevicePath1 : DevicePath2;
}

