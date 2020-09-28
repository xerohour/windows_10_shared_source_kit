//
//    Copyright (C) Microsoft.  All rights reserved.
//
/*++

Module Name:

    interrupt.h

Abstract:

   This file contains the declarations for 
   interrupt callbacks.

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel-mode Driver Framework

--*/

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_


//
// WDF Interrupt callback events
//

EVT_WDF_INTERRUPT_ISR           OnInterruptIsr;
EVT_WDF_INTERRUPT_WORKITEM      OnInterruptWorkItem;
EVT_WDF_INTERRUPT_DPC           OnInterruptDpc;

//
// ETW tracing event helpers
//

GUID 
_LogGetActivityId(
    _In_opt_    WDFREQUEST FxRequest
    );

#endif