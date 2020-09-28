// Includes
#include "DiscreteListTest.h"

// Namespaces
using namespace WindowsTest;
using namespace WindowsTest::Graphics::Direct3D;
using namespace WindowsTest::Graphics::Direct3D9;
using namespace WindowsTest::Graphics::DXGI;
using namespace WindowsTest::Graphics::D3DKMT;

// Globals
const double g_maxAllowedDListQueryTimeInMS = 4.0;		// 4 milliseconds	(Hybrid DList requirement 4.5)
const double g_maxAllowedDListSizeKiloBytes = 200;		// 200 kB			(Hybrid DList requirement 4.5)

////////////////////////

CDiscreteListTest::CDiscreteListTest(void)
	: m_hDList(nullptr)
	, m_hGDI32(nullptr)
    , m_pfnCloseAdapter(nullptr)
    , m_hDiscreteAdapter(0)
	, m_bFoundHybridDiscreteAdapter(false)
	, m_hybridDiscreteAdapterIndex(0)
{
}

/////

CDiscreteListTest::~CDiscreteListTest(void)
{
}

/////

void CDiscreteListTest::InitTestParameters()
{
	// I'm not setting a root test parameter because I want a single test case.
}

/////

TEST_RESULT CDiscreteListTest::SetupTestCase()
{
	// Local variables
	HRESULT hr = E_FAIL;
	TEST_RESULT tr = RESULT_PASS;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	D3DKMT_DLIST_DRIVER_NAME DListName;
	D3DKMT_ENUMADAPTERS Adapters;
	D3DKMT_QUERYADAPTERINFO QueryAdapterInfo;
	DevX::GRFX::AdapterDescriptor *pAdapterDescriptor = nullptr;
	bool bAdaptersAreOpen = false;

	//
	// Get D3D KMT pointers
	//

	m_hGDI32 = LoadLibrary( _T("gdi32.dll") );
	if( !m_hGDI32 )
	{
		LogError( __FILEW__, __LINE__, L"LoadLibrary() failed for the gdi32.dll.", HRESULT_FROM_WIN32(GetLastError()), ERRORCODE_TYPE_HRESULT, gFCLoadLibrary );
		tr = RESULT_FAIL;
		return tr;
	}

	                             m_pfnCloseAdapter			= reinterpret_cast<PFND3DKMT_CLOSEADAPTER>			(GetProcAddress( m_hGDI32, "D3DKMTCloseAdapter" ));
	PFND3DKMT_ENUMADAPTERS         pfnEnumAdapters			= reinterpret_cast<PFND3DKMT_ENUMADAPTERS>			(GetProcAddress( m_hGDI32, "D3DKMTEnumAdapters" ));
	PFND3DKMT_QUERYADAPTERINFO     pfnQueryAdapterInfo		= reinterpret_cast<PFND3DKMT_QUERYADAPTERINFO>		(GetProcAddress( m_hGDI32, "D3DKMTQueryAdapterInfo" ));
	
	// Check pointers for D3DKMT functions
	if( !m_pfnCloseAdapter )
	{ 
		LogError( __FILEW__, __LINE__, L"GetProcAddress failed for D3DKMTCloseAdapter.", HRESULT_FROM_WIN32(GetLastError()), ERRORCODE_TYPE_HRESULT, gFCGetProcAddress );
		tr = RESULT_FAIL; 
		goto Done;
	}

	if( !pfnEnumAdapters )
	{
		LogError( __FILEW__, __LINE__, L"GetProcAddress failed for D3DKMTEnumAdapters.", HRESULT_FROM_WIN32(GetLastError()), ERRORCODE_TYPE_HRESULT, gFCGetProcAddress );
		tr = RESULT_FAIL;
		goto Done;
	}

	if( !pfnQueryAdapterInfo )
	{
		LogError( __FILEW__, __LINE__, L"GetProcAddress failed for D3DKMTQueryAdapterInfo.", HRESULT_FROM_WIN32(GetLastError()), ERRORCODE_TYPE_HRESULT, gFCGetProcAddress );
		tr = RESULT_FAIL;
		goto Done;
	}
	
	//
	// Enumerate Adapters to find the Hybrid Discrete adapter
	//

    ZeroMemory( &Adapters, sizeof(Adapters) );
	status = pfnEnumAdapters( &Adapters );

	// Check Enum Adapters' returned values
	if( !NT_SUCCESS(status) )
	{
		LogError( __FILEW__, __LINE__, L"D3DKMTEnumAdapters() failed.", status, ERRORCODE_TYPE_NTSTATUS, gFCD3DKMTEnumAdapters );
		tr = RESULT_FAIL;
		goto Done;
	}

	if( Adapters.NumAdapters == 0 )
	{
		LogError( __FILEW__, __LINE__, L"D3DKMTEnumAdapters() couldn't find any adapters.", status, ERRORCODE_TYPE_NTSTATUS, gFCD3DKMTEnumAdapters );
		tr = RESULT_FAIL;
		goto Done;
	}

	// Set the flag to indicate we need to close adapters
	bAdaptersAreOpen = true;

	
	BeginSection( L"Find the hybrid discrete adapter" );

	WriteToLog( _T("D3DKMTEnumAdapters() found %d adapters:"), Adapters.NumAdapters );

	// For each adapter...
	m_bFoundHybridDiscreteAdapter = false;
    for( UINT adapterIndex = 0; adapterIndex < Adapters.NumAdapters; ++adapterIndex )
    {
		// Get the adapter type for the current adapter
		D3DKMT_ADAPTERTYPE AdapterType;
        QueryAdapterInfo.hAdapter = Adapters.Adapters[adapterIndex].hAdapter;
        QueryAdapterInfo.Type = KMTQAITYPE_ADAPTERTYPE;
        QueryAdapterInfo.pPrivateDriverData = (void*)&AdapterType;
        QueryAdapterInfo.PrivateDriverDataSize = sizeof(AdapterType);
        status = pfnQueryAdapterInfo( &QueryAdapterInfo );
        if( !NT_SUCCESS(status) )
        {
			WriteToLog( _T("D3DKMTQueryAdapterInfo() failed for KMTQAITYPE_ADAPTERTYPE") );
			LogError( __FILEW__, __LINE__, L"D3DKMTQueryAdapterInfo() failed.", status, ERRORCODE_TYPE_NTSTATUS, gFCD3DKMTQueryAdapterInfo );
			tr = RESULT_FAIL;
			EndSection();
			goto Done;
        }

		//
		// Log some info about the current adapter
		//

		LUID currAdapterLuid = Adapters.Adapters[adapterIndex].AdapterLuid;
		pAdapterDescriptor = DevX::GRFX::AdapterDescriptor::GetAdapterDescriptor( currAdapterLuid );
		if( !pAdapterDescriptor )
		{
			WriteToLog( _T("     Adapter #%d: GetAdapterDescriptor() failed."), adapterIndex );
			LogError( __FILEW__, __LINE__, L"GetAdapterDescriptor() failed.", false, ERRORCODE_TYPE_BOOL, gFCRequiredCapabilityNotSupported );
			tr = RESULT_FAIL;
			EndSection();
			goto Done;
		}

		// Get the WDDM driver version
		D3DKMT_DRIVERVERSION kmtDriverVersion;
		hr = pAdapterDescriptor->GetDisplayDriverModel( kmtDriverVersion );
		if( FAILED(hr) )
		{
			WriteToLog( _T("     Adapter #%d: GetDisplayDriverModel() failed"), adapterIndex );
			LogError( __FILEW__, __LINE__, L"GetDisplayDriverModel() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCRequiredCapabilityNotSupported );
			tr = RESULT_FAIL;
			EndSection();
			goto Done;
		}

		// Get the driver version
		LARGE_INTEGER userModeDriverVersion;
		const DXGI_ADAPTER_DESC1 adapterDesc = pAdapterDescriptor->GetDxgiAdapterDesc1();
		hr = pAdapterDescriptor->GetIDXGIAdapter1()->CheckInterfaceSupport( __uuidof( IDXGIDevice ), &userModeDriverVersion );
		if( FAILED(hr) )
		{
			WriteToLog( _T("     Adapter #%d: CheckInterfaceSupport() failed"), adapterIndex );
			LogError( __FILEW__, __LINE__, L"CheckInterfaceSupport() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCAdapterCheckInterfaceSupport );
			tr = RESULT_FAIL;
			EndSection();
			goto Done;
		}
		tstring userModeDriverVersionStr = _T("");
		userModeDriverVersionStr += ToString( HIWORD( userModeDriverVersion.HighPart ) ) + _T(".");	// Product
		userModeDriverVersionStr += ToString( LOWORD( userModeDriverVersion.HighPart ) ) + _T(".");	// Version
		userModeDriverVersionStr += ToString( HIWORD( userModeDriverVersion.LowPart ) ) + _T(".");	// Sub-Version
		userModeDriverVersionStr += ToString( LOWORD( userModeDriverVersion.LowPart ) );			// Build
		WriteToLog( _T("     Adapter #%d: %ls (KMT WDDM Driver Model: %s, Driver Version: %s)"), adapterIndex, 
			adapterDesc.Description,
			ToString(kmtDriverVersion).c_str(),
			userModeDriverVersionStr.c_str());
		

		// Cleanup the adapter descriptor
		if( pAdapterDescriptor )
		{
			delete pAdapterDescriptor;
			pAdapterDescriptor = nullptr;
		}
		
		// Is the current adapter the hybrid discrete adapter?
        if( AdapterType.HybridDiscrete )
        {
			m_bFoundHybridDiscreteAdapter = true;
			m_hybridDiscreteAdapterIndex = adapterIndex;
            m_hDiscreteAdapter = Adapters.Adapters[adapterIndex].hAdapter;
            Adapters.Adapters[adapterIndex].hAdapter = 0; // don't close it yet, need to use it for the escape
			WriteToLog( _T("Found a hybrid discrete adapter.") );
            break;
        }
    }

	if( m_bFoundHybridDiscreteAdapter )
	{
		WriteToLog( _T("Adapter #%d is the hybrid discrete adapter."), m_hybridDiscreteAdapterIndex );
	}
	else
	{
		WriteToLog( _T("The current system doesn't contain a hybrid discrete adapter. Skipping test.") );
		tr = RESULT_SKIP;
		EndSection();
		goto Done;
	}

	EndSection();

	//
	// Query the hybrid discrete adapter for the DList name
	//

	BeginSection( L"Query the hybrid discrete adapter for the discrete list (DList) dll" );

	QueryAdapterInfo.hAdapter = m_hDiscreteAdapter;
    QueryAdapterInfo.Type = KMTQAITYPE_DLIST_DRIVER_NAME;
    QueryAdapterInfo.pPrivateDriverData = (void*)&DListName;
    QueryAdapterInfo.PrivateDriverDataSize = sizeof(DListName);
	status = pfnQueryAdapterInfo( &QueryAdapterInfo );
	if( !NT_SUCCESS(status) )
    {
		WriteToLog( _T("D3DKMTQueryAdapterInfo() failed for KMTQAITYPE_DLIST_DRIVER_NAME") );
		LogError( __FILEW__, __LINE__, L"D3DKMTQueryAdapterInfo() failed.", status, ERRORCODE_TYPE_NTSTATUS, gFCD3DKMTQueryAdapterInfo );
		tr = RESULT_FAIL;
		EndSection();
		goto Done;
    }

	//
	// Load the DList dll
	//

	WriteToLog( L"Hybrid discrete list (DList) dll: %s", DListName.DListFileName );
	m_hDList = LoadLibraryW( DListName.DListFileName );
	if( !m_hDList )
	{
		// Regardless of the info below, the IHV failed
		LogError( __FILEW__, __LINE__, L"LoadLibraryW() failed for the DList dll.", HRESULT_FROM_WIN32(GetLastError()), ERRORCODE_TYPE_HRESULT, gFCLoadLibrary );
		tr = RESULT_FAIL;

		//
		// Load libaray failed, but we'll attempt to load the DLL as a data file to get some more info:
		// If it loads as a data file, then the dll exists and the IHV is missing some dependency for the dll. 
		// If it fails to load as a data file, then the IHV is missing the dll.
		//

		// Load the DLL as a data file
		HMODULE tempHandle = LoadLibraryExW( DListName.DListFileName, nullptr, LOAD_LIBRARY_AS_DATAFILE );
		if( tempHandle )
		{
			// The dll loaded as a data file, so it exists
			WriteToLog( _T("LoadLibraryExW() was successful for the Dlist dll with LOAD_LIBRARY_AS_DATAFILE. It looks like a Dlist dll dependency is missing.") );
			FreeLibrary( tempHandle );
			tempHandle = nullptr;
		}
		else
		{
			// The dll didn't even load as a data file
			WriteToLog( _T("LoadLibraryExW() failed for the Dlist dll with LOAD_LIBRARY_AS_DATAFILE. It looks like the Dlist dll is missing or in the wrong path.") );
		}

		// Exit because the test failed
		EndSection();
		goto Done;
	}

	EndSection();

Done:
	// Close adapters
	if( bAdaptersAreOpen && m_pfnCloseAdapter )
	{
		for( UINT i = 0; i < Adapters.NumAdapters; i++ )
		{
			if( Adapters.Adapters[i].hAdapter )
			{
				D3DKMT_CLOSEADAPTER CloseAdapter = {Adapters.Adapters[i].hAdapter};
				m_pfnCloseAdapter( &CloseAdapter );
			}
		}
	}

	if( pAdapterDescriptor )
	{
		delete pAdapterDescriptor;
		pAdapterDescriptor = nullptr;
	}

	return tr;
}

HRESULT APIENTRY CDiscreteListTest::EscapeCB(HANDLE hAdapter, CONST D3DDDICB_ESCAPE *pEscapeCB)
{
    D3DKMT_ESCAPE Escape = {};
    Escape.hAdapter = (D3DKMT_HANDLE)hAdapter;
    Escape.hDevice = 0;
    Escape.hContext = 0;
    Escape.pPrivateDriverData = pEscapeCB->pPrivateDriverData;
    Escape.PrivateDriverDataSize = pEscapeCB->PrivateDriverDataSize;
    Escape.Type = D3DKMT_ESCAPE_DRIVERPRIVATE;

    HMODULE hGDI32 = GetModuleHandle(_T("gdi32.dll"));
    PFND3DKMT_ESCAPE pfnEscape = reinterpret_cast<PFND3DKMT_ESCAPE>(GetProcAddress(hGDI32, "D3DKMTEscape"));
    NTSTATUS Status = pfnEscape( &Escape );

    if (NT_SUCCESS(Status))
    {
        return S_OK;
    }
    else
    {
        switch(Status)
        {
        case STATUS_INVALID_HANDLE:
            return D3DDDIERR_INVALIDHANDLE;
        case STATUS_DEVICE_REMOVED:
            return D3DDDIERR_DEVICEREMOVED;
        case STATUS_INVALID_PARAMETER:
            return E_INVALIDARG;
        default:
            return E_FAIL;
        }
    }
}

TEST_RESULT CDiscreteListTest::ExecuteTestCase()
{
	// Local variables
	HRESULT hr = E_FAIL;
	TEST_RESULT tr = RESULT_PASS;
	PFND3DDDI_QUERYDLISTFORAPPLICATION1 pfnDListQuery = nullptr;
	BOOL bDListAnswer = FALSE;
	MODULEINFO moduleInfo = { 0 };
	__int64 startTime = 0;
	__int64 endTime = 0;
	__int64 freq = 0;
	
	//
	// Get the QueryDListForApplication1 entry point
    // Driver should not pass if it only exports QueryDListForApplication
	//

	pfnDListQuery = (PFND3DDDI_QUERYDLISTFORAPPLICATION1)GetProcAddress( m_hDList, "QueryDListForApplication1" );
	if( !pfnDListQuery ) 
	{ 
		LogError( __FILEW__, __LINE__, L"GetProcAddress failed for QueryDListForApplication1.", HRESULT_FROM_WIN32(GetLastError()), ERRORCODE_TYPE_HRESULT, gFCGetProcAddress );
		tr = RESULT_FAIL;
		goto Done;
	}

	//
	// Time a DList query
	//

	BeginSection( L"Time a DList query" );

	// Start timing the code
	if( QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER*>(&startTime) ) != 0 ) 
	{
		// Code segment is being timed (Query DList)
		hr = pfnDListQuery( &bDListAnswer, (HANDLE)m_hDiscreteAdapter, &EscapeCB );

		// Finish timing the code
		if( QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER*>(&endTime) ) != 0 )
		{
			// Make sure the DList query was successful
			if( FAILED(hr) )
			{
				LogError( __FILEW__, __LINE__, L"QueryDListForApplication() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
				tr = RESULT_FAIL;
			}

			// Log the start and end values
			WriteToLog( _T("Start QueryPerformanceCounter value: %I64d"), startTime );
			WriteToLog( _T("End QueryPerformanceCounter value: %I64d"), endTime );

			// Get the frequency
			if( QueryPerformanceFrequency(  reinterpret_cast<LARGE_INTEGER*>(&freq) ) != 0 )
			{
				const double queryTimeInSeconds = ((endTime - startTime) * 1.0 / freq);
				const double queryTimeInMS = queryTimeInSeconds * 1000;
				WriteToLog( _T("QueryPerformanceCounter minimum resolution: 1/%I64d seconds (%g ms)."), freq, (1.0 / freq) * 1000 );
				WriteToLog( _T("DList query time: %g seconds (%g ms)."), queryTimeInSeconds, queryTimeInMS );
				WriteToLog( _T("Max allowed DList query time: %g seconds (%g ms)."), g_maxAllowedDListQueryTimeInMS / 1000.0, g_maxAllowedDListQueryTimeInMS );

				// If the query takes longer than 300 ms, it fails because that's a requirement
				if( queryTimeInMS > g_maxAllowedDListQueryTimeInMS )
				{
					LogError( __FILEW__, __LINE__, L"The DList query took longer than the maximum allowed time.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
					tr = RESULT_FAIL;
				}
			}
			else
			{
				LogError( __FILEW__, __LINE__, L"QueryPerformanceFrequency() failed.", HRESULT_FROM_WIN32(GetLastError()), ERRORCODE_TYPE_HRESULT, gFCQueryPerformanceFrequency );
				tr = RESULT_FAIL;
			}
		}
		else
		{
			LogError( __FILEW__, __LINE__, L"End QueryPerformanceCounter() failed.", HRESULT_FROM_WIN32(GetLastError()), ERRORCODE_TYPE_HRESULT, gFCQueryPerformanceCounter );
			tr = RESULT_FAIL;
		}
	}
	else
	{
		LogError( __FILEW__, __LINE__, L"Start QueryPerformanceCounter() failed.", HRESULT_FROM_WIN32(GetLastError()), ERRORCODE_TYPE_HRESULT, gFCQueryPerformanceCounter );
		tr = RESULT_FAIL;
	}

	EndSection();

	//
	// Check the size of the DList dll
	//

	BeginSection( L"Check the size of the DList dll" );

	if( !GetModuleInformation( GetCurrentProcess(), m_hDList, &moduleInfo, sizeof(moduleInfo) ) )
	{
		LogError( __FILEW__, __LINE__, L"GetModuleInformation() failed.", HRESULT_FROM_WIN32(GetLastError()), ERRORCODE_TYPE_HRESULT, gFCGetModuleInformation );
		tr = RESULT_FAIL;
	}
	else
	{
		const DWORD imageSizeBytes = moduleInfo.SizeOfImage;
		const double imageSizeKiloBytes = imageSizeBytes / 1024.0;
		WriteToLog( _T("DList dll size: %lu bytes (%g kB)."), imageSizeBytes, imageSizeKiloBytes );
		WriteToLog( _T("Max allowed DList dll size: %g bytes (%g kB)."), g_maxAllowedDListSizeKiloBytes * 1024.0, g_maxAllowedDListSizeKiloBytes );
		if( imageSizeKiloBytes > g_maxAllowedDListSizeKiloBytes )
		{
			LogError( __FILEW__, __LINE__, L"The DList dll uses too much memory.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			tr = RESULT_FAIL;
		}
	}

	EndSection();

Done:
	return tr;
}

void CDiscreteListTest::CleanupTestCase()
{
    // Close the discrete adapter
    if ( m_hDiscreteAdapter && m_pfnCloseAdapter )
    {
        D3DKMT_CLOSEADAPTER CloseAdapter = {m_hDiscreteAdapter};
        m_pfnCloseAdapter(&CloseAdapter);
        m_hDiscreteAdapter = 0;
        m_pfnCloseAdapter = nullptr;
    }

	// Free the loaded modules
	if( m_hDList )
	{
		FreeLibrary( m_hDList );
		m_hDList = nullptr;
	}

	if( m_hGDI32 )
	{
		FreeLibrary( m_hGDI32 );
		m_hGDI32 = nullptr;
	}
}