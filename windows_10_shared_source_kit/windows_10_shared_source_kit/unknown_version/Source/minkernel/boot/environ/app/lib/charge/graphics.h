//
// Copyright (C) Microsoft. All rights reserved.
//

#pragma once
#include <bldisplay.h>

#include <xfboot.h>


NTSTATUS BlpDisplayInitialize (ULONG Flags);

//
// This enum is used to index into the CHARGE_IMAGE array
//
typedef enum _DISPLAY_SCREEN 
{
    BATTERY_0 = 0,          // 0
    BATTERY_PLUG_0,         // 1
    BATTERY_1,              // 2
    BATTERY_PLUG_1,         // 3
    BATTERY_2,              // 4
    BATTERY_PLUG_2,         // 5
    BATTERY_3,              // 6
    BATTERY_PLUG_3,         // 7
    BATTERY_4,              // 8
    BATTERY_PLUG_4,         // 9
    BATTERY_5,              // 10
    BATTERY_PLUG_5,         // 11
    BATTERY_6,              // 12
    BATTERY_PLUG_6,         // 13
    BATTERY_7,              // 14
    BATTERY_PLUG_7,         // 15
    BATTERY_8,              // 16
    BATTERY_PLUG_8,         // 17
    BATTERY_9,              // 18
    BATTERY_PLUG_9,         // 19
    BATTERY_10,             // 20
    BATTERY_PLUG_10,        // 21
    BATTERY_11,             // 22
    BATTERY_PLUG_11,        // 23
    BATTERY_1_RED,          // 24
    BATTERY_PLUG_1_RED,     // 25
    BATTERY_UNKNOWN,        // 26
    BATTERY_LAST            // 27

} DISPLAY_SCREEN;

//
// This stores the Path of an image and its in memory buffer
//
typedef struct _CHARGE_IMAGE
{
    PWSTR Path;
    PVOID Buffer;
    
} CHARGE_IMAGE;



#define     GRAPHICS_DIR                    L"\\Windows\\System32\\boot\\ui\\boot.battery."
#define     GRAPHICS_FILE_EXT               L".bmpx"
#define     GRAPHICS_PATH(__x__)            GRAPHICS_DIR __x__ GRAPHICS_FILE_EXT

#define     SET_IMAGE(_w_, _x_, _y_)       { _w_[_x_].Path = GRAPHICS_PATH(_y_); _w_[_x_].Buffer = NULL; }


//
// Time to delay the BATTERY_UNKNOWN screen before shutdown
//
#define BATTERY_ERROR_DELAY             ( 8 * SECONDS )

//
// Time to show the Low Battery UI before turning off the device
//
#define LOW_BATTERY_TURNOFF_DELAY       ( 5 * SECONDS )


//
// Time delay between alternating images
//
#define ANIMATION_DELAY                 ( 1 * SECONDS )

//
// Screen timeout
//
#define SCREEN_TIMEOUT_DELAY            ( 10 * SECONDS )


NTSTATUS DisplayInfo();

NTSTATUS DisplayUI(DISPLAY_SCREEN Screen);

NTSTATUS DisplayAlternatingUI(DISPLAY_SCREEN ScreenA, DISPLAY_SCREEN ScreenB);

NTSTATUS DisplayControl();

NTSTATUS InitGraphicsSubsystem();

NTSTATUS DestroyGraphicsSubsystem();

NTSTATUS TurnScreenOn();

NTSTATUS TurnScreenOff();
