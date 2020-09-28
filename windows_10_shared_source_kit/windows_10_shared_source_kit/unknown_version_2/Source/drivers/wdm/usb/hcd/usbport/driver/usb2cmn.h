/*++

Copyright (c) Microsoft Corporation

Module Name:

    usb2cmn.h

Abstract:

    Common structures between budget code and USBPORT core code

Environment:

    Kernel & user mode

Revision History:

    10-31-00 : created

--*/

#ifndef   __USB2CMN_H__
#define   __USB2CMN_H__

#include <common.h>

#include "usb2lib.h"
#include "sched.h"

VOID
USB2LIB_DbgPrintMsg(
    __in PCSTR Msg
    );   

VOID
USB2LIB_DbgPrintUlong(
    __in PCSTR Msg,
    __in ULONG Arg
    );    

VOID
USB2LIB_DbgPrintUchar(
    __in PCSTR Msg,
    __in UCHAR Arg
    );    
    
VOID
USB2LIB_DbgPrintPtr(
    __in PCSTR Msg,
    __in_opt PVOID ArgPtr
    );


typedef struct _USB2LIB_HC_CONTEXT {

    ULONG Sig;
    // context for host controller
    HC Hc;
    // TT used for HS endpoints to get to HC struct
    TT NullTt;  
    
} USB2LIB_HC_CONTEXT, *PUSB2LIB_HC_CONTEXT;


typedef struct _USB2LIB_TT_CONTEXT {

    ULONG Sig;
    TT Tt;
    
} USB2LIB_TT_CONTEXT, *PUSB2LIB_TT_CONTEXT;


typedef struct _USB2LIB_ENDPOINT_CONTEXT {

    ULONG Sig;
    PVOID RebalanceContext;
    USB2_EP Ep;
    
} USB2LIB_ENDPOINT_CONTEXT, *PUSB2LIB_ENDPOINT_CONTEXT;


/*
    prototypes
*/

#endif /*  __USB2CMN_H__ */


