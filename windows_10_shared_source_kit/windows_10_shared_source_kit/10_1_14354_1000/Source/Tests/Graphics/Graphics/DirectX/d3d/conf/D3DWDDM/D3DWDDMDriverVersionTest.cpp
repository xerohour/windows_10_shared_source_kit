// Includes
#include "D3DWDDMDriverVersionTest.h"

// Namespaces
using namespace WindowsTest;
using namespace WindowsTest::Graphics::Direct3D;

// Create the strings for parameter names
const tstring g_featureLevelStr = "FeatureLevel";

////////////////////////

CD3DWDDMDriverVersionTest::CD3DWDDMDriverVersionTest(void)
	: m_pDevice(nullptr)
	, m_pDeviceContext(nullptr)
	, m_pDevTest(nullptr)
	, m_pAdapterDescriptor( nullptr )
{
}

void CD3DWDDMDriverVersionTest::InitTestParameters()
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

TEST_RESULT CD3DWDDMDriverVersionTest::Setup()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = E_FAIL;
	ATL::CComPtr<IDXGIAdapter1> pAdapter1 = nullptr;

	//
	// Get the AdapterDescriptor
	//

	assert( m_pAdapterDescriptor == nullptr );
	if( FAILED( hr = GetFramework()->GetDXGIAdapter()->QueryInterface( __uuidof(IDXGIAdapter1), (void**)&pAdapter1 ) ) )
	{
		LogError( __FILEW__, __LINE__, L"QueryInterface for IDXGIAdapter1 failed.", hr, ERRORCODE_TYPE_HRESULT, gFCIUnknownQueryInterface );
		tr = RESULT_FAIL;
	}

	m_pAdapterDescriptor = DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor( pAdapter1 );
	if( !m_pAdapterDescriptor )
	{
		LogError( __FILEW__, __LINE__, L"DevX::GRFX::AdapterDescriptor::GetAdapterDescriptor failed.", false, ERRORCODE_TYPE_BOOL, gFCRequiredCapabilityNotSupported );
		tr = RESULT_FAIL;
	}

	//
	// Get the display driver model
	//

	m_pAdapterDescriptor->GetRenderDriverModel( m_kmtDriverVersion );
	switch( m_kmtDriverVersion )
	{
	default:
		{
			WriteToLog( _T("Unknown KMT Driver Version is reported as %u."), m_kmtDriverVersion );
			LogError( __FILEW__, __LINE__, L"Unknown KMT Driver Version reported.", false, ERRORCODE_TYPE_BOOL, gFCRequiredCapabilityNotSupported );
			tr = RESULT_FAIL;
			break;
		}
	case KMT_DRIVERVERSION_WDDM_2_1:
	{
		WriteToLog(_T("KMT Driver Version is reported as WDDM 2.1"));
		break;
	}
	case KMT_DRIVERVERSION_WDDM_2_0:
		{
			WriteToLog(_T("KMT Driver Version is reported as WDDM 2.0"));
			break;
		}
	case KMT_DRIVERVERSION_WDDM_1_3:
		{
			WriteToLog( _T("KMT Driver Version is reported as WDDM 1.3") );
			break;
		}
	case KMT_DRIVERVERSION_WDDM_1_2:
		{
			WriteToLog( _T("KMT Driver Version is reported as WDDM 1.2") );
			break;
		}
	case KMT_DRIVERVERSION_WDDM_1_1:
		{
			WriteToLog( _T("KMT Driver Version is reported as WDDM 1.1") );
			break;
		}
	case KMT_DRIVERVERSION_WDDM_1_0:
		{
			WriteToLog( _T("KMT Driver Version is reported as WDDM 1.0") );
			break;
		}
	}

	return tr;
}

TEST_RESULT CD3DWDDMDriverVersionTest::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = E_FAIL;
	D3D_FEATURE_LEVEL outputFeatureLevel;
	ID3D11Device2 *pD3DDevice2 = nullptr;

	//
	// Create the device
	//

	// The device MUST be able to create a feature level 9.X device. However, we have other tests (MiscDevice) that already check this requirement.
	// This call may fail if the hardware doesn't support the specified feature level
	WriteToLog( _T("Creating a D3D device with feature level %s:"), ToString(m_featureLevel).c_str() );
	if( FAILED( hr = D3D11CreateDevice( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &m_featureLevel, 1, D3D11_SDK_VERSION, &m_pDevice, &outputFeatureLevel, &m_pDeviceContext ) ) )
	{
		WriteToLog( _T("D3D11CreateDevice failed to create a device with feature level %s. This may be expected if the hardware doesn't support this feature level. HR = %s."), ToString(m_featureLevel).c_str(), D3DHResultToString(hr).c_str() );
		tr = RESULT_SKIP;
	}
	else
	{
		WriteToLog( _T("D3D11CreateDevice created a feature level %s device."), ToString(outputFeatureLevel).c_str() );
	}

	// QI for ID3D11DeviceTest and call GetDriverDDIVersion on it. However, the GetDriverDDIVersion method is not available on Win8 or prior, so we need to check our runtime version before attempting to call this. This is done by checking to see if we can QI for ID3D11Device2.
	m_pDevTest = nullptr;
	if(m_pDevice)
	{
		if( SUCCEEDED( m_pDevice->QueryInterface(__uuidof( ID3D11Device2 ), (void **) &pD3DDevice2) ) )
		{
			if( FAILED( m_pDevice->QueryInterface(__uuidof( ID3D11DeviceTest ), (void**)&m_pDevTest) ) )
			{
				WriteToLog( _T( "QueryInterface for ID3D11DeviceTest interface pointer failed." ) );
				tr = RESULT_FAIL;
			}
		}
	}

	return tr;
}

TEST_RESULT CD3DWDDMDriverVersionTest::ExecuteTestCase()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;
	const bool bWin8DDIsAreExpected = (m_kmtDriverVersion >= KMT_DRIVERVERSION_WDDM_1_2 );
	const bool bWin8DDIsAreSupported = GetFramework()->Is11_1DDISupported( m_pDevice );
	D3D_DRIVER_DDI_VERSION driverDDIVersion;
	D3D_DRIVER_DDI_VERSION expectedDDIVersion;

	WriteToLog( _T("The test expects the driver version %s support Windows 8 D3D9/D3D11.1 DDIs"), bWin8DDIsAreExpected ? "should" : "should NOT" );
	WriteToLog( _T("The driver %s support Windows 8 D3D9/D3D11.1 DDIs"), bWin8DDIsAreSupported ? "does" : "does NOT" );
	if( bWin8DDIsAreExpected )
	{
		// The driver needs to support the new Win8 D3D9 and/or D3D11.1 DDIs
		if( !bWin8DDIsAreSupported )
		{
			LogError( __FILEW__, __LINE__, L"Windows 8 DDIs are NOT supported when they should be.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			tr = RESULT_FAIL;
		}
	}
	else
	{
		// The driver should not support the new Win8 D3D9 and/or D3D11.1 DDIs
		if( bWin8DDIsAreSupported )
		{
			LogError( __FILEW__, __LINE__, L"Windows 8 DDIs are supported when they should NOT be.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			tr = RESULT_FAIL;
		}
	}

	// Perform additional check against the DDI version.
	if(m_pDevTest)
	{
		driverDDIVersion = m_pDevTest->GetDriverDDIVersion();

		// If you hit this assert, you also update switch for KMT driver version in Setup().
		static_assert(D3D_DRIVER_DDI_VERSION_LATEST == D3D_DRIVER_DDI_VERSION_2_1, "This test must be kept in sync with latest drivers.");

		switch( m_kmtDriverVersion )
		{
		default:
			{
				// we shouldn't ever hit this as this should be validated in Setup()
				LogError( __FILEW__, __LINE__, L"Unknown KMT Driver Version reported.", false, ERRORCODE_TYPE_BOOL, gFCRequiredCapabilityNotSupported );
				return RESULT_FAIL;
			}
		case KMT_DRIVERVERSION_WDDM_2_1:
			{
				expectedDDIVersion = D3D_DRIVER_DDI_VERSION_2_1;
				break;
			}
		case KMT_DRIVERVERSION_WDDM_2_0:
			{
				expectedDDIVersion = D3D_DRIVER_DDI_VERSION_2_0;
				break;
			}
		case KMT_DRIVERVERSION_WDDM_1_3:
			{
				expectedDDIVersion = D3D_DRIVER_DDI_VERSION_1_3;
				break;
			}
		case KMT_DRIVERVERSION_WDDM_1_2:
			{
				expectedDDIVersion = D3D_DRIVER_DDI_VERSION_1_2;
				break;
			}
		case KMT_DRIVERVERSION_WDDM_1_1:
			{
				expectedDDIVersion = D3D_DRIVER_DDI_VERSION_1_1;
				break;
			}
		case KMT_DRIVERVERSION_WDDM_1_1_PRERELEASE:
			{
				expectedDDIVersion = D3D_DRIVER_DDI_VERSION_1_1;
				break;
			}
		case KMT_DRIVERVERSION_WDDM_1_0:
			{
				expectedDDIVersion = D3D_DRIVER_DDI_VERSION_1_0;
				break;
			}
		}

		if(driverDDIVersion != expectedDDIVersion)
		{
			WriteToLog( _T("Expected the GetSupportedVersions DDI call to return a value matching %s, but it returned %s as the highest value."), ToString(expectedDDIVersion).c_str(), ToString(driverDDIVersion).c_str() );
			LogError( __FILEW__, __LINE__, L"Expected driver DDI version does not match actual DDI version.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			tr = RESULT_FAIL;
		}
		else
		{
			WriteToLog ( _T("Expected driver DDI version matches actual DDI version.") );
		}
	}

	return tr;
}

void CD3DWDDMDriverVersionTest::CleanupTestCase()
{
	m_pDevTest.Release();
	m_pDeviceContext.Release();
	m_pDevice.Release();
}

void CD3DWDDMDriverVersionTest::Cleanup()
{
	if( m_pAdapterDescriptor )
	{
		delete m_pAdapterDescriptor;
		m_pAdapterDescriptor = nullptr;
	}
}