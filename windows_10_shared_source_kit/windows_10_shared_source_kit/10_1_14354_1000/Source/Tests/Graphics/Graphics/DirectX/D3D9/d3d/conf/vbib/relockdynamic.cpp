//////////////////////////////////////////////////////////////////////
// File:  RelockDynamic.cpp
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
const DWORD RelockDynamic::Usages[] =
{
    USAGE_DYNAMIC,
    USAGE_DYNAMIC | USAGE_WRITEONLY,
    USAGE_DYNAMIC | USAGE_DONOTCLIP,
    USAGE_DYNAMIC | USAGE_POINTS,
    USAGE_DYNAMIC | USAGE_RTPATCHES,
    USAGE_DYNAMIC | USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_DYNAMIC | USAGE_POINTS | USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_DYNAMIC | USAGE_RTPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
};

const DWORD RelockDynamic::Pools[] =
{
    POOL_DEFAULT,
    POOL_SYSTEMMEM,
};

const DWORD RelockDynamic::LockFlags[] =
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

const DWORD RelockDynamic::Usages[] =
{
	USAGE_DYNAMIC,
	USAGE_DYNAMIC | USAGE_WRITEONLY,
	USAGE_DYNAMIC | USAGE_DONOTCLIP,
	USAGE_DYNAMIC | USAGE_POINTS,
	USAGE_DYNAMIC | USAGE_RTPATCHES,
	USAGE_DYNAMIC | USAGE_NPATCHES,
	USAGE_DYNAMIC | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_DYNAMIC | USAGE_POINTS | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_DYNAMIC | USAGE_RTPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_DYNAMIC | USAGE_NPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_DYNAMIC | USAGE_POINTS | USAGE_RTPATCHES | USAGE_NPATCHES,
};


const DWORD RelockDynamic::Pools[] =
{
	POOL_DEFAULT,
	POOL_SYSTEMMEM,
};


const DWORD RelockDynamic::LockFlags[] =
{
	0,
	LOCK_DISCARD,
	LOCK_NOOVERWRITE,
	LOCK_NOSYSLOCK,
	LOCK_DISCARD|LOCK_NOOVERWRITE,
	LOCK_DISCARD|LOCK_NOSYSLOCK,
	LOCK_NOOVERWRITE|LOCK_NOSYSLOCK,
	LOCK_DISCARD|LOCK_NOOVERWRITE|LOCK_NOSYSLOCK,
};

#endif //FAST_TEST


static RelockDynamicTestCase s_RelockDynamicTestCases
[
	COUNTOF( RelockDynamic::Usages ) *
	COUNTOF( RelockDynamic::Pools ) *
	COUNTOF( RelockDynamic::LockFlags ) *
	COUNTOF( RelockDynamic::LockFlags )
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
// Function:  RelockDynamic
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
RelockDynamic::RelockDynamic()
{
	m_szTestName = _T("VBIB Relock Dynamic");
	m_szCommandKey = _T("RelockDynamic");
};


///////////////////////////////////////////////////////
//
// Framework method overrides
//
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  TestInitialize
//
// Purpose:
// Test group initialization.
///////////////////////////////////////////////////////
UINT RelockDynamic::TestInitialize()
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
			for ( UINT iCurrentLockFlag1 = 0; iCurrentLockFlag1 < COUNTOF( LockFlags ); ++iCurrentLockFlag1 )
			{
				for ( UINT iCurrentLockFlag2 = 0; iCurrentLockFlag2 < COUNTOF( LockFlags ); ++iCurrentLockFlag2 )
				{
					ASSERT( cTestCases < COUNTOF( s_RelockDynamicTestCases ) );
/*
					WriteToLog
					(
						_T( "Test case %u:  0x%x, %u, 0x%x, 0x%x." ),
						cTestCases,
						Usages[ iCurrentUsage ],
						Pools[ iCurrentPool ],
						LockFlags[ iCurrentLockFlag1 ],
						LockFlags[ iCurrentLockFlag2 ]
					);
*/

					s_RelockDynamicTestCases[ cTestCases ].nTest = cTestCases;
					s_RelockDynamicTestCases[ cTestCases ].Usage = Usages[ iCurrentUsage ];
					s_RelockDynamicTestCases[ cTestCases ].Pool = Pools[ iCurrentPool ];
					s_RelockDynamicTestCases[ cTestCases ].LockFlag1 = LockFlags[ iCurrentLockFlag1 ];
					s_RelockDynamicTestCases[ cTestCases ].LockFlag2 = LockFlags[ iCurrentLockFlag2 ];

					++cTestCases;
				};
			};
		};
	};


	//
	// Set the test range.
	//

	//WriteToLog( _T( "%u test cases found." ), cTestCases );
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

	UINT uiRet = D3DTESTINIT_RUN;

	do
	{
		uiRet = TestInitReal(CD3D_SRC);
		if ( uiRet != D3DTESTINIT_RUN )
		{
			continue;
		};

		uiRet = TestInitReal(CD3D_REF);
		if ( uiRet != D3DTESTINIT_RUN )
		{
			continue;
		};
	} while( 0 );


	return uiRet;
};


///////////////////////////////////////////////////////
// Function:  ExecuteTest
//
// Purpose:
// Runs a test case.
///////////////////////////////////////////////////////
#if FAST_TEST
bool RelockDynamic::ExecuteTest
(
IN UINT nTest
)
{
    //
    // Begin the test case.
    //

    BeginTestCase("Relock Dynamic - Fast", nTest);


    //
    // Clear the frame in preparation for rendering.
    //

    ClearFrame();


    auto& drawLamda = [=](UINT testIndex) -> bool
    {
        //
        // Validate the test case ordinal.
        //

        if (testIndex > COUNTOF(s_RelockDynamicTestCases))
        {
            WriteToLog
                (
                TEXT("Invalid test ordinal %u.\n"),
                testIndex
                );

            return false;
        };

        //
        // Hand off to actually run the test.
        //

        BOOL bRetTest = ExecuteReal
            (
            s_RelockDynamicTestCases[testIndex - 1].Usage,
            s_RelockDynamicTestCases[testIndex - 1].Pool,
            s_RelockDynamicTestCases[testIndex - 1].LockFlag1,
            s_RelockDynamicTestCases[testIndex - 1].LockFlag2,
            (m_pSrcDevice->GetBehaviors() & DEVICECREATE_MIXED),
            CD3D_SRC
            );

        BOOL bRetRef = ExecuteReal
            (
            s_RelockDynamicTestCases[testIndex - 1].Usage,
            s_RelockDynamicTestCases[testIndex - 1].Pool,
            s_RelockDynamicTestCases[testIndex - 1].LockFlag1,
            s_RelockDynamicTestCases[testIndex - 1].LockFlag2,
            (m_pRefDevice->GetBehaviors() & DEVICECREATE_MIXED),
            CD3D_REF
            );


        bool bRet = (bRetTest && bRetRef);
        if (!bRet)
        {
            Fail();
        };

        return bRet;
    };

    return DrawMultiTests(drawLamda);
};
#else //FAST_TEST
bool RelockDynamic::ExecuteTest
(
	IN UINT nTest
)
{
	//
	// Validate the test case ordinal.
	//

	if ( nTest > COUNTOF( s_RelockDynamicTestCases ) )
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
		TEXT("Usage = 0x%x, Pool = %u, LockFlags = 0x%x, 0x%x"),
		s_RelockDynamicTestCases[ nTest - 1 ].Usage,
		s_RelockDynamicTestCases[ nTest - 1 ].Pool,
		s_RelockDynamicTestCases[ nTest - 1 ].LockFlag1,
		s_RelockDynamicTestCases[ nTest - 1 ].LockFlag2
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
		s_RelockDynamicTestCases[ nTest - 1 ].Usage,
		s_RelockDynamicTestCases[ nTest - 1 ].Pool,
		s_RelockDynamicTestCases[ nTest - 1 ].LockFlag1,
		s_RelockDynamicTestCases[ nTest - 1 ].LockFlag2,
		(m_pSrcDevice->GetBehaviors() & DEVICECREATE_MIXED),
		CD3D_SRC
	);

	BOOL bRetRef = ExecuteReal
	(
		s_RelockDynamicTestCases[ nTest - 1 ].Usage,
		s_RelockDynamicTestCases[ nTest - 1 ].Pool,
		s_RelockDynamicTestCases[ nTest - 1 ].LockFlag1,
		s_RelockDynamicTestCases[ nTest - 1 ].LockFlag2,
		(m_pRefDevice->GetBehaviors() & DEVICECREATE_MIXED),
		CD3D_REF
	);


	bool bRet = ( bRetTest && bRetRef );
	if ( !bRet )
	{
		Fail();
	};

	return bRet;
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
D3DTESTINIT RelockDynamic::TestInitReal
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
		if ( false == bResult )
		{
			WriteToLog
			(
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
		if ( false == bResult )
		{
			WriteToLog
			(
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
		if ( false == bResult )
		{
			WriteToLog
			(
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

		bResult = SetMaterial(
			&mtrl,
			dwFlags
		);
		if ( false == bResult )
		{
			WriteToLog
			(
				TEXT("SetMaterial()  failed.\n")
			);

			continue;
		}
	}
	while(0);


	return( false == bResult ? D3DTESTINIT_ABORT : D3DTESTINIT_RUN);
};


///////////////////////////////////////////////////////
// Function:  ExecuteReal
//
// Purpose:
// Handles the heavy lifting of running a test case.
///////////////////////////////////////////////////////
BOOL RelockDynamic::ExecuteReal
(
	IN DWORD Usage,
	IN DWORD Pool,
	IN DWORD LockFlags1,
	IN DWORD LockFlags2,
	IN BOOL fMixedMode,
	IN DWORD dwFlags
)
{
	//
	// Initialize locals.
	//

	bool bResult = true;
	LPBYTE pbObject = NULL;
	BOOL bOverallResult = FALSE;
	LPVERTEXBUFFERS pVB = NULL;
	LPINDEXBUFFERS pIB = NULL;


	if ( ( dwFlags & CD3D_REF ) != 0 )
	{
		WriteToLog
		(
			TEXT("Processing reference scenario.")
		);
	}
	else
	{
		WriteToLog
		(
			TEXT("Processing test scenario.")
		);
	};


	//
	// Determine whether to use software vertex processing.
	//

	BOOL fSoftwareUsage = FALSE;
	if ( fMixedMode )
	{
		fSoftwareUsage = GetSoftwareVertexProcessing( dwFlags );
	};


	//
	// Create vertex buffer.
	//

	WriteToLog
	(
		TEXT("Calling CreateVertexBuffer.")
	);

	bResult = CreateVertexBuffer
	(
		(m_shape.m_nVertices + m_shape2.m_nVertices) * sizeof(D3DVERTEX),
		Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : NULL),
		D3DFVF_VERTEX,
		Pool,
		&pVB,
		dwFlags
	);

	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed.\n"),
			(m_shape.m_nVertices + m_shape2.m_nVertices) * sizeof(D3DVERTEX),
			Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : NULL),
			D3DFVF_VERTEX,
			Pool
		);

		goto Cleanup;
	};

	if ( pVB == NULL )
	{
		WriteToLog
		(
			TEXT("CreateVertexBuffer succeeded, but pVB == NULL.\n")
		);

		goto Cleanup;
	};


	//
	// Create index buffer.
	//

	WriteToLog
	(
		TEXT("Calling CreateIndexBuffer.")
	);

	bResult = CreateIndexBuffer
	(
		(m_shape.m_nIndices + m_shape2.m_nIndices) * sizeof(WORD),
		Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : NULL),
		FMT_INDEX16,
		Pool,
		&pIB,
		dwFlags
	);

	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,) failed.\n"),
			(m_shape.m_nIndices + m_shape2.m_nIndices) * sizeof(WORD),
			Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : NULL),
			D3DFMT_INDEX16,
			Pool
		);

		goto Cleanup;
	}

	if ( pIB == NULL )
	{
		WriteToLog
		(
			TEXT("CreateIndexBuffer succeeded, but pIB == NULL.\n" )
		);

		goto Cleanup;
	};


	//
	// Lock vertex buffer the first time.
	//

	WriteToLog
	(
		TEXT("Locking vertex buffer.")
	);

	bResult = pVB->Lock
	(
		0,
		m_shape.m_nVertices * sizeof(D3DVERTEX),
		&pbObject,
		LockFlags1,
		dwFlags
	);

	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("VB::Lock(0,%d,,0x%x) failed.\n"),
			m_shape.m_nVertices * sizeof(D3DVERTEX),
			LockFlags1
		);

		goto Cleanup;
	};

	if ( pbObject == NULL )
	{
		WriteToLog
		(
			TEXT("VB::Lock succeeded, but pbObject == NULL.\n" )
		);

		goto Cleanup;
	};


	//
	// Fill the vertex buffer with the first shape.
	//

	WriteToLog
	(
		TEXT("Copying first shape into vertex buffer.")
	);

	ASSERT( pbObject != NULL );
	ASSERT( m_shape.m_pVertices != NULL );
	CopyMemory
	(
		pbObject,
		m_shape.m_pVertices,
		m_shape.m_nVertices * sizeof(D3DVERTEX)
	);


	//
	// Lock the index buffer the first time.
	//

	WriteToLog
	(
		TEXT("Locking index buffer.")
	);

	bResult = pIB->Lock
	(
		0,
		m_shape.m_nIndices * sizeof(WORD),
		&pbObject,
		LockFlags1,
		dwFlags
	);

	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("IB::Lock(0,%d,,0x%x) failed.\n"),
			m_shape.m_nIndices * sizeof(WORD),
			LockFlags1
		);

		goto Cleanup;
	};

	if ( pbObject == NULL )
	{
		WriteToLog
		(
			TEXT("IB::Lock succeeded, but pbObject == NULL.\n" )
		);

		goto Cleanup;
	};


	//
	// Fill the index buffer with the first shape.
	//

	WriteToLog
	(
		TEXT("Copying first shape into index buffer.")
	);

	ASSERT( pbObject != NULL );
	ASSERT( m_shape.m_pIndices != NULL );
	CopyMemory
	(
		pbObject,
		m_shape.m_pIndices,
		m_shape.m_nIndices * sizeof(WORD)
	);


	//
	// Determine the region of the vertex buffer to lock
	// the second time around.
	//

	UINT VBLockOffset = 0;
	UINT VBLockRegionSize = 0;
	if (LockFlags2 & LOCK_DISCARD)
	{
		VBLockOffset = 0;
		VBLockRegionSize = ( m_shape.m_nVertices + m_shape2.m_nVertices ) * sizeof(D3DVERTEX);
	}
	else
	{
		VBLockOffset = m_shape.m_nVertices * sizeof(D3DVERTEX);
		VBLockRegionSize = m_shape2.m_nVertices * sizeof(D3DVERTEX);
	};


	//
	// Lock the vertex buffer the second time.
	//

	WriteToLog
	(
		TEXT("Locking vertex buffer again.")
	);

	bResult = pVB->Lock
	(
		VBLockOffset,
		VBLockRegionSize,
		&pbObject,
		LockFlags2,
		dwFlags
	);

	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("VB::Lock(%d,%d,,0x%x) failed.\n"),
			VBLockOffset,
			VBLockRegionSize,
			LockFlags2
		);

		goto Cleanup;
	};

	if ( pbObject == NULL )
	{
		WriteToLog
		(
			TEXT("VB::Lock succeeded, but pbObject == NULL.\n" )
		);

		goto Cleanup;
	};


	//
	// Add the second shape to the vertex buffer.
	//

	WriteToLog
	(
		TEXT("Copying second shape into vertex buffer.")
	);

	if (LockFlags2 & LOCK_DISCARD)
	{
		ASSERT( pbObject != NULL );
		ASSERT( m_shape.m_pVertices != NULL );
		CopyMemory
		(
			pbObject,
			m_shape.m_pVertices,
			m_shape.m_nVertices * sizeof(D3DVERTEX)
		);

		ASSERT( m_shape2.m_pVertices != NULL );
		CopyMemory
		(
			pbObject + m_shape.m_nVertices * sizeof(D3DVERTEX),
			m_shape2.m_pVertices,
			m_shape2.m_nVertices * sizeof(D3DVERTEX)
		);
	}
	else
	{
		ASSERT( pbObject != NULL );
		ASSERT( m_shape2.m_pVertices != NULL );
		CopyMemory
		(
			pbObject,
			m_shape2.m_pVertices,
			m_shape2.m_nVertices * sizeof(D3DVERTEX)
		);
	};


	//
	// Determine the region of the index buffer to lock
	// the second time around.
	//

	UINT IBLockOffset = 0;
	UINT IBLockRegionSize = 0;
	if (LockFlags2 & LOCK_DISCARD)
	{
		IBLockOffset = 0;
		IBLockRegionSize = ( m_shape.m_nIndices + m_shape2.m_nIndices ) * sizeof(WORD);
	}
	else
	{
		IBLockOffset = m_shape.m_nIndices * sizeof(WORD);
		IBLockRegionSize = m_shape2.m_nIndices * sizeof(WORD);
	};


	//
	// Lock the index buffer the second time.
	//

	WriteToLog
	(
		TEXT("Locking index buffer again.")
	);

	bResult = pIB->Lock
	(
		IBLockOffset,
		IBLockRegionSize,
		&pbObject,
		LockFlags2,
		dwFlags
	);

	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("IB::Lock(%d,%d,,0x%x) failed.\n"),
			IBLockOffset,
			IBLockRegionSize,
			LockFlags2
		);

		goto Cleanup;
	};

	if ( pbObject == NULL )
	{
		WriteToLog
		(
			TEXT("IB::Lock succeeded, but pbObject == NULL.\n")
		);

		goto Cleanup;
	};


	//
	// Add the second shape to the index buffer.
	//

	WriteToLog
	(
		TEXT("Copying second shape into index buffer.")
	);

	if (LockFlags2 & LOCK_DISCARD)
	{
		ASSERT( pbObject != NULL );
		ASSERT( m_shape.m_pIndices != NULL );
		CopyMemory
		(
			pbObject,
			m_shape.m_pIndices,
			m_shape.m_nIndices * sizeof(WORD)
		);

		for( UINT iIndex = 0; iIndex < m_shape2.m_nIndices; iIndex++ )
		{
			((WORD*)pbObject)[iIndex+m_shape.m_nIndices] = m_shape2.m_pIndices[iIndex] + m_shape.m_nVertices;
		}
	}
	else
	{
		for( UINT iIndex = 0; iIndex < m_shape2.m_nIndices; iIndex++ )
		{
			((WORD*)pbObject)[iIndex] = m_shape2.m_pIndices[iIndex] + m_shape.m_nVertices;
		}
	};


	//
	// Unlock the second vertex buffer lock.
	//

	WriteToLog
	(
		TEXT("Unlocking vertex buffer.")
	);

	bResult = pVB->Unlock(dwFlags);
	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("VB::Unlock() failed.\n")
		);

		goto Cleanup;
	};


	//
	// Unlock the second index buffer lock.
	//

	WriteToLog
	(
		TEXT("Unlocking index buffer.")
	);

	bResult = pIB->Unlock(dwFlags);
	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("IB::Unlock() failed.\n")
		);

		goto Cleanup;
	};


	//
	// Unlock the first vertex buffer lock.
	//

	WriteToLog
	(
		TEXT("Unlocking vertex buffer again.")
	);

	bResult = pVB->Unlock(dwFlags);
	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("VB::Unlock() failed.\n")
		);

		goto Cleanup;
	};


	//
	// Unlock the first index buffer lock.
	//

	WriteToLog
	(
		TEXT("Unlocking index buffer again.")
	);

	bResult = pIB->Unlock(dwFlags);
	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("IB::Unlock() failed.\n")
		);

		goto Cleanup;
	};


	//
	// Set the stream source to be our vertex buffer.
	//

	WriteToLog
	(
		TEXT("Setting stream source.")
	);

	bResult = SetStreamSource
	(
		0,
		pVB,
		sizeof(D3DVERTEX),
		0,
		dwFlags
	);

	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("SetStreamSource() failed.\n")
		);

		goto Cleanup;
	};


	//
	// Set indices to our index buffer.
	//

	WriteToLog
	(
		TEXT("Calling SetIndices.")
	);

	bResult = SetIndices
	(
		pIB,
		0,
		dwFlags
	);

	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("SetIndices() failed.\n")
		);

		goto Cleanup;
	};


	//
	// Draw the scene.
	//

	WriteToLog
	(
		TEXT("Drawing the scene.")
	);

	Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0, dwFlags );

	bResult = BeginScene(dwFlags);
	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("BeginScene() failed.\n")
		);

		goto Cleanup;
	};



	bResult = SetVertexShader( D3DFVF_VERTEX, dwFlags );
	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("SetVertexShader(D3DFVF_VERTEX) failed.\n")
		);

		goto Cleanup;
	}


	bResult = DrawIndexedPrimitive
	(
		D3DPT_TRIANGLELIST,
		0,
		m_shape.m_nVertices + m_shape2.m_nVertices,
		0,
		(m_shape.m_nIndices + m_shape2.m_nIndices) / 3,
		0,
		dwFlags
	);

	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("DrawIndexedPrimitive() failed.\n")
		);

		goto Cleanup;
	};


	bResult = EndScene(dwFlags);
	if ( false == bResult )
	{
		WriteToLog
		(
			TEXT("EndScene() failed.\n")
		);

		goto Cleanup;
	};


	bOverallResult = TRUE;


Cleanup:
	//
	// Clean up.
	//

	RELEASE(pVB);
	RELEASE(pIB);

	return bOverallResult;
};
