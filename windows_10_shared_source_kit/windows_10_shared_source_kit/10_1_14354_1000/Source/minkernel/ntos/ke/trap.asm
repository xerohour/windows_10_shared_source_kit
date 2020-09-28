     TTL  "Trap Processing"
;++
;
; Copyright (c) 2003  Microsoft Corporation
;
; Module Name:
;
;   trap.asm
;
; Abstract:
;
;   This module implements the code necessary to field and process ARM
;   exceptions
;
; Environment:
;
;   Kernel mode only.
;
;--

#include "ksarm.h"

        AREA    |.text|,ALIGN=5,CODE,READONLY

#if ARM_VFP_ENABLE_STATISTICS
        IMPORT KiArmVfpStats
#endif
        IMPORT KdSetOwedBreakpoints
        IMPORT KdpOweBreakpoint
        IMPORT KeBugCheckEx
        IMPORT KeServiceDescriptorTable
        IMPORT KeServiceDescriptorTableShadow
        IMPORT KeServiceDescriptorTableFilter
#if ENABLE_MULTIPLE_WIN32K
        IMPORT KeServiceDescriptorTableSession
#endif
        IMPORT __imp_KfLowerIrql
        IMPORT KiCopyCounters
        IMPORT KiDebugException
        IMPORT KiDeliverApc
        IMPORT KiDispatchException
        IMPORT KiDispatchInterrupt
        IMPORT KiFatalExceptionHandler
        IMPORT KiGetDebugExceptionType
        IMPORT KiInitiateUserApc
        IMPORT KiPlayInterrupt
        IMPORT KiProcessInterrupt
        IMPORT KiReadDebugRegs
        IMPORT KiTrapFrameLog
        IMPORT KiTrapFrameLogIndex
        IMPORT KiWriteDebugRegs
        IMPORT MmAccessFault
        IMPORT MmSystemRangeStart
        IMPORT MmUserProbeAddress
        IMPORT PerfInfoLogSysCallEntry
        IMPORT PerfInfoLogSysCallExit
        IMPORT PsConvertToGuiThread
        IMPORT PsInvokeWin32Callout
        IMPORT PsPicoSystemCallDispatch
        IMPORT PsWatchEnabled
        IMPORT PsWatchWorkingSet
        IMPORT PerfGlobalGroupMask
        IMPORT RtlUnwindEx

#if defined(_CAPKERN)

        IMPORT CAPLogSystemService
        IMPORT CAPLogTrap

#endif

;
; Global start label for checking faults
;

KiTrapModuleStart


     SUBT "trap handling"

        MACRO
        CAP_TRAP

#if defined(_CAPKERN)

        str     lr, [sp, #4]                            ; save return address
        mov     r0, lr                                  ; load handler address
        bl      CAPLogTrap                              ; call Trap probe
        ldr     lr, [sp, #4]                            ; restore return address

#endif

        MEND

        MACRO
        CAP_SYSTEMSERVICE

#if defined(_CAPKERN)

        strd    r12, lr, [sp, #TRAP_FRAME_OFFSET]       ; save trap frame pointer and service address
        mov     r0, lr                                  ; set address of system service
        bl      CAPLogSystemService                     ; log system call event
        ldrd    r12, lr, [sp, #TRAP_FRAME_OFFSET]       ; restore trap frame pointer and service address

#endif

        MEND


;
; N.B. ARM Exception Processing
;
;      There are 7 execution modes on an modern ARM processor -- one
;      for each type of exception (including two types of interrupts),
;      a user mode, and a system mode.  Each mode has banked R13, R14, SPSR
;      registers.  FIQ has additional banked registers.
;
;      In this port, user mode code runs in the ARM user mode and
;      kernel mode code runs in ARM's supervisor mode (SVC) code rather than
;      the ARM system mode.  On any exception or interrupt, a
;      transition is made to the SVC mode stack via the SRS instruction.
;
;


;++
;
; Routine Description:
;
;   This macro updates a circular buffer of mini-trap frame logs based on the
;   trap frame information stored at [sp].
;
;   Logs are maintained for the first four processors. Each entry in the log
;   is 48 bytes long, and there are 32 of them per processor (128 total for
;   four CPUs).
;
; Arguments:
;
;   sp - pointer to the trap frame
;
; Disposition:
;
;   This code will overwrite r0-r3 and r12, just like a function call
;
;--

        MACRO
        LOG_MINI_TRAP_FRAME $Type

        PCR_READ r0                                     ; get PCR
        ldr     r1, [r0, #PcNumber]                     ; get CPU number
        mov32   r2, KiTrapFrameLogIndex                 ; get array of indices
        mov32   r12, KiTrapFrameLog                     ; get pointer to the log itself
        cmp     r1, #4                                  ; CPU #4 or more?
        bhs     %F1                                     ; if so, skip logging

        ldr     r3, [r2, r1, lsl #2]                    ; read log index for this processor
        ldr     r0, [r0, #PcCurrentThread]              ; get current thread
        bfi     r3, r1, #5, #32-5                       ; insert processor index starting at bit 5

        add     r12, r12, r3, lsl #5                    ; multiply by 32 and add to base
        add     r12, r12, r3, lsl #4                    ; multiply by 16 and also add (32+16=48)

        adds    r3, r3, #1                              ; increment log index (ignoring overflow)
        str     r3, [r2, r1, lsl #2]                    ; store updated index

        orr     r1, r1, #($Type:SHL:8)                  ; OR in the type
        ldrd    r2, r3, [sp, #TrR0]                     ; load r0/r1
        strd    r0, r1, [r12, #TlThread]                ; store thread/CPU number
        strd    r2, r3, [r12, #TlR0]                    ; store r0/r1

        ldrd    r0, r1, [sp, #TrR2]                     ; load r2/r3
        ldrd    r2, r3, [sp, #TrR12]                    ; load r12/sp
        strd    r0, r1, [r12, #TlR2]                    ; save r2/r3
        strd    r2, r3, [r12, #TlR12]                   ; save r12/sp
        ldrd    r0, r1, [sp, #TrLr]                     ; load lr/r11
        ldrd    r2, r3, [sp, #TrPc]                     ; load pc/cpsr
        strd    r0, r1, [r12, #TlLr]                    ; save lr/r11
        strd    r2, r3, [r12, #TlPc]                    ; save pc/cpsr
1
        MEND


;++
;
; Routine Description:
;
;   This macro creates a trap frame on the stack, after a trap handler's initial
;   setup.
;
; Arguments:
;
;   sp - pointer to mini-stack structure
;
; Disposition:
;
;   Upon entry, the caller is required to make sure that the the LR and SPSR are
;   saved into the current mode's mini-stack.
;
;--

        MACRO
        BUILD_TRAP_FRAME $FaultLabel, $PcOffsetThumb, $PcOffsetARM, $ExceptionActive, $TrapType

;
; Free up r4/r5 by saving them to the mini-stack, and remember the
; mini-stack's location by putting it into r4.
;

        strd    r4, r5, [sp, #MsR4]                     ; save r4/r5 at exception:[sp+8]
        mov     r4, sp                                  ; r4 = stack pointer in original mode

;
; Switch to SVC mode, allocate space on the stack, and align it,
; keeping the starting SVC stack pointer in r5.
;

        cps     #CPSRM_SVC                              ; switch to SVC mode
        mov     r5, sp                                  ; r5 = initial stack pointer in SVC mode
        tst     r5, #4                                  ; see if it's misaligned
        sub     sp, sp, #KTRAP_FRAME_LENGTH + VFP_STATE_LENGTH + ARM_RED_ZONE_BYTES ; allocate trap frame + red-zone
        beq     %F1                                     ; if aligned, skip next instruction
        sub     sp, sp, #4                              ; subtract an additional 4 bytes to align the stack
1

;
; Touch the bottom of the proposed trap frame to force a fault at
; a known location in the event of a stack overflow.
;
; N.B. KiDataAbortPanicBugCheck assumes some knowledge of the mini-stack and
;      register contents in the event of a fault here.
;

$FaultLabel                                             ; look for faults here
        str     r4, [sp]                                ; do a probing store

;
; Save volatile integer registers, plus r11/lr to form a frame chain
; link. Also initialize the ExceptionActive and Reserved fields.
;
; N.B. The value stored in Reserved is a sentinel to provide the fast stack walking
;      code a means of detecting an interrupt trap frame. The sentinel used is the
;      code address of the reset exception handler, with the thumb bit cleared. This
;      address is unlikely to accidentally appear on the stack in any other context,
;      and is cleared on when returning through the trap frame.
;

        strd    r0, r1, [sp, #TrR0]                     ; save r0/r1
        ldrd    r0, r1, [r4, #MsPc]                     ; fetch saved PC/CPSR from mini:[sp+0]
        strd    r2, r3, [sp, #TrR2]                     ; save r2/r3
        ubfx    r2, r1, #5, #1                          ; extract CPSRC_THUMB bit into r2
        strd    r12, r5, [sp, #TrR12]                   ; save r12/initial SP
        orrs    r0, r0, r2                              ; OR thumb bit into PC
        strd    lr, r11, [sp, #TrLr]                    ; save lr and r11 for the frame chain
        IF "$PcOffsetThumb" != "0"
        subs    r0, r0, #$PcOffsetThumb                 ; recover original PC, assuming Thumb mode
        ENDIF
        ldrd    r4, r5, [r4, #MsR4]                     ; recover r4/r5 from mini:[sp+8]
        IF "$PcOffsetThumb" != "$PcOffsetARM"
        cbnz    r2, %F1                                 ; if we came from thumb, PC is fixed
        subs    r0, r0, #$PcOffsetARM - $PcOffsetThumb  ; otherwise, apply additional ARM32 offset                   
1
        ENDIF
        mov32   r2, KiResetException - 1                ; get de-thumb-ified address of KiResetException
        movs    r3, #$ExceptionActive                   ; get ExceptionActive value
        tst     r1, #0xf                                ; did we come from user mode?
        strd    r0, r1, [sp, #TrPc]                     ; save PC/SPSR
        add     r11, sp, #TrR11                         ; point r11 to the frame chain
        strd    r2, r3, [sp, #TrReserved]               ; write Reserved and ExceptionActive, clearing other byte fields
        bne     %F1                                     ; if not from user mode, skip over debug regs and user SP/LR

;
; When coming from user mode, fetch the user mode SP/LR and overwrite the ones
; that were originally saved. Also read the debug registers if appropriate.
;

        PCR_READ r12                                    ; get PCR
        ldr     r12, [r12, #PcCurrentThread]            ; get current thread
        CURTHREAD_WRITE_PASSIVE r12                     ; reload it back into TPIDRURO
        ldrb    r1, [r12, #ThDebugActive]               ; get debug active flag
        mov     r2, sp                                  ; point r0 to our stack
        cps     #CPSRM_SYS                              ; go to SYS mode to get user state
        str     sp, [r2, #TrSp]                         ; store user mode SP
        str     lr, [r2, #TrLr]                         ; store user mode LR
        cps     #CPSRM_SVC                              ; back to SVC mode
        tst     r1, #DEBUG_ACTIVE_DBG                   ; test if debug active
        beq     %F1                                     ; if no debugging going on, don't bother with debug regs
        mov     r0, sp                                  ; r0 = trapframe
        bl      KiReadDebugRegs                         ; fetch the debug registers
1

;
; In all modes, save the FPU state as needed.
;

#if (ARM_VFP_MANAGEMENT == ARM_VFP_ALWAYSON)
        add     r0, sp, #KTRAP_FRAME_LENGTH             ; point to the VFP state
        str     r0, [sp, #TrVfpState]                   ; store the VFP state pointer
        SAVE_VFP_STATE r0, r1                           ; write the VFP state (leaves FPSCR in r1)
        bic     r1, r1, #FPSCRM_DEPRECATED              ; clear deprecated bits in FPSCR
        vmsr    fpscr, r1                               ; update the value
#else
        CP_READ r0, CP15_CPACR                          ; read CPACR
        movs    r1, #0                                  ; default to a NULL pointer
        tst     r0, #CP15_CPACR_VFP_MASK                ; is the VFP enabled?
        beq     %F2                                     ; if not, skip and store the NULL
        add     r3, sp, #KTRAP_FRAME_LENGTH             ; VFP state is at the end of the trap frame
        add     r1, sp, #KTRAP_FRAME_LENGTH             ; VFP state is at the end of the trap frame
        SAVE_VFP_STATE r3, r2                           ; write the VFP state (leaves FPSCR in r2)
        bic     r2, r2, #FPSCRM_DEPRECATED              ; clear deprecated bits in FPSCR
        vmsr    fpscr, r2                               ; update the value
2       str     r1, [sp, #TrVfpState]                   ; store the VFP state pointer
#if ARM_VFP_ENABLE_STATISTICS
        mov32   r1, KiArmVfpStats + VtTrapsEnabled      ; get pointer to enabled stat
        tst     r0, #CP15_CPACR_VFP_MASK                ; is the VFP enabled?
        addeq   r1, r1, #8                              ; if not, point to traps disabled stat
        INCREMENT_STAT r1, r0, r2, r12                  ; increment the statistic
#endif
#endif

;
; Do any trap frame logging and get out
;

        LOG_MINI_TRAP_FRAME $TrapType
        CAP_TRAP

        MEND


;++
;
; Routine Description:
;
;   This macro restores the state of the processor from a trap frame and returns
;   to kernel or user mode as appropriate.
;
; Arguments:
;
;   sp - implicit trap frame
;
; Disposition:
;
;   Returns to the target mode.
;
;--

        MACRO
        RESTORE_FROM_TRAP_FRAME $Syscall

;
; General case: Determine kernel versus user-mode restore and load PC/SPSR.
;

        IF "$Syscall" == ""

        ldr     r1, [sp, #TrCpsr]                       ; get target CPSR
        cpsid   i                                       ; disable interrupts
        clrex                                           ; clear interlocked state
        tst     r1, #0xf                                ; are we returning to user mode?
        beq     %F2                                     ; if so, skip past the kernel restore

        ELSE

        cpsid   i                                       ; disable interrupts
        clrex                                           ; clear interlocked state

        ENDIF

;
; Kernel mode restore: first recover floating-point state
;

        IF "$Syscall" == ""

        ldr     r2, [sp, #TrVfpState]                   ; r2 = pointer to VFP state
        ldr     r0, [sp, #TrPc]                         ; fetch target PC
#if (ARM_VFP_MANAGEMENT != ARM_VFP_ALWAYSON)
        cbz     r2, %F1                                 ; skip if NULL
#if (ARM_VFP_MANAGEMENT == ARM_VFP_LAZY_WITH_DEMOTION)
        CP_READ r3, CP15_CPACR                          ; get CPACR
        tst     r3, #CP15_CPACR_VFP_MASK                ; is VFP enabled?
        beq.w   %F97                                    ; if not, copy it
#endif
#endif
        RESTORE_VFP_STATE r2, r3                        ; restore VFP state
1

;
; Kernel mode restore: Clear thumb bit on target PC, restore integer registers 
; and return
;

        movs    r3, #0                                  ; r3 = 0
        ldrd    r12, r2, [sp, #TrR12]                   ; restore r12 and get original SP
        str     r3, [sp, #TrReserved]                   ; overwrite the reserved field with zero
        bfi     r1, r0, #5, #1                          ; insert low bit of PC as CPSR thumb bit
        bic     r0, r0, #1                              ; clear the PC's low bit
        mov     r3, sp                                  ; save current SP in r3
        mov     sp, r2                                  ; set the final SVC stack pointer
        ldrd    lr, r11, [r3, #TrLr]                    ; restore lr/r11
        cps     #CPSRM_INT                              ; switch to INT mode
        strd    r0, r1, [sp, #MsPc]                     ; write Cpsr, Pc to INT mini-stack
        ldrd    r0, r1, [r3, #TrR0]                     ; restore r0/r1
        ldrd    r2, r3, [r3, #TrR2]                     ; restore r2/r3
        rfeia   sp                                      ; restore from exception
2

        ENDIF

;
; User mode restore: fetch interesting flags from the thread and dispatch
; to out-of-line helpers where needed
;

3
        CURTHREAD_READ_PASSIVE r12                      ; get current thread address
        ldrb    r0, [r12, #ThApcState + AsUserApcPending]; get the APC pending flag
        ldrh    r1, [r12, #ThThreadControlFlags]        ; get the ThreadControlFlags (lo) and DebugActive (hi)
        cmp     r0, #0
        bne     %F90                                    ; if APC pending, go out of line to handle it

;
; Currently live: r12=current thread, r1=ThreadControlFlags/DebugActive
;

        tst     r1, #THREAD_FLAGS_CYCLE_PROFILING       ; check for profiling in ThreadControlFlags
        bne     %F91                                    ; if enabled, go out of line to update counters

;
; Currently live: r12=current thread, r1=ThreadControlFlags/DebugActive
;

4
        tst     r1, #(DEBUG_ACTIVE_DBG << 8)            ; check DebugActive flag
        bne     %F92                                    ; if debugging active, go out of line to update debug regs

;
; Currently live: r12=current thread
;

5
        ldrb    r1, [r12, #ThDebugActive]               ; get debug flags
        tst     r1, #DEBUG_ACTIVE_MINIMAL_THREAD        ; test for minimal flag
        beq     %F51                                    ; if not minimal, skip

;
; Load the custom read-only base for pico processes to be used in TPIDRURO.
;

        ldr     r3, [r12, #ThUserRoBase]                ; get custom pico base for user thread value
        b       %F52
               
;
; Load the processor number for non-pico processes to be used in TPIDRURO.
;

51
        PCR_READ r3                                     ; get the PCR pointer
        ldr     r3, [r3, #PcNumber]                     ; get processor number for user thread value

52
        CURTHREAD_WRITE_PASSIVE r3                      ; replace the thread pointer user thread value
        ldr     r2, [sp, #TrVfpState]                   ; fetch VFP state pointer

;
; User mode restore: next, recover floating point state
; Currently live: r2 = VPFState
;

        IF "$Syscall" == ""
        ldrd    r0, r1, [sp, #TrPc]                     ; load final PC/CPSR
        ELSE
        ldr     r0, [sp, #TrPc]                         ; load final PC
        movs    r1, #CPSRM_USER                         ; sanitize CPSR to nothing
        ENDIF
        bfi     r1, r0, #5, #1                          ; insert low bit of PC into CPSR Thumb bit
        bic     r0, r0, #1                              ; clear the PC's low bit

#if (ARM_VFP_MANAGEMENT != ARM_VFP_ALWAYSON)
        cbz     r2, %F6                                 ; skip if NULL
#if (ARM_VFP_MANAGEMENT == ARM_VFP_LAZY_WITH_DEMOTION)
        CP_READ r3, CP15_CPACR                          ; get CPACR
        tst     r3, #CP15_CPACR_VFP_MASK                ; is VFP enabled?
        beq     %F98                                    ; if not, copy it
#endif
#endif

        IF "$Syscall" == ""

        RESTORE_VFP_STATE r2, r3                        ; just restore always

        ELSE

        adds    r2, r2, #VsVfpD + 8*8                   ; point to d8-d15
        vldm    r2, {d8-d15}                            ; restore registers
        vmov.f32 q0, #0                                 ; sanitize remaining registers
        vmov.f32 q1, #0                                 ;
        vmov.f32 q2, #0                                 ;
        vmov.f32 q3, #0                                 ;
        vmov.f32 q8, #0                                 ;
        vmov.f32 q9, #0                                 ;
        vmov.f32 q10, #0                                ;
        vmov.f32 q11, #0                                ;
        vmov.f32 q12, #0                                ;
        vmov.f32 q13, #0                                ;
        vmov.f32 q14, #0                                ;
        vmov.f32 q15, #0                                ;

        ENDIF
6

;
; User mode restore: Restore integer registers and return
;

        IF "$Syscall" == ""

        mov     r3, #0                                  ; r3 = 0
        ldrd    r12, r2, [sp, #TrR12]                   ; restore r12 and get original SP
        str     r3, [sp, #TrReserved]                   ; overwrite the reserved field with zero
        mov     r3, sp                                  ; save current SP in r3
        add     sp, sp, #KTRAP_FRAME_LENGTH + VFP_STATE_LENGTH + ARM_RED_ZONE_BYTES ; restore original SVC stack pointer
        cps     #CPSRM_SYS                              ; go to SYS mode to modify user state
        mov     sp, r2                                  ; restore user mode SP
        ldrd    lr, r11, [r3, #TrLr]                    ; restore lr/r11
        cps     #CPSRM_INT                              ; switch to INT mode
        strd    r0, r1, [sp, #MsPc]                     ; write Cpsr, Pc to INT ministack

        ldrd    r0, r1, [r3, #TrR0]                     ; restore r0/r1
        ldrd    r2, r3, [r3, #TrR2]                     ; restore r2/r3

        ELSE

        mov     r3, sp                                  ; save current SP in r3
        add     sp, sp, #KTRAP_FRAME_LENGTH + VFP_STATE_LENGTH + ARM_RED_ZONE_BYTES ; restore original SVC stack pointer
        cps     #CPSRM_SYS                              ; go to SYS mode to modify user state
        ldr     sp, [r3, #TrSp]                         ; restore user mode SP
        ldrd    lr, r11, [r3, #TrLr]                    ; restore lr/r11
        cps     #CPSRM_INT                              ; switch to INT mode
        strd    r0, r1, [sp, #MsPc]                     ; write Cpsr, Pc to INT ministack
        ldr     r0, [r3, #TrR0]                         ; recover return value

;
; Re-enable instrumentation for non-pico/minimal processes.
;

        ldrb    r2, [r12, #ThDebugActive]               ; get debug flags
        tst     r2, #DEBUG_ACTIVE_MINIMAL_THREAD        ; test for minimal flag
        bne     %F93                                    ; if minimal, go out of line

        movs    r1, #0                                  ; sanitize remaining volatiles
        SETUP_FOR_INSTRUMENTATION_RETURN                ; activate instrumentation callback
        movs    r2, #0                                  ; sanitize remaining volatiles
        movs    r3, #0                                  ; sanitize remaining volatiles
        mov     r12, r1                                 ; sanitize remaining volatiles

        ENDIF

61
        rfeia   sp                                      ; restore from exception

;
; Process any pending APCs
;

90
        SET_THREAD_AND_IRQL r2, r12, #APC_LEVEL         ; set IRQL to APC level
        cpsie   i                                       ; allow interrupts
        bl      KiInitiateUserApc                       ; initiate APC execution
        mov32   r3, __imp_KfLowerIrql                   ; get lower irql thunk
        ldr     r3, [r3]                                ; get lower irql routine
        movs    r0, #PASSIVE_LEVEL                      ; get passive level
        blx     r3                                      ; lower irql to passive
        cpsid   i                                       ; disable interrupts
        b       %B3                                     ; loop back and check again

;
; Copy profiling counters
;

91
        mov     r0, r12                                 ; thread pointer is parameter to...
        bl      KiCopyCounters
        CURTHREAD_READ_PASSIVE r12                      ; get current thread address
        ldrh    r1, [r12, #ThThreadControlFlags]        ; get the ThreadControlFlags (lo) and DebugActive (hi)
        b       %B4                                     ; return to mainline

;
; Copy debug registers
;

92
        mov     r0, sp                                  ; r0 -> trapframe
        bl      KiWriteDebugRegs                        ; write changes to the debug registers
        CURTHREAD_READ_PASSIVE r12                      ; get current thread address
        b       %B5                                     ; return to mainline

;
; Restore volatile registers for pico/minimal processes.
;

93
        ldr     r12, [r3, #TrR12]                       ; restore r12
        ldrd    r0, r1, [r3, #TrR0]                     ; restore r0/r1
        ldrd    r2, r3, [r3, #TrR2]                     ; restore r2/r3
        b       %B61


;
; There is valid VFP state but the VFP is disabled; copy
; the state to the thread's buffer so it will be restored
; on the next touch
;

#if (ARM_VFP_MANAGEMENT == ARM_VFP_LAZY_WITH_DEMOTION)

97
        CURTHREAD_READ r12                              ; get current thread
        ldr     r3, [r3, #ThVfpState]                   ; get state pointer
        movs    r2, #(VFP_STATE_LENGTH / 8)             ; loop counter
971     subs    r2, r2, #1                              ; decrement count
        ldrd    r0, r1, [r2], #8                        ; copy data
        strd    r0, r1, [r3], #8                        ; copy data
        bne     %B971                                   ; loop until done
        ldrd    r0, r1, [sp, #TrPc]                     ; re-fetch target PC/CPSR
        b       %B1

98
        ldr     r3, [r12, #ThVfpState]                  ; get state pointer
        movs    r2, #(VFP_STATE_LENGTH / 8)             ; loop counter
981     subs    r2, r2, #1                              ; decrement count
        ldrd    r0, r1, [r2], #8                        ; copy data
        strd    r0, r1, [r3], #8                        ; copy data
        bne     %B981                                   ; loop until done
        ldrd    r0, r1, [sp, #TrPc]                     ; re-fetch target PC/CPSR
        b       %B6

#endif

        MEND

;++
;
; Routine Description:
;
;   This routine is the common case path for restoring the trap frame.
;
; Arguments:
;
;   sp - implicit trap frame
;
; Disposition:
;
;   Returns to the target mode.
;
;--

        ALIGN 32
        NESTED_ENTRY KiRestoreFromTrapFrame
        PROLOG_PUSH_TRAP_FRAME                          ; we have a trap frame under us whenever we make calls

        RESTORE_FROM_TRAP_FRAME

        NESTED_END KiRestoreFromTrapFrame


        SUBT "Reset Exception"

;++
;
; Routine Description:
;
;   This routine processes a reset exception and just bugchecks.
;
; Arguments:
;
;   none
;
; Disposition:
;
;    Bugcheck
;
;--

        ALIGN 32
        NESTED_ENTRY KiResetException

;
; N.B. Reset exceptions come in via SVC mode. Because KiBuildTrapFrame
; assumes a separate stack, it is important to reserve MiniStackLength
; bytes space for scratch.
;

        PROLOG_PUSH_TRAP_FRAME                          ; inform the unwinder we have a trap frame
        sub     sp, sp, #MiniStackLength                ; reserve space for temporary storage
        srsia   sp, #CPSRM_SVC                          ; save state on SVC stack
        BUILD_TRAP_FRAME 0, 0, 0, KEXCEPTION_ACTIVE_INTERRUPT_FRAME, TRAP_TYPE_RESET

        movs    r0, #INTERRUPT_EXCEPTION_NOT_HANDLED    ; specify the bugcheck number
        movs    r1, #0                                  ; clear remaining parameters
        movs    r2, #0                                  ;
        movs    r3, #0                                  ;
        mov     r12, r1                                 ;
        bl      KiBugCheckDispatch                      ; generate the bugcheck
        EMIT_BREAKPOINT                                 ; only here for cleaner unwinding

        NESTED_END KiResetException

        SUBT "Undefined Instruction Exceptions"

;++
;
; Routine Description:
;
;   This exception routine handles undefined instructions which on ARM
;   includes breakpoints, asserts and debug services.
;
;   Several special case fast paths are checked for and handled first.
;
;   If the source of the trap does not fall into any of these special
;   categories, a trap frame is generated and then the exception is
;   forwarded to the exception dispatcher for processing in higher
;   level code.
;
; Arguments:
;
;   None.
;
; Return Value:
;
;    There is no return from this function directly.
;
;--

        ALIGN 32
        NESTED_ENTRY KiUndefinedInstructionException

;
; N.B. Undefined instruction exceptions come in via UDF mode.
;

        PROLOG_PUSH_TRAP_FRAME                          ; inform the unwinder we have a trap frame
        strd    r6, r7, [sp, #MsR6]                     ; temporarily save r6-r7 @ exception:[sp+16]

#if DBG_TLB
        CP_WRITE lr, CP15_ATS1CPR                       ; ask for fault address translation
#endif

;
; Before creating the trap frame, fetch the opcode and look
; for special cases:
;
;   Fast Path #1: __rdpmccntr64
;
;      +-------+-------+---------------+
;      |1 1 0 1|1 1 1 0|1 1 1 1 1 0 1 0|
;      +-------+-------+---------------+
;

KiUndefinedInstructionLoadFault1                        ; look for faults here
        ldrh    r7, [lr, #-2]                           ; fetch first 16 bits of instruction that got us here
        cmp     r7, #0xe800                             ; 16-bit instruction?
        eor     r6, r7, #(THUMB_READ_CYCLES:AND:0xff00) ; XOR the top half
        blo     %F0                                     ; if not, check for __rdpmccntr64
KiUndefinedInstructionLoadFault2                        ; look for faults here
        ldrh    r6, [lr]                                ; fetch second 16 bits
        orr     r7, r6, r7, lsl #16                     ; combine the two halves
        b       %F1                                     ; go to general case
0
        cmp     r6, #(THUMB_READ_CYCLES:AND:0xff)       ; compare the bottom half
        bne     %F1                                     ; if we match, read cycles and return
        READ_CYCLE_COUNTER_64BIT r0, r1, r2, r3         ; read the cycle counter in r0/r1, trash r2/r3
        ldrd    r6, r7, [sp, #MsR6]                     ; restore r6-r7
        subs    pc, lr, #0                              ; return to the caller: -2 for offset, +2 to skip opcode
1

;
;   Fast Path #2: SDIV/UDIV
;
;      +---------+-----------+-+-------+ +-------+-------+-------+-------+
;      |1 1 1 1 1|0 1 1 1 0 S|1|   Rn  | |1 1 1 1|   Rd  |1 1 1 1|   Rm  | SDIV/UDIV (diffentiated by S bit)
;      +---------+-----------+-+-------+ +-------+-------+-------+-------+
;

        ubfx    r6, r7, #24, #8                         ; get top byte in r6
        cmp     r6, #0xfb                               ; do we match?
        bne     %F2                                     ; if not, skip
        ubfx    r6, r7, #20, #4                         ; get next 4 bits
        bic     r6, r6, #2                              ; clear the "don't care" bit
        cmp     r6, #9                                  ; do we match?
        bne     %F2                                     ; if not, skip
        and     r6, r7, r7, lsr #8                      ; and together r7 and r7 >> 8
        and     r6, r6, #0xf0                           ; keep only the 0xf0 bits
        cmp     r6, #0xf0                               ; are they all set?
        beq.w   KiEmulateDivide                         ; if so, emulate the division
2

;
;   Fast Path #3: Floating point/NEON. Either the VFP is disabled or we
;      took a VFP exception (which is disabled and ignored)
;
;      +-----+-+-----+---------+-------+ +-------+-----+-----------------+
;      |1 1 1|0|1 1 0|  Opcode |   Rn  | |       |1 0 1|                 | Load/store
;      +-----+-+-----+---------+-------+ +-------+-----+-----------------+
;      +-----+-+-------+-----+---------+ +-------+-----+-+-------+-------+
;      |1 1 1|0|1 1 0 0|0 1 0|         | |       |1 0 1|C|   op  |       | 64-bit register transfer
;      +-----+-+-------+-----+---------+ +-------+-----+-+-------+-------+
;      +-----+-+-------+-------+-------+ +-------+-----+-+---+-+-+-------+
;      |1 1 1|0|1 1 1 0|  opc1 |  opc2 | |       |1 0 1| |op3| |0|  opc4 | VFP data-processing
;      +-----+-+-------+-------+-------+ +-------+-----+-+---+-+-+-------+
;      +-----+-+-------+-----+-+-------+ +-------+-----+-+-+---+-+-------+
;      |1 1 1|0|1 1 1 0|  A  |L|       | |       |1 0 1|C| | B |1|       | Register transfer
;      +-----+-+-------+-----+-+-------+ +-------+-----+-+-+---+-+-------+
;      +-----+-+-------+---------+-----+ +-------+-------+-------+-------+
;      |1 1 1|U|1 1 1 1|    A    |     | |       |   B   |   C   |       | SIMD data-processing
;      +-----+-+-------+---------+-----+ +-------+-------+-------+-------+
;      +-------+-------+-+-+-+-+-------+ +-------+-------+---------------+
;      |1 1 1 1|1 0 0 1|A| |L|0|       | |       |   B   |               | SIMD Element load/store
;      +-------+-------+-+-+-+-+-------+ +-------+-------+---------------+
;
;      N.B. This is a "sloppy" partial decode that could potentially cause
;           VFP/NEON to be enabled for some other undefined instruction with
;           a similar bit signature. But this is both highly unlikely and
;           benign, so the decode is kept simple and fast.
;

        and     r6, r7, #0xe8000000                     ; copy and mask top byte
        cmp     r6, #0xe8000000                         ; do we match the signatures above?
        bne     %F3                                     ; if remaining bits non-zero, we're not a VFP/NEON instruction
#if (ARM_VFP_MANAGEMENT != ARM_VFP_ALWAYSON)
        CP_READ r6, CP15_CPACR                          ; get the CPACR register
        tst     r6, #CP15_CPACR_VFP_MASK                ; is access enabled?
        beq.w   KiEnableVfp                             ; if not, enable VFP on this thread
#endif
        vmrs    r6, fpexc                               ; read the FPEXC
        tst     r6, #(CPVFP_FPEXC_EX | CPVFP_FPEXC_DEX) ; is a VFP exception pending (EX=async, DEX=sync)?
        beq     %F3                                     ; if not, skip
        bic     r6, r6, #(CPVFP_FPEXC_EX | CPVFP_FPEXC_DEX) ; clear FPEXC.EX/FPEXC.DEX
        vmsr    fpexc, r6                               ; store it
        vmrs    r7, fpscr                               ; read FPSCR
        bic     r7, r7, #0xff00                         ; disable all exceptions
        vmsr    fpscr, r7                               ; write it back
        ldrd    r6, r7, [sp, #MsR6]                     ; recover r6-r7
        subs    pc, lr, #2                              ; re-execute the instruction
3

;
; See the comment in KiPreprocessInternalInvalidOpcode to understand why this
; code captures the faulting instruction bytes.
;

        str     r7, [sp, #MsReserved]                   ; save the faulting instruction

#if DBG
        CP_READ r6, CP15_CPACR                          ; get the CPACR register
        tst     r6, #CP15_CPACR_VFP_MASK                ; is access enabled?
        beq.w   %F1                                     ; if not, enable VFP on this thread
        vmrs    r6, fpexc                               ; read the FPEXC
        tst     r6, #CPVFP_FPEXC_EN                     ; is FPEXC.EN on?
        bne.w   %F1                                     ; if so, good
        mov     r6, #CPVFP_FPEXC_EN                     ; get FPEXC.EN
        vmsr    fpexc, r6                               ; turn it on
        ldrd    r6, r7, [sp, #MsR6]                     ; recover r6-r4
        srsia   sp, #CPSRM_UDF                          ; save state on UDF mini-stack
        BUILD_TRAP_FRAME KiUndefinedExceptionFaultAddrV, 2, 4, KEXCEPTION_ACTIVE_EXCEPTION_FRAME, TRAP_TYPE_UNDEFINED ; go to common code to build the rest
        __debugbreak                                    ; if not, fail now, this should never happen
1
#endif

;
; Resume with normal trap frame handling
;

        ldrd    r6, r7, [sp, #MsR6]                     ; recover r6-r7
        srsia   sp, #CPSRM_UDF                          ; save state on UDF mini-stack
        BUILD_TRAP_FRAME KiUndefinedExceptionFaultAddr, 2, 4, KEXCEPTION_ACTIVE_EXCEPTION_FRAME, TRAP_TYPE_UNDEFINED

#if DBG_TLB
        CP_READ r0, CP15_PAR                            ; capture PAR result (not architecturally safe, but works on A9)
        str     r0, [sp, #TrBcr + 7*4]                  ; store PAR result
#endif


KiUI10  ldr     r12, [sp, #TrCpsr]                      ; get old cpsr
        ldr     r0, =KI_EXCEPTION_INVALID_OP            ; set exception code
        movs    r1, #1                                  ; set number of parameters
        ldr     r2, [sp, #TrPc]                         ; set exception address
        cps     #CPSRM_UDF                              ; switch to UDF mode
        ldr     r3, [sp, #MsReserved]                   ; get the faulting instruction bytes
        cps     #CPSRM_SVC                              ; back to SVC mode
        RESTORE_INTERRUPT_STATE r12                     ; enable interrupts if previously enab.
        CAPSTART KiUndefinedInstructionException, KiExceptionDispatch
        bl      KiExceptionDispatch                     ; dispatch exception - no return
        CAPEND KiUndefinedInstructionException

        EMIT_BREAKPOINT                                 ; only here for cleaner unwinding

;
; Handle a fault on the ldrh at the start of this trap handler
;

KiUndefinedInstructionLoadFault
        cps     #CPSRM_UDF                              ; switch back to UDF mode
        ldrd    r6, r7, [sp, #MsR6]                     ; recover r6-r7
        subs    pc, lr, #2                              ; re-execute the instruction

        NESTED_END KiUndefinedInstructionException

        SUBT "Prefetch Abort Exceptions"

;++
;
; Routine Description:
;
;   This routine is entered as the result of a prefetch or data abort
;   because of the following reasons:
;
;   1. The referenced page is not present.
;
;   2. The referenced page does not allow the requested access.
;
;   3. An unaligned access
;
;   4. An access bit fault
;
; Arguments:
;
;   A standard trap frame is pushed by software on the kernel stack.
;   The hardware fault status registers are interrogated to determine
;   whether this is a an alignment fault which is handled seperately.
;   Otherwse, the fault status is passed on to MM.
;
; Disposition:
;
;   A standard trap frame is constructed on the kernel stack and memory
;   management is called to resolve the page fault. If memory management
;   successfully resolves the page fault, then working set information is
;   recorded, owed breakpoints are inserted, and execution is continued.
;
;   If the page fault occurred at an IRQL greater than APC_LEVEL, then the
;   system is shut down via a call to bug check. Otherwise, an appropriate
;   exception is raised.
;--

KiPrefetchAbortStatusTable                              ; IFSR[10,3:0]
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 00000
        DCB     SWFS_ALIGN_FAULT | SWFS_EXECUTE         ; 00001 alignment
        DCB     SWFS_DEBUG_FAULT | SWFS_EXECUTE         ; 00010 debug XXX
        DCB     SWFS_PAGE_FAULT  | SWFS_EXECUTE         ; 00011 access
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 00100
        DCB     SWFS_PAGE_FAULT  | SWFS_EXECUTE         ; 00101 translation
        DCB     SWFS_PAGE_FAULT  | SWFS_EXECUTE         ; 00110 access
        DCB     SWFS_PAGE_FAULT  | SWFS_EXECUTE         ; 00111 translation
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 01000
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 01001 domain
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 01010
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 01011 domain
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 01100
        DCB     SWFS_PAGE_FAULT  | SWFS_EXECUTE         ; 01101 permission
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 01110
        DCB     SWFS_PAGE_FAULT  | SWFS_EXECUTE         ; 01111 permission
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 10000
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 10001
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 10010
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 10011
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 10100
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 10101
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 10110
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 10111
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 11000
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 11001
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 11010
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 11011
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 11100
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 11101
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 11110
        DCB     SWFS_HWERR_FAULT | SWFS_EXECUTE         ; 11111

        ALIGN 32
        NESTED_ENTRY KiPrefetchAbortException

;
; N.B. Prefetch abort exceptions come in via ABT mode.
;

        PROLOG_PUSH_TRAP_FRAME                          ; inform the unwinder we have a trap frame
        srsia   sp, #CPSRM_ABT                          ; save state on ABT mini-stack

#if DBG_TLB
        CP_WRITE lr, CP15_ATS1CPR                       ; ask for fault address translation
#endif

KiPAE10
        BUILD_TRAP_FRAME KiPrefetchAbortExceptionFaultAddr, 4, 4, KEXCEPTION_ACTIVE_EXCEPTION_FRAME, TRAP_TYPE_PREFETCH_ABORT

#if DBG_TLB
        CP_READ r0, CP15_PAR                            ; capture PAR result (not architecturally safe, but works on A9)
        str     r0, [sp, #TrBcr + 7*4]                  ; store PAR result
#endif

        CP_READ r2, CP15_IFSR                           ; get fault status register in r2
        CP_READ r1, CP15_IFAR                           ; get fault address from IFAR in r1

        ldr     r12, [sp, #TrCpsr]                      ; re-enable interrupts if
        RESTORE_INTERRUPT_STATE r12                     ; previously enabled

        mov32   r3, KiPrefetchAbortStatusTable-1        ; minus 1 to account for thumbification of address
        lsr     r0, r2, #6                              ; shift high bit from 0x400 to 0x10
        bfi     r0, r2, #0, #4                          ; insert the low 4 bits
        and     r0, r0, #0x1f                           ; mask off extra bits
        ldrb    r0, [r3, r0]                            ; map hw fault status to sw
        b       KiCommonMemoryManagementAbort           ; handle the rest in common code

        NESTED_END KiPrefetchAbortException

        SUBT "Data Abort Exceptions"

;++
;
; Routine Description:
;
;   This routine is entered as the result of a data abort exception.
;   It retrieves the data abort specific status register and then enters
;   common code.
;
;--

KiDataAbortStatusTable                                  ; DFSR[10,3:0]
        DCB     SWFS_HWERR_FAULT                        ; 00000
        DCB     SWFS_ALIGN_FAULT                        ; 00001 align
        DCB     SWFS_DEBUG_FAULT                        ; 00010 debug XXX
        DCB     SWFS_PAGE_FAULT                         ; 00011 access
        DCB     SWFS_PAGE_FAULT                         ; 00100 icache
        DCB     SWFS_PAGE_FAULT                         ; 00101 translation
        DCB     SWFS_PAGE_FAULT                         ; 00110 access
        DCB     SWFS_PAGE_FAULT                         ; 00111 translation
        DCB     SWFS_HWERR_FAULT                        ; 01000
        DCB     SWFS_HWERR_FAULT                        ; 01001 domain
        DCB     SWFS_HWERR_FAULT                        ; 01010
        DCB     SWFS_HWERR_FAULT                        ; 01011 domain
        DCB     SWFS_HWERR_FAULT                        ; 01100
        DCB     SWFS_PAGE_FAULT                         ; 01101 permission
        DCB     SWFS_HWERR_FAULT                        ; 01110
        DCB     SWFS_PAGE_FAULT                         ; 01111 permission
        DCB     SWFS_HWERR_FAULT                        ; 10000
        DCB     SWFS_HWERR_FAULT                        ; 10001
        DCB     SWFS_HWERR_FAULT                        ; 10010
        DCB     SWFS_HWERR_FAULT                        ; 10011
        DCB     SWFS_HWERR_FAULT                        ; 10100
        DCB     SWFS_HWERR_FAULT                        ; 10101
        DCB     SWFS_HWERR_FAULT                        ; 10110
        DCB     SWFS_HWERR_FAULT                        ; 10111
        DCB     SWFS_HWERR_FAULT                        ; 11000
        DCB     SWFS_HWERR_FAULT                        ; 11001
        DCB     SWFS_HWERR_FAULT                        ; 11010
        DCB     SWFS_HWERR_FAULT                        ; 11011
        DCB     SWFS_HWERR_FAULT                        ; 11100
        DCB     SWFS_HWERR_FAULT                        ; 11101
        DCB     SWFS_HWERR_FAULT                        ; 11110
        DCB     SWFS_HWERR_FAULT                        ; 11111

        ALIGN 32
        NESTED_ENTRY KiDataAbortException

;
; N.B. Data abort exceptions come in via ABT mode.
;

        PROLOG_PUSH_TRAP_FRAME                          ; inform the unwinder we have a trap frame
        strd    r6, r7, [sp, #MsR6]                     ; temporarily save r6/r7 @ exception:[sp+16]

#if DBG_TLB
        CP_WRITE lr, CP15_ATS1CPR                       ; ask for fault address translation
#endif

;
; Before creating the trap frame, look for special cases. All the
; special fault addresses are within this module, so check first
; against the module bounds, and then branch out-of-line if the
; fault occurred there.
;

        mov32   r6, KiTrapModuleStart + 8 - 1           ; get start of module, plus 8 for data abort, minus 1 to de-thumbify
        subs    r6, lr, r6                              ; subtract from LR
        cmp.w   r6, #(((KiTrapModuleEnd - KiTrapModuleStart + 63) / 64) * 64) ; within range?
        blo.w   KiCheckSpecialFaultAddresses
KiNotSpecialFaultAddress

;
; Look for Alignment faults that we can emulate
;

        srsia   sp, #CPSRM_ABT                          ; save state on ABT mini-stack
                                                        ;   (must be *after* KiCheckSpecialFaultAddresses above)
        CP_READ r6, CP15_DFSR                           ; read fault status register
        ubfx    r6, r6, #0, #11                         ; extract the low 11 bits
        bic     r6, r6, #0x3f0                          ; clear out the irrelevant middle bits
        cmp     r6, #1                                  ; alignment fault?
        beq.w   KiCheckEmulatedAlignmentFault           ; if so, jump ahead
KiNotEmulatedAlignmentFault

;
; Resume with normal trap frame handling
;

        ldrd    r6, r7, [sp, #MsR6]                     ; restore r6/r7
KiEmulatedLoadStoreFaultResume
        BUILD_TRAP_FRAME KiDataAbortExceptionFaultAddr, 8, 8, KEXCEPTION_ACTIVE_EXCEPTION_FRAME, TRAP_TYPE_DATA_ABORT

#if DBG_TLB
        CP_READ r0, CP15_PAR                            ; capture PAR result (not architecturally safe, but works on A9)
        str     r0, [sp, #TrBcr + 7*4]                  ; store PAR result
#endif

        ldr     r0, [sp, #TrCpsr]                       ; get previous interrupt state
        CP_READ r2, CP15_DFSR                           ; get fault status register in r2
        CP_READ r1, CP15_DFAR                           ; get fault address register in r1
        RESTORE_INTERRUPT_STATE r0                      ; previously enabled

        mov32   r3, KiDataAbortStatusTable-1            ; minus 1 to account for thumbification of address
        lsr     r0, r2, #6                              ; shift high bit from 0x400 to 0x10
        bfi     r0, r2, #0, #4                          ; insert the low 4 bits
        and     r0, r0, #0x1f                           ; mask off extra bits
        ldrb    r0, [r3, r0]                            ; map hw fault status to sw

        and     r3, r2, #CP15_DFSR_WnR                  ; preserve write mask
        orr     r0, r0, r3, lsr #11                     ; merge with sw status

;
; Shared entry point for prefetch and data aborts
;
;    r0 - Hardware fault status
;    r1 - Fault adddress
;    r2 - Software fault status (mapped from hardware fault status)
;
;    Interrupts enabled (if they were previously)
;

        ALTERNATE_ENTRY KiCommonMemoryManagementAbort

        str     r1, [sp, #TrFaultAddress]               ; save fault address to frame
        and     r3, r0, #9                              ; only bits 0 and 3 can be passed to MM
        str     r3, [sp, #TrFaultStatus]                ; save fault status to trap frame

;
; ARM-specific: look for a faulting cache operation from kernel mode,
; and ignore if appropriate. The following opcodes are known as of
; this writing:
;
;    MRC   CP15_ICIMVAU,  Rt         ; 0xee07tf35
;    MRC   CP15_DCIMVAC,  Rt         ; 0xee07tf36
;    MRC   CP15_DCCMVAC,  Rt         ; 0xee07tf3a
;    MRC   CP15_DCCMVAU,  Rt         ; 0xee07tf3b
;    MRC   CP15_DCCIMVAC, Rt         ; 0xee07tf3e
;
; Since these are identical apart from the low nibble, just ignore that
; and look for the remaining bits.
;

        tst     r0, #SWFS_WRITE                         ; was this a write?
        beq     %F1                                     ; if not, can't be a cache operation

        mov32   r3, MmSystemRangeStart                  ; get the base address of kernel mode
        ldr     r3, [r3]                                ;
        ldr     r12, [sp, #TrPc]                        ; get address of faulting instruction
        cmp     r12, r3                                 ; did it come from kernel mode?
        blo     %F1                                     ; if not, process normally

        GET_IRQL r3                                     ; get current IRQL
        cmp     r3, #APC_LEVEL                          ; are we at APC_LEVEL or above?
        bls     %F1                                     ; if not, let the fault happen naturally

        ldrh    r3, [r12, #-1]                          ; fetch the 16-bit opcode
        eor     r3, r3, #0xee00                         ; match bits 8..15 via an XOR (makes them 0)
        cmp     r3, #0x0007                             ; check remaining bits
        bne     %F1                                     ; if not a match, can't be a cache operation

        ldrh    r3, [r12, #1]                           ; load the low 16 bits
        bic     r3, r3, #0xf000                         ; mask off Rt and low 4 bits (catch all clean/invalidate by MVA types)
        eor     r3, r3, #5
        cmp     r3, #0x0f30                             ; do the middle bits match?
        bne     %F1
2       add     r12, r12, #4                            ; advance past the opcode
        str     r12, [sp, #TrPc]                        ; update the PC
        b       KiPF60                                  ; and get out

1
        tst     r0, #SWFS_DEBUG_FAULT                   ; h/w [break/watch]point?
        bne     KiDebugFault
        tst     r0, #SWFS_PAGE_FAULT                    ; is this a common page fault?
        bne     KiCMMAPageFault
        tst     r0, #SWFS_ALIGN_FAULT                   ; is this an alignment fault?
        bne     KiCMMAAlignmentFault
                                                        ; anything else is a hardware error
        mov     r2, r1                                  ; set exception address (from r1)
        movs    r1, #0                                  ; set number of parameters
        ldr     r0, =KI_EXCEPTION_HARDWARE_ERROR        ; set exception code
        bl      KiExceptionDispatch                     ; dispatch exception, no ret

;
; Handle a fault on the ldrh at the start of this trap handler
;

KiMisalignedInstructionLoadFault
        ldr     r6, [sp, #MsPc]                         ; recover PC saved from original fault
        subs    r6, r6, #8                              ; point back to the original instruction
        str     r6, [sp, #MsPc]                         ; save it back
        ldrd    r6, r7, [sp, #MsReserved]               ; recover r6/r7 from the original fault
        rfeia   sp                                      ; return to the original instruction to re-execute

;
; If we hit a h/w breakpoint or watchpoint, raise an
; exception and let the debugger handle the fault. Else
; bugcheck.
;

KiDebugFault
        mov     r0, sp
        bl      KiDebugException                        ; returns Fault Address
        isb                                             ; for debugging disabled

        bl      KiGetDebugExceptionType                 ; returns exception type
        movs    r3, r0                                  ; exception param 1
        ldr     r2, [sp, #TrPc]                         ; fetch exception address
        beq     %F1

        movs    r1, #1                                  ; set number of parameters
        ldr     r0, =STATUS_BREAKPOINT                  ; set exception code

        cmp     r3, #BREAKPOINT_HW_ASYNCH_WATCH         ; watchpoint?
        beq     %F2                                     ; if so, process
        cmp     r3, #BREAKPOINT_HW_SYNCH_WATCH          ; watchpoint?
        bne     %F3                                     ; if not, skip
2
        movs    r1, #2                                  ; bump to two parameters
        CP_READ r12, CP14_DBGWFAR                       ; get fault address register in r12
3
        CAPSTART KiDebugFault, KiExceptionDispatch
        bl      KiExceptionDispatch                     ; generate exception - no return
        CAPEND  KiDebugFault

1       movs    r1, #0                                  ; set number of parameters
        ldr     r0, =KI_EXCEPTION_INVALID_OP            ; set exception code

        CAPSTART KiDebugFault, KiExceptionDispatch
        bl      KiExceptionDispatch                     ; generate exception - no return
        CAPEND  KiDebugFault

KiCMMAAlignmentFault
        mov     r2, r1                                  ; set exception address (from r1)
        movs    r1, #0                                  ; set number of parameters
        ldr     r0, =STATUS_DATATYPE_MISALIGNMENT       ; set exception code
        bl      KiExceptionDispatch                     ; dispatch exception, no ret

KiCMMAPageFault
        mov     r3, sp                                  ; set trap frame address
        ldr     r2, [sp, #TrCpsr]                       ; isolate mode
        tst     r2, #CPSRC_INT                          ; check previous interrupt status
        bne     KiPF50                                  ; if NE, interrupts were masked
        mvns    r2, r2                                  ;
        and     r2, r2, #1                              ;
        and     r0, #9                                  ; only bits 0 and 3 can be passed to MM
KiPF05
        CAPSTART KiCMMAPageFault, MmAccessFault
        bl      MmAccessFault                           ; attempt to resolve page fault
        CAPEND  KiCMMAPageFault
        cmp     r0, #0                                  ; test for successful completion
        blt     KiPF20                                  ; if l, not successful completion

;
; If watch working set is enabled, then record working set information.
;

        ldr     r1, =PsWatchEnabled                     ; check if working set watch enabled
        ldrb    r1, [r1]                                ;
        cbz     r1, KiPF10                              ; if e, working set watch not enabled
        ldr     r2, [sp, #TrFaultAddress]               ; set fault address
        ldr     r1, [sp, #TrPc]                         ; set exception address
        bl      PsWatchWorkingSet

;
; If the debugger has any breakpoints that should be inserted, then attempt
; to insert them now.
;

KiPF10  ldr     r0, =KdpOweBreakpoint                   ; check if breakpoints are owed
        ldrb    r0, [r0]                                ;
        cmp     r0, #0                                  ;
        beq     KiPF60                                  ; if e, no owed breakpoints
        CAPSTART KiCMMAPageFault, KdSetOwedBreakpoints
        ldr     r0, [sp, #TrFaultAddress]               ; set fault address
        bl      KdSetOwedBreakpoints                    ; notify the debugger of new page
        CAPEND  KiCMMAPageFault
        b       KiPF60

KiPF20

;
; Memory management failed to resolve the fault.
;
; STATUS_IN_PAGE_ERROR | 0x10000000 is a special status that indicates a
;       page fault at IRQL greater than APC level. This status causes a
;       bugcheck.
;
; The following status values can be raised:
;
; STATUS_ACCESS_VIOLATION
; STATUS_GUARD_PAGE_VIOLATION
; STATUS_STACK_OVERFLOW
;
; All other status values are sconverted to:
;
; STATUS_IN_PAGE_ERROR
;
;

KiPF25  movs    r1, #2                                  ; set number of parameters
        ldr     r2, =STATUS_IN_PAGE_ERROR | 0x10000000  ; check for bugcheck code
        cmp     r0, r2                                  ;
        beq     KiPF40                                  ; if e, bugcheck code returned
        ldr     r2, =STATUS_ACCESS_VIOLATION            ; check for status values
        cmp     r0, r2                                  ;
        beq     KiPF28                                  ; if e, raise exception with internal code
        ldr     r2, =STATUS_GUARD_PAGE_VIOLATION        ; check for status code
        cmp     r0, r2                                  ;
        beq     KiPF30                                  ; if e, raise exception with code
        ldr     r2, =STATUS_STACK_OVERFLOW              ; check for status code
        cmp     r0, r2                                  ;
        beq     KiPF30                                  ; if e, raise exception with code
        str     r0, [sp, #TrArg3]                       ; set parameter 3 to real status value
        ldr     r0, =STATUS_IN_PAGE_ERROR               ; convert all other status codes
        movs    r1, #3                                  ; set number of parameters
        b       KiPF30

KiPF28  ldr     r0, =KI_EXCEPTION_ACCESS_VIOLATION      ; set internal code

;
; Set virtual address, load/store and i/d indicators, exception address, and
; dispatch the exception.
;

KiPF30  ldr     r2, [sp, #TrPc]                         ; set exception address
        ldr     r3, [sp, #TrFaultStatus]                ; set load/store indicator to match
        ldr     r12, [sp, #TrFaultAddress]              ; set fault address
        CAPSTART KiPrefetchAbortException, KiExceptionDispatch
        bl      KiExceptionDispatch                     ; dispatch exception - no return
        CAPEND KiPrefetchAbortException

;
; A page fault occurred at an IRQL that was greater than APC_LEVEL. Set bug
; check parameters and join common code.
;

KiPF40
        GET_IRQL r0                                     ; get current IRQL

KiPF45
        ldr     r12, [sp, #TrPc]                        ; set parameter 5 to exception address
        ldr     r3, [sp, #TrFaultStatus]                ; set load/store indicator
        mov     r2, r0                                  ; set current IRQL
        ldr     r1, [sp, #TrFaultAddress]               ; set fault address
        movs    r0, #IRQL_NOT_LESS_OR_EQUAL             ; set bug check code
        bl      KiBugCheckDispatch                      ; bug check system - no return

;
; A page fault occurred with interrupts disabled.
;

KiPF50
        mov     r0, #0xFF                               ; set previous IRQL to 0xFF
        b       KiPF45                                  ; bugcheck

;
; ARM's Load/Store Exclusive feature replaces the need for SLIST sequence
; numbers (or CPUID) and procedure rewind on generic traps and interrupts,
; so there is no KiCheckForSListAddress here.
; Rewinds for #AV while accessing First->Next are still required and are
; performed by KiOpPreprocessAccessViolation.
;

KiPF60  bl      KiRestoreFromTrapFrame                  ; restore from the trap frame
        EMIT_BREAKPOINT                                 ; only here for cleaner unwinding

;
; Check for special faults at particular addresses within this module.
;

KiCheckSpecialFaultAddresses
        ;
        ; Undefined instruction exception faults
        ;
        subs    r6, r6, #(KiUndefinedInstructionLoadFault1 - KiTrapModuleStart) ; fault when fetching opcodes?
        beq.w   KiUndefinedInstructionLoadFault         ; if equal, handle
        subs    r6, r6, #(KiUndefinedInstructionLoadFault2 - KiUndefinedInstructionLoadFault1) ; fault when fetching opcodes?
        beq.w   KiUndefinedInstructionLoadFault         ; if equal, handle
        movs    r7, #2                                  ; PC offset is 2 if this was a panic

#if !DBG

        subs    r6, r6, #(KiUndefinedExceptionFaultAddr - KiUndefinedInstructionLoadFault2) ; fault in undefined exception handler?
        beq.w   KiDataAbortPanicBugcheck                ; if equal, panic

#else   // if DBG

        subs    r6, r6, #(KiUndefinedExceptionFaultAddrV- KiUndefinedInstructionLoadFault2) ; fault in vfp validation code?
        beq.w   KiDataAbortPanicBugcheck

        subs    r6, r6, #(KiUndefinedExceptionFaultAddr - KiUndefinedExceptionFaultAddrV) ; fault in undefined exception handler?
        beq.w   KiDataAbortPanicBugcheck                ; if equal, panic

#endif

        ;
        ; Prefetch abort exception faults
        ;
        movs    r7, #4                                  ; PC offset is 4 if this was a panic
        subs    r6, r6, #(KiPrefetchAbortExceptionFaultAddr - KiUndefinedExceptionFaultAddr) ; fault in prefetch abort exception handler?
        beq.w   KiDataAbortPanicBugcheck                ; if equal, panic

        ;
        ; Data abort exception faults
        ;
        movs    r7, #8                                  ; PC offset is 8 if this was a panic
        subs    r6, r6, #(KiDataAbortExceptionFaultAddr - KiPrefetchAbortExceptionFaultAddr) ; fault when fetching opcodes?
        beq.w   KiDataAbortPanicBugcheck                ; if equal, panic
        movw    r7, #(KiMisalignedInstructionLoadFault1 - KiDataAbortExceptionFaultAddr) ; fault when fetching opcodes?
        subs    r6, r6, r7                              ;
        beq.w   KiMisalignedInstructionLoadFault        ; if equal, handle
        subs    r6, r6, #(KiMisalignedInstructionLoadFault2 - KiMisalignedInstructionLoadFault1) ; fault when fetching opcodes?
        beq.w   KiMisalignedInstructionLoadFault        ; if equal, handle

        ;
        ; Interrupt/FIQ exception faults
        ;
        movw    r7, #(KiInterruptExceptionFaultAddr - KiMisalignedInstructionLoadFault2) ; fault in interrupt exception handler?
        subs    r6, r6, r7                              ;
        mov     r7, #4                                  ; PC offset is 4 if this was a panic
        beq.w   KiDataAbortPanicBugcheck                ; if equal, panic
        movw    r7, #(KiFIQExceptionFaultAddr - KiInterruptExceptionFaultAddr) ; fault in FIQ exception handler?
        subs    r6, r6, r7                              ;
        beq.w   KiDataAbortPanicBugcheck                ; if equal, panic

        ;
        ; Emulation faults
        ;
        subw    r6, r6, #(KiEmulatedLoadStoreStart - KiFIQExceptionFaultAddr) ; fault in emulated load store?
        cmp     r6, #(KiEmulatedLoadStoreEnd - KiEmulatedLoadStoreStart) ; are we within the code?
        blo.w   KiEmulatedLoadStoreFault                ; if so, handle it specially

        b       KiNotSpecialFaultAddress                ; go back; it wasn't anything we expected

;
; Check for for alignment faults that we can emulate.
;

KiCheckEmulatedAlignmentFault
        ldrd    r6, r7, [sp, #MsR6]                     ; reload r6/r7 we saved before
        strd    r6, r7, [sp, #MsReserved]               ; save an extra copy in case we fault here
KiMisalignedInstructionLoadFault1
        ldrh    r7, [lr, #-8]                           ; fetch first instruction halfword
        cmp     r7, #0xe800                             ; 16-bit instruction?
        blo     %F0                                     ; if so, skip
KiMisalignedInstructionLoadFault2
        ldrh    r6, [lr, #-6]                           ; fetch second instruction halfword
        orr     r7, r6, r7, lsl #16                     ; combine into one
0

;
;      +-------+-------+-+-+-+-+-------+ +-------+-------+---------------+
;      |1 1 1 0|1 1 0 1|U|D|0|1|   Rn  | |   Vd  |1 0 1 1|      imm8     | VLDR (d)
;      +-------+-------+-+-+-+-+-------+ +-------+-------+---------------+
;      |1 1 1 0|1 1 0 1|U|D|0|1|   Rn  | |   Vd  |1 0 1 0|      imm8     | VLDR (s)
;      +-------+-------+-+-+-+-+-------+ +-------+-------+---------------+
;      |1 1 1 0|1 1 0 1|U|D|0|0|   Rn  | |   Vd  |1 0 1 1|      imm8     | VSTR (d)
;      +-------+-------+-+-+-+-+-------+ +-------+-------+---------------+
;      |1 1 1 0|1 1 0 1|U|D|0|0|   Rn  | |   Vd  |1 0 1 0|      imm8     | VSTR (s)
;      +-------+-------+-+-+-+-+-------+ +-------+-------+---------------+
;

        ubfx    r6, r7, #24, #8                         ; get top byte
        cmp     r6, #0xed                               ; does it match VLDR/VSTR?
        bne     %F1                                     ; if not, skip
        ubfx    r6, r7, #9, #3                          ; get bits 9..11
        cmp     r6, #5                                  ; are they 101?
        bne     %F1                                     ; if not, skip
        tst     r7, #0x00200000                         ; check bit 21
        beq.w   KiEmulateLoadStoreVfp                   ; if 0, process as VFP load/store
1

;
;      +---------+---+-+-+-+-+-+-------+ +-------+-------+---------------+
;      |1 1 1 0 1|0 0|P|U|1|W|1|   Rn  | |   Rt  |  Rt2  |      imm8     | LDRD (imm)
;      +---------+---+-+-+-+-+-+-------+ +-------+-------+---------------+
;      |1 1 1 0 1|0 0|P|U|1|W|0|   Rn  | |   Rt  |  Rt2  |      imm8     | STRD (imm)
;      +---------+---+-+-+-+-+-+-------+ +-------+-------+---------------+
;
;      (P | W) != 0
;

        ubfx    r6, r7, #25, #7                         ; get top 7 bits
        cmp     r6, #0x74                               ; does it match LDRD/STRD?
        bne     %F2                                     ; if not, skip
        tst     r7, #0x00400000                         ; check bit 22
        beq     %f2                                     ; if 0, skip
        orr     r6, r7, r7, lsl #3                      ; or P and W together
        tst     r6, #0x01000000                         ; check the result
        bne.w   KiEmulateLoadStoreDouble                ; if non-zero, it is a proper LDRD/STRD
2

;
;      +-------+-+-----+---------------+
;      |1 1 0 0|1|  Rn |    reglist    | LDM <Rn>!, <reglist>
;      +-------+-+-----+---------------+
;      |1 1 0 0|0|  Rn |    reglist    | STM <Rn>!, <reglist>
;      +-------+-+-----+---------------+
;      +---------+---+-----+-+-+-------+ +-+-+-+-------------------------+
;      |1 1 1 0 1|0 0|0 1 0|W|1|   Rn  | |P|M|0|       reglist           | LDM.W <Rn>{!}, <reglist>
;      +---------+---+-----+-+-+-------+ +-+-+-+-------------------------+
;      |1 1 1 0 1|0 0|1 0 0|W|1|   Rn  | |P|M|0|       reglist           | LDMDB <Rn>{!}, <reglist>
;      +---------+---+-----+-+-+-------+ +-+-+-+-------------------------+
;      |1 1 1 0 1|0 0|0 1 0|W|0|   Rn  | |0|M|0|       reglist           | STM.W <Rn>{!}, <reglist>
;      +---------+---+-----+-+-+-------+ +-+-+-+-------------------------+
;      |1 1 1 0 1|0 0|1 0 0|W|0|   Rn  | |0|M|0|       reglist           | STMDB <Rn>{!}, <reglist>
;      +---------+---+-----+-+-+-------+ +-+-+-+-------------------------+
;

        ubfx    r6, r7, #12, #20                        ; get upper part of 16-bit opcode
        cmp     r6, #0xc                                ; does it match LDM/STM?
        beq.w   KiEmulateLoadStoreMultiple16            ; if so, hop to the emulator
        ubfx    r6, r7, #25, #7                         ; get top 7 bits
        cmp     r6, #0x74                               ; does it match LDM/STM?
        bne     %F3                                     ; if not, skip
        tst     r7, #0x00400000                         ; check bit 22
        bne     %F3                                     ; if non-zero, it's not LDM/STM
        eor     r6, r7, r7, lsl #1                      ; EOR r7 with (r7 << 1)
        tst     r6, #0x01000000                         ; ensure that bits 24 and 23 are opposites
        beq     %F3                                     ; if not, this isn't us
        ubfx    r6, r7, #16, #4                         ; extract base register
        cmp     r6, #13                                 ; sp?
        beq     %F3                                     ; if so, fail - don't want to hide misaligned stacks
        cmp     r6, #15                                 ; pc?
        bne.w   KiEmulateLoadStoreMultiple              ; if not, we can handle it
3
        b       KiNotEmulatedAlignmentFault             ; nope, can't handle it

        NESTED_END KiDataAbortException

;
; If we enter the data abort handler with the address of a trap frame
; builder fault address, then we ran out of kernel stack space. Switch to
; a 1-page panic stack and bugcheck the system.
;
; Here is the sequence of events that leads to this point:
;
;    1. Original fault occurs
;    2. Exception PC and SPSR are saved on the original fault's mini-stack
;    3. r4/r5 are saved on the original fault's mini-stack by BUILD_TRAP_FRAME
;    4. System is switched to SVC mode
;    5. SVC's SP is adjusted, and the top of the trap frame is touched
;    6. Secondary fault (data abort) occurs
;    7. Exception PC and SPSR and kept live in ABT.LR and ABT.SPSR
;    8. r6/r7 are saved on the ABT mini-stack
;    9. r7 is loaded with the PC offset of the original fault type
;   10. KiDataAbortPanicBugcheck is called
;
; Thus the current state is:
;   r0-r3  = valid from original fault
;   r4     = mini-stack pointer at time of original fault
;   r5     = starting SVC stack pointer at time of original fault
;   r6     = junk
;   r7     = offset to apply to PC
;   r8-r12 = valid from original fault
;   abt.sp = points to ABT mini-stack
;   abt.lr = address+8 of secondary data abort
;   svc.sp = points to overflow point at original fault time
;

        NESTED_ENTRY KiDataAbortPanicBugcheck
        PROLOG_PUSH_TRAP_FRAME

        str     r7, [sp, #MsReserved]                   ; save PC offset in ABT mini-stack's reserved section

        cps     #CPSRM_SVC                              ; switch to SVC mode
        PCR_READ r6                                     ; get the PCR
        movw    r7, #PcPrcb + PbPanicStackBase          ; get offset of panic stack base field
        ldr     sp, [r6, r7]                            ; point SVC SP to the panic stack base

        sub     sp, sp, #KTRAP_FRAME_LENGTH + KTRAP_FRAME_LENGTH + VFP_STATE_LENGTH ; allocate 2 trap frames and one VFP frame
        strd    r0, r1, [sp, #TrR0]                     ; save r0/r1 at secondary fault time
        strd    r2, r3, [sp, #TrR2]                     ; save r2/r3 at secondary fault time
        add     r6, sp, #KTRAP_FRAME_LENGTH             ; point r6 to the second trap frame
        strd    r12, r6, [sp, #TrR12]                   ; save r12/sp at secondary fault time
        add     r6, sp, #KTRAP_FRAME_LENGTH + TrR11     ; point to the r11/PC that will be stored in the reconstructed original frame
        strd    lr, r6, [sp, #TrLr]                     ; save lr/r11 at secondary fault time
        cps     #CPSRM_ABT                              ; back to ABT mode
        sub     r6, lr, #8                              ; compute PC - 8 to get actual exception PC
        orr     r6, r6, #1                              ; set thumb bit
        mrs     r7, spsr                                ; fetch SPSR
        cps     #CPSRM_SVC                              ; once again to SVC mode
        strd    r6, r7, [sp, #TrPc]                     ; store PC/SPSR
        movs    r6, #KEXCEPTION_ACTIVE_EXCEPTION_FRAME  ; write ExceptionActive field
        movs    r7, #0                                  ; default to NULL for VFP state
        strd    r6, r7, [sp, #TrExceptionActive]        ; and clear other byte fields

        add     sp, sp, #KTRAP_FRAME_LENGTH             ; point to where we will reconstruct the original frame
        strd    r0, r1, [sp, #TrR0]                     ; save r0/r1 at original fault time
        strd    r2, r3, [sp, #TrR2]                     ; save r2/r3 at original fault time
        strd    r12, r5, [sp, #TrR12]                   ; save r12/sp at original fault time
        strd    lr, r11, [sp, #TrLr]                    ; save lr/r11 at original fault time
        ldrd    r6, r7, [r4, #MsPc]                     ; fetch saved PC/SPSR from original mini-stack
        cps     #CPSRM_ABT                              ; back to ABT mode
        ldr     r0, [sp, #MsReserved]                   ; recover PC offset
        subs    r6, r6, r0                              ; adjust PC
        orr     r6, r6, #1                              ; set thumb bit
        cps     #CPSRM_SVC                              ; once again to SVC mode
        strd    r6, r7, [sp, #TrPc]                     ; store PC/SPSR
        movs    r6, #KEXCEPTION_ACTIVE_EXCEPTION_FRAME  ; write ExceptionActive field
        movs    r7, #0                                  ; default to NULL for VFP state
        strd    r6, r7, [sp, #TrExceptionActive]        ; and clear other byte fields

        CP_READ r6, CP15_CPACR                          ; fetch CPACR
        tst     r6, #CP15_CPACR_VFP_MASK                ; was FPU enabled?
        beq     %F1                                     ; if not, skip
        add     r6, sp, #KTRAP_FRAME_LENGTH             ; point to VFP state in r6
        str     r6, [sp, #TrVfpState]                   ; write the VFP state pointer
        SAVE_VFP_STATE r6, r7                           ; write out VFP state
1
        sub     sp, sp, #KTRAP_FRAME_LENGTH             ; point the stack back to the proper trap frame
        add     r11, sp, #TrR11                         ; point r11 to the frame chain
        cps     #CPSRM_ABT                              ; back to ABT mode
        ldrd    r6, r7, [sp, #MsR6]                     ; recover original r6/r7 from ABT mini-stack
        cps     #CPSRM_SVC                              ; once again to SVC mode
        ldrd    r4, r5, [r4, #MsR4]                     ; recover original r4/r5 from original fault's mini-stack

        movs    r0, #PANIC_STACK_SWITCH                 ; set bugcheck code
        mov     r1, sp                                  ; parameter 1 is the trap frame (on our panic stack)
        movs    r2, #0                                  ; clear other parameters
        movs    r3, #0                                  ;
        mov     r12, r2                                 ;
        bl      KiBugCheckDispatch                      ; will not return
        EMIT_BREAKPOINT                                 ; should never get here

        NESTED_END KiDataAbortException

        SUBT "Interrupts"

;++
;
; Routine Description:
;
;   This routine processes incoming interrupts which are a type of
;   exception on ARM processors.  A trap frame is generated on the SVC
;   mode stack, the interrupt(s) are processed, and then the frame is
;   restored.
;
; Arguments:
;
;   Trap Frame <implicit>
;
; Disposition:
;
;   return to previous mode.
;
;--

        ALIGN 32
        NESTED_ENTRY KiInterruptException

;
; N.B. Interrupt exceptions come in via INT mode.
;

        PROLOG_PUSH_TRAP_FRAME                          ; inform the unwinder we have a trap frame
        srsia   sp, #CPSRM_INT                          ; save state on INT mini-stack
        BUILD_TRAP_FRAME KiInterruptExceptionFaultAddr, 4, 4, KEXCEPTION_ACTIVE_INTERRUPT_FRAME, TRAP_TYPE_INTERRUPT

        dsb                                             ; barrier for consistency
        mov     r0, sp                                  ; pass trap frame
        bl      KiProcessInterrupt                      ; call the interrupt handlers
        bl      KiRestoreFromTrapFrame                  ; restore from the trap frame
        EMIT_BREAKPOINT                                 ; only here for cleaner unwinding

        NESTED_END KiInterruptException

        SUBT "Fast Interrupts"

;++
;
; Routine Description:
;
;   This routine processes FIQ-style interrupt exceptions which bank
;   additional registers.  This port does not enable FIQ-style interrupts and
;   masks them whenever it masks regular interrupts.
;
;   N.B. An alternate use of FIQ interrupts is as a NMI-equivalent.
;        This would require re-visiting every interrupt enable/disable
;        to ensure it left FIQ enabled.
;
; Arguments:
;
;   Trap Frame <implicit>
;
; Disposition:
;
;   return to previous mode.
;
;--

        ALIGN 32
        NESTED_ENTRY KiFIQException

;
; N.B. Fast interrupt exceptions come in via FIQ mode.
;

        PROLOG_PUSH_TRAP_FRAME                          ; inform the unwinder we have a trap frame
        srsia   sp, #CPSRM_FIQ                          ; save state on FIQ mini-stack
        BUILD_TRAP_FRAME KiFIQExceptionFaultAddr, 4, 4, KEXCEPTION_ACTIVE_INTERRUPT_FRAME, TRAP_TYPE_FIQ

        movs    r0, #INTERRUPT_EXCEPTION_NOT_HANDLED    ; specify the bugcheck number
        movs    r1, #0                                  ; clear remaining parameters
        movs    r2, #0                                  ;
        movs    r3, #0                                  ;
        mov     r12, r1                                 ;
        bl      KiBugCheckDispatch                      ; generate the bugcheck
        EMIT_BREAKPOINT                                 ; should never get here

        NESTED_END KiFIQException

        SUBT  "System Service Exception Handler"
;++
;
; EXCEPTION_DISPOSITION
; KiSystemServiceHandler (
;    IN PEXCEPTION_RECORD ExceptionRecord,
;    IN PVOID EstablisherFrame,
;    IN OUT PCONTEXT ContextRecord,
;    IN OUT PDISPATCHER_CONTEXT DispatcherContext
;    )
;
; Routine Description:
;
;   This routine is the exception handler for the system service dispatcher.
;
;   If an unwind is being performed and the system service dispatcher is
;   the target of the unwind, then an exception occured while attempting
;   to copy the user's in-memory argument list. Control is transfered to
;   the system service exit by return a continue execution disposition
;   value.
;
;   If an unwind is being performed and the previous mode is user, then
;   bug check is called to crash the system. It is not valid to unwind
;   out of a system service into user mode.
;
;   If an unwind is being performed and the previous mode is kernel, then
;   the previous mode field from the trap frame is restored to the thread
;   object.
;
;   If an exception is being raised and the exception PC is the address
;   of the system service dispatcher in-memory argument copy code, then an
;   unwind to the system service exit code is initiated.
;
;   If an exception is being raised and the exception PC is not within
;   the range of the system service dispatcher, and the previous mode is
;   not user, then a continue search disposition value is returned. Otherwise,
;   a system service has failed to handle an exception and bug check is
;   called. It is invalid for a system service not to handle all exceptions
;   that can be raised in the service.
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
;   If bug check is called, there is no return from this routine and the
;   system is crashed. If an exception occured while attempting to copy
;   the user in-memory argument list, then there is no return from this
;   routine, and unwind is called. Otherwise, ExceptionContinueSearch is
;   returned as the function value.
;
;--

        NESTED_ENTRY KiSystemServiceHandler

        PROLOG_PUSH         {r4-r5, r11, lr}            ; establish stack frame
        PROLOG_STACK_ALLOC  8                           ; stack space for RtlUnwindEx

        ldr     r12, [r0, #ErExceptionFlags]
        tst     r12, #EXCEPTION_UNWIND                  ; test for unwind
        bne     KiSH30                                  ; if nz, unwind in progress

;
; An exception is in progress.
;
; If the exception PC is the address of the GDI TEB access, then call unwind
; to transfer control to the system service exit code. Otherwise, check if
; the exception PC is the address of the in memory argument copy code for
; the system service dispatcher. If the exception PC is within the range of
; the in memory copy code, then call unwind to transfer control to the
; system service exit code. Otherwise, check if the previous mode is user
; or kernel mode.
;

        ldr     r4, [r0, #ErExceptionAddress]           ; get exception address
        ldr     r12, =KiSystemServiceGdiTebAccess       ; get GDI TEB access address
        cmp     r4, r12                                 ; check if address match
        beq     KiSH05                                  ; if match, jumpn
        ldr     r12, =KiSystemServiceCopyStart          ; get copy code start address
        cmp     r4, r12                                 ; check if address in range
        blo     KiSH10                                  ; if address not in range
        ldr     r12, =KiSystemServiceCopyEnd            ; get copy code end address
        cmp     r4, r12                                 ; check if address match
        bhs     KiSH10                                  ; if address not in range

;
; The exception was raised by the system service dispatcher GDI TEB access
; code or the argument copy code. Unwind to the system service exit with the
; exception status code as the return value.
;

KiSH05  mov     r12, #0                                 ; clear address of history table
        str     r12, [sp, #4]                           ;
        str     r2, [sp, #0]                            ; set address of context
        ldr     r3, [r0, #ErExceptionCode]              ; set return value
        mov     r2, r0                                  ; set address of exception record
        mov     r0, r1                                  ; set target frame address
        ldr     r1, =KiSystemServiceExit                ; set target IP address
        CAPSTART KiSystemServiceHandler, RtlUnwindEx
        bl      RtlUnwindEx                             ; unwind - no return
        CAPEND  KiSystemServiceHandler

;
; If the previous mode was kernel mode, then the continue the search for an
; exception handler. Otherwise, bug check the system.
;

KiSH10
        CURTHREAD_READ r4                               ; get current thread address
        ldrb    r4, [r4, #ThPreviousMode]               ; get previous mode
        cmp     r4, #KernelMode
        beq     KiSH20                                  ; if eq, previous mode kernel

;
; Previous mode is user mode - bug check the system.
;

        mov     r12, #0                                 ; zero parameter 5
        mov     r3, r2                                  ; set context record address
        mov     r2, r0                                  ; set exception record address
        ldr     r1, [r0, #ErExceptionCode]              ; set exception code
        movs    r0, #SYSTEM_SERVICE_EXCEPTION           ; set bug check code
        bl      KiBugCheckDispatch                      ; bug check system - no return

;
; Previous mode is kernel mode - continue search for a handler.
;

KiSH20  movs    r0, #ExceptionContinueSearch            ; set return value
        EPILOG_STACK_FREE   8                           ; deallocate stack frame
        EPILOG_POP          {r4-r5, r11, pc}            ; return

;
; An unwind is in progress.
;
; If a target unwind is being performed, then continue the unwind operation.
; Otherwise, check if the previous mode is user or kernel mode.
;

KiSH30  tst     r12, #EXCEPTION_TARGET_UNWIND           ; test for target unwind
        bne     KiSH20                                  ; if nz, target unwind in progress

;
; If the previous mode was kernel mode, then restore the previous mode and
; continue the unwind operation. Otherwise, bug check the system.
;

        CURTHREAD_READ r4                               ; get current thread address
        ldrb    r12, [r4, #ThPreviousMode]              ; get previous mode
        cmp     r12, #KernelMode                        ; check for kernel mode
        beq     KiSH40                                  ; if e, previous mode kernel

;
; Previous mode was user mode - bug check the system.
;

        movs    r0, #SYSTEM_UNWIND_PREVIOUS_USER        ; set bug check code
        bl      KiBugCheckDispatch                      ; bug check - no return

;
; Previous mode is kernel mode - restore previous mode and continue unwind
; operation.
;

KiSH40  ldr     r0, [r4, #ThTrapFrame]                  ; get current trap frame
        ldr     r1, [r0, #TrTrapFrame]                  ; restore previous trap frame address
        str     r1, [r4, #ThTrapFrame]                  ;
        ldrb    r1, [r0, #TrPreviousMode]               ; restore previous mode
        strb    r1, [r4, #ThPreviousMode]               ;
        b       KiSH20                                  ; finish in common code

        NESTED_END KiSystemServiceHandler

        SUBT  "System Service Internal"

;++
;
; VOID
; KiServiceInternal (
;     VOID
;     )
;
; Routine Description:
;
;   This function is called to provide the linkage between an internally
;   called system service and the system service dispatcher.
;
;   N.B. It is known that the previous mode was kernel
;
; Arguments:
;
;   R12  - Supplies the system service number.
;
;   r0-r3 - Supply the service register arguments.
;
;   Sp - May supply additional arguments based on the requirements of the
;       system call.
;
; Return value:
;
;   None.
;
;--

        ALIGN 32
        NESTED_ENTRY KiServiceInternal

        PROLOG_STACK_ALLOC  4                           ; reserve space for CPSR
        PROLOG_PUSH         {r11, lr}                   ; save return PC and r11 (also creates frame chain)
        PROLOG_STACK_ALLOC  8                           ; reserve space for SP/LR
        PROLOG_PUSH         {r0-r3, r12}                ; spill non-volatile registers
        PROLOG_STACK_ALLOC  TrR0                        ; reserve full trap frame, plus space for parameters

        READ_CPSR r0                                    ; get CPSR
        add     r1, sp, #KTRAP_FRAME_LENGTH             ; get original SP in r1
        movs    r2, #0                                  ; get a 0 in r2
        str     r0, [sp, #TrCpsr]                       ; save CPSR to trap frame
        str     r1, [sp, #TrSp]                         ; save original stack pointer as well
        str     r2, [sp, #TrVfpState]                   ; NULL out the VFP state

        cpsie   i                                       ; enable interrupts
        mov     r0, r12                                 ; copy service number to r0
        CURTHREAD_READ r12                              ; get current thread address
        ldrb    r1, [r12, #ThPreviousMode]              ; read thread's previous mode
        ldr     r2, [r12, #ThTrapFrame]                 ; read thread's trap frame
        movs    r3, #KernelMode                         ; set up to change mode to kernel
        strb    r1, [sp, #TrPreviousMode]               ; save thread's previous mode
        str     r2, [sp, #TrTrapFrame]                  ; save thread's trap frame
        strb    r3, [r12, #ThPreviousMode]              ; set thread's previous mode to kernel

        bl      KiSystemService                         ; execute the service

        CURTHREAD_READ r1                               ; get current thread address
        ldr     r3, [sp, #TrTrapFrame]                  ; restore previous trap frame addr
        ldrb    r2, [sp, #TrPreviousMode]               ; restore previous mode
        str     r3, [r1, #ThTrapFrame]                  ;
        strb    r2, [r1, #ThPreviousMode]               ;

        ldrd    r11, lr, [sp, #TrR11]                   ; restore r11 and target PC
        EPILOG_STACK_FREE KTRAP_FRAME_LENGTH            ; deallocate stack
        EPILOG_RETURN                                   ; return

        NESTED_END  KiServiceInternal

        SUBT  "System Service Call"

;++
;
; Routine Description:
;
;   This routine dispatches the exception caused by the SWI
;   instruction.  This instruction may be invoked only for system call
;   dispatch.
;
;   The system call dispatch is expected to only take place from user
;   mode. The specified system service is executed by locating its
;   routine address in system service dispatch table and calling the
;   specified function.
;
; Arguments:
;
;   r0-r3 - Supplies the first four system call arguments.
;
;   r12 - Supplies the system service call ordinal.
;
;   sp+ - Supplies any stack arguments
;
; Return Value:
;
;   r0 - System service status code.
;
;--

        ALIGN 32
        NESTED_ENTRY KiSWIException,,KiSystemServiceHandler

;
; N.B. SWI exceptions come in via SVC mode, meaning we don't need to
; switch modes here.
;
; N.B. SWI exceptions are only ever generated from user mode; thus
; there is no need to check for misaligned stacks, and no risk of
; stack overflows.
;

        PROLOG_PUSH_TRAP_FRAME                          ; inform the unwinder we have a trap frame
        sub     sp, sp, #KTRAP_FRAME_LENGTH + VFP_STATE_LENGTH + ARM_RED_ZONE_BYTES ; allocate trap frame + red-zone

;
; Save volatile integer registers, plus r11/lr to form a frame chain
; link. Also initialize the ExceptionActive and Reserved fields.
;

        strd    r0, r1, [sp, #TrR0]                     ; save r0/r1
        strd    r2, r3, [sp, #TrR2]                     ; save r2/r3
        str     r12, [sp, #TrR12]                       ; save r12
        str     r11, [sp, #TrR11]                       ; save r11
        mrs     r0, spsr                                ; retrieve SPSR
        tst     r0, #CPSRC_THUMB                        ; check if thumb mode
        beq     %F1                                     ; if not, skip
        orr     lr, lr, #1                              ; set the thumb bit in the return PC
1

        PCR_READ r12                                    ; get PCR
        ldr     r12, [r12, #PcCurrentThread]            ; get current thread
        CURTHREAD_WRITE_PASSIVE r12                     ; reload it back into TPIDRURO
        strd    lr, r0, [sp, #TrPc]                     ; save PC/SPSR
        ldrb    r1, [r12, #ThDebugActive]               ; get debug flags
        movs    r3, #KEXCEPTION_ACTIVE_SERVICE_FRAME    ; get ExceptionActive value
        str     r3, [sp, #TrExceptionActive]            ; write ExceptionActive, clearing other byte fields
        add     r11, sp, #TrR11                         ; point r11 to the frame chain

;
; Read the debug registers if appropriate.
;

        tst     r1, #DEBUG_ACTIVE_DBG                   ; test for debugger flag
        bne     KiSS85                                  ; if debugging going on, read debug regs
KiSS05

;
; In all modes, save the FPU state as needed.
;

#if (ARM_VFP_MANAGEMENT == ARM_VFP_ALWAYSON)
        add     r0, sp, #KTRAP_FRAME_LENGTH             ; point to the VFP state
        str     r0, [sp, #TrVfpState]                   ; store the VFP state pointer
        SAVE_VFP_STATE r0, r1                           ; write the VFP state (leaves FPSCR in r1)
        bic     r1, r1, #FPSCRM_DEPRECATED              ; clear deprecated bits in FPSCR
        vmsr    fpscr, r1                               ; update the value
#else
        CP_READ r0, CP15_CPACR                          ; read CPACR
        movs    r1, #0                                  ; default to a NULL pointer
        tst     r0, #CP15_CPACR_VFP_MASK                ; is the VFP enabled?
        beq     %F2                                     ; if not, skip and store the NULL
        add     r3, sp, #KTRAP_FRAME_LENGTH + VsVfpD + 8*8 ; point to where d8-d15 live
        vmrs    r2, fpscr                               ; get FPSCR
        add     r1, sp, #KTRAP_FRAME_LENGTH             ; VFP state is at the end of the trap frame
        str     r2, [sp, #KTRAP_FRAME_LENGTH + VsFpscr] ; save it
        vstm    r3, {d8-d15}                            ; store d8-d15
        bic     r2, r2, #FPSCRM_DEPRECATED              ; clear deprecated bits in FPSCR
        vmsr    fpscr, r2                               ; update the value
2       str     r1, [sp, #TrVfpState]                   ; store the VFP state pointer
#if ARM_VFP_ENABLE_STATISTICS
        mov32   r1, KiArmVfpStats + VtTrapsEnabled      ; get pointer to enabled stat
        tst     r0, #CP15_CPACR_VFP_MASK                ; is the VFP enabled?
        addeq   r1, r1, #8                              ; if not, point to traps disabled stat
        INCREMENT_STAT r1, r0, r2, r3                   ; increment the statistic
#endif
#endif

;
; Finish configuring the trapframe
;

        mov     r2, sp                                  ; point r2 to our stack
        cps     #CPSRM_SYS                              ; go to SYS mode to get user state
        str     sp, [r2, #TrSp]                         ; store user mode SP
        str     lr, [r2, #TrLr]                         ; store user mode LR
        cpsie   i, #CPSRM_SVC                           ; back to SVC mode and enable interrupts

;
; Dispatch to the pico provider if necessary.
;

        ldrb    r1, [r12, #ThDebugActive]              ; get debug flags
        tst     r1, #DEBUG_ACTIVE_MINIMAL_THREAD       ; test for minimal flag
        bne     KiSS80                                 ; if minimal, dispatch to pico provider

        ldr     r0, [sp, #TrR12]                        ; get system call number
        ldr     r1, [sp, #TrR0]                         ; get first arg
        strd    r0, r1, [r12, #ThSystemCallNumber]      ; save system call number and first arg

        LOG_MINI_TRAP_FRAME TRAP_TYPE_SYSCALL           ; log the trap frame
        CURTHREAD_READ_PASSIVE r12                      ; recover thread pointer in r12
        ldr     r0, [r12, #ThSystemCallNumber]          ; recover system call number in r0

#if DBG
        ldrb    r3, [r12, #ThPreviousMode]              ; check previous mode is user
        cmp     r3, #UserMode
        beq     %F1
        EMIT_BREAKPOINT
1
#endif

        bl      KiSystemService                         ; call the generic code
        b       KiSystemServiceExit                     ; skip over out-of-line code

;
; Out-of-line helpers for the entry path
;

KiSS80  mov     r0, sp                                  ; r0 = trapframe
        bl      PsPicoSystemCallDispatch                ; dispatch system call to pico provider
        b       KiSystemServiceExit                     ; exit the system service

KiSS85  mov     r0, sp                                  ; r0 = trapframe
        bl      KiReadDebugRegs                         ; fetch the debug registers
        CURTHREAD_READ_PASSIVE r12                      ; recover thread pointer in r12
        b       KiSS05                                  ; return

;
; An invalid state was detected on return from a system service. The cause is
; one of:
;
;  IRQL not equal to zero
;  Kernel APCs are disabled
;  Attached to another process
;
; N.B. This code is out-of-place on ARM, as it is more logical to handle it
; here in the user mode return path.
;

KiSS70  movs    r0, #IRQL_GT_ZERO_AT_SYSTEM_SERVICE     ; set bug check code
        movs    r3, #0                                  ; clear parameter value
        cbnz    r2, KiSS75                              ; check if IRQL is passive level; if ne, IRQL not passive level
        movs    r0, #APC_INDEX_MISMATCH                 ; set bug check code
        ldrb    r2, [r1, #ThApcStateIndex]              ; get APC state index
        ldr     r3, [r1, #ThCombinedApcDisable]         ; get kernel APC disable
KiSS75  ldr     r1, [sp, #TrPc]                         ; set system call address
        mov     r12, sp                                 ; set trap frame address
        bl      KiBugCheckDispatch                      ; bug check system - no return
        EMIT_BREAKPOINT                                 ; should never get here

;
; System service exit.
;
;   r0 - Supplies the system service status.
;
;   N.B. If you change the stack frame here, then make sure you fix
;   KiCallUserMode in callout.asm to match, since KiCallUserMode calls
;   KiSystemServiceExit.
;

        ALTERNATE_ENTRY KiSystemServiceExit

;
; If the current IRQL is above passive level, kernel APCs are disabled, or a
; process is attached, then bugcheck.
;
; N.B. This check is in a different location than on AMD64.
;

        str     r0, [sp, #TrR0]                         ; save return value

        GET_THREAD_AND_IRQL r1, r2                      ; get thread and irql
        ldrb    r3, [r1, #ThApcStateIndex]              ; get APC state index
        ldr     r0, [r1, #ThCombinedApcDisable]         ; get kernel APC disable
        orrs    r0, r0, r2                              ; OR with current IRQL
        orrs    r0, r0, r3                              ; and the APC state index as well
        bne     KiSS70                                  ; if nz, cause for bugcheck

        RESTORE_FROM_TRAP_FRAME SysCall

        EMIT_BREAKPOINT                                 ; only here for cleaner unwinding

        NESTED_END KiSWIException

;
; Dispatch system service.
;
;   r0-r3, r12 - Temporary registers.  The real values have already been
;                spilled.
;
;   r0 - Supplies the system service number.
;   r12 - Supplies the current thread address.
;   sp - Supplies the trap frame.
;   TrSp - Supplies additional arguments.
;

        ASSERT (SERVICE_NUMBER_MASK == 0xfff)
        ASSERT (SERVICE_TABLE_SHIFT == 12-4)
        ASSERT (SERVICE_TABLE_MASK == 0x10)

        ALIGN 32
        NESTED_ENTRY KiSystemService,,KiSystemServiceHandler

        PROLOG_NOP str sp, [r12, #ThTrapFrame]          ; set frame pointer address in thread
        PROLOG_PUSH {r11, lr}                           ; save return address and
        PROLOG_STACK_ALLOC KTRAP_FRAME_ARGUMENTS        ; allocate space for the arguments, plus alignment

        GBLA      TRAP_FRAME_OFFSET                     ; use TRAP_FRAME_OFFSET to get from SP to trap frame
TRAP_FRAME_OFFSET SETA KTRAP_FRAME_ARGUMENTS + 8        ; includes all space allocated/pushed above

;
; Repeat system service after attempt to convert to GUI thread.
;

        ubfx    r1, r0, #12, #1                         ; get service table number (0 or 1)
        bfc     r0, #12, #20                            ; get service table offset (and with SERVICE_NUMBER_MASK)

;
; If the specified system service number is not within range, then attempt
; to convert the thread to a GUI thread and retry the service dispatch.
;
; N.B. The system service table is encoded as 28 bits of offset and 4 bits of
;      in-memory argument number.
;

KiSystemServiceRepeat
        ldr     r2, [r12, #ThThreadFlags]               ; test if GUI thread
        mov32   r3, KeServiceDescriptorTable            ; get system service table address
        tst     r2, #KTHREAD_GUI_THREAD_MASK            ;
        beq     %F0
#if ENABLE_MULTIPLE_WIN32K
        mov32   r3, KeServiceDescriptorTableSession     ; get the per session service table address 
	ldr     r3, [r3]
#else
        mov32   r3, KeServiceDescriptorTableShadow      ; get shadow service table address 
        tst     r2, #KTHREAD_RESTRICTED_GUI_THREAD_MASK ; test if restricted GUI thread
        beq     %F0                                     ; branch if not a restricted GUI thread
        mov32   r3, KeServiceDescriptorTableFilter      ; get filter descriptor table
#endif

0       add     r3, r3, r1, lsl #4                      ; add in table number
        ldr     r2, [r3, #SdLimit]                      ; get service table limit
        ldr     r3, [r3, #SdBase]                       ; get service table address
        cmp     r0, r2                                  ; check if valid service
        bhs     KiSS50                                  ; if hs, not valid service
        ldr     r2, [r3, r0, lsl #2]                    ; get system service offset
        add     r3, r3, r2, asr #4                      ; add table base to service offset

;
; If the service is a GUI service and the GDI user batch queue is not empty,
; then call the appropriate service to flush the user batch.
;

        cbz     r1, KiSS10                              ; check if GUI service, skip if not
        ldr     r0, [r12, #ThTeb]                       ; get user TEB address
KiSystemServiceGdiTebAccess                             ; address needed for exception handling
        ldr     r0, [r0, #TeGdiBatchCount]              ; check batch queue empty
        cbz     r0, KiSS10                              ; if e, batch queue empty

        strd    r2, r3, [sp]                            ; save r2/r3
        movs    r0, #Win32BatchFlushCallout             ; operation is batch flush callout
        movs    r1, #0                                  ; no parameters
        movs    r2, #0                                  ; no session attach
        movs    r3, #0
        CAPSTART KiSystemService, PsInvokeWin32Callout
        bl      PsInvokeWin32Callout                    ; invoke the GDI flush callout
        CAPEND KiSystemService
        ldrd    r2, r3, [sp]                            ; restore r2/r3
        CURTHREAD_READ r12                              ; restore r12

;
; Check if system service has any in memory arguments.
;

KiSS10  ands    r2, r2, #15                             ; isolate number of in-memory arguments
        beq     KiSS30                                  ; if z, no in memory arguments
        ldr     r0, [sp, #TRAP_FRAME_OFFSET + TrCpsr]   ; check if previous mode user
        ldr     r1, [sp, #TRAP_FRAME_OFFSET + TrSp]     ; compute copy source address
        tst     r0, #0xf                                ;
        bne     KiSS20                                  ; if nz, previous mode kernel
        mov32   r0, MmUserProbeAddress                  ; check if source address in range
        ldr     r0, [r0]                                ;
        cmp     r1, r0                                  ;
        movhs   r1, r0                                  ; if ae, reset copy source address

;
; The following code is very carefully optimized so there is exactly 4 bytes
; of code for each argument move.
;
; N.B. Four additional arguments are specified in registers.
;

KiSS20  adr     r0, KiSystemServiceCopyEnd+1            ; get copy ending address
        sub     r2, r0, r2, lsl #2                      ; subtract number of bytes to copy
        bx      r2                                      ; do it

KiSystemServiceCopyStart                                ; address needed for exception handling
        ldr     r0, [r1, #52]                           ; copy fourteenth argument
        str     r0, [sp, #52]                           ;
        ldr     r0, [r1, #48]                           ; copy thirteenth argument
        str     r0, [sp, #48]                           ;
        ldr     r0, [r1, #44]                           ; copy twelveth argument
        str     r0, [sp, #44]                           ;
        ldr     r0, [r1, #40]                           ; copy eleventh argument
        str     r0, [sp, #40]                           ;
        ldr     r0, [r1, #36]                           ; copy tenth argument
        str     r0, [sp, #36]                           ;
        ldr     r0, [r1, #32]                           ; copy ninth argument
        str     r0, [sp, #32]                           ;
        ldr     r0, [r1, #28]                           ; copy eigth argument
        str     r0, [sp, #28]                           ;
        ldr     r0, [r1, #24]                           ; copy seventh argument
        str     r0, [sp, #24]                           ;
        ldr     r0, [r1, #20]                           ; copy sixth argument
        str     r0, [sp, #20]                           ;
        ldr     r0, [r1, #16]                           ; copy fifth argument
        str     r0, [sp, #16]                           ;
        ldr     r0, [r1, #12]                           ; copy fourth argument
        str     r0, [sp, #12]                           ;
        ldr     r0, [r1, #8]                            ; copy third argument
        str     r0, [sp, #8]                            ;
        ldr     r0, [r1, #4]                            ; copy second argument
        str     r0, [sp, #4]                            ;
        ldr     r0, [r1]                                ; copy first argument
        str     r0, [sp]                                ;
KiSystemServiceCopyEnd                                  ; address needed for exception handling

;
; Call system service
;
; N.B. On ARM, the location of the trap frame pointer in r12 is assumed by code in miscs.asm
;      and callout.asm, specifically NtContinue, NtRaiseException, and NtCallbackReturn.
;

KiSS30
        PCR_READ r1                                     ; get PCR
        mov32   r0, PerfGlobalGroupMask                 ; get global mask address
        ldr     r2, [r1, #PcSystemCalls]                ; get system call counter
        ldr     r0, [r0, #PERF_SYSCALL_OFFSET]          ; fetch syscall mask
        adds    r2, r2, #1                              ; increment system call count
        tst     r0, #PERF_SYSCALL_FLAG                  ; check syscall logging flag
        add     r12, sp, #TRAP_FRAME_OFFSET             ; save pointer to trap frame in r12
        mov     lr, r3                                  ; copy service address to lr
        str     r2, [r1, #PcSystemCalls]                ; store updated system call count
        bne.w   KiSS_LogSystemCall                      ; if ne, logging is enabled

KiSS33
        CAP_SYSTEMSERVICE
        ldrd    r0, r1, [sp, #TRAP_FRAME_OFFSET + TrR0] ; restore r0-r1
        ldrd    r2, r3, [sp, #TRAP_FRAME_OFFSET + TrR2] ; restore r2-r3
        blx     lr                                      ; call service
KiSS35

;
; System service exit. For ARM, this code has been moved to KiSWIException, above.
;
;       ALTERNATE_ENTRY KiSystemServiceExit
;

;
; For ARM, we return to the caller (either KiSWIException for user mode or
; KiServiceInternal for kernel mode), who performs any final checks and
; returns in the appropriate manner. Thus, don't be surprised that this code
; differs from AMD64.
;

KiSSExit
        EPILOG_STACK_FREE KTRAP_FRAME_ARGUMENTS         ; restore stack
        EPILOG_POP {r11, pc}                            ; restore and return

;
; The specified system service number is not within range. Attempt to convert
; the thread to a GUI thread if the specified system service is a GUI service
; and the thread has not already been converted to a GUI thread.
;

KiSS50  cbz     r1, KiSS60                              ; check if GUI service; if not, just error
        str     r0, [sp]                                ; save service number
        bl      PsConvertToGuiThread                    ; attempt to convert to GUI thread
        cmp     r0, #0                                  ; check if service was successful
        ldr     r0, [sp]                                ; restore service number
        mov     r1, #1                                  ; restore service table
        CURTHREAD_READ r12                              ; recover thread pointer
        beq.w   KiSystemServiceRepeat                   ; if z, successful convert to GUI

;
; The conversion to a GUI thread failed. The correct return value is encoded
; in a byte table indexed by the service number that is at the end of the
; service address table. The encoding is as follows:
;
;   0 - return 0.
;   -1 - return -1.
;   1 - return status code.
;

#if ENABLE_MULTIPLE_WIN32K
        mov32   r1, KeServiceDescriptorTableSession
        ldr     r1, [r1]
        add     r1, SERVICE_TABLE_TEST
#else
        mov32   r1, KeServiceDescriptorTableShadow + SERVICE_TABLE_TEST
#endif
        ldr     r2, [r1, #SdLimit]                      ; get service table limit
        ldr     r1, [r1, #SdBase]                       ; get service table base
        cmp     r0, r2                                  ; check if valid service number
        bhs     KiSS60                                  ; if ae, invalid service number
        add     r1, r1, r2, lsl #2                      ; get ending service table address
        ldrsb   r0, [r0, r1]                            ; get status byte value
        cmp     r0, #0                                  ; check for 0 or - 1
        ble     KiSSExit                                ; if le, return status byte value
KiSS60  ldr     r0, =STATUS_INVALID_SYSTEM_SERVICE      ; set return status
        b       KiSSExit                                ; finish in common code

;
; For ARM, the KiSS70 code is moved to KiSWIException, since it is user
; mode only.
;

;
; Log system call event.
;

KiSS_LogSystemCall
        str     lr, [sp, #TRAP_FRAME_OFFSET]            ; save service address
        mov     r0, lr                                  ; set address of system service
        bl      PerfInfoLogSysCallEntry                 ; log system call event
        ldr     lr, [sp, #TRAP_FRAME_OFFSET]            ; restore service address
        add     r12, sp, #TRAP_FRAME_OFFSET             ; save pointer to trap frame in r12

        CAP_SYSTEMSERVICE
        ldrd    r0, r1, [sp, #TRAP_FRAME_OFFSET + TrR0] ; restore r0-r1
        ldrd    r2, r3, [sp, #TRAP_FRAME_OFFSET + TrR2] ; restore r2-r3
        blx     lr                                      ; call service

        str     r0, [sp, #TRAP_FRAME_OFFSET + TrR0]     ; save system call return value
        bl      PerfInfoLogSysCallExit                  ; log system call return event
        ldr     r0, [sp, #TRAP_FRAME_OFFSET + TrR0]     ; restore system call return value
        b       KiSS35                                  ; finish in common code

;
; Service calls to NtContinue/NtRaiseException need a path to restore without
; scrubbing registers or checking for APCs.
;

        ALTERNATE_ENTRY KiExceptionRestoreFromService
        EPILOG_STACK_FREE KTRAP_FRAME_ARGUMENTS         ; restore allocated stack
        EPILOG_POP {r11}                                ; restore saved r11
        EPILOG_STACK_FREE 4                             ; ignore saved return address

        ALTERNATE_ENTRY KiExceptionRestore
        bl      KiRestoreFromTrapFrame                  ; restore from the trap frame
        EMIT_BREAKPOINT                                 ; only here for cleaner unwinding

        NESTED_END KiSystemService

        SUBT "MM Fault Exceptions"

;++
;
; Routine Description:
;
;   This routine allocates an exception frame on stack, saves nonvolatile
;   machine state, and calls the system bug check code.
;
;   N.B. It is the responsibility of the caller to initialize the exception
;        record.
;
; Arguments:
;
;   r0 - Supplies the bug check code.
;
;   r1-r3, r12 - Supplies the bug check parameters.
;
; Return Value:
;
;    There is no return from this function.
;
;--

        NESTED_ENTRY KiBugCheckDispatch

        GENERATE_EXCEPTION_FRAME                        ; generate exception frame

        str     r12, [sp, #0]                           ; save parameter on the stack
        bl      KeBugCheckEx                            ; bugcheck system - no return
1
        b       %B1                                     ; stick here just in case

        NESTED_END KiBugCheckDispatch

;++
;
; Routine Description:
;
;   This routine allocates an exception record and exception frame on
;   the stack, saves exception frame on stack, saves nonvolatile
;   state (which wasn't preserved by the trap frame), and calls the
;   system exception dispatcher for further processing of the exception.
;
;   If the exception dispatcher returns, then this routine will restore
;   the processor state from the exception and trap frames.
;
; Arguments:
;
;   r0 - Supplies the exception code.
;
;   r1 - Supplies the number of parameters to the exception.
;
;   r2 - Supplies a pointer to where the exception took place.
;
;   r3 - Supplies parameter 1 of the exception.
;
;   r12 - Supplies parameter 2 of the exception.
;
;   Arg3 <in trap frame> - Supplies parameter 3 of the exception.
;
; Return Value:
;
;    There is no return from this function.
;
;--

        NESTED_ENTRY KiExceptionDispatch

        PROLOG_STACK_ALLOC EXCEPTION_RECORD_LENGTH      ; alloc exception record
        GENERATE_EXCEPTION_FRAME                        ; generate exception frame


KiXD01  add     r4, sp, #KEXCEPTION_FRAME_LENGTH        ; get exception record address
        str     r0, [r4, #ErExceptionCode]              ; set exception code
        movs    r0, #0
        str     r0, [r4, #ErExceptionFlags]             ; clear exception flags
        str     r0, [r4, #ErExceptionRecord]            ; clear exception record address
        str     r2, [r4, #ErExceptionAddress]           ; set exception address
        str     r1, [r4, #ErNumberParameters]           ; set number of parameters
        str     r3, [r4, #ErExceptionInformation]       ; set exception parameters
        str     r12, [r4, #ErExceptionInformation+4]    ;
        add     r2, sp, #(KEXCEPTION_FRAME_LENGTH + EXCEPTION_RECORD_LENGTH) ; set trap frame address
        ldr     r0, [r2, #TrArg3]                       ;
        str     r0, [r4, #ErExceptionInformation+8]     ;
        ldr     r3, [r2, #TrCpsr]                       ; isolate previous mode
        mvns    r3, r3                                  ;
        and     r3, r3, #1                              ;
        movs    r0, #TRUE
        str     r0, [sp, #0]                            ; set first chance parameter
        mov     r1, sp                                  ; set exception frame address
        mov     r0, r4                                  ; set exception record address

        CAPSTART KiExceptionDispatch, KiDispatchException
        bl      KiDispatchException                     ; dispatch exception
        CAPEND  KiExceptionDispatch

        RESTORE_EXCEPTION_FRAME                         ; restore exception state
        EPILOG_STACK_FREE EXCEPTION_RECORD_LENGTH       ; free exception record

        bl      KiRestoreFromTrapFrame                  ; restore from the trap frame
        EMIT_BREAKPOINT                                 ; only here for cleaner unwinding

        NESTED_END KiExceptionDispatch

;++
;
; Routine Description:
;
;   Performs a stack switch to the ISR stack.
;
; Arguments:
;
;   r0   - TrapFrame
;   r1   - Vector
;   r2   - Irql
;   r3   - PreviousIrql
;   [sp] - IsrStack
;
; Return Value:
;
;   None
;
;--

        NESTED_ENTRY KxSwitchStackAndPlayInterrupt,, KiFatalExceptionHandler

        PROLOG_PUSH {r4-r5, r11, lr}
        PROLOG_STACK_SAVE r5                            ; save current SP to non-volatile
                                                        ; register
        ldr     r4,[r5,#0x10]                           ; retrieve IsrStack
        sub     r4,r4,#8                                ; reserve space for previous SP and align
        str     r5,[r4,#0x4]                            ; save previous stack value
        mov     sp, r4                                  ; switch stacks
        str     r5,[r4,#0x4]                            ; re-save previous SP in case a nested
                                                        ; interrupt occured
        bl      KiPlayInterrupt

        EPILOG_STACK_RESTORE r5                         ; go back to previous stack
        EPILOG_POP {r4-r5, r11, pc}

        NESTED_END KxSwitchStackAndPlayInterrupt

;++
;
; Routine Description:
;
;   Enable the VFP/NEON registers for the current thread.
;
; Arguments:
;
;   r6 - contains current CPACR value.
;
; Return Value:
;
;   None.
;
;--

#if (ARM_VFP_MANAGEMENT != ARM_VFP_ALWAYSON)
        LEAF_ENTRY KiEnableVfp

        orr     r6, r6, #CP15_CPACR_VFP_MASK            ; enable access to the FPU
        CP_WRITE r6, CP15_CPACR                         ; set the new CPACR
        isb                                             ; make it visible

        PCR_READ r6                                     ; get PCR
        ldr     r6, [r6, #PcCurrentThread]              ; get current thread

        movs    r7, #1                                  ; constant 1
        strb    r7, [r6, #ThNpxState]                   ; update the NPX state

        ldr     r6, [r6, #ThVfpState]                   ; get a pointer to the state area
        RESTORE_VFP_STATE r6, r7                        ; restore the VFP state and poison it

#if ARM_VFP_ENABLE_STATISTICS
        mov32   r6, KiArmVfpStats + VtEnableLogIndex    ; get address of current index
        ldr     r7, [r6]                                ; read index
        adds    r7, r7, #1                              ; increment
        str     r7, [r6]                                ; write back (not enough registers for interlocked)
        adds    r6, r6, VtEnableLog - VtEnableLogIndex  ; point r6 to the base of the table
        uxtb    r7, r7                                  ; convert index to a byte
        add     r6, r6, r7, lsl #2                      ; get address of entry
        sub     r7, lr, #2                              ; get address of faulting instruction
        str     r7, [r6]                                ; store in the table
#endif

        ldrd    r6, r7, [sp, #MsR6]                     ; reload r6/r7
        subs    pc, lr, #2                              ; return and re-execute the opcode

        LEAF_END KiEnableVfp
#endif

#include "emulate.asm"

;
; Global end label for checking faults
;

KiTrapModuleEnd

        END

