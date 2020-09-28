/*++
Copyright (c) Microsoft Corporation

Module Name:

    wer.c

Abstract:

    WER Live Kernel Dump

--*/

#include "pch.h"

#include "wer.tmh"

/* 368321c9-3f58-4e14-8fa0-3a06ef92fc9c */
DEFINE_GUID(GUID_USBHUB3_MINIDUMP_DATA,
            0x368321c9, 0x3f58, 0x4e14, 0x8f, 0xa0, 0x3a, 0x06, 0xef, 0x92, 0xfc, 0x9c);

const CHAR WERTag[]            = "WER Tag";


NTSTATUS
WER_CheckThrottlePolicy(
    __in
        WDFDEVICE           Device,
    __in
        ULONG               BugCheckCode,
    __in
        RECORDER_LOG        IfrLog
        )
/*++

Routine Description:

    Determines if the USBHUB3 WER thottle policy for a bugcheck code should prevent further live dumps from being created.
    
    Currently all USBHUB3 live dumps are throttled to one live dump per bugcheck code, per device instance, per boot.

Arguments:

    Device - WDF device object

    BugCheckCode - Bugcheck code to validate throttle policy for

    IfrLog - IFR log handle to log errors to
    
Return Value:

    NTSTATUS

--*/
{
    NTSTATUS    status;
    DWORD       throttleMask;
    DWORD       bugCheckCodeBitNumber;


    TRY {

        status = STATUS_SUCCESS;

        bugCheckCodeBitNumber = BugCheckCode & 0x1F;

        //
        // Read throttle mask from volatile registry key.
        //
        throttleMask = HUBREG_QueryDeviceWerThrottleMask(Device, IfrLog);

        //
        // Prevent any more dumps from being created if flag is already set 
        //
        if (TEST_BIT(throttleMask, bugCheckCodeBitNumber)) {
            status = STATUS_UNSUCCESSFUL;
        }
    } FINALLY {
    }

    return status;
}


VOID
WER_UpdateThrottlePolicy(
    __in
        WDFDEVICE           Device,
    __in
        ULONG               BugCheckCode,
    __in
        RECORDER_LOG        IfrLog
        )
/*++

Routine Description:

    Update the throttle policy for a device instance

Arguments:

    Device - WDF device object 

    BugCheckCode - Bugcheck code to update the throttle policy for
    
    IfrLog - IFR log handle to log errors to

Return Value:

    VOID

--*/
{
    NTSTATUS    status;
    DWORD       throttleMask;
    DWORD       bugCheckCodeBitNumber;


    TRY {

        //
        // USBHUB3 bugcheck parameter 1 values range from 0x3000 - 0x301F.  Lower 5 bits are used as a bit
        // number in the throttle mask for the device.
        //
        bugCheckCodeBitNumber = BugCheckCode & 0x1F;

        throttleMask = HUBREG_QueryDeviceWerThrottleMask(Device, IfrLog);

        SET_BIT(throttleMask, bugCheckCodeBitNumber);

        status = HUBREG_WriteDeviceWerThrottleMask(Device,
                                                   throttleMask,
                                                   IfrLog);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "Failed to update WER throttle policy for device %!STATUS!",
                       status);
        }

    } FINALLY {
    }

}

FORCEINLINE
NTSTATUS
WER_CopyToSecondaryData(
    __in
        PVOID   Destination,
    __in
        PVOID   Source,
    __in
        size_t  SizeToCopy,
    __in
        PVOID   EndOfDestination
        )
/*++

Routine Description:

Arguments:

--*/
{
    NTSTATUS    status;

    TRY {    
    
        status = STATUS_SUCCESS;

        if (((PUCHAR)Destination + SizeToCopy) > (PUCHAR)EndOfDestination) {
            NT_ASSERT(NT_SUCCESS(status));
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }
    
        RtlCopyMemory(Destination,
                      Source,
                      SizeToCopy);

    } FINALLY {
    }

    return status;
}


FORCEINLINE
VOID
WER_ReferenceDeferenceAllDeviceAndPortContexts(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        BOOLEAN             Reference
        )
/*++

Routine Description:

    Acquires or releases a reference on all port and device contexts associated with a hub FDO

Arguments:

    HubFdoContext - Pointer to the hub FDO context.

    Reference - TRUE: Acquire references
                FALSE: Release references

--*/
{
    ULONG i;
    
    for (i = 1; i <= HubFdoContext->HubInformation.NumberOfPorts; i++) {
        PPORT_CONTEXT       portContext;
        PDEVICE_CONTEXT     deviceContext;

        portContext = HUBFDO_FindPortContextFromNumber(HubFdoContext, i);
            
        if (portContext == NULL) {
            continue;
        }

        deviceContext = portContext->DeviceContext;

        if (deviceContext != NULL) {

            if (Reference == FALSE) {
                WdfObjectDereferenceWithTag(WdfObjectContextGetObject(deviceContext),
                                            (PVOID) WERTag);

                if (deviceContext->PdoContext != NULL) {
                    WdfObjectDereferenceWithTag(WdfObjectContextGetObject(deviceContext->PdoContext),
                                                (PVOID) WERTag);
                }

            } else {
                WdfObjectReferenceWithTag(WdfObjectContextGetObject(deviceContext),
                                          (PVOID) WERTag);

                if (deviceContext->PdoContext != NULL) {
                    WdfObjectReferenceWithTag(WdfObjectContextGetObject(deviceContext->PdoContext),
                                              (PVOID) WERTag);
                }
           }
        }
    }
}

                
NTSTATUS
WER_StoreDeviceContext(
    __in
        HANDLE              TelemetryHandle,
    __in
        PDEVICE_CONTEXT     DeviceContext
        )
/*++

Routine Description:

    Stores a Device Context in the minidump's triage data

Arguments:

    TelemetryHandle - Opaque handle for telemetry data to be stored in minidump

    DeviceContext - Pointer to the device context to be stored

Returns:

    NTSTATUS

--*/
{
    NTSTATUS    status;

    TRY {
        
        //
        // Store the device context for the current port
        //
        status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                     DeviceContext,
                                                     sizeof(DEVICE_CONTEXT));

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }
                
        if (DeviceContext->CurrentDeviceConfiguration != NULL) {
            PUSB_CONFIGURATION_DESCRIPTOR   configDesc;
            size_t                          configSize;
            PINTERFACE_HANDLE               interfaceHandle;

            configDesc = 
                (PUSB_CONFIGURATION_DESCRIPTOR)&DeviceContext->CurrentDeviceConfiguration->ConfigurationDescriptor;

            configSize = sizeof(DEVICE_CONFIGURATION) - 1 + configDesc->wTotalLength;
            
            //
            // Store the current device configuration
            //
            status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                         DeviceContext->CurrentDeviceConfiguration,
                                                         (ULONG)configSize);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
                
            //
            // Walk the list of current interfaces and store them
            //
            FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                            &DeviceContext->CurrentDeviceConfiguration->InterfaceList,
                            InterfaceLink,
                            interfaceHandle) {

                ULONG   sizeOfInterfaceHandle;

                sizeOfInterfaceHandle = (sizeof(INTERFACE_HANDLE) - sizeof(PIPE_HANDLE)) +
                                        (interfaceHandle->NumberOfEndpoints * sizeof(PIPE_HANDLE));

                status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                             interfaceHandle,
                                                             sizeOfInterfaceHandle);

                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }
            }
        }

        if (DeviceContext->OldDeviceConfiguration != NULL) {
            PUSB_CONFIGURATION_DESCRIPTOR   configDesc;
            size_t                          configSize;
            PINTERFACE_HANDLE               interfaceHandle;

            configDesc = 
                (PUSB_CONFIGURATION_DESCRIPTOR)DeviceContext->OldDeviceConfiguration->ConfigurationDescriptor;

            configSize = sizeof(DEVICE_CONFIGURATION) - 1 + configDesc->wTotalLength;

            //
            // Store the old device configuration
            //
            status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                         DeviceContext->OldDeviceConfiguration,
                                                         (ULONG)configSize);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
                
            //
            // Walk the list of old interfaces and store them
            //
            FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                            &DeviceContext->OldDeviceConfiguration->InterfaceList,
                            InterfaceLink,
                            interfaceHandle) {

                ULONG   sizeOfInterfaceHandle;

                sizeOfInterfaceHandle = (sizeof(INTERFACE_HANDLE) - sizeof(PIPE_HANDLE)) +
                                        (interfaceHandle->NumberOfEndpoints * sizeof(PIPE_HANDLE));

                status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                             interfaceHandle,
                                                             sizeOfInterfaceHandle);

                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }
            }
        }

        if (DeviceContext->OldInterface != NULL) {
            PINTERFACE_HANDLE   interfaceHandle;
            size_t              interfaceSize;

            interfaceHandle = DeviceContext->OldInterface;

            interfaceSize = (sizeof(INTERFACE_HANDLE) - 
                            sizeof(PIPE_HANDLE)) +
                            (interfaceHandle->NumberOfEndpoints * (sizeof(PIPE_HANDLE)));
                                                     
            //
            // store the old interface handle
            //
            status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                         interfaceHandle,
                                                         (ULONG)interfaceSize);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        if (DeviceContext->NewInterface != NULL) {
            PINTERFACE_HANDLE   interfaceHandle;
            size_t              interfaceSize;

            interfaceHandle = DeviceContext->NewInterface;

            interfaceSize = (sizeof(INTERFACE_HANDLE) - 
                             sizeof(PIPE_HANDLE)) +
                            (interfaceHandle->NumberOfEndpoints * (sizeof(PIPE_HANDLE)));

            //
            // store the new interface handle
            //
            status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                         interfaceHandle,
                                                         (ULONG)interfaceSize);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        if ((DeviceContext->EndpointsToBeEnabled != NULL) && 
            (DeviceContext->EndpointsToBeEnabledCount != 0)) {
                    
            //
            // store the array of endpoints to be enabled
            //
            status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                         DeviceContext->EndpointsToBeEnabled,
                                                         sizeof(UCXENDPOINT) * DeviceContext->EndpointsToBeEnabledCount);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        if ((DeviceContext->EndpointsToBeDisabled != NULL) && 
            (DeviceContext->EndpointsToBeDisabledCount != 0)) {

            //
            // store the array of endpoints to be disabled
            //
            status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                         DeviceContext->EndpointsToBeDisabled,
                                                         sizeof(UCXENDPOINT) * DeviceContext->EndpointsToBeDisabledCount);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        if ((DeviceContext->EndpointsToRemainUnchanged != NULL) && 
            (DeviceContext->EndpointsToRemainUnchangedCount != 0)) {

            //
            // store the array of endpoints to remain unchanged
            //
            status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                         DeviceContext->EndpointsToRemainUnchanged,
                                                         sizeof(UCXENDPOINT) * DeviceContext->EndpointsToRemainUnchangedCount);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }
            
        if (DeviceContext->ControlRequest.Irp != NULL) {

            //
            // store the device's control transfer IRP
            //
            status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                         DeviceContext->ControlRequest.Irp,
                                                         DeviceContext->ControlRequest.Irp->Size);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        if (DeviceContext->UcxRequestIrp != NULL) {

            //
            // store the device's UCX IRP
            //
            status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                         DeviceContext->UcxRequestIrp,
                                                         DeviceContext->UcxRequestIrp->Size);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        if (DeviceContext->ConfigurationDescriptor != NULL) {

            //
            // store the device's configuration descriptor
            //
            status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                         DeviceContext->ConfigurationDescriptor,
                                                         DeviceContext->ConfigurationDescriptor->wTotalLength);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        if (DeviceContext->LanguageIdDescriptor != NULL) {

            //
            // store the device's language ID descriptor
            //
            status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                         DeviceContext->LanguageIdDescriptor,
                                                         DeviceContext->LanguageIdDescriptor->bLength);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        if (DeviceContext->ProductIdStringDescriptor != NULL) {

            //
            // store the device's Product ID string descriptor
            //
            status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                         DeviceContext->ProductIdStringDescriptor,
                                                         DeviceContext->ProductIdStringDescriptor->bLength);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        if (DeviceContext->BOSDescriptor != NULL) {

            //
            // store the device's BOS descriptor
            //
            status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                         DeviceContext->BOSDescriptor,
                                                         DeviceContext->BOSDescriptor->wTotalLength);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        if (DeviceContext->PdoContext != NULL) {

            //
            // store the device's PDO context
            //
            status = TelemetryData_InsertTriageDataBlock(TelemetryHandle,
                                                         DeviceContext->PdoContext,
                                                         sizeof(HUB_PDO_CONTEXT));
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }
    } FINALLY {
    }

    return status;
}


NTSTATUS
WER_CreateReport(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in
        PPORT_CONTEXT       PortContext,
    __in
        ULONG               BugCheckParam1,
    __in
        ULONG               BugCheckSubReason
    )
/*++

Routine Description:

    Creates a WER Live Kernel Dump

    All data structures are stored in triage data blocks.  The following data will be stored in the dump:

        USBHUB_LIVEDUMP_CONTEXT - Stores VID/PID/REV of the device (if DeviceContext!=NULL), or the hub (if DeviceContext==NULL),
                                  along with relevant hub, device, and port context addresses.        

        address of global variable storing pointer to WPP control block
        WPP control block
        WPP driver context

        HUB_FDO_CONTEXT
            Hub PDO
            Hub PDO Context
            Hub IFR Log
            Hub Device Context
            Hub Control Request IRP
            Hub Configuration Descriptor
            Hub SymbolicLink Name

        All associated port context structures                
            Port Control Request IRP
            Port IFR Log

            For each valid port context structure the associated device context will be stored, along 
            with the following associated data structures if they exist:
        
                Current Device Configuration
                All Current Interface Handles
                Old Device Configuration
                All Old Interface Handles
                Old Interface
                New Interface
                Array of EndpointsToBeEnabled
                Array of EndpointsToBeDisabled
                Array of EndpointsToRemainUnchanged
                Device Control Request IRP
                UCX Request IRP
                Configuration Descriptor
                Language ID String Descriptor
                Product ID String Descriptor
                BOS Descriptor

            The port's associated PDO context will be save if it exists

Arguments:

    HubFdoContext - Pointer to the hub FDO context

    DeviceContext - Pointer to the device context if live dump is associated with a USB device, NULL otherwise
    
    PortContext - Pointer to the port context if live dump is associated with a port, NULL otherwise

    BugCheckParam1 - Bugcheck parameter 1 - Reason for the live dump
    
    BugCheckSubReason - Sub Reason (optional, can be zero) - Passed as Bugcheck parameter 3
                        (Triage context is passed as Bugcheck parameter 2)

Return Value:

    NTSTATUS

--*/
{
    USBHUB3_LIVEDUMP_CONTEXT    usbhub3LiveDumpContext;
    NTSTATUS                    status;
    HANDLE                      telemetryHandle;
    ULONG                       i;
    PVOID                       ifrLog;
    ULONG                       ifrLogSize;
    PVOID                       wppDriverContext;
    ULONG                       wppDriverContextSize;
    GUID                        wppGuid;
    USHORT                      vendorId;
    USHORT                      productId;
    USHORT                      bcdDevice;
    BOOLEAN                     referencesAcquired;

    TRY {

        telemetryHandle = NULL;
        referencesAcquired = FALSE;

        if (DeviceContext == NULL) {
            //
            // No device context, so check throttle policy for the hub
            //
            status = WER_CheckThrottlePolicy(HubFdoContext->HubFdoDevice, 
                                             BugCheckParam1, 
                                             HubFdoContext->IfrLog);

            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            vendorId = HubFdoContext->HubInfoFromParent.DeviceDescriptor.idVendor;
            productId = HubFdoContext->HubInfoFromParent.DeviceDescriptor.idProduct;
            bcdDevice = HubFdoContext->HubInfoFromParent.DeviceDescriptor.bcdDevice;

        } else {
            //
            // Check throttle policy for the USB device
            //
            status = WER_CheckThrottlePolicy(WdfObjectContextGetObject(DeviceContext->PdoContext), 
                                             BugCheckParam1, 
                                             DeviceContext->PortContext->IfrLog);

            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
            
            vendorId = DeviceContext->DeviceDescriptor.idVendor;
            productId = DeviceContext->DeviceDescriptor.idProduct;
            bcdDevice = DeviceContext->DeviceDescriptor.bcdDevice;
        }

        RtlZeroMemory(&usbhub3LiveDumpContext, sizeof(USBHUB3_LIVEDUMP_CONTEXT));
        
        status = RtlStringCchPrintfA(usbhub3LiveDumpContext.VendorId,
                                     sizeof(usbhub3LiveDumpContext.VendorId),
                                     "%04X",
                                     vendorId);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        status = RtlStringCchPrintfA(usbhub3LiveDumpContext.ProductId,
                                     sizeof(usbhub3LiveDumpContext.ProductId),
                                     "%04X",
                                     productId);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }
                        
        status = RtlStringCchPrintfA(usbhub3LiveDumpContext.BcdDevice,
                                     sizeof(usbhub3LiveDumpContext.BcdDevice),
                                     "%04X",
                                     bcdDevice);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        usbhub3LiveDumpContext.HubFdoContext = HubFdoContext;
        usbhub3LiveDumpContext.DeviceContext = DeviceContext;
        usbhub3LiveDumpContext.PortContext = PortContext;
        
        WdfFdoLockStaticChildListForIteration(HubFdoContext->HubFdoDevice);
        WER_ReferenceDeferenceAllDeviceAndPortContexts(HubFdoContext, TRUE);
        referencesAcquired = TRUE;

        telemetryHandle = TelemetryData_CreateReport(L"USBHUB3",
                                                     BUGCODE_USB3_DRIVER,
                                                     BugCheckParam1,
                                                     (ULONG_PTR)&usbhub3LiveDumpContext,
                                                     BugCheckSubReason,
                                                     0);
                                             
        if (telemetryHandle == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }
    
        //
        // Add the USBHUB3_LIVEDUMP_CONTEXT structure so we can find the hub, device, and 
        // port context addresses when looking at the minidump
        //
        status = TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                     &usbhub3LiveDumpContext,
                                                     sizeof(USBHUB3_LIVEDUMP_CONTEXT));

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // Store the hub FDO context
        //
        status = TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                     HubFdoContext,
                                                     sizeof(HUB_FDO_CONTEXT));

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // Store the global pointer to the WPP control block
        //
        status = TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                     &WPP_GLOBAL_Control,
                                                     sizeof(PVOID));
        
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // Store the WPP control block if it exists
        //
        if (WPP_GLOBAL_Control != NULL) {
            status = TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                         WPP_GLOBAL_Control,
                                                         sizeof(*WPP_GLOBAL_Control));

            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }
        
        //
        // Store the hub's IFR log
        //
        status = WppRecorderLogDumpLiveData(HubFdoContext->IfrLog,
                                            &ifrLog,
                                            &ifrLogSize,
                                            &wppGuid);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        status = TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                     ifrLog,
                                                     ifrLogSize);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }
        
        //
        // Store the WPP driver context
        //
        status = WppRecorderDumpLiveDriverData(&wppDriverContext,
                                               &wppDriverContextSize,
                                               &wppGuid);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        status = TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                     wppDriverContext,
                                                     wppDriverContextSize);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // Store the hub's pdo context and device context if not a root hub
        //
        if ((HubFdoContext->HubParentInterface.HubDepth != 0) && 
            (HubFdoContext->HubPdo != NULL)) {
            PHUB_PDO_CONTEXT    pdoContext;

            status = TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                         HubFdoContext->HubPdo,
                                                         sizeof(DEVICE_OBJECT));

            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            pdoContext = (PHUB_PDO_CONTEXT)HubFdoContext->HubPdo->DeviceExtension;

            if (pdoContext != NULL) {
                status = TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                             pdoContext,
                                                             sizeof(HUB_PDO_CONTEXT));

                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }

                if (pdoContext->AssociatedDSM != NULL) {
                    status = WER_StoreDeviceContext(telemetryHandle,
                                                    pdoContext->AssociatedDSM);

                    if (!NT_SUCCESS(status)) {
                        LEAVE;
                    }
                }
            }
        }

        if (HubFdoContext->HubControlRequest.Irp != NULL) {
            
            //
            // Store the hub control request IRP
            //
            status = TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                         HubFdoContext->HubControlRequest.Irp,
                                                         HubFdoContext->HubControlRequest.Irp->Size);

            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        if (HubFdoContext->ConfigurationDescriptor != NULL) {

            //
            // Store the hub's configuration descriptor
            //
            status = TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                         HubFdoContext->ConfigurationDescriptor,
                                                         HubFdoContext->ConfigurationDescriptor->wTotalLength);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        if (HubFdoContext->HubSymbolicLinkNameUnicode.Buffer != NULL) {

            //
            // Store the hub's symbolic link name
            //
            status = TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                         HubFdoContext->HubSymbolicLinkNameUnicode.Buffer,
                                                         HubFdoContext->HubSymbolicLinkNameUnicode.Length);
                                                         
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        //
        // Enumerate all port contexts
        //
        for (i = 1; i <= HubFdoContext->HubInformation.NumberOfPorts; i++) {
            PPORT_CONTEXT       portContext;
            PDEVICE_CONTEXT     deviceContext;

            portContext = HUBFDO_FindPortContextFromNumber(HubFdoContext, i);
            
            if (portContext == NULL) {
                continue;
            }

            //
            // Store the port context structure
            //
            status = TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                         portContext,
                                                         sizeof(PORT_CONTEXT));
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            if (portContext->PortControlRequest.Irp != NULL) {
                status = TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                             portContext->PortControlRequest.Irp,
                                                             portContext->PortControlRequest.Irp->Size);
        
                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }
            }

            //
            // Store the port's IFR log
            //
            status = WppRecorderLogDumpLiveData(portContext->IfrLog,
                                                &ifrLog,
                                                &ifrLogSize,
                                                &wppGuid);

            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            status = TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                         ifrLog,
                                                         ifrLogSize);

            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            deviceContext = portContext->DeviceContext;
           
            if (deviceContext != NULL) {
                status = WER_StoreDeviceContext(telemetryHandle,
                                                deviceContext);

                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }
            }
        }

    } FINALLY {

        if (referencesAcquired != FALSE) {
            WER_ReferenceDeferenceAllDeviceAndPortContexts(HubFdoContext, FALSE);
            WdfFdoUnlockStaticChildListFromIteration(HubFdoContext->HubFdoDevice);
        }

        if (NT_SUCCESS(status)) {

            status = TelemetryData_SubmitReport(telemetryHandle);

            if (NT_SUCCESS(status)) {

                if (DeviceContext == NULL) {
                    WER_UpdateThrottlePolicy(HubFdoContext->HubFdoDevice, 
                                             BugCheckParam1, 
                                             HubFdoContext->IfrLog);
                } else {
                    WER_UpdateThrottlePolicy(WdfObjectContextGetObject(DeviceContext->PdoContext), 
                                             BugCheckParam1, 
                                             DeviceContext->PortContext->IfrLog);
                }
            }

        } else {
            DbgTraceTo(DeviceContext==NULL ? HubFdoContext->IfrLog : DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failed to create Live Kernel Dump %!STATUS!",
                       status);
        }

        if (telemetryHandle != NULL) {
            TelemetryData_CloseHandle(telemetryHandle);
        }

    }

    return status;
}


