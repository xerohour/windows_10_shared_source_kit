// Includes
#include "D3D9DriverRuntimeVersionTest.h"

// Namespaces
using namespace WindowsTest;
using namespace WindowsTest::Graphics::Direct3D;

// Call Types
BEGIN_NAMED_VALUES( TEST_DDI_VERSION )
	NAMED_VALUE( _T( "D3D_UMD_INTERFACE_VERSION_WDDM2_0" ), TEST_WDDM_2_0)
    NAMED_VALUE( _T( "D3D_UMD_INTERFACE_VERSION_WDDM1_3" ), TEST_WDDM_1_3 )
    NAMED_VALUE( _T( "D3D_UMD_INTERFACE_VERSION_WIN8" ), TEST_WIN8 )
	NAMED_VALUE( _T( "D3D_UMD_INTERFACE_VERSION_WIN7" ), TEST_WIN7 )
	NAMED_VALUE( _T( "INVALID DATA" ), TEST_INVALID )
END_NAMED_VALUES( TEST_DDI_VERSION )

////////////////////////

CD3D9DriverRuntimeVersionTest::CD3D9DriverRuntimeVersionTest(void)
	: m_pAdapterDescriptor( nullptr )
	, m_RuntimeVersion( TEST_INVALID )
	, m_OSCorrespondingWDDMVersion( TEST_INVALID )
	, m_kmtDriverVersion( KMT_DRIVERVERSION_WDDM_1_3 )
{
}

void CD3D9DriverRuntimeVersionTest::InitTestParameters()
{
	CUserValueSet<TEST_DDI_VERSION> *pRuntimeVersion = new CUserValueSet<TEST_DDI_VERSION>();
	pRuntimeVersion->AddValue( TEST_WDDM_2_0 );
	pRuntimeVersion->AddValue( TEST_WDDM_1_3 );
	pRuntimeVersion->AddValue( TEST_WIN8 );
	pRuntimeVersion->AddValue( TEST_WIN7 );
	pRuntimeVersion->AddValue( TEST_INVALID );
	testfactor::RFactor m_rfRuntimeVersion = AddParameter<TEST_DDI_VERSION>( _T( "RuntimeVersion" ), &m_RuntimeVersion, pRuntimeVersion );

	SetRootTestFactor( m_rfRuntimeVersion );
}

TEST_RESULT CD3D9DriverRuntimeVersionTest::Setup()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = E_FAIL;
	ATL::CComPtr<IDXGIAdapter1> pAdapter1 = nullptr;

	if( GetFramework()->IsMobile() )
	{
		WriteToLog( _T("Not applicable on mobile, skipping.") );
		tr = RESULT_SKIP;
		return tr;
	}

	//
	// Get the AdapterDescriptor
	//

	assert( m_pAdapterDescriptor == nullptr );
	if( FAILED( hr = GetFramework()->GetDXGIAdapter()->QueryInterface( __uuidof(IDXGIAdapter1), (void**)&pAdapter1 ) ) )
	{
		LogError( __FILEW__, __LINE__, L"QueryInterface for IDXGIAdapter1 failed.", hr, ERRORCODE_TYPE_HRESULT, gFCIUnknownQueryInterface );
		tr = RESULT_FAIL;
		return tr;
	}

	m_pAdapterDescriptor = DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor( pAdapter1 );
	if( !m_pAdapterDescriptor )
	{
		LogError( __FILEW__, __LINE__, L"DevX::GRFX::AdapterDescriptor::GetAdapterDescriptor failed.", false, ERRORCODE_TYPE_BOOL, gFCRequiredCapabilityNotSupported );
		tr = RESULT_FAIL;
		return tr;
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

	// Get the Windows version and corresponding WDDM version
	RTL_OSVERSIONINFOW windowsVersionInfo;
	windowsVersionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
	NTSTATUS getVersionSuccess = RtlGetVersion(&windowsVersionInfo);

	if (getVersionSuccess != STATUS_SUCCESS)
	{
		LogError( __FILEW__, __LINE__, L"RtlGetVersion failed", false, ERRORCODE_TYPE_BOOL, gFCRequiredCapabilityNotSupported );
		tr = RESULT_FAIL;
	}

	if (windowsVersionInfo.dwMajorVersion == 10 && windowsVersionInfo.dwMinorVersion == 0) // 10.0
	{
		m_OSCorrespondingWDDMVersion = TEST_WDDM_2_0;
	}
	else if (windowsVersionInfo.dwMajorVersion == 6 && windowsVersionInfo.dwMinorVersion == 3) // 8.1
	{
		m_OSCorrespondingWDDMVersion = TEST_WDDM_1_3;
	}
	else if (windowsVersionInfo.dwMajorVersion == 6 && windowsVersionInfo.dwMinorVersion == 2) // 8.0
	{
		m_OSCorrespondingWDDMVersion = TEST_WIN8;
	}
	else if (windowsVersionInfo.dwMajorVersion == 6 && windowsVersionInfo.dwMinorVersion == 1) // 7
	{
		m_OSCorrespondingWDDMVersion = TEST_WIN7;
	}
	else
	{
		LogError( __FILEW__, __LINE__, L"Unknown OS version, the test needs to be updated", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		tr = RESULT_FAIL;
	}

	return tr;
}

TEST_RESULT CD3D9DriverRuntimeVersionTest::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;
	return tr;
}

TEST_RESULT CD3D9DriverRuntimeVersionTest::ExecuteTestCase()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;
	bool		bExpectedSucceed;

	// Drivers may optionally support a UMD that is a greater than the corresponding OS version if their driver was built against newer headers
	if (m_RuntimeVersion != TEST_INVALID && m_RuntimeVersion > m_OSCorrespondingWDDMVersion)
	{
		WriteToLog( _T("A UMD version greater than supported on this OS is being tested. Since the driver is allowed but not required to support this, this test will be skipped.") );
		return RESULT_SKIP;
	}
	
	switch (m_RuntimeVersion)
	{
	case TEST_WDDM_2_0:
		if (m_kmtDriverVersion == KMT_DRIVERVERSION_WDDM_2_0)
		{
			bExpectedSucceed = true;
		}
		else if (m_kmtDriverVersion < KMT_DRIVERVERSION_WDDM_2_0)
		{
			bExpectedSucceed = false;
		}
		else
		{
			WriteToLog(_T("The UMD interface version being tested is older than the one corresponding to this driver's WDDM version. This may not be supported, skipping."));
			return RESULT_SKIP;
		}
		break;
	case TEST_WDDM_1_3:
		if (m_kmtDriverVersion == KMT_DRIVERVERSION_WDDM_1_3)
		{
			bExpectedSucceed = true;
		}
		else if (m_kmtDriverVersion < KMT_DRIVERVERSION_WDDM_1_3)
		{
			bExpectedSucceed = false;
		}
		else
		{
			WriteToLog( _T("The UMD interface version being tested is older than the one corresponding to this driver's WDDM version. This may not be supported, skipping.") );
			return RESULT_SKIP;
		}
		break;
	case TEST_WIN8:
		if (m_kmtDriverVersion == KMT_DRIVERVERSION_WDDM_1_2)
		{
			bExpectedSucceed = true;
		}
		else if (m_kmtDriverVersion < KMT_DRIVERVERSION_WDDM_1_2)
		{
			bExpectedSucceed = false;
		}
		else
		{
			WriteToLog( _T("The UMD interface version being tested is older than the one corresponding to this driver's WDDM version. This may not be supported, skipping.") );
			return RESULT_SKIP;
		}
		break;
	case TEST_WIN7:
		if (m_kmtDriverVersion == KMT_DRIVERVERSION_WDDM_1_1)
		{
			bExpectedSucceed = true;
		}
		else if (m_kmtDriverVersion < KMT_DRIVERVERSION_WDDM_1_1)
		{
			bExpectedSucceed = false;
		}
		else
		{
			WriteToLog( _T("The UMD interface version being tested is older than the one corresponding to this driver's WDDM version. This may not be supported, skipping.") );
			return RESULT_SKIP;
		}
		break;
	case TEST_INVALID:
		bExpectedSucceed = false;
		break;
	default:
		LogError( __FILEW__, __LINE__, L"Unknown value for parameter m_RuntimeVersion.", false, ERRORCODE_TYPE_BOOL, gFCConflictingParameters );
		return RESULT_FAIL;
	}

	if ( FAILED ( m_pAdapterDescriptor->OpenAdapterRuntimeVersion(m_RuntimeVersion) ) )
	{
		if (bExpectedSucceed)
		{
			LogError( __FILEW__, __LINE__, L"The UMD interface version was expected to be supported, but it wasn't.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			tr = RESULT_FAIL;
		}
	} 
	else
	{
		if (!bExpectedSucceed)
		{
			LogError( __FILEW__, __LINE__, L"The UMD interface version was expected to fail, but it succeeded.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			tr = RESULT_FAIL;
		}
	}

	return tr;
}

void CD3D9DriverRuntimeVersionTest::CleanupTestCase()
{
}

void CD3D9DriverRuntimeVersionTest::Cleanup()
{
	if( m_pAdapterDescriptor )
	{
		delete m_pAdapterDescriptor;
		m_pAdapterDescriptor = NULL;
	}
}