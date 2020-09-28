/*++
Copyright (c) Microsoft Corporation

Module Name:

    ucx.c

Abstract:

    USB Controller Extension Driver

--*/
#include "pch.h"

#include "ucx.tmh"

WDF_CLASS_LIBRARY_INFO WdfClassLibraryInfo =
{
    sizeof(WDF_CLASS_LIBRARY_INFO), // Size
    {
        UCX_MAJOR_VERSION_1,       // Major version
        UCX_MINOR_VERSION_CURRENT, // Minor version
        0,                         // Build
    },                               // Version
    Ucx_ClassInitialize,             // ClassLibraryInitialize
    Ucx_ClassDeinitialize,           // ClassLibraryDeinitialize
    Ucx_ClassBindClient,             // ClassLibraryBindClient
    Ucx_ClassUnbindClient,           // ClassLibraryUnbindClient
};

#define UCX_CLASS_EXTENSION_NAME L"\\Device\\UCX"
#define MAX_UCX_CLASS_EXTENSION_NAME_SIZE 100
#define MAX_OBJECT_NAME_COLLISSIONS 100

PWDFDRIVER_UCX_CONTEXT g_WdfDriverUcxContext = NULL;
ULONG                  g_RandomNumberSeed = 0;

PFN_IO_TRY_QUEUE_WORKITEM       g_IoTryQueueWorkItem = NULL;

NTSTATUS
DriverEntry(
    PDRIVER_OBJECT  DriverObject,
    PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    DriverEntry is the first routine called after a driver is loaded,
    and is responsible for initializing the driver.

Arguments:

    DriverObject - Caller-supplied pointer to a DRIVER_OBJECT structure.
                   This is the driver's driver object.

    RegistryPath - Pointer to a counted Unicode string specifying the path
                   to the driver's registry key.

Return Value:

    STATUS_SUCCESS if the function succeeded and failure status if it didn't.

--*/
{
    WDF_DRIVER_CONFIG             wdfDriverConfig;
    WDF_OBJECT_ATTRIBUTES         objectAttributes;
    WDFDRIVER                     wdfDriver;
    PWDFDEVICE_INIT               pWdfDeviceInit = NULL;
    NTSTATUS                      status;
    ULONG                         index;
    UNICODE_STRING                functionName;
    LARGE_INTEGER                 tickCount;
    NTSTATUS                      sleepStudyStatus;

    DECLARE_UNICODE_STRING_SIZE(uniName, MAX_UCX_CLASS_EXTENSION_NAME_SIZE);

    TRY {

        KeQueryTickCount(&tickCount);
        g_RandomNumberSeed = (ULONG)tickCount.LowPart;

        //
        // Initialize global to make NonPagedPool be treated as NxPool on Win8 
        // and NonPagedPool on down-level
        //
        ExInitializeDriverRuntime(DrvRtPoolNxOptIn);

        WPP_INIT_TRACING(DriverObject, RegistryPath);
        
        WDF_DRIVER_CONFIG_INIT(&wdfDriverConfig, NULL);
        wdfDriverConfig.DriverInitFlags = WdfDriverInitNonPnpDriver;
        wdfDriverConfig.EvtDriverUnload = Ucx_EvtDriverUnload;
        
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&objectAttributes,
                                                WDFDRIVER_UCX_CONTEXT);
        
        status = WdfDriverCreate(DriverObject,
                                 RegistryPath,
                                 &objectAttributes,
                                 &wdfDriverConfig,
                                 &wdfDriver);
        
        if (!NT_SUCCESS(status)) {            
            LogError(FlagUcx,"WdfDriverCreate Failed %!STATUS!", status);
            LEAVE;
        }
        
        g_WdfDriverUcxContext = WdfDriverGetUcxContext(wdfDriver);
        g_WdfDriverUcxContext->Sig = SigWdfDriverUcxContext;
        InitializeListHead(&g_WdfDriverUcxContext->UcxControllerListHead);
        
        KeInitializeSpinLock(&g_WdfDriverUcxContext->UcxControllerListLock);
        g_WdfDriverUcxContext->UcxControllerListCount = 0;

        pWdfDeviceInit = WdfControlDeviceInitAllocate(WdfGetDriver(), &SDDL_DEVOBJ_KERNEL_ONLY);
        
        if (pWdfDeviceInit == NULL) {
            
            LogError(FlagUcx,"WdfControlDeviceInitAllocate returned NULL");
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }
        
        index = 0;
        
        do {
            
            status = RtlUnicodeStringPrintf(&uniName, L"%s%d", UCX_CLASS_EXTENSION_NAME, index++);
            
            if (!NT_SUCCESS(status)) {
                LogError(FlagUcx,"RtlUnicodeStringPrintf Failed %!STATUS!", status);
                break;
            }
            
            status = WdfDeviceInitAssignName(pWdfDeviceInit, &uniName);
            
            if (!NT_SUCCESS(status)) {
                LogError(FlagUcx,"WdfDeviceInitAssignName Failed %!STATUS!", status);
                break;
            }

            status = WdfDeviceCreate(&pWdfDeviceInit, NULL, &g_WdfDriverUcxContext->UcxWdfDevice);

        } while ((status == STATUS_OBJECT_NAME_COLLISION) && (index < MAX_OBJECT_NAME_COLLISSIONS));
        
        if (!NT_SUCCESS(status)) {
            
            LogError(FlagUcx,"WdfDeviceCreate Failed %!STATUS!", status);            
            LEAVE;            
        } 
        
        WdfControlFinishInitializing(g_WdfDriverUcxContext->UcxWdfDevice);
        
        status = WdfRegisterClassLibrary(&WdfClassLibraryInfo,
                                         RegistryPath,
                                         &uniName);
        
        if (!NT_SUCCESS(status)) {
            
            LogError(FlagUcx,"WdfRegisterClassLibrary Failed %!STATUS!", status);
            LEAVE;
        }
        
        //
        // Import kernel functions that are only supported in Windows 8 and later.
        //
        
        RtlInitUnicodeString(&functionName, L"IoTryQueueWorkItem");
        g_IoTryQueueWorkItem = (PFN_IO_TRY_QUEUE_WORKITEM)(ULONG_PTR)MmGetSystemRoutineAddress(&functionName);

        RtlInitUnicodeString(&functionName, L"IoGetActivityIdIrp");
        g_IoGetActivityIdIrp = (PFN_IO_GET_ACTIVITY_ID_IRP)(ULONG_PTR)MmGetSystemRoutineAddress(&functionName);

        RtlInitUnicodeString(&functionName, L"IoSetActivityIdIrp");
        g_IoSetActivityIdIrp = (PFN_IO_SET_ACTIVITY_ID_IRP)(ULONG_PTR)MmGetSystemRoutineAddress(&functionName);

        RtlInitUnicodeString(&functionName, L"IoFreeActivityIdIrp");
        g_IoFreeActivityIdIrp = (PFN_IO_FREE_ACTIVITY_ID_IRP)(ULONG_PTR)MmGetSystemRoutineAddress(&functionName);

        
        status = Registry_InitializeAllow64KLowOrFullSpeedControlTransfersFlag();
        
        if (NT_SUCCESS(status)) {
            
            if (Registry_QueryAllow64KLowOrFullSpeedControlTransfersFlag()) {
                
                LogInfo(FlagUcx, "Allow64KLowOrFullSpeedControlTransfersFlag is set in registry");
                
            } else {
                
                LogInfo(FlagUcx, "Allow64KLowOrFullSpeedControlTransfersFlag is cleared in registry");
            }
            
        } else {
            
            if (status != STATUS_OBJECT_NAME_NOT_FOUND) {
                
                LogError(FlagUcx, "Query for Allow64KLowOrFullSpeedControlTransfersFlag failed with %!STATUS!", status);
            }
            
            //
            // Treat this error as non-fatal and continue driver load.
            //
            
            status = STATUS_SUCCESS;
        }
        
        //
        // Register the UCX ETW event provider.
        //
        EventRegisterMicrosoft_Windows_USB_UCX();
        sleepStudyStatus = UsbSleepStudy_Initialize();

        if (NT_SUCCESS(sleepStudyStatus)) {
            g_WdfDriverUcxContext->SleepStudyEnabled = TRUE;
        }
        else {
            g_WdfDriverUcxContext->SleepStudyEnabled = FALSE;
            LogError(FlagUcx, "UsbSleepStudy_Initialize Failed: %!STATUS!", sleepStudyStatus);
        }

        //
        // Telemetry opt-in for Manifested ETW Providers.
        //
        EnableManifestedProviderForMicrosoftTelemetry(Microsoft_Windows_USB_UCXHandle);

        LogInfo(FlagUcx, "Driver Entry %!STATUS!", status);
    
    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (pWdfDeviceInit != NULL) {
                WdfDeviceInitFree(pWdfDeviceInit);
                pWdfDeviceInit = NULL;
            }

            if (g_WdfDriverUcxContext != NULL && 
                g_WdfDriverUcxContext->UcxWdfDevice != NULL) {
                
                WdfObjectDelete(g_WdfDriverUcxContext->UcxWdfDevice);
                g_WdfDriverUcxContext->UcxWdfDevice = NULL;
            }

            WPP_CLEANUP(DriverObject);
        }     
    }

    return status;
}

VOID
Ucx_EvtDriverUnload(
    WDFDRIVER   WdfDriver
    )
/*++

Routine Description:

    The driver's EvtDriverUnload event callback function.

Arguments:

    WdfDriver - A handle to a framework object.

Return Value:

    None

Comments:
    This deletes a WDFDEVICE object that was in DriverEntry to prevent the 
    driver from getting unloaded. 
    
--*/
{
    UNREFERENCED_PARAMETER(WdfDriver);

    TRY {
        
        LogVerbose(FlagUcx, "WdfDriver Unloading 0x%p\n", WdfDriver);

        if (g_WdfDriverUcxContext->UcxWdfDevice != NULL) {

            WdfObjectDelete(g_WdfDriverUcxContext->UcxWdfDevice);
            g_WdfDriverUcxContext->UcxWdfDevice = NULL;
        }

        WPP_CLEANUP(WdfDriverWdmGetDriverObject((WDFDRIVER)WdfDriver));

        if (g_WdfDriverUcxContext->SleepStudyEnabled) {
            UsbSleepStudy_Uninitialize();
        }

        //
        // Unregister the UCX ETW event provider.
        //
        EventUnregisterMicrosoft_Windows_USB_UCX();

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
EXPORT(UcxInitializeDeviceInit)(
    __in_opt
        PUCX_GLOBALS                    Globals,
    __inout
        PWDFDEVICE_INIT                 DeviceInit
)
/*++

Routine Description:

    This function is called by HCD before it creates the WdfDevice object.
    In this routine UCX sets preprocessor routines.

Arguments:

    DeviceInit - A handle to a framework device init object.  UCX uses this
    handle to allocate a class extension device init object

Return Value:

    NTSTATUS

--*/
{
    UNREFERENCED_PARAMETER(Globals);
    UNREFERENCED_PARAMETER(DeviceInit);

    //
    // This function currently does nothing, but exists so in the future
    // WdfCxDeviceInit functions requiring the WDFDEVICE_INIT of the HCD
    // (such as preprocessor routines) may be called without incrementing
    // the minor version.
    //

    return STATUS_SUCCESS;
}

// NOTE: VOID is needed in the function declaration since PFN_WDF_CLASS_LIBRARY_INITIALIZE
//  has a VOID in it. Not putting VOID in the prototype of this routine, causes a compiler error.
_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Ucx_ClassInitialize(VOID)
/*++

Routine Description:

    A Dummy Class Initialize function that Wdf Calls. 

Return Value:

    STATUS_SUCCESS since this is a dummy function.

--*/
{
    LogVerbose(FlagUcx,"UcxClassInitialize");
    return STATUS_SUCCESS;
}

// NOTE: VOID is needed in the function declaration since PFN_WDF_CLASS_LIBRARY_DEINITIALIZE
//  has a VOID in it. Not putting VOID in the prototype of this routine, causes a compiler error.
__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Ucx_ClassDeinitialize(VOID)
/*++

Routine Description:

    A Dummy Class Deinitialize function that Wdf Calls. 

Return Value:

    VOID

--*/
{
    LogVerbose(FlagUcx,"UcxClassDeinitialize");
    return;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Ucx_ClassBindClient(
    __in
        PWDF_CLASS_BIND_INFO    WdfClassBindInfo,
    __in
        PWDF_COMPONENT_GLOBALS  WdfComponentGlobals
    )
/*++

Routine Description:

    A bind call that is called by Wdf to bind a controller driver to this 
    controller extension.
    
Arguments:

    WdfClassBindInfo - A pointer to the WDF_CLASS_BIND_INFO structure that has
        version information, class name. This function fills all the controller
        extension function pointers in this structure.
        
    WdfComponentGlobals - Unused parameter

Return Value:

    NT_SUCCESS status if the operation succeeds.

--*/
{
    NTSTATUS                   status;
    PUCX_GLOBALS               ucxGlobals;

    static const PFN_UCX_CLASS exports[] =
    {
        (PFN_UCX_CLASS) EXPORT(UcxIoDeviceControl),
        (PFN_UCX_CLASS) EXPORT(UcxControllerCreate),
        (PFN_UCX_CLASS) EXPORT(UcxControllerNeedsReset),
        (PFN_UCX_CLASS) EXPORT(UcxControllerResetComplete),
        (PFN_UCX_CLASS) EXPORT(UcxControllerSetFailed),
        (PFN_UCX_CLASS) EXPORT(UcxRootHubCreate),
        (PFN_UCX_CLASS) EXPORT(UcxRootHubPortChanged),
        (PFN_UCX_CLASS) EXPORT(UcxUsbDeviceCreate),
        (PFN_UCX_CLASS) EXPORT(UcxUsbDeviceInitSetEventCallbacks),
        (PFN_UCX_CLASS) EXPORT(UcxUsbDeviceRemoteWakeNotification),
        (PFN_UCX_CLASS) EXPORT(UcxEndpointCreate),
        (PFN_UCX_CLASS) EXPORT(UcxEndpointGetStaticStreamsReferenced),
        (PFN_UCX_CLASS) EXPORT(UcxEndpointNeedToCancelTransfers),
        (PFN_UCX_CLASS) EXPORT(UcxEndpointInitSetEventCallbacks),
        (PFN_UCX_CLASS) EXPORT(UcxDefaultEndpointInitSetEventCallbacks),
        (PFN_UCX_CLASS) EXPORT(UcxEndpointSetWdfIoQueue),
        (PFN_UCX_CLASS) EXPORT(UcxEndpointPurgeComplete),
        (PFN_UCX_CLASS) EXPORT(UcxEndpointAbortComplete),
        (PFN_UCX_CLASS) EXPORT(UcxEndpointNoPingResponseError),
        (PFN_UCX_CLASS) EXPORT(UcxStaticStreamsSetStreamInfo),
        (PFN_UCX_CLASS) EXPORT(UcxStaticStreamsCreate),
        (PFN_UCX_CLASS) EXPORT(UcxInitializeDeviceInit),
    };

    UNREFERENCED_PARAMETER(WdfComponentGlobals);

    TRY {
        #if DBG
        VERIFY_EXPORT_FUNC(exports, UcxIoDeviceControl);
        VERIFY_EXPORT_FUNC(exports, UcxControllerCreate);
        VERIFY_EXPORT_FUNC(exports, UcxControllerNeedsReset);
        VERIFY_EXPORT_FUNC(exports, UcxControllerResetComplete);
        VERIFY_EXPORT_FUNC(exports, UcxControllerSetFailed);
        VERIFY_EXPORT_FUNC(exports, UcxRootHubCreate);
        VERIFY_EXPORT_FUNC(exports, UcxRootHubPortChanged);
        VERIFY_EXPORT_FUNC(exports, UcxUsbDeviceCreate);
        VERIFY_EXPORT_FUNC(exports, UcxUsbDeviceInitSetEventCallbacks);
        VERIFY_EXPORT_FUNC(exports, UcxUsbDeviceRemoteWakeNotification);
        VERIFY_EXPORT_FUNC(exports, UcxEndpointCreate);
        VERIFY_EXPORT_FUNC(exports, UcxEndpointGetStaticStreamsReferenced);
        VERIFY_EXPORT_FUNC(exports, UcxEndpointInitSetEventCallbacks);
        VERIFY_EXPORT_FUNC(exports, UcxDefaultEndpointInitSetEventCallbacks);
        VERIFY_EXPORT_FUNC(exports, UcxEndpointSetWdfIoQueue);
        VERIFY_EXPORT_FUNC(exports, UcxEndpointPurgeComplete);
        VERIFY_EXPORT_FUNC(exports, UcxEndpointAbortComplete);
        VERIFY_EXPORT_FUNC(exports, UcxEndpointNoPingResponseError);
        VERIFY_EXPORT_FUNC(exports, UcxStaticStreamsSetStreamInfo);
        VERIFY_EXPORT_FUNC(exports, UcxStaticStreamsCreate);
        VERIFY_EXPORT_FUNC(exports, UcxInitializeDeviceInit);
        #endif

        LogVerbose(FlagUcx, "UcxClassBindClient");

        //
        // First thing to validate is that ClassBindInfo is not NULL. Kmdf calls Unbind even if
        // bind fails. Thus we need to set the *(PUCX_GLOBALS*)WdfClassBindInfo->ClassBindInfo) to
        // NULL as soon as possible in this function.
        //
        if (WdfClassBindInfo->ClassBindInfo == NULL) {
            LogError(FlagUcx, "WdfClassBindInfo->ClassBind Info should not be NULL");
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }
        (*(PUCX_GLOBALS*)WdfClassBindInfo->ClassBindInfo) = NULL;

        //
        // Require exact version match between us (UCX) and the version of UCX that the client
        // driver was built against.
        //
        if (WdfClassBindInfo->Version.Major != WdfClassLibraryInfo.Version.Major ||
            WdfClassBindInfo->Version.Minor != WdfClassLibraryInfo.Version.Minor) {

            LogError(FlagUcx, "Version mismatch: UCX %d.%d, Client built against UCX %d.%d",
                     WdfClassLibraryInfo.Version.Major, WdfClassLibraryInfo.Version.Minor,
                     WdfClassBindInfo->Version.Major, WdfClassBindInfo->Version.Minor);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        if (WdfClassBindInfo->FunctionTableCount != UcxFunctionsMax) {

            LogError(FlagUcx, "WdfClassBindInfo->FunctionTableCount != UcxFunctionsMax%d", UcxFunctionsMax);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        RtlCopyMemory(WdfClassBindInfo->FunctionTable, &exports[0], sizeof(exports));

        ucxGlobals = (PUCX_GLOBALS) ExAllocatePoolWithTag(NonPagedPool,
                                                          sizeof(UCX_GLOBALS),
                                                          UCX_TAG);


        if (ucxGlobals == NULL) {
            LogError(FlagUcx, "Could not allocate UcxGlobals");
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        ucxGlobals->Size = sizeof(UCX_GLOBALS);

        //
        // checked with the KMDF team that it is ok to leverage WdfComponentGlobals and cast it
        // to WdfDriverGlobals. 
        //
        ucxGlobals->WdfDriverGlobals = (PWDF_DRIVER_GLOBALS)WdfComponentGlobals;

        (*(PUCX_GLOBALS*)WdfClassBindInfo->ClassBindInfo) = ucxGlobals;

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Ucx_ClassUnbindClient(
    __in
        PWDF_CLASS_BIND_INFO    WdfClassBindInfo,
    __in
        PWDF_COMPONENT_GLOBALS  WdfComponentGlobals
    )
/*++

Routine Description:

    A unbind call that is called by Wdf to unbind a controller driver from this 
    controller extension.
    
Arguments:

    WdfClassBindInfo - A pointer to the WDF_CLASS_BIND_INFO structure
        
    WdfComponentGlobals - Unused parameter

Comments:
    At this point this function does nothing.
    
--*/
{
    UNREFERENCED_PARAMETER(WdfComponentGlobals);

    TRY {
        
        if (WdfClassBindInfo->ClassBindInfo != NULL &&
            (*(PUCX_GLOBALS*)WdfClassBindInfo->ClassBindInfo) != NULL) {

            ExFreePoolWithTag(*(PUCX_GLOBALS*)WdfClassBindInfo->ClassBindInfo, UCX_TAG);
            (*(PUCX_GLOBALS*)WdfClassBindInfo->ClassBindInfo) = NULL;
        
        }
    
    } FINALLY {

    }

    LogVerbose(FlagUcx, "UcxClassUnbindClient");

    return;
}

#define MAX_RANDOM 32768
ULONG
Random()
/*++

Routine Description:

    Simple random number generator to use at DISPATCH_LEVEL
    because the system provided function RtlRandomEx
    can be called at only passive-level.

    Simple random-number formula is taken from the book
    "The C Programming Language," by Kernighan and Ritchie:

    This formula assumes the existence of a variable called
    random_seed, which is initially set to some number.
    The random_seed variable is multiplied by 1,103,515,245
    and then 12,345 gets added to the product; random_seed
    is then replaced by this new value. This is actually a
    pretty good pseudo-random number generator.
    It has a good distribution and it is non-repeating.

Return Value:

   NT status code

--*/
{
    g_RandomNumberSeed = g_RandomNumberSeed * 1103515245 + 12345;
    return (ULONG)(g_RandomNumberSeed / 65536) % 32768;
}


NTSTATUS
Ucx_GetRandomErrorNTSTATUS()
{
    NTSTATUS status = 0xC0000000;
    status |= Random();
    return status;
}

ULONG
Ucx_NonZeroRandomWithMax(
    ULONG   MaxRandom
    )
/*++

Generates a Random number upto a MaxRandom value

--*/
{
    ULONG value = Random();
    return ((value % MaxRandom)+1);
}

BOOLEAN
Ucx_FailDueToVerifierSetting(
    ULONG   VerifierSetting
    )
/*++
Routine Description:
    
    Takes a VerifierSetting and decides whether the corresponding operation should be failed
    
    VerifierSetting         Meaning
    0                       Dont Fail
    1                       Always Fail
    N                       Fail with a probability of 1/N
    
Return Value:
    Returns True if operation should be failed, false otherwise       
--*/    
{
    BOOLEAN shouldFail;
    ULONG   randomNumber;
    ULONG   cutoff;

    TRY  {
        switch (VerifierSetting) {
        case 0:
            shouldFail = FALSE;
            LEAVE;
        case 1:
            shouldFail = TRUE;
            LEAVE;
        default:
            randomNumber = Random();
            cutoff =  MAX_RANDOM/VerifierSetting;
            if (cutoff > randomNumber) {
                shouldFail = TRUE;
            } else {
                shouldFail = FALSE;
            }
        }

    } FINALLY {

    }
    return shouldFail;
}

VOID
Ucx_DbgBreak(
    __in
        PCSTR                       ReasonForBreak
    )
/*++

Routine Description:

    This function checks if there is a debugger connected
    and if yes, breaks in the debugger, even on the fre
    build

Returns:

--*/
{
   TRY {
       
       LogError(FlagUcx, "%s", ReasonForBreak);

       if (KdRefreshDebuggerNotPresent() == FALSE) {

           DbgPrint("Breaking due to: %s\n", ReasonForBreak);

           DbgBreakPoint();
       }

   } FINALLY {

   }


}

