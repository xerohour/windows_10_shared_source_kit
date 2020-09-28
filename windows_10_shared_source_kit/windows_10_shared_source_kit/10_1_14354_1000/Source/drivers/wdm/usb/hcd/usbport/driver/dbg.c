/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    dbg.c

Abstract:

    Debug functions and services

Environment:

    kernel mode only

Notes:

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs
    04-14-05 : removed win98 era debug functions

--*/

#include "stdarg.h"
#include "stdio.h"

#include "common.h"

#ifdef USBPORT_WPP
#include "dbg.tmh"
#endif

// default to everything but noisy
ULONG USBPORT_LogMask = 0xFFFFFFFE;

ULONG USBPORT_DebugLogEnable = 1;
ULONG USBPORT_CatcTrapEnable = 0;

#if DBG
/******************************************************************************************************************
DEBUG ONLY
*******************************************************************************************************************/

#define  DEFAULT_DEBUG_LEVEL    0

#ifdef DEBUG1
#undef DEFAULT_DEBUG_LEVEL
#define  DEFAULT_DEBUG_LEVEL    1
#endif

#ifdef DEBUG2
#undef DEFAULT_DEBUG_LEVEL
#define  DEFAULT_DEBUG_LEVEL    2
#endif

ULONG USBPORT_Debug_Trace_Level = DEFAULT_DEBUG_LEVEL;
ULONG USBPORT_BreakOn = 0;




VOID
USBPORTSVC_DbgPrint(
    __in PDEVICE_DATA DeviceData,
    ULONG Level,
    __in  LPSTR Format,
    int Arg0,
    int Arg1,
    int Arg2,
    int Arg3,
    int Arg4,
    int Arg5
    )
{
    PDEVICE_EXTENSION devExt;

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);

    if (USBPORT_Debug_Trace_Level > Level) {
        DbgPrint("%s.SYS: ", devExt->Fdo.Name);

        DbgPrint(Format, Arg0, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}


VOID
USBPORTSVC_TestDebugBreak(
    __in PDEVICE_DATA DeviceData
    )
{
    TEST_TRAP();
}



NTSTATUS
USBPORT_GetGlobalDebugRegistryParameters(
    VOID
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
#define MAX_KEYS    5
    ULONG k = 0;
    PWCHAR usb = L"usb";
    NTSTATUS ntStatus = STATUS_BOGUS;
    RTL_QUERY_REGISTRY_TABLE QueryTable[MAX_KEYS];

    ASSERT_PASSIVE();

    // Set up QueryTable to do the following:

    // spew level - 0
    QueryTable[k].QueryRoutine = USBPORT_GetConfigValue;
    QueryTable[k].Flags = 0;
    QueryTable[k].Name = DEBUG_LEVEL_KEY;
    QueryTable[k].EntryContext = &USBPORT_Debug_Trace_Level;
    QueryTable[k].DefaultType = REG_DWORD;
    QueryTable[k].DefaultData = &USBPORT_Debug_Trace_Level;
    QueryTable[k].DefaultLength = sizeof(USBPORT_Debug_Trace_Level);

    k++;
    USBPORT_ASSERT(k < MAX_KEYS);

    // log mask - 1
    QueryTable[k].QueryRoutine = USBPORT_GetConfigValue;
    QueryTable[k].Flags = 0;
    QueryTable[k].Name = DEBUG_LOG_MASK;
    QueryTable[k].EntryContext = &USBPORT_LogMask;
    QueryTable[k].DefaultType = REG_DWORD;
    QueryTable[k].DefaultData = &USBPORT_LogMask;
    QueryTable[k].DefaultLength = sizeof(USBPORT_LogMask);

    k++;
    USBPORT_ASSERT(k < MAX_KEYS);

    // log enable - 2
    QueryTable[k].QueryRoutine = USBPORT_GetConfigValue;
    QueryTable[k].Flags = 0;
    QueryTable[k].Name = DEBUG_LOG_ENABLE;
    QueryTable[k].EntryContext = &USBPORT_DebugLogEnable;
    QueryTable[k].DefaultType = REG_DWORD;
    QueryTable[k].DefaultData = &USBPORT_DebugLogEnable;
    QueryTable[k].DefaultLength = sizeof(USBPORT_DebugLogEnable);

    k++;
    USBPORT_ASSERT(k < MAX_KEYS);

    // catc trap enable - 3
    QueryTable[k].QueryRoutine = USBPORT_GetConfigValue;
    QueryTable[k].Flags = 0;
    QueryTable[k].Name = DEBUG_CATC_ENABLE;
    QueryTable[k].EntryContext = &USBPORT_CatcTrapEnable;
    QueryTable[k].DefaultType = REG_DWORD;
    QueryTable[k].DefaultData = &USBPORT_CatcTrapEnable;
    QueryTable[k].DefaultLength = sizeof(USBPORT_CatcTrapEnable);

    k++;
    USBPORT_ASSERT(k < MAX_KEYS);

    // stop
    QueryTable[k].QueryRoutine = NULL;
    QueryTable[k].Flags = 0;
    QueryTable[k].Name = NULL;

    ntStatus = RtlQueryRegistryValues(
                    RTL_REGISTRY_SERVICES,
                    usb,
                    QueryTable,     // QueryTable
                    NULL,           // Context
                    NULL);          // Environment

    if (NT_SUCCESS(ntStatus)) {
        USBPORT_KdPrint((1, "Debug Trace Level Set: (%d)\n", USBPORT_Debug_Trace_Level));
        if (USBPORT_DebugLogEnable) {
            USBPORT_KdPrint((1, "DEBUG-LOG is ON\n"));
        } else {
            USBPORT_KdPrint((1, "DEBUG-LOG is OFF\n"));
        }

        USBPORT_KdPrint((1, "DEBUG Log Mask is 0x%08.8x\n", USBPORT_LogMask));

        if (USBPORT_CatcTrapEnable) {
            USBPORT_KdPrint((1, "DEBUG ANALYZER TRIGGER is ON\n"));
        }
    }

    if ( STATUS_OBJECT_NAME_NOT_FOUND == ntStatus ) {
        ntStatus = STATUS_SUCCESS;
    }

    return ntStatus;
}

VOID
USBPORT_AssertTransferUrb(
    __inout PTRANSFER_URB Urb
    )
{
    PDEVICE_OBJECT fdoDeviceObject;
    PHCD_TRANSFER_CONTEXT transfer;
    PHCD_ENDPOINT endpoint;

    transfer = Urb->pd.HcdTransferContext;
    ASSERT_TRANSFER(transfer);

    USBPORT_ASSERT(transfer->Urb == Urb);

    endpoint = USBPORT_EndpointFromHandle(transfer->EpRefHandle);
    ASSERT_ENDPOINT(endpoint);

    fdoDeviceObject = endpoint->FdoDeviceObject;
    LOGENTRY(fdoDeviceObject, LOG_TRANSFER, 'Aurb', Urb, transfer, 0);

    USBPORT_ASSERT(Urb->pd.UrbSig == URB_SIG);
}


ULONG
_cdecl
USBPORT_KdPrintX(
    ULONG level,
    __in LPSTR Format,
    ...
    )
/*++

Routine Description:

    Debug Print wrapper function.

    Prints based on the value of the global var USBPORT_Debug_Trace_Level.  The input level must be less than
    USBPORT_Debug_Trace_Level, currently levels 0 - 3 are used

Arguments:

Return Value:


--*/
{
    va_list list;
    int i;
    int arg[6];

    if (USBPORT_Debug_Trace_Level > level) {
        DbgPrint("USBPORT.SYS: ");
        va_start(list, Format);
        for (i=0; i<6; i++)
            arg[i] = va_arg(list, int);

        DbgPrint(Format, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
    }

    return 0;
}


#else

/******************************************************************************************************************
RETAIL ONLY
*******************************************************************************************************************/

VOID
USBPORTSVC_DbgPrint(
    __in PDEVICE_DATA DeviceData,
    ULONG Level,
    __in PCCHAR Format,
    int Arg0,
    int Arg1,
    int Arg2,
    int Arg3,
    int Arg4,
    int Arg5
    )
{
    // nop
}


VOID
USBPORTSVC_TestDebugBreak(
    __in PDEVICE_DATA DeviceData
    )
{
    // nop
}

#endif /* DBG */

/******************************************************************************************************************
RETAIL and DEBUG
*******************************************************************************************************************/

VOID
USBPORTSVC_AssertFailure(
    __in PDEVICE_DATA DeviceData,
    __in PVOID FailedAssertion,
    __in PVOID FileName,
    ULONG LineNumber,
    __in PCCHAR Message
    )
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT hcFdo;
    ULONG ilog;
    PLOG_ENTRY lelog;

    ULONG bc = 1;

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    //ASSERT_FDOEXT(devExt);
    hcFdo = devExt->HcFdoDeviceObject;

    ilog = devExt->Log.LogIdx;
    ilog &= devExt->Log.LogSizeMask;
    lelog = devExt->Log.LogStart+ilog;

    if (bc) {
        BUGCHECK_FDO(hcFdo,
                     USBBUGCODE_MINIPORT_ASSERT_FAILURE,    // BugCode
                     (ULONG_PTR)lelog,                      // BugCheckParameter2
                     (ULONG_PTR)Message,                    // BugCheckParameter3
                     (ULONG_PTR)FileName,                   // BugCheckParameter4
                     0,                                     // DataLength
                     NULL);                                 // DataBuffer
    }
}


VOID
USBPORTSVC_LogEntry(
    __inout PDEVICE_DATA DeviceData,
    ULONG Mask,
    ULONG Sig,
    ULONG_PTR Info1,
    ULONG_PTR Info2,
    ULONG_PTR Info3
    )
/*++

Routine Description:

    Service for miniport to add log entries.

Arguments:

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION devExt;
    PDEBUG_LOG l;

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);

    if (devExt->Log.LogStart != NULL) {
        l = &devExt->Log;
        USBPORT_AddLogEntry(l, Sig, Info1, Info2, Info3);
    }
}


VOID
USBPORT_LogAlloc(
    __inout PDEBUG_LOG Log,
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
    ULONG logSize = 0;
    NTSTATUS nts;

    Log->LogStart = NULL;

    // logSize = 4096 * Pages
    nts = RtlULongMult(4096, Pages, &logSize);

    if (NT_SUCCESS(nts) && USBPORT_DebugLogEnable) {

        // we won't track the mem we alloc for the log
        // we will let the verifier do that
        ALLOC_POOL_Z(Log->LogStart,
                     NonPagedPoolNx,
                     logSize);

        if (Log->LogStart) {
            Log->LogIdx = 0;
            Log->LogSize = logSize;
            Log->LogSizeMask = (logSize/sizeof(LOG_ENTRY));
            Log->LogSizeMask-=1;
            // Point the end (and first entry) 1 entry from the end
            // of the segment
            Log->LogEnd = Log->LogStart +
                (logSize / sizeof(struct LOG_ENTRY)) - 1;
        } else {
            TEST_TRAP();
        }
    }
}


VOID
USBPORT_LogFree(
    __inout PDEVICE_OBJECT FdoDeviceObject,
    __inout PDEBUG_LOG Log
    )
/*++

Routine Description:

Arguments:

Return Value:

    None.

--*/
{
    if (Log->LogStart != NULL) {
        // log the free of the log in order to debug
        // verifier bugs
        UsbFreePool(Log->LogStart);
        // this will indicate that we have freed the
        // log, other log pointers will remain intact
        Log->LogStart = NULL;
        Log->LogSize = 0;
    }

    return;
}

/*
     Transmit the analyzer trigger packet
*/

VOID
USBPORT_BeginTransmitTriggerPacket(
    __inout PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION devExt;
    USB_MINIPORT_STATUS mpStatus;
    HW_32BIT_PHYSICAL_ADDRESS phys;
    PUCHAR va, mpData;
    ULONG length, mpDataLength;
    MP_PACKET_PARAMETERS mpPacket;
    USBD_STATUS usbdStatus;
    UCHAR data[4];

    DbgTrace((UsbpDebugTrace, "'USBPORT_TransmitTriggerPacket\n"));

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'TRIG', &mpPacket, 0,
       0);

    // build up request for miniport

    length = devExt->Fdo.ScratchCommonBuffer->TotalLength;
    va = devExt->Fdo.ScratchCommonBuffer->MiniportVa;
    phys = devExt->Fdo.ScratchCommonBuffer->MiniportPhys;

    mpPacket.DeviceAddress = 127;
    mpPacket.EndpointAddress = 8;
    mpPacket.MaximumPacketSize = 64;
    mpPacket.Type = ss_Out;
    mpPacket.Speed = ss_Full;
    mpPacket.Toggle = ss_Toggle0;

    data[0] = 'G';
    data[1] = 'O';
    data[2] = 'A';
    data[3] = 'T';

    mpData = &data[0];
    mpDataLength = sizeof(data);

    MP_StartSendOnePacket(devExt,
                          &mpPacket,
                          mpData,
                          &mpDataLength,
                          va,
                          phys,
                          length,
                          &usbdStatus,
                          mpStatus);

}


VOID
USBPORT_EndTransmitTriggerPacket(
    __inout PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION devExt;
    USB_MINIPORT_STATUS mpStatus;
    HW_32BIT_PHYSICAL_ADDRESS phys;
    PUCHAR va, mpData;
    ULONG length, mpDataLength;
    MP_PACKET_PARAMETERS mpPacket;
    USBD_STATUS usbdStatus;
    UCHAR data[4];

    DbgTrace((UsbpDebugTrace, "'USBPORT_TransmitTriggerPacket\n"));

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    mpData = &data[0];
    mpDataLength = sizeof(data);

    length = devExt->Fdo.ScratchCommonBuffer->TotalLength;
    va = devExt->Fdo.ScratchCommonBuffer->MiniportVa;
    phys = devExt->Fdo.ScratchCommonBuffer->MiniportPhys;


    USBPORT_Wait(FdoDeviceObject, 10);

    MP_EndSendOnePacket(devExt,
                        &mpPacket,
                        mpData,
                        &mpDataLength,
                        va,
                        phys,
                        length,
                        &usbdStatus,
                        mpStatus);


    DbgTrace((UsbpDebugTrace, "'<ANALYZER TRIGER FIRED>\n"));
    NT_ASSERTMSG("Catc Trap",FALSE);

}


VOID
USBPORT_CatcTrap(
    __inout PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

Arguments:

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
        USBPORT_BeginTransmitTriggerPacket(FdoDeviceObject);
        devExt->Fdo.Flags.CatcTrap = 1;
    } else {
        TEST_TRAP();
        USBPORT_BeginTransmitTriggerPacket(FdoDeviceObject);
        USBPORT_EndTransmitTriggerPacket(FdoDeviceObject);
    }
}

VOID
USBPORT_AssertSig(PVOID Object,
                  ULONG Sig,
                  ULONG ExpectedSig
                  )
/*
    Used to assert data structures signatures. If these structures are
    corrupted then we bugcheck now to prevent furthur exploitation (eg
    buffer overruns).
*/
{
    USBPORT_ASSERT(Sig == ExpectedSig);

    if (Sig != ExpectedSig) {
        BUGCHECK(USBBUGCODE_BAD_SIGNATURE,  // BugCode
                 (ULONG_PTR)Object,         // BugCheckParameter2
                 (ULONG_PTR)ExpectedSig,    // BugCheckParameter3
                 (ULONG_PTR)0);             // BugCheckParameter4
    }
}

