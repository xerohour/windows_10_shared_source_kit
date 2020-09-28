/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efiusbfn.c

Abstract:

    Contains USB functions consumed directly by other subsystems of the Boot
    Charging application
    
Environment:

    Boot

--*/

#include "usbfn.h"
#include <UsbFnIo.h>
#include "usbfnsupport.h"
#include "efiwrappers.h"

//
// Guid to the UsbFn Protocol
//
EFI_GUID EfiUsbProtocolGuid = EFI_USBFN_IO_PROTOCOL_GUID;

//
// IO handle
//
EFI_HANDLE *ClUsbFnIoHandle;

//
// Global pointer to the UsbFn Protocol
//
EFI_USBFN_IO_PROTOCOL            *EfiUsbProtocol;

//
// Maximum transfer size on the Usb bus
//
UINTN cMaxTransferSize = EFI_USB_MAXIMUM_PACKET_SIZE;

//
// Boot Battery Charging App's Usb device descriptor
//
extern EFI_USB_DEVICE_INFO EfiUsbDeviceInfo;

//
// Global to keep track of initialization status of the protocol
//
extern BOOLEAN UsbInit;

//
// Externed current value. DetectPort is used to set this value
//
extern UINT32 ChargeCurrent;


extern BOOLEAN                      cConnectionEstablished;


NTSTATUS UsbFnProtocolInit()
/*++

Routine Description:

    Initialize the UsbFn protocol. This function also calls 
    ConfigureEnableEndpoints and so, it is important to enumerate the device
    soon after the call to this routine, as defined in the USB 2.0 spec

Arguments:

    None
    
Return Value:

    NT status code.

--*/
{

    EFI_STATUS Status = EFI_SUCCESS;
    UINTN MaxTransferSize = 0;    
    UINTN HandleCount = 0;
    EFI_HANDLE *HandleBuffer = 0;
    
    
    //
    // Search for the USBFn protocol
    //
    Status = EfiLocateHandleBuffer(ByProtocol, &EfiUsbProtocolGuid, NULL,
                                    &HandleCount, &HandleBuffer);
    //
    // If suceeded and we found > 0 handles, get the first first one
    //
    if (!EFI_ERROR(Status) && BOOLIFY(HandleCount))
    {
        //
        // Open the interface to USBFn protocol
        //
        Status = EfiOpenProtocol(HandleBuffer[HandleCount - 1], 
                                    &EfiUsbProtocolGuid, &EfiUsbProtocol);
        if (EFI_ERROR(Status))
        {
            dprintf(L"charge: Failed to open USBFn protocol interface.\r\n");
            goto Done;
        }
        
        //
        // Make sure the USBFn protocol revision is greater than or equal to
        // REVISION1 for charge application to work. If we don't have the
        // correct revision, we have to fail as the behavior is undefined
        //
        if (!IS_USBFN_PROTOCOL_REVISION_SUPPORTED(EfiUsbProtocol->Revision,
            EFI_USBFN_IO_PROTOCOL_REVISION1))
        {
            Status = EFI_UNSUPPORTED;
            dprintf(L"charge: USBFnIo interface revision mismatch\r\n");
            dprintf(L"charge: Supported revision 0x%x or greater, USBFn "
                L"revision 0x%x\r\n", EFI_USBFN_IO_PROTOCOL_REVISION1,
                EfiUsbProtocol->Revision);
            goto Done;
        }
        
        //
        // Allocate the internal buffers for IO over the bus
        //
        Status = AllocBuffers();
        if (EFI_ERROR(Status))
        {
            dprintf(L"charge: Memory allocation failed.\r\n");
            goto Done;
        }
        
        // 
        // Keep track of the handle
        //
        ClUsbFnIoHandle = HandleBuffer[0];
    }
    else
    {
        dprintf(L"charge: LocateHandleBuffer() failed, error 0x%x\r\n", Status);
    }
        
    //
    // Enable the endpoints of the USB bus
    //
    Status = ConfigureEnableEndpoints(EfiUsbProtocol);
    if (EFI_ERROR(Status))
    {
        dprintf(L"charge: Could not ConfigureEnableEndpoints\r\n");
        goto Done;
    }
    
    //
    // Getting maximum transfer size from USBFn
    //
    Status = EfiUsbFnGetMaxTransferSize(EfiUsbProtocol, &MaxTransferSize);
    if (EFI_ERROR(Status))
    {
        dprintf(L"charge: Could not get the Max Transfer Size");
        goto Done;
    }

    //
    // If new transfer size is different from the default transfer size, 
    // realloc memory
    //
    if (MaxTransferSize && (MaxTransferSize != cMaxTransferSize))
    {
        cMaxTransferSize = MaxTransferSize;
        if (cRxBuffer)
        {
            dprintf(L"charge: Old RxBuffer=0x%x\r\n", cRxBuffer);
            Status = EfiUsbFnFreeTransferBuffer(EfiUsbProtocol, cRxBuffer);
            if (EFI_ERROR(Status))
            {
                dprintf(L"charge: Failed to deallocate memory for hanging RX "
                    L"buffer.\r\n");
                goto Done;
            }
            cRxBuffer = 0;
        }
        Status = EfiUsbFnAllocateTransferBuffer(EfiUsbProtocol, cMaxTransferSize, &cRxBuffer);
        if (EFI_ERROR(Status))
        {
            dprintf(L"charge: Failed to allocate memory for hanging RX buffer. "
                L"\r\n");
            goto Done;
        }
    }
    
    //
    // Set the global usb init to TRUE
    //
    UsbInit = TRUE;


Done:

    //
    // Free the HandleBuffer
    //
    if (BOOLIFY(HandleBuffer))
    {
                BlMmFreeHeap (HandleBuffer);
    }
    
    //
    // Free internal structures if we had an error
    //
    if (EFI_ERROR(Status))
    {
        dprintf(L"charge: Error in USBFn subsystem. Freeing buffers\r\n");
        FreeBuffers();
    }

    return EfiGetNtStatusCode(Status);

}



NTSTATUS UsbFnEnumerate()
/*++

Routine Description:

    Enumerate with a USB host

Arguments:

    None
    
Return Value:
    STATUS_TIMEOUT if we timed out during enumeration
    and others

--*/
{
    EFI_STATUS  Status = EFI_SUCCESS;
    EFI_EVENT   ConnectTimeoutEvent = 0;
   
    //
    // Get max packet sizes of all endpoints
    //
    Status = GetChargerEndpointMaxPacketSize();
    if (EFI_ERROR(Status))
    {
        goto Done;
    }
    
    //
    // Enumeration has to succeed in finite amount of time. Otherwise
    // we need to timeout
    // Create an EFI event to track this
    //
    Status = EfiCreateEvent(EVT_TIMER, TPL_NOTIFY, 0,0, &ConnectTimeoutEvent);
    if (EFI_ERROR(Status))
    {   
        dprintf(L"charge: Error creating event. Cannot enumerate\r\n");
        goto Done;
    }
    //
    // Start the timer
    //
    Status = EfiSetTimer(ConnectTimeoutEvent, TimerRelative, 
                                    USB_ENUMERATION_TIMEOUT);
    if (EFI_ERROR(Status))
    {
        dprintf(L"charge: Error setting event. Cannot enumerate\r\n");
        goto Done;
    }
    
    //
    // Start the enumeration loop. Keep looping till we have established
    // a connection with the host, or if timeout event has been signalled
    //
    while (EFI_SUCCESS != EfiCheckEvent(ConnectTimeoutEvent))
    {
        EventHandler();
        if (TRUE == cConnectionEstablished)
        {
            break;
        }
    }
    
    //
    // If we came out of the loop without establishing a connection,
    // it was probably due to time out
    //
    if(FALSE == cConnectionEstablished)
    {
        Status = EFI_TIMEOUT;
    }
    
Done: 
    if (BOOLIFY(ConnectTimeoutEvent))
    {
        EfiCloseEvent(ConnectTimeoutEvent);
    }
    return EfiGetNtStatusCode(Status);
}



NTSTATUS GetUsbConnectedToCharger(BOOLEAN *ConnectedToCharger)
/*++

Routine Description:

    Get whether or not we are connected to a USB charger

Arguments:

    ConnectedToCharger : Allocated boolean buffer
    
Return Value:
    NT Status

--*/
{
    EFI_STATUS Status;
    EFI_USBFN_PORT_TYPE PortType;
    
    if( NULL == ConnectedToCharger)
    {
        return STATUS_INVALID_PARAMETER;
    }
    
    //
    // Refresh our internal structs
    //
    EventHandler();
    
    //
    // Detect the port
    //
    Status = EfiUsbFnDetectPort(EfiUsbProtocol, &PortType); 
    if(EFI_ERROR(Status))
    {
        *ConnectedToCharger = FALSE;
        ChargeCurrent = CURRENT_SUSPENDED;
        return EfiGetNtStatusCode(Status);
    }

    //
    // Interpret the detected port and set the charge current
    //
    switch(PortType)
    {
        case EfiUsbStandardDownstreamPort:
                    *ConnectedToCharger = TRUE;
                    ChargeCurrent = CURRENT_STANDARD_DOWNSTREAM;
                    break;
        case EfiUsbStandardDownstreamPort30:
                    *ConnectedToCharger = TRUE;
                    ChargeCurrent = CURRENT_STANDARD_DOWNSTREAM_30;
                    break;
        case EfiUsbChargingDownstreamPort:
                    *ConnectedToCharger = TRUE;
                    ChargeCurrent = CURRENT_CHARGING_DOWNSTREAM;
                    break;
        case EfiUsbDedicatedChargingPort:
                    *ConnectedToCharger = TRUE;
                    ChargeCurrent = CURRENT_DEDICATED_CHARGING;
                    break;
        case EfiUsbInvalidDedicatedChargingPort:
                    *ConnectedToCharger = TRUE;
                    ChargeCurrent = CURRENT_SUSPENDED;
                    break;
        default:
                    *ConnectedToCharger = FALSE;
                    ChargeCurrent = CURRENT_SUSPENDED;
    }
        
    return STATUS_SUCCESS;
}



NTSTATUS GetUsbFnConnectionToHost(BOOLEAN *ConnectedToHost)
/*++

Routine Description:

    Get whether or not we are connected to a USB Host
    This host will require enumeration

Arguments:

    ConnectedToHost : Allocated boolean buffer
    
Return Value:
    NT Status

--*/
{
    EFI_STATUS Status;
    EFI_USBFN_PORT_TYPE PortType;
    
    if( NULL == ConnectedToHost)
    {
        return STATUS_INVALID_PARAMETER;
    }
    
    //
    // Refresh internal structures
    //
    EventHandler();
    
    //
    // Detect the port
    //
    Status = EfiUsbFnDetectPort(EfiUsbProtocol, &PortType); 
    if(EFI_ERROR(Status))
    {
        *ConnectedToHost = FALSE;
        return EfiGetNtStatusCode(Status);
    }

    
    
    // 
    // Interpret only the cases that represent Host ports
    //
    if(EfiUsbStandardDownstreamPort == PortType || 
            EfiUsbStandardDownstreamPort30 == PortType ||        
            EfiUsbChargingDownstreamPort == PortType)
    {
        *ConnectedToHost = TRUE;
    }
    else
    {
        *ConnectedToHost = FALSE;
    }
    
    return STATUS_SUCCESS;
}



NTSTATUS UsbFnEnumerateIfConnected(BOOLEAN *ConnectedToHost)
/*++

Routine Description:

    Returns BOOLEAN whether or not we are connected to a HOST
    and also enumerates with the HOST if connected

Arguments:

    ConnectedToHost : Allocated boolean buffer
    
Return Value:
    NT Status

--*/
{
    NTSTATUS Status;
    
    if( ConnectedToHost == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }
    
    
    //
    // Refresh internal structures
    //
    EventHandler();
    
    Status = GetUsbFnConnectionToHost(ConnectedToHost);
    if(!NT_SUCCESS(Status))
    {
        return Status;
    }
    
    if(TRUE == *ConnectedToHost && FALSE == cConnectionEstablished)
    {
        //
        // Blocking call till we finish enumeration
        //
        Status = UsbFnEnumerate();
    }
    if(FALSE == *ConnectedToHost)
    {
        GetUsbConnectedToCharger(ConnectedToHost);
    }
    
    return Status;

}


NTSTATUS GetCurrentConnectionStatus(BOOLEAN *CurrentConnectionStatus)
/*++

Routine Description:

    Returns BOOLEAN whether or not we have an established connection with
    a host. A connection that is established also has enumeration successfully
    completed

Arguments:

    CurrentConnectionStatus: Allocated boolean buffer
    
Return Value:
    NT Status

--*/
{
    if(NULL == CurrentConnectionStatus)
    {
        return STATUS_INVALID_PARAMETER;
    }
    
    *CurrentConnectionStatus = cConnectionEstablished;
    
    return STATUS_SUCCESS;
}


NTSTATUS DetectPort()
/*++

Routine Description:

    Detects port

Arguments:

    None
    
Return Value:
    NT Status

--*/
{
    EFI_STATUS Status;
    EFI_USBFN_PORT_TYPE PortType;
    
    
    EventHandler();
    
    Status = EfiUsbFnDetectPort(EfiUsbProtocol, &PortType); 
    dprintf(L"charge: DetectPort %d  | 0x%x \r\n", PortType, PortType);
    
    return EfiGetNtStatusCode(Status);
}









