// DynamicTexture.cpp: implementation of the CDynamicTextureTest class.
//
//////////////////////////////////////////////////////////////////////

#include "DynamicTexture.h"
#include "dx8decl.h"

#define UPPER_LEFT	0
#define UPPER_RIGHT	1
#define LOWER_LEFT	2
#define LOWER_RIGHT	3
#define ALL			4

// these LockRect values are designed to 64x64 texture
RECT lockRect[] = 
{	// left	top	right	bottom
	{	0,	0,	32,		32 },	// UPPER_LEFT
	{	32,	0,	64,		32},	// UPPER_RIGHT
	{	0,	32,	32,		64},	// LOWER_LEFT
	{	32,	32,	64,		64},	// LOWER_RIGHT
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDynamicTextureTest::CDynamicTextureTest() : 
	m_pVertexBuffers( NULL ),
	m_pIndexBuffers( NULL ),
	m_pVertexDeclarations( NULL )
{
	m_szTestName	= _T("DynamicTexture");
	m_szCommandKey	= _T("DynamicTexture");
	SetTestType(TESTTYPE_CONF);
	
	m_Options.D3DOptions.fMinDXVersion		= 8.1f;
	m_Options.D3DOptions.bReference			= true;
	m_Options.D3DOptions.bZBuffer			= true;
	m_Options.D3DOptions.dwBehaviors = m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_BESTAVAILABLE;
	m_pFramework->GetImageCmp()->SetPassRequired(0.98f);		// we want perfect match
	
	m_bClearBetweenPresent	= KeySet( _T("Clear") );
	m_bForceREFFailure		= KeySet( _T("ForceREFFailure") );
}

CDynamicTextureTest::~CDynamicTextureTest()
{
}


//////////////////////////////////////////////////////////////////////
// Name: CommandLineHelp()
// Desc: command line help for the test
//////////////////////////////////////////////////////////////////////
void CDynamicTextureTest::CommandLineHelp()
{
	WriteCommandLineHelp(_T("The application tests Dynamic Textures\n"));
	WriteCommandLineHelp(_T("A sequence of Locks will be performed with various flags\n"));
	WriteCommandLineHelp(_T("The result will be compared with a reference image (1 lock)\n"));
	WriteCommandLineHelp(_T("Dim Cubes: original texture\n"));
	WriteCommandLineHelp(_T("Purple Cube: 1st lock, Yellow Cube: 2nd lock, Green Cube: 3rd lock, Blue Cube: 4th lock\n"));
	WriteCommandLineHelp(_T("White Cube: previous DISCARD buffer. Should not be seen and causes test to fail.\n"));
}


//////////////////////////////////////////////////////////////////////
// Name: Check_HRFail( HRESULT, const char )
// Desc: if hResult is not D3D_OK, throws an exception
//////////////////////////////////////////////////////////////////////
void CDynamicTextureTest::Check_HRFail( HRESULT hResult, const char *sMethod )
{
	if( FAILED(hResult) )
	{
		WriteToLog( TEXT( "%s failed, error code %s (0x%x)\n" ),
				sMethod,
				m_pD3D->HResultToString( hResult ),
				hResult );
		if( m_pFramework->m_DXGLog.InTestCase() )
			Fail();
		throw hResult;
	}
}


//////////////////////////////////////////////////////////////////////
// Name:	BuildTestVectors()
// Desc:	virtual method for each test to initialize its vector list
//////////////////////////////////////////////////////////////////////
void CDynamicTextureTest::BuildTestVectors()
{
#if FAST_TEST
    DWORD arraydwLockNo[]			= {1, 2};
    // D3DLOCK_NOOVERWRITE will be supported in LH only
    // TODO: update the test so the flag is tested for LH only
    DWORD arraydwFlagsNo[]			= { D3DLOCK_DISCARD, /*D3DLOCK_NOOVERWRITE,*/ 0 };
    DWORD arraydwIndexRectNo[]		= {UPPER_LEFT, LOWER_RIGHT};
#else//FAST_TEST
	DWORD arraydwLockNo[]			= {1, 2, 3, 4};
	// D3DLOCK_NOOVERWRITE will be supported in LH only
	// TODO: update the test so the flag is tested for LH only
	DWORD arraydwFlagsNo[]			= { D3DLOCK_DISCARD, /*D3DLOCK_NOOVERWRITE,*/ 0 };
	DWORD arraydwIndexRectNo[]		= {UPPER_LEFT, UPPER_RIGHT, LOWER_LEFT, LOWER_RIGHT};
#endif//FAST_TEST

	m_arraydwLockNo		= vector<DWORD>(arraydwLockNo, arraydwLockNo + sizeof(arraydwLockNo) / sizeof DWORD);
	m_arraydwFlagsNo	= vector<DWORD>(arraydwFlagsNo, arraydwFlagsNo + sizeof(arraydwFlagsNo) / sizeof DWORD);
	m_arraydwIndexRectNo= vector<DWORD>(arraydwIndexRectNo, arraydwIndexRectNo + sizeof(arraydwIndexRectNo) / sizeof DWORD);
}


//////////////////////////////////////////////////////////////////////
// Name:	BuildTestList()
// Desc:	build test list
//////////////////////////////////////////////////////////////////////
void CDynamicTextureTest::BuildTestList()
{
	m_vectorTests.clear();
	BuildTestVectors();
	
	DWORD dwSizeFlags = m_arraydwFlagsNo.size();
	DWORD dwSizeIndexRect = m_arraydwIndexRectNo.size();

	for( int iLockNo = 0; iLockNo < m_arraydwLockNo.size(); iLockNo++ )
	{
		vector<DWORD> vecLockFlags(m_arraydwLockNo[iLockNo]);		// lock flags for each state
		int iStack;
		for(iStack = 0; iStack < m_arraydwLockNo[iLockNo]; iStack++ )
			vecLockFlags[iStack] = 0;
		iStack = 0;
		while( iStack < (int)m_arraydwLockNo[iLockNo] )
		{
			vectorTexture_Lock_Flags vectorTest(m_arraydwLockNo[iLockNo]);
			bool bValid = true;
			for( int iLock = 0; iLock < m_arraydwLockNo[iLockNo]; iLock++ )
			{
				// create Texture_Lock_Flags structure
				Texture_Lock_Flags lockFlags;
				lockFlags.Flags = m_arraydwFlagsNo[vecLockFlags[iLock] % dwSizeFlags];
				lockFlags.indexRectToLock = m_arraydwIndexRectNo[vecLockFlags[iLock] / dwSizeFlags];
				lockFlags.indexCubeToLock = ( iLock << 1 ) + 1;
				lockFlags.Level = 0;
			
				vectorTest[iLock] = lockFlags;

				// since DISCARD will drop the entire texture, subrects don't matter
				// so we'll just select the first value
				if( ( lockFlags.Flags & D3DLOCK_DISCARD ) && lockFlags.indexRectToLock )
					bValid = false;
			}

			// save this scenario
			if( bValid )
				m_vectorTests.push_back( vectorTest );

			// increment the stack anyway for next testcase
			for( iStack = 0; iStack < m_arraydwLockNo[iLockNo]; iStack++ )
				if( ++vecLockFlags[iStack] == dwSizeFlags * dwSizeIndexRect )	// did we reach the end?
					vecLockFlags[iStack] = 0;
				else
					break;
		}
	}
}


//////////////////////////////////////////////////////////////////////
// Name:	TestInitialize()
// Desc:	Called once for each group if CapsCheck succeeds
//////////////////////////////////////////////////////////////////////
UINT CDynamicTextureTest::TestInitialize()
{
	UINT uResult = CD3DTest::TestInitialize();

	if( D3DTESTINIT_RUN == uResult )
	{
		BuildTestList();
		SetTestRange( 1, m_vectorTests.size() );
	}

	return uResult;
}


//////////////////////////////////////////////////////////////////////
// Name:	CapsCheck()
// Desc:	Called once for each group before any test initialization has been done
//////////////////////////////////////////////////////////////////////
bool CDynamicTextureTest::CapsCheck()
{
	HRESULT hr;
	CAPS caps;
	if( FAILED( hr = m_pSrcDevice->GetDeviceCaps(&caps) ) )
	{
		WriteToLog(
				_T("Error: GetDeviceCaps failed: %s (0x%x). Some tests might show inaccurate results\n"),
				m_pD3D->HResultToString(hr),
				hr 
			);
		return false;
	}
	if( FAILED( ConfirmDevice( &caps, m_pSrcDevice->GetBehaviors(), FMT_UNKNOWN ) ) )
	{
		WriteToLog( _T("Device doesn't support dynamic textures\n") );
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
// Name:	Setup()
// Desc:	Called everytime devices are restored(group init, device lost, etc)
//////////////////////////////////////////////////////////////////////
bool CDynamicTextureTest::Setup()
{
	OneTimeSceneInit();
	return SUCCEEDED(InitDeviceObjects()) && SUCCEEDED(RestoreDeviceObjects());
}


//////////////////////////////////////////////////////////////////////
// Name:	Cleanup()
// Desc:	Called everytime devices are restored(group init, device lost, etc)
//////////////////////////////////////////////////////////////////////
void CDynamicTextureTest::Cleanup()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
}


//////////////////////////////////////////////////////////////////////
// Name:	TestTerminate()
// Desc:	Called once for each group if TestInitialize succeeds
//////////////////////////////////////////////////////////////////////
bool CDynamicTextureTest::TestTerminate()
{
	return CD3DTest::TestTerminate() && SUCCEEDED(FinalCleanup());
}


//////////////////////////////////////////////////////////////////////
// Name:	ExecuteTest( UINT )
// Desc:	Called once for each test defined by SetTestRange()
//////////////////////////////////////////////////////////////////////
bool CDynamicTextureTest::ExecuteTest(UINT nTest)
{
	try
	{
		TCHAR tszTestName[1024];
		m_TestIndex = nTest - 1;
		vectorTexture_Lock_Flags vectorFlags = m_vectorTests[m_TestIndex];
		_sntprintf( tszTestName, 1024, _T("%s: %d Locks"), m_szTestName, vectorFlags.size() );
		tszTestName[1023] = _T('\0');
		for( int iLock = 0; iLock < vectorFlags.size(); iLock++ )
		{
			TCHAR tszFlags[64];
			switch( ((Texture_Lock_Flags)vectorFlags[iLock]).Flags )
			{
			case 0:
				_tcscpy( tszFlags, _T(" (None) ") );
				break;
			case D3DLOCK_DISCARD:
				_tcscpy( tszFlags, _T(" (DISCARD) ") );
				break;
			case D3DLOCK_NOOVERWRITE:
				_tcscpy( tszFlags, _T(" (NOOVERWRITE) ") );
				break;
			default:
				_tcscpy( tszFlags, _T(" (Unknown) ") );
				break;
			}
			_tcsncat( tszTestName, tszFlags, 1024 - _tcslen(tszTestName) );
			tszTestName[1023] = _T('\0');
		}

		BeginTestCase( tszTestName, nTest );

		// advance the frame and render
		Check_HRFail( FrameMove(), "FrameMove" );
		Check_HRFail( Render(), "Render" );
	}
	catch(HRESULT)
	{
		Fail();
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
// Name:	CopyDynamicTexture( LOCKED_RECT *, bool, RECT *, D3DCOLOR )
// Desc:	update the texture; if it's an absolute lockedRect (NULL passed in LockRect), we need to offset
//////////////////////////////////////////////////////////////////////
void CDynamicTextureTest::CopyDynamicTexture( LOCKED_RECT *pLockedRect, bool bAbsoluteLockedAddress, RECT *pRect, D3DCOLOR color )
{
	LONG lYOffset = bAbsoluteLockedAddress? 0 : pRect->top;
	LONG lXOffset = bAbsoluteLockedAddress? 0 : pRect->left;
	for( LONG lY = pRect->top; lY < pRect->bottom; lY++ )
	{
		D3DCOLOR *pColor = (D3DCOLOR*) ((BYTE*)pLockedRect->pBits + ( lY - lYOffset ) * pLockedRect->iPitch + (pRect->left - lXOffset) * sizeof D3DCOLOR );
		for( LONG lX = pRect->left; lX < pRect->right; lX++ )
			*pColor++ = color;
	}
}


//////////////////////////////////////////////////////////////////////
// Name:	ConfirmDevice( CAPS*, DWORD, FORMAT )
// Desc:	Called during device initialization, this code checks the device for some minimum set of capabilities
//////////////////////////////////////////////////////////////////////
HRESULT CDynamicTextureTest::ConfirmDevice( CAPS* pCaps, DWORD dwBehavior, FORMAT Format )
{
	if( !( pCaps->Caps2 & D3DCAPS2_DYNAMICTEXTURES ) )
		return D3DERR_NOTAVAILABLE;

	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	OneTimeSceneInit()
// Desc:	Called during initial app startup, this function performs all the permanent initialization
//////////////////////////////////////////////////////////////////////
HRESULT CDynamicTextureTest::OneTimeSceneInit()
{
	m_p81VertexShaders = 0;
	m_pVertexDeclarations = NULL;
	m_pVertexBuffers = NULL;
	m_pIndexBuffers = NULL;
	m_pTextures = NULL;

	m_AllShapes.NewShape(CS_COMPOSITE);
	for( int x = 0; x < WIDTH; x++ )
		for( int y = 0; y < HEIGHT; y++ )
		{
			m_CubeShapes[x][y].NewShape(CS_BOX);
			m_CubeShapes[x][y].Scale(1.5);
			m_CubeShapes[x][y].Translate( 2*x - WIDTH + 1.0f, 2*y - HEIGHT + 1.0f, 0.0f );
			m_AllShapes.AddShape( m_CubeShapes[x][y] );
		}
	
	if( !LightEnable( 0,FALSE ) )
		return E_FAIL;

	// set default matrices
	D3DXMATRIX		Matrix;
	D3DXVECTOR3		from(0.0f, 1.0f, - 2.0f*WIDTH ), at(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f); 
	float			fAspect = (float)m_pCurrentMode->nBackBufferWidth/(float)m_pCurrentMode->nBackBufferHeight;
	float			fZn = 0.1f;
	float			fZf = 20.1f;

	// Projection matrix
	D3DXMatrixPerspectiveFovLH(&Matrix, D3DX_PI/3, fAspect, fZn, fZf); 
	if( !SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&Matrix) )
		return E_FAIL;

	// World matrix
	D3DXMatrixIdentity(&Matrix);
	if( !SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix) )
		return E_FAIL;

	// View matrix
	D3DXMatrixLookAtLH(&Matrix, &from, &at, &up);
	if( !SetTransform(D3DTS_VIEW, (D3DMATRIX*)&Matrix) )
		return E_FAIL;

	SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	m_VBStride = sizeof D3DVERTEX;
	VERTEXELEMENT D3DVERTEX_Elements[] =		// dynamic vertex elements
	{
		// Stream	Offset	Type					Method					Usage					UsageIndex
		{	0,		0,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
		{	0,		12,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0},
		{	0,		24,		D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0},
		D3DDECL_END()
	};
	m_VertexElements = vector<VERTEXELEMENT>( D3DVERTEX_Elements, D3DVERTEX_Elements + sizeof(D3DVERTEX_Elements) / sizeof VERTEXELEMENT );

	DWORD dwDecl[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG( D3DVSDE_POSITION,	D3DVSDT_FLOAT3 ),
		D3DVSD_REG( D3DVSDE_NORMAL,		D3DVSDT_FLOAT3 ),
		D3DVSD_REG( D3DVSDE_TEXCOORD0,	D3DVSDT_FLOAT2 ),
		D3DVSD_END()
	};
	m_p81VertexDeclaration = vector<DWORD>( dwDecl, dwDecl + sizeof( dwDecl ) / sizeof DWORD );

	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDeviceObjects()
// Desc:	Initialize device-dependent objects. This is the place to create mesh and texture objects.
//////////////////////////////////////////////////////////////////////
HRESULT CDynamicTextureTest::InitDeviceObjects()
{
	HRESULT hResult = D3D_OK;

	if(	!SetSamplerState( 0, SAMP_MINFILTER, TEXF_LINEAR) ||
		!SetSamplerState( 0, SAMP_MAGFILTER, TEXF_LINEAR) ||
		!SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 ) ||
		!SetTextureStageState( 0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE ) )
		hResult = E_FAIL;
	return hResult;
}


//////////////////////////////////////////////////////////////////////
// Name:	RestoreDeviceObjects()
// Desc:	Restore device-memory objects and state after a device is created or resized.
//////////////////////////////////////////////////////////////////////
HRESULT CDynamicTextureTest::RestoreDeviceObjects()
{
	try
	{
		if( !CreateTexture( 64,				// Width
							64,				// Height
							1,				// Mips
							USAGE_DYNAMIC,	// Usage (dynamic textures)
							FMT_A8R8G8B8,	// Format
							POOL_DEFAULT,	// Pool
							&m_pTextures ) ||
			!SetTexture( 0, m_pTextures ) ||
			!CreateVertexBuffer(	m_AllShapes.m_nVertices * m_VBStride,
									USAGE_WRITEONLY,
									0,
									POOL_DEFAULT,
									&m_pVertexBuffers ) ||
			!CreateIndexBuffer(		m_AllShapes.m_nIndices * sizeof WORD,
									USAGE_WRITEONLY,
									FMT_INDEX16,
									POOL_DEFAULT,
									&m_pIndexBuffers ) )
			throw E_FAIL;

		// dx8.1 interfaces don't support CreateVertexDeclaration
		if( m_pSrcDevice->GetInterface() <= 8.1f )
		{
			if( !CreateVertexShader( &m_p81VertexDeclaration[0], NULL, &m_p81VertexShaders, 0 ) )
				throw E_FAIL;
		}
		else
		{
			if( !CreateVertexDeclaration( &m_VertexElements[0], &m_pVertexDeclarations ) )
				throw E_FAIL;
		}

		// copy vertices
		BYTE *pVertixes;
		if( !m_pVertexBuffers->Lock( 0, 0, (void**)&pVertixes, 0 ) ) throw E_FAIL;
		memcpy( pVertixes, m_AllShapes.m_pVertices, m_AllShapes.m_nVertices * sizeof D3DVERTEX );
		if( !m_pVertexBuffers->Unlock() ) throw E_FAIL;

		// copy indices
		BYTE *pIndices;
		if( !m_pIndexBuffers->Lock( 0, 0, (void**)&pIndices, 0 ) ) throw E_FAIL;
		memcpy( pIndices, m_AllShapes.m_pIndices, m_AllShapes.m_nIndices * sizeof WORD );
		if( !m_pIndexBuffers->Unlock() ) throw E_FAIL;

		if( !SetIndices( m_pIndexBuffers, 0 ) ||
			!SetStreamSource( 0, m_pVertexBuffers, m_VBStride, 0 ) )
			throw E_FAIL;

		// dx8.1 doesn't know about vertex declarations
		if( m_pSrcDevice->GetInterface() <= 8.1f )
		{
			if( !SetVertexShader( m_p81VertexShaders ) )
				throw E_FAIL;
		}
		else
		{
			if( !SetVertexDeclaration( m_pVertexDeclarations ) )
				throw E_FAIL;
		}
		SetRenderState( D3DRS_ZENABLE, TRUE );
	}
	catch( HRESULT hError )
	{
		return hError;
	}

	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	FrameMove()
// Desc:	Called once per frame, the call is the entry point for animating the scene.
//////////////////////////////////////////////////////////////////////
HRESULT CDynamicTextureTest::FrameMove()
{
	try
	{
		if( !Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE( 0.8f, 0.8f, 0.8f, 1.0f ), 1.0f, 0L ) ) throw E_FAIL;

		// fill the texture with 4 dark blocks
		LOCKED_RECT LockedRect;
		if( !m_pTextures->LockRect( 0, &LockedRect, NULL, LOCK_DISCARD ) ) throw E_FAIL;
		CopyDynamicTexture( &LockedRect, true, &lockRect[UPPER_LEFT], D3DCOLOR_COLORVALUE( 0.5f, 0.25f, 0.25f, 0 ) );
		CopyDynamicTexture( &LockedRect, true, &lockRect[UPPER_RIGHT], D3DCOLOR_COLORVALUE( 0.25f, 0.5f, 0.25f, 0 ) );
		CopyDynamicTexture( &LockedRect, true, &lockRect[LOWER_LEFT], D3DCOLOR_COLORVALUE( 0.25f, 0.25f, 0.5f, 0 ) );
		CopyDynamicTexture( &LockedRect, true, &lockRect[LOWER_RIGHT], D3DCOLOR_COLORVALUE( 0.5f, 0.5f, 0.5f, 0 ) );
		if( !m_pTextures->UnlockRect( 0 ) ) throw E_FAIL;

		/*LPTEXTURE pHalTexture, pRefTexture;
		m_pTextures->GetTexture( &pHalTexture, 0 );
		m_pTextures->GetTexture( &pRefTexture, 1 );
		m_pSrcDevice->D3DXSaveTextureToFile( _T("C:\\HalTexture.bmp"),  D3DXIFF_BMP, pHalTexture, NULL );
		m_pRefDevice->D3DXSaveTextureToFile( _T("C:\\RefTexture.bmp"),  D3DXIFF_BMP, pRefTexture, NULL );
		RELEASE( pHalTexture );
		RELEASE( pRefTexture );*/
	}
	catch( HRESULT hError )
	{
		return hError;
	}

	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	Render()
// Desc:	Called once per frame, the call is the entry point for 3d
//			rendering. This function sets up render states, clears the
//			viewport, and renders the scene.
//////////////////////////////////////////////////////////////////////
HRESULT CDynamicTextureTest::Render()
{
	HRESULT hr = D3D_OK;
	vectorTexture_Lock_Flags vectorFlags = m_vectorTests[m_TestIndex];
	D3DCOLOR vecColors[] =	{	D3DCOLOR_COLORVALUE( 1.0f, 0.0f, 0.0f, 0 ),
								D3DCOLOR_COLORVALUE( 0.0f, 1.0f, 0.0f, 0 ),
								D3DCOLOR_COLORVALUE( 0.0f, 0.0f, 1.0f, 0 ),
								D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 1.0f, 0 )
							};
	try
	{
		// 1st step: present the original texture with darker blocks
		if( !BeginScene() ) throw E_FAIL;
		if( !DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_AllShapes.m_nVertices, 0, m_AllShapes.m_nIndices/3, 0 ) ) throw E_FAIL;
		if( !EndScene() ) throw E_FAIL;
		DisplayFrame();

		// clear the screen if desired, to focus on the locked parts
		if( m_bClearBetweenPresent )
			if( !Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE( 0.8f, 0.8f, 0.8f, 1.0f ), 1.0f, 0L ) ) throw E_FAIL;

		// perform all the locks and overdraw the new triangles
		if( !BeginScene() ) throw E_FAIL;
		for( int iLock = 0; iLock < vectorFlags.size(); iLock++ )
		{
			Texture_Lock_Flags *pLockFlags = &vectorFlags[iLock];

			LOCKED_RECT LockedRect;
			if( !m_pTextures->LockRect( pLockFlags->Level, &LockedRect, pLockFlags->Flags & D3DLOCK_DISCARD? NULL : &lockRect[pLockFlags->indexRectToLock], pLockFlags->Flags ) ) throw E_FAIL;
			if( pLockFlags->Flags & D3DLOCK_DISCARD )
			{
				CopyDynamicTexture( &LockedRect, true, &lockRect[UPPER_LEFT], UPPER_LEFT == pLockFlags->indexRectToLock? vecColors[pLockFlags->indexRectToLock % (sizeof(vecColors) / sizeof D3DCOLOR) ] : D3DCOLOR_COLORVALUE( 0.25f, 0.125f, 0.125f, 0 ) );
				CopyDynamicTexture( &LockedRect, true, &lockRect[UPPER_RIGHT], UPPER_RIGHT == pLockFlags->indexRectToLock? vecColors[pLockFlags->indexRectToLock % (sizeof(vecColors) / sizeof D3DCOLOR) ] : D3DCOLOR_COLORVALUE( 0.125f, 0.25f, 0.125f, 0 ) );
				CopyDynamicTexture( &LockedRect, true, &lockRect[LOWER_LEFT], LOWER_LEFT == pLockFlags->indexRectToLock? vecColors[pLockFlags->indexRectToLock % (sizeof(vecColors) / sizeof D3DCOLOR) ] : D3DCOLOR_COLORVALUE( 0.125f, 0.125f, 0.25f, 0 ) );
				CopyDynamicTexture( &LockedRect, true, &lockRect[LOWER_RIGHT], LOWER_RIGHT == pLockFlags->indexRectToLock? vecColors[pLockFlags->indexRectToLock % (sizeof(vecColors) / sizeof D3DCOLOR) ] : D3DCOLOR_COLORVALUE( 0.25f, 0.25f, 0.25f, 0 ) );
			}
			else
			{
				CopyDynamicTexture( &LockedRect, false, &lockRect[pLockFlags->indexRectToLock], vecColors[pLockFlags->indexRectToLock % (sizeof(vecColors) / sizeof D3DCOLOR) ] );
			}
			if( !m_pTextures->UnlockRect( pLockFlags->Level ) ) throw E_FAIL;

			if( m_bForceREFFailure )
			{
				LPTEXTURE pRefTexture;
				m_pTextures->GetTexture( &pRefTexture, 1 );

				// fill the texture with 4 dark blocks
				LOCKED_RECT LockedRect;
				if( FAILED( pRefTexture->LockRect( 0, &LockedRect, NULL, 0/*LOCK_DISCARD*/ ) ) ) throw E_FAIL;
				CopyDynamicTexture( &LockedRect, true, &lockRect[UPPER_LEFT], D3DCOLOR_COLORVALUE( 0.5f, 0.25f, 0.25f, 0 ) );
				CopyDynamicTexture( &LockedRect, true, &lockRect[UPPER_RIGHT], D3DCOLOR_COLORVALUE( 0.25f, 0.5f, 0.2f, 0 ) );
				CopyDynamicTexture( &LockedRect, true, &lockRect[LOWER_LEFT], D3DCOLOR_COLORVALUE( 0.25f, 0.25f, 0.5f, 0 ) );
				CopyDynamicTexture( &LockedRect, true, &lockRect[LOWER_RIGHT], D3DCOLOR_COLORVALUE( 0.5f, 0.5f, 0.5f, 0 ) );
				if( FAILED( pRefTexture->UnlockRect( 0 ) ) ) throw E_FAIL;

				RELEASE( pRefTexture );
			}

			UINT nVertices = 24;
			UINT nOffset = nVertices * pLockFlags->indexCubeToLock;
			UINT nStartIndex =  (36 * nOffset / 24);	// there are 36 indices for 24 vertices in a cube
			if( !DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nOffset, nVertices, nStartIndex, nVertices / 2, 0 ) ) throw E_FAIL;
		}
		if( !EndScene() ) throw E_FAIL;

		DisplayFrame();
    }
	catch( HRESULT hError )
	{
		EndScene();
		hr = hError;
	}

	return hr;
}


//////////////////////////////////////////////////////////////////////
// Name:	InvalidateDeviceObjects()
// Desc:	Called when the device-dependent objects are about to be lost.
//////////////////////////////////////////////////////////////////////
HRESULT CDynamicTextureTest::InvalidateDeviceObjects()
{
	RELEASE( m_pTextures );
	RELEASE( m_pVertexBuffers );
	RELEASE( m_pIndexBuffers );
	RELEASE( m_pVertexDeclarations );

	// DX8.1 test has to clean up the fake vertex shader
	if( m_pSrcDevice->GetInterface() <= 8.1f )	
	{
		if( m_p81VertexShaders )
			DeleteVertexShader( m_p81VertexShaders );
	}

	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	DeleteDeviceObjects()
// Desc:	Called when the app is exiting, or the device is being changed, this function deletes any device dependent objects.
//////////////////////////////////////////////////////////////////////
HRESULT CDynamicTextureTest::DeleteDeviceObjects()
{
	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	FinalCleanup()
// Desc:	Called before the app exits, this function gives the app the chance to cleanup after itself.
//////////////////////////////////////////////////////////////////////
HRESULT CDynamicTextureTest::FinalCleanup()
{
	return D3D_OK;
}
