/*++
Copyright (c) Microsoft Corporation

Module Name:

    userioctls.c

Abstract:

    USB Controller Extension Driver

--*/
#include "pch.h"

#include "userioctls.tmh"

_Must_inspect_result_
BOOLEAN
UsbUserIoctl_CheckAndSetParamLength(
    __in
        PUSBUSER_REQUEST_HEADER Header,
    __in
        ULONG                   ParameterLength
    )
/*++

Routine Description:

    COMPAT:
    This routine is based off USBPORT_CheckLength
    
    Checks Length of user supplied buffer based on api.
    If the length is too small, it sets the UsbUserStatusCode in the
    Header to an error value.

Arguments:

    Header - The pointer to a buffer that has a USBUSER_REQUEST_HEADER structure
    as header.
    
    ParameterLength - Expected length of the buffer (pointer by Header), in addition to 
    size of USBUSER_REQUEST_HEADER
    
Return Value:

    TRUE if buffer is large enough, FALSE if buffer too small

--*/
{
    BOOLEAN  retCode;

    TRY {
        
        Header->ActualBufferLength = sizeof(*Header) + ParameterLength;
        
        if (Header->ActualBufferLength > Header->RequestBufferLength) {
            LogError(FlagUsbDevice, "UsbUserIoctl length Expected %d, Actual %d",
                     Header->ActualBufferLength, Header->RequestBufferLength);

            Header->UsbUserStatusCode = UsbUserBufferTooSmall;
            retCode = FALSE;
            LEAVE;
        }

        retCode = TRUE;

    } FINALLY {

    }

    return retCode;
}


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
USBPORTStyle_UserGetControllerInfo_0(
    __in
        PUCXCONTROLLER_PRIVCONTEXT UcxControllerContext,
    __in
        PUSBUSER_REQUEST_HEADER    Header,
    __out 
        PUSB_CONTROLLER_INFO_0     ControllerInfo_0
    )
/*++

Routine Description:

    COMPAT: 
    This function is based off USBPORT_UserGetControllerInfo_0
    
    This routine fills the ControllerInfo_0 structure.

Arguments:

    UcxControllerContext - The Controller Context
    
    Header - The pointer to the Header of the USBUSER request
    
    ControllerInfo_0 - The pointer to the buffer that should hold the controller info.

Return Value:

    none.
--*/
{
    TRY {

        ControllerInfo_0->HcFeatureFlags = 0;
        ControllerInfo_0->NumberOfRootPorts
            = UcxControllerContext->UcxRootHubContext->NumberOf20Ports;
        ControllerInfo_0->PciVendorId = UcxControllerContext->Config.PciDeviceInfo.VendorId;
        ControllerInfo_0->PciDeviceId = UcxControllerContext->Config.PciDeviceInfo.DeviceId;
        ControllerInfo_0->PciRevision = UcxControllerContext->Config.PciDeviceInfo.RevisionId;
        ControllerInfo_0->ControllerFlavor = UcxControllerContext->ControllerFlavor;
        
        Header->UsbUserStatusCode =  UsbUserSuccess;
    
    } FINALLY {

    }
}


VOID
USBPORTStyle_UserPassThru(
    __in
        PUSBUSER_REQUEST_HEADER    Header,
    __in
        PUSB_PASS_THRU_PARAMETERS  PassThru
    )
/*++

Routine Description:

    COMPAT:
    This function is based off USBPORT_UserPassThru
        
    Handles a PassThru USER_USER Ioctl. Please see comments section.
    
Arguments:

    Header - The pointer to the Header of the USBUSER request
    
    PassThru - Pointer to PassThru params.
    
Return Value:

COMPAT:
Comments:
    This function exists just for compatibility purposes. 
    There is no useful work being done in this function.
    Please refer to the 2.0 stack implementation to see what a real implementation would do.

    2.0 Implmentation handles the following 2 GUIDS.
    GUID_USBPRIV_ROOTPORT_STATUS
    GUID_IDLE_STATUS
    
    However these guids were never made public, and only exist in internal header files. Even no
    internal component uses these guids. Thus will will not handle these anymore.
--*/
{
    ULONG length;
    ULONG parameterLength;

    TRY {

        // limit pass thru blocks to 64k
        if (PassThru->ParameterLength > 0x10000) {
            Header->UsbUserStatusCode = UsbUserInvalidParameter;
            LEAVE;
        }

        // extra length check is needed since we
        // have embedded data
        // if we get here we know packet parameters are valid
        length = sizeof(*Header) 
                 + sizeof(USB_PASS_THRU_PARAMETERS) 
                 - 4 /*USB_PASS_THRU_PARAMETERS->Parameters*/ 
                 + PassThru->ParameterLength;

        if (length > Header->RequestBufferLength) {
            Header->UsbUserStatusCode = UsbUserBufferTooSmall;
            LEAVE;
        }

        // userlength
        parameterLength = PassThru->ParameterLength;

        //
        // This should have a -4 in there (as above), however, leaving it like this
        // due to compat with 2.0 stack.
        //
        Header->ActualBufferLength = 
            sizeof(*Header) + sizeof(USB_PASS_THRU_PARAMETERS) + parameterLength;

        //
        // We dont want to handle this request.
        // Set UsbUserMiniportError as the StatusCode to maintain compat.
        // This is what the 2.0 port does the miniport fails the request.
        //
        Header->UsbUserStatusCode = UsbUserMiniportError;

    } FINALLY {

    }

}

// Hard coding the total bandwidth value that is returned by USBPORT_GetTotal20Bandwidth
#define USBPORT_20_TOTAL_BANDWIDTH (7000*8*8)
VOID
USBPORTStyle_UserGetBandwidthInformation(
    __in
        PUSBUSER_REQUEST_HEADER       Header,
    __in 
        PUSB_BANDWIDTH_INFO           BandwidthInfo
    )
/*++

Routine Description:
    COMPAT:
    This function is based off USBPORT_UserGetBandwidthInformation
    
    This function provides fake bandwidth information for the USBUSER Ioctls.

Arguments:

    Header - The pointer to the Header of the USBUSER request
    
    BandwidthInfo - Pointer to USB_BANDWIDTH_INFO structure.

Return Value:
    
    VOID

Comments:
    COMPAT:
    This function exists just for compatibility purposes. 
    We cannot provide the actual bandwidth information for many controllers.
    
TODO: PostWin8
    If the controller does support the GetBandwidthInfo API, call the controller to get information    
--*/
{
    TRY {

        RtlZeroMemory(BandwidthInfo, sizeof(USB_BANDWIDTH_INFO));

        BandwidthInfo->DeviceCount = 0;

        //
        // For Compat we are using whatever function USBPORT_GetTotal20Bandwidth
        // returns
        // This is total/mili-sec
        //
        BandwidthInfo->TotalBusBandwidth = USBPORT_20_TOTAL_BANDWIDTH;

        //
        // return allocation based on bits/32 msec
        // segment of bus time.  Data is always stored
        // in bits/frame(ms)
        // Note: The name says this is 32sec, but it is really 32 mSec.
        //
        BandwidthInfo->Total32secBandwidth = BandwidthInfo->TotalBusBandwidth * 32;

        //
        // Bulk and Interrupt is 20% of total
        //
        BandwidthInfo->AllocedBulkAndControl = (BandwidthInfo->TotalBusBandwidth/5) * 32;
        BandwidthInfo->AllocedIso = 0;            
        BandwidthInfo->AllocedInterrupt_1ms = 0;
        BandwidthInfo->AllocedInterrupt_2ms = 0;
        BandwidthInfo->AllocedInterrupt_4ms = 0;
        BandwidthInfo->AllocedInterrupt_8ms = 0;
        BandwidthInfo->AllocedInterrupt_16ms = 0;
        BandwidthInfo->AllocedInterrupt_32ms = 0;
        
        Header->ActualBufferLength =  sizeof(*Header)+sizeof(USB_BANDWIDTH_INFO);
    
    } FINALLY {

    }
}

FORCEINLINE
WDMUSB_POWER_STATE  
DevicePowerState_To_WdmUsbPowerState(
    __in 
        DEVICE_POWER_STATE DevicePowerState
    )
/*++

Routine Description:
    Translates a DEVICE_POWER_STATE into WDMUSB_POWER_STATE

Arguments:
    DevicePowerState - The Device Power state.

Return Value:    
    WDMUSB_POWER_STATE that matches the WDM power state passed in

--*/
{
    WDMUSB_POWER_STATE usbPowerState;
    TRY {
        switch(DevicePowerState) {
        case PowerDeviceUnspecified:
            usbPowerState = WdmUsbPowerDeviceUnspecified;
            LEAVE;
        case PowerDeviceD0:
            usbPowerState = WdmUsbPowerDeviceD0;
            LEAVE;
        case PowerDeviceD1:
            usbPowerState = WdmUsbPowerDeviceD1;
            LEAVE;
        case PowerDeviceD2:
            usbPowerState = WdmUsbPowerDeviceD2;
            LEAVE;
        case PowerDeviceD3:
            usbPowerState = WdmUsbPowerDeviceD3;
            LEAVE;
        default: 
            usbPowerState = WdmUsbPowerNotMapped;
            LEAVE;
        }
    } FINALLY {

    }
    return usbPowerState;
}

WDMUSB_POWER_STATE
SystemPowerState_To_WdmUsbPowerState(
    __in
        SYSTEM_POWER_STATE SystemPowerState
    )
/*++

Routine Description:
    Translates a SYSTEM_POWER_STATE into WDMUSB_POWER_STATE

Arguments:
    SystemPowerState - The System Power state.

Return Value:    
    WDMUSB_POWER_STATE that matches the WDM power state passed in
--*/
{
    WDMUSB_POWER_STATE usbPowerState;

    TRY {
        switch(SystemPowerState) {
        case PowerSystemWorking:
            usbPowerState = WdmUsbPowerSystemWorking;
            LEAVE;
        case PowerSystemSleeping1:
            usbPowerState = WdmUsbPowerSystemSleeping1;
            LEAVE;
        case PowerSystemSleeping2:
            usbPowerState = WdmUsbPowerSystemSleeping2;
            LEAVE;
        case PowerSystemSleeping3:
            usbPowerState = WdmUsbPowerSystemSleeping3;
            LEAVE;
        case PowerSystemHibernate:
            usbPowerState = WdmUsbPowerSystemHibernate;
            LEAVE;
        case PowerSystemShutdown:
            usbPowerState = WdmUsbPowerSystemShutdown;
            LEAVE;
        default:
            usbPowerState = WdmUsbPowerNotMapped;
            LEAVE;
        } 
    } FINALLY {
        
    }
    return usbPowerState;

}

VOID
USBPORTStyle_MapCanWakeAndIsPowered(
    __in
        PUSB_POWER_INFO          PowerInformation,
    __in
        PDEVICE_CAPABILITIES     HcCaps,
    __in
        SYSTEM_POWER_STATE       SystemState
    )
/*++

Routine Description:

    COMPAT: 
    This function is a derived from USBPORT_ComputeHcPowerStates. The USBPORT function 
    pre-calculates the this information for each power state and stores it in a 
    table. This derivative calculates the needed information only for the 
    needed system power state.

    (Modified) Comments from USBPORT_ComputeHcPowerStates:
    Using the HC capabilities reported by the parent bus compute
    the host controllers power attributes. 
    
    This function will store those computed attributes in PowerInformation

    Power attributes are defined as follows:

                                   {     attributes      }
     | SystemState |  DeviceState  |  Powered?  |  Wake? |
     +-------------+---------------+------------+--------+
           S1-S5          D0-D3          Y/N        Y/N

    We have four possible cases for each sleep state:

             Powered?   Wake?
    case 1      Y         Y
    case 2      N         N
    case 3      Y         N
    case 4      N         Y

    The computed attributes indicates the state of the BUS (BUS underlined), not the HC
    brains which may or may not lose power.

    Case 1 and 2 are typical of most systems in S1.
    Some systems fall into case 3, for these we try to suspend the controller
    vs turning it off since resume is faster this way.

    Case 4 does not happen in reality -- the bus has to have power to support
    resume sigalling.

Arguments:

    PowerInformation - Pointer to the USB_POWER_INFO structure
    
    HcCaps - Device Capabilities for the Host Controller
    
    SystemState - The System State for which the information is being requested.
    
Return Value:

    none

--*/
{
    SYSTEM_POWER_STATE      systemWakeState;
    DEVICE_POWER_STATE      deviceState;
    
    TRY {

        systemWakeState = HcCaps->SystemWake;
        deviceState = HcCaps->DeviceState[SystemState];
        //
        // The HC can wake the system for any sleep state lighter (<=)
        // systemWake
        //

        if (SystemState <= systemWakeState) {

            if (deviceState == PowerDeviceUnspecified) {
                //
                // for unspecified we go with case 2, ie no power
                // case 2
                //
                PowerInformation->CanWakeup = FALSE;
                PowerInformation->IsPowered = FALSE;
            } else {
                //
                // case 1
                //
                PowerInformation->CanWakeup = TRUE;
                PowerInformation->IsPowered = TRUE;
            }

        } else if (SystemState  == PowerSystemShutdown) {
            //        
            // do not support wake from the shutdown case
            // case 2
            //        
            PowerInformation->CanWakeup = FALSE;
            PowerInformation->IsPowered = FALSE;

        } else {
            if (deviceState == PowerDeviceD3 ||
                deviceState == PowerDeviceUnspecified) {
                //
                // case 2
                //
                PowerInformation->CanWakeup = FALSE;
                PowerInformation->IsPowered = FALSE;
            } else {
                //
                // case 3
                //
                PowerInformation->CanWakeup = FALSE;
                PowerInformation->IsPowered = TRUE;
            }
        }
    } FINALLY {

    }
    return;
}

VOID
USBPORTStyle_MapPowerStateInformation(
    __in
        PUSB_POWER_INFO                         PowerInformation,
    __in
        PDEVICE_CAPABILITIES                    HcCaps,
    __in
        PWDF_DEVICE_POWER_CAPABILITIES          RhCaps,
    __in
        SYSTEM_POWER_STATE                      LastSystemSleepState 
    )
/*++

Routine Description:
    
    COMPAT:
    This routine is based off USBPORT_MapPowerStateInformation
    
    This routine fill the USB_POWER_INFO structure based off the
    HcCaps and RhCaps provided.
    
Arguments:

    PowerInformation - Pointer to the USB_POWER_INFO structure
    
    HcCaps - Device Capabilities for the Host Controller
    
    RhCaps - Device Capabilities for the Root Hub.
    
Return Value:

    none.
--*/
{
    TRY {
    
        PowerInformation->RhDeviceWake = DevicePowerState_To_WdmUsbPowerState(RhCaps->DeviceWake);
        PowerInformation->RhSystemWake = SystemPowerState_To_WdmUsbPowerState(RhCaps->SystemWake);
        PowerInformation->HcDeviceWake = DevicePowerState_To_WdmUsbPowerState(HcCaps->DeviceWake);
        PowerInformation->HcSystemWake = SystemPowerState_To_WdmUsbPowerState(HcCaps->SystemWake);

        switch (PowerInformation->SystemState) {
        
        case WdmUsbPowerSystemWorking:
            PowerInformation->RhDevicePowerState = 
                DevicePowerState_To_WdmUsbPowerState(RhCaps->DeviceState[PowerSystemWorking]);
            PowerInformation->HcDevicePowerState =
                DevicePowerState_To_WdmUsbPowerState(HcCaps->DeviceState[PowerSystemWorking]);
            
            PowerInformation->CanWakeup = 0;
            PowerInformation->IsPowered = 0;
            
            break;

        case WdmUsbPowerSystemSleeping1:
            PowerInformation->RhDevicePowerState =
                DevicePowerState_To_WdmUsbPowerState(RhCaps->DeviceState[PowerSystemSleeping1]);
            PowerInformation->HcDevicePowerState =
                DevicePowerState_To_WdmUsbPowerState(HcCaps->DeviceState[PowerSystemSleeping1]);
            
            USBPORTStyle_MapCanWakeAndIsPowered(PowerInformation, HcCaps, PowerSystemSleeping1);
            break;

        case WdmUsbPowerSystemSleeping2:
            PowerInformation->RhDevicePowerState =
                DevicePowerState_To_WdmUsbPowerState(RhCaps->DeviceState[PowerSystemSleeping2]);
            PowerInformation->HcDevicePowerState =
                DevicePowerState_To_WdmUsbPowerState(HcCaps->DeviceState[PowerSystemSleeping2]);
            
            USBPORTStyle_MapCanWakeAndIsPowered(PowerInformation, HcCaps, PowerSystemSleeping2);            
            break;

        case WdmUsbPowerSystemSleeping3:
            PowerInformation->RhDevicePowerState =
                DevicePowerState_To_WdmUsbPowerState(RhCaps->DeviceState[PowerSystemSleeping3]);
            PowerInformation->HcDevicePowerState =
                DevicePowerState_To_WdmUsbPowerState(HcCaps->DeviceState[PowerSystemSleeping3]);
            
            USBPORTStyle_MapCanWakeAndIsPowered(PowerInformation, HcCaps, PowerSystemSleeping3);            
            break;

         case WdmUsbPowerSystemHibernate:
            PowerInformation->RhDevicePowerState =
                DevicePowerState_To_WdmUsbPowerState(RhCaps->DeviceState[PowerSystemHibernate]);
            PowerInformation->HcDevicePowerState =
                DevicePowerState_To_WdmUsbPowerState(HcCaps->DeviceState[PowerSystemHibernate]);
            USBPORTStyle_MapCanWakeAndIsPowered(PowerInformation, HcCaps, PowerSystemHibernate);            
            break;
        }

        PowerInformation->LastSystemSleepState =
            SystemPowerState_To_WdmUsbPowerState(LastSystemSleepState);
    
    } FINALLY {
    }
}

VOID
USBPORTStyle_UserPowerInformation(
    __in
        PUCXCONTROLLER_PRIVCONTEXT UcxControllerContext,
    __in
        PUSBUSER_REQUEST_HEADER    Header,
    __in
        PUSB_POWER_INFO            PowerInformation
    )
/*++

Routine Description:

    COMPAT: 
    This routine is a derivative of USBPORT_UserPowerInformation
    
    This routine is a wrapper for USBPORTStyle_MapPowerStateInformation

Arguments:

    UcxControllerContext - The controller context
    
    Header - The pointer to the Header of the USBUSER request
    
    PowerInformation - Pointer to the USB_POWER_INFO structure

Return Value:

    none.
--*/
{
    PDEVICE_CAPABILITIES             hcDeviceCaps;
    PWDF_DEVICE_POWER_CAPABILITIES   rhDeviceCaps;
    
    TRY {
    
        if (FALSE == UcxControllerContext->UcxRootHubContext->RHPdoStarted) {
            Header->ActualBufferLength = sizeof(*Header)+sizeof(USB_POWER_INFO);
            Header->UsbUserStatusCode = UsbUserDeviceNotStarted;
            LEAVE;
        }

        rhDeviceCaps = &UcxControllerContext->UcxRootHubContext->PowerCaps;
        hcDeviceCaps = &UcxControllerContext->HcCaps;

        USBPORTStyle_MapPowerStateInformation(
            PowerInformation,
            hcDeviceCaps,
            rhDeviceCaps,
            UcxControllerContext->UcxRootHubContext->LastSystemSleepState);

        Header->ActualBufferLength = sizeof(*Header)+sizeof(USB_POWER_INFO);

    } FINALLY {

    }
}

VOID
USBPORTStyle_UserGetBusStatistics0(
    __in
        PUCXCONTROLLER_PRIVCONTEXT UcxControllerContext,
    __in
        PUSBUSER_REQUEST_HEADER    Header,
    __in
        PUSB_BUS_STATISTICS_0      BusStatistics0
    )
/*++

Routine Description:

    COMPAT: 
    This routine is a derivative of USBPORT_UserGetBusStatistics0
    
    This routine fills in the information in the USB_BUS_STATISTICS_0 structure

Arguments:

    UcxControllerContext - The controller context
    
    Header - The pointer to the Header of the USBUSER request
    
    BusStatistics0 - Pointer to the USB_BUS_STATISTICS_0 structure

Return Value:

    none.

--*/
{
    NTSTATUS status;

    TRY {
        //
        // Build some fake values, some actual ones. 
        //
        BusStatistics0->DeviceCount = UcxControllerContext->ChildListCount;
        KeQuerySystemTime(&BusStatistics0->CurrentSystemTime);
        if (NULL != UcxControllerContext->Config.EvtControllerGetCurrentFrameNumber) {
            if (Controller_ResetReferenceAcquire(UcxControllerContext)) {
                
                status = UcxControllerContext->Config.EvtControllerGetCurrentFrameNumber(
                    UcxControllerContext->UcxController,
                    &BusStatistics0->CurrentUsbFrame);
                UcxControllerContext->CachedFrameNumber = BusStatistics0->CurrentUsbFrame;
                Controller_ResetReferenceRelease(UcxControllerContext);
                
            } else {

                status = STATUS_SUCCESS;
                BusStatistics0->CurrentUsbFrame = UcxControllerContext->CachedFrameNumber;
                LogInfo(FlagUsbDevice, 
                        "Faking EvtControllerGetCurrentFrameNumber during controller reset %d",
                        BusStatistics0->CurrentUsbFrame);                

            }
        } else {
            status = STATUS_UNSUCCESSFUL;
        }
        
        if (!NT_SUCCESS(status)){
            //
            // Fake a value
            //
            UcxControllerContext->FakeFrameNumber = 
                (UcxControllerContext->FakeFrameNumber + 1) & USB_FRAME_NUBMER_MASK;
            BusStatistics0->CurrentUsbFrame = UcxControllerContext->FakeFrameNumber;
        }

        BusStatistics0->HardResetCount = 0;
        BusStatistics0->WorkerIdleTimeMs = 0;
        BusStatistics0->WorkerSignalCount = 0;
        BusStatistics0->RootHubDevicePowerState = 4;
        BusStatistics0->RootHubEnabled = FALSE;
        
        if (UcxControllerContext->UcxRootHubContext->RHPdoStarted) {
            BusStatistics0->RootHubEnabled = TRUE;
            if (UcxControllerContext->RHPdoInD0 == FALSE) {
                BusStatistics0->RootHubDevicePowerState = 2;
            } else {
                BusStatistics0->RootHubDevicePowerState = 0;
            }
        }

        //
        // TODO: PostWin8 unless Beta Feedback suggests otherwise
        // See if you need to put actual values in here especially for data bytes, 
        // Also see if you need to fake incrementing values for PciInterruptCount to ensure
        // that apps do not believe that we are in a wedged controller situation. 
        // 

        BusStatistics0->BulkBytes = 0;
        BusStatistics0->IsoBytes = 0;
        BusStatistics0->InterruptBytes = 0;
        BusStatistics0->ControlDataBytes = 0;
        BusStatistics0->CommonBufferBytes = 0;
        BusStatistics0->PciInterruptCount = 1;

        Header->ActualBufferLength =
            sizeof(*Header)+sizeof(USB_BUS_STATISTICS_0);

    } FINALLY {

    }
}

__drv_out(
    __drv_when(Header->UsbUserStatusCode != UsbUserBufferTooSmall,
        __drv_when(ControllerKeyLength < ControllerKey->Length + sizeof(ULONG),
                   __notvalid)))
VOID
USBPORTStyle_UserGetControllerKey(
    __in
        WDFDEVICE                  Device,
    __in
        PUSBUSER_REQUEST_HEADER    Header,
    __out_bcount(ControllerKeyLength) 
        PUSB_UNICODE_NAME          ControllerKey,
    __in
        ULONG                      ControllerKeyLength
    )
/*++

Routine Description:

    COMPAT: 
    This function is based on USBPORT_UserGetControllerKey
    
    This function returns the Driver key associated with this symbolic link for the
    host controller.

Arguments:

    Device - WDFDEVICE for which the ioctl was recieved.

    Header - The pointer to the Header of the USBUSER request
    
    ControllerKey - Poiner to the output buffer where the driver
        key should be stored
        
    ControllerKeyLength - Length of ControllerKey buffer
    
Return Value:

    none.
--*/
{
    NTSTATUS                status;
    ULONG                   actualLength;
    ULONG                   adjustedControllerLength;
    TRY {

        if (ControllerKeyLength < sizeof(USB_UNICODE_NAME)) {
            Header->UsbUserStatusCode = UsbUserBufferTooSmall;
            LEAVE;
        }
        
        //
        // COMPAT:
        // The USBPORT implementation was incorrect as it zeroes out less than it should.
        // Maintaining that behavior
        //
        adjustedControllerLength = ControllerKeyLength -  sizeof(USB_UNICODE_NAME);
        
        RtlZeroMemory(ControllerKey, adjustedControllerLength);
        
        //
        // COMPAT:
        // Again, a wrong length is being passed here.
        // But mainting the behaviour of 2.0 stack
        //
        actualLength = 0;
        status = WdfDeviceQueryProperty(Device,
                                        DevicePropertyDriverKeyName,
                                        adjustedControllerLength,
                                        &ControllerKey->String[0],
                                        &actualLength);

        if (NT_SUCCESS(status)) {
            Header->UsbUserStatusCode = UsbUserSuccess;
            ControllerKey->Length = actualLength + sizeof(UNICODE_NULL);
        } else if (status == STATUS_BUFFER_TOO_SMALL) {
            Header->UsbUserStatusCode = UsbUserBufferTooSmall;
        } else {
            Header->UsbUserStatusCode = UsbUserInvalidParameter;
        }

        Header->ActualBufferLength = actualLength+sizeof(*Header)+sizeof(USB_UNICODE_NAME);

    } FINALLY {

    }

}

NTSTATUS
USBPORTStyle_GetSymbolicName(
    __in
        PUCXROOTHUB_PRIVCONTEXT       UcxRootHubContext,
    __deref_out_opt 
        WDFSTRING                     *SymbolicName
    )
/*++

Routine Description:

    COMPAT:
    This routine is a derivative of USBPORT_GetSymbolicName

    Return the symbolic name for the RootHub with leading
    \xxx\ removed

    This routine also ensures that the RootHub is not in a stopped state.
    If it is then the Symbolic name is not fetched.
    
    COMPAT:
    This is a behaviour change, since in case of USBPORT, the symbolic name
    is still returned in case of Pnp stop. Symbolic name is not returned
    in case of Remove device (that means disable from device manager).
    However in WDF it is not obvious to distinguish between Stop and Remove.
    Anyway stop happens on pnp resource re-balance which is very infrequent.
    Thus the compat risk is very very low
    
Arguments:

    UcxRootHubContext - Pointer to the context of UCXROOTHUB object
    
    SymbolicName - Output - WdfString object that represents symbolic name.
    This can be NULL even if the function returns success. In that case it means that the 
    roothub devnode is stopped / removed.    

Return Value:

    NTSTATUS

Comments:
    If this function succeeds and SymbolicName is not NULL, the caller must call WdfObjectDelete
    on SymbolicName    
--*/
{
    ULONG                   offset = 0;
    PWCHAR                  pwch;
    NTSTATUS                status;
    KIRQL                   irql;
    WDFSTRING               symbolicNameWdfString = NULL;
    WDFSTRING               symbolicNameWdfStringStripped = NULL;
    UNICODE_STRING          symbolicNameUnicodeString;
    UNICODE_STRING          symbolicNameUnicodeStringStripped;
    WDF_OBJECT_ATTRIBUTES   objectAttributes;
    ULONG                   numchars;

    TRY {

        *SymbolicName = NULL;

        KeAcquireSpinLock(&UcxRootHubContext->RHPdoInfoLock, &irql);
        if (UcxRootHubContext->RHPdoStarted &&
            NULL != UcxRootHubContext->RHPdoSymbolicName) {
            symbolicNameWdfString = UcxRootHubContext->RHPdoSymbolicName;
            WdfObjectReferenceWithTag(symbolicNameWdfString, TAG(USBPORTStyle_GetSymbolicName));
        } 
        KeReleaseSpinLock(&UcxRootHubContext->RHPdoInfoLock, irql);
        
        if (NULL == symbolicNameWdfString) {
            //
            // Due to Compat this is not a failure case.
            // This is because, USBPORTStyle_UserGetRootHubName does not fail if 
            // it is unable to reference the Roothub Pdo.
            //
            status = STATUS_SUCCESS;
            LEAVE;
        }

        WdfStringGetUnicodeString(symbolicNameWdfString, &symbolicNameUnicodeString);

        //
        // assuming the string is \xxx\name strip of '\xxx\' where
        // x is zero or more chars
        //
        numchars = symbolicNameUnicodeString.Length/sizeof(WCHAR);
        
        pwch = &symbolicNameUnicodeString.Buffer[0];

        if (pwch == NULL || numchars == 0) {
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        //
        // Try to parse the string assuming it is of type \xxxx\... 
        // If it is not, dont generate an error (due to compat), but dont cause a 
        // buffer overrun either.
        //
        NT_ASSERT(*pwch == '\\');
        if (*pwch == '\\') {
            pwch++;
            numchars--;
            while (numchars != 0 && *pwch != '\\' && *pwch != 0) {
                numchars--;
                pwch++;
            }

            NT_ASSERT(numchars != 0 && *pwch == '\\');
            if (numchars != 0 && *pwch == '\\') {
                pwch++;
            } 
            offset = (ULONG)((PUCHAR)pwch - (PUCHAR)&symbolicNameUnicodeString.Buffer[0]);
        } 

        symbolicNameUnicodeStringStripped.Length = 
            (USHORT)(symbolicNameUnicodeString.Length - offset);
        symbolicNameUnicodeStringStripped.MaximumLength = 
            (USHORT)(symbolicNameUnicodeString.MaximumLength - offset);
        symbolicNameUnicodeStringStripped.Buffer = pwch;

        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
        objectAttributes.ParentObject = UcxRootHubContext->UcxRootHub;
        status = WdfStringCreate(&symbolicNameUnicodeStringStripped, 
                                 &objectAttributes, 
                                 &symbolicNameWdfStringStripped);

        if (!NT_SUCCESS(status)) {
            LogError(FlagUsbDevice, 
                     "WdfStringCreate failed, Str:Len-%d,MaxLen-%d,Buffer-0x%p, Status %!STATUS!",
                     symbolicNameUnicodeStringStripped.Length, 
                     symbolicNameUnicodeStringStripped.MaximumLength,
                     symbolicNameUnicodeStringStripped.Buffer,
                     status);
            NT_ASSERT(status == STATUS_INSUFFICIENT_RESOURCES);
            //
            // Modifying Status Value for Compat reasons
            //
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        status = STATUS_SUCCESS;
    
    } FINALLY {
        if (symbolicNameWdfString != NULL) {
            WdfObjectDereferenceWithTag(symbolicNameWdfString, TAG(USBPORTStyle_GetSymbolicName));
        }

        *SymbolicName = symbolicNameWdfStringStripped;
    }


    return status;
}

__drv_out(
    __drv_when(Header->UsbUserStatusCode != UsbUserBufferTooSmall,
        __drv_when(RootHubNameSize < RootHubName->Length + sizeof(ULONG),
                   __notvalid)))
VOID
USBPORTStyle_UserGetRootHubName(
    __in
        PUCXROOTHUB_PRIVCONTEXT    UcxRootHubContext,
    __in
        PUSBUSER_REQUEST_HEADER    Header,
    __in_bcount(RootHubNameSize)
        PUSB_UNICODE_NAME          RootHubName,
    __in
        ULONG                      RootHubNameSize
        
    )
/*++

Routine Description:

    Returns the Driver key associated with this symbolic link for the
    host controller.

Arguments:

    UcxRootHubContext - Pointer to the context of UCXROOTHUB object
    
    Header - The pointer to the Header of the USBUSER request
    
    RootHubName - Poiner to the output buffer where the RootHubName should be stored
        
    RootHubNameSize - Length of RootHubName buffer

Return Value:

    none.
--*/
{
    NTSTATUS                status;
    ULONG                   actualLength;
    ULONG                   adjustedRootHubNameSize;
    WDFSTRING               rootHubNameWdfString = NULL;
    UNICODE_STRING          rootHubNameUnicodeString = {0};

    TRY {
        
        if (RootHubNameSize < sizeof(USB_UNICODE_NAME)) {
            Header->UsbUserStatusCode = UsbUserBufferTooSmall;
            LEAVE;
        }
        
        //
        // COMPAT:
        // The USBPORT implementation was incorrect as it zeroes out less than it should.
        // Maintaining that behavior
        //
        adjustedRootHubNameSize = RootHubNameSize -  sizeof(USB_UNICODE_NAME);

        RtlZeroMemory(RootHubName, adjustedRootHubNameSize);
        
        status = USBPORTStyle_GetSymbolicName(UcxRootHubContext,
                                              &rootHubNameWdfString);
        
        if (NULL == rootHubNameWdfString && NT_SUCCESS(status)) {
            //
            // This case is similar to the case in USBPORT_UserGetRootHubName
            // where the USBPORT_RefRootHubPdo returned NULL.
            //
            LEAVE;
        }
        
        if (NT_SUCCESS(status)) {
            WdfStringGetUnicodeString(rootHubNameWdfString, &rootHubNameUnicodeString);
        }
        
        actualLength = 0;
        
        if (NT_SUCCESS(status)) {
            
            ULONG copySize;
            
            actualLength = rootHubNameUnicodeString.Length;
            copySize = rootHubNameUnicodeString.Length;
            
            if (copySize > adjustedRootHubNameSize) {
                copySize = adjustedRootHubNameSize;
                status = STATUS_BUFFER_TOO_SMALL;
            }
            
            if (copySize > 0) {
                RtlCopyMemory(&RootHubName->String[0],
                              rootHubNameUnicodeString.Buffer,
                              copySize);
            }
        }
        
        if (NT_SUCCESS(status)) {
            Header->UsbUserStatusCode = UsbUserSuccess;
            RootHubName->Length = actualLength + sizeof(UNICODE_NULL);
        } else if (status == STATUS_BUFFER_TOO_SMALL) {
            Header->UsbUserStatusCode = UsbUserBufferTooSmall;
        } else {
            Header->UsbUserStatusCode = UsbUserInvalidParameter;
        }
        
        Header->ActualBufferLength = actualLength+sizeof(*Header)+sizeof(USB_UNICODE_NAME);
        
        
    } FINALLY {
        
        if (NULL != rootHubNameWdfString) {
            WdfObjectDelete(rootHubNameWdfString);
        }
    
    }
}

NTSTATUS
FORCEINLINE
USBPORTStyle_UsbAllocPool_Z(
    __deref_out_bcount(NumberOfBytes) 
        PVOID     *Ptr,
    __in  
        POOL_TYPE  PoolType,
    __in 
        ULONG      NumberOfBytes
    )
/*++

Routine Description:

    COMPAT: 
    This function is based on USBPORT_UsbAllocPool_Z
    
    This function allocates a buffer and Securely zero'es out the contents.

Arguments:

    Ptr - Output - Pointer to the Allocated buffer
    
    PoolType - PoolType (as described by ExAllocatePool standard API)
    
    NumberOfBytes - Number of bytes to allocate
    
Return Value:

    NTSTATUS
--*/
{
    PVOID    buffer;
    NTSTATUS status;

    TRY {
        if (Ptr == NULL) {
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        *Ptr = NULL;
        NT_ASSERT(NumberOfBytes != 0);
        
        if (NumberOfBytes == 0) {
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        buffer = ExAllocatePoolWithTag(PoolType, NumberOfBytes, UCX_TAG);

        if (NULL == buffer) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }
            
        RtlZeroMemory(buffer, NumberOfBytes);
        
        *Ptr = buffer;
        
        status = STATUS_SUCCESS;

    } FINALLY {
    }

    return status;
}

NTSTATUS
USBPORTStyle_LegacyGetUnicodeName(
    __in
        WDFDEVICE                       Device,
    __in 
        WDFREQUEST                      Request,
    __in 
        size_t                          OutputBufferLength,
    __in  
        size_t                          InputBufferLength,
    __in
        ULONG                           IoControlCode,
    __out
        ULONG                          *BytesReturned
    )
/*++

Routine Description:

    COMPAT:
    This function is based off USBPORT_LegacyGetUnicodeName.
    This function could be simplified quite a bit, however not touching it due to compat. 
    
    Handles the old style IOCTL to fetch the USB host controllers
    driver key name.


Arguments:

    Device - WDFDEVICE for which the ioctl was recieved.
    
    Request - A handle to a framework request object (representing the User mode ioctl).

    OutputBufferLength - The length, in bytes, of the request's output buffer, if an output buffer 
        is available.

    InputBufferLength - The length, in bytes, of the request's input buffer, if an input buffer 
        is available.

    IoControlCode - The driver-defined or system-defined I/O control code (IOCTL) that is 
        associated with the request.
        
    
    BytesRetrned - Number of bytes being returned in the buffer.

Return Value:

    NTSTATUS

--*/
{
    
    PUSBUSER_CONTROLLER_UNICODE_NAME  request = NULL;
    NTSTATUS                          status;
    ULONG                             need;
    PUSB_HCD_DRIVERKEY_NAME           ioBufferO;
    PUCXCONTROLLER_PRIVCONTEXT        ucxControllerContext;
    PWDFDEVICE_UCX_CONTEXT            wdfDeviceUcxContext;
    LONG                              numTries;

    UNREFERENCED_PARAMETER(InputBufferLength);

    TRY {

        NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
        *BytesReturned = 0;
        
        wdfDeviceUcxContext = WdfDeviceGetUcxContext(Device);
        ucxControllerContext = wdfDeviceUcxContext->UcxControllerContext;

        // Bail immediately if the output buffer is too small
        //
        if (OutputBufferLength < sizeof(USB_HCD_DRIVERKEY_NAME)) {
            status = STATUS_BUFFER_TOO_SMALL;
            LEAVE;
        }

        status = WdfRequestRetrieveOutputBuffer(Request,
                                                OutputBufferLength,
                                                &ioBufferO,
                                                NULL);
                
        if (!NT_SUCCESS(status)) {
            LogError(FlagUsbDevice, 
                     "WdfRequestRetrieveOutputBuffer Failed %!STATUS! (changing status to STATUS_BUFFER_TOO_SMALL)", 
                     status);
            //
            // Change status for compat
            //
            status = STATUS_BUFFER_TOO_SMALL;
            LEAVE;
        }

        // first get the driver key name using the standard API

        need = sizeof(USBUSER_CONTROLLER_UNICODE_NAME);

        //
        // Initialize numTries to a reasonable number to prevent a 
        // bugcheck. Ideally the loop should never run for more than twice.
        //
        numTries = 5;
        while(numTries > 0) {
            
            numTries--;

            USBPORTStyle_UsbAllocPool_Z(&request, PagedPool, need);

            if (NULL == request) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }

            request->Header.RequestBufferLength = need;
            if (IoControlCode == IOCTL_GET_HCD_DRIVERKEY_NAME) {
                request->Header.UsbUserRequest = USBUSER_GET_CONTROLLER_DRIVER_KEY;
                USBPORTStyle_UserGetControllerKey(Device,
                                                  &request->Header,
                                                  &request->UnicodeName,
                                                  need - sizeof(USBUSER_REQUEST_HEADER));
            } else {
                NT_ASSERT(IoControlCode == IOCTL_USB_GET_ROOT_HUB_NAME);
                request->Header.UsbUserRequest = USBUSER_GET_ROOTHUB_SYMBOLIC_NAME;
                USBPORTStyle_UserGetRootHubName(ucxControllerContext->UcxRootHubContext,
                                                &request->Header,
                                                &request->UnicodeName,
                                                need - sizeof(USBUSER_REQUEST_HEADER));
            }
            __assume(request->UnicodeName.Length <= need 
                                                    - sizeof(USBUSER_REQUEST_HEADER) 
                                                    - sizeof(ULONG));

            if (request->Header.UsbUserStatusCode == UsbUserBufferTooSmall) {

                need = request->Header.ActualBufferLength;

                ExFreePoolWithTag(request, UCX_TAG);
                request = NULL;
                continue;

            }

            break;

        }
            
        if (request->Header.UsbUserStatusCode != UsbUserSuccess) {

            status = STATUS_UNSUCCESSFUL;
            LEAVE;

        }

            
        // map the result into the callers buffer
        
        // note: actual length is the size of the request structure
        // plus the name.
        
        status = RtlULongAdd(request->UnicodeName.Length,
                             sizeof(ULONG),
                             &ioBufferO->ActualLength);
        if (OutputBufferLength >= ioBufferO->ActualLength
            && NT_SUCCESS(status)) {
            
            // we can return the name
            RtlCopyMemory(&ioBufferO->DriverKeyName[0],
                          &request->UnicodeName.String[0],
                          request->UnicodeName.Length);
            
            *BytesReturned = ioBufferO->ActualLength;
            
        } else {
            ioBufferO->DriverKeyName[0] =  L'\0';
            *BytesReturned = sizeof(USB_HCD_DRIVERKEY_NAME);
        }
        
        status = STATUS_SUCCESS;
        
    } FINALLY {
        
        if (request != NULL) {
            ExFreePoolWithTag(request, UCX_TAG);
        }

    }
    

    return status;
}

//
// USBPORT_TRACKING_ID is definded by USBPORT
//
#define USBPORT_TRACKING_ID              3
VOID
USBPORTStyle_UserGetDriverVersion(
    __in
        PUSBUSER_REQUEST_HEADER        Header,
    __in
        PUSB_DRIVER_VERSION_PARAMETERS Parameters
    )
/*++

Routine Description:

    COMPAT: This routine is based off USBPORT_UserGetDriverVersion
    
    This routine fills in the driver information in Parameters.

Arguments:

    Header - The pointer to the Header of the USBUSER request
    
    Parameters - Pointer to Params structure.

Return Value:

    none.

--*/
{
    TRY {

        //
        // Increment the value usbport reported.
        //
        Parameters->DriverTrackingCode = USBPORT_TRACKING_ID+1;
        
        //
        // USBDI Api set supported 
        //
        Parameters->USBDI_Version = USBDI_VERSION;
        
        //
        // USB USER Api Set supported
        //
        Parameters->USBUSER_Version = USBUSER_VERSION;

        //
        // set to true if checked vesrion(s) on the stack are loaded
        //           
        #if DBG
        Parameters->CheckedPortDriver = TRUE;
        Parameters->CheckedMiniportDriver = TRUE;
        #else
        Parameters->CheckedPortDriver = FALSE;
        Parameters->CheckedMiniportDriver = FALSE;
        #endif

        Header->ActualBufferLength = sizeof(*Header)+sizeof(USB_DRIVER_VERSION_PARAMETERS);
    
    } FINALLY {

    }
}


NTSTATUS
USBPORTStyle_UsbFdoUserIoctl(
    __in
        WDFDEVICE                       Device,
    __in 
        WDFREQUEST                      Request,
    __in 
        size_t                          OutputBufferLength,
    __in  
        size_t                          InputBufferLength,
    __out
        ULONG                          *BytesReturned
    )
/*++

Routine Description:

    COMPAT: 
    This routine tries to mimic the behavior of USBPORT_UsbFdoUserIoctl routine as much as 
    possible.
    
    Following are the comments from the USBPORT_UsbFdoUserIoctl documentation:
    
    The goal here is to have all user mode APIS
    pass thru this routine so that the parameter
    validation is handled in one place.

    We define user APIS supported by the PORT FDO
    thru this single IOCTL.

    The USUSER APIs use the same buffer for input and output,
    hence the InputBufferLength and the OutputbufferLength must
    always be equal.


    We get here if the client sends the IOCTL_USB_USER_REQUEST.
    We only return NTSTATUS failure if the header portion of the
    buffer is invalid.



Arguments:

    Device - WDFDEVICE for which the ioctl was recieved.
    
    Request - WdfRequest for the USERUSER ioctl

    OutputBufferLength - Length of output Buffer
    
    InputBufferLength - Length of Input Buffer 
    
    BytesRetrned - Number of bytes being returned in the buffer.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS                          status;
    PUSBUSER_REQUEST_HEADER           header;
    PUCHAR                            ioBufferO;
    PUCXCONTROLLER_PRIVCONTEXT        ucxControllerContext;
    PWDFDEVICE_UCX_CONTEXT            wdfDeviceUcxContext;
    TRY {
        
        wdfDeviceUcxContext = WdfDeviceGetUcxContext(Device);
        ucxControllerContext = wdfDeviceUcxContext->UcxControllerContext;
        
        //
        // parameter validation
        //
        if (InputBufferLength != OutputBufferLength) {
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }


        status = WdfRequestRetrieveOutputBuffer(Request,
                                                sizeof(USBUSER_REQUEST_HEADER),
                                                &ioBufferO,
                                                NULL);

        if (!NT_SUCCESS(status)) {
            LogError(FlagUsbDevice, 
                     "WdfRequestRetrieveOutputBuffer Failed %!STATUS! (changing status to STATUS_BUFFER_TOO_SMALL)", 
                     status);
            //
            // Change status for compat
            //
            status = STATUS_BUFFER_TOO_SMALL;
            LEAVE;
        }

        //
        // validate the header ioBufferO parameters
        //
        header = (PUSBUSER_REQUEST_HEADER) ioBufferO;

        //
        // assume success, set return values
        //
        header->UsbUserStatusCode = UsbUserSuccess;
        
        header->ActualBufferLength = sizeof(*header);
        *BytesReturned = header->ActualBufferLength;

        //
        // length set in header should be the same
        // as length passed in the ioctl
        //
        if (header->RequestBufferLength != OutputBufferLength) {
            LogError(FlagUsbDevice,
                     "header->RequestBufferLength(%d) != OutputBufferLength(%d)",
                     header->RequestBufferLength, (ULONG)OutputBufferLength);
            header->UsbUserStatusCode = UsbUserInvalidHeaderParameter;
            LEAVE;
        }

        //
        // we have a valid header and client buffer, attempt to execute
        // the api
        //

        switch (header->UsbUserRequest) {
        
        case USBUSER_OP_SEND_ONE_PACKET:
            if (FALSE == UsbUserIoctl_CheckAndSetParamLength(header, sizeof(PACKET_PARAMETERS))){
                LEAVE;
            }
            
            //
            // 2.0 Stack Rely's on DcaEnable registry key. Thus no need to support on 
            // the current stack
            //
            header->UsbUserStatusCode = UsbUserInvalidHeaderParameter;
            break;

        case USBUSER_OP_RAW_RESET_PORT:
            if (FALSE == UsbUserIoctl_CheckAndSetParamLength(header, 
                                                             sizeof(RAW_RESET_PORT_PARAMETERS))) {
                LEAVE;
            }
            
            //
            // 2.0 stack Rely's on DcaEnable registry key. Thus no need to support on 
            // the current stack
            //
            header->UsbUserStatusCode = UsbUserFeatureDisabled;
            break;
        case USBUSER_SET_ROOTPORT_FEATURE:
        case USBUSER_CLEAR_ROOTPORT_FEATURE:
        case USBUSER_GET_ROOTPORT_STATUS:
            if (FALSE == UsbUserIoctl_CheckAndSetParamLength(header, 
                                                             sizeof(RAW_ROOTPORT_FEATURE))) {
                LEAVE;
            }
            
            //
            // 2.0 stack Rely's on DcaEnable registry key. Thus no need to support on 
            // the current stack
            //
            header->UsbUserStatusCode = UsbUserFeatureDisabled;
            break;

        case USBUSER_GET_CONTROLLER_INFO_0:
            if (FALSE == UsbUserIoctl_CheckAndSetParamLength(header, 
                                                             sizeof(USB_CONTROLLER_INFO_0))){
                LEAVE;
            }
            
            USBPORTStyle_UserGetControllerInfo_0(ucxControllerContext, 
                                                 header,
                                                 (PUSB_CONTROLLER_INFO_0)(ioBufferO+sizeof(*header)));
            break;

        case USBUSER_GET_CONTROLLER_DRIVER_KEY:
            if (FALSE == UsbUserIoctl_CheckAndSetParamLength(header, sizeof(USB_UNICODE_NAME))){
                LEAVE;
            }
            
            USBPORTStyle_UserGetControllerKey(Device,
                                              header,
                                              (PUSB_UNICODE_NAME)(ioBufferO+sizeof(*header)),
                                              ((ULONG)OutputBufferLength) - sizeof(*header));
            break;

        case USBUSER_GET_ROOTHUB_SYMBOLIC_NAME:
            if (FALSE == UsbUserIoctl_CheckAndSetParamLength(header, sizeof(USB_UNICODE_NAME))){
                LEAVE;
            }
            
            USBPORTStyle_UserGetRootHubName(ucxControllerContext->UcxRootHubContext, 
                                            header,
                                            (PUSB_UNICODE_NAME) (ioBufferO+sizeof(*header)),
                                            ((ULONG)OutputBufferLength) - sizeof(*header));
            break;

        case USBUSER_PASS_THRU:
            if (FALSE == UsbUserIoctl_CheckAndSetParamLength(header, 
                                                             sizeof(USB_PASS_THRU_PARAMETERS))){
                LEAVE;
            } 
            
            USBPORTStyle_UserPassThru(header, 
                                      (PUSB_PASS_THRU_PARAMETERS) (ioBufferO+sizeof(*header)));
            break;

        case USBUSER_GET_BANDWIDTH_INFORMATION:
            if (FALSE == UsbUserIoctl_CheckAndSetParamLength(header, sizeof(USB_BANDWIDTH_INFO))){
                LEAVE;
            }

            USBPORTStyle_UserGetBandwidthInformation(header,
                                                     (PUSB_BANDWIDTH_INFO)(ioBufferO+sizeof(*header)));
            break;

        case USBUSER_GET_POWER_STATE_MAP:
            if (FALSE == UsbUserIoctl_CheckAndSetParamLength(header, sizeof(USB_POWER_INFO))) {
                LEAVE;
            }
            
            USBPORTStyle_UserPowerInformation(ucxControllerContext, 
                                              header,
                                              (PUSB_POWER_INFO)(ioBufferO+sizeof(*header)));
            break;

        case USBUSER_GET_BUS_STATISTICS_0:
            if (FALSE == UsbUserIoctl_CheckAndSetParamLength(header, sizeof(USB_BUS_STATISTICS_0))){
                LEAVE;
            }
            
            USBPORTStyle_UserGetBusStatistics0(ucxControllerContext,
                                               header,
                                               (PUSB_BUS_STATISTICS_0)(ioBufferO+sizeof(*header)));
            break;

        case USBUSER_USB_REFRESH_HCT_REG:
            if (FALSE == UsbUserIoctl_CheckAndSetParamLength(header, sizeof(ULONG))) {
                LEAVE;
            }
            
            header->UsbUserStatusCode = UsbUserSuccess;
            break;

        case USBUSER_GET_USB_DRIVER_VERSION:
            if (FALSE == 
                UsbUserIoctl_CheckAndSetParamLength(header, sizeof(USB_DRIVER_VERSION_PARAMETERS))){
                LEAVE;
            }
            
            USBPORTStyle_UserGetDriverVersion(header, 
                                              (PUSB_DRIVER_VERSION_PARAMETERS) 
                                                  (ioBufferO+sizeof(*header)));
            break;

        default:
            header->UsbUserStatusCode = UsbUserInvalidRequestCode;
            break;
        }

        if (header->RequestBufferLength > header->ActualBufferLength) {
            //
            // if the packet buffer is larger then just return 'actual length'
            //
            *BytesReturned = header->ActualBufferLength;
        } else {
            //
            // packet buffer is smaller -- return the size of the
            // packet buffer passed in
            //
            *BytesReturned = header->RequestBufferLength;
        }

    } FINALLY {

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
_Must_inspect_result_
BOOLEAN
EXPORT(UcxIoDeviceControl)(
    __in_opt 
        PUCX_GLOBALS                    Globals,
    __in
        WDFDEVICE                       Device,
    __in 
        WDFREQUEST                      Request,
    __in 
        size_t                          OutputBufferLength,
    __in  
        size_t                          InputBufferLength,
    __in
        ULONG                           IoControlCode
    )
/*++

Routine Description:

    This function is called by HCD to inform us about a User Mode ioctl.
    
    TODO: PostWin8: Once we have WDF functionality that allows UCX to set a pre-processor routine
        we should consider using that.
        
Arguments:

    Request - A handle to a framework request object (representing the User mode ioctl).

    OutputBufferLength - The length, in bytes, of the request's output buffer, if an output buffer 
        is available.

    InputBufferLength - The length, in bytes, of the request's input buffer, if an input buffer 
        is available.

    IoControlCode - The driver-defined or system-defined I/O control code (IOCTL) that is 
        associated with the request.
        
Return Value: 
    
    BOOLEAN - Returns True if UCX took ownership of the WDFREQUEST, False otherwise
    
--*/
{
    BOOLEAN                           knownIoctl;
    NTSTATUS                          status;
    ULONG                             bytesReturned;

    UNREFERENCED_PARAMETER(Globals);
    TRY {

        knownIoctl = TRUE;
        switch (IoControlCode) {
        case IOCTL_USB_DIAGNOSTIC_MODE_OFF:
        case IOCTL_USB_DIAGNOSTIC_MODE_ON: 
            //
            // Fake Successful Completion
            //
            WdfRequestComplete(Request, STATUS_SUCCESS);
            break;
        
        case IOCTL_USB_GET_ROOT_HUB_NAME:
        case IOCTL_GET_HCD_DRIVERKEY_NAME:
            status = USBPORTStyle_LegacyGetUnicodeName(Device,
                                                       Request,
                                                       OutputBufferLength,
                                                       InputBufferLength,
                                                       IoControlCode,
                                                       &bytesReturned);

            WdfRequestCompleteWithInformation(Request, status, (ULONG_PTR) bytesReturned);
            
            break;
        
        
        case IOCTL_USB_USER_REQUEST: 
             
            bytesReturned = 0;
            status = USBPORTStyle_UsbFdoUserIoctl(Device, 
                                                  Request, 
                                                  OutputBufferLength, 
                                                  InputBufferLength,
                                                  &bytesReturned);
            
            WdfRequestCompleteWithInformation(Request, status, (ULONG_PTR) bytesReturned);
                                         
            break;
        
        default: 
            knownIoctl = FALSE;
            LEAVE;
        }

    } FINALLY {

    }
    
    return knownIoctl;
}

