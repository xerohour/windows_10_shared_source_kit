/*****************************************************************************
 * pcwpptr.h - WPP tracing control guid for Portcls
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 */

#ifndef _PCWPPTR_H_
#define _PCWPPTR_H_

#ifdef PC_USE_ETW
#define WPP_CONTROL_GUIDS \
    WPP_DEFINE_CONTROL_GUID(PortCls,(10EB6007,818C,4DB6,A694,B518E589D07A), \
        WPP_DEFINE_BIT(PCTRACE_PNP)     /* bit  0 = 0x00000001 */ \
        WPP_DEFINE_BIT(PCTRACE_POWER)   /* bit  1 = 0x00000002 */ \
        WPP_DEFINE_BIT(PCTRACE_IRPSTRM) /* bit  2 = 0x00000004 */ \
        WPP_DEFINE_BIT(PCTRACE_WAVEPCI) /* bit  3 = 0x00000008 */ \
        WPP_DEFINE_BIT(PCTRACE_WAVECYC) /* bit  4 = 0x00000010 */ \
        WPP_DEFINE_BIT(PCTRACE_WAVERT)  /* bit  5 = 0x00000020 */ \
        WPP_DEFINE_BIT(PCTRACE_WAVETOPO)/* bit  6 = 0x00000040 */ \
        WPP_DEFINE_BIT(PCTRACE_WAVEDMUS)/* bit  7 = 0x00000080 */ \
        WPP_DEFINE_BIT(PCTRACE_POSITION)/* bit  8 = 0x00000100 */ \
        WPP_DEFINE_BIT(PCTRACE_DEVSTATE)/* bit  9 = 0x00000200 */ \
        WPP_DEFINE_BIT(PCTRACE_INIT)    /* bit 10 = 0x00000400 */ \
        )

#define WPP_LEVEL_FLAGS_LOGGER(level,flags) WPP_LEVEL_LOGGER(flags)
#define WPP_LEVEL_FLAGS_ENABLED(level, flags) (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= level)

// Include the wpp magic to support the advanced IFR recorder.
#include "WppRecorder.h"

#else   // PC_USE_ETW

#define WPP_INIT_TRACING(a,b)
#define WPP_CLEANUP(a)

typedef enum _DEBUG_LEVEL
{
    PCTRACE_PNP = 0,
    PCTRACE_POWER = 1,
    PCTRACE_IRPSTRM = 2,
    PCTRACE_WAVEPCI = 3,
    PCTRACE_WAVECYC = 4,
    PCTRACE_WAVERT = 5,
    PCTRACE_WAVETOPO = 6,
    PCTRACE_WAVEDMUS = 7,
    PCTRACE_POSITION = 8,
    PCTRACE_DEVSTATE = 9,
    PCTRACE_INIT = 10
} DEBUG_LEVEL, *PDEBUG_LEVEL;

#endif  // PC_USE_ETW

#endif  //_PCWPPTR_H_
