/*++
Copyright (c) Microsoft Corporation

Module Name:

    crashdump.h

Abstract:

    USB XHCI Crashdump write module.

--*/

#pragma once

typedef struct _CRASHDUMP_DATA {

    CRASHDUMP_REGISTER_DATA     RegisterData;
    CRASHDUMP_EVENTRING_DATA    EventRingData;
    CRASHDUMP_EVENTRING_DATA    EventRingHibernateData;
    CRASHDUMP_COMMAND_DATA      CommandData;

    //
    // Device Context Array.
    //
    ULONG                       DeviceSlotCount;
    CRASHDUMP_BUFFER_DATA       DeviceContextBaseBufferData;

    //
    // CommonBuffer shared by device objects.
    //
    CRASHDUMP_BUFFER_DATA       ControlTransferBuffer;
    CRASHDUMP_BUFFER_DATA       InputDeviceContextBuffer;

    //
    // Preallocated DMA buffers;
    //
    HANDLE                      CommonBufferHandle;
    LIST_ENTRY                  CommonBufferList;
    ULONG                       CommonBufferPageCount;
    ULONG                       CommonBufferUsedBytes;
    ULONG                       CurrentBufferIndex;

    //
    // Information about Crashdump target device and its parent hub.
    //
    USB_DEVICE_SPEED            DeviceSpeed;
    CRASHDUMP_PORT_TYPE         RootPortType;
    USB_DEVICE_PORT_PATH        PortPath;

    __field_ecount(PortPath.PortPathDepth)
    PCRASHDUMP_USBDEVICE_DATA   DeviceArray;

    PCRASHDUMP_USBDEVICE_DATA   TargetDevice;

    //
    // Specifies whether this context is for crash or hibernate.
    //
    DUMP_POWER_DOWN_TYPE        PowerDownType;

    //
    // Common buffer to be used for device specific DMA
    //
    CRASHDUMP_BUFFER_DATA       DumpMiniportBuffer;

    //
    // A copy of the controller data to be used for hibernate
    //
    HANDLE                      ControllerHandle;

    //
    // Keeps track if the controller was reset
    //
    BOOLEAN                     ControllerWasReset;

    //
    // Keeps track if we need to save the controller state
    //
    BOOLEAN                     HybridSleepSaveStateAllowed;

    //
    // Keeps track if we need skip the next port reset
    //
    BOOLEAN                     DumpIgnoreNextPortReset;
    

} CRASHDUMP_DATA, *PCRASHDUMP_DATA;

//
// Interface between xHCI run-time stack and xHCI Crashdump module.
//
EVT_UCX_CONTROLLER_GET_DUMPDATA Crashdump_UcxEvtGetDumpData;

EVT_UCX_CONTROLLER_FREE_DUMPDATA Crashdump_UcxEvtFreeDumpData;

//
// Interface between Crashdump miniport driver (dmpusbstor.sys, uasp.sys)
// and xHCI Crashdump module.
//
_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
__stdcall
Crashdump_Initialize(
    __inout
        PCRASHDUMP_DATA CrashdumpData
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
__stdcall
Crashdump_Cleanup(
    __inout
        PCRASHDUMP_DATA CrashdumpData
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
__stdcall
Crashdump_SendUrb(
    __inout
        PCRASHDUMP_DATA     CrashdumpData,
    __in
        PURB                TransferUrb,
    __in
        PPHYSICAL_ADDRESS   TransferPAArray
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
__stdcall
Crashdump_ResetDevice(
    __inout
        PCRASHDUMP_DATA CrashdumpData
    );

//
// Interface between other objects and the dumper.
//
VOID
Crashdump_ProcessSystemPowerAction(
    __in
        PCRASHDUMP_DATA         CrashdumpData,
    __in
        SYSTEM_POWER_ACTION     SystemPowerAction
    );

__checkReturn
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Crashdump_CommonBufferAcquire(
    __in
        PCRASHDUMP_DATA         CrashdumpData,
    __in
        ULONG                   RequiredSize,
    __out
        PCRASHDUMP_BUFFER_DATA  Buffer
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
HANDLE
Crashdump_GetRegisterHandle(
    __in
        PCRASHDUMP_DATA CrashdumpData
    )
{
    return &CrashdumpData->RegisterData;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
HANDLE
Crashdump_GetEventRingHandle(
    __in
        PCRASHDUMP_DATA CrashdumpData
    )
{
    if (CrashdumpData->PowerDownType == DumpPowerDownTypeCrashdump ||
        CrashdumpData->ControllerWasReset) {
        return &CrashdumpData->EventRingData;
    } else {
        return &CrashdumpData->EventRingHibernateData;
    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
HANDLE
Crashdump_GetCommandHandle(
    __in
        PCRASHDUMP_DATA CrashdumpData
    )
{
    return &CrashdumpData->CommandData;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
PCRASHDUMP_BUFFER_DATA
Crashdump_GetControlTransferBuffer(
    __in
        PCRASHDUMP_DATA CrashdumpData
    )
{
    RtlZeroMemory(CrashdumpData->ControlTransferBuffer.VirtualAddress,
                  CrashdumpData->ControlTransferBuffer.Size);
                  
    return &CrashdumpData->ControlTransferBuffer;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
PCRASHDUMP_BUFFER_DATA
Crashdump_GetInputDeviceContextBuffer(
    __in
        PCRASHDUMP_DATA CrashdumpData
    )
{
    RtlZeroMemory(CrashdumpData->InputDeviceContextBuffer.VirtualAddress,
                  CrashdumpData->InputDeviceContextBuffer.Size);

    return &CrashdumpData->InputDeviceContextBuffer;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Crashdump_SetDeviceContext(
    __in
        PCRASHDUMP_DATA     CrashdumpData,
    __in
        ULONG               SlotId,
    __in
        PPHYSICAL_ADDRESS   DeviceContextLA
    )
{
    PDEVICE_CONTEXT_BASE_ARRAY  deviceContextBaseArray;

    NT_ASSERT(SlotId <= CrashdumpData->DeviceSlotCount);

    deviceContextBaseArray =
        (PDEVICE_CONTEXT_BASE_ARRAY)CrashdumpData->DeviceContextBaseBufferData.VirtualAddress;

    deviceContextBaseArray->DeviceContextBaseAddress[SlotId] = DeviceContextLA->QuadPart;
}

//
// Private functions.
//
__checkReturn
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Crashdump_InitializeDeviceContext(
    __in
        HANDLE                  DeviceSlotHandle,
    __in
        PCRASHDUMP_DATA         CrashdumpData,
    __in_opt
        PUSB_DUMP_DEVICE_INFO   DumpDeviceInfo,
    __in
        PUSB_DEVICE_PORT_PATH   PortPath,
    __inout
        HANDLE                  DumpUsbDevice
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Crashdump_FreeDeviceContext(
    __in
        HANDLE  TargetDevice
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_ResetPipe(
    __inout
        PCRASHDUMP_DATA     CrashdumpData,
    __in
        PURB_PIPE_REQUEST   ResetPipeUrb
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_DoBulkTransfer(
    __inout
        PCRASHDUMP_DATA                 CrashdumpData,
    __in
        PURB_BULK_OR_INTERRUPT_TRANSFER BulkUrb,
    __in
        PPHYSICAL_ADDRESS               TransferPAArray,
    __in
        BOOLEAN                         IsTransferAsync
    );

_Must_inspect_result_
NTSTATUS
__stdcall
Crashdump_InitializeWithControllerReset(
    __inout
        PCRASHDUMP_DATA CrashdumpData
    );

_Must_inspect_result_
NTSTATUS
__stdcall
Crashdump_InitializeWithoutControllerReset(
    __inout
        PCRASHDUMP_DATA CrashdumpData
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
__stdcall
Crashdump_SendUrbAsync(
    __inout
        PCRASHDUMP_DATA     CrashdumpData,
    __in
        PURB                TransferUrb,
    __in
        PPHYSICAL_ADDRESS   TransferPAArray
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
__stdcall
Crashdump_PollForCompletion(
    __inout
        PCRASHDUMP_DATA     CrashdumpData,
    __inout
        PURB                *Urb
    );


_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
NTSTATUS
Crashdump_DoControlTransfer(
    __inout
        PCRASHDUMP_DATA         CrashdumpData,
    __in
        PURB_CONTROL_TRANSFER   ControlUrb,
    __in
        PPHYSICAL_ADDRESS       TransferPAArray
    )
{
    return Crashdump_UsbDevice_SendControlTransfer(CrashdumpData->TargetDevice,
                                                   (PUSB_DEFAULT_PIPE_SETUP_PACKET)ControlUrb->SetupPacket,
                                                   ControlUrb->TransferBufferLength,
                                                   TransferPAArray);
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
NTSTATUS
Crashdump_CommonBufferAllocate(
    __inout
        PCRASHDUMP_DATA CrashdumpData
    )
{
    NTSTATUS        status;
    PBUFFER_DATA    buffer;

    TRY {

        buffer = CommonBuffer_AcquireBuffer(CrashdumpData->CommonBufferHandle,
                                            PAGE_SIZE,
                                            CrashdumpData,
                                            CRASHDUMP_ALLOC_TAG1);

        if (buffer == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        InsertHeadList(&CrashdumpData->CommonBufferList, &buffer->ListEntry);

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
VOID
Crashdump_CommonBufferFreeAll(
    __in
        PCRASHDUMP_DATA CrashdumpData
    )
{
    while (IsListEmpty(&CrashdumpData->CommonBufferList) == FALSE) {

        CommonBuffer_ReleaseBuffer(CrashdumpData->CommonBufferHandle,
                                   (PBUFFER_DATA)RemoveHeadList(&CrashdumpData->CommonBufferList));
    }
}

FORCEINLINE
VOID
Crashdump_BufferMarkForHibernate(
    __in
        PVOID Buffer,
    __in
        ULONG Length
    )
{
#pragma warning(push)
#pragma warning(disable:6309)

    //
    // PoSetHiberRange is not properly annotated.
    // It does not mark the first parameter as __in_opt which it should
    //
    PoSetHiberRange(NULL,
                    PO_MEM_BOOT_PHASE,
                    Buffer,
                    Length,
                    CrashdumpAllocationTag);
#pragma warning(pop)
}

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
VOID
Crashdump_CommonBufferMarkForHibernate(
    __in
        PCRASHDUMP_DATA CrashdumpData
    )
{
    PBUFFER_DATA    bufferData;

    bufferData = (PBUFFER_DATA)CrashdumpData->CommonBufferList.Flink;

    while (bufferData->ListEntry.Flink != CrashdumpData->CommonBufferList.Flink) {
        
        Crashdump_BufferMarkForHibernate(bufferData, sizeof(BUFFER_DATA));
        Crashdump_BufferMarkForHibernate(bufferData->VirtualAddress, bufferData->Size);

        bufferData = (PBUFFER_DATA)bufferData->ListEntry.Flink;
    }
    
}

__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Crashdump_CleanCommonBuffer(
    __in
        PCRASHDUMP_DATA CrashdumpData
    )
{
    PBUFFER_DATA    bufferData;

    bufferData = (PBUFFER_DATA)CrashdumpData->CommonBufferList.Flink;
    while (bufferData->ListEntry.Flink != CrashdumpData->CommonBufferList.Flink) {
        RtlZeroMemory(bufferData->VirtualAddress, bufferData->Size);
        bufferData = (PBUFFER_DATA)bufferData->ListEntry.Flink;
    }
     
}

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
VOID
Crashdump_NonPagedPoolBufferMarkForHibernate(
    __in
        PCRASHDUMP_DATA CrashdumpData
    )
{
    Crashdump_BufferMarkForHibernate(CrashdumpData, sizeof(CRASHDUMP_DATA));

    Crashdump_BufferMarkForHibernate(CrashdumpData->DeviceArray, 
                                     sizeof(CRASHDUMP_USBDEVICE_DATA) * CrashdumpData->PortPath.PortPathDepth);

    Crashdump_BufferMarkForHibernate(CrashdumpData->RegisterData.RegisterHandle, sizeof(REGISTER_DATA));

    Crashdump_BufferMarkForHibernate(CrashdumpData->ControllerHandle, sizeof(CONTROLLER_DATA));

    Crashdump_BufferMarkForHibernate(CrashdumpData->EventRingData.CurrentBufferData, sizeof(BUFFER_DATA));
}

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
VOID
Crashdump_SetPowerDownType(
    __in
        PCRASHDUMP_DATA       CrashdumpData,
    __in
        DUMP_POWER_DOWN_TYPE  DumpPowerDownType
    )
{
    CrashdumpData->PowerDownType = DumpPowerDownType;
}

FORCEINLINE
DUMP_POWER_DOWN_TYPE
Crashdump_GetPowerDownType(
    __in
        PCRASHDUMP_DATA   CrashdumpData
    )
{
    return CrashdumpData->PowerDownType;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
BOOLEAN
Crashdump_HiberfileReadEnabled(
    __in_opt
        PCRASHDUMP_DATA   CrashdumpData
    )
/*++

Routine Description:

    Returns true if the Crashdump stack was used to read the hiberfile.
    Even if the Crashdump stack is used to write the hiberfile, it may
    or may not be used to read the hiberfile.

--*/
{
    return ((CrashdumpData != NULL) &&
            (CrashdumpData->PowerDownType == DumpPowerDownTypeHibernateReadEnd));
}

FORCEINLINE
HANDLE
Crashdump_GetTargetDevice(
    __in
        PCRASHDUMP_DATA   CrashdumpData
    )
{
    return CrashdumpData->TargetDevice;
}

