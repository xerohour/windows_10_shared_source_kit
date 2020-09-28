/*++

Copyright (c) 2004  Microsoft Corporation

Module Name:

    service.c

Abstract:

    DLL Services provided by usbport.sys

    This module conatins the load and initialization code
    used by the port driver to link up with the miniport.

Environment:

    kernel mode only

Notes:

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs

--*/

#define USBPORT

#include "common.h"
#include <process.h>
#include <MicrosoftTelemetry.h>

#ifdef USBPORT_WPP
#include "service.tmh"
#endif


extern LIST_ENTRY USBPORT_USB1fdoList;
extern LIST_ENTRY USBPORT_USB2fdoList;
extern LIST_ENTRY USBPORT_MiniportDriverList;
extern KSPIN_LOCK USBPORT_GlobalsSpinLock;

extern ULONG WppInitialized = 0;

REGHANDLE   EtwRegHandle = (REGHANDLE)NULL;
volatile ULONG   EtwLoggingEnabled = 0;
volatile ULONG   EtwPerfLoggingEnabled = 0;

PFN_IO_GET_ACTIVITY_ID_IRP      g_IoGetActivityIdIrp = NULL;
BOOLEAN                         g_IsInterruptPerTdEnabled = TRUE;
PVOID                           g_IsInterruptPerTdEnabledCBHandle = NULL;
EXT_CALLBACK                    USBPORT_AsyncTimerDpcHighRes;


//
// Disable inlining for USBPORT_Initialize() to ensure that the compiler
// doesn't add a security cookie to DllInitialize as a result of
// inlining USBPORT_Initialize().
//

DECLSPEC_NOINLINE
NTSTATUS
USBPORT_Initialize(
    IN PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    extern ULONG USBD_Version;
    NTSTATUS status = STATUS_SUCCESS;
    REGHANDLE regHandle;
    UNICODE_STRING functionName;

    // Initialize global to make NonPagedPool be treated as NxPool on Win8
    // and NonPagedPool on down-level
    //
    ExInitializeDriverRuntime(DrvRtPoolNxOptIn);

    GET_GLOBAL_DEBUG_PARAMETERS();

    USBD_Version = USBDI_VERSION;

    InitializeListHead(&USBPORT_USB1fdoList);
    InitializeListHead(&USBPORT_USB2fdoList);
    InitializeListHead(&USBPORT_MiniportDriverList);

    KeInitializeSpinLock(&USBPORT_GlobalsSpinLock);

    //
    // Pre-initialize emergency worker thread
    //
    USBPORT_PreInitializeEmergencyWorkerThread();

    MPx_InitializeHsb();

    USBPORT_InitAllBugChecks();
    USBPORT_InitLookasideLists();

    //
    // Import kernel functions that are only supported in Windows 8 and later.
    //
    RtlInitUnicodeString(&functionName, L"IoGetActivityIdIrp");
    g_IoGetActivityIdIrp = (PFN_IO_GET_ACTIVITY_ID_IRP)(ULONG_PTR)MmGetSystemRoutineAddress(&functionName);

    USBPORT_InitializeHighResTimer();

    //
    // Register with ETW
    //
    status = EtwRegister( &USBPORT_ETW_PROVIDER,
                          USBPORTSVC_EtwEnableCallback,
                          NULL,
                          &regHandle );

    if (NT_SUCCESS(status)) {
        EtwRegHandle = regHandle;

        //
        // Telemetry opt-in for Manifested ETW Providers.
        //
        EnableManifestedProviderForMicrosoftTelemetry(regHandle);
    }

    return status;
}

ULARGE_INTEGER
USBPort_GetTimeStampInMicroSeconds()
/*++

Routine Description:

    This routine returns the time stamp in Micro seconds

Return Value:

    Time stamp in Micro seconds

--*/
{
    LARGE_INTEGER performanceCounter;
    LARGE_INTEGER performanceFrequency;

    ULARGE_INTEGER microSeconds;
    ULARGE_INTEGER tempHi;
    ULARGE_INTEGER tempLow;

    performanceCounter = KeQueryPerformanceCounter(&performanceFrequency);

    if (performanceFrequency.QuadPart == 0) {
        //
        // This should not never happen. Let's bail out instead of causing a divide by zero
        //
        microSeconds.QuadPart = (ULONGLONG) performanceCounter.QuadPart;
        return microSeconds;
    }

    //
    // microSeconds = (performanceCounter / performanceFrequency) * 1000000
    //
    // However, since we are dealing with large integers diving by frequency first can result in
    // precision loss (remainder is lost).
    //
    // Instead to preserve precision, we do:
    //
    // microSeconds = (performanceCounter * 1000000) /performanceFrequency
    //
    // Again, since we are dealing with large integers we can overflow in the multiplication step
    // To avoid this we split the multiplication to 32bit HighPart and 32bit low
    // part and accumulate the result of the multiplication in 64 bit values (to prevent overflow)
    //

    //
    // Multiply higher order bits first
    //
    tempHi.QuadPart = (ULONG) performanceCounter.HighPart;
    tempHi.QuadPart *= 1000000; // Microseconds per second

    //
    // Divide by frequency and shift to higher order bits
    //
    microSeconds.QuadPart = (tempHi.QuadPart / (ULONGLONG) performanceFrequency.QuadPart) << 32;

    //
    // Get the remainder of the higher order bit division
    //
    tempHi.QuadPart %= (ULONGLONG) performanceFrequency.QuadPart;
    tempHi.QuadPart <<= 32;

    //
    // Multiply the lower order bits
    //
    tempLow.QuadPart = performanceCounter.LowPart;
    tempLow.QuadPart *= 1000000; // Microseconds per second

    //
    // The remainder of higher order operation should be added to result
    // of lower order bits
    //
    tempLow.QuadPart += tempHi.QuadPart;
    tempLow.QuadPart /= (ULONGLONG) performanceFrequency.QuadPart;

    //
    // Add result of division of lowe order bits
    //
    microSeconds.QuadPart += tempLow.QuadPart;

    return microSeconds;
}

NTSTATUS
USBPORT_PowerSettingCallback (
    __in LPCGUID SettingGuid,
    __in_bcount(ValueLength) PVOID Value,
    __in ULONG ValueLength,
    __inout_opt PVOID Context
    )
/*++

Routine Description:

    Power manager callback function to notify about requested settings changes.

Return Value:

    STATUS_SUCCESS.

--*/
{
    UNREFERENCED_PARAMETER(Context);

    if (IsEqualGUID(SettingGuid, &GUID_POWER_INTERRUPT_PER_TD_ENABLE) &&
        Value != NULL &&
        ValueLength == sizeof(ULONG)) {
        g_IsInterruptPerTdEnabled = (*((PULONG)Value) != 0);
    }

    return STATUS_SUCCESS;
}


BOOLEAN
USBPORTSVC_IsInterruptPerTdEnabled(
    __in PDEVICE_DATA DeviceData
    )
{
    PDEVICE_EXTENSION devExt = NULL;

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);

    if (devExt->Fdo.Flags.InterruptPerTd == 1) {
        return TRUE;
    } 

    return g_IsInterruptPerTdEnabled;
}


NTSTATUS
DllInitialize(
    IN PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    __security_init_cookie();

    //
    // Failing to register the power setting callback is not fatal
    //
    PoRegisterPowerSettingCallback(NULL, // optional device object
                                   &GUID_POWER_INTERRUPT_PER_TD_ENABLE,
                                   USBPORT_PowerSettingCallback,
                                   NULL, // context
                                   &g_IsInterruptPerTdEnabledCBHandle);

    return USBPORT_Initialize(RegistryPath);
}


NTSTATUS
DllUnload(
    VOID
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    USBPORT_FreeLookasideLists();
    USBPORT_FreeAllBugChecks();

    if (g_IsInterruptPerTdEnabledCBHandle != NULL) {
        PoUnregisterPowerSettingCallback(g_IsInterruptPerTdEnabledCBHandle);
    }

    if (InterlockedExchange(&WppInitialized, 0)){
        WPP_CLEANUP(NULL);
    }

    //
    //  Unregister the driver as an ETW provider
    //
    if (EtwRegHandle != (REGHANDLE)NULL) {
        EtwUnregister(EtwRegHandle);
        EtwRegHandle = (REGHANDLE)NULL;
    }

    return STATUS_SUCCESS;
}


ULONG
USBPORT_GetHciMn(
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    return USB_HCI_MN;
}


PDEVICE_OBJECT
USBPORT_FindUSB2Controller(
    PDEVICE_OBJECT CcFdoDeviceObject
    )
/*++

Routine Description:

    Given a companion controller find the FDO for the parent.  This function takes a
    refernce on the returned object since the routines that call it are fpr a different
    instance of the USB stack -- we don't want the devobj to unload.

Arguments:

Return Value:

--*/
{
    KIRQL irql;
    PLIST_ENTRY listEntry = NULL;
    PDEVICE_OBJECT deviceObject = NULL;
    PDEVICE_EXTENSION ccExt = NULL;
    PDEVICE_EXTENSION devExt = NULL;

    GET_DEVICE_EXT(ccExt, CcFdoDeviceObject);
    ASSERT_FDOEXT(ccExt);

    KeAcquireSpinLock(&USBPORT_GlobalsSpinLock, &irql);

    GET_HEAD_LIST(USBPORT_USB2fdoList, listEntry);

    while (listEntry &&
           listEntry != &USBPORT_USB2fdoList) {

        devExt = (PDEVICE_EXTENSION) CONTAINING_RECORD(
                    listEntry,
                    struct _DEVICE_EXTENSION,
                    Fdo.ControllerLink);

        // find the USB 2 controller assocaited with this CC

        if (devExt->Fdo.BusNumber == ccExt->Fdo.BusNumber &&
            devExt->Fdo.BusDevice == ccExt->Fdo.BusDevice) {
            deviceObject = devExt->HcFdoDeviceObject;
            break;
        }

        listEntry = devExt->Fdo.ControllerLink.Flink;
    }

    if (deviceObject) {
        ObReferenceObject(deviceObject);
    }

    KeReleaseSpinLock(&USBPORT_GlobalsSpinLock, irql);

    return deviceObject;
}


BOOLEAN
USBPORT_IsCCForFdo(
    PDEVICE_OBJECT Usb2FdoDeviceObject,
    PDEVICE_EXTENSION CcExt
    )
/*++

--*/
{
    PDEVICE_EXTENSION usb2Ext = NULL;

    GET_DEVICE_EXT(usb2Ext, Usb2FdoDeviceObject);
    ASSERT_FDOEXT(usb2Ext);

    ASSERT_FDOEXT(CcExt);

    if (usb2Ext->Fdo.BusNumber == CcExt->Fdo.BusNumber &&
        usb2Ext->Fdo.BusDevice == CcExt->Fdo.BusDevice) {
        return TRUE;
    }

    return FALSE;
}


PDEVICE_RELATIONS
USBPORT_FindCompanionControllers(
    PDEVICE_OBJECT Usb2FdoDeviceObject,
    BOOLEAN ReferenceObjects,
    BOOLEAN ReturnFdo
    )
/*++

Routine Description:

    Given a a USB2 Fdo return the CCs in a device-relations style structure.  The objects returbed
    are the ones that are referenced.

Arguments:

    ReturnFdo - if True Hc FDO is returned otherwise PDO is returned.

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;
    PLIST_ENTRY listEntry;
    KIRQL irql;
    PDEVICE_RELATIONS deviceRelations = NULL;
    ULONG count = 0;

    KeAcquireSpinLock(&USBPORT_GlobalsSpinLock, &irql);

    GET_HEAD_LIST(USBPORT_USB1fdoList, listEntry);

    while (listEntry &&
           listEntry != &USBPORT_USB1fdoList) {

        devExt = (PDEVICE_EXTENSION) CONTAINING_RECORD(
                    listEntry,
                    struct _DEVICE_EXTENSION,
                    Fdo.ControllerLink);

        if (devExt->Fdo.Flags.IsCompanionController &&
            USBPORT_IsCCForFdo(Usb2FdoDeviceObject, devExt)) {
            count++;
        }

        listEntry = devExt->Fdo.ControllerLink.Flink;

    } /* while */

    LOGENTRY(Usb2FdoDeviceObject, LOG_MISC, 'fccR', count, 0,
        Usb2FdoDeviceObject);

    if (count) {
        ALLOC_POOL_OSOWNED(&deviceRelations,
                           NonPagedPoolNx,
                           sizeof(*deviceRelations)*count);
    }

    if (deviceRelations != NULL) {
        deviceRelations->Count = count;
        count = 0;

        GET_HEAD_LIST(USBPORT_USB1fdoList, listEntry);

        while (listEntry &&
               listEntry != &USBPORT_USB1fdoList) {

            devExt = (PDEVICE_EXTENSION) CONTAINING_RECORD(
                        listEntry,
                        struct _DEVICE_EXTENSION,
                        Fdo.ControllerLink);

            if (devExt->Fdo.Flags.IsCompanionController &&
                USBPORT_IsCCForFdo(Usb2FdoDeviceObject, devExt)) {

                USBPORT_ASSERT(count < deviceRelations->Count);

                if (ReturnFdo) {
                    deviceRelations->Objects[count] = devExt->HcFdoDeviceObject;
                } else {
                    deviceRelations->Objects[count] = devExt->Fdo.PhysicalDeviceObject;
                }

                // ref the objects that we return
                if (ReferenceObjects) {
                    ObReferenceObject(deviceRelations->Objects[count]);
                }

                DbgTrace((UsbpDebugTrace, "'Found CC %p\n", deviceRelations->Objects[count]));

                count++;
            }

            listEntry = devExt->Fdo.ControllerLink.Flink;

        } /* while */
    }
    KeReleaseSpinLock(&USBPORT_GlobalsSpinLock, irql);

    return deviceRelations;
}


VOID
USBPORT_RegisterUSB2fdo(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    tracks an instance of a USB 2 device

Arguments:

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt = NULL;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);
    devExt->Fdo.Flags.FdoRegistered = 1;

    ExInterlockedInsertTailList(&USBPORT_USB2fdoList,
                                &devExt->Fdo.ControllerLink,
                                &USBPORT_GlobalsSpinLock);
}


VOID
USBPORT_RegisterUSB1fdo(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    tracks an instance of a USB 2 device

Arguments:

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt = NULL;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);
    devExt->Fdo.Flags.FdoRegistered = 1;

    ExInterlockedInsertTailList(&USBPORT_USB1fdoList,
                                &devExt->Fdo.ControllerLink,
                                &USBPORT_GlobalsSpinLock);
}


VOID
USBPORT_DeregisterUSB2fdo(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    tracks an instance of a USB 2 device

Arguments:

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(devExt->Fdo.ControllerLink.Flink != NULL);
    USBPORT_ASSERT(devExt->Fdo.ControllerLink.Blink != NULL);

    USBPORT_InterlockedRemoveEntryList(&devExt->Fdo.ControllerLink,
                                       &USBPORT_GlobalsSpinLock);

    devExt->Fdo.Flags.FdoRegistered = 0;

    devExt->Fdo.ControllerLink.Blink = NULL;
    devExt->Fdo.ControllerLink.Flink = NULL;

}


VOID
USBPORT_DeregisterUSB1fdo(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    tracks an instance of a USB 2 device

Arguments:

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(devExt->Fdo.ControllerLink.Flink != NULL);
    USBPORT_ASSERT(devExt->Fdo.ControllerLink.Blink != NULL);

    USBPORT_InterlockedRemoveEntryList(&devExt->Fdo.ControllerLink,
                                       &USBPORT_GlobalsSpinLock);

    devExt->Fdo.Flags.FdoRegistered = 0;

    devExt->Fdo.ControllerLink.Blink = NULL;
    devExt->Fdo.ControllerLink.Flink = NULL;

    return;
}


NTSTATUS
USBPORT_RegisterUSBPortDriver(
    PDRIVER_OBJECT DriverObject,
    ULONG MiniportHciVersion,
    PUSBPORT_REGISTRATION_PACKET RegistrationPacket,
    PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    Called from DriverEntry by miniport.

    The opposite of this function is the DriverObject->Unload
    routine which we hook.

    This routie is called once for each class of miniport so it may be
    called multiple times.

Arguments:

Return Value:

--*/
{
    ULONG regPacketLength = 0;
    NTSTATUS ntStatus = STATUS_BOGUS;
    PUSBPORT_MINIPORT_DRIVER miniportDriver = NULL;

    ALLOC_POOL_Z(miniportDriver,
                 NonPagedPoolNx,
                 sizeof(USBPORT_MINIPORT_DRIVER));
    if (NULL == miniportDriver) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto USBPORT_RegisterUSBPortDriver_Clean;
    }

    //
    // WPP requires that we call this from DriverEntry.
    // Only initialize this on our first entry
    //
    if (!InterlockedExchange(&WppInitialized, 1)){
        WPP_INIT_TRACING(DriverObject, RegistryPath);
    }

    DbgTrace((UsbpDebugTrace, "'miniport driver: %p\n", miniportDriver));

    DriverObject->MajorFunction[IRP_MJ_CREATE]=
    DriverObject->MajorFunction[IRP_MJ_CLOSE] =
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] =
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] =
    DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] =
    DriverObject->MajorFunction[IRP_MJ_POWER] = USBPORT_Dispatch;
    DriverObject->MajorFunction[IRP_MJ_PNP] = USBPORT_DispatchPnP;
    DriverObject->DriverExtension->AddDevice = USBPORT_PnPAddDevice;
    miniportDriver->MiniportUnload = DriverObject->DriverUnload;
    DriverObject->DriverUnload = USBPORT_Unload;
    miniportDriver->DriverObject = DriverObject;

#define USBHDCDI_MIN_REV_SUPPORTED      100

    // remeber the registered version
    miniportDriver->HciVersion = MiniportHciVersion;

    if (MiniportHciVersion < USBHDCDI_MIN_REV_SUPPORTED) {
        ntStatus = STATUS_UNSUCCESSFUL;
        goto USBPORT_RegisterUSBPortDriver_Clean;
    }

    // do our version (1) stuff
    if (MiniportHciVersion >= 100) {
        RegistrationPacket->USBPORTSVC_DbgPrint
                                      = USBPORTSVC_DbgPrint;
        RegistrationPacket->USBPORTSVC_TestDebugBreak
                                      = USBPORTSVC_TestDebugBreak;
        RegistrationPacket->USBPORTSVC_AssertFailure
                                      = USBPORTSVC_AssertFailure;
        RegistrationPacket->USBPORTSVC_GetMiniportRegistryKeyValue
                                      = USBPORTSVC_GetMiniportRegistryKeyValue;
        RegistrationPacket->USBPORTSVC_InvalidateRootHub
                                      = USBPORTSVC_InvalidateRootHub;
        RegistrationPacket->USBPORTSVC_InvalidateEndpoint
                                      = USBPORTSVC_InvalidateEndpoint;
        RegistrationPacket->USBPORTSVC_CompleteTransfer
                                      = USBPORTSVC_CompleteTransfer;
        RegistrationPacket->USBPORTSVC_CompleteIsoTransfer
                                      = USBPORTSVC_CompleteIsoTransfer;
        RegistrationPacket->USBPORTSVC_LogEntry
                                      = USBPORTSVC_LogEntry;
        RegistrationPacket->USBPORTSVC_MapHwPhysicalToVirtual
                                      = USBPORTSVC_MapHwPhysicalToVirtual;
        RegistrationPacket->USBPORTSVC_RequestAsyncCallback
                                      = USBPORTSVC_RequestAsyncCallback;
        RegistrationPacket->USBPORTSVC_ReadWriteConfigSpace
                                      = USBPORTSVC_ReadWriteConfigSpace;
        RegistrationPacket->USBPORTSVC_Wait
                                      = USBPORTSVC_Wait;
        RegistrationPacket->USBPORTSVC_InvalidateController
                                      = USBPORTSVC_InvalidateController;
        RegistrationPacket->USBPORTSVC_BugCheck
                                      = USBPORTSVC_BugCheck;
        RegistrationPacket->USBPORTSVC_NotifyDoubleBuffer
                                      = USBPORTSVC_NotifyDoubleBuffer;
        regPacketLength = sizeof(USBPORT_REGISTRATION_PACKET_V1);

        DbgTrace((UsbpDebugTrace, "'Miniport Version 1 support\n"));
    }

    // do our version (2) stuff, this is a superset of version 1
    if (MiniportHciVersion >= 200) {
        DbgTrace((UsbpDebugTrace, "'Miniport Version 2 support\n"));
        regPacketLength = sizeof(USBPORT_REGISTRATION_PACKET_V2);
    }

    // do our version (3) stuff, this is a superset of version 1 & 2
    if (MiniportHciVersion >= 300) {
        DbgTrace((UsbpDebugTrace, "'Miniport Version 3 support\n"));
        regPacketLength = sizeof(USBPORT_REGISTRATION_PACKET_V3);

        RegistrationPacket->USBPORTSVC_RequestAsyncCallbackEx
                                      = USBPORTSVC_RequestAsyncCallbackEx;
        RegistrationPacket->USBPORTSVC_CancelAsyncCallback
                                      = USBPORTSVC_CancelAsyncCallback;
    }

    if (MiniportHciVersion >= 310) {

        DbgTrace((UsbpDebugTrace, "'Miniport Version 3.10 support\n"));
        regPacketLength = sizeof(USBPORT_REGISTRATION_PACKET);

        RegistrationPacket->USBPORTSVC_Rh_Complete
                                      = USBPORTSVC_Rh_Complete;


        RegistrationPacket->USBPORTSVC_EtwWrite
                                      = USBPORTSVC_EtwWrite;

        RegistrationPacket->USBPORTSVC_IsInterruptPerTdEnabled
                                      = USBPORTSVC_IsInterruptPerTdEnabled;

        RegistrationPacket->USBPORTSVC_IgnoreResumeSignaling
                                      = USBPORTSVC_IgnoreResumeSignaling;
    }

    // save a copy of the packet
    RtlCopyMemory(&miniportDriver->RegistrationPacket,
                  RegistrationPacket,
                  regPacketLength);

    //
    // Store the registry path for WMI
    //
    ntStatus = USBPORT_WmiDriverEntry(DriverObject,
                                      RegistryPath,
                                      miniportDriver);

    if (!NT_SUCCESS(ntStatus)) {
        goto USBPORT_RegisterUSBPortDriver_Clean;
    }

    // put this driver on our list
    ExInterlockedInsertTailList(&USBPORT_MiniportDriverList,
                                &miniportDriver->ListEntry,
                                &USBPORT_GlobalsSpinLock);

    goto USBPORT_RegisterUSBPortDriver_Done;

USBPORT_RegisterUSBPortDriver_Clean:

    if (NULL != miniportDriver) {
        UsbFreePool(miniportDriver);
        miniportDriver = NULL;
    }

USBPORT_RegisterUSBPortDriver_Done:

    return ntStatus;
}

#pragma warning(push)
#pragma warning(disable: 4200) //  zero-sized array in struct/union

typedef struct _USBPORT_ASYNC_TIMER {

    LIST_ENTRY NextTimer;
    ULONG Sig;
    PDEVICE_OBJECT FdoDeviceObject;
    KTIMER Timer;
    KDPC Dpc;
    USB_IOREQUEST_CONTEXT UsbIoRequest;
    PMINIPORT_CALLBACK MpCallbackFunction;
    ULONG MpContextLength;
    PVOID HighResTimer;
    PVOID MiniportContext[0];

} USBPORT_ASYNC_TIMER, *PUSBPORT_ASYNC_TIMER;

#pragma warning(pop)


VOID
USBPORT_AsyncTimerDpc(
    PKDPC Dpc,
    PVOID DeferredContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2
    )

/*++

Routine Description:

    This routine runs at DISPATCH_LEVEL IRQL.

Arguments:

    Dpc - Pointer to the DPC object.

    DeferredContext - supplies FdoDeviceObject.

    SystemArgument1 - not used.

    SystemArgument2 - not used.

Return Value:

    None.

--*/
{
    PDEVICE_OBJECT fdoDeviceObject;
    KIRQL irql;
    PUSBPORT_ASYNC_TIMER asyncTimer  = DeferredContext;
    PDEVICE_EXTENSION devExt;
    ETW_EXECUTION_METRICS   execMetrics;

    ICE_START_TRACE();
    ETW_MEASURE_TIME_START(execMetrics);

    fdoDeviceObject = asyncTimer->FdoDeviceObject;
    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(fdoDeviceObject, LOG_RH, 'ayTM', fdoDeviceObject, asyncTimer, 0);
    ASSERT_MPASYNC_TIMER(asyncTimer);


    // Remove the async timer from the MpAsyncTimerListHead with the
    // MpAsyncTimerListSpin held.
    //
    USBPORT_AcquireSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpAsyncTimerListSpin, &irql);

    RemoveEntryList(&asyncTimer->NextTimer);
    USB_ZAP_LINK(&asyncTimer->NextTimer);

    USBPORT_ReleaseSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpAsyncTimerListSpin, irql);


    // Call the miniport timer callback with the MpRootHubFunctionSpin held.
    //
    USBPORT_AcquireSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);

    asyncTimer->MpCallbackFunction(devExt->Fdo.MiniportDeviceData,
                                   &asyncTimer->MiniportContext[0]);

    USBPORT_ReleaseSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);

    // Free the async timer.
    //
    USBPORT_ASSERT(!asyncTimer->UsbIoRequest.FreeContext);
    asyncTimer->UsbIoRequest.FreeContext = FALSE; // prefix 1313864
    USBPORT_Core_DecIoRequestCount(devExt->HcFdoDeviceObject, &asyncTimer->UsbIoRequest, TRUE);

    if (asyncTimer->HighResTimer != NULL) {
        USBPORT_FreeHighResTimer(asyncTimer->HighResTimer, FALSE);
    }

    UsbFreePool(asyncTimer);

    ETW_STOP_AND_LOG_TIME(fdoDeviceObject, execMetrics, ASYNCTIMERDPC);
    ICE_STOP_TRACE();
}


NTSTATUS
USBPORTSVC_RequestAsyncCallbackEx(
    PDEVICE_DATA DeviceData,
    ULONG MilliSeconds,
    PVOID Context,
    ULONG ContextLength,
    PMINIPORT_CALLBACK CallbackFunction,
    PVOID *CbHandle,
    BOOLEAN RootHubPortFunction
    )
/*++

Routine Description:

    Service exported to miniports to wait on the HW and to time asynchronous events. This version allows the miniport
    to handle the error.

Arguments:

    CbHandle - (optional) filled in with handle that may be used to cancel the callback.

    RootHubPortFunction - miniport must set this value to true if the request is in the context of a root hub port function,
                        MINIPORT_RH_* ora an async timer callback

Return Value:

    Error if timer could not be allocated.

--*/
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT hcFdo;
    LONG dueTime;
    ULONG timerIncrement;
    PUSBPORT_ASYNC_TIMER asyncTimer = NULL;
    ULONG siz;
    NTSTATUS nts;
    KIRQL irql = PASSIVE_LEVEL;

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);
    hcFdo = devExt->HcFdoDeviceObject;

    // allocate and schedule the timer

    do {

        if (devExt->Fdo.HcDisconnectRootHubPorts) {
            nts = STATUS_UNSUCCESSFUL;
            break;
        }

        USBPORT_ASSERT(ContextLength);
        if (ContextLength == 0) {
            nts = STATUS_INVALID_PARAMETER;
            break;
        }

        nts = RtlULongAdd(sizeof(struct _USBPORT_ASYNC_TIMER), ContextLength, &siz);

        LOGENTRY(hcFdo, LOG_RH, 'asy0', hcFdo, nts, ContextLength);
        if (NT_ERROR(nts)) {
            break;
        }

        ALLOC_POOL_Z(asyncTimer, NonPagedPoolNx, siz);

        LOGENTRY(hcFdo, LOG_RH, 'asy1', 0, siz, asyncTimer);

        // if this fails the miniport will be waiting a very long time.

        if (asyncTimer == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }
        asyncTimer->Sig = SIG_MP_TIMR;
        asyncTimer->MpContextLength = ContextLength;

        if (ContextLength) {
            RtlCopyMemory(&asyncTimer->MiniportContext,
                          Context,
                          ContextLength);
        }

        asyncTimer->MpCallbackFunction = CallbackFunction;
        asyncTimer->FdoDeviceObject = hcFdo;

        USBPORT_Core_IncIoRequestCount(hcFdo, &asyncTimer->UsbIoRequest, asyncTimer, UsbIoTag_MpAsyncTimer, FALSE);

        // use the high resolution timer if available
        asyncTimer->HighResTimer = USBPORT_AllocateHighResTimer(USBPORT_AsyncTimerDpcHighRes, asyncTimer);

        if (asyncTimer->HighResTimer == NULL) {
            // if not using high resolution timer, or we failed to allocate a high res timer, use
            // the standard kernel timer
            KeInitializeTimer(&asyncTimer->Timer);
            KeInitializeDpc(&asyncTimer->Dpc,
                            USBPORT_AsyncTimerDpc,
                            asyncTimer);
        }

        timerIncrement = KeQueryTimeIncrement() - 1;

        dueTime = -1 * (timerIncrement + MILLISECONDS_TO_100_NS_UNITS(MilliSeconds));

        USBPORT_AcquireSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpAsyncTimerListSpin, &irql);

        InsertTailList(&devExt->Fdo.MpAsyncTimerListHead,
                       &asyncTimer->NextTimer);

        USBPORT_ReleaseSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpAsyncTimerListSpin, irql);

        if (asyncTimer->HighResTimer != NULL) {
            USBPORT_SetHighResTimer(asyncTimer->HighResTimer, MilliSeconds);
        } else {
            KeSetTimer(&asyncTimer->Timer,
                       RtlConvertLongToLargeInteger(dueTime),
                       &asyncTimer->Dpc);
        }

        if (CbHandle) {
            *CbHandle = asyncTimer;
        }
        nts = STATUS_SUCCESS;

    } WHILE (0);

    return nts;
}


VOID
USBPORTSVC_RequestAsyncCallback(
    PDEVICE_DATA DeviceData,
    ULONG MilliSeconds,
    PVOID Context,
    ULONG ContextLength,
    PMINIPORT_CALLBACK CallbackFunction
    )
/*++

Routine Description:

    Service exported to miniports to wait on the HW and to time asynchronous events.  This service is supported for
    compatibility, callers should use USBPORTSVC_RequestAsyncCallbackEx which returns an error.

Arguments:

Return Value:

    None.

--*/
{
    USBPORTSVC_RequestAsyncCallbackEx(DeviceData, MilliSeconds, Context, ContextLength, CallbackFunction, NULL, TRUE);
}


BOOLEAN
USBPORTSVC_CancelAsyncCallback(
    PDEVICE_DATA DeviceData,
    PVOID CbHandle,
    BOOLEAN RootHubPortFunction
    )
/*++

Routine Description:

    Cancels previosuly requested timer callback.

Arguments:

    RootHubPortFunction - miniport must set this value to true if the request is in the context of a root hub port function,
                        MINIPORT_RH_* ora an async timer callback

Return Value:

    TRUE if callback canceled.

--*/
{
    PDEVICE_OBJECT hcFdo;
    KIRQL irql = PASSIVE_LEVEL;
    PUSBPORT_ASYNC_TIMER asyncTimer  = CbHandle;
    PDEVICE_EXTENSION devExt;
    BOOLEAN canceled = FALSE;
    PLIST_ENTRY listEntry;

    hcFdo = asyncTimer->FdoDeviceObject;
    GET_DEVICE_EXT(devExt, hcFdo);
    ASSERT_FDOEXT(devExt);

    if (RootHubPortFunction == FALSE) {
        USBPORT_AcquireSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        LOGENTRY(hcFdo, LOG_RH, 'axl+', hcFdo, asyncTimer, 0);

    }
    USBPORT_ASSERT(asyncTimer->Sig == SIG_MP_TIMR);

    LOGENTRY(hcFdo, LOG_RH, 'ayCA', hcFdo, asyncTimer, 0);

    // walk the list and locate this timer and attempt to cancel the dpc
    listEntry = devExt->Fdo.MpAsyncTimerListHead.Flink;
    while (listEntry != &devExt->Fdo.MpAsyncTimerListHead) {
        PUSBPORT_ASYNC_TIMER tmp;

        tmp = (PUSBPORT_ASYNC_TIMER) CONTAINING_RECORD(
                        listEntry,
                        struct _USBPORT_ASYNC_TIMER,
                        NextTimer);

        if (tmp == asyncTimer) {
            LOGENTRY(hcFdo, LOG_RH, 'ayX1', hcFdo, asyncTimer, 0);

            if (asyncTimer->HighResTimer != NULL) {
                canceled = USBPORT_CancelHighResTimer(asyncTimer->HighResTimer);
            } else {
                canceled = KeCancelTimer(&asyncTimer->Timer);
            }

            if (canceled) {
                LOGENTRY(hcFdo, LOG_RH, 'ayX2', hcFdo, asyncTimer, 0);

                // if removed from the queue dpc will not run.
                RemoveEntryList(&asyncTimer->NextTimer);
                USB_ZAP_LINK(&asyncTimer->NextTimer);

                USBPORT_ASSERT(!asyncTimer->UsbIoRequest.FreeContext);
                asyncTimer->UsbIoRequest.FreeContext = FALSE; // prefix 1313864
                USBPORT_Core_DecIoRequestCount(devExt->HcFdoDeviceObject, &asyncTimer->UsbIoRequest, TRUE);

                if (asyncTimer->HighResTimer != NULL) {
                    USBPORT_FreeHighResTimer(asyncTimer->HighResTimer, FALSE);
                }

                UsbFreePool(asyncTimer);
            }
            break;
        }
        listEntry = tmp->NextTimer.Flink;
    }

    if (RootHubPortFunction == FALSE) {
        LOGENTRY(hcFdo, LOG_RH, 'axl-', hcFdo, asyncTimer, 0);
        USBPORT_ReleaseSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
    }
    return canceled;
}

VOID
USBPORT_CancelAllAsyncTimerCallbacks(
    __in PDEVICE_OBJECT FdoDeviceObject
    )
{
    PDEVICE_EXTENSION       devExt;
    KIRQL                   irql;
    PLIST_ENTRY             listEntry;
    PUSBPORT_ASYNC_TIMER    asyncTimer;
    BOOLEAN                 canceled;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'cat+',
             FdoDeviceObject, 0, 0);

    USBPORT_AcquireSpinLock(FdoDeviceObject,
                            &devExt->Fdo.MpRootHubFunctionSpin,
                            &irql);

    while (!IsListEmpty(&devExt->Fdo.MpAsyncTimerListHead)) {

        listEntry = RemoveHeadList(&devExt->Fdo.MpAsyncTimerListHead);

        USBPORT_ReleaseSpinLock(FdoDeviceObject,
                                &devExt->Fdo.MpRootHubFunctionSpin,
                                irql);


        asyncTimer = (PUSBPORT_ASYNC_TIMER) CONTAINING_RECORD(
                         listEntry,
                         struct _USBPORT_ASYNC_TIMER,
                         NextTimer);

        if (asyncTimer->HighResTimer != NULL) {
            canceled = USBPORT_CancelHighResTimer(asyncTimer->HighResTimer);
        } else {
            canceled = KeCancelTimer(&asyncTimer->Timer);
        }

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'cat1',
                 FdoDeviceObject, asyncTimer, canceled);

        if (canceled) {

            // If canceled the timer dpc will not run.

            USB_ZAP_LINK(&asyncTimer->NextTimer);

            // Even though the callback was canceled, call the miniport
            // so it can release resources it allocated in association
            // with the async callback.

            // The async callback is normally called at DISPATCH_LEVEL.

            KeRaiseIrql(DISPATCH_LEVEL, &irql);

            LOGENTRY(FdoDeviceObject, LOG_MISC, 'cat2',
                     FdoDeviceObject, asyncTimer, asyncTimer->MpCallbackFunction);

            asyncTimer->MpCallbackFunction(devExt->Fdo.MiniportDeviceData,
                                           &asyncTimer->MiniportContext[0]);

            KeLowerIrql(irql);

            USBPORT_ASSERT(!asyncTimer->UsbIoRequest.FreeContext);
            asyncTimer->UsbIoRequest.FreeContext = FALSE;

            USBPORT_Core_DecIoRequestCount(devExt->HcFdoDeviceObject,
                                           &asyncTimer->UsbIoRequest,
                                           TRUE);

            if (asyncTimer->HighResTimer != NULL) {
                USBPORT_FreeHighResTimer(asyncTimer->HighResTimer, FALSE);
            }

            UsbFreePool(asyncTimer);
        }


        USBPORT_AcquireSpinLock(FdoDeviceObject,
                                &devExt->Fdo.MpRootHubFunctionSpin,
                                &irql);
    }

    USBPORT_ReleaseSpinLock(FdoDeviceObject,
                            &devExt->Fdo.MpRootHubFunctionSpin,
                            irql);

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'cat-',
             FdoDeviceObject, 0, 0);
}



VOID
USBPORTSVC_EtwEnableCallback (
    __in LPCGUID SourceId,
    __in ULONG IsEnabled,
    __in UCHAR Level,
    __in ULONGLONG MatchAnyKeyword,
    __in ULONGLONG MatchAllKeyword,
    __in_opt PEVENT_FILTER_DESCRIPTOR FilterData,
    __in_opt PVOID CallbackContext
    )
{
    if (IsEnabled) {

        EtwLoggingEnabled = 1;

        //
        // Log performance events only if explicitly asked by the logger
        // This can be done by passing a keyword option
        // (E.g.: logman start .. -p Microsoft-Windows-USB-USBPORT PerfDiagnostics,.. ..)
        // If the user is using default options (i.e. no keywords) then we don't
        // enable perf tracing
        //
        if ((MatchAnyKeyword & USBPORT_ETW_KEYWORD_PERF_DIAGNOSTICS) &&
                MatchAnyKeyword != (ULONGLONG) -1) {
            EtwPerfLoggingEnabled = 1;
        }

        USBPORT_RundownAllHostControllers();

    } else {

        EtwLoggingEnabled = 0;
        EtwPerfLoggingEnabled = 0;

    }
}

VOID
USBPORT_RundownAllHostControllers (
    VOID
    )
{
    LIST_ENTRY  listHead;
    PLIST_ENTRY listEntry;
    KIRQL       irql;

    // Initialize the local working list of host controllers.
    //
    InitializeListHead(&listHead);

    // USBPORT_USB1fdoList and USBPORT_USB2fdoList are protected by
    // USBPORT_GlobalsSpinLock.  Acquire the lock while walking the
    // lists.
    //
    KeAcquireSpinLock(&USBPORT_GlobalsSpinLock, &irql);

    // Walk the USBPORT_USB1fdoList adding host controllers to the
    // local working list.
    //
    listEntry = USBPORT_USB1fdoList.Flink;

    while (listEntry != &USBPORT_USB1fdoList) {

        PDEVICE_EXTENSION   devExt;

        devExt = (PDEVICE_EXTENSION) CONTAINING_RECORD(
            listEntry,
            struct _DEVICE_EXTENSION,
            Fdo.ControllerLink);

        // Reference the host controller device object to keep it from
        // disappearing as soon as USBPORT_GlobalsSpinLock is released.
        //
        ObReferenceObject(devExt->HcFdoDeviceObject);

        InsertTailList(&listHead,
                       &devExt->Fdo.ControllerRundownLink);

        listEntry = devExt->Fdo.ControllerLink.Flink;
    }

    // Walk the USBPORT_USB2fdoList adding host controllers to the
    // local working list.
    //
    listEntry = USBPORT_USB2fdoList.Flink;

    while (listEntry != &USBPORT_USB2fdoList) {

        PDEVICE_EXTENSION   devExt;

        devExt = (PDEVICE_EXTENSION) CONTAINING_RECORD(
            listEntry,
            struct _DEVICE_EXTENSION,
            Fdo.ControllerLink);

        // Reference the host controller device object to keep it from
        // disappearing as soon as USBPORT_GlobalsSpinLock is released.
        //
        ObReferenceObject(devExt->HcFdoDeviceObject);

        InsertTailList(&listHead,
                       &devExt->Fdo.ControllerRundownLink);

        listEntry = devExt->Fdo.ControllerLink.Flink;
    }

    KeReleaseSpinLock(&USBPORT_GlobalsSpinLock, irql);

    // Now walk the local working list of host controllers that was
    // just built.
    //
    while (!IsListEmpty(&listHead)) {

        PDEVICE_EXTENSION   devExt;

        listEntry = RemoveHeadList(&listHead);

        devExt = (PDEVICE_EXTENSION) CONTAINING_RECORD(
            listEntry,
            struct _DEVICE_EXTENSION,
            Fdo.ControllerRundownLink);

        USBPORT_RundownHostController(devExt->HcFdoDeviceObject);

        // The host controller device object may disappear now after
        // releasing the reference on it.
        //
        ObDereferenceObject(devExt->HcFdoDeviceObject);
    }
}

VOID
USBPORT_RundownHostController (
    PDEVICE_OBJECT FdoDeviceObject
    )
{
    PDEVICE_EXTENSION   devExt;
    LIST_ENTRY          listHead;
    PLIST_ENTRY         listEntry;
    ULONG               idleStatus = 0;
    USB_MINIPORT_STATUS mpStatus;
    struct {
        UCHAR   HC_HighSpeed;
        UCHAR   HC_Run;
        UCHAR   HC_AsyncScheduleEnable;
        UCHAR   HC_PeriodicScheduleEnable;
    } hcIdleStatus;
    PWSTR               pdoNameBuffer;
    USHORT              pdoNameLength;
    PSTR                acpiInstanceIdString;
    ULONG               acpiInstanceIdLen;
    NTSTATUS            nts;

    C_ASSERT(sizeof(hcIdleStatus) == 4);

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // Query the host controller run and schedule enable status
    // from the miniport.
    //
    MP_PassThru(devExt,
                (LPGUID)&GUID_IDLE_STATUS,
                sizeof(idleStatus),
                &idleStatus,
                mpStatus);

    if (mpStatus == USBMP_STATUS_SUCCESS) {

        hcIdleStatus.HC_HighSpeed
                = (idleStatus & USBHC_HIGHSPEED)            ? 1 : 0;

        hcIdleStatus.HC_Run
                = (idleStatus & USBHC_RUN)                  ? 1 : 0;

        hcIdleStatus.HC_AsyncScheduleEnable
                = (idleStatus & USBHC_ASYNC_LIST_ENABLE)    ? 1 : 0;

        hcIdleStatus.HC_PeriodicScheduleEnable
                = (idleStatus & USBHC_PERIODIC_LIST_ENABLE) ? 1 : 0;

    } else {
        hcIdleStatus.HC_HighSpeed               = (UCHAR)-1;
        hcIdleStatus.HC_Run                     = (UCHAR)-1;
        hcIdleStatus.HC_AsyncScheduleEnable     = (UCHAR)-1;
        hcIdleStatus.HC_PeriodicScheduleEnable  = (UCHAR)-1;
    }

    if (devExt->Fdo.PhysicalDeviceObjectName.Buffer != NULL &&
        devExt->Fdo.PhysicalDeviceObjectName.Length != 0) {

        pdoNameBuffer = devExt->Fdo.PhysicalDeviceObjectName.Buffer;
        pdoNameLength = devExt->Fdo.PhysicalDeviceObjectName.Length +
                        sizeof(UNICODE_NULL);
    } else {

        pdoNameBuffer = L"";;
        pdoNameLength = sizeof(UNICODE_NULL);
    }

    acpiInstanceIdString = NULL;
    acpiInstanceIdLen = 0;

    if ((devExt->Fdo.HcResources.DisablePciConfigSpace != 0) &&
        (devExt->Fdo.AcpiDeviceInformation->VendorStringLength != 0)) {

        acpiInstanceIdLen = devExt->Fdo.AcpiDeviceInformation->VendorStringLength +
                            sizeof(UCHAR) +
                            devExt->Fdo.AcpiDeviceInformation->InstanceIdLength +
                            sizeof(NULL);

        ALLOC_POOL_Z(acpiInstanceIdString,
                     NonPagedPoolNx,
                     acpiInstanceIdLen);

        if (acpiInstanceIdString) {
            nts = RtlStringCbPrintfA(acpiInstanceIdString,
                                     (size_t)acpiInstanceIdLen,
                                     "%s\\%s",
                                     (PUCHAR)devExt->Fdo.AcpiDeviceInformation + devExt->Fdo.AcpiDeviceInformation->VendorIdStringOffset,
                                     (PUCHAR)devExt->Fdo.AcpiDeviceInformation + devExt->Fdo.AcpiDeviceInformation->InstanceIdOffset);

            if (NT_SUCCESS(nts)) {
                nts = RtlStringCbLengthA(acpiInstanceIdString,
                                         (size_t)acpiInstanceIdLen,
                                         (size_t*)&acpiInstanceIdLen);
            }

            if (!NT_SUCCESS(nts)) {
                UsbFreePool(acpiInstanceIdString);
                acpiInstanceIdString = NULL;
                acpiInstanceIdLen = 0;
            }
        }
    }

    // Log some interesting info about the host controller.
    // XXXXX Need more info...
    //
    USBPORT_EtwWriteHcData5(FdoDeviceObject,
                            &USBPORT_ETW_EVENT_HC_INFORMATION,
                            NULL,

                            &hcIdleStatus,
                            sizeof(hcIdleStatus),

                            pdoNameBuffer,
                            pdoNameLength,

                            &devExt->Fdo.RuntimePMContext.SetupStatus,
                            sizeof(devExt->Fdo.RuntimePMContext.SetupStatus),

                            &devExt->Fdo.RuntimePMContext.CurrentDevicePowerState,
                            sizeof(devExt->Fdo.RuntimePMContext.SetupStatus),

                            acpiInstanceIdString ? acpiInstanceIdString : "",
                            acpiInstanceIdLen ? acpiInstanceIdLen : sizeof(ANSI_NULL));

    if (acpiInstanceIdString) {
        UsbFreePool(acpiInstanceIdString);
    }

    // Allow the miniport to log host controller specific information
    MP_PassThru(devExt,
                (LPGUID)&GUID_HC_DEVICE_DATA,
                0,
                NULL,
                mpStatus);


    // Build a referenced list of all device handles on the
    // host controller.
    //
    USBPORT_ReferenceAllDeviceHandles(FdoDeviceObject,
                                      &listHead,
                                      NULL,
                                      'DnuR',   // RunD
                                      TRUE,
                                      &listHead);

    while (!IsListEmpty(&listHead)) {

        PDEVH_REF_OBJ_EX    refObjEx;
        PUSBD_DEVICE_HANDLE deviceHandle;

        listEntry = RemoveHeadList(&listHead);

        refObjEx = (PDEVH_REF_OBJ_EX) CONTAINING_RECORD(
            listEntry,
            struct _DEVH_REF_OBJ_EX,
            ExRefLink);

        deviceHandle = refObjEx->DeviceHandle;

        if (deviceHandle->PhysicalDeviceObjectName.Buffer != NULL &&
            deviceHandle->PhysicalDeviceObjectName.Length != 0) {

            pdoNameBuffer = deviceHandle->PhysicalDeviceObjectName.Buffer;
            pdoNameLength = deviceHandle->PhysicalDeviceObjectName.Length +
                            sizeof(UNICODE_NULL);
        } else {

            pdoNameBuffer = L"";
            pdoNameLength = sizeof(UNICODE_NULL);
        }

        // Log some interesting info about the device handle.
        // XXXXX Need more info...
        //
        USBPORT_EtwWriteHcDeviceData4(FdoDeviceObject,
                                      deviceHandle,
                                      &USBPORT_ETW_EVENT_DEVICE_INFORMATION,
                                      NULL,

                                      &deviceHandle->DeviceDescriptor,
                                      sizeof(USB_DEVICE_DESCRIPTOR),

                                      &deviceHandle->DriverName[0],
                                      sizeof(deviceHandle->DriverName),

                                      &deviceHandle->IdleReadyState,
                                      sizeof(ULONG),

                                      pdoNameBuffer,
                                      pdoNameLength);

        // Release the reference on the device handle.  This frees the
        // refObjEx and the device handle is free to disappear at this
        // point.
        //
        USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                        deviceHandle,
                                        &listHead,
                                        'DnuR',
                                        TRUE);
    }

    // Build a referenced list of all endpoints on the host controller.
    //
    USBPORT_ReferenceAllEndpoints(FdoDeviceObject,
                                  'EnuR',       // RunE
                                  &listHead);

    while (!IsListEmpty(&listHead)) {

        PUSBOBJ_REF_EX      epRefObjEx;
        PHCD_ENDPOINT       endpoint;

        listEntry = RemoveHeadList(&listHead);

        epRefObjEx = (PUSBOBJ_REF_EX) CONTAINING_RECORD(
            listEntry,
            struct _USBOBJ_REF_EX,
            ExRefLink);

        endpoint = USBPORT_EndpointFromHandle((PUSBOBJ_REF)epRefObjEx);

        // Log some interesting info about the endpoint.
        // XXXXX Need more info...
        //
        USBPORT_EtwWriteHcDeviceEndpoint(
            FdoDeviceObject,
            endpoint->EtwUsbEndpoint.DeviceHandle,
            endpoint,
            &USBPORT_ETW_EVENT_ENDPOINT_INFORMATION,
            NULL);

        // Release the reference on the endpoint.  This frees the
        // epRefObjEx and the endpoint is free to disappear at this
        // point.
        //
        USBPORT_DereferenceEndpoint(FdoDeviceObject,
                                    (PUSBOBJ_REF)epRefObjEx);
    }
}

NTSTATUS
USBPORTSVC_EtwWrite(
    __in PDEVICE_DATA DeviceData,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in_opt LPCGUID ActivityId,
    __in ULONG UserDataCount,
    __in_ecount_opt(UserDataCount) PEVENT_DATA_DESCRIPTOR  UserData
    )
{
    PDEVICE_EXTENSION   devExt;
    NTSTATUS            ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);

    EventDataDescCreate(&UserData[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        UserDataCount,
                        UserData);

    return ntStatus;
}

BOOLEAN
USBPORTSVC_IgnoreResumeSignaling(
    __in PDEVICE_DATA DeviceData
    )
{
    PDEVICE_EXTENSION   devExt;

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);

    return (1 == devExt->Fdo.Flags.IgnoreResumeSignaling);
}

NTSTATUS
USBPORT_EtwWriteHc(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[1];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        1,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcData1(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[2];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        DataPtr1,
                        DataSize1);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        2,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcData2(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[3];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        DataPtr1,
                        DataSize1);

    EventDataDescCreate(&eventDataDescriptor[2],
                        DataPtr2,
                        DataSize2);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        3,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcData3(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[4];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        DataPtr1,
                        DataSize1);

    EventDataDescCreate(&eventDataDescriptor[2],
                        DataPtr2,
                        DataSize2);

    EventDataDescCreate(&eventDataDescriptor[3],
                        DataPtr3,
                        DataSize3);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        4,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcData4(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[5];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        DataPtr1,
                        DataSize1);

    EventDataDescCreate(&eventDataDescriptor[2],
                        DataPtr2,
                        DataSize2);

    EventDataDescCreate(&eventDataDescriptor[3],
                        DataPtr3,
                        DataSize3);

    EventDataDescCreate(&eventDataDescriptor[4],
                        DataPtr4,
                        DataSize4);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        5,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcData5(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4,
    __in const VOID*            DataPtr5,
    __in ULONG                  DataSize5
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[6];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        DataPtr1,
                        DataSize1);

    EventDataDescCreate(&eventDataDescriptor[2],
                        DataPtr2,
                        DataSize2);

    EventDataDescCreate(&eventDataDescriptor[3],
                        DataPtr3,
                        DataSize3);

    EventDataDescCreate(&eventDataDescriptor[4],
                        DataPtr4,
                        DataSize4);

    EventDataDescCreate(&eventDataDescriptor[5],
                        DataPtr5,
                        DataSize5);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        6,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcDevice(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[2];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is the USB Device data.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        &DeviceHandle->EtwUsbDevice,
                        sizeof(ETW_USB_DEVICE));

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        2,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcDeviceData1(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[3];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is the USB Device data.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        &DeviceHandle->EtwUsbDevice,
                        sizeof(ETW_USB_DEVICE));

    // The third data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[2],
                        DataPtr1,
                        DataSize1);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        3,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcDeviceData2(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[4];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is the USB Device data.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        &DeviceHandle->EtwUsbDevice,
                        sizeof(ETW_USB_DEVICE));

    // The third data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[2],
                        DataPtr1,
                        DataSize1);

    EventDataDescCreate(&eventDataDescriptor[3],
                        DataPtr2,
                        DataSize2);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        4,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcDeviceData3(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[5];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is the USB Device data.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        &DeviceHandle->EtwUsbDevice,
                        sizeof(ETW_USB_DEVICE));

    // The third data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[2],
                        DataPtr1,
                        DataSize1);

    EventDataDescCreate(&eventDataDescriptor[3],
                        DataPtr2,
                        DataSize2);

    EventDataDescCreate(&eventDataDescriptor[4],
                        DataPtr3,
                        DataSize3);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        5,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcDeviceData4(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[6];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is the USB Device data.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        &DeviceHandle->EtwUsbDevice,
                        sizeof(ETW_USB_DEVICE));

    // The third data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[2],
                        DataPtr1,
                        DataSize1);

    EventDataDescCreate(&eventDataDescriptor[3],
                        DataPtr2,
                        DataSize2);

    EventDataDescCreate(&eventDataDescriptor[4],
                        DataPtr3,
                        DataSize3);

    EventDataDescCreate(&eventDataDescriptor[5],
                        DataPtr4,
                        DataSize4);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        6,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcDeviceData6(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4,
    __in const VOID*            DataPtr5,
    __in ULONG                  DataSize5,
    __in const VOID*            DataPtr6,
    __in ULONG                  DataSize6
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[8];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is the USB Device data.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        &DeviceHandle->EtwUsbDevice,
                        sizeof(ETW_USB_DEVICE));

    // The third data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[2],
                        DataPtr1,
                        DataSize1);

    EventDataDescCreate(&eventDataDescriptor[3],
                        DataPtr2,
                        DataSize2);

    EventDataDescCreate(&eventDataDescriptor[4],
                        DataPtr3,
                        DataSize3);

    EventDataDescCreate(&eventDataDescriptor[5],
                        DataPtr4,
                        DataSize4);

    EventDataDescCreate(&eventDataDescriptor[6],
                        DataPtr5,
                        DataSize5);

    EventDataDescCreate(&eventDataDescriptor[7],
                        DataPtr6,
                        DataSize6);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        8,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcDeviceEndpoint(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PHCD_ENDPOINT          Endpoint,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[4];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is the USB Device data.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        &DeviceHandle->EtwUsbDevice,
                        sizeof(ETW_USB_DEVICE));

    // The third data item is the USB Endpoint data.
    //
    EventDataDescCreate(&eventDataDescriptor[2],
                        &Endpoint->EtwUsbEndpoint,
                        sizeof(ETW_USB_ENDPOINT));

    // The fourth data item is the Endpoint Descriptor
    //
    EventDataDescCreate(&eventDataDescriptor[3],
                        &Endpoint->EndpointDescriptor,
                        sizeof(USB_ENDPOINT_DESCRIPTOR));

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        4,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcDeviceEndpointData2(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PHCD_ENDPOINT          Endpoint,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[6];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is the USB Device data.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        &DeviceHandle->EtwUsbDevice,
                        sizeof(ETW_USB_DEVICE));

    // The third data item is the USB Endpoint data.
    //
    EventDataDescCreate(&eventDataDescriptor[2],
                        &Endpoint->EtwUsbEndpoint,
                        sizeof(ETW_USB_ENDPOINT));

    // The fourth data item is the Endpoint Descriptor
    //
    EventDataDescCreate(&eventDataDescriptor[3],
                        &Endpoint->EndpointDescriptor,
                        sizeof(USB_ENDPOINT_DESCRIPTOR));

    // The fifth data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[4],
                        DataPtr1,
                        DataSize1);

    EventDataDescCreate(&eventDataDescriptor[5],
                        DataPtr2,
                        DataSize2);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        6,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcDeviceEndpointData3(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PHCD_ENDPOINT          Endpoint,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[7];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is the USB Device data.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        &DeviceHandle->EtwUsbDevice,
                        sizeof(ETW_USB_DEVICE));

    // The third data item is the USB Endpoint data.
    //
    EventDataDescCreate(&eventDataDescriptor[2],
                        &Endpoint->EtwUsbEndpoint,
                        sizeof(ETW_USB_ENDPOINT));

    // The fourth data item is the Endpoint Descriptor
    //
    EventDataDescCreate(&eventDataDescriptor[3],
                        &Endpoint->EndpointDescriptor,
                        sizeof(USB_ENDPOINT_DESCRIPTOR));

    // The fifth data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[4],
                        DataPtr1,
                        DataSize1);

    EventDataDescCreate(&eventDataDescriptor[5],
                        DataPtr2,
                        DataSize2);

    EventDataDescCreate(&eventDataDescriptor[6],
                        DataPtr3,
                        DataSize3);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        7,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcDeviceEndpointData4(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PHCD_ENDPOINT          Endpoint,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[8];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is the USB Device data.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        &DeviceHandle->EtwUsbDevice,
                        sizeof(ETW_USB_DEVICE));

    // The third data item is the USB Endpoint data.
    //
    EventDataDescCreate(&eventDataDescriptor[2],
                        &Endpoint->EtwUsbEndpoint,
                        sizeof(ETW_USB_ENDPOINT));

    // The fourth data item is the Endpoint Descriptor
    //
    EventDataDescCreate(&eventDataDescriptor[3],
                        &Endpoint->EndpointDescriptor,
                        sizeof(USB_ENDPOINT_DESCRIPTOR));

    // The fifth data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[4],
                        DataPtr1,
                        DataSize1);

    EventDataDescCreate(&eventDataDescriptor[5],
                        DataPtr2,
                        DataSize2);

    EventDataDescCreate(&eventDataDescriptor[6],
                        DataPtr3,
                        DataSize3);

    EventDataDescCreate(&eventDataDescriptor[7],
                        DataPtr4,
                        DataSize4);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        8,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcDeviceEndpointData5(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PHCD_ENDPOINT          Endpoint,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4,
    __in const VOID*            DataPtr5,
    __in ULONG                  DataSize5
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[9];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is the USB Device data.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        &DeviceHandle->EtwUsbDevice,
                        sizeof(ETW_USB_DEVICE));

    // The third data item is the USB Endpoint data.
    //
    EventDataDescCreate(&eventDataDescriptor[2],
                        &Endpoint->EtwUsbEndpoint,
                        sizeof(ETW_USB_ENDPOINT));

    // The fourth data item is the Endpoint Descriptor
    //
    EventDataDescCreate(&eventDataDescriptor[3],
                        &Endpoint->EndpointDescriptor,
                        sizeof(USB_ENDPOINT_DESCRIPTOR));

    // The fifth data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[4],
                        DataPtr1,
                        DataSize1);

    EventDataDescCreate(&eventDataDescriptor[5],
                        DataPtr2,
                        DataSize2);

    EventDataDescCreate(&eventDataDescriptor[6],
                        DataPtr3,
                        DataSize3);

    EventDataDescCreate(&eventDataDescriptor[7],
                        DataPtr4,
                        DataSize4);

    EventDataDescCreate(&eventDataDescriptor[8],
                        DataPtr5,
                        DataSize5);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        9,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteHcDeviceEndpointData6(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PHCD_ENDPOINT          Endpoint,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4,
    __in const VOID*            DataPtr5,
    __in ULONG                  DataSize5,
    __in const VOID*            DataPtr6,
    __in ULONG                  DataSize6
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[10];
    NTSTATUS                ntStatus;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is the USB Device data.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        &DeviceHandle->EtwUsbDevice,
                        sizeof(ETW_USB_DEVICE));

    // The third data item is the USB Endpoint data.
    //
    EventDataDescCreate(&eventDataDescriptor[2],
                        &Endpoint->EtwUsbEndpoint,
                        sizeof(ETW_USB_ENDPOINT));

    // The fourth data item is the Endpoint Descriptor
    //
    EventDataDescCreate(&eventDataDescriptor[3],
                        &Endpoint->EndpointDescriptor,
                        sizeof(USB_ENDPOINT_DESCRIPTOR));

    // The fifth data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[4],
                        DataPtr1,
                        DataSize1);

    EventDataDescCreate(&eventDataDescriptor[5],
                        DataPtr2,
                        DataSize2);

    EventDataDescCreate(&eventDataDescriptor[6],
                        DataPtr3,
                        DataSize3);

    EventDataDescCreate(&eventDataDescriptor[7],
                        DataPtr4,
                        DataSize4);

    EventDataDescCreate(&eventDataDescriptor[8],
                        DataPtr5,
                        DataSize5);

    EventDataDescCreate(&eventDataDescriptor[9],
                        DataPtr6,
                        DataSize6);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        10,
                        eventDataDescriptor);

    return ntStatus;
}

NTSTATUS
USBPORT_EtwWriteDmaExecutionTime(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PHCD_ENDPOINT          Endpoint,
    __in PETW_EXECUTION_METRICS ExecutionMetrics,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4,
    __in const VOID*            DataPtr5,
    __in ULONG                  DataSize5,
    __in PCWSTR                 Tag
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[11];
    NTSTATUS                ntStatus;
    UNICODE_STRING          logTag;

    if ( !EtwEventEnabled( EtwRegHandle, EventDescriptor )) {
        return STATUS_SUCCESS;
    }

    if ( ExecutionMetrics->StartTime.QuadPart == 0) {
        // ETW was enabled after function we are tracing started execution
        // Ignore this sample
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data item is the USB Device data.
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        &DeviceHandle->EtwUsbDevice,
                        sizeof(ETW_USB_DEVICE));

    // The third data item is the USB Endpoint data.
    //
    EventDataDescCreate(&eventDataDescriptor[2],
                        &Endpoint->EtwUsbEndpoint,
                        sizeof(ETW_USB_ENDPOINT));

    // The fourth data item is the Endpoint Descriptor
    //
    EventDataDescCreate(&eventDataDescriptor[3],
                        &Endpoint->EndpointDescriptor,
                        sizeof(USB_ENDPOINT_DESCRIPTOR));

    // The fifth data is the execution time information
    //
    EventDataDescCreate(&eventDataDescriptor[4],
                        ExecutionMetrics,
                        sizeof(ETW_EXECUTION_METRICS));

    // The sixth data item is first caller supplied data item.
    //
    EventDataDescCreate(&eventDataDescriptor[5],
                        DataPtr1,
                        DataSize1);

    EventDataDescCreate(&eventDataDescriptor[6],
                        DataPtr2,
                        DataSize2);

    EventDataDescCreate(&eventDataDescriptor[7],
                        DataPtr3,
                        DataSize3);

    EventDataDescCreate(&eventDataDescriptor[8],
                        DataPtr4,
                        DataSize4);

    EventDataDescCreate(&eventDataDescriptor[9],
                        DataPtr5,
                        DataSize5);

    ntStatus = RtlUnicodeStringInit(&logTag, Tag);

    if (NT_SUCCESS(ntStatus)) {

        EventDataDescCreate(&eventDataDescriptor[10],
                            logTag.Buffer,
                            logTag.Length);
        //
        // Now actually write the event.
        //
        ntStatus = EtwWrite(EtwRegHandle,
                EventDescriptor,
                ActivityId,
                11,
                eventDataDescriptor);
    }

    return ntStatus;
}


NTSTATUS
USBPORT_EtwWriteCodeExecutionTime(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PETW_EXECUTION_METRICS ExecutionMetrics,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr,
    __in ULONG                  DataSize
    )
{
    PDEVICE_EXTENSION       devExt;
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[3];
    NTSTATUS                ntStatus;

    if (!EtwEventEnabled( EtwRegHandle, &USBPORT_ETW_EVENT_CODE_EXECUTION_TIME )) {
        return STATUS_SUCCESS;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // The first data item is the Host Controller data.
    //
    EventDataDescCreate(&eventDataDescriptor[0],
                        &devExt->Fdo.EtwHcDevice,
                        sizeof(ETW_HC_DEVICE));

    // The second data is the execution time information
    //
    EventDataDescCreate(&eventDataDescriptor[1],
                        ExecutionMetrics,
                        sizeof(ETW_EXECUTION_METRICS));


    // The third data item is first caller supplied data item, the code tag
    //
    EventDataDescCreate(&eventDataDescriptor[2],
                        DataPtr,
                        DataSize);

    // Now actually write the event.
    //
    ntStatus = EtwWrite(EtwRegHandle,
                        &USBPORT_ETW_EVENT_CODE_EXECUTION_TIME,
                        ActivityId,
                        3,
                        eventDataDescriptor);

    return ntStatus;
}

#ifndef XP_COMPAT

BOOLEAN USBPORT_IsControllerInHibernatePath(
    __in PDEVICE_OBJECT FdoDeviceObject
    )
{
    PDEVICE_EXTENSION       devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    return devExt->Fdo.CurrentSystemPowerState == PowerSystemHibernate &&
           devExt->Fdo.HibernateCount;
}

#endif

VOID
USBPORT_AsyncTimerDpcHighRes (
    _In_ PEX_TIMER Timer,
    _In_ PVOID Context
    )

/*++

Routine Description:

    This routine is an EXT_CALLBACK wrapper for USBPORT_AsyncTimerDpc.

Arguments:

    Timer - Supplies the timer that expired and triggered this callback.

    Context - Supplies USBPORT_AsyncTimerDpc's context.

Return value:

    None.

*/

{

    USBPORT_AsyncTimerDpc(NULL, Context, NULL, NULL);
    return;
}
