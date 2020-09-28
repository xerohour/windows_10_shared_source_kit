//
//  CLockDepth.cpp
//
//  D3DFMT_D16_LOCKABLE Tests.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2000 Microsoft Corporation.
//
//

#include "LockDepth.h"
#include "CBuffers.h"
#include "CSurface.h"
#include "CTextures.h"
#include <ddraw.h>
#include "PerlinNoise.h"

#if FAST_TEST
#define NUMBEROFTESTS	4
#else //FAST_TEST
#define NUMBEROFTESTS	48
#endif

struct TEX2DVERTEX {D3DXVECTOR4 p;D3DXVECTOR2 t;};
inline TEX2DVERTEX InitTex2DVertex( const D3DXVECTOR4& p, D3DXVECTOR2& t )
{
    TEX2DVERTEX v;   v.p = p;   v.t = t;
    return v;
}

	
#define D3DFVF_TEX2DVERTEX (D3DFVF_XYZRHW|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0))

#define MAX_VERTICES 32
#define PALETTE_IDENTIFIER 3

//
// Declare the framework
//

CD3DWindowFramework	App;

//
// Define the groups here
//

CLockDepth tstLockDepth;

//-----------------------------------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------------------------------
CLockDepth::CLockDepth()
{
	SetTestType(TESTTYPE_CONF);
    m_szTestName							= _T("LockDepth");
    m_szCommandKey							= _T("LockDepth");
	m_Options.D3DOptions.fMinDXVersion		= 8.f;
	m_Options.D3DOptions.dwDevTypes			= DEVICETYPE_HAL;
    m_Options.D3DOptions.dwBehaviors		= DEVICECREATE_STANDARD | DEVICECREATE_SWVP;
	m_Options.D3DOptions.bReference			= true;
    m_Options.D3DOptions.bEnumReference		= false;
	m_Options.D3DOptions.bZBuffer			= false;
    m_Options.D3DOptions.nBackBufferWidth = 256;
	m_Options.D3DOptions.nBackBufferHeight= 256;

	m_pVB									= 0L;
	m_bHadForceDriversOnKey					= false;
	m_bHadForceDriversOffKey				= false;
	m_dwOldForceDriversOnValue				= 0;
	m_dwOldForceDriversOffValue				= 0;
	m_pTextures								= 0L;

	int i;
	ReadInteger( _T("TOL"), 4, &i );
	m_wTolerance = (i>=0)? i & 0x7fff:0;
	
	ReadInteger( _T("LOGS"), 1, &i );
	m_uMaxFailsLog = (i>=0)? i & 0x7fff:0;
}

//-----------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------

// munge the registry before creating the device
bool CLockDepth::PreModeSetup(void)
{
	if( KeySet(_T("FORCE") ) )
	{
		if( !SetDriverFlags( KNOWN_D16_LOCKABLE, true ) )
			return false;
	}

	return CD3DTest::PreModeSetup();
}

//-----------------------------------------------------------------------------

bool CLockDepth::SetDefaultRenderStates()
{
	bool bResult = true;
    bResult &= SetRenderState( D3DRS_ZENABLE, (DWORD) TRUE );
    bResult &= SetRenderState( D3DRS_DITHERENABLE, (DWORD) TRUE );
    bResult &= SetRenderState( D3DRS_SPECULARENABLE, (DWORD) FALSE );
    bResult &= SetRenderState( D3DRS_AMBIENT, (DWORD) 0x00ffffff );
    bResult &= SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD) FALSE );
    bResult &= SetRenderState( D3DRS_CLIPPING, (DWORD)FALSE);
	return bResult;
}

//-----------------------------------------------------------------------------

bool CLockDepth::SetDefaultMatrices()
{
	bool bResult = true;

    // Set the transform matrices
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f,-5.0f,-10.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );
    D3DXMATRIX  matWorld, matView, matProj;

    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    float fAspect = (float)GetCurrentMode()->pDisplay->dwWidth / (float)GetCurrentMode()->pDisplay->dwHeight;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );

    bResult &= SetTransform( D3DTS_WORLD,      &matWorld );
    bResult &= SetTransform( D3DTS_VIEW,       &matView );
    bResult &= SetTransform( D3DTS_PROJECTION, &matProj );
	return bResult;
}

//-----------------------------------------------------------------------------

UINT CLockDepth::TestInitialize()
{
    UINT uResult = CD3DTest::TestInitialize();

    if(uResult == D3DTESTINIT_RUN)
    {
		// set the test range
        SetTestRange(1, NUMBEROFTESTS * m_vecDepthStencil.size() );

		// begin resource creation
		if( false == CreateVertexBuffer(MAX_VERTICES*sizeof(TEX2DVERTEX), USAGE_DYNAMIC, D3DFVF_TEX2DVERTEX, POOL_SYSTEMMEM, &m_pVB ) )
			return D3DTESTINIT_ABORT;

		static const float sx = 0.0f;
		static const float sy = 0.0f;
		static const float w = (float)GetCurrentMode()->pDisplay->dwWidth;
		static const float h = (float)GetCurrentMode()->pDisplay->dwHeight;

		static const TEX2DVERTEX vbData[] =
		{
			InitTex2DVertex(D3DXVECTOR4(sx+0-0.5f,sy+h-0.5f,1.0f,1.0f), D3DXVECTOR2(0.f,1.f)),
			InitTex2DVertex(D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.0f,1.0f), D3DXVECTOR2(0.f,0.f)),
			InitTex2DVertex(D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.0f,1.0f), D3DXVECTOR2(1.f,1.f)),
			InitTex2DVertex(D3DXVECTOR4(sx+w-0.5f,sy+0-0.5f,1.0f,1.0f), D3DXVECTOR2(1.f,0.f)),
			InitTex2DVertex(D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.0f,1.0f), D3DXVECTOR2(1.f,1.f)),
			InitTex2DVertex(D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.0f,1.0f), D3DXVECTOR2(0.f,0.f))
		};

		BYTE* pVertices = 0L;
		if( false == m_pVB->Lock( 0, 0, &pVertices, LOCK_NOSYSLOCK ) )
		{
			WriteToLog( _T("Failed to lock the vertex buffer. Aborting tests\n") );
			uResult = D3DTESTINIT_ABORT;
		}
		else
		{
			memcpy( pVertices, (LPVOID)vbData, sizeof(vbData) );
 			m_pVB->Unlock();
		}

		if( 0L == (m_pTextures = CreateNoisyTextures() ) )
		{
			WriteToLog( _T("Failed to create noisy textures. Aborting tests\n") );
			uResult = D3DTESTINIT_ABORT;
		}
		else
		{
			SetTexture(0, m_pTextures);
		}

		if( D3DTESTINIT_RUN != uResult )
		{
			RELEASE( m_pVB );
		}
    }

    return uResult;
}

//-----------------------------------------------------------------------------

bool CLockDepth::ExecuteTest(UINT nTest)
{
	bool bResult = true;
	TCHAR szTestCase[256];

	// see if the depth stencil surface has changed
	if( (nTest-1) / m_vecDepthStencil.size() != m_iLastDepthStencilSurface )
	{
		m_iLastDepthStencilSurface = (nTest-1) / NUMBEROFTESTS;

		if( m_pSrcDevice->GetInterface() >= 9.0f )
			bResult &= SetDepthStencilSurface( m_vecDepthStencil[m_iLastDepthStencilSurface].pSurfaceDepthStencil );
		else
			bResult &= SetRenderTarget( (LPSURFACES)NULL, m_vecDepthStencil[m_iLastDepthStencilSurface].pSurfaceDepthStencil );
	}

	FmtToString((FORMATTYPE)0, &(m_vecDepthStencil[m_iLastDepthStencilSurface].fmtDepthStencil), szTestCase);
	_stprintf( szTestCase, _T("%s #%d"), szTestCase, nTest );
    BeginTestCase( szTestCase, nTest );

    bResult &= ValidateZBuffers(m_vecDepthStencil[m_iLastDepthStencilSurface].fmtDepthStencil);
	if( false == bResult )
	{
		Fail();
		//Skip the rest of the test;
		return false;
	}

	bResult &= ClearFrame();
	bResult &= PrepareZBuffers( nTest );
	
	switch( nTest % NUMBEROFTESTS )
	{
	case 1:
	case 2:
	case 3:
		bResult &= RenderClearFrame( nTest );
		break;
	default:
		bResult &= Render( nTest );
	}
	
	bResult &= CompareZBuffers();

	if( false == bResult)
		Fail();

	return true;
}

//-----------------------------------------------------------------------------

bool CLockDepth::ProcessFrame(void)
{
	return CD3DTest::ProcessFrame();
}

//-----------------------------------------------------------------------------

// sha nah nah nah, sha nah nah nah, hey hey..... goodbye.
bool CLockDepth::TestTerminate()
{

	RELEASE( m_pVB );
	RELEASE( m_pTextures );
	if( m_pSrcDevice->GetInterface() >= 9.0f )
		SetDepthStencilSurface( NULL );
	else
		SetRenderTarget( (LPSURFACES)NULL, NULL );
	for(vector<DEPTHSURFACE>::iterator itDepthSurface = m_vecDepthStencil.begin(); itDepthSurface < m_vecDepthStencil.end(); itDepthSurface++ )
	{
		RELEASE( (*itDepthSurface).pSurfaceDepthStencil );
	}

	if( KeySet(_T("FORCE") ) )
		ResetDriverFlags( );

	return CD3DTest::TestTerminate( );
}


//-----------------------------------------------------------------------------

// Check caps
bool CLockDepth::CapsCheck()
{
	m_vecDepthStencil.clear();
	DEPTHSURFACE depthSurface;
	depthSurface.fmtDepthStencil	= FMT_D16_LOCKABLE;
	if( IsDepthStencilFormatSupported( depthSurface.fmtDepthStencil ) &&
		InitZBuffers(depthSurface.fmtDepthStencil, &(depthSurface.pSurfaceDepthStencil)) )
		m_vecDepthStencil.push_back( depthSurface );
	if( m_pSrcDevice->GetInterface() >= 9.0f )
	{
		depthSurface.fmtDepthStencil	= FMT_D32F_LOCKABLE;
		if( IsDepthStencilFormatSupported( depthSurface.fmtDepthStencil ) &&
			InitZBuffers(depthSurface.fmtDepthStencil, &(depthSurface.pSurfaceDepthStencil)) )
			m_vecDepthStencil.push_back( depthSurface );
	}
	m_iLastDepthStencilSurface = -1;

	// see if we have any support for lockable depth stencil; otherwise skip the tests
	if( 0 == m_vecDepthStencil.size() )
	{
		WriteToLog( _T("Device doesn't support any lockable depth stencil. Skipping tests\n") );
		return false;
	}
	return true;
}


//-----------------------------------------------------------------------------

// verifies if the device can support this depth stencil format
bool CLockDepth::IsDepthStencilFormatSupported( FORMAT fmtDepthStencil )
{
	// verify the Src device can handle the desired format
	bool bFormatHALSupported =	SUCCEEDED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pSrcDevice->GetPresParams()->DesktopFormat, USAGE_DEPTHSTENCIL, RTYPE_SURFACE, fmtDepthStencil) ) &&
								SUCCEEDED(m_pD3D->CheckDepthStencilMatch(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pSrcDevice->GetPresParams()->DesktopFormat, m_pSrcDevice->GetPresParams()->BackBufferFormat, fmtDepthStencil) );
	if( !bFormatHALSupported )
		WriteToLog(_T("%d is not supported by the HAL device.\n"),fmtDepthStencil.d3dfFormat);
			
	// verify the Ref device can handle the desired format
	bool bFormatREFSupported =	SUCCEEDED(m_pD3D->CheckDeviceFormat(m_pRefDevice->GetAdapterID(), m_pRefDevice->GetDevType(), m_pRefDevice->GetPresParams()->DesktopFormat, USAGE_DEPTHSTENCIL, RTYPE_SURFACE, fmtDepthStencil) ) &&
								SUCCEEDED(m_pD3D->CheckDepthStencilMatch(m_pRefDevice->GetAdapterID(), m_pRefDevice->GetDevType(), m_pRefDevice->GetPresParams()->DesktopFormat, m_pRefDevice->GetPresParams()->BackBufferFormat, fmtDepthStencil) );
	if( !bFormatREFSupported )
		WriteToLog(_T("%d is not supported by the REF device.\n"),fmtDepthStencil.d3dfFormat);

	return bFormatHALSupported && bFormatREFSupported;
}

//-----------------------------------------------------------------------------

bool CLockDepth::Render( UINT nTest )
{
	Clear( 0, NULL, CLEARF_TARGET, 0, 1.0f, 0 );
	BeginScene();
	SetVertexShader( D3DFVF_TEX2DVERTEX);
	SetStreamSource( 0, m_pVB, sizeof(TEX2DVERTEX));
	DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2);
	EndScene();

    return true;
}

//-----------------------------------------------------------------------------

bool CLockDepth::RenderClearFrame( UINT nTest )
{
	float Z;
	switch ( nTest % NUMBEROFTESTS )
	{
	case 3:
		Z = 0.5f;
		break;
	case 2:
		Z = 0.0f;
		break;
	case 1:
		Z = 1.0f;
		break;
	}
	Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, Z, 0 );

	BeginScene();
	SetVertexShader( D3DFVF_TEX2DVERTEX);
	SetStreamSource( 0, m_pVB, sizeof(TEX2DVERTEX));
	DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2);
	EndScene();

    return true;
}

//-----------------------------------------------------------------------------

bool CLockDepth::InitZBuffers(FORMAT fmtDepthStencil, LPSURFACES* ppDepthStencilSurface)
{
	bool bResult;
	TCHAR sz[256];
	*ppDepthStencilSurface = NULL;

	bResult = CreateDepthStencilSurface(
										  GetCurrentMode()->pDisplay->dwWidth,
										  GetCurrentMode()->pDisplay->dwHeight,
										  fmtDepthStencil,
										  MULTISAMPLE_NONE,
										  ppDepthStencilSurface
										);

	if( false == bResult )
	{
		FmtToString((FORMATTYPE)0, &fmtDepthStencil, sz);
		WriteToLog(_T("Could not create %s Depth Stencil Surface\n"), sz);
		return false;
	}

	return bResult;
}

//-----------------------------------------------------------------------------

// make sure we have a valid z-buffer
bool CLockDepth::ValidateZBuffers(FORMAT fmtDepthStencil)
{
	bool bResult = true;
	LPSURFACE pDepthSurface = 0L;
	bResult &= SUCCEEDED(m_pSrcDevice->GetDepthStencilSurface( &pDepthSurface ));
	if( 0L == pDepthSurface )
		return false;

	SURFACEDESC sd;
	memset( &sd, 0, sizeof(sd) );
	bResult &= SUCCEEDED(pDepthSurface->GetDesc(&sd));
	if( sd.Format.d3dfFormat != fmtDepthStencil.d3dfFormat )
		bResult = false;
	RELEASE( pDepthSurface );

	pDepthSurface = 0L;
	bResult &= SUCCEEDED(m_pRefDevice->GetDepthStencilSurface( &pDepthSurface ));
	if( 0L == pDepthSurface )
		return false;
	memset( &sd, 0, sizeof(sd) );
	bResult &= SUCCEEDED(pDepthSurface->GetDesc(&sd));
	if( sd.Format.d3dfFormat != fmtDepthStencil.d3dfFormat )
		bResult = false;

	RELEASE( pDepthSurface );
	return bResult;
}

//-----------------------------------------------------------------------------

// lock the zbuffer and populate it with data
bool CLockDepth::PrepareZBuffers(UINT nTest)
{
	bool bResult = true;

	for( int dev=0; dev<2; dev++)
	{
		LPSURFACE pDepthSurface = 0L;
		bResult &= 0==dev ?	SUCCEEDED(m_pSrcDevice->GetDepthStencilSurface( &pDepthSurface )) :
							SUCCEEDED(m_pRefDevice->GetDepthStencilSurface( &pDepthSurface ));
		if( false == bResult )
			return false;

		// Lock
		LOCKED_RECT LockedRect;
		memset( &LockedRect, 0, sizeof(LockedRect) );
		bResult &= SUCCEEDED(pDepthSurface->LockRect(&LockedRect, NULL, LOCK_NOSYSLOCK));

		int iWidth  = (int)GetCurrentMode()->pDisplay->dwWidth;
		int iHeight = (int)GetCurrentMode()->pDisplay->dwHeight;

		WORD* pThisWord = NULL;
		float* pThisFloat = NULL;
		// populate:
		switch( m_vecDepthStencil[m_iLastDepthStencilSurface].fmtDepthStencil.d3dfFormat )
		{
		case D3DFMT_D16_LOCKABLE:
			for( int y=0; y<iHeight; y++)
			{
				pThisWord = (WORD*)((BYTE*)LockedRect.pBits + y * LockedRect.iPitch);
				for( int x=0; x<iWidth; x++)
					*pThisWord++ = GetZDepthWord( nTest, (float)x-0.5f,(float)y-0.5f );
			}
			break;
		case D3DFMT_D32F_LOCKABLE:
			for( int y=0; y<iHeight; y++)
			{
				pThisFloat = (float*)((BYTE*)LockedRect.pBits + y * LockedRect.iPitch);
				for( int x=0; x<iWidth; x++)
					*pThisFloat++ = GetZDepthFloat( nTest , (float)x-0.5f,(float)y-0.5f );
			}
			break;
		default:
			// unsupported format
			bResult = false;
		}

		// Unlock
		bResult &= SUCCEEDED(pDepthSurface->UnlockRect());
		RELEASE( pDepthSurface );
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CLockDepth::CompareZBuffers()
{
	bool bResult = true;
	LPSURFACE pSrcDepthSurface=0L, pRefDepthSurface=0L;
	LOCKED_RECT SrcLockedRect, RefLockedRect;
	memset(&SrcLockedRect,0,sizeof(SrcLockedRect));
	memset(&RefLockedRect,0,sizeof(RefLockedRect));
	WORD *pwSrc = NULL,*pwRef = NULL;
	float *pfSrc = NULL, *pfRef = NULL;
	int iWidth  = (int)GetCurrentMode()->pDisplay->dwWidth;
	int iHeight = (int)GetCurrentMode()->pDisplay->dwHeight;

	bResult &= SUCCEEDED(m_pSrcDevice->GetDepthStencilSurface( &pSrcDepthSurface ));
	if( false == bResult )
		return false;

	bResult &= SUCCEEDED(m_pRefDevice->GetDepthStencilSurface( &pRefDepthSurface ));
	if( false == bResult )
		goto out;

	bResult &= SUCCEEDED(pSrcDepthSurface->LockRect(&SrcLockedRect, NULL, LOCK_NOSYSLOCK | LOCK_READONLY));
	if( false == bResult )
		goto out;

	bResult &= SUCCEEDED(pRefDepthSurface->LockRect(&RefLockedRect, NULL, LOCK_NOSYSLOCK | LOCK_READONLY));
	if( false == bResult )
		goto out;

	int iErrors = 0;
	switch( m_vecDepthStencil[m_iLastDepthStencilSurface].fmtDepthStencil.d3dfFormat )
	{
	case D3DFMT_D16_LOCKABLE:
		for(int y=0; y<iHeight; y++)
		{
			pwSrc = (WORD*)((BYTE*)SrcLockedRect.pBits + y * SrcLockedRect.iPitch);
			pwRef = (WORD*)((BYTE*)RefLockedRect.pBits + y * RefLockedRect.iPitch);
			if( 0 != memcmp( pwSrc, pwRef, iWidth * sizeof(WORD) ) )
			{
				for( int x=0; x<iWidth; x++)
				{
					if( *pwSrc - *pwRef + m_wTolerance > m_wTolerance<<1 )
					{
						if( iErrors < (int)m_uMaxFailsLog )
							WriteToLog(_T("Pixel[%d][%d] Source = %d  != Ref = %d\n"), x,y,(int)*pwSrc,(int)*pwRef );
						iErrors++;
					}
					pwSrc++;
					pwRef++;
				}
			}
		}
		break;
	case D3DFMT_D32F_LOCKABLE:
		for(int y=0; y<iHeight; y++)
		{
			pfSrc = (float*)((BYTE*)SrcLockedRect.pBits + y * SrcLockedRect.iPitch);
			pfRef = (float*)((BYTE*)RefLockedRect.pBits + y * RefLockedRect.iPitch);
			if( 0 != memcmp( pfSrc, pfRef, iWidth * sizeof(float) ) )
			{
				for( int x=0; x<iWidth; x++)
				{
					if( *pfSrc - *pfRef + m_wTolerance > m_wTolerance<<1 )
					{
						if( iErrors < (int)m_uMaxFailsLog )
							WriteToLog(_T("Pixel[%d][%d] Source = %d  != Ref = %d\n"), x,y,(int)*pfSrc,(int)*pfRef );
						iErrors++;
					}
					pfSrc++;
					pfRef++;
				}
			}
		}
		break;
	}

	if( iErrors > 0 )
	{
		WriteToLog( _T("%d errors in depth buffer.\n"), (UINT)iErrors );
		Fail();
	}

out:
	bResult &= SUCCEEDED(pSrcDepthSurface->UnlockRect() );
	bResult &= SUCCEEDED(pRefDepthSurface->UnlockRect() );
	RELEASE(pSrcDepthSurface);
	RELEASE(pRefDepthSurface);
	return bResult;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
//  Determines what value to populate our zbuffer with prior to rendering
//
//-----------------------------------------------------------------------------

WORD CLockDepth::GetZDepthWord(UINT nTest, float x, float y)
{
	nTest = nTest % NUMBEROFTESTS;
	WORD w;

	w = (WORD) ((x * 16.0f* (float)nTest) + y * 500.0f);
	w += (WORD) ((x * 201.0f) - (y * 20.0f * (float)nTest));

	if( nTest > NUMBEROFTESTS )
		w = (WORD) (nTest-1-NUMBEROFTESTS) * 5001;

	return w;
}


float CLockDepth::GetZDepthFloat(UINT nTest, float x, float y)
{
	float f;
	WORD w = GetZDepthWord( nTest, x, y );

	f = w / (float)0xffff;

	return f;
}


//-----------------------------------------------------------------------------

bool CLockDepth::SetDriverFlags(DWORD dwFlags, bool bValue)
{
	LONG    lResult;
	HKEY    hKey;

	// Open the key

	lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Direct3D"),
							0, NULL, 0, KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY, 
							NULL, &hKey, NULL);
	if(lResult != ERROR_SUCCESS)
		return false;

	// Query for the value
   
	DWORD dwType, dwValue, dwSize = 4;

	// make sure the ForceDriverFlagsOn key is ON if requested.
	lResult = RegQueryValueEx(hKey, _T("ForceDriverFlagsOn"), 0, &dwType, (BYTE *)&dwValue, &dwSize);
	if(ERROR_SUCCESS != lResult)
	{
		dwValue = (bValue == true) ? dwFlags | 0 : 0 & (~dwFlags);
		m_bHadForceDriversOnKey = false;
		
		// Since this machine didn't have the value to begin with, we
		// must create one.

		lResult = RegSetValueEx(hKey, _T("ForceDriverFlagsOn"), 0, REG_DWORD, (BYTE *)&dwValue, dwSize);
		if(ERROR_SUCCESS != lResult) 
		{
			RegCloseKey(hKey);
			return false;
		}
	} 
	else 
	{
		// We already had a value, record it and change it
		m_bHadForceDriversOnKey = true;
		m_dwOldForceDriversOnValue = dwValue;
		dwValue = (bValue == true) ? dwFlags | dwValue : dwValue & (~dwFlags);
		
		lResult = RegSetValueEx(hKey, _T("ForceDriverFlagsOn"), 0, REG_DWORD, (BYTE *)&dwValue, dwSize);
		if(ERROR_SUCCESS != lResult)
		{
			RegCloseKey(hKey);
			return false;
		}
	}

	// make sure the ForceDriverFlagsOff key is OFF if requested.
	lResult = RegQueryValueEx(hKey, _T("ForceDriverFlagsOff"), 0, &dwType, (BYTE *)&dwValue, &dwSize);
	if(ERROR_SUCCESS != lResult)
	{
		dwValue = (bValue == true) ?  0 & (~dwFlags) : 0 | dwFlags ;
		m_bHadForceDriversOffKey = false;
		
		// Since this machine didn't have the value to begin with, we
		// must create one.

		lResult = RegSetValueEx(hKey, _T("ForceDriverFlagsOff"), 0, REG_DWORD, (BYTE *)&dwValue, dwSize);
		if(ERROR_SUCCESS != lResult) 
		{
			RegCloseKey(hKey);
			return false;
		}
	} 
	else 
	{
		// We already had a value, record it and change it
		m_bHadForceDriversOffKey = true;
		m_dwOldForceDriversOffValue = dwValue;
		dwValue = (bValue == true) ? dwValue & (~dwFlags) : dwValue | dwFlags;
		
		lResult = RegSetValueEx(hKey, _T("ForceDriverFlagsOff"), 0, REG_DWORD, (BYTE *)&dwValue, dwSize);
		if(ERROR_SUCCESS != lResult)
		{
			RegCloseKey(hKey);
			return false;
		}
	}

	RegCloseKey(hKey);

	return true;
}

//-----------------------------------------------------------------------------

bool CLockDepth::ResetDriverFlags()
{
	LONG    lResult;
	HKEY    hKey;
	bool	bRet = true;

	// Open the key

	lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Direct3D"),
							0, NULL, 0, KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY, 
							NULL, &hKey, NULL);
	if(lResult != ERROR_SUCCESS)
		return false;

	// set the value
	DWORD dwType, dwValue, dwSize = 4;

	if( m_bHadForceDriversOnKey )
	{
		dwValue = m_dwOldForceDriversOnValue;
		bRet &= (lResult = RegSetValueEx(hKey, _T("ForceDriverFlagsOn"), 0, REG_DWORD, (BYTE *)&dwValue, dwSize) != ERROR_SUCCESS);
	}
	else
	{
		bRet &= (lResult = RegDeleteValue( hKey, _T("ForceDriverFlagsOn") ) != ERROR_SUCCESS);
	}

	if( m_bHadForceDriversOffKey )
	{
		dwValue = m_dwOldForceDriversOffValue;
		bRet &= (lResult = RegSetValueEx(hKey, _T("ForceDriverFlagsOff"), 0, REG_DWORD, (BYTE *)&dwValue, dwSize) != ERROR_SUCCESS);
	}
	else
	{
		bRet &= (lResult = RegDeleteValue( hKey, _T("ForceDriverFlagsOff") ) != ERROR_SUCCESS);
	}

	RegCloseKey(hKey);

	return true;
}

//-----------------------------------------------------------------------------

LPTEXTURES CLockDepth::CreateNoisyTextures()
{
	const int iWidth  =256;
	const int iHeight =256;

	bool bResult = true;
	LPTEXTURES pTextures = 0L;
	
	bResult = CreateTexture( iWidth, iHeight, 1, 0, FMT_X8R8G8B8, POOL_MANAGED, &pTextures, CD3D_ALL);
	if( false == bResult )
	{
		bResult = CreateTexture( iWidth, iHeight, 1, 0, FMT_R5G6B5, POOL_MANAGED, &pTextures, CD3D_ALL);
		if( false == bResult )
		{
			bResult = CreateTexture( iWidth, iHeight, 1, 0, FMT_X1R5G5B5, POOL_MANAGED, &pTextures, CD3D_ALL);
			if( false == bResult )
				return 0L;
		}
	}

	SURFACEDESC sd;
	memset( &sd, 0, sizeof(sd) );
	bResult &= pTextures->GetLevelDesc( 0, &sd );
	if( false == bResult )
		goto out;
	
	LOCKED_RECT lr;
	memset( &lr, 0, sizeof(lr) );
	bResult &= pTextures->LockRect( 0, &lr, NULL, LOCK_NOSYSLOCK, CD3D_ALL );
	if( false == bResult )
		goto out;

	for( int y = 0; y< iHeight; y++)
	{
		DWORD* pdw = (DWORD*)((BYTE*)lr.pBits + y*lr.iPitch);
		WORD*  pw  = (WORD*) ((BYTE*)lr.pBits + y*lr.iPitch);

		for( int x=0; x<iWidth; x++ )
		{
			// carve out the wood grain
			float d = (float)(
								PerlinNoise_2D( (double)x/40.f,
											    (double)y/300.f,
											    4,
											    0.5f
											    )
								);
			float f = d * 20.0f;
			f = f - floor(f);		// make f = 0-1

			// add a very fine grain in vertical direction
			float nf = 0.5f * (float)PerlinNoise_2D(x*5.f, -20.0f + y*0.001, 2, 0.25f);
			nf *= nf;
			BYTE r,g,b,a;
			r = (BYTE)((140.0 * f + 100.0)*(1.0 - nf)) + (BYTE)(255.0*nf);
			g = (BYTE)((70.0 * (f) + 50.0)*(1.0 - nf)) + (BYTE)(250.0*nf);
			b = (BYTE)((60.0 * (f) + 20.0)*(1.0 - nf)) + (BYTE)(240.0*nf);
			a = (BYTE)0xff;
			switch( sd.Format.d3dfFormat )
			{
			case D3DFMT_X8R8G8B8:
				*pdw = b | g<<8 | r<<16 | 0xff<<24;
				pdw++;
				break;
			case D3DFMT_X1R5G5B5:
				*pw = (b>>3) | (g>>3)<<5 | (r>>3)<<10;
				pw++;
				break;
			case D3DFMT_R5G6B5:
				*pw = (b>>3) | (g>>2)<<5 | (r>>3)<<11;
				pw++;
				break;
			}
		}
	}

	bResult &= pTextures->UnlockRect(0, CD3D_ALL);
	if( false == bResult )
		goto out;

	return pTextures;

out:
	RELEASE( pTextures );
	return 0L;

}
