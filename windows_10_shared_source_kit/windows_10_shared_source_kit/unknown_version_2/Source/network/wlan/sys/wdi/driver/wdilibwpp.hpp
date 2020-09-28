//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once
#ifndef WDILIB_WPP_H
#define WDILIB_WPP_H

extern "C"
{
#include <WppRecorder.h>
}
#define WPP_USE_TRACE_LEVELS

// 21ba7b61-05f8-41f1-9048-c09493dcfe38
#define WPP_CONTROL_GUIDS                                               \
    WPP_DEFINE_CONTROL_GUID(                                            \
        WdiLibraryCtlGuid,(21ba7b61,05f8,41f1,9048,c09493dcfe38),        \
        WPP_DEFINE_BIT(DUMMY)                                            \
    )

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

//
// C++ with Activity ID function entry/exit commands
//
// begin_wpp config
//
// USEPREFIX (TraceActEntry, "%!STDPREFIX! [O:%p][A:%x]", this, this->get_ActivityId());
// FUNC TraceActEntry{LEVEL=TRACE_LEVEL_VERBOSE}(...);
// USESUFFIX (TraceActEntry, "--> %!FUNC!");
//
// USEPREFIX (TraceActExit, "%!STDPREFIX! [O:%p][A:%x]", this, this->get_ActivityId());
// FUNC TraceActExit{LEVEL=TRACE_LEVEL_VERBOSE}(EXP);
// USESUFFIX (TraceActExit, "<-- %!FUNC!: 0x%x", EXP);
//
// end_wpp
//


//
// C++ with Activity ID trace commands
//
// begin_wpp config
//
// USEPREFIX (WFCActError, "%!STDPREFIX! [O:%p][A:%x][ERROR]", this, this->get_ActivityId());
// FUNC WFCActError{LEVEL=TRACE_LEVEL_ERROR}(MSG, ...);
//
// USEPREFIX (WFCActErrorStatus, "%!STDPREFIX! [O:%p][A:%x][ERROR=%x]", this, this->get_ActivityId(), status);
// FUNC WFCActErrorStatus{LEVEL=TRACE_LEVEL_ERROR}(MSG, ...);
//
// USEPREFIX (WFCActInfo, "%!STDPREFIX! [O:%p][A:%x][INFO]", this, this->get_ActivityId());
// FUNC WFCActInfo{LEVEL=TRACE_LEVEL_INFORMATION}(MSG, ...);
//
// USEPREFIX (WFCActTrace, "%!STDPREFIX! [O:%p][A:%x][TRACE]", this, this->get_ActivityId());
// FUNC WFCActTrace{LEVEL=TRACE_LEVEL_VERBOSE}(MSG, ...);
//
// end_wpp
//

//
// C++ with Port ID trace commands
//
// begin_wpp config
//
// USEPREFIX (WFCPortError, "%!STDPREFIX! [O:%p][P:%x][ERROR]", this, this->GetPortId());
// FUNC WFCPortError{LEVEL=TRACE_LEVEL_ERROR}(MSG, ...);
//
// USEPREFIX (WFCPortErrorStatus, "%!STDPREFIX! [O:%p][P:%x][ERROR=%x]", this, this->GetPortId(), status);
// FUNC WFCPortErrorStatus{LEVEL=TRACE_LEVEL_ERROR}(MSG, ...);
//
// USEPREFIX (WFCPortInfo, "%!STDPREFIX! [O:%p][P:%x][INFO]", this, this->GetPortId());
// FUNC WFCPortInfo{LEVEL=TRACE_LEVEL_INFORMATION}(MSG, ...);
//
// USEPREFIX (WFCPortTrace, "%!STDPREFIX! [O:%p][P:%x][TRACE]", this, this->GetPortId());
// FUNC WFCPortTrace{LEVEL=TRACE_LEVEL_VERBOSE}(MSG, ...);
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
// CUSTOM_TYPE(WFC_RX_PEER_STATE, ItemEnum(_WFC_RX_PEER_STATE));
//
// CUSTOM_TYPE(MESSAGE_ID, ItemEnum(WDI_TLV::ENUMS::MESSAGE_ID));
//
// end_wpp
//


#endif

