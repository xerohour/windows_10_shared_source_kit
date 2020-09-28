// StretchRectDepthStencil.cpp: implementation of the CStretchRectDepthStencil class.
//
//////////////////////////////////////////////////////////////////////

#include "StretchRectDepthStencil.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStretchRectDepthStencil::CStretchRectDepthStencil()
{
	m_szTestName	= _T("StretchRectDepthStencil");
	m_szCommandKey	= _T("StretchRectDepthStencil");
	SetTestType(TESTTYPE_CONF);
	
	m_Options.D3DOptions.fMinDXVersion		= 9.0f;
	m_Options.D3DOptions.bReference			= false;
	m_Options.D3DOptions.bZBuffer			= true;
	m_Options.D3DOptions.bCachedBackBuffer	= false;
	m_Options.D3DOptions.dwDevTypes			= DEVICETYPE_HAL;	// ref not supported!!! | DEVICETYPE_REF;
	m_Options.D3DOptions.dwBehaviors		= DEVICECREATE_STANDARD | DEVICECREATE_SW_OR_HW_VP;
	m_Options.ModeOptions.ZBufferFmt		= FMT_ALL;

	m_pCurrentDepthStencilFormat = NULL;
	m_nFrames	= 10;
	for( int i = 0; i <= WIDTH*HEIGHT*DEPTH; i++ )
	{
		m_pVertexBuffer[i]	= NULL;
		m_pIndexBuffer[i]	= NULL;
	}
	m_pTexture = NULL;
	m_pOriginalDepthStencil	= m_pCopyDepthStencil	= NULL;
	m_pBackBuffer	= NULL;
	memset( &m_sdBackBuffer, 0, sizeof SURFACEDESC );
	m_pSurfaceBeforeStretchRect = m_pSurfaceAfterStretchRect = NULL;

	m_bSave = KeySet(_T("Save"));
}

CStretchRectDepthStencil::~CStretchRectDepthStencil()
{
}


//////////////////////////////////////////////////////////////////////
// Name: CommandLineHelp()
// Desc: command line help for the test
//////////////////////////////////////////////////////////////////////
void CStretchRectDepthStencil::CommandLineHelp()
{
	WriteCommandLineHelp(_T("The test uses copies the depth stencil with StretchRect\n"));
	WriteCommandLineHelp(_T("and then draws the same scene using the copied buffer.\n"));
	WriteCommandLineHelp(_T("The test expects the backbuffers in both cases to be identical.\n"));
	WriteCommandLineHelp(_T("Command line parameter: -save\n"));
	WriteCommandLineHelp(_T("Test will save in the local folder the images rendered with both buffers.\n"));
}


//////////////////////////////////////////////////////////////////////
// Name: Check_HRFail( HRESULT, const char )
// Desc: if hResult is not D3D_OK, throws an exception
//////////////////////////////////////////////////////////////////////
void CStretchRectDepthStencil::Check_HRFail( HRESULT hResult, const char *sMethod )
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
// Name:	DuplicateDepthStencil()
// Desc:	duplicate the depth stencil format (the contents are NOT copied though)
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectDepthStencil::DuplicateDepthStencil()
{
	HRESULT hr = D3D_OK;
	try
	{
		// release surfaces
		RELEASE( m_pOriginalDepthStencil );
		RELEASE( m_pCopyDepthStencil );

		// get depth stencil description
		SURFACEDESC surfDepthStencilDesc;
		Check_HRFail( m_pSrcDevice->GetDepthStencilSurface( &m_pOriginalDepthStencil ), "GetDepthStencilSurface" );
		Check_HRFail( m_pOriginalDepthStencil->GetDesc( &surfDepthStencilDesc ), "GetDesc" );

		// create the new depth stencil surface
		Check_HRFail( m_pSrcDevice->CreateDepthStencilSurface(	surfDepthStencilDesc.Width,
																surfDepthStencilDesc.Height,
																surfDepthStencilDesc.Format,
																surfDepthStencilDesc.MultiSampleType,
																0,
																FALSE,
																&m_pCopyDepthStencil
									), "CreateDepthStencilSurface" 
						);

		// update the last depth stencil format
		RELEASE( m_pOriginalDepthStencil );
	}
	catch( HRESULT hError )
	{
		hr = hError;
	}
	return hr;
}


//////////////////////////////////////////////////////////////////////
// Name:	CompareSurfaces()
// Desc:	compare the two surfaces, before and after StretchRect
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectDepthStencil::CompareSurfaces()
{
	HRESULT		hr = D3D_OK;
	LOCKED_RECT	lockedRectBefore, lockedRectAfter;
	DWORD		dwBefore, dwAfter;
	BYTE        btBefore, btAfter, btError;
	bool		bSurfaceBeforeLocked = false, bSurfaceAfterLocked = false;
	LPSURFACE	pLocalSrcSurface = NULL, pLocalDestSurface = NULL;
	SURFACEDESC srcSurfDesc, destSurfDesc;
	int			x, y;

	try
	{
		// grab surface description
		Check_HRFail( m_pSurfaceBeforeStretchRect->GetDesc( &srcSurfDesc ), "GetDesc(pSrcSurface)" );
		Check_HRFail( m_pSurfaceAfterStretchRect->GetDesc( &destSurfDesc ), "GetDesc(pDestSurface)" );

		// prepare the local surface copies
		Check_HRFail( m_pSrcDevice->CreateOffscreenPlainSurface(	srcSurfDesc.Width,
																	srcSurfDesc.Height,
																	FMT_A8R8G8B8,
																	POOL_SYSTEMMEM,
																	&pLocalSrcSurface
																), "CreateOffscreenPlainSurface"
					);
		Check_HRFail( m_pSrcDevice->D3DXLoadSurfaceFromSurface(		pLocalSrcSurface,
																	NULL,
																	NULL,
																	m_pSurfaceBeforeStretchRect,
																	NULL,
																	NULL,
																	D3DX_FILTER_NONE,
																	0
															 ),	"D3DXLoadSurfaceFromSurface" 
					);

		Check_HRFail( m_pSrcDevice->CreateOffscreenPlainSurface(	destSurfDesc.Width,
																	destSurfDesc.Height,
																	FMT_A8R8G8B8,
																	POOL_SYSTEMMEM,
																	&pLocalDestSurface
																), "CreateOffscreenPlainSurface"
					);
		Check_HRFail( m_pSrcDevice->D3DXLoadSurfaceFromSurface(		pLocalDestSurface,
																	NULL,
																	NULL,
																	m_pSurfaceAfterStretchRect,
																	NULL,
																	NULL,
																	D3DX_FILTER_NONE,
																	0
															 ),	"D3DXLoadSurfaceFromSurface" 
					);

		// lock the surfaces
		Check_HRFail( pLocalSrcSurface->LockRect( &lockedRectBefore, NULL, LOCK_READONLY ), "LockRect" );
		bSurfaceBeforeLocked	= true;
		Check_HRFail( pLocalDestSurface->LockRect( &lockedRectAfter, NULL, LOCK_READONLY ), "LockRect" );
		bSurfaceAfterLocked		= true;

		// perform the comparison byte by byte (so we can store the difference between them)
		// Zero tolerance! We used no stretching/shrinking/filtering
		for( y = 0; y < m_sdBackBuffer.Height; y++ )
		{
			for( x = 0; x < m_sdBackBuffer.Width; x++ )
			{
				dwBefore = *(BYTE*)( (UINT_PTR)lockedRectBefore.pBits + y * lockedRectBefore.iPitch + x * 4 );
				dwAfter = *(BYTE*)( (UINT_PTR)lockedRectAfter.pBits + y * lockedRectAfter.iPitch + x * 4 );
				for (int i = 0; i < 4; i++)
				{
					btBefore = (BYTE)(dwBefore >> (i * 8));
					btAfter = (BYTE)(dwAfter >> (i * 8));
					btError = abs(btBefore - btAfter);

					if( btError != 0 )
					{
						WriteToLog( _T("Images differ at (%d,%d). Original Buffer: %08x, StretchRect Buffer: %08x\n"),
									x, y,
									dwBefore, dwAfter );

						if( btError > 0x10 )
						{
							break;   // failed, exit comparison
						}
					}
				}
			}
		}
		if( y < m_sdBackBuffer.Height )
		{
			WriteToLog( _T( "Mismatche(s) found. Test fails.\n" ) );
			Fail();
		}

		// unlock the surfaces
		Check_HRFail( pLocalSrcSurface->UnlockRect(), "UnlockRect" );
		bSurfaceBeforeLocked	= false;
		Check_HRFail( pLocalDestSurface->UnlockRect(), "UnlockRect" );
		bSurfaceAfterLocked		= false;
	}
	catch( HRESULT hError )
	{
		if( bSurfaceBeforeLocked )
			pLocalSrcSurface->UnlockRect();
		if( bSurfaceAfterLocked )
			pLocalDestSurface->UnlockRect();

		hr = hError;
	}

	RELEASE(pLocalSrcSurface);
	RELEASE(pLocalDestSurface);

	return hr;
}


//////////////////////////////////////////////////////////////////////
// Name:	TestInitialize()
// Desc:	Init the testcase
//////////////////////////////////////////////////////////////////////
UINT CStretchRectDepthStencil::TestInitialize()
{
	UINT uResult = CD3DTest::TestInitialize();

	if( D3DTESTINIT_RUN == uResult )
	{
		// check caps for HW support
		HRESULT hr;
		CAPS caps;
		if( FAILED( hr = m_pSrcDevice->GetDeviceCaps(&caps) ) )
		{
			WriteToLog(
					_T("Error: GetDeviceCaps failed: %s (0x%x). Some tests might show inaccurate results\n"),
					m_pD3D->HResultToString(hr),
					hr 
				);
			uResult = D3DTESTINIT_ABORT;
		}
		if( FAILED( ConfirmDevice( &caps, m_pSrcDevice->GetBehaviors(), FMT_UNKNOWN ) ) )
		{
			WriteToLog( _T("Device doesn't support depth stencil stretch rect\n") );
			uResult = D3DTESTINIT_SKIPALL;
		}

		m_pCurrentDepthStencilFormat = GetCurrentZBufferFormat();

		if( FAILED( OneTimeSceneInit() ) )
			uResult = D3DTESTINIT_ABORT;

		// for now, call these restore methods it here
		if( FAILED( InitDeviceObjects() ) || 
			FAILED( RestoreDeviceObjects() ) )
			uResult = D3DTESTINIT_ABORT;

		Check_HRFail( DuplicateDepthStencil(), "DuplicateDepthStencil" );

		SetTestRange( 1, m_nFrames );
	}

	return uResult;
}


//////////////////////////////////////////////////////////////////////
// Name:	TestTerminate()
// Desc:	cleanup
//////////////////////////////////////////////////////////////////////
bool CStretchRectDepthStencil::TestTerminate()
{
	// for now, call the cleanup methods here
	InvalidateDeviceObjects();
	DeleteDeviceObjects();

	FinalCleanup();
	return true;
}


//////////////////////////////////////////////////////////////////////
// Name:	ExecuteTest( UINT )
// Desc:	Execute the testcase
//////////////////////////////////////////////////////////////////////
bool CStretchRectDepthStencil::ExecuteTest(UINT nTest)
{
	try
	{
		BeginTestCase( "TestCase", nTest );

		// advance the frame and render
		Check_HRFail( FrameMove(), "FrameMove" );
		Check_HRFail( Render(), "Render" );
	}
	catch(HRESULT)
	{
		Fail();
	}
	return false;
}


//////////////////////////////////////////////////////////////////////
// Name:	OnDeviceLost()
// Desc:	Called after DEVICELOST.  Test should release all D3D objects created in POOL_DEFAULT
//////////////////////////////////////////////////////////////////////
void CStretchRectDepthStencil::OnDeviceLost()
{
	DeleteDeviceObjects();
}


//////////////////////////////////////////////////////////////////////
// Name:	OnReset();
// Desc:	Called after Reset() succeeds.  Test should re-create all D3D objects released in OnDeviceLost and refresh all data within existing objects.
//////////////////////////////////////////////////////////////////////
void CStretchRectDepthStencil::OnReset()
{
	RestoreDeviceObjects();
}

	
//////////////////////////////////////////////////////////////////////
// Name:	ConfirmDevice( CAPS*, DWORD, FORMAT )
// Desc:	Called during device initialization, this code checks the device for some minimum set of capabilities
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectDepthStencil::ConfirmDevice( CAPS* pCaps, DWORD dwBehavior, FORMAT Format )
{
	return (pCaps->DevCaps2 & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES)? D3D_OK:D3DERR_NOTAVAILABLE;
}


//////////////////////////////////////////////////////////////////////
// Name:	OneTimeSceneInit()
// Desc:	Called during initial app startup, this function performs all the permanent initialization
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectDepthStencil::OneTimeSceneInit()
{
	int x, i = 0;
	for(x = 0; x < WIDTH; x++ )
		for( int y = 0; y < HEIGHT; y++ )
			for( int z = 0; z < DEPTH; z++)
			{
				m_shapes[i].NewShape(CS_BOX);
				m_shapes[i].Scale(2);
				m_shapes[i].Translate( 2.0f * x - WIDTH + 1.0f, 2.0f * y - HEIGHT, 2.0f * z + 0.1f + 1.0f );
				i++;
			}
	m_shapes[i].NewShape(CS_SPHERE);
	m_shapes[i].Scale(4);
	
	HRESULT hr = D3D_OK;
	// set default render states
	m_pSrcDevice->SetRenderState(D3DRS_AMBIENT, RGBA_MAKE(128, 192, 255, 255) );
	m_pSrcDevice->SetRenderState( D3DRS_STENCILREF,	0x1 );
	m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_pSrcDevice->SetRenderState( D3DRS_STENCILENABLE, m_pCurrentDepthStencilFormat->ddpfPixelFormat.dwStencilBitDepth > 0 );
	
	// set default lights
	LIGHT light;
	D3DVECTOR	vLightPos;
	D3DVECTOR	vLightDir;
	
	vLightPos.x = 0.f; vLightPos.y = 2.f; vLightPos.z = -2.f;
	vLightDir.x = 0.f; vLightDir.y = -0.707f; vLightDir.z = 0.707f;

	ZeroMemory(&light,sizeof(LIGHT));
	light.dltType		= D3DLIGHT_SPOT;
	light.Diffuse.r		= 0.9f;
	light.Diffuse.g		= 0.7f;
	light.Diffuse.b		= 0.5f;
	light.Diffuse.a		= 1.0f;
	light.Specular.r	= 0.3f;
	light.Specular.g	= 0.5f;
	light.Specular.b	= 0.9f;
	light.Specular.a	= 1.0f;
	light.Position		= vLightPos;
	light.Direction		= vLightDir;
	light.Range			= 1000.f;
	light.Attenuation0	= 0.7f;
	light.Attenuation1	= 0.0f;
	light.Attenuation2	= 0.0f;
	light.Theta			= pi/10;
	light.Phi			= pi/2;

	if( FAILED(hr = m_pSrcDevice->SetLight(0,&light)) )
		return hr;
	if( FAILED( hr = m_pSrcDevice->LightEnable(0,TRUE)) )
		return hr;
	
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
	
	if( FAILED(hr = m_pSrcDevice->SetMaterial(&material)) )
		return hr;

	// set default matrices
	D3DXMATRIX		Matrix;
	D3DXVECTOR3		from(0.0f, 3.0f, -2.0f -1.5f*WIDTH), at(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f); 
	float			fAspect = (float)m_pCurrentMode->nBackBufferWidth/(float)m_pCurrentMode->nBackBufferHeight;
	float			fZn = 0.09f;
	float			fZf = DEPTH*2 + 0.1f;

	// Projection matrix
	D3DXMatrixPerspectiveFovLH(&Matrix, D3DX_PI/3, fAspect, fZn, fZf); 
	if( FAILED( hr = m_pSrcDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&Matrix)) )
		return hr;

	// World matrix
	D3DXMatrixIdentity(&Matrix);
	if( FAILED( hr = m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix)) )
		return hr;

	// View matrix
	D3DXMatrixLookAtLH(&Matrix, &from, &at, &up);
	if( FAILED( hr = m_pSrcDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&Matrix)) )
		return hr;

	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDeviceObjects()
// Desc:	Initialize device-dependent objects. This is the place to create mesh and texture objects.
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectDepthStencil::InitDeviceObjects()
{
	HRESULT hr = D3D_OK;

	try
	{
		Check_HRFail( m_pSrcDevice->D3DXCreateTextureFromResourceEx(
																NULL,
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
																&m_pTexture
									), "D3DXCreateTextureFromFileEx"
						);

		// create the system surfaces to hold before/after images of the backbuffer
		Check_HRFail( m_pSrcDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer ), "GetBackBuffer" );
		Check_HRFail( m_pBackBuffer->GetDesc( &m_sdBackBuffer ), "GetDesc" );
		RELEASE( m_pBackBuffer );
		Check_HRFail( m_pSrcDevice->CreateOffscreenPlainSurface(	m_sdBackBuffer.Width,
																	m_sdBackBuffer.Height,
																	m_sdBackBuffer.Format,
																	POOL_SYSTEMMEM,
																	&m_pSurfaceBeforeStretchRect
										), "CreateOffscreenPlainSurface"
						);
		Check_HRFail( m_pSrcDevice->CreateOffscreenPlainSurface(	m_sdBackBuffer.Width,
																	m_sdBackBuffer.Height,
																	m_sdBackBuffer.Format,
																	POOL_SYSTEMMEM,
																	&m_pSurfaceAfterStretchRect
										), "CreateOffscreenPlainSurface"
						);
	}
	catch( HRESULT hError )
	{
		hr = hError;
	}
			
	return hr;
}


//////////////////////////////////////////////////////////////////////
// Name:	RestoreDeviceObjects()
// Desc:	Restore device-memory objects and state after a device is created or resized.
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectDepthStencil::RestoreDeviceObjects()
{
	try
	{
		for( int i = 0; i <= WIDTH*HEIGHT*DEPTH; i++ )
		{
			Check_HRFail( m_pSrcDevice->CreateVertexBuffer(	m_shapes[i].m_nVertices * sizeof D3DVERTEX,
															USAGE_DYNAMIC | USAGE_WRITEONLY,
															D3DVERTEXHANDLE,
															POOL_DEFAULT,
															&m_pVertexBuffer[i] ), 
							"CreateVertexBuffer" );
			Check_HRFail( m_pSrcDevice->CreateIndexBuffer(	m_shapes[i].m_nIndices * sizeof WORD,
															USAGE_WRITEONLY,
															FMT_INDEX16,
															POOL_DEFAULT,
															&m_pIndexBuffer[i] ), 
							"CreateIndexBuffer" );

			// fill the index buffer
			BYTE *pIndices;
			Check_HRFail( m_pIndexBuffer[i]->Lock( 0, 0, (void**)&pIndices, 0 ), "Lock" );
			memcpy( pIndices, m_shapes[i].m_pIndices, m_shapes[i].m_nIndices * sizeof WORD );
			Check_HRFail( m_pIndexBuffer[i]->Unlock(), "Unlock" );
		}
		Check_HRFail( m_pSrcDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer ), "GetBackBuffer" );
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
HRESULT CStretchRectDepthStencil::FrameMove()
{
	for( int i = 0; i <= WIDTH*HEIGHT*DEPTH; i++ )
	{
		m_shapes[i].Rotate( D3DX_PI/32.0f, D3DX_PI/32.0f, D3DX_PI/32.0f );

		// fill the vertex buffer
		BYTE *pVertixes;
		Check_HRFail( m_pVertexBuffer[i]->Lock( 0, 0, (void**)&pVertixes, D3DLOCK_DISCARD ), "Lock" );
		memcpy( pVertixes, m_shapes[i].m_pVertices, m_shapes[i].m_nVertices * sizeof D3DVERTEX );
		Check_HRFail( m_pVertexBuffer[i]->Unlock(), "Unlock" );			
	}

	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	RenderScene( bool )
// Desc:	render the main scene
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectDepthStencil::RenderScene( bool bStencilRendering )
{
	// set texture and fvf
	Check_HRFail( m_pSrcDevice->SetTexture( 0, m_pTexture ), "SetTexture" );
	Check_HRFail( m_pSrcDevice->SetFVF( D3DVERTEXHANDLE ), "SetFVF" );
	
	if( bStencilRendering )		// draw the sphere in the depth stencil
	{
		Check_HRFail( m_pSrcDevice->SetStreamSource( 0, m_pVertexBuffer[WIDTH*HEIGHT*DEPTH], sizeof D3DVERTEX, 0 ), "SetStreamSource" );
		Check_HRFail( m_pSrcDevice->SetIndices( m_pIndexBuffer[WIDTH*HEIGHT*DEPTH], 0 ), "SetIndices" );
		Check_HRFail( m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_shapes[WIDTH*HEIGHT*DEPTH].m_nVertices, 0, m_shapes[WIDTH*HEIGHT*DEPTH].m_nIndices/3, 0 ), "DrawIndexedPrimitive" );
	}
	else	// draw all the objects (not the sphere)
		for( int i = 0; i < WIDTH*HEIGHT*DEPTH; i++ )
		{
			// set stream source and indices
			Check_HRFail( m_pSrcDevice->SetStreamSource( 0, m_pVertexBuffer[i], sizeof D3DVERTEX, 0 ), "SetStreamSource" );		// got to keep parameter order reversed because of framework design!
			Check_HRFail( m_pSrcDevice->SetIndices( m_pIndexBuffer[i], 0 ), "SetIndices" );

			// draw the scene
			Check_HRFail( m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_shapes[i].m_nVertices, 0, m_shapes[i].m_nIndices/3, 0 ), "DrawIndexedPrimitive" );
		}

	// clear texture state
	Check_HRFail( m_pSrcDevice->SetTexture( 0, NULL ), "SetTexture" );
		
	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	Render()
// Desc:	Called once per frame, the call is the entry point for 3d
//			rendering. This function sets up render states, clears the
//			viewport, and renders the scene.
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectDepthStencil::Render()
{
	HRESULT hr = D3D_OK;
	try
	{
		// clear flags
		DWORD dwFlags = D3DCLEAR_TARGET;
		if( m_pCurrentDepthStencilFormat->ddpfPixelFormat.dwStencilBitDepth > 0 )
			dwFlags |= D3DCLEAR_STENCIL;
		if( m_pCurrentDepthStencilFormat->ddpfPixelFormat.dwZBufferBitDepth - m_pCurrentDepthStencilFormat->ddpfPixelFormat.dwStencilBitDepth > 0 )
			dwFlags |= D3DCLEAR_ZBUFFER;

		// 1st phase of drawing: create depthstencil content

		// clear and begin the scene
		Check_HRFail( m_pSrcDevice->Clear( 0L, NULL, dwFlags, RGBA_MAKE(0xB0, 0x60, 0xC0, 0x00), 1.0f, 0L ), "Clear" );
		Check_HRFail( m_pSrcDevice->BeginScene(), "BeginScene" );

		if( m_pCurrentDepthStencilFormat->ddpfPixelFormat.dwStencilBitDepth > 0 )
		{
			// store the ref value where the sphere is drawn
			m_pSrcDevice->SetRenderState( D3DRS_STENCILFUNC,	D3DCMP_ALWAYS );
			m_pSrcDevice->SetRenderState( D3DRS_STENCILPASS,	D3DSTENCILOP_REPLACE );

			// prevent the zbuffer/render target from being rendered
			m_pSrcDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
			m_pSrcDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );

			// render to the depth stencil
			RenderScene(true);

			// now accept less
			m_pSrcDevice->SetRenderState( D3DRS_STENCILFUNC,	D3DCMP_EQUAL );
			m_pSrcDevice->SetRenderState( D3DRS_STENCILPASS,	D3DSTENCILOP_KEEP );
			m_pSrcDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );
			m_pSrcDevice->SetRenderState( D3DRS_SRCBLEND,		D3DBLEND_DESTCOLOR );
		}

		// render the scene
		Check_HRFail( RenderScene(false), "RenderScene" );

		// End the scene
        Check_HRFail( m_pSrcDevice->EndScene(), "EndScene" );

		// grab the backbuffer
		Check_HRFail( m_pSrcDevice->D3DXLoadSurfaceFromSurface(	
														m_pSurfaceBeforeStretchRect,
														NULL,
														NULL,
														m_pBackBuffer,
														NULL,
														NULL,
														D3DX_FILTER_NONE,
														0 ),
						"D3DXLoadSurfaceFromSurface" );
		if( m_bSave )
			m_pSrcDevice->D3DXSaveSurfaceToFile( "dsOriginalBackBuffer.dds", D3DXIFF_DDS, m_pSurfaceBeforeStretchRect, NULL, NULL );

		// present the scene
		Check_HRFail( m_pSrcDevice->Present( NULL, NULL, NULL, NULL ), "Present" );

		// save a copy of the depth stencil
		Check_HRFail( m_pSrcDevice->GetDepthStencilSurface( &m_pOriginalDepthStencil ), "GetDepthStencilSurface" );
		Check_HRFail( m_pSrcDevice->StretchRect(	m_pOriginalDepthStencil,
													NULL,
													m_pCopyDepthStencil,
													NULL,
													D3DTEXF_NONE 
											), "StretchRect"
						);


		// 2nd faze of drawing: clear everything and render without writing to the depthstencil buffers using the copied buffer;
		Check_HRFail( m_pSrcDevice->Clear( 0L, NULL, dwFlags, RGBA_MAKE(0xB0, 0x60, 0xC0, 0x00), 1.0f, 0L ), "Clear" );
		Check_HRFail( m_pSrcDevice->BeginScene(), "BeginScene" );

		// set the freshly copied depthstencil buffer; don't write to it
		Check_HRFail( m_pSrcDevice->SetDepthStencilSurface( m_pCopyDepthStencil ), "SetDepthStencilSurface" );
		Check_HRFail( m_pSrcDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE ), "SetRenderState(D3DRS_ZWRITEENABLE)" );

		Check_HRFail( RenderScene(false), "RenderScene" );

		// restore the original depth stencil
		Check_HRFail( m_pSrcDevice->SetDepthStencilSurface( m_pOriginalDepthStencil ), "SetDepthStencilSurface" );
		Check_HRFail( m_pSrcDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE ), "SetRenderState(D3DRS_ZWRITEENABLE)" );

		// End the scene
        Check_HRFail( m_pSrcDevice->EndScene(), "EndScene" );

		// grab the backbuffer
		Check_HRFail( m_pSrcDevice->D3DXLoadSurfaceFromSurface(	
														m_pSurfaceAfterStretchRect,
														NULL,
														NULL,
														m_pBackBuffer,
														NULL,
														NULL,
														D3DX_FILTER_NONE,
														0 ),
						"D3DXLoadSurfaceFromSurface" );
		if( m_bSave )
			m_pSrcDevice->D3DXSaveSurfaceToFile( "dsStretchRectBackBuffer.dds", D3DXIFF_DDS, m_pSurfaceAfterStretchRect, NULL, NULL );

		// present the scene
		Check_HRFail( m_pSrcDevice->Present( NULL, NULL, NULL, NULL ), "Present" );

		Check_HRFail( CompareSurfaces(), "CompareSurfaces" );
    }
	catch( HRESULT hError )
	{
		m_pSrcDevice->EndScene();
		hr = hError;
	}

	RELEASE( m_pOriginalDepthStencil );
	return hr;
}


//////////////////////////////////////////////////////////////////////
// Name:	InvalidateDeviceObjects()
// Desc:	Called when the device-dependent objects are about to be lost.
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectDepthStencil::InvalidateDeviceObjects()
{
	for( int i = 0; i <= WIDTH*HEIGHT*DEPTH; i++ )
	{
		RELEASE( m_pVertexBuffer[i] );
		RELEASE( m_pIndexBuffer[i] );
	}
	RELEASE( m_pOriginalDepthStencil );
	RELEASE( m_pCopyDepthStencil );

	RELEASE( m_pBackBuffer );

	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	DeleteDeviceObjects()
// Desc:	Called when the app is exiting, or the device is being changed, this function deletes any device dependent objects.
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectDepthStencil::DeleteDeviceObjects()
{
	RELEASE( m_pTexture );
	RELEASE( m_pSurfaceBeforeStretchRect );
	RELEASE( m_pSurfaceAfterStretchRect );
	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	FinalCleanup()
// Desc:	Called before the app exits, this function gives the app the chance to cleanup after itself.
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectDepthStencil::FinalCleanup()
{
	return D3D_OK;
}

