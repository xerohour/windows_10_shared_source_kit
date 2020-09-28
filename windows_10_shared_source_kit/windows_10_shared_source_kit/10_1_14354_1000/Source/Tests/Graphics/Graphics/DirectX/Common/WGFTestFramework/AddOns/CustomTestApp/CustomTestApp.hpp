//////////////////////////////////////////////////////////////////////
// File:  CustomTestApp.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a custom D3D/DXGI test app object.
//
// History:
// 10 Mar 2006  SEdmison  Factored from other tests into a shared lib.
//////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(_NTSTATUS_)
typedef long NTSTATUS;
#endif

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Win32 headers
//

#include <windows.h>
#include <mmsystem.h>
#include <powrprof.h>
#include <wtsapi32.h>


//
// Direct3D headers
//

#include <d3d10_1.h>
#include <dxgi.h>
#include <d3dx10.h>


//
// C runtime headers
//

#include <stdio.h>
#include <tchar.h>


//
// Framework headers
//

#include "WGFTestCore.h"
#include "WGFTestUI.h"


#include <strsafe.h>


/////////////////////////////////////////////////////////////////
//
// Constants
//
/////////////////////////////////////////////////////////////////

//
// Monitor power states
//

const LPARAM MONITOR_ON       = -1;
const LPARAM MONITOR_STANDBY  =  1;
const LPARAM MONITOR_OFF      =  2;


//
// Monitor slots
//

const UINT MONITOR_CENTER      = 0;
const UINT MONITOR_RIGHT       = 1;
const UINT MONITOR_LEFT        = 2;
const UINT MONITOR_SLOTS       = 3;
const UINT MONITOR_UNSPECIFIED = 4;


//
// Monitor slot descriptions
//

const LPCTSTR g_MonitorDescriptions[] =
{
	_T( "center" ),
	_T( "right" ),
	_T( "left" )
};


//
// Device driver model enumeration (ripped off from the d3d9framework).
//

enum DRIVERMODEL
{
	DRIVERMODEL_XPDM = 1,
	DRIVERMODEL_LDDM = 2
};


/////////////////////////////////////////////////////////////////
//
// Macros
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Macro:  DECODE_FLAG
//
// Purpose:
// Decodes a flag value into a string buffer.
///////////////////////////////////////////////////////
#define DECODE_FLAG( Variable, Flag, Padding, Delimiter ) \
	if ( ( (Variable) & (Flag) ) != 0 ) \
	{ \
		if ( bFirstFlag == TRUE ) \
		{ \
			bFirstFlag = FALSE; \
		} \
		else \
		{ \
			StringCchCatW \
			( \
				Buffer, \
				nBuffer, \
				Delimiter \
			); \
		}; \
		\
		if ( Padding != NULL && Padding[ 0 ] != L'\0' ) \
		{ \
			StringCchCatW \
			( \
				Buffer, \
				nBuffer, \
				Padding \
			); \
		}; \
		\
		StringCchCatW \
		( \
			Buffer, \
			nBuffer, \
			L ## #Flag \
		); \
	}


///////////////////////////////////////////////////////
// Macro:  HANDLE_ENUM
//
// Purpose:
// Adds a case to a switch case which handles a value
// and adds a return for the value.
//
// Additional information:
// Expands to code like the following:
//     case (WTSActive):
//     {
//         return "WTSActive";
//     } break
///////////////////////////////////////////////////////
#define HANDLE_ENUM_A(x) \
	case (x): \
	{ \
		return #x; \
	} break

#define HANDLE_ENUM_W(x) \
	case (x): \
	{ \
		return L ## #x; \
	} break

#define HANDLE_ENUM_T(x) \
	case (x): \
	{ \
		return _T( #x ); \
	} break


#define HANDLE_HRESULT HANDLE_ENUM_T


///////////////////////////////////////////////////////
// Macro:  HANDLE_WIN32_HRESULT
//
// Purpose:
// Expands to a block of code like the following:
//   case HRESULT_FROM_WIN32( ERROR_BAD_PIPE ):
//   {
//      return _T( "HRESULT_FROM_WIN32( " _T( "ERROR_BAD_PIPE" ) _T( " )" );
//   } break
///////////////////////////////////////////////////////
#define HANDLE_WIN32_HRESULT( x ) \
		case __HRESULT_FROM_WIN32( x ): \
		{ \
			return _T( "HRESULT_FROM_WIN32( " ) _T( #x ) _T( " )" ); \
		} break


/////////////////////////////////////////////////////////////////
//
// Type definitions
//
/////////////////////////////////////////////////////////////////

//
// DXGI.dll API prototypes
//

typedef HRESULT (WINAPI *PCREATEDXGIFACTORY)
(
	REFIID riid,
	void **ppFactory
);


//
// D3D10.dll API prototypes
//

typedef HRESULT (WINAPI *PD3D10CREATEDEVICE)
(
	IDXGIAdapter *pAdapter,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	UINT SDKVersion,
	ID3D10Device **ppDevice
);


//
// DWMAPI.dll API prototypes
//
// N.B. [SEdmison]:
// The actions are DWM_EC_ENABLECOMPOSITION and
// DWM_EC_DISABLECOMPOSITION from dwmapi.h.
//
typedef HRESULT (WINAPI *PDWMENABLECOMPOSITION)
(
	UINT CompositionAction
);


/////////////////////////////////////////////////////////////////
//
// Helper functions
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  DecodeGDIDeviceStateFlags
//
// Purpose:
// Decodes the StateFlags member of a DISPLAY_DEVICE
// structure.
///////////////////////////////////////////////////////
HRESULT DecodeGDIDeviceStateFlags
(
	DWORD StateFlags,
	LPCWSTR Padding,
	LPCWSTR Delimiter,
	__out_ecount(nBuffer) LPWSTR Buffer,
	UINT nBuffer
);


int ExceptionFilter
(
	unsigned int ExceptionCode,
	_EXCEPTION_POINTERS * pExceptionDetails
);


///////////////////////////////////////////////////////
// Function:  GetWTSStateName
//
// Purpose:
// Returns the name of the specified Terminal Services
// connection state.
///////////////////////////////////////////////////////
LPCSTR GetWTSStateName
(
	WTS_CONNECTSTATE_CLASS State
);


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CCustomTestApp
//
// Purpose:
// Custom test application object.
//
// Notes:
// Singleton application object derived from
// CWGFTestFramework.  Required by the WGF framework.
///////////////////////////////////////////////////////
template < typename ParentClass >
class CCustomTestApp
:
	public ParentClass
{
	//
	// Construction/destruction
	//

	public:
		CCustomTestApp< ParentClass >();

		//virtual ~CCustomTestApp();


	//
	// Public framework method overrides
	//

	public:
		// App-wide setup.
		bool Setup();

		// App-wide cleanup.
		void Cleanup();


	//
	// Public methods
	//

	public:
		// Driver model detection code
		DRIVERMODEL DetectDriverModelForDevice
		(
			IN LPCWSTR pDeviceName
		);

		HRESULT EnablePrivilege
		(
			IN LPCTSTR PrivilegeName
		);

		HRESULT ToggleDWMCompositionUI
		(
			IN BOOL bRestore
		);


	//
	// Data members
	//

	public:
		// Original foreground window lock timeout.
		DWORD m_OriginalForegroundWindowLockTimeout;


		//
		// DWM stuff
		//

		// DWM composition registry settings stuff.
		BOOL m_bCompositionUIValueFound;
		DWORD m_CompositionUIValue;
};
