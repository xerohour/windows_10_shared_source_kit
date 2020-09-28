/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    debug.c

Abstract:

    Contains debug and tracing routines.

Author:

    Erik Schmidt (eriksch) - October 2013

Environment:



Revision History:

--*/

#include "storufs.h"

#if USE_DBGLOG
VOID
UfsInitializeDbgLog(
    _Inout_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*+++

Routine Description:

    Free all the resources associated with the AdapterExtension.

Arguments:

    AdapterExtension - StorUfs specific extension structure

Return Value:

    N/A

--*/
{
    ULONG logSize = UFS_DBGLOG_SIZE;

    AdapterExtension->DbgLog.Index = 0;
    AdapterExtension->DbgLog.IndexMask = logSize / sizeof(UFS_DBGLOG_ENTRY) - 1;

    AdapterExtension->DbgLog.LogStart = (PUFS_DBGLOG_ENTRY) &AdapterExtension->DbgLogEntry[0];
    AdapterExtension->DbgLog.LogCurrent = NULL;
    AdapterExtension->DbgLog.LogEnd = AdapterExtension->DbgLog.LogStart +
                                      (logSize / sizeof(UFS_DBGLOG_ENTRY)) - 1;
}

VOID
UfsAddDbgLog(
    _Inout_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_  ULONG Sig,
    _In_  ULONG_PTR Data1,
    _In_  ULONG_PTR Data2,
    _In_  ULONG_PTR Data3
    )
/*+++

Routine Description:

    Free all the resources associated with the AdapterExtension.

Arguments:

    AdapterExtension - StorUfs specific extension structure

    Sig -

    Data1 -

    Data2 -

    Data3 -

Return Value:

    N/A

--*/
{
    PUFS_DBGLOG dbglog;
    PUFS_DBGLOG_ENTRY entry;
    ULONG index;

    NT_ASSERT(AdapterExtension && AdapterExtension->DbgLog.LogStart);

    dbglog = &AdapterExtension->DbgLog;
    index = InterlockedDecrement((volatile LONG *) &(dbglog->Index));
    index &= dbglog->IndexMask;

    entry = dbglog->LogStart + index;
    dbglog->LogCurrent = entry;

    entry->Sig = ((Sig & 0xFF) << 24) |
                 ((Sig & 0xFF00) << 8) |
                 ((Sig & 0xFF0000) >> 8) |
                 ((Sig & 0xFF000000) >> 24);
    entry->Data1 = Data1;
    entry->Data2 = Data2;
    entry->Data3 = Data3;
}
#endif

#if USE_DUMP_DEBUG
VOID
UfsDumpDebugBreakpoint(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*+++

Routine Description:

    If enabled, signal a dump mode breakpoint

Arguments:

    AdapterExtension - UFS adapter extension

Return Value:

    N/A

--*/
{
    if (UfsQueryDumpMode(AdapterExtension)) {
        DbgBreakPoint();
    }
}
#endif