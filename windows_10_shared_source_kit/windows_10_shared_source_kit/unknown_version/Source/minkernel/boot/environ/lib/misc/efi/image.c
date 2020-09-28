/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    image.c

Abstract:

    This module provides support for loading and starting images using
    EFI.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "image.h"

// -------------------------------------------------------- Firmware Prototypes

NTSTATUS
ImgArchEfiStartBootApplication (
    __in PBOOT_APPLICATION_TRANSITION_ENTRY ApplicationEntry,
    __in_bcount(ImageSize) PVOID ImageBase,
    __in ULONG ImageSize,
    __in ULONG StartFlags,
    __out PBOOT_APPLICATION_RETURN_ARGUMENTS ReturnArgs
    );

// ------------------------------------------------------------------ Functions

NTSTATUS
ImgFwLoadBootApplication (
    __in DEVICE_ID DeviceId,
    __in PWSTR FilePath,
    __in ULONG Attributes,
    __deref_out_bcount(*ImageSize) PVOID *ImageBase,
    __out PULONG ImageSize,
    __out_bcount_opt(MAX_HASH_LEN) PUCHAR ImageHash,
    __in ULONG ImageFlags,
    __out PULONG LoadInformation
    )

/*++

Routine Description:

    This routine will load the specified image into memory.

Arguments:

    DeviceId - The device containing the specified boot application.

    FilePath - The file path for the specified boot application.

    Attributes - Supplies attributes of the boot entry.

    ImageBase - On return, contains the address of the loaded image.

    ImageSize - On return, contains the in-memory size of the loaded image.

    ImageHash - If present then on return contains the hash of the loaded image.

    ImageFlags - Supplies a set of image load flags that must be used during
        this image load.  Additional load flags can be added by this routine.

    LoadInformation - Provides flags that describe properties of the loaded image.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_SUCH_DEVICE if Device is invalid.
    STATUS_NO_SUCH_FILE if FilePath does not exist.
    STATUS_INVALID_IMAGE_FORMAT if the file exists, but is neither a
        legacy loader or a PE boot application.

--*/

{

    ULONG Flags;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Attributes);

    //
    // Load the application into memory.  All boot applications are PE images
    // and can be loaded as such.
    //
    // N.B.  All boot applications need to be mapped one to one since they
    //       can disable address translation at any time.
    //

    Flags =
        ImageFlags |
        IMAGE_FLAGS_BOOT_APPLICATION |
        IMAGE_FLAGS_MAP_ONE_TO_ONE |
        IMAGE_FLAGS_VALIDATE_MACHINE_TYPE;

    *ImageBase = NULL;                       // No preferred address.
    Status = BlImgLoadPEImageWithInfo(DeviceId,
                                      MEMORY_TYPE_BOOT_APPLICATION,
                                      FilePath,
                                      ImageBase,
                                      ImageSize,
                                      ImageHash,
                                      Flags,
                                      LoadInformation);

    return Status;
}

NTSTATUS
ImgFwUnloadBootApplication (
    __in_bcount(ImageSize) PVOID ImageBase,
    __in ULONG ImageSize
    )

/*++

Routine Description:

    This routine will unload a boot application loaded by
    ImageFirmwareLoadBootApplication.

Arguments:

    ImageBase - Base address of the loaded image.

    ImageSize - Size (in bytes) of the loaded image.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_MEMORY_NOT_ALLOCATED if there as an error freeing the memory
        allocated for the image.

--*/

{

    NTSTATUS Status;

    //
    // Free page allocation made by BlImgLoadPEImage.
    //

    Status = BlImgUnLoadImage(ImageBase, ImageSize, IMAGE_FLAGS_NONE);
    if (!NT_SUCCESS(Status)) {
        return STATUS_MEMORY_NOT_ALLOCATED;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
ImgFwStartBootApplication (
    __in PBOOT_APPLICATION_TRANSITION_ENTRY ApplicationEntry,
    __in_bcount(ImageSize) PVOID ImageBase,
    __in ULONG ImageSize,
    __in ULONG StartFlags,
    __out PBOOT_APPLICATION_RETURN_ARGUMENTS ReturnArgs
    )

/*++

Routine Description:

    This routine will transfer execution to a boot application loaded by
    ImageFirmwareBootApplication.

Arguments:

    ApplicationEntry - Supplies a boot entry for the application.

    ImageBase - Base address of the loaded image.

    ImageSize - Size (in bytes) of the loaded image.

    StartFlags - Supplies start flags. Possible values include:

        IMAGE_FLAGS_ISOLATED_EXECUTION_CONTEXT - Specifies that the child
            application should run completely in its own execution context.

    ReturnArgs - Supplies a buffer for the application's return arguments.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if the image type is invalid.

--*/

{

    return ImgArchEfiStartBootApplication(ApplicationEntry,
                                          ImageBase,
                                          ImageSize,
                                          StartFlags,
                                          ReturnArgs);
}

