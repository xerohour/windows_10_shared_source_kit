//////////////////////////////////////////////////////////////////////
// File:  DXGITestFramework.h
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a DXGI test framework base class.
//
// History:
// 11 Sep 2007  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

#pragma once


/////////////////////////////////////////////////////////////////
//
// Macros
//
/////////////////////////////////////////////////////////////////


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
// C runtime headers
//

#include <stdio.h>


//
// STL headers
//

#include <vector>
#include <algorithm>


//
// Direct3D headers
//

#include <d3d11_1.h>
#include <d3d11.h>
#include <d3d10_1.h>
#include <d3dx10.h>

#include <dwmapi.h>
#include <dwmapip.h>


//
// Framework headers
//

#include <WGFTestCore.h>
#include <GraphicsTest.h>


/////////////////////////////////////////////////////////////////
//
// Type definitions
//
/////////////////////////////////////////////////////////////////

#define ADAPTER_UNDEFINED ((UINT) - 1)

///////////////////////////////////////////////////////
// Enum:  TEST_FEATURE_LEVEL
//
// Purpose:
// Expresses a feature level against which a test wants
// to be run.
///////////////////////////////////////////////////////
enum TEST_FEATURE_LEVEL
{
	//
	// Direct3D 10.0
	//

	// Direct3D 10.0
	TEST_FEATURE_10_0_LEVEL_10_0             =  0,


	//
	// Direct3D 10.1
	//

	// Direct3D 10.1 unspecified feature level
	TEST_FEATURE_10_1_LEVEL_UNSPECIFIED      =  1,

	// Direct3D 10.1 feature levels 9.x
	TEST_FEATURE_10_1_LEVEL_9_1              =  2,
	TEST_FEATURE_10_1_LEVEL_9_2              =  3,
	TEST_FEATURE_10_1_LEVEL_9_3              =  4,
	TEST_FEATURE_10_1_LEVEL_9_BESTAVAILABLE  =  5,

	// Direct3D 10.1 feature levels 10.x
	TEST_FEATURE_10_1_LEVEL_10_0             =  6,
	TEST_FEATURE_10_1_LEVEL_10_1             =  7,
	TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE =  8,


	//
	// Direct3D 10.*
	//

	// Direct3D 10.* best available feature level
	TEST_FEATURE_10_ALL_LEVEL_BESTAVAILABLE  =  9,


	//
	// Direct3D 11.0
	//

	// Direct3D 11.0 unspecified feature level
	TEST_FEATURE_11_0_LEVEL_UNSPECIFIED      = 10,

	// Direct3D 11.0 feature levels 9.x
	TEST_FEATURE_11_0_LEVEL_9_1              = 11,
	TEST_FEATURE_11_0_LEVEL_9_2              = 12,
	TEST_FEATURE_11_0_LEVEL_9_3              = 13,
	TEST_FEATURE_11_0_LEVEL_9_BESTAVAILABLE  = 14,

	// Direct3D 11.0 feature levels 10.x
	TEST_FEATURE_11_0_LEVEL_10_0             = 15,
	TEST_FEATURE_11_0_LEVEL_10_1             = 16,
	TEST_FEATURE_11_0_LEVEL_10_BESTAVAILABLE = 17,

	// Direct3D 11.0 feature levels 11.x
	TEST_FEATURE_11_0_LEVEL_11_0             = 18,
	TEST_FEATURE_11_0_LEVEL_11_BESTAVAILABLE = 19,


	//
	// Direct3D 11.*
	//

	// Direct3D 11.* best available feature level
	TEST_FEATURE_11_ALL_LEVEL_BESTAVAILABLE  = 20,


	//
	// All versions
	//

	TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE     = 21,

	TEST_FEATURE_COUNT                       = 22
};


///////////////////////////////////////////////////////
// Struct:  D3D_OPTIONS
//
// Purpose:
// Tracks Direct3D-related settings for a DXGI test.
//
// Additional information:
// Stolen from the version in D3DTest.h, and then cut
// down.
///////////////////////////////////////////////////////
struct DXGI_D3D_OPTIONS : BASE_D3D_OPTIONS
{
	bool SrcOnly;
	D3D10_DRIVER_TYPE RefDriverType;
	union
	{
		D3D10_DRIVER_TYPE DriverType;
		D3D10_DRIVER_TYPE SrcDriverType;
	};
	bool bStricterValidationLayer;
};


typedef HRESULT (WINAPI *PCREATEDXGIFACTORY)
(
	REFIID riid,
	void ** ppFactory
);


//
// Direct3D 10/10.1 pointer union
//

typedef union
{
	ID3D10Device * pDevice;
	ID3D10Device1 * pDevice1;
} D3D10_DEVICE_POINTER;


//
// Direct3D 11 pointer union
//
// @N.B. [SEdmison]:
// Admittedly, this one isn't too interesting at the moment.
// But it could/would/will get more interesting once an 11.1
// feature level comes online.  In any case, it was cleaner
// and more straightforward to structure the code in terms
// of these unions throughout.
//


/////////////////////////////////////////////////////////////////
//
// Helper functions
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  GetD3DDriverTypeForD3D10DriverType
//
// Purpose:
// Returns the D3D_DRIVER_TYPE value that most closely
// corresponds to a given D3D10_DRIVER_TYPE value.
///////////////////////////////////////////////////////
D3D_DRIVER_TYPE GetD3DDriverTypeForD3D10DriverType
(
	D3D10_DRIVER_TYPE DriverType
);


///////////////////////////////////////////////////////
// Function:  FromString
//
// Purpose:
// Parses a tstring into a DXGI_MODE_ROTATION enum.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	DXGI_MODE_ROTATION * pRotation,
	const tstring & RotationString
);


///////////////////////////////////////////////////////
// Function:  ToString( DXGI_MODE_ROTATION )
//
// Purpose:
// Returns a tstring name for a
// DXGI_MODE_ROTATION enum value.
///////////////////////////////////////////////////////
tstring __stdcall ToString
(
	const DXGI_MODE_ROTATION & Rotation
);


///////////////////////////////////////////////////////
// Function:  FromString
//
// Purpose:
// Parses a tstring into a DXGI_MODE_SCANLINE_ORDER enum.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	DXGI_MODE_SCANLINE_ORDER * pScanlineOrdering,
	const tstring & ScanlineOrderingString
);


///////////////////////////////////////////////////////
// Function:  ToString( DXGI_MODE_SCANLINE_ORDER )
//
// Purpose:
// Returns a tstring name for a
// DXGI_MODE_SCANLINE_ORDER enum value.
///////////////////////////////////////////////////////
tstring __stdcall ToString
(
	const DXGI_MODE_SCANLINE_ORDER & ScanlineOrdering
);


///////////////////////////////////////////////////////
// Function:  FromString
//
// Purpose:
// Parses a tstring into a DXGI_MODE_SCALING enum.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	DXGI_MODE_SCALING * pScaling,
	const tstring & ScalingString
);


///////////////////////////////////////////////////////
// Function:  ToString( DXGI_MODE_SCALING )
//
// Purpose:
// Returns a tstring name for a DXGI_MODE_SCALING enum
// value.
///////////////////////////////////////////////////////
tstring __stdcall ToString
(
	const DXGI_MODE_SCALING & Scaling
);


///////////////////////////////////////////////////////
// Function:  FromString
//
// Purpose:
// Parses a tstring into a D3D10_DRIVER_TYPE enum.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	D3D10_DRIVER_TYPE * pDriverType,
	const tstring & DriverTypeString
);


///////////////////////////////////////////////////////
// Function:  ToString( D3D10_DRIVER_TYPE )
//
// Purpose:
// Returns a tstring name for a D3D10_DRIVER_TYPE
// enum value.
///////////////////////////////////////////////////////
tstring __stdcall ToString
(
	const D3D10_DRIVER_TYPE & DriverType
);


///////////////////////////////////////////////////////
// Function:  FromString
//
// Purpose:
// Parses a tstring into a D3D10_FEATURE_LEVEL1 enum.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	D3D10_FEATURE_LEVEL1 * pFeatureLevel,
	const tstring & FeatureLevelString
);


///////////////////////////////////////////////////////
// Function:  ToString( D3D10_FEATURE_LEVEL1 )
//
// Purpose:
// Returns a tstring name for a D3D10_FEATURE_LEVEL1
// enum value.
///////////////////////////////////////////////////////
tstring __stdcall ToString
(
	const D3D10_FEATURE_LEVEL1 & TestFeatureLevel
);


///////////////////////////////////////////////////////
// Function:  FromString
//
// Purpose:
// Parses a tstring into a TEST_FEATURE_LEVEL enum.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	TEST_FEATURE_LEVEL * pTestFeatureLevel,
	const tstring & TestFeatureLevelString
);


///////////////////////////////////////////////////////
// Function:  ToString( TEST_FEATURE_LEVEL )
//
// Purpose:
// Returns a tstring name for a TEST_FEATURE_LEVEL enum
// value.
///////////////////////////////////////////////////////
tstring __stdcall ToString
(
	const TEST_FEATURE_LEVEL & TestFeatureLevel
);


///////////////////////////////////////////////////////
// Function:  FromString
//
// Purpose:
// Parses a tstring into a TEST_DXGI_VERSION enum.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	TEST_DXGI_VERSION * pTestDXGIVersion,
	const tstring & TestDXGIVersionString
);


///////////////////////////////////////////////////////
// Function:  ToString( TEST_DXGI_VERSION )
//
// Purpose:
// Returns a tstring name for a TEST_DXGI_VERESION enum
// value.
///////////////////////////////////////////////////////
tstring __stdcall ToString
(
	const TEST_DXGI_VERSION & TestDXGIVersion
);


///////////////////////////////////////////////////////
// Function:  GetSDKVersionForTestFeatureLevel
//
// Purpose:
// Returns an SDK version for a corresponding
// TEST_FEATURE_LEVEL enum value.
///////////////////////////////////////////////////////
UINT GetSDKVersionForTestFeatureLevel
(
	TEST_FEATURE_LEVEL TestFeatureLevel
);


///////////////////////////////////////////////////////
// Function:  GetD3D10FeatureLevelForTestFeatureLevel
//
// Purpose:
// Returns a D3D10_FEATURE_LEVEL1 enum value for a
// corresponding TEST_FEATURE_LEVEL enum value.
///////////////////////////////////////////////////////
D3D10_FEATURE_LEVEL1 GetD3D10FeatureLevelForTestFeatureLevel
(
	TEST_FEATURE_LEVEL TestFeatureLevel
);


///////////////////////////////////////////////////////
// Function:  GetD3DFeatureLevelForTestFeatureLevel
// Purpose:
// Returns a D3D_FEATURE_LEVEL enum value for a
// corresponding TEST_FEATURE_LEVEL enum value.
///////////////////////////////////////////////////////
D3D_FEATURE_LEVEL GetD3DFeatureLevelForTestFeatureLevel
(
	TEST_FEATURE_LEVEL TestFeatureLevel
);

///////////////////////////////////////////////////////
// Function:  IsFeatureLevel9
//
// Purpose:
// Returns true if TEST_FEATURE_LEVEL is level 9.1 - 9.3
///////////////////////////////////////////////////////

bool IsFeatureLevel9
(
	TEST_FEATURE_LEVEL TestFeatureLevel
);

/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CDXGITestFramework
//
// Purpose:
// Test framework object for DXGI tests.
///////////////////////////////////////////////////////
class CDXGITestFramework
:
	public CGraphicsTestFramework
{
	//
	// Construction/destruction
	//

	public:
		// Constructor.
		CDXGITestFramework();

		// Destructor.
		virtual ~CDXGITestFramework();


	//
	// Framework oberride methods
	//

	public:
		virtual void InitOptionVariables();

		virtual bool UpdateCurrentDeviceInfo
		(
			__out DXGLOG_DEVICEINFO * pDeviceInfo
		);


	//
	// Public methods
	//

	public:
		virtual HRESULT CreateAdapter
		(
			DXGI_FACTORY_POINTER FactoryPointer,
			UINT AdapterOrdinal,
			__out DXGI_ADAPTER_POINTER * pAdapterPointer
		);


		virtual HRESULT CreateFactoryAdapter
		(
			__inout DXGI_FACTORY_POINTER * ppFactory,
			UINT AdapterOrdinal,
			__out DXGI_ADAPTER_POINTER * ppAdapter
		);


		// CreateDevice specializations that return an ID3D10Device pointer.
		virtual HRESULT CreateDevice
		(
			IDXGIAdapter * pAdapter,
			D3D10_DRIVER_TYPE DriverType,
			HMODULE hSoftware,
			UINT Flags,
			TEST_FEATURE_LEVEL TestFeatureLevel,
			UINT SDKVersion,
			ID3D10Device ** ppDevice,
			bool bIgnoreRequestedFeatureLevels = false,
			bool bAutomaticallyLoadSoftwareRasterizer = true
		);

		virtual HRESULT CreateDevice
		(
			IDXGIAdapter * pAdapter,
			HMODULE hSoftware,
			UINT Flags,
			TEST_FEATURE_LEVEL TestFeatureLevel,
			UINT SDKVersion,
			ID3D10Device ** ppDevice,
			bool bIgnoreRequestedFeatureLevels = false,
			bool bAutomaticallyLoadSoftwareRasterizer = true
		);


		// CreateDevice specializations that return an ID3D10Device1 pointer.
		virtual HRESULT CreateDevice
		(
			IDXGIAdapter * pAdapter,
			D3D10_DRIVER_TYPE DriverType,
			HMODULE hSoftware,
			UINT Flags,
			TEST_FEATURE_LEVEL TestFeatureLevel,
			UINT SDKVersion,
			ID3D10Device1 ** ppDevice,
			bool bIgnoreRequestedFeatureLevels = false,
			bool bAutomaticallyLoadSoftwareRasterizer = true
		);

		virtual HRESULT CreateDevice
		(
			IDXGIAdapter * pAdapter,
			HMODULE hSoftware,
			UINT Flags,
			TEST_FEATURE_LEVEL TestFeatureLevel,
			UINT SDKVersion,
			ID3D10Device1 ** ppDevice,
			bool bIgnoreRequestedFeatureLevels = false,
			bool bAutomaticallyLoadSoftwareRasterizer = true
		);


		virtual HRESULT CreateFactoryAdapterDevice
		(
			__inout DXGI_FACTORY_POINTER * ppFactory,
			UINT AdapterOrdinal,
			__inout DXGI_ADAPTER_POINTER * ppAdapter,
			D3D10_DRIVER_TYPE DriverType,
			UINT Flags,
			TEST_FEATURE_LEVEL TestFeatureLevel,
			UINT SDKVersion,
			D3D10_DEVICE_POINTER * ppDevice,
			bool bIgnoreRequestedFeatureLevels = false,
			bool * pbAdapterIsRemote = NULL
		);


	//
	// Static helper methods
	//

	public:
		static void OptionCallback_DXGIVersion
		(
			const tstring &DXGIVersion
		);

		static void OptionCallback_FeatureLevel
		(
			const tstring &FeatureLevel
		);

		static void OptionCallback_RasterizerDLL
		(
			const tstring &RasterizerDLL
		);

		static void OptionCallback_SourceDriverType
		(
			const tstring &SourceDriverType
		);

		static void OptionCallback_TerminalServices
		(
			const tstring &DXGIVersion
		);


	//
	// Member data
	//

	public:
		bool m_bRegisterFeatureLevel;
		bool m_RequestedFeatureLevels[ TEST_FEATURE_COUNT ];

		bool m_bRegisterDriverType;
		DXGI_D3D_OPTIONS m_D3DOptions;

		bool m_bLogDebug;


	//
	// Direct3D 10.1 module handle and entry points.
	//

	public:
		HMODULE m_hD3D10_1;
		PFN_D3D10_CREATE_DEVICE1 m_pD3D10CreateDevice1;
		PFN_D3D10_CREATE_DEVICE_AND_SWAP_CHAIN1 m_pD3D10CreateDeviceAndSwapChain1;


	//
	// Direct3D 11 compatibility layer module handle and entry points.
	//

	private:
		HMODULE m_hD3D11Layer;
		PFN_D3D11_CREATE_DEVICE m_pD3D11LayerCreateDevice;
		PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN  m_pD3D11LayerCreateDeviceAndSwapChain;
		HRESULT LoadD3D11RuntimeLayer();
		HRESULT GetAdapter(__in D3D10_DEVICE_POINTER *ppDevice, __out DXGI_ADAPTER_POINTER &AdapterPointer);
		static void OptionCallback_AdapterID( const UINT &adapterID );
};
