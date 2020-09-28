/*++
Copyright (c) Microsoft Corporation

Module Name:

    crashdumpregister.c

Abstract:

    USB XHCI Crashdump write module.

--*/

#include "pch.h"

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Crashdump_Register_Initialize(
    __inout
        PCRASHDUMP_REGISTER_DATA    RegisterData,
    __in
        HANDLE                      RegisterHandle,
    __in
        HANDLE                      ControllerHandle
    )
{
    HANDLE deviceSlotHandle;

    deviceSlotHandle = Controller_GetDeviceSlotHandle(ControllerHandle);
    
    RegisterData->RegisterHandle = RegisterHandle;
    RegisterData->DeviceFlags = Controller_GetDeviceFlags(ControllerHandle);
    RegisterData->MaximumDeviceSlots = Register_GetSupportedNumberOfDeviceSlots(RegisterData->RegisterHandle);
    RegisterData->MaxScratchpadBuffers = Register_GetMaxScratchpadBuffers(RegisterData->RegisterHandle);

    //
    // We are reusing runtime stacks scratchpad buffer. Assuming this is safe to
    // do as the system software is not supposed to touch this buffer
    //
    if (RegisterData->MaxScratchpadBuffers > 0) {
        RegisterData->ScratchpadBufferArray.LogicalAddress.QuadPart = DeviceSlot_GetScratchpadBufferLA(deviceSlotHandle);
        RegisterData->ScratchpadBufferArray.VirtualAddress = DeviceSlot_GetScratchpadBufferVA(deviceSlotHandle);
        RegisterData->ScratchpadBufferArray.Size = RegisterData->MaxScratchpadBuffers * sizeof(ULONG64);
        RegisterData->ScatchpadBufferDatas = DeviceSlot_GetScratchpadBufferDatas(deviceSlotHandle);
    }
}

VOID
Crashdump_Register_PrepareForHibernate(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    )
{
    PBUFFER_DATA    currentBuffer;
    ULONG           i;
    
    if (RegisterData->MaxScratchpadBuffers > 0) {
        Crashdump_BufferMarkForHibernate(RegisterData->ScratchpadBufferArray.VirtualAddress,
                                         RegisterData->ScratchpadBufferArray.Size);

        currentBuffer = (PBUFFER_DATA)RegisterData->ScatchpadBufferDatas.Flink;

        for (i=0; i<RegisterData->MaxScratchpadBuffers; i++) {

            Crashdump_BufferMarkForHibernate(currentBuffer->VirtualAddress, currentBuffer->Size);

            currentBuffer = (PBUFFER_DATA)currentBuffer->ListEntry.Flink;
        }
    }
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Register_VerifyRegisterState(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    )
{
    NTSTATUS                status;
    USBSTS                  usbStatus;
    POPERATIONAL_REGISTERS  operationalRegisters;

    TRY {

        operationalRegisters = Register_GetOperationalRegisters(RegisterData->RegisterHandle);

        usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);

        if (usbStatus.ControllerNotReady == 1) {
            CrashdumpLogError("xHC is not ready, cannot continue.");
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        if (usbStatus.HostSystemError == 1) {
            CrashdumpLogError("Host System Error bit is set, cannot continue.");
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        if (usbStatus.HostControllerError == 1) {
            CrashdumpLogError("xHC is in an error state, cannot continue.");
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Register_StopController(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    )
{
    NTSTATUS                status;
    ULONG                   count;
    USBCMD                  usbCommand;
    USBSTS                  usbStatus;
    POPERATIONAL_REGISTERS  operationalRegisters;

    TRY {

        operationalRegisters = Register_GetOperationalRegisters(RegisterData->RegisterHandle);

        usbCommand.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbCommand.AsUlong32);
        usbCommand.RunStopBit = 0;
        Register_WriteUlong(&operationalRegisters->UsbCommand.AsUlong32, usbCommand.AsUlong32);

        //
        // Run-time stack waits for 4ms, which is too little for us because
        // bus is not guaranteed to be idle. So we wait for the full 16ms
        // per the spec (5.4.1).
        //
        for (count = 0; count < CRASHDUMP_CONTROLLER_STOP_WAIT_COUNT; count++) {

            usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);

            if (usbStatus.HcHalted == 1) {

                CrashdumpLogInfo("xHC successfully stopped in %u ms",
                                 ((count * CRASHDUMP_CONTROLLER_STOP_WAIT_TIME) / 1000));

                status = STATUS_SUCCESS;
                LEAVE;
            }

            KeStallExecutionProcessor(CRASHDUMP_CONTROLLER_STOP_WAIT_TIME);
        }

        CrashdumpLogError("xHC failed to stop in %u ms", ((count * CRASHDUMP_CONTROLLER_STOP_WAIT_TIME) / 1000));

        status = STATUS_ADAPTER_HARDWARE_ERROR;

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Register_StartController(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    )
{
    NTSTATUS                status;
    ULONG                   count;
    USBCMD                  usbCommand;
    USBSTS                  usbStatus;
    POPERATIONAL_REGISTERS  operationalRegisters;

    TRY {

        operationalRegisters = Register_GetOperationalRegisters(RegisterData->RegisterHandle);

        usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);
        NT_ASSERT(usbStatus.HcHalted == 1);

        usbCommand.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbCommand.AsUlong32);
        usbCommand.RunStopBit = 1;
        Register_WriteUlong(&operationalRegisters->UsbCommand.AsUlong32, usbCommand.AsUlong32);

        //
        // Spec doesn't say how long to wait, but not waiting doesn't work.
        //
        for (count = 0; count < CRASHDUMP_CONTROLLER_STOP_WAIT_COUNT; count++) {

            usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);

            if (usbStatus.HcHalted == 0) {

                CrashdumpLogInfo("xHC successfully started in %u ms",
                                 ((count * CRASHDUMP_CONTROLLER_STOP_WAIT_TIME) / 1000));

                status = STATUS_SUCCESS;
                LEAVE;
            }

            KeStallExecutionProcessor(CRASHDUMP_CONTROLLER_STOP_WAIT_TIME);
        }

        CrashdumpLogError("xHC failed to start in %u ms", ((count * CRASHDUMP_CONTROLLER_STOP_WAIT_TIME) / 1000));

        status = STATUS_ADAPTER_HARDWARE_ERROR;

    } FINALLY {

    }

    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Register_ResetController(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    )
/*++

Routine Description:

--*/
{
    NTSTATUS                status;
    ULONG                   count;
    USBCMD                  usbCommand;
    POPERATIONAL_REGISTERS  operationalRegisters;

    TRY {

        status = Crashdump_Register_StopController(RegisterData);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        operationalRegisters = Register_GetOperationalRegisters(RegisterData->RegisterHandle);

        usbCommand.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbCommand.AsUlong32);
        usbCommand.HostControllerReset = 1;
        Register_WriteUlong(&operationalRegisters->UsbCommand.AsUlong32, usbCommand.AsUlong32);

        for (count = 0; count < CRASHDUMP_CONTROLLER_RESET_WAIT_COUNT; count++) {

            usbCommand.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbCommand.AsUlong32);

            if (usbCommand.HostControllerReset == 0) {

                CrashdumpLogInfo("xHC successfully reset in %u ms",
                                 ((count * CRASHDUMP_CONTROLLER_RESET_WAIT_TIME) / 1000));

                status = STATUS_SUCCESS;
                LEAVE;
            }

            KeStallExecutionProcessor(CRASHDUMP_CONTROLLER_RESET_WAIT_TIME);
        }

        CrashdumpLogError("xHC failed to reset in %u ms", ((count * CRASHDUMP_CONTROLLER_RESET_WAIT_TIME) / 1000));

        status = STATUS_ADAPTER_HARDWARE_ERROR;

    } FINALLY {

    }

    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Register_ResetPort(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData,
    __in
        ULONG                       PortNumber,
    __in
        CRASHDUMP_PORT_TYPE         PortType
    )
/*++

Routine Description:

    This routine resets a root-hub port and ensures successful completion
    of the reset.

Reference:

    4.3.1       - Resetting a Root Hub Port.

--*/
{
    NTSTATUS            status;
    PORTSC              portSC;
    PULONG              portSCRegister;
    PPORT_REGISTER_SET  portRegisterSet;
    ULONG               count;

    TRY {

        CrashdumpLogInfo("Crashdump_Register_ResetPort: begin: port %u", PortNumber);

        portRegisterSet = Register_GetPortRegisterSet(RegisterData->RegisterHandle);
        portSCRegister = &portRegisterSet[PortNumber-1].PortStatusAndControl.AsUlong32;

        //
        // Wait for the port to be ready for reset 
        //
        CrashdumpLogInfo("Polling port %u for device presence", PortNumber);
        for (count = 0; count < CRASHDUMP_PORT_LINK_STATE_WAIT_COUNT; count++) {

            portSC.AsUlong32 = Register_ReadUlong(portSCRegister);

            if (portSC.CurrentConnectStatus == 1) {
                
                break;
            }

            if (PortType == CrashdumpPortType30 && 
                (portSC.PortLinkState == PORT_LINK_STATE_COMPLIANCE_MODE ||
                 portSC.PortLinkState == PORT_LINK_STATE_INACTIVE)) {

                break;
            }                       

            KeStallExecutionProcessor(CRASHDUMP_PORT_LINK_STATE_WAIT_TIME);
        }

        if (count == CRASHDUMP_PORT_LINK_STATE_WAIT_COUNT) {

            //
            // No device is found present even after waiting for the entire timeout
            //
            CrashdumpLogError("Unable to detect a device on port %u in %u ms",
                              PortNumber,
                              ((count * CRASHDUMP_PORT_LINK_STATE_WAIT_TIME) / 1000));

            status = STATUS_DEVICE_DOES_NOT_EXIST;

            LEAVE;
        }

        CrashdumpLogInfo("Device found present on port %u", PortNumber);

        //
        // Clear CSC bit before continuing
        //
        portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
        portSC.ConnectStatusChange = 1;
        Register_WriteUlong(portSCRegister, portSC.AsUlong32);

        //
        // Issue port reset if port reset is not in progress
        //
        portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
        if (portSC.PortReset == 1) {
            
            CrashdumpLogInfo("Port reset is underway on port %u.", PortNumber);
        } else {
        
            CrashdumpLogInfo("Issuing port reset on port %u.", PortNumber);
            portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
            if (PortType == CrashdumpPortType20) {

                portSC.PortReset = 1;
            } else {

                portSC.WarmPortReset = 1;
            }
            Register_WriteUlong(portSCRegister, portSC.AsUlong32);
        }

        //
        // 5.4.8 - PED shall automatically be cleared to '0' when PR is set
        // to '1', and set to '1' when PR transitions from '1' to '0' after
        // a successful reset.
        //
        CrashdumpLogInfo("Checking port %u for reset completion.", PortNumber);
        for (count = 0; count < CRASHDUMP_PORT_RESET_WAIT_COUNT; count++) {

            portSC.AsUlong32 = Register_ReadUlong(portSCRegister);

            //
            // 4.19.5.1 The Port Reset (PR) flag shall be '1' while Hot or Warm Reset 
            // is being executed. The Port Reset Change (PRC) flag shall be set ('1') 
            // when the reset execution is complete and PR transitions to '0'.
            //
            
            if (portSC.PortResetChange == 1) {

                //
                // If the following checks fail we will loop again till we timeout
                //
                if ((portSC.CurrentConnectStatus == 1) &&
                    (portSC.PortReset == 0) &&
                    (portSC.PortEnabled == 1) &&
                    (portSC.PortLinkState == PORT_LINK_STATE_U0) &&
                    (portSC.PortSpeed != PORT_SPEED_FULL_SPEED_DEVICE_ATTACHED) &&
                    (portSC.PortSpeed != PORT_SPEED_LOW_SPEED_DEVICE_ATTACHED) &&
                    (portSC.PortSpeed != PORT_SPEED_UNDEFINED_DEVICE_ATTACHED)) {

                    CrashdumpLogInfo("Port %u is correctly reset in %u ms",
                                     PortNumber,
                                     ((count * CRASHDUMP_PORT_RESET_WAIT_TIME) / 1000));

                    //
                    // If we are here, it means that the port reset completed successfully
                    // Let us clear the resetchange bits. Only PortResetChange for 2.0 and
                    // both PortResetChange and WarmPortResetChange for 3.0
                    //
                    portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
                    if (PortType == CrashdumpPortType30) {
                        portSC.WarmPortResetChange= 1;
                    }
                    portSC.PortResetChange = 1;
                    Register_WriteUlong(portSCRegister, portSC.AsUlong32);

                    status = STATUS_SUCCESS;
                    LEAVE;

                }
            }

            KeStallExecutionProcessor(CRASHDUMP_PORT_RESET_WAIT_TIME);
        }

        //
        // The port was not successfully reset even after waiting for the entire timeout
        //
        CrashdumpLogError("Port %u failed to reset in %u ms",
                          PortNumber,
                          ((count * CRASHDUMP_PORT_RESET_WAIT_TIME) / 1000));

        status = STATUS_ADAPTER_HARDWARE_ERROR;

    } FINALLY {

        Crashdump_Register_LogRHPortInfo(RegisterData, PortNumber);
        CrashdumpLogInfo("Crashdump_Register_ResetPort: end 0x%X", status);
    }

    return status;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Crashdump_Register_LogRHPortInfo(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData,
    __in
        ULONG                       PortNumber
    )
{
    PORTSC              portSC;
    PULONG              portSCRegister;
    PPORT_REGISTER_SET  portRegisterSet;

    /*
    //
    // PortChangeDetect always stays at 1. Even if you write 1 to clear.
    //
    USBSTS                  usbStatus;
    POPERATIONAL_REGISTERS  operationalRegisters;

    operationalRegisters = Register_GetOperationalRegisters(RegisterData->RegisterHandle);

    usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);
    CrashdumpLogInfo("PortChangeDetect: %u", usbStatus.PortChangeDetect);

    usbStatus.PortChangeDetect = 0;
    Register_WriteUlong(&operationalRegisters->UsbStatus.AsUlong32, usbStatus.AsUlong32);

    usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);
    CrashdumpLogInfo("PortChangeDetect: %u", usbStatus.PortChangeDetect);
    */

    portRegisterSet = Register_GetPortRegisterSet(RegisterData->RegisterHandle);
    portSCRegister = &portRegisterSet[PortNumber-1].PortStatusAndControl.AsUlong32;
    portSC.AsUlong32 = Register_ReadUlong(portSCRegister);

    CrashdumpLogInfo("Port %u: CCS: %u, PE: %u, OC: %u, PR: %u, PLS: %u, PP: %u, PS: %u, PIC: %u",
                     PortNumber,
                     portSC.CurrentConnectStatus,
                     portSC.PortEnabled,
                     portSC.OverCurrentActive,
                     portSC.PortReset,
                     portSC.PortLinkState,
                     portSC.PortPower,
                     portSC.PortSpeed,
                     portSC.PortIndicatorControl);

    CrashdumpLogInfo("PortChange %u: CSC: %u, PEDC: %u, OCC: %u, PRC: %u, WPRC: %u, PLSC: %u, PCEC: %u",
                     PortNumber,
                     portSC.ConnectStatusChange,
                     portSC.PortEnabledOrDisabledChange,
                     portSC.OverCurrentChange,
                     portSC.PortResetChange,
                     portSC.WarmPortResetChange,
                     portSC.PortLinkStateChange,
                     portSC.PortConfigErrorChange);
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Crashdump_Register_UpdateDeviceSlotInfo(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData,
    __in
        ULONG                       NumberOfDeviceSlots,
    __in
        CRASHDUMP_BUFFER_DATA       DeviceContextBase
    )
{
    POPERATIONAL_REGISTERS  operationalRegisters;
    CONFIG_REGISTER         configRegister;
    PULONG64                deviceContextBaseAddress;

    operationalRegisters = Register_GetOperationalRegisters(RegisterData->RegisterHandle);

    NT_ASSERT(NumberOfDeviceSlots <= RegisterData->MaximumDeviceSlots);

    CrashdumpLogInfo("MaximumDeviceSlots: %u, NumberOfDeviceSlotsEnabled: %u",
                      RegisterData->MaximumDeviceSlots,
                      NumberOfDeviceSlots);

    //
    // 5.4.7 Configure Register (CONFIG)
    //
    configRegister.AsUlong32 = 0;
    configRegister.NumberOfDeviceSlotsEnabled = NumberOfDeviceSlots;

    //
    // Disable it for now.
    //
#if 0
    configRegister.ConfigurationInformationEnable = (Register_GetConfigurationInformation(RegisterData->RegisterHandle) != 0);
#endif

    Register_WriteUlong(&operationalRegisters->ConfigRegister.AsUlong32, configRegister.AsUlong32);

    //
    // 5.4.6 Device Context Base Address Array Pointer Register (DCBAAP)
    //
    Register_WriteUlong64(&operationalRegisters->DeviceContextBaseAddressArrayPointer.QuadPart,
                          DeviceContextBase.LogicalAddress.QuadPart,
                          (Crashdump_Register_GetDeviceFlags(RegisterData).Use32BitRegisterAccess == 1));

    if (RegisterData->MaxScratchpadBuffers > 0) {

        deviceContextBaseAddress = (PULONG64)DeviceContextBase.VirtualAddress;
        deviceContextBaseAddress[0] = RegisterData->ScratchpadBufferArray.LogicalAddress.QuadPart;
    }
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Register_SuspendPort(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData,
    __in
        ULONG                       PortNumber
    )
{
    
    NTSTATUS            status;
    PORTSC              portSC;
    PULONG              portSCRegister;
    PPORT_REGISTER_SET  portRegisterSet;

    TRY {

        CrashdumpLogInfo("Crashdump_Register_SuspendPort: begin: port %u", PortNumber);

        //
        // The way to suspend ports in 3.0 is to set the port link state to U3
        //
        portRegisterSet = Register_GetPortRegisterSet(RegisterData->RegisterHandle);
        portSCRegister = &portRegisterSet[PortNumber-1].PortStatusAndControl.AsUlong32;

        portSC.AsUlong32 = Register_ReadUlong(portSCRegister);
        portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
        portSC.PortLinkState = PORT_LINK_STATE_U3;

        Register_WriteUlong(portSCRegister, portSC.AsUlong32);

        status = STATUS_SUCCESS;

    } FINALLY {

        CrashdumpLogInfo("Crashdump_Register_SuspendPort: end 0x%X", status);
    }

    return status;
}


__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Register_SaveStateForHybridSleep(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    )
{
    NTSTATUS                status;
    ULONG                   count;
    USBCMD                  usbCommand;
    USBSTS                  usbStatus;
    POPERATIONAL_REGISTERS  operationalRegisters;

    TRY {

        operationalRegisters = Register_GetOperationalRegisters(RegisterData->RegisterHandle);

        usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);

        if (usbStatus.HcHalted == 0) {
            CrashdumpLogError("Controller not halted");
            status = STATUS_ADAPTER_HARDWARE_ERROR;
            LEAVE;
        }

        if ((usbStatus.SaveStateStatus == TRUE) || (usbStatus.RestoreStateStatus == TRUE)) {
            CrashdumpLogError("Controller State Save or Restore operation in progress");
            status = STATUS_ADAPTER_HARDWARE_ERROR;
            LEAVE;
        }

        usbCommand.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbCommand.AsUlong32);
        usbCommand.ControllerSaveState = 1;
        Register_WriteUlong(&operationalRegisters->UsbCommand.AsUlong32, usbCommand.AsUlong32);

        for(count = 0; count < DEFAULT_STATE_SAVE_WAIT_COUNT; count++) {

            usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);

            if (usbStatus.SaveRestoreError == 1) {

                CrashdumpLogError("Controller State Save failed");

                //
                // Clear the Save/Restore Error
                //
                usbStatus.AsUlong32 = 0;
                usbStatus.SaveRestoreError = 1;
                Register_WriteUlong(&operationalRegisters->UsbStatus.AsUlong32, usbStatus.AsUlong32);
                status = STATUS_ADAPTER_HARDWARE_ERROR;
                LEAVE;
            }

            if (usbStatus.SaveStateStatus == 0) {
                CrashdumpLogInfo("Controller State Save successful");
                break;
            }

            //
            // Delay for 200 microseconds before re-reading the register.
            //
            KeStallExecutionProcessor(DEFAULT_STATE_SAVE_WAIT_TIME);

        } 

        if (count == DEFAULT_STATE_SAVE_WAIT_COUNT) {
            CrashdumpLogError("Controller State Save failed to complete");
            status = STATUS_IO_TIMEOUT;
            LEAVE;
        }

        status = STATUS_SUCCESS;
    } FINALLY {
    }
    return status;
}


__checkReturn
NTSTATUS
Crashdump_Register_BiosHandoff(
    __in
    PCRASHDUMP_REGISTER_DATA    RegisterData
    )
/*++

Routine Description:

    This routine attempts to perform the BIOS handoff by unconditionally setting
    the OS_OWN semaphore, followed by ensuring that the BIOS_OWN semaphore gets
    cleared.

--*/
{
    USBSTS                  usbStatus;
    PUSBLEGSUPCAPABILITY    usbLegSupCapability;
    PUSBLEGSUP              usbLegacySupport;
    PUSBLEGCTLSTS           usbLegacySupportControlStatus;
    USBLEGCTLSTS            usbLegacySupportControlStatusValue;
    HC_BIOS_OWNED_SEMAPHORE hcBiosOwnedSemaphore;
    HC_OS_OWNED_SEMAPHORE   hcOsOwnedSemaphore;
    ULONG                   count;
    ULONG                   clearTime;
    NTSTATUS                status;
    POPERATIONAL_REGISTERS  operationalRegisters;

    TRY {

        usbLegSupCapability = Register_GetLegacySupportCapability(RegisterData->RegisterHandle);

        if (usbLegSupCapability == NULL) {

            CrashdumpLogError("Legacy support capability not implemented, BIOS handoff will not be attempted.");

        } else {

            usbLegacySupport = &usbLegSupCapability->UsbLegacySupportCapability;

            //
            // Unconditionally set the OS_OWN semaphore.
            //
            hcOsOwnedSemaphore.AsUchar8 =
                Register_ReadUchar(&usbLegacySupport->HcOsOwnedSemaphore.AsUchar8);

            hcOsOwnedSemaphore.HcOsOwnedSemaphore = 1;

            Register_WriteUchar(&usbLegacySupport->HcOsOwnedSemaphore.AsUchar8,
                                hcOsOwnedSemaphore.AsUchar8);

            //
            // Poll for the BIOS_OWN semaphore to clear.
            //
            count = DEFAULT_HC_BIOS_WAIT_COUNT;
            clearTime = 0;

            do {

                hcBiosOwnedSemaphore.AsUchar8 =
                    Register_ReadUchar(&usbLegacySupport->HcBiosOwnedSemaphore.AsUchar8);

                if (hcBiosOwnedSemaphore.HcBiosOwnedSemaphore == 0) {

                    CrashdumpLogInfo("HC BIOS Owned Semaphore bit cleared after %dus", clearTime);
                    status = STATUS_SUCCESS;
                    break;
                }

                if (count == 0) {

                    CrashdumpLogInfo("HC BIOS Owned Semaphore bit not cleared after %dus. Ignoring for this controller.",
                                     DEFAULT_HC_BIOS_WAIT_COUNT * DEFAULT_HC_BIOS_WAIT_TIME_US);

                    status = STATUS_SUCCESS;
                    break;
                }

                count--;
                clearTime += DEFAULT_HC_BIOS_WAIT_TIME_US;

                //
                // Delay for 100ms before re-reading the register.
                //
                KeStallExecutionProcessor(DEFAULT_HC_BIOS_WAIT_TIME_US);

            } WHILE (TRUE);
        }

        //
        // Controller should be halted after BIOS handoff. If it is not then halt it
        //
        operationalRegisters = Register_GetOperationalRegisters(RegisterData->RegisterHandle);
        usbStatus.AsUlong32 =
            Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);

        if (usbStatus.HcHalted == FALSE) {

            CrashdumpLogError("Controller not halted after BIOS handoff, attempting to stop.");
            status = Crashdump_Register_StopController(RegisterData);
            if (!NT_SUCCESS(status)) {
                CrashdumpLogError("Failed to stop controller 0x%X", status);
                LEAVE;
            }
        } else {
            CrashdumpLogInfo("Controller halted after BIOS handoff.");
        }

        //
        // Turn off SMI on OS ownership change. This is to workaround a problem on some
        // systems that on resume from S3, OS ownership change bit gets set, resulting in an
        // SMI storm and system hang. So always turn off this SMI after a BIOS handoff.
        //
        if (usbLegSupCapability != NULL) {

            usbLegacySupportControlStatus = &usbLegSupCapability->UsbLegacySupportControlStatus;

            usbLegacySupportControlStatusValue.AsUlong32 =
                Register_ReadUlong(&usbLegacySupportControlStatus->AsUlong32);

            usbLegacySupportControlStatusValue.SmiOnOsOwnershipEnable = 0;

            //
            // Clear the RW1C bits so as not to overwrite their values.
            //
            usbLegacySupportControlStatusValue.SmiOnOsOnwershipChange = 0;
            usbLegacySupportControlStatusValue.SmiOnPciCommand = 0;
            usbLegacySupportControlStatusValue.SmiOnBar = 0;

            Register_WriteUlong(&usbLegacySupportControlStatus->AsUlong32,
                                usbLegacySupportControlStatusValue.AsUlong32);
        }

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

