//////////////////////////////////////////////////////////////////////
// File:  DXGICustomTest.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for custom D3D/DXGI test app object.
//
// History:
// 10 Mar 2006  SEdmison  Factored from other tests into a shared lib.
//////////////////////////////////////////////////////////////////////

#pragma once

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

//
// Project headers
//

#include <DXGITest.h>
#include "CustomTestApp.hpp"

//
// Direct3D headers
//

#include <d3d10_1.h>
#include <dxgi.h>
#include <d3dx10.h>
#include <d3d9.h>
#include <d3dkmthk.h>
#include <d3dukmdt.h>
#include <d3dumddi.h>


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


//
// Win32 headers
//

#include <strsafe.h>


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CDXGICustomTestApp
//
// Purpose:
// DXGI custom test application object.
//
// Notes:
// Adds functionality for initializing DXGI/D3D10 and
// correlating DXGI adapters to GDI display devices.
///////////////////////////////////////////////////////
class CDXGICustomTestApp
:
	public CCustomTestApp< CDXGITestFramework >
{
	//
	// Construction/destruction
	//

	public:
		CDXGICustomTestApp();

		CDXGICustomTestApp
		(
			D3D10_DRIVER_TYPE DriverType
		);
};
