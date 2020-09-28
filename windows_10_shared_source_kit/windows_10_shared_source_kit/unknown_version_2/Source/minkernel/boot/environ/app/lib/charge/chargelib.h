/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    chargelib.h

Abstract:

    Internal header for the Boot Battery Charging application.

Environment:

    Boot    
    
--*/
#pragma once

#include <bootlib.h>
#include <BootFlowAPI.h>
#include <BootLogLib.h>
#include <blctxp.h>

//
// Boot Charging Application Subsystems
//
#include "usbfn.h"
#include "battery.h"
#include "graphics.h"

#define CHARGE_DEBUG_LOG

#ifdef CHARGE_DEBUG_LOG
#define ChargeLogPrint               BootLogPrint
#else
#define ChargeLogPrint(...)               ((void) 0)
#endif

BOOLEAN UEFIChargingLogToDisplay;
BOOLEAN RenderTimeExpired;

// Only going to print a specific number of lines
int LinesPrinted;
#define MAX_LINES_TO_PRINT 12

// Take parameters in same order as used by BlDisplaySetCursorPosition for
// consistency, but SIMPLE_TEXT_OUTPUT_INTERFACE.SetCursorPosition takes them
// in swapped. As this is a macro, all the symbols it contains will be found
// at run time just fine, as long as the right header files are included.
#define EfiSetCursor(_VR, _HC)                                        \
    if (BlpArchQueryCurrentContextType() != ExecutionContextFirmware) \
    {                                                                 \
        BlpArchSwitchContext(ExecutionContextFirmware);               \
        EfiConOut->SetCursorPosition(EfiST->ConOut, (_HC), (_VR));    \
        BlpArchSwitchContext(ExecutionContextApplication);            \
    }                                                                 \
    else                                                              \
    {                                                                 \
        EfiConOut->SetCursorPosition(EfiST->ConOut, (_HC), (_VR));    \
    }

#define PcatSetCursor(_VR, _HC) ((void) 0)

#ifdef ConsoleSetCursor
#undef ConsoleSetCursor
#endif

#ifdef EFI
#define ConsoleSetCursor EfiSetCursor
#endif

#ifdef PCAT
#define ConsoleSetCursor PcatSetCursor
#endif

#define LOG_TO_DISPLAY(__S__,...)                                        \
    if (RenderTimeExpired == FALSE && UEFIChargingLogToDisplay != FALSE) \
    {                                                                    \
        if (++LinesPrinted > MAX_LINES_TO_PRINT)                         \
        {                                                                \
            LinesPrinted = 0;                                            \
            ConsoleSetCursor(0, 0);                                      \
            BlDisplayClearScreen();                                      \
        }                                                                \
        ConsolePrint(L ## __S__, __VA_ARGS__);                           \
    }

#define CHARGING_LOGGING(__S__,...) \
    DebugConsolePrint(L ## __S__, __VA_ARGS__); \
    ChargeLogPrint(__S__, __VA_ARGS__); \
    LOG_TO_DISPLAY(__S__, __VA_ARGS__)

#define BOOLIFY(expr)           (!!(expr))

#define MILLI_SECONDS           ( 1000 )
#define SECONDS                 ( 1000 * MILLI_SECONDS )


//
// The log file
//
#define CHARGE_LOG_FILE_NAME           L"\\Windows\\System32\\CHARGELOG.txt"

//
// Functionality files
//
#define UEFI_CHARGING_LOG_TO_DISPLAY    L"\\Windows\\System32\\boot\\UEFIChargingLogToDisplay.txt"

// ----------------------------------------------------------------------------
//
//  Charge Library Function Prototypes
//
// ----------------------------------------------------------------------------

NTSTATUS Initialize();

NTSTATUS DisplayErrorAndShutDown(DISPLAY_SCREEN DisplayScreen, UINT32 Delay);

NTSTATUS DisplayErrorAndStall(DISPLAY_SCREEN DisplayScreen);

NTSTATUS GetConnectedToPowerSource(BOOLEAN *ConnectedToPowerSource);

NTSTATUS GetOemBatteryChargingMode(BOOLEAN *BatteryChargingMode);

NTSTATUS GetOemBatteryBootThreshold(UINT32 *Threshold);

NTSTATUS DestroyChargeLibraryBuffers();

NTSTATUS UpdateProvisionedPermissions();
