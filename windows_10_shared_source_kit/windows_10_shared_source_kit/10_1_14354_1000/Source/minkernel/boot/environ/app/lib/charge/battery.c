/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    battery.c

Abstract:

    Battery Charging Protocol support functions for the Charge Library

Environment:

    Boot
    

--*/

#include "chargelib.h"

//
// Externed variables
//
extern UINT32 Threshold;
extern BOOLEAN UsbInit;
extern DISPLAY_SCREEN ScreenA;
extern DISPLAY_SCREEN ScreenB;
extern UINT32 BatteryLevel;

//
// Globals and externed in other files
//
LARGE_INTEGER CurrentTick;
LONGLONG ScreenTimeOutSeconds;
LARGE_INTEGER ScreenTurnOnTick;

//
// The amount of current that the charge source is capable of handling
// This is updated by the charge source detection logic. For EFI systems, this
// is done by the usbfn subsystem
//
UINT32 ChargeCurrent = CURRENT_SUSPENDED;

//
// Charge Status is updated by the firmware-specific ChargeBattery function
//
CHARGE_STATUS ChargeStatus = None;


NTSTATUS Charge(UINT32 StateOfCharge, CHARGE_STATE State)
/*++

Routine Description:

    Charge routine
    This call blocks till the StateOfCharge has been achieved.

Arguments:

    StateOfCharge:  The target state of charge till when this call will be 
                    blocked
    State:          What mode of charge we are in - Threshold Charging or
                    Battery Charging Mode
Return Value:

    NT status code.

--*/
{
    NTSTATUS Status = STATUS_SUCCESS;
    NTSTATUS FinalScreenStatus = STATUS_SUCCESS;
    KEY_STROKE KeyStroke;
    BOOLEAN ConnectedToUsbCharger;
    BOOLEAN OffChargingSuccessLogged;
    BOOLEAN UpperErrorLogged;
    LONGLONG LongPressSeconds;
    LONGLONG AnimationDelaySeconds;
    LONGLONG TimeOutSeconds;
    LONGLONG ChargePauseDelaySeconds;
    LARGE_INTEGER QpcFrequency;
    LARGE_INTEGER InitialTick;
    LARGE_INTEGER AnimationTick;
    LARGE_INTEGER LongPressTick;
    LARGE_INTEGER ChargePauseDelayTicks;
    UINT32 TargetStateOfCharge = StateOfCharge;
    UINT32 OriginalChargeCurrent = ChargeCurrent;
    BOOLEAN ClearScreen = TRUE;
    
    //
    // Make sure the screen is ON when starting a fresh charge operation.
    // A real failure here is fatal and results in the phone shutting down.
    // 
    Status = TurnScreenOn();
    if (!NT_SUCCESS(Status))
    {
        CHARGING_LOGGING("charge: FATAL - Failed to ensure screen was ON before starting charging: 0x%x\r\n", Status);
        BlFwShutdown();
    }

    //
    // Start the charge operation
    //
    ChargeBattery(ChargeCurrent, TargetStateOfCharge);
    
    //
    // Initialize the alternating display screens
    //
    switch(State)
    {
        case StateBatteryChargingMode:
            CHARGING_LOGGING("charge: Enter Off Charging Mode SOC %u ChargeCurrent %u\r\n",
                BatteryLevel, ChargeCurrent);
            ScreenA = CHARGING_UI_NOPLUG(BatteryLevel);
            ScreenB = CHARGING_UI_PLUG(BatteryLevel);
            break;
        case StateChargingUntilThreshold:
            CHARGING_LOGGING("charge: Enter Threshold Charging Mode TSOC %u SOC %u ChargeCurrent %u\r\n",
                Threshold, BatteryLevel, ChargeCurrent);
            __fallthrough;
        default:
            ScreenA = BATTERY_1_RED;
            ScreenB = BATTERY_PLUG_1_RED;
    }
    
    //
    // Poll the battery to refresh it's state
    //
    Status = UpdateBatteryStatus();
    
    //
    //
    // If ChargeBattery failed due to no charge source being present 
    // 
    if(SourceNotDetected == ChargeStatus)
    {
        switch(State)
        {
            //
            // In the case of Battery Charging Mode, we will boot to the main OS
            //
            case StateBatteryChargingMode:
                CHARGING_LOGGING("charge: No source connected. Exit Off Charging Mode\r\n");
                ClearScreen = FALSE;
                Status = STATUS_SUCCESS;
                goto ChargeEnd;

            //
            // In all other cases, we will immediately shut down the device
            //
            case StateChargingUntilThreshold:
                __fallthrough;
            case StateUserInvokedChargeUntilThreshold:
                __fallthrough;
            default:    
                //
                // Display the UI
                //
                CHARGING_LOGGING("charge: No source connected. Shut Down\r\n");
                DisplayAlternatingUI(ScreenA, ScreenB);                
                Sleep(LOW_BATTERY_TURNOFF_DELAY);
                BlFwShutdown();
        }
    }            
    
    CHARGING_LOGGING("charge: A charge source is connected\r\n");

    // If the charge lib was called by a user invoked application, return back
    // to the charge routine.
    //
    if(StateUserInvokedChargeUntilThreshold == State)
    {
        CHARGING_LOGGING("charge: User invoked. Exit charging.\r\n");
        Status = STATUS_SUCCESS;
        goto ChargeEnd;
    }
    
    //
    // Initialize the USB protocol if not already initialized
    //
    if(FALSE == UsbInit)
    {
        CHARGING_LOGGING("charge: Initializing the USB protocol for enumeration\r\n"); 
        Status = UsbFnProtocolInit();
        if(!NT_SUCCESS(Status))
        {
            goto ChargeEnd;
        }
    }
    
    //
    // Record the current time.
    //
    InitialTick = BlTimeQueryPerformanceCounter(&QpcFrequency);

    //
    // Convert time to cycles
    //  
    ScreenTimeOutSeconds = CONVERT_TO_CYCLES(SCREEN_TIMEOUT_DELAY, 
                                                QpcFrequency.QuadPart);
                                                
    AnimationDelaySeconds = CONVERT_TO_CYCLES(ANIMATION_DELAY, 
                                                QpcFrequency.QuadPart);
                                                
    TimeOutSeconds = CONVERT_TO_CYCLES(THRESHOLD_TIMEOUT, 
                                            QpcFrequency.QuadPart);
    
    LongPressSeconds = CONVERT_TO_CYCLES(POWER_BUTTON_LONG_PRESS_DELAY, 
                                            QpcFrequency.QuadPart);

    ChargePauseDelaySeconds = CONVERT_TO_CYCLES(CHARGE_PAUSE_DELAY, 
                                            QpcFrequency.QuadPart);

    //
    // Initialize all the timers
    //
    ScreenTurnOnTick = InitialTick;
    AnimationTick = InitialTick;
    LongPressTick = InitialTick;
    ChargePauseDelayTicks = InitialTick;

    OffChargingSuccessLogged = FALSE;

    //
    // Start the main charging loop. This loop is multiplexed for Threshold 
    // Charging (TC) and Battery Charging Mode (BCM).
    //
    // This loop handles:
    //  -   Calling UpdateBatteryStatus() to update the battery metrics and draw
    //      the correct image based on weather the mode is TC or BCM
    //  -   Handle key press scenarios and the case that long press of power
    //      button in BCM will boot to the main OS
    //  -   Exiting from this loop on success. In the case of TC, boot to the 
    //      OS. In the case of BCM, remain in the loop until user presses 
    //      the power button or we get disconnected from the USB
    //      
    for(;;)
    {
        // 
        // Get the latest time
        //
        CurrentTick = BlTimeQueryPerformanceCounter(NULL);

        UpperErrorLogged = FALSE;

        switch(ChargeStatus)
        {
            //
            // Short circuit out of the switch on average case
            //
            case None:
                break;

            //
            // Success is signaled when TargetStateOfCharge is reached
            // successfully
            //
            case Success:
                {
                    //
                    // In the case of Threshold charging, continue with boot
                    //
                    if(StateChargingUntilThreshold == State)
                    {
                        CHARGING_LOGGING("charge: Threshold charging success.\r\n");
                        Status = STATUS_SUCCESS;
                        goto ChargeEnd;
                    }
                        
                    //
                    // In the case of Battery Charging mode, shut down if we 
                    // are not connected to a USB charger
                    //
                    else
                    {
                        if (OffChargingSuccessLogged == FALSE)
                        {
                            CHARGING_LOGGING("charge: Off charging success. Loop.\r\n");
                            OffChargingSuccessLogged = TRUE;
                        }
                        Status = GetUsbConnectedToCharger(&ConnectedToUsbCharger);
                        if(!NT_SUCCESS(Status) || FALSE == ConnectedToUsbCharger)
                        {
                            CHARGING_LOGGING("charge: Off charging not supported without USB. Shut down.\r\n");
                            BlFwShutdown();
                        }
                    }
                }
                break;

            //
            // In the following cases, display BATTERY_UNKNOWN image for 
            // a little while and then shut the device down
            //
            case Aborted:
                CHARGING_LOGGING("charge: Aborted. Shut down.\r\n");
                UpperErrorLogged = TRUE;
                __fallthrough;
            case DeviceError:
                if (UpperErrorLogged == FALSE)
                {
                    CHARGING_LOGGING("charge: DeviceError. Shut down.\r\n");
                    UpperErrorLogged = TRUE;
                }
                __fallthrough;
            case ExtremeCold:
                if (UpperErrorLogged == FALSE)
                {
                    CHARGING_LOGGING("charge: ExtremeCold. Shut down.\r\n");
                    UpperErrorLogged = TRUE;
                }
                __fallthrough;
            case ChargingNotSupported:
                if (UpperErrorLogged == FALSE)
                {
                    CHARGING_LOGGING("charge: ChargingNotSupported. Shut down.\r\n");
                    UpperErrorLogged = TRUE;
                }
                __fallthrough;
            case BatteryNotDetected:
                if (UpperErrorLogged == FALSE)
                {
                    CHARGING_LOGGING("charge: BatteryNotDetected. Shut down.\r\n");
                    UpperErrorLogged = TRUE;
                }
                __fallthrough;
            case SourceVoltageInvalid:
                if (UpperErrorLogged == FALSE)
                {
                    CHARGING_LOGGING("charge: SourceVoltageInvalid. Shut down.\r\n");
                    UpperErrorLogged = TRUE;
                }
                __fallthrough;
            case SourceCurrentInvalid:
                if (UpperErrorLogged == FALSE)
                {
                    CHARGING_LOGGING("charge: SourceCurrentInvalid. Shut down.\r\n");
                    UpperErrorLogged = TRUE;
                }
                DisplayUI(BATTERY_UNKNOWN);
                Sleep(BATTERY_ERROR_DELAY);
                BlFwShutdown();
                break;
            
            //
            // Shutdown the device if the driver requests it
            //
            case RequestShutdown:
                CHARGING_LOGGING("charge: Shutdown request from driver. Shutting down.\r\n");
                BlFwShutdown();
                break;
            
            //
            // Reboot the device if the driver requests it
            //
            case RequestReboot:
                CHARGING_LOGGING("charge: Reboot request from driver. Rebooting.\r\n");
                BlFwReboot();
                break;

            //
            // Pause charging for some time if timing out            
            //
            case Timeout:
                CHARGING_LOGGING("charge: Charging timing out. Pausing charging.\r\n");
                ChargePauseDelayTicks = CurrentTick;
                ChargeStatus = Pause;
                break;
            
            //
            // Pause charging for some time if overheating
            //
            case Overheat:
                CHARGING_LOGGING("charge: Battery overheating. Pausing charging.\r\n");
                ChargePauseDelayTicks = CurrentTick;
                ChargeStatus = Pause;
                break;
            
            //        
            // Restart the charge after the wait period is over
            //
            case Pause:
                if((CurrentTick.QuadPart - ChargePauseDelayTicks.QuadPart) >= 
                        ChargePauseDelaySeconds)
                {
                    //
                    // Restart Charge
                    //
                    CHARGING_LOGGING("charge: Pause done. Resume charging.\r\n");
                    ChargeBattery(ChargeCurrent, TargetStateOfCharge);
                    ChargeStatus = None;
                }       
                break;

            //
            // The following cases are valid cases and expected to be thrown 
            // by supported artificial batteries
            //
            case VoltageOutOfRange:
                CHARGING_LOGGING("charge: VoltageOutOfRange.\r\n");
                UpperErrorLogged = TRUE;
                __fallthrough;
            case CurrentOutOfRange:
                if (UpperErrorLogged == FALSE)
                {
                    CHARGING_LOGGING("charge: CurrentOutOfRange.\r\n");
                    UpperErrorLogged = TRUE;
                }
                Status = STATUS_SUCCESS;
                break;

            //
            // In all cases on charge source disconnect, shutdown the device
            //
            case SourceNotDetected:
                CHARGING_LOGGING("charge: Source not detected. Shutting down.\r\n");
                BlFwShutdown();
                break;

            default:
                break;
        }               
        
        //
        // Start and / or continue with enumeration. 
        //
        Status = GetUsbConnectedToCharger(&ConnectedToUsbCharger);
        if(NT_SUCCESS(Status) && FALSE != ConnectedToUsbCharger)
        {
            UsbFnEnumerateIfConnected(&ConnectedToUsbCharger);
        }
        
        //
        // If we have a new charge current determined by the USB stack, restart
        // charging 
        //
        if(ChargeCurrent != OriginalChargeCurrent)
        {
            CHARGING_LOGGING("charge: Enumeration resulted in updated current of %d. Restart charging.\r\n", ChargeCurrent);
            OriginalChargeCurrent = ChargeCurrent;
            ChargeBattery(ChargeCurrent, TargetStateOfCharge);
        }

        //
        // Get the keystroke
        //
        BlDisplayGetKeyStroke(FALSE, &KeyStroke);
        
        //
        // Handle a press of the power button
        //
        if(POWER_BUTTON == KeyStroke.ScanCode)
        {   
            // 
            // Always reset the screen timer
            //
            ScreenTurnOnTick = CurrentTick;
            
            //
            // If this is a long press, continue to boot if in Battery Charging
            // mode
            //
            if((StateBatteryChargingMode == State) && 
                (CurrentTick.QuadPart - LongPressTick.QuadPart > LongPressSeconds))
            {
                CHARGING_LOGGING("charge: Long press of power button detected. Exit charging to boot phone.\r\n");
                Status = STATUS_SUCCESS;
                goto ChargeEnd;
            }
        }
        else
        {
            //
            // Keep resetting the LongPressTick
            //
            LongPressTick = CurrentTick;
        }
        
        //
        // Reset the input buffer to pick up scan codes of the same kind on the 
        // next iteration
        //
        BlDisplayWipeInputBuffer(NULL);
        
        //
        // Update the UI if it is time to do so
        //
        if(CurrentTick.QuadPart - AnimationTick.QuadPart >= AnimationDelaySeconds)
        {
            //
            // Reset the animation counter
            //
            AnimationTick = BlTimeQueryPerformanceCounter(NULL);
            
            //
            // Poll the battery
            //
            Status = UpdateBatteryStatus();
            if(!NT_SUCCESS(Status))
            {
                goto ChargeEnd;
            }

            //
            // In battery charging mode, get the most relevant image
            //
            if(StateBatteryChargingMode == State)
            {                
                //
                // Check to see if we are below threshold
                //
                if(BatteryLevel < Threshold)
                {
                    CHARGING_LOGGING("charge: SOC %d went below threshold %d in Off Charging. Exit charging.\r\n",
                        BatteryLevel, Threshold);
                    goto ChargeEnd;
                }
                
                //
                // Update the UI. Min and Max values need special handling.
                // 
                switch(BatteryLevel)
                {   
                    case CHARGE_MIN:    
                        ScreenA = BATTERY_0;
                        ScreenB = BATTERY_PLUG_0;
                        break;

                    case CHARGE_MAX:
                        // Make both screens the plug icon so that it doesn't appear to blink.
                        ScreenA = CHARGING_UI_PLUG(BatteryLevel);
                        ScreenB = CHARGING_UI_PLUG(BatteryLevel);
                        break;

                    default:
                        ScreenA = CHARGING_UI_NOPLUG(BatteryLevel);
                        ScreenB = CHARGING_UI_PLUG(BatteryLevel);
                }
            }
            
            //
            // In threshold charging, check for threshold
            //
            if(StateChargingUntilThreshold == State)
            {
                //
                // If we are above threshold, continue with boot
                //
                if(BatteryLevel >= Threshold)
                {
                    CHARGING_LOGGING("charge: Threshold Charging has enough SOC %u\r\n", BatteryLevel);
                    goto ChargeEnd;
                }
            }
            
            //
            // Update the display
            //
            DisplayControl();
        }
    }

ChargeEnd:
    
    //
    // Ensure screen is ON and display is clear.
    //
    FinalScreenStatus = TurnScreenOn();
    if (!NT_SUCCESS(FinalScreenStatus))
    {
        CHARGING_LOGGING("charge: FATAL - Failed to ensure screen was ON before finishing charging: 0x%x\r\n", FinalScreenStatus);
        BlFwShutdown();
    }

    if(ClearScreen)
    {
        BlDisplayClearScreen(); //Fix for WPB 531737 [HTC 6891] [WP Blue GDR1] Black screen shows about 10 seconds between htc logo and windows logo
    }

    CHARGING_LOGGING("charge: Exit Charge with Status 0x%x\r\n", Status);

    return Status;
}



NTSTATUS DestroyBatterySubsystem()
/*++

Routine Description:

    Close all events, and free all dynamically allocated memory associated with
    battery charging subsystem

Return Value:

    NT status code.

--*/
{
    CloseCompletionTokenEvent();

    return STATUS_SUCCESS;
}
