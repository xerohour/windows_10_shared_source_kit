        TTL  "System Startup"
;++
;
; Copyright (c) 2003  Microsoft Corporation
;
; Module Name:
;
;   start.asm
;
; Abstract:
;
;   This module implements the code necessary to initially startup the NT
;   system on an ARM system.
;
; Environment:
;
;    Kernel mode only.
;
;--

#include "ksarm.h"

        IMPORT KdInitSystem
        IMPORT KeLoaderBlock
        IMPORT KfRaiseIrql
        IMPORT KiBarrierWait
        IMPORT KiIdleLoop
        IMPORT KiInitializeBootStructures
        IMPORT KiInitializeGSCookieValue
        IMPORT KiInitializeKernel
        IMPORT KiInitialPCR
        IMPORT KiDataAbortException

TotalFrameLength EQU (KEXCEPTION_FRAME_LENGTH + KSWITCH_FRAME_LENGTH + KSTACK_CONTROL_LENGTH + VFP_STATE_LENGTH)

        SUBT  "System Startup"
;++
;
; Routine Description:
;
;   This routine is called at system startup to perform early initialization
;   and to inititialize the kernel debugger. This allows breaking into the
;   kernel debugger very early during system startup. After kernel debugger
;   initialization, kernel initialization is performed. On return from kernel
;   initialization the idle loop is entered. The idle loop begins execution
;   and immediately finds the system startup (phase 1) thread ready to run.
;   Phase 1 initialization is performed and all other processors are started.
;   As each process starts it also passes through the system startup code, but
;   it does not initialization the kernel debugger.
;
; Arguments:
;
;   LoaderBlock (r0) - Supplies a pointer to the loader block.
;
; Implicit Arguments:
;
;   When the system starts up the loader has done some initialization. In
;   particular all structures have at least been zeroed.
;
;   The loader block has been formatted by the loader and all
;   pertinent fields have been filled in.
;
;   The address of the PRCB is passed in the loader block (only for processors
;   other than zero).
;
;   The address of the idle thread and idle process are passed in the loader
;   block (only for processors other than zero).
;
;   The stack register (SP) is loaded with the idle thread stack.
;
; Return Value:
;
;   None - function does not return.
;
;--

        LEAF_ENTRY KiSystemStartup, PAGELK

;
; Locals
;   r4 - address of loader block
;   r5 - address of prcb
;

;
; Save the address of the loader block.
;
; N.B. This is the same address for all processors.
;

        ldr     r1, =KeLoaderBlock                      ; get loader block ptr
        str     r0, [r1]                                ; save loader block to addr
        mov     r4, r0                                  ; save loader block to reg
        mov     r11, #0                                 ; terminate frame chaining

;
; Initialize PCR self address and the current PRCB address.
;

        ldr     r5, [r4, #LpbPrcb]                      ; get specified PRCB address
        ldr     r0, =(KiInitialPCR + PcPrcb)            ; get builtin PRCB
        cmp     r5, #0                                  ; is specified in loader block?
        moveq   r5, r0                                  ; yes, use loader block prcb
        sub     r0, r5, #PcPrcb                         ; compute PCR address
        CP_WRITE r0, CP15_TPIDRPRW                      ; Store PCR in proc reg
        str     r5, [r0, #PcCurrentPrcb]                ; set current PRCB address
        str     r0, [r0, #PcSelf]                       ; set PCR self address
        str     r5, [r4, #LpbPrcb]                      ; set loader block PRCB address

;
; Initialize kernel special registers, zero debug registers, setup
;   control registers, etc.
;

        ldr     r1, [r5, #PbPanicStackBase]             ; fetch the panic stack base from the PRCB
        cbnz    r1, %F1                                 ; if NULL, then we need info from the loader block
        ldr     r1, [r4, #LpbKernelStack]               ; use the KernelStack from the loader
        add     r1, r1, #DOUBLE_FAULT_STACK_SIZE        ; assume DF stack is after the kernel stack
        str     r1, [r5, #PbPanicStackBase]             ; use the KernelStack from the loader
1

        cps     #CPSRM_FIQ                              ; set FIQ stack
        add     r0, r5, #PbFiqMiniStack                 ; get offset of the FIQ mini-stack
        mov     sp, r0                                  ; set the new stack

        cps     #CPSRM_INT                              ; set INT stack
        add     r0, r5, #PbIrqMiniStack                 ; get offset of the INT mini-stack
        mov     sp, r0                                  ; set the new stack

        cps     #CPSRM_UDF                              ; set UDF stack
        add     r0, r5, #PbUdfMiniStack                 ; get offset of the UDF mini-stack
        mov     sp, r0                                  ; set the new stack

        cps     #CPSRM_ABT                              ; set ABT stack
        add     r0, r5, #PbAbtMiniStack                 ; get offset of the ABT mini-stack
        mov     sp, r0                                  ; set the new stack

        cps     #CPSRM_SVC                              ; back to SVC mode

;
; Initialize boot structures. Initialize the idle thread's stack
; variables so that the following debugger entry exception is
; considered within the idle stack's limits.
;

        mov     r0, r4                                  ; pass loader block address
        bl      KiInitializeBootStructures              ; initialize boot structures

;
; Initialize the kernel debugger if this is processor zero.
;

        movs    r0, #0                                  ; pass phase 0
        mov     r1, r4                                  ; pass loader block address
        ldr     r2, [r1, #LpbPrcb]                      ; get PRCB
        ldr     r3, [r2, #PbNumber]                     ; get processor number
        cbnz    r3, %F0                                 ; if ne, not BSP
        bl      KdInitSystem                            ; initialize debugger
0

;
; At this point exception handlers are initialized, and the debugger is ready
; to go, so enable asynchronous aborts to clear out anything pending from
; boot.
;

        cpsie   a                                       ; enable asynchronous aborts
        nop                                             ; small landing pad

;
; Raise IRQL to high level and initialize the kernel.
;

        mov     r1, #HIGH_LEVEL                         ; get HIGH_LEVEL
        RAISE_IRQL r0, r1                               ; set IRQL

;
; Reserve space for idle thread stack initialization.
;
; N.B. This reservation ensures that the initialization of the thread stack
;      does not overwrite any information on the current stack which is the
;      same stack.
;

        ldr     r0, =TotalFrameLength                   ; get reserve size
        sub     sp, sp, r0                              ; allocate except/switch frames

;
; Create a frame with a 0 PC and 0 r11 (FP) so that the frame chain ends with a
; PC and R11 of 0 for stack walks in kernel initialization.
;

        sub     sp, sp, #8                              ; allocate an empty frame
        str     r11, [sp]                               ; zero the saved r11
        str     r11, [sp, #4]                           ; zero the saved lr
        mov     r11, sp                                 ; set r11 to point to the frame

;
; Initialize kernel.
;
; N.B. Kernel initialization is called with interupts disabled at IRQL
;      HIGH_LEVEL and returns with interrupt enabled at IRQL DISPATCH_LEVEL.
;

        sub     sp, sp, #8                              ; allocate stack args
        str     r4, [sp, #4]                            ; pass loader block address
        ldr     r0, [r5, #PbNumber]                     ; get processor number
        str     r0, [sp]                                ; pass processor number

        ldr     r0, [r4, #LpbProcess]                   ; pass idle process address
        ldr     r1, [r4, #LpbThread]                    ; pass idle thread address
        add     r2, sp, #(TotalFrameLength+16)          ; pass initial stack
        str     r2, [r5, #PbSpBase]                     ; set initial stack in PRCB
        mov     r3, r5                                  ; pass PRCB address
        bl      KiInitializeKernel                      ; Initialize kernel

        ldr     r0, [r5, #PbNumber]                     ; check if this is BSP
        cbnz    r0, %F0
        bl      KiInitializeGSCookieValue               ; randomize security cookie
0
;
; Reset stack to include only the space for the KSTACK
;
; N.B. Interrupts may be enabled at this point.
;

        ldr     r1, [r5, #PbSpBase]                     ; get idle stack address
        ldr     r0, =VFP_STATE_LENGTH
        sub     r1, r1, r0                              ; Allocate VFP save area
        movs    r0, #KSTACK_CONTROL_LENGTH
        sub     r1, r1, r0                              ; Allocate KSTACK control
        mov     sp, r1                                  ; set new stack pointer
        mov     r11, #0                                 ; clear the frame chain

;
; Set the wait IRQL for the idle thread.
;

        CURTHREAD_READ r0                               ; get current thread address
        movs    r1, #DISPATCH_LEVEL                     ; set wait irql
        strb    r1, [r0, #ThWaitIrql]                   ; set wait irql

;
; In a multiprocessor system the boot processor proceeds directly into the
; idle loop. As other processors start executing, however, they do not enter
; the idle loop directly - they spin until all processors have been started
; and the boot master allows them to proceed.
;

#ifndef NT_UP


2       ldr     r0, =KiBarrierWait                      ; load barrier ptr
        ldr     r0, [r0]                                ; read barrier
        cmp     r0, #0                                  ; check if barrier set

        yield                                           ; yield processor execution

        bne     %B2                                     ; if barrier set, loop
        dmb     ish                                     ; execute a memory barrier

#endif

        bl      KiIdleLoop                              ; enter idle loop - no return
        bx      lr                                      ; do not remove

        LEAF_END KiSystemStartup

        END

