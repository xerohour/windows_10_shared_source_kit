//
//  VBSwap.cpp
//
//  Vertex buffer renaming tests.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2000 Microsoft Corporation.
//
//

//  The execute/draw codepath has turned into a mess, but it shouldn't be
//  changed out from under the dct test kit unless necessary.

#include <d3d8.h>
#include <d3dx8.h>
#include <CD3DTest.h>
#include <CBuffers.h>
#include <CTextures.h>
#include "VBSwap.h"

//
// Declare the framework
//

CD3DWindowFramework	App;

//
// Define the groups here
//

CVBSwapTest tstCVBSwapTest;

//-----------------------------------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------------------------------

CVBSwapTest::CVBSwapTest()
{
	SetTestType(TESTTYPE_CONF);
    m_szTestName = _T("CVBSwapTest");
    m_szCommandKey = _T("CVBSwapTest");
	m_Options.D3DOptions.fMinDXVersion		= 8.f;
    //m_Options.D3DOptions.fMaxDXVersion      = 8.1f;
	m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
    m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD|DEVICECREATE_HWVP;
	m_Options.D3DOptions.bReference = false;
    m_Options.D3DOptions.bEnumReference = false;
	m_Options.D3DOptions.bZBuffer = true;
	
	m_bDynamicTextures = false;
	memset(&m_SrcContext, 0, sizeof(DEV_CONTEXT));
	m_bHandleExceptions = false;
    memset(m_pfvfTriSorted, 0, sizeof(PVOID)*10);
}

//-----------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SortVertexBuffer
//-----------------------------------------------------------------------------
bool CVBSwapTest::SortVertexBuffer(PVOID pVBDataDest,  PVOID pVBDataSrc, WORD *pwIBDataSrc, UINT uIndexCount, UINT uVertexSize)
{
    UINT i;
    PBYTE pSrc = (PBYTE)pVBDataSrc, pDest = (PBYTE)pVBDataDest;

    for (i=0; i<uIndexCount; i++)            
        memcpy(&pDest[i*uVertexSize], &pSrc[pwIBDataSrc[i]*uVertexSize], uVertexSize);

    return true;
}

//-----------------------------------------------------------------------------
bool CVBSwapTest::SetDefaultRenderStates()
{
 	bool bResult = true;

	bResult &= SUCCEEDED(m_pSrcDevice->SetRenderState( D3DRS_ZENABLE, (DWORD) TRUE ));
    bResult &= SUCCEEDED(m_pSrcDevice->SetRenderState( D3DRS_DITHERENABLE, (DWORD) TRUE ));
    bResult &= SUCCEEDED(m_pSrcDevice->SetRenderState( D3DRS_SPECULARENABLE, (DWORD) FALSE ));
    bResult &= SUCCEEDED(m_pSrcDevice->SetRenderState( D3DRS_AMBIENT, (DWORD) 0x00000000 ));
    bResult &= SUCCEEDED(m_pSrcDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD) FALSE ));
    bResult &= SUCCEEDED(m_pSrcDevice->SetRenderState( D3DRS_CLIPPING, (DWORD)FALSE));
	bResult &= SUCCEEDED(m_pSrcDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ));
	return bResult;
}

//-----------------------------------------------------------------------------

bool CVBSwapTest::SetDefaultLights()
{
 	bool bResult = true;

	static LIGHT liDefaultLight = 
	{		
		D3DLIGHT_DIRECTIONAL,	// Type
		.65f, .65f, .65f, .65f,	// Diffuse
		 0.f,  0.f,  0.f,  0.f,	// Specular
		.35f, .35f, .35f, .35f,	// Ambient
		 5.f, 20.f, -5.f,		// Position
		+1.f,-0.8f, +2.5f,		// Direction
		100.f,					// Range
		1.f,					// Falloff
		1.f,					// Attenuation0
		1.f,					// Attenuation1
		1.f,					// Attenuation2
		1.f,					// Theta
		1.f						// Phi
	};
	bResult &= SUCCEEDED(m_pSrcDevice->SetLight( 0, &liDefaultLight ));
	bResult &= SUCCEEDED(m_pSrcDevice->LightEnable( 0, TRUE ));
	return bResult;
}

//-----------------------------------------------------------------------------

bool CVBSwapTest::SetDefaultMatrices()
{
	bool bResult = true;

    // Set the transform matrices
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( -8.f, 1.8f, 0.16f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( -4.f, 0.0f, 0.16f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMATRIX  matWorld, matView, matProj;

    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    float fAspect = (float)GetCurrentMode()->pDisplay->dwWidth / (float)GetCurrentMode()->pDisplay->dwHeight;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 180.0f );

    bResult &= SetTransform( D3DTS_WORLD,      &matWorld );
    bResult &= SetTransform( D3DTS_VIEW,       &matView );
    bResult &= SetTransform( D3DTS_PROJECTION, &matProj );
	return bResult;
}


//-----------------------------------------------------------------------------

bool CVBSwapTest::SetDefaultMaterials()
{
	bool bResult = true;

	MATERIAL mtrl =
	{
		{ 1.0f, 1.0f, 1.0f, 0.0f },  // diffuse
		{ 0.8f, 0.8f, 0.8f, 0.8f },  // ambient
		{ 0.0f, 0.0f, 0.0f, 0.0f },	 // specular
		{ 0.0f, 0.0f, 0.0f, 0.0f },	 // emissive
		0.0f  // sharpness (for specular)
	};

	bResult &=  SUCCEEDED(m_pSrcDevice->SetMaterial(&mtrl));

	return bResult;
}

//-----------------------------------------------------------------------------

UINT CVBSwapTest::TestInitialize()
{
	m_bHandleExceptions = KeySet(_T("TRAP")) ? false : true;
	
	if( KeySet(_T("TEXDYNAMIC") ) )
		m_bDynamicTextures = true;
	else if( KeySet(_T("TEXSTATIC") ) )
		m_bDynamicTextures = false;
	else 
		//m_bDynamicTextures = ((m_pSrcDevice->GetCaps()->dwCaps2) & D3DCAPS2_DYNAMICTEXTURES) ? true : false;
        m_bDynamicTextures = false;  // Using dynamic textures seems to defeat the benefits of dynamic VBs (at least on ATI R300)

	// set the test range
    SetTestRange(1, 1000);  //200 frames each of DrawPrim, DrawIndexedPrim, and mixed.
 
    // Create a series of meshes that we can cycle between.
    for(int i=0; i< 8; i++)
    {
        m_Shape[i].NewShape(CS_SPHERE, 10, 10);
	    m_Shape[i].Scale(3,3,3);
        // Rotate each of them so they at least look a little different.
        // Ideally we'd use entirely different meshes, but that's more work.        
            m_Shape[i].Rotate(0.0f, i*45.0f, 0.0f);

        // Create versions that are sorted into Triangle Lists for non index buffer usage.
        m_pfvfTriSorted[i] = new FVFVERTEX[m_Shape[i].m_nIndices];
        if (!m_pfvfTriSorted[i])
        {
	    	WriteToLog(_T("GlobalAlloc failed.  Insufficient system memory!\n"));
    		return D3DTESTINIT_SKIPALL;
        }
        SortVertexBuffer(m_pfvfTriSorted[i], m_Shape[i].m_pVertices, m_Shape[i].m_pIndices, m_Shape[i].m_nIndices, sizeof(FVFVERTEX) );
    }

	m_SrcContext.pDevice = m_pSrcDevice;
    return InitResources( &m_SrcContext );
}

//-----------------------------------------------------------------------------

bool CVBSwapTest::ExecuteTest(UINT nTest)
{
	bool bResult = true;
	TCHAR szTestCase[256];
	UINT n = nTest-1;

    if (nTest<600)
   	    _stprintf( szTestCase, _T("DrawPrim - Test Case #%d"), nTest );
    else if (nTest<800)
   	    _stprintf( szTestCase, _T("DrawIndexedPrim - Test Case #%d"), nTest );
    else
   	    _stprintf( szTestCase, _T("Mixed - Test Case #%d"), nTest );

	BeginTestCase( szTestCase, nTest );

	bResult &= ClearFrame();
        bResult &= Clear(0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00404040, 1.0f, 0 );
	bResult &= BeginScene();
	
	bResult &= Draw(nTest-1, &m_SrcContext);
	
	bResult &= EndScene();

	if( false == bResult)
		Fail();

	return true;
}

//-----------------------------------------------------------------------------

UINT CVBSwapTest::InitResources(DEV_CONTEXT* pContext)
{
	CDevice* pDevice = pContext->pDevice;
	CTexture** ppTexture = &(pContext->pTexture);
	CTexture** ppDynamicTexture = &(pContext->pDynamicTexture);
	CVertexBuffer** ppVb = &(pContext->pVB);
	CIndexBuffer** ppIb = &(pContext->pIB);
	if( FAILED( pDevice->D3DXCreateTextureFromResourceEx( 0L, 
														  _T("TEXTURE_BMP"),
														  D3DX_DEFAULT,
														  D3DX_DEFAULT,
														  1,
														  0,
														  D3DFMT_UNKNOWN,
														  false == m_bDynamicTextures ? D3DPOOL_MANAGED : D3DPOOL_SYSTEMMEM,
														  D3DX_DEFAULT,
														  D3DX_DEFAULT,
														  0,
														  0L,
														  0L,
														  ppTexture
														  ) ) )
	{
		WriteToLog(_T("Failed to load required texture resource.\n"));
		return D3DTESTINIT_SKIPALL;
	}

	if( FAILED( pDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP ) ) )
		return D3DTESTINIT_SKIPALL;

	if( FAILED( pDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP ) ) )
		return D3DTESTINIT_SKIPALL;


    // Create a vertex buffer twice as large as needed to store the indexed and non-indexed
    // versions of our mesh.   We'll use the extra space to test sub-buffer locking and still
    // be able to write our full mesh.
    //
	if( FAILED( m_pSrcDevice->CreateVertexBuffer( m_Shape[0].m_nIndices * sizeof(FVFVERTEX) * 2,
														USAGE_DYNAMIC | USAGE_WRITEONLY,
														FVFVERTEXHANDLE,
														POOL_DEFAULT,
														ppVb )))
    {
        WriteToLog("Unexpected CreateVertexBuffer failure!");
		return D3DTESTINIT_SKIPALL;
    }

    // Create an index buffer for the indexed version of the mesh.
    //
	if( FAILED( m_pSrcDevice->CreateIndexBuffer(	m_Shape[0].m_nIndices * sizeof(WORD),
														USAGE_DYNAMIC | USAGE_WRITEONLY, 
                                                        FMT_INDEX16, POOL_DEFAULT, ppIb)))
    {
        WriteToLog("Unexpected CreateIndexBuffer failure!");
		return D3DTESTINIT_SKIPALL;
    }
           
	WORD* pIndices = 0L;
	if( FAILED((*ppIb)->Lock( 0, 0, (BYTE**)&pIndices, LOCK_DISCARD | LOCK_NOSYSLOCK )) )
		return D3DTESTINIT_SKIPALL;
    memcpy( pIndices, m_Shape[0].m_pIndices, m_Shape[0].m_nIndices*sizeof(WORD) );
    (*ppIb)->Unlock( );

	if( false == m_bDynamicTextures )
	    return D3DTESTINIT_RUN;

	SURFACEDESC sd;
	memset( &sd, 0, sizeof(sd) );
	if( FAILED( (*ppTexture)->GetLevelDesc( 0, &sd ) ) )
		return D3DTESTINIT_SKIPALL;

	if( FAILED( pDevice->CreateTexture( sd.dwWidth,
										sd.dwHeight,
										(*ppTexture)->GetLevelCount(),
										D3DUSAGE_DYNAMIC /*| D3DUSAGE_WRITEONLY implied */,
										sd.Format,
										D3DPOOL_DEFAULT,
										ppDynamicTexture
										) ) )
		return D3DTESTINIT_SKIPALL;

    return D3DTESTINIT_RUN;
}

//-----------------------------------------------------------------------------

bool CVBSwapTest::Draw(UINT n, DEV_CONTEXT* pContext)
{
	bool bResult = true;
	static int key=0;
	CDevice* pDevice = pContext->pDevice;
	CTexture* pTexture = pContext->pTexture;
	CTexture* pDynamicTexture = pContext->pDynamicTexture;
	CVertexBuffer* pVB = pContext->pVB;
	CIndexBuffer* pIB  = pContext->pIB;
    LONGLONG liStart, liEnd, liLongestTime = 0;
    UINT uLongest=99999;
    static UINT uLongestLastFrame=99999;
    BYTE *pVBDataPtr[64];
    UINT uReused=0;
    bool fUseDrawPrim = 0;  // Choose DrawPrim vs. DrawIndexedPrim
    UINT uLockOffset;
    HRESULT hr;

	// draw back to front to keep the hardware busy.
	D3DXVECTOR3 vLeft  = D3DXVECTOR3( 4.0f, -1.0f, 0.0f );
//	D3DXVECTOR3 vRight = D3DXVECTOR3( 4.3f, -2.0f, -2.0f );


	D3DMATRIX matIn;
	D3DXMATRIX matOut, matInt, matTemp, matRot, matTrans;

	bResult &= SUCCEEDED( pDevice->SetFVF( FVFVERTEXHANDLE ) );
	bResult &= SUCCEEDED( pDevice->SetStreamSource( 0, pVB, sizeof(FVFVERTEX) ) );
	bResult &= SUCCEEDED( pDevice->SetIndices( pIB, 0 ) );
	bResult &= SUCCEEDED( pDevice->SetTexture( 0, false == m_bDynamicTextures ? pTexture : pDynamicTexture ) );

	bResult &= SUCCEEDED( pDevice->GetTransform( D3DTS_WORLD, &matIn ) );
	matInt = matIn;
	
	D3DXMatrixMultiply( &matOut, &matInt, D3DXMatrixScaling(&matTemp, .25f, .25f, .25f)); 
    D3DXMatrixMultiply( &matTrans, &matOut, D3DXMatrixTranslation( &matTemp, 0, -2, 0 ) ); 

	int iObj = 0;
	int iMaskPow = 0;
	bool bLockResult;

	for( int i=0; i<32; i++)
	{

        // Use DrawPrim for the first 200 test cases, and part of the time in
        // cases 401 and later.
		if( (n<600) || (n>800 &&(1<<iMaskPow) & n) )
		{
            fUseDrawPrim = true;
            uLockOffset = rand()*m_Shape[key].m_nIndices/RAND_MAX* sizeof(FVFVERTEX);
//            uLockOffset = 0;
        }
        else // Use DrawIndexedPrim
        {
            fUseDrawPrim = false;
            uLockOffset =0;
        }


		if( m_bDynamicTextures )
			if( false == CopyTexture( pContext->pDynamicTexture, pContext->pTexture ) )
			{
                WriteToLog(_T("Texture copy, object %d (zero-based) failed.\n"), iObj );
				bResult = false;
			}

		BYTE* pVertexData = 0L ; //, *pSourceVertexData;

        // Time how long it takes to lock the vertex buffer.
        //
        QueryPerformanceCounter((LARGE_INTEGER*)&liStart);

		bResult &= ( bLockResult = SUCCEEDED( pVB->Lock( uLockOffset, m_Shape[key].m_nIndices * sizeof(FVFVERTEX), &pVertexData, LOCK_DISCARD) ) );		
		if( false == bLockResult )
			continue;
        QueryPerformanceCounter((LARGE_INTEGER*)&liEnd);

        for(int p=0;p<i;p++)
            if (pVBDataPtr[p] == pVertexData)
            {
                uReused = i;
            }

        pVBDataPtr[i] = pVertexData;

        // Record the longest lock call.  This will most likely correspond to the vertex buffer lock that triggered a flush.
        // A flush is only likely to occur if we exceeded the number of VB swap buffers the driver has avialable.
        if ( (liEnd-liStart) > liLongestTime*15/10) 
        {
            liLongestTime = liEnd-liStart;
            uLongest = i;
        }

		if( fUseDrawPrim )
        {
            memcpy( pVertexData, 
                    (BYTE *)m_pfvfTriSorted[key], 
                    m_Shape[key].m_nIndices * sizeof(FVFVERTEX) );
        }
        else
        {
            memcpy( pVertexData, 
                    (BYTE *)m_Shape[key].m_pVertices, 
                    m_Shape[key].m_nVertices * sizeof(FVFVERTEX) );
        }


		bResult &= SUCCEEDED( pVB->Unlock() );

		matInt = matTrans;
    	D3DXMatrixMultiply( &matRot, &matInt, D3DXMatrixRotationX( &matTemp, i*D3DX_PI/8*n/1000 ) ); 
//        if (i == uLongestLastFrame)
//            D3DXMatrixMultiply( &matOut, &matRot, D3DXMatrixTranslation( &matTemp, vLeft.x-2.0, vLeft.y, vLeft.z ) ); 
//        else
            D3DXMatrixMultiply( &matOut, &matRot, D3DXMatrixTranslation( &matTemp, vLeft.x, vLeft.y, vLeft.z ) ); 


        //D3DXMatrixMultiply( &matOut, &matRot, D3DXMatrixTranslation( &matTemp, vLeft.x, 0.0, 0.0 ) ); 
		bResult &= SUCCEEDED( pDevice->SetTransform( D3DTS_WORLD, &matOut ) );

        // Draw the update mesh.
        //
		if( fUseDrawPrim )
		{			
            hr = pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, uLockOffset/sizeof(FVFVERTEX), m_Shape[key].m_nIndices/3);
            bResult &= SUCCEEDED(hr); 
		}
		else
		{
			hr = pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
											0, m_Shape[key].m_nVertices,
											0, m_Shape[key].m_nIndices/3
											);
            bResult &= SUCCEEDED(hr); 
        }
        
        if (FAILED(hr))
        {
            WriteToLog("DrawPrimitive failed!\n");
        }

        ++iMaskPow = iMaskPow > 7 ? 0 : iMaskPow;
		iObj++;
		vLeft.x -= .20f;

        if( ++key >= 8 )
			key = 0;
	}
    uLongestLastFrame = uLongest;

	bResult &= SUCCEEDED( pDevice->SetTransform( D3DTS_WORLD, &matIn ) );
	
	return bResult;
}

//-----------------------------------------------------------------------------

bool CVBSwapTest::CopyTexture( CTexture* pDst, CTexture* pSrc )
{
	bool bResult = true;
	DWORD dwLevels=1;
	if( dwLevels != pDst->GetLevelCount() || dwLevels != pSrc->GetLevelCount())
		return false;
	for( int i=0; i<(int)dwLevels; i++ )
	{
		SURFACEDESC sdSrc;
		LOCKED_RECT lrcSrc, lrcDst;
		PIXELFORMAT pf;
		pSrc->GetLevelDesc(i,&sdSrc);
		d3df2ddpf(sdSrc.Format.d3dfFormat, &pf);
		if( FAILED(pDst->LockRect(i,&lrcDst,0L,D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD) ) )
			return false;
		if( FAILED(pSrc->LockRect(i,&lrcSrc,0L,D3DLOCK_NOSYSLOCK) ) )
		{
			pDst->UnlockRect(i);
			return false;
		}
		for(int y=0; y<(int)sdSrc.dwHeight; y++)
		{
			if( false == m_bHandleExceptions )
			{
				
				memcpy(  (LPBYTE)lrcDst.pBits + y * lrcDst.iPitch,
						 (LPBYTE)lrcSrc.pBits + y * lrcSrc.iPitch,
						 (pf.dwRGBBitCount>>3) * sdSrc.dwWidth );
			}
			else
			{
				__try
				{
					BYTE* dst = (LPBYTE)lrcDst.pBits + y * lrcDst.iPitch;
					BYTE* src = (LPBYTE)lrcSrc.pBits + y * lrcSrc.iPitch;
					int n = (pf.dwRGBBitCount>>3) * sdSrc.dwWidth;
					memcpy( dst, src, n );
				}
				__except(1)
				{
					bResult &= false;
				}
			}
		}
		pDst->UnlockRect(i);
		pSrc->UnlockRect(i);
	}

	if( false == bResult )
		WriteToLog(_T("Exception writing to Texture.\n"));

	return bResult;
}

//-----------------------------------------------------------------------------

bool CVBSwapTest::TestTerminate()
{

	RELEASE( m_SrcContext.pVB );
	RELEASE( m_SrcContext.pIB );
	RELEASE( m_SrcContext.pTexture );
	RELEASE( m_SrcContext.pDynamicTexture );
    for(int i=0;i<8;i++)
    {
        if (m_pfvfTriSorted[i])
            delete m_pfvfTriSorted[i];
        m_pfvfTriSorted[i] = NULL;
    }
	return true;
}

//-----------------------------------------------------------------------------

