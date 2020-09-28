//////////////////////////////////////////////////////////////////////
// File:  LockActive.cpp
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
// Constants and statics
//
/////////////////////////////////////////////////////////////////
#if FAST_TEST
const DWORD LockActive::Usages[] =
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


const DWORD LockActive::Pools[] =
{
    POOL_DEFAULT,
    POOL_MANAGED,
    POOL_SYSTEMMEM,
};


const DWORD LockActive::LockFlags[] =
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


const LockActiveTestStage LockActive::Stages[] =
{
    { TEXT("Creating buffers, rendering base objects"), TRUE, FALSE, FALSE, TRUE, FALSE },
    { TEXT("Locking Vertex Buffer and adding second object"), FALSE, TRUE, FALSE, TRUE, FALSE },
    { TEXT("Locking Index Buffer and adding second object"), FALSE, FALSE, TRUE, TRUE, FALSE },
    { TEXT("Rendering second object"), FALSE, FALSE, FALSE, FALSE, TRUE },
    { TEXT("Rendering both objects"), FALSE, FALSE, FALSE, TRUE, TRUE },
    { TEXT("Multiple locks and drawprim in one scene"), TRUE, TRUE, TRUE, TRUE, TRUE },
};

#else //FAST_TEST

const DWORD LockActive::Usages[] =
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


const DWORD LockActive::Pools[] =
{
	POOL_DEFAULT,
	POOL_MANAGED,
	POOL_SYSTEMMEM,
};


const DWORD LockActive::LockFlags[] =
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


const LockActiveTestStage LockActive::Stages[] =
{
	{ TEXT("Creating buffers, rendering base objects"), TRUE, FALSE, FALSE, TRUE, FALSE },
	{ TEXT("Locking Vertex Buffer and adding second object"), FALSE, TRUE, FALSE, TRUE, FALSE },
	{ TEXT("Locking Index Buffer and adding second object"), FALSE, FALSE, TRUE, TRUE, FALSE },
	{ TEXT("Rendering second object"), FALSE, FALSE, FALSE, FALSE, TRUE },
	{ TEXT("Rendering both objects"), FALSE, FALSE, FALSE, TRUE, TRUE },
	{ TEXT("Multiple locks and drawprim in one scene"), TRUE, TRUE, TRUE, TRUE, TRUE },
};

#endif //FAST_TEST

static LockActiveTestCase s_LockActiveTestCases
[
	COUNTOF( LockActive::LockFlags ) *
	COUNTOF( LockActive::Usages ) *
	COUNTOF( LockActive::Pools ) *
	COUNTOF( LockActive::Stages )
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
// Function:  LockActive
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
LockActive::LockActive()
:
	m_pVB( NULL ),
	m_pIB( NULL )
{
	m_szTestName = _T("VBIB Lock Active");
	m_szCommandKey = _T("LockActive");
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
UINT LockActive::TestInitialize()
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
				for ( UINT iCurrentStage = 0; iCurrentStage < COUNTOF( Stages ); ++iCurrentStage )
				{
					// Skip invalid combinations of parameters:
					if
					(
						// Dynamic buffers can't be in POOL_MANAGED, and
						( ( Usages[ iCurrentUsage ] & USAGE_DYNAMIC ) && ( Pools[ iCurrentPool] == POOL_MANAGED ) ) ||

						// LOCK_DISCARD and LOCK_NOOVERWRITE can only be used on dynamic buffers.
						( !( Usages[ iCurrentUsage ] & USAGE_DYNAMIC ) && ( LockFlags[ iCurrentLockFlag ] & LOCK_DISCARD ) ) ||
						( !( Usages[ iCurrentUsage ] & USAGE_DYNAMIC ) && ( LockFlags[ iCurrentLockFlag ] & LOCK_NOOVERWRITE ) )
					)
					{
						continue;
					};


					ASSERT( cTestCases < COUNTOF( s_LockActiveTestCases ) );

					s_LockActiveTestCases[ cTestCases ].nTest = cTestCases;

					s_LockActiveTestCases[ cTestCases ].Usage = Usages[ iCurrentUsage ];
					s_LockActiveTestCases[ cTestCases ].Pool = Pools[ iCurrentPool ];
					s_LockActiveTestCases[ cTestCases ].LockFlag = LockFlags[ iCurrentLockFlag ];
					s_LockActiveTestCases[ cTestCases ].TestStage = iCurrentStage;
					s_LockActiveTestCases[ cTestCases ].bCompleted = false;

					++cTestCases;
				};
			};
		};
	};


	//
	// Set the test range.
	//

#if FAST_TEST
    m_totalNumTestCases = cTestCases;
    SetTestRange(1, 1);
#else //FAST_TEST
    SetTestRange
        (
        1,
        cTestCases
        );
#endif //FAST_TEST


	//
	// Initialize our shapes.
	//

	m_shape.NewShape( CS_SPHERE, 10 );
	m_shape2.NewShape( CS_PYRAMID );


	//
	// Hand off to do the rest of our initialization.
	//

	return TestInitReal( CD3D_ALL );
};


///////////////////////////////////////////////////////
// Function:  TestTerminate
//
// Purpose:
//
///////////////////////////////////////////////////////
bool LockActive::TestTerminate()
{
	RELEASE( m_pVB );
	RELEASE( m_pIB );

	return true;
};


///////////////////////////////////////////////////////
// Function:  ExecuteTest
//
// Purpose:
// Runs a test case.
///////////////////////////////////////////////////////
#if FAST_TEST
bool LockActive::ExecuteTest
(
IN UINT nTest
)
{

    //
    // Begin the test case.
    //

    BeginTestCase("Lock Active - fast", nTest);

    //
    // Clear the frame in preparation for rendering.
    //

    ClearFrame();


    auto& drawLamda = [=](UINT testIndex) -> bool
    {
        //
        // Validate the test case ordinal.
        //

        if (testIndex > COUNTOF(s_LockActiveTestCases))
        {
            WriteToLog
                (
                _T("Invalid test ordinal %u."),
                testIndex
                );

            return false;
        };

        //
        // Skip this stage if it's part of a sequence, and the prior
        // stage(s) did not complete.
        //

        if
            (
            testIndex > 1 &&
            s_LockActiveTestCases[testIndex - 1].TestStage > 0 &&
            s_LockActiveTestCases[testIndex - 2].bCompleted != true
            )
        {
            WriteToLog(_T("Skipping this stage, because prior stage did not complete."));

            Skip();

            return false;
        };


        //
        // Hand off to actually run the test.
        //

        BOOL bRet = ExecuteReal
            (
            &m_pVB,
            &m_pIB,
            s_LockActiveTestCases[testIndex - 1].Usage,
            s_LockActiveTestCases[testIndex - 1].Pool,
            s_LockActiveTestCases[testIndex - 1].LockFlag,
            s_LockActiveTestCases[testIndex - 1].TestStage,
            (m_pSrcDevice->GetBehaviors() & DEVICECREATE_MIXED),
            CD3D_ALL
            );


        if (!bRet)
        {
            Fail();

            return false;
        };

        s_LockActiveTestCases[testIndex - 1].bCompleted = true;

        return true;
    };

    return DrawMultiTests(drawLamda);
};
#else //FAST_TEST

bool LockActive::ExecuteTest
(
	IN UINT nTest
)
{
	//
	// Validate the test case ordinal.
	//

	if ( nTest > COUNTOF( s_LockActiveTestCases ) )
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
		TEXT("Usage = 0x%x, Pool = %d, LockFlags = 0x%x; %s"),
		s_LockActiveTestCases[ nTest - 1 ].Usage,
		s_LockActiveTestCases[ nTest - 1 ].Pool,
		s_LockActiveTestCases[ nTest - 1 ].LockFlag,
		Stages[ s_LockActiveTestCases[ nTest - 1 ].TestStage ].szName
	);


	//
	// Begin the test case.
	//

	BeginTestCase( sz, nTest );


	//
	// Skip this stage if it's part of a sequence, and the prior
	// stage(s) did not complete.
	//

	if
	(
		nTest > 1 &&
		s_LockActiveTestCases[ nTest - 1 ].TestStage > 0 &&
		s_LockActiveTestCases[ nTest - 2 ].bCompleted != true
	)
	{
		WriteToLog( _T( "Skipping this stage, because prior stage did not complete." ) );

		Skip();

		return false;
	};


	//
	// Clear the frame in preparation for rendering.
	//

	ClearFrame();


	//
	// Hand off to actually run the test.
	//

	BOOL bRet = ExecuteReal
	(
		&m_pVB,
		&m_pIB,
		s_LockActiveTestCases[ nTest - 1 ].Usage,
		s_LockActiveTestCases[ nTest - 1 ].Pool,
		s_LockActiveTestCases[ nTest - 1 ].LockFlag,
		s_LockActiveTestCases[ nTest - 1 ].TestStage,
		(m_pSrcDevice->GetBehaviors() & DEVICECREATE_MIXED),
		CD3D_ALL
	);


	if ( !bRet )
	{
		Fail();

		return false;
	};

	s_LockActiveTestCases[ nTest - 1 ].bCompleted = true;

	return true;
};
#endif


////////////////////////////////////////////////////////////
//
// Helper methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  TestInitReal
//
// Purpose:
// Initializes Direct3D view port state.
///////////////////////////////////////////////////////
D3DTESTINIT LockActive::TestInitReal
(
	IN DWORD dwFlags
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
		if( !bResult )
		{
			WriteToLog
			(
				TEXT("SetTransform(D3DTS_VIEW,) failed.\n")
			);

			continue;
		};

		D3DMATRIX matProj =
		{
			2.0f,    0,       0,       0,
			0,       2.0f,    0,       0,
			0,       0,       1.0f,    1.0f,
			0,       0,       -1.0f,   0.0f
		};
		bResult = SetTransform( D3DTS_PROJECTION, &matProj, dwFlags );
		if( !bResult )
		{
			WriteToLog
			(
				TEXT("SetTransform(D3DTS_PROJECTION,) failed.\n")
			);

			continue;
		};

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

		D3DXMatrixMultiply
		(
			&d3dMatOut,
			&d3dMat,
			D3DXMatrixRotationY( &d3dMatOut, 2.0f )
		);

		d3dMat = d3dMatOut;

		D3DXMatrixMultiply
		(
			&d3dMatOut,
			&d3dMat,
			D3DXMatrixRotationZ( &d3dMatOut, 0.40f )
		);

		d3dMat = d3dMatOut;

		D3DXMatrixMultiply
		(
			&d3dMatOut,
			&d3dMat,
			D3DXMatrixRotationX( &d3dMatOut, -0.46f )
		);

		d3dMat = d3dMatOut;

		bResult = SetTransform( D3DTS_WORLD, &d3dMat, dwFlags );
		if( !bResult )
		{
			WriteToLog
			(
				TEXT("SetTransform(D3DTS_WORLD,) failed.\n")
			);

			continue;
		};


		MATERIAL mtrl =
		{
			{ 0.8f, 0.0f, 0.0f, 0.0f },  // diffuse
			{ 0.0f, 0.3f, 0.0f, 0.5f },  // ambient
			{ 0.0f, 0.0f, 1.0f, 0.0f },	 // specular
			{ 0.0f, 0.0f, 0.0f, 0.0f },	 // emissive
			0.0f  // sharpness (for specular)
		};

		bResult = SetMaterial(&mtrl, dwFlags);
		if( !bResult )
		{
			WriteToLog
			(
				TEXT("SetMaterial() failed.\n")
			);

			continue;
		};
	} while( 0 );


	return ( bResult == false ? D3DTESTINIT_ABORT : D3DTESTINIT_RUN );
};


///////////////////////////////////////////////////////
// Function:  ExecuteReal
//
// Purpose:
// Handles the heavy lifting of running a test case.
///////////////////////////////////////////////////////
BOOL LockActive::ExecuteReal
(
	LPVERTEXBUFFERS * ppVB,
	LPINDEXBUFFERS * ppIB,
	IN DWORD Usage,
	IN DWORD Pool,
	IN DWORD LockFlags,
	IN UINT iTestStage,
	IN BOOL fMixedMode,
	IN DWORD dwFlags
)
{
	bool bResult = true;
	LPBYTE pbObject = NULL;
	BOOL bRet = FALSE;

	LPTSTR szLastCall = TEXT("");

	do
	{
		__try
		{
			if ( Stages[ iTestStage ].fCreate )  // create buffers, copy first object
			{
				BOOL fSoftwareUsage = FALSE;

				// determine sw usage
				if( fMixedMode )
					fSoftwareUsage = GetSoftwareVertexProcessing(dwFlags );

				szLastCall = TEXT("Release(VB)");
				RELEASE(*ppVB);

				szLastCall = TEXT("CreateVertexBuffer()");
				bResult = CreateVertexBuffer(
					(m_shape.m_nVertices + m_shape2.m_nVertices) * sizeof(D3DVERTEX),
					Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : NULL),
					D3DFVF_VERTEX,
					Pool,
					ppVB,
					dwFlags
				);
				if( !bResult )
				{
					WriteToLog(
						TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed.\n"),
						(m_shape.m_nVertices + m_shape2.m_nVertices) * sizeof(D3DVERTEX),
						Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : NULL),
						D3DFVF_VERTEX,
						Pool
					);
					continue;
				}

				szLastCall = TEXT("VB->Lock()");
				bResult = (*ppVB)->Lock(
					0,
					m_shape.m_nVertices * sizeof(D3DVERTEX),
					&pbObject,
					LockFlags,
					dwFlags
				);
				if( !bResult )
				{
					WriteToLog(
						TEXT("VB::Lock(0,%d,,0x%x) failed.\n"),
						m_shape.m_nVertices * sizeof(D3DVERTEX),
						LockFlags
					);
					continue;
				}

				szLastCall = TEXT("CopyMemory(,VB,)");
				CopyMemory( pbObject, m_shape.m_pVertices, m_shape.m_nVertices * sizeof(D3DVERTEX) );

				szLastCall = TEXT("VB->Unlock()");
				bResult = (*ppVB)->Unlock(dwFlags);
				if( !bResult )
				{
					WriteToLog(
						TEXT("VB::Unlock() failed.\n")
					);
					continue;
				}

				szLastCall = TEXT("SetStreamSource()");
				bResult = SetStreamSource(
					0,
					*ppVB,
					sizeof(D3DVERTEX),
					0,
					dwFlags
				);
				if( !bResult )
				{
					WriteToLog(
						TEXT("SetStreamSource() failed.\n")
					);
					continue;
				}


				szLastCall = TEXT("Release(IB)");
				RELEASE(*ppIB);

				szLastCall = TEXT("CreateIndexBuffer()");
				bResult = CreateIndexBuffer(
					(m_shape.m_nIndices + m_shape2.m_nIndices) * sizeof(WORD),
					Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : NULL),
					FMT_INDEX16,
					Pool,
					ppIB,
					dwFlags
				);
				if( !bResult )
				{
					WriteToLog(
						TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,) failed.\n"),
						(m_shape.m_nIndices + m_shape2.m_nIndices) * sizeof(WORD),
						Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : NULL),
						FMT_INDEX16,
						Pool
					);
					continue;
				}

				szLastCall = TEXT("IB->Lock()");
				bResult = (*ppIB)->Lock(
					0,
					m_shape.m_nIndices * sizeof(WORD),
					&pbObject,
					LockFlags,
					dwFlags
				);
				if( !bResult )
				{
					WriteToLog(
						TEXT("IB::Lock(0,%d,,0x%x) failed.\n"),
						m_shape.m_nIndices * sizeof(WORD),
						LockFlags
					);
					continue;
				}

				szLastCall = TEXT("CopyMemory(,IB,)");
				CopyMemory( pbObject, m_shape.m_pIndices, m_shape.m_nIndices * sizeof(WORD) );

				szLastCall = TEXT("IB->Unlock()");
				bResult = (*ppIB)->Unlock(dwFlags);
				if( !bResult )
				{
					WriteToLog(
						TEXT("IB::Unlock() failed.\n")
					);
					continue;
				}

				szLastCall = TEXT("SetIndices()");
				bResult = SetIndices(
					*ppIB,
					0,
					dwFlags
				);
				if( !bResult )
				{
					WriteToLog(
						TEXT("SetIndices() failed.\n")
					);
					continue;
				}
			}


			if ( Stages[ iTestStage ].fAppendVB )  // add second object to vb
			{
				UINT offset;
				UINT sizeToLock;
				if (LockFlags & LOCK_DISCARD)
				{
					offset = 0;
					sizeToLock = ( m_shape.m_nVertices + m_shape2.m_nVertices ) * sizeof(D3DVERTEX);
				}
				else
				{
					offset = m_shape.m_nVertices * sizeof(D3DVERTEX);
					sizeToLock = m_shape2.m_nVertices * sizeof(D3DVERTEX);
				}
				
				
				szLastCall = TEXT("VB->Lock()");
				bResult = (*ppVB)->Lock(
					offset,
					sizeToLock,
					&pbObject,
					LockFlags,
					dwFlags
				);
				if( !bResult )
				{
					WriteToLog(
						TEXT("VB::Lock(%d,%d,,0x%x) failed.\n"),
						offset,
						sizeToLock,
						LockFlags
					);
					continue;
				}

				szLastCall = TEXT("CopyMemory(,VB,)");
				if (LockFlags & LOCK_DISCARD)
				{
					CopyMemory( pbObject, m_shape.m_pVertices, m_shape.m_nVertices * sizeof(D3DVERTEX) );
					CopyMemory( pbObject+m_shape.m_nVertices*sizeof(D3DVERTEX), m_shape2.m_pVertices, m_shape2.m_nVertices * sizeof(D3DVERTEX) );
				}
				else
					CopyMemory( pbObject, m_shape2.m_pVertices, m_shape2.m_nVertices * sizeof(D3DVERTEX) );

				szLastCall = TEXT("VB->Unlock()");
				bResult = (*ppVB)->Unlock(dwFlags);
				if( !bResult )
				{
					WriteToLog(
						TEXT("VB::Unlock() failed.\n")
					);
					continue;
				}
			}


			if ( Stages[ iTestStage ].fAppendIB )  // add second object to ib
			{
				UINT offset;
				UINT sizeToLock;
				if (LockFlags & LOCK_DISCARD)
				{
					offset = 0;
					sizeToLock = ( m_shape.m_nIndices + m_shape2.m_nIndices ) * sizeof(WORD);
				}
				else
				{
					offset = m_shape.m_nIndices * sizeof(WORD);
					sizeToLock = m_shape2.m_nIndices * sizeof(WORD);
				}
				szLastCall = TEXT("IB->Lock()");
				bResult = (*ppIB)->Lock(
					offset,
					sizeToLock,
					&pbObject,
					LockFlags,
					dwFlags
				);
				if( !bResult )
				{
					WriteToLog(
						TEXT("IB::Lock(%d,%d,,0x%x) failed.\n"),
						offset,
						sizeToLock,
						LockFlags
					);
					continue;
				}

				szLastCall = TEXT("IB mem copy");
				if (LockFlags & LOCK_DISCARD)
				{
					CopyMemory( pbObject, m_shape.m_pIndices, m_shape.m_nIndices * sizeof(WORD) );
					for ( UINT iIndex = 0; iIndex < m_shape2.m_nIndices; iIndex++ )
					{
						((WORD*)pbObject)[iIndex+m_shape.m_nIndices] = m_shape2.m_pIndices[iIndex] + m_shape.m_nVertices;
					}
				}
				else
				{
					for ( UINT iIndex = 0; iIndex < m_shape2.m_nIndices; iIndex++ )
					{
						((WORD*)pbObject)[iIndex] = m_shape2.m_pIndices[iIndex] + m_shape.m_nVertices;
					}
				}

				szLastCall = TEXT("IB->Unlock()");
				bResult = (*ppIB)->Unlock(dwFlags);
				if( !bResult )
				{
					WriteToLog(
						TEXT("IB::Unlock() failed.\n")
					);
					continue;
				}
			}

			Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0, dwFlags );

			szLastCall = TEXT("BeginScene()");
			bResult = BeginScene(dwFlags);
			if( !bResult )
			{
				WriteToLog(
					TEXT("BeginScene() failed.\n")
					);
				continue;
			}

			bRet = TRUE;

			do
			{
				szLastCall = TEXT("SetVertexShader()");
				bResult = SetVertexShader( D3DFVF_VERTEX, dwFlags );
				if( !bResult )
				{
					WriteToLog(
						TEXT("SetVertexShader(D3DFVF_VERTEX) failed\n")
					);
					bRet = FALSE;
					continue;
				}

				if ( Stages[ iTestStage ].fDraw1 )  // draw first opject
				{
					szLastCall = TEXT("DrawIndexedPrimitive(1)");
					bResult = DrawIndexedPrimitive(
						D3DPT_TRIANGLELIST,
						0,
						m_shape.m_nVertices,
						0,
						m_shape.m_nIndices / 3,
						0,
						dwFlags
					);
					if( !bResult )
					{
						WriteToLog(
							TEXT("DrawIndexedPrimitive()failed.\n")
						 );
						bRet = FALSE;
						continue;
					}
				}

				if ( Stages[ iTestStage ].fDraw2 )  // draw second object
				{
					szLastCall = TEXT("DrawIndexedPrimitive(2)");
					bResult = DrawIndexedPrimitive(
						D3DPT_TRIANGLELIST,
						m_shape.m_nVertices,
						m_shape2.m_nVertices,
						m_shape.m_nIndices,
						m_shape2.m_nIndices / 3,
						0,
						dwFlags
					);
					if( !bResult )
					{
						WriteToLog(
							TEXT("DrawIndexedPrimitive()failed.\n")
						);
						bRet = FALSE;
						continue;
					}
				}
			}  while( 0 );  // execute once, so continue can bust out

			bResult = EndScene(dwFlags);
			if( !bResult )
			{
				WriteToLog(
					TEXT("EndScene() failed.\n")
				);
				bRet = FALSE;
				continue;
			}
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
				TEXT("%s caused unhandled exception\n"),
				szLastCall
			);

			bRet = FALSE;
		};
	} while( 0 );


	return bRet;
};

