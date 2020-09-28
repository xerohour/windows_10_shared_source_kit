/*++

Copyright (C) Microsoft. All rights reserved

Module Name:

    callback.c

Abstract:

    This module implements kernel mode callback functionality provided by the
    PnP subsystem.

Author:

    Jamie Schwartz (jamschw) 08-Aug-2010
    Vassili Nikolaev (vasnik) 15-Oct-2010

Environment:

    Kernel mode

--*/

#include "pnpmgrp.h"
#pragma hdrstop

#pragma alloc_text(PAGE, IoRegisterBootDriverCallback)
#pragma alloc_text(INIT, IoUnregisterBootDriverCallback)
#pragma alloc_text(INIT, PnpCloseBootDriverCallbackRegistration)
#pragma alloc_text(INIT, PnpEarlyLaunchHasCallbacksRegistered)
#pragma alloc_text(INIT, PnpEarlyLaunchImageNotificationPreProcess)
#pragma alloc_text(INIT, PnpEarlyLaunchImageNotificationPostProcess)
#pragma alloc_text(INIT, PnpEarlyLaunchStatusNotificationPreProcess)
#pragma alloc_text(INIT, PnpNotifyEarlyLaunchImageLoad)
#pragma alloc_text(INIT, PnpNotifyEarlyLaunchStatusUpdate)

#define IO_BOOTDRIVER_CALLBACK_NAME L"\\Callback\\BootDriver"

#ifdef ALLOC_DATA_PRAGMA
#pragma data_seg("INITDATA")
#pragma bss_seg("INITDATAZ")
#pragma const_seg("INITCONST")
#endif

PCALLBACK_OBJECT PnpBootDriverCallbackObject;

//
// Rank of classifications in ascending order. (Higher rank has precedence.)
//

const ULONG PnpClassificationRank[] = {
    0, //BdCbClassificationUnknownImage
    1, //BdCbClassificationKnownGoodImage
    3, //BdCbClassificationKnownBadImage
    2  //BdCbClassificationKnownBadImageBootCritical
};

//
// Please update the PnpClassificationRank array when modifying the 
// classification enumeration types.
//

C_ASSERT(BdCbClassificationEnd == ELEMENT_COUNT(PnpClassificationRank));

#ifdef ALLOC_DATA_PRAGMA
#pragma bss_seg()
#pragma data_seg()
#pragma const_seg()
#endif

BOOLEAN PnpBootDriverCallbackRegistrationClosed;

PVOID
IoRegisterBootDriverCallback (
    __in PBOOT_DRIVER_CALLBACK_FUNCTION CallbackFunction,
    __in_opt PVOID CallbackContext
    )

/*++

Routine Description:

    This routine registers a callback function to be called during the
    initialization of boot start drivers and their dependent DLLs.

Arguments:

    CallbackFunction - Supplies the function to be called when initializing
        a boot start driver or DLL.

    CallbackContext - Supplies an opaque context that is passed to the callback
        function.

Return Value:

    A handle representing the registration. The handle must be provided to
    deregister the callback.

--*/

{

    OBJECT_ATTRIBUTES Attributes;
    PVOID Handle;
    UNICODE_STRING Name;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // Fail the request if the registration window has passed.
    //

    if (PnpBootDriverCallbackRegistrationClosed != FALSE) {
        return NULL;
    }

    //
    // If this is the first register caller, create a callback object. Allow
    // for multiple registered callback routines.
    //

    if (PnpBootDriverCallbackObject == NULL) {
        RtlInitUnicodeString(&Name, IO_BOOTDRIVER_CALLBACK_NAME);
        InitializeObjectAttributes(&Attributes,
                                   &Name,
                                   OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                   NULL,
                                   NULL);

        Status = ExCreateCallback(&PnpBootDriverCallbackObject,
                                  &Attributes,
                                  TRUE,
                                  TRUE);

        if (!NT_SUCCESS(Status)) {
            return NULL;
        }
    }

    //
    // Register the callback object.
    //

    Handle = ExRegisterCallback(PnpBootDriverCallbackObject,
                                (PCALLBACK_FUNCTION)CallbackFunction,
                                CallbackContext);

    return Handle;
}

VOID
IoUnregisterBootDriverCallback (
    __in PVOID CallbackHandle
    )

/*++

Routine Description:

    This routine deregisters a previously registered boot driver callback.

Arguments:

    CallbackHandle - Supplies a registration handle that was returned from
        callback registration.

Return Value:

    None.

--*/

{

    ExUnregisterCallback(CallbackHandle);
    return;
}

VOID
PnpCloseBootDriverCallbackRegistration (
    VOID
    )

/*++

Routine Description:

    This routine closes the registration window for boot driver callbacks.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PnpBootDriverCallbackRegistrationClosed = TRUE;
    return;
}

LOGICAL
PnpEarlyLaunchHasCallbacksRegistered (
    VOID
    )

/*++

Routine Description:

    This routine determines whether there are registrants for any of the 
    callbacks.

Arguments:

    None.

Return Value:

    None.

--*/

{

    return (PnpBootDriverCallbackObject != NULL);
}

VOID
PnpEarlyLaunchImageNotificationPreProcess (
    __in PVOID Context,
    __in PVOID CallbackType,
    __in PBDCB_IMAGE_INFORMATION ImageInformation,
    __in PBDCB_CLASSIFICATION Classification,
    __in PBDCB_IMAGE_INFORMATION ControlImageInformation
    )

/*++

Routine Description:

    This function pre processes notification data before it is forwarded to 
    early launch drivers.

Arguments:

    Context - Supplies context that will be passed to early launch drivers.

    CallbackType - Supplies CallbackType that will passed to early launch 
        drivers.

    ImageInformation - Supplies ImageInformation that will passed to early 
        launch drivers.

    Classification - Supplies classification from pnp.

    ControlImageInformation - Supplies original image information.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(CallbackType);
    UNREFERENCED_PARAMETER(Classification);

    //
    // Restore image information context.
    // Copy all the parameters so EarlyLaunch drivers cannot interfere with 
    // eachothers data.
    //

    RtlCopyMemory(ImageInformation,
                  ControlImageInformation,
                  sizeof(BDCB_IMAGE_INFORMATION));

    RtlDuplicateUnicodeString(0,
                              &ControlImageInformation->ImageName,
                              &ImageInformation->ImageName);

    RtlDuplicateUnicodeString(0,
                              &ControlImageInformation->RegistryPath,
                              &ImageInformation->RegistryPath);

    RtlDuplicateUnicodeString(0,
                              &ControlImageInformation->CertificateIssuer,
                              &ImageInformation->CertificateIssuer);

    RtlDuplicateUnicodeString(0,
                              &ControlImageInformation->CertificatePublisher,
                              &ImageInformation->CertificatePublisher);

    PnpDiagnosticTraceObject(&KMPnPEvt_EarlyLaunch_LoadNotification_Start,
                             &ImageInformation->ImageName);

    return;
}

VOID
PnpEarlyLaunchImageNotificationPostProcess (
    __in PVOID Context,
    __in PVOID CallbackType,
    __in PBDCB_IMAGE_INFORMATION ReturnedImageInformation,
    __in PBDCB_CLASSIFICATION Classification,
    __in PBDCB_IMAGE_INFORMATION ControlImageInformation
    )

/*++

Routine Description:

    This function post processes notification data after an early launch driver
    has been called.

Arguments:

    Context - Supplies context that was passed to early launch drivers.

    CallbackType - Supplies CallbackType that was passed to early launch 
        drivers.

    ReturnedImageInformation - Supplies ImageInformation that was passed to 
        early launch drivers.

    Classification - Supplies classification from pnp.

    ControlImageInformation - Supplies original image information.


Return Value:

    None.

--*/

{

    BDCB_CLASSIFICATION AmClassification;

    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(CallbackType);
    UNREFERENCED_PARAMETER(ControlImageInformation);

    PnpDiagnosticTraceObject(&KMPnPEvt_EarlyLaunch_LoadNotification_Stop,
                             &ControlImageInformation->ImageName);

    AmClassification = ReturnedImageInformation->Classification;

    //
    // Verify the returned classification and check it against the current 
    // classificication, based on the priority.
    //

    NT_ASSERT(AmClassification < ELEMENT_COUNT(PnpClassificationRank));
    NT_ASSERT(*Classification < ELEMENT_COUNT(PnpClassificationRank));

    if (PnpClassificationRank[AmClassification] > 
        PnpClassificationRank[*Classification]) {

        *Classification = AmClassification;
    }

    //
    // Deallocate unicode strings.
    // N.B After serious discussions it was decided that deallocating here
    // is safe since any memory leaks that may occur from EarlyLaunch drivers 
    // will be caught during logo test. And in case they are not these 
    // functions can be changed.
    //

    RtlFreeUnicodeString(&ReturnedImageInformation->ImageName);
    RtlFreeUnicodeString(&ReturnedImageInformation->CertificateIssuer);
    RtlFreeUnicodeString(&ReturnedImageInformation->CertificatePublisher);
    return;
}

VOID
PnpEarlyLaunchStatusNotificationPreProcess (
    __in PVOID Context,
    __in PVOID CallbackType,
    __in PBDCB_STATUS_UPDATE_CONTEXT StatusContext,
    __in PVOID Classification,
    __in PBDCB_STATUS_UPDATE_CONTEXT ControlStatusContext
    )

/*++

Routine Description:

    This function pre processes notification data before it is forwarded to 
    early launch drivers.

Arguments:

    Context - Supplies context that will be passed to early launch drivers.

    CallbackType - Supplies CallbackType that will passed to early launch 
        drivers.

    StatusContext - Supplies StatusContext that will passed to early 
        launch drivers.

    Classification - Supplies classification from pnp.

    ControlStatusContext - Supplies original ControlStatusContext information.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(CallbackType);
    UNREFERENCED_PARAMETER(Classification);

    RtlCopyMemory(StatusContext,
                  ControlStatusContext,
                  sizeof(BDCB_STATUS_UPDATE_CONTEXT));

    return;
}

VOID
PnpNotifyEarlyLaunchImageLoad (
    __in PBDCB_IMAGE_INFORMATION ImageInformation,
    __inout PBDCB_CLASSIFICATION Classification
    )

/*++

Routine Description:

    This function notifies all registered drivers, and calls back after each 
    notification. .

Arguments:

    ImageInformation - Supplies a pointer will be passed to callback function.

    Classification - Supplies a pointer to the variable which will receive the 
        Classification value at the end of the notification.

Return Value:

    None.

--*/

{

    EX_PROCESSING_DATA ProcessingData;
    BDCB_IMAGE_INFORMATION ReplicatedImageInformation;

    //
    // Setup processing data structure and notify registrants.
    //

    if (PnpEarlyLaunchHasCallbacksRegistered() != FALSE) {
        ProcessingData.PreProcessing = 
            PnpEarlyLaunchImageNotificationPreProcess;

        ProcessingData.PostProcessing = 
            PnpEarlyLaunchImageNotificationPostProcess;

        ProcessingData.Argument1 = Classification;
        ProcessingData.Argument2 = ImageInformation;
        ExNotifyWithProcessing(PnpBootDriverCallbackObject,
                               (PVOID)BdCbInitializeImage,
                               &ReplicatedImageInformation,
                               &ProcessingData);
    }

    return;
}

VOID
PnpNotifyEarlyLaunchStatusUpdate (
    __in BDCB_STATUS_UPDATE_TYPE Status
    )

/*++

Routine Description:

    This function notifies all registered drivers, and calls back after each 
    notification. .

Arguments:

    Status - Supplies the notification status that will be passed to Early 
        Launch drivers.

Return Value:

    None.

--*/

{

    EX_PROCESSING_DATA ProcessingData;
    BDCB_STATUS_UPDATE_CONTEXT ReplicatedUpdateContext;
    BDCB_STATUS_UPDATE_CONTEXT UpdateContext;

    //
    // Setup processing data structure and notify registrants.
    //
    
    if (PnpEarlyLaunchHasCallbacksRegistered() != FALSE) {
        PnpDiagnosticTraceElamStatus(
                           &KMPnPEvt_EarlyLaunch_StatusNotification_Start,
                           Status
                           );

        UpdateContext.StatusType = Status;
        ProcessingData.PreProcessing = 
            PnpEarlyLaunchStatusNotificationPreProcess;

        ProcessingData.PostProcessing = NULL;
        ProcessingData.Argument1 = NULL;
        ProcessingData.Argument2 = &UpdateContext;
        ExNotifyWithProcessing(PnpBootDriverCallbackObject,
                               (PVOID)BdCbStatusUpdate,
                               &ReplicatedUpdateContext,
                               &ProcessingData);

        PnpDiagnosticTraceElamStatus(
                           &KMPnPEvt_EarlyLaunch_StatusNotification_Stop,
                           Status
                           );
    }

    return;
}
