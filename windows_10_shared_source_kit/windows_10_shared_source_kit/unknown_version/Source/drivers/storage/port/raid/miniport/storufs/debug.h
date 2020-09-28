/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    debug.h

Abstract:

    Contains structure definitions for debug and tracing operations.

Author:

    Erik Schmidt (eriksch) - October 2013

Environment:



Revision History:

--*/

#pragma once

#define UFS_DBGLOG_SIZE                       PAGE_SIZE
#define USE_DBGLOG                            DBG
#define USE_DUMP_DEBUG                        0

typedef struct _UFS_DBGLOG_ENTRY {
    ULONG_PTR Sig;
    ULONG_PTR Data1;
    ULONG_PTR Data2;
    ULONG_PTR Data3;
} UFS_DBGLOG_ENTRY, *PUFS_DBGLOG_ENTRY;


typedef struct _UFS_DBGLOG {
    ULONG Index;
    ULONG IndexMask;

    PUFS_DBGLOG_ENTRY LogStart;
    PUFS_DBGLOG_ENTRY LogCurrent;
    PUFS_DBGLOG_ENTRY LogEnd;
} UFS_DBGLOG, *PUFS_DBGLOG;

//
// Function Prototypes
//

#if USE_DBGLOG

VOID
UfsInitializeDbgLog(
    _Inout_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );

VOID
UfsAddDbgLog(
    _Inout_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_  ULONG Sig,
    _In_  ULONG_PTR Data1,
    _In_  ULONG_PTR Data2,
    _In_  ULONG_PTR Data3
    );

#else

#define UfsInitializeDbgLog
#define UfsAddDbgLog

#endif

#if USE_DUMP_DEBUG

VOID
UfsDumpDebugBreakpoint(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );

#else

#define UfsDumpDebugBreakpoint

#endif