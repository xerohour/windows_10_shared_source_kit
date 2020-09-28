//
//    Copyright (C) Microsoft.  All rights reserved.
//
#ifndef   __COMMON_H__
#define   __COMMON_H__

//-----------------------------------------------------------------------------
// 4127 -- Conditional Expression is Constant warning
//-----------------------------------------------------------------------------
#define WHILE(constant) \
__pragma(warning(disable: 4127)) while(constant) __pragma(warning(default: 4127))

#define WHILE_TRUE \
__pragma(warning(disable: 4127)) while(1) __pragma(warning(default: 4127))

#if defined(XPSP)
// define for the XPSP build environment
#include <commonv.h>
#else
#include <initguid.h>
#include <ntddk.h>
#include <wdmguid.h>
#include <wdm.h>
#include <ntintsafe.h>
#include <ntstrsafe.h>

#define InterlockedOr _InterlockedOr
#define InterlockedAnd _InterlockedAnd

#include <windef.h>
#include <unknown.h>

#include <wmilib.h>
#include <wmikm.h>

#include <ursinterface.h>

#include "usb2lib.h"
#include "usb2cmn.h"

#include "usb.h"

#include "usbhcdi.h"

#include "hcdiguid.h"
#include "dbg.h"
#include "usbbugcode.h"

#include "acpiioct.h"

// enable irp tracking to catch
// double submits and other bugs
//#define TRACK_IRPS

// include all bus interfaces
#include "usbbusif.h"
#include "hubbusif.h"
//#include "tswin.h"

#define USB20_API
// inclulde ioctl defs for port drivers
#include "usbuser.h"
#include "usbioctl.h"

#include "usbintrl.h"

// include iodefs for client drivers and hub
#include "usbboot.h"

#pragma warning(push)
#pragma warning(disable: 4200) //  zero-sized array in struct/union
#pragma warning(disable: 4201) //  zero-sized array in struct/union
#pragma warning(disable: 4214) //  bit field types other than int

#include "smstructs.h"
#include "RTPMPublic_AutoGen.h"


#include "usbport.h"

#pragma warning(pop)

#include "prototyp.h"


#include "wmilog.h"

#include "trace.h"



#include "usbport.events.h"

#endif //XPSP

#include <dsfhrmports.h>

#ifdef ICE_CAP
//
// ICE CAP Header file must be copied from ICE cap release and placed in the current directory
//
#include "icecap.h"
#else
#define IceSetTraceState(x)
#endif

#define ICE_START_TRACE() IceSetTraceState(TRUE)
#define ICE_STOP_TRACE()  IceSetTraceState(FALSE)

FORCEINLINE
VOID
TRANSFER_LIST(
    __out
        PLIST_ENTRY dest_head,
    __in
        PLIST_ENTRY source_head
    )
{
    if (IsListEmpty(source_head)){
        InitializeListHead(dest_head);
    } else {
        dest_head->Flink = source_head->Flink;
        dest_head->Blink = source_head->Blink;
        dest_head->Flink->Blink = dest_head;
        dest_head->Blink->Flink = dest_head;
        InitializeListHead(source_head);
    }
}

FORCEINLINE
VOID
APPEND_LIST(
    __in
        PLIST_ENTRY dest_head,
    __in
        PLIST_ENTRY source_head
    )
{
    if (!IsListEmpty(source_head)) {
        dest_head->Blink->Flink   = source_head->Flink;
        source_head->Flink->Blink = dest_head->Blink;
        source_head->Blink->Flink = dest_head;
        dest_head->Blink          = source_head->Blink;
        InitializeListHead(source_head);
    }
}

#define FOR_ALL_IN_LIST(Type, Head, Field, Current)                 \
    for((Current) = CONTAINING_RECORD((Head)->Flink, Type, Field);  \
       (Head) != &(Current)->Field;                                 \
       (Current) = CONTAINING_RECORD((Current)->Field.Flink,        \
                                     Type,                          \
                                     Field)                         \
       )

#define FOR_ALL_IN_LIST_SAFE(Type, Head, Field, Current, Next)          \
    for((Current) = CONTAINING_RECORD((Head)->Flink, Type, Field),      \
            (Next) = CONTAINING_RECORD((Current)->Field.Flink,          \
                                       Type, Field);                    \
       (Head) != &(Current)->Field;                                     \
       (Current) = (Next),                                              \
            (Next) = CONTAINING_RECORD((Current)->Field.Flink,          \
                                     Type, Field)                       \
       )


#endif //__COMMON_H__
