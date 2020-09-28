#include "DXGITrimMemory.h"
#include "DXGITrimMemoryTest.h"

// DXGITrimMemoryTest
// The goal of this test is to verify that the time-to-first-frame after a Trim call is less than the time-to-first-frame after device creation.
// It also verifies the driver doesn't mishandle any resources after a Trim call

// Namespaces for LogError
using namespace WindowsTest;
using namespace WindowsTest::Graphics::Direct3D;

extern CTrimTestApp g_TestApp;

const LPCSTR m_WindowTitle = _T("Trim Window");

static LRESULT CALLBACK MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
		case WM_DESTROY:
			return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

// Initialize Test Parameters
void CTrimMemoryTest::InitTestParameters()
{
	// Scenario
	CUserValueSet<UINT> *pScenario = new CUserValueSet<UINT>();
	pScenario->AddValue( 0 );
	pScenario->AddValue( 1 );
	testfactor::RFactor rScenario = AddParameter( _T( "Scenario" ), &m_Scenario, pScenario );
	SetRootTestFactor( rScenario );
}

// Setup
TEST_RESULT CTrimMemoryTest::Setup()
{	
	for (UINT i=0; i<TRIM_TEXTURE_COUNT; i++)
	{
		m_pTrimTextures[i] = nullptr;
	}

	WNDCLASSEX wc;
	// Register the window class
	WNDCLASSEX twc = { sizeof(WNDCLASSEX), CS_CLASSDC, (WNDPROC)MsgProc, 0L, 0L, 
						GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr,
						m_WindowTitle, nullptr };
	wc = twc;
	RegisterClassEx( &wc );
	
	// Create the application's window
	m_hWnd = CreateWindow( m_WindowTitle, m_WindowTitle, 
							WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
							nullptr, nullptr, wc.hInstance, nullptr );

	// raise the priority of the current thread to reduce perf noise caused by other processes
	if( !SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL) )
	{
		LogError( __FILEW__, __LINE__, L"SetThreadPriority() failed", GetLastError(), ERRORCODE_TYPE_WIN32, gFCRequiredCapabilityNotSupported );
		return RESULT_FAIL;
	}

	// Set thread affinity so that this thread will only run on one CPU on multi-core systems
	// Bind this thread to the first processor.
	SetThreadAffinityMask(GetCurrentThread(), 0x1);

	return RESULT_PASS;
}

// Setup Test Case
TEST_RESULT CTrimMemoryTest::SetupTestCase()
{	
	IDXGIAdapter1* pAdapter1 = nullptr;
	ID3D10Device1* pD3DDevice = nullptr;
	ID3D11DeviceTest *pDevTest = nullptr;
	DevX::GRFX::AdapterDescriptor* pAdapterDescriptor = nullptr;
	D3DKMT_DRIVERVERSION kmtDriverVersion = KMT_DRIVERVERSION_WDDM_1_1;
	TEST_RESULT tr = RESULT_PASS;

	// Setup our D3D resources
	if (FAILED(InitD3D(m_hWnd)))
	{
		WriteToLog("InitD3D() failed");
		tr = RESULT_FAIL;
		goto Cleanup;
	}

	// Check if the driver supports WDDM1.3
	HRESULT hr = m_pAdapter->QueryInterface( __uuidof(IDXGIAdapter1), (void**)&pAdapter1 );
	if( FAILED( hr ) || pAdapter1 == nullptr)
	{
		WriteToLog( "CTrimMemoryTest::SetupTestCase() - QueryInterface failed for IDXGIAdapter1. hr = %s", D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Cleanup;
	}

	pAdapterDescriptor = DevX::GRFX::AdapterDescriptor::GetAdapterDescriptor( pAdapter1 );
	if( !pAdapterDescriptor )
	{
		WriteToLog( "CTrimMemoryTest::SetupTestCase() - DevX::GRFX::AdapterDescriptor::GetAdapterDescriptor failed." );
		tr = RESULT_FAIL;
		goto Cleanup;
	}

	pAdapterDescriptor->GetDisplayDriverModel( kmtDriverVersion );
	
	if ( kmtDriverVersion < KMT_DRIVERVERSION_WDDM_1_3)
	{
		WriteToLog( "This test is only applicable to drivers that are WDDM 1.3+, skipping." );
		tr = RESULT_SKIP;
		goto Cleanup;
	}

	// If we are running on FL9, check if this driver supports 10+ but doesn't implement the Level9 perf features
	if (g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0)
	{
		hr = D3D10CreateDevice1(m_pAdapter,D3D10_DRIVER_TYPE_HARDWARE,nullptr,0,D3D10_FEATURE_LEVEL_10_0,D3D10_1_SDK_VERSION,&pD3DDevice);

		if (SUCCEEDED(hr))
		{
			hr = m_pD3DDevice->QueryInterface(__uuidof( ID3D11DeviceTest ), (void**)&pDevTest );
			if (FAILED(hr) || pDevTest == NULL)
			{
				WriteToLog( _T( "CTrimMemoryTest::SetupTestCase() - QI for ID3D11DeviceTest failed. hr = %s" ), D3DHResultToString(hr).c_str());
				tr = RESULT_FAIL;
				goto Cleanup;
			}
		
			if (pDevTest->SupportsLevel9PerfFeatures() == FALSE)
			{
				WriteToLog( "This is a 10+ part that doesn't support optional Level9 perf features, skipping." );
				tr = RESULT_SKIP;
				goto Cleanup;
			}
		}
	}

	if (FAILED(InitGeom()))
	{
		WriteToLog("InitGeom() failed");
		tr = RESULT_FAIL;
		goto Cleanup;
	}

Cleanup:
	SAFE_RELEASE(pAdapter1);
	SAFE_RELEASE(pDevTest);
	SAFE_RELEASE(pD3DDevice);

	return tr;
}

// Execute Test case
TEST_RESULT CTrimMemoryTest::ExecuteTestCase()
{
	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;
	IDXGIDevice3* pDXGIDevice3 = nullptr;

	switch (m_Scenario)
	{
	case 0: // Timing case
		WriteToLog("Validate time-to-first-frame after Trim");
		LARGE_INTEGER qpcTimerFrequency; 
		LARGE_INTEGER qpcStartTime, qpcEndTime;
		LARGE_INTEGER timeElapsed1, timeElapsed2;

		// Get Timer Frequency
		if ( !QueryPerformanceFrequency( &qpcTimerFrequency ) || qpcTimerFrequency.QuadPart == 0)
		{
			LogError( __FILEW__, __LINE__, L"QueryPerformanceFrequency() failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			tr = RESULT_FAIL;
			goto Cleanup;
		}
	
		/*-------------------Start :: Time Sensitive Section 1--------------------------------*/
		//
		// Here we are measuring the time-to-first-present after device creation
		//

		// Start Time
		QPC_WITH_ERROR_CHECK( qpcStartTime );

		Render();
		hr = m_pSwapChain->Present(0, DXGI_PRESENT_DO_NOT_WAIT);

		if (FAILED(hr))
		{
			LogError( __FILEW__, __LINE__, L"Present() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
			tr = RESULT_FAIL;
			goto Cleanup;
		}

		// End time
		QPC_WITH_ERROR_CHECK( qpcEndTime );

		timeElapsed1.QuadPart = (qpcEndTime.QuadPart - qpcStartTime.QuadPart) * 1000 / qpcTimerFrequency.QuadPart;

		/*-------------------End :: Time Sensitive Section 1--------------------------------*/

		// free our staging buffers
		for (UINT i=0; i<10; i++)
		{
			m_pSwapChain->Present(1,0);
		}

		// to maintain backwards compatibilty with older OSes, a failure is okay but we can't call Trim
		if (SUCCEEDED(m_pD3DDevice->QueryInterface( __uuidof( IDXGIDevice3 ), reinterpret_cast< void** >( &pDXGIDevice3 ) )))
		{
			pDXGIDevice3->Trim();
		}
		else
		{
			m_pD3DDevice->Flush();
		}

		// give adequate time for Trim to occur	since it's asynchronous
		Sleep(5000);

		/*-------------------Start :: Time Sensitive Section 2--------------------------------*/
		//
		// Here we are measuring the time-to-first-present after a Trim call
		//

		// Start Time
		QPC_WITH_ERROR_CHECK( qpcStartTime );

		Render();
		hr = m_pSwapChain->Present(0, DXGI_PRESENT_DO_NOT_WAIT);

		if (FAILED(hr))
		{
			LogError( __FILEW__, __LINE__, L"Present() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
			tr = RESULT_FAIL;
			goto Cleanup;
		}

		// End time
		QPC_WITH_ERROR_CHECK( qpcEndTime );

		timeElapsed2.QuadPart = (qpcEndTime.QuadPart - qpcStartTime.QuadPart) * 1000 / qpcTimerFrequency.QuadPart;

		/*-------------------End :: Time Sensitive Section 2--------------------------------*/

		// Log results
		WriteToLog("Time-to-first-frame after device creation: %lld ms\n", timeElapsed1.QuadPart);
		WriteToLog("Time-to-first-frame after Trim call: %lld ms\n", timeElapsed2.QuadPart);

		if (timeElapsed2.QuadPart > (timeElapsed1.QuadPart*1.05))
		{
			LogError( __FILEW__, __LINE__, L"The time-to-first-frame after Trim was greater than 5 percent more than the time-to-first-frame after device creation", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			tr = RESULT_FAIL;
			goto Cleanup;
		}
		break;

	case 1: // Validate resources
		WriteToLog("Validate resources are maintained after a Trim call.");
		Render();
		hr = m_pSwapChain->Present(1,0);

		if (FAILED(hr))
		{
			LogError( __FILEW__, __LINE__, L"Present() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
			tr = RESULT_FAIL;
			goto Cleanup;
		}

		// to maintain backwards compatibilty with older OSes, a failure is okay but we can't call Trim
		if (SUCCEEDED(m_pD3DDevice->QueryInterface( __uuidof( IDXGIDevice3 ), reinterpret_cast< void** >( &pDXGIDevice3 ) )))
		{
			pDXGIDevice3->Trim();
		}
		else
		{
			m_pD3DDevice->Flush();
		}

		// give adequate time for Trim to occur	since it's asynchronous
		Sleep(5000);

		// check resources
		if (!ValidateResources())
		{
			LogError( __FILEW__, __LINE__, L"Resources didn't match what was expected. A Trim call should not affect any resources.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			tr = RESULT_FAIL;
			goto Cleanup;
		}

		break;

	default:
		LogError( __FILEW__, __LINE__, L"Unknown scenario parameter.", false, ERRORCODE_TYPE_BOOL, gFCConflictingParameters );
		tr = RESULT_FAIL;
		goto Cleanup;

	}

Cleanup:
	SAFE_RELEASE(pDXGIDevice3);
	return tr;
}


HRESULT CTrimMemoryTest::InitD3D( HWND hWnd )
{
	HRESULT hr;
	//Get the window client size
	RECT clientRect;
	GetClientRect(hWnd,&clientRect);
	UINT clientWidth=clientRect.right-clientRect.left;
	UINT clientHeight=clientRect.bottom-clientRect.top;

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferCount=2;
	sd.BufferDesc.RefreshRate.Numerator=1;
	sd.BufferDesc.RefreshRate.Denominator=60;
	sd.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Width=clientWidth;
	sd.BufferDesc.Height=clientHeight;
	sd.BufferDesc.Scaling=DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering=DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count=1;
	sd.SampleDesc.Quality=0;
	sd.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow=hWnd;
	sd.SwapEffect=DXGI_SWAP_EFFECT_DISCARD;
	sd.Windowed=true;
	sd.Flags=0;

	//Get the adapter
	if(FAILED(hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&m_pFactory) ) ) )
	{
		WriteToLog("Could not create DXGI Factory.");
		return hr;
	}

	//
	// Adapter 0 is guaranteed to be the adapter owning desktop coordinate (0,0) and the one we want to test.
	//
	if(FAILED(hr = m_pFactory->EnumAdapters(0, &m_pAdapter)))
	{
		WriteToLog("Could not enumerate adapters (do you have a D3D10-capable device?)");
		return hr;
	}

	DXGI_ADAPTER_DESC AdapterDesc;
	m_pAdapter->GetDesc(&AdapterDesc);
	wprintf(L"Using: %s\n",AdapterDesc.Description);

	D3D10_FEATURE_LEVEL1 FeatureLevels_All[] = {
		D3D10_FEATURE_LEVEL_10_0,
		D3D10_FEATURE_LEVEL_9_3,
		D3D10_FEATURE_LEVEL_9_2,
		D3D10_FEATURE_LEVEL_9_1
	};

	D3D10_FEATURE_LEVEL1* FeatureLevels;
	UINT NbFeatureLevels;
	FeatureLevels = FeatureLevels_All;

	if ((D3D10_FEATURE_LEVEL1)g_TestApp.m_D3DOptions.FeatureLevel < D3D10_FEATURE_LEVEL_10_0)
	{
		FeatureLevels_All[0] = (D3D10_FEATURE_LEVEL1)g_TestApp.m_D3DOptions.FeatureLevel;
		NbFeatureLevels = 1;
	}
	else
	{
		NbFeatureLevels = ARRAYSIZE(FeatureLevels_All);
	}
	
	for (UINT fl = 0; fl < NbFeatureLevels; fl++)
	{
		hr = D3D10CreateDevice1(m_pAdapter,D3D10_DRIVER_TYPE_HARDWARE,nullptr,0,FeatureLevels[fl],D3D10_1_SDK_VERSION,&m_pD3DDevice);
		if (SUCCEEDED(hr))
		{
			break;
		}
	}
	if(FAILED(hr))
	{
		WriteToLog("Could not create a D3D10 Device.");
		return hr;
	}

	   
	if(FAILED(m_pFactory->CreateSwapChain(m_pD3DDevice,&sd,&m_pSwapChain) ) )
	{
		WriteToLog("Could not create Swap Chain.");
		return hr;
	}
	
	// Create depth stencil texture
	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = clientWidth;
	descDepth.Height = clientHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D16_UNORM;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = m_pD3DDevice->CreateTexture2D( &descDepth, nullptr, &m_pDepthStencil );
	if( FAILED(hr) )
		return hr;
	
	// Create the depth stencil view
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_pD3DDevice->CreateDepthStencilView( m_pDepthStencil, &descDSV, &m_pDepthStencilView );
	if( FAILED(hr) )
		return hr;
	
	//Set the viewport
	D3D10_VIEWPORT *pViewports=new D3D10_VIEWPORT[1];
	pViewports[0].TopLeftX=0;
	pViewports[0].TopLeftY=0;
	pViewports[0].Width=clientWidth;
	pViewports[0].Height=clientHeight;
	pViewports[0].MinDepth=0.0f;
	pViewports[0].MaxDepth=1.0f;
	m_pD3DDevice->RSSetViewports(1,pViewports);

	// Create a render target view
	ID3D10Texture2D *pBackBuffer;
	hr = m_pSwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), (LPVOID*)&pBackBuffer );
	if( FAILED(hr) )
		return hr;

	hr = m_pD3DDevice->CreateRenderTargetView( pBackBuffer, nullptr, &m_pRenderTargetView );
	if( FAILED(hr) )
		return hr;

	pBackBuffer->Release();

	m_pD3DDevice->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );
	
	//turn off back-face culling---
	D3D10_RASTERIZER_DESC rasterizerDesc;
	
	rasterizerDesc.CullMode = D3D10_CULL_NONE;
	//defaults----
	rasterizerDesc.FillMode = D3D10_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	//----defaults
	
	m_pD3DDevice->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
	m_pD3DDevice->RSSetState(m_rasterizerState);
	//----back-face culling
	
	return S_OK;
}

HRESULT CTrimMemoryTest::InitGeom() 
{
	HRESULT hr = S_OK;

	// *** Create data and textures for UpdateSubResource scenario for Texture2D ***

	// Create a block of data to upload
	ULONG TextureSize = TRIM_TEXTURE_DIMENSION * TRIM_TEXTURE_DIMENSION * 4;
	m_pTrimData = new BYTE[TextureSize];
	memset(m_pTrimData, 0xff, TextureSize);

	// Build a description of the textures to create
	ZeroMemory(&m_DescTexture, sizeof(m_DescTexture));
	m_DescTexture.Width = TRIM_TEXTURE_DIMENSION;
	m_DescTexture.Height = TRIM_TEXTURE_DIMENSION;
	m_DescTexture.MipLevels = 1;
	m_DescTexture.ArraySize = 1;
	m_DescTexture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_DescTexture.SampleDesc.Count = 1;
	m_DescTexture.SampleDesc.Quality = 0;
	m_DescTexture.Usage = D3D10_USAGE_DEFAULT;
	m_DescTexture.BindFlags = D3D10_BIND_SHADER_RESOURCE;

	ZeroMemory(&m_DescSRV, sizeof(m_DescSRV));
	m_DescSRV.Format = m_DescTexture.Format;
	m_DescSRV.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	m_DescSRV.Texture2D.MostDetailedMip = 0;
	m_DescSRV.Texture2D.MipLevels = 1;

	// Create the textures
	m_pTextures = new ID3D10ShaderResourceView*[TRIM_TEXTURE_COUNT];
	m_TextureCount = TRIM_TEXTURE_COUNT;
	for (UINT i = 0; i < TRIM_TEXTURE_COUNT; i++)
	{
		ID3D10Texture2D *pTexture2D;
		m_pD3DDevice->CreateTexture2D(&m_DescTexture, nullptr, &pTexture2D);
		m_pD3DDevice->CreateShaderResourceView(pTexture2D, &m_DescSRV, &m_pTextures[i]);
				
		// add to the texture array
		m_pTrimTextures[i] = pTexture2D;
	}

	// *** End UpdateSubResource scenario resource creation for Texture2D ***

	// *** Start Map Discard scenario resource creation for Texture2D ***

	// Build a description of the textures to create
	ZeroMemory(&m_DescTexture2, sizeof(m_DescTexture2));
	m_DescTexture2.Width = TRIM_TEXTURE_DIMENSION;
	m_DescTexture2.Height = TRIM_TEXTURE_DIMENSION;
	m_DescTexture2.MipLevels = 1;
	m_DescTexture2.ArraySize = 1;
	m_DescTexture2.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_DescTexture2.SampleDesc.Count = 1;
	m_DescTexture2.SampleDesc.Quality = 0;
	m_DescTexture2.Usage = D3D10_USAGE_DYNAMIC;
	m_DescTexture2.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	m_DescTexture2.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	ZeroMemory(&m_DescSRV2, sizeof(m_DescSRV2));
	m_DescSRV2.Format = m_DescTexture2.Format;
	m_DescSRV2.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	m_DescSRV2.Texture2D.MostDetailedMip = 0;
	m_DescSRV2.Texture2D.MipLevels = 1;
			
	m_TextureCountTrimMap = 1;
	m_pD3DDevice->CreateTexture2D(&m_DescTexture2, nullptr, &m_pTexture2D);
	m_pD3DDevice->CreateShaderResourceView(m_pTexture2D, &m_DescSRV2, &m_pTextureTrimMap);

	D3D10_TEXTURE2D_DESC copyTextureDesc;
	ZeroMemory(&copyTextureDesc, sizeof(copyTextureDesc));
	copyTextureDesc.Width = TRIM_TEXTURE_DIMENSION;
	copyTextureDesc.Height = TRIM_TEXTURE_DIMENSION;
	copyTextureDesc.MipLevels = 1;
	copyTextureDesc.ArraySize = 1;
	copyTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	copyTextureDesc.SampleDesc.Count = 1;
	copyTextureDesc.SampleDesc.Quality = 0;
	copyTextureDesc.Usage = D3D10_USAGE_STAGING;
	copyTextureDesc.BindFlags = 0;
	copyTextureDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
	copyTextureDesc.MiscFlags = 0;

	m_pD3DDevice->CreateTexture2D(&copyTextureDesc, nullptr, &m_pTexture2D_Copy);

	// *** End Map Discard scenario resource creation for Texture2D ***
	
	// *** Start Map Discard scenario resource creation for Vertex & Constant Buffers ***

	// Create cube vertices 

	VertexPositionColor Corners[8];
	for (int i=0;i<8;i++)
	{
		int x = i%2;
		int y = (i%4) / 2;
		int z = i/4;

		Corners[i].Position.x = (float)x * 100 - 50;
		Corners[i].Position.y = (float)y * 100 - 50;
		Corners[i].Position.z = (float)z * 100 - 50;
		Corners[i].Color.x = (float)x * 100 - 50;
		Corners[i].Color.y = (float)y * 100 - 50;
		Corners[i].Color.z = (float)z * 100 - 50;
	}

	int Indices[] = {
		2, 1, 0,
		3, 1, 2,

		3, 5, 1,
		7, 5, 3,

		7, 4, 5,
		6, 4, 7,

		6, 0, 4,
		2, 0, 6,

		5, 4, 0,
		1, 5, 0,

		3, 7, 6,
		2, 3, 6
	};

	#define numIndices (sizeof(Indices) / sizeof(int))
	int x = numIndices;

	ZeroMemory(&m_Vertices, sizeof(m_Vertices));
	C_ASSERT(ARRAYSIZE(m_Vertices) >= numIndices);
	for (int i=0;i<numIndices;i++)
	{
		m_Vertices[i] = Corners[Indices[i]];
	}
				
	//Create the effect for drawing vbs
	ID3D10Blob *pErrors = nullptr;
	hr =  D3DX10CreateEffectFromResource(NULL, "DXGITrim_shader_vb.fx", "DXGITrim_shader_vb.fx", NULL, NULL, "fx_4_0", D3D10_SHADER_NO_PRESHADER, 0, m_pD3DDevice, NULL, NULL, &m_pEffect, &pErrors, nullptr);
	if ( FAILED(hr) )
	{
		WriteToLog("Failed to compile effect");
		SAFE_RELEASE(pErrors);
		return hr;
	}
	SAFE_RELEASE(pErrors);

	// Obtain the technique
	m_pTechnique = m_pEffect->GetTechniqueByName( "TransformColor" );
	m_pWorldViewProjVariable = m_pEffect->GetVariableByName( "m_WorldViewProj" )->AsMatrix();
			
	//Define the input layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
				D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, 
				D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = sizeof(layout)/sizeof(layout[0]);

	// Create the input layout
	ID3D10EffectPass *pass0=m_pTechnique->GetPassByIndex( 0 );
	D3D10_PASS_DESC PassDesc;
	hr=pass0->GetDesc( &PassDesc );
	if ( FAILED( hr = m_pD3DDevice->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pVertexLayout ) ) )
	{
		return hr;
	}

	// Set the input layout
	m_pD3DDevice->IASetInputLayout( m_pVertexLayout );

	//Create description for all VBs
	m_bufferDesc.ByteWidth = sizeof(m_Vertices);
	m_bufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	m_bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	m_bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	m_bufferDesc.MiscFlags = 0;

	//Create description for all CBs
	m_cbufferDesc.ByteWidth = sizeof(m_Vertices);
	m_cbufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	m_cbufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	m_cbufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	m_cbufferDesc.MiscFlags = 0;
	
	m_pBuffers = new ID3D10Buffer*[RESOURCE_COUNT];
	m_pCBuffers = new ID3D10Buffer*[RESOURCE_COUNT];

	// Create VBs and CBs
	for (int i=0;i<RESOURCE_COUNT;i++)
	{
		m_pD3DDevice->CreateBuffer(&m_bufferDesc, nullptr, &m_pBuffers[i]);
		m_pD3DDevice->CreateBuffer(&m_cbufferDesc, nullptr, &m_pCBuffers[i]);
	}

	// *** End Map Discard scenario resource creation for Vertex & Constant Buffers ***
	
	// *** Start UpdateSubResource scenario resource creation for Vertex & Constant Buffers ***

	//Create description for all VBs
	m_bufferDesc2.ByteWidth = sizeof(m_Vertices);
	m_bufferDesc2.Usage = D3D10_USAGE_DEFAULT;
	m_bufferDesc2.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	m_bufferDesc2.CPUAccessFlags = 0;
	m_bufferDesc2.MiscFlags = 0;

	//Create description for all CBs
	m_cbufferDesc2.ByteWidth = sizeof(m_Vertices);
	m_cbufferDesc2.Usage = D3D10_USAGE_DEFAULT;
	m_cbufferDesc2.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	m_cbufferDesc2.CPUAccessFlags = 0;
	m_cbufferDesc2.MiscFlags = 0;

	m_pBuffers2 = new ID3D10Buffer*[RESOURCE_COUNT];
	m_pCBuffers2 = new ID3D10Buffer*[RESOURCE_COUNT];

	// Create VBs and CBs
	for (int i=0;i<RESOURCE_COUNT;i++)
	{
		m_pD3DDevice->CreateBuffer(&m_bufferDesc2, nullptr, &m_pBuffers2[i]);
		m_pD3DDevice->CreateBuffer(&m_cbufferDesc2, nullptr, &m_pCBuffers2[i]);
	}

	// Create a block of data to upload
	m_pTrimBufferData = new BYTE[sizeof(m_Vertices)];
	memset(m_pTrimBufferData, 0xff, sizeof(m_Vertices));

	m_pTrimCBufferData = new BYTE[sizeof(m_Vertices)];
	memset(m_pTrimCBufferData, 0xff, sizeof(m_Vertices));
	
	// *** End UpdateSubResource scenario resource creation for Vertex & Constant Buffers ***

	return S_OK;
}

HRESULT CTrimMemoryTest::CreateQuad(LPCSTR path)
{    
	HRESULT hr = S_OK;
	//Create a quad
	VertexPositionNormalTexture Vertices[] =
	{
		D3DXVECTOR3(-100,100,0), D3DXVECTOR3(0,0,0), D3DXVECTOR2(0,0),
		D3DXVECTOR3(-100,-100,0),  D3DXVECTOR3(0,0,0), D3DXVECTOR2(0,1),
		D3DXVECTOR3(100,100,0),  D3DXVECTOR3(0,0,0), D3DXVECTOR2(1,0),

		D3DXVECTOR3(100,100,0),  D3DXVECTOR3(0,0,0), D3DXVECTOR2(1,0),
		D3DXVECTOR3(-100,-100,0),  D3DXVECTOR3(0,0,0), D3DXVECTOR2(0,1),
		D3DXVECTOR3(100,-100,0),   D3DXVECTOR3(0,0,0), D3DXVECTOR2(1,1)
	};
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = Vertices;

	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(Vertices);
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	m_pD3DDevice->CreateBuffer(&bufferDesc, &InitData, &m_pQuad);

	//Create effect for rendering textured quad
	hr = createStandardEffect(path);
	if (FAILED(hr))
	{
		WriteToLog("Could not create standard effect");
	}

	return hr;
}

HRESULT CTrimMemoryTest::createStandardEffect(LPCSTR path)
{
	ID3D10Blob* EffectErrors = nullptr;
	HRESULT hr2;
	HRESULT hr = D3DX10CreateEffectFromResource(nullptr, path, path, nullptr, nullptr, "fx_4_0", D3D10_SHADER_NO_PRESHADER, 0, m_pD3DDevice, nullptr, nullptr, &m_pEffect2, &EffectErrors, &hr2);
	
	if (FAILED(hr))
	{
		char* ErrorText = (char*)EffectErrors->GetBufferPointer();
		SAFE_RELEASE(EffectErrors);
		return hr;
	}

	SAFE_RELEASE(EffectErrors);

	// Obtain the technique
	m_pTechnique = m_pEffect2->GetTechniqueByName( "TransformColor" );
	m_pWorldViewProjVariable = m_pEffect2->GetVariableByName( "m_WorldViewProj" )->AsMatrix();
	m_pTextureVariable = m_pEffect2->GetVariableByName( "m_Texture" )->AsShaderResource();

	// Define the input layout
	const D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout)/sizeof(layout[0]);

	// Create the input layout
	ID3D10EffectPass *pass0=m_pTechnique->GetPassByIndex( 0 );
	D3D10_PASS_DESC PassDesc;
	hr=pass0->GetDesc( &PassDesc );
	if ( FAILED( hr = m_pD3DDevice->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pVertexLayout ) ) )
	{
		WriteToLog("CreateInputLayout failed");
		return hr;
	}

	// Set the input layout
	m_pD3DDevice->IASetInputLayout( m_pVertexLayout );

	return hr;
}

VOID CTrimMemoryTest::Render()
{
	HRESULT hr;

	//
	// Create a textured quad and effect to render the textures
	//
	CreateQuad("DXGITrim_shader.fx");

	//
	// Clear the back buffer
	//
	static float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
	m_pD3DDevice->ClearRenderTargetView( m_pRenderTargetView, ClearColor );

	//
	// Clear the depth buffer to 1.0 (max depth)
	//
	m_pD3DDevice->ClearDepthStencilView( m_pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );

	//
	// Update variables that change once per frame
	//
	SetupMatrices();

	// run our Texture2D UpdateSubResource scenario
	if (m_pQuad && m_pTextures)
	{
		PrepareRect();

		for (UINT i = 0; i < m_TextureCount; i++)
		{
			UpdateTrimResource(i);		
			DrawRect(m_pTextures[i]);
		}
	}

	// run our Texture2D Map Discard scenario
	if (m_pTexture2D)
	{
		D3D10_MAPPED_TEXTURE2D pMappedTexture;

		for (UINT i = 0; i < TRIM_MAPS_PER_FRAME; i++)
		{
			hr = m_pTexture2D->Map(0, D3D10_MAP_WRITE_DISCARD, 0, &pMappedTexture);
			if(SUCCEEDED(hr))
			{
				// only need to fill it the last time for later verification
				if (i == (TRIM_MAPS_PER_FRAME-1))
				{
					UCHAR* pTexels = (UCHAR*)pMappedTexture.pData;
					for( UINT row = 0; row < m_DescTexture2.Height; row++ )
					{
					  UINT rowStart = row * pMappedTexture.RowPitch;
					  for( UINT col = 0; col < m_DescTexture2.Width; col++ )
					  {
							UINT colStart = col * 4;
							pTexels[rowStart + colStart + 0] = 0; // Red
							pTexels[rowStart + colStart + 1] = 100; // Green
							pTexels[rowStart + colStart + 2] = 200; // Blue
							pTexels[rowStart + colStart + 3] = 255; // Alpha
					  }
					}
				}

				m_pTexture2D->Unmap(0);

				m_pD3DDevice->CopyResource( m_pTexture2D_Copy, m_pTexture2D );
			}
			else
			{
				LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
			}
		}
	}
			
	// run our Buffer Map Discard scenario
	if (m_pBuffers)
	{
		m_pD3DDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		UINT Stride = sizeof(VertexPositionColor);
		UINT Offset = 0;

		float* pMappedBuffer;
		D3D10_TECHNIQUE_DESC techDesc;
		m_pTechnique->GetDesc( &techDesc );
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			m_pTechnique->GetPassByIndex( p )->Apply(0);
			for (UINT i = 0; i < RESOURCE_COUNT; i++)
			{
				hr = m_pBuffers[i]->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&pMappedBuffer);
				if(SUCCEEDED(hr))
				{
					memset(pMappedBuffer, 0xfe, sizeof(m_Vertices));
					m_pBuffers[i]->Unmap();
				}
				else
				{
					LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
				}
				m_pD3DDevice->IASetVertexBuffers(0,1,&m_pBuffers[i],&Stride,&Offset);
				m_pD3DDevice->Draw(36,0);
			}
		}

	}

	// run our Buffer UpdateSubResource scenario
	if (m_pBuffers2)
	{
		for (UINT i = 0; i < RESOURCE_COUNT; i++)
		{
			m_pD3DDevice->UpdateSubresource(m_pBuffers2[i], 0, nullptr, m_pTrimBufferData, 0, 0);
		}
	}
			
	// run our Constant Buffer Map Discard scenario
	if (m_pCBuffers)
	{
		m_pD3DDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		UINT Stride = sizeof(VertexPositionColor);
		UINT Offset = 0;

		float* pMappedCBuffer;
		D3D10_TECHNIQUE_DESC techDesc;
		m_pTechnique->GetDesc( &techDesc );
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			m_pTechnique->GetPassByIndex( p )->Apply(0);
			for (UINT i = 0; i < RESOURCE_COUNT; i++)
			{
				hr = m_pCBuffers[i]->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&pMappedCBuffer);
				if(SUCCEEDED(hr))
				{
					memset(pMappedCBuffer, 0xfe, sizeof(m_Vertices));
					m_pCBuffers[i]->Unmap();
				}
				else
				{
					LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
				}
				m_pD3DDevice->VSSetConstantBuffers(0,1,&m_pCBuffers[i]);
				m_pD3DDevice->Draw(36,0);
			}
		}

	}

	// run our Constant Buffer UpdateSubResource scenario
	if (m_pCBuffers2)
	{
		for (UINT i = 0; i < RESOURCE_COUNT; i++)
		{
			m_pD3DDevice->UpdateSubresource(m_pCBuffers2[i], 0, nullptr, m_pTrimCBufferData, 0, 0);
		}
	}

	SAFE_RELEASE(m_pQuad);
	SAFE_RELEASE(m_pEffect2);
	SAFE_RELEASE(m_pVertexLayout);
}

VOID CTrimMemoryTest::SetupMatrices()
{
	// Set up world matrix
	D3DXMATRIX matWorld;
	D3DXMatrixRotationY( &matWorld, timeGetTime()/1000.0f );

	// Set up our view matrix. A view matrix can be defined given an eye point,
	// a point to lookat, and a direction for which way is up. Here, we set the
	// eye five units back along the z-axis and up three units, look at the 
	// origin, and define "up" to be in the y-direction.
	D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-500.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIX matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );

	// For the projection matrix, we set up a perspective transform (which
	// transforms geometry from 3D view space to 2D viewport space, with
	// a perspective divide making objects smaller in the distance). To build
	// a perpsective transform, we need the field of view (1/4 pi is common),
	// the aspect ratio, and the near and far clipping planes (which define at
	// what distances geometry should be no longer be rendered).
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, (float)D3DX_PI/4, 1.0f, 1.0f, 1000.0f );

	//Multiply and pass to the effect
	if (m_pWorldViewProjVariable) {
		D3DXMATRIX matWorldView=matWorld * matView;
		D3DXMATRIX matWorldViewProj=matWorldView * matProj;
		m_pWorldViewProjVariable->SetMatrix((float *)matWorldViewProj);
	}
}

//Sets up state for DrawRect calls
void CTrimMemoryTest::PrepareRect()
{
	m_pD3DDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	UINT Stride = sizeof(VertexPositionNormalTexture);
	UINT Offset = 0;
	m_pD3DDevice->IASetVertexBuffers(0,1,&m_pQuad,&Stride,&Offset);
}

// forces an update to the resource data in Trim scenario
void CTrimMemoryTest::UpdateTrimResource(UINT i)
{
	fprintf(stderr, "updating resource %d", i);
	fflush(stderr);
	
	m_pD3DDevice->UpdateSubresource(m_pTrimTextures[i], 0, nullptr, m_pTrimData, TRIM_TEXTURE_DIMENSION * 4, 0);
}

//draws a rect given a pointer to a ShaderResourceView.
void CTrimMemoryTest::DrawRect(ID3D10ShaderResourceView * shaderResourceView)
{
	m_pTextureVariable->SetResource(shaderResourceView);

	D3D10_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pTechnique->GetPassByIndex( p )->Apply(0);
		m_pD3DDevice->Draw(6,0);
	}
}

//draws a rect given a pointer to a ShaderResourceView.
bool CTrimMemoryTest::ValidateResources()
{
	HRESULT hr;
	ID3D10Texture2D* Tex2D = nullptr;
	D3D10_TEXTURE2D_DESC Desc2D  = { '\0' };
	ID3D10ShaderResourceView* pShaderRV = nullptr;
	D3D10_SHADER_RESOURCE_VIEW_DESC DescShaderRV = {};
	D3D10_MAPPED_TEXTURE2D pData;
	ID3D10Buffer* pBuffer = nullptr;
	D3D10_BUFFER_DESC BufferDesc = { '\0' };
	float* pMappedBuffer;
	ID3D10Buffer* pBufferCopy;
	BYTE *pMappedBuffer2 = new BYTE[sizeof(m_Vertices)];
	ID3D10Texture2D* pCopyTexture;
	D3D10_MAPPED_TEXTURE2D pMappedTexture;
	VertexPositionColor	vertices[680];

	// Check our textures and shader resource views used in the Texture2D UpdateSubResource scenario

	D3D10_TEXTURE2D_DESC copyTextureDesc;
	copyTextureDesc.Width = TRIM_TEXTURE_DIMENSION;
	copyTextureDesc.Height = TRIM_TEXTURE_DIMENSION;
	copyTextureDesc.MipLevels = 1;
	copyTextureDesc.ArraySize = 1;
	copyTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	copyTextureDesc.SampleDesc.Count = 1;
	copyTextureDesc.SampleDesc.Quality = 0;
	copyTextureDesc.Usage = D3D10_USAGE_STAGING;
	copyTextureDesc.BindFlags = 0;
	copyTextureDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
	copyTextureDesc.MiscFlags = 0;

	if (FAILED(hr = m_pD3DDevice->CreateTexture2D(&copyTextureDesc, nullptr, &pCopyTexture)))
	{
		WriteToLog( _T( "CreateTexture2D failed, hr = %s" ), D3DHResultToString( hr ).c_str() );
		return false;
	}

	for (UINT i = 0; i < m_TextureCount; i++)
	{
		hr = m_pTrimTextures[i]->QueryInterface(__uuidof(ID3D10Texture2D), (VOID**)&Tex2D);
		if(SUCCEEDED(hr))
		{
			Tex2D->GetDesc( &Desc2D );
			Tex2D->Release();
		}
		else
		{
			WriteToLog( _T( "QueryInterface failed, hr = %s" ), D3DHResultToString( hr ).c_str() );
			return false;
		}

		if (!CompareTexture2D(Desc2D, m_DescTexture))
		{
			LogError( __FILEW__, __LINE__, L"CompareTexture2D() failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			return false;
		}

		hr = m_pTextures[i]->QueryInterface(__uuidof(ID3D10ShaderResourceView), (VOID**)&pShaderRV);
		if(SUCCEEDED(hr))
		{
			pShaderRV->GetDesc( &DescShaderRV );
			pShaderRV->Release();
		}
		else
		{
			WriteToLog( _T( "QueryInterface failed, hr = %s" ), D3DHResultToString( hr ).c_str() );
			return false;
		}

		if (!CompareShaderRV(DescShaderRV, m_DescSRV))
		{
			LogError( __FILEW__, __LINE__, L"CompareShaderRV() failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			return false;
		}

		m_pD3DDevice->CopyResource( pCopyTexture, m_pTrimTextures[i] );
		hr = pCopyTexture->Map(0, D3D10_MAP_READ, 0, &pMappedTexture);

		if(SUCCEEDED(hr))
		{
			UCHAR* pTexels = (UCHAR*)pMappedTexture.pData;
			for( UINT row = 0; row < m_DescTexture.Height; row++ )
			{
				UINT rowStart = row * pMappedTexture.RowPitch;
				for( UINT col = 0; col < m_DescTexture.Width; col++ )
				{
					UINT colStart = col * 4;
					if (pTexels[rowStart + colStart + 0] != 0xff ||
						pTexels[rowStart + colStart + 1] != 0xff ||
						pTexels[rowStart + colStart + 2] != 0xff ||
						pTexels[rowStart + colStart + 3] != 0xff)
					{
						LogError( __FILEW__, __LINE__, L"Texture2D value compare failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
						return false;
					}
				}
			}

			pCopyTexture->Unmap(0);
		}
		else
		{
			LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
			return false;
		}

	}

	// Check our texture and shader resource view used in the Texture2D Map scenario
	hr = m_pTexture2D->QueryInterface(__uuidof(ID3D10Texture2D), (VOID**)&Tex2D);
	if(SUCCEEDED(hr))
	{
		Tex2D->GetDesc( &Desc2D );
		Tex2D->Release();
	}
	else
	{
		WriteToLog( _T( "QueryInterface failed, hr = %s" ), D3DHResultToString( hr ).c_str() );
		return false;
	}

	if (!CompareTexture2D(Desc2D, m_DescTexture2))
	{
		LogError( __FILEW__, __LINE__, L"CompareTexture2D() failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
		return false;
	}

	hr = m_pTextureTrimMap->QueryInterface(__uuidof(ID3D10ShaderResourceView), (VOID**)&pShaderRV);
	if(SUCCEEDED(hr))
	{
		pShaderRV->GetDesc( &DescShaderRV );
		pShaderRV->Release();
	}
	else
	{
		WriteToLog( _T( "QueryInterface failed, hr = %s" ), D3DHResultToString( hr ).c_str() );
		return false;
	}

	if (!CompareShaderRV(DescShaderRV, m_DescSRV2))
	{
		LogError( __FILEW__, __LINE__, L"CompareShaderRV() failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
		return false;
	}

	m_pD3DDevice->CopyResource( pCopyTexture, m_pTexture2D );
	hr = pCopyTexture->Map(0, D3D10_MAP_READ, 0, &pMappedTexture);

	if(SUCCEEDED(hr))
	{
		UCHAR* pTexels = (UCHAR*)pMappedTexture.pData;
		for( UINT row = 0; row < m_DescTexture2.Height; row++ )
		{
			UINT rowStart = row * pMappedTexture.RowPitch;
			for( UINT col = 0; col < m_DescTexture2.Width; col++ )
			{
				UINT colStart = col * 4;
				if (pTexels[rowStart + colStart + 0] != 0 ||
					pTexels[rowStart + colStart + 1] != 100 ||
					pTexels[rowStart + colStart + 2] != 200 ||
					pTexels[rowStart + colStart + 3] != 255)
				{
					LogError( __FILEW__, __LINE__, L"Texture2D value compare failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
					return false;
				}
			}
		}

		pCopyTexture->Unmap(0);
	}
	else
	{
		LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
		return false;
	}

	// Check our vertex and constant buffers used in the Map scenario

	D3D10_BUFFER_DESC copyDesc;
	copyDesc.ByteWidth = sizeof(m_Vertices);
	copyDesc.Usage = D3D10_USAGE_STAGING;
	copyDesc.BindFlags = 0;
	copyDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
	copyDesc.MiscFlags = 0;
	
	// Fill in our memory that we will use for comparison
	memset(vertices, 0xfe, sizeof(vertices));

	if (FAILED(hr = m_pD3DDevice->CreateBuffer(&copyDesc, nullptr, &pBufferCopy)))
	{
		WriteToLog( _T( "CreateBuffer failed, hr = %s" ), D3DHResultToString( hr ).c_str() );
		return false;
	}

	for (UINT i = 0; i < RESOURCE_COUNT; i++)
	{
		hr = m_pBuffers[i]->QueryInterface(__uuidof(ID3D10Buffer), (VOID**)&pBuffer);
		if(SUCCEEDED(hr))
		{
			pBuffer->GetDesc( &BufferDesc );
			pBuffer->Release();
		}
		else
		{
			WriteToLog( _T( "QueryInterface failed, hr = %s" ), D3DHResultToString( hr ).c_str() );
			return false;
		}

		if (!CompareBuffers(BufferDesc, m_bufferDesc))
		{
			LogError( __FILEW__, __LINE__, L"CompareBuffers() failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			return false;
		}

		m_pD3DDevice->CopyResource( pBufferCopy, m_pBuffers[i] );
		hr = pBufferCopy->Map(D3D10_MAP_READ, 0, (void**)&pMappedBuffer);
		if(SUCCEEDED(hr))
		{
			if (memcmp(pMappedBuffer, vertices, sizeof(m_Vertices)) != 0)
			{
				LogError( __FILEW__, __LINE__, L"Buffer value compare failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
				return false;
			}
			pBufferCopy->Unmap();
		}
		else
		{
			LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
			return false;
		}

		hr = m_pCBuffers[i]->QueryInterface(__uuidof(ID3D10Buffer), (VOID**)&pBuffer);
		if(SUCCEEDED(hr))
		{
			pBuffer->GetDesc( &BufferDesc );
			pBuffer->Release();
		}
		else
		{
			WriteToLog( _T( "QueryInterface failed, hr = %s" ), D3DHResultToString( hr ).c_str() );
			return false;
		}

		if (!CompareBuffers(BufferDesc, m_cbufferDesc))
		{
			LogError( __FILEW__, __LINE__, L"CompareBuffers() failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			return false;
		}

		m_pD3DDevice->CopyResource( pBufferCopy, m_pCBuffers[i] );
		hr = pBufferCopy->Map(D3D10_MAP_READ, 0, (void**)&pMappedBuffer);
		if(SUCCEEDED(hr))
		{
			if (memcmp(pMappedBuffer, vertices, sizeof(m_Vertices)) != 0)
			{
				LogError( __FILEW__, __LINE__, L"Buffer value compare failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
				return false;
			}
			pBufferCopy->Unmap();
		}
		else
		{
			LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
			return false;
		}
	}

	// Check our vertex and constant buffers used in the UploadSubResource scenario
	for (UINT i = 0; i < RESOURCE_COUNT; i++)
	{
		hr = m_pBuffers2[i]->QueryInterface(__uuidof(ID3D10Buffer), (VOID**)&pBuffer);
		if(SUCCEEDED(hr))
		{
			pBuffer->GetDesc( &BufferDesc );
			pBuffer->Release();
		}
		else
		{
			WriteToLog( _T( "QueryInterface failed, hr = %s" ), D3DHResultToString( hr ).c_str() );
			return false;
		}

		if (!CompareBuffers(BufferDesc, m_bufferDesc2))
		{
			LogError( __FILEW__, __LINE__, L"CompareBuffers() failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			return false;
		}

		m_pD3DDevice->CopyResource( pBufferCopy, m_pBuffers2[i] );
		hr = pBufferCopy->Map(D3D10_MAP_READ, 0, (void**)&pMappedBuffer2);

		if(SUCCEEDED(hr))
		{
			for (UINT z=0; z<sizeof(pMappedBuffer2); z++)
			{
				if (pMappedBuffer2[z] != m_pTrimBufferData[z])
				{
					LogError( __FILEW__, __LINE__, L"Buffer value compare failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
					return false;
				}
			}
			pBufferCopy->Unmap();
		}
		else
		{
			LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
			return false;
		}

		hr = m_pCBuffers2[i]->QueryInterface(__uuidof(ID3D10Buffer), (VOID**)&pBuffer);
		if(SUCCEEDED(hr))
		{
			pBuffer->GetDesc( &BufferDesc );
			pBuffer->Release();
		}
		else
		{
			WriteToLog( _T( "QueryInterface failed, hr = %s" ), D3DHResultToString( hr ).c_str() );
			return false;
		}

		if (!CompareBuffers(BufferDesc, m_cbufferDesc2))
		{
			LogError( __FILEW__, __LINE__, L"CompareBuffers() failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			return false;
		}

		m_pD3DDevice->CopyResource( pBufferCopy, m_pCBuffers2[i] );
		hr = pBufferCopy->Map(D3D10_MAP_READ, 0, (void**)&pMappedBuffer2);

		if(SUCCEEDED(hr))
		{
			for (UINT z=0; z<sizeof(pMappedBuffer2); z++)
			{
				if (pMappedBuffer2[z] != m_pTrimCBufferData[z])
				{
					LogError( __FILEW__, __LINE__, L"Buffer value compare failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
					return false;
				}
			}
			pBufferCopy->Unmap();
		}
		else
		{
			LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
			return false;
		}
	}

	SAFE_RELEASE(Tex2D);
	SAFE_RELEASE(pBufferCopy);
	SAFE_RELEASE(pCopyTexture);

	return true;
}

bool CTrimMemoryTest::CompareTexture2D(D3D10_TEXTURE2D_DESC tex1, D3D10_TEXTURE2D_DESC tex2)
{
	if (tex1.Width != tex2.Width) return false;
	if (tex1.Height != tex2.Height) return false;
	if (tex1.MipLevels != tex2.MipLevels) return false;
	if (tex1.ArraySize != tex2.ArraySize) return false;
	if (tex1.Format != tex2.Format) return false;
	if (tex1.SampleDesc.Count != tex2.SampleDesc.Count) return false;
	if (tex1.SampleDesc.Quality != tex2.SampleDesc.Quality) return false;
	if (tex1.Usage != tex2.Usage) return false;
	if (tex1.BindFlags != tex2.BindFlags) return false;
	return true;
}

bool CTrimMemoryTest::CompareShaderRV(D3D10_SHADER_RESOURCE_VIEW_DESC srv1, D3D10_SHADER_RESOURCE_VIEW_DESC srv2)
{
	if (srv1.Format != srv2.Format) return false;
	if (srv1.ViewDimension != srv2.ViewDimension) return false;
	if (srv1.Texture2D.MostDetailedMip != srv2.Texture2D.MostDetailedMip) return false;
	if (srv1.Texture2D.MipLevels != srv2.Texture2D.MipLevels) return false;
	return true;
}

bool CTrimMemoryTest::CompareBuffers(D3D10_BUFFER_DESC buf1, D3D10_BUFFER_DESC buf2)
{
	if (buf1.ByteWidth != buf2.ByteWidth) return false;
	if (buf1.Usage != buf2.Usage) return false;
	if (buf1.BindFlags != buf2.BindFlags) return false;
	if (buf1.CPUAccessFlags != buf2.CPUAccessFlags) return false;
	if (buf1.MiscFlags != buf2.MiscFlags) return false;
	return true;
}

void CTrimMemoryTest::CleanupTestCase()
{
	SAFE_RELEASE(m_pQuad);
	SAFE_RELEASE(m_pTexture2D);
	SAFE_RELEASE(m_pTexture2D_Copy);
	SAFE_RELEASE(m_pTextureTrimMap);

	if (m_pTextures)
	{
		for (UINT i = 0; i < m_TextureCount; i++)
		{
			SAFE_RELEASE(m_pTextures[i]);
		}
	}
	SAFE_DELETE_ARRAY(m_pTextures);

	if (m_pTrimTextures)
	{
		for (UINT i = 0; i < m_TextureCount; i++)
		{
			SAFE_RELEASE(m_pTrimTextures[i]);
		}
	}

	if (m_pBuffers)
	{
		for (UINT i = 0; i < RESOURCE_COUNT; i++)
		{
			SAFE_RELEASE(m_pBuffers[i]);
		}
	}
	SAFE_DELETE_ARRAY(m_pBuffers);

	if (m_pCBuffers)
	{
		for (UINT i = 0; i < RESOURCE_COUNT; i++)
		{
			SAFE_RELEASE(m_pCBuffers[i]);
		}
	}
	SAFE_DELETE_ARRAY(m_pCBuffers);

	if (m_pBuffers2)
	{
		for (UINT i = 0; i < RESOURCE_COUNT; i++)
		{
			SAFE_RELEASE(m_pBuffers2[i]);
		}
	}
	SAFE_DELETE_ARRAY(m_pBuffers2);

	if (m_pCBuffers2)
	{
		for (UINT i = 0; i < RESOURCE_COUNT; i++)
		{
			SAFE_RELEASE(m_pCBuffers2[i]);
		}
	}
	SAFE_DELETE_ARRAY(m_pCBuffers2);

	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pEffect2);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_rasterizerState);
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pDepthStencil);
	SAFE_RELEASE(m_pAdapter);
	SAFE_RELEASE(m_pFactory);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pD3DDevice);
	
}

void CTrimMemoryTest::Cleanup()
{
	DestroyWindow(m_hWnd);
}