/*++

Copyright (c) Microsoft Corporation

Module Name:

    dbg.h

Abstract:


Environment:


Revision History:

--*/

#define TL_NONE 0x00
#define TL_ALL 0xf0

// begin_wpp config
// USEPREFIX (LogVerbose, "%!STDPREFIX!");
// LogVerbose{LEVEL=TRACE_LEVEL_VERBOSE}(FLAGS,MSG,...);
// end_wpp

// begin_wpp config
// USEPREFIX (LogError, "%!STDPREFIX!!!");
// LogError{LEVEL=TRACE_LEVEL_ERROR}(FLAGS,MSG,...);
// end_wpp

// begin_wpp config
// USEPREFIX (LogWarning, "%!STDPREFIX!!");
// LogWarning{LEVEL=TRACE_LEVEL_WARNING}(FLAGS,MSG,...);
// end_wpp

// begin_wpp config
// USEPREFIX (LogInfo, "%!STDPREFIX!");
// LogInfo{LEVEL=TRACE_LEVEL_INFORMATION}(FLAGS,MSG,...);
// end_wpp

// begin_wpp config
// USEPREFIX (LogFatal, "%!STDPREFIX!!!!!!");
// LogFatal{LEVEL=TRACE_LEVEL_FATAL}(FLAGS,MSG,...);
// end_wpp

//#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags) WPP_LEVEL_LOGGER(flags)
//#define WPP_LEVEL_FLAGS_ENABLED(lvl,flags) (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)



// begin_wpp config
// CUSTOM_TYPE(UCX_IOCTL, ItemEnum(_UCX_IOCTL));
// end_wpp

