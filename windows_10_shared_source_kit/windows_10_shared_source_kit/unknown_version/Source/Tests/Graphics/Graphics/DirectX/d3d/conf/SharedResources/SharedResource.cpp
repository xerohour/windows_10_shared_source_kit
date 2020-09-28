//////////////////////////////////////////////////////////////////////////
//  SharedResources.cpp
//  created: 2007/02/05
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <dxmini.h>
#include <d3dkmddi.h>
#include <d3dkmthk.h>
#include "AdapterUtil.h"

#include <d3d10_1.h>
#include "D3DX11async.h"
#include <sddl.h>

#include "SharedResource.h"
#include "SharedResourceChild.h"
#include "FormatHelper.h"
#include "FormatDesc.hpp"
#include "CreateDevice.h"

#include <Mmsystem.h> // needed for timeGetTime()

BEGIN_NAMED_VALUES( D3D_RESOURCE_TYPE )
	NAMED_VALUE( _T( "D3D_RESOURCE_TYPE_BUFFER" ), D3D_RESOURCE_TYPE_BUFFER )
	NAMED_VALUE( _T( "D3D_RESOURCE_TYPE_TEXTURE1D" ), D3D_RESOURCE_TYPE_TEXTURE1D )
	NAMED_VALUE( _T( "D3D_RESOURCE_TYPE_TEXTURE1D_ARRAY" ), D3D_RESOURCE_TYPE_TEXTURE1D_ARRAY )
	NAMED_VALUE( _T( "D3D_RESOURCE_TYPE_TEXTURE2D" ), D3D_RESOURCE_TYPE_TEXTURE2D )
	NAMED_VALUE( _T( "D3D_RESOURCE_TYPE_TEXTURE2D_ARRAY" ), D3D_RESOURCE_TYPE_TEXTURE2D_ARRAY )
	NAMED_VALUE( _T( "D3D_RESOURCE_TYPE_TEXTURE3D" ), D3D_RESOURCE_TYPE_TEXTURE3D )
	NAMED_VALUE( _T( "D3D_RESOURCE_TYPE_TEXTURECUBE" ), D3D_RESOURCE_TYPE_TEXTURECUBE )
END_NAMED_VALUES( D3D_RESOURCE_TYPE )

BEGIN_NAMED_VALUES( SR_WRITE_METHOD )
	NAMED_VALUE( _T( "COPY_RESOURCE" ), SR_COPY_RESOURCE )
	NAMED_VALUE( _T( "INIT_DATA" ), SR_INIT_DATA )
	NAMED_VALUE( _T( "UPDATE_RESOURCE" ), SR_UPDATE_RESOURCE )
	NAMED_VALUE( _T( "DRAWTO_RESOURCE" ), SR_DRAWTO_RESOURCE )
END_NAMED_VALUES( SR_WRITE_METHOD )

BEGIN_NAMED_VALUES( SR_TEST_TYPE )
	NAMED_VALUE( _T( "SAME_DEVICE" ), SR_SAME_DEVICE )
	NAMED_VALUE( _T( "SAME_PROCESS" ), SR_SAME_PROCESS )
	NAMED_VALUE( _T( "OTHER_PROCESS" ), SR_OTHER_PROCESS )
END_NAMED_VALUES( SR_TEST_TYPE )

BEGIN_NAMED_VALUES( SR_SYNC_METHOD )
	NAMED_VALUE( _T( "NO_SYNC" ), SR_NO_SYNC )
	NAMED_VALUE( _T( "SYS_EVENT" ), SR_SYS_EVENT )
	NAMED_VALUE( _T( "KEYED_MUTEX" ), SR_KEYED_MUTEX )
END_NAMED_VALUES( SR_SYNC_METHOD )


SharedResourcesTestApp g_App;



void SharedResourcesTestApp::InitOptionVariables()
{

	int n = AddOptionCategory( _T( "Debug" ) );

	CD3D11TestFramework::InitOptionVariables();

	UnRegisterOptionVariable( "SrcOnly" );
	m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;
	RegisterOptionVariable( _T( "EmulateR8WithL8" ), _T( "Get L8 coverage by using it to emulate R8_UNORM." ), &m_bEmulateR8WithL8, false, OPTION_VALUE_CONSTANT );

}

bool SharedResourcesTestApp::InitTestGroups()
{	
	IDXGIDevice* pDXGI = NULL;
	IDXGIAdapter* pAdapter = NULL;
	IDXGIAdapter1* pAdapter1 = NULL;
	ID3D11DeviceTest *pDevTest = NULL;
	ID3D11Device *pTempDevice = NULL;
	ID3D11Device *pTempDeviceL9 = NULL;
	DevX::GRFX::AdapterDescriptorCore* pAdapterDescriptor = NULL;
	D3DKMT_DRIVERVERSION kmtDriverVersion = KMT_DRIVERVERSION_WDDM_1_1;
	bool bFL10OrAbove = true;
	bool bSupportsTiledResources = false;
	
	// First test if ExtendedResourceSharing is supported by the underlying driver. 
	HRESULT hr = S_OK;
	m_bSupportExtendedShare = false;
	m_bSupportExtendedNV12Share = false;
	TEST_RESULT SetupD3DResult = SetupDXGIAndD3D();
	if ( SetupD3DResult != RESULT_PASS )
		return false;

	bool bErr = false;
	D3D11_FEATURE_DATA_D3D11_OPTIONS4 d3d11Options4;
	if( FAILED( hr = GetDevice()->CheckFeatureSupport( D3D11_FEATURE_D3D11_OPTIONS4, &d3d11Options4, sizeof(d3d11Options4) ) ) )
	{
		WriteError( _T( "SharedResourcesTestApp::InitTestGroups(): CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS4) failed. HR = %s." ), D3DHResultToString(hr).c_str() );
		bErr = true;
		goto doneCheck;
	}
	if( d3d11Options4.ExtendedNV12SharedTextureSupported )
	{
		m_bSupportExtendedNV12Share = true;
	}

	D3D11_FEATURE_DATA_D3D11_OPTIONS d3d11Options;
	if( FAILED( hr = GetDevice()->CheckFeatureSupport( D3D11_FEATURE_D3D11_OPTIONS, &d3d11Options, sizeof(d3d11Options) ) ) )
	{
		WriteError( _T( "SharedResourcesTestApp::InitTestGroups(): CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS) failed. HR = %s." ), D3DHResultToString(hr).c_str() );
		bErr = true;
		goto doneCheck;
	}
	if( d3d11Options.ExtendedResourceSharing )
	{
		m_bSupportExtendedShare = true;
	}

	// Check if the test is running on WARP. WARP supports ExtendedResourceSharing but not VideoDevice
	hr = GetDevice()->QueryInterface(__uuidof(IDXGIDevice), (void**)(&pDXGI));
	if (FAILED(hr) || pDXGI == NULL)
	{
		WriteToLogF( _T( "SharedResourcesTestApp::InitTestGroups() - QueryInterface failed for IDXGIDevice. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		bErr = true;
		goto doneCheck;
	}
	hr = pDXGI->GetAdapter(&pAdapter);
	if (FAILED(hr) || pAdapter == NULL)
	{
		WriteToLogF( _T( "SharedResourcesTestApp::InitTestGroups() - GetAdapter failed. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		bErr = true;
		goto doneCheck;
	}
	m_bIsWARP = g_App.IsWARPAdapter( pAdapter );
	m_bIs11On12 = g_App.IsD3D11On12Device( GetDevice() );

	// Check if the driver supports WDDM1_3
	hr = pAdapter->QueryInterface( __uuidof(IDXGIAdapter1), (void**)&pAdapter1 );
	if( FAILED( hr ) || pAdapter1 == NULL)
	{
		WriteToLogF( _T( "SharedResourcesTestApp::InitTestGroups() - QueryInterface failed for IDXGIAdapter1. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		bErr = true;
		goto doneCheck;
	}

	pAdapterDescriptor = DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor( pAdapter1 );
	if( !pAdapterDescriptor )
	{
		WriteToLogF( "SharedResourcesTestApp::InitTestGroups() -DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor failed." );
		bErr = true;
		goto doneCheck;
	}

	pAdapterDescriptor->GetDisplayDriverModel( kmtDriverVersion );
	m_bIsAtLeastWDDM1_3 = false;
	if ( kmtDriverVersion >= KMT_DRIVERVERSION_WDDM_1_3)
		m_bIsAtLeastWDDM1_3 = true;

	if ( m_bIsAtLeastWDDM1_3 )
	{
		D3D11_FEATURE_DATA_D3D11_OPTIONS1 d3d11Options1;
		if( FAILED( hr = GetDevice()->CheckFeatureSupport( D3D11_FEATURE_D3D11_OPTIONS1, &d3d11Options1, sizeof(d3d11Options1) ) ) )
		{
			WriteError( _T( "SharedResourcesTestApp::InitTestGroups(): CheckFeatureSupport() failed. HR = %s." ), D3DHResultToString(hr).c_str() );
			bErr = true;
			goto doneCheck;
		}
		if( d3d11Options1.TiledResourcesTier >= D3D11_TILED_RESOURCES_TIER_1 )
		{
			bSupportsTiledResources = true;
		}

		// Check if the driver supports FL10+ and Level9PerfFeatures
		D3D_FEATURE_LEVEL featureLevels[] = 
			{
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_3,
				D3D_FEATURE_LEVEL_9_2,
				D3D_FEATURE_LEVEL_9_1
			};

		HRESULT hr = D3D11CreateDevice_11or12(
						g_App.m_D3DOptions.Force11On12 ? UseD3D11On12_D3D11CreateDeviceFlag : UseD3D11,
						NULL, 
						m_D3DOptions.DriverType,
						NULL,
						0,
						featureLevels,
						ARRAYSIZE(featureLevels),
						D3D11_SDK_VERSION,
						&pTempDevice,
						NULL,
						NULL,
						NULL);
	
		if ( hr != S_OK)
		{
			WriteToLogF( "%s in %s at line %d: D3D11CreateDevice failed for temp device. hr = %s", __FUNCTION__, __FILE__, __LINE__ ,
				D3DHResultToString(hr).c_str());
			bErr = true;
			goto doneCheck;
		}
		bool bSupportsFL10 = ( pTempDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_0 );

		// Create a FL9 device for SupportsLevel9PerfFeatures() query
		D3D_FEATURE_LEVEL featureLevels9[] = 
				{
					D3D_FEATURE_LEVEL_9_3,
					D3D_FEATURE_LEVEL_9_2,
					D3D_FEATURE_LEVEL_9_1
				};

		hr = D3D11CreateDevice_11or12(
						g_App.m_D3DOptions.Force11On12 ? UseD3D11On12_D3D11CreateDeviceFlag : UseD3D11,
						NULL, 
						m_D3DOptions.DriverType,
						NULL,
						0,
						featureLevels9,
						ARRAYSIZE(featureLevels9),
						D3D11_SDK_VERSION,
						&pTempDeviceL9,
						NULL,
						NULL,
						NULL);
	
		if ( hr != S_OK)
		{
			WriteToLogF( "%s in %s at line %d: D3D11CreateDevice failed for FL9 device. hr = %s", __FUNCTION__, __FILE__, __LINE__ ,
				D3DHResultToString(hr).c_str());
			bErr = true;
			goto doneCheck;
		}
		
		hr = pTempDeviceL9->QueryInterface(__uuidof( ID3D11DeviceTest ), (void**)&pDevTest );
		if (FAILED(hr) || pDevTest == NULL)
		{
			WriteToLogF( _T( "SharedResourcesTestApp::InitTestGroups() - QI for ID3D11DeviceTest failed. hr = %s" ), 
				D3DHResultToString(hr).c_str());
			bErr = true;
			goto doneCheck;
		}
		if (pDevTest->SupportsLevel9PerfFeatures() == FALSE && bSupportsFL10 )
		{
			m_bSkipLevel9Cases = true;
		}
	}
	
	D3D_FEATURE_LEVEL fl = GetDevice()->GetFeatureLevel();
	if( fl < D3D_FEATURE_LEVEL_10_0 )
	{
		bFL10OrAbove = false;
	}

doneCheck:
	SAFE_RELEASE(pDXGI);
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pAdapter1);
	SAFE_RELEASE(pTempDevice);
	SAFE_RELEASE(pTempDeviceL9);
	SAFE_RELEASE(pDevTest);
	if( pAdapterDescriptor )
	{
		delete pAdapterDescriptor;
		pAdapterDescriptor = NULL;
	}
	CleanupD3D();
	CleanupDXGI();
	if (bErr) 
		return false;
	  
	//see if this is the child process
	 if( strstr( GetFramework()->GetCommandLine()->GetArgumentString(), _T("-child")) )
	 {
		 if( !ADD_TEST( "SharedResourcesChild", SharedResourcesChild ) ) return false;
	 }
	 else if( strstr( GetFramework()->GetCommandLine()->GetArgumentString(), _T("-tilepoolchild")) )
	 {
		 if( !ADD_TEST( "ShareTilePoolChild", ShareTilePoolChild ) ) return false;
	 }
	 else if(m_bEmulateR8WithL8)
	 {
		 if( !ADD_TEST( "ShareFromCompBuff\\ToDX11",		ShareFromCompBuffToDX11Test) ) return false;
		 if( !ADD_TEST( "ShareDX11FL9",		ShareDX11FL9Test) ) return false;
	 }
	 else
	 {
		if( bSupportsTiledResources && !getIsLessThanWin8() /* keyed mutex needs win8 */ )
		{
			// Shared tile pools are not supported right now
			// if( !ADD_TEST( "ShareTilePool", ShareTilePoolTest) ) return false;
		}
		if( bFL10OrAbove )
		{
			if( !ADD_TEST( "ShareFromDX11\\ToDX11",		ShareFromDX11ToDX11Test ) ) return false;

			if ( !IsMobile() )
			{
				if( !ADD_TEST( "ShareFromDX11\\ToDX10_1",	ShareFromDX11ToDX10_1Test ) ) return false;
				if( !ADD_TEST( "ShareFromDX11\\ToDX10_0",	ShareFromDX11ToDX10_0Test ) ) return false;
				if( !ADD_TEST( "ShareFromDX11\\ToDX9",		ShareFromDX11ToDX9Test ) ) return false;

				if( !ADD_TEST( "ShareFromDX10_1\\ToDX11",	ShareFromDX10_1ToDX11Test ) ) return false;
				if( !ADD_TEST( "ShareFromDX10_1\\ToDX10_1", ShareFromDX10_1ToDX10_1Test ) ) return false;
				if( !ADD_TEST( "ShareFromDX10_1\\ToDX10_0",	ShareFromDX10_1ToDX10_0Test ) ) return false;
				if( !ADD_TEST( "ShareFromDX10_1\\ToDX9",	ShareFromDX10_1ToDX9Test ) ) return false;

				if( !ADD_TEST( "ShareFromDX10_0\\ToDX11",	ShareFromDX10_0ToDX11Test ) ) return false;
				if( !ADD_TEST( "ShareFromDX10_0\\ToDX10_1",	ShareFromDX10_0ToDX10_1Test ) ) return false;
				if( !ADD_TEST( "ShareFromDX10_0\\ToDX10_0", ShareFromDX10_0ToDX10_0Test ) ) return false;
				if( !ADD_TEST( "ShareFromDX10_0\\ToDX9",	ShareFromDX10_0ToDX9Test ) ) return false;

				if( !ADD_TEST( "ShareFromDX9\\ToDX11",		ShareFromDX9ToDX11Test ) ) return false;
				if( !ADD_TEST( "ShareFromDX9\\ToDX10_1",	ShareFromDX9ToDX10_1Test ) ) return false;
				if( !ADD_TEST( "ShareFromDX9\\ToDX10_0",	ShareFromDX9ToDX10_0Test ) ) return false;
				if( !ADD_TEST( "ShareFromDX9\\ToDX9",		ShareFromDX9ToDX9Test ) ) return false;

				if( !ADD_TEST( "ShareFromDX11\\ToGDI",		ShareFromDX11ToGDITest ) ) return false;
				if( !ADD_TEST( "ShareFromGDI\\ToDX11",		ShareFromGDIToDX11Test ) ) return false;
			}
			// video formats only available on DDI 11.1
			if (m_bSupportExtendedShare && !m_bIsWARP && !m_bIs11On12)
			{
				if( !ADD_TEST( "ShareFromDX11\\ToVideo",		ShareFromDX11ToVideoTest ) ) return false;
				if( !ADD_TEST( "ShareFromVideo\\ToDX11",		ShareFromVideoToDX11Test ) ) return false;
			}
		}
		if( m_bIsAtLeastWDDM1_3 )
		{
			if ( !IsMobile() )
			{
				if( !ADD_TEST( "ShareFromCompBuff\\ToDX11",		ShareFromCompBuffToDX11Test) ) return false;
			}
			if( !ADD_TEST( "ShareDX11FL9",		ShareDX11FL9Test) ) return false;
		}

		if( !bFL10OrAbove && !m_bIsAtLeastWDDM1_3)
		{
			if( !ADD_TEST( _T("NoGroups"), CDummyTest ) ) return false;
			WriteToLogF( "SharedResourcesTestApp::InitTestGroups(): No eligible test groups for DX9 HW with driver version less than WDDM 1.3." );
			return true;
		}
	 }

	// Test windows version
	// NTHandle test cases are invalid on Win7+8IP and below
	OSVERSIONINFO versionInfo;  
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);  
	if (!GetVersionEx(&versionInfo) )
	{
		WriteToLogF( _T( "SharedResourcesTestApp::InitTestGroups() - GetVersionEx() failed." ) ); 
		return false; 
	}
	m_bLessThanWin8 = false;
	if(versionInfo.dwMajorVersion < 6 || (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion < 2) )
	{
		m_bLessThanWin8 = true;
	}

	return true;
}

// return a typed format for a typeless format or video to create views
DXGI_FORMAT SharedResourcesTest::GetViewFormat( DXGI_FORMAT formatTypeless )
{
	switch(formatTypeless)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS: 
		return DXGI_FORMAT_R16G16B16A16_FLOAT;

	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		return DXGI_FORMAT_R8G8B8A8_UNORM;

	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		return DXGI_FORMAT_B8G8R8A8_UNORM;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		return DXGI_FORMAT_R10G10B10A2_UINT;

	case DXGI_FORMAT_R32_TYPELESS:
		return DXGI_FORMAT_R32_SINT;

	case DXGI_FORMAT_R16_TYPELESS:
		return DXGI_FORMAT_R16_UNORM; 

	case DXGI_FORMAT_R8_TYPELESS:
		return DXGI_FORMAT_R8_UNORM;

	default:
		WriteToLog(_T("Format unrecognized"));
		return DXGI_FORMAT_UNKNOWN;
	}

	return DXGI_FORMAT_UNKNOWN;
}

void SharedResourcesTest::InitTestParameters()
{
	CTestCaseParameter<D3D_RESOURCE_TYPE> *pResParam = AddParameter<D3D_RESOURCE_TYPE>( _T("ResourceType"), &m_ResourceType );
	testfactor::RFactor rTex2D = AddParameterValue<D3D_RESOURCE_TYPE>( pResParam, D3D_RESOURCE_TYPE_TEXTURE2D);
	
	// Array size
	CTestCaseParameter<UINT> *pArraySize = AddParameter<UINT>( _T("ArraySize"), &m_ArraySize );
	CUserValueSet<UINT> *pArraySizeValues = new CUserValueSet<UINT>( );
	pArraySizeValues->AddValue(1);
	pArraySizeValues->AddValue(2);
	pArraySizeValues->AddValue(6);
	pArraySizeValues->AddValue(9);
	testfactor::RFactor rArraySizeAll = AddParameterValueSet<UINT>(pArraySize, pArraySizeValues);
	testfactor::RFactor rArraySize1 = AddParameterValue<UINT>(pArraySize, 1);

	// Mip levels
	CTestCaseParameter<UINT> *pMipLevels = AddParameter<UINT>( _T("MipLevels"), &m_MipLevels );
	CUserValueSet<UINT> *pMipLevelValues = new CUserValueSet<UINT>( );
	pMipLevelValues->AddValue(1);
	pMipLevelValues->AddValue(2);
	pMipLevelValues->AddValue(3);
	testfactor::RFactor rMipLevelsAll = AddParameterValueSet<UINT>(pMipLevels, pMipLevelValues);
	testfactor::RFactor rMipLevels1 = AddParameterValue<UINT>(pMipLevels, 1);
	m_rResTex2D = rTex2D * rArraySize1 * rMipLevels1;
	m_rResTex2DBC = rTex2D * rArraySizeAll * rMipLevels1;

	// texture format
	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter<DXGI_FORMAT>( _T("Format"), &m_Format );
	CUserValueSet<DXGI_FORMAT> *pFormatValues = new CUserValueSet<DXGI_FORMAT>( );
	pFormatValues->AddValue( DXGI_FORMAT_R8G8B8A8_UNORM );
	pFormatValues->AddValue( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB );
	pFormatValues->AddValue( DXGI_FORMAT_R10G10B10A2_UNORM );
	pFormatValues->AddValue( DXGI_FORMAT_R16G16B16A16_FLOAT );
	m_rFormatsBasic_NoBGR8 = AddParameterValueSet<DXGI_FORMAT>( pFormatParam, pFormatValues );
	
	pFormatValues->AddValue( DXGI_FORMAT_B8G8R8A8_UNORM );
	pFormatValues->AddValue( DXGI_FORMAT_B8G8R8A8_UNORM_SRGB );
	pFormatValues->AddValue( DXGI_FORMAT_B8G8R8X8_UNORM );
	pFormatValues->AddValue( DXGI_FORMAT_B8G8R8X8_UNORM_SRGB );
	m_rFormatsBasic = AddParameterValueSet<DXGI_FORMAT>( pFormatParam, pFormatValues );
	
	CUserValueSet<DXGI_FORMAT> *pNV12FormatValues = new CUserValueSet<DXGI_FORMAT>( );
	CUserValueSet<DXGI_FORMAT> *pVideoFormatValues = new CUserValueSet<DXGI_FORMAT>( );
	if (g_App.getSupportExtendedNV12Share())
	{
		// Keep NV12 out of the normal video formats, and expand on its coverage seperately
		pNV12FormatValues->AddValue( DXGI_FORMAT_NV12 );
		m_rFormats_NV12 = AddParameterValueSet<DXGI_FORMAT>( pFormatParam, pNV12FormatValues );
	}
	else
	{
		pVideoFormatValues->AddValue( DXGI_FORMAT_NV12 );
	}
	pVideoFormatValues->AddValue( DXGI_FORMAT_AYUV );
	pVideoFormatValues->AddValue( DXGI_FORMAT_YUY2 );
	pVideoFormatValues->AddValue( DXGI_FORMAT_NV11 );
	pVideoFormatValues->AddValue( DXGI_FORMAT_P010 );
	pVideoFormatValues->AddValue( DXGI_FORMAT_P016 );
	pVideoFormatValues->AddValue( DXGI_FORMAT_Y210 );
	pVideoFormatValues->AddValue( DXGI_FORMAT_Y216 );
	pVideoFormatValues->AddValue( DXGI_FORMAT_Y410 );
	pVideoFormatValues->AddValue( DXGI_FORMAT_Y416 ); 
	//comment out 420_OPAQUE test cases, since 420_OPAQUE is not CPU-lockable
	//pVideoFormatValues->AddValue( DXGI_FORMAT_420_OPAQUE );
	m_rFormats_Video = AddParameterValueSet<DXGI_FORMAT>( pFormatParam, pVideoFormatValues );

	CUserValueSet<DXGI_FORMAT> *pGDIFormatValues = new CUserValueSet<DXGI_FORMAT>( );
	pGDIFormatValues->AddValue( DXGI_FORMAT_B8G8R8A8_UNORM );
	pGDIFormatValues->AddValue( DXGI_FORMAT_B8G8R8A8_UNORM_SRGB );
	if (g_App.getSupportExtendedShare())
	{
		pGDIFormatValues->AddValue( DXGI_FORMAT_B8G8R8A8_TYPELESS );
	}
	m_rFormats_GDI = AddParameterValueSet<DXGI_FORMAT>( pFormatParam, pGDIFormatValues );

	CUserValueSet<DXGI_FORMAT> *pExtendedFormatValues = new CUserValueSet<DXGI_FORMAT>( );
	pExtendedFormatValues->AddValue(DXGI_FORMAT_R32G32B32A32_TYPELESS);
	pExtendedFormatValues->AddValue(DXGI_FORMAT_R32G32B32A32_UINT);
	pExtendedFormatValues->AddValue(DXGI_FORMAT_R16G16B16A16_TYPELESS);
	pExtendedFormatValues->AddValue(DXGI_FORMAT_R16G16B16A16_UNORM);
	pExtendedFormatValues->AddValue(DXGI_FORMAT_R8G8B8A8_TYPELESS);
	pExtendedFormatValues->AddValue(DXGI_FORMAT_B8G8R8A8_TYPELESS);
	pExtendedFormatValues->AddValue(DXGI_FORMAT_R10G10B10A2_TYPELESS);
	pExtendedFormatValues->AddValue(DXGI_FORMAT_R10G10B10A2_UINT);
	pExtendedFormatValues->AddValue(DXGI_FORMAT_R32_TYPELESS);
	pExtendedFormatValues->AddValue(DXGI_FORMAT_R32_FLOAT);
	pExtendedFormatValues->AddValue(DXGI_FORMAT_R16_TYPELESS);
	pExtendedFormatValues->AddValue(DXGI_FORMAT_R16_FLOAT);
	pExtendedFormatValues->AddValue(DXGI_FORMAT_R8_TYPELESS);
	pExtendedFormatValues->AddValue(DXGI_FORMAT_R8_UINT);
	pExtendedFormatValues->AddValue(DXGI_FORMAT_A8_UNORM);
	pExtendedFormatValues->AddValue(DXGI_FORMAT_R8G8_UNORM);
	m_rFormats_Extended = AddParameterValueSet<DXGI_FORMAT>( pFormatParam, pExtendedFormatValues );

	// CompBuffer formats
	CUserValueSet<DXGI_FORMAT> *pCompBufferFormatValues = new CUserValueSet<DXGI_FORMAT>();
	pCompBufferFormatValues->AddValue( DXGI_FORMAT_R16G16B16A16_FLOAT );
	pCompBufferFormatValues->AddValue( DXGI_FORMAT_B8G8R8A8_UNORM );
	pCompBufferFormatValues->AddValue( DXGI_FORMAT_R8G8B8A8_UNORM );
	pCompBufferFormatValues->AddValue( DXGI_FORMAT_A8_UNORM );
	pCompBufferFormatValues->AddValue( DXGI_FORMAT_R8_UNORM );
	pCompBufferFormatValues->AddValue( DXGI_FORMAT_R8G8_UNORM );
	m_rFormats_CompBuffer = AddParameterValueSet<DXGI_FORMAT>( pFormatParam, pCompBufferFormatValues );

	CUserValueSet<DXGI_FORMAT> *pNewCompBufferFormatValues = new CUserValueSet<DXGI_FORMAT>();
	pNewCompBufferFormatValues->AddValue( DXGI_FORMAT_A8_UNORM );
	pNewCompBufferFormatValues->AddValue( DXGI_FORMAT_R8_UNORM );
	pNewCompBufferFormatValues->AddValue( DXGI_FORMAT_R8G8_UNORM );
	m_rFormats_NewCompBuffer = AddParameterValueSet<DXGI_FORMAT>( pFormatParam, pNewCompBufferFormatValues );
	
	
	CUserValueSet<DXGI_FORMAT> *pBCFormatValues = new CUserValueSet<DXGI_FORMAT>();
	pBCFormatValues->AddValue( DXGI_FORMAT_BC1_UNORM );
	pBCFormatValues->AddValue( DXGI_FORMAT_BC1_UNORM_SRGB );
	pBCFormatValues->AddValue( DXGI_FORMAT_BC2_UNORM );
	pBCFormatValues->AddValue( DXGI_FORMAT_BC2_UNORM_SRGB );
	pBCFormatValues->AddValue( DXGI_FORMAT_BC3_UNORM );
	pBCFormatValues->AddValue( DXGI_FORMAT_BC3_UNORM_SRGB );
	m_rFormats_BC = AddParameterValueSet<DXGI_FORMAT>( pFormatParam, pBCFormatValues );


	// the allowable bind flags and Misc flags
	CTestCaseParameter<bool> *pBindRTParam = AddParameter<bool>( _T("BindRenderTarget"), &m_bBindRenderTarget );
	testfactor::RFactor rBindRTTrue = AddParameterValue<bool>( pBindRTParam, true );
	testfactor::RFactor rBindRTFalse = AddParameterValue<bool>( pBindRTParam, false );
	pBindRTParam->SetDefaultValue(true);

	CTestCaseParameter<bool> *pBindSRParam = AddParameter<bool>( _T("BindShaderResource"), &m_bBindShaderResource );
	testfactor::RFactor rBindSRTrue = AddParameterValue<bool>( pBindSRParam, true );
	testfactor::RFactor rBindSRFalse = AddParameterValue<bool>( pBindSRParam, false );
	pBindSRParam->SetDefaultValue(true);

	CTestCaseParameter<bool> *pBindCapParam = AddParameter<bool>( _T("BindCapture"), &m_bBindCapture );
	testfactor::RFactor rBindCapTrue = AddParameterValue<bool>( pBindCapParam, true );
	testfactor::RFactor rBindCapFalse = AddParameterValue<bool>( pBindCapParam, false );
	pBindCapParam->SetDefaultValue(false);

	CTestCaseParameter<bool> *pMiscGMParam = AddParameter<bool>( _T("MiscGenMips"), &m_bMiscGenMips );
	testfactor::RFactor rMiscGMTrue = AddParameterValue<bool>( pMiscGMParam, true );
	testfactor::RFactor rMiscGMFalse = AddParameterValue<bool>( pMiscGMParam, false );
	pMiscGMParam->SetDefaultValue(false);

	// D3D11_RESOURCE_MISC_SHARED_NTHANDLE
	CTestCaseParameter<bool> *pMiscSharedNTHParam = AddParameter<bool>( _T("MiscSharedNTHandle"), &m_bMiscSharedNTHandle );
	pMiscSharedNTHParam->SetDefaultValue(false);
	testfactor::RFactor rMiscSharedNTHandle = AddParameterValueSet<bool>( pMiscSharedNTHParam, new CBoolValueSet() );
	testfactor::RFactor rMiscSharedNTHandleTrue = AddParameterValue<bool>( pMiscSharedNTHParam, true );

	// m_bCompBuffer and m_bStereo only used for CompositionBuffer sharing
	m_bCompBuffer = false;
	m_bStereo = false;

	//write method of the parent process
	CTestCaseParameter<SR_WRITE_METHOD> *pWriteMethodParam = AddParameter<SR_WRITE_METHOD>( _T("WriteMethod"), &m_WriteMethod );
	CUserValueSet<SR_WRITE_METHOD> *pWriteMethodValues = new CUserValueSet<SR_WRITE_METHOD>( );
	pWriteMethodValues->AddValue( SR_INIT_DATA );
	pWriteMethodValues->AddValue( SR_COPY_RESOURCE );
	pWriteMethodValues->AddValue( SR_UPDATE_RESOURCE );
	m_rWriteMethod_UpdateALL = 
		AddParameterValueSet<SR_WRITE_METHOD>( pWriteMethodParam, pWriteMethodValues );
	m_rWriteMethod_Copy = 
		AddParameterValue<SR_WRITE_METHOD>( pWriteMethodParam, SR_COPY_RESOURCE );
	m_rWriteMethod_Update = 
		AddParameterValue<SR_WRITE_METHOD>( pWriteMethodParam, SR_UPDATE_RESOURCE );
	m_rWriteMethod_Draw = 
		AddParameterValue<SR_WRITE_METHOD>( pWriteMethodParam, SR_DRAWTO_RESOURCE );

	m_rWriteMethod_All = m_rWriteMethod_UpdateALL + m_rWriteMethod_Draw;
	
	// Child process accessing method
	CTestCaseParameter<SR_WRITE_METHOD> *pChildMethodParam = AddParameter<SR_WRITE_METHOD>( _T("ChildAccessMethod"), &m_ChildAccessMethod );
	CUserValueSet<SR_WRITE_METHOD> *pChildMethodValues = new CUserValueSet<SR_WRITE_METHOD>( );
	pChildMethodValues->AddValue( SR_UPDATE_RESOURCE );
	pChildMethodValues->AddValue( SR_DRAWTO_RESOURCE );
	pChildMethodValues->AddValue( SR_COPY_RESOURCE );
	m_rChildMethod_All = AddParameterValueSet<SR_WRITE_METHOD>( pChildMethodParam, pChildMethodValues );
	m_rChildMethod_Update = AddParameterValue<SR_WRITE_METHOD>( pChildMethodParam, SR_UPDATE_RESOURCE );
	m_rChildMethod_Copy = AddParameterValue<SR_WRITE_METHOD>( pChildMethodParam, SR_COPY_RESOURCE );



	// TODO: SR_GENMIP_RESOURCE
	testfactor::RFactor rAllowGenMips = 
		rMiscGMTrue * rBindRTTrue * rBindSRTrue * rBindCapFalse; // mipgen + draw + update
	testfactor::RFactor rAllowDraw = 
		rMiscGMFalse * rBindRTTrue * ( rBindSRTrue + rBindSRFalse) * rBindCapFalse; // draw + update
	testfactor::RFactor rAllowUpdate = 
		rMiscGMFalse * rBindRTFalse * ( rBindSRTrue + rBindSRFalse) * rBindCapFalse; // update

	m_rFlagsGDI = rAllowDraw;

	// The video processor output has to be created with BIND_RENDER_TARGET flag.
	m_rFlagsNV12 = rMiscGMFalse * rBindRTTrue * rBindSRFalse * (rBindCapTrue + rBindCapFalse);
	m_rFlagsVideo = rMiscGMFalse * rBindRTTrue * rBindSRFalse * rBindCapFalse;

	m_rFlagsCompBuff = rMiscGMFalse * rBindRTTrue * rBindSRTrue * rBindCapFalse;

	m_rFlagsBC = rMiscGMFalse * rBindRTFalse * rBindSRTrue * rBindCapFalse;

	m_rFlagsDX9 = rMiscGMFalse * rBindRTFalse * rBindSRFalse * rBindCapFalse;
	// extended shared resource capabilities are only supported if the driver supports DD1 11.1 and above
	testfactor::RFactor rArraySize;
	if ( g_App.getSupportExtendedShare() )
	{
		rArraySize = rArraySizeAll;

		m_rResTex2DArray = rTex2D * rArraySize * rMipLevelsAll;
		m_rFormats_All = m_rFormatsBasic + m_rFormats_Extended;
		m_rFormats_NoBGR8 = m_rFormatsBasic_NoBGR8 + m_rFormats_Extended;
		m_rResourceAccessMethod_All = ( rAllowGenMips +  rAllowDraw ) * m_rChildMethod_All % m_rWriteMethod_All
			+ rAllowUpdate * m_rWriteMethod_UpdateALL * m_rChildMethod_Update;
			
	}
	else
	{
		rArraySize = rArraySize1;
		m_rResTex2DArray = rTex2D * rArraySize * rMipLevels1;
		m_rFormats_All = m_rFormatsBasic;
		m_rFormats_NoBGR8 = m_rFormatsBasic_NoBGR8;
		m_rResourceAccessMethod_All =  m_rWriteMethod_All % m_rChildMethod_All; 
	}
		
	// Texture sizes
	CTestCaseParameter<UINT> *pResWidth = AddParameter<UINT>( _T("ResWidth"), &m_ResWidth );
	CTestCaseParameter<UINT> *pResHeight = AddParameter<UINT>( _T("ResHeight"), &m_ResHeight );
	CUserValueSet<UINT> *pWidthValues = new CUserValueSet<UINT>( );
	pWidthValues->AddValue(64);
	pWidthValues->AddValue(128);
	pWidthValues->AddValue(256);
	CUserValueSet<UINT> *pHeightValues = new CUserValueSet<UINT>( );
	pHeightValues->AddValue(64);
	pHeightValues->AddValue(64);
	pHeightValues->AddValue(480);
	// BC formats need dimensions to be multiple of 4 
	m_rResWidthBC = AddParameterValueSet<UINT>( pResWidth, pWidthValues );
	m_rResHeightBC = AddParameterValueSet<UINT>( pResHeight , pHeightValues );
	m_rResSizeBC = m_rResWidthBC % m_rResHeightBC;

	pWidthValues->AddValue(17);
	pHeightValues->AddValue(17);
	m_rResWidth = AddParameterValueSet<UINT>( pResWidth, pWidthValues );
	m_rResHeight = AddParameterValueSet<UINT>( pResHeight , pHeightValues );
	m_rResSize = m_rResWidth % m_rResHeight;

	// Specail cases for 1x1 texture
	m_rResWidth1 = AddParameterValue<UINT>( pResWidth, 1 );
	m_rResHeight1 = AddParameterValue<UINT>( pResHeight , 1 );
	m_rResSize1 = m_rResWidth1 % m_rResHeight1;
	m_rResCreationSize1 = rTex2D * m_rResSize1 * rArraySize * rMipLevels1 * m_rFormatsBasic_NoBGR8;

	// Sync method: only valid for cross-process cases
	// KeyedMutex can only be used on DXGI1.1, indicating DX10.1 and above. DX10.0 and below will only use event to sync
	CTestCaseParameter<SR_SYNC_METHOD> *pSyncMethodParam = AddParameter<SR_SYNC_METHOD>( _T("SyncMethod"), &m_SyncMethod );
	pSyncMethodParam->SetDefaultValue( SR_NO_SYNC );
	m_rSyncMethod_KeyedMutex = AddParameterValue<SR_SYNC_METHOD>( pSyncMethodParam, SR_KEYED_MUTEX );
	m_rSyncMethod_Event = AddParameterValue<SR_SYNC_METHOD>( pSyncMethodParam, SR_SYS_EVENT );
	m_rSyncMethod_All = m_rSyncMethod_Event + m_rSyncMethod_KeyedMutex;
	m_rSyncMethod_CompBuff = m_rSyncMethod_KeyedMutex * rMiscSharedNTHandleTrue;

	// NTHandle test cases are invalid on Win7+8IP and below, using non-NTHandle test cases instead for 8IP
	if (g_App.getIsLessThanWin8())
	{
		m_rSyncMethod_KeyedMutexWithNTHandle = m_rSyncMethod_KeyedMutex;
		m_rSyncMethod_AllWithNTHandle = m_rSyncMethod_Event + m_rSyncMethod_KeyedMutex;
	}
	else
	{
		m_rSyncMethod_KeyedMutexWithNTHandle = m_rSyncMethod_KeyedMutex * rMiscSharedNTHandle;
		m_rSyncMethod_AllWithNTHandle = m_rSyncMethod_Event + m_rSyncMethod_KeyedMutex * rMiscSharedNTHandle;
	}
	
	//test type
	CTestCaseParameter<SR_TEST_TYPE> *pDeviceSpacesParam =  AddParameter( _T("DeviceSpaces"), &m_testType );
	m_rDevice_SameProc = AddParameterValue<SR_TEST_TYPE>(pDeviceSpacesParam, SR_SAME_PROCESS);
	m_rDevice_CrossProc = AddParameterValue<SR_TEST_TYPE>(pDeviceSpacesParam, SR_OTHER_PROCESS);
	m_rDevice_SameDevice = AddParameterValue<SR_TEST_TYPE>(pDeviceSpacesParam, SR_SAME_DEVICE);


	//source feature level
	CTestCaseParameter<D3D_FEATURE_LEVEL> *pSourceFLParam 
		= AddParameter<D3D_FEATURE_LEVEL>( _T("SourceFeatureLevel"), &m_srcDeviceFL );
	SetParameterDefaultValue<D3D_FEATURE_LEVEL>(pSourceFLParam, (D3D_FEATURE_LEVEL)0 );
	m_rSrcFL_All
		= AddParameterValue<D3D_FEATURE_LEVEL>(pSourceFLParam, D3D_FEATURE_LEVEL_11_0)
		+ AddParameterValue<D3D_FEATURE_LEVEL>(pSourceFLParam, D3D_FEATURE_LEVEL_10_1)
		+ AddParameterValue<D3D_FEATURE_LEVEL>(pSourceFLParam, D3D_FEATURE_LEVEL_10_0);
	m_rSrcFL_10x
		= AddParameterValue<D3D_FEATURE_LEVEL>(pSourceFLParam, D3D_FEATURE_LEVEL_10_1)
		+ AddParameterValue<D3D_FEATURE_LEVEL>(pSourceFLParam, D3D_FEATURE_LEVEL_10_0);
	m_rSrcFL_10_0
		= AddParameterValue<D3D_FEATURE_LEVEL>(pSourceFLParam, D3D_FEATURE_LEVEL_10_0);
	m_rSrcFL_11_0
		= AddParameterValue<D3D_FEATURE_LEVEL>(pSourceFLParam, D3D_FEATURE_LEVEL_11_0);
	testfactor::RFactor rSrcFL_9_1 = AddParameterValue<D3D_FEATURE_LEVEL>(pSourceFLParam, D3D_FEATURE_LEVEL_9_1);

	//dest feature level
	CTestCaseParameter<D3D_FEATURE_LEVEL> *pDestFLParam 
		= AddParameter<D3D_FEATURE_LEVEL>( _T("DestFeatureLevel"), &m_destDeviceFL );
	SetParameterDefaultValue<D3D_FEATURE_LEVEL>(pDestFLParam, (D3D_FEATURE_LEVEL)0 );
	m_rDestFL_All
		= AddParameterValue<D3D_FEATURE_LEVEL>(pDestFLParam, D3D_FEATURE_LEVEL_11_0)
		+ AddParameterValue<D3D_FEATURE_LEVEL>(pDestFLParam, D3D_FEATURE_LEVEL_10_1)
		+ AddParameterValue<D3D_FEATURE_LEVEL>(pDestFLParam, D3D_FEATURE_LEVEL_10_0);
	m_rDestFL_10x
		= AddParameterValue<D3D_FEATURE_LEVEL>(pDestFLParam, D3D_FEATURE_LEVEL_10_1)
		+ AddParameterValue<D3D_FEATURE_LEVEL>(pDestFLParam, D3D_FEATURE_LEVEL_10_0);
	m_rDestFL_10_0
		= AddParameterValue<D3D_FEATURE_LEVEL>(pDestFLParam, D3D_FEATURE_LEVEL_10_0);
	m_rDestFL_11_0
		= AddParameterValue<D3D_FEATURE_LEVEL>(pDestFLParam, D3D_FEATURE_LEVEL_11_0);
	testfactor::RFactor rDestFL_9_1 = AddParameterValue<D3D_FEATURE_LEVEL>(pDestFLParam, D3D_FEATURE_LEVEL_9_1);

	m_rSrcDescFL9_Mix = rSrcFL_9_1 * m_rDestFL_11_0 + rSrcFL_9_1 * m_rDestFL_10_0 + rSrcFL_9_1 * rDestFL_9_1 +
		m_rSrcFL_11_0 * rDestFL_9_1 + m_rSrcFL_10_0 * rDestFL_9_1 + m_rSrcFL_11_0 * m_rDestFL_10_0 + m_rSrcFL_10_0 * m_rDestFL_11_0;
	

	// Adding priority filters
	AddPriorityPatternFilter(FilterEqual<SR_WRITE_METHOD>(_T("WriteMethod"), SR_UPDATE_RESOURCE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<D3D_FEATURE_LEVEL>(_T("DestFeatureLevel"), D3D_FEATURE_LEVEL_10_1), WeightFactorVerySmall);
	AddPriorityPatternFilter(FilterEqual<D3D_FEATURE_LEVEL>(_T("SourceFeatureLevel"), D3D_FEATURE_LEVEL_10_1), WeightFactorVerySmall);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"),DXGI_FORMAT_B8G8R8X8_UNORM_SRGB), WeightFactorVerySmall);
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );

	// Windows Vista has a bug that doesn't allow 10 => 9 sharing to work
	if ( (g_App.GetTestMode() == WGFTEST_MODE_WHQL) )
	{
		OSVERSIONINFO versionInfo;
		versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
		GetVersionEx(&versionInfo);
		// Want to run the test cases on Win7 or better, which is 6.1
		if (versionInfo.dwMajorVersion < 6 || (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion < 1) )
		{
			SkipMatching(GetSkipMatchingFilter() || (
				FilterEqual<UINT>(_T("SourceDevice"), 10)
				&& FilterEqual<UINT>(_T("DestDevice"), 9)
				));
		}
	}
}

void ShareFromDX11ToDX11Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 1100;
	m_destDevice = 1100;

	testfactor::RFactor rResCreation = testfactor::NewPairwiseFactor( m_rResTex2DArray, m_rFormats_All, m_rResSize );
	testfactor::RFactor rResCreationBC = m_rResTex2DBC * m_rFormats_BC * m_rResSizeBC;

	testfactor::RFactor rResource = ( rResCreation + m_rResCreationSize1 ) % m_rResourceAccessMethod_All;
	testfactor::RFactor rResourceBC = rResCreationBC * ( m_rWriteMethod_Copy * m_rChildMethod_Copy * m_rFlagsBC );
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_All * m_rSrcFL_All;

	testfactor::RFactor rSameProcCases = ( rResource + rResourceBC ) * ( m_rDevice_SameDevice + m_rDevice_SameProc ) % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( ( rResource + rResourceBC ) * m_rDevice_CrossProc % rDestAndSrcFLs) % m_rSyncMethod_AllWithNTHandle;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}
 
void ShareFromDX11ToDX10_1Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 1100;
	m_destDevice = 1010;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		{m_rResTex2DArray, 1, NULL, 0},
		{m_rFormats_All, 1, NULL, 0},
		{m_rResSize, 1, NULL, 0},
	};

	size_t groupOrders [] = { XGROUPORDER(1, 2) };

	testfactor::RFactor rfResCreation = NewCrossFactor(
		pairwiseTable, 
		_countof(pairwiseTable),
		_T("ResCreationFactors"),
		groupOrders,
		_countof(groupOrders) );

	testfactor::RFactor rResource = ( rfResCreation + m_rResCreationSize1 ) % m_rResourceAccessMethod_All;
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_10x * m_rSrcFL_All; 

	testfactor::RFactor rSameProcCases = rResource * m_rDevice_SameProc % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc % rDestAndSrcFLs) % m_rSyncMethod_AllWithNTHandle;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromDX11ToDX10_0Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 1100;
	m_destDevice = 1000;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		{m_rResTex2DArray, 1, NULL, 0},
		{m_rFormats_NoBGR8, 1, NULL, 0},
		{m_rResSize, 1, NULL, 0},
	};

	size_t groupOrders [] = { XGROUPORDER(1, 2) };

	testfactor::RFactor rfResCreation = NewCrossFactor(
		pairwiseTable, 
		_countof(pairwiseTable),
		_T("ResCreationFactors"),
		groupOrders,
		_countof(groupOrders) );

	testfactor::RFactor rResource = ( rfResCreation + m_rResCreationSize1 ) % m_rResourceAccessMethod_All;
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_10_0 * m_rSrcFL_All; 

	testfactor::RFactor rSameProcCases = rResource * m_rDevice_SameProc % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc % rDestAndSrcFLs) % m_rSyncMethod_Event;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromDX11ToDX9Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 1100;
	m_destDevice = 900;

	testfactor::RFactor rResSize = m_rResSize + m_rResSize1;
	testfactor::RFactor rResource = ( m_rResTex2D * m_rFormatsBasic * rResSize ) % m_rWriteMethod_All;
	testfactor::RFactor rDestAndSrcFLs = m_rSrcFL_All; 

	testfactor::RFactor rSameProcCases = rResource * m_rDevice_SameProc % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc % rDestAndSrcFLs) % m_rSyncMethod_Event;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromDX10_1ToDX11Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 1010;
	m_destDevice = 1100;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		{m_rResTex2DArray, 1, NULL, 0},
		{m_rFormats_All, 1, NULL, 0},
		{m_rResSize, 1, NULL, 0},
	};

	size_t groupOrders [] = { XGROUPORDER(1, 2) };

	testfactor::RFactor rfResCreation = NewCrossFactor(
		pairwiseTable, 
		_countof(pairwiseTable),
		_T("ResCreationFactors"),
		groupOrders,
		_countof(groupOrders) );

	testfactor::RFactor rResource = ( rfResCreation + m_rResCreationSize1 ) % m_rResourceAccessMethod_All;
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_All * m_rSrcFL_10x; 

	testfactor::RFactor rSameProcCases = rResource * m_rDevice_SameProc % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc % rDestAndSrcFLs) % m_rSyncMethod_All;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromDX10_1ToDX10_1Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 1010;
	m_destDevice = 1010;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		{m_rResTex2DArray, 1, NULL, 0},
		{m_rFormats_All, 1, NULL, 0},
		{m_rResSize, 1, NULL, 0},
	};

	size_t groupOrders [] = { XGROUPORDER(1, 2) };

	testfactor::RFactor rfResCreation = NewCrossFactor(
		pairwiseTable, 
		_countof(pairwiseTable),
		_T("ResCreationFactors"),
		groupOrders,
		_countof(groupOrders) );

	testfactor::RFactor rResource = ( rfResCreation + m_rResCreationSize1 ) % m_rResourceAccessMethod_All;
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_10x * m_rSrcFL_10x; 

	testfactor::RFactor rSameProcCases = rResource * ( m_rDevice_SameDevice + m_rDevice_SameProc ) % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc % rDestAndSrcFLs) % m_rSyncMethod_All;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromDX10_1ToDX10_0Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 1010;
	m_destDevice = 1000;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		{m_rResTex2DArray, 1, NULL, 0},
		{m_rFormats_NoBGR8, 1, NULL, 0},
		{m_rResSize, 1, NULL, 0},
	};

	size_t groupOrders [] = { XGROUPORDER(1, 2) };

	testfactor::RFactor rfResCreation = NewCrossFactor(
		pairwiseTable, 
		_countof(pairwiseTable),
		_T("ResCreationFactors"),
		groupOrders,
		_countof(groupOrders) );

	testfactor::RFactor rResource = ( rfResCreation + m_rResCreationSize1 ) % m_rResourceAccessMethod_All;
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_10_0 * m_rSrcFL_10x; 

	testfactor::RFactor rSameProcCases = rResource * m_rDevice_SameProc % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc % rDestAndSrcFLs) % m_rSyncMethod_Event;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromDX10_1ToDX9Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 1010;
	m_destDevice = 900;

	testfactor::RFactor rResSize = m_rResSize + m_rResSize1;
	testfactor::RFactor rResource = ( m_rResTex2D * m_rFormatsBasic * rResSize ) % m_rWriteMethod_All;
	testfactor::RFactor rDestAndSrcFLs = m_rSrcFL_10x; 

	testfactor::RFactor rSameProcCases = rResource * m_rDevice_SameProc % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc % rDestAndSrcFLs) % m_rSyncMethod_Event;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromDX10_0ToDX11Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 1000;
	m_destDevice = 1100;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		{m_rResTex2DArray, 1, NULL, 0},
		{m_rFormats_NoBGR8, 1, NULL, 0},
		{m_rResSize, 1, NULL, 0},
	};

	size_t groupOrders [] = { XGROUPORDER(1, 2) };

	testfactor::RFactor rfResCreation = NewCrossFactor(
		pairwiseTable, 
		_countof(pairwiseTable),
		_T("ResCreationFactors"),
		groupOrders,
		_countof(groupOrders) );

	testfactor::RFactor rResource = ( rfResCreation + m_rResCreationSize1 ) % m_rResourceAccessMethod_All;
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_All * m_rSrcFL_10_0; 

	testfactor::RFactor rSameProcCases = rResource * m_rDevice_SameProc % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc % rDestAndSrcFLs) % m_rSyncMethod_Event;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromDX10_0ToDX10_1Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 1000;
	m_destDevice = 1010;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		{m_rResTex2DArray, 1, NULL, 0},
		{m_rFormats_NoBGR8, 1, NULL, 0},
		{m_rResSize, 1, NULL, 0},
	};

	size_t groupOrders [] = { XGROUPORDER(1, 2) };

	testfactor::RFactor rfResCreation = NewCrossFactor(
		pairwiseTable, 
		_countof(pairwiseTable),
		_T("ResCreationFactors"),
		groupOrders,
		_countof(groupOrders) );

	testfactor::RFactor rResource = ( rfResCreation + m_rResCreationSize1 ) % m_rResourceAccessMethod_All;
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_10x * m_rSrcFL_10_0; 

	testfactor::RFactor rSameProcCases = rResource * m_rDevice_SameProc % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc % rDestAndSrcFLs) % m_rSyncMethod_Event;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromDX10_0ToDX10_0Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 1000;
	m_destDevice = 1000;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		{m_rResTex2DArray, 1, NULL, 0},
		{m_rFormats_NoBGR8, 1, NULL, 0},
		{m_rResSize, 1, NULL, 0},
	};

	size_t groupOrders [] = { XGROUPORDER(1, 2) };

	testfactor::RFactor rfResCreation = NewCrossFactor(
		pairwiseTable, 
		_countof(pairwiseTable),
		_T("ResCreationFactors"),
		groupOrders,
		_countof(groupOrders) );


	testfactor::RFactor rResource = ( rfResCreation + m_rResCreationSize1 ) % m_rResourceAccessMethod_All;
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_10_0 * m_rSrcFL_10_0; 

	testfactor::RFactor rSameProcCases = rResource * ( m_rDevice_SameDevice + m_rDevice_SameProc ) % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc % rDestAndSrcFLs ) % m_rSyncMethod_Event;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromDX10_0ToDX9Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 1000;
	m_destDevice = 900;

	testfactor::RFactor rResSize = m_rResSize + m_rResSize1;
	testfactor::RFactor rResource = ( m_rResTex2D * m_rFormatsBasic_NoBGR8 * rResSize ) % m_rWriteMethod_All;
	testfactor::RFactor rDestAndSrcFLs = m_rSrcFL_10_0; 

	testfactor::RFactor rSameProcCases = rResource * m_rDevice_SameProc % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc % rDestAndSrcFLs ) % m_rSyncMethod_Event;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromDX9ToDX11Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 900;
	m_destDevice = 1100;

	testfactor::RFactor rResSize = m_rResSize + m_rResSize1;
	testfactor::RFactor rResource = m_rResTex2D * m_rFormatsBasic * rResSize * m_rFlagsDX9 % m_rChildMethod_Update;
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_All; 

	testfactor::RFactor rSameProcCases = rResource * m_rDevice_SameProc % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc % rDestAndSrcFLs) % m_rSyncMethod_Event;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromDX9ToDX10_1Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 900;
	m_destDevice = 1010;

	testfactor::RFactor rResSize = m_rResSize + m_rResSize1;
	testfactor::RFactor rResource = m_rResTex2D * m_rFormatsBasic * rResSize % m_rChildMethod_Update;
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_10x; 

	testfactor::RFactor rSameProcCases = rResource * m_rDevice_SameProc % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc % rDestAndSrcFLs) % m_rSyncMethod_Event;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromDX9ToDX10_0Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();
	
	m_srcDevice = 900;
	m_destDevice = 1000;

	testfactor::RFactor rResSize = m_rResSize + m_rResSize1;
	testfactor::RFactor rResource = m_rResTex2D * m_rFormatsBasic_NoBGR8 * rResSize % m_rChildMethod_Update;
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_10_0; 

	testfactor::RFactor rSameProcCases = rResource * m_rDevice_SameProc % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc % rDestAndSrcFLs) % m_rSyncMethod_Event;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromDX9ToDX9Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 900;
	m_destDevice = 900;

	testfactor::RFactor rResSize = m_rResSize + m_rResSize1;
	testfactor::RFactor rResource = m_rResTex2D * m_rFormatsBasic * rResSize;
	
	testfactor::RFactor rSameProcCases = rResource * ( m_rDevice_SameDevice + m_rDevice_SameProc);
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc ) % m_rSyncMethod_Event;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}


void ShareFromDX11ToVideoTest::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 1100;
	// use 1101 for DX11 Video device
	m_destDevice = 1101;

	// FL does not matter in this group. Use 10.0
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_10_0 * m_rSrcFL_10_0;

	testfactor::RFactor rResSize = m_rResSize + m_rResSize1;
	testfactor::RFactor rResource = ( m_rResTex2D * m_rFormats_Video * rResSize * m_rFlagsVideo )  % m_rWriteMethod_Copy;
	
	testfactor::RFactor rSameProcCases = rResource * m_rDevice_SameProc % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc  % rDestAndSrcFLs) % m_rSyncMethod_AllWithNTHandle;

	testfactor::RFactor rRoot;
	if (g_App.getSupportExtendedNV12Share())
	{
		testfactor::RFactor rNV12Resource = ( m_rResTex2D * m_rFormats_NV12 * rResSize * m_rFlagsNV12 )  % m_rWriteMethod_Copy;
		
		testfactor::RFactor rNV12SameProcCases = rNV12Resource * m_rDevice_SameProc % rDestAndSrcFLs;
		testfactor::RFactor rNV12CrossProcCases = ( rNV12Resource * m_rDevice_CrossProc  % rDestAndSrcFLs) % m_rSyncMethod_AllWithNTHandle;

		rRoot = rSameProcCases + rCrossProcCases + rNV12SameProcCases + rNV12CrossProcCases;
	}
	else
	{
		rRoot = rSameProcCases + rCrossProcCases;
	}

	SetRootTestFactor( rRoot );
}

void ShareFromVideoToDX11Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	// use 1101 for DX11 Video device
	m_srcDevice = 1101;
	m_destDevice = 1100;

	// FL does not matter in this group. Use 10.0
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_10_0 * m_rSrcFL_10_0;

	testfactor::RFactor rResSize = m_rResSize + m_rResSize1;
	testfactor::RFactor rResource = ( m_rResTex2D * m_rFormats_Video * rResSize * m_rFlagsVideo )  % m_rChildMethod_Copy;
	
	testfactor::RFactor rSameProcCases = rResource * m_rDevice_SameProc % rDestAndSrcFLs;
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc  % rDestAndSrcFLs) % m_rSyncMethod_AllWithNTHandle;

	testfactor::RFactor rRoot;
	if (g_App.getSupportExtendedNV12Share())
	{
		testfactor::RFactor rNV12Resource = ( m_rResTex2D * m_rFormats_NV12 * rResSize * m_rFlagsNV12 )  % m_rWriteMethod_Copy;
		
		testfactor::RFactor rNV12SameProcCases = rNV12Resource * m_rDevice_SameProc % rDestAndSrcFLs;
		testfactor::RFactor rNV12CrossProcCases = ( rNV12Resource * m_rDevice_CrossProc  % rDestAndSrcFLs) % m_rSyncMethod_AllWithNTHandle;

		rRoot = rSameProcCases + rCrossProcCases + rNV12SameProcCases + rNV12CrossProcCases;
	}
	else
	{
		rRoot = rSameProcCases + rCrossProcCases;
	}

	SetRootTestFactor( rRoot );
}

void ShareFromDX11ToGDITest::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 1100;
	// use 1102 for GDI compatible
	m_destDevice = 1102;
	m_bMiscGDICompatible = true;

	// FL does not matter in this group. Use 10.0
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_10_0 * m_rSrcFL_10_0;

	testfactor::RFactor rResSize = m_rResSize + m_rResSize1;
	testfactor::RFactor rResource = ( m_rResTex2D * m_rFormats_GDI * rResSize * m_rFlagsGDI )  % m_rWriteMethod_UpdateALL;
	
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc  % rDestAndSrcFLs) % m_rSyncMethod_KeyedMutexWithNTHandle;
	testfactor::RFactor rRoot = rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromGDIToDX11Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	// use 1102 for GDI compatible
	m_srcDevice = 1102;
	m_destDevice = 1100;
	m_bMiscGDICompatible = true;

	// FL does not matter in this group. Use 10.0
	testfactor::RFactor rDestAndSrcFLs = m_rDestFL_10_0 * m_rSrcFL_10_0;

	testfactor::RFactor rResSize = m_rResSize + m_rResSize1;
	testfactor::RFactor rResource = ( m_rResTex2D * m_rFormats_GDI * rResSize * m_rFlagsGDI )  % m_rChildMethod_Update;
	
	testfactor::RFactor rCrossProcCases = ( rResource * m_rDevice_CrossProc  % rDestAndSrcFLs) % m_rSyncMethod_KeyedMutexWithNTHandle;
	testfactor::RFactor rRoot = rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareDX11FL9Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_srcDevice = 1100;
	m_destDevice = 1100;

	testfactor::RFactor rResSize = m_rResSize + m_rResSize1;
	testfactor::RFactor rResCreation = m_rResTex2D * ( m_rFormatsBasic + m_rFormats_NewCompBuffer ) * rResSize;
	testfactor::RFactor rResCreationBC = m_rResTex2D * m_rFormats_BC * m_rResSizeBC;

	testfactor::RFactor rResource = rResCreation % m_rResourceAccessMethod_All;
	testfactor::RFactor rResourceBC = rResCreationBC * ( m_rWriteMethod_Copy * m_rChildMethod_Copy * m_rFlagsBC );

	testfactor::RFactor rSameProcCases = ( rResource + rResourceBC ) * ( m_rDevice_SameDevice + m_rDevice_SameProc ) * m_rSrcDescFL9_Mix;
	testfactor::RFactor rCrossProcCases = ( ( rResource + rResourceBC ) * m_rDevice_CrossProc * m_rSrcDescFL9_Mix) % m_rSyncMethod_AllWithNTHandle;

	testfactor::RFactor rRoot = rSameProcCases + rCrossProcCases;

	SetRootTestFactor( rRoot );
}

void ShareFromCompBuffToDX11Test::InitTestParameters()
{
	SharedResourcesTest::InitTestParameters();

	m_bCompBuffer = true;
	m_srcDevice = 1100;
	m_destDevice = 1100;
	m_MipLevels = 1;
	m_ArraySize = 1;

	// whether it's a stereo composition buffer
	CTestCaseParameter<bool> *pIsStereoParam = AddParameter<bool>( _T("IsStereo"), &m_bStereo );
	pIsStereoParam->SetDefaultValue(false);
	testfactor::RFactor rIsStereo = AddParameterValueSet<bool>( pIsStereoParam, new CBoolValueSet() );

	testfactor::RFactor rResourceAccessMethod = m_rFlagsCompBuff * (  m_rWriteMethod_Copy +  m_rWriteMethod_Update + m_rWriteMethod_Draw ) * m_rChildMethod_All;
	testfactor::RFactor rResCreation = testfactor::NewPairwiseFactor( m_rFormats_CompBuffer, m_rResSize, rResourceAccessMethod, m_rSrcDescFL9_Mix);
	testfactor::RFactor rResourceAccessMethodBC = m_rWriteMethod_Copy * m_rChildMethod_Copy * m_rFlagsBC;
	testfactor::RFactor rResCreationBC = testfactor::NewPairwiseFactor( m_rFormats_BC, m_rResSizeBC, rResourceAccessMethodBC, m_rSrcDescFL9_Mix);
	
	// number of resources to present (Multi-present/Present1 testing)
	CTestCaseParameter<UINT> *pNumResources = AddParameter<UINT>( _T("NumResources"), &m_NumCompBuffers );
	CUserValueSet<UINT> *pNumResourcesValues = new CUserValueSet<UINT>( );
	pNumResourcesValues->AddValue(1);
	pNumResourcesValues->AddValue(2);
	pNumResourcesValues->AddValue(10);
	testfactor::RFactor rNumResources = AddParameterValueSet<UINT>(pNumResources, pNumResourcesValues);
	
	// Flip Model backbuffers are created in a way that restricts opening to either DWM or the creator process. So test SameProc sharing only
	testfactor::RFactor rRoot = ( rResCreation + rResCreationBC ) * m_rDevice_SameProc * m_rSyncMethod_CompBuff * rIsStereo * rNumResources;

	SetRootTestFactor( rRoot );

}


TEST_RESULT ShareFromCompBuffToDX11Test::Setup()
{
	// Fill the window class struct.
	ZeroMemory( &m_WindowClass, sizeof( m_WindowClass ) );
	m_WindowClass.cbSize = sizeof( m_WindowClass );
	m_WindowClass.style = 0;
	m_WindowClass.lpfnWndProc = DefWindowProcW;
	m_WindowClass.cbClsExtra = 0;
	m_WindowClass.cbWndExtra = 0;
	m_WindowClass.hInstance = GetModuleHandle( NULL );
	m_WindowClass.lpszClassName = L"WGF11SharedResources";
	m_WindowClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	m_WindowClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	
	// Register the window class.
	if ( !RegisterClassExW( &m_WindowClass ) )
	{
		DWORD RegisterClassError = GetLastError();
		WriteToLog(	"SharedResourcesTest::Setup():  RegisterClassEx failed with error %u.",
				RegisterClassError );
		return RESULT_FAIL;
	}

	RECT WndRect = { 16, 16, MAX_RES_WIDTH + 16, MAX_RES_HEIGHT + 16 };
	
	// create the window for compBuff sync. 
	m_hWindow = CreateWindowExW
			(
				0,
				L"WGF11SharedResources",
				L"CompositionBuffer Window",
				WS_OVERLAPPEDWINDOW | WS_VISIBLE,
				WndRect.left,
				WndRect.top,
				WndRect.right - WndRect.left,
				WndRect.bottom - WndRect.top,
				NULL,
				NULL,
				m_WindowClass.hInstance,
				NULL
			);

	if ( m_hWindow == NULL)
	{
		DWORD CreateWindowError = GetLastError();
		WriteToLog(	"SharedResourcesTest::SyncCompBuffer(): CreateWindowEx failed with error %u.",
				CreateWindowError );
		return RESULT_FAIL;
	}

	return SharedResourcesTest::Setup();
}
	
void ShareFromCompBuffToDX11Test::Cleanup()
{
	if ( m_hWindow != NULL )
	{
		BOOL bResult = DestroyWindow( m_hWindow );
		if ( !bResult )
		{
			DWORD lastError = GetLastError();
			WriteToLog(	_T( "SharedResourcesTest::SyncCompBuffer():  DestroyWindow failed with error %u." ),
				lastError );
		}
		m_hWindow = NULL;
	}

	return SharedResourcesTest::Cleanup();
}


//-----------------------------------------------------------------------------
TEST_RESULT SharedResourcesTest::CreateAndVerifyEvent(HANDLE &hEvent, tstring EventName)
{
	SECURITY_ATTRIBUTES sec;
	sec.nLength = sizeof(SECURITY_ATTRIBUTES);
	sec.bInheritHandle= TRUE;
	sec.lpSecurityDescriptor = NULL;

	hEvent = CreateEvent( 
		&sec,         // default security attributes
		FALSE,         // auto-reset event
		FALSE,         // initial state is NOT signaled
		EventName.c_str()  // object name
	);	
	if (hEvent == NULL) 
	{ 
		WriteToLog(_T( "SharedResourcesTest::CreateAndVerifyEvent - CreateEvent failed (%d) for %s."),
			GetLastError(), EventName.c_str());
		return RESULT_SKIP;
	}
	return RESULT_PASS;
}

//-----------------------------------------------------------------------------
TEST_RESULT SharedResourcesTest::Setup()
{
	TEST_RESULT tr = CD3D11Test::Setup( );
	if( tr != RESULT_PASS )
		return tr;

	//create 9 object
	if (!g_App.IsSession0() && !GetFramework()->IsMobile())
	{
		HRESULT hr = S_FALSE;
		hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &m_pD3D9);

		if (hr != S_OK)
		{
			WriteToLog(_T("SharedResourcesTest::Setup() - Direct3DCreate9Ex() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
			return RESULT_FAIL;
		}
	}

	// the following events are used to sync the two processes at sync points other than shared resource access
	tr = CreateAndVerifyEvent(m_exitChildProcessEvent, _T("ExitChildProcess"));
	if (tr != RESULT_PASS)
		return tr;

	tr = CreateAndVerifyEvent(m_finishUpdateSharedMemEvent, _T("FinishUpdateSharedMem"));
	if (tr != RESULT_PASS)
		return tr;

	tr = CreateAndVerifyEvent(m_endChildCaseWithError, _T("EndChildCaseWithError"));
	if (tr != RESULT_PASS)
		return tr;
	
	
	// the following two events are used to sync the shared resource access of the two processes
	// they are alternatives of KeyedMutex
	tr = CreateAndVerifyEvent(m_startChildResAccessEvent, _T("StartChildResAccess"));
	if (tr != RESULT_PASS)
		return tr;

	tr = CreateAndVerifyEvent(m_endChildResAccessEvent, _T("EndChildResAccess"));
	if (tr != RESULT_PASS)
		return tr;

	// create shared memory
	m_hFileMap = ::CreateFileMapping(INVALID_HANDLE_VALUE , 
										 NULL, 
										 PAGE_READWRITE, 
										 0,
										 sizeof(RESOURCE_CASE),
										 _T("SharedResourcesMem") );
	   
	if(NULL == m_hFileMap)
	{
		WriteToLog( _T( "SharedResourcesTest::Setup() - CreateFileMapping() failed. (%d)" ), GetLastError());
		return RESULT_SKIP;
	}
	
	 //create the child process
	 const UINT maxCommandLineLength = 512;

	 char szCommandLine[maxCommandLineLength];
	 memset(szCommandLine, 0, sizeof(szCommandLine));

	 _snprintf_s(szCommandLine, ARRAYSIZE(szCommandLine), ARRAYSIZE(szCommandLine) - 1, _T("%s -logfilename:WGF11SharedResourcesChild -child "), GetCommandLine()); //pass on current command line args
	 //_snprintf_s(szCommandLine, ARRAYSIZE(szCommandLine), ARRAYSIZE(szCommandLine) - 1, _T("c:\\debuggers\\ntsd.exe %s -nolog -child"), GetCommandLine()); //pass on current command line args
	 //_snprintf_s(szCommandLine, ARRAYSIZE(szCommandLine), ARRAYSIZE(szCommandLine) - 1, _T("%s -nolog -child "), GetCommandLine()); //pass on current command line args

	 STARTUPINFO si;
	 ZeroMemory( &si, sizeof(si) );
	 si.cb = sizeof(si);

	 // Create anonymous pipes to deliberately ignore and discard child process I/O.
	 // The pipes are currently leaked - we could probably do better to remember these
	 // handles more broadly, re-use them for multiple child processes, and clean them up.
	 // But for right now it's not a priority.
	 SECURITY_ATTRIBUTES saAttr; 
	 saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	 saAttr.bInheritHandle = TRUE; 
	 saAttr.lpSecurityDescriptor = NULL; 

	 HANDLE hOutRead, hOutWrite, hInRead, hInWrite;
	 if( CreatePipe(&hOutRead, &hOutWrite, &saAttr, 0) &&
		 CreatePipe(&hInRead, &hInWrite, &saAttr, 0) )
	 {
		 CloseHandle(hOutRead);
		 CloseHandle(hInWrite);

		 si.hStdError = hOutWrite;
		 si.hStdOutput = hOutWrite;
		 si.hStdInput = hInRead;
		 si.dwFlags = STARTF_USESTDHANDLES;
	 }

	  if( false == CreateProcess(NULL, 
								szCommandLine, 
								NULL, 
								NULL, 
								TRUE, 
								0, 
								NULL, 
								NULL, 
								&si, 
								&m_ProcInf ) ) 
	{
		WriteToLog(0, _T("SharedResourcesTest::Setup() - CreateProcess failed.") );
		return RESULT_SKIP;
	}

	return RESULT_PASS;
}


//--------------------------------------------------------------------------
TEST_RESULT SharedResourcesTest::CreateDataBytes(UINT width, UINT height, DXGI_FORMAT format)
{
	
	UINT bytesPerElement = GetBitsPerElement( format ) / 8;
	UINT sizeInBytes = width * height * bytesPerElement;
	
	for (UINT i = 0; i < sizeInBytes; ++i)
	{	
		// 122 is 0x7a. Values larger than 0x7a will result in NaN for 16-bit float
		m_pDataByte1[i] = 1 + i % 122;	
		//m_pDataByte2 is used to update texture data in opened resource
		m_pDataByte2[i] = i % 30;
	}
	return RESULT_PASS;
}

//--------------------------------------------------------------------------
TEST_RESULT SharedResourcesTest::CreateDataBytesGDI(UINT width, UINT height, DXGI_FORMAT format)
{
	UINT bytesPerElement = GetBitsPerElement( format ) / 8;
	UINT sizeInBytes = width * height * bytesPerElement;
	
	for (UINT i = 0; i < sizeInBytes; ++i)
	{
		// (0, 0, 255, 0) for GDI source resource. Set 0 for alpha channel.
		if (  i%4 == 2 )
			m_pDataByte1[i] = 255;
		else
			m_pDataByte1[i] = 0;

		// (255, 255, 0, 0) for GDI dest resource. Set 0 for alpha channel.
		if (  i%4 <=1 )
			m_pDataByte2[i] = 255;
		else
			m_pDataByte2[i] = 0;			
	}
	return RESULT_PASS;
}

//--------------------------------------------------------------------------
const	char alpha = 0xFF;
const	WORD alpha_word = 0xFFFF;
//YELLOW
const	char cy0 = 0xD2; //210; 
const	char cu0 = 0x10; //16; 
const	char cv0 = 0x92; //146; 
//RED
const	char cy1 = 0x51; //81; 
const	char cu1 = 0x5A; //90; 
const	char cv1 = 0xF0; //240; 

//YELLOW
const WORD cy0_word = (WORD)(cy0 << 8); //0xD2; //210; 
const WORD cu0_word = (WORD)(cu0 << 8); //0x10; //16; 
const WORD cv0_word = (WORD)(cv0 << 8); //0x92; //146; 
//RED
const WORD cy1_word = (WORD)(cy1 << 8); //0x51; //81; 
const WORD cu1_word = (WORD)(cu1 << 8); //0x5A; //90; 
const WORD cv1_word = (WORD)(cv1 << 8); //0xF0; //240; 

TEST_RESULT SharedResourcesTest::WriteStagingVideoResource(UINT width, UINT height, DXGI_FORMAT format, 
	ID3D11Device *pDevice, 
	ID3D11DeviceContext* pImmediateContext, 
	ID3D11Texture2D *pTexStaging,
	BYTE *pData)
{
	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;
	D3D11_MAPPED_SUBRESOURCE mappedResource = {0};       
	if( FAILED( hr = pImmediateContext->Map(pTexStaging, 0, D3D11_MAP_WRITE, 0, &mappedResource) ) )
	{
		WriteToLog( _T( "SharedResourcesTest::CreateDataBytesVideo() - Mapping failed for video staging texture, hr = %s" ), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}
	
	UINT bytesPerRow = 0;
	UINT numOfRows = 0;
	
	switch(format)
	{
	// 8 bits per channel;  
	// -------------------------------------------------------------------------------
	case DXGI_FORMAT_NV12: 
	case DXGI_FORMAT_NV11:
	case DXGI_FORMAT_420_OPAQUE:
		{
		bytesPerRow = width;
		numOfRows = height + height/2;
		break;
		}
		
	case DXGI_FORMAT_YUY2:
		{
		bytesPerRow = width * 2;
		numOfRows = height;
		break;
		}
	case DXGI_FORMAT_AYUV:
		{
		bytesPerRow = width * 4;
		numOfRows = height;
		break;
		}
	// 16 or 10 bits per channel;  
	// -------------------------------------------------------------------------------
	case DXGI_FORMAT_Y210: 
	case DXGI_FORMAT_Y216:
		{
		UINT wordsPerRow = width * 2;
		bytesPerRow = wordsPerRow * 2;
		numOfRows = height;
		break;
		}
	case DXGI_FORMAT_P010: 
	case DXGI_FORMAT_P016:
		{
		UINT wordsPerRow = width;
		bytesPerRow = wordsPerRow * 2;
		numOfRows = height + height/2;
		break;
		}

	case DXGI_FORMAT_Y416: //UYVA
		{
		UINT wordsPerRow = width * 4;
		bytesPerRow = wordsPerRow * 2;
		numOfRows = height;
		break;
		}

	case DXGI_FORMAT_Y410:
		{
		UINT wordsPerRow = width * 2;
		bytesPerRow = wordsPerRow * 2;
		numOfRows = height;
		break;
		}

	default:
		{
			WriteToLog( _T( "SharedResourcesTest::CreateDataBytesVideo() - Test error: wrong video format." ));
			tr = RESULT_FAIL;
			goto exit;
		}
	}

	for (UINT i = 0; i < numOfRows; i++)
	{
		BYTE *pYData = (BYTE*)mappedResource.pData + i * mappedResource.RowPitch;
		memcpy(pYData, pData, bytesPerRow);
		pData += bytesPerRow;
	}

exit:
	pImmediateContext->Unmap(pTexStaging, 0);
	return tr;
}

TEST_RESULT SharedResourcesTest::CreateDataBytesVideo(UINT width, UINT height, DXGI_FORMAT format)
{
	
	HRESULT hr = S_OK;
	memset(m_pDataByte1, 0, MAX_DATA_BYTES);
	memset(m_pDataByte2, 0, MAX_DATA_BYTES);

	
	float bytesPerElement = (float)GetBitsPerElementVideo(format) / 8;
	assert(bytesPerElement != 0);
	UINT sizeInBytes = (UINT)(width * height * bytesPerElement);
	
	switch(format)
	{
	// 8 bits per channel;  
	// -------------------------------------------------------------------------------
	case DXGI_FORMAT_NV12: 
	case DXGI_FORMAT_NV11:
	case DXGI_FORMAT_420_OPAQUE:
		{
			UINT rawBytesNum = 0;
			// write Y data
			for (UINT i = 0; i < height; i++)
			{
				for (UINT j = 0; j < width; j++)
				{
					assert(rawBytesNum < sizeInBytes);
					m_pDataByte1[rawBytesNum] = cy0;
					m_pDataByte2[rawBytesNum] = cy1;
					rawBytesNum++;
				}
			}
			// write UV data
			for (UINT i = 0; i < height/2; i++)
			{
				for (UINT j = 0; j < width/2; j++)
				{
					assert(rawBytesNum + 1 < sizeInBytes);
					m_pDataByte1[rawBytesNum] = cu0;
					m_pDataByte2[rawBytesNum] = cu1;
					m_pDataByte1[rawBytesNum + 1] = cv0;
					m_pDataByte2[rawBytesNum + 1] = cv1;
					rawBytesNum += 2;
				}
			}
			break;
		}
	case DXGI_FORMAT_YUY2:
		{
			UINT rawBytesNum = 0;
			UINT bytesPerRow = width * 2;
			for (UINT i = 0; i < height; i++)
			{
				
				for (UINT j = 0; j < bytesPerRow; j += 4)
				{
					assert(j + 3 < bytesPerRow);
					assert(rawBytesNum + 3 < sizeInBytes);
					m_pDataByte1[rawBytesNum] = cy0;
					m_pDataByte2[rawBytesNum] = cy1;
					m_pDataByte1[rawBytesNum + 1] = cu0;
					m_pDataByte2[rawBytesNum + 1] = cu1;
					m_pDataByte1[rawBytesNum + 2] = cy0;
					m_pDataByte2[rawBytesNum + 2] = cy1;
					m_pDataByte1[rawBytesNum + 3] = cv0;
					m_pDataByte2[rawBytesNum + 3] = cv1;
					rawBytesNum += 4;
				}
			}
			break;
		}
	case DXGI_FORMAT_AYUV:
		{
			UINT rawBytesNum = 0;
			UINT bytesPerRow = width * 4;
			for (UINT i = 0; i < height; i++)
			{
							
				for (UINT j = 0; j < bytesPerRow; j += 4)
				{
					assert(j + 3 < bytesPerRow);
					assert(rawBytesNum + 3 < sizeInBytes);
					m_pDataByte1[rawBytesNum] = cv0;
					m_pDataByte2[rawBytesNum] = cv1;
					m_pDataByte1[rawBytesNum + 1] = cu0;
					m_pDataByte2[rawBytesNum + 1] = cu1;
					m_pDataByte1[rawBytesNum + 2] = cy0;
					m_pDataByte2[rawBytesNum + 2] = cy1;
					m_pDataByte1[rawBytesNum + 3] = alpha;
					m_pDataByte2[rawBytesNum + 3] = alpha;
					rawBytesNum += 4;
				}
			}
			break;
		}
	// 16 or 10 bits per channel;  
	// -------------------------------------------------------------------------------
	case DXGI_FORMAT_Y210: 
	case DXGI_FORMAT_Y216:
		{
			UINT rawWordsNum = 0;
			UINT sizeInWords = sizeInBytes/2;
			UINT wordsPerRow = width * 2;
			WORD *pDataWord1 = (WORD*)m_pDataByte1;
			WORD *pDataWord2 = (WORD*)m_pDataByte2;
		
			for (UINT i = 0; i < height; i++)
			{
				for (UINT j = 0; j < wordsPerRow; j += 4)
				{
					assert(j + 3 < wordsPerRow);
					assert(rawWordsNum + 3 < sizeInWords);
					pDataWord1[rawWordsNum] = cy0_word;
					pDataWord2[rawWordsNum] = cy1_word;
					pDataWord1[rawWordsNum + 1] = cu0_word;
					pDataWord2[rawWordsNum + 1] = cu1_word;
					pDataWord1[rawWordsNum + 2] = cy0_word;
					pDataWord2[rawWordsNum + 2] = cy1_word;
					pDataWord1[rawWordsNum + 3] = cv0_word;
					pDataWord2[rawWordsNum + 3] = cv1_word;
					rawWordsNum += 4;
				}
			}
			break;
		}
		
	case DXGI_FORMAT_P010: 
	case DXGI_FORMAT_P016:
		{
			UINT rawWordsNum = 0;
			UINT sizeInWords = sizeInBytes/2;
			WORD *pDataWord1 = (WORD*)m_pDataByte1;
			WORD *pDataWord2 = (WORD*)m_pDataByte2;
			// write Y data
			for (UINT i = 0; i < height; i++)
			{
				for (UINT j = 0; j < width; j++)
				{
					assert(rawWordsNum < sizeInWords);
					pDataWord1[rawWordsNum] = cy0_word;
					pDataWord2[rawWordsNum] = cy1_word;
					rawWordsNum++;
				}
			}
			// write UV data
			for (UINT i = 0; i < height/2; i++)
			{
				for (UINT j = 0; j < width/2; j++)
				{
					assert(rawWordsNum + 1 < sizeInWords);
					pDataWord1[rawWordsNum] = cu0_word;
					pDataWord2[rawWordsNum] = cu1_word;
					pDataWord1[rawWordsNum + 1] = cv0_word;
					pDataWord2[rawWordsNum + 1] = cv1_word;
					rawWordsNum += 2;
				}
			}
			break;
		}
	case DXGI_FORMAT_Y416: //UYVA
		{
			UINT rawWordsNum = 0;
			UINT wordsPerRow = width * 4;
			UINT sizeInWords = sizeInBytes/2;
			WORD *pDataWord1 = (WORD*)m_pDataByte1;
			WORD *pDataWord2 = (WORD*)m_pDataByte2;
			for (UINT i = 0; i < height; i++)
			{		
				for (UINT j = 0; j < wordsPerRow; j += 4)
				{
					assert(j + 3 < wordsPerRow);
					assert(rawWordsNum + 3 < sizeInWords);
					pDataWord1[rawWordsNum] = cu0_word;
					pDataWord2[rawWordsNum] = cu1_word;
					pDataWord1[rawWordsNum + 1] = cy0_word;
					pDataWord2[rawWordsNum + 1] = cy1_word;
					pDataWord1[rawWordsNum + 2] = cv0_word;
					pDataWord2[rawWordsNum + 2] = cv1_word;
					pDataWord1[rawWordsNum + 3] = alpha_word;
					pDataWord2[rawWordsNum + 3] = alpha_word;
					rawWordsNum += 4;
				}
			}
			break;
		}
	case DXGI_FORMAT_Y410:
		{
			UINT rawDWordsNum = 0;
			UINT sizeInDWords = sizeInBytes/4;
						
			DWORD *pDataDWord1 = (DWORD*)m_pDataByte1;
			DWORD *pDataDWord2 = (DWORD*)m_pDataByte2;
			DWORD DWORDColor1 =  (DWORD)( ( alpha << 30 ) + ( cv0 << 22 ) + ( cy0 << 12 ) + ( cu0 << 2 ) );
			DWORD DWORDColor2 =  (DWORD)( ( alpha << 30 ) + ( cv1 << 22 ) + ( cy1 << 12 ) + ( cu1 << 2 ) );
			for (UINT i = 0; i < height; i++)
			{
			
				for (UINT j = 0; j < width; j++)
				{
					assert(rawDWordsNum < sizeInDWords);
					pDataDWord1[rawDWordsNum] = DWORDColor1;
					pDataDWord2[rawDWordsNum] = DWORDColor2;
					rawDWordsNum++;
				}
			}
			break;
		}

	default:
		{
			WriteToLog( _T( "SharedResourcesTest::CreateDataBytesVideo() - Test error: wrong video format." ));
			return RESULT_FAIL;
		}
	}

	return RESULT_PASS;
}

/** Create all needed devices and the resource to be shared. */
TEST_RESULT SharedResourcesTest::SetupTestCase()
{	
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;

	if (g_App.m_D3DOptions.DriverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		WriteToLog("Skipping: Ref doesn't support shared resources.");
		return RESULT_SKIP;
	}

	if (g_App.IsSession0() && (m_srcDevice == 900 || m_destDevice == 900))
	{
		WriteToLog(_T("Skipping: D3D9 device is not supported in session 0."));
		return RESULT_SKIP;
	}

	if ( g_App.m_D3DOptions.FeatureLevel < m_destDeviceFL || g_App.m_D3DOptions.FeatureLevel < m_srcDeviceFL )
	{
		// e.g, when running on FL10.1, skip cases with FL11.0 for dest or source device. This is necessary for WHQL mode
		WriteToLog("Skipping: The feature level of dest or source device is higher than the test requirement.");
		return RESULT_SKIP;
	}

	if ( ( m_srcDevice >= 1000 && m_destDevice == 900 ) || ( m_srcDeviceFL >= D3D_FEATURE_LEVEL_10_0 && m_destDeviceFL < D3D_FEATURE_LEVEL_10_0 ) )
	{
		switch(m_Format)
		{
		default:
			break;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			WriteToLog(_T("Skipping: Cannot share BGRA_SRGB from 10 or 11 to 9."));
			return RESULT_SKIP;
		}
	}

	if( m_srcDeviceFL < D3D_FEATURE_LEVEL_10_0 || m_destDeviceFL < D3D_FEATURE_LEVEL_10_0 )
	{
		if( m_Format == DXGI_FORMAT_R8G8B8A8_UNORM ||  m_Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB )
		{
			WriteToLog(_T("Skipping: Cannot create shared resource with R8G8B8A8_UNORM* on FL9."));
			return RESULT_SKIP;
		}
	}

	// Skipping due to "won't fix" runtime bug Window7 755687
	if ( m_srcDevice == 1010 && m_destDevice == 1100 && m_Format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB )
	{
		WriteToLog("Skipping due to \"won't fix\" runtime bug Window7 755687.");
		return RESULT_SKIP;
	}

	UINT WidthAlign = CD3D10FormatHelper_Test::GetWidthAlignment( m_Format );
	UINT HeightAlign = CD3D10FormatHelper_Test::GetHeightAlignment( m_Format );
	if( m_ResWidth % WidthAlign != 0 ||
		m_ResHeight % HeightAlign != 0 )
	{
		WriteToLog("Skipping due to alignment restriction for video formats.");
		return RESULT_SKIP;
	}

	// Skipping level9 cases on 10+ hardware which does not support Level9PerfFeatures
	if( g_App.getSkipLevel9Cases() && ( m_destDeviceFL < D3D_FEATURE_LEVEL_10_0 || m_srcDeviceFL < D3D_FEATURE_LEVEL_10_0 ) &&
		( m_Format == DXGI_FORMAT_A8_UNORM || m_Format == DXGI_FORMAT_R8_UNORM || m_Format == DXGI_FORMAT_R8G8_UNORM ) )
	{ 
		WriteToLog("Skipping level9 new format cases on 10+ hardware which does not support Level9PerfFeatures.");
		return RESULT_SKIP;
	}

	if ( !g_App.IsAtLeastWDDM1_3() )
	{
		switch(m_Format)
		{
		case DXGI_FORMAT_BC1_UNORM:     
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC2_UNORM:     
		case DXGI_FORMAT_BC2_UNORM_SRGB: 
		case DXGI_FORMAT_BC3_UNORM:     
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_R8G8_UNORM:
			WriteToLog("Skipping format cases that aren't supported on hardware below WDDM 1.3.");
			return RESULT_SKIP;
		}
	}

	//we need the equivalent 9 format
	if (m_srcDevice == 900 || m_destDevice == 900)	
	{
		D3DFORMAT D3DFormat = GetD3DFormat(m_Format);

		//check device format
		D3DDISPLAYMODE CurrentMode;
		if (FAILED(hr = m_pD3D9->GetAdapterDisplayMode(0, &CurrentMode)))
		{
			WriteToLog( _T( "SharedResourcesTest::SetupTestCase() - m_pD3D9->GetAdapterDisplayMode() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			return RESULT_FAIL;
		}
		if (FAILED(hr = m_pD3D9->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, CurrentMode.Format, 0, D3DRTYPE_TEXTURE, D3DFormat)))
		{
			WriteToLog( _T( "SharedResourcesTest::SetupTestCase() - D3DFormat %d not supported on this device. hr = %s" ), D3DFormat, D3DHResultToString(hr).c_str());
			return RESULT_SKIP;
		}
	}

	// create shared memory
	if (m_testType != SR_OTHER_PROCESS)
	{
		m_sharedMemory = new RESOURCE_CASE;
	}
	else
	{
		//map shared memory
		m_sharedMemory = (RESOURCE_CASE*)::MapViewOfFile(m_hFileMap,
														 FILE_MAP_WRITE,
														 0,
														 0,
														 sizeof(RESOURCE_CASE));
		if (NULL == m_sharedMemory)
		{
			WriteToLog( _T( "SharedResourcesTest::SetupTestCase() - MapViewOfFile() failed. (%d)" ), GetLastError());
			tr = RESULT_SKIP;
			goto failed_setup;
		}
	}

	m_pDataByte1 = new BYTE[MAX_DATA_BYTES];
	m_pDataByte2 = new BYTE[MAX_DATA_BYTES];
	memset(m_pDataByte1, 0, MAX_DATA_BYTES);
	memset(m_pDataByte2, 0, MAX_DATA_BYTES);

	// Create data for m_pDataByte1 and m_pDataByte2
	if ( m_srcDevice == 1102 || m_destDevice == 1102 ) // GDI
		tr = CreateDataBytesGDI(m_ResWidth, m_ResHeight, m_Format);
	else if ( GetFlags(m_Format) & FF_VIDEO ) // video 
		tr = CreateDataBytesVideo(m_ResWidth, m_ResHeight, m_Format);
	else
		tr = CreateDataBytes(m_ResWidth, m_ResHeight, m_Format);
	if (tr != RESULT_PASS)
	{
		WriteToLog( _T( "SharedResourcesTest::SetupTestCase() - Create data for m_pDataByte1 and m_pDataByte2 failed. " ));
		goto failed_setup;
	}

	m_FormatBlockSize = 1;
	switch(m_Format)
	{
	case DXGI_FORMAT_BC1_UNORM:     
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		m_FormatBlockSize = 8;
		break;
	case DXGI_FORMAT_BC2_UNORM:     
	case DXGI_FORMAT_BC2_UNORM_SRGB: 
	case DXGI_FORMAT_BC3_UNORM:     
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		m_FormatBlockSize = 16;
		break;
	}

	//CREATE SOURCE DEVICE
	switch (m_srcDevice)
	{
	case (1100):	
		if( m_bCompBuffer ) 
		{
			// Skipping for a won't fix OS bug where the dest FL is less than the src FL. 
			// Only test the really world compositionbuffer scenerio where DWM always opens the composition Buffer, 
			// and DWM always uses the max feature level available
			if( m_destDeviceFL < m_srcDeviceFL )
			{
				WriteToLog( "SharedResourcesTest::SetupTestCase(): Skipping cases where the dest FL is less than the src FL for CompositionBuffers." );
				tr = RESULT_SKIP;
				goto failed_setup;
			}
			if( m_srcDeviceFL < D3D_FEATURE_LEVEL_10_0 && m_bStereo )
			{
				WriteToLog( "SharedResourcesTest::SetupTestCase(): Skipping. Stereo is only supported on FL10+." );
				tr = RESULT_SKIP;
				goto failed_setup;
			}
			if( m_WriteMethod == SR_INIT_DATA)
			{
				WriteToLog( "SharedResourcesTest::SetupTestCase(): Skipping. INIT_DATA does not apply to CompositionBuffers." );
				tr = RESULT_SKIP;
				goto failed_setup;
			}
			m_ArraySize = m_bStereo? 2: 1; 
			m_pSrcObjs = new SrcObjectsCompBuffer(this, m_srcDeviceFL);
		}
		else
			m_pSrcObjs = new SrcObjects11(this, m_srcDeviceFL); 
		break;
	case (1101):	m_pSrcObjs = new SrcObjectsVideo(this, m_srcDeviceFL); break;
	case (1102):	m_pSrcObjs = new SrcObjectsGDI(this, m_srcDeviceFL); break;
	case (1010):	m_pSrcObjs = new SrcObjects10_1(this, m_srcDeviceFL);		break;
	case (1000):	m_pSrcObjs = new SrcObjects10(this);		break;
	case (900):		m_pSrcObjs = new SrcObjects9(this);	break;
	
	default: 
		WriteToLog( _T( "SharedResourcesTest::SetupTestCase(): wrong m_srcDevice." ));
		return RESULT_FAIL;
	}
	if (!m_pSrcObjs)
	{
		tr = RESULT_BLOCKED;
		goto failed_setup;
	}
	
	tr = m_pSrcObjs->SetupTestCase(this);
	if (!(tr == RESULT_PASS))
	{
		goto failed_setup;
	}

	// if the dest device is video device we need to see if the test case should skip before m_pSrcObjs->SetupResources
	if ( m_destDevice == 1101)
	{
		DestObjects *pTempDestObjs = new DestObjectsVideo(this, m_destDeviceFL); 
		if (!pTempDestObjs)
		{
			tr = RESULT_BLOCKED;
			goto failed_setup;
		}
		tr = pTempDestObjs->SetupTestCase(this);
		if (!(tr == RESULT_PASS))
		{
			delete pTempDestObjs;
			goto failed_setup;
		}
		delete pTempDestObjs;
	}

	tr = m_pSrcObjs->SetupResources(this, m_pDataByte1, m_WriteMethod);
	if (!(tr == RESULT_PASS))
	{
		goto failed_setup;
	}

	//CREATE DEST DEVICE
	if (m_testType == SR_SAME_PROCESS)
	{
		switch (m_destDevice)
		{
		case (1100):	m_pDestObjs = new DestObjects11(this, m_destDeviceFL);	break;	
		case (1101):	m_pDestObjs = new DestObjectsVideo(this, m_destDeviceFL); break;
		case (1102):	m_pDestObjs = new DestObjectsGDI(this, m_destDeviceFL); break;
		case (1010):	m_pDestObjs = new DestObjects10_1(this, m_destDeviceFL);		break;
		case (1000):	m_pDestObjs = new DestObjects10(this);		break;
		case (900):		m_pDestObjs = new DestObjects9(this);		break;
		
		default: 
			WriteToLog( _T( "SharedResourcesTest::SetupTestCase(): wrong m_destDevice." ));
			tr = RESULT_FAIL;
			goto failed_setup;
		}
		if (!m_pDestObjs)
		{
			tr = RESULT_BLOCKED;
			goto failed_setup;
		}
		tr = m_pDestObjs->SetupTestCase(this);
		if (!(tr == RESULT_PASS))
		{
			goto failed_setup;
		}
	}

	if ((m_testType == SR_OTHER_PROCESS))
	{
		
		//set values to pass to child process
		m_sharedMemory->SharedHandle = m_pSrcObjs->m_SharedHandle;
		m_sharedMemory->Format = m_Format;
		m_sharedMemory->Device = m_destDevice;
		m_sharedMemory->DeviceFeatureLevel = m_destDeviceFL;
		m_sharedMemory->SrcFeatureLevel = m_srcDeviceFL;
		m_sharedMemory->Result = E_FAIL;
		m_sharedMemory->SyncMethod = m_SyncMethod;
		m_sharedMemory->WriteMethod = m_WriteMethod;
		m_sharedMemory->ChildAccessMethod = m_ChildAccessMethod;
		m_sharedMemory->Width = m_ResWidth;
		m_sharedMemory->Height = m_ResHeight;
		m_sharedMemory->ArraySize = m_ArraySize;
		m_sharedMemory->MipLevels = m_MipLevels;
		m_sharedMemory->isNTHandle = m_bMiscSharedNTHandle;
		m_sharedMemory->isGDICompatible = m_bMiscGDICompatible;
		m_sharedMemory->isShaderResource = m_bBindShaderResource;
		m_sharedMemory->isRenderTarget = m_bBindRenderTarget;
		m_sharedMemory->isCapture = m_bBindCapture;
		m_sharedMemory->isMiscGenMips = m_bMiscGenMips;
		memcpy(m_sharedMemory->TestData, m_pDataByte1, MAX_DATA_BYTES);
		memcpy(m_sharedMemory->TestData2, m_pDataByte2, MAX_DATA_BYTES);
		strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "SharedResourcesTest::SetupTestCase() - Child not started.");

		//tell child process to read shared memory
		if (! SetEvent(m_finishUpdateSharedMemEvent) ) 
		{ 
			WriteToLog( _T( "SharedResourcesTest::SetupTestCase() - Failed to signal child process to start reading shared memory. (%d)"),
				GetLastError());
			tr = RESULT_SKIP;
			goto failed_setup;
		}

	}

	return tr;

failed_setup:
	if (m_pDataByte1)
	{
		delete[] m_pDataByte1;
		m_pDataByte1 = NULL;
	}

	if (m_pDataByte2)
	{
		delete[] m_pDataByte2;
		m_pDataByte2 = NULL;
	}

	if (m_pSrcObjs)
	{
		delete m_pSrcObjs;
		m_pSrcObjs = NULL;
	}

	if (m_pDestObjs)
	{
		delete m_pDestObjs;
		m_pDestObjs = NULL;
	}
	return tr;

}


////////////////////////////////////////////
const char g_VS_L9[] = 
	"struct VSOut \n"
	"{"
	"	float4 vPos : SV_POSITION; \n"
	"	float2 vTexCoords : TEXCOORD; \n"
	"};"
	"struct VSIn \n"
	"{"
	"	float4 vPos : POSITION; \n"
	"	float2 vTexCoords : TEXCOORD; \n"
	"};"
	"VSOut vs_main( VSIn input )\n"
	"{\n"
	"	VSOut output; \n" 
	"	output.vPos = input.vPos; \n"
	"	output.vTexCoords = input.vTexCoords; \n"
	"	return output; \n"
	"}\n"
	;
const char g_PS_L9[] = 
	"struct PSIn \n"
	"{"
	"	float4 vPos : SV_POSITION; \n"
	"	float2 vTexCoords : TEXCOORD; \n"
	"};"
	"Texture2D <FORMAT_TYPE> Tex2D; \n"
	"SamplerState s; \n"
	"FORMAT_TYPE ps_main( PSIn input) : SV_TARGET \n"
	"{\n"
	"	return Tex2D.Sample( s, float2( input.vTexCoords.x, input.vTexCoords.y ) ); \n"
	"}\n"
	;

const char g_VS[] = 
	"float4 vs_main( float4 input : POSITION ): SV_POSITION \n"
	"{\n"
	"	return input; \n"
	"}\n"
	;

const char g_PS[] = 
	"Texture2D <FORMAT_TYPE> Tex2D; \n"
	"FORMAT_TYPE ps_main( float4 Pos : SV_POSITION ) : SV_TARGET \n"
	"{\n"
	"	return Tex2D.Load( int3(floor(Pos.x), floor(Pos.y), Pos.z ) ); \n"
	"}\n"
	;

TEST_RESULT SharedResourcesTest::CompileVsPsShaders(ID3D10Blob **ppVertexShaderBlob, ID3D10Blob **ppPixelShaderBlob, DWORD dxgiFormatFlags, bool bLevel9)
{
	TEST_RESULT tr = RESULT_PASS;
	ID3D10Blob *pErrorBlob = NULL;
	
	const char* formatType = dxgiFormatFlags & FF_UINT ? "uint4" : (dxgiFormatFlags & FF_SINT ? "int4" : "float4");
	const D3D10_SHADER_MACRO pMacroArray[] =
	{
		{ "FORMAT_TYPE", formatType },
		NULL
	};

	// Compile Vertex shader
	const char* shaderProfile = bLevel9? "vs_4_0_level_9_1": "vs_4_0";
	const char* shaderSource = bLevel9? g_VS_L9: g_VS;

	HRESULT hr = D3DX11CompileFromMemory( 
		shaderSource, 
		strlen( shaderSource ), 
		NULL, 
		NULL, 
		NULL, 
		"vs_main", 
		shaderProfile, 
		0, 
		0, 
		NULL, 
		ppVertexShaderBlob, 
		&pErrorBlob, 
		NULL 
	);

	if ( FAILED(hr) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T( "D3DX11CompileFromMemory failed for vertex shader. HR = %s. Compiler errors:"), D3DHResultToString(hr).c_str() );
		if (pErrorBlob)
		{
			LPVOID pBuf = pErrorBlob->GetBufferPointer();
			size_t bufSize = pErrorBlob->GetBufferSize();
			WriteToLog( _T( "%.*s\n" ), bufSize, pBuf);
		}
		goto Done;
	}

	// Compile Pixel shader
	SAFE_RELEASE( pErrorBlob );
	shaderProfile = bLevel9? "ps_4_0_level_9_1": "ps_4_0";
	shaderSource = bLevel9 ? g_PS_L9 : g_PS;
	
	hr = D3DX11CompileFromMemory( 
		shaderSource, 
		strlen( shaderSource ), 
		NULL, 
		pMacroArray,
		NULL, 
		"ps_main", 
		shaderProfile, 
		0, 
		0, 
		NULL, 
		ppPixelShaderBlob, 
		&pErrorBlob, 
		NULL 
	);

	if ( FAILED(hr) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T( "D3DX11CompileFromMemory failed for pixel shader. HR = %s. Compiler errors:"), D3DHResultToString(hr).c_str() );
		if (pErrorBlob)
		{
			LPVOID pBuf = pErrorBlob->GetBufferPointer();
			size_t bufSize = pErrorBlob->GetBufferSize();
			WriteToLog( _T( "%.*s\n" ), bufSize, pBuf);
		}
		goto Done;
	}

	Done:
	SAFE_RELEASE( pErrorBlob );
	return tr;

}

DestObjects9::DestObjects9(SharedResourcesTest *pTest ) :
	m_pDevice(pTest->Create9Device()),
	m_DXGIFormat(pTest->GetFormat()),
	m_D3DFormat(GetD3DFormat(pTest->GetFormat())), 
	m_pTexShared(NULL),
	m_Width(pTest->GetWidth()),
	m_Height(pTest->GetHeight())
{ }

SrcObjects9::SrcObjects9(SharedResourcesTest *pTest ) :
	DestObjects9(pTest),
	m_pSrcTex(NULL),
	m_pTex(NULL)
{ }

TEST_RESULT DestObjects9::SetupTestCase(SharedResourcesTest*)
{
	if (NULL == m_pDevice)
		return RESULT_SKIP;
	return RESULT_PASS;
}

TEST_RESULT SrcObjects9::SetupResources(SharedResourcesTest *pTest, BYTE* pDataByte1, SR_WRITE_METHOD)
{
	HRESULT hr;
	UINT bytesPerElement = GetBitsPerElement(m_DXGIFormat) / 8;
	
	if (FAILED (hr = m_pDevice->CreateTexture(m_Width,
												  m_Height,
												  1,
												  0,
												  m_D3DFormat,
												  D3DPOOL_SYSTEMMEM,
												  &m_pSrcTex,
												  NULL)))
	{
		pTest->WriteToLog(_T("SrcObjects9::SetupResources() - CreateTexture() Failed for m_pSrcTex. hr = %s"), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	} 

	//map the texture and fill pDataByte1 to use for verification
	D3DLOCKED_RECT LockedRect;
	if( FAILED( hr = m_pSrcTex->LockRect(0, &LockedRect, NULL, 0 ) ) )
	{
		pTest->WriteToLog(_T("SrcObjects9::SetupResources() - Failed to lock the d3d9 surface. hr = %s\n"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	BYTE* pData=(BYTE*) LockedRect.pBits;
	for( UINT y = 0; y < m_Height; y++ )
	{
		for( UINT x = 0; x < m_Width * bytesPerElement; x++ )
		{
			pData[(y * LockedRect.Pitch) + x] = pDataByte1[((y * m_Width * bytesPerElement) + x)];	 
		}
	}
	m_pSrcTex->UnlockRect(0);

	//create the shared resource
	if (FAILED(hr = m_pDevice->CreateTexture(m_Width, m_Height, 1, D3DPOOL_DEFAULT, m_D3DFormat, D3DPOOL_DEFAULT, &m_pTex, &m_SharedHandle)))
	{
		pTest->WriteToLog( _T( "SrcObjects9::SetupResources() - CreateTexture() as shared resource failed. hr = %s" ), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

TEST_RESULT SrcObjects9::UpdateSharedResource(SharedResourcesTest *pTest, BYTE* pDataByte1, SR_WRITE_METHOD WriteMethod)
{
	//fill texture
	HRESULT hr = m_pDevice->UpdateTexture(m_pSrcTex, m_pTex);

	if( FAILED(hr) )
	{
		pTest->WriteToLog(_T("SrcObjects9::UpdateSharedResource() - UpdateTexture Failed"));
		return RESULT_FAIL;
	}

	pTest->VerifyIDirect3DTexture9(m_pTex, pDataByte1, m_D3DFormat);

	if(FAILED( hr = pTest->WaitForEvent( NULL, NULL, m_pDevice) ))
	{
		pTest->WriteToLog( _T( "SrcObjects9::UpdateSharedResource() - WaitForEvent() failed. hr = %s" ), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

DestObjects10::DestObjects10(SharedResourcesTest *pTest) :
	m_pDevice(pTest->Create10Device()),
	m_pDevice11_1(NULL),
	m_DXGIFormat(pTest->GetFormat()),
	m_pTexShared(NULL),
	m_Width(pTest->GetWidth()),
	m_Height(pTest->GetHeight()),
	m_ArraySize(pTest->GetArraySize()),
	m_MipLevels(pTest->GetMipLevels()),
	m_pVertexShader(NULL),
	m_pPixelShader(NULL),
	m_pInputLayout(NULL),
	m_pVertexBuffer(NULL), 
	m_pSRVTex(NULL),
	m_pSRV(NULL),
	m_pRTV(NULL)
{}

DestObjects10::DestObjects10(SharedResourcesTest *pTest, ID3D10Device* pDev) :
	m_pDevice(pDev),
	m_pDevice11_1(NULL),
	m_DXGIFormat(pTest->GetFormat()),
	m_pTexShared(NULL),
	m_Width(pTest->GetWidth()),
	m_Height(pTest->GetHeight()),
	m_ArraySize(pTest->GetArraySize()),
	m_MipLevels(pTest->GetMipLevels()),
	m_pVertexShader(NULL),
	m_pPixelShader(NULL),
	m_pInputLayout(NULL),
	m_pVertexBuffer(NULL), 
	m_pSRVTex(NULL),
	m_pSRV(NULL),
	m_pRTV(NULL)
{ }

TEST_RESULT DestObjects10::SetupTestCase(SharedResourcesTest* pTest)
{
	if (NULL == m_pDevice)
		return RESULT_BLOCKED;

	UINT formatSupport;
	HRESULT hr = m_pDevice->CheckFormatSupport(m_DXGIFormat, &formatSupport);
	if (FAILED(hr))
	{
		pTest->WriteToLog( _T( "DestObjects10::SetupTestCase() - CheckFormatSupport(%s) failed. hr = %s" ), ToString(m_DXGIFormat).c_str(), D3DHResultToString(hr).c_str());
		return RESULT_SKIP;
	}

	UINT neededBits = D3D10_FORMAT_SUPPORT_TEXTURE2D;
	if (neededBits != (neededBits & formatSupport))
	{
		pTest->WriteToLog( _T( "DestObjects10::SetupTestCase() - DXGI Format %s not supported on this device. hr = %s" ), ToString(m_DXGIFormat).c_str(), D3DHResultToString(hr).c_str());
		return RESULT_SKIP;
	}

	if( pTest->GetBindRenderTarget() )
	{
		if( !( formatSupport & D3D11_FORMAT_SUPPORT_RENDER_TARGET ) )
		{
			pTest->WriteToLog( "DestObjects10::SetupTestCase(): Format %s does not support RenderTarget on this device. Skip test case.", ToString(m_DXGIFormat).c_str() );
			return RESULT_SKIP;
		}
	}

	return RESULT_PASS;
}

TEST_RESULT DestObjects10::DrawToResource( SharedResourcesTest *pTest, UINT arrayIndex, UINT mipIndex, void *pData,  ID3D10Texture2D *pRTVTex)
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;
	ID3D10Blob *pVertexShaderBlob = NULL;
	ID3D10Blob *pPixelShaderBlob = NULL;
	ID3D10SamplerState *pSamplerState = NULL;


	// use a compatible typed format for views if m_DXGIFormat is typeless
	DXGI_FORMAT viewFormat = m_DXGIFormat;
	if (FF_TYPELESS & GetFlags(m_DXGIFormat))
		viewFormat = pTest->GetViewFormat(m_DXGIFormat);
	tr = pTest->CompileVsPsShaders(&pVertexShaderBlob, &pPixelShaderBlob, GetFlags(viewFormat));
	if (tr != RESULT_PASS)
	{
		pTest->WriteToLog( _T( "DestObjects10::DrawToResource() - CompileVsPsShaders Failed." ) );
		tr = RESULT_FAIL;
		goto Done;
	}

	// Create shaders
	hr = m_pDevice->CreateVertexShader( 
		pVertexShaderBlob->GetBufferPointer(), 
		pVertexShaderBlob->GetBufferSize(), 
		&m_pVertexShader
	);
	if( FAILED( hr ) )
	{
		pTest->WriteToLog( _T( "DestObjects10::DrawToResource() - Failed to create vertex shader, hr=%s" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto Done;
	}
	hr = m_pDevice->CreatePixelShader( 
		pPixelShaderBlob->GetBufferPointer(), 
		pPixelShaderBlob->GetBufferSize(), 
		&m_pPixelShader
	);
	if( FAILED( hr ) )
	{
		pTest->WriteToLog( _T( "DestObjects10::DrawToResource() - Failed to create pixel shader, hr=%s" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto Done;
	}
	
	// create inputlayout
	D3D10_INPUT_ELEMENT_DESC pIEDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = m_pDevice->CreateInputLayout( 
		pIEDesc, 
		2, 
		pVertexShaderBlob->GetBufferPointer(), 
		pVertexShaderBlob->GetBufferSize(), 
		&m_pInputLayout );

	if( FAILED( hr ) )
	{
		pTest->WriteToLog( _T( "DestObjects10::DrawToResource() - CreateInputLayout() failed" ) );
		tr = RESULT_FAIL;
		goto Done;
	}


	// Create Vertex buffer; first 3 values are for POSITION and the last 2 values are for TEXCOORD
	float vertices[6][5] = 
	{
		{ -1.0f,	-1.0f,		0.5f,	0.f,	1.0f },
		{ -1.0f,	1.0f,		0.5f,	0.f,	0.f },
		{ 1.0f,		-1.0f,		0.5f,	1.0f,	1.0f },
		{ 1.0f,		-1.0f,		0.5f,	1.0f,	1.0f },
		{ -1.0f,	1.0f,		0.5f,	0.f,	0.f },
		{ 1.0f,		1.0f,		0.5f,	1.0f,	0.f }
	};

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( float ) * 6 * 5;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA initData;
	initData.pSysMem = vertices;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	hr = m_pDevice->CreateBuffer( &bd, &initData, &m_pVertexBuffer );
	if( FAILED( hr ) )
	{
		pTest->WriteToLog( _T( "DestObjects10::DrawToResource() - CreateBuffer failed, hr=%s" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto Done;
	}

	// create shader resource
	D3D10_TEXTURE2D_DESC tex2DDesc;
	tex2DDesc.ArraySize			= 1;
	tex2DDesc.BindFlags			= D3D10_BIND_SHADER_RESOURCE;
	tex2DDesc.CPUAccessFlags	= 0;
	tex2DDesc.Format			= m_DXGIFormat;
	tex2DDesc.Height			= m_Height >> mipIndex;
	tex2DDesc.Width				= m_Width >> mipIndex;
	tex2DDesc.MipLevels			= 1;
	tex2DDesc.MiscFlags			= 0;
	tex2DDesc.SampleDesc.Count	= 1;
	tex2DDesc.SampleDesc.Quality= 0;
	tex2DDesc.Usage				= D3D10_USAGE_DEFAULT;
	D3D10_SUBRESOURCE_DATA initDataTex2D;
	initDataTex2D.pSysMem = pData;
	initDataTex2D.SysMemPitch = tex2DDesc.Width * GetBitsPerElement( tex2DDesc.Format ) / 8;
	initDataTex2D.SysMemSlicePitch = 0;
	if( FAILED( hr = m_pDevice->CreateTexture2D( &tex2DDesc, &initDataTex2D, &m_pSRVTex) ) )
	{
		pTest->WriteToLog( _T( "DestObjects10::DrawToResource() - CreateTexture2D() failed for pSRVTex, hr = %s" ), D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Done;
	}

	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = viewFormat;
	srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	if( FAILED( hr = m_pDevice->CreateShaderResourceView( m_pSRVTex, &srvDesc, &m_pSRV) ) )
	{
		pTest->WriteToLog( _T( "DestObjects10::DrawToResource() - CreateShaderResourceView() failed for m_pSRV, hr = %s" ), D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Done;
	}

	// Create Render Target View
	D3D10_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = viewFormat;
	rtvDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.MipSlice = mipIndex;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.FirstArraySlice = arrayIndex;
	if( FAILED( hr = m_pDevice->CreateRenderTargetView( pRTVTex, &rtvDesc, &m_pRTV) ) )
	{
		pTest->WriteToLog( _T( "DestObjects10::DrawToResource() - CreateRenderTargetView() failed for m_pRTV, hr = %s" ), D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Done;
	}
	D3D10_VIEWPORT vp;
	vp.Width = m_Width >> mipIndex;
	vp.Height = m_Height >> mipIndex;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	// create and set sampler
	D3D10_SAMPLER_DESC SamplerDesc;
	SamplerDesc.Filter = D3D10_FILTER_MIN_MAG_MIP_POINT;
	SamplerDesc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.MipLODBias = 0;
	SamplerDesc.MaxAnisotropy = 1;
	SamplerDesc.ComparisonFunc = D3D10_COMPARISON_NEVER;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = FLT_MAX;
	if( FAILED( hr = m_pDevice->CreateSamplerState(&SamplerDesc, &pSamplerState ) ))
	{
		pTest->WriteToLog( "DestObjects10::DrawToResource() - CreateSamplerState() failed. hr = %s", D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto Done;
	}

	// Set rendering pipeline
	UINT stride = sizeof(vertices[0]);
	UINT offset = 0;
	m_pDevice->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pDevice->IASetInputLayout(m_pInputLayout);
	m_pDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_pDevice->VSSetShader(m_pVertexShader);
	m_pDevice->PSSetShader(m_pPixelShader);
	m_pDevice->PSSetShaderResources(0, 1, &m_pSRV);
	m_pDevice->PSSetSamplers(0, 1, &pSamplerState);
	m_pDevice->RSSetViewports(1, &vp);
	m_pDevice->OMSetRenderTargets(1, &m_pRTV, NULL);

	m_pDevice->Draw(6, 0);
	

Done:

	SAFE_RELEASE( pVertexShaderBlob );
	SAFE_RELEASE( pPixelShaderBlob );

	SAFE_RELEASE( m_pVertexShader );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pSRV );
	SAFE_RELEASE( m_pSRVTex );
	SAFE_RELEASE( m_pRTV );
	SAFE_RELEASE( pSamplerState );

	return tr;
}


SrcObjects10::SrcObjects10(SharedResourcesTest *pTest) :
	DestObjects10(pTest),
	m_pSrcTex(NULL),
	m_pTex(NULL)
{ }

SrcObjects10::SrcObjects10(SharedResourcesTest *pTest, ID3D10Device* pDev) :
	DestObjects10(pTest, pDev),
	m_pSrcTex(NULL),
	m_pTex(NULL)
{ }

TEST_RESULT SrcObjects10::SetupResources(SharedResourcesTest *pTest, BYTE* pDataByte1, SR_WRITE_METHOD WriteMethod)
{
	HRESULT hr = E_FAIL;
	TEST_RESULT tr = RESULT_PASS;
	UINT bytesPerElement = GetBitsPerElement(m_DXGIFormat) / 8;

	D3D10_SUBRESOURCE_DATA	 *pSubResData = NULL;
	pSubResData = new D3D10_SUBRESOURCE_DATA[m_ArraySize * m_MipLevels];

	IDXGIResource* pDXGIResource = NULL;
	IDXGIResource1* pDXGIResource1 = NULL;

	for ( UINT i = 0; i < m_ArraySize; i++ )
	{
		UINT subResWidth = m_Width;
		for ( UINT j = 0; j < m_MipLevels; j++ )
		{
			pSubResData[i * m_MipLevels + j].pSysMem = pDataByte1;
			pSubResData[i * m_MipLevels + j].SysMemPitch = subResWidth * bytesPerElement;
			subResWidth = subResWidth >> 1;
		}
	}

	//create texture
	{
		D3D10_TEXTURE2D_DESC tex2DDesc;
		tex2DDesc.ArraySize			= m_ArraySize;
		tex2DDesc.BindFlags			= 0;
		tex2DDesc.CPUAccessFlags	= 0;
		tex2DDesc.Format			= m_DXGIFormat;
		tex2DDesc.Height			= m_Height;
		tex2DDesc.Width				= m_Width;
		tex2DDesc.MipLevels			= m_MipLevels;
		tex2DDesc.MiscFlags			= 0;
		tex2DDesc.SampleDesc.Count	= 1;
		tex2DDesc.SampleDesc.Quality= 0;
		tex2DDesc.Usage				= D3D10_USAGE_DEFAULT;

		if( FAILED( hr = m_pDevice->CreateTexture2D( &tex2DDesc, pSubResData, &m_pSrcTex) ) )
		{
			pTest->WriteToLog( _T( "SrcObjects10::SetupResources() - CreateTexture2D() failed for source resource, hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}
	}

	
	//create the shared resource
	UINT bindFlags = 0;
	if ( pTest->GetBindRenderTarget() ) 
		bindFlags |= D3D10_BIND_RENDER_TARGET;
	if ( pTest->GetBindShaderResource() ) 
		bindFlags |= D3D10_BIND_SHADER_RESOURCE;

	UINT miscFlags = D3D10_RESOURCE_MISC_SHARED;
	if ( pTest->GetSyncMethod() == SR_KEYED_MUTEX )
		miscFlags = D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX;
	if ( pTest->GetMiscGenMips() )
		miscFlags |= D3D10_RESOURCE_MISC_GENERATE_MIPS;

	D3D10_TEXTURE2D_DESC tex2DDesc;
	tex2DDesc.Height			= m_Height;
	tex2DDesc.Width				= m_Width;
	tex2DDesc.ArraySize			= m_ArraySize;
	tex2DDesc.MipLevels			= m_MipLevels;
	tex2DDesc.Format			= m_DXGIFormat;
	tex2DDesc.SampleDesc.Count	= 1;
	tex2DDesc.SampleDesc.Quality = 0;
	tex2DDesc.Usage				= D3D10_USAGE_DEFAULT;
	tex2DDesc.BindFlags			= bindFlags; 
	tex2DDesc.CPUAccessFlags	= 0;
	tex2DDesc.MiscFlags			= miscFlags;
	
	if (WriteMethod == SR_INIT_DATA) 
		hr = m_pDevice->CreateTexture2D(&tex2DDesc, pSubResData, &m_pTex);
	else 
		hr = m_pDevice->CreateTexture2D(&tex2DDesc, NULL, &m_pTex);
	
	if(FAILED(hr))
	{
		pTest->WriteToLog( _T( "SrcObjects10::SetupResources() - CreateTexture2D() as shared resource failed. hr = %s" ), D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Exit;
	}

	if ( ( miscFlags & D3D11_RESOURCE_MISC_SHARED_NTHANDLE ) && 
		 ( miscFlags & D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX) )
	{
		if (FAILED (hr = m_pTex->QueryInterface(__uuidof(IDXGIResource1), (void **)&pDXGIResource1)))
		{
			pTest->WriteToLog(_T( "SrcObjects11::SetupResources() - QueryInterface(IDXGIResource1) failed. hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}
	
		if (FAILED (hr = pDXGIResource1->CreateSharedHandle(
			NULL, 
			DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE, 
			HANDLE_NAME_VALID, 
			&m_SharedHandle)))
		{
			pTest->WriteToLog( _T( "SrcObjects11::SetupResources() - CreateSharedHandle() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}
	}
	else
	{
	
		if (FAILED (hr = m_pTex->QueryInterface(__uuidof(IDXGIResource), (void**) (&pDXGIResource))))
		{
			pTest->WriteToLog( _T( "SrcObjects10::SetupResources() - IDXGIResource::QueryInterface() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}

		if (FAILED (hr = pDXGIResource->GetSharedHandle(&m_SharedHandle)))
		{
			pTest->WriteToLog( _T( "SrcObjects10::SetupResources() - IDXGIResource::GetSharedHandle() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}
	}

Exit:
	SAFE_RELEASE(pDXGIResource);
	SAFE_RELEASE(pDXGIResource1);

	if ( pSubResData )
	{
		delete[] pSubResData;
	}

	return tr;
}

TEST_RESULT SrcObjects10::UpdateSharedResource(SharedResourcesTest *pTest, BYTE* pDataByte1, SR_WRITE_METHOD WriteMethod)
{
	//fill texture
	TEST_RESULT tr = RESULT_PASS;
	UINT bytesPerElement = GetBitsPerElement(m_DXGIFormat) / 8;
	switch( WriteMethod )
	{
	case SR_COPY_RESOURCE:
		{
			m_pDevice->CopyResource(m_pTex, m_pSrcTex); 
			break;
		}
	case SR_UPDATE_RESOURCE:
		{			
			for ( UINT i = 0; i < m_ArraySize; i++ )
			{
				UINT subResourceWidth = m_Width;
				UINT subResourceHeight = m_Height;
				for ( UINT j = 0; j< m_MipLevels; j++ )
				{
					UINT srcRowPitch =  bytesPerElement * subResourceWidth;
					UINT srcDepthPitch = srcRowPitch * subResourceHeight;
					UINT subResource = i * m_MipLevels + j;
					m_pDevice->UpdateSubresource(m_pTex, subResource, NULL, pDataByte1, srcRowPitch, srcDepthPitch);

					subResourceWidth = subResourceWidth >> 1;
					subResourceHeight = subResourceHeight >> 1;
				}
			}
			break;
		}
	case SR_DRAWTO_RESOURCE:
		{
			for ( UINT i = 0; i < m_ArraySize; i++ )
			{
				for ( UINT j = 0; j < m_MipLevels; j++ )
				{
					tr = DrawToResource(pTest, i, j, pDataByte1, m_pTex);
					if (tr != RESULT_PASS)
					{
						pTest->WriteToLog( _T( "SrcObjects10::UpdateSharedResource() - DrawToResource() failed, \
											   ArraySlice = %d, MipSlice = %d." ), i, j);
						tr = RESULT_FAIL;
						goto Done;
					}
				}
			}
			break;
		}
	case SR_INIT_DATA:
		{
			// the shared resource has been created with SR_INIT_DATA; no need to fill the texture
			break;
		}
	default:
		{
			pTest->WriteToLog( _T( "SrcObjects10::UpdateSharedResource() - WriteMethod doesn't exist." ));
			tr = RESULT_FAIL;
			goto Done;
		}

	}
	
	m_pDevice->Flush();

	// Loop until the GPU signals that it finished processing the commands in last command buffer
	HRESULT hr = S_OK;
	if(FAILED( hr = pTest->WaitForEvent(NULL, m_pDevice) ))
	{
		pTest->WriteToLog( _T( "SrcObjects10::UpdateSharedResource() - WaitForEvent() failed. hr = %s" ), D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Done;
	}

Done: // releasing interfaces that've been used by DrawToResource()
	SAFE_RELEASE( m_pVertexShader );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pSRV );
	SAFE_RELEASE( m_pSRVTex );
	SAFE_RELEASE( m_pRTV );

	return tr;
}


DestObjects10_1::DestObjects10_1(SharedResourcesTest *pTest, D3D_FEATURE_LEVEL fl) :
	DestObjects10(pTest, pTest->Create10_1Device(fl))
{ }

SrcObjects10_1::SrcObjects10_1(SharedResourcesTest *pTest, D3D_FEATURE_LEVEL fl) :
	SrcObjects10(pTest, pTest->Create10_1Device(fl))
{ }


static const UINT c_ValidD3DCreateFlags = D3D11_CREATE_DEVICE_SINGLETHREADED | 
									 D3D11_CREATE_DEVICE_DEBUG | 
									 D3D11_CREATE_DEVICE_SWITCH_TO_REF | 
									 D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;

DestObjects11::DestObjects11(SharedResourcesTest *pTest, D3D_FEATURE_LEVEL fl ) :
	m_pDevice(NULL),
	m_pDevice11_1(NULL),
	m_pPartnerDevice(NULL),
	m_pImmediateContext(NULL),
	m_pEffectiveContext(NULL),
	m_pDevTest(NULL),
	m_DXGIFormat(pTest->GetFormat()),
	m_pTexShared(NULL),
	m_Width(pTest->GetWidth()),
	m_Height(pTest->GetHeight()),
	m_ArraySize(pTest->GetArraySize()),
	m_MipLevels(pTest->GetMipLevels()),
	m_pVertexShader(NULL),
	m_pPixelShader(NULL),
	m_pInputLayout(NULL),
	m_pVertexBuffer(NULL), 
	m_pSRRTTex(NULL),
	m_pSRV(NULL),
	m_pRTV(NULL),
	m_hCompositionSurface(NULL)
{
	HRESULT hr = E_UNEXPECTED;
	IDXGIDevice* pDXGI = NULL;
	IDXGIAdapter* pAdapter = NULL;
	ID3D11Device* pExisting = pTest->GetDevice();

	hr = pExisting->QueryInterface(__uuidof(IDXGIDevice), (void**)(&pDXGI));
	if (FAILED(hr) || pDXGI == NULL)
	{
		pTest->WriteToLog( _T( "DestObjects11::DestObjects11 - QueryInterface failed. hr = %s" ), D3DHResultToString(hr).c_str());
		goto func_end;
	}
	hr = pDXGI->GetAdapter(&pAdapter);
	const UINT createFlags = c_ValidD3DCreateFlags & pExisting->GetCreationFlags();

	hr = D3D11CreateDevice_11or12(
		g_App.m_D3DOptions.Force11On12 ? UseD3D11On12_D3D11CreateDeviceFlag : UseD3D11,
		pAdapter, 
		pAdapter ? D3D_DRIVER_TYPE_UNKNOWN: g_App.m_D3DOptions.DriverType, 
		NULL, createFlags, 
		&fl, 
		1, 
		D3D11_SDK_VERSION, 
		&m_pDevice, 
		NULL, 
		&m_pImmediateContext,
		NULL);
	
	if ( FAILED(hr) )
	{
		pTest->WriteToLog( _T( "SharedResourcesTest::Create11Device - g_App.CreateDevice failed. hr = %s" ), D3DHResultToString(hr).c_str());
		m_pDevice = NULL;
		m_pImmediateContext = NULL;
		goto func_end;
	}
	
	if (pTest->GetFramework()->IsDeferredContextUsed())
	{
		HRESULT hr;
		if (FAILED(hr = m_pDevice->CreateDeferredContext( 0, &m_pEffectiveContext )))
		{
			pTest->WriteToLog( _T( "CreateDeferredContext failed with error %s." ), D3DHResultToString( hr ).c_str() );
			goto func_end;
		}
	}
	else
	{
		m_pEffectiveContext = m_pImmediateContext;
		m_pEffectiveContext->AddRef();
	}

	func_end:
	SAFE_RELEASE(pDXGI);
	SAFE_RELEASE(pAdapter);
	
}

TEST_RESULT DestObjects11::SetupTestCase(SharedResourcesTest* pTest)
{
	if (NULL == m_pDevice || NULL == m_pEffectiveContext)
		return RESULT_BLOCKED;
	
	HRESULT hr = m_pDevice->QueryInterface( __uuidof(m_pPartnerDevice), (void**)&m_pPartnerDevice );
	if ( FAILED(hr) || !m_pPartnerDevice )
	{
		pTest->LogError( __FILEW__, __LINE__, L"QI failed for ID3D11PartnerDevice.", hr, ERRORCODE_TYPE_HRESULT); 
		return RESULT_FAIL;
	}

	UINT formatSupport;
	hr = m_pDevice->CheckFormatSupport(m_DXGIFormat, &formatSupport);
	if (FAILED(hr))
	{
		pTest->WriteToLog( _T( "DestObjects11::SetupTestCase() - CheckFormatSupport(%s) failed. hr = %s" ), ToString(m_DXGIFormat).c_str(), D3DHResultToString(hr).c_str());
		return RESULT_SKIP;
	}

	UINT neededBits = D3D11_FORMAT_SUPPORT_TEXTURE2D;
	if (neededBits != (neededBits & formatSupport))
	{
		pTest->WriteToLog( _T( "DestObjects11::SetupTestCase() - DXGI Format %s not supported on this device. hr = %s" ), ToString(m_DXGIFormat).c_str(), D3DHResultToString(hr).c_str());
		return RESULT_SKIP;
	}

	// need a bind flag for default resource on FL9
	if( ( pTest->GetSourceDeviceFeatureLevel() < D3D_FEATURE_LEVEL_10_0 || pTest->GetDestinationDeviceFeatureLevel() < D3D_FEATURE_LEVEL_10_0 ) &&
		( pTest->GetWriteMethod() ==  SR_COPY_RESOURCE || pTest->GetChildAccessMethod() ==  SR_COPY_RESOURCE ) )
	{	
		if( !( formatSupport & ( D3D11_FORMAT_SUPPORT_RENDER_TARGET | D3D11_FORMAT_SUPPORT_SHADER_LOAD | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE ) ) )
		{
			pTest->WriteToLog( "DestObjects11::SetupTestCase(): Format %s does not support RenderTarget or ShaderResource on this device. Skip test case.", ToString(m_DXGIFormat).c_str() );
			return RESULT_SKIP;
		}
	}

	if( pTest->GetBindRenderTarget() )
	{
		if( !( formatSupport & D3D11_FORMAT_SUPPORT_RENDER_TARGET ) )
		{
			pTest->WriteToLog( "DestObjects11::SetupTestCase(): Format %s does not support RenderTarget on this device. Skip test case.", ToString(m_DXGIFormat).c_str() );
			return RESULT_SKIP;
		}
	}

	// Emulating R8 with L8 on WARP on FL9
	if( g_App.getIsWARP() &&  g_App.IsEmulatingR8WithL8() && m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_0 )
	{
		if( FAILED( hr = m_pDevice->QueryInterface( __uuidof(ID3D11DeviceTest), (void **)&m_pDevTest ) ) )
		{
			pTest->WriteToLog( "DestObjects11::SetupTestCase(): QI for ID3D11DeviceTest failed. hr = %s.", D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
		if( FAILED( hr = m_pDevTest->EmulateR8WithL8( TRUE ) ) )
		{
			pTest->WriteToLog( "DestObjects11::SetupTestCase(): Call to EmulateR8WithL8() failed. hr = %s.", D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
		pTest->WriteToLog( _T("Emulating R8 With L8...") );

	}

	return RESULT_PASS;
}

TEST_RESULT DestObjects11::DrawToResource( SharedResourcesTest *pTest, UINT arrayIndex, UINT mipIndex, void *pData, ID3D11Texture2D *pSharedTex, bool bSampleFromShared)
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;
	ID3D10Blob *pVertexShaderBlob = NULL;
	ID3D10Blob *pPixelShaderBlob = NULL;
	ID3D11SamplerState *pSamplerState = NULL;

	bool bLevel9 = false;
	D3D_FEATURE_LEVEL fl = m_pDevice->GetFeatureLevel();
	if( fl < D3D_FEATURE_LEVEL_10_0 )
		bLevel9 = true;

	DXGI_FORMAT viewFormat = m_DXGIFormat;
	if (FF_TYPELESS & GetFlags(m_DXGIFormat))
		viewFormat = pTest->GetViewFormat(m_DXGIFormat);

	tr = pTest->CompileVsPsShaders(&pVertexShaderBlob, &pPixelShaderBlob, GetFlags(viewFormat), bLevel9);
	if (tr != RESULT_PASS)
	{
		pTest->WriteToLog( _T( "DestObjects11::DrawToResource() - CompileVsPsShaders Failed." ) );
		tr = RESULT_FAIL;
		goto Done;
	}

	// Create shaders
	hr = m_pDevice->CreateVertexShader( 
		pVertexShaderBlob->GetBufferPointer(), 
		pVertexShaderBlob->GetBufferSize(), 
		NULL,
		&m_pVertexShader
	);
	if( FAILED( hr ) )
	{
		pTest->WriteToLog( _T( "DestObjects11::DrawToResource() - Failed to create vertex shader, hr=%s" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto Done;
	}
	hr = m_pDevice->CreatePixelShader( 
		pPixelShaderBlob->GetBufferPointer(), 
		pPixelShaderBlob->GetBufferSize(), 
		NULL,
		&m_pPixelShader
	);
	if( FAILED( hr ) )
	{
		pTest->WriteToLog( _T( "DestObjects11::DrawToResource() - Failed to create pixel shader, hr=%s" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto Done;
	}
	
	// create inputlayout
	D3D11_INPUT_ELEMENT_DESC pIEDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = m_pDevice->CreateInputLayout( 
		pIEDesc, 
		2, 
		pVertexShaderBlob->GetBufferPointer(), 
		pVertexShaderBlob->GetBufferSize(), 
		&m_pInputLayout );

	if( FAILED( hr ) )
	{
		pTest->WriteToLog( _T( "DestObjects11::DrawToResource() - CreateInputLayout() failed" ) );
		tr = RESULT_FAIL;
		goto Done;
	}

	// Create Vertex buffer; first 3 values are for POSITION and the last 2 values are for TEXCOORD
	float vertices[6][5] = 
	{
		{ -1.0f,	-1.0f,		0.5f,	0.f,	1.0f },
		{ -1.0f,	1.0f,		0.5f,	0.f,	0.f },
		{ 1.0f,		-1.0f,		0.5f,	1.0f,	1.0f },
		{ 1.0f,		-1.0f,		0.5f,	1.0f,	1.0f },
		{ -1.0f,	1.0f,		0.5f,	0.f,	0.f },
		{ 1.0f,		1.0f,		0.5f,	1.0f,	0.f }
	};

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( float ) * 6 * 5;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = vertices;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	hr = m_pDevice->CreateBuffer( &bd, &initData, &m_pVertexBuffer );
	if( FAILED( hr ) )
	{
		pTest->WriteToLog( _T( "DestObjects11::DrawToResource() - CreateBuffer failed, hr=%s" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto Done;
	}

	// create shader resource
	D3D11_TEXTURE2D_DESC tex2DDesc;
	tex2DDesc.ArraySize			= 1;
	tex2DDesc.BindFlags			= bSampleFromShared ? D3D11_BIND_RENDER_TARGET: D3D11_BIND_SHADER_RESOURCE;
	tex2DDesc.CPUAccessFlags	= 0;
	tex2DDesc.Format			= m_DXGIFormat;
	tex2DDesc.Height			= m_Height >> mipIndex;
	tex2DDesc.Width				= m_Width >> mipIndex;
	tex2DDesc.MipLevels			= 1;
	tex2DDesc.MiscFlags			= 0;
	tex2DDesc.SampleDesc.Count	= 1;
	tex2DDesc.SampleDesc.Quality= 0;
	tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA initDataTex2D;
	initDataTex2D.pSysMem = pData;
	initDataTex2D.SysMemPitch = tex2DDesc.Width * GetBitsPerElement( tex2DDesc.Format ) / 8;
	initDataTex2D.SysMemSlicePitch = 0;
	D3D11_SUBRESOURCE_DATA *pInitArg = bSampleFromShared? NULL: &initDataTex2D;
	SAFE_RELEASE( m_pSRRTTex );
	if( FAILED( hr = m_pDevice->CreateTexture2D( &tex2DDesc, pInitArg, &m_pSRRTTex) ) )
	{
		pTest->WriteToLog( _T( "DestObjects11::DrawToResource() - CreateTexture2D() failed for pSRVTex, hr = %s" ), D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Done;
	}

	if( !bSampleFromShared )
	{
		// the following is for debugging
		if (FAILED(hr = pTest->VerifyTex2D_D( m_pDevice, m_pImmediateContext, m_pSRRTTex, (BYTE*)pData)))
		{
			pTest->WriteToLog( _T( "DestObjects11::DrawToResource() - Verifying data failed after CreateTexture2D() for pSRVTex. \
									hr = %s. array = %d, mip = %d. " ), D3DHResultToString(hr).c_str(), arrayIndex, mipIndex);
					tr = RESULT_FAIL;
					goto Done;		
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = viewFormat;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	ID3D11Resource *pShaderResource = bSampleFromShared? pSharedTex: m_pSRRTTex;
	if( FAILED( hr = m_pDevice->CreateShaderResourceView( pShaderResource, &srvDesc, &m_pSRV) ) )
	{
		pTest->WriteToLog( _T( "DestObjects11::DrawToResource() - CreateShaderResourceView() failed for m_pSRV, hr = %s" ), D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Done;
	}

	// Create Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = viewFormat;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.MipSlice = mipIndex;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.FirstArraySlice = arrayIndex;
	ID3D11Resource *pRenderTarget = bSampleFromShared? m_pSRRTTex: pSharedTex;
	if( FAILED( hr = m_pDevice->CreateRenderTargetView( pRenderTarget, &rtvDesc, &m_pRTV) ) )
	{
		pTest->WriteToLog( _T( "DestObjects11::DrawToResource() - CreateRenderTargetView() failed for m_pRTV, hr = %s" ), D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Done;
	}
	D3D11_VIEWPORT vp;
	vp.Width = (float)(m_Width >> mipIndex);
	vp.Height = (float)(m_Height >> mipIndex);
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	// create and set sampler
	D3D11_SAMPLER_DESC SamplerDesc;
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.MipLODBias = 0;
	SamplerDesc.MaxAnisotropy = 1;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = FLT_MAX;
	if( FAILED( hr = m_pDevice->CreateSamplerState(&SamplerDesc, &pSamplerState ) ))
	{
		pTest->WriteToLog( "DestObjects11::DrawToResource() - CreateSamplerState() failed. hr = %s", D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto Done;
	}

	// Set rendering pipeline
	UINT stride = sizeof(vertices[0]);
	UINT offset = 0;
	m_pEffectiveContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pEffectiveContext->IASetInputLayout(m_pInputLayout);
	m_pEffectiveContext->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_pEffectiveContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pEffectiveContext->PSSetShader(m_pPixelShader, NULL, 0);
	m_pEffectiveContext->PSSetShaderResources(0, 1, &m_pSRV);
	m_pEffectiveContext->PSSetSamplers(0, 1, &pSamplerState);
	m_pEffectiveContext->RSSetViewports(1, &vp);
	m_pEffectiveContext->OMSetRenderTargets(1, &m_pRTV, NULL);

	m_pEffectiveContext->Draw(6, 0);
	

Done:

	SAFE_RELEASE( pVertexShaderBlob );
	SAFE_RELEASE( pPixelShaderBlob );

	SAFE_RELEASE( m_pVertexShader );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pSRV );
	SAFE_RELEASE( m_pRTV );
	SAFE_RELEASE( pSamplerState );

	return tr;
}

SrcObjects11::SrcObjects11(SharedResourcesTest *pTest, D3D_FEATURE_LEVEL fl) :
	DestObjects11(pTest, fl),
	m_pSrcTex(NULL),
	m_pTex(NULL),
	m_pTexArray(NULL),
	m_TexArrayCount(0)
{ }

TEST_RESULT SrcObjects11::SetupResources(SharedResourcesTest *pTest, BYTE* pDataByte1, SR_WRITE_METHOD WriteMethod)
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;
	IDXGIResource	*pDXGIResource = NULL;
	IDXGIResource1  *pDXGIResource1 = NULL;

	D3D11_SUBRESOURCE_DATA	 *pSubResData = NULL;
	pSubResData = new D3D11_SUBRESOURCE_DATA[m_ArraySize * m_MipLevels];
	BYTE **ppInitData = new BYTE* [m_ArraySize * m_MipLevels];

	UINT blockSize = pTest->GetFormatBlockSize();

	float bytesPerElement = (float)GetBitsPerElement(m_DXGIFormat)/8;
	if ( GetFlags(m_DXGIFormat) & FF_VIDEO )
	{
		bytesPerElement = (float)GetBitsPerElementVideo(m_DXGIFormat) / 8;
	}

	for ( UINT i = 0; i < m_ArraySize; i++ )
	{
		UINT subResWidth = m_Width;
		UINT subResHeight = m_Height;
		for ( UINT j = 0; j < m_MipLevels; j++ )
		{
			UINT dataSize = (UINT)(subResWidth * subResHeight * bytesPerElement);
			ppInitData[i * m_MipLevels + j] = new BYTE[dataSize];
			memcpy(ppInitData[i * m_MipLevels + j], pDataByte1, dataSize);
			pSubResData[i * m_MipLevels + j].pSysMem = ppInitData[i * m_MipLevels + j];	
			if( blockSize > 1 )
				pSubResData[i * m_MipLevels + j].SysMemPitch = ( subResWidth / 4) * blockSize;
			else 
				pSubResData[i * m_MipLevels + j].SysMemPitch = (UINT)(subResWidth * bytesPerElement);
			subResWidth = subResWidth >> 1;
			subResHeight = subResHeight >> 1;
		}
	}

	UINT formatSupport;
	hr = m_pDevice->CheckFormatSupport(m_DXGIFormat, &formatSupport);
	if (FAILED(hr))
	{
		pTest->WriteToLog( _T( "SrcObjects11::SetupResources() - CheckFormatSupport(%s) failed. hr = %s" ), ToString(m_DXGIFormat).c_str(), D3DHResultToString(hr).c_str());
		tr = RESULT_SKIP;
		goto Exit;
	}

	//create source texture
	D3D11_TEXTURE2D_DESC tex2DDesc;
	tex2DDesc.ArraySize			= m_ArraySize;
	tex2DDesc.BindFlags			= 0;
	
	tex2DDesc.Format			= m_DXGIFormat;
	tex2DDesc.Height			= m_Height;
	tex2DDesc.Width				= m_Width;
	tex2DDesc.MipLevels			= m_MipLevels;
	tex2DDesc.MiscFlags			= 0;
	tex2DDesc.SampleDesc.Count	= 1;
	tex2DDesc.SampleDesc.Quality= 0;
	
	if ( GetFlags(m_DXGIFormat) & FF_VIDEO )
	{
		// create staging resource for video format resource
		tex2DDesc.Usage				= D3D11_USAGE_STAGING;
		tex2DDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
		if( FAILED( hr = m_pDevice->CreateTexture2D( &tex2DDesc, NULL, &m_pSrcTex) ) )
		{
			pTest->WriteToLog( _T( "SrcObjects11::SetupResources() - CreateTexture2D() failed for source resource, hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}

		tr = pTest->WriteStagingVideoResource(m_Width, m_Height, m_DXGIFormat, m_pDevice, m_pImmediateContext, m_pSrcTex, pDataByte1);
		if (tr != RESULT_PASS)
		{
			pTest->WriteToLog( _T( "SrcObjects11::SetupResources() - WriteStagingVideoResource() failed." ));
			goto Exit;
		}
		//VerifyTex2D for debugging purpose
		hr = pTest->VerifyTex2D(m_pDevice, m_pImmediateContext, 0, 0, m_pSrcTex, pDataByte1);
		if (hr != S_OK)
		{
			pTest->WriteToLog( _T( "SrcObjects11::SetupResources() - VerifyTex2D() failed after WriteStagingVideoResource, hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}


	}
	else
	{
		tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
		tex2DDesc.CPUAccessFlags	= 0;

		// need a bind flag for default resource on FL9
		if( pTest->GetSourceDeviceFeatureLevel() < D3D_FEATURE_LEVEL_10_0 || pTest->GetDestinationDeviceFeatureLevel() < D3D_FEATURE_LEVEL_10_0 )
		{
			if( formatSupport & ( D3D11_FORMAT_SUPPORT_RENDER_TARGET | D3D11_FORMAT_SUPPORT_SHADER_LOAD | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE ) )
				tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			else if( formatSupport & D3D11_FORMAT_SUPPORT_RENDER_TARGET )
				tex2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
			else
			{
				pTest->WriteToLog( "SrcObjects11::SetupResources(): Format %s does not support RenderTarget or ShaderResource on this device. Skip test case.", ToString(m_DXGIFormat).c_str() );
				tr = RESULT_SKIP;
				goto Exit;
			}
		}

		if( FAILED( hr = m_pDevice->CreateTexture2D( &tex2DDesc, pSubResData, &m_pSrcTex) ) )
		{
			pTest->WriteToLog( _T( "SrcObjects11::SetupResources() - CreateTexture2D() failed for source resource, hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}

		// the following is for debugging
		UINT k = 0;
		for ( UINT i = 0; i < m_ArraySize; i++ )
		{
			for ( UINT j = 0; j < m_MipLevels; j++ )
			{
				if (FAILED(hr = pTest->VerifyTex2D( m_pDevice, m_pImmediateContext, i, j, m_pSrcTex, pDataByte1)))
				{
					pTest->WriteToLog( _T( "SrcObjects11::SetupResources - Verifying data in m_pSrcTex failed after resource creation with init data.\
											hr = %s. array = %d, mip = %d" ), D3DHResultToString(hr).c_str(), i, j);
					if ( k++ < 10 )
						continue;
					tr = RESULT_FAIL;
					goto Exit;
				}
			}
		}
	}

	//create the shared resource
	UINT bindFlags = 0;
	if ( pTest->GetBindRenderTarget() )  
		bindFlags |= D3D11_BIND_RENDER_TARGET;
	if ( pTest->GetBindShaderResource() )  
		bindFlags |= D3D11_BIND_SHADER_RESOURCE;
	if ( pTest->GetBindCapture() ) 
		bindFlags |= D3D11_PRIVATE_BIND_CAPTURE;

	// need a bind flag for default resource on FL9
	if( ( pTest->GetSourceDeviceFeatureLevel() < D3D_FEATURE_LEVEL_10_0 || pTest->GetDestinationDeviceFeatureLevel() < D3D_FEATURE_LEVEL_10_0 ) && !( pTest->GetBindRenderTarget()) && !( pTest->GetBindShaderResource() ) )
	{
		if( formatSupport & ( D3D11_FORMAT_SUPPORT_RENDER_TARGET | D3D11_FORMAT_SUPPORT_SHADER_LOAD | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE ) )
			bindFlags = D3D11_BIND_SHADER_RESOURCE;
		else if( formatSupport & D3D11_FORMAT_SUPPORT_RENDER_TARGET )
			bindFlags = D3D11_BIND_RENDER_TARGET;
	}

	UINT miscFlags = D3D11_RESOURCE_MISC_SHARED;
	if ( pTest->GetSyncMethod() == SR_KEYED_MUTEX )
		miscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
	if ( pTest->GetMiscGenMips() ) 
		miscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	if ( pTest->GetMiscSharedNTHandle() ) 
		miscFlags |= D3D11_RESOURCE_MISC_SHARED_NTHANDLE;
	if ( pTest->GetMiscGDICompatible() )
		miscFlags |= D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

	tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
	tex2DDesc.BindFlags			= bindFlags;
	tex2DDesc.CPUAccessFlags	= 0;
	tex2DDesc.MiscFlags			= miscFlags;

	if (WriteMethod == SR_INIT_DATA) 
	{
		hr = m_pDevice->CreateTexture2D(&tex2DDesc, pSubResData, &m_pTex);
	}
	else 
		hr = m_pDevice->CreateTexture2D(&tex2DDesc, NULL, &m_pTex);
	
	if(FAILED(hr))
	{
		pTest->WriteToLog( _T( "SrcObjects11::SetupResources() - CreateTexture2D() as shared resource failed. hr = %s" ), D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Exit;
	}

	
	if ( ( miscFlags & D3D11_RESOURCE_MISC_SHARED_NTHANDLE ) && 
		 ( miscFlags & D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX) )
	{
		if (FAILED (hr = m_pTex->QueryInterface(__uuidof(IDXGIResource1), (void **)&pDXGIResource1)))
		{
			pTest->WriteToLog(_T( "SrcObjects11::SetupResources() - QueryInterface(IDXGIResource1) failed. hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}
	
		if (FAILED (hr = pDXGIResource1->CreateSharedHandle(
			NULL, 
			DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE,
			HANDLE_NAME_VALID, 
			&m_SharedHandle)))
		{
			pTest->WriteToLog( _T( "SrcObjects11::SetupResources() - CreateSharedHandle() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}
	}
	else
	{
		if (FAILED (hr = m_pTex->QueryInterface(__uuidof(IDXGIResource), (void**) (&pDXGIResource))))
		{
			pTest->WriteToLog( _T( "SrcObjects11::SetupResources() - IDXGIResource::QueryInterface() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}

		if (FAILED (hr = pDXGIResource->GetSharedHandle(&m_SharedHandle)))
		{
			pTest->WriteToLog( _T( "SrcObjects11::SetupResources() - IDXGIResource::GetSharedHandle() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}
	}

Exit:
	SAFE_RELEASE(pDXGIResource);
	SAFE_RELEASE(pDXGIResource1);
	if( ppInitData )
	{
		for( UINT i = 0; i < m_ArraySize; i++ )
		{
			for( UINT j = 0; j < m_MipLevels; j++ )
			{
				if( ppInitData[i * m_MipLevels + j] )
				{
					delete[] ppInitData[i * m_MipLevels + j];
				}
			}
		}
		delete[] ppInitData;
		ppInitData = NULL;
	}
	if( pSubResData )
	{
		delete[] pSubResData;
	}
	return tr;
}

TEST_RESULT SrcObjects11::UpdateSharedResource(SharedResourcesTest *pTest, BYTE* pDataByte1, SR_WRITE_METHOD WriteMethod)
{
	//fill texture
	TEST_RESULT tr = RESULT_PASS;
	UINT bytesPerElement = GetBitsPerElement(m_DXGIFormat) / 8;

	// create the data for subresources
	BYTE **ppSubResData = new BYTE* [m_ArraySize * m_MipLevels];
	for ( UINT i = 0; i < m_ArraySize; i++ )
	{
		UINT subResWidth = m_Width;
		UINT subResHeight = m_Height;
		for ( UINT j = 0; j < m_MipLevels; j++ )
		{
			ppSubResData[i * m_MipLevels + j] = new BYTE[subResWidth * subResHeight * bytesPerElement];
			memcpy(ppSubResData[i * m_MipLevels + j], pDataByte1, subResWidth * subResHeight * bytesPerElement);
			subResWidth = subResWidth >> 1;
			subResHeight = subResHeight >> 1;
		}
	}

	switch( WriteMethod )
	{
	case SR_COPY_RESOURCE:
		{
			if ( pTest->GetCompBuffer() )
			{
				for ( UINT r = 0; r < pTest->GetNumCompBuffers(); r++ )
				{
					m_pEffectiveContext->CopyResource(m_pTexArray[r], m_pSrcTex);
				}
			}
			else
			{
				m_pEffectiveContext->CopyResource(m_pTex, m_pSrcTex);
			}
			break;
		}
	case SR_UPDATE_RESOURCE:
		{
			for ( UINT i = 0; i < m_ArraySize; i++ )
			{
				UINT subResourceWidth = m_Width;
				UINT subResourceHeight = m_Height;
				for ( UINT j = 0; j< m_MipLevels; j++ )
				{
					UINT srcRowPitch =  bytesPerElement * subResourceWidth;
					UINT srcDepthPitch = srcRowPitch * subResourceHeight;
					UINT subResource = i * m_MipLevels + j;

					if ( pTest->GetCompBuffer() )
					{
						for ( UINT r = 0; r < pTest->GetNumCompBuffers(); r++ )
						{
							m_pEffectiveContext->UpdateSubresource(m_pTexArray[r], subResource, NULL, ppSubResData[i * m_MipLevels + j], srcRowPitch, srcDepthPitch);
						}
					}
					else
					{
						m_pEffectiveContext->UpdateSubresource(m_pTex, subResource, NULL, ppSubResData[i * m_MipLevels + j], srcRowPitch, srcDepthPitch);
					}

					subResourceWidth = subResourceWidth >> 1;
					subResourceHeight = subResourceHeight >> 1;
				}
			}
			break;
		}
	case SR_DRAWTO_RESOURCE:
		{
			for ( UINT i = 0; i < m_ArraySize; i++ )
			{
				for ( UINT j = 0; j < m_MipLevels; j++ )
				{
					if ( pTest->GetCompBuffer() )
					{
						for ( UINT r = 0; r < pTest->GetNumCompBuffers(); r++ )
						{
							tr = DrawToResource(pTest, i, j, ppSubResData[i * m_MipLevels + j], m_pTexArray[r]);
							if (tr != RESULT_PASS)
							{
								pTest->WriteToLog( _T( "SrcObjects11::UpdateSharedResource() - DrawToResource() failed, \
													   ArraySlice = %d, MipSlice = %d, Resource Number = %u." ), i, j, r);
								tr = RESULT_FAIL;
								goto Done;
							}
						}
					}
					else
					{
						tr = DrawToResource(pTest, i, j, ppSubResData[i * m_MipLevels + j], m_pTex);
						if (tr != RESULT_PASS)
						{
							pTest->WriteToLog( _T( "SrcObjects11::UpdateSharedResource() - DrawToResource() failed, \
												   ArraySlice = %d, MipSlice = %d." ), i, j);
							tr = RESULT_FAIL;
							goto Done;
						}
					}
				}
			}
			break;
		}
	case SR_INIT_DATA:
		{
			// the shared resource has been created with SR_INIT_DATA; no need to fill the texture
			break;
		}
	default:
		{
			pTest->WriteToLog( _T( "SrcObjects11::UpdateSharedResource() - WriteMethod doesn't exist." ));
			tr = RESULT_FAIL;
			goto Done;
		}
	}

	HRESULT hr = S_OK;
	if (m_pEffectiveContext != m_pImmediateContext)
	{
		ID3D11CommandList* pCommandList = NULL;
		if (FAILED (hr = m_pEffectiveContext->FinishCommandList(TRUE, &pCommandList)))
		{
			pTest->WriteToLog( _T( "SrcObjects11::UpdateSharedResource() - ID3D11CommandList::FinishCommandList() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_BLOCKED;
			goto Done;
		}
		m_pImmediateContext->ExecuteCommandList(pCommandList, TRUE);
		pCommandList->Release();
	}
	m_pImmediateContext->Flush();

	// making sure writing is finished
	if( pTest->GetCompBuffer() )
	{
		// present the compositionBuffers
		hr = m_pPartnerDevice->PresentCompositionBuffers( NULL, reinterpret_cast< IUnknown** >( m_pTexArray ), pTest->GetNumCompBuffers() );
		if( hr != S_OK )
		{
			pTest->LogError( __FILEW__, __LINE__, L"PresentCompositionBuffers() failed.", hr, ERRORCODE_TYPE_HRESULT); 
			tr = RESULT_FAIL;
			goto Done;
		}// Sync the compositionBuffers
		if(FAILED( hr = pTest->SyncCompBuffer( m_pDevice ) ))
		{
			pTest->WriteToLog( _T( "SrcObjects11::UpdateSharedResource() - SyncCompBuffer() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Done;
		}	
	}
	else
	{
		// Loop until the GPU signals that it finished processing the commands in last command buffer
		if(FAILED( hr = pTest->WaitForEvent( m_pDevice ) ))
		{
			pTest->WriteToLog( _T( "SrcObjects11::UpdateSharedResource() - WaitForEvent() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Done;
		}
	}


	// the following is for debugging
	UINT k = 0;
	for ( UINT i = 0; i < m_ArraySize; i++ )
	{
		for ( UINT j = 0; j < m_MipLevels; j++ )
		{
			if ( pTest->GetCompBuffer() )
			{
				for ( UINT r = 0; r < pTest->GetNumCompBuffers(); r++ )
				{
					if (FAILED(hr = pTest->VerifyTex2D( m_pDevice, m_pImmediateContext, i, j, m_pTexArray[r], pDataByte1)))
					{
						pTest->WriteToLog( _T( "SrcObjects11::UpdateSharedResource - Verifying data in resource failed after resource update.\
												hr = %s. array = %d, mip = %d, resource number = %u" ), D3DHResultToString(hr).c_str(), i, j, r);
						if ( k++ < 10 )
								continue;
						tr = RESULT_FAIL;
						goto Done;
					}
				}
			}
			else
			{
				if (FAILED(hr = pTest->VerifyTex2D( m_pDevice, m_pImmediateContext, i, j, m_pTex, pDataByte1)))
				{
					pTest->WriteToLog( _T( "SrcObjects11::UpdateSharedResource - Verifying data in resource failed after resource update.\
											hr = %s. array = %d, mip = %d" ), D3DHResultToString(hr).c_str(), i, j);
					if ( k++ < 10 )
							continue;
					tr = RESULT_FAIL;
					goto Done;
				}
			}
		}
	}
	/*
	ID3D11Texture2D *p_Tex2DTemp;
	if (FAILED(hr = m_pDevice->OpenSharedResource(m_SharedHandle, __uuidof(ID3D11Texture2D), (void**) &p_Tex2DTemp)))
	{
		pTest->WriteToLog( _T( "SrcObjects11::UpdateSharedResource() - Failed to open shared resource. hr = %s" ), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}
	if (FAILED( hr = pTest->VerifyTex2D( m_pDevice, m_pImmediateContext, 0, 0, p_Tex2DTemp, pDataByte1)))
	{
		pTest->WriteToLog( _T( "SrcObjects11::UpdateSharedResource - Verifying data in resource failed on opened resource.\
								hr = %s" ), D3DHResultToString(hr).c_str());
		SAFE_RELEASE(p_Tex2DTemp);
		return RESULT_FAIL;
	}
	SAFE_RELEASE(p_Tex2DTemp); */


Done: // releasing interfaces that've been used by DrawToResource()
	if( ppSubResData )
	{
		for( UINT i = 0; i < m_ArraySize; i++ )
		{
			for( UINT j = 0; j < m_MipLevels; j++ )
			{
				if( ppSubResData[i * m_MipLevels + j] )
				{
					delete[] ppSubResData[i * m_MipLevels + j];
				}
			}
		}
		delete[] ppSubResData;
		ppSubResData = NULL;
	}

	SAFE_RELEASE( m_pVertexShader );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pSRV );
	SAFE_RELEASE( m_pSRRTTex );
	SAFE_RELEASE( m_pRTV );

	return tr;
}


DestObjectsVideo::DestObjectsVideo(SharedResourcesTest *pTest, D3D_FEATURE_LEVEL fl ) :
	m_pDevice(NULL),
	m_pDevice11_1(NULL),
	m_pImmediateContext(NULL),
	m_pVideoDevice(NULL),
	m_pVideoContext(NULL),
	m_pVideoProcessorEnum(NULL),
	m_pVideoProcessor(NULL),
	m_DXGIFormat(pTest->GetFormat()),
	m_pTexShared(NULL),
	m_Width(pTest->GetWidth()),
	m_Height(pTest->GetHeight())
{
	HRESULT hr = E_UNEXPECTED;
	IDXGIDevice* pDXGI = NULL;
	IDXGIAdapter* pAdapter = NULL;
	ID3D11Device* pExisting = pTest->GetDevice();

	hr = pExisting->QueryInterface(__uuidof(IDXGIDevice), (void**)(&pDXGI));
	if (FAILED(hr) || pDXGI == NULL)
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::DestObjectsVideo - QueryInterface failed for pDXGI. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		goto func_end;
	}
	hr = pDXGI->GetAdapter(&pAdapter);
	const UINT createFlags = c_ValidD3DCreateFlags & pExisting->GetCreationFlags();

	hr = D3D11CreateDevice_11or12(
		g_App.m_D3DOptions.Force11On12 ? UseD3D11On12_D3D11CreateDeviceFlag : UseD3D11,
		pAdapter, 
		pAdapter ? D3D_DRIVER_TYPE_UNKNOWN: g_App.m_D3DOptions.DriverType, 
		NULL, createFlags, 
		&fl, 
		1, 
		D3D11_SDK_VERSION, 
		&m_pDevice, 
		NULL, 
		&m_pImmediateContext,
		NULL);
	
	if ( FAILED(hr) )
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::DestObjectsVideo - g_App.CreateDevice failed. hr = %s" ), D3DHResultToString(hr).c_str());
		m_pDevice = NULL;
		m_pImmediateContext = NULL;
		goto func_end;
	}

	// get VideoDevice
	hr = m_pDevice->QueryInterface( __uuidof( ID3D11VideoDevice ), (void**)(&m_pVideoDevice));
	if (FAILED(hr) || m_pVideoDevice == NULL)
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::DestObjectsVideo - QueryInterface failed for m_pVideoDevice. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		m_pVideoDevice = NULL;
		goto func_end;
	}

	// get VideoContext
	hr = m_pImmediateContext->QueryInterface( __uuidof( ID3D11VideoContext ), (void**)(&m_pVideoContext));
	if (FAILED(hr) || m_pVideoContext == NULL)
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::DestObjectsVideo - QueryInterface failed for m_pVideoContext. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		m_pVideoContext = NULL;
		goto func_end;
	}

	// Create VideoProcessorEnumerator
	
	D3D11_VIDEO_PROCESSOR_CONTENT_DESC ContentDesc;
	ZeroMemory( &ContentDesc, sizeof( ContentDesc ) );
	ContentDesc.InputFrameFormat = D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE;
	ContentDesc.InputWidth = m_Width;
	ContentDesc.InputHeight = m_Height;
	ContentDesc.OutputWidth = m_Width;
	ContentDesc.OutputHeight = m_Height;
	ContentDesc.Usage = D3D11_VIDEO_USAGE_PLAYBACK_NORMAL;
	hr = m_pVideoDevice->CreateVideoProcessorEnumerator( &ContentDesc, &m_pVideoProcessorEnum );
	if (FAILED(hr) )
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::DestObjectsVideo -CreateVideoProcessorEnumerator failed. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		m_pVideoProcessorEnum = NULL;
		goto func_end;
	}

	// Create VideoProcessor	
	hr = m_pVideoDevice->CreateVideoProcessor( m_pVideoProcessorEnum, 0, &m_pVideoProcessor );
	if ( FAILED(hr) || m_pVideoProcessor == NULL )
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::DestObjectsVideo - CreateVideoProcessor failed. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		m_pVideoProcessor = NULL;
		goto func_end;
	}
	
	func_end:
	SAFE_RELEASE(pDXGI);
	SAFE_RELEASE(pAdapter);
	
}


TEST_RESULT DestObjectsVideo::SetupTestCase(SharedResourcesTest* pTest)
{
	if ( NULL == m_pVideoDevice || 
		NULL == m_pVideoContext || 
		NULL == m_pVideoProcessorEnum || 
		NULL == m_pVideoProcessor )
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::SetupTestCase() - Invalid DestObjectsVideo." ) );
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
			"Invalid DestObjectsVideo.");
		return RESULT_BLOCKED;
	}

	
	UINT formatSupport;
	HRESULT hr = m_pDevice->CheckFormatSupport(m_DXGIFormat, &formatSupport);
	if (FAILED(hr))
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::SetupTestCase() - CheckFormatSupport(%s) failed. hr = %s" ), ToString(m_DXGIFormat).c_str(), D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
			"DestObjectsVideo::SetupTestCase() - CheckFormatSupport failed.");
		return RESULT_SKIP;
	}
	UINT vpFormatSupport;
	hr = m_pVideoProcessorEnum->CheckVideoProcessorFormat( m_DXGIFormat, &vpFormatSupport );
	if (FAILED(hr))
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::SetupTestCase() - CheckVideoProcessorFormat(%s) failed. hr = %s" ), 
			ToString(m_DXGIFormat).c_str(), D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
			"DestObjectsVideo::SetupTestCase() - CheckVideoProcessorFormat failed.");
		return RESULT_SKIP;
	}

	UINT neededBits = 
		D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_RENDER_TARGET |
		D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT | 
		D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT;

	UINT neededVPBits = 
		D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_INPUT | 
		D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_OUTPUT;
	

	if ( (neededBits != (neededBits & formatSupport)) ||
		 (neededVPBits != (neededVPBits & vpFormatSupport))
		 )
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::SetupTestCase() - DXGI Format %s not supported for video processor input or output\
							   on this device. hr = %s" ), ToString(m_DXGIFormat).c_str(), D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
			"DestObjectsVideo::SetupTestCase() - DXGI Format not supported for video processor input or output\
							   on this device.");
		return RESULT_SKIP;
	}
	return RESULT_PASS;
}

TEST_RESULT DestObjectsVideo::UpdateDataWithVideoProcessor(SharedResourcesTest *pTest, ID3D11Texture2D* pTex2D, BYTE* pDataByte)
{

	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;

	ID3D11Texture2D* pInput = NULL;
	ID3D11Texture2D* pInputStaging = NULL;
	ID3D11VideoProcessorInputView* pInputView = NULL;
	ID3D11VideoProcessorOutputView* pOutputView = NULL;

	D3D11_VIDEO_PROCESSOR_STREAM StreamData = {0};
	D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC OutputViewDesc;
	D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC InputViewDesc;
	D3D11_TEXTURE2D_DESC Desc = {0};

	// Create the outputview from the shared resource
	// Resource of the output view has to be created with D3D11_BIND_RENDER_TARGET
	
	OutputViewDesc.ViewDimension = D3D11_VPOV_DIMENSION_TEXTURE2D;
	OutputViewDesc.Texture2D.MipSlice = 0;

	
   
	hr = m_pVideoDevice->CreateVideoProcessorOutputView( 
		pTex2D,
		m_pVideoProcessorEnum,
		&OutputViewDesc,
		&pOutputView );
	if ( hr != S_OK )
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::UpdateDataWithVideoProcessor() -  CreateVideoProcessorOutputView() Failed. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
			"DestObjectsVideo::UpdateDataWithVideoProcessor() -  CreateVideoProcessorOutputView() Failed.");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto Done;
	}


	// Create 1 input surface
	Desc.Width = m_Width;
	Desc.Height = m_Height;
	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.Format = m_DXGIFormat;
	Desc.SampleDesc.Count = 1;
	Desc.BindFlags = 0;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.CPUAccessFlags	= 0;

	hr = m_pDevice->CreateTexture2D(&Desc,NULL, &pInput);
	if ( hr != S_OK )
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::UpdateDataWithVideoProcessor() -  CreateTexture2D() Failed for pInput. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
			"DestObjectsVideo::UpdateDataWithVideoProcessor() - CreateTexture2D() Failed for pInput.");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto Done;
	}
	
	// create the staging tex for the input surface
	Desc.Usage = D3D11_USAGE_STAGING;
	Desc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
	
	hr = m_pDevice->CreateTexture2D(&Desc,NULL, &pInputStaging);
	if ( hr != S_OK )
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::UpdateDataWithVideoProcessor() -  CreateTexture2D() Failed for pInputStaging. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
			"DestObjectsVideo::UpdateDataWithVideoProcessor() - CreateTexture2D() Failed for pInputStaging.");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto Done;
	}

	// write to the staging tex and copy staging tex to the input surface
	tr = pTest->WriteStagingVideoResource(m_Width, m_Height, m_DXGIFormat, m_pDevice, m_pImmediateContext, pInputStaging, pDataByte);
	if (tr != RESULT_PASS)
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::UpdateDataWithVideoProcessor() - WriteStagingVideoResource() failed." ));
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
			"DestObjectsVideo::UpdateDataWithVideoProcessor() - WriteStagingVideoResource() failed.");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto Done;
	}

	m_pImmediateContext->CopyResource(pInput, pInputStaging);

	//VerifyTex2D for debugging purpose
	if (FAILED( hr = pTest->VerifyTex2D( m_pDevice, m_pImmediateContext, 0, 0, pInput, pDataByte)))
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::UpdateDataWithVideoProcessor - Verifying data for pInput failed before blt.\
								hr = %s" ), D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
			"DestObjectsVideo::UpdateDataWithVideoProcessor - Verifying data for pInput failed before blt.");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto Done;;
	}

	InputViewDesc.FourCC = 0;
	InputViewDesc.ViewDimension = D3D11_VPIV_DIMENSION_TEXTURE2D;
	InputViewDesc.Texture2D.MipSlice = 0;
	InputViewDesc.Texture2D.ArraySlice = 0;
	hr = m_pVideoDevice->CreateVideoProcessorInputView( 
		pInput,
		m_pVideoProcessorEnum,
		&InputViewDesc,
		&pInputView );
	if ( hr != S_OK )
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::UpdateDataWithVideoProcessor() -  CreateVideoProcessorInputView() Failed. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
			"DestObjectsVideo::UpdateDataWithVideoProcessor() -  CreateVideoProcessorInputView() Failed.");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto Done;
	}
	
	RECT Rect;
	Rect.top = 0;
	Rect.left = 0;
	Rect.right = m_Width;
	Rect.bottom = m_Height;
	m_pVideoContext->VideoProcessorSetOutputTargetRect( m_pVideoProcessor, TRUE, &Rect );
	m_pVideoContext->VideoProcessorSetStreamFrameFormat( m_pVideoProcessor, 0, D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE );
	m_pVideoContext->VideoProcessorSetStreamSourceRect( m_pVideoProcessor, 0, TRUE, &Rect);
	m_pVideoContext->VideoProcessorSetStreamDestRect( m_pVideoProcessor, 0, TRUE, &Rect);

   

	StreamData.Enable = TRUE;
	StreamData.OutputIndex = 0;
	StreamData.InputFrameOrField = 0;
	StreamData.PastFrames = 0;
	StreamData.FutureFrames = 0;
	StreamData.ppPastSurfaces = NULL;
	StreamData.ppFutureSurfaces = NULL;
	StreamData.pInputSurface = pInputView;

   hr = m_pVideoContext->VideoProcessorBlt( m_pVideoProcessor, pOutputView, 0, 1, &StreamData );
   if ( hr != S_OK )
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::UpdateDataWithVideoProcessor() -  VideoProcessorBlt() Failed. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
			"DestObjectsVideo::UpdateDataWithVideoProcessor() -  VideoProcessorBlt() Failed.");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto Done;
	}

   m_pImmediateContext->Flush();
   if(FAILED( hr = pTest->WaitForEvent( m_pDevice ) ))
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::UpdateDataWithVideoProcessor() - WaitForEvent() failed. hr = %s" ), D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
			"DestObjectsVideo::UpdateDataWithVideoProcessor() - WaitForEvent() failed.");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto Done;
	}

	//VerifyTex2D for debugging purpose
	if (FAILED( hr = pTest->VerifyTex2D( m_pDevice, m_pImmediateContext, 0, 0, pTex2D, pDataByte)))
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::UpdateDataWithVideoProcessor - Verifying data for pTex2D failed after blt.\
								hr = %s" ), D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
			"DestObjectsVideo::UpdateDataWithVideoProcessor - Verifying data for pTex2D failed after blt.");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto Done;
	}
   
Done:
   SAFE_RELEASE( pInput );
   SAFE_RELEASE( pInputStaging );
   SAFE_RELEASE( pInputView );
   SAFE_RELEASE( pOutputView );
   return tr;
}

SrcObjectsVideo::SrcObjectsVideo(SharedResourcesTest *pTest, D3D_FEATURE_LEVEL fl) :
	DestObjectsVideo(pTest, fl),
	m_pTex(NULL)
{ }

TEST_RESULT SrcObjectsVideo::SetupResources(SharedResourcesTest *pTest, BYTE* pDataByte1, SR_WRITE_METHOD WriteMethod)
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;
	IDXGIResource	*pDXGIResource = NULL;
	IDXGIResource1  *pDXGIResource1 = NULL;


	//create the shared resource
	//output resource of the video process has to have RENDER_TARGET flag
	//currently only ArraySize=1 and Miplevels=1 are tested for video formats
	UINT bindFlags = 0;
	if ( pTest->GetBindRenderTarget() )  
		bindFlags |= D3D11_BIND_RENDER_TARGET;
	if ( pTest->GetBindShaderResource() )  
		bindFlags |= D3D11_BIND_SHADER_RESOURCE;
	if ( pTest->GetBindCapture() )  
		bindFlags |= D3D11_PRIVATE_BIND_CAPTURE;

	UINT miscFlags = D3D11_RESOURCE_MISC_SHARED;
	if ( pTest->GetSyncMethod() == SR_KEYED_MUTEX )
		miscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
	if ( pTest->GetMiscGenMips() ) 
		miscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	if ( pTest->GetMiscSharedNTHandle() ) 
		miscFlags |= D3D11_RESOURCE_MISC_SHARED_NTHANDLE;


	D3D11_TEXTURE2D_DESC tex2DDesc;
	tex2DDesc.Height			= m_Height;
	tex2DDesc.Width				= m_Width;
	tex2DDesc.ArraySize			= 1;
	tex2DDesc.MipLevels			= 1;
	tex2DDesc.Format			= m_DXGIFormat;
	tex2DDesc.SampleDesc.Count	= 1;
	tex2DDesc.SampleDesc.Quality = 0;
	tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
	tex2DDesc.BindFlags			= bindFlags;
	tex2DDesc.CPUAccessFlags	= 0;
	tex2DDesc.MiscFlags			= miscFlags;

	hr = m_pDevice->CreateTexture2D(&tex2DDesc, NULL, &m_pTex);
	
	if(FAILED(hr))
	{
		pTest->WriteToLog( _T( "SrcObjectsVideo::SetupResources() - CreateTexture2D() as shared resource failed. hr = %s" ), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}
	
	if ( ( miscFlags & D3D11_RESOURCE_MISC_SHARED_NTHANDLE ) && 
		 ( miscFlags & D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX) )
	{
		if (FAILED (hr = m_pTex->QueryInterface(__uuidof(IDXGIResource1), (void **)&pDXGIResource1)))
		{
			pTest->WriteToLog(_T( "SrcObjects11::SetupResources() - QueryInterface(IDXGIResource1) failed. hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}
	
		if (FAILED (hr = pDXGIResource1->CreateSharedHandle(
			NULL, 
			DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE,
			HANDLE_NAME_VALID, 
			&m_SharedHandle)))
		{
			pTest->WriteToLog( _T( "SrcObjects11::SetupResources() - CreateSharedHandle() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}
	}
	else
	{
		if (FAILED (hr = m_pTex->QueryInterface(__uuidof(IDXGIResource), (void**) (&pDXGIResource))))
		{
			pTest->WriteToLog( _T( "SrcObjects11::SetupResources() - IDXGIResource::QueryInterface() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}

		if (FAILED (hr = pDXGIResource->GetSharedHandle(&m_SharedHandle)))
		{
			pTest->WriteToLog( _T( "SrcObjects11::SetupResources() - IDXGIResource::GetSharedHandle() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}
	}

Exit:
	SAFE_RELEASE(pDXGIResource);
	SAFE_RELEASE(pDXGIResource1);
	return tr;
}

TEST_RESULT SrcObjectsVideo::UpdateSharedResource(SharedResourcesTest *pTest, BYTE* pDataByte1, SR_WRITE_METHOD WriteMethod)
{
	TEST_RESULT tr = RESULT_PASS;
	
	tr = UpdateDataWithVideoProcessor(pTest, m_pTex, pDataByte1);
	
	if ( tr != RESULT_PASS)
	{
		pTest->WriteToLog( _T( "SrcObjectsVideo::UpdateSharedResource() - UpdateDataWithVideoProcessor() failed." ));
	}

	/*
	// the following is for debugging
	HRESULT hr = S_OK;
	ID3D11Texture2D *p_Tex2DTemp;
	if (FAILED(hr = m_pDevice->OpenSharedResource(m_SharedHandle, __uuidof(ID3D11Texture2D), (void**) &p_Tex2DTemp)))
	{
		pTest->WriteToLog( _T( "SrcObjectsVideo::UpdateSharedResource() - Failed to open shared resource. hr = %s" ), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}
	if (FAILED( hr = pTest->VerifyTex2D( m_pDevice, m_pImmediateContext, 0, 0, p_Tex2DTemp, pDataByte1)))
	{
		pTest->WriteToLog( _T( "SrcObjectsVideo::UpdateSharedResource - Verifying data in resource failed on opened resource.\
								hr = %s" ), D3DHResultToString(hr).c_str());
		SAFE_RELEASE(p_Tex2DTemp);
		return RESULT_FAIL;
	}
	SAFE_RELEASE(p_Tex2DTemp); */



	return tr;
}


SrcObjectsGDI::SrcObjectsGDI( SharedResourcesTest *pTest, D3D_FEATURE_LEVEL fl):
	SrcObjects11(pTest, fl)
{
}


TEST_RESULT SrcObjectsGDI::UpdateSharedResource(SharedResourcesTest *pTest, BYTE* pDataByte1, SR_WRITE_METHOD WriteMethod)
{
	TEST_RESULT tr = RESULT_PASS;
	
	tr = UpdateDataWithGDI(pTest, m_pTex, pDataByte1);
	
	if ( tr != RESULT_PASS)
	{
		pTest->WriteToLog( _T( "SrcObjectsGDI::UpdateSharedResource() - UpdateDataWithGDI() failed." ));
	}

	return tr;

}

TEST_RESULT SrcObjectsGDI::UpdateDataWithGDI(SharedResourcesTest *pTest, ID3D11Texture2D* pTex2D, BYTE* pDataByte)
{
	IDXGISurface1 *pDXGISurface1 = NULL;
	HDC hdc = NULL;
	TEST_RESULT tr = RESULT_PASS;

	// QI for pDXGISurface1 and GetDC
	HRESULT hr = pTex2D->QueryInterface(__uuidof( pDXGISurface1 ), reinterpret_cast<void**>( &pDXGISurface1 ) );
	if ( FAILED (hr) || pDXGISurface1 == NULL )
	{
		pTest->WriteToLog(_T("SrcObjectsGDI::UpdateDataWithGDI(): QI for IDXGISurface1 failed. hr = %s"), D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto cleanup;
	}
	hr = pDXGISurface1->GetDC(FALSE, &hdc);
	if ( FAILED (hr) || hdc == NULL )
	{
		pTest->WriteToLog(_T("SrcObjectsGDI::UpdateDataWithGDI(): GetDC failed. hr = %s"), D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto cleanup;
	}
	 
	// Write to the shared resource with GDI
	COLORREF Color;
	BYTE* pColorData = pDataByte;
	for (UINT row = 0; row < m_Height; row++)
	{
		for (UINT col = 0; col < m_Width; col++)
		{
			// reverse RGB order since the format for GDI is BGR
			Color = RGB(pColorData[2], pColorData[1], pColorData[0]);
			::SetPixel(hdc, col, row, Color);
			pColorData += 4;
		}
	}
					   
	hr = pDXGISurface1->ReleaseDC(NULL);
	// wait for a sec to let GDI finish its job
	Sleep(1000); 
	if ( FAILED (hr) )
	{
		pTest->WriteToLog(_T("SrcObjectsGDI::UpdateDataWithGDI(): ReleaseDC failed. hr = %s"), D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto cleanup;
	}
  
cleanup:
	SAFE_RELEASE(pDXGISurface1);
	return tr;
}


DestObjectsGDI::DestObjectsGDI( SharedResourcesTest *pTest, D3D_FEATURE_LEVEL fl ):
	DestObjects11(pTest, fl)
{
}

SrcObjectsCompBuffer::SrcObjectsCompBuffer(SharedResourcesTest* pTest, D3D_FEATURE_LEVEL fl):
	SrcObjects11(pTest, fl)
{
}

TEST_RESULT SrcObjectsCompBuffer::SetupResources(SharedResourcesTest* pTest, BYTE* pDataByte1, SR_WRITE_METHOD WriteMethod)
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;
	ID3D11DeviceTest* pTestDevice = NULL;

	assert(pTest->GetNumCompBuffers() > 0);
	m_TexArrayCount = pTest->GetNumCompBuffers();

	m_pTexArray = new ID3D11Texture2D*[m_TexArrayCount];
	m_SharedHandleArray = new HANDLE[m_TexArrayCount];

	for ( UINT i = 0;  i < m_TexArrayCount; i++ )
	{
		m_pTexArray[i] = NULL;
		m_SharedHandleArray[i] = NULL;
	}
	
	D3D11_SUBRESOURCE_DATA	 *pSubResData = NULL;
	pSubResData = new D3D11_SUBRESOURCE_DATA[m_ArraySize * m_MipLevels];
	BYTE **ppInitData = new BYTE* [m_ArraySize * m_MipLevels]; 


	float bytesPerElement = (float)GetBitsPerElement(m_DXGIFormat)/8;
	if ( GetFlags(m_DXGIFormat) & FF_VIDEO )
	{
		bytesPerElement = (float)GetBitsPerElementVideo(m_DXGIFormat) / 8;
	}
	UINT blockSize = pTest->GetFormatBlockSize();
	for ( UINT i = 0; i < m_ArraySize; i++ )
	{
		UINT subResWidth = m_Width;
		UINT subResHeight = m_Height;
		for ( UINT j = 0; j < m_MipLevels; j++ )
		{
			UINT dataSize = (UINT)(subResWidth * subResHeight * bytesPerElement);
			ppInitData[i * m_MipLevels + j] = new BYTE[dataSize];
			memcpy(ppInitData[i * m_MipLevels + j], pDataByte1, dataSize);
			pSubResData[i * m_MipLevels + j].pSysMem = ppInitData[i * m_MipLevels + j];
			if( blockSize > 1 )
				pSubResData[i * m_MipLevels + j].SysMemPitch = ( subResWidth / 4) * blockSize;
			else 
				pSubResData[i * m_MipLevels + j].SysMemPitch = (UINT)(subResWidth * bytesPerElement);
			subResWidth = subResWidth >> 1;
			subResHeight = subResHeight >> 1;
		}
	}


	//create source texture	
	D3D11_TEXTURE2D_DESC tex2DDesc;
	tex2DDesc.ArraySize			= m_ArraySize;
	tex2DDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	tex2DDesc.Format			= m_DXGIFormat;
	tex2DDesc.Height			= m_Height;
	tex2DDesc.Width				= m_Width;
	tex2DDesc.MipLevels			= m_MipLevels;
	tex2DDesc.MiscFlags			= 0;
	tex2DDesc.SampleDesc.Count	= 1;
	tex2DDesc.SampleDesc.Quality= 0;
	tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
	tex2DDesc.CPUAccessFlags	= 0;

	if( FAILED( hr = m_pDevice->CreateTexture2D( &tex2DDesc, pSubResData, &m_pSrcTex) ) )
	{
		pTest->WriteToLog( "SrcObjectsCompBuffer::SetupResources() - CreateTexture2D() failed for source resource, hr = %s", D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Exit;
	}

	// create CompositionBuffer
	UINT CBFlag = 0;
	DXGI_USAGE DXGIUsage = 0;
	if ( pTest->GetBindRenderTarget() )  
		DXGIUsage |= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	if ( pTest->GetBindShaderResource() )  
		DXGIUsage |= DXGI_USAGE_SHADER_INPUT;

	for ( UINT i = 0;  i < m_TexArrayCount; i++ )
	{
		hr = m_pPartnerDevice->CreateCompositionBuffer(m_Width, m_Height, m_DXGIFormat, m_ArraySize > 1, DXGIUsage, CBFlag, __uuidof(ID3D11Texture2D), &m_hCompositionSurface, reinterpret_cast<void**>(&m_pTexArray[i]));
		if (FAILED(hr))
		{
			pTest->WriteToLog( "SrcObjectsCompBuffer::SetupResources() - CreateCompositionBuffer() failed for index = %u, hr = %s", i, D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}
	}


	if ( FAILED(hr = m_pDevice->QueryInterface(__uuidof( ID3D11DeviceTest ), (void**)&pTestDevice) ) )
	{
		pTest->LogError( __FILEW__, __LINE__, L"QI failed for ID3D11DeviceTest.", hr, ERRORCODE_TYPE_HRESULT); 
		tr = RESULT_FAIL;
		goto Exit;
	}
	
	for ( UINT i = 0;  i < m_TexArrayCount; i++ )
	{
		std::wstringstream out;
		out << i;
		std::wstring handleName = HANDLE_NAME_VALID + out.str();

		if ( FAILED( hr = pTestDevice->CreateSharedHandle( m_pTexArray[i], NULL, DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE, 
			handleName.c_str(), 
			&m_SharedHandleArray[i] ) ) )
		{
			pTest->LogError( __FILEW__, __LINE__, L"CreateSharedHandle() failed.", hr, ERRORCODE_TYPE_HRESULT); 
			tr = RESULT_FAIL;
			goto Exit;
		}
	}

Exit:
	SAFE_RELEASE(pTestDevice);
	if( ppInitData )
	{
		for( UINT i = 0; i < m_ArraySize; i++ )
		{
			for( UINT j = 0; j < m_MipLevels; j++ )
			{
				if( ppInitData[i * m_MipLevels + j] )
				{
					delete[] ppInitData[i * m_MipLevels + j];
				}
			}
		}
		delete[] ppInitData;
		ppInitData = NULL;
	}
	if( pSubResData )
	{
		delete[] pSubResData;
	}

	if ( tr != RESULT_PASS )
	{
		for ( UINT i = 0;  i < m_TexArrayCount; i++ )
		{
			if (m_SharedHandleArray[i])
			{
				CloseHandle( m_SharedHandleArray[i] );
			}
		}
	}

	return tr;
}



//try accessing the target resource from the destination device
TEST_RESULT SharedResourcesTest::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	TEST_RESULT trUpdateSharedResource = RESULT_PASS;
	HRESULT hr;
	IDXGIKeyedMutex *pDXGIKeyedMutex = NULL;

	if ( m_SyncMethod == SR_KEYED_MUTEX && m_testType == SR_OTHER_PROCESS)
	{
		hr = m_pSrcObjs->QIKeyedMutex( &pDXGIKeyedMutex );
		if ( hr != S_OK )
		{
			WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase() - QIKeyedMutex() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto testDone;
		}

		// Acquire a lock to the resource.
		hr = pDXGIKeyedMutex->AcquireSync(0, 20000);
		if ( hr != S_OK)
		{
			WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase() - First AcquireSync() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto testDone;
		}
	}
	
	tr = m_pSrcObjs->UpdateSharedResource(this, m_pDataByte1, m_WriteMethod);
	if (!(tr == RESULT_PASS))
	{
		WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase() - UpdateSharedResource() failed."));
		trUpdateSharedResource = RESULT_FAIL;
		// The child process has a dependency on the parent managing synchronization. 
		// If we skip to cleanup here, we may leave the child in a bad state (for example,
		// it may be stuck waiting on m_startChildResAccessEvent, and the child will exit
		// after timeout). Therefore, continue the rest of the test case to ensure we maintain
		// correct synchronization, even if UpdateSharedResource fails.
	}

	bool waitChildFail = false;

	//OPEN SHARED RESOURCE
	if (m_testType == SR_OTHER_PROCESS)
	{
		
		if ( m_SyncMethod == SR_KEYED_MUTEX)
		{
			// Release the lock to the child process.
			hr = pDXGIKeyedMutex->ReleaseSync(1);
			if ( hr != S_OK)
			{
				WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase() - ReleaseSync() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
				tr = RESULT_FAIL;
				goto testDone;
			}

			// Acquiring the lock to the resource from the child process
			hr = pDXGIKeyedMutex->AcquireSync(0, 20000);
			if ( hr != S_OK)
			{
				WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase() - AcquireSync() from Child process failed. (HR = %s)"), D3DHResultToString(hr).c_str());
				waitChildFail = true;
			}
		}
		else
		{
			//tell child process to begin test
			if (! SetEvent(m_startChildResAccessEvent) ) 
			{ 
				WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase() - Failed to signal child process to start."));
				tr = RESULT_SKIP;
				goto testDone;
			}

			//wait for child test case to finish
			waitChildFail = true;
		
			DWORD dwResult = WaitForSingleObject(m_endChildResAccessEvent, 8000);
			if (dwResult == WAIT_OBJECT_0)
			{
				waitChildFail = false;
			}
			else // Failed
			{		
				switch (dwResult)
				{
					case WAIT_TIMEOUT:
						WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase() - WaitForSingleObject() returned WAIT_TIMEOUT."));
						break;

					case WAIT_ABANDONED:
						WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase() - WaitForSingleObject() returned WAIT_ABANDONED."));
						break;

					case WAIT_FAILED:
						WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase() - WaitForSingleObject() returned WAIT_FAILED with error code %d"), GetLastError());
						break;
				}
				WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase() - Failed to recieve end test signal from child."));
			}
			
		}

		// child test case may fail before it gets to access the shared resource
		if ( waitChildFail )
		{
			if ( WaitForSingleObject(m_endChildCaseWithError, 10) != WAIT_OBJECT_0 ) 
			{ 
				WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase - Failed to recieve endChildCaseWithError signal from child. (%d)"),
					GetLastError());
				if ( WaitForSingleObject(m_ProcInf.hProcess, 10) != WAIT_OBJECT_0 )
				{
					WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase - Failed to recieve end process signal from child. (%d)"),
					GetLastError());
				}
				else
				{
					WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase - Child Process exits unexpectedly."));
				}
				// abort test group if the main process can't even get m_endChildCaseWithError signal
				WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase() - Failure in child process. %s hr = %s" ), m_sharedMemory->ResultString, D3DHResultToString(m_sharedMemory->Result).c_str());
				tr = RESULT_ABORT;
				goto testDone;
			}
		}

		//get result
		if (FAILED(m_sharedMemory->Result))
		{
			WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase() - Failure in child process. %s hr = %s" ), m_sharedMemory->ResultString, D3DHResultToString(m_sharedMemory->Result).c_str());
			tr = RESULT_FAIL;
			goto testDone;
		}
		if (m_sharedMemory->Result == S_FALSE)
		{
			WriteToLog( _T( "SharedResourcesTest::ExecuteTestCase() - Skip in child process. %s hr = %s" ), m_sharedMemory->ResultString, D3DHResultToString(m_sharedMemory->Result).c_str());
			tr = RESULT_SKIP;
			goto testDone;
		}
	}
	else //open resource in this process
	{
		DestObjects* pDestObjs = NULL;
		if ( m_testType == SR_SAME_DEVICE )
		{
			pDestObjs = m_pSrcObjs;
		}
		else 
		{
			pDestObjs = m_pDestObjs;
		}

		if ( GetCompBuffer() )
		{
			for ( UINT r = 0; r < GetNumCompBuffers(); r++ )
			{
				m_CurrCompBuffer = r;
				tr = pDestObjs->ExecuteResourceOpen(this, m_pSrcObjs->m_SharedHandleArray[r]);
				if( tr == RESULT_PASS )
					tr = pDestObjs->ExecuteResourceAccess(this, m_pDataByte1, m_pDataByte2);
			}
		}
		else
		{
			tr = pDestObjs->ExecuteResourceOpen(this, m_pSrcObjs->m_SharedHandle);
			if( tr == RESULT_PASS )
				tr = pDestObjs->ExecuteResourceAccess(this, m_pDataByte1, m_pDataByte2);
		}
	}

	if ( tr != RESULT_PASS )
	{
		goto testDone;
	}

	//verify data on source device
	tr = m_pSrcObjs->ExecuteVerifyTestCase(this, m_pDataByte1, m_pDataByte2);
	

testDone:
	if (trUpdateSharedResource != RESULT_PASS)
	{
		// Ensure we expose an earlier UpdateSharedResource failure
		tr = trUpdateSharedResource;
	}
	if ( pDXGIKeyedMutex )
		SAFE_RELEASE( pDXGIKeyedMutex );
	if ( m_bMiscSharedNTHandle )
	{
		CloseHandle(m_pSrcObjs->m_SharedHandle);

		if ( GetCompBuffer() )
		{
			for ( UINT r = 0; r < GetNumCompBuffers(); r++ )
			{
				if (m_pSrcObjs->m_SharedHandleArray[r])
				{
					CloseHandle(m_pSrcObjs->m_SharedHandleArray[r]);
				}
			}
		}
	}

	return tr;
}

TEST_RESULT SrcObjects9::ExecuteVerifyTestCase(SharedResourcesTest *pTest, BYTE* pDataByte1, BYTE* pDataByte2)
{
	HRESULT hr;
	
	if (FAILED(hr = pTest->VerifyIDirect3DTexture9(m_pTex, pDataByte2, m_D3DFormat)))
	{
		pTest->WriteToLog( _T( "SrcObjects9::ExecuteVerifyTestCase() - Invalid data read in source resource." ));
		return RESULT_FAIL;
	}
	return RESULT_PASS;
}

TEST_RESULT SrcObjects10::ExecuteVerifyTestCase(SharedResourcesTest *pTest, BYTE* pDataByte1, BYTE* pDataByte2)
{
	HRESULT hr;
	// For tex2DArrays, the child process only updated the first and the last array slices with DataByte2.
	// All the rest subresources should still contain DataByte1

	// The first array slice
	for ( UINT j = 0; j < m_MipLevels; ++j )
	{
		if (FAILED(hr = pTest->VerifyTex2D(m_pDevice, 0, j, m_pTex, pDataByte2)))
		{
			pTest->WriteToLog( _T( "SrcObjects10::ExecuteVerifyTestCase() - Invalid data read in source resource, \
								   arraySlice = 0, MipSlice = %d."), j);
			return RESULT_FAIL;
		}
	}
	if ( m_ArraySize > 1 )
	{
		if ( m_ArraySize > 2 )
		{
			for ( UINT i = 1; i < m_ArraySize -1; i ++ )
			{
				for ( UINT j = 0; j < m_MipLevels; ++j )
				{
					if (FAILED(hr = pTest->VerifyTex2D(m_pDevice, i, j, m_pTex, pDataByte1)))
					{
						pTest->WriteToLog( _T( "SrcObjects10::ExecuteVerifyTestCase() - Invalid data read in source resource, \
										  arraySlice = %d, MipSlice = %d."), i, j);
						return RESULT_FAIL;
					}
				}
			}
		}

		// the last array slice
		for ( UINT j = 0; j < m_MipLevels; ++j )
		{
			if (FAILED(hr = pTest->VerifyTex2D(m_pDevice, m_ArraySize - 1, j, m_pTex, pDataByte2)))
			{
				pTest->WriteToLog( _T( "SrcObjects10::ExecuteVerifyTestCase() - Invalid data read in source resource, \
								   arraySlice = %d, MipSlice = %d."), m_ArraySize - 1, j);
				return RESULT_FAIL;
			}
		}
		
	}
	return RESULT_PASS;
}

TEST_RESULT SrcObjects11::ExecuteVerifyTestCase(SharedResourcesTest *pTest,  BYTE* pDataByte1, BYTE* pDataByte2)
{
	HRESULT hr;
	// For tex2DArrays, the child process only updated the first and the last array slices with DataByte2.
	// All the rest subresources should still contain DataByte1

	// The first array slice
	for ( UINT j = 0; j < m_MipLevels; ++j )
	{
		if ( pTest->GetCompBuffer() )
		{
			for ( UINT r = 0; r < pTest->GetNumCompBuffers(); r++ )
			{
				if (FAILED(hr = pTest->VerifyTex2D(m_pDevice, m_pImmediateContext, 0, j, m_pTexArray[r], pDataByte2)))
				{
					pTest->WriteToLog( _T( "SrcObjects11::ExecuteVerifyTestCase() - Invalid data read in source resource, \
											arraySlice = 0, MipSlice = %d, resource number = %u."), j, r);
					return RESULT_FAIL;
				}
			}
		}
		else
		{
			if (FAILED(hr = pTest->VerifyTex2D(m_pDevice, m_pImmediateContext, 0, j, m_pTex, pDataByte2)))
			{
				pTest->WriteToLog( _T( "SrcObjects11::ExecuteVerifyTestCase() - Invalid data read in source resource, \
										arraySlice = 0, MipSlice = %d."), j);
				return RESULT_FAIL;
			}
		}
	}
	if ( m_ArraySize > 1 )
	{
		if ( m_ArraySize > 2 )
		{
			for ( UINT i = 1; i < m_ArraySize -1; i ++ )
			{
				for ( UINT j = 0; j < m_MipLevels; ++j )
				{
					if ( pTest->GetCompBuffer() )
					{
						for ( UINT r = 0; r < pTest->GetNumCompBuffers(); r++ )
						{
							if (FAILED(hr = pTest->VerifyTex2D(m_pDevice, m_pImmediateContext, i, j, m_pTexArray[r], pDataByte1)))
							{
								pTest->WriteToLog( _T( "SrcObjects11::ExecuteVerifyTestCase() - Invalid data read in source resource, \
													arraySlice = %d, MipSlice = %d, resource number = %u."), i, j, r);
								return RESULT_FAIL;
							}
						}
					}
					else
					{
						if (FAILED(hr = pTest->VerifyTex2D(m_pDevice, m_pImmediateContext, i, j, m_pTex, pDataByte1)))
						{
							pTest->WriteToLog( _T( "SrcObjects11::ExecuteVerifyTestCase() - Invalid data read in source resource, \
												arraySlice = %d, MipSlice = %d."), i, j);
							return RESULT_FAIL;
						}
					}
				}
			}
		}

		// the last array slice
		for ( UINT j = 0; j < m_MipLevels; ++j )
		{
			if ( pTest->GetCompBuffer() )
			{
				for ( UINT r = 0; r < pTest->GetNumCompBuffers(); r++ )
				{
					if (FAILED(hr = pTest->VerifyTex2D(m_pDevice, m_pImmediateContext, m_ArraySize - 1, j, m_pTexArray[r], pDataByte2)))
					{
						pTest->WriteToLog( _T( "SrcObjects11::ExecuteVerifyTestCase() - Invalid data read in source resource, \
										   arraySlice = %d, MipSlice = %d, resource number = %u."), m_ArraySize - 1, j, r);
						return RESULT_FAIL;
					}
				}
			}
			else
			{
				if (FAILED(hr = pTest->VerifyTex2D(m_pDevice, m_pImmediateContext, m_ArraySize - 1, j, m_pTex, pDataByte2)))
				{
					pTest->WriteToLog( _T( "SrcObjects11::ExecuteVerifyTestCase() - Invalid data read in source resource, \
									   arraySlice = %d, MipSlice = %d."), m_ArraySize - 1, j);
					return RESULT_FAIL;
				}
			}
		}
		
	}
	return RESULT_PASS;
}


TEST_RESULT SrcObjectsVideo::ExecuteVerifyTestCase(SharedResourcesTest *pTest,  BYTE* pDataByte1, BYTE* pDataByte2)
{
	HRESULT hr;
	
	if (FAILED(hr = pTest->VerifyTex2D(m_pDevice, m_pImmediateContext, 0, 0, m_pTex, pDataByte2)))
	{
		pTest->WriteToLog( _T( "SrcObjectsVideo::ExecuteVerifyTestCase() - Invalid data read in source resource." ));
		return RESULT_FAIL;
	}
	return RESULT_PASS;
}

void SharedResourcesTest::CleanupTestCase()
{
	if (m_sharedMemory)
	{
		if (m_testType == SR_OTHER_PROCESS || m_isChildProcess)
		{
			UnmapViewOfFile(m_sharedMemory);
		}
		else
		{
			delete m_sharedMemory;
		}

		m_sharedMemory = NULL;
	}

	if (m_pDataByte1)
	{
		delete[] m_pDataByte1;
		m_pDataByte1 = NULL;
	}

	if (m_pDataByte2)
	{
		delete[] m_pDataByte2;
		m_pDataByte2 = NULL;
	}

	if (m_pSrcObjs)
	{
		delete m_pSrcObjs;
		m_pSrcObjs = NULL;
	}

	if (m_pDestObjs)
	{
		delete m_pDestObjs;
		m_pDestObjs = NULL;
	}
	
	// Reset state of test case specific events
	if (!ResetEvent(m_finishUpdateSharedMemEvent))
	{
		WriteToLog( _T("SharedResourcesTest::CleanupTestCase() - ResetEvent for m_finishUpdateSharedMemEvent failed with error (%d)" ), GetLastError() );
	}
	if (!ResetEvent(m_startChildResAccessEvent))
	{
		WriteToLog( _T("SharedResourcesTest::CleanupTestCase() - ResetEvent for m_startChildResAccessEvent failed with error (%d)" ), GetLastError() );
	}
	if (!ResetEvent(m_endChildResAccessEvent))
	{
		WriteToLog( _T("SharedResourcesTest::CleanupTestCase() - ResetEvent for m_endChildResAccessEvent failed with error (%d)" ), GetLastError() );
	}
	if (!ResetEvent(m_endChildCaseWithError))
	{
		WriteToLog( _T("SharedResourcesTest::CleanupTestCase() - ResetEvent for m_endChildCaseWithError failed with error (%d)" ), GetLastError() );
	}
	
}

SrcObjectsCompBuffer::~SrcObjectsCompBuffer()
{
	
}

DestObjectsGDI::~DestObjectsGDI()
{
	
}

SrcObjectsGDI::~SrcObjectsGDI()
{
	
}

SrcObjectsVideo::~SrcObjectsVideo()
{
	SAFE_RELEASE(m_pTex);

}

DestObjectsVideo::~DestObjectsVideo()
{
	SAFE_RELEASE(m_pTexShared);

	
	SAFE_RELEASE(m_pVideoDevice);
	SAFE_RELEASE(m_pVideoContext);
	SAFE_RELEASE(m_pVideoProcessorEnum);
	SAFE_RELEASE(m_pVideoProcessor);

	SAFE_RELEASE(m_pImmediateContext);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pDevice11_1);
}


SrcObjects11::~SrcObjects11()
{
	SAFE_RELEASE(m_pTex);
	SAFE_RELEASE(m_pSrcTex);
	for ( UINT i = 0;  i < m_TexArrayCount; i++ )
	{
		SAFE_RELEASE(m_pTexArray[i]);
	}
	SAFE_DELETE_ARRAY(m_pTexArray);
	SAFE_DELETE_ARRAY(m_SharedHandleArray);
}

DestObjects11::~DestObjects11()
{
	SAFE_RELEASE(m_pTexShared);
	SAFE_RELEASE(m_pSRRTTex);

	SAFE_RELEASE(m_pDevTest);
	SAFE_RELEASE(m_pEffectiveContext);
	SAFE_RELEASE(m_pImmediateContext);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pDevice11_1);
	SAFE_RELEASE(m_pPartnerDevice);
}

SrcObjects10::~SrcObjects10()
{
	SAFE_RELEASE(m_pTex);
	SAFE_RELEASE(m_pSrcTex);
}

DestObjects10::~DestObjects10()
{
	SAFE_RELEASE(m_pTexShared);

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pDevice11_1);
}

SrcObjects9::~SrcObjects9()
{
	SAFE_RELEASE(m_pTex);
	SAFE_RELEASE(m_pSrcTex);
}

DestObjects9::~DestObjects9()
{
	SAFE_RELEASE(m_pTexShared);
	SAFE_RELEASE(m_pDevice);
}

void SharedResourcesTest::Cleanup()
{
	SAFE_RELEASE(m_pD3D9);

	if (!m_isChildProcess)
	{
		//tell child process to end
		if ( m_exitChildProcessEvent && ! SetEvent(m_exitChildProcessEvent) ) 
		{ 
			WriteToLog( _T( "SharedResourcesTest::Cleanup() - Failed to signal child process to exit."));
		}

		if (m_ProcInf.hProcess)
		{
			//end the child process
			if (WAIT_OBJECT_0 != WaitForSingleObject( m_ProcInf.hProcess, 5000 )) //wait for a bit
			{
				WriteToLog(_T("SharedResourcesTest::Cleanup() - Child process didn't exit in 5 second after being signaled. \
							  It'll be terminated by the main process." ));
				TerminateProcess(m_ProcInf.hProcess, 0); //kill it
				// wait for child to actually exit to make sure all handles opened by child are closed
				// since TerminateProcess returns immediately without waiting for the process to actually exit.
				if (WAIT_OBJECT_0 != WaitForSingleObject( m_ProcInf.hProcess, 10000 ))
					WriteToLog(_T("SharedResourcesTest::Cleanup() - Failed to terminate Child process in 10 seconds." ));
			}

			//close process
			CloseHandle(m_ProcInf.hProcess);
			CloseHandle(m_ProcInf.hThread);
			ZeroMemory( &m_ProcInf, sizeof(m_ProcInf) );
		}
	}
	
	if (m_hFileMap)
		CloseHandle(m_hFileMap);
	if (m_startChildResAccessEvent)
		CloseHandle(m_startChildResAccessEvent);
	if (m_endChildResAccessEvent)
		CloseHandle(m_endChildResAccessEvent);
	if (m_exitChildProcessEvent)
		CloseHandle(m_exitChildProcessEvent);
	if (m_finishUpdateSharedMemEvent)
		CloseHandle(m_finishUpdateSharedMemEvent);
	if (m_endChildCaseWithError)
		CloseHandle(m_endChildCaseWithError);
	
	m_hFileMap = NULL;
	m_startChildResAccessEvent = NULL;
	m_endChildResAccessEvent = NULL;
	m_exitChildProcessEvent = NULL;
	m_finishUpdateSharedMemEvent = NULL;
	m_endChildCaseWithError = NULL;
	
}

/** Get the D3D_FORMAT equivilant to the given DXGI_FORMAT.
Returns D3DFMT_UNKNOWN is the format is not supported for shared resources. */
D3DFORMAT GetD3DFormat(DXGI_FORMAT pFormat)
{
	D3DFORMAT format;

	switch(pFormat)
	{ 
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		format = D3DFMT_A8B8G8R8;
		break;
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		format = D3DFMT_A8R8G8B8;
		break;
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		format = D3DFMT_X8R8G8B8;
		break;
	case DXGI_FORMAT_R10G10B10A2_UNORM:
		format = D3DFMT_A2B10G10R10;
		break;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		format = D3DFMT_A16B16G16R16F;
		break;
	default:
		format = D3DFMT_UNKNOWN;
	};

	return format;
}

/** Creates a new IDirect3DDevice9Ex. */
LPDIRECT3DDEVICE9EX SharedResourcesTest::Create9Device()
{
	HRESULT hr;
	IDirect3DDevice9Ex *device = NULL;

	D3DPRESENT_PARAMETERS PresentationParameters;
	ZeroMemory( &PresentationParameters, sizeof( D3DPRESENT_PARAMETERS ) );
	PresentationParameters.BackBufferWidth = PresentationParameters.BackBufferHeight = 1;
	PresentationParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	PresentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	PresentationParameters.Windowed = TRUE;
	PresentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	if (FAILED (hr = m_pD3D9->CreateDeviceEx(D3DADAPTER_DEFAULT,
											D3DDEVTYPE_HAL,
											NULL,
											D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
											&PresentationParameters,
											NULL,
											&device )))
	{
		WriteToLog( _T( "SharedResourcesTest::Create9Device - CreateDeviceEx failed. hr = %s" ), D3DHResultToString(hr).c_str());
	}

	return device;
}

ID3D10Device* SharedResourcesTest::Create10Device()
{
	HRESULT hr;
	ID3D10Device *device = NULL;
	IDXGIAdapter *pDXGIAdapter = g_App.GetDXGIAdapter();

	if ( pDXGIAdapter == NULL)
	{
		WriteToLog( _T( "SharedResourcesTest::Create10Device - DXGIAdapter pointer is NULL." ));
	}


	if (FAILED (hr = D3D10CreateDevice( pDXGIAdapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_SDK_VERSION, &device )))
	{
		WriteToLog( _T( "SharedResourcesTest::Create10Device - D3D10CreateDevice failed. hr = %s" ), D3DHResultToString(hr).c_str());
	}

	return device;
}

ID3D10Device* SharedResourcesTest::Create10_1Device( D3D_FEATURE_LEVEL fl )
{
	HRESULT hr;
	ID3D10Device1 *device = NULL;

	IDXGIAdapter *pDXGIAdapter = g_App.GetDXGIAdapter();

	if ( pDXGIAdapter == NULL)
	{
		WriteToLog( _T( "SharedResourcesTest::Create10Device - DXGIAdapter pointer is NULL." ));
	}

	if (FAILED (hr = D3D10CreateDevice1( pDXGIAdapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, (D3D10_FEATURE_LEVEL1)fl, D3D10_1_SDK_VERSION, &device )))
	{
		WriteToLog( _T( "SharedResourcesTest::Create10_1Device - D3D10CreateDevice1 failed. hr = %s" ), D3DHResultToString(hr).c_str());
	}

	return device;
}


//right now no test cases open shared resources on 9 devices....
HRESULT SharedResourcesTest::VerifyIDirect3DTexture9(IDirect3DTexture9 *tex, BYTE *data, D3DFORMAT D3DFormat, tstring* failureString)
{
HRESULT hr =  S_OK;
	UINT bytesPerElement = GetBitsPerElement( m_Format ) / 8;
	bool bDidLock = false;

	IDirect3DDevice9* pTexDevice = NULL;
	IDirect3DTexture9* pRTTexture9 = NULL;
	IDirect3DTexture9* pTempTexture9 = NULL;
	IDirect3DSurface9* pTexSurface9 = NULL;
	IDirect3DSurface9* pRTSurface9 = NULL;
	IDirect3DSurface9* pTempSurface9 = NULL;
	if (FAILED(hr = tex->GetDevice(&pTexDevice)) || !pTexDevice)
	{
		tstring failStr = FormatString(_T("SharedResourcesTest::VerifyIDirect3DTexture9() - GetDevice Failed hr = %s"), D3DHResultToString(hr).c_str());
		if (failureString)
			*failureString = failStr;
		WriteToLog(_T("%s"), failStr.c_str());
		goto EXIT;
	}

	D3DSURFACE_DESC texDesc;
	if (FAILED (hr = tex->GetLevelDesc(0, &texDesc)))
	{
		tstring failStr = FormatString(_T("SharedResourcesTest::VerifyIDirect3DTexture9() - GetLevelDesc Failed hr = %s"), D3DHResultToString(hr).c_str());
		if (failureString)
			*failureString = failStr;
		WriteToLog(_T("%s"), failStr.c_str());
		goto EXIT;
	}
	if (texDesc.Pool == D3DPOOL_SYSTEMMEM)
	{
		pTempTexture9 = tex;
		pTempTexture9->AddRef();
	}
	else
	{
		if (FAILED (hr = pTexDevice->CreateTexture(
			m_ResWidth,
			m_ResHeight,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFormat,
			D3DPOOL_DEFAULT,
			&pRTTexture9,
			NULL)))
		{
			tstring failStr = FormatString(_T("SharedResourcesTest::VerifyIDirect3DTexture9() - CreateTexture(RT) Failed hr = %s"), D3DHResultToString(hr).c_str());
			if (failureString)
				*failureString = failStr;
			WriteToLog(_T("%s"), failStr.c_str());
			goto EXIT;
		} 
		if (FAILED (hr = pTexDevice->CreateTexture(
			m_ResWidth,
			m_ResHeight,
			1,
			0,
			D3DFormat,
			D3DPOOL_SYSTEMMEM,
			&pTempTexture9,
			NULL)))
		{
			tstring failStr = FormatString(_T("SharedResourcesTest::VerifyIDirect3DTexture9() - CreateTexture(Temp) Failed hr = %s"), D3DHResultToString(hr).c_str());
			if (failureString)
				*failureString = failStr;
			WriteToLog(_T("%s"), failStr.c_str());
			goto EXIT;
		} 

		if (FAILED(hr = tex->GetSurfaceLevel(0, &pTexSurface9)))
		{
			tstring failStr = FormatString(_T("SharedResourcesTest::VerifyIDirect3DTexture9() - GetSurfaceLevel(Tex) Failed hr = %s"), D3DHResultToString(hr).c_str());
			if (failureString)
				*failureString = failStr;
			WriteToLog(_T("%s"), failStr.c_str());
			goto EXIT;
		}
		if (FAILED(hr = pRTTexture9->GetSurfaceLevel(0, &pRTSurface9)))
		{
			tstring failStr = FormatString(_T("SharedResourcesTest::VerifyIDirect3DTexture9() - GetSurfaceLevel(RT) Failed hr = %s"), D3DHResultToString(hr).c_str());
			if (failureString)
				*failureString = failStr;
			WriteToLog(_T("%s"), failStr.c_str());
			goto EXIT;
		}
		if (FAILED(hr = pTempTexture9->GetSurfaceLevel(0, &pTempSurface9)))
		{
			tstring failStr = FormatString(_T("SharedResourcesTest::VerifyIDirect3DTexture9() - GetSurfaceLevel(Temp) Failed hr = %s"), D3DHResultToString(hr).c_str());
			if (failureString)
				*failureString = failStr;
			WriteToLog(_T("%s"), failStr.c_str());
			goto EXIT;
		}

		if (FAILED(hr = pTexDevice->StretchRect(pTexSurface9, NULL, pRTSurface9, NULL, D3DTEXF_NONE)) )
		{
			tstring failStr = FormatString(_T("SharedResourcesTest::VerifyIDirect3DTexture9() - StretchRect Failed hr = %s"), D3DHResultToString(hr).c_str());
			if (failureString)
				*failureString = failStr;
			WriteToLog(_T("%s"), failStr.c_str());
			goto EXIT;
		}
		if (FAILED(hr = pTexDevice->GetRenderTargetData(pRTSurface9, pTempSurface9)) )
		{
			tstring failStr = FormatString(_T("SharedResourcesTest::VerifyIDirect3DTexture9() - GetRenderTargetData Failed hr = %s"), D3DHResultToString(hr).c_str());
			if (failureString)
				*failureString = failStr;
			WriteToLog(_T("%s"), failStr.c_str());
			goto EXIT;
		}
	}

	D3DLOCKED_RECT LockedRect;
	if( FAILED( hr = pTempTexture9->LockRect(0, &LockedRect, NULL, D3DLOCK_READONLY ) ) )
	{
		tstring failStr = FormatString(_T("SharedResourcesTest::VerifyIDirect3DTexture9() - Failed to lock the d3d9 surface. hr = %s\n"), D3DHResultToString(hr).c_str() );
		if (failureString)
			*failureString = failStr;
		WriteToLog(_T("%s"), failStr.c_str());
		goto EXIT;
	}
	bDidLock = true;

	BYTE* pData=(BYTE*) LockedRect.pBits;
	for( UINT y = 0; y < m_ResHeight; y++ )
	{
		for( UINT x = 0; x < m_ResWidth * bytesPerElement; x++ )
		{		
			const BYTE desiredData = data[x+y*bytesPerElement*m_ResWidth];
			const BYTE currData = pData[(y * LockedRect.Pitch) + x];
			if (desiredData != currData)
			{
				if (   D3DFormat == D3DFMT_X8R8G8B8
					&& (x % 4) == 3)
				{
					// the "X" in XRGB can be anything
					continue;
				}
				if( D3DFormat == D3DFMT_A16B16G16R16F )
				{
					const BYTE* pDesiredDataAligned = &data[ (x&~1) +y*bytesPerElement*m_ResWidth];
					const BYTE* pCurrDataAligned = &pData[(y * LockedRect.Pitch) + (x&~1)];
					FLOAT_COLOR desiredFloat;
					FLOAT_COLOR currFloat;
					if ( !CFormatHelper::ConvertPixels(CONV_FROM_FORMAT, DXGI_FORMAT_R16_FLOAT, &desiredFloat, 1, 1, (void*)pDesiredDataAligned) )
					{
						WriteToLog(_T("%s in %s at line %d: CFormatHelper::ConvertPixels() failed"), __FUNCTION__, __FILE__, __LINE__);
						hr = E_FAIL;
						goto EXIT;
					}
					if ( !CFormatHelper::ConvertPixels(CONV_FROM_FORMAT, DXGI_FORMAT_R16_FLOAT, &currFloat, 1, 1, (void*)pCurrDataAligned) )
					{
						WriteToLog(_T("%s in %s at line %d: CFormatHelper::ConvertPixels() failed"), __FUNCTION__, __FILE__, __LINE__);
						hr = E_FAIL;
						goto EXIT;
					}
					if( _isnan( desiredFloat.comp[0] ) && _isnan( currFloat.comp[0] ) )
					{
						// there are very many NaN values.
						continue;
					}
				}
				tstring failStr = FormatString("SharedResourcesTest::VerifyIDirect3DTexture9 - %d instead of %d found at position (%d, %d)", 
					currData, desiredData, 
					x, y);
				if (failureString)
					*failureString = failStr;
				WriteToLog(_T("%s"), failStr.c_str());
				hr = E_FAIL;
				goto EXIT;
			}
		}
	}

EXIT:
	if (pTempTexture9 && bDidLock)
		pTempTexture9->UnlockRect(0);
	SAFE_RELEASE(pRTTexture9);
	SAFE_RELEASE(pTempTexture9);
	SAFE_RELEASE(pTexDevice);
	SAFE_RELEASE(pTexSurface9);
	SAFE_RELEASE(pRTSurface9);
	SAFE_RELEASE(pTempSurface9);

	return hr;
}

/** Verifies that the contents of tex match those of m_pDataByte. */
HRESULT SharedResourcesTest::VerifyTex2D(ID3D10Device *device, 
										 UINT arrayIndex, 
										 UINT mipIndex, 
										 ID3D10Texture2D *tex, 
										 BYTE *data, 
										 tstring* failureString)
{
	HRESULT hr = S_OK;

	//create a staging resource to map open shared resource
	UINT subResourceWidth = m_ResWidth >> mipIndex;
	UINT subResourceHeight = m_ResHeight >> mipIndex;
	ID3D10Texture2D *stagingTex;
	D3D10_TEXTURE2D_DESC tex2DDesc;
	tex2DDesc.Height			= subResourceHeight;
	tex2DDesc.Width				= subResourceWidth;
	tex2DDesc.ArraySize			= 1;
	tex2DDesc.MipLevels			= 1;
	tex2DDesc.Format			= m_Format;
	tex2DDesc.SampleDesc.Count	= 1;
	tex2DDesc.SampleDesc.Quality = 0;
	tex2DDesc.Usage				= D3D10_USAGE_STAGING;
	tex2DDesc.BindFlags			= 0;
	tex2DDesc.CPUAccessFlags	= D3D10_CPU_ACCESS_READ;
	tex2DDesc.MiscFlags			= 0;

	hr = device->CreateTexture2D( &tex2DDesc, NULL, &stagingTex);
	
	if( FAILED( hr ))
	{
		tstring failStr = FormatString(_T("SharedResourcesTest::VerifyTex2D - CreateTexture2D for staging resource failed. hr = %s. "), D3DHResultToString(hr).c_str());
		if (failureString)
			*failureString = failStr;
		WriteToLog(_T("%s"), failStr.c_str());
		return hr;
	}

	//copy to staging resource
	device->CopySubresourceRegion(
		stagingTex,
		0,
		0, 0, 0,
		tex,
		arrayIndex * m_MipLevels + mipIndex,
		NULL);

	D3D10_MAPPED_TEXTURE2D tex2DMap;
	if (FAILED(hr = stagingTex->Map(0, D3D10_MAP_READ, 0, &tex2DMap)))
	{
		tstring failStr = FormatString( _T( "SharedResourcesTest::VerifyTex2D - CTexture2D::Map() failed. hr = %s. " ), D3DHResultToString( hr ).c_str() );
		if (failureString)
			*failureString = failStr;
		WriteToLog(_T("%s"), failStr.c_str());
		return hr;
	}
	
	//verify the contents of the staging buffer
	BYTE* pData = (BYTE*) tex2DMap.pData;
	
	UINT bytesPerElement = GetBitsPerElement( m_Format ) / 8;
	for (UINT y = 0; y < (subResourceHeight); ++y)
	{
		for (UINT x = 0; x < (subResourceWidth * bytesPerElement); ++x)
		{
			const BYTE desiredData = data[x+y*bytesPerElement*subResourceWidth];
			const BYTE currData = pData[(y * tex2DMap.RowPitch) + x];
			if (desiredData != currData)
			{
				if (   (m_Format == DXGI_FORMAT_B8G8R8X8_UNORM || m_Format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB)
					&& (x % 4) == 3)
				{
					// the "X" in BGRX can be anything
					continue;
				}
				if( m_Format == DXGI_FORMAT_R16G16B16A16_FLOAT )
				{
					const BYTE* pDesiredDataAligned = &data[ (x&~1) +y*bytesPerElement*m_ResWidth];
					const BYTE* pCurrDataAligned = &pData[(y * tex2DMap.RowPitch) + (x&~1)];
					FLOAT_COLOR desiredFloat;
					FLOAT_COLOR currFloat;
					if ( !CFormatHelper::ConvertPixels(CONV_FROM_FORMAT, DXGI_FORMAT_R16_FLOAT, &desiredFloat, 1, 1, (void*)pDesiredDataAligned) )
					{
						WriteToLog(_T("%s in %s at line %d: CFormatHelper::ConvertPixels() failed"), __FUNCTION__, __FILE__, __LINE__);
						hr = E_FAIL;
						goto EXIT;
					}
					if ( !CFormatHelper::ConvertPixels(CONV_FROM_FORMAT, DXGI_FORMAT_R16_FLOAT, &currFloat, 1, 1, (void*)pCurrDataAligned) )
					{
						WriteToLog(_T("%s in %s at line %d: CFormatHelper::ConvertPixels() failed"), __FUNCTION__, __FILE__, __LINE__);
						hr = E_FAIL;
						goto EXIT;
					}
					if( _isnan( desiredFloat.comp[0] ) && _isnan( currFloat.comp[0] ) )
					{
						// there are very many NaN values.
						continue;
					}
				}
				tstring failStr = FormatString(_T("SharedResourcesTest::VerifyTex2D - %d instead of %d found at position (%d, %d). "), 
					currData, desiredData, 
					x, y);
				if (failureString)
					*failureString = failStr;
				WriteToLog(_T("%s"), failStr.c_str());
				hr = E_FAIL;
				goto EXIT;
			}
		}
	}

EXIT:
	
	//release staging buffer
	if (stagingTex) stagingTex->Unmap(0);
	SAFE_RELEASE(stagingTex);

	return hr;
}

/** Verifies that the contents of tex match those of m_pDataByte. */
HRESULT SharedResourcesTest::VerifyTex2D(ID3D11Device *device, 
										 ID3D11DeviceContext *immediatecontext, 
										 UINT arrayIndex, 
										 UINT mipIndex, 
										 ID3D11Texture2D *tex, 
										 BYTE *data, 
										 tstring* failureString)
{
	HRESULT hr = S_OK;

	//create a staging resource to map open shared resource
	UINT subResourceWidth = m_ResWidth >> mipIndex;
	UINT subResourceHeight = m_ResHeight >> mipIndex;
	ID3D11Texture2D *stagingTex;
	D3D11_TEXTURE2D_DESC tex2DDesc;
	tex->GetDesc( &tex2DDesc );
	UINT mipLevels = tex2DDesc.MipLevels;
	UINT arraySize = tex2DDesc.ArraySize;

	tex2DDesc.SampleDesc.Count	= 1;
	tex2DDesc.SampleDesc.Quality = 0;
	tex2DDesc.Usage				= D3D11_USAGE_STAGING;
	tex2DDesc.BindFlags			= 0;
	tex2DDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ;
	tex2DDesc.MiscFlags			= 0;

	hr = device->CreateTexture2D( &tex2DDesc, NULL, &stagingTex);

	if( FAILED( hr ))
	{
		tstring failStr = FormatString(_T("SharedResourcesTest::VerifyTex2D - CreateTexture2D for staging resource failed. hr = %s. "), D3DHResultToString(hr).c_str());
		if (failureString)
			*failureString = failStr;
		WriteToLog(_T("%s"), failStr.c_str());
		return hr;
	}

	immediatecontext->CopyResource(stagingTex, tex);
	
	D3D11_MAPPED_SUBRESOURCE tex2DMap;
	if (FAILED(hr = immediatecontext->Map(stagingTex, mipLevels * arrayIndex + mipIndex, D3D11_MAP_READ, 0, &tex2DMap)))
	{
		tstring failStr = FormatString( _T( "SharedResourcesTest::VerifyTex2D - CTexture2D::Map() failed. hr = %s. " ), D3DHResultToString( hr ).c_str() );
		if (failureString)
			*failureString = failStr;
		WriteToLog(_T("%s"), failStr.c_str());
		return hr;
	}
	
	//verify the contents of the staging buffer
	BYTE* pData = (BYTE*) tex2DMap.pData;
	
	UINT bytesPerElement = GetBitsPerElement( m_Format ) / 8;

	// real dimensions for BC formats
	UINT rowBytes = subResourceWidth * bytesPerElement;
	if( m_FormatBlockSize > 1 )
	{
		rowBytes = subResourceWidth / 4 * m_FormatBlockSize;
		subResourceHeight = subResourceHeight / 4;
	}
	for (UINT y = 0; y < (subResourceHeight); ++y)
	{
		for (UINT x = 0; x < rowBytes; ++x)
		{
			const BYTE desiredData = data[x+y*rowBytes];
			const BYTE currData = pData[(y * tex2DMap.RowPitch) + x];
			if (desiredData != currData)
			{
				if (   (m_Format == DXGI_FORMAT_B8G8R8X8_UNORM || m_Format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB)
					&& (x % 4) == 3)
				{
					// the "X" in BGRX can be anything
					continue;
				}
				if( m_Format == DXGI_FORMAT_R16G16B16A16_FLOAT )
				{
					const BYTE* pDesiredDataAligned = &data[ (x&~1) +y*bytesPerElement*m_ResWidth];
					const BYTE* pCurrDataAligned = &pData[(y * tex2DMap.RowPitch) + (x&~1)];
					FLOAT_COLOR desiredFloat;
					FLOAT_COLOR currFloat;
					if ( !CFormatHelper::ConvertPixels(CONV_FROM_FORMAT, DXGI_FORMAT_R16_FLOAT, &desiredFloat, 1, 1, (void*)pDesiredDataAligned) )
					{
						WriteToLog(_T("%s in %s at line %d: CFormatHelper::ConvertPixels() failed"), __FUNCTION__, __FILE__, __LINE__);
						hr = E_FAIL;
						goto EXIT;
					}
					if ( !CFormatHelper::ConvertPixels(CONV_FROM_FORMAT, DXGI_FORMAT_R16_FLOAT, &currFloat, 1, 1, (void*)pCurrDataAligned) )
					{
						WriteToLog(_T("%s in %s at line %d: CFormatHelper::ConvertPixels() failed"), __FUNCTION__, __FILE__, __LINE__);
						hr = E_FAIL;
						goto EXIT;
					}
					if( _isnan( desiredFloat.comp[0] ) && _isnan( currFloat.comp[0] ) )
					{
						// there are very many NaN values.
						continue;
					}
				}
				if ( GetFlags(m_Format) & FF_VIDEO )
				{
					// leave some error margin for video format, since the video device hardware may do some optimization 
					const BYTE RelativeEpsilon = 10;
					if (abs( desiredData - currData ) < RelativeEpsilon )
						continue;
				}
				tstring failStr = FormatString(_T("SharedResourcesTest::VerifyTex2D - %d instead of %d found at position (%d, %d). "), 
					currData, desiredData, 
					x, y);
				if (failureString)
					*failureString = failStr;
				WriteToLog(_T("%s"), failStr.c_str());
				hr = E_FAIL;
				goto EXIT;
			}
		}
	}

EXIT:
	
	//release staging buffer
	if (stagingTex) immediatecontext->Unmap(stagingTex, mipLevels * arrayIndex + mipIndex);
	SAFE_RELEASE(stagingTex);

	return hr;
}

// For debugging uses
HRESULT SharedResourcesTest::VerifyTex2D_D(ID3D11Device *device, 
										 ID3D11DeviceContext *immediatecontext, 									
										 ID3D11Texture2D *tex, 
										 BYTE *data)
{
	HRESULT hr = S_OK;

	//create a staging resource to map open shared resource
	ID3D11Texture2D *stagingTex;
	D3D11_TEXTURE2D_DESC tex2DDesc;
	tex->GetDesc( &tex2DDesc );
	//tex2DDesc.Height			= m_ResHeight;
	//tex2DDesc.Width				= m_ResWidth;
	tex2DDesc.ArraySize			= 1;
	tex2DDesc.MipLevels			= 1;

	tex2DDesc.Format			= m_Format;
	tex2DDesc.SampleDesc.Count	= 1;
	tex2DDesc.SampleDesc.Quality = 0;
	tex2DDesc.Usage				= D3D11_USAGE_STAGING;
	tex2DDesc.BindFlags			= 0;
	tex2DDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ;
	tex2DDesc.MiscFlags			= 0;

	hr = device->CreateTexture2D( &tex2DDesc, NULL, &stagingTex);

	if( FAILED( hr ))
	{
		tstring failStr = FormatString(_T("SharedResourcesTest::VerifyTex2D - CreateTexture2D for staging resource failed. hr = %s. "), D3DHResultToString(hr).c_str());
		WriteToLog(_T("%s"), failStr.c_str());
		return hr;
	}

	immediatecontext->CopyResource(stagingTex, tex);
	
	D3D11_MAPPED_SUBRESOURCE tex2DMap;
	if (FAILED(hr = immediatecontext->Map(stagingTex, 0, D3D11_MAP_READ, 0, &tex2DMap)))
	{
		tstring failStr = FormatString( _T( "SharedResourcesTest::VerifyTex2D - CTexture2D::Map() failed. hr = %s. " ), D3DHResultToString( hr ).c_str() );
		WriteToLog(_T("%s"), failStr.c_str());
		return hr;
	}
	
	//verify the contents of the staging buffer
	BYTE* pData = (BYTE*) tex2DMap.pData;
	
	UINT bytesPerElement = GetBitsPerElement( m_Format ) / 8;
	UINT subResourceHeight = tex2DDesc.Height;
	UINT subResourceWidth = tex2DDesc.Width;

	// real dimensions for BC formats
	UINT rowBytes = subResourceWidth * bytesPerElement;
	if( m_FormatBlockSize > 1 )
	{
		rowBytes = subResourceWidth / 4 * m_FormatBlockSize;
		subResourceHeight = subResourceHeight / 4;
	}
	for (UINT y = 0; y < (subResourceHeight); ++y)
	{
		for (UINT x = 0; x < rowBytes; ++x)
		{
			const BYTE desiredData = data[x+y*rowBytes];
			const BYTE currData = pData[(y * tex2DMap.RowPitch) + x];
			if (desiredData != currData)
			{
				if (   (m_Format == DXGI_FORMAT_B8G8R8X8_UNORM || m_Format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB)
					&& (x % 4) == 3)
				{
					// the "X" in BGRX can be anything
					continue;
				}
				if( m_Format == DXGI_FORMAT_R16G16B16A16_FLOAT )
				{
					const BYTE* pDesiredDataAligned = &data[ (x&~1) +y*bytesPerElement*m_ResWidth];
					const BYTE* pCurrDataAligned = &pData[(y * tex2DMap.RowPitch) + (x&~1)];
					FLOAT_COLOR desiredFloat;
					FLOAT_COLOR currFloat;
					if ( !CFormatHelper::ConvertPixels(CONV_FROM_FORMAT, DXGI_FORMAT_R16_FLOAT, &desiredFloat, 1, 1, (void*)pDesiredDataAligned) )
					{
						WriteToLog(_T("%s in %s at line %d: CFormatHelper::ConvertPixels() failed"), __FUNCTION__, __FILE__, __LINE__);
						hr = E_FAIL;
						goto EXIT;
					}
					if ( !CFormatHelper::ConvertPixels(CONV_FROM_FORMAT, DXGI_FORMAT_R16_FLOAT, &currFloat, 1, 1, (void*)pCurrDataAligned) )
					{
						WriteToLog(_T("%s in %s at line %d: CFormatHelper::ConvertPixels() failed"), __FUNCTION__, __FILE__, __LINE__);
						hr = E_FAIL;
						goto EXIT;
					}
					if( _isnan( desiredFloat.comp[0] ) && _isnan( currFloat.comp[0] ) )
					{
						// there are very many NaN values.
						continue;
					}
				}
				if ( GetFlags(m_Format) & FF_VIDEO )
				{
					// leave some error margin for video format, since the video device hardware may do some optimization 
					const BYTE RelativeEpsilon = 10;
					if (abs( desiredData - currData ) < RelativeEpsilon )
						continue;
				}
				tstring failStr = FormatString(_T("SharedResourcesTest::VerifyTex2D - %d instead of %d found at position (%d, %d). "), 
					currData, desiredData, 
					x, y);
				WriteToLog(_T("%s"), failStr.c_str());
				hr = E_FAIL;
				goto EXIT;
			}
		}
	}

EXIT:
	
	//release staging buffer
	if (stagingTex) immediatecontext->Unmap(stagingTex, 0);
	SAFE_RELEASE(stagingTex);

	return hr;
}


HRESULT SharedResourcesTest::WaitForEvent(ID3D11Device* pDevice11, ID3D10Device* pDevice10, IDirect3DDevice9* pDevice9)
{
	HRESULT hr = E_INVALIDARG;

	if (pDevice11)
	{
		ID3D11Query* pEvent = NULL;
		D3D11_QUERY_DESC QueryDesc;
		QueryDesc.Query = D3D11_QUERY_EVENT;
		QueryDesc.MiscFlags = 0;
		ID3D11DeviceContext* immContext = NULL; 
		pDevice11->GetImmediateContext(&immContext);
		if (!immContext)
		{
			WriteToLog( _T("GetImmediateContext failed") );
			hr = E_FAIL;
		}
		else
		{
			hr = pDevice11->CreateQuery(&QueryDesc, &pEvent);
			if (SUCCEEDED(hr))
			{
				immContext->End(pEvent);

				hr = GetFramework()->GetDataWithTimeout(immContext, pEvent, nullptr, 0, 0);
				if (hr != S_OK)
				{
					WriteToLog( _T("GetData failed or timed out for Event query. HR = %s"), D3DHResultToString(hr).c_str() );
					if (SUCCEEDED(hr)) hr = E_FAIL;
				}

				pEvent->Release();
				SAFE_RELEASE(immContext);
			}
			else
			{
				WriteToLog( _T("CreateQuery failed for Event query. HR = %s"), D3DHResultToString(hr).c_str() );
			}
		}
	}
	else if (pDevice10)
	{
		ID3D10Query* pEvent = NULL;
		D3D10_QUERY_DESC QueryDesc;
		QueryDesc.Query = D3D10_QUERY_EVENT;
		QueryDesc.MiscFlags = 0;
		hr = pDevice10->CreateQuery(&QueryDesc, &pEvent);
		if (SUCCEEDED(hr))
		{
			pEvent->End();

			// Copy/paste of GetDataWithTimeout modified for D3D10
			DWORD startTime = timeGetTime();
			DWORD currentTime;
			UINT timeout = GetFramework()->GetQueryTimeout();

			//
			// Loop until query completes or times out
			//
			for(;;)
			{
				hr = pEvent->GetData(nullptr, 0, 0);

				if( S_FALSE != hr )
					break;
		
				currentTime = timeGetTime();
				if( (currentTime - startTime) > timeout ) // Timeout in milliseconds
					break;

				Sleep(0);
			}
			if (hr != S_OK)
			{
				WriteToLog( _T("GetData failed or timed out for Event query. HR = %s"), D3DHResultToString(hr).c_str() );
				if (SUCCEEDED(hr)) hr = E_FAIL;
			}

			pEvent->Release();
		}
		else
		{
			WriteToLog( _T("CreateQuery failed for Event query. HR = %s"), D3DHResultToString(hr).c_str() );
		}
	}
	else if (pDevice9)
	{
		IDirect3DQuery9* pEvent = NULL;
		hr = pDevice9->CreateQuery(D3DQUERYTYPE_EVENT, &pEvent);
		if (SUCCEEDED(hr))
		{
			pEvent->Issue(D3DISSUE_END);

			// Copy/paste of GetDataWithTimeout modified for D3D9
			DWORD startTime = timeGetTime();
			DWORD currentTime;
			UINT timeout = GetFramework()->GetQueryTimeout();

			//
			// Loop until query completes or times out
			//
			for(;;)
			{
				hr = pEvent->GetData(nullptr, 0, D3DGETDATA_FLUSH);

				if( S_FALSE != hr )
					break;
		
				currentTime = timeGetTime();
				if( (currentTime - startTime) > timeout ) // Timeout in milliseconds
					break;

				Sleep(0);
			}
			if (hr != S_OK)
			{
				WriteToLog( _T("GetData failed or timed out for Event query. HR = %s"), D3DHResultToString(hr).c_str() );
				if (SUCCEEDED(hr)) hr = E_FAIL;
			}

			pEvent->Release();
		}
		else
		{
			WriteToLog( _T("CreateQuery failed for Event query. HR = %s"), D3DHResultToString(hr).c_str() );
		}
	}
	else
	{
		assert(!"Invalid arguments, must specify either 10 or 9 device pointer");
	}
	return hr;
}


HRESULT SharedResourcesTest::SyncCompBuffer( ID3D11Device* pDevice11 )
{
	// create the window. 
	HRESULT hr = S_OK;
	RECT WndRect = { 16, 16, MAX_RES_WIDTH + 16, MAX_RES_HEIGHT + 16 };
	DXGI_PRESENT_PARAMETERS PParams = { 0 };
	IDXGIDevice* pDXGI = NULL;
	IDXGIAdapter* pAdapter = NULL;
	IDXGIFactory2* pFactory2 = NULL;
	IDXGISwapChain1* pSwapChain1 = NULL;

	// Create swap_chain
	hr = pDevice11->QueryInterface(__uuidof(IDXGIDevice), (void**)(&pDXGI));
	if ( hr != S_OK || pDXGI == NULL)
	{
		WriteToLog( "SharedResourcesTest::SyncCompBuffer(): QueryInterface for IDXGIDevice failed. hr = %s", D3DHResultToString(hr).c_str());
		goto cleanup;
	}
	hr = pDXGI->GetAdapter(&pAdapter);
	if ( hr != S_OK || pAdapter == NULL)
	{
		WriteToLog( "SharedResourcesTest::SyncCompBuffer(): GetAdapter failed. hr = %s", D3DHResultToString(hr).c_str());
		goto cleanup;
	}
	hr = pAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)(&pFactory2));
	if ( hr != S_OK || pFactory2 == NULL)
	{
		WriteToLog( "SharedResourcesTest::SyncCompBuffer(): GetParent for IDXGIFactory2 failed. hr = %s", D3DHResultToString(hr).c_str());
		goto cleanup;
	}

	DXGI_SWAP_CHAIN_DESC1 SCDesc;
	SCDesc.Width = WndRect.right - WndRect.left;
	SCDesc.Height = WndRect.bottom - WndRect.top;
	SCDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SCDesc.Stereo = m_bStereo;
	SCDesc.SampleDesc.Count = 1;
	SCDesc.SampleDesc.Quality = 0;
	SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SCDesc.BufferCount = 2;
	SCDesc.Scaling = DXGI_SCALING_STRETCH;
	SCDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	SCDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	SCDesc.Flags = 0;

	hr = pFactory2->CreateSwapChainForHwnd( 
		pDevice11, 
		m_hWindow,
		&SCDesc,
		NULL,
		NULL,
		&pSwapChain1);
	if (hr != S_OK)
	{
		WriteToLog(	_T( "SharedResourcesTest::SyncCompBuffer(): CreateSwapChainForHwnd() failed with error %s." ),
				D3DHResultToString(hr).c_str() );
		goto cleanup;
	}
	
	// double present calls to sync CompositionBuffers
	hr = pSwapChain1->Present1( 0, 0, &PParams );
	if (hr != S_OK)
	{
		WriteToLog(	_T( "SharedResourcesTest::SyncCompBuffer(): first Present1() call failed with error %s." ),
				D3DHResultToString(hr).c_str() );
		goto cleanup;
	}
	hr = pSwapChain1->Present1( 0, 0, &PParams );
	if (hr != S_OK)
	{
		WriteToLog(	_T( "SharedResourcesTest::SyncCompBuffer(): Second Present1() call failed with error %s." ),
				D3DHResultToString(hr).c_str() );
		goto cleanup;
	}

cleanup:
	SAFE_RELEASE(pDXGI);
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pFactory2);
	SAFE_RELEASE(pSwapChain1);
	return hr;
}

const UINT g_NumTiles = 25;

void ShareTilePoolTest::InitTestParameters()
{
	// D3D11_RESOURCE_MISC_SHARED_NTHANDLE
	CTestCaseParameter<bool> *pMiscSharedNTHParam = AddParameter<bool>( _T("MiscSharedNTHandle"), &m_bMiscSharedNTHandle );
	pMiscSharedNTHParam->SetDefaultValue(false);
	testfactor::RFactor rMiscSharedNTHandle = AddParameterValueSet<bool>( pMiscSharedNTHParam, new CBoolValueSet() );
	testfactor::RFactor rMiscSharedNTHandleTrue = AddParameterValue<bool>( pMiscSharedNTHParam, true );


	// Sync method
	CTestCaseParameter<SR_SYNC_METHOD> *pSyncMethodParam = AddParameter<SR_SYNC_METHOD>( _T("SyncMethod"), &m_SyncMethod );
	pSyncMethodParam->SetDefaultValue( SR_KEYED_MUTEX );
	m_rSyncMethod_KeyedMutex = AddParameterValue<SR_SYNC_METHOD>( pSyncMethodParam, SR_KEYED_MUTEX );
	m_rSyncMethod_Event = AddParameterValue<SR_SYNC_METHOD>( pSyncMethodParam, SR_SYS_EVENT );
	m_rSyncMethod_All = m_rSyncMethod_Event*rMiscSharedNTHandle + m_rSyncMethod_KeyedMutex * rMiscSharedNTHandleTrue;
	
	testfactor::RFactor rRoot = m_rSyncMethod_All;

	SetRootTestFactor( rRoot );
}

TEST_RESULT ShareTilePoolTest::SetupTestCase()
{
	IDXGIDevice* pDXGI = NULL;
	IDXGIAdapter* pAdapter = NULL;
	ID3D11Device* pExisting = GetDevice();
	ID3D11Device* pDevice = NULL;
	IDXGIResource1* pDXGITilePool = NULL;
	TEST_RESULT tr = RESULT_PASS;

	HRESULT hr = pExisting->QueryInterface(__uuidof(IDXGIDevice), (void**)(&pDXGI));
	if (FAILED(hr) || pDXGI == NULL)
	{
		WriteToLog( _T( "ShareTilePoolTest::SetupTestCase - QueryInterface failed. hr = %s" ), D3DHResultToString(hr).c_str());
		goto func_end;
	}
	hr = pDXGI->GetAdapter(&pAdapter);
	const UINT createFlags = c_ValidD3DCreateFlags & pExisting->GetCreationFlags();
	D3D_FEATURE_LEVEL fl = g_App.GetDevice()->GetFeatureLevel();
	D3D_DRIVER_TYPE dt = pAdapter ? D3D_DRIVER_TYPE_UNKNOWN: g_App.m_D3DOptions.DriverType;
	hr = D3D11CreateDevice_11or12(
		g_App.m_D3DOptions.Force11On12 ? UseD3D11On12_D3D11CreateDeviceFlag : UseD3D11,
		pAdapter, 
		dt, 
		NULL, createFlags, 
		&fl, 
		1, 
		D3D11_SDK_VERSION, 
		&pDevice, 
		NULL, 
		NULL,
		NULL);
	
	if ( FAILED(hr) )
	{
		WriteToLog( _T( "ShareTilePoolTest::SetupTestCase - CreateDevice failed. hr = %s" ), D3DHResultToString(hr).c_str());
		goto func_end;
	}

	hr = pDevice->QueryInterface( __uuidof(ID3D11Device2), (void**)&m_pDevice );
	if( FAILED(hr) )
	{
		WriteToLog( _T( "ShareTilePoolTest::SetupTestCase - QI Device2 failed. hr = %s" ), D3DHResultToString(hr).c_str());
		goto func_end;
	}
	m_pDevice->GetImmediateContext2(&m_pContext);

	D3D11_FEATURE_DATA_D3D11_OPTIONS1 D3D11Options1;
	if( FAILED(m_pDevice->CheckFeatureSupport( D3D11_FEATURE_D3D11_OPTIONS1, &D3D11Options1, sizeof( D3D11Options1 ) ) ) ||
				(D3D11Options1.TiledResourcesTier == D3D11_TILED_RESOURCES_NOT_SUPPORTED))
	{
		WriteToLog( _T( "ShareTilePoolTest::SetupTestCase - Tiled Resources not supported - Shared Tile Pool test should not have been run. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		goto func_end;
	}

	D3D11_BUFFER_DESC DescBuffer;
	DescBuffer.ByteWidth = D3D11_2_TILED_RESOURCE_TILE_SIZE_IN_BYTES*1024;
	DescBuffer.BindFlags = 0;
	DescBuffer.Usage = D3D11_USAGE_DEFAULT;
	DescBuffer.CPUAccessFlags = 0;
	DescBuffer.MiscFlags = D3D11_RESOURCE_MISC_TILE_POOL;
	if( m_bMiscSharedNTHandle )
	{
		DescBuffer.MiscFlags |= D3D11_RESOURCE_MISC_SHARED_NTHANDLE;
	}
	if( SR_KEYED_MUTEX == m_SyncMethod )
	{
		DescBuffer.MiscFlags |= D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
	}
	else
	{
		DescBuffer.MiscFlags |= D3D11_RESOURCE_MISC_SHARED;
	}
	assert( !( (DescBuffer.MiscFlags & D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX) && 
			   (!(DescBuffer.MiscFlags & D3D11_RESOURCE_MISC_SHARED_NTHANDLE))
			 )
		  );

	hr = m_pDevice->CreateBuffer( &DescBuffer, NULL, &m_pTilePool );
	if( FAILED(hr) )
	{
		WriteToLog( _T( "ShareTilePoolTest::SetupTestCase - Creating Tile Pool failed. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		goto func_end;
	}

	DescBuffer.ByteWidth = g_NumTiles*D3D11_2_TILED_RESOURCE_TILE_SIZE_IN_BYTES;
	DescBuffer.Usage = D3D11_USAGE_STAGING;
	DescBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	DescBuffer.MiscFlags = 0;
	hr = m_pDevice->CreateBuffer( &DescBuffer, NULL, &m_pStagingBuffer );
	if( FAILED(hr) )
	{
		WriteToLog( _T( "ShareTilePoolTest::SetupTestCase - Creating staging buffer failed. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		goto func_end;
	}

	D3D11_TEXTURE2D_DESC tex2DDesc;
	tex2DDesc.ArraySize			= 1;
	tex2DDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	tex2DDesc.CPUAccessFlags	= 0;
	tex2DDesc.Format			= DXGI_FORMAT_R32_UINT;
	tex2DDesc.Height			= 1024;
	tex2DDesc.Width				= 1024;
	tex2DDesc.MipLevels			= 1;
	tex2DDesc.MiscFlags			= D3D11_RESOURCE_MISC_TILED;
	tex2DDesc.SampleDesc.Count	= 1;
	tex2DDesc.SampleDesc.Quality= 0;
	tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
	hr = m_pDevice->CreateTexture2D( &tex2DDesc, NULL, &m_pTiledTexture );
	if( FAILED(hr) )
	{
		WriteToLog( _T( "ShareTilePoolTest::SetupTestCase - Creating tiled texture failed. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		goto func_end;
	}
 
	hr = m_pTilePool->QueryInterface(__uuidof(IDXGIResource1), (void**)&pDXGITilePool);
	if( FAILED(hr) )
	{
		WriteToLog( _T( "ShareTilePoolTest::SetupTestCase - QI for DXGI resource1 failed. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		goto func_end;
	}

	if ( m_bMiscSharedNTHandle )
	{
		if (FAILED (hr = pDXGITilePool->CreateSharedHandle(
			NULL, 
			DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE, 
			HANDLE_NAME_VALID, 
			&m_SharedHandle)))
		{
			WriteToLog( _T( "ShareTilePoolTest::SetupTestCase) - CreateSharedHandle() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			goto func_end;
		}
	}
	else
	{
		if (FAILED (hr = pDXGITilePool->GetSharedHandle(&m_SharedHandle)))
		{
			WriteToLog( _T( "ShareTilePoolTest::SetupTestCase - IDXGIResource::GetSharedHandle() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			goto func_end;
		}
	}

	if( nullptr == m_SharedHandle )
	{
		WriteToLog( _T( "ShareTilePoolTest::SetupTestCase - Shared handle is NULL. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		hr = E_FAIL;
		goto func_end;
	}

	// the following events are used to sync the two processes at sync points other than shared resource access
	tr = CreateAndVerifyEvent(m_exitChildProcessEvent, _T("ExitChildProcess"));
	if (tr != RESULT_PASS)
		goto func_end;

	tr = CreateAndVerifyEvent(m_finishUpdateSharedMemEvent, _T("FinishUpdateSharedMem"));
	if (tr != RESULT_PASS)
		goto func_end;

	tr = CreateAndVerifyEvent(m_endChildCaseWithError, _T("EndChildCaseWithError"));
	if (tr != RESULT_PASS)
		goto func_end;
	
	
	// the following two events are used to sync the shared resource access of the two processes
	// they are alternatives of KeyedMutex
	tr = CreateAndVerifyEvent(m_startChildResAccessEvent, _T("StartChildResAccess"));
	if (tr != RESULT_PASS)
		goto func_end;

	tr = CreateAndVerifyEvent(m_endChildResAccessEvent, _T("EndChildResAccess"));
	if (tr != RESULT_PASS)
		goto func_end;

	// create shared memory
	m_hFileMap = ::CreateFileMapping(INVALID_HANDLE_VALUE , 
										 NULL, 
										 PAGE_READWRITE, 
										 0,
										 sizeof(TILE_POOL_SHARED_MEMORY),
										 _T("SharedTilePoolMem") );
	   
	if(NULL == m_hFileMap)
	{
		WriteToLog( _T( "ShareTilePoolTest::Setup() - CreateFileMapping() failed. (%d)" ), GetLastError());
		tr = RESULT_SKIP;
		goto func_end;
	}

	//map shared memory
	m_sharedMemory = (TILE_POOL_SHARED_MEMORY*)::MapViewOfFile(m_hFileMap,
														FILE_MAP_WRITE,
														0,
														0,
														sizeof(TILE_POOL_SHARED_MEMORY));
	if (NULL == m_sharedMemory)
	{
		WriteToLog( _T( "ShareTilePoolTest::SetupTestCase() - MapViewOfFile() failed. (%d)" ), GetLastError());
		tr = RESULT_SKIP;
		goto func_end;
	}
	// info for child proc
	m_sharedMemory->FeatureLevel = fl;
	m_sharedMemory->DriverType = dt;
	m_sharedMemory->SharedHandle = m_SharedHandle;
	m_sharedMemory->SyncMethod = m_SyncMethod;
	m_sharedMemory->isNTHandle = m_bMiscSharedNTHandle;

	//tell child process to read shared memory
	if (! SetEvent(m_finishUpdateSharedMemEvent) ) 
	{ 
		WriteToLog( _T( "ShareTilePoolTest::SetupTestCase() - Failed to signal child process to start reading shared memory. (%d)"),
			GetLastError());
		tr = RESULT_SKIP;
		goto func_end;
	}

	//create the child process
	STARTUPINFO si;

	char szExe[MAX_PATH];
	memset(szExe, 0, sizeof(char) * MAX_PATH);

	_snprintf_s(szExe, MAX_PATH, MAX_PATH-1, _T("%s -nolog -tilepoolchild "), GetCommandLine()); //pass on current command line args

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	if( false == CreateProcess(NULL, 
							szExe, 
							NULL, 
							NULL, 
							FALSE, 
							0, 
							NULL, 
							NULL, 
							&si, 
							&m_ProcInf ) ) 
	{

		WriteToLog(0, _T("ShareTilePoolTest::ExecuteTestCase() - CreateProcess failed.") );
		tr = RESULT_SKIP;
		goto func_end;
	}

func_end:
	SAFE_RELEASE(pDXGITilePool);
	SAFE_RELEASE(pDXGI);
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pDevice);
	if( FAILED(hr) ) tr = RESULT_FAIL;
	return tr;
}

TEST_RESULT ShareTilePoolTest::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;

	IDXGIKeyedMutex *pDXGIKeyedMutex = NULL;
	bool waitChildFail = false;

	if ( m_SyncMethod == SR_KEYED_MUTEX)
	{
		hr = m_pTilePool->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)( &pDXGIKeyedMutex ));
		if ( hr != S_OK )
		{
			WriteToLog( _T( "ShareTilePoolTest::ExecuteTestCase() - QIKeyedMutex() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto testDone;
		}

		// Acquire a lock to the resource.
		hr = pDXGIKeyedMutex->AcquireSync(0, 10000);
		if ( hr != S_OK)
		{
			WriteToLog( _T( "ShareTilePoolTest::ExecuteTestCase() - AcquireSync() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto testDone;
		}
	}

	// Test tile pool sharing
	tr = VerifySharedTilePool(0/*pass 0*/);
	if( tr != RESULT_PASS )
	{
		WriteToLog( _T( "ShareTilePoolTest::VerifySharedTilePool() - Tile Pool did not share correctly."));
		tr = RESULT_FAIL;
		goto testDone;
	}

	if ( m_SyncMethod == SR_KEYED_MUTEX)
	{
		// Release the lock to the child process.
		hr = pDXGIKeyedMutex->ReleaseSync(1);
		if ( hr != S_OK)
		{
			WriteToLog( _T( "ShareTilePoolTest::ExecuteTestCase() - ReleaseSync() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto testDone;
		}


		// Acquiring the lock to the resource from the child process
		hr = pDXGIKeyedMutex->AcquireSync(0, 8000);
		if (hr != S_OK)
		{
			WriteToLog( _T( "ShareTilePoolTest::ExecuteTestCase() - AcquireSync() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
			waitChildFail = true;
		}
	}
	else
	{
		//tell child process to begin test
		if (! SetEvent(m_startChildResAccessEvent) ) 
		{ 
			WriteToLog( _T( "ShareTilePoolTest::ExecuteTestCase() - Failed to signal child process to start."));
			tr = RESULT_SKIP;
			goto testDone;
		}

		//wait for child test case to finish
		waitChildFail = true;
		
		DWORD dwResult = WaitForSingleObject(m_endChildResAccessEvent, 12000);
		if (dwResult == WAIT_OBJECT_0)
		{
			waitChildFail = false;
		}
		else // Failed
		{		
			switch (dwResult)
			{
				case WAIT_TIMEOUT:
					WriteToLog( _T( "ShareTilePoolTest::ExecuteTestCase() - WaitForSingleObject() returned WAIT_TIMEOUT."));
					break;

				case WAIT_ABANDONED:
					WriteToLog( _T( "ShareTilePoolTest::ExecuteTestCase() - WaitForSingleObject() returned WAIT_ABANDONED."));
					break;

				case WAIT_FAILED:
					WriteToLog( _T( "ShareTilePoolTest::ExecuteTestCase() - WaitForSingleObject() returned WAIT_FAILED with error code %d"), GetLastError());
					break;
			}
			WriteToLog( _T( "ShareTilePoolTest::ExecuteTestCase() - Failed to recieve end test signal from child."));
		}
	}


	// child test case may fail before it gets to access the shared resource
	if ( waitChildFail )
	{
		if ( WaitForSingleObject(m_endChildCaseWithError, 10) != WAIT_OBJECT_0 ) 
		{ 
			WriteToLog( _T( "ShareTilePoolTest::ExecuteTestCase - Failed to recieve endChildCaseWithError signal from child. (%d)"),
				GetLastError());
			if ( WaitForSingleObject(m_ProcInf.hProcess, 10) != WAIT_OBJECT_0 )
			{
				WriteToLog( _T( "ShareTilePoolTest::ExecuteTestCase - Failed to recieve end process signal from child. (%d)"),
				GetLastError());
			}
			else
			{
				WriteToLog( _T( "ShareTilePoolTest::ExecuteTestCase - Child Process exits unexpectedly."));
			}
			// abort test group if the main process can't even get m_endChildCaseWithError signal
			WriteToLog( _T( "ShareTilePoolTest::ExecuteTestCase() - Failure in child process. %s hr = %s" ), m_sharedMemory->ResultString, D3DHResultToString(m_sharedMemory->Result).c_str());
			tr = RESULT_ABORT;
			goto testDone;
		}
	}
	else
	{
		// Test tile pool sharing
		tr = VerifySharedTilePool(1/*pass 1*/);
		if( tr != RESULT_PASS )
		{
			WriteToLog( _T( "ShareTilePoolTest::VerifySharedTilePool() - Tile Pool did not share correctly."));
			tr = RESULT_FAIL;
			goto testDone;
		}
	}

testDone:
	SAFE_RELEASE(pDXGIKeyedMutex);
	if( FAILED(hr) ) tr = RESULT_FAIL;
	return tr;
}

TEST_RESULT ShareTilePoolTest::VerifySharedTilePool(UINT pass)
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;

	D3D11_TILED_RESOURCE_COORDINATE TRC;
	D3D11_TILE_REGION_SIZE TRS;
	TRC.X = 1;
	TRC.Y = 1;
	TRC.Z = 0;
	TRC.Subresource = 0;
	TRS.NumTiles = g_NumTiles;
	TRS.bUseBox = TRUE;
	TRS.Width = 5;
	TRS.Height = 5;
	TRS.Depth = 1;
	if( 0 == pass )
	{
		// Put some data into the tile pool
		const UINT NumRanges = 1;
		UINT TilePoolRangeStartOffsets[NumRanges] = {0};
		UINT RangeTileCounts[NumRanges] = {25};
		hr = m_pContext->UpdateTileMappings(m_pTiledTexture,1,&TRC,&TRS,m_pTilePool,NumRanges,NULL,TilePoolRangeStartOffsets,RangeTileCounts,0);
		if( FAILED(hr) )
		{
			WriteToLog( _T( "ShareTilePoolTest::VerifySharedTilePool() - UpdateTileMappings() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto testDone;
		}
		D3D11_MAPPED_SUBRESOURCE mapped;
		hr = m_pContext->Map(m_pStagingBuffer,0,D3D11_MAP_WRITE,0,&mapped);
		if( FAILED(hr) )
		{
			WriteToLog( _T( "ShareTilePoolTest::VerifySharedTilePool() - Map() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto testDone;
		}
		UINT* pData = (UINT*)mapped.pData;
		for(UINT i = 0; i < g_NumTiles*D3D11_2_TILED_RESOURCE_TILE_SIZE_IN_BYTES/sizeof(UINT); i++)
		{
			pData[i] = i;
		}
		m_pContext->Unmap(m_pStagingBuffer,0);
		m_pContext->CopyTiles(m_pTiledTexture,&TRC,&TRS,m_pStagingBuffer,0,D3D11_TILE_COPY_LINEAR_BUFFER_TO_SWIZZLED_TILED_RESOURCE);

		// Force GPU to complete all pending operations before leaving
		m_pContext->Flush();
		if(!BlockingFinish())
		{
			tr = RESULT_FAIL;
			goto testDone;
		}
	}
	else
	{
		// Examine what the child process wrote back to us in the tile pool
		m_pContext->CopyTiles(m_pTiledTexture,&TRC,&TRS,m_pStagingBuffer,0,D3D11_TILE_COPY_SWIZZLED_TILED_RESOURCE_TO_LINEAR_BUFFER);
		D3D11_MAPPED_SUBRESOURCE mapped;
		hr = m_pContext->Map(m_pStagingBuffer,0,D3D11_MAP_READ,0,&mapped);
		if( FAILED(hr) )
		{
			WriteToLog( _T( "ShareTilePoolTest::VerifySharedTilePool() - Map() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto testDone;
		}
		UINT* pData = (UINT*)mapped.pData;
		for(UINT i = 0; i < g_NumTiles*D3D11_2_TILED_RESOURCE_TILE_SIZE_IN_BYTES/sizeof(UINT); i++)
		{
			UINT expected = i*2;// child wrote i*2
			if( pData[i] != expected ) 
			{
				WriteToLog( _T( "ShareTilePoolTest::VerifySharedTilePool() - Data written by child process not correct. Expected %u at UINT[%u] but saw %u instead."), 
					expected,i,pData[i]);
				m_pContext->Unmap(m_pStagingBuffer,0);
				tr = RESULT_FAIL;
				goto testDone;
			}
		}
		m_pContext->Unmap(m_pStagingBuffer,0);
	}
testDone:
	return tr;
}


void ShareTilePoolTest::CleanupTestCase()
{
	if (!m_isChildProcess)
	{
		//tell child process to end
		if ( m_exitChildProcessEvent && ! SetEvent(m_exitChildProcessEvent) ) 
		{ 
			WriteToLog( _T( "ShareTilePoolTest::Cleanup() - Failed to signal child process to exit."));
		}

		if (m_ProcInf.hProcess)
		{
			//end the child process
			if (WAIT_OBJECT_0 != WaitForSingleObject( m_ProcInf.hProcess, 5000 )) //wait for a bit
			{
				WriteToLog(_T("ShareTilePoolTest::Cleanup() - Child process didn't exit in 5 second after being signaled. \
							  It'll be terminated by the main process." ));
				TerminateProcess(m_ProcInf.hProcess, 0); //kill it
				// wait for child to actually exit to make sure all handles opened by child are closed
				// since TerminateProcess returns immediately without waiting for the process to actually exit.
				if (WAIT_OBJECT_0 != WaitForSingleObject( m_ProcInf.hProcess, 10000 ))
					WriteToLog(_T("ShareTilePoolTest::Cleanup() - Failed to terminate Child process in 10 seconds." ));
			}

			//close process
			CloseHandle(m_ProcInf.hProcess);
			CloseHandle(m_ProcInf.hThread);
			ZeroMemory( &m_ProcInf, sizeof(m_ProcInf) );
		}
	}
	
	if (m_hFileMap)
		CloseHandle(m_hFileMap);
	if (m_startChildResAccessEvent)
		CloseHandle(m_startChildResAccessEvent);
	if (m_endChildResAccessEvent)
		CloseHandle(m_endChildResAccessEvent);
	if (m_exitChildProcessEvent)
		CloseHandle(m_exitChildProcessEvent);
	if (m_finishUpdateSharedMemEvent)
		CloseHandle(m_finishUpdateSharedMemEvent);
	if (m_endChildCaseWithError)
		CloseHandle(m_endChildCaseWithError);
	
	m_hFileMap = NULL;
	m_startChildResAccessEvent = NULL;
	m_endChildResAccessEvent = NULL;
	m_exitChildProcessEvent = NULL;
	m_finishUpdateSharedMemEvent = NULL;
	m_endChildCaseWithError = NULL;

	CleanupD3DObjects();
}

//-----------------------------------------------------------------------------
void ShareTilePoolTest::CleanupD3DObjects()
{
	SAFE_RELEASE(m_pStagingBuffer);
	SAFE_RELEASE(m_pTiledTexture);
	SAFE_RELEASE(m_pTilePool);
	if( m_SharedHandle )
	{
		if(!m_isChildProcess && m_bMiscSharedNTHandle)
		{
			CloseHandle( m_SharedHandle );
		}
		m_SharedHandle = nullptr;
	}
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pDevice);
}

//-----------------------------------------------------------------------------
TEST_RESULT ShareTilePoolTest::CreateAndVerifyEvent(HANDLE &hEvent, tstring EventName)
{
	SECURITY_ATTRIBUTES sec;
	sec.nLength = sizeof(SECURITY_ATTRIBUTES);
	sec.bInheritHandle= TRUE;
	sec.lpSecurityDescriptor = NULL;

	hEvent = CreateEvent( 
		&sec,         // default security attributes
		FALSE,         // auto-reset event
		FALSE,         // initial state is NOT signaled
		EventName.c_str()  // object name
	);	
	if (hEvent == NULL) 
	{ 
		WriteToLog(_T( "ShareTilePoolTest::CreateAndVerifyEvent - CreateEvent failed (%d) for %s."),
			GetLastError(), EventName.c_str());
		return RESULT_SKIP;
	}
	return RESULT_PASS;
}
