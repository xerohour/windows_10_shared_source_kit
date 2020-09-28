//
//    Copyright (C) Microsoft.  All rights reserved.
//
/*++

Module Name:

    device.cpp

Abstract:

   This file contains the device entry points and callbacks.

Author:

    Arvind Aiyar March 2011
    
Environment:

    Kernel-mode Driver Framework

--*/

#include "internal.h"

#include "device.tmh"

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, OnD0Exit)
    #pragma alloc_text(PAGE, DeviceReportPnpProblem)
#endif

//
// Declarations from ntrtl_x.h, which cannot be included with wdm.h
//

typedef struct _MESSAGE_RESOURCE_ENTRY {
    USHORT Length;
    USHORT Flags;
    UCHAR Text[ 1 ];
} MESSAGE_RESOURCE_ENTRY, *PMESSAGE_RESOURCE_ENTRY;

EXTERN_C
NTSYSAPI
NTSTATUS
NTAPI
RtlFindMessage (
    __in PVOID DllHandle,
    __in ULONG MessageTableId,
    __in ULONG MessageLanguageId,
    __in ULONG MessageId,
    __out PMESSAGE_RESOURCE_ENTRY *MessageEntry
    );


NTSTATUS
OnPrepareHardware(
    _In_  WDFDEVICE    FxDevice,
    _In_  WDFCMRESLIST FxResourcesRaw,
    _In_  WDFCMRESLIST FxResourcesTranslated
    )
/*++
 
  Routine Description:

    This routine is called by the PnP manager with a list of 
    resources (CmResourceTypeConnection) assigned to it

  Arguments:

    FxDevice - a handle to the framework device object
    FxResourcesRaw - list of translated hardware resources that 
        the PnP manager has assigned to the device
    FxResourcesTranslated - list of raw hardware resources that 
        the PnP manager has assigned to the device

  Return Value:

    NTSTATUS indicating sucess or failure

--*/
{
    NTSTATUS status;
    
    FuncEntry(TRACE_FLAG_INIT);

    PDEVICE_CONTEXT pDeviceContext = GetDeviceContext(FxDevice);
    NT_ASSERT(pDeviceContext != NULL);

    //
    // Initialize the SPB connection and SPB request object
    //
    status = SpbInitialize(
                    pDeviceContext,
                    FxResourcesRaw,
                    FxResourcesTranslated);

    if (!NT_SUCCESS(status))
    {
        goto exit;
    }
    
    //
    // Get Device and HID-specific Information from ACPI 
    //
    status = AcpiInitialize(pDeviceContext);

    if (!NT_SUCCESS(status))
    {
        goto exit;
    }

exit:
    FuncExit(TRACE_FLAG_INIT);
    return status;
}


NTSTATUS
OnD0Entry(
   _In_ WDFDEVICE               FxDevice,
   _In_ WDF_POWER_DEVICE_STATE  FxPreviousState
   )
/*++

Routine Description:

    This routine is called when the device is entering the D0 powered on state.
    It must perform any operations that are necessary before the specified 
    device is used.
    
Arguments:
    
    FxDevice - Handle to the framework device object
    FxPreviousState - Device power state which the device was in most recently

Return Value:

    NTSTATUS indicating success or failure

*/
{
    NTSTATUS status;

    FuncEntry(TRACE_FLAG_INIT);

    PDEVICE_CONTEXT pDeviceContext = GetDeviceContext(FxDevice);
    NT_ASSERT(pDeviceContext != NULL);

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_FLAG_INIT,
        "Device:0x%p is resuming from D%d state",
        FxDevice,
        FxPreviousState - 1);

    //
    // Initialize the underlying hardware and retrieve
    // HID data from the device
    //
    status = HidInitialize(pDeviceContext, FxPreviousState);

    if (!NT_SUCCESS(status))
    {
        goto exit;
    }

    //
    // Complete any pended Idle IRPs
    //
    PowerIdleIrpCompletion(pDeviceContext);

    //
    // Log an ETW event for successful initialization
    //
    EventWrite_HIDI2C_ENUMERATION_OK(NULL, FxDevice, status);

exit:
    FuncExit(TRACE_FLAG_INIT);
    return status;
}

NTSTATUS
OnPostInterruptsEnabled (
_In_ WDFDEVICE FxDevice,
_In_ WDF_POWER_DEVICE_STATE FxPreviousState
)
/*++

Routine Description:

    This routine is invoked by the framework post enabling the interrupts
    on the device. Now that interrupts are enabled, we can safely RESET
    the device if required.

Arguments:

    FxDevice - a handle to the framework device object
    FxTargetState - Device power state which the device will be put in after this call

Return Value:

    NTSTATUS indicating success or failure

*/
{
    FuncEntry(TRACE_FLAG_INIT);
    
    NTSTATUS status = STATUS_SUCCESS;

    PDEVICE_CONTEXT pDeviceContext = GetDeviceContext(FxDevice);
    NT_ASSERT(pDeviceContext != NULL);
    
    //
    // Only do a HID reset if we are coming into D0 
    // after a shutdown, device removal or resource rebalance. 
    // In all other cases, we are coming back into D0 after Dx idle
    //
    if (FxPreviousState == WdfPowerDeviceD3Final)
    {
        //
        // Reset the device. This should tell the device to
        // send data and resume all normal activity
        //
        status = HidReset(pDeviceContext);

        if (!NT_SUCCESS(status)) 
        {
            TraceEvents(
                    TRACE_LEVEL_ERROR,
                    TRACE_FLAG_INTERRUPT,
                    "HidReset failed resetting device "
                    "status:%!STATUS!",
                    status);			
            DeviceReportPnpProblem(FxDevice, HIDI2C_RESET_ERROR);
        }
    }

    FuncExit(TRACE_FLAG_INIT);
    return status;
}


NTSTATUS
OnD0Exit(
   _In_ WDFDEVICE               FxDevice,
   _In_ WDF_POWER_DEVICE_STATE  FxTargetState
   )
/*++

Routine Description:

    This routine is called whenever the device leaves the D0 state, which happens 
    when the device is stopped, when it is removed, and when it is powered off.
    If the driver needs to save hardware state before the device is powered down, then
    that should be done here.Note that interrupts have already been disabled by the time
    that this callback is invoked.

Arguments:

    FxDevice - a handle to the framework device object
    FxTargetState - Device power state which the device will be put in after this call

Return Value:

    NTSTATUS indicating success or failure

*/
{
    NTSTATUS status;

    FuncEntry(TRACE_FLAG_INIT);
    
    //
    // This macro will assert if this routine runs at >= DISPATCH_LEVEL
    //
    PAGED_CODE();

    PDEVICE_CONTEXT pDeviceContext = GetDeviceContext(FxDevice);
    NT_ASSERT(pDeviceContext != NULL);

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_FLAG_INIT,
        "Device:0x%p is going to D%d state",
        FxDevice,
        FxTargetState - 1);

    //
    // Uninitialize the underlying hardware
    //
    status = HidDestroy(pDeviceContext, FxTargetState);

    FuncExit(TRACE_FLAG_INIT);
    return status;
}

NTSTATUS
OnReleaseHardware(
    _In_  WDFDEVICE    FxDevice,
    _In_  WDFCMRESLIST FxResourcesTranslated
    )
/*++
 
  Routine Description:

    This routine cleans up any resources provided.

  Arguments:

    FxDevice - a handle to the framework device object
    FxResourcesTranslated - list of raw hardware resources that 
        the PnP manager has assigned to the device

  Return Value:

    NTSTATUS indicating sucesss or failure

--*/
{
    UNREFERENCED_PARAMETER(FxResourcesTranslated);
    
    NTSTATUS status = STATUS_SUCCESS;

    FuncEntry(TRACE_FLAG_INIT);
        
    PDEVICE_CONTEXT pDeviceContext = GetDeviceContext(FxDevice);
    NT_ASSERT(pDeviceContext != NULL);

    //
    // Destroy any SPB resources
    //
    SpbDestroy(pDeviceContext);

    
    FuncExit(TRACE_FLAG_INIT);
    return status;
}

VOID
DeviceReportPnpProblem(
    _In_ WDFDEVICE  Device,
    _In_ ULONG      MsgId
    )
/*++

Routine Description:

    Set an actionable error message in the device manager's "Device status" field if
    this device fails to start.

Arguments:

    Device - A handle to the framework device object
    MsgId - A message ID corresponding to a message in this driver's binary.

Return Value:

    None. Failure is not fatal.

--*/
{
    LPWSTR problemDescString;
    PMESSAGE_RESOURCE_ENTRY messageEntry;
    size_t messageStringSize;
    size_t problemDescStringSize;
    WCHAR driverPath[] = L"@System32\\drivers\\hidi2c.sys";
    NTSTATUS status;

    FuncEntry(TRACE_FLAG_INIT);

    PAGED_CODE();

    problemDescString = NULL;

    TraceLoggingPNPFailure(MsgId);


    //
    // Extract the problem description string from the message table.
    // This is the fallback string in case Device Manager can't extract
    // the string for the locale it is running under.
    //
#pragma prefast(disable:__WARNING_INACCESSIBLE_MEMBER, "We need DriverStart here.")
    status = RtlFindMessage(WdfDriverWdmGetDriverObject(WdfGetDriver())->DriverStart,
                            11, // RT_MESSAGETABLE
                            LANG_NEUTRAL,
                            MsgId,
                            &messageEntry);

    if (!NT_SUCCESS(status))
    {
        NT_ASSERTMSG("Unable to extract problem description from message table", 0);

        TraceEvents(TRACE_LEVEL_ERROR,
                    TRACE_FLAG_INIT,
                    "RtlFindMessage failed (%!STATUS!)",
                    status);

        goto exit;
    }

    messageStringSize = messageEntry->Length - FIELD_OFFSET(MESSAGE_RESOURCE_ENTRY, Text);

    if (messageStringSize > (NTSTRSAFE_MAX_CCH * sizeof(WCHAR)))
    {
        TraceEvents(TRACE_LEVEL_ERROR,
                    TRACE_FLAG_INIT,
                    "Abnormally large fallback string size.");

        goto exit;
    }

    //
    // Get the length of the extracted message string.
    //
    status = RtlUnalignedStringCbLengthW((LPWSTR) messageEntry->Text,
                                         messageStringSize,
                                         &messageStringSize);

    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR,
                    TRACE_FLAG_INIT,
                    "RtlUnalignedStringCbLengthW failed (%!STATUS!)",
                    status);
        goto exit;
    }

    //
    // Compute the size of the indirect string we will build.  It will have the following
    // format:
    //
    //     @System32\\drivers\\hidi2c.sys,#<MsgId>;<Fallback String>
    //
    // Where <MsgId> is the ULONG message ID in decimal for the message string (max 10 characters),
    // and <Fallback String> is the string extracted from the message table.
    //
    problemDescStringSize = sizeof(driverPath) +  // "@System32\drivers\hidi2c.sys"
                            13 * sizeof(WCHAR) +  // ",#<MsgId>;"
                            messageStringSize +   // <Fallback String>
                            sizeof(WCHAR);        // terminating NULL

    problemDescString = (LPWSTR) HIDI2C_ALLOCATE_POOL(NonPagedPoolNx, problemDescStringSize);

    if (problemDescString == NULL)
    {
        TraceEvents(TRACE_LEVEL_ERROR,
                    TRACE_FLAG_INIT,
                    "Failed allocating memory.");
        goto exit;
    }
 
    status = RtlStringCbPrintfW(problemDescString,
                                problemDescStringSize,
                                L"%s,#%d;%s",
                                driverPath,
                                MsgId,
                                (LPWSTR) messageEntry->Text);

    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR,
                    TRACE_FLAG_INIT,
                    "RtlStringCbPrintfW failed (%!STATUS!)",
                    status);
        goto exit;
    }

    //
    // Get the actual size.
    //
    status = RtlUnalignedStringCbLengthW(problemDescString,
                                         problemDescStringSize,
                                         &problemDescStringSize);

    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR,
                    TRACE_FLAG_INIT,
                    "RtlUnalignedStringCbLengthW failed (%!STATUS!)",
                    status);
        goto exit;
    }

    //
    // Set the problem description string for Device Manager.
    //
    status = IoSetDevicePropertyData(WdfDeviceWdmGetPhysicalDevice(Device),
                                     &DEVPKEY_Device_DriverProblemDesc,
                                     LOCALE_NEUTRAL,
                                     0,
                                     DEVPROP_TYPE_STRING_INDIRECT,
                                     (ULONG)(problemDescStringSize + sizeof(WCHAR)),
                                     problemDescString);

    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR,
                    TRACE_FLAG_INIT,
                    "Failed to set problem description (%!STATUS!)",
                    status);
        goto exit;
    }

exit:
    if (problemDescString)
    {
        HIDI2C_FREE_POOL(problemDescString);
    }

    FuncExit(TRACE_FLAG_INIT);
}
