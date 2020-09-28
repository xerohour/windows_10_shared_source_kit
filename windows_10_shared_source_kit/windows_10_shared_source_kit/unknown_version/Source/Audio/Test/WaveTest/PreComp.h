// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// File Name:
//
//  PreComp.h
//
// Abstract:
//
//  Precompiled common headers
//



// -------------------------------------------------------------------------------

#include <windows.h>
#include <basiclog.h>
#include <BasicLogHelper.h>
#include <setupapi.h>
#include <MobileCoreHelper.h>
#include <mmsystem.h>
#include <ks.h>
#include <ksmedia.h>
#include <kslib.h>
#include <atlbase.h>
#include <atlcom.h>
#include <strsafe.h>
#include "Helpers.h"
#include "DataIntersectionTest.h"

#define TEST_AMPLITUDE 0.01f
#define TEST_FREQUENCY 200.0f

#include <ilog.h>
#include <shlflags.h>
#include <basicprintf.h>
#include <wextestclass.h>
#include <resourcelist.h>

//#include "halfdup.h"
//#include "WaveTestTaef.h"

#define XLOG(xlogType, nLevel, szFormat, ...) BasicLogPrintf(g_pBasicLog, xlogType, nLevel, szFormat, __VA_ARGS__)
#define SLOG(nLevel, szFormat, ...) BasicLogPrintf(g_pBasicLog, XMSG, nLevel, szFormat, __VA_ARGS__)
