//
// Copyright(C) Microsoft.All rights reserved.
//
// StdAfx.h
//
// Description:
//
//  Standard Includes
//

#pragma once
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <tchar.h>

#ifndef BUILD_TAEF
#include <shell98.h>
#include <module98.h>
#else
#include <module_taef.h>
#endif

#pragma message( "stdafx.h compiling ... " )
#define _ATL_NO_COM_SUPPORT

#pragma pack(push, 8)
#pragma warning(disable : 4786)
#pragma warning(disable : 4995)

#include <dvmodule.h>
#include <TaefModuleAdapter.h>

//Version Info stuff
//#include <winver.h>

// ATL stuff
#include <atlbase.h>
#include <atlcom.h>
// No need to declare/define ATL Module class

// Test Support Headers
#include "resource.h"
#include <strsafe.h>//
#include <BasicLog.h>//
#include <BasicPrintf.h>//
#include <BasicLogHelper.h>//
#include "utilities.h"

// Test Streaming
#include <audiostreaming.h>

// Test Case Function Prototypes
#include "TestFunctions.h"

// Audio Core Headers
#include <MMDeviceApi.h>
#include <MMDeviceApiP.h>
#include <mmsystem.h>

#include <helperclasses/wfx.h>
#include <waveio.h>
#include <hrstring.h>

//sleep resume library
#include <SleepResume.h>
enum { SYNC, ASYNC };
enum { SLEEP_STATE_S1 = 1, SLEEP_STATE_S2, SLEEP_STATE_S3, SLEEP_STATE_S4 }; 
enum { TEST_INVALID, TEST_PLAYBACK, TEST_CAPTURE };

// mutually exclusive - used in lpUser member of DV_TCINFO
#define F_S1    0x00000001
#define F_S2    0x00000002
#define F_S3    0x00000003
#define F_S4    0x00000004


#include "Lullaby.h"

extern CLullabyDeviceTestModule     *g_pLullabyDeviceTestModule;
#define g_pModule g_pLullabyDeviceTestModule
