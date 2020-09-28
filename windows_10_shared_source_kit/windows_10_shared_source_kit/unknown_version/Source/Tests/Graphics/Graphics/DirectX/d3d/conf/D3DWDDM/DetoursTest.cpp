// Includes
#include "DetoursTest.h"
#include "psapi.h"
#include "d3d9.h"
#include <string>

// Extern
extern CD3DWDDMApp g_testApp;

// Namespaces
using namespace WindowsTest;
using namespace WindowsTest::Graphics::Direct3D;
using namespace WindowsTest::Graphics::Direct3D9;
using namespace WindowsTest::Graphics::DXGI;

// Create the strings for parameter names
const tstring g_featureLevelStr = "D3D11DeviceFeatureLevel";

const tstring CDetoursTest::s_disallowedModuleNames[] = 
{ 
	_T("detoured.dll")
	,_T("detours.dll")
};

#ifdef CDETOURSTEST_ENABLEDETOUREDTEST

// Global functions to detour to
typedef bool (* PFNVOIDTESTFN) (void);
PFNVOIDTESTFN g_pfnTestFunctionToDetour = nullptr;

FARPROC (WINAPI * g_pfnGetProcAddress)( HMODULE, LPCSTR ) = nullptr;
FARPROC (WINAPI * g_pfnGetProcAddressKernelBase)( HMODULE, LPCSTR ) = nullptr;
FARPROC WINAPI MyGetProcAddress( HMODULE hModule, LPCSTR lpProcName )
{
	if( g_pfnGetProcAddress )
	{
		return g_pfnGetProcAddress( hModule, lpProcName );
	}
	return nullptr;
}

HMODULE (WINAPI * g_pfnLoadLibraryA)( LPCTSTR lpFileName ) = nullptr;
HMODULE WINAPI MyLoadLibraryA( LPCTSTR lpFileName )
{
	if( g_pfnLoadLibraryA )
	{
		return g_pfnLoadLibraryA( lpFileName );
	}
	return nullptr;
}

HMODULE (WINAPI * g_pfnLoadLibraryW)( LPCTSTR lpFileName ) = nullptr;
HMODULE WINAPI MyLoadLibraryW( LPCTSTR lpFileName )
{
	if( g_pfnLoadLibraryW )
	{
		return g_pfnLoadLibraryW( lpFileName );
	}
	return nullptr;
}

HMODULE (WINAPI * g_pfnLoadLibraryExA)( LPCTSTR, HANDLE, DWORD ) = nullptr;
HMODULE WINAPI MyLoadLibraryExA( LPCTSTR lpFileName, HANDLE hFile, DWORD dwFlags )
{
	if( g_pfnLoadLibraryExA )
	{
		return g_pfnLoadLibraryExA( lpFileName, hFile, dwFlags );
	}
	return nullptr;
}

HMODULE (WINAPI * g_pfnLoadLibraryExW)( LPCTSTR, HANDLE, DWORD ) = nullptr;
HMODULE WINAPI MyLoadLibraryExW( LPCTSTR lpFileName, HANDLE hFile, DWORD dwFlags )
{
	if( g_pfnLoadLibraryExW )
	{
		return g_pfnLoadLibraryExW( lpFileName, hFile, dwFlags );
	}
	return nullptr;
}


HRESULT (WINAPI * g_pfnD3D10CreateDevice)( IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, ID3D10Device** ) = nullptr;
HRESULT WINAPI MyD3D10CreateDevice(
	IDXGIAdapter *pAdapter,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	UINT SDKVersion,
	ID3D10Device ** ppDevice)
{
	if( g_pfnD3D10CreateDevice )
	{
		return g_pfnD3D10CreateDevice( pAdapter, DriverType, Software, Flags, SDKVersion, ppDevice );
	}

	return E_NOTIMPL;
}

HRESULT (WINAPI * g_pfnD3D10CreateDeviceAndSwapChain)( IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D10Device** ) = nullptr;
HRESULT WINAPI MyD3D10CreateDeviceAndSwapChain(
	IDXGIAdapter *pAdapter,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	UINT SDKVersion,
	DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
	IDXGISwapChain **ppSwapChain,    
	ID3D10Device **ppDevice)
{
	if( g_pfnD3D10CreateDeviceAndSwapChain )
	{
		return g_pfnD3D10CreateDeviceAndSwapChain( pAdapter, DriverType, Software, Flags, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice );
	}

	return E_NOTIMPL;
}

PFN_D3D10_CREATE_DEVICE1 g_pfnD3D10CreateDevice1 = nullptr;
HRESULT WINAPI MyD3D10CreateDevice1(
	IDXGIAdapter *pAdapter,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	D3D10_FEATURE_LEVEL1 HardwareLevel,
	UINT SDKVersion,
	ID3D10Device1 **ppDevice)
{
	if( g_pfnD3D10CreateDevice1 )
	{
		return g_pfnD3D10CreateDevice1( pAdapter, DriverType, Software, Flags, HardwareLevel, SDKVersion, ppDevice );
	}

	return E_NOTIMPL;
}

PFN_D3D10_CREATE_DEVICE_AND_SWAP_CHAIN1 g_pfnD3D10CreateDeviceAndSwapChain1 = nullptr;
HRESULT WINAPI MyD3D10CreateDeviceAndSwapChain1(
	IDXGIAdapter *pAdapter,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	D3D10_FEATURE_LEVEL1 HardwareLevel,
	UINT SDKVersion,
	DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
	IDXGISwapChain **ppSwapChain,
	ID3D10Device1 **ppDevice)
{
	if( g_pfnD3D10CreateDeviceAndSwapChain1 )
	{
		return g_pfnD3D10CreateDeviceAndSwapChain1( pAdapter, DriverType, Software, Flags, HardwareLevel, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice );
	}

	return E_NOTIMPL;
}

HRESULT (WINAPI * g_pfnD3D11CreateDevice)( IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, CONST D3D_FEATURE_LEVEL*, UINT, UINT, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext** ) = nullptr;
HRESULT WINAPI MyD3D11CreateDevice(
	IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	CONST D3D_FEATURE_LEVEL* pFeatureLevels,
	UINT FeatureLevels,
	UINT SDKVersion,
	ID3D11Device** ppDevice,
	D3D_FEATURE_LEVEL* pFeatureLevel,
	ID3D11DeviceContext** ppImmediateContext )
{
	if( g_pfnD3D11CreateDevice )
	{
		return g_pfnD3D11CreateDevice( pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext );
	}

	return E_NOTIMPL;
}

HRESULT (WINAPI * g_pfnD3D11CreateDeviceAndSwapChain)( IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, CONST D3D_FEATURE_LEVEL*, UINT, UINT, CONST DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain** ppSwapChain, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext** ) = nullptr;
HRESULT WINAPI MyD3D11CreateDeviceAndSwapChain(
	IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	CONST D3D_FEATURE_LEVEL* pFeatureLevels,
	UINT FeatureLevels,
	UINT SDKVersion,
	CONST DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
	IDXGISwapChain** ppSwapChain,
	ID3D11Device** ppDevice,
	D3D_FEATURE_LEVEL* pFeatureLevel,
	ID3D11DeviceContext** ppImmediateContext )
{
	if( g_pfnD3D11CreateDeviceAndSwapChain )
	{
		return g_pfnD3D11CreateDeviceAndSwapChain( pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext );
	}

	return E_NOTIMPL;
}

HRESULT (WINAPI * g_pfnCreateDXGIFactory)( REFIID, void ** ) = nullptr;
HRESULT WINAPI MyCreateDXGIFactory( REFIID riid, void **ppFactory )
{
	if( g_pfnCreateDXGIFactory )
	{
		return g_pfnCreateDXGIFactory( riid, ppFactory );
	}

	return E_NOTIMPL;
}

HRESULT (WINAPI * g_pfnCreateDXGIFactory1)( REFIID, void ** ) = nullptr;
HRESULT WINAPI MyCreateDXGIFactory1( REFIID riid, void **ppFactory )
{
	if( g_pfnCreateDXGIFactory1 )
	{
		return g_pfnCreateDXGIFactory1( riid, ppFactory );
	}

	return E_NOTIMPL;
}

HRESULT (WINAPI * g_pfnCreateDXGIFactory2)( UINT, REFIID, void ** ) = nullptr;
HRESULT WINAPI MyCreateDXGIFactory2( UINT Flags, REFIID riid, void **ppFactory )
{
	if( g_pfnCreateDXGIFactory2 )
	{
		return g_pfnCreateDXGIFactory2( Flags, riid, ppFactory );
	}

	return E_NOTIMPL;
}

IDirect3D9 * (WINAPI * g_pfnDirect3DCreate9)(UINT) = nullptr;
IDirect3D9 * WINAPI MyDirect3DCreate9(UINT SDKVersion)
{
	if( g_pfnDirect3DCreate9 )
	{
		return g_pfnDirect3DCreate9( SDKVersion );
	}

	return nullptr;
}

/////

struct FunctionToAttachOrDetach
{
	tstring functionName;	// Name of the function to detour. Used for GetProcAddress and logging
	tstring moduleName;		// Name that the function resides in
	void**	pfn;			// The pointer to the function detour
	void*	pfnDetour;		// The pointer to the function that we want to detour to
};

const FunctionToAttachOrDetach g_functionsToDetour[] = 
{
	// Note we don't include the local function that I detour. We handle that as a special case below.
	{ _T("GetProcAddress"), _T("kernel32"), reinterpret_cast<void**>(&g_pfnGetProcAddress), MyGetProcAddress },
	{ _T("GetProcAddress"), _T("kernelbase"), reinterpret_cast<void**>(&g_pfnGetProcAddressKernelBase), MyGetProcAddress },

	{ _T("LoadLibraryA"), _T("kernel32"), reinterpret_cast<void**>(&g_pfnLoadLibraryA), MyLoadLibraryA },
	{ _T("LoadLibraryW"), _T("kernel32"), reinterpret_cast<void**>(&g_pfnLoadLibraryW), MyLoadLibraryW },
	{ _T("LoadLibraryExA"), _T("kernel32"), reinterpret_cast<void**>(&g_pfnLoadLibraryExA), MyLoadLibraryExA },
	{ _T("LoadLibraryExW"), _T("kernel32"), reinterpret_cast<void**>(&g_pfnLoadLibraryExW), MyLoadLibraryExW },

	{ _T("D3D10CreateDevice"), _T("d3d10"), reinterpret_cast<void**>(&g_pfnD3D10CreateDevice), MyD3D10CreateDevice },
	{ _T("D3D10CreateDeviceAndSwapChain"), _T("d3d10"), reinterpret_cast<void**>(&g_pfnD3D10CreateDeviceAndSwapChain), MyD3D10CreateDeviceAndSwapChain },
	{ _T("D3D10CreateDevice1"), _T("d3d10_1"), reinterpret_cast<void**>(&g_pfnD3D10CreateDevice1), MyD3D10CreateDevice1 },
	{ _T("D3D10CreateDeviceAndSwapChain1"), _T("d3d10_1"), reinterpret_cast<void**>(&g_pfnD3D10CreateDeviceAndSwapChain1), MyD3D10CreateDeviceAndSwapChain1 },
	{ _T("D3D11CreateDevice"), _T("d3d11"), reinterpret_cast<void**>(&g_pfnD3D11CreateDevice), MyD3D11CreateDevice },
	{ _T("D3D11CreateDeviceAndSwapChain"), _T("d3d11"), reinterpret_cast<void**>(&g_pfnD3D11CreateDeviceAndSwapChain), MyD3D11CreateDeviceAndSwapChain },

	{ _T("CreateDXGIFactory"), _T("dxgi"), reinterpret_cast<void**>(&g_pfnCreateDXGIFactory), MyCreateDXGIFactory },
	{ _T("CreateDXGIFactory1"), _T("dxgi"), reinterpret_cast<void**>(&g_pfnCreateDXGIFactory1), MyCreateDXGIFactory1 },
	{ _T("CreateDXGIFactory2"), _T("dxgi"), reinterpret_cast<void**>(&g_pfnCreateDXGIFactory2), MyCreateDXGIFactory2 },

	{ _T("Direct3DCreate9"), _T("d3d9"), reinterpret_cast<void**>(&g_pfnDirect3DCreate9), MyDirect3DCreate9 },
};
const UINT g_numFunctionsToDetour = ARRAY_SIZE( g_functionsToDetour );

#endif

////////////////////////

CDetoursTest::CDetoursTest(void)
	: m_pDevice(nullptr)
	, m_pDeviceContext(nullptr)
	, m_pD3D9(nullptr)
	, m_pD3D9Device(nullptr)
	, m_hD3D10(nullptr)
	, m_hD3D10_1(nullptr)
	, m_bDetoursAttached(false)
{
	m_adapters.clear();
	m_bTestAdapterDXGI.clear();
	m_bTestAdapterD3D9.clear();
}

void CDetoursTest::InitTestParameters()
{
	CUserValueSet<D3D_FEATURE_LEVEL> *pFeatureLevelValues = new CUserValueSet<D3D_FEATURE_LEVEL>();
	pFeatureLevelValues->AddValue( D3D_FEATURE_LEVEL_11_1 );
	pFeatureLevelValues->AddValue( D3D_FEATURE_LEVEL_11_0 );
	pFeatureLevelValues->AddValue( D3D_FEATURE_LEVEL_10_1 );
	pFeatureLevelValues->AddValue( D3D_FEATURE_LEVEL_10_0 );
	pFeatureLevelValues->AddValue( D3D_FEATURE_LEVEL_9_3 );
	pFeatureLevelValues->AddValue( D3D_FEATURE_LEVEL_9_2 );
	pFeatureLevelValues->AddValue( D3D_FEATURE_LEVEL_9_1 );

	testfactor::RFactor m_rfFeatureLevel = AddParameter<D3D_FEATURE_LEVEL>( _T( g_featureLevelStr ), &m_featureLevel, pFeatureLevelValues );
	SetRootTestFactor( m_rfFeatureLevel );
}

TEST_RESULT CDetoursTest::Setup()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = E_FAIL;
	UINT numAdaptersToTest = 0;
	tstring adaptersToTestStr = "";

	if( GetFramework()->IsMobile() )
	{
		WriteToLog( _T("Not applicable on mobile, skipping.") );
		tr = RESULT_SKIP;
		return tr;
	}

#ifndef CDETOURSTEST_ENABLEDETOUREDTEST
	WriteToLog( _T("This test can check itself by detouring system and D3D functions, and then making sure that it can detect the detours. To enable this check, uncomment the CDETOURSTEST_ENABLEDETOUREDTEST macro definition and then recompile.") );
#endif

	//
	// Create a D3D9 interface
	//

	m_pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );
	if( !m_pD3D9 )
	{
		LogError( __FILEW__, __LINE__, L"Direct3DCreate9() failed.", false, ERRORCODE_TYPE_BOOL, gFCDirect3DCreate9 );
		tr = RESULT_FAIL;
	}

	//
	// Enum adapters in the system and identify which adapters the test should run on (HW w/ WDDM 1.3 or higher) 
	//

	for(UINT adapterIndex = 0; true; adapterIndex++)
	{
		ATL::CComPtr<IDXGIAdapter1> pAdapter = nullptr;
		if( FAILED( hr = GetFramework()->GetDXGIFactory1()->EnumAdapters1( adapterIndex, &pAdapter ) ) )
		{
			if( DXGI_ERROR_NOT_FOUND == hr )
			{
				// We have enumerated all adapters in the system. Break out.
				break;
			}
			else
			{
				// We failed to enumerate...
				WriteToLog( _T("EnumAdapters1() failed on adapter index %d"), adapterIndex );
				LogError( __FILEW__, __LINE__, L"EnumAdapters1() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDXGIEnumAdapters );
				tr = RESULT_FAIL;
			}
		}

		m_adapters.push_back( new CAdapter(pAdapter) );
	}

	// Log the adapters that we found
	// Indicate what adapters we will test on
	BeginSection( L"Enumerate DXGI Adapters" );
	WriteToLog( _T("DXGI: EnumAdapters1() found %d adapter(s) in the current system:"), m_adapters.size() );
	for( UINT i = 0; i < m_adapters.size(); i++ )
	{
		// Check that the adapter is valid
		if( !m_adapters[i]->IsValid() )
		{
			WriteToLog( _T("     Adapter #%d: Not valid"), i );
			continue;
		}

		D3DKMT_DRIVERVERSION kmtDriverVersion;
		if( FAILED( hr = m_adapters[i]->GetDescriptor()->GetDisplayDriverModel( kmtDriverVersion ) ) )
		{
			WriteToLog( _T("     Adapter #%d: Failed on GetDisplayDriverModel()"), i );
			LogError( __FILEW__, __LINE__, L"GetDisplayDriverModel() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCRequiredCapabilityNotSupported );
			tr = RESULT_FAIL;
		}

		// Should we test the current adapter? Only if it supports WDDM 1.3 or higher (default).
		if( g_testApp.RunOnAllWDDMVersions() )
		{
			m_bTestAdapterDXGI.push_back( true );
		}
		else
		{
			m_bTestAdapterDXGI.push_back( kmtDriverVersion >= KMT_DRIVERVERSION_WDDM_1_3 );
		}
		

		WriteToLog( L"     Adapter #%d: %s (KMT WDDM Driver Model: %S, Driver Version: %S)", i, 
			m_adapters[i]->GetDescriptor()->GetDxgiAdapterDesc1().Description,
			ToString(kmtDriverVersion).c_str(),
			m_adapters[i]->GetUserModeDriverVersionStr().c_str());
	}

	assert( m_bTestAdapterDXGI.size() == m_adapters.size() );
	for( UINT i = 0; i < m_bTestAdapterDXGI.size(); i++ )
	{
		if( m_bTestAdapterDXGI[i] )
		{
			if( numAdaptersToTest > 0 )
			{
				adaptersToTestStr += _T(", "); // Prefix
			}

			adaptersToTestStr += ToString(i);
			numAdaptersToTest++;
		}
	}

	WriteToLog( _T("This test will only run on DXGI adapters with a WDDM 1.3+ driver. Use \"-RunOnAllWDDMVersions\" to run on all WDDM versions.") );
	if( 0 == numAdaptersToTest )
	{
		WriteToLog( _T("The are no DXGI adapters in the current system that support the WDDM 1.3+ driver model. Skipping group.") );
		tr = RESULT_SKIP;
		goto Done;
	}
	else
	{
		WriteToLog( _T("The test will run on DXGI adapter(s): %s"), adaptersToTestStr.c_str() );
	}

	EndSection();

	BeginSection( L"Enumerate D3D9 Adapters" );
	
	const UINT numAdaptersD3D9 = m_pD3D9->GetAdapterCount();
	WriteToLog( _T("D3D9: GetAdapterCount() found %d adapter(s) in the current system:"), numAdaptersD3D9 );
	for( UINT i = 0; i < numAdaptersD3D9; i++ )
	{
		// Indicate we won't test this adapter for now
		m_bTestAdapterD3D9.push_back( true );		// For now...always test every D3D9 adapter
		//m_bTestAdapterD3D9.push_back( false );	// If we want to be selective about what D3D9 adapter we'll test...uncomment this.

		D3DADAPTER_IDENTIFIER9 identifier;
		if( FAILED( hr = m_pD3D9->GetAdapterIdentifier( i, 0, &identifier ) ) )
		{
			LogError( __FILEW__, __LINE__, L"GetAdapterIdentifier() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCGetAdapterIdentifier );
			tr = RESULT_FAIL;
		}

		// See if this adapter matches any DXGI adapter descriptons (if so, we'll run on that adapter)
		// Convert from char* to a wchar*
		const std::string desc(identifier.Description);
		const CCHAR *pDesc = desc.c_str();
		size_t origsize = strlen(identifier.Description) + 1;
		const size_t newsize = 1024;
		size_t convertedChars = 0;
		WCHAR wcstr[newsize];
		mbstate_t mbst = { 0 }; 
		mbsrtowcs_s( &convertedChars, wcstr, newsize, &pDesc, origsize, &mbst );
		const std::wstring d3d9AdapterDesc( wcstr );
		//for( UINT j = 0; j < m_adapters.size(); j++ )
		//{
		//	const std::wstring dxgiAdapterDesc(m_adapters[j]->GetDescriptor()->GetDxgiAdapterDesc1().Description);
		//	if( ToUpper(dxgiAdapterDesc) == ToUpper(d3d9AdapterDesc) )
		//	{
		//		// We found a match, should this adapter be tested?
		//		if( m_bTestAdapterDXGI[j] )
		//		{
		//			// We are testing the DXGI adapter, so also test the corresponding D3D9 adapter
		//			m_bTestAdapterD3D9[i] = true;
		//			break;
		//		}
		//	}
		//}

		WriteToLog( L"     Adapter #%d: %s (Driver Version: %S)", i, d3d9AdapterDesc.c_str(), CAdapter::GetUserModeDriverVersionStr( identifier.DriverVersion ).c_str());
	}

	//numAdaptersToTest = 0;
	//adaptersToTestStr = "";
	//for( UINT i = 0; i < m_bTestAdapterD3D9.size(); i++ )
	//{
	//	if( m_bTestAdapterD3D9[i] )
	//	{
	//		if( numAdaptersToTest > 0 )
	//		{
	//			adaptersToTestStr += _T(", "); // Prefix
	//		}

	//		adaptersToTestStr += ToString(i);
	//		numAdaptersToTest++;
	//	}
	//}

	//WriteToLog( _T("This test will only run on D3D9 adapters that would have run on the corresponding DXGI adapter (see above).") );
	if( 0 == numAdaptersToTest )
	{
		WriteToLog( _T("The are no D3D9 adapters in the current system that support the WDDM 1.3+ driver model. See DXGI adapters above.") );
	}
	else
	{
		WriteToLog( _T("The test will run on D3D9 adapter(s): %s"), adaptersToTestStr.c_str() );
	}

	EndSection();

Done:
	return tr;
}

TEST_RESULT CDetoursTest::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;

	// Load D3D10
	m_hD3D10 = GetModuleHandle( _T("d3d10") );
	if( !m_hD3D10 )
	{
		m_hD3D10 = LoadLibrary( _T("d3d10") );
	}

	// Load D3D10_1
	m_hD3D10_1 = GetModuleHandle( _T("d3d10_1") );
	if( !m_hD3D10_1 )
	{
		m_hD3D10_1 = LoadLibrary( _T("d3d10_1") );
	}

	if( !m_hD3D10 || !m_hD3D10_1 )
	{
		LogError( __FILEW__, __LINE__, L"LoadLibrary failed for D3D10 dlls.", HRESULT_FROM_WIN32(GetLastError()), ERRORCODE_TYPE_HRESULT, gFCRequiredCapabilityNotSupported );
		tr = RESULT_FAIL;
	}

	return tr;
}

TEST_RESULT CDetoursTest::ExecuteTestCase()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;

	//
	// Check the test can detects detours
	//

#ifdef CDETOURSTEST_ENABLEDETOUREDTEST
	BeginSection( L"Check the test can detect detours" );
	if( !CheckTestDetectsDetours() )
	{
		WriteToLog( _T("CheckTestDetectsDetours failed.") );
		EndSection();
		tr = RESULT_FAIL;
		goto Done;	// Early exit if the test is faulty.
	}
	EndSection();
#endif

	//
	// Check for disallowed loaded modules
	//

	BeginSection( L"Check for disallowed loaded modules" );
	if( !CheckForDisallowedLoadedModules() )
	{
		WriteToLog( _T("CheckForDisallowedLoadedModules failed.") );
		tr = RESULT_FAIL;
		// Don't early exit. We should run all tests.
	}
	EndSection();

	//
	// Check for detoured system functions
	// LoadLibrary, GetProcAddress, GDI query functions, etc...
	//

	BeginSection( L"Check for detoured system functions" );
	if( !CheckForDetouredSystemFunctions() )
	{
		WriteToLog( _T("CheckForDetouredSystemFunctions indicated that functions were detoured.") );
		LogError( __FILEW__, __LINE__, L"System functions were detoured.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
		tr = RESULT_FAIL;
		// Don't early exit. We should run all tests.
	}
	EndSection();

	//
	// Check for detoured D3D functions
	// D3D11CreateDevice, CreateDevice, etc...
	//

	BeginSection( L"Check for detoured D3D functions" );
	if( !CheckForDetouredD3DFunctions() )
	{
		WriteToLog( _T("CheckForDetouredD3DFunctions indicated that functions were detoured.") );
		LogError( __FILEW__, __LINE__, L"D3D/DXGI functions were detoured.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
		tr = RESULT_FAIL;
		// Don't early exit. We should run all tests.
	}
	EndSection();

	//
	// Check that vtables are present in the correct module spaces
	//

	BeginSection( L"Check that vtables are present in the correct module spaces" );
	if( !CheckVTableModuleSpaces() )
	{
		WriteToLog( _T("CheckVTableModuleSpaces failed.") );
		// Error logged within CheckVTableModuleSpaces
		tr = RESULT_FAIL;
		// Don't early exit. We should run all tests.
	}
	EndSection();

	//
	// Check for detoured system functions again
	// LoadLibrary, GetProcAddress, GDI query functions, etc...
	//

	BeginSection( L"Check for detoured system functions again" );
	if( !CheckForDetouredSystemFunctions() )
	{
		WriteToLog( _T("CheckForDetouredSystemFunctions indicated that functions were detoured.") );
		LogError( __FILEW__, __LINE__, L"System functions were detoured.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
		tr = RESULT_FAIL;
		// Don't early exit. We should run all tests.
	}
	EndSection();

	//
	// Check for detoured D3D functions again
	// D3D11CreateDevice, CreateDevice, etc...
	//

	BeginSection( L"Check for detoured D3D functions again" );
	if( !CheckForDetouredD3DFunctions() )
	{
		WriteToLog( _T("CheckForDetouredD3DFunctions indicated that functions were detoured.") );
		LogError( __FILEW__, __LINE__, L"D3D/DXGI functions were detoured.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
		tr = RESULT_FAIL;
		// Don't early exit. We should run all tests.
	}
	EndSection();

#ifdef CDETOURSTEST_ENABLEDETOUREDTEST
Done:
#endif
	return tr;
}

void CDetoursTest::CleanupTestCase()
{
	// ATL pointers will cleanup themselves

	// Unload D3D10
	if( m_hD3D10 )
	{
		FreeLibrary( m_hD3D10 );
		m_hD3D10 = nullptr;
	}

	// Unload D3D10_1
	if( m_hD3D10_1 )
	{
		FreeLibrary( m_hD3D10_1 );
		m_hD3D10_1 = nullptr;
	}
}

void CDetoursTest::Cleanup()
{
	for( UINT i = 0; i < m_adapters.size(); i++ )
	{
		delete m_adapters[i];
	}

	// ATL pointers will cleanup themselves
}

////////////////////////
// Helpers

bool CDetoursTest::CheckForDisallowedLoadedModules()
{
	// Local variables
	bool bSuccess = false;
	const UINT testProcessID = GetCurrentProcessId();
	std::vector<tstring> loadedModules;
	std::vector<tstring> disallowedModules;
	TCHAR testProcessName[1024];

	// Get the name of the process for logging
	GetProcessImageFileName( GetCurrentProcess(), testProcessName, ARRAY_SIZE(testProcessName) );

	//
	// Get the module names for the process
	//

	loadedModules.clear();
	disallowedModules.clear();

	loadedModules = GetModuleNamesForProcess( testProcessID );
	WriteToLog( _T("PID %d (%s) has %d loaded modules."), testProcessID, testProcessName, loadedModules.size() );

	if( !GetFramework()->LogVerbose() )
	{
		WriteToLog( _T("To see all loaded modules, run the test with \"-logverbose\".") );
	}

	// Check to see if there are any loaded modules from our disallowed list
	const UINT numDisallowedModuleNames = ARRAY_SIZE(s_disallowedModuleNames);
	for( std::vector<tstring>::iterator iter = loadedModules.begin(); iter != loadedModules.end(); iter++ )
	{
		// Parse the module name from the full path
		const tstring currModuleFullPath = *iter;
		const size_t slashIndex = currModuleFullPath.find_last_of("/\\");
		const tstring currModuleName = currModuleFullPath.substr( slashIndex + 1 );

		// Display all found modules
		if( GetFramework()->LogVerbose() )
		{
			WriteToLog( _T("Loaded module: %s (%s)"), currModuleName.c_str(), currModuleFullPath.c_str() );
		}

		// For each "bad" module...see if we have a match
		for( UINT i = 0; i < numDisallowedModuleNames; i++ )
		{
			if( ToUpper(currModuleName) == ToUpper(s_disallowedModuleNames[i]) )
			{
				WriteToLog( _T("Disallowed module: %s"), currModuleName.c_str() );
				disallowedModules.push_back(*iter);
				break;	// Break out of the inner loop
			}
		}
	}

	WriteToLog( _T("Found %d disallowed loaded modules."), disallowedModules.size() );
	if( disallowedModules.size() > 0 )
	{
		LogError( __FILEW__, __LINE__, L"Disallowed loaded modules were found in the process.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
		goto Done;
	}

	// If we got here, we passed
	bSuccess = true;

Done:
	return bSuccess;
}

/////

bool CDetoursTest::CheckForDetouredSystemFunctions( const bool &bDetoursExpected )
{
	// Local variables
	bool bSuccess = true;

	// Check system functions
	if( !CheckFunctionDetours( IsFunctionDetoured( LoadLibraryA, _T("LoadLibraryA") ), bDetoursExpected ) ) { bSuccess = false; }
	if( !CheckFunctionDetours( IsFunctionDetoured( LoadLibraryW, _T("LoadLibraryW") ), bDetoursExpected ) ) { bSuccess = false; }
	if( !CheckFunctionDetours( IsFunctionDetoured( LoadLibraryExA, _T("LoadLibraryExA") ), bDetoursExpected ) ) { bSuccess = false; }
	if( !CheckFunctionDetours( IsFunctionDetoured( LoadLibraryExW, _T("LoadLibraryExW") ), bDetoursExpected ) ) { bSuccess = false; }
	if( !CheckFunctionDetours( IsFunctionDetoured( GetProcAddress, _T("GetProcAddress") ), bDetoursExpected ) ) { bSuccess = false; }
	
	// TODO: Add GDI query functions
	return bSuccess;
}

/////

bool CDetoursTest::CheckForDetouredD3DFunctions( const bool &bDetoursExpected )
{
	// Local variables
	bool bSuccess = true;

	// Check D3D functions
	if( !CheckFunctionDetours( IsFunctionDetoured( GetProcAddress( GetModuleHandle( _T("d3d10") ), _T("D3D10CreateDevice") ), _T("D3D10CreateDevice") ), bDetoursExpected ) )  { bSuccess = false; }
	if( !CheckFunctionDetours( IsFunctionDetoured( GetProcAddress( GetModuleHandle( _T("d3d10") ), _T("D3D10CreateDeviceAndSwapChain") ), _T("D3D10CreateDeviceAndSwapChain") ), bDetoursExpected ) ) { bSuccess = false; }
	if( !CheckFunctionDetours( IsFunctionDetoured( GetProcAddress( GetModuleHandle( _T("d3d10_1") ), _T("D3D10CreateDevice1") ), _T("D3D10CreateDevice1") ), bDetoursExpected ) )  { bSuccess = false; }
	if( !CheckFunctionDetours( IsFunctionDetoured( GetProcAddress( GetModuleHandle( _T("d3d10_1") ), _T("D3D10CreateDeviceAndSwapChain1") ), _T("D3D10CreateDeviceAndSwapChain1") ), bDetoursExpected ) ) { bSuccess = false; }
	if( !CheckFunctionDetours( IsFunctionDetoured( GetProcAddress( GetModuleHandle( _T("d3d11") ), _T("D3D11CreateDevice") ), _T("D3D11CreateDevice") ), bDetoursExpected ) )  { bSuccess = false; }
	if( !CheckFunctionDetours( IsFunctionDetoured( GetProcAddress( GetModuleHandle( _T("d3d11") ), _T("D3D11CreateDeviceAndSwapChain") ), _T("D3D11CreateDeviceAndSwapChain") ), bDetoursExpected ) ) { bSuccess = false; }
	
	// Check DXGI functions
	if( !CheckFunctionDetours( IsFunctionDetoured( GetProcAddress( GetModuleHandle( _T("dxgi") ), _T("CreateDXGIFactory") ), _T("CreateDXGIFactory") ), bDetoursExpected ) )  { bSuccess = false; }
	if( !CheckFunctionDetours( IsFunctionDetoured( GetProcAddress( GetModuleHandle( _T("dxgi") ), _T("CreateDXGIFactory1") ), _T("CreateDXGIFactory1") ), bDetoursExpected ) )  { bSuccess = false; }
	if( !CheckFunctionDetours( IsFunctionDetoured( GetProcAddress( GetModuleHandle( _T("dxgi") ), _T("CreateDXGIFactory2") ), _T("CreateDXGIFactory2") ), bDetoursExpected ) )  { bSuccess = false; }

	// Check D3D9 functions
	if( !CheckFunctionDetours( IsFunctionDetoured( GetProcAddress( GetModuleHandle( _T("d3d9") ), _T("Direct3DCreate9") ), _T("Direct3DCreate9") ), bDetoursExpected ) )  { bSuccess = false; }

	return bSuccess;
}

/////

bool CDetoursTest::CheckFunctionDetours( const bool &bFunctionIsDetoured, const bool &bDetouredFunctionExpected )
{
	// Check if the function was detoured when it shouldn't be...or if it wasn't detoured whern it should be.
	if( (bFunctionIsDetoured && !bDetouredFunctionExpected) ||
		(!bFunctionIsDetoured && bDetouredFunctionExpected) )
	{
		return false;
	}

	return true;
}

/////
class Window
{
public:
	Window(unsigned Width = 0x100, unsigned Height = 0x100, const wchar_t* WindowClass = L"Test Window", const wchar_t* WindowTitle = L"Test Window")
		: className(WindowClass)
	{
		WNDCLASSEXW Wc = WNDCLASSEXW();
		Wc.cbSize           = sizeof(WNDCLASSEXW);
		Wc.style            = CS_HREDRAW | CS_VREDRAW;
		Wc.hInstance        = GetModuleHandle(NULL);
		Wc.lpszClassName    = WindowClass;
		Wc.lpfnWndProc      = DefWindowProcW;

		if (!RegisterClassExW(&Wc))
		{
			return;
		}

		hWnd = CreateWindowW(WindowClass, WindowTitle, WS_OVERLAPPEDWINDOW, 0, 0, Width, Height, NULL, NULL, GetModuleHandle(NULL), NULL);
		ShowWindow(hWnd, SW_NORMAL);
	}

	~Window()
	{
		if (hWnd != 0)
		{
			DestroyWindow(hWnd);
		}
		UnregisterClassW(className.c_str(), GetModuleHandle(NULL));
	}

	std::wstring className;
	HWND hWnd = 0;
};

bool CDetoursTest::CheckVTableModuleSpaces()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool bVTablesArePresentInTheCorrectModuleSpaces = true;
	D3D_FEATURE_LEVEL outputFL = D3D_FEATURE_LEVEL_9_1;

	//
	// Create a D3D11 device
	//

	// For each DXGI adapter...
	assert( m_adapters.size() == m_bTestAdapterDXGI.size() );
	for( UINT i = 0; i < m_bTestAdapterDXGI.size(); i++ )
	{
		if( !m_bTestAdapterDXGI[i] )
		{
			// Don't test the current adapter...
			continue;
		}

		WriteToLog( _T("Checking VTable for D3D11 device on DXGI adapter #%d..."), i );

		hr = D3D11CreateDevice( 
			m_adapters[i]->GetAdapter(),	// Current DXGI adapter
			D3D_DRIVER_TYPE_UNKNOWN,
			nullptr,
			0,								// Flags
			&m_featureLevel,				// Current feature level
			1,
			D3D11_SDK_VERSION,
			&m_pDevice,
			&outputFL,
			&m_pDeviceContext );
		if( FAILED( hr ) )
		{
			if( hr == DXGI_ERROR_UNSUPPORTED )
			{
				WriteToLog( _T("DXGI Adapter #%d doesn't support the current feature level (%s). Skipping the current adapter."), i, ToString(m_featureLevel).c_str() );
				continue;
			}

			LogError( __FILEW__, __LINE__, L"D3D11CreateDevice failed.", hr, ERRORCODE_TYPE_HRESULT, gFCD3D11CreateDevice );
			bVTablesArePresentInTheCorrectModuleSpaces = false;	// Fail the check
			continue;
		}

		// Check that the vtable is in the correct module space
		size_t *vptr = (size_t *)m_pDevice.p;
		vptr = (size_t *)*vptr;
		if( !IsAddressInModuleSpace( vptr, _T("d3d11") ) )
		{
			LogError( __FILEW__, __LINE__, L"IsAddressInModuleSpace failed.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			bVTablesArePresentInTheCorrectModuleSpaces = false;
			continue;
		}
	}

	//
	// Create a D3D9 device
	//

	Window wnd;
	// For each D3D9 adapter...
	for( UINT i = 0; i < m_bTestAdapterD3D9.size(); i++ )
	{
		if( !m_bTestAdapterD3D9[i] )
		{
			// Don't test the current adapter...
			continue;
		}

		WriteToLog( _T("Checking VTable for D3D9 device on D3D9 adapter #%d..."), i );

		D3DPRESENT_PARAMETERS pp; 
		ZeroMemory( &pp, sizeof(pp) );
		pp.Windowed   = TRUE;
		pp.SwapEffect = D3DSWAPEFFECT_DISCARD;

		hr = m_pD3D9->CreateDevice( i,
			D3DDEVTYPE_HAL,
			wnd.hWnd,														// Focus window
			D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,	// Flags
			&pp,
			&m_pD3D9Device );
		if( FAILED( hr ) )
		{
			LogError( __FILEW__, __LINE__, L"IDirect3D9::CreateDevice failed.", hr, ERRORCODE_TYPE_HRESULT, gFCCreateDevice );
			bVTablesArePresentInTheCorrectModuleSpaces = false;	// Fail the check
			continue;
		}

		// Check that the vtable is in the correct module space
		// D3D9's vtable is on the heap, so we'll check QueryInterface instead (double dereference)
		size_t *vptr = (size_t *)m_pD3D9Device.p;
		vptr = (size_t *)*vptr;
		vptr = (size_t *)*vptr;	// Double dereference to find QueryInterface
		if( !IsAddressInModuleSpace( vptr, _T("d3d9") ) )
		{
			LogError( __FILEW__, __LINE__, L"IsAddressInModuleSpace failed.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			bVTablesArePresentInTheCorrectModuleSpaces = false;
			continue;
		}
	}

	return bVTablesArePresentInTheCorrectModuleSpaces;
}

/////

bool CDetoursTest::IsFunctionDetoured( void* pfnToCheck, const tstring fnName )
{
	// Local variables
	bool bFunctionIsDetoured = false;
	tstring architecture = "NOT SET. Test Bug!";

	// Check for null
	if( !pfnToCheck )
	{
		WriteToLog( _T("NULLPTR for function: %s. Unable to check for detour."), fnName.c_str() );
		bFunctionIsDetoured = false;
		return bFunctionIsDetoured;
	}
	
	// Build our architecture string
#ifdef _X86_
	architecture = "X86";
#elif _AMD64_
	architecture = "AMD64";
#elif _ARM_
	architecture = "ARM";
#elif _ARM64_
	architecture = "ARM64";
#else
#error Unrecognized architecture.
#endif

	// Check if the function has been detoured based on the architecture
#if defined _X86_ || defined _AMD64_
	BYTE* byte = (BYTE*)pfnToCheck;
	switch( byte[0] )
	{
		case 0xEA:
		case 0xEB:
		case 0xE9:
		{
			bFunctionIsDetoured = true;
			WriteToLog( _T("Found JMP Opcode: %#X"), byte[0] );
		} break;

		case 0xFF:
		{
			// According to Table A - 6 Opcode Extensions for One - and Two - byte Opcodes by Group Number
			// of Vol 2B of “Intel® 64 and IA - 32 Architectures Software Developer’s Manual”,
			// 0xFF opcodes with the second byte's Bits 5, 4, 3s encoded as 4 or 5 are JMP instructions.
			const BYTE bits543 = (byte[1] >> 3) & 0x7;
			if ((bits543 == 0x4) || (bits543 == 0x5))
			{
				bFunctionIsDetoured = true;
				WriteToLog( _T("Found JMP Opcode: %#X (Byte[1] = %#X)"), byte[0], byte[1] );
			}
		} break;
	}
#elif _ARM_
	architecture = "ARM";
	WriteToLog( _T("ARM detour detection not implemented!") );
#elif _ARM64_
	architecture = "ARM64";
	WriteToLog(_T("ARM64 detour detection not implemented!"));
#else
#error Unrecognized architecture.
#endif

	WriteToLog( _T("%s (%#X) %s been detoured (Architecture: %s)"), fnName.c_str(), pfnToCheck, bFunctionIsDetoured ? _T("has") : _T("has NOT"), architecture.c_str() );
	return bFunctionIsDetoured;
}

/////

bool CDetoursTest::IsAddressInModuleSpace( void* pToCheck, const tstring moduleName )
{
	// Local variables
	bool bAddressInModuleSpace = false;
	HMODULE hModule = nullptr;
	MODULEINFO moduleInfo = { 0 };

	//
	// Load the specified module and it's information
	//

	hModule = LoadLibrary( moduleName.c_str() );
	if( !hModule )
	{
		LogError( __FILEW__, __LINE__, L"LoadLibrary failed.", HRESULT_FROM_WIN32(GetLastError()), ERRORCODE_TYPE_HRESULT, gFCLoadLibrary );
		goto Done;
	}

	if( !GetModuleInformation( GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo) ) )
	{
		LogError( __FILEW__, __LINE__, L"GetModuleInformation failed.", HRESULT_FROM_WIN32(GetLastError()), ERRORCODE_TYPE_HRESULT, gFCGetModuleInformation );
		goto Done;
	}

	const BYTE *pModuleBase = reinterpret_cast<BYTE*>(moduleInfo.lpBaseOfDll);
	const BYTE *pModuleEnd = pModuleBase + moduleInfo.SizeOfImage;	// SizeOfImage is in BYTES
	bAddressInModuleSpace = (pToCheck >= pModuleBase) && (pToCheck < pModuleEnd);
	WriteToLog( _T("Address %#X %s within %s's module space ([%#X, %#X])"), pToCheck, bAddressInModuleSpace ? _T("is") : _T("is NOT"), moduleName.c_str(), pModuleBase, pModuleEnd );
	
Done:
	if( hModule )
	{
		FreeLibrary( hModule );
		hModule = nullptr;
	}

	return bAddressInModuleSpace; 
}

/////

std::vector<tstring> CDetoursTest::GetModuleNamesForProcess( const DWORD &processID )
{
	// Local variables
	HMODULE hMods[1024];
	HANDLE hProcess = nullptr;
	DWORD cbNeeded = 0;
	std::vector<tstring> moduleNames;
	moduleNames.clear();

	// Get a handle to the process
	hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
							PROCESS_VM_READ,
							FALSE, processID );
	if (NULL == hProcess)
	{
		return moduleNames;
	}

	// Get a list of all the modules in this process
	if( EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_ALL) )
	{
		for ( UINT i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
		{
			// Get the full path to the module's file
			TCHAR szModName[MAX_PATH];
			if ( GetModuleFileNameEx( hProcess, hMods[i], szModName,
									  sizeof(szModName) / sizeof(TCHAR)))
			{
				moduleNames.push_back( tstring(szModName) );
			}
		}
	}
	
	// Release the handle to the process
	CloseHandle( hProcess );

	return moduleNames;
}

/////

#ifdef CDETOURSTEST_ENABLEDETOUREDTEST

bool CDetoursTest::CheckTestDetectsDetours()
{
	// Local variables
	bool bTestCatchesDetours = false;
	LONG retVal = 0;
	bool bDetoursAttached = false;

	//
	// Check that system and D3D functions are NOT detoured
	//

	BeginSection( L"Check that system and D3D functions are NOT detoured" );
	if( !CheckForDetouredSystemFunctions() )
	{
		LogError( __FILEW__, __LINE__, L"System functions were detoured when they should NOT be.", true, ERRORCODE_TYPE_BOOL, gFCLogicError );
		EndSection();
		goto Done;
	}

	if( !CheckForDetouredD3DFunctions() )
	{
		LogError( __FILEW__, __LINE__, L"D3D functions were detoured when they should NOT be.", true, ERRORCODE_TYPE_BOOL, gFCLogicError );
		EndSection();
		goto Done;
	}

	// Call the detoured function
	// This should really call TestFunctionToDetour, which should return false
	if( IsFunctionDetoured( CDetoursTest::TestFunctionToDetour, _T("CDetoursTest::TestFunctionToDetour") ) ||
		TestFunctionToDetour() )	
	{
		LogError( __FILEW__, __LINE__, L"CDetoursTest::TestFunctionToDetour was detoured when it should NOT be.", true, ERRORCODE_TYPE_BOOL, gFCLogicError );
		EndSection(); 
		goto Done;
	}
	EndSection();

	//
	// Detour system and D3D functions
	//

	WriteToLog( _T("Attaching detours to system and D3D functions...") );
	if( !AttachDetoursToSystemAndD3DFunctions() )
	{
		WriteToLog( _T("AttachDetoursToSystemAndD3DFunctions failed.") );
		goto Done;
	}

	//
	// Check that system and D3D functions are detoured
	//

	BeginSection( L"Check that system and D3D functions are detoured" );
	if( !CheckForDetouredSystemFunctions( true ) )
	{
		LogError( __FILEW__, __LINE__, L"System functions were NOT detoured when they should be.", true, ERRORCODE_TYPE_BOOL, gFCLogicError );
		EndSection();
		goto Done;
	}

	if( !CheckForDetouredD3DFunctions( true ) )
	{
		LogError( __FILEW__, __LINE__, L"D3D functions were NOT detoured when they should be.", true, ERRORCODE_TYPE_BOOL, gFCLogicError );
		EndSection();
		goto Done;
	}

	// Call the detoured function
	// This should really call TestFunctionToDetourTo, which should return true
	if( !IsFunctionDetoured( CDetoursTest::TestFunctionToDetour, _T("CDetoursTest::TestFunctionToDetour") ) ||
		!TestFunctionToDetour() )	
	{
		LogError( __FILEW__, __LINE__, L"CDetoursTest::TestFunctionToDetour was NOT detoured when it should be.", true, ERRORCODE_TYPE_BOOL, gFCLogicError );
		EndSection(); 
		goto Done;
	}
	EndSection();

	//
	// The test doesn't load detoured.dll...So no need to check for disallowed loaded modules
	//

	// N/A

	//
	// Remove detours from system and D3D functions
	//

	WriteToLog( _T("Detaching detours from system and D3D functions...") );
	if( !DetachDetoursFromSystemAndD3DFunctions() )
	{
		WriteToLog( _T("DetachDetoursFromSystemAndD3DFunctions failed.") );
		goto Done;
	}

	//
	// Check that system and D3D functions are NOT detoured
	//

	BeginSection( L"Check that system and D3D functions are NOT detoured" );
	if( !CheckForDetouredSystemFunctions() )
	{
		LogError( __FILEW__, __LINE__, L"System functions were detoured when they shouldn't be.", true, ERRORCODE_TYPE_BOOL, gFCLogicError );
		EndSection();
		goto Done;
	}

	if( !CheckForDetouredD3DFunctions() )
	{
		LogError( __FILEW__, __LINE__, L"D3D functions were detoured when they shouldn't be.", true, ERRORCODE_TYPE_BOOL, gFCLogicError );
		EndSection();
		goto Done;
	}

	// Call the detoured function
	// This should really call TestFunctionToDetour, which should return false
	if( TestFunctionToDetour() ||
		IsFunctionDetoured( CDetoursTest::TestFunctionToDetour, _T("CDetoursTest::TestFunctionToDetour") ) )	
	{
		LogError( __FILEW__, __LINE__, L"CDetoursTest::TestFunctionToDetour was detoured when it should NOT be.", true, ERRORCODE_TYPE_BOOL, gFCLogicError );
		EndSection(); 
		goto Done;
	}
	EndSection();

	// If we got here, we passed
	bTestCatchesDetours = true;

Done:
	// Ensure we don't have any dangling detours
	if( m_bDetoursAttached )
	{
		if( !DetachDetoursFromSystemAndD3DFunctions() )
		{
			WriteToLog( _T("DetachDetoursFromSystemAndD3DFunctions failed.") );
		}
	}
	
	return bTestCatchesDetours;
}

/////

bool CDetoursTest::AttachDetoursToSystemAndD3DFunctions()
{
	// Local variables
	bool bSuccess = false;
	LONG retVal = 0;

	//
	// Detour system and D3D functions
	//

	if( !m_bDetoursAttached )
	{
		retVal = DetourTransactionBegin();
		if( retVal != ERROR_SUCCESS )
		{
			LogError( __FILEW__, __LINE__, L"DetourTransactionBegin failed.", retVal, ERRORCODE_TYPE_WIN32, gFCDetours );
			goto Done;
		}

		retVal = DetourUpdateThread(GetCurrentThread());
		if( retVal != ERROR_SUCCESS )
		{
			LogError( __FILEW__, __LINE__, L"DetourUpdateThread failed.", retVal, ERRORCODE_TYPE_WIN32, gFCDetours );
			goto Done;
		}

		// Attach any detours here...
		// For each function we need to detach...

		g_pfnTestFunctionToDetour = CDetoursTest::TestFunctionToDetour;
		retVal = DetourAttach(&(PVOID&)g_pfnTestFunctionToDetour, CDetoursTest::TestFunctionToDetourTo);
		if( retVal != ERROR_SUCCESS )
		{
			WriteToLog( _T("DetourAttach failed while trying to attach %s."), _T("CDetoursTest::TestFunctionToDetour") );
			LogError( __FILEW__, __LINE__, L"DetourAttach failed.", retVal, ERRORCODE_TYPE_WIN32, gFCDetours );
			goto Done;
		}

		for( UINT i = 0; i < g_numFunctionsToDetour; i++ )
		{
			HMODULE currModuleHandle = GetModuleHandle( g_functionsToDetour[i].moduleName.c_str() );
			if( !currModuleHandle )
			{
				WriteToLog( _T("Module not loaded: %s. Unable to detour."), g_functionsToDetour[i].moduleName.c_str() );
				continue;	// Attempt next detour...
			}

			*(g_functionsToDetour[i].pfn) = GetProcAddress( currModuleHandle, g_functionsToDetour[i].functionName.c_str() );
			retVal = DetourAttach(&(PVOID&)(*g_functionsToDetour[i].pfn), g_functionsToDetour[i].pfnDetour);
			if( retVal != ERROR_SUCCESS )
			{
				WriteToLog( _T("DetourAttach failed while trying to attach %s (module: %s, address: %#X, detour address: %#X)."), 
					g_functionsToDetour[i].functionName.c_str(), g_functionsToDetour[i].moduleName.c_str(), 
					*(g_functionsToDetour[i].pfn), g_functionsToDetour[i].pfnDetour );
				LogError( __FILEW__, __LINE__, L"DetourAttach failed.", retVal, ERRORCODE_TYPE_WIN32, gFCDetours );
				goto Done;
			}
		}

		retVal = DetourTransactionCommit();
		if( retVal != ERROR_SUCCESS )
		{
			LogError( __FILEW__, __LINE__, L"DetourTransactionCommit failed.", retVal, ERRORCODE_TYPE_WIN32, gFCDetours );
			goto Done;
		}

		m_bDetoursAttached = true;
	}

	// If we got here, we passed
	bSuccess = true;

Done:
	return bSuccess;
}

/////

bool CDetoursTest::DetachDetoursFromSystemAndD3DFunctions()
{
	// Local variables
	bool bSuccess = false;
	LONG retVal = 0;

	//
	// Detach detours from system and D3D functions
	//

	if( m_bDetoursAttached )
	{
		retVal = DetourTransactionBegin(); 
		if( retVal != ERROR_SUCCESS )
		{ 
			LogError( __FILEW__, __LINE__, L"DetourTransactionBegin failed.", retVal, ERRORCODE_TYPE_WIN32, gFCDetours ); 
			goto Done;
		}

		retVal = DetourUpdateThread(GetCurrentThread()); 
		if( retVal != ERROR_SUCCESS )
		{ 
			LogError( __FILEW__, __LINE__, L"DetourTransactionBegin failed.", retVal, ERRORCODE_TYPE_WIN32, gFCDetours ); 
			goto Done; 
		}

		// Detach any detours here...
		retVal = DetourDetach(&(PVOID&)g_pfnTestFunctionToDetour, CDetoursTest::TestFunctionToDetourTo); 
		if( retVal != ERROR_SUCCESS )
		{ 
			WriteToLog( _T("DetourDetach failed while trying to detach %s."), _T("CDetoursTest::TestFunctionToDetour") );
			LogError( __FILEW__, __LINE__, L"DetourTransactionBegin failed.", retVal, ERRORCODE_TYPE_WIN32, gFCDetours ); 
			goto Done;
		}

		// For each function we need to detach...
		for( UINT i = 0; i < g_numFunctionsToDetour; i++ )
		{
			if( g_functionsToDetour[i].pfn == nullptr )
			{
				continue;
			}

			retVal = DetourDetach(&(PVOID&)(*g_functionsToDetour[i].pfn), (g_functionsToDetour[i].pfnDetour));
			if( retVal != ERROR_SUCCESS )
			{
				WriteToLog( _T("DetourDetach failed while trying to detach %s."), g_functionsToDetour[i].functionName.c_str() );
				LogError( __FILEW__, __LINE__, L"DetourDetach failed.", retVal, ERRORCODE_TYPE_WIN32, gFCDetours );
				goto Done;
			}
		}

		retVal = DetourTransactionCommit();
		if( retVal != ERROR_SUCCESS )
		{
			LogError( __FILEW__, __LINE__, L"DetourTransactionCommit failed.", retVal, ERRORCODE_TYPE_WIN32, gFCDetours );
			goto Done;
		}

		m_bDetoursAttached = false;
	}

	// If we got here, we passed
	bSuccess = true;

Done:
	return bSuccess;
}

/////

bool CDetoursTest::TestFunctionToDetour()
{
	// This function does nothing. It's just used to test our test.

	// Local variables
	bool bIMakeThisFunctionBigEnoughToDetour = true;
	int i = 0;

	if( bIMakeThisFunctionBigEnoughToDetour )
	{
		i++;
	}

	return false;	// Always fail
}

/////

bool CDetoursTest::TestFunctionToDetourTo()
{
	// This function does nothing. It's just used to test our test.

	// Local variables
	bool bIMakeThisFunctionBigEnoughToDetour = true;
	int i = 2;

	if( bIMakeThisFunctionBigEnoughToDetour )
	{
		i++;
	}

	return true;	// Always pass
}

#endif