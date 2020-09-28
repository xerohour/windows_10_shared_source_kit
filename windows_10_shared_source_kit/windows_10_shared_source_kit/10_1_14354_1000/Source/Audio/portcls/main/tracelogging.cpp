/*****************************************************************************
* tracelogging.cpp - WDM Streaming port class driver
*****************************************************************************
* Copyright (c) Microsoft.  All rights reserved.
*/

#include "private.h"

// The provider name and GUID are tightly linked
// If you change the name, regenerate the GUID
// using the handy-dandy tool:
// <build>\diagnosibility\etw\tlgguid.exe Microsoft.Windows.Audio.PortClass
// intentionally not adding the telemetry attribute, since we don't (yet) have any telemetry events
TRACELOGGING_DEFINE_PROVIDER(
    g_PortclsProvider,
    "Microsoft.Windows.Audio.PortClass",
    // {bebd6159-6f76-5a22-366a-7753c2a63c3c}
    (0xbebd6159, 0x6f76, 0x5a22, 0x36, 0x6a, 0x77, 0x53, 0xc2, 0xa6, 0x3c, 0x3c));

#pragma code_seg()  // non-paged pool

void TraceFunctionCall(UCHAR tracelevel, LPCSTR pcszCaller, LPCSTR pcszCallee)
{
    switch(tracelevel)
    {
        case TRACE_LEVEL_FATAL:
            TraceLoggingWrite(g_PortclsProvider,
                                "[Function:calling]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingLevel(TRACE_LEVEL_FATAL));
            break;

        case TRACE_LEVEL_ERROR:
            TraceLoggingWrite(g_PortclsProvider,
                                "[Function:calling]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingLevel(TRACE_LEVEL_ERROR));
            break;

        case TRACE_LEVEL_WARNING:
            TraceLoggingWrite(g_PortclsProvider,
                                "[Function:calling]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingLevel(TRACE_LEVEL_WARNING));
            break;

        case TRACE_LEVEL_INFORMATION:
            TraceLoggingWrite(g_PortclsProvider,
                                "[Function:calling]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingLevel(TRACE_LEVEL_INFORMATION));
            break;

        case TRACE_LEVEL_VERBOSE:
            TraceLoggingWrite(g_PortclsProvider,
                                "[Function:calling]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingLevel(TRACE_LEVEL_VERBOSE));
            break;

        default:
            break;
    }
}

void TraceFunctionReturn(UCHAR tracelevel, LPCSTR pcszCaller, LPCSTR pcszCallee)
{
    switch(tracelevel)
    {
        case TRACE_LEVEL_FATAL:
            TraceLoggingWrite(g_PortclsProvider,
                                "[Function:returned]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingLevel(TRACE_LEVEL_FATAL));
            break;

        case TRACE_LEVEL_ERROR:
            TraceLoggingWrite(g_PortclsProvider,
                                "[Function:returned]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingLevel(TRACE_LEVEL_ERROR));
            break;

        case TRACE_LEVEL_WARNING:
            TraceLoggingWrite(g_PortclsProvider,
                                "[Function:returned]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingLevel(TRACE_LEVEL_WARNING));
            break;

        case TRACE_LEVEL_INFORMATION:
            TraceLoggingWrite(g_PortclsProvider,
                                "[Function:returned]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingLevel(TRACE_LEVEL_INFORMATION));
            break;

        case TRACE_LEVEL_VERBOSE:
            TraceLoggingWrite(g_PortclsProvider,
                                "[Function:returned]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingLevel(TRACE_LEVEL_VERBOSE));
            break;

        default:
            break;
    }
}

void TraceRefcountCall(UCHAR tracelevel, LPCSTR pcszCaller, LPCSTR pcszCallee)
{
    switch(tracelevel)
    {
        case TRACE_LEVEL_FATAL:
            TraceLoggingWrite(g_PortclsProvider,
                                "[RefCount:calling]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingLevel(TRACE_LEVEL_FATAL));
            break;

        case TRACE_LEVEL_ERROR:
            TraceLoggingWrite(g_PortclsProvider,
                                "[RefCount:calling]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingLevel(TRACE_LEVEL_ERROR));
            break;

        case TRACE_LEVEL_WARNING:
            TraceLoggingWrite(g_PortclsProvider,
                                "[RefCount:calling]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingLevel(TRACE_LEVEL_WARNING));
            break;

        case TRACE_LEVEL_INFORMATION:
            TraceLoggingWrite(g_PortclsProvider,
                                "[RefCount:calling]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingLevel(TRACE_LEVEL_INFORMATION));
            break;

        case TRACE_LEVEL_VERBOSE:
            TraceLoggingWrite(g_PortclsProvider,
                                "[RefCount:calling]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingLevel(TRACE_LEVEL_VERBOSE));
            break;

        default:
            break;
    }
}

void TraceRefcountReturn(UCHAR tracelevel, LPCSTR pcszCaller, LPCSTR pcszCallee, ULONG refCount)
{
    switch(tracelevel)
    {
        case TRACE_LEVEL_FATAL:
            TraceLoggingWrite(g_PortclsProvider,
                                "[RefCount:returned]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingValue(refCount, "RefCount"),
                                TraceLoggingLevel(TRACE_LEVEL_FATAL));
            break;

        case TRACE_LEVEL_ERROR:
            TraceLoggingWrite(g_PortclsProvider,
                                "[RefCount:returned]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingValue(refCount, "RefCount"),
                                TraceLoggingLevel(TRACE_LEVEL_ERROR));
            break;

        case TRACE_LEVEL_WARNING:
            TraceLoggingWrite(g_PortclsProvider,
                                "[RefCount:returned]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingValue(refCount, "RefCount"),
                                TraceLoggingLevel(TRACE_LEVEL_WARNING));
            break;

        case TRACE_LEVEL_INFORMATION:
            TraceLoggingWrite(g_PortclsProvider,
                                "[RefCount:returned]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingValue(refCount, "RefCount"),
                                TraceLoggingLevel(TRACE_LEVEL_INFORMATION));
            break;

        case TRACE_LEVEL_VERBOSE:
            TraceLoggingWrite(g_PortclsProvider,
                                "[RefCount:returned]",
                                TraceLoggingValue(pcszCaller, "Caller"),
                                TraceLoggingValue(pcszCallee, "Callee"),
                                TraceLoggingValue(refCount, "RefCount"),
                                TraceLoggingLevel(TRACE_LEVEL_VERBOSE));
            break;

        default:
            break;
    }
}

