;       title  "CPU and coprocessor primatives"
;++
;
; Copyright (c) 2003  Microsoft Corporation
;
; Module Name:
;
;    vector.asm
;
; Abstract:
;    
;    This module implements the ARM exception vector table which is mapped
;    via VBAR register.
;    
;    The vector table itself has enough room for a single instruction
;    per vector.  This instruction will load the PC via a PC-relative
;    load from a nearby array of vectors which contains the real OS
;    exception entry point.
;
; Environment:
;
;    Kernel mode only.
;
;--

#include "ksarm.h"

        AREA    |.text|,ALIGN=5,CODE,READONLY

        IMPORT KiDataAbortException
        IMPORT KiFIQException
        IMPORT KiInterruptException
        IMPORT KiPrefetchAbortException
        IMPORT KiSWIException
        IMPORT KiUndefinedInstructionException
        EXPORT KiArmExceptionVectors


#define VECTOR_OFFSET ((KiVectorTable - KiArmExceptionVectors) - 4)

        ALIGN   32                                      ; 32-byte alignment required
        
KiArmExceptionVectors
        ldr     pc, [pc, #VECTOR_OFFSET]                ; Reset
        ldr     pc, [pc, #VECTOR_OFFSET]                ; Undefined instruction
        ldr     pc, [pc, #VECTOR_OFFSET]                ; SWI exception
        ldr     pc, [pc, #VECTOR_OFFSET]                ; Prefetch abort
        ldr     pc, [pc, #VECTOR_OFFSET]                ; Data abort
        ldr     pc, [pc, #VECTOR_OFFSET]                ; unused vector location
        ldr     pc, [pc, #VECTOR_OFFSET]                ; IRQ exception
        ldr     pc, [pc, #VECTOR_OFFSET]                ; FIQ exception

KiVectorTable
        DCD     -1                                      ; Reset
        DCD     KiUndefinedInstructionException         ; Undefined instruction
        DCD     KiSWIException                          ; SWI exception
        DCD     KiPrefetchAbortException                ; Prefetch abort
        DCD     KiDataAbortException                    ; Data abort
        DCD     -1                                      ; unused vector
        DCD     KiInterruptException                    ; IRQ exception
        DCD     KiFIQException                          ; FIQ exception

EXCEPTION_REGION_END

        END

