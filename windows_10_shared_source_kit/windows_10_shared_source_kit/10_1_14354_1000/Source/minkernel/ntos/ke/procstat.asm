        TTL  "Processor State Save Restore"
;++
;
; Copyright (c) 2000  Microsoft Corporation
;
; Module Name:
;
;   procstat.asm
;
; Abstract:
;
;   This module implements routines to save and restore processor control
;   state.
;
; Environment:
;
;   Kernel mode only.
;
;--

#include "ksarm.h"

        TEXTAREA

        SUBT  "Get Current Stack Pointer"
;++
;
; ULONG_PTR
; KeGetCurrentStackPointer (
;     VOID
;     );
;
; Routine Description:
;
;   This function returns the caller's stack pointer.
;
; Arguments:
;
;   None.
;
; Return Value:
;
;   The callers stack pointer is returned as the function value.
;
;--

        LEAF_ENTRY KeGetCurrentStackPointer //, _TEXT$00

        mov     r0, sp
        bx      lr

        LEAF_END KeGetCurrentStackPointer //, _TEXT$00
        
        END

