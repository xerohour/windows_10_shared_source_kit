// Copyright (C) Microsoft. All rights reserved.
#pragma once

// Header files for Shell98
#include <tchar.h>
#include <windows.h>

#ifndef BUILD_TAEF
#include <shlflags.h>
#include <shell98.h>
#include <module98.h>
#else
#include <module_taef.h>
#endif

#include <dvmodule.h>
#include <TaefModuleAdapter.h>

//#include "ksimport.h"
#include "resource.h"
//#include "tlist.h"

// Header files for KS specific functions

//  Some of our public headers (ks.h, ksmedia.h, mmsystem.h) use
//    non-standard extensions (Bug#1982168):
//      4201: nameless struct/union
//      4214: bit field types other than int
//  Will remove when headers are fixed.
#pragma warning (disable:4201 4214)
#include <ks.h>
#pragma warning (default:4201 4214)
#include <devioctl.h>
#include <mmreg.h>
//#include <ksmediap.h>
//#include <MMDeviceAPI.h>
//#include <MMDeviceAPIP.h>

// Test Support Headers
#include <strsafe.h>
#include <BasicLog.h>
#include <BasicPrintf.h>
#include <BasicLogHelper.h>

// we are using the old header
// so we need ksmedia.h
#if defined(_KSMEDIAP_H) 
#include <ksmedia.h>
#endif

#include <atlbase.h>
#include <atlcoll.h>    // For the CAtlList template class
#include <atlconv.h>
#include <stdio.h>

#include <mmsystem.h>

#include <MMDeviceAPI.h>
#include <MMDeviceAPIp.h>
#include <DeviceTopology.h>
#include <DeviceTopologyp.h>
#include <AudioEngineBaseAPO.h>
#include <AudioEngineCoreAPO.h>
#include <mfapi.h>
#include <AVEndpointKeys.h>
#include <AudioCoreAPO32Params.h>
#include <AudioEngineCore.h>

// Test libraries.
#include <nan.h>
#include <signal-source.h>
#include <dithering.h>
#include <sine-signal-source.h>

// Include project specific headers here
#include "base.h"
#include "ExtSysFxTest.h"
#include "TestCases.h"

// Trace redefinition
#define LOG_LEVEL_VERBOSE		eInfo2
#define LOG_LEVEL_TERSE     	eInfo1
#define LOG_LEVEL_ERROR     	eError

#ifndef ARRAYSIZE
#define ARRAYSIZE(a)      (sizeof(a)/sizeof(a[0]))
#endif
#define SAL_TERMINATE(a)  a[sizeof(a)/sizeof(a[0]) - 1] = 0

// Debugging/To Do stuff
#define __PRAGMSG(l,x,c) message(__FILE__"("#l") : "c": "x)
#define _WARN(l,x) __PRAGMSG(l,x, "warning")
#define _WARNMSG(x) _WARN(__LINE__,x)

#define _FIX(l,x) __PRAGMSG(l,x, "fix")
#define _FIXMSG(x) _FIX(__LINE__,x)

#ifdef FORCE_MSG_ERRORS
#define WARNMSG(x) _WARNMSG(x)
#define FIXMSG(x) _FIXMSG(x)
#else
#define WARNMSG(x) message("\n"x"\n")
#define FIXMSG(x)   message("\n"x"\n")
#endif

template <typename T>
inline void SAFE_DELETE(T* &p)
{
    if(NULL != p)
    {
        delete p;
        p = NULL;
    }
}

template <typename T>
inline void SAFE_DELETE_ARRAY(T* &p)
{
    if(NULL != p)
    {
        delete[] p;
        p = NULL;
    }
}

extern CAPODeviceTestModule        *g_pAPODeviceTestModule;
#define g_pModule g_pAPODeviceTestModule

#ifndef _PREFAST_
#pragma warning(disable:4068)
#endif

