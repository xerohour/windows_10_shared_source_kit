/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    charge.c

Abstract:

    This module contains implementation for the boot loader charging application

Environment:

    Boot

--*/

#include "chargelib.h"
#include "chargelibex.h"

extern UINT32 Threshold;
extern UINT32 ChargeCurrent;
extern DISPLAY_SCREEN ScreenA;
extern DISPLAY_SCREEN ScreenB;

//
// This function will just drain the battery until the PMIC shuts things down
//
void DrainBattery();

//
// Current going into battery retrieved from the EFI Battery Charging Protocol
//
INT32 CurrentGoingIntoBattery = UNKNOWN_CURRENT_INTO_BATTERY;

//
// Revision of the loaded charging protocol
//
UINT32 ChargingProtocolRevision = EFI_BATTERY_CHARGING_PROTOCOL_REVISION1;

//
// Voltage across battery terminals retrieved from the EFI Battery Charging Protocol
//
UINT32 VoltageAcrossBatteryTerminals= UNKNOWN_VOLTAGE_ACROSS_BATTERY_TERMINALS;

//
// Temperature of the battery retrieved from the EFI Battery Charging Protocol
//
INT32 TemperatureOfBattery = UNKNOWN_BATTERY_TEMPERATURE;

//
// Voltage across the USB cable retrieved from the EFI Battery Charging Protocol
//
UINT32 VoltageOverUSBCable = UNKNOWN_VOLTAGE_OVER_USB_CABLE;

//
// Current through the USB cable retrieved from the EFI Battery Charging Protocol
//
UINT32 CurrentThroughUSBCable = UNKNOWN_CURRENT_THROUGH_USB_CABLE;

//
// Battery level retrieved from the EFI Battery Charging Protocol
//
UINT32 BatteryLevel = CHARGE_MIN;

NTSTATUS StartCharge (StartChargeType ChargeType)
/*++

Routine Description:

    Charge applications entry point

Arguments:

    StartChargeType: Value indicating the type of charge operation to perform

Return Value:

    NT status code.

--*/
{
    NTSTATUS Status;
    TIME_FIELDS Time;    
    
    //
    // Continue loop counter
    //
    UINT32 ContinueLoopCounter = 0;
    
    //
    // OEM configured boolean in the BCD to enable or disable
    // Battery Charging Mode (off charging)
    //
    BOOLEAN BatteryChargingMode = DEFAULT_BATTERY_CHARGING_MODE;
    
    //
    // Boot Flow global flag which determines whether we are supposed
    // to go into battery charging mode on this boot or not
    //
    BOOLEAN BootFlowBatteryChargingMode = FALSE;
        
    //
    // Read configured boot flow scenarios. If they are not
    // configured to BfScenarioDefault, device will not go 
    // into Battery Charging Mode
    //
    BF_SCENARIO BootFlowScenario = BfScenarioDefault;
    BF_OPTION BootFlowOption = BfOptionNone;
    
    //
    // Boolean to read in the ForceFFU flag from the BCD
    //
    BOOLEAN FFUForced = FALSE;

    //
    // In the case of Update OS, the app will skip any other checks
    //
    BOOLEAN UpdateOsSkip = FALSE;

    //
    // Initialize the boot log file
    //
#ifdef CHARGE_DEBUG_LOG
    if(!NT_SUCCESS(BootLogInitialize(CHARGE_LOG_FILE_NAME, TRUE, 0)))
    {
        DebugConsolePrint(L"charge: Could not initialize BootLogLib\r\n");
    }
#endif  

    //
    // Dump the current system time
    //
    Status = BlGetTime(&Time, NULL);
    if (!NT_SUCCESS(Status))
    {
        CHARGING_LOGGING("charge: BlGetTime failed with status = 0x%08x\r\n", Status);
    } else
    {    
        CHARGING_LOGGING("charge: Begin System time %02d/%02d/%04d %02d:%02d:%02d:%02d \r\n", 
            Time.Month, Time.Day, Time.Year, Time.Hour, Time.Minute, Time.Second, Time.Milliseconds);
    }
    
    //
    // Initialize application specific protocols
    //
    Status = Initialize();
    if(!NT_SUCCESS(Status))
    {
        CHARGING_LOGGING("charge: Initialization failed with 0x%x\r\n", Status);
        goto ChargeMainEnd;
    }

    if (ChargeType >= StartChargeType_MAXTYPES)
    {
        CHARGING_LOGGING("charge: Warning - unsupported start type - defaulting to normal\r\n");
        ChargeType = StartChargeType_NormalOperation;
    }

    //
    // Checking for forced FFU option and skipping charging is only done if
    // flags are not being ignored. Flags are anything that prevent threshold
    // charging from starting or anything that allows further charging actions
    // after threshold charging is done.
    //
    if (ChargeType != StartChargeType_IgnoreFlags)
    {
        //
        // Get "forced" update mode flag setting. With this flag, set the device
        // will always enter FFU mode on startup. This flag is set during imaging
        // time in the BCD.
        // If flag is set, the charge app will be skipped as we are probably 
        // connected to a battery blank
        //
        Status = BlGetApplicationOptionBoolean(BCDE_FFULOADER_TYPE_FORCE_FFU, 
                                                &FFUForced );
        if (NT_SUCCESS(Status) && FALSE != FFUForced) 
        {
            CHARGING_LOGGING("charge: ForceFFU set. Skipping charging\r\n");
            goto ChargeMainEnd;
        }    
    }

    if (ChargeType == StartChargeType_DrainBattery)
    {
        CHARGING_LOGGING("charge: Request to drain battery.\r\n");
        DrainBattery();
    }

    //
    // Get the OEM configured battery threshold
    //
    Status = GetOemBatteryBootThreshold(&Threshold);
    if(!NT_SUCCESS(Status))
    {
        CHARGING_LOGGING("charge: Failed to get threshold 0x%x\r\n", Status);
        goto ChargeMainEnd;
    }

    CHARGING_LOGGING("charge: Threshold from BCD is %u\r\n", Threshold);

    //    
    // Skip Charge if we have to
    //
    if(BATTERY_SKIP_CHARGING == Threshold)
    {
        CHARGING_LOGGING("charge: Skipping charging based on special threshold value\r\n");
        goto ChargeMainEnd;
    }

    //    
    // Make sure BatteryLevel is above the OEM defined threshold. Otherwise, 
    // block and charge until the threshold has been reached
    //  
    do
    {
        Status = UpdateBatteryStatus();

        switch (ChargingProtocolRevision)
        {
        case EFI_BATTERY_CHARGING_PROTOCOL_REVISION2:
            CHARGING_LOGGING("charge: Starting Values - SOC: %u CurrentIn: %dmA Voltage: %umV - Status 0x%x\r\n",
                BatteryLevel, CurrentGoingIntoBattery, VoltageAcrossBatteryTerminals, Status);
            break;
        case EFI_BATTERY_CHARGING_PROTOCOL_REVISION1:
            __fallthrough;
        default:
            CHARGING_LOGGING("charge: Starting Values - SOC: %u CurrentIn: %dmA - Status 0x%x\r\n",
                BatteryLevel, CurrentGoingIntoBattery, Status);
        }
        
        switch(Status)
        {
            //
            // On success, continue with the function
            //
            case    STATUS_SUCCESS:
                    break;            
            
            //
            // In the following cases, call Charge, which will examine the 
            // platform specific Battery Charging Status and take the respective
            // action
            //
            case    STATUS_IO_DEVICE_ERROR:
                    __fallthrough;
            case    STATUS_DEVICE_NOT_READY:
                    Charge(Threshold, StateChargingUntilThreshold);
                    break;
            
            //
            // In the following cases, display a UI and shutdown the device 
            // after a predefined amount of time
            //
            case    STATUS_INVALID_PARAMETER:
                    __fallthrough;
            default:
                    //
                    // An unrecognized status code will cause the device to 
                    // shutdown
                    //
                    BlDisplayClearScreen();
                    DisplayUI(BATTERY_UNKNOWN);
                    Sleep(BATTERY_ERROR_DELAY);
                    BlFwShutdown();
        }

        //
        // Check to see if we are below threshold, in which case, we will block
        // until the battery level is above the predefined threshold value.
        // 
        // N.B. If StartCharge was called by a user invoked application, only 
        // the presence of a charger will be tested for.
        //      
        if(!IS_ABOVE_THRESHOLD(Threshold, BatteryLevel))
        {        
            //
            // Since we are going ahead and trying to charge, we get to control
            // the screen for sure now, so a clear call here is allowed.
            // 
            BlDisplayClearScreen();

            //
            // Go into charging mode
            //
            CHARGING_LOGGING("charge: Not enough battery to boot so blocking till %d\r\n", Threshold);
                        
            if(ChargeType != StartChargeType_UserInvoked)
            {
                Status = Charge(Threshold, StateChargingUntilThreshold);
                
                switch(Status)
                {
                    //
                    // On success, continue with function. If the Update OS
                    // flag is set, then skip to the end of the application
                    //
                    case    STATUS_SUCCESS:
                            if(FALSE != UpdateOsSkip)
                            {
                                goto ChargeMainEnd;
                            }
                            else
                            {
                                break;
                            }

                    //
                    // In the case of timeout, retry to charge the predefined
                    // number of times
                    case    STATUS_TIMEOUT:
                            ContinueLoopCounter++;
                                            
                            //
                            // Sanity check before proceeding by forcing the 
                            // next iteration of this loop
                            //
                            if( SANITY_CHECK_LOOP_LIMIT > ContinueLoopCounter)
                            {
                                CHARGING_LOGGING("charge: Retry charge %d of %d due to STATUS_TIMEOUT\r\n", 
                                        ContinueLoopCounter, SANITY_CHECK_LOOP_LIMIT);
                                continue;
                            }
                            
                            //
                            // Otherwise, we are connected to a charger but it 
                            // is taking a long time to charge. Try booting to 
                            // the main OS and charging there.
                            //
                            else
                            {
                                CHARGING_LOGGING("charge: Charge timed out %d times\r\n", ContinueLoopCounter);
                                Status = STATUS_SUCCESS;
                                goto ChargeMainEnd;
                            }
                    
                    //
                    // In all other cases, just boot to the Main OS
                    //
                    default:
                            Status = STATUS_SUCCESS;
                            goto ChargeMainEnd;
                }
                         
            }
            
            else
            {
                //
                // Will not block in charging as the charge lib was user invoked
                // This is a test to see if we are connected to a charge source 
                // or not.
                //
                Status = Charge(Threshold, StateUserInvokedChargeUntilThreshold);
                
                //
                // The Charge() call handles shutting down the device if it has
                // to. After completing the connection test, boot to the Main OS
                //
                goto ChargeMainEnd;
            }
        }

        CHARGING_LOGGING("charge: Device has enough SOC %u to boot\r\n", BatteryLevel);

        //
        // Decide if we are going to go into battery charging mode. If the 
        // function was called from a user invoked app, we will not.
        //
        if(ChargeType == StartChargeType_UserInvoked)
        {
            Status = STATUS_SUCCESS;
            goto ChargeMainEnd;
        }
                
        //
        // Check to see if a non-Default scenario is set by getting the 
        // BootFlowScenario and BootFlowOption
        //
        Status = BfGetScenario( &BootFlowScenario,  &BootFlowOption);
        if(!NT_SUCCESS(Status))
        {  
            //
            // If there is an error from the BootFlowAPI
            // it is probably safe to just continue with boot
            // and not even consider going to sticky charging
            //
            CHARGING_LOGGING("charge: ERROR BfGetScenario failed 0x%x \r\n", Status);
            goto ChargeMainEnd;
        }
        
                
        //
        // If the boot is to UpdateOS, then make sure we have enough battery.
        // Do this by adjusting the threshold and then checking for threshold
        // again
        //
        if(BfScenarioUpdateOs == BootFlowScenario)
        {
            CHARGING_LOGGING("charge: Readjusting threshold and checking again with %u\r\n", Threshold);            
            Threshold = GET_UPDATE_OS_THRESHOLD(Threshold);
            UpdateOsSkip = TRUE;
            //
            // Force rechecking of threshold conditions
            //
            continue;
        }
        
        if(BfScenarioDefault != BootFlowScenario)
        {
            goto ChargeMainEnd;
        }
        
        //
        // Checking for offline charging mode and initiating it if enabled is
        // only done if flags are not being ignored. Flags are anything that
        // prevent threshold charging from starting or anything that allows
        // further charging actions after threshold charging is done.
        //
        if (ChargeType != StartChargeType_IgnoreFlags)
        {
            //
            // Check to see if the global flag was set during previous shutdown
            //
            Status = BfGetGlobal(BfGlobalBatteryChargingMode, &BootFlowBatteryChargingMode);        
            if(!NT_SUCCESS(Status))
            {   
                // The function itself failed. Not fatal.
                CHARGING_LOGGING("charge: ERROR BfGetGlobal for BootFlowBatteryChargingMode status 0x%x", Status);
                goto ChargeMainEnd;
            }
            else if (BootFlowBatteryChargingMode == FALSE)
            {
                // Would only have been set TRUE if the previous phone shutdown was clean.
                CHARGING_LOGGING("charge: Off-mode charging is NOT enabled for this boot.\r\n");
                goto ChargeMainEnd;
            }
        
            //
            // Read the OEM setting from the BCD for Battery Charging Mode
            //
            Status = GetOemBatteryChargingMode(&BatteryChargingMode);
            if(!NT_SUCCESS(Status))
            {
                // The function itself failed. Not fatal.
                CHARGING_LOGGING("charge: ERROR GetOemBatteryChargingMode status 0x%x\r\n", Status);            
                goto ChargeMainEnd;
            }
            else if (BatteryChargingMode == FALSE)
            {
                // OEM value is not set so the feature is not enabled.
                CHARGING_LOGGING("charge: Off-mode charging is NOT enabled on this device.\r\n");
                goto ChargeMainEnd;
            }
        
            //
            // Go to Battery Charging Mode. This is a blocking call. Will not return
            // until the user presses the power button or some form of failure.
            // Failures are considered non-fatal. Please look at documentation of 
            // the function for more details. 
            // 
            Charge(CHARGE_MAX, StateBatteryChargingMode);
        
            //
            // Set BootFlowBatteryChargingMode to FALSE to avoid going into 
            // Battery Charging Mode again, and start the next iteration of this 
            // loop to test for threshold
            //
            BootFlowBatteryChargingMode = FALSE;
        }

    } while (!IS_ABOVE_THRESHOLD(Threshold, BatteryLevel));
    
    //
    // We now are guaranteed to have enough energy to boot to the Main OS
    //
    
    //
    // Make sure the battery is charging and then signal the driver to charge 
    // till CHARGE_MAX
    //
    Status = ChargeBattery(ChargeCurrent, CHARGE_MAX);
    
ChargeMainEnd:    
    
    BLOCK_CHK
    (
        if(FALSE == UpdateOsSkip)
        {
            CHARGING_LOGGING("charge: Booting to HLOS with SOC %u\r\n", BatteryLevel);
        }
        else
        {
            CHARGING_LOGGING("charge: Booting to UpdateOS with SOC %u\r\n", BatteryLevel);
        }
        
    )

    //
    // Clear the boot flow option for PowerButton from the persisted store.
    // Error is non-fatal
    //
    BfSetGlobal(BfGlobalBatteryChargingMode, FALSE);

    //
    // Dump the current system time
    //
    if (NT_SUCCESS(BlGetTime(&Time, NULL)))
    {
        CHARGING_LOGGING("charge: End System time %02d/%02d/%04d %02d:%02d:%02d:%02d \r\n", 
            Time.Month, Time.Day, Time.Year, Time.Hour, Time.Minute, Time.Second, Time.Milliseconds);
    }

#ifdef CHARGE_DEBUG_LOG
    if(!NT_SUCCESS(BootLogDestroy()))
    {
        DebugConsolePrint(L"charge: Could not destroy BootLogLib\r\n");
    }
#endif      

    //    
    // Free all allocated buffers. Errors are non-fatal as we want to continue
    // with the boot sequence and boot the main OS
    //
    DestroyChargeLibraryBuffers();
    
    return Status;
}

void DrainBattery()
{
    int Level = CHARGE_MAX;
    int Delay = 5;
    int Red = 100;

    // Start with a clear screen
    BlDisplayClearScreen();

    // Give ourselves a countdown
    while (Delay > 0)
    {
        ConsolePrint(L"Will start draining the battery in %d...\r\n", Delay);
        Sleep(SECONDS);
        Delay--;
    }

    // It ends when PMIC can't take it anymore...
    while (1)
    {
        // Populate data only for logging to the screen - it is not actually used
        UpdateBatteryStatus();

        // Render a decreasing battery, then hold red for a bit, then start full again - repeat
        while (Level > CHARGE_MIN)
        {
            DisplayUI(CHARGING_UI_NOPLUG(Level--));
        }

        while (Red > 0)
        {
            DisplayUI(BATTERY_1_RED);
            Red--;
        }

        Level = CHARGE_MAX;
        Red = 100;
    }
}
