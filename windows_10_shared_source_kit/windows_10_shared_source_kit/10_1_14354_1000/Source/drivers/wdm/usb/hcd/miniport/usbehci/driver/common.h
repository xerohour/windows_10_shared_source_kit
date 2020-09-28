/*++

Copyright (c) 1999, 2000  Microsoft Corporation

Module Name:

    usbehci.h

Abstract:



Environment:

    Kernel & user mode

Revision History:

    1-1-00 : created

--*/

#ifndef   __COMMON_H__
#define   __COMMON_H__

#if defined(XPSP)
#include "xpbp.h"
#endif

//-----------------------------------------------------------------------------
// 4127 -- Conditional Expression is Constant warning
//-----------------------------------------------------------------------------
#define WHILE(constant) \
__pragma(warning(disable: 4127)) while(constant) __pragma(warning(default: 4127))

#define WHILE_TRUE \
__pragma(warning(disable: 4127)) while(1) __pragma(warning(default: 4127))

#include "ntddk.h"
#include "stdarg.h"
#include "stdio.h"
#include "dsfhrmports.h"

#include "usb.h"

#pragma warning(push)
#pragma warning(disable: 4214) // bit field types other than int
#pragma warning(disable: 4201) // nameless struct/union

#include "usbhcdi.h"
#include "ehci.h"

#pragma warning(pop)

#include "dbg.h"

#include "usbehci.h"
#include "usbehcidump.h"


#ifdef NOINTERLOCKSUPPORTINCOMMONBUFFER
#undef CLEAR_FLAG
#undef SET_FLAG
#define CLEAR_FLAG(var, flag)   ((var) &= ~(flag))
#define SET_FLAG(var, flag)     ((var) |= (flag));
#endif

#include "usbport.events.h"

#endif // __COMMON_H__
