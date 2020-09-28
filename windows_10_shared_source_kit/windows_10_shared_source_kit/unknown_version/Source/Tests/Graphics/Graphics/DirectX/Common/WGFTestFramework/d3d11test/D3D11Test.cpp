//////////////////////////////////////////////////////////////////////////
//  D3D11Test.cpp
//  created:	2004/10/13
//
//  purpose: Implementation of CD3D11Test class
//////////////////////////////////////////////////////////////////////////

//
// Precompiled header
//

#include "pch.h"


//
// Win32 headers
//

#include <d3d11_1.h>
#include <d3d11.h>

#include <d3d10_1.h>
#include <d3d10.h>

#include <d3d9.h> // needed for d3d10level9 driver version access

#include <MMSystem.h> // needed for timeGetTime()

#include <D3DDriverVerifier.h>


//
// Project headers
//

#include "d3d11test.h"
#include "Shell.h"
#include "D3D11Test.h"


//
// Test infrastructure headers
//

#include <FormatHelper.h>
#include <rtl_simple.h>
#include <tref.h>


static DXGI_OPTIONS					s_DXGIOptionsCached;
static D3D_OPTIONS					s_D3DOptionsCached;
static D3D_RENDER_TARGET_OPTIONS	s_RenderTargetOptionsCached;


BEGIN_NAMED_VALUES( D3D_SHADER_STAGE )
	NAMED_VALUE( _T( "VERTEX" ), D3D_SHADER_STAGE_VERTEX )
	NAMED_VALUE( _T( "GEOMETRY" ), D3D_SHADER_STAGE_GEOMETRY )
	NAMED_VALUE( _T( "PIXEL" ), D3D_SHADER_STAGE_PIXEL )
	NAMED_VALUE( _T( "HULL" ), D3D_SHADER_STAGE_HULL )
	NAMED_VALUE( _T( "DOMAIN" ), D3D_SHADER_STAGE_DOMAIN )
	NAMED_VALUE( _T( "COMPUTE" ), D3D_SHADER_STAGE_COMPUTE )
END_NAMED_VALUES( D3D_SHADER_STAGE )

BEGIN_NAMED_VALUES(D3DTEST_TILED_RESOURCE_MODE)
	NAMED_VALUE( _T( "NEVER" ), D3DTEST_TILED_RESOURCE_MODE_NEVER )
	NAMED_VALUE( _T( "ONLY" ), D3DTEST_TILED_RESOURCE_MODE_ONLY )
	NAMED_VALUE( _T( "ALLOW" ), D3DTEST_TILED_RESOURCE_MODE_ALLOW )
END_NAMED_VALUES(D3DTEST_TILED_RESOURCE_MODE)


////////////////////////////////////////////////////////////
//
// FromString
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  FromString(DXGI_INFO_QUEUE_MESSAGE_CATEGORY *)
//
// Purpose:
// Returns a DXGI_INFO_QUEUE_MESSAGE_CATEGORY from a
// string.
///////////////////////////////////////////////////////
bool WINAPI FromString
(
	__out DXGI_INFO_QUEUE_MESSAGE_CATEGORY * pVal,
	const std::wstring & str
)
{
	if (str == L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_UNKNOWN")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_CATEGORY_UNKNOWN;
	}
	else if (str == L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_MISCELLANEOUS")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_CATEGORY_MISCELLANEOUS;
	}
	else if (str == L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_INITIALIZATION")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_CATEGORY_INITIALIZATION;
	}
	else if (str == L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_CLEANUP")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_CATEGORY_CLEANUP;
	}
	else if (str == L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_COMPILATION")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_CATEGORY_COMPILATION;
	}
	else if (str == L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_CREATION")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_CREATION;
	}
	else if (str == L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_SETTING")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_SETTING;
	}
	else if (str == L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_GETTING")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_GETTING;
	}
	else if (str == L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_RESOURCE_MANIPULATION")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_CATEGORY_RESOURCE_MANIPULATION;
	}
	else if (str == L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_EXECUTION")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_CATEGORY_EXECUTION;
	}
	else if (str == L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_SHADER")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_CATEGORY_SHADER;
	}
	else
	{
		// the value is not part of the enumeration
		// in that case, try conversion to integer
		*pVal = static_cast<DXGI_INFO_QUEUE_MESSAGE_CATEGORY>(_wtoi(str.c_str()));
	};

	return true;
};


///////////////////////////////////////////////////////
// Function:  FromString(DXGI_INFO_QUEUE_MESSAGE_SEVERITY *)
//
// Purpose:
// Returns a DXGI_INFO_QUEUE_MESSAGE_SEVERITY from a
// string.
///////////////////////////////////////////////////////
bool WINAPI FromString
(
	__out DXGI_INFO_QUEUE_MESSAGE_SEVERITY * pVal,
	const std::wstring & str
)
{
	if (str == L"DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION;
	}
	else if (str == L"DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR;
	}
	else if (str == L"DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING;
	}
	else if (str == L"DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO;
	}
	else if (str == L"DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE")
	{
		*pVal = DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE;
	}
	else
	{
		// the value is not part of the enumeration
		// in that case, try conversion to integer
		*pVal = static_cast<DXGI_INFO_QUEUE_MESSAGE_SEVERITY>(_wtoi(str.c_str()));
	};

	return true;
};


////////////////////////////////////////////////////////////
//
// ToString
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  ToString(DXGI_INFO_QUEUE_MESSAGE_CATEGORY)
//
// Purpose:
// Returns the string name of a DXGI info queue message
// category.
///////////////////////////////////////////////////////
std::wstring WINAPI ToString
(
	const DXGI_INFO_QUEUE_MESSAGE_CATEGORY val
)
{
	std::wstring out;
	switch (val)
	{
		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_UNKNOWN:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_UNKNOWN";
		} break;

		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_MISCELLANEOUS:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_MISCELLANEOUS";
		} break;

		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_INITIALIZATION:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_INITIALIZATION";
		} break;

		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_CLEANUP:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_CLEANUP";
		} break;

		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_COMPILATION:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_COMPILATION";
		} break;

		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_CREATION:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_CREATION";
		} break;

		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_SETTING:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_SETTING";
		} break;

		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_GETTING:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_GETTING";
		} break;

		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_RESOURCE_MANIPULATION";
		} break;

		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_EXECUTION:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_EXECUTION";
		} break;

		case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_SHADER:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_CATEGORY_SHADER";
		} break;

		default:
		{
			// the value is not part of the enumeration
			// in that case, try conversion from integer
			wchar_t valOut[16];
			_itow_s<16>(val, valOut, 10);
			out = valOut;
		} break;
	}

	return out;
};


///////////////////////////////////////////////////////
// Function:  ToString(DXGI_INFO_QUEUE_MESSAGE_SEVERITY)
//
// Purpose:
// Returns the string name of a DXGI info queue message
// severity.
///////////////////////////////////////////////////////
std::wstring WINAPI ToString
(
	const DXGI_INFO_QUEUE_MESSAGE_SEVERITY val
)
{
	std::wstring out;
	switch (val)
	{
		case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION";
		} break;

		case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR";
		} break;

		case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING";
		} break;

		case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO";
		} break;

		case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE:
		{
			out = L"DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE";
		} break;

		default:
		{
			// the value is not part of the enumeration
			// in that case, try conversion from integer
			wchar_t valOut[16];
			_itow_s<16>(val, valOut, 10);
			out = valOut;
		} break;
	}

	return out;
};

static void FormatDriverVersion(TCHAR buffer[32], LARGE_INTEGER driverVersion)
{
	_sntprintf( buffer, ( 32 * sizeof( TCHAR ) ) - 1, _T( "%d.%d.%d.%d" ),
		HIWORD( driverVersion.HighPart ), // product
		LOWORD( driverVersion.HighPart ), // version
		HIWORD( driverVersion.LowPart ),  // sub-version
		LOWORD( driverVersion.LowPart ) ); // build

}

typedef HRESULT (WINAPI PA_D3DDisassemble)
	(LPCVOID pSrcData,
		SIZE_T SrcDataSize,
		UINT Flags,
		LPCSTR szComments,
		ID3D10Blob** ppDisassembly);
typedef HRESULT (WINAPI PA_D3DReflect)
		(	LPCVOID pSrcData,
			SIZE_T SrcDataSize,
			REFIID pInterface,
			void** ppReflector);

typedef HRESULT (WINAPI PA_D3DGetSignatureBlob)
	(LPCVOID pSrcData,
		SIZE_T SrcDataSize,
		ID3DBlob** ppSignatureBlob);

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

bool CD3D11TestFramework::Is11_1DDISupported( ID3D11Device *pDevice ) const
{
	/*
		Description: Checks the specified device to see if its driver supports the
			D3D11.1 Device Driver Interface (DDI).

		Parameters:
			ID3D11DeviceContext* pDevice - The device to use check for driver support

		Returns:
			bool. The function will return true if the driver supports the 11.1 DDI. Otherwise, it will return false.
	*/

	// Local variables
	HRESULT hr = E_FAIL;
	bool b11_1DDIIsSupported = false;
	D3D11_FEATURE_DATA_D3D11_OPTIONS d3d11Options;

	if( !pDevice )
	{
		WriteError( _T( "Is11_1DDISupported() was called with an invalid device pointer." ) );
		goto done;
	}

	if( FAILED( hr = pDevice->CheckFeatureSupport( D3D11_FEATURE_D3D11_OPTIONS, &d3d11Options, sizeof(d3d11Options) ) ) )
	{
		WriteError( _T( "Is11_1DDISupported(): CheckFeatureSupport() failed. HR = %s." ), D3DHResultToString(hr).c_str() );
		goto done;
	}

	// FlagsForUpdateAndCopySeenByDriver is required on the 11.1 DDI. Thus if it is supported, the driver supports the 11.1 DDI.
	if( d3d11Options.FlagsForUpdateAndCopySeenByDriver )
	{
		b11_1DDIIsSupported = true;
	}

done:
	return b11_1DDIIsSupported;
}

const char * CD3D11TestFramework::GetShaderProfile(D3D_SHADER_STAGE ShaderStage, D3D_FEATURE_LEVEL MinFeatureLevel, D3D_FEATURE_LEVEL MaxFeatureLevel, const D3D_OPTIONS &D3DOptions)
{
	LPCSTR szNewProfile = nullptr;

	const char* szVS40_9_1 = "vs_4_0_level_9_1";
	const char* szPS40_9_1 = "ps_4_0_level_9_1";

	const char* szVS40_9_3 = "vs_4_0_level_9_3";
	const char* szPS40_9_3 = "ps_4_0_level_9_3";

	const char* szVS40 = "vs_4_0";
	const char* szGS40 = "gs_4_0";
	const char* szPS40 = "ps_4_0";
	const char* szCS40 = "cs_4_0";


	const char* szVS41 = "vs_4_1";
	const char* szGS41 = "gs_4_1";
	const char* szPS41 = "ps_4_1";
	const char* szCS41 = "cs_4_1";

	const char* szVS50 = "vs_5_0";
	const char* szGS50 = "gs_5_0";
	const char* szPS50 = "ps_5_0";
	const char* szHS50 = "hs_5_0";
	const char* szDS50 = "ds_5_0";
	const char* szCS50 = "cs_5_0";

	if (D3DOptions.FeatureLevel >= MinFeatureLevel)
	{
		D3D_FEATURE_LEVEL TargetFeatureLevel = min(MaxFeatureLevel, D3DOptions.FeatureLevel);
		switch (ShaderStage)
		{
			case D3D_SHADER_STAGE_VERTEX:
				
				if (TargetFeatureLevel == D3D_FEATURE_LEVEL_9_1 || TargetFeatureLevel == D3D_FEATURE_LEVEL_9_2)
					szNewProfile = szVS40_9_1;
				else if (TargetFeatureLevel == D3D_FEATURE_LEVEL_9_3)
					szNewProfile = szVS40_9_3;
				else if (TargetFeatureLevel == D3D_FEATURE_LEVEL_10_0)
					szNewProfile = szVS40;
				else if (TargetFeatureLevel == D3D_FEATURE_LEVEL_10_1)
					szNewProfile = szVS41;
				else if (TargetFeatureLevel == D3D_FEATURE_LEVEL_11_0 || TargetFeatureLevel == D3D_FEATURE_LEVEL_11_1)
					szNewProfile = szVS50;
				break;

			case D3D_SHADER_STAGE_GEOMETRY:
			
				if (TargetFeatureLevel == D3D_FEATURE_LEVEL_10_0)
					szNewProfile = szGS40;
				else if (TargetFeatureLevel == D3D_FEATURE_LEVEL_10_1)
					szNewProfile = szGS41;
				else if(TargetFeatureLevel == D3D_FEATURE_LEVEL_11_0 || TargetFeatureLevel == D3D_FEATURE_LEVEL_11_1)
					szNewProfile = szGS50;
				break;

			case D3D_SHADER_STAGE_PIXEL:
			
				if (TargetFeatureLevel == D3D_FEATURE_LEVEL_9_1 || TargetFeatureLevel == D3D_FEATURE_LEVEL_9_2)
					szNewProfile = szPS40_9_1;
				else if (TargetFeatureLevel == D3D_FEATURE_LEVEL_9_3)
					szNewProfile = szPS40_9_3;
				else if (TargetFeatureLevel == D3D_FEATURE_LEVEL_10_0)
					szNewProfile = szPS40;
				else if (TargetFeatureLevel == D3D_FEATURE_LEVEL_10_1)
					szNewProfile = szPS41;
				else if (TargetFeatureLevel == D3D_FEATURE_LEVEL_11_0 || TargetFeatureLevel == D3D_FEATURE_LEVEL_11_1)
					szNewProfile = szPS50;
				break;

			case D3D_SHADER_STAGE_HULL:

				if (TargetFeatureLevel == D3D_FEATURE_LEVEL_11_0 || TargetFeatureLevel == D3D_FEATURE_LEVEL_11_1)
					szNewProfile = szHS50;
				break;

			case D3D_SHADER_STAGE_DOMAIN:

				if (TargetFeatureLevel == D3D_FEATURE_LEVEL_11_0 || TargetFeatureLevel == D3D_FEATURE_LEVEL_11_1)
					szNewProfile = szDS50;
				break;

			case D3D_SHADER_STAGE_COMPUTE:

				if (TargetFeatureLevel == D3D_FEATURE_LEVEL_10_0)
					szNewProfile = szCS40;
				else if (TargetFeatureLevel == D3D_FEATURE_LEVEL_10_1)
					szNewProfile = szCS41;
				else if (TargetFeatureLevel == D3D_FEATURE_LEVEL_11_0 || TargetFeatureLevel == D3D_FEATURE_LEVEL_11_1)
					szNewProfile = szCS50;
				break;
		}
	}

	return szNewProfile;
}

HRESULT CD3D11TestFramework::GetDataWithTimeout( ID3D11DeviceContext* pDeviceContext, ID3D11Asynchronous* pAsync, void* pData, UINT dataSize, UINT dataFlags )
{
	/*
		Description: Gets the query data and times out if call exceeds timeout limit.

		Parameters:
			ID3D11DeviceContext* pDeviceContext - The context to use for the GetData call
			ID3D11Asynchronous* pAsync - Either a ID3D11Query, Predicate, or Counter interface
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
	assert( nullptr != pDeviceContext );
	assert( nullptr != pAsync );
	assert( nullptr != pData );

	//
	// Loop until query completes or times out
	//
	for(;;)
	{
		hr = pDeviceContext->GetData( pAsync, pData, dataSize, dataFlags );

		if( S_FALSE != hr )
			break;
		
		currentTime = timeGetTime();
		if( (currentTime - startTime) > m_nQueryTimeout ) // Timeout in milliseconds
			break;

		Sleep(0);
	}

	return hr;
}

static bool operator==(const LUID& lhs, const LUID& rhs)
{
	return (lhs.LowPart == rhs.LowPart && lhs.HighPart == rhs.HighPart);
}

// Entries from D3DCompiler_TEST.dll
/////////////////////////////////////////////////////////////
HRESULT WINAPI LL_D3DDisassemble( 
				LPCVOID pSrcData,
				SIZE_T SrcDataSize,
				UINT Flags,
				LPCSTR szComments,
				ID3D10Blob** ppDisassembly)
{
	HMODULE hD3DCompile = LoadLibraryExW( L"D3DCompiler_TEST.dll", nullptr, 0 );
	if( hD3DCompile == nullptr )
	{
		WriteError( _T( "D3DCompiler_TEST.dll not found." ) );
		return E_FAIL;
	}

	PA_D3DDisassemble *pD3DDisassemble = (PA_D3DDisassemble*) GetProcAddress( hD3DCompile, "D3DDisassemble" );
	if( pD3DDisassemble == nullptr )
	{
		WriteError( _T("D3DDisassemble - Function ont found in D3DCompiler_TEST.dll") );
		return E_FAIL;
	}

	return pD3DDisassemble( pSrcData, SrcDataSize, Flags, szComments, ppDisassembly );
}

HRESULT WINAPI LL_D3DReflect(
				LPCVOID pSrcData,
				SIZE_T SrcDataSize,
				REFIID pInterface,
				void** ppReflector)
{
	HMODULE hD3DCompile = LoadLibraryExW( L"D3DCompiler_TEST.dll", nullptr, 0 );
	if( hD3DCompile == nullptr )
	{
		WriteError( _T( "D3DCompiler_TEST.dll not found." ) );
		return E_FAIL;
	}

	PA_D3DReflect *pD3DReflect = (PA_D3DReflect*) GetProcAddress( hD3DCompile, "D3DReflect" );
	if( pD3DReflect == nullptr )
	{
		WriteError( _T("D3DReflect - Function ont found in D3DCompiler_TEST.dll") );
		return E_FAIL;
	}

	return pD3DReflect( pSrcData, SrcDataSize, pInterface, ppReflector );
}


HRESULT WINAPI LL_D3DGetInputAndOutputSignatureBlob(
				LPCVOID pSrcData,
				SIZE_T SrcDataSize,
				ID3DBlob** ppSignatureBlob)
{
	HMODULE hD3DCompile = LoadLibraryExW( L"D3DCompiler_TEST.dll", nullptr, 0 );
	if( hD3DCompile == nullptr )
	{
		WriteError( _T( "D3DCompiler_TEST.dll not found." ) );
		return E_FAIL;
	}

	PA_D3DGetSignatureBlob *pD3DGetInputAndOutputSignatureBlob = (PA_D3DGetSignatureBlob*) GetProcAddress( hD3DCompile, "D3DGetInputAndOutputSignatureBlob" );
	if( pD3DGetInputAndOutputSignatureBlob == nullptr )
	{
		WriteError( _T("pD3DGetInputAndOutputSignatureBlob - Function ont found in D3DCompiler_TEST.dll") );
		return E_FAIL;
	}

	return pD3DGetInputAndOutputSignatureBlob( pSrcData, SrcDataSize, ppSignatureBlob );
}


///////////////////////////////////////////////

// Enumerates adapters to find adapter with matching LUID, and fills in driver version from that adapter
// This function exists because CheckInterfaceSupport doesn't work with D3D9-only devices
static HRESULT GetD3D9DriverVersion(LUID AdapterLUID, LARGE_INTEGER* pUserModeDriverVersion)
{
	typedef HRESULT (WINAPI *fnDirect3DCreate9Ex)(UINT SDKVersion, IDirect3D9Ex**);
	HRESULT hr = E_UNEXPECTED;
	HMODULE hD3D9Dll = LoadLibraryExW( L"D3D9.dll", nullptr, 0 );
	if (hD3D9Dll)
	{
		fnDirect3DCreate9Ex pDirect3DCreate9Ex = (fnDirect3DCreate9Ex)GetProcAddress(hD3D9Dll, "Direct3DCreate9Ex");
		if (pDirect3DCreate9Ex)
		{
			IDirect3D9Ex* pD3D9 = nullptr;
			hr = pDirect3DCreate9Ex(D3D_SDK_VERSION, &pD3D9);
			if (SUCCEEDED(hr) && pD3D9)
			{
				D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
				ZeroMemory( &AdapterIdentifier, sizeof( AdapterIdentifier ) );

				const UINT adapterCount = pD3D9->GetAdapterCount();
				for (UINT n = 0; n < adapterCount; ++n)
				{
					LUID tempLUID;
					hr = pD3D9->GetAdapterLUID( n, &tempLUID );
					if ( SUCCEEDED(hr) )
					{
						if (tempLUID == AdapterLUID)
						{
							hr = pD3D9->GetAdapterIdentifier( n, 0, &AdapterIdentifier );
							if ( SUCCEEDED(hr) && pUserModeDriverVersion )
							{
								*pUserModeDriverVersion = AdapterIdentifier.DriverVersion;
							}
							// stop enumerating adapters
							break;
						}
						else
						{
							// we want this to be the error if all adapters are enumerated and the LUID isn't found
							// it will be overridden by GetAdapterIdentifier above on a match
							hr = E_HANDLE;
						}
					}
				}

			}
			SAFE_RELEASE(pD3D9);
		}

		FreeLibrary(hD3D9Dll);
		hD3D9Dll = nullptr;
	}

	return hr;
}


//--------------------------------------------------------------------

HRESULT CD3D11TestFramework::CreateDevice( IDXGIAdapter **ppAdapter, D3D_DRIVER_TYPE DriverType, UINT uFlags, D3D_FEATURE_LEVEL FeatureLevel, ID3D11Device **ppDevice, ID3D11DeviceContext **ppDeviceContext )
{
	return CreateDevice11( ppAdapter, DriverType, uFlags, FeatureLevel, ppDevice, ppDeviceContext );
}


HRESULT CD3D11TestFramework::CreateDevice11( IDXGIAdapter **ppAdapter, D3D_DRIVER_TYPE DriverType, UINT uFlags, D3D_FEATURE_LEVEL FeatureLevel, ID3D11Device **ppDevice, ID3D11DeviceContext **ppDeviceContext )
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;
	HRESULT hCreateDeviceResult = E_FAIL;

	CD3D11TestFramework * pFramework = dynamic_cast< CD3D11TestFramework *>( GetFramework() );
	assert( pFramework != nullptr );

	ID3D11Device * pDevice11 = nullptr;
	ID3D11DeviceContext * pDeviceContext = nullptr;

	bool bAdapterSupportsIDXGIAdapter1 = false;
	bool bAdapterIsRemote = false;

	IDXGIAdapter * pAdapter = ppAdapter ? *ppAdapter : nullptr;
	IDXGIAdapter1 * pAdapter1 = nullptr;


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
	// Load the rasterizer, and create the device.
	//

	if (pFramework->m_D3DOptions.proxyDriver)
	{
		HMODULE hProxyDriver = nullptr;
		HRESULT hSetupProxyDriverResult = SetupProxyDriver(&hProxyDriver);
		if
		(
			FAILED(hSetupProxyDriverResult) ||
			nullptr == hProxyDriver
		)
		{
			WriteError(_T("CreateDevice11:  SetupProxyDriver failed."));

			hOverallResult = E_FAIL;
			goto Cleanup;
		}

		//DriverType needs to be software to load the module
		DriverType = D3D_DRIVER_TYPE_SOFTWARE;
		pAdapter = nullptr;

		assert( pDevice11 == nullptr );
		assert( pDeviceContext == nullptr );
		hCreateDeviceResult = LocalD3D11CreateDevice
		(
			pAdapter,
			DriverType,
			hProxyDriver,
			uFlags,
			&FeatureLevel,
			1,
			D3D11_SDK_VERSION,
			&pDevice11,
			nullptr,
			&pDeviceContext
		);
	}
	else if
	(
		DriverType == D3D_DRIVER_TYPE_SOFTWARE &&
		pFramework->m_D3DOptions.SoftwareRasterizerDLLName[ 0 ] != L'\0'
	)
	{
		HMODULE hD3DSoftwareDriver = pFramework->LoadSoftwareRasterizer();
		if ( nullptr == hD3DSoftwareDriver )
		{
			WriteErrorW
			(
				L"CreateDevice11:  Failed to load software rasterizer DLL %s.",
				pFramework->m_D3DOptions.SoftwareRasterizerDLLName
			);

			hOverallResult = E_FAIL;
			goto Cleanup;
		}

		assert( pDevice11 == nullptr );
		assert( pDeviceContext == nullptr );
		hCreateDeviceResult = LocalD3D11CreateDevice
		(
			pAdapter,
			DriverType,
			hD3DSoftwareDriver,
			uFlags,
			&FeatureLevel,
			1,
			D3D11_SDK_VERSION,
			&pDevice11,
			nullptr,
			&pDeviceContext
		);
	}
	else
	{
		if ( pAdapter != nullptr )
		{
			DriverType = D3D_DRIVER_TYPE_UNKNOWN;
		}

		assert( pDevice11 == nullptr );
		assert( pDeviceContext == nullptr );
		hCreateDeviceResult = LocalD3D11CreateDevice
		(
			pAdapter,
			DriverType,
			nullptr,
			uFlags,
			&FeatureLevel,
			1,
			D3D11_SDK_VERSION,
			&pDevice11,
			nullptr,
			&pDeviceContext
		);
	}

	if ( FAILED( hCreateDeviceResult ) )
	{
		SAFE_RELEASE(pDevice11);
		SAFE_RELEASE(pDeviceContext);

		hOverallResult = hCreateDeviceResult;

		// BUGBUG - H-A-C-K FIX to enable skipping test cases which cycle on debug layer when sdk not installed.
		if( uFlags & D3D11_CREATE_DEVICE_DEBUG )
		{
			D3D_FEATURE_LEVEL fl;
			HRESULT hr = D3D11CreateDevice(
				nullptr, 
				D3D_DRIVER_TYPE_NULL, 
				nullptr, 
				D3D11_CREATE_DEVICE_DEBUG, 
				nullptr,
				0,
				D3D11_SDK_VERSION,
				nullptr,
				&fl,
				nullptr
				);

			if(FAILED(hr))
			{
				GetFramework()->WriteToLog( "CreateDevice:  SDKLayers cannot be found.  Unable to create debug layer." );
				goto Cleanup;
			}
		}

		GetFramework()->WriteToLogF
		(
			"CreateDevice11:  D3D11CreateDevice failed with error %s.",
			D3DHResultToString( hCreateDeviceResult ).c_str()
		);
	}
	else
	{
		*ppDevice = pDevice11;
		*ppDeviceContext = pDeviceContext;

		hOverallResult = S_OK;
	}


Cleanup:
	SAFE_RELEASE( pAdapter1 );

	return hOverallResult;
}

D3D10_FEATURE_LEVEL1 D3DFeatureLevelToD3D10FeatureLevel1(D3D_FEATURE_LEVEL featureLevel)
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
		default:
			featureLevel10 = D3D10_FEATURE_LEVEL_10_0;
			assert( _T( "Feature level cannot be converted to D3D10_FEATURE_LEVEL1" ) );
			break;
	}

	return featureLevel10;
}

HRESULT D3DDriverTypeToD3D10DriverType(D3D_DRIVER_TYPE d3dDriverType, D3D10_DRIVER_TYPE *d3d10DriverType )
{
	D3D10_DRIVER_TYPE driverType = D3D10_DRIVER_TYPE_HARDWARE;

	switch( d3dDriverType )
	{
	default:
	case D3D_DRIVER_TYPE_HARDWARE:
		driverType = D3D10_DRIVER_TYPE_HARDWARE;
		break;
	case D3D_DRIVER_TYPE_REFERENCE:
		driverType = D3D10_DRIVER_TYPE_REFERENCE;
		break;
	case D3D_DRIVER_TYPE_NULL:
		driverType = D3D10_DRIVER_TYPE_NULL;
		break;
	case D3D_DRIVER_TYPE_SOFTWARE:
		driverType = D3D10_DRIVER_TYPE_SOFTWARE;
		break;
	case D3D_DRIVER_TYPE_WARP:
		driverType = D3D10_DRIVER_TYPE_WARP;
		break;
	}

	*d3d10DriverType = driverType;
	
	return S_OK;
}

UINT D3D11CreateDeviceFlagsToD3D10CreateDeviceFlags(UINT d3d11Flags)
{
	UINT d3d10Flags = 0;

	if( d3d11Flags & D3D11_CREATE_DEVICE_SINGLETHREADED	)
		d3d10Flags |= D3D10_CREATE_DEVICE_SINGLETHREADED;

	if( d3d11Flags & D3D11_CREATE_DEVICE_DEBUG	)
		d3d10Flags |= D3D10_CREATE_DEVICE_DEBUG;

	if( d3d11Flags & D3D11_CREATE_DEVICE_SWITCH_TO_REF	)
		d3d10Flags |= D3D10_CREATE_DEVICE_SWITCH_TO_REF;

	if( d3d11Flags & D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS	)
		d3d10Flags |= D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;

	if( d3d11Flags & D3D11_CREATE_DEVICE_BGRA_SUPPORT	)
		d3d10Flags |= D3D10_CREATE_DEVICE_BGRA_SUPPORT;

	if( d3d11Flags & D3D11_CREATE_DEVICE_DEBUGGABLE	)
		d3d10Flags |= D3D10_CREATE_DEVICE_DEBUGGABLE;

	return d3d10Flags;
};

HRESULT CD3D11TestFramework::LocalD3D11CreateDevice(
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
	HRESULT hresult = E_FAIL;
	ID3D10Device1 *device10_1 = nullptr;
	ID3D10Device *device10 = nullptr;
	ID3D11Device1 *device11_1 = nullptr;
	ID3D11DeviceContext1 *immediateContext1 = nullptr;
	ID3DDeviceContextState *functionBlock = nullptr;

	switch(m_D3DOptions.origInterfaceLevel)
	{
	case D3DTEST_INTERFACE_LEVEL_11_1:
	case D3DTEST_INTERFACE_LEVEL_11_0:
		// Call straight through to the real D3D11CreateDevice method
		hresult = D3D11CreateDevice(
			pAdapter,
			DriverType,
			Software,
			Flags,
			pFeatureLevels,
			FeatureLevels,
			SDKVersion,
			ppDevice,
			pFeatureLevel,
			ppImmediateContext);
		break;
	case D3DTEST_INTERFACE_LEVEL_10_1:
		{
			D3D10_DRIVER_TYPE d3d10DriverType;
			if(FAILED(hresult = D3DDriverTypeToD3D10DriverType(DriverType, &d3d10DriverType)))
				goto cleanup;

			assert( m_D3DOptions.origFeatureLevel <= D3D_FEATURE_LEVEL_10_1 );

			// Create a D3D10.1 device
			hresult = IndirectD3D10CreateDevice1(
				pAdapter,
				d3d10DriverType,
				Software,
				D3D11CreateDeviceFlagsToD3D10CreateDeviceFlags(Flags),
				D3DFeatureLevelToD3D10FeatureLevel1( m_D3DOptions.origFeatureLevel ),
				D3D10_1_SDK_VERSION,
				&device10_1 );

			if(FAILED(hresult))
				goto cleanup;

			// Query for 11.0 device 
			if( FAILED( hresult = device10_1->QueryInterface(__uuidof(ID3D11Device), (void **) ppDevice) ) )
				goto cleanup;

			(*ppDevice)->GetImmediateContext( ppImmediateContext );

			// Create a D3D11 Function Block and activate it
			if( FAILED( hresult = device10_1->QueryInterface(__uuidof(ID3D11Device1), (void **) &device11_1) ) )
				goto cleanup;

			D3D_FEATURE_LEVEL featureLevelCreated;
			assert( functionBlock == nullptr );
			if( FAILED( hresult = device11_1->CreateDeviceContextState( 0, pFeatureLevels, 1, D3D11_SDK_VERSION, __uuidof(ID3D11Device), &featureLevelCreated, &functionBlock ) ) )
				goto cleanup;

			device11_1->GetImmediateContext1( &immediateContext1 );
			immediateContext1->SwapDeviceContextState( functionBlock, nullptr );
		}
		break;
	case D3DTEST_INTERFACE_LEVEL_10_0:
		{
			D3D10_DRIVER_TYPE d3d10DriverType;
			if(FAILED(hresult = D3DDriverTypeToD3D10DriverType(DriverType, &d3d10DriverType)))
				goto cleanup;

			// Create a D3D10.1 device
			hresult = IndirectD3D10CreateDevice(
				pAdapter,
				d3d10DriverType,
				Software,
				D3D11CreateDeviceFlagsToD3D10CreateDeviceFlags(Flags),
				D3D10_SDK_VERSION,
				&device10 );

			if(FAILED(hresult))
				goto cleanup;

			// Query for 11.0 device 
			if( FAILED( hresult = device10->QueryInterface(__uuidof(ID3D11Device), (void **) ppDevice) ) )
				goto cleanup;
			(*ppDevice)->GetImmediateContext( ppImmediateContext );

			// Create a D3D11 Function Block and activate it
			if( FAILED( hresult = device10->QueryInterface(__uuidof(ID3D11Device1), (void **) &device11_1) ) )
				goto cleanup;

			D3D_FEATURE_LEVEL featureLevelCreated;
			assert( functionBlock == nullptr );
			if( FAILED( hresult = device11_1->CreateDeviceContextState( 0, pFeatureLevels, 1, D3D11_SDK_VERSION, __uuidof(ID3D11Device), &featureLevelCreated, &functionBlock ) ) )
				goto cleanup;

			device11_1->GetImmediateContext1( &immediateContext1 );
			immediateContext1->SwapDeviceContextState( functionBlock, nullptr );
		}
		break;
	}

cleanup:
	SAFE_RELEASE(functionBlock);
	SAFE_RELEASE(immediateContext1);
	SAFE_RELEASE(device11_1);
	SAFE_RELEASE(device10_1);
	SAFE_RELEASE(device10);

	return hresult;
}


//--------------------------------------------------------------------

CD3D11TestFramework::CD3D11TestFramework()
:
	CGraphicsTestFramework
	(
		&m_D3DOptions
	),
	m_pAdapters( nullptr ),
	m_pAdapterOutputs( nullptr ),
	m_pSwapChains( nullptr ),
	m_pD3DDevice( nullptr ),
	m_pD3DDeviceImmediateContext( nullptr ),
	m_pD3DDeviceDeferredContext( nullptr ),
	m_pD3DDevice1( nullptr ),
	m_pD3DDeviceImmediateContext1( nullptr ),
	m_pD3DDeviceDeferredContext1( nullptr ),
	m_pD3DDevice2( nullptr ),
	m_pD3DDeviceImmediateContext2( nullptr ),
	m_pD3DDeviceDeferredContext2( nullptr ),
	m_pDXGIDebug( nullptr ),
	m_pDXGIInfoQueue( nullptr ),
	m_pD3DDebug( nullptr ),
	m_pD3DInfoQueue( nullptr ),
	m_pTestDevice( nullptr ),
	// BUGBUG - chrisza: Switch to ref not implemented in M1
	//m_pD3DSwitchToRef( nullptr ),
	m_pExtensions( nullptr ),
	m_pRTTextures( nullptr ),
	m_pRTTextures1D( nullptr ),
	m_pRTBuffers( nullptr ),
	m_pRTViews( nullptr ),
	m_pDepthStencil( nullptr ),
	m_pDepthStencil1D( nullptr ),
	m_pDSView( nullptr ),
	m_bD3DValid( false ),
	m_bRenderTargetValid( false ),
	m_bDXGIValid( false ),
	m_bSetupCalled( false ),
	m_uDeviceCount( 0 ),
	m_uSwapChainCount( 0 ),
	m_bForceFeatureLevel( false ),
	m_pTilePoolManager(nullptr),
	m_viewResources(false),
	m_nQueryTimeout( 5000 )	// Timeout in ms
{
	// Initialize cached options to all 0xff...
	memset( &s_DXGIOptionsCached, 0xff, sizeof( s_DXGIOptionsCached ) );
	memset( &s_D3DOptionsCached, 0xff, sizeof( s_D3DOptionsCached ) );
	memset( &s_RenderTargetOptionsCached, 0xff, sizeof( s_RenderTargetOptionsCached ) );
	memset( &m_DXGIFactory, 0, sizeof( m_DXGIFactory ) );


	// Intialize default options
	memset( &m_RenderTargetOptions, 0, sizeof( m_RenderTargetOptions ) );

	m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_11_1;
	m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_10_0;
	m_D3DOptions.wMaxFeatureLevel = D3D_FEATURE_LEVEL_11_1;
	m_D3DOptions.SrcOnly = true;
	m_D3DOptions.SrcDriverType = D3D_DRIVER_TYPE_HARDWARE;
	m_D3DOptions.RefDriverType = D3D_DRIVER_TYPE_REFERENCE;
	m_D3DOptions.bgraSupport = false;
	m_D3DOptions.tiledResourceMode = D3DTEST_TILED_RESOURCE_MODE_ALLOW;

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
	m_RenderTargetOptions.SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	m_RenderTargetOptions.SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	m_RenderTargetOptions.SwapChainDesc.Flags = 0;
	
	m_RenderTargetOptions.OutputWindow = nullptr;
		m_RenderTargetOptions.Windowed = FALSE;

	// Default D3D11_RENDER_TARGET_VIEW_DESC 
	m_RenderTargetOptions.RenderTargetViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	m_RenderTargetOptions.RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_UNKNOWN;
	m_RenderTargetOptions.RenderTargetViewDesc.Texture2D.MipSlice = 0;

	// Default D3D11_DEPTH_STENCIL_VIEW_DESC
	m_RenderTargetOptions.DepthStencilViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	m_RenderTargetOptions.DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_UNKNOWN;
	m_RenderTargetOptions.DepthStencilViewDesc.Texture2D.MipSlice = 0;

	// Default D3D11_DEPTH_STENCIL_DESC
	m_RenderTargetOptions.DepthStencilDesc.DepthEnable = TRUE;
	m_RenderTargetOptions.DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	m_RenderTargetOptions.DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	m_RenderTargetOptions.DepthStencilDesc.StencilEnable = FALSE;

	m_RenderTargetOptions.DepthTextureDesc.ArraySize = 1;
	m_RenderTargetOptions.DepthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	m_RenderTargetOptions.DepthTextureDesc.CPUAccessFlags = 0;
	m_RenderTargetOptions.DepthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_RenderTargetOptions.DepthTextureDesc.Height = 280;
	m_RenderTargetOptions.DepthTextureDesc.Width = 320;
	m_RenderTargetOptions.DepthTextureDesc.MipLevels = 0;
	m_RenderTargetOptions.DepthTextureDesc.MiscFlags = 0;
	m_RenderTargetOptions.DepthTextureDesc.SampleDesc.Count = 1;
	m_RenderTargetOptions.DepthTextureDesc.Usage = D3D11_USAGE_DEFAULT;

	// Tests should run latest DXGI version
	m_DXGIOptions.Version = TEST_DXGI_VERSION_1_2;	

	hMainWaiting = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	hWorkerWaiting = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	dwMainThread = GetCurrentThreadId();
}


//--------------------------------------------------------------------

CD3D11TestFramework::~CD3D11TestFramework()
{
	CloseHandle(hMainWaiting);
	CloseHandle(hWorkerWaiting);
}


//--------------------------------------------------------------------

bool CD3D11TestFramework::Setup()
{
	// Copy current device settings to global cache
	memcpy( &m_DXGIOptionsGlobal, &m_DXGIOptions, sizeof( m_DXGIOptions ) );
	memcpy( &m_D3DOptionsGlobal, &m_D3DOptions, sizeof( m_D3DOptions ) );
	memcpy( &m_RenderTargetOptionsGlobal, &m_RenderTargetOptions, sizeof( m_RenderTargetOptions ) );

	return true;
}


//--------------------------------------------------------------------

void CD3D11TestFramework::SetRuntimeVersion()
{
	// setup the devices just to set the actual supported feature level 
	// for "-whql" mode before InitTestParameters()
	if( CD3D11TestFramework::SetupDXGI() == RESULT_PASS )
	{
		CD3D11TestFramework::SetupD3D();
		CD3D11TestFramework::CleanupDXGI();
	}
}


//--------------------------------------------------------------------

void CD3D11TestFramework::Cleanup()
{
	CleanupDXGI();
}


//--------------------------------------------------------------------

void CD3D11TestFramework::CleanupTest()
{
	CFormatHelper::Cleanup();
	//if(!m_DXGLog.InTestCase())
	//{
	//	m_DXGLog.BeginTestCase(_T("WDDM Aperture Corruption Detected."));
	//}
	//						
	//m_DXGLog.TestCaseResult(WTT_TESTCASE_RESULT_FAIL);
	//m_DXGLog.EndTestCase();
	// BUGBUG - MARTY ADD APERTURE CODE HERE...
	CWGFTestFramework::CleanupTest();
}


//--------------------------------------------------------------------

NRESULT CD3D11TestFramework::CompileShaderFromFile( 
	LPCTSTR pFile, LPCSTR pFunctionName, 
	D3D_SHADER_STAGE ShaderStage, 
	D3D_FEATURE_LEVEL MinFeatureLevel, D3D_FEATURE_LEVEL MaxFeatureLevel, 
	UINT Flags, 
	ID3D10Blob** ppShader, ID3D10Blob** ppError, 
	CONST D3D10_SHADER_MACRO *pDefines, ID3D10Blob **ppShaderText )
{
	HRESULT hr;
	NRESULT nr = S_OK;
	ID3D10Blob *pShader = nullptr;
	ID3D10Blob *pError = nullptr;

	LPCSTR szNewProfile = GetShaderProfile(ShaderStage, MinFeatureLevel, MaxFeatureLevel, m_D3DOptions);
	if( szNewProfile == nullptr )
	{
		WriteToLog(_T("CompileShaderFromFile:  No shader profile found that matches the current feature level and required feature level range."));
		return E_INVALIDARG;
	}

	if (pDefines != nullptr && ppShaderText != nullptr)
	{
		hr = D3DX10PreprocessShaderFromFile( pFile, pDefines, nullptr, nullptr, ppShaderText, nullptr, nullptr );
		nr = hr;
	}

	hr = D3DX10CompileFromFile( pFile, pDefines, nullptr, pFunctionName, szNewProfile, Flags, 0, nullptr, &pShader, &pError, nullptr );
	nr = hr;

	*ppShader = pShader;
	if (ppError != nullptr)
		*ppError = pError;
	else
	if( pError )
		pError->Release();

	return nr;
}


NRESULT CD3D11TestFramework::CompileShaderFromResource( 
	HMODULE hModule, LPCTSTR pResource, LPCSTR pFunctionName, 
	D3D_SHADER_STAGE ShaderStage, 
	D3D_FEATURE_LEVEL MinFeatureLevel, D3D_FEATURE_LEVEL MaxFeatureLevel, 
	UINT Flags, 
	ID3D10Blob **ppShader, ID3D10Blob **ppError, 
	CONST D3D10_SHADER_MACRO *pDefines, ID3D10Blob **ppShaderText )
{
	HRESULT hr;
	NRESULT nr = S_OK;
	ID3D10Blob *pShader = nullptr;
	ID3D10Blob *pError = nullptr;

	LPCSTR szNewProfile = GetShaderProfile(ShaderStage, MinFeatureLevel, MaxFeatureLevel, m_D3DOptions);
	if( szNewProfile == nullptr )
	{
		WriteToLog(_T("CompileShaderFromResource:  No shader profile found that matches the current feature level and required feature level range."));
		return E_INVALIDARG;
	}

	if (pDefines != nullptr && ppShaderText != nullptr)
	{
		hr = D3DX10PreprocessShaderFromResource( hModule, pResource, nullptr, pDefines, nullptr, nullptr, ppShaderText, nullptr, nullptr );
		nr = hr;
	}

	hr = D3DX10CompileFromResource( hModule, pResource, nullptr, pDefines, nullptr, pFunctionName, szNewProfile, Flags, 0, nullptr, &pShader, &pError, nullptr );
	nr = hr;

	*ppShader = pShader;
	if (ppError != nullptr)
		*ppError = pError;
	else
	if( pError )
		pError->Release();

	return nr;
}


NRESULT CD3D11TestFramework::CompileShaderFromMemory( 
	LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFunctionName, 
	D3D_SHADER_STAGE ShaderStage, 
	D3D_FEATURE_LEVEL MinFeatureLevel, D3D_FEATURE_LEVEL MaxFeatureLevel, 
	UINT Flags, 
	ID3D10Blob** ppShader, ID3D10Blob** ppError, 
	CONST D3D10_SHADER_MACRO *pDefines, ID3D10Blob **ppShaderText )
{
	HRESULT hr;
	NRESULT nr = S_OK;
	ID3D10Blob *pShader = nullptr;
	ID3D10Blob *pError = nullptr;

	LPCSTR szNewProfile = GetShaderProfile(ShaderStage, MinFeatureLevel, MaxFeatureLevel, m_D3DOptions);
	if( szNewProfile == nullptr )
	{
		WriteToLog(_T("CompileShaderFromMemory:  No shader profile found that matches the current feature level and required feature level range."));
		return E_INVALIDARG;
	}

	if (pDefines != nullptr && ppShaderText != nullptr)
	{
		hr = D3DX10PreprocessShaderFromMemory( pSrcData, SrcDataLen, nullptr, pDefines, nullptr, nullptr, ppShaderText, nullptr, nullptr );
		nr = hr;
	}

	hr = D3DX10CompileFromMemory( pSrcData, SrcDataLen, nullptr, pDefines, nullptr, pFunctionName, szNewProfile, Flags, 0, nullptr, &pShader, &pError, nullptr );
	nr = hr;
		
	*ppShader = pShader;
	if (ppError != nullptr)
		*ppError = pError;
	else
	if( pError )
		pError->Release();

	return nr;
}


TEST_RESULT CD3D11TestFramework::SetupDXGI()
{
	//
	// Initialize locals.
	//

	TEST_RESULT SetupResult = RESULT_PASS;

	
	//
	// Clean up any DXGI objects that we might have created
	// up to now.
	//

	CleanupDXGI();


	//
	// Create the DXGI factory.
	//

	HRESULT hCreateFactoryResult = CreateDXGIFactory( &m_DXGIFactory );
	if ( FAILED( hCreateFactoryResult ) )
	{
		WriteToLogF
		(
			_T( "SetupDXGI:  CreateDXGIFactory failed with error %s." ),
			D3DHResultToString( hCreateFactoryResult ).c_str()
		);

		SetupResult = RESULT_FAIL;

		// hCreateFactoryResult == E_NOINTERFACE could indicate that the target 
		// machine does not have the correct OS or updates installed (e.g. 8IP)
		if( hCreateFactoryResult == E_NOINTERFACE )
		{
			WriteError( _T( "Test requires DXGI interface not compatible with this system.  Please ensure all service packs and recommended updates are installed." ) );
		}

		goto Cleanup;
	};


	//
	// Determine how many devices the test needs, and allocate
	// a corresponding number of DXGI adapters and outputs.
	//

	m_uDeviceCount = m_D3DOptions.SrcOnly ? 1 : 2;
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
	
	for( int i = (int) m_uDeviceCount - 1; i >= 0; --i )
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
			// Set-up adapters and fullscreen outputs
			//

			SetupResult = SetupAdapter( i, &m_DXGIFactory, &m_pAdapters[i] );
			if( SetupResult != RESULT_PASS )
			{
				goto Cleanup;
			}

		if ( m_pAdapters[ i ] == nullptr )
		{
			WriteToLogF
			(
				_T( "SetupDXGI:  EnumAdapters on adapter %i for device %d succeeded but returned a NULL adapter pointer." ),
				m_DXGIOptions.AdapterID[ i ],
				i
			);
				if (!IsSession0())
				{
					SetupResult = SetupFullscreenOutput( i, m_pAdapters[i], &m_pAdapterOutputs[i] );
						if( SetupResult != RESULT_PASS )
						{
							goto Cleanup;
						}
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
}

//--------------------------------------------------------------------

void CD3D11TestFramework::CleanupDXGI()
{
	//
	// Release Direct3D devices.
	//

	CleanupD3D();


	//
	// Release DXGI adapters and outputs.
	//

	for
	(
		UINT i = 0;
		i < m_uDeviceCount;
		++i
	)
	{
		if ( m_pAdapterOutputs != nullptr )
		{
			SAFE_RELEASE( m_pAdapterOutputs[ i ] );
		};

		if ( m_pAdapters != nullptr )
		{
			SAFE_RELEASE( m_pAdapters[ i ] );
		};
	};

	SAFE_DELETE_ARRAY( m_pAdapters );
	SAFE_DELETE_ARRAY( m_pAdapterOutputs );

	m_uDeviceCount = 0;


	//
	// Release the DXGI factory.
	//

	SAFE_RELEASE( m_DXGIFactory.pFactory );


	//
	// Report leaked objects.
	//

	ReportLiveDXGIObjects();


	//
	// Release debug objects and info queues.
	//

	SAFE_RELEASE( m_pDXGIInfoQueue );
	SAFE_RELEASE( m_pDXGIDebug );


	//
	// Note that DXGI objects are no longer valid.
	//

	m_bDXGIValid = false;
}


//--------------------------------------------------------------------

TEST_RESULT CD3D11TestFramework::SetupD3D()
{
	//
	// Initialize locals.
	//

	HRESULT hr = E_FAIL;


	//
	// Clean up Direct3D devices that we might have created
	// up to this point.
	//

	CleanupD3D();

	// Get the Windows version
	RTL_OSVERSIONINFOW windowsVersionInfo;
	windowsVersionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
	RtlGetVersion(&windowsVersionInfo);

	//
	// Override flags if necessary.
	//

	if( m_D3DOptions.DriverVerifier )
	{
		// Force the driver verifier layer to be on, if possible
		HMODULE hDriverVerifier = LoadLibraryW(L"D3DDriverVerifier.dll");
		if( hDriverVerifier == NULL )
		{
			// We can force the driver verifier layer on without blocking
			m_D3DOptions.DriverVerifier = false;
		}
		FreeLibrary(hDriverVerifier);
	}

	// Check if the test has opted out of the D3D driver verifier
	if( m_D3DOptions.DriverVerifier && m_D3DOptions.DriverVerifierOptOut )
	{
		WriteToLog( _T("This test has opted out of using the D3D driver verifier.") );
		m_D3DOptions.DriverVerifier = false;
	}

	//
	// Initialize our flags.
	//

	DWORD dwFlags = 0;
	dwFlags |= m_D3DOptions.Debug ? D3D11_CREATE_DEVICE_DEBUG : 0;
	dwFlags |= m_D3DOptions.DriverVerifier ? D3D11_CREATE_DEVICE_DRIVER_VERIFIER : 0;
	
	if( m_D3DOptions.allowSwitchToRef || m_D3DOptions.forceSwitchToRef )
	{
		dwFlags |= D3D11_CREATE_DEVICE_SWITCH_TO_REF;
	}

	if( m_D3DOptions.bgraSupport )
	{
		dwFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	}

	dwFlags |= m_D3DOptions.CreateDeviceFlags;

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
		// D3D 10.0 interfaces cannot specify feature level, so automatically upgrade origInterfaceLevel to D3D 10.1.
		m_D3DOptions.origInterfaceLevel = D3DTEST_INTERFACE_LEVEL_10_1;
		
		// If target feature level is 9_x, then origFeatureLevel must be 9_x as well.
		if( m_D3DOptions.origFeatureLevel >= (UINT) D3D_FEATURE_LEVEL_10_0 )
		{
			m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_9_1;
		}
	}

	if( m_D3DOptions.DriverVerifier && m_D3DOptions.origInterfaceLevel < D3DTEST_INTERFACE_LEVEL_11_0 )
	{
		// Lower interfaces cannot specify the driver verifier flag so automatically upgrade origInterfaceLevel to D3D11.0
		m_D3DOptions.origInterfaceLevel = D3DTEST_INTERFACE_LEVEL_11_0;
	}

	//
	// Create our device(s).
	//

	D3D_DRIVER_TYPE DriverType = m_D3DOptions.SrcDriverType;
	
	//check for proxyDriver flag
	if (m_D3DOptions.proxyDriver) //driver type must be software to load module
	{
		DriverType = D3D_DRIVER_TYPE_SOFTWARE;
	}

	if( !m_bForceFeatureLevel )
	{
		// Use this array to control what feature levels are tried, and in what order
		const D3D_FEATURE_LEVEL featureLevelsToTry[] = { 
			D3D_FEATURE_LEVEL_11_1,		// Try this first...
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1};	// Try this last.
		const UINT numFeatureLevelsToTry = ARRAY_SIZE(featureLevelsToTry);
		const char szAttemptingCreateDeviceStr[] = "Attempting to create a D3D11.0 feature level %s device...";
		const char szUnsuccessfulCreateDeviceStr[] = "Creation of a feature level %s device was unsuccessful, falling back the next feature level...";
		const char szSuccessfulCreateDeviceStr[] = "A feature level %s device was created successfully.";

		// Go through each feature level, one at a time...
		for( UINT i = 0; i < numFeatureLevelsToTry; i++ )
		{
			// Don't bother with feature levels above the max supported by the test
			if(m_D3DOptions.wMaxFeatureLevel < featureLevelsToTry[i])
				continue;

			// If less than the minimum feature level then stop trying
			if(m_D3DOptions.wMinFeatureLevel > featureLevelsToTry[i])
				break;

			// Get the current feature level to try...
			m_D3DOptions.FeatureLevel = featureLevelsToTry[i];
			WriteToLog( FormatStringTS( szAttemptingCreateDeviceStr, ToString(m_D3DOptions.FeatureLevel).c_str() ).c_str() );
			
			// Try to create the device...
			assert( m_pD3DDevice == nullptr );
			assert( m_pD3DDeviceImmediateContext == nullptr );
			hr = CreateDevice
			(
				m_pAdapters ? m_pAdapters : nullptr,
				DriverType,
				dwFlags,
				m_D3DOptions.FeatureLevel,
				&m_pD3DDevice,
				&m_pD3DDeviceImmediateContext
			);

			// Check to see if the device was created. If not, try again.
			if ( FAILED( hr ) )
			{
				WriteToLog( FormatStringTS( szUnsuccessfulCreateDeviceStr, ToString(m_D3DOptions.FeatureLevel).c_str() ).c_str() );
			}
			else
			{
				assert( m_pD3DDevice );
				assert( m_pD3DDeviceImmediateContext );
				WriteToLog( FormatStringTS( szSuccessfulCreateDeviceStr, ToString(m_D3DOptions.FeatureLevel).c_str() ).c_str() );

				// On subsequent attempts force the same feature level
				m_bForceFeatureLevel = true;
				break;
			}
		}
	}
	else
	{
		assert( m_pD3DDevice == nullptr );
		assert( m_pD3DDeviceImmediateContext == nullptr );
		hr = CreateDevice
		(
			m_pAdapters ? m_pAdapters : nullptr,
			DriverType,
			dwFlags,
			m_D3DOptions.FeatureLevel,
			&m_pD3DDevice,
			&m_pD3DDeviceImmediateContext
		);
	}
		
	if ( FAILED( hr ) )
	{
		SAFE_RELEASE( m_pD3DDevice );
		SAFE_RELEASE( m_pD3DDeviceImmediateContext );

		switch( hr )
		{
			case DXGI_ERROR_UNSUPPORTED:
			case E_NOINTERFACE:
			/* case E_NOTIMPL: */ // N.B. [SEdmison]:  jianchen says that this is possible as well; I'm not sure yet
			{
				return RESULT_SKIP;
			} break;

			default:
			{
				WriteError
				(
					_T( "SetupD3D:  CreateDevice failed with error %s." ),
					D3DHResultToString( hr ).c_str()
				);

				return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
			} break;
		};
	}

	if ( m_bDeferredContextUsed )
	{
		if( GetTestMode() == WGFTEST_MODE_WHQL )
		{
			D3D11_FEATURE_DATA_THREADING threadingCaps;
			if( FAILED( hr = m_pD3DDevice->CheckFeatureSupport( D3D11_FEATURE_THREADING, &threadingCaps, sizeof(threadingCaps) ) ) )
			{
				WriteError
				(
					_T( "SetupD3D:  CheckFeatureSupport failed with error %s." ),
					D3DHResultToString( hr ).c_str()
				);
			}

			if( threadingCaps.DriverCommandLists == FALSE )
			{
				WriteToLog(_T( "Driver does not support hardware command lists, skipping deferred context test cases." ) );
				return RESULT_SKIP;
			}
		}

		SAFE_RELEASE( m_pD3DDeviceDeferredContext );
		if ( FAILED( hr = m_pD3DDevice->CreateDeferredContext(0, &m_pD3DDeviceDeferredContext) ))
		{
			WriteError
			(
				_T( "SetupD3D:  CreateDeferredContext failed with error %s." ),
				D3DHResultToString( hr ).c_str()
			);

			CleanupD3D();

			return RESULT_FAIL;
		}
	}

	//
	// Query the Direct3D device for its 11.1 interfaces.
	//

	assert( m_pD3DDevice );
	if ( FAILED( m_pD3DDevice->QueryInterface(__uuidof( ID3D11Device1 ), (void **) &m_pD3DDevice1 ) ))
	{
		WriteToLog( _T( "SetupD3D:  QueryInterface for ID3D11Device1 interface pointer failed." ) );

		CleanupD3D();

		return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
	}

	assert( m_pD3DDeviceImmediateContext );
	assert( m_pD3DDeviceImmediateContext1 == nullptr );
	if ( FAILED( m_pD3DDeviceImmediateContext->QueryInterface(__uuidof( ID3D11DeviceContext1 ), (void **) &m_pD3DDeviceImmediateContext1 ) ))
	{
		WriteToLog( _T( "SetupD3D:  QueryInterface for ID3D11DeviceContext1 interface pointer failed." ) );

		CleanupD3D();

		return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
	}

	if( m_pD3DDeviceDeferredContext )
	{
		assert( m_pD3DDeviceDeferredContext1 == nullptr );
		if ( FAILED( m_pD3DDeviceDeferredContext->QueryInterface(__uuidof( ID3D11DeviceContext1 ), (void **) &m_pD3DDeviceDeferredContext1 ) ))
		{
			WriteToLog( _T( "SetupD3D:  QueryInterface for ID3D11DeviceContext1 interface pointer failed." ) );

			CleanupD3D();

			return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
		}
	}

	assert( m_pD3DDevice );
	assert( m_pD3DDevice2 == nullptr );
	if ( FAILED( m_pD3DDevice->QueryInterface(__uuidof( ID3D11Device2 ), (void **) &m_pD3DDevice2 ) ))
	{
		WriteToLog( _T( "SetupD3D:  QueryInterface for ID3D11Device2 interface pointer failed.  Maximum interface available is ID3D11Device1." ) );

		// Since tests need to remain compatible with Windows 7 and Windows 8 this QI failure is permitted
		// by leaving m_pD3DDevice2, m_pD3DDeviceDeferredContext, and m_pD3DDeviceImmediateContext NULL;
		m_pD3DDevice2 = nullptr;
		m_pD3DDeviceDeferredContext2 = nullptr;
		m_pD3DDeviceImmediateContext2 = nullptr;
	}
	else
	{
		assert( m_pD3DDeviceImmediateContext );
		assert( m_pD3DDeviceImmediateContext2 == nullptr );
		if ( FAILED( m_pD3DDeviceImmediateContext->QueryInterface(__uuidof( ID3D11DeviceContext2 ), (void **) &m_pD3DDeviceImmediateContext2 ) ))
		{
			WriteToLog( _T( "SetupD3D:  QueryInterface for ID3D11DeviceContext2 interface pointer failed." ) );

			CleanupD3D();

			return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
		}

		if( m_pD3DDeviceDeferredContext )
		{
			assert( m_pD3DDeviceDeferredContext2 == nullptr );
			if ( FAILED( m_pD3DDeviceDeferredContext->QueryInterface(__uuidof( ID3D11DeviceContext2 ), (void **) &m_pD3DDeviceDeferredContext2 ) ))
			{
				WriteToLog( _T( "SetupD3D:  QueryInterface for ID3D11DeviceContext2 interface pointer failed." ) );

				CleanupD3D();

				return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
			}
		}
	}

	//
	// Query the Direct3D device for its debug interfaces.
	//

	if ( m_D3DOptions.Debug == true )
	{
		// DXGI debug function prototypes
		typedef HRESULT (WINAPI * PFNDXGIGetDebugInterface)(
			__in REFIID,
			__out void**);

		HMODULE hDXGIDebug = GetModuleHandle( _T( "DXGIDebug.dll" ) );

		if ( hDXGIDebug != nullptr )
		{
			PFNDXGIGetDebugInterface pDXGIGetDebugInterface = (PFNDXGIGetDebugInterface) GetProcAddress
			(
				hDXGIDebug,
				"DXGIGetDebugInterface"
			);

			if ( pDXGIGetDebugInterface != nullptr )
			{
				// Create a DXGI debug object.
				assert( m_pDXGIDebug == nullptr );
				HRESULT hGetDebugInterfaceResult = (*pDXGIGetDebugInterface)
				(
					IID_IDXGIDebug,
					(void **) &m_pDXGIDebug
				);

				if ( FAILED( hGetDebugInterfaceResult ) )
				{
					WriteToLog( _T( "SetupD3D:  DXGIGetDebugInterface for IDXGIDebug failed." ) );
				}


				// Create a DXGI info queue object.
				assert( m_pDXGIInfoQueue == nullptr );
				HRESULT hGetInfoQueueResult = (*pDXGIGetDebugInterface)
				(
					IID_IDXGIInfoQueue,
					(void **) &m_pDXGIInfoQueue
				);

				if ( FAILED( hGetInfoQueueResult ) )
				{
					WriteToLog( _T( "SetupD3D:  DXGIGetDebugInterface for IDXGIInfoQueue failed." ) );
				}
			}
		}

		assert( m_pD3DDebug == nullptr );
		if ( FAILED( m_pD3DDevice->QueryInterface(__uuidof( ID3D11Debug ), (void **) &m_pD3DDebug ) ))
		{
			WriteToLog( _T( "SetupD3D:  QueryInterface for ID3D11Debug interface pointer failed." ) );

			CleanupD3D();

			return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
		}

		assert( m_pD3DInfoQueue == nullptr );
		if ( FAILED( m_pD3DDevice->QueryInterface(__uuidof( ID3D11InfoQueue ), (void **) &m_pD3DInfoQueue ) ))
		{
			WriteToLog( _T( "SetupD3D:  QueryInterface for ID3D11InfoQueue interface pointer failed." ) );

			CleanupD3D();

			return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
		}
	}


	//
	// Cache our Direct3D options.
	//

	memcpy( &s_D3DOptionsCached, &m_D3DOptions, sizeof( m_D3DOptions ) );


	//
	// Note that devices are valid now.
	//

	m_bD3DValid = true;

	//
	// Create the tile pool manager
	//
	if(GetDevice2() && D3DTEST_TILED_RESOURCE_MODE_NEVER != GetTiledResourceMode())
	{
		D3D11_FEATURE_DATA_D3D11_OPTIONS1 featureData;
		GetDevice2()->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, &featureData, sizeof(featureData));
		if(featureData.TiledResourcesTier != D3D11_TILED_RESOURCES_NOT_SUPPORTED)
		{
			m_pTilePoolManager = new CTilePoolManager();
			m_pTilePoolManager->Create(GetDevice2());
		}
	}

	//
	// Setup the resource viewer
	//
	hr = m_resourceViewer.Setup(GetDevice1());
	if(FAILED(hr))
	{
		WriteToLog(FormatString(_T("CResourceViewer::Setup() failed! hr = %s"), D3DHResultToString(hr)));
		return RESULT_FAIL;
	}

	//
	// Turn on EmulateL8WithR8 on WARP if requested
	//
	bool bIsWARP = IsWARPAdapter( GetDXGIAdapter(0) );
	D3D_FEATURE_LEVEL fl = m_pD3DDevice->GetFeatureLevel();
	if( m_bEmulateR8WithL8 && bIsWARP && fl < D3D_FEATURE_LEVEL_10_0)
	{
		hr = m_pD3DDevice->QueryInterface( __uuidof(ID3D11DeviceTest), (void **)&m_pTestDevice );
		if( FAILED( hr ) || !m_pTestDevice )
		{
			WriteToLog(FormatString(_T("QueryInterface failed for ID3D11DeviceTest. hr = %s"), D3DHResultToString(hr)));
			CleanupD3D();
			return RESULT_FAIL;
		}
		
		if( FAILED( hr = m_pTestDevice->EmulateR8WithL8( TRUE ) ) )
		{
			WriteToLog(FormatString(_T("EmulateR8WithL8 failed for ID3D11DeviceTest. hr = %s"), D3DHResultToString(hr)));
			CleanupD3D();
			return RESULT_FAIL;
		}

		// Write a message in the log to indicate we are now emulating
		WriteToLog( _T("Emulating R8 With L8...") );
	}

	//
	// Create the switch-to-ref layer.
	//
	// BUGBUG - chrisza: SwitchToRef won't be implemented in M1
	//

	/*
	if( m_D3DOptions.forceSwitchToRef )
	{
		hr = m_pD3DDevice->CreateSwitchToRef( &m_pD3DSwitchToRef );
		if( FAILED( hr ) )
		{
			WriteToLog
			(
				_T( "SetupD3D:  CreateSwitchToRef failed with error %s." ),
				D3DHResultToString( hr ).c_str()
			);

			return RESULT_SKIP;
		}

		m_pD3DSwitchToRef->SetUseRef( true );
	}
	*/


	return RESULT_PASS;
}

//--------------------------------------------------------------------

TEST_RESULT CD3D11TestFramework::CheckForDriverErrors()
{
	if (!m_D3DOptions.DriverVerifier)
	{
		return RESULT_PASS;
	}

	HMODULE hDriverVerifier = GetModuleHandle("D3DDriverVerifier.dll");
	if (!hDriverVerifier)
	{
		WriteToLog( L"CheckForDriverErrors:  Driver Verifier DLL not loaded." );
		return RESULT_PASS;
	}

	PFNRETRIEVEVERIFIERINTERFACE pfnRetrieveVerifierInterface = reinterpret_cast<PFNRETRIEVEVERIFIERINTERFACE>(
		GetProcAddress(hDriverVerifier, "RetrieveVerifierInterface"));
	if (!pfnRetrieveVerifierInterface)
	{
		WriteToLog( L"CheckForDriverErrors:  Driver Verifier DLL doesn't have required export." );
		return RESULT_FAIL;
	}

	ID3DDriverVerifier* pVerifierIface = nullptr;
	if (FAILED(pfnRetrieveVerifierInterface(IID_PPV_ARGS(&pVerifierIface))))
	{
		WriteToLog( L"CheckForDriverErrors:  ID3DDriverVerifier interface not available." );
		return RESULT_FAIL;
	}

	ID3DDriverVerifierMessageIterator* pIterator = nullptr;
	if (FAILED(pVerifierIface->GetMessageIterator(GetDevice(), &pIterator)))
	{
		WriteToLog( L"CheckForDriverErrors:  Failed to retrieve message iterator." );
        pVerifierIface->Release();
		return RESULT_FAIL;
	}


	UINT NumMessages = pIterator->GetTotalMessages();
	UINT NumErrors = 0;

	if (NumMessages != 0)
	{
		WriteToLog(L"CheckForDriverErrors:  D3D11 Driver Verifier detected errors:" );
	}

	// Nonzero errors are present, let's dump them to the log
	for (UINT64 i = 0; i < NumMessages; ++i)
	{
		SIZE_T MessageSize = 0;
		D3D11_MESSAGE_SEVERITY Severity = D3D11_MESSAGE_SEVERITY_ERROR;
		if (FAILED(pIterator->RetrieveMessage(&Severity, nullptr, &MessageSize, nullptr)))
		{
			WriteToLog( L"CheckForDriverErrors:  ID3DDriverVerifierMessageIterator::RetrieveMessage failed retrieving message size." );
            ++NumErrors;
			break;
		}

		if (Severity == D3D11_MESSAGE_SEVERITY_ERROR || Severity == D3D11_MESSAGE_SEVERITY_CORRUPTION)
		{
			// Now it's definitely a test failure
			++NumErrors;
		}

		char *pMessage = new char[MessageSize];
		if (FAILED(pIterator->RetrieveMessage(&Severity, nullptr, &MessageSize, pMessage)))
		{
			WriteToLog( L"CheckForDriverErrors:  ID3DDriverVerifierMessageIterator::RetrieveMessage failed retrieving message." );
			delete pMessage;
			break;
		}

		WriteToLog( str2wstr(pMessage ).c_str());

		delete pMessage;
	}

	pVerifierIface->ClearMessages(GetDevice());
    pIterator->Release();
    pVerifierIface->Release();

	return NumErrors > 0 ? RESULT_FAIL : RESULT_PASS;
}


///////////////////////////////////////////////////////
// Function:  ReportLiveDXGIObjects
//
// Purpose:
// Have the DXGI debug object report DXGI and Direct3D
// objects that are still live at the end of the test
// (and presumably leaked).
///////////////////////////////////////////////////////
void CD3D11TestFramework::ReportLiveDXGIObjects()
{
	//
	// Initialize locals.
	//

	UINT64 QueuedMessages = 0;


	//
	// Validate members and state.
	//

	if ( m_pDXGIDebug == nullptr )
	{
		return;
	}

	if ( m_pDXGIInfoQueue == nullptr )
	{
		return;
	}


	if ( QueuedMessages > 0 )
	{
		WriteToLog
		(
			L"One or more Direct3D or DXGI objects are still alive at the end of the test (and thus have likely been leaked):"
		);
	};


	//
	// Report live Direct3D 11 and DXGI objects.
	//

	m_pDXGIInfoQueue->ClearStoredMessages( DXGI_DEBUG_ALL );

	m_pDXGIDebug->ReportLiveObjects
	(
		DXGI_DEBUG_ALL,
		DXGI_DEBUG_RLO_DETAIL
	);

	QueuedMessages = m_pDXGIInfoQueue->GetNumStoredMessages( DXGI_DEBUG_ALL );
	for
	(
		UINT64 iCurrentMessage = 0;
		iCurrentMessage < QueuedMessages;
		++iCurrentMessage
	)
	{
		// Get the message length.
		SIZE_T nCurrentMessage = 0;
		HRESULT hGetMessageLengthResult = m_pDXGIInfoQueue->GetMessage
		(
			DXGI_DEBUG_ALL,
			iCurrentMessage,
			nullptr,
			&nCurrentMessage
		);

		if ( FAILED( hGetMessageLengthResult ) )
		{
			LogError
			(
				__FILEW__,
				__LINE__,
				L"GetMessage to determine message length failed.",
				hGetMessageLengthResult,
				ERRORCODE_TYPE_HRESULT,
				WindowsTest::gFCUnspecified
			);

			continue;
		};

		assert(nCurrentMessage > 0);
		if (nCurrentMessage > 0)
		{
			// Allocate a buffer to hold the message.
			BYTE * pCurrentMessageBuffer = new(std::nothrow) BYTE[nCurrentMessage];
			if (pCurrentMessageBuffer == nullptr)
			{
				LogError
				(
					__FILEW__,
					__LINE__,
					L"Failed to allocate DXGI info queue message buffer.",
					E_OUTOFMEMORY,
					ERRORCODE_TYPE_HRESULT,
					WindowsTest::gFCUnspecified
				);

				return;
			}


			// Get the message.
			DXGI_INFO_QUEUE_MESSAGE * pCurrentMessage = reinterpret_cast< DXGI_INFO_QUEUE_MESSAGE * >(pCurrentMessageBuffer);

			HRESULT hGetMessageResult = m_pDXGIInfoQueue->GetMessage
			(
				DXGI_DEBUG_ALL,
				iCurrentMessage,
				pCurrentMessage,
				&nCurrentMessage
			);

			if ( FAILED( hGetMessageResult ) )
			{
				LogError
				(
					__FILEW__,
					__LINE__,
					L"GetMessage to retrieve message contents failed.",
					hGetMessageResult,
					ERRORCODE_TYPE_HRESULT,
					WindowsTest::gFCUnspecified
				);

				delete [] pCurrentMessageBuffer;
				pCurrentMessageBuffer = nullptr;

				continue;
			};


			// Log the message.
			WriteToLogF
			(
				L"DXGI message (category %s, severity %s, ID %u):  %S.",
				ToString(pCurrentMessage->Category).c_str(),
				ToString(pCurrentMessage->Severity).c_str(),
				pCurrentMessage->ID,
				pCurrentMessage->pDescription
			);


			// Delete the message buffer.
			pCurrentMessage = nullptr;

			delete [] pCurrentMessageBuffer;
			pCurrentMessageBuffer = nullptr;
		}
	}
}


//--------------------------------------------------------------------
void CD3D11TestFramework::CleanupD3D()
{
	//
	// Release render targets.
	//

	CleanupRenderTargets();


	//
	// Release Direct3D device-related interface pointers.
	//

	m_resourceViewer.Cleanup();

	// Cleanup any tiled resources no longer in use
	if(m_pTilePoolManager)
	{
		m_pTilePoolManager->CleanupResources();
		SAFE_DELETE(m_pTilePoolManager);
	}

	// disable EmulateR8WithL8
	if( m_pTestDevice )
	{
		HRESULT hr = m_pTestDevice->EmulateR8WithL8( FALSE );
		if( FAILED( hr ) )
		{
			WriteToLog(FormatString(_T("Failed to disable EmulateR8WithL8 for ID3D11DeviceTest. hr = %s"), D3DHResultToString(hr)));
		}
			// Write a message in the log to indicate we are now stopping emulation
		WriteToLog( _T("Stopping emulation of R8 With L8...") );
	}

	SAFE_RELEASE( m_pTestDevice );
	SAFE_RELEASE( m_pD3DDevice1 );
	SAFE_RELEASE( m_pD3DDeviceImmediateContext1 );
	SAFE_RELEASE( m_pD3DDeviceDeferredContext1 );
	SAFE_RELEASE( m_pD3DDevice2 );
	SAFE_RELEASE( m_pD3DDeviceImmediateContext2 );
	SAFE_RELEASE( m_pD3DDeviceDeferredContext2 );
	SAFE_RELEASE( m_pD3DDevice );
	SAFE_RELEASE( m_pD3DDeviceImmediateContext );
	SAFE_RELEASE( m_pD3DDeviceDeferredContext );
	
	// BUGBUG - chrisza: SwitchToRef won't be implemented in M1
	//SAFE_RELEASE( m_pD3DSwitchToRef );


	//
	// Unload software rasterizers.
	//

	UnloadReferenceRasterizer();
	UnloadSoftwareRasterizer();


	//
	// Release debug objects and info queues.
	//

	SAFE_RELEASE( m_pD3DInfoQueue );
	SAFE_RELEASE( m_pD3DDebug );


	//
	// Note that Direct3D devices are no longer valid.
	//

	m_bD3DValid = false;
}


//--------------------------------------------------------------------
TEST_RESULT CD3D11TestFramework::SetupDXGIAndD3D()
{
	//	
	// Determine the device type.
	//

	D3D_DRIVER_TYPE DriverType = m_D3DOptions.SrcDriverType;
	if ( m_D3DOptions.proxyDriver == true )
	{
		DriverType = D3D_DRIVER_TYPE_SOFTWARE;
	};


	//
	// Create our DXGI adapter if necessary.
	//
	// The only time when pAdapter can be explicitly specified in D3D11CreateDevice is when
	// creating D3D_DRIVER_TYPE_UNKNOWN devices. Here we allow the creation of an explicitly-set adapter
	// even for D3D_DRIVER_TYPE_HARDWARE because we convert D3D_DRIVER_TYPE_HARDWARE to D3D_DRIVER_TYPE_UNKNOWN
	// just before creating the device anyway.
	//
	// pAdapter cannot be specified in D3D11CreateDevice for any software driver types (ie: WARP, REF and SOFTWARE),
	// so we skip SetupDXGI in those cases. We also skip SetupDXGI when proxyDriver is being used (which
	// we assume to be SOFTWARE driver type above).
	//

	if ( m_bDXGIValid != true )
	{
		switch ( DriverType )
		{
			case D3D_DRIVER_TYPE_UNKNOWN:
			case D3D_DRIVER_TYPE_HARDWARE:
			case D3D_DRIVER_TYPE_WARP:
			{
				// Create DXGI resources.
				TEST_RESULT SetupDXGIResult = SetupDXGI();
				if ( SetupDXGIResult != RESULT_PASS )
				{
					return SetupDXGIResult;
				}
			} break;


			default:
			{
				// Free DXGI resources now, then QueryAdapter later.
				CleanupDXGI();
			} break;
		};
	};


	//
	// Create our Direct3D device using the adapter created above.
	//

	if ( m_bD3DValid != true )
	{
		TEST_RESULT SetupD3DResult = SetupD3D();
		if ( SetupD3DResult != RESULT_PASS )
		{
			return SetupD3DResult;
		}
	};


	//
	// If we just created a software driver type, we used a NULL adapter in D3D11CreateDevice.
	// Hence we need to query for the actual adapter from the device itself. The DXGI_ADAPTER_DESC for 
	// the adapter returned in these cases is going contain information coming from a two sources:
	//
	// - Information hardcoded to software driver types:
	//   Description == Software Adapter, DeviceID == VendorID == SubSysID == Revision == 0
	//
	// - Information coming from the first adapter in EnumAdapters:
	//   DedicatedVideoMemory, DedicatedSystemMemory, SharedSystemMemory, AdapterLuid, Flags
	//

	switch ( DriverType )
	{
		case D3D_DRIVER_TYPE_UNKNOWN:
		case D3D_DRIVER_TYPE_HARDWARE:
		{
			// Do nothing in these cases - we have already chosen 
			// an adapter explicitly in SetupDXGI.
		} break;


		default:
		{
			// Query the adapter from the created device.

			if ( m_bDXGIValid != true )
			{
				TEST_RESULT QueryAdapterResult = QueryAdapter();
				if ( QueryAdapterResult != RESULT_PASS )
				{
					return QueryAdapterResult;
				}
			}
		} break;
	};


	return RESULT_PASS;
}


//--------------------------------------------------------------------
TEST_RESULT CD3D11TestFramework::QueryAdapter()
{
	//
	// Initialize locals.
	//

	TEST_RESULT OverallResult = RESULT_PASS;

	IDXGIDevice * pDXGIDevice = nullptr;
	IDXGIAdapter * pDXGIAdapter = nullptr;
	IDXGIOutput * pDXGIOutput = nullptr;


	//
	// Validate members and state.
	//

	assert( m_pD3DDevice != nullptr );
	if ( m_pD3DDevice == nullptr )
	{
		WriteToLog
		(
			_T( "QueryAdapter:  m_pD3DDevice is NULL." )
		);

		if ( GetTestMode() == WGFTEST_MODE_WHQL )
		{
			OverallResult = RESULT_SKIP;
		}
		else
		{
			OverallResult = RESULT_FAIL;
		};

		goto Cleanup;
	};


	if ( m_pAdapters == nullptr )
	{
		//
		// Determine how many devices the test needs, and allocate
		// a corresponding number of DXGI adapters and outputs.
		//

		m_uDeviceCount = m_D3DOptions.SrcOnly ? 1 : 2;
		m_pAdapters = new IDXGIAdapter * [ m_uDeviceCount ]; 
		ZeroMemory(m_pAdapters, sizeof(IDXGIAdapter *) * m_uDeviceCount);

		assert( m_pAdapterOutputs == nullptr );
		m_pAdapterOutputs = new IDXGIOutput * [ m_uDeviceCount ]; 
		ZeroMemory(m_pAdapterOutputs, sizeof(IDXGIOutput *) * m_uDeviceCount);
	};


	assert( m_pAdapters[ 0 ] == nullptr );
	if ( m_pAdapters[ 0 ] != nullptr )
	{
		WriteToLog
		(
			_T( "QueryAdapter:  Adapter slot 0 is occupied." )
		);

		if ( GetTestMode() == WGFTEST_MODE_WHQL )
		{
			OverallResult = RESULT_SKIP;
		}
		else
		{
			OverallResult = RESULT_FAIL;
		};

		goto Cleanup;
	};


	//
	// Query the Direct3D device for its IDXGIDevice interface pointer.
	//

	HRESULT hQueryForIDXGIDeviceResult = m_pD3DDevice->QueryInterface
	(
		IID_IDXGIDevice,
		(void **) &pDXGIDevice
	);

	if ( FAILED( hQueryForIDXGIDeviceResult ) )
	{
		WriteToLogF
		(
			_T( "QueryAdapter:  QueryInterface for IDXGIDevice failed with error %s." ),
			D3DHResultToString( hQueryForIDXGIDeviceResult ).c_str()
		);


		if ( GetTestMode() == WGFTEST_MODE_WHQL )
		{
			OverallResult = RESULT_SKIP;
		}
		else
		{
			OverallResult = RESULT_FAIL;
		};

		goto Cleanup;
	};

	if ( pDXGIDevice == nullptr )
	{
		WriteToLog
		(
			_T( "QueryAdapter:  QueryInterface for IDXGIDevice returned NULL device pointer." )
		);

		if ( GetTestMode() == WGFTEST_MODE_WHQL )
		{
			OverallResult = RESULT_SKIP;
		}
		else
		{
			OverallResult = RESULT_FAIL;
		};

		goto Cleanup;
	};


	//
	// Get the DXGI device's parent adapter.
	//

	HRESULT hGetDXGIAdapterResult = pDXGIDevice->GetAdapter( &m_pAdapters[ 0 ] );

	if ( FAILED( hGetDXGIAdapterResult ) )
	{
		WriteToLogF
		(
			_T( "QueryAdapter:  GetAdapter failed with error %s." ),
			D3DHResultToString( hGetDXGIAdapterResult ).c_str()
		);

		if ( GetTestMode() == WGFTEST_MODE_WHQL )
		{
			OverallResult = RESULT_SKIP;
		}
		else
		{
			OverallResult = RESULT_FAIL;
		};

		goto Cleanup;
	};

	if ( m_pAdapters[ 0 ] == nullptr )
	{
		WriteToLog
		(
			_T( "QueryAdapter:  GetAdapter returned a NULL adapter pointer." )
		);

		if ( GetTestMode() == WGFTEST_MODE_WHQL )
		{
			OverallResult = RESULT_SKIP;
		}
		else
		{
			OverallResult = RESULT_FAIL;
		};

		goto Cleanup;
	};


	//
	// Get the DXGI adapter's parent factory.
	//

	HRESULT hGetDXGIFactoryResult = m_pAdapters[ 0 ]->GetParent
	(
		IID_IDXGIFactory,
		(void **) &m_DXGIFactory
	);

	if ( FAILED( hGetDXGIFactoryResult ) )
	{
		WriteToLogF
		(
			_T( "QueryAdapter:  GetParent on DXGI adapter failed with %s." ),
			D3DHResultToString( hGetDXGIFactoryResult ).c_str()
		);

		if ( GetTestMode() == WGFTEST_MODE_WHQL )
		{
			OverallResult = RESULT_SKIP;
		}
		else
		{
			OverallResult = RESULT_FAIL;
		};

		goto Cleanup;
	};

	if ( m_DXGIFactory.pFactory == nullptr )
	{
		WriteToLog
		(
			_T( "QueryAdapter:  GetParent returned a NULL factory pointer." )
		);

		if ( GetTestMode() == WGFTEST_MODE_WHQL )
		{
			OverallResult = RESULT_SKIP;
		}
		else
		{
			OverallResult = RESULT_FAIL;
		};

		goto Cleanup;
	};


	//
	// Set the adapter index.
	//
	// Since this function is always dealing with a software
	// adapter, and because software adapters are not
	// enumerated from a factory by an ordinal in the way
	// that hardware adapters are, this will always be zero.
	//

	m_DXGIOptions.AdapterID[ 0 ] = 0;


	//
	// Find the output index.
	//

	if (!IsSession0())
	{
		for
		(
			UINT m = 0;
			m_pAdapters[ 0 ]->EnumOutputs( m, &pDXGIOutput ) != DXGI_ERROR_NOT_FOUND;
			++m
		)
		{
			if ( pDXGIOutput != nullptr )
			{
				m_pAdapterOutputs[ 0 ] = pDXGIOutput;
				pDXGIOutput = nullptr;

				m_DXGIOptions.OutputID[ 0 ] = m;
				break;
			}

			SAFE_RELEASE( pDXGIOutput );
		};
	};


	memcpy( &s_DXGIOptionsCached, &m_DXGIOptions, sizeof( m_DXGIOptions ) );


	m_bDXGIValid = true;

	OverallResult = RESULT_PASS;


Cleanup:
	SAFE_RELEASE( pDXGIOutput );
	SAFE_RELEASE( pDXGIAdapter );
	SAFE_RELEASE( pDXGIDevice );


	if ( OverallResult != RESULT_PASS )
	{
		CleanupDXGI();
	};

	return OverallResult;
}


//--------------------------------------------------------------------

TEST_RESULT CD3D11TestFramework::SetupRenderTargets()
{
	CD3D11Test *pTest = dynamic_cast< CD3D11Test *>( GetCurrentTest() );

	assert( pTest && "dynamic_cast failed" );

	UINT numRenderWindowsPerDevice = pTest->GetNumRenderWindows();

	// Make sure the shell has the desired number of test windows.
	if( GetShell() )
	{
		GetShell()->SetNumDeviceWindows( numRenderWindowsPerDevice * m_uDeviceCount );
	}

	if ( numRenderWindowsPerDevice > 0 )
	{
		assert( m_uDeviceCount > 0 );

		// Create arrays of swap chain and render target pointers.
		m_pSwapChains = new IDXGISwapChain1 *[ numRenderWindowsPerDevice * m_uDeviceCount ];
		ZeroMemory(m_pSwapChains, sizeof(IDXGISwapChain1 *) * numRenderWindowsPerDevice * m_uDeviceCount);
		m_uSwapChainCount = numRenderWindowsPerDevice * m_uDeviceCount;
		m_pRTViews = new ID3D11RenderTargetView *[ numRenderWindowsPerDevice ];
		ZeroMemory(m_pRTViews, sizeof(ID3D11RenderTargetView *) * numRenderWindowsPerDevice);
	}

	// The default behavior is to create a render target and RT view for each render window.
	// The developer can however choose to let the framework manage separate render targets
	// by setting RenderTargetViewDesc.Format to something other than DXGI_FORMAT_UNKNOWN (the default value).
	// This will cause the framework to create render targets separate from the swap chains.
	bool createRTFromSwapChains = m_RenderTargetOptions.RenderTargetViewDesc.Format == DXGI_FORMAT_UNKNOWN;

	if( !m_RenderTargetOptions.NoRenderTargets && createRTFromSwapChains )
	{
		m_pRTTextures = new ID3D11Texture2D *[ numRenderWindowsPerDevice ];
		ZeroMemory(m_pRTTextures, sizeof(ID3D11Texture2D *) * numRenderWindowsPerDevice);
	}

	if( !m_RenderTargetOptions.NoSwapChains )
	{	
		// For each render window create a swap chain
		for( int i = numRenderWindowsPerDevice - 1; i >= 0; i-- )
		{
			HWND hWnd;
			bool bFullScreen = pTest->FullScreen();

			// BUGBUG - Only one swap chain can be full screen at at time.  For now
			// only the first swap chain can be full screen.
			if( bFullScreen )
			{
				hWnd = GetShell()->GetMainHWnd(); // BUGBUG - I don't like this option.  Consider creating popup window.
				m_DXGIFactory.pFactory->MakeWindowAssociation( hWnd, DXGI_MWA_NO_ALT_ENTER );
			}
			else
			{
				if( !GetShell() )
				{
					WriteToLog( "SetupRenderTargets:  Unable to use windowed mode without a shell" );
					CleanupRenderTargets();
					return RESULT_SKIP;
				}
			}

			// For each device
			for
			(
				UINT j = 0;
				j < m_uDeviceCount;
				++j
			)
			{
				hWnd = GetShell()->GetDeviceRenderWindow( i * m_uDeviceCount + j );
				m_RenderTargetOptions.OutputWindow = hWnd;
				m_RenderTargetOptions.Windowed = bFullScreen && j == 0 ? false : true;

				assert( m_pSwapChains[ i * m_uDeviceCount + j ] == nullptr );
				HRESULT hr = m_DXGIFactory.pFactory2->CreateSwapChainForHwnd
				(
					m_pD3DDevice,
					m_RenderTargetOptions.OutputWindow,
					&m_RenderTargetOptions.SwapChainDesc,
					m_RenderTargetOptions.Windowed ? nullptr : &m_RenderTargetOptions.FullscreenDesc,
					nullptr,
					&m_pSwapChains[ i * m_uDeviceCount + j ]
				);

				if ( FAILED( hr ) )
				{
					WriteError
					(
						_T("SetupRenderTargets:  CreateSwapChain on swap chain %u failed with error %s."),
						i * m_uDeviceCount + j,
						D3DHResultToString( hr ).c_str()
					);

					CleanupRenderTargets();

					return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
				}

				// Set the fullscreen state on the swap chain
				// BUGBUG - How do we select separate outputs for separate swap chains?
				m_pSwapChains[ i*m_uDeviceCount + j ]->SetFullscreenState( bFullScreen, bFullScreen?m_pAdapterOutputs[j]:nullptr );
			}

			if ( !m_RenderTargetOptions.NoRenderTargets && createRTFromSwapChains )
			{
				IDXGISwapChain1 *pSwapChain = *(m_pSwapChains + i*m_uDeviceCount);

				HRESULT hGetBufferResult = pSwapChain->GetBuffer
				(
					0,
					__uuidof( ID3D11Texture2D ),
					(void **) &m_pRTTextures[ i ]
				);

				if ( FAILED( hGetBufferResult ) )		
				{
					WriteError
					(
						_T( "SetupRenderTargets:  GetBuffer 0 on swap chain %u failed with error %s." ),
						i * m_uDeviceCount,
						D3DHResultToString( hGetBufferResult ).c_str()
					);

					CleanupRenderTargets();
					return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
				}

				// Get the render target view from the render target texture
				D3D11_RENDER_TARGET_VIEW_DESC *pRTViewDesc = m_RenderTargetOptions.UseDefaultRenderTargetView ? nullptr : &m_RenderTargetOptions.RenderTargetViewDesc;

				assert( m_pRTViews[ i ] == nullptr );
				if( FAILED( m_pD3DDevice->CreateRenderTargetView( m_pRTTextures[ i ], pRTViewDesc, &m_pRTViews[ i ] ) ) )
				{
					WriteToLog( _T( "SetupRenderTargets:  CreateRenderTargetView failed to get view from render target textures." ) );

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
			case D3D11_RTV_DIMENSION_BUFFER:
				m_pRTBuffers = new ID3D11Buffer *[1];
				m_pRTBuffers[0] = nullptr;
				if( FAILED( m_pD3DDevice->CreateBuffer( &m_RenderTargetOptions.RenderTargetBufferDesc, nullptr, &m_pRTBuffers[0] ) ) )
				{
					WriteError( _T( "SetupRenderTargets:  CreateBuffer on render target buffer failed." ) );
					CleanupRenderTargets();
					return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
				}
				break;

			case D3D11_RTV_DIMENSION_TEXTURE1D:
			case D3D11_RTV_DIMENSION_TEXTURE1DARRAY:
				m_pRTTextures1D = new ID3D11Texture1D *[1];
				m_pRTTextures1D[0] = nullptr;
				assert( m_RenderTargetOptions.RenderTargetTextureDesc1D.Format != DXGI_FORMAT_UNKNOWN );
				if( FAILED( m_pD3DDevice->CreateTexture1D( &m_RenderTargetOptions.RenderTargetTextureDesc1D, nullptr, &m_pRTTextures1D[0] ) ) )
				{
					WriteError( _T( "SetupRenderTargets:  CreateTexture1D on render target failed." ) );
					CleanupRenderTargets();
					return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
				}
				break;

			case D3D11_RTV_DIMENSION_TEXTURE2D:
			case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
			case D3D11_RTV_DIMENSION_TEXTURE2DMS:
			case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
				assert( m_RenderTargetOptions.RenderTargetTextureDesc.Format != DXGI_FORMAT_UNKNOWN );
				m_pRTTextures = new ID3D11Texture2D *[1];
				m_pRTTextures[0] = nullptr;
				if( FAILED( m_pD3DDevice->CreateTexture2D( &m_RenderTargetOptions.RenderTargetTextureDesc, nullptr, &m_pRTTextures[0] ) ) )
				{
					WriteError( _T( "SetupRenderTargets:  CreateTexture2D on render target failed." ) );
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
		D3D11_RENDER_TARGET_VIEW_DESC *pRTViewDesc = m_RenderTargetOptions.UseDefaultRenderTargetView ? nullptr : &m_RenderTargetOptions.RenderTargetViewDesc;

		assert( m_pRTViews[ 0 ] == nullptr );
		if( FAILED( m_pD3DDevice->CreateRenderTargetView( m_pRTTextures[ 0 ], pRTViewDesc, &m_pRTViews[ 0 ] ) ) )
		{
			WriteToLog( _T( "SetupRenderTargets:  CreateRenderTargetView failed to get view from render target textures." ) );

			CleanupRenderTargets();

			return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
		}
	}

	// Create depth/stencil textures and views
	if
	(
		m_RenderTargetOptions.NoDepthStencils || 
		( !m_RenderTargetOptions.NoRenderTargets && m_RenderTargetOptions.RenderTargetViewDesc.ViewDimension == D3D11_RTV_DIMENSION_BUFFER )
	)
	{
		m_pDepthStencil = nullptr;
		m_pDSView = nullptr;
	}
	else
	{
		switch ( m_RenderTargetOptions.DepthStencilViewDesc.ViewDimension )
		{
			case D3D11_DSV_DIMENSION_TEXTURE1D:
				assert( m_pDepthStencil1D == nullptr );
				if( FAILED( m_pD3DDevice->CreateTexture1D( &m_RenderTargetOptions.DepthTextureDesc1D, nullptr, &m_pDepthStencil1D ) ) )
				{
					WriteToLog( _T( "SetupRenderTargets:  CreateTexture1D on depth/stencil textures failed." ) );

					CleanupRenderTargets();

					return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
				}
				break;

			default:
			case D3D11_DSV_DIMENSION_TEXTURE2D:
			case D3D11_DSV_DIMENSION_TEXTURE2DMS:
			case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
			case D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY:
				if (m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0)
					m_RenderTargetOptions.DepthTextureDesc.MipLevels = 1;

				assert( m_pDepthStencil == nullptr );
				if( FAILED( m_pD3DDevice->CreateTexture2D( &m_RenderTargetOptions.DepthTextureDesc, nullptr, &m_pDepthStencil ) ) )
				{
					WriteToLog( _T( "SetupRenderTargets:  CreateTexture2D on depth/stencil textures failed." ) );

					CleanupRenderTargets();

					return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
				}
				break;
		}

		// Create the ID3D11DepthStencilView
		D3D11_DEPTH_STENCIL_VIEW_DESC *pDSVDesc = m_RenderTargetOptions.UseDefaultDepthStencilView ? nullptr : &m_RenderTargetOptions.DepthStencilViewDesc;

		assert( m_pDSView == nullptr );
		if( FAILED( m_pD3DDevice->CreateDepthStencilView( m_pDepthStencil, pDSVDesc, &m_pDSView ) ) )
		{
			WriteToLog( _T( "SetupRenderTargets:  CreateDepthStencilView failed to create view for depth stencil textures." ) );

			CleanupRenderTargets();

			return GetTestMode() == WGFTEST_MODE_WHQL ? RESULT_SKIP : RESULT_FAIL;
		}
	}

	GetEffectiveContext()->OMSetRenderTargets( createRTFromSwapChains ? numRenderWindowsPerDevice : 1, m_pRTViews, m_pDSView );

	// Set default viewports
	D3D11_VIEWPORT viewports[] =
	{
		{ 
			0, 
				0, 
				(FLOAT) m_RenderTargetOptions.SwapChainDesc.Width, 
				(FLOAT) m_RenderTargetOptions.SwapChainDesc.Height, 
				0.f, 
				1.f
		}, 
	};

	GetEffectiveContext()->RSSetViewports( 1, viewports );

	memcpy( &s_RenderTargetOptionsCached, &m_RenderTargetOptions, sizeof( m_RenderTargetOptions ) );
	m_bRenderTargetValid = true;

	return RESULT_PASS;
}


//--------------------------------------------------------------------

void CD3D11TestFramework::CleanupRenderTargets()
{
	UINT uNumRTPerDevice = m_uDeviceCount ? (m_uSwapChainCount / m_uDeviceCount) : 0;

	if ( m_pD3DDeviceImmediateContext ) 
	{
		m_pD3DDeviceImmediateContext->OMSetRenderTargets( 0, nullptr, nullptr );
	}

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
		SAFE_RELEASE( m_pSwapChains[ i ] );
	}

	SAFE_DELETE_ARRAY( m_pSwapChains );

	m_uSwapChainCount = 0;

	SAFE_RELEASE( m_pDepthStencil );
	SAFE_RELEASE( m_pDepthStencil1D );
	SAFE_RELEASE( m_pDSView );

	m_bRenderTargetValid = false;
}


//--------------------------------------------------------------------

TEST_RESULT CD3D11TestFramework::SetupTest()
{
	if (m_D3DOptions.FeatureLevel > m_D3DOptions.wMaxFeatureLevel || m_D3DOptions.FeatureLevel < m_D3DOptions.wMinFeatureLevel)
	{
		WriteToLog("The requested Direct3D feature level is not supported by this test.");
		return RESULT_SKIP;
	}

	CD3D11Test *pTest = dynamic_cast< CD3D11Test *>( GetCurrentTest() );
	assert( pTest && "Test object class is not derived from CD3D11Test." );

	m_bSetupCalled = false;
	m_bDXGIValid = false;

	TEST_RESULT tr = RefreshGraphics();

	return tr;
}


void CD3D11TestFramework::PreAllocateTest()
{
	// Restore device options to global settings
	memcpy( &m_DXGIOptions, &m_DXGIOptionsGlobal, sizeof( m_DXGIOptions ) );
	memcpy( &m_D3DOptions, &m_D3DOptionsGlobal, sizeof( m_D3DOptions ) );
	memcpy( &m_RenderTargetOptions, &m_RenderTargetOptionsGlobal, sizeof( m_RenderTargetOptions ) );
	m_bDXGIValid = false; // Force recycle of all devices
}


void CD3D11TestFramework::PreGenerateTestCase()
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


void CD3D11TestFramework::PostCleanupTestCase()
{
	// necessary to ensure resource destruction
	ID3D11DeviceContext *pContext = GetImmediateContext();

	if (pContext)
		pContext->Flush();

	// Cleanup any tiled resources no longer in use
	if(m_pTilePoolManager)
		m_pTilePoolManager->CleanupResources();

	if ( (m_bLogDebug || (m_pD3DDevice && m_pD3DDevice->GetDeviceRemovedReason() != S_OK)) && m_pD3DInfoQueue)
	{
		ID3D11InfoQueue* pFirstInfoQueue = m_pD3DInfoQueue;
		SIZE_T sizeBlob = 1024;
		SIZE_T sizeMsg = 0;
		const UINT64 numMessages = pFirstInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
		BYTE* blob = nullptr;
		D3D11_MESSAGE *pMsg = nullptr;

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

			pMsg = (D3D11_MESSAGE *)blob;
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

TEST_RESULT CD3D11TestFramework::PreSetupTestCase()
{
	TEST_RESULT tr = RESULT_PASS;

	if( !GetCurrentTest() )
		return RESULT_NONE;

	tr = RefreshGraphics();
	if (tr == RESULT_NONE)
		tr = RESULT_PASS;

	if(!m_viewResources)
	{
		// Destroy all resource viewers if m_viewResources is false (may have been changed by the user)
		m_resourceViewer.DestroyAll();
	}

	if(m_pTilePoolManager)
	{
		tr = m_pTilePoolManager->ShrinkTilePoolIfPossible();
	}

	return tr;
}


//--------------------------------------------------------------------
bool CD3D11TestFramework::UpdateGraphicsValidFlags()
{
	if( m_pD3DDevice && m_pD3DDevice->GetDeviceRemovedReason() != S_OK )
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

TEST_RESULT CD3D11TestFramework::RefreshGraphics()
{
	TEST_RESULT tr = RESULT_NONE;
	m_RefreshGraphicsFlags = 0;

	if (UpdateGraphicsValidFlags() || !m_bSetupCalled)
	{
		CD3D11Test *pTest = dynamic_cast< CD3D11Test *>( GetCurrentTest() );
		assert( pTest );
		bool bInterfacesChanged = false;
		bool bCleanupCalled = false;

		if( !m_bDXGIValid || !m_bD3DValid )
		{
			bool updatingDXGI(false);
			bool updatingD3D(false);
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
				updatingDXGI = true;
			}
			if( !m_bD3DValid )
			{
				// Call the test's Cleanup() function since we will be 
				// calling its Setup() function again
				if (m_bSetupCalled && !bCleanupCalled)
				{
					pTest->Cleanup();
				}

				CleanupD3D();
				updatingD3D = true;
			}

			tr = SetupDXGIAndD3D();
			if( tr != RESULT_PASS )
			{
				return tr;
			}

			bInterfacesChanged = true;
			if( updatingDXGI )
			{
				m_RefreshGraphicsFlags |= DXGI_RECYCLED;
			}
			if( updatingD3D )
			{
				m_RefreshGraphicsFlags |= D3D_RECYCLED;
			}
		}

		// Check whether D3D settings have changed.
		if( !m_bRenderTargetValid )
		{
			CleanupRenderTargets();
			tr = SetupRenderTargets();
			if( tr != RESULT_PASS )
				return tr;
			bInterfacesChanged = true;
			m_RefreshGraphicsFlags |= RT_RECYCLED;
		}

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

		for
		(
			UINT i = 0;
			i < m_uDeviceCount;
			++i
		)
		{
			if (!m_pD3DDevice)
			{
				break;
			}

			DXGI_ADAPTER_DESC aid;
			IDXGIAdapter* pDeviceAdapter = nullptr;
			HRESULT hr = E_FAIL;
			{
				IDXGIDevice* pDXGIDevice = nullptr;
				ID3D11Device* pSubDevice = m_pD3DDevice;
				//IUnknown* pUnknownDevice = pSubDevice->GetIUnknown();
				//hr = pUnknownDevice->QueryInterface( __uuidof(IDXGIDevice), (void**)(&pDXGIDevice) );
				hr = pSubDevice->QueryInterface( __uuidof(IDXGIDevice), (void**)(&pDXGIDevice) );
				if (SUCCEEDED(hr) && pDXGIDevice)
				{
					hr = pDXGIDevice->GetAdapter(&pDeviceAdapter);
					if (SUCCEEDED(hr) && pDeviceAdapter)
					{
						DXGI_ADAPTER_DESC adapterDesc;
						hr = pDeviceAdapter->GetDesc(&aid);
					}
				}
				SAFE_RELEASE(pDXGIDevice);
			}
			if (FAILED(hr) || !pDeviceAdapter)
			{
				SAFE_RELEASE(pDeviceAdapter);
				continue;
			}

			tstring sTitle, sDescription;
			sTitle = _T("D3D ");
			sTitle += ToString(m_D3DOptions.FeatureLevel);
			sTitle += _T(" ");
			sTitle += ToString( m_D3DOptions.SrcDriverType );

			LARGE_INTEGER UserModeDriverVersion = { 0, 0 };
			switch (m_D3DOptions.FeatureLevel)
			{
			case D3D_FEATURE_LEVEL_11_0:
			case D3D_FEATURE_LEVEL_11_1:
				// CheckInterfaceSupport does not support querying on ID3D11Device. It does support IDXGIDevice to get a
				// a consistent UMD version for all feature levels. (bug: windows7: 741517)
				//pDeviceAdapter->CheckInterfaceSupport( __uuidof( ID3D11Device ), &UserModeDriverVersion );
				pDeviceAdapter->CheckInterfaceSupport( __uuidof( IDXGIDevice ), &UserModeDriverVersion );
				break;
			case D3D_FEATURE_LEVEL_10_0:
				pDeviceAdapter->CheckInterfaceSupport( __uuidof( ID3D10Device ), &UserModeDriverVersion );
				break;
			case D3D_FEATURE_LEVEL_10_1:
				pDeviceAdapter->CheckInterfaceSupport( __uuidof( ID3D10Device1 ), &UserModeDriverVersion );
				break;
			case D3D_FEATURE_LEVEL_9_1:
			case D3D_FEATURE_LEVEL_9_2:
			case D3D_FEATURE_LEVEL_9_3:
				GetD3D9DriverVersion(aid.AdapterLuid, &UserModeDriverVersion);
				break;
			}
			TCHAR szDriverVersion[32];
			FormatDriverVersion(szDriverVersion, UserModeDriverVersion);

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
	}

	return tr;
}


//--------------------------------------------------------------------

WORD GetLatestRuntimeVersion()
{
	//TODO: this should probably be done using windows versions

	HMODULE hMod = nullptr;
	if ( hMod = LoadLibraryExW( UNICODIFY(D3D11DLLNAME), nullptr, 0 ) )
	{
		FreeLibrary(hMod);
		hMod = nullptr;

		return 0x1100;
	}
	else if ( hMod = LoadLibraryExW( UNICODIFY(D3D10_1DLLNAME), nullptr, 0 ) )
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

bool CD3D11TestFramework::UpdateCurrentDeviceInfo( DXGLOG_DEVICEINFO *pDeviceInfo )
{

	DXGI_ADAPTER_DESC aid;

	if( m_pAdapters == nullptr )
	{
		if( SetupDXGIAndD3D() != RESULT_PASS )
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

	LARGE_INTEGER UserModeDriverVersion = { 0, 0 };

	switch (m_D3DOptions.FeatureLevel)
	{
		case D3D_FEATURE_LEVEL_11_0:
		case D3D_FEATURE_LEVEL_11_1:
			// CheckInterfaceSupport does not support querying on ID3D11Device. It does support IDXGIDevice to get a
			// a consistent UMD version for all feature levels. (bug: windows7: 741517)
			//m_pAdapters[ 0 ]->CheckInterfaceSupport( __uuidof( ID3D11Device ), &UserModeDriverVersion );
			m_pAdapters[ 0 ]->CheckInterfaceSupport( __uuidof( IDXGIDevice ), &UserModeDriverVersion );
			break;

		case D3D_FEATURE_LEVEL_10_0:
			m_pAdapters[ 0 ]->CheckInterfaceSupport( __uuidof( ID3D10Device ), &UserModeDriverVersion );
			break;

		case D3D_FEATURE_LEVEL_10_1:
			m_pAdapters[ 0 ]->CheckInterfaceSupport( __uuidof( ID3D10Device1 ), &UserModeDriverVersion );
			break;

		case D3D_FEATURE_LEVEL_9_1:
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_3:
			GetD3D9DriverVersion(aid.AdapterLuid, &UserModeDriverVersion);
			break;

		default:
			WriteError
			(
				_T( "UpdateCurrentDeviceInfo:  %s is not a valid Direct3D feature level." ),
				ToString(m_D3DOptions.FeatureLevel).c_str()
			);
			break;
	}

	pDeviceInfo->sDriverVersion = new TCHAR[32];
	FormatDriverVersion(pDeviceInfo->sDriverVersion, UserModeDriverVersion);

	// create PnP ID string
	char szBuffer[56];
	sprintf_s(szBuffer, ARRAYSIZE(szBuffer), _T("VEN_%04X&DEV_%04X&SUBSYS_%08X&REV_%02X"), aid.VendorId, aid.DeviceId, aid.SubSysId, aid.Revision);

	size = strlen( szBuffer );
	if( size )
	{
		size++;
		pDeviceInfo->sPnPID = new TCHAR[size];
		CopyString( pDeviceInfo->sPnPID, szBuffer, size );
	}

	//Get the current display settings
	DISPLAY_DEVICE	DisplayDevice;
	DEVMODE			DevMode;

	DisplayDevice.cb = sizeof(DISPLAY_DEVICE);

	bool bFoundPrimaryDisplayDevice = false;
	for
		(
		UINT iDevNum = 0;
	iDevNum < 10;
	++iDevNum
		)
	{
		if (!EnumDisplayDevices(nullptr, iDevNum, &DisplayDevice, 0))
		{
			continue;
		};

		if ((DisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) == 0)
		{
			continue;
		};

		DevMode.dmSize = sizeof(DEVMODE);
		DevMode.dmSpecVersion = DM_SPECVERSION;
		if (EnumDisplaySettings(DisplayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &DevMode))
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


	pDeviceInfo->fMaxDXVersion =  11.0f; // BUGBUG - TODO: CWGFTestFramework::GetMaxDXVersion()
	pDeviceInfo->fMinDXVersion =  11.0f; // BUGBUG - TODO: CWGFTestFramework::GetMinDXVersion()
	pDeviceInfo->bPSGP = false; // BUGBUG - This may get supported in later versions of D3D.
	pDeviceInfo->bDriverDebug = m_D3DOptions.Debug; // BUGBUG - What determines this?
	pDeviceInfo->dwVendorID = aid.VendorId;
	pDeviceInfo->dwDeviceID = aid.DeviceId;
	pDeviceInfo->nDDI = 11; // BUGBUG - reporting 11 DDI even when 10.1 or 10.0 DDI may be used - TODO: CWGFTestFramework::GetD3DDDI()
	pDeviceInfo->fVersion = 11.0f;
	pDeviceInfo->fBuild = CWGFTestFramework::GetSDKBuild();
	pDeviceInfo->bDebug = m_D3DOptions.Debug;
	pDeviceInfo->fInterface = 11.0f; // BUGBUG - TODO: CWGFTestFramework::GetD3DInterface()
	pDeviceInfo->wDesktopBPP = (WORD)DevMode.dmBitsPerPel; 


	// BUGBUG - TODO: Finish this component...

	return true;
}


//--------------------------------------------------------------------

void FeatureLevelCallback( const tstring &FeatureLevel )
{
	CD3D11TestFramework *pFramework = dynamic_cast< CD3D11TestFramework *>( GetFramework() );
	if      ( FeatureLevel == _T("9.1") ) pFramework->m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_9_1;
	else if ( FeatureLevel == _T("9.2") ) pFramework->m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_9_2;
	else if ( FeatureLevel == _T("9.3") ) pFramework->m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_9_3;
	else if ( FeatureLevel == _T("10.0")) pFramework->m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_10_0;
	else if ( FeatureLevel == _T("10.1")) pFramework->m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_10_1;
	else if ( FeatureLevel == _T("11.0")) pFramework->m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	else if ( FeatureLevel == _T("11.1")) pFramework->m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_11_1;
	else pFramework->m_D3DOptions.FeatureLevel = D3D_FEATURE_LEVEL_11_1;
	pFramework->m_bForceFeatureLevel = true;
}

void OrigFeatureLevelCallback( const tstring &FeatureLevel )
{
	CD3D11TestFramework *pFramework = dynamic_cast< CD3D11TestFramework *>( GetFramework() );
	if      ( FeatureLevel == _T("9.1") ) pFramework->m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_9_1;
	else if ( FeatureLevel == _T("9.2") ) pFramework->m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_9_2;
	else if ( FeatureLevel == _T("9.3") ) pFramework->m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_9_3;
	else if ( FeatureLevel == _T("10.0")) pFramework->m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_10_0;
	else if ( FeatureLevel == _T("10.1")) pFramework->m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_10_1;
	else if ( FeatureLevel == _T("11.0")) pFramework->m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	else if ( FeatureLevel == _T("11.1")) pFramework->m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_11_1;
	else if ( FeatureLevel == _T("0"))    pFramework->m_D3DOptions.origFeatureLevel = (D3D_FEATURE_LEVEL) 0;
	else pFramework->m_D3DOptions.origFeatureLevel = D3D_FEATURE_LEVEL_11_1;
	pFramework->m_bForceFeatureLevel = true;
}

void CreateDeviceFlagCallback( const tstring &strCreateDeviceFlag )
{
	CD3D11TestFramework *pFramework = dynamic_cast< CD3D11TestFramework *>( GetFramework() );
	D3D11_CREATE_DEVICE_FLAG enumCreateDeviceFlag = (D3D11_CREATE_DEVICE_FLAG)0;
	FromString( &enumCreateDeviceFlag, strCreateDeviceFlag );
	pFramework->m_D3DOptions.CreateDeviceFlags = (D3D11_CREATE_DEVICE_FLAG)((UINT)pFramework->m_D3DOptions.CreateDeviceFlags | (UINT)enumCreateDeviceFlag);
}

void RasterizerDLL_OptionCallback( const tstring &Rasterizer )
{
	CD3D11TestFramework *pFramework = dynamic_cast< CD3D11TestFramework *>( GetFramework() );

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

void CD3D11TestFramework::InitOptionVariables()
{
	CGraphicsTestFramework::InitOptionVariables();

	int id = AddOptionCategory( "D3D" );
	RegisterOptionVariable( "Src", "Source driver type", &m_D3DOptions.SrcDriverType, D3D_DRIVER_TYPE_HARDWARE, 0, id, _T( "-src:[REF|HW|SW|NULL|WARP]" ) );
	RegisterOptionVariable( "Ref", "Reference driver type", &m_D3DOptions.RefDriverType, D3D_DRIVER_TYPE_REFERENCE, 0, id, _T( "-ref:[REF|HW|SW|NULL|WARP]" ) );
	RegisterOptionVariable( "SrcOnly", "Only use Source device", &m_D3DOptions.SrcOnly, true, 0, id );
	RegisterOptionVariable( _T( "SDK" ), _T( "Run in SDK mode. Includes additional test conditions for SDK validation." ), &m_bSDKTest, false, OPTION_VALUE_CONSTANT, id );

	// BUGBUG - chrisza: Feature level defaults to 10.1 in M1 since 11.0 is not available
	RegisterOptionVariable<tstring>( "FeatureLevel", "Specify Feature Level (9.1, 9.2, 9.3, 10.0, 10.1, 11.0, 11.1 (default) )."
		, FeatureLevelCallback, tstring(_T("11.1")), OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable<bool>( "GenerateGolden", "Generate golden image (o/w use existing golden image)", &m_D3DOptions.generateGolden, false, OPTION_VALUE_CONSTANT, id );

	RegisterOptionVariable( "Deferred", "Use deferred context in API calls when possible", &m_bDeferredContextUsed, false, OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable( "Tiled", "ALLOW - Allow test to substitue tiled resources, ONLY - Only run cases supporting tiled resources, NEVER - Never substitute tiled resources", &m_D3DOptions.tiledResourceMode, D3DTEST_TILED_RESOURCE_MODE_ALLOW, OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable<UINT32>( "QueryTimeout", "Specify time for query to timeout in milliseconds", &m_nQueryTimeout, 5000, 0, id, "-QueryTimeout:#" );

	RegisterOptionVariable<D3DTEST_INTERFACE_LEVEL>( _T( "OrigD3DInterfaceLevel" ), _T( "Original device creation interface level (default is 10.0)" ), &m_D3DOptions.origInterfaceLevel, D3DTEST_INTERFACE_LEVEL_10_0, OPTION_VALUE_CONSTANT, id );
	RegisterOptionVariable<tstring>( "OrigFeatureLevel", "Original Feature Level (10.0, 10.1, 11.0, 11.1, 0 (default indicating use target FeatureLevel) )."
		, OrigFeatureLevelCallback, tstring(_T("0")), OPTION_VALUE_CONSTANT, id );

	RegisterOptionVariable(_T("ViewResources"), _T("Brings up windows for resources with resource viewer"), &m_viewResources, false, 0, 0, _T("-viewresources"));

	RegisterOptionVariable( _T( "EmulateR8WithL8" ), _T( "Get L8 coverage by using it to emulate R8_UNORM. Currently only effective on WARP." ), &m_bEmulateR8WithL8, false, OPTION_VALUE_CONSTANT, id );

	RegisterOptionVariable<tstring>( "CreateDeviceFlag", "Device Create Flag (You can use multiple -CreateDeviceFlag:... arguments)."
		, CreateDeviceFlagCallback, tstring(_T("")), 0, id, "-CreateDeviceFlag:D3D11_CREATE_DEVICE_DEBUGGABLE (etc.)" );

	m_bForceFeatureLevel = false;
	m_bDeferredContextUsed = false;
	m_D3DOptions.tiledResourceMode = D3DTEST_TILED_RESOURCE_MODE_ALLOW;

	// Check if test is running on Win7
	const OSVERSIONINFOEX *osVersionInfo = GetOSVersionInfo();
	if(osVersionInfo->dwMajorVersion == 6 && osVersionInfo->dwMinorVersion == 1)
	{
		// Test is running on Win7.  Since 8IP doesn't expose feature
		// level promotion from 10.x to 11.x change the default value
		// for m_D3DOptions.origInterfaceLevel to D3DTEST_INTERFACE_LEVEL_11_0
		m_D3DOptions.origInterfaceLevel = D3DTEST_INTERFACE_LEVEL_11_0;
	}
}


//--------------------------------------------------------------------

bool CD3D11TestFramework::Present(RECT *pSrc, RECT *pDest, void *pSourceDirtyRegion, void *pDestClipRegion, UINT SyncInterval, UINT dwFlags)
{
	HRESULT hr = S_OK;
	bool result = true;

	for (int i = m_uSwapChainCount - 1; i >= 0; i--)
	{
		if (FAILED( hr = m_pSwapChains[ i ]->Present(SyncInterval, dwFlags)))
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

HRESULT CD3D11TestFramework::ExecuteEffectiveContext()
{
	HRESULT hr = S_OK;

	// This is called by CD3D11Test::Cleanup() in some tests
	// CD3D11Test::Cleanup() can be called if there is a failure during startup (device or context creation fails)
	// In which case, the context pointers may be NULL
	if (m_bDeferredContextUsed && m_pD3DDeviceDeferredContext && m_pD3DDeviceImmediateContext)
	{
		ID3D11CommandList* pCommandList = nullptr;
		hr = m_pD3DDeviceDeferredContext->FinishCommandList(TRUE, &pCommandList);
		if (FAILED(hr))
			return hr;
		m_pD3DDeviceImmediateContext->ExecuteCommandList(pCommandList, TRUE);
		pCommandList->Release();
	}
	
	return hr;
}

const UINT DefaultSampleCount = 1; // No AA.

static void GetTileSize(DXGI_FORMAT format, _Out_ UINT &tileWidth, _Out_ UINT &tileHeight)
{
	UINT elementSize = CFormatHelper::ElementBits(format);

	switch(format)
	{
	default:
		switch(elementSize)
		{
		case 8:
			tileWidth = 256;
			tileHeight = 256;
			break;
		case 16:
			tileWidth = 256;
			tileHeight = 128;
			break;
		case 32:
			tileWidth = 128;
			tileHeight = 128;
			break;
		case 64:
			tileWidth = 128;
			tileHeight = 64;
			break;
		case 128:
			tileWidth = 64;
			tileHeight = 64;
			break;
		default:
			// Not good
			throw(std::exception("Format dimensions cannot be tiled"));
			return;
		}
		break;

	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		tileWidth = 512;
		tileHeight = 256;
		break;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		tileWidth = 256;
		tileHeight = 256;
		break;
	}
}

template<typename T>
bool Tileable(_In_ ID3D11Device2* pDevice, _In_ const T* pDesc, DXGI_FORMAT Format, UINT SampleCount, UINT ExtraBindFlagsToDisallow = 0, UINT ExtraMiscFlagsToDisallow = 0, UINT ArraySize = 1, UINT Width = 0, UINT Height = 0, UINT MipLevels = 1)
{
	// Without ExtraBindFlagsToDisallow or ExtraMiscFlagsToDisallow, this will check all flags that are incompatible with all tiled resources
	const UINT IncompatibleBindFlags = D3D11_BIND_CONSTANT_BUFFER | D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_INDEX_BUFFER | D3D11_BIND_STREAM_OUTPUT |
										D3D11_BIND_DECODER | D3D11_BIND_VIDEO_ENCODER | ExtraBindFlagsToDisallow;
	const UINT IncompatibleMiscFlags = D3D11_RESOURCE_MISC_SHARED | D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX | D3D11_RESOURCE_MISC_GDI_COMPATIBLE | D3D11_RESOURCE_MISC_SHARED_NTHANDLE |
										D3D11_RESOURCE_MISC_RESTRICTED_CONTENT | D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE | D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE_DRIVER |
										D3D11_RESOURCE_MISC_GUARDED | D3D11_RESOURCE_MISC_TILE_POOL | ExtraMiscFlagsToDisallow;

	// Check to see if the device supports tiled resources
	D3D11_FEATURE_DATA_D3D11_OPTIONS1 featureData;
	HRESULT hr = pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, &featureData, sizeof(featureData));

	D3D11_FEATURE_DATA_FORMAT_SUPPORT2 formatSupportData = {Format, D3D11_FORMAT_SUPPORT2_TILED};
	UINT NumQualityLevels = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;
	if (SUCCEEDED(hr) && Format != DXGI_FORMAT_UNKNOWN)
	{
		// If a format is specified, make sure it supports tiled resources
		hr = pDevice->CheckFeatureSupport(D3D11_FEATURE_FORMAT_SUPPORT2, &formatSupportData, sizeof(formatSupportData)); 
		if (SUCCEEDED(hr) && SampleCount > DefaultSampleCount)
		{
			// If a non-default sample count is specified, make sure it supports tiled resources.
			hr = pDevice->CheckMultisampleQualityLevels1(Format, SampleCount, D3D11_CHECK_MULTISAMPLE_QUALITY_LEVELS_TILED_RESOURCE, &NumQualityLevels);
		}

		if(SUCCEEDED(hr) && (formatSupportData.OutFormatSupport2 & D3D11_FORMAT_SUPPORT2_TILED) && ArraySize > 1)
		{
			UINT leastDetailedMipWidth = 1;
			UINT leastDetailedMipHeight = 1;

			if(MipLevels > 0)
			{
				leastDetailedMipWidth = Width;
				leastDetailedMipHeight = Height;

				CFormatHelper::GetMipDimensions(MipLevels - 1, &leastDetailedMipWidth, &leastDetailedMipHeight);
			}

			UINT TileWidth = 0;
			UINT TileHeight = 0;
			
			GetTileSize(Format, TileWidth, TileHeight);
			
			if(D3D11_TILED_RESOURCES_TIER_2 & featureData.TiledResourcesTier)
			{
				// On tier-2 arrays of tiled resources with packed mips are only supported
				// if the least detailed mip is larger than 1/4 the width and height of a
				// tile in each dimension
				if(leastDetailedMipWidth < TileWidth || leastDetailedMipHeight < TileHeight)
				{
					return false;
				}
			}
			else
			if(D3D11_TILED_RESOURCES_TIER_1 & featureData.TiledResourcesTier)
			{
				// On Tier-1 array textures with packed mips cannot be tiled.
				// Also on tier-1 all mips must be multiples of tile size in order to not 
				// be considered packed.
				if(
					static_cast<float>(leastDetailedMipWidth / TileWidth) != static_cast<float>(leastDetailedMipWidth) / static_cast<float>(TileWidth) ||
					static_cast<float>(leastDetailedMipHeight / TileHeight) != static_cast<float>(leastDetailedMipHeight) / static_cast<float>(TileHeight)
					)
				{
					return false;
				}
			}
		}
	}

	return SUCCEEDED(hr) && (featureData.TiledResourcesTier != D3D11_TILED_RESOURCES_NOT_SUPPORTED) && (formatSupportData.OutFormatSupport2 & D3D11_FORMAT_SUPPORT2_TILED) &&
			(pDesc->Usage == D3D11_USAGE_DEFAULT) && !(pDesc->BindFlags & IncompatibleBindFlags) && !(pDesc->MiscFlags & IncompatibleMiscFlags) && NumQualityLevels;
}

// Is the texture2d desc compatible with tiled resources?
bool TileableTexture2D(_In_ ID3D11Device2* pDevice, _In_ const D3D11_TEXTURE2D_DESC* pDesc)
{
	return Tileable(pDevice, pDesc, pDesc->Format, pDesc->SampleDesc.Count, 0, 0, pDesc->ArraySize, pDesc->Width, pDesc->Height, pDesc->MipLevels); 
}

// Is the buffer desc compatible with tiled resources?
bool TileableBuffer(_In_ ID3D11Device2* pDevice, _In_ const D3D11_BUFFER_DESC* pDesc, const DXGI_FORMAT ViewFormat)
{
	const UINT UnsupportedViewBpp = 96;
	const UINT IncompatibleBindFlags = D3D11_BIND_DEPTH_STENCIL; // Depth stencils specifically incompatible with tiled buffers
	if (pDesc->Usage == D3D11_USAGE_DEFAULT && pDesc->CPUAccessFlags != 0) 
	{ 
		// This is a 'Map Default' scenario, which isn't compatible with tiled resources
		return false; 
	}
	return (GetBitsPerElement(ViewFormat) != UnsupportedViewBpp) && Tileable(pDevice, pDesc, DXGI_FORMAT_UNKNOWN, DefaultSampleCount, IncompatibleBindFlags);
}

HRESULT CD3D11TestFramework::CreateTexture2DTiledOptional(_In_ const D3D11_TEXTURE2D_DESC *pDesc, _In_ const D3D11_SUBRESOURCE_DATA *pInitialData, _Out_ ID3D11Texture2D **ppTexture2D)
{
	HRESULT hr = S_OK;
	ID3D11Texture2D *pTexture2D = nullptr;
	const D3DTEST_TILED_RESOURCE_MODE TilingMode = GetTiledResourceMode();
	const bool useTiled = (TilingMode != D3DTEST_TILED_RESOURCE_MODE_NEVER) && m_pTilePoolManager && TileableTexture2D(GetDevice2(), pDesc);

	// Default range buffers
	const UINT defaultNumRanges = 16;
	static UINT defaultRangeOffsetBuffer[defaultNumRanges];
	static UINT defaultRangeCountBuffer[defaultNumRanges];

	// Pointers to range buffers (may be changed if larger buffers are needed)
	UINT numRanges = defaultNumRanges;
	UINT *pRangeOffsets = defaultRangeOffsetBuffer;
	UINT *pRangeCounts = defaultRangeCountBuffer;

	if(useTiled)
	{
		m_pOutputParamTiledResourceUsed->SetValue(true);
		WriteToLog("Using tiled texture2d");

		D3D11_TEXTURE2D_DESC newDesc;
		CopyMemory(&newDesc, pDesc, sizeof(D3D11_TEXTURE2D_DESC));

		newDesc.MiscFlags |= D3D11_RESOURCE_MISC_TILED;

		assert( pTexture2D == nullptr );
		hr = GetDevice2()->CreateTexture2D(&newDesc, nullptr, &pTexture2D);

		if(hr != S_OK)
			goto end;

		// Get the fully resolved desc (to determine the number of mips)
		pTexture2D->GetDesc(&newDesc);

		// Determine the resource tiling configuration
		UINT numTilesNeeded;
		D3D11_PACKED_MIP_DESC packedMipDesc;
		D3D11_TILE_SHAPE tileShapeNonPacked;
		D3D11_SUBRESOURCE_TILING subresourceTilingNonPacked;
		GetDevice2()->GetResourceTiling(pTexture2D, &numTilesNeeded, nullptr, nullptr, nullptr, 0, &subresourceTilingNonPacked);

		// Get the necessary number of tiles from the tile pool
		UINT count = m_pTilePoolManager->FetchTiles(pTexture2D, numTilesNeeded, pRangeOffsets, pRangeCounts, numRanges);
		if(count > numRanges)
		{
			numRanges = count;
			pRangeOffsets = new UINT[count];
			pRangeCounts = new UINT[count];
			count = m_pTilePoolManager->FetchTiles(pTexture2D, numTilesNeeded, pRangeOffsets, pRangeCounts, numRanges);
		}

		if(count == 0)
		{
			WriteToLog(_T("Unable to fetch the required number of tiles from the managed tile pool"));
			hr = E_FAIL;
			goto end;
		}

		numRanges = count;

		// Update tile mapping for the entire resource list
		D3D11_TILED_RESOURCE_COORDINATE startCoord;
		startCoord.X = 0;
		startCoord.Y = 0;
		startCoord.Z = 0;
		startCoord.Subresource = 0;
		D3D11_TILE_REGION_SIZE regionSize;
		regionSize.bUseBox = false;
		regionSize.NumTiles = numTilesNeeded;
		hr = GetEffectiveContext2()->UpdateTileMappings(
			pTexture2D, 
			1,
			&startCoord, 
			&regionSize, 
			m_pTilePoolManager->GetTilePool(),
			numRanges, 
			nullptr, 
			pRangeOffsets, 
			pRangeCounts, 
			D3D11_TILE_MAPPING_NO_OVERWRITE
			);

		if(hr != S_OK)
			goto end;

		if(pInitialData)
		{
			// Create a staging resource to store the initialization data
			ID3D11Texture2D *pStagingTexture = nullptr;
			newDesc.BindFlags = 0;
			newDesc.Usage = D3D11_USAGE_DEFAULT;
			newDesc.MiscFlags = 0;

			assert( pStagingTexture == nullptr );
			hr = GetDevice2()->CreateTexture2D(&newDesc, pInitialData, &pStagingTexture);

			if(hr == S_OK)
			{
				// Copy the initialization data to the tiled resource
				GetEffectiveContext2()->CopyResource(pTexture2D, pStagingTexture);
			}

			SAFE_RELEASE(pStagingTexture);
		}

	}
	else
	{
		// Not using tiled resource
		if(D3DTEST_TILED_RESOURCE_MODE_ONLY == TilingMode)
		{
			hr = E_NOTIMPL;
		}
		else
		{
			assert( pTexture2D == nullptr );
			hr = GetDevice()->CreateTexture2D(pDesc, pInitialData, &pTexture2D);
		}
	}

end:
	if(pRangeOffsets != defaultRangeOffsetBuffer)
	{
		SAFE_DELETE_ARRAY(pRangeOffsets);
		SAFE_DELETE_ARRAY(pRangeCounts);
	}

	*ppTexture2D = pTexture2D;

	return hr;
}

void CD3D11TestFramework::InitOutputParameters(_In_ class CD3D11Test *pTest)
{
	if(m_D3DOptions.tiledResourceMode)
	{
		pTest->CreateOutputParameter<bool>(_T("TiledResourcesUsed"), &m_pOutputParamTiledResourceUsed);
	}
}

HRESULT CD3D11TestFramework::CreateBufferTiledOptional(_In_ const D3D11_BUFFER_DESC *pDesc, _In_ const D3D11_SUBRESOURCE_DATA *pInitialData, _Out_ ID3D11Buffer **ppBuffer, DXGI_FORMAT ViewFormat)
{
	const D3DTEST_TILED_RESOURCE_MODE TilingMode = GetTiledResourceMode();
	const bool bTileable = (TilingMode != D3DTEST_TILED_RESOURCE_MODE_NEVER) && m_pTilePoolManager && TileableBuffer(GetDevice2(), pDesc, ViewFormat);

	// Default range buffers
	const UINT defaultNumRanges = 16;
	static UINT defaultRangeOffsetBuffer[defaultNumRanges];
	static UINT defaultRangeCountBuffer[defaultNumRanges];

	// Pointers to range buffers (may be changed if larger buffers are needed)
	UINT numRanges = defaultNumRanges;
	UINT *pRangeOffsets = defaultRangeOffsetBuffer;
	UINT *pRangeCounts = defaultRangeCountBuffer;

	if (!bTileable)
	{
		if (TilingMode == D3DTEST_TILED_RESOURCE_MODE_ONLY)
		{
			return E_NOTIMPL;
		}

		assert( ppBuffer != nullptr );
		assert( *ppBuffer == nullptr );
		return GetDevice()->CreateBuffer(pDesc, pInitialData, ppBuffer);
	}

	assert(bTileable && (TilingMode == D3DTEST_TILED_RESOURCE_MODE_ONLY || TilingMode == D3DTEST_TILED_RESOURCE_MODE_ALLOW));

	m_pOutputParamTiledResourceUsed->SetValue(true);
	WriteToLog("Using tiled buffer");

	D3D11_BUFFER_DESC TileableDesc = *pDesc;
	TileableDesc.MiscFlags |= D3D11_RESOURCE_MISC_TILED;

	WindowsTest::CHandle<ID3D11Buffer> spBuffer;
	HRESULT hr = GetDevice()->CreateBuffer(&TileableDesc, nullptr, &spBuffer);
	if (SUCCEEDED(hr))
	{
		// Determine how many tiles are required.
		UINT numTilesNeeded;
		GetDevice2()->GetResourceTiling(spBuffer, &numTilesNeeded, nullptr, nullptr, nullptr, 0, nullptr);

		// Get the necessary number of tiles from the tile pool
		UINT count = m_pTilePoolManager->FetchTiles(spBuffer, numTilesNeeded, pRangeOffsets, pRangeCounts, numRanges);
		if(count > numRanges)
		{
			numRanges = count;
			pRangeOffsets = new UINT[count];
			pRangeCounts = new UINT[count];
			count = m_pTilePoolManager->FetchTiles(spBuffer, numTilesNeeded, pRangeOffsets, pRangeCounts, numRanges);
		}

		if(count == 0)
		{
			WriteToLog(_T("Unable to fetch the required number of tiles from the managed tile pool"));
			hr = E_FAIL;
			goto end;
		}

		numRanges = count;

		// Map all tiles
		const D3D11_TILED_RESOURCE_COORDINATE startCoord = {};
		const D3D11_TILE_REGION_SIZE regionSize = {numTilesNeeded};

		hr = GetEffectiveContext2()->UpdateTileMappings(spBuffer, 1, &startCoord, &regionSize, m_pTilePoolManager->GetTilePool(),
														numRanges, nullptr, pRangeOffsets, pRangeCounts, D3D11_TILE_MAPPING_NO_OVERWRITE);
		if (SUCCEEDED(hr) && pInitialData)
		{
			// Create a staging resource to store the initialization data
			D3D11_BUFFER_DESC& StagingDesc = TileableDesc;
			StagingDesc.BindFlags = 0;
			StagingDesc.MiscFlags = 0;

			WindowsTest::CHandle<ID3D11Buffer> spStaging;
			hr = GetDevice2()->CreateBuffer(&StagingDesc, pInitialData, &spStaging);
			if (SUCCEEDED(hr))
			{
				GetEffectiveContext2()->CopyResource(spBuffer, spStaging);
			}
		}
	}

end:
	*ppBuffer = SUCCEEDED(hr) ? spBuffer.Detach() : nullptr;

	if(pRangeOffsets != defaultRangeOffsetBuffer)
	{
		SAFE_DELETE_ARRAY(pRangeOffsets);
		SAFE_DELETE_ARRAY(pRangeCounts);
	}

	return hr;
}

//--------------------------------------------------------------------
//NRESULT CD3D11TestFramework::GoldenImage( ID3D11Resource* pResource, LPCWSTR pFile )
//{
//	NRESULT nr( S_OK, 2 );
//
//	if ( m_D3DOptions.generateGolden )
//	{
//		for( UINT i = 0; i < m_uDeviceCount; ++i )
//		{
//			ID3D11Resource* pResource11 = nullptr;
//			pResource->GetSubResource(i)->GetInterfacePtr(&pResource11);
//
//			HRESULT hr = D3DX10SaveTextureToFileW(pResource11, D3DX10_IFF_DDS, pFile);
//			nr[i] = hr;
//		}
//	}
//	else
//	{
//		for( UINT i = 0; i < m_uDeviceCount; ++i )
//		{
//			ID3D11Resource* pResource11 = nullptr;
//			pResource->GetSubResource(i)->GetInterfacePtr(&pResource11);
//
//			WindowsTest::CHandle<ID3D10Blob> pSrID3D10Blob;
//
//			HRESULT hr = D3DX10SaveTextureToMemory(pResource11, D3DX10_IFF_DDS, &pSrID3D10Blob, 0);
//			if (SUCCEEDED(hr))
//			{
//				hr = E_FAIL;
//				HANDLE h = CreateFileW(
//					pFile,
//					GENERIC_READ,
//					FILE_SHARE_READ,
//					nullptr,
//					OPEN_EXISTING,
//					0,
//					nullptr);
//				if (h != INVALID_HANDLE_VALUE)
//				{
//					HANDLE hMapping = CreateFileMapping(
//						h,
//						nullptr,
//						PAGE_READONLY,
//						0,0,0);
//
//					if (hMapping)
//					{
//						LPVOID pView = MapViewOfFile(
//							hMapping,
//							FILE_MAP_READ,
//							0,0,0);
//
//						if (pView)
//						{
//							DWORD dwLength = GetFileSize(h,0);
//							if ( NO_ERROR == GetLastError())
//							{
//								hr = memcmp(pSrID3D10Blob->GetBufferPointer(), pView, dwLength) == 0 ?
//S_OK : E_FAIL;
//							}
//
//							UnmapViewOfFile(pView);
//						}
//
//						CloseHandle(hMapping);
//					}
//					CloseHandle(h);
//				}
//				else
//				{
//					WriteError( "GoldenImage:  Golden image %s not found", pFile );
//				}
//			}
//			nr[i] = hr;
//		}
//
//	}
//	return nr;
//}

//--------------------------------------------------------------------

CD3D11Test::CD3D11Test()
{
}

void CD3D11Test::InitOutputParameters()
{
	GetFramework()->InitOutputParameters(this);
}

bool CD3D11Test::BlockingFinish() 
{
	HRESULT hr = S_OK;
	WindowsTest::CHandle<IDXGIDevice2> pDXGIDevice;
	HANDLE hEvent = nullptr;

	assert(GetDevice2());

	hr = GetDevice2()->QueryInterface(__uuidof(IDXGIDevice2), (void **)&pDXGIDevice);
	if(FAILED(hr))
	{
		WriteError(_T("QI for IDXGIDevice1 interface failed"));
		return false;
	}

	// Create a manual-reset event object. 
	hEvent = CreateEvent( 
		nullptr,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		nullptr             // unnamed
		); 

	if (hEvent == nullptr) 
	{ 
		WriteError(_T("CreateEvent failed (%d)"), GetLastError());
		return false;
	}

	hr = pDXGIDevice->EnqueueSetEvent(hEvent);   
	if(FAILED(hr))
	{
		WriteError(_T("EnqueueSetEvent() failed in method BlockingFinish()"));
		CloseHandle(hEvent);
		return false;
	}

	DWORD dwWaitResult = WaitForSingleObject( 
		hEvent, // event handle
		10000);    // timeout after ten seconds

	CloseHandle(hEvent); 

	switch (dwWaitResult) 
	{
		// Event object was signaled
		case WAIT_OBJECT_0: 
			// Commands completed
			break; 

		// An error occurred
		default: 
			WriteError(_T("Error waiting for event in BlockingFinish() (%d)"), GetLastError()); 
			return false;
	}

	return true;
}



void CD3D11Test::Cleanup()
{
	CTest::Cleanup();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// CTilePoolManager
//
//////////////////////////////////////////////////////////////////////////
#define TILE_SIZE_IN_BYTES (65536)

//////////////////////////////////////////////////////////////////////////
// CTilePoolManager::CTilePoolManager()
//////////////////////////////////////////////////////////////////////////
CTilePoolManager::CTilePoolManager() :
	m_poolSizeInTiles(0),
	m_pTilePool(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////
// CTilePoolManager::~CTilePoolManager()
//////////////////////////////////////////////////////////////////////////
CTilePoolManager::~CTilePoolManager()
{
	// Ensure that there are no more allocated ranges
	if(m_pTilePool)
	{
		assert(m_allocatedRanges.size() == 0);
		assert(m_freeRangeList.size() == 1);
	}

	// Release the tile pool resource
	SAFE_RELEASE(m_pTilePool);
}

bool CTilePoolManager::RangeIntersectsList(const CTilePoolManager::TILE_RANGE &range, RangeListType &rangeList)
{
	for(RangeListType::iterator it = rangeList.begin(); it != rangeList.end(); ++it)
	{
		if(range.start < (*it).start && range.start + range.size > (*it).start)
			return true;

		if(range.start >= (*it).start && range.start < (*it).start + (*it).size)
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
// CTilePoolManager::FetchRange()
//////////////////////////////////////////////////////////////////////////
UINT CTilePoolManager::FetchRange(ID3D11Resource *pResource, UINT sizeInTiles)
{
	TILE_RANGE range = {0,0};

	assert(m_pTilePool);

	// Iterate through the range list and find a range that will fit sizeInTiles
	for(RangeListType::iterator it = m_freeRangeList.begin(); it != m_freeRangeList.end(); ++it)
	{
		if(it->size >= sizeInTiles)
		{
			range.size = sizeInTiles;
			range.start = it->start;

			if(sizeInTiles < it->size)
			{
				// Decrease the available size of the selected range
				it->start += sizeInTiles;
				it->size -= sizeInTiles;
			}
			else
			{
				// Remove the range if the entire range was allocated
				m_freeRangeList.erase(it);
			}

			// Exit the search loop
			break;
		}
	}

	if(range.size == sizeInTiles)
	{
		assert(false == RangeIntersectsList(range, m_freeRangeList));

		// Find the other ranges allocated for this resource
		RangeListType &rangeList = FindRangeList(pResource);
		InsertRange(rangeList, &range);
	}
	else
	{
		// No range found.
		// Double the tile pool size and try again
		TILE_RANGE newRange;
		newRange.size = m_poolSizeInTiles;
		newRange.start = m_poolSizeInTiles;

		m_poolSizeInTiles *= 2;
		assert(m_poolSizeInTiles > 0);
		assert(m_freeRangeList.size() + m_allocatedRanges.size() > 0);
		HRESULT hr = reinterpret_cast<CD3D11TestFramework *>(GetFramework())->GetEffectiveContext2()->ResizeTilePool(m_pTilePool, TILE_SIZE_IN_BYTES * m_poolSizeInTiles);

		if( hr != S_OK)
			throw std::exception("Out of Memory");

		// Append the range to the end of the free range list
		InsertRange(m_freeRangeList, &newRange);

		return FetchRange(pResource, sizeInTiles);
	}

	return range.start;
}

UINT CTilePoolManager::FetchTiles(_In_ ID3D11Resource *pResource, UINT numTiles, _Inout_updates_(bufferSizes) UINT *pOffsets, _Inout_updates_(bufferSizes) UINT *pRangeSizes, UINT bufferSizesInTiles)
{
	UINT numRangesNeeded = 1; // At least one range is needed
	UINT numTilesLeft = numTiles;

	// Interate through the available ranges and determine the number of ranges required to for the requested tile count
	for(RangeListType::iterator it = m_freeRangeList.begin(); it != m_freeRangeList.end(); ++it)
	{
		// Break out of this loop if the remaining tiles fit in this range
		if(it->size >= numTilesLeft)
						break;

		numTilesLeft -= it->size;
		numRangesNeeded += 1;
	}

	// If the number of ranges needed exceeds to buffer sizes...
	if(numRangesNeeded >= bufferSizesInTiles || nullptr == pRangeSizes || nullptr == pOffsets)
	{
		// ...exit now
		return numRangesNeeded;
	}

	// If the tile pool is too small then grow the tile pool to the smallest power of 2
	// large enough for all the tiles
	if(numTilesLeft > 0)
	{
		UINT newPoolSize = m_poolSizeInTiles * 2;

		for(;;)
		{
			if(numTilesLeft < newPoolSize - m_poolSizeInTiles)
				break;

			newPoolSize *= 2;
		}

		// Grow the tile pool
		TILE_RANGE newRange;
		newRange.size = newPoolSize - m_poolSizeInTiles;
		newRange.start = m_poolSizeInTiles;

		HRESULT hr = reinterpret_cast<CD3D11TestFramework *>(GetFramework())->GetImmediateContext2()->ResizeTilePool(m_pTilePool, TILE_SIZE_IN_BYTES * newPoolSize);
		if(FAILED(hr))
		{
			GetFramework()->WriteToLog(FormatString(_T("Unexpected failure resizing tile pool.  hr = %s"), D3DHResultToString(hr).c_str()));
			GetFramework()->WriteToLogF(_T("Original size: %i, Requested size: %i"), m_poolSizeInTiles, newPoolSize);

			return 0;
		}

		// Append the range to the end of the free range list
		InsertRange(m_freeRangeList, &newRange);

		m_poolSizeInTiles = newPoolSize;
	}

	// Reset the range counter
	numRangesNeeded = 0;

	// Now iterate through the ranges and actually allocate them
	// Interate through the available ranges and determine the number of ranges required to for the requested tile count
	numTilesLeft = numTiles;
	RangeListType &rangeList = FindRangeList(pResource);

	for(RangeListType::iterator it = m_freeRangeList.begin(); numTilesLeft != 0 && it != m_freeRangeList.end(); )
	{
		TILE_RANGE range = *it;

		// Break out of this loop if the remaining tiles fit in this range
		if(it->size > numTilesLeft)
		{
			// Set the allocated range size to the remaining tile count
			range.size = numTilesLeft;

			// Trim the free range
			it->size -= numTilesLeft;
			it->start += numTilesLeft;
		}
		else
		{
			// Remove this range from the free list
			it = m_freeRangeList.erase(it);
		}

		// Insert the entire range into the resource's allocated range list
		InsertRange(rangeList, &range);
								
		// Update the output buffers
		pOffsets[numRangesNeeded] = range.start;
		pRangeSizes[numRangesNeeded] = range.size;
		numRangesNeeded++;
		numTilesLeft -= range.size;
	}

	return numRangesNeeded;
}

//////////////////////////////////////////////////////////////////////////
// CTilePoolManager::ReleaseRanges()
//////////////////////////////////////////////////////////////////////////
void CTilePoolManager::ReleaseRanges(ID3D11Resource *pResource)
{
	AllocatedRangeMapType::iterator it = m_allocatedRanges.find(pResource);
	if(it != m_allocatedRanges.end())
	{
		CTilePoolManager::ReleaseRanges(it);
	}
	assert(m_poolSizeInTiles > 0);
	assert(m_freeRangeList.size() + m_allocatedRanges.size() > 0);
}

//////////////////////////////////////////////////////////////////////////
// CTilePoolManager::ReleaseRanges()
//////////////////////////////////////////////////////////////////////////
CTilePoolManager::AllocatedRangeMapType::iterator CTilePoolManager::ReleaseRanges(AllocatedRangeMapType::iterator it)
{
	RangeListType &rangeList = it->second;

	assert(rangeList.size() > 0);

	// found list of ranges associated with the resource
	for(RangeListType::iterator rlit = rangeList.begin(); rlit != rangeList.end(); ++rlit)
	{
		TILE_RANGE range = *rlit;

		// Add the range back to the free list
		InsertRange(m_freeRangeList, &range);
	}

	// Remove this element from the allocated range map
	return m_allocatedRanges.erase(it);
}

//////////////////////////////////////////////////////////////////////////
// CTilePoolManager::CleanupResources()
//////////////////////////////////////////////////////////////////////////
void CTilePoolManager::CleanupResources()
{
	assert(m_poolSizeInTiles > 0);
	assert(m_freeRangeList.size() + m_allocatedRanges.size() > 0);
	for(AllocatedRangeMapType::iterator it = m_allocatedRanges.begin(); it != m_allocatedRanges.end();)
	{
		ID3D11Resource *pResource = it->first;

		if(pResource && pResource->AddRef() == 2)
		{
			// The resource is no longer in use so free up tile pool ranges
			it = ReleaseRanges(it);

			// Clear tile mappings if tier 1
			ClearMappingsIfTier1(pResource);

			// Final release the resource
			pResource->Release();
		}
		else
		{
			++it;
		}

		if(pResource)
			pResource->Release();
	}
	assert(m_poolSizeInTiles > 0);
	assert(m_freeRangeList.size() + m_allocatedRanges.size() > 0);
}

//////////////////////////////////////////////////////////////////////////
// CTilePoolManager::Create()
//////////////////////////////////////////////////////////////////////////
HRESULT CTilePoolManager::Create(ID3D11Device2 *pDevice)
{
	HRESULT hr = S_OK;
	// Initial tile pool size is 16 tiles
	m_poolSizeInTiles = 16;

	D3D11_BUFFER_DESC tilePoolDesc;
	tilePoolDesc.BindFlags = 0;
	tilePoolDesc.ByteWidth = TILE_SIZE_IN_BYTES * m_poolSizeInTiles;
	tilePoolDesc.CPUAccessFlags = 0;
	tilePoolDesc.MiscFlags = D3D11_RESOURCE_MISC_TILE_POOL;
	tilePoolDesc.Usage = D3D11_USAGE_DEFAULT;

	assert( m_pTilePool == nullptr );
	hr = pDevice->CreateBuffer(&tilePoolDesc, nullptr, &m_pTilePool);
	if(FAILED(hr))
	{
		GetFramework()->WriteToLog(_T("CreateBuffer() failed creating tile pool"));
	}

	// Initialize the free range list
	TILE_RANGE initRange;
	initRange.size = m_poolSizeInTiles;
	initRange.start = 0;
	InsertRange(m_freeRangeList, &initRange);

	assert(m_poolSizeInTiles > 0);
	assert(m_freeRangeList.size() + m_allocatedRanges.size() > 0);

	return hr;
}

TEST_RESULT CTilePoolManager::ShrinkTilePoolIfPossible()
{
	TEST_RESULT tr = RESULT_PASS;

	// Find the upper-bound of the allocated ranges
	CTilePoolManager::RangeListType::reverse_iterator rit = m_freeRangeList.rbegin();

	if(rit->start + rit->size == m_poolSizeInTiles)
	{
		UINT newSize = m_poolSizeInTiles;
		for(UINT halfSize = newSize >> 1; newSize > 16 && rit->start < (newSize >> 1); halfSize >>= 1)
		{
			newSize = halfSize;
		}

		if(newSize < m_poolSizeInTiles)
		{
			HRESULT hr = reinterpret_cast<CD3D11TestFramework *>(GetFramework())->GetImmediateContext2()->ResizeTilePool(m_pTilePool, TILE_SIZE_IN_BYTES * newSize);
			if(FAILED(hr))
			{
				GetFramework()->WriteToLog(FormatString(_T("Unexpected failure resizing tile pool.  hr = %s"), D3DHResultToString(hr).c_str()));
				GetFramework()->WriteToLogF(_T("Original size: %i, Requested size: %i"), m_poolSizeInTiles, newSize);
				tr = RESULT_FAIL;
			}

			rit->size -= m_poolSizeInTiles - newSize;
			m_poolSizeInTiles = newSize;
		}
	}

	return tr;
}

//////////////////////////////////////////////////////////////////////////
// CTilePoolManager::InsertRange()
//////////////////////////////////////////////////////////////////////////
void CTilePoolManager::InsertRange(RangeListType &rangeList, TILE_RANGE *pRange)
{
	if(rangeList.empty())
	{
		// Range list is empty so add a new range
		rangeList.push_back(*pRange);
		return;
	}

	RangeListType::iterator it;
	RangeListType::iterator itLast = rangeList.end();

	for(it = rangeList.begin(); it != rangeList.end();)
	{
		assert(pRange->start != (*it).start);

		if(pRange->start < (*it).start)
		{
			// Insert before the current range
			if(pRange->start + pRange->size < (*it).start)
			{
				if(itLast != rangeList.end() && pRange->start == (*itLast).start + (*itLast).size)
				{
					// Append to the previous range
					(*itLast).size += pRange->size;
				}
				else
				{
					// Add a new range
					assert(pRange->start + pRange->size < (*it).start);
					rangeList.insert(it, *pRange);
				}
			}
			else
			{
				assert(pRange->start + pRange->size == (*it).start);

				if(itLast != rangeList.end() && pRange->start == (*itLast).start + (*itLast).size)
				{
					// Merge all three 
					pRange->start = (*itLast).start;
					pRange->size += (*itLast).size + (*it).size;
					rangeList.insert(it, *pRange);

					// Erase old ranges
					it = rangeList.erase(it);
					it = rangeList.erase(itLast);

				}
				else
				{
					// Prepend the range
					(*it).start = pRange->start;
					(*it).size += pRange->size;
				}
			}

			assert(rangeList.size() > 0);
			return;
		}

		itLast = it++;
	}

	if(it == rangeList.end())
	{
		assert(itLast != rangeList.end());

		// Append the range to the end of the free range list
		if((*itLast).start + (*itLast).size == pRange->start)
		{
			// Append the range to this one
			(*itLast).size += pRange->size;
		}
		else
		{
			// Add the range to the end of the list
			rangeList.push_back(*pRange);
			assert(pRange->start > (*itLast).start + (*itLast).size);
		}
	}

	assert(rangeList.size() > 0);
}

//////////////////////////////////////////////////////////////////////////
// CTilePoolManager::FindRangeList()
//////////////////////////////////////////////////////////////////////////
CTilePoolManager::RangeListType &CTilePoolManager::FindRangeList(ID3D11Resource *pResource)
{
	// Get the rangeList for the associated resource
	AllocatedRangeMapType::iterator it = m_allocatedRanges.find(pResource);
	if(it == m_allocatedRanges.end())
	{
		// First range allocation so add it to the list
		RangeListType emptyRangeList;
		AllocatedRangeMapType::value_type value(pResource, emptyRangeList);
		it = m_allocatedRanges.insert(value).first;

		// Hold a ref count on the resource for cleanup later
		if(pResource)
			pResource->AddRef();
	}

	return it->second;
}

void CTilePoolManager::ClearMappingsIfTier1(_In_ ID3D11Resource *pResource)
{
	HRESULT hr = S_OK;
	WindowsTest::CHandle<ID3D11Device> pDevice;
	WindowsTest::CHandle<ID3D11Device2> pDevice2;

	pResource->GetDevice(&pDevice);
	hr = pDevice->QueryInterface(__uuidof(ID3D11Device2), (void **) &pDevice2);
	if(FAILED(hr))
	{
		throw(std::exception("Cannot QI for ID3D11Device2 interface from tiled resource"));
		return;
	}

	// Determine the tiled resource tier
	D3D11_FEATURE_DATA_D3D11_OPTIONS1 featureData;
	hr = pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, &featureData, sizeof(featureData));
	if(FAILED(hr))
	{
		throw(std::exception("CheckFeatureSupport() failed checking for tiled resources support"));
		return;
	}
	if(featureData.TiledResourcesTier == D3D11_TILED_RESOURCES_TIER_1)
	{
		WindowsTest::CHandle<ID3D11DeviceContext2> pContext;
		pDevice2->GetImmediateContext2(&pContext);
		D3D11_TILED_RESOURCE_COORDINATE startCoords;
		startCoords.Subresource = 0;
		startCoords.X = 0;
		startCoords.Y = 0;
		startCoords.Z = 0;
		D3D11_TILE_REGION_SIZE regionSize;
		regionSize.bUseBox = false;
		// Clear the tile mappings on the buffer
		pDevice2->GetResourceTiling(
			pResource, 
			&regionSize.NumTiles,
			nullptr,
			nullptr,
			nullptr,
			0,
			nullptr
			);
		UINT rangeFlags = D3D11_TILE_RANGE_NULL; // NULL out all tile mappings
		UINT startOffsets = 0;
		UINT rangeCounts = regionSize.NumTiles;
		hr = pContext->UpdateTileMappings(
			pResource, 
			1, 
			&startCoords, 
			&regionSize, 
			m_pTilePool, 
			1, 
			&rangeFlags, 
			&startOffsets, 
			&rangeCounts, 
			0
			);
		if(FAILED(hr))
		{
			throw(std::exception("UpdateTileMappings unexpectedly failed during while clearing tile mappings"));
			return;
		}
	}
}

