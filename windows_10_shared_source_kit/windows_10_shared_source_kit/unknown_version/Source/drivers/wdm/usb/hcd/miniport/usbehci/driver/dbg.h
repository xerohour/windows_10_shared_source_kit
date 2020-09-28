/*++

Copyright (c) 1999, 2000  Microsoft Corporation

Module Name:

    dbg.h

Abstract:

    debug macros

Environment:

    Kernel & user mode

Revision History:

    6-20-99 : created

--*/

#ifndef   __DBG_H__
#define   __DBG_H__

//
// Structure signatures
//

#define EHCI_TAG          'ehci'        //"EHCI"

#if DBG

#define DEBUG_LOG

// Triggers a break in the debugger in the registry key
// debugbreakOn is set.  These breakpoins are useful for
// debugging hardware/client software problems
//

#define DEBUG_BREAK(dd)  RegistrationPacket.USBPORTSVC_TestDebugBreak;

//
// This Breakpoint means we either need to test the code path
// somehow or the code is not implemented.  ie either case we
// should not have any of these when the driver is finished
//

#define TEST_TRAP(dd)     RegistrationPacket.USBPORTSVC_TestDebugBreak((dd))



#define ASSERT_TRANSFER(dd, t) EHCI_ASSERT((dd), (t)->Sig == SIG_EHCI_TRANSFER)

#define ASSERT_TD(dd, t) EHCI_ASSERT((dd), (t)->Sig == SIG_HCD_TD)
#define ASSERT_SITD(dd, t) EHCI_ASSERT((dd), (t)->Sig == SIG_HCD_SITD)
#define ASSERT_ITD(dd, t) EHCI_ASSERT((dd), (t)->Sig == SIG_HCD_ITD)

#define ASSERT_DUMMY_TD(dd, t) \
    EHCI_ASSERT((dd), (t)->NextHcdTD.Pointer == NULL);\
    EHCI_ASSERT((dd), (t)->AltNextHcdTD.Pointer == NULL);

ULONG
_cdecl
EHCI_KdPrintX(
    __in PVOID DeviceData,
    ULONG Level,
    __in PSTR Format,
    ...
    );

#define   EHCI_KdPrint(_x_) EHCI_KdPrintX _x_

#define EHCI_ASSERT(dd, exp ) {\
    NT_ASSERT(exp);\
    __pragma(warning(disable: 4127)) if (!(exp)) {__pragma(warning(default: 4127))\
        RegistrationPacket.USBPORTSVC_AssertFailure( (dd), #exp, __FILE__, __LINE__, NULL );\
    }\
}

#define EHCI_ASSERTMSG(dd, msg, exp ) {\
    NT_ASSERTMSG(msg, exp);\
    __pragma(warning(disable: 4127)) if (!(exp)) {__pragma(warning(default: 4127))\
        RegistrationPacket.USBPORTSVC_AssertFailure( (dd), #exp, __FILE__, __LINE__, NULL );\
    }\
}

#define EHCI_QHCHK(dd, ed)  EHCI_AssertQhChk(dd, ed);

#else
// keep debug logging on in retail
// so we can debug stress failires and such
#define DEBUG_LOG

// debug macros for retail build

#define TEST_TRAP(dd)
#define DEBUG_BREAK(dd)

#define ASSERT_TRANSFER(dd, t)

#define ASSERT_DUMMY_TD(dd, t)
#define ASSERT_TD(dd, t)
#define ASSERT_SITD(dd, t)
#define ASSERT_ITD(dd, t)


#define EHCI_ASSERT(dd, exp )
#define EHCI_ASSERTMSG(dd, msg, exp )

#define   EHCI_KdPrint(_x_)

#define EHCI_QHCHK(dd, ed)

#endif /* DBG */

// retail and debug

#ifdef DEBUG_LOG

#define SWIZZLE(sig) \
    ((((sig) >> 24) & 0x000000FF) | \
     (((sig) >>  8) & 0x0000FF00) | \
     (((sig) <<  8) & 0x00FF0000) | \
     (((sig) << 24) & 0xFF000000))

#define LOGENTRY(dd, mask, sig, info1, info2, info3)  \
    RegistrationPacket.USBPORTSVC_LogEntry( (dd), (mask), SWIZZLE(sig), \
        (ULONG_PTR)(info1), (ULONG_PTR)(info2), (ULONG_PTR)(info3) )

#else

#define LOGENTRY(dd, mask, sig, info1, info2, info3)

#endif


#endif /* __DBG_H__ */

