// Copyright(C) Microsoft. All rights reserved.

//
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#define INITGUID
#include <windows.h>

#define _ATL_NO_COM_SUPPORT

#pragma pack(push, 8)
#pragma warning(disable : 4786)
#pragma warning(disable : 4995)
#pragma warning(disable : 4244)


// TODO: fix
//#define ASSERT_REALTIME()
//#define ASSERT_NONREALTIME()

#include <mfidl.h>
#include <SmartPtr.h>
#include <audioenginebaseapo.h>
#include <audioenginecoreapo.h>

//WMI stuff
#include <initguid.h> 
#include <wmistr.h>
#include <evntrace.h>
#include <aewmistats.h>


//Helpers to use non compiler defined numeric types
#include <numconv.h>

using namespace std;
using namespace numeric_conversions;


//ATL stuff
#include <atlbase.h>
#include <atlcom.h>

class CMyModule : public CAtlExeModuleT<CMyModule>{} _AtlModule;

#include <atlcoll.h>
#include <atlsync.h>
#include <atlconv.h>


#include "resource.h"

#include <kslib.h>
#include <ksmedia.h>
#include <HRString.h>
#include <macros.h>
#include <MemoryEndpoint.h>
#include <ApoHelp.h>
#include <ProcessorHelp.h>
#include <GraphHelp.h>
#include <WmiHelpers.h>
#include <Module.h>
#include <TestDefs.h>

#include <DeviceGraph.h>
#include <Obsolete.h>
#include <ProjectUtils.h>
#include <AudioMediaTypeMMTest.h>
#include <WMIHelpers.h>

#include "TstFns.h"
#include "APISrcAPO.h"
#include "APIGenericAPO.h"
#include "Pump.h"
#include "MixerAPO.h"
#include "VolumeAPO.h"
#include "MatrixAPO.h"
#include "BaseFormat.h"
#include "FormatFl32Int16.h"
#include "FormatInt16Fl32.h"
#include "FormatInt24Fl32.h"
#include "FormatFl32Int24.h"
#include "FormatFl32Int32.h"
#include "FormatInt32Fl32.h"
#include "FormatInt32Fl64.h"
#include "FormatFl64Int32.h"
#include "FormatInt16Fl64.h"
#include "FormatFl64Int16.h"
#include "FormatInt24wideFl32.h"
#include "FormatFl32Int24wide.h"
#include "FormatFl32Uint8.h"
#include "FormatUint8Fl32.h"
#include "FormatInt20Fl32.h"
#include "FormatFl32Int20.h"
#include "MeterAPO.h"
#include "KSEndpointBase.h"
#include "KSEndpointCapture.h"
#include "KSEndpointRender.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
