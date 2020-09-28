//////////////////////////////////////////////////////////////////////
// File:  GraphicsTest.h
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a graphics test framework base class.  This class holds
// data that's common between DXGI and D3D test framework derived classes.
//
// History:
// 16 Feb 2008  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Build flags
//
/////////////////////////////////////////////////////////////////

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
#include <basetsd.h>


//
// DirectX headers
//

#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>


//
// Framework headers
//

#include <WGFTestCore.h>
#include "GraphicsDeviceManager.h"


// Magic constant from a private header until I can get it published
// into a header that I can access.
#define D3D10_CREATE_DEVICE_REMOTE_FORCE_REF  0x0100

#ifdef FORCE_D3D10_REF
#define D3D_REF_DLLNAME "d3d10ref.dll"
#else
#define D3D_REF_DLLNAME "d3d11ref.dll"
#endif
//////////////////////////////////////////////////////////////////////////
// CGraphicsTestFramework
//
// Base class for graphics test applications.
//
// NOTE:
// Requires all test group classes be derived from CD3DTest.
//////////////////////////////////////////////////////////////////////////
class CGraphicsTestFramework
:
	public CWGFTestFramework
{
	//
	// Construction/destruction
	//

public:
	CGraphicsTestFramework
	(
	);

	virtual ~CGraphicsTestFramework();

	//
	// Accessors
	//
	virtual CGraphicsDeviceManager *GetGraphicsDeviceManager() const = 0;


	//
	// Public methods
	//

public:
	bool IsD3D11On12Device( IUnknown *pDevice ) const {return GetGraphicsDeviceManager()->IsD3D11On12Device(pDevice); }
	bool IsWARPAdapter( IDXGIAdapter *pAdapter ) const  {return GetGraphicsDeviceManager()->IsWARPAdapter(pAdapter); } 
	bool IsREFAdapter( IDXGIAdapter *pAdapter ) const   {return GetGraphicsDeviceManager()->IsREFAdapter(pAdapter); } 

	bool IsSession0() { return GetGraphicsDeviceManager()->IsSession0Mode(); }
	void InitiateJournalDump(bool bWriteToLog);
	static void WriteDXGIJournalToLog(const char *pMessage);
	static bool s_mbDumpToDXGLogger;
};
