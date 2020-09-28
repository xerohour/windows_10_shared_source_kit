//-----------------------------------------------------------------------------
// File: CReschk_cubetexture.cpp
//
// Desc: Cube Texture Test Functions
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "reschk.h"

CReschk_CubeTexture::CReschk_CubeTexture()
{
    m_szTestName = _T("CheckResource - Cube Texture");
    m_szCommandKey = _T("Cube");

    pVB          = NULL;

    pTestTexture = NULL;
    pCopyTexture = NULL;
}

CReschk_CubeTexture::~CReschk_CubeTexture()
{
}

//----------------------------------------------------------------------------
// Name: CapsCheck()
// Desc: Check caps requred by test. 
//----------------------------------------------------------------------------
bool CReschk_CubeTexture::CapsCheck()
{
    if( !(m_pSrcDevice->GetCaps()->dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP) )
    {
        WriteToLog(_T("TextureCaps D3DPTEXTURECAPS_CUBEMAP needed.\n"));
        return false;
    }

    return CReschk::CapsCheck();;
}

//----------------------------------------------------------------------------
// Name: Setup()
// Desc: Setup device specific settings, variables, ect.
// Called when devices are restored(group init, device lost, etc)
//----------------------------------------------------------------------------
bool CReschk_CubeTexture::SetupResource()
{
    // Create the vertex buffers
    if( FAILED(CreateCubeVB(&pVB)) )
    {
        WriteToLog(_T("CreateVB() Failed.\n"));
        return false;
    }
    
    // Create the textures
    if( FAILED(CreateCubeTexture(&pTestTexture)) )
    {
        WriteToLog(_T("CreateCubeTexture() Failed.\n"));
        return false;
    }
    if( FAILED(CreateCubeTexture(&pCopyTexture)) )
    {
        WriteToLog(_T("CreateCubeTexture() Failed.\n"));
        return false;
    }

    // Create the resource
    if( FAILED( TestResource.Initialize(pTestTexture, RTYPE_CUBETEXTURE, m_pSrcDevice) ) ) 
    {
        WriteToLog(_T("Initialize of Texture test resource Failed.\n"));
        return false;
    }
    if( FAILED( CopyResource.Initialize(pCopyTexture, RTYPE_CUBETEXTURE, m_pSrcDevice) ) )
    {
        WriteToLog(_T("Initialize of Texture copy resource Failed.\n"));
        return false;
    }

    // Set resources for testing
    if( FAILED( SetTestResources(&TestResource, &CopyResource) ) )
    {
        WriteToLog(_T("SetTestResources() Failed.\n"));
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Cleanup device specific settings, variables, ect. 
// Tied with Setup()
//----------------------------------------------------------------------------
void CReschk_CubeTexture::Cleanup()
{
    RELEASE(pVB);
    RELEASE(pTestTexture);
    RELEASE(pCopyTexture);

    CReschk::Cleanup();
}

//----------------------------------------------------------------------------
// Name: CreateTexture()
// Desc: 
//----------------------------------------------------------------------------
HRESULT CReschk_CubeTexture::CreateCubeTexture(CCubeTexture** ppTexture)
{
    HRESULT hResult = E_FAIL;
    if(FAILED(hResult = m_pSrcDevice->CreateCubeTexture(128, 1, 0,
                        m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat], 
                        POOL_DEFAULT, ppTexture)))
    {
        WriteToLog(_T("CreateCubeTexture() failed with HResult = %s.\n"),m_pD3D->HResultToString(hResult));
        return hResult;
    }

    D3DCOLOR d3dColorsStripe[] = { RGBA_MAKE(0,255,0,255), RGBA_MAKE(0,0,255,255)};
    CCubeTexture* pTexture = *ppTexture;
    if( FAILED(hResult = pTexture->LoadStripes(1, CUBEMAP_FACE_ALL, 800, 600, 
        2, (DWORD*)d3dColorsStripe, true, false, TEXLOAD_UPDATETEXTURE)))
    {
        WriteToLog(_T("LoadStripes() failed with HResult = %s.\n"),m_pD3D->HResultToString(hResult));
        return hResult;
    }
    
    if(pTexture->IsPalettized())
		pTexture->SetPalette(NULL);
   
    return S_OK;
}

//----------------------------------------------------------------------------
// Name: CreateVB()
// Desc: 
//----------------------------------------------------------------------------
HRESULT CReschk_CubeTexture::CreateCubeVB(CVertexBuffer** ppVertexBuffer)
{
    HRESULT hResult = E_FAIL;

	DIFUSEVERTEX* pVertices = GetVertices();
    VOLVERTEX d3dvolVertices[] = 
    {
        VOLVERTEX( pVertices[0], 0.0f,  0.0f, 0.0f),
        VOLVERTEX( pVertices[1], 1.0f,  0.0f, 0.0f),
        VOLVERTEX( pVertices[2], 0.0f,  1.0f, 0.0f),
        VOLVERTEX( pVertices[3], 1.0f,  1.0f, 0.0f)
    };
    
    hResult = m_pSrcDevice->CreateVertexBuffer( sizeof( d3dvolVertices ), USAGE_WRITEONLY, FVF_VOLVERTEX, POOL_DEFAULT, ppVertexBuffer );

    if( SUCCEEDED(hResult) )
    {    
        void *pVertices;
        ((CVertexBuffer*)*ppVertexBuffer)->Lock(0, 0, &pVertices, 0);
        memcpy( pVertices, d3dvolVertices, sizeof( d3dvolVertices ) );
        ((CVertexBuffer*)*ppVertexBuffer)->Unlock();
    }

    return hResult;
}

//----------------------------------------------------------------------------
// Name: LoadResource()
// Desc: 
//----------------------------------------------------------------------------
bool CReschk_CubeTexture::LoadResource(LPRESOURCE pResource)
{
    HRESULT hResult = E_FAIL;
    if( pResource == &TestResource )
        hResult = m_pSrcDevice->SetTexture(0, pTestTexture);
    else if( pResource == &CopyResource )
        hResult = m_pSrcDevice->SetTexture(0, pCopyTexture);
    else
    {
        WriteToLog(_T("RenderWithResource() - Bad input.\n"));
        return false;
    }

    if( FAILED(hResult) )
    {
        WriteToLog(_T("SetTexture() failed with HResult = %s.\n"),m_pD3D->HResultToString(hResult));
        return false;
    }
    
    if( FAILED( m_pSrcDevice->SetStreamSource( 0, pVB, sizeof( VOLVERTEX ), 0 ) ) )   return false;
    if( FAILED( m_pSrcDevice->SetFVF( FVF_VOLVERTEX ) ) )                             return false;
    if( FAILED( m_pSrcDevice->BeginScene() ) )                                        return false;
    StartTime();
        m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    EndTime();
    if( FAILED( m_pSrcDevice->EndScene() ) )                                          return false;
    m_pSrcDevice->Present(NULL, NULL, NULL, NULL);

    return true;
}