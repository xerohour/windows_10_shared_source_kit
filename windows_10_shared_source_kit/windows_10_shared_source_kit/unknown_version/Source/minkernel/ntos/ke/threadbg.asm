        TTL  "Thread Startup"
;++
;
; Copyright (c) 2000  Microsoft Corporation
;
; Module Name:
;
;    threadbg.asm
;
; Abstract:
;
;    This module implements the code necessary to startup a thread in kernel
;    mode.
;
; Environment:
;
;    Kernel mode only, IRQL APC_LEVEL.
;
;--

#include "ksarm.h"

        IMPORT  KeBugCheck
        IMPORT  __imp_KfLowerIrql
        IMPORT  KiExceptionRestore

        SUBT  "System Thread Startup"

;++
;
; Routine Description:
;
;   This routine is called to start a system thread. This function calls the
;   initial thread procedure after having extracted the startup parameters
;   from the specified start frame. If control returns from the initial
;   thread procedure, then a bug check will occur.
;
; Implicit Arguments:
;
;   N.B. This function begins execution at its alternate entry point with
;        a start frame on the stack. This frame contains the start context,
;        the start routine, and the system routine.
;
; Return Value:
;
;    None - no return.
;
;--

        NESTED_ENTRY KiStartSystemThreadFake

        PROLOG_PUSH     {r0-r2, lr}

        ALTERNATE_ENTRY KiStartSystemThread

        mov     r0, #APC_LEVEL                          ; get APC_LEVEL
        ldr     r3, =__imp_KfLowerIrql                  ; get thunk address
        ldr     r3, [r3]                                ; get routine address
        blx     r3                                      ; lower IRQL

        ldr     r0, [sp, #SfR1]                         ; get start routine
        ldr     r1, [sp, #SfR0]                         ; get start context
        ldr     r2, [sp, #SfR2]                         ; get system routine

        blx     r2                                      ; call system routine

        movs    r0, #NO_USER_MODE_CONTEXT               ; set bug check parameter
        bl      KeBugCheck                              ; call bug check - no return
        EMIT_BREAKPOINT                                 ; should never get here

        NESTED_END KiStartSystemThreadFake

        SUBT  "User Thread Startup"
;++
;
; Routine Description:
;
;   This routine is called to start a user thread. This function calls the
;   initial thread procedure after having extracted the startup parameters
;   from the specified exception frame. If control returns from the initial
;   thread routine, then the user mode context is restored and control is
;   transfered to the exception exit code.
;
; Implicit Arguments:
;
;   N.B. This function begins execution with a trap frame, an exception frame
;        and a start frame on the stack. The trap and exception frames represent
;        the user mode context. The start frame contains the start context,
;        start routine, and the system routine parameters.
;
; Return Value:
;
;    None.
;
;--

        NESTED_ENTRY KyStartUserThread

        PROLOG_STACK_ALLOC      KTRAP_FRAME_LENGTH      ; generate trap frame

        bl      KxStartUserThread                       ; call dummy startup routine

        ALTERNATE_ENTRY KiStartUserThreadReturn

        nop                                             ; do not remove

        NESTED_END KyStartUserThread


        NESTED_ENTRY KxStartUserThread

        GENERATE_EXCEPTION_FRAME                        ; generate exception frame
        PROLOG_STACK_ALLOC KSTART_FRAME_LENGTH

        ALTERNATE_ENTRY KiStartUserThread

        mov     r0, #APC_LEVEL                          ; get APC_LEVEL
        ldr     r3, =__imp_KfLowerIrql                  ; get thunk address
        ldr     r3, [r3]                                ; get routine address
        blx     r3                                      ; lower IRQL

        ldr     r0, [sp, #SfR1]                         ; get start routine
        ldr     r1, [sp, #SfR0]                         ; get start context
        ldr     r2, [sp, #SfR2]                         ; get system routine
        blx     r2                                      ; call system routine

        EPILOG_STACK_FREE KSTART_FRAME_LENGTH
        RESTORE_EXCEPTION_FRAME                         ; restore exception state
        EPILOG_BRANCH KiExceptionRestore                ;

        NESTED_END KxStartUserThread

        END

