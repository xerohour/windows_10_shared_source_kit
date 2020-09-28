//--------------------------------------------------------------------------
//
//  File: Common.h
//
//  Copyright (C) Microsoft. All rights reserved.
//
//  Abstract:
//      Common header.
//
//
//--------------------------------------------------------------------------
#pragma once

// Header files for Shell98
#include <windows.h>
#include <tchar.h>

#ifndef BUILD_TAEF
#include <shlflags.h>
#include <shltypes.h>
#include <shell98.h>
#include <module98.h>
#else
#include <module_taef.h>
#endif

#include <dvmodule.h>
#include <TaefModuleAdapter.h>

#include "resource.h"
#include <BasicLogHelper.h>

DEFINE_GUID(GUID_NULL, 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

#include <atlbase.h>
#include <atlcoll.h>    // For the CAtlList template class
#include <atlconv.h>
#include <StrSafe.h>

#include <MMDeviceAPI.h>
#include <avendpointkeys.h>
#include <functiondiscoverykeys.h>

#include "AudioLogo.h"
#include "TestCases.h"

// Trace redefinition
#define LOG_LEVEL_VERBOSE       eInfo2
#define LOG_LEVEL_TERSE         eInfo1
#define LOG_LEVEL_ERROR         eError

#ifndef BUILD_TAEF
#define LOG(...)     g_IShell->XLog(XMSG,  LOG_LEVEL_TERSE, __VA_ARGS__)
#define ERR(...)     g_IShell->XLog(XFAIL, LOG_LEVEL_ERROR, __VA_ARGS__)
#define SKIP(...)    g_IShell->XLog(XSKIP, LOG_LEVEL_ERROR, __VA_ARGS__)
#define XLOG         g_IShell->XLog
#else
#define LOG(...)     XLOG(XMSG,  LOG_LEVEL_TERSE, __VA_ARGS__)
#define ERR(...)     XLOG(XFAIL, LOG_LEVEL_ERROR, __VA_ARGS__)
#define SKIP(...)    XLOG(XSKIP, LOG_LEVEL_ERROR, __VA_ARGS__)
#endif

// Debugging/To Do stuff
#define __PRAGMSG(l,x,c) message(__FILE__"("#l") : "c": "x)
#define _WARN(l,x) __PRAGMSG(l,x, "warning")
#define _WARNMSG(x) _WARN(__LINE__,x)

#define _FIX(l,x) __PRAGMSG(l,x, "fix")
#define _FIXMSG(x) _FIX(__LINE__,x)

#define SAFE_CLOSE_HANDLE(h) CloseHandle(h); h = NULL;

#ifdef FORCE_MSG_ERRORS
#define WARNMSG(x) _WARNMSG(x)
#define FIXMSG(x) _FIXMSG(x)
#else
#define WARNMSG(x) message("\n"x"\n")
#define FIXMSG(x)   message("\n"x"\n")
#endif

extern CAudioLogoDeviceTestModule  *g_pAudioLogoDeviceTestModule;
#define g_pModule g_pAudioLogoDeviceTestModule

// {2013DBB2-2F76-4B2C-950A-0C9DFAC62398}
DEFINE_GUID(GUID_AEGLITCH, 0x2013DBB2, 0x2F76, 0x4B2C, 0x95, 0x0A, 0x0C, 0x9D, 0xFA, 0xC6, 0x23, 0x98);

#ifndef _PREFAST_
#pragma warning(disable:4068)
#endif

#define TEST_FREQUENCY 1000
#define TEST_AMPLITUDE 0.01f // -40 dB FS
