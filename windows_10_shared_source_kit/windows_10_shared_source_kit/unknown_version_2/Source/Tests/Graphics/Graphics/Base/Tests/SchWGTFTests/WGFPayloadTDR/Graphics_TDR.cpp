/******************************Module*Header*******************************
* Module Name: WGFPayloadTDR.h
*
* D3D11 Graphics and Compute TDR workload test
*
* Author: David Flenniken [davidfl]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#include "internal.h"

#include <d3dcompiler.h>
#include <D3D11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>

#include "Globals.h"
#include <Quadrant.h>
#include "Graphics_TDR.h"
#include "WGFPayloadTDRResources.h"

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
LRESULT CALLBACK    RenderWndProc( HWND, UINT, WPARAM, LPARAM );

extern char* SHADER_TDR;
#ifdef WANT_FUNCTIONAL_SHADER
//extern char* SHADER_TDR;
extern const char* PSCONST_SHADER;
#endif

//--------------------------------------------------------------------------------------
// Vertex shader and pixel shader source code
//--------------------------------------------------------------------------------------
#ifdef WANT_YELLOW_TRIANGLES
CHAR* g_strVS = 
    "void VS( in float4 posIn : POSITION,\n"
    "         out float4 posOut : SV_Position )\n"
    "{\n"
    "    // Output the vertex position, unchanged\n"
    "    posOut = posIn;\n"
    "}\n";

CHAR* g_strPS = 
    "void PS( out float4 colorOut : SV_Target )\n"
    "{\n"
    "    // Make each pixel yellow, with alpha = 1\n"
    "    colorOut = float4( 1.0f, 1.0f, 0.0f, 1.0f );\n"
    "}\n";
#else
CHAR* g_strVS = 
    "void VS( in float4 posIn : POSITION,\n"
    "         out float4 posOut : SV_Position )\n"
    "{\n"
    "    // Output the vertex position, unchanged\n"
    "    posOut = posIn;\n"
    "}\n";
CHAR* g_strPS = 
    "void PS( out float4 colorOut : SV_Target )\n"
    "{\n"
    "    // Make each pixel orange, with alpha = 0.8\n"
    "    colorOut = float4( 1.0f, 0.5f, 0.0f, 0.8f );\n"
    "}\n";

#endif

//------------------------------------------------------------------------------

// Only support EquilateralTriangle for proof of concept
// _PixelsPerTriangle = (1/2)b*b
CTriangle::CTriangle(TriangleType _Type,
                     UINT _iPixelsPerTriangle):
    m_Base(0.0),
    m_iBase(0),
    m_iHeight(0)
{
    int iPixels=0;
    m_Type = _Type;
    switch(m_Type)
    {
    case EquilateralTriangle:
        {
            // Determine the height and base
            m_Base=sqrt((double)(2*_iPixelsPerTriangle));
            m_iBase=(int)m_Base;
            m_iHeight=(int)m_Base;

            iPixels = (int)((m_iBase*m_iHeight)/2);
        }
        break;
    case RightTriangle:
    case IsoscelesTriangle:
    case ScaleneTriangle:
    case ObtuseTriangle:
    default:
        {
            return;
        }
        break;
    }

#ifdef DBG
    char szBuf[128];
    StringCchPrintf(szBuf,128,"m_Base=%.2f, m_iBase=%d, m_iHeight=%d, effective pixels=%d\n\r",
                    m_Base,m_iBase,m_iHeight,iPixels);
    OutputDebugString(szBuf);
#endif
}

CTriangle::~CTriangle()
{
}

int 
CTriangle::Base()
{
    return m_iBase;
}

int 
CTriangle::Height()
{
    return m_iHeight;
}

int
CTriangle::Pixels()
{
    return (int)((m_iBase*m_iHeight)/2);
}



CWorkLoad::CWorkLoad(ID3D11Device* _pDevice,UINT _Width, UINT _Height):
    m_pVertices(NULL),
    m_pVB(NULL),
    m_pInputLayout(NULL),
    m_pVertexShader(NULL),
    m_Width(_Width),
    m_Height(_Height)
{
    m_pDevice = _pDevice;
}

CWorkLoad::~CWorkLoad()
{
    if( m_pVertices )
    {
        delete m_pVertices;
    }
}


ID3D11Buffer* CWorkLoad::GetVertexBuffer()
{
    return m_pVB;
}

UINT CWorkLoad::GetNumberOfVertices()
{
    return m_nVertices;
}
UINT CWorkLoad::GetVertexSize()
{
    return m_uiVertexSize;
}

ID3D11InputLayout* CWorkLoad::GetInputLayout()
{
    return m_pInputLayout;
}

ID3D11VertexShader* CWorkLoad::GetVertexShader()
{
    return m_pVertexShader;
}


// Create the vertex buffer and fill it in with our data.
void CWorkLoad::Initialize(CTriangle* _pTriangle,
                           int _iTriangles,
                           bool _bcase)
{
    HRESULT hr;

    m_nVertices = _iTriangles * 3;
#ifdef WANT_YELLOW_TRIANGLES
    m_uiVertexSize = sizeof(SimpleVertex);
#else
    m_uiVertexSize = sizeof(Vertex);
#endif

    // Compile and create the vertex shader
    DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#ifdef _DEBUG
    dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif
    ID3D10Blob* pBlobVS = NULL;
    ID3D10Blob* pBlobError = NULL;

    hr = D3DCompile( g_strVS,                  // LPCVOID pSrcData
                     lstrlenA( g_strVS ) + 1,  // SIZE_T SrcDataSize
                     "VS",                     // LPCSTR pSourceName
                     NULL,                     // const D3D10_SHADER_MACRO *pDefines
                     NULL,                     // LPD3D10INCLUDE pInclude
                     "VS",                     // LPCSTR pEntrypoint
                     (m_pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_0 ? "vs_4_0" : "vs_4_0_level_9_1"), 
                     dwShaderFlags,            // UINT Flags1
                     0,                        // UINT Flags2
                     &pBlobVS, 
                     &pBlobError );
                     
    if( FAILED( hr ) )
    {
        if( pBlobError != NULL )
        {
            OutputDebugStringA( (CHAR*)pBlobError->GetBufferPointer() );
            pBlobError->Release();
        }
        PATH_TRAP();
        throw std::bad_alloc();
    }
    hr = m_pDevice->CreateVertexShader( pBlobVS->GetBufferPointer(), pBlobVS->GetBufferSize(), 
                                        NULL, &m_pVertexShader );
    if( FAILED( hr ) )
    {
        PATH_TRAP();
        throw std::bad_alloc();
    }


#ifdef WANT_YELLOW_TRIANGLES
    // Create the input layout
    D3D11_INPUT_ELEMENT_DESC elements[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = _countof( elements );
#else
    // Create the input layout
    D3D11_INPUT_ELEMENT_DESC elements[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = _countof( elements );
#endif

    hr = m_pDevice->CreateInputLayout( elements, numElements, pBlobVS->GetBufferPointer(),
                                       pBlobVS->GetBufferSize(), &m_pInputLayout );
    if( FAILED( hr ) )
    {
        PATH_TRAP();
        throw std::bad_alloc();
    }

    pBlobVS->Release();


#ifdef WANT_YELLOW_TRIANGLES

    // Create vertex buffer
    SimpleVertex vertices[] =
    {
        D3DXVECTOR3( 0.0f, 0.5f, 0.5f ),
        D3DXVECTOR3( 0.5f, -0.5f, 0.5f ),
        D3DXVECTOR3( -0.5f, -0.5f, 0.5f ),

        D3DXVECTOR3( 0.01f, 0.51f, 0.51f ),
        D3DXVECTOR3( 0.51f, -0.51f, 0.51f ),
        D3DXVECTOR3( -0.51f, -0.51f, 0.51f ),

        D3DXVECTOR3( 0.02f, 0.52f, 0.52f ),
        D3DXVECTOR3( 0.52f, -0.52f, 0.52f ),
        D3DXVECTOR3( -0.52f, -0.52f, 0.52f ),

        D3DXVECTOR3( 0.03f, 0.53f, 0.53f ),
        D3DXVECTOR3( 0.53f, -0.53f, 0.53f ),
        D3DXVECTOR3( -0.53f, -0.53f, 0.53f ),

        D3DXVECTOR3( 0.04f, 0.54f, 0.54f ),
        D3DXVECTOR3( 0.54f, -0.54f, 0.54f ),
        D3DXVECTOR3( -0.54f, -0.54f, 0.54f ),

        D3DXVECTOR3( 0.05f, 0.55f, 0.55f ),
        D3DXVECTOR3( 0.55f, -0.55f, 0.55f ),
        D3DXVECTOR3( -0.55f, -0.55f, 0.55f ),

        D3DXVECTOR3( 0.06f, 0.56f, 0.56f ),
        D3DXVECTOR3( 0.56f, -0.56f, 0.56f ),
        D3DXVECTOR3( -0.56f, -0.56f, 0.56f ),

        D3DXVECTOR3( 0.07f, 0.57f, 0.57f ),
        D3DXVECTOR3( 0.57f, -0.57f, 0.57f ),
        D3DXVECTOR3( -0.57f, -0.57f, 0.57f ),

        D3DXVECTOR3( 0.08f, 0.58f, 0.58f ),
        D3DXVECTOR3( 0.58f, -0.58f, 0.58f ),
        D3DXVECTOR3( -0.58f, -0.58f, 0.58f ),

        D3DXVECTOR3( 0.09f, 0.59f, 0.59f ),
        D3DXVECTOR3( 0.59f, -0.59f, 0.59f ),
        D3DXVECTOR3( -0.59f, -0.59f, 0.59f ),

        D3DXVECTOR3( 0.08f, 0.58f, 0.58f ),
        D3DXVECTOR3( 0.58f, -0.58f, 0.58f ),
        D3DXVECTOR3( -0.58f, -0.58f, 0.58f ),

        D3DXVECTOR3( 0.07f, 0.57f, 0.57f ),
        D3DXVECTOR3( 0.57f, -0.57f, 0.57f ),
        D3DXVECTOR3( -0.57f, -0.57f, 0.57f ),

        D3DXVECTOR3( 0.06f, 0.56f, 0.56f ),
        D3DXVECTOR3( 0.56f, -0.56f, 0.56f ),
        D3DXVECTOR3( -0.56f, -0.56f, 0.56f ),

        D3DXVECTOR3( 0.05f, 0.55f, 0.55f ),
        D3DXVECTOR3( 0.55f, -0.55f, 0.55f ),
        D3DXVECTOR3( -0.55f, -0.55f, 0.55f ),

        D3DXVECTOR3( 0.04f, 0.54f, 0.54f ),
        D3DXVECTOR3( 0.54f, -0.54f, 0.54f ),
        D3DXVECTOR3( -0.54f, -0.54f, 0.54f ),

        D3DXVECTOR3( 0.03f, 0.53f, 0.53f ),
        D3DXVECTOR3( 0.53f, -0.53f, 0.53f ),
        D3DXVECTOR3( -0.53f, -0.53f, 0.53f ),

        D3DXVECTOR3( 0.02f, 0.52f, 0.52f ),
        D3DXVECTOR3( 0.52f, -0.52f, 0.52f ),
        D3DXVECTOR3( -0.52f, -0.52f, 0.52f ),

        D3DXVECTOR3( 0.01f, 0.51f, 0.51f ),
        D3DXVECTOR3( 0.51f, -0.51f, 0.51f ),
        D3DXVECTOR3( -0.51f, -0.51f, 0.51f ),
    };
    D3D11_BUFFER_DESC bd;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = m_uiVertexSize * 3 * 18;//3
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertices;
    m_pDevice->CreateBuffer( &bd, &InitData, &m_pVB );
    if( FAILED( hr ) )
    {
        PATH_TRAP();
        throw std::bad_alloc();
    }

#else

    // Allocate a buffer that represents our data.

    m_pVertices = (Vertex*)new Vertex[m_nVertices];
    if(NULL == m_pVertices)
    {
        PATH_TRAP();
        throw std::bad_alloc();
    }

    // Because our triangle is placed in the center of the window, the X and Y coordinates
    // need to range between -1 to 1. zero being the center of the screen.
    // Yet we want a triangle of a particular pixel size. So, to figure out what the 
    // coordinate location needs to be (-1 to 1 float) we need to figure out the width 
    // of the screen. 
    //
    // So, if the screen width is 640, 480 (the default - width, height) we know that:
    //
    // the XHalfScreen = width/2
    // the YHalfScreen = height/2
    //
    // Thus, there are XHalfScreen units between 0 and 1 for the X locations.
    //
    // So, if the user specified a triangle of 40,000 pixels, it will have a base
    // of 282 and a height of 282. 1/2 this number is 141. Thus, we'll want to choose
    // a coordinate 141 units of the 240 in the positive direction.
    //
    // 240/141 = 1/x -> 240x = 141  -> x = 141/240 -> 0.5875. 
    //
    // Thus to compute our locations:
    //
    // x = ((float)_pTriangle->Base()/2)/XHalfScreen
    // y = ((float)_pTriangle->Height()/2)YHalfScreen
    //
    // We must validate that the *HalfScreen values are larger then the 1/2 triangle values.
    // In other words, if the triangle is bigger than the client area our coordinate will
    // fall outside the client area 1.X. This is not bad, but there will be clipping which
    // may affect result numbers.

    float fx = (((float)_pTriangle->Base()/(float)2)/((float)m_Width/(float)2));
    float fy = ((float)_pTriangle->Height()/(float)2)/((float)m_Height/(float)2);

    float fxNeg = fx * -1.0f;
    float fyNeg = fy * -1.0f;

    // now fill the allocated memory with stuff
    for(int i = 0; i < _iTriangles; i++)
    {
        Vertex* pStart = &m_pVertices[i * 3];

        pStart[0].Pos = D3DXVECTOR4(  0.0f,     fy, 1.0f, 1.0f );
        pStart[1].Pos = D3DXVECTOR4(    fx,  fyNeg, 1.0f, 1.0f );
        pStart[2].Pos = D3DXVECTOR4( fxNeg,  fyNeg, 1.0f, 1.0f );
    }

    // Now stack the triangles from back to front so they all have to be processed.
    for(UINT i = 0; i < m_nVertices; i++)
    {
        m_pVertices[i].Pos.z = 1.0f - (i / (float)_iTriangles);//back to front
    }    

    // Fill in a buffer description.
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth        = m_uiVertexSize * m_nVertices;
    bufferDesc.BindFlags        = D3D10_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags   = 0;
    bufferDesc.MiscFlags        = 0;

    // Fill in the subresource data.
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = m_pVertices;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    // Create the vertex buffer.
    hr = m_pDevice->CreateBuffer( &bufferDesc, &InitData, &m_pVB );
    if( FAILED(hr) )
    {
        PATH_TRAP();
        throw std::bad_alloc();
    }
#endif
}

//------------------------------------------------------------------------------
Graphics_TDR::Graphics_TDR() :
    m_bTDRShader(false), // Command Line argument
    m_uiTime(1),         // Command Line argument
    m_uiPixelsPerTriangle(1), // Command Line argument
    m_uiTrianglesPerPrimitive(1), // Command Line argument
    m_uiPrimitivesPerFrame(1), // Command Line argument 
    m_uiLoopsInShader(1), // Command Line argument
    m_uiCreateDeviceFlags(0), // Command Line argument

    m_fl(D3D_FEATURE_LEVEL_10_1),

    g_hWnd(NULL),
    g_driverType(D3D_DRIVER_TYPE_UNKNOWN),
    g_pd3dDevice(NULL),
    g_pSwapChain(NULL),
    g_pRenderTargetView(NULL),
    m_pImmediateContext(NULL),

    m_pDxgiDevice2(NULL),
    m_hEvent(NULL),

    m_DxgiFactory(NULL),
    m_DxgiAdapter(NULL),

    g_pInputLayout(NULL),
    g_pVertexBuffer(NULL),
    g_pVertexShader(NULL),
    g_pPixelShader(NULL),

    m_pTriangle(NULL),
    m_pWorkLoad(NULL),

    m_uiWidth(640),
    m_uiHeight(480),

    m_pWindowLoc(NULL)
{
}

Graphics_TDR::~Graphics_TDR()
{
    if(m_pTriangle) 
    {
        delete m_pTriangle;
    }
    if( m_pWorkLoad )
    {
        delete m_pWorkLoad;
    }
    if( m_pWindowLoc )
    {
        delete m_pWindowLoc;
    }
};


void Graphics_TDR::SetTDRShaderOn()
{
    m_bTDRShader = true;
}
void Graphics_TDR::SetTime(UINT _uiTime)
{
    m_uiTime = _uiTime;
}

HRESULT Graphics_TDR::SetPixelsPerTriangle(UINT _PixelsPerTriangle)
{
    // The Pixels per triangle value need to be coorilated with the window size.
    // If the user asks for more pixels than what the window can hold, we'll clip
    // the corners of the triangle. Thus, we really want to have a window big enough
    // to handle the values. Thus, we'll calculate our window size based on the 
    // desired pixels.

    if( 0 == m_Quadrant )
    {
        CTriangle tForWindow(EquilateralTriangle,_PixelsPerTriangle);

        m_uiWidth = max((UINT)tForWindow.Base()+2,m_uiWidth);
        m_uiHeight = max((UINT)tForWindow.Height()+2,m_uiHeight);
    }

    m_uiPixelsPerTriangle = _PixelsPerTriangle;
    return S_OK;
}
HRESULT Graphics_TDR::SetTrianglesPerPrimitive(UINT _TrianglesPerPrimitive)
{
    // The largest number of triangles that we can accept is 715,827,882 which, when multiplied
    // by 3 hits the limit for a int - which is what we pass to Draw below.
    if( _TrianglesPerPrimitive > 715827882 )
    {
        PATH_TRAP();
        return ERROR_INVALID_DATA;
    }

    m_uiTrianglesPerPrimitive = _TrianglesPerPrimitive;
    return S_OK;
}

HRESULT Graphics_TDR::SetPrimitivesPerFrame(UINT _PrimitivesPerFrame)
{
    m_uiPrimitivesPerFrame = _PrimitivesPerFrame;
    return S_OK;
}

HRESULT Graphics_TDR::SetLoopsInShader(UINT _LoopsInShader)
{
    m_uiLoopsInShader = _LoopsInShader;
    return S_OK;
}

HRESULT Graphics_TDR::SetNonInteractive()
{
    m_uiCreateDeviceFlags |= D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT; // 0x00000100 is DisableGpuTimeout
    return S_OK;
}


HRESULT Graphics_TDR::SetQuadrant(UINT _Quadrant)
{
    // User wants to use a Quadrant of the screen
    m_Quadrant = _Quadrant;
    CQuadrant* pWindowLoc = new CQuadrant(NULL,m_Quadrant,SIDE_PIXELS);
    if( pWindowLoc )
    {
        // now adjust our triangle size so that it matches the window size - at max.
        m_uiWidth = max(pWindowLoc->Width(),m_uiWidth);
        m_uiHeight = max(pWindowLoc->Height(),m_uiHeight);

        delete pWindowLoc;
        return S_OK;
    }
    return S_FALSE;
}

HRESULT Graphics_TDR::SelectAdapter(bool _bNullAdapter)
{
    m_bNullAdapter = _bNullAdapter;
    return S_OK;
}

HRESULT Graphics_TDR::SelectDeviceType(bool _bSoftwareDevice)
{
    m_bSoftwareDevice = _bSoftwareDevice;
    return S_OK;
}

HRESULT Graphics_TDR::SetFeatureLevel(D3D_FEATURE_LEVEL _fl)
{
    m_fl = _fl;
    return S_OK;
}



HRESULT Graphics_TDR::Initialize()
{
    HRESULT hr = S_OK;

    // Create the window that we want to render into.
    hr = InitWindow( NULL, SW_SHOWNORMAL );
    if( FAILED( hr ) )
    {
        PATH_TRAP();
        return hr;
    }

    // Now setup our rendering conditions.
    hr = InitDeviceAndSwapChain();
    if( FAILED( hr ) )
    {
        return hr;
    }

    hr = InitDeviceObjects();
    if( FAILED( hr ) )
    {
        CleanupDevice();
        return hr;
    }


    // Create our vertex workloads based on user preferences
    m_pTriangle = new CTriangle(EquilateralTriangle,m_uiPixelsPerTriangle);
    if( NULL == m_pTriangle )
    {
        PATH_TRAP();
        throw std::bad_alloc();
    }

    // Create our workload
    m_pWorkLoad = new CWorkLoad(g_pd3dDevice,m_uiWidth,m_uiHeight);
    if( NULL == m_pWorkLoad )
    {
        PATH_TRAP();
        throw std::bad_alloc();
    }

    // initialize with the triangle primitive we want to use and the number of triangles.
    m_pWorkLoad->Initialize(m_pTriangle,m_uiTrianglesPerPrimitive,true);

    g_pInputLayout = m_pWorkLoad->GetInputLayout();
    g_pVertexBuffer = m_pWorkLoad->GetVertexBuffer();
    g_pVertexShader = m_pWorkLoad->GetVertexShader();

    InitDeviceContext();

    return hr;
}

HRESULT Graphics_TDR::Work()
{
    HRESULT hr = S_OK;

    // If the user is asking for a particular window placement, let's honor their request.
    // Note that we are going to limit the window size to the size of the quadrant.
    if( m_Quadrant )
    {
        m_pWindowLoc = new CQuadrant(g_hWnd,m_Quadrant,SIDE_PIXELS);
    }

    // Always want our window to be on top so nothing is clipped.
    SetForegroundWindow(g_hWnd);

    time_t StartTime = time(NULL);
    time_t Diff=0;

    // Main message loop
    MSG msg = {0};
    bool bRunning=true;
    while( WM_QUIT != msg.message && bRunning)
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            // Have we run long enough?
            time_t CurrentTime = time(NULL);
            Diff = CurrentTime - StartTime;

            // If we need to move to a particular quadrant of the screen, we'll have
            // to wait a little while before starting to work.
            if(m_Quadrant)
            {
                if( ((UINT)Diff >= 1) )
                {
                    hr = Render();
                }
            }
            else
            {
                hr = Render();
            }
            
            // if we TDR (or incounter another error, we'll have a non-success condition
            // describved in hr.
            if( S_OK != hr )
            {
                //PostQuitMessage( 0 );
                bRunning=false;
            }

            // if time is up, we want to post a quit message so this loop will terminate
            if( ((UINT)Diff >= m_uiTime) )
            {
                //PostQuitMessage( 0 );
                bRunning=false;
            }
        }
    }

    SetForegroundWindow(g_hWnd);
    return hr;
}

HRESULT Graphics_TDR::WaitForDMABufferCompletion()
{
    HRESULT hr = S_OK;

    if( m_pDxgiDevice2 )
    {
        hr = m_pDxgiDevice2->EnqueueSetEvent( m_hEvent );

        if ( S_OK == hr )
        {
            WaitForSingleObject( m_hEvent, INFINITE );
        }
    }

    return hr;
}

HRESULT Graphics_TDR::Cleanup()
{
    HRESULT hr = S_OK;

    CleanupDevice();

    DestroyWindow( g_hWnd );

    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }

    g_hWnd=NULL;

    if (m_DxgiFactory != NULL)
    {
        m_DxgiFactory->Release();
        m_DxgiFactory = NULL;
    }
    if (m_DxgiAdapter != NULL)
    {
        m_DxgiAdapter->Release();
        m_DxgiAdapter = NULL;
    }

    return hr;
}


















//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT Graphics_TDR::InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    if( !g_hWnd )
    {
        // Register class
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof( WNDCLASSEX );
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = RenderWndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
        wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
        wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = "TutorialWindowClass";
        wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
        RegisterClassEx( &wcex );

        // Create window
        RECT rc = { 0, 0, m_uiWidth, m_uiHeight };
        AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
        g_hWnd = CreateWindow( "TutorialWindowClass", "Direct3D 11: Setting Up Window", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                               NULL );
        if( !g_hWnd )
            return E_FAIL;

        ShowWindow( g_hWnd, nCmdShow );
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK RenderWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_CHAR:
            if( wParam == VK_ESCAPE )
                DestroyWindow( hWnd );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}


//-- * ++ * -- * ++ * -- * ++ * -- * ++ * -- * ++ * -- * ++ * -- * ++ * -- * ++ * --

#ifdef WANT_SIMULATE
int giSimulateFailureCount=0;
#endif


//
// This method enumerates the adapters on the machine looking for the first device that 
// is not "Microsoft Basic Render Driver".  It will return S_OK if it finds a valid
// device and set m_DxgiAdapter. m_DxgiFactory is also set by default.
//
HRESULT Graphics_TDR::EnumerateAdapters(bool _bWantBasicRender)
{
    HRESULT hr = S_OK;;

    // If we're not going to use the NULL adapter, we need to enumerate for
    // an adapter here.

    if( NULL == m_DxgiFactory )
    {
        // Only call this routine once.
        hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&m_DxgiFactory);
        if( FAILED( hr ) )
        {
            PATH_TRAP();
#ifdef DBG
            // output the feature level
            char szBuf[MAX_PATH];
            StringCchPrintf(szBuf,MAX_PATH,"CreateDXGIFactory1 failed hResult=%d(0x%x)\n\r",
                            hr,hr);
            OutputDebugString(szBuf);
#endif
            return hr;
        }
    }

    for (UINT adapterIndex = 0; m_DxgiAdapter == NULL; ++adapterIndex)
    {
        IDXGIAdapter1* dxgiAdapter = NULL;

        hr = m_DxgiFactory->EnumAdapters1(adapterIndex, &dxgiAdapter);
        if (FAILED(hr))
        {
            if (hr == DXGI_ERROR_NOT_FOUND)
            {
#ifdef DBG
                // output the feature level
                char szBuf[MAX_PATH];
                StringCchPrintf(szBuf,MAX_PATH,"EnumAdapters1 ran out of adapters, hResult=%d(0x%x)\n\r",
                                hr,hr);
                OutputDebugString(szBuf);
#endif
            }
            else
            {
                PATH_TRAP();
#ifdef DBG
                // output the feature level
                char szBuf[MAX_PATH];
                StringCchPrintf(szBuf,MAX_PATH,"EnumAdapters1 failed, hResult=%d(0x%x)\n\r",
                                hr,hr);
                OutputDebugString(szBuf);
#endif
            }
            return hr;
        }

        DXGI_ADAPTER_DESC1 adapterDesc = {};
        hr = dxgiAdapter->GetDesc1(&adapterDesc);
        if (FAILED(hr))
        {
            PATH_TRAP();
#ifdef DBG
            // output the feature level
            char szBuf[MAX_PATH];
            StringCchPrintf(szBuf,MAX_PATH,"EGetDesc1 failed, hResult=%d(0x%x)\n\r",
                            hr,hr);
            OutputDebugString(szBuf);
#endif
            return hr;
        }

#ifdef DBG
        // output the feature level
        char szBuf[MAX_PATH];
        StringCchPrintf(szBuf,MAX_PATH,"Enumerated Adapter Name: %S\n\r",adapterDesc.Description);
        OutputDebugString(szBuf);
#endif
        // Now we need to check to see if this adapter has the capabilities that we're enumerating
        // for - if it's not the microsoft basic render driver

        if( _wcsnicmp(adapterDesc.Description,
                      L"Microsoft Basic Render Driver", 
                      sizeof(L"Microsoft Basic Render Driver")) == 0 )
        {
            // Found the software driver
            if( _bWantBasicRender )
            {
                m_DxgiAdapter = dxgiAdapter;
            }
            else
            {
                dxgiAdapter->Release();
            }
        }
        else
        {
            // Found something other than the software driver
            if( _bWantBasicRender )
            {
                dxgiAdapter->Release();
            }
            else
            {
                m_DxgiAdapter = dxgiAdapter;
            }
        }
    }

    return hr;
}

HRESULT Graphics_TDR::InitDeviceAndSwapChain()
{
    HRESULT hr = S_OK;;
    RECT rc;
    GetClientRect( g_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

//------------------------------------------------------------------------------

    // If we where to use NULL as the first parameter to D3D11CreateDeviceAndSwapChain(), that
    // function will enumerate adapters for the first one - which it considers the NULL device.
    // Thus, this is what we'll do in our loop too. When we loop, we'll make sure that
    // the device is NOT "Microsoft Basic Render Driver". If it is not, we'll assume that
    // it's a hardware device.
    // 
    // Note that there is no way to determine what type of adapter this enumerated adapter
    // is. The only method available is GetDesc() which returns limited info.

    if( !m_bNullAdapter )
    {
        hr = EnumerateAdapters(m_bSoftwareDevice);
        if( S_OK != hr )
        {
            return hr;
        }
    }

//------------------------------------------------------------------------------

    D3D_DRIVER_TYPE driverTypesHW[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypesHW = sizeof( driverTypesHW ) / sizeof( driverTypesHW[0] );
    D3D_DRIVER_TYPE driverTypesSW[] =
    {
        D3D_DRIVER_TYPE_SOFTWARE,
        D3D_DRIVER_TYPE_WARP,
    };
    UINT numDriverTypesSW = sizeof( driverTypesSW ) / sizeof( driverTypesSW[0] );
    UINT numfeatureLevels = 1;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferCount = 1;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    UINT numDriverTypes = numDriverTypesHW;
    if( m_bSoftwareDevice )
    {
        numDriverTypes = numDriverTypesSW;
    }

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        if( m_bSoftwareDevice )
        {
            g_driverType = driverTypesSW[driverTypeIndex];
        }
        else
        {
            g_driverType = driverTypesHW[driverTypeIndex];
        }
        D3D_FEATURE_LEVEL d3dFeatureLevel=D3D_FEATURE_LEVEL_9_3;

        IDXGIAdapter* pAdapter = NULL;
        if( !m_bNullAdapter )
        {
            pAdapter = m_DxgiAdapter;
            g_driverType = D3D_DRIVER_TYPE_UNKNOWN;
        }
#ifdef DBG
        m_uiCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        hr = D3D11CreateDeviceAndSwapChain(pAdapter,                // IDXGIAdapter *
                                           g_driverType,            // D3D_DRIVER_TYPE
                                           NULL,                    // HMODULE
                                           m_uiCreateDeviceFlags,   // UINT
                                           &m_fl,                   // const D3D_FEATURE_LEVEL *
                                           numfeatureLevels,        // UINT
                                           D3D11_SDK_VERSION,       // UINT
                                           &sd,                     // const DXGI_SWAP_CHAIN_DESC *

                                           &g_pSwapChain,           // out: IDXGISwapChain **
                                           &g_pd3dDevice,           // out: ID3D11Device **
                                           &d3dFeatureLevel,        // out: D3D_FEATURE_LEVEL *
                                           &m_pImmediateContext);   // out: ID3D11DeviceContext **

        if( SUCCEEDED( hr ) )
        {
#ifdef DBG
            // output the feature level
            char szBuf[MAX_PATH];
            StringCchPrintf(szBuf,MAX_PATH,"ProcessId=%d Dx Feature level=%d(0x%x)\n\r",
                            GetCurrentProcessId(),d3dFeatureLevel,d3dFeatureLevel);
            OutputDebugString(szBuf);
#endif
            break;
        }

        // if context is NULL, we've got a problem.
        if( NULL == m_pImmediateContext )
        {
            PATH_TRAP();
        }

    }
#ifdef WANT_SIMULATE
    // When we simulate a failure, we're going to fail everything after the 6th attempt
    // but only if we're not using software. Thus, every attempt to access hardware
    // after 6 TDRs fails. Note that we're returning E_ABORT here as our custom error.
    giSimulateFailureCount++;
    if( S_OK == hr && giSimulateFailureCount >= 6 && !m_bSoftwareDevice )
    {
        hr = E_ABORT;
#ifdef DBG
       char szBuf[MAX_PATH];
       StringCchPrintf(szBuf,MAX_PATH,"Faking a device creation failure, attempt=%d.\n\r",
                       giSimulateFailureCount);
       OutputDebugString(szBuf);
#endif
    }
    else if( S_OK != hr )
    {
        PATH_TRAP();
    }

#endif
    return hr;
}

HRESULT Graphics_TDR::InitDeviceObjects()
{
    HRESULT hr = S_OK;

    hr = g_pd3dDevice->QueryInterface( __uuidof(m_pDxgiDevice2), (void**)&m_pDxgiDevice2 );
    if( S_OK == hr)
    {
        m_hEvent = CreateEvent( NULL, false, false, NULL );
        if( NULL == m_hEvent )
            return E_OUTOFMEMORY;
    }

    // Create a render target view
    ID3D11Texture2D* pBackBuffer;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    // Compile and create the vertex shader
    DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#ifdef _DEBUG
    dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif
    ID3D10Blob* pBlobError = NULL;
//********** --------- ********** --------- ********** --------- ********** --------- 
#define WANT_FUNCTIONAL_SHADER
#ifdef WANT_FUNCTIONAL_SHADER
    // Dynamically adjust the number of loops in the shader based on user input.
    size_t tSize = strlen(SHADER_TDR)*2;
    size_t tActualSize=0;

    char* pShaderString = (char*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, tSize);
    if( NULL == pShaderString )
    {
        ExitProcess(2);
    }

    // now read our string and add in our loop count
    hr = StringCchPrintf(pShaderString,tSize,SHADER_TDR,m_uiLoopsInShader);
    if( S_OK != hr )
    {
        ExitProcess(2);
    }
    hr = StringCchLength(pShaderString,tSize,&tActualSize);
    if( S_OK != hr )
    {
        ExitProcess(2);
    }

    // Now create our shader

    const D3D_SHADER_MACRO defines[] = 
    {
        "Graphics_Shader","1",
        NULL, NULL
    };

#endif
//********** --------- ********** --------- ********** --------- ********** --------- 
    ID3D10Blob* pBlobPS = NULL;
    // Compile and create the pixel shader
    hr = D3DCompile( g_strPS, 
                     lstrlenA( g_strPS ) + 1, 
                     "PS", 
                     NULL, 
                     NULL, 
                     "PS", 
                     (g_pd3dDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_0 ? "ps_4_0" : "ps_4_0_level_9_1"), 
                     dwShaderFlags, 
                     0, 
                     &pBlobPS, 
                     &pBlobError );

    if( FAILED( hr ) )
    {
        if( pBlobError != NULL )
        {
            OutputDebugStringA( (CHAR*)pBlobError->GetBufferPointer() );
            pBlobError->Release();
        }
        return hr;
    }

    hr = g_pd3dDevice->CreatePixelShader( pBlobPS->GetBufferPointer(), pBlobPS->GetBufferSize(), 
                                       NULL, &g_pPixelShader );
    if( FAILED( hr ) )
        return hr;
    pBlobPS->Release();

    return hr;
}

void Graphics_TDR::InitDeviceContext()
{
    // Just validate that we have all our pointers.
    if( NULL == g_pInputLayout ||
        NULL == g_pVertexBuffer ||
        NULL == g_pVertexShader || 
        NULL == g_pPixelShader || 
        NULL == g_pRenderTargetView) 
    {
        PATH_TRAP();
        throw std::bad_alloc();
    }
    m_pImmediateContext->IASetInputLayout( g_pInputLayout );

    UINT stride = m_pWorkLoad->GetVertexSize();
    UINT offset = 0;
    m_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );

    m_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    m_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );

    m_pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)(rc.right - rc.left);
    vp.Height = (FLOAT)(rc.bottom - rc.top);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pImmediateContext->RSSetViewports( 1, &vp );

    m_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, NULL );
}




//--------------------------------------------------------------------------------------
// Render the frame
//--------------------------------------------------------------------------------------
int giFlip=0;
HRESULT Graphics_TDR::Render()
{
    HRESULT hr; 

    //Abort the render calls if Context or Swapchain are NULL
    if(NULL == m_pImmediateContext || NULL == g_pSwapChain)
    {
        PATH_TRAP();
        return E_ABORT;
    }

    // Just clear the backbuffer
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
    m_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );

    UINT uinVertices = m_pWorkLoad->GetNumberOfVertices();
#ifdef WANT_YELLOW_TRIANGLES
    // We're going to draw one of the many triangles each time through.
    m_pImmediateContext->Draw( 3, (giFlip++%18)*3 );
#else
    for( UINT k=0;k<m_uiPrimitivesPerFrame;k++)
    {
        // We're goint to draw all the triangles each time through.
        m_pImmediateContext->Draw( uinVertices, 0 );
    }
#endif
    // Present the information rendered to the back buffer to the front buffer (the screen)
    hr = g_pSwapChain->Present( 0, 0 );

    // now check the return codes to see if there is anything we need to do.
    if( DXGI_STATUS_OCCLUDED == hr )
    {
        // window is not visible, this is not good. we have to have our window visible!
        // For now, we'll let our standard error path handle the error.
        PATH_TRAP();
    }
    return hr;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void Graphics_TDR::CleanupDevice()
{
    if( m_pImmediateContext ) m_pImmediateContext->ClearState();
    
    if( m_pDxgiDevice2 ) m_pDxgiDevice2->Release();
    if( m_hEvent ) CloseHandle(m_hEvent);

    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();

    if( g_pVertexBuffer ) g_pVertexBuffer->Release();
}

//-- * ++ * -- * ++ * -- * ++ * -- * ++ * -- * ++ * -- * ++ * -- * ++ * -- * ++ * --

