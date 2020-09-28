/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    whea.c

Abstract:

    This module provides WHEA support for the PCI Express error type. The
    functions perform initialization and configuration, as well as the
    construction of the WHEA error packet in the event of an error.









--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include <initguid.h>
#include <cperguid.h>

// --------------------------------------------------------------------- Defines

// ----------------------------------------------------------------------- Types

// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciWheaCorrectErrorSource (
    __inout PWHEA_ERROR_SOURCE_DESCRIPTOR ErrorSource,
    __out PULONG MaximumSectionLength
    );

NTSTATUS
PciWheaInitErrorSource (
    __in ULONG Phase,
    __inout PWHEA_ERROR_SOURCE_DESCRIPTOR ErrorSource,
    __inout_opt PVOID Context
    );

NTSTATUS
PciWheaCreateErrorRecord (
    __inout PWHEA_ERROR_SOURCE_DESCRIPTOR Source,
    __inout PWHEA_ERROR_PACKET Packet,
    __out_bcount(BufferSize) PWHEA_ERROR_RECORD ErrorRecord,
    __in ULONG BufferSize,
    __inout_opt PVOID Context
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(INIT, PciWheaInit)
    #pragma alloc_text(PAGE, PciWheaInitErrorSource)
#endif

// --------------------------------------------------------------------- Globals

WHEA_ERROR_SOURCE_CONFIGURATION PcipErrorSourceConfiguration = {
    0,                              // Flags
    PciWheaCorrectErrorSource,      // Correct
    PciWheaInitErrorSource,         // Initialize
    PciWheaCreateErrorRecord,       // CreateRecord
    NULL                            // Recover
};

// ------------------------------------------------------------------- Functions

VOID
PciWheaInit (
    VOID
    )

/*++

Routine Description:

    This routine configures the PCI Express error type callback routines.

Arguments:

    None.

Return Value:

    None.

--*/

{

    WheaConfigureErrorSource(WheaErrSrcTypePCIe, &PcipErrorSourceConfiguration);
}

NTSTATUS
PciWheaCorrectErrorSource (
    __inout PWHEA_ERROR_SOURCE_DESCRIPTOR ErrorSource,
    __out PULONG MaximumSectionLength
    )

/*++

Routine Description:

    This routine allows the Express error source to adjust an error source and
    return the maximum section length of its error records.

Arguments:

    ErrorSource - Supplies a pointer to the error source.

    MaximumSectionLength - Supplies a pointer to a location in which the maximum
        length of any section in the error record is returned.

Return Value:

    STATUS_SUCCESS.

--*/

{

    if (ErrorSource->MaxSectionsPerRecord < 3) {
        ErrorSource->MaxSectionsPerRecord = 3;
    }

    *MaximumSectionLength = max(sizeof(EXPRESS_ERROR_PACKET),
                                sizeof(WHEA_PROCESSOR_GENERIC_ERROR_SECTION));

    *MaximumSectionLength += ErrorSource->MaxRawDataLength;
    return STATUS_SUCCESS;
}

NTSTATUS
PciWheaInitErrorSource (
    __in ULONG Phase,
    __inout PWHEA_ERROR_SOURCE_DESCRIPTOR ErrorSource,
    __inout_opt PVOID Context
    )

/*++

Routine Description:

    This routine is called by WHEA to initialize each root port that supports
    AER.

Arguments:

    Phase - Supplies a value that identifies the initialization phase.

    ErrorSource - supplies a pointer to the error source descriptor.

    Context - Supplies a pointer to the EXPRESS_BRIDGE structure for the root
        port.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PEXPRESS_BRIDGE ExpressBridge;

    PAGED_CODE();

    if (Phase == 1) {
        TRACE(AER_TRACE_LEVEL, "ExpressInitErrorSource: ErrorSourceId %x\n",
              ErrorSource->ErrorSourceId);

        if (Context != NULL) {
            ExpressBridge = Context;
            ExpressBridge->WheaInitialized = 1;
            ExpressBridge->ErrorSource.ErrorSourceId =
                ErrorSource->ErrorSourceId;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciWheaCreateErrorRecord (
    __inout PWHEA_ERROR_SOURCE_DESCRIPTOR Source,
    __inout PWHEA_ERROR_PACKET ErrorPacket,
    __out_bcount(BufferSize) PWHEA_ERROR_RECORD ErrorRecord,
    __in ULONG BufferSize,
    __inout_opt PVOID Context
    )

/*++

Routine Description:

    This routine is the handler that will be called by the windows hardware
    error system to populate the error record.

Arguments:

    ErrorSource - Provides the descriptor for the error source.

    ErrorPacket - Provides a pointer to the error packet.

    ErrorRecord - Provides a pointer to the error record buffer.

    Context - Provides a pointer to the PCI Express driver provided context.

Return Value:

    NT status code.

--*/

{

    PWHEA_ERROR_RECORD_SECTION_DESCRIPTOR Descriptor;
    ULONG Offset;
    PEXPRESS_ERROR_PACKET Packet;
    PWHEA_PROCESSOR_GENERIC_ERROR_SECTION Processor;
    ULONG RequiredSize;

    UNREFERENCED_PARAMETER(Context);

    Packet = (PEXPRESS_ERROR_PACKET)ErrorPacket;

    //
    // This routine will populate an error record with three error record
    // sections (PCI express, processor generic and error packet).  Ensure
    // the provided buffer is large enough to initialize the error record
    // with all three sections.
    //

    if (Source->MaxSectionsPerRecord < 3) {
        return STATUS_INVALID_PARAMETER;
    }

    RequiredSize = sizeof(WHEA_ERROR_RECORD_HEADER);
    RequiredSize +=
        sizeof(WHEA_ERROR_RECORD_SECTION_DESCRIPTOR) *
        Source->MaxSectionsPerRecord;

    RequiredSize += sizeof(WHEA_PCIEXPRESS_ERROR_SECTION);
    RequiredSize += sizeof(WHEA_PROCESSOR_GENERIC_ERROR_SECTION);
    RequiredSize += Packet->Header.Length;
    if (BufferSize < RequiredSize) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Initialize the record header.
    //

    RtlZeroMemory(ErrorRecord, BufferSize);
    WheaInitializeRecordHeader(&ErrorRecord->Header);
    ErrorRecord->Header.SectionCount = 3;
    ErrorRecord->Header.Severity = Packet->Header.ErrorSeverity;
    ErrorRecord->Header.Length = BufferSize;
    ErrorRecord->Header.NotifyType = PCIe_NOTIFY_TYPE_GUID;

    //
    // Initialize the PCI express section.
    //

    Descriptor = Add2Ptr(ErrorRecord, sizeof(WHEA_ERROR_RECORD_HEADER));
    Offset = sizeof(WHEA_ERROR_RECORD_HEADER) +
        (sizeof(WHEA_ERROR_RECORD_SECTION_DESCRIPTOR) *
         Source->MaxSectionsPerRecord);

    Descriptor->SectionOffset = Offset;
    Descriptor->SectionLength = sizeof(WHEA_PCIEXPRESS_ERROR_SECTION);
    Descriptor->Revision.AsUSHORT =
        WHEA_ERROR_RECORD_SECTION_DESCRIPTOR_REVISION;

    Descriptor->Flags.Primary = 1;
    Descriptor->SectionType = PCIEXPRESS_ERROR_SECTION_GUID;
    Descriptor->SectionSeverity = Packet->Header.ErrorSeverity;
    RtlCopyMemory(Add2Ptr(ErrorRecord, Offset),
                  &Packet->PciExpressError,
                  sizeof(WHEA_PCIEXPRESS_ERROR_SECTION));

    //
    // Initialize the processor generic error section.
    //

    Descriptor += 1;
    Offset += sizeof(WHEA_PCIEXPRESS_ERROR_SECTION);
    Processor = Add2Ptr(ErrorRecord, Offset);
    HalWheaInitProcessorGenericSection(Descriptor, Processor);
    Descriptor->SectionOffset = Offset;

    //
    // Initialize the error packet section.
    //

    Descriptor += 1;
    Offset += sizeof(WHEA_PROCESSOR_GENERIC_ERROR_SECTION);
    Descriptor->SectionOffset = Offset;
    Descriptor->SectionLength = Packet->Header.Length;
    Descriptor->Revision.AsUSHORT =
        WHEA_ERROR_RECORD_SECTION_DESCRIPTOR_REVISION;

    Descriptor->SectionType = WHEA_ERROR_PACKET_SECTION_GUID;
    Descriptor->SectionSeverity = Packet->Header.ErrorSeverity;
    RtlCopyMemory(Add2Ptr(ErrorRecord, Offset),
                  Packet,
                  Packet->Header.Length);

    return STATUS_SUCCESS;
}
