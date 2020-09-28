/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    trace.h

Abstract:

    This file contains prototypes and definitions related to debugging.


Environment:

    Kernel mode

--*/

#pragma once

#include <evntrace.h> // For TRACE_LEVEL definitions

#if defined(EVENT_TRACING)

#define WPP_CHECK_FOR_NULL_STRING  // to prevent exceptions due to NULL strings.

//
// WPP_DEFINE_CONTROL_GUID specifies the GUID used for this driver.
//

#define WPP_CONTROL_GUIDS \
    WPP_DEFINE_CONTROL_GUID(GpioTraceGuid,(8d83ba5c,e85e,4859,b18e,314ba4475a12), \
        WPP_DEFINE_BIT(DBG_INIT)             /* bit  0 = 0x00000001 */ \
        WPP_DEFINE_BIT(DBG_PNP)              /* bit  1 = 0x00000002 */ \
        WPP_DEFINE_BIT(DBG_POWER)            /* bit  2 = 0x00000004 */ \
        WPP_DEFINE_BIT(DBG_WMI)              /* bit  3 = 0x00000008 */ \
        WPP_DEFINE_BIT(DBG_CREATE_CLOSE)     /* bit  4 = 0x00000010 */ \
        WPP_DEFINE_BIT(DBG_IOCTL)            /* bit  5 = 0x00000020 */ \
        WPP_DEFINE_BIT(DBG_IO)               /* bit  6 = 0x00000040 */ \
        WPP_DEFINE_BIT(DBG_INTERRUPT)        /* bit  7 = 0x00000080 */ \
        WPP_DEFINE_BIT(DBG_MASK_UNMASK)      /* bit  8 = 0x00000100 */ \
        WPP_DEFINE_BIT(DBG_CLIENT)           /* bit  9 = 0x00000200 */ \
        WPP_DEFINE_BIT(DBG_ACPIEVT)          /* bit 10 = 0x00000400 */ \
        WPP_DEFINE_BIT(DBG_FUNCCONFIG)       /* bit 11 = 0x00000800 */ \
        )

#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags) WPP_LEVEL_LOGGER(flags)
#define WPP_LEVEL_FLAGS_ENABLED(lvl, flags)                                 \
    (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level  >= lvl)

#else

//
// Define Debug Flags.
//

#define DBG_INIT                0x00000001
#define DBG_PNP                 0x00000002
#define DBG_POWER               0x00000004
#define DBG_WMI                 0x00000008
#define DBG_CREATE_CLOSE        0x00000010
#define DBG_IOCTL               0x00000020
#define DBG_IO                  0x00000040
#define DBG_INTERRUPT           0x00000080
#define DBG_MASK_UNMASK         0x00000100
#define DBG_CLIENT              0x00000200
#define DBG_ACPIEVT             0x00000400
#define DBG_FUNCCONFIG          0x00000800

VOID
TraceEvents (
    __in HANDLE LogHandle,
    __in ULONG DebugPrintLevel,
    __in ULONG DebugPrintFlag,
    __drv_formatString(printf) __in PCSTR DebugMessage,
    ...
    );

#define WPP_INIT_TRACING(DriverObject, RegistryPath)
#define WPP_CLEANUP(DriverObject)

#endif

//
// Friendly names for various trace levels.
//

typedef enum _INTERNAL_TRACE_LEVELS {
    Error = TRACE_LEVEL_ERROR,
    Warning = TRACE_LEVEL_WARNING,
    Info = TRACE_LEVEL_INFORMATION,
    Verbose = TRACE_LEVEL_VERBOSE
} INTERNAL_TRACE_LEVELS, *PINTERNAL_TRACE_LEVELS;


