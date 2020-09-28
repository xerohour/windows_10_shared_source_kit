/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    log.c

Abstract:

    This module implements the logging routines. This logger can log data once
    the memory is initialized, followed by which the logger has been
    initialized. All the logging should happen through the BlLogData function as
    it checks for adequate space in the log, adjusts the current offset and
    updates the header.

    At the start of the logging session, a session start indicator is logged. At
    the end of the logging session the memore used for logging is persisted, so
    that it can be retrieved later.

    A failure from any of the routines should be ignored and should not affect
    the boot or resume process.

    For the boot scenario, once the memory map has been built no more heap
    memory should be used. For this purpose, once the end of the log has been
    set, no more data is logged.

--*/

// ------------------------------------------------------------------- Includes

#include <bootlibp.h>
#include "systemevents.h"

// -------------------------------------------------------------------- Defines

#define BL_LOG_MEMORY \
    GET_MEMORY_TYPE(MEMORY_CLASS_LIBRARY, MEMORY_LIBRARY_TYPE_MISC_DATA)

//
// Number of Log Pages to be used for each application.
//

#define BL_LOG_NUMBER_OF_PAGES  128

//
// Return value is the Pointer increased to be aligned with Alignment. Alignment
// must be a power of 2.
//

#define BL_ALIGN_UP(Pointer, Alignment)    \
  ((((ULONG_PTR)(Pointer) + (Alignment) - 1) & (~((ULONG_PTR)(Alignment) - 1))))

// -------------------------------------------------------------------- Globals

typedef struct _BL_LOG_BUFFER {

    //
    // Buffer where data will be stored.
    //

    __field_ecount_opt(BufferSize) PUCHAR BufferHead;

    //
    // Offset until which data has been written in the buffer.
    //

    ULONG CurrentOffset;

    //
    // Size of the allocated buffer.
    //

    ULONG BufferSize;

    //
    // This is to be set once the end mark has been logged. Indicates that no
    // more processing should be done.
    //

    BOOLEAN StopLogging;

} BL_LOG_BUFFER, *PBL_LOG_BUFFER;

//
// Global Buffer to keep our log data
//

BL_LOG_BUFFER BlLogBuffer = {0};

// ----------------------------------------------- Internal Function Prototypes

VOID
LogCleanup (
    VOID
    );

BOOLEAN
LogIsEnabled (
    VOID
    );

NTSTATUS
LogSessionStart (
    VOID
    );

// ------------------------------------------------------------------ Functions

NTSTATUS
BlpLogInitialize (
    VOID
    )

/*++

Routine Description:

    This routine allocates memory for logging and initializes the BlLogBuffer.
    It also initialize any other logging components.

    This function should be called after Memory Intialization (BlMmInitialize).
    This is because all the memory descriptors and all data structures for
    allocation are set up there.

    The return from the function should be ignored as this logging is not
    critical for system functionality.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS if logging started successfully.
    An error status otherwise.

--*/

{
    NTSTATUS Status;

    //
    // Initialize variables such that cleanup is easy in case of failure in this
    // routine.
    //

    BlLogBuffer.BufferSize = (BL_LOG_NUMBER_OF_PAGES * PAGE_SIZE);
    BlLogBuffer.CurrentOffset = 0;
    BlLogBuffer.StopLogging = TRUE;
    BlLogBuffer.BufferHead = NULL;

    //
    //
    // Allocate a buffer block where all data would be logged. No preferred
    // address, so set the BufferHead to null.
    //

    Status = BlMmAllocatePages(&BlLogBuffer.BufferHead,
                               BL_LOG_NUMBER_OF_PAGES,
                               BL_LOG_MEMORY,
                               MEMORY_ATTRIBUTE_NONE,
                               0);

    if (!NT_SUCCESS(Status)) {
        goto cleanup;
    }

    //
    // Initialize the logging info for the device log.
    //

    Status = BlpDeviceLogInitialize();
    if (!NT_SUCCESS(Status)) {
        goto cleanup;
    }

    //
    // Initialize the logging info for the file log.
    //

    Status = BlpFileLogInitialize();
    if (!NT_SUCCESS(Status)) {
        goto cleanup;
    }

    //
    // If initialization went fine, go ahead and start logging.
    //

    BlLogBuffer.StopLogging = FALSE;

    //
    // Log the App Start event.
    //

    Status = LogSessionStart();
    if (!NT_SUCCESS(Status)) {
        goto cleanup;
    }

    Status = STATUS_SUCCESS;

cleanup:
    if (!NT_SUCCESS(Status)) {
        LogCleanup();
    }

    return Status;

}

NTSTATUS
BlLogDestroy (
    VOID
    )

/*++

Routine Description:

    This routine is called to persist any log information and do the necessary
    cleanup.

    The return from the function should be ignored as this logging is not
    critical for system functionality.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS if operation completed successfully.

--*/

{

    GENERIC_BUFFER LogBuffer;
    GUID LogGuid = BL_LOG_GUID;
    PBL_LOG_SESSION_START SessionHeader;
    NTSTATUS Status;

    //
    // Only destroy the log if it exists.
    //

    if (BlLogBuffer.BufferHead == NULL) {
        Status = STATUS_SUCCESS;
        goto BlLogDestroyEnd;
    }

    //
    // Set the amount of data that was logged for this session
    //

    SessionHeader = (PBL_LOG_SESSION_START)(&(BlLogBuffer.BufferHead[0]));
    SessionHeader = (PBL_LOG_SESSION_START) BL_ALIGN_UP(SessionHeader,
                                                        BL_LOG_ALIGNMENT);
    SessionHeader->DataSize = (ULONG)(BlLogBuffer.CurrentOffset);

    //
    // Set the generic buffer to the start of our log.
    //

    BlInitGenericBuffer(&LogBuffer,
                        BlLogBuffer.BufferHead,
                        BlLogBuffer.CurrentOffset);

    Status = BlPdSaveData(NULL, &LogGuid, &LogBuffer);
    if (!NT_SUCCESS(Status)) {
        BlStatusPrint(L"Library call to persist log results failed 0x%08x\n",
                      Status);
    }

    Status = STATUS_SUCCESS;

BlLogDestroyEnd:
    LogCleanup();
    return Status;
}

NTSTATUS
BlLogData (
    __in PVOID Data
    )

/*++

Routine Description:

    This functions Logs any Data that is provided to it and updates the buffer
    header. Any data sent to be logged must have a BL_LOG_ENTRY_HEADER at the
    start of the log entry.

Arguments:

    Data - Supplies a pointer to the data that needs to be logged to the boot
        log buffer.

Return Value:

    STATUS_SUCCESS if the data was logged successfully.
    STATUS_LOG_BLOCK_INVALID if logging is not enabled right now.
    STATUS_BUFFER_TOO_SMALL if the logging buffer has run out of space.

--*/

{

    PUCHAR CurrentBufferPos;
    PBL_LOG_ENTRY_HEADER DataHeader;
    ULONG NewOffset;
    NTSTATUS Status;

    //
    // Assert that parameters are correct.
    //

    DataHeader = (PBL_LOG_ENTRY_HEADER)Data;

    ASSERT(DataHeader);
    ASSERT(DataHeader->DataSize >= sizeof(BL_LOG_ENTRY_HEADER));

    //
    // Check if logging is enabled.
    //

    if (LogIsEnabled() == FALSE) {
        Status = STATUS_LOG_BLOCK_INVALID;
        goto cleanup;
    }

    //
    // Get the pointer to the current buffer position.
    //

    CurrentBufferPos = &(BlLogBuffer.BufferHead[BlLogBuffer.CurrentOffset]);
    CurrentBufferPos = (PUCHAR) BL_ALIGN_UP(CurrentBufferPos,
                                            BL_LOG_ALIGNMENT);

    //
    // Check if there is enough space to log the data
    //

    NewOffset = (ULONG)(CurrentBufferPos - BlLogBuffer.BufferHead) +
                DataHeader->DataSize;

    if (NewOffset > BlLogBuffer.BufferSize) {

        //
        // The log is full. Turn off logging.
        //

        BlLogBuffer.StopLogging = TRUE;

        Status = STATUS_BUFFER_TOO_SMALL;
        goto cleanup;
    }

    //
    // If there is sufficient space in our buffer then log the data.
    //

    RtlCopyMemory(CurrentBufferPos, Data, DataHeader->DataSize);

    //
    // Set the current offset to the NewOffset and update the header.
    //

    BlLogBuffer.CurrentOffset = NewOffset;
    Status = STATUS_SUCCESS;

cleanup:

    return Status;
}

NTSTATUS
BlLogEtwTotalIo (
    VOID
    )

/*++

Routine Description:

    This routine logs an ETW event that captures the total IO performed by
    all physical devices.

Arguments:

    None

Return Value:

    STATUS_SUCCESS if the operation completed successfully.

--*/

{

    GUID *ApplicationGuid;
    DEVICE_IO_INFORMATION DeviceIoInformation;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[3];
    GUID NullGuid = { 0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0 } };
    NTSTATUS Status;

    Status = BlDeviceGetIoInformation(&DeviceIoInformation);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Some boot applications don't have an application identifier, so just use
    // the NULL GUID.
    //

    ApplicationGuid = BlGetApplicationIdentifier();
    if (ApplicationGuid == NULL) {
        ApplicationGuid = &NullGuid;
    }

    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)ApplicationGuid,
                        sizeof(GUID));

    EventDataDescCreate(&EventDataDescriptor[1],
                        (PVOID)&(DeviceIoInformation.TotalBytesRead),
                        sizeof(ULONGLONG));

    EventDataDescCreate(&EventDataDescriptor[2],
                        (PVOID)&(DeviceIoInformation.TotalBytesWritten),
                        sizeof(ULONGLONG));

    Status = BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                           &BOOT_TOTAL_IO,
                           NULL,
                           3,
                           EventDataDescriptor);

    return Status;
}

NTSTATUS
BlLogEtwWrite (
    __in LPCGUID ProviderId,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in_opt LPCGUID ActivityId,
    __in ULONG UserDataCount,
    __in_opt PEVENT_DATA_DESCRIPTOR UserData
    )

/*++

Routine Description:

    This routine writes an ETW event to the log to be consumed by the kernel
    at a later time.

Arguments:

    ProviderId - Supplies a pointer to an event provider GUID.

    EventDescriptor - Supplies a pointer to the structure describing the ETW
        event.

    ActivityId - Supplies the identifier associated with the event.

    UserDataCount - Supplies the the number of data descriptors.

    UserData - Supplies a pointer to an array of data descriptors.

Return Value:

    STATUS_SUCCESS if operation completed successfully.

--*/

{

    PBL_LOG_ETW_EVENT EtwEvent;
    ULONG EtwEventSize;
    ULONG Index;
    PVOID NextData;
    NTSTATUS Status;
    ULONG TotalDataSize;

    Status = STATUS_SUCCESS;

    //
    // Calculate the total data size associated with this ETW event.
    //

    TotalDataSize = 0;
    for (Index = 0; Index < UserDataCount; Index += 1) {
        TotalDataSize += UserData[Index].Size;
    }

    //
    // Allocate an ETW log event structure large enough to hold all the data.
    //

    EtwEventSize = FIELD_OFFSET(BL_LOG_ETW_EVENT, Data) + TotalDataSize;
    EtwEvent = (PBL_LOG_ETW_EVENT)BlMmAllocateHeap(EtwEventSize);
    if (EtwEvent == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto EtwWriteEnd;
    }

    RtlZeroMemory(EtwEvent, EtwEventSize);
    EtwEvent->Header.LogType = BlLogTypeEtwEvent;
    EtwEvent->Header.Timestamp = BlTimeGetRelativeTime();
    EtwEvent->Header.DataSize = EtwEventSize;
    EtwEvent->ProviderId = *ProviderId;
    EtwEvent->EventDescriptor = *EventDescriptor;
    if (ARGUMENT_PRESENT(ActivityId) != FALSE) {
        EtwEvent->ActivityIdValid = TRUE;
        EtwEvent->ActivityId = *ActivityId;
    }

    //
    // Copy all the data into the allocated buffer.
    //

    EtwEvent->DataSize = TotalDataSize;
    NextData = &EtwEvent->Data;
    for (Index = 0; Index < UserDataCount; Index += 1) {
        RtlCopyMemory(NextData,
                     *(PVOID*)&UserData[Index].Ptr,
                     UserData[Index].Size);

        NextData = Add2Ptr(NextData, UserData[Index].Size);
    }

    //
    // Finally, write the event to the log.
    //

    Status = BlLogData((PVOID)EtwEvent);

EtwWriteEnd:
    if (!NT_SUCCESS(Status) && (EtwEvent != NULL)) {
        BlMmFreeHeap(EtwEvent);
    }

    return Status;
}

VOID
BlpLogResume (
    VOID
    )

/*++

Routine Description:

    This routine resumes all logging operations.

Arguments:

    None.

Return Value:

    None.

--*/

{

    BlLogBuffer.StopLogging = FALSE;
    return;
}

VOID
BlpLogSuspend (
    VOID
    )

/*++

Routine Description:

    This routine suspends all logging operations.

Arguments:

    None.

Return Value:

    None.

--*/

{

    //
    // Log desctruction does not depend on the value of
    // BlLogBuffer.StopLogging, so it is safe to set it here.
    //

    BlLogBuffer.StopLogging = TRUE;
    return;
}

//
// ----------------------------------------------------------- Private Routines
//

VOID
LogCleanup (
    VOID
    )

/*++

Routine Description:

    Frees any memory used by the logger.

Arguments:

    None.

Return Value:

    None.

--*/

{

    //
    // Destroy the support in other components.
    //

    BlpFileLogDestroy();
    BlpDeviceLogDestroy();

    //
    // Free the buffer allocated for logging
    //

    if (BlLogBuffer.BufferHead) {
        BlMmFreePages(BlLogBuffer.BufferHead);
        BlLogBuffer.BufferHead = 0;
        BlLogBuffer.CurrentOffset = 0;
        BlLogBuffer.BufferSize = 0;
        BlLogBuffer.StopLogging = TRUE;
    }

    return;
}

NTSTATUS
LogSessionStart (
    VOID
    )

/*++

Routine Description:

    Log the Session start.

Arguments:

    None.

Return Value:

    Status as returned by BlLogData.

--*/

{

    BL_LOG_SESSION_START SessionStart;
    NTSTATUS Status;

    //
    // Set the application start data and log it to the buffer.
    //

    SessionStart.DataSize = BlLogBuffer.CurrentOffset;
    SessionStart.Header.LogType = BlLogTypeSessionStart;
    SessionStart.Header.Timestamp = BlTimeGetRelativeTime();
    SessionStart.Header.DataSize = sizeof(SessionStart);
    Status = BlLogData(&SessionStart);
    if (!NT_SUCCESS(Status)) {
        goto cleanup;
    }

    Status = STATUS_SUCCESS;

cleanup:
    return Status;
}

BOOLEAN
LogIsEnabled (
    VOID
    )

/*++

Routine Description:

    This routine checks if boot logging is enabled or not.

Arguments:

    None.

Return Value:

    True if enabled else false

--*/

{

    if ((BlLogBuffer.BufferHead == NULL) ||
        (BlLogBuffer.StopLogging != FALSE)) {

        return FALSE;
    }

    return TRUE;
}

