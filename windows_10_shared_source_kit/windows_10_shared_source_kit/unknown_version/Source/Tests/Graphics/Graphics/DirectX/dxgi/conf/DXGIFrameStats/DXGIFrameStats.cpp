//-----------------------------------------------------------------------------
// File: DXGIFrameStats.cpp
//
// Desc: DXGI GetFrameStatistics tests.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
// Project headers
#include <initguid.h>
#include "DXGIFrameStats.h"

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
// Singleton application object need by test framework.
CMyTestApp g_Application;

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro: HANDLE_HRESULT
// Desc: Expands to a block of code like the following:
//          case D3DERR_DEVICELOST:
//          {
//              return _T( "D3DERR_DEVICELOST" );
//          } break
//-----------------------------------------------------------------------------
#define HANDLE_HRESULT(x) case x:{ return _T(#x); } break

//-----------------------------------------------------------------------------
// Macro: LOG_RETURN_ON_NULL
// Desc: Helper to write to log and return on NULL
//-----------------------------------------------------------------------------
#define LOG_RETURN_ON_NULL( p, rt, msg) if(NULL == p){WriteToLog(msg); return rt;}

//-----------------------------------------------------------------------------
// Macro: LOG_RETURN_ON_NULL
// Desc: Helper to return on HRESULT FAILURE
//-----------------------------------------------------------------------------
#define RETURN_ON_HRESULT_FAILURE(hr, rt) {if(FAILED(hr)) {return rt;}}

//-----------------------------------------------------------------------------
// The window hellper class (CDXGISwapChainWindow)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function: CreateTestWindow
// Desc: Create Test window
//-----------------------------------------------------------------------------
HRESULT CDXGISwapChainWindow::CreateTestWindow(RECT* pRect)
{
    RECT rect = { 0, 0, 640, 480 };
    if( NULL != pRect )
    {
        rect = *pRect;
    }

    if( NULL != m_hWnd )
        return E_FAIL;

    // Create/Register window class
    WNDCLASS WinCls = { 0, MsgProc, 0, sizeof(this),
        GetModuleHandle(0), NULL, NULL,
        (HBRUSH)(COLOR_WINDOW + 1),
        NULL, _T("DXGISwapChainWindowClass") };
    if( 0 == RegisterClass(&WinCls) )
    {
        DWORD RegisterClassError = GetLastError();
        if( ERROR_CLASS_ALREADY_EXISTS != RegisterClassError )
        {
            return E_FAIL;
        }
    }

    AdjustWindowRectEx( &rect, WS_OVERLAPPED, FALSE, 0);

    m_hWnd = CreateWindowEx( WS_EX_APPWINDOW,
        _T("DXGISwapChainWindowClass"),
        _T("DXGI Test Window"),
        WS_OVERLAPPED | WS_VISIBLE,
        0, 0,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL, NULL,
        GetModuleHandle(0), (LPVOID)(this) );

    if( NULL != m_hWnd )
    {
        ShowWindow(m_hWnd, TRUE);
        SetWindowPos( m_hWnd, HWND_NOTOPMOST,
            0, 0, 
            rect.right - rect.left,
            rect.bottom - rect.top,
            0);
        SetWindowPos( m_hWnd, HWND_NOTOPMOST,
            0, 0, 
            rect.right - rect.left,
            rect.bottom - rect.top,
            0);

        return S_OK;
    }

    return E_FAIL;
}

//-----------------------------------------------------------------------------
// The Tests Baseclass (CDXGIFrameStatsTest)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function: IncrementPresentCount
// Desc: Increments the present count for pDXGISwapChain
//-----------------------------------------------------------------------------
HRESULT CDXGIFrameStatsTest::IncrementPresentCount(IDXGISwapChain* pDXGISwapChain)
{
    HRESULT hr = E_FAIL;

    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, E_INVALIDARG, _T("IncrementPresentCount: Invalid input."));

    // Verify we have a render target view
    LOG_RETURN_ON_NULL(m_pRTView, E_FAIL, _T("IncrementPresentCount: No Render Target view."));

    // Verify we have access to a d3ddevice
    LOG_RETURN_ON_NULL(GetD3DDevice(), E_FAIL, _T("IncrementPresentCount: Unable to get D3DDevice from test framework."));  

    // Get a test resource
    LOG_RETURN_ON_NULL(m_pTestResource, E_FAIL, _T("IncrementPresentCount: No test resource to use."));  

    // Clear render target view
    FLOAT fRGBA[4] = { 0.0f, 0.0f, 0.5f, 1.0f };
    GetD3DDevice()->ClearRenderTargetView(m_pRTView, fRGBA);

    // Render test resource
    if( FALSE ==  m_pTestResource->UseResource(GetD3DDevice()) )
    {
        WriteToLog(_T("IncrementPresentCount: Use test resource failed."));
        return hr;
    }

    // Present.
    hr = pDXGISwapChain->Present(0,0);
    LogOnFailure(hr, _T("IncrementPresentCount: DXGISwapChain->Present() failed."));

    // Handle occlusion.
    if( DXGI_STATUS_OCCLUDED == hr )
    {
        WriteToLog(_T("IncrementPresentCount: Test is occluded."));
        return hr;
    }

    // Handle Present failure.
    if( FAILED( hr ) )
    {
        // Log more detail on device removal.
        if( hr == DXGI_ERROR_DEVICE_REMOVED )
        {
            WriteToLog(_T("IncrementPresentCount: GetDeviceRemovedReason reports %s."), D3DHResultToString(GetD3DDevice()->GetDeviceRemovedReason()).c_str());
        }

        return hr;
    }

    if( FALSE == GetOwnByFullScreen() ) // If windowed mode, FLIP_SQUENTIAL is used. Must re-bind render target after present
        GetD3DDevice()->OMSetRenderTargets(1, &m_pRTView, NULL);

    hr = S_OK;

    return hr;
}

//-----------------------------------------------------------------------------
// Function: CreateTestResource
// Desc:  Create the test resource
//-----------------------------------------------------------------------------
HRESULT CDXGIFrameStatsTest::CreateTestResource()
{
    HRESULT hr = E_FAIL;

    ReleaseTestResource();

    switch( m_CurrentTestResource )
    {
    case D3D10VERTEXBUFFER:
        {
            m_pTestResource = new CDXGIResource_D3D10VB;
            break;
        }

    default:
        {
            WriteToLog(_T("CreateTestResource: Unknown resource type"));
            break;
        }
    }

    if( NULL == m_pTestResource )
    {
        WriteToLog(_T("CreateTestResource: Out of memory"));
        return E_FAIL;
    }

    // Create resource
    hr = m_pTestResource->CreateResource(GetD3DDevice(), GetFramework() );
    RETURN_ON_HRESULT_FAILURE(LogOnFailure(hr, _T("CreateTestResource: Unable to create test resource")), RESULT_FAIL);

    return hr;
}

//-----------------------------------------------------------------------------
// Function: ReleaseTestResource
// Desc: ReleaseTestResource
//-----------------------------------------------------------------------------
void CDXGIFrameStatsTest::ReleaseTestResource()
{
    if( NULL != m_pTestResource )
    {
        delete m_pTestResource;
        m_pTestResource = NULL;
    }
}

//-----------------------------------------------------------------------------
// Function: SetupDWMStatus
// Desc: Setup DWM status flags 
//-----------------------------------------------------------------------------
BOOL CDXGIFrameStatsTest::SetupDWMStatus()
{
    BOOL rt = FALSE;
    HMODULE hdwm = NULL;
    PFDWMISCOMPOSITIONENABLED  pfnDwmIsCompositionEnabled = NULL;

    // Load DWMAPI
    hdwm = LoadLibraryExW(L"dwmapi.dll", NULL, 0);
    if( NULL == hdwm)
    {
        WriteToLog(_T("SetupDWMStatus: Failed to load dwmapi.dll."));
        goto cleanup;
    }

    // get funk pointer
    pfnDwmIsCompositionEnabled = (PFDWMISCOMPOSITIONENABLED)GetProcAddress(hdwm, "DwmIsCompositionEnabled");
    if( NULL == pfnDwmIsCompositionEnabled )
    {
        WriteToLog(_T("SetupDWMStatus: Failed to find DwmIsCompositionEnabled in dwmapi.dll."));
        goto cleanup;
    }

    if( FAILED(pfnDwmIsCompositionEnabled(&m_bDWMCompositionEnabled)) )
    {
        WriteToLog(_T("SetupDWMStatus: DwmIsCompositionEnabled Faild."));
        goto cleanup;
    }

    rt = TRUE;

cleanup:

    if( NULL != hdwm )
    {
        FreeLibrary(hdwm);
    }

    return rt;
}

//-----------------------------------------------------------------------------
// Function: TakeOwnership
// Desc:  Takes ownership over the output
//-----------------------------------------------------------------------------
HRESULT CDXGIFrameStatsTest::TakeOwnership(IDXGISwapChain* pDXGISwapChain, IDXGIOutput* pDXGIOutput)
{
    HRESULT hr = E_FAIL;
    IDXGIOutput* pOutput = NULL;

    if( TRUE == GetOwnByFullScreen() )
    {
        // Verify input
        LOG_RETURN_ON_NULL(pDXGISwapChain, E_INVALIDARG, _T("TakeOwnership: Invalid input"));

        // Take ownership
        if( NULL == pDXGIOutput )
        {   
            // Get IDXGIOutput
            hr = GetSwapChainOutput(pDXGISwapChain, &pOutput);
            RETURN_ON_HRESULT_FAILURE(LogOnFailure(hr, _T("TakeOwnership: GetSwapChainOutput failed")), RESULT_FAIL);

            // Set full screen
            hr = pDXGISwapChain->SetFullscreenState(TRUE, pOutput);
        }
        else
        {
            // Set full screen
            hr = pDXGISwapChain->SetFullscreenState(TRUE, pDXGIOutput);
        }

        LogOnFailure(hr, _T("TakeOwnership: DXGISwapChain->SetFullscreenState(TRUE) failed") );

        if( NULL != pOutput )
        {
            pOutput->Release();
            pOutput = NULL;
        }
    }
    // Don't need to take ownership for windowed mode.  Apps should never
    // call TakeOwnership directly.   TakeOwnership should have been an 
    // internal function, or at least have more limited exposure.
    //else
    //{
    //    // Verify input
    //    LOG_RETURN_ON_NULL(pDXGIOutput, E_INVALIDARG, _T("TakeOwnership: Invalid input"));
    //
    //    hr = pDXGIOutput->TakeOwnership(GetD3DDevice(), FALSE);
    //    LogOnFailure(hr, _T("TakeOwnership: DXGIOutput->TakeOwnership(FALSE) failed") );
    //}
    else
    {
        hr = S_OK;
    }

    if( SUCCEEDED(hr) )
        m_bOwnedResource = TRUE;

    return hr;
}

//-----------------------------------------------------------------------------
// Function: ReleaseOwnership
// Desc:  Release ownership over the output
//-----------------------------------------------------------------------------
HRESULT CDXGIFrameStatsTest::ReleaseOwnership(IDXGISwapChain* pDXGISwapChain, IDXGIOutput* pDXGIOutput)
{
    HRESULT hr = E_FAIL;

    if( FALSE == m_bOwnedResource )
    {
        WriteToLog(_T("ReleaseOwnership: Don't have ownership"));
        return E_FAIL;
    }

    if( TRUE == GetOwnByFullScreen() )
    {
        // Verify input
        LOG_RETURN_ON_NULL(pDXGISwapChain, E_INVALIDARG, _T("ReleaseOwnership: Invalid input"));

        // Release ownership
        hr = pDXGISwapChain->SetFullscreenState(FALSE, NULL);

        LogOnFailure(hr, _T("ReleaseOwnership: DXGISwapChain->SetFullscreenState(FALSE) failed") );
    }
    else
    {
        // Verify input
        LOG_RETURN_ON_NULL(pDXGIOutput, E_INVALIDARG, _T("ReleaseOwnership: Invalid input"));

        // Release ownership
        pDXGIOutput->ReleaseOwnership();

        hr = S_OK;
    }

    if( SUCCEEDED(hr) )
        m_bOwnedResource = FALSE;

    return hr;
}

//-----------------------------------------------------------------------------
// Function: GetSwapChainOutput
// Desc: DXGISwapChain->GetContainingOutput()
//-----------------------------------------------------------------------------
HRESULT CDXGIFrameStatsTest::GetSwapChainOutput(IDXGISwapChain* pDXGISwapChain, IDXGIOutput** ppDXGIOutput)
{
    HRESULT hr = E_FAIL;

    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, E_INVALIDARG, _T("GetSwapChainOutput: Invalid input (pDXGISwapChain = NULL)"));
    LOG_RETURN_ON_NULL(ppDXGIOutput, E_INVALIDARG, _T("GetSwapChainOutput: Invalid input( ppDXGIOutput = NULL)"));
    if( NULL != *ppDXGIOutput )
    {
        WriteToLog(_T("GetSwapChainOutput: Invalid input(*ppDXGIOutput != NULL)"));
        return E_INVALIDARG;
    }

    // GetContainingOutput
    hr = pDXGISwapChain->GetContainingOutput(ppDXGIOutput);
    if( FAILED(hr) )
    {
        IDXGIOutput* pOutput = *ppDXGIOutput;
        if( NULL != *ppDXGIOutput )
        {
            pOutput->Release();
            pOutput = NULL;
        }
        WriteToLog(_T("GetSwapChainOutput: DXGISwapChain->GetContainingOutput() failed with %s"), 
            HResultToString(hr));

        return hr;
    }

    // Verify pOutput
    LOG_RETURN_ON_NULL(*ppDXGIOutput, hr, _T("GetSwapChainOutput: DXGISwapChain->GetContainingOutput succeeded but Output is NULL" ));

    return hr;
}

//-----------------------------------------------------------------------------
// Function: GetFrameStatistics
// Desc: GetFrameStatistics() for current resourec type being tested
//-----------------------------------------------------------------------------
HRESULT CDXGIFrameStatsTest::GetFrameStatistics(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput, DXGI_FRAME_STATISTICS* pFrameStats)
{
    HRESULT hr = E_FAIL;

    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, E_INVALIDARG, _T("GetFrameStatistics: Invalid input (pDXGISwapChain = NULL)"));
    LOG_RETURN_ON_NULL(pDXGIOutput, E_INVALIDARG, _T("GetFrameStatistics: Invalid input (pDXGIOutput = NULL)"));

    switch( GetCurrentResourceType() )
    {
    case DXGIOUTPUT:
        hr = pDXGIOutput->GetFrameStatistics(pFrameStats);
        LogOnFailure(hr, _T("GetFrameStatistics: IDXGIOutput->GetFrameStatistics() failed") );
        break;

    case DXGISWAPCHAIN:
        hr = pDXGISwapChain->GetFrameStatistics(pFrameStats);
        LogOnFailure(hr, _T("GetFrameStatistics: IDXGISwapChain->GetFrameStatistics() failed") );
        break;

    default:
        hr = E_INVALIDARG;
        WriteToLog(_T("GetFrameStatistics: Unknown resource type"));
        break;
    }

    //    WriteToLog("   Frame Stats - PresentCount: %u, PresentRefreshCount: %u, SyncRefreshCount: %u, SyncQPCTime: %ld, SyncGPUTime: %ld", 
    //                pFrameStats->PresentCount, pFrameStats->PresentRefreshCount, pFrameStats->SyncRefreshCount, pFrameStats->SyncQPCTime, pFrameStats->SyncGPUTime);

    return hr;
}

//-----------------------------------------------------------------------------
// Function: GetFrameStatistics
// Desc: GetFrameStatistics() for current resourec type being tested
//-----------------------------------------------------------------------------
HRESULT CDXGIFrameStatsTest::GetLastPresentCount(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput, UINT* pLastPresentCount)
{
    HRESULT hr = E_FAIL;

    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, E_INVALIDARG, _T("GetLastPresentCount: Invalid input (pDXGISwapChain = NULL)"));

    switch( GetCurrentResourceType() )
    {
    case DXGISWAPCHAIN:
        hr = pDXGISwapChain->GetLastPresentCount(pLastPresentCount);
        LogOnFailure(hr, _T("GetLastPresentCount: IDXGISwapChain->GetFrameStatistics() failed") );
        break;

    default:
        hr = E_INVALIDARG;
        WriteToLog(_T("GetLastPresentCount: Unknown resource type"));
        break;
    }

    return hr;
}

//-----------------------------------------------------------------------------
// Function: HResultToString
// Desc: Converts HRESULT to string
//-----------------------------------------------------------------------------
LPCTSTR CDXGIFrameStatsTest::HResultToString( IN HRESULT hResult ) const
{
    switch ( hResult )
    {
        HANDLE_HRESULT( S_OK );
        HANDLE_HRESULT( DXGI_STATUS_OCCLUDED );
        HANDLE_HRESULT( DXGI_STATUS_CLIPPED );
        HANDLE_HRESULT( DXGI_STATUS_NO_REDIRECTION );

        HANDLE_HRESULT( DXGI_ERROR_DEVICE_HUNG );
        HANDLE_HRESULT( DXGI_ERROR_DEVICE_RESET );
        HANDLE_HRESULT( DXGI_ERROR_DRIVER_INTERNAL_ERROR );
        HANDLE_HRESULT( DXGI_ERROR_DEVICE_REMOVED );
        HANDLE_HRESULT( DXGI_ERROR_INVALID_CALL );
        HANDLE_HRESULT( DXGI_ERROR_UNSUPPORTED );
        HANDLE_HRESULT( DXGI_ERROR_NOT_FOUND );
        HANDLE_HRESULT( DXGI_ERROR_MORE_DATA );
        HANDLE_HRESULT( DXGI_ERROR_WAS_STILL_DRAWING );
        HANDLE_HRESULT( DXGI_ERROR_NONEXCLUSIVE );
        HANDLE_HRESULT( DXGI_ERROR_FRAME_STATISTICS_DISJOINT );

        HANDLE_HRESULT( E_INVALIDARG );
        HANDLE_HRESULT( E_POINTER );
        HANDLE_HRESULT( E_FAIL );
        HANDLE_HRESULT( E_OUTOFMEMORY );

        HANDLE_HRESULT( E_NOTIMPL );
        HANDLE_HRESULT( E_NOINTERFACE );

        HANDLE_HRESULT( E_PENDING );

    default:
        return _T( "Unknown" );
        break;
    }
}

//-----------------------------------------------------------------------------
// Function: LogOnFailure
// Desc: Logs a message and HRESULT on failure
//-----------------------------------------------------------------------------
HRESULT CDXGIFrameStatsTest::LogOnFailure( IN HRESULT hResult, const TCHAR *szText  )
{   
    if( (FAILED(hResult) || DXGI_STATUS_OCCLUDED == hResult )&& NULL != szText)
    {
        WriteToLog(_T("%s HRESULT(%s),0x%x"), szText, HResultToString(hResult), hResult);
    }

    return hResult;
}

//-----------------------------------------------------------------------------
// Function: InitTestParameters
// Desc: Init the parameter for the test framework to iterate.
//-----------------------------------------------------------------------------
void CDXGIFrameStatsTest::InitTestParameters()
{

    // Fullscreen vs. Windowed mode factors
    //
    // Need to create seperate factors in order to apply different DCOMP factors.
    //
    CUserValueSet<BOOL> *pUseFullScreenValues = new CUserValueSet<BOOL>();
    pUseFullScreenValues->AddValue(TRUE);
    RFactor FullscreenFactor = AddParameter<BOOL>(_T("FullScreen"), &m_bOwnByFullScreen, pUseFullScreenValues);

    CUserValueSet<BOOL> *pUseWindowedValues = new CUserValueSet<BOOL>();
    pUseWindowedValues->AddValue(FALSE); // Window mode support added in Windows 8 (At least for Flip model swap effects).
    RFactor WindowedModeFactor = AddParameterValueSet<BOOL>(_T("FullScreen"), pUseWindowedValues);


    // Windowed vs. Windowless(DCOMP) SwapChain factors
    // Windowless(DCOMP) only supported in Windowed mode.  (Yeah... I know how funny that last sentence sounds)
    CUserValueSet<BOOL> *pDCOMPWindowed = new CUserValueSet<BOOL>();
    pDCOMPWindowed->AddValue(TRUE);
    pDCOMPWindowed->AddValue(FALSE);
    RFactor DCOMPWindowedFactor = AddParameter<BOOL>(_T("UseDCOMP"), &m_tp_bDCOMP, pDCOMPWindowed);

    CUserValueSet<BOOL> *pDCOMPFullscreen = new CUserValueSet<BOOL>();
    pDCOMPFullscreen->AddValue(FALSE);
    RFactor DCOMPFullscreenFactor = AddParameterValueSet<BOOL>(_T("UseDCOMP"), pDCOMPFullscreen);

    // The resource type
    CUserValueSet<TESTS_RESOURCETYPE> *pResourceTypeValues = new CUserValueSet<TESTS_RESOURCETYPE>();
    //pResourceTypeValues->AddValue(DXGIOUTPUT);
    pResourceTypeValues->AddValue(DXGISWAPCHAIN);
    RFactor ResourceFactors = AddParameter<TESTS_RESOURCETYPE>(_T("ResourceType"), &m_ResourceType, pResourceTypeValues);

    // Don't test DCOMP on Win7+8IP and below
    OSVERSIONINFO versionInfo;  
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);  
    GetVersionEx(&versionInfo);  
    if(versionInfo.dwMajorVersion < 6 || (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion < 2) )
    {
        SetRootTestFactor(ResourceFactors * ((FullscreenFactor * DCOMPFullscreenFactor) + (WindowedModeFactor * DCOMPFullscreenFactor)));
    }
    else
    {
        SetRootTestFactor(ResourceFactors * ((FullscreenFactor * DCOMPFullscreenFactor) + (WindowedModeFactor * DCOMPWindowedFactor)));
    }

}

//-----------------------------------------------------------------------------
// Function: InitNegativeTestParameters
// Desc: Init very limited parameters for negative tests.
//-----------------------------------------------------------------------------
void CDXGIFrameStatsTest::InitNegativeTestParameters()
{

    //
    // Run in fullscreen only
    //
    CUserValueSet<BOOL> *pUseFullScreenValues = new CUserValueSet<BOOL>();
    pUseFullScreenValues->AddValue(TRUE);
    RFactor FullscreenFactor = AddParameter<BOOL>(_T("FullScreen"), &m_bOwnByFullScreen, pUseFullScreenValues);

    //
    // Don't use DCOMP
    //
    CUserValueSet<BOOL> *pDCOMPWindowed = new CUserValueSet<BOOL>();
    pDCOMPWindowed->AddValue(FALSE);
    RFactor DCOMPWindowedFactor = AddParameter<BOOL>(_T("UseDCOMP"), &m_tp_bDCOMP, pDCOMPWindowed);

    //
    // The resource type
    //
    CUserValueSet<TESTS_RESOURCETYPE> *pResourceTypeValues = new CUserValueSet<TESTS_RESOURCETYPE>();
    //pResourceTypeValues->AddValue(DXGIOUTPUT);
    pResourceTypeValues->AddValue(DXGISWAPCHAIN);
    RFactor ResourceFactors = AddParameter<TESTS_RESOURCETYPE>(_T("ResourceType"), &m_ResourceType, pResourceTypeValues);


    SetRootTestFactor(ResourceFactors * FullscreenFactor * DCOMPWindowedFactor);
}


//-----------------------------------------------------------------------------
// Function: ExecuteTestCase
// Desc: Setups the test window, Swapchain, and calls RunCase() for each output
//-----------------------------------------------------------------------------
TEST_RESULT CDXGIFrameStatsTest::ExecuteTestCase()
{
    HRESULT hr = E_FAIL;
    TEST_RESULT tr = RESULT_FAIL;

    RECT winrect = { 0, 0, 640, 480 };

    CDXGISwapChainWindow myWindow;

    IDXGIFactory2* pDXGIFactory2 = NULL;

    IDXGISwapChain* pDXGISwapChain = NULL;

    IDXGIAdapter* pDXGIAdapter = NULL;

    IDXGIDevice* pDXGIDevice = NULL;


    // Verify test framework
    LOG_RETURN_ON_NULL(GetFramework(), RESULT_FAIL, _T("ExecuteTestCase: Unable to get test framework"));

    // Verify getting a DXGIFactory from the test framework
    LOG_RETURN_ON_NULL(GetDXGIFactory(), RESULT_FAIL, _T("ExecuteTestCase: Unable to get a DXGIFactory from test framework"));

    // Verify getting a D3D10Device from the test framework
    LOG_RETURN_ON_NULL(GetD3DDevice(), RESULT_FAIL, _T("ExecuteTestCase: Unable to get a D3D10Device from test framework"));

	// Need DWM
    if(FALSE == IsDWMCompositionEnabled() && GetOwnByFullScreen() == FALSE)
    {
        WriteToLog(_T("ExecuteTestCase: DWM composition is not enabled at start of test."));
        WriteToLog(_T("ExecuteTestCase: Frame statistics are not supported for windowed swap chains when DWM composition is disabled."));

        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Get a window
    hr = myWindow.CreateTestWindow(&winrect);
    RETURN_ON_HRESULT_FAILURE(LogOnFailure(hr, _T("ExecuteTestCase: CDXGISwapChainWindow->CreateTestWindow() failed")), RESULT_FAIL);
    LOG_RETURN_ON_NULL(myWindow.GetTestWindow(), RESULT_FAIL, _T("ExecuteTestCase: CDXGISwapChainWindow->CreateTestWindow() succeeded but can't get HWND"));
    SetTestWindow(myWindow.GetTestWindow());

    // Get DXGIDevice from framework
    hr = GetD3DDevice()->QueryInterface( IID_IDXGIDevice, (void**)&pDXGIDevice);
    RETURN_ON_HRESULT_FAILURE(LogOnFailure(hr, _T("ExecuteTestCase: D3D10Device->QueryInterface(IID_IDXGIDevice) failed")), RESULT_FAIL);
    LOG_RETURN_ON_NULL(pDXGIDevice, RESULT_FAIL, _T("ExecuteTestCase: D3D10Device->QueryInterface(IID_IDXGIDevice) succeeded but pDXGIDevice = NULL."));

    // Get the DXGIAdapter
    hr = pDXGIDevice->GetAdapter(&pDXGIAdapter);
    RETURN_ON_HRESULT_FAILURE(LogOnFailure(hr, _T("ExecuteTestCase: DXGIDevice->GetAdapter() failed")), RESULT_FAIL);
    LOG_RETURN_ON_NULL(pDXGIAdapter, RESULT_FAIL, _T("ExecuteTestCase: DXGIDevice->GetAdapter() succeeded but pDXGIAdapter = NULL."));

    // Get the DXGISwapChain
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(scd));
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.SampleDesc.Count = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT|DXGI_USAGE_BACK_BUFFER;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    if (m_tp_bDCOMP) // Should we use Windowless (DCOMP) presents?
    {
        // DCOMP Presentation
        scd.OutputWindow = NULL; 
        scd.BufferDesc.Width = 640; // We must specify buffer width and height with Windowless swapchains.
        scd.BufferDesc.Height = 480;
    }
    else // Regular windowed presentation
    {        
        scd.OutputWindow = myWindow.GetTestWindow();
        // No need to specify buffer dimensions.   DXGI will use the window client area for default dimensions.
    }

    scd.BufferCount = 2;

    if(FALSE == GetOwnByFullScreen() ) // If windowed mode, use FLIP_SEQUENTIAL (defaults to DISCARD for fullscreen).
        scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

    if( m_tp_bDCOMP )
    {
        DXGI_SWAP_CHAIN_DESC1 scd1;
        ZeroMemory(&scd1,sizeof(scd1));
        scd1.Width = scd.BufferDesc.Width;
        scd1.Height = scd.BufferDesc.Height;
        scd1.Format = scd.BufferDesc.Format;
        scd1.Stereo = FALSE;
        scd1.BufferCount = scd.BufferCount;
        scd1.BufferUsage = scd.BufferUsage;    
        scd1.SampleDesc.Count = scd.SampleDesc.Count;
        scd1.SampleDesc.Quality = scd.SampleDesc.Quality;
        scd1.Scaling = DXGI_SCALING_STRETCH;
        scd1.SwapEffect = scd.SwapEffect;
        scd1.Flags = 0;

        hr = GetDXGIFactory()->QueryInterface(__uuidof(IDXGIFactory2),(void**)&pDXGIFactory2);
        RETURN_ON_HRESULT_FAILURE(LogOnFailure(hr, _T("ExecuteTestCase: DXGIFactory->QueryInterface() for IDXGIFactory2 failed")), RESULT_FAIL);
        IDXGISwapChain1* pSC1 = NULL;
        hr = pDXGIFactory2->CreateSwapChainForComposition(GetD3DDevice(),&scd1,NULL,&pSC1);
        RETURN_ON_HRESULT_FAILURE(LogOnFailure(hr, _T("ExecuteTestCase: pDXGIFactory2->CreateSwapChainForComposition() failed")), RESULT_FAIL);
        pDXGISwapChain = pSC1;
        LOG_RETURN_ON_NULL(pDXGISwapChain, RESULT_FAIL, _T("ExecuteTestCase: pDXGIFactory2->CreateSwapChainForComposition() succeeded but pDXGISwapChain = NULL."));
    }
    else
    {
        hr = GetDXGIFactory()->CreateSwapChain(GetD3DDevice(), &scd, &pDXGISwapChain);
        RETURN_ON_HRESULT_FAILURE(LogOnFailure(hr, _T("ExecuteTestCase: DXGIFactory->CreateSwapChain() failed")), RESULT_FAIL);
        LOG_RETURN_ON_NULL(pDXGISwapChain, RESULT_FAIL, _T("ExecuteTestCase: DXGIFactory->CreateSwapChain() succeeded but pDXGISwapChain = NULL."));
    }

    // Instantiate DCOMP if necessary
    if (m_tp_bDCOMP)
    {
        hr = m_DCOMPIsolation.CreateAndAssociateDCOMPVisual(pDXGISwapChain, myWindow.GetTestWindow());
        RETURN_ON_HRESULT_FAILURE(LogOnFailure(hr, _T("ExecuteTestCase: CreateAndAssociateDCOMPVisual() failed")), RESULT_FAIL);
    }

    // Get backbuffer
    ID3D10Texture2D* pDXGIBackBuffer = NULL;
    hr = pDXGISwapChain->GetBuffer(0, __uuidof( ID3D10Texture2D ), (void**)&pDXGIBackBuffer);
    RETURN_ON_HRESULT_FAILURE(LogOnFailure(hr, _T("DXGISwapChainrrentDevice->GetBuffer(IID_ID3D10Texture2D) failed")), RESULT_FAIL);
    LOG_RETURN_ON_NULL(pDXGIBackBuffer, RESULT_FAIL, _T("DXGISwapChainrrentDevice->GetBuffer(IID_ID3D10Texture2D) succeeded but pDXGIBackBuffer = NULL."));

    // Create render target view of backbuffer
    D3D10_RENDER_TARGET_VIEW_DESC rtvd;
    ZeroMemory( &rtvd, sizeof( rtvd ) );
    rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvd.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
    rtvd.Texture2D.MipSlice = 0;

    SAFE_RELEASE(m_pRTView);
    hr = GetD3DDevice()->CreateRenderTargetView(pDXGIBackBuffer, NULL, &m_pRTView);
    SAFE_RELEASE(pDXGIBackBuffer); // Done with backbuffer
    RETURN_ON_HRESULT_FAILURE(LogOnFailure(hr, _T("D3D10Device->CreateRenderTargetView() failed")), RESULT_FAIL);
    LOG_RETURN_ON_NULL(m_pRTView, RESULT_FAIL, _T("D3D10Device->CreateRenderTargetView() succeeded but pView = NULL."));

    // Set render target view
    GetD3DDevice()->OMSetRenderTargets(1, &m_pRTView, NULL);

    // Setup the viewport
    D3D10_VIEWPORT vp;
    ZeroMemory(&vp, sizeof(vp));
    vp.Width = winrect.right - winrect.left;
    vp.Height = winrect.bottom - winrect.top;
    vp.MaxDepth = 1.0f;
    GetD3DDevice()->RSSetViewports( 1, &vp );

    // Get a test resource
    if( NULL == m_pTestResource )
    {
        hr = CreateTestResource();
        RETURN_ON_HRESULT_FAILURE(LogOnFailure(hr, _T("ExecuteTestCase: CreateTestResource failed")), RESULT_FAIL);
    }

    // Delay for a second to give DWM time to recover from fullscreen mode.
    Sleep(1000);

    // Test all outputs
    IDXGIOutput* pDXGIOutput = NULL;
    for(UINT iOutput = 0; SUCCEEDED(pDXGIAdapter->EnumOutputs(iOutput, &pDXGIOutput)); ++iOutput)
    {
        WriteToLog(_T("-------------------"), iOutput);
        WriteToLog(_T("Testing output: %u"), iOutput);
        WriteToLog(_T("-------------------"), iOutput);

        // Run the test case
        tr = RunCase(pDXGISwapChain, pDXGIOutput);

        // Cleanup the output
        if( NULL != pDXGIOutput )
        {
            pDXGIOutput->Release();
            pDXGIOutput = NULL;
        }

        // If we did not pass the test case on this output be done.
        if( RESULT_PASS != tr )
        {
            goto cleanup;
        }
    }

cleanup:

    // Release Factory2
    SAFE_RELEASE(pDXGIFactory2);

    // Release RenderTargetView
    SAFE_RELEASE(m_pRTView);

    // Release DXGISwapChain
    SAFE_RELEASE(pDXGISwapChain);

    // Relase DXGI Adapter
    SAFE_RELEASE(pDXGIAdapter);

    // Release DXGI Device
    SAFE_RELEASE(pDXGIDevice);

    // Release the TestResource
    ReleaseTestResource();

    // Clean up DirectComposition if necessary
    m_DCOMPIsolation.DestroyDCompDeviceAndVisual();

    return tr;
}

//-----------------------------------------------------------------------------
// The CDXGIIncrementPresentCountTest Test
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Function: RunCase
// Desc: Verify that present will up the DXGI_FRAME_STATISTICS
//-----------------------------------------------------------------------------
TEST_RESULT CDXGIIncrementPresentCountTest::RunCase(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput)
{
    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, RESULT_FAIL, _T("RunCase: Bad input (DXGISwapChain = NULL)"));
    LOG_RETURN_ON_NULL(pDXGIOutput, RESULT_FAIL, _T("RunCase: Bad input (DXGIOutput = NULL)"));


    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = E_FAIL;
    UINT nTimeout;

    // Frame statistices before and after present
    DXGI_FRAME_STATISTICS fs1,fs2;
    ZeroMemory( &fs1, sizeof(fs1) ); // Before
    ZeroMemory( &fs2, sizeof(fs2) ); // After

    // Take ownership
    if( DXGISWAPCHAIN == GetCurrentResourceType() && FALSE == GetOwnByFullScreen() )
    {
        // We want the swapchain to GetFrameStats without having ownership
        WriteToLog(_T("Not taking ownership for this case."));
    }
    else
    {
        hr = TakeOwnership(pDXGISwapChain, pDXGIOutput);
        if( FAILED(hr) )
        {
            WriteToLog(_T("Unable to take ownership"));
            return RESULT_SKIP;
        }
    }

    // Present something to get everything started
    WriteToLog("Presenting an initial frame...");
    hr = IncrementPresentCount(pDXGISwapChain);
    if( DXGI_STATUS_OCCLUDED == hr )
    {
        WriteToLog(_T("Test is occluded - Unable to increment the present count"));
        ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
        return RESULT_SKIP;
    }
    else if( FAILED(hr) )
    {
        WriteToLog(_T("Unable to increment the present count"));
        ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
        return RESULT_FAIL;
    }

    // Pause for frame to be output and get baseline present stats
    WriteToLog("Pausing for a moment to give Present a chance to complete...");
    Sleep(1000);

    //
    // Clear out any initial DISJOINT error.  We always get 
    // DXGI_ERROR_FRAME_STATISTICS_DISJOINT on the first call to 
    // GetFrameStatistics, but we will also get the error after the first 
    // Present call when DWM is off.
    //
    WriteToLog("Calling GetFrameStatistics() to clear out any initial DISJOINT errors...");
    GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1); // Ignore any error returns

    WriteToLog(_T("Waiting for GetFrameStatistics to reflect first Present..."));

    //
    // Call GetFrameStatistics until our first present is reflected in the 
    // PresentCount or we time out.
    //
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1);
    if (FAILED(hr))
    {
        WriteToLog(_T("Unable to get baseline frame statistics"));
        ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
        if( DXGI_STATUS_OCCLUDED == hr )
            return RESULT_SKIP;

        return RESULT_FAIL;
    }

    nTimeout = 0;
    while( 0 == fs1.PresentCount )
    {
        Sleep(1000);
        ZeroMemory( &fs1, sizeof(fs1) );
        hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1);
        if( FAILED(hr) )
        {
            WriteToLog(_T("Unable to get baseline frame statistics"));
            ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
            if( DXGI_STATUS_OCCLUDED == hr )
                return RESULT_SKIP;

            return RESULT_FAIL;
        }
        if(nTimeout++ > 9 )
        {
            ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
            WriteToLog(_T("*** FAILURE: Present count is not incrementing after trying repeatedly for %u seconds ***"), nTimeout);
            return RESULT_FAIL;
        }
    }

    // Log present stats from first frame
    WriteToLog(_T("First Frame Present Stats:"));
    WriteToLog(_T("PresentCount: %u"), fs1.PresentCount);
    WriteToLog(_T("PresentRefreshCount: %u"), fs1.PresentRefreshCount);
    WriteToLog(_T("SyncRefreshCount: %u"), fs1.SyncRefreshCount);
    WriteToLog(_T("SyncQPCTime: %lu"), fs1.SyncQPCTime);
    WriteToLog(_T("SyncGPUTime: %lu"), fs1.SyncGPUTime);


    // Up the present count
    WriteToLog(_T("Presenting second frame..."));
    hr = IncrementPresentCount(pDXGISwapChain);
    if( DXGI_STATUS_OCCLUDED == hr )
    {
        WriteToLog(_T("Test is occluded - Unable to increment the present count"));
        ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
        return RESULT_SKIP;
    }
    else if( FAILED(hr) )
    {
        WriteToLog(_T("Unable to increment the present count"));
        ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
        return RESULT_FAIL;
    }

    // Pause for frame to be output and get test present stats
    WriteToLog(_T("Waiting for GetFrameStatistics to reflect second Present..."));
    Sleep(1000);
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs2);
    nTimeout = 0;
    while( fs2.PresentCount <= fs1.PresentCount )
    {
        if( 9 < nTimeout )
        {
            ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
            WriteToLog(_T("Present count is not incrementing"));
            return RESULT_FAIL;
        }
        Sleep(1000);
        ZeroMemory( &fs2, sizeof(fs2) );
        hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs2);
        if( FAILED(hr) )
        {
            WriteToLog(_T("Unable to get test frame statistics"));
            ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
            if( DXGI_STATUS_OCCLUDED == hr )
                return RESULT_SKIP;

            return RESULT_FAIL;
        }
        nTimeout++;
    }

    // Release ownership
    ReleaseOwnership(pDXGISwapChain, pDXGIOutput);

    // Log present stats from second frame
    WriteToLog(_T("Second Frame Present Stats:"));
    WriteToLog(_T("PresentCount: %u"), fs2.PresentCount);
    WriteToLog(_T("PresentRefreshCount: %u"), fs2.PresentRefreshCount);
    WriteToLog(_T("SyncRefreshCount: %u"), fs2.SyncRefreshCount);
    WriteToLog(_T("SyncQPCTime: %lu"), fs2.SyncQPCTime);
    WriteToLog(_T("SyncGPUTime: %lu"), fs2.SyncGPUTime);


    // PresentCount should have incremented
    if( (fs1.PresentCount + 1) != fs2.PresentCount )
    {
        WriteToLog(_T("*** FAILURE: Present count incrementing by more then one ***"));
        return RESULT_FAIL;
    }

    // PresentRefreshCount should have incremented
    if( fs1.PresentRefreshCount >= fs2.PresentRefreshCount )
    {
        WriteToLog(_T("*** FAILURE: PresentRefreshCount is not incrementing ***"));
        return RESULT_FAIL;
    }

    // SyncRefreshCount should have incremented
    if( fs1.SyncRefreshCount >= fs2.SyncRefreshCount )
    {
        WriteToLog(_T("*** FAILURE: SyncRefreshCount is not incrementing ***"));
        return RESULT_FAIL;
    }

    // SyncQPCTime should have incremented
    if( fs1.SyncQPCTime.QuadPart == fs2.SyncQPCTime.QuadPart )
    {
        WriteToLog(_T("*** FAILURE: SyncQPCTime.QuadPart is not incrementing ***"));
        return RESULT_FAIL;
    }

    // SyncGPUTime should have incremented.. if not full screen
    /* GPUTime is not yet supported in full-screen or under DWM.
    if( TRUE == GetOwnByFullScreen() )
    {
    if( fs1.SyncGPUTime.QuadPart == fs2.SyncGPUTime.QuadPart )
    {
    WriteToLog(_T("SyncGPUTime.QuadPart is not incrementing"));
    return RESULT_FAIL;
    }  
    }*/

    return tr;
}

//-----------------------------------------------------------------------------
// The CDXGIGetLastPresentCountIncrementTest Test
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Function: RunCase
// Desc: Verify that present will up the count returned by GetLastPresentCount
//-----------------------------------------------------------------------------
TEST_RESULT CDXGIGetLastPresentCountIncrementTest::RunCase(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput)
{
    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, RESULT_FAIL, _T("RunCase: Bad input (DXGISwapChain = NULL)"));
    if( DXGISWAPCHAIN != GetCurrentResourceType() )
    {
        WriteToLog(_T("Can only run on DXGISWAPCHAIN."));
        return RESULT_SKIP;
    }

    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = E_FAIL;

    // PresentCount before and after present
    UINT nPresentCountBeforePresent = 0;
    UINT nPresentCountAfterPresent = 0;

    // Get frame present count
    hr = GetLastPresentCount(pDXGISwapChain, pDXGIOutput, &nPresentCountBeforePresent);
    if( FAILED(hr) )
    {
        WriteToLog(_T("*** FAILURE: GetLastPresentCount() failed before present ***"));
        return RESULT_FAIL;
    }

    // Up the present count
    hr = IncrementPresentCount(pDXGISwapChain);
    if( DXGI_STATUS_OCCLUDED == hr )
    {
        WriteToLog(_T("Test is occluded - Unable to increment the present count"));
        return RESULT_SKIP;
    }
    else if( FAILED(hr) )
    {
        WriteToLog(_T("*** FAILURE: Unable to increment the present count ***"));
        return RESULT_FAIL;
    }

    // Get frame statistics after present
    hr = GetLastPresentCount(pDXGISwapChain, pDXGIOutput, &nPresentCountAfterPresent);
    if( FAILED(hr) )
    {
        WriteToLog(_T("*** FAILURE: GetLastPresentCount() failed after present ***"));
        return RESULT_FAIL;
    }

    // PresentCount should have incremented
    if( nPresentCountBeforePresent == nPresentCountAfterPresent )
    {
        WriteToLog(_T("*** FAILURE: Present count is not incrementing ***"));
        return RESULT_FAIL;
    }

    return tr;
}
//-----------------------------------------------------------------------------
// The CDXGIWithoutOwnershipTest Test
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function: RunCase
// Desc: Verify calling GetFrameStatistics() without ownerhsip will fail
//-----------------------------------------------------------------------------
TEST_RESULT CDXGIWithoutOwnershipTest::RunCase(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput)
{
    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, RESULT_FAIL, _T("RunCase: Bad input (DXGISwapChain = NULL)"));
    LOG_RETURN_ON_NULL(pDXGIOutput, RESULT_FAIL, _T("RunCase: Bad input (DXGIOutput = NULL)"));

    TEST_RESULT tr = RESULT_FAIL;
    HRESULT hr = E_FAIL;
    HRESULT expectedhr = DXGI_ERROR_UNSUPPORTED;

    // Frame statistices
    DXGI_FRAME_STATISTICS fs1;
    ZeroMemory( &fs1, sizeof(fs1) );


    // Get frame statistics
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1);
    if( expectedhr != hr )
    {
        WriteToLog(_T("GetFrameStatistics() returned an unexpected HRESULT()."));
        goto cleanup;
    }

    tr = RESULT_PASS;

cleanup:

    return tr;
}

//-----------------------------------------------------------------------------
// The CDXGINULLStatsTest Test
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Function: RunCase
// Desc: GetFrameStatistics(NULL)
//-----------------------------------------------------------------------------
TEST_RESULT CDXGINULLStatsTest::RunCase(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput)
{
    TEST_RESULT tr = RESULT_FAIL;
    HRESULT hr = E_FAIL;

    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, RESULT_FAIL, _T("RunCase: Bad input (DXGISwapChain = NULL)"));
    LOG_RETURN_ON_NULL(pDXGIOutput, RESULT_FAIL, _T("RunCase: Bad input (DXGIOutput = NULL)"));

    // Take ownership
    hr = TakeOwnership(pDXGISwapChain, pDXGIOutput);
    if( DXGI_STATUS_OCCLUDED == hr || FAILED(hr) )
    {
        WriteToLog(_T("Unable to take ownership"));
        return RESULT_SKIP;
    }

    // Get frame statistics before present
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, NULL);
    if( SUCCEEDED(hr) )
    {
        WriteToLog(_T("Error GetFrameStatistics(NULL) Succeeded"));
        goto cleanup;
    }

    tr = RESULT_PASS;

cleanup:
    // Release ownership
    ReleaseOwnership(pDXGISwapChain, pDXGIOutput);

    return tr;
}

//-----------------------------------------------------------------------------
// The CDXGIGetLastPresentCountNULL Test
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Function: RunCase
// Desc: GetLastPresentCount(NULL)
//-----------------------------------------------------------------------------
TEST_RESULT CDXGIGetLastPresentCountNULLTest::RunCase(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput)
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = E_FAIL;

    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, RESULT_FAIL, _T("RunCase: Bad input (DXGISwapChain = NULL)"));
    if( DXGISWAPCHAIN != GetCurrentResourceType() )
    {
        WriteToLog(_T("Can only run on DXGISWAPCHAIN."));
        return RESULT_SKIP;
    }

    // Get frame statistics before present
    hr = GetLastPresentCount(pDXGISwapChain, pDXGIOutput, NULL);
    if( SUCCEEDED(hr) )
    {
        WriteToLog(_T("*** FAILURE: Error GetFrameStatistics(NULL) Succeeded ***"));
        return RESULT_FAIL;
    }

    return tr;
}

//-----------------------------------------------------------------------------
// The CDXGIGlitchTest Test
//-----------------------------------------------------------------------------

// In windowed mode, Present Stats can be quite laggy.
#define DELAY_BETWEEN_GFS_CALLS 1000

//-----------------------------------------------------------------------------
// Function: RunCase
// Desc: Verify that that a glitch can be detected.
//-----------------------------------------------------------------------------
TEST_RESULT CDXGIGlitchTest::RunCase(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput)
{
    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, RESULT_FAIL, _T("RunCase: Bad input (DXGISwapChain = NULL)"));
    if( DXGISWAPCHAIN != GetCurrentResourceType() )
    {
        WriteToLog(_T("RunCase: Can only run on DXGISWAPCHAIN."));
        return RESULT_SKIP;
    }

    if( GetFramework()->m_D3DOptions.SrcDriverType == D3D_DRIVER_TYPE_REFERENCE )
    {
        WriteToLog(_T("RunCase: Can't run case against reference rasterizer."));
        return RESULT_SKIP;
    }
    else if( GetFramework()->m_D3DOptions.SrcDriverType == D3D_DRIVER_TYPE_SOFTWARE )
    {
        WriteToLog(_T("RunCase: Can't run case against software rasterizer."));
        return RESULT_SKIP;
    }
    else if( GetFramework()->m_D3DOptions.SrcDriverType == D3D_DRIVER_TYPE_WARP )
    {
        WriteToLog(_T("RunCase: Can't run case against Warp rasterizer."));
        return RESULT_SKIP;
    };


    TEST_RESULT tr = RESULT_FAIL;   // case result
    HRESULT hr = E_FAIL;

    // Frame statistices
    DXGI_FRAME_STATISTICS fs1, fs2, fsInitial;
    ZeroMemory( &fs1, sizeof(fs1) );// 1st frame stats
    ZeroMemory( &fs2, sizeof(fs2) );// 2nd frame stats
    ZeroMemory( &fsInitial, sizeof(fsInitial) );// Initial frame stats

    // Timeout for waiting for fame stats to update
    UINT nTimeout = 0;

    // Take ownership
    if( DXGISWAPCHAIN == GetCurrentResourceType() && FALSE == GetOwnByFullScreen() )
    {
        // We want the swapchain to GetFrameStats without having ownership
        WriteToLog(_T("RunCase: Not taking ownership for this case."));
    }
    else
    {
        hr = TakeOwnership(pDXGISwapChain, pDXGIOutput);
        if( DXGI_STATUS_OCCLUDED == hr || FAILED(hr) )
        {
            WriteToLog(_T("RunCase: Unable to take ownership"));
            tr = RESULT_SKIP;
            goto cleanup;
        }

    }

    // Reset FrameStats (Clear out initial DISJOINT error and get initial stats)
    WriteToLog(_T("Calling GetFrameStatistics to clear out initial DISJOINT return code..."));
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fsInitial);
    if( FAILED(hr) && hr != DXGI_ERROR_FRAME_STATISTICS_DISJOINT)
    {
        WriteToLog(_T("RunCase: GetFrameStatistics is failing with %s(0x%x)."), HResultToString(hr), hr);
        tr = RESULT_SKIP;
        goto cleanup;
    }

    WriteToLog(_T("Calling GetFrameStatistics to get initial stats..."));
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fsInitial);
    if( FAILED(hr))
    {
        WriteToLog(_T("RunCase: GetFrameStatistics is failing with %s(0x%x)."), HResultToString(hr), hr);
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Log present stats from initial state
    WriteToLog(_T("******************************"));
    WriteToLog(_T("Initial Present Stats (prior to presenting any frames):"));
    WriteToLog(_T("PresentCount: %u"), fsInitial.PresentCount);
    WriteToLog(_T("PresentRefreshCount: %u"), fsInitial.PresentRefreshCount);
    WriteToLog(_T("SyncRefreshCount: %u"), fsInitial.SyncRefreshCount);
    WriteToLog(_T("SyncQPCTime: %lu"), fsInitial.SyncQPCTime);
    WriteToLog(_T("SyncGPUTime: %lu"), fsInitial.SyncGPUTime);
    WriteToLog(_T("******************************"));


    // Render our baseline
    WriteToLog(_T("Making first Present call..."));
    hr = IncrementPresentCount(pDXGISwapChain);
    if( DXGI_STATUS_OCCLUDED == hr || FAILED(hr) )
    {
        WriteToLog(_T("RunCase: Present #1: Failed"));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Reset FrameStats (Clear out initial DISJOINT error and get initial stats)
    WriteToLog(_T("Calling GetFrameStatistics to clear out any second DISJOINT return code when DWM is off..."));
    ZeroMemory( &fs1, sizeof(fs1) );
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1);
    if( FAILED(hr) && hr != DXGI_ERROR_FRAME_STATISTICS_DISJOINT)
    {
        WriteToLog(_T("RunCase: GetFrameStatistics is failing with %s(0x%x)."), HResultToString(hr), hr);
        tr = RESULT_SKIP;
        goto cleanup;
    }

    WriteToLog(_T("Calling GetFrameStatistics repeatedly until our first present is reflected in the results..."));
    //
    // Wait for GetFrameStats to register our Present call
    //
    for(nTimeout = 0; nTimeout < 10 && fs1.PresentCount<=fsInitial.PresentCount  ; nTimeout++ )
    {
        Sleep(DELAY_BETWEEN_GFS_CALLS);

        ZeroMemory( &fs1, sizeof(fs1) );
        hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1);
        if( FAILED(hr) )
        {
            if (DXGI_ERROR_FRAME_STATISTICS_DISJOINT == hr)
            {
                WriteToLog(_T("RunCase: Present #1: Warning! GetFrameStatistics returned unexpected DXGI_ERROR_FRAME_STATISTICS_DISJOINT on call %d."), nTimeout);
            }
            else
            {
                WriteToLog(_T("RunCase: Present #1: GetFrameStatistics is failing."));
                tr = RESULT_SKIP;
                goto cleanup;
            }
        }

    }

    // Log present stats from first frame
    WriteToLog(_T("******************************"));
    WriteToLog(_T("First Frame Present Stats:"));
    WriteToLog(_T("PresentCount: %u"), fs1.PresentCount);
    WriteToLog(_T("PresentRefreshCount: %u"), fs1.PresentRefreshCount);
    WriteToLog(_T("SyncRefreshCount: %u"), fs1.SyncRefreshCount);
    WriteToLog(_T("SyncQPCTime: %lu"), fs1.SyncQPCTime);
    WriteToLog(_T("SyncGPUTime: %lu"), fs1.SyncGPUTime);
    WriteToLog(_T("******************************"));

    //
    //  If we timed out waiting, log a failure.
    //
    if( 10 == nTimeout )
    {
        WriteToLog(_T("*** FAILURE: RunCase: Present #1: PresentCount is not updating after %u ms. ***"), (nTimeout)*DELAY_BETWEEN_GFS_CALLS);
        tr = RESULT_FAIL;
        goto cleanup;
    }

    //
    // If GetFrameStats didn't reflect our present call after a couple iterations, let's log a warning.  It's clearly taking too long.
    //
    if (nTimeout > 1)
        WriteToLog(_T("RunCase: Present #1: Warning!  It took over %ums for GetFrameStatistics to reflect our present call."), nTimeout*DELAY_BETWEEN_GFS_CALLS );

    // Glitch - Introduce simulated delay between presented frames.
    Sleep(1000);

    // Render our 2nd frame
    WriteToLog(_T("Making second Present call..."));
    hr = IncrementPresentCount(pDXGISwapChain);
    if( DXGI_STATUS_OCCLUDED == hr || FAILED(hr) )
    {
        WriteToLog(_T("RunCase: Present #2: Unable to increment the present count"));
        tr = RESULT_SKIP;
        goto cleanup;
    }


    //
    // Wait for GetFrameStats to register our Present call
    //
    WriteToLog(_T("Calling GetFrameStatistics repeatedly until our second present is reflected in the results..."));
    for( nTimeout = 0;  nTimeout < 10 && fs2.PresentCount <= fs1.PresentCount; nTimeout++ )
    {
        Sleep(DELAY_BETWEEN_GFS_CALLS);

        ZeroMemory( &fs2, sizeof(fs2) );
        hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs2);
        if( FAILED(hr) )
        {
            if (DXGI_ERROR_FRAME_STATISTICS_DISJOINT == hr)
            {
                WriteToLog(_T("RunCase: Present #2: Warning! GetFrameStatistics returned unexpected DXGI_ERROR_FRAME_STATISTICS_DISJOINT on call %d."), nTimeout);
            }
            else
            {
                WriteToLog(_T("RunCase: Present #2: GetFrameStatistics is failing."));
                tr = RESULT_SKIP;
                goto cleanup;
            }
        }

    }

    // Log present stats from first frame
    WriteToLog(_T("******************************"));
    WriteToLog(_T("Second Frame Present Stats:"));
    WriteToLog(_T("PresentCount: %u"), fs2.PresentCount);
    WriteToLog(_T("PresentRefreshCount: %u"), fs2.PresentRefreshCount);
    WriteToLog(_T("SyncRefreshCount: %u"), fs2.SyncRefreshCount);
    WriteToLog(_T("SyncQPCTime: %lu"), fs2.SyncQPCTime);
    WriteToLog(_T("SyncGPUTime: %lu"), fs2.SyncGPUTime);
    WriteToLog(_T("******************************"));

    //
    //  If we timed out waiting, log a failure.
    //
    if( 10 == nTimeout )
    {
        WriteToLog(_T("*** FAILURE: RunCase: Present #2: PresentCount is not updating after %u ms. ***"), nTimeout*DELAY_BETWEEN_GFS_CALLS);
        tr = RESULT_FAIL;
        goto cleanup;
    }

    //
    // If GetFrameStats didn't reflect our present call after a couple iterations, let's log a warning.  It's clearly taking too long.
    //
    if (nTimeout > 1)
        WriteToLog(_T("RunCase: Present #2: Warning!  It took over %ums for GetFrameStatistics to reflect our present call."), nTimeout*DELAY_BETWEEN_GFS_CALLS );

    // Find the delta of the 1st and 2nd frame... should only be one.
    UINT FramesCompleteDelta = (UINT)fs2.PresentCount - (UINT)fs1.PresentCount;
    if( 1 != FramesCompleteDelta )
    {
        // Fail. Present count did not update.
        WriteToLog(_T("RunCase: Present count did not update by 1, from 1st frame to 2nd frame. Present #1 ID: %u, Present #2 ID: %u,"), fs1.PresentCount, fs2.PresentCount);
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Find the delta of the refresh count for each presented frame.
    UINT PresentRefreshCountDelta = (UINT)fs2.PresentRefreshCount - (UINT)fs1.PresentRefreshCount;

    // Verify that a glitch did happen.
    if( FramesCompleteDelta < PresentRefreshCountDelta )
    {
        // PASS! frame missed
        WriteToLog(_T("RunCase: GetFrameStatistics properly detected simulated glitch."));
    }
    else
    {
        // FAIL! no frames missed
        WriteToLog(_T("*** FAILURE: RunCase: GetFrameStatistics failed to detect simulated glitch!  Frame Delta: %d, PresentRefrestCount Delta: %d ***"), FramesCompleteDelta, PresentRefreshCountDelta);
        goto cleanup;
    }

    // if we made it here the test passed
    tr = RESULT_PASS;

cleanup:

    ReleaseOwnership(pDXGISwapChain, pDXGIOutput);

    return tr;
}

//-----------------------------------------------------------------------------
// The CDXGIQueuedFrameTest Test
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Function: RunCase
// Desc: Try to detect a queued frame.
// TODO: Detect a queued frame.
//-----------------------------------------------------------------------------
TEST_RESULT CDXGIQueuedFrameTest::RunCase(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput)
{
    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, RESULT_FAIL, _T("RunCase: Bad input (DXGISwapChain = NULL)"));
    if( DXGISWAPCHAIN != GetCurrentResourceType() )
    {
        WriteToLog(_T("RunCase: Can only run on DXGISWAPCHAIN."));
        return RESULT_SKIP;
    }

    TEST_RESULT tr = RESULT_FAIL;   // case result to return
    HRESULT hr = E_FAIL;            

    // Frame statistices
    DXGI_FRAME_STATISTICS fs1, fs2;              
    ZeroMemory( &fs1, sizeof(fs1) );    // baseline stats        
    ZeroMemory( &fs2, sizeof(fs2) );    // test stats

    // Last present count GetLastPresentCount
    UINT nLPC1 = 0;     // baseline count
    UINT nLPC2 = 0;     // test count

    // Timeout for waiting for fame stats to update
    UINT nTimeout = 0;

    // Take ownership
    if( DXGISWAPCHAIN == GetCurrentResourceType() && FALSE == GetOwnByFullScreen() )
    {
        // We want the swapchain to GetFrameStats without having ownership
        WriteToLog(_T("RunCase: Not taking ownership for this case."));
    }
    else
    {
        hr = TakeOwnership(pDXGISwapChain, pDXGIOutput);
        if( DXGI_STATUS_OCCLUDED == hr || FAILED(hr) )
        {
            WriteToLog(_T("RunCase: Unable to take ownership"));
            tr = RESULT_SKIP;
            goto cleanup;
        }

        // Reset after fullscreen change
        GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1);
        ZeroMemory( &fs1, sizeof(fs1) );
    }

    // Render a baseline frame to get everything going.
    hr = IncrementPresentCount(pDXGISwapChain);
    if( DXGI_STATUS_OCCLUDED == hr || FAILED(hr) )
    {
        WriteToLog(_T("RunCase: Present1: Unable to increment the present count"));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Pause for any outstanding frames
    Sleep(1000);

    // Get baseline frame stats
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1); // This should clear out any initial DISJOINT error
    nTimeout = 0;
    for(; nTimeout < 10 && 0 == fs1.PresentCount; nTimeout++ )
    {
        Sleep(1000);
        ZeroMemory( &fs1, sizeof(fs1) );
        hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1);
        if( FAILED(hr) )
        {
            WriteToLog(_T("RunCase: Present1: GetFrameStatistics is failing."));
            tr = RESULT_SKIP;
            goto cleanup;
        }
    }
    if( 10 == nTimeout )
    {

        WriteToLog(_T("RunCase: Present1: PresentCount is not updating."));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Get baseline LastPresentCount
    hr = GetLastPresentCount(pDXGISwapChain, pDXGIOutput, &nLPC1);
    if( FAILED(hr) || 0 == nLPC1)
    {
        WriteToLog(_T("RunCase: Present1: GetLastPresentCount is failing"));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Render the 2nd frame
    hr = IncrementPresentCount(pDXGISwapChain);
    if( DXGI_STATUS_OCCLUDED == hr || FAILED(hr) )
    {
        WriteToLog(_T("RunCase: Present2: Unable to increment the present count"));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Get test frame stats
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs2);
    if( FAILED(hr) )
    {
        WriteToLog(_T("RunCase: Present2: GetFrameStatistics is failing."));
        tr = RESULT_SKIP;
        goto cleanup;
    }
    // are we able to call GetFrameStatistics before the present got displayed?
    if( fs1.PresentCount != fs2.PresentCount )
    {
        // nope
        WriteToLog(_T("RunCase: Unable to queue a frame."));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Get test LastPresentCount
    hr = GetLastPresentCount(pDXGISwapChain, pDXGIOutput, &nLPC2);
    if( FAILED(hr) || 0 == nLPC1)
    {
        WriteToLog(_T("RunCase: Present2: GetLastPresentCount is failing"));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    if( nLPC1 == nLPC2 || 1 != (nLPC2-nLPC1) )
    {
        // There are other test case that will fail for this.
        WriteToLog(_T("RunCase: GetLastPresentCount is not updating correctly after a call to present"));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // If we get here we know thee number of queued frams is 1. 
    // as in (nLPC2 - nLPC1) - (fs2.PresentCount - fs1.PresentCount).
    // TODO: Detect more then 1 queued frame.
    tr = RESULT_PASS;

cleanup:

    ReleaseOwnership(pDXGISwapChain, pDXGIOutput);

    return tr;
}

//-----------------------------------------------------------------------------
// The CDXGIResource_D3D10VB
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Function: CreateTestResource
// Desc: Creates the resource to test
//-----------------------------------------------------------------------------
HRESULT CDXGIResource_D3D10VB::CreateResource(ID3D10Device* pDevice, CD3D10TestFramework* pTestFramework)
{
    // Verify input
    if( NULL == pDevice || NULL == pTestFramework )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource(): Bad input"));
        return E_POINTER;
    }

    if( NULL != m_pD3D10VB || NULL != m_pDXGIResource || NULL != m_pVS || NULL != m_pPS || NULL != m_pInputLayout)
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource(): Resources not released before call"));
        return E_FAIL;
    }

    HRESULT hr = E_FAIL;

    // Create vertex buffer
    XYZW_VERTEX v[3] = { 0.0f,  0.5f, 0.5f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f,
        -0.5f, -0.5f, 0.5f, 1.0f };

    D3D10_SUBRESOURCE_DATA sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.pSysMem = v;
    sd.SysMemPitch = 0;
    sd.SysMemSlicePitch = 0;

    D3D10_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(XYZW_VERTEX) * 3;
    bd.Usage = GetUsageFlags();
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.MiscFlags = 0;
    bd.CPUAccessFlags = GetUsageFlags() == D3D10_USAGE_DYNAMIC? D3D10_CPU_ACCESS_WRITE: 0;

    // Create buffer
    hr = pDevice->CreateBuffer( &bd, &sd, &m_pD3D10VB);
    if( FAILED(hr) )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource(): ID3D10Device->CreateBuffer() Failed"));
        return hr;
    }
    if( NULL == m_pD3D10VB )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource(): ID3D10Device->CreateBuffer() Succeed but pVB is NULL!"));
        return E_FAIL;
    }    

    // Get DXGIResource
    hr = SetResourceFromInterface((IUnknown**)&m_pD3D10VB);
    if( FAILED(hr) || NULL == m_pDXGIResource )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource: ISetResourceFromInterface failed"));
        return E_FAIL;
    }

    // Setup shaders
    // Compile vertex shader
    CBlob *pShaderBuff = NULL;
    CBlob *pErrorBuff = NULL;
    hr = pTestFramework->CompileShaderFromResource(NULL, _T("vs.vsh"), "VS", "vs_4_0", D3D_SHADER_DEBUG, &pShaderBuff, &pErrorBuff);
    if( FAILED(hr) || NULL == pShaderBuff)
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource: CompileShaderFromResource(vs.vsh) failed"));
        ReleaseResource();
        return hr;
    }

    // Create vertex shader
    hr = pDevice->CreateVertexShader(pShaderBuff->GetBufferPointer(), pShaderBuff->GetBufferSize(), &m_pVS);
    if( FAILED(hr) || NULL == m_pVS )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource: ID3D10Device->CreateVertexShader() failed"));
        ReleaseResource();
        return hr;
    } 

    // Create the input for the vertex shader
    D3D10_INPUT_ELEMENT_DESC layout[] =
    {
        { _T("POSITION"), 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },  
    };
    UINT numElements = sizeof(layout)/sizeof(layout[0]);

    hr = pDevice->CreateInputLayout(layout, numElements, pShaderBuff->GetBufferPointer(), pShaderBuff->GetBufferSize(), &m_pInputLayout);
    if( FAILED(hr) || NULL == m_pInputLayout )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource: ID3D10Device->CreateVertexShader() failed"));
        ReleaseResource();
        return hr;
    }

    SAFE_RELEASE(pShaderBuff);
    SAFE_RELEASE(pErrorBuff);

    // Compile pixel shader
    hr = pTestFramework->CompileShaderFromResource(NULL, _T("ps.psh"), "PS", "ps_4_0", D3D_SHADER_DEBUG, &pShaderBuff, &pErrorBuff);
    if( FAILED(hr) || NULL == pShaderBuff )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource: CompileShaderFromResource(ps.psh) failed"));
        ReleaseResource();
        return hr;
    }

    // Create pixel shader
    hr = pDevice->CreatePixelShader(pShaderBuff->GetBufferPointer(), pShaderBuff->GetBufferSize(), &m_pPS);
    if( FAILED(hr) || NULL == m_pPS )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource: ID3D10Device->CreatePixelShader() failed"));
        ReleaseResource();
        return hr;
    }

    SAFE_RELEASE(pShaderBuff);
    SAFE_RELEASE(pErrorBuff);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Function: UseResource
// Desc: Uses the resource. To get the resource into memory.
//-----------------------------------------------------------------------------
BOOL CDXGIResource_D3D10VB::UseResource(ID3D10Device* pDevice)
{    
    // render resource
    UINT stride = sizeof(XYZW_VERTEX);
    UINT offset = 0;
    pDevice->IASetVertexBuffers(0, 1, &m_pD3D10VB, &stride, &offset);
    pDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    pDevice->IASetInputLayout(m_pInputLayout);
    pDevice->VSSetShader(m_pVS);
    pDevice->PSSetShader(m_pPS);
    pDevice->GSSetShader(NULL);
    pDevice->Draw(3,0);

    return TRUE;
}