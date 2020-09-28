//////////////////////////////////////////////////////////////////////
// File:  GraphicsTest.h
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a graphics test framework base class.  This class
// holds data that's common between DXGI and D3D test framework
// derived classes.
//
// History:
// 16 Feb 2008  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <initguid.h>
#include "GraphicsTest.h"

#include "AdapterUtil.h"
#include <strsafe.h>
#include <d3d11on12.h>

BEGIN_NAMED_VALUES( D3DTEST_INTERFACE_LEVEL )
	NAMED_VALUE( _T( "10.0" ), D3DTEST_INTERFACE_LEVEL_10_0 )
	NAMED_VALUE( _T( "10.1" ), D3DTEST_INTERFACE_LEVEL_10_1 )
	NAMED_VALUE( _T( "11.0" ), D3DTEST_INTERFACE_LEVEL_11_0 )
	NAMED_VALUE( _T( "11.1" ), D3DTEST_INTERFACE_LEVEL_11_1 )
	NAMED_VALUE( _T( "10" ), D3DTEST_INTERFACE_LEVEL_10_0 )
	NAMED_VALUE( _T( "11" ), D3DTEST_INTERFACE_LEVEL_11_0 )
END_NAMED_VALUES( D3DTEST_INTERFACE_LEVEL )

BEGIN_NAMED_VALUES( TEST_DXGI_ADAPTER_TYPE )
	NAMED_VALUE( _T( "fullhw" ), TEST_DXGI_ADAPTER_TYPE_FULL_HW )
	NAMED_VALUE( _T( "renderonlyhw" ), TEST_DXGI_ADAPTER_TYPE_RENDERONLY_HW )
	NAMED_VALUE( _T( "fullsw" ), TEST_DXGI_ADAPTER_TYPE_FULL_SW )
	NAMED_VALUE( _T( "renderonlysw" ), TEST_DXGI_ADAPTER_TYPE_RENDERONLY_SW )
	NAMED_VALUE( _T( "all" ), TEST_DXGI_ADAPTER_TYPE_ALL )
END_NAMED_VALUES( TEST_DXGI_ADAPTER_TYPE )


BEGIN_NAMED_VALUES( TEST_EXPLICIT_OPTION_SWITCH )
	NAMED_VALUE( _T( "enabled" ), TEST_EXPLICIT_OPTION_SWITCH_ENABLED )
	NAMED_VALUE( _T( "disabled" ), TEST_EXPLICIT_OPTION_SWITCH_DISABLED )
END_NAMED_VALUES( TEST_EXPLICIT_OPTION_SWITCH )


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CGraphicsTestFramework
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CGraphicsTestFramework::CGraphicsTestFramework
(
	BASE_D3D_OPTIONS * pD3DOptions
)
:
	m_hDXGI( NULL ),
	m_pCreateDXGIFactory( NULL ),
	m_pCreateDXGIFactory1( NULL ),
	m_pCreateDXGIFactory2( NULL ),
	m_bLogDebug( false ),
	m_bLogDXGIJounrnal( true ),
	m_bRegisterOrdinals( true ),
	m_pD3DOptions( pD3DOptions ),
	m_bTerminalServices( false )
{
	// Intialize default options
	ZeroMemory( &m_DXGIOptions, sizeof( m_DXGIOptions ) );

	m_DXGIOptions.Version = TEST_DXGI_VERSION_BESTAVAILABLE;
	m_DXGIOptions.uHybridOverride = TEST_DXGI_HYBRID_OVERRIDE_NONE;	// Let the UMD decide what to do
	m_DXGIOptions.AdapterTypeSrc = TEST_DXGI_ADAPTER_TYPE_ALL;
	m_DXGIOptions.AdapterTypeRef = TEST_DXGI_ADAPTER_TYPE_ALL;
	m_DXGIOptions.AdapterNumberRef = 0;
	m_DXGIOptions.AdapterNumberSrc = 0;
	m_DXGIOptions.AdapterIDRef = 0;
	m_DXGIOptions.AdapterIDSrc = 0;
	m_DXGIOptions.OutputIDRef = 0;
	m_DXGIOptions.OutputIDSrc = 0;

	m_pD3DOptions->Debug = false;
	m_pD3DOptions->SingleThreaded = false;
	m_pD3DOptions->PreventThreadingOptimizations = false;
	m_pD3DOptions->allowSwitchToRef = false;
	m_pD3DOptions->forceSwitchToRef = false;
	m_pD3DOptions->Force11On12 = false;
	m_pD3DOptions->PSOCache = false;
	m_pD3DOptions->generateGolden = false;
	m_pD3DOptions->proxyDriver = false;
	m_pD3DOptions->hFilterDriver = NULL;
	m_pD3DOptions->hReferenceRasterizer = NULL;
	m_pD3DOptions->hSoftwareRasterizer = NULL;
	m_pD3DOptions->DriverVerifier = false;
	m_pD3DOptions->DriverVerifierOptOut = false;
};


///////////////////////////////////////////////////////
// Function:  ~CGraphicsTestFramework
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CGraphicsTestFramework::~CGraphicsTestFramework()
{
	UnloadDXGI();
};


////////////////////////////////////////////////////////////
//
// Framework override methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  InitOptionVariables
//
// Purpose:
// Registers command-line option switches.
///////////////////////////////////////////////////////
void CGraphicsTestFramework::InitOptionVariables()
{
	int id = AddOptionCategory( "D3D" );

	if ( m_bRegisterOrdinals == true )
	{
		RegisterOptionVariable( "AdapterType", "Specifies the subset of enabled adapters that will be considered for device creation. AdapterNum '0' in the AdapterType 'all' subset is the primary adapter. (def: all)",
								OptionCallback_AdapterType, TEST_DXGI_ADAPTER_TYPE_ALL, OPTION_VALUE_CONSTANT, id,
								"-AdapterType:[all|fullhw|renderonlyhw|fullsw|renderonlysw]" );

		RegisterOptionVariable<UINT32>( "AdapterNum", "Specifies adapter ordinal number in current AdapterType subset (def:0)", OptionCallback_AdapterNumber, 0, OPTION_VALUE_CONSTANT, id );
		RegisterOptionVariable<UINT32>( "AdapterNumSrc", "Specifies source render adapter ordinal number in current AdapterTypeSrc subset (def:0)", &m_DXGIOptions.AdapterNumberSrc, 0, 0, id );
		RegisterOptionVariable<UINT32>( "AdapterNumRef", "Specifies reference render adapter ordinal number in current AdapterTypeRef subset (def:0)", &m_DXGIOptions.AdapterNumberRef, 0, 0, id );

		// Register alias Adapter options without the "Num" part to ensure backwards compatibility
		RegisterOptionVariable<UINT32>( "Adapter", "Deprecated option, alias for AdapterNum (see AdapterNum for description)", OptionCallback_AdapterNumber, 0, OPTION_VALUE_CONSTANT, id );
		RegisterOptionVariable<UINT32>( "AdapterSrc", "Deprecated option, alias for AdapterNumSrc (see AdapterNumSrc for description)", &m_DXGIOptions.AdapterNumberSrc, 0, 0, id );
		RegisterOptionVariable<UINT32>( "AdapterRef", "Deprecated option, alias for AdapterNumRef (see AdapterNumRef for description)", &m_DXGIOptions.AdapterNumberRef, 0, 0, id );

		RegisterOptionVariable<UINT32>( "OutputID", "Specifies the output ordinal number from outputs in current adapter to use for full-screen tests (def: 0)", OptionCallback_OutputID, 0, OPTION_VALUE_CONSTANT, id );
		RegisterOptionVariable<UINT32>( "OutputIDSrc", "Specifies the source render output ordinal number from outputs in current adapter to use for full-screen tests (def: 0)", &m_DXGIOptions.OutputIDSrc, 0, OPTION_VALUE_CONSTANT, id );
		RegisterOptionVariable<UINT32>( "OutputIDRef", "Specifies the reference render output ordinal number from outputs in current adapter to use for full-screen tests (def: 0)", &m_DXGIOptions.OutputIDRef, 0, OPTION_VALUE_CONSTANT, id );
	}


	RegisterOptionVariable<bool>( "AllowSwitchToRef", "Create device with switch to ref flag set", &m_pD3DOptions->allowSwitchToRef, false, OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable<bool>( "ForceSwitchToRef", "Force switch to REF after device creation", &m_pD3DOptions->forceSwitchToRef, false, OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable( "D3DDebug", "Use debug D3D layer", &m_pD3DOptions->Debug, false, 0, id );
	RegisterOptionVariable( "D3DDriverVerifier", "Use D3D driver verifier layer", &m_pD3DOptions->DriverVerifier, true, 0, id );
	RegisterOptionVariable( "SingleThreaded", "Use single-threaded D3D device", &m_pD3DOptions->SingleThreaded, false, 0, id );

	// If not present, default to ForceIntegrated, if present, default to ForceDiscrete
	RegisterOptionVariable( "Hybrid", "Overrides DXGI's behavior for hybrid enumeration (def:ForceIntegrated)", 
		OptionCallback_HybridOverride, tstring(_T("None")), OPTION_VALUE_CONSTANT, id,
		"-Hybrid:[ForceDiscrete|ForceIntegrated|None]" );

	RegisterOptionVariable<bool>( "11on12", "Forces hardware D3D11 devices to run through D3D11On12", &m_pD3DOptions->Force11On12, false, OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable<bool>( "PSOCache", "When D3D11On12 is used, all PSOs are round-tripped through the PSO cache before being used", &m_pD3DOptions->PSOCache, false, OPTION_VALUE_CONSTANT, id );

	RegisterOptionVariable( "ThreadingOptimizations", "Specifies whether to keep the threading optimizations enabled or disabled when creating the device. (def: enabled)",
							OptionCallback_ThreadingOptimizations, TEST_EXPLICIT_OPTION_SWITCH_ENABLED, OPTION_VALUE_CONSTANT, id,
							"-ThreadingOptimizations:[enabled|disabled]" );

	RegisterOptionVariable( "SWDLL", "Software rasterizer", OptionCallback_RasterizerDLL, tstring(_T("d3d10warp.dll")), OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable( "DXGIDLL", "DXGI DLL", OptionCallback_DxgiDLL, tstring(_T(DXGI_DLL_NAME)), OPTION_VALUE_CONSTANT, id );

	RegisterOptionVariable<bool>( "LogDebug", "Log Debug Spew from D3D", &m_bLogDebug, false, OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable<bool>( "LogDXGIJournal", "Log DXGI Journal entries", &m_bLogDXGIJounrnal, true, OPTION_VALUE_CONSTANT, id );


	RegisterOptionVariable
	(
		"DXGI",
		"Specify DXGI version to test (1.0, 1.1 or 1.2).",
		OptionCallback_DXGIVersion,
		tstring(_T("default")),
		OPTION_VALUE_CONSTANT,
		id
	);


	RegisterOptionVariable
	(
		"TS",
		"Test terminal services functionality.",
		OptionCallback_TerminalServices,
		false,
		OPTION_VALUE_CONSTANT,
		id
	);
};


////////////////////////////////////////////////////////////
//
// DXGI initialization/cleanup methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  LoadDXGI
//
// Purpose:
// Loads DXGI and retrieves pointers to entry points
// that we need.
///////////////////////////////////////////////////////
HRESULT CGraphicsTestFramework::LoadDXGI()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Load DXGI if needed.
	//

	if ( m_hDXGI == NULL )
	{
		m_hDXGI = LoadLibraryExW
		(
			m_pD3DOptions->DxgiDLLName,
			NULL,
			0
		);

		if ( m_hDXGI == NULL )
		{
			DWORD LoadLibraryError = GetLastError();

			WriteToLogF
			(
				L"CGraphicsTestFramework::LoadDXGI:  LoadLibrary on DXGI DLL %s failed with error %u.",
				m_pD3DOptions->DxgiDLLName,
				LoadLibraryError
			);

			if ( LoadLibraryError != ERROR_SUCCESS )
			{
				hOverallResult = HRESULT_FROM_WIN32( LoadLibraryError );
			};

			goto Cleanup;
		};
	}
	else
	{
		hOverallResult = S_FALSE;
		goto Cleanup;
	};

	//
	// Determine the best available DXGI version
	//

	TEST_DXGI_VERSION bestAvailableDXGIVersion;
	if( !GetBestAvailableDXGIVersion( bestAvailableDXGIVersion ) )
	{
		WriteToLogF( _T("CGraphicsTestFramework::LoadDXGI:  GetBestAvailableDXGIVersion() failed.") );
		hOverallResult = E_FAIL;
		goto Cleanup;
	}

	WriteToLogF( _T("Best available DXGI version: 1.%s."), ToString(bestAvailableDXGIVersion).c_str() );
	if( TEST_DXGI_VERSION_BESTAVAILABLE == m_DXGIOptions.Version )
	{
		// If "-hybrid" is specified on command line parameter and DXGI version is not explicitely specified
		// use DXGI 1.0. Framework has some code paths that only work for adapters with full capabilities if
		// DXGI >= 1.1 is specified. This poses a problem for Hybrid tests since when running in hybrid mode
		// our default adapter is dGPU which is Render Only.
		// Once bug 218689 is fixed this check should be removed
		if(m_DXGIOptions.uHybridOverride != TEST_DXGI_HYBRID_OVERRIDE_NONE)
		{
			m_DXGIOptions.Version = TEST_DXGI_VERSION_1_0;
			WriteToLogF( _T("Using DXGI version: 1.0 because Hybrid command line option was specified"));
		}
		else
			m_DXGIOptions.Version = bestAvailableDXGIVersion;
	}

	//
	// Get pointers to DXGI entry points.
	//

	m_pCreateDXGIFactory = (PCREATEDXGIFACTORY) GetProcAddress
	(
		m_hDXGI,
		"CreateDXGIFactory"
	);

	if ( m_pCreateDXGIFactory == NULL )
	{
		DWORD GetProcAddressError = GetLastError();

		WriteToLogF
		(
			"CGraphicsTestFramework::LoadDXGI:  GetProcAddress on CreateDXGIFactory failed with error %u.",
			GetProcAddressError
		);

		if ( GetProcAddressError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( GetProcAddressError );
		};

		goto Cleanup;
	};


	m_pCreateDXGIFactory1 = (PCREATEDXGIFACTORY1) GetProcAddress
	(
		m_hDXGI,
		"CreateDXGIFactory1"
	);

	if ( m_pCreateDXGIFactory1 == NULL )
	{
		DWORD GetProcAddressError = GetLastError();

		WriteToLogF
		(
			"CGraphicsTestFramework::LoadDXGI:  GetProcAddress on CreateDXGIFactory1 failed with error %u.",
			GetProcAddressError
		);

		// DXGI 1.1 requires the CreateDXGIFactory1 entry point
		if( m_DXGIOptions.Version >= TEST_DXGI_VERSION_1_1 )
		{
			if ( GetProcAddressError != ERROR_SUCCESS )
			{
				hOverallResult = HRESULT_FROM_WIN32( GetProcAddressError );
			};

			goto Cleanup;
		}
	};

	m_pCreateDXGIFactory2 = (PCREATEDXGIFACTORY2) GetProcAddress
	(
		m_hDXGI,
		"CreateDXGIFactory2"
	);

	if ( m_pCreateDXGIFactory2 == NULL )
	{
		DWORD GetProcAddressError = GetLastError();

		WriteToLogF
		(
			"CGraphicsTestFramework::LoadDXGI:  GetProcAddress on CreateDXGIFactory2 failed with error %u.",
			GetProcAddressError
		);

		// DXGI 1.3 requires the CreateDXGIFactory2 entry point
		if( m_DXGIOptions.Version >= TEST_DXGI_VERSION_1_3 )
		{
			if ( GetProcAddressError != ERROR_SUCCESS )
			{
				hOverallResult = HRESULT_FROM_WIN32( GetProcAddressError );
			};

			goto Cleanup;
		}
	};

	m_pSetAppCompatStringPointer = (PSETAPPCOMPATSTRINGPOINTER) GetProcAddress
	(
		m_hDXGI,
		"SetAppCompatStringPointer"
	);
	if ( m_pSetAppCompatStringPointer == NULL )
	{
		DWORD GetProcAddressError = GetLastError();

		WriteToLogF
		(
			"CGraphicsTestFramework::LoadDXGI:  GetProcAddress on SetAppCompatStringPointer failed with error %u.",
			GetProcAddressError
		);
	}

	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  UnloadDXGI
//
// Purpose:
// Unloads DXGI.
///////////////////////////////////////////////////////
void CGraphicsTestFramework::UnloadDXGI()
{
	m_pCreateDXGIFactory = NULL;
	m_pCreateDXGIFactory1 = NULL;
	m_pCreateDXGIFactory2 = NULL;
	m_pSetAppCompatStringPointer = NULL;

	if ( m_hDXGI != NULL )
	{
		FreeLibrary( m_hDXGI );
		m_hDXGI = NULL;
	};
};


///////////////////////////////////////////////////////
// Function:  CreateDXGIFactory
//
// Purpose:
// Creates a DXGI factory.
///////////////////////////////////////////////////////
HRESULT CGraphicsTestFramework::CreateDXGIFactory
(
	DXGI_FACTORY_POINTER * FactoryPointer
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Make sure that DXGI is loaded.
	//

	if ( m_pCreateDXGIFactory == NULL )
	{
		HRESULT hLoadDXGIResult = LoadDXGI();

		if ( FAILED( hLoadDXGIResult ) )
		{
			hOverallResult = hLoadDXGIResult;
			goto Cleanup;
		};
	};
	
	//
	// Force D3D/DXGI-wide behavior overrides
	//
	
	if (nullptr != m_pSetAppCompatStringPointer)
	{
		// Must pass strings that are not on the stack or heap
		static char shimString[1024] = {};
		char* nextShimString = shimString;
		size_t shimSizeRemaining = ARRAYSIZE(shimString);
		if (m_DXGIOptions.uHybridOverride != TEST_DXGI_HYBRID_OVERRIDE_NONE)
		{
			if (FAILED(StringCchPrintfExA(nextShimString, shimSizeRemaining, &nextShimString, &shimSizeRemaining, 0, "%sHybrid=%u", nextShimString == shimString ? "" : ";", m_DXGIOptions.uHybridOverride)))
			{
				hOverallResult = E_OUTOFMEMORY;
				goto Cleanup;
			}
		}
		if (m_pD3DOptions->PSOCache)
		{
			if (FAILED(StringCchPrintfExA(nextShimString, shimSizeRemaining, &nextShimString, &shimSizeRemaining, 0, "%sRoundTripPSOs=1", nextShimString == shimString ? "" : ";")))
			{
				hOverallResult = E_OUTOFMEMORY;
				goto Cleanup;
			}
		}

		if (shimString != nextShimString)
		{
			m_pSetAppCompatStringPointer( strlen( shimString ) + 1, shimString );
		}
	}

	//
	// Create the factory.
	//

	switch ( m_DXGIOptions.Version )
	{
		case TEST_DXGI_VERSION_1_3:
		{
			assert( nullptr != m_pCreateDXGIFactory2 );
			const UINT createFactoryFlags = GetCreateFactoryFlags();
			hOverallResult = (*m_pCreateDXGIFactory2)
			(
				createFactoryFlags,
				IID_IDXGIFactory2,
				(void **) &FactoryPointer->pFactory2
			);
		} break;

		case TEST_DXGI_VERSION_1_2:
		{
			assert( nullptr != m_pCreateDXGIFactory1 );
			hOverallResult = (*m_pCreateDXGIFactory1)
			(
				IID_IDXGIFactory2,
				(void **) &FactoryPointer->pFactory2
			);
		} break;

		case TEST_DXGI_VERSION_1_1:
		{
			assert( nullptr != m_pCreateDXGIFactory1 );
			hOverallResult = (*m_pCreateDXGIFactory1)
			(
				IID_IDXGIFactory1,
				(void **) &FactoryPointer->pFactory1
			);
		} break;

		case TEST_DXGI_VERSION_1_0:
		{
			assert( nullptr != m_pCreateDXGIFactory != NULL );
			hOverallResult = (*m_pCreateDXGIFactory)
			(
				IID_IDXGIFactory,
				(void **) &FactoryPointer->pFactory
			);
		} break;

		default:
		{
			assert( FALSE );
			hOverallResult = E_INVALIDARG;
		} break;
	};


Cleanup:
	return hOverallResult;
};


////////////////////////////////////////////////////////////
//
// Rasterizer management methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  LoadReferenceRasterizer
//
// Purpose:
// Loads the reference rasterizer DLL and returns a
// module handle.
///////////////////////////////////////////////////////
HMODULE CGraphicsTestFramework::LoadReferenceRasterizer()
{
	if ( m_pD3DOptions->hReferenceRasterizer == NULL )
	{
		m_pD3DOptions->hReferenceRasterizer = LoadLibraryExW
		(
			D3D_REF_DLLNAME,
			NULL,
			0
		);

		if ( m_pD3DOptions->hReferenceRasterizer == NULL )
		{
			DWORD LoadLibraryError = GetLastError();

			WriteToLogF
			(
				L"CGraphicsTestFramework::LoadReferenceRasterizer:  LoadLibrary on reference rasterizer DLL %s failed with error %u.",
				D3D_REF_DLLNAME,
				LoadLibraryError
			);
		};
	};

	return m_pD3DOptions->hReferenceRasterizer;
};


///////////////////////////////////////////////////////
// Function:  UnloadReferenceRasterizer
//
// Purpose:
// Unloads the reference rasterizer.
///////////////////////////////////////////////////////
void CGraphicsTestFramework::UnloadReferenceRasterizer()
{
	if ( m_pD3DOptions->hReferenceRasterizer != NULL )
	{
		FreeLibrary( m_pD3DOptions->hReferenceRasterizer );
		m_pD3DOptions->hReferenceRasterizer = NULL;
	};
};


///////////////////////////////////////////////////////
// Function:  LoadSoftwareRasterizer
//
// Purpose:
// Loads the software rasterizer DLL and returns a
// module handle.
///////////////////////////////////////////////////////
HMODULE CGraphicsTestFramework::LoadSoftwareRasterizer()
{
	if ( m_pD3DOptions->hSoftwareRasterizer == NULL )
	{
		if ( m_pD3DOptions->SoftwareRasterizerDLLName[ 0 ] == L'\0' )
		{
			WriteToLog
			(
				_T("CGraphicsTestFramework::LoadSoftwareRasterizer:  Software rasterizer DLL name not specified.")
			);
		}
		else
		{
			m_pD3DOptions->hSoftwareRasterizer = LoadLibraryExW
			(
				m_pD3DOptions->SoftwareRasterizerDLLName,
				NULL,
				0
			);

			if ( m_pD3DOptions->hSoftwareRasterizer == NULL )
			{
				DWORD LoadLibraryError = GetLastError();

				WriteToLogF
				(
					L"CGraphicsTestFramework::LoadSoftwareRasterizer:  LoadLibrary on software rasterizer DLL %s failed with error %u.",
					m_pD3DOptions->SoftwareRasterizerDLLName,
					LoadLibraryError
				);
			};
		};
	};


	return m_pD3DOptions->hSoftwareRasterizer;
};


///////////////////////////////////////////////////////
// Function:  UnloadSoftwareRasterizer
//
// Purpose:
// Unloads the software rasterizer.
///////////////////////////////////////////////////////
void CGraphicsTestFramework::UnloadSoftwareRasterizer()
{
	if ( m_pD3DOptions->hSoftwareRasterizer != NULL )
	{
		FreeLibrary( m_pD3DOptions->hSoftwareRasterizer );
		m_pD3DOptions->hSoftwareRasterizer = NULL;
	};
};

///////////////////////////////////////////////////////
// Function:  IsD3D11On12Device
//
// Purpose:
// Returns true if the provided device is using 11on12.
///////////////////////////////////////////////////////
bool CGraphicsTestFramework::IsD3D11On12Device( IUnknown *pDevice ) const
{
	ID3D11On12Device *pDevice11On12 = NULL;
	if (SUCCEEDED(pDevice->QueryInterface(&pDevice11On12)))
	{
		pDevice11On12->Release();
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////
// Function:  IsWARPAdapter
//
// Purpose:
// Returns true if the provided adapter is using WARP (MSBDA / WARP).
///////////////////////////////////////////////////////
bool CGraphicsTestFramework::IsWARPAdapter( IDXGIAdapter *pAdapter ) const
{
	const UINT vendorID = 0x1414;
	const UINT deviceID = 0x008C;
	return AdapterMatchesVendorIDAndDeviceID( pAdapter, vendorID, deviceID );
}

///////////////////////////////////////////////////////
// Function:  IsREFAdapter
//
// Purpose:
// Returns true if the provided adapter is using REF.
///////////////////////////////////////////////////////
bool CGraphicsTestFramework::IsREFAdapter( IDXGIAdapter *pAdapter ) const
{
	const UINT vendorID = 0x0000;
	const UINT deviceID = 0x0000;
	return AdapterMatchesVendorIDAndDeviceID( pAdapter, vendorID, deviceID );
}

///////////////////////////////////////////////////////
// Function:  IsAllowedAdapterType
//
// Purpose:
// Returns true if the requested adapter type is allowed in
// the requested DXGI verison.
///////////////////////////////////////////////////////

bool CGraphicsTestFramework::IsAllowedAdapterType( UINT DeviceNum )
{
	bool Allowed = true;

	// must at least include TEST_DXGI_ADAPTER_TYPE_FULL_HW as an option when using DXGI v1.0
	if( m_DXGIOptions.Version < TEST_DXGI_VERSION_1_1 &&
		!(m_DXGIOptions.AdapterType[DeviceNum] & TEST_DXGI_ADAPTER_TYPE_FULL_HW) )
	{
		Allowed = false;
	}

	return Allowed;
}

///////////////////////////////////////////////////////
// Function:  SetupAdapter
//
// Purpose:
// Sets up the adapter by selecting the one that the user
// requested from a specific subset of adapters.
///////////////////////////////////////////////////////
TEST_RESULT CGraphicsTestFramework::SetupAdapter( UINT DeviceNum, DXGI_FACTORY_POINTER* ppDXGIFactory, __out IDXGIAdapter** ppAdapter )
{
	assert( ppDXGIFactory != NULL );
	assert( ppAdapter != NULL );

	TEST_RESULT SetupResult = RESULT_PASS;

	// Determine the AdapterID ordinal number given the AdapterType and AdapterNumber

	IDXGIAdapter* pAdapter = NULL;
	IDXGIAdapter1* pAdapter1 = NULL;
	bool FoundRequestedAdapter = false;
	UINT SubsetAdapterNum = 0;
	HRESULT hr = S_OK;
	UINT n = 0;

	while( true )
	{
		// Use the appropriate EnumAdapters call based on the version of DXGI requested
		if( m_DXGIOptions.Version < TEST_DXGI_VERSION_1_1 )
		{
			hr = ppDXGIFactory->pFactory->EnumAdapters( n, &pAdapter );
		}
		else if( m_DXGIOptions.Version >= TEST_DXGI_VERSION_1_1 && m_DXGIOptions.Version < TEST_DXGI_VERSION_BESTAVAILABLE )
		{
			hr = ppDXGIFactory->pFactory1->EnumAdapters1( n, &pAdapter1 );
		}
		else
		{
			hr = E_FAIL;
		}

		// Stop the search if EnumAdapters above failed
		if( !SUCCEEDED(hr) )
		{
			break;
		}
		else if( SUCCEEDED(hr) && pAdapter == NULL && pAdapter1 == NULL )
		{
			WriteToLogF( _T( "EnumAdapters on adapter %i for device %d succeeded but returned a NULL adapter pointer." ),
						m_DXGIOptions.AdapterID[DeviceNum], DeviceNum );

			break;
		}

		// If we're working with DXGI 1.1+, make sure the adapter belongs to the current AdapterType subset
		// before extracting the DXGI 1.0 adapter pointer
		if( pAdapter1 != NULL )
		{
			if( IsAdapterOfType( pAdapter1, m_DXGIOptions.AdapterType[DeviceNum] ) )
			{
				// Get the DXGI 1.0 Adapter interface since this is what the tests/framework actually needs
				hr = pAdapter1->QueryInterface( __uuidof(IDXGIAdapter), (void**)&pAdapter );

				if( FAILED(hr) )
				{
					WriteToLogF( _T( "Could not QI for IDXGIAdapter interface from IDXGIAdapter1 interface for adapter %i, device %d" ),
									m_DXGIOptions.AdapterID[DeviceNum], DeviceNum );
					break;
				}

				// We don't stop the loop here
			}

			SAFE_RELEASE(pAdapter1);
		}

		// Past this point, pAdapter1 is always NULL

		// Now pAdapter refers to either a DXGI 1.1+ or DXGI 1.0 adapter that is a member of the requested subset,
		// and if both of them were null, pAdapter would also be null, so we continue the search in that case
		if( pAdapter != NULL )
		{
			// Stop search if we found the requested adapter ordinal number in this subset
			if( SubsetAdapterNum == m_DXGIOptions.AdapterNumber[DeviceNum] )
			{
				m_DXGIOptions.AdapterID[DeviceNum] = n;
				*ppAdapter = pAdapter;
				FoundRequestedAdapter = true;

				// We don't release the found adapter here, just stop the loop

				break;
			}

			// The adapter we found belonged to the right subset but it was not the one
			// we were looking for, so keep looking
			SubsetAdapterNum++;
		}

		SAFE_RELEASE(pAdapter);

		// Increment our adapter ordinal number
		++n;
	}

	// Make sure we found an adapter
	if( !FoundRequestedAdapter )
	{
		WriteToLogF( _T( "Requested AdapterNum (%d) was not found in the requested AdapterType subset (%s) of size (%d) for requested DXGI version (%s)" ), m_DXGIOptions.AdapterNumber[DeviceNum], ToString(m_DXGIOptions.AdapterType[DeviceNum]).c_str(), SubsetAdapterNum, ToString(m_DXGIOptions.Version).c_str() );

		SetupResult = (GetTestMode() == WGFTEST_MODE_WHQL) ? RESULT_SKIP : RESULT_FAIL;
		goto Cleanup;
	}

Cleanup:

	return SetupResult;
}

///////////////////////////////////////////////////////
// Function:  SetupFullscreenOutput
//
// Purpose:
// Sets up the output that will be used when/if the test transitions
// to full-screen mode.
///////////////////////////////////////////////////////
TEST_RESULT CGraphicsTestFramework::SetupFullscreenOutput( UINT DeviceNum, IDXGIAdapter* pAdapter, __out IDXGIOutput** ppOutput )
{
	assert( pAdapter != NULL );
	assert( ppOutput != NULL );

	TEST_RESULT SetupResult = RESULT_PASS;
	HRESULT hr = S_OK;
	bool IsFullyCapableAdapter = true;
	bool IsInConsoleSession = (GetSystemMetrics(SM_REMOTESESSION) == 0);

	// Get the Adapter1 interface if we're running under correct version of DXGI and
	// determine if this adapter is a fully-capable adapter
	if( m_DXGIOptions.Version >= TEST_DXGI_VERSION_1_1 )
	{
		IDXGIAdapter1* pAdapter1 = NULL;

		hr = pAdapter->QueryInterface( __uuidof(IDXGIAdapter1), (void**)&pAdapter1 );
		if( FAILED(hr) || pAdapter1 == NULL )
		{
			WriteToLog( _T( "Could not QI for IDXGIAdapter1 interface from IDXGIAdapter interface even though DXGI verison above 1.1 was used." ) );

			SetupResult = RESULT_SKIP;
			goto Cleanup;
		}

		using namespace DevX::GRFX;

		AdapterDescriptorCore* pAdapterDesc = AdapterDescriptorCore::GetAdapterDescriptor(pAdapter1);
		hr = pAdapterDesc->IsRenderAndDisplay(IsFullyCapableAdapter);

		SAFE_RELEASE(pAdapter1);
		SAFE_DELETE(pAdapterDesc);

		if( FAILED(hr) )
		{
			WriteToLogF( _T( "Could not determine whether given adapter supports both Render Only and Display Only DDIs" ) );

			if( hr != E_NOINTERFACE )
			{
				SetupResult = RESULT_SKIP;
				goto Cleanup; 
			}
		}
	}

	// Determine the IDXGIOutput on which the test will go full-screen (this has to be
	// a child output of the IDXGIAdapter on which the device was created)

	hr = pAdapter->EnumOutputs( m_DXGIOptions.OutputID[DeviceNum], ppOutput );

	// Determine if we actually care about whether EnumOutputs failed
	if( (IsInConsoleSession && IsFullyCapableAdapter) || // It cannot fail when we're in console session unless we're using a fully-capable adapter
		m_DXGIOptions.Version < TEST_DXGI_VERSION_1_1 )  // It cannot fail when DXGI less than 1.1 is used
	{
		// Make sure the call itself did not fail
		if( FAILED(hr) )
		{
			WriteToLogF( _T( "Adapter %i, output %i for device %d not found." ), 
							m_DXGIOptions.AdapterID[DeviceNum], m_DXGIOptions.OutputID[DeviceNum], DeviceNum );

			SetupResult = RESULT_SKIP;
			goto Cleanup;
		}

		// Sanity-check the output pointer
		if ( *ppOutput == NULL )
		{
			WriteToLogF( _T( "EnumOutputs for adapter %i, output %i for device %d succeeded but returned a NULL output pointer." ),
							m_DXGIOptions.AdapterID[DeviceNum], m_DXGIOptions.OutputID[DeviceNum], DeviceNum );

			SetupResult = (GetTestMode() == WGFTEST_MODE_WHQL) ? RESULT_SKIP : RESULT_FAIL;
			goto Cleanup;
		}
	}

Cleanup:

	return SetupResult;
}

///////////////////////////////////////////////////////
// Function:  IsAdapterOfType
//
// Purpose:
// Returns true if the given adapter belongs to the given
// AdapterType subset.
///////////////////////////////////////////////////////
bool CGraphicsTestFramework::IsAdapterOfType( IDXGIAdapter1* pAdapter, UINT AdapterType )
{
	using namespace DevX::GRFX;

	if( pAdapter == NULL )
	{
		return false;
	}

	bool Result = false;
	HRESULT hr = S_OK;

	AdapterDescriptorCore* pAdapterDesc = AdapterDescriptorCore::GetAdapterDescriptor(pAdapter);
	bool IsOnlyRO = false;
	bool IsOnlyDO = false;
	bool IsROandDO = false;
	bool AdapterSw = false;

	// IsDisplayOnly / IsRenderOnly / IsRenderAndDisplay / IsSoftwareDevice are Win 8 specific.
	// Cannot query for KMTQAITYPE_ADAPTERTYPE on Win7 + 8IP and below. There is no concept of Display Only/Render Only for adapters
	// Functions will retrun E_NOINTERFACE not running on Win8 or above, but in/out bool parameter will still be returned with the valid values.
	hr = pAdapterDesc->IsDisplayOnly(IsOnlyDO);
	if( FAILED(hr) )
	{ 
		WriteToLogF( _T( "Could not determine whether given adapter only supports Display Only DDIs" ) );
		if( hr != E_NOINTERFACE )	
		{
			Result = false;
			goto Cleanup; 
		}
	}

	hr = pAdapterDesc->IsRenderOnly(IsOnlyRO);
	if( FAILED(hr) )
	{ 
		WriteToLogF( _T( "Could not determine whether given adapter only supports Render Only DDIs" ) );
		if( hr != E_NOINTERFACE )  
		{
			Result = false;
			goto Cleanup; 
		}
	}

	hr = pAdapterDesc->IsRenderAndDisplay(IsROandDO);
	if( FAILED(hr) )
	{
		WriteToLogF( _T( "Could not determine whether given adapter supports both Render Only and Display Only DDIs" ) );
		if( hr != E_NOINTERFACE )  
		{
			Result = false;
			goto Cleanup; 
		}
	}

	hr = pAdapterDesc->IsSoftwareDevice(AdapterSw);
	if( FAILED(hr) )
	{
		WriteToLogF( _T( "Could not determine whether given adapter is a HW or SW adapter" ) );
		if( hr != E_NOINTERFACE ) // 
		{
			Result = false;
			goto Cleanup; 
		}
	}

	if( pAdapterDesc != NULL )
	{
		UINT allowedTypeCount = 0;
		// TEST_DXGI_ADAPTER_TYPE_ALL includes all of the following
		if( AdapterType & TEST_DXGI_ADAPTER_TYPE_FULL_HW )
			if( !AdapterSw && IsROandDO ) allowedTypeCount++;
		if( AdapterType & TEST_DXGI_ADAPTER_TYPE_RENDERONLY_HW )
			if( !AdapterSw && IsOnlyRO ) allowedTypeCount++;
		if( AdapterType & TEST_DXGI_ADAPTER_TYPE_FULL_SW )
		// Full-SW adapters are the PnP Basic Display Adapter (BDA) and non-PnP Basic Display Driver (BDD)
		// The kernel will report them both as having { DisplaySupported: 1, RenderSupported: 0 } capabilities.
		// The { SoftwareDevice: 1 or 0 } capability can be either 1 or 0 (1 for BDA, 0 for BDD). Therefore,
		// we only check for the { DisplaySupported: 1, RenderSupported: 0 } capability to find Full-SW adapters.
		// This functionality will break if/when HW Display Only adapters start appearing and the kernel will
		// need to be fixed to return { SoftwareDevice: 1 } for BDA and BDD.
			if( IsOnlyDO ) allowedTypeCount++;;
		if( AdapterType & TEST_DXGI_ADAPTER_TYPE_RENDERONLY_SW )
			if( AdapterSw && IsOnlyRO ) allowedTypeCount++;;

		Result = allowedTypeCount > 0 ? true : false;
	}

Cleanup:

	SAFE_DELETE(pAdapterDesc);

	return Result;
}

///////////////////////////////////////////////////////
// Function:  IsAdapterOfType
//
// Purpose:
// Returns true if the given adapter belongs to the given
// AdapterType subset.
///////////////////////////////////////////////////////
bool CGraphicsTestFramework::IsAdapterOfType( IDXGIAdapter2* pAdapter, UINT AdapterType )
{
	// Get the DXGI 1.1 Adapter interface from this 1.2 interface
	IDXGIAdapter1* pAdapter1 = NULL;
	HRESULT hr = pAdapter->QueryInterface( __uuidof(IDXGIAdapter1), (void**)&pAdapter1 );

	if( FAILED(hr) )
	{
		WriteToLogF( _T( "Could not QI for IDXGIAdapter1 interface from IDXGIAdapter2 interface" ) );
		return false;
	}

	// Perform the actual query for adapter type
	bool IsOfType = IsAdapterOfType( pAdapter1, AdapterType );

	SAFE_RELEASE(pAdapter1);

	return IsOfType;
}

///////////////////////////////////////////////////////
// Function:  IsAdapterOfType
//
// Purpose:
// Does the specified adapter match the vendor and device id?
///////////////////////////////////////////////////////
bool CGraphicsTestFramework::AdapterMatchesVendorIDAndDeviceID( IDXGIAdapter *pAdapter, UINT vendorID, UINT deviceID ) const
{
	HRESULT hr = E_FAIL;
	DXGI_ADAPTER_DESC adapterDesc;
	ZeroMemory( &adapterDesc, sizeof(adapterDesc) );
	if( !pAdapter || FAILED( hr = pAdapter->GetDesc( &adapterDesc ) ) )
	{
		return false;
	}

	// Both Vendor ID and Device ID have to match
	return ((adapterDesc.VendorId == vendorID) && (adapterDesc.DeviceId == deviceID));
}

///////////////////////////////////////////////////////
// Function:  GetCreateFactoryFlags
//
// Purpose:
// Define what flags should be passed into CreateFactory
///////////////////////////////////////////////////////
UINT CGraphicsTestFramework::GetCreateFactoryFlags() const
{
	// Local variables
	UINT createFactoryFlags = 0;

	return createFactoryFlags;
}

///////////////////////////////////////////////////////
// Function:  GetBestAvailableDXGIVersion
//
// Purpose:
// Return the best available DXGI version for the current system
///////////////////////////////////////////////////////
bool CGraphicsTestFramework::GetBestAvailableDXGIVersion( __out TEST_DXGI_VERSION &bestAvailableDXGIVersion ) const
{
	//
	// Initalize the best available DXGI version.
	//

	bestAvailableDXGIVersion = TEST_DXGI_VERSION_1_0;
	PCREATEDXGIFACTORY2 pCreateDXGIFactory2 = nullptr;
	PCREATEDXGIFACTORY1 pCreateDXGIFactory1 = nullptr;
	PCREATEDXGIFACTORY pCreateDXGIFactory = nullptr;
	IDXGIFactory2 *pIDXGIFactory2 = nullptr;
	IDXGIFactory *pIDXGIFactory = nullptr;
	HRESULT hr = E_FAIL;
	bool bSuccess = false;

	//
	// Check state.
	//

	if( nullptr == m_hDXGI )
	{
		GetFramework()->WriteToLogF( L"CGraphicsTestFramework::GetBestAvailableDXGIVersion:  DXGI module handle is null." );
		goto Cleanup;
	}

	//
	// Check for DXGI support in descending order.
	// Note we're not checking some return values because some things are expected to not be available.
	// We're checking things that are only available in each DXGI version.
	//

	// If CreateDXGIFactory2 is available, DXGI 1.3 is supported
	pCreateDXGIFactory2 = (PCREATEDXGIFACTORY2)GetProcAddress( m_hDXGI, "CreateDXGIFactory2" );
	if( nullptr != pCreateDXGIFactory2 )
	{
		bestAvailableDXGIVersion = TEST_DXGI_VERSION_1_3;
		bSuccess = true;
		goto Cleanup;
	}

	// If the IDXGIFactory2 interface is available, DXGI 1.2 is supported
	pCreateDXGIFactory = (PCREATEDXGIFACTORY)GetProcAddress( m_hDXGI, "CreateDXGIFactory" );
	if( nullptr == pCreateDXGIFactory )
	{
		// CreateDXGIFactory should always be available
		GetFramework()->WriteToLogF( L"CGraphicsTestFramework::GetBestAvailableDXGIVersion:  GetProcAddress for CreateDXGIFactory returned null." );
		goto Cleanup;
	}
	else
	{
		// CreateFactory should always pass
		if( FAILED( hr = pCreateDXGIFactory( __uuidof(IDXGIFactory), (void**)(&pIDXGIFactory) ) ) )
		{
			GetFramework()->WriteToLogF( L"CGraphicsTestFramework::GetBestAvailableDXGIVersion:  CreateDXGIFactory for IDXGIFactory returned HR = %s.", ToString(hr).c_str() );
			goto Cleanup;
		}

		// The DXGIFactory should always be available
		if( nullptr == pIDXGIFactory )
		{
			GetFramework()->WriteToLogF( L"CGraphicsTestFramework::GetBestAvailableDXGIVersion:  CreateDXGIFactory for IDXGIFactory returned null." );
			goto Cleanup;
		}

		// QI for the IDXGIFactory2 interface
		hr = pIDXGIFactory->QueryInterface( __uuidof(IDXGIFactory2), (void **)&pIDXGIFactory2 );
		if( SUCCEEDED(hr) )
		{
			bestAvailableDXGIVersion = TEST_DXGI_VERSION_1_2;
			bSuccess = true;
			goto Cleanup;
		}
	}
	
	// If CreateDXGIFactory1 is available, DXGI 1.1 is supported
	pCreateDXGIFactory1 = (PCREATEDXGIFACTORY1)GetProcAddress( m_hDXGI, "CreateDXGIFactory1" );
	if( nullptr != pCreateDXGIFactory1 )
	{
		bestAvailableDXGIVersion = TEST_DXGI_VERSION_1_1;
		bSuccess = true;
		goto Cleanup;
	}

	// If we got here, we passed (DXGI 1.0 is supported)
	bSuccess = true;

Cleanup:
	SAFE_RELEASE( pIDXGIFactory2 );
	SAFE_RELEASE( pIDXGIFactory );
	return bSuccess;
}


///////////////////////////////////////////////////////
// Function:  OptionCallback_DXGIVersion
//
// Purpose:
// Called by the test framework in response to a
// DXGI version specified on the command line.
///////////////////////////////////////////////////////
void CGraphicsTestFramework::OptionCallback_DXGIVersion
(
	const tstring &DXGIVersion
)
{
	//
	// Get a pointer to the framework.
	//

	CGraphicsTestFramework * pFramework = dynamic_cast< CGraphicsTestFramework *>( GetFramework() );
	assert( pFramework != NULL );


	//
	// Process the specified DXGI version string.
	//

	// Default level passed by the framework at startup.
	if
	(
		DXGIVersion == _T("default")
	)
	{
		// Just ignore this one.
	}
	else if
	(
		DXGIVersion == _T("1.0") ||
		DXGIVersion == _T("1_0") ||
		DXGIVersion == _T("TEST_DXGI_VERSION_1_0")
	)
	{
		pFramework->m_DXGIOptions.Version = TEST_DXGI_VERSION_1_0;
	}
	else if
	(
		DXGIVersion == _T("1.1") ||
		DXGIVersion == _T("1_1") ||
		DXGIVersion == _T("TEST_DXGI_VERSION_1_1")
	)
	{
		pFramework->m_DXGIOptions.Version = TEST_DXGI_VERSION_1_1;
	}
	else if
	(
		DXGIVersion == _T("1.2") ||
		DXGIVersion == _T("1_2") ||
		DXGIVersion == _T("TEST_DXGI_VERSION_1_2")
	)
	{
		pFramework->m_DXGIOptions.Version = TEST_DXGI_VERSION_1_2;
	}
	else if
	(
		DXGIVersion == _T("1.3") ||
		DXGIVersion == _T("1_3") ||
		DXGIVersion == _T("TEST_DXGI_VERSION_1_3")
	)
	{
		pFramework->m_DXGIOptions.Version = TEST_DXGI_VERSION_1_3;
	};
};


///////////////////////////////////////////////////////
// Function:  OptionCallback_AdapterType
//
// Purpose:
// Called from InitTestOptions in response to a
// adapter type switch.
///////////////////////////////////////////////////////
void CGraphicsTestFramework::OptionCallback_AdapterType( const TEST_DXGI_ADAPTER_TYPE& adapterType )
{
	CGraphicsTestFramework *pFramework = dynamic_cast< CGraphicsTestFramework *>( GetFramework() );
	pFramework->m_DXGIOptions.AdapterTypeSrc = 
	pFramework->m_DXGIOptions.AdapterTypeRef = static_cast< UINT >( adapterType );
}


///////////////////////////////////////////////////////
// Function:  OptionCallback_AdapterNumber
//
// Purpose:
// Called from InitTestOptions in response to an
// adapter number switch.
///////////////////////////////////////////////////////
void CGraphicsTestFramework::OptionCallback_AdapterNumber( const UINT &adapterNumber )
{
	CGraphicsTestFramework *pFramework = dynamic_cast< CGraphicsTestFramework *>( GetFramework() );
	pFramework->m_DXGIOptions.AdapterNumberSrc = 
	pFramework->m_DXGIOptions.AdapterNumberRef = adapterNumber;
}


///////////////////////////////////////////////////////
// Function:  OptionCallback_OutputID
//
// Purpose:
// Called from InitTestOptions in response to an
// output ID switch.
///////////////////////////////////////////////////////
void CGraphicsTestFramework::OptionCallback_OutputID( const UINT &outputID )
{
	CGraphicsTestFramework *pFramework = dynamic_cast< CGraphicsTestFramework *>( GetFramework() );
	pFramework->m_DXGIOptions.OutputIDRef = 
	pFramework->m_DXGIOptions.OutputIDSrc = outputID;
}


///////////////////////////////////////////////////////
// Function:  OptionCallback_RasterizerDLL
//
// Purpose:
// Called from InitTestOptions in response to the swdll
// switch.
///////////////////////////////////////////////////////
void CGraphicsTestFramework::OptionCallback_RasterizerDLL( const tstring &Rasterizer )
{
	if ( Rasterizer.length() == 0 )
	{
		return;
	};

	CGraphicsTestFramework *pFramework = dynamic_cast< CGraphicsTestFramework *>( GetFramework() );

	assert( pFramework != NULL );
	if ( pFramework == NULL )
	{
		return;
	};

#if defined(UNICODE)
	StringCchCopyW
	(
		pFramework->m_pD3DOptions->SoftwareRasterizerDLLName,
		ARRAYSIZE( pFramework->m_pD3DOptions->SoftwareRasterizerDLLName ),
		Rasterizer.c_str()
	);
#else
	MultiByteToWideChar
	(
		CP_ACP,
		MB_ERR_INVALID_CHARS,
		Rasterizer.c_str(),
		-1,
		pFramework->m_pD3DOptions->SoftwareRasterizerDLLName,
		ARRAYSIZE( pFramework->m_pD3DOptions->SoftwareRasterizerDLLName )
	);
#endif
};


//////////////////////////////////////////////////////////////////
// Function:  OptionCallback_DxgiDLL
//
// Purpose:
// Called from InitTestOptions in response to the DXGIDLL switch.
//////////////////////////////////////////////////////////////////
void CGraphicsTestFramework::OptionCallback_DxgiDLL( const tstring &DxgiDLL )
{
	if ( DxgiDLL.length() == 0 )
	{
		return;
	}

	CGraphicsTestFramework *pFramework = dynamic_cast< CGraphicsTestFramework *>( GetFramework() );

	assert( pFramework != NULL );
	if ( pFramework == NULL )
	{
		return;
	};

#if defined(UNICODE)
	StringCchCopyW
	(
		pFramework->m_pD3DOptions->DxgiDLLName,
		ARRAYSIZE( pFramework->m_pD3DOptions->DxgiDLL ),
		DxgiDLL.c_str()
	);
#else
	MultiByteToWideChar
	(
		CP_ACP,
		MB_ERR_INVALID_CHARS,
		DxgiDLL.c_str(),
		-1,
		pFramework->m_pD3DOptions->DxgiDLLName,
		ARRAYSIZE( pFramework->m_pD3DOptions->DxgiDLLName )
	);
#endif
};


///////////////////////////////////////////////////////
// Function:  OptionCallback_TerminalServices
//
// Purpose:
// Called by the test framework in response to the
// -ts command line switch.
///////////////////////////////////////////////////////
void CGraphicsTestFramework::OptionCallback_TerminalServices
(
	const bool &bTerminalServices
)
{
	//
	// Get a pointer to the framework.
	//

	CGraphicsTestFramework * pFramework = dynamic_cast< CGraphicsTestFramework *>( GetFramework() );
	assert( pFramework != NULL );


	//
	// Process the specified TS switch value.
	//

	switch ( bTerminalServices )
	{
		case true:
		{
			pFramework->m_bTerminalServices = bTerminalServices;

			pFramework->m_DXGIOptions.Version = TEST_DXGI_VERSION_1_1;
		} break;

		case false:
		{
			pFramework->m_bTerminalServices = bTerminalServices;
		} break;
	};
};

///////////////////////////////////////////////////////
// Function:  OptionCallback_ThreadingOptimizations
//
// Purpose:
// Called from InitTestOptions in response to a
// threading optimizations.
///////////////////////////////////////////////////////
void CGraphicsTestFramework::OptionCallback_ThreadingOptimizations( const TEST_EXPLICIT_OPTION_SWITCH& optionSwitch )
{
	CGraphicsTestFramework *pFramework = dynamic_cast< CGraphicsTestFramework *>( GetFramework() );
	assert( pFramework != NULL );

	if( optionSwitch == TEST_EXPLICIT_OPTION_SWITCH_DISABLED )
	{
		pFramework->m_pD3DOptions->PreventThreadingOptimizations = true;
	}
	else
	{
		pFramework->m_pD3DOptions->PreventThreadingOptimizations = false;
	}
}

///////////////////////////////////////////////////////
// Function:  OptionCallback_HybridOverride
//
// Purpose:
// Called by the test framework in response to a
// -hybrid on the command line
///////////////////////////////////////////////////////
void CGraphicsTestFramework::OptionCallback_HybridOverride
(
	const tstring &HybridOverride
)
{
	//
	// Get a pointer to the framework.
	//

	CGraphicsTestFramework * pFramework = dynamic_cast< CGraphicsTestFramework *>( GetFramework() );
	assert( pFramework != NULL );


	//
	// Process the specified override string.
	//

	// Default level passed by the framework at startup.
	if
	(
		ToLower(HybridOverride) == _T("forceintegrated")
	)
	{
		pFramework->m_DXGIOptions.uHybridOverride = TEST_DXGI_HYBRID_OVERRIDE_FORCE_INTEGRATED;
	}
	else if
	(
		ToLower(HybridOverride) == _T("forcediscrete") ||
		HybridOverride.size() == 0 // "-hybrid"
	)
	{
		pFramework->m_DXGIOptions.uHybridOverride = TEST_DXGI_HYBRID_OVERRIDE_FORCE_DISCRETE;
	}
	else if
	(
		ToLower(HybridOverride) == _T("none")
	)
	{
		pFramework->m_DXGIOptions.uHybridOverride = TEST_DXGI_HYBRID_OVERRIDE_NONE;
	}
};

////////////////////////////////////////////////////////////
//
// DXGI Debug Helper Methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  InitiateJournalDump
//
// Purpose:
// Dump DXGI's Journal to the DXGLogger or discard past journal entries
///////////////////////////////////////////////////////
void CGraphicsTestFramework::InitiateJournalDump(bool bWriteToLog)
{
	if( m_pDXGIDumpJournal == NULL )
	{
		if( !m_hDXGI )
			return;
		m_pDXGIDumpJournal = (PDXGIDUMPJOURNAL) GetProcAddress(m_hDXGI, "DXGIDumpJournal");
	}
	s_mbDumpToDXGLogger = bWriteToLog;
	if( m_pDXGIDumpJournal)
		m_pDXGIDumpJournal(CGraphicsTestFramework::WriteDXGIJournalToLog);
}

///////////////////////////////////////////////////////
// Function:  WriteDXGIJournalToLog
//
// Purpose:
// Callback for DXGIDumpJournal
///////////////////////////////////////////////////////
bool CGraphicsTestFramework::s_mbDumpToDXGLogger = false;
void CGraphicsTestFramework::WriteDXGIJournalToLog(const char *pMessage)
{
	OutputDebugString((LPSTR)pMessage);

	// Messages are formatted as follows: 
	// DXGIOutputMessage( void(*pfn)(const char *), ULONG32 Code, DWORD Thread, DWORD Tick, void* ip, const char * pMessage)
	if( s_mbDumpToDXGLogger )
		CTest::WriteToLog( "CGraphicsTestFramework::WriteDXGIJournalToLog: %s", pMessage );
}
