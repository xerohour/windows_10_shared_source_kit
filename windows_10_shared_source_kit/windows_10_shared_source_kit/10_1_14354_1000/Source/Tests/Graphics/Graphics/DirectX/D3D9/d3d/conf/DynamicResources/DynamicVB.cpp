// DynamicVB.cpp: implementation of the CDynamicVBTest class.
//
//////////////////////////////////////////////////////////////////////

#include "DynamicVB.h"

typedef struct
{
	float x, y, z;
	float nx, ny, nz;
	D3DCOLOR color;
	float tu, tv;
} D3DVERTEXTEXTURECOLOR;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDynamicVBTest::CDynamicVBTest() : 
	m_pDynamicVertexBuffers( NULL ),
	m_pIndexBuffers( NULL ),
	m_pVertexDeclarations( NULL )
{
	m_szTestName	= _T("DynamicVB");
	m_szCommandKey	= _T("DynamicVB");
	SetTestType(TESTTYPE_CONF);
	
	m_Options.D3DOptions.fMinDXVersion		= 8.1f;
	m_Options.D3DOptions.bReference			= true;
	m_Options.D3DOptions.bZBuffer			= true;
	//m_Options.D3DOptions.dwDevTypes = m_Options.D3DOptions.dwRefDevType		= DEVICETYPE_HAL;
	m_Options.D3DOptions.dwBehaviors = m_Options.D3DOptions.dwRefBehaviors	= DEVICECREATE_STANDARD | DEVICECREATE_BESTAVAILABLE;
	m_pFramework->GetImageCmp()->SetPassRequired(0.98f);		// we want perfect match
	
	m_bClearBetweenPresent	= KeySet( _T("Clear") );
	m_bForceREFFailure		= KeySet( _T("ForceREFFailure") );
}

CDynamicVBTest::~CDynamicVBTest()
{
}


//////////////////////////////////////////////////////////////////////
// Name: CommandLineHelp()
// Desc: command line help for the test
//////////////////////////////////////////////////////////////////////
void CDynamicVBTest::CommandLineHelp()
{
	WriteCommandLineHelp(_T("The application tests Dynamic Vertex Buffers\n"));
	WriteCommandLineHelp(_T("A sequence of Locks will be performed with various flags\n"));
	WriteCommandLineHelp(_T("The result will be compared with a reference image (1 lock)\n"));
	WriteCommandLineHelp(_T("Dim Cubes: original vertex buffer\n"));
	WriteCommandLineHelp(_T("Purple Cube: 1st lock, Yellow Cube: 2nd lock, Green Cube: 3rd lock, Blue Cube: 4th lock\n"));
	WriteCommandLineHelp(_T("White Cube: previous DISCARD buffer. Should not be seen and causes test to fail.\n"));
}


//////////////////////////////////////////////////////////////////////
// Name: Check_HRFail( HRESULT, const char )
// Desc: if hResult is not D3D_OK, throws an exception
//////////////////////////////////////////////////////////////////////
void CDynamicVBTest::Check_HRFail( HRESULT hResult, const char *sMethod )
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
void CDynamicVBTest::BuildTestVectors()
{
#if FAST_TEST
    DWORD arraydwLockNo[] = { 1, 2 };
    DWORD arraydwFlagsNo[]			= { D3DLOCK_DISCARD, D3DLOCK_NOOVERWRITE, 0 };
    DWORD arraydwOffsetNo[]			= {0, 192};
    DWORD arraydwVerticesNo[]		= {24};
#else//FAST_TEST
	DWORD arraydwLockNo[]			= {1, 2, 3, 4};
	DWORD arraydwFlagsNo[]			= { D3DLOCK_DISCARD, D3DLOCK_NOOVERWRITE, 0 };
	DWORD arraydwOffsetNo[]			= {0, 48, 96, 144, 192};
	DWORD arraydwVerticesNo[]		= {24};
#endif//FAST_TEST

	m_arraydwLockNo		= vector<DWORD>(arraydwLockNo, arraydwLockNo + sizeof(arraydwLockNo) / sizeof DWORD);
	m_arraydwFlagsNo	= vector<DWORD>(arraydwFlagsNo, arraydwFlagsNo + sizeof(arraydwFlagsNo) / sizeof DWORD);
	m_arraydwOffsetNo	= vector<DWORD>(arraydwOffsetNo, arraydwOffsetNo + sizeof(arraydwOffsetNo) / sizeof DWORD);
	m_arraydwVerticesNo = vector<DWORD>(arraydwVerticesNo, arraydwVerticesNo + sizeof(arraydwVerticesNo) / sizeof DWORD);
}


//////////////////////////////////////////////////////////////////////
// Name:	BuildTestList()
// Desc:	build test list
//////////////////////////////////////////////////////////////////////
void CDynamicVBTest::BuildTestList()
{
	m_vectorTests.clear();
	BuildTestVectors();
	
	DWORD dwSizeFlags = m_arraydwFlagsNo.size();
	DWORD dwSizeOffset = m_arraydwOffsetNo.size();

	for( int iLockNo = 0; iLockNo < m_arraydwLockNo.size(); iLockNo++ )
	{
		vector<DWORD> vecLockFlags(m_arraydwLockNo[iLockNo]);		// lock flags for each state
		int iStack;
		for(iStack = 0; iStack < m_arraydwLockNo[iLockNo]; iStack++ )
			vecLockFlags[iStack] = 0;
		iStack = 0;
		while( iStack < (int)m_arraydwLockNo[iLockNo] )
		{
			// we want to filter so we don't lock the same area twice
			// it's not a valid case if such scenario is found
			bool bValid = true;
			for( iStack = 0; bValid && iStack < m_arraydwLockNo[iLockNo] - 1; iStack++ )
			{
				int iFlagsNo = m_arraydwFlagsNo[vecLockFlags[iStack] % dwSizeFlags];
				int iOffsetNo = m_arraydwOffsetNo[vecLockFlags[iStack] / dwSizeFlags];
				int iVerticesNo = m_arraydwVerticesNo[0];	// for now we assume that locking / filling 24 vertices is enough for a repro case

				for( int iNextStack = iStack + 1; bValid && iNextStack < m_arraydwLockNo[iLockNo]; iNextStack++ )
				{
					int iNextFlagsNo = m_arraydwFlagsNo[vecLockFlags[iNextStack] % dwSizeFlags];
					int iNextOffsetNo = m_arraydwOffsetNo[vecLockFlags[iNextStack] / dwSizeFlags];
					int iNextVerticesNo = m_arraydwVerticesNo[0];	// for now we assume that locking / filling 24 vertices is enough for a repro case

					// reduce complexity: for 4 locks, allow 0 as flas only at the beginning or end
					if( 4 == m_arraydwLockNo[iLockNo] && ( ( 0 == iFlagsNo && iOffsetNo != 0 && iOffsetNo != 192 ) ||
														 ( 0 == iNextFlagsNo && iNextOffsetNo != 0 && iNextOffsetNo != 192 ) ) )
						bValid = false;

					// TODO: we may want to add extra logic based on the current flags
					if( ( iOffsetNo  >= iNextOffsetNo && iOffsetNo < iNextOffsetNo + iNextVerticesNo ) ||
						( iOffsetNo + iVerticesNo >= iNextOffsetNo && iOffsetNo + iVerticesNo < iNextOffsetNo + iNextVerticesNo ) )
						bValid = false;	// intersecting intervals found, error!!!
				}
			}

			if( bValid )		// no overlapping locks found
			{
				vectorLock_Flags vectorTest(m_arraydwLockNo[iLockNo]);

				for( int iLock = 0; iLock < m_arraydwLockNo[iLockNo]; iLock++ )
				{
					// create Lock_Flags structure
					Lock_Flags lockFlags;
					lockFlags.Flags = m_arraydwFlagsNo[vecLockFlags[iLock] % dwSizeFlags];
					lockFlags.OffsetToLock = m_DynamicStride * m_arraydwOffsetNo[vecLockFlags[iLock] / dwSizeFlags];
					lockFlags.SizeToLock = m_DynamicStride * m_arraydwVerticesNo[0];	// for now we assume that locking / filling 24 vertices is enough for a repro case
				
					vectorTest[iLock] = lockFlags;
				}

				// save this scenario
				m_vectorTests.push_back( vectorTest );
			}

			// increment the stack anyway for next testcase
			for( iStack = 0; iStack < m_arraydwLockNo[iLockNo]; iStack++ )
				if( ++vecLockFlags[iStack] == dwSizeFlags * dwSizeOffset )	// did we reach the end?
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
UINT CDynamicVBTest::TestInitialize()
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
bool CDynamicVBTest::CapsCheck()
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
		WriteToLog( _T("Device doesn't support hardware vertex processing\n") );
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
// Name:	Setup()
// Desc:	Called everytime devices are restored(group init, device lost, etc)
//////////////////////////////////////////////////////////////////////
bool CDynamicVBTest::Setup()
{
	OneTimeSceneInit();
	return SUCCEEDED(InitDeviceObjects()) && SUCCEEDED(RestoreDeviceObjects());
}


//////////////////////////////////////////////////////////////////////
// Name:	Cleanup()
// Desc:	Called everytime devices are restored(group init, device lost, etc)
//////////////////////////////////////////////////////////////////////
void CDynamicVBTest::Cleanup()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
}


//////////////////////////////////////////////////////////////////////
// Name:	TestTerminate()
// Desc:	Called once for each group if TestInitialize succeeds
//////////////////////////////////////////////////////////////////////
bool CDynamicVBTest::TestTerminate()
{
	return CD3DTest::TestTerminate() && SUCCEEDED(FinalCleanup());
}


//////////////////////////////////////////////////////////////////////
// Name:	ExecuteTest( UINT )
// Desc:	Called once for each test defined by SetTestRange()
//////////////////////////////////////////////////////////////////////
bool CDynamicVBTest::ExecuteTest(UINT nTest)
{
	try
	{
		char szTestName[1024];
		m_TestIndex = nTest - 1;
		vectorLock_Flags vectorFlags = m_vectorTests[m_TestIndex];
		_snprintf( szTestName, 1024, "%s: %d Locks", m_szTestName, vectorFlags.size() );
		szTestName[1023] = '\0';
		for( int iLock = 0; iLock < vectorFlags.size(); iLock++ )
		{
			char szFlags[64];
			switch( ((Lock_Flags)vectorFlags[iLock]).Flags )
			{
			case 0:
				strcpy( szFlags, " (None) " );
				break;
			case D3DLOCK_DISCARD:
				strcpy( szFlags, " (DISCARD) " );
				break;
			case D3DLOCK_NOOVERWRITE:
				strcpy( szFlags, " (NOOVERWRITE) " );
				break;
			default:
				strcpy( szFlags, " (Unknown) " );
				break;
			}
			strncat( szTestName, szFlags, 1024 - strlen(szTestName) );
			szTestName[1023] = 0;
		}

		BeginTestCase( szTestName, nTest );

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
// Name:	CopyDynamicVertex( void*, D3DVERTEX*, D3DCOLOR )
// Desc:	copy the shape vertex into the dynamic Vertex
//////////////////////////////////////////////////////////////////////
void CDynamicVBTest::CopyDynamicVertex( void* pDynamicVertex, D3DVERTEX* pShapeVertex, D3DCOLOR color )
{
	D3DVERTEXTEXTURECOLOR *pMyDynamicVertex = (D3DVERTEXTEXTURECOLOR *)pDynamicVertex;
	pMyDynamicVertex->x = pShapeVertex->x;
	pMyDynamicVertex->y = pShapeVertex->y;
	pMyDynamicVertex->z = pShapeVertex->z;
	pMyDynamicVertex->nx = pShapeVertex->nx;
	pMyDynamicVertex->ny = pShapeVertex->ny;
	pMyDynamicVertex->nz = pShapeVertex->nz;
	pMyDynamicVertex->tu = pShapeVertex->tu;
	pMyDynamicVertex->tv = pShapeVertex->tv;
	pMyDynamicVertex->color = color;
}


//////////////////////////////////////////////////////////////////////
// Name:	ConfirmDevice( CAPS*, DWORD, FORMAT )
// Desc:	Called during device initialization, this code checks the device for some minimum set of capabilities
//////////////////////////////////////////////////////////////////////
HRESULT CDynamicVBTest::ConfirmDevice( CAPS* pCaps, DWORD dwBehavior, FORMAT Format )
{
	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	OneTimeSceneInit()
// Desc:	Called during initial app startup, this function performs all the permanent initialization
//////////////////////////////////////////////////////////////////////
HRESULT CDynamicVBTest::OneTimeSceneInit()
{
	m_AllShapes.NewShape(CS_COMPOSITE);
	for( int x = 0; x < WIDTH; x++ )
		for( int y = 0; y < HEIGHT; y++ )
		{
			m_CubeShapes[x][y].NewShape(CS_BOX);
			m_CubeShapes[x][y].Scale(1.5);
			m_CubeShapes[x][y].Translate( 2*x - WIDTH + 1.0f, 2*y - HEIGHT + 1.0f, 0.0f );
			m_AllShapes.AddShape( m_CubeShapes[x][y] );
		}
	
	// set default lights
	LIGHT light;
	D3DVECTOR	vLightPos;
	D3DVECTOR	vLightDir;
	
	vLightPos.x = 0.f; vLightPos.y = 2.f; vLightPos.z = -2.f;
	vLightDir.x = 0.f; vLightDir.y = -0.707f; vLightDir.z = 0.707f;

	ZeroMemory(&light,sizeof(LIGHT));
	light.dltType		= D3DLIGHT_SPOT;
	light.Diffuse.r		= 1.0f;
	light.Diffuse.g		= 1.0f;
	light.Diffuse.b		= 1.0f;
	light.Diffuse.a		= 1.0f;
	light.Specular.r	= 1.0f;
	light.Specular.g	= 1.0f;
	light.Specular.b	= 1.0f;
	light.Specular.a	= 1.0f;
	light.Position		= vLightPos;
	light.Direction		= vLightDir;
	light.Range			= 1000.f;
	light.Attenuation0	= 0.7f;
	light.Attenuation1	= 0.0f;
	light.Attenuation2	= 0.0f;
	light.Theta			= pi/2;
	light.Phi			= pi;

	if( !SetLight(0,&light) )
		return E_FAIL;
	if( !LightEnable(0,TRUE) )
		return E_FAIL;
	
	// set default material
	MATERIAL material;
	ZeroMemory(&material,sizeof(MATERIAL));
	material.diffuse.r  = 1.0f;
	material.diffuse.g  = 1.0f;
	material.diffuse.b  = 1.0f;
	material.diffuse.a  = 1.0f;
	
	material.ambient.r  = 1.0f;
	material.ambient.g  = 1.0f;
	material.ambient.b  = 1.0f;
	material.ambient.a  = 1.0f;
	
	material.specular.r = 1.0f;
	material.specular.g = 1.0f;
	material.specular.b = 1.0f;
	material.specular.a = 1.0f;

	material.emissive.r = 0.0f;
	material.emissive.g = 0.0f;
	material.emissive.b = 0.0f;
	material.emissive.a = 0.0f;
	
	material.power      = 20.0f;
	
	if( !SetMaterial(&material) )
		return E_FAIL;

	// set default matrices
	D3DXMATRIX		Matrix;
	D3DXVECTOR3		from(0.0f, 0.0f, - 2.0f*WIDTH ), at(0.0f, -0.5f, 0.0f), up(0.0f, 1.0f, 0.0f); 
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

	m_DynamicStride = sizeof D3DVERTEXTEXTURECOLOR;
	VERTEXELEMENT D3DVERTEX_Elements[] =		// dynamic vertex elements
	{
		// Stream	Offset	Type					Method					Usage					UsageIndex
		{	0,		0,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
		{	0,		12,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0},
		{	0,		24,		D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		0},
		{	0,		28,		D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0},
		D3DDECL_END()
	};
	m_VertexElements = vector<VERTEXELEMENT>( D3DVERTEX_Elements, D3DVERTEX_Elements + sizeof(D3DVERTEX_Elements) / sizeof VERTEXELEMENT );

	DWORD dwDecl[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG( D3DVSDE_POSITION,	D3DVSDT_FLOAT3 ),
		D3DVSD_REG( D3DVSDE_NORMAL,		D3DVSDT_FLOAT3 ),
		D3DVSD_REG( D3DVSDE_DIFFUSE,	D3DVSDT_D3DCOLOR ),
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
HRESULT CDynamicVBTest::InitDeviceObjects()
{
	HRESULT hResult = D3D_OK;

	if( !D3DXCreateTextureFromResourceEx(	NULL,
											"DXLOGO",
											256,
											256,
											1,
											0,
											D3DFMT_A8R8G8B8,
											D3DPOOL_DEFAULT,
											D3DX_DEFAULT,
											D3DX_DEFAULT,
											0,
											NULL,
											NULL,
											&m_pTextures
									) ||
		!SetTexture( 0, m_pTextures ) ||
		!SetSamplerState( 0, SAMP_MINFILTER, TEXF_LINEAR) ||
		!SetSamplerState( 0, SAMP_MAGFILTER, TEXF_LINEAR) )
		hResult = E_FAIL;
	return hResult;
}


//////////////////////////////////////////////////////////////////////
// Name:	RestoreDeviceObjects()
// Desc:	Restore device-memory objects and state after a device is created or resized.
//////////////////////////////////////////////////////////////////////
HRESULT CDynamicVBTest::RestoreDeviceObjects()
{
	try
	{
		if( !CreateVertexBuffer(	m_AllShapes.m_nVertices * m_DynamicStride,
									USAGE_WRITEONLY | USAGE_DYNAMIC,
									0,
									POOL_DEFAULT,
									&m_pDynamicVertexBuffers ) ||
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

		BYTE *pIndices;
		if( !m_pIndexBuffers->Lock( 0, 0, (void**)&pIndices, 0 ) ) throw E_FAIL;
		memcpy( pIndices, m_AllShapes.m_pIndices, m_AllShapes.m_nIndices * sizeof WORD );
		if( !m_pIndexBuffers->Unlock() ) throw E_FAIL;

		if( !SetIndices( m_pIndexBuffers, 0 ) ||
			!SetStreamSource( 0, m_pDynamicVertexBuffers, m_DynamicStride, 0 ) )
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
		SetRenderState( D3DRS_ZENABLE, FALSE );
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
HRESULT CDynamicVBTest::FrameMove()
{
	float fRotationAngle = (float)m_TestIndex * D3DX_PI/32.0f;

	// rotate each individual cube and re-populate the whole shapes vector
	m_AllShapes.NewShape(CS_COMPOSITE);
	for( int x = 0; x < WIDTH; x++ )
		for( int y = 0; y < HEIGHT; y++ )
		{
			m_CubeShapes[x][y].NewShape(CS_BOX);
			m_CubeShapes[x][y].Scale(1.5);
			m_CubeShapes[x][y].Translate( (float)(2*x) - (float)WIDTH + 1.0f, (float)(2*y) - (float)HEIGHT, 0.1f + 1.0f );
			m_CubeShapes[x][y].Rotate( fRotationAngle, fRotationAngle, fRotationAngle );
			m_AllShapes.AddShape( m_CubeShapes[x][y] );
		}

	try
	{
		if( !Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE( 0.8f, 0.8f, 0.8f, 1.0f ), 1.0f, 0L ) ) throw E_FAIL;

		BYTE *pVertixes;
		if( !m_pDynamicVertexBuffers->Lock( 0, 0, (void**)&pVertixes, 0 ) ) throw E_FAIL;
		for( int iVertex = 0; iVertex < m_AllShapes.m_nVertices; iVertex++ )
		{
			CopyDynamicVertex( (void*)pVertixes, m_AllShapes.m_pVertices + iVertex, D3DCOLOR_COLORVALUE( 0.2f, 0.2f, 0.2f, 0 ) );
			pVertixes += m_DynamicStride;
		}
		if( !m_pDynamicVertexBuffers->Unlock() ) throw E_FAIL;
	}
	catch( HRESULT hError )
	{
		return hError;
	}

	// now we want to alter the cubes involved in Locks
	vectorLock_Flags vectorFlags = m_vectorTests[m_TestIndex];
	for( int iLock = 0; iLock < vectorFlags.size(); iLock++ )
	{
		Lock_Flags *pLockFlags = &vectorFlags[iLock];
		int x = ( ( pLockFlags->OffsetToLock / m_DynamicStride ) / m_CubeShapes[0][0].m_nVertices ) / (WIDTH * HEIGHT);
		int y = ( ( pLockFlags->OffsetToLock / m_DynamicStride ) / m_CubeShapes[0][0].m_nVertices ) % (WIDTH * HEIGHT);
		m_CubeShapes[x][y].Scale(1.1f);
		for( int iVertex = 0; iVertex < m_CubeShapes[x][y].m_nVertices; iVertex++ )
		{
			// mirror the textures
			(m_CubeShapes[x][y].m_pVertices + iVertex)->tu = 1.0f - (m_CubeShapes[x][y].m_pVertices + iVertex)->tu;
			(m_CubeShapes[x][y].m_pVertices + iVertex)->tv = 1.0f - (m_CubeShapes[x][y].m_pVertices + iVertex)->tv;
		}
	}

	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	Render()
// Desc:	Called once per frame, the call is the entry point for 3d
//			rendering. This function sets up render states, clears the
//			viewport, and renders the scene.
//////////////////////////////////////////////////////////////////////
HRESULT CDynamicVBTest::Render()
{
	HRESULT hr = D3D_OK;
	BYTE *pVertixes = NULL;
	LPVERTEXBUFFER pVertexBuffer = NULL;
	vectorLock_Flags vectorFlags = m_vectorTests[m_TestIndex];
	D3DCOLOR vecColors[] =	{	D3DCOLOR_COLORVALUE( 1.0f, 0.0f, 1.0f, 0 ),
								D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 0.0f, 0 ),
								D3DCOLOR_COLORVALUE( 0.0f, 1.0f, 1.0f, 0 ),
								D3DCOLOR_COLORVALUE( 0.5f, 0.5f, 1.0f, 0 ),
								D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 1.0f, 0 ),
							};
	try
	{
		// 1st step: present the original vertices, the whole buffer
		if( !BeginScene() ) throw E_FAIL;
		if( !DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_AllShapes.m_nVertices, 0, m_AllShapes.m_nIndices/3, 0 ) ) throw E_FAIL;
		if( !EndScene() ) throw E_FAIL;
		DisplayFrame();

		// clear the screen if desired, to focus on the locked parts
		if( m_bClearBetweenPresent )
			if( !Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE( 0.8f, 0.8f, 0.8f, 1.0f ), 1.0f, 0L ) ) throw E_FAIL;


		// 2nd step: preliminary - update the buffers, where DISCARD or 0 is to be used ONLY (so no overwrite)
		if( !BeginScene() ) throw E_FAIL;
		int iLock;
		for(iLock = 0; iLock < vectorFlags.size(); iLock++ )
		{
			Lock_Flags *pLockFlags = &vectorFlags[iLock];

			if( 0 == pLockFlags->Flags || LOCK_DISCARD == pLockFlags->Flags )
			{
				int x = ( ( pLockFlags->OffsetToLock / m_DynamicStride ) / m_CubeShapes[0][0].m_nVertices ) / (WIDTH * HEIGHT);
				int y = ( ( pLockFlags->OffsetToLock / m_DynamicStride ) / m_CubeShapes[0][0].m_nVertices ) % (WIDTH * HEIGHT);

				if( !m_pDynamicVertexBuffers->Lock( pLockFlags->OffsetToLock, pLockFlags->SizeToLock, (void**)&pVertixes, LOCK_DISCARD ) ) throw E_FAIL;

				for( int iVertex = 0; iVertex < pLockFlags->SizeToLock / m_DynamicStride; iVertex++ )
				{
					CopyDynamicVertex( pVertixes, m_CubeShapes[x][y].m_pVertices + iVertex, vecColors[ sizeof vecColors / sizeof D3DCOLOR - 1 ] );
					pVertixes += m_DynamicStride;
				}

				UINT nVertices = pLockFlags->SizeToLock / m_DynamicStride;
				UINT nOffset = pLockFlags->OffsetToLock / m_DynamicStride;
				UINT nStartIndex =  (36 * nOffset / 24);	// there are 36 indices for 24 vertices in a cube
				if( !m_pDynamicVertexBuffers->Unlock() ) throw E_FAIL;
				if( !DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nOffset, nVertices, nStartIndex, nVertices / 2, 0 ) ) throw E_FAIL;
			}
		}
		if( !EndScene() ) throw E_FAIL;
		DisplayFrame();


		// 3rd step: Update the HAL device
		pVertexBuffer = NULL;
		m_pDynamicVertexBuffers->GetVertexBuffer( &pVertexBuffer, 0 );
		if( NULL == pVertexBuffer )
		{
			WriteToLog( _T("Framework error: couldn't get hal vertex buffer") );
			Fail();
			throw E_FAIL;
		}

		// update the hal buffer, doing "smart" locks as requested by vectorFlags, also draw the scene
		Check_HRFail( m_pSrcDevice->BeginScene(), "BeginScene" );

		for(iLock = 0; iLock < vectorFlags.size(); iLock++ )
		{
			Lock_Flags *pLockFlags = &vectorFlags[iLock];

			int x = ( ( pLockFlags->OffsetToLock / m_DynamicStride ) / m_CubeShapes[0][0].m_nVertices ) / (WIDTH * HEIGHT);
			int y = ( ( pLockFlags->OffsetToLock / m_DynamicStride ) / m_CubeShapes[0][0].m_nVertices ) % (WIDTH * HEIGHT);
			
			Check_HRFail( pVertexBuffer->Lock( pLockFlags->OffsetToLock, pLockFlags->SizeToLock, (void**)&pVertixes, pLockFlags->Flags ), "Lock HAL Buffer" );

			for( int iVertex = 0; iVertex < pLockFlags->SizeToLock / m_DynamicStride; iVertex++ )
			{
				CopyDynamicVertex( pVertixes, m_CubeShapes[x][y].m_pVertices + iVertex, vecColors[ iLock % (sizeof vecColors / sizeof D3DCOLOR) ] );
				pVertixes += m_DynamicStride;
			}
			Check_HRFail( pVertexBuffer->Unlock(), "Unlock HAL Buffer" );

			UINT nVertices = pLockFlags->SizeToLock / m_DynamicStride;
			UINT nOffset = pLockFlags->OffsetToLock / m_DynamicStride;
			UINT nStartIndex =  (36 * nOffset / 24);	// there are 36 indices for 24 vertices in a cube
			Check_HRFail( m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nOffset, nVertices, nStartIndex, nVertices / 2, 0 ), "HAL DrawIndexedPrimitive" );
		}
		Check_HRFail( m_pSrcDevice->EndScene(), "EndScene" );

		RELEASE( pVertexBuffer );


		// 4th step: on the ref device do all the locks at once
		pVertexBuffer = NULL;
		m_pDynamicVertexBuffers->GetVertexBuffer( &pVertexBuffer, 1 );
		if( NULL == pVertexBuffer )
		{
			WriteToLog( _T("Framework error: couldn't get ref vertex buffer") );
			Fail();
			throw E_FAIL;
		}

		// update the ref buffer
		Check_HRFail( pVertexBuffer->Lock( 0, 0, (void**)&pVertixes, LOCK_DISCARD ), "Lock REF Buffer" );

		for(iLock = 0; iLock < vectorFlags.size(); iLock++ )
		{
			Lock_Flags *pLockFlags = &vectorFlags[iLock];
			int x = ( ( pLockFlags->OffsetToLock / m_DynamicStride ) / m_CubeShapes[0][0].m_nVertices ) / (WIDTH * HEIGHT);
			int y = ( ( pLockFlags->OffsetToLock / m_DynamicStride ) / m_CubeShapes[0][0].m_nVertices ) % (WIDTH * HEIGHT);
			
			BYTE *pVertex = pVertixes + pLockFlags->OffsetToLock;
			for( int iVertex = 0; iVertex < pLockFlags->SizeToLock / m_DynamicStride; iVertex++ )
			{
				CopyDynamicVertex( pVertex, m_CubeShapes[x][y].m_pVertices + iVertex, 
																					(m_bForceREFFailure && LOCK_DISCARD == pLockFlags->Flags)? 
																					vecColors[ sizeof vecColors / sizeof D3DCOLOR - 1 ] :
																					vecColors[ iLock % (sizeof vecColors / sizeof D3DCOLOR) ] );
				pVertex += m_DynamicStride;
			}
		}
		Check_HRFail( pVertexBuffer->Unlock(), "Unlock REF Buffer" );
		RELEASE( pVertexBuffer );

		// update the REF scene
		Check_HRFail( m_pRefDevice->BeginScene(), "BeginScene" );
		for( iLock = 0; iLock < vectorFlags.size(); iLock++ )
		{
			Lock_Flags *pLockFlags = &vectorFlags[iLock];
			UINT nVertices = pLockFlags->SizeToLock / m_DynamicStride;
			UINT nOffset = pLockFlags->OffsetToLock / m_DynamicStride;
			UINT nStartIndex =  (36 * nOffset / 24);	// there are 36 indices for 24 vertices in a cube
			Check_HRFail( m_pRefDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nOffset, nVertices, nStartIndex, nVertices / 2, 0 ), "REF DrawIndexedPrimitive" );
		}
        Check_HRFail( m_pRefDevice->EndScene(), "EndScene" );


		// time to display the updated frame
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
HRESULT CDynamicVBTest::InvalidateDeviceObjects()
{
	RELEASE( m_pDynamicVertexBuffers );
	RELEASE( m_pIndexBuffers );
	RELEASE( m_pVertexDeclarations );

	// DX8.1 test has to clean up the fake vertex shader
	if( m_pSrcDevice->GetInterface() <= 8.1f )	
	{
		DeleteVertexShader( m_p81VertexShaders );
	}

	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	DeleteDeviceObjects()
// Desc:	Called when the app is exiting, or the device is being changed, this function deletes any device dependent objects.
//////////////////////////////////////////////////////////////////////
HRESULT CDynamicVBTest::DeleteDeviceObjects()
{
	RELEASE( m_pTextures );
	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	FinalCleanup()
// Desc:	Called before the app exits, this function gives the app the chance to cleanup after itself.
//////////////////////////////////////////////////////////////////////
HRESULT CDynamicVBTest::FinalCleanup()
{
	return D3D_OK;
}
