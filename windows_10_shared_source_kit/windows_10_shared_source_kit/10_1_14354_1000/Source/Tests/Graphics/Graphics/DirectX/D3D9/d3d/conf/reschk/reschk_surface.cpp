//-----------------------------------------------------------------------------
// File: CReschk_Surface.cpp
//
// Desc: Surface Test Functions
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "reschk.h"

CReschk_Surface::CReschk_Surface()
{
    m_szTestName = _T("CheckResource - Surface");
    m_szCommandKey = _T("Surface");

    pTestSurface = NULL;
    pCopySurface = NULL;

    ReadInteger(_T("MaxRender"), 50, (int*)&m_nMaxPageOutRender, false);
}

CReschk_Surface::~CReschk_Surface()
{
}

//----------------------------------------------------------------------------
// Name: Setup()
// Desc: Setup device specific settings, variables, ect.
// Called when devices are restored(group init, device lost, etc)
//----------------------------------------------------------------------------
bool CReschk_Surface::SetupResource()
{    
    // Create the textures
    if( FAILED(CreateSurface(&pTestSurface)) )
    {
        WriteToLog(_T("CreateSurface() Failed.\n"));
        return false;
    }
    if( FAILED(CreateSurface(&pCopySurface)) )
    {
        WriteToLog(_T("CreateSurface() Failed.\n"));
        return false;
    }

    // Create the resource
    if( FAILED( TestResource.Initialize(pTestSurface, RTYPE_SURFACE, m_pSrcDevice) ) ) 
    {
        WriteToLog(_T("Initialize of Surface test resource Failed.\n"));
        return false;
    }
    if( FAILED( CopyResource.Initialize(pCopySurface, RTYPE_SURFACE, m_pSrcDevice) ) )
    {
        WriteToLog(_T("Initialize of Surface copy resource Failed.\n"));
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
void CReschk_Surface::Cleanup()
{
    RELEASE(pTestSurface);
    RELEASE(pCopySurface);

    CReschk::Cleanup();
}

//----------------------------------------------------------------------------
// Name: CreateTexture()
// Desc: 
//----------------------------------------------------------------------------
HRESULT CReschk_Surface::CreateSurface(CSurface** ppSurface)
{
    HRESULT hResult = E_FAIL;
    if(FAILED(hResult = m_pSrcDevice->CreateOffscreenPlainSurface(800,600,
                        m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat], 
                        POOL_DEFAULT, ppSurface)))
    {
        WriteToLog(_T("CreateOffscreenPlainSurface() failed with HResult = %s.\n"),m_pD3D->HResultToString(hResult));
        return hResult;
    }
 
    return S_OK;
}

//----------------------------------------------------------------------------
// Name: LoadResource()
// Desc: 
//----------------------------------------------------------------------------
bool CReschk_Surface::LoadResource(LPRESOURCE pResource)
{
    CSurface* pSurface = NULL;
    if( pResource == &TestResource )
        pSurface = pTestSurface;
    else if( pResource == &CopyResource )
        pSurface = pCopySurface;
        
    else
    {
        WriteToLog(_T("RenderWithResource() - Bad input.\n"));
        return false;
    }
       
    StartTime();
        pSurface->ColorFill(0xFF00FF);
    EndTime();

    return true;
}
