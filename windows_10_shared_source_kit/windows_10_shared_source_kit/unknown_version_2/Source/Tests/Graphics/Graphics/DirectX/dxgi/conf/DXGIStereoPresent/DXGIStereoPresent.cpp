#include "DXGIStereoPresent.h"
#include "TestApp.h"

#include "DirectX.h"

DXGI_FORMAT g_pFormatsToTest [] =
{
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_B8G8R8A8_UNORM,
	DXGI_FORMAT_B8G8R8X8_UNORM,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
};

FLOAT_COLOR g_pEyeColors [] =
{
	{1.0f, 0.0f, 0.0f, 1.0f},	// left eye
	{0.0f, 0.0f, 1.0f, 1.0f},	// right eye
	{0.0f, 1.0f, 0.0f, 1.0f},	// temporary mono negative case
	{0.0f, 1.0f, 1.0f, 1.0f},	// dirty rect left eye
	{1.0f, 1.0f, 0.0f, 1.0f},	// dirty rect right eye
	{1.0f, 1.0f, 1.0f, 1.0f}	// scroll rect border
};

bool operator ==(const DXGI_MODE_DESC1 &mode1, const DXGI_MODE_DESC1 &mode2)
{
	float refreshDelta = fabsf(((float)mode1.RefreshRate.Numerator/(float)mode1.RefreshRate.Denominator) - 
			((float)mode2.RefreshRate.Numerator/(float)mode2.RefreshRate.Denominator));
	
	return mode1.Width == mode2.Width &&
		mode1.Height == mode2.Height &&
		mode1.Format == mode2.Format &&
		mode1.ScanlineOrdering == mode2.ScanlineOrdering &&
		mode1.Scaling == mode2.Scaling &&
		mode1.Stereo == mode2.Stereo &&
		refreshDelta <= 1.0f;
}

bool operator !=(const DXGI_MODE_DESC1 &mode1, const DXGI_MODE_DESC1 &mode2)
{
	return !(mode1 == mode2);
}

CDXGIStereoPresent::CDXGIStereoPresent() :
	m_pFactory(NULL),
	m_pSwapChain(NULL),
	m_pOutput(NULL),
	m_pStagingBB(NULL),
	m_pSlicer(NULL),
	m_bTestDComp(FALSE)
{
	m_bWindowed = false;
	m_currentMode = 0;
	m_BufferCount = 2;
	m_SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	m_PresentFlags = 0;
	m_SyncInterval = 0;
	
	m_PresentParams.DirtyRectsCount = 0;
	m_PresentParams.pDirtyRects = NULL;
	m_PresentParams.pScrollRect = NULL;
	m_PresentParams.pScrollOffset = NULL;

	IDXGIAdapter1 *pAdapter = NULL;
	
	if(FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&m_pFactory)))
	{
		return;
	}
}

TEST_RESULT CDXGIStereoPresent::Setup()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = E_FAIL;

	WNDCLASS wndclass = {0};

	// Fail if factory failed to create during construction
	if(m_pFactory == NULL) 
	{
		WriteToLog(_T("Failed to create DXGI factory for enumerating modes during construction"));
		goto Fail;
	}

	SAFE_RELEASE(m_pFactory);

	m_pFactory = GetFramework()->GetDXGIFactory2();
	if(m_pFactory == NULL)
	{
		WriteToLog(_T("Failed to get DXGI factory from framework"));
		goto Fail;
	}

	m_hInstance = GetModuleHandle(NULL);

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = DefWindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = m_hInstance;
	wndclass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
	wndclass.hbrBackground = (HBRUSH)GetStockObject( NULL_BRUSH );
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = WINDOW_NAME;
	
	if(!RegisterClass(&wndclass))
	{
		WriteToLog(_T("RegisterClass() failed."));
		goto Fail;
	}
	
	InitializeRad();
	m_pSlicer = new DwmSlicer();

	goto Done;

Fail:
	tr = RESULT_FAIL;

Done:
	return tr;
}

TEST_RESULT CDXGIStereoPresent::SetupTestCase()
{
	HRESULT hr = E_FAIL;

	if(FAILED(hr = CreateTestWindow()))
	{
		WriteToLog(_T("Failed to create test window, hr = %s"), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	if(FAILED(hr = m_pFactory->MakeWindowAssociation(m_hWnd, 0)))
	{
		WriteToLog(_T("MakeWindowAssociation() failed, hr=%s"), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	if(FAILED(hr = CreateStereoSwapChain()))
	{
		WriteToLog(_T("Failed to create stereo swap chain, hr = %s"), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	if(m_pSwapChain->IsTemporaryMonoSupported() == FALSE && m_PresentFlags & DXGI_PRESENT_STEREO_TEMPORARY_MONO)
	{
		WriteToLog(_T("Skipping test case, hardware swap chain does not support temporary mono"));
		return RESULT_SKIP;
	}

	return RESULT_PASS;
}

TEST_RESULT CDXGIStereoPresent::ExecuteTestCase()
{
	HRESULT hr = E_FAIL;
	TEST_RESULT tr = RESULT_PASS;
	IDXGIOutput *pOutput = NULL;
	UINT numRects = 1;
	POINT scrollOffset;
	RECT scrollDirtyRects[2];

	RECT clientRect = {0};
	RECT captureRect = {0};

	const UINT width = m_supportedModes[m_currentMode].Width;
	const UINT height = m_supportedModes[m_currentMode].Height;

	SetCursorPos(width - 1, height - 1);
	ShowCursor(FALSE);

	if(m_bWindowed == FALSE)
	{
		if(FAILED(hr = m_pSwapChain->ResizeBuffers(
			m_BufferCount, 
			m_supportedModes[m_currentMode].Width,
			m_supportedModes[m_currentMode].Height,
			m_supportedModes[m_currentMode].Format,
			0)))
		{
			WriteToLog(_T("CDXGIStereoPresent::ExecuteTestCase - ResizeBuffers() failed, hr = %s"), D3DHResultToString(hr).c_str());
			goto Fail;
		}

		m_pSwapChain->SetFullscreenState(TRUE, NULL);

		BOOL bFullscreen;
		if(FAILED(hr = m_pSwapChain->GetFullscreenState(&bFullscreen, &pOutput)))
		{
			WriteToLog(_T("CDXGIStereoPresent::ExecuteTestCase - GetFullscreenState() failed, hr = %s"), D3DHResultToString(hr).c_str());
			goto Fail;
		}

		if(bFullscreen == FALSE || pOutput == NULL)
		{
			WriteToLog(_T("CDXGIStereoPresent::ExecuteTestCase - Swapchain failed to go fullscreen after SetFullscreenState()"));
			goto Fail;
		}

		if(FAILED(pOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)&m_pOutput)))
		{
			WriteToLog(_T("CDXGIStereoPresent::ExecuteTestCase - QueryInterface for IDXGIOutput1 failed, hr = %s"), D3DHResultToString(hr).c_str());
			goto Fail;
		}
	}

	if(FAILED(hr = InitializeStereoBackbuffer()))
	{
		WriteToLog(_T("CDXGIStereoPresent::ExecuteTestCase - Failed to initialize stereo backbuffer, hr = %s"), D3DHResultToString(hr).c_str());
		goto Fail;
	}

	m_PresentParams.DirtyRectsCount = 0;
	m_PresentParams.pDirtyRects = NULL;
	m_PresentParams.pScrollRect = NULL;
	m_PresentParams.pScrollOffset = NULL;
	
	if(m_bWindowed == TRUE)
	{
		// prepare DWM slicer

		//Calculate the capture rect
		GetClientRect(m_hWnd, &clientRect);
		captureRect = clientRect;

		m_capturableFrames = 0;

		m_pSlicer->Begin(captureRect, 
		             30,                 //max frames to capture
		             0,                  //frames to skip between captures
		             SlicerMode::Stereo,
					 &m_capturableFrames
		             );
	}

	if(FAILED(hr = m_pSwapChain->Present1(m_SyncInterval, m_PresentFlags, &m_PresentParams)))
	{
		WriteToLog(_T("CDXGIStereoPresent::ExecuteTestCase - Present1() failed, hr=%s"), D3DHResultToString(hr).c_str());
		goto Fail;	
	}

	if(m_bTestDirtyRects)
	{
		// determine dirty rect dimensions based on swap chain dimensions and test case
		switch(m_DirtyRectTest)
		{
		case DIRTY_RECT_TOP_LEFT:
			m_Rects[0].left = 0;
			m_Rects[0].top = 0;
			m_Rects[0].right = width / 2;
			m_Rects[0].bottom = height / 2;
			break;

		case DIRTY_RECT_BOTTOM_RIGHT:
			m_Rects[0].left = width / 2;
			m_Rects[0].top = height / 2;
			m_Rects[0].right = width;
			m_Rects[0].bottom = height;
			break;

		case DIRTY_RECT_INTERNAL:
			m_Rects[0].left = width / 2;
			m_Rects[0].top = height / 2;
			m_Rects[0].right = width - (width / 4);
			m_Rects[0].bottom = height - (height / 4);
			break;

		case DIRTY_RECT_PIXEL:
			m_Rects[0].left = width / 2;
			m_Rects[0].top = height / 2;
			m_Rects[0].right = width / 2 + 1;
			m_Rects[0].bottom = height / 2 + 1;
			break;

		case DIRTY_RECT_FULL:
			m_Rects[0].left = 0;
			m_Rects[0].top = 0;
			m_Rects[0].right = width;
			m_Rects[0].bottom = height;
			break;

		case DIRTY_RECT_MULTIPLE:
			numRects = 2;

			m_Rects[0].left = width / 2;
			m_Rects[0].top = height / 2;
			m_Rects[0].right = width - (width / 4);
			m_Rects[0].bottom = height - (height / 4);

			m_Rects[1].left = 0;
			m_Rects[1].top = 0;
			m_Rects[1].right = width / 3;
			m_Rects[1].bottom = height / 3;
			break;

		case DIRTY_RECT_SCROLL_BOTTOM_RIGHT:
			numRects = 2;

			m_Rects[0].left = 0;
			m_Rects[0].top = 0;
			m_Rects[0].right = width - 20;
			m_Rects[0].bottom = height - 20;

			scrollDirtyRects[0].left = 0;
			scrollDirtyRects[0].top = 0;
			scrollDirtyRects[0].right = 20;
			scrollDirtyRects[0].bottom = height;

			scrollDirtyRects[1].left = 20;
			scrollDirtyRects[1].top = 0;
			scrollDirtyRects[1].right = width;
			scrollDirtyRects[1].bottom = 20;

			m_ScrollOffset.x = -20;
			m_ScrollOffset.y = -20;

			break;

		case DIRTY_RECT_SCROLL_TOP_LEFT:
			numRects = 2;

			m_Rects[0].left = 20;
			m_Rects[0].top = 20;
			m_Rects[0].right = width;
			m_Rects[0].bottom = height;

			scrollDirtyRects[0].left = width - 20;
			scrollDirtyRects[0].top = 0;
			scrollDirtyRects[0].right = width;
			scrollDirtyRects[0].bottom = height;

			scrollDirtyRects[1].left = 0;
			scrollDirtyRects[1].top = height - 20;
			scrollDirtyRects[1].right = width - 20;
			scrollDirtyRects[1].bottom = height;

			m_ScrollOffset.x = 20;
			m_ScrollOffset.y = 20;

			break;
		}

		// set present params to use rects
		if(m_DirtyRectTest == DIRTY_RECT_SCROLL_BOTTOM_RIGHT || m_DirtyRectTest == DIRTY_RECT_SCROLL_TOP_LEFT)
		{
			m_PresentParams.DirtyRectsCount = numRects;
			m_PresentParams.pDirtyRects = scrollDirtyRects;
			m_PresentParams.pScrollRect = m_Rects;
			m_PresentParams.pScrollOffset = &m_ScrollOffset;
		}
		else
		{
			m_PresentParams.DirtyRectsCount = numRects;
			m_PresentParams.pDirtyRects = m_Rects;
			m_PresentParams.pScrollRect = NULL;
			m_PresentParams.pScrollOffset = NULL;
		}

		// change contents of backbuffer
		if(FAILED(hr = InitializeStereoBackbuffer(true)))
		{
			WriteToLog(_T("CDXGIStereoPresent::ExecuteTestCase - Failed to initialize stereo backbuffer for dirty rect present, hr = %s"), D3DHResultToString(hr).c_str());
			goto Fail;
		}

		// present again over existing contents
		if(FAILED(hr = m_pSwapChain->Present1(m_SyncInterval, m_PresentFlags, &m_PresentParams)))
		{
			WriteToLog(_T("CDXGIStereoPresent::ExecuteTestCase - Present1() failed for dirty rect present, hr=%s"), D3DHResultToString(hr).c_str());
			goto Fail;	
		}
	}

	Sleep(POST_PRESENT_SLEEP_TIME);
	
	if(m_bWindowed == TRUE)
	{
		Sleep(CAPTURE_TIME);

		// end slicing and prepare capture for validation

		//End capture
		m_captureAtlas = m_pSlicer->EndStereo(m_framesCaptured);
		
		if (m_framesCaptured == 0)
		{
		    WriteToLog(_T("CDXGIStereoPresent::ExecuteTestCase - Slicer failed to capture window, capturable frames = %i"), m_capturableFrames);
		    goto Fail;
		}
	}

	tr = ValidateResults();
	if(tr != RESULT_PASS)
	{
		goto Fail;
	}

	goto Done;

Fail:
	tr = RESULT_FAIL;
	LogMode();

Done:
	ShowCursor(TRUE);

	if(m_bWindowed == FALSE)
	{
		Sleep(FULLSCREEN_TRANSITION_SLEEP_TIME);

		m_pSwapChain->SetFullscreenState(FALSE, NULL);
		SAFE_RELEASE(pOutput);
	}

	return tr;
}

void CDXGIStereoPresent::CleanupTestCase()
{
	if(m_bWindowed == FALSE && m_pSwapChain != NULL)
	{
		BOOL bFullscreen = FALSE;
		IDXGIOutput *pOutput = NULL;
		
		m_pSwapChain->GetFullscreenState(&bFullscreen, &pOutput);
		
		if(bFullscreen == TRUE)
		{
			m_pSwapChain->SetFullscreenState(FALSE, NULL);
		}
	
		SAFE_RELEASE(pOutput);
	}

	SAFE_RELEASE(m_pStagingBB);
	SAFE_RELEASE(m_pSwapChain);

	if(m_hWnd != NULL)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

void CDXGIStereoPresent::Cleanup()
{
	// if slicer pointer is non-null then Rad was initialized
	if(m_pSlicer != NULL)
	{
		UninitializeRad();
	}

	SAFE_DELETE(m_pSlicer);
	SAFE_RELEASE(m_pOutput);

	UnregisterClass(WINDOW_NAME, m_hInstance);
}

HRESULT CDXGIStereoPresent::CreateTestWindow()
{
	HRESULT hr = S_OK;
	DWORD dwStyleEx = 0;

	DWORD dwStyle = WS_POPUP | WS_VISIBLE;

	//RECT clientRect = { CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT };
	DXGI_MODE_DESC1 currentMode = m_supportedModes[m_currentMode];
	RECT clientRect = { 0, 0, currentMode.Width, currentMode.Height };

	m_hWnd = CreateWindowEx
		(
		dwStyleEx,
		WINDOW_NAME,
		WINDOW_NAME,
		dwStyle,
		clientRect.left,
		clientRect.top,
		clientRect.right,
		clientRect.bottom,
		NULL,
		NULL,
		m_hInstance,
		NULL
		);

	if (m_hWnd == NULL)
	{
		DWORD error = GetLastError();
		hr = (ERROR_SUCCESS == error) ? E_FAIL : HRESULT_FROM_WIN32(hr);
	}

	GetClientRect(m_hWnd, &clientRect);
	WriteToLog(_T("CreateTestWindow: Client Rect (left:%d,top:%d,width:%d,height:%d) "), 
        clientRect.left,
        clientRect.top,
        clientRect.right,
        clientRect.bottom);

	if(m_hWnd == NULL)
	{
		hr = E_INVALIDARG;
		goto Done;
	};

	// Start by making sure that the window is visible.
	ShowWindow(m_hWnd, SW_SHOWNORMAL);

	UpdateWindow(m_hWnd);

	ProcessMessages();

	SetForegroundWindow(m_hWnd);

	SetWindowPos
	(
		m_hWnd,
		HWND_TOP,
		0,
		0,
		0,
		0,
		SWP_NOSIZE | SWP_NOMOVE
	);

	// Now see whether the window is already at the top of the
	// z-order.

	HWND hForegroundWindow = GetForegroundWindow();

	if(hForegroundWindow == m_hWnd)
	{
		hr = S_OK;
	};

Done:
	return hr;
}

void CDXGIStereoPresent::LogMode()
{
	DXGI_MODE_DESC1 mode = m_supportedModes[m_currentMode];

	WriteToLog(_T("Mode information:\nFormat=%s\nWidth=%i\nHeight=%i\nRefreshRate=%f\nScanlineOrdering=%s\nScaling=%s\nStereo=%s\n"),
			ToString(mode.Format).c_str(),
			mode.Width,
			mode.Height,
			(float)mode.RefreshRate.Numerator / (float)mode.RefreshRate.Denominator,
			ToString(mode.ScanlineOrdering).c_str(),
			ToString(mode.Scaling).c_str(),
			mode.Stereo == TRUE ? "TRUE" : "FALSE");
}

void CDXGIStereoPresent::ProcessMessages()
{
	MSG msg;

	while( PeekMessage( &msg,NULL,0,0,PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );

		// If this is a quit message, re-post it.
		if( WM_QUIT == msg.message )
		{
			PostQuitMessage( (int)msg.wParam );
		}
	}
}

HRESULT CDXGIStereoPresent::InitializeStereoBackbuffer(const bool &bForDirtyRectPresent)
{
	HRESULT hr = S_OK;
	ID3D11Texture2D *pBBTex = NULL;
	BYTE *eyeData = NULL;

	UINT width = m_supportedModes[m_currentMode].Width;
	UINT height = m_supportedModes[m_currentMode].Height;

	D3D11_TEXTURE2D_DESC texDesc;

	// assuming this function will have been called once before when called for dirty rect case
	if(bForDirtyRectPresent == false)
	{
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 2;
		texDesc.Format = m_supportedModes[m_currentMode].Format;
		texDesc.SampleDesc.Quality = 0;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_STAGING;
		texDesc.BindFlags = 0;
		texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		texDesc.MiscFlags = 0;

		if(FAILED(hr = GetDevice()->CreateTexture2D(&texDesc, NULL, &m_pStagingBB)))
		{
			WriteToLog(_T("CDXGIStereoPresent::InitializeStereoBackbuffer - Failed to created expected result texture, hr=%s"), D3DHResultToString(hr).c_str());
			goto Done;
		}
	}

	const UINT elementSize = GetBitsPerElement(m_supportedModes[m_currentMode].Format) / 8;

	eyeData = new BYTE[elementSize * NUM_TEST_ELEMENTS]; 

	if(!CFormatHelper::ConvertPixels(CONV_TO_FORMAT, m_supportedModes[m_currentMode].Format, g_pEyeColors, 1, NUM_TEST_ELEMENTS, eyeData))
	{
		WriteToLog(_T("CDXGIStereoPresent::InitializeStereoBackbuffer - Failed to convert expected data"));
		goto Done;
	}

	for(UINT i = 0; i < 2; ++i)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubres;
		if(FAILED(hr = GetEffectiveContext()->Map(m_pStagingBB, i, D3D11_MAP_WRITE, 0, &mappedSubres)))
		{
			WriteToLog(_T("CDXGIStereoPresent::InitializeStereoBackbuffer - Failed to map expected result texture, hr=%s"), D3DHResultToString(hr).c_str());
			goto Done;
		}

		UINT idx = i;
		if(i == 1 && m_PresentFlags & DXGI_PRESENT_STEREO_TEMPORARY_MONO)
		{
			idx = 2;	
		}
		else if(bForDirtyRectPresent)
		{
			idx += 3;
		}

		BYTE *pData = (BYTE*)mappedSubres.pData;
		for(UINT y = 0; y < height; ++y)
		{
			for(UINT x = 0; x < width; ++x)
			{
				memcpy(pData + x * elementSize, &eyeData[idx * elementSize], elementSize); 
			}

			pData += mappedSubres.RowPitch;
		}

		GetEffectiveContext()->Unmap(m_pStagingBB, i);
	}

	if(FAILED(hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBBTex)))
	{
		WriteToLog(_T("CDXGIStereoPresent::InitializeStereoBackbuffer - GetBuffer() failed on swap chain, hr=%s"), D3DHResultToString(hr).c_str());
		goto Done;
	}

	GetEffectiveContext()->CopyResource(pBBTex, m_pStagingBB);

	SAFE_RELEASE(pBBTex);

Done:
	if(eyeData != NULL)
	{
		delete [] eyeData;
		eyeData = NULL;
	}

	return hr;
}
