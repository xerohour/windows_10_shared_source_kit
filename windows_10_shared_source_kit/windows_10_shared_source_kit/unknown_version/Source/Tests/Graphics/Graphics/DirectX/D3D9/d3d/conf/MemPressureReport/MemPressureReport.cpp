//**********************************************************************
// File: MemPressureReport.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Inplementation for the test of Memory Pressure Report.
//
// History:
// 09/04/2007 Created by Jimmy Chen.
//**********************************************************************

#include <initguid.h>
#include "test.hpp"

#include <CustomTest.inl>
#include <D3D9CustomTest.inl>
#include <CustomTestApp.inl>
#include <D3D9CustomTestApp.inl>

LPTSTR    CMyTest::g_pwszWindowClass = _T("MemoryPressureClass");
LPTSTR    CMyTest::g_pwszWindowName = _T("MemoryPressure");

//**********************************************************************
// Member functions for class CMyTestApp
//**********************************************************************
//-----------------------------------------------------
// Constructor.
//-----------------------------------------------------
CMyTestApp::CMyTestApp()
{
};

//-----------------------------------------------------
// Function: GetTestType()
// Purpose: Get the test type.
//-----------------------------------------------------
TESTTYPE 
CMyTestApp::GetTestType() const
{
	return TESTTYPE_FUNC;
};

//-----------------------------------------------------
// Function: InitTestGroups()
// Purpose: Register test class with the group manager.
//-----------------------------------------------------
bool 
CMyTestApp::InitTestGroups()
{
	// Register test class with the group manager. No space allowed in the name
	if (!ADD_TEST(_T("MemPressureReport"), CMyTest))
	{
		return false;
	};
	return true;
};

//----------------------------------------------------------------------------------
// Function: CleanupTestGroup()
// Purpose: Clean up the test group.
//----------------------------------------------------------------------------------
void
CMyTestApp::CleanupTestGroup()
{
	WriteToLog(_T("Clean up the test group of MemPressureReport."));
}

//**************************************************************************
// Member functions for class CMyTest
//**************************************************************************
//-----------------------------------------------------
// Constructor.
//-----------------------------------------------------
CMyTest::CMyTest()
: CD3D9CustomTest< CMyTestApp >::CD3D9CustomTest(&g_Application),
  m_ushCurTestCase(0),
  m_uiVideoMemSize(0),
  m_pD3DEx(NULL),
  m_pD3dDeviceEx(NULL),
  m_pDirect3DQuery9(NULL),
  m_hWnd(NULL),
  m_bImplementAllTests(FALSE)
{
    for (USHORT i=0; i<MAX_TEXTURES_CREATED; i++)
    {
        m_pTexture[i] = NULL;
        m_pRenderTarget[i] = NULL;
        m_pDepthStencilSurface[i] = NULL;
    }
};

//----------------------------------------------------------------------------------
// Function: InitTestParameters()
// Purpose: Initializes the parameter axes along which the framework should iterate.
//----------------------------------------------------------------------------------
TEST_RESULT 
CMyTest::Setup()
{
    HRESULT         hrResult;
    TEST_RESULT     test_result = RESULT_PASS;  

	WriteToLog(_T("Enter the test of MemPressureReport..."));

    // Enumerate Adapters, and monitors connected
    if (Helper_TotalAdapters() == 0)
    {
		test_result = RESULT_SKIP;
        WriteToLog(_T("Warning[Setup()]: Helper_TotalAdapters() - no Adapter in the system."));
    }

    return test_result;
}

//----------------------------------------------------------------------------------
// Function: InitTestParameters()
// Purpose: Initializes the parameter axes along which the framework should iterate.
//----------------------------------------------------------------------------------
void 
CMyTest::Cleanup()
{
	WriteToLog(_T("Exit the test of MemPressureReport."));
}

//*************************************************************************
// Helper functions
//*************************************************************************
//----------------------------------------------------------------------------------
// Function: Helper_DisplayLastError()
// Purpose:  Display last error message
//----------------------------------------------------------------------------------
void 
CMyTest::Helper_DisplayLastError
(
    __in LPCTSTR    pszParentFunctionName,
    __in LPCTSTR    pszFunctionName
)
{
    DWORD       dwLastError = GetLastError();
    LPVOID      lpMsgBuf;

    assert(pszParentFunctionName != NULL);
    assert(pszFunctionName != NULL);

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL, 
                  dwLastError, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&lpMsgBuf, 
                  0, 
                  NULL);
    WriteToLog(_T("Error[%s]: %s failed with error %u (%s)."),
               pszParentFunctionName,
               pszFunctionName,
	           dwLastError,
		       lpMsgBuf);
    LocalFree(lpMsgBuf);
}

//----------------------------------------------------------------------------------
// Function: Helper_TotalAdapters()
// Purpose:  Enumerate all adapters
//----------------------------------------------------------------------------------
UINT
CMyTest::Helper_TotalAdapters
(
)
{
    HRESULT         hrResult = E_FAIL;
    UINT            uiAdapters = 0;

    hrResult = Direct3DCreate9Ex(D3D_SDK_VERSION, &m_pD3DEx);
    if (SUCCEEDED(hrResult))
	{
        uiAdapters = m_pD3DEx->GetAdapterCount();
    }

    if (m_pD3DEx != NULL)
    {
        m_pD3DEx->Release();
        m_pD3DEx = NULL;
    }
	return uiAdapters;
}

//----------------------------------------------------------------------------------
// Function: RegisterWindowClass()
// Purpose:  Creates and register the window class.
//----------------------------------------------------------------------------------
HRESULT 
CMyTest::Helper_RegisterWindowClass
(
)
{
    HRESULT     hrResult = S_OK;
    WNDCLASS	g_WindowClass;

    ZeroMemory(&g_WindowClass, sizeof(WNDCLASS));
    g_WindowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    g_WindowClass.hIcon         = NULL;
    g_WindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    g_WindowClass.style         = CS_HREDRAW | CS_VREDRAW;
    g_WindowClass.lpfnWndProc   = WndProc;
    g_WindowClass.hInstance     = NULL;
    g_WindowClass.lpszClassName = g_pwszWindowClass;

    ATOM ClassAtom = RegisterClass(&g_WindowClass);
    if (ClassAtom == 0)
    {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        {
            Helper_DisplayLastError(_T("Helper_RegisterWindowClass()"), _T("RegisterClass()"));
            hrResult = E_FAIL;
        }
    }
    return hrResult;
}

//----------------------------------------------------------------------------------
// Function: CreateAppWindows()
// Purpose:  Creates application windows based on the command-line arguments.
//----------------------------------------------------------------------------------
HRESULT 
CMyTest::Helper_CreateAppWindows
(
)
{
    HRESULT             hrResult = S_OK;
    int                 iLeft, iTop, iWidth, iHeight;
    HMONITOR            hMonitor;
    MONITORINFOEX       monitorInfo;;

    // display all windows on monitor 0
    hMonitor = m_pD3DEx->GetAdapterMonitor(0);
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &monitorInfo);

    iWidth = (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left) / 2;
    iHeight = (monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top) / 2;
    iLeft = monitorInfo.rcMonitor.left + iWidth / 2;
    iTop = monitorInfo.rcMonitor.top + iHeight / 2;

    m_hWnd = CreateWindowEx(WS_EX_TOPMOST,
                            g_pwszWindowClass,
                            g_pwszWindowName,
                            WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX),
                            iLeft,
                            iTop,
  	                        iWidth,
                            iHeight,
                            NULL,
                            0,
                            NULL,
                            NULL);
   
    if (m_hWnd == NULL)
    {
        Helper_DisplayLastError(_T("Helper_CreateAppWindows()"), _T("CreateWindowEx()"));
        hrResult = E_FAIL;
    }
    else
    {
        ShowWindow(m_hWnd, SW_SHOWDEFAULT);
    }
    return hrResult;
}

//----------------------------------------------------------------------------------
// Function: CreateDevicePerAdapter()
// Purpose:  Creates a device for adapter.
//----------------------------------------------------------------------------------
HRESULT 
CMyTest::Helper_CreateDeviceForAdapter
(
)
{
    HRESULT                 hrResult = S_OK;
	LPDIRECT3DSURFACE9      pBackBufferSurfacDesc = NULL;
	LPDIRECT3DSWAPCHAIN9    pSwapChain = NULL;
    D3DDISPLAYMODE          desktopMode = {0};
    USHORT                  i;
    RECT                    rcClient;
    DWORD                   behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;

	ZeroMemory(&g_pPresentParams, sizeof(D3DPRESENT_PARAMETERS));
    hrResult = m_pD3DEx->GetAdapterDisplayMode(0, &desktopMode);
    GetClientRect(m_hWnd, &rcClient);
    g_pPresentParams.BackBufferWidth = rcClient.right - rcClient.left;
    g_pPresentParams.BackBufferHeight = rcClient.bottom - rcClient.top;
    g_pPresentParams.FullScreen_RefreshRateInHz = 0;

    g_pPresentParams.BackBufferFormat = desktopMode.Format;
    g_pPresentParams.BackBufferCount = 1;
    g_pPresentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
    g_pPresentParams.MultiSampleQuality = 0;
    g_pPresentParams.SwapEffect = D3DSWAPEFFECT_FLIP;
    g_pPresentParams.hDeviceWindow = m_hWnd;
    g_pPresentParams.Windowed = TRUE;
    g_pPresentParams.EnableAutoDepthStencil = FALSE;
    g_pPresentParams.AutoDepthStencilFormat = D3DFMT_D16;
    g_pPresentParams.Flags = 0;
    g_pPresentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Create device(s)
    hrResult = m_pD3DEx->CreateDeviceEx(0, 
                                        D3DDEVTYPE_HAL,
                                        m_hWnd, 
                                        behaviorFlags, 
                                        &g_pPresentParams,
                                        NULL,
                                        &m_pD3dDeviceEx);
	
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_CreateDeviceForAdapter()]: CreateDeviceEx() failed with error 0x%x (%s)."),
           	       hrResult,
	               HResultToString(hrResult));
		// When mem is not enough, D3DERR_INVALIDCALL could be returned, simply skip the test case
		if (hrResult == D3DERR_INVALIDCALL)
		{
			hrResult = E_TESTSKIPPED;
	        WriteToLog(_T("Error[Helper_CreateDeviceForAdapter()]: The test case is skipped."),
			           hrResult,
				       HResultToString(hrResult));
		}
    }

    return hrResult;
}

//----------------------------------------------------------------------------------
// Function: Helper_Initialization()
// Purpose:  Do some test initialization e.g. creating device.
//----------------------------------------------------------------------------------
HRESULT 
CMyTest::Helper_Initialization
(
)
{
    HRESULT             hrResult = S_OK;
    HWND                hWndMain = NULL;

    // check if a window with registered class name g_pwszWindowClass, and title g_pwszWindowName
    // was created. If No, implement multiprocess tests
    hWndMain = FindWindowEx(NULL, 
                            NULL, 
                            g_pwszWindowClass, 
                            g_pwszWindowName);
    if (hWndMain == NULL)
    {
        m_bImplementAllTests = TRUE;
    }

    // Create D3D9 device
    hrResult = Direct3DCreate9Ex(D3D_SDK_VERSION, &m_pD3DEx);
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_Initialization()]: Direct3DCreate9Ex() failed with error 0x%x (%s)."),
           	       hrResult,
	               HResultToString(hrResult));
        goto Exit;
	}

    // Create a window
    hrResult = Helper_RegisterWindowClass();
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_Initialization()]: Helper_RegisterWindowClass() failed with error 0x%x (%s)."),
           	       hrResult,
	               HResultToString(hrResult));
        goto Exit;
	}
    hrResult = Helper_CreateAppWindows();
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_Initialization()]: Helper_CreateAppWindows() failed with error 0x%x (%s)."),
           	       hrResult,
	               HResultToString(hrResult));
        goto Exit;
	}

    // Create an adapter
    hrResult = Helper_CreateDeviceForAdapter();
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_Initialization()]: Helper_CreateDeviceForAdapter() failed with error 0x%x (%s)."),
           	       hrResult,
	               HResultToString(hrResult));
        goto Exit;
	}

    // We will request 4 times of video memory as estimated value 
    m_uiVideoMemSize = m_pD3dDeviceEx->GetAvailableTextureMem();

    // Prepare for memory pressure report
    hrResult = m_pD3dDeviceEx->CreateQuery(D3DQUERYTYPE_MEMORYPRESSURE, &m_pDirect3DQuery9);
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_Initialization()]: CreateQuery() failed with error 0x%x (%s)."),
           	       hrResult,
	               HResultToString(hrResult));
        goto Exit;
	}
    hrResult = m_pDirect3DQuery9->Issue(D3DISSUE_BEGIN);
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_Initialization()]: Issue(D3DISSUE_BEGIN) failed with error 0x%x (%s)."),
           	       hrResult,
	               HResultToString(hrResult));
        goto Exit;
	}

    // Get the initial memory pressure report data
    hrResult = Helper_GetMemPressureData(&m_iniD3dMemPressure, 0);
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_Initialization()]: Helper_GetMemPrerssureData() failed with error 0x%x (%s)."),
       	   	       hrResult,
	               HResultToString(hrResult));
        goto Exit;
    }
    m_preD3dMemPressure = m_iniD3dMemPressure;


Exit:
    return hrResult;
}

//---------------------------------------------------------------------------------------------
// Function: Helper_RenderToTarget()
// Purpose:  Create given number of render targets, create a texture, and render to the targets
//---------------------------------------------------------------------------------------------
HRESULT 
CMyTest::Helper_RenderToTarget
(
)
{
	void                          *pData;
	HRESULT                       hrResult;
	D3DCAPS9                      caps;
    D3DDEVICE_CREATION_PARAMETERS d3dCreationParams = {0};
    IDirect3DSurface9             *pSrcRenderTarget = NULL;
	IDirect3DVertexBuffer9        *pVertexBuffer = NULL;
    DWORD                         dwTotalMemSize = 0;
    UINT                          uiVideoMemSize = m_uiVideoMemSize;
    USHORT                        i;  
    SIMPLE_VERTEX                 vertexRectangle[] =
    {
        { -1.0f,-1.0f, 0.0f, 0xffff0000, },
        {  1.0f,-1.0f, 0.0f, 0xff0000ff, },
        {  0.0f, 1.0f, 0.0f, 0xff00ff00, },
    };

    // Issue D3DISSUE_BEGIN to staert the test
    hrResult = m_pDirect3DQuery9->Issue(D3DISSUE_BEGIN);
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_RenderToTarget()]: Issue(D3DISSUE_BEGIN) failed with error 0x%x (%s)."),
           	       hrResult,
	               HResultToString(hrResult));
        goto Exit;
	}

    i = 0;
    hrResult = m_pD3dDeviceEx->GetCreationParameters(&d3dCreationParams);
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_RenderToTarget()]: GetCreationParameters() failed with error 0x%x (%s)."),
           	       hrResult,
	               HResultToString(hrResult));
        goto Exit;
	}
    hrResult = m_pD3DEx->GetDeviceCaps( d3dCreationParams.AdapterOrdinal, d3dCreationParams.DeviceType, &caps );
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_RenderToTarget()]: GetDeviceCaps() failed with error 0x%x (%s)."),
           	       hrResult,
	               HResultToString(hrResult));
        goto Exit;
	}
    UINT width =  caps.MaxTextureWidth / 16;
    UINT height = caps.MaxTextureHeight / 16;
    do
    {
	    // create the largest rendertarget and depthstencil buffers possible
	    while (width > 0 && height > 0)
	    {
            hrResult = m_pD3dDeviceEx->CreateRenderTarget(width,
			     		    			  			      height,
 				    								      g_pPresentParams.BackBufferFormat,
					    							      g_pPresentParams.MultiSampleType,
						    						      g_pPresentParams.MultiSampleQuality,
                                                          FALSE,
								    					  &m_pRenderTarget[i],
                                                          NULL);
		    if (SUCCEEDED(hrResult))
            {
                hrResult = m_pD3dDeviceEx->CreateDepthStencilSurface(width,
        		     												 height,
		    		    											 g_pPresentParams.AutoDepthStencilFormat,
                                                                     g_pPresentParams.MultiSampleType,
				    		    									 g_pPresentParams.MultiSampleQuality,
					    		    								 FALSE,
						    		    							 &m_pDepthStencilSurface[i],
                                                                     NULL);
	            if (SUCCEEDED(hrResult))
		        {
			        break;
		        }
            }

            // reduce the render target size and try agian
		    width -= 256;
		    height -= 256;
	    }

        if (FAILED(hrResult))
        {
            WriteToLog(_T("Warning[Helper_RenderToTarget()]: CreateRenderTarget() failed with error 0x%x (%s)."),
       	               hrResult,
                       HResultToString(hrResult));
            break;
	    }

	    hrResult = m_pD3dDeviceEx->GetRenderTarget(0, &pSrcRenderTarget);
        if (FAILED(hrResult))
        {
            WriteToLog(_T("Warning[Helper_RenderToTarget()]: GetRenderTarget() failed with error 0x%x (%s)."),
           	           hrResult,
	                   HResultToString(hrResult));
            break;
        }

        hrResult = m_pD3dDeviceEx->SetRenderTarget(0, m_pRenderTarget[i]);
        if (FAILED(hrResult))
        {
            WriteToLog(_T("Warning[Helper_RenderToTarget()]: SetRenderTarget(0, m_pRenderTarget[i]) failed with error 0x%x (%s)."),
           	           hrResult,
	                   HResultToString(hrResult));
            break;
        }

	    // create the vertices for the quad
        hrResult = m_pD3dDeviceEx->CreateVertexBuffer(3 * sizeof(SIMPLE_VERTEX), 
                                                      0,
		                                              D3DFVF_CUSTOMVERTEX,
                                                      D3DPOOL_DEFAULT,
                                                      &pVertexBuffer, 
                                                      NULL);
        if (FAILED(hrResult))
        {
            WriteToLog(_T("Warning[Helper_RenderToTarget()]: CreateVertexBuffer() failed with error 0x%x (%s)."),
           	           hrResult,
	                   HResultToString(hrResult));
            break;
	    }

        // ignore errors
        void    *pVertex = NULL;
	    hrResult |= pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);
        memcpy(pVertex, vertexRectangle, sizeof(vertexRectangle));
	    hrResult |= pVertexBuffer->Unlock();
        if (FAILED(hrResult))
        {
            WriteToLog(_T("Warning[Helper_RenderToTarget()]: failed to copy vertex buffer."),
           	           hrResult,
	                   HResultToString(hrResult));
            break;
	    }

        // ignore errors
        m_pD3dDeviceEx->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        m_pD3dDeviceEx->SetRenderState(D3DRS_LIGHTING, FALSE);
	    m_pD3dDeviceEx->SetRenderState(D3DRS_AMBIENT, D3DXCOLOR(1.0, 1.0, 1.0, 1.0));

	    hrResult |= m_pD3dDeviceEx->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
        hrResult |= m_pD3dDeviceEx->BeginScene();
        hrResult |= m_pD3dDeviceEx->SetStreamSource(0, pVertexBuffer, 0, sizeof(SIMPLE_VERTEX));
        hrResult |= m_pD3dDeviceEx->SetFVF(D3DFVF_CUSTOMVERTEX);
        hrResult |= m_pD3dDeviceEx->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 1);
        hrResult |= m_pD3dDeviceEx->EndScene();
	    hrResult |= m_pD3dDeviceEx->SetRenderTarget((DWORD)0, pSrcRenderTarget);
        hrResult |= m_pD3dDeviceEx->StretchRect(m_pRenderTarget[i], NULL, pSrcRenderTarget, NULL, D3DTEXF_NONE);
        if (FAILED(hrResult))
        {
            WriteToLog(_T("Warning[Helper_RenderToTarget()]: StretchRect() failed with error 0x%x (%s)."),
           	           hrResult,
	                   HResultToString(hrResult));
            break;
	    }
	    hrResult = m_pD3dDeviceEx->Present(NULL, NULL, NULL, NULL);
        if (FAILED(hrResult))
        {
            WriteToLog(_T("Warning[Helper_RenderToTarget()]: Present() failed with error 0x%x (%s)."),
           	           hrResult,
	                   HResultToString(hrResult));
            break;
	    }
        pSrcRenderTarget->Release();
        pSrcRenderTarget = NULL;
        pVertexBuffer->Release();
        pVertexBuffer = NULL;

        dwTotalMemSize += (width * height * 4);            //each pixel 32 bits          
    } while (dwTotalMemSize <= uiVideoMemSize && ++i < MAX_TEXTURES_CREATED);

    if (hrResult == E_OUTOFMEMORY || hrResult == D3DERR_INVALIDCALL || hrResult == D3DERR_OUTOFVIDEOMEMORY)
    {
        hrResult = S_OK;
    }

Exit:
    if (pSrcRenderTarget != NULL)
    {
        pSrcRenderTarget->Release();
    }
    if (pVertexBuffer != NULL)
    {
        pVertexBuffer->Release();
    }
    return hrResult;
}

//---------------------------------------------------------------------------------------------
// Function: Helper_CreateTexture()
// Purpose:  Create different textures according to the argument
//---------------------------------------------------------------------------------------------
HRESULT 
CMyTest::Helper_CreateTexture
(
    __in D3DPOOL        d3dPool
)
{
    HRESULT                         hrResult = S_OK;
    IDirect3DTexture9               *pTexture = NULL;
    D3DDEVICE_CREATION_PARAMETERS   d3dCreationParams = {0};
	D3DCAPS9                        caps;
    DWORD                           dwTotalMemSize = 0;
    USHORT                          i;
    UINT                            uiVideoMemSize = m_uiVideoMemSize / 4;

    // Issue D3DISSUE_BEGIN to staert the test
    hrResult = m_pDirect3DQuery9->Issue(D3DISSUE_BEGIN);
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_CreateTexture()]: Issue(D3DISSUE_BEGIN) failed with error 0x%x (%s)."),
           	       hrResult,
	               HResultToString(hrResult));
        goto Exit;
	}

    // ignore errors
	hrResult = m_pD3dDeviceEx->GetCreationParameters(&d3dCreationParams);
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_CreateTexture()]: GetCreationParameters() failed with error 0x%x (%s)."),
           	       hrResult,
	               HResultToString(hrResult));
        goto Exit;
	}
	hrResult = m_pD3DEx->GetDeviceCaps(d3dCreationParams.AdapterOrdinal, d3dCreationParams.DeviceType, &caps);
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_CreateTexture()]: GetDeviceCaps() failed with error 0x%x (%s)."),
       	           hrResult,
                   HResultToString(hrResult));
        goto Exit;
    }
	UINT width = caps.MaxTextureWidth / 16;
	UINT height = caps.MaxTextureHeight / 16;

    i = 0;
    do {
        // Create a video texture with the same size and format as the system texture
	    hrResult = m_pD3dDeviceEx->CreateTexture(width,
		    								     height,
			    							     9,
				    						     0,
					    					     D3DFMT_A8R8G8B8,
						    				     d3dPool,
 							    		         &m_pTexture[i],
                                                 NULL);
        // if the video dirver doesn't support specified d3dPool, then ignore the error
        if (FAILED(hrResult))
        {
            if (hrResult == E_OUTOFMEMORY || hrResult == D3DERR_INVALIDCALL)
            {
                hrResult = S_OK;
            }
            else
            {
                WriteToLog(_T("Error[Helper_GetMemPrerssureData()]: GetData() failed with error 0x%x (%s)."),
    	                       hrResult,
                               HResultToString(hrResult));
            }
            break;
        }
        dwTotalMemSize += (width * height * 4);            //each pixel 32 bits
    } while (dwTotalMemSize <= uiVideoMemSize && ++i < MAX_TEXTURES_CREATED);

Exit:
    return hrResult;
}

//---------------------------------------------------------------------------------------------
// Function: Helper_ReleaseResources()
// Purpose:  Release allocated resources
//---------------------------------------------------------------------------------------------
void 
CMyTest::Helper_ReleaseResources
(
    __in USHORT     ushMemType
)
{
    USHORT      i;

    switch(ushMemType)
    {
        case Mem_Texture:
            for (i=0; i<MAX_TEXTURES_CREATED; i++)
            {
                if (m_pTexture[i] == NULL)
                {
                    break;
                }
                m_pTexture[i]->Release();
                m_pTexture[i] = NULL;
            }
            break;

        case Mem_RenderTarget:
            for (i=0; i<MAX_TEXTURES_CREATED; i++)
            {
                if (m_pRenderTarget[i] == NULL)
                {
                    break;
                }
                m_pRenderTarget[i]->Release();
                m_pRenderTarget[i] = NULL;
            }
            break;

        case Mem_DepthStencilSurface:
            for (i=0; i<MAX_TEXTURES_CREATED; i++)
            {
                if (m_pDepthStencilSurface[i] == NULL)
                {
                    break;
                }
                m_pDepthStencilSurface[i]->Release();
                m_pDepthStencilSurface[i] = NULL;
            }
            break;

        default:   // release all
            for (i=0; i<MAX_TEXTURES_CREATED; i++)
            {
                if (m_pRenderTarget[i] == NULL)
                {
                    break;
                }
                m_pRenderTarget[i]->Release();
                m_pRenderTarget[i] = NULL;
            }
            for (i=0; i<MAX_TEXTURES_CREATED; i++)
            {
                if (m_pDepthStencilSurface[i] == NULL)
                {
                    break;
                }
                m_pDepthStencilSurface[i]->Release();
                m_pDepthStencilSurface[i] = NULL;
            }
            for (i=0; i<MAX_TEXTURES_CREATED; i++)
            {
                if (m_pTexture[i] == NULL)
                {
                    break;
                }
                m_pTexture[i]->Release();
                m_pTexture[i] = NULL;
            }
            break;
    }
}

//-------------------------------------------------------------------------------------------
// Function: Helper_Helper_GetMemPressureData()
// Purpose:  Get memory pressure report
//-------------------------------------------------------------------------------------------
HRESULT 
CMyTest::Helper_GetMemPressureData
(
    __in D3DMEMORYPRESSURE  *pD3dMemPressure,
    __in DWORD              dwWaitTime
)
{
    HRESULT             hrResult = S_OK;

    assert(pD3dMemPressure != NULL);

    Sleep(dwWaitTime);          // Sleep to wait for eviction finished.
    ZeroMemory(pD3dMemPressure, sizeof(D3DMEMORYPRESSURE));
    hrResult = m_pDirect3DQuery9->Issue(D3DISSUE_END);
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_GetMemPrerssureData()]: Issue(D3DISSUE_END) failed with error 0x%x (%s)."),
       	           hrResult,
                   HResultToString(hrResult));
    }
    while((hrResult = m_pDirect3DQuery9->GetData(pD3dMemPressure, sizeof(D3DMEMORYPRESSURE), D3DGETDATA_FLUSH)) == S_FALSE);
    if (FAILED(hrResult))
    {
        WriteToLog(_T("Error[Helper_GetMemPrerssureData()]: GetData() failed with error 0x%x (%s)."),
       	           hrResult,
                   HResultToString(hrResult));
    }
    return hrResult;
}

//-------------------------------------------------------------------------------------------
// Function: Helper_CommonTestFunction()
// Purpose:  This is common test functino shared across all test cases
//-------------------------------------------------------------------------------------------
HRESULT 
CMyTest::Helper_CommonTestFunction
(
    __in_ecount(ushApps) AppDescriptor *pAppDescriptor,
    __in USHORT                        ushApps 
)
{
    HRESULT         hrResult = S_OK;  
    TCHAR           wszCommand[MAX_PATH];   
    AppDescriptor   *pMyAppDescriptor = pAppDescriptor;

    assert(pAppDescriptor != NULL);

    for (USHORT i=0; i<ushApps; i++)
    {
        // Implement actions associated with the app. Always call down to get thw window handle
        hrResult = Helper_ImplementTests(pAppDescriptor);
        if (FAILED(hrResult))
        {
            WriteToLog(_T("Error[Helper_CommonTestFunction()]: Helper_CommonTestFunction() failed with error 0x%x (%s)."),
       	    	       hrResult,
	    	           HResultToString(hrResult));
	        break;
        }

        hrResult = Helper_GetMemPressureData(&m_curD3dMemPressure, 1500);
        if (FAILED(hrResult))
        {
            WriteToLog(_T("Error[Helper_CommonTestFunction()]: Helper_GetMemPrerssureData() failed with error 0x%x (%s)."),
   	    	           hrResult,
       	               HResultToString(hrResult));
            break;
        }

        hrResult = Helper_VerifyTestResults();
        if (hrResult == E_TESTSKIPPED)
        {
            WriteToLog(_T("Warning[Helper_CommonTestFunction()]: The test is skipped for untestable hardware configuration."));
            break;
        }
        if (FAILED(hrResult))
        {
            WriteToLog(_T("Error[Helper_CommonTestFunction()]: Helper_VerifyTestResults() failed with error 0x%x (%s)."),
       	    	       hrResult,
        	           HResultToString(hrResult));
	        break;
        }
        Helper_ReleaseResources(Mem_None);    // Release resources for the next test
        pAppDescriptor++;                     // go to next AppDescriptor element 
    }

    return hrResult;
}

//-------------------------------------------------------------------------------------------
// Function: Helper_ImplementTests()
// Purpose:  Implement the tests
//-------------------------------------------------------------------------------------------
HRESULT 
CMyTest::Helper_ImplementTests
(
    __in AppDescriptor *pAppDescriptor
)
{
    HRESULT             hrResult = S_OK;  
    USHORT              i;
    DWORD               dwStatus;
    D3DMEMORYPRESSURE   d3dMemPressure;
    PROCESS_INFORMATION *pProcessInfo = NULL;
    DWORD               dwIniTime;

    assert(pAppDescriptor != NULL);

    for (USHORT i=0; i<ARRAYSIZE(pAppDescriptor->AppOperations); i++)
    {
        switch (pAppDescriptor->AppOperations[i].ushAction)
        {
            case Action_Wait:
                Sleep(pAppDescriptor->AppOperations[i].dwParameter);
                break;

            case Action_Wait_Process:
                pProcessInfo = (PROCESS_INFORMATION *)pAppDescriptor->AppOperations[i].pData;

                assert (pProcessInfo != NULL);
                
                // wait 7 minutes if the 2nd test process doesn't exit, fail the test
                if (WaitForSingleObject(pProcessInfo->hProcess, 
                                        pAppDescriptor->AppOperations[i].dwParameter) == WAIT_OBJECT_0)
                {
                    if (!GetExitCodeProcess(pProcessInfo->hProcess, &dwStatus))
                    {
                        Helper_DisplayLastError(_T("Helper_ImplementTests()"), _T("GetExitCodeProcess()"));
                        hrResult = E_FAIL;
                        goto Exit;
                    }
                    if (dwStatus != 0)
                    {
                        WriteToLog(_T("Error[Helper_ImplementTests()]: Another test process doesn't exit successfully."));
                        hrResult = E_FAIL;
                        goto Exit;
                    }
                }
                else
                {
                    WriteToLog(_T("Error[Helper_ImplementTests()]: Another test process doesn't exit within expected time! "));
                    hrResult = E_FAIL;
                    goto Exit;
                }
                CloseHandle(pProcessInfo->hProcess);
                CloseHandle(pProcessInfo->hThread);
                break;

            case Action_Issue_Begin:
                hrResult = m_pDirect3DQuery9->Issue(D3DISSUE_BEGIN);
                if (FAILED(hrResult))
                {
                    WriteToLog(_T("Error[Helper_ImplementTests()]: Issue(D3DISSUE_BEGIN) failed with error 0x%x (%s)!"),
       	            	       hrResult,
	    	                   HResultToString(hrResult));
                    goto Exit;
	            }
                m_preD3dMemPressure = m_iniD3dMemPressure;     // copy ini data to pre data for comparison
                break;

            case Action_Issue_End:
                hrResult = m_pDirect3DQuery9->Issue(D3DISSUE_END);
                if (FAILED(hrResult))
                {
                    WriteToLog(_T("Error[Helper_ImplementTests()]: Issue(D3DISSUE_END) failed with error 0x%x (%s)!"),
       	            	       hrResult,
	    	                   HResultToString(hrResult));
                    goto Exit;
	            }
                break;

            case Action_RenderToTarget:
                hrResult = Helper_RenderToTarget();
                if (FAILED(hrResult))
                {
                    WriteToLog(_T("Error[Helper_ImplementTests()]: Helper_RenderToTarget() failed with error 0x%x (%s)!"),
       	            	       hrResult,
	    	                   HResultToString(hrResult));
                    goto Exit;
                }
                break;

            case Action_CreateTexture:
                hrResult = Helper_CreateTexture((D3DPOOL)pAppDescriptor->AppOperations[i].dwParameter);
                if (FAILED(hrResult))
                {
                    WriteToLog(_T("Error[Helper_ImplementTests()]: Helper_CreateTexture() failed with error 0x%x (%s)!"),
       	            	       hrResult,
	    	                   HResultToString(hrResult));
                    goto Exit;
                }
                break;

            case Action_ReleaseMemory: 
                // Befor testing memory deallocation, get the mem pressure report for comparison
                hrResult = Helper_GetMemPressureData(&m_preD3dMemPressure, 500);
                if (FAILED(hrResult))
                {
                    WriteToLog(_T("Error[Helper_Initialization()]: Action_ReleaseMemory, Helper_GetMemPrerssureData() failed with error 0x%x (%s)."),
       	   	        hrResult,
	                HResultToString(hrResult));
                    goto Exit;
                }
                Helper_ReleaseResources((USHORT)pAppDescriptor->AppOperations[i].dwParameter);
                break;

            case Action_None:
            default:
                goto Exit;
        }
    }

Exit:
    return hrResult;
}

//----------------------------------------------------------------------------------
// Function: Helper_Cleanup()
// Purpose:  Release resources.
//----------------------------------------------------------------------------------
void 
CMyTest::Helper_Cleanup
(
)
{
    HRESULT             hrResult = S_OK;

    // ignore errors
    Helper_ReleaseResources(Mem_None);
    if (m_pDirect3DQuery9 != NULL)
    {
        m_pDirect3DQuery9->Release();
        m_pDirect3DQuery9 = NULL;
    }
    if (m_pD3dDeviceEx != NULL)
    {
        m_pD3dDeviceEx->Release();
        m_pD3dDeviceEx = NULL;
    }
    if (m_hWnd != NULL)
    {
        DestroyWindow(m_hWnd);
        m_hWnd = NULL;
    }
    if (m_pD3DEx != NULL)
    {
        m_pD3DEx->Release();
        m_pD3DEx = NULL;
    }
}

//-------------------------------------------------------------------------------------------
// Function: Helper_VerifyTestResults()
// Purpose:  Verify the test results
//-------------------------------------------------------------------------------------------
HRESULT
CMyTest::Helper_VerifyTestResults
(
)
{
    HRESULT      hrResult = S_OK;  

    // Verify the test results
    for (USHORT i=0; i<ARRAYSIZE(m_testCaseDesc[m_ushCurTestCase].ExpectedResults); i++)
    {
        // continue to check other critetia no matter what hrResult is
        switch (m_testCaseDesc[m_ushCurTestCase].ExpectedResults[i])
        {
            case Verify_Result_InitialCondition:
                if (m_iniD3dMemPressure.LevelOfEfficiency == 0
                ||  m_iniD3dMemPressure.LevelOfEfficiency > 100)
                {
                    WriteToLog(_T("[Helper_VerifyTestResults()]: Verify_Result_InitialConditions Failed! LevelOfEfficiency[%u] is out of range."),
       	            	       m_iniD3dMemPressure.LevelOfEfficiency);
                    WriteToLog(_T("    m_iniD3dMemPressure[%u, %u, %u]]"),
                               m_iniD3dMemPressure.BytesEvictedFromProcess,
                               m_iniD3dMemPressure.SizeOfInefficientAllocation,
       	            	       m_iniD3dMemPressure.LevelOfEfficiency);
                    hrResult |= E_FAIL;
                }
                if (m_iniD3dMemPressure.SizeOfInefficientAllocation == 0
                &&  m_iniD3dMemPressure.LevelOfEfficiency != 100)
                {
                    WriteToLog(_T("[Helper_VerifyTestResults()]: Verify_Result_InitialConditions Failed! SizeOfInefficientAllocation[%u] mismatches LevelOfEfficiency[%u]."),
                               m_iniD3dMemPressure.SizeOfInefficientAllocation,
       	            	       m_iniD3dMemPressure.LevelOfEfficiency);
                    WriteToLog(_T("    m_iniD3dMemPressure[%u, %u, %u]]"),
                               m_iniD3dMemPressure.BytesEvictedFromProcess,
                               m_iniD3dMemPressure.SizeOfInefficientAllocation,
       	            	       m_iniD3dMemPressure.LevelOfEfficiency);
                    hrResult |= E_FAIL;
                }
                if (SUCCEEDED(hrResult))
                {
                    WriteToLog(_T("[Helper_VerifyTestResults()]: Verify_Result_InitialConditions Passed!"));
                }
                break;

            case Verify_Result_NoVideoMemoryAllocation:
                if (m_curD3dMemPressure.LevelOfEfficiency == 0
                ||  m_curD3dMemPressure.LevelOfEfficiency > 100)
                {
                    WriteToLog(_T("[Helper_VerifyTestResults()]: Verify_Result_NoVideoMemoryAllocation Failed! LevelOfEfficiency[%u] is out of range."),
       	            	       m_curD3dMemPressure.LevelOfEfficiency);
                    WriteToLog(_T("    m_curD3dMemPressure[%u, %u, %u]]"),
                               m_curD3dMemPressure.BytesEvictedFromProcess,
                               m_curD3dMemPressure.SizeOfInefficientAllocation,
       	            	       m_curD3dMemPressure.LevelOfEfficiency);
                    hrResult |= E_FAIL;
                }
                if (m_curD3dMemPressure.SizeOfInefficientAllocation == 0
                &&  m_curD3dMemPressure.LevelOfEfficiency != 100)
                {
                    WriteToLog(_T("[Helper_VerifyTestResults()]: Verify_Result_NoVideoMemoryAllocation Failed! SizeOfInefficientAllocation[%u] mismatches LevelOfEfficiency[%u]."),
                               m_curD3dMemPressure.SizeOfInefficientAllocation,
       	            	       m_curD3dMemPressure.LevelOfEfficiency);
                    WriteToLog(_T("    m_curD3dMemPressure[%u, %u, %u]]"),
                               m_curD3dMemPressure.BytesEvictedFromProcess,
                               m_curD3dMemPressure.SizeOfInefficientAllocation,
       	            	       m_curD3dMemPressure.LevelOfEfficiency);
                    hrResult |= E_FAIL;
                }
                if (SUCCEEDED(hrResult))
                {
                    WriteToLog(_T("[Helper_VerifyTestResults()]: Verify_Result_NoVideoMemoryAllocation Passed!"));
                }
                break;

            case Verify_Result_VideoMemoryAllocation:  
                // Some hardware setups may not cause evictions to occur, 
                // but this is not a bug, so we skip it
                if (m_curD3dMemPressure.BytesEvictedFromProcess == 0
                &&  m_curD3dMemPressure.SizeOfInefficientAllocation == 0)
                {
                    WriteToLog(_T("[Helper_VerifyTestResults()]: The test is skipped!\nBytesEvictedFromProcess and SizeOfInefficientAllocation are 0."));
                    WriteToLog(_T("    m_curD3dMemPressure[%u, %u, %u]]"),
                               m_curD3dMemPressure.BytesEvictedFromProcess,
                               m_curD3dMemPressure.SizeOfInefficientAllocation,
       	            	       m_curD3dMemPressure.LevelOfEfficiency);
                    hrResult = E_TESTSKIPPED;
                    goto Exit; 
                }

                if (m_curD3dMemPressure.BytesEvictedFromProcess >= m_preD3dMemPressure.BytesEvictedFromProcess
                &&  m_curD3dMemPressure.SizeOfInefficientAllocation >= m_preD3dMemPressure.SizeOfInefficientAllocation
                &&  m_preD3dMemPressure.LevelOfEfficiency >= m_curD3dMemPressure.LevelOfEfficiency)
                {
                    WriteToLog(_T("[Helper_VerifyTestResults()]: Verify_Result_VideoMemoryAllocation Passed!"));
                }
                else
                {
                    WriteToLog(_T("[Helper_VerifyTestResults()]: Verify_Result_VideoMemoryAllocation Failed!"));
                    WriteToLog(_T("    m_preD3dMemPressure[%u, %u, %u]]"),
                               m_preD3dMemPressure.BytesEvictedFromProcess,
                               m_preD3dMemPressure.SizeOfInefficientAllocation,
       	            	       m_preD3dMemPressure.LevelOfEfficiency);
                    WriteToLog(_T("    m_curD3dMemPressure[%u, %u, %u]]"),
                               m_curD3dMemPressure.BytesEvictedFromProcess,
                               m_curD3dMemPressure.SizeOfInefficientAllocation,
       	            	       m_curD3dMemPressure.LevelOfEfficiency);
                    hrResult |= E_FAIL;
                }
                break;

            case Verify_Result_VideoMemoryDeallocation: 
                if (m_curD3dMemPressure.BytesEvictedFromProcess <= m_preD3dMemPressure.BytesEvictedFromProcess
                &&  m_curD3dMemPressure.SizeOfInefficientAllocation <= m_preD3dMemPressure.SizeOfInefficientAllocation
                &&  m_preD3dMemPressure.LevelOfEfficiency <= m_curD3dMemPressure.LevelOfEfficiency)
                {
                    WriteToLog(_T("[Helper_VerifyTestResults()]: Verify_Result_MemoryDeallocation Passed!"));
                }
                else
                {
                    WriteToLog(_T("[Helper_VerifyTestResults()]: Verify_Result_MemoryDeallocation Failed!"));
                    WriteToLog(_T("    m_preD3dMemPressure[%u, %u, %u]]"),
                               m_preD3dMemPressure.BytesEvictedFromProcess,
                               m_preD3dMemPressure.SizeOfInefficientAllocation,
       	            	       m_preD3dMemPressure.LevelOfEfficiency);
                    WriteToLog(_T("    m_curD3dMemPressure[%u, %u, %u]]"),
                               m_curD3dMemPressure.BytesEvictedFromProcess,
                               m_curD3dMemPressure.SizeOfInefficientAllocation,
       	            	       m_curD3dMemPressure.LevelOfEfficiency);
                    hrResult |= E_FAIL;
                }
                break;

            case Verify_Result_None:  // Ignore all errors, but no exception is allowed 
            default:
                break;
        }
    }

Exit:
    // save current memory report
    m_preD3dMemPressure = m_curD3dMemPressure;
    return hrResult;
}

//--------------------------------------------------------------------------
// Function: WndProc()
// Purpose:  Process windows messages
//--------------------------------------------------------------------------
LRESULT CALLBACK 
CMyTest::WndProc
(
    __in HWND        hWnd, 
    __in UINT        uiMessage, 
    __in WPARAM      wParam, 
    __in LPARAM      lParam
)
{
    switch(uiMessage)
    {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if(wParam == VK_ESCAPE)
            {
                PostQuitMessage(0);
            }
            return 0;

        case WM_CLOSE:
            PostQuitMessage( 0 );
            return 0;
    };
    return DefWindowProc(hWnd, uiMessage, wParam, lParam);
}