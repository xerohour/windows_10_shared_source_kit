        TTL  "Call Out to User Mode"
;++
;
; Copyright (c) 2000  Microsoft Corporation
;
; Module Name:
;
;   callout.asm
;
; Abstract:
;
;   This module implements the code necessary to call out from kernel
;   mode to user mode.
;
; Environment:
;
;    Kernel mode only.
;
;--

#include "ksarm.h"

        IMPORT  KeBugCheck
        IMPORT  KiFatalExceptionHandler
        IMPORT  KiCopyDebugRegs
        IMPORT  KiSystemServiceExit
        IMPORT  MmGrowKernelStackEx
        IMPORT  PsInvokeWin32Callout
        IMPORT  RtlUnwindEx

        IMPORT  KeKernelStackSize
        IMPORT  KeUserCallbackDispatcher

        SUBT  "Call User Mode Function"

;++
;
; NTSTATUS
; KiCallUserMode (
;     __deref_out_bcount_opt(*OutputLength) PVOID *OutputBuffer,
;     __out PULONG OutputLength,
;     __inout PVOID NewInitialStack,
;     __in PVOID NewStackBase
;     )
;
; Routine Description:
;
;   This function calls a user mode function from kernel mode.
;
;   N.B. This function calls out to user mode and the callback return
;        function returns back to the caller of this function. Therefore,
;        the stack layout must be consistent between the two routines.
;
; Arguments:
;
;   OutputBuffer (r0) - Supplies a pointer to the variable that receives
;       the address of the output buffer.
;
;   OutputLength (r1) - Supplies a pointer to a variable that receives
;       the length of the output buffer.
;
;   NewInitialStack (r2) - Supplies a pointer to the new initial kernel
;       stack address that will be used for nested system calls from
;       the usermode callout.
;
;   NewStackBase (r3) - Supplies a pointer to the new kernel stack base.
;
; Return Value:
;
;   The final status of the call out function is returned as the status
;   of the function.
;
;   N.B. This function does not return to its caller. A return to the
;        caller is executed when a callback return system service is
;        executed.
;
;--

        NESTED_ENTRY KiCallUserMode
        
#if (ARM_VFP_MANAGEMENT == ARM_VFP_ALWAYSON)
        PROLOG_VPUSH {d8-d15}                           ; save non-volatile FP
        GENERATE_EXCEPTION_FRAME                        ; generate exception frame
        PROLOG_STACK_SAVE r7                            ; use r7 as our frame pointer
#else
        PROLOG_STACK_ALLOC 8*8 + 4 + 4                  ; allocate space for VFP regs + FPSCR + align
        GENERATE_EXCEPTION_FRAME                        ; generate exception frame
        PROLOG_STACK_SAVE r7                            ; use r7 as our frame pointer
        CP_READ r10, CP15_CPACR                         ; read CPACR
        mov     r4, #-1                                 ; use -1 to indicate VFP was previously off
        tst     r10, #CP15_CPACR_VFP_MASK               ; is the VFP enabled?
        beq     %F0                                     ; skip if not
        add     r5, sp, #KEXCEPTION_FRAME_LENGTH        ; point past exception frame
        vstm    r5, {d8-d15}                            ; save the non-volatile registers if it is
        vmrs    r4, fpscr                               ; fetch FPSCR
0       str     r4, [sp, #KEXCEPTION_FRAME_LENGTH + 8*8]; store FPSCR or -1 if none
#endif

;
; Save argument registers in the frame and obtain a pointer to
; the current thread.
;

        str     r0, [r7, #CuOutputBuffer]               ; save the output buffer address
        str     r1, [r7, #CuOutputLength]               ; save the output length address
        str     sp, [r2, #KcPreviousKernel]             ; save the return stack
        CURTHREAD_READ r4                               ; get current thread address

;
; Save the current trap frame address and establish a new initial kernel stack
; address.
;
; N.B. Note the callback stack pointer is set once the entire callback frame
;      has been established to ensure correct stack walks.
;

        ldr     r6, [r4, #ThTrapFrame]                  ; get current trap frame
        str     r6, [r7, #CuTrapFrame]                  ; save current trap frame
        cpsid   i                                       ; disable interrupts
        str     r2, [r4, #ThInitialStack]               ; set new initial stack address
        str     r3, [r4, #ThStackBase]                  ; set new stack base
        PCR_READ r5                                     ; get pointer to PCR
        ldr.w   r0, =KeKernelStackSize
        ldr     r0, [r0]
        sub     r0, r3, r0                              ; calculate new stack limit
        str     r2, [r5, #PcSpBase]                     ; set initial stack address in PRCB
        str     r0, [r4, #ThStackLimit]                 ; set new stack limit

;
; _ARM_PERF_ AMD64 has a performance optimization here if a user
; APC is not pending where it exits directly back into user mode
; without building a trap frame

;
; A user APC is pending.
;
; Construct a trap frame to facilitate the transfer into user mode via
; the standard system call exit.
;
; (r2) - New initial stack address.
;
; (r6) - Previous trap frame pointer.
;

KiUC20  sub     r1, r2, #KTRAP_FRAME_LENGTH + VFP_STATE_LENGTH ; allocate a trap frame with full VFP state
        mov     sp, r1                                  ; set new stack pointer

        movs    r0, #KTRAP_FRAME_LENGTH                 ; set length of copy
1       subs    r0, r0, #4                              ; copy trap frame
        ldr     r3, [r6, r0]                            ; load data from old trap frame
        str     r3, [r1, r0]                            ; store data in new trap frame
        bne     %B1                                     ;
        
#if (ARM_VFP_MANAGEMENT != ARM_VFP_ALWAYSON)
        movs    r3, #0                                  ; optimization: NULL the VFP state pointer
        str     r3, [sp, #TrVfpState]                   ; so we don't bother restoring VFP state
#endif

        ldr.w   r0, =KeUserCallbackDispatcher           ; default to user callback dispatcher
        ldr     r0, [r0]                                ; get the address of callback
        ldr.w   r2, =(CPSRM_MASK | CPSR_IT_MASK)        ; get a mask with both the mode and IT flags
        str     r0, [sp, #TrPc]                         ; set user return address
        ldr     r1, [sp, #TrCpsr]                       ; get CPSR
        bics    r1, r1, r2                              ; clear processor mode and IT flags
        orr     r1, r1, #CPSRM_USER                     ;
        str     r1, [sp, #TrCpsr]                       ; set mode to user
        b       KiSystemServiceExit                     ; finish in common code

        NESTED_END KiCallUserMode

        SUBT  "Switch Kernel Stack and Callout"
;++
;
; NTSTATUS
; KiSwitchKernelStackAndCallout (
;     __in PVOID Parameter,
;     __in PEXPAND_STACK_CALLOUT Callout,
;     __in ULONG_PTR InitialStack,
;     __in SIZE_T CommitSize
;     )
;
; Routine Description:
;
;   This function sets up the kernel stack control region of the new
;   stack, sets the new stack parameters for the current thread, calls
;   the specified function with the specified argument, and reverses the
;   process on return from the called function.
;
; Arguments:
;
;   Parameter (r0) - Supplies a the call out routine parameter.
;
;   Callout (r1) - Supplies a pointer to the call out function.
;
;   InitialStack (r2) - Supplies the inital stack address of the new segment.
;
;       N.B. This parameter points to the KSTACK_CONTROL structure of the
;            new segment. If the low bit of the pointer is set, the entirety
;            of the new segment is already committed.
;
;   CommitSize (r3) - Supplies the stack commit size in bytes page aligned.
;
;       N.B. A commit size of zero signifies a small kernel stack with no
;            commit checks required.
;
; Return Value:
;
;   A success value is returned if the specified function is successfully
;   called. Otherwise, a failure status is returned.
;
;--

        NESTED_ENTRY KiSwitchKernelStackAndCallout,,KiFatalExceptionHandler
        PROLOG_PUSH     {r4-r9, r11, lr}

        ands    r9, r2, #3                              ; save committed stack flag
        bic     r2, r2, #3                              ; clear low bits
        CURTHREAD_READ r4                               ; get current thread address
        add     r5, r2, #KSTACK_CONTROL_LENGTH          ; compute stack base address

#if DBG

        ldr     r6, [r2, #KcCurrentBase]                ; get stack base
        cmp     r5, r6                                  ; compare stack base
        beq     %F1                                     ; if eq, do not break
        EMIT_BREAKPOINT
1       nop

#endif

        mov     r8, sp                                  ; save current stack address
        str     r8, [r2, #KcPreviousKernel]             ; save current kernel stack

;
; Initialize a dummy UCALLOUT frame on the new stack.
;
; N.B. Only the return address and previous stack pointer are filled into
;      the dummy UCALLOUT frame.
;

        sub     r7, r2, #CalloutFrameLength             ; get new frame address
        ldr     r6, =KiSwitchKernelStackContinue
        str     r6, [r7, #CkPc]                         ; set PC in machine frame
        str     sp, [r7, #CkSp]                         ; set SP in machine frame

;
; Initialize new thread stack parameters and jump dispatch routine.
;
; N.B. The PRCB initial stack value must be updated to ensure proper detection
;      of the stack type.
;
; N.B. The old thread stack limit must be captured and updated to the new
;      value with interrupts disabled to avoid storing a stale value. 
;      This situation may occur if this thread is running on a partially
;      committed stack when an APC routine interrupts this function and
;      grows the stack within the current segment.
;

        cpsid   i                                       ; disable interrupts
        ldr     r6, [r4, #ThStackLimit]                 ; get current stack limit
        str     r6, [r2, #KcPreviousLimit]              ; save current stack limit
        str     r2, [r4, #ThInitialStack]               ; set thread initial stack
        PCR_READ r6                                     ; get pointer to PCR
        str     r2, [r6, #PcSpBase]                     ; set initial stack in PRCB
        str     r5, [r4, #ThStackBase]                  ; set new stack base

        cmp     r9, #0
        ldr.w   r12, =KeKernelStackSize
        ldr     r12, [r12]                              ; 0 = small stack
        beq     %F0

        cmp     r9, #1
        mov     r12, #KERNEL_LARGE_STACK_COMMIT         ; 1 = partial large stack
        beq     %F0

        cmp     r9, #3
        mov     r12, #KERNEL_LARGE_STACK_SIZE           ; 3 = full large stack
        beq     %F0
0
        sub     r2, r5, r12
        str     r2, [r4, #ThStackLimit]                 ; set new stack limit

        mov     sp, r7                                  ; set new stack pointer
                                                        ; enable interrupts is done at the target

        b       KxSwitchKernelStackCallout              ; call specified routine

;
; Continue from callout on original stack with and return the completion status
;

KiSwitchKernelStackContinue

        cpsie   i                                       ; enable interrupts (done here to ensure correct unwinding)
        EPILOG_POP      {r4-r9, r11, pc}
        NESTED_END KiSwitchKernelStackAndCallout //, _TEXT$00

;++
;
; This is a dummy function to effect a call out on an expanded stack with
; the proper unwind information.
;
; N.B. The prologue is not executed and the stack frame is set up before
;      transfering control to the alternate entry.
;
; At entry to this dummy function:
;
;    r0 - Supplies the call out routine parameter.
;    r1 - Supplies the address of the call out routine.
;    r3 - Supplies the required commit limit.
;    r4 - Supplies the current thread address.
;    r8 - Supplies the old kernel stack address.
;    sp - Supplies the new stack address.
;
;--

        NESTED_ENTRY KxSwitchKernelStackCalloutFake //, _TEXT$00
        PROLOG_PUSH_MACHINE_FRAME                       ; simulate creation of a UCALLOUT_FRAME
        PROLOG_STACK_ALLOC  16                          ; which is a MACHINE_FRAME and 4 DWORDs

        ALTERNATE_ENTRY KxSwitchKernelStackCallout

        cpsie   i                                       ; enable interrupts (done here to ensure correct unwinding)

        str     r1, [sp, #CkLength]                     ; save call out routine address
        cbz     r3, %F1                                 ; test if commit check required
        str     r0, [sp, #CkBuffer]                     ; save call out parameter
        mov     r0, sp                                  ; set current kernel stack
        mov     r1, r3                                  ; set require commit size
        bl      MmGrowKernelStackEx                     ; grow stack to required commit
        cbnz    r0, KxSKSCRestore                       ; test if stack committed; if nz, stack not committed
        ldr     r0, [sp, #CkBuffer]                     ; set call out parameter
1       ldr     lr, [sp, #CkLength]                     ; get pointer to specified routine
        blx     lr                                      ; call it
        movs    r0, #0                                  ; set success completion status

;
; Restore the thread stack parameters and finish in previous code.
;
; N.B. The PRCB initial stack only needs to be
;      reloaded after the call out and just before switching back to the
;      previous stack. This is in contrast to the user mode call code
;      which reloads these values in both directions. The reason this
;      is possible is because the thread can not return to user mode
;      until after the call out is complete and any exceptions or interrupts
;      will nest on the current kernel stack.
;

KxSKSCRestore
        ldr     r1, [r4, #ThInitialStack]               ; get address of initial frame
        cpsid   i                                       ; disable interrupts
        ldr     r6, [r1, #KcPreviousBase]               ; set stack base
        str     r6, [r4, #ThStackBase]
        ldr     r6, [r1, #KcPreviousLimit]              ; set stack limit
        str     r6, [r4, #ThStackLimit]
        ldr     r6, [r1, #KcPreviousInitial]            ; set initial stack
        str     r6, [r4, #ThInitialStack]
        PCR_READ r2                                     ; get pointer to PCR
        str     r6, [r2, #PcSpBase]                     ; set initial stack in PRCB
        mov     sp, r8                                  ; set previous stack address
                                                        ; enable interrupts is done at the start

        b       KiSwitchKernelStackContinue             ; finish in common code

        NESTED_END KxSwitchKernelStackCalloutFake //, _TEXT$00

        SUBT  "Callback return exception handler"

;++
;
; EXCEPTION_DISPOSITION
; KiCallbackReturnHandler (
;    IN PEXCEPTION_RECORD ExceptionRecord,
;    IN PVOID EstablisherFrame,
;    IN OUT PCONTEXT ContextRecord,
;    IN OUT PDISPATCHER_CONTEXT DispatcherContext
;    )
;
; Routine Description:
;
;   This routine is the exception handler for the NtCallbackReturn system
;   service. Its function is to determine if the exception occurred from
;   the access to the user-mode TEB address containing the GDI batch count.
;
; Arguments:
;
;   ExceptionRecord (r0) - Supplies a pointer to an exception record.
;
;   EstablisherFrame (r1) - Supplies the frame pointer of the establisher
;       of this exception handler.
;
;   ContextRecord (r2) - Supplies a pointer to a context record.
;
;   DispatcherContext (r3) - Supplies a pointer to  the dispatcher context
;       record.
;
; Return Value:
;
;   If the exception is caught and handled, there is no return from this
;   routine.  Otherwise, a value of ExceptionContinueSearch is returned.
;
;--

        NESTED_ENTRY KiCallbackReturnHandler

        PROLOG_PUSH         {r11, lr}
        PROLOG_STACK_ALLOC  8                           ; stack space for RtlUnwindEx 

        ldr     r3, [r0, #ErExceptionFlags]             ; get exception flags
        tst     r3, #EXCEPTION_UNWIND                   ; test for unwind
        bne     KiCh20                                  ; if nz, unwind in progress        
        ldr     r3, =NtCallbackReturnTebAccess          ; get GDI TEB access address
        ldr     r12, [r0, #ErExceptionAddress]          ; get exception address
        cmp     r3, r12                                 ; check if address matches
        bne     KiCh10                                  ; if ne, address does not match

        CURTHREAD_READ r3                               ; get current thread address
        ldrb    r3, [r3, #ThPreviousMode]               ; get previous mode
        cmp     r3, #KernelMode                         ; check if kernel mode
        beq     KiCh10                                  ; if eq, previous mode kernel
        movs    r3, #0                                  ; clear address of history table
        str     r3, [sp, #4]                            ;
        str     r2, [sp, #0]                            ; set address of context
        ldr     r3, [r0, #ErExceptionCode]              ; set return value
        mov     r2, r0                                  ; set address of exception record
        mov     r0, r1                                  ; set target frame address
        ldr     r1, =NtCallbackReturnTebResume          ; set target IP address
        CAPSTART KiCallbackReturnHandler, RtlUnwindEx
        bl      RtlUnwindEx                             ; unwind - no return
        CAPEND  KiCallbackReturnHandler

KiCh10
        movs    r0, #ExceptionContinueSearch
        EPILOG_STACK_FREE   8                           ; deallocate stack frame
        EPILOG_POP          {r11, pc}                   ; return

        NESTED_END KiCallbackReturnHandler

;
; An unwind is in progress.
;
; If a target unwind is in progress, then continue the unwind operation.
; Otherwise bug check the machine.
;

KiCh20
        tst     r3, #EXCEPTION_TARGET_UNWIND            ; test for target unwind
        bne     KiCh10                                  ; if ne, target unwind

        movs    r0, #KMODE_EXCEPTION_NOT_HANDLED        ; set bugcheck code
        bl      KeBugCheck                              ; bugcheck the system

        SUBT  "Return from User Mode Callback"
;++
;
; NTSTATUS
; NtCallbackReturn (
;     __in_opt PVOID OutputBuffer,
;     __in ULONG OutputLength,
;     __in NTSTATUS Status
;     )
;
; Routine Description:
;
;   This function returns from a user mode callout to the kernel
;   mode caller of the user mode callback function.
;
;   N.B. This function returns to the function that called out to user mode
;        using the call user mode function. Therefore, the stack layout must
;        be consistent between the two routines.
;
; Arguments:
;
;   OutputBuffer (r0) - Supplies an optional pointer to an output buffer.
;
;   OutputLength (r1) - Supplies the length of the output buffer.
;
;   Status (r2) - Supplies the status value returned to the caller of the
;       callback function.
;
; Implicit Arguments:
;
;   r12 - Supplies a pointer to a trap frame.
;
; Return Value:
;
;   If the callback return cannot be executed, then an error status is
;   returned. Otherwise, the specified callback status is returned to
;   the caller of the callback function.
;
;   N.B. This function returns to the function that called out to user
;        mode is a callout is currently active.
;
;--

        NESTED_ENTRY NtCallbackReturn,, KiCallbackReturnHandler
        PROLOG_PUSH     {r11, lr}

        CURTHREAD_READ r3                               ; get current thread address
        ldr     r3, [r3, #ThInitialStack]               ; get stack control address
        ldr     r3, [r3, #KcPreviousKernel]             ; get previous kernel segment
        cmp     r3, #0                                  ; check if previous segment exists
        beq.w   KiCRNoCallback

;
; N.B. Once it has been determined that a callout is active,
;      all of the general registers may be freely used.

        CURTHREAD_READ r4                               ; re-get current thread address
        ldr     r5, [r4, #ThInitialStack]               ; get stack control address
        ldr     r6, [r5, #KcPreviousKernel]             ; get previous kernel segment

;
; Store the output buffer address and length.
;

        mov     r10, r2                                 ; save completion status
        ldr     r3, [r6, #CuOutputBuffer]               ; address for output buffer
        ldr     r2, [r6, #CuOutputLength]               ; address for output length
        str     r0, [r3]                                ; store output buffer address
        str     r1, [r2]                                ; store output length address

;
; Flush the GDI batch before transitioning to the previous stack segment in
; order to give the GDI code the maximum stack in which to run.
;

        ldr     r0, [r4, #ThTeb]                        ; get user TEB address

NtCallbackReturnTebAccess

        ldr     r0, [r0, #TeGdiBatchCount]              ; load the batch count
        cbz     r0, NtCb10                              ; if z, batch is empty

NtCallbackReturnTebResume
        movs    r0, #Win32BatchFlushCallout
        movs    r1, #0                                  ; no parameters
        movs    r2, #0                                  ; no session attach
        movs    r3, #0
        CAPSTART NtCallbackReturn, PsInvokeWin32Callout
        bl      PsInvokeWin32Callout                    ; invoke the GDI flush callout
        CAPEND NtCallbackReturn

NtCb10

;
; Restore the previous callback stack address and trap frame address.
;

        ldr     r0, [r6, #CuTrapFrame]                  ; get previous trap frame
        cpsid   i                                       ; disable interrupts
        ldr     r1, [r4, #ThTrapFrame]                  ; get current trap frame
        str     r0, [r4, #ThTrapFrame]                  ; restore previous trap frame

;
; If debug registers or instrumentation is active, then copy the debug registers
; from the current trap frame to the previous trap frame.
;
; N.B. Trap frame offsets are biased offsets.
;

        ldrb    r2, [r4, #ThDebugActive]                ; test if debug/inst. enabled
        tst     r2, #DEBUG_ACTIVE_DBG                   ;
        beq     KiCRRestore                             ; if not, skip restore
        bl      KiCopyDebugRegs                         ; copy debug regs from r0 -> r1


KiCRRestore

;
; Restore initial stack address and restore exception state.
;

        ldr     r1, [r5, #KcPreviousBase]               ; get previous stack base
        str     r1, [r4, #ThStackBase]                  ; restore stack base
        ldr.w   r2, =KeKernelStackSize
        ldr     r2, [r2]
        sub     r1, r1, r2                              ; calculate stack limit
        str     r1, [r4, #ThStackLimit]                 ; restore stack limit
        ldr     r2, [r5, #KcPreviousInitial]            ; get previous initial stack
        str     r2, [r4, #ThInitialStack]               ; restore initial stack addr
        PCR_READ r0                                     ; get pointer to PCR
        str     r2, [r0, #PcSpBase]                     ; set initial stack in PRCB
        mov     sp, r6                                  ; set stack to callback frame
        mov     r0, r10

#if (ARM_VFP_MANAGEMENT == ARM_VFP_ALWAYSON)
        RESTORE_EXCEPTION_FRAME                         ; restore exception state
        EPILOG_VPOP {d8-d15}                            ; restore non-volatile FP
#else
        ldr     r2, [sp, #KEXCEPTION_FRAME_LENGTH + 8*8]; read saved FPSCR
        cmp     r2, #-1                                 ; was it the sentinel?
        beq     %F0                                     ; if so, skip
        add     r1, sp, #KEXCEPTION_FRAME_LENGTH        ; point to FP save area
        vldm    r1, {d8-d15}                            ; restore saved registers
0
        RESTORE_EXCEPTION_FRAME                         ; restore exception state
        EPILOG_STACK_FREE 8*8 + 4 + 4                   ; release stack allocation
#endif
        EPILOG_NOP cpsie   i                            ; re-enable interrupts
        EPILOG_RETURN                                   ; return

;
; No callback is currently active.
;

KiCRNoCallback
        ldr.w   r0, =STATUS_NO_CALLBACK_ACTIVE          ; set service status
        EPILOG_POP      {r11, pc}

        NESTED_END NtCallbackReturn

        END

