//-----------------------------------------------------------------------------
// File: CReschk_Texture.cpp
//
// Desc: Texture Test Functions
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "reschk.h"

CReschk_Texture::CReschk_Texture()
{
    m_szTestName = _T("CheckResource - Texture");
    m_szCommandKey = _T("Texture");

    pVB          = NULL;
    pTestTexture = NULL;
    pCopyTexture = NULL;
}

CReschk_Texture::~CReschk_Texture()
{
}

//----------------------------------------------------------------------------
// Name: Setup()
// Desc: Setup device specific settings, variables, ect.
// Called when devices are restored(group init, device lost, etc)
//----------------------------------------------------------------------------
bool CReschk_Texture::SetupResource()
{
    // Create the vertex buffers
    if( FAILED(CreateVBTEX1(&pVB)) )
    {
        WriteToLog(_T("CreateVB() Failed.\n"));
        return false;
    }
    
    // Create the textures
    if( FAILED(CreateTexture(&pTestTexture)) )
    {
        WriteToLog(_T("CreateTexture() Failed.\n"));
        return false;
    }
    if( FAILED(CreateTexture(&pCopyTexture)) )
    {
        WriteToLog(_T("CreateTexture() Failed.\n"));
        return false;
    }

    // Create the resource
    if( FAILED( TestResource.Initialize(pTestTexture, RTYPE_TEXTURE, m_pSrcDevice) ) ) 
    {
        WriteToLog(_T("Initialize of Texture test resource Failed.\n"));
        return false;
    }
    if( FAILED( CopyResource.Initialize(pCopyTexture, RTYPE_TEXTURE, m_pSrcDevice) ) )
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
void CReschk_Texture::Cleanup()
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
HRESULT CReschk_Texture::CreateTexture(CTexture** ppTexture)
{
    HRESULT hResult = E_FAIL;
    DWORD dwTextureWidth  = 800;
	DWORD dwTextureHeight = 600;
    if(FAILED(hResult = m_pSrcDevice->CreateTexture(dwTextureWidth, dwTextureHeight, 1, 0, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], POOL_DEFAULT , ppTexture)))
    {
        WriteToLog(_T("CreateTexture() failed with HResult = %s.\n"),m_pD3D->HResultToString(hResult));
        return hResult;
    }

    D3DCOLOR d3dColorsStripe[] = {
        RGBA_MAKE(255,255,255,255), RGBA_MAKE(255,0,0,255), // White, Red
        RGBA_MAKE(0,255,0,255), RGBA_MAKE(0,0,255,255)};    // Green, Blue
    CTexture* pTexture = *ppTexture;
    if( FAILED(hResult = pTexture->LoadStripes(1, dwTextureWidth, dwTextureHeight, 
        4, (DWORD*)d3dColorsStripe, false, false, TEXLOAD_UPDATETEXTURE)))
    {
        WriteToLog(_T("LoadStripes() failed with HResult = %s.\n"),m_pD3D->HResultToString(hResult));
        return hResult;
    }
    
    if(pTexture->IsPalettized())
		pTexture->SetPalette(NULL);
   
    return S_OK;
}

//----------------------------------------------------------------------------
// Name: LoadResource()
// Desc: 
//----------------------------------------------------------------------------
bool CReschk_Texture::LoadResource(LPRESOURCE pResource)
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
    
    if( FAILED( m_pSrcDevice->SetStreamSource( 0, pVB, sizeof( TEX1VERTEX ), 0 ) ) )  return false;
    if( FAILED( m_pSrcDevice->SetFVF( D3DFVF_TEX1VERTEX ) ) )                         return false;
    if( FAILED( m_pSrcDevice->BeginScene() ) )                                        return false;
    StartTime();
        m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    EndTime();
    if( FAILED( m_pSrcDevice->EndScene() ) )                                          return false;
    m_pSrcDevice->Present(NULL, NULL, NULL, NULL);
    GetLastError();

    return true;
}
