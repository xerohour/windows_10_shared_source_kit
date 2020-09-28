;++
;
; Copyright (c)  Microsoft Corporation
;
; Module Name:
;
;   emulate.asm
;
; Abstract:
;
;   This module implements the code necessary to emulate certain
;   classes of ARM instructions from within trap handlers without
;   the need to build up a full trap frame.
;
;   WARNING: This file is NOT standalone. It is designed to be
;   #included from another .asm file.
;
; Environment:
;
;   Kernel mode only.
;
;--


;++
;
; Emulation environment:
;
; These functions are designed to be called from either Undefined
; mode or Abort mode with minimal setup. Register contents should 
; be as follows:
;
;    r6 = scratch; original value saved to [sp, #MsR6]
;    r7 = opcode; original value saved to [sp, #MsR7]
;    sp = mini-stack pointer
;    lr = exception address (+2 for UDF mode, +8 for ABT mode)
;
; During emulation, additional scratch registers are saved to the
; mini-stack:
;
;    r0 -> saved to [sp, #MsReserved]
;    r4 -> saved to [sp, #MsR4]
;    r5 -> saved to [sp, #MsR5]
;    r6 -> saved to [sp, #MsR6] by the original caller
;    r7 -> saved to [sp, #MsR7] by the original caller
;    lr -> saved to [sp, #MsPc]
;    spsr -> saved to [sp, #MsCpsr]
;
; When complete, the emulation functions will advance LR to
; point past the instruction emulated, and will restore all
; temporary registers to their proper values.
;
; During execution, loads and stores are performed by code between
; KiEmulatedLoadStoreStart and KiEmulatedLoadStoreEnd. If a fault
; is detected with an exception address in this range, it should
; be diverted and treated as the equivalent fault on the original
; instruction.
;
;--

;
; Macro: Standard emulation helper setup, coming from a UDF mode
; trap. Saves r0,r4-r5 plus the adjusted LR and SPSR onto the target 
; mini-stack.
;

        MACRO
        SAVE_STANDARD_REGS_UDF
        
        sub     lr, lr, #2                              ; point to original PC
        srsia   sp, #CPSRM_UDF                          ; store to stack with CPSR
        str     r0, [sp, #MsReserved]                   ; store r0
        strd    r4, r5, [sp, #MsR4]                     ; store r4-r5
        
        MEND
        
        
;
; Macro: Standard emulation helper setup, coming from an ABT mode
; trap. Switches to UDF mode, copying the saved r6-r7 from the old
; mini-stack, and saving r0,r3,r4 plus the adjusted LR and SPSR onto
; the target mini-stack.
;

        MACRO
        SAVE_STANDARD_REGS_ABT
        
        sub     lr, lr, #8                              ; point to original PC
        mov     r6, sp                                  ; copy ABT mode SP to r6
        srsia   sp, #CPSRM_UDF                          ; store to stack with CPSR
        cps     #CPSRM_UDF                              ; switch to UDF mode
        str     r0, [sp, #MsReserved]                   ; store r0
        ldrd    r6, r0, [r6, #MsR6]                     ; copy r6-r7 from ABT stack
        strd    r4, r5, [sp, #MsR4]                     ; store r4-r5
        strd    r6, r0, [sp, #MsR6]                     ; store r6-r7 to UDF stack
        
        MEND
        
        
;
; Macro: Standard emulation helper return. Restores r0,r4-r7 from the
; target mini-stack and adjusts LR to point after the opcode.
;

        MACRO
        RESTORE_AND_RETURN $InstructionSize

        ldr     r0, [sp, #MsPc]                         ; fetch original PC
        ldrd    r4, r5, [sp, #MsR4]                     ; restore r4-r5
        ldrd    r6, r7, [sp, #MsR6]                     ; restore r6-r7
        adds    r0, r0, #$InstructionSize               ; adjust for instruction size
        str     r0, [sp, #MsPc]                         ; store updated PC
        ldr     r0, [sp, #MsReserved]                   ; restore r0
        rfeia   sp                                      ; return to target
        
        MEND

;
; Macro: Read a GPR register by index, specified in $IndexReg.
; The resulting register value is loaded into r4.
;

        MACRO
        EMULATE_GPR_READ $IndexReg

        adr     r4, GetR0+1                             ; get thumbified address of GetR0
        add     r4, r4, $IndexReg, lsl #2               ; add 4 * index
        blx     r4                                      ; call there
        
        MEND

;
; Macro: Write a GPR register by index, specified in $IndexReg.
; The register value is written from r4.
;

        MACRO
        EMULATE_GPR_WRITE $IndexReg, $TempReg

        adr     $TempReg, SetR0+1                       ; get thumbified address of SetR0
        add     $TempReg, $TempReg, $IndexReg, lsl #2   ; add 4 * index
        blx     $TempReg                                ; call there
        
        MEND

;
; Macro: Read a single-precision VFP register by index, specified 
; in $IndexReg. The resulting register value is loaded into r4.
;

        MACRO
        EMULATE_VFPS_READ $IndexReg

        adr     r4, GetS0+1                             ; get thumbified address of GetS0
        add     r4, r4, $IndexReg, lsl #2               ; add 4 * index
        add     r4, r4, $IndexReg, lsl #1               ; add 2 * index = 6 * index
        blx     r4                                      ; call there
        
        MEND

;
; Macro: Write a single-precision VFP register by index, specified 
; in $IndexReg. The register value is written from r4.
;

        MACRO
        EMULATE_VFPS_WRITE $IndexReg, $TempReg

        adr     $TempReg, SetS0+1                       ; get thumbified address of SetS0
        add     $TempReg, $TempReg, $IndexReg, lsl #2   ; add 4 * index
        add     $TempReg, $TempReg, $IndexReg, lsl #1   ; add 2 * index = 6 * index
        blx     $TempReg                                ; call there
        
        MEND

;
; Macro: Read a double-precision VFP register by index, specified 
; in $IndexReg. The resulting register value is loaded into r4:r5.
;

        MACRO
        EMULATE_VFPD_READ $IndexReg

        adr     r4, GetD0+1                             ; get thumbified address of GetD0
        add     r4, r4, $IndexReg, lsl #2               ; add 4 * index
        add     r4, r4, $IndexReg, lsl #1               ; add 2 * index = 6 * index
        blx     r4                                      ; call there
        
        MEND

;
; Macro: Write a double-precision VFP register by index, specified 
; in $IndexReg. The register value is written from r4:r5.
;

        MACRO
        EMULATE_VFPD_WRITE $IndexReg, $TempReg

        adr     $TempReg, SetD0+1                       ; get thumbified address of SetD0
        add     $TempReg, $TempReg, $IndexReg, lsl #2   ; add 4 * index
        add     $TempReg, $TempReg, $IndexReg, lsl #1   ; add 2 * index = 6 * index
        blx     $TempReg                                ; call there
        
        MEND

;++
;
; Routine Description:
;
;   Emulate an SDIV/UDIV instruction. Must be called in Undefined mode.
;
; Arguments:
;
;   r7 - Contains the full 32-bit instruction
;
;   lr - Points to the address of the instruction + 2
;
;   sp - Points to the Undefined mini stack, with r6/r7 saved into the
;        MsR6 area
;
; Return Value:
;
;   Mini stack and non-volatile registers are updated.
;
; Instruction forms:
;
;      +---------+-----------+-+-------+ +-------+-------+-------+-------+
;      |1 1 1 1 1|0 1 1 1 0 1|1|   Rn  | |1 1 1 1|   Rd  |1 1 1 1|   Rm  | UDIV
;      +---------+-----------+-+-------+ +-------+-------+-------+-------+
;      |1 1 1 1 1|0 1 1 1 0 0|1|   Rn  | |1 1 1 1|   Rd  |1 1 1 1|   Rm  | SDIV
;      +---------+-----------+-+-------+ +-------+-------+-------+-------+
;
;--
;

        MACRO
        DIVIDE_CORE_32_STEP $Shift
        subs    r6, r0, r5, lsl #$Shift                 ; r6 = r0 - r5 << Shift
        movcs   r0, r6                                  ; keep the result if carry set
        adcs    r4, r4, r4                              ; double the result and add the carry
        MEND

        LEAF_ENTRY KiEmulateDivide

        SAVE_STANDARD_REGS_UDF                          ; coming from UDF mode

        ubfx    r5, r7, #16, #4                         ; get Rn
        EMULATE_GPR_READ r5                             ; read the GPR into r4
        mov     r0, r4                                  ; copy to r0
        ubfx    r5, r7, #0, #4                          ; get Rm
        EMULATE_GPR_READ r5                             ; read the GPR into r4
        
        tst     r7, #0x00200000                         ; SDIV or UDIV?
        bne     KiedStartDivide                         ; if UDIV, skip it all
        teq     r0, r4                                  ; determine if the result will be negative
        bmi     %F1                                     ; if so, skip ahead
        eor     r7, r7, #0x00200000                     ; otherwise, flip the opcode bit to make it like UDIV
1       cmp     r0, #0                                  ; is r0 negative?
        neglt   r0, r0                                  ; negate if so
        cmp     r4, #0                                  ; is r4 negative?
        neglt   r4, r4                                  ; negate if so

KiedStartDivide
        ; this is copied from the CRT divide implemention, with r4/r0 swapped
        cbz     r4, %F1                                 ; skip if divide by 0
        mov     r5, r4                                  ; save the original divisor in r5
        clz     r4, r4                                  ; get zero count of divisor
        clz     r6, r0                                  ; get zero count of dividend
        subs    r4, r4, r6                              ; get difference between bit counts
        adr     r6, %F8 + 1                             ; get address of entry 0 plus thumb bit
        sub     r6, r6, r4, lsl #3                      ; subtract count * 8
        sub     r6, r6, r4, lsl #1                      ; subtract count * 2 = count * 10
        mov     r4, #0                                  ; initialize final result
        blt     KiedEndDivide                           ; if the number of bits is < 0, result is 0
        bx      r6                                      ; go there
1
        movs    r4, #0                                  ; result is 0
        b       KiedEndDivide                           ; finish up

        DIVIDE_CORE_32_STEP 31
        DIVIDE_CORE_32_STEP 30
        DIVIDE_CORE_32_STEP 29
        DIVIDE_CORE_32_STEP 28
        DIVIDE_CORE_32_STEP 27
        DIVIDE_CORE_32_STEP 26
        DIVIDE_CORE_32_STEP 25
        DIVIDE_CORE_32_STEP 24
        DIVIDE_CORE_32_STEP 23
        DIVIDE_CORE_32_STEP 22
        DIVIDE_CORE_32_STEP 21
        DIVIDE_CORE_32_STEP 20
        DIVIDE_CORE_32_STEP 19
        DIVIDE_CORE_32_STEP 18
        DIVIDE_CORE_32_STEP 17
        DIVIDE_CORE_32_STEP 16
        DIVIDE_CORE_32_STEP 15
        DIVIDE_CORE_32_STEP 14
        DIVIDE_CORE_32_STEP 13
        DIVIDE_CORE_32_STEP 12
        DIVIDE_CORE_32_STEP 11
        DIVIDE_CORE_32_STEP 10
        DIVIDE_CORE_32_STEP 9
        DIVIDE_CORE_32_STEP 8
        DIVIDE_CORE_32_STEP 7
        DIVIDE_CORE_32_STEP 6
        DIVIDE_CORE_32_STEP 5
        DIVIDE_CORE_32_STEP 4
        DIVIDE_CORE_32_STEP 3
        DIVIDE_CORE_32_STEP 2
        DIVIDE_CORE_32_STEP 1
8
        DIVIDE_CORE_32_STEP 0
KiedEndDivide

        tst     r7, #0x00200000                         ; do we need to negate the result?
        bne     %F1                                     ; if not, skip
        negs    r4, r4                                  ; negate it
1       ubfx    r0, r7, #8, #4                          ; get Rd
        EMULATE_GPR_WRITE r0, r7                        ; write from r4

        RESTORE_AND_RETURN 4                            ; restore from exception
        
        LEAF_END KiEmulateDivide

;++
;
; Routine Description:
;
;   Emulate a misaligned VLDR/VSTR instruction. Must be called in
;   Abort mode.
;
; Arguments:
;
;   r7 - Contains the full 32-bit instruction
;
;   lr - Points to the address of the instruction + 8
;
;   sp - Points to the Abort mini stack, with r6/r7 saved into the
;        MsR6 area
;
; Return Value:
;
;   Mini stack and non-volatile registers are updated.
;
; Instruction forms:
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
;--

        LEAF_ENTRY KiEmulateLoadStoreVfp

        SAVE_STANDARD_REGS_ABT                          ; coming from ABT mode
        
        ubfx    r0, r7, #16, #4                         ; get base register index
        EMULATE_GPR_READ r0                             ; read the GPR into r4
        tst     r7, #0x00800000                         ; check the U bit
        uxtb    r0, r7                                  ; extract offset
        bne     %F1                                     ; if non-zero, we're ok
        negs    r0, r0                                  ; negate offset
1
        add     r0, r4, r0, lsl #2                      ; add offset * 4, final result in r0
        tst     r7, #0x00100000                         ; load or store?
        ubfx    r5, r7, #12, #4                         ; get low bits of VFP register index
        ubfx    r4, r7, #22, #1                         ; get upper bit
        bne     %F3                                     ; handle loads ahead

        tst     r7, #0x00000100                         ; single or double?
        bne     %F2                                     ; handle double precision ahead
        orr     r6, r4, r5, lsl #1                      ; get final register index
        EMULATE_VFPS_READ r6                            ; read a single-precision register into r4
        bl      KiEmulateStoreWord                      ; store the word safely (trashes r6)
        b       %F5                                     ; done!
2
        orr     r6, r5, r4, lsl #4                      ; get final register index
        EMULATE_VFPD_READ r6                            ; read a double-precision register into r4:r5
        bl      KiEmulateStoreWord                      ; store the low word safely (trashes r6)
        adds    r0, r0, #4                              ; increment address
        mov     r4, r5                                  ; copy high result into r4
        bl      KiEmulateStoreWord                      ; store the high word safely (trashes r6)
        b       %F5                                     ; done!
3
        tst     r7, #0x00000100                         ; single or double?
        bne     %F4                                     ; handle double precision ahead
        orr     r6, r4, r5, lsl #1                      ; get final register index
        bl      KiEmulateLoadWord                       ; load the word safely into r4
        EMULATE_VFPS_WRITE r6, r0                       ; store to the target
        b       %F5                                     ; done!
4
        orr     r6, r5, r4, lsl #4                      ; get final register index
        bl      KiEmulateLoadWord                       ; load the word safely into r4
        mov     r7, r4                                  ; copy low result into r7
        adds    r0, r0, #4                              ; advance address by a word
        bl      KiEmulateLoadWord                       ; load the word safely into r4
        mov     r5, r4                                  ; upper word in r5
        mov     r4, r7                                  ; lower word in r4
        EMULATE_VFPD_WRITE r6, r0                       ; store to the target
5
        RESTORE_AND_RETURN 4                            ; restore from exception

        LEAF_END KiEmulateLoadStoreVfp

;++
;
; Routine Description:
;
;   Emulate a misaligned LDRD/STRD instruction. Must be called in
;   Abort mode.
;
; Arguments:
;
;   r7 - Contains the full 32-bit instruction
;
;   lr - Points to the address of the instruction + 8
;
;   sp - Points to the Abort mini stack, with r6/r7 saved into the
;        MsR6 area
;
; Return Value:
;
;   Mini stack and non-volatile registers are updated.
;
; Instruction forms:
;
;      +---------+---+-+-+-+-+-+-------+ +-------+-------+---------------+
;      |1 1 1 0 1|0 0|P|U|1|W|1|   Rn  | |   Rt  |  Rt2  |      imm8     | LDRD (imm)
;      +---------+---+-+-+-+-+-+-------+ +-------+-------+---------------+
;      |1 1 1 0 1|0 0|P|U|1|W|0|   Rn  | |   Rt  |  Rt2  |      imm8     | STRD (imm)
;      +---------+---+-+-+-+-+-+-------+ +-------+-------+---------------+
;--

        LEAF_ENTRY KiEmulateLoadStoreDouble

        SAVE_STANDARD_REGS_ABT                          ; coming from ABT mode
        
        ubfx    r0, r7, #16, #4                         ; get base register index
        EMULATE_GPR_READ r0                             ; read the GPR into r4
        tst     r7, #0x00800000                         ; check the U bit
        uxtb    r0, r7                                  ; extract offset
        bne     %F1                                     ; if non-zero, we're ok
        negs    r0, r0                                  ; negate offset
1
        tst     r7, #0x01000000                         ; check P bit
        add     r6, r4, r0, lsl #2                      ; add offset * 4, final result in r6
        mov     r0, r4                                  ; assume P = 0, use original value for addr
        beq     %F2                                     ; if P = 0, skip
        mov     r0, r6                                  ; use adjusted value for addr
2        
        tst     r7, #0x00100000                         ; load or store?
        bne     %F3                                     ; handle loads ahead

        ubfx    r5, r7, #12, #4                         ; get Rt index in r5
        EMULATE_GPR_READ r5                             ; read register into r4
        bl      KiEmulateStoreWord                      ; store the low word safely (trashes r6)
        adds    r0, r0, #4                              ; increment address
        ubfx    r5, r7, #8, #4                          ; get Rt2 index in r5
        EMULATE_GPR_READ r5                             ; read register into r4
        bl      KiEmulateStoreWord                      ; store the high word safely (trashes r6)
        b       %F5                                     ; done!
3
        bl      KiEmulateLoadWord                       ; load the word safely into r4
        adds    r0, r0, #4                              ; advance address by a word
        mov     r5, r4                                  ; copy to r5
        bl      KiEmulateLoadWord                       ; load the word safely into r4
        ubfx    r0, r7, #8, #4                          ; get Rt2 index in r0
        EMULATE_GPR_WRITE r0, r6                        ; write the result
        mov     r4, r5                                  ; recover first loaded word
        ubfx    r0, r7, #12, #4                         ; get Rt index in r0
        EMULATE_GPR_WRITE r0, r6                        ; write the result
5
        tst     r7, #0x00200000                         ; writeback?
        beq     %F6                                     ; if not, skip
        ubfx    r0, r7, #16, #4                         ; get base register index
        EMULATE_GPR_READ r0                             ; read the GPR into r4
        tst     r7, #0x00800000                         ; check the U bit
        uxtb    r0, r7                                  ; extract offset
        bne     %F7                                     ; if non-zero, we're ok
        negs    r0, r0                                  ; negate offset
7
        add     r4, r4, r0, lsl #2                      ; add offset * 4, final result in r4
        ubfx    r0, r7, #16, #4                         ; get base register index
        EMULATE_GPR_WRITE r0, r6                        ; write to the register
6
        RESTORE_AND_RETURN 4                            ; restore from exception

        LEAF_END KiEmulateLoadStoreDouble

;++
;
; Routine Description:
;
;   Emulate a misaligned LDM/STM instruction. Must be called in
;   Abort mode.
;
; Arguments:
;
;   r7 - Contains the full 16-bit or 32-bit instruction
;
;   lr - Points to the address of the instruction + 8
;
;   sp - Points to the Abort mini stack, with r6/r7 saved into the
;        MsR6 area
;
; Return Value:
;
;   Mini stack and non-volatile registers are updated.
;
; Instruction forms:
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
;--

        LEAF_ENTRY KiEmulateLoadStoreMultiple16

        SAVE_STANDARD_REGS_ABT                          ; coming from ABT mode
        
        ubfx    r0, r7, #8, #3                          ; get Rn
        ubfx    r4, r7, #11, #1                         ; get load/store and set flag
        uxtb    r7, r7                                  ; get just the register mask in r7
        cmp     r4, #0                                  ; load or store?
        orr     r7, r7, r0, lsl #16                     ; put Rn up in the right spot
        orr     r7, r7, r4, lsl #20                     ; put load/store in the right spot
        orr     r7, r7, #0x00a00000                     ; set W=1 and bit 23 to indicate IA
        beq     KiEmulateLoadStoreMultipleCommon        ; if store, we're done
        movs    r4, #1                                  ; start with 1
        lsls    r4, r4, r0                              ; compute 1 << Rn
        tst     r7, r4                                  ; is Rn set?
        beq     KiEmulateLoadStoreMultipleCommon        ; if not, we're done
        bic     r7, r7, #0x00200000                     ; clear W if Rn is included in register list
        b       KiEmulateLoadStoreMultipleCommon        ; hop to the common code
        
        LEAF_END


        LEAF_ENTRY KiEmulateLoadStoreMultiple

        SAVE_STANDARD_REGS_ABT                          ; coming from ABT mode
        
KiEmulateLoadStoreMultipleCommon
        ubfx    r0, r7, #16, #4                         ; get base register index
        EMULATE_GPR_READ r0                             ; read the GPR into r4
        tst     r7, #0x01000000                         ; DB forms?
        beq     %F2                                     ; skip if not
        uxth    r6, r7                                  ; get register bits in r6
        movs    r0, #0                                  ; start with 0 count
0
        lsrs    r6, r6, #1                              ; shift out next bit
        adc     r0, r0, #0                              ; add the carry to r0
        bne     %B0                                     ; loop until all bits are gone
        sub     r4, r4, r0, lsl #2                      ; pre-decrement
2
        tst     r7, #0x00100000                         ; load or store?
        mov     r0, r4                                  ; starting address in r0
        mov     r5, #0                                  ; register index in r5
        bne     %F5                                     ; handle loads ahead
3
        lsr.w   r6, r7, r5                              ; r6 = r7 >> r5
        tst     r6, #1                                  ; low bit set?
        beq     %F4                                     ; if not set, skip
        EMULATE_GPR_READ r5                             ; read register into r4
        bl      KiEmulateStoreWord                      ; write it out (trashes r6)
        adds    r0, r0, #4                              ; increment address
4
        adds    r5, r5, #1                              ; increment register index
        cmp     r5, #16                                 ; done?
        blo     %B3                                     ; loop if not
        b       %F9                                     ; finish up
5
        lsr.w   r6, r7, r5                              ; r6 = r7 >> r5
        tst     r6, #1                                  ; low bit set?
        beq     %F6                                     ; if not set, skip
        bl      KiEmulateLoadWord                       ; load the word into r4
        adds    r0, r0, #4                              ; increment address
        EMULATE_GPR_WRITE r5, r6                        ; write register from r4
6
        adds    r5, r5, #1                              ; increment register index
        cmp     r5, #16                                 ; done?
        blo     %B5                                     ; loop if not
9
        tst     r7, #0x00200000                         ; writeback?
        beq     %F7                                     ; if not, skip
        tst     r7, #0x01000000                         ; DB forms?
        mov     r4, r0                                  ; copy final address to r4
        beq     %F8                                     ; if not DB, then final address is correct
        uxth    r6, r7                                  ; get register bits in r6
        movs    r0, #0                                  ; start with 0 count
0
        lsrs    r6, r6, #1                              ; shift out next bit
        adc     r0, r0, #0                              ; add the carry to r0
        bne     %B0                                     ; loop until all bits are gone
        sub     r4, r4, r0, lsl #2                      ; back up to where we started
8
        ubfx    r0, r7, #16, #4                         ; get base register index
        EMULATE_GPR_WRITE r0, r6                        ; write to the register
7
        tst     r7, #0x80000000                         ; is the high bit set?
        beq     %F10                                    ; if not, it was a 16-bit instruction
        RESTORE_AND_RETURN 4                            ; restore from exception
10
        RESTORE_AND_RETURN 2                            ; restore from exception

        LEAF_END KiEmulateLoadStoreMultiple

;++
;
; Routine Description:
;
;   Perform a user-mode equivalent read with proper exception semantics.
;
; Arguments:
;
;   r0 - Supplies the address to read from.
;
; Return Value:
;
;   r4 - Contains the result.
;
; Modified:
;
;   r6 - loaded with a pointer to the mini-stack for exception handling.
;
;--

KiEmulatedLoadStoreStart
        EXPORT KiEmulatedLoadStoreStart

        LEAF_ENTRY KiEmulateLoadWord
        ldr     r4, [sp, #MsCpsr]                       ; get originating mode
        tst     r4, #0xf                                ; user mode?
        bne     %F1                                     ; if not, skip
        ldrt    r4, [r0]                                ; load from the address
        bx      lr                                      ; return
1       ldr     r4, [r0]
        bx      lr
        LEAF_END KiEmulateLoadWord

;++
;
; Routine Description:
;
;   Perform a user-mode equivalent write with proper exception semantics.
;
; Arguments:
;
;   r0 - Supplies the address to read from.
;
;   r4 - Supplies the value to write.
;
; Return Value:
;
;   none
;
; Modified:
;
;   r6 - loaded with a pointer to the mini-stack for exception handling.
;
;--

        LEAF_ENTRY KiEmulateStoreWord
        ldr     r6, [sp, #MsCpsr]                       ; get originating mode
        tst     r6, #0xf                                ; user mode?
        bne     %F1                                     ; if not, skip
        strt    r4, [r0]                                ; store to the address
        bx      lr                                      ; return
1       str     r4, [r0]
        bx      lr
        LEAF_END KiEmulateStoreWord

KiEmulatedLoadStoreEnd
        EXPORT KiEmulatedLoadStoreEnd

;++
;
; Routine Description:
;
;   Helper to convert a data abort between KiEmulatedLoadStoreStart and
;   KiEmulatedLoadStoreEnd (above) into a data abort on the original
;   instruction.
;
; Arguments:
;
;   None.
;
; Return Value:
;
;   none
;
; Modified:
;
;   r0-r7 are reset back to their original states when the initial emulated
;   instruction was executed.
;
;   lr is set to the address of the original instruction, plus 8.
;
;   spsr is copied from the original exception.
;
;--

KiEmulatedLoadStoreFault
        EXPORT KiEmulatedLoadStoreFault
        
        cps     #CPSRM_UDF                              ; switch to UDF mode
        mov     r0, sp                                  ; copy UDF stack pointer to r0
        cps     #CPSRM_ABT                              ; back to ABT mode
        ldrd    lr, r4, [r0, #MsPc]                     ; fetch PC and SPSR
        add     lr, lr, #8                              ; offset PC by 8 to simulate data abort
        msr     spsr_fsxc, r4                           ; copy SPSR back from r4     
        srsia   sp, #CPSRM_ABT                          ; save LR/SPSR to ABT mini-stack
        ldrd    r6, r7, [r0, #MsR6]                     ; restore r6/r7
        ldrd    r4, r5, [r0, #MsR4]                     ; restore r4/r5
        ldr     r0, [r0, #MsReserved]                   ; restore r0
        b       KiEmulatedLoadStoreFaultResume          ; resume handling

;++
;
; Helpers for EMULATE_GPR_READ.
;
; Each entry must be exactly 4 bytes long (except the last)
;
;--

GetR0   ldr     r4, [sp, #MsReserved]                   ; r0
        bx      lr
GetR1   mov     r4, r1                                  ; r1
        bx      lr
GetR2   mov     r4, r2                                  ; r2
        bx      lr
GetR3   mov     r4, r3                                  ; r3
        bx      lr
GetR4   ldr     r4, [sp, #MsR4]                         ; r4
        bx      lr
GetR5   ldr     r4, [sp, #MsR5]                         ; r5
        bx      lr
GetR6   ldr     r4, [sp, #MsR6]                         ; r6
        bx      lr
GetR7   ldr     r4, [sp, #MsR7]                         ; r7
        bx      lr
GetR8   mov     r4, r8                                  ; r8
        bx      lr
GetR9   mov     r4, r9                                  ; r9
        bx      lr
GetR10  mov     r4, r10                                 ; r10
        bx      lr
GetR11  mov     r4, r11                                 ; r11
        bx      lr
GetR12  mov     r4, r12                                 ; r12
        bx      lr
GetR13  b       GetR13X                                 ; r13 - special case
        nop
GetR14  b       GetR14X                                 ; r14 - special case
        nop
GetR15  ldr     r4, [sp, #MsPc]                         ; r15 - get the PC value
        bic     r4, r4, #3                              ; force align
        bx      lr                                      ; return

GetR13X ldr     r4, [sp, #MsCpsr]                       ; fetch original mode
        tst     r4, #0xf                                ; kernel/user?
        bne     GetR13K                                 ; skip ahead if kernel
        cps     #CPSRM_SYS                              ; switch to SYS mode
        mov     r4, sp                                  ; get SP
        cps     #CPSRM_UDF                              ; back to UDF mode
        bx      lr                                      ; return
GetR13K cps     #CPSRM_SVC                              ; switch to SVC mode
        mov     r4, sp                                  ; get SP
        cps     #CPSRM_UDF                              ; back to UDF mode
        bx      lr                                      ; return

GetR14X ldr     r4, [sp, #MsCpsr]                       ; fetch original mode
        tst     r4, #0xf                                ; kernel/user?
        bne     GetR14K                                 ; skip ahead if kernel
        cps     #CPSRM_SYS                              ; switch to SYS mode
        mov     r4, lr                                  ; get LR
        cps     #CPSRM_UDF                              ; back to UDF mode
        bx      lr                                      ; return
GetR14K cps     #CPSRM_SVC                              ; switch to SVC mode
        mov     r4, lr                                  ; get LR
        cps     #CPSRM_UDF                              ; back to UDF mode
        bx      lr                                      ; return

;++
;
; Helpers for EMULATE_GPR_WRITE.
;
; Each entry must be exactly 4 bytes long (except the last)
;
;--

SetR0   str     r4, [sp, #MsReserved]                   ; r0
        bx      lr
SetR1   mov     r1, r4                                  ; r1
        bx      lr
SetR2   mov     r2, r4                                  ; r1
        bx      lr
SetR3   mov     r3, r4                                  ; r1
        bx      lr
SetR4   str     r4, [sp, #MsR4]                         ; r4
        bx      lr
SetR5   str     r4, [sp, #MsR5]                         ; r5
        bx      lr
SetR6   str     r4, [sp, #MsR6]                         ; r6
        bx      lr
SetR7   str     r4, [sp, #MsR7]                         ; r7
        bx      lr
SetR8   mov     r8, r4                                  ; r8
        bx      lr
SetR9   mov     r9, r4                                  ; r9
        bx      lr
SetR10  mov     r10, r4                                 ; r10
        bx      lr
SetR11  mov     r11, r4                                 ; r11
        bx      lr
SetR12  mov     r12, r4                                 ; r12
        bx      lr
SetR13  bx      lr                                      ; r13 - can't write
        nop
SetR14  b       SetR14X                                 ; r14 - special case
        nop
SetR15  bx      lr                                      ; r15 - can't write

SetR14X str     r4, [sp, #MsReserved+4]                 ; save value in r4
        ldr     r4, [sp, #MsCpsr]                       ; fetch original mode
        tst     r4, #0xf                                ; kernel/user?
        ldr     r4, [sp, #MsReserved+4]                 ; restore value from r4
        bne     SetR14K                                 ; skip ahead if kernel
        cps     #CPSRM_SYS                              ; switch to SYS mode
        mov     lr, r4                                  ; set LR
        cps     #CPSRM_UDF                              ; back to UDF mode
        bx      lr                                      ; return
SetR14K cps     #CPSRM_SVC                              ; switch to SVC mode
        mov     lr, r4                                  ; set LR
        cps     #CPSRM_UDF                              ; back to UDF mode
        bx      lr                                      ; return

;++
;
; Helpers for EMULATE_VFPS_READ.
;
; Each entry must be exactly 6 bytes long (except the last)
;
;--

GetS0   vmov    r4, s0
        bx      lr
GetS1   vmov    r4, s1
        bx      lr
GetS2   vmov    r4, s2
        bx      lr
GetS3   vmov    r4, s3
        bx      lr
GetS4   vmov    r4, s4
        bx      lr
GetS5   vmov    r4, s5
        bx      lr
GetS6   vmov    r4, s6
        bx      lr
GetS7   vmov    r4, s7
        bx      lr
GetS8   vmov    r4, s8
        bx      lr
GetS9   vmov    r4, s9
        bx      lr
GetS10  vmov    r4, s10
        bx      lr
GetS11  vmov    r4, s11
        bx      lr
GetS12  vmov    r4, s12
        bx      lr
GetS13  vmov    r4, s13
        bx      lr
GetS14  vmov    r4, s14
        bx      lr
GetS15  vmov    r4, s15
        bx      lr
GetS16  vmov    r4, s16
        bx      lr
GetS17  vmov    r4, s17
        bx      lr
GetS18  vmov    r4, s18
        bx      lr
GetS19  vmov    r4, s19
        bx      lr
GetS20  vmov    r4, s20
        bx      lr
GetS21  vmov    r4, s21
        bx      lr
GetS22  vmov    r4, s22
        bx      lr
GetS23  vmov    r4, s23
        bx      lr
GetS24  vmov    r4, s24
        bx      lr
GetS25  vmov    r4, s25
        bx      lr
GetS26  vmov    r4, s26
        bx      lr
GetS27  vmov    r4, s27
        bx      lr
GetS28  vmov    r4, s28
        bx      lr
GetS29  vmov    r4, s29
        bx      lr
GetS30  vmov    r4, s30
        bx      lr
GetS31  vmov    r4, s31
        bx      lr

;++
;
; Helpers for EMULATE_VFPD_READ.
;
; Each entry must be exactly 6 bytes long (except the last)
;
;--

GetD0   vmov    r4, r5, d0
        bx      lr
GetD1   vmov    r4, r5, d1
        bx      lr
GetD2   vmov    r4, r5, d2
        bx      lr
GetD3   vmov    r4, r5, d3
        bx      lr
GetD4   vmov    r4, r5, d4
        bx      lr
GetD5   vmov    r4, r5, d5
        bx      lr
GetD6   vmov    r4, r5, d6
        bx      lr
GetD7   vmov    r4, r5, d7
        bx      lr
GetD8   vmov    r4, r5, d8
        bx      lr
GetD9   vmov    r4, r5, d9
        bx      lr
GetD10  vmov    r4, r5, d10
        bx      lr
GetD11  vmov    r4, r5, d11
        bx      lr
GetD12  vmov    r4, r5, d12
        bx      lr
GetD13  vmov    r4, r5, d13
        bx      lr
GetD14  vmov    r4, r5, d14
        bx      lr
GetD15  vmov    r4, r5, d15
        bx      lr
GetD16  vmov    r4, r5, d16
        bx      lr
GetD17  vmov    r4, r5, d17
        bx      lr
GetD18  vmov    r4, r5, d18
        bx      lr
GetD19  vmov    r4, r5, d19
        bx      lr
GetD20  vmov    r4, r5, d20
        bx      lr
GetD21  vmov    r4, r5, d21
        bx      lr
GetD22  vmov    r4, r5, d22
        bx      lr
GetD23  vmov    r4, r5, d23
        bx      lr
GetD24  vmov    r4, r5, d24
        bx      lr
GetD25  vmov    r4, r5, d25
        bx      lr
GetD26  vmov    r4, r5, d26
        bx      lr
GetD27  vmov    r4, r5, d27
        bx      lr
GetD28  vmov    r4, r5, d28
        bx      lr
GetD29  vmov    r4, r5, d29
        bx      lr
GetD30  vmov    r4, r5, d30
        bx      lr
GetD31  vmov    r4, r5, d31
        bx      lr

;++
;
; Helpers for EMULATE_VFPS_WRITE.
;
; Each entry must be exactly 6 bytes long (except the last)
;
;--

SetS0   vmov    s0, r4
        bx      lr
SetS1   vmov    s1, r4
        bx      lr
SetS2   vmov    s2, r4
        bx      lr
SetS3   vmov    s3, r4
        bx      lr
SetS4   vmov    s4, r4
        bx      lr
SetS5   vmov    s5, r4
        bx      lr
SetS6   vmov    s6, r4
        bx      lr
SetS7   vmov    s7, r4
        bx      lr
SetS8   vmov    s8, r4
        bx      lr
SetS9   vmov    s9, r4
        bx      lr
SetS10  vmov    s10, r4
        bx      lr
SetS11  vmov    s11, r4
        bx      lr
SetS12  vmov    s12, r4
        bx      lr
SetS13  vmov    s13, r4
        bx      lr
SetS14  vmov    s14, r4
        bx      lr
SetS15  vmov    s15, r4
        bx      lr
SetS16  vmov    s16, r4 
        bx      lr
SetS17  vmov    s17, r4 
        bx      lr
SetS18  vmov    s18, r4 
        bx      lr
SetS19  vmov    s19, r4 
        bx      lr
SetS20  vmov    s20, r4 
        bx      lr
SetS21  vmov    s21, r4 
        bx      lr
SetS22  vmov    s22, r4 
        bx      lr
SetS23  vmov    s23, r4 
        bx      lr
SetS24  vmov    s24, r4 
        bx      lr
SetS25  vmov    s25, r4 
        bx      lr
SetS26  vmov    s26, r4 
        bx      lr
SetS27  vmov    s27, r4 
        bx      lr
SetS28  vmov    s28, r4 
        bx      lr
SetS29  vmov    s29, r4 
        bx      lr
SetS30  vmov    s30, r4 
        bx      lr
SetS31  vmov    s31, r4 
        bx      lr

;++
;
; Helpers for EMULATE_VFPD_WRITE.
;
; Each entry must be exactly 6 bytes long (except the last)
;
;--

SetD0   vmov    d0, r4, r5
        bx      lr
SetD1   vmov    d1, r4, r5
        bx      lr
SetD2   vmov    d2, r4, r5
        bx      lr
SetD3   vmov    d3, r4, r5
        bx      lr
SetD4   vmov    d4, r4, r5
        bx      lr
SetD5   vmov    d5, r4, r5
        bx      lr
SetD6   vmov    d6, r4, r5
        bx      lr
SetD7   vmov    d7, r4, r5
        bx      lr
SetD8   vmov    d8, r4, r5
        bx      lr
SetD9   vmov    d9, r4, r5
        bx      lr
SetD10  vmov    d10, r4, r5
        bx      lr
SetD11  vmov    d11, r4, r5
        bx      lr
SetD12  vmov    d12, r4, r5
        bx      lr
SetD13  vmov    d13, r4, r5
        bx      lr
SetD14  vmov    d14, r4, r5
        bx      lr
SetD15  vmov    d15, r4, r5
        bx      lr
SetD16  vmov    d16, r4, r5
        bx      lr
SetD17  vmov    d17, r4, r5
        bx      lr
SetD18  vmov    d18, r4, r5
        bx      lr
SetD19  vmov    d19, r4, r5
        bx      lr
SetD20  vmov    d20, r4, r5
        bx      lr
SetD21  vmov    d21, r4, r5
        bx      lr
SetD22  vmov    d22, r4, r5
        bx      lr
SetD23  vmov    d23, r4, r5
        bx      lr
SetD24  vmov    d24, r4, r5
        bx      lr
SetD25  vmov    d25, r4, r5
        bx      lr
SetD26  vmov    d26, r4, r5
        bx      lr
SetD27  vmov    d27, r4, r5
        bx      lr
SetD28  vmov    d28, r4, r5
        bx      lr
SetD29  vmov    d29, r4, r5
        bx      lr
SetD30  vmov    d30, r4, r5
        bx      lr
SetD31  vmov    d31, r4, r5
        bx      lr

