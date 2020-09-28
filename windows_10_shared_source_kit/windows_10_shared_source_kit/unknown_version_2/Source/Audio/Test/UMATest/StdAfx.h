// Copyright(C) Microsoft. All rights reserved.

//
// StdAfx.h
//
// Description:
//
//  Standard Includes
//

#pragma once

#include <windows.h>
#include <tchar.h>
#include <shlflags.h>
#include <shltypes.h>

#ifdef BUILD_TAEF
#include <module_taef.h>
#else // !BUILD_TAEF
#include <shell98.h>
#include <module98.h>
#endif // !BUILD_TAEF
#include <dvmodule.h>

// TaefModuleAdapter includes definitions used in non-TAEF build
#include <TaefModuleAdapter.h>

#pragma message( "stdafx.h compiling ... " )
#define _ATL_NO_COM_SUPPORT
#define INIT_GUID

//#include <coguid.h>
//#include <initguid.h>

#pragma pack(push, 8)
#pragma warning(disable : 4786)
#pragma warning(disable : 4995)

#define ASSERT_REALTIME()
#define ASSERT_NONREALTIME()

#include <stdlib.h>
#include <objbase.h>
#include <assert.h>

//Version Info stuff
#include <winver.h>
#include <wmcodecdsp.h>

// get the header in to prevent further inclusion
#include <propidl.h>
#include <propkeydef.h>
#include <AudioMediaType.h>

#include <ks.h>
#include <ksmedia.h>
#include <kslib.h>

// Helpers to use non compiler defined numeric types
#include <numconv.h>

//using namespace std;
using namespace numeric_conversions;

// ATL stuff
#include <atlbase.h>
#include <atlcom.h>

class CMyModule : public CAtlExeModuleT<CMyModule>{} _AtlModule;

#include <atlcoll.h>
#include <atlsync.h>
#include <atlconv.h>

// Test Support Headers
#include "resource.h"
#include <ILog.h>
#include <BasicLogHelper.h>
#include <math.h>
#include <macros.h>
#include <HRString.h>

// Test Case Function Prototypes
#include "TestFunctions.h"

// Audio Core Headers
#include <AudioPolicy.h>
#include <AudioPolicyP.h>
#include <AudioClient.h>
#include <AudioClientP.h>
#include <EndpointVolume.h>
#include <MMDeviceApi.h>
#include <MMDeviceApiP.h>

#include <mmsystem.h>
#include <dsound.h>

// Module utility library
#include <EndpointModule.h>

#include "AudioServiceUtil.h"