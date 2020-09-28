/*++
Copyright (c) Microsoft Corporation

Module Name:

    roothub.c

Abstract:

    USB XHCI Driver

--*/

#include "pch.h"

#include "roothub.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, RootHub_Create)
#pragma alloc_text(PAGE, RootHub_PrepareHardware)
#pragma alloc_text(PAGE, RootHub_ReleaseHardware)
#endif

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
RootHub_Create(
    __in
        WDFDEVICE   WdfDevice,
    __in
        HANDLE      ControllerHandle,
    __inout
        PHANDLE     ObjectHandle
    )
/*++

Routine Description:

    This function creates an commmand object and initializes the data
    associated with the root hub.

--*/
{
    NTSTATUS                status;
    UCX_ROOTHUB_CONFIG      ucxRootHubConfig;
    WDF_OBJECT_ATTRIBUTES   wdfAttributes;
    UCXROOTHUB              ucxRootHub;
    PROOTHUB_DATA           rootHubData;

    UNREFERENCED_PARAMETER(WdfDevice);

    PAGED_CODE();

    TRY {

        UCX_ROOTHUB_CONFIG_INIT(&ucxRootHubConfig,
                                RootHub_UcxEvtClearHubFeature,
                                RootHub_UcxEvtClearPortFeature,
                                RootHub_UcxEvtGetHubStatus,
                                RootHub_UcxEvtGetPortStatus,
                                RootHub_UcxEvtSetHubFeature,
                                RootHub_UcxEvtSetPortFeature,
                                RootHub_UcxEvtGetPortErrorCount,
                                RootHub_UcxEvtInterruptTransfer,
                                RootHub_UcxEvtGetInfo,
                                RootHub_UcxEvtGet20PortInfo,
                                RootHub_UcxEvtGet30PortInfo);

        //
        // Specify the context for all WDF Request objects. This is defined for the
        // endpoint object and requests that are sent to the WDF Queue for the
        // endpoint use this structure.
        //
        // TODO: PostWin8: UCX may be using a WDFQUEUE at root-hub PDO level, in which
        //       case, we need to ensure that the WDFREQUESTS that flow through the
        //       queue and end up in xHCI will have REQUEST_DATA as the context.
        //
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&ucxRootHubConfig.WdfRequestAttributes, REQUEST_DATA);

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, ROOTHUB_DATA);

        status = UcxRootHubCreate(Controller_GetUcxControllerObject(ControllerHandle),
                                  &ucxRootHubConfig,
                                  &wdfAttributes,
                                  &ucxRootHub);

        if (NT_SUCCESS(status)) {

            rootHubData = GetRootHubData(ucxRootHub);

            rootHubData->UcxRootHub = ucxRootHub;
            rootHubData->ControllerHandle = ControllerHandle;

            LogInfo(Controller_GetRecorderLog(ControllerHandle), RootHub,
                    "ucxRootHub 0x%p, rootHubData 0x%p", ucxRootHub, rootHubData);

            //
            // Return roothub data as the handle to the roothub object.
            //
            *ObjectHandle = (HANDLE)rootHubData;

        } else {

            LogError(Controller_GetRecorderLog(ControllerHandle), RootHub,
                     "WdfObjectCreate Failed %!STATUS!", status);
        }

    } FINALLY {

    }

    return status;
}

VOID
RootHub_DumpPortData(
    __in
        PPORTSC PortRegister,
    __in
        HANDLE  ControllerHandle
    )
{
    PORTSC  portSC;

    portSC.AsUlong32 = Register_ReadUlong(&PortRegister->AsUlong32);

    LogInfo(Controller_GetRecorderLog(ControllerHandle), RootHub,
            "PortRegister 0x%p PortSC 0x%08x",
            PortRegister, portSC.AsUlong32);

    LogInfo(Controller_GetRecorderLog(ControllerHandle), RootHub,
            "CurrentConnectStatus    %d   Enabled                 %d  OverCurrentActive      %d",
            portSC.CurrentConnectStatus, portSC.PortEnabled, portSC.OverCurrentActive);

    LogInfo(Controller_GetRecorderLog(ControllerHandle), RootHub,
            "Reset                   %d   Power                   %d  LinkState              %d",
            portSC.PortReset, portSC.PortPower, (short)portSC.PortLinkState);

    LogInfo(Controller_GetRecorderLog(ControllerHandle), RootHub,
            "Speed                   %d   IndicatorControl        %d  LinkStateWriteStrobe   %d",
            portSC.PortSpeed, portSC.PortIndicatorControl, portSC.PortLinkStateWriteStrobe);

    LogInfo(Controller_GetRecorderLog(ControllerHandle), RootHub,
            "ConnectStatusChange     %d   EnabledOrDisabledChange %d  OverCurrentChange      %d",
            portSC.ConnectStatusChange, portSC.PortEnabledOrDisabledChange, portSC.OverCurrentChange);

    LogInfo(Controller_GetRecorderLog(ControllerHandle), RootHub,
            "WarmPortResetChange     %d   ResetChange             %d  LinkStateChange        %d",
            portSC.WarmPortResetChange, portSC.PortResetChange, portSC.PortLinkStateChange);

    LogInfo(Controller_GetRecorderLog(ControllerHandle), RootHub,
            "ConfigErrorChange       %d   WakeOnConnectEnable     %d  WakeOnDisconnectEnable %d",
            portSC.PortConfigErrorChange, portSC.WakeOnConnectEnable, portSC.WakeOnDisconnectEnable);

    LogInfo(Controller_GetRecorderLog(ControllerHandle), RootHub,
            "WakeOnOverCurrentEnable %d   DeviceRemovable         %d  WarmPortReset          %d",
            portSC.WakeOnOverCurrentEnable, portSC.DeviceRemovable, portSC.WarmPortReset);

    return;
}


_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
RootHub_PrepareHardware(
    __in
        PROOTHUB_DATA   RootHubData
    )
/*++

Routine Description:

    This function is called on a prepare hardware event and allocates any
    resources required by the root hub.

--*/
{
    NTSTATUS                        status;
    ULONG                           i;
    HANDLE                          registerHandle;
    PEXTENDED_CAPABILITY            extendedCapability;
    SUPPPORTED_PROTOCOL_CAPABILITY  supportedProtocolCapability;
    BOOLEAN                         foundSupportedPorts;

    PAGED_CODE();

    TRY {

        registerHandle = Controller_GetRegisterHandle(RootHubData->ControllerHandle);

        foundSupportedPorts = FALSE;

        RootHubData->NumberOfPorts = Register_GetNumberOfPorts(registerHandle);
        RootHubData->NumberOf20Ports = 0;
        RootHubData->NumberOf30Ports = 0;
        RootHubData->U1DeviceExitLatency = Register_GetU1DeviceExitLatency(registerHandle);
        RootHubData->U2DeviceExitLatency = Register_GetU2DeviceExitLatency(registerHandle);

        RootHubData->OperationalRegisters = Register_GetOperationalRegisters(registerHandle);
        RootHubData->PortRegisterSet = Register_GetPortRegisterSet(registerHandle);

        //
        // Determine if the controller supports Debug Capability
        //
        if (NULL !=
            Register_FindFirstExtendedCapability(registerHandle,
                                                 EXTENDED_CAPABILITY_ID_USB_DEBUG_CAPABILITY)) {

            RootHubData->DebugCapable = TRUE;
        }

        RootHubData->PortData = (PROOTHUB_PORT_DATA)
            ExAllocatePoolWithTag(NonPagedPool,
                                  RootHubData->NumberOfPorts * sizeof(ROOTHUB_PORT_DATA),
                                  DriverAllocationTag);

        if (RootHubData->PortData == NULL) {

            LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                     "Failed to allocate PortData");
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        RtlZeroMemory(RootHubData->PortData,
                      RootHubData->NumberOfPorts * sizeof(ROOTHUB_PORT_DATA));

        //
        // Walk the xHCI Supported Protocol Capability list and look
        // for "USB " 2.x or 3.x protocol ports.
        //
        extendedCapability =
            Register_FindFirstExtendedCapability(registerHandle,
                                                 EXTENDED_CAPABILITY_ID_SUPPORTED_PROTOCOL);

        while (extendedCapability != NULL) {

            Register_ReadBufferUlong(&extendedCapability->AsUlong32,
                                     &supportedProtocolCapability.AsUlong32[0],
                                     ARRAYSIZE(supportedProtocolCapability.AsUlong32));

            if (supportedProtocolCapability.NameString.AsUlong32 == SUPPORTED_PROTOCOL_NAME_STRING_USB) {
                if (supportedProtocolCapability.Revision.MajorRevision == 2 ||
                    supportedProtocolCapability.Revision.MajorRevision == 3) {

                    ULONG   offset;
                    ULONG   count;

                    offset = supportedProtocolCapability.PortOffsetCount.CompatiblePortOffset;
                    count  = supportedProtocolCapability.PortOffsetCount.CompatiblePortCount;

                    LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                            "Protocol %d.%d ports: %d - %d",
                            supportedProtocolCapability.Revision.MajorRevision,
                            supportedProtocolCapability.Revision.MinorRevision,
                            offset,
                            offset + count - 1);

                    if ((offset == 0) || (count == 0) ||
                        ((offset + count - 1)  > RootHubData->NumberOfPorts)) {

                        LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                                 "HW_COMPLIANCE: Invalid protocol port range");
                        status = STATUS_INVALID_PARAMETER;
                        LEAVE;
                    }

                    foundSupportedPorts = TRUE;

                    //
                    // Store the supported protocol capabilities into
                    // the range of ports spanned by the capability into
                    // the PortData array.
                    //
                    for (i = 0; i < count; i++) {

                        PROOTHUB_PORT_DATA  portData;
                        PORTSC              portSC;

                        portData = &RootHubData->PortData[i + offset - 1];

                        if (portData->MajorRevision != 0) {

                            LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                                     "HW_COMPLIANCE: Invalid overlapping protocol port range");
                            status = STATUS_INVALID_PARAMETER;
                            LEAVE;
                        }

                        if (supportedProtocolCapability.Revision.MajorRevision == 2) {
                            RootHubData->NumberOf20Ports++;
                        } else {
                            RootHubData->NumberOf30Ports++;
                        }

                        portData->MinorRevision =
                            (UCHAR)supportedProtocolCapability.Revision.MinorRevision;

                        portData->MajorRevision =
                            (UCHAR)supportedProtocolCapability.Revision.MajorRevision;

                        portData->HighSpeedOnly =
                            (BOOLEAN)supportedProtocolCapability.PortOffsetCount.HighSpeedOnly;

                        portData->IntegratedHubImplemented =
                            (BOOLEAN)supportedProtocolCapability.PortOffsetCount.IntegratedHubImplemented;

                        portSC.AsUlong32 =
                            Register_ReadUlong(&RootHubData->PortRegisterSet[i + offset - 1].PortStatusAndControl.AsUlong32);

                        portData->HardwareLPMFlags.AsUchar = 0;

                        if (supportedProtocolCapability.PortOffsetCount.HardwareLpmCapability) {
                            portData->HardwareLPMFlags.L1CapabilitySupported = 1;
                        }

                        if (supportedProtocolCapability.PortOffsetCount.BeslLpmCapability) {
                            portData->HardwareLPMFlags.BeslLpmCapabilitySupported = 1;
                        }

                        if (portSC.DeviceRemovable == 0) {
                            portData->DeviceRemovable = TRUE;
                        }

                        portData->HubDepth = (UCHAR)supportedProtocolCapability.PortOffsetCount.HubDepth;
                        portData->SpeedsCount = (USHORT)supportedProtocolCapability.PortOffsetCount.SpeedIDCount;
                        portData->Speeds = SUPPORTED_PROTOCOL_SPEED_POINTER((PSUPPPORTED_PROTOCOL_CAPABILITY)&extendedCapability->AsUlong32);
                    }

                } else {

                    //
                    // Unknown Major Revision, ignore.
                    //
                    LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                            "HW_COMPLIANCE: Unknown \"USB \" Supported Protocol Capability revision %d.%d",
                            supportedProtocolCapability.Revision.MajorRevision,
                            supportedProtocolCapability.Revision.MinorRevision);
                }

            } else {

                //
                // Unknown Name String, ignore.
                //
                LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                        "HW_COMPLIANCE: Unknown Supported Protocol Capability name string \"%c%c%c%c\"",
                        supportedProtocolCapability.NameString.NameString[0],
                        supportedProtocolCapability.NameString.NameString[1],
                        supportedProtocolCapability.NameString.NameString[2],
                        supportedProtocolCapability.NameString.NameString[3]);
            }

            extendedCapability =
                Register_FindNextExtendedCapability(registerHandle,
                                                    extendedCapability,
                                                    EXTENDED_CAPABILITY_ID_SUPPORTED_PROTOCOL);
        }

        if (foundSupportedPorts == FALSE) {

            LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                     "HW_COMPLIANCE: Failed to find \"USB \" 2.x or 3.x Supported Protocol Capability ports");
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        for (i = 0; i < RootHubData->NumberOfPorts; i++) {

            RootHub_DumpPortData(&RootHubData->PortRegisterSet[i].PortStatusAndControl,
                                 RootHubData->ControllerHandle);
        }

        status = STATUS_SUCCESS;

        for (i = 0; i < RootHubData->NumberOfPorts; i++) {

            KeInitializeSpinLock(&RootHubData->PortData[i].ReadModifyWriteLock);

            //
            // Create per 2.0 port PortResumeComplete timer objects
            // which are used to time the TDRSMDN delay (duration of
            // driving resume to a downstream port; only from a
            // controlling hub).
            //
            if (RootHubData->PortData[i].MajorRevision == 2) {

                PROOTHUB_TIMER_DATA timerData;
                timerData = (PROOTHUB_TIMER_DATA) ExAllocatePoolWithTag(NonPagedPool,
                                                                        sizeof(ROOTHUB_TIMER_DATA),
                                                                        DriverAllocationTag);
                if (timerData == NULL) {
                    status = STATUS_INSUFFICIENT_RESOURCES;
                    LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                             "ExAllocatePool Failed %!STATUS!", status);
                    break;
                }

                timerData->RootHubData = RootHubData;
                timerData->PortNumber = i + 1;

                RootHubData->PortData[i].ExTimer20PortResumeComplete = ExAllocateTimer(RootHub_WdfEvtTimer20PortResumeComplete,
                                                                                       timerData,
                                                                                       EX_TIMER_HIGH_RESOLUTION);
                if (RootHubData->PortData[i].ExTimer20PortResumeComplete == NULL) {
                    status = STATUS_INSUFFICIENT_RESOURCES;
                    LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                             "ExAllocateTimer Failed %!STATUS!", status);
                    ExFreePoolWithTag(timerData, DriverAllocationTag);
                    break;
                }

                RootHubData->PortData[i].PortResumeTimerData = timerData;


            }
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (RootHubData->PortData != NULL) {

                for (i = 0; i < RootHubData->NumberOfPorts; i++) {

                    if (RootHubData->PortData[i].MajorRevision == 2) {

                        if (RootHubData->PortData[i].ExTimer20PortResumeComplete != NULL) {
                            ExDeleteTimer(RootHubData->PortData[i].ExTimer20PortResumeComplete,
                                          TRUE,
                                          FALSE,
                                          NULL);
                            RootHubData->PortData[i].ExTimer20PortResumeComplete = NULL;
                        }

                        if (RootHubData->PortData[i].PortResumeTimerData != NULL) {
                            ExFreePoolWithTag(RootHubData->PortData[i].PortResumeTimerData, DriverAllocationTag);
                            RootHubData->PortData[i].PortResumeTimerData = NULL;
                        }

                    }
                }

                ExFreePoolWithTag(RootHubData->PortData, DriverAllocationTag);
                RootHubData->PortData = NULL;
            }
        }
    }

    return status;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
RootHub_D0Entry(
    __in
        PROOTHUB_DATA           RootHubData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    )
/*++

Routine Description:

    This function is called on an entry to d0 event.

--*/
{
    ULONG   portNumber;
    PULONG  portSCRegister;
    PORTSC  portSC;
    KIRQL   irql;

    TRY {

        UNREFERENCED_PARAMETER(WdfPreviousState);

        RootHubData->NoInterruptTransfersSinceFDOD0Entry = TRUE;

        for (portNumber = 1; portNumber <= RootHubData->NumberOfPorts; portNumber++) {

            portSCRegister = &RootHubData->PortRegisterSet[portNumber-1].PortStatusAndControl.AsUlong32;
            portSC.AsUlong32 = Register_ReadUlong(portSCRegister);

            if (RootHubData->PortData[portNumber-1].MajorRevision == 2) {

                LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                        "2.0 Port %2d portSC 0x%08x", portNumber, portSC.AsUlong32);

            } else if (RootHubData->PortData[portNumber-1].MajorRevision == 3) {

                LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                        "3.0 Port %2d portSC 0x%08x", portNumber, portSC.AsUlong32);
            } else {

                LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                        "HW_COMPLIANCE: Port %2d has invalid MajorRevision value: %u", portNumber,
                        RootHubData->PortData[portNumber-1].MajorRevision);

                continue;
            }

            //
            // Some controllers will not detect connect/disconnect reliably if the
            // wake bits are not set, even while in D0.
            //
            if (Controller_GetDeviceFlags(RootHubData->ControllerHandle).ForceWakeEnableBitsInD0) {

                KeAcquireSpinLock(&RootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.WakeOnConnectEnable     = 1;
                portSC.WakeOnDisconnectEnable  = 1;
                portSC.WakeOnOverCurrentEnable = 1;
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&RootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);
            }

        }

    } FINALLY {

    }

    return STATUS_SUCCESS;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
RootHub_D0EntryPostInterruptsEnabled(
    __in
        PROOTHUB_DATA           RootHubData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    )
/*++

Routine Description:

    This function is called on an entry to d0 event.

--*/
{
    ULONG   portNumber;

    TRY {

        UNREFERENCED_PARAMETER(WdfPreviousState);

        for (portNumber = 1; portNumber <= RootHubData->NumberOfPorts; portNumber++) {

            //
            // If a port is in resume state, it is unclear if a port change event will be
            // generated by the controller.  To address this, we will manually check
            // all of the ports and acknowledge any port resume events.
            // Note that for USB 3.0 device initiated resume, as soon as the port returns
            // to U0, the device will send a function wake notification.  In order to receive
            // this notification, the controller needs to be running, interrupts enabled, and the
            // notification enabled in the controller.  Therefore, this function must be called
            // after that point.
            // That may mean that if the controller was going to generate a port change event
            // due to the resume, that event may have already been handled, and the resume
            // already acknowledged.  The RootHub_DetectAndAcknowledgePortResume function already
            // provides synchronization for this case.
            //
            RootHub_DetectAndAcknowledgePortResume(RootHubData,
                                                   portNumber,
                                                   TRUE);
        }

    } FINALLY {

    }

    return STATUS_SUCCESS;
}



__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
RootHub_D0Exit(
    __in
        PROOTHUB_DATA           RootHubData,
    __in
        WDF_POWER_DEVICE_STATE  WdfTargetState,
    __in
        SYSTEM_POWER_ACTION     SystemPowerAction
    )
/*++

Routine Description:

    This function is called on an exit from d0 event.

--*/
{
    ULONG           portNumber;
    NTSTATUS        status;
    KIRQL           irql;
    PULONG          portSCRegister;
    PORTSC          portSC;
    SUB_DX_STATE    subDxState;

    TRY {


        //
        // It is possible that a remote wake detected on a root port has already been acknowledged,
        // thus allowing the port to transition to U0.  Such ports need to be put back into U3 prior
        // to powering down the controller, in order to ensure proper wake detection, and to ensure
        // the link state remains valid if the controller disables its clocks.
        //
        RootHub_HandleResumedPorts(RootHubData);

        subDxState = SubDxStateDisconnected;

        //
        // Make sure that all of the ports are in an appropriate state before powering
        // down the controller.  Essentially, since we typically don't wait for the U3
        // transitions to complete, the transition may still be in progress.  If the
        // transitions don't happen in time, we need to go ahead and let the controller
        // power down.
        //
        for (portNumber = 1; portNumber <= RootHubData->NumberOfPorts; portNumber++) {

            if (WdfTargetState != WdfPowerDeviceD3Final) {

                status = RootHub_WaitForPendingU3TransitionCompletion(RootHubData,
                                                                      portNumber,
                                                                      FALSE);
                //
                // If this D state change is due to a system state change, failures of the
                // U3 transitions will be ignored, otherwise the system would need to wake
                // back up just to handle the failure.  However, if this D state change is due
                // to runtime idle power management, then the root hub's WAIT_WAKE irp should
                // be completed so that the hub driver can initiate recovery.  This should
                // be sufficient to avoid the user being left with a "dead" device. The wake
                // triggering is accomplished by indicating that there is a port status change
                // to UCX.
                //
                if (!NT_SUCCESS(status) &&
                    (SystemPowerAction == SystemPowerAction_None)) {
                    UcxRootHubPortChanged(RootHubData->UcxRootHub);
                }

                //
                // If atleast one port is connected, then the state is connected
                //
                if (RootHub_IsPortConnected(RootHubData,
                                            portNumber)) {
                    subDxState = SubDxStateConnected;
                }
            }

            if (Controller_GetDeviceFlags(RootHubData->ControllerHandle).ClearCSCPriorToD0Exit) {











                portSCRegister = &RootHubData->PortRegisterSet[portNumber-1].PortStatusAndControl.AsUlong32;
                KeAcquireSpinLock(&RootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);
                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.ConnectStatusChange = 1;
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);
                KeReleaseSpinLock(&RootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);
            }

            if (SystemPoweringDown(SystemPowerAction) &&
                Controller_GetDeviceFlags(RootHubData->ControllerHandle).ForceWakeEnableBitsInD0) {
                //
                // Typically the ports are disarmed for wake when exiting D3 while in S0.
                // If the controller requires the ports to be armed for wake whenever in D0,
                // then they need to be disarmed here if the system is leaving S0.
                //
                portSCRegister = &RootHubData->PortRegisterSet[portNumber-1].PortStatusAndControl.AsUlong32;
                KeAcquireSpinLock(&RootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);
                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.WakeOnConnectEnable     = 0;
                portSC.WakeOnDisconnectEnable  = 0;
                portSC.WakeOnOverCurrentEnable = 0;
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);
                KeReleaseSpinLock(&RootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);
            }

            RootHub_DumpPortData(&RootHubData->PortRegisterSet[portNumber-1].PortStatusAndControl,
                                 RootHubData->ControllerHandle);
        }

        Controller_NotifyPortStateToPep(RootHubData->ControllerHandle,
                                        subDxState);

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
RootHub_ReleaseHardware(
    __in
        PROOTHUB_DATA   RootHubData
    )
/*++

Routine Description:

    This function is called on a release hardware event and frees any
    resources used by the root hub.

--*/
{
    ULONG   i;

    PAGED_CODE();

    TRY {

        if (RootHubData->PortData != NULL) {

            for (i = 0; i < RootHubData->NumberOfPorts; i++) {

                if (RootHubData->PortData[i].MajorRevision == 2) {

                    if (RootHubData->PortData[i].ExTimer20PortResumeComplete != NULL) {
                        ExDeleteTimer(RootHubData->PortData[i].ExTimer20PortResumeComplete,
                                      TRUE,
                                      FALSE,
                                      NULL);
                    }

                    if (RootHubData->PortData[i].PortResumeTimerData != NULL) {
                        ExFreePoolWithTag(RootHubData->PortData[i].PortResumeTimerData, DriverAllocationTag);
                    }
                }
            }

            ExFreePoolWithTag(RootHubData->PortData, DriverAllocationTag);

            RootHubData->PortData = NULL;
        }

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
RootHub_ControllerResetPostReset(
    __in
        PROOTHUB_DATA   RootHubData
    )
/*++

Routine Description:

    This routine is called when the non-PnP controller reset process is
    in progress. The controller has been reset and the controller has
    not yet been started again.

--*/
{
    TRY {

        //
        // Treat this scenario as though we are re-initializing after D0Entry,
        // which also assumes that the controller may have been reset.
        //

        (VOID)RootHub_D0Entry(RootHubData, WdfPowerDeviceD3);

    } FINALLY {

    }

    return;
}

VOID
RootHub_UcxEvtClearHubFeature(
    UCXROOTHUB  UcxRootHub,
    WDFREQUEST  WdfRequest
    )
/*++

Routine Description:


Arguments:

    UcxRootHub - The UCX root hub object.

    WdfRequest - The WDF request object for the Clear Hub Feature request.

Return Value:

    None directly from this routine.

    The request completion status in the WdfRequest and the USBD_STATUS
    in the urb header.

--*/
{
    NTSTATUS                        status;
    PROOTHUB_DATA                   rootHubData;
    WDF_REQUEST_PARAMETERS          wdfRequestParams;
    PURB                            urb;
    PWDF_USB_CONTROL_SETUP_PACKET   setupPacket;
    ULONG                           featureSelector;

    UNREFERENCED_PARAMETER(UcxRootHub);

    TRY {

        rootHubData = GetRootHubData(UcxRootHub);

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        urb = (PURB)wdfRequestParams.Parameters.Others.Arg1;
        setupPacket = (PWDF_USB_CONTROL_SETUP_PACKET)&urb->UrbControlTransferEx.SetupPacket[0];

        //
        // USB Specification 2.0, 11.24.2.1 Clear Hub Feature
        // USB Specification 3.0, 10.14.2.1 Clear Hub Feature
        //
        featureSelector = setupPacket->Packet.wValue.Value;

        if ((setupPacket->Packet.bm.Byte != 0x20) ||
            (setupPacket->Packet.bRequest != USB_REQUEST_CLEAR_FEATURE) ||
            (setupPacket->Packet.wIndex.Value != 0) ||
            (setupPacket->Packet.wLength != 0)) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                     setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                     setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                     setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                     setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

            urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        switch (featureSelector) {

        case C_HUB_LOCAL_POWER:
            LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "C_HUB_LOCAL_POWER");
            urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
            status = STATUS_SUCCESS;
            break;

        case C_HUB_OVER_CURRENT:
            LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "C_HUB_OVER_CURRENT");
            urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
            status = STATUS_SUCCESS;
            break;

        default:
            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                     setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                     setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                     setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                     setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

            urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

    } FINALLY {

        NT_ASSERT(status == STATUS_SUCCESS);
        WdfRequestComplete(WdfRequest, status);
    }

    return;
}

VOID
RootHub_UcxEvtClearPortFeature(
    UCXROOTHUB  UcxRootHub,
    WDFREQUEST  WdfRequest
    )
/*++

Routine Description:


Arguments:

    UcxRootHub - The UCX root hub object.

    WdfRequest - The WDF request object for the Clear Port Feature request.

Return Value:

    None directly from this routine.

    The request completion status in the WdfRequest and the USBD_STATUS
    in the urb header.

--*/
{
    NTSTATUS                        status;
    PROOTHUB_DATA                   rootHubData;
    WDF_REQUEST_PARAMETERS          wdfRequestParams;
    PURB                            urb;
    PWDF_USB_CONTROL_SETUP_PACKET   setupPacket;
    ULONG                           portNumber;
    ULONG                           featureSelector;
    ULONG                           featureSpecificValue;
    PULONG                          portSCRegister;
    PULONG                          portPMSCRegister;
    PORTSC                          portSC;
    PORTPMSC                        portPMSC;
    USBSTS                          usbStatus;
    KIRQL                           irql;
    USHORT                          resumeTime;

    TRY {

        rootHubData = GetRootHubData(UcxRootHub);

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        urb = (PURB)wdfRequestParams.Parameters.Others.Arg1;
        setupPacket = (PWDF_USB_CONTROL_SETUP_PACKET)&urb->UrbControlTransferEx.SetupPacket[0];

        if (Controller_IsControllerAccessible(rootHubData->ControllerHandle) == FALSE) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "RootHub_UcxEvtClearPortFeature: controller gone");

            urb->UrbHeader.Status = USBD_STATUS_DEVICE_GONE;
            status = STATUS_NO_SUCH_DEVICE;
            LEAVE;
        }

        //
        // Software shall ensure that the xHC is running (HCHalted == '0')
        // before attempting to write to the Port Status and Control
        // Register (PORTSC).
        //
        usbStatus.AsUlong32 = Register_ReadUlong(&rootHubData->OperationalRegisters->UsbStatus.AsUlong32);

        if (usbStatus.AsUlong32 == HARDWARE_GONE_ULONG32_REGISTER_VALUE) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "HW_COMPLIANCE: controller gone detected!");

            Controller_SetControllerGone(rootHubData->ControllerHandle, TRUE);

            urb->UrbHeader.Status = USBD_STATUS_DEVICE_GONE;
            status = STATUS_NO_SUCH_DEVICE;
            LEAVE;
        }

        if (usbStatus.HcHalted != FALSE) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "HW_COMPLIANCE: Controller halted!");
            NT_ASSERTMSG("Controller halted", 0);

            urb->UrbHeader.Status = USBD_STATUS_XACT_ERROR;
            status = STATUS_ADAPTER_HARDWARE_ERROR;
            LEAVE;
        }

        //
        // USB Specification 2.0, 11.24.2.2 Clear Port Feature
        // USB Specification 3.0, 10.14.2.2 Clear Port Feature
        //
        featureSelector      = setupPacket->Packet.wValue.Value;
        portNumber           = setupPacket->Packet.wIndex.Bytes.LowByte;
        featureSpecificValue = setupPacket->Packet.wIndex.Bytes.HiByte;

        if ((setupPacket->Packet.bm.Byte != 0x23) ||
            (setupPacket->Packet.bRequest != USB_REQUEST_CLEAR_FEATURE) ||
            (portNumber == 0) ||
            (portNumber > rootHubData->NumberOfPorts) ||
            (setupPacket->Packet.wLength != 0)) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                     setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                     setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                     setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                     setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

            urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        portSCRegister   = &rootHubData->PortRegisterSet[portNumber-1].
                           PortStatusAndControl.AsUlong32;

        portPMSCRegister = &rootHubData->PortRegisterSet[portNumber-1].
                           PortPowerManagementStatusAndControl.AsUlong32;

        if (rootHubData->PortData[portNumber-1].MajorRevision == 2) {

            LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "2.0 Port %2d, Feature Selector %2d, Feature Specific Value %3d",
                    portNumber, featureSelector, featureSpecificValue);

            //
            // The most significant byte of wIndex is zero, except when
            // the feature selector is PORT_INDICATOR.
            //
            if ((featureSelector != PORT_INDICATOR) &&
                (featureSpecificValue != 0)) {

                LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                         "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                         setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                         setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                         setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                         setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

                urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            switch (featureSelector) {

            case PORT_ENABLE:

                //
                // Clearing the PORT_ENABLE feature causes the port
                // to be placed in the Disabled state.
                //
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_ENABLE: Before portSC  0x%08x", portSC.AsUlong32);

                if (Controller_GetDeviceFlags(rootHubData->ControllerHandle).Map20PortDisableToSuspend) {

                    //
                    // 4.15.1 Port Suspend
                    // Software should not attempt to suspend a port
                    // unless the port reports that it is in the
                    // enabled (PED = '1', PLS < '3') state.
                    //
                    if ((portSC.PortEnabled == 0) || (portSC.PortLinkState >= PORT_LINK_STATE_U3)) {
                        LogWarning(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                                   "PORT_SUSPEND: port is not in the enabled state");
                    }

                    portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                    portSC.PortLinkState = PORT_LINK_STATE_U3;
                    portSC.PortLinkStateWriteStrobe = 1;
                    LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                            "PORT_SUSPEND: Writing portSC 0x%08x", portSC.AsUlong32);
                    Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                } else {

                    portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                    portSC.PortEnabled = 1;
                    LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                            "PORT_ENABLE: Writing portSC 0x%08x", portSC.AsUlong32);
                    Register_WriteUlong(portSCRegister, portSC.AsUlong32);
                }

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_ENABLE: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case PORT_SUSPEND:

                //
                // 4.15.2 Port Resume
                // Software shall not attempt to resume a port that it has initiated the
                // supsend process on unless the port reports that it is in the suspended
                // (PED = '1', PLS = '3') state.
                //
                // Wait for any pending suspend transitions to complete before starting resume.
                //
                status = RootHub_WaitForPendingU3TransitionCompletion(rootHubData, portNumber, FALSE);

                if (!NT_SUCCESS(status)) {
                    //
                    // The best urb status to pick in this case might be STALL
                    // to simulate that the hub did not like that command at
                    // that time
                    //
                    urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                    LEAVE;
                }

                //
                // Clearing the PORT_SUSPEND feature causes a
                // host-initiated resume on the specified port.
                //
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_SUSPEND: Before portSC  0x%08x", portSC.AsUlong32);

                if ((portSC.PortEnabled == 0) ||
                    ((portSC.PortLinkState != PORT_LINK_STATE_U3) &&
                     (portSC.PortLinkState != PORT_LINK_STATE_RESUME))) {

                    //
                    // We can be in this situation in the following cases:
                    // 1. Software and hardware are racing to resume the port and hardware won.
                    // 2. Device got disconnected right before software tried to resume the port.
                    // 3. Port is in a bad state.
                    //
                    LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                               "PORT_SUSPEND: port is not in the suspended state");
                }

                //
                // For a USB2 protcol port, software shall write a
                // '15' (Resume) to the PLS field to initiate
                // resume signaling.
                //
                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortLinkState = PORT_LINK_STATE_RESUME;
                portSC.PortLinkStateWriteStrobe = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_SUSPEND: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_SUSPEND: After portSC   0x%08x", portSC.AsUlong32);

                resumeTime = TDRSMDN_IN_MS;

                if (rootHubData->PortData[portNumber-1].ResumeTimeReferenceCount > 0) {
                    resumeTime = rootHubData->PortData[portNumber-1].ResumeTime;
                }

                //
                // Software shall ensure that resume is signaled for at
                // least 20 ms (TDRSMDN).  Software shall start timing
                // TDRSMDN from the write of '15' (Resume) to PLS.
                // After TDRSMDN is complete, software shall write a '0'
                // (U0) to the PLS field.
                //
                ExSetTimer(rootHubData->PortData[portNumber-1].ExTimer20PortResumeComplete,
                           WDF_REL_TIMEOUT_IN_MS(resumeTime),
                           0,
                           NULL);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case PORT_POWER:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_POWER: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortPower = 0;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_POWER: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_POWER: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case PORT_INDICATOR:
                //
                // USB Specification 2.0, Table 11-25.
                // Port Indicator Selector Codes
                //
                if (featureSpecificValue > 3) {

                    LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                             "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                             setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                             setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                             setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                             setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

                    urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                    status = STATUS_UNSUCCESSFUL;
                    LEAVE;
                }

                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_INDICATOR: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortIndicatorControl = 0;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_INDICATOR: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_INDICATOR: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case C_PORT_CONNECTION:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_CONNECTION: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.ConnectStatusChange = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_CONNECTION: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_CONNECTION: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case C_PORT_RESET:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_RESET: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortResetChange = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_RESET: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_RESET: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case C_PORT_ENABLE:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_ENABLE: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortEnabledOrDisabledChange = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_ENABLE: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_ENABLE: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case C_PORT_SUSPEND:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_SUSPEND: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortLinkStateChange = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_SUSPEND: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_SUSPEND: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case C_PORT_OVER_CURRENT:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_OVER_CURRENT: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.OverCurrentChange = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_OVER_CURRENT: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_OVER_CURRENT: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            default:
                LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                         "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                         setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                         setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                         setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                         setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

                urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

        } else if (rootHubData->PortData[portNumber-1].MajorRevision == 3) {

            LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "3.0 Port %2d, Feature Selector %2d, Feature Specific Value %3d",
                    portNumber, featureSelector, featureSpecificValue);

            // The most significant byte of wIndex is zero.
            //
            if (featureSpecificValue != 0) {
                LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                         "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                         setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                         setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                         setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                         setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

                urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            switch (featureSelector) {

            case PORT_POWER:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_POWER: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortPower = 0;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_POWER: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_POWER: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case C_PORT_CONNECTION:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_CONNECTION: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.ConnectStatusChange = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_CONNECTION: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_CONNECTION: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case C_PORT_RESET:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_RESET: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortResetChange = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_RESET: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_RESET: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case C_PORT_OVER_CURRENT:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_OVER_CURRENT: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.OverCurrentChange = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_OVER_CURRENT: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_OVER_CURRENT: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case C_PORT_LINK_STATE:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_LINK_STATE: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortLinkStateChange = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_LINK_STATE: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_LINK_STATE: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case C_PORT_CONFIG_ERROR:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_CONFIG_ERROR: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortConfigErrorChange = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_CONFIG_ERROR: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_PORT_CONFIG_ERROR: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case C_BH_PORT_RESET:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_BH_PORT_RESET: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.WarmPortResetChange = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_BH_PORT_RESET: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "C_BH_PORT_RESET: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case FORCE_LINKPM_ACCEPT:
                //
                // See also: Q1'09 USB 3.0 Errata (Released 05/15/2009)
                // FORCE_LINKPM_ACCEPT Feature left out
                //
                portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "FORCE_LINKPM_ACCEPT: Before portPMSC  0x%08x", portPMSC.AsUlong32);

                portPMSC.Usb3.ForceLinkPMAccept = 0;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "FORCE_LINKPM_ACCEPT: Writing portPMSC 0x%08x", portPMSC.AsUlong32);
                Register_WriteUlong(portPMSCRegister, portPMSC.AsUlong32);

                portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "FORCE_LINKPM_ACCEPT: After portPMSC   0x%08x", portPMSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            default:
                LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                         "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                         setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                         setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                         setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                         setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

                urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

        } else {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                     setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                     setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                     setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                     setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

            urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

    } FINALLY {

        WdfRequestComplete(WdfRequest, status);
    }

    return;
}

VOID
RootHub_UcxEvtGetHubStatus(
    UCXROOTHUB  UcxRootHub,
    WDFREQUEST  WdfRequest
    )
/*++

Routine Description:


Arguments:

    UcxRootHub - The UCX root hub object.

    WdfRequest - The WDF request object for the Get Hub Status request.

Return Value:

    None directly from this routine.

    The request completion status in the WdfRequest and the USBD_STATUS
    in the urb header.

--*/
{
    NTSTATUS                        status;
    PROOTHUB_DATA                   rootHubData;
    WDF_REQUEST_PARAMETERS          wdfRequestParams;
    PURB                            urb;
    PWDF_USB_CONTROL_SETUP_PACKET   setupPacket;

    UNREFERENCED_PARAMETER(UcxRootHub);

    TRY {

        rootHubData = GetRootHubData(UcxRootHub);

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        urb = (PURB)wdfRequestParams.Parameters.Others.Arg1;
        setupPacket = (PWDF_USB_CONTROL_SETUP_PACKET)&urb->UrbControlTransferEx.SetupPacket[0];

        //
        // USB Specification 2.0, 11.24.2.6 Get Hub Status
        // USB Specification 3.0, 10.14.2.4 Get Hub Status
        //
        if ((setupPacket->Packet.bm.Byte != 0xA0) ||
            (setupPacket->Packet.bRequest != USB_REQUEST_GET_STATUS) ||
            (setupPacket->Packet.wValue.Value != 0) ||
            (setupPacket->Packet.wIndex.Value != 0) ||
            (setupPacket->Packet.wLength != 4)) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                     setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                     setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                     setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                     setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

            urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        //
        // The xHCI root hub does not report loss of local power or
        // over-current, or change of local power or change of
        // over-current.
        //
        *((PULONG)urb->UrbControlTransferEx.TransferBuffer) = 0;

        urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
        status = STATUS_SUCCESS;

    } FINALLY {

        NT_ASSERT(status == STATUS_SUCCESS);
        WdfRequestComplete(WdfRequest, status);
    }

    return;
}

VOID
RootHub_UcxEvtGetPortStatus(
    UCXROOTHUB  UcxRootHub,
    WDFREQUEST  WdfRequest
    )
/*++

Routine Description:


Arguments:

    UcxRootHub - The UCX root hub object.

    WdfRequest - The WDF request object for the Get Port Status request.

Return Value:

    None directly from this routine.

    The request completion status in the WdfRequest and the USBD_STATUS
    in the urb header.

--*/
{
    NTSTATUS                        status;
    PROOTHUB_DATA                   rootHubData;
    WDF_REQUEST_PARAMETERS          wdfRequestParams;
    PURB                            urb;
    PWDF_USB_CONTROL_SETUP_PACKET   setupPacket;
    ULONG                           portNumber;
    ULONG                           packetLength;
    PULONG                          portSCRegister;
    PULONG                          portLIRegister;
    PORTSC                          portSC;
    PORTLI                          portLI;
    PUSB_PORT_STATUS_AND_CHANGE     pusbPortStatusChange;
    PUSB_PORT_EXT_STATUS_AND_CHANGE pusbPortExtStatusChange;

    TRY {

        rootHubData = GetRootHubData(UcxRootHub);

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        urb = (PURB)wdfRequestParams.Parameters.Others.Arg1;
        setupPacket = (PWDF_USB_CONTROL_SETUP_PACKET)&urb->UrbControlTransferEx.SetupPacket[0];

        if (Controller_IsControllerAccessible(rootHubData->ControllerHandle) == FALSE) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "RootHub_UcxEvtGetPortStatus: controller gone");

            urb->UrbHeader.Status = USBD_STATUS_DEVICE_GONE;
            status = STATUS_NO_SUCH_DEVICE;
            LEAVE;
        }

        //
        // USB Specification 2.0, 11.24.2.7 Get Port Status
        // USB Specification 3.0, 10.14.2.6 Get Port Status
        //
        portNumber = setupPacket->Packet.wIndex.Value;
        packetLength = (setupPacket->Packet.wValue.Value == USB_STATUS_EXT_PORT_STATUS) ? 8 : 4;

        if ((setupPacket->Packet.bm.Byte != 0xA3) ||
            (setupPacket->Packet.bRequest != USB_REQUEST_GET_STATUS) ||
            ((setupPacket->Packet.wValue.Value != USB_STATUS_PORT_STATUS) &&
             (setupPacket->Packet.wValue.Value != USB_STATUS_EXT_PORT_STATUS)) ||
            (portNumber == 0) ||
            (portNumber > rootHubData->NumberOfPorts) ||
            (setupPacket->Packet.wLength != packetLength)) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                     setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                     setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                     setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                     setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

            urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        portSCRegister = &rootHubData->PortRegisterSet[portNumber-1].PortStatusAndControl.AsUlong32;
        portSC.AsUlong32 = Register_ReadUlong(portSCRegister);

        if ((rootHubData->PortData[portNumber-1].MajorRevision == 2) &&
            (setupPacket->Packet.wValue.Value == USB_STATUS_PORT_STATUS)) {

            if (portSC.AsUlong32 == HARDWARE_GONE_ULONG32_REGISTER_VALUE) {

                LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "2.0 controller gone detected");

                Controller_SetControllerGone(rootHubData->ControllerHandle, TRUE);

                urb->UrbHeader.Status = USBD_STATUS_DEVICE_GONE;
                status = STATUS_NO_SUCH_DEVICE;
                LEAVE;
            }

            pusbPortStatusChange = (PUSB_PORT_STATUS_AND_CHANGE)urb->UrbControlTransferEx.TransferBuffer;
            pusbPortStatusChange->AsUlong32 = 0;

            LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "2.0 Port %2d, portSC 0x%08x", portNumber, portSC.AsUlong32);

            //
            // USB Specification 2.0, 11.24.2.7.1 Port Status Bits
            //
            pusbPortStatusChange->PortStatus.Usb20PortStatus.CurrentConnectStatus =
                (USHORT)portSC.CurrentConnectStatus;

            pusbPortStatusChange->PortStatus.Usb20PortStatus.PortEnabledDisabled =
                (USHORT)portSC.PortEnabled;

            switch (portSC.PortLinkState) {

            case PORT_LINK_STATE_U2:
                pusbPortStatusChange->PortStatus.Usb20PortStatus.L1 = 1;
                break;

            case PORT_LINK_STATE_U3:
            case PORT_LINK_STATE_RESUME:
                pusbPortStatusChange->PortStatus.Usb20PortStatus.Suspend = 1;
                break;

            case PORT_LINK_STATE_TEST_MODE:
                pusbPortStatusChange->PortStatus.Usb20PortStatus.PortTestMode = 1;
                LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "HW_COMPLIANCE: 2.0 Port %2d, is in TEST MODE", portNumber);
                break;

            default:
                break;
            }

            pusbPortStatusChange->PortStatus.Usb20PortStatus.OverCurrent =
                (USHORT)portSC.OverCurrentActive;

            pusbPortStatusChange->PortStatus.Usb20PortStatus.Reset =
                (USHORT)portSC.PortReset;

            pusbPortStatusChange->PortStatus.Usb20PortStatus.PortPower =
                (USHORT)portSC.PortPower;

            if (portSC.PortSpeed == PORT_SPEED_LOW_SPEED_DEVICE_ATTACHED) {

                pusbPortStatusChange->PortStatus.Usb20PortStatus.LowSpeedDeviceAttached = 1;

            } else if (portSC.PortSpeed == PORT_SPEED_HIGH_SPEED_DEVICE_ATTACHED) {

                pusbPortStatusChange->PortStatus.Usb20PortStatus.HighSpeedDeviceAttached = 1;
            }

            if (portSC.PortIndicatorControl != 0) {

                pusbPortStatusChange->PortStatus.Usb20PortStatus.PortIndicatorControl = 1;
            }

            //
            // USB Specification 2.0, 11.24.2.7.2 Port Status Change Bits
            //
            pusbPortStatusChange->PortChange.Usb20PortChange.ConnectStatusChange =
                (USHORT)portSC.ConnectStatusChange;

            pusbPortStatusChange->PortChange.Usb20PortChange.PortEnableDisableChange =
                (USHORT)portSC.PortEnabledOrDisabledChange;

            if (portSC.PortLinkState == PORT_LINK_STATE_RESUME) {
                pusbPortStatusChange->PortChange.Usb20PortChange.SuspendChange = 0;
            } else {
                pusbPortStatusChange->PortChange.Usb20PortChange.SuspendChange =
                    (USHORT)portSC.PortLinkStateChange;
            }

            pusbPortStatusChange->PortChange.Usb20PortChange.OverCurrentIndicatorChange =
                (USHORT)portSC.OverCurrentChange;

            pusbPortStatusChange->PortChange.Usb20PortChange.ResetChange =
                (USHORT)portSC.PortResetChange;

            LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "PortStatus 0x%04x PortChange 0x%04x",
                    pusbPortStatusChange->PortStatus.Usb20PortStatus.AsUshort16,
                    pusbPortStatusChange->PortChange.Usb20PortChange.AsUshort16);

            if (portSC.PortLinkState == PORT_LINK_STATE_RESUME) {
                //
                // Consider the case where controller was asleep and it wakes up to
                // handle a device resuming. WHenver controller wakes up, it also
                // wakes up the root hub right away. Now, as mentioned above, the root
                // hub will not see any change until the resume is acknowledged. As a
                // result, Root hub might start idling down. That considerably increases
                // the probability of hub idle down process overlapping with  a 2.0 port
                // resume process. This overlap can lead to 2.0 port being suspended right
                // after it completes resuming, which can confuse some legacy devices.
                // To avoid this problem, this special pending URB status is being returned.
                // When the hub sees this status, it will acquire a power reference and
                // will only release it when it sees a new get port status with status success.
                //
                urb->UrbHeader.Status = USBD_STATUS_PORT_OPERATION_PENDING;
            } else {
                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
            }

            status = STATUS_SUCCESS;

        } else if (rootHubData->PortData[portNumber-1].MajorRevision == 3) {

            if (portSC.AsUlong32 == HARDWARE_GONE_ULONG32_REGISTER_VALUE) {

                LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "3.0 controller gone detected");

                Controller_SetControllerGone(rootHubData->ControllerHandle, TRUE);

                urb->UrbHeader.Status = USBD_STATUS_DEVICE_GONE;
                status = STATUS_NO_SUCH_DEVICE;
                LEAVE;
            }

            pusbPortStatusChange = (PUSB_PORT_STATUS_AND_CHANGE)urb->UrbControlTransferEx.TransferBuffer;
            pusbPortStatusChange->AsUlong32 = 0;

            LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "3.0 Port %2d, portSC 0x%08x", portNumber, portSC.AsUlong32);

            if (RootHub_HideInvalidDebugPortStatusAndChange(rootHubData, portNumber)) {

                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "For Debug Port %2d, reporting default Port Status and Change to hub driver", portNumber);

                pusbPortStatusChange->PortStatus.Usb30PortStatus.CurrentConnectStatus       = 0;
                pusbPortStatusChange->PortStatus.Usb30PortStatus.PortEnabledDisabled        = 0;
                pusbPortStatusChange->PortStatus.Usb30PortStatus.OverCurrent                = 0;
                pusbPortStatusChange->PortStatus.Usb30PortStatus.Reset                      = 0;
                pusbPortStatusChange->PortStatus.Usb30PortStatus.PortLinkState              = PORT_LINK_STATE_RX_DETECT;
                pusbPortStatusChange->PortStatus.Usb30PortStatus.PortPower                  = 1;

                pusbPortStatusChange->PortChange.Usb30PortChange.ConnectStatusChange        = 0;
                pusbPortStatusChange->PortChange.Usb30PortChange.OverCurrentIndicatorChange = 0;
                pusbPortStatusChange->PortChange.Usb30PortChange.ResetChange                = 0;
                pusbPortStatusChange->PortChange.Usb30PortChange.BHResetChange              = 0;
                pusbPortStatusChange->PortChange.Usb30PortChange.PortLinkStateChange        = 0;
                pusbPortStatusChange->PortChange.Usb30PortChange.PortConfigErrorChange      = 0;

                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PortStatus 0x%04x PortChange 0x%04x",
                        pusbPortStatusChange->PortStatus.Usb30PortStatus.AsUshort16,
                        pusbPortStatusChange->PortChange.Usb30PortChange.AsUshort16);

            } else {

                //
                // USB Specification 3.0, 10.14.2.6.1 Port Status Bits
                //
                pusbPortStatusChange->PortStatus.Usb30PortStatus.CurrentConnectStatus =
                    (USHORT)portSC.CurrentConnectStatus;

                pusbPortStatusChange->PortStatus.Usb30PortStatus.PortEnabledDisabled =
                    (USHORT)portSC.PortEnabled;

                pusbPortStatusChange->PortStatus.Usb30PortStatus.OverCurrent =
                    (USHORT)portSC.OverCurrentActive;

                pusbPortStatusChange->PortStatus.Usb30PortStatus.Reset =
                    (USHORT)portSC.PortReset;

                pusbPortStatusChange->PortStatus.Usb30PortStatus.PortLinkState =
                    (USHORT)portSC.PortLinkState;

                //
                // If the hub driver happens to read the port state before the XHCI driver has handled the resume
                // event, the link state may be RESUME, which isn't a valid link state for a hub.  To hide this
                // from the hub, RESUME will be converted to RECOVERY, which will be the state as soon as
                // the resume is acknowledged.
                //
                if (pusbPortStatusChange->PortStatus.Usb30PortStatus.PortLinkState == PORT_LINK_STATE_RESUME) {
                    pusbPortStatusChange->PortStatus.Usb30PortStatus.PortLinkState = PORT_LINK_STATE_RECOVERY;
                }

                //
                // When the host controller is powered down and can't complete the training
                // sequence when a SuperSpeed device is attached, it sets the Cold
                // Attach Status (CAS) bit.  The device is likely in the compliance state
                // at this point and needs a warm reset to restart the training sequence.
                // Inform the hub driver that the state is compliance, and it will do the
                // right thing.
                //
                if (portSC.ColdAttachStatus == 1) {
                    pusbPortStatusChange->PortStatus.Usb30PortStatus.PortLinkState =
                        PORT_LINK_STATE_COMPLIANCE_MODE;
                }
                pusbPortStatusChange->PortStatus.Usb30PortStatus.PortPower =
                    (USHORT)portSC.PortPower;

                //
                // USB Specification 3.0, 10.14.2.6.2 Port Status Change Bits
                //
                pusbPortStatusChange->PortChange.Usb30PortChange.ConnectStatusChange =
                    (USHORT)portSC.ConnectStatusChange;

                pusbPortStatusChange->PortChange.Usb30PortChange.OverCurrentIndicatorChange =
                    (USHORT)portSC.OverCurrentChange;

                pusbPortStatusChange->PortChange.Usb30PortChange.ResetChange =
                    (USHORT)portSC.PortResetChange;

                pusbPortStatusChange->PortChange.Usb30PortChange.BHResetChange =
                    (USHORT)portSC.WarmPortResetChange;

                pusbPortStatusChange->PortChange.Usb30PortChange.PortLinkStateChange =
                    (USHORT)portSC.PortLinkStateChange;

                pusbPortStatusChange->PortChange.Usb30PortChange.PortConfigErrorChange =
                    (USHORT)portSC.PortConfigErrorChange;

                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PortStatus 0x%04x PortChange 0x%04x",
                        pusbPortStatusChange->PortStatus.Usb30PortStatus.AsUshort16,
                        pusbPortStatusChange->PortChange.Usb30PortChange.AsUshort16);

                //
                // USB Specification 3.1, 10.16.2.6.3 Extended Port Status Bits
                //
                if (setupPacket->Packet.wValue.Value == USB_STATUS_EXT_PORT_STATUS) {
                    portLIRegister = &rootHubData->PortRegisterSet[portNumber-1].PortLinkInfo.AsUlong32;
                    portLI.AsUlong32 = Register_ReadUlong(portLIRegister);

                    pusbPortExtStatusChange = (PUSB_PORT_EXT_STATUS_AND_CHANGE)pusbPortStatusChange;
                    pusbPortExtStatusChange->PortExtStatus.AsUlong32 = 0;

                    //
                    // If a controller does not have a table with speeds and the port speed is undefined
                    // (i.e., unacceptable as a default value for 3.0 port), then a controller does not
                    // report speed in portSC correctly; assume SuperSpeed mode in this case.
                    // For ports with no devices attached, we should not care about speeds since
                    // USB_STATUS_EXT_PORT_STATUS returns a meaningful value only if devices are attached.
                    //
                    if ((rootHubData->PortData[portNumber-1].SpeedsCount == 0) &&
                        (portSC.PortSpeed == PORT_SPEED_UNDEFINED_DEVICE_ATTACHED)) {

                        pusbPortExtStatusChange->PortExtStatus.RxSublinkSpeedID = PORT_SPEED_SUPER_SPEED_DEVICE_ATTACHED;
                        pusbPortExtStatusChange->PortExtStatus.TxSublinkSpeedID = PORT_SPEED_SUPER_SPEED_DEVICE_ATTACHED;
                    } else {
                        pusbPortExtStatusChange->PortExtStatus.RxSublinkSpeedID = portSC.PortSpeed;
                        pusbPortExtStatusChange->PortExtStatus.TxSublinkSpeedID = portSC.PortSpeed;
                    }

                    pusbPortExtStatusChange->PortExtStatus.RxLaneCount = portLI.RxLaneCount;
                    pusbPortExtStatusChange->PortExtStatus.TxLaneCount = portLI.TxLaneCount;

                    LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PortExtStatus 0x%08x",
                        pusbPortExtStatusChange->PortExtStatus.AsUlong32);
                }
            }

            urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
            status = STATUS_SUCCESS;

        } else {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                     setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                     setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                     setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                     setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

            urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }


    } FINALLY {

        WdfRequestComplete(WdfRequest, status);
    }

    return;
}

VOID
RootHub_UcxEvtSetHubFeature(
    UCXROOTHUB  UcxRootHub,
    WDFREQUEST  WdfRequest
    )
/*++

Routine Description:


Arguments:

    UcxRootHub - The UCX root hub object.

    WdfRequest - The WDF request object for the Set Hub Feature request.

Return Value:

    None directly from this routine.

    The request completion status in the WdfRequest and the USBD_STATUS
    in the urb header.

--*/
{
    NTSTATUS                status;
    WDF_REQUEST_PARAMETERS  wdfRequestParams;
    PURB                    urb;

    UNREFERENCED_PARAMETER(UcxRootHub);

    TRY {

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        urb = (PURB)wdfRequestParams.Parameters.Others.Arg1;

        //
        // USB Specification 2.0, 11.24.2.6 Get Hub Status
        // USB Specification 3.0, 10.14.2.4 Get Hub Status
        //
        // "Hubs may allow setting of these change bits with
        // SetHubFeature() requests in order to support diagnostics.  If
        // the hub does not support setting of these bits, it should
        // either treat the SetHubFeature() request as a Request Error
        // or as a functional no-operation."
        //
        urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
        status = STATUS_UNSUCCESSFUL;

    } FINALLY {

        NT_ASSERT(status == STATUS_SUCCESS);
        WdfRequestComplete(WdfRequest, status);
    }

    return;
}

VOID
RootHub_UcxEvtSetPortFeature(
    UCXROOTHUB  UcxRootHub,
    WDFREQUEST  WdfRequest
    )
/*++

Routine Description:


Arguments:

    UcxRootHub - The UCX root hub object.

    WdfRequest - The WDF request object for the Set Port Feature request.

Return Value:

    None directly from this routine.

    The request completion status in the WdfRequest and the USBD_STATUS
    in the urb header.

--*/
{
    NTSTATUS                        status;
    PROOTHUB_DATA                   rootHubData;
    WDF_REQUEST_PARAMETERS          wdfRequestParams;
    PURB                            urb;
    PWDF_USB_CONTROL_SETUP_PACKET   setupPacket;
    ULONG                           portNumber;
    ULONG                           featureSelector;
    ULONG                           featureSpecificValue;
    PULONG                          portSCRegister;
    PULONG                          portPMSCRegister;
    PULONG                          portHLPMCRegister;
    PORTSC                          portSC;
    PORTPMSC                        portPMSC;
    PORTHLPMC                       portHLPMC;
    USBSTS                          usbStatus;
    KIRQL                           irql;

    TRY {

        rootHubData = GetRootHubData(UcxRootHub);

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        urb = (PURB)wdfRequestParams.Parameters.Others.Arg1;
        setupPacket = (PWDF_USB_CONTROL_SETUP_PACKET)&urb->UrbControlTransferEx.SetupPacket[0];

        if (Controller_IsControllerAccessible(rootHubData->ControllerHandle) == FALSE) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "RootHub_UcxEvtSetPortFeature: controller gone");

            urb->UrbHeader.Status = USBD_STATUS_DEVICE_GONE;
            status = STATUS_NO_SUCH_DEVICE;
            LEAVE;
        }

        //
        // Software shall ensure that the xHC is running (HCHalted == '0')
        // before attempting to write to the Port Status and Control
        // Register (PORTSC).
        //
        usbStatus.AsUlong32 = Register_ReadUlong(&rootHubData->OperationalRegisters->UsbStatus.AsUlong32);

        if (usbStatus.AsUlong32 == HARDWARE_GONE_ULONG32_REGISTER_VALUE) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "controller gone detected");

            Controller_SetControllerGone(rootHubData->ControllerHandle, TRUE);

            urb->UrbHeader.Status = USBD_STATUS_DEVICE_GONE;
            status = STATUS_NO_SUCH_DEVICE;
            LEAVE;
        }

        if (usbStatus.HcHalted != FALSE) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "HW_COMPLIANCE: Controller is halted!");
            NT_ASSERTMSG("Controller halted", 0);

            urb->UrbHeader.Status = USBD_STATUS_XACT_ERROR;
            status = STATUS_ADAPTER_HARDWARE_ERROR;
            LEAVE;
        }

        //
        // USB Specification 2.0, 11.24.2.13 Set Port Feature
        // USB Specification 3.0, 10.14.2.10 Set Port Feature
        //
        featureSelector      = setupPacket->Packet.wValue.Value;
        portNumber           = setupPacket->Packet.wIndex.Bytes.LowByte;
        featureSpecificValue = setupPacket->Packet.wIndex.Bytes.HiByte;

        if ((setupPacket->Packet.bm.Byte != 0x23) ||
            (setupPacket->Packet.bRequest != USB_REQUEST_SET_FEATURE) ||
            (portNumber == 0) ||
            (portNumber > rootHubData->NumberOfPorts) ||
            (setupPacket->Packet.wLength != 0)) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                     setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                     setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                     setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                     setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

            urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        portSCRegister   = &rootHubData->PortRegisterSet[portNumber-1].
                           PortStatusAndControl.AsUlong32;

        portPMSCRegister = &rootHubData->PortRegisterSet[portNumber-1].
                           PortPowerManagementStatusAndControl.AsUlong32;
        portHLPMCRegister = &rootHubData->PortRegisterSet[portNumber-1].
                            PortHardwareLPMControl.AsUlong32;

        if (rootHubData->PortData[portNumber-1].MajorRevision == 2) {

            LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "2.0 Port %2d, Feature Selector %2d, Feature Specific Value %3d",
                    portNumber, featureSelector, featureSpecificValue);

            //
            // The most significant byte of wIndex is zero, except when
            // the feature selector is PORT_TEST or PORT_INDICATOR.
            //
            if ((featureSelector != PORT_TEST) && (featureSelector != PORT_INDICATOR) &&
                 (featureSpecificValue != 0)) {

                LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                         "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                         setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                         setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                         setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                         setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

                urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            switch (featureSelector) {

            case PORT_RESET:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                //
                // Determine if we need to reset the hardware LPM settings since they don't
                // get reset on their own.
                //
                if (rootHubData->PortData[portNumber-1].HardwareLPMDirty) {
                    portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);

                    LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                            "PORT_RESET: Before portPMSC  0x%08x", portPMSC.AsUlong32);
                    portPMSC.Usb2.BestEfforServiceLatency = 0;
                    portPMSC.Usb2.L1DeviceSlot = 0;
                    portPMSC.Usb2.RemoteWakeEnable = 0;
                    portPMSC.Usb2.HardwareLPMEnable = 0;

                    LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                            "PORT_RESET: Writing portPMSC 0x%08x", portPMSC.AsUlong32);
                    Register_WriteUlong(portPMSCRegister, portPMSC.AsUlong32);

                    portHLPMC.AsUlong32 = Register_ReadUlong(portHLPMCRegister);

                    LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                            "PORT_RESET: Before portHLPMC  0x%08x", portHLPMC.AsUlong32);
                    portHLPMC.BestEffortServiceLatencyDeep = 0;
                    portHLPMC.HostInitiatedResumeDurationMode = 0;
                    portHLPMC.L1Timeout = 0;
                    LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                            "PORT_RESET: Writing portHLPMC 0x%08x", portHLPMC.AsUlong32);
                    Register_WriteUlong(portHLPMCRegister, portHLPMC.AsUlong32);

                    //
                    // Record that the HardwareLPM states on in their initial, disabled state
                    //
                    rootHubData->PortData[portNumber-1].HardwareLPMDirty = FALSE;

                }
                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_RESET: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortReset = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_RESET: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_RESET: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case PORT_SUSPEND:

                //
                // Some controllers interpreted the XHCI spec incorrectly and assumed
                // that the port state would always be in U0 prior to software starting
                // a U3 transition.  In order to fix this, we have to temporarily disable
                // the U1/U2 timers, bring the port back to U0, THEN set the port link state
                // to U3.
                //

                if (Controller_GetDeviceFlags(rootHubData->ControllerHandle).Disable20LPMPriorToU3) {
                    //
                    // This controller will not automatically transition from L1 to L2, so
                    // LPM needs to be disabled prior to suspending the port.
                    //
                    status = RootHub_ForceU0AndWait(rootHubData,
                                                    portNumber);
                    if (!NT_SUCCESS(status)) {
                        //
                        // The best urb status to pick in this case might be STALL
                        // to simulate that the hub did not like that command at
                        // that time
                        //
                        urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                        LEAVE;
                    }
                }
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                //
                // Reset the resume acknowledged state
                //
                rootHubData->PortData[portNumber-1].ResumeAcknowledged = FALSE;

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_SUSPEND: Before portSC  0x%08x", portSC.AsUlong32);

                //
                // 4.15.1 Port Suspend
                // Software should not attempt to suspend a port
                // unless the port reports that it is in the
                // enabled (PED = '1', PLS < '3') state.
                //
                if ((portSC.PortEnabled == 0) || (portSC.PortLinkState >= PORT_LINK_STATE_U3)) {
                    LogWarning(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                               "PORT_SUSPEND: port is not in the enabled state");
                }

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortLinkState = PORT_LINK_STATE_U3;
                portSC.PortLinkStateWriteStrobe = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_SUSPEND: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_SUSPEND: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case PORT_POWER:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_POWER: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortPower = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_POWER: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_POWER: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case PORT_TEST:
                //
                // USB Specification 2.0, Table 11-24.
                // Test Mode Selector Codes
                //
                if (featureSpecificValue > 5) {

                    LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                             "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                             setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                             setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                             setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                             setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

                    urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                    status = STATUS_UNSUCCESSFUL;
                    LEAVE;
                }

                portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_TEST: Before portPMSC  0x%08x", portPMSC.AsUlong32);

                portPMSC.Usb2.TestMode = featureSpecificValue;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_TEST: Writing portPMSC 0x%08x", portPMSC.AsUlong32);
                Register_WriteUlong(portPMSCRegister, portPMSC.AsUlong32);

                portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_TEST: After portPMSC   0x%08x", portPMSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case PORT_INDICATOR:
                //
                // USB Specification 2.0, Table 11-25.
                // Port Indicator Selector Codes
                //
                if (featureSpecificValue > 3) {

                    LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                             "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                             setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                             setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                             setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                             setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

                    urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                    status = STATUS_UNSUCCESSFUL;
                    LEAVE;
                }

                if (featureSpecificValue == 3) {

                    //
                    // Map manual Off mode to Off
                    //
                    featureSpecificValue = 0;
                }

                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_INDICATOR: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortIndicatorControl = featureSpecificValue;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_INDICATOR: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_INDICATOR: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            default:
                LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                         "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                         setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                         setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                         setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                         setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

                urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

        } else if (rootHubData->PortData[portNumber-1].MajorRevision == 3) {

            USB_HUB_30_PORT_REMOTE_WAKE_MASK usbHubPortRWMask;

            LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "3.0 Port %2d, Feature Selector %2d, Feature Specific Value %3d",
                    portNumber, featureSelector, featureSpecificValue);

            //
            // The most significant byte of wIndex is zero, except when
            // the feature selector is PORT_U1_TIMEOUT or
            // PORT_U2_TIMEOUT or PORT_LINK_STATE or
            // PORT_REMOTE_WAKE_MASK.
            //
            if ((featureSelector != PORT_U1_TIMEOUT) &&
                (featureSelector != PORT_U2_TIMEOUT) &&
                (featureSelector != PORT_LINK_STATE) &&
                (featureSelector != PORT_REMOTE_WAKE_MASK) &&
                (featureSpecificValue != 0)) {

                LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                         "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                         setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                         setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                         setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                         setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

                urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            switch (featureSelector) {

            case PORT_RESET:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_RESET: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortReset = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_RESET: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_RESET: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case BH_PORT_RESET:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "BH_PORT_RESET: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.WarmPortReset = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "BH_PORT_RESET: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "BH_PORT_RESET: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case PORT_POWER:
                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_POWER: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.PortPower = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_POWER: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_POWER: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case PORT_U1_TIMEOUT:
                //
                // USB Specification 3.0, Table 10-12.
                // U1 Timeout Value Encoding.
                //
                if ((featureSpecificValue > 0x7F) && (featureSpecificValue > 0xFF)) {

                    LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                             "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                             setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                             setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                             setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                             setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

                    urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                    status = STATUS_UNSUCCESSFUL;
                    LEAVE;
                }

                portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_U1_TIMEOUT: Before portPMSC  0x%08x", portPMSC.AsUlong32);

                portPMSC.Usb3.U1Timeout = featureSpecificValue;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_U1_TIMEOUT: Writing portPMSC 0x%08x", portPMSC.AsUlong32);
                Register_WriteUlong(portPMSCRegister, portPMSC.AsUlong32);

                portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_U1_TIMEOUT: After portPMSC   0x%08x", portPMSC.AsUlong32);

                //
                // Save the data for those controllers that need it restored on
                // resume from U3
                //
                rootHubData->PortData[portNumber-1].U1Timeout = (UCHAR)featureSpecificValue;

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case PORT_U2_TIMEOUT:
                portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_U2_TIMEOUT: Before portPMSC  0x%08x", portPMSC.AsUlong32);

                portPMSC.Usb3.U2Timeout = featureSpecificValue;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_U2_TIMEOUT: Writing portPMSC 0x%08x", portPMSC.AsUlong32);
                Register_WriteUlong(portPMSCRegister, portPMSC.AsUlong32);

                portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_U2_TIMEOUT: After portPMSC   0x%08x", portPMSC.AsUlong32);

                //
                // Save the data for those controllers that need it restored on
                // resume from U3
                //
                rootHubData->PortData[portNumber-1].U2Timeout = (UCHAR) featureSpecificValue;

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case PORT_LINK_STATE:
                if (featureSpecificValue > 5) {

                    LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                             "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                             setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                             setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                             setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                             setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

                    urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                    status = STATUS_UNSUCCESSFUL;
                    LEAVE;
                }

                //
                // If this controller requires U1/U2 timers to be disabled, then
                // see if it needs to be disabled or enabled now.
                //
                if (Controller_GetDeviceFlags(rootHubData->ControllerHandle).ForceU0PriorToU3) {

                    if (featureSpecificValue != PORT_LINK_STATE_U0) {

                        status = RootHub_ForceU0AndWait(rootHubData,
                                                        portNumber);
                        if (!NT_SUCCESS(status)) {
                            //
                            // The best urb status to pick in this case might be STALL
                            // to simulate that the hub did not like that command at
                            // that time
                            //
                            urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                            LEAVE;
                        }
                    }
                }

                //
                // On U3 transitions, we don't wait for the transition to complete before
                // completing the request back to the hub.  Therefore, on resume, we need
                // to make sure that the U3 transition has completed before starting the resume
                //
                if (featureSpecificValue == PORT_LINK_STATE_U0) {

                    status = RootHub_WaitForPendingU3TransitionCompletion(rootHubData, portNumber, FALSE);

                    if (!NT_SUCCESS(status)) {
                        //
                        // The best urb status to pick in this case might be STALL
                        // to simulate that the hub did not like that command at
                        // that time
                        //
                        urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                        LEAVE;
                    }
                }

                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                //
                // If initiating a new port suspend operation, clear the ResumeAcknowledged state
                //
                if (featureSpecificValue == PORT_LINK_STATE_U3) {
                    rootHubData->PortData[portNumber-1].ResumeAcknowledged = FALSE;
                }

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_LINK_STATE: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                //
                // To send link to SS.Disabled state, xHCI spec(Section 4.19.1.2) requires
                // the PED to be set to 1 instead
                //
                if (featureSpecificValue == PORT_LINK_STATE_DISABLED) {
                    portSC.PortEnabled = 1;
                } else {
                    portSC.PortLinkState = featureSpecificValue;
                    portSC.PortLinkStateWriteStrobe = 1;
                }
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_LINK_STATE: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_LINK_STATE: After portSC   0x%08x", portSC.AsUlong32);

                //
                // If the ControllerCanMissU3Request hack flag is enabled, this
                // means that writing the U3 may not "stick".  For such controllers, wait
                // for U3 transition completion here before completing the U3 request.
                //
                if ((featureSpecificValue == PORT_LINK_STATE_U3) &&
                    Controller_GetDeviceFlags(rootHubData->ControllerHandle).ControllerCanMissU3Request) {

                    status = RootHub_WaitForPendingU3TransitionCompletion(rootHubData, portNumber, TRUE);

                    if (!NT_SUCCESS(status)) {
                        //
                        // The best urb status to pick in this case might be STALL
                        // to simulate that the hub did not like that command at
                        // that time
                        //
                        urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                        LEAVE;
                    }
                }

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case PORT_REMOTE_WAKE_MASK:
                //
                // USB Specification 3.0, Table 10-14.
                // Downstream Port Remote Wake Mask Encoding.
                //
                usbHubPortRWMask.AsUchar8 = (UCHAR)featureSpecificValue;

                if (usbHubPortRWMask.Reserved0 != 0) {

                    LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                             "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                             setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                             setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                             setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                             setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

                    urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                    status = STATUS_UNSUCCESSFUL;
                    LEAVE;
                }

                KeAcquireSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_REMOTE_WAKE_MASK: Before portSC  0x%08x", portSC.AsUlong32);

                portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                portSC.WakeOnConnectEnable     = usbHubPortRWMask.ConnectRemoteWakeEnable;
                portSC.WakeOnDisconnectEnable  = usbHubPortRWMask.DisconnectRemoteWakeEnable;
                portSC.WakeOnOverCurrentEnable = usbHubPortRWMask.OverCurrentRemoteWakeEnable;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_REMOTE_WAKE_MASK: Writing portSC 0x%08x", portSC.AsUlong32);
                Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                KeReleaseSpinLock(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock, irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "PORT_REMOTE_WAKE_MASK: After portSC   0x%08x", portSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            case FORCE_LINKPM_ACCEPT:
                //
                // See also: Q1'09 USB 3.0 Errata (Released 05/15/2009)
                // FORCE_LINKPM_ACCEPT Feature left out
                //
                portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "FORCE_LINKPM_ACCEPT: Before portPMSC  0x%08x", portPMSC.AsUlong32);

                portPMSC.Usb3.ForceLinkPMAccept = 1;
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "FORCE_LINKPM_ACCEPT: Writing portPMSC 0x%08x", portPMSC.AsUlong32);
                Register_WriteUlong(portPMSCRegister, portPMSC.AsUlong32);

                portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);
                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "FORCE_LINKPM_ACCEPT: After portPMSC   0x%08x", portPMSC.AsUlong32);

                urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
                status = STATUS_SUCCESS;
                break;

            default:
                LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                         "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                         setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                         setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                         setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                         setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

                urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

        } else {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                     setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                     setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                     setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                     setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

            urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

    } FINALLY {

        WdfRequestComplete(WdfRequest, status);
    }

    return;
}

VOID
RootHub_UcxEvtGetPortErrorCount(
    UCXROOTHUB  UcxRootHub,
    WDFREQUEST  WdfRequest
    )
/*++

Routine Description:


Arguments:

    UcxRootHub - The UCX root hub object.

    WdfRequest - The WDF request object for the Get Port Error Count request.

Return Value:

    None directly from this routine.

    The request completion status in the WdfRequest and the USBD_STATUS
    in the urb header.

--*/
{
    NTSTATUS                        status;
    PROOTHUB_DATA                   rootHubData;
    WDF_REQUEST_PARAMETERS          wdfRequestParams;
    PURB                            urb;
    PWDF_USB_CONTROL_SETUP_PACKET   setupPacket;
    ULONG                           portNumber;

    TRY {

        rootHubData = GetRootHubData(UcxRootHub);

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        urb = (PURB)wdfRequestParams.Parameters.Others.Arg1;
        setupPacket = (PWDF_USB_CONTROL_SETUP_PACKET)&urb->UrbControlTransferEx.SetupPacket[0];

        if (Controller_IsControllerAccessible(rootHubData->ControllerHandle) == FALSE) {

            LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "controller gone");

            urb->UrbHeader.Status = USBD_STATUS_DEVICE_GONE;
            status = STATUS_NO_SUCH_DEVICE;
            LEAVE;
        }

        //
        // USB Specification 3.0, 10.14.2.5 Get Port Error Count
        // See also: Q1'09 USB 3.0 Errata (Released 05/15/2009)
        // Correction: bmRequestype must be 10100011B
        //
        portNumber = setupPacket->Packet.wIndex.Value;

        if ((setupPacket->Packet.bm.Byte != 0xA3) ||
            (setupPacket->Packet.bRequest != USB_REQUEST_GET_PORT_ERR_COUNT) ||
            (setupPacket->Packet.wValue.Value != 0) ||
            (portNumber == 0) ||
            (portNumber > rootHubData->NumberOfPorts) ||
            (setupPacket->Packet.wLength != 2)) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                     setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                     setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                     setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                     setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

            urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        if (rootHubData->PortData[portNumber-1].MajorRevision == 3) {

            PULONG      portLinkInfoRegister;
            PORTLI      portLinkInfo;
            PUSHORT     numberOfLinkErrors;

            portLinkInfoRegister = &rootHubData->PortRegisterSet[portNumber-1].
                                   PortLinkInfo.AsUlong32;

            portLinkInfo.AsUlong32 = Register_ReadUlong(portLinkInfoRegister);

            numberOfLinkErrors = (PUSHORT)urb->UrbControlTransferEx.TransferBuffer;
            *numberOfLinkErrors = (USHORT)portLinkInfo.LinkErrorCount;

            LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "3.0 Port %2d, Number Of Link Errors: %u",
                    portNumber, portLinkInfo.LinkErrorCount);

            if(portLinkInfo.LinkErrorCount > HW_COMPLIANCE_LINK_ERROR_COUNT_WARN_LIMIT) {
                LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "HW_COMPLIANCE: 3.0 Port %2d is reporting %u Link Errors!",
                        portNumber, portLinkInfo.LinkErrorCount);
            }

            urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
            status = STATUS_SUCCESS;

        } else {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "invalid request [%02X %02X %02X %02X %02X %02X %02X %02X]",
                     setupPacket->Generic.Bytes[0], setupPacket->Generic.Bytes[1],
                     setupPacket->Generic.Bytes[2], setupPacket->Generic.Bytes[3],
                     setupPacket->Generic.Bytes[4], setupPacket->Generic.Bytes[5],
                     setupPacket->Generic.Bytes[6], setupPacket->Generic.Bytes[7]);

            urb->UrbHeader.Status = USBD_STATUS_STALL_PID;
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

    } FINALLY {

        NT_ASSERT(status == STATUS_SUCCESS);
        WdfRequestComplete(WdfRequest, status);
    }

    return;
}

VOID
RootHub_UcxEvtInterruptTransfer(
    __in
        UCXROOTHUB  UcxRootHub,
    __in
        WDFREQUEST  WdfRequest
    )
/*++

Routine Description:

    UcxRootHub - The UCX root hub object.

    WdfRequest - The WDF request object for the root hub interrupt transfer request.

Return Value:

    None directly from this routine.

    The request completion status in the WdfRequest.

--*/
{
    NTSTATUS                status;
    PROOTHUB_DATA           rootHubData;
    WDF_REQUEST_PARAMETERS  wdfRequestParams;
    PURB                    urb;
    ULONG                   portNumber, maxPortNumber;
    PULONG                  portSCRegister;
    PORTSC                  portSC;
    ULONG                   transferBufferLength;
    PVOID                   transferBuffer;

    TRY {

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        urb = (PURB)wdfRequestParams.Parameters.Others.Arg1;
        transferBuffer = urb->UrbBulkOrInterruptTransfer.TransferBuffer;
        transferBufferLength = urb->UrbBulkOrInterruptTransfer.TransferBufferLength;

        rootHubData = GetRootHubData(UcxRootHub);

        if (Controller_IsControllerAccessible(rootHubData->ControllerHandle) == FALSE) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "RootHub_UcxEvtInterruptTransfer: controller gone");

            urb->UrbHeader.Status = USBD_STATUS_DEVICE_GONE;
            status = STATUS_NO_SUCH_DEVICE;
            LEAVE;
        }

        //
        // In absense of a UCX callback, detect D0Entry of RootHub PDO by identifying
        // the first interrupt transfer from UCX after the Controller FDO enters D0.
        //
        if (rootHubData->NoInterruptTransfersSinceFDOD0Entry) {

            Controller_UpdateIdleTimeoutOnRootHubPDOD0Entry(rootHubData->ControllerHandle);

            rootHubData->NoInterruptTransfersSinceFDOD0Entry = FALSE;
        }

        if (transferBufferLength > 0) {

            RtlZeroMemory(transferBuffer, transferBufferLength);

            if (transferBufferLength > 32) {
                maxPortNumber = 255;
            } else {
                maxPortNumber = (transferBufferLength * 8) - 1;
            }

            if (maxPortNumber > rootHubData->NumberOfPorts) {
                maxPortNumber = rootHubData->NumberOfPorts;
            }

            for (portNumber = 1; portNumber <= maxPortNumber; portNumber++) {

                if (rootHubData->PortData[portNumber-1].MajorRevision == 2) {

                    portSCRegister = &rootHubData->PortRegisterSet[portNumber-1].PortStatusAndControl.AsUlong32;
                    portSC.AsUlong32 = Register_ReadUlong(portSCRegister);

                    if (portSC.AsUlong32 == HARDWARE_GONE_ULONG32_REGISTER_VALUE) {

                        LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                                "RootHub_UcxEvtInterruptTransfer: controller gone detected");

                        Controller_SetControllerGone(rootHubData->ControllerHandle, TRUE);

                        urb->UrbHeader.Status = USBD_STATUS_DEVICE_GONE;
                        status = STATUS_NO_SUCH_DEVICE;
                        LEAVE;
                    }

                    //
                    // Mask the change that happens due to the link entering the
                    // resumed state as we will handle that on our own when the
                    // resume timer fires. Note that we have a similar logic in
                    // get port status.
                    //
                    if (portSC.PortLinkState == PORT_LINK_STATE_RESUME) {
                        portSC.PortLinkStateChange = 0;
                    }

                    if (portSC.ConnectStatusChange ||
                        portSC.PortEnabledOrDisabledChange ||
                        portSC.PortLinkStateChange ||
                        portSC.OverCurrentChange ||
                        portSC.PortResetChange) {

                        ((PUCHAR)transferBuffer)[portNumber / 8] |= (1 << (portNumber % 8));

                        LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                                "2.0 Port %2d change, portSC 0x%08x", portNumber, portSC.AsUlong32);
                    }

                } else if (rootHubData->PortData[portNumber-1].MajorRevision == 3) {

                    portSCRegister = &rootHubData->PortRegisterSet[portNumber-1].PortStatusAndControl.AsUlong32;
                    portSC.AsUlong32 = Register_ReadUlong(portSCRegister);

                    if (portSC.AsUlong32 == HARDWARE_GONE_ULONG32_REGISTER_VALUE) {

                        LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                                "controller gone detected");

                        Controller_SetControllerGone(rootHubData->ControllerHandle, TRUE);

                        urb->UrbHeader.Status = USBD_STATUS_DEVICE_GONE;
                        status = STATUS_NO_SUCH_DEVICE;
                        LEAVE;
                    }

                    if (portSC.ConnectStatusChange ||
                        portSC.OverCurrentChange ||
                        portSC.PortResetChange ||
                        portSC.WarmPortResetChange ||
                        portSC.PortLinkStateChange ||
                        portSC.PortConfigErrorChange) {

                        LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                                "3.0 Port %2d change, portSC 0x%08x", portNumber, portSC.AsUlong32);

                        if (RootHub_HideInvalidDebugPortStatusAndChange(rootHubData, portNumber)) {

                            LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                                    "For Debug Port %2d, hiding Port Status Change from hub driver", portNumber);
                            continue;
                        }

                        ((PUCHAR)transferBuffer)[portNumber / 8] |= (1 << (portNumber % 8));
                    }
                }
            }
        }

        urb->UrbHeader.Status = USBD_STATUS_SUCCESS;
        status = STATUS_SUCCESS;

    } FINALLY {

        WdfRequestComplete(WdfRequest, status);
    }
}

VOID
RootHub_UcxEvtGetInfo(
    UCXROOTHUB  UcxRootHub,
    WDFREQUEST  WdfRequest
    )
/*++

Routine Description:

    This routine is called by UCX to process an
    IOCTL_INTERNAL_USB_GET_ROOTHUB_INFO request and return information
    about the root hub in the ROOTHUB_INFO structure attached to the
    request.

    The main information of interest returned in the ROOTHUB_INFO
    structure is the number of 2.0 ports and the number of 3.0 ports
    supported by this root hub.  That information was collected by
    RootHub_PrepareHardware() when it processed the xHCI Supported
    Protocol Capability list.

Return Value:

    None directly from this routine.

    The request completion status in the WdfRequest.

--*/
{
    PROOTHUB_DATA           rootHubData;
    WDF_REQUEST_PARAMETERS  wdfRequestParams;
    PROOTHUB_INFO           roothubInfo;
    NTSTATUS                status;

    TRY {

        rootHubData = GetRootHubData(UcxRootHub);

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        roothubInfo = (PROOTHUB_INFO)wdfRequestParams.Parameters.Others.Arg1;

        if (roothubInfo->Size < sizeof(ROOTHUB_INFO)) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "HW_COMPLIANCE: Invalid ROOTHUB_INFO Size %d",
                     roothubInfo->Size);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        roothubInfo->ControllerType = ControllerTypeXhci;
        roothubInfo->NumberOf20Ports = rootHubData->NumberOf20Ports;
        roothubInfo->NumberOf30Ports = rootHubData->NumberOf30Ports;
        //
        // Units here are in microseconds.
        //
        roothubInfo->MaxU1ExitLatency = rootHubData->U1DeviceExitLatency;
        roothubInfo->MaxU2ExitLatency = rootHubData->U2DeviceExitLatency;


        LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                "RootHub_UcxEvtGetInfo NumberOf20Ports %d NumberOf30Ports %d",
                roothubInfo->NumberOf20Ports, roothubInfo->NumberOf30Ports);

        status = STATUS_SUCCESS;

    } FINALLY {

        WdfRequestComplete(WdfRequest, status);
    }

    return;
}

FORCEINLINE
ULONG
RootHub_GetLinkProtocolForSpeedID(
    ULONG SpeedID,
    UCHAR MinorVersion
    )
{
    ULONG linkProtocol;

    if ((MinorVersion >= 1) &&
        (SpeedID > PORT_SPEED_SUPER_SPEED_DEVICE_ATTACHED)) {
        linkProtocol = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_PROTOCOL_SSP;
    } else {
        linkProtocol = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_PROTOCOL_SS;
    }

    return linkProtocol;
}

NTSTATUS
RootHub_Read30PortSpeeds(
    PROOTHUB_DATA                                RootHubData,
    PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED  Speeds,
    PUSHORT                                      SpeedsCount,
    USHORT                                       MaxSpeedsCount,
    PSUPPPORTED_PROTOCOL_CAPABILITY_SPEED        SrcSpeeds,
    USHORT                                       SrcSpeedsCount,
    UCHAR                                        MinorVersion
    )
/*++

Routine Description:

    This routine reads speed IDs and corresponding information from a controller,
    verifies data, and converts it to PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED format.

    The routine will at most fill MaxSpeedsCount entries in the Speed buffer.
    Total number of entries available returned in SpeedCount. The number may exceed
    MaxSpeedsCount if the Speed buffer is not large enough.

Return Value:

    Status of the operation.

--*/
{
    SUPPPORTED_PROTOCOL_CAPABILITY_SPEED speed;
    SUPPPORTED_PROTOCOL_CAPABILITY_SPEED speedTX;
    USHORT                               i;
    USHORT                               speedsCount;
    ULONG                                speedsBitmap;
    NTSTATUS                             status;

    TRY {

        *SpeedsCount = 0;

        //
        // 32-bit bitmap is sufficient to accommodate all 16 unique SpeedIDs
        //
        speedsBitmap = 0;
        speedsCount = 0;

        //
        // Read & convert all speeds, check default speedIDs
        //
        for (i = 0; i < SrcSpeedsCount; i++) {
            speed.AsUlong32 = Register_ReadUlong(&SrcSpeeds->AsUlong32 + i);

            //
            // Make sure the SpeedID is not duplicated
            //
            if (speedsBitmap & (1U << speed.SpeedID)) {
                LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                         "HW_COMPLIANCE: Speed Table duplicates SpeedID=0x%x",
                    speed.SpeedID);
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }
            speedsBitmap |= (1U << speed.SpeedID);

            //
            // Convert the speed to the USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED format
            //
            if (speedsCount < MaxSpeedsCount) {
                Speeds[speedsCount].AsUlong32 = 0;
                Speeds[speedsCount].LinkProtocol = RootHub_GetLinkProtocolForSpeedID(speed.SpeedID, MinorVersion);
                Speeds[speedsCount].LaneSpeedExponent = speed.SpeedExponent;
                Speeds[speedsCount].LaneSpeedMantissa = speed.SpeedMantissa;
                Speeds[speedsCount].SublinkSpeedAttrID = speed.SpeedID;
            }

            if (speed.PSIType == SUPPPORTED_PROTOCOL_CAPABILITY_SPEED_TYPE_ASYMMETRIC_RX) {
                if (speedsCount < MaxSpeedsCount) {
                    Speeds[speedsCount].SublinkTypeMode = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_MODE_ASYMMETRIC;
                    Speeds[speedsCount].SublinkTypeDir = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_DIR_RX;
                }
                speedsCount++;
                if (++i >= SrcSpeedsCount) {
                    LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                             "HW_COMPLIANCE: Speed Table terminates without TX entry for SpeedID=0x%x",
                        speed.SpeedID);
                    status = STATUS_INVALID_PARAMETER;
                    LEAVE;
                }
                speedTX.AsUlong32 = Register_ReadUlong(&SrcSpeeds->AsUlong32 + i);
                if ((speedTX.SpeedID != speed.SpeedID) ||
                    (speedTX.PSIType != SUPPPORTED_PROTOCOL_CAPABILITY_SPEED_TYPE_ASYMMETRIC_TX)) {

                    LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                             "HW_COMPLIANCE: Speed Table's RX entry (SpeedID=0x%x,PSIType=0x%x) has incompatible pair (SpeedID=0x%x,PSIType=0x%x)",
                        speed.SpeedID, speed.PSIType, speedTX.SpeedID, speedTX.PSIType);
                    status = STATUS_INVALID_PARAMETER;
                    LEAVE;
                }
                if (speedsCount < MaxSpeedsCount) {
                    Speeds[speedsCount].AsUlong32 = 0;
                    Speeds[speedsCount].LinkProtocol = RootHub_GetLinkProtocolForSpeedID(speed.SpeedID, MinorVersion);
                    Speeds[speedsCount].LaneSpeedExponent = speedTX.SpeedExponent;
                    Speeds[speedsCount].LaneSpeedMantissa = speedTX.SpeedMantissa;
                    Speeds[speedsCount].SublinkSpeedAttrID = speedTX.SpeedID;
                    Speeds[speedsCount].SublinkTypeMode = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_MODE_ASYMMETRIC;
                    Speeds[speedsCount].SublinkTypeDir = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_DIR_TX;
                }
                speedsCount++;
            } else if (speed.PSIType == SUPPPORTED_PROTOCOL_CAPABILITY_SPEED_TYPE_SYMMETRIC) {
                if (speedsCount < MaxSpeedsCount) {
                    Speeds[speedsCount].SublinkTypeMode = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_MODE_SYMMETRIC;
                    Speeds[speedsCount].SublinkTypeDir = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_DIR_RX;
                }
                speedsCount++;
                if (speedsCount < MaxSpeedsCount) {
                    Speeds[speedsCount].AsUlong32 = 0;
                    Speeds[speedsCount].LinkProtocol = RootHub_GetLinkProtocolForSpeedID(speed.SpeedID, MinorVersion);
                    Speeds[speedsCount].LaneSpeedExponent = speed.SpeedExponent;
                    Speeds[speedsCount].LaneSpeedMantissa = speed.SpeedMantissa;
                    Speeds[speedsCount].SublinkSpeedAttrID = speed.SpeedID;
                    Speeds[speedsCount].SublinkTypeMode = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_MODE_SYMMETRIC;
                    Speeds[speedsCount].SublinkTypeDir = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_DIR_TX;
                }
                speedsCount++;
            } else if (speed.PSIType == SUPPPORTED_PROTOCOL_CAPABILITY_SPEED_TYPE_ASYMMETRIC_TX) {
                LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                         "HW_COMPLIANCE: Speed Table's TX entry preceeds RX entry for SpeedID=0x%x",
                    speed.SpeedID);
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }
        }

        //
        // Super Speed default speed
        //
        if ((speedsBitmap & PORT_SPEED_SUPER_SPEED_DEVICE_ATTACHED) == 0) {
            if (speedsCount < MaxSpeedsCount) {
                Speeds[speedsCount].AsUlong32 = 0;
                Speeds[speedsCount].LinkProtocol = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_PROTOCOL_SS;
                Speeds[speedsCount].SublinkSpeedAttrID = PORT_SPEED_SUPER_SPEED_DEVICE_ATTACHED;
                Speeds[speedsCount].SublinkTypeMode = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_MODE_SYMMETRIC;
                Speeds[speedsCount].SublinkTypeDir = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_DIR_RX;
                Speeds[speedsCount].LaneSpeedExponent = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_LSE_GBPS;
                Speeds[speedsCount].LaneSpeedMantissa = 5;
            }
            speedsCount++;
            if (speedsCount < MaxSpeedsCount) {
                Speeds[speedsCount].AsUlong32 = 0;
                Speeds[speedsCount].LinkProtocol = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_PROTOCOL_SS;
                Speeds[speedsCount].SublinkSpeedAttrID = PORT_SPEED_SUPER_SPEED_DEVICE_ATTACHED;
                Speeds[speedsCount].SublinkTypeMode = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_MODE_SYMMETRIC;
                Speeds[speedsCount].SublinkTypeDir = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_DIR_TX;
                Speeds[speedsCount].LaneSpeedExponent = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_LSE_GBPS;
                Speeds[speedsCount].LaneSpeedMantissa = 5;
            }
            speedsCount++;
        }

        //
        // Super Speed Plus default speed
        //
        if ((speedsBitmap & PORT_SPEED_SUPER_SPEED_PLUS_DEVICE_ATTACHED) == 0) {
            if (speedsCount < MaxSpeedsCount) {
                Speeds[speedsCount].AsUlong32 = 0;
                Speeds[speedsCount].LinkProtocol = RootHub_GetLinkProtocolForSpeedID(PORT_SPEED_SUPER_SPEED_PLUS_DEVICE_ATTACHED, MinorVersion);
                Speeds[speedsCount].SublinkSpeedAttrID = PORT_SPEED_SUPER_SPEED_PLUS_DEVICE_ATTACHED;
                Speeds[speedsCount].SublinkTypeMode = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_MODE_SYMMETRIC;
                Speeds[speedsCount].SublinkTypeDir = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_DIR_RX;
                Speeds[speedsCount].LaneSpeedExponent = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_LSE_GBPS;
                Speeds[speedsCount].LaneSpeedMantissa = 10;
            }
            speedsCount++;
            if (speedsCount < MaxSpeedsCount) {
                Speeds[speedsCount].AsUlong32 = 0;
                Speeds[speedsCount].LinkProtocol = RootHub_GetLinkProtocolForSpeedID(PORT_SPEED_SUPER_SPEED_PLUS_DEVICE_ATTACHED, MinorVersion);
                Speeds[speedsCount].SublinkSpeedAttrID = PORT_SPEED_SUPER_SPEED_PLUS_DEVICE_ATTACHED;
                Speeds[speedsCount].SublinkTypeMode = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_MODE_SYMMETRIC;
                Speeds[speedsCount].SublinkTypeDir = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_DIR_TX;
                Speeds[speedsCount].LaneSpeedExponent = USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_LSE_GBPS;
                Speeds[speedsCount].LaneSpeedMantissa = 10;
            }
            speedsCount++;
        }

        status = STATUS_SUCCESS;
        *SpeedsCount = speedsCount;

    } FINALLY {

    }

    return status;
}

VOID
RootHub_UcxEvtGet20PortInfo(
    UCXROOTHUB  UcxRootHub,
    WDFREQUEST  WdfRequest
    )
/*++

Routine Description:

    This routine is called by UCX to process an
    IOCTL_INTERNAL_USB_ROOTHUB_GET_20PORT_INFO request and return
    information about the root hub in the ROOTHUB_20PORTS_INFO structure
    attached to the request.

    The main information of interest returned in the
    ROOTHUB_20PORTS_INFO structure is the list of 2.0 ports supported by
    this root hub.  That is returned in the PortInfoArray array of
    ROOTHUB_20PORT_INFO structures.  The list of 2.0 ports supported by
    this root hub was collected by RootHub_PrepareHardware() when it
    processed the xHCI Supported Protocol Capability list.

Return Value:

    None directly from this routine.

    The request completion status in the WdfRequest.

--*/
{
    PROOTHUB_DATA           rootHubData;
    WDF_REQUEST_PARAMETERS  wdfRequestParams;
    PROOTHUB_20PORTS_INFO   roothub20PortsInfo;
    PROOTHUB_20PORT_INFO*   roothub20PortInfo;
    USHORT                  srcPortIndex;
    USHORT                  dstPortIndex;
    NTSTATUS                status;

    TRY {

        rootHubData = GetRootHubData(UcxRootHub);

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        roothub20PortsInfo = (PROOTHUB_20PORTS_INFO)wdfRequestParams.Parameters.Others.Arg1;
        roothub20PortInfo  = &roothub20PortsInfo->PortInfoArray[0];

        if (roothub20PortsInfo->Size < sizeof(ROOTHUB_20PORTS_INFO)) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "HW_COMPLIANCE: Invalid ROOTHUB_20PORTS_INFO Size %d",
                     roothub20PortsInfo->Size);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        if (roothub20PortsInfo->NumberOfPorts != rootHubData->NumberOf20Ports) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "HW_COMPLIANCE: Invalid ROOTHUB_20PORTS_INFO NumberOfPorts %d",
                     roothub20PortsInfo->NumberOfPorts);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        if (roothub20PortsInfo->PortInfoSize < sizeof(ROOTHUB_20PORT_INFO)) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "HW_COMPLIANCE: Invalid ROOTHUB_20PORT_INFO PortInfoSize %d",
                     roothub20PortsInfo->PortInfoSize);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        //
        // Return port information for all of the MajorRevision == 2
        // ports in the PortData[] array.
        //
        dstPortIndex = 0;

        for (srcPortIndex = 0;
             (srcPortIndex < rootHubData->NumberOfPorts && dstPortIndex < roothub20PortsInfo->NumberOfPorts);
             srcPortIndex++) {

            if (rootHubData->PortData[srcPortIndex].MajorRevision == 2) {

                //
                // The PortData[] array is 0-based indexed while
                // port numbers are 1-based indexed.
                //
                roothub20PortInfo[dstPortIndex]->PortNumber = srcPortIndex + 1;

                if (rootHubData->PortData[srcPortIndex].DeviceRemovable == TRUE) {
                    roothub20PortInfo[dstPortIndex]->Removable = TriStateTrue;
                } else {
                    roothub20PortInfo[dstPortIndex]->Removable = TriStateFalse;
                }

                if (rootHubData->PortData[srcPortIndex].IntegratedHubImplemented == TRUE) {
                    roothub20PortInfo[dstPortIndex]->IntegratedHubImplemented = TriStateTrue;
                } else {
                    roothub20PortInfo[dstPortIndex]->IntegratedHubImplemented = TriStateFalse;
                }

                if (Controller_GetDeviceFlags(rootHubData->ControllerHandle).Disable20LPM == 0) {
                    roothub20PortInfo[dstPortIndex]->ControllerUsb20HardwareLpmFlags = rootHubData->PortData[srcPortIndex].HardwareLPMFlags;
                }

                roothub20PortInfo[dstPortIndex]->MinorRevision = rootHubData->PortData[srcPortIndex].MinorRevision;
                roothub20PortInfo[dstPortIndex]->HubDepth = rootHubData->PortData[srcPortIndex].HubDepth;

                dstPortIndex++;
            }
        }

        status = STATUS_SUCCESS;

    } FINALLY {

        WdfRequestComplete(WdfRequest, status);
    }

    return;
}

VOID
RootHub_UcxEvtGet30PortInfo(
    UCXROOTHUB  UcxRootHub,
    WDFREQUEST  WdfRequest
    )
/*++

Routine Description:

    This routine is called by UCX to process an
    IOCTL_INTERNAL_USB_ROOTHUB_GET_30PORT_INFO request and return
    information about the root hub in the ROOTHUB_30PORTS_INFO structure
    attached to the request.

    The main information of interest returned in the
    ROOTHUB_30PORTS_INFO structure is the list of 3.0 ports supported by
    this root hub.  That is returned in the PortInfoArray array of
    ROOTHUB_30PORT_INFO structures.  The list of 3.0 ports supported by
    this root hub was collected by RootHub_PrepareHardware() when it
    processed the xHCI Supported Protocol Capability list.

Return Value:

    None directly from this routine.

    The request completion status in the WdfRequest.

--*/
{
    PROOTHUB_DATA            rootHubData;
    WDF_REQUEST_PARAMETERS   wdfRequestParams;
    PROOTHUB_30PORTS_INFO    roothub30PortsInfo;
    PROOTHUB_30PORT_INFO_EX* roothub30PortInfo;
    USHORT                   srcPortIndex;
    USHORT                   dstPortIndex;
    NTSTATUS                 status;

    TRY {

        rootHubData = GetRootHubData(UcxRootHub);

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        roothub30PortsInfo = (PROOTHUB_30PORTS_INFO)wdfRequestParams.Parameters.Others.Arg1;
        roothub30PortInfo = (PROOTHUB_30PORT_INFO_EX*)&roothub30PortsInfo->PortInfoArray[0];

        if (roothub30PortsInfo->Size < sizeof(ROOTHUB_30PORTS_INFO)) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "HW_COMPLIANCE: Invalid ROOTHUB_30PORTS_INFO Size %d",
                     roothub30PortsInfo->Size);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        if (roothub30PortsInfo->NumberOfPorts != rootHubData->NumberOf30Ports) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "HW_COMPLIANCE: Invalid ROOTHUB_30PORTS_INFO NumberOfPorts %d",
                     roothub30PortsInfo->NumberOfPorts);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        if (roothub30PortsInfo->PortInfoSize < sizeof(ROOTHUB_30PORT_INFO)) {

            LogError(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                     "HW_COMPLIANCE: Invalid ROOTHUB_30PORT_INFO Size %d",
                     roothub30PortsInfo->PortInfoSize);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        //
        // Return port information for all of the MajorRevision == 3
        // ports in the PortData[] array.
        //
        dstPortIndex = 0;

        for (srcPortIndex = 0;
             (srcPortIndex < rootHubData->NumberOfPorts && dstPortIndex < roothub30PortsInfo->NumberOfPorts);
             srcPortIndex++) {

            if (rootHubData->PortData[srcPortIndex].MajorRevision == 3) {

                //
                // The PortData[] array is 0-based indexed while
                // port numbers are 1-based indexed.
                //
                roothub30PortInfo[dstPortIndex]->PortNumber = srcPortIndex + 1;

                if (rootHubData->PortData[srcPortIndex].DeviceRemovable == TRUE) {
                    roothub30PortInfo[dstPortIndex]->Removable = TriStateTrue;
                } else {
                    roothub30PortInfo[dstPortIndex]->Removable = TriStateFalse;
                }

                if (rootHubData->DebugCapable == TRUE) {
                    roothub30PortInfo[dstPortIndex]->DebugCapable = TriStateTrue;
                } else {
                    roothub30PortInfo[dstPortIndex]->DebugCapable = TriStateFalse;
                }

                roothub30PortInfo[dstPortIndex]->MinorRevision = rootHubData->PortData[srcPortIndex].MinorRevision;
                roothub30PortInfo[dstPortIndex]->HubDepth = rootHubData->PortData[srcPortIndex].HubDepth;

                if (roothub30PortsInfo->PortInfoSize >= sizeof(ROOTHUB_30PORT_INFO_EX)) {

                    status =
                        RootHub_Read30PortSpeeds(rootHubData, roothub30PortInfo[dstPortIndex]->Speeds,
                                                 &roothub30PortInfo[dstPortIndex]->SpeedsCount, roothub30PortInfo[dstPortIndex]->MaxSpeedsCount,
                                                 rootHubData->PortData[srcPortIndex].Speeds, rootHubData->PortData[srcPortIndex].SpeedsCount,
                                                 rootHubData->PortData[srcPortIndex].MinorRevision);

                    if (status != STATUS_SUCCESS) {
                        LEAVE;
                    }
                }

                dstPortIndex++;
            }
        }

        status = STATUS_SUCCESS;

    } FINALLY {

        WdfRequestComplete(WdfRequest, status);
    }

    return;
}


VOID
RootHub_SetPortResumeTime(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber,
    __in
        USHORT          ResumeTime
    )
/*++

Routine Description:

Return Value:


--*/
{
    PROOTHUB_PORT_DATA  portData;
    KIRQL               irql;

    TRY {

        portData = &RootHubData->PortData[PortNumber - 1];

        KeAcquireSpinLock(&portData->ReadModifyWriteLock, &irql);

        //
        // Multiple devices attached to a non-root hub could specify mimimum
        // resume times.  Keep a reference count of how may times the minuimum resume
        // time has been set for the port.  Note that the hub driver should
        // always be setting the resume time to 20ms for non-root attached
        // devices.
        //
        portData->ResumeTime = ResumeTime;
        portData->ResumeTimeReferenceCount++;

        KeReleaseSpinLock(&portData->ReadModifyWriteLock, irql);

    } FINALLY {
    }
}


VOID
RootHub_ClearPortResumeTime(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber
    )
/*++

Routine Description:

Return Value:

--*/
{
    PROOTHUB_PORT_DATA          portData;
    KIRQL                       irql;

    TRY {

        portData = NULL;

        portData = &RootHubData->PortData[PortNumber - 1];


        KeAcquireSpinLock(&portData->ReadModifyWriteLock, &irql);
        NT_ASSERT(portData->ResumeTimeReferenceCount > 0);

        portData->ResumeTimeReferenceCount--;

        KeReleaseSpinLock(&portData->ReadModifyWriteLock, irql);

    } FINALLY {

    }

}


BOOLEAN
RootHub_DetectAndAcknowledgePortResume(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber,
    __in
        BOOLEAN         OnPowerUp
    )
/*++

    This routine checks the port state to see if the controller is reporting
    that a device initiated remote wake, and if so, acknowledges the change to
    allow the transition to U0, unless that has already be done since the last
    suspend.

--*/
{
    PULONG              portSCRegister;
    PORTSC              portSC;
    KIRQL               irql;
    PROOTHUB_PORT_DATA  portData;
    BOOLEAN             hideChange;
    BOOLEAN             releaseLock;
    USHORT              resumeTime;

    TRY {

        hideChange = FALSE;
        releaseLock = FALSE;
        portData = &RootHubData->PortData[PortNumber-1];
        portSCRegister = &RootHubData->PortRegisterSet[PortNumber-1].PortStatusAndControl.AsUlong32;


        KeAcquireSpinLock(&portData->ReadModifyWriteLock,
                          &irql);
        releaseLock = TRUE;
        portSC.AsUlong32 = Register_ReadUlong(portSCRegister);


        //
        // First, take this opportunity to check if the U1/U2 timeouts need to be
        // restored
        //
        if ((portData->MajorRevision == 3) &&
            (portSC.PortLinkStateChange == 1) &&
            (portSC.PortLinkState == PORT_LINK_STATE_U0) &&
            Controller_GetDeviceFlags(RootHubData->ControllerHandle).ForceU0PriorToU3) {

            RootHub_RestoreU1U2Timeouts(RootHubData, PortNumber);
        }

        //
        // 4.15.2 Port Resume
        // 4.15.2.1 Device Initiated
        // 3) PLC = '1' and PLS = Resume if the event was due to a
        //    device initiated resume.
        //
        if (portSC.PortLinkState != PORT_LINK_STATE_RESUME) {
            LEAVE;
        }

        //
        // On power up, it may be possible that the PLC bit may not be set
        // due to the resume, so on power up, we will acknowledge any port in the
        // resume state
        //
        if ((OnPowerUp == FALSE) &&
            (portSC.PortLinkStateChange == 0)) {
            LEAVE;
        }

        //
        // The hub driver doesn't need to be notified about the start of resume, only the completion
        //
        hideChange = TRUE;

        //
        // It is possible that on D0Entry, we already acknowledged the event, but the controller
        // may have also generated an event in the event ring or visa versa.  The resume should
        // only be acknowledged once.
        //
        if (portData->ResumeAcknowledged == TRUE) {
            LEAVE;
        }

        portData->ResumeAcknowledged = TRUE;

        if (portData->MajorRevision == 2) {

            KeReleaseSpinLock(&portData->ReadModifyWriteLock,
                              irql);
            releaseLock = FALSE;

            //
            // For a 2.0 port, software is required to ensure resume signalling lasts at least 20ms
            // We will queue the timer, but not acknowledge the port status change yet.  This will delay
            // detection of disconnect or error events, but if we power down the controller before the 20ms
            // timer expires, the change bit will still be pending, which should ensure that the controller
            // generates wake signalling.
            //

            resumeTime = (portData->ResumeTimeReferenceCount > 0) ?
                         (portData->ResumeTime) :
                         (TDRSMDN_IN_MS);

            LogWarning(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "Detected Device-Initiated Resume on 2.0 Root Port %d (PortSC 0x%08x). Starting resume timer (resumeTime = %d ms)",
                PortNumber, portSC.AsUlong32, resumeTime);

            //
            // Software shall ensure that resume is signaled for at
            // least 20 ms (TDRSMDN).  Refer to section 7.1.7.7 of
            // the USB2 spec.  Software shall start timing TDRSMDN
            // from the notification of the transition to the Resume
            // state.  After TDRSMDN is complete, software shall
            // write a '0' to the PLS field.
            //
            ExSetTimer(portData->ExTimer20PortResumeComplete,
                       WDF_REL_TIMEOUT_IN_MS(resumeTime),
                       0,
                       NULL);

        } else if (portData->MajorRevision == 3) {

            LogWarning(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "Detected Device-Initiated Resume on 3.0 Root Port %d (PortSC 0x%08x).  Acknowledging the Resume.",
                PortNumber, portSC.AsUlong32);
            //
            // For a USB3 protocol port, software shall write a '0'
            // to the PLS field to direct the xHC to initiate LFPS
            // to the device and initiate the LFPS Handshake.
            //
            // Note, we are also preserving the PortLinkStateChange bit (bit 22) so
            // that we only clear it if it is set (write 1 to clear)
            //
            portSC.AsUlong32 &= (PORTSC_RMW_PRESERVATION_MASK | (1<<22));
            portSC.PortLinkStateWriteStrobe = 1;
            portSC.PortLinkState = PORT_LINK_STATE_U0;

            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "Writing portSC 0x%08x", portSC.AsUlong32);
            Register_WriteUlong(portSCRegister, portSC.AsUlong32);

            portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "After portSC   0x%08x", portSC.AsUlong32);

        }


    } FINALLY {

        if (releaseLock) {
            KeReleaseSpinLock(&portData->ReadModifyWriteLock,
                              irql);
        }
    }

    return hideChange;

}



__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
RootHub_HandlePortStatusChangeEvent(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        PTRB            EventTrb
    )
/*++

Routine Description:

    This function is called by the interrupter when a port status change
    event is received.

--*/
{
    ULONG   portNumber;
    PULONG              portSCRegister;
    PORTSC              portSC;

    TRY {

        NT_ASSERT(EventTrb != NULL);

        portNumber = EventTrb->PortStatusChangeEvent.PortId;

        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "PortNumber %d", portNumber);

        RootHub_DumpPortData(&RootHubData->PortRegisterSet[portNumber-1].PortStatusAndControl,
                             RootHubData->ControllerHandle);

        if (RootHub_HideInvalidDebugPortStatusAndChange(RootHubData, portNumber)) {

            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "For Debug Port %u, acknowledging and ignoring Port Status Change", portNumber);

            KeAcquireSpinLockAtDpcLevel(&RootHubData->PortData[portNumber-1].ReadModifyWriteLock);

            portSCRegister = &RootHubData->PortRegisterSet[portNumber-1].
                             PortStatusAndControl.AsUlong32;

            portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "Before portSC 0x%08x", portSC.AsUlong32);

            //
            // Unconditionally clear all change bits.
            //
            portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
            portSC.ConnectStatusChange   = 1;
            portSC.WarmPortResetChange   = 1;
            portSC.OverCurrentChange     = 1;
            portSC.PortResetChange       = 1;
            portSC.PortLinkStateChange   = 1;
            portSC.PortConfigErrorChange = 1;

            Register_WriteUlong(portSCRegister, portSC.AsUlong32);

            KeReleaseSpinLockFromDpcLevel(&RootHubData->PortData[portNumber-1].ReadModifyWriteLock);

            portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "After portSC 0x%08x", portSC.AsUlong32);

            LEAVE;
        }

        if (FALSE == RootHub_DetectAndAcknowledgePortResume(RootHubData, portNumber, FALSE)) {
                //
                // This change does not need to be hidden from the
                // USBHUB3 driver so tell the class extension that a
                // root hub port changed.
                //
                UcxRootHubPortChanged(RootHubData->UcxRootHub);

        }

    } FINALLY {

    }

    return;
}

VOID
RootHub_WdfEvtTimer20PortResumeComplete(
    __in
        PEX_TIMER  Timer,
    __in_opt
        PVOID      Context
    )
/*++

Routine Description:

    This WdfTimer event routine is used to implement the following:

    4.15.2 Port Resume

    4.15.2.1 Device Initiated
    3) b. For a USB2 protocol port, when a resume signaling is detected
          from a device the xHC shall transmit the resume signaling
          within 1 ms (TURSM).  Software shall ensure that resume is
          signaled for at least 20 ms (TDRSMDN).  Refer to section
          7.1.7.7 of the USB2 spec.  Software shall start timing TDRSMDN
          from the notification of the transition to the Resume state.
          After TDRSMDN is complete, software shall write a '0' to the
          PLS field.

    4.15.2.2 Host Initiated
    1) b. For a USB2 protocol port, software shall write a '15' (Resume)
          to the PLS field to initiate resume signaling.  The port shall
          transition to the U3Exit substate and the xHC shall transmit
          the resume signaling within 1 ms (TURSM).  Software shall
          ensure that resume is signaled for at least 20 ms (TDRSMDN).
          Software shall start timing TDRSMDN from the write of '15'
          (Resume) to PLS.  After TDRSMDN is complete, software shall
          write a '0' (U0) to the PLS field.


    This routines executes after the TDRSMDN delay is complete.

    This routine executes at DISPATCH_LEVEL.

--*/
{
    PROOTHUB_TIMER_DATA timerData;
    PROOTHUB_DATA       rootHubData;
    ULONG               portNumber;
    PULONG              portSCRegister;
    PORTSC              portSC;
    PULONG              portPMSCRegister;
    PORTPMSC            portPMSC;

    TRY {

        UNREFERENCED_PARAMETER(Timer);

        __analysis_assume(Context != NULL);

        timerData = (PROOTHUB_TIMER_DATA)Context;
        rootHubData = timerData->RootHubData;
        portNumber = timerData->PortNumber;

        if (Controller_IsControllerAccessible(rootHubData->ControllerHandle) == FALSE) {

            LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                    "controller gone");
            LEAVE;
        }

        KeAcquireSpinLockAtDpcLevel(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock);

        portSCRegister = &rootHubData->PortRegisterSet[portNumber-1].
                         PortStatusAndControl.AsUlong32;

        LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                "RootHub_WdfEvtTimer20PortResumeComplete: PortNumber %d", portNumber);

        portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
        LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                "RootHub_WdfEvtTimer20PortResumeComplete: Before portSC  0x%08x", portSC.AsUlong32);

        //
        // After TDRSMDN is complete, software shall write a '0' to the
        // PLS field.  We will also clear the PortLinkChange bit as well.
        //
        portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
        portSC.PortLinkState = PORT_LINK_STATE_U0;
        portSC.PortLinkStateChange = 1;

        portSC.PortLinkStateWriteStrobe = 1;
        LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                "RootHub_WdfEvtTimer20PortResumeComplete: Writing portSC 0x%08x", portSC.AsUlong32);
        Register_WriteUlong(portSCRegister, portSC.AsUlong32);

        KeReleaseSpinLockFromDpcLevel(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock);

        portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
        LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                "RootHub_WdfEvtTimer20PortResumeComplete: After portSC   0x%08x", portSC.AsUlong32);

        //
        // See if we need to also restore LPM
        //
        if (Controller_GetDeviceFlags(rootHubData->ControllerHandle).Disable20LPMPriorToU3) {
            if (rootHubData->PortData[portNumber-1].HardwareLPMEnabled) {

                portPMSCRegister = &rootHubData->PortRegisterSet[portNumber-1].
                                   PortPowerManagementStatusAndControl.AsUlong32;
                KeAcquireSpinLockAtDpcLevel(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock);

                portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);

                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "Re-enabling 2.0 LPM: Before portPMSC  0x%08x", portPMSC.AsUlong32);
                portPMSC.Usb2.HardwareLPMEnable = 1;

                LogInfo(Controller_GetRecorderLog(rootHubData->ControllerHandle), RootHub,
                        "Re-enabling 2.0 LPM: Writing portPMSC 0x%08x", portPMSC.AsUlong32);
                Register_WriteUlong(portPMSCRegister, portPMSC.AsUlong32);

                KeReleaseSpinLockFromDpcLevel(&rootHubData->PortData[portNumber-1].ReadModifyWriteLock);
            }
        }

    } FINALLY {

    }

    return;
}

_Requires_lock_held_(RootHubData->PortData[PortNumber-1].ReadModifyWriteLock)
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
RootHub_RestoreU1U2Timeouts(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber
    )
/*++

Routine Description:

    This routine is called upon returning to U0 from U3 when the XHCI
    controller requires a workaround to force a port to U0 prior to
    transitioning the port to U3.  Upon doing so, the U1 and U2 timers
    needed to be disabled to prevent the port from leaving U0 on its own.
    This function restores the U1 and U2 settings last programmed in the port.

--*/
{

    PULONG                          portPMSCRegister;
    PORTPMSC                        portPMSC;

    TRY {

        portPMSCRegister = &RootHubData->PortRegisterSet[PortNumber-1].
                           PortPowerManagementStatusAndControl.AsUlong32;

        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "Workaround: Restoring U1/U2 timeouts on U0 transition");
        portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);
        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "RootHub_RestoreU1U2Timeouts: Before portPMSC  0x%08x", portPMSC.AsUlong32);

        portPMSC.Usb3.U1Timeout = RootHubData->PortData[PortNumber-1].U1Timeout;
        portPMSC.Usb3.U2Timeout = RootHubData->PortData[PortNumber-1].U2Timeout;

        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "RootHub_RestoreU1U2Timeouts: Writing portPMSC 0x%08x", portPMSC.AsUlong32);

        Register_WriteUlong(portPMSCRegister, portPMSC.AsUlong32);

        portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);
        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "RootHub_RestoreU1U2Timeouts: After portPMSC   0x%08x", portPMSC.AsUlong32);

    } FINALLY {

    }
}

BOOLEAN
RootHub_IsAcknowledgedResumePending(
    __in
        PROOTHUB_PORT_DATA  PortData,
    __in
        PORTSC              PortSC
    )
/*++

Routine Description:

    This routine checks the state of a port to determine whether or not a previous remote wake
    event might have been acknowledged by the XHCI driver without another U3 entry prior to
    powering down the controller.  This routine assumes that the hub driver will put all ports in U3
    prior to powering down.

--*/
{

    if (PortSC.AsUlong32 == HARDWARE_GONE_ULONG32_REGISTER_VALUE) {
        return FALSE;
    }

    if (!PortData->ResumeAcknowledged) {
        return FALSE;
    }

    //
    // If the port got disabled, or there was a connect change,
    // then the U0 transition wouldn't succeed.
    //

    if ((PortSC.PortPower == 0) ||
        (PortSC.CurrentConnectStatus == 0) ||
        (PortSC.PortEnabled == 0) ||
        (PortSC.ConnectStatusChange == 1) ||
        (PortSC.PortLinkState == PORT_LINK_STATE_DISABLED)) {
        return FALSE;
    }

    if ((PortSC.PortLinkState == PORT_LINK_STATE_RESUME) ||
        (PortSC.PortLinkState == PORT_LINK_STATE_RECOVERY)) {
        return TRUE;
    }

    return FALSE;
}


BOOLEAN
RootHub_IsU3NeededPriorToControllerSuspend(
    __in
        PROOTHUB_PORT_DATA      PortData,
    __in
        PORTSC                  PortSC
    )
/*++

Routine Description:

    This routine checks the state of a port to determine whether or not a port needs to
    be forced back into U3 before suspending the controller.  The caller will have already ensured
    that all pending resume operations have been completed before calling this routing.

--*/
{
    UNREFERENCED_PARAMETER(PortData);
    if (PortSC.AsUlong32 == HARDWARE_GONE_ULONG32_REGISTER_VALUE) {
        return FALSE;
    }

    //
    // If the port is off, empty, disabled, or there is an unserviced connect change, then
    // there is no need to force the port to U3
    //
    if ((PortSC.PortPower == 0) ||
        (PortSC.CurrentConnectStatus == 0) ||
        (PortSC.PortEnabled == 0) ||
        (PortSC.ConnectStatusChange == 1) ||
        (PortSC.PortLinkState == PORT_LINK_STATE_DISABLED)) {
        return FALSE;
    }


    //
    // We only need to reforce a U3 transition if the port is in recovery, or a
    // valid U state (0, 1, or 2).  For other link states, the link is already in a state that
    // shouldn't prevent power down.
    // Note, before this function is called, the caller should have already waited for a resume
    // operation to complete, so typically the port should NOT be in recovery.  If, howevever the port
    // resume operation failed to complete, the port might be stuck in recovery.  We will attempt to force
    // U3 on such ports anyway.
    //
    if ((PortSC.PortLinkState == PORT_LINK_STATE_RECOVERY) ||
        (PortSC.PortLinkState == PORT_LINK_STATE_U0) ||
        (PortSC.PortLinkState == PORT_LINK_STATE_U1) ||
        (PortSC.PortLinkState == PORT_LINK_STATE_U2)) {
        return TRUE;
    }

    //
    // Otherwise, there is no need to force the port to U3
    //
    return FALSE;

}

VOID
RootHub_WaitForResumeCompletion(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber,
    __out
        PPORTSC         PortSC
    )
/*++

    This routine checks the port status registers to determine if a resume operation is in progress, and if so
    waits until that operation has completed.  It is expected that outstanding 2.0 resume timers would have already
    been cancelled prior to this routine getting called.

--*/
{
    PULONG              portSCRegister;
    PORTSC              lastPortSC;
    ULONG               count;
    PROOTHUB_PORT_DATA  portData;

    TRY {

        portSCRegister = &RootHubData->PortRegisterSet[PortNumber-1].PortStatusAndControl.AsUlong32;

        portData = &RootHubData->PortData[PortNumber-1];

        PortSC->AsUlong32 = Register_ReadUlong(portSCRegister);

        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "RootHub_WaitForResumeCompletion: Port %2d portSC 0x%08x", PortNumber, PortSC->AsUlong32);

        lastPortSC.AsUlong32 = PortSC->AsUlong32;

        //
        // Wait until the resume operation (if in progress) to complete before we
        // attempt to force the port back into U3
        //
        count = DEFAULT_U0_WAIT_COUNT;

        while (RootHub_IsAcknowledgedResumePending(portData, *PortSC)) {

            if (count == 0) {
                LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                         "HW_COMPLIANCE: Port %2d Resume failed to complete before timeout", PortNumber);
                LEAVE;
            }

            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "Port %2d Resume still pending.  Waiting to retry, count = %d", PortNumber, (DEFAULT_U0_WAIT_COUNT - count));
            count--;
            KeStallExecutionProcessor(DEFAULT_U0_WAIT_INTERVAL_IN_US);

            PortSC->AsUlong32 = Register_ReadUlong(portSCRegister);
            if (PortSC->AsUlong32 != lastPortSC.AsUlong32) {
                LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                        "Port %2d portSC 0x%08x", PortNumber, PortSC->AsUlong32);
            }

        }

    } FINALLY {

    }

}

VOID
RootHub_ForceU3(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber
    )
/*++

    This routine will initiate a U3 transition on the specified port.  This routine is called if a port
    has resumed just prior to suspending this controller.

--*/
{

    PULONG              portSCRegister;
    PORTSC              portSC;
    PROOTHUB_PORT_DATA  portData;
    KIRQL               irql;
    NTSTATUS            status;

    TRY {

        portSCRegister   = &RootHubData->PortRegisterSet[PortNumber-1].PortStatusAndControl.AsUlong32;

        portData = &RootHubData->PortData[PortNumber-1];
        //
        // Some controllers interpreted the XHCI spec incorrectly and assumed
        // that the port state would always be in U0 prior to software starting
        // a U3 transition.  In order to fix this, we have to temporarily disable
        // the U1/U2 timers, bring the port back to U0, THEN set the port link state
        // to U3.
        //

        if (portData->MajorRevision == 2) {

            if (Controller_GetDeviceFlags(RootHubData->ControllerHandle).Disable20LPMPriorToU3) {
                //
                // This controller will not automatically transition from L1 to L2, so
                // LPM needs to be disabled prior to suspending the port.
                //
                status = RootHub_ForceU0AndWait(RootHubData,
                                                PortNumber);
                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }
            }

        } else {

            //
            // If this controller requires U1/U2 timers to be disabled, then
            // see if it needs to be disabled or enabled now.
            //
            if (Controller_GetDeviceFlags(RootHubData->ControllerHandle).ForceU0PriorToU3) {

                status = RootHub_ForceU0AndWait(RootHubData,
                                                PortNumber);
                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }
            }

        }

        KeAcquireSpinLock(&portData->ReadModifyWriteLock, &irql);

        //
        // Reset the resume acknowledged state
        //
        portData->ResumeAcknowledged = FALSE;

        portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "PORT_SUSPEND: Before portSC  0x%08x", portSC.AsUlong32);

        //
        // 4.15.1 Port Suspend
        // Software should not attempt to suspend a port
        // unless the port reports that it is in the
        // enabled (PED = '1', PLS < '3') state.
        //
        if ((portSC.PortEnabled == 0) || (portSC.PortLinkState >= PORT_LINK_STATE_U3)) {
            LogWarning(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                        "PORT_SUSPEND: port is not in the enabled state");
        }

        portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
        portSC.PortLinkState = PORT_LINK_STATE_U3;
        portSC.PortLinkStateWriteStrobe = 1;
        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "PORT_SUSPEND: Writing portSC 0x%08x", portSC.AsUlong32);
        Register_WriteUlong(portSCRegister, portSC.AsUlong32);

        KeReleaseSpinLock(&portData->ReadModifyWriteLock, irql);

        portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "PORT_SUSPEND: After portSC   0x%08x", portSC.AsUlong32);

    } FINALLY {

    }

}

VOID
RootHub_HandleResumedPorts(
    __in
        PROOTHUB_DATA   RootHubData
    )
/*++

    It is possible that prior to suspending the controller, a remote wake was detected on a port.
    These remote wake events are acknowledged by the XHCI driver itself without notifying the hub driver.

    When the hub and controller are powering down, the hub may have stopped handling port status changes, and the
    controller driver may have already acknowledged the remote wake, thus allowing a port to enter U0.  Before
    the controller itself is powered down, these ports need to be put back into U3 in order to allow another wake event
    to cause the system to wake (in the case of Sx), and to ensure that if the controller disables its clocks, that
    doesn't cause the link to fail.

    This routine detects if there was an auto-acknowledged resume in progress, waits for such resume to complete,
    and forces the port back into U3.

--*/
{
    PORTSC              portSC;
    ULONG		        portNumber;
    PROOTHUB_PORT_DATA  portData;

    TRY {

        for (portNumber = 1; portNumber <= RootHubData->NumberOfPorts; portNumber++) {

            portData = &RootHubData->PortData[portNumber-1];

            //
            // For 2.0 ports, when the port indicates resume, we start a 20ms timer to time
            // the duration of resume signalling before we write U0 to the PORTSC to complete
            // the resume sequence.  This code attempts to stop such a timer.  If the timer was
            // pending when it was cancelled, then the port will remain in the RESUME state, which
            // should cause the control to initiate wake once powered down.
            //
            if (portData->MajorRevision == 2) {
                if (ExCancelTimer(portData->ExTimer20PortResumeComplete,
                                  NULL)) {
                    //
                    // In this case, the 2.0 port was resuming, but we were able to cancel the
                    // timer before we acknowledged the resume.  This should allow that resume
                    // to cause our WaitWake irp to complete once we power down.
                    //
                    portData->ResumeAcknowledged = 0;
                    LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                        "Cancelled ExTimer20PortResumeComplete timer before acknowledging resume, port %d", portNumber);
                    continue;
                }
                else {
                    LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                        "Attempted to cancel ExTimer20PortResumeComplete timer before acknowledging resume; ExCancelTimer returned FALSE, port %d", portNumber);
                }
            } else if (portData->MajorRevision != 3) {
                //
                // We don't know how to handle a port that isn't USB 2 or USB 3
                //
                continue;

            }
            RootHub_WaitForResumeCompletion(RootHubData,
                                            portNumber,
                                            &portSC);

            if (RootHub_IsU3NeededPriorToControllerSuspend(portData,
                                                           portSC)) {
                RootHub_ForceU3(RootHubData,
                                portNumber);
            }

        }

    } FINALLY {

    }

}

BOOLEAN
RootHub_IsPortU3TransitionPending(
    __in
        PROOTHUB_PORT_DATA  PortData,
    __in
        PORTSC              PortSC
    )
/*++

Routine Description:

    This routine checks the state of a port to determine whether or not a U3
    transition might still be in progress, assuming the caller had previously initiated
    the transition.  This function does not check whether the driver has actually started
    the transition.  This routine is called prior to completing D0Exit, and prior to initiating
    a port resume.

--*/
{

    //
    // If the port is in U3, then the transition must be complete
    //
    if (PortSC.PortLinkState == PORT_LINK_STATE_U3) {
        return FALSE;
    }

    //
    // If the port is off, empty, disabled, or there is an unserviced connect change, then
    // a U3 transition can't be in progress
    //
    if ((PortSC.PortPower == 0) ||
        (PortSC.CurrentConnectStatus == 0) ||
        (PortSC.PortEnabled == 0) ||
        (PortSC.ConnectStatusChange == 1) ||
        (PortSC.PortLinkState == PORT_LINK_STATE_DISABLED)) {
        return FALSE;
    }

    //
    // It could be that the U3 transition completed and the device already
    // tried to initiate resume
    //
    if (PortSC.PortLinkState == PORT_LINK_STATE_RESUME) {
        return FALSE;
    }

    //
    // Then there are some error states that a link could have reached which would
    // prevent the link from reaching U3
    //
    if ((PortSC.PortLinkState == PORT_LINK_STATE_INACTIVE) ||
        (PortSC.PortLinkState == PORT_LINK_STATE_COMPLIANCE_MODE)) {
        return FALSE;
    }

    //
    // Then there are the really unexpected link states
    //
    if ((PortSC.PortLinkState == PORT_LINK_STATE_HOT_RESET) ||
        (PortSC.PortLinkState == PORT_LINK_STATE_TEST_MODE)) {
        // TODO: Add HW_COMPLIANCE log here
        return FALSE;
    }

    //
    // We also don't expect the port to be resetting at the time
    //
    if (PortSC.PortReset == 1) {
        return FALSE;
    }


    //
    // We may have already acknowledged hardware generated resume signalling,
    // then there isn't a U3 transition pending.
    //
    if (PortData->ResumeAcknowledged) {
        return FALSE;
    }
    //
    // Otherwise, the transition must still be pending
    //
    return TRUE;

}

BOOLEAN
RootHub_IsPortConnected(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber
    )
/*++

Routine Description:

    Some platforms can do additional power optimization when the controller
    suspends, if they know that there are no devices connected downstream.
    This function will return this information for the given port

--*/
{
    PULONG      portSCRegister;
    PORTSC      portSC;

    portSCRegister = &RootHubData->PortRegisterSet[PortNumber-1].PortStatusAndControl.AsUlong32;
    portSC.AsUlong32 = Register_ReadUlong(portSCRegister);

    //
    // If the port is off, empty, disabled, or in error, then
    // the port is considered disconnected from the point of view
    // of power optimizations
    //
    if ((portSC.PortPower == 0) ||
        (portSC.CurrentConnectStatus == 0) ||
        (portSC.PortLinkState == PORT_LINK_STATE_DISABLED) ||
        (portSC.PortLinkState == PORT_LINK_STATE_INACTIVE) ||
        (portSC.PortLinkState == PORT_LINK_STATE_COMPLIANCE_MODE)) {
        return FALSE;
    }

    return TRUE;
}


NTSTATUS
RootHub_WaitForPendingU3TransitionCompletion(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber,
    __in
        BOOLEAN         RetryU3
    )
/*++

Routine Description:

    This routine checks if a U3 transition is pending, and if so, waits
    for the transition to complete.

--*/
{

    PULONG      portSCRegister;
    PORTSC      portSC;
    PORTSC      lastPortSC;
    ULONG       retryCount;
    NTSTATUS    status;

    TRY {
        status = STATUS_UNSUCCESSFUL;
        retryCount = 0;
        portSCRegister = &RootHubData->PortRegisterSet[PortNumber-1].PortStatusAndControl.AsUlong32;
        lastPortSC.AsUlong32 = 0xffffffff;

        do {
            //
            // Some controllers miss the U3 request from software, so they require a retry
            //
            if (RetryU3 &&
                (retryCount > 0) &&
                (retryCount % DEFAULT_U3_RETRY_INTERVAL == 0)) {

                KIRQL irql;
                PORTSC writeValue;

                KeAcquireSpinLock(&RootHubData->PortData[PortNumber-1].ReadModifyWriteLock, &irql);

                portSC.AsUlong32 = Register_ReadUlong(portSCRegister);

                writeValue.AsUlong32 = portSC.AsUlong32;

                writeValue.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                writeValue.PortLinkState = PORT_LINK_STATE_U3;
                writeValue.PortLinkStateWriteStrobe = 1;
                LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                        "Retrying U3 for port %d. PortSC currently 0x%08x, writing 0x%08x", PortNumber, portSC.AsUlong32, writeValue.AsUlong32);
                Register_WriteUlong(portSCRegister, writeValue.AsUlong32);
                KeReleaseSpinLock(&RootHubData->PortData[PortNumber-1].ReadModifyWriteLock, irql);
            }

            portSC.AsUlong32 = Register_ReadUlong(portSCRegister);

            //
            // Only log the port states when they change to keep from filling the logs
            // Also, if the port state is unchanged, then no need to check if the transition
            // has completed.
            //
            if (portSC.AsUlong32 != lastPortSC.AsUlong32) {
                lastPortSC = portSC;

                if (RootHubData->PortData[PortNumber-1].MajorRevision == 2) {

                    LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                                    "2.0 Port %2d portSC 0x%08x", PortNumber, portSC.AsUlong32);


                } else if (RootHubData->PortData[PortNumber-1].MajorRevision == 3) {

                    LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                            "3.0 Port %2d portSC 0x%08x", PortNumber, portSC.AsUlong32);

                } else {

                    //
                    // Unknown port type.  Not sure what to do with these.  Lets just
                    // return success to unblock the caller.
                    //
                    LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                            "HW_COMPLIANCE: Unknown type Port %2d portSC 0x%08x", PortNumber, portSC.AsUlong32);
                    status = STATUS_SUCCESS;
                    LEAVE;
                }

                if (RootHub_IsPortU3TransitionPending(&RootHubData->PortData[PortNumber-1],portSC) == FALSE) {

                    //
                    // Port is in a valid state for suspend, continue to the next port
                    //
                    status = STATUS_SUCCESS;
                    LEAVE;
                }

                LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                        "Port %2d U3 Transition still pending after retry # %d. Waiting to retry", PortNumber, retryCount);
            }

            //
            // We need to wait for a bit before polling again.  The
            // timing of a U3 transition from U0 is quite fast.  However, the link may have
            // been in U2 when the U3 transition was initiated, so the link needed to first
            // transition from U2 to U0, and then transition to U3.  The U2 transition should,
            // under successful conditions, always be less than 2ms (sometimes much less).
            // Under error conditions, it could be longer.  The typical timing may very well
            // land between what is too long to busy wait, but much shorter than a timer tick.
            // Waiting a timer tick will likely add significant delay to controller suspend,
            // so instead, busy wait
            //

            KeStallExecutionProcessor(DEFAULT_U3_WAIT_INTERVAL_IN_US);
            retryCount++;

        } while (retryCount < DEFAULT_U3_WAIT_COUNT);

    } FINALLY {

        if (retryCount == DEFAULT_U3_WAIT_COUNT) {

            LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                     "HW_COMPLIANCE: Port %d U3 Transition did not complete before timeout (PortSC 0x%08x)",
                     PortNumber, portSC.AsUlong32);

        } else if (retryCount > 0) {

            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "Waited %d intervals for U3 transition to complete for port %d",
                    retryCount, PortNumber);

        }

    }

    return status;

}


NTSTATUS
RootHub_ForceU0AndWait(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber
    )
/*++

Routine Description:

    This routine is called prior to transitioning a port
    to U3 when the XHCI controller requires a workaround to force a port
    to U0 prior to transitioning the port to U3.

    For USB 3.0 ports, this routine disables the U1 and U2 timers to ensure that the port doesn't
    leave U0 on its own, then it initiates a U0 transition.  This function waits for
    some amount of time for the port to successfully reach the U0 state or indicating
    an error prior to returnning.

    For USB 2.0 ports, if hardware LPM is enabled, this routine will disable it, then
    wait for the automatic transition to U0 to complete.

--*/
{
    NTSTATUS                        status;
    PULONG                          portSCRegister;
    PULONG                          portPMSCRegister;
    PORTSC                          portSC;
    PORTPMSC                        portPMSC;
    KIRQL                           irql;
    ULONG                           count;
    PROOTHUB_PORT_DATA              portData;

    TRY {

        portSCRegister   = &RootHubData->PortRegisterSet[PortNumber-1].
                           PortStatusAndControl.AsUlong32;

        portPMSCRegister = &RootHubData->PortRegisterSet[PortNumber-1].
                           PortPowerManagementStatusAndControl.AsUlong32;

        portData = &RootHubData->PortData[PortNumber-1];

        if (portData->MajorRevision == 2) {
            if (portData->HardwareLPMEnabled == 0) {
                status = STATUS_SUCCESS;
                LEAVE;
            }
            KeAcquireSpinLock(&portData->ReadModifyWriteLock, &irql);

            portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);

            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "Disabling 2.0 LPM: Before portPMSC  0x%08x", portPMSC.AsUlong32);
            portPMSC.Usb2.HardwareLPMEnable = 0;

            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "Disabling 2.0 LPM: Writing portPMSC 0x%08x", portPMSC.AsUlong32);
            Register_WriteUlong(portPMSCRegister, portPMSC.AsUlong32);

            KeReleaseSpinLock(&portData->ReadModifyWriteLock, irql);

        } else {

            KeAcquireSpinLock(&portData->ReadModifyWriteLock, &irql);
            //
            // We need to disable the U1 and U2 timeouts in order to ensure that
            // once we put the port into U0, it stays that way.
            //

            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "Workaround: Disabling U1/U2 timeouts and forcing U0 on U3 transition");

            portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);
            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "RootHub_ForceU0AndWait: Before portPMSC  0x%08x", portPMSC.AsUlong32);

            portPMSC.Usb3.U1Timeout = 0;
            portPMSC.Usb3.U2Timeout = 0;

            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "RootHub_ForceU0AndWait: Writing portPMSC 0x%08x", portPMSC.AsUlong32);

            Register_WriteUlong(portPMSCRegister, portPMSC.AsUlong32);

            portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);
            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "RootHub_ForceU0AndWait: After portPMSC   0x%08x", portPMSC.AsUlong32);

            //
            // Now put the port into U0
            //

            portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "RootHub_ForceU0AndWait: Before portSC  0x%08x", portSC.AsUlong32);

            portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
            portSC.PortLinkState = PORT_LINK_STATE_U0;
            portSC.PortLinkStateWriteStrobe = 1;

            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "RootHub_ForceU0AndWait: Writing portSC 0x%08x", portSC.AsUlong32);
            Register_WriteUlong(portSCRegister, portSC.AsUlong32);

            KeReleaseSpinLock(&portData->ReadModifyWriteLock, irql);
        }

        count = DEFAULT_U0_WAIT_COUNT;

        do {

            portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
            LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                    "RootHub_ForceU0AndWait: portSC after %d us 0x%08x", DEFAULT_U0_WAIT_INTERVAL_IN_US * (DEFAULT_U0_WAIT_COUNT - count),
                    portSC.AsUlong32);

            //
            // We need to wait for the port to get back to U0
            //
            if (portSC.AsUlong32 == HARDWARE_GONE_ULONG32_REGISTER_VALUE) {
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            if (portSC.PortLinkState == PORT_LINK_STATE_U0) {
                //
                // We are done
                //
                status = STATUS_SUCCESS;
                break;
            }

            //
            // Check for failures
            //
            if ((portSC.PortLinkState == PORT_LINK_STATE_RX_DETECT) ||
                (portSC.PortLinkState == PORT_LINK_STATE_INACTIVE) ||
                (portSC.PortLinkState == PORT_LINK_STATE_COMPLIANCE_MODE)) {

                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            //
            // If the port got disabled, or there was a connect change,
            // then the U0 transition wouldn't succeed.

            if ((portSC.PortPower == 0) ||
                (portSC.CurrentConnectStatus == 0) ||
                (portSC.PortEnabled == 0) ||
                (portSC.ConnectStatusChange == 1) ||
                (portSC.PortLinkState == PORT_LINK_STATE_DISABLED)) {
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            //
            // Then there are some error states that a link could have reached which would
            // prevent the link from reaching U0
            //
            if ((portSC.PortLinkState == PORT_LINK_STATE_INACTIVE) ||
                (portSC.PortLinkState == PORT_LINK_STATE_COMPLIANCE_MODE)) {
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            //
            // Then there are the really unexpected link states
            //
            if ((portSC.PortLinkState == PORT_LINK_STATE_HOT_RESET) ||
                (portSC.PortLinkState == PORT_LINK_STATE_TEST_MODE)) {
                status = STATUS_UNSUCCESSFUL;
                LogError(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                        "HW_COMPLIANCE: Port Link State is %d -- this is unexpected.", portSC.PortLinkState);
                LEAVE;
            }

            //
            // We also don't expect the port to be resetting at the time
            //
            if (portSC.PortReset == 1) {
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            if (count == 0) {
                //
                // It never got set
                //
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            NT_ASSERT(count > 0);
            count--;
            KeStallExecutionProcessor(DEFAULT_U0_WAIT_INTERVAL_IN_US);

        } WHILE (TRUE);

    } FINALLY {

    }

    return status;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
CRASHDUMP_PORT_TYPE
RootHub_GetPortType(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber
    )
{
    if (RootHubData->PortData[PortNumber-1].MajorRevision == 2) {
        return CrashdumpPortType20;
    } else {
        return CrashdumpPortType30;
    }
}

VOID
RootHub_Update20HardwareLpmParameters(
    __in
        PROOTHUB_DATA                                   RootHubData,
    __in
        ULONG                                           PortNumber,
    __in
        USBDEVICE_UPDATE_20_HARDWARE_LPM_PARAMETERS     LpmParameters,
    __in
        ULONG                                           DeviceSlot
    )
/*++

Routine Description:

    This routine is called when the hub sends down an UpdateUSBDevice command,
    specifying that the 20 LPM parameters are to be updated.  This function
    programs the port registers with these new parameters.

--*/
{
    PULONG      portPMSCRegister;
    PULONG      portHLPMCRegister;
    PORTPMSC    portPMSC;
    PORTHLPMC   portHLPMC;
    KIRQL       irql;

    TRY {

        NT_ASSERT(RootHubData->PortData[PortNumber-1].MajorRevision == 2);

        portPMSCRegister = &RootHubData->PortRegisterSet[PortNumber-1].
                           PortPowerManagementStatusAndControl.AsUlong32;

        portHLPMCRegister = &RootHubData->PortRegisterSet[PortNumber-1].
                            PortHardwareLPMControl.AsUlong32;


        KeAcquireSpinLock(&RootHubData->PortData[PortNumber-1].ReadModifyWriteLock, &irql);

        portHLPMC.AsUlong32 = Register_ReadUlong(portHLPMCRegister);
        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "Updating20HardwareLpm: Before portHLPMC  0x%08x", portHLPMC.AsUlong32);
        portHLPMC.BestEffortServiceLatencyDeep = LpmParameters.BestEffortServiceLatencyDeep;
        portHLPMC.HostInitiatedResumeDurationMode = LpmParameters.HostInitiatedResumeDurationMode;
        portHLPMC.L1Timeout = LpmParameters.L1Timeout;
        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "Updating20HardwareLpm: Writing portHLPMC 0x%08x", portHLPMC.AsUlong32);
        Register_WriteUlong(portHLPMCRegister, portHLPMC.AsUlong32);

        portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);

        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "Updating20HardwareLpm: Before portPMSC  0x%08x", portPMSC.AsUlong32);
        portPMSC.Usb2.BestEfforServiceLatency = LpmParameters.BestEffortServiceLatency;
        portPMSC.Usb2.L1DeviceSlot = DeviceSlot;
        portPMSC.Usb2.RemoteWakeEnable = LpmParameters.RemoteWakeEnable;
        portPMSC.Usb2.HardwareLPMEnable = LpmParameters.HardwareLpmEnable;
        RootHubData->PortData[PortNumber-1].HardwareLPMEnabled = (LpmParameters.HardwareLpmEnable != 0);

        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "Updating20HardwareLpm: Writing portPMSC 0x%08x", portPMSC.AsUlong32);
        Register_WriteUlong(portPMSCRegister, portPMSC.AsUlong32);

        //
        // Record that the HardwareLPM states have been updated from their initial, disabled state
        //
        RootHubData->PortData[PortNumber-1].HardwareLPMDirty = TRUE;

        KeReleaseSpinLock(&RootHubData->PortData[PortNumber-1].ReadModifyWriteLock, irql);

    } FINALLY {

    }

}

VOID
RootHub_DisableLPMForSlot(
    __in
        PROOTHUB_DATA           RootHubData,
    __in
        ULONG                   PortNumber,
    __in
        ULONG                   DeviceSlot
    )
/*++

Routine Description:

    This routine is called when a device slot is disabled in the controller.
    For USB 2.0 LPM, the slot number is programmed into the port registers.  In
    order to prevent the port register from referencing an invalid port, this is
    the opportunitiy to clear it out.  It may be possible that the port has already
    been assigned to a new slot, so it is important to verify that the slot number
    in the port register matches the slot number of the device being disabled before
    modifying the port registers

--*/
{
    PULONG      portPMSCRegister;
    PULONG      portHLPMCRegister;
    PORTPMSC    portPMSC;
    PORTHLPMC   portHLPMC;
    KIRQL       irql;

    TRY {

        NT_ASSERT(RootHubData->PortData[PortNumber-1].MajorRevision == 2);

        portPMSCRegister = &RootHubData->PortRegisterSet[PortNumber-1].
                            PortPowerManagementStatusAndControl.AsUlong32;

        portHLPMCRegister = &RootHubData->PortRegisterSet[PortNumber-1].
                            PortHardwareLPMControl.AsUlong32;


        KeAcquireSpinLock(&RootHubData->PortData[PortNumber-1].ReadModifyWriteLock, &irql);
        if (RootHubData->PortData[PortNumber-1].HardwareLPMDirty == FALSE) {
            LEAVE;
        }
        portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);
        if (portPMSC.Usb2.L1DeviceSlot != DeviceSlot) {
            LEAVE;
        }

        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "RootHub_ClearSlotNumberInPort: Before portPMSC  0x%08x", portPMSC.AsUlong32);
        portPMSC.Usb2.BestEfforServiceLatency = 0;
        portPMSC.Usb2.L1DeviceSlot = 0;
        portPMSC.Usb2.RemoteWakeEnable = 0;
        portPMSC.Usb2.HardwareLPMEnable = 0;

        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "RootHub_ClearSlotNumberInPort: Writing portPMSC 0x%08x", portPMSC.AsUlong32);
        Register_WriteUlong(portPMSCRegister, portPMSC.AsUlong32);

        portHLPMC.AsUlong32 = Register_ReadUlong(portHLPMCRegister);
        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "RootHub_ClearSlotNumberInPort: Before portHLPMC  0x%08x", portHLPMC.AsUlong32);
        portHLPMC.BestEffortServiceLatencyDeep = 0;
        portHLPMC.HostInitiatedResumeDurationMode = 0;
        portHLPMC.L1Timeout = 0;
        LogInfo(Controller_GetRecorderLog(RootHubData->ControllerHandle), RootHub,
                "RootHub_ClearSlotNumberInPort: Writing portHLPMC 0x%08x", portHLPMC.AsUlong32);
        Register_WriteUlong(portHLPMCRegister, portHLPMC.AsUlong32);

        portPMSC.AsUlong32 = Register_ReadUlong(portPMSCRegister);

        //
        // Record that the HardwareLPM states have been reset to their initial, disabled state
        //
        RootHubData->PortData[PortNumber-1].HardwareLPMDirty = FALSE;


    } FINALLY {

        KeReleaseSpinLock(&RootHubData->PortData[PortNumber-1].ReadModifyWriteLock, irql);

    }
}

BOOLEAN
RootHub_HideInvalidDebugPortStatusAndChange(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber
    )
{
    if (KD_DEBUGGER_ENABLED &&
        RootHubData->DebugCapable &&
        Controller_GetDeviceFlags(RootHubData->ControllerHandle).HideInvalidDebugPortStatusAndChange &&
        (Register_ReadDebugPortNumber(Controller_GetRegisterHandle(RootHubData->ControllerHandle))
         == PortNumber)) {

        return TRUE;

    } else {

        return FALSE;
    }
}

USB_VERSION
RootHub_GetHighestUsbVersionSupported(
    __in
        PROOTHUB_DATA   RootHubData
    )
/*++

Routine Description:

    Scan through all the ports declared by the controller and find the highest
    USB version supported by any of the ports. This version information will
    be included in the Friendly Name string of the xHCI controller, which
    shows up in Device Manager.

    Note: If the xHCI controller does not declare any USB 3.* ports, the highest
    version returned will be 2.*.

--*/
{
    USB_VERSION         highestVersion;
    PROOTHUB_PORT_DATA  portData;
    ULONG               portNumber;

    highestVersion.MajorVersion = 0;
    highestVersion.MinorVersion = 0;

    for (portNumber = 1; portNumber <= RootHubData->NumberOfPorts; portNumber += 1) {

        portData = &RootHubData->PortData[portNumber - 1];

        if (portData->MajorRevision > highestVersion.MajorVersion) {

            highestVersion.MajorVersion = portData->MajorRevision;

            if (portData->MinorRevision > highestVersion.MinorVersion) {
                highestVersion.MinorVersion = portData->MinorRevision;
            }
        }
    }

    return highestVersion;
}
