/*****************************************************************************
* tracelogging.h - WDM Streaming port class driver definitions for port drivers
*****************************************************************************
* Copyright (c) Microsoft.  All rights reserved.
*/

#include <TraceLoggingProvider.h>
TRACELOGGING_DECLARE_PROVIDER(g_PortclsProvider);

void TraceFunctionCall(UCHAR tracelevel, LPCSTR pcszCaller, LPCSTR pcszCallee);
void TraceFunctionReturn(UCHAR tracelevel, LPCSTR pcszCaller, LPCSTR pcszCallee);

void TraceRefcountCall(UCHAR tracelevel, LPCSTR pcszCaller, LPCSTR pcszCallee);
void TraceRefcountReturn(UCHAR tracelevel, LPCSTR pcszCaller, LPCSTR pcszCallee, ULONG refCount);


#ifndef TRACELOGGING_FUNC
    #define TRACELOGGING_FUNC(tracelevel, expr) do {                    \
                TraceFunctionCall(tracelevel, __FUNCTION__, #expr);      \
                (expr);                                                 \
                TraceFunctionReturn(tracelevel, __FUNCTION__, #expr);    \
            } while(false);
#endif  // TRACELOGGING_FUNC

#ifndef TRACELOGGING_REFCOUNT
    #define TRACELOGGING_REFCOUNT(tracelevel, expr) do {                        \
                TraceRefcountCall(tracelevel, __FUNCTION__, #expr);             \
                TraceRefcountReturn(tracelevel, __FUNCTION__, #expr, (expr));   \
            } while(false);
#endif  // TRACELOGGING_REFCOUNT

