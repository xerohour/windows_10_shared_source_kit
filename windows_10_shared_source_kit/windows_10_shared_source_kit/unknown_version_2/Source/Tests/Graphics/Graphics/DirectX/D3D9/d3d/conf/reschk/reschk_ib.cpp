//-----------------------------------------------------------------------------
// File: CReschk_IB.cpp
//
// Desc: IB Test Functions
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "reschk.h"

CReschk_IB::CReschk_IB()
{
    // Test name
    m_szTestName = _T("CheckResource - IB");
    m_szCommandKey = _T("IB");

    pVB     = NULL;
    pIBTest = NULL;
    pIBCopy = NULL;
}

CReschk_IB::~CReschk_IB()
{
}

//----------------------------------------------------------------------------
// Name: Setup()
// Desc: Setup device specific settings, variables, ect.
// Called when devices are restored(group init, device lost, etc)
//----------------------------------------------------------------------------
bool CReschk_IB::SetupResource()
{
    // Create the vertex buffer
    if( FAILED(CreateVB(&pVB)))
    {
        WriteToLog(_T("CreateVB() Failed.\n"));
        return false;
    }

    // Create Indexs
    if( FAILED(CreateIB(&pIBTest)) )
    {
        WriteToLog(_T("CreateIB() Failed.\n"));
        return false;
    }
    if( FAILED(CreateIB(&pIBCopy)) )
    {
        WriteToLog(_T("CreateIB() Failed.\n"));
        return false;
    }

    // Create the resource
    if( FAILED( TestResource.Initialize(pIBTest, RTYPE_INDEXBUFFER, m_pSrcDevice) ) ) 
    {
        WriteToLog(_T("Initialize of IB resource Failed.\n"));
        return false;
    }
    if( FAILED( CopyResource.Initialize(pIBCopy, RTYPE_INDEXBUFFER, m_pSrcDevice) ) )
    {
        WriteToLog(_T("Initialize of IB resource Failed.\n"));
        return false;
    }

    // Set resources for testing
    if( FAILED( SetTestResources(&TestResource, &CopyResource) ) )
    {
        WriteToLog(_T("SetTestResources() Failed.\n"));
        return false;
    }

    return true;;
}

//----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Cleanup device specific settings, variables, ect. 
// Tied with Setup()
//----------------------------------------------------------------------------
void CReschk_IB::Cleanup()
{
    RELEASE(pVB);
    RELEASE(pIBTest);
    RELEASE(pIBCopy);

    CReschk::Cleanup();
}

//----------------------------------------------------------------------------
// Name: CreateIB()
// Desc: 
//----------------------------------------------------------------------------
HRESULT CReschk_IB::CreateIB(CIndexBuffer** ppIndexBuffer)
{
    HRESULT hResult = E_FAIL;
    WORD wIndexs[] = {0,1,2,3};
	hResult = m_pSrcDevice->CreateIndexBuffer( sizeof(wIndexs), 0, FMT_INDEX16, POOL_DEFAULT, ppIndexBuffer );
    if( SUCCEEDED(hResult) )
    {    
        void *pIndexs;
        ((CIndexBuffer*)*ppIndexBuffer)->Lock(0, 0, &pIndexs, 0);
        memcpy( pIndexs, wIndexs, sizeof(wIndexs) );
        ((CIndexBuffer*)*ppIndexBuffer)->Unlock();
    }

    return hResult;
}

//----------------------------------------------------------------------------
// Name: CreateVB()
// Desc:
//----------------------------------------------------------------------------
HRESULT CReschk_IB::CreateVB(CVertexBuffer** ppVertexBuffer)
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
// Name: LoadResource()
// Desc: 
//----------------------------------------------------------------------------
bool CReschk_IB::LoadResource(LPRESOURCE pResource)
{
    CIndexBuffer* pID;
    if( pResource == &TestResource )
        pID = pIBTest;
    else if( pResource == &CopyResource )
        pID = pIBTest;
    else
    {
        WriteToLog(_T("RenderWithResource() - Bad input.\n"));
        return false;
    }
    if( FAILED( m_pSrcDevice->SetIndices(pID, 0)) )                                    return false;
    if( FAILED( m_pSrcDevice->SetStreamSource( 0, pVB, sizeof( DIFUSEVERTEX ), 0 ) ) ) return false;
    if( FAILED( m_pSrcDevice->SetFVF( D3DFVF_DIFUSEVERTEX ) ) )                        return false;
    if( FAILED( m_pSrcDevice->BeginScene() ) )                                         return false;
    StartTime();
        m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 4, 0, 2, 0 );
    EndTime();
    if( FAILED( m_pSrcDevice->EndScene() ) )                                           return false;
    m_pSrcDevice->Present(NULL, NULL, NULL, NULL);
    GetLastError();

    return true;
}