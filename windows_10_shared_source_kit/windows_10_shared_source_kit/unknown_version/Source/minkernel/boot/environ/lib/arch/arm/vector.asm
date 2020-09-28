;       title  "CPU and coprocessor primatives"
;++
;
; Copyright (C) Microsoft. All rights reserved.
;
; Module Name:
;
;    vector.asm
;
; Abstract:
;    
;    This module implements the ARM exception vector table which can
;    be located at 0x00000000 or 0xFFFF0000.  The NT loader initialize
;    the processor such that the vector table appears at
;    0xFFFF0000.
;    
;    The vector table itself consists of a room for a single
;    instruction per vector.  This instruction will load the PC via a
;    PC-relative load from a nearby array of vectors which will
;    contain the real OS exception entry points.
;
; Environment:
;
;    Kernel mode only.
;
;--

#include "ksarm.h"


        AREA    |.text|,ALIGN=5,CODE,READONLY

        EXPORT ArchStubExceptionVectors

;
; This set of stub exception vectors each invokes an endless loop.  It
; is intended to be installed when the boot loader debugger is not
; active or before switching to a new application.
;

        ALIGN   32                                      ; 32-byte alignment required
        
ArchStubExceptionVectors
ArchResetException
        b       ArchResetException
        nop
ArchUndefinedException
        b       ArchUndefinedException
        nop
ArchSWIException
        b       ArchSWIException
        nop
ArchPrefetchException
        b       ArchPrefetchException
        nop
ArchDataException
        b       ArchDataException
        nop
ArchUnusedException
        b       ArchUnusedException
        nop
ArchIrqException
        b       ArchIrqException
        nop
ArchFiqException
        b       ArchFiqException
        nop

        END

