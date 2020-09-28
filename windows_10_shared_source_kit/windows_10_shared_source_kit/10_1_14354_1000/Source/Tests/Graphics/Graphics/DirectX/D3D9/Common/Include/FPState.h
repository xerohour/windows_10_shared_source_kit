// FILE:        fpstate.h
// DESC:        floating point state comparison utilities
// COMMENTS:
//
// GetTop: convert stack-relative register index to absolute value
// GetTag: get tag bits (2) of register relative to stack pointer
// GetReg: get register relative to stack pointer

#ifndef _FPSTATE_
#define _FPSTATE_

typedef struct _FloatingPointRegister {
    WORD w[5]; // 80 bit FP register
} FPREG, *PFPREG;

typedef struct _FloatingPointState {
    WORD cw;      // control word
    WORD res0;    // reserved
    WORD sw;      // status word
    WORD res1;    // reserved
    WORD tw;      // tag word
    WORD res2;    // reserved
    DWORD EIP;    // FPU instruction pointer offset
    WORD CS;      // FPU instruction pointer selector
    WORD Opcode;  // opcode (bits 0..10)
    DWORD DPTR;   // FPU operand pointer offset
    WORD DS;      // FPU operand pointer selector
    WORD res3;    // reserved
    FPREG reg[8]; // FP registers
    int GetTop(int n = 0) const { return((n + (sw >> 11)) & 7); }
    int GetTag(int n = 0) const { return((tw >> (GetTop(n) << 1)) & 3); }
    const FPREG &GetReg(int n = 0) const { return reg[GetTop(n)]; }
} FPSTATE, *PFPSTATE;

inline bool operator == (const FPREG &a, const FPREG &b)
{
    return !memcmp(&a, &b, sizeof(FPREG));
}

inline bool operator != (const FPREG &a, const FPREG &b)
{
    return !operator == (a, b);
}

typedef VOID (WINAPI *INITFUNC)(VOID);

typedef bool (WINAPI *GETFUNC)(PFPSTATE pState);

typedef bool (WINAPI *SETFUNC)(PFPSTATE pState);

typedef bool (WINAPI *COMPAREFUNC)(PFPSTATE pBefore, PFPSTATE pAfter);

#endif
