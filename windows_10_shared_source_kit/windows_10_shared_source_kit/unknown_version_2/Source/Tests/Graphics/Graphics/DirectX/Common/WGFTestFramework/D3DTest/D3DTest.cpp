//////////////////////////////////////////////////////////////////////////
//  D3DTest.cpp
//  created:	2004/10/13
//
//  purpose: Implementation of CD3DTest class
//////////////////////////////////////////////////////////////////////////

//
// NT headers
//

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#define WIN32_NO_STATUS //make sure there are no conflicts between ntstatus.h and windows.h

//
// Precompiled header
//

#include "pch.h"


//
// Win32 headers
//

#include <detours.h>
#include <d3d11_1.h>
#include <d3d11.h>
#include <d3d10_1.h>
#include <d3d10.h>

#include <d3d9.h> // needed for d3d10level9 driver version access
#include <MMSystem.h> // needed for timeGetTime()

#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>


//
// Project headers
//

#include "d3dtest.h"
#include "Shell.h"
#include <d3dcompiler.h>
#include "d3dx10.h"
#include "D3D10Wrapped.h"
#include "D3DX10Wrapped.h"
#include "D3DMultiWrapped.h"
#include "D3D10Test.h"
#include "D3D10Convert.h"
#include "D3D11Wrapped.h"
#include "D3D11Test.h"
#include "D3D11Convert.h"
#include "FormatHelper.h"
#include "AdapterUtil.h"


//
// STL headers
//

#include <list>
using namespace std;

// Wraps call to appropriate D3D<n>CreateDevice method depending on the value of m_D3DOptions.origInterfaceLevel
static HRESULT LocalD3D10CreateDevice(
	IDXGIAdapter *pAdapter,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	UINT SDKVersion,
	ID3D10Device **ppDevice);

static HRESULT LocalD3D10CreateDevice1(
	IDXGIAdapter *pAdapter,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	D3D10_FEATURE_LEVEL1 HardwareLevel,
	UINT SDKVersion,
	ID3D10Device1 **ppDevice);



#pragma warning(push)
#pragma warning(disable:4068)
#pragma prefast(disable: 321, "We want relative path when loading DLLs for testing purposes.") 
#pragma warning(pop)

static DXGI_OPTIONS					s_DXGIOptionsCached;
static D3D_OPTIONS					s_D3DOptionsCached;
static D3D_RENDER_TARGET_OPTIONS	s_RenderTargetOptionsCached;


#define	D3D10DLLNAME "D3D10.DLL"
#define	D3D10_PROXYDRIVER_DLLNAME "D3D10ProxyDriver.DLL"
#define	D3D11_PROXYDRIVER_DLLNAME "D3D11ProxyDriver.DLL"
#define	D3D11DLLNAME "D3D11.DLL"

list<void*>                         g_lstVAddresses;     // List of virtual memory addresses for Large Address Aware testing.
list<void*>                         g_lstHAddresses;     // List of heap memory addresses.

typedef HRESULT (WGFTESTAPI D3D10CreateDeviceProc)( 
    IDXGIAdapter *pAdapter,
    D3D10_DRIVER_TYPE DriverType,
    HANDLE hSoftware,
    UINT Flags,
    UINT32 SDKVersion,
	ID3D10Device **ppDevice );

typedef HRESULT (WGFTESTAPI D3D10CreateDevice1Proc)( 
    IDXGIAdapter *pAdapter,
    D3D10_DRIVER_TYPE DriverType,
    HANDLE hSoftware,
    UINT Flags,
	D3D10_FEATURE_LEVEL1 HardwareLevel,
    UINT32 SDKVersion,
	ID3D10Device1 **ppDevice );

typedef HRESULT (WGFTESTAPI D3D11CreateDeviceProc)( 
    IDXGIAdapter *pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE hSoftware,
    UINT Flags,
	CONST D3D_FEATURE_LEVEL* pFeatureLevels,
	UINT FeatureLevels,
    UINT SDKVersion,
	ID3D11Device **ppDevice,
	D3D_FEATURE_LEVEL* pFeatureLevel,
	ID3D11DeviceContext** ppImmediateContext);

HRESULT IndirectD3D10CreateDevice( 
    IDXGIAdapter *pAdapter,
    D3D10_DRIVER_TYPE DriverType,
    HANDLE hSoftware,
    UINT Flags,
    UINT32 SDKVersion,
	ID3D10Device **ppDevice )
{
	HRESULT hr = S_OK;

	HMODULE hD3D = LoadLibraryExW( L"D3D10.DLL", nullptr, 0 );
	if( nullptr == hD3D )
	{
		WriteError( _T( "D3D10.DLL not found" ) );
		return E_FAIL;
	}

	D3D10CreateDeviceProc *pD3D10CreateDevice = (D3D10CreateDeviceProc *) GetProcAddress
	(
		hD3D,
		"D3D10CreateDevice"
	);

	return pD3D10CreateDevice(
		pAdapter,
		DriverType,
		hSoftware,
		Flags,
		SDKVersion,
		ppDevice );
}

HRESULT IndirectD3D10CreateDevice1( 
    IDXGIAdapter *pAdapter,
    D3D10_DRIVER_TYPE DriverType,
    HANDLE hSoftware,
    UINT Flags,
	D3D10_FEATURE_LEVEL1 HardwareLevel,
    UINT32 SDKVersion,
	ID3D10Device1 **ppDevice )
{
	HRESULT hr = S_OK;

	HMODULE hD3D = LoadLibraryExW( L"D3D10_1.DLL", nullptr, 0 );
	if( nullptr == hD3D )
	{
		WriteError( _T( "D3D10_1.DLL not found" ) );
		return E_FAIL;
	}

	D3D10CreateDevice1Proc *pD3D10CreateDevice1 = (D3D10CreateDevice1Proc *) GetProcAddress
	(
		hD3D,
		"D3D10CreateDevice1"
	);

	return pD3D10CreateDevice1( 
		pAdapter,
		DriverType,
		hSoftware,
		Flags,
		HardwareLevel,
		SDKVersion,
		ppDevice );
}

HRESULT IndirectD3D11CreateDevice( 
    IDXGIAdapter *pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE hSoftware,
    UINT Flags,
	CONST D3D_FEATURE_LEVEL* pFeatureLevels,
	UINT FeatureLevels,
    UINT SDKVersion,
	ID3D11Device **ppDevice,
	D3D_FEATURE_LEVEL* pFeatureLevel,
	ID3D11DeviceContext** ppImmediateContext)
{
	HRESULT hr = S_OK;

	HMODULE hD3D = LoadLibraryExW( L"D3D11.DLL", nullptr, 0 );
	if( nullptr == hD3D )
	{
		WriteError( _T( "D3D11.DLL not found" ) );
		return E_FAIL;
	}

	D3D11CreateDeviceProc *pD3D11CreateDevice = (D3D11CreateDeviceProc *) GetProcAddress
	(
		hD3D,
		"D3D11CreateDevice"
	);

	return pD3D11CreateDevice(
		pAdapter,
		DriverType,
		hSoftware,
		Flags,
		pFeatureLevels,
		FeatureLevels,
		SDKVersion,
		ppDevice,
		pFeatureLevel,
		ppImmediateContext );
}

float D3DVersionToFloat( WORD wVersion )
{
	switch( wVersion )
	{
		case 0x1000:
			return 10.0f;
			
		case 0x1010:
			return 10.1f;
			
		case 0x1100:
			return 11.0f;

		case 0xffff:
			return 10.1f;
	}
	
	return 0;
};


static UINT ConvertShaderFlagsTo10( UINT shaderFlags )
{
	UINT out = 0;

	if( shaderFlags & D3D_SHADER_DEBUG )
		out |= D3D10_SHADER_DEBUG;
	if( shaderFlags & D3D_SHADER_SKIP_VALIDATION )
		out |= D3D10_SHADER_SKIP_VALIDATION;
	if( shaderFlags & D3D_SHADER_SKIP_OPTIMIZATION )
		out |= D3D10_SHADER_SKIP_OPTIMIZATION;
	if( shaderFlags & D3D_SHADER_PACK_MATRIX_ROW_MAJOR )
		out |= D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
	if( shaderFlags & D3D_SHADER_PACK_MATRIX_COLUMN_MAJOR )
		out |= D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
	if( shaderFlags & D3D_SHADER_PARTIAL_PRECISION )
		out |= D3D10_SHADER_PARTIAL_PRECISION;
	if( shaderFlags & D3D_SHADER_FORCE_VS_SOFTWARE_NO_OPT )
		out |= D3D10_SHADER_FORCE_VS_SOFTWARE_NO_OPT;
	if( shaderFlags & D3D_SHADER_FORCE_PS_SOFTWARE_NO_OPT )
		out |= D3D10_SHADER_FORCE_PS_SOFTWARE_NO_OPT;
	if( shaderFlags & D3D_SHADER_NO_PRESHADER )
		out |= D3D10_SHADER_NO_PRESHADER;
	if( shaderFlags & D3D_SHADER_AVOID_FLOW_CONTROL )
		out |= D3D10_SHADER_AVOID_FLOW_CONTROL;
	if( shaderFlags & D3D_SHADER_PREFER_FLOW_CONTROL )
		out |= D3D10_SHADER_PREFER_FLOW_CONTROL;
	if( shaderFlags & D3D_SHADER_ENABLE_STRICTNESS )
		out |= D3D10_SHADER_ENABLE_STRICTNESS;
	if( shaderFlags & D3D_SHADER_ENABLE_BACKWARDS_COMPATIBILITY )
		out |= D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
		
	return out;
}

static
void
PrintDriverVersion(TCHAR buffer[32], LARGE_INTEGER driverVersion)
{
	sprintf_s( buffer, ( 32 * sizeof( TCHAR ) ) - 1, _T( "%d.%d.%d.%d" ),
		HIWORD( driverVersion.HighPart ), // product
		LOWORD( driverVersion.HighPart ), // version
		HIWORD( driverVersion.LowPart ),  // sub-version
		LOWORD( driverVersion.LowPart ) ); // build

}

static
const char *
GetShaderProfile(const char * pShaderProfile, const D3D_OPTIONS &D3DOptions)
{
	LPCSTR szNewProfile = pShaderProfile;

	const char* szVS40_9_1 = "vs_4_0_level_9_1";
	const char* szPS40_9_1 = "ps_4_0_level_9_1";

	const char* szVS40_9_3 = "vs_4_0_level_9_3";
	const char* szPS40_9_3 = "ps_4_0_level_9_3";

	const char* szVS41 = "vs_4_1";
	const char* szGS41 = "gs_4_1";
	const char* szPS41 = "ps_4_1";

	if (D3DOptions.wVersion >= 0x1010)
	{
		if (0 == strcmp(pShaderProfile, "vs_4_0"))
		{
			if (D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_9_1 || D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_9_2)
				szNewProfile = szVS40_9_1;
			else if (D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_9_3)
				szNewProfile = szVS40_9_3;
			else if (D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_1)
				szNewProfile = szVS41;
		}
		if (0 == strcmp(pShaderProfile, "gs_4_0") && D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_1)
			szNewProfile = szGS41;
		if (0 == strcmp(pShaderProfile, "ps_4_0"))
		{
			if (D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_9_1 || D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_9_2)
				szNewProfile = szPS40_9_1;
			else if (D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_9_3)
				szNewProfile = szPS40_9_3;
			else if (D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_1)
				szNewProfile = szPS41;
		}
	}

	return szNewProfile;
}

static bool operator==(const LUID& lhs, const LUID& rhs)
{
	return (lhs.LowPart == rhs.LowPart && lhs.HighPart == rhs.HighPart);
}

//--------------------------------------------------------------------

HRESULT CD3DTestFramework::CreateDevice( IDXGIAdapter **ppAdapter, D3D_DRIVER_TYPE DriverType, UINT uFlags, WORD wVersion, /*const DEVICEMODE *pDevMode,*/ CDevice **ppDevice )
{
	if ( wVersion == 0x1000 )
	{
		return CreateDevice10( ppAdapter, DriverType, uFlags, ppDevice );
	}
	else if ( wVersion == 0x1010 )
	{
		return CreateDevice10_1( ppAdapter, DriverType, uFlags, ppDevice );
	}
	else if ( wVersion == 0x1100 )
	{
		return CreateDevice11( ppAdapter, DriverType, uFlags, ppDevice );
	}
	else
	{
		WriteError( _T( "Unknown Direct3D interface version number" ) );
		return E_FAIL;
	}
}


HRESULT CD3DTestFramework::CreateDevice10( IDXGIAdapter **ppAdapter, D3D_DRIVER_TYPE DriverType, UINT uFlags,  CDevice **ppDevice )
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;
	HRESULT hCreateDeviceResult = E_FAIL;

	CD3DTestFramework * pFramework = dynamic_cast< CD3DTestFramework *>( GetFramework() );
	assert( pFramework != nullptr );

	ID3D10Device * pDevice10 = nullptr;
	UINT Flags10 = 0;

	bool bAdapterIsRemote = false;

	IDXGIAdapter * pAdapter = ppAdapter ? *ppAdapter : nullptr;
	IDXGIAdapter1 * pAdapter1 = nullptr;


	//
	// Translate flags.
	//

	if( uFlags & D3D_CREATE_DEVICE_DEBUG )
	{
		Flags10 |= D3D10_CREATE_DEVICE_DEBUG;
	}

	if( uFlags & D3D_CREATE_DEVICE_SINGLETHREADED )
	{
		Flags10 |= D3D10_CREATE_DEVICE_SINGLETHREADED;
	}

	if( uFlags & D3D_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS )
	{
		Flags10 |= D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;
	}

	if( uFlags & D3D_CREATE_DEVICE_SWITCH_TO_REF )
	{
		Flags10 |= D3D10_CREATE_DEVICE_SWITCH_TO_REF;
	}


	//
	// Determine the device type.
	//

	D3D10_DRIVER_TYPE DriverType10;
	switch( DriverType )
	{
		case D3D_DRIVER_TYPE_HARDWARE:
			DriverType10 = D3D10_DRIVER_TYPE_HARDWARE;
		break;

		case D3D_DRIVER_TYPE_SOFTWARE:
			DriverType10 = D3D10_DRIVER_TYPE_SOFTWARE;
		break;

		case D3D_DRIVER_TYPE_REFERENCE:
			DriverType10 = D3D10_DRIVER_TYPE_REFERENCE;
		break;

		case D3D_DRIVER_TYPE_WARP:
			DriverType10 = D3D10_DRIVER_TYPE_WARP;
		break;

		case D3D_DRIVER_TYPE_NULL:
			DriverType10 = D3D10_DRIVER_TYPE_NULL;
		break;		
	};


	//
	// Get information about the adapter.
	//

	if ( pAdapter != nullptr )
	{
		HRESULT hQueryForIDXGIAdapter1Result = pAdapter->QueryInterface
		(
			IID_IDXGIAdapter1,
			(void **) &pAdapter1
		);

		if
		(
			SUCCEEDED( hQueryForIDXGIAdapter1Result ) &&
			pAdapter1 != nullptr
		)
		{
			pFramework->WriteToLog
			(
				"Adapter supports IDXGIAdapter1 interface."
			);

			DXGI_ADAPTER_DESC1 AdapterDescriptor;
			ZeroMemory( &AdapterDescriptor, sizeof( AdapterDescriptor ) );

			HRESULT hGetDescriptorResult = pAdapter1->GetDesc1( &AdapterDescriptor );
			if ( FAILED( hGetDescriptorResult ) )
			{
				hOverallResult = hGetDescriptorResult;
				goto Cleanup;
			};

			if ( ( AdapterDescriptor.Flags & DXGI_ADAPTER_FLAG_REMOTE ) != 0 )
			{
				bAdapterIsRemote = true;
			};
		};
	};

	if ( bAdapterIsRemote == true )
	{
		pFramework->WriteToLog
		(
			_T( "Adapter was enumerated from remote client machine." )
		);
	}
	else
	{
		pFramework->WriteToLog
		(
			_T( "Adapter was enumerated from local machine." )
		);
	};

	pAdapter = (DriverType == D3D_DRIVER_TYPE_HARDWARE) ? pAdapter : nullptr;


	//
	// Load the rasterizer, and create the device.
	//

	if (pFramework->m_D3DOptions.proxyDriver)
	{
		//pass in D3D10ProxyDriver.dll
		HMODULE hD3D10ProxyDriver = LoadLibraryExW( UNICODIFY(D3D10_PROXYDRIVER_DLLNAME), nullptr, 0 );
		if( nullptr == hD3D10ProxyDriver )
		{
			WriteError( _T( "Proxy driver DLL %s not found." ), _T( D3D10_PROXYDRIVER_DLLNAME ) );

			hOverallResult = E_FAIL;
			goto Cleanup;
		}

		//DriverType needs to be software to load the module
		DriverType10 = D3D10_DRIVER_TYPE_SOFTWARE;
		hCreateDeviceResult = LocalD3D10CreateDevice
		(
			pAdapter,
			DriverType10,
			hD3D10ProxyDriver,
			Flags10,
			D3D10_SDK_VERSION,
			&pDevice10
		);
	}
	//Non-proxy driver software flag set without a dll being specified?
	else if
	(
		pFramework->m_D3DOptions.DriverType == D3D_DRIVER_TYPE_SOFTWARE &&
		pFramework->m_D3DOptions.SoftwareRasterizerDLLName[ 0 ] == L'\0'
	)
	{
		WriteError( _T( "The -SWDLL switch requires a corresponding DLL name, e.g., '-SWDLL:D3D10Ref.dll'." ) );

		hOverallResult = E_FAIL;
		goto Cleanup;
	}
	//Non-proxy driver software rasterizer (i.e. d3d10warp.dll)?
	else if
	(
		pFramework->m_D3DOptions.DriverType == D3D_DRIVER_TYPE_SOFTWARE &&
		pFramework->m_D3DOptions.SoftwareRasterizerDLLName[ 0 ] != L'\0'
	)
	{
		DriverType10 = D3D10_DRIVER_TYPE_SOFTWARE;
		HMODULE hD3D10SoftwareDriver = pFramework->LoadSoftwareRasterizer();
		if(nullptr == hD3D10SoftwareDriver)
		{
			WriteErrorW
			(
				L"Failed to load software rasterizer DLL %s.",
				pFramework->m_D3DOptions.SoftwareRasterizerDLLName
			);

			hOverallResult = E_FAIL;
			goto Cleanup;
		}

		hCreateDeviceResult = LocalD3D10CreateDevice
		(
			pAdapter,
			DriverType10,
			hD3D10SoftwareDriver,
			Flags10,
			D3D10_SDK_VERSION,
			&pDevice10
		);
	}
	else
	{
		hCreateDeviceResult = LocalD3D10CreateDevice
		(
			pAdapter,
			DriverType10,
			nullptr,
			Flags10,
			D3D10_SDK_VERSION,
			&pDevice10
		);
	}
	
	if ( FAILED( hCreateDeviceResult ) )
	{
		hOverallResult = hCreateDeviceResult;

		// BUGBUG - H-A-C-K FIX to enable skipping test cases which cycle on debug layer when sdk not installed.
		if ( ( Flags10 & D3D10_CREATE_DEVICE_DEBUG ) != 0 )
		{
			WindowsTest::CHandle<ID3D10Device1> pDevice;
			D3D_FEATURE_LEVEL fl;
			HRESULT hr = IndirectD3D10CreateDevice1(
				nullptr, 
				D3D10_DRIVER_TYPE_NULL, 
				nullptr, 
				D3D10_CREATE_DEVICE_DEBUG, 
				D3D10_FEATURE_LEVEL_10_0,
				D3D10_1_SDK_VERSION,
				&pDevice
				);


			if(FAILED(hr))
			{
				GetFramework()->WriteToLog( "CreateDevice:  SDKLayers cannot be found.  Unable to create debug layer." );

				goto Cleanup;
			}
		}

		hOverallResult = hCreateDeviceResult;
		goto Cleanup;
	}
	else
	{
		*ppDevice = new C10Device( pDevice10 );
		(*ppDevice)->AddRef();

		hOverallResult = S_OK;
	};


Cleanup:
	if ( pAdapter1 != nullptr )
	{
		pAdapter1->Release();
		pAdapter1 = nullptr;
	};


	return hOverallResult;
}


HRESULT CD3DTestFramework::CreateDevice10_1( IDXGIAdapter **ppAdapter, D3D_DRIVER_TYPE DriverType, UINT uFlags,  CDevice **ppDevice )
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;
	HRESULT hCreateDeviceResult = E_FAIL;

	CD3DTestFramework * pFramework = dynamic_cast< CD3DTestFramework *>( GetFramework() );
	assert( pFramework != nullptr );

	ID3D10Device1 * pDevice10_1 = nullptr;
	UINT Flags10 = 0;

	bool bAdapterSupportsIDXGIAdapter1 = false;
	bool bAdapterIsRemote = false;

	IDXGIAdapter * pAdapter = ppAdapter ? *ppAdapter : nullptr;
	IDXGIAdapter1 * pAdapter1 = nullptr;

	D3D10_FEATURE_LEVEL1 FeatureLevel10;

	//
	// Translate flags.
	//

	if( uFlags & D3D_CREATE_DEVICE_DEBUG )
	{
		Flags10 |= D3D10_CREATE_DEVICE_DEBUG;
	}

	if( uFlags & D3D_CREATE_DEVICE_SINGLETHREADED )
	{
		Flags10 |= D3D10_CREATE_DEVICE_SINGLETHREADED;
	}

	if( uFlags & D3D_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS )
	{
		Flags10 |= D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;
	}

	if( uFlags & D3D_CREATE_DEVICE_SWITCH_TO_REF )
	{
		Flags10 |= D3D10_CREATE_DEVICE_SWITCH_TO_REF;
	}


	//
	// Determine the device type.
	//

	D3D10_DRIVER_TYPE DriverType10;
	switch( DriverType )
	{
		case D3D_DRIVER_TYPE_HARDWARE:
			DriverType10 = D3D10_DRIVER_TYPE_HARDWARE;
		break;

		case D3D_DRIVER_TYPE_SOFTWARE:
			DriverType10 = D3D10_DRIVER_TYPE_SOFTWARE;
		break;

		case D3D_DRIVER_TYPE_REFERENCE:
			DriverType10 = D3D10_DRIVER_TYPE_REFERENCE;
		break;

		case D3D_DRIVER_TYPE_WARP:
			DriverType10 = D3D10_DRIVER_TYPE_WARP;
		break;

		case D3D_DRIVER_TYPE_NULL:
			DriverType10 = D3D10_DRIVER_TYPE_NULL;
		break;		
	};


	//
	// Get information about the adapter.
	//

	if ( pAdapter != nullptr )
	{
		HRESULT hQueryForIDXGIAdapter1Result = pAdapter->QueryInterface
		(
			IID_IDXGIAdapter1,
			(void **) &pAdapter1
		);

		if
		(
			SUCCEEDED( hQueryForIDXGIAdapter1Result ) &&
			pAdapter1 != nullptr
		)
		{
			pFramework->WriteToLog
			(
				"Adapter supports IDXGIAdapter1 interface."
			);

			bAdapterSupportsIDXGIAdapter1 = true;

			DXGI_ADAPTER_DESC1 AdapterDescriptor;
			ZeroMemory( &AdapterDescriptor, sizeof( AdapterDescriptor ) );

			HRESULT hGetDescriptorResult = pAdapter1->GetDesc1( &AdapterDescriptor );
			if ( FAILED( hGetDescriptorResult ) )
			{
				hOverallResult = hGetDescriptorResult;
				goto Cleanup;
			};

			if ( ( AdapterDescriptor.Flags & DXGI_ADAPTER_FLAG_REMOTE ) != 0 )
			{
				bAdapterIsRemote = true;
			};
		};
	};

	if ( bAdapterIsRemote == true )
	{
		pFramework->WriteToLog
		(
			_T( "Adapter was enumerated from remote client machine." )
		);
	}
	else
	{
		pFramework->WriteToLog
		(
			_T( "Adapter was enumerated from local machine." )
		);
	};

	pAdapter = (DriverType == D3D_DRIVER_TYPE_HARDWARE) ? pAdapter : nullptr;


	//
	// Determine the feature level.
	//

	D3DWrapperConvertEnum(&FeatureLevel10, pFramework->m_D3DOptions.FeatureLevel);


	// @N.B. [SEdmison]:
	// Pass the magic REMOTE_FORCE_REF flag if we're running in
	// a remote session and the -TS command-line switch is set.
	if
	(
		bAdapterSupportsIDXGIAdapter1 &&
		DriverType == D3D_DRIVER_TYPE_REFERENCE &&
		pFramework->IsTerminalServices() &&
		GetSystemMetrics( SM_REMOTESESSION ) != 0
	)
	{
		pAdapter = ppAdapter ? *ppAdapter : nullptr;
		DriverType10 = D3D10_DRIVER_TYPE_HARDWARE;
		Flags10 |= D3D10_CREATE_DEVICE_REMOTE_FORCE_REF;
	};


	//
	// Load the rasterizer, and create the device.
	//

	if (pFramework->m_D3DOptions.proxyDriver)
	{
		HMODULE hD3D10ProxyDriver = LoadLibraryExW( UNICODIFY(D3D10_PROXYDRIVER_DLLNAME), nullptr, 0 );
		if( nullptr == hD3D10ProxyDriver )
		{
			WriteError( _T( "Proxy driver DLL %s not found." ), _T( D3D10_PROXYDRIVER_DLLNAME ) );

			hOverallResult = E_FAIL;
			goto Cleanup;
		}

		//DriverType needs to be software to load the module
		DriverType10 = D3D10_DRIVER_TYPE_SOFTWARE;
		pAdapter = nullptr;
		hCreateDeviceResult = LocalD3D10CreateDevice1
		(
			pAdapter,
			DriverType10,
			hD3D10ProxyDriver,
			Flags10,
			FeatureLevel10,
			D3D10_1_SDK_VERSION,
			&pDevice10_1
		);
	}
	//Non-proxy driver software flag set without a dll being specified?
	else if
	(
		pFramework->m_D3DOptions.DriverType == D3D_DRIVER_TYPE_SOFTWARE &&
		pFramework->m_D3DOptions.SoftwareRasterizerDLLName[ 0 ] == L'\0'
	)
	{
		WriteError( _T( "The -SWDLL switch requires a corresponding DLL name, e.g., '-SWDLL:D3D10Ref.dll'." ) );

		hOverallResult = E_FAIL;
		goto Cleanup;
	}
	//Non-proxy driver software rasterizer (i.e. d3d10warp.dll)?
	else if
	(
		pFramework->m_D3DOptions.DriverType == D3D_DRIVER_TYPE_SOFTWARE &&
		pFramework->m_D3DOptions.SoftwareRasterizerDLLName[ 0 ] != L'\0'
	)
	{
		DriverType10 = D3D10_DRIVER_TYPE_SOFTWARE;
		HMODULE hD3D10SoftwareDriver = pFramework->LoadSoftwareRasterizer();
		if (nullptr == hD3D10SoftwareDriver)
		{
			WriteErrorW
			(
				L"Failed to load software rasterizer DLL %s.",
				pFramework->m_D3DOptions.SoftwareRasterizerDLLName
			);

			hOverallResult = E_FAIL;
			goto Cleanup;
		}

		hCreateDeviceResult = LocalD3D10CreateDevice1
		(
			pAdapter,
			DriverType10,
			hD3D10SoftwareDriver,
			Flags10,
			FeatureLevel10,
			D3D10_1_SDK_VERSION,
			&pDevice10_1
		);		
	}
	else
	{
		hCreateDeviceResult = LocalD3D10CreateDevice1
		(
			pAdapter,
			DriverType10,
			nullptr,
			Flags10,
			FeatureLevel10,
			D3D10_1_SDK_VERSION,
			&pDevice10_1
		);				
	}
	
	if ( FAILED( hCreateDeviceResult ) )
	{
		hOverallResult = hCreateDeviceResult;

		// BUGBUG - H-A-C-K FIX to enable skipping test cases which cycle on debug layer when sdk not installed.
		if ( ( Flags10 & D3D10_CREATE_DEVICE_DEBUG ) != 0 )
		{
			WindowsTest::CHandle<ID3D10Device1> pDevice;
			D3D_FEATURE_LEVEL fl;
			HRESULT hr = IndirectD3D10CreateDevice1(
				nullptr, 
				D3D10_DRIVER_TYPE_NULL, 
				nullptr, 
				D3D10_CREATE_DEVICE_DEBUG, 
				D3D10_FEATURE_LEVEL_10_0,
				D3D10_1_SDK_VERSION,
				&pDevice
				);

			if(FAILED(hr))
			{
				GetFramework()->WriteToLog( "CreateDevice:  SDKLayers cannot be found.  Unable to create debug layer." );

				goto Cleanup;
			}
		};


		if
		(
			( Flags10 & D3D10_CREATE_DEVICE_REMOTE_FORCE_REF ) != 0 &&
			hCreateDeviceResult == E_NOINTERFACE
		)
		{
			pFramework->WriteToLog
			(
				"D3D10_CREATE_DEVICE_REMOTE_FORCE_REF may not be supported on client machine."
			);
		};


		if (FeatureLevel10 <= D3D10_FEATURE_LEVEL_9_3)
		{
			HMODULE hmod = LoadLibraryExW( L"D3D10Level9.dll", nullptr, 0 );
			if( hmod == nullptr )
			{
				pFramework->WriteToLog( "'D3D10Level9.dll' not found.  Unable to create 10Level9 device." );
			}
			else
			{
				FreeLibrary( hmod );
				hmod = nullptr;

				if (hCreateDeviceResult == E_FAIL)
				{
					pFramework->WriteToLogF
					(
						"D3D10CreateDevice1() returned '%s'.  Unable to create device on this feature level.",
						D3DHResultToString( hCreateDeviceResult ).c_str()
					);

					hOverallResult = DXGI_ERROR_UNSUPPORTED;
					goto Cleanup;
				}
			}
		};

		
		hOverallResult = hCreateDeviceResult;
		goto Cleanup;
	}
	else
	{
		*ppDevice = reinterpret_cast<CDevice*> (new C10Device1( pDevice10_1 ));
		(*ppDevice)->AddRef();

		hOverallResult = S_OK;
	};


Cleanup:
	if ( pAdapter1 != nullptr )
	{
		pAdapter1->Release();
		pAdapter1 = nullptr;
	};


	return hOverallResult;
}


HRESULT CD3DTestFramework::CreateDevice11( IDXGIAdapter **ppAdapter, D3D_DRIVER_TYPE DriverType, UINT uFlags,  CDevice **ppDevice )
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;
	HRESULT hCreateDeviceResult = E_FAIL;

	CD3DTestFramework * pFramework = dynamic_cast< CD3DTestFramework *>( GetFramework() );
	assert( pFramework != nullptr );

	ID3D11Device *pDevice11 = nullptr;
	UINT Flags11 = 0;

	bool bAdapterIsRemote = false;

	IDXGIAdapter * pAdapter = ppAdapter ? *ppAdapter : nullptr;
	IDXGIAdapter1 * pAdapter1 = nullptr;

	D3D_FEATURE_LEVEL FeatureLevel11;

	//
	// Translate flags.
	//

	if( uFlags & D3D_CREATE_DEVICE_DEBUG )
	{
		Flags11 |= D3D11_CREATE_DEVICE_DEBUG;
	}

	if( uFlags & D3D_CREATE_DEVICE_SWITCH_TO_REF )
	{
		Flags11 |= D3D11_CREATE_DEVICE_SWITCH_TO_REF;
	}

    if( uFlags & D3D_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS )
	{
		Flags11 |= D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;
	}

	//
	// Determine the device type.
	//

	D3D_DRIVER_TYPE DriverType11 = DriverType;


	//
	// Get information about the adapter.
	//

	if ( pAdapter != nullptr )
	{
		HRESULT hQueryForIDXGIAdapter1Result = pAdapter->QueryInterface
		(
			IID_IDXGIAdapter1,
			(void **) &pAdapter1
		);

		if
		(
			SUCCEEDED( hQueryForIDXGIAdapter1Result ) &&
			pAdapter1 != nullptr
		)
		{
			pFramework->WriteToLog
			(
				"Adapter supports IDXGIAdapter1 interface."
			);

			DXGI_ADAPTER_DESC1 AdapterDescriptor;
			ZeroMemory( &AdapterDescriptor, sizeof( AdapterDescriptor ) );

			HRESULT hGetDescriptorResult = pAdapter1->GetDesc1( &AdapterDescriptor );
			if ( FAILED( hGetDescriptorResult ) )
			{
				hOverallResult = hGetDescriptorResult;
				goto Cleanup;
			};

			if ( ( AdapterDescriptor.Flags & DXGI_ADAPTER_FLAG_REMOTE ) != 0 )
			{
				bAdapterIsRemote = true;
			};
		};
	};

	if ( bAdapterIsRemote == true )
	{
		pFramework->WriteToLog
		(
			_T( "Adapter was enumerated from remote client machine." )
		);
	}
	else
	{
		pFramework->WriteToLog
		(
			_T( "Adapter was enumerated from local machine." )
		);
	};

	pAdapter = (DriverType == D3D_DRIVER_TYPE_HARDWARE) ? pAdapter : nullptr;

	if (pAdapter != nullptr)
	{
		DriverType11 = D3D_DRIVER_TYPE_UNKNOWN;
	}


	//
	// Determine the feature level.
	//

	D3DWrapperConvertEnum(&FeatureLevel11, pFramework->m_D3DOptions.FeatureLevel);

/*	Disabled for now because D3D11 doesn't have this flag
	// @N.B. [SEdmison]:
	// Pass the magic REMOTE_FORCE_REF flag if we're running in
	// a remote session and the -TS command-line switch is set.
	if
	(
		DriverType == D3D_DRIVER_TYPE_REFERENCE &&
		pFramework->IsTerminalServices() &&
		GetSystemMetrics( SM_REMOTESESSION ) != 0
	)
	{
		pAdapter = ppAdapter ? *ppAdapter : nullptr;
		DriverType11 = D3D_DRIVER_TYPE_HARDWARE;
		Flags11 |= D3D11_CREATE_DEVICE_REMOTE_FORCE_REF;
	};
*/


	//
	// Load the rasterizer, and create the device.
	//

	if (pFramework->m_D3DOptions.proxyDriver)
	{
	    HMODULE hD3D11ProxyDriver = LoadLibraryExW( UNICODIFY(D3D11_PROXYDRIVER_DLLNAME), nullptr, 0 );
		if( nullptr == hD3D11ProxyDriver )
		{
			WriteError( _T( "Proxy driver DLL %s not found." ), _T( D3D11_PROXYDRIVER_DLLNAME ) );

			hOverallResult = E_FAIL;
			goto Cleanup;
		}

		//DriverType needs to be software to load the module
		DriverType11 = D3D_DRIVER_TYPE_SOFTWARE;
		pAdapter = nullptr;
		hCreateDeviceResult = IndirectD3D11CreateDevice
		(
			pAdapter,
			DriverType11,
			hD3D11ProxyDriver,
			Flags11,
			&FeatureLevel11,
			1,
			D3D11_SDK_VERSION,
			&pDevice11,
			nullptr,
			nullptr
		);
	}
	//Non-proxy driver software flag set without a dll being specified?
	else if
	(
		pFramework->m_D3DOptions.DriverType == D3D_DRIVER_TYPE_SOFTWARE &&
		pFramework->m_D3DOptions.SoftwareRasterizerDLLName[ 0 ] == L'\0'
	)
	{
		WriteError( _T( "The -SWDLL switch requires a corresponding DLL nane, e.g., '-SWDLL:D3D11Ref.dll'." ) );

		hOverallResult = E_FAIL;
		goto Cleanup;
	}
	//Non-proxy driver software rasterizer (i.e. d3d10warp.dll)?
	else if
	(
		pFramework->m_D3DOptions.DriverType == D3D_DRIVER_TYPE_SOFTWARE &&
		pFramework->m_D3DOptions.SoftwareRasterizerDLLName[ 0 ] != L'\0'
	)
	{
		DriverType11 = D3D_DRIVER_TYPE_SOFTWARE;
		HMODULE hD3D11SoftwareDriver = pFramework->LoadSoftwareRasterizer();

		if(nullptr == hD3D11SoftwareDriver)
		{
			WriteErrorW
			(
				L"Failed to load software rasterizer DLL %s.",
				pFramework->m_D3DOptions.SoftwareRasterizerDLLName
			);

			hOverallResult = E_FAIL;
			goto Cleanup;
		}

		hCreateDeviceResult = IndirectD3D11CreateDevice
		(
			pAdapter,
			DriverType11,
			hD3D11SoftwareDriver,
			Flags11,
			&FeatureLevel11,
			1,
			D3D11_SDK_VERSION,
			&pDevice11,
			nullptr,
			nullptr
		);
	}
	else
	{
		hCreateDeviceResult = IndirectD3D11CreateDevice
		(
			pAdapter,
			DriverType11,
			nullptr,
			Flags11,
			&FeatureLevel11,
			1,
			D3D11_SDK_VERSION,
			&pDevice11,
			nullptr,
			nullptr
		);				
	}

	if ( FAILED( hCreateDeviceResult ) )
	{
		// BUGBUG - H-A-C-K FIX to enable skipping test cases which cycle on debug layer when sdk not installed.
		if( Flags11 & D3D11_CREATE_DEVICE_DEBUG )
		{
			WindowsTest::CHandle<ID3D10Device1> pDevice;
			D3D_FEATURE_LEVEL fl;
			HRESULT hr = IndirectD3D10CreateDevice1(
				nullptr, 
				D3D10_DRIVER_TYPE_NULL, 
				nullptr, 
				D3D10_CREATE_DEVICE_DEBUG, 
				D3D10_FEATURE_LEVEL_10_0,
				D3D10_1_SDK_VERSION,
				&pDevice
				);


			if(FAILED(hr))
			{
				GetFramework()->WriteToLog( "CreateDevice:  SDKLayers cannot be found.  Unable to create debug layer." );

				goto Cleanup;
			}
		}

		if (FeatureLevel11 <= D3D_FEATURE_LEVEL_9_3)
		{
			HMODULE hmod = LoadLibraryExW( L"D3D10Level9.dll", nullptr, 0 );
			if( hmod == nullptr )
			{
				pFramework->WriteToLog( "'D3D10Level9.dll' not found.  Unable to create 11Level9 device." );
			}
			else
			{
				FreeLibrary( hmod );
				hmod = nullptr;

				if (hCreateDeviceResult == E_FAIL)
				{
					pFramework->WriteToLogF
					(
						"D3D11CreateDevice() returned '%s'.  Unable to create device on this feature level.",
						D3DHResultToString( hCreateDeviceResult ).c_str()
					);

					hOverallResult = hCreateDeviceResult;
					goto Cleanup;
				}
			}
		}
		
		hOverallResult = hCreateDeviceResult;
		goto Cleanup;
	}
	else
	{
		*ppDevice = reinterpret_cast<CDevice*> (new C11Device( pDevice11 ));
		(*ppDevice)->AddRef();

		hOverallResult = S_OK;
	};


Cleanup:	
	if ( pAdapter1 != nullptr )
	{
		pAdapter1->Release();
		pAdapter1 = nullptr;
	};


	return hOverallResult;
}

HRESULT CD3DTestFramework::GetDataWithTimeout( CQuery *pQuery, void* pData, UINT dataSize, UINT dataFlags )
{
	/*
		Description: Gets the query data and times out if call exceeds timeout limit.

		Parameters:
			CQuery *pQuery - The query to use for the GetData call
			void* pData - Data returned by the query
			UINT dataSize - Size of the query data
			UINT dataFlags - Flags to get query data

		Returns:
			HRESULT. The function will return S_FALSE if the query timed out. Otherwise, it will return the HRESULT from the GetData() call.
	*/

	// Local variables
	HRESULT hr = S_FALSE;
	DWORD startTime = timeGetTime();
	DWORD currentTime;

	// Simple checks
	assert( nullptr != pQuery );
	assert( nullptr != pData );

	//
	// Loop until query completes or times out
	//
	for(;;)
	{
		hr = pQuery->GetData( pData, dataSize, dataFlags );

		if( S_FALSE != hr )
			break;
		
		currentTime = timeGetTime();
		if( (currentTime - startTime) > m_nQueryTimeout ) // Timeout in milliseconds
			break;

		Sleep(0);
	}

	return hr;
}

HRESULT D3D10DriverTypeToD3DDriverType(D3D10_DRIVER_TYPE d3d10DriverType, D3D_DRIVER_TYPE *d3dDriverType )
{
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_UNKNOWN;

	switch( d3d10DriverType )
	{
	case D3D10_DRIVER_TYPE_HARDWARE:
		driverType = D3D_DRIVER_TYPE_HARDWARE;
		break;
    case D3D10_DRIVER_TYPE_REFERENCE:
		driverType = D3D_DRIVER_TYPE_REFERENCE;
		break;
    case D3D10_DRIVER_TYPE_NULL:
		driverType = D3D_DRIVER_TYPE_NULL;
		break;
    case D3D10_DRIVER_TYPE_SOFTWARE:
		driverType = D3D_DRIVER_TYPE_SOFTWARE;
		break;
    case D3D10_DRIVER_TYPE_WARP:
		driverType = D3D_DRIVER_TYPE_WARP;
		break;
	}

	*d3dDriverType = driverType;
	
	return S_OK;
}

UINT D3D10CreateDeviceFlagsToD3D11CreateDeviceFlags(UINT d3d10Flags)
{
	UINT d3d11Flags = 0;

    if( d3d10Flags & D3D10_CREATE_DEVICE_SINGLETHREADED	)
		d3d11Flags |= D3D11_CREATE_DEVICE_SINGLETHREADED;

	if( d3d10Flags & D3D10_CREATE_DEVICE_DEBUG	)
		d3d11Flags |= D3D11_CREATE_DEVICE_DEBUG;

	if( d3d10Flags & D3D10_CREATE_DEVICE_SWITCH_TO_REF	)
		d3d11Flags |= D3D11_CREATE_DEVICE_SWITCH_TO_REF;

	if( d3d10Flags & D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS	)
		d3d11Flags |= D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;

	if( d3d10Flags & D3D10_CREATE_DEVICE_BGRA_SUPPORT	)
		d3d11Flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	return d3d11Flags;
};

D3D_FEATURE_LEVEL D3D10FeatureLevel1ToD3DFeatureLevel(D3D10_FEATURE_LEVEL1 featureLevel10)
{
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_10_0;

	switch( featureLevel10 )
	{
		case D3D10_FEATURE_LEVEL_10_0:
			featureLevel = D3D_FEATURE_LEVEL_10_0;
			break;
		case D3D10_FEATURE_LEVEL_10_1:
			featureLevel = D3D_FEATURE_LEVEL_10_1;
			break;
		case D3D10_FEATURE_LEVEL_9_1:
			featureLevel = D3D_FEATURE_LEVEL_9_1;
			break;
		case D3D10_FEATURE_LEVEL_9_2:
			featureLevel = D3D_FEATURE_LEVEL_9_2;
			break;
		case D3D10_FEATURE_LEVEL_9_3:
			featureLevel = D3D_FEATURE_LEVEL_9_3;
			break;
	}

	return featureLevel;
}

D3D10_FEATURE_LEVEL1 D3DFeatureLevel1ToD3D10FeatureLevel1( D3D_FEATURE_LEVEL1 featureLevel )
{
	D3D10_FEATURE_LEVEL1 featureLevel10 = D3D10_FEATURE_LEVEL_10_0;

	switch( featureLevel )
	{
		case D3D_FEATURE_LEVEL_10_0:
			featureLevel10 = D3D10_FEATURE_LEVEL_10_0;
			break;
		case D3D_FEATURE_LEVEL_10_1:
			featureLevel10 = D3D10_FEATURE_LEVEL_10_1;
			break;
		case D3D_FEATURE_LEVEL_9_1:
			featureLevel10 = D3D10_FEATURE_LEVEL_9_1;
			break;
		case D3D_FEATURE_LEVEL_9_2:
			featureLevel10 = D3D10_FEATURE_LEVEL_9_2;
			break;
		case D3D_FEATURE_LEVEL_9_3:
			featureLevel10 = D3D10_FEATURE_LEVEL_9_3;
			break;
	}

	return featureLevel10;
}

HRESULT LocalD3D10CreateDevice1(
	IDXGIAdapter *pAdapter,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	D3D10_FEATURE_LEVEL1 FeatureLevel,
	UINT SDKVersion,
	ID3D10Device1 **ppDevice)
{
	HRESULT hresult = E_FAIL;
	ID3D10Device *device10 = nullptr;
	ID3D11Device *device11 = nullptr;
	ID3D11Device1 *device11_1 = nullptr;
	ID3D11DeviceContext* immediateContext = nullptr;
	ID3D11DeviceContext1* immediateContext1 = nullptr;
	ID3DDeviceContextState *functionBlock = nullptr;

	CD3DTestFramework *pFramework = dynamic_cast< CD3DTestFramework *>( GetFramework() );

	switch(pFramework->m_D3DOptions.origInterfaceLevel)
	{
	case D3DTEST_INTERFACE_LEVEL_10_1:
		{
			// Call straight through to the real D3D10CreateDevice1 method
			hresult = IndirectD3D10CreateDevice1(
				pAdapter,
				DriverType,
				Software,
				Flags,
				FeatureLevel,
				D3D10_1_SDK_VERSION,
				ppDevice
				);
		}

		if(FAILED(hresult))
		{
				pFramework->WriteToLog( "Failed D3D10.1 Device creation." );
				goto cleanup;
		}
		break;

	case D3DTEST_INTERFACE_LEVEL_10_0:
		{
			// Translate to D3D10CreateDevice call
			hresult = IndirectD3D10CreateDevice(
				pAdapter,
				DriverType,
				Software,
				Flags & ~(D3D10_CREATE_DEVICE_BGRA_SUPPORT), // Strip out BGRA support bit if set
				D3D10_SDK_VERSION,
				&device10 );

			if(FAILED(hresult))
			{
				pFramework->WriteToLog( "Failed D3D10 Device creation." );
				goto cleanup;
			}

			// Query for 10.1 device 
			if( FAILED( hresult = device10->QueryInterface(__uuidof(ID3D10Device1), (void **) ppDevice) ) )
			{
				pFramework->WriteToLog( "Failed query for D3D10.1 device from D3D10 device." );
				goto cleanup;
			}

			// Create a D3D10.1 Function Block and activate it to enable 10.1 features
			if( FAILED( hresult = device10->QueryInterface(__uuidof(ID3D11Device1), (void **) &device11_1 ) ) )
			{
				pFramework->WriteToLog( "Failed query for D3D11.1 device from D3D10 device." );
				goto cleanup;
			}
			D3D_FEATURE_LEVEL FeatureLevelCreated;
			D3D_FEATURE_LEVEL featureLevels[1];
			featureLevels[0] = D3D10FeatureLevel1ToD3DFeatureLevel( FeatureLevel );
			assert( functionBlock == nullptr );
			if( FAILED( hresult = device11_1->CreateDeviceContextState( Flags & D3D_CREATE_DEVICE_SINGLETHREADED, featureLevels, 1, D3D11_SDK_VERSION, __uuidof(ID3D10Device1), &FeatureLevelCreated, &functionBlock ) ) )
			{
				pFramework->WriteToLog( "Failed creation of D3D10.1 device context from a D3D11.1 device." );
				goto cleanup;
			}

			assert( immediateContext1 == nullptr );
			device11_1->GetImmediateContext1( &immediateContext1 );
			immediateContext1->SwapDeviceContextState( functionBlock, nullptr );
		}
		break;
	case D3DTEST_INTERFACE_LEVEL_11_1:
	case D3DTEST_INTERFACE_LEVEL_11_0:
		{
			D3D_DRIVER_TYPE d3dDriverType;
			hresult = D3D10DriverTypeToD3DDriverType(DriverType, &d3dDriverType);
			if(FAILED( hresult ) )
				goto cleanup;
			D3D_FEATURE_LEVEL FeatureLevelCreated;

			D3D_FEATURE_LEVEL featureLevels[1];
			featureLevels[0] = pFramework->m_D3DOptions.origFeatureLevel;

			// Create a D3D11 device
			hresult = IndirectD3D11CreateDevice(
				pAdapter,
				pAdapter == nullptr ? d3dDriverType : D3D_DRIVER_TYPE_UNKNOWN,
				Software,
				D3D10CreateDeviceFlagsToD3D11CreateDeviceFlags(Flags),
				featureLevels,
				1,
				D3D11_SDK_VERSION,
				&device11,
				&FeatureLevelCreated,
				&immediateContext
				);

			if(FAILED(hresult))
			{
				pFramework->WriteToLog( "Failed D3D11 Device creation." );
				goto cleanup;
			}

			// Query for 11.1 device and context
			if( FAILED( hresult = device11->QueryInterface(__uuidof(ID3D11Device1), (void **) &device11_1 ) ) )
			{
				pFramework->WriteToLog( "Failed query for D3D11.1 device from D3D11 device." );
				goto cleanup;
			}

			// Create a D3D10.1 Function Block and activate it
			featureLevels[0] = D3D10FeatureLevel1ToD3DFeatureLevel( FeatureLevel );
			assert( functionBlock == nullptr );
			if( FAILED( hresult = device11_1->CreateDeviceContextState( Flags & D3D_CREATE_DEVICE_SINGLETHREADED, featureLevels, 1, D3D11_SDK_VERSION, __uuidof(ID3D10Device1), &FeatureLevelCreated, &functionBlock ) ) )
			{
				pFramework->WriteToLog( "Failed creation of D3D10.1 device context from a D3D11.1 device." );
				goto cleanup;
			}

			assert( immediateContext1 == nullptr );
			device11_1->GetImmediateContext1( &immediateContext1 );

			immediateContext1->SwapDeviceContextState( functionBlock, nullptr );

			// Query for 10.1 device 
			if( FAILED( hresult = device11->QueryInterface(__uuidof(ID3D10Device1), (void **) ppDevice) ) )
			{
				pFramework->WriteToLog( "Failed query for D3D10.1 device from D3D11 device." );
				goto cleanup;
			}

		}
		break;
	}

cleanup:
	SAFE_RELEASE( immediateContext1 );
	SAFE_RELEASE( immediateContext );
	SAFE_RELEASE( functionBlock );
	SAFE_RELEASE( device11_1 );
	SAFE_RELEASE( device11 );
	SAFE_RELEASE( device10 );

	return hresult;
}

HRESULT LocalD3D10CreateDevice(
	IDXGIAdapter *pAdapter,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	UINT SDKVersion,
	ID3D10Device **ppDevice)
{
	HRESULT hresult = E_FAIL;
	ID3D10Device1 *device10_1 = nullptr;
	ID3D11Device *device11 = nullptr;
	ID3D11Device1 *device11_1 = nullptr;
	ID3D11DeviceContext* immediateContext = nullptr;
	ID3D11DeviceContext1* immediateContext1 = nullptr;
	ID3DDeviceContextState *functionBlock = nullptr;

	CD3DTestFramework *pFramework = dynamic_cast< CD3DTestFramework *>( GetFramework() );

	switch(pFramework->m_D3DOptions.origInterfaceLevel)
	{
	case D3DTEST_INTERFACE_LEVEL_10_0:
		{
			//
			// Load D3D10.DLL and find the D3D10CreateDevice entry point.
			//
			// Call straight through to the real D3D10CreateDevice method
			hresult = IndirectD3D10CreateDevice(
				pAdapter,
				DriverType,
				Software,
				Flags,
				D3D10_SDK_VERSION,
				ppDevice
				);
		}

		if(FAILED(hresult))
		{
				pFramework->WriteToLog( "Failed D3D10 Device creation." );
				goto cleanup;
		}
		break;

	case D3DTEST_INTERFACE_LEVEL_10_1:
		{
			// Translate to D3D10CreateDevice call
			hresult = IndirectD3D10CreateDevice1(
				pAdapter,
				DriverType,
				Software,
				Flags & ~(D3D10_CREATE_DEVICE_BGRA_SUPPORT), // Strip out BGRA support bit if set
				D3DFeatureLevel1ToD3D10FeatureLevel1( pFramework->m_D3DOptions.origFeatureLevel ),
				D3D10_1_SDK_VERSION,
				&device10_1
				);

			if(FAILED(hresult))
			{
				pFramework->WriteToLog( "Failed D3D10.1 Device creation." );
				goto cleanup;
			}

			// Query for 10.0 device 
			if( FAILED( hresult = device10_1->QueryInterface(__uuidof(ID3D10Device), (void **) ppDevice) ) )
			{
				pFramework->WriteToLog( "Failed query for D3D10 device from D3D10.1 device." );
				goto cleanup;
			}

			// Activate a D3D10 DCS to enable 10.0 validation
			if( FAILED( hresult = device10_1->QueryInterface(__uuidof(ID3D11Device1), (void **) &device11_1 ) ) )
			{
				pFramework->WriteToLog( "Failed query for D3D11.1 device from D3D10.1 device." );
				goto cleanup;
			}
			D3D_FEATURE_LEVEL FeatureLevelCreated;
			D3D_FEATURE_LEVEL featureLevels[1];
			featureLevels[0] = D3D_FEATURE_LEVEL_10_0;
			assert( functionBlock == nullptr );
			if( FAILED( hresult = device11_1->CreateDeviceContextState( Flags & D3D_CREATE_DEVICE_SINGLETHREADED, featureLevels, 1, D3D11_SDK_VERSION, __uuidof(ID3D10Device), &FeatureLevelCreated, &functionBlock ) ) )
			{
				pFramework->WriteToLog( "Failed creation of D3D10 device context from a D3D11.1 device." );
				goto cleanup;
			}

			assert( immediateContext1 == nullptr );
			device11_1->GetImmediateContext1( &immediateContext1 );
			immediateContext1->SwapDeviceContextState( functionBlock, nullptr );
		}
		break;

	case D3DTEST_INTERFACE_LEVEL_11_1:
	case D3DTEST_INTERFACE_LEVEL_11_0:
		{
			D3D_FEATURE_LEVEL FeatureLevelCreated;

			ID3D11Device *device11 = nullptr; // BUGBUG - Need to change to 11.1 device when available
			D3D_DRIVER_TYPE d3dDriverType;
			if(FAILED(hresult = D3D10DriverTypeToD3DDriverType(DriverType, &d3dDriverType)))
				goto cleanup;

			D3D_FEATURE_LEVEL featureLevels[1];
			featureLevels[0] = pFramework->m_D3DOptions.origFeatureLevel;

			// Create a D3D11 device
			hresult = IndirectD3D11CreateDevice(
				pAdapter,
				pAdapter == nullptr ? d3dDriverType : D3D_DRIVER_TYPE_UNKNOWN,
				Software,
				D3D10CreateDeviceFlagsToD3D11CreateDeviceFlags(Flags),
				featureLevels,
				1,
				D3D11_SDK_VERSION,
				&device11,
				nullptr,
				nullptr
				);

			if(FAILED(hresult))
			{
				pFramework->WriteToLog( "Failed D3D11 Device creation." );
				goto cleanup;
			}

			// Query for 11.1 device and context
			if( FAILED( hresult = device11->QueryInterface(__uuidof(ID3D11Device1), (void **) &device11_1 ) ) )
			{
				pFramework->WriteToLog( "Failed query for D3D11.1 device from D3D11 device." );
				goto cleanup;
			}

			// Create a D3D10.0 Function Block and activate it
			featureLevels[0] = pFramework->m_D3DOptions.FeatureLevel;
			assert( functionBlock == nullptr );
			if( FAILED( hresult = device11_1->CreateDeviceContextState( Flags & D3D_CREATE_DEVICE_SINGLETHREADED, featureLevels, 1, D3D11_SDK_VERSION, __uuidof(ID3D10Device), &FeatureLevelCreated, &functionBlock ) ) )
			{
				pFramework->WriteToLog( "Failed creation of D3D10 device context from a D3D11.1 device." );
				goto cleanup;
			}

			assert( immediateContext1 == nullptr );
			device11_1->GetImmediateContext1( &immediateContext1 );
			immediateContext1->SwapDeviceContextState( functionBlock, nullptr );

			// Query for 10.1 device 
			if( FAILED( hresult = device11->QueryInterface(__uuidof(ID3D10Device1), (void **) ppDevice) ) )
			{
				pFramework->WriteToLog( "Failed query for D3D10.1 device from D3D11 device." );
				goto cleanup;
			}

		}
		break;
	}

cleanup:
	SAFE_RELEASE( immediateContext1 );
	SAFE_RELEASE( immediateContext );
	SAFE_RELEASE( functionBlock );
	SAFE_RELEASE( device11_1 );
	SAFE_RELEASE( device11 );
	SAFE_RELEASE( device10_1 );

	return hresult;
}


//--------------------------------------------------------------------

CD3DTestFramework::CD3DTestFramework()
:
	CGraphicsTestFramework
	(
		&m_D3DOptions
	),
	m_DXGIFactories( nullptr ),
	m_pAdapters( nullptr ),
	m_pSwapChains( nullptr ),
	m_pD3DDevices( nullptr ),
	m_pD3DDebugs( nullptr ),
	m_pD3DInfoQueues( nullptr ),
	m_pD3DMultithreads( nullptr ),
	m_pD3DSwitchToRef( nullptr ),
	m_pExtensions( nullptr ),
	m_pRTTextures( nullptr ),
	m_pRTTextures1D( nullptr ),
	m_pRTBuffers( nullptr ),
	m_pRTViews( nullptr ),
	m_pDepthStencils( nullptr ),
	m_pDepthStencils1D( nullptr ),
	m_pDSViews( nullptr ),
	m_bD3DValid( false ),
	m_bRenderTargetValid( false ),
	m_bDXGIValid( false ),
	m_bSetupCalled( false ),
	m_uDeviceCount( 0 ),
	m_uSwapChainCount( 0 ),
	m_bForce10_0( false ),
	m_bForce10_1( false ),
	m_bForceFeatureLevel( false ),
	m_bCommitLowerMemory( false),
	m_bReserveLowerMemory(false),
	m_nQueryTimeout( 5000 )	// Timeout in ms
{
	// Initialize cached options to all 0xff...
	memset( &s_DXGIOptionsCached, 0xff, sizeof( s_DXGIOptionsCached ) );
	memset( &s_D3DOptionsCached, 0xff, sizeof( s_D3DOptionsCached ) );
	memset( &s_RenderTargetOptionsCached, 0xff, sizeof( s_RenderTargetOptionsCached ) );


	// Intialize default options
	memset( &m_RenderTargetOptions, 0, sizeof( m_RenderTargetOptions ) );

	m_D3DOptions.wVersion = 0xffff;
	m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_10_1;
	m_D3DOptions.wMinVersion = 0x1000;
	m_D3DOptions.wMaxVersion = 0x1010;
	m_D3DOptions.SrcOnly = false;
	m_D3DOptions.SrcDriverType = D3D_DRIVER_TYPE_HARDWARE;
	m_D3DOptions.RefDriverType = D3D_DRIVER_TYPE_REFERENCE;
	m_D3DOptions.bStricterValidationLayer = false;

	// Initialize render target options
	m_RenderTargetOptions.UseDefaultDepthStencilView = true;
	m_RenderTargetOptions.UseDefaultRenderTargetView = true;
	m_RenderTargetOptions.NoRenderTargets = false;
	m_RenderTargetOptions.NoDepthStencils = false;
	m_RenderTargetOptions.NoSwapChains = false;

	// Default DXGI_SWAP_CHAIN_DESC
	m_RenderTargetOptions.SwapChainDesc.BufferCount = 1;
	m_RenderTargetOptions.SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_RenderTargetOptions.SwapChainDesc.Height = 280;
	m_RenderTargetOptions.SwapChainDesc.Width = 320;
	m_RenderTargetOptions.SwapChainDesc.Stereo = false;
	m_RenderTargetOptions.SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_RenderTargetOptions.SwapChainDesc.SampleDesc.Count = 1;
	m_RenderTargetOptions.SwapChainDesc.SampleDesc.Quality = 0;
	m_RenderTargetOptions.SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	m_RenderTargetOptions.SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	m_RenderTargetOptions.SwapChainDesc.Flags = 0;
	
    m_RenderTargetOptions.OutputWindow = nullptr;
	m_RenderTargetOptions.Windowed = FALSE;

	// Default D3D_RENDER_TARGET_VIEW_DESC 
	m_RenderTargetOptions.RenderTargetViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	m_RenderTargetOptions.RenderTargetViewDesc.ViewDimension = D3D_RTV_DIMENSION_UNKNOWN;
	m_RenderTargetOptions.RenderTargetViewDesc.Texture2D.MipSlice = 0;
	
	// Default D3D_DEPTH_STENCIL_VIEW_DESC
	m_RenderTargetOptions.DepthStencilViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	m_RenderTargetOptions.DepthStencilViewDesc.ViewDimension = D3D_DSV_DIMENSION_UNKNOWN;
	m_RenderTargetOptions.DepthStencilViewDesc.Texture2D.MipSlice = 0;
		
	// Default D3D_DEPTH_STENCIL_DESC
    m_RenderTargetOptions.DepthStencilDesc.DepthEnable = TRUE;
    m_RenderTargetOptions.DepthStencilDesc.DepthWriteMask = D3D_DEPTH_WRITE_MASK_ALL;
    m_RenderTargetOptions.DepthStencilDesc.DepthFunc = D3D_COMPARISON_LESS_EQUAL;
    m_RenderTargetOptions.DepthStencilDesc.StencilEnable = FALSE;

	m_RenderTargetOptions.DepthTextureDesc.ArraySize = 1;
	m_RenderTargetOptions.DepthTextureDesc.BindFlags = D3D_BIND_DEPTH_STENCIL;
	m_RenderTargetOptions.DepthTextureDesc.CPUAccessFlags = 0;
	m_RenderTargetOptions.DepthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_RenderTargetOptions.DepthTextureDesc.Height = 280;
	m_RenderTargetOptions.DepthTextureDesc.Width = 320;
	m_RenderTargetOptions.DepthTextureDesc.MipLevels = 0;
	m_RenderTargetOptions.DepthTextureDesc.MiscFlags = 0;
	m_RenderTargetOptions.DepthTextureDesc.SampleDesc.Count = 1;
	m_RenderTargetOptions.DepthTextureDesc.Usage = D3D_USAGE_DEFAULT;

	// Tests should run the best available DXGI version
	m_DXGIOptions.Version = TEST_DXGI_VERSION_BESTAVAILABLE;

	hMainWaiting = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	hWorkerWaiting = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	dwMainThread = GetCurrentThreadId();
}

//--------------------------------------------------------------------

CD3DTestFramework::~CD3DTestFramework()
{
	CloseHandle(hMainWaiting);
	CloseHandle(hWorkerWaiting);
}

 //--------------------------------------------------------------------

bool  CD3DTestFramework::AllocLowerHalfOfAddressSpace( DWORD flags )
{
	SYSTEM_INFO     sysinfo = {0};

	GetSystemInfo(&sysinfo);
	
	// If not large-address aware, exit.
	if ( (size_t)sysinfo.lpMaximumApplicationAddress <= 0x7ffeffff )
	{
		return false;
	}
		
	void   *pMem = nullptr;
    DWORD   dwSizeToAlloc = 0x80000000;  
    
	// Allocate ALL virtual memory
	do
	{
        pMem = nullptr;

        while ( pMem == nullptr && dwSizeToAlloc > 0 )
        {
		    pMem = VirtualAlloc( nullptr, dwSizeToAlloc, flags, PAGE_NOACCESS );

            if ( pMem == nullptr )
            {
                 dwSizeToAlloc >>= 1;
            }
        }

		if ( pMem != nullptr )
		{
			g_lstVAddresses.push_back( pMem );
		}
	}
	while( pMem != nullptr );

	// Free all memory above the 2GB boundary
	list<void*>::iterator  i;

	for( i = g_lstVAddresses.begin(); i != g_lstVAddresses.end(); ++i )
	{
		if( (size_t)*i > 0x80000000 )
		{
			VirtualFree( *i, 0, MEM_RELEASE );
            *i = nullptr;
		}
	}

	// Allocate any small areas of heap left over
	void *pAfter = nullptr;
    dwSizeToAlloc = sysinfo.dwAllocationGranularity;
	
	do
	{
        pAfter = nullptr;

        while ( pAfter == nullptr && dwSizeToAlloc > 0 )
        {
		    pAfter = malloc( dwSizeToAlloc );

            if ( pAfter == nullptr )
            {
                 dwSizeToAlloc >>= 1;
            }
        }

        if ( pAfter != nullptr )
        {
			g_lstHAddresses.push_back( pAfter );
        }
	}
    while( pAfter != nullptr && pAfter < (PVOID)0xB0000000 );  // Stop the loop when allocation addresses are well past the 2GB boundary.

	// Again, free all memory above the 2GB boundary
	for( i = g_lstHAddresses.begin(); i != g_lstHAddresses.end(); ++i )
	{
		if( (size_t)*i > 0x80000000 )
		{
			free( *i );
            *i = nullptr;
		}
	}
        
	return true;
}

//--------------------------------------------------------------------

bool CD3DTestFramework::Setup()
{
	// Copy current device settings to global cache
	memcpy( &m_DXGIOptionsGlobal, &m_DXGIOptions, sizeof( m_DXGIOptions ) );
	memcpy( &m_D3DOptionsGlobal, &m_D3DOptions, sizeof( m_D3DOptions ) );
	memcpy( &m_RenderTargetOptionsGlobal, &m_RenderTargetOptions, sizeof( m_RenderTargetOptions ) );

	//Setup in the base classes parses and sets option variables, so now
	//here we force sub pixel precision to 4 for WARP and 8 for WHQL
	if(m_bWARP)
	{
		m_iSubPixelPrecision=4;
	}
	if(GetTestMode() == WGFTEST_MODE_WHQL)
	{
		m_bWARP=false;
		m_iSubPixelPrecision=8;
	}

	//Preallocate lower half of virtual memory?
	if(m_bReserveLowerMemory || m_bCommitLowerMemory)
	{
		DWORD flags=MEM_RESERVE;
		if(m_bCommitLowerMemory)
		{
			flags = MEM_RESERVE | MEM_COMMIT;
		}
		AllocLowerHalfOfAddressSpace(flags);
	}
	
	// If running in Mobile mode, have the FormatHelper use WARP instead of REF
	CFormatHelper::SetReferenceDeviceType(IsMobile());

	return true;
}

//--------------------------------------------------------------------

bool  CD3DTestFramework::FreeLowerHalfOfAddressSpace()
{
	SYSTEM_INFO     sysinfo = {0};

	GetSystemInfo(&sysinfo);
	
	// If not large-address aware, exit.
	if ( (size_t)sysinfo.lpMaximumApplicationAddress <= 0x7ffeffff )
	{
		return false;
	}
		
	// Free all memory
	list<void*>::iterator  i;

	for( i = g_lstVAddresses.begin(); i != g_lstVAddresses.end(); ++i )
	{
    	VirtualFree( *i, 0, MEM_RELEASE );
	}

	for( i = g_lstHAddresses.begin(); i != g_lstHAddresses.end(); ++i )
	{
    	free( *i );
	}

	return true;
}

//--------------------------------------------------------------------

void CD3DTestFramework::SetRuntimeVersion()
{
	//If requested to run latest version of runtime, use the max version the test supports.
	//TODO: This might not be the behavior we want.  remove this todo if it is.
	if (m_D3DOptions.wVersion == 0xffff)
		m_D3DOptions.wVersion = m_D3DOptions.wMaxVersion;

	// Run with 10.1 on WHQL.
	if (GetTestMode() == WGFTEST_MODE_WHQL)
	{
		m_D3DOptions.wVersion = 0x1010;
		m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_10_1;
	}

	// setup the devices just to set the actual supported feature level 
	// for "-whql" mode before InitTestParameters()
    if( CD3DTestFramework::SetupDXGI() == RESULT_PASS )
    {
        CD3DTestFramework::SetupD3D();
        CD3DTestFramework::CleanupDXGI();
    }
}

//--------------------------------------------------------------------

void CD3DTestFramework::Cleanup()
{
	CleanupDXGI();

    FreeLowerHalfOfAddressSpace();
}

//--------------------------------------------------------------------

void CD3DTestFramework::CleanupTest()
{
	CFormatHelper::Cleanup();

	// TODO:  Add aperture corruption detection code here.

	CWGFTestFramework::CleanupTest();
}

//--------------------------------------------------------------------

TEST_RESULT CD3DTestFramework::SetupDXGI()
{
	//
	// Initialize locals.
	//

	TEST_RESULT SetupResult = RESULT_PASS;

	//
	// Determine how many devices the test needs, and allocate
	// a corresponding number of DXGI adapters and outputs.
	//

	m_uDeviceCount = m_D3DOptions.SrcOnly ? 1 : 2;

	m_DXGIFactories = new DXGI_FACTORY_POINTER [ m_uDeviceCount ];
	ZeroMemory(m_DXGIFactories, sizeof(DXGI_FACTORY_POINTER) * m_uDeviceCount);

	m_pAdapters = new IDXGIAdapter * [ m_uDeviceCount ]; 
	ZeroMemory(m_pAdapters, sizeof(IDXGIAdapter *) * m_uDeviceCount);

    if (!IsSession0())
    {
	    m_pAdapterOutputs = new IDXGIOutput * [ m_uDeviceCount ]; 
	    ZeroMemory(m_pAdapterOutputs, sizeof(IDXGIOutput *) * m_uDeviceCount);
    }

	//
	// Enumerate adapters and outputs for each device slot.
	//

	for ( int i = (int) m_uDeviceCount - 1; i >= 0; --i )
	{
        //
        // First, ensure that the requested AdapterType is supported by the requested version of DXGI
        //

        if( !IsAllowedAdapterType( i ) )
        {
            WriteToLogF( _T( "The requested AdapterType (%s) is not supported by the requested version of DXGI (%s)." ),
                        ToString(m_DXGIOptions.AdapterType[i]).c_str(), ToString(m_DXGIOptions.Version).c_str() );

            SetupResult = GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
			goto Cleanup;
        }

		//
		// Create the DXGI factory
		//

		HRESULT hCreateFactoryResult = CreateDXGIFactory( &m_DXGIFactories[i] );
		if ( FAILED( hCreateFactoryResult ) )
		{
			WriteToLog( _T( "CreateDXGIFactory failed." ) );

			SetupResult = GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
			goto Cleanup;
		};

        //
        // Set-up adapters and fullscreen outputs
        //

        // Don't enumereate adapters or monitors in Session0 mode
        if (!IsSession0())
        {
            SetupResult = SetupAdapter( i, &m_DXGIFactories[i], &m_pAdapters[i] );
            if( SetupResult != RESULT_PASS )
            {
                goto Cleanup;
            }

            SetupResult = SetupFullscreenOutput( i, m_pAdapters[i], &m_pAdapterOutputs[i] );
            if( SetupResult != RESULT_PASS )
            {
                goto Cleanup;
            }
        }
    }

	//
	// Cache our DXGI options, and note that DXGI is ready for business.
	//

	memcpy( &s_DXGIOptionsCached, &m_DXGIOptions, sizeof( m_DXGIOptions ) );
	m_bDXGIValid = true;

Cleanup:

	if ( SetupResult != RESULT_PASS )
	{
		CleanupDXGI();
	}
	else
	{
		// D3D interfaces will need to be recreated.
		CleanupD3D();
	}

	return SetupResult;
};


//--------------------------------------------------------------------

void CD3DTestFramework::CleanupDXGI()
{
	CleanupD3D(); // Release D3D interfaces

	for (int i = m_uDeviceCount - 1; i >= 0; i--)
	{
        if (m_pAdapterOutputs != nullptr)
        {
    		SAFE_RELEASE( m_pAdapterOutputs[ i ] );
        }

        if (m_pAdapters != nullptr)
        {
		    SAFE_RELEASE( m_pAdapters[ i ] );
        }

        if (m_DXGIFactories != nullptr)
        {
		    SAFE_RELEASE( m_DXGIFactories[ i ].pFactory );
        }
	}
	SAFE_DELETE_ARRAY( m_pAdapters );
	SAFE_DELETE_ARRAY( m_pAdapterOutputs );
	SAFE_DELETE_ARRAY( m_DXGIFactories );
	
	m_uDeviceCount = 0;

	m_bDXGIValid = false;
}

//--------------------------------------------------------------------

TEST_RESULT CD3DTestFramework::SetupD3D()
{
	//
	// Initialize locals.
	//

	HRESULT hr = E_FAIL;
	DWORD dwFlags = 0;
	
	dwFlags |= m_D3DOptions.Debug ? D3D_CREATE_DEVICE_DEBUG : 0;
	dwFlags |= m_D3DOptions.SingleThreaded ? D3D_CREATE_DEVICE_SINGLETHREADED : 0;
	dwFlags |= m_D3DOptions.PreventThreadingOptimizations ? D3D_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS : 0;
	dwFlags |= m_D3DOptions.bStricterValidationLayer ? D3D_CREATE_DEVICE_STRICT_VALIDATION : 0;

	if( m_D3DOptions.allowSwitchToRef || m_D3DOptions.forceSwitchToRef )
	{
		dwFlags |= D3D_CREATE_DEVICE_SWITCH_TO_REF;
	}

	if( (int) m_D3DOptions.origFeatureLevel == 0 )
	{
		// Match origFeatureLevel to origInterfaceLevel
		switch( m_D3DOptions.origInterfaceLevel )
		{
			case D3DTEST_INTERFACE_LEVEL_10_0:
				m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_10_0;
				break;
			case D3DTEST_INTERFACE_LEVEL_10_1:
				m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_10_1;
				break;
			case D3DTEST_INTERFACE_LEVEL_11_0:
				m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_11_0;
				break;
			case D3DTEST_INTERFACE_LEVEL_11_1:
				m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_11_1;
				break;
		}
	}

	if( m_D3DOptions.FeatureLevel <= (UINT) D3D_FEATURE_LEVEL_9_3 && m_D3DOptions.origInterfaceLevel == D3DTEST_INTERFACE_LEVEL_10_0 )
	{
		// D3D10.0 interfaces cannot specify interface so automatically upgrade origInterfaceLevel to D3D10.1
		m_D3DOptions.origInterfaceLevel = D3DTEST_INTERFACE_LEVEL_10_1;

		// If FL9.x is the target feature level then origFeatureLevel must be 9.x as well
		if( m_D3DOptions.origFeatureLevel >= (UINT) D3D_FEATURE_LEVEL_10_0 )
			m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_9_1;
	}


	//
	// Create devices.
	//

	SAFE_RELEASE( m_pD3DDevices );

		
	// BUGBUG - I don't know if I like allocating device arrays dynamically.

	// BUGBUG - Creating only one device for now.  We need some way to indicate how subsequent
	// devices are created.
	m_pD3DDevices = new CMultiDevice( m_uDeviceCount );
	m_pD3DDevices->AddRef();

	for (int i = m_uDeviceCount - 1; i >= 0; i--)
	{
		D3D_DRIVER_TYPE DriverType = D3D_DRIVER_TYPE_REFERENCE;
		if (i == 0)
		{
			DriverType = m_D3DOptions.SrcDriverType;
		}
		else
		{
			DriverType = m_D3DOptions.RefDriverType;
		};

		// Check for proxy driver flag.
		if (m_D3DOptions.proxyDriver) //driver type must be software to load module
		{
			DriverType = D3D_DRIVER_TYPE_SOFTWARE;
		}

		CDevice *pDevice = nullptr;

		if( (GetTestMode() == WGFTEST_MODE_WHQL) && !m_bForce10_0 && !m_bForce10_1 )
		{
			m_D3DOptions.wVersion = 0x1010;
			if (!m_bForceFeatureLevel)
			{
				m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_10_1;
				if( FAILED( hr = CreateDevice( m_pAdapters ? &m_pAdapters[ i ] : nullptr, DriverType, dwFlags, m_D3DOptions.wVersion, &pDevice ) ) )
				{
					WriteToLog( _T( "WHQL: Failed to create D3D10.1 device, falling back to Feature Level D3D10.0" ) );
					m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_10_0;
					hr = CreateDevice( m_pAdapters ? &m_pAdapters[ i ] : nullptr, DriverType, dwFlags, m_D3DOptions.wVersion, &pDevice );
				}
			}
			else
			{
				hr = CreateDevice( m_pAdapters ? &m_pAdapters[ i ] : nullptr, DriverType, dwFlags, m_D3DOptions.wVersion, &pDevice );
			}

			if( FAILED( hr ) )
			{
				WriteToLog( _T( "WHQL: Failed to create D3D10.1 device, falling back to D3D10.0 API" ) );
				m_D3DOptions.wVersion = 0x1000;
				m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_10_0;
				hr = CreateDevice( m_pAdapters ? &m_pAdapters[ i ] : nullptr, DriverType, dwFlags, m_D3DOptions.wVersion, &pDevice );
			}
		}
		else if ( (GetTestMode() == WGFTEST_MODE_WHQL) && m_D3DOptions.wVersion == 0x1010 && !m_bForceFeatureLevel )
		{
			m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_10_1;
			if( FAILED( hr = CreateDevice( m_pAdapters ? &m_pAdapters[ i ] : nullptr, DriverType, dwFlags, m_D3DOptions.wVersion, &pDevice ) ) )
			{
				WriteToLog( _T( "WHQL: Failed to create D3D10.1 device, falling back to Feature Level D3D10.0" ) );
				m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_10_0;
				hr = CreateDevice( m_pAdapters ? &m_pAdapters[ i ] : nullptr, DriverType, dwFlags, m_D3DOptions.wVersion, &pDevice );
			}
		}
		else
		{
			hr = CreateDevice( m_pAdapters ? &m_pAdapters[ i ] : nullptr, DriverType, dwFlags, m_D3DOptions.wVersion, &pDevice );
		}
		
		if( FAILED( hr ) )
		{
			SAFE_RELEASE( m_pD3DDevices );
			
			if
			(
				hr == DXGI_ERROR_UNSUPPORTED ||
				hr == E_NOINTERFACE ||
				hr == E_NOTIMPL
			)
			{
				return RESULT_SKIP; // Unsupported device types should just cause a skip
			}
			else
			{
				WriteError( _T( "SetupD3D - CreateDevice() failed to create device. hr = %s" ), D3DHResultToString(hr).c_str() );
				return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
			}
		}

		m_pD3DDevices->SetSubDevice( i, pDevice );
		pDevice->Release();
	}



	// Create the D3D debugger object.
	if( m_D3DOptions.Debug )
	{
		assert( m_pD3DDebugs == nullptr );
		if( FAILED( m_pD3DDevices->CreateDebug( &m_pD3DDebugs ) ))
		{
			WriteToLog( _T( "Failure creating CMultiDebug object." ) );
			CleanupD3D();
			return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
		}

		assert( m_pD3DInfoQueues == nullptr );
		if( FAILED( m_pD3DDevices->CreateInfoQueue( &m_pD3DInfoQueues ) ))
		{
			WriteToLog( _T( "Failure creating CInfoQueueDebug object." ) );
			CleanupD3D();
			return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
		}
	}

	// Create the D3D thread safety object.
	if( !m_D3DOptions.SingleThreaded && m_D3DOptions.wVersion < 0x1100 )
	{
		assert( m_pD3DMultithreads == nullptr );
		if( FAILED( m_pD3DDevices->CreateMultithread( &m_pD3DMultithreads ) ))
		{
			WriteToLog( _T( "Failure creating CMultithread object." ) );
			CleanupD3D();
			return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
		}
	}

	memcpy( &s_D3DOptionsCached, &m_D3DOptions, sizeof( m_D3DOptions ) );
	m_bD3DValid = true;
	
	// Render targets will need to be reinitialized.
	CleanupRenderTargets();
	
	if( m_D3DOptions.forceSwitchToRef )
	{
		assert( m_pD3DSwitchToRef == nullptr );
		hr = m_pD3DDevices->CreateSwitchToRef( &m_pD3DSwitchToRef );
		if( FAILED( hr ) )
		{
			WriteToLog( _T( "Unexpected failure creating CSwitchToRef object" ) );
			return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
		}
		m_pD3DSwitchToRef->SetUseRef( true );
	}

	return RESULT_PASS;
}

//--------------------------------------------------------------------

void CD3DTestFramework::CleanupD3D()
{
	CleanupRenderTargets(); // This will invalidate render targets.

	SAFE_RELEASE( m_pD3DMultithreads );
	SAFE_RELEASE( m_pD3DInfoQueues );
	SAFE_RELEASE( m_pD3DDebugs );
	SAFE_RELEASE( m_pD3DDevices );
	SAFE_RELEASE( m_pD3DSwitchToRef );

	UnloadReferenceRasterizer();
	UnloadSoftwareRasterizer();
	
	m_bD3DValid = false;
}

//--------------------------------------------------------------------

TEST_RESULT CD3DTestFramework::SetupRenderTargets()
{
	CD3DTest *pTest = dynamic_cast< CD3DTest *>( GetCurrentTest() );
	
	assert( pTest && "dynamic_cast failed" );
	
	UINT numRenderWindowsPerDevice = pTest->GetNumRenderWindows();

	// Make sure the shell has the desired number of test windows.
	if( GetShell() )
		GetShell()->SetNumDeviceWindows( numRenderWindowsPerDevice * m_uDeviceCount );

	if ( numRenderWindowsPerDevice > 0 )
	{
		// Create arrays of swap chain and render target pointers.
		m_pSwapChains = new DXGI_SWAPCHAIN_POINTER [ numRenderWindowsPerDevice * m_uDeviceCount ];
		ZeroMemory(m_pSwapChains, sizeof( DXGI_SWAPCHAIN_POINTER ) * numRenderWindowsPerDevice * m_uDeviceCount);
		m_uSwapChainCount = numRenderWindowsPerDevice * m_uDeviceCount;
		m_pRTViews = new CRenderTargetView *[ numRenderWindowsPerDevice ];
		ZeroMemory(m_pRTViews, sizeof(CRenderTargetView *) * numRenderWindowsPerDevice);
	}

	// The default behavior is to create a render target and RT view for each render window.
	// The developer can however choose to let the framework manage separate render targets
	// by setting RenderTargetViewDesc.Format to something other than DXGI_FORMAT_UNKNOWN (the default value).
	// This will cause the framework to create render targets separate from the swap chains.
	bool createRTFromSwapChains = m_RenderTargetOptions.RenderTargetViewDesc.Format == DXGI_FORMAT_UNKNOWN;

	if( !m_RenderTargetOptions.NoRenderTargets && createRTFromSwapChains )
	{
		m_pRTTextures = new CTexture2D *[ numRenderWindowsPerDevice ];
		ZeroMemory(m_pRTTextures, sizeof(CTexture2D *) * numRenderWindowsPerDevice);
	}

	if( !m_RenderTargetOptions.NoSwapChains )
	{	
		// For each render window create a swap chain
		for( int i = numRenderWindowsPerDevice - 1; i >= 0; i-- )
		{
			HWND hWnd;
			bool bFullScreen = pTest->FullScreen();
			
			// For each device
			for( UINT j = 0; j < m_uDeviceCount; ++j )
			{
				// BUGBUG - Only one swap chain can be full screen at at time.  For now
				// only the first swap chain can be full screen.
				if( bFullScreen )
				{
					hWnd = GetShell()->GetMainHWnd(); // BUGBUG - I don't like this option.  Consider creating popup window.
					m_DXGIFactories[ j ].pFactory->MakeWindowAssociation( hWnd, DXGI_MWA_NO_ALT_ENTER );
				}
				else
				{
					if( !GetShell() )
					{
						WriteToLog( "Unable to use windowed mode without a shell" );
						CleanupRenderTargets();
						return RESULT_SKIP;
					}
				}

				hWnd = GetShell()->GetDeviceRenderWindow( i * m_uDeviceCount + j );
				m_RenderTargetOptions.OutputWindow = hWnd;
				m_RenderTargetOptions.Windowed = bFullScreen && j == 0 ? false : true;
				HRESULT hr = E_FAIL;
				switch( m_DXGIOptions.Version )
				{
				case TEST_DXGI_VERSION_1_3:
				case TEST_DXGI_VERSION_1_2:
                    if(GetFramework()->IsMobile() == FALSE)
                    {
					    assert( m_pSwapChains[ i * m_uDeviceCount + j ].pSwapChain1 == nullptr );
					        hr = m_DXGIFactories[ j ].pFactory2->CreateSwapChainForHwnd( 
						        m_pD3DDevices->GetSubDevice( j )->GetIUnknown(), 
						        m_RenderTargetOptions.OutputWindow,
						        &m_RenderTargetOptions.SwapChainDesc, 
						        m_RenderTargetOptions.Windowed ? nullptr : &m_RenderTargetOptions.FullscreenDesc,
						        nullptr, 
						        &m_pSwapChains[ i*m_uDeviceCount + j ].pSwapChain1 );
					    break;
                    }
				case TEST_DXGI_VERSION_1_1:
				case TEST_DXGI_VERSION_1_0:
				default:
					DXGI_SWAP_CHAIN_DESC scDesc;
					scDesc.BufferCount = m_RenderTargetOptions.SwapChainDesc.BufferCount;
					scDesc.BufferDesc.Format = m_RenderTargetOptions.SwapChainDesc.Format;
					scDesc.BufferDesc.Height = m_RenderTargetOptions.SwapChainDesc.Height;
					scDesc.BufferDesc.Width = m_RenderTargetOptions.SwapChainDesc.Width;
					scDesc.BufferUsage = m_RenderTargetOptions.SwapChainDesc.BufferUsage;
					scDesc.OutputWindow =  m_RenderTargetOptions.OutputWindow;
					scDesc.SampleDesc.Count = m_RenderTargetOptions.SwapChainDesc.SampleDesc.Count;
					scDesc.SampleDesc.Quality = m_RenderTargetOptions.SwapChainDesc.SampleDesc.Quality;
					scDesc.Windowed = m_RenderTargetOptions.Windowed;
					scDesc.SwapEffect = m_RenderTargetOptions.SwapChainDesc.SwapEffect;
					scDesc.Flags = m_RenderTargetOptions.SwapChainDesc.Flags;

                    // On phone if we are using ref/warp, change test settings to match hardware output settings.
	                if(GetFramework()->IsMobile() && 
                       (D3D10_DRIVER_TYPE_HARDWARE == m_D3DOptions.DriverType))
	                {
			            IDXGIOutput* pOutput = nullptr;
			            hr = m_pAdapters[j]->EnumOutputs(0, &pOutput);
			            if (FAILED(hr))
			            {
	    					WriteToLog( "SetupRenderTargets:  EnumOutputs failed." );
				            CleanupRenderTargets();
				            SAFE_RELEASE(pOutput);
				            return RESULT_FAIL;
			            }

                        DXGI_OUTPUT_DESC desc;
                        hr = pOutput->GetDesc(&desc);
				        SAFE_RELEASE(pOutput);
			            if (FAILED(hr))
			            {
	    					WriteToLog( "SetupRenderTargets:  GetDesc failed." );
				            CleanupRenderTargets();
				            return RESULT_FAIL;
			            }

					    scDesc.BufferDesc.Height = desc.DesktopCoordinates.bottom - desc.DesktopCoordinates.top;
					    scDesc.BufferDesc.Width = desc.DesktopCoordinates.right - desc.DesktopCoordinates.left;
                    }

					assert( m_pSwapChains[ i * m_uDeviceCount + j ].pSwapChain == nullptr );
					hr = m_DXGIFactories[ j ].pFactory1->CreateSwapChain( m_pD3DDevices->GetSubDevice( j )->GetIUnknown(), &scDesc, &m_pSwapChains[ i*m_uDeviceCount + j ].pSwapChain );
					break;
				}
				if( FAILED( hr ) )
				{
					WriteError( _T("SetupRenderTargets - CreateSwapChain() failed on %dth swap chain. Result code %s"), i*m_uDeviceCount + j, D3DHResultToString( hr ).c_str() );
					CleanupRenderTargets();
					return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
				}
				
				// Set the fullscreen state on the swap chain
				// BUGBUG - How do we select separate outputs for separate swap chains?
				m_pSwapChains[ i*m_uDeviceCount + j ].pSwapChain->SetFullscreenState( bFullScreen, bFullScreen?m_pAdapterOutputs[j]:nullptr );
			}

			if( !m_RenderTargetOptions.NoRenderTargets && createRTFromSwapChains )
			{
				assert( m_pRTTextures[ i ] == nullptr );
				if( FAILED( m_pD3DDevices->CreateTexture2DFromSwapChain( &(m_pSwapChains[i*m_uDeviceCount].pSwapChain) , &m_pRTTextures[ i ] )))		
				{
					WriteError( _T( "SetupRenderTargets - CreateTexture2DFromSwapChain() failed on %dth swap chains" ), i*m_uDeviceCount );
					CleanupRenderTargets();
					return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
				}
			
				// Get the render target view from the render target texture
				D3D_RENDER_TARGET_VIEW_DESC *pRTViewDesc = m_RenderTargetOptions.UseDefaultRenderTargetView ? nullptr : &m_RenderTargetOptions.RenderTargetViewDesc;

				assert( m_pRTViews[ i ] == nullptr );
				if( FAILED( m_pD3DDevices->CreateRenderTargetView( m_pRTTextures[ i ], pRTViewDesc, &m_pRTViews[ i ] ) ) )
				{
					WriteToLog( _T( "SetupRenderTargets - CreateRenderTargetView() failed to get view from render target textures." ) );
					CleanupRenderTargets();
					return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
				}
			}
		}
	}
	
	// Create *single* render target texture (per device)
	if( !m_RenderTargetOptions.NoRenderTargets && !createRTFromSwapChains )
	{
		switch( m_RenderTargetOptions.RenderTargetViewDesc.ViewDimension )
		{
			case D3D_RTV_DIMENSION_BUFFER:
				assert( m_pRTBuffers == nullptr );
				m_pRTBuffers = new CBuffer *[1];
				m_pRTBuffers[0] = nullptr;

				if( FAILED( m_pD3DDevices->CreateBuffer( &m_RenderTargetOptions.RenderTargetBufferDesc, nullptr, &m_pRTBuffers[0] ) ) )
				{
					WriteError( _T( "CreateBuffer() failed unexpectedly creating render target buffer" ) );
					CleanupRenderTargets();
					return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
				}
				break;
				
			case D3D_RTV_DIMENSION_TEXTURE1D:
			case D3D_RTV_DIMENSION_TEXTURE1DARRAY:
				assert( m_pRTTextures1D == nullptr );
				m_pRTTextures1D = new CTexture1D *[1];
				m_pRTTextures1D[0] = nullptr;

				assert( m_RenderTargetOptions.RenderTargetTextureDesc1D.Format != DXGI_FORMAT_UNKNOWN );
				if( FAILED( m_pD3DDevices->CreateTexture1D( &m_RenderTargetOptions.RenderTargetTextureDesc1D, nullptr, &m_pRTTextures1D[0] ) ) )
				{
					WriteError( _T( "CreateTexture1D() failed unexpectedly creating render target texture1d" ) );
					CleanupRenderTargets();
					return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
				}
				break;

			case D3D_RTV_DIMENSION_TEXTURE2D:
			case D3D_RTV_DIMENSION_TEXTURE2DARRAY:
			case D3D_RTV_DIMENSION_TEXTURE2DMS:
			case D3D_RTV_DIMENSION_TEXTURE2DMSARRAY:
				assert( m_RenderTargetOptions.RenderTargetTextureDesc.Format != DXGI_FORMAT_UNKNOWN );

				assert( m_pRTTextures == nullptr );
				m_pRTTextures = new CTexture2D *[1];
				m_pRTTextures[0] = nullptr;

				if( FAILED( m_pD3DDevices->CreateTexture2D( &m_RenderTargetOptions.RenderTargetTextureDesc, nullptr, &m_pRTTextures[0] ) ) )
				{
					WriteError( _T( "CreateTexture2D() failed unexpectedly creating render target texture2d" ) );
					CleanupRenderTargets();
					return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
				}
				break;
			default:
				assert( !"Must give a valid ViewDimension" );
				return RESULT_SKIP;
				break;
		}
		
		// Get the render target view from the render target texture
		D3D_RENDER_TARGET_VIEW_DESC *pRTViewDesc = m_RenderTargetOptions.UseDefaultRenderTargetView ? nullptr : &m_RenderTargetOptions.RenderTargetViewDesc;

		assert( m_pRTViews[ 0 ] == nullptr );
		if( FAILED( m_pD3DDevices->CreateRenderTargetView( m_pRTTextures[ 0 ], pRTViewDesc, &m_pRTViews[ 0 ] ) ) )
		{
			WriteToLog( _T( "SetupRenderTargets - CreateRenderTargetView() failed to get view from render target textures." ) );
			CleanupRenderTargets();
			return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
		}
	}
	
	// Create depth/stencil textures and views
	if( m_RenderTargetOptions.NoDepthStencils || 
		( !m_RenderTargetOptions.NoRenderTargets && m_RenderTargetOptions.RenderTargetViewDesc.ViewDimension == D3D_RTV_DIMENSION_BUFFER ) )
	{
		m_pDepthStencils = nullptr;
		m_pDSViews = nullptr;
	}
	else
	{
		switch( m_RenderTargetOptions.DepthStencilViewDesc.ViewDimension )
		{
			case D3D_DSV_DIMENSION_TEXTURE1D:
				assert( m_pDepthStencils1D == nullptr );
				if( FAILED( m_pD3DDevices->CreateTexture1D( &m_RenderTargetOptions.DepthTextureDesc1D, nullptr, &m_pDepthStencils1D ) ) )
				{
					WriteToLog( _T( "SetupRenderTargets - Failed creating depth/stencil textures." ) );
					CleanupRenderTargets();
					return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
				}
				break;
			
			default:
			case D3D_DSV_DIMENSION_TEXTURE2D:
			case D3D_DSV_DIMENSION_TEXTURE2DMS:
			case D3D_DSV_DIMENSION_TEXTURE2DARRAY:
			case D3D_DSV_DIMENSION_TEXTURE2DMSARRAY:
				if (m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0)
					m_RenderTargetOptions.DepthTextureDesc.MipLevels = 1;

				assert( m_pDepthStencils == nullptr );
				if( FAILED( m_pD3DDevices->CreateTexture2D( &m_RenderTargetOptions.DepthTextureDesc, nullptr, &m_pDepthStencils ) ) )
				{
					WriteToLog( _T( "SetupRenderTargets - Failed creating depth/stencil textures." ) );
					CleanupRenderTargets();
					return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
				}
				break;
		}
				
		// Create the CDepthStencilView
		D3D_DEPTH_STENCIL_VIEW_DESC *pDSVDesc = m_RenderTargetOptions.UseDefaultDepthStencilView ? nullptr : &m_RenderTargetOptions.DepthStencilViewDesc;
		
		assert( m_pDSViews == nullptr );		
		if( FAILED( m_pD3DDevices->CreateDepthStencilView( m_pDepthStencils, pDSVDesc, &m_pDSViews ) ) )
		{
			WriteToLog( _T( "SetupRenderTargets - Unable to create view for depth stencil textures." ) );
			CleanupRenderTargets();
			return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
		}
	}
			
	m_pD3DDevices->OMSetRenderTargets( createRTFromSwapChains ? numRenderWindowsPerDevice : 1, m_pRTViews, m_pDSViews );

	// Set default viewports
	D3D_VIEWPORT viewports[] =
	{
		{ 
			0, 
			0, 
			m_RenderTargetOptions.SwapChainDesc.Width, 
			m_RenderTargetOptions.SwapChainDesc.Height, 
			0.f, 
			1.f
		}, 
	};

	m_pD3DDevices->RSSetViewports( 1, viewports );
	
	memcpy( &s_RenderTargetOptionsCached, &m_RenderTargetOptions, sizeof( m_RenderTargetOptions ) );
	m_bRenderTargetValid = true;
		
	return RESULT_PASS;
}

//--------------------------------------------------------------------

void CD3DTestFramework::CleanupRenderTargets()
{
	UINT uNumRTPerDevice = m_uDeviceCount ? (m_uSwapChainCount / m_uDeviceCount) : 0;

	if ( m_pD3DDevices ) m_pD3DDevices->OMSetRenderTargets( 0, nullptr, nullptr );
	for (int i = uNumRTPerDevice - 1; i >= 0; i--)
	{
		SAFE_RELEASE( m_pRTViews[ i ] );
		if( m_pRTTextures )
		{
			SAFE_RELEASE( m_pRTTextures[ i ] );
		}
	}
	
	if( m_pRTTextures1D )
	{
		SAFE_RELEASE( m_pRTTextures1D[ 0 ] );
	}

	if( m_pRTBuffers )
	{
		SAFE_RELEASE( m_pRTBuffers[ 0 ] );
	}

	SAFE_DELETE_ARRAY( m_pRTTextures );
	SAFE_DELETE_ARRAY( m_pRTTextures1D );
	SAFE_DELETE_ARRAY( m_pRTBuffers );
	SAFE_DELETE_ARRAY( m_pRTViews );
	
	for (int i = m_uSwapChainCount - 1; i >= 0; i--)
	{
		SAFE_RELEASE( m_pSwapChains[ i ].pSwapChain );
	}
	SAFE_DELETE_ARRAY( m_pSwapChains );
	m_pSwapChains = nullptr;
	m_uSwapChainCount = 0;

	SAFE_RELEASE( m_pDepthStencils );
	SAFE_RELEASE( m_pDepthStencils1D );
	SAFE_RELEASE( m_pDSViews );
	
	m_bRenderTargetValid = false;
}

//--------------------------------------------------------------------

TEST_RESULT CD3DTestFramework::SetupTest()
{
	if (m_D3DOptions.wVersion > m_D3DOptions.wMaxVersion || m_D3DOptions.wVersion < m_D3DOptions.wMinVersion)
	{
		WriteToLog("The requested version of DX to run against is not supported by this test");
		return RESULT_SKIP;
	}

	CD3DTest *pTest = dynamic_cast< CD3DTest *>( GetCurrentTest() );
	assert( pTest && "Test object class is not derived from CD3DTest" );

	m_bSetupCalled = false;
	m_bDXGIValid = false;

	TEST_RESULT tr = RefreshGraphics();
	
	return tr;
}

void CD3DTestFramework::PreAllocateTest()
{
	// Restore device options to global settings
	memcpy( &m_DXGIOptions, &m_DXGIOptionsGlobal, sizeof( m_DXGIOptions ) );
	memcpy( &m_D3DOptions, &m_D3DOptionsGlobal, sizeof( m_D3DOptions ) );
	memcpy( &m_RenderTargetOptions, &m_RenderTargetOptionsGlobal, sizeof( m_RenderTargetOptions ) );
	m_bDXGIValid = false; // Force recycle of all devices
}

void CD3DTestFramework::PreGenerateTestCase()
{
	// If graphics options have changed since PostCleanupTestCase() then this was done by the UI.
	// UI changes to options should be reflected to the global device options.  The best way to 
	// handle this is on a member-by-member basis.  For now I will only reflect registered option
	// variable targets.

    if( m_DXGIOptions.AdapterTypeSrc != m_DXGIOptionsGroup.AdapterTypeSrc )
		m_DXGIOptionsGlobal.AdapterTypeSrc = m_DXGIOptions.AdapterTypeSrc;

    if( m_DXGIOptions.AdapterTypeRef != m_DXGIOptionsGroup.AdapterTypeRef )
		m_DXGIOptionsGlobal.AdapterTypeRef = m_DXGIOptions.AdapterTypeRef;

    if( m_DXGIOptions.AdapterNumberSrc != m_DXGIOptionsGroup.AdapterNumberSrc )
		m_DXGIOptionsGlobal.AdapterNumberSrc = m_DXGIOptions.AdapterNumberSrc;

    if( m_DXGIOptions.AdapterNumberRef != m_DXGIOptionsGroup.AdapterNumberRef )
		m_DXGIOptionsGlobal.AdapterNumberRef = m_DXGIOptions.AdapterNumberRef;

	if( m_DXGIOptions.AdapterIDSrc != m_DXGIOptionsGroup.AdapterIDSrc )
		m_DXGIOptionsGlobal.AdapterIDSrc = m_DXGIOptions.AdapterIDSrc;

	if( m_DXGIOptions.AdapterIDRef != m_DXGIOptionsGroup.AdapterIDRef )
		m_DXGIOptionsGlobal.AdapterIDRef = m_DXGIOptions.AdapterIDRef;

	if( m_DXGIOptions.OutputIDSrc != m_DXGIOptionsGroup.OutputIDSrc )
		m_DXGIOptionsGlobal.OutputIDSrc = m_DXGIOptions.OutputIDSrc;

	if( m_DXGIOptions.OutputIDRef != m_DXGIOptionsGroup.OutputIDRef )
		m_DXGIOptionsGlobal.OutputIDRef = m_DXGIOptions.OutputIDRef;

	if( m_D3DOptions.Debug != m_D3DOptionsGroup.Debug )
		m_D3DOptionsGlobal.Debug = m_D3DOptions.Debug;

	if( m_D3DOptions.SingleThreaded != m_D3DOptionsGroup.SingleThreaded )
		m_D3DOptionsGlobal.SingleThreaded = m_D3DOptions.SingleThreaded;

    if( m_D3DOptions.PreventThreadingOptimizations != m_D3DOptionsGroup.PreventThreadingOptimizations )
		m_D3DOptionsGlobal.PreventThreadingOptimizations = m_D3DOptions.PreventThreadingOptimizations;

	if( m_D3DOptions.SrcDriverType != m_D3DOptionsGroup.SrcDriverType )
		m_D3DOptionsGlobal.SrcDriverType = m_D3DOptions.SrcDriverType;

	if( m_D3DOptions.RefDriverType != m_D3DOptionsGroup.RefDriverType )
		m_D3DOptionsGlobal.RefDriverType = m_D3DOptions.RefDriverType;

	// It is possible for a test case to change one of the graphics
	// options.  If so it is a good idea to save the current
	// graphics options and restore them after test case cleanup
	// to avoid corrupting graphics options in other test groups.
	memcpy( &m_DXGIOptionsGroup, &m_DXGIOptions, sizeof( m_DXGIOptions ) );
	memcpy( &m_D3DOptionsGroup, &m_D3DOptions, sizeof( m_D3DOptions ) );
	memcpy( &m_RenderTargetOptionsGroup, &m_RenderTargetOptions, sizeof( m_RenderTargetOptions ) );
}

void CD3DTestFramework::PostCleanupTestCase()
{
	if ( (m_bLogDebug || (m_pD3DDevices && m_pD3DDevices->GetDeviceRemovedReason() != S_OK)) && m_pD3DInfoQueues)
	{
		CInfoQueue* pFirstInfoQueue = m_pD3DInfoQueues->GetSubInfoQueue(0);
		SIZE_T sizeBlob = 1024;
		SIZE_T sizeMsg = 0;
		const UINT64 numMessages = pFirstInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
		BYTE* blob = nullptr;
		D3D_MESSAGE *pMsg = nullptr;

		if (numMessages > 0)
		{
			WriteToLog("InfoQueue Messages:");
			blob = new BYTE[sizeBlob];
		}

		for (UINT64 i = 0; i < numMessages; i++)
		{
			//Get message size
			sizeMsg = 0;
			if (FAILED( pFirstInfoQueue->GetMessage( i, nullptr, &sizeMsg ) ) )
			{
				//         result = false;
				//goto exit;
			}

			if (sizeBlob < sizeMsg)
			{
				delete[] blob;
				blob = new BYTE[sizeMsg];
				sizeBlob = sizeMsg;
			}

			pMsg = (D3D_MESSAGE *)blob;
			if (FAILED( pFirstInfoQueue->GetMessage( i, pMsg, &sizeMsg ) ) )
			{
				goto exit;
			}

			tstring strCategory = ToString(pMsg->Category);
			tstring strSeverity = ToString(pMsg->Severity);
			tstring strID = ToString(pMsg->ID);

			WriteToLogF("Category=%s Severity=%s ID=%s Description='%s'",
				strCategory.c_str(), strSeverity.c_str(), strID.c_str(), pMsg->pDescription);
		}

exit:;
		delete[] blob;
	}

	// Restore saved graphics options
	memcpy( &m_DXGIOptions, &m_DXGIOptionsGroup, sizeof( m_DXGIOptions ) );
	memcpy( &m_D3DOptions, &m_D3DOptionsGroup, sizeof( m_D3DOptions ) );
	memcpy( &m_RenderTargetOptions, &m_RenderTargetOptionsGroup, sizeof( m_RenderTargetOptions ) );
}


//--------------------------------------------------------------------

TEST_RESULT CD3DTestFramework::PreSetupTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	
	if( !GetCurrentTest() )
		return RESULT_NONE;
	
	tr = RefreshGraphics();
	if (tr == RESULT_NONE)
		tr = RESULT_PASS;

	if (m_pD3DInfoQueues)
	{
		m_pD3DInfoQueues->ClearStoredMessages();
	}

	return tr;
}

//--------------------------------------------------------------------
bool CD3DTestFramework::UpdateGraphicsValidFlags()
{
	if( m_pD3DDevices && m_pD3DDevices->GetDeviceRemovedReason() != S_OK )
		m_bDXGIValid = false;

	if( memcmp( &s_DXGIOptionsCached, &m_DXGIOptions, sizeof( m_DXGIOptions ) ) != 0 )
		m_bDXGIValid = false;

	if( memcmp( &s_D3DOptionsCached, &m_D3DOptions, sizeof( m_D3DOptions ) ) != 0 )
		m_bD3DValid = false;

	if( memcmp( &s_RenderTargetOptionsCached, &m_RenderTargetOptions, sizeof( m_RenderTargetOptions ) ) != 0 )
		m_bRenderTargetValid = false;

	return !m_bDXGIValid || !m_bD3DValid || !m_bRenderTargetValid;
}

//--------------------------------------------------------------------

TEST_RESULT CD3DTestFramework::RefreshGraphics()
{
	TEST_RESULT tr = RESULT_NONE;
	m_RefreshGraphicsFlags = 0;

	if (UpdateGraphicsValidFlags() || !m_bSetupCalled)
	{
		CD3DTest *pTest = dynamic_cast< CD3DTest *>( GetCurrentTest() );
		assert( pTest );
		bool bInterfacesChanged = false;
		bool bCleanupCalled = false;

		// Check whether DXGI options have changed.
		if( !m_bDXGIValid )
		{
			// Call the test's Cleanup() function since we will be 
			// calling its Setup() function again
			if (m_bSetupCalled)
			{
				pTest->Cleanup();
				bCleanupCalled = true;
			}

			CleanupDXGI();
			tr = SetupDXGI();
			if( tr != RESULT_PASS )
				return tr;
			bInterfacesChanged = true;
			m_RefreshGraphicsFlags |= DXGI_RECYCLED;
		}

		// Check whether D3D settings have changed.
		if( !m_bD3DValid )
		{
			// Call the test's Cleanup() function since we will be 
			// calling its Setup() function again
			if (m_bSetupCalled && !bCleanupCalled)
			{
				pTest->Cleanup();
			}

			CleanupD3D();
			tr = SetupD3D();
			if( tr != RESULT_PASS )
				return tr;
			bInterfacesChanged = true;
			m_RefreshGraphicsFlags |= D3D_RECYCLED;
		}

		// Query the adapter & factory off the device to make sure they match
		for (UINT i = 0; i < m_uDeviceCount; ++i)
		{
			if (!m_pD3DDevices)
				break;
			DXGI_ADAPTER_DESC aid;
			IDXGIAdapter* pDeviceAdapter = nullptr;
			HRESULT hr = E_FAIL;
			{
				IDXGIDevice* pDXGIDevice = nullptr;
				CDevice* pSubDevice = m_pD3DDevices->GetSubDevice(i);
				if (!pSubDevice)
					pSubDevice = m_pD3DDevices;
				IUnknown* pUnknownDevice = pSubDevice->GetIUnknown();
				hr = pUnknownDevice->QueryInterface( __uuidof(IDXGIDevice), (void**)(&pDXGIDevice) );
				if (SUCCEEDED(hr) && pDXGIDevice)
				{
					hr = pDXGIDevice->GetAdapter(&pDeviceAdapter);
					if (SUCCEEDED(hr) && pDeviceAdapter)
					{
						DXGI_ADAPTER_DESC adapterDesc;
						hr = pDeviceAdapter->GetDesc(&aid);

						if (SUCCEEDED(hr))
						{
							// Get the DXGI Factory from the Adapter
							SAFE_RELEASE(m_DXGIFactories[ i ].pFactory);
							hr = pDeviceAdapter->GetParent(__uuidof( IDXGIFactory ), (void**)&m_DXGIFactories[ i ].pFactory );

							if (SUCCEEDED(hr))
							{
								// We should use the queried factory & adapter from now on
								pDeviceAdapter->AddRef();
								SAFE_RELEASE( m_pAdapters[ i ] );
								m_pAdapters[ i ] = pDeviceAdapter;

								//
								// Find the adapter index.
								//

								IDXGIAdapter * pDXGIAdapter = nullptr;

								for ( UINT n = 0;
									m_DXGIFactories[ i ].pFactory->EnumAdapters( n, &pDXGIAdapter ) != DXGI_ERROR_NOT_FOUND;
									++n )
								{
									if ( m_pAdapters[ i ] == pDXGIAdapter )
									{
										m_DXGIOptions.AdapterID[ i ] = n;

										SAFE_RELEASE( pDXGIAdapter );
										break;
									}

									SAFE_RELEASE( pDXGIAdapter );
								};


								//
								// Get the appropriate output from the adapter
								//


                                				if (!IsSession0())
                                				{
    									SAFE_RELEASE( m_pAdapterOutputs[i] );
	    								SetupFullscreenOutput( i, m_pAdapters[i], &m_pAdapterOutputs[i] );
                                				}
							}
						}
					}
				}
				SAFE_RELEASE(pDXGIDevice);
			}
			if (FAILED(hr) || !pDeviceAdapter)
			{
				SAFE_RELEASE(pDeviceAdapter);
				WriteToLogF("GetAdapter failed on device %d. hr=%s", i, D3DHResultToString(hr).c_str() );
				continue;
			}

			tstring sTitle, sDescription;
			sTitle = _T("D3D ");
			sTitle += ToString(m_D3DOptions.FeatureLevel);
			sTitle += _T(" ");
			sTitle += ToString( m_D3DOptions.SrcDriverType );

			IDXGIAdapter1* pAdapter1 = nullptr;
			hr = pDeviceAdapter->QueryInterface(__uuidof(IDXGIAdapter1), (void**)&pAdapter1);
			if (FAILED(hr) || pAdapter1 == nullptr)
			{
				WriteError( "RefreshGraphics:  Failed to QueryInterface for IDXGIAdapter1" );
				continue;
			}

			DevX::GRFX::AdapterDescriptorCore* pAdapterDescriptor = DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor(pAdapter1);
			if (!pAdapterDescriptor)
			{
				WriteError( "RefreshGraphics:  Failed to call DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor" );
				SAFE_RELEASE(pAdapter1);
				continue;
			}

			// Get UMD driver version
			D3DKMT_UMD_DRIVER_VERSION umdDriverVersion;
			ZeroMemory(&umdDriverVersion, sizeof(umdDriverVersion));
			pAdapterDescriptor->GetDisplayDriverVersionUMD(umdDriverVersion);
			TCHAR szDriverVersion[32] = { 0 };
			PrintDriverVersion(szDriverVersion, umdDriverVersion.DriverVersion);

			SAFE_DELETE(pAdapterDescriptor);
			SAFE_RELEASE(pAdapter1);

			{
				size_t size = wcslen( aid.Description );
				if( size )
				{
					size++;
					TCHAR * szDriverDescription = new TCHAR[size];
					CopyString( szDriverDescription, aid.Description, size );
					sDescription = szDriverDescription;
					delete[] szDriverDescription;
				}
				else
					sDescription = _T("Unknown Adapter");
			}

			sDescription += _T(" (");
			sDescription += szDriverVersion;
			sDescription += _T(", DDI ");
			sDescription += ToString(10); // BUGBUG - TODO: CWGFTestFramework::GetD3DDDI()
			sDescription += _T(")");
			GetShell()->SetDeviceTitle( i, sTitle.c_str(), sDescription.c_str() );

			SAFE_RELEASE(pDeviceAdapter);
		}

		if( !m_bRenderTargetValid )
		{
			CleanupRenderTargets();
			tr = SetupRenderTargets();
			if( tr != RESULT_PASS )
				return tr;
			bInterfacesChanged = true;
			m_RefreshGraphicsFlags |= RT_RECYCLED;
		}

		// Check whether D3D settings have changed.
		if (bInterfacesChanged || !m_bSetupCalled)
		{
			// Setup test	
			tr = pTest->Setup();
			
			// If test setup fails, abort the test
			if (tr == RESULT_FAIL)
			{
				tr = RESULT_ABORT;
			}

			// BUGBUG - NEED ASSERT THAT CACHED OPTIONS MATCH CURRENT OPTIONS...

			// In the event that setup made changes to any device options
			// reflect current options into stored group options.
			memcpy( &m_DXGIOptionsGroup, &m_DXGIOptions, sizeof( m_DXGIOptions ) );
			memcpy( &m_D3DOptionsGroup, &m_D3DOptions, sizeof( m_D3DOptions ) );
			memcpy( &m_RenderTargetOptionsGroup, &m_RenderTargetOptions, sizeof( m_RenderTargetOptions ) );

			m_bSetupCalled = true;
		}
	}

	return tr;
}

//--------------------------------------------------------------------

WORD GetLatestRuntimeVersion()
{
	//TODO: this should probably be done using windows versions

	HMODULE hMod = nullptr;
	if ( hMod = LoadLibraryExW( L"d3d10_1.dll", nullptr, 0 ) )
	{
		FreeLibrary(hMod);
		hMod = nullptr;

		return 0x1010;
	}
	else if ( hMod = LoadLibraryExW( L"d3d10.dll", nullptr, 0 ) )
	{
		FreeLibrary(hMod);
		hMod = nullptr;

		return 0x1000;
	}
	else
	{
		return 0x0910;
	}
}

//--------------------------------------------------------------------

bool CD3DTestFramework::UpdateCurrentDeviceInfo( DXGLOG_DEVICEINFO *pDeviceInfo )
{
	
	DXGI_ADAPTER_DESC aid;
	
	if( m_pAdapters == nullptr )
	{
		if( SetupDXGI() != RESULT_PASS )
			return false;
	}
	
	if( m_pAdapters[0] == nullptr )
		return false;
		
	m_pAdapters[ 0 ]->GetDesc( &aid );
	memset( pDeviceInfo, 0, sizeof( DXGLOG_DEVICEINFO ) );
	
	// BUGBUG - This string may need to contain more information.
	tstring device = ToString( m_D3DOptions.SrcDriverType );
	const TCHAR *sDevice = device.c_str();
	
	// BUGBUG - This is vomit!  I should not have to know
	// that pDeviceInfo->sDevice needs to be allocated
	// off the heap using new().  DXGLOG_DEVICEINFO should be
	// a class with accessor functions which allocate these for
	// me, even handling conversion from one string type to another.
	size_t size = strlen( sDevice );
	if( size )
	{
		size++;
		pDeviceInfo->sDevice = new TCHAR[size];
		CopyString( pDeviceInfo->sDevice, sDevice, size );
	}
		
	size = wcslen( aid.Description );
	if( size )
	{
		size++;
		pDeviceInfo->sDisplay = new TCHAR[size];
		CopyString( pDeviceInfo->sDisplay, aid.Description, size );
	}

	IDXGIAdapter1* pAdapter1 = nullptr;
	HRESULT hr = m_pAdapters[ 0 ]->QueryInterface(__uuidof(IDXGIAdapter1), (void**)&pAdapter1);
	if (FAILED(hr) || pAdapter1 == nullptr)
	{
		WriteError( "UpdateCurrentDeviceInfo:  Failed to QueryInterface for IDXGIAdapter1" );
		return false;
	}

	DevX::GRFX::AdapterDescriptorCore* pAdapterDescriptor = DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor(pAdapter1);
	if (!pAdapterDescriptor)
	{
		WriteError( "UpdateCurrentDeviceInfo:  Failed to call DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor" );
		SAFE_RELEASE(pAdapter1);
		return false;
	}

	// Get UMD driver version
	D3DKMT_UMD_DRIVER_VERSION umdDriverVersion;
	ZeroMemory(&umdDriverVersion, sizeof(umdDriverVersion));
	pAdapterDescriptor->GetDisplayDriverVersionUMD(umdDriverVersion);
	pDeviceInfo->sDriverVersion = new TCHAR[32];
	PrintDriverVersion(pDeviceInfo->sDriverVersion, umdDriverVersion.DriverVersion);

	SAFE_DELETE(pAdapterDescriptor);
	SAFE_RELEASE(pAdapter1);

	// create PnP ID string
	char szBuffer[56] = "";
	sprintf_s(szBuffer, ARRAYSIZE(szBuffer), _T("VEN_%04X&DEV_%04X&SUBSYS_%08X&REV_%02X"), aid.VendorId, aid.DeviceId, aid.SubSysId, aid.Revision);

	size = strlen( szBuffer );
	if( size )
	{
		size++;
		pDeviceInfo->sPnPID = new TCHAR[size];
		CopyString( pDeviceInfo->sPnPID, szBuffer, size );
	}

	//Get the current display settings
	DISPLAY_DEVICEW	DisplayDevice;
	DEVMODEW	    DevMode;

	DisplayDevice.cb = sizeof(DISPLAY_DEVICEW);

	bool bFoundPrimaryDisplayDevice = false;
	for
	(
		UINT iDevNum = 0;
		iDevNum < 10;
		++iDevNum
	)
	{
		if (!EnumDisplayDevicesW(nullptr, iDevNum, &DisplayDevice, 0))
		{
			continue;
		};

		if ((DisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) == 0)
		{
			continue;
		};

		DevMode.dmSize = sizeof(DEVMODE);
		DevMode.dmSpecVersion = DM_SPECVERSION;
		if (EnumDisplaySettingsW(DisplayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &DevMode))
		{
			bFoundPrimaryDisplayDevice = true;

			switch (DevMode.dmBitsPerPel)
			{
				case 16:
					sprintf_s(szBuffer, ARRAYSIZE(szBuffer), _T("%dx%dxR5G6B5 @ %dhz"), DevMode.dmPelsWidth, DevMode.dmPelsHeight, DevMode.dmDisplayFrequency);
					break;
				case 32:
					sprintf_s(szBuffer, ARRAYSIZE(szBuffer), _T("%dx%dxX8R8G8B8 @ %dhz"), DevMode.dmPelsWidth, DevMode.dmPelsHeight, DevMode.dmDisplayFrequency);
					break;
				default:
					sprintf_s(szBuffer, ARRAYSIZE(szBuffer), _T("%dx%dxUNKNOWN @ %dhz"), DevMode.dmPelsWidth, DevMode.dmPelsHeight, DevMode.dmDisplayFrequency);
					break;
			}

			// create Display Mode string
			size = strlen( szBuffer );
			if( size )
			{
				size++;
				pDeviceInfo->sResolution = new TCHAR[size];
				CopyString( pDeviceInfo->sResolution, szBuffer, size );
			}

			break;
		}
		else
		{
			WriteError( "UpdateCurrentDeviceInfo:  EnumDisplaySettings failed." );
		}
	}

	if (bFoundPrimaryDisplayDevice == false)
	{
		WriteError( "UpdateCurrentDeviceInfo:  Failed to locate primary display device with EnumDisplayDevices." );
	}
	

	pDeviceInfo->fMaxDXVersion =  D3DVersionToFloat( m_D3DOptions.wMaxVersion ); // BUGBUG - TODO: CWGFTestFramework::GetMaxDXVersion()
	pDeviceInfo->fMinDXVersion =  D3DVersionToFloat( m_D3DOptions.wMinVersion ); // BUGBUG - TODO: CWGFTestFramework::GetMinDXVersion()
	pDeviceInfo->bPSGP = false; // BUGBUG - This may get supported in later versions of D3D.
	pDeviceInfo->bDriverDebug = m_D3DOptions.Debug; // BUGBUG - What determines this?
	pDeviceInfo->dwVendorID = aid.VendorId;
	pDeviceInfo->dwDeviceID = aid.DeviceId;
	pDeviceInfo->nDDI = 10; // BUGBUG - TODO: CWGFTestFramework::GetD3DDDI()
	pDeviceInfo->fVersion = D3DVersionToFloat( m_D3DOptions.wVersion );
	pDeviceInfo->fBuild = CWGFTestFramework::GetSDKBuild();
	pDeviceInfo->bDebug = m_D3DOptions.Debug;
	pDeviceInfo->fInterface = D3DVersionToFloat( m_D3DOptions.wVersion ); // BUGBUG - TODO: CWGFTestFramework::GetD3DInterface()
	pDeviceInfo->wDesktopBPP = (WORD)DevMode.dmBitsPerPel; 
	
	
	// BUGBUG - TODO: Finish this component...
	
	return true;
}

//--------------------------------------------------------------------

void DX10_0OptionCallback( const bool &bDX10_0 )
{
	if (!bDX10_0)
		return;

	CD3DTestFramework *pFramework = dynamic_cast< CD3DTestFramework *>( GetFramework() );
	pFramework->m_D3DOptions.wVersion = 0x1000;
	pFramework->m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_10_0;
	pFramework->m_bForce10_0 = true;
}

void DX10_1OptionCallback( const bool &bDX10_1 )
{
	if (!bDX10_1)
		return;

	CD3DTestFramework *pFramework = dynamic_cast< CD3DTestFramework *>( GetFramework() );
	pFramework->m_D3DOptions.wVersion = 0x1010;
	pFramework->m_bForce10_1 = true;
}

void DX10LatestOptionCallback( const bool &bDX10Latest )
{
	if (!bDX10Latest)
		return;

	CD3DTestFramework *pFramework = dynamic_cast< CD3DTestFramework *>( GetFramework() );
	pFramework->m_D3DOptions.wVersion = 0xffff;
}

void DX11_0OptionCallback( const bool &bDX11_0 )
{
	if (!bDX11_0)
		return;

	CD3DTestFramework *pFramework = dynamic_cast< CD3DTestFramework *>( GetFramework() );
	pFramework->m_D3DOptions.wVersion = 0x1100;
}

void FeatureLevelCallback( const tstring &FeatureLevel )
{
    CD3DTestFramework *pFramework = dynamic_cast< CD3DTestFramework *>( GetFramework() );
	if (pFramework->m_D3DOptions.wVersion < 0x1010)
		pFramework->m_D3DOptions.wVersion = 0x1010;
    if      ( FeatureLevel == _T("9.1") ) pFramework->m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_9_1;
    else if ( FeatureLevel == _T("9.2") ) pFramework->m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_9_2;
    else if ( FeatureLevel == _T("9.3") ) pFramework->m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_9_3;
    else if ( FeatureLevel == _T("10.0")) pFramework->m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_10_0;
    else if ( FeatureLevel == _T("10.1")) pFramework->m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_10_1;
    else pFramework->m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_10_1;
    pFramework->m_bForceFeatureLevel = true;
}

void OrigFeatureLevelCallback( const tstring &FeatureLevel )
{
    CD3DTestFramework *pFramework = dynamic_cast< CD3DTestFramework *>( GetFramework() );
    if      ( FeatureLevel == _T("9.1") ) pFramework->m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_9_1;
    else if ( FeatureLevel == _T("9.2") ) pFramework->m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_9_2;
    else if ( FeatureLevel == _T("9.3") ) pFramework->m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_9_3;
    else if ( FeatureLevel == _T("10.0")) pFramework->m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_10_0;
    else if ( FeatureLevel == _T("10.1")) pFramework->m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_10_1;
	else if ( FeatureLevel == _T("0"))    pFramework->m_D3DOptions.origFeatureLevel = (D3D_FEATURE_LEVEL) 0;
    else pFramework->m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_10_1;
    pFramework->m_bForceFeatureLevel = true;
}

void RasterizerDLL_OptionCallback( const tstring &Rasterizer )
{
    CD3DTestFramework *pFramework = dynamic_cast< CD3DTestFramework *>( GetFramework() );
#if defined(UNICODE)
	StringCchCopyW
	(
		pFramework->m_D3DOptions.SoftwareRasterizerDLLName,
		ARRAYSIZE( pFramework->m_D3DOptions.SoftwareRasterizerDLLName ),
		Rasterizer.c_str()
	);
#else
	MultiByteToWideChar
	(
		CP_ACP,
		MB_ERR_INVALID_CHARS,
		Rasterizer.c_str(),
		-1,
		pFramework->m_D3DOptions.SoftwareRasterizerDLLName,
		ARRAYSIZE( pFramework->m_D3DOptions.SoftwareRasterizerDLLName )
	);
#endif
}

//--------------------------------------------------------------------

void CD3DTestFramework::InitOptionVariables()
{
	CGraphicsTestFramework::InitOptionVariables();

	int id = AddOptionCategory( "D3D" );

	RegisterOptionVariable( "Src", "Source driver type", &m_D3DOptions.SrcDriverType, D3D_DRIVER_TYPE_HARDWARE, 0, id, _T( "-src:[REF|HW|SW|NULL|WARP]" ) );
	RegisterOptionVariable( "Ref", "Reference driver type", &m_D3DOptions.RefDriverType, D3D_DRIVER_TYPE_REFERENCE, 0, id, _T( "-ref:[REF|HW|SW|NULL|WARP]" ) );
	RegisterOptionVariable( "SrcOnly", "Only use Source device", &m_D3DOptions.SrcOnly, false, 0, id );

	RegisterOptionVariable<bool>( "DX10.0", "Use D3D10.0 runtime", DX10_0OptionCallback, false, OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable<bool>( "DX10.1", "Use D3D10.1 runtime", DX10_1OptionCallback, false, OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable<bool>( "DX10", "Use latest D3D10 runtime", DX10LatestOptionCallback, false, OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable<bool>( "DX11.0", "Use D3D11.0 runtime", DX11_0OptionCallback, false, OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable<tstring>( "FeatureLevel", "Specify D3D feature Level:  9.1, 9.2, 9.3, 10.0, 10.1 (default), 9.8 == 9.ALL"
        " Forces -DX10.1:true.", FeatureLevelCallback, tstring(_T("10.1")), OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable( "StrictValidation", "Enable Stricter Validation Layer", &m_D3DOptions.bStricterValidationLayer, false, 0, id );

	RegisterOptionVariable<D3DTEST_INTERFACE_LEVEL>( _T( "OrigD3DInterfaceLevel" ), _T( "Original device creation interface level (default is 10.0)" ), &m_D3DOptions.origInterfaceLevel, D3DTEST_INTERFACE_LEVEL_10_0, OPTION_VALUE_CONSTANT, id );
	/*BUGBUG - delete after changes RI to WinMain -->*/ RegisterOptionVariable<tstring>( _T( "OrigD3DFeatureLevel" ), _T( "Original device creation feature level when origInterfaceLevel is different from target interface level (default is target feature level)" ), &OrigFeatureLevelCallback, tstring(_T("0")), OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable<tstring>( _T( "OrigFeatureLevel" ), _T( "Original device creation feature level when origInterfaceLevel is different from target interface level (default is target feature level)" ), &OrigFeatureLevelCallback, tstring(_T("0")), OPTION_VALUE_CONSTANT, id );

	RegisterOptionVariable( "SubPixelPrecision", "Specify sub-pixel precision; defaults to 8", &m_iSubPixelPrecision, 8, OPTION_VALUE_HIDDEN);
	RegisterOptionVariable( "WARP", "Enable WARP-specific tests", &m_bWARP, false, OPTION_VALUE_HIDDEN);
#if defined(_X86_)
	RegisterOptionVariable<bool>( "ReserveLowerMemory", "Reserve but don't commit lower half of virtual memory before running tests. For 32-bit test binaries on 64-bit OSs only.", &m_bReserveLowerMemory, false);
	RegisterOptionVariable<bool>( "CommitLowerMemory", "Reserve and commit lower half of virtual memory before running tests. For 32-bit test binaries on 64-bit OSs only.", &m_bCommitLowerMemory, false);
#endif

	RegisterOptionVariable<bool>( "GenerateGolden", "Generate new golden image; default is false, meaning use existing golden image", &m_D3DOptions.generateGolden, false, OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable<UINT32>( "QueryTimeout", "Specify time for query to timeout in milliseconds", &m_nQueryTimeout, 5000, 0, id, "-QueryTimeout:#" );
	m_bForceFeatureLevel = false;
}

//--------------------------------------------------------------------

bool CD3DTestFramework::Present(RECT *pSrc, RECT *pDest, void *pSourceDirtyRegion, void *pDestClipRegion, UINT SyncInterval, UINT dwFlags)
{
	HRESULT hr = S_OK;
	bool result = true;

	for (int i = m_uSwapChainCount - 1; i >= 0; i--)
	{
		if (FAILED( hr = m_pSwapChains[ i ].pSwapChain->Present(SyncInterval, dwFlags)))
		{
			result = false;
			assert( m_uDeviceCount > 0 );
			UINT uNumSWPerDevice = m_uSwapChainCount / m_uDeviceCount;
			WriteToLogF
			(
				_T( "Present:  Present on device %d and swap chain %d failed with error %s." ),
				i % uNumSWPerDevice,
				i / uNumSWPerDevice,
				D3DHResultToString( hr ).c_str()
			);
		}
	}

	return result;
}

//--------------------------------------------------------------------

NRESULT CD3DTestFramework::CompileShaderFromFile( LPCWSTR pFile, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, CBlob** ppShader, CBlob** ppError )
{
	NRESULT nr( S_OK, 2 );
	CBlob *pShader = nullptr;
	CBlob *pError = nullptr;

	LPCSTR szNewProfile = GetShaderProfile(pProfile, m_D3DOptions);

	for( UINT i = 0; i < m_uDeviceCount; ++i )
	{
		ID3D10Blob *pShader10;
		ID3D10Blob *pError10;

        HRESULT hr = D3DCompileFromFile(pFile, nullptr, nullptr, pFunctionName, szNewProfile,
            ConvertShaderFlagsTo10(Flags), 0, &pShader10, &pError10);

        nr[i] = hr;

		if( pShader10 )
		{
			if( pShader == nullptr )
			{
				pShader = new CMultiBlob( (int) m_uDeviceCount );
				pShader->AddRef();
			}
			pShader->SetSubBlob( i, new C10Blob( pShader10 ) );
		}

		if( pError10 )
		{
			if( pError == nullptr )
			{
				pError = new CMultiBlob( (int) m_uDeviceCount );
				pError->AddRef();
			}
			pError->SetSubBlob( i, new C10Blob( pError10 ) );
		}
	}

	*ppShader = pShader;
	if (ppError != nullptr)
		*ppError = pError;
	else
	if( pError )
		pError->Release();

	return nr;
}


//--------------------------------------------------------------------

NRESULT CD3DTestFramework::CompileShaderFromFile( LPCSTR pFile, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, CBlob** ppShader, CBlob** ppError )
{
	NRESULT nr( S_OK, 2 );
	CBlob *pShader = nullptr;
	CBlob *pError = nullptr;

	LPCSTR szNewProfile = GetShaderProfile(pProfile, m_D3DOptions);

	for( UINT i = 0; i < m_uDeviceCount; ++i )
	{
		ID3D10Blob *pShader10;
		ID3D10Blob *pError10;

        HRESULT hr = D3DX10CompileFromFile(pFile, nullptr, nullptr, pFunctionName, szNewProfile, ConvertShaderFlagsTo10(Flags), NULL, NULL, &pShader10, &pError10, NULL);
		nr[i] = hr;

		if( pShader10 )
		{
			if( pShader == nullptr )
			{
				pShader = new CMultiBlob( (int) m_uDeviceCount );
				pShader->AddRef();
			}
			pShader->SetSubBlob( i, new C10Blob( pShader10 ) );
		}

		if( pError10 )
		{
			if( pError == nullptr )
			{
				pError = new CMultiBlob( (int) m_uDeviceCount );
				pError->AddRef();
			}
			pError->SetSubBlob( i, new C10Blob( pError10 ) );
		}
	}

	*ppShader = pShader;
	if (ppError != nullptr)
		*ppError = pError;
	else
	if( pError )
		pError->Release();

	return nr;
}


NRESULT CD3DTestFramework::CompileShaderFromResource( HMODULE hModule, LPCWSTR pResource, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, CBlob **ppShader, CBlob **ppError, SHADER_MACRO const* pMacros )
{
	NRESULT nr( S_OK, 2 );
	CBlob *pShader = nullptr;
	CBlob *pError = nullptr;

	LPCSTR szNewProfile = GetShaderProfile(pProfile, m_D3DOptions);

	for( UINT i = 0; i < m_uDeviceCount; ++i )
	{
		ID3D10Blob *pShader10;
		ID3D10Blob *pError10;

        // Find the resource
        HRSRC resource = FindResourceExW(hModule, (LPWSTR)RT_RCDATA, (LPCWSTR)pResource, 0);
        if (!resource)
        {
            nr[i] = HRESULT_FROM_WIN32(GetLastError());
            continue;
        }

        size_t cbShader = SizeofResource(hModule, resource);

        // Load the resource
        HGLOBAL hglob = LoadResource(hModule, resource);
        if (!hglob)
        {
            nr[i] = HRESULT_FROM_WIN32(GetLastError());
            continue;
        }

        LPCSTR shader = (LPCSTR)LockResource(hglob);
        if (!shader)
        {
            nr[i] = HRESULT_FROM_WIN32(GetLastError());
            continue;
        }
        // Compile it!
        nr[i] = D3DCompile(shader, cbShader, nullptr, nullptr, nullptr,
            pFunctionName, szNewProfile, Flags, 0, &pShader10, &pError10);

		if( pShader10 )
		{
			if( pShader == nullptr )
			{
				pShader = new CMultiBlob( (int) m_uDeviceCount );
				pShader->AddRef();
			}
			pShader->SetSubBlob( i, new C10Blob( pShader10 ) );
		}

		if( pError10 )
		{
			if( pError == nullptr )
			{
				pError = new CMultiBlob( (int) m_uDeviceCount );
				pError->AddRef();
			}
			pError->SetSubBlob( i, new C10Blob( pError10 ) );
		}
	}

	*ppShader = pShader;
	if (ppError != nullptr)
		*ppError = pError;
	else
	if( pError )
		pError->Release();

	return nr;
}

//--------------------------------------------------------------------

NRESULT CD3DTestFramework::CompileShaderFromResource( HMODULE hModule, LPCSTR pResource, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, CBlob **ppShader, CBlob **ppError, SHADER_MACRO const* pMacros )
{
	NRESULT nr( S_OK, 2 );
	CBlob *pShader = nullptr;
	CBlob *pError = nullptr;

	LPCSTR szNewProfile = GetShaderProfile(pProfile, m_D3DOptions);

	for( UINT i = 0; i < m_uDeviceCount; ++i )
	{
		ID3D10Blob *pShader10 = nullptr;
		ID3D10Blob *pError10 = nullptr;

		HRESULT hr = S_OK;
            
        if(IsMobile())
        {
            Flags &= ~D3D_SHADER_DEBUG;
        }

        hr = D3DX10CompileFromResource(
                    hModule,
                    pResource,
                    NULL,
                    (D3D10_SHADER_MACRO const*)pMacros,
                    NULL,
                    pFunctionName,
                    szNewProfile,
                    ConvertShaderFlagsTo10( Flags ),
                    0,
                    NULL,
                    &pShader10, 
                    &pError10,
                    NULL);
		nr[i] = hr;

		if( pShader10 )
		{
			if( pShader == nullptr )
			{
				pShader = new CMultiBlob( (int) m_uDeviceCount );
				pShader->AddRef();
			}
			pShader->SetSubBlob( i, new C10Blob( pShader10 ) );
		}

		if( pError10 )
		{
			if( pError == nullptr )
			{
				pError = new CMultiBlob( (int) m_uDeviceCount );
				pError->AddRef();
			}
			pError->SetSubBlob( i, new C10Blob( pError10 ) );
		}
    }

	*ppShader = pShader;
	if (ppError != nullptr)
		*ppError = pError;
	else
	if( pError )
		pError->Release();

	return nr;
}


//--------------------------------------------------------------------

NRESULT CD3DTestFramework::CompileShaderFromMemory( LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, CBlob** ppShader, CBlob** ppError )
{
	NRESULT nr( S_OK, 2 );
	CBlob *pShader = nullptr;
	CBlob *pError = nullptr;

	LPCSTR szNewProfile = GetShaderProfile(pProfile, m_D3DOptions);

	for( UINT i = 0; i < m_uDeviceCount; ++i )
	{
		ID3D10Blob *pShader10 = nullptr;
		ID3D10Blob *pError10 = nullptr;

        nr[i] = D3DX10CompileFromMemory(
                        pSrcData,
                        SrcDataLen,
                        NULL,
                        NULL,
                        NULL,
                        pFunctionName,
                        szNewProfile,
                        Flags,
                        0,
                        NULL,
                        &pShader10,
                        &pError10,
                        &nr[i]);

		if( pShader10 )
		{
			if( pShader == nullptr )
			{
				pShader = new CMultiBlob( (int) m_uDeviceCount );
				pShader->AddRef();
			}
			pShader->SetSubBlob( i, new C10Blob( pShader10 ) );
		}

		if( pError10 )
		{
			if( pError == nullptr )
			{
				pError = new CMultiBlob( (int) m_uDeviceCount );
				pError->AddRef();
			}
			pError->SetSubBlob( i, new C10Blob( pError10 ) );
		}
	}

	*ppShader = pShader;
	if (ppError != nullptr)
		*ppError = pError;
	else
	if( pError )
		pError->Release();

	return nr;
}

//--------------------------------------------------------------------

NRESULT CD3DTestFramework::SaveTextureToMemory( CResource* pResource, CBlob** ppDestBuf )
{
	NRESULT nr( S_OK, 2 );

	(*ppDestBuf) = nullptr;

	for( int i = 0; i < pResource->GetSubObjectCount(); ++i )
	{
		ID3D10Blob *pDestBuf10 = nullptr;

		ID3D10Resource* pResource10 = nullptr;
		pResource->GetSubResource(i)->GetInterfacePtr(&pResource10);

		HRESULT hr = D3DXSaveTextureToMemory(pResource10, D3DX10_IFF_DDS, &pDestBuf10, 0);
		nr[i] = hr;

		if( pDestBuf10 )
		{
			if( (*ppDestBuf) == nullptr )
			{
				(*ppDestBuf) = new CMultiBlob( (int) m_uDeviceCount );
				(*ppDestBuf)->AddRef();
			}
			(*ppDestBuf)->SetSubBlob( i, new C10Blob( pDestBuf10 ) );
		}
	}

	return nr;
}

//--------------------------------------------------------------------

NRESULT CD3DTestFramework::CreateTextureFromMemory( CDevice* pDevice, 
												   CBlob* pSrcBuf, 
												   CResource** ppDestResource, 
												   DXGI_FORMAT newFormat, 
												   CResource* pClonedResource,
												   bool bStaging)
{
	NRESULT nr( S_OK, 2 );

	(*ppDestResource) = nullptr;

	for( int i = 0; i < pDevice->GetSubObjectCount(); ++i )
	{
		HRESULT hr = E_UNEXPECTED;
		ID3D10Device* pDevice10 = nullptr;
		pDevice->GetSubDevice(i)->GetInterfacePtr(&pDevice10);

		ID3D10Blob* pSrcBuf10 = nullptr;
		pSrcBuf->GetSubBlob(i)->GetInterfacePtr(&pSrcBuf10);

		D3DX10_IMAGE_INFO imgInfo;
		D3DX10_IMAGE_LOAD_INFO loadInfo;
		if( FAILED( nr = D3DXGetImageInfoFromMemory(pSrcBuf10->GetBufferPointer(), pSrcBuf10->GetBufferSize(), nullptr, &imgInfo, nullptr) ) )
			return nr;

		loadInfo.Format = newFormat;
		loadInfo.Filter = D3DX10_FILTER_NONE;
		loadInfo.MipFilter = D3DX10_FILTER_NONE;
		loadInfo.FirstMipLevel = 0;
		loadInfo.pSrcInfo = &imgInfo;
		loadInfo.MipLevels = imgInfo.MipLevels;
		loadInfo.Width = imgInfo.Width;
		loadInfo.Height = imgInfo.Height;
		loadInfo.Depth = imgInfo.Depth;

		ID3D10Texture1D* pTex1d = nullptr;
		ID3D10Texture2D* pTex2d = nullptr;
		ID3D10Texture3D* pTex3d = nullptr;
		if (pClonedResource)
		{
			ID3D10Resource* pClonedResource10 = nullptr;
			pClonedResource->GetSubResource(i)->GetInterfacePtr(&pClonedResource10);

			if (SUCCEEDED(hr = pClonedResource10->QueryInterface(__uuidof(ID3D10Texture1D), (void**)(&pTex1d))))
			{
				D3D10_TEXTURE1D_DESC tex1d_desc;
				pTex1d->GetDesc(&tex1d_desc);
				loadInfo.BindFlags = tex1d_desc.BindFlags;
				loadInfo.CpuAccessFlags = tex1d_desc.CPUAccessFlags;
				loadInfo.MipLevels = tex1d_desc.MipLevels;
				loadInfo.MiscFlags = tex1d_desc.MiscFlags;
				loadInfo.Usage = tex1d_desc.Usage;
				loadInfo.Width = tex1d_desc.Width;
				loadInfo.Height = 1;
				loadInfo.Depth = 1;

				SAFE_RELEASE(pTex1d);
			}
			else if (SUCCEEDED(hr = pClonedResource10->QueryInterface(__uuidof(ID3D10Texture2D), (void**)(&pTex2d))))
			{
				D3D10_TEXTURE2D_DESC tex2d_desc;
				pTex2d->GetDesc(&tex2d_desc);
				loadInfo.BindFlags = tex2d_desc.BindFlags;
				loadInfo.CpuAccessFlags = tex2d_desc.CPUAccessFlags;
				loadInfo.MipLevels = tex2d_desc.MipLevels;
				loadInfo.MiscFlags = tex2d_desc.MiscFlags;
				loadInfo.Usage = tex2d_desc.Usage;
				loadInfo.Width = tex2d_desc.Width;
				loadInfo.Height = tex2d_desc.Height;
				loadInfo.Depth = 1;

				SAFE_RELEASE(pTex2d);
			}
			else if (SUCCEEDED(hr = pClonedResource10->QueryInterface(__uuidof(ID3D10Texture3D), (void**)(&pTex3d))))
			{
				D3D10_TEXTURE3D_DESC tex3d_desc;
				pTex3d->GetDesc(&tex3d_desc);
				loadInfo.BindFlags = tex3d_desc.BindFlags;
				loadInfo.CpuAccessFlags = tex3d_desc.CPUAccessFlags;
				loadInfo.MipLevels = tex3d_desc.MipLevels;
				loadInfo.MiscFlags = tex3d_desc.MiscFlags;
				loadInfo.Usage = tex3d_desc.Usage;
				loadInfo.Width = tex3d_desc.Width;
				loadInfo.Height = tex3d_desc.Height;
				loadInfo.Depth = tex3d_desc.Depth;

				SAFE_RELEASE(pTex3d);
			}
			else
			{
				assert(!"Unknown texture type");
				return nr;
			}
		}
		else
		{
			loadInfo.BindFlags = D3DX10_DEFAULT;
			loadInfo.CpuAccessFlags = D3DX10_DEFAULT;
			loadInfo.MiscFlags = D3DX10_DEFAULT;
			loadInfo.Usage = (D3D10_USAGE) D3DX10_DEFAULT;
		}

		if (bStaging)
		{
			loadInfo.BindFlags = 0;
			loadInfo.Usage = D3D10_USAGE_STAGING;
			loadInfo.CpuAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
		}

		ID3D10Resource* pDestResource10 = nullptr;
		hr = D3DXCreateTextureFromMemory(pDevice10, pSrcBuf10->GetBufferPointer(), pSrcBuf10->GetBufferSize(), &loadInfo, nullptr, &pDestResource10, nullptr );
		nr[i] = hr;

		if( pDestResource10 )
		{
			if (SUCCEEDED(hr = pDestResource10->QueryInterface(__uuidof(ID3D10Texture1D), (void**)(&pTex1d))))
			{
				if( (*ppDestResource) == nullptr )
				{
					(*ppDestResource) = (CMultiResource*)(new CMultiTexture1D( (int) m_uDeviceCount ));
					(*ppDestResource)->AddRef();
				}
				CMultiTexture1D* pMulti1d = (CMultiTexture1D*)(CMultiResource*)(*ppDestResource);
				pMulti1d->SetSubTexture1D( i, new C10Texture1D( pTex1d ) );

				SAFE_RELEASE(pTex1d);
			}
			else if (SUCCEEDED(hr = pDestResource10->QueryInterface(__uuidof(ID3D10Texture2D), (void**)(&pTex2d))))
			{
				if( (*ppDestResource) == nullptr )
				{
					(*ppDestResource) = (CMultiResource*)(new CMultiTexture2D( (int) m_uDeviceCount ));
					(*ppDestResource)->AddRef();
				}
				CMultiTexture2D* pMulti2d = (CMultiTexture2D*)(CMultiResource*)(*ppDestResource);
				pMulti2d->SetSubTexture2D( i, new C10Texture2D( pTex2d ) );

				SAFE_RELEASE(pTex2d);
			}
			else if (SUCCEEDED(hr = pDestResource10->QueryInterface(__uuidof(ID3D10Texture3D), (void**)(&pTex3d))))
			{
				if( (*ppDestResource) == nullptr )
				{
					(*ppDestResource) = (CMultiResource*)(new CMultiTexture3D( (int) m_uDeviceCount ));
					(*ppDestResource)->AddRef();
				}
				CMultiTexture3D* pMulti3d = (CMultiTexture3D*)(CMultiResource*)(*ppDestResource);
				pMulti3d->SetSubTexture3D( i, new C10Texture3D( pTex3d ) );

				SAFE_RELEASE(pTex3d);
			}
			else
			{
				assert(!"Unknown texture type");
			}
		}
	}

	return nr;
}

//--------------------------------------------------------------------
NRESULT CD3DTestFramework::GoldenImage( CResource* pResource, LPCWSTR pFile )
{
    NRESULT nr( S_OK, 2 );

    if ( m_D3DOptions.generateGolden )
    {
	    for( UINT i = 0; i < m_uDeviceCount; ++i )
	    {
		    ID3D10Resource* pResource10 = nullptr;
		    pResource->GetSubResource(i)->GetInterfacePtr(&pResource10);

		    HRESULT hr = D3DX10SaveTextureToFileW(pResource10, D3DX10_IFF_DDS, pFile);
		    nr[i] = hr;
        }
    }
    else
    {
	    for( UINT i = 0; i < m_uDeviceCount; ++i )
	    {
		    ID3D10Resource* pResource10 = nullptr;
		    pResource->GetSubResource(i)->GetInterfacePtr(&pResource10);

            WindowsTest::CHandle<ID3D10Blob> pSrcBlob;

		    HRESULT hr = D3DX10SaveTextureToMemory(pResource10, D3DX10_IFF_DDS, &pSrcBlob, 0);
            if (SUCCEEDED(hr))
            {
                hr = E_FAIL;
                HANDLE h = CreateFileW(
                    pFile,
                    GENERIC_READ,
                    FILE_SHARE_READ,
                    nullptr,
                    OPEN_EXISTING,
                    0,
                    nullptr);

                if (h != INVALID_HANDLE_VALUE)
                {
                    HANDLE hMapping = CreateFileMapping(
                        h,
                        nullptr,
                        PAGE_READONLY,
                        0,
						0,
						0);

                    if (hMapping)
                    {
                        LPVOID pView = MapViewOfFile(
                            hMapping,
                            FILE_MAP_READ,
                            0,
							0,
							0);

                        if (pView)
                        {
                            DWORD dwLength = GetFileSize(h,0);
                            if ( NO_ERROR == GetLastError())
                            {
                                hr = memcmp(
									pSrcBlob->GetBufferPointer(),
									pView,
									dwLength) == 0 ?
                                        S_OK :
										E_FAIL;
                            }

                            UnmapViewOfFile(pView);
							pView = nullptr;
                        }

                        CloseHandle(hMapping);
						hMapping = nullptr;
                    }

                    CloseHandle(h);
					h = nullptr;
                }
                else
                {
        			WriteError( "ERROR: GoldenImage %s not found", pFile );
                }
            }

		    nr[i] = hr;
        }

    }

	return nr;
}

//--------------------------------------------------------------------

void CD3DTest::Cleanup()
{
	CTest::Cleanup();
}

