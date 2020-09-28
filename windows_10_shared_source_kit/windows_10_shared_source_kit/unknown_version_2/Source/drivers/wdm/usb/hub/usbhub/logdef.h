/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    logdef.h

Abstract:

    Log structures.

Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/

#ifndef   __LOG_H__
#define   __LOG_H__

#define LOG_MISC            0x00000001
#define LOG_PNP             0x00000002
#define LOG_BUS             0x00000004
#define LOG_HUB             0x00000008
#define LOG_POWER           0x00000010
#define LOG_IOCTL           0x00000020
#define LOG_WMI             0x00000040
#define LOG_EX              0x00000080
#define LOG_PDO             0x00000100
#define LOG_PCQ             0x00000200
#define LOG_BUSF            0x00000400
#define LOG_BUSM            0x00000800
#define LOG_IDSTR           0x00001000
#define LOG_TMR             0x00002000
#define LOG_OVERC           0x00004000
#define LOG_PIND            0x00008000
#define LOG_SSH             0x00010000
#define LOG_REG             0x00020000



/*
    ** DEBUG LOG **

    since log entries may hold pointers the size of a log struct
    varies with the platform
*/

#ifdef _WIN64
#define LOG_ENTRY LOG_ENTRY64
#define PLOG_ENTRY PLOG_ENTRY64
#else
#define LOG_ENTRY LOG_ENTRY32
#define PLOG_ENTRY PLOG_ENTRY32
#endif

typedef struct LOG_ENTRY64 {
    ULONG        le_sig;          // Identifying string
    ULONG        pad;
    ULONG64      le_info1;        // entry specific info
    ULONG64      le_info2;        // entry specific info
    ULONG64      le_info3;        // entry specific info
} LOG_ENTRY64, *PLOG_ENTRY64; /* LOG_ENTRY */

typedef struct LOG_ENTRY32 {
    ULONG        le_sig;          // Identifying string
    ULONG        le_info1;        // entry specific info
    ULONG        le_info2;        // entry specific info
    ULONG        le_info3;        // entry specific info
} LOG_ENTRY32, *PLOG_ENTRY32; /* LOG_ENTRY */


typedef struct _DEBUG_LOG {
    ULONG LogIdx;
    ULONG LogSizeMask;
    PLOG_ENTRY LogStart;
    PLOG_ENTRY LogEnd;
} DEBUG_LOG, *PDEBUG_LOG;


VOID
UsbhLogAlloc(
    PDEVICE_OBJECT HubFdo,
    PDEBUG_LOG Log,
    ULONG Pages
    );

VOID
UsbhLogFree(
    PDEVICE_OBJECT HubFdo,
    PDEBUG_LOG Log
    );

VOID
UsbhAddLogEntry(
    PDEBUG_LOG Log,
    ULONG Mask,
    ULONG Sig,
    ULONG_PTR Info1,
    ULONG_PTR Info2,
    ULONG_PTR Info3
    );

typedef union _LOGSIG {
    struct {
        UCHAR Byte0;
        UCHAR Byte1;
        UCHAR Byte2;
        UCHAR Byte3;
    } b;
    ULONG l;
} LOGSIG, *PLOGSIG;

#define LOG(fdo, lmask, lsig, linfo1, linfo2)  \
    Log((fdo), (lmask), (lsig), (ULONG_PTR) (linfo1), (ULONG_PTR) (linfo2))

//#define LOG(fdo, lmask, lsig, linfo1, linfo2)  \
//    {\
//    PDEVICE_EXTENSION_HUB delog;\
//    PDEBUG_LOG llog;\
//    ULONG s;\
//    extern ULONG UsbhDebugLogEnable;\
//    extern ULONG UsbhLogMask;\
//    extern ULONG UsbhLogSequence;\
//    delog = FdoExt(fdo);\
//    if (UsbhDebugLogEnable && \
//        delog->Log.LogStart != NULL && \
//        ((lmask) & UsbhLogMask)) {\
//        s = InterlockedIncrement(&UsbhLogSequence);\
//        llog = &delog->Log;\
//        UsbhAddLogEntry(llog, (lmask), (lsig), s, (linfo1), (linfo2));\
//    }\
//    }

#define UsbhAddLogEntry(log, mask, insig, i1, i2, i3) \
    {\
    PLOG_ENTRY lelog;\
    ULONG ilog;\
    LOGSIG siglog, rsiglog;\
    siglog.l = (insig);\
    rsiglog.b.Byte0 = siglog.b.Byte3;\
    rsiglog.b.Byte1 = siglog.b.Byte2;\
    rsiglog.b.Byte2 = siglog.b.Byte1;\
    rsiglog.b.Byte3 = siglog.b.Byte0;\
    UsbhAssert(NULL, (insig) != 0);\
    ilog = InterlockedDecrementNoFence((PLONG)&(log)->LogIdx);\
    ilog &= (log)->LogSizeMask;\
    lelog = (log)->LogStart+ilog;\
    UsbhAssert(NULL, lelog <= (log)->LogEnd);\
    lelog->le_sig = rsiglog.l;\
    lelog->le_info1 = (ULONG_PTR) (i1);\
    lelog->le_info2 = (ULONG_PTR) (i2);\
    lelog->le_info3 = (ULONG_PTR) (i3);\
    };


#endif /* __LOG_H__ */

