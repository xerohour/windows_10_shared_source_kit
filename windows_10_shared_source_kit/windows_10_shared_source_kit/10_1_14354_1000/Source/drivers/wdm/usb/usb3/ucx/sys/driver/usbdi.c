//
//    Copyright (C) Microsoft.  All rights reserved.
//
#include "pch.h"
#include "usbdi.tmh"

BOOLEAN
Usbd_IsDeviceAtleastHighSpeed(
    __in 
        PVOID Context
    )
/*++

Routine Description:

    This routine is there for compatibility with the 2.0 stack.
    This routine will return false true if it is a LS / FS device, and true if it is a 
    HighSpeed / SuperSpeed device.

Arguments:

    Context - ucxUsbDevice

Return Value:

    False if it is LS / FS
    
--*/
{
    UCXUSBDEVICE              ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT ucxUsbDeviceContext;
    USB_DEVICE_SPEED          usbDeviceSpeed;
    BOOLEAN                   result;

    TRY {
        //
        // Normally we wouldnt want to check for NULL, however the 2.0 stack does that. 
        // So checking to maintain compat
        //
        if (Context == NULL) {
            result = FALSE;
            LEAVE;
        }

        ucxUsbDevice = (UCXUSBDEVICE) Context;
        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);

        usbDeviceSpeed = ucxUsbDeviceContext->UcxUsbDeviceInfo.DeviceSpeed;

        if (usbDeviceSpeed == UsbLowSpeed || usbDeviceSpeed == UsbFullSpeed) {
            result = FALSE;
            LEAVE;            
        }

        result = TRUE;

    } FINALLY {

    }

    return result;
    
}


NTSTATUS
Usbd_BusQueryBusInformation(
    __in      
        PVOID       Context,
    __in      
        ULONG       Level,
    __in      
        PVOID       BusInformationBuffer,
    __inout   
        PULONG      BusInformationBufferLength,
    __out_opt 
        PULONG      BusInformationActualLength
    )
/*++

Routine Description:

    This routine is there for compatibility with the 2.0 stack.
    
    This routine returns information such as bandwidth, total bandwidth, symbolic name.
    Some of that information has to be faked.

Arguments:

    Context - UcxUsbDevice
    
    Level - A level value describing the information that is being requested.
    
    BusInformationBuffer - A caller allocated buffer to accept the 
        bus information.
    
    BusInformationBufferLength - Size of BusInformationBuffer in bytes.
    
    BusInformationActualLength - Output - Size of info filled / size of buffer needed to 
        successfully complete this request.

Return Value:

    NT status code.
    
Comments: 
    On the New 3.0 stack (especially XHCI), it is not always possible to get 
    information like TotalBanwidth, ConsumedBandwidth. 
    
    However for supporting existing client drivers we do need to provide some 
    fake values here, if the HCD does not support this feature. 
    
--*/
{
    NTSTATUS                          status;
    UCXUSBDEVICE                      ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT         ucxUsbDeviceContext;
    PUCXCONTROLLER_PRIVCONTEXT        ucxControllerContext;
    ULONG                             totalBandwidth;
    ULONG                             consumedBandwidth;
    ULONG                             bufferLength;
    UNICODE_STRING                    symbolicLinkName;    
    PUSB_BUS_INFORMATION_LEVEL_0      level_0;
    PUSB_BUS_INFORMATION_LEVEL_1      level_1;
    USB_DEVICE_SPEED                  usbDeviceSpeed;

    TRY {
        ucxUsbDevice = (UCXUSBDEVICE) Context;
        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
        ucxControllerContext = ucxUsbDeviceContext->UcxControllerContext;
        
        usbDeviceSpeed = ucxUsbDeviceContext->UcxUsbDeviceInfo.DeviceSpeed;

        if (usbDeviceSpeed != UsbLowSpeed 
            && usbDeviceSpeed != UsbFullSpeed
            && usbDeviceSpeed != UsbHighSpeed) {
            
            LogError(FlagUsbdi, "Request valid only for <= 2x devices. Ignoring due to compat");
        
        }
    
        if (ucxControllerContext->Config.EvtControllerGetBandwidthInformation == NULL) {
        
            //
            // Returning fake madeup values. 
            // Let's say we have 4 Mb/sec  
            //  
            totalBandwidth = 4 * 1000 * 1000;
            consumedBandwidth = 0;

        } else {

            LogError(FlagUsbdi, "UCX does not support EvtControllerGetBandwidthInformation yet");
            
            NT_ASSERTMSG("UCX does not support EvtControllerGetBandwidthInformation yet", 0);
            NT_ASSERTMSG("Since this is an Asynchronous call to the HCD, this should happen on PASSIVE LEVEL", 
                         KeGetCurrentIrql() == PASSIVE_LEVEL);

            status = STATUS_NOT_SUPPORTED;
            LEAVE;
        }

        switch (Level) {
        
        case 0: 

            level_0 =  BusInformationBuffer;
            
            LogVerbose(FlagUsbdi, "Level 0 BusInformation Requested for ucxusbdevice 0x%p", ucxUsbDevice);

            if (BusInformationActualLength != NULL) {
                *BusInformationActualLength = sizeof(*level_0);
            }
            
            if (*BusInformationBufferLength < sizeof(*level_0)) {
                status = STATUS_BUFFER_TOO_SMALL;
                LogWarning(FlagUsbdi, "Buffer Too Small, Actual %d, Expected %d",
                         *BusInformationBufferLength, sizeof(*level_0));
                LEAVE;
            }

            *BusInformationBufferLength = sizeof(*level_0);
            
            level_0->TotalBandwidth = totalBandwidth;  
            level_0->ConsumedBandwidth = consumedBandwidth;  
            
            status = STATUS_SUCCESS;
                
            break;
        
        case 1: 
            
            level_1 =  BusInformationBuffer;
            
            LogVerbose(FlagUsbdi, "Level 1 BusInformation Requested for ucxusbdevice 0x%p", ucxUsbDevice);
            
            WdfStringGetUnicodeString(ucxControllerContext->SymbolicNameForHostControllerInterface,
                                      &symbolicLinkName);
            
            bufferLength = sizeof(*level_1) + symbolicLinkName.Length;
            
            if (BusInformationActualLength != NULL) {
                *BusInformationActualLength = bufferLength;
            }
            
            
            if (*BusInformationBufferLength < bufferLength) {
                status = STATUS_BUFFER_TOO_SMALL;
                LogWarning(FlagUsbdi, "Buffer Too Small, Actual %d, Expected %d",
                         *BusInformationBufferLength, sizeof(*level_1) + symbolicLinkName.Length);
                LEAVE;
            }

            *BusInformationBufferLength = bufferLength;
            
            level_1->TotalBandwidth = totalBandwidth;
            level_1->ConsumedBandwidth = consumedBandwidth;
            
            
            level_1->ControllerNameLength = symbolicLinkName.Length;
            
            
            RtlCopyMemory(level_1->ControllerNameUnicodeString,
                          symbolicLinkName.Buffer,
                          level_1->ControllerNameLength);
            
            status = STATUS_SUCCESS;
            break;

        default: 
            status = STATUS_NOT_SUPPORTED;
            break;
        }
    } FINALLY {

    }
    return status;
}


VOID
Usbd_BusGetUSBDIVersion(
    __in_opt 
        PVOID                         Context,
    __out_opt 
        PUSBD_VERSION_INFORMATION     VersionInformation,
    __out_opt 
        PULONG                        HcdCapabilities
    )
/*++

Routine Description:

    returns USBDI and USB version information. 

Arguments:

    Context - UcxUsbDevice
    
    VersionInformation - optional caller allocated buffer to be filled in with
        appropraite version information

    HcdCapabilities - optional pointer to ulong that is supposed to set 
        the capailities flags for the host controller where the device resides.
        This is always set to 0 (to maintain compat with 2.0 stack) 
    
Return Value:

    NONE.

--*/
{
    UCXUSBDEVICE              ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT ucxUsbDeviceContext;
    USB_DEVICE_SPEED          usbDeviceSpeed;

    TRY {
        
        ucxUsbDevice = (UCXUSBDEVICE) Context;
        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
        usbDeviceSpeed = ucxUsbDeviceContext->UcxUsbDeviceInfo.DeviceSpeed;
        
        if (HcdCapabilities != NULL) {
            *HcdCapabilities = 0;
        }        
        
        if (VersionInformation != NULL) {
            
            VersionInformation->USBDI_Version = USBDI_VERSION;

            if (usbDeviceSpeed == UsbLowSpeed || usbDeviceSpeed == UsbFullSpeed) {

                VersionInformation->Supported_USB_Version = USB_VERSION_11;
            
            } else {
                
                VersionInformation->Supported_USB_Version = USB_VERSION_20;

            }

        }
    
    } FINALLY {
    }

}


NTSTATUS
Usbd_BusQueryBusTime(
    __in
        PVOID     Context,
    __out_opt 
        PULONG    CurrentFrame
    )
/*++

Routine Description:

    Returns the value of the current USB frame counter or 'clock'.

Arguments:

    Context - UcxUsbDevice

    CurrentFrame - pointer to receive the current frame number.

Return Value:

    NT status code.

--*/
{
    NTSTATUS                   status;
    UCXUSBDEVICE               ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT  ucxUsbDeviceContext;
    PUCXCONTROLLER_PRIVCONTEXT ucxControllerContext;

    TRY {
        
        if (CurrentFrame == NULL) {
            status = STATUS_INVALID_PARAMETER;
            LogWarning(FlagUsbdi, "Invalid Parameter");
            LEAVE;
        } 
        
        ucxUsbDevice = (UCXUSBDEVICE) Context;
        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
        ucxControllerContext = ucxUsbDeviceContext->UcxControllerContext;
        
        status = Controller_GetCurrentFrameNumber(ucxControllerContext, CurrentFrame);

    } FINALLY {

    }
 
    return status;
}

NTSTATUS
Usbd_BusQueryBusTimeEx(
    __in_opt 
        PVOID     Context,
    __out_opt 
        PULONG    CurrentMicroFrame
    )
/*++

Routine Description:

    Returns the value of the current USB frame counter or 'clock'. 
    This routine returns the microframe count in the lowest three bits.  
    If called on a full speed bus the API returns STATUS_NOT_SUPPORTED.

Arguments:

    Context - UcxUsbDevice

    CurrentMicroFrame - pointer to receive the current frame number.

Return Value:

    NT status code.

--*/
{

    NTSTATUS                   status;
    UCXUSBDEVICE               ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT  ucxUsbDeviceContext;
    PUCXCONTROLLER_PRIVCONTEXT ucxControllerContext;
    USB_DEVICE_SPEED           usbDeviceSpeed;

    TRY {
        
        if (CurrentMicroFrame == NULL) {
            status = STATUS_INVALID_PARAMETER;
            LogWarning(FlagUsbdi, "Invalid Param");
            LEAVE;
        } 
        
        ucxUsbDevice = (UCXUSBDEVICE) Context;
        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
        ucxControllerContext = ucxUsbDeviceContext->UcxControllerContext;

        usbDeviceSpeed = ucxUsbDeviceContext->UcxUsbDeviceInfo.DeviceSpeed;

        if (usbDeviceSpeed == UsbLowSpeed || usbDeviceSpeed == UsbFullSpeed) {
            
            status = STATUS_NOT_SUPPORTED;
            LogWarning(FlagUsbdi, "Usbd_BusQueryBusTimeEx not supported for 1.1 devices");
            LEAVE;
        }

        //
        // The Current Microframe number was never implemented in 2.0 stack. 
        //
        *CurrentMicroFrame = 0;
        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}


NTSTATUS
Usbd_QueryControllerType(
    __in_opt 
        PVOID     Context,
    __out_opt 
        PULONG    HcdiOptionFlags,
    __out_opt 
        PUSHORT   PciVendorId,
    __out_opt 
        PUSHORT   PciDeviceId,
    __out_opt 
        PUCHAR    PciClass,
    __out_opt 
        PUCHAR    PciSubClass,
    __out_opt 
        PUCHAR    PciRevisionId,
    __out_opt 
        PUCHAR    PciProgIf
    )
/*++

Routine Description:

    Returns information about the controller this device is connected to.
   
Arguments:

    Context - bus context for a USB device.

    HcdiOptionFlags - miniport option flags set when controller registers with usbport.

    PciVendorId - pointer to receive the pci info for controller.

    PciDeviceId - 

    PciClass - typically PCI_CLASS_SERIAL_BUS_CTLR

    PciSubClass - typically PCI_SUBCLASS_SB_USB

    PciRevisionId -

    PciProgIf - 0x00 = UHCI, 0x10 = OHCI, 0x20 EHCI.
   
   
Return Value:

    NT status code.

--*/
{
    NTSTATUS                  status;
    UCXUSBDEVICE              ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT ucxUsbDeviceContext;
    ULONG                     tmpHcdiOptionFlags;
    UCHAR                     tmpPciClass;
    UCHAR                     tmpPciSubClass;
    UCHAR                     tmpPciProgIf;
    USB_DEVICE_SPEED          usbDeviceSpeed;

    TRY {
        
        ucxUsbDevice = (UCXUSBDEVICE) Context;
        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
    
        usbDeviceSpeed = ucxUsbDeviceContext->UcxUsbDeviceInfo.DeviceSpeed;
        
        if (usbDeviceSpeed == UsbLowSpeed || usbDeviceSpeed == UsbFullSpeed) {
            // 
            // COMPAT: Using UHCI values here. Could also have used OHCI values
            //
            
            //
            // From the UHCI spec
            //
            tmpPciProgIf = 0x0;
            //
            // From UHCI miniport. 
            // RegistrationPacket.OptionFlags = USB_MINIPORT_OPT_NEED_IRQ |
            //                                  USB_MINIPORT_OPT_NEED_IOPORT |
            //                                  USB_MINIPORT_OPT_NO_IRQ_SYNC |
            //                                  USB_MINIPORT_OPT_POLL_IN_SUSPEND |
            //                                  USB_MINIPORT_OPT_POLL_CONTROLLER;
            //
            tmpHcdiOptionFlags = 0x2c3;
        } else {
            //
            // From the EHCI spec
            //
            tmpPciProgIf = 0x20;
            
            //
            // From EHCI Miniport Code
            // RegistrationPacket.OptionFlags = USB_MINIPORT_OPT_NEED_IRQ |
            //                                  USB_MINIPORT_OPT_NEED_MEMORY |
            //                                  USB_MINIPORT_OPT_USB20 |
            //                                  USB_MINIPORT_OPT_POLL_IN_SUSPEND |
            //                                  USB_MINIPORT_OPT_POLL_CONTROLLER |
            //                                  USB_MINIPORT_OPT_SUSPEND_DISABLE_IRQ;
            //
            tmpHcdiOptionFlags = 0x80a95;
        }

        //
        // From EHCI, OHCI spec.
        //
        tmpPciClass = 0x0C;
        tmpPciSubClass = 0x3;

        if (HcdiOptionFlags != NULL) {
            *HcdiOptionFlags = tmpHcdiOptionFlags;
        } 

        if (PciVendorId != NULL) {
            *PciVendorId =
                (USHORT) ucxUsbDeviceContext->UcxControllerContext->Config.PciDeviceInfo.VendorId;
        }

        if (PciDeviceId != NULL) {
            *PciDeviceId =
                (USHORT) ucxUsbDeviceContext->UcxControllerContext->Config.PciDeviceInfo.DeviceId;
        }

        if (PciClass != NULL) {
            *PciClass = tmpPciClass;                
        }       

        if (PciSubClass != NULL) {
            *PciSubClass = tmpPciSubClass;
        }

        if (PciRevisionId != NULL) {
            *PciRevisionId =
                (UCHAR) ucxUsbDeviceContext->UcxControllerContext->Config.PciDeviceInfo.RevisionId;
        }

        if (PciProgIf != NULL) {
            *PciProgIf = tmpPciProgIf;
        }            

        status = STATUS_SUCCESS;

    } FINALLY {

    }
    
    return status;
}

NTSTATUS
Usbd_BusEnumLogEntry(
    PVOID Context,
    ULONG DriverTag,
    ULONG EnumTag,
    ULONG P1,
    ULONG P2
    )
/*++

Routine Description:

    Depricated. Just return status for Compat purposes.

Return Value:

    STATUS_SUCCESS for compat

--*/
{

    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(DriverTag);
    UNREFERENCED_PARAMETER(EnumTag);
    UNREFERENCED_PARAMETER(P1);
    UNREFERENCED_PARAMETER(P2);
    
    return STATUS_SUCCESS;
}


NTSTATUS
Usbd_BusSubmitIsoOutUrb(
    PVOID Context,
    PURB  Urb
    )
/*++

Routine Description:

    Depricated. Function exists for compat purposes only

Return Value:

    STATUS_NOT_SUPPORTED for compat.

--*/
{

    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(Urb);
    NT_ASSERTMSG("Usbd_BusSubmitIsoOutUrb is Depricated", 0);
    return STATUS_NOT_SUPPORTED;
}


