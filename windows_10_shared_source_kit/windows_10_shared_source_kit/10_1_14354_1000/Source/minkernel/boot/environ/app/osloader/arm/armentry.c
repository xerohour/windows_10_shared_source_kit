/*++

Copyright (c) Microsoft Corporation

Module Name:

    armentry.c

Abstract:

    This module contains the entry point for an ARM OS Loader.

Environment:

    Boot.

--*/

// ------------------------------------------------------------------- Includes

#include "osloader.h"


//--------------------------------------------------------------------- Defines

//#define DBG_NEW_TRANSPORT 1


// ------------------------------------------------------------------ Functions

NTSTATUS
OslMain (
    __in PBOOT_APPLICATION_PARAMETERS Parameters
    )

/*++

Routine Description:

    Entry point for the x86-64 BIOS Boot OS Loader.

Arguments:

    Parameters - Incoming parameters to the OS loader, including firmware
        specific handles and an allocated memory descriptor list.

Return Value:

    Error code if fatal failure is experienced.

--*/

{

    LIBRARY_PARAMETERS LibraryParameters;
    PBOOT_APPLICATION_RETURN_ARGUMENTS ReturnArgs;
    ULONG ReturnFlags;
    NTSTATUS Status;

    ReturnArgs = Add2Ptr(Parameters, Parameters->ReturnArgumentOffset);
    ReturnArgs->Version = BOOT_APPLICATION_RETURN_ARGUMENTS_VERSION;

    //
    // Initialize the boot environment library.
    //

    LibraryParameters = BlGetDefaultLibraryParameters();
    LibraryParameters.FontDirectory = OSLOADER_DEFAULT_FONT_DIRECTORY;
    LibraryParameters.TranslationType = VA_TRANSLATION_TYPE_DEFAULT;
    LibraryParameters.HeapAllocationAttributes =
        MEMORY_ATTRIBUTE_ALLOCATION_KSEG;

    LibraryParameters.Flags |= LF_SVN_CHAIN_CHECK_SELF;
    LibraryParameters.Flags |= LF_NO_TCG_LOG_WRITETHROUGH;

#ifdef DBG_NEW_TRANSPORT

    LibraryParameters.Flags |= LF_DEBUG_TRANSPORT;

#endif

    //
    // Set the number of descriptors required to 512. This should be
    // sufficient to load ~1GB file
    //

    LibraryParameters.NumberOfDynamicDescriptors = 512;
    Status = BlInitializeLibrary(Parameters, &LibraryParameters);
    if (!NT_SUCCESS(Status)) {
        ReturnArgs->ReturnStatus = Status;
        return Status;
    }

#ifdef DBG_NEW_TRANSPORT

    LibraryParameters = BlGetCurrentLibraryParameters();
    LibraryParameters.Flags |=
        LF_BOOT_OPTIONS_INITIALIZED | LF_REINITIALIZE;

    Status = BlInitializeLibrary(Parameters, &LibraryParameters);

#endif

    //
    // Call the OS Loader's Architecture and Platform independent entry point.
    //

    Status = OslpMain(&ReturnFlags);

    //
    // The loader has returned control.  Record the information about why this
    // occurred and return to the boot manager.
    //

    ReturnArgs->ReturnStatus = Status;
    ReturnArgs->Flags = ReturnFlags;
    BlDestroyLibrary();
    return Status;
}
