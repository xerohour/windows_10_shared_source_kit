;       title  "Idle Loop"
;++
;
; Copyright (c) 2000-2003  Microsoft Corporation
;
; Module Name:
;
;   idle.asm
;
; Abstract:
;
;   This module implements the platform specific idle loop.
;
; Environment:
;
;    Kernel mode only.
;
;--

#include "ksarm.h"

#ifdef _HYPERVISOR_

        IMPORT  HvlEnableIdleYield
        IMPORT  HvlEnlightenments
        IMPORT  HvlLongSpinCountMask
        IMPORT  HvlNotifyLongSpinWait

#endif

        IMPORT  KiCaptureTotalCyclesCurrentThread
        IMPORT  KiFatalExceptionHandler
        IMPORT  KiIdleSchedule
        IMPORT  KiQueryEffectivePriorityThread
        IMPORT  KiRetireDpcList
        IMPORT  KiQuantumEnd

        IMPORT  PoIdle
        IMPORT  SwapContext

;++
; VOID
; KiIdleLoop (
;     VOID
;     )
;
; Routine Description:
;
;    This routine continuously executes the idle loop and never returns.
;
; Arguments:
;
;    None.
;
; Return value:
;
;    This routine never returns.
;
;--

        NESTED_ENTRY KiIdleLoop,,KiFatalExceptionHandler

        PROLOG_NOP mov lr, #0                           ; zero the return address to terminate the unwind here
        PROLOG_PUSH {r11, lr}                           ; save return address
        b       KiIL20                                  ; skip idle processor on first iteration

;
; There appear to be no entries in the DPC list and a thread has not been
; selected for execution on this processor. Recheck the DPC status (a barrier
; was issued when idle halt was set), and then call the HAL so power managment
; can be performed as appropriate.
;
; N.B. The HAL is called with interrupts disabled. The HAL will return
;      with interrupts enabled.
;

KiIL10  ldrb     r0, [r4, #PbNormalDpcState]            ; get dpc state
        tst      r0, #KI_DPC_ALL_FLAGS                  ; test pending DPC
        bne      KiIL15                                 ; if nz, DPC pending, do not enter idle state

;
; N.B. Strongly resist the temptation to change this code so that it calls
;      PoIdle() at PASSIVE_LEVEL.  The effect is that when interrupts are
;      re-enabled a clock interrupt will request a dispatch interrupt which
;      will then cause a rescheduling of threads and put the idle loop thread
;      onto the ready thread queues.  After that the idle loop will be
;      scheduled as a normal thread and ready threads will sit unscheduled
;      ad-infinitum.
;

        mov      r1, #DISPATCH_LEVEL                    ; get dispatch level
        RAISE_IRQL r0, r1                               ; set irql

        mov      r0, r4                                 ; set address of processor block
        bl       PoIdle                                 ; call idle function
        cpsie    i                                      ; enable interrupts/avoid spurious

KiIL15                                                  ; reference label

        strb     r7, [r4, #PbIdleHalt]                  ; clear idle halt

;
; Disable interrupts and check if there is any work in the DPC list of the
; current processor or a target processor.
;
; N.B. The following code enables interrupts for a few cycles, then disables
;      them again for the subsequent DPC and next thread checks.
;

KiIL20                                                  ; reference label

;
; ARM: reload r4 with PRCB and r7 with #0 here; this is both our
; first initialization point, as well as the location immediately
; after SwapContext is called. SwapContext does not preserve non-
; volatile registers, so we must reload things after calling it.
;

        PCR_READ r4                                     ; get pointer to PCR
        add     r4, r4, #PcPrcb                         ; get current processor block address
        movs    r7, #0                                  ; use r7 as a 0 register

#ifdef _HYPERVISOR

        ldr     r0, =HvlEnableIdleYield
        ldrb    r0, [r0]
        cbz     r0, KiIL25                              ; check for yield bypass; if e, do not yield

#endif

        yield                                           ; yield processor execution
KiIL25  cpsie   i                                       ; enable interrupts
        nop                                             ;
        nop                                             ;
        cpsid   i                                       ; disable interrupts

;
; Process the deferred procedure call list for the current processor.
;

#if DBG

        ldr     r0, [r4, #PbDeferredReadyListHead]
        cbz     r0, %F1
        EMIT_BREAKPOINT

#endif

1       ldrb    r0, [r4, #PbNormalDpcState]             ; get DPC request status
        tst     r0, #KI_DPC_ALL_FLAGS                   ; test DPCs
        beq     CheckQuantumEnd                         ; if z, no DPCs to process

        mov     r0, r4                                  ; pass current PRCB address
        CAPSTART KiIdleLoop, KiRetireDpcList
        bl      KiRetireDpcList                         ; process the current DPC list
        CAPEND  KiIdleLoop

CheckQuantumEnd
        ldrb    r0, [r4, #PbQuantumEnd]                 ; get quantum end status
        cbz     r0, CheckNextThread                     ; if e, no quantum end request
        movs    r0, #0
        strb    r0, [r4, #PbQuantumEnd]                 ; clear quantum end indicator
        cpsie   i                                       ; enable interrupts
        CAPSTART KiIdleLoop, KiQuantumEnd
        bl      KiQuantumEnd                            ; process Quantum End
        CAPEND  KiIdleLoop
        cpsid   i                                       ; disable interrupts

;
; Check if a thread has been selected to run on the current processor.
;
; N.B. The variable idle halt is only written on the owning processor.
;      It is only read on other processors. This variable is set when
;      the respective processor may enter a sleep state. The following
;      code sets the variable under interlocked which provides a memory
;      barrier, then checks to determine if a thread has been schedule.
;      Code elsewhere in the system that reads this variable, set next
;      thread, executes a memory barrier, then reads the variable.
;

CheckNextThread
        movs    r0, #1
        strb    r0, [r4, #PbIdleHalt]                   ; set idle halt
        dmb     ish                                     ; invoke memory barrier -- is this right?

        ldr     r0, [r4, #PbNextThread]                 ; check if thread slected
        cmp     r0, r7                                  ; if e, no thread selected
        beq     KiIL50                                  ;
        strb    r7, [r4, #PbIdleHalt]                   ; clear idle halt
        cpsie   i                                       ; enable interrupts

;
; Acquire the PRCB Lock.
;

        ldr     r6, [r4, #PbIdleThread]                 ; get idle thread address

        ACQUIRE_SPIN_LOCK r4, #PbPrcbLock               ; acquire PRCB lock (trashes r0-r2)

        ldr     r5, [r4, #PbNextThread]                 ; get next thread address
        str     r7, [r4, #PbNextThread]                 ; clear next thread address

;
; If a thread had been scheduled for this processor, but was removed from
; eligibility (e.g., an affinity change), then the new thread could be the
; idle thread.
;

        cmp     r5, r6                                  ; check if swap from idle to idle
        beq     KiIL40                                  ; if eq, idle to idle

;
; Capture total cycles for the idle thread before the context switch.
;
; N.B. Runtime is not accumulated for the idle thread.
;
; N.B. This must be done with interrupts disabled.
;

        cpsid   i                                       ; disable interrupts
        ldrb    r0, [r4, #PbNestingLevel]               ; begin interrupt time
        adds    r0, r0, #1                              ;
        strb    r0, [r4, #PbNestingLevel]               ;
        READ_CYCLE_COUNTER_64BIT r0, r1, r2, r3, r4     ; read "time stamp" counter
        add     r8, r4, #PbStartCycles                  ; pre-compute offset of ldrd/strd
        ldrd    r2, r3, [r8]                            ; compute total cycles for period
        strd    r0, r1, [r8]                            ; compute start of next period
        subs    r0, r0, r2                              ;
        sbcs    r1, r1, r3                              ;
        add     r9, r6, #ThCycleTime                    ;
0       ldrexd  r2, r3, [r9]                            ; accumulate cycles
        adds    r2, r2, r0                              ;
        adcs    r3, r3, r1                              ;
        strexd  r12, r2, r3, [r9]                       ;
        cmp     r12, #1                                 ; did it work?
        beq     %B0                                     ; if not, try again
        cpsie   i                                       ; enable interrupts

#if DBG

        mov         r0, r5                              ; load arguments
        mov         r1, r4                              ;
        bl          KiQueryEffectivePriorityThread      ; check for rank bias
        ldr         r2, [r4, #PbPriorityState]          ; load priority state
        ldrb        r1, [r2]                            ; load previous priority
        cmp         r0, r1                              ; compare priorities
        beq         %F0                                 ; priority matches
        EMIT_BREAKPOINT                                 ; corrupted state

#endif

;
; Set the current thread address and set the thread state to running.
;

0       str     r5, [r4, #PbCurrentThread]              ; set current thread address
        movs    r0, #Running                            ; set new thread state
        strb    r0, [r5, #ThState]

;
; N.B. A barrier (embeddeded in spinlock release) is used to order the read of
;      the DPC state with the write to the current thread field in the PRCB
;      above.
;

#if DBG

        ldrb    r0, [r4, #PbIdleSchedule]               ; test idle schedule
        cbz     r0, %F1                                 ; if nz, break
        EMIT_BREAKPOINT
1

#endif

        RELEASE_SPIN_LOCK r4, #PbPrcbLock, r7           ; release current PRCB lock

;
; Switch context to new thread. Check if DPCs are pending as this is ordered
; after a current thread update. As such, it should see any DPC request that
; may have bypassed issuing a dispatch interrupt based on the state of the
; current thread. Note that a dispatch interrupt is requested rather than
; retiring DPCs inline as it is likely an interrupt is already pending.
;

KiIL30

        cmp     r5, r6                                  ; check swap from idle to idle
        bne     %F1

        FASTFAIL #FAST_FAIL_INVALID_NEXT_THREAD         ; fast fail invalid next thread
1

; Revisit this; currently PbInterruptRequest doesn't exist for ARM
;       ldrb    r0, [r4, #PbNormalDpcState]             ; check DPC pending
;       movs    r1, #1                                  ;
;       tst     r0, #KI_DPC_INTERRUPT_FLAGS             ;
;       strbne  r1, [r4, #PbInterruptRequest]           ; merge interrupt request

        mov     r0, r6                                  ; set old thread
        mov     r1, r5                                  ; set new thread
        movs    r2, #APC_LEVEL                          ; set APC bypass disable
        mov     r3, r4                                  ; set PRCB
        CAPSTART KiIdleLoop, SwapContext
        bl      SwapContext                             ; swap context to next thread
        CAPEND KiIdleLoop
        b       KiIL20                                  ; loop

;
; The new thread is the Idle thread (same as old thread).  This can happen
; rarely when a thread scheduled for this processor is made unable to run
; on this processor. As this processor has again been marked idle, other
; processors may unconditionally assign new threads to this processor.
;

KiIL40
        RELEASE_SPIN_LOCK r4, #PbPrcbLock, r7           ; release current PRCB lock
        b       KiIL20                                  ; loop

;
; Call idle schedule if requested.
;

KiIL50  ldrb    r1, [r4, #PbIdleSchedule]               ; check if idle schedule
        cmp     r1, r7                                  ;
        beq     KiIL10                                  ; if e, idle schedule not requested
        strb    r7, [r4, #PbIdleHalt]                   ; clear idle halt
        cpsie   i                                       ; enable interrupts
        mov     r0, r4                                  ; pass current PRCB address
        CAPSTART KiIdleLoop, KiIdleSchedule
        bl      KiIdleSchedule                          ; attempt to schedule thread
        CAPEND KiIdleLoop
        cmp     r0, r7                                  ; test if new thread scheduled
        mov     r5, r0                                  ; set new thread address
        ldr     r6, [r4, #PbIdleThread]                 ; get idle thread address
        bne     KiIL30                                  ; if nz, new thread scheduled
        b       KiIL20

        NESTED_END KiIdleLoop

        END

