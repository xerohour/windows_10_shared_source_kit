/*++

Copyright (c) Microsoft Corporation

Module Name:

    bootstat.c

Abstract:

    Manipulates the boot status data file.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"
#include "systemevents.h"

//
// ---------------------------------------------------------------- Definitions
//

#define FIELD_OFFSET_AND_SIZE(n) { FIELD_OFFSET(BSD_BOOT_STATUS_DATA, n), \
                                   FIELD_SIZE(BSD_BOOT_STATUS_DATA, n) }

#define BOOTSTAT_FILE_NAME L"bootstat.dat"

typedef struct _BOOT_STATUS_FIELD {
    ULONG FieldOffset;
    ULONG FieldLength;
} BOOT_STATUS_FIELD, *PBOOT_STATUS_FIELD;

//
// ------------------------------------------------- Local Function Prototypes
//

NTSTATUS
OslpGetBootStatusData (
    __in RTL_BSD_ITEM_TYPE DataItem,
    __out PVOID Buffer,
    __inout PULONG Size
    );

NTSTATUS
OslpGetSetBootStatusData (
    __in BOOLEAN Get,
    __in RTL_BSD_ITEM_TYPE DataItem,
    __inout PVOID Buffer,
    __inout PULONG Size
    );

NTSTATUS
OslpReadWriteBootStatusData (
    __in BOOLEAN Write
    );

NTSTATUS
OslpSetBootStatusData (
    __in RTL_BSD_ITEM_TYPE DataItem,
    __in PVOID Buffer,
    __inout PULONG Size
    );

//
// --------------------------------------------------------------------- Locals
//

static const BOOT_STATUS_FIELD OslpBootStatusFields[] = {
    FIELD_OFFSET_AND_SIZE(Version),
    FIELD_OFFSET_AND_SIZE(ProductType),
    FIELD_OFFSET_AND_SIZE(AutoAdvancedBoot),
    FIELD_OFFSET_AND_SIZE(AdvancedBootMenuTimeout),
    FIELD_OFFSET_AND_SIZE(LastBootSucceeded),
    FIELD_OFFSET_AND_SIZE(LastBootShutdown),
    FIELD_OFFSET_AND_SIZE(SleepInProgress),
    FIELD_OFFSET_AND_SIZE(PowerTransition),
    FIELD_OFFSET_AND_SIZE(BootAttemptCount),
    FIELD_OFFSET_AND_SIZE(LastBootCheckpoint),
    FIELD_OFFSET_AND_SIZE(LastBootId),
    FIELD_OFFSET_AND_SIZE(LastSuccessfulShutdownBootId),
    FIELD_OFFSET_AND_SIZE(LastReportedAbnormalShudownBootId),
};

C_ASSERT(RtlBsdItemMax == (sizeof(OslpBootStatusFields) /
                           sizeof(OslpBootStatusFields[0])));

static PBSD_BOOT_STATUS_DATA BsdBootStatusData;

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
OslGetBootStatusData (
    __out PBOOLEAN LastBootSucceeded,
    __out PBOOLEAN LastBootShutdown,
    __out PBOOLEAN LastBootCheckpoint,
    __out PUCHAR BootAttemptCount,
    __out PULONG LastBootId,
    __out PBOOLEAN UnfilteredBootSucceeded,
    __out PBOOLEAN UnfilteredBootShutdown
    )

/*++

Routine Description:

    Reads the previous boot status from the log file.

Arguments:

    LastBootSucceeded - Supplies a pointer to a boolean value that receives TRUE
        if the last boot was successful after filtering based on the boot status
        policy, else FALSE.

    LastBootShutdown - Supplies a pointer to a boolean value that receives TRUE
        if the last OS shutdown successfully after filtering based on the boot
        status policy, else FALSE.

    LastBootCheckpoint - Supplies a pointer to a boolean value that receives
        TRUE if the last boot reached the boot checkpoint successfully, else
        FALSE.

    BootAttemptCount - Supplies a pointer to receive the number of boot
        attempts that have been made since the last successful boot.

    LastBootId - Supplies a pointer to receive the boot sequence for last 
        boot attempt.

    UnfilteredBootSucceeded - Supplies a pointer to a boolean value that
        receives TRUE if the last boot was successful, else FALSE.

    UnfilteredBootShutdown - Supplies a pointer to a boolean value that
        receives TRUE if the last OS shutdown successfully, else FALSE.

Return Value:

    STATUS_SUCCESS, or an appropriate error status.

--*/

{

    UCHAR AttemptCount;
    ULONGLONG BootStatusPolicy;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[2];
    ULONG LastBootStatusBuffer[2];
    BOOLEAN LastCheckpointOk;
    BOOLEAN LastShutdownOk;
    BOOLEAN LastBootOk;
    ULONG Size;
    NTSTATUS Status;

    //
    // Read the boot status data structure from the boot status file, caching it
    // for subsequent reads.
    //

    Status = OslpReadWriteBootStatusData(FALSE);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Capture the last boot sequence. Ignore any errors, as this is not critical.
    //

    Size = sizeof(ULONG);
    Status = OslpGetBootStatusData(RtlBsdItemBootId,
                                   LastBootId,
                                   &Size);

    if (!NT_SUCCESS(Status)) {
        *LastBootId = 0;
    }

    //
    // Check the BCD policy for handling boot status failures.
    //

    Status = BlGetApplicationOptionInteger(
                 BCDE_OSLOADER_TYPE_BOOT_STATUS_POLICY,
                 &BootStatusPolicy);

    if (!NT_SUCCESS(Status)) {
        BootStatusPolicy = BsPolicyIgnoreShutdownFailures;
    }

    //
    // Capture boot/shutdown status from the last OS boot. Do this no matter
    // what the policy is in order to send the actual boot/shutdown status to
    // SQM.
    //

    Size = sizeof(BOOLEAN);
    Status = OslpGetBootStatusData(RtlBsdItemBootShutdown,
                                   &LastShutdownOk,
                                   &Size);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    *UnfilteredBootShutdown = LastShutdownOk;
    Size = sizeof(BOOLEAN);
    Status = OslpGetBootStatusData(RtlBsdItemBootGood, &LastBootOk, &Size);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    *UnfilteredBootSucceeded = LastBootOk;

    //
    // Log the boot status information from the last boot to ETW for SQM
    // pick-up.
    //

    LastBootStatusBuffer[0] = (ULONG)LastShutdownOk;
    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)&LastBootStatusBuffer[0],
                        sizeof(ULONG));

    LastBootStatusBuffer[1] = (ULONG)LastBootOk;
    EventDataDescCreate(&EventDataDescriptor[1],
                        (PVOID)&LastBootStatusBuffer[1],
                        sizeof(ULONG));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_DIRTY_BOOT_SHUTDOWN,
                  NULL,
                  2,
                  EventDataDescriptor);

    //
    // Only look at the checkpoint value if the system booted properly but did
    // not shutdown properly.
    //

    if ((LastBootOk != FALSE) && (LastShutdownOk == FALSE)) {
        Size = sizeof(BOOLEAN);
        Status = OslpGetBootStatusData(RtlBsdItemBootCheckpoint,
                                       &LastCheckpointOk,
                                       &Size);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

    } else {
        LastCheckpointOk = TRUE;
    }

    //
    // Only look at the real value of the shutdown flag if the previous boot
    // succeeded and the checkpoint was reached.
    //

    if ((LastBootOk == FALSE) || (LastCheckpointOk == FALSE)) {
        LastShutdownOk = TRUE;
    }

    //
    // After logging the real values read from disk, override the real values
    // based on policy.
    //

    switch (BootStatusPolicy) {
        case BsPolicyIgnoreShutdownFailures:
            LastShutdownOk = TRUE;
            break;

        case BsPolicyIgnoreBootFailures:
            LastBootOk = TRUE;
            break;

        case BsPolicyIgnoreCheckpointFailures:
            LastCheckpointOk = TRUE;
            break;

        case BsPolicyIgnoreAllFailures:
            LastShutdownOk = TRUE;
            LastBootOk = TRUE;
            LastCheckpointOk = TRUE;
            break;

        case BsPolicyDisplayShutdownFailures:
            LastBootOk = TRUE;
            LastCheckpointOk = TRUE;
            break;

        case BsPolicyDisplayBootFailures:
            LastCheckpointOk = TRUE;
            LastShutdownOk = TRUE;
            break;

        case BsPolicyDisplayCheckpointFailures:
            LastBootOk = TRUE;
            LastShutdownOk = TRUE;
            break;

        default:
            break;
    }

    //
    // With the policy applied, if the last boot did not succeed or the last
    // boot checkpoint was not reached, read the attempt count from the boot
    // status file. Otherwise, reset the counter.
    //

    if ((LastBootOk == FALSE) || (LastCheckpointOk == FALSE)) {
        Size = sizeof(UCHAR);
        Status = OslpGetBootStatusData(RtlBsdItemBootAttemptCount,
                                       &AttemptCount,
                                       &Size);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

    } else {
        AttemptCount = DEFAULT_BOOT_ATTEMPT_COUNT;
    }

    *LastBootSucceeded = LastBootOk;
    *LastBootShutdown = LastShutdownOk;
    *LastBootCheckpoint = LastCheckpointOk;
    *BootAttemptCount = AttemptCount;
    return STATUS_SUCCESS;
}

NTSTATUS
OslSetBootStatusData (
    __in BOOLEAN LastBootSucceeded,
    __in BOOLEAN LastBootShutdown,
    __in BOOLEAN LastBootCheckpoint,
    __in OSL_BOOT_COUNT_UPDATE_TYPE RequestedBootCountUpdate,
    __in UCHAR BootAttemptCount
    )

/*++

Routine Description:

    Sets the boot status fields.

Arguments:

    LastBootShutdown - Supplies the boolean value of TRUE if the last boot
        shutdown ok, otherwise FALSE.

    LastBootSucceeded - Supplies the boolean value of TRUE if the last boot was
        successful, else FALSE.

    LastBootCheckpoint - Supplies the boolean value of TRUE if the last boot
        reached the checkpoint, else FALSE.

    RequestedBootCountUpdate - How we are intending to update the boot count.

    BootAttemptCount - Supplies the number of boot attempts that have been made
        since the last successful boot.

Return Value:

    STATUS_SUCCESS, or an appropriate error status.

--*/

{

    ULONG NewBootId;
    ULONG OriginalBootId;
    ULONG Size;
    NTSTATUS Status;

    //
    // Read the boot status data structure from the boot status file, caching it
    // for subsequent writes.
    //

    Status = OslpReadWriteBootStatusData(FALSE);
    if (!NT_SUCCESS(Status)) {
        goto SetBootStatusDataEnd;
    }

    Size = sizeof(BOOLEAN);
    Status = OslpSetBootStatusData(RtlBsdItemBootShutdown,
                                   &LastBootShutdown,
                                   &Size);

    if (!NT_SUCCESS(Status)) {
        goto SetBootStatusDataEnd;
    }

    Status = OslpSetBootStatusData(RtlBsdItemBootGood,
                                   &LastBootSucceeded,
                                   &Size);

    if (!NT_SUCCESS(Status)) {
        goto SetBootStatusDataEnd;
    }

    Status = OslpSetBootStatusData(RtlBsdItemBootCheckpoint,
                                   &LastBootCheckpoint,
                                   &Size);

    if (!NT_SUCCESS(Status)) {
        goto SetBootStatusDataEnd;
    }

    Size = sizeof(UCHAR);
    Status = OslpSetBootStatusData(RtlBsdItemBootAttemptCount,
                                   &BootAttemptCount,
                                   &Size);

    if (!NT_SUCCESS(Status)) {
        goto SetBootStatusDataEnd;
    }

    //
    // Get the current boot sequence.
    //

    Size = sizeof(ULONG);
    Status = OslpGetBootStatusData(RtlBsdItemBootId,
                                   &OriginalBootId,
                                   &Size);

    if (!NT_SUCCESS(Status)) {
        OriginalBootId = 0;
    }

    //
    // If we are attempting a new boot, increment the boot sequence. If we are 
    // aborting or canceling a boot attempt, decrement the previously incremented
    // boot sequence.
    //

    NewBootId = OriginalBootId;

    if (RequestedBootCountUpdate == OslBootCountUpdateIncrement) {
        NewBootId = OriginalBootId + 1;
    } else if (RequestedBootCountUpdate == OslBootCountUpdateDecrement) {
        ASSERT(OriginalBootId > 0);
        if (OriginalBootId > 0) {
            NewBootId = OriginalBootId - 1;
        }
    } else {
        //
        // No changes to boot sequence was requested, e.g. we might be 
        // resetting boot attempts before initiating a firmware update.
        //

        ASSERT(RequestedBootCountUpdate == OslBootCountUpdateNone);
        ASSERT(NewBootId == OriginalBootId);
    }

    //
    // Update the boot id if necessary, we will ignore the errors as this is
    // not critical.
    //

    if (NewBootId != OriginalBootId) {
        Size = sizeof(ULONG);
        Status = OslpSetBootStatusData(RtlBsdItemBootId,
                                       &NewBootId,
                                       &Size);
    }

    //
    // Write the boot status data structure to the boot status file, flushing
    // the cached copy.
    //

    Status = OslpReadWriteBootStatusData(TRUE);
    if (!NT_SUCCESS(Status)) {
        goto SetBootStatusDataEnd;
    }

SetBootStatusDataEnd:
    return Status;
}

NTSTATUS
OslGetLastPowerTransition (
    __out PBSD_POWER_TRANSITION PowerTransition
    )

/*++

Routine Description:

    This routine returns the power transition data from the boot status file.

Arguments:

    PowerTransition - Supplies a pointer to a variable that receives the
        information from the last power transition.

Return Value:

    NT status code.

--*/

{

    ULONG Size;
    NTSTATUS Status;

    //
    // Read the boot status data structure from the boot status file, caching it
    // for subsequent read.
    //

    Status = OslpReadWriteBootStatusData(FALSE);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Size = sizeof(BSD_POWER_TRANSITION);
    return OslpGetBootStatusData(RtlBsdPowerTransition,
                                 PowerTransition,
                                 &Size);
}

//
// ------------------------------------------------------------ Local Functions
//

NTSTATUS
OslpGetBootStatusData (
    __in RTL_BSD_ITEM_TYPE DataItem,
    __out PVOID Buffer,
    __inout PULONG Size
    )

/*++

Routine Description:

    Reads an item from the boot status data.

Arguments:

    DataItem - Describes the item to read.

    Buffer - Buffer to return the data item.

    Size - Size of the data buffer.

Return Value:

    STATUS_SUCCESS, or an appropriate error status.

--*/

{
    return OslpGetSetBootStatusData(TRUE, DataItem, Buffer, Size);
}

NTSTATUS
OslpGetSetBootStatusData (
    __in BOOLEAN Get,
    __in RTL_BSD_ITEM_TYPE DataItem,
    __inout PVOID Buffer,
    __inout PULONG Size
    )

/*++

Routine Description:

    Reads or writes a boot status data item of the log file.

Arguments:

    DataItem - Describes the data item to read or write.

    Buffer - Buffer for the data item to read or write.

    Size - Size of the data item buffer.

Return Value:

    STATUS_SUCCESS, or an appropriate error status.

--*/

{

    ULONG FieldOffset;
    ULONG ItemLength;
    NTSTATUS Status;

    //
    // Boot status data cannot complete a get or set request if the data was not
    // read from disk.
    //

    if (BsdBootStatusData == NULL) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // If the data item requsted isn't one we have code to handle then
    // return invalid parameter.
    //

    if ((DataItem < 0) || (DataItem >= RtlBsdItemMax)) {
        return STATUS_INVALID_PARAMETER;
    }

    FieldOffset = OslpBootStatusFields[DataItem].FieldOffset;
    ItemLength = OslpBootStatusFields[DataItem].FieldLength;

    //
    // If the data item offset is beyond the end of the structure then return a
    // versioning error.
    //

    if ((FieldOffset + ItemLength) > BsdBootStatusData->Version) {
        Status = STATUS_REVISION_MISMATCH;
        goto OslpBootStatusGetSetDataItemEnd;
    }

    //
    // Inform the caller that the buffer is too small.
    //

    if (*Size < ItemLength) {
        *Size = ItemLength;
        Status = STATUS_BUFFER_TOO_SMALL;
        goto OslpBootStatusGetSetDataItemEnd;
    }

    //
    // Read or write the data item.
    //

    if (Get == TRUE) {
        RtlCopyMemory(Buffer,
                      Add2Ptr(BsdBootStatusData, FieldOffset),
                      ItemLength);

    } else {
        RtlCopyMemory(Add2Ptr(BsdBootStatusData, FieldOffset),
                      Buffer,
                      ItemLength);
    }

    Status = STATUS_SUCCESS;

OslpBootStatusGetSetDataItemEnd:
    return Status;
}

NTSTATUS
OslpReadWriteBootStatusData (
    __in BOOLEAN Write
    )

/*++

Routine Description:

    This routine reads or writes the boot status data structure from or to the
    bootstat.dat data file.

Arguments:

    Write - Supplies a boolean indicating if a read or write is being
        requested.

Return Value:

    STATUS_SUCCESS, or an appropriate error status.

--*/

{

    PVOID BootStatusDataBuffer;
    PWCHAR BootStatusPath;
    UINTN BootStatusPathLength;
    ULONG BytesRead;
    ULONG BytesWritten;
    DEVICE_ID DeviceId;
    ULONG FileFlags;
    FILE_ID FileId;
    FILE_INFORMATION FileInformation;
    NTSTATUS Status;

    if ((Write == FALSE) && (BsdBootStatusData != NULL)) {
        return STATUS_SUCCESS;
    }

    if ((Write != FALSE) && (BsdBootStatusData == NULL)) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Allocate a buffer for the boot status data file path.
    //

    BootStatusPathLength = wcslen(OslSystemRoot) +
                           (sizeof(BOOTSTAT_FILE_NAME) / sizeof(WCHAR));

    BootStatusPath = BlMmAllocateHeap(BootStatusPathLength * sizeof(WCHAR));
    if (BootStatusPath == NULL) {
        return STATUS_NO_MEMORY;
    }

    swprintf_s(BootStatusPath,
               BootStatusPathLength,
               L"%s%s",
               OslSystemRoot,
               BOOTSTAT_FILE_NAME);

    //
    // Attempt to open the boot status file.
    //

    FileFlags = OPEN_READ;
    if (Write != FALSE) {
        FileFlags |= OPEN_WRITE;
    }

    Status = BlDeviceOpen(OslLoadDevice, FileFlags, &DeviceId);
    if (!NT_SUCCESS(Status)) {
        BlMmFreeHeap(BootStatusPath);
        return Status;
    }

    Status = BlFileOpen(DeviceId, BootStatusPath, FileFlags, &FileId);
    BlDeviceClose(DeviceId);
    BlMmFreeHeap(BootStatusPath);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Get the file size, checking to see if it is large enough to hold the
    // current boot status data structure.
    //

    Status = BlFileGetInformation(FileId, &FileInformation);
    if (!NT_SUCCESS(Status)) {
        goto ReadWriteBootStatusDataEnd;
    }

    if (FileInformation.FileSize < sizeof(BSD_BOOT_STATUS_DATA)) {
        Status = STATUS_REVISION_MISMATCH;
        goto ReadWriteBootStatusDataEnd;
    }

    //
    // On read requests, read and cache the boot status data structure out of
    // the data file. On write requests, flush the boot status data structure to
    // the data file.
    //

    if (Write == FALSE) {
        BootStatusDataBuffer = BlMmAllocateHeap(sizeof(BSD_BOOT_STATUS_DATA));
        if (BootStatusDataBuffer == NULL) {
            Status = STATUS_NO_MEMORY;
            goto ReadWriteBootStatusDataEnd;
        }

        Status = BlFileReadAtOffset(FileId,
                                    sizeof(BSD_BOOT_STATUS_DATA),
                                    0,
                                    BootStatusDataBuffer,
                                    &BytesRead);

        if (!NT_SUCCESS(Status)) {
            goto ReadWriteBootStatusDataEnd;
        }

        if (BytesRead != sizeof(BSD_BOOT_STATUS_DATA)) {
            Status = STATUS_UNEXPECTED_IO_ERROR;
            goto ReadWriteBootStatusDataEnd;
        }

        BsdBootStatusData = (PBSD_BOOT_STATUS_DATA)BootStatusDataBuffer;

    } else {
        Status = BlFileWriteAtOffset(FileId,
                                     sizeof(BSD_BOOT_STATUS_DATA),
                                     0,
                                     BsdBootStatusData,
                                     &BytesWritten);

        if (BytesWritten != sizeof(BSD_BOOT_STATUS_DATA)) {
            Status = STATUS_UNEXPECTED_IO_ERROR;
            goto ReadWriteBootStatusDataEnd;
        }

        BlMmFreeHeap(BsdBootStatusData);
        BsdBootStatusData = NULL;
    }

    Status = STATUS_SUCCESS;

ReadWriteBootStatusDataEnd:
    BlFileClose(FileId);
    return Status;
}

NTSTATUS
OslpSetBootStatusData (
    __in RTL_BSD_ITEM_TYPE DataItem,
    __in PVOID Buffer,
    __inout PULONG Size
    )

/*++

Routine Description:

    Writes an item to the boot status data.

Arguments:

    DataItem - Describes the item to write.

    Buffer - Buffer containing the data item.

    Size - Size of the data buffer.

Return Value:

    STATUS_SUCCESS, or an appropriate error status.

--*/

{

    return OslpGetSetBootStatusData(FALSE, DataItem, Buffer, Size);
}

