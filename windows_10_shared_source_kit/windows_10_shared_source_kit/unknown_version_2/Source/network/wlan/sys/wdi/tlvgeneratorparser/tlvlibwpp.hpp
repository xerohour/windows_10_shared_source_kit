//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once
#ifndef WDILIB_WPP_H
#define WDILIB_WPP_H

#if WDI_DISABLED
// WPP Recorder is not available to IHVs
extern "C"
{
#include <WppRecorder.h>
}
#endif // WDI_DISABLED

#define WPP_USE_TRACE_LEVELS

// f3486b27-31d7-4465-b333-f851e60f6d4b
#define WPP_CONTROL_GUIDS                                               \
    WPP_DEFINE_CONTROL_GUID(                                            \
        WdiLibraryCtlGuid,(f3486b27,31d7,4465,b333,f851e60f6d4b),        \
        WPP_DEFINE_BIT(DUMMY)                                            \
    )

#define WPP_LEVEL_EXP_ENABLED(LEVEL, EXP) WPP_LEVEL_ENABLED(LEVEL)
#define WPP_LEVEL_EXP_LOGGER(LEVEL, EXP) WPP_LEVEL_LOGGER(LEVEL)


#define WPP_RECORDER_LEVEL_FLAGS_ARGS(lvl, flags) WPP_CONTROL(WPP_BIT_ ## flags).AutoLogContext, lvl, WPP_BIT_ ## flags
#define WPP_RECORDER_LEVEL_FLAGS_FILTER(lvl,flags) (WPP_LEVEL_ENABLED(lvl) || lvl < TRACE_LEVEL_VERBOSE || WPP_CONTROL(WPP_BIT_ ## flags).AutoLogVerboseEnabled)

#define WPP_RECORDER_LEVEL_ARGS(LEVEL) WPP_RECORDER_LEVEL_FLAGS_ARGS(LEVEL,DUMMY)
#define WPP_RECORDER_LEVEL_FILTER(LEVEL) WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL,DUMMY)

#define WPP_RECORDER_LEVEL_EXP_FILTER(LEVEL,EXP) WPP_RECORDER_LEVEL_FILTER(LEVEL)
#define WPP_RECORDER_LEVEL_EXP_ARGS(LEVEL,EXP) WPP_RECORDER_LEVEL_ARGS(LEVEL)

// Suppress warnings about constants in logical expressions because the
// level is often a constant
#define WPP_LEVEL_PRE(LEVEL)         __pragma(warning(suppress:25039 25040))
#define WPP_LEVEL_EXP_PRE(LEVEL,EXP) __pragma(warning(suppress:25039 25040))


// begin_wpp config
// USEPREFIX (TraceEntry, "%!STDPREFIX!");
// FUNC TraceEntry{LEVEL=TRACE_LEVEL_VERBOSE}(...);
// USESUFFIX (TraceEntry, "--> %!FUNC!");
// end_wpp


// begin_wpp config
// USEPREFIX (TraceExit, "%!STDPREFIX!");
// FUNC TraceExit{LEVEL=TRACE_LEVEL_VERBOSE}(EXP);
// USESUFFIX (TraceExit, "<-- %!FUNC!: 0x%x", EXP);
// end_wpp

//
// Flat-C trace commands
//
// begin_wpp config
//
// USEPREFIX (WFCError, "%!STDPREFIX! [ERROR]");
// FUNC WFCError{LEVEL=TRACE_LEVEL_ERROR}(MSG, ...);
//
// USEPREFIX (WFCInfo, "%!STDPREFIX! [INFO]");
// FUNC WFCInfo{LEVEL=TRACE_LEVEL_INFORMATION}(MSG, ...);
//
// USEPREFIX (WFCTrace, "%!STDPREFIX! [TRACE]");
// FUNC WFCTrace{LEVEL=TRACE_LEVEL_VERBOSE}(MSG, ...);
//
// end_wpp
//

typedef struct _ByteArray {
    USHORT usLength;
    const void * pvBuffer;
} ByteArray;

__inline ByteArray
log_lenstr(ULONG len, const void* buf)
{
    ByteArray xs = {0};
    xs.usLength = (USHORT)len;
    xs.pvBuffer = buf;
    return xs;
}

#define WPP_LOGHEXDUMP(x) WPP_LOGPAIR(2, &((x).usLength)) \
    WPP_LOGPAIR((x).usLength, (x).pvBuffer)

#define WPP_LOGANSISTRING(x) WPP_LOGPAIR(2, &((x).usLength)) \
    WPP_LOGPAIR((x).usLength, (x).pvBuffer)

#define WPP_LOGMACADDR(x)   WPP_LOGPAIR(6,x)

#define WPP_LOGDOT11SSID(x) WPP_LOGPAIR(2, &((*(x)).uSSIDLength)) \
    WPP_LOGPAIR((*(x)).uSSIDLength,((const char *)(*(x)).ucSSID))

//
// Custom types
//
// begin_wpp config
//
// DEFINE_CPLX_TYPE(HEXDUMP, WPP_LOGHEXDUMP, ByteArray, ItemHEXDump,"s", _HEX_, 0,2);
// WPP_FLAGS(-DLOG_HEXDUMP(len,str)=log_lenstr(len,str));
//
// DEFINE_CPLX_TYPE(ANSISTRING, WPP_LOGANSISTRING, ByteArray, ItemPString,"s", _SSID_, 0,2);
// WPP_FLAGS(-DLOG_ANSISTRING(len,str)=log_lenstr(len,str));
//
// DEFINE_CPLX_TYPE(DOT11SSID, WPP_LOGDOT11SSID, DOT11_SSID*, ItemPString,"s", _SSID_, 0,2);
//
// DEFINE_CPLX_TYPE(MACADDR, WPP_LOGMACADDR, DOT11_MAC_ADDRESS, ItemMACAddr,"s", _MAC_, 0);
//
// CUSTOM_TYPE(TLV_ID, ItemEnum(WDI_TLV::ENUMS::TLV_ID));
//
// CUSTOM_TYPE(MESSAGE_ID, ItemEnum(WDI_TLV::ENUMS::MESSAGE_ID));
//
// CUSTOM_TYPE(FIELD_NAME, ItemEnum(WDI_TLV::ENUMS::FIELD_NAME));
//
// CUSTOM_TYPE(ENCODED_TYPE_NAME, ItemEnum(WDI_TLV::ENUMS::ENCODED_TYPE_NAME));
//
// end_wpp
//


#endif

