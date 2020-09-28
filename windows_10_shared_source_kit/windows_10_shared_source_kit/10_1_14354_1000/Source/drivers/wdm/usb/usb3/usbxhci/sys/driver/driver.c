/*++
Copyright (c) Microsoft Corporation

Module Name:

    driver.c

Abstract:

    USB XHCI Driver

--*/

#include "pch.h"

#include "driver.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, DriverEntry)
#pragma alloc_text(PAGE, DriverCleanup)
#endif

#if TEST_MODE

volatile BOOLEAN g_RANDOM_FAILURE_TEST_REDUCE_RINGSEGMENTS = FALSE;
volatile BOOLEAN g_RANDOM_FAILURE_TEST_CHANGE_COMMAND_TYPE = FALSE;
volatile BOOLEAN g_SKIP_LAST_TD_COMPLETION = FALSE;

ULONG                  g_RandomNumberSeed = 0;
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
Xhci_ShouldFail(
    ULONG FailPercent
    )
/*++


--*/
{
    ULONG value = Random() % 100;
    if (FailPercent <= value) {
        return FALSE;
    }
    return TRUE;
}

ULONG
Xhci_NonZeroRandomWithMax(
    ULONG   MaxRandom
    )
/*++

Generates a Random number upto a MaxRandom value

--*/
{
    ULONG value = Random();
    return ((value % MaxRandom)+1);
}
#endif

//
// This variable is used by the USB3 kernel debug extension.
//
PDRIVER_OBJECT  g_UsbXhciDriverObject;

PWDFDRIVER_USBXHCI_CONTEXT g_WdfDriverUsbXhciContext = NULL;

//
// Global WPP Triage info structure
//
WPP_TRIAGE_INFO g_UsbXhci_WppTriage_Info;

//
// USB Triage info structure
//
extern USBXHCI_TRIAGE_INFO g_UsbXhciTriageInfo;

NTSTATUS
DriverEntry(
    struct _DRIVER_OBJECT   *DriverObject,
    PUNICODE_STRING         RegistryPath
    )
/*++

Routine Description:

    DriverEntry is the first routine called after a driver is loaded,
    and is responsible for initializing the driver.

--*/
{
    SYSTEM_CODEINTEGRITY_INFORMATION    codeIntegrityInfo;
    UNICODE_STRING                      functionName;
    ULONG                               length;
    RECORDER_CONFIGURE_PARAMS           recorderConfigureParams;
    NTSTATUS                            status;
    WDFDRIVER                           wdfDriver;
    WDF_OBJECT_ATTRIBUTES               wdfAttributes;
    WDF_DRIVER_CONFIG                   wdfDriverConfig;

    PAGED_CODE();

    TRY {

        #if TEST_MODE
        {
            LARGE_INTEGER                 tickCount;
            KeQueryTickCount(&tickCount);
            g_RandomNumberSeed = (ULONG)tickCount.LowPart;
        }
        #endif 
        
        g_UsbXhciDriverObject = DriverObject;

        //
        // Initialize global to make NonPagedPool be treated as NxPool on Win8
        // and NonPagedPool on down-level
        //
        ExInitializeDriverRuntime(DrvRtPoolNxOptIn);

        //
        // Retrieve the system routine address for KseQueryDeviceFlags. This is done
        // to maintain backward compatibility with OSs that do not support this routine.
        //
        RtlInitUnicodeString(&functionName, L"KseQueryDeviceFlags");
        g_KseQueryDeviceFlags = (PFN_KSE_QUERY_DEVICE_FLAGS)MmGetSystemRoutineAddress(&functionName);

        //
        // Retrieve the system routine address for IoTryQueueWorkItem. This is done
        // to maintain backward compatibility with OSs that do not support this routine.
        //
        RtlInitUnicodeString(&functionName, L"IoTryQueueWorkItem");
        g_IoTryQueueWorkItem = (PFN_IO_TRY_QUEUE_WORKITEM)MmGetSystemRoutineAddress(&functionName);

        WPP_INIT_TRACING(DriverObject, RegistryPath);
        RECORDER_CONFIGURE_PARAMS_INIT(&recorderConfigureParams);
        recorderConfigureParams.CreateDefaultLog = FALSE;
        WppRecorderConfigure(&recorderConfigureParams);

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes,
                                                WDFDRIVER_USBXHCI_CONTEXT);

        wdfAttributes.EvtCleanupCallback = DriverCleanup;

        WDF_DRIVER_CONFIG_INIT(&wdfDriverConfig, &Controller_WdfEvtDeviceAdd);
        wdfDriverConfig.DriverPoolTag = DriverTag_Wdf;

        status = WdfDriverCreate(DriverObject,
                                 RegistryPath,
                                 &wdfAttributes,
                                 &wdfDriverConfig,
                                 &wdfDriver);

        if (!NT_SUCCESS(status)) {

            //
            // DriverCleanup will not be called since the WdfDriver object
            // failed to create. Clean-up any resources that were created
            // prior to WDF driver creation.
            //
            WPP_CLEANUP(DriverObject);
        }

        //
        // Initialize the global controller list.
        //
        g_WdfDriverUsbXhciContext = WdfDriverGetUsbXhciContext(wdfDriver);
        InitializeListHead(&g_WdfDriverUsbXhciContext->ControllerListHead);
        KeInitializeSpinLock(&g_WdfDriverUsbXhciContext->ControllerListLock);
        g_WdfDriverUsbXhciContext->ControllerListCount = 0;

        //
        // If TestSigning is on, consider the system to be in TestMode, in which case,
        // expose test hooks, counters etc.
        //
        RtlZeroMemory(&codeIntegrityInfo, sizeof(SYSTEM_CODEINTEGRITY_INFORMATION));
        codeIntegrityInfo.Length = sizeof(SYSTEM_CODEINTEGRITY_INFORMATION);

        status = ZwQuerySystemInformation(SystemCodeIntegrityInformation, 
                                          &codeIntegrityInfo, 
                                          sizeof(SYSTEM_CODEINTEGRITY_INFORMATION), 
                                          &length);

        if (NT_SUCCESS(status) &&
            (codeIntegrityInfo.CodeIntegrityOptions & CODEINTEGRITY_OPTION_TESTSIGN)) {

            g_WdfDriverUsbXhciContext->TestModeOn = TRUE;

        } else {

            status = STATUS_SUCCESS;
            g_WdfDriverUsbXhciContext->TestModeOn = FALSE;
        }

        //
        // Update the global WPP Triage info structure
        //
        WppRecorderGetTriageInfo(&g_UsbXhci_WppTriage_Info);

        //
        // Update the USB Triage info with WPP fields
        //
        g_UsbXhciTriageInfo.IfrLogHeaderSize = g_UsbXhci_WppTriage_Info.WppAutoLogHeaderSize;
        g_UsbXhciTriageInfo.IfrLogSizeOffset = g_UsbXhci_WppTriage_Info.WppAutoLogHeaderSizeOffset;
        g_UsbXhciTriageInfo.IfrSizeOfLogSizeField = g_UsbXhci_WppTriage_Info.WppSizeOfAutoLogHeaderSizeField;
        g_UsbXhciTriageInfo.IfrDriverContextOffset = g_UsbXhci_WppTriage_Info.WppDriverContextOffset;
        g_UsbXhciTriageInfo.IfrDriverContextSize = g_UsbXhci_WppTriage_Info.WppDriverContextSize;

        //
        // Update the global WDF Triage info structure
        //
        g_UsbXhciTriageInfo.WdfTriageInfoPtr = (PWDF_TRIAGE_INFO) WdfGetTriageInfo();

        //
        // Register the USBXHCI ETW event provider.
        //
        EventRegisterMicrosoft_Windows_USB_USBXHCI();

        //
        // Telemetry opt-in for Manifested ETW Providers.
        //
        EnableManifestedProviderForMicrosoftTelemetry(Microsoft_Windows_USB_USBXHCIHandle);

        Counter_Register();

    } FINALLY {

    }

    return status;
}

VOID
DriverCleanup(
    WDFOBJECT   WdfDriver
    )
/*++

Routine Description:

    The driver's EvtCleanupCallback event callback function removes the driver's
    references on an object so that the object can be deleted.

--*/
{
    PAGED_CODE();

    TRY {

        WPP_CLEANUP(WdfDriverWdmGetDriverObject((WDFDRIVER)WdfDriver));

        //
        // Unregister the USBXHCI ETW event provider.
        //
        EventUnregisterMicrosoft_Windows_USB_USBXHCI();

        Counter_Unregister();

    } FINALLY {

    }

    return;
}
