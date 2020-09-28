     TTL  "Miscellaneous Functions"
;++
;
; Copyright (c) 2000  Microsoft Corporation
;
; Module Name:
;
;   miscs.asm
;
; Abstract:
;
;   This module implements machine dependent miscellaneous kernel functions.
;
; Environment:
;
;   Kernel mode only.
;
;--

#include "ksarm.h"

        TEXTAREA

        IMPORT KeTestAlertThread
        IMPORT KiContinue
        IMPORT KiExceptionRestoreFromService
        IMPORT KiFreezeTargetExecution
        IMPORT KiRaiseException
        IMPORT KiSaveProcessorControlState
        IMPORT RtlCaptureContext
        EXPORT KeSaveStateForHibernate

        SUBT "PCR functions"
;++
;
; NTSTATUS
; NtContinue (
;     IN PCONTEXT ContextRecord,
;     IN BOOLEAN TestAlert
;     )
;
; Routine Description:
;
;   This routine is called as a system service to continue execution after
;   an exception has occurred. Its function is to transfer information from
;   the specified context record into the trap frame that was built when the
;   system service was executed, and then exit the system as if an exception
;   had occurred.
;
; Arguments:
;
;   ContextRecord (r0) - Supplies a pointer to a context record.
;
;   TestAlert (r1) - Supplies a boolean value that specifies whether alert
;       should be tested for the previous processor mode.
;
; Implicit Arguments:
;
;   r12 - Supplies the address of a trap frame.
;
; Return Value:
;
;   Normally there is no return from this routine. However, if the specified
;   context record is misaligned or is not accessible, then the appropriate
;   status code is returned.
;
;--

        NESTED_ENTRY NtContinue //_TEXT$00
        GENERATE_EXCEPTION_FRAME                        ; generate exception frame

;
; Copy context to kernel frames or bypass for user APC.
;

        CURTHREAD_READ r5                               ; get current thread address
        uxtb    r4, r1                                  ; save test alert argument
        mov     r6, r12                                 ; save trap frame address
        mov     r1, sp                                  ; pass exception frame address
        mov     r2, r12                                 ; pass trap frame address
        CAPSTART NtContinue, KiContinue
        bl      KiContinue                              ; transfer context to kframes
        CAPEND  NtContinue

;
; If the kernel continuation routine returns success, then exit via the
; exception exit code. Otherwise, return to the system service dispatcher.
;

        cmp     r0, #0                                  ; success?
        bne     KiCO10                                  ; KiContinue failed, go report error

;
; Check to determine if alert should be tested for the previous processor
; mode and restore the previous mode in the thread object.
;

        ldr     r1, [r6, #TrCpsr]                       ; get previous cpsr
        tst     r1, #0xf                                ; was previous mode kernel?
        beq     KiCO04                                  ; if eq, previous mode user
        ldr     r1, [r6, #TrTrapFrame]                  ; set previous trap frame address
        str     r1, [r5, #ThTrapFrame]
        ldrb    r1, [r6, #TrPreviousMode]               ; get frame previous mode
        strb    r1, [r5, #ThPreviousMode]               ; set thread previous mode
KiCO04
        cbz     r4, KiCO05                              ; test if alert specified; if z, alert not specified
        ldrb    r0, [r5, #ThPreviousMode]               ; get thread previous mode
        CAPSTART NtContinue, KeTestAlertThread
        bl      KeTestAlertThread                       ; Alert for current thread
        CAPEND  NtContinue

;
; Return via exception exit.
;

KiCO05
        RESTORE_EXCEPTION_FRAME                         ; restore exception state
        EPILOG_BRANCH KiExceptionRestoreFromService     ; 

;
; Return via the system service dispatcher.
;

KiCO10
        RESTORE_EXCEPTION_FRAME                         ; restore exception state
        EPILOG_RETURN

        NESTED_END NtContinue //, _TEXT$00

        SUBT  "Raise Exception System Service"
;++
;
; NTSTATUS
; NtRaiseException (
;     IN PEXCEPTION_RECORD ExceptionRecord,
;     IN PCONTEXT ContextRecord,
;     IN BOOLEAN FirstChance
;     )
;
; Routine Description:
;
;   This routine is called as a system service to raise an exception. Its
;   function is to transfer information from the specified context record
;   into the trap frame that was built when the system service was executed.
;   The exception may be raised as a first or second chance exception.
;
; Arguments:
;
;   ExceptionRecord (r0) - Supplies a pointer to an exception record.
;
;   ContextRecord (r1) - Suppilies a pointer to a context record.
;
;   FirstChance (r2) - Supplies a boolean value that specifies whether
;       this is the first (TRUE) or second chance (FALSE) for dispatching
;       the exception.
;
; Implicit Arguments:
;
;   r12 - Supplies a pointer to a trap frame.
;
; Return Value:
;
;   Normally there is no return from this routine. However, if the specified
;   context record or exception record is misaligned or is not accessible,
;   then the appropriate status code is returned.
;
;--

        NESTED_ENTRY NtRaiseException //, _TEXT$00
        GENERATE_EXCEPTION_FRAME                        ; generate exception frame

;
; Record the address of the caller in a place that won't be overwritten
; by the following context to k-frames.
;

        ldr     r3, [r12, #TrPc]                        ; copy fault address to preserved entry
        str     r3, [r12, #TrFaultAddress]              ;

;
; Call the raise exception kernel routine which will marshall the arguments
; and then call the exception dispatcher.
;

        CURTHREAD_READ r4                               ; get current thread address
        mov     r5, r12                                 ; save trap frame address in r5
        uxtb    r2, r2                                  ; clear high bits of FirstChance
        str     r2, [sp, #ExParam5]                     ; set first chance parameter
        mov     r2, sp                                  ; set exception frame addr
        mov     r3, r12                                 ; set trap frame address
        CAPSTART NtRaiseException, KiRaiseException
        bl      KiRaiseException                        ; call raise exception routine
        CAPEND  NtRaiseException

;
; If the kernel raise exception routine returns success, then exit via the
; exception exit code. Otherwise, return to the system service dispatcher.
;

        cbnz    r0, KiRE20                              ; test if service failed; if nz, service failed

;
; Exit via the exception exit code which will restore the machine state.
;
; N.B. The below code uses an unusual sequence to transfer control. This
;      instruction sequence is required to avoid detection as an epilogue.
;

        ldr     r1, [r5, #TrCpsr]                       ; get previous cpsr
        tst     r1, #0xf                                ; was previous mode kernel?
        beq     KiRE10                                  ; if eq, previous mode user
        ldr     r0, [r5, #TrTrapFrame]                  ; restore prev. trap addr
        str     r0, [r4, #ThTrapFrame]
        ldrb    r1, [r5, #TrPreviousMode]               ;  restore previous mode
        strb    r1, [r4, #ThPreviousMode]

KiRE10
        RESTORE_EXCEPTION_FRAME                         ; restore exception frame
        EPILOG_BRANCH KiExceptionRestoreFromService

;
; The context or exception record is misaligned or not accessible, or the
; exception was not handled.
;

KiRE20
        RESTORE_EXCEPTION_FRAME                         ; restore exception state

        EPILOG_RETURN                                   ; return
        NESTED_END NtRaiseException //, _TEXT$00

        SUBT "Interrupt Support"

;++
;
; VOID
; KeFreezeCurrentProcessor (
;     IN PKTRAP_FRAME TrapFrame
;     )
;
; Routine Description:
;
;   This routine is called to freeze the current processor.  It generates
;   an exception frame before calling the machine independent freeze code.
;
; Arguments:
;
;   Trap Frame (r0) - a pointer to the trap frame
;
; Return Value:
;
;   None.
;
;--
        NESTED_ENTRY KiFreezeCurrentProcessor
        GENERATE_EXCEPTION_FRAME
        PROLOG_STACK_ALLOC KTRAP_FRAME_LENGTH
        
        strd    r0, r1, [sp, #TrR0]         ; save r0-r1
        strd    r2, r3, [sp, #TrR2]         ; save r2-r3
        str     r12, [sp, #TrR12]           ; save r12
        mov     r0, sp                      ; get SP into r0
        strd    r0, lr, [sp, #TrSp]         ; save sp/lr
        str     r11, [sp, #TrR11]           ; save r11
        adr     r2, TrapFrameInitialized+1  ; get a PC within this function
        READ_CPSR r3                        ; get flags
        strd    r2, r3, [sp, #TrPc]         ; save pc/cpsr
        mov     r2, #KEXCEPTION_ACTIVE_INTERRUPT_FRAME ; treat as an interrupt frame
        mov     r3, #0                      ; NULL out the VFP state
        strd    r2, r3, [sp, #TrExceptionActive] ; set exception type and clear out byte fields plus VFP state

TrapFrameInitialized
        add     r1, sp, #KTRAP_FRAME_LENGTH ; pass exception frame in r1 (r0 already has trap frame)
        bl      KiFreezeTargetExecution                 ; freeze target

        EPILOG_STACK_FREE KTRAP_FRAME_LENGTH
        RESTORE_EXCEPTION_FRAME
        EPILOG_RETURN

        NESTED_END KiFreezeCurrentProcessor

;++
;
; VOID
; KeSaveStateForHibernate (
;     IN PKPROCESSOR_STATE ProcessorState
;     )
;
; Routine Description:
;
;   Saves all processor-specific state that must be preserved context losing
;   idle-states or across a processor reset.
;
;   N.B. The caller must setup the KPROCESSOR_STATE's {r0, sp, lr, pc}.
;
; Arguments:
;
;   ProcessorState - Supplies the KPROCESSOR_STATE where the current CPU's
;       state is to be saved.
;
; Return Value:
;
;   None.
;
;--

        NESTED_ENTRY KeSaveStateForHibernate

        PROLOG_PUSH         {r11, lr}
        PROLOG_STACK_ALLOC  8

        str     r0, [sp, #0]                ; save r0
        READ_CPSR r0                        ; get flags
        str     r0, [sp, #4]                ; save flags

        cpsid   i                           ; disable interrupts
        ldr     r0, [sp, #0]                ; restore r0
        adds    r0, r0, #PsContextFrame     ; advance r0 to context frame
        bl      RtlCaptureContext           ; capture context
        ldr     r0, [sp, #0]                ; restore r0
        bl      KiSaveProcessorControlState ; capture control state
        ldr     r0, [sp, #4]                ; restore flags
        tst     r0, #CPSRC_INT              ; were interrupts enabled?
        bne     Done                        ; if not, skip restore
        cpsie   i                           ; restore interrupts

Done

        ;
        ; Save the caller's r11, pc, and sp.
        ;

        ldr     r0, [sp, #0]                ; restore r0
        ldr     r1, [sp, #8]
        str     r1, [r0, #PsContextFrame + CxR11]

        ldr     r1, [sp, #12]
        str     r1, [r0, #PsContextFrame + CxPc]

        add     r1, sp, #16
        str     r1, [r0, #PsContextFrame + CxSp]

        EPILOG_STACK_FREE   8
        EPILOG_POP          {r11, pc}

        NESTED_END KeSaveStateForHibernate

;++
;
; VOID
; KiReadBankedRegisters (
;     __in ULONG ProcessorMode,
;     __out PULONG Sp,
;     __out PULONG Lr,
;     __out PULONG Spsr
;     )
;
; Routine Description:
;
;   This routine reads the banked state of the current processor.
;
; Arguments:
;
;   ProcessorMode (r0) - Banked processor mode to affect.
;
;   Sp (r1) - Pointer to ULONG that will receive SP, or NULL.
;
;   Lr (r2) - Pointer to ULONG that will receive LR, or NULL.
;
;   Spsr (r3) - Pointer to ULONG that will receive SPSR, or NULL.
;
; Return Value:
;
;   None.
;
;--

        LEAF_ENTRY KiReadBankedRegisters

        sub     r0, r0, #0x10                           ; only modes 0x10-0x1f supported
        cmp     r0, #0x10                               ;
        bhs     KiRBR90                                 ;

        tbb     [pc, r0]                                ; branch based on the mode
RBRTab  dcb     (KiRBR1f - RBRTab) / 2                  ; 0x10 = USR mode -> SYS
        dcb     (KiRBR90 - RBRTab) / 2                  ; 0x11 = FIQ mode -> invalid
        dcb     (KiRBR12 - RBRTab) / 2                  ; 0x12 = IRQ mode -> INT
        dcb     (KiRBR13 - RBRTab) / 2                  ; 0x13 = SVC mode -> SVC
        dcb     (KiRBR90 - RBRTab) / 2                  ; 0x14 = invalid
        dcb     (KiRBR90 - RBRTab) / 2                  ; 0x15 = invalid
        dcb     (KiRBR90 - RBRTab) / 2                  ; 0x16 = Monitor mode -> invalid
        dcb     (KiRBR17 - RBRTab) / 2                  ; 0x17 = Abort mode -> ABT
        dcb     (KiRBR90 - RBRTab) / 2                  ; 0x18 = invalid
        dcb     (KiRBR90 - RBRTab) / 2                  ; 0x19 = invalid
        dcb     (KiRBR90 - RBRTab) / 2                  ; 0x1a = invalid
        dcb     (KiRBR1b - RBRTab) / 2                  ; 0x1b = Undefined mode -> UDF
        dcb     (KiRBR90 - RBRTab) / 2                  ; 0x1c = invalid
        dcb     (KiRBR90 - RBRTab) / 2                  ; 0x1d = invalid
        dcb     (KiRBR90 - RBRTab) / 2                  ; 0x1e = invalid
        dcb     (KiRBR1f - RBRTab) / 2                  ; 0x1f = System mode -> SYS

KiRBR1f cps     #CPSRM_SYS                              ; System mode
        movs    r3, #0                                  ; no SPSR in system mode
        b       KiRBR40                                 ; go to common code

KiRBR12 cps     #CPSRM_INT                              ; IRQ mode
        b       KiRBR40                                 ; go to common code

KiRBR17 cps     #CPSRM_ABT                              ; Abort mode
        b       KiRBR40                                 ; go to common code

KiRBR1b cps     #CPSRM_UDF                              ; Undefined mode
                                                        ; fall through to common code

KiRBR13                                                 ; SVC mode, just fall through

KiRBR40 cbz     r1, KiRBR50                             ; does caller want SP?
        str     sp, [r1]                                ; if so, store it

KiRBR50 cbz     r2, KiRBR60                             ; does caller want LR?
        str     lr, [r2]                                ; if so, store it
        
KiRBR60 cbz     r3, KiRBR80                             ; does caller want SPSR?
        mrs     r12, spsr                               ; if so, read it
        str     r12, [r3]                               ; and store it
        
KiRBR80 cps     #CPSRM_SVC                              ; back to SVC mode
KiRBR90 bx      lr                                      ; return

        LEAF_END KiReadBankedRegisters

;++
;
; VOID
; KiWriteBankedRegisters (
;     __in ULONG ProcessorMode,
;     __in PULONG Sp,
;     __in PULONG Lr,
;     __in PULONG Spsr
;     )
;
; Routine Description:
;
;   This routine writes the banked state of the current processor.
;
; Arguments:
;
;   ProcessorMode (r0) - Banked processor mode to affect.
;
;   Sp (r1) - Pointer to ULONG that contains SP, or NULL.
;
;   Lr (r2) - Pointer to ULONG that contains LR, or NULL.
;
;   Spsr (r3) - Pointer to ULONG that contains SPSR, or NULL.
;
; Return Value:
;
;   None.
;
;--

        LEAF_ENTRY KiWriteBankedRegisters

        sub     r0, r0, #0x10                           ; only modes 0x10-0x1f supported
        cmp     r0, #0x10                               ;
        bhs     KiWBR90                                 ;

        tbb     [pc, r0]                                ; branch based on the mode
WBRTab  dcb     (KiWBR1f - WBRTab) / 2                  ; 0x10 = USR mode -> SYS
        dcb     (KiWBR90 - WBRTab) / 2                  ; 0x11 = FIQ mode -> invalid
        dcb     (KiWBR12 - WBRTab) / 2                  ; 0x12 = IRQ mode -> INT
        dcb     (KiWBR13 - WBRTab) / 2                  ; 0x13 = SVC mode -> SVC
        dcb     (KiWBR90 - WBRTab) / 2                  ; 0x14 = invalid
        dcb     (KiWBR90 - WBRTab) / 2                  ; 0x15 = invalid
        dcb     (KiWBR90 - WBRTab) / 2                  ; 0x16 = Monitor mode -> invalid
        dcb     (KiWBR17 - WBRTab) / 2                  ; 0x17 = Abort mode -> ABT
        dcb     (KiWBR90 - WBRTab) / 2                  ; 0x18 = invalid
        dcb     (KiWBR90 - WBRTab) / 2                  ; 0x19 = invalid
        dcb     (KiWBR90 - WBRTab) / 2                  ; 0x1a = invalid
        dcb     (KiWBR1b - WBRTab) / 2                  ; 0x1b = Undefined mode -> UDF
        dcb     (KiWBR90 - WBRTab) / 2                  ; 0x1c = invalid
        dcb     (KiWBR90 - WBRTab) / 2                  ; 0x1d = invalid
        dcb     (KiWBR90 - WBRTab) / 2                  ; 0x1e = invalid
        dcb     (KiWBR1f - WBRTab) / 2                  ; 0x1f = System mode -> SYS

KiWBR1f cps     #CPSRM_SYS                              ; System mode
        movs    r3, #0                                  ; no SPSR in system mode
        b       KiWBR40                                 ; go to common code

KiWBR12 cps     #CPSRM_INT                              ; IRQ mode
        b       KiWBR40                                 ; go to common code

KiWBR17 cps     #CPSRM_ABT                              ; Abort mode
        b       KiWBR40                                 ; go to common code

KiWBR1b cps     #CPSRM_UDF                              ; Undefined mode
                                                        ; fall through to common code

KiWBR13                                                 ; SVC mode, just fall through

KiWBR40 cbz     r1, KiWBR50                             ; does caller want SP?
        ldr     sp, [r1]                                ; if so, read it

KiWBR50 cbz     r2, KiWBR60                             ; does caller want LR?
        ldr     lr, [r2]                                ; if so, read it
        
KiWBR60 cbz     r3, KiWBR80                             ; does caller want SPSR?
        ldr     r12, [r3]                               ; if so, read it
        msr     spsr_fc, r12                            ; and store it back
        
KiWBR80 cps     #CPSRM_SVC                              ; back to SVC mode
KiWBR90 bx      lr                                      ; return

        LEAF_END KiWriteBankedRegisters

;++
;
; ULONG64
; ReadTimeStampCounter (
;     VOID
;     )
;
; Routine Description:
;
;   Read the cycle counter and return a 64-bit extended result.
;
; Arguments:
;
;   None.
;
; Return Value:
;
;   The 64-bit cycle counter value in r0(lo) and r1(hi).
;
;--

        LEAF_ENTRY ReadTimeStampCounter

        READ_CYCLE_COUNTER_64BIT r0, r1, r2, r3         ; use standard macro
        bx      lr                                      ; return

        LEAF_END ReadTimeStampCounter


;++
;
; ULONG
; LogVfpState (
;     __out PKARM_VFP_STATE VfpState
;     )
;
; Routine Description:
;
;   Write the currently saved VFP state to the given buffer.
;
; Arguments:
;
;   None.
;
; Return Value:
;
;  -1 if the state was invalid
;   0 if the state was live and came from live registers
;   1 if the state came from the thread's context
;
;--

        LEAF_ENTRY LogVfpState
        
#if (ARM_VFP_MANAGEMENT != ARM_VFP_ALWAYSON)
        CP_READ r1, CP15_CPACR                          ; read CPACR
        tst     r1, #CP15_CPACR_VFP_MASK                ; is the VFP enabled?
        beq     %F2                                     ; if not, we're in state 0 or 1
#endif

        SAVE_VFP_STATE r0, r2                           ; write the VFP state
        movs    r0, #0                                  ; indicate state 0
        bx      lr                                      ; return

#if (ARM_VFP_MANAGEMENT != ARM_VFP_ALWAYSON)
2
        CURTHREAD_READ r2                               ; get thread pointer
        cbz     r2, %F0                                 ; fail if NULL
        ldr     r2, [r2, #ThVfpState]                   ; get thread's VFP state
        cbz     r2, %F0                                 ; fail if NULL
        ldr     r3, [r2, #VsFpscr]                      ; load FPSCR
        str     r3, [r0, #VsFpscr]                      ; save FPSCR
        adds    r0, r0, #VsVfpD                         ; point to D registers
        adds    r2, r2, #VsVfpD                         ; ditto
        movs    r3, #32                                 ; 32 registers
4
        subs    r3, r3, #1                              ; decrement counter
        ldrd    r1, r12, [r2], #8                       ; read from source
        strd    r1, r12, [r0], #8                       ; write to dest
        bne     %B4                                     ; loop until done
        movs    r0, #0                                  ; indicate state 0
        bx      lr                                      ; return
0
        movs    r0, #-1                                 ; invalid state
        bx      lr                                      ; return
#endif

        LEAF_END LogVfpState

        END

