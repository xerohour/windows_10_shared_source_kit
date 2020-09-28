//////////////////////////////////////////////////////////////////////
// File:  LostOnReset.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of test cases to verify that POOL_DEFAULT vertex
// buffers and index buffers must be released before Reset will
// succeed for Direct3D versions prior to 9.L.  Also verifies that
// POOL_SYSTEMMEM resources survive the Reset.
//
// History:
// ?? ??? 2000  Darius Rad  Created.
// 03 Jul 2005  OlanH       Added comments added.  Improved logging.
// 27 Oct 2005  SEdmison    Cleaned up.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

#include "CVertexIndexBufferTest.h"
#include "CBuffers.h"
#include <d3d8.h>
#include <d3dx8.h>


/////////////////////////////////////////////////////////////////
//
// Statics and globals
//
/////////////////////////////////////////////////////////////////

extern CD3DWindowFramework *g_pD3DFramework;

#if FAST_TEST
const DWORD LostOnReset::Usages[] =
{
    0,
    USAGE_WRITEONLY,
    USAGE_DONOTCLIP,
    USAGE_POINTS,
    USAGE_RTPATCHES,
    USAGE_DYNAMIC,
    USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_POINTS | USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_RTPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_WRITEONLY | USAGE_DYNAMIC,
    USAGE_DONOTCLIP | USAGE_DYNAMIC,
    USAGE_WRITEONLY | USAGE_DONOTCLIP | USAGE_DYNAMIC,
};


const DWORD LostOnReset::Pools[] =
{
    POOL_DEFAULT,
    POOL_MANAGED,
    POOL_SYSTEMMEM,
};
#else //FAST_TEST

const DWORD LostOnReset::Usages[] =
{
	0,
	USAGE_WRITEONLY,
	USAGE_DONOTCLIP,
	USAGE_POINTS,
	USAGE_RTPATCHES,
	USAGE_NPATCHES,
	USAGE_DYNAMIC,
	USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_POINTS | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_RTPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_NPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_POINTS | USAGE_RTPATCHES | USAGE_NPATCHES,
	USAGE_WRITEONLY | USAGE_DYNAMIC,
	USAGE_DONOTCLIP | USAGE_DYNAMIC,
	USAGE_WRITEONLY | USAGE_DONOTCLIP | USAGE_DYNAMIC,
};


const DWORD LostOnReset::Pools[] =
{
	POOL_DEFAULT,
	POOL_MANAGED,
	POOL_SYSTEMMEM,
};

#endif //FAST_TEST

static LostOnResetTestCase s_LostOnResetTestCases
[
	COUNTOF( LostOnReset::Usages ) *
	COUNTOF( LostOnReset::Pools )
];


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction / destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  LostOnReset
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
LostOnReset::LostOnReset()
{
	m_szTestName = _T("VBIB Lost on Reset");
	m_szCommandKey = _T("LostOnReset");

	m_Options.D3DOptions.bReference = false;
	m_Options.D3DOptions.bCachedBackBuffer    = false;
}


////////////////////////////////////////////////////////////
//
// Framework override methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  TestInitialize
//
// Purpose:
// Test group initialization.
///////////////////////////////////////////////////////
UINT LostOnReset::TestInitialize()
{
	//
	// Count the number of valid scenarios, and build our
	// scenario data list.
	//

	UINT cTestCases = 0;
	for ( UINT iCurrentUsage = 0; iCurrentUsage < COUNTOF( Usages ); ++iCurrentUsage )
	{
		for ( UINT iCurrentPool = 0; iCurrentPool < COUNTOF( Pools ); ++iCurrentPool )
		{
			// Skip invalid combinations of parameters:
			if
			(
				( Usages[ iCurrentUsage] & USAGE_DYNAMIC ) &&
				( Pools[ iCurrentPool ] == POOL_MANAGED )
			)
			{
				continue;
			};

			s_LostOnResetTestCases[ cTestCases ].nTest = cTestCases;
			s_LostOnResetTestCases[ cTestCases ].Usage = Usages[ iCurrentUsage ];
			s_LostOnResetTestCases[ cTestCases ].Pool = Pools[ iCurrentPool ];

			++cTestCases;
		};
	};


	//
	// Set the test range.
	//

	//WriteToLog( _T( "%u test cases found." ), cTestCases );
	SetTestRange
	(
		1,
		cTestCases
	);


	//
	// Initialize our shape.
	//

	m_shape.NewShape( CS_SPHERE, 10 );

	return D3DTESTINIT_RUN;
}


///////////////////////////////////////////////////////
// Function:  TestTerminate
//
// Purpose:
//
///////////////////////////////////////////////////////
bool LostOnReset::TestTerminate()
{
	return true;
}


///////////////////////////////////////////////////////
// Function:  ExecuteTest
//
// Purpose:
// Runs a test case.
///////////////////////////////////////////////////////
bool LostOnReset::ExecuteTest
(
	UINT nTest
)
{
	//
	// Validate the test case ordinal.
	//

	if ( nTest > COUNTOF( s_LostOnResetTestCases ) )
	{
		WriteToLog
		(
			TEXT( "Invalid test ordinal %u.\n" ),
			nTest
		);

		return false;
	};


	//
	// Build the test case name.
	//

	TCHAR sz[ 256 ] = _T( "" );
	wsprintf
	(
		sz,
		TEXT("Usage = 0x%x, Pool = %u"),
		s_LostOnResetTestCases[ nTest - 1 ].Usage,
		s_LostOnResetTestCases[ nTest - 1 ].Pool
	);


	//
	// Begin the test case.
	//

	BeginTestCase( sz, nTest );


	//
	// Look up test case data.
	//

	D3DTEST_NORMALBUFFER curtest;
	curtest.szName = TEXT("");
	curtest.shape = &m_shape;
	curtest.VBFVF = D3DFVF_VERTEX;
	curtest.IBFormat = FMT_INDEX16;
	curtest.Usage = s_LostOnResetTestCases[ nTest - 1 ].Usage;
	curtest.Pool = s_LostOnResetTestCases[ nTest - 1 ].Pool;


	//
	// Do initialization before every test case, because Reset()
	// from previous test may fail.
	//

	D3DTESTINIT uiRet = TestInitReal();
	if( uiRet != D3DTESTINIT_RUN )
	{
		Skip();

		return false;
	}


	BOOL bRetTest;

	// Execute real *may* (or may not) create a new CDevice
	bRetTest = ExecuteReal
	(
		&curtest
	);

	if( !bRetTest )
	{
		Fail();
	}

	return false;
}


///////////////////////////////////////////////////////
// Function:  TestInitReal
//
// Purpose:
//
///////////////////////////////////////////////////////
D3DTESTINIT LostOnReset::TestInitReal()
{
	HRESULT hr;


	//
	// Set the magic matrices
	//

	D3DMATRIX matView =
	{
		1.0f,    0,       0,       0,
		0,       1.0f,    0,       0,
		0,       0,       1.0f,    0,
		0,       0,       2.0f,/*10.0f,*/   1.0f
	};
	hr = m_pSrcDevice->SetTransform( D3DTS_VIEW, &matView );
	if( FAILED(hr) )
	{
		WriteToLog(
			TEXT("SetTransform(D3DTS_VIEW,) returned %s (0x%x)\n"),
			m_pD3D->HResultToString(hr),
			hr
		);

		return D3DTESTINIT_SKIPALL;
	};


	D3DMATRIX matProj =
	{
		2.0f,    0,       0,       0,
		0,       2.0f,    0,       0,
		0,       0,       1.0f,    1.0f,
		0,       0,       -1.0f,   0.0f
	};
	hr = m_pSrcDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	if( FAILED(hr) )
	{
		WriteToLog(
			TEXT("SetTransform(D3DTS_PROJECTION,) returned %s (0x%x)\n"),
			m_pD3D->HResultToString(hr),
			hr
		);

		return D3DTESTINIT_SKIPALL;
	};


	//
	// Set render states.
	//

	m_pSrcDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB( 0xff, 0xff, 0xff ) );
	m_pSrcDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pSrcDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	m_pSrcDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	m_pSrcDevice->SetRenderState( D3DRS_CLIPPING, FALSE );


	D3DXMATRIX d3dMat;
	D3DXMATRIX d3dMatOut;

	D3DMATRIX matWorld =
	{
		1.0f,    0,       0,       0,
		0,       1.0f,    0,       0,
		0,       0,       1.0f,    0,
		0,       0,       0,       1.0f
	};

	d3dMat = matWorld;

	D3DXMatrixMultiply(
		&d3dMatOut,
		&d3dMat,
		D3DXMatrixRotationY( &d3dMatOut, 2.0f )
	);

	d3dMat = d3dMatOut;

	D3DXMatrixMultiply(
		&d3dMatOut,
		&d3dMat,
		D3DXMatrixRotationZ( &d3dMatOut, 0.40f )
	);

	d3dMat = d3dMatOut;

	D3DXMatrixMultiply(
		&d3dMatOut,
		&d3dMat,
		D3DXMatrixRotationX( &d3dMatOut, 2.0f )
	);

	d3dMat = d3dMatOut;

	hr = m_pSrcDevice->SetTransform( D3DTS_WORLD, &d3dMat );
	if( FAILED(hr) )
	{
		WriteToLog(
			TEXT("SetTransform(D3DTS_WORLD,) returned %s (0x%x)\n"),
			m_pD3D->HResultToString(hr),
			hr
		);

		return D3DTESTINIT_SKIPALL;
	};


	MATERIAL mtrl =
	{
		{ 1.0f, 0.0f, 0.0f, 0.0f },  // diffuse
		{ 0.0f, 0.3f, 0.0f, 0.5f },  // ambient
		{ 1.0f, 0.0f, 0.0f, 0.0f },     // specular
		{ 0.0f, 0.0f, 0.0f, 0.0f },     // emissive
		0.0f  // sharpness (for specular)
	};

	hr = m_pSrcDevice->SetMaterial(&mtrl);
	if( FAILED(hr) )
	{
		WriteToLog(
			TEXT("SetMaterial() returned %s (0x%x)\n"),
			m_pD3D->HResultToString(hr),
			hr
		);

		return D3DTESTINIT_SKIPALL;
	};


	return D3DTESTINIT_RUN;
};


///////////////////////////////////////////////////////
// Function:  ExecuteReal
//
// Purpose:
// Handles the heavy lifting in running a test.
///////////////////////////////////////////////////////
BOOL LostOnReset::ExecuteReal
(
	const D3DTEST_NORMALBUFFER *lpBufInfo
)
{
	HRESULT hr = D3D_OK;
	LPVERTEXBUFFER pVB = NULL;
	LPINDEXBUFFER  pIB = NULL;
	LPBYTE pbObject = NULL;
	BOOL bRet = FALSE;

	BOOL fSoftwareUsage = FALSE;


	// determine sw usage
	if( m_bMixedMode )
	{
		BOOL dwSW;
		dwSW = m_pSrcDevice->GetSoftwareVertexProcessing();
		fSoftwareUsage = dwSW;
	}

	do
	{
		hr = m_pSrcDevice->CreateVertexBuffer(
			lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX),
			lpBufInfo->Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : 0),
			lpBufInfo->VBFVF,
			lpBufInfo->Pool,
			&pVB
		);
		if( FAILED(hr) )
		{
			WriteToLog(
				TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed, returned %s (0x%x)\n"),
				lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX),
				lpBufInfo->Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : 0),
				lpBufInfo->VBFVF,
				lpBufInfo->Pool,
				m_pD3D->HResultToString(hr),
				hr
			);
			continue;
		}

		hr = pVB->Lock(
			0,
			lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX),
			&pbObject,
			0
		);
		if( FAILED(hr) )
		{
			WriteToLog(
				TEXT("Lock() returned %s (0x%x)\n"),
				m_pD3D->HResultToString(hr),
				hr
			);
			continue;
		}

		CopyMemory
		(
			pbObject,
			lpBufInfo->shape->m_pVertices,
			lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX)
		);

		hr = pVB->Unlock();
		if( FAILED(hr) )
		{
			WriteToLog(
				TEXT("Unlock() returned %s (0x%x)\n"),
				m_pD3D->HResultToString(hr),
				hr
			);
			continue;
		}

		hr = m_pSrcDevice->SetStreamSource(
			0,
			pVB,
			sizeof(D3DVERTEX)
		);
		if( FAILED(hr) )
		{
			WriteToLog(
				TEXT("SetStreamSource() returned %s (0x%x)\n"),
				m_pD3D->HResultToString(hr),
				hr
			);
			continue;
		}


		hr = m_pSrcDevice->CreateIndexBuffer(
			lpBufInfo->shape->m_nIndices * sizeof(WORD),
			lpBufInfo->Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : 0),
			lpBufInfo->IBFormat,
			lpBufInfo->Pool,
			&pIB
		);
		if( FAILED(hr) )
		{
			WriteToLog(
				TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,) failed, returned %s (0x%x)\n"),
				lpBufInfo->shape->m_nIndices * sizeof(WORD),
				lpBufInfo->Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : 0),
				lpBufInfo->IBFormat,
				lpBufInfo->Pool,
				m_pD3D->HResultToString(hr),
				hr
			);
			continue;
		}

		hr = pIB->Lock(
			0,
			lpBufInfo->shape->m_nIndices * sizeof(WORD),
			&pbObject,
			0
		);
		if( FAILED(hr) )
		{
			WriteToLog(
				TEXT("Lock() returned %s (0x%x)\n"),
				m_pD3D->HResultToString(hr),
				hr
			);
			continue;
		}

		CopyMemory
		(
			pbObject,
			lpBufInfo->shape->m_pIndices,
			lpBufInfo->shape->m_nIndices * sizeof(WORD)
		);

		hr = pIB->Unlock();
		if( FAILED(hr) )
		{
			WriteToLog(
				TEXT("Unlock() returned %s (0x%x)\n"),
				m_pD3D->HResultToString(hr),
				hr
			);
			continue;
		}

		hr = m_pSrcDevice->SetIndices(
			pIB,
			0
		);
		if( FAILED(hr) )
		{
			WriteToLog(
				TEXT("SetIndices() returned %s (0x%x)\n"),
				m_pD3D->HResultToString(hr),
				hr
			);
			continue;
		}


		hr = m_pSrcDevice->SetVertexShader( D3DFVF_VERTEX );
		if( FAILED(hr) )
		{
			WriteToLog(
				TEXT("SetVertexShader() returned %s (0x%x)\n"),
				m_pD3D->HResultToString(hr),
				hr
			);
			continue;
		}

		bRet = TRUE;

		ReleaseObjects();
		m_pSrcDevice->ReleaseWrappers();

   
		if( lpBufInfo->Pool == POOL_DEFAULT )
		{
			// make sure reset fails until buffers freed

			hr = m_pSrcDevice->Reset( m_pSrcDevice->GetPresParams() );
			if
			(
				m_pD3D->GetVersion() < 9.0f &&
					hr != D3DERR_DEVICELOST ||
				m_pD3D->GetVersion() >= 9.0f &&
					m_pD3D->GetVersion() < 9.1f &&
					hr != D3DERR_INVALIDCALL
			)
			{
				WriteToLog(
					TEXT("Reset() succeeded even though POOL_DEFAULT resources exist (VB and IB).  Reset returned %s (0x%x)\n"),
					m_pD3D->HResultToString(hr),
					hr
				);
				bRet = FALSE;
				continue;
			}
			else if
			(
				m_pD3D->GetVersion() >= 9.1f &&
					FAILED( hr )
			)
			{
				WriteToLog(
					TEXT("Reset() failed due to POOL_DEFAULT resources (VB and IB) when expected to succeed on 9.L.  Reset returned %s (0x%x)\n"),
					m_pD3D->HResultToString(hr),
					hr
				);
				bRet = FALSE;
				continue;
			};


			m_pSrcDevice->GetLastError();

			__try
			{
				RELEASE(pVB);
			}
			__except
			(
				ExceptionFilter
				(
					GetExceptionCode(),
					GetExceptionInformation()
				)
			)
			{
				WriteToLog
				(
					TEXT("VB->Release() caused unhandled exception\n")
				);

				Fail();
			}

			// Now that the VB is released.  See if Reset succeeds.
			// It shouldn't prior to 9.L since the IB is still around.
			hr = m_pSrcDevice->Reset( m_pSrcDevice->GetPresParams() );
			if
			(
				m_pD3D->GetVersion() < 9.0f &&
					hr != D3DERR_DEVICELOST ||
				m_pD3D->GetVersion() >= 9.0f &&
					m_pD3D->GetVersion() < 9.1f &&
					hr != D3DERR_INVALIDCALL
			)
			{
				WriteToLog(
					TEXT("Reset() succeeded even though POOL_DEFAULT resources exist (IB). Reset returned %s (0x%x)\n"),
					m_pD3D->HResultToString(hr),
					hr
				);
				bRet = FALSE;
				continue;
			}
			else if
			(
				m_pD3D->GetVersion() >= 9.1f &&
					FAILED( hr )
			)
			{
				WriteToLog(
					TEXT("Reset() failed due to POOL_DEFAULT resources (IB) when expected to succeed on 9.L.  Reset returned %s (0x%x)\n"),
					m_pD3D->HResultToString(hr),
					hr
				);
				bRet = FALSE;
				continue;
			};

			m_pSrcDevice->GetLastError();

			__try
			{
				RELEASE(pIB);
			}
			__except
			(
				ExceptionFilter
				(
					GetExceptionCode(),
					GetExceptionInformation()
				)
			)
			{
				WriteToLog
				(
					TEXT("IB->Release() caused unhandled exception\n")
				);

				Fail();
			}

			// Now that all resources are freed, reset should succeed. 
			hr = m_pSrcDevice->Reset( m_pSrcDevice->GetPresParams() );
			if( FAILED(hr) )
			{
				WriteToLog(
					TEXT("Reset() returned %s (0x%x). Expected success since all POOL_DEFAULT resources are freed.\n"),
					m_pD3D->HResultToString(hr),
					hr
				);
				bRet = FALSE;
				continue;
			}

		}
		else // Using SYSTEMMEM resources.
		{
			// buffers should not be lost

			hr = m_pSrcDevice->Reset( m_pSrcDevice->GetPresParams() );
			if( FAILED(hr) )
			{
				WriteToLog(
					TEXT("Reset() returned %s (0x%x).  Unexpected failure.  Testcase cannot continue.\n"),
					m_pD3D->HResultToString(hr),
					hr
				);
				bRet = FALSE;
				continue;
			}

			hr = pVB->Lock(
				0,
				0,
				&pbObject,
				0
			);
			if( FAILED(hr) )
			{
				WriteToLog(
					TEXT("VB::Lock() after Reset() returned %s (0x%x).  This was a SYSTEMMEM resource.  It should have survived the Reset()\n"),
					m_pD3D->HResultToString(hr),
					hr
				);
				bRet = FALSE;
				continue;
			}

			hr = pVB->Unlock();
			if( FAILED(hr) )
			{
				WriteToLog(
					TEXT("VB::Unlock() failed.  Returned %s (0x%x)\n"),
					m_pD3D->HResultToString(hr),
					hr
				);
				bRet = FALSE;
				continue;
			}

			hr = pIB->Lock(
				0,
				0,
				&pbObject,
				0
			);
			if( FAILED(hr) )
			{
				WriteToLog(
					TEXT("IB::Lock() after Reset() returned %s (0x%x).  This was a SYSTEMMEM resource.  It should have survived the Reset()\n"),
					m_pD3D->HResultToString(hr),
					hr
				);
				bRet = FALSE;
				continue;
			}

			hr = pIB->Unlock();
			if( FAILED(hr) )
			{
				WriteToLog(
					TEXT("IB::Unlock() failed.  Returned %s (0x%x)\n"),
					m_pD3D->HResultToString(hr),
					hr
				);
				bRet = FALSE;
				continue;
			}
		}
	}
	while(0);


	RELEASE(pIB);
	RELEASE(pVB);

	return(bRet);
}
