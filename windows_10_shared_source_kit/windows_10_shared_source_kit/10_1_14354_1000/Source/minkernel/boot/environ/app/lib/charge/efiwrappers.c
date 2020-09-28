/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efiwrappers.c

Abstract:

    Every function in this source file is a wrapper for the UEFI USBfn protocol 
    and the Battery Charging Protocol functions.
    
    The wrappers change the context from application-to-firmware before making 
    the EFI function call.


Environment:

    Boot    

--*/

#include "efiwrappers.h"


//-----------------------------------------------------------------------------
// General wrappers
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// EFI UsbFn Protocol Wrappers
//-----------------------------------------------------------------------------



NTSTATUS EfiUsbFnEventHandler( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    EFI_USBFN_MESSAGE    *Message,
    UINTN *PayloadSize,
    EFI_USBFN_MESSAGE_PAYLOAD *Payload
    )
{
    PHYSICAL_ADDRESS EfiUsbFnProtocolPa;
    PHYSICAL_ADDRESS MessagePa;
    PHYSICAL_ADDRESS PayloadSizePa;
    PHYSICAL_ADDRESS PayloadPa;
    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS Status;
    
    
    // Check the context... switch to firmware to make the firmware call
    
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {
        BlMmTranslateVirtualAddress(EfiUsbFnProtocol, &EfiUsbFnProtocolPa);
        EfiUsbFnProtocol = (EFI_USBFN_IO_PROTOCOL *) (ULONG_PTR)EfiUsbFnProtocolPa.QuadPart;
        
        BlMmTranslateVirtualAddress(Message, &MessagePa);
        Message = (EFI_USBFN_MESSAGE *) (ULONG_PTR)MessagePa.QuadPart;
        
        BlMmTranslateVirtualAddress(PayloadSize, &PayloadSizePa);
        PayloadSize = (UINTN *) (ULONG_PTR)PayloadSizePa.QuadPart;
        
        BlMmTranslateVirtualAddress(Payload, &PayloadPa);
        Payload = (EFI_USBFN_MESSAGE_PAYLOAD *) (ULONG_PTR)PayloadPa.QuadPart;
        
        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to EFI function
    
    Status = EfiUsbFnProtocol->EventHandler(EfiUsbFnProtocol, Message, PayloadSize, Payload);

    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);

}



NTSTATUS EfiUsbFnConfigureEnableEndpoints( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    EFI_USB_DEVICE_INFO    *DeviceInfo
    )
{
    PHYSICAL_ADDRESS EfiUsbFnProtocolPa;
    PHYSICAL_ADDRESS DeviceInfoPa;
    EFI_STATUS Status;
    EXECUTION_CONTEXT_TYPE ContextType;
    
     
    // Check the context... switch to firmware to make the firmware call
    
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {    
         
        BlMmTranslateVirtualAddress(EfiUsbFnProtocol, &EfiUsbFnProtocolPa);
        EfiUsbFnProtocol = (EFI_USBFN_IO_PROTOCOL *) (ULONG_PTR)EfiUsbFnProtocolPa.QuadPart;
        
        BlMmTranslateVirtualAddress(DeviceInfo, &DeviceInfoPa);
        DeviceInfo = (EFI_USB_DEVICE_INFO *) (ULONG_PTR)DeviceInfoPa.QuadPart;
        
        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to EFI function
    Status = EfiUsbFnProtocol->ConfigureEnableEndpoints(EfiUsbFnProtocol, DeviceInfo);

    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
 
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);

}



NTSTATUS EfiUsbFnDetectPort( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    EFI_USBFN_PORT_TYPE    *PortType
    )
{
    PHYSICAL_ADDRESS EfiUsbFnProtocolPa;
    PHYSICAL_ADDRESS PortTypePa;
    EFI_STATUS Status;
    EXECUTION_CONTEXT_TYPE ContextType;
    
     
    // Check the context... switch to firmware to make the firmware call
    
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {    
         
        BlMmTranslateVirtualAddress(EfiUsbFnProtocol, &EfiUsbFnProtocolPa);
        EfiUsbFnProtocol = (EFI_USBFN_IO_PROTOCOL *) (ULONG_PTR)EfiUsbFnProtocolPa.QuadPart;
        
        BlMmTranslateVirtualAddress(PortType, &PortTypePa);
        PortType = (EFI_USBFN_PORT_TYPE *) (ULONG_PTR)PortTypePa.QuadPart;
        
        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to EFI function
    Status = EfiUsbFnProtocol->DetectPort(EfiUsbFnProtocol, PortType);

    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
 
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);

}



NTSTATUS EfiUsbFnAllocateTransferBuffer( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    UINTN    Size,
    VOID    **Buffer
    )
{
    PHYSICAL_ADDRESS EfiUsbFnProtocolPa;
    PHYSICAL_ADDRESS BufferPa;
    EFI_STATUS Status;
    EXECUTION_CONTEXT_TYPE ContextType;
    
     
    // Check the context... switch to firmware to make the firmware call
    
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {    
         
        BlMmTranslateVirtualAddress(EfiUsbFnProtocol, &EfiUsbFnProtocolPa);
        EfiUsbFnProtocol = (EFI_USBFN_IO_PROTOCOL *) (ULONG_PTR)EfiUsbFnProtocolPa.QuadPart;
        
        BlMmTranslateVirtualAddress(Buffer, &BufferPa);
        Buffer = (VOID **) (ULONG_PTR)BufferPa.QuadPart;
        
        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to EFI function
    Status = EfiUsbFnProtocol->AllocateTransferBuffer(EfiUsbFnProtocol, Size, Buffer);

    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
 
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);

}



NTSTATUS EfiUsbFnFreeTransferBuffer( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    VOID *Buffer 
    )
{
    PHYSICAL_ADDRESS EfiUsbFnProtocolPa;
    PHYSICAL_ADDRESS BufferPa;
    EFI_STATUS Status;
    EXECUTION_CONTEXT_TYPE ContextType;
    
     
    // Check the context... switch to firmware to make the firmware call
    
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {    
         
        BlMmTranslateVirtualAddress(EfiUsbFnProtocol, &EfiUsbFnProtocolPa);
        EfiUsbFnProtocol = (EFI_USBFN_IO_PROTOCOL *) (ULONG_PTR)EfiUsbFnProtocolPa.QuadPart;
        
        BlMmTranslateVirtualAddress(Buffer, &BufferPa);
        Buffer = (VOID *) (ULONG_PTR)BufferPa.QuadPart;
        
        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to EFI function
    Status = EfiUsbFnProtocol->FreeTransferBuffer(EfiUsbFnProtocol, Buffer);

    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
 
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);

}



NTSTATUS EfiUsbFnGetEndpointMaxPacketSize( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol,
    EFI_USB_ENDPOINT_TYPE EndpointType,
    EFI_USB_BUS_SPEED BusSpeed,
    UINT16 *MaxPacketSize 
    )
{
    PHYSICAL_ADDRESS EfiUsbFnProtocolPa;
    PHYSICAL_ADDRESS MaxPacketSizePa;
    EFI_STATUS Status;
    EXECUTION_CONTEXT_TYPE ContextType;
    
     
    // Check the context... switch to firmware to make the firmware call
    
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {    
         
        BlMmTranslateVirtualAddress(EfiUsbFnProtocol, &EfiUsbFnProtocolPa);
        EfiUsbFnProtocol = (EFI_USBFN_IO_PROTOCOL *) (ULONG_PTR)EfiUsbFnProtocolPa.QuadPart;
        
        BlMmTranslateVirtualAddress(MaxPacketSize, &MaxPacketSizePa);
        MaxPacketSize = (UINT16 *) (ULONG_PTR)MaxPacketSizePa.QuadPart;
        
        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to EFI function
    Status = EfiUsbFnProtocol->GetEndpointMaxPacketSize(EfiUsbFnProtocol, EndpointType, BusSpeed, MaxPacketSize);

    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
 
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);

}



NTSTATUS EfiUsbFnGetMaxTransferSize( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    UINTN *MaxTransferSize 
    )
{
    PHYSICAL_ADDRESS EfiUsbFnProtocolPa;
    PHYSICAL_ADDRESS MaxTransferSizePa;
    EFI_STATUS Status;
    EXECUTION_CONTEXT_TYPE ContextType;
    
     
    // Check the context... switch to firmware to make the firmware call
    
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {    
         
        BlMmTranslateVirtualAddress(EfiUsbFnProtocol, &EfiUsbFnProtocolPa);
        EfiUsbFnProtocol = (EFI_USBFN_IO_PROTOCOL *) (ULONG_PTR)EfiUsbFnProtocolPa.QuadPart;
        
        BlMmTranslateVirtualAddress(MaxTransferSize, &MaxTransferSizePa);
        MaxTransferSize = (UINTN *) (ULONG_PTR)MaxTransferSizePa.QuadPart;
        
        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to EFI function
    Status = EfiUsbFnProtocol->GetMaxTransferSize(EfiUsbFnProtocol, MaxTransferSize);

    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
 
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);

}



NTSTATUS EfiUsbFnGetDeviceInfo( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    EFI_USBFN_DEVICE_INFO_ID Id,
    UINTN *BufferSize,
    VOID *Buffer
    )
{
    PHYSICAL_ADDRESS EfiUsbFnProtocolPa;
    PHYSICAL_ADDRESS BufferSizePa;
    PHYSICAL_ADDRESS BufferPa;
    EFI_STATUS Status;
    EXECUTION_CONTEXT_TYPE ContextType;
    
     
    // Check the context... switch to firmware to make the firmware call
    
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {    
         
        BlMmTranslateVirtualAddress(EfiUsbFnProtocol, &EfiUsbFnProtocolPa);
        EfiUsbFnProtocol = (EFI_USBFN_IO_PROTOCOL *) (ULONG_PTR)EfiUsbFnProtocolPa.QuadPart;
        
        BlMmTranslateVirtualAddress(BufferSize, &BufferSizePa);
        BufferSize = (UINTN *) (ULONG_PTR)BufferSizePa.QuadPart;
        
        BlMmTranslateVirtualAddress(Buffer, &BufferPa);
        Buffer = (VOID *) (ULONG_PTR)BufferPa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to EFI function
    Status = EfiUsbFnProtocol->GetDeviceInfo(EfiUsbFnProtocol, Id, BufferSize, Buffer);

    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
 
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);

}



NTSTATUS EfiUsbFnGetVendorIdProductId( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    UINT16 *Vid,
    UINT16 *Pid
    )
{
    PHYSICAL_ADDRESS EfiUsbFnProtocolPa;
    PHYSICAL_ADDRESS VidPa;
    PHYSICAL_ADDRESS PidPa;
    EFI_STATUS Status;
    EXECUTION_CONTEXT_TYPE ContextType;
    
     
    // Check the context... switch to firmware to make the firmware call
    
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {    
         
        BlMmTranslateVirtualAddress(EfiUsbFnProtocol, &EfiUsbFnProtocolPa);
        EfiUsbFnProtocol = (EFI_USBFN_IO_PROTOCOL *) (ULONG_PTR)EfiUsbFnProtocolPa.QuadPart;
        
        BlMmTranslateVirtualAddress(Vid, &VidPa);
        Vid = (UINT16 *) (ULONG_PTR)VidPa.QuadPart;
        
        BlMmTranslateVirtualAddress(Pid, &PidPa);
        Pid = (VOID *) (ULONG_PTR)PidPa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to EFI function
    Status = EfiUsbFnProtocol->GetVendorIdProductId(EfiUsbFnProtocol, Vid, Pid);

    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
 
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);

}



NTSTATUS EfiUsbFnAbortTransfer( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    UINT8 EndpointIndex,
    EFI_USBFN_ENDPOINT_DIRECTION Direction
    )
{
    PHYSICAL_ADDRESS EfiUsbFnProtocolPa;
    EFI_STATUS Status;
    EXECUTION_CONTEXT_TYPE ContextType;
    
     
    // Check the context... switch to firmware to make the firmware call
    
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {    
         
        BlMmTranslateVirtualAddress(EfiUsbFnProtocol, &EfiUsbFnProtocolPa);
        EfiUsbFnProtocol = (EFI_USBFN_IO_PROTOCOL *) (ULONG_PTR)EfiUsbFnProtocolPa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to EFI function
    Status = EfiUsbFnProtocol->AbortTransfer(EfiUsbFnProtocol, EndpointIndex, Direction);

    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
 
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);

}



NTSTATUS EfiUsbFnTransfer( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    UINT8 EndpointIndex,
    EFI_USBFN_ENDPOINT_DIRECTION Direction,
    UINTN *BufferSize,
    VOID *Buffer
    )
{
    PHYSICAL_ADDRESS EfiUsbFnProtocolPa;
    PHYSICAL_ADDRESS BufferSizePa;
    PHYSICAL_ADDRESS BufferPa;
    EFI_STATUS Status;
    EXECUTION_CONTEXT_TYPE ContextType;
    
     
    // Check the context... switch to firmware to make the firmware call
    
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {    
         
        BlMmTranslateVirtualAddress(EfiUsbFnProtocol, &EfiUsbFnProtocolPa);
        EfiUsbFnProtocol = (EFI_USBFN_IO_PROTOCOL *) (ULONG_PTR)EfiUsbFnProtocolPa.QuadPart;

        BlMmTranslateVirtualAddress(BufferSize, &BufferSizePa);
        BufferSize = (UINTN *) (ULONG_PTR)BufferSizePa.QuadPart;

        BlMmTranslateVirtualAddress(Buffer, &BufferPa);
        Buffer = (UINTN *) (ULONG_PTR)BufferPa.QuadPart;
        
        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to EFI function
    Status = EfiUsbFnProtocol->Transfer(EfiUsbFnProtocol, EndpointIndex, Direction, BufferSize, Buffer);

    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
 
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);

}



NTSTATUS EfiUsbFnGetEndpointStallState( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    UINT8 EndpointIndex,
    EFI_USBFN_ENDPOINT_DIRECTION Direction,
    BOOLEAN *State
    )
{
    PHYSICAL_ADDRESS EfiUsbFnProtocolPa;
    PHYSICAL_ADDRESS StatePa;
    EFI_STATUS Status;
    EXECUTION_CONTEXT_TYPE ContextType;
    
     
    // Check the context... switch to firmware to make the firmware call
    
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {    
         
        BlMmTranslateVirtualAddress(EfiUsbFnProtocol, &EfiUsbFnProtocolPa);
        EfiUsbFnProtocol = (EFI_USBFN_IO_PROTOCOL *) (ULONG_PTR)EfiUsbFnProtocolPa.QuadPart;
        
        BlMmTranslateVirtualAddress(State, &StatePa);
        State = (BOOLEAN *) (ULONG_PTR)StatePa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to EFI function
    Status = EfiUsbFnProtocol->GetEndpointStallState(EfiUsbFnProtocol, EndpointIndex, Direction, State);

    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
 
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);

}



NTSTATUS EfiUsbFnSetEndpointStallState( 
    EFI_USBFN_IO_PROTOCOL *EfiUsbFnProtocol, 
    UINT8 EndpointIndex,
    EFI_USBFN_ENDPOINT_DIRECTION Direction,
    BOOLEAN State
    )
{
    PHYSICAL_ADDRESS EfiUsbFnProtocolPa;
    EFI_STATUS Status;
    EXECUTION_CONTEXT_TYPE ContextType;
    
     
    // Check the context... switch to firmware to make the firmware call
    
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {    
         
        BlMmTranslateVirtualAddress(EfiUsbFnProtocol, &EfiUsbFnProtocolPa);
        EfiUsbFnProtocol = (EFI_USBFN_IO_PROTOCOL *) (ULONG_PTR)EfiUsbFnProtocolPa.QuadPart;
        

        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to EFI function
    Status = EfiUsbFnProtocol->SetEndpointStallState(EfiUsbFnProtocol, EndpointIndex, Direction, State);

    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
 
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);

}



//-----------------------------------------------------------------------------
// EFI Battery Charging Protocol wrappers
//-----------------------------------------------------------------------------



NTSTATUS EfiBatteryChargingGetBatteryStatus( 
    EFI_BATTERY_CHARGING_PROTOCOL *EfiBatteryChargingProtocol, 
    UINT32    *StateOfCharge,
    UINT32    *RatedCapacity,
    INT32     *CurrentIntoBattery
    )
{
    PHYSICAL_ADDRESS EfiBatteryChargingProtocolPa;
    PHYSICAL_ADDRESS StateOfChargePa;
    PHYSICAL_ADDRESS RatedCapacityPa;
    PHYSICAL_ADDRESS CurrentIntoBatteryPa;
    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS Status;
        
    // Check the context... switch to firmware to make the firmware call

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {
        BlMmTranslateVirtualAddress(EfiBatteryChargingProtocol, &EfiBatteryChargingProtocolPa);
        EfiBatteryChargingProtocol = (EFI_BATTERY_CHARGING_PROTOCOL *) (ULONG_PTR)EfiBatteryChargingProtocolPa.QuadPart;
        
        BlMmTranslateVirtualAddress(StateOfCharge, &StateOfChargePa);
        StateOfCharge = (UINT32 *) (ULONG_PTR)StateOfChargePa.QuadPart;
        
        BlMmTranslateVirtualAddress(RatedCapacity, &RatedCapacityPa);
        RatedCapacity = (UINT32 *) (ULONG_PTR)RatedCapacityPa.QuadPart;
        
        BlMmTranslateVirtualAddress(CurrentIntoBattery, &CurrentIntoBatteryPa);
        CurrentIntoBattery = (INT32 *) (ULONG_PTR)CurrentIntoBatteryPa.QuadPart;
        
        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to the protocol function
    
    Status = EfiBatteryChargingProtocol->GetBatteryStatus(EfiBatteryChargingProtocol, StateOfCharge, RatedCapacity, CurrentIntoBattery);
    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);
}



NTSTATUS EfiBatteryChargingChargeBattery( 
    EFI_BATTERY_CHARGING_PROTOCOL *EfiBatteryChargingProtocol, 
    UINT32    MaximumCurrent,
    UINT32    TargetStateOfCharge,
    EFI_BATTERY_CHARGING_COMPLETION_TOKEN    *CompletionToken
    )
{
    PHYSICAL_ADDRESS EfiBatteryChargingProtocolPa;
    PHYSICAL_ADDRESS CompletionTokenPa;   
    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS Status;
        
    // Check the context... switch to firmware to make the firmware call
    
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {
        BlMmTranslateVirtualAddress(EfiBatteryChargingProtocol, &EfiBatteryChargingProtocolPa);
        EfiBatteryChargingProtocol = (EFI_BATTERY_CHARGING_PROTOCOL *) (ULONG_PTR)EfiBatteryChargingProtocolPa.QuadPart;
        
        BlMmTranslateVirtualAddress(CompletionToken, &CompletionTokenPa);
        CompletionToken = (EFI_BATTERY_CHARGING_COMPLETION_TOKEN *) (ULONG_PTR)CompletionTokenPa.QuadPart;
        
        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to the protocol function
    
    Status = EfiBatteryChargingProtocol->ChargeBattery(EfiBatteryChargingProtocol, MaximumCurrent, TargetStateOfCharge, CompletionToken);
    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);

}



NTSTATUS EfiBatteryChargingGetBatteryInformation( 
    EFI_BATTERY_CHARGING_PROTOCOL *EfiBatteryChargingProtocol, 
    UINT32    *StateOfCharge,
    INT32     *CurrentIntoBattery,
    UINT32    *BatteryTerminalVoltage,
    INT32     *BatteryTemperature,
    UINT32    *USBCableVoltage,
    UINT32    *USBCableCurrent
    )
{
    PHYSICAL_ADDRESS EfiBatteryChargingProtocolPa;
    PHYSICAL_ADDRESS StateOfChargePa;
    PHYSICAL_ADDRESS CurrentIntoBatteryPa;
    PHYSICAL_ADDRESS BatteryTerminalVoltagePa;
    PHYSICAL_ADDRESS BatteryTemperaturePa;
    PHYSICAL_ADDRESS USBCableVoltagePa;
    PHYSICAL_ADDRESS USBCableCurrentPa;
    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS Status;
        
    // Check the context... switch to firmware to make the firmware call
    
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware)
    {
        BlMmTranslateVirtualAddress(EfiBatteryChargingProtocol, &EfiBatteryChargingProtocolPa);
        EfiBatteryChargingProtocol = (EFI_BATTERY_CHARGING_PROTOCOL *) (ULONG_PTR)EfiBatteryChargingProtocolPa.QuadPart;
        
        BlMmTranslateVirtualAddress(StateOfCharge, &StateOfChargePa);
        StateOfCharge = (UINT32 *) (ULONG_PTR)StateOfChargePa.QuadPart;
        
        BlMmTranslateVirtualAddress(CurrentIntoBattery, &CurrentIntoBatteryPa);
        CurrentIntoBattery = (INT32 *) (ULONG_PTR)CurrentIntoBatteryPa.QuadPart;
        
        BlMmTranslateVirtualAddress(BatteryTerminalVoltage, &BatteryTerminalVoltagePa);
        BatteryTerminalVoltage = (UINT32 *) (ULONG_PTR)BatteryTerminalVoltagePa.QuadPart;

        BlMmTranslateVirtualAddress(BatteryTemperature, &BatteryTemperaturePa);
        BatteryTemperature = (INT32 *) (ULONG_PTR)BatteryTemperaturePa.QuadPart;

        BlMmTranslateVirtualAddress(USBCableVoltage, &USBCableVoltagePa);
        USBCableVoltage = (UINT32 *) (ULONG_PTR)USBCableVoltagePa.QuadPart;

        BlMmTranslateVirtualAddress(USBCableCurrent, &USBCableCurrentPa);
        USBCableCurrent = (UINT32 *) (ULONG_PTR)USBCableCurrentPa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }
    
    // Make the call to the protocol function
    
    Status = EfiBatteryChargingProtocol->GetBatteryInformation(EfiBatteryChargingProtocol, StateOfCharge,
        CurrentIntoBattery, BatteryTerminalVoltage, BatteryTemperature, USBCableVoltage, USBCableCurrent);
    
    // Restore the original context
    
    if (ContextType != ExecutionContextFirmware)
    {
        BlpArchSwitchContext (ContextType);
    }
    
    return EfiGetNtStatusCode(Status);
}

