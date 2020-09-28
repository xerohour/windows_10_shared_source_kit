/*++
Copyright (c) Microsoft Corporation

Module Name:

    driver.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

typedef
NTSTATUS
(*PFN_KSE_QUERY_DEVICE_FLAGS)(
    __in LPCWSTR DeviceKey,
    __in LPCWSTR DeviceClass,
    __out PULONG64 DeviceFlags
    );

typedef
BOOLEAN
(*PFN_IO_TRY_QUEUE_WORKITEM)(
    __inout PIO_WORKITEM IoWorkItem,
    __in PIO_WORKITEM_ROUTINE_EX WorkerRoutine,
    __in WORK_QUEUE_TYPE QueueType,
    __in_opt __drv_aliasesMem PVOID Context
    );

typedef struct _WDFDRIVER_USBXHCI_CONTEXT {
    LIST_ENTRY  ControllerListHead;
    KSPIN_LOCK  ControllerListLock;
    ULONG       ControllerListCount;
    BOOLEAN     TestModeOn;
} WDFDRIVER_USBXHCI_CONTEXT, *PWDFDRIVER_USBXHCI_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WDFDRIVER_USBXHCI_CONTEXT, WdfDriverGetUsbXhciContext);

extern PWDFDRIVER_USBXHCI_CONTEXT   g_WdfDriverUsbXhciContext;
extern PDRIVER_OBJECT               g_UsbXhciDriverObject;

PFN_KSE_QUERY_DEVICE_FLAGS          g_KseQueryDeviceFlags;
PFN_IO_TRY_QUEUE_WORKITEM           g_IoTryQueueWorkItem;

DRIVER_INITIALIZE                   DriverEntry;
EVT_WDF_OBJECT_CONTEXT_CLEANUP      DriverCleanup;

#if TEST_MODE
NTSTATUS
Xhci_ShouldFail(
    ULONG FailPercent
    );

ULONG
Xhci_NonZeroRandomWithMax(
    ULONG   MaxRandom
    );

extern volatile BOOLEAN g_RANDOM_FAILURE_TEST_CHANGE_COMMAND_TYPE;
extern volatile BOOLEAN g_RANDOM_FAILURE_TEST_REDUCE_RINGSEGMENTS;
extern volatile BOOLEAN g_SKIP_LAST_TD_COMPLETION;

#endif
