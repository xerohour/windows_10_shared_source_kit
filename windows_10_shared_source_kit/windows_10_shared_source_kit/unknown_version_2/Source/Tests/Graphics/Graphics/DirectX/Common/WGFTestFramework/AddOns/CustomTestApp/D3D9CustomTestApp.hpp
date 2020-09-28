//////////////////////////////////////////////////////////////////////
// File:  D3D9CustomTestApp.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for Direct3D 9.L custom test app object.
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
// Project headers
//

#include "CustomTestApp.hpp"


//
// Direct3D headers
//

#include <d3d9.h>
#include <d3dkmthk.h>
#include <d3dukmdt.h>
#include <d3dumddi.h>


/////////////////////////////////////////////////////////////////
//
// Helper functions
//
/////////////////////////////////////////////////////////////////

float GetDXBuild();


///////////////////////////////////////////////////////
// Function:  ToString( D3DDEVTYPE )
//
// Purpose:
// Converts a D3DDEVTYPE to a string.
///////////////////////////////////////////////////////
tstring __stdcall ToString
(
	const D3DDEVTYPE & DeviceType
);


///////////////////////////////////////////////////////
// Function: FromString( D3DDEVTYPE *, const tstring )
//
// Purpose:
// Converts a string to a D3DDEVTYPE.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	__out D3DDEVTYPE * pType,
	const tstring & str
);


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CD3D9CustomTestApp
//
// Purpose:
// D3D 9 custom test application object.
//
// Notes:
// Singleton application object derived from
// CWGFTestFramework.  Required by the WGF framework.
///////////////////////////////////////////////////////
class CD3D9CustomTestApp
:
	public CCustomTestApp< CWGFTestFramework >
{
	//
	// Construction/destruction
	//

	public:
		// Constructor.
		CD3D9CustomTestApp();

		// Destructor.
		//virtual ~CD3D9CustomTestApp();


	//
	// Public framework method overrides
	//

	public:
		// Registers option variables.
		virtual void InitOptionVariables();

		// Updates device information for the log.
		virtual bool UpdateCurrentDeviceInfo
		(
			__in DXGLOG_DEVICEINFO * pDeviceInfo
		);

		bool IsTerminalServices() { return ( GetSystemMetrics( SM_REMOTESESSION ) != 0 ); };


	//
	// Member data
	//

	public:
		// Enable D3D 9 debug helper spew?
		bool m_bDebugHelper;

		// Device type
		D3DDEVTYPE m_DeviceType;
};
