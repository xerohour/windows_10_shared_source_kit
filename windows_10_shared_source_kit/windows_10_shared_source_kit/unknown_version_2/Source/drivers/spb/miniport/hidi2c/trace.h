//
//    Copyright (C) Microsoft.  All rights reserved.
//
/*++

Module Name:

    trace.h

Abstract:

    Header file for the debug tracing related function defintions and macros.

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel mode

--*/

#ifndef _TRACE_H
#define _TRACE_H

#define TELEMETRY_EVENT_PROVIDER_NAME		"Microsoft.Windows.Input.HidI2c"

EXTERN_C
//
// Define the tracing flags.
//
// Tracing GUID - E742C27D-29B1-4E4B-94EE-074D3AD72836
//

#define WPP_CONTROL_GUIDS                                               \
    WPP_DEFINE_CONTROL_GUID(                                            \
        HidI2CTraceGuid,                                                \
        (E742C27D,29B1,4E4B,94EE,074D3AD72836),                         \
        WPP_DEFINE_BIT(TRACE_FLAG_INIT)                                 \
        WPP_DEFINE_BIT(TRACE_FLAG_ACPI)                                 \
        WPP_DEFINE_BIT(TRACE_FLAG_SPB)                                  \
        WPP_DEFINE_BIT(TRACE_FLAG_HID)                                  \
        WPP_DEFINE_BIT(TRACE_FLAG_IOCTL)                                \
        WPP_DEFINE_BIT(TRACE_FLAG_INTERRUPT)                            \
        WPP_DEFINE_BIT(TRACE_FLAG_POWER)                                \
        WPP_DEFINE_BIT(TRACE_FLAG_REGISTRY)                             \
        )

// begin_wpp config
// FUNC FuncEntry{LEVEL=TRACE_LEVEL_VERBOSE}(FLAGS);
// FUNC FuncExit{LEVEL=TRACE_LEVEL_VERBOSE}(FLAGS);
// USEPREFIX(FuncEntry, "%!STDPREFIX! [%!FUNC!] --> entry");
// USEPREFIX(FuncExit, "%!STDPREFIX! [%!FUNC!] <-- exit");
// end_wpp

#endif