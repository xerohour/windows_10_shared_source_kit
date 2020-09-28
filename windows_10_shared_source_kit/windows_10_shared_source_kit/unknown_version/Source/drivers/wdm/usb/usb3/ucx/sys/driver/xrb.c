/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    xrb.c

Abstract:

    This file implements the XRB support for the USB3 stack.

Environment:

    kernel mode only

Notes:

Revision History:

--*/

#include "pch.h"

#include "xrb.tmh"

volatile BOOLEAN g_EnablePreallocatedRequestForXrbs = TRUE;

FORCEINLINE
VOID
Xrb_AddToList(
    __in
        PUSBDI_HANDLE_DATA UsbdiHandleData,
    __in
        PXRB               Xrb
    )
/*++
    If the USB Verifier is enabled for the client driver, track this XRB in a list.
--*/
{
    KIRQL irql;

    TRY {
        if (FALSE == UsbdiHandleData->UsbVerifierTrackXrbAllocationsEnabled) {
            LEAVE;
        }

        KeAcquireSpinLock(&UsbdiHandleData->XrbListLock, &irql);
        InsertTailList(&UsbdiHandleData->XrbListHead,
                       &Xrb->XrbHeader.XrbListEntry);
        KeReleaseSpinLock(&UsbdiHandleData->XrbListLock, irql);

    } FINALLY {
    }

    return;
}

FORCEINLINE
VOID
Xrb_RemoveFromList(
    __in
        PUSBDI_HANDLE_DATA UsbdiHandleData,
    __in
        PXRB               Xrb
    )
/*++
    If the USB Verifier is enabled for the client driver, track this XRB in a list.
--*/
{
    KIRQL irql;

    TRY {
        if (FALSE == UsbdiHandleData->UsbVerifierTrackXrbAllocationsEnabled) {
            LEAVE;
        }

        KeAcquireSpinLock(&UsbdiHandleData->XrbListLock, &irql);
        RemoveEntryList(&Xrb->XrbHeader.XrbListEntry);
        InitializeListEntry(&Xrb->XrbHeader.XrbListEntry);
        KeReleaseSpinLock(&UsbdiHandleData->XrbListLock, irql);

    } FINALLY {
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
_Must_inspect_result_
NTSTATUS
Xrb_Allocate(
    __in
        USBDI_HANDLE         USBDInterfaceHandle,
    __out
        PURB                *Urb
)
/*++

Routine Description:

    This routine is called by the client driver to allocate an XRB.

Arguments:

    USBDInterfaceHandle - USBDI_HANDLE returned to the client during the Query Interface exchange.

    Urb - Output allocated Xrb (Urb)

Returns:

    NTSTATUS

Comments:

    The Xrb allocated by this routine MUST be freed by Xrb_Free routine.

--*/
{
    NTSTATUS                status;
    PXRB                    xrb;
    PUSBDI_HANDLE_DATA      usbdiHandleData;
    WDF_OBJECT_ATTRIBUTES   objectAttributes;
    WDFMEMORY               xrbWdfMemory=NULL;
    WDFREQUEST              request=NULL;
    PRHPDO_CONTEXT          rhPdoContext;

    TRY {
        UcxPointerNotNull(USBDInterfaceHandle);
        UcxPointerNotNull(Urb);

        *Urb = NULL;

        usbdiHandleData = (PUSBDI_HANDLE_DATA) USBDInterfaceHandle;

        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
        objectAttributes.ParentObject = usbdiHandleData->ThisWdfMemory;

        status = WdfMemoryCreate(&objectAttributes,
                                 NonPagedPool,
                                 usbdiHandleData->PoolTag,
                                 sizeof(XRB),
                                 &xrbWdfMemory,
                                 &xrb);

        if (!NT_SUCCESS(status)){
            LogError(FlagUsbdi, "WdfMemoryCreate failed, USBDI_HANDLE_DATA 0x%p, %!STATUS!",
                     usbdiHandleData, status);
            LEAVE;
        }

        WdfObjectReferenceWithTag(xrbWdfMemory, TAG(Xrb_Free));

        RtlZeroMemory(xrb, sizeof(XRB));

        xrb->XrbHeader.Sig = SigXrb;
        xrb->XrbHeader.SizeOfXRB = sizeof(*xrb);
        xrb->XrbHeader.USBDClientContractVersion = usbdiHandleData->USBDClientContractVersion;
        xrb->XrbHeader.UsbdiHandleData = usbdiHandleData;
        xrb->XrbHeader.XrbStatus = XrbStatusInactive;
        xrb->XrbHeader.XrbType = XrbTypeGeneral;
        xrb->XrbHeader.ThisWdfMemory = xrbWdfMemory;

        if (g_EnablePreallocatedRequestForXrbs) {

            rhPdoContext = RHPdoGetContext(usbdiHandleData->RHPdo);

            RtlCopyMemory(&objectAttributes,
                          &rhPdoContext->UcxRootHubContext->Config.WdfRequestAttributes,
                          sizeof(objectAttributes));

            objectAttributes.ParentObject = usbdiHandleData->RHPdo;

            status = WdfRequestCreate(&objectAttributes, NULL, &request);

            if (!NT_SUCCESS(status)){
                LogError(FlagUsbdi, "WdfRequestCreate failed, USBDI_HANDLE_DATA 0x%p, %!STATUS!",
                         usbdiHandleData, status);
                LEAVE;
            }

            xrb->XrbHeader.Request = request;

        }

        Xrb_AddToList(usbdiHandleData, xrb);
    } FINALLY {

        if (NT_SUCCESS(status)) {
            *Urb = &xrb->Urb;
        } else {
            if (xrbWdfMemory != NULL) {
                WdfObjectDelete(xrbWdfMemory);
                WdfObjectDereferenceWithTag(xrbWdfMemory, TAG(Xrb_Free));
            }
            if (request != NULL) {
                WdfObjectDelete(request);
            }
        }
    }
    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__checkReturn
NTSTATUS
Xrb_IsochAllocate(
    __in
        USBDI_HANDLE         USBDInterfaceHandle,
    __in
        ULONG                NumberOfIsochPackets,
    __out
        PURB                *Urb
)
/*++

Routine Description:

    This routine is called by the client driver to allocate an Isoch XRB.

Arguments:

    USBDInterfaceHandle - USBDI_HANDLE returned to the client during the Query Interface exchange.

    NumberOfIsochPackets - Maximum Number of Isoch Packets that will be programmed into that Urb

    Urb - Output allocated Xrb (Urb)

Returns:

    NTSTATUS

Comments:

    The Xrb allocated by this routine MUST be freed by Xrb_Free routine.

--*/
{
    NTSTATUS                status;
    ULONG                   size;
    PXRB                    xrb;
    PUSBDI_HANDLE_DATA      usbdiHandleData;
    WDF_OBJECT_ATTRIBUTES   objectAttributes;
    WDFMEMORY               xrbWdfMemory=NULL;

    TRY {
        UcxPointerNotNull(USBDInterfaceHandle);
        UcxPointerNotNull(Urb);

        *Urb = NULL;

        usbdiHandleData = (PUSBDI_HANDLE_DATA) USBDInterfaceHandle;

        size = sizeof(XRB_HEADER) + GET_ISO_URB_SIZE(NumberOfIsochPackets);

        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
        objectAttributes.ParentObject = usbdiHandleData->ThisWdfMemory;

        status = WdfMemoryCreate(&objectAttributes,
                                 NonPagedPool,
                                 usbdiHandleData->PoolTag,
                                 size,
                                 &xrbWdfMemory,
                                 &xrb);

        if (!NT_SUCCESS(status)){
            LogError(FlagUsbdi, "WdfMemoryCreate failed, USBDI_HANDLE_DATA 0x%p, %!STATUS!",
                     usbdiHandleData, status);
            LEAVE;
        }

        WdfObjectReferenceWithTag(xrbWdfMemory, TAG(Xrb_Free));

        RtlZeroMemory(xrb, size);

        xrb->XrbHeader.Sig = SigXrb;
        xrb->XrbHeader.SizeOfXRB = size;
        xrb->XrbHeader.USBDClientContractVersion = usbdiHandleData->USBDClientContractVersion;
        xrb->XrbHeader.UsbdiHandleData = usbdiHandleData;
        xrb->XrbHeader.XrbStatus = XrbStatusInactive;
        xrb->XrbHeader.XrbType = XrbTypeIsoch;
        xrb->XrbHeader.ThisWdfMemory = xrbWdfMemory;

        Xrb_AddToList(usbdiHandleData, xrb);
    } FINALLY {

        if (NT_SUCCESS(status)) {
            *Urb = &xrb->Urb;
        } else {
            if (xrbWdfMemory != NULL) {
                WdfObjectDelete(xrbWdfMemory);
                WdfObjectDereferenceWithTag(xrb->XrbHeader.ThisWdfMemory, TAG(Xrb_Free));
            }
        }
    }
    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Xrb_Free(
    _At_(CONTAINING_RECORD(Urb, XRB, Urb), __in)
        PURB                Urb
)
/*++

Routine Description:

    This routine is called by the client driver to free an XRB, previously allocated by one of
    the following routines: Xrb_Allocate, Xrb_IsochAllocate, Xrb_SelectInterfaceAllocateAndBuild or
    USBDI_SelectInterfaceXrbAllocateAndBuild

Arguments:

    USBDInterfaceHandle - USBDI_HANDLE returned to the client during the Query Interface exchange.

    Urb - Urb (XRB) to be freed.

--*/
{
    PXRB  xrb;
    TRY {
        UcxPointerNotNull(Urb);

        xrb = CONTAINING_RECORD(Urb, XRB, Urb);

        if (xrb->XrbHeader.Sig != SigXrb) {
            Usb3_KeBugCheckEx(USB3_BUGCODE_CLIENT_CORRUPTED_XRB,
                              NULL,
                              Urb,
                              xrb->XrbHeader.UsbdiHandleData->DeviceObject);
        }

        if (xrb->XrbHeader.XrbStatus != XrbStatusInactive) {
            Usb3_KeBugCheckEx(USB3_BUGCODE_CLIENT_USED_AN_ACTIVE_URB,
                              NULL,
                              Urb,
                              xrb->XrbHeader.UsbdiHandleData->DeviceObject);
        }

        Xrb_RemoveFromList(xrb->XrbHeader.UsbdiHandleData, xrb);

        if (xrb->XrbHeader.Request) {
            WdfObjectDelete(xrb->XrbHeader.Request);
            xrb->XrbHeader.Request = NULL;
        }

        WdfObjectDelete(xrb->XrbHeader.ThisWdfMemory);
        WdfObjectDereferenceWithTag(xrb->XrbHeader.ThisWdfMemory, TAG(Xrb_Free));


    } FINALLY {
    }
    return;
}

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
    )
/*++

Routine Description:

    This routine is called by the client driver to allocate and initialize a select configuration
    urb

    This function is a derivative of USBD_CreateConfigurationRequestEx

    In this function we create an XRB of the proper
    size and format for a select_configuration request.

    This function uses the configurstion descritor as a
    reference and builds a URB with interface_information
    structures for each interface requested in the interface
    list passed in

    The config descriptor may contain interfaces that
    the caller does not specify in the list -- in this case
    the other interfaces will be ignored.

Arguments:

    USBDInterfaceHandle - USBDI_HANDLE returned to the client during the Query Interface exchange.

    ConfigurationDescriptor - Pointer to the configuration descriptor

    InterfaceList - An array of USBD_INTERFACE_LIST_ENTRY structures. The client driver must
        the InterfaceDescriptor member of each element of the array. The last element of the array
        must have a NULL InterfaceDescriptor

    Urb - Output allocated and inialized Select config urb (Xrb)


Return Value:

    NTSTATUS

--*/
{
    ULONG                             numInterfaces;
    ULONG                             numPipes;
    ULONG                             totalPipes;
    USHORT                            size;
    USHORT                            ifaceLen;
    NTSTATUS                          status;
    PXRB                              xrb=NULL;
    PUSBDI_HANDLE_DATA                usbdiHandleData;
    WDF_OBJECT_ATTRIBUTES             objectAttributes;
    PUSBD_INTERFACE_LIST_ENTRY        ifaceList;
    PUSBD_INTERFACE_INFORMATION       ifaceInfo;
    PUSB_INTERFACE_DESCRIPTOR         ifaceDescriptor;
    ULONG                             index;
    WDFMEMORY                         xrbWdfMemory=NULL;
    BOOLEAN                           clientContractV1;


    TRY {


        UcxPointerNotNull(USBDInterfaceHandle);
        UcxPointerNotNull(InterfaceList);
        UcxPointerNotNull(Urb);

        *Urb = NULL;

        usbdiHandleData = (PUSBDI_HANDLE_DATA) USBDInterfaceHandle;
        clientContractV1 = (usbdiHandleData->USBDClientContractVersion
                            >= USBD_CLIENT_CONTRACT_VERSION_602) ? TRUE : FALSE;
        //
        // Figure out how many interfaces we are dealing with, and calculate the size of
        // memory we need to allocate
        //

        ifaceList = InterfaceList;

        size = sizeof(XRB_HEADER)
               + sizeof(struct _URB_SELECT_CONFIGURATION)
               - sizeof(USBD_INTERFACE_INFORMATION);

        numInterfaces = 0;
        totalPipes = 0;

        while (ifaceList->InterfaceDescriptor) {

            numPipes = ifaceList->InterfaceDescriptor->bNumEndpoints;
            totalPipes += numPipes;

            if (totalPipes > MAX_ENDPOINTS_PER_DEVICE) {
                LogError(FlagUsbdi, "Too Many Pipes. Possibly detected Infinite loop");
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }

            if (clientContractV1 && ifaceList->Interface != NULL) {
                LogError(FlagUsbdi, "ifaceList(0x%p)->Interface should be null, usbdiHandleData 0x%p",
                         ifaceList, usbdiHandleData);
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }

            size += (USHORT) GET_USBD_INTERFACE_SIZE(numPipes);
            ifaceList++;

            if (numInterfaces++ > MAX_INTERFACES_PER_DEVICE) {
                LogError(FlagUsbdi, "Too Many Interfaces. Possibly detected Infinite loop");
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }
        }

        if (clientContractV1 && ifaceList->Interface != NULL) {
            LogError(FlagUsbdi, "ifaceList(0x%p)->Interface should be null, usbdiHandleData 0x%p",
                     ifaceList, usbdiHandleData);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
        objectAttributes.ParentObject = usbdiHandleData->ThisWdfMemory;

        status = WdfMemoryCreate(&objectAttributes,
                                 NonPagedPool,
                                 usbdiHandleData->PoolTag,
                                 size,
                                 &xrbWdfMemory,
                                 &xrb);

        if (!NT_SUCCESS(status)){
            LogError(FlagUsbdi, "WdfMemoryCreate failed, USBDI_HANDLE_DATA 0x%p, %!STATUS!",
                     usbdiHandleData, status);
            LEAVE;
        }

        WdfObjectReferenceWithTag(xrbWdfMemory, TAG(Xrb_Free));

        RtlZeroMemory(xrb, size);

        xrb->XrbHeader.Sig = SigXrb;
        xrb->XrbHeader.SizeOfXRB = size;
        xrb->XrbHeader.USBDClientContractVersion = usbdiHandleData->USBDClientContractVersion;
        xrb->XrbHeader.UsbdiHandleData = usbdiHandleData;
        xrb->XrbHeader.XrbStatus = XrbStatusInactive;
        xrb->XrbHeader.XrbType = XrbTypeSelectConfig;
        xrb->XrbHeader.ThisWdfMemory = xrbWdfMemory;

        //
        // Walk throuth the Interface List and build the Select Configuration request.
        //
        ifaceInfo = &xrb->Urb.UrbSelectConfiguration.Interface;
        ifaceList = InterfaceList;

        while (ifaceList->InterfaceDescriptor) {

            ifaceDescriptor = ifaceList->InterfaceDescriptor;

            #pragma prefast(suppress: 26000, "BUG - ESP:675")
            ifaceInfo->InterfaceNumber = ifaceDescriptor->bInterfaceNumber;
            ifaceInfo->AlternateSetting = ifaceDescriptor->bAlternateSetting;

            //
            // Getting a descriptor usually involves two requests. On the first one, the device sets 
            // the total length for the full descriptor. The caller uses this length to allocate memory 
            // for the descriptor, and then makes a second request for the full descriptor. 
            // On this second request, a malicious or buggy device could send a different length. 
            // Assuming that this new length has not changed (as expected) could lead into parsing 
            // beyond the allocated memory when this new length is larger than the allocated memory,
            // potentially accessing data beyond our control.
            //
            // If the value of bNumEndpoints lies beyond the descriptor's allocated memory, its contents 
            // might change and bNumEndpoints can become larger.
            // If this happens, writing beyond xrb's allocated memory could cause a Write AV.
            //
            // To prevent this, we’ll cache bNumEndpoints in numPipes, and we'll verify that 
            // the pipes still fit within our allocated memory.
            //

            numPipes = ifaceDescriptor->bNumEndpoints;
            ifaceLen = (USHORT) GET_USBD_INTERFACE_SIZE(numPipes);

            if (((PUCHAR) ifaceInfo) + ifaceLen > ((PUCHAR) xrb) + size) {
                LogError(FlagUsbdi, "Unexpected increase in number of pipes.");
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }

            ifaceInfo->NumberOfPipes = numPipes;
            ifaceInfo->Length = ifaceLen;

            for (index=0; index < numPipes; index++) {
                ifaceInfo->Pipes[index].MaximumTransferSize = USBD_DEFAULT_MAXIMUM_TRANSFER_SIZE;
                ifaceInfo->Pipes[index].PipeFlags = 0;
            }


            ifaceList->Interface = ifaceInfo;

            ifaceList++;
            ifaceInfo = (PUSBD_INTERFACE_INFORMATION) ((PUCHAR) ifaceInfo + ifaceLen);

        }

        xrb->Urb.UrbHeader.Length = XrbSizeToUrbSize(size);
        xrb->Urb.UrbHeader.Function = URB_FUNCTION_SELECT_CONFIGURATION;
        xrb->Urb.UrbSelectConfiguration.ConfigurationDescriptor = ConfigurationDescriptor;

        #if DBG
        ifaceList = InterfaceList;
        while (ifaceList->InterfaceDescriptor) {
            LogVerbose(FlagUsbdi, "InterfaceList, InterfaceInfo = 0x%p",
                       ifaceList->Interface);
            LogVerbose(FlagUsbdi, "InterfaceList, InterfaceDescriptor = 0x%p",
                       ifaceList->InterfaceDescriptor);
            ifaceList++;
        }
        #endif

        Xrb_AddToList(usbdiHandleData, xrb);

        LogVerbose(FlagUsbdi, "SelectConfig Xrb = 0x%p", xrb);
    } FINALLY {
        if (NT_SUCCESS(status)) {
            *Urb = &xrb->Urb;
        } else {
            if (xrbWdfMemory != NULL) {
                WdfObjectDelete(xrbWdfMemory);
                WdfObjectDereferenceWithTag(xrbWdfMemory, TAG(Xrb_Free));
            }
        }
    }


    return status;
}


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
    )
/*++

Routine Description:

    This routine is called by the client driver to allocate and initialize a select interface
    urb

    In this function we create an XRB of the proper
    size and format for a select_interface request.

Arguments:

    USBDInterfaceHandle - USBDI_HANDLE returned to the client during the Query Interface exchange.

    ConfigurationHandle - Handle returned by the core stack on sending the Select configuration
        request.

    InterfaceListEntry - Pointer to a USBD_INTERFACE_LIST_ENTRY structure. The client driver must
        set the InterfaceDescriptor.

    Urb - Output allocated and inialized Select config urb (Xrb)


Return Value:

    NTSTATUS

--*/
{
    ULONG                             numPipes;
    USHORT                            size;
    NTSTATUS                          status;
    PXRB                              xrb=NULL;
    PUSBDI_HANDLE_DATA                usbdiHandleData;
    WDF_OBJECT_ATTRIBUTES             objectAttributes;
    PUSBD_INTERFACE_INFORMATION       ifaceInfo;
    PUSB_INTERFACE_DESCRIPTOR         ifaceDescriptor;
    ULONG                             index;
    WDFMEMORY                         xrbWdfMemory=NULL;


    TRY {


        UcxPointerNotNull(USBDInterfaceHandle);
        UcxPointerNotNull(ConfigurationHandle);
        UcxPointerNotNull(InterfaceListEntry);
        UcxPointerNotNull(Urb);

        *Urb = NULL;

        usbdiHandleData = (PUSBDI_HANDLE_DATA) USBDInterfaceHandle;

        //
        // Figure out how many interfaces we are dealing with, and calculate the size of
        // memory we need to allocate
        //
        if (InterfaceListEntry->InterfaceDescriptor == NULL) {
            LogError(FlagUsbdi,
                     "InterfaceListEntry(0x%p)->InterfaceDescriptor is NULL, USBDI_HANDLE_DATA 0x%p",
                     InterfaceListEntry, usbdiHandleData);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        if (InterfaceListEntry->Interface != NULL) {
            LogError(FlagUsbdi,
                     "InterfaceListEntry(0x%p)->Interface must be NULL, USBDI_HANDLE_DATA 0x%p",
                     InterfaceListEntry, usbdiHandleData);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        numPipes = InterfaceListEntry->InterfaceDescriptor->bNumEndpoints;

        if (numPipes > MAX_ENDPOINTS_PER_DEVICE) {
            LogError(FlagUsbdi,
                     "Too Many Pipes(%d), InterfaceListEntry(0x%p), USBDI_HANDLE_DATA 0x%p",
                     numPipes, InterfaceListEntry, usbdiHandleData);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        size = (USHORT)GET_SELECT_INTERFACE_REQUEST_SIZE(numPipes);
        size+= sizeof(XRB_HEADER);

        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
        objectAttributes.ParentObject = usbdiHandleData->ThisWdfMemory;

        status = WdfMemoryCreate(&objectAttributes,
                                 NonPagedPool,
                                 usbdiHandleData->PoolTag,
                                 size,
                                 &xrbWdfMemory,
                                 &xrb);

        if (!NT_SUCCESS(status)){
            LogError(FlagUsbdi, "WdfMemoryCreate failed, USBDI_HANDLE_DATA 0x%p, %!STATUS!",
                     usbdiHandleData, status);
            LEAVE;
        }

        WdfObjectReferenceWithTag(xrbWdfMemory, TAG(Xrb_Free));

        RtlZeroMemory(xrb, size);

        xrb->XrbHeader.Sig = SigXrb;
        xrb->XrbHeader.SizeOfXRB = size;
        xrb->XrbHeader.USBDClientContractVersion = usbdiHandleData->USBDClientContractVersion;
        xrb->XrbHeader.UsbdiHandleData = usbdiHandleData;
        xrb->XrbHeader.XrbStatus = XrbStatusInactive;
        xrb->XrbHeader.XrbType = XrbTypeSelectInterface;
        xrb->XrbHeader.ThisWdfMemory = xrbWdfMemory;

        //
        // Build the Select Interface request.
        //
        ifaceInfo = &xrb->Urb.UrbSelectInterface.Interface;

        ifaceDescriptor = InterfaceListEntry->InterfaceDescriptor;

        //#pragma prefast(suppress: 26000, "BUG - ESP:675")
        ifaceInfo->InterfaceNumber = ifaceDescriptor->bInterfaceNumber;
        ifaceInfo->AlternateSetting = ifaceDescriptor->bAlternateSetting;
        ifaceInfo->NumberOfPipes = numPipes;

        for (index=0; index < numPipes; index++) {
            ifaceInfo->Pipes[index].MaximumTransferSize = USBD_DEFAULT_MAXIMUM_TRANSFER_SIZE;
            ifaceInfo->Pipes[index].PipeFlags = 0;
        }

        ifaceInfo->Length = (USHORT) GET_USBD_INTERFACE_SIZE(numPipes);

        NT_ASSERT(((PUCHAR) ifaceInfo) + ifaceInfo->Length <= ((PUCHAR) xrb) + size);

        InterfaceListEntry->Interface = ifaceInfo;

        xrb->Urb.UrbHeader.Length = XrbSizeToUrbSize(size);
        xrb->Urb.UrbHeader.Function = URB_FUNCTION_SELECT_INTERFACE;
        xrb->Urb.UrbSelectInterface.ConfigurationHandle = ConfigurationHandle;

        #if DBG
        LogVerbose(FlagUsbdi, "InterfaceList, InterfaceInfo = 0x%p",
                   InterfaceListEntry->Interface);
        LogVerbose(FlagUsbdi, "InterfaceList, InterfaceDescriptor = 0x%p",
                   InterfaceListEntry->InterfaceDescriptor);
        #endif

        Xrb_AddToList(usbdiHandleData, xrb);

        LogVerbose(FlagUsbdi, "SelectInterface Xrb = 0x%p", xrb);

    } FINALLY {
        if (NT_SUCCESS(status)) {
            *Urb = &xrb->Urb;
        } else {
            if (xrbWdfMemory) {
                WdfObjectDelete(xrbWdfMemory);
                WdfObjectDereferenceWithTag(xrbWdfMemory, TAG(Xrb_Free));
            }
        }
    }


    return status;
}

NTSTATUS
Xrb_UrbCompletion(
    PDEVICE_OBJECT  DeviceObject,
    PIRP            Irp,
    PVOID           Context
    )
/*++

Routine Description:

    Completion handler for new Urbs.

Arguments:

    DeviceObject - Root Hub Pdo Device Object

    Irp - Request Irp

    Context - Urb as PVOID

Return Value:

    returns STATUS_MORE_PROCESSING_REQUIRED or STATUS_CONTINUE_COMPLETION depending on the type
    or urb
--*/
{
    USBD_STATUS                       usbdStatus;
    NTSTATUS                          status;
    PURB                              urb;
    PUCXPIPE_INFO                     endpointPipeInfo;
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext;
    PXRB                              xrb;
    UCXUSBDEVICE                      ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT         ucxUsbDeviceContext;

    UNREFERENCED_PARAMETER(DeviceObject);

    TRY {

        urb = (PURB)Context;
        usbdStatus = urb->UrbHeader.Status;

        if (usbdStatus == USBD_STATUS_PENDING) {

            //
            // HCD did not get a chance to even look at the URB.
            //
            NT_ASSERTMSG("Irp status cannot be success", !NT_SUCCESS(Irp->IoStatus.Status));

            ucxUsbDevice = (UCXUSBDEVICE) urb->UrbHeader.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            if (ucxUsbDeviceContext->Disconnected) {
                urb->UrbHeader.Status = USBD_STATUS_DEVICE_GONE;
                Irp->IoStatus.Status = USBD_STATUS_To_NTSTATUS(urb->UrbHeader.Status);
            } else {
                urb->UrbHeader.Status = USBD_STATUS_CANCELED;
            }

        } else {

            //
            // If usbdstatus is success, NTSTATUS must be success
            //
            NT_ASSERT((USBD_SUCCESS(usbdStatus) && NT_SUCCESS(Irp->IoStatus.Status)) ||
                      (!USBD_SUCCESS(usbdStatus)));

            Irp->IoStatus.Status = USBD_STATUS_To_NTSTATUS(usbdStatus);

        }

        if (Irp->PendingReturned) {
            IoMarkIrpPending(Irp);
        }

        switch (urb->UrbHeader.Function) {

        case URB_FUNCTION_OPEN_STATIC_STREAMS:
            endpointPipeInfo = urb->UrbOpenStaticStreams.PipeHandle;
            ucxEndpointContext = UcxEndpointGetContextFromEndpointPipeInfo(endpointPipeInfo);
            Endpoint_StaticStreamsEnableUrbCompleteFromHcd(ucxEndpointContext, urb, Irp);
            status = STATUS_MORE_PROCESSING_REQUIRED;
            break;

        case URB_FUNCTION_CLOSE_STATIC_STREAMS:
            endpointPipeInfo = urb->UrbPipeRequest.PipeHandle;
            ucxEndpointContext = UcxEndpointGetContextFromEndpointPipeInfo(endpointPipeInfo);
            Endpoint_StaticStreamsDisableUrbComplete(ucxEndpointContext, Irp);
            status = STATUS_MORE_PROCESSING_REQUIRED;
            break;

        default:

            xrb = UrbToXrb(urb);
            Xrb_OnComplete(xrb);

            status = STATUS_CONTINUE_COMPLETION;
            break;

        }
    } FINALLY {

    }

    return status;

}

NTSTATUS
Xrb_TransferUrbCompletion(
    PDEVICE_OBJECT  DeviceObject,
    PIRP            Irp,
    PVOID           Context
    )
/*++

Routine Description:

    Completion handler for new transfer Urbs.

    URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER
    URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_USING_CHAINED_MDL
    URB_FUNCTION_ISOCH_TRANSFER
    URB_FUNCTION_ISOCH_TRANSFER_USING_CHAINED_MDL

Arguments:

    DeviceObject - Root Hub Pdo Device Object

    Irp - Request Irp

    Context - Urb as PVOID

Return Value:

    returns STATUS_CONTINUE_COMPLETION

--*/
{
    USBD_STATUS                 usbdStatus;
    PTRANSFER_URB               transferUrb = (PTRANSFER_URB) Context;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    PXRB                        xrb;
    PUCXPIPE_INFO               ucxPipeInfo;
    PUCXENDPOINT_PRIVCONTEXT    ucxEndpointContext;
    ULONG                       index;

    UNREFERENCED_PARAMETER(DeviceObject);

    TRY {

        usbdStatus = transferUrb->Hdr.Status;

        if (usbdStatus == USBD_STATUS_PENDING) {

            //
            // HCD did not get a chance to even look at the URB.
            //
            NT_ASSERTMSG("Irp status cannot be success", !NT_SUCCESS(Irp->IoStatus.Status));

            ucxUsbDevice = (UCXUSBDEVICE) transferUrb->Hdr.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            ucxUsbDeviceContext->TransferFailureCount++;

            transferUrb->TransferBufferLength = 0;

            if (ucxUsbDeviceContext->Disconnected) {
                transferUrb->Hdr.Status = USBD_STATUS_DEVICE_GONE;
                Irp->IoStatus.Status = USBD_STATUS_To_NTSTATUS(transferUrb->Hdr.Status);
            } else {

                //
                // For XRBs, use the same behavior that is used with the USB2 stack. This
                // is done because an HCD will mimic the USB2 behavior and having the URB
                // completed differently here and by the HCD would be inconsistent.
                //
                // COMPAT: For compat use error code that is commonly returned by USBPORT in this
                // situation
                //
                transferUrb->Hdr.Status = USBD_STATUS_CANCELED;

                //
                // COMPAT: for an isoch transfer, the USBD status must be set to all 0xFF's if
                // the request was never handled by the HCD. The error count must be set to zero.
                //
                if (transferUrb->Hdr.Function == URB_FUNCTION_ISOCH_TRANSFER) {

                    for (index = 0; index < transferUrb->u.Isoch.NumberOfPackets; index++) {
                        transferUrb->u.Isoch.IsoPacket[index].Status = 0xFFFFFFFF;
                    }

                    transferUrb->u.Isoch.ErrorCount = 0;
                }
            }

        } else {

            //
            // If usbdstatus is success, NTSTATUS must be success
            //
            NT_ASSERT((USBD_SUCCESS(usbdStatus) && NT_SUCCESS(Irp->IoStatus.Status)) ||
                      (!USBD_SUCCESS(usbdStatus)));

            if (!USBD_SUCCESS(usbdStatus)) {
                ucxUsbDevice = (UCXUSBDEVICE) transferUrb->Hdr.UsbdDeviceHandle;
                ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
                ucxUsbDeviceContext->TransferFailureCount++;

                if (1 == InterlockedExchange(&ucxUsbDeviceContext->PendingNoPingResponseError, 0)) {
                    UsbDevice_NoPingResponse(ucxUsbDeviceContext);
                }

                //
                // Determine if HCD is prematurely completing a transfer
                //
                ucxPipeInfo = (PUCXPIPE_INFO) transferUrb->UsbdPipeHandle;
                if (ucxPipeInfo->IsStreamPipe == FALSE) {
                    ucxEndpointContext = CONTAINING_RECORD(ucxPipeInfo,
                                                           UCXENDPOINT_PRIVCONTEXT,
                                                           UcxPipeInfo);
                    NT_ASSERT(ucxEndpointContext->OkToCancelTransfersSyncEvent == 0);
                }

            }

            Irp->IoStatus.Status = USBD_STATUS_To_NTSTATUS(usbdStatus);

        }

        if (Irp->PendingReturned) {
            IoMarkIrpPending(Irp);
        }

        xrb = UrbToXrb((PURB)transferUrb);
        Xrb_OnComplete(xrb);

        Etw_XrbComplete(Irp, transferUrb);

    } FINALLY {

    }

    return STATUS_CONTINUE_COMPLETION;
}

VOID
LogWppMsgForCloseStaticStreamOnDx(
    __in
        PIRP            Irp
    )
/*++

Routine Description:

    This worker function is created to move the logging from header
    to source file. Adding trace statements in the log file leads
    to binary bloat and also incorrect function name in the log if the
    inline function is called from multiple places.

--*/
{
    LogWarning(FlagUsbdi, "Received URB_FUNCTION_CLOSE_STATIC_STREAMS in Dx, so completing Irp 0x%p with %!STATUS!",
            Irp, STATUS_SUCCESS);
    return;
}
