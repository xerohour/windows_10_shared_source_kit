/*++
Copyright (c) Microsoft Corporation

Module Name:

    deviceslot.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

typedef struct _DEVICESLOT_DATA {

    WDFOBJECT                   WdfDeviceSlot;

    HANDLE                      ControllerHandle;

    //
    // DeviceSlot - data used to manage device slots.
    //
    ULONG                       NumberOfDeviceSlots;
    ULONG                       NumberOfScratchpadBuffers;

    PBUFFER_DATA                DeviceContextBaseBufferData;
    PHANDLE                     UsbDeviceHandleArray;

    PBUFFER_DATA                ScratchpadBufferArrayData;
    LIST_ENTRY                  ScratchpadBufferDatas;

} DEVICESLOT_DATA, *PDEVICESLOT_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICESLOT_DATA, GetDeviceSlotData)

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
DeviceSlot_Create(
    __in
        WDFDEVICE   WdfDevice,
    __in
        HANDLE      ControllerHandle,
    __inout
        PHANDLE     ObjectHandle
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
DeviceSlot_PrepareHardware(
    __in
        PDEVICESLOT_DATA    DeviceSlotData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
DeviceSlot_ReleaseHardware(
    __in
        PDEVICESLOT_DATA    DeviceSlotData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
DeviceSlot_Initialize(
    __in
        PDEVICESLOT_DATA        DeviceSlotData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
DeviceSlot_ScratchpadBuffersInitialize(
    __in
        PDEVICESLOT_DATA        DeviceSlotData
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
DeviceSlot_D0Entry(
    __in
        PDEVICESLOT_DATA        DeviceSlotData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
DeviceSlot_DisableAllDeviceSlots(
    __in
        PDEVICESLOT_DATA        DeviceSlotData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
DeviceSlot_D0EntryCleanupState(
    __in
        PDEVICESLOT_DATA        DeviceSlotData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    );

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
DeviceSlot_SetDeviceContext(
    __in
        PDEVICESLOT_DATA    DeviceSlotData,
    __in
        HANDLE              UsbDeviceHandle,
    __in
        ULONG               SlotId,
    __in
        PHYSICAL_ADDRESS    DeviceContextLA
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
DeviceSlot_ClearDeviceContext(
    __in
        PDEVICESLOT_DATA    DeviceSlotData,
    __in
        HANDLE              UsbDeviceHandle,
    __in
        ULONG               SlotId
    );

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
__inline
HANDLE
DeviceSlot_GetUsbDeviceHandle(
    __in
        PDEVICESLOT_DATA    DeviceSlotData,
    __in
        ULONG               SlotId
    )
{
    if (DeviceSlotData->UsbDeviceHandleArray != NULL) {
        return DeviceSlotData->UsbDeviceHandleArray[SlotId];
    } else {
        return NULL;
    }
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
DeviceSlot_LocateDeviceByPortPath(
    __in
        PDEVICESLOT_DATA        DeviceSlotData,
    __in
        PUSB_DEVICE_PORT_PATH   TargetDevicePortPath,
    __out
        PHANDLE                 UsbDeviceHandle
    );

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
BOOLEAN
DeviceSlot_PortPathMatch(
    __in
        PUSB_DEVICE_PORT_PATH   PortPath1,
    __in
        PUSB_DEVICE_PORT_PATH   PortPath2
    )
{
    return ((PortPath1->PortPathDepth == PortPath2->PortPathDepth) &&
            (RtlCompareMemory(PortPath1->PortPath,
                              PortPath2->PortPath,
                              (sizeof(ULONG) * PortPath1->PortPathDepth))
             == (sizeof(ULONG) * PortPath1->PortPathDepth)));
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
DeviceSlot_ControllerGone(
    __in
        PDEVICESLOT_DATA        DeviceSlotData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
DeviceSlot_ControllerResetPreReset(
    __in
        PDEVICESLOT_DATA        DeviceSlotData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
DeviceSlot_ControllerResetPostReset(
    __in
        PDEVICESLOT_DATA        DeviceSlotData
    );

FORCEINLINE
ULONG64
DeviceSlot_GetScratchpadBufferLA(
    __in
        PDEVICESLOT_DATA        DeviceSlotData
    )
{
    return DeviceSlotData->ScratchpadBufferArrayData->LogicalAddress.QuadPart;
}

FORCEINLINE
PUCHAR
DeviceSlot_GetScratchpadBufferVA(
    __in
        PDEVICESLOT_DATA        DeviceSlotData
    )
{
    return DeviceSlotData->ScratchpadBufferArrayData->VirtualAddress;
}

FORCEINLINE
LIST_ENTRY
DeviceSlot_GetScratchpadBufferDatas(
    __in
        PDEVICESLOT_DATA        DeviceSlotData
    )
{
    return DeviceSlotData->ScratchpadBufferDatas;
}

