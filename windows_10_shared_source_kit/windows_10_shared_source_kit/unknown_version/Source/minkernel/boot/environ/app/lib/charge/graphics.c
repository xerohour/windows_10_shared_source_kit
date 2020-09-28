/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    graphics.c

Abstract:

    Support functions for the charge app in EFI

Environment:

    Boot
    

--*/

#include "chargelib.h"   
#include "graphics.h"
#include <displaylib.h>

//
// ChargeBootDevice required to open handle to the graphics files
//
extern PBOOT_ENVIRONMENT_DEVICE ChargeBootDevice;

//
// Global variables that define the current alternating screens
//
DISPLAY_SCREEN ScreenA = BATTERY_0;
DISPLAY_SCREEN ScreenB = BATTERY_0;       
extern BOOLEAN RenderTimeExpired;
extern LONGLONG ScreenTimeOutSeconds;
extern LARGE_INTEGER CurrentTick;

//
// Screen display and on/off stuff
//
extern LARGE_INTEGER ScreenTurnOnTick;    
BOOLEAN DisplayPowerStateProtocolLoaded = FALSE;

//
// Also used to open and close the handle to the graphics files
//
DEVICE_ID DeviceId = INVALID_DEVICE_ID;

//
// Screen resolution
//
ULONG HResolution = 0;
ULONG VResolution = 0;          

//
// DISPLAY_SCREEN is used as index to get the path of the image, 
// or its associated in-memory buffer
//
CHARGE_IMAGE ChargeGraphics[BATTERY_LAST];
    
DISPLAY_SCREEN CurrentAlternatingScreen = BATTERY_UNKNOWN;
extern UINT32 BatteryLevel;
extern INT32 CurrentGoingIntoBattery;
extern UINT32 VoltageAcrossBatteryTerminals;
extern UINT32 ChargingProtocolRevision;
extern INT32 TemperatureOfBattery;
extern UINT32 VoltageOverUSBCable;
extern UINT32 CurrentThroughUSBCable;

NTSTATUS DisplayBuffer(PVOID Buffer)
/*++

Routine Description:

    Display the image buffer, centered, on the console

Arguments:

    Buffer:     Pre-allocated buffer with an BITMAP_IMAGE with a 
                BITMAP_IMAGE_PROLOG

Return Value:

    NT status code.

--*/
{
    NTSTATUS Status;
    PBITMAP_IMAGE Image;
    ULONG XCoord = 0;
    ULONG YCoord = 0;
    CONSOLE_COLOR BackgroundColor;
    
    if(NULL == Buffer)
    {
        return STATUS_INVALID_PARAMETER;
    }
        
    //
    // Get screen resolution and cache it in local variable to avoid repeated
    // calls to the function
    //
    if(0 == HResolution || 0 == VResolution)
    {
        Status = BlDisplayGetOriginalResolution(&HResolution, &VResolution);
        if(!NT_SUCCESS(Status))
        {
            return Status;
        }
    }
    
    //
    // Retrieve the size of the image
    //
    Image = (PBITMAP_IMAGE) ( Add2Ptr(Buffer, sizeof(BITMAP_IMAGE_PROLOG)));
    
    //
    // Center the X and Y coordinates based on the size of the image and the 
    // resolution of the screen
    //
    XCoord = (HResolution - Image->Width) >> 1;
    YCoord = (VResolution - Image->Height) >> 1;
    
    //
    // Suppress graphics if compiled with DEBUG_SUPPRESS_GRAPHCIS
    //
#ifdef DEBUG_SUPPRESS_GRAPHICS
    if(HResolution)
        return 0;
#endif
    
    // 
    // Set background color to black.
    //
    BackgroundColor = BLACK;
    Status = BlDisplaySetBackgroundColor(BackgroundColor);
    if (!NT_SUCCESS(Status))
    {
        // An error here is not fatal.
        CHARGING_LOGGING("charge: Setting backround color to %d failed with 0x%x\r\n", BackgroundColor, Status);
    }
    
    return BlDisplayDrawBitmap(Buffer, XCoord, YCoord,TRUE );
}



NTSTATUS ReadFileAndUncompress( __in PWSTR PathToImage, PVOID *OutputBuffer)
/*++

Routine Description:

    Read the contents of an images compressed with XpressFiles desktop tool,
    allocates a buffer for the OutputBuffer and fills it up with the expanded
    file contents

    N.B The callee is responsible to free OuputBuffer
    
Arguments:
    
    PathToImage:        Path to the compressed image. Relative to the EFIESP
                        partition
    
    OutputBuffer:       Pointer to a PVOID - this will point to a block of 
                        memory allocated in this function. Callee is responsible
                        to free it

Return Value:

    NT status code.

--*/
{
    NTSTATUS Status;
    
    FILE_ID FileId = INVALID_FILE_ID;
    PVOID FileBuffer = NULL;
    FILE_INFORMATION FileInformation;    
    ULONG FileSize;
    
    if(OutputBuffer == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }
    
    *OutputBuffer = NULL;
    
    //
    // Open the device 
    //
    Status = BlDeviceOpen(ChargeBootDevice,  OPEN_READ_WRITE, &DeviceId);
    if (!NT_SUCCESS(Status) && DeviceId != INVALID_DEVICE_ID)
    {
        CHARGING_LOGGING("charge: BlDeviceOpen failed with 0x%x\r\n", Status);
        return Status;
    }
    
    //
    // Open a handle to the file
    //
    Status = BlFileOpen(DeviceId, PathToImage, OPEN_READ, &FileId);
    if(!NT_SUCCESS(Status))
    {   
        CHARGING_LOGGING("charge: BlFileOpen failed with 0x%x\r\n", Status);
        return Status;
    }

    //
    // Fill up the FileInformation structure
    //
    Status = BlFileGetInformation(FileId, &FileInformation);
    if(!NT_SUCCESS(Status))
    {
        CHARGING_LOGGING("charge: BlFileGetInformation failed with  0x%x\r\n", Status);
        return Status;
    }

    //
    // Get the file size so that we can allocate a large enough buffer to 
    // read in the compressed file
    //
    FileSize = (ULONG)FileInformation.FileSize;

    //
    // Allocate the file buffer
    //
    FileBuffer = BlMmAllocateHeap( FileSize );
    if(NULL == FileBuffer)
    {
        CHARGING_LOGGING("charge: BlMmAllocateHeap failed with  0x%x\r\n", Status);
        return STATUS_NO_MEMORY;
    }

    //
    // Read the compressed file into a local buffer
    //
    Status = BlFileRead(FileId, FileBuffer, FileSize, NULL);
    if(!NT_SUCCESS(Status))
    {
        CHARGING_LOGGING("charge: BlFileRead failed with  0x%x\r\n", Status);
        return Status;
    }
    
    //
    // Call the BootXpressLib function to uncompress the buffer, and 
    // point OutputBuffer to this memory block. OutputBuffer will 
    // point to the uncompressed bytes
    //
    Status = BlXfUncompress(FileBuffer, FileSize,  OutputBuffer);
    if(!NT_SUCCESS(Status))
    {
        CHARGING_LOGGING("charge: BlXfUncompress failed with  0x%x\r\n", Status);
        return Status;
    }

    return Status;
}


NTSTATUS DisplayUI(DISPLAY_SCREEN Screen)
/*++

Routine Description:

    Display's the UI pointed to by Screen parameter. The parameter will be used
    to index into the ChargeGraphics array and then display its associated 
    buffer OR read in the compressed file, uncompress it, and then display
    the associated buffer
    
Arguments:
    
    Screen:         Index into the ChargeGraphics array
Return Value:

    NT status code.

--*/
{
    NTSTATUS Status;
    
    // Ensure the screen is on
    Status = TurnScreenOn();
    if (!NT_SUCCESS(Status))
    {
        CHARGING_LOGGING("charge: Fatal - Failed to turn screen ON before graphics display: 0x%x\r\n", Status);
        return Status;
    }

    //
    // Index into the ChargeGraphics array which is a list of CHARGE_IMAGE
    // If the buffer is NULL, that means we haven't loaded in and uncompressed
    // the associated graphic. If that is the case, fill up the buffer, and 
    // then display the image
    //
    if (NULL == ChargeGraphics[Screen].Buffer)
    {    
        CHARGING_LOGGING("charge: Charge bitmap cache miss for screen %d\r\n", Screen);
        Status = ReadFileAndUncompress(ChargeGraphics[Screen].Path, 
                                        &ChargeGraphics[Screen].Buffer);
        if(!NT_SUCCESS(Status))
        {
            return Status;
        }
    }
    
    //
    // Log information to the screen if the feature is enabled
    //
    switch (ChargingProtocolRevision)
    {
    case EFI_BATTERY_CHARGING_PROTOCOL_REVISION2:
        LOG_TO_DISPLAY("SOC:%u CIB:%dmA VABT:%umV TOB:%d VUSB:%umV CUSB:%umA\r\n",
                BatteryLevel, CurrentGoingIntoBattery, VoltageAcrossBatteryTerminals,
                TemperatureOfBattery, VoltageOverUSBCable, CurrentThroughUSBCable);
        break;
    case EFI_BATTERY_CHARGING_PROTOCOL_REVISION1:
        __fallthrough;
    default:
        LOG_TO_DISPLAY("SOC:%u, CIB:%dmA\r\n", BatteryLevel, CurrentGoingIntoBattery);
    }

    // 
    // We have the buffer filled up with the image. Display it
    //
    Status = DisplayBuffer(ChargeGraphics[Screen].Buffer);
    
    return Status;
}



NTSTATUS DisplayAlternatingUI(DISPLAY_SCREEN ScreenA, DISPLAY_SCREEN ScreenB)
/*++

Routine Description:

    Each call to this function will alternate the value of 
    CurrentAlternatingScreen, and hence, the image thats being shown on the 
    display will alternate
    
Arguments:
    
    ScreenA:         First screen 
    ScreenB:         Second screen
    
Return Value:

    NT status code.

--*/
{
    //
    // Check to see if this is the first call with these arguments.
    // If it is not, then set the current screen to the SECOND screen because
    // the following logic will alternate it to the FIRST screen.
    //
    if( CurrentAlternatingScreen != ScreenA && CurrentAlternatingScreen != ScreenB)
    {
        CurrentAlternatingScreen = ScreenB;
    }
    
    //
    // Get the next screen
    //
    if(ScreenA == CurrentAlternatingScreen)
    {
        CurrentAlternatingScreen = ScreenB;
    }
    else
    {
        CurrentAlternatingScreen = ScreenA;
    }
    
    //
    // Display the screen
    //
    return DisplayUI(CurrentAlternatingScreen);
}



NTSTATUS DisplayControl()
{
    //
    // Only render if the screen has not timed out
    //
    if ((CurrentTick.QuadPart - ScreenTurnOnTick.QuadPart) >= ScreenTimeOutSeconds )
    {
        //
        // If the screen has already been cleared and turned OFF, short circuit
        //
        if(RenderTimeExpired)
        {
            return STATUS_SUCCESS;
        }
        
        //
        // Otherwise clear the screen, turn it OFF, set the flag for next time, and return
        //
        BlDisplayClearScreen();
        TurnScreenOff();
        RenderTimeExpired = TRUE;
        return STATUS_SUCCESS;
    }
    
    //
    // Otherwise, display the alternating UI
    RenderTimeExpired = FALSE;
    return DisplayAlternatingUI(ScreenA, ScreenB);
}


NTSTATUS InitGraphicsSubsystem()
{
    NTSTATUS Status = STATUS_SUCCESS;

    RenderTimeExpired = FALSE;

    SET_IMAGE(ChargeGraphics, BATTERY_0,            L"0");
    SET_IMAGE(ChargeGraphics, BATTERY_PLUG_0,       L"pluggedin.0");
    SET_IMAGE(ChargeGraphics, BATTERY_1,            L"1");
    SET_IMAGE(ChargeGraphics, BATTERY_PLUG_1,       L"pluggedin.1");
    SET_IMAGE(ChargeGraphics, BATTERY_2,            L"2");
    SET_IMAGE(ChargeGraphics, BATTERY_PLUG_2,       L"pluggedin.2");
    SET_IMAGE(ChargeGraphics, BATTERY_3,            L"3");
    SET_IMAGE(ChargeGraphics, BATTERY_PLUG_3,       L"pluggedin.3");
    SET_IMAGE(ChargeGraphics, BATTERY_4,            L"4");
    SET_IMAGE(ChargeGraphics, BATTERY_PLUG_4,       L"pluggedin.4");
    SET_IMAGE(ChargeGraphics, BATTERY_5,            L"5");
    SET_IMAGE(ChargeGraphics, BATTERY_PLUG_5,       L"pluggedin.5");
    SET_IMAGE(ChargeGraphics, BATTERY_6,            L"6");
    SET_IMAGE(ChargeGraphics, BATTERY_PLUG_6,       L"pluggedin.6");
    SET_IMAGE(ChargeGraphics, BATTERY_7,            L"7");
    SET_IMAGE(ChargeGraphics, BATTERY_PLUG_7,       L"pluggedin.7");
    SET_IMAGE(ChargeGraphics, BATTERY_8,            L"8");
    SET_IMAGE(ChargeGraphics, BATTERY_PLUG_8,       L"pluggedin.8");
    SET_IMAGE(ChargeGraphics, BATTERY_9,            L"9");
    SET_IMAGE(ChargeGraphics, BATTERY_PLUG_9,       L"pluggedin.9");
    SET_IMAGE(ChargeGraphics, BATTERY_10,           L"10");
    SET_IMAGE(ChargeGraphics, BATTERY_PLUG_10,      L"pluggedin.10");
    SET_IMAGE(ChargeGraphics, BATTERY_11,           L"11");
    SET_IMAGE(ChargeGraphics, BATTERY_PLUG_11,      L"pluggedin.11");
    SET_IMAGE(ChargeGraphics, BATTERY_1_RED,        L"1.red");
    SET_IMAGE(ChargeGraphics, BATTERY_PLUG_1_RED,   L"pluggedin.1.red");    
    SET_IMAGE(ChargeGraphics, BATTERY_UNKNOWN,      L"unknown");

    // Load the protocol responsible for turning the display on or off
    DisplayPowerStateProtocolLoaded = FALSE;
    Status = InitializeDisplayPowerStateProtocol();
    if (NT_SUCCESS(Status))
    {
        CHARGING_LOGGING("charge: DisplayPowerState protocol successfully loaded\r\n");
        DisplayPowerStateProtocolLoaded = TRUE;
        Status = STATUS_SUCCESS;
    }
    else if (Status == STATUS_NOT_IMPLEMENTED)
    {
        // Failure is not fatal. The functionality will simply be missing.
        CHARGING_LOGGING("charge: Warning - DisplayPowerState protocol not implemented: 0x%x\r\n", Status);
        Status = STATUS_SUCCESS;
    }
    else
    {
        // Anything else is fatal.
        CHARGING_LOGGING("charge: FATAL - DisplayPowerState protocol failed to load: 0x%x\r\n", Status);
    }

    return Status;
}


NTSTATUS DestroyGraphicsSubsystem()
/*++

Routine Description:

    Free all associated buffers
    
Arguments:
    
    None
    
Return Value:

    NT status code.

--*/
{
    NTSTATUS Status = STATUS_SUCCESS;
    DISPLAY_SCREEN Screen = BATTERY_10;
    
    // Unload the display power state protocol if it was ever successfully loaded
    if (DisplayPowerStateProtocolLoaded)
    {
        DestroyDisplayPowerStateProtocol();
        DisplayPowerStateProtocolLoaded = FALSE;
    }

    //
    // Free the image buffers if they are allocated
    //
    for(Screen = 0; Screen < BATTERY_LAST; Screen++)
    {
        if(NULL != ChargeGraphics[Screen].Buffer)
        {
            //
            // The buffer is allocated using BlMmAllocateHeap
            //
            BlMmFreeHeap(ChargeGraphics[Screen].Buffer);
        }
    }
    return Status;
}


NTSTATUS TurnScreenOn()
{
    NTSTATUS Status = STATUS_SUCCESS;

    // If the protocol is not loaded just return with success
    if (DisplayPowerStateProtocolLoaded == FALSE)
    {
        goto TurnScreenOnEnd;
    }
    
    Status = SetDisplayPowerState(DisplayPowerStateMaximum);
    if (!NT_SUCCESS(Status))
    {
        CHARGING_LOGGING("charge: FATAL - Failed to turn screen ON: 0x%x\r\n", Status);
        goto TurnScreenOnEnd;
    }

TurnScreenOnEnd:

    return Status;
}


NTSTATUS TurnScreenOff()
{
    NTSTATUS Status = STATUS_SUCCESS;
    
    // If the protocol is not loaded just return with success
    if (DisplayPowerStateProtocolLoaded == FALSE)
    {
        goto TurnScreenOffEnd;
    }
    
    Status = SetDisplayPowerState(DisplayPowerStateOff);
    if (!NT_SUCCESS(Status))
    {
        CHARGING_LOGGING("charge: FATAL - Failed to turn screen OFF: 0x%x\r\n", Status);
        goto TurnScreenOffEnd;
    }

TurnScreenOffEnd:

    return Status;
}
