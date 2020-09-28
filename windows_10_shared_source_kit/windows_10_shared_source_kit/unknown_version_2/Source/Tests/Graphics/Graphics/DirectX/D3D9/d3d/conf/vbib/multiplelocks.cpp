//////////////////////////////////////////////////////////////////////
// File:  MultipleLocks.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of Direct3D vertex buffer and index buffer tests.
//
// History:
// 27 Oct 2005  SEdmison  Cleaned up.
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
// Globals and statics
//
/////////////////////////////////////////////////////////////////
#if FAST_TEST
const DWORD MultipleLocks::Usages[] =
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


const DWORD MultipleLocks::Pools[] =
{
    POOL_DEFAULT,
    POOL_MANAGED,
    POOL_SYSTEMMEM,
};


const DWORD MultipleLocks::LockFlags[] =
{
    0,
    LOCK_DISCARD,
    LOCK_NOOVERWRITE,
    LOCK_NOSYSLOCK,
    LOCK_DISCARD | LOCK_NOOVERWRITE,
    LOCK_DISCARD | LOCK_NOSYSLOCK,
    LOCK_NOOVERWRITE | LOCK_NOSYSLOCK,
    LOCK_DISCARD | LOCK_NOOVERWRITE | LOCK_NOSYSLOCK,
};
#else //FAST_TEST

const DWORD MultipleLocks::Usages[] =
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


const DWORD MultipleLocks::Pools[] =
{
	POOL_DEFAULT,
	POOL_MANAGED,
	POOL_SYSTEMMEM,
};


const DWORD MultipleLocks::LockFlags[] =
{
	0,
	LOCK_DISCARD,
	LOCK_NOOVERWRITE,
	LOCK_NOSYSLOCK,
	LOCK_DISCARD | LOCK_NOOVERWRITE,
	LOCK_DISCARD | LOCK_NOSYSLOCK,
	LOCK_NOOVERWRITE | LOCK_NOSYSLOCK,
	LOCK_DISCARD | LOCK_NOOVERWRITE | LOCK_NOSYSLOCK,
};

#endif //FAST_TEST

static MultipleLocksTestCase s_MultipleLocksTestCases
[
	COUNTOF( MultipleLocks::Usages ) *
	COUNTOF( MultipleLocks::Pools ) *
	COUNTOF( MultipleLocks::LockFlags )
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
// Function:  MultipleLocks
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
MultipleLocks::MultipleLocks()
{
	m_szTestName = _T("Multiple Locks");
	m_szCommandKey = _T("MultipleLocks");
};


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
UINT MultipleLocks::TestInitialize()
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
			for ( UINT iCurrentLockFlag = 0; iCurrentLockFlag < COUNTOF( LockFlags ); ++iCurrentLockFlag )
			{
				ASSERT( cTestCases < COUNTOF( s_MultipleLocksTestCases ) );

				s_MultipleLocksTestCases[ cTestCases ].nTest = cTestCases;

				s_MultipleLocksTestCases[ cTestCases ].Usage = Usages[ iCurrentUsage ];
				s_MultipleLocksTestCases[ cTestCases ].Pool = Pools[ iCurrentPool ];
				s_MultipleLocksTestCases[ cTestCases ].LockFlags = LockFlags[ iCurrentLockFlag ];

				++cTestCases;
			};
		};
	};


	//
	// Set the test range.
	//

	SetTestRange
	(
		1,
		cTestCases
	);


	//
	// Initialize our shape.
	//

	m_shape.NewShape( CS_SPHERE, 10 );


	//
	// Hand off to do the rest of our initialization.
	//

	D3DTESTINIT uiRet = D3DTESTINIT_RUN;
	do
	{
		uiRet = TestInitReal(CD3D_SRC);
		if( uiRet != D3DTESTINIT_RUN )
		{
			continue;
		}

		uiRet = TestInitReal(CD3D_REF);
		if( uiRet != D3DTESTINIT_RUN )
		{
			continue;
		}
	} while(0);

	return uiRet;
};


///////////////////////////////////////////////////////
// Function:  ExecuteTest
//
// Purpose:
// Runs a test case.
///////////////////////////////////////////////////////
bool MultipleLocks::ExecuteTest
(
	UINT nTest
)
{
	//
	// Validate the test case ordinal.
	//

	if ( nTest > COUNTOF( s_MultipleLocksTestCases ) )
	{
		WriteToLog
		(
			_T( "Invalid test ordinal %u." ),
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
		TEXT("Usage = 0x%x, Pool = %d, LockFlags = 0x%x"),
		s_MultipleLocksTestCases[ nTest - 1 ].Usage,
		s_MultipleLocksTestCases[ nTest - 1 ].Pool,
		s_MultipleLocksTestCases[ nTest - 1 ].LockFlags
	);


	//
	// Begin the test case.
	//

	BeginTestCase( sz, nTest );


	//
	// Clear the frame in preparation for rendering.
	//

	ClearFrame();


	//
	// Hand off to actually run the test.
	//

	BOOL bRetTest = ExecuteReal
	(
		s_MultipleLocksTestCases[ nTest - 1 ].Usage,
		s_MultipleLocksTestCases[ nTest - 1 ].Pool,
		s_MultipleLocksTestCases[ nTest - 1 ].LockFlags,
		(m_pSrcDevice->GetBehaviors() & DEVICECREATE_MIXED),
		CD3D_SRC
	);

	BOOL bRetRef = ExecuteReal
	(
		s_MultipleLocksTestCases[ nTest - 1 ].Usage,
		s_MultipleLocksTestCases[ nTest - 1 ].Pool,
		s_MultipleLocksTestCases[ nTest - 1 ].LockFlags,
		(m_pRefDevice->GetBehaviors() & DEVICECREATE_MIXED),
		CD3D_REF
	);


	bool bRet = bRetTest && bRetRef;
	if( !bRet )
	{
		Fail();
	}

	return bRet;
}


////////////////////////////////////////////////////////////
//
// Helper methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  TestInitReal
//
// Purpose:
//
///////////////////////////////////////////////////////
D3DTESTINIT MultipleLocks::TestInitReal
(
	DWORD dwFlags
)
{
	bool bResult;


	do
	{
		// Set the magic matricies
		D3DMATRIX matView =
		{
			1.0f,    0,       0,       0,
			0,       1.0f,    0,       0,
			0,       0,       1.0f,    0,
			0,       0,       2.0f,/*10.0f,*/   1.0f
		};
		bResult = SetTransform( D3DTS_VIEW, &matView, dwFlags );
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetTransform(D3DTS_VIEW,) failed.\n")
			);
			continue;
		}

		D3DMATRIX matProj =
		{
			2.0f,    0,       0,       0,
			0,       2.0f,    0,       0,
			0,       0,       1.0f,    1.0f,
			0,       0,       -1.0f,   0.0f
		};
		bResult = SetTransform( D3DTS_PROJECTION, &matProj, dwFlags );
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetTransform(D3DTS_PROJECTION,) failed.\n")
			);
			continue;
		}

		// Set appropiate states
		SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB( 0xff, 0xff, 0xff ), dwFlags );
		SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE, dwFlags );
		SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE, dwFlags );
		SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE, dwFlags );
		SetRenderState( D3DRS_CLIPPING, FALSE, dwFlags );

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
			D3DXMatrixRotationX( &d3dMatOut, -0.46f )
		);

		d3dMat = d3dMatOut;

		bResult = SetTransform( D3DTS_WORLD, &d3dMat, dwFlags );
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetTransform(D3DTS_WORLD,) failed.\n")
			);
			continue;
		}


		MATERIAL mtrl =
		{
			{ 0.8f, 0.0f, 0.0f, 0.0f },  // diffuse
			{ 0.0f, 0.3f, 0.0f, 0.5f },  // ambient
			{ 0.0f, 0.0f, 1.0f, 0.0f },	 // specular
			{ 0.0f, 0.0f, 0.0f, 0.0f },	 // emissive
			0.0f  // sharpness (for specular)
		};

		bResult = SetMaterial(&mtrl, dwFlags);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetMaterial() returned %s (0x%x)\n")
			);
			continue;
		}
	}
	while(0);


	return( false == bResult ? D3DTESTINIT_ABORT : D3DTESTINIT_RUN );
}


///////////////////////////////////////////////////////
// Function:  ExecuteReal
//
// Purpose:
// Handles the heavy lifting in running a test case.
///////////////////////////////////////////////////////
BOOL MultipleLocks::ExecuteReal
(
	DWORD Usage,
	DWORD Pool,
	DWORD LockFlags,
	BOOL fMixedMode,
	DWORD dwFlags
)
{
	bool bResult;
	BOOL bRet = FALSE;

	LPBYTE pbObject = NULL;
	LPVERTEXBUFFERS pVB = NULL;
	LPINDEXBUFFERS pIB = NULL;

	do
	{
		BOOL fSoftwareUsage = FALSE;

		// determine sw usage
		if( fMixedMode )
			fSoftwareUsage = GetSoftwareVertexProcessing(dwFlags );
		// determine sw usage
		/*if( fMixedMode )
		{
			DWORD dwSW;
			GetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, &dwSW, dwFlags );
			fSoftwareUsage = dwSW;
		}*/


		// create buffers
		bResult = CreateVertexBuffer
		(
			m_shape.m_nVertices * sizeof(D3DVERTEX),
			Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : NULL),
			D3DFVF_VERTEX,
			Pool,
			&pVB,
			dwFlags
		);
		if( false == bResult )
		{
			WriteToLog
			(
				TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed.\n"),
				m_shape.m_nVertices * sizeof(D3DVERTEX),
				Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : NULL),
				D3DFVF_VERTEX,
				Pool
			);
			continue;
		}

		bResult = CreateIndexBuffer
		(
			m_shape.m_nIndices * sizeof(WORD),
			Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : NULL),
			FMT_INDEX16,
			Pool,
			&pIB,
			dwFlags
		);
		if( false == bResult )
		{
			WriteToLog
			(
				TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,) failed.\n"),
				m_shape.m_nIndices * sizeof(WORD),
				Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : NULL),
				FMT_INDEX16,
				Pool
			);
			continue;
		}


		// setup streams
		bResult = SetStreamSource
		(
			0,
			pVB,
			sizeof(D3DVERTEX),
			0,
			dwFlags
		);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetStreamSource() failed.\n")
			);
			continue;
		}

		bResult = SetIndices
		(
			pIB,
			0,
			dwFlags
		);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetIndices() failed.\n")
			);
			continue;
		}

		bResult = SetVertexShader( D3DFVF_VERTEX, dwFlags );
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetVertexShader(D3DFVF_VERTEX) failed.\n")
			);
			continue;
		}

		Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0, dwFlags );

		bResult = BeginScene(dwFlags);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetVertexShader(D3DFVF_VERTEX) failed.\n")
			);
			continue;
		}

		bRet = TRUE;

		do
		{
			// lock buffers mutiple times
			for( INT iIndex = 0; iIndex < 10; iIndex++ )
			{
				pbObject = NULL;
				bResult = pVB->Lock(
					0,
					m_shape.m_nVertices * sizeof(D3DVERTEX),
					&pbObject,
					LockFlags,
					dwFlags
				);
				if( false == bResult )
				{
					WriteToLog(
						TEXT("VB::Lock(0,%d,,0x%x) failed.\n"),
						m_shape.m_nVertices * sizeof(D3DVERTEX),
						LockFlags
					);
					bRet = FALSE;
					continue;
				}
				else if( pbObject == NULL )
				{
					WriteToLog(
						TEXT("VB::Lock(0,%d,,0x%x) succeeded but pointer was NULL\n"),
						m_shape.m_nVertices * sizeof(D3DVERTEX),
						LockFlags
					);
					bRet = FALSE;
					continue;
				}


				CopyMemory( pbObject, m_shape.m_pVertices, m_shape.m_nVertices * sizeof(D3DVERTEX) );

				bResult = pVB->Unlock(dwFlags);
				if( false == bResult )
				{
					WriteToLog(
						TEXT("VB::Unlock() failed.\n")
					);
					bRet = FALSE;
					continue;
				}


				pbObject = NULL;
				bResult = pIB->Lock
				(
					0,
					m_shape.m_nIndices * sizeof(WORD),
					&pbObject,
					LockFlags,
					dwFlags
				);
				if( false == bResult )
				{
					WriteToLog(
						TEXT("IB::Lock(0,%d,,0x%x) failed.\n"),
						m_shape.m_nIndices * sizeof(WORD),
						LockFlags
					);
					bRet = FALSE;
					continue;
				}
				else if( pbObject == NULL )
				{
					WriteToLog(
						TEXT("IB::Lock(0,%d,,0x%x) succeeded but pointer was NULL\n"),
						m_shape.m_nIndices * sizeof(WORD),
						LockFlags
					);
					bRet = FALSE;
					continue;
				}

				CopyMemory( pbObject, m_shape.m_pIndices, m_shape.m_nIndices * sizeof(WORD) );

				bResult = pIB->Unlock(dwFlags);
				if( false == bResult )
				{
					WriteToLog(
						TEXT("IB::Unlock() failed.\n")
					);
					bRet = FALSE;
					continue;
				}
			}


			// draw opject
			bResult = DrawIndexedPrimitive
			(
				D3DPT_TRIANGLELIST,
				0,
				m_shape.m_nVertices,
				0,
				m_shape.m_nIndices / 3,
				0,
				dwFlags
			);
			if( false == bResult )
			{
				WriteToLog(
					TEXT("DrawIndexedPrimitive() failed.\n")
				);
				bRet = FALSE;
				continue;
			}
		}
		while(0);

		bResult = EndScene(dwFlags);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("EndScene() failed.\n")
			);
			bRet = FALSE;
			continue;
		}
	} while(0);


	RELEASE(pVB);
	RELEASE(pIB);


	return bRet;
}
