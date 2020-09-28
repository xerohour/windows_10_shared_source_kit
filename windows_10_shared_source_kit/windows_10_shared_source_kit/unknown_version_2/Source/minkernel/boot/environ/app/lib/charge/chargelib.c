/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    chargelib.c

Abstract:

    Support functions for the charge app in EFI

Environment:

    Boot
    

--*/

#include "chargelib.h"

//
// Global to keep track of whether or not the USB protocol has been initialized
//
BOOLEAN UsbInit = FALSE;

//
// The boot device is used to query into 
//
PBOOT_ENVIRONMENT_DEVICE ChargeBootDevice = NULL;

//
// Global for Battery threshold.
// Battery levels below this should block booting to main os
// Default it max limit
//
UINT32 Threshold = BATTERY_CHARGING_BOOT_THRESHOLD_LIMIT;


NTSTATUS Initialize()
/*++

Routine Description:

    Initialize display and Battery Charging protocol

Arguments:

    None

Return Value:

    NT status code.

--*/
{
    NTSTATUS Status;
    
    //
    // Query boot device here so we can open the device & graphics file
    //
    Status = BlGetApplicationOptionDevice( BCDE_LIBRARY_TYPE_APPLICATION_DEVICE,
                                               &ChargeBootDevice,
                                               NULL);

    // Update provisioned permissions for various functionality
    UpdateProvisionedPermissions();

    //
    // Initialize the display
    //
    Status = BlpDisplayInitialize((ULONG)0);
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    ConsoleSetCursor(0, 0);
    LinesPrinted = 0;

    //
    // Initialize the graphics subsystem. This will set up the ChargeGraphics
    // data structure
    //
    Status = InitGraphicsSubsystem();
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }
    
    //
    // Initialize the Battery Charging Protocol on EFI systems. Does nothing on 
    // PCAT
    //
    Status = BatteryChargingProtocolInit();
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    return Status;

}



NTSTATUS DisplayErrorAndShutDown(DISPLAY_SCREEN DisplayScreen, UINT32 Delay)
/*++

Routine Description:

    This function will display the DisplayScreen for Delay amount of time
    and then shut down the device

Arguments:

    DisplayScreen:      The display screen to show
    
    Delay:              Amount of time to show it for. Please use macros defined
                        in chargelib.h for unit of time

Return Value:

    NT status code.

--*/
{   
    //
    // Display the UI
    //
    DisplayUI(DisplayScreen);
    
    //
    // Stall
    //
    Sleep(Delay);
    
    //
    // Shutdown. We do not expect this to fail
    //
    BlFwShutdown();

    //
    // We do not expect to reach here
    //
    return STATUS_UNSUCCESSFUL;
}


       
NTSTATUS DisplayErrorAndStall(DISPLAY_SCREEN DisplayScreen)
/*++

Routine Description:

    This will show an error screen and stall the device forever
    
Arguments:

    DisplayScreen:      The display screen to show
    
Return Value:

    NT status code.

--*/
{
    NTSTATUS Status;
    
    //
    // Ensure screen is ON
    //
    Status = TurnScreenOn();
    if (!NT_SUCCESS(Status))
    {
        goto End;
    }

    //
    // Display the screen
    //
    Status = DisplayUI(DisplayScreen);
    if(!NT_SUCCESS(Status))
    {
        goto End;
    }
    
    //
    // Stall forever
    //
    for(;;)
    {}
    
End:
    return Status;
}



NTSTATUS GetOemBatteryChargingMode(BOOLEAN *BatteryChargingMode)
/*++

Routine Description:

    The Boot Charging app supports a "Battery Charging" mode, also called
    "OFF" charging mode. This is an OEM configurable, optional, mode.
    The OEM can set a flag in the BCD to enable or disable this mode.
    
    Read the BCD flag and return the BOOLEAN
    
Arguments:

    BatteryChargingMode:        Allocated memory block that will receive the 
                                boolean
    
Return Value:

    NT status code.

--*/

{
    NTSTATUS Status;
    BOOLEAN RetVal;
    
    if(NULL == BatteryChargingMode)
    {
        return STATUS_INVALID_PARAMETER;
    }
    
    //
    // Read the flag from the BCD
    //
    Status = BlGetApplicationOptionBoolean(BCDE_OEM_CHARGING_MODE_ENABLED,
                                            &RetVal );
    //
    // If there are some errors, or if the flag is not defined, default it to
    // FALSE
    //
    if(!NT_SUCCESS(Status))
    {
        RetVal = FALSE;
    }
    
    *BatteryChargingMode = RetVal;
    
    return Status;
}



NTSTATUS GetOemBatteryBootThreshold(UINT32 *Threshold)
/*++

Routine Description:

    The threshold to boot into the main OS is configurable by the OEM as
    different devices will have different thresholds to boot into the OS
    
    Read that value from the BCD and return it
    
Arguments:

    Threshold:          Allocated block that will receive the threshold
    
Return Value:

    NT status code.

--*/
{
    NTSTATUS Status;
    ULONGLONG RetVal; 
    
    if(NULL == Threshold)
    {
        return STATUS_INVALID_PARAMETER;
    }
    
    //
    // Read the value from the BCD
    //
    Status = BlGetApplicationOptionInteger(BCDE_OEM_CHARGING_BOOT_THRESHOLD,
                                            &RetVal );
    
    //
    // If there is an error reading the flag or if it is not defined, 
    // use the default value defined within the Boot Charging app
    //
    if(!NT_SUCCESS(Status))
    {
        RetVal = BATTERY_CHARGING_BOOT_THRESHOLD;
    }
    
    //
    // For FRE builds, make sure that the threshold is not too high to avoid
    // being stuck in threshold charging mode
    //    
    BLOCK_FRE
    ( 
        *Threshold = (UINT32) RetVal; 
    )

    //
    // For CHK builds, just return the value as it is. It is useful to debug
    // threshold charging mode by setting the threshold to be very high
    //
    BLOCK_CHK
    ( 
        *Threshold = (UINT32) RetVal; 
    )
    
    return Status;
}



NTSTATUS DestroyChargeLibraryBuffers()
/*++

Routine Description:

    Free memory consumed by the various subsystems
    
Arguments:

    None
    
Return Value:

    NT status code.

--*/
{

    NTSTATUS Status = STATUS_SUCCESS;
    NTSTATUS TempStatus = STATUS_SUCCESS;
    
    //
    // Get the last error value of a series of functions.
    //
    
    TempStatus = DestroyBatterySubsystem();
    Status = TempStatus != STATUS_SUCCESS ? TempStatus : Status;
                
    TempStatus = DestroyGraphicsSubsystem();
    Status = TempStatus != STATUS_SUCCESS ? TempStatus : Status;
    

    return Status;
}

NTSTATUS UpdateProvisionedPermissions()
/*++

Routine Description:

    Updates provisioned permissions for various functionality
    and sets their corresponding flags for later use.

Arguments:

    None.

Return Value:

    NT status code.

--*/
{
    NTSTATUS Status = STATUS_SUCCESS;
    DEVICE_ID BootDeviceID = INVALID_DEVICE_ID;
    FILE_ID UEFIChargingLogToDisplayFileID = INVALID_FILE_ID;

    UEFIChargingLogToDisplay = FALSE;

    CHARGING_LOGGING("charge: Updating provisioned permissions...\r\n");

    Status = BlDeviceOpen(ChargeBootDevice, OPEN_READ_WRITE, &BootDeviceID);
    if (!NT_SUCCESS(Status))
    {
        CHARGING_LOGGING("charge: Boot device not found\r\n");
        goto UpdateProvisionedPermissionsEnd;
    }

    // Check for permission to show certain helpful log messages on the phone's screen
    Status = BlFileOpen(BootDeviceID, UEFI_CHARGING_LOG_TO_DISPLAY, OPEN_READ, &UEFIChargingLogToDisplayFileID);
    if (!NT_SUCCESS(Status))
    {
        CHARGING_LOGGING("charge: UEFIChargingLogToDisplay.txt not found\r\n");
        UEFIChargingLogToDisplay = FALSE;
    }
    else
    {
        CHARGING_LOGGING("charge: Found UEFIChargingLogToDisplay.txt\r\n");
        UEFIChargingLogToDisplay = TRUE;
    }

UpdateProvisionedPermissionsEnd:

    //
    // Log all results, close all file handles that were successfully opened,
    // then close the boot device handle if it was successfully opened.
    //

    CHARGING_LOGGING("charge: UEFIChargingLogToDisplay %d\r\n", UEFIChargingLogToDisplay);

    if (UEFIChargingLogToDisplayFileID != INVALID_FILE_ID)
    {
        BlFileClose(UEFIChargingLogToDisplayFileID);
    }

    if (BootDeviceID != INVALID_DEVICE_ID)
    {
        BlDeviceClose(BootDeviceID);
    }

    return Status;
}
