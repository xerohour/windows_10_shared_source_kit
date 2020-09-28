/*++

Copyright (c) 2004  Microsoft Corporation

Module Name:

    dbg.h

Abstract:

    debug macros

Environment:

    Kernel & user mode

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs

--*/

#ifndef   __DBG_H__
#define   __DBG_H__

#define USBPORT_TAG          'pbsu'        //"USBP"

extern ULONG USBPORT_Debug_Trace_Level;


#if DBG
/**********
DUBUG
***********/

#define UNSIG(x) (x)->Sig = SIG_FREE

// this is intentionally not defined in retail in order to break the build.  These should not be left in. 
#define TEST_TRAP_TEST()         NT_ASSERTMSG("Test Trap",FALSE)
                            

//
// This Breakpoint means we either need to test the code path somehow or the code is not implemented.  Either case we
// should not have any of these when the driver is fully tested.  These only go off if the USBPORT_Debug_Trace_Level is 
// set to a nonzero value so we won't see them in a default debug build.
//

#define TEST_TRAP()         if (USBPORT_Debug_Trace_Level) {\
                                DbgPrint("<TEST_TRAP> %s, line %d\n", __FILE__, __LINE__);\
                                NT_ASSERTMSG("Test Trap",FALSE);\
                            }

//
// This trap means something very bad has happened, the system will crash
//

#define BUGCHECK(bc, p2, p3, p4) \
{ \
    KeBugCheckEx(BUGCODE_USB_DRIVER, (bc), (p2), (p3), (p4)); \
}

#define BUGCHECK_FDO(fdo, bc, p2, p3, p4, DataLength, DataBuffer) \
{ \
    USBPORT_SetMiniDumpData((fdo), (DataLength), (DataBuffer)); \
    KeBugCheckEx(BUGCODE_USB_DRIVER, (bc), (p2), (p3), (p4)); \
}


#define CATC_TRAP(d) USBPORT_CatcTrap((d))

#define CATC_TRAP_ERROR(d, e) \
    do {\
    extern ULONG USBPORT_CatcTrapEnable;\
    if (!NT_SUCCESS((e) && USBPORT_CatcTrapEnable)) { \
        USBPORT_CatcTrap((d));\
    }\
    }\
    WHILE (0)

ULONG
_cdecl
USBPORT_KdPrintX(
    ULONG l,
    __in LPSTR Format,
    ...
    );

#define   USBPORT_KdPrint(_x_) USBPORT_KdPrintX _x_

#if defined(XP_COMPAT)
#define USBPORT_ASSERT  ASSERT
#define USBPORT_ASSERTMSG ASSERTMSG
#else
#define USBPORT_ASSERT NT_ASSERT
#define USBPORT_ASSERTMSG NT_ASSERTMSG
#endif

#define ASSERT_TRANSFER_URB(u) USBPORT_AssertTransferUrb((u))



NTSTATUS
USBPORT_GetGlobalDebugRegistryParameters(
    VOID
    );

#define GET_GLOBAL_DEBUG_PARAMETERS() \
    USBPORT_GetGlobalDebugRegistryParameters();

#define ASSERT_PASSIVE() \
    do {\
        USBPORT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL); \
    } WHILE (0)

// test failure paths
#define FAILED_GETRESOURCES              1
#define FAILED_LOWER_START               2
#define FAILED_REGISTERUSBPORT           3
#define FAILED_USBPORT_START             4
#define FAILED_NEED_RESOURCE             5

//#define TEST_PATH(status, pathname) 



#else
/**********
RETAIL
***********/

// debug macros for retail build

#define TEST_TRAP()
#define TRAP()
#define BUG_TRAP()

#define GET_GLOBAL_DEBUG_PARAMETERS()

#define ASSERT_PASSIVE()

#define TEST_TRAP()

#define BUGCHECK(bc, p2, p3, p4) \
{ \
    KeBugCheckEx(BUGCODE_USB_DRIVER, (bc), (p2), (p3), (p4)); \
}

#define BUGCHECK_FDO(fdo, bc, p2, p3, p4, DataLength, DataBuffer) \
{ \
    USBPORT_SetMiniDumpData((fdo), (DataLength), (DataBuffer)); \
    KeBugCheckEx(BUGCODE_USB_DRIVER, (bc), (p2), (p3), (p4)); \
}

#define CATC_TRAP(d)

#define CATC_TRAP_ERROR(d, e)

#define   USBPORT_KdPrint(_x_)

#if defined(XP_COMPAT)
#define USBPORT_ASSERT 
#define USBPORT_ASSERTMSG 
#else
#define USBPORT_ASSERT NT_ASSERT
#define USBPORT_ASSERTMSG NT_ASSERTMSG
#endif

#define ASSERT_TRANSFER_URB(u) 

//#define TEST_PATH(status, path)

#define UNSIG(x)

#endif /* DBG */

/*************
RETAIL & DEBUG
**************/

VOID 
USBPORT_AssertSig(PVOID Object,
                  ULONG Sig,
                  ULONG ExpectedSig
                  );

#define ASSERT_PDOEXT(x)  USBPORT_AssertSig((x), (x)->Sig, ROOTHUB_DEVICE_EXT_SIG)

#define ASSERT_FDOEXT(x)  USBPORT_AssertSig((x), (x)->Sig, USBPORT_DEVICE_EXT_SIG)

#define ASSERT_DEVICE_HANDLE(x) USBPORT_AssertSig((x), (x)->Sig, SIG_DEVICE_HANDLE)

#define ASSERT_CONFIG_HANDLE(x) USBPORT_AssertSig((x), (x)->Sig, SIG_CONFIG_HANDLE)

#define ASSERT_PIPE_HANDLE(x) USBPORT_AssertSig((x), (x)->Sig, SIG_PIPE_HANDLE)

#define ASSERT_INTERFACE_HANDLE(x) USBPORT_AssertSig((x), (x)->Sig, SIG_INTERFACE_HANDLE)

#define ASSERT_ENDPOINT(x) USBPORT_AssertSig((x), (x)->Sig, SIG_ENDPOINT) 

#define ASSERT_EPHANDLE(x) USBPORT_AssertSig((x), (x)->Sig, SIG_EPREF)

#define ASSERT_TRANSFER(x) USBPORT_AssertSig((x), (x)->Sig, SIG_TRANSFER)

#define ASSERT_COMMON_BUFFER(x) USBPORT_AssertSig((x), (x)->Sig, SIG_CMNBUF)

#define ASSERT_INTERFACE(x) USBPORT_AssertSig((x), (x)->Sig, SIG_INTERFACE_HANDLE)

#define ASSERT_TT(x) USBPORT_AssertSig((x), (x)->Sig, SIG_TT)

#define ASSERT_DB_HANDLE(x) USBPORT_AssertSig((x), (x)->Sig, SIG_DB)

#define ASSERT_IRP_CONTEXT(x) USBPORT_AssertSig((x), (x)->Sig, SIG_IRPC)

#define ASSERT_REG_CACHE(x) USBPORT_AssertSig((x), (x)->Sig, SIG_REG_CACHE)

#define ASSERT_XDPC(x) USBPORT_AssertSig((x), (x)->Sig, SIG_XDPC)

#define ASSERT_USB_IOREQUEST(x) USBPORT_ASSERT((x)->Sig == SIG_USB_IOREQUEST)

#define ASSERT_ABORT_CONTEXT(x) USBPORT_AssertSig((x), (x)->Sig, SIG_ABORT_PIPE_CONTEXT)

#define ASSERT_USB_TIMEOUT(x) USBPORT_AssertSig((x), (x)->Sig, SIG_USB_TIMEOUT)

#define ASSERT_MPASYNC_TIMER(x) USBPORT_AssertSig((x), (x)->Sig, SIG_MP_TIMR)

#define ASSERT_HC_WAKE_CONTEXT(x) USBPORT_AssertSig((x), (x)->Sig,  SIG_HC_WAKE_CONTEXT)

#define ASSERT_BUS_CONTEXT(bc)  USBPORT_AssertSig((bc), ((PBUS_CONTEXT)(bc))->BusCxtSig, SIG_USBPORT_BUS_CONTEXT)

VOID
USBPORTSVC_DbgPrint(
    __in PVOID DeviceData,
    ULONG Level,
    __in LPSTR Format,
    int Arg0,
    int Arg1,
    int Arg2,
    int Arg3,
    int Arg4,
    int Arg5
    );

VOID
USBPORTSVC_TestDebugBreak(
    __in PVOID DeviceData
    );

VOID
USBPORTSVC_AssertFailure(
    __in PVOID DeviceData,
    __in PVOID FailedAssertion,
    __in PVOID FileName,
    ULONG LineNumber,
    __in PCCHAR Message
    );

/*
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
    ULONG LogSize;
    ULONG LogSizeMask;
    PLOG_ENTRY LogStart;
    PLOG_ENTRY LogEnd;
} DEBUG_LOG, *PDEBUG_LOG;


// log noisy is for entries that tend
// to fill up the log and we genrally
// don't use
#define LOG_NOISY       0x00000001
#define LOG_MINIPORT    0x00000002
#define LOG_TRANSFER    0x00000004
#define LOG_PNP         0x00000008
#define LOG_MEM         0x00000010
#define LOG_SPIN        0x00000020
#define LOG_POWER       0x00000040
#define LOG_RH          0x00000080
#define LOG_MISC        0x00000200
#define LOG_ISO         0x00000400



VOID
USBPORT_LogAlloc(
    __inout PDEBUG_LOG Log,
    ULONG Pages
    );

VOID
USBPORT_LogFree(
   __inout PDEVICE_OBJECT FdoDeviceObject,
   __inout PDEBUG_LOG Log
    );

#define SWIZZLE(sig) \
    ((((sig) >> 24) & 0x000000FF) | \
     (((sig) >>  8) & 0x0000FF00) | \
     (((sig) <<  8) & 0x00FF0000) | \
     (((sig) << 24) & 0xFF000000))

#define LOGENTRY(fdo, lmask, lsig, linfo1, linfo2, linfo3)  \
    do {\
    PDEVICE_EXTENSION delog;\
    PDEBUG_LOG llog;\
    GET_DEVICE_EXT(delog, (fdo));\
    if (delog->Log.LogStart != NULL) {\
        llog = &delog->Log;\
        USBPORT_AddLogEntry(llog, SWIZZLE(lsig), (linfo1), (linfo2), (linfo3));\
    }\
    } WHILE (0);

#define USBPORT_AddLogEntry(log, insig, i1, i2, i3) \
    {\
    PLOG_ENTRY lelog;\
    ULONG ilog;\
    ilog = InterlockedDecrementNoFence(&(log)->LogIdx);\
    ilog &= (log)->LogSizeMask;\
    lelog = (log)->LogStart+ilog;\
    lelog->le_sig = insig;\
    lelog->le_info1 = (ULONG_PTR) (i1);\
    lelog->le_info2 = (ULONG_PTR) (i2);\
    lelog->le_info3 = (ULONG_PTR) (i3);\
    };

#endif /* __DBG_H__ */
