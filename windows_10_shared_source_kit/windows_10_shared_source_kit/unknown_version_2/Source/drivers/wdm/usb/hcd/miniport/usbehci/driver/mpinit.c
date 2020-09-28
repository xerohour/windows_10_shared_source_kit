/*++

Copyright (c) 1999, 2000  Microsoft Corporation

Module Name:

   mpinit.c

Abstract:

   miniport initialization

Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1999, 2000 Microsoft Corporation.  All Rights Reserved.


Revision History:

    2-19-99 : created, jdunn

--*/

#include "common.h"

// global registration packet for this miniport
USBPORT_REGISTRATION_PACKET RegistrationPacket;

DRIVER_INITIALIZE DriverEntry;

NTSTATUS
DriverEntry(
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    Installable driver initialization entry point.
    This entry point is called directly by the I/O system.

Arguments:

    DriverObject - pointer to the driver object

    RegistryPath - pointer to a unicode string representing the path
                   to driver-specific key in the registry

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    ULONG mn;

    // Initialize global to make NonPagedPool be treated as NxPool on Win8
    // and NonPagedPool on down-level
    //
    ExInitializeDriverRuntime(DrvRtPoolNxOptIn);

    mn = USBPORT_GetHciMn();

    if (mn != USB_HCI_MN) {

        return STATUS_UNSUCCESSFUL;
    }

    RegistrationPacket.DeviceDataSize =
        sizeof(DEVICE_DATA);
    RegistrationPacket.EndpointDataSize =
        sizeof(ENDPOINT_DATA);
    RegistrationPacket.TransferContextSize =
        sizeof(TRANSFER_CONTEXT);

    RegistrationPacket.CommonBufferBytes = ROUND_TO_PAGES(USBEHCI_COMMON_BUFFER_BYTES);

    RegistrationPacket.CommonBufferBlockCount = 1;
    // 1024 'dummy' queue heads
    RegistrationPacket.CommonBufferBlockBytes[0] =
        sizeof(HCD_QUEUEHEAD_DESCRIPTOR) * 1024;

    RegistrationPacket.MINIPORT_PreStartController =
        EHCI_PreStartController;
    RegistrationPacket.MINIPORT_StartController =
        EHCI_StartController;
    RegistrationPacket.MINIPORT_StopController =
        EHCI_StopController;
    RegistrationPacket.MINIPORT_SuspendController =
        EHCI_SuspendController;
    RegistrationPacket.MINIPORT_ResumeController =
        EHCI_ResumeController;
    RegistrationPacket.MINIPORT_EnableInterrupts =
        EHCI_EnableInterrupts;
    RegistrationPacket.MINIPORT_DisableInterrupts =
        EHCI_DisableInterrupts;
    RegistrationPacket.MINIPORT_InterruptService =
        EHCI_InterruptService;
    RegistrationPacket.MINIPORT_ResetController =
        EHCI_ResetController;
    RegistrationPacket.MINIPORT_ResetControllerDontReinit =
        EHCI_ResetControllerDontReinit;
    RegistrationPacket.MINIPORT_ReinitController =
        EHCI_ReinitController;
    RegistrationPacket.MINIPORT_PostResetController =
        EHCI_PostResetController;

    // root hub functions
    RegistrationPacket.MINIPORT_RH_DisableIrq =
        EHCI_RH_DisableIrq;
    RegistrationPacket.MINIPORT_RH_EnableIrq =
        EHCI_RH_EnableIrq;
    RegistrationPacket.MINIPORT_RH_GetRootHubData =
        EHCI_RH_GetRootHubData;
    RegistrationPacket.MINIPORT_RH_GetStatus =
        EHCI_RH_GetStatus;
    RegistrationPacket.MINIPORT_RH_GetHubStatus =
        EHCI_RH_GetHubStatus;
    RegistrationPacket.MINIPORT_RH_GetPortStatus =
        EHCI_RH_GetPortStatus;

    RegistrationPacket.MINIPORT_RH_SetFeaturePortReset =
        EHCI_RH_SetFeaturePortReset;
    RegistrationPacket.MINIPORT_RH_SetFeaturePortPower =
        EHCI_RH_SetFeaturePortPower;
    RegistrationPacket.MINIPORT_RH_SetFeaturePortEnable =
        EHCI_RH_SetFeaturePortEnable;
    RegistrationPacket.MINIPORT_RH_SetFeaturePortSuspend =
        EHCI_RH_SetFeaturePortSuspend;

    RegistrationPacket.MINIPORT_RH_ClearFeaturePortEnable =
        EHCI_RH_ClearFeaturePortEnable;
    RegistrationPacket.MINIPORT_RH_ClearFeaturePortPower =
        EHCI_RH_ClearFeaturePortPower;
    RegistrationPacket.MINIPORT_RH_ClearFeaturePortSuspend =
        EHCI_RH_ClearFeaturePortSuspend;
    RegistrationPacket.MINIPORT_RH_ClearFeaturePortEnableChange =
        EHCI_RH_ClearFeaturePortEnableChange;
    RegistrationPacket.MINIPORT_RH_ClearFeaturePortConnectChange =
        EHCI_RH_ClearFeaturePortConnectChange;
    RegistrationPacket.MINIPORT_RH_ClearFeaturePortResetChange =
        EHCI_RH_ClearFeaturePortResetChange;
    RegistrationPacket.MINIPORT_RH_ClearFeaturePortSuspendChange =
        EHCI_RH_ClearFeaturePortSuspendChange;
    RegistrationPacket.MINIPORT_RH_ClearFeaturePortOvercurrentChange =
        EHCI_RH_ClearFeaturePortOvercurrentChange;

    RegistrationPacket.MINIPORT_SetEndpointStatus =
        EHCI_SetEndpointStatus;
    RegistrationPacket.MINIPORT_GetEndpointStatus =
        EHCI_GetEndpointStatus;
    RegistrationPacket.MINIPORT_SetEndpointDataToggle =
        EHCI_SetEndpointDataToggle;
    RegistrationPacket.MINIPORT_OpenEndpoint =
        EHCI_OpenEndpoint;
    RegistrationPacket.MINIPORT_PokeEndpoint =
        EHCI_PokeEndpoint;
    RegistrationPacket.MINIPORT_QueryEndpointRequirements =
        EHCI_QueryEndpointRequirements;
    RegistrationPacket.MINIPORT_CloseEndpoint =
        EHCI_CloseEndpoint;
    RegistrationPacket.MINIPORT_PollEndpoint =
        EHCI_PollEndpoint;
    RegistrationPacket.MINIPORT_SetEndpointState =
        EHCI_SetEndpointState;
    RegistrationPacket.MINIPORT_GetEndpointState =
        EHCI_GetEndpointState;
    RegistrationPacket.MINIPORT_Get32BitFrameNumber =
        EHCI_Get32BitFrameNumber;
    RegistrationPacket.MINIPORT_PollController =
        EHCI_PollController;
    RegistrationPacket.MINIPORT_CheckController =
        EHCI_CheckController;
    RegistrationPacket.MINIPORT_InterruptNextSOF =
        EHCI_InterruptNextSOF;
    RegistrationPacket.MINIPORT_SubmitTransfer =
        EHCI_SubmitTransfer;
    RegistrationPacket.MINIPORT_InterruptDpc =
        EHCI_InterruptDpc;
    RegistrationPacket.MINIPORT_AbortTransfer =
        EHCI_AbortTransfer;
    RegistrationPacket.MINIPORT_StartSendOnePacket =
        EHCI_StartSendOnePacket;
    RegistrationPacket.MINIPORT_EndSendOnePacket =
        EHCI_EndSendOnePacket;
    RegistrationPacket.MINIPORT_PassThru =
        EHCI_PassThru;
    RegistrationPacket.MINIPORT_SubmitIsoTransfer =
        EHCI_SubmitIsoTransfer;
    RegistrationPacket.MINIPORT_RebalanceEndpoint =
        EHCI_RebalanceEndpoint;
    RegistrationPacket.MINIPORT_FlushInterrupts =
        EHCI_FlushInterrupts;
    RegistrationPacket.MINIPORT_Chirp_RH_Port =
        EHCI_RH_ChirpRootPort;
    RegistrationPacket.MINIPORT_TakePortControl =
        EHCI_TakePortControl;
    RegistrationPacket.MINIPORT_ReleasePortControl =
        EHCI_ReleasePortControl;
    RegistrationPacket.MINIPORT_ReadCfgFlag =
        EHCI_ReadConfigFlag;
    RegistrationPacket.MINIPORT_SetWakeOnConnect =
        EHCI_RH_SetWakeConnect;
    RegistrationPacket.MINIPORT_InterruptDpcEx =
        EHCI_InterruptDpcEx;
    RegistrationPacket.MINIPORT_NotifyTransferQueueState =
        EHCI_NotifyTransferQueueState;
    RegistrationPacket.MINIPORT_PollHwSync =
        EHCI_CheckHwSync;
    RegistrationPacket.MINIPORT_CreateDeviceData =
        EHCI_CreateDeviceData;
    RegistrationPacket.MINIPORT_DeleteDeviceData =
        EHCI_DeleteDeviceData;
    //RegistrationPacket.MINIPORT_CloseDeviceHandle =
    //    EHCI_CloseDeviceHandle;
    //RegistrationPacket.MINIPORT_CreateDeviceHandle =
    //    EHCI_CreateDeviceHandle;
    RegistrationPacket.MINIPORT_DbgFreeEndpoint =
        EHCI_DbgFreeEndpoint;
    RegistrationPacket.MINIPORT_HaltController =
        EHCI_HaltController;
#ifdef VISTA_RTM
    RegistrationPacket.MINIPORT_Get32BitMicroFrameNumber =
        EHCI_Get32BitMicroFrameNumber;
#endif

    RegistrationPacket.MINIPORT_GetHcCapabilities =
        EHCI_GetHcCapabilities;

    RegistrationPacket.MINIPORT_InitializeDumpInterface =
        EHCIDumpInitializeInterface;

    RegistrationPacket.MINIPORT_CleanupDumpInterface =
        EHCIDumpCleanUpInterface;

    RegistrationPacket.MINIPORT_GetDebugPortNumber =
        EHCI_GetDebugPortNumber;
	
    RegistrationPacket.MINIPORT_CheckInvalidateRootHub =
        EHCI_CheckInvalidateRootHub;

    RegistrationPacket.MINIPORT_SetPortResumeTime =
        EHCI_SetPortResumeTime;

    RegistrationPacket.Name[0] = 'U';
    RegistrationPacket.Name[1] = 'S';
    RegistrationPacket.Name[2] = 'B';
    RegistrationPacket.Name[3] = 'E';
    RegistrationPacket.Name[4] = 'H';
    RegistrationPacket.Name[5] = 'C';
    RegistrationPacket.Name[6] = 'I';
    RegistrationPacket.Name[7] = 0;


    RegistrationPacket.OptionFlags = USB_MINIPORT_OPT_NEED_IRQ |
                                     USB_MINIPORT_OPT_NEED_MEMORY |
                                     USB_MINIPORT_OPT_USB20 |
                                     USB_MINIPORT_OPT_POLL_IN_SUSPEND |
                                    // disable ss sometimes for testing
                                    // USB_MINIPORT_OPT_NO_SS |
                                     USB_MINIPORT_OPT_HW_SYNC |
                                     USB_MINIPORT_OPT_POLL_CONTROLLER |
                                     USB_MINIPORT_OPT_SUSPEND_DISABLE_IRQ |
                                     USB_MINIPORT_OPT_NO_DOUBLE_BUFFER;

    RegistrationPacket.HciType = USB_EHCI;
    RegistrationPacket.BusBandwidth = USB_20_BUS_BANDWIDTH;

    DriverObject->DriverUnload = NULL;

    return USBPORT_RegisterUSBPortDriver(
                DriverObject,
                USB_MINIPORT_HCI_VERSION,
                &RegistrationPacket,
                RegistryPath);
}
