/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    bsdlog.c

Abstract:

    This module implements the boot status data logging functionality in the
    boot environment library.

Environment:

    Boot.

--*/

// ------------------------------------------------------------------- Includes

#include <bootlibp.h>

// -------------------------------------------------------------------- Defines

typedef struct _BSD_LOG_OBJECT {

    //
    // A DEVICE_ID for the device containing the open bootstat.dat file.
    //

    DEVICE_ID LogDevice;

    //
    // A FILE_ID for the open bootstat.dat file.
    //

    FILE_ID LogFile;

    //
    // The application type as defined in bcdtypes.h.  They are defined to be
    // of the form BCD_APPLICATION_TYPE_...
    //

    ULONG ApplicationType;

    //
    // The offset from the start of the file to the beginning of the log data
    // section, and the length in bytes of the log data section.
    //

    ULONG LogStart;
    ULONG LogSize;

} BSD_LOG_OBJECT, *PBSD_LOG_OBJECT;

// -------------------------------------------------------------------- Globals

BSD_LOG_OBJECT BsdpLogObject = {0};
BOOLEAN BsdpLogObjectInitialized = FALSE;

// ----------------------------------------------------------------- Prototypes

NTSTATUS
BsdpGetFileSize (
    __out PULONGLONG FileSize
    );

NTSTATUS
BsdpGetLogControl (
    __out PBSD_BOOT_LOG_CONTROL Control
    );

NTSTATUS
BsdpSetLogControl (
    __in PBSD_BOOT_LOG_CONTROL Control
    );

NTSTATUS
BsdpWriteAtLogOffset (
    __in ULONG LogOffset,
    __in_bcount(DataSize) PVOID Data,
    __in ULONG DataSize
    );

NTSTATUS
BsdpReadAtLogOffset (
    __in ULONG LogOffset,
    __out_bcount(DataSize) PVOID Data,
    __in ULONG DataSize
    );

// ------------------------------------------------------------------ Functions

VOID
BlBsdInitializeLog (
    __in ULONG ApplicationType,
    __in PBOOT_ENVIRONMENT_DEVICE LogDevice,
    __in PCWSTR LogPath,
    __in ULONG LogStart,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine will intialize the logging structure for the currently running
    boot application.  This function must be called before any other BlBsd...
    functions can be called.  Any return value would be ignored anyway, so there
    isn't one.

Arguments:

    Application - Provides the application type.  The different application
        types can be one of BCD_APPLICATION_TYPE_... from bcdtypes.h.

    LogDevice - Provides the device on which the file containing the log to be
        initialized resides.

    LogPath - Provides the path of the file containing the log to be
        initialized.

    LogStart - Provides the offset in bytes from the beginning of the specified
        file to the start of the log data.

    Flags - This specifies optional initialization flags.  Valid flags can
        currently only be 0 or BSD_INIT_FLAGS_PRESERVE_LOG.

Return Value:

    None.

--*/

{

    ULONGLONG FileSize;
    BSD_EVENT_LOGDATA_INITIALIZED InitializeEvent;
    BSD_BOOT_LOG_CONTROL LogControl;
    NTSTATUS Status;
    BOOLEAN Valid;

    //
    // If the log is already initialized, then don't initialize it again.  Just
    // return here, otherwise the FILE_ID and DEVICE_ID in the structure will
    // be closed.
    //

    if (BsdpLogObjectInitialized != FALSE) {
        return;
    }

    BsdpLogObject.LogDevice = INVALID_DEVICE_ID;
    BsdpLogObject.LogFile = INVALID_FILE_ID;

    //
    // Open the device containing the file containing the log.
    //

    Status = BlDeviceOpen(LogDevice,
                          OPEN_READ | OPEN_WRITE,
                          &BsdpLogObject.LogDevice);

    if (!NT_SUCCESS(Status)) {
        goto InitializeLogEnd;
    }

    //
    // Open the file containing the log.
    //

    Status = BlFileOpen(BsdpLogObject.LogDevice,
                        (PWSTR)LogPath,
                        OPEN_READ | OPEN_WRITE,
                        &BsdpLogObject.LogFile);

    if (!NT_SUCCESS(Status)) {
        goto InitializeLogEnd;
    }

    Status = BsdpGetFileSize(&FileSize);
    if (!NT_SUCCESS(Status)) {
        goto InitializeLogEnd;
    }

    //
    // Ensure that the log size is within the valid range.  Note that the log
    // size doesn't include the portion of the file before the LogOffset
    // position.  This is being reserved by the caller for other purposes.
    //

    if ((FileSize < (LogStart + BSD_BOOT_LOG_MINIMUM_SIZE)) ||
        (FileSize > (LogStart + BSD_BOOT_LOG_MAXIMUM_SIZE))) {

        Status = STATUS_INVALID_PARAMETER;
        goto InitializeLogEnd;
    }

    //
    // Initialize the rest of the log object.
    //

    BsdpLogObject.ApplicationType = ApplicationType;
    BsdpLogObject.LogStart = LogStart;
    BsdpLogObject.LogSize = (ULONG)(FileSize - LogStart);

    //
    // If the user wanted the log preserved, then do some sanity checks on the
    // file.  If the sanity checks fail, then set the flag indicating that the
    // file is not valid.
    //

    Valid = TRUE;
    if (CHECK_FLAG(Flags, BSD_INIT_FLAGS_PRESERVE_LOG)) {
        Status = BsdpGetLogControl(&LogControl);
        if ((!NT_SUCCESS(Status)) ||
            (LogControl.Version != BSD_BOOT_LOG_VERSION) ||
            (LogControl.BootLogStart < sizeof(BSD_BOOT_LOG_CONTROL)) ||
            (LogControl.NextBootLogEntry < LogControl.BootLogStart) ||
            (LogControl.NextBootLogEntry > BsdpLogObject.LogSize)) {

            Valid = FALSE;
        }
    }

    //
    // If the caller wanted the log cleared, then do so now.  Also do this if
    // the log file was determined to be invalid for some reason.
    //

    if ((!CHECK_FLAG(Flags, BSD_INIT_FLAGS_PRESERVE_LOG)) ||
        (Valid == FALSE)) {

        RtlZeroMemory(&LogControl, sizeof(BSD_BOOT_LOG_CONTROL));
        LogControl.Version = BSD_BOOT_LOG_VERSION;
        LogControl.BootLogStart = sizeof(BSD_BOOT_LOG_CONTROL);
        LogControl.BootLogSize = BsdpLogObject.LogSize;
        LogControl.NextBootLogEntry = sizeof(BSD_BOOT_LOG_CONTROL);
        Status = BsdpSetLogControl(&LogControl);
        if (!NT_SUCCESS(Status)) {
            goto InitializeLogEnd;
        }
    }

    //
    // Log the initialization event.
    //

    Status = BlGetTime(&InitializeEvent.SystemTime, NULL);
    if (!NT_SUCCESS(Status)) {
        goto InitializeLogEnd;
    }

    InitializeEvent.TimeStampMultiplier = 1;

    //
    // Show the log as initialized. This will allow the initialization log entry
    // to be written.
    //

    BsdpLogObjectInitialized = TRUE;

    //
    // Now write the initialization log entry.
    //

    BlBsdLogEntry(BSD_EVENT_LEVEL_INFORMATION,
                  BSD_EVENT_INITIALIZED,
                  &InitializeEvent,
                  sizeof(BSD_EVENT_LOGDATA_INITIALIZED));

InitializeLogEnd:
    if (!NT_SUCCESS(Status)) {
        if (BsdpLogObject.LogFile != INVALID_FILE_ID) {
            BlFileClose(BsdpLogObject.LogFile);
        }

        if (BsdpLogObject.LogDevice != INVALID_DEVICE_ID) {
            BlDeviceClose(BsdpLogObject.LogDevice);
        }

        RtlZeroMemory(&BsdpLogObject, sizeof(BSD_LOG_OBJECT));
        BsdpLogObjectInitialized = FALSE;
    }

    return;
}

VOID
BlBsdLogEntry (
    __in ULONG Level,
    __in ULONG EventCode,
    __in_bcount_opt(EventDataSize) PVOID EventData,
    __in ULONG EventDataSize
    )

/*++

Routine Description:

    This routine will add an entry to the Boot Status Data log.  This routine
    should only be called subsequent to a successful call to BlBsdInitializeLog.

Arguments:

    Level - Supplies the level of the event being logged. This can be one of the
        following values:

        BSD_EVENT_LEVEL_SUCCESS (0)
        BSD_EVENT_LEVEL_INFORMATION (1)
        BSD_EVENT_LEVEL_WARNING (2)
        BSD_EVENT_LEVEL_ERROR (3)

    ApplicationType - Provides the source of the event.  The application code
        and boot environment subsystem are encoded here.

    EventCode - This uniquely identifies the reason code for the event being
        logged.

    EventData - This provides the data payload to be logged for the event. The
        structure and size of the payload is determined by the event code.

    EventDataSize - The size of the data payload.

Return Value:

    A NTSTATUS value indicating the success or failure of the operation.

--*/

{

    GUID* ApplicationIdentifier;
    BSD_BOOT_LOG_EVENT_HEADER EventHeader;
    BSD_BOOT_LOG_CONTROL LogControl;
    ULONG LogOffset;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;

    //
    // Ensure that the log is initialized before proceeding any further.
    //

    if (BsdpLogObjectInitialized == FALSE) {
        return;
    }

    RtlZeroMemory(&EventHeader, sizeof(BSD_BOOT_LOG_EVENT_HEADER));

    //
    // TODO: Work out some time stamp functionality.
    //

    ApplicationIdentifier = BlGetApplicationIdentifier();

    EventHeader.TimeStamp = BlTimeGetRelativeTime();
    EventHeader.EntrySize = sizeof(BSD_BOOT_LOG_EVENT_HEADER) + EventDataSize;
    if (ApplicationIdentifier != NULL) {
        RtlCopyMemory(&EventHeader.ApplicationId,
                      ApplicationIdentifier,
                      sizeof(GUID));
    }

    EventHeader.Level = Level;
    EventHeader.ApplicationType = BsdpLogObject.ApplicationType;
    EventHeader.EventCode = EventCode;

    //
    // Obtain the log control structure.  This will give us the file location
    // at which the next log entry is to be written.
    //

    Status = BsdpGetLogControl(&LogControl);
    if (!NT_SUCCESS(Status)) {
        goto LogEntryEnd;
    }

    //
    // Move the file pointer to the location where the next boot log entry
    // will be written.
    //

    LogOffset = LogControl.NextBootLogEntry;
    if (!NT_SUCCESS(Status)) {
        goto LogEntryEnd;
    }

    //
    // Write the event header to the log.
    //

    Status = BsdpWriteAtLogOffset(LogOffset,
                                  &EventHeader,
                                  sizeof(BSD_BOOT_LOG_EVENT_HEADER));

    if (!NT_SUCCESS(Status)) {
        goto LogEntryEnd;
    }

    LogOffset += sizeof(BSD_BOOT_LOG_EVENT_HEADER);

    //
    // Write the event data payload to the log, but only if there is an event
    // data payload to write.
    //

    if (EventData != NULL) {
        Status = BsdpWriteAtLogOffset(LogOffset,
                                      EventData,
                                      EventDataSize);

        if (!NT_SUCCESS(Status)) {
            goto LogEntryEnd;
        }

        LogOffset += EventDataSize;
    }

    //
    // Obtain the current location of the file pointer and store it in the log
    // control structure.  This is the location where the next log entry must
    // be written.
    //

    LogControl.NextBootLogEntry = LogOffset;

    //
    // Save the log control structure to disk.
    //

    Status = BsdpSetLogControl(&LogControl);
    if (!NT_SUCCESS(Status)) {
        goto LogEntryEnd;
    }

LogEntryEnd:
    return;
}

VOID
BlBsdCloseLog (
    VOID
    )

/*++

Routine Description:

    This routine will close a boot log, and free any resources associated with
    the boot log object.

Arguments:

    None.

Return Value:

    A NTSTATUS value indicating the success or failure of the operation.

--*/

{

    if (BsdpLogObjectInitialized == FALSE) {
        return;
    }

    if (BsdpLogObject.LogFile != INVALID_FILE_ID) {
        BlFileClose(BsdpLogObject.LogFile);
    }

    if (BsdpLogObject.LogDevice != INVALID_DEVICE_ID) {
        BlDeviceClose(BsdpLogObject.LogDevice);
    }

    RtlZeroMemory(&BsdpLogObject, sizeof(BSD_LOG_OBJECT));
    BsdpLogObjectInitialized = FALSE;
    return;
}

NTSTATUS
BsdpGetFileSize (
    __out PULONGLONG FileSize
    )

/*++

Routine Description:

    This routine will obtain the size of the specified open file and return it
    to the caller.

Arguments:

    FileSize - Supplies a buffer in which the size will be returned.

Return Value:

    A NTSTATUS value indicating the success or failure of the operation.

--*/

{

    FILE_INFORMATION FileInformation;
    NTSTATUS Status;

    //
    // Obtain information about the file.
    //

    Status = BlFileGetInformation(BsdpLogObject.LogFile, &FileInformation);
    if (!NT_SUCCESS(Status)) {
        goto GetFileSizeEnd;
    }

    *FileSize = FileInformation.FileSize;

GetFileSizeEnd:
    return Status;

}

NTSTATUS
BsdpGetLogControl (
    __out PBSD_BOOT_LOG_CONTROL Control
    )

/*++

Routine Description:

    This routine will retrieve the log control structure from the log file and
    return it to the caller.

Arguments:

    Control - Supplies a buffer in which the log control structure will be
        returned to the caller.

Return Value:

    A NTSTATUS value indicating the success or failure of the operation.

--*/

{

    NTSTATUS Status;

    //
    // Write the control block to the file.
    //

    Status = BsdpReadAtLogOffset(0, Control, sizeof(BSD_BOOT_LOG_CONTROL));
    return Status;
}

NTSTATUS
BsdpSetLogControl (
    __in PBSD_BOOT_LOG_CONTROL Control
    )

/*++

Routine Description:

    This routine will set the log control structure in the log file.

Arguments:

    Control - Supplies the log control structure to be set in the log file.

Return Value:

    A NTSTATUS value indicating the success or failure of the operation.

--*/

{

    NTSTATUS Status;

    //
    // Write the control block to the file.
    //

    Status = BsdpWriteAtLogOffset(0, Control, sizeof(BSD_BOOT_LOG_CONTROL));

    return Status;
}

NTSTATUS
BsdpWriteAtLogOffset (
    __in ULONG LogOffset,
    __in_bcount(DataSize) PVOID Data,
    __in ULONG DataSize
    )

/*++

Routine Description:

    This routine will write the specified data block to the specified location
    in the log section of the BCD file.  The offset is relative to the start of
    the log section.

Arguments:

    LogOffset - Supplies the offset relative to the start of the log section of
        the BSD file.

    Data - Supplies the data to be written to the file.

    DataSize - Supplies the size in bytes of the data block to be written.

Return Value:

    A NTSTATUS value indicating the success or failure of the operation.

--*/

{

    ULONG BytesWritten;
    NTSTATUS Status;

    BytesWritten = 0;
    Status = BlFileWriteAtOffset(BsdpLogObject.LogFile,
                                 DataSize,
                                 BsdpLogObject.LogStart + LogOffset,
                                 Data,
                                 &BytesWritten);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    if (BytesWritten != DataSize) {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
BsdpReadAtLogOffset (
    __in ULONG LogOffset,
    __out_bcount(DataSize) PVOID Data,
    __in ULONG DataSize
    )

/*++

Routine Description:

    This routine will read the specified data block from the specified location
    in the log section of the BCD file.  The offset is relative to the start of
    the log section.

Arguments:

    LogOffset - This provides the offset relative to the start of the log
        section of the BSD file.

    Data - This provides the data buffer that the data will be read into.

    DataSize - This provides the size in bytes of the data block to be read.

Return Value:

    A NTSTATUS value indicating the success or failure of the operation.

--*/

{

    ULONG BytesRead;
    NTSTATUS Status;

    Status = BlFileReadAtOffset(BsdpLogObject.LogFile,
                                DataSize,
                                BsdpLogObject.LogStart + LogOffset,
                                Data,
                                &BytesRead);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    if (BytesRead != DataSize) {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}


