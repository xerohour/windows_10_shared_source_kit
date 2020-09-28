/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    log.c

     3-10-2002

Abstract:

   Provides bus trace functionality and logging.


Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/

#include "hubdef.h"
#include "pch.h"

// global sequence number, we use this to interleve multiple hub logs
// if we need to.
ULONG UsbhLogMask = (
 LOG_MISC     |       
 LOG_PNP      |       
 LOG_BUS      |       
 LOG_HUB      |       
 LOG_POWER    |       
 LOG_IOCTL    |       
 LOG_WMI      |       
 LOG_EX       |       
 LOG_PDO      |       
 LOG_PCQ      |       
 LOG_BUSF     |       
 LOG_BUSM     |       
 LOG_IDSTR    |       
 LOG_OVERC    |       
 LOG_PIND     |       
 LOG_SSH      |       
 LOG_TMR      |
 LOG_REG);     


VOID
UsbhLogAlloc(
    PDEVICE_OBJECT HubFdo,
    PDEBUG_LOG Log,
    ULONG Pages
    )
/*++

Routine Description:

    Init the debug log -
    remember interesting information in a circular buffer

Arguments:

Return Value:

    None.

--*/
{
    ULONG logSize = 4096*Pages;
    PDEVICE_EXTENSION_HUB hubExt;
    
    hubExt = FdoExt(HubFdo);

    RtlZeroMemory(&hubExt->DebugLogEntries[0],
                  logSize);

    Log->LogStart = &hubExt->DebugLogEntries[0];
    Log->LogIdx = 0;
    Log->LogSizeMask = (logSize / sizeof(LOG_ENTRY)) - 1;
    // Point the end (and first entry) 1 entry from the end
    // of the segment
    Log->LogEnd = Log->LogStart +
                  (logSize / sizeof(struct LOG_ENTRY)) - 1;
}


VOID
UsbhLogFree(
    PDEVICE_OBJECT HubFdo,
    PDEBUG_LOG Log
    )
/*++

Routine Description:

Arguments:

Return Value:

    None.

--*/
{
}
