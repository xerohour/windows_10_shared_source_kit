//////////////////////////////////////////////////////////////////////
// File:  DiscardDynamic.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of vertex and index buffer conformance tests.
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
#include "cbuffers.h"
#include <d3d8.h>
#include <d3dx8.h>


/////////////////////////////////////////////////////////////////
//
// Statics and globals
//
/////////////////////////////////////////////////////////////////
#if FAST_TEST
const DWORD DiscardDynamic::Usages[] =
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

const DWORD DiscardDynamic::Pools[] =
{
    POOL_DEFAULT,
    POOL_SYSTEMMEM,
};


const DWORD DiscardDynamic::LockFlags[] =
{
    0,
    LOCK_DISCARD,
    LOCK_NOSYSLOCK,
    LOCK_DISCARD | LOCK_NOSYSLOCK,
};
#else //FAST_TEST
const DWORD DiscardDynamic::Usages[] =
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


const DWORD DiscardDynamic::Pools[] =
{
	POOL_DEFAULT,
	POOL_SYSTEMMEM,
};


const DWORD DiscardDynamic::LockFlags[] =
{
	0,
	LOCK_DISCARD,
	LOCK_NOSYSLOCK,
	LOCK_DISCARD|LOCK_NOSYSLOCK,
};
#endif

static DiscardDynamicTestCase s_DiscardDynamicTestCases
[
	COUNTOF( DiscardDynamic::Usages ) *
	COUNTOF( DiscardDynamic::Pools ) *
	COUNTOF( DiscardDynamic::LockFlags ) *
	COUNTOF( DiscardDynamic::LockFlags )
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
// Function:  DiscardDynamic
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
DiscardDynamic::DiscardDynamic()
{
	m_szTestName = _T("VBIB Discard Dynamic");
	m_szCommandKey = _T("DiscardDynamic");
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
UINT DiscardDynamic::TestInitialize()
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
					ASSERT( cTestCases < COUNTOF( s_DiscardDynamicTestCases ) );

					s_DiscardDynamicTestCases[ cTestCases ].nTest = cTestCases;
					s_DiscardDynamicTestCases[ cTestCases ].Usage = Usages[ iCurrentUsage ];
					s_DiscardDynamicTestCases[ cTestCases ].Pool = Pools[ iCurrentPool ];
					s_DiscardDynamicTestCases[ cTestCases ].LockFlag1 = LockFlags[ iCurrentLockFlag1 ];
					s_DiscardDynamicTestCases[ cTestCases ].LockFlag2 = LockFlags[ iCurrentLockFlag2 ];

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
	// Hand off to finish initialization.
	//

	UINT uiRet = D3DTESTINIT_RUN;

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
	} while( 0 );


	return uiRet;
}


///////////////////////////////////////////////////////
// Function:  ExecuteTest
//
// Purpose:
// Runs a test case.
///////////////////////////////////////////////////////
#if FAST_TEST
bool DiscardDynamic::ExecuteTest
(
UINT nTest
)
{

    //
    // Begin the test case.
    //

    BeginTestCase("Dynamic Discard - Fast", nTest);


    //
    // Clear the frame in preparation for rendering.
    //

    ClearFrame();

    auto& drawLamda = [=](UINT testIndex) -> bool
    {
        //
        // Validate the test case ordinal.
        //

        if (testIndex > COUNTOF(s_DiscardDynamicTestCases))
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
            s_DiscardDynamicTestCases[testIndex - 1].Usage,
            s_DiscardDynamicTestCases[testIndex - 1].Pool,
            s_DiscardDynamicTestCases[testIndex - 1].LockFlag1,
            s_DiscardDynamicTestCases[testIndex - 1].LockFlag2,
            (m_pSrcDevice->GetBehaviors() & DEVICECREATE_MIXED),
            CD3D_SRC
            );

        BOOL bRetRef = ExecuteReal
            (
            s_DiscardDynamicTestCases[testIndex - 1].Usage,
            s_DiscardDynamicTestCases[testIndex - 1].Pool,
            s_DiscardDynamicTestCases[testIndex - 1].LockFlag1,
            s_DiscardDynamicTestCases[testIndex - 1].LockFlag2,
            (m_pRefDevice->GetBehaviors() & DEVICECREATE_MIXED),
            CD3D_REF
            );


        bool bRet = bRetTest && bRetRef;
        if (!bRet)
        {
            Fail();
        };

        return bRet;
    };

    return DrawMultiTests(drawLamda); 
};
#else //FAST_TEST
bool DiscardDynamic::ExecuteTest
(
	UINT nTest
)
{
	//
	// Validate the test case ordinal.
	//

	if ( nTest > COUNTOF( s_DiscardDynamicTestCases ) )
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
		TEXT("Usage = 0x%x, Pool = %d, LockFlags = 0x%x, 0x%x"),
		s_DiscardDynamicTestCases[ nTest - 1 ].Usage,
		s_DiscardDynamicTestCases[ nTest - 1 ].Pool,
		s_DiscardDynamicTestCases[ nTest - 1 ].LockFlag1,
		s_DiscardDynamicTestCases[ nTest - 1 ].LockFlag2
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
		s_DiscardDynamicTestCases[ nTest - 1 ].Usage,
		s_DiscardDynamicTestCases[ nTest - 1 ].Pool,
		s_DiscardDynamicTestCases[ nTest - 1 ].LockFlag1,
		s_DiscardDynamicTestCases[ nTest - 1 ].LockFlag2,
		(m_pSrcDevice->GetBehaviors() & DEVICECREATE_MIXED),
		CD3D_SRC
	);

	BOOL bRetRef = ExecuteReal
	(
		s_DiscardDynamicTestCases[ nTest - 1 ].Usage,
		s_DiscardDynamicTestCases[ nTest - 1 ].Pool,
		s_DiscardDynamicTestCases[ nTest - 1 ].LockFlag1,
		s_DiscardDynamicTestCases[ nTest - 1 ].LockFlag2,
		(m_pRefDevice->GetBehaviors() & DEVICECREATE_MIXED),
		CD3D_REF
	);


	bool bRet = bRetTest && bRetRef;
	if( !bRet )
	{
		Fail();
	};

	return bRet;
};
#endif//FAST_TEST

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
D3DTESTINIT DiscardDynamic::TestInitReal
(
	DWORD dwFlags
)
{
	bool bResult = false;


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

		return D3DTESTINIT_ABORT;
	}

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

		return D3DTESTINIT_ABORT;
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
	if( !bResult )
	{
		WriteToLog
		(
			TEXT("SetTransform(D3DTS_WORLD,) failed.\n")
		);

		return D3DTESTINIT_ABORT;
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
	if( !bResult )
	{
		WriteToLog
		(
			TEXT("SetMaterial() failed.\n")
		);

		return D3DTESTINIT_ABORT;
	}


	return D3DTESTINIT_RUN;
}


///////////////////////////////////////////////////////
// Function:  ExecuteReal
//
// Purpose:
// Handles the heavy lifting of running a test case.
///////////////////////////////////////////////////////
BOOL DiscardDynamic::ExecuteReal
(
	DWORD Usage,
	DWORD Pool,
	DWORD LockFlags1,
	DWORD LockFlags2,
	BOOL fMixedMode,
	DWORD dwFlags
)
{
	bool bResult = true;
	LPBYTE pbObject = NULL;
	BOOL bRet = FALSE;
	LPVERTEXBUFFERS pVB = NULL;
	LPINDEXBUFFERS pIB = NULL;


	Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0, dwFlags );

	bResult = BeginScene( dwFlags );
	if( !bResult )
	{
		WriteToLog
		(
			TEXT("BeginScene() failed.\n")
		);

		goto Cleanup;
	};

	/*BOOL fSoftwareUsage = FALSE;

	// determine sw usage
	if( fMixedMode )
	{
		DWORD dwSW;
		GetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, &dwSW, dwFlags );
		fSoftwareUsage = dwSW;
	}*/
	BOOL fSoftwareUsage = FALSE;

	// determine sw usage
	if( fMixedMode )
		fSoftwareUsage = GetSoftwareVertexProcessing(dwFlags );


	//
	// Create vertex buffer.
	//

	bResult = CreateVertexBuffer
	(
		(m_shape.m_nVertices + m_shape2.m_nVertices) * sizeof(D3DVERTEX),
		Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : NULL),
		D3DFVF_VERTEX,
		Pool,
		&pVB,
		dwFlags
	);

	if( !bResult )
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
	}

	if ( pVB == NULL )
	{
		WriteToLog
		(
			TEXT("CreateVertexBuffer succeeded, but pVB == NULL.\n")
		);

		goto Cleanup;
	}


	//
	// Create index buffer.
	//

	bResult = CreateIndexBuffer
	(
		(m_shape.m_nIndices + m_shape2.m_nIndices) * sizeof(WORD),
		Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : NULL),
		FMT_INDEX16,
		Pool,
		&pIB,
		dwFlags
	);

	if( !bResult )
	{
		WriteToLog
		(
			TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,) failed.\n"),
			(m_shape.m_nIndices + m_shape2.m_nIndices) * sizeof(WORD),
			Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : NULL),
			FMT_INDEX16,
			Pool
		);

		goto Cleanup;
	}

	if ( pIB == NULL )
	{
		WriteToLog
		(
			TEXT("CreateIndexBuffer succeeded, but pIB == NULL.\n")
		);

		goto Cleanup;
	}


	//
	// Set the stream source to the vertex buffer.
	//

	bResult = SetStreamSource
	(
		0,
		pVB,
		sizeof(D3DVERTEX),
		0,
		dwFlags
	);
	if( !bResult )
	{
		WriteToLog
		(
			TEXT("SetStreamSource() failed.\n")
		);

		goto Cleanup;
	}


	//
	// Set indices to the index buffer.
	//

	bResult = SetIndices
	(
		pIB,
		0,
		dwFlags
	);
	if( !bResult )
	{
		WriteToLog
		(
			TEXT("SetIndices() returned failed.\n")
		);

		goto Cleanup;
	}


	bResult = SetVertexShader( D3DFVF_VERTEX, dwFlags );
	if( !bResult )
	{
		WriteToLog
		(
			TEXT("SetVertexShader(D3DFVF_VERTEX) failed.\n")
		);

		goto Cleanup;
	}


	//
	// Lock the vertex buffer the first time.
	//

	bResult = pVB->Lock
	(
		0,
		m_shape.m_nVertices * sizeof(D3DVERTEX),
		&pbObject,
		LockFlags1,
		dwFlags
	);

	if( !bResult )
	{
		WriteToLog
		(
			TEXT("VB::Lock(0,%d,,0x%x) failed.\n"),
			m_shape.m_nVertices * sizeof(D3DVERTEX),
			LockFlags1
		);

		goto Cleanup;
	}

	if ( pbObject == NULL )
	{
		WriteToLog
		(
			TEXT("VB::Lock succeeded, but pbObject == NULL.\n")
		);

		goto Cleanup;
	}


	//
	// Fill the vertex buffer with the first object.
	//

	ASSERT( pbObject != NULL );
	ASSERT( m_shape.m_pVertices != NULL );
	CopyMemory
	(
		pbObject,
		m_shape.m_pVertices,
		m_shape.m_nVertices * sizeof(D3DVERTEX)
	);


	//
	// Unlock the vertex buffer.
	//

	bResult = pVB->Unlock(dwFlags);
	if( !bResult )
	{
		WriteToLog
		(
			TEXT("VB::Unlock() failed.\n")
		);

		goto Cleanup;
	}


	//
	// Lock the index buffer the first time.
	//

	bResult = pIB->Lock
	(
		0,
		m_shape.m_nIndices * sizeof(WORD),
		&pbObject,
		LockFlags1,
		dwFlags
	);

	if( !bResult )
	{
		WriteToLog
		(
			TEXT("IB::Lock(0,%d,,0x%x) failed.\n"),
			m_shape.m_nIndices * sizeof(WORD),
			LockFlags1
		);

		goto Cleanup;
	}

	if ( pbObject == NULL )
	{
		WriteToLog
		(
			TEXT("IB::Lock succeeded, but pbObject == NULL.\n")
		);

		goto Cleanup;
	}


	//
	// Fill the index buffer with the first object.
	//

	ASSERT( pbObject != NULL );
	ASSERT( m_shape.m_pIndices != NULL );
	CopyMemory
	(
		pbObject,
		m_shape.m_pIndices,
		m_shape.m_nIndices * sizeof(WORD)
	);


	//
	// Unlock the index buffer.
	//

	bResult = pIB->Unlock(dwFlags);
	if( !bResult )
	{
		WriteToLog
		(
			TEXT("IB::Unlock() failed.\n")
		);

		goto Cleanup;
	}


	//
	// Draw the first object.
	//

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
	if( !bResult )
	{
		WriteToLog
		(
			TEXT("DrawIndexedPrimitive() failed.\n")
		);


		goto Cleanup;
	}


	//
	// Lock the vertex buffer the second time.
	//

	bResult = pVB->Lock
	(
		0,
		m_shape2.m_nVertices * sizeof(D3DVERTEX),
		&pbObject,
		LockFlags2,
		dwFlags
	);

	if( !bResult )
	{
		WriteToLog(
			TEXT("VB::Lock(%d,%d,,0x%x) failed.\n"),
			0,
			m_shape2.m_nVertices * sizeof(D3DVERTEX),
			LockFlags2
		);

		goto Cleanup;
	}

	if ( pbObject == NULL )
	{
		WriteToLog(
			TEXT("VB::Lock succeeded, but pbObject == NULL.\n")
		);

		goto Cleanup;
	}


	//
	// Fill the vertex buffer with the second object.
	//

	ASSERT( pbObject != NULL );
	ASSERT( m_shape2.m_pVertices != NULL );
	CopyMemory
	(
		pbObject,
		m_shape2.m_pVertices,
		m_shape2.m_nVertices * sizeof(D3DVERTEX)
	);

	bResult = pVB->Unlock(dwFlags);
	if( !bResult )
	{
		WriteToLog
		(
			TEXT("VB::Unlock() failed.\n")
		);

		goto Cleanup;
	}


	//
	// Lock the index buffer the second time.
	//

	bResult = pIB->Lock
	(
		0,
		m_shape2.m_nIndices * sizeof(WORD),
		&pbObject,
		LockFlags2,
		dwFlags
	);

	if( !bResult )
	{
		WriteToLog
		(
			TEXT("IB::Lock(%d,%d,,0x%x) failed.\n"),
			0,
			m_shape2.m_nIndices * sizeof(WORD),
			LockFlags2
		);

		goto Cleanup;
	}

	if ( pbObject == NULL )
	{
		WriteToLog
		(
			TEXT("IB::Lock succeeded, but pbObject == NULL.\n")
		);

		goto Cleanup;
	}


	//
	// Fill the index buffer with the second object.
	//

	ASSERT( pbObject != NULL );
	ASSERT( m_shape2.m_pIndices != NULL );
	CopyMemory
	(
		pbObject,
		m_shape2.m_pIndices,
		m_shape2.m_nIndices * sizeof(WORD)
	);


	//
	// Unlock the index buffer.
	//

	bResult = pIB->Unlock(dwFlags);
	if( !bResult )
	{
		WriteToLog
		(
			TEXT("IB::Unlock() failed.\n")
		);

		goto Cleanup;
	}


	//
	// Draw the second object.
	//

	bResult = DrawIndexedPrimitive
	(
		D3DPT_TRIANGLELIST,
		0,
		m_shape2.m_nVertices,
		0,
		m_shape2.m_nIndices / 3,
		0,
		dwFlags
	);

	if( !bResult )
	{
		WriteToLog
		(
			TEXT("DrawIndexedPrimitive() failed.\n")
		);

		goto Cleanup;
	}

	bResult = EndScene(dwFlags);
	if( !bResult )
	{
		WriteToLog
		(
			TEXT("EndScene() returned failed.\n")
		);

		goto Cleanup;
	};


	//
	// If we made it this far, we're good to go.
	//

	bRet = TRUE;


Cleanup:
	RELEASE(pVB);
	RELEASE(pIB);

	return bRet;
}
