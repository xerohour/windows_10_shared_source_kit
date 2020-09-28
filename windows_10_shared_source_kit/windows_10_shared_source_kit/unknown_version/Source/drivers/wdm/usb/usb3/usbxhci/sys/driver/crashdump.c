/*++
Copyright (c) Microsoft Corporation

Module Name:

    crashdump.c

Abstract:

    USB XHCI Crashdump write module.

--*/

#include "pch.h"

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
__stdcall
Crashdump_ResetDevice(
    __inout
        PCRASHDUMP_DATA CrashdumpData
    )
/*++

Routine Description:

    This routine will reset the port that the Crashdump device is connected to,
    and then reconfigure the device.

--*/
{
    NTSTATUS            status;
    ULONG               portPathIndex;
    ULONG               portNumber;
    HANDLE              parentHub;
    CRASHDUMP_PORT_TYPE portType;

    TRY {

        CrashdumpLogInfo("Crashdump_ResetDevice: begin");

        portType = CrashdumpPortTypeUnknown;

        //
        // If the crashdump stack needs us to not do a port reset here, just return success
        //
        if (CrashdumpData->DumpIgnoreNextPortReset) {
            CrashdumpData->DumpIgnoreNextPortReset = FALSE;
            status = STATUS_SUCCESS;
            LEAVE;
        } else {
            CrashdumpData->HybridSleepSaveStateAllowed = FALSE;
        }
        portPathIndex = CrashdumpData->PortPath.PortPathDepth - 1;
        portNumber = CrashdumpData->PortPath.PortPath[portPathIndex];

        if (portPathIndex > 0) {

            parentHub = &CrashdumpData->DeviceArray[portPathIndex - 1];

            status = Crashdump_UsbDevice_GetPortType(parentHub, portNumber, &portType);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            status = Crashdump_UsbDevice_ResetPort(parentHub, portNumber, portType);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

        } else {

            portType = CrashdumpData->RootPortType;
            status = Crashdump_Register_ResetPort(&CrashdumpData->RegisterData,
                                                  portNumber,
                                                  CrashdumpData->RootPortType);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        status = Crashdump_UsbDevice_ReConfigureOnPortReset(CrashdumpData->TargetDevice, portType);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

    } FINALLY {

        CrashdumpLogInfo("Crashdump_ResetDevice: end 0x%X", status);
    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
__stdcall
Crashdump_Initialize(
    __inout
        PCRASHDUMP_DATA CrashdumpData
    )
{
    NTSTATUS                    status;
    HCLENGTHVERSION             lengthVersion;
    KSE_XHCI_CONTROLLER_FLAGS   deviceFlags;

    TRY {

        CrashdumpLogInfo("Crashdump_Initialize: Begin");

        lengthVersion = Crashdump_Register_GetVersionInfo(&CrashdumpData->RegisterData);

        CrashdumpData->ControllerWasReset = FALSE;

        deviceFlags = Crashdump_Register_GetDeviceFlags(&CrashdumpData->RegisterData);

        CrashdumpLogInfo("Device speed: %s, RootHub port #: %u",
                         (CrashdumpData->DeviceSpeed == UsbHighSpeed) ? "HS" : "SS",
                         CrashdumpData->PortPath.PortPath[0]);

        CrashdumpLogInfo("xHCI version: 0x%x.0x%x, Flags: 0x%I64X, ContextSize: %u, ScratchpadBuffers: %u, DeviceSlots: %u",
                         lengthVersion.MajorRevision,
                         lengthVersion.MinorRevision,
                         deviceFlags.AsULong64,
                         Crashdump_Register_GetContextSize(&CrashdumpData->RegisterData),
                         Crashdump_Register_GetMaxScratchpadBuffers(&CrashdumpData->RegisterData),
                         Crashdump_Register_GetSupportedNumberOfDeviceSlots(&CrashdumpData->RegisterData));

        if (CrashdumpData->PowerDownType == DumpPowerDownTypeHibernateWriteEnd) {

            CrashdumpData->PowerDownType = DumpPowerDownTypeHibernateReadStart;
        }

        //
        // Donot reset the controller for for hibernate and hybrid sleep
        //
        if (CrashdumpData->PowerDownType == DumpPowerDownTypeHibernateWriteStart ||
            CrashdumpData->PowerDownType == DumpPowerDownTypeHybridSleep){

            //
            // If we are able to configure the dump device without a controller reset
            // and this is a hybrid sleep scenario, we should avoid a port reset
            // to maintain the states
            //
            CrashdumpData->DumpIgnoreNextPortReset = TRUE;

            status = Crashdump_InitializeWithoutControllerReset(CrashdumpData);
            if (!NT_SUCCESS(status)) {
                CrashdumpLogError("Crashdump_InitializeWithoutControllerReset failed with error 0x%X", status);

                //
                // We do not expect to see crashdump initialize without controller reset to fail on
                // hibernate
                //
                NT_ASSERT(FALSE);
                
                status = Crashdump_InitializeWithControllerReset(CrashdumpData);
                if (!NT_SUCCESS(status)) {
                    CrashdumpLogError("Crashdump_InitializeWithControllerReset failed with error 0x%X", status);
                    LEAVE;
                }
            }
        }
        
        status = Crashdump_Register_VerifyRegisterState(&CrashdumpData->RegisterData);
        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("Crashdump_Register_VerifyRegisterState failed with error 0x%X", status);
            LEAVE;
        }
        
        if (CrashdumpData->PowerDownType == DumpPowerDownTypeCrashdump || 
            CrashdumpData->PowerDownType == DumpPowerDownTypeHibernateReadStart) {

            Crashdump_Register_LogRHPortInfo(&CrashdumpData->RegisterData, CrashdumpData->PortPath.PortPath[0]);

            //
            // If we are resuming from hibernate, we need to do a BIOS handoff
            // Failure to complete BIOS handoff is not fatal
            //
            if (CrashdumpData->PowerDownType == DumpPowerDownTypeHibernateReadStart) {
                Crashdump_CleanCommonBuffer(CrashdumpData);
                status = Crashdump_Register_BiosHandoff(&CrashdumpData->RegisterData);
                if (!NT_SUCCESS(status)) {
                    CrashdumpLogError("Crashdump_Register_BiosHandoff failed with error 0x%X", status);
                }
            }
            status = Crashdump_InitializeWithControllerReset(CrashdumpData);
            if (!NT_SUCCESS(status)) {
                CrashdumpLogError("Crashdump_InitializeWithControllerReset failed with error 0x%X", status);
                LEAVE;
            }
        }

    } FINALLY {

        CrashdumpData->HybridSleepSaveStateAllowed = FALSE;

        //
        // In Multistage resume we need to prevent a call to PoSetHiberRange during
        // resume
        //
        if (CrashdumpData->PowerDownType == DumpPowerDownTypeHibernateWriteStart) {

            CrashdumpData->PowerDownType = DumpPowerDownTypeHibernateWriteEnd;
        }

        //
        // If we are going to Hybrid sleep we need to save the controller state
        // in Cleanup if the controller was not reset
        //
        if (CrashdumpData->PowerDownType == DumpPowerDownTypeHybridSleep) {

            CrashdumpData->PowerDownType = DumpPowerDownTypeHibernateWriteEnd;

            if ((deviceFlags.NoControllerSaveRestoreState == 0) &&
                (CrashdumpData->ControllerWasReset == FALSE)) {
                CrashdumpData->HybridSleepSaveStateAllowed = TRUE;
            }
        }

        if (CrashdumpData->PowerDownType == DumpPowerDownTypeHibernateReadStart) {

            CrashdumpData->PowerDownType = DumpPowerDownTypeHibernateReadEnd;
        }

        CrashdumpLogInfo("Crashdump_Initialize: End 0x%X", status);
    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
__stdcall
Crashdump_Cleanup(
    __inout
        PCRASHDUMP_DATA CrashdumpData
    )
{

    ULONG               portPathIndex;
    ULONG               portNumber;
    NTSTATUS            status;
    
    CrashdumpLogInfo("Crashdump_Cleanup: begin");

    TRY {

        //
        // We need to stop all endpoints to successfully save the controller state for resume from Hybrid sleep
        //
        for (portPathIndex = 0; portPathIndex < CrashdumpData->PortPath.PortPathDepth; portPathIndex++) {
        
            status = Crashdump_UsbDevice_Cleanup(&CrashdumpData->DeviceArray[portPathIndex]);
            if (!NT_SUCCESS(status)) {
                CrashdumpLogError("Crashdump_UsbDevice_Cleanup failed with error 0x%X", status);
            }

            //
            // If it is safe to do hybrid sleep, configure the endpoints to the 
            // runtime state
            //
            if (CrashdumpData->HybridSleepSaveStateAllowed) {
                status = Crashdump_UsbDevice_ConfigureEndpointsForSaveState(&CrashdumpData->DeviceArray[portPathIndex]);
                if (!NT_SUCCESS(status)) {
                    CrashdumpData->HybridSleepSaveStateAllowed = FALSE;
                }
            }
        }

        //
        // We need to suspend all the ports in the boot path
        // We have to start suspending ports at the downstream hubs and move upstream
        //
        for (portPathIndex = CrashdumpData->PortPath.PortPathDepth-1; ; portPathIndex--) {
        
            portNumber = CrashdumpData->PortPath.PortPath[portPathIndex];
        
            if (portPathIndex == 0) {
                status = Crashdump_Register_SuspendPort(&CrashdumpData->RegisterData,
                                           portNumber);
                if (!NT_SUCCESS(status)) {
                    CrashdumpLogError("Crashdump_Register_SuspendPort failed with error 0x%X", status);
                }
                //
                // Once we are at the root hub we can break out of the loop
                //
                break;
            } else {
                status = Crashdump_UsbDevice_SuspendPort(&CrashdumpData->DeviceArray[portPathIndex - 1],
                                                         portNumber);

                if (!NT_SUCCESS(status)) {
                    CrashdumpLogError("Crashdump_UsbDevice_SuspendPort failed with error 0x%X", status);
                }
        
            }
        }

        status = Crashdump_Command_Stop(&CrashdumpData->CommandData, TRUE);
        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("Crashdump_Command_Stop failed with error 0x%X", status);
        }

        //
        // We are all done, lets stop the controller now.
        //
        status = Crashdump_Register_StopController(&CrashdumpData->RegisterData);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        if (CrashdumpData->HybridSleepSaveStateAllowed) {

             Crashdump_EventRing_PrepareForSaveState(
                            &CrashdumpData->EventRingHibernateData,
                            Controller_GetInterrupterHandle(CrashdumpData->ControllerHandle));

            status = Crashdump_Register_SaveStateForHybridSleep(&CrashdumpData->RegisterData);
            if (!NT_SUCCESS(status)) {
                CrashdumpLogError("Crashdump_Register_SaveStateForHybridSleep failed with error 0x%X", status);
            } else {
                CrashdumpLogInfo("Crashdump_Register_SaveStateForHybridSleep succeeded.");
            }
        }
    } FINALLY {
    }
    CrashdumpLogInfo("Crashdump_Cleanup: end");

    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_ResetPipe(
    __inout
        PCRASHDUMP_DATA     CrashdumpData,
    __in
        PURB_PIPE_REQUEST   ResetPipeUrb
    )
{
    NTSTATUS    status;
    UCHAR       endpointAddress;

    TRY {

        CrashdumpLogInfo("Crashdump_ResetPipe: Begin");

        UNREFERENCED_PARAMETER(CrashdumpData);

        //
        // N.B: PipeHandle is not USBD_PIPE_HANDLE, it is USBD_PIPE_INFORMATION.
        //
        endpointAddress = ((PUSBD_PIPE_INFORMATION)ResetPipeUrb->PipeHandle)->EndpointAddress;

        CrashdumpLogInfo("Endpoint address is %u", endpointAddress);

        status = STATUS_SUCCESS;

    } FINALLY {

        CrashdumpLogInfo("Crashdump_ResetPipe: end 0x%X", status);
    }

    return status;
}

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
    )
{
    NTSTATUS    status;
    UCHAR       endpointAddress;
    UCHAR       endpointDCI;

    TRY {

        CrashdumpLogInfo("Crashdump_DoBulkTransfer: Begin");

        //
        // N.B: PipeHandle is not USBD_PIPE_HANDLE, it is USBD_PIPE_INFORMATION.
        //
        endpointAddress = ((PUSBD_PIPE_INFORMATION)BulkUrb->PipeHandle)->EndpointAddress;
        endpointDCI = EndpointAddressToDCI(endpointAddress);

        CrashdumpLogInfo("Endpoint: 0x%X,%u, BufferLength %u", endpointAddress, endpointDCI, BulkUrb->TransferBufferLength);

        //
        // If we have started bulk transfers we should no longer honor this flag
        //
        CrashdumpData->DumpIgnoreNextPortReset = FALSE;

        status = Crashdump_UsbDevice_SendBulkTransfer(CrashdumpData->TargetDevice,
                                                      endpointDCI,
                                                      BulkUrb,
                                                      TransferPAArray,
                                                      IsTransferAsync);
    } FINALLY {

        CrashdumpLogInfo("Crashdump_DoBulkTransfer: end 0x%X", status);
    }

    return status;
}

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
    )
{
    NTSTATUS    status;

    TRY {

        CrashdumpLogInfo("Crashdump_SendUrb: Begin");

        //
        // Initialize the URB status to USBD_STATUS_SUCCESS.
        // Some drivers do not initialize the URB status.
        //
        TransferUrb->UrbHeader.Status = USBD_STATUS_SUCCESS;

        switch (TransferUrb->UrbHeader.Function) {

            case URB_FUNCTION_RESET_PIPE:

                status = Crashdump_ResetPipe(CrashdumpData,
                                             (PURB_PIPE_REQUEST)TransferUrb);
                
                break;

            case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:

                status = Crashdump_DoBulkTransfer(CrashdumpData,
                                                  (PURB_BULK_OR_INTERRUPT_TRANSFER)TransferUrb,
                                                  TransferPAArray,
                                                  FALSE);
                break;

            case URB_FUNCTION_CONTROL_TRANSFER:

                status = Crashdump_DoControlTransfer(CrashdumpData,
                                                     (PURB_CONTROL_TRANSFER)TransferUrb,
                                                     TransferPAArray);
                break;

            default:

                CrashdumpLogError("Not supported URB function called.");
                status = STATUS_UNSUCCESSFUL;
                break;
        }

    } FINALLY {

        CrashdumpLogInfo("Crashdump_SendUrb: end 0x%X", status);
    }

    return status;
}

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
    )
{
    NTSTATUS                        status;
    ULONG                           i;
    HANDLE                          usbDeviceHandle;
    ULONG                           endpointDCI;
    PCRASHDUMP_ENDPOINT_DATA        crashdumpEndpoint;
    PUSB_DUMP_DEVICE_ENDPOINT_INFO  endpointInfo;
    PVOID                           deviceContext;

    TRY {

        crashdumpEndpoint = NULL;

        status = DeviceSlot_LocateDeviceByPortPath(DeviceSlotHandle,
                                                   PortPath,
                                                   &usbDeviceHandle);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        deviceContext = UsbDevice_GetDeviceContextBuffer(usbDeviceHandle)->VirtualAddress;

        status = Crashdump_UsbDevice_Initialize(DumpUsbDevice,
                                                CrashdumpData,
                                                DumpDeviceInfo,
                                                UsbDevice_GetSlotId(usbDeviceHandle),
                                                deviceContext,
                                                usbDeviceHandle,
                                                PortPath->PortPathDepth);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // Default Control Endpoint is setup for both hubs and target device.
        //
        endpointDCI = 1;

        crashdumpEndpoint = ExAllocatePoolWithTag(NonPagedPool,
                                                  sizeof(CRASHDUMP_ENDPOINT_DATA),
                                                  CrashdumpAllocationTag);

        if (crashdumpEndpoint == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        RtlZeroMemory(crashdumpEndpoint, sizeof(CRASHDUMP_ENDPOINT_DATA));

        status = Crashdump_Endpoint_Initialize(crashdumpEndpoint,
                                               CrashdumpData,
                                               DumpUsbDevice,
                                               endpointDCI,
                                               0,
                                               deviceContext);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        Crashdump_UsbDevice_SetEndpointHandle(DumpUsbDevice,
                                              endpointDCI,
                                              crashdumpEndpoint);
        crashdumpEndpoint = NULL;

        //
        // For just the storage device DumpDeviceInfo would not be NULL.
        // For the hubs in the path to the storage device DumpDeviceInfo would be NULL
        //
        if (DumpDeviceInfo != NULL) {

            //
            // Setup other bulk endpoints if this is the target device, utilizing the
            // endpoint information provided by the storage client driver.
            //
            for (i = 0; i < DumpDeviceInfo->EndpointCount; i++) {

                endpointInfo = &DumpDeviceInfo->EndpointInfo[i];
                endpointDCI = EndpointAddressToDCI(endpointInfo->Address);

                if (UsbDevice_GetEndpointHandle(usbDeviceHandle, endpointDCI) == NULL) {
                    status = STATUS_INVALID_PARAMETER;
                    LEAVE;
                }

                crashdumpEndpoint = ExAllocatePoolWithTag(NonPagedPool,
                                                          sizeof(CRASHDUMP_ENDPOINT_DATA),
                                                          CrashdumpAllocationTag);

                if (crashdumpEndpoint == NULL) {
                    status = STATUS_INSUFFICIENT_RESOURCES;
                    LEAVE;
                }

                RtlZeroMemory(crashdumpEndpoint, sizeof(CRASHDUMP_ENDPOINT_DATA));

                status = Crashdump_Endpoint_Initialize(crashdumpEndpoint,
                                                       CrashdumpData,
                                                       DumpUsbDevice,
                                                       endpointDCI,
                                                       endpointInfo->MaximumTransferSize,
                                                       deviceContext);
                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }

                Crashdump_UsbDevice_SetEndpointHandle(DumpUsbDevice,
                                                      endpointDCI,
                                                      crashdumpEndpoint);
                crashdumpEndpoint = NULL;
            }
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            if (crashdumpEndpoint != NULL) {
                ExFreePoolWithTag(crashdumpEndpoint, CrashdumpAllocationTag);
            }
        }
    }

    return status;
}

NTSTATUS
Crashdump_UcxEvtGetDumpData(
    __in
        UCXCONTROLLER               UcxController,
    __in
        UCXUSBDEVICE                UcxUsbDevice,
    __in
        PUSB_DUMP_DEVICE_INFO       DumpDeviceInfo,
    __out
        PUSB_DUMP_CONTROLLER_INFO   DumpControllerInfo
    )
{
    NTSTATUS                status;
    ULONG                   i;
    HANDLE                  controllerHandle;
    HANDLE                  registerHandle;
    HANDLE                  deviceSlotHandle;
    HANDLE                  commonBufferHandle;
    HANDLE                  targetDeviceHandle;
    PCRASHDUMP_DATA         crashdumpData;
    PUSB_DEVICE_PORT_PATH   targetDevicePortPath;
    USB_DEVICE_PORT_PATH    usbDevicePortPath;

    TRY {

        CrashdumpLogInfo("Crashdump_UcxEvtGetDumpData: Begin");

        targetDeviceHandle = GetUsbDeviceData(UcxUsbDevice);
        controllerHandle = GetControllerData(UcxController);
        registerHandle = Controller_GetRegisterHandle(controllerHandle);
        deviceSlotHandle = Controller_GetDeviceSlotHandle(controllerHandle);
        commonBufferHandle = Controller_GetCommonBufferHandle(controllerHandle);

        targetDevicePortPath = UsbDevice_GetPortPath(targetDeviceHandle);

        //
        // Initialize private function pointer interface.
        //
        DumpControllerInfo->ControllerInitialize = Crashdump_Initialize;
        DumpControllerInfo->ControllerCleanUp = Crashdump_Cleanup;
        DumpControllerInfo->ControllerSendURB = Crashdump_SendUrb;
        DumpControllerInfo->ControllerResetDevice = Crashdump_ResetDevice;        
        DumpControllerInfo->ControllerSendURBAsync = Crashdump_SendUrbAsync;
        DumpControllerInfo->ControllerPollForCompletion = Crashdump_PollForCompletion;

        DumpControllerInfo->ControllerSupportsMultistageResume = FALSE;

        //
        // Allocate controller context.
        //
        crashdumpData = ExAllocatePoolWithTag(NonPagedPool,
                                              sizeof(CRASHDUMP_DATA),
                                              CrashdumpAllocationTag);
        if (crashdumpData == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        RtlZeroMemory(crashdumpData, sizeof(CRASHDUMP_DATA));

        InitializeListHead(&crashdumpData->CommonBufferList);
        crashdumpData->CommonBufferHandle = commonBufferHandle;

        //
        // Initialize Register, Event Ring, Command objects.
        //
        Crashdump_Register_Initialize(&crashdumpData->RegisterData,
                                      registerHandle,
                                      controllerHandle);

        //
        // At this point we do not know if the dump pointers were required for hibernate
        // or crashdump. We allocate memory for event ring for both.
        //
        status = Crashdump_EventRing_InitializeForDump(&crashdumpData->EventRingData, crashdumpData);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        Crashdump_EventRing_InitializeForHibernate(&crashdumpData->EventRingHibernateData, crashdumpData);

        status = Crashdump_Command_Initialize(&crashdumpData->CommandData, crashdumpData);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // Allocate common buffer request for upper level dump driver
        //
        if (DumpControllerInfo->ControllerCommonBufferSize != 0) {

            status = Crashdump_CommonBufferAcquire(crashdumpData,
                                                   DumpControllerInfo->ControllerCommonBufferSize,
                                                   &crashdumpData->DumpMiniportBuffer);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            DumpControllerInfo->ControllerCommonBufferVA = crashdumpData->DumpMiniportBuffer.VirtualAddress;
            DumpControllerInfo->ControllerCommonBufferPA.QuadPart = crashdumpData->DumpMiniportBuffer.LogicalAddress.QuadPart;
            
        }

        //
        // Populate information about the target device.
        //
        crashdumpData->DeviceSpeed = UsbDevice_GetDeviceSpeed(targetDeviceHandle);
        crashdumpData->PortPath = *targetDevicePortPath;
        crashdumpData->RootPortType = Controller_GetRootPortType(controllerHandle,
                                                                   crashdumpData->PortPath.PortPath[0]);

        crashdumpData->DeviceArray =
            ExAllocatePoolWithTag(NonPagedPool,
                                  sizeof(CRASHDUMP_USBDEVICE_DATA) * crashdumpData->PortPath.PortPathDepth,
                                  CrashdumpAllocationTag);

        if (crashdumpData->DeviceArray == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        RtlZeroMemory(crashdumpData->DeviceArray,
                      sizeof(CRASHDUMP_USBDEVICE_DATA) * crashdumpData->PortPath.PortPathDepth);

        __analysis_assume(crashdumpData->PortPath.PortPathDepth <= MAX_USB_DEVICE_DEPTH);

        for (i = 0; i < crashdumpData->PortPath.PortPathDepth; i++) {

            RtlZeroMemory(&usbDevicePortPath, sizeof(USB_DEVICE_PORT_PATH));

            usbDevicePortPath.PortPathDepth = i + 1;

            RtlCopyMemory(&usbDevicePortPath.PortPath[0],
                          &crashdumpData->PortPath.PortPath[0],
                          usbDevicePortPath.PortPathDepth * sizeof(ULONG));

            status =
                Crashdump_InitializeDeviceContext(deviceSlotHandle,
                                                  crashdumpData,
                                                  ((i + 1) == crashdumpData->PortPath.PortPathDepth) ? DumpDeviceInfo : NULL,
                                                  &usbDevicePortPath,
                                                  &crashdumpData->DeviceArray[i]);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        crashdumpData->TargetDevice = &crashdumpData->DeviceArray[crashdumpData->PortPath.PortPathDepth - 1];

        //
        // Allocate CommonBuffers shared between device objects.
        //
        status = Crashdump_CommonBufferAcquire(crashdumpData,
                                               USB_DEFAULT_MAX_PACKET,
                                               &crashdumpData->ControlTransferBuffer);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        if (Crashdump_Register_GetContextSize(&crashdumpData->RegisterData) == REGISTER_CONTEXTSIZE64) {

            status = Crashdump_CommonBufferAcquire(crashdumpData,
                                                   sizeof(INPUT_CONTEXT64),
                                                   &crashdumpData->InputDeviceContextBuffer);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

        } else {

            status = Crashdump_CommonBufferAcquire(crashdumpData,
                                                   sizeof(INPUT_CONTEXT32),
                                                   &crashdumpData->InputDeviceContextBuffer);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }


        //
        // Only allocate as many device slots as needed. Note that SlotId indexes are 1-based.
        //
        crashdumpData->DeviceSlotCount = crashdumpData->PortPath.PortPathDepth;

        status = Crashdump_CommonBufferAcquire(crashdumpData,
                                               (crashdumpData->DeviceSlotCount + 1) * sizeof(ULONG64),
                                               &crashdumpData->DeviceContextBaseBufferData);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // Typical CommonBuffer consumption, in #pages:
        //     BOT, on RH:  2.
        //     UASP, on RH: 
        //     BOT, 1 Hub: 2.
        //     UASP, 1 Hub:
        //
        CrashdumpLogInfo("CommonBuffer: Allocated %u pages, %u bytes. Used %u bytes",
                         crashdumpData->CommonBufferPageCount,
                         crashdumpData->CommonBufferPageCount * PAGE_SIZE,
                         crashdumpData->CommonBufferUsedBytes);

        //
        // At Crashdump initialization time, it is not known whether this context
        // will be used for hibernate write. This will be inferred later using
        // special file notification, and the DumpType will be updated then.
        //
        crashdumpData->PowerDownType = DumpPowerDownTypeCrashdump;

        //
        // Return the xHCI context information in ControllerContext.
        //
        DumpControllerInfo->ControllerContext = crashdumpData;

        //
        // Add the ControllerData to be used for backing up parameters for the
        // event ring during hibernate
        //
        crashdumpData->ControllerHandle = controllerHandle;

        //
        // Save a reference to the context so we can update it later.
        // This context is used to detect Hibernate/Hybrid Sleep
        //
        Controller_SetHibernateContext(controllerHandle, crashdumpData);

        status = STATUS_SUCCESS;

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            if (crashdumpData != NULL) {
                if (crashdumpData->DeviceArray != NULL) {
                    for (i = 0; i < crashdumpData->PortPath.PortPathDepth; i++) {
                        Crashdump_FreeDeviceContext(&crashdumpData->DeviceArray[i]);
                    }
                    ExFreePoolWithTag(crashdumpData->DeviceArray, CrashdumpAllocationTag);
                }
                Crashdump_CommonBufferFreeAll(crashdumpData);
                ExFreePoolWithTag(crashdumpData, CrashdumpAllocationTag);
            }
        }

        CrashdumpLogInfo("Crashdump_UcxEvtGetDumpData: End 0x%X", status);
    }

    return status;
}

VOID
Crashdump_UcxEvtFreeDumpData(
    __in
        UCXCONTROLLER               UcxController,
    __inout
        PUSB_DUMP_CONTROLLER_INFO   DumpControllerInfo
    )
{
    HANDLE                  controllerHandle;
    PCRASHDUMP_DATA         crashdumpData;
    ULONG                   i;

    TRY {

        CrashdumpLogInfo("Crashdump_UcxEvtFreeDumpData: Begin");

        controllerHandle = GetControllerData(UcxController);
        crashdumpData = DumpControllerInfo->ControllerContext;


        DumpControllerInfo->ControllerContext = NULL;

        if (Controller_GetHibernateContext(controllerHandle) == crashdumpData) {
            Controller_SetHibernateContext(controllerHandle, NULL);
        }

        //
        // Free all the memory allocated for the dump context.
        //
        if (crashdumpData->DeviceArray != NULL) {

            for (i = 0; i < crashdumpData->PortPath.PortPathDepth; i++) {
                Crashdump_FreeDeviceContext(&crashdumpData->DeviceArray[i]);
            }

            ExFreePoolWithTag(crashdumpData->DeviceArray, CrashdumpAllocationTag);
        }

        Crashdump_EventRing_FreeDumpData(&crashdumpData->EventRingData);

        Crashdump_CommonBufferFreeAll(crashdumpData);
        ExFreePoolWithTag(crashdumpData, CrashdumpAllocationTag);

    } FINALLY {

        CrashdumpLogInfo("Crashdump_UcxEvtFreeDumpData: End");
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Crashdump_FreeDeviceContext(
    __in
        HANDLE  TargetDevice
    )
{
    ULONG                       endpointDCI;
    PCRASHDUMP_ENDPOINT_DATA    crashdumpEndpoint;

    for (endpointDCI = 1; endpointDCI < MAX_DEVICE_CONTEXT_INDEX; endpointDCI++) {

        crashdumpEndpoint =
            Crashdump_UsbDevice_GetEndpointHandle(TargetDevice, endpointDCI);

        if (crashdumpEndpoint != NULL) {
            ExFreePoolWithTag(crashdumpEndpoint, CrashdumpAllocationTag);
        }
    }
}

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
    )
{
    NTSTATUS        status;
    PBUFFER_DATA    bufferData;

    TRY {









        RequiredSize = ROUND_TO_64(RequiredSize);

        //
        // Crashdump stack does not support allocation requests greater than a Page
        //
        if (RequiredSize > PAGE_SIZE) {
            CrashdumpLogError("Crashdump_CommonBufferAcquire: Required Size: %4u", RequiredSize);
            status = STATUS_INSUFFICIENT_RESOURCES;            
            LEAVE;
        }
        
        //
        // Allocate a new page of Common Buffer if we haven't allocated any pages
        // yet, or this request is bigger than the bytes left in the current page.
        //
        if ((CrashdumpData->CommonBufferUsedBytes == 0) ||
            (RequiredSize > PageBytesLeft(CrashdumpData->CurrentBufferIndex))) {

            status = Crashdump_CommonBufferAllocate(CrashdumpData);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            CrashdumpData->CommonBufferPageCount += 1;
            CrashdumpData->CurrentBufferIndex = 0;
        }

        //
        // Current page is the one at the front of the list.
        //
        bufferData = (PBUFFER_DATA)CrashdumpData->CommonBufferList.Flink;

        Buffer->Size = RequiredSize;

        NT_ASSERT(Is64ByteAligned(bufferData->VirtualAddress));

        Buffer->VirtualAddress = bufferData->VirtualAddress;
        Buffer->VirtualAddress += CrashdumpData->CurrentBufferIndex;

        Buffer->LogicalAddress.QuadPart = bufferData->LogicalAddress.QuadPart;
        Buffer->LogicalAddress.QuadPart += CrashdumpData->CurrentBufferIndex;

        CrashdumpData->CommonBufferUsedBytes += RequiredSize;

        CrashdumpData->CurrentBufferIndex += RequiredSize;

        CrashdumpLogInfo("CommonBuffer: %4u (0x%04X) bytes acquired, virtual: 0x%p, logical: 0x%I64X",
                         RequiredSize,
                         RequiredSize,
                         Buffer->VirtualAddress,
                         Buffer->LogicalAddress.QuadPart);

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
NTSTATUS
__stdcall
Crashdump_InitializeWithoutControllerReset(
    __inout
        PCRASHDUMP_DATA CrashdumpData
    )
{
    
    NTSTATUS        status;
    ULONG           portPathIndex;

    TRY {
        //
        // For multistage resume we need to mark the buffers to be available
        // during boot. 
        //
        Crashdump_CommonBufferMarkForHibernate(CrashdumpData);

        Crashdump_NonPagedPoolBufferMarkForHibernate(CrashdumpData);

        //
        // N.B: Disabling of MFINDEX wrap events is not needed as the run-time stack
        // has stopped enabling them. Keeping this code around so as not to break
        // crashdump in case run-time stack starts enabling them in future.
        //
        Crashdump_Register_DisableWrapEvent(&CrashdumpData->RegisterData);

        Crashdump_Register_PrepareForHibernate(&CrashdumpData->RegisterData);

        Crashdump_EventRing_PrepareForHibernate(&CrashdumpData->EventRingHibernateData,
                                                Controller_GetInterrupterHandle(CrashdumpData->ControllerHandle));

        status = Crashdump_Command_PrepareForDump(&CrashdumpData->CommandData);
        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("Crashdump_Command_PrepareForDump failed with error 0x%X", status);
            LEAVE;
        }

        status = Crashdump_Command_TestCommandRingOperation(&CrashdumpData->CommandData);
        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("Crashdump_Command_TestCommandRingOperation failed with error 0x%X", status);
            LEAVE;
        }

        for (portPathIndex = 0; portPathIndex < CrashdumpData->PortPath.PortPathDepth; portPathIndex++) {

            status = Crashdump_UsbDevice_PrepareForHibernate(&CrashdumpData->DeviceArray[portPathIndex]);
            if (!NT_SUCCESS(status)) {
                CrashdumpLogError("Crashdump_UsbDevice_PrepareForHibernate failed with error 0x%X", status);
                LEAVE;
            }
        }
    } FINALLY {

    }
    return status;
}

_Must_inspect_result_
NTSTATUS
__stdcall
Crashdump_InitializeWithControllerReset(
    __inout
        PCRASHDUMP_DATA CrashdumpData
    )
{
    NTSTATUS            status;
    ULONG               portPathIndex;
    ULONG               portNumber;
    ULONG               parentPortNumber;
    HANDLE              parentHub;
    HANDLE              targetDevice;
    CRASHDUMP_PORT_TYPE portType;

    TRY {
        
        CrashdumpData->ControllerWasReset = TRUE;
        portType = CrashdumpPortTypeUnknown;
        
        //
        // Event Ring cannot be updated without halting the controller. However,
        // sometimes devices fall off the bus if the controller is halted. So we
        // always do a full controller reset.
        //
        // 5.5.2.3.2 : ERSTBA shall not be modified if HCHalted=0.
        //
        // 4.9.4.1 : ERST entries between 0 and ERSTSZ-1 shall not be modified
        // when HCHalted=0.
        //
        status = Crashdump_Register_ResetController(&CrashdumpData->RegisterData);
        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("Crashdump_Register_ResetController failed with error 0x%X", status);
            LEAVE;
        }
        
        //
        // Before starting the controller, install a new Device Context Base Address
        // array, event ring, and command ring.
        //
        Crashdump_Register_UpdateDeviceSlotInfo(&CrashdumpData->RegisterData,
                                                CrashdumpData->DeviceSlotCount,
                                                CrashdumpData->DeviceContextBaseBufferData);
        
        Crashdump_EventRing_PrepareForDump(&CrashdumpData->EventRingData);
        
        status = Crashdump_Command_PrepareForDump(&CrashdumpData->CommandData);
        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("Crashdump_Command_PrepareForDump failed with error 0x%X", status);
            LEAVE;
        }
        
        status = Crashdump_Register_StartController(&CrashdumpData->RegisterData);
        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("Crashdump_Register_StartController failed with error 0x%X", status);
            LEAVE;
        }
        
        Crashdump_Register_LogRHPortInfo(&CrashdumpData->RegisterData, CrashdumpData->PortPath.PortPath[0]);
        
        //
        // Validate command and event ring operation by sending a no-op command.
        //
        status = Crashdump_Command_TestCommandRingOperation(&CrashdumpData->CommandData);
        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("Crashdump_Command_TestCommandRingOperation failed with error 0x%X", status);
            LEAVE;
        }
        
        //
        // Reconfigure the target disk device and hubs on the path to the disk.
        //
        for (portPathIndex = 0; portPathIndex < CrashdumpData->PortPath.PortPathDepth; portPathIndex++) {
        
            portNumber = CrashdumpData->PortPath.PortPath[portPathIndex];
        
            if (portPathIndex > 0) {
        
                //
                // We need to get the port type of the parent hub port.
                // portPathIndex:1 means the device is attached to root hub port 
                //
                parentPortNumber = CrashdumpData->PortPath.PortPath[portPathIndex-1];
                if (portPathIndex == 1) {
                    portType = CrashdumpData->RootPortType;
                } else {
                    status = Crashdump_UsbDevice_GetPortType(&CrashdumpData->DeviceArray[portPathIndex - 1],
                                                    parentPortNumber,
                                                    &portType);
                    if (!NT_SUCCESS(status)) {
                        CrashdumpLogError("Crashdump_UsbDevice_GetPortType failed with error 0x%X", status);
                        LEAVE;
                    }
                }
        
                //
                // portType is either 20 or 30
                //
                if (portType == CrashdumpPortType20) {
                    parentHub = &CrashdumpData->DeviceArray[portPathIndex - 1];
        
                    status = Crashdump_UsbDevice_SetPortPower(parentHub, portNumber);
                    if (!NT_SUCCESS(status)) {
                        CrashdumpLogError("Crashdump_UsbDevice_SetPortPower failed with error 0x%X", status);
                        LEAVE;
                    }
        
                    status = Crashdump_UsbDevice_ResetPort(parentHub, portNumber, portType);
                    if (!NT_SUCCESS(status)) {
                        CrashdumpLogError("Crashdump_UsbDevice_ResetPort failed with error 0x%X", status);
                        LEAVE;
                    }
                }
            } else {

                portType = CrashdumpData->RootPortType;
                status = Crashdump_Register_ResetPort(&CrashdumpData->RegisterData,
                                                      portNumber,
                                                      CrashdumpData->RootPortType);
                if (!NT_SUCCESS(status)) {
                    CrashdumpLogError("Crashdump_Register_ResetPort failed with error 0x%X", status);
                    LEAVE;
                }
            }
        
            targetDevice = &CrashdumpData->DeviceArray[portPathIndex];
        
            status = Crashdump_UsbDevice_ReConfigureOnControllerReset(targetDevice, portType);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }
    } FINALLY {
    }
    return status;
}

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
    )
{
    NTSTATUS    status;

    TRY {

        CrashdumpLogInfo("Crashdump_SendUrbAsync: Begin");

        //
        // Initialize the URB status to USBD_STATUS_SUCCESS.
        // Some drivers do not initialize the URB status.
        //
        TransferUrb->UrbHeader.Status = USBD_STATUS_SUCCESS;

        switch (TransferUrb->UrbHeader.Function) {

            case URB_FUNCTION_RESET_PIPE:

                status = Crashdump_ResetPipe(CrashdumpData,
                                             (PURB_PIPE_REQUEST)TransferUrb);
                
                break;

            case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:

                status = Crashdump_DoBulkTransfer(CrashdumpData,
                                                 (PURB_BULK_OR_INTERRUPT_TRANSFER)TransferUrb,
                                                 TransferPAArray,
                                                 TRUE);
                break;

            case URB_FUNCTION_CONTROL_TRANSFER:

                status = Crashdump_DoControlTransfer(CrashdumpData,
                                                     (PURB_CONTROL_TRANSFER)TransferUrb,
                                                     TransferPAArray);
                break;

            default:

                CrashdumpLogError("Not supported URB function called.");
                status = STATUS_UNSUCCESSFUL;
                break;
        }

    } FINALLY {

        CrashdumpLogInfo("Crashdump_SendUrbAsync: end 0x%X", status);
    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
__stdcall
Crashdump_PollForCompletion(
    __inout
        PCRASHDUMP_DATA     CrashdumpData,
    __inout
        PURB                *Urb
    )
{
    return Crashdump_EventRing_AsyncPoll(Crashdump_GetEventRingHandle(CrashdumpData),Urb);
}

VOID
Crashdump_ProcessSystemPowerAction(
    __in
        PCRASHDUMP_DATA         CrashdumpData,
    __in
        SYSTEM_POWER_ACTION     SystemPowerAction
    )
{
    switch (SystemPowerAction) {

    case SystemPowerAction_HybridSleep:
        CrashdumpLogInfo("Going to S3");
        Crashdump_SetPowerDownType(CrashdumpData, DumpPowerDownTypeHybridSleep);
        break;

    case SystemPowerAction_Hibernate:
        CrashdumpLogInfo("Going to S4");
        Crashdump_SetPowerDownType(CrashdumpData, DumpPowerDownTypeHibernateWriteStart);
        break;

    default:
        break;
    }
}
