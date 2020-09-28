//-----------------------------------------------------------------------------
// File: CReschk_VB.cpp
//
// Desc: VB Test Functions
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "reschk.h"

CReschk_VB::CReschk_VB()
{
    // Test name
    m_szTestName = _T("CheckResource - VB");
    m_szCommandKey = _T("VB");

    pVBTest = NULL;
    pVBCopy = NULL;
}

CReschk_VB::~CReschk_VB()
{
}

//----------------------------------------------------------------------------
// Name: Setup()
// Desc: Setup device specific settings, variables, ect.
// Called when devices are restored(group init, device lost, etc)
//----------------------------------------------------------------------------
bool CReschk_VB::SetupResource()
{
    // Create the vertex buffers
    if( FAILED(CreateVB(&pVBTest)))
    {
        WriteToLog(_T("CreateVB() Failed on test resource.\n"));
        return false;
    }
    if( FAILED(CreateVB(&pVBCopy)))
    {
        WriteToLog(_T("CreateVB() Failed on copy resource\n"));
        return false;
    }

    // Create the resource
    if( FAILED( TestResource.Initialize(pVBTest, RTYPE_VERTEXBUFFER, m_pSrcDevice) ) ) 
    {
        WriteToLog(_T("Initialize of VB resource Failed.\n"));
        return false;
    }
    if( FAILED( CopyResource.Initialize(pVBCopy, RTYPE_VERTEXBUFFER, m_pSrcDevice) ) )
    {
        WriteToLog(_T("Initialize of VB resource Failed.\n"));
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
// Name: CreateVB()
// Desc:
//----------------------------------------------------------------------------
HRESULT CReschk_VB::CreateVB(CVertexBuffer** ppVertexBuffer)
{
    HRESULT hResult = E_FAIL;
    DIFUSEVERTEX* pVertices = GetVertices();

    if(FAILED(hResult = m_pSrcDevice->CreateVertexBuffer(sizeof(DIFUSEVERTEX)*4, USAGE_WRITEONLY, 
                        D3DFVF_DIFUSEVERTEX, POOL_DEFAULT, ppVertexBuffer)))
    {
        WriteToLog(_T("CreateVertexBuffer() failed with HResult = %s.\n"),m_pD3D->HResultToString(hResult));
        return hResult;
    }

    void *pVoid;
    CVertexBuffer* pVB = *ppVertexBuffer;
    if( FAILED(pVB->Lock(0, 0, &pVoid, 0))) 
    {
        RELEASE(pVB);
        return hResult;
    }
    memcpy( pVoid, pVertices, sizeof(DIFUSEVERTEX)*4 );
    pVB->Unlock();

    return hResult;
}

//----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Cleanup device specific settings, variables, ect. 
// Tied with Setup()
//----------------------------------------------------------------------------
void CReschk_VB::Cleanup()
{
    RELEASE(pVBTest);
    RELEASE(pVBCopy);

    CReschk::Cleanup();
}

//----------------------------------------------------------------------------
// Name: LoadResource()
// Desc: 
//----------------------------------------------------------------------------
bool CReschk_VB::LoadResource(LPRESOURCE pResource)
{
    CVertexBuffer* pVB;
    if( pResource == &TestResource )
        pVB = pVBTest;
    else if( pResource == &CopyResource )
        pVB = pVBCopy;
    else
    {
        WriteToLog(_T("RenderWithResource() - Bad input.\n"));
        return false;
    }

    if( FAILED( m_pSrcDevice->SetStreamSource( 0, pVB, sizeof( DIFUSEVERTEX ), 0 ) ) ) return false;
    if( FAILED( m_pSrcDevice->SetFVF( D3DFVF_DIFUSEVERTEX ) ) )                        return false;
    if( FAILED( m_pSrcDevice->BeginScene() ) )                                         return false;
    StartTime();
        m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    EndTime();
    if( FAILED( m_pSrcDevice->EndScene() ) )                                           return false;
    m_pSrcDevice->Present(NULL, NULL, NULL, NULL);
    GetLastError();

    return true;
}