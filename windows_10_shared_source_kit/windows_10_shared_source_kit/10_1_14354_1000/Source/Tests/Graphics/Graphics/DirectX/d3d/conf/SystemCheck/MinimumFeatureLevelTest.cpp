// Includes
#include "MinimumFeatureLevelTest.h"

// Extern
extern CSystemCheckApp g_testApp;

// Namespaces
using namespace WindowsTest;
using namespace WindowsTest::Graphics::Direct3D;
using namespace WindowsTest::Graphics::Direct3D9;
using namespace WindowsTest::Graphics::DXGI;

////////////////////////

CMinimumFeatureLevelTest::CMinimumFeatureLevelTest(void)
{
}

/////

CMinimumFeatureLevelTest::~CMinimumFeatureLevelTest(void)
{
}

/////

void CMinimumFeatureLevelTest::InitTestParameters()
{
	// I'm not setting a root test parameter because I want a single test case.
}

/////

TEST_RESULT CMinimumFeatureLevelTest::ExecuteTestCase()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = E_FAIL;
	const D3D_FEATURE_LEVEL minimumFeatureLevel = g_testApp.GetMinimumFeatureLevel();	// Default feature level is FL9.1

	//
	// Determine what feature level to check against
	// 

	BeginSection( L"Determine what feature level to check against" );
	WriteToLog( _T("The test will ensure each enumerated adapter supports at least feature level %s."), ToString(minimumFeatureLevel).c_str() );
	EndSection();

	//
	// Create a device on each adapter
	//

	BeginSection( L"Create a device on each adapter" );
	UINT numEnumeratedAdapters= 0;
	for(UINT adapterIndex = 0; true; adapterIndex++)
	{
		ATL::CComPtr<IDXGIAdapter1> pAdapter1 = nullptr;
		ATL::CComPtr<ID3D11Device> pDevice = nullptr;
		ATL::CComPtr<ID3D11DeviceContext> pDeviceContext = nullptr;
		D3D_FEATURE_LEVEL outputFeatureLevel = D3D_FEATURE_LEVEL_9_1;
		if( FAILED( hr = GetFramework()->GetDXGIFactory1()->EnumAdapters1( adapterIndex, &pAdapter1 ) ) )
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

		// Increment our enumerated adapter count
		numEnumeratedAdapters++;

		DXGI_ADAPTER_DESC1 adapterDesc;
		if( FAILED( hr = pAdapter1->GetDesc1( &adapterDesc ) ) )
		{
			WriteToLog( _T("GetDesc1() failed for adapter #%d."), adapterIndex );
			LogError( __FILEW__, __LINE__, L"GetDesc1 failed.", hr, ERRORCODE_TYPE_HRESULT, gFCRequiredCapabilityNotSupported );
			tr = RESULT_FAIL;
		}

		// Log some information about the adapter
		LARGE_INTEGER userModeDriverVersion;
		hr = pAdapter1->CheckInterfaceSupport( __uuidof( IDXGIDevice ), &userModeDriverVersion );
		tstring userModeDriverVersionStr = _T("");
		userModeDriverVersionStr += ToString( HIWORD( userModeDriverVersion.HighPart ) ) + _T(".");	// Product
		userModeDriverVersionStr += ToString( LOWORD( userModeDriverVersion.HighPart ) ) + _T(".");	// Version
		userModeDriverVersionStr += ToString( HIWORD( userModeDriverVersion.LowPart ) ) + _T(".");	// Sub-Version
		userModeDriverVersionStr += ToString( LOWORD( userModeDriverVersion.LowPart ) );			// Build
		WriteToLog( _T("Adapter #%d: %ls (Driver Version: %s)"), adapterIndex, 
			adapterDesc.Description,
			userModeDriverVersionStr.c_str() );

		// Create device
		hr = D3D11CreateDevice( pAdapter1, D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &pDevice, &outputFeatureLevel, &pDeviceContext );
		if( FAILED( hr ) )
		{
			LogError( __FILEW__, __LINE__, L"D3D11CreateDevice failed.", hr, ERRORCODE_TYPE_HRESULT, gFCRequiredCapabilityNotSupported );
			tr = RESULT_FAIL;
		}
		else
		{
			WriteToLog( _T("Device successfully created on adapter #%d with feature level %s."), adapterIndex, ToString(outputFeatureLevel).c_str() );
			if( outputFeatureLevel < minimumFeatureLevel )
			{
				WriteToLog( _T("Feature level %s is below the minimum required feature level (%s)."), ToString(outputFeatureLevel).c_str(), ToString(minimumFeatureLevel).c_str() );
				LogError( __FILEW__, __LINE__, L"This adapter doesn't support the minimum required feature level.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
				tr = RESULT_FAIL;
			}
		}
	}

	// Make sure we found at least one adapter
	if( numEnumeratedAdapters == 0 )
	{
		LogError( __FILEW__, __LINE__, L"No adapters were enumerated.", false, ERRORCODE_TYPE_BOOL, gFCRequiredCapabilityNotSupported );
		tr = RESULT_FAIL;
	}

	return tr;
}

