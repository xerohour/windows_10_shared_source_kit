;++
;
; Copyright (C) Microsoft. All rights reserved.
;
; Module Name:
;
;    transita.asm
;
; Abstract:
;
;   ASM routines for transitioning control between boot applications.
;
;--


#include <ksarm.h>

    TEXTAREA

;++
;
; BOOLEAN
; BlpArchQueryInterruptStatus (
;    VOID
;    );
;
; Routine Description:
;
;     This routines queries the status of interrupts on the system.
;
; Arguments:
;
;     None.
;
; Return Value:
;
;     FALSE if interrupts are globally disabled.
;
;     TRUE if interrupts are globally enabled.
;
;--

        LEAF_ENTRY BlpArchQueryInterruptStatus

        READ_CPSR r1                                    ; get cpsr
        tst    r1, #CPSRC_INT                           ; are interrupts enabled?

;
; N.B. These assignments look backwards but CPSRC_INT is a
;      interrupt disable bit rather than traditional enable bit.
;

        movne  r0, #FALSE                               ; if ne, ints were disabled
        moveq  r0, #TRUE                                ; if eq, ints were enabled
        bx     lr

        LEAF_END BlpArchQueryInterruptStatus

;++
;
; VOID
; BlpArchTransferToApplicationState
;    __in PBOOT_APPLICATION_ENTRYPOINT EntryRoutine,
;    __inout PBOOT_APPLICATION_PARAMETERS EntryParameters,
;    __in PVOID Stack,
; )
;
; Routine Description:
;
;   This routine will transfer control to an ARM boot application.
;
; Arguments:
;
;   EntryRoutine A pointer to the entry routine for the new boot application.
;
;   EntryParameters - A pointer to the entry parameters for the new
;       boot application.
;
;   Stack - A pointer to memory allocated for the new boot
;       application's stack.
; 
; Return Value:
;
;   None.
;
;--
    NESTED_ENTRY BlpArchTransferToApplicationState
        PROLOG_PUSH {r4, lr}

        mov     r4, sp                                 ; save incoming stack
        cps     #CPSRM_SVC                             ; go to svc mode

        mov     sp, r2                                 ; switch to new stack
        mov     r2, r0
        mov     r0, r1
        blx     r2

        mov     sp, r4

        EPILOG_POP {r4, pc}
    NESTED_END BlpArchTransferToApplicationState

    END

