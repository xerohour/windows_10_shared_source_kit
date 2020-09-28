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


#include <oacr.h>

OACR_WARNING_PUSH
OACR_WARNING_DISABLE(28727, "")

#include <kslib.h>

#include <atlbase.h>
#include <atlcom.h>

#include <strsafe.h>

#include <signal-source.h>
#include <dithering.h>
#include <sine-signal-source.h>
#include <WaveFormatEnum.h>

#include <ilog.h>
#include <shlflags.h>
#include <basicprintf.h>
#include <wextestclass.h>
#include <resourcelist.h>
#include <testdevice.h>
#include <testdevnode.h>
#include <audiodevnodeset.h>
#include "halfdup.h"
#include "KsPosTestTaef.h"
#include <hrstring.h>

#define XLOG(xlogType, nLevel, szFormat, ...) BasicLogPrintf(g_pBasicLog, xlogType, nLevel, szFormat, __VA_ARGS__)
#define SLOG(nLevel, szFormat, ...) BasicLogPrintf(g_pBasicLog, XMSG, nLevel, szFormat, __VA_ARGS__)

typedef long NTSTATUS;

#define NT_SUCCESS(Status)          (((NTSTATUS)(Status)) >= 0)

extern "C" __kernel_entry NTSYSCALLAPI NTSTATUS NTAPI NtSetTimerResolution (
    _In_ ULONG DesiredTime,     _In_ BOOLEAN SetResolution,     _Out_ PULONG ActualTime  );

// 1 ms timer resolution
static ULONG timerResolution = (1 * 10000);
static ULONG actualTimerResolution;
