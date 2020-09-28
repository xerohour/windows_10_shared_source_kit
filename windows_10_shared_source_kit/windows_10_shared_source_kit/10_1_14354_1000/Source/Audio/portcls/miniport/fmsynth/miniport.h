//
// Copyright (c) Microsoft.  All rights reserved.
//
// ======================================================================
//  miniport.h
//      defines globals used by miniport.cpp
//
// ======================================================================
/* MIDI defines */
#define NUMCHANNELS                     (16)
#define NUMPATCHES                      (256)
#define DRUMCHANNEL                     (9)     /* midi channel 10 */


#define BCODE
#define fEnabled TRUE



#define AsULMUL(a, b) ((DWORD)((DWORD)(a) * (DWORD)(b)))
#define AsLSHL(a, b) ((DWORD)((DWORD)(a) << (DWORD)(b)))
#define AsULSHR(a, b) ((DWORD)((DWORD)(a) >> (DWORD)(b)))

#define AsMemCopy CopyMemory



































































































































































/* typedefs for MIDI patches */
#define PATCH_1_4OP             (0) /* use 4-operator patch */
#define PATCH_2_2OP             (1) /* use two 2-operator patches */
#define PATCH_1_2OP             (2) /* use one 2-operator patch */

#define NUM2VOICES   18
#define NUMOPS      4

#pragma pack (1)

typedef struct _operStruct {
    BYTE    bAt20;              /* flags which are send to 0x20 on fm */
    BYTE    bAt40;              /* flags seet to 0x40 */
                                /* the note velocity & midi velocity affect total level */
    BYTE    bAt60;              /* flags sent to 0x60 */
    BYTE    bAt80;              /* flags sent to 0x80 */
    BYTE    bAtE0;              /* flags send to 0xe0 */
} operStruct;

typedef struct _noteStruct {
    operStruct op[NUMOPS];      /* operators */
    BYTE    bAtA0[2];           /* send to 0xA0, A3 */
    BYTE    bAtB0[2];           /* send to 0xB0, B3 */
                                /* use in a patch, the block should be 4 to indicate
                                    normal pitch, 3 => octave below, etc. */
    BYTE    bAtC0[2];           /* sent to 0xc0, C3 */
    BYTE    bOp;                /* see PATCH_??? */
    BYTE    bDummy;             /* place holder */
} noteStruct;


typedef struct _patchStruct {
    noteStruct note;            /* note. This is all in the structure at the moment */
} patchStruct;


#include "patch.h"

#pragma pack()

/* MIDI */

typedef struct _voiceStruct {
        BYTE    bNote;                  /* note played */
        BYTE    bChannel;               /* channel played on */
        BYTE    bPatch;                 /* what patch is the note,
                                           drums patch = drum note + 128 */
        BYTE    bOn;                    /* TRUE if note is on, FALSE if off */
        BYTE    bVelocity;              /* velocity */
        BYTE    bJunk;                  /* filler */
        DWORD   dwTime;                 /* time that was turned on/off;
                                           0 time indicates that its not in use */
        DWORD   dwOrigPitch[2];         /* original pitch, for pitch bend */
        BYTE    bBlock[2];              /* value sent to the block */
        BYTE    bSusHeld;               /* turned off, but held on by sustain */
} voiceStruct;


/* a bit of tuning information */
#define FSAMP                           (50000.0)     /* sampling frequency */
#define PITCH(x)                        ((DWORD)((x) * (double) (1L << 19) / FSAMP))
                            /* x is the desired frequency,
                                == FNUM at b=1 */
#define EQUAL                           (1.059463094359)
#ifdef EUROPE
#       define  A                                                       (442.0)
#else
#       define  A                           (440.0)
#endif
#define ASHARP                          (A * EQUAL)
#define B                               (ASHARP * EQUAL)
#define C                               (B * EQUAL / 2.0)
#define CSHARP                          (C * EQUAL)
#define D                               (CSHARP * EQUAL)
#define DSHARP                          (D * EQUAL)
#define E                               (DSHARP * EQUAL)
#define F                               (E * EQUAL)
#define FSHARP                          (F * EQUAL)
#define G                               (FSHARP * EQUAL)
#define GSHARP                          (G * EQUAL)



























/* pitch values, from middle c, to octave above it */
static DWORD BCODE gdwPitch[12] = {
        PITCH(C), PITCH(CSHARP), PITCH(D), PITCH(DSHARP),
        PITCH(E), PITCH(F), PITCH(FSHARP), PITCH(G),
        PITCH(GSHARP), PITCH(A), PITCH(ASHARP), PITCH(B)};

