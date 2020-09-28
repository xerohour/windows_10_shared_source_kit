/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name: 

    driver.cpp

Abstract:

    This module contains the loading and binding code for the SPB class
    extension driver.

Environment:

    kernel-mode only

Revision History:

--*/

//
// Generate the function table when inporting scxdynamics.h thorugh internal.h
//

#define SCX_DYNAMICS_GENERATE_TABLE 1

#include "internal.h"

extern "C" 
{
#include "driver.tmh"
}

//
// Forward declarations of module internal functions
//

extern "C" 
{
DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_UNLOAD EvtDriverUnload;

NTSTATUS
CxInitialize(
    VOID
    );

VOID
CxDeinitialize(
    VOID
    );

NTSTATUS
CxBindClient(
    __inout PWDF_CLASS_BIND_INFO ClassInfo,
    __in    PWDF_COMPONENT_GLOBALS ClientGlobals
    );

VOID
CxUnbindClient(
    __inout PWDF_CLASS_BIND_INFO ClassInfo,
    __in    PWDF_COMPONENT_GLOBALS ClientGlobals
    );
}

//
// Globals
//

WDF_CLASS_LIBRARY_INFO g_WdfClassLibraryInfo =
{
    sizeof(WDF_CLASS_LIBRARY_INFO),                             // Size
    {
        __SPB_MAJOR_VERSION,  // Major
        __SPB_MINOR_VERSION,  // Minor
        __SPB_BUILD_NUMBER ,  // Build
    },                                                          // Version
    CxInitialize,                                     // ClassLibraryInitialize
    CxDeinitialize,                                   // ClassLibraryDeinitialize
    CxBindClient,                                     // ClassLibraryBindClient
    CxUnbindClient,                                   // ClassLibraryUnbindClient
};

WDFDEVICE g_ClassLibraryDevice = NULL;

#define MAX_CONTROL_OBJECT_NAME_CCH 100

NTSTATUS
#pragma prefast(suppress:__WARNING_DRIVER_FUNCTION_TYPE, "thanks, i know this already")
DriverEntry(
    __in PDRIVER_OBJECT  DriverObject,
    __in PUNICODE_STRING RegistryPath
    )
{
    WDF_DRIVER_CONFIG config;
    PWDFDEVICE_INIT pInit;
    NTSTATUS status;
    ULONG i;
    DECLARE_UNICODE_STRING_SIZE(name, MAX_CONTROL_OBJECT_NAME_CCH);

    //
    // Initialize WPP for debugging-style events
    //

    WPP_INIT_TRACING(DriverObject, RegistryPath);

    //
    // Register with ETW for manifested events
    //

    EventRegisterMicrosoft_Windows_SPB_ClassExtension();

    //
    // Setup the WDF driver object for the class extension.
    //

    WDF_DRIVER_CONFIG_INIT(&config, NULL);
    config.DriverInitFlags = WdfDriverInitNonPnpDriver;
    config.EvtDriverUnload = EvtDriverUnload;

    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             NULL,
                             &config,
                             NULL);
    if (!NT_SUCCESS(status)) {
        TraceMessage(TRACE_LEVEL_ERROR,
                     TRACE_FLAG_LOADING,
                     "DriverEntry failure - could not create WDF driver (%!STATUS!)", 
                     status);
        goto exit;
    }

    //
    // Allocate a control device init structure to create the control device 
    // with which WDFLDR communciates
    //

    pInit = WdfControlDeviceInitAllocate(WdfGetDriver(), &SDDL_DEVOBJ_KERNEL_ONLY);
    if (pInit == NULL) {
        TraceMessage(TRACE_LEVEL_ERROR,
                     TRACE_FLAG_LOADING,
                     "DriverEntry failure - could not create control device init (%!STATUS!)", 
                     status);
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    //
    // Loop creating the control device until we find an available name.
    // 

    for(i = 0, status = STATUS_OBJECT_NAME_COLLISION; 
        status == STATUS_OBJECT_NAME_COLLISION;
        i += 1)
    {
        status = RtlUnicodeStringPrintf(&name, L"%s%d", SCX_CONTROL_DEVICE_NAME, i++);
        if (!NT_SUCCESS(status)) {
            break;
        }

        status = WdfDeviceInitAssignName(pInit, &name);
        if (!NT_SUCCESS(status)) {
            break;
        }

        status = WdfDeviceCreate(&pInit, NULL, &g_ClassLibraryDevice);
    }

    if (!NT_SUCCESS(status))
    {
        WdfDeviceInitFree(pInit);
        pInit = NULL;
        goto exit;
    }

    //
    // Finish creating the control object and then register as a class
    // extension.
    // 

    WdfControlFinishInitializing(g_ClassLibraryDevice);

    status = WdfRegisterClassLibrary(&g_WdfClassLibraryInfo,
                                     RegistryPath,
                                     &name);
    if (!NT_SUCCESS(status)) {
        WdfObjectDelete(g_ClassLibraryDevice);
        g_ClassLibraryDevice = NULL;
        goto exit;
    }
    
    //
    // Cleanup in the event of an error.
    //

exit:

    name.Buffer[MAX_CONTROL_OBJECT_NAME_CCH-1] = UNICODE_NULL;

    if (!NT_SUCCESS(status))
    {
        TraceMessage(TRACE_LEVEL_ERROR,
                     TRACE_FLAG_LOADING,
                     "DriverEntry failure - could not create control device (%!STATUS!)", 
                     status);

        EventWrite_SPBCX_START_FAILURE(NULL, name.Buffer, status);

        WPP_CLEANUP(DriverObject);
        EventUnregisterMicrosoft_Windows_SPB_ClassExtension();
    }
    else
    {
        TraceMessage(TRACE_LEVEL_INFORMATION,
                     TRACE_FLAG_LOADING,
                     "SpbCx driver loaded - control device name = %wZ", &name);

        EventWrite_SPBCX_START_OK(NULL, name.Buffer, status);
    }

    return status;
}

VOID
EvtDriverUnload(
    __in WDFDRIVER DriverObject
    )
{
    if (g_ClassLibraryDevice != NULL) {
        WdfObjectDelete(g_ClassLibraryDevice);
        g_ClassLibraryDevice = NULL;
    }

    TraceMessage(TRACE_LEVEL_INFORMATION,
                 TRACE_FLAG_LOADING,
                 "SpbCx driver unloading");

    EventWrite_SPBCX_UNLOAD(NULL);
    EventUnregisterMicrosoft_Windows_SPB_ClassExtension();
    WPP_CLEANUP(DriverObject);
}

NTSTATUS
CxInitialize(
    VOID
    )
{
    return STATUS_SUCCESS;
}

VOID
CxDeinitialize(
    VOID
    )
{
}

NTSTATUS
CxBindClient(
    __inout PWDF_CLASS_BIND_INFO   ClassInfo,
    __in    PWDF_COMPONENT_GLOBALS /* ClientGlobals */
    )
{
    PSCX_CLIENT_GLOBALS pGlobals;
    NTSTATUS status;

    if (ClassInfo->Version.Major != __SPB_MAJOR_VERSION)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_LOADING,
            "Incorrect major version %lu. Major version must be %lu.",
            ClassInfo->Version.Major,
            __SPB_MAJOR_VERSION
            );
        goto exit;
    }

    switch (ClassInfo->Version.Minor)
    {
    case __SPB_MINOR_VERSION:
        if (ClassInfo->FunctionTableCount != SpbFunctionTableNumEntries)
        {
            status = STATUS_INVALID_PARAMETER;
            TraceMessage(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_LOADING,
                "Controller function table count (%d) doesn't match with "
                "SPB framework extension (%d). Rebuild the controller driver.",
                ClassInfo->FunctionTableCount,
                SpbFunctionTableNumEntries);
            goto exit;
        }
        break;

    //
    // If a new minor version is introduced, add additional "cases" here. Check to see if
    // the function table count matches the count expected for the reported minor version.
    //

    default:
        status = STATUS_INVALID_PARAMETER;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_LOADING,
            "Unsupported minor version. Controller driver minor version: %lu, "
            "SPB framework minor version: %lu.",
            ClassInfo->Version.Minor,
            __SPB_MINOR_VERSION);
        goto exit;
    }

    RtlCopyMemory(ClassInfo->FunctionTable,
                  &(SpbVersion.Functions),
                  ClassInfo->FunctionTableCount * sizeof(PVOID));

    pGlobals = (PSCX_CLIENT_GLOBALS) ExAllocatePoolWithTag(NonPagedPoolNx,
                                     sizeof(SCX_CLIENT_GLOBALS),
                                     'rDuC');
    if (pGlobals == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        TraceMessage(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_LOADING,
            "Failed to allocate client globals - %!STATUS!",
            status);
        goto exit;
    }

    pGlobals->Signature = SCX_GLOBALS_SIG;

    *((PSPB_DRIVER_GLOBALS*)ClassInfo->ClassBindInfo) = &pGlobals->Public;

    status = STATUS_SUCCESS;

exit:
    
    return status;
}

VOID
CxUnbindClient(
    __inout PWDF_CLASS_BIND_INFO   ClassInfo,
    __in    PWDF_COMPONENT_GLOBALS /* ClientGlobals */
    )
{
    PSCX_CLIENT_GLOBALS pGlobals;

    if (*((PVOID*)ClassInfo->ClassBindInfo) == NULL) {
        return;
    }

    pGlobals = GetPrivateGlobals(
        *((PSPB_DRIVER_GLOBALS*)ClassInfo->ClassBindInfo)
        );

    ScxVerifyPrivateGlobals(pGlobals);

    ExFreePool(pGlobals);

    *((PVOID*)ClassInfo->ClassBindInfo) = NULL;
}
