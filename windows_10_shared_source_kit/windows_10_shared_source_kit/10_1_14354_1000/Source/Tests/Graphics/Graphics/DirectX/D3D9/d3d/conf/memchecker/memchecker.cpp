// MemChecker.cpp: implementation of the CMemChecker class.
//
//////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <time.h>
#include "MemChecker.h"
#include <d3dtypes.h>

//
// Declare the framework
//
CD3DWindowFramework	App;


//
// Define the groups here
//
CCreateTexture TMemChecker_CreateTexture;
CCreateCubeTexture TMemChecker_CreateCubeTexture;
CCreateVolumeTexture TMemChecker_VolumeTexture;
CCreateVertexBuffer TMemChecker_CreateVertexBuffer;
CCreateIndexBuffer TMemChecker_CreateIndexBuffer;
CCreateAdditionalSwapChain TMemChecker_CreateAdditionalSwapChain;
CCreateOffscreenPlainSurface TMemChecker_CreateOffscreenPlainSurface;
CCreateRenderTarget TMemChecker_CreateRenderTarget;
CCreateVertexShader TMemChecker_CreateVertexShader;
CCreatePixelShader TMemChecker_CreatePixelShader;
CRenderTest TMemChecker_Render;


//
// Name: FillD3DXTexture
// Desc: method called from the D3DX, fills a texture
//

VOID __stdcall FillTexture9(D3DXVECTOR4 *pOut, D3DXVECTOR2 *pTexCoord, D3DXVECTOR2 *pTexelSize, LPVOID pData)
{
	ASSERT( NULL != pData );

	DWORD xPixel = (DWORD) ( pTexCoord->x / pTexelSize->x + 0.51 );		// current Pixel X
	DWORD yPixel = (DWORD) ( pTexCoord->y / pTexelSize->y + 0.51 );		// current Pixel Y
	DWORD xMaxPixel = (DWORD) ( 1.00f / pTexelSize->x + 0.51 );			// Max Pixel X
	DWORD yMaxPixel = (DWORD) ( 1.00f / pTexelSize->y + 0.51 );			// Max Pixel Y

	ASSERT( xPixel <= xMaxPixel && yPixel <= yMaxPixel );

	float xSpace = 4.0f * ( xPixel >> 2 ) / float( xMaxPixel );
	float ySpace = 4.0f * ( yPixel >> 2 ) / float( yMaxPixel );

	ASSERT(NULL != pData);
	int iObject = (*(DWORD*)pData) + 1;
	ASSERT( 1 <= iObject && 100 >= iObject );

	pOut->x = 0.5f * xSpace * iObject/100.0f + 0.5f * ySpace * (101-iObject)/100.0f;
	pOut->y = 0.75f * xSpace * iObject/100.0f + 0.25f * ySpace * (101-iObject)/100.0f;
	pOut->z = 0.25f * xSpace * iObject/100.0f + 0.75f * ySpace * (101-iObject)/100.0f;
	pOut->w = 0.6f * xSpace + 0.4f * ySpace;
}


VOID __cdecl FillTexture8(D3DXVECTOR4 *pOut, D3DXVECTOR2 *pTexCoord, D3DXVECTOR2 *pTexelSize, LPVOID pData)
{
	ASSERT( NULL != pData );

	DWORD xPixel = (DWORD) ( pTexCoord->x / pTexelSize->x + 0.51 );		// current Pixel X
	DWORD yPixel = (DWORD) ( pTexCoord->y / pTexelSize->y + 0.51 );		// current Pixel Y
	DWORD xMaxPixel = (DWORD) ( 1.00f / pTexelSize->x + 0.51 );			// Max Pixel X
	DWORD yMaxPixel = (DWORD) ( 1.00f / pTexelSize->y + 0.51 );			// Max Pixel Y

	ASSERT( xPixel <= xMaxPixel && yPixel <= yMaxPixel );

	float xSpace = 4.0f * ( xPixel >> 2 ) / float( xMaxPixel );
	float ySpace = 4.0f * ( yPixel >> 2 ) / float( yMaxPixel );

	ASSERT(NULL != pData);
	int iObject = (*(DWORD*)pData) + 1;
	ASSERT( 1 <= iObject && 100 >= iObject );

	pOut->x = 0.5f * xSpace * iObject/100.0f + 0.5f * ySpace * (101-iObject)/100.0f;
	pOut->y = 0.75f * xSpace * iObject/100.0f + 0.25f * ySpace * (101-iObject)/100.0f;
	pOut->z = 0.25f * xSpace * iObject/100.0f + 0.75f * ySpace * (101-iObject)/100.0f;
	pOut->w = 0.6f * xSpace + 0.4f * ySpace;
}


//
// CMemChecker - base class 
//

CMemChecker::CMemChecker()
{
	SetTestType(TESTTYPE_CONF);
	
	m_Options.D3DOptions.fMinDXVersion		= 8.0f;
	m_Options.D3DOptions.bReference			= false;
	m_Options.D3DOptions.bZBuffer			= false;
	m_Options.D3DOptions.bCachedBackBuffer	= false;
	m_Options.D3DOptions.dwBehaviors		= DEVICECREATE_STANDARD | DEVICECREATE_SW_OR_HW_VP;

	ReadInteger( _T("Calls"), 16384, &m_iCalls );
	m_Retry = 1;
}

CMemChecker::~CMemChecker()
{
}

void CMemChecker::Check_HRFail( HRESULT hResult, const char *sMethod )
{
	if( FAILED(hResult) )
	{
		WriteToLog( TEXT( "%s failed, error code %s (0x%x)\n" ),
				sMethod,
				m_pD3D->HResultToString( hResult ),
				hResult );
		//Fail();		// DO NOT FAIL: the format might not be supported so it's ok
		throw hResult;
	}
}

UINT CMemChecker::TestInitialize()
{
	m_pdeviceCaps = m_pSrcDevice->GetCaps();
	
	POINT p;
	p.x = 256; p.y = 256;
	m_vecSize.clear();
	m_vecSize.push_back(p);
	
	m_vecPool.clear();
	m_vecPool.push_back(POOL_SYSTEMMEM);
	m_vecPool.push_back(POOL_DEFAULT);
	m_vecPool.push_back(POOL_MANAGED);
	m_vecPool.push_back(POOL_SCRATCH);

	m_vecUsage.clear();
	m_vecUsage.push_back(0);	// just an index into a local table that makes sense for each resource
	m_vecUsage.push_back(1);
	m_vecUsage.push_back(2);

	m_vecFormat.clear();
	m_vecFormat.push_back(FMT_A8R8G8B8);

	SetTestRange(1, m_vecSize.size() * m_vecPool.size() * m_vecUsage.size() * m_vecFormat.size() * 2);
	return D3DTESTINIT_RUN;
}

bool CMemChecker::TestTerminate()
{
	return true;
}

bool CMemChecker::ExecuteTest(UINT nTest)
{
	bool bTestFrames = ( nTest - 1 ) / ( m_vecSize.size() * m_vecPool.size() * m_vecUsage.size() * m_vecFormat.size() ) >= 1;
	MEMORYSTATUS	memoryBefore, memoryAfter;
	//MEMORYSTATUSEX	memoryBefore, memoryAfter;
	memoryBefore.dwLength = memoryAfter.dwLength = sizeof MEMORYSTATUS;
	//memoryBefore.dwLength = memoryAfter.dwLength = sizeof MEMORYSTATUSEX;
	
	// shut up the logger
	UINT iRestoreLogLevel = m_pFramework->GetLogLevel();
	m_pFramework->SetLogLevel( 0 );

	BeginTestCase( m_szTestName, nTest );
	DecodeTestNumber(nTest);

	LONG lmemoryLeak = 0;
	UINT uTry = 1;
	do
	{
		if( bTestFrames )
		{
			Sleep(1000);
			GlobalMemoryStatus( &memoryBefore );
		}
	
		// Resource creation
		onActivity();
		SetLastError( D3D_OK );
	
		if( bTestFrames )
		{
			// validate no memory leak
			Sleep(1000);
			GlobalMemoryStatus( &memoryAfter );
			//GlobalMemoryStatusEx( &memoryAfter );
			LONG lmemoryLeak = (LONG)memoryBefore.dwAvailVirtual - (LONG)memoryAfter.dwAvailVirtual;
			//DWORDLONG lmemoryLeak = memoryBefore.ullAvailVirtual - memoryAfter.ullAvailVirtual;

			// need to allow for greater errors
			// due to many background processes in longhorn
			if( lmemoryLeak > 1024 * 1024 )
			{
				_sntprintf( m_szMemLeak, 256, _T("Try %d: %d bytes leaked in resource creation\n"), uTry, lmemoryLeak );
				m_szMemLeak[255] = 0;
				m_pFramework->WriteToLog( 0, m_szMemLeak );
			}
		}
	}
	while( ++uTry <= m_Retry && lmemoryLeak > 1024 * 1024 );

	if( lmemoryLeak > 1024 * 1024 )
		Fail();

	GetLastError();
	EndTestCase();

	// restore the logger
	m_pFramework->SetLogLevel( iRestoreLogLevel );

	return false;
}

void CMemChecker::DecodeTestNumber(UINT nTest)
{
	UINT nAllTests = m_vecSize.size() * m_vecPool.size() * m_vecUsage.size() * m_vecFormat.size();
	nTest= ( nTest-1 ) % nAllTests;
	m_indexSize		= nTest % m_vecSize.size();
	nTest			/= m_vecSize.size();
	m_indexPool		= nTest % m_vecPool.size();
	nTest			/= m_vecPool.size();
	m_indexUsage	= nTest % m_vecUsage.size();
	nTest			/= m_vecUsage.size();
	m_indexFormat	= nTest % m_vecFormat.size();
	nTest			/= m_vecFormat.size();
}

bool CMemChecker::DisplayFrame()
{
	return true;
}

bool CMemChecker::ProcessFrame()
{
	return true;
}


//
// CCreateTexture - calls CreateTexture
//

CCreateTexture::CCreateTexture() :
	m_pTexture( NULL )
{
	m_szTestName	= _T("MemChecker-CreateTexture");
	m_szCommandKey	= _T("CreateTexture");
}

void CCreateTexture::onActivity()
{
	HRESULT hResult = D3D_OK;
	for( int i = 0; i < m_iCalls; i++ )
	{
		try
		{
			DWORD vUsages[] = 
			{
				0,
				USAGE_RENDERTARGET,
			};

			m_pTexture = (LPTEXTURE)(LONG_PTR)0xcdcdcdcd;
			DWORD usage = vUsages[m_indexUsage%(sizeof(vUsages)/sizeof DWORD)];
			DWORD correctedPool = USAGE_RENDERTARGET == usage? POOL_DEFAULT : m_vecPool[m_indexPool];
			hResult = m_pSrcDevice->CreateTexture(	m_vecSize[m_indexSize].x,
													m_vecSize[m_indexSize].y,
													0,
													usage,
													m_vecFormat[m_indexFormat],
													correctedPool,
													&m_pTexture );
			if( (LPTEXTURE)(LONG_PTR)0xcdcdcdcd != m_pTexture )
			{
				RELEASE(m_pTexture);
			}
		}
		catch( HRESULT hError )
		{
			WriteToLog( _T("CCreateTexture::onActivity - Unhandled exception caught: 0x%x (%s)"), hError, m_pD3D->HResultToString(hError) );
			Fail();
		}
	}
}


//
// CCreateCubeTexture - calls CreateCubeTexture
//

CCreateCubeTexture::CCreateCubeTexture() :
	m_pCubeTexture( NULL )
{
	m_szTestName	= _T("MemChecker-CreateCubeTexture");
	m_szCommandKey	= _T("CreateCubeTexture");
}

void CCreateCubeTexture::onActivity()
{
	HRESULT hResult = D3D_OK;
	for( int i = 0; i < m_iCalls; i++ )
	{
		try
		{
			DWORD vUsages[] = 
			{
				0,
				USAGE_RENDERTARGET,
			};

			m_pCubeTexture = (LPCUBETEXTURE)(LONG_PTR)0xcdcdcdcd;
			DWORD usage = vUsages[m_indexUsage%(sizeof(vUsages)/sizeof DWORD)];
			DWORD correctedPool = USAGE_RENDERTARGET == usage? POOL_DEFAULT : m_vecPool[m_indexPool];
			hResult = m_pSrcDevice->CreateCubeTexture(	m_vecSize[m_indexSize].x,
														0,
														usage,
														m_vecFormat[m_indexFormat],
														correctedPool,
														&m_pCubeTexture );
			if( (LPCUBETEXTURE)(LONG_PTR)0xcdcdcdcd != m_pCubeTexture )
			{
				RELEASE(m_pCubeTexture);
			}
		}
		catch( HRESULT hError )
		{
			WriteToLog( _T("CCreateCubeTexture::onActivity - Unhandled exception caught: 0x%x (%s)"), hError, m_pD3D->HResultToString(hError) );
			Fail();
		}
	}
}


//
// CCreateVolumeTexture - calls CreateVolumeTexture
//

CCreateVolumeTexture::CCreateVolumeTexture() :
	m_pVolumeTexture( NULL )
{
	m_szTestName	= _T("MemChecker-CreateVolumeTexture");
	m_szCommandKey	= _T("CreateVolumeTexture");
}

void CCreateVolumeTexture::onActivity()
{
	HRESULT hResult = D3D_OK;
	for( int i = 0; i < m_iCalls; i++ )
	{
		try
		{
			m_pVolumeTexture = (LPVOLUMETEXTURE)(LONG_PTR)0xcdcdcdcd;
			hResult = m_pSrcDevice->CreateVolumeTexture(	m_vecSize[m_indexSize].x,
															m_vecSize[m_indexSize].y,
															8,
															0,
															0,
															m_vecFormat[m_indexFormat],
															m_vecPool[m_indexPool],
															&m_pVolumeTexture );
			if( (LPVOLUMETEXTURE)(LONG_PTR)0xcdcdcdcd != m_pVolumeTexture )
			{
				RELEASE(m_pVolumeTexture);
			}
		}
		catch( HRESULT hError )
		{
			WriteToLog( _T("CCreateVolumeTexture::onActivity - Unhandled exception caught: 0x%x (%s)"), hError, m_pD3D->HResultToString(hError) );
			Fail();
		}
	}
}


//
// CCreateVertexBuffer - calls CreateVertexBuffer
//

CCreateVertexBuffer::CCreateVertexBuffer() :
	m_pVertexBuffer( NULL )
{
	m_szTestName	= _T("MemChecker-CreateVertexBuffer");
	m_szCommandKey	= _T("CreateVertexBuffer");
}

void CCreateVertexBuffer::onActivity()
{
	HRESULT hResult = D3D_OK;
	for( int i = 0; i < m_iCalls; i++ )
	{
		try
		{
			DWORD vUsages[] = 
			{
				0,
				USAGE_WRITEONLY,
				USAGE_DYNAMIC,
			};
	
			m_pVertexBuffer = (LPVERTEXBUFFER)(LONG_PTR)0xcdcdcdcd;
			DWORD usage = vUsages[m_indexUsage%(sizeof(vUsages)/sizeof DWORD)];
			DWORD correctedPool = USAGE_DYNAMIC == usage || POOL_SCRATCH == m_vecPool[m_indexPool] ? POOL_SYSTEMMEM : m_vecPool[m_indexPool];
			hResult = m_pSrcDevice->CreateVertexBuffer(	m_vecSize[m_indexSize].x * m_vecSize[m_indexSize].y * sizeof FVFVERTEX,
														usage,
														FVFVERTEXHANDLE,
														correctedPool,
														&m_pVertexBuffer );
			if( (LPVERTEXBUFFER)(LONG_PTR)0xcdcdcdcd != m_pVertexBuffer )
			{
				RELEASE(m_pVertexBuffer);
			}
		}
		catch( HRESULT hError )
		{
			WriteToLog( _T("CCreateVertexBuffer::onActivity - Unhandled exception caught: 0x%x (%s)"), hError, m_pD3D->HResultToString(hError) );
			Fail();
		}
	}
}


//
// CCreateIndexBuffer - calls CreateIndexBuffer
//

CCreateIndexBuffer::CCreateIndexBuffer() :
	m_pIndexBuffer( NULL )
{
	m_szTestName	= _T("MemChecker-CreateIndexBuffer");
	m_szCommandKey	= _T("CreateIndexBuffer");
}

void CCreateIndexBuffer::onActivity()
{
	HRESULT hResult = D3D_OK;
	for( int i = 0; i < m_iCalls; i++ )
	{
		try
		{
			DWORD vUsages[] = 
			{
				0,
				USAGE_WRITEONLY,
				USAGE_DYNAMIC,
			};
	
			m_pIndexBuffer = (LPINDEXBUFFER)(LONG_PTR)0xcdcdcdcd;
			DWORD usage = vUsages[m_indexUsage%(sizeof(vUsages)/sizeof DWORD)];
			DWORD correctedPool = USAGE_DYNAMIC == usage || POOL_SCRATCH == m_vecPool[m_indexPool] ? POOL_SYSTEMMEM : m_vecPool[m_indexPool];
			hResult = m_pSrcDevice->CreateIndexBuffer(	m_vecSize[m_indexSize].x * m_vecSize[m_indexSize].y * sizeof DWORD,
														usage,
														FMT_INDEX32,
														correctedPool,
														&m_pIndexBuffer );
			if( (LPINDEXBUFFER)(LONG_PTR)0xcdcdcdcd != m_pIndexBuffer )
			{
				RELEASE(m_pIndexBuffer);
			}
		}
		catch( HRESULT hError )
		{
			WriteToLog( _T("CCreateIndexBuffer::onActivity - Unhandled exception caught: 0x%x (%s)"), hError, m_pD3D->HResultToString(hError) );
			Fail();
		}
	}
}


//
// CCreateAdditionalSwapChain - calls CreateAdditionalSwapChain
//

CCreateAdditionalSwapChain::CCreateAdditionalSwapChain() :
	m_pSwapChain( NULL )
{
	m_szTestName	= _T("MemChecker-CreateAdditionalSwapChain");
	m_szCommandKey	= _T("CreateAdditionalSwapChain");
}

void CCreateAdditionalSwapChain::onActivity()
{
	HRESULT hResult = D3D_OK;
	for( int i = 0; i < m_iCalls; i++ )
	{
		try
		{
			m_pSwapChain = (LPSWAPCHAIN)(LONG_PTR)0xcdcdcdcd;
			PRESENT_PARAMETERS pParams;
			memcpy( &pParams, m_pSrcDevice->GetPresParams(), sizeof PRESENT_PARAMETERS );
			pParams.uWidth			= m_vecSize[m_indexSize].x;
			pParams.uHeight			= m_vecSize[m_indexSize].y;
			pParams.BackBufferFormat= m_vecFormat[m_indexFormat];

			hResult = m_pSrcDevice->CreateAdditionalSwapChain(&pParams, &m_pSwapChain);

			if( (LPSWAPCHAIN)(LONG_PTR)0xcdcdcdcd != m_pSwapChain )
			{
				RELEASE(m_pSwapChain);
			}
		}
		catch( HRESULT hError )
		{
			WriteToLog( _T("CCreateAdditionalSwapChain::onActivity - Unhandled exception caught: 0x%x (%s)"), hError, m_pD3D->HResultToString(hError) );
			Fail();
		}
	}
}


//
// CCreateOffscreenPlainSurface - calls CreateOffscreenPlainSurface
//

CCreateOffscreenPlainSurface::CCreateOffscreenPlainSurface() :
	m_pOffscreenPlainSurface( NULL )
{
	m_szTestName	= _T("MemChecker-CreateOffscreenPlainSurface");
	m_szCommandKey	= _T("CreateOffscreenPlainSurface");
}

void CCreateOffscreenPlainSurface::onActivity()
{
	HRESULT hResult = D3D_OK;
	for( int i = 0; i < m_iCalls; i++ )
	{
		try
		{
			if( m_pSrcDevice->GetInterface() >= 9.0 )
			{
				m_pOffscreenPlainSurface = (LPSURFACE)(LONG_PTR)0xcdcdcdcd;
				DWORD correctedPool = POOL_MANAGED == m_vecPool[m_indexPool] ? POOL_SYSTEMMEM : m_vecPool[m_indexPool];
				hResult = m_pSrcDevice->CreateOffscreenPlainSurface(m_vecSize[m_indexSize].x,
																	m_vecSize[m_indexSize].y,
																	m_vecFormat[m_indexFormat],
																	correctedPool,
																	&m_pOffscreenPlainSurface );
				if( (LPSURFACE)(LONG_PTR)0xcdcdcdcd != m_pOffscreenPlainSurface )
				{
					RELEASE(m_pOffscreenPlainSurface);
				}
			}

		}
		catch( HRESULT hError )
		{
			WriteToLog( _T("CCreateOffscreenPlainSurface::onActivity - Unhandled exception caught: 0x%x (%s)"), hError, m_pD3D->HResultToString(hError) );
			Fail();
		}
	}
}


//
// CCreateRenderTarget - calls CreateRenderTarget
//

CCreateRenderTarget::CCreateRenderTarget() :
	m_pRenderTarget( NULL )
{
	m_szTestName	= _T("MemChecker-CreateRenderTarget");
	m_szCommandKey	= _T("CreateRenderTarget");
}

void CCreateRenderTarget::onActivity()
{
	HRESULT hResult = D3D_OK;
	for( int i = 0; i < m_iCalls; i++ )
	{
		try
		{
			m_pRenderTarget = (LPSURFACE)(LONG_PTR)0xcdcdcdcd;
			hResult = m_pSrcDevice->CreateRenderTarget(	m_vecSize[m_indexSize].x,
														m_vecSize[m_indexSize].y,
														m_vecFormat[m_indexFormat],
														MULTISAMPLE_NONE,
														//0,
														(m_indexPool % 2),
														&m_pRenderTarget );
			if( (LPSURFACE)(LONG_PTR)0xcdcdcdcd != m_pRenderTarget )
			{
				RELEASE(m_pRenderTarget);
			}
		}
		catch( HRESULT hError )
		{
			WriteToLog( _T("CCreateRenderTarget::onActivity - Unhandled exception caught: 0x%x (%s)"), hError, m_pD3D->HResultToString(hError) );
			Fail();
		}
	}
}


//
// CCreateVertexShader - calls CreateVertexShader
//

CCreateVertexShader::CCreateVertexShader() :
	m_pVertexShader( NULL )
{
	m_szTestName	= _T("MemChecker-CreateVertexShader");
	m_szCommandKey	= _T("CreateVertexShader");
}

UINT CCreateVertexShader::TestInitialize()
{
	UINT uResult = CMemChecker::TestInitialize();
	if( D3DTESTINIT_RUN == uResult )
	{
		if( m_pSrcDevice->GetInterface() >= 9.0 )
		{
			strcpy( m_strVertexShader1, _T("vs.1.1								// version instruction \n" \
											"dcl_position	v0					\n" \
											"dcl_texcoord	v8					\n" \
											"dp4     oPos.x , v0   , c4         // emit projected x position\n" \
											"dp4     oPos.y , v0   , c5         // emit projected y position\n" \
											"dp4     oPos.z , v0   , c6         // emit projected z position\n" \
											"dp4     oPos.w , v0   , c7         // emit projected w position\n" \
											"mov     oT0.xy , v8                // copy texture coordinates to output\n" \)
						);
		}
		else
		{
			strcpy( m_strVertexShader1, _T("vs.1.0								// version instruction \n" \
											"dp4     oPos.x , v0   , c4         // emit projected x position\n" \
											"dp4     oPos.y , v0   , c5         // emit projected y position\n" \
											"dp4     oPos.z , v0   , c6         // emit projected z position\n" \
											"dp4     oPos.w , v0   , c7         // emit projected w position\n" \
											"mov     oT0.xy , v8                // copy texture coordinates to output\n" \)
						);
		}
	}

	return uResult;
}

void CCreateVertexShader::onActivity()
{
	HRESULT hResult = D3D_OK;
	for( int i = 0; i < m_iCalls; i++ )
	{
		try
		{
			LPD3DXBUFFER pShader = NULL;	// the assembled shader

			if( m_pSrcDevice->GetInterface() < 9.0 )
			{
				//shader decl
				DWORD dwDecl[] =
				{
					D3DVSD_STREAM(0),
					D3DVSD_REG(D3DVSDE_POSITION,	D3DVSDT_FLOAT3 ),			// D3DVSDE_POSITION, 0  
					D3DVSD_REG(D3DVSDE_TEXCOORD1,	D3DVSDT_FLOAT2 ),			// D3DVSDE_TEXCOORD1, 8
					D3DVSD_END()
				};
				m_pVertexShader = (CVertexShader*)(LONG_PTR)0xcdcdcdcd;
				hResult = D3DXAssembleShader8( m_strVertexShader1, strlen(m_strVertexShader1) - 1, 0, NULL, &pShader, NULL );
				if( SUCCEEDED( hResult ) )
					hResult = m_pSrcDevice->CreateVertexShader( dwDecl, (DWORD*)pShader->GetBufferPointer(), &m_pVertexShader, 0 );

				RELEASE( pShader );
				if( m_pVertexShader && (CVertexShader*)(LONG_PTR)0xcdcdcdcd != m_pVertexShader )
				{
					m_pSrcDevice->DeleteVertexShader( m_pVertexShader );
				}
			}
			else
			{
				m_pVertexShader = (CVertexShader*)(LONG_PTR)0xcdcdcdcd;
				hResult = D3DXAssembleShader9( m_strVertexShader1, strlen(m_strVertexShader1) - 1, NULL, NULL, 0, &pShader, NULL );
				if( SUCCEEDED( hResult ) )
					hResult = m_pSrcDevice->CreateVertexShader( (DWORD*)pShader->GetBufferPointer(), &m_pVertexShader );

				RELEASE( pShader );
				if( m_pVertexShader && (CVertexShader*)(LONG_PTR)0xcdcdcdcd != m_pVertexShader )
				{
					RELEASE( m_pVertexShader );
				}
			}
		}
		catch( HRESULT hError )
		{
			WriteToLog( _T("CCreateVertexShader::onActivity - Unhandled exception caught: 0x%x (%s)"), hError, m_pD3D->HResultToString(hError) );
			Fail();
		}
	}
}


//
// CCreatePixelShader - calls CreatePixelShader
//

CCreatePixelShader::CCreatePixelShader() :
	m_pPixelShader( NULL )
{
	m_szTestName	= _T("MemChecker-CreatePixelShader");
	m_szCommandKey	= _T("CreatePixelShader");
}

UINT CCreatePixelShader::TestInitialize()
{
	UINT uResult = CMemChecker::TestInitialize();
	if( D3DTESTINIT_RUN == uResult )
	{
		if( m_pSrcDevice->GetInterface() >= 9.0 )
		{
			strcpy( m_strPixelShader1,	_T("ps.1.1 \n" \
										"tex t0 \n" \
										"tex t1 \n" \
										"mov r1, t1 \n" \
										"lrp r0, v0, t0, r1 \n")
					);
			}
		else
		{
			strcpy( m_strPixelShader1,	_T("ps.1.0 \n" \
										"tex t0 \n" \
										"tex t1 \n" \
										"mov r1, t1 \n" \
										"lrp r0, v0, t0, r1 \n")
					);
		}
	}

	return uResult;
}

void CCreatePixelShader::onActivity()
{
	HRESULT hResult = D3D_OK;
	for( int i = 0; i < m_iCalls; i++ )
	{
		try
		{
			LPD3DXBUFFER pShader = NULL;	// the assembled shader

			if( m_pSrcDevice->GetInterface() < 9.0 )
			{
				m_PixelShader = (DWORD_PTR)0xcdcdcdcd;
				hResult = D3DXAssembleShader8( m_strPixelShader1, strlen(m_strPixelShader1) - 1, 0, NULL, &pShader, NULL );
				if( SUCCEEDED( hResult ) )
					hResult = m_pSrcDevice->CreatePixelShader( (DWORD*)pShader->GetBufferPointer(), &m_PixelShader );

				RELEASE( pShader );
				if( m_PixelShader && (DWORD_PTR)0xcdcdcdcd != m_PixelShader )
				{
					m_pSrcDevice->DeletePixelShader( m_PixelShader );
				}
			}
			else
			{
				m_pPixelShader = (CPShader*)(LONG_PTR)0xcdcdcdcd;
				hResult = D3DXAssembleShader9( m_strPixelShader1, strlen(m_strPixelShader1) - 1, NULL, NULL, 0, &pShader, NULL );
				if( SUCCEEDED( hResult ) )
					hResult = m_pSrcDevice->CreatePixelShader( (DWORD*)pShader->GetBufferPointer(), &m_pPixelShader );

				RELEASE( pShader );
				if( m_pPixelShader && (CPShader*)(LONG_PTR)0xcdcdcdcd != m_pPixelShader )
				{
					RELEASE( m_pPixelShader );
				}
			}
		}
		catch( HRESULT hError )
		{
			WriteToLog( _T("CCreatePixelShader::onActivity - Unhandled exception caught: 0x%x (%s)"), hError, m_pD3D->HResultToString(hError) );
			Fail();
		}
	}
}


//
// CRenderTest - renders a scene from buffers
//

CRenderTest::CRenderTest() :
	m_psBackBuffer( NULL )
{
	m_szTestName	= _T("MemChecker-Render");
	m_szCommandKey	= _T("Render");

	for( int i = 0; i < OBJECTS; i++ )
	{
		m_pShapes[i]			= NULL;
		m_pTextures[i]			= NULL;
		m_pVBs[i]				= NULL;
		m_pIBs[i]				= NULL;
	}

	ReadInteger( _T("Calls"), 256, &m_iCalls );
	m_Retry = 5;
}

UINT CRenderTest::TestInitialize()
{
	UINT uResult = CMemChecker::TestInitialize();
	if( D3DTESTINIT_RUN == uResult )
	{
		CS_SHAPES shapes[] = {
								CS_SPHERE,
								CS_PYRAMID,
								CS_CYLINDER,
								CS_CONE,
								CS_MESH,
								CS_BOID,
								CS_ICOSOHEDRON,
								CS_TEAPOT
							};
	
		// Seed the random-number generator
		srand( (unsigned)666 );

		int i;
		for(i = 0; i < OBJECTS; i++ )
		{
			ASSERT( NULL == m_pShapes[i] );
			m_pShapes[i] = new CShapes();
			if( NULL == m_pShapes[i] )
			{
				WriteToLog( _T( "Failed to allocate memory\n" ) );
				throw E_OUTOFMEMORY;
			}

			CS_SHAPES eShape;
			eShape = ( 0 == i || OBJECTS == i+1 )? CS_BOX:shapes[i%(sizeof(shapes) / sizeof CS_SHAPES)];
			m_pShapes[i]->NewShape(eShape, 8, 8);

			// floor
			if( 0 == i )
			{
				m_pShapes[i]->Scale(15,1,15);
			}
			// sky
			else if( OBJECTS == i+1 )
			{
				m_pShapes[i]->Scale(15,1,15);
				m_pShapes[i]->Translate(0,10,0);
			}
			// flying objects
			else
			{
				m_pShapes[i]->Scale(2);
				m_pShapes[i]->Translate((float)(rand()%20 - 10), (float)(rand()%10), (float)(rand()%10));
			}

			m_pShapes[i]->Rotate(0.0f, 0.5f, 0.0f);

			m_vSpeed[i] = (10+rand()%100)/100.0f;
		}
	}

	SetDefaultMatrices();
	SetDefaultMaterials();
	SetDefaultLights();
	SetDefaultRenderStates();

	GetBackBuffer();
	CreateBuffers();

	return uResult;
}

void CRenderTest::onDestroy()
{
	for( int i = 0; i < OBJECTS; i++ )
	{
		RELEASE(m_pTextures[i]);
		RELEASE(m_pVBs[i]);
		RELEASE(m_pIBs[i]);
	}

	RELEASE(m_psBackBuffer);
}

bool CRenderTest::TestTerminate()
{
	onDestroy();
	for( int i = 0; i < OBJECTS; i++ )
	{
		SAFEDELETE(m_pShapes[i]);
	}

	return CMemChecker::TestTerminate();
}

void CRenderTest::onActivity()
{
	try
	{
		for( int i = 0; i < m_iCalls; i++ )
		{
			FillBuffers();
			onDraw();
		}
	}
	catch(HRESULT hError)
	{
		WriteToLog( _T("Unhandled exception caught: 0x%x (%s)"), hError, m_pD3D->HResultToString(hError) );
		WriteToLog( _T("Texture Format is probably not supported. We will not fail the test") );
	}
}

void CRenderTest::GetBackBuffer()
{
	ASSERT( NULL == m_psBackBuffer );
	Check_HRFail( m_pSrcDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_psBackBuffer), "GetBackBuffer" );
	Check_HRFail( m_psBackBuffer->GetDesc( &m_sdBackBuffer ), "GetDesc" );
}

void CRenderTest::CreateBuffers()
{
	for( int i = 0; i < OBJECTS; i++ )
	{
		Check_HRFail( m_pSrcDevice->CreateTexture(	m_vecSize[m_indexSize].x,
													m_vecSize[m_indexSize].y,
													0,
													0,
													m_vecFormat[m_indexFormat],
													POOL_MANAGED,
													&m_pTextures[i] ), "CreateTexture" );

		if( m_pSrcDevice->GetInterface() < 9.0 )
			Check_HRFail( m_pSrcDevice->D3DXFillTexture(m_pTextures[i], (LPD3DXFILL2D)FillTexture8, &i), "D3DXFillTexture" );
		else
			Check_HRFail( m_pSrcDevice->D3DXFillTexture(m_pTextures[i], (LPD3DXFILL2D)FillTexture9, &i), "D3DXFillTexture" );

		if( FAILED ( m_pSrcDevice->CreateVertexBuffer( m_pShapes[i]->m_nVertices * sizeof FVFVERTEX,
														USAGE_DYNAMIC,
														FVFVERTEXHANDLE,
														POOL_DEFAULT,
														&m_pVBs[i] )
					)
		  )
		{
			Check_HRFail( m_pSrcDevice->CreateVertexBuffer( m_pShapes[i]->m_nVertices * sizeof FVFVERTEX,
														NULL,
														FVFVERTEXHANDLE,
														POOL_MANAGED,
														&m_pVBs[i] ), "CreateVertexBuffer" );
		}
		Check_HRFail( m_pSrcDevice->CreateIndexBuffer(	m_pShapes[i]->m_nIndices * sizeof WORD,
														NULL,
														FMT_INDEX16,
														POOL_SYSTEMMEM,
														&m_pIBs[i] ), "CreateIndexBuffer" );

		// copy indices
		WORD *pwIndices;
		DWORD *pdwIndices;
		Check_HRFail( m_pIBs[i]->Lock(0, 0, (BYTE**)&pwIndices, 0), "Lock" );
		memcpy( pwIndices, m_pShapes[i]->m_pIndices, m_pShapes[i]->m_nIndices * sizeof WORD );
		Check_HRFail( m_pIBs[i]->Unlock(), "Unlock" );
	}
}

void CRenderTest::FillBuffers()
{
	FVFVERTEX *pVertices;

	for( int i = 0; i < OBJECTS; i++ )
	{
		// rotate the shape
		m_pShapes[i]->Rotate(0.0f, 0.5f, 0.0f);

		// and translate it
		if( 0 != i && OBJECTS != i+1 )
		{
			m_pShapes[i]->Translate(0, m_vSpeed[i], 0);
			if( m_pShapes[i]->m_pVertices[0].y > 10 || m_pShapes[i]->m_pVertices[0].y < 0 )
				m_vSpeed[i] = -m_vSpeed[i];
		}
		
		// copy vertices
		if( FAILED( m_pVBs[i]->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
		{
			Check_HRFail( m_pVBs[i]->Lock( 0, 0, (BYTE**)&pVertices, LOCK_DISCARD ), "Lock" );
		}
		memcpy( pVertices, m_pShapes[i]->m_pVertices, m_pShapes[i]->m_nVertices * sizeof FVFVERTEX );
		Check_HRFail( m_pVBs[i]->Unlock(), "Unlock" );
	}
}

void CRenderTest::onDraw()
{
	try
	{
		// Test the cooperative level to see if it's okay to render
		Check_HRFail( m_pSrcDevice->TestCooperativeLevel(), "TestCooperativeLevel" );
	
		Check_HRFail( m_pSrcDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, RGBA_MAKE(128,128,128,255), 1.0f, 0L ), "Clear" );

		// Begin the scene
		Check_HRFail( m_pSrcDevice->BeginScene(), "BeginScene" );

		// draw the scene
		Check_HRFail( m_pSrcDevice->SetVertexShader(FVFVERTEXHANDLE), "SetFVF" );
		for( int i = 0; i < OBJECTS; i++ )
		{
			if( m_pTextures[i] )
			{
				Check_HRFail( m_pSrcDevice->SetTexture(0, m_pTextures[i]), "SetTexture" );
				Check_HRFail( m_pSrcDevice->SetStreamSource(0, m_pVBs[i], sizeof FVFVERTEX), "SetStreamSource" );
				Check_HRFail( m_pSrcDevice->SetIndices(m_pIBs[i], 0), "SetIndices" );
				Check_HRFail( m_pSrcDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_pShapes[i]->m_nVertices, 0, m_pShapes[i]->m_nIndices/3), "DrawIndexedPrimitive" );
				Check_HRFail( m_pSrcDevice->SetTexture( 0, NULL ), "SetTexture" );
			}
		}

        // End the scene
		Check_HRFail( m_pSrcDevice->EndScene(), "EndScene" );

		// Show the frame on the primary surface.
		Check_HRFail( m_pSrcDevice->Present(NULL, NULL, NULL, NULL), "Present" );
	}
	catch( HRESULT hError )
	{
		WriteToLog( _T("Unhandled exception caught: 0x%x (%s)"), hError, m_pD3D->HResultToString(hError) );
	}
}

bool CRenderTest::SetDefaultMatrices()
{
	D3DXMATRIX		Matrix;
	D3DXVECTOR3		from(0.0f, 5.0f, -25.0f), at(0.0f, 5.0f, 0.0f), up(0.0f, 1.0f, 0.0f); 
	float			fAspect = (float)m_pCurrentMode->nBackBufferWidth/(float)m_pCurrentMode->nBackBufferHeight;
	float			fZn = 0.0f;
	float			fZf = 1.0f;

	// Projection matrix
	D3DXMatrixPerspectiveFovLH(&Matrix, pi/4, fAspect, fZn, fZf); 
	if( FAILED( m_pSrcDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&Matrix)) )
		return false;

	// World matrix
	D3DXMatrixIdentity(&Matrix);
	if( FAILED( m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix)) )
		return false;

	// View matrix
	D3DXMatrixLookAtLH(&Matrix, &from, &at, &up);
	if( FAILED( m_pSrcDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&Matrix)) )
		return false;
	
	return true;
}

bool CRenderTest::SetDefaultMaterials()
{
	ZeroMemory(&m_sMaterial,sizeof(MATERIAL));
	m_sMaterial.diffuse.r  = 1.0f;
	m_sMaterial.diffuse.g  = 1.0f;
	m_sMaterial.diffuse.b  = 1.0f;
	m_sMaterial.diffuse.a  = 1.0f;
	
	m_sMaterial.ambient.r  = 1.0f;
	m_sMaterial.ambient.g  = 1.0f;
	m_sMaterial.ambient.b  = 1.0f;
	m_sMaterial.ambient.a  = 1.0f;
	
	m_sMaterial.specular.r = 1.0f;
	m_sMaterial.specular.g = 1.0f;
	m_sMaterial.specular.b = 1.0f;
	m_sMaterial.specular.a = 1.0f;

	m_sMaterial.emissive.r = 0.0f;
	m_sMaterial.emissive.g = 0.0f;
	m_sMaterial.emissive.b = 0.0f;
	m_sMaterial.emissive.a = 0.0f;
	
	m_sMaterial.power      = 20.0f;
	
	if( FAILED(m_pSrcDevice->SetMaterial(&m_sMaterial)) )
		return false;
	
	return true;
}

bool CRenderTest::SetDefaultLights()
{
	D3DVECTOR	vLightPos;
	D3DVECTOR	vLightDir;
	
	vLightPos.x = 0.f; vLightPos.y = 2.f; vLightPos.z = -2.f;
	vLightDir.x = 0.f; vLightDir.y = -0.707f; vLightDir.z = 0.707f;

	ZeroMemory(&m_sLight,sizeof(LIGHT));
	m_sLight.dltType		= D3DLIGHT_SPOT;
	m_sLight.Diffuse.r		= 0.9f;
	m_sLight.Diffuse.g		= 0.7f;
	m_sLight.Diffuse.b		= 0.5f;
	m_sLight.Diffuse.a		= 1.0f;
	m_sLight.Specular.r		= 0.3f;
	m_sLight.Specular.g		= 0.5f;
	m_sLight.Specular.b		= 0.9f;
	m_sLight.Specular.a		= 1.0f;
	m_sLight.Position		= vLightPos;
	m_sLight.Direction		= vLightDir;
	m_sLight.Range			= 1000.f;
	m_sLight.Attenuation0	= 0.7f;
	m_sLight.Attenuation1	= 0.0f;
	m_sLight.Attenuation2	= 0.0f;
	m_sLight.Theta			= pi/10;
	m_sLight.Phi			= pi/2;

	if( FAILED(m_pSrcDevice->SetLight(0,&m_sLight)) )
		return false;
	if( FAILED( m_pSrcDevice->LightEnable(0,TRUE)) )
		return false;
	
	return true;
}

bool CRenderTest::SetDefaultRenderStates()
{
	m_pSrcDevice->SetRenderState(D3DRS_AMBIENT, RGBA_MAKE(128, 192, 255, 255) );
	m_pSrcDevice->SetRenderState(D3DRS_LOCALVIEWER,(DWORD)TRUE);
	m_pSrcDevice->SetRenderState(D3DRS_CLIPPING, (DWORD)TRUE);
	m_pSrcDevice->SetRenderState(D3DRS_LASTPIXEL, (DWORD)TRUE);
	m_pSrcDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
	m_pSrcDevice->SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	m_pSrcDevice->SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_ZERO);
	m_pSrcDevice->SetRenderState(D3DRS_LIGHTING, (DWORD)TRUE);
	m_pSrcDevice->SetRenderState(D3DRS_NORMALIZENORMALS, (DWORD)TRUE);
	//m_pSrcDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
	//m_pSrcDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_MIRROR);
	m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	return true;
}
