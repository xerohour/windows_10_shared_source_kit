/******************************Module*Header*******************************
* Module Name: WGFPayloadTDR.h
*
* D3D11 Graphics and Compute TDR workload test
*
* Author: David Flenniken [davidfl]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#pragma once

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

// Comment out the following line to use raw buffers instead of structured buffers
#define USE_STRUCTURED_BUFFERS

#define MAX_THREADS 100
#define BUF_SIZE 255

typedef enum _TriangleType {
    EquilateralTriangle,
    RightTriangle,
    IsoscelesTriangle,
    ScaleneTriangle,
    ObtuseTriangle
} TriangleType;

class CTriangle
{
private:
    TriangleType m_Type;
    double m_Base;
    int m_iBase;
    int m_iHeight;
public:
    CTriangle(TriangleType _Type,
              UINT _iPixelsPerTriangle);
    ~CTriangle();

    int Base();
    int Height();
    int Pixels();
};
               

// If we want YELLOW triangles, we'll define the following
//#define WANT_YELLOW_TRIANGLES

#ifdef WANT_YELLOW_TRIANGLES
struct SimpleVertex
{
    D3DXVECTOR3 Pos;
};
#else
struct Vertex
{
    D3DXVECTOR4 Pos;//x y z rhw
};
#endif

class CWorkLoad
{
private:

    // we do not free these on exit
    ID3D11Device* m_pDevice; 
    ID3D11Buffer* m_pVB;
    ID3D11InputLayout* m_pInputLayout;
    ID3D11VertexShader* m_pVertexShader;

#ifdef WANT_YELLOW_TRIANGLES
    SimpleVertex* m_pVertices;
#else
    Vertex* m_pVertices;
#endif
    UINT m_nVertices;
    UINT m_uiVertexSize;

    // client area pixels
    UINT m_Width;
    UINT m_Height;

public:
    CWorkLoad(ID3D11Device* _pDevice,UINT _Width, UINT _Height);
    ~CWorkLoad();
    void Initialize(CTriangle* _pTriangle,
                    int _iTriangles,
                    bool _bcase);
    ID3D11Buffer* GetVertexBuffer();
    UINT GetNumberOfVertices();
    UINT GetVertexSize();
    ID3D11InputLayout* GetInputLayout();
    ID3D11VertexShader* GetVertexShader();
};

//------------------------------------------------------------------------------
// Forward declarations 
//------------------------------------------------------------------------------
void RunGraphicsWork( ID3D11Device* pd3dDevice,
                      ID3D11DeviceContext* pd3dImmediateContext,
                      ID3D11ComputeShader* pComputeShader,
                      UINT nNumViews, ID3D11ShaderResourceView** pShaderResourceViews, 
                      ID3D11Buffer* pCBCS, void* pCSData, DWORD dwNumDataBytes,
                      ID3D11UnorderedAccessView* pUnorderedAccessView,
                      UINT X, UINT Y, UINT Z );
DWORD WINAPI GraphicThread( LPVOID lpParam );

class Graphics_TDR : public Base_TDR
{
public:
    Graphics_TDR();
    ~Graphics_TDR();

    // Setup options
    void SetTDRShaderOn();
    void SetTime(UINT _uiTime);
    HRESULT SetPixelsPerTriangle(UINT _PixelsPerTriangle);
    HRESULT SetTrianglesPerPrimitive(UINT _TrianglesPerPrimitive);
    HRESULT SetPrimitivesPerFrame(UINT _PrimitivesPerFrame);
    HRESULT SetLoopsInShader(UINT _LoopsInShader);
    HRESULT SetQuadrant(UINT _Quadrant);

    HRESULT SetNonInteractive();
    HRESULT SelectAdapter(bool _bNullAdapter);
    HRESULT SelectDeviceType(bool _bSoftwareDevice);
    HRESULT SelectBasicRender(bool _bBasicRender);
    HRESULT SetFeatureLevel(D3D_FEATURE_LEVEL _fl);

    HRESULT Initialize();
    HRESULT Start(){return S_OK;};
    HRESULT Work();
    HRESULT Cleanup();
    HRESULT WaitForDMABufferCompletion();

private:

    // For the client window, we default to 640x480 but the SetPixelsPerTriangle()
    // method may override these values making the window larger so that we don't 
    // clip the corners of the triangles.
    UINT m_uiWidth;
    UINT m_uiHeight;

    HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );

    HRESULT EnumerateAdapters(bool _bWantBasicRender);
    HRESULT InitDeviceAndSwapChain();
    HRESULT InitDeviceObjects();
    void InitDeviceContext();

    HRESULT Render();
    void CleanupDevice();

    // Initialization stuff
    bool m_bNullAdapter; // Command Line argument
    bool m_bSoftwareDevice; // Command Line argument
    bool m_bTDRShader; // Command Line argument
    UINT m_uiTime;     // Command Line Argument

    UINT m_uiPixelsPerTriangle;     // Command Line Argument
    UINT m_uiTrianglesPerPrimitive; // Command Line Argument
    UINT m_uiPrimitivesPerFrame;    // Command Line Argument
    UINT m_uiLoopsInShader;         // Command Line Argument
    UINT m_uiCreateDeviceFlags;     // Command Line Argument
    UINT m_Quadrant;   // Command Line Argument

    CQuadrant* m_pWindowLoc;

    D3D_FEATURE_LEVEL m_fl;

    //--------------------------------------------------------------------------
    // Main Variables
    //--------------------------------------------------------------------------
    HWND g_hWnd;
    D3D_DRIVER_TYPE         g_driverType;

    ID3D11Device*           g_pd3dDevice;
    IDXGISwapChain*         g_pSwapChain;
    ID3D11RenderTargetView* g_pRenderTargetView;
    ID3D11DeviceContext*    m_pImmediateContext;

    ID3D11InputLayout*      g_pInputLayout;
    ID3D11Buffer*           g_pVertexBuffer;
    ID3D11VertexShader*     g_pVertexShader;
    ID3D11PixelShader*      g_pPixelShader;

    IDXGIDevice2*           m_pDxgiDevice2;
    HANDLE                  m_hEvent;

    CTriangle* m_pTriangle;
    CWorkLoad* m_pWorkLoad;

    IDXGIFactory1* m_DxgiFactory;
    IDXGIAdapter1* m_DxgiAdapter;
};

