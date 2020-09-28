//////////////////////////////////////////////////////////////////////
// File:  CMultiHeadConf.cpp
//
// Copyright (c) Microsoft Corporation
//
// Purpose:
// Test cases for Direct3D multi-head devices.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

#include "CMultiHeadConf.h"
#include <CTextures.h>
#include <CBuffers.h>
#include <CSwapChain.h>
#include <math.h>
#include <d3d9.h>
#include <d3dx9math.h>
#include "resource.h"
#include <strsafe.h>

#pragma prefast(disable: 22109)
#pragma prefast(disable: 22110)
#pragma prefast(disable: 22111)
#pragma prefast(disable: 22112)


/////////////////////////////////////////////////////////////////
//
// Type definitions
//
/////////////////////////////////////////////////////////////////

typedef std::basic_string<TCHAR> tstring;


///////////////////////////////////////////////////////
// Struct:  Vertex
//
// Purpose:
// Holds vertex data, including vertex coordinates (x,
// y, z) and texture coordinates (u, v).
///////////////////////////////////////////////////////
struct Vertex
{
	Vertex
	(
		float x = 0.0f,
		float y = 0.0f,
		float z = 0.0f,
		float u = 0.0f,
		float v = 0.0f
	)
	:
		x(x),
		y(y),
		z(z),
		u(u),
		v(v)
	{
	}

	// Position.
	float x, y, z;

	// Texture coordinates.
	float u, v;
};


/////////////////////////////////////////////////////////////////
//
// Global variables
//
/////////////////////////////////////////////////////////////////

// Framework Instance
CD3DWindowFramework gApp;

// Test Groups
CMultiHeadConf                  confGroup;
CMultiHeadConfCopy              confCopyGroup;
CMultiHeadConfBBCount           confBBCountGroup;
CMultiHeadConfBBLock            confBBLockGroup;
CMultiHeadConfDiffIntvl         confDiffIntvlGroup;
CMultiHeadConfDiffRefrsh        confDiffRefrshGroup;
CMultiHeadConfNoZ               confNoZGroup;
CMultiHeadConfNoZDiffModes      confNoZDiffModesGroup;
CMultiHeadCursor                confCursorGroup;


/////////////////////////////////////////////////////////////////
//
// CMultiHeadConf Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CMultiHeadConf
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CMultiHeadConf::CMultiHeadConf()
{
	//
	// Set framework test parameters.
	//

	SetTestType(TESTTYPE_CONF);
	m_szTestName = _T("Multi-Head Conformance Flip Test");
	m_szCommandKey = _T("CMultiHeadConf");

	m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
	m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_SWVP | DEVICECREATE_ADAPTERGROUP_DEVICE;
	m_Options.D3DOptions.dwSwapEffect = SWAPEFFECT_FLIP;
	m_Options.D3DOptions.fMinDXVersion = 9.0f;
	m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_SWVP;

	m_Options.ModeOptions.uMaxDisplayModes = 100;


	//
	// Initialize our shapes.
	//

	// Define 2D-face geometry by number of sides.
	m_nSides = 12;
	m_nVertices = 2 * (m_nSides + 1);
	m_fStep = 1.0f / float( m_nSides );

	// Initialize our cylinder.
	const float fTheta = 2.0f * pi * m_fStep;
	const float fCos = cos( fTheta );
	const float fSin = sin( fTheta );

	float fX = 1.0f, fY = 0.0f;

	for( int i=0; i <= m_nSides; i++ )
	{
		m_shape1.push_back( D3DXVECTOR2( fX, fY ) );

		float x = fX * fCos - fY * fSin;
		float y = fX * fSin + fY * fCos;

		fX = x;
		fY = y;
	}

	// Initialize our block.
	m_shape2.push_back( D3DXVECTOR2( 1.0f, 0.4f ) );
	m_shape2.push_back( D3DXVECTOR2( 0.4f, 0.4f ) );
	m_shape2.push_back( D3DXVECTOR2( 0.4f, 1.0f ) );
	m_shape2.push_back( D3DXVECTOR2( -0.4f, 1.0f ) );
	m_shape2.push_back( D3DXVECTOR2( -0.4f, 0.4f ) );
	m_shape2.push_back( D3DXVECTOR2( -1.0f, 0.4f ) );
	m_shape2.push_back( D3DXVECTOR2( -1.0f, -1.0f ) );
	m_shape2.push_back( D3DXVECTOR2( -0.4f, -1.0f ) );
	m_shape2.push_back( D3DXVECTOR2( -0.4f, -0.4f ) );
	m_shape2.push_back( D3DXVECTOR2( 0.4f, -0.4f ) );
	m_shape2.push_back( D3DXVECTOR2( 0.4f, -1.0f ) );
	m_shape2.push_back( D3DXVECTOR2( 1.0f, -1.0f ) );
	m_shape2.push_back( D3DXVECTOR2( 1.0f, 0.4f ) );
}


///////////////////////////////////////////////////////
// Function:  TestInitialize
//
// Purpose:
// Per-test-group initialization.
///////////////////////////////////////////////////////
UINT CMultiHeadConf::TestInitialize()
{
	UINT uResult = CMultiHead::TestInitialize();

	if(uResult == D3DTESTINIT_RUN)
	{
		SetTestRange(1,4);
	}

	return uResult;
}


///////////////////////////////////////////////////////
// Function:  ExecuteTest
//
// Purpose:
// Executes a test case.
//
// Details:
// This test defines two 2D shapes, a cylinder and a
// block, and interpolates a 3D extruded figure between
// them.  This provides two images (same or different)
// that can be drawn in each swapchain.
///////////////////////////////////////////////////////
bool CMultiHeadConf::ExecuteTest
(
	UINT uTest
)
{
	m_bDifferent = ( uTest == 2 );         // different images in each swapchain
	m_bPresent = ( uTest == 3 );           // single device present
	m_bDrawPrimitive = ( uTest != 4 );     // use DrawPrimitive instead of RenderPrimitive


	//
	// Build the test case name.
	//

	TCHAR TestCaseName[256] = _T( "" );
	if ( m_bDifferent == true )
	{
		StringCchPrintf
		(
			TestCaseName,
			ARRAYSIZE( TestCaseName ),
			TEXT("Different ")
		);
	}
	else
	{
		StringCchPrintf
		(
			TestCaseName,
			ARRAYSIZE( TestCaseName ),
			TEXT("Same ")
		);
	};

	StringCchCat
	(
		TestCaseName,
		ARRAYSIZE( TestCaseName ),
		TEXT("geometry and ")
	);

	if ( m_bPresent == true )
	{
		StringCchCat
		(
			TestCaseName,
			ARRAYSIZE( TestCaseName ),
			TEXT("device, ")
		);
	}
	else
	{
		StringCchCat
		(
			TestCaseName,
			ARRAYSIZE( TestCaseName ),
			TEXT("swap chain, ")
		);
	};

	if ( m_bDrawPrimitive == true )
	{
		StringCchCat
		(
			TestCaseName,
			ARRAYSIZE( TestCaseName ),
			TEXT("DrawIndexedPrimitiveUP and ")
		);
	}
	else
	{
		StringCchCat
		(
			TestCaseName,
			ARRAYSIZE( TestCaseName ),
			TEXT("RenderPrimitive and ")
		);
	};

	StringCchCat
	(
		TestCaseName,
		ARRAYSIZE( TestCaseName ),
		TEXT("Present")
	);


	//
	// Begin the test case.
	//

	BeginTestCase
	(
		TestCaseName,
		uTest
	);


	if ( NULL == m_pRefDevice )
	{
		Skip();
		return false;
	}

	TEST_RESULT TestResult = DoMultiHeadTest();

	switch (TestResult)
	{
		case RESULT_CONTINUE:
		{
			return true;
		} break;

		case RESULT_PASS:
		{
			Pass();
		} break;

		case RESULT_FAIL:
		{
			Fail();
		} break;

		case RESULT_SKIP:
		{
			Skip();
		} break;

		case RESULT_ABORT:
		{
			Abort();
		} break;
	};


	return false;
}


///////////////////////////////////////////////////////
// Function:  DoMultiHeadTest
//
// Purpose:
// Helper function to do the heavy lifting in running
// a test case.
///////////////////////////////////////////////////////
TEST_RESULT CMultiHeadConf::DoMultiHeadTest()
{
	TEST_RESULT TestResult = RESULT_FAIL;
	CnVertexBuffer  *pVertexBuffer = NULL;
    CnSurface       *pRenderTarget = NULL;
	CnTexture       *pSrcTexture = NULL;
	CnSurface       *pDepthStencil = NULL;
	CnSwapChain     *pSwapChain = NULL;
	CnSurface       *pSurface = NULL;

	LONG uSrcCount = GetRefCount( m_pSrcDevice );
	LONG uRefCount = GetRefCount( m_pRefDevice );

	std::vector<Vertex> vertices;

	if ( !m_bDrawPrimitive )
	{
		vertices.reserve( m_nVertices );
	}

	const int nSteps = 1; //25;
	const float el = D3DXToRadian( 35.0f );
	const float ra = 2.9f;

	UINT nSwapChains = GetNumberOfSwapChains();

	for(int n=0; n<nSteps; n++)
	{
		const float az = float( n ) / float( nSteps ) * 8.0f * pi;
		DWORD dwFlags = CLEARF_TARGET;

		for( m_iSwapChain = 0; m_iSwapChain < nSwapChains; ++m_iSwapChain )
		{
			LONG uCount = GetRefCount( m_pSrcDevice );
			if ( uCount != uSrcCount )
			{
				WriteToLog
				(
					TEXT("Source device reference count %d differs from expected reference count %d."),
					uSrcCount,
					uCount
				);

				goto Cleanup;
			};

			uCount = GetRefCount( m_pRefDevice );
			if ( uCount != uRefCount )
			{
				WriteToLog
				(
					TEXT("Reference device reference count %d differs from expected reference count %d."),
					uRefCount,
					uCount
				);

				goto Cleanup;
			};

			PRESENT_PARAMETERS *pSrcParams = m_pSrcDevice->GetPresParams();
			PRESENT_PARAMETERS *pRefParams = m_pRefDevice->GetPresParams();

			if
			(
				pSrcParams[m_iSwapChain].uWidth != pRefParams[0].uWidth || 
				pSrcParams[m_iSwapChain].uHeight != pRefParams[0].uHeight ||
				pSrcParams[m_iSwapChain].BackBufferFormat.d3dfFormat != pRefParams[0].BackBufferFormat.d3dfFormat
			)
			{
				// Ref has only one swap chain, so if ref[0] is different
				// from source[m_iSwapChain], we need to create a new
				// ref device to match.
				PRESENT_PARAMETERS params[2];
				memcpy( &params[ 0 ], &pSrcParams[ m_iSwapChain ], sizeof(PRESENT_PARAMETERS) );
				params[ 0 ].bWindowed = true;
				params[ 0 ].SwapEffect = 0x40000000;
				params[ 0 ].hDeviceWindow = pRefParams->hDeviceWindow;
				params[ 0 ].uFullscreenRefresh = 0;

				memcpy( &params[ 1 ], &params[ 0 ], sizeof(PRESENT_PARAMETERS) );
    
				UINT Adapter = m_pRefDevice->GetAdapterID();
				DWORD DeviceType = m_pRefDevice->GetDevType();
				DWORD BehaviorFlags = m_pRefDevice->GetBehaviors();
				m_pRefDevice->ReleaseD3D();
				RELEASE(m_pRefDevice);
				m_DeviceList[--m_uDeviceList] = m_pRefDevice;
				HRESULT hr = m_pD3D->CreateDevice
				(
					Adapter, 
					DeviceType,
					m_pFramework->m_hWindow, 
					BehaviorFlags, 
					params,
					&m_pRefDevice
				);

				if ( FAILED( hr ) )
				{
					WriteToLog
					(
						TEXT("Failed to create ref device.")
					);

					TestResult = RESULT_SKIP;

					GetLastError();
					goto Cleanup;
				};

				m_DeviceList[m_uDeviceList++] = m_pRefDevice;
			}


			//
			// Set the view matrix.
			//

			D3DXMATRIX mat;
			bool bSetViewResult = SetTransform
			(
				D3DTS_VIEW, 
				D3DXMatrixLookAtLH
				(
					&mat,
					&D3DXVECTOR3
					(
						ra * sin( az ) * cos( el ),
						ra * sin( el ),
						ra * cos( az ) * cos( el ) + 0.5f
					),
					&D3DXVECTOR3( 0.0f, 0.0f, 0.5f ),
					&D3DXVECTOR3( 0.0f, 1.0f, 0.0f )
				)
			);

			if ( bSetViewResult != true )
			{
				WriteToLog
				(
					TEXT("SetTransform(D3DTS_VIEW) failed.")
				);

				goto Cleanup;
			};


			//
			// Set the projection matrix.
			//

			bool bSetProjectionResult = SetTransform
			(
				D3DTS_PROJECTION, 
				D3DXMatrixPerspectiveFovLH
				(
					&mat, 
					D3DXToRadian( 60.0f ),
					1.0f,
					0.1f,
					10.0f
				)
			);

			if ( bSetProjectionResult != true )
			{
				WriteToLog
				(
					TEXT("SetTransform(D3DTS_PROJECTION) failed.")
				);

				goto Cleanup;
			};


			//
			// Initialize our geometry.
			//

			float t = ( m_bPresent == true )
				?
					(float) rand() / (float) RAND_MAX
				:
					(float) n / (float) nSteps;


			if ( m_iSwapChain > 0 && m_bDifferent == true )
				t =  1.0f - t;

			Vertex *pVertex;
			if ( m_bDrawPrimitive )
			{    
				bool bCreateVertexBufferResult = CreateVertexBuffer
				(
					m_nVertices * sizeof(Vertex),
					0,
					D3DFVF_XYZ | D3DFVF_TEX1,
					D3DPOOL_DEFAULT,
					&pVertexBuffer
				);

				if ( bCreateVertexBufferResult != true )
				{
					WriteToLog
					(
						TEXT("CreateVertexBuffer failed.")
					);

					goto Cleanup;
				};


				bool bSetStreamSourceResult = SetStreamSource( 0, pVertexBuffer, sizeof(Vertex) );

				if ( bSetStreamSourceResult != true )
				{
					WriteToLog
					(
						TEXT("SetStreamSource failed.")
					);

					goto Cleanup;
				};


				bool bSetFVFResult = SetFVF( D3DFVF_XYZ | D3DFVF_TEX1 );

				if ( bSetFVFResult != true )
				{
					WriteToLog
					(
						TEXT("SetFVF failed.")
					);

					goto Cleanup;
				};


				bool bLockVertexBufferResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

				if ( bLockVertexBufferResult != true )
				{
					WriteToLog
					(
						TEXT("Lock failed.")
					);

					goto Cleanup;
				};
			}
			else
			{
				pVertex = &*vertices.begin();
			}

			// linear interpolation between the two shapes
			float fU = 0.0f;
			for( int j=0; j <= m_nSides; j++ )
			{
				D3DXVECTOR2 v1 = m_shape1[j], v2 = m_shape2[j], v3;
				D3DXVec2Add( &v3, D3DXVec2Scale(&v1, &v1, 1.0f - t), D3DXVec2Scale(&v2, &v2, t) );

				*pVertex++ = Vertex( v3.x, v3.y, 1.0f, fU, 1.0f );
				*pVertex++ = Vertex( v3.x, v3.y, 0.0f, fU, 0.0f );

				fU += m_fStep;
			}

			if ( m_bDrawPrimitive )
			{
				bool bUnlockVertexBufferResult = pVertexBuffer->Unlock();

				if ( bUnlockVertexBufferResult != true )
				{
					WriteToLog
					(
						TEXT("Unlock failed.")
					);

					goto Cleanup;
				};
			}


			//
			// Create our bitmap texture.
			//

			bool bCreateTextureResult = D3DXCreateTextureFromResourceA
			(
				NULL,
				MAKEINTRESOURCE(IDB_BITMAP1),
				&pSrcTexture
			);

			if ( bCreateTextureResult != true )
			{
				WriteToLog
				(
					TEXT("D3DXCreateTextureFromResource failed.")
				);

				goto Cleanup;
			};


			//
			// Set render states.
			//

			bool bSetLightingResult = SetRenderState( D3DRS_LIGHTING, FALSE );

			if ( bSetLightingResult != true )
			{
				WriteToLog
				(
					TEXT("SetRenderState(D3DRS_LIGHTING, FALSE) failed.")
				);

				goto Cleanup;
			};


			bool bSetCullModeResult = SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

			if ( bSetCullModeResult != true )
			{
				WriteToLog
				(
					TEXT("SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE) failed.")
				);

				goto Cleanup;
			};


			//
			// Set the bitmap texture as the current active texture.
			//

			bool bSetTextureResult = SetTexture( 0, pSrcTexture );

			if ( bSetTextureResult != true )
			{
				WriteToLog
				(
					TEXT("SetTexture failed.")
				);

				goto Cleanup;
			};


			//
			// Set texture stage states.
			//

			bool bSetColorOpResult = SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

			if ( bSetColorOpResult != true )
			{
				WriteToLog
				(
					TEXT("SetTextureStateState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1) failed.")
				);

				goto Cleanup;
			};


			bool bSetColorArgResult = SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

			if ( bSetColorArgResult != true )
			{
				WriteToLog
				(
					TEXT("SetTextureStateState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE) failed.")
				);

				goto Cleanup;
			};


			bool bSetAlphaOpResult = SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

			if ( bSetAlphaOpResult != true )
			{
				WriteToLog
				(
					TEXT("SetTextureStageState(0, D3DTSS_ALPHOP, D3DTOP_DISABLE) failed.")
				);

				goto Cleanup;
			};


			//
			// Set sampler states.
			//

			bool bSetMagFilterResult = SetSamplerState(0, SAMP_MAGFILTER, TEXF_LINEAR);

			if ( bSetMagFilterResult != true )
			{
				WriteToLog
				(
					TEXT("SetSamplerState(0, SAMP_MAGFILTER, TEXF_LINEAR) failed.")
				);

				goto Cleanup;
			};


			bool bSetMinFilterResult = SetSamplerState(0, SAMP_MINFILTER, TEXF_LINEAR);

			if ( bSetMinFilterResult != true )
			{
				WriteToLog
				(
					TEXT("SetSamplerState(0, SAMP_MINFILTER, TEXF_LINEAR) failed.")
				);

				goto Cleanup;
			};


			//
			// Get the current render target.
			//

			bool bGetRenderTargetResult = GetRenderTarget( &pRenderTarget );

			if ( bGetRenderTargetResult != true )
			{
				WriteToLog
				(
					TEXT("GetRenderTarget failed.")
				);

				goto Cleanup;
			};


			//
			// Get the depth stencil for this swap chain.
			//

			bool bGetDepthStencilResult = GetDepthStencil( m_iSwapChain, &pDepthStencil );

			if ( bGetDepthStencilResult != true )
			{
				WriteToLog
				(
					TEXT("GetDepthStencil failed.")
				);

				goto Cleanup;
			};


			//
			// Get the swap chain.
			//

			if ( m_iSwapChain == 0 || !m_Options.D3DOptions.bReference )
			{
				bool bGetSwapChainResult = GetSwapChain( m_iSwapChain, &pSwapChain );

				if ( bGetSwapChainResult != true )
				{
					WriteToLog
					(
						TEXT("GetSwapChain failed.")
					);

					goto Cleanup;
				};
			}
			else
			{
				// REF device has only one swapchain so we need to do some handy-work here
				pSwapChain = new CnSwapChain;
				if ( pSwapChain == NULL )
				{
					WriteToLog
					(
						TEXT("Failed to allocate new swap chain object.")
					);

					goto Cleanup;
				};


				bool bInitSwapChainResult = pSwapChain->Init(nSwapChains);

				if ( bInitSwapChainResult != true )
				{
					WriteToLog
					(
						TEXT("Failed to initialize swap chain object.")
					);

					goto Cleanup;
				};


				LPSWAPCHAIN swapChain;
				HRESULT hGetSwapChain1Result = m_pSrcDevice->GetSwapChain( 1, &swapChain );

				if ( FAILED( hGetSwapChain1Result ) )
				{
					WriteToLog
					(
						TEXT("GetSwapChain(1) failed.")
					);

					goto Cleanup;
				};


				pSwapChain->SetSwapChain( swapChain, 0 );


				HRESULT hGetSwapChain0Result = m_pRefDevice->GetSwapChain( 0, &swapChain );

				if ( FAILED( hGetSwapChain0Result ) )
				{
					WriteToLog
					(
						TEXT("GetSwapChain(0) failed.")
					);

					goto Cleanup;
				};


				pSwapChain->SetSwapChain( swapChain, 1 );
			}


			//
			// Get the back buffer for the swap chain.
			//

			bool bGetBackBufferResult = pSwapChain->GetBackBuffer
			(
				0,
				BACKBUFFER_TYPE_MONO,
				&pSurface
			);

			if ( bGetBackBufferResult != true )
			{
				WriteToLog
				(
					TEXT("GetBackBuffer failed.")
				);

				goto Cleanup;
			};


			//
			// Set the render target.
			//

			bool bSetRenderTargetResult = SetRenderTarget( DWORD( 0 ), pSurface );

			if ( bSetRenderTargetResult != true )
			{
				WriteToLog
				(
					TEXT("SetRenderTarget failed.")
				);

				goto Cleanup;
			};


			//
			// Set the depth stencil.
			//

			bool bSetDepthStencilSurfaceResult = SetDepthStencilSurface( pDepthStencil );

			if ( bSetDepthStencilSurfaceResult != true )
			{
				WriteToLog
				(
					TEXT("SetDepthStencilSurface failed.")
				);

				goto Cleanup;
			};


			if ( pDepthStencil )
			{
				dwFlags |= CLEARF_ZBUFFER;
			}


			bool bSetZEnableResult = SetRenderState( D3DRS_ZENABLE, pDepthStencil != NULL );

			if ( bSetZEnableResult != true )
			{
				WriteToLog
				(
					TEXT("SetRenderState(D3DRS_ZENABLE) failed.")
				);

				goto Cleanup;
			};


			//
			// Clear.
			//

			bool bClearResult = Clear( 0, NULL, dwFlags, 0x00000032, 1.0f, 0 );

			if ( bClearResult != true )
			{
				WriteToLog
				(
					TEXT("Clear failed.")
				);

				goto Cleanup;
			};


			//
			// Render the scene.
			//

			bool bBeginSceneResult = BeginScene();

			if ( bBeginSceneResult != true )
			{
				WriteToLog
				(
					TEXT("BeginScene failed.")
				);

				goto Cleanup;
			};


			if ( m_bDrawPrimitive )
			{
				bool bDrawPrimitiveResult = DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, m_nVertices - 2 );

				if ( bDrawPrimitiveResult != true )
				{
					WriteToLog
					(
						TEXT("DrawPrimitive failed.")
					);

					goto Cleanup;
				};
			}
			else
			{
				bool bRenderPrimitiveResult = RenderPrimitive
				(
					D3DPT_TRIANGLESTRIP, 
					D3DFVF_XYZ | D3DFVF_TEX1,
					&*vertices.begin(), 
					m_nVertices,
					NULL,
					0,
					NULL
				);

				if ( bRenderPrimitiveResult != true )
				{
					WriteToLog
					(
						TEXT("RenderPrimitive failed.")
					);

					goto Cleanup;
				};
			};


			bool bEndSceneResult = EndScene();

			if ( bEndSceneResult != true )
			{
				WriteToLog
				(
					TEXT("EndScene failed.")
				);

				goto Cleanup;
			};


			// compare if lockable backbuffer set
			if ( m_Options.D3DOptions.bReference && pSrcParams[m_iSwapChain].dwFlags )
			{
			    CSurface *pSrcSurf = NULL;
			    CSurface *pRefSurf = NULL;
			    pSurface->GetSurface(&pSrcSurf, 0);
			    pSurface->GetSurface(&pRefSurf, 1);

			    HRESULT hr = m_pFramework->GetImageCmp()->CompareImages( pSrcSurf, pRefSurf, NULL );

                RELEASE(pSrcSurf);
                RELEASE(pRefSurf);
                
                if (FAILED(hr))
			    {
				    WriteToLog
				    (
					    TEXT("CompareImages failed.")
				    );

				    TestResult = RESULT_FAIL;
				    goto Cleanup;
			    };

			    m_fPassPercentage = m_pFramework->GetImageCmp()->GetCompareResult();
			    if
			    (
				    m_fPassPercentage < m_pFramework->GetImageCmp()->GetPassRequired()
			    )
                {
					WriteToLog
					(
						TEXT("Images on source and reference surfaces differ.")
					);
					TestResult = RESULT_FAIL;
					goto Cleanup;
				}
				else
				{
					TestResult = RESULT_PASS;
				}
			}

			bool bDrawStatsResult = DrawStats
			(
				pSrcParams[m_iSwapChain].uWidth, 
				pSrcParams[m_iSwapChain].uHeight, 
				m_pSrcDevice
			);

			if ( bDrawStatsResult != true )
			{
				WriteToLog
				(
					TEXT("DrawStats failed.")
				);

				goto Cleanup;
			};


			if ( m_bPresent == false )
			{
				CSwapChain *swapChain = NULL;
				HRESULT hGetSwapChainResult = m_pSrcDevice->GetSwapChain
				(
					m_iSwapChain,
					&swapChain
				);

				if ( FAILED( hGetSwapChainResult ) )
				{
					WriteToLog
					(
						TEXT("GetSwapChain failed.")
					);

					goto Cleanup;
				};

				HRESULT hSwapChainPresentResult = swapChain->Present
				(
					NULL,
					NULL,
					NULL,
					NULL
				);

                RELEASE(swapChain);

				if ( FAILED( hSwapChainPresentResult ) )
				{
					WriteToLog
					(
						TEXT("Swap chain Present failed.")
					);

					goto Cleanup;
				};
			}

			bool bRestoreRenderTargetResult = SetRenderTarget
			(
				pRenderTarget,
				pSrcParams->bAutoDepthStencil ? pDepthStencil : NULL
			);

			if ( bRestoreRenderTargetResult != true )
			{
				WriteToLog
				(
					TEXT("SetRenderTarget failed.")
				);

				goto Cleanup;
			};

            RELEASE(pVertexBuffer);
            RELEASE(pRenderTarget);
            RELEASE(pSrcTexture);
            RELEASE(pDepthStencil);
            RELEASE(pSwapChain);
            RELEASE(pSurface);
		}

		if ( m_bPresent == true )
		{
			bool bPresentResult = Present
			(
				NULL,
				NULL,
				NULL,
				NULL,
				CD3D_SRC
			);

			if ( bPresentResult != true )
			{
				WriteToLog
				(
					TEXT("Present failed.")
				);

				goto Cleanup;
			};
		};
	};


	if ( TestResult != RESULT_PASS )
	{
		TestResult = RESULT_CONTINUE;
	};


Cleanup:
    RELEASE(pVertexBuffer);
    RELEASE(pRenderTarget);
    RELEASE(pSrcTexture);
    RELEASE(pDepthStencil);
    RELEASE(pSwapChain);
    RELEASE(pSurface);

	return TestResult;
};


/////////////////////////////////////////////////////////////////
//
// CMultiHeadConfBBCount Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CreatePresentParams
//
// Purpose:
//
///////////////////////////////////////////////////////
HRESULT CMultiHeadConfBBCount::CreatePresentParams
(
	TESTMODE *pMode,
	PRESENT_PARAMETERS **ppParams
)
{
	HRESULT hr = CMultiHeadConf::CreatePresentParams(pMode, ppParams);

	if ( SUCCEEDED( hr ) )
	{
		PRESENT_PARAMETERS *pParams = *ppParams;
		for( int i=0; i<m_nParams; i++ )
		{
			pParams[i].uBackBufferCount = i ? 1 : 3;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////
//
// CMultiHeadConfBBLock Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CreatePresentParams
//
// Purpose:
//
///////////////////////////////////////////////////////
HRESULT CMultiHeadConfBBLock::CreatePresentParams
(
	TESTMODE *pMode,
	PRESENT_PARAMETERS **ppParams
)
{
	HRESULT hr = CMultiHeadConf::CreatePresentParams(pMode, ppParams);

	if ( SUCCEEDED( hr ) )
	{
		PRESENT_PARAMETERS *pParams = *ppParams;
		for( int i=0; i<m_nParams; i++ )
		{
			pParams[i].dwFlags = i ? 0 : i ? 0 : PRESENTFLAG_LOCKABLE_BACKBUFFER;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////
//
// CMultiHeadConfDiffIntvl Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CreatePresentParams
//
// Purpose:
//
///////////////////////////////////////////////////////
HRESULT CMultiHeadConfDiffIntvl::CreatePresentParams
(
	TESTMODE *pMode,
	PRESENT_PARAMETERS **ppParams
)
{
	HRESULT hr = CMultiHeadConf::CreatePresentParams(pMode, ppParams);

	if ( SUCCEEDED( hr ) )
	{
		PRESENT_PARAMETERS *pParams = *ppParams;
		for( int i=0; i<m_nParams; i++ )
		{
			pParams[i].uFullscreenPresentInterval = i ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////
//
// CMultiHeadConfDiffRefrsh Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CreatePresentParams
//
// Purpose:
//
///////////////////////////////////////////////////////
HRESULT CMultiHeadConfDiffRefrsh::CreatePresentParams
(
	TESTMODE *pMode,
	PRESENT_PARAMETERS **ppParams
)
{
	HRESULT hr = CMultiHeadConf::CreatePresentParams(pMode, ppParams);

	if ( SUCCEEDED( hr ) )
	{
		PRESENT_PARAMETERS *pParams = *ppParams;
		for( int i=0; i<m_nParams; i++ )
		{
			if ( i )
			{
				pParams[i].uFullscreenRefresh = 0;
                
			}
			else
			{
				DISPLAYMODE Mode;

				for( UINT j = 0; S_OK == ( hr = m_pD3D->EnumAdapterModes( i, pParams[i].BackBufferFormat, j, &Mode ) ); j++ )
				{
					if ( Mode.dwWidth == pParams[i].uWidth &&
						 Mode.dwHeight == pParams[i].uHeight &&
						 Mode.dwRefreshRate != 60 )
					{
						pParams[i].uFullscreenRefresh = 0;
						break;
					}
				}

				if ( S_OK != hr )
				{
					pParams[i].uFullscreenRefresh = 0;
					hr = S_OK;
				}
			}
		}
	}

	return hr;
}


///////////////////////////////////////////////////////
// Function:  DrawStats
//
// Purpose:
//
///////////////////////////////////////////////////////
bool CMultiHeadConfDiffRefrsh::DrawStats
(
	int nHeight,
	int nWidth,
	CDevice *pDevice
)
{
	// draw stats so mode display is current on both swapchains
	PRESENT_PARAMETERS *pParams = pDevice->GetPresParams();
	m_pCurrentMode->pDisplay->dwRefreshRate = 0;

	bool bResult = CMultiHeadConf::DrawStats( nWidth, nHeight, pDevice );


	return bResult;
}


/////////////////////////////////////////////////////////////////
//
// CMultiHeadConfNoZDiffModes Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CreatePresentParams
//
// Purpose:
//
///////////////////////////////////////////////////////
HRESULT CMultiHeadConfNoZDiffModes::CreatePresentParams
(
	TESTMODE *pMode,
	PRESENT_PARAMETERS **ppParams
)
{
	HRESULT hr = CMultiHeadConfNoZ::CreatePresentParams(pMode, ppParams);

	if ( SUCCEEDED( hr ) )
	{
		PRESENT_PARAMETERS *pParams = *ppParams;
		TESTMODE *pNextMode = pMode;

		for( int i=1; i<m_nParams; i++ )
		{
			// set head to a different mode
			pNextMode = pNextMode->pNext ? pNextMode->pNext : m_pModeList;

			pParams[i].uWidth = pNextMode->pDisplay->dwWidth;				
			pParams[i].uHeight = pNextMode->pDisplay->dwHeight; 				
			pParams[i].BackBufferFormat = pNextMode->pDisplay->Format;
			pParams[i].DesktopFormat = pNextMode->pDisplay->Format;
		}
	}

	return hr;
}


///////////////////////////////////////////////////////
// Function:  DrawStats
//
// Purpose:
//
///////////////////////////////////////////////////////
bool CMultiHeadConfNoZDiffModes::DrawStats
(
	int nHeight,
	int nWidth,
	CDevice *pDevice
)
{
	// draw stats so mode display is current on both swapchains
	TESTMODE *pCurMode = m_pCurrentMode;
	if ( m_iSwapChain )
	{
		m_pCurrentMode = m_pCurrentMode->pNext ? m_pCurrentMode->pNext : m_pModeList;
	}
	bool bResult = CMultiHeadConfNoZ::DrawStats( nWidth, nHeight, pDevice );
	m_pCurrentMode = pCurMode;

	return bResult;
}


/////////////////////////////////////////////////////////////////
//
// CMultiHeadConfNoZDiffModesDB Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  GetDepthStencil
//
// Purpose:
//
///////////////////////////////////////////////////////
bool CMultiHeadConfNoZDiffModesDB::GetDepthStencil
(
	UINT iSwapChain,
	LPSURFACES *pDepthStencil
)
{
	PRESENT_PARAMETERS*pParams = m_pSrcDevice->GetPresParams();

	return CreateDepthStencilSurface
	(
		pParams[iSwapChain].uWidth,
		pParams[iSwapChain].uHeight,
		*GetCurrentZBufferFormat(),
		pParams[iSwapChain].MultiSample,
		pDepthStencil
	);
}


/////////////////////////////////////////////////////////////////
//
// CSurfaceArray implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
template<class T> class CSurfaceArray
{
	public:
		CSurfaceArray(void *base, DWORD stride, DWORD length = 4) :
			m_base(static_cast<BYTE*>(base)),
			m_stride(stride),
			m_length(length)
		{
		}
		T *operator()(unsigned x, unsigned y) const
		{
			return reinterpret_cast<T*>(m_base + y * m_stride + x * m_length);
		}


	private:
		BYTE *m_base;
		DWORD m_stride;
		DWORD m_length;
};


/////////////////////////////////////////////////////////////////
//
// CMultiHeadCursor Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CMultiHeadCursor
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CMultiHeadCursor::CMultiHeadCursor()
{
	SetTestType(TESTTYPE_CONF);
	m_szTestName = _T("Multi-Head Conformance Cursor Test");
	m_szCommandKey = _T("CMultiHeadCursor");

	m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
	m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_SWVP | DEVICECREATE_ADAPTERGROUP_DEVICE;
	m_Options.D3DOptions.dwSwapEffect = SWAPEFFECT_FLIP;
	m_Options.D3DOptions.fMinDXVersion = 9.0f;
	m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_SWVP;

	m_Options.ModeOptions.uMaxDisplayModes = MODEOPT_MAXMODES;
}


///////////////////////////////////////////////////////
// Function:  TestInitialize
//
// Purpose:
//
///////////////////////////////////////////////////////
UINT CMultiHeadCursor::TestInitialize()
{
	UINT uResult = CMultiHead::TestInitialize();

	if(uResult == D3DTESTINIT_RUN)
	{
		SetTestRange(1,1);
	}

	return uResult;
}


///////////////////////////////////////////////////////
// Function:  ExecuteTest
//
// Purpose:
// Executes a test case.
///////////////////////////////////////////////////////
bool CMultiHeadCursor::ExecuteTest
(
	UINT uTest
)
{
	TEST_RESULT TestResult = RESULT_FAIL;
	CSurface    *pSurf = NULL;
	CnSurface   *pRenderTarget = NULL;
	CnSwapChain *pSwapChain = NULL;
	CnSurface   *pSurface = NULL;
	CSwapChain  *swapChain = NULL;
	CSurface    *pBuffer = NULL;
	SURFACEDESC desc;


	//
	// Begin the test.
	//

	BeginTestCase
	(
		_T("Multi-head SetCursor"),
		uTest
	);


	if ( m_pRefDevice == NULL )
	{
		TestResult = RESULT_SKIP;
		goto Cleanup;
	};


	HRESULT hCreateSurfaceResult = m_pSrcDevice->CreateOffscreenPlainSurface
	(
		16,
		16,
		FMT_A8R8G8B8,
		POOL_SCRATCH,
		&pSurf
	);

	if ( FAILED( hCreateSurfaceResult ) )
	{
		WriteToLog
		(
			TEXT("CreateOffscreenPlainSurface failed with error %s."),
			m_pD3D->HResultToString( hCreateSurfaceResult )
		);

		goto Cleanup;
	};


	HRESULT hGetDescResult = pSurf->GetDesc(&desc);

	if ( FAILED( hGetDescResult ) )
	{
		WriteToLog
		(
			TEXT("GetDesc failed with error %s."),
			m_pD3D->HResultToString( hGetDescResult )
		);

		goto Cleanup;
	};


	LOCKED_RECT lockRect1;
	HRESULT hLockRectResult = pSurf->LockRect(&lockRect1, NULL, 0);

	if ( FAILED( hLockRectResult ) )
	{
		WriteToLog
		(
			TEXT("LockRect failed with error %s."),
			m_pD3D->HResultToString( hLockRectResult )
		);

		goto Cleanup;
	};


	{
		CSurfaceArray<unsigned> surf(lockRect1.pBits, lockRect1.iPitch);

		for(unsigned y=0; y < desc.Height; ++y)
		{
			for(unsigned x=0; x < desc.Width; ++x)
			{
				*surf(x, y) = 0xffffffff;
			}
		}
	}


	HRESULT hUnlockRectResult = pSurf->UnlockRect();

	if ( FAILED( hUnlockRectResult ) )
	{
		WriteToLog
		(
			TEXT("UnlockRect failed with error %s."),
			m_pD3D->HResultToString( hUnlockRectResult )
		);

		goto Cleanup;
	};


	HRESULT hr = m_pSrcDevice->SetCursorProperties(desc.Width/2, desc.Height/2, pSurf);

	if ( hr == D3DERR_NOTAVAILABLE )
	{
		GetLastError();
		WriteToLog(TEXT("SetCursorProperties return D3DERR_NOTAVAILABLE\n"));

		TestResult = RESULT_SKIP;
		goto Cleanup;
	}
	else if ( FAILED( hr ) )
	{
		WriteToLog
		(
			TEXT("SetCursorProperties failed with error %s."),
			m_pD3D->HResultToString( hr )
		);

		goto Cleanup;
	};


	UINT nSwapChains = GetNumberOfSwapChains();

	for( m_iSwapChain = 0; m_iSwapChain < nSwapChains; ++m_iSwapChain )
	{
		WriteToLog
		(
			TEXT("Processing swap chain %u."),
			m_iSwapChain
		);


		PRESENT_PARAMETERS *pSrcParams = m_pSrcDevice->GetPresParams();
		PRESENT_PARAMETERS *pRefParams = m_pRefDevice->GetPresParams();

		if
		(
			pSrcParams[m_iSwapChain].uWidth != pRefParams[0].uWidth || 
			pSrcParams[m_iSwapChain].uHeight != pRefParams[0].uHeight ||
			pSrcParams[m_iSwapChain].BackBufferFormat.d3dfFormat != pRefParams[0].BackBufferFormat.d3dfFormat
		)
		{
			// ref has only one swapchain so if ref[0] different from source[m_iSwapChain], then must update it to match
			PRESENT_PARAMETERS params[2];
			memcpy( params, pSrcParams + m_iSwapChain , sizeof(PRESENT_PARAMETERS) );
			params[0].bWindowed = true;
			params[0].SwapEffect = 0x40000000;
			params[0].hDeviceWindow = pRefParams->hDeviceWindow;
			params[0].uFullscreenRefresh = 0;
			memcpy( params + 1, params, sizeof(PRESENT_PARAMETERS) );

			UINT Adapter = m_pRefDevice->GetAdapterID();
			DWORD DeviceType = m_pRefDevice->GetDevType();
			DWORD BehaviorFlags = m_pRefDevice->GetBehaviors();
			m_pRefDevice->ReleaseD3D();
			RELEASE(m_pRefDevice);
			m_DeviceList[--m_uDeviceList] = m_pRefDevice;
			HRESULT hr = m_pD3D->CreateDevice
			(
				Adapter, 
				DeviceType,
				m_pFramework->m_hWindow, 
				BehaviorFlags, 
				params,
				&m_pRefDevice
			);

			if ( FAILED( hr ) )
			{
				WriteToLog
				(
					TEXT("Failed to create reference device.")
				);

				GetLastError();

				TestResult = RESULT_SKIP;
				goto Cleanup;
			}
			m_DeviceList[m_uDeviceList++] = m_pRefDevice;
		}

		bool bGetRenderTargetResult = GetRenderTarget( &pRenderTarget );

		if ( bGetRenderTargetResult != true )
		{
			WriteToLog
			(
				TEXT("GetRenderTarget failed.")
			);

			goto Cleanup;
		};

		if ( m_iSwapChain == 0 || !m_Options.D3DOptions.bReference )
		{
			bool bGetSwapChainResult = GetSwapChain( m_iSwapChain, &pSwapChain );

			if ( bGetSwapChainResult != true )
			{
				WriteToLog
				(
					TEXT("GetSwapChain failed.")
				);

				goto Cleanup;
			};
		}
		else
		{
			// REF device has only one swapchain so we need to do some handy-work here
			pSwapChain = new CnSwapChain;
			pSwapChain->Init(nSwapChains);

			LPSWAPCHAIN tempSwapChain;
			HRESULT hGetSwapChain1Result = m_pSrcDevice->GetSwapChain( 1, &tempSwapChain );

			if ( FAILED( hGetSwapChain1Result ) )
			{
				WriteToLog
				(
					TEXT("GetSwapChain on swap chain 1 failed with error %s."),
					m_pD3D->HResultToString( hGetSwapChain1Result )
				);

				goto Cleanup;
			};

			pSwapChain->SetSwapChain( tempSwapChain, 0 );

			HRESULT hGetSwapChain0Result = m_pRefDevice->GetSwapChain( 0, &tempSwapChain );

			if ( FAILED( hGetSwapChain0Result ) )
			{
				WriteToLog
				(
					TEXT("GetSwapChain on swap chain 0 failed with error %s."),
					m_pD3D->HResultToString( hGetSwapChain0Result )
				);

				goto Cleanup;
			};

			pSwapChain->SetSwapChain( tempSwapChain, 1 );
        }

		bool bGetBackBufferResult = pSwapChain->GetBackBuffer
		(
			0,
			BACKBUFFER_TYPE_MONO,
			&pSurface
		);

		if ( bGetBackBufferResult != true )
		{
			WriteToLog
			(
				TEXT("GetBackBuffer failed.")
			);

			goto Cleanup;
		};


		bool bSetRenderTargetResult = SetRenderTarget( DWORD( 0 ), pSurface );

		if ( bSetRenderTargetResult != true )
		{
			WriteToLog
			(
				TEXT("SetRenderTarget failed.")
			);

			goto Cleanup;
		};


		bool bClearResult = Clear
		(
			0,
			NULL,
			CLEARF_TARGET,
			0x00000032,
			1.0f,
			0
		);

		if ( bClearResult != true )
		{
			WriteToLog
			(
				TEXT("Clear failed.")
			);

			goto Cleanup;
		};


		ShowCursor( FALSE );

		HMONITOR hMonitor = m_pD3D->GetAdapterMonitor( m_Adapters[m_iSwapChain] );
		MONITORINFOEX monitorInfo;
		monitorInfo.cbSize = sizeof(MONITORINFOEX);
		BOOL bGetMonitorInfoResult = GetMonitorInfo( hMonitor, &monitorInfo );

		if ( bGetMonitorInfoResult != TRUE )
		{
			WriteToLog
			(
				TEXT("GetMonitorInfo failed.")
			);

			goto Cleanup;
		};


		//
		// Generate random coordinates on the monitor.
		//

		long x = monitorInfo.rcMonitor.left + (long) ((float) rand() / (float) RAND_MAX * ( (float) monitorInfo.rcMonitor.right - (float) monitorInfo.rcMonitor.left ));
		long y = monitorInfo.rcMonitor.top + (long) ((float) rand() / (float) RAND_MAX * ( (float) monitorInfo.rcMonitor.bottom - (float) monitorInfo.rcMonitor.top ));


		//
		// Display the cursor at those coordinates.
		//

		WriteToLog
		(
			TEXT("Monitor extents:  (%d, %d) to (%d, %d).  Displaying cursor at (%d, %d)."),
			monitorInfo.rcMonitor.left,
			monitorInfo.rcMonitor.top,
			monitorInfo.rcMonitor.right,
			monitorInfo.rcMonitor.bottom,
			x,
			y
		);


		SetCursorPosition( x, y, 0 );
		ShowCursor( TRUE );


		//
		// Get the swap chain.
		//

		HRESULT hGetSwapChainResult = m_pSrcDevice->GetSwapChain( m_iSwapChain, &swapChain );

		if ( FAILED( hGetSwapChainResult ) )
		{
			WriteToLog
			(
				TEXT("GetSwapChain failed with error %s."),
				m_pD3D->HResultToString( hGetSwapChainResult )
			);

			goto Cleanup;
		};


		//
		// Get the swapchain buffer dimensions.  We'll need the dimensions 
		// later to create a compatible system memory surface for a 
		// GetFrontBufferData call.
		//
		CSurface *pbackbuffer;
		swapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pbackbuffer);
		SURFACEDESC desc;

		pbackbuffer->GetDesc(&desc);
		WriteToLog("SwapChain Width: %d   Height: %d", desc.dwWidth, desc.dwHeight);
		pbackbuffer->Release();

		//
		// Get the swap chain's display mode.
		//
		DISPLAYMODE Mode;
		HRESULT hGetDisplayModeResult = m_pSrcDevice->GetDisplayMode( m_iSwapChain, &Mode);
		WriteToLog("SwapChain: %d Mode: %dx%d", m_iSwapChain, Mode.dwWidth, Mode.dwHeight);

		if ( FAILED( hGetDisplayModeResult ) )
		{
			WriteToLog
			(
				TEXT("GetDisplayMode failed with error %s."),
				m_pD3D->HResultToString( hGetDisplayModeResult )
			);

			goto Cleanup;
		};


		//
		// Verify the display mode against our expectations.
		//
		if ( Mode.dwWidth != monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left )
		{
			WriteToLog
			(
				TEXT("Monitor width %d differs from mode width %u."),
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				Mode.dwWidth
			);

			goto Cleanup;
		};

		if ( Mode.dwHeight != monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top )
		{
			WriteToLog
			(
				TEXT("Monitor height %d differs from mode height %u."),
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				Mode.dwHeight
			);

			goto Cleanup;
		};


		//
		// Create our scratch surface.
		//

		HRESULT hCreateSurface2Result = m_pSrcDevice->CreateOffscreenPlainSurface
		(
		desc.Width,
		desc.Height,
			FMT_A8R8G8B8, 
			POOL_SCRATCH, 
			&pBuffer
		);

		if ( FAILED( hCreateSurface2Result ) )
		{
			WriteToLog
			(
				TEXT("CreateOffscreenPlainSurface failed with error %s."),
				m_pD3D->HResultToString( hCreateSurface2Result )
			);

			goto Cleanup;
		};


		//
		// Capture the contents of the front buffer into our scratch buffer.
		//

		WriteToLog
		(
			_T( "Calling GetFrontBuffer." )
		);

		HRESULT hGetFrontBuffer2Result = swapChain->GetFrontBuffer(pBuffer);

		if ( FAILED( hGetFrontBuffer2Result ) )
		{
			WriteToLog
			(
				TEXT("GetFrontBuffer failed with error %s."),
				m_pD3D->HResultToString( hGetFrontBuffer2Result )
			);

			goto Cleanup;
		};


		//
		// Lock the scratch buffer.
		//

		WriteToLog
		(
			_T( "Locking scratch buffer." )
		);

		LOCKED_RECT lockRect2;
		HRESULT hLockRect2Result = pBuffer->LockRect(&lockRect2, NULL, 0);

		if ( FAILED( hLockRect2Result ) )
		{
			WriteToLog
			(
				TEXT("LockRect failed with error %s."),
				m_pD3D->HResultToString( hLockRect2Result )
			);

			goto Cleanup;
		};


		//
		// Scan out the contents of the buffer at the test coordinates.
		//

		WriteToLog
		(
			_T( "Scanning out test pixel." )
		);

		CSurfaceArray<unsigned> buffer(lockRect2.pBits, lockRect2.iPitch);

		unsigned color = *buffer( x - monitorInfo.rcMonitor.left, y - monitorInfo.rcMonitor.top );


		//
		// Unlock the scratch buffer.
		//

		WriteToLog
		(
			_T( "Unlocking scratch buffer." )
		);

		HRESULT hUnlockRect2Result = pBuffer->UnlockRect();

		if ( FAILED( hUnlockRect2Result ) )
		{
			WriteToLog
			(
				TEXT("UnlockRect failed with error %s."),
				m_pD3D->HResultToString( hUnlockRect2Result )
			);

			goto Cleanup;
		};


		//
		// Verify that the test pixel is white.
		//

		WriteToLog
		(
			TEXT("Comparing color at (%d, %d)."),
			x,
			y
		);

		if ( (color & 0x00ffffff) != 0x00ffffff)
		{
			WriteToLog
			(
				TEXT("Color 0x%08x at (%d, %d) does not match expected color of white (0x??ffffff)."),
				color,
				x,
				y
			);

			goto Cleanup;
		};


		//
		// DrawStats.
		//

		bool bDrawStatsResult = DrawStats
		(
			pSrcParams[m_iSwapChain].uWidth, 
			pSrcParams[m_iSwapChain].uHeight, 
			m_pSrcDevice
		);

		if ( bDrawStatsResult != true )
		{
			WriteToLog
			(
				TEXT("DrawStats failed.")
			);

			goto Cleanup;
		};


		//
		// Present.
		//

		HRESULT hSwapChainPresentResult = swapChain->Present
		(
			NULL,
			NULL,
			NULL,
			NULL
		);

		if ( FAILED( hSwapChainPresentResult ) )
		{
			WriteToLog
			(
				TEXT("Swap chain Present failed with error %s."),
				m_pD3D->HResultToString( hSwapChainPresentResult )
			);

			goto Cleanup;
		};


		//
		// Restore the render target.
		//

		bool bSetRenderTarget2Result = SetRenderTarget( DWORD( 0 ), pRenderTarget );

		if ( bSetRenderTarget2Result != true )
		{
			WriteToLog
			(
				TEXT("SetRenderTarget failed.")
			);

			goto Cleanup;
		};

        RELEASE(pRenderTarget);
        RELEASE(pSwapChain);
        RELEASE(pSurface);
        RELEASE(swapChain);
        RELEASE(pBuffer);
	};


	TestResult = RESULT_PASS;


Cleanup:
    RELEASE(pSurf);
    RELEASE(pRenderTarget);
    RELEASE(pSwapChain);
    RELEASE(pSurface);
    RELEASE(swapChain);
    RELEASE(pBuffer);

	switch (TestResult)
	{
		case RESULT_CONTINUE:
		{
			return true;
		} break;

		case RESULT_PASS:
		{
			Pass();
		} break;

		case RESULT_FAIL:
		{
			Fail();
		} break;

		case RESULT_SKIP:
		{
			Skip();
		} break;

		case RESULT_ABORT:
		{
			Abort();
		} break;
	};

	return false;
}
