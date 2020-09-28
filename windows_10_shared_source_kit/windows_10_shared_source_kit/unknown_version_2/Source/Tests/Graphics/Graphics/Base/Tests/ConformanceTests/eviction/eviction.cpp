eDumpData) {
            status = STATUS_NOT_SUPPORTED;
            LogInfo(FlagController, "HCD did not provide EvtControllerFreeDumpData");
            LEAVE;
        }

        UcxControllerContext->Config.EvtControllerFreeDumpData(
            UcxControllerContext->UcxController,
            DumpControllerInfo);

        status = STATUS_SUCCESS;

    } FINALLY {
        
        WdfRequestComplete(Request, status);
    }
}

NTSTATUS
Controller_EnableForwardProgress(
    __in 
        PUCXCONTROLLER_PRIVCONTEXT                UcxControllerContext,
    __in
        PUSB_FORWARD_PROGRESS_INFO                ForwardProgressInfo
    )
{
    NTSTATUS                                    status;
    UCXUSBDEVICE                                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT                   ucxUsbDeviceContext;
    HANDLE                                      systemThread;    
    OBJECT_ATTRIBUTES                           attributes;
    BOOLEAN                                     mutexAcquired = FALSE;

    TRY {

        //
        // First Start the System Thread
        //

        KeWaitForMutexObject(&UcxControllerContext->EnableForwardProgressMutex,
                             Executive,
                             KernelMode,
                             FALSE,
                             NULL);
        mutexAcquired = TRUE;

        if (UcxControllerContext->SystemThread == NULL) {
            
            InitializeObjectAttributes(&attributes, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

            status = PsCreateSystemThread(&systemThread,
                                          0,
                                          &attributes,
                                          NULL,
                                          NULL,
                                          Controller_SystemThread,
                                          UcxControllerContext);

            if (!NT_SUCCESS(status)) {
                LogError(FlagController, "PsCreateSystemThread Failed %!STATUS!", status);
                LEAVE;
            }

            status = ObReferenceObjectByHandle(systemThread,
                                               0,
                                               NULL,
                                               KernelMode,
                                               (PVOID*)&UcxControllerContext->SystemThread,
                                               NULL);

            //
            // Need to close the systemThread handle