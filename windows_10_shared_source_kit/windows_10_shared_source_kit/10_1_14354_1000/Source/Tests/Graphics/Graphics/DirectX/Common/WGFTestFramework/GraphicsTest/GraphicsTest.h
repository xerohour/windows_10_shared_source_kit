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


// Magic constant from a private header until I can get it published
// into a header that I can access.
#define D3D10_CREATE_DEVICE_REMOTE_FORCE_REF  0x0100

#ifdef FORCE_D3D10_REF
#define D3D_REF_DLLNAME L"d3d10ref.dll"
#else
#define D3D_REF_DLLNAME L"d3d11ref.dll"
#endif


///////////////////////////////////////////////////////
// Enum:  TEST_DXGI_VERSION
//
// Purpose:
// Expresses the version of DXGI against which a test
// wants to be run.
///////////////////////////////////////////////////////
enum TEST_DXGI_VERSION
{
	TEST_DXGI_VERSION_1_0 = 0,
	TEST_DXGI_VERSION_1_1,
	TEST_DXGI_VERSION_1_2,
	TEST_DXGI_VERSION_1_3,
	TEST_DXGI_VERSION_BESTAVAILABLE
};

enum D3DTEST_INTERFACE_LEVEL
{
	D3DTEST_INTERFACE_LEVEL_10_0 = 1,
	D3DTEST_INTERFACE_LEVEL_10_1 = 2,
	D3DTEST_INTERFACE_LEVEL_11_0 = 3,
	D3DTEST_INTERFACE_LEVEL_11_1 = 4,
};

DECLARE_NAMED_VALUES( D3DTEST_INTERFACE_LEVEL )

///////////////////////////////////////////////////////
// Enum: TEST_DXGI_ADAPTER_TYPE
//
// Purpose:
// Specifies the type of adapter to use when creating
// a D3D device.
///////////////////////////////////////////////////////
enum TEST_DXGI_ADAPTER_TYPE
{
	TEST_DXGI_ADAPTER_TYPE_FULL_HW =		0x1,          // Subset of HW adapters that support both Render Only and Display Only functionality
	TEST_DXGI_ADAPTER_TYPE_RENDERONLY_HW =	0x2,    // Subset of HW adapters that support only Render Only functionality
	TEST_DXGI_ADAPTER_TYPE_FULL_SW =		0x4,          // Subset of WARP-based SW adapters that support both Render Only and Display Only functionality
	TEST_DXGI_ADAPTER_TYPE_RENDERONLY_SW =	0x8,     // Subset of WARP-based SW adapters that support only Render Only functionality
	TEST_DXGI_ADAPTER_TYPE_ALL =			0xF              // Subset of all adapters (supported by requested version of DXGI)
};

DECLARE_NAMED_VALUES( TEST_DXGI_ADAPTER_TYPE );

///////////////////////////////////////////////////////
// Enum: TEST_EXPLICIT_OPTION_SWITCH
//
// Purpose:
// Used by boolean option variables that need to be specified explicitly 
// (eg: -someoption:enabled, -someoption:disabled)
///////////////////////////////////////////////////////
enum TEST_EXPLICIT_OPTION_SWITCH
{
	TEST_EXPLICIT_OPTION_SWITCH_ENABLED,
	TEST_EXPLICIT_OPTION_SWITCH_DISABLED
};

DECLARE_NAMED_VALUES( TEST_EXPLICIT_OPTION_SWITCH )

///////////////////////////////////////////////////////
// Enum: TEST_DXGI_HYBRID_OVERRIDE
//
// Purpose:
// Forces the runtime to enable/disable hybrid overrides 
///////////////////////////////////////////////////////
enum TEST_DXGI_HYBRID_OVERRIDE
{
	TEST_DXGI_HYBRID_OVERRIDE_NONE = 0,
	TEST_DXGI_HYBRID_OVERRIDE_FORCE_DISCRETE = 1,
	TEST_DXGI_HYBRID_OVERRIDE_EMULATE = 2,
	TEST_DXGI_HYBRID_OVERRIDE_FORCE_INTEGRATED = 3
};

//
// DXGI 1.0/1.1 pointer unions
//

typedef union
{
	IDXGIFactory * pFactory;
	IDXGIFactory1 * pFactory1;
	IDXGIFactory2 * pFactory2;
} DXGI_FACTORY_POINTER;

typedef union
{
	IDXGISwapChain * pSwapChain;
	IDXGISwapChain1 * pSwapChain1;
} DXGI_SWAPCHAIN_POINTER;

typedef union
{
	IDXGIAdapter * pAdapter;
	IDXGIAdapter1 * pAdapter1;
	IDXGIAdapter2 * pAdapter2;
} DXGI_ADAPTER_POINTER;


typedef HRESULT (WINAPI *PCREATEDXGIFACTORY)
(
	REFIID riid,
	void **ppFactory
);

typedef HRESULT (WINAPI *PCREATEDXGIFACTORY1)
(
	REFIID riid,
	void **ppFactory
);

typedef HRESULT (WINAPI *PCREATEDXGIFACTORY2)
(
	UINT Flags,
	REFIID riid,
	void **ppFactory
);

typedef void (WINAPI *PDXGIDUMPJOURNAL)
(
	void(*pfn)(const char *)
);

typedef void (WINAPI * PSETAPPCOMPATSTRINGPOINTER)
(
	SIZE_T BufferLength,
	const char* pStringBuffer
);

struct DXGI_OPTIONS
{
	TEST_DXGI_VERSION Version;
	TEST_DXGI_HYBRID_OVERRIDE uHybridOverride;

	// This union contains the requested subset of adapters to consider for device creation
	union
	{
		UINT AdapterType[2];
		struct  
		{
			UINT AdapterTypeSrc;
			UINT AdapterTypeRef;
		};
	};

	// This union contains the requested adapter ordinal number in the current AdapterType subset
	union
	{
		UINT32 AdapterNumber[2];
		struct  
		{
			UINT32 AdapterNumberSrc;
			UINT32 AdapterNumberRef;
		};
	};

	// This union contains the actual adapter ID in EnumAdapters1 for device creation that corresponds
	// to the requested AdapterType and AdapterNumber ordinal number
	union
	{
		UINT32 AdapterID[2];
		struct  
		{
			UINT32 AdapterIDSrc;
			UINT32 AdapterIDRef;
		};
	};
	
	// This union contains the requested output ordinal number from the outputs in current adapter
	union
	{
		UINT32 OutputID[2];
		struct
		{
			UINT32 OutputIDSrc;
			UINT32 OutputIDRef;
		};
	};
};


// WARNING
// This structure has memset() and memcpy() called against it.
// DO NOT ADD VIRTUAL MEMBER FUNCTIONS OR ADD ANY MEMBER VARIABLES
// WHICH HAVE CONSTRUCTORS/DESTRUCTORS.  DOING SO WILL RESULT
// IN DIFFICULT TO DEBUG PROBLEMS SUCH AS HEAP CORRUPTIONS.
struct BASE_D3D_OPTIONS
{
	bool Debug;
	bool DriverVerifier;
	bool DriverVerifierOptOut;				// Some tests won't work with the D3D driver verifier by design
	bool SingleThreaded;
	bool PreventThreadingOptimizations;
	bool SrcOnly;
	bool allowSwitchToRef;
	bool forceSwitchToRef;
	bool Force11On12;
	bool PSOCache;

	bool generateGolden;

	bool proxyDriver;
	HMODULE hFilterDriver;

	HMODULE hReferenceRasterizer;

	WCHAR SoftwareRasterizerDLLName[MAX_PATH];

	WCHAR DxgiDLLName[MAX_PATH];

	HMODULE hSoftwareRasterizer;
};


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
			__in BASE_D3D_OPTIONS * pD3DOptions
		);

		virtual ~CGraphicsTestFramework();


	//
	// Framework override methods
	//

	public:
		virtual void InitOptionVariables();


	//
	// Public methods
	//

	public:
		virtual HRESULT CreateDXGIFactory
		(
			DXGI_FACTORY_POINTER * pFactoryPointer
		);


		virtual bool IsTerminalServices() const { return m_bTerminalServices; };


		virtual HRESULT LoadDXGI();

		virtual void UnloadDXGI();


		virtual HMODULE LoadReferenceRasterizer();

		virtual void UnloadReferenceRasterizer();


		virtual HMODULE LoadSoftwareRasterizer();

		virtual void UnloadSoftwareRasterizer();

		bool IsD3D11On12Device( IUnknown *pDevice ) const;
		bool IsWARPAdapter( IDXGIAdapter *pAdapter ) const;
		bool IsREFAdapter( IDXGIAdapter *pAdapter ) const;

		void InitiateJournalDump(bool bWriteToLog);
		static void WriteDXGIJournalToLog(const char *pMessage);
		static bool s_mbDumpToDXGLogger;

	//
	// Protected helper methods
	//

	protected:

		virtual bool IsAllowedAdapterType( UINT DeviceNum );
		TEST_RESULT SetupAdapter( UINT DeviceNum, DXGI_FACTORY_POINTER* ppDXGIFactory, __out IDXGIAdapter** ppAdapter );
		TEST_RESULT SetupFullscreenOutput( UINT DeviceNum, IDXGIAdapter* pAdapter, __out IDXGIOutput** ppOutput );
		bool IsAdapterOfType( IDXGIAdapter1* pAdapter, UINT AdapterType );
		bool IsAdapterOfType( IDXGIAdapter2* pAdapter, UINT AdapterType );
		bool AdapterMatchesVendorIDAndDeviceID( IDXGIAdapter *pAdapter, UINT vendorID, UINT deviceID ) const;
		UINT GetCreateFactoryFlags() const;
		bool GetBestAvailableDXGIVersion( __out TEST_DXGI_VERSION &bestAvailableDXGIVersion ) const;

	//
	// Static methods
	//

	protected:
		static void OptionCallback_DXGIVersion( const tstring &DXGIVersion );

		static void OptionCallback_AdapterType( const TEST_DXGI_ADAPTER_TYPE& adapterType );

		static void OptionCallback_AdapterNumber( const UINT &adapterId );

		static void OptionCallback_OutputID( const UINT &outputID );

		static void OptionCallback_RasterizerDLL( const tstring &Rasterizer );

		static void OptionCallback_DxgiDLL( const tstring &DxgiDLL );

		static void OptionCallback_TerminalServices( const bool &bTerminalServices );

		static void OptionCallback_ThreadingOptimizations( const TEST_EXPLICIT_OPTION_SWITCH& optionSwitch );

		static void OptionCallback_HybridOverride( const tstring &HybridOverride );

	//
	// Member data
	//

	public:
		HMODULE m_hDXGI;
		PCREATEDXGIFACTORY m_pCreateDXGIFactory;
		PCREATEDXGIFACTORY1 m_pCreateDXGIFactory1;
		PCREATEDXGIFACTORY2 m_pCreateDXGIFactory2;

		PDXGIDUMPJOURNAL m_pDXGIDumpJournal;

		PSETAPPCOMPATSTRINGPOINTER m_pSetAppCompatStringPointer;

		bool m_bLogDebug;

		bool m_bLogDXGIJounrnal;

		bool m_bRegisterOrdinals;

		DXGI_OPTIONS m_DXGIOptions;

		bool m_bTerminalServices;


	private:
		BASE_D3D_OPTIONS * m_pD3DOptions;
};
