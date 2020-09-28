vice is "USBROOT(0)" 
        //
        RtlCopyMemory(stringBuffer,
                      ROOTHUB_LOCATION,
                      sizeof(ROOTHUB_LOCATION));
        
        *LocationStrings = stringBuffer;

        status = STATUS_SUCCESS;
    
    } FINALLY {
    
    }

    return status;
}


_Must_inspect_result_
NTSTATUS
RootHub_Pdo_QueryInterfaceAdd(
    __in 
        WDFDEVICE          RHPdo,
    __in 
        UCXROOTHUB         UcxRootHub
)
/*++

Routine Description:
    
    This is a function that adds the following query interfaces:
        GUID_HUB_CONTROLLERSTACK_INTERFACE
        GUID_HUB_PARENT_INTERFACE
    
Arguments:

    RHPdo - A handle representing the WDFDEVICE for the enumerated Roothub Pdo
    
    UcxRoothub - A handle to the roothub object.
    
    UcxEndpointRHInterrupt - A handle to the interrupt endpoint of the roothub 
        object

--*/
{
    NTSTATUS                          status;    
    HUB_CONTROLLERSTACK_INTERFACE     hubinterface;    
    HUB_PARENT_INTERFACE              hubparentInterface;
    WDF_QUERY_INTERFACE_CONFIG        qiConfig;
    USBD_INTERFACE_V1                 usbdInterfaceV1;
    PNP_LOCATION_INTERFACE            pnpLocationInterface;

    UNREFERENCED_PARAMETER(UcxRootHub);

    TRY {

        //
        // Create a UCXUSBDEVICE for the UCXROOTHUB
        //
        RtlZeroMemory(&hubparentInterface, sizeof(hubparentInterface));

        hubparentInterface.Size = sizeof(hubparentInterface);
        hubparentInterface.Version = HUB_PARENT_INTERFACE_VERSION_LATEST;

        WDF_QUERY_INTERFACE_CONFIG_INIT(&qiConfig,
                                        (PINTERFACE) &hubparentInterface,
                                        &GUID_HUB_PARENT_INTERFACE,
                                        RootHub_EvtHubParentQueryInterfaceRequest);
        
        qiConfig.ImportInterface = TRUE;
        
        status = WdfDeviceAddQueryInterface(RHPdo, &qiConfig);

        if (!NT_SUCCESS(status)) {
            LogError(FlagHubInterface, 
                     "WdfDeviceAddQueryInterface for Parent Hub Interface Failed %!STATUS!", 
                     status);
            LEAVE;
        }

        RtlZeroMemory(&hubinterface, sizeof(hubinterface));
        
        hubinterface.Size = sizeof(hubinterface);
        hubinterface.Version = HUB_CONTROLLERSTACK_INTERFACE_VERSION_1_0; 

        WDF_QUERY_INTERFACE_CONFIG_INIT(&qiConfig,
                                        (PINTERFACE) &hubinterface, 
                                        &GUID_HUB_CONTROLLERSTACK_INTERFACE,
                                        RootHub_EvtHubControllerStackQueryInterfaceRequest);
        
        qiConfig.ImportInterface = TRUE;

        status = WdfDeviceAddQueryInterface(RHPdo, &qiConfig);

        if (!NT_SUCCESS(status)) {
            LogError(FlagHubInterface, 
                     "WdfDeviceAddQueryInterface for Hub Controller Interface Failed %!STATUS!", 
                     status);
            LEAVE;
        }
    
        #pragma prefast(suppress:6309, "Windows 7:759103")
        WDF_QUERY_INTERFACE_CONFIG_INIT(&qiConfig,
                                        NULL, 
                                        &USB_BUS_INTERFACE_USBDI_GUID,
                                        RootHub_Evt20USBDIQueryInterfaceRequest);
        qiConfig.ImportInterface = TRUE;

        status = WdfDeviceAddQueryInterface(RHPdo, &qiConfig);

        if (!NT_SUCCESS(status)) {
            LogError(FlagHubInterface, 
                     "WdfDeviceAddQueryInterface for 2.0 USBDI Interface Failed %!STATUS!", 
                     status);
            LEAVE;
        }

        //
        // Register the GUID_USBD_INTERFACE_VERSION_1 interface.
        // (This interface defines the XRB contract)
        //

        RtlZeroMemory(&usbdInterfaceV1, sizeof(usbdInterfaceV1));

        usbdInterfaceV1.Size = sizeof(usbdInterfaceV1);
        usbdInterfaceV1.Version = USBD_INTERFACE_VERSION_602;

        WDF_QUERY_INTERFACE_CONFIG_INIT(&qiConfig,
                                        (PINTERFACE) &usbdInterfaceV1,
                                        &GUID_USBD_INTERFACE_V1,
                                        RootHub_EvtUSBDInterfaceV1Request);

        qiConfig.ImportInterface = TRUE;

        status = WdfDeviceAddQueryInterface(RHPdo, &qiConfig);

        if (!NT_SUCCESS(status)) {
            LogError(FlagHubInterface, 
                     "WdfDeviceAddQueryInterface for Parent Hub Interface Failed %!STATUS!", 
                     status);
            LEAVE;
        }

        //
        // Add the PnP location interface
        //
        RtlZeroMemory(&pnpLocationInterface, sizeof(pnpLocationInterface));

        pnpLocationInterface.Size = sizeof(pnpLocationInterface);
        pnpLocationInterface.Version = LOCATION_INTERFACE_VERSION;
        pnpLocationInterface.InterfaceReference = WdfDeviceInterfaceReferenceNoOp;
        pnpLocationInterface.InterfaceDereference = WdfDeviceInterfaceDereferenceNoOp;
        pnpLocationInterface.GetLocationString = RootHub_Pdo_GetLocationString;

        WDF_QUERY_INTERFACE_CONFIG_INIT(&qiConfig,
                                        (PINTERFACE)&pnpLocationInterface,
                                        &GUID_PNP_LOCATION_INTERFACE,
                                        NULL);
        
        status = WdfDeviceAddQueryInterface(RHPdo, &qiConfig);

        if (!NT_SUCCESS(status)){

            LogError(FlagHubInterface,
                     "WdfDeviceAddQueryInterface for GUID_PNP_LOCATION_INTERFACE failed %!STATUS!",
                     status);

            LEAVE;
        }


    } FINALLY {

    }

    return status; 
    
}


NTSTATUS
RootHub_EvtUSBDInterfaceV1Request(
    WDFDEVICE            RHPdo,    
    LPGUID               InterfaceType,    
    PINTERFACE           ExposedInterface,    
    PVOID                ExposedInterfaceSpecificData
    )
/*++

Routine Description:
    
    This is the EvtDeviceProcessQueryInterfaceRequest callback for the 
    GUID_USBD_INTERFACE_V1 interface that the controller extension
    implments.
    
Arguments:

    RHPdo - A handle representing the WDFDEVICE for the enumerated Roothub Pdo
    
    InterfaceType - Pointer to the GUID_USBD_INTERFACE_V1
    
    ExposedInterface - A pointer to an INTERFACE structure that describes the 
        USBD_INTERFACE_V1 structure.
        
    ExposedInterfaceSpecificData - This is the UcxUsbDevice that is requesting the interface 
        exchange
    
Comments:
    

--*/
{
    NTSTATUS                          status;
    PUSBD_INTERFACE_V1                usbdInterfaceV1;
    WDFMEMORY                         wdfMemory=NULL;
    PUSBDI_HANDLE_DATA                usbdiHandleData;
    UCXUSBDEVICE                      ucxUsbDevice;
    WDF_OBJECT_ATTRIBUTES             objectAttributes;
    PUCXUSBDEVICE_PRIVCONTEXT         ucxUsbDeviceContext;
    KIRQL                             irql;
    PRHPDO_CONTEXT                    rhPdoContext;
    
    UNREFERENCED_PARAMETER(InterfaceType);
    
    TRY {

        ucxUsbDevice = (UCXUSBDEVICE) ExposedInterfaceSpecificData;

        if (NULL == ucxUsbDevice) {
            //
            // it seems that the request is being sent by the RootHub
            //
            rhPdoContext = RHPdoGetContext(RHPdo);
            ucxUsbDevice = UCXROOTHUB_TO_UCXUSBDEVICE(rhPdoContext->UcxRootHubContext->UcxRootHub);            
        }

        if (ExposedInterface->Size != sizeof(USBD_INTERFACE_V1)) {
            LogError(FlagUsbdi, "Insufficient size of Interface Expected %d, Actual %d",
                     sizeof(USBD_INTERFACE_V1),
                     ExposedInterface->Size);
            status = STATUS_BUFFER_TOO_SMALL;
            LEAVE;
        }

        if (ExposedInterface->Version != USBD_INTERFACE_VERSION_602) {
            LogError(FlagUsbdi, "Unsupported version Expected 0x602, Actual %d",
                     ExposedInterface->Version);
            status = STATUS_NOT_SUPPORTED;
            LEAVE;
        }

        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
        
        usbdInterfaceV1 = (PUSBD_INTERFACE_V1) ExposedInterface;
        
        if (usbdInterfaceV1->DeviceObject == NULL) {
            LogError(FlagUsbdi, 
                     "Null Device object, usbdInterfaceV1 0x%p,, ucxUsbDevice 0x%p", 
                     usbdInterfaceV1, ucxUsbDevice);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }
        
        if (usbdInterfaceV1->PoolTag == 0) {
            LogError(FlagUsbdi, 
                     "PoolTab must be non null, usbdInterfaceV1 0x%p, ucxUsbDevice 0x%p", 
                     usbdInterfaceV1, ucxUsbDevice);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
        objectAttributes.ParentObject = ucxUsbDevice;
        
        status = WdfMemoryCreate(&objectAttributes,
                                 NonPagedPool,
                                 usbdInterfaceV1->PoolTag,
                                 sizeof(USBDI_HANDLE_DATA),
                                 &wdfMemory,
                                 &usbdiHandleData);
        if (!NT_SUCCESS(status)) {
            LogError(FlagUsbdi, 
                     "WdfMemoryCreate failed, usbdInterfaceV1 0x%p, ucxUsbDevice 0x%p, %!STATUS!", 
                     usbdInterfaceV1, ucxUsbDevice, status);
            LEAVE;
        }

        //
        // Take an additional reference so that the object stays in memory even if UcxUsbDevice
        // is deleted
        //
        WdfObjectReferenceWithTag(wdfMemory, TAG(RootHub_USBDInterfaceV1Unregister));

        RtlZeroMemory(usbdiHandleData, sizeof(USBDI_HANDLE_DATA));

        usbdiHandleData->Sig = SigUsbdiHandleData;
        usbdiHandleData->DeviceObject = usbdInterfaceV1->DeviceObject;
        usbdiHandleData->PoolTag = usbdInterfaceV1->PoolTag;
        usbdiHandleData->USBDClientContractVersion = usbdInterfaceV1->USBDClientContractVersion;
        usbdiHandleData->ContextFromClient = usbdInterfaceV1->ContextFromClient;
        usbdiHandleData->UcxUsbDeviceContext = ucxUsbDeviceContext;
        usbdiHandleData->ThisWdfMemory = wdfMemory;
        usbdiHandleData->RHPdo = RHPdo;

        KeInitializeSpinLock(&usbdiHandleData->XrbListLock);
        InitializeListHead(&usbdiHandleData->XrbListHead);

        if (usbdInterfaceV1->UsbVerifierEnabled != 0) {
            usbdiHandleData->UsbVerifierEnabled = TRUE;
            usbdiHandleData->UsbVerifierFailRegistration = usbdInterfaceV1->UsbVerifierFailRegistration;
            usbdiHandleData->UsbVerifierFailChainedMdlSupport = usbdInterfaceV1->UsbVerifierFailChainedMdlSupport;
            usbdiHandleData->UsbVerifierFailStaticStreamSupport = usbdInterfaceV1->UsbVerifierFailStaticStreamSupport;
            usbdiHandleData->UsbVerifierStaticStreamCountOverride = usbdInterfaceV1->UsbVerifierStaticStreamCountOverride;
            usbdiHandleData->UsbVerifierFailEnableStaticStreams = usbdInterfaceV1->UsbVerifierFailEnableStaticStreams;
        }

        if (usbdiHandleData->UsbVerifierEnabled ||
            ucxUsbDeviceContext->UcxControllerContext->DriverVerifierEnabled ) {
            usbdiHandleData->UsbVerifierTrackXrbAllocationsEnabled = TRUE;
        }

        if (usbdiHandleData->UsbVerifierEnabled) {

            //
            // Enable strict validation of pipe handles.
            //
            UsbDevice_EnableInvalidHandleTracking(ucxUsbDeviceContext);
        }

        if (Ucx_FailDueToVerifierSetting(usbdiHandleData->UsbVerifierFailRegistration)) {
            status = Ucx_GetRandomErrorNTSTATUS();
            LogInfo(FlagUsbdi, "UsbVerifier failing Query Interface, Status %!STATUS!", status);
            LEAVE;
        }

        //
        // DONT fail after this point
        //

        usbdInterfaceV1->InterfaceReference = WdfDeviceInterfaceReferenceNoOp;
        usbdInterfaceV1->InterfaceDereference = WdfDeviceInterfaceDereferenceNoOp;
        usbdInterfaceV1->Context = NULL;
        
        usbdInterfaceV1->USBDInterfaceHandle = (USBDI_HANDLE)usbdiHandleData;
        usbdInterfaceV1->Unregister = RootHub_USBDInterfaceV1Unregister;
        usbdInterfaceV1->XrbAllocate = Xrb_Allocate;
        usbdInterfaceV1->IsochXrbAllocate = Xrb_IsochAllocate;
        usbdInterfaceV1->SelectConfigXrbAllocateAndBuild = Xrb_SelectConfigAllocateAndBuild;
        usbdInterfaceV1->SelectInterfaceXrbAllocateAndBuild = 
            Xrb_SelectInterfaceAllocateAndBuild;
        usbdInterfaceV1->XrbFree = Xrb_Free;

        ObReferenceObjectWithTag(usbdiHandleData->DeviceObject, 
                                 TAG_ULONG(RootHub_USBDInterfaceV1Unregister));
        //
        // Track a list, count for diagnosability purposes.
        //
        KeAcquireSpinLock(&ucxUsbDeviceContext->USBDInterfaceListLock, &irql);
        InsertTailList(&ucxUsbDeviceContext->USBDInterfaceListHead,
                       &usbdiHandleData->USBDInterfaceListEntry);
        KeReleaseSpinLock(&ucxUsbDeviceContext->USBDInterfaceListLock, irql);
        InterlockedIncrement(&ucxUsbDeviceContext->UcxControllerContext->USBDInterfaceCount);

        status = STATUS_SUCCESS;

    } FINALLY {

        LogVerbose(FlagUsbdi, "Returning ExposedInterface Size %d, Version %d, Status %!STATUS!",
                   ExposedInterface->Size, ExposedInterface->Version, status);

        if (!NT_SUCCESS(status)) {
            
            if (wdfMemory != NULL) {
                WdfObjectDelete(wdfMemory);
                WdfObjectDereferenceWithTag(wdfMemory, TAG(RootHub_USBDInterfaceV1Unregister));
            }
        }
    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
RootHub_USBDInterfaceV1Unregister(
    __in
        USBDI_HANDLE         USBDInterfaceHandle
)
/*++

Routine Description:

    This routine is called by the client driver to unregister this interface. 

Arguments:

    USBDInterfaceHandle - USBDI_HANDLE returned to the client during the Query Interface exchange. 

Returns:

    VOID
--*/
{
    PUSBDI_HANDLE_DATA         usbdiHandleData;
    PUCXUSBDEVICE_PRIVCONTEXT  ucxUsbDeviceContext;
    LONG                       value;
    KIRQL                      irql;
    
    TRY {
        UcxPointerNotNull(USBDInterfaceHandle);
        
        usbdiHandleData = (PUSBDI_HANDLE_DATA) USBDInterfaceHandle;
        
        //
        // Track a count, list for diagnosability purposes.
        //
        ucxUsbDeviceContext = usbdiHandleData->UcxUsbDeviceContext;
        
        KeAcquireSpinLock(&ucxUsbDeviceContext->USBDInterfaceListLock, &irql);
        RemoveEntryList(&usbdiHandleData->USBDInterfaceListEntry);
        InitializeListEntry(&usbdiHandleData->USBDInterfaceListEntry);
        KeReleaseSpinLock(&ucxUsbDeviceContext->USBDInterfaceListLock, irql);

        value = InterlockedDecrement(&ucxUsbDeviceContext->UcxControllerContext->USBDInterfaceCount);
        NT_ASSERT(value >= 0);

        ObDereferenceObjectWithTag(usbdiHandleData->DeviceObject, 
                                   TAG_ULONG(RootHub_USBDInterfaceV1Unregister));

        WdfObjectDelete(usbdiHandleData->ThisWdfMemory);
        WdfObjectDereferenceWithTag(usbdiHandleData->ThisWdfMemory, 
                                    TAG(RootHub_USBDInterfaceV1Unregister));


    } FINALLY {

    }
    return;
}


NTSTATUS
RootHub_Pdo_EvtSetPowerIrpPreprocessCallback(
    WDFDEVICE   Device,
    PIRP        Irp
    )
/*++

Routine Description:
    
    This is an irp preprocess routine for the set power irp. 
    
    WDF does not provide the ability to retrieve the target SIrp inforamtion

Arguments:

    Device - The Roothub Pdo's WDFDEVICE
    
    Irp - The Query Interface request
    
Comments:

    

--*/
{
    NTSTATUS                    status;
    PIO_STACK_LOCATION          ioStack;
    SYSTEM_POWER_STATE_CONTEXT  systemPowerStateContext;
    PUCXROOTHUB_PRIVCONTEXT     ucxRootHubContext;
    PRHPDO_CONTEXT              rhPdoContext;

    TRY {

        rhPdoContext = RHPdoGetContext(Device);
        ucxRootHubContext = rhPdoContext->UcxRootHubContext;
        ioStack = IoGetCurrentIrpStackLocation(Irp);

        //
        // Translate the various Power transition information to simpler System
        // Power actions.
        //
        if (ioStack->Parameters.Power.Type == SystemPowerState) {
            systemPowerStateContext = ioStack->Parameters.Power.SystemPowerStateContext;
            if (systemPowerStateContext.TargetSystemState != PowerSystemWorking) {
                ucxRootHubContext->LastSystemSleepState = systemPowerStateContext.TargetSystemState;
                //
                // Storing the SystemPowerAction on our own is a workaround
                // for a bug in the WdfDeviceGetSystemPowerAction API. This
                // API returns the Shutdowntype from the D IRP, which is not
                // reliable in scenarios where we are going to S0Idle while the
                // system has still not completed the system resume transition.
                // We are pretty much mimicking the API behavior except that 
                // instead of pulling the ShutdownType from the D Irp, we pull 
                // it from the last S Irp.
                //                     
                ucxRootHubContext->SystemPowerAction = ioStack->Parameters.Power.ShutdownType;
            } else {
                ucxRootHubContext->SystemPowerAction = PowerActionNone;
            }

        }

        IoSkipCurrentIrpStackLocation(Irp);
        status = WdfDeviceWdmDispatchPreprocessedIrp(Device, Irp);

    } FINALLY {

    }

    return status;
}

NTSTATUS
RootHub_Pdo_EvtQueryInterfaceIrpPreprocessCallback(
    WDFDEVICE   Device,
    PIRP        Irp
    )
/*++

Routine Description:
    
    This is an irp preprocess routine for the query interface irp. 
    
    WDF does not support a query interface with same GUID but different input version. Thus
    we need to intercept the USB_BUS_INTERFACE_USBDI_GUID request in the pre-processor routine, 
    and move the interface version and size information to the PINTERFACE structure that would be
    passed to the handler for this query interface.

    For more details please refer to bug 282435 in Windows7 database.

Arguments:

    Device - The Roothub Pdo's WDFDEVICE
    
    Irp - The Query Interface request
    
Comments:

    

--*/
{
    PIO_STACK_LOCATION      irpStack;
    NTSTATUS                status;
    PINTERFACE              interface;

    TRY {
        
        irpStack = IoGetCurrentIrpStackLocation(Irp);

        if (RtlCompareMemory(irpStack->Parameters.QueryInterface.InterfaceType,
                             &USB_BUS_INTERFACE_USBDI_GUID,
                             sizeof(GUID)) == sizeof(GUID)) {
            
            interface = irpStack->Parameters.QueryInterface.Interface;
            
            LogWarning(FlagUsbdi, 
                       "USB_BUS_INTERFACE_USBDI Query Exchange: Fixing irpStack->Parameters.QueryInterface.Interface: Size from %d to %d, Version from %d to %d",
                       interface->Size, irpStack->Parameters.QueryInterface.Size,
                       interface->Version, irpStack->Parameters.QueryInterface.Version);

            interface->Size = irpStack->Parameters.QueryInterface.Size;
            interface->Version = irpStack->Parameters.QueryInterface.Version;

        } else if (RtlCompareMemory(irpStack->Parameters.QueryInterface.InterfaceType,
                                    &GUID_HUB_PARENT_INTERFACE,
                                    sizeof(GUID)) == sizeof(GUID)) {

            //
            // KMDF does not do an explicit version check. We need 
            // an explicit check for our Hub Parent Inerface to prevent a mismatched hub from
            // loading on top of ucx. 
            //
            interface = irpStack->Parameters.QueryInterface.Interface;
            interface->Size = irpStack->Parameters.QueryInterface.Size;
            interface->Version = irpStack->Parameters.QueryInterface.Version;

        }

        IoSkipCurrentIrpStackLocation(Irp);
        status = WdfDeviceWdmDispatchPreprocessedIrp(Device, Irp);

    } FINALLY {

    }

    return status;
}


NTSTATUS
RootHub_Evt20USBDIQueryInterfaceRequest(
    WDFDEVICE            RHPdo,    
    LPGUID               InterfaceType,    
    PINTERFACE           ExposedInterface,    
    PVOID                ExposedInterfaceSpecificData
    )
/*++

Routine Description:
    
    This is the EvtDeviceProcessQueryInterfaceRequest callback for the 
    GUID_HUB_CONTROLLERSTACK_INTERFACE interface that the controller extension
    implments.
    
Arguments:

    RHPdo - A handle representing the WDFDEVICE for the enumerated Roothub Pdo
    
    InterfaceType - Pointer to the GUID_HUB_CONTROLLERSTACK_INTERFACE
    
    ExposedInterface - A pointer to an INTERFACE structure that describes the 
        HUB_CONTROLLERSTACK_INTERFACE structure.
        
    ExposedInterfaceSpecificData - NOT USED
    
Comments:
    

--*/
{
    NTSTATUS                      status;
    
    PUSB_BUS_INTERFACE_USBDI_V0   usbdiInterfaceV0; 
    PUSB_BUS_INTERFACE_USBDI_V1   usbdiInterfaceV1;
    PUSB_BUS_INTERFACE_USBDI_V2   usbdiInterfaceV2;
    PUSB_BUS_INTERFACE_USBDI_V3   usbdiInterfaceV3;

    PRHPDO_CONTEXT                rhPdoContext;
    UCXUSBDEVICE                  ucxUsbDevice;
    
    USHORT                        size = 0;
    USHORT                        version = 0;

    UNREFERENCED_PARAMETER(InterfaceType);

    TRY {

        ucxUsbDevice = (UCXUSBDEVICE) ExposedInterfaceSpecificData;

        if (NULL == ucxUsbDevice) {
            //
            // AMD Filter driver exchanges this interface and later calls IsDeviceHighSpeed 
            // for the controller itself. The filter driver is sitting below the RootHub FDO
            // and exchanges this interface. Return The UCXROOTHUB in this case. 
            // (Windows 8 Bug 328163)
            //
            rhPdoContext = RHPdoGetContext(RHPdo);
            ucxUsbDevice = UCXROOTHUB_TO_UCXUSBDEVICE(rhPdoContext->UcxRootHubContext->UcxRootHub);
        }

        if (ExposedInterface->Version >= USB_BUSIF_USBDI_VERSION_0) {

            if (ExposedInterface->Size < sizeof(USB_BUS_INTERFACE_USBDI_V0)) {
                
                LogError(FlagUsbdi, "Insufficient size of Interface Expected %d, Actual %d",
                         sizeof(USB_BUS_INTERFACE_USBDI_V0),
                         ExposedInterface->Size);
                status = STATUS_BUFFER_TOO_SMALL;
                LEAVE;
            }
            
            usbdiInterfaceV0 = (PUSB_BUS_INTERFACE_USBDI_V0) ExposedInterface;
            
            usbdiInterfaceV0->BusContext = (PVOID) ucxUsbDevice;
            usbdiInterfaceV0->InterfaceReference = WdfDeviceInterfaceReferenceNoOp;
            usbdiInterfaceV0->InterfaceDereference = WdfDeviceInterfaceDereferenceNoOp;
            usbdiInterfaceV0->GetUSBDIVersion = Usbd_BusGetUSBDIVersion;
            usbdiInterfaceV0->QueryBusTime = Usbd_BusQueryBusTime;
            usbdiInterfaceV0->SubmitIsoOutUrb = Usbd_BusSubmitIsoOutUrb;
            usbdiInterfaceV0->QueryBusInformation = Usbd_BusQueryBusInformation;

            size  = sizeof(USB_BUS_INTERFACE_USBDI_V0);
            version = USB_BUSIF_USBDI_VERSION_0;
        }

        if (ExposedInterface->Version >= USB_BUSIF_USBDI_VERSION_1) {

            if (ExposedInterface->Size < sizeof(USB_BUS_INTERFACE_USBDI_V1)) {
                
                LogError(FlagUsbdi, "Insufficient size of Interface Expected %d, Actual %d",
                         sizeof(USB_BUS_INTERFACE_USBDI_V1),
                         ExposedInterface->Size);
                status = STATUS_BUFFER_OVERFLOW;
                LEAVE;
            }
            
            usbdiInterfaceV1 = (PUSB_BUS_INTERFACE_USBDI_V1) ExposedInterface;

            usbdiInterfaceV1->IsDeviceHighSpeed = Usbd_IsDeviceAtleastHighSpeed;

            size = sizeof(USB_BUS_INTERFACE_USBDI_V1);
            version = USB_BUSIF_USBDI_VERSION_1;
        }
        
        if (ExposedInterface->Version >= USB_BUSIF_USBDI_VERSION_2) {
            
            if (ExposedInterface->Size < sizeof(USB_BUS_INTERFACE_USBDI_V2)) {
                
                LogError(FlagUsbdi, "Insufficient size of Interface Expected %d, Actual %d",
                         sizeof(USB_BUS_INTERFACE_USBDI_V2),
                         ExposedInterface->Size);
                status = STATUS_BUFFER_OVERFLOW;
                LEAVE;
            }
            
            usbdiInterfaceV2 = (PUSB_BUS_INTERFACE_USBDI_V2) ExposedInterface;
            
            usbdiInterfaceV2->EnumLogEntry =  Usbd_BusEnumLogEntry;
            
            size = sizeof(USB_BUS_INTERFACE_USBDI_V2);
            version = USB_BUSIF_USBDI_VERSION_2;
        }
        
        
        if (ExposedInterface->Version >= USB_BUSIF_USBDI_VERSION_3) {

            if (ExposedInterface->Size < sizeof(USB_BUS_INTERFACE_USBDI_V3)) {
                
                LogError(FlagUsbdi, "Insufficient size of Interface Expected %d, Actual %d",
                         sizeof(USB_BUS_INTERFACE_USBDI_V3),
                         ExposedInterface->Size);
                status = STATUS_BUFFER_OVERFLOW;
                LEAVE;
            }
            
            usbdiInterfaceV3 = (PUSB_BUS_INTERFACE_USBDI_V3) ExposedInterface;
            
            usbdiInterfaceV3->QueryBusTimeEx = Usbd_BusQueryBusTimeEx;

            usbdiInterfaceV3->QueryControllerType = Usbd_QueryControllerType;

            size = sizeof(USB_BUS_INTERFACE_USBDI_V3);
            version = USB_BUSIF_USBDI_VERSION_3;
        }
        
        status = STATUS_SUCCESS;
    } FINALLY {

        ExposedInterface->Size = size;
        ExposedInterface->Version = version;
        

        LogVerbose(FlagUsbdi, "Returning ExposedInterface Size %d, Version %d, Status %!STATUS!",
                   ExposedInterface->Size, ExposedInterface->Version, status);

    }

    return status;
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
RootHub_GetHubSymbolicLinkName(
    __in 
        PVOID             Context,
    __in 
        PUNICODE_STRING  HubSymbolicLinkName
    )
/*++

Routine Description:

    This is the handler for GetHubSymbolicLinkName of the HUB_PARENT_INTERFACE.
    This function is called by the hub driver to retrieve the root hub's symbolic link
    name.  

Arguments:

    Context - The context that was returned to the Hub driver during an HUB_PARENT_INTERFACE 
        exchange. It is the handle to the UCXROOTHUB object.

    HubSymbolicLinkName - A pointer to a UNICODE_STRING to store the symbolic link name string

--*/
{
    UCXROOTHUB              ucxRootHub;
    PUCXROOTHUB_PRIVCONTEXT ucxRootHubContext;

    TRY {

        ucxRootHub = (UCXROOTHUB)Context;
        ucxRootHubContext = UcxRootHubGetContext(ucxRootHub);

        RtlInitUnicodeString(HubSymbolicLinkName, NULL);

        if (ucxRootHubContext->RHPdoSymbolicName != NULL) {
            WdfStringGetUnicodeString(ucxRootHubContext->RHPdoSymbolicName, HubSymbolicLinkName);
        }
    } FINALLY {
    }
}


__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
RootHub_WasResetOnResume(
    __in 
        PVOID             Context
    )
/*++

Routine Description:

    This is the handler or WasHubResetOnResume of the HUB_PARENT_INTERFACE.
    This function is called by the Root hub driver to inquire whether the Roothub
    was reset on resume.
    
    Essentially the purpose of this query is for the hub driver to determine whether it needs
    to re-enumerate the downstream devices. 

Arguments:

    Context - The context that was returned to the Hub driver during an HUB_PARENT_INTERFACE 
        exchange. It is the handle to the UCXROOTHUB object    
--*/
{
    UCXROOTHUB              ucxRootHub;
    PUCXROOTHUB_PRIVCONTEXT ucxRootHubContext;
    PUCXCONTROLLER_PRIVCONTEXT ucxControllerContext;
    LONG                       resultAsLong;
    BOOLEAN                 result;

    TRY {

        ucxRootHub = (UCXROOTHUB)Context;
        ucxRootHubContext = UcxRootHubGetContext(ucxRootHub);
        ucxControllerContext = ucxRootHubContext->UcxControllerContext;

        resultAsLong = InterlockedExchange(&ucxControllerContext->WasRootHubReset, 0);
                
        if (resultAsLong == 0) {
            result = FALSE;
        } else {
            result = TRUE;
        }

    } FINALLY {

    }

    return result;
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
RootHub_WasDeviceProgrammingLostOnResume(
    __in 
        PVOID             Context
    )
/*++

Routine Description:

    This is the handler or WasDeviceProgrammingLostOnResume of the HUB_PARENT_INTERFACE.
    This function is called by the Root hub driver to inquire whether the controller lost 
    device programming during last resume.
    
Arguments:

    Context - The context that was returned to the Hub driver during an HUB_PARENT_INTERFACE 
        exchange. It is the handle to the UCXROOTHUB object    
--*/
{
    UCXROO