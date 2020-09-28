/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efibattery.c

Abstract:

    Support functions for the battery charging protocol

Environment:

    Boot
    

--*/

#include "chargelib.h"
#include "efiwrappers.h"

//
// GUID for the Battery Charging Protocol
//
EFI_GUID EfiBatteryCharchingProtocolGuid = EFI_BATTERY_CHARGING_PROTOCOL_GUID;

//
// Pointer to the Battery Charging Protocol struct
//
EFI_BATTERY_CHARGING_PROTOCOL *EfiBatteryChargingProtocol;

//
// Charging completion token
//
EFI_BATTERY_CHARGING_COMPLETION_TOKEN   *CompletionToken = NULL;

extern UINT32 BatteryLevel;
extern UINT32 ChargingProtocolRevision;
extern INT32 CurrentGoingIntoBattery;
extern UINT32 VoltageAcrossBatteryTerminals;
extern INT32 TemperatureOfBattery;
extern UINT32 VoltageOverUSBCable;
extern UINT32 CurrentThroughUSBCable;


//
// Used by the EFI / PCAT common routine. Set by ChargeBattery
//
extern CHARGE_STATUS ChargeStatus;


NTSTATUS BatteryChargingProtocolInit()
/*++

Routine Description:

    Locate the battery charging protocol on the EFI device

Arguments:
    
    None
  
Return Value:

    NT status code.

--*/
{
    NTSTATUS Status;        
    UINTN HandleCount = 0;
    EFI_HANDLE *HandleBuffer = 0;
    
    //
    // Search for the charging protocol
    //
    Status = EfiLocateHandleBuffer(ByProtocol, &EfiBatteryCharchingProtocolGuid, NULL,
                                    &HandleCount, &HandleBuffer);
    //
    // If suceeded and we found > 0 handles, get the last one
    //
    if (!EFI_ERROR(Status) && BOOLIFY(HandleCount))
    {
        //
        // Open the interface to charging protocol
        //
        Status = EfiOpenProtocol(HandleBuffer[HandleCount - 1], 
                                    &EfiBatteryCharchingProtocolGuid, &EfiBatteryChargingProtocol);
        if (EFI_ERROR(Status))
        {
            CHARGING_LOGGING("charge: Failed to open charging protocol interface with 0x%x\r\n", Status);
        }
        else
        {
            ChargingProtocolRevision = EfiBatteryChargingProtocol->Revision;
            CHARGING_LOGGING("charge: Battery Charging Protocol Revision %u (0x%x)\r\n", ChargingProtocolRevision, ChargingProtocolRevision);
        }
    }
    else
    {
        CHARGING_LOGGING("charge: Failed to locate charging protocol with 0x%x\r\n", Status);
    }    

    return Status;
}


NTSTATUS GetBatteryStatus()
/*++

Routine Description:

    Get the current battery level and amount of current going into battery.

Arguments:
    
    None.

Return Value:

    NT status code.

--*/
{
    NTSTATUS Status;
    UINT32 RatedCapacity;
    
    BatteryLevel = CHARGE_MIN;
    CurrentGoingIntoBattery = UNKNOWN_CURRENT_INTO_BATTERY;

    Status = EfiBatteryChargingGetBatteryStatus(EfiBatteryChargingProtocol, 
                                                &BatteryLevel, 
                                                &RatedCapacity, 
                                                &CurrentGoingIntoBattery);
    if(!NT_SUCCESS(Status))
    {
        return Status;
    }

    BatteryLevel = (BatteryLevel > CHARGE_MAX) ? CHARGE_MAX : BatteryLevel;
    DebugConsolePrint(L"charge: Battery SOC %u Total Capacity %u Current into Battery %d\r\n",
        BatteryLevel, RatedCapacity, CurrentGoingIntoBattery);
       
    return Status;
}


NTSTATUS GetBatteryInformation()
/*++

Routine Description:

    Get the current battery level, amount of current going into battery,
    voltage across battery terminals, temperature of the the battery,
    voltage over the USB cable, and current through the USB cable.

Arguments:
    
    None.

Return Value:

    NT status code.

--*/
{
    NTSTATUS Status;
   
    BatteryLevel = CHARGE_MIN;
    CurrentGoingIntoBattery = UNKNOWN_CURRENT_INTO_BATTERY;
    VoltageAcrossBatteryTerminals = UNKNOWN_VOLTAGE_ACROSS_BATTERY_TERMINALS;
    TemperatureOfBattery = UNKNOWN_BATTERY_TEMPERATURE;
    VoltageOverUSBCable = UNKNOWN_VOLTAGE_OVER_USB_CABLE;
    CurrentThroughUSBCable = UNKNOWN_CURRENT_THROUGH_USB_CABLE;

    Status = EfiBatteryChargingGetBatteryInformation(EfiBatteryChargingProtocol, 
                                                     &BatteryLevel, 
                                                     &CurrentGoingIntoBattery,
                                                     &VoltageAcrossBatteryTerminals,
                                                     &TemperatureOfBattery,
                                                     &VoltageOverUSBCable,
                                                     &CurrentThroughUSBCable);
    if(!NT_SUCCESS(Status))
    {
        return Status;
    }

    BatteryLevel = (BatteryLevel > CHARGE_MAX) ? CHARGE_MIN : BatteryLevel;
    DebugConsolePrint(L"charge: Battery SOC %u CIB %d VABT %u TOB %d VUSB %u IUSB %u\r\n",
        BatteryLevel,
        CurrentGoingIntoBattery,
        VoltageAcrossBatteryTerminals,
        TemperatureOfBattery,
        VoltageOverUSBCable,
        CurrentThroughUSBCable);
    
    return Status;
}

NTSTATUS UpdateBatteryStatus()
{
    NTSTATUS Status = STATUS_SUCCESS;
    switch (ChargingProtocolRevision)
    {
    case EFI_BATTERY_CHARGING_PROTOCOL_REVISION2:
        Status = GetBatteryInformation();
        break;
    case EFI_BATTERY_CHARGING_PROTOCOL_REVISION1:
        __fallthrough;
    default:
        Status = GetBatteryStatus();
    }
    return Status;
}

VOID
#ifdef _X86_
_cdecl
#endif
 __CompletionFunction(EFI_EVENT Event, VOID *Context)
/*++

Routine Description:

    This function is called when the battery driver completes the ChargeBattery
    call, either with success or failure

Arguments:
    
    Event:          The EFI Event
    Context:        Context passed
  
Return Value:

    NT status code.

--*/
{
    EFI_BATTERY_CHARGING_COMPLETION_TOKEN* Token = (EFI_BATTERY_CHARGING_COMPLETION_TOKEN*)Context;

    BLOCK_FRE
    ( 
        UNREFERENCED_PARAMETER(Event);
    )
    
    DebugConsolePrint(L"charge: Charge CompletionToken: Status %d Event 0x%x\r\n", CompletionToken->Status, CompletionToken->Event);
    DebugConsolePrint(L"charge: Event 0x%x \r\n", Event);
    DebugConsolePrint(L"charge: Completion event signaled, EFI Battery Charging Status=%d\r\n", Token->Status);
    
    //
    // Convert the EFI_BATTERY_CHARGING_STATUS to the platform agnostic 
    // CHARGE_STATUS
    //
    switch(Token->Status)
    {
        //
        // None
        //
        case    EfiBatteryChargingStatusNone:
                ChargeStatus = None;
                break;
        //
        // Success
        //
        case    EfiBatteryChargingStatusSuccess:
                ChargeStatus = Success;
                break;
        //
        // Overheat
        //
        case    EfiBatteryChargingStatusOverheat:
                ChargeStatus = Overheat;
                break;
        //
        // VoltageOutOfRange
        //
        case    EfiBatteryChargingStatusVoltageOutOfRange:
                ChargeStatus = VoltageOutOfRange;
                break;

        case    EfiBatteryChargingStatusCurrentOutOfRange:
                ChargeStatus = CurrentOutOfRange;
                break;

        case    EfiBatteryChargingStatusTimeout:
                ChargeStatus = Timeout;
                break;

        case    EfiBatteryChargingStatusAborted:
                ChargeStatus = Aborted;
                break;

        case    EfiBatteryChargingStatusDeviceError:
                ChargeStatus = DeviceError;
                break;

        case    EfiBatteryChargingStatusBatteryChargingNotSupported:
                ChargeStatus = ChargingNotSupported;
                break;

        case    EfiBatteryChargingStatusExtremeCold:
                ChargeStatus = ExtremeCold;
                break;                

        case    EfiBatteryChargingStatusBatteryNotDetected:
                ChargeStatus = BatteryNotDetected;
                break;

        case    EfiBatteryChargingSourceNotDetected:
                ChargeStatus = SourceNotDetected;
                break;

        case    EfiBatteryChargingSourceVoltageInvalid:
                ChargeStatus = SourceVoltageInvalid;
                break;

        case    EfiBatteryChargingSourceCurrentInvalid:
                ChargeStatus = SourceCurrentInvalid;
                break;

        case    EfiBatteryChargingErrorRequestShutdown:
                ChargeStatus = RequestShutdown;
                break;

        case    EfiBatteryChargingErrorRequestReboot:
                ChargeStatus = RequestReboot;
                break;

        default:
                ChargeStatus = None;
    }

    switch (ChargingProtocolRevision)
    {
    case EFI_BATTERY_CHARGING_PROTOCOL_REVISION2:
        CHARGING_LOGGING("charge: Event: %x, Status=%d, ChargeStatus=%d SOC=%d CIB=%d VABT=%u TOB=%d VUSB=%u IUSB=%u Tick=0x%x\r\n",
            Event, Token->Status, ChargeStatus, BatteryLevel, CurrentGoingIntoBattery, VoltageAcrossBatteryTerminals,
            TemperatureOfBattery, VoltageOverUSBCable, CurrentThroughUSBCable, BlTimeGetTickCount());
        break;
    case EFI_BATTERY_CHARGING_PROTOCOL_REVISION1:
        __fallthrough;
    default:
        CHARGING_LOGGING("charge: Event: %x, Status=%d, ChargeStatus=%d SOC=%d CIB=%d Tick=0x%x\r\n",
            Event, Token->Status, ChargeStatus, BatteryLevel, CurrentGoingIntoBattery, BlTimeGetTickCount());
    }
    return;    
}


NTSTATUS ChargeBattery(UINT32 MaximumCurrent, UINT32 TargetStateOfCharge)
/*++

Routine Description:

    Non-blocking call to ask the battery driver to start charging
    Calls the __CompletionFunction when the driver completes the request

Arguments:
    
    MaximumCurrent:         Maximum current to use in the case of USB stack
    
    TargetStateOfCharge:    Battery level (in percentage) till what the battery
                            needs to be charged
  
Return Value:

    NT status code.

--*/
{
    EFI_STATUS EfiStatus = EFI_SUCCESS;
    
    if(NULL == CompletionToken)
    {
        CompletionToken = BlMmAllocateHeap(sizeof(EFI_BATTERY_CHARGING_COMPLETION_TOKEN));
        if(NULL == CompletionToken)
        {
            return STATUS_NO_MEMORY;
        }
        CompletionToken->Event = NULL;
        CompletionToken->Status = EfiBatteryChargingStatusNone;
    }
    
    CompletionToken->Status = EfiBatteryChargingStatusNone;
    
    //
    // Create an EFI event if not already created.
    //
    if(NULL == CompletionToken->Event)
    {    
        EfiStatus = EfiCreateEvent(EVT_NOTIFY_SIGNAL, TPL_NOTIFY, 
                                    __CompletionFunction, (VOID*)CompletionToken, 
                                    &(CompletionToken->Event));
        if (EFI_ERROR(EfiStatus))
        {
            goto End;
        }    
    }

    //
    // Reset the ChargeStatus variable
    //
    ChargeStatus = None;
    
    //
    // Tell the driver to start charging
    //
    EfiStatus = EfiBatteryChargingChargeBattery(EfiBatteryChargingProtocol,
                                                    MaximumCurrent, 
                                                    TargetStateOfCharge,
                                                    CompletionToken);
    CHARGING_LOGGING("charge: ChargeBattery MaxCurrent %d TSOC %d \r\n",
        MaximumCurrent, TargetStateOfCharge);
End:    
    
    return EfiGetNtStatusCode(EfiStatus);
}


NTSTATUS CloseCompletionTokenEvent()
{
    EFI_STATUS EfiStatus;
    
    //
    // Close the event
    //
    if(NULL != CompletionToken && NULL != CompletionToken->Event)
    {
        EfiStatus = EfiCloseEvent(CompletionToken->Event);
    }
    
    //
    // Free the memory associated with the Completion Token
    //
    if(NULL != CompletionToken)
    { 
        BlMmFreeHeap(CompletionToken);
        CompletionToken = NULL;
    }

    return STATUS_SUCCESS;
}


NTSTATUS BatteryEventHandler()
/*++

Routine Description:
    
    On EFI, this does nothing because the driver is driven by calls to 
    GetBatteryStatus
  
Arguments:
    
    None
    
Return Value:

    NT status code.

--*/
{
    return STATUS_SUCCESS;
}


NTSTATUS Sleep( UINT32 Time )
/*++

Routine Description:

    For EFI, this uses EfiStall to stall the processor for the given amount
    of time (in milliseconds)
Arguments:
    
    Time:       Time in milliseconds
  
Return Value:

    NT Status

--*/
{
    EfiStall( Time);
    return STATUS_SUCCESS;
}
