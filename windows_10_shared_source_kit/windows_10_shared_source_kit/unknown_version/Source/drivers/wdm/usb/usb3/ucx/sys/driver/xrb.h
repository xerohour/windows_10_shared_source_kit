//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once

typedef struct _USBDI_HANDLE_DATA{
    UCX_SIG                                               Sig;
    ULONG                                                 USBDClientContractVersion;
    PDEVICE_OBJECT                                        DeviceObject;
    ULONG                                                 PoolTag;
    PVOID                                                 ContextFromClient;
    PUCXUSBDEVICE_PRIVCONTEXT                             UcxUsbDeviceContext;
    WDFMEMORY                                             ThisWdfMemory;
    WDFDEVICE                                             RHPdo;
    LIST_ENTRY                                            USBDInterfaceListEntry;

    ULONG                                                 UsbVerifierFailRegistration;
    ULONG                                                 UsbVerifierFailChainedMdlSupport;
    ULONG                                                 UsbVerifierFailStaticStreamSupport;
    ULONG                                                 UsbVerifierStaticStreamCountOverride;
    ULONG                                                 UsbVerifierFailEnableStaticStreams;
    ULONG                                                 StreamCountGranted;
    
    // List to track XRBs if UsbVerifier is On.
    KSPIN_LOCK                                            XrbListLock;
    LIST_ENTRY                                            XrbListHead;
    
    BOOLEAN                                               StreamSupportGranted;
    BOOLEAN                                               ChainedMdlSupportGranted;
    BOOLEAN                                               UsbVerifierEnabled;
    BOOLEAN                                               UsbVerifierTrackXrbAllocationsEnabled;                                                   
} USBDI_HANDLE_DATA, *PUSBDI_HANDLE_DATA;


typedef enum _XRB_TYPE {
    XrbTypeGeneral = 0xAABB0000,
    XrbTypeSelectConfig,
    XrbTypeSelectInterface,
    XrbTypeIsoch,
    XrbTypeForTransferBuffer,
    XrbTypeForTransferMdl
} XRB_TYPE;

typedef enum _XRB_STATUS {
    XrbStatusInactive = 0xABCD0000,
    XrbStatusActive,
} XRB_STATUS;

typedef struct _XRB_HEADER {
    ULONG                   SizeOfXRB;
    ULONG                   USBDClientContractVersion;
    XRB_TYPE                XrbType;
    XRB_STATUS              XrbStatus;
    PIRP                    Irp;
    PUSBDI_HANDLE_DATA      UsbdiHandleData;
    WDFMEMORY               ThisWdfMemory;
    WDFREQUEST              Request;
    
    // XrbListEntry is only used if USB Verifier is enabled for the client driver. 
    LIST_ENTRY              XrbListEntry;
    
    
    
    // !! IMPORTANT !!
    // DO NOT CHANGE THIS STRUCTURE BEYOND THIS POINT. 
    // The following fields may be used by USBDEX.lib to extend the non-XRB Urbs 
    // Currently usbdex.lib is not extending the URB
    //
    PVOID                   Reserved;
    // Keep Sig as last field, introduce a PAD on 64 Bit 
    // We use it to check if client driver did a buffer underrun
#if defined (_WIN64)
    ULONG                   ReservedPad;
#endif
    UCX_SIG                 Sig;
} XRB_HEADER, *PXRB_HEADER;

typedef struct _XRB {
    XRB_HEADER   XrbHeader;    
    URB          Urb;
} XRB, *PXRB;

__drv_maxIRQL(DISPATCH_LEVEL)
_Must_inspect_result_
NTSTATUS
Xrb_Allocate(
    __in
        USBDI_HANDLE         USBDInterfaceHandle,
    __out
        PURB                *Urb
);

__drv_maxIRQL(DISPATCH_LEVEL)
_Must_inspect_result_
NTSTATUS
Xrb_IsochAllocate(
    __in
        USBDI_HANDLE         USBDInterfaceHandle,
    __in
        ULONG                NumberOfIsochPackets,
    __out
        PURB                *Urb
);

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Xrb_Free(
    _At_(CONTAINING_RECORD(Urb, XRB, Urb), __in)
        PURB                Urb
);

__drv_maxIRQL(DISPATCH_LEVEL)
_Must_inspect_result_
NTSTATUS
Xrb_SelectConfigAllocateAndBuild(
    __in
        USBDI_HANDLE                  USBDInterfaceHandle,
    __in
        PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor,
    __in
        PUSBD_INTERFACE_LIST_ENTRY    InterfaceList,
    __out
        PURB                         *Urb
);

__drv_maxIRQL(DISPATCH_LEVEL)
_Must_inspect_result_
NTSTATUS
Xrb_SelectInterfaceAllocateAndBuild(
    __in
        USBDI_HANDLE                  USBDInterfaceHandle,
    __in
        USBD_CONFIGURATION_HANDLE     ConfigurationHandle,
    __in
        PUSBD_INTERFACE_LIST_ENTRY    InterfaceListEntry,
    __out
        PURB                         *Urb
);

FORCEINLINE
BOOLEAN
IsStreamPipe(
    __in
        PUCXPIPE_INFO UcxPipeInfo
    )
{
    return UcxPipeInfo->IsStreamPipe;
}

FORCEINLINE
BOOLEAN
IsUrbXrb(
    __in
        PURB               Urb,
    __in
        PIO_STACK_LOCATION CurrentIrpStackLocation
    )
{
    return ((PVOID)Urb == (PVOID)CurrentIrpStackLocation->FileObject);
}

FORCEINLINE
PXRB
UrbToXrb(
    __in
        PURB               Urb
    )
{
    return CONTAINING_RECORD(Urb, XRB, Urb);
}

FORCEINLINE
VOID
Xrb_OnDispatch(
    __in
        PIRP               Irp,
    __in
        PXRB               Xrb
    )
/*++
Routine Description:

    This routine is called when UCX recieves a new IOCTL_INTERNAL_USB_SUBMIT_URB with an XRB.
    This routine does the following validations on Xrb
    
    - Catches Double Urb Submission cases by tracking the state of the Xrb

--*/
    
{
    TRY {
        
        if (Xrb->XrbHeader.Sig != SigXrb) {
            //
            // Client driver has either submitted an Urb as Xrb 
            // or did a buffer underrun on the Urb (Xrb)
            //
            Usb3_KeBugCheckEx(USB3_BUGCODE_CLIENT_CORRUPTED_XRB,
                              Irp,
                              &Xrb->Urb,
                              Xrb->XrbHeader.UsbdiHandleData->DeviceObject);
        }

        if (Xrb->XrbHeader.XrbStatus != XrbStatusInactive) {
            
            //
            // Client driver has resubmitted the Xrb that he had submitted us previously. 
            // Bugcheck explicitly in this case (so that we don't have to deal with a 
            // more implicit bugcheck that is hard to evaluate)
            //
            Usb3_KeBugCheckEx(USB3_BUGCODE_CLIENT_USED_AN_ACTIVE_URB,
                              Irp,
                              &Xrb->Urb,
                              Xrb->XrbHeader.UsbdiHandleData->DeviceObject);

        }

        Xrb->XrbHeader.XrbStatus = XrbStatusActive;

    } FINALLY {

    }
}

FORCEINLINE
VOID
Xrb_OnComplete(
    __in
        PXRB               Xrb
    )
{
    TRY {
        
        NT_ASSERT(Xrb->XrbHeader.XrbStatus == XrbStatusActive);
        Xrb->XrbHeader.XrbStatus = XrbStatusInactive;

    } FINALLY {

    }
}

FORCEINLINE
USHORT
XrbSizeToUrbSize(
    __in
        USHORT             Size
    )
{
    return (Size - sizeof(XRB_HEADER));
}

FORCEINLINE 
VOID
Xrb_IoCompleteRequest(
    __in
        PIRP         Irp,
    __in
        PURB         Urb,
    __in
        NTSTATUS     Status,
    __in
        USBD_STATUS  UsbdStatus
    )
{
    PXRB               xrb;
    PIO_STACK_LOCATION irpStack;

    TRY {

        irpStack = IoGetCurrentIrpStackLocation(Irp);
        
        if (IsUrbXrb(Urb, irpStack)){
            
            xrb = UrbToXrb(Urb);
            Xrb_OnComplete(xrb);
        }

        Urb->UrbHeader.Status = UsbdStatus;
        Irp->IoStatus.Status = Status;
        Irp->IoStatus.Information = 0;

        IoCompleteRequest(Irp, IO_NO_INCREMENT);

    } FINALLY {

    }
}

FORCEINLINE 
VOID
Xrb_IoCompleteRequest_OnDispatchIfNonXrb(
    __in
        PIRP         Irp,
    __in
        PURB         Urb,
    __in
        NTSTATUS     Status,
    __in
        USBD_STATUS  UsbdStatus
    )
/*++

Routine Description:
    
COMPAT: 
    2.0 Stack completes certain requests from DPC routine, thus ensuring that the 
    completion irql will be DISPATCH_LEVEL
    
    This routine completes the non-XRB Urb on DISPATCH_LEVEL
    
--*/
{
    PXRB               xrb;
    PIO_STACK_LOCATION irpStack;
    BOOLEAN            isUrbXrb;
    KIRQL              irql = 0;

    TRY {

        irpStack = IoGetCurrentIrpStackLocation(Irp);
        
        isUrbXrb = IsUrbXrb(Urb, irpStack);

        if (isUrbXrb){
            xrb = UrbToXrb(Urb);
            Xrb_OnComplete(xrb);
        } 

        Urb->UrbHeader.Status = UsbdStatus;
        Irp->IoStatus.Status = Status;
        Irp->IoStatus.Information = 0;

        if (isUrbXrb) {            
            IoCompleteRequest(Irp, IO_NO_INCREMENT);        
        } else {
            KeRaiseIrql(DISPATCH_LEVEL, &irql);
            IoCompleteRequest(Irp, IO_NO_INCREMENT);            
            KeLowerIrql(irql);
        }

    } FINALLY {

    }
}

FORCEINLINE 
VOID
Xrb_IoCompleteRequest_StatusAlreadySet(
    __in
        PIRP         Irp
    )
{
    PIO_STACK_LOCATION irpStack;
    PURB               urb;
    PXRB               xrb;

    TRY {

        irpStack = IoGetCurrentIrpStackLocation(Irp);
        NT_ASSERT(irpStack->Parameters.DeviceIoControl.IoControlCode 
                  == IOCTL_INTERNAL_USB_SUBMIT_URB);        
        urb = (PURB)irpStack->Parameters.Others.Argument1;
        
        if (IsUrbXrb(urb, irpStack)){            
            xrb = UrbToXrb(urb);
            Xrb_OnComplete(xrb);
        }

        IoCompleteRequest(Irp, IO_NO_INCREMENT);

    } FINALLY {

    }
}

IO_COMPLETION_ROUTINE Xrb_TransferUrbCompletion;
IO_COMPLETION_ROUTINE Xrb_UrbCompletion;

VOID
LogWppMsgForCloseStaticStreamOnDx(
    __in
        PIRP            Irp
    );

//
// This function prototype is also defined in etw.h, but must be reproduced here because the
// function is used in the below inline function, and order of includes already dictates that etw.h
// must be included after this file.
//
VOID
Etw_BulkTransferXrbDispatch(
    __in
        PIRP            Irp,
    __in
        PURB            Urb
    );

FORCEINLINE
_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Xrb_ProcessURB(
    __in
        WDFDEVICE          RHPdo,
    __in
        PIRP               Irp,
    __in
        PIO_STACK_LOCATION CurrentIrpStackLocation,
    __in
        PXRB               Xrb,
    __in
        PURB               Urb
    )
/*++

Routine Description:

    Processes a URB from a function-driver IRP.

    This routine only processes Urb's that can be subject to a contract
    newer than the 2.0 stack.

    Essentially what we do here is look at the URB and validate some
    of the the parameters for the function-driver.

Arguments:

    WdfDevice - The WDFDEVICE handle for the RootHub Pdo

    Irp -  IO request block

    CurrentIrpStackLocation - Curent stack Location.

    Urb -  ptr to USB request block

Return Value:

--*/
{
    NTSTATUS                                    status;
    PUCXPIPE_INFO                               ucxPipeInfo;
    PTRANSFER_URB                               transferUrb;
    PUCXENDPOINT_PRIVCONTEXT                    ucxEndpointContext;
    PUCXPIPE_INFO                               endpointPipeInfo;

    UNREFERENCED_PARAMETER(CurrentIrpStackLocation);

    TRY {

        switch (Urb->UrbHeader.Function) {

        case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_USING_CHAINED_MDL:
        case URB_FUNCTION_ISOCH_TRANSFER_USING_CHAINED_MDL:
            if (FALSE == Xrb->XrbHeader.UsbdiHandleData->ChainedMdlSupportGranted) {
                if (Xrb->XrbHeader.UsbdiHandleData->UsbVerifierEnabled) {
                    Usb3_KeBugCheckEx(
                        USB3_VERIFIER_BUGCODE_CHAINED_MDL_WITHOUT_QUERYING_FOR_CAPABILITY,
                        Irp,
                        &Xrb->Urb,
                        Xrb->XrbHeader.UsbdiHandleData->DeviceObject);                                            
                } 
                
                //
                // FAIL REQUEST
                // 
                status = STATUS_INVALID_PARAMETER;
                Xrb_IoCompleteRequest(Irp, Urb, status, NTSTATUS_To_USBD_STATUS(status));
                LEAVE;
            }
            
            // Using a goto Here to avoid addition check in the hot path. 
            goto case_BULK_INTERRUPT_ISOCH_WITH_OR_WITHOUT_CHAINED_MDL;

        case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
        case URB_FUNCTION_ISOCH_TRANSFER:

#if 0 // Disable to unblock storage team. More detail in Windows 8 Bugs : 504192
            transferUrb = (PTRANSFER_URB) Urb;
            if (transferUrb->TransferBufferMDL &&
                (transferUrb->TransferBufferLength
                 > MmGetMdlByteCount(transferUrb->TransferBufferMDL))) {

                if (Xrb->XrbHeader.UsbdiHandleData->UsbVerifierEnabled ||
                    Xrb->XrbHeader.UsbdiHandleData->UcxUsbDeviceContext->UcxControllerContext->DriverVerifierEnabled) {
                    
                    Usb3_KeBugCheckEx(USB3_VERIFIER_BUGCODE_CLIENT_USED_CHAINED_MDL,
                                      transferUrb->TransferBufferMDL,
                                      transferUrb,
                                      Xrb->XrbHeader.UsbdiHandleData->DeviceObject);
                }

                status = STATUS_INVALID_PARAMETER;
                Xrb_IoCompleteRequest(Irp, Urb, status, NTSTATUS_To_USBD_STATUS(status));
                LEAVE;
            }
#endif

        case_BULK_INTERRUPT_ISOCH_WITH_OR_WITHOUT_CHAINED_MDL: 

            //
            // Initialize the URB status to USBD_STATUS_PENDING.
            //
            Urb->UrbHeader.Status = USBD_STATUS_PENDING;
        
            transferUrb = (PTRANSFER_URB) Urb;
            //
            // Retrieve the pipe handle from the URB.
            //
            ucxPipeInfo = (PUCXPIPE_INFO) transferUrb->UsbdPipeHandle;

            if (ucxPipeInfo->IsEndpointDirectionIn) {
                SET_TRANSFER_DIRECTION_IN(&transferUrb->TransferFlags);
            } else {
                SET_TRANSFER_DIRECTION_OUT(&transferUrb->TransferFlags);
            }
            
            transferUrb->UrbData.ProcessorNumber = KeGetCurrentProcessorNumberEx(NULL);

            Etw_BulkTransferXrbDispatch(Irp, Urb);

            if (Xrb->XrbHeader.Request) {                
                WDF_REQUEST_REUSE_PARAMS reuseParams;
                WDF_REQUEST_REUSE_PARAMS_INIT(&reuseParams, 
                                              WDF_REQUEST_REUSE_SET_NEW_IRP | WDF_REQUEST_REUSE_MUST_COMPLETE, 
                                              STATUS_SUCCESS);
                reuseParams.NewIrp = Irp;
                WdfRequestReuse(Xrb->XrbHeader.Request, &reuseParams);
            
                status = UcxForwardIrpToQueueWithCompletionUsingRequest(RHPdo,
                                                                        Irp,
                                                                        Xrb->XrbHeader.Request,
                                                                        Xrb_TransferUrbCompletion,
                                                                        Urb,
                                                                        ucxPipeInfo->Queue);
            } else {
                
                status = UcxForwardIrpToQueueWithCompletion(RHPdo,
                                                            Irp,
                                                            Xrb_TransferUrbCompletion,
                                                            Urb,
                                                            ucxPipeInfo->Queue);
            }

            break;
        
        case URB_FUNCTION_OPEN_STATIC_STREAMS: 

            //
            // We create the streams object from our pre-processor routine itself, while we are on
            // PASSIVE_LEVEL
            //
            endpointPipeInfo = Urb->UrbOpenStaticStreams.PipeHandle;
            ucxEndpointContext = UcxEndpointGetContextFromEndpointPipeInfo(endpointPipeInfo);

            status = Endpoint_StaticStreamsEnableUrb(ucxEndpointContext, RHPdo, Xrb, Irp);

            break;

        case URB_FUNCTION_CLOSE_STATIC_STREAMS: 
            
            endpointPipeInfo = Urb->UrbOpenStaticStreams.PipeHandle;
            ucxEndpointContext = UcxEndpointGetContextFromEndpointPipeInfo(endpointPipeInfo);

            //
            // Hack: In case the controller is selectively suspended, it is possible that the 
            // client driver got a surprise remove and it sent a 
            // URB_FUNCTION_CLOSE_STATIC_STREAMS request down to the core stack. 
            // Ideally we should call WdfDeviceStopIdle here, however since the 
            // closer static streams operation is optional anyway, we will just complete it 
            // here synchronously. 
            //
            // if the client driver tried to open static streams again it would fail.
            // However a correctly written client driver wouldn't do that since client drivers are 
            // only supposed to close streams when the are in D0 or during removal. In the 
            // removal case it doesnt makes sense to try to reopen streams.
            //
            // Please note that This check does not prevent the controller from entering Dx right
            // after we see the root hub in D0, therefore we could end up touching the hardware in 
            // Dx. The complexity of fixing this is too high for Win8.  
            // Mitigation: the window of time for this power transition should be small, and it is 
            // unnecessary for clients to close streams while cleaning up. If client is not 
            // cleaning up, it should be in D0 which would mean the controller is staying in D0.
            //
            if (ucxEndpointContext->UcxControllerContext->RHPdoInD0 == FALSE) {
                status = STATUS_SUCCESS;
                LogWppMsgForCloseStaticStreamOnDx(Irp);
                Xrb_IoCompleteRequest(Irp, Urb, status, NTSTATUS_To_USBD_STATUS(status));
                break;
            }


            status = Endpoint_StaticStreamsDisableUrb(ucxEndpointContext, RHPdo, Xrb, Irp);

            break;

        default:

            //  
            // For now, mimic behavior of USBPORT_ProcessURB, to handle this request.
            // In future, however for newer client driver we may need to reimplement more code here.
            //
            status = Urb_USBPORTStyle_ProcessURB(RHPdo, Irp, Urb);

        }

    } FINALLY {

    }
    return status;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
NTSTATUS
Xrb_ProcessSubmitUrbIoctl(
    __in
        WDFDEVICE          RHPdo,
    __in
        PIRP               Irp,
    __in
        PIO_STACK_LOCATION CurrentIrpStackLocation
    )
/*++

Routine Description:

    A routine to handle the IOCTL_INTERNAL_USB_SUBMIT_URB.
    Processes a URB from a function-driver IRP.

    This function basically determines if the Legacy version of Process URB be called or the new
    version

Arguments:

    WdfDevice - The WDFDEVICE handle for the RootHub Pdo

    Irp -  IO request block

    CurrentIrpStackLocation - Curent stack Location.

Return Value:

--*/
{
    NTSTATUS            status;
    PURB                urb;
    PXRB                xrb;

    TRY {

        urb = CurrentIrpStackLocation->Parameters.Others.Argument1;

        if (IsUrbXrb(urb, CurrentIrpStackLocation)) {
            xrb = UrbToXrb(urb);

            Xrb_OnDispatch(Irp, xrb);
            
            if (xrb->XrbHeader.UsbdiHandleData->USBDClientContractVersion
                >= USBD_CLIENT_CONTRACT_VERSION_602) {
                //
                // Change this to use XRBs
                //
                status = Xrb_ProcessURB(RHPdo, Irp, CurrentIrpStackLocation, xrb, urb);
                LEAVE;
            }
        }

        //
        // Mimic behavior of USBPORT_ProcessURB, to handle this request.
        //
        status = Urb_USBPORTStyle_ProcessURB(RHPdo, Irp, urb);

    } FINALLY {

    }
    return status;
}



