// StretchRectKeepStates.cpp: implementation of the CStretchRectKeepStates class.
//
//////////////////////////////////////////////////////////////////////

#include "StretchRectKeepStates.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStretchRectKeepStates::CStretchRectKeepStates()
{
	m_szTestName	= _T("StretchRectKeepStates");
	m_szCommandKey	= _T("StretchRectKeepStates");
	SetTestType(TESTTYPE_CONF);
	
	m_Options.D3DOptions.fMinDXVersion		= 9.0f;
	m_Options.D3DOptions.bReference			= false;
	m_Options.D3DOptions.bZBuffer			= true;
	m_Options.D3DOptions.bCachedBackBuffer	= false;
	m_Options.D3DOptions.dwDevTypes			= DEVICETYPE_HAL;	// ref not supported!!! | DEVICETYPE_REF;
	m_Options.D3DOptions.dwBehaviors		= DEVICECREATE_STANDARD | DEVICECREATE_SW_OR_HW_VP;

	m_bSave = KeySet(_T("Save"));

	for( int i = 0; i < WIDTH*HEIGHT*DEPTH; i++ )
	{
		m_pVertexBuffer[i]	= NULL;
		m_pIndexBuffer[i]	= NULL;
	}
	m_pTexture = NULL;
	memset( &m_sdBackBuffer, 0, sizeof SURFACEDESC );
	m_pBackBuffer = m_pBackBufferSurfaceWithoutSR = m_pBackBufferSurfaceWithSR= NULL;
	m_pSrcTexture = NULL;
	m_pSrcSurface = m_pDestSurfaceWithoutRS = m_pDestSurfaceWithRS = NULL;

	m_d3dRenderState = D3DRS_FORCE_DWORD;
}

CStretchRectKeepStates::~CStretchRectKeepStates()
{
}


//////////////////////////////////////////////////////////////////////
// Name: Check_HRFail( HRESULT, const char )
// Desc: if hResult is not D3D_OK, throws an exception
//////////////////////////////////////////////////////////////////////
void CStretchRectKeepStates::Check_HRFail( HRESULT hResult, const char *sMethod )
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
// Name:	Dump()
// Desc:	dump the input parameters
//////////////////////////////////////////////////////////////////////
void CStretchRectKeepStates::Dump()
{
	WriteToLog( _T( "Src Format = %d\nDest Format = %d\nSrc Rect = (%d,%d,%d,%d)\nDest Rect = (%d,%d,%d,%d)\nFilter = %d\nRender State = %d\n" ),
				m_vecFormats[m_nIndexFormat].formatSrc.d3dfFormat,
				m_vecFormats[m_nIndexFormat].formatDest.d3dfFormat,
				m_vecSrcRects[m_nIndexSrcRect].left, m_vecSrcRects[m_nIndexSrcRect].top, m_vecSrcRects[m_nIndexSrcRect].right, m_vecSrcRects[m_nIndexSrcRect].bottom,
				m_vecDestRects[m_nIndexDestRect].left, m_vecDestRects[m_nIndexDestRect].top, m_vecDestRects[m_nIndexDestRect].right, m_vecDestRects[m_nIndexDestRect].bottom,
				m_vecFilters[m_nIndexFilter],
				m_vecd3dRenderStateValues[m_nIndexd3dRenderStateValue]
			);
}


//////////////////////////////////////////////////////////////////////
// Name:	DecodeTestNumber( UINT )
// Desc:	decode test number
//////////////////////////////////////////////////////////////////////
void CStretchRectKeepStates::DecodeTestNumber( UINT nTest )
{
	int nAllTests = m_vecFormats.size() * m_vecSrcRects.size() * m_vecDestRects.size() * m_vecFilters.size() * m_vecd3dRenderStateValues.size();

	// compute indexes into vectors from nTest
	UINT nIndex = nAllTests / m_vecFormats.size();
	m_nIndexFormat = nTest / nIndex;				// index in the SrcDestFormats vector

	nTest = nTest % nIndex;
	nIndex /= m_vecSrcRects.size();
	m_nIndexSrcRect = nTest / nIndex;				// index in the SrcRects vector

	nTest = nTest % nIndex;
	nIndex /= m_vecDestRects.size();
	m_nIndexDestRect = nTest / nIndex;				// index in the DestRects vector

	nTest = nTest % nIndex;
	nIndex /= m_vecFilters.size();
	m_nIndexFilter = nTest / nIndex;				// index in the Filters vector

	nTest = nTest % nIndex;
	nIndex /= m_vecd3dRenderStateValues.size();
	m_nIndexd3dRenderStateValue = nTest / nIndex;	// index in the RenderStateValues vector
}


//////////////////////////////////////////////////////////////////////
// Name:	CompareSurfaces()
// Desc:	compare the two surfaces, before and after StretchRect
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectKeepStates::CompareSurfaces( LPSURFACE pSrcSurface, LPSURFACE pDestSurface, RECT *pRect )
{
	HRESULT		hr = D3D_OK;
	RECT		localRect;
	LOCKED_RECT	lockedSrcRect, lockedDestRect;
	DWORD		dwSrc, dwDest;
	BYTE        btSrc, btDest, btError;
	bool		bSrcSurfaceLocked = false, bDestSurfaceLocked = false;
	LPSURFACE	pLocalSrcSurface = NULL, pLocalDestSurface = NULL;
	SURFACEDESC srcSurfDesc, destSurfDesc;
	int			x, y;

	try
	{
		// grab surface description
		Check_HRFail( pSrcSurface->GetDesc( &srcSurfDesc ), "GetDesc(pSrcSurface)" );
		Check_HRFail( pDestSurface->GetDesc( &destSurfDesc ), "GetDesc(pDestSurface)" );

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
																	pSrcSurface,
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
																	pDestSurface,
																	NULL,
																	NULL,
																	D3DX_FILTER_NONE,
																	0
															 ),	"D3DXLoadSurfaceFromSurface" 
					);

		// prepare the local rect variable
		if( NULL == pRect )
		{
			localRect.left		= localRect.top = 0;
			localRect.right		= srcSurfDesc.Width;
			localRect.bottom	= srcSurfDesc.Height;
		}
		else
			memcpy( &localRect, pRect, sizeof RECT );

		// lock the surfaces
		Check_HRFail( pLocalSrcSurface->LockRect( &lockedSrcRect, pRect, LOCK_READONLY ), "LockRect" );
		bSrcSurfaceLocked		= true;
		Check_HRFail( pLocalDestSurface->LockRect( &lockedDestRect, pRect, LOCK_READONLY ), "LockRect" );
		bDestSurfaceLocked		= true;

		// perform the comparison byte by byte (so we can store the difference between them)
		// Zero tolerance! Images should be identical
		for( y = localRect.top; y < localRect.bottom; y++ )
		{
			for( x = localRect.left; x < localRect.right; x++ )
			{
				dwSrc = *(DWORD*)( (UINT_PTR)lockedSrcRect.pBits + (y - localRect.top) * lockedSrcRect.iPitch + (x - localRect.left) * 4 );
				dwDest = *(DWORD*)( (UINT_PTR)lockedDestRect.pBits + (y - localRect.top) * lockedDestRect.iPitch + (x - localRect.left) * 4 );
				for (int i = 0; i < 4; i++)
				{
					btSrc = (BYTE)(dwSrc >> (i * 8));
					btDest = (BYTE)(dwDest >> (i * 8));
					btError = abs(btSrc - btDest);
					if( btError != 0 )
					{
						WriteToLog( _T("Images differ at (%d,%d). Src: %08x, Dest: %08x\n"),
									x, y,
									dwSrc, dwDest);

						if( btError > 0x10 )
						{
							break;   // failed, exit comparison
						}
					}
				}
			}
		}
		if( y < localRect.bottom )
		{
			WriteToLog( _T( "mismatches have been found. Test fails.\n" ) );
			Fail();
			Dump();
		}

		// unlock the surfaces
		Check_HRFail( pLocalSrcSurface->UnlockRect(), "UnlockRect" );
		bSrcSurfaceLocked	= false;
		Check_HRFail( pLocalDestSurface->UnlockRect(), "UnlockRect" );
		bDestSurfaceLocked		= false;
	}
	catch( HRESULT hError )
	{
		if( bSrcSurfaceLocked )
			pLocalSrcSurface->UnlockRect();
		if( bDestSurfaceLocked )
			pLocalDestSurface->UnlockRect();

		hr = hError;
	}

	// free surfaces if they were created locally
	RELEASE(pLocalSrcSurface);
	RELEASE(pLocalDestSurface);

	return hr;
}


//////////////////////////////////////////////////////////////////////
// Name:	CapsCheck()
// Desc:	Check caps
//////////////////////////////////////////////////////////////////////
bool CStretchRectKeepStates::CapsCheck()
{
	HRESULT hr;
	if( FAILED( hr = m_pSrcDevice->GetDeviceCaps(&m_DeviceCaps) ) )
	{
		WriteToLog(
				_T("Error: GetDeviceCaps failed: %s (0x%x). Some tests might show inaccurate results\n"),
				m_pD3D->HResultToString(hr),
				hr 
			);
		return false;
	}
	if( FAILED( ConfirmDevice( &m_DeviceCaps, m_pSrcDevice->GetBehaviors(), FMT_UNKNOWN ) ) )
	{
		WriteToLog( _T("Confirm Device fails, skipping test\n") );
		return false;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////
// Name:	TestInitialize()
// Desc:	Init the testcase
//////////////////////////////////////////////////////////////////////
UINT CStretchRectKeepStates::TestInitialize()
{
	UINT uResult = CD3DTest::TestInitialize();

	if( D3DTESTINIT_RUN == uResult )
	{	
		if( FAILED( OneTimeSceneInit() ) )
			uResult = D3DTESTINIT_ABORT;

#if FAST_TEST
        // subset of available formats
        FMT Formats[] =
        {
            FMT_A8R8G8B8
        };
#else //FAST_TEST
		// subset of available formats
		FMT Formats[] =
		{
			FMT_X1R5G5B5,
			FMT_A1R5G5B5,
			FMT_R5G6B5,
			FMT_X8R8G8B8,
			FMT_A8R8G8B8,
			FMT_A2R10G10B10
		};
#endif //FAST_TEST

		m_vecFormats.clear();
		for( int iSrc = 0; iSrc < sizeof(Formats)/sizeof FMT; iSrc++ )
			for( int iDest = 0; iDest < sizeof(Formats)/sizeof FMT; iDest++ )
			{
				// is the format supported???
				if( SUCCEEDED( m_pD3D->CheckDeviceFormat(	m_pSrcDevice->GetAdapterID(), 
															m_pSrcDevice->GetDevType(),
															m_DisplayMode.Format,
															USAGE_RENDERTARGET,
															RTYPE_TEXTURE,
															Formats[iSrc]
													)
								) &&
					SUCCEEDED( m_pD3D->CheckDeviceFormat(	m_pSrcDevice->GetAdapterID(), 
															m_pSrcDevice->GetDevType(),
															m_DisplayMode.Format,
															USAGE_RENDERTARGET,
															RTYPE_TEXTURE,
															Formats[iDest]
													)
								) &&
					SUCCEEDED( m_pD3D->CheckDeviceFormatConversion(	m_pSrcDevice->GetAdapterID(), 
																	m_pSrcDevice->GetDevType(),
																	Formats[iSrc],
																	Formats[iDest]
														)
							)
					)
				{
					// REF doesn't support 2:10:10:10
					if( DEVICETYPE_REF == m_pSrcDevice->GetDevType() && ( FMT_A2R10G10B10 == Formats[iSrc] || FMT_A2R10G10B10 == Formats[iDest] ) )
						continue;

					SRCDEST_FORMATS formatSrcDest;
					formatSrcDest.formatSrc = Formats[iSrc];
					formatSrcDest.formatDest = Formats[iDest];

					m_vecFormats.push_back( formatSrcDest );
				}
			}

#if FAST_TEST
        RECT rects[] = {
            {0, 0, 64, 64},
            {16, 16, 48, 48},
        };
#else //FAST_TEST
		RECT rects[] = {
			{0, 0, 64, 64},
			{16, 16, 48, 48},
			{0, 0, 48, 48},
			{5, 7, 25, 33}
		};
#endif
		m_vecSrcRects.clear();
		m_vecDestRects.clear();
		for( int iRect = 0; iRect < sizeof(rects) / sizeof RECT; iRect++ )
		{
			m_vecSrcRects.push_back( rects[iRect] );
			m_vecDestRects.push_back( rects[iRect] );
		}

		m_vecFilters.clear();
		m_vecFilters.push_back( D3DTEXF_NONE );
		if( (m_DeviceCaps.StretchRectFilterCaps & D3DPTFILTERCAPS_MINFPOINT) && (m_DeviceCaps.StretchRectFilterCaps & D3DPTFILTERCAPS_MAGFPOINT) )
			m_vecFilters.push_back( D3DTEXF_POINT );
		if( (m_DeviceCaps.StretchRectFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) && (m_DeviceCaps.StretchRectFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR) )
			m_vecFilters.push_back( D3DTEXF_LINEAR );

		m_vecd3dRenderStateValues.clear();
		InitializeRenderState();

        SetTestRange( 1, m_vecFormats.size() * m_vecSrcRects.size() * m_vecDestRects.size() * m_vecFilters.size() * m_vecd3dRenderStateValues.size() );

		// for now, call these restore methods it here
		if( FAILED( InitDeviceObjects() ) || 
			FAILED( RestoreDeviceObjects() ) )
			uResult = D3DTESTINIT_ABORT;
	}

	return uResult;
}


//////////////////////////////////////////////////////////////////////
// Name:	TestTerminate()
// Desc:	cleanup
//////////////////////////////////////////////////////////////////////
bool CStretchRectKeepStates::TestTerminate()
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
bool CStretchRectKeepStates::ExecuteTest(UINT nTest)
{
	try
	{
		DecodeTestNumber( nTest - 1 );
		BeginTestCase( "TestCase", nTest );

		// skip the test if the surface formats are not supported or if the conversion is not supported
		if( true 
			/*SUCCEEDED( m_pD3D->CheckDeviceFormat(	m_pSrcDevice->GetAdapterID(), 
													m_pSrcDevice->GetDevType(),
													m_DisplayMode.Format,
													USAGE_RENDERTARGET,
													RTYPE_TEXTURE,
													m_vecSrcFormats[m_nIndexSrcFormat]
												)
					) &&
			SUCCEEDED( m_pD3D->CheckDeviceFormat(	m_pSrcDevice->GetAdapterID(), 
													m_pSrcDevice->GetDevType(),
													m_DisplayMode.Format,
													USAGE_RENDERTARGET,
													RTYPE_SURFACE,
													m_vecDestFormats[m_nIndexDestFormat]
												)
					) &&
			SUCCEEDED( m_pD3D->CheckDeviceFormatConversion(	m_pSrcDevice->GetAdapterID(), 
															m_pSrcDevice->GetDevType(),
															m_vecSrcFormats[m_nIndexSrcFormat],
															m_vecDestFormats[m_nIndexDestFormat]
														)
					)*/
			)
		{
			// create Src and Dest surfaces for StretchRect
			Check_HRFail( m_pSrcDevice->D3DXCreateTextureFromResourceEx(
																NULL,
																"LAKEDDS",
																64,
																64,
																1,
																D3DUSAGE_RENDERTARGET,
																m_vecFormats[m_nIndexFormat].formatSrc.d3dfFormat,
																D3DPOOL_DEFAULT,
																D3DX_FILTER_TRIANGLE,
																D3DX_DEFAULT,
																0,
																NULL,
																NULL,
																&m_pSrcTexture
									), "D3DXCreateTextureFromResourceEx"
						);
			Check_HRFail( m_pSrcDevice->CreateRenderTarget(		64,
																64,
																m_vecFormats[m_nIndexFormat].formatDest,
																MULTISAMPLE_NONE,
																0,
																TRUE,
																&m_pDestSurfaceWithoutRS
													), "CreateRenderTarget"
						);
			Check_HRFail( m_pSrcDevice->CreateRenderTarget(		64,
																64,
																m_vecFormats[m_nIndexFormat].formatDest,
																MULTISAMPLE_NONE,
																0,
																TRUE,
																&m_pDestSurfaceWithRS
													), "CreateRenderTarget"
						);
			// let's clear the surfaces, don't fail if we can't (some formats unsupported on REF)
			m_pSrcDevice->ColorFill(	m_pDestSurfaceWithoutRS,
													NULL,
													(D3DCOLOR)0x00000000
									);
			m_pSrcDevice->ColorFill(	m_pDestSurfaceWithRS,
													NULL,
													(D3DCOLOR)0x00000000
									);

			Check_HRFail( m_pSrcTexture->GetSurfaceLevel( 0, &m_pSrcSurface ), "GetSurfaceLevel" );
			Check_HRFail( FrameMove(), "FrameMove" );
			Check_HRFail( Render(), "Render" );
		}
		else
			Skip();
	}
	catch(HRESULT)
	{
		Fail();
	}

	RELEASE(m_pSrcSurface);
	RELEASE(m_pDestSurfaceWithoutRS);
	RELEASE(m_pDestSurfaceWithRS);
	RELEASE(m_pSrcTexture);

	// clear the last error
	GetLastError();
	return false;
}


//////////////////////////////////////////////////////////////////////
// Name:	OnDeviceLost()
// Desc:	Called after DEVICELOST.  Test should release all D3D objects created in POOL_DEFAULT
//////////////////////////////////////////////////////////////////////
void CStretchRectKeepStates::OnDeviceLost()
{
	DeleteDeviceObjects();
}


//////////////////////////////////////////////////////////////////////
// Name:	OnReset();
// Desc:	Called after Reset() succeeds.  Test should re-create all D3D objects released in OnDeviceLost and refresh all data within existing objects.
//////////////////////////////////////////////////////////////////////
void CStretchRectKeepStates::OnReset()
{
	RestoreDeviceObjects();
}

	
//////////////////////////////////////////////////////////////////////
// Name:	ConfirmDevice( CAPS*, DWORD, FORMAT )
// Desc:	Called during device initialization, this code checks the device for some minimum set of capabilities
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectKeepStates::ConfirmDevice( CAPS* pCaps, DWORD dwBehavior, FORMAT Format )
{
	return (pCaps->DevCaps2 & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES)? D3D_OK:D3DERR_NOTAVAILABLE;
}


//////////////////////////////////////////////////////////////////////
// Name:	OneTimeSceneInit()
// Desc:	Called during initial app startup, this function performs all the permanent initialization
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectKeepStates::OneTimeSceneInit()
{
	for( int x = 0, i = 0; x < WIDTH; x++ )
		for( int y = 0; y < HEIGHT; y++ )
			for( int z = 0; z < DEPTH; z++)
			{
				m_shapes[i].NewShape(CS_BOX);
				m_shapes[i].Scale(2);
				m_shapes[i].Translate( (float)(2*x - WIDTH + 1.0f), (float)(2*y - HEIGHT), (float)(2*z + 0.1f + 1.0f) );
				i++;
			}
	
	HRESULT hr = D3D_OK;
	if( FAILED( hr = m_pSrcDevice->GetDisplayMode( &m_DisplayMode ) ) )
		return hr;

	// common render states
	m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

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
	float			fZn = 0.01f;
	float			fZf = 8 + DEPTH*8 + 0.1f;

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
HRESULT CStretchRectKeepStates::InitDeviceObjects()
{
	HRESULT hr = D3D_OK;

	try
	{
		// texture used in drawing the scene
		Check_HRFail( m_pSrcDevice->D3DXCreateTextureFromResourceEx(
																NULL,
																"LAKE",
																64,
																64,
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
									), "D3DXCreateTextureFromResourceEx"
						);

		// create the system surfaces to hold before/after images of the backbuffer
		Check_HRFail( m_pSrcDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer ), "GetBackBuffer" );
		Check_HRFail( m_pBackBuffer->GetDesc( &m_sdBackBuffer ), "GetDesc" );
		RELEASE( m_pBackBuffer );
		Check_HRFail( m_pSrcDevice->CreateOffscreenPlainSurface(	m_sdBackBuffer.Width,
																	m_sdBackBuffer.Height,
																	m_sdBackBuffer.Format,
																	POOL_SYSTEMMEM,
																	&m_pBackBufferSurfaceWithoutSR
										), "CreateOffscreenPlainSurface"
						);
		Check_HRFail( m_pSrcDevice->CreateOffscreenPlainSurface(	m_sdBackBuffer.Width,
																	m_sdBackBuffer.Height,
																	m_sdBackBuffer.Format,
																	POOL_SYSTEMMEM,
																	&m_pBackBufferSurfaceWithSR
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
HRESULT CStretchRectKeepStates::RestoreDeviceObjects()
{
	try
	{
		for( int i = 0; i < WIDTH*HEIGHT*DEPTH; i++ )
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
HRESULT CStretchRectKeepStates::FrameMove()
{
	for( int i = 0; i < WIDTH*HEIGHT*DEPTH; i++ )
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
// Name:	RenderScene( bool, bool )
// Desc:	render the main scene
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectKeepStates::RenderScene( bool bRender, bool bStretchRect )
{

	// store the old render state
	DWORD dwOldRS;
	Check_HRFail( m_pSrcDevice->GetRenderState( m_d3dRenderState, &dwOldRS ), "GetRenderState" );
	// set the tested render state
	if( bRender )
		Check_HRFail( m_pSrcDevice->SetRenderState( m_d3dRenderState, m_vecd3dRenderStateValues[m_nIndexd3dRenderStateValue] ), "SetRenderState" );

	if( bStretchRect )
	{
		LPSURFACE pDestSurface = bRender? m_pDestSurfaceWithRS:m_pDestSurfaceWithoutRS;

		Check_HRFail( m_pSrcDevice->StretchRect(	m_pSrcSurface,
													&(m_vecSrcRects[m_nIndexSrcRect]),
													pDestSurface,
													&m_vecDestRects[m_nIndexDestRect],
													m_vecFilters[m_nIndexFilter]
												), "StretchRect"
					);
		// if desired, simulate StretchRect actually alters the render state
		if( KeySet("ForceFail" ) )
			Check_HRFail( m_pSrcDevice->SetRenderState( m_d3dRenderState, m_vecd3dRenderStateValues[(m_nIndexd3dRenderStateValue+1)%m_vecd3dRenderStateValues.size()] ), "SetRenderState" );
	}

	if( bRender )
	{
		// set texture and fvf
		Check_HRFail( m_pSrcDevice->SetTexture( 0, m_pTexture ), "SetTexture" );
		Check_HRFail( m_pSrcDevice->SetFVF( D3DVERTEXHANDLE ), "SetFVF" );
	
		// draw all the objects (not the sphere)
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

		// copy the backbuffer contents
		LPSURFACE pBackBufferSurface = bStretchRect? m_pBackBufferSurfaceWithSR:m_pBackBufferSurfaceWithoutSR;
		Check_HRFail( m_pSrcDevice->D3DXLoadSurfaceFromSurface(	
														pBackBufferSurface,
														NULL,
														NULL,
														m_pBackBuffer,
														NULL,
														NULL,
														D3DX_FILTER_NONE,
														0 ),
								"D3DXLoadSurfaceFromSurface" );
	}
	
	// restore the old RS
	Check_HRFail( m_pSrcDevice->SetRenderState( m_d3dRenderState, dwOldRS ), "SetRenderState" );
	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	Render()
// Desc:	Called once per frame, the call is the entry point for 3d
//			rendering. This function sets up render states, clears the
//			viewport, and renders the scene.
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectKeepStates::Render()
{
	HRESULT hr = D3D_OK;
	try
	{
		// don't render the scene, but use StretchRect
		Check_HRFail( RenderScene( false, true ), "RenderScene(false,true)" );

		// render the scene without any interference from StretchRect
		// clear and begin the scene
		Check_HRFail( m_pSrcDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, RGBA_MAKE(0x70, 0x60, 0xC0, 0x00), 1.0f, 0L ), "Clear" );
		Check_HRFail( m_pSrcDevice->BeginScene(), "BeginScene" );
		Check_HRFail( RenderScene( true, false ), "RenderScene(true,false)" );
		// End the scene
        Check_HRFail( m_pSrcDevice->EndScene(), "EndScene" );
		// present the scene
		Check_HRFail( m_pSrcDevice->Present( NULL, NULL, NULL, NULL ), "Present" );

		// render the scene again, also calling StretchRect
		// clear and begin the scene
		Check_HRFail( m_pSrcDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, RGBA_MAKE(0x70, 0x60, 0xC0, 0x00), 1.0f, 0L ), "Clear" );
		Check_HRFail( m_pSrcDevice->BeginScene(), "BeginScene" );
		Check_HRFail( RenderScene( true, true ), "RenderScene(true,true)" );
		// End the scene
        Check_HRFail( m_pSrcDevice->EndScene(), "EndScene" );
		// present the scene
		Check_HRFail( m_pSrcDevice->Present( NULL, NULL, NULL, NULL ), "Present" );

		
		// debug: save surface to file
		if( m_bSave )
		{
			m_pSrcDevice->D3DXSaveSurfaceToFile( "m_pSrcSurface.dds",					D3DXIFF_DDS, m_pSrcSurface,					NULL, NULL );
			m_pSrcDevice->D3DXSaveSurfaceToFile( "m_pDestSurfaceWithoutRS.dds",			D3DXIFF_DDS, m_pDestSurfaceWithoutRS,		NULL, NULL );
			m_pSrcDevice->D3DXSaveSurfaceToFile( "m_pDestSurfaceWithRS.dds",			D3DXIFF_DDS, m_pDestSurfaceWithRS,			NULL, NULL );
			m_pSrcDevice->D3DXSaveSurfaceToFile( "m_pBackBufferSurfaceWithoutSR.dds",	D3DXIFF_DDS, m_pBackBufferSurfaceWithoutSR, NULL, NULL );
			m_pSrcDevice->D3DXSaveSurfaceToFile( "m_pBackBufferSurfaceWithSR.dds",		D3DXIFF_DDS, m_pBackBufferSurfaceWithSR,	NULL, NULL );
		}


		Check_HRFail( CompareSurfaces( m_pDestSurfaceWithoutRS, m_pDestSurfaceWithRS, &(m_vecDestRects[m_nIndexDestRect]) ), "CompareSurfaces (StretchRect)" );
		Check_HRFail( CompareSurfaces( m_pBackBufferSurfaceWithoutSR, m_pBackBufferSurfaceWithSR ), "CompareSurfaces (BackBuffers)" );
    }
	catch( HRESULT hError )
	{
		m_pSrcDevice->EndScene();
		hr = hError;
	}

	return hr;
}


//////////////////////////////////////////////////////////////////////
// Name:	InvalidateDeviceObjects()
// Desc:	Called when the device-dependent objects are about to be lost.
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectKeepStates::InvalidateDeviceObjects()
{
	for( int i = 0; i < WIDTH*HEIGHT*DEPTH; i++ )
	{
		RELEASE( m_pVertexBuffer[i] );
		RELEASE( m_pIndexBuffer[i] );
	}
	
	RELEASE( m_pBackBuffer );

	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	DeleteDeviceObjects()
// Desc:	Called when the app is exiting, or the device is being changed, this function deletes any device dependent objects.
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectKeepStates::DeleteDeviceObjects()
{
	RELEASE( m_pTexture );
	RELEASE( m_pBackBufferSurfaceWithoutSR );
	RELEASE( m_pBackBufferSurfaceWithSR );
	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	FinalCleanup()
// Desc:	Called before the app exits, this function gives the app the chance to cleanup after itself.
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectKeepStates::FinalCleanup()
{
	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// CStretchRectColorWriteEnable class
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStretchRectColorWriteEnable::CStretchRectColorWriteEnable()
{
	m_szTestName	= _T("StretchRectColorWriteEnable");
	m_szCommandKey	= _T("StretchRectColorWriteEnable");
}


//////////////////////////////////////////////////////////////////////
// Name:	InitializeRenderState()
// Desc:	virtual pure method to force derived classes from here, for each tested render state
//////////////////////////////////////////////////////////////////////
void CStretchRectColorWriteEnable::InitializeRenderState()
{
	m_d3dRenderState = D3DRS_COLORWRITEENABLE;
	m_vecd3dRenderStateValues.push_back( D3DCOLORWRITEENABLE_ALPHA );
	m_vecd3dRenderStateValues.push_back( D3DCOLORWRITEENABLE_BLUE );
	m_vecd3dRenderStateValues.push_back( D3DCOLORWRITEENABLE_GREEN );
	m_vecd3dRenderStateValues.push_back( D3DCOLORWRITEENABLE_RED );
	m_vecd3dRenderStateValues.push_back( D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN );
	m_vecd3dRenderStateValues.push_back( D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED);
	m_vecd3dRenderStateValues.push_back( D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED );
	m_vecd3dRenderStateValues.push_back( D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED );
}


//////////////////////////////////////////////////////////////////////
// Name:	ConfirmDevice( CAPS*, DWORD, FORMAT )
// Desc:	Called during device initialization, this code checks the device for some minimum set of capabilities
//////////////////////////////////////////////////////////////////////
HRESULT CStretchRectColorWriteEnable::ConfirmDevice( CAPS* pCaps, DWORD dwBehavior, FORMAT Format )
{
	return (pCaps->PrimitiveMiscCaps & D3DPMISCCAPS_COLORWRITEENABLE)? CStretchRectKeepStates::ConfirmDevice(pCaps, dwBehavior, Format):D3DERR_NOTAVAILABLE;
}



//////////////////////////////////////////////////////////////////////
// CStretchRectFillMode class
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStretchRectFillMode::CStretchRectFillMode()
{
	m_szTestName	= _T("StretchRectFillMode");
	m_szCommandKey	= _T("StretchRectFillMode");
}


//////////////////////////////////////////////////////////////////////
// Name:	InitializeRenderState()
// Desc:	virtual pure method to force derived classes from here, for each tested render state
//////////////////////////////////////////////////////////////////////
void CStretchRectFillMode::InitializeRenderState()
{
	m_d3dRenderState = D3DRS_FILLMODE;
	m_vecd3dRenderStateValues.push_back( D3DFILL_POINT );
	m_vecd3dRenderStateValues.push_back( D3DFILL_WIREFRAME );
	m_vecd3dRenderStateValues.push_back( D3DFILL_SOLID );
}



//////////////////////////////////////////////////////////////////////
// CStretchRectShadeMode class
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStretchRectShadeMode::CStretchRectShadeMode()
{
	m_szTestName	= _T("StretchRectShadeMode");
	m_szCommandKey	= _T("StretchRectShadeMode");
}


//////////////////////////////////////////////////////////////////////
// Name:	InitializeRenderState()
// Desc:	virtual pure method to force derived classes from here, for each tested render state
//////////////////////////////////////////////////////////////////////
void CStretchRectShadeMode::InitializeRenderState()
{
	m_d3dRenderState = D3DRS_SHADEMODE;
	m_vecd3dRenderStateValues.push_back( D3DSHADE_FLAT );
	m_vecd3dRenderStateValues.push_back( D3DSHADE_GOURAUD );
	// m_vecd3dRenderStateValues.push_back( D3DSHADE_PHONG );	// not supported
}



//////////////////////////////////////////////////////////////////////
// CStretchRectLastPixel class
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStretchRectLastPixel::CStretchRectLastPixel()
{
	m_szTestName	= _T("StretchRectLastPixel");
	m_szCommandKey	= _T("StretchRectLastPixel");
}


//////////////////////////////////////////////////////////////////////
// Name:	InitializeRenderState()
// Desc:	virtual pure method to force derived classes from here, for each tested render state
//////////////////////////////////////////////////////////////////////
void CStretchRectLastPixel::InitializeRenderState()
{
	m_d3dRenderState = D3DRS_LASTPIXEL;
	m_vecd3dRenderStateValues.push_back( TRUE );
	m_vecd3dRenderStateValues.push_back( FALSE );
	m_pSrcDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
}



//////////////////////////////////////////////////////////////////////
// CStretchRectSrcBlend class
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStretchRectSrcBlend::CStretchRectSrcBlend()
{
	m_szTestName	= _T("StretchRectSrcBlend");
	m_szCommandKey	= _T("StretchRectSrcBlend");
}


//////////////////////////////////////////////////////////////////////
// Name:	InitializeRenderState()
// Desc:	virtual pure method to force derived classes from here, for each tested render state
//////////////////////////////////////////////////////////////////////
void CStretchRectSrcBlend::InitializeRenderState()
{
	m_d3dRenderState = D3DRS_SRCBLEND;
	if( m_DeviceCaps.SrcBlendCaps & D3DPBLENDCAPS_ZERO )
		m_vecd3dRenderStateValues.push_back( D3DBLEND_ZERO );
	if( m_DeviceCaps.SrcBlendCaps & D3DPBLENDCAPS_ONE )
		m_vecd3dRenderStateValues.push_back( D3DBLEND_ONE );
	if( m_DeviceCaps.SrcBlendCaps & D3DPBLENDCAPS_SRCCOLOR )
		m_vecd3dRenderStateValues.push_back( D3DBLEND_SRCCOLOR );
	if( m_DeviceCaps.SrcBlendCaps & D3DPBLENDCAPS_INVSRCCOLOR )
		m_vecd3dRenderStateValues.push_back( D3DBLEND_INVSRCCOLOR );
	if( m_DeviceCaps.SrcBlendCaps & D3DPBLENDCAPS_DESTCOLOR )
		m_vecd3dRenderStateValues.push_back( D3DBLEND_DESTCOLOR );
	if( m_DeviceCaps.SrcBlendCaps & D3DPBLENDCAPS_INVDESTCOLOR )
		m_vecd3dRenderStateValues.push_back( D3DBLEND_INVDESTCOLOR );

	m_pSrcDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
}



//////////////////////////////////////////////////////////////////////
// CStretchRectDitherEnable class
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStretchRectDitherEnable::CStretchRectDitherEnable()
{
	m_szTestName	= _T("StretchRectDitherEnable");
	m_szCommandKey	= _T("StretchRectDitherEnable");
}


//////////////////////////////////////////////////////////////////////
// Name:	InitializeRenderState()
// Desc:	virtual pure method to force derived classes from here, for each tested render state
//////////////////////////////////////////////////////////////////////
void CStretchRectDitherEnable::InitializeRenderState()
{
	m_d3dRenderState = D3DRS_DITHERENABLE;
	m_vecd3dRenderStateValues.push_back( TRUE );
	m_vecd3dRenderStateValues.push_back( FALSE );
}



//////////////////////////////////////////////////////////////////////
// CStretchRectSpecularEnable class
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStretchRectSpecularEnable::CStretchRectSpecularEnable()
{
	m_szTestName	= _T("StretchRectSpecularEnable");
	m_szCommandKey	= _T("StretchRectSpecularEnable");
}


//////////////////////////////////////////////////////////////////////
// Name:	InitializeRenderState()
// Desc:	virtual pure method to force derived classes from here, for each tested render state
//////////////////////////////////////////////////////////////////////
void CStretchRectSpecularEnable::InitializeRenderState()
{
	m_d3dRenderState = D3DRS_SPECULARENABLE;
	m_vecd3dRenderStateValues.push_back( TRUE );
	m_vecd3dRenderStateValues.push_back( FALSE );
}



//////////////////////////////////////////////////////////////////////
// CStretchRectPixelFog class
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStretchRectPixelFog::CStretchRectPixelFog()
{
	m_szTestName	= _T("StretchRectPixelFog");
	m_szCommandKey	= _T("StretchRectPixelFog");
}


//////////////////////////////////////////////////////////////////////
// Name:	InitializeRenderState()
// Desc:	virtual pure method to force derived classes from here, for each tested render state
//////////////////////////////////////////////////////////////////////
void CStretchRectPixelFog::InitializeRenderState()
{
	m_d3dRenderState = D3DRS_FOGENABLE;
	m_vecd3dRenderStateValues.push_back( TRUE );
	m_vecd3dRenderStateValues.push_back( FALSE );
	
	float Start		= 0.5f;    // For linear mode
	float End		= 0.8f;
	
	// Set the fog color
	m_pSrcDevice->SetRenderState( D3DRS_FOGCOLOR, D3DCOLOR_ARGB(0xff, 0xff, 0x60, 0x40 ) );

	// Set the fog parameters
	m_pSrcDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
	m_pSrcDevice->SetRenderState( D3DRS_FOGSTART, *(DWORD *)(&Start) );
	m_pSrcDevice->SetRenderState( D3DRS_FOGEND,   *(DWORD *)(&End) );
}



//////////////////////////////////////////////////////////////////////
// CStretchRectVertexFog class
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStretchRectVertexFog::CStretchRectVertexFog()
{
	m_szTestName	= _T("StretchRectVertexFog");
	m_szCommandKey	= _T("StretchRectVertexFog");
}


//////////////////////////////////////////////////////////////////////
// Name:	InitializeRenderState()
// Desc:	virtual pure method to force derived classes from here, for each tested render state
//////////////////////////////////////////////////////////////////////
void CStretchRectVertexFog::InitializeRenderState()
{
	m_d3dRenderState = D3DRS_FOGENABLE;
	m_vecd3dRenderStateValues.push_back( TRUE );
	m_vecd3dRenderStateValues.push_back( FALSE );
	
	float Density	= 0.66f;    // For exponential mode
	
	// Set the fog color
	m_pSrcDevice->SetRenderState( D3DRS_FOGCOLOR, D3DCOLOR_ARGB(0xff, 0xff, 0x60, 0x40 ) );

	// Set the fog parameters
	m_pSrcDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_EXP );
	m_pSrcDevice->SetRenderState( D3DRS_FOGDENSITY, *(DWORD *)(&Density) );
}



//////////////////////////////////////////////////////////////////////
// CStretchRectAmbient class
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStretchRectAmbient::CStretchRectAmbient()
{
	m_szTestName	= _T("StretchRectAmbient");
	m_szCommandKey	= _T("StretchRectAmbient");
}


//////////////////////////////////////////////////////////////////////
// Name:	InitializeRenderState()
// Desc:	virtual pure method to force derived classes from here, for each tested render state
//////////////////////////////////////////////////////////////////////
void CStretchRectAmbient::InitializeRenderState()
{
	m_d3dRenderState = D3DRS_AMBIENT;
	m_vecd3dRenderStateValues.push_back( 0 );
	m_vecd3dRenderStateValues.push_back( D3DCOLOR_ARGB(0xff, 0xff, 0x00, 0x00 ) );
	m_vecd3dRenderStateValues.push_back( D3DCOLOR_ARGB(0xee, 0x00, 0xff, 0x00 ) );
	m_vecd3dRenderStateValues.push_back( D3DCOLOR_ARGB(0xdd, 0x00, 0x00, 0xff ) );
	m_vecd3dRenderStateValues.push_back( D3DCOLOR_ARGB(0xcc, 0x80, 0x80, 0x80 ) );
}
