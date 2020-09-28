        TTL  "Asynchronous Procedure Call Interrupt"
;++
;
; Copyright (c) 2009  Microsoft Corporation
;
; Module Name:
;
;   apcint.asm
;
; Abstract:
;
;   This module implements the code necessary to process the Asynchronous
;   Procedure Call interrupt request.
;
; Environment:
;
;   Kernel mode only.
;
;--

#include "ksarm.h"

        TEXTAREA

        IMPORT KiDeliverApc

;++
;
; Routine Description:
;
;   This routine generates an exception frame and attempts to deliver a user
;   APC.
;
; Arguments:
;
;   sp - Supplies the address of the trap frame.
;
; Return value:
;
;   None.
;
;--

        NESTED_ENTRY KiInitiateUserApc //, _TEXT$00
        GENERATE_EXCEPTION_FRAME                        ; generate exception frame

        movs    r0, #UserMode                           ; set APC processor mode
        mov     r1, sp                                  ; set exception frame address
        add     r2, sp, #KEXCEPTION_FRAME_LENGTH        ; set trap frame address
        CAPSTART KiInitiateUserApc, KiDeliverApc
        bl      KiDeliverApc                            ; deliver APC
        CAPEND  KiInitiateUserApc

        RESTORE_EXCEPTION_FRAME                         ; restore exception state
        EPILOG_RETURN

        NESTED_END KiInitiateUserApc //, _TEXT$00
        
        END

