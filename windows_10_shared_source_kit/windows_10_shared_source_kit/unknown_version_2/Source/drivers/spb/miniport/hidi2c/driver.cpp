//
//    Copyright (C) Microsoft.  All rights reserved.
//
/*++

Module Name:

    driver.cpp

Abstract:

    This file contains the driver entry points and callbacks.

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel-mode Driver Framework

--*/

#include "internal.h"

#include "driver.tmh"

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(INIT, DriverEntry )
    #pragma alloc_text(PAGE, OnDeviceAdd)
    #pragma alloc_text(PAGE, OnDriverUnload)
#endif

TRACELOGGING_DEFINE_PROVIDER(
    g_TraceLoggingProvider,
    TELEMETRY_EVENT_PROVIDER_NAME,
    // {b7651cf4-92fb-5e12-fd62-6dffda250e97} // bin\diagnosibility\ETW\TlgGuid.exe
    (0xb7651cf4, 0x92fb, 0x5e12, 0xfd, 0x62, 0x6d, 0xff, 0xda, 0x25, 0x0e, 0x97),
    TraceLoggingOptionMicrosoftTelemetry());

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    Installable driver initialization entry point.
    This entry point is called directly by the I/O system.

Arguments:

    DriverObject - pointer to the driver object
    RegistryPath - pointer to a unicode string representing the path,
                   to driver-specific key in the registry.

Return Value:

    STATUS_SUCCESS if successful, error code otherwise.

--*/
{   
    NTSTATUS status;

    //
    // Initialize WPP Tracing
    //
    WPP_INIT_TRACING(DriverObject, RegistryPath);

    //
    // Register with ETW for manifested events
    //
    EventRegisterMicrosoft_Windows_SPB_HIDI2C();


    WDF_DRIVER_CONFIG driverConfig;
    WDF_DRIVER_CONFIG_INIT(&driverConfig,OnDeviceAdd);
    driverConfig.EvtDriverUnload = OnDriverUnload;
    driverConfig.DriverPoolTag = HIDI2C_POOL_TAG;

    status = WdfDriverCreate(
                        DriverObject,
                        RegistryPath,
                        WDF_NO_OBJECT_ATTRIBUTES,
                        &driverConfig,
                        WDF_NO_HANDLE);

    //
    // Register the Telemetry provider
    //
    TraceLoggingRegister(
        g_TraceLoggingProvider);

    if (!NT_SUCCESS(status)) 
    {
        TraceEvents(
                TRACE_LEVEL_ERROR, 
                TRACE_FLAG_INIT, 
                "WdfDriverCreate failed "
                "status:%!STATUS!", 
                status);
        TraceLoggingWDFDevice(
            status, 
            DriverObject);
        
        //
        // If DriverCreate fails then the driver context cleanup
        // is NOT invoked, so we need to cleanup tracing/ETW here.
        // All other error paths post DriverCreate ensure that the 
        // cleanup callback is invoked
        //
        TraceLoggingUnregister(g_TraceLoggingProvider);
        EventUnregisterMicrosoft_Windows_SPB_HIDI2C();
        WPP_CLEANUP(DriverObject);

        goto exit;
    }

    TraceEvents(
            TRACE_LEVEL_INFORMATION, 
            TRACE_FLAG_INIT, 
            "Created WDF driver object:0x%p",
            DriverObject);
exit:
    return status;
}

NTSTATUS
OnDeviceAdd(
    _In_    WDFDRIVER       FxDriver,
    _Inout_ PWDFDEVICE_INIT FxDeviceInit
    )
/*++
Routine Description:

    OnDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager. We create and initialize a device object to
    represent a new instance of the device.

Arguments:

    FxDriver - Handle to a framework driver object created in DriverEntry

    FxDeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
{
    FuncEntry(TRACE_FLAG_INIT);

    UNREFERENCED_PARAMETER(FxDriver);
    
    NTSTATUS status;
    WDFDEVICE device;
    PDEVICE_CONTEXT pDeviceContext = NULL;
    
    //
    // This macro will assert if this routine runs at >= DISPATCH_LEVEL
    //
    PAGED_CODE();

    //
    // HIDCLASS is the PPO for HID stack so relinquish 
    // our power policy ownership
    //
    WdfDeviceInitSetPowerPolicyOwnership(FxDeviceInit, FALSE);

    //
    // Puristically: since mshidkmdf registers with HIDCLASS it is the FDO
    // and hidi2c is the Lower Filter. 
    // But this has some unwanted side effects with Interrupts and Power IRPs
    // So we will install mshidkmdf as a Upper Filter
    // and hidi2c as the Function driver and not call WdfFdoInitSetFilter
    // Also, we except this driver to be power-pageable, which is the default for
    // WDF function drivers.
    //

    //
    // PnP & Power callbacks
    //
    {
        WDF_PNPPOWER_EVENT_CALLBACKS pnpPowerCallbacks;
        WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);
        
        pnpPowerCallbacks.EvtDevicePrepareHardware              =   OnPrepareHardware;
        pnpPowerCallbacks.EvtDeviceReleaseHardware              =   OnReleaseHardware;
        pnpPowerCallbacks.EvtDeviceD0Entry                      =   OnD0Entry;
        pnpPowerCallbacks.EvtDeviceD0Exit                       =   OnD0Exit;
        pnpPowerCallbacks.EvtDeviceD0EntryPostInterruptsEnabled =   OnPostInterruptsEnabled;

        WdfDeviceInitSetPnpPowerEventCallbacks(FxDeviceInit, &pnpPowerCallbacks);
    }

    //
    // Create the framework device object. 
    //
    {
        WDF_OBJECT_ATTRIBUTES deviceAttributes;
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

        status = WdfDeviceCreate(&FxDeviceInit, &deviceAttributes, &device);

        if (!NT_SUCCESS(status))
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_INIT,
                "WdfDeviceCreate failed "
                "status:%!STATUS!",
                status);
            goto exit;
        }

        //
        // Store the device object in the device context
        //        
        pDeviceContext = GetDeviceContext(device);
        pDeviceContext->FxDevice = device;
    }

    //
    // Initialize Device Reset Notification members.
    //
    {
        WDF_OBJECT_ATTRIBUTES spinLockAttributes;
        WDF_OBJECT_ATTRIBUTES_INIT(&spinLockAttributes);

        spinLockAttributes.ParentObject = device;

        status = WdfSpinLockCreate(&spinLockAttributes, &pDeviceContext->DeviceResetNotificationSpinLock);
        
        if (!NT_SUCCESS(status))
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_INIT,
                "WdfSpinLockCreate failed "
                "status:%!STATUS!",
                status);
            goto exit;
        }

        pDeviceContext->DeviceResetNotificationRequest = NULL;
    }

    //
    // Allow the device to be disableable
    //
    {
        WDF_DEVICE_STATE deviceState;
        WDF_DEVICE_STATE_INIT(&deviceState);
        
        deviceState.NotDisableable = WdfFalse;
        WdfDeviceSetDeviceState(device, &deviceState);
    }

    //
    // Create queue objects
    //
    {
        //
        // Create a default parallel dispatch queue to handle internal
        // IOCTL requests from HID Class. This will be power-managed
        // since it will touch the device for data
        //
        WDF_IO_QUEUE_CONFIG queueConfig;
        WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchParallel);
        queueConfig.EvtIoInternalDeviceControl = OnInternalDeviceIoControl;
        queueConfig.EvtIoStop = OnIoStop;
        
        status = WdfIoQueueCreate(
                            device,
                            &queueConfig,
                            WDF_NO_OBJECT_ATTRIBUTES,
                            &pDeviceContext->IoctlQueue);

        if (!NT_SUCCESS (status)) 
        {
            TraceEvents(
                    TRACE_LEVEL_ERROR,
                    TRACE_FLAG_INIT,
                    "WdfIoQueueCreate failed  creating IoctlQueue "
                    "status:%!STATUS!",
                    status);
            goto exit;
        }
    
        //
        // Register a manual I/O queue for handling ReadReport Requests.
        // This queue will be used for storing Requests that need to wait for an
        // interrupt to occur before they can be completed. 
        // This queue is used for requests that dont directly access the device.
        // so we can use a non-power managed queue to park the requests 
        // since we dont care whether the device is idle
        // or fully powered up.
        //
        WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, WdfIoQueueDispatchManual);
        queueConfig.PowerManaged = WdfFalse;

        status = WdfIoQueueCreate(
                            device,
                            &queueConfig,
                            WDF_NO_OBJECT_ATTRIBUTES,
                            &pDeviceContext->ReportQueue);

        if (!NT_SUCCESS(status)) 
        {
            TraceEvents(
                    TRACE_LEVEL_ERROR,
                    TRACE_FLAG_INIT,
                    "WdfIoQueueCreate failed creating ReportQueue "
                    "status:%!STATUS!",
                    status);
            goto exit;
        }

        //
        // Register a manual queue to store requests that are pending
        // completion. This is basically to make sure that the passive-level
        // ISR isnt completing requests while holding an interrupt lock
        // TODO: Revisit this design if we are able to do the same at Dispatch
        // Level
        //
        WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, WdfIoQueueDispatchManual);
        queueConfig.PowerManaged = WdfFalse;

        status = WdfIoQueueCreate(
                            device,
                            &queueConfig,
                            WDF_NO_OBJECT_ATTRIBUTES,
                            &pDeviceContext->CompletionQueue);

        if (!NT_SUCCESS(status)) 
        {
            TraceEvents(
                    TRACE_LEVEL_ERROR,
                    TRACE_FLAG_INIT,
                    "WdfIoQueueCreate failed creating CompletionQueue "
                    "status:%!STATUS!",
                    status);
            goto exit;
        }

        //
        // Register a manual queue to store idle requests that need pending
        //
        WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, WdfIoQueueDispatchManual);
        queueConfig.PowerManaged = WdfFalse;

        status = WdfIoQueueCreate(
                            device,
                            &queueConfig,
                            WDF_NO_OBJECT_ATTRIBUTES,
                            &pDeviceContext->IdleQueue);

        if (!NT_SUCCESS(status)) 
        {
            TraceEvents(
                    TRACE_LEVEL_ERROR,
                    TRACE_FLAG_INIT,
                    "WdfIoQueueCreate failed creating IdleQueue "
                    "status:%!STATUS!",
                    status);
            goto exit;
        }
    }

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_FLAG_INIT,
        "Created device:0x%p status:%!STATUS!",
        device,
        status);

exit:
     if (!NT_SUCCESS(status)) 
     {
         TraceLoggingDeviceDriver(
             status,
             device);
     }
    FuncExit(TRACE_FLAG_INIT);
    return status;
}

VOID
OnDriverUnload(
    _In_ WDFDRIVER FxDriver
    )
/*++
Routine Description:

    Performs operations that must take place before the driver is unloaded.

Arguments:

    FxDriver - handle to a WDF driver object.

Return Value:

    VOID.

--*/
{
    //
    // This macro will assert if this routine runs at >= DISPATCH_LEVEL
    //
    PAGED_CODE();

    //
    // Unregister from ETW events
    //
    EventUnregisterMicrosoft_Windows_SPB_HIDI2C();

    //
    // Unregister from Tracelogging 
    //
    TraceLoggingUnregister(g_TraceLoggingProvider);

    //
    // Stop WPP Tracing
    //
    WPP_CLEANUP(WdfDriverWdmGetDriverObject(FxDriver));
}
