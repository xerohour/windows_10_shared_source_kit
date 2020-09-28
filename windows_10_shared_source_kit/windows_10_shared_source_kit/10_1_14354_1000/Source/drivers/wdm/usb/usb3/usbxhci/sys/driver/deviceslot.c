/*++
Copyright (c) Microsoft Corporation

Module Name:

    deviceslot.c

Abstract:

    USB XHCI Driver

--*/

#include "pch.h"

#include "deviceslot.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, DeviceSlot_Create)
#pragma alloc_text(PAGE, DeviceSlot_PrepareHardware)
#pragma alloc_text(PAGE, DeviceSlot_ReleaseHardware)
#endif

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
    )
/*++

Routine Description:

    This function creates a device slot object and initializes the data
    associated with the device slots.

--*/
{
    NTSTATUS                status;
    WDF_OBJECT_ATTRIBUTES   wdfAttributes;
    WDFOBJECT               wdfDeviceSlot;
    PDEVICESLOT_DATA        deviceSlotData;

    UNREFERENCED_PARAMETER(WdfDevice);

    PAGED_CODE();

    TRY {

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, DEVICESLOT_DATA);
        wdfAttributes.ParentObject = WdfDevice;

        status = WdfObjectCreate(&wdfAttributes, &wdfDeviceSlot);

        if (NT_SUCCESS(status)) {

            deviceSlotData = GetDeviceSlotData(wdfDeviceSlot);

            deviceSlotData->WdfDeviceSlot = wdfDeviceSlot;
            deviceSlotData->ControllerHandle = ControllerHandle;

            LogInfo(Controller_GetRecorderLog(ControllerHandle), DeviceSlot,
                    "wdfDeviceSlot 0x%p", wdfDeviceSlot);

            //
            // Return device slot data as the handle to the device slot object.
            //
            *ObjectHandle = (HANDLE)deviceSlotData;

        } else {

            LogError(Controller_GetRecorderLog(ControllerHandle), DeviceSlot,
                     "WdfObjectCreate Failed %!STATUS!", status);
        }

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
DeviceSlot_PrepareHardware(
    __in
        PDEVICESLOT_DATA    DeviceSlotData
    )
/*++

Routine Description:

    This function configures the device slots for the xHCI controller.

--*/
{
    NTSTATUS                    status;
    ULONG                       numberOfDeviceSlots;
    HANDLE                      registerHandle;
    HANDLE                      commonBufferHandle;
    PSCRATCHPAD_BUFFER_ARRAY    scratchpadBufferArray;
    PBUFFER_DATA                currentBuffer;
    PDEVICE_CONTEXT_BASE_ARRAY  deviceContextBaseArray;
    ULONG                       i;

    PAGED_CODE();

    TRY {

        registerHandle = Controller_GetRegisterHandle(DeviceSlotData->ControllerHandle);
        commonBufferHandle = Controller_GetCommonBufferHandle(DeviceSlotData->ControllerHandle);

        //
        // Determine the amount of common buffer to allocate for the device context array.
        //
        DeviceSlotData->DeviceContextBaseBufferData =
            CommonBuffer_AcquireBuffer(commonBufferHandle,
                                       PAGE_SIZE, // TODO: PostWin8 Use the actual size required.
                                       DeviceSlotData,
                                       DEVICESLOT_ALLOC_TAG1);

        if (DeviceSlotData->DeviceContextBaseBufferData == NULL) {

            LogError(Controller_GetRecorderLog(DeviceSlotData->ControllerHandle), DeviceSlot,
                     "Failed to allocate DeviceContextBaseBufferData");
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        //
        // Always initialize the scratchpad buffer list since it is referenced
        // on the release hardware event.
        //
        InitializeListHead(&DeviceSlotData->ScratchpadBufferDatas);

        //
        // Determine the number of scratchpad buffers required. The scratchpad buffers are
        // located in the device context array at index 0.
        //
        DeviceSlotData->NumberOfScratchpadBuffers = Register_GetMaxScratchpadBuffers(registerHandle);

        //
        // Retrieve the common buffer for the scratchpad buffer array. This is based on the
        // number of entries that are in the array times the size of each entry.
        //
        if (DeviceSlotData->NumberOfScratchpadBuffers != 0) {

            DeviceSlotData->ScratchpadBufferArrayData =
                CommonBuffer_AcquireBuffer(commonBufferHandle,
                                           (sizeof(ULONG64) * DeviceSlotData->NumberOfScratchpadBuffers),
                                           DeviceSlotData,
                                           DEVICESLOT_ALLOC_TAG2);

            if (DeviceSlotData->ScratchpadBufferArrayData == NULL) {

                LogError(Controller_GetRecorderLog(DeviceSlotData->ControllerHandle), DeviceSlot,
                         "Failed to allocate ScratchpadBufferArrayData");
                status = STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }

            //
            // Acquire the scratchpad buffers.
            //
            status = CommonBuffer_AcquireBuffers(commonBufferHandle,
                                                 DeviceSlotData->NumberOfScratchpadBuffers,
                                                 Register_GetPageSize(registerHandle),
                                                 &DeviceSlotData->ScratchpadBufferDatas,
                                                 DeviceSlotData,
                                                 DEVICESLOT_ALLOC_TAG3,
                                                 NULL);

            if (!NT_SUCCESS(status)) {

                LogError(Controller_GetRecorderLog(DeviceSlotData->ControllerHandle), DeviceSlot,
                         "CommonBuffer_AcquireBuffers Failed for ScratchpadBufferDatas %!STATUS!", status);
                LEAVE;
            }

            //
            // Set all of the scratch pad buffers.
            //
            scratchpadBufferArray =
                (PSCRATCHPAD_BUFFER_ARRAY)DeviceSlotData->ScratchpadBufferArrayData->VirtualAddress;

            currentBuffer = (PBUFFER_DATA)DeviceSlotData->ScratchpadBufferDatas.Flink;

            for (i=0; i<DeviceSlotData->NumberOfScratchpadBuffers; i++) {

                scratchpadBufferArray->DeviceContextBaseAddress[i] = currentBuffer->LogicalAddress.QuadPart;
                currentBuffer = (PBUFFER_DATA)currentBuffer->ListEntry.Flink;
            }

            //
            // Set the scratch pad array in the device context.
            //
            deviceContextBaseArray =
                (PDEVICE_CONTEXT_BASE_ARRAY)DeviceSlotData->DeviceContextBaseBufferData->VirtualAddress;

            deviceContextBaseArray->DeviceContextBaseAddress[0] =
                DeviceSlotData->ScratchpadBufferArrayData->LogicalAddress.QuadPart;
        }

        //
        // Retrieve the number of device slots supported by this controller.
        //
        numberOfDeviceSlots = Register_GetSupportedNumberOfDeviceSlots(registerHandle);

        //
        // This assert is a sanity check. The register object verifies the validity
        // of the number of device slots supported by the controller.
        //
        NT_ASSERT((numberOfDeviceSlots > 0) && (numberOfDeviceSlots <= 255));

        DeviceSlotData->NumberOfDeviceSlots = numberOfDeviceSlots;

        //
        // Allocate a buffer that holds an array of USB device handles. An additional slot
        // is required (slot #0), in the case where a USB device is in the unaddressed state.
        //
        DeviceSlotData->UsbDeviceHandleArray =
            ExAllocatePoolWithTag(NonPagedPool,
                                  (numberOfDeviceSlots+1)*sizeof(HANDLE),
                                  DriverAllocationTag);

        if (DeviceSlotData->UsbDeviceHandleArray == NULL) {

            LogError(Controller_GetRecorderLog(DeviceSlotData->ControllerHandle), DeviceSlot,
                     "Failed to allocate UsbDeviceHandleArray");
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        RtlZeroMemory(DeviceSlotData->UsbDeviceHandleArray, (numberOfDeviceSlots+1)*sizeof(HANDLE));

        LogInfo(Controller_GetRecorderLog(DeviceSlotData->ControllerHandle), DeviceSlot,
                "DeviceContextBase 0x%p",
                DeviceSlotData->DeviceContextBaseBufferData->VirtualAddress);

        status = STATUS_SUCCESS;

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (DeviceSlotData->DeviceContextBaseBufferData != NULL) {

                CommonBuffer_ReleaseBuffer(commonBufferHandle,
                                           DeviceSlotData->DeviceContextBaseBufferData);

                DeviceSlotData->DeviceContextBaseBufferData = NULL;
            }

            if (DeviceSlotData->ScratchpadBufferArrayData != NULL) {

                CommonBuffer_ReleaseBuffer(commonBufferHandle,
                                           DeviceSlotData->ScratchpadBufferArrayData);
                DeviceSlotData->ScratchpadBufferArrayData = NULL;
            }

            if (IsListEmpty(&DeviceSlotData->ScratchpadBufferDatas) == FALSE) {

                CommonBuffer_ReleaseBuffers(commonBufferHandle,
                                            &DeviceSlotData->ScratchpadBufferDatas);
            }

            if (DeviceSlotData->UsbDeviceHandleArray != NULL) {

                ExFreePoolWithTag(DeviceSlotData->UsbDeviceHandleArray, DriverAllocationTag);
                DeviceSlotData->UsbDeviceHandleArray = NULL;
            }
        }
    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
DeviceSlot_ReleaseHardware(
    __in
        PDEVICESLOT_DATA    DeviceSlotData
    )
/*++

Routine Description:

    This function is called on a release hardware event and frees any
    resources acquired for managing the device slot.

--*/
{
    HANDLE                  registerHandle;
    HANDLE                  commonBufferHandle;
    POPERATIONAL_REGISTERS  operationalRegisters;

    PAGED_CODE();

    TRY {

        registerHandle = Controller_GetRegisterHandle(DeviceSlotData->ControllerHandle);
        commonBufferHandle = Controller_GetCommonBufferHandle(DeviceSlotData->ControllerHandle);
        operationalRegisters = Register_GetOperationalRegisters(registerHandle);

        if (DeviceSlotData->DeviceContextBaseBufferData != NULL) {

            Register_WriteUlong(&operationalRegisters->ConfigRegister.AsUlong32, 0);

            Register_WriteUlong64(
                &operationalRegisters->DeviceContextBaseAddressArrayPointer.QuadPart,
                0,
                (Controller_GetDeviceFlags(DeviceSlotData->ControllerHandle).Use32BitRegisterAccess == 1));

            CommonBuffer_ReleaseBuffer(commonBufferHandle,
                                       DeviceSlotData->DeviceContextBaseBufferData);

            if (DeviceSlotData->ScratchpadBufferArrayData != NULL) {

                CommonBuffer_ReleaseBuffer(commonBufferHandle,
                                           DeviceSlotData->ScratchpadBufferArrayData);
                DeviceSlotData->ScratchpadBufferArrayData = NULL;
            }

            if (IsListEmpty(&DeviceSlotData->ScratchpadBufferDatas) == FALSE) {

                CommonBuffer_ReleaseBuffers(commonBufferHandle,
                                            &DeviceSlotData->ScratchpadBufferDatas);
            }

            DeviceSlotData->DeviceContextBaseBufferData = NULL;
        }

        if (DeviceSlotData->UsbDeviceHandleArray != NULL) {

            ExFreePoolWithTag(DeviceSlotData->UsbDeviceHandleArray, DriverAllocationTag);
            DeviceSlotData->UsbDeviceHandleArray = NULL;
        }

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
DeviceSlot_Initialize(
    __in
        PDEVICESLOT_DATA        DeviceSlotData
    )
/*++

Routine Description:

    This routine initializes (or re-initializes) the following XHCI host
    controller registers:

    5.4.6 Device Context Base Address Array Pointer Register (DCBAAP)
    5.4.7 Configure Register (CONFIG)

    This routine is called during D0 entry and also during non-PnP reset
    recovery after the host controller has been reset.

    The host controller should be halted when this routine is called.

--*/
{
    HANDLE                  registerHandle;
    POPERATIONAL_REGISTERS  operationalRegisters;
    CONFIG_REGISTER         configRegister;

    TRY {

        if (Controller_IsControllerAccessible(DeviceSlotData->ControllerHandle) == FALSE) {
            LEAVE;
        }

        registerHandle = Controller_GetRegisterHandle(DeviceSlotData->ControllerHandle);
        operationalRegisters = Register_GetOperationalRegisters(registerHandle);

        //
        // 5.4.7 Configure Register (CONFIG)
        //
        configRegister.AsUlong32 = 0;
        configRegister.NumberOfDeviceSlotsEnabled = DeviceSlotData->NumberOfDeviceSlots;

        //
        // Disable it for now.
        //
#if 0
        configRegister.ConfigurationInformationEnable = (Register_GetConfigurationInformation(registerHandle) != 0);
#endif

        Register_WriteUlong(&operationalRegisters->ConfigRegister.AsUlong32, configRegister.AsUlong32);

        //
        // 5.4.6 Device Context Base Address Array Pointer Register (DCBAAP)
        //
        Register_WriteUlong64(&operationalRegisters->DeviceContextBaseAddressArrayPointer.QuadPart,
                              DeviceSlotData->DeviceContextBaseBufferData->LogicalAddress.QuadPart,
                              (Controller_GetDeviceFlags(DeviceSlotData->ControllerHandle).Use32BitRegisterAccess == 1));

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
DeviceSlot_ScratchpadBuffersInitialize(
    __in
        PDEVICESLOT_DATA        DeviceSlotData
    )
/*++

Routine Description:

    This routine zero initializes the Scratchpad Buffers (if any).

    This routine is called when the controller enters D0 only if the
    controller state restore failed.

    This routine is also called when the non-PnP controller reset
    process is in progress.

--*/
{
    PBUFFER_DATA    currentBuffer;

    TRY {

        currentBuffer = (PBUFFER_DATA)DeviceSlotData->ScratchpadBufferDatas.Flink;

        while (currentBuffer != (PBUFFER_DATA)&DeviceSlotData->ScratchpadBufferDatas) {

            RtlZeroMemory(currentBuffer->VirtualAddress, currentBuffer->Size);

            currentBuffer = (PBUFFER_DATA)currentBuffer->ListEntry.Flink;
        }

    } FINALLY {

    }

    return;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
DeviceSlot_D0Entry(
    __in
        PDEVICESLOT_DATA        DeviceSlotData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    )
/*++

Routine Description:

    This function is called when the controller enters D0.

    It initializes (or re-initializes) the following XHCI host
    controller registers:

    5.4.6 Device Context Base Address Array Pointer Register (DCBAAP)
    5.4.7 Configure Register (CONFIG)

--*/
{
    TRY {

        LogInfo(Controller_GetRecorderLog(DeviceSlotData->ControllerHandle), DeviceSlot,
                "DeviceSlotData 0x%p WdfPreviousState %d", DeviceSlotData, WdfPreviousState);

        DeviceSlot_Initialize(DeviceSlotData);

    } FINALLY {

    }

    return STATUS_SUCCESS;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
DeviceSlot_DisableAllDeviceSlots(
    __in
        PDEVICESLOT_DATA        DeviceSlotData
    )
/*++

Routine Description:

    This routine marks all device slots as disabled after the controller
    state has been lost.

    This routine is called when the controller enters D0 only if the
    controller state restore failed.

    This routine is also called when the non-PnP controller reset
    process is in progress.

--*/
{
    ULONG       slotId;
    HANDLE      usbDeviceHandle;

    TRY {

        for (slotId = 1; slotId <= DeviceSlotData->NumberOfDeviceSlots; slotId++) {

            usbDeviceHandle = DeviceSlot_GetUsbDeviceHandle(DeviceSlotData, slotId);

            if (usbDeviceHandle != NULL) {

                UsbDevice_SetDeviceDisabled(usbDeviceHandle);
            }
        }

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
DeviceSlot_D0EntryCleanupState(
    __in
        PDEVICESLOT_DATA        DeviceSlotData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    )
/*++

Routine Description:

    This routine is called when the controller enters D0 to purge
    any device slot information, before the controller will be reset
    due to a resume operation.

--*/
{
    TRY {

        LogInfo(Controller_GetRecorderLog(DeviceSlotData->ControllerHandle), DeviceSlot,
                "DeviceSlotData 0x%p WdfPreviousState %d", DeviceSlotData, WdfPreviousState);

        DeviceSlot_ScratchpadBuffersInitialize(DeviceSlotData);

        DeviceSlot_DisableAllDeviceSlots(DeviceSlotData);

    } FINALLY {

    }

    return;
}

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
    )
/*++

Routine Description:

    This function set a device context in a specified slot.

--*/
{
    NTSTATUS                    status;
    PDEVICE_CONTEXT_BASE_ARRAY  deviceContextBaseArray;

    NT_ASSERT((SlotId > 0) && (SlotId <= DeviceSlotData->NumberOfDeviceSlots));

    TRY {

        if (Controller_IsControllerAccessible(DeviceSlotData->ControllerHandle) == FALSE) {
            status = STATUS_SUCCESS;
            LEAVE;
        }

        deviceContextBaseArray =
            (PDEVICE_CONTEXT_BASE_ARRAY)DeviceSlotData->DeviceContextBaseBufferData->VirtualAddress;

        if (deviceContextBaseArray->DeviceContextBaseAddress[SlotId] == 0) {

            DeviceSlotData->UsbDeviceHandleArray[SlotId] = UsbDeviceHandle;
            deviceContextBaseArray->DeviceContextBaseAddress[SlotId] = DeviceContextLA.QuadPart;
            LogInfo(Controller_GetRecorderLog(DeviceSlotData->ControllerHandle), DeviceSlot,
                    "UsbDeviceHandle 0x%p device slot %d set with 0x%I64x",
                    UsbDeviceHandle, SlotId, DeviceContextLA.QuadPart);

            status = STATUS_SUCCESS;

        } else {

            status = STATUS_ACCESS_DENIED;
            LogError(Controller_GetRecorderLog(DeviceSlotData->ControllerHandle), DeviceSlot,
                     "Device slot %d already occupied by UsbDeviceHandle 0x%p",
                     SlotId, DeviceSlotData->UsbDeviceHandleArray[SlotId]);
        }

    } FINALLY {

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
DeviceSlot_ClearDeviceContext(
    __in
        PDEVICESLOT_DATA    DeviceSlotData,
    __in
        HANDLE              UsbDeviceHandle,
    __in
        ULONG               SlotId
    )
/*++

Routine Description:

    This function clears a device context for a specified slot.

--*/
{
    PDEVICE_CONTEXT_BASE_ARRAY  deviceContextBaseArray;

    TRY {

        NT_ASSERT((SlotId > 0) && (SlotId <= DeviceSlotData->NumberOfDeviceSlots));
        NT_ASSERT(DeviceSlotData->UsbDeviceHandleArray[SlotId] != NULL);
        NT_ASSERT(UsbDeviceHandle == DeviceSlotData->UsbDeviceHandleArray[SlotId]);

        deviceContextBaseArray =
            (PDEVICE_CONTEXT_BASE_ARRAY)DeviceSlotData->DeviceContextBaseBufferData->VirtualAddress;

        DeviceSlotData->UsbDeviceHandleArray[SlotId] = NULL;

        //
        // 4.6.4 Disable Slot:
        //   Note: After software receives the Command Completion TRB for a Disable Slot
        //   Command it shall clear the respective DCBAA entry to ‘0’. This action allows
        //   the xHC to identify valid vs. invalid Device Slots after a Restore State
        //   operation.
        //

        deviceContextBaseArray->DeviceContextBaseAddress[SlotId] = 0;

        LogInfo(Controller_GetRecorderLog(DeviceSlotData->ControllerHandle), DeviceSlot,
                "UsbDeviceHandle 0x%p Device Slot %d cleared", UsbDeviceHandle, SlotId);

    } FINALLY {

    }

    return;
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
    )
{
    NTSTATUS                status;
    ULONG                   slotId;
    HANDLE                  deviceHandle;
    PUSB_DEVICE_PORT_PATH   portPath;

    TRY {

        for (slotId = 1; slotId <= DeviceSlotData->NumberOfDeviceSlots; slotId++) {

            deviceHandle = DeviceSlotData->UsbDeviceHandleArray[slotId];
            if (deviceHandle == NULL) {
                continue;
            }

            portPath = UsbDevice_GetPortPath(deviceHandle);

            if (DeviceSlot_PortPathMatch(portPath, TargetDevicePortPath)) {

                *UsbDeviceHandle = deviceHandle;

                status = STATUS_SUCCESS;
                LEAVE;
            }
        }

        status = STATUS_NOT_FOUND;

    } FINALLY {

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
DeviceSlot_ControllerGone(
    __in
        PDEVICESLOT_DATA        DeviceSlotData
    )
/*++

Routine Description:

    Ths routine is called when it is determined that a controller is gone
    
    This routine essentially completes any pending device management
    requests by disabling all device slots. 
--*/
{
    ULONG       slotId;
    HANDLE      usbDeviceHandle;

    TRY {

        for (slotId = 1; slotId <= DeviceSlotData->NumberOfDeviceSlots; slotId++) {

            usbDeviceHandle = DeviceSlot_GetUsbDeviceHandle(DeviceSlotData, slotId);

            if (usbDeviceHandle != NULL) {

                UsbDevice_ControllerGone(usbDeviceHandle);
            }
        }

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
DeviceSlot_ControllerResetPreReset(
    __in
        PDEVICESLOT_DATA        DeviceSlotData
    )
{
    ULONG   slotId;
    HANDLE  usbDeviceHandle;

    for (slotId = 1; slotId <= DeviceSlotData->NumberOfDeviceSlots; slotId++) {

        usbDeviceHandle = DeviceSlot_GetUsbDeviceHandle(DeviceSlotData, slotId);

        if (usbDeviceHandle != NULL) {

            UsbDevice_ControllerResetPreReset(usbDeviceHandle);
        }
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
DeviceSlot_ControllerResetPostReset(
    __in
        PDEVICESLOT_DATA        DeviceSlotData
    )
/*++

Routine Description:

    This routine is called when the non-PnP controller reset process is
    in progress. The controller has been reset and the controller has
    not yet been started again.

--*/
{
    ULONG   slotId;
    HANDLE  usbDeviceHandle;

    TRY {

        for (slotId = 1; slotId <= DeviceSlotData->NumberOfDeviceSlots; slotId++) {

            usbDeviceHandle = DeviceSlot_GetUsbDeviceHandle(DeviceSlotData, slotId);

            if (usbDeviceHandle != NULL) {

                UsbDevice_ControllerResetPostReset(usbDeviceHandle);
            }
        }

        //
        // Mark all device slots as disabled.
        //
        DeviceSlot_DisableAllDeviceSlots(DeviceSlotData);

        DeviceSlot_ScratchpadBuffersInitialize(DeviceSlotData);

        //
        // Re-initializes the following XHCI host controller registers:
        //
        // 5.4.6 Device Context Base Address Array Pointer Register (DCBAAP)
        // 5.4.7 Configure Register (CONFIG)
        //
        DeviceSlot_Initialize(DeviceSlotData);

    } FINALLY {

    }

    return;
}

