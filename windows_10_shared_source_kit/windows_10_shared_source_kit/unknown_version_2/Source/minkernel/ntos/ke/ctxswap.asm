       TTL  "Context Swap"
;++
;
; Copyright (c) 2000  Microsoft Corporation
;
; Module Name:
;
;   ctxswap.asm
;
; Abstract:
;
;   This module implements the code necessary to field the dispatch interrupt
;   and perform context switching.
;
; Environment:
;
;    Kernel mode only.
;
;--

#include "ksarm.h"

#if ARM_VFP_ENABLE_STATISTICS
        IMPORT KiArmVfpStats
#endif
        IMPORT EtwTraceContextSwap
        IMPORT KeBugCheckEx
        IMPORT KeInterlockedClearProcessorAffinityEx
        IMPORT KeInterlockedSetProcessorAffinityEx
        IMPORT KiAbProcessContextSwitch
        IMPORT KiBeginThreadAccountingPeriod
        IMPORT KiDeferGroupSchedulingPreemption
        IMPORT KiEndThreadAccountingPeriod
        IMPORT KiFatalExceptionHandler
        IMPORT KiQuantumEnd
        IMPORT KiQueryEffectivePriorityThread
        IMPORT KiQueueReadyThread
        IMPORT KiRequestSoftwareInterrupt
        IMPORT KiRetireDpcList
        IMPORT KxSwapProcess
        IMPORT PerfGlobalGroupMask

#ifdef _HYPERVISOR_

        IMPORT HvlEnlightenments
        IMPORT HvlLongSpinCountMask
        IMPORT HvlNotifyLongSpinWait
        IMPORT HvlSwitchVirtualAddressSpace:proc

#endif

        SUBT  "Swap Context"
        TEXTAREA

        ;
        ; Helper macro for statistics cleanliness
        ;

        MACRO
        INCREMENT_VFP_STAT $StatName, $t1, $t2, $t3, $t4
#if ARM_VFP_ENABLE_STATISTICS
        mov32   $t1, KiArmVfpStats + $StatName           ; get pointer to attempted demotions stat
        INCREMENT_STAT $t1, $t2, $t3, $t4                ; increment the statistic
#endif
        MEND

;++
;
; BOOLEAN
; KiSwapContext (
;    IN PKTHREAD OldThread,
;    IN PKTHREAD NewThread,
;    IN KIRQL SwapIrql
;    )
;
; Routine Description:
;
;   This function is a small wrapper that marshalls arguments and calls the
;   actual swap context routine.
;
;   N.B. The current thread address and the new thread state has been set
;        before this routine is called.
;
; Arguments:
;
;   OldThread (r0) - Supplies the address of the old thread.
;
;   NewThread (r1) - Supplies the address of the new thread.
;
;   SwapIrql (r2) - Supplies the IRQL of the caller, which is used to
;       determine whether APC bypass is possible.
;
; Return Value:
;
;   If a kernel APC is pending, then a value of TRUE is returned. Otherwise,
;   a value of FALSE is returned.
;
;--

        NESTED_ENTRY KiSwapContext

        GENERATE_EXCEPTION_FRAME                        ; generate exception frame

        PCR_READ r3                                     ; get PKPCR from id register
        add     r3, r3, #PcPrcb                         ; get address of PRCB
        CAPSTART KiSwapContext, SwapContext
        bl      SwapContext                             ; swap context
        CAPEND  KiSwapContext

        RESTORE_EXCEPTION_FRAME                         ; restore exception frame/exit
        EPILOG_RETURN

        NESTED_END KiSwapContext

        SUBT  "Dispatch Interrupt"
;++
;
; Routine Description:
;
;   This routine is entered as the result of a software interrupt generated
;   at DISPATCH_LEVEL. Its function is to process the DPC list, and then
;   perform a context switch if a new thread has been selected for execution
;   on the current processor.
;
;   This routine is entered at DISPATCH_LEVEL with the dispatcher database
;   unlocked.
;
; Arguments:
;
;   PreviousIrql (r0) - Supplies the previous IRQL.
;
; Implicit Arguments:
;
;   None.
;
; Return Value:
;
;   None.
;
;--

        NESTED_ENTRY KiDispatchInterrupt
        PROLOG_PUSH     {r11, lr}

;
; ARM's Load/Store Exclusive feature replaces the need for SLIST sequence
; numbers (or CPUID) and procedure rewind on generic traps and interrupts,
; so there is no KiCheckForSListAddress here.
; Rewinds for #AV while accessing First->Next are still required and are
; performed by KiOpPreprocessAccessViolation.
;

;
; Disable interrupts.
;

        PCR_READ r3
        add     r3, r3, #PcPrcb                         ; get current PRCB address
KiDI10  cpsid   i                                       ; disable interrupts

;
; Save the irql of the waiting thread in order to make correct priority
; decisions for threads doing kernel work (e.g., holding kernel locks).
;

        ldr     r1, [r3, #PbCurrentThread]              ; get thread address
        strb    r0, [r1, #ThWaitIrql]                   ; save wait IRQL.

;
; Check if the DPC queue has any entries to process.
;


#if DBG

        ldr     r1, [r3, #PbDeferredReadyListHead]
        cbz     r1, %F2
        EMIT_BREAKPOINT
2

#endif

        ldrb    r1, [r3, #PbNormalDpcState]
        tst     r1, #KI_DPC_ALL_FLAGS                   ; test DPCs
        beq     KiDI20                                  ; if z, no DPCs to process
        mov     r1, sp                                  ; save current stack pointer
        mov     r0, r3                                  ; set PRCB address parameter
        ldr     sp, [r3, #PbDpcStack]                   ; switch to DPC stack
        b       KxRetireDpcList                         ; process the DPC list

        ALTERNATE_ENTRY KiDispatchInterruptContinue

        PCR_READ r3
        add     r3, r3, #PcPrcb                         ; get current PRCB address

;
; Check to determine if quantum end is requested.
;

KiDI20  cpsie   i                                       ; enable interupts
        ldrb    r0, [r3, #PbQuantumEnd]
        cbz     r0, KiDI40                              ; check if quantum end request; if e, no quantum end request
        movs    r0, #0
        strb    r0, [r3, #PbQuantumEnd]                 ; clear quantum end indicator
        CAPSTART KiDispatchInterrupt, KiQuantumEnd
        bl      KiQuantumEnd                            ; process quantum end
        CAPEND  KiDispatchInterrupt

;
; Restore nonvolatile registers, deallocate stack frame, and return.
;

KiDI30
        EPILOG_POP      {r11, pc}

;
; Check to determine if a new thread has been selected for execution on this
; processor.
;

KiDI40  ldr     r0, [r3, #PbNextThread]
        cmp     r0, #0                                  ; check if new thread selected
        beq     KiDI30                                  ; if eq, then no new thread

;
; Check if the thread preemption should be deferred.
;

        ldr     r0, [r3, #PbCurrentThread]              ; get thread address
        ldrb    r1, [r0, #ThThreadControlFlags]         ; get thread control flags
        tst     r1, #THREAD_FLAGS_GROUP_SCHEDULING
        bne     KiDI60                  ; if nz, group scheduling active

;
; Swap context to a new thread as the result of new thread being scheduled
; by the dispatch interrupt.
;

KiDI50
        EPILOG_POP      {r11, lr}                       ; restore regs, but don't jump
        EPILOG_BRANCH   KxDispatchInterrupt

        NESTED_END KiDispatchInterrupt

;
; Continue swapping to a new thread scheduled by the dispatch interrupt.
;
; (r0) - Current thread address
; (r3) - PRCB address
;

KiDI60  mov     r1, r3                  ; set PRCB address
        bl      KiDeferGroupSchedulingPreemption
        cmp     r0, #0
        beq     KiDI50                  ; if eq, preemption not deferred.
                                        ; Continue swapping thread.
        b       KiDI30                  ; if neq, preemption deferred.

;
; There is a new thread scheduled for execution. Context switch to the
; new thread immediately.
;
; N.B. The following routine is entered by falling through from the above
;      routine.
;
; N.B. The following routine is carefully written as a nested function that
;      appears to have been called directly by the caller of the above
;      function which processes the dispatch interrupt.
;
; Arguments:
;
;   None.
;

        NESTED_ENTRY KxDispatchInterrupt

        GENERATE_EXCEPTION_FRAME                        ; generate exception frame

        PCR_READ r4                                     ; get PKPCR from register
        add     r4, r4, #PcPrcb                         ; get address of PRCB
        ldr     r5, [r4, #PbCurrentThread]              ; get old thread address

;
; Capture total cycles for the current thread before the context switch.
;
; N.B. This must be done with interrupts disabled.
;

        cpsid   i                                       ; disable interrupt
        ldrb    r0, [r4, #PbNestingLevel]               ; begin interrupt time
        adds    r0, r0, #1
        strb    r0, [r4, #PbNestingLevel]
        READ_CYCLE_COUNTER_64BIT r0, r1, r2, r3, r4     ; read "time stamp counter"
        add     r7, r4, #PbStartCycles                  ; compute total cycles for period
        ldrd    r2, r3, [r7]
        strd    r0, r1, [r7]                            ; compute start of next period
        subs    r0, r0, r2
        sbcs    r1, r1, r3
        add     r7, r5, #ThCycleTime                    ; point r7 to the 64-bit cycle time
0       ldrexd  r2, r3, [r7]                            ; accumulate cycles
        adds    r2, r2, r0
        adcs    r3, r3, r1
        strexd  r12, r2, r3, [r7]                       ; update this value first
        cmp     r12, #1                                 ; did it work?
        beq     %B0                                     ; if not, try again
        add     r7, r5, #ThCurrentRunTime               ; point r7 to the runtime
        cbnz    r1, %F2                                 ; check overflow of runtime
        ldr     r2, [r7]                                ; load current runtime
        adds    r2, r2, r0                              ; accumulate runtime
        bcc     %F3                                     ;
2       ldr     r2, =-1                                 ; saturate runtime
3       str     r2, [r7]                                ; save new runtime
        ldrb    r2, [r5, #ThThreadControlFlags]         ; check for profiling
        tst     r2, #THREAD_FLAGS_ACCOUNTING_ANY        ;
        bne     KxDI10                                  ; if nz, accounting is enabled
KxDI20  cpsie   i                                       ; enable interrupts

;
; Notify AutoBoost about the pending context switch and acquire the current PRCB lock 
; (perform lock order checks on checked builds).
;
        mov     r0, r5                                  ; set address of old thread
        movs    r1, #0                                  ; set request interrupt to zero    
        bl      KiAbProcessContextSwitch                ; call AutoBoost context swap routine

#if DBG

        add     r7, r4, #PbKeSpinLockOrdering           ; point r7 to spin lock ordering
0       ldrex   r0, [r7]                                ; load current spin lock order
        tst     r0, #KI_SPINLOCK_ORDER_PRCB_LOCK        ; check if lock held
        beq     %F1                                     ; if eq, lock is not held
        EMIT_BREAKPOINT                                 ; otherwise, lock is held

1       orr     r0, #KI_SPINLOCK_ORDER_PRCB_LOCK        ; set PRCB lock held
        strex   r12, r0, [r7]                           ; store lock ordering
        cmp     r12, #1                                 ; did it work?
        beq     %B0                                     ; if not, try again

#endif

;
;       _ARM_PERF_ Update register allocation to reflect lack of call
;

        ACQUIRE_SPIN_LOCK r4, #PbPrcbLock               ; acquire PRCB lock (trashes r0-r2)

;
; Get the next thread address, set the thread state to running, queue the old
; running thread, and swap context to the next thread.
;

        ldr     r6, [r4, #PbNextThread]                 ; get next thread address
        movs    r0, #0
        str     r0, [r4, #PbNextThread]                 ; clear next thread address

#if DBG

        mov         r0, r6                              ; load arguments
        mov         r1, r4                              ;
        bl          KiQueryEffectivePriorityThread      ; check for rank bias
        ldr         r2, [r4, #PbPriorityState]          ; load priority state
        ldrb        r1, [r2]                            ; load previous priority
        cmp         r0, r1                              ; compare priorities
        beq         %F0                                 ; priority matches
        EMIT_BREAKPOINT                                 ; corrupted state

#endif

0       str     r6, [r4, #PbCurrentThread]              ; set current thread address
        movs    r0, #Running
        strb    r0, [r6, #ThState]                      ; set new thread state
        movs    r0, #WrDispatchInt
        strb    r0, [r5, #ThWaitReason]                 ; set wait reason
        mov     r0, r4                                  ; set address of current PRCB
        mov     r1, r5                                  ; set address of old thread
        CAPSTART KxDispatchInterrupt, KiQueueReadyThread
        bl      KiQueueReadyThread                      ; queue ready thread for exec.
        CAPEND  KxDispatchInterrupt
        mov     r0, r5                                  ; set address of old thread
        mov     r1, r6                                  ; set address of new thread
        movs    r2, #APC_LEVEL                          ; set APC bypass disable
        mov     r3, r4                                  ; set address of current PRCB
        CAPSTART KxDispatchInterrupt, KiSwapContext
        bl      SwapContext                             ; call context swap routine
        CAPEND  KxDispatchInterrupt

        RESTORE_EXCEPTION_FRAME                         ; restore exception state
        EPILOG_RETURN                                   ; return

;
; Less common paths for dispatch interrupts with thread accounting active.
;
; (r5) - Current Thread
; (r4) - Current PRCB
; (r0) - Low cycle count
; (r1) - High cycle count
;

KxDI10  mov     r2, r0                                  ; load 64-bit cycle count
        mov     r3, r1                                  ;
        mov     r1, r5                                  ; set current thread address
        mov     r0, r4                                  ; set PRCB address
        bl      KiEndThreadAccountingPeriod             ; call accounting routine
        b       KxDI20                                  ; join common code

        NESTED_END KxDispatchInterrupt

;++
;
; This is a dummy function to effect a call out on the DPC stack with
; the proper unwind information.
;
; N.B. The prologue is not executed and the stack frame is set up before
;      transfering control to the alternate entry.
;
; At entry to this dummy function:
;
;    r0 - Supplies the address of the current PRCB.
;
;    r1 - Previous stack pointer.
;
;--

        NESTED_ENTRY KxRetireDpcListFake,, KiFatalExceptionHandler

        PROLOG_PUSH_MACHINE_FRAME                       ; these pushes must be equivalent
        PROLOG_STACK_ALLOC 8                            ; to pushing a KDPC_STACK_FRAME

        ALTERNATE_ENTRY KxRetireDpcList //, _TEXT$00    ; real entry point
        str     r1, [sp, #DpSp]                         ; store previous stack poiner

        CAPSTART KxRetireDpcList, KiRetireDpcList
        bl    KiRetireDpcList                           ; process the DPC list
        CAPEND  KxRetireDpcList

        ldr     sp, [sp, #DpSp]                         ; restore previous sp
        b       KiDispatchInterruptContinue             ; finish in common code

        NESTED_END KxRetireDpcListFake //, _TEXT$00

        SUBT  "Swap Context"
;++
;
; Routine Description:
;
;   This routine is called to swap context from one thread to the next. It
;   swaps context, flushes the translation buffer, swaps the process address
;   space if necessary, and returns to its caller.  The stack frames may be
;   generated by this code or via the new thread initialization code.
;
;   N.B. This routine is only called by code within this module and the idle
;        thread code and uses special register calling conventions.
;
; Arguments:
;
;   r0 - Supplies the address of previous thread.
;
;   r1 - Supplies the address of next thread.
;
;   r2 - Supplies the APC interrupt bypass disable IRQL value.
;
;   r3 - Supplies the address of the current PRCB.
;
; Return value:
;
;   r0 - boolean? indicating whether the request was pended.
;
;   r4 - Supplies the address of the current PRCB.
;

        NESTED_ENTRY SwapContext

        PROLOG_PUSH         {r11, lr}                   ; save switch frame context
        PROLOG_STACK_ALLOC  SwR11                       ; allocate remaining switch frame

        strb    r2, [sp, #SwApcBypass]                  ; save APC bypass disable
        mov     r4, r0                                  ; save arg because of callout
        mov     r5, r1                                  ; save arg because of callout
        mov     r6, r3                                  ; save arg because of callout

;
; Save TPIDRURW for the previous thread.
;

        TEB_READ r0
        str     r0, [r4, #ThUserRwBase]

;
; N.B. It is necessary to wait for the new thread context to be swapped so
;      that any context switch away from the new thread on another processor
;      is completed before attempting to swap context context to the thread.
;

        ldrb    r0, [r5, #ThRunning]                    ; check if new thread running
        cmp     r0, #0
        bne.w   KiSC55                                  ; if ne, the thread is running

;
; N.B. A memory barrier is required after reading ThRunning of the new thread
;      in order to ensure that no accesses to the new thread occur before
;      the new thread is observed to swapped off on another processor. This
;      barrier is achieved by creating an address dependency between the value
;      of the previous thread running flag and reads from the new thread.
;

KiSC00  add     r5, r5, r0                              ; create address dependency
        movs    r0, #1
        strb    r0, [r5, #ThRunning]                    ; set new thread running

;
; Update the current thread.
;

#if DBG

        tst     r5, #CP15_THREAD_RESERVED_MASK          ; check reserved bits
        beq     %F1                                     ; if eq, bits are clear

        EMIT_BREAKPOINT
1
#endif

        cpsid   i                                       ; disable interrupts
        SET_THREAD_AND_IRQL r0, r5, #DISPATCH_LEVEL     ; write the thread and irql register

#if (ARM_VFP_MANAGEMENT == ARM_VFP_ALWAYSON)

        ldr     r1, [r4, #ThVfpState]                   ; get pointer to our context VFP state
        ldr     r2, [r5, #ThVfpState]                   ; get new context pointer
        SAVE_VFP_STATE r1, r0                           ; save old state
        RESTORE_VFP_STATE r2, r0                        ; restore new state

#else

;
; Save old VFP state, if necessary
;

        ldrb    r1, [r4, #ThNpxState]                   ; get outgoing thread's VFP state
        ldrb    r0, [r5, #ThNpxState]                   ; get incoming thread's VFP state
#if (ARM_VFP_MANAGEMENT == ARM_VFP_LAZY_WITH_DEMOTION)
#if ARM_VFP_ENABLE_STATISTICS
        cmp     r1, #-1                                 ; did we miss a demotion?
        bne     %F0                                     ; if not, skip
        INCREMENT_VFP_STAT VtMissedDemotions, r2, r3, r7, r8 ; track how many times we never demoted
0
#endif
#endif
        cbz     r1, %F1                                 ; if outgoing VFP is off, don't save anything
        ldr     r2, [r4, #ThVfpState]                   ; get pointer to outgoing VFP context buffer
        SAVE_VFP_STATE r2, r3                           ; store the VFP state there
1
        cbz     r0, %F3                                 ; if incoming VFP is off, make sure CPACR reflects that

;
; Incoming thread has VFP enabled; enable VFP and swap in new state
;

        INCREMENT_VFP_STAT VtContextSwitchesToEnabled, r2, r3, r7, r8 ; track how many switches to enabled state
#if (ARM_VFP_MANAGEMENT == ARM_VFP_LAZY_WITH_DEMOTION)
        movs    r0, #2                                  ; use 2 to indicate a thread is demotion-worthy
        strb    r0, [r5, #ThNpxState]                   ; store in the NPX state
        INCREMENT_VFP_STAT VtAttemptedDemotions, r2, r3, r7, r8 ; track how many times we tried to demote
#endif
        cbnz    r1, %F2                                 ; if outgoing was already on, we don't need to touch CPACR
        CP_READ r1, CP15_CPACR                          ; fetch CPACR
        orr     r1, r1, #CP15_CPACR_VFP_MASK            ; enable VFP
        CP_WRITE r1, CP15_CPACR                         ; update CPACR
        isb                                             ; make it visible
2
        ldr     r2, [r5, #ThVfpState]                   ; get incoming VFP state pointer
        RESTORE_VFP_STATE r2, r0                        ; load it into VFP registers
        b       %F4                                     ; skip ahead

;
; Incoming thread has VFP disabled; disable VFP
;

3
        INCREMENT_VFP_STAT VtContextSwitchesToDisabled, r2, r3, r7, r8 ; track how many switches to disabled state
        cbz     r1, %F4                                 ; if outgoing was already off, we don't need to touch CPACR
        CP_READ r1, CP15_CPACR                          ; fetch CPACR
        bic     r1, r1, #CP15_CPACR_VFP_MASK            ; disable VFP
        CP_WRITE r1, CP15_CPACR                         ; update CPACR
        isb                                             ; make it visible
4

#endif

;
; The new thread cannot be the current thread on any other processor at this
; point - resume thread cycle accumulation.
;

        READ_CYCLE_COUNTER_64BIT r0, r1, r2, r3, r6     ; read "time stamp" counter
        add     r12, r6, #PbStartCycles                 ; compute total cycles for period
        ldrd    r2, r3, [r12]
        strd    r0, r1, [r12]                           ; compute start of next period
        subs    r0, r0, r2
        sbcs    r1, r1, r3
        ldrd    r2, r3, [r12, #(PbCycleTime - PbStartCycles)] ; accumulate cycles
        adds    r2, r2, r0
        adcs    r3, r3, r1
        strd    r2, r3, [r12, #(PbCycleTime - PbStartCycles)]

;        ldrb    r0, [r6, #PbInterruptRequest]           ; check interrupt request
;        cmp     r0, #0
;        beq     KiSC01                                  ; if z, no interrupt request
;        ldr     r0, =0                                  ; clear interrupt request
;        strb    r0, [r6, #PbInterruptRequest]
;        ldr     r0, [r6, #PbIdleThread]                 ; check the new thread
;        cmp     r0, r5
;        beq     KiSC01                                  ; if z, new thread is the idle thread
;        mov     r0, r6                                  ; get address of PRCB
;        ldr     r1, =DISPATCH_LEVEL                     ; request dispatch interrupt
;        bl      KiRequestSoftwareInterrupt

;
; Check whether thread accounting is active on the new thread.
;

KiSC01  ldrb    r2, [r5, #ThThreadControlFlags]
        tst     r2, #THREAD_FLAGS_ACCOUNTING_CSWITCH    ; check for thread accounting
        bne.w   KiSC43                                  ; if nz, accounting active

        ldrb    r0, [r6, #PbNestingLevel]               ; decrement nesting level
        subs    r0, r0, #1
        strb    r0, [r6, #PbNestingLevel]

#if DBG

        beq     %F1                                     ; if z, nesting level was one
        EMIT_BREAKPOINT                                 ; break if non-zero
1

#endif

        cpsie   i                                       ; enable interrupts

;
; Increment the number of context switches on this processor.
;
; N.B. This increment is done here is force the cache block containing the
;      context switch count into the cache as write exclusive. There are
;      several other references to this cache block in the following code.
;

KiSC05  ldr     r0, [r6, #PbContextSwitches]
        adds    r0, r0, #1
        str     r0, [r6, #PbContextSwitches]            ; processor count

;
; Switch kernel stacks.
;

KiSC10

#if DBG

        ldr     r0, [r4, #ThStackLimit]                 ; get lower bound
        cmp     sp, r0                                  ; check old sp against lower bound
        blo     %F1                                     ; if lo, break
        ldr     r0, [r4, #ThStackBase]                  ; get upper bound
        cmp     sp, r0                                  ; check old sp against upper bound
        blo     %F2                                     ; if lo, stack is in bounds

1
        EMIT_BREAKPOINT

2

#endif

        str     sp, [r4, #ThKernelStack]                ; save old kernel stack pointer
        ldr     sp, [r5, #ThKernelStack]                ; get new kernel stack pointer

#if DBG

        ldr     r0, [r5, #ThStackLimit]                 ; get lower bound
        cmp     sp, r0                                  ; check new sp against lower bound
        blo     %F1                                     ; if lo, break
        ldr     r0, [r5, #ThStackBase]                  ; get upper bound
        cmp     sp, r0                                  ; check new sp against upper bound
        blo     %F2                                     ; if lo, stack is in bounds

1
        EMIT_BREAKPOINT

2

#endif

        ldr     r11, [sp, #SwR11]                       ; fetch the new r11 chain as well


;
; Swap the process address space if the new process is not the same as the
; previous process.
;

KiSC14  ldr     r1, [r4, #ThApcState + AsProcess]       ; get old process address
        ldr     r0, [r5, #ThApcState + AsProcess]       ; get new process address
        cmp     r0, r1                                  ; check if process match
        beq     KiSC20                                  ; if e, process addresses match

;
; Set the processor bit in the new process.
;
; N.B. This code assumes there is only 1 group (MAXIMUM_GROUPS == 1 in affinity.h).
;

        ldrb    r2, [r6, #PbGroupIndex]                 ; get group relative index
        mov     r7, #1                                  ; r7 = 1
        lsls    r7, r7, r2                              ; get group relative mask
0
        ldrex   r2, [r0, #PrActiveProcessors + AfBitmap]; read current value from new 
        orr     r2, r2, r7                              ; set the bit
        strex   r12, r2, [r0, #PrActiveProcessors + AfBitmap] ; attempt to save
        cmp     r12, #1                                 ; did it work?
        beq     %B0                                     ; if not, try again

;
; Switch address spaces.
;

        bl      KxSwapProcess

;
; Clear the processor bit in the old process after the address space switch.
; This is important to ensure that TB flushes targetting the old process are
; issued to this processor until the address switch has occurred. This also
; enables other parts of the code  (e.g. process outswap) to know when all
; threads of a given process have fully swapped off of all processors.
;
; N.B. This code assumes there is only 1 group (MAXIMUM_GROUPS == 1 in affinity.h).
;

        ldrb    r2, [r6, #PbGroupIndex]                 ; get group relative index
        mov     r7, #1                                  ; r7 = 1
        lsls    r7, r7, r2                              ; get group relative mask
0
        ldr     r1, [r4, #ThApcState + AsProcess]       ; get old process address
        ldrex   r2, [r1, #PrActiveProcessors + AfBitmap]; read current value from old 
        bic     r2, r2, r7                              ; clear the bit
        strex   r12, r2, [r1, #PrActiveProcessors + AfBitmap] ; attempt to save
        cmp     r12, #1                                 ; did it work?
        beq     %B0                                     ; if not, try again

;
; Set the new kernel stack base.
;
; N.B. It is assumed elsewhere that this occurs after the directory base
;      is loaded.
;

KiSC20  ldr     r0, [r5, #ThInitialStack]               ; get new stack base address
        str     r0, [r6, #PbSpBase]                     ; set stack base in PRCB

;
; Check for context swap logging.
;

        ldr     r0, =PerfGlobalGroupMask                ; get global mask address
        ldr     r0, [r0, #PERF_CONTEXTSWAP_OFFSET]
        tst     r0, #PERF_CONTEXTSWAP_FLAG              ; check flag
        bne     KiSC45                                  ; log context swap

;
; N.B. A memory barrier is required before writing ThRunning of the old thread
;      to ensure that all preceding stores of the old thread saved state are
;      complete before the thread can be swapped in on another processor.
;

KiSC21  movs    r0, #0
        dmb     ish                                     ; execute a memory barrier
        strb    r0, [r4, #ThRunning]                    ; set old thread not running

;
; Setup TEB.
;
;

        ldr      r0, [r5, #ThTeb]                       ; get new TEB address
        ldr      r1, =(PcTeb - PcPrcb)
        str      r0, [r6, r1]                           ; set TEB in PCR

;
; Use the saved TPIDRURW value if the TEB is NULL.
;

        cmp      r0, #0
        bne      %F1
        ldr      r0, [r5, #ThUserRwBase]

1
        TEB_WRITE r0                                    ; set address of new TEB

;
; Check if an attempt is being made to context switch while in a DPC routine.
;

KiSC30  ldr     r0, [r6, #PbDpcRequestSummary]
        tst     r0, #KI_DPC_ANY_DPC_ACTIVE              ; check if any DPC active
        bne     KiSC50                                  ; if nz, DPC is active

;
; Update context switch counter.
;

        ldr     r0, [r5, #ThContextSwitches]
        adds    r0, r0, #1
        str     r0, [r5, #ThContextSwitches]            ; thread count

;
; If the new thread has a kernel mode APC pending, then request an APC
; interrupt if APC bypass is disabled.
;

        ldrb    r1, [r5, #ThApcState + AsKernelApcPending]
        cmp     r1, #TRUE                               ; check if APC pending
        bne     KiSC40                                  ; if ne, kernel APC not pending
        ldrb    r0, [sp, #SwApcBypass]                  ; get disable IRQL level
        ldrh    r1, [r5, #ThSpecialApcDisable]
        orrs    r1, r0, r1                              ; merge special APC disable
        mov     r1, #TRUE
        beq     KiSC40                                  ; no apc
        mov     r0, r6                                  ; get address of PRCB
        movs    r1, #APC_LEVEL                          ; request APC interrupt
        CAPSTART SwapContext, KiRequestSoftwareInterrupt
        bl      KiRequestSoftwareInterrupt
        CAPEND  SwapContext
        movs    r1, #FALSE

KiSC40
        mov     r0, r1                                  ; return APC pending

        EPILOG_STACK_FREE   SwR11                       ; free remaining switch frame
        EPILOG_POP          {r11, lr}                   ; restore r11, lr
        EPILOG_RETURN

;
; Begin a new thread accumulation period.
;
; (r5) - Current Thread
; (r6) - Current PRCB
; (r0) - Low cycle count
; (r1) - High cycle count
;

KiSC43  mov     r2, r0
        mov     r3, r1
        mov     r0, r6                                  ; set current PRCB
        mov     r1, r5                                  ; set current thread address
        bl      KiBeginThreadAccountingPeriod           ; call thread accounting routine
        b       KiSC05                                  ; finish in common code

;
; Log context swap events.
;

KiSC45  mov     r0, r4                                  ; set address of old thread
        mov     r1, r5                                  ; set address of new thread
        CAPSTART SwapContext, EtwTraceContextSwap
        bl      EtwTraceContextSwap                     ; call trace routine
        CAPEND  SwapContext
        b       KiSC21                                  ; finish in common code

;
; An attempt is being made to context switch while in a DPC routine. This is
; most likely caused by a DPC routine calling one of the wait functions.
;

KiSC50  movs    r3, #0                                  ; clear registers
        str     r3, [sp, #0]                            ; set parameter 5
        mov     r2, r5                                  ; set new thread address
        mov     r1, r4                                  ; set old thread address
        movs    r0, #ATTEMPTED_SWITCH_FROM_DPC          ; set bug check code
        bl      KeBugCheckEx
        bx      lr                                      ; return

;
; Wait for the new thread context to be swapped.
;

KiSC55

KiSC60  yield                                           ; yield processor execution
        ldrb    r0, [r5, #ThRunning]
        cmp     r0, #0                                  ; if new thread running
        beq     KiSC00                                  ; if eq, swap busy clear
        b       KiSC60
        NESTED_END SwapContext

;
; An attempt is being made to context switch while in a DPC routine. This is
; most likely caused by a DPC routine calling one of the wait functions.
;

        LEAF_ENTRY SwapContextDpcBugcheck
        movs    r3, #0
        mov     r2, r5
        mov     r1, r4
        movs    r0, #ATTEMPTED_SWITCH_FROM_DPC          ; set bug check code
        bl      KeBugCheckEx                            ; bugcheck system, no return
        bx      lr
        LEAF_END SwapContextDpcBugcheck

        END

