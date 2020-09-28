;        title  "Zero Page"
;++
;
; Copyright (c) 2001  Microsoft Corporation
;
; Module Name:
;
;   zero.asm
;
; Abstract:
;
;   This module implements the architecture dependent code necessary to
;   zero pages of memory in the fastest possible way.
;
; Environment:
;
;   Kernel mode only.
;
;--

#include "ksarm.h"

        TEXTAREA

;        subttl  "Zero Page"
;++
;
; VOID
; KeZeroPages (
;     IN PVOID PageBase,
;     IN SIZE_T NumberOfBytes
;     )
;
; Routine Description:
;
;   This routine zeros the specified pages of memory.
;
; Arguments:
;
;   PageBase (r0) - Supplies the address of the pages to zero.
;
;   NumberOfBytes (r1) - Supplies the number of bytes to zero.  Always
;       a PAGE_SIZE multiple.
;
; Return Value:
;
;    None.
;
;--

        NESTED_ENTRY KeZeroPages
        PROLOG_PUSH     {r4-r7, r11, lr}

        movs    r2, #0                                  ; zero 8 registers
        movs    r3, #0                                  ;
        movs    r4, #0                                  ;
        movs    r5, #0                                  ;
        movs    r6, #0                                  ;
        movs    r7, #0                                  ;
        mov     r12, r2                                 ;
        mov     lr, r2                                  ;
        lsr     r1, #8                                  ; number of 256 byte chunks (loop count)

;
; N.B.  The zero page loop is carefully laid out to conform to the requirements
;       for fast loop semantics.
;

ZpLoop  subs    r1, r1, #1                              ; decrement loop count
        stmia   r0!, {r2-r7, r12, lr}                   ; clear 256 bytes
        stmia   r0!, {r2-r7, r12, lr}                   ;
        stmia   r0!, {r2-r7, r12, lr}                   ;
        stmia   r0!, {r2-r7, r12, lr}                   ;
        stmia   r0!, {r2-r7, r12, lr}                   ;
        stmia   r0!, {r2-r7, r12, lr}                   ;
        stmia   r0!, {r2-r7, r12, lr}                   ;
        stmia   r0!, {r2-r7, r12, lr}                   ;
        bne     ZpLoop                                  ; if ne, more bytes to zero

        EPILOG_POP      {r4-r7, r11, pc}

        NESTED_END KeZeroPages


;++
;
; VOID
; KeZeroSinglePage (
;     IN PVOID PageBase
;     )
;
; Routine Description:
;
;   This routine zeros the specified page of memory using normal moves.
;
; Arguments:
;
;   PageBase (r0) - Supplies the address of the pages to zero.
;
; Return Value:
;
;    None.
;
;--

        LEAF_ENTRY KeZeroSinglePage

        ldr     r1, =PAGE_SIZE
        b       KeZeroPages

        LEAF_END KeZeroSinglePage

        END

