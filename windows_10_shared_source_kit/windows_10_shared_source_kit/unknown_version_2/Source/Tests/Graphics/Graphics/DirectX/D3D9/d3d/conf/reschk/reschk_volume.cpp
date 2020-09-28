//-----------------------------------------------------------------------------
// File: CReschk_volumetxture.cpp
//
// Desc: Volume Texture Test Functions
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "reschk.h"

CReschk_VolumeTexture::CReschk_VolumeTexture()
{
    m_szTestName = _T("CheckResource - Volume Texture");
    m_szCommandKey = _T("Volume");

    pVB          = NULL;

    pTestTexture = NULL;
    pCopyTexture = NULL;
}

CReschk_VolumeTexture::~CReschk_VolumeTexture()
{
}

//----------------------------------------------------------------------------
// Name: CapsCheck()
// Desc: Check caps requred by test. 
//----------------------------------------------------------------------------
bool CReschk_VolumeTexture::CapsCheck()
{
    if( !(m_pSrcDevice->GetCaps()->dwTextureCaps & 0x00002000) )
    {
        WriteToLog(_T("TextureCaps D3DPTEXTURECAPS_VOLUMEMAP needed.\n"));
        return false;
    }

    if( 0 == m_pSrcDevice->GetCaps()->dwMaxVolumeExtent )
    {
        WriteToLog(_T("Caps MaxVolumeExtent needed.\n")); 
        return false;
    }

    return CReschk::CapsCheck();;
}

//----------------------------------------------------------------------------
// Name: Setup()
// Desc: Setup device specific settings, variables, ect.
// Called when devices are restored(group init, device lost, etc)
//----------------------------------------------------------------------------
bool CReschk_VolumeTexture::SetupResource()
{
    
    // Create the vertex buffers
    if( FAILED(CreateVolVB(&pVB)) )
    {
        WriteToLog(_T("CreateVolVB() Failed.\n"));
        return false;
    }
    
    // Create the textures
    if( FAILED(CreateVolTexture(&pTestTexture)) )
    {
        WriteToLog(_T("CreateVolTexture() Failed. For test resource\n"));
        return false;
    }
    if( FAILED(CreateVolTexture(&pCopyTexture)) )
    {
        WriteToLog(_T("CreateVolTexture() Failed. For copy resource\n"));
        return false;
    }

    // Create the resource
    if( FAILED( TestResource.Initialize(pTestTexture, RTYPE_VOLUMETEXTURE, m_pSrcDevice) ) ) 
    {
        WriteToLog(_T("Initialize of Texture test resource Failed.\n"));
        return false;
    }
    if( FAILED( CopyResource.Initialize(pCopyTexture, RTYPE_VOLUMETEXTURE, m_pSrcDevice) ) )
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
void CReschk_VolumeTexture::Cleanup()
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
HRESULT CReschk_VolumeTexture::CreateVolTexture(CVolTexture** ppTexture)
{
    HRESULT hResult = E_FAIL;
    DWORD dwVolumeExtent  = min(512, m_pSrcDevice->GetCaps()->dwMaxVolumeExtent);
    
    if(FAILED(hResult = m_pSrcDevice->CreateVolumeTexture(dwVolumeExtent, dwVolumeExtent, 2, 1,
                        0, m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat], 
                        POOL_DEFAULT, ppTexture)))
    {
        WriteToLog(_T("CreateVolumeTexture() failed with HResult = %s.\n"),m_pD3D->HResultToString(hResult));
        return hResult;
    }

    D3DCOLOR d3dGradeinet[] = { RGBA_MAKE(255,255,255,255), RGBA_MAKE(255,255,255,255), RGBA_MAKE(255,255,255,255), RGBA_MAKE(255,255,255,255)};
    CVolTexture* pTexture = *ppTexture;
    if( FAILED(hResult = pTexture->LoadGradient(1, 0, dwVolumeExtent, dwVolumeExtent, 
        d3dGradeinet, TEXLOAD_DEFAULT)))
    {
        WriteToLog(_T("LoadGradient() failed with HResult = %s.\n"),m_pD3D->HResultToString(hResult));
        return hResult;
    }
    d3dGradeinet[0] = RGBA_MAKE(0,255,0,255); d3dGradeinet[1] = RGBA_MAKE(0,255,0,255); d3dGradeinet[2] = RGBA_MAKE(0,255,0,255); 
    d3dGradeinet[3] = RGBA_MAKE(0,255,0,255);
    if( FAILED(hResult = pTexture->LoadGradient(1, 1, dwVolumeExtent, dwVolumeExtent, 
        d3dGradeinet, TEXLOAD_UPDATETEXTURE)))
    {
        WriteToLog(_T("LoadGradient() failed with HResult = %s.\n"),m_pD3D->HResultToString(hResult));
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
HRESULT CReschk_VolumeTexture::CreateVolVB(CVertexBuffer** ppVertexBuffer)
{
    HRESULT hResult = E_FAIL;
	DIFUSEVERTEX* pVertices = GetVertices();
    VOLVERTEX d3dvolVertices[] = 
    {
        VOLVERTEX( pVertices[0], 0.0f,  0.0f, 0.0f),
        VOLVERTEX( pVertices[1], 1.0f,  0.0f, 0.0f),
        VOLVERTEX( pVertices[2], 0.0f,  1.0f, 1.0f),
        VOLVERTEX( pVertices[3], 1.0f,  1.0f, 1.0f)
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
bool CReschk_VolumeTexture::LoadResource(LPRESOURCE pResource)
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
    GetLastError();

    return true;
}
