////////////////////////////////////////
// DXGIMultiPresent.cpp
// Author: JeffWick
// 2/25/2013
// Purpose: 
// HCK test to validate implementation of the Present1 DDI in WDDM 1.3+ drivers
// This can also be run on older drivers, where it provides functional coverage.
////////////////////////////////////////

#include "DXGIMultiPresent.h"

CMultiPresentApp g_TestApp;

////////////////////////////////////////////////////////////////////////////////
//
// CMultiPresentApp
//
////////////////////////////////////////////////////////////////////////////////

bool CMultiPresentApp::InitTestGroups()
{
	ADD_TEST_AND_DESC( "MultiPresent", CMultiPresentTest, "Verifies the Present1 DDI callback.");

	return true;
}

void CMultiPresentApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();
	m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;
}

CMultiPresentTest::CMultiPresentTest()
	: m_pDevice(nullptr)
	, m_pPartnerDevice(nullptr)
	, m_PresentCount(0)
	, m_numTextures(1)
{
}

// Initialize Test Parameters
void CMultiPresentTest::InitTestParameters()
{
	CUserValueSet<UINT> *pNumTextures = new CUserValueSet<UINT>();
    pNumTextures->AddValue( 1 );
    pNumTextures->AddValue( 5 );
	pNumTextures->AddValue( 1000 );
	testfactor::RFactor rTextures = AddParameter( _T( "Num_Textures" ), &m_numTextures, pNumTextures );
    SetRootTestFactor(rTextures);
}

TEST_RESULT CMultiPresentTest::Setup()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;

	if (!IsElevated())
	{
		LogError( __FILEW__, __LINE__, L"This test must be run elevated.", false, ERRORCODE_TYPE_BOOL); 
		tr = RESULT_FAIL;
		goto EndSetup;
	}

	m_pDevice = GetFramework()->GetDevice();

	assert(m_pDevice != nullptr);

	hr = m_pDevice->QueryInterface( __uuidof(m_pPartnerDevice), (void**)&m_pPartnerDevice );

	if (FAILED(hr))
	{
		LogError( __FILEW__, __LINE__, L"Unable to QueryInterface for a ID3D11PartnerDevice interface.", hr, ERRORCODE_TYPE_HRESULT); 
		tr = RESULT_FAIL;
		goto EndSetup;
	}

	assert(m_pPartnerDevice != nullptr);

EndSetup:
	return tr;
}

TEST_RESULT CMultiPresentTest::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	HANDLE hCompositionSurface = nullptr;
	ID3D11Texture2D** pTexture2DArray = new ID3D11Texture2D*[m_numTextures];
	CController EtwController;
	bool etwProcessingEnabled;
	DWORD pData = 0xfefefefe;

	ZeroMemory(pTexture2DArray, sizeof(ID3D11Texture2D*) * m_numTextures);

	m_PresentCount = 0;
	EtwController.AddConsumer(this);
	etwProcessingEnabled = EtwController.StartProcessing();
	
	if (!etwProcessingEnabled)
	{
		LogError( __FILEW__, __LINE__, L"Failed to start ETW listeners. Ensure this test is run elevated.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCRequiredCapabilityNotSupported );
		tr = RESULT_FAIL;
		goto Cleanup;
	}

	Sleep(100);

	D3D11_BOX destRegion;
	destRegion.left = 0;
	destRegion.right = 1;
	destRegion.top = 0;
	destRegion.bottom = 1;
	destRegion.front = 0;
	destRegion.back = 1;
	
	for (UINT i=0; i<m_numTextures; i++)
	{
		hr = m_pPartnerDevice->CreateCompositionBuffer(16, 16, DXGI_FORMAT_B8G8R8A8_UNORM, FALSE, DXGI_USAGE_RENDER_TARGET_OUTPUT, 0, __uuidof(ID3D11Texture2D), &hCompositionSurface, reinterpret_cast<void**>(&pTexture2DArray[i]));

		if (FAILED(hr))
		{
			LogError( __FILEW__, __LINE__, L"Unable to create Composition Buffer.", hr, ERRORCODE_TYPE_HRESULT); 
			tr = RESULT_FAIL;
			goto Cleanup;
		}

		GetEffectiveContext()->UpdateSubresource(pTexture2DArray[i], 0, &destRegion, (void*)&pData, 0, 0);
	}

	// On WDDM 1.3 drivers, a single call will be made to the Present1 DD1 with 
	// all resources. Each driver callback will result in a Dxgk Present ETW event.
	// On older drivers, the runtime intercepts the extra callbacks, so we also only
	// see one ETW event per call. 

	hr = m_pPartnerDevice->PresentCompositionBuffers(nullptr, reinterpret_cast< IUnknown** >(&pTexture2DArray[0]), m_numTextures);

	if (FAILED(hr))
	{
		LogError( __FILEW__, __LINE__, L"Unable to Present Composition Buffers.", hr, ERRORCODE_TYPE_HRESULT); 
		tr = RESULT_FAIL;
		goto Cleanup;
	}
	
	Sleep(100);
    EtwController.StopProcessing();
	etwProcessingEnabled = false;

	WriteToLog("Number of Present callbacks detected: %u", m_PresentCount);

	if (m_PresentCount != 1)
	{
		LogError( __FILEW__, __LINE__, L"Did not detect a single Present callback as expected", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure );
		tr = RESULT_FAIL;
		goto Cleanup;
	}

Cleanup:
	if (etwProcessingEnabled)
	{
		EtwController.StopProcessing();
	}

	for (UINT i=0; i<m_numTextures; i++)
	{
		SAFE_RELEASE(pTexture2DArray[i]);
	}
	SAFE_DELETE_ARRAY(pTexture2DArray);
	
	if (hCompositionSurface)
	{
		hCompositionSurface = nullptr;
	}
	
	return tr;
}

void CMultiPresentTest::Cleanup()
{
    SAFE_RELEASE(m_pPartnerDevice);
}

void CMultiPresentTest::Present_Handler(PEVENT_RECORD pEvent)
{
	if(pEvent->EventHeader.ProcessId == (ULONG)GetCurrentProcessId())
	{
		m_PresentCount++;
	}
}

bool CMultiPresentTest::IsElevated()
{
	HANDLE hToken = nullptr;
	TOKEN_ELEVATION te;
	DWORD length = 0;
	bool result = true;

	BOOL bOpenTokenResult = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);

	if (bOpenTokenResult != TRUE ||	hToken == nullptr || hToken == INVALID_HANDLE_VALUE)
	{
		LogError( __FILEW__, __LINE__, L"Unable to OpenProcessToken.", GetLastError(), ERRORCODE_TYPE_WIN32); 
		result = false;
		goto Cleanup;
	}

	ZeroMemory( &te, sizeof( te ) );

	BOOL bGetTokenResult = GetTokenInformation(hToken, TokenElevation, &te, sizeof(te), &length);

	if (bGetTokenResult != TRUE)
	{
		LogError( __FILEW__, __LINE__, L"Unable to GetTokenInformation.", GetLastError(), ERRORCODE_TYPE_WIN32); 
		result = false;
		goto Cleanup;
	}

	if (te.TokenIsElevated == 0)
	{
		result = false;
		goto Cleanup;
	}

Cleanup:
	if (hToken)
	{
		CloseHandle(hToken);
		hToken = nullptr;
	}
	return result;
}