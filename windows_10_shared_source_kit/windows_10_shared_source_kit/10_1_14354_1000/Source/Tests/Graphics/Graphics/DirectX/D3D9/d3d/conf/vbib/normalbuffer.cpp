//////////////////////////////////////////////////////////////////////
// File:  NormalBuffer.cpp
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
// Statics and globals
//
/////////////////////////////////////////////////////////////////
#if FAST_TEST

const DWORD NormalBuffer::Usages[] =
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
    USAGE_SOFTWAREPROCESSING,
    USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY,
    USAGE_SOFTWAREPROCESSING | USAGE_DONOTCLIP,
    USAGE_SOFTWAREPROCESSING | USAGE_POINTS,
    USAGE_SOFTWAREPROCESSING | USAGE_RTPATCHES,
    USAGE_SOFTWAREPROCESSING | USAGE_DYNAMIC,
    USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_SOFTWAREPROCESSING | USAGE_POINTS | USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_SOFTWAREPROCESSING | USAGE_RTPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY | USAGE_DYNAMIC,
    USAGE_SOFTWAREPROCESSING | USAGE_DONOTCLIP | USAGE_DYNAMIC,
    USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY | USAGE_DONOTCLIP | USAGE_DYNAMIC,
};

const DWORD NormalBuffer::Pools[] =
{
    POOL_DEFAULT,
    POOL_MANAGED,
    POOL_SYSTEMMEM,
};


#else //FAST_TEST

const DWORD NormalBuffer::Usages[] =
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
	USAGE_SOFTWAREPROCESSING,
	USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY,
	USAGE_SOFTWAREPROCESSING | USAGE_DONOTCLIP,
	USAGE_SOFTWAREPROCESSING | USAGE_POINTS,
	USAGE_SOFTWAREPROCESSING | USAGE_RTPATCHES,
	USAGE_SOFTWAREPROCESSING | USAGE_NPATCHES,
	USAGE_SOFTWAREPROCESSING | USAGE_DYNAMIC,
	USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_SOFTWAREPROCESSING | USAGE_POINTS | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_SOFTWAREPROCESSING | USAGE_RTPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_SOFTWAREPROCESSING | USAGE_NPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_SOFTWAREPROCESSING | USAGE_POINTS | USAGE_RTPATCHES | USAGE_NPATCHES,
	USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY | USAGE_DYNAMIC,
	USAGE_SOFTWAREPROCESSING | USAGE_DONOTCLIP | USAGE_DYNAMIC,
	USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY | USAGE_DONOTCLIP | USAGE_DYNAMIC,
};


const DWORD NormalBuffer::Pools[] =
{
	POOL_DEFAULT,
	POOL_MANAGED,
	POOL_SYSTEMMEM,
};
#endif //FAST_TEST

static NormalBufferTestCase s_NormalBufferTestCases
[
	COUNTOF( NormalBuffer::Usages ) *
	COUNTOF( NormalBuffer::Pools )
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
// Function:  NormalBuffer
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
NormalBuffer::NormalBuffer()
{
	m_szTestName = _T("VBIB Normal Buffer");
	m_szCommandKey = _T("NormalBuffer");
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
UINT NormalBuffer::TestInitialize()
{
	BOOL dwSW = FALSE;
	if( m_pSrcDevice->GetBehaviors() & DEVICECREATE_MIXED )
	{
		//GetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, &dwSW, CD3D_SRC );
		dwSW = GetSoftwareVertexProcessing(CD3D_SRC );
	}


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
				// Usage dynamic is not valid on managed buffers
				(
					( Usages[ iCurrentUsage ] & USAGE_DYNAMIC ) &&
					( Pools[ iCurrentPool ] == POOL_MANAGED )
				) ||

				// Software usage is not valid on hardware device
				(
					( Usages[ iCurrentUsage ] & USAGE_SOFTWAREPROCESSING ) &&
					( m_pSrcDevice->GetBehaviors() & DEVICECREATE_HWVP )
				) ||

				// Software usage is required on mixed device with SW
				// vertex processing when pool is not sysmem
				(
					!( Usages[ iCurrentUsage ] & USAGE_SOFTWAREPROCESSING ) &&
					( m_pSrcDevice->GetBehaviors() & DEVICECREATE_MIXED ) &&
					dwSW &&
					Pools[ iCurrentPool ] != POOL_SYSTEMMEM
				)
			)
			{
				continue;
			};


			ASSERT( cTestCases < COUNTOF( s_NormalBufferTestCases ) );

			s_NormalBufferTestCases[ cTestCases ].nTest = cTestCases;

			s_NormalBufferTestCases[ cTestCases ].Usage = Usages[ iCurrentUsage ];
			s_NormalBufferTestCases[ cTestCases ].Pool = Pools[ iCurrentPool ];

			++cTestCases;
		};
	};


	//
	// Set the test range.
	//
#if FAST_TEST
    m_totalNumTestCases = cTestCases;
    SetTestRange(1,1);
#else //FAST_TEST
	SetTestRange
	(
		1,
		cTestCases
	);
#endif //FAST_TEST


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
		uiRet = TestInitReal( CD3D_SRC );
		if( uiRet != D3DTESTINIT_RUN )
		{
			continue;
		}

		uiRet = TestInitReal( CD3D_REF );
		if( uiRet != D3DTESTINIT_RUN )
		{
			continue;
		}
	} while(0);

	return uiRet;
}


///////////////////////////////////////////////////////
// Function:  ExecuteTest
//
// Purpose:
// Runs a test case.
///////////////////////////////////////////////////////
#if FAST_TEST
bool NormalBuffer::ExecuteTest(UINT nTest)
{
    //
    // Validate the test case ordinal.
    //

    if ( nTest > COUNTOF( s_NormalBufferTestCases ) )
    {
        WriteToLog
            (
            _T( "Invalid test ordinal %u." ),
            nTest
            );

        return false;
    };

    //
    // Begin the test case.
    //

    BeginTestCase("VBIB normal - Fast", nTest);

    //
    // Clear the frame in preparation for rendering.
    //

    ClearFrame();

    auto& drawLamda = [=](UINT testIndex) -> bool
    {
        bool bRet = true;
        //Original implementation
        {
            //
            // Hand off to actually run the test.
            //

            D3DTEST_NORMALBUFFER curtest;
            curtest.szName = TEXT("");
            curtest.shape = &m_shape;
#ifdef SWPUREDEVICEHACK
            curtest.VBFVF = D3DFVF_TLVERTEX;
#else
            curtest.VBFVF = D3DFVF_VERTEX;
#endif
            curtest.IBFormat = FMT_INDEX16;
            curtest.Usage = s_NormalBufferTestCases[testIndex - 1].Usage;
            curtest.Pool = s_NormalBufferTestCases[testIndex - 1].Pool;


            BOOL bRetTest = ExecuteReal
                (
                &curtest,
                CD3D_SRC
                );

            BOOL bRetRef = ExecuteReal
                (
                &curtest,
                CD3D_REF
                );

            bRet = bRetTest && bRetRef;
            if (!bRet)
            {
                Fail();
            }
        }

        return bRet;
    };

    return  DrawMultiTests(drawLamda);
};
#else //FAST_TEST

bool NormalBuffer::ExecuteTest
(
	UINT nTest
)
{
	//
	// Validate the test case ordinal.
	//

	if ( nTest > COUNTOF( s_NormalBufferTestCases ) )
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
		TEXT("Usage = 0x%x, Pool = %d"),
		s_NormalBufferTestCases[ nTest - 1 ].Usage,
		s_NormalBufferTestCases[ nTest - 1 ].Pool
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

	D3DTEST_NORMALBUFFER curtest;
	curtest.szName = TEXT("");
	curtest.shape = &m_shape;
#ifdef SWPUREDEVICEHACK
	curtest.VBFVF = D3DFVF_TLVERTEX;
#else
	curtest.VBFVF = D3DFVF_VERTEX;
#endif
	curtest.IBFormat = FMT_INDEX16;
	curtest.Usage = s_NormalBufferTestCases[ nTest - 1 ].Usage;
	curtest.Pool = s_NormalBufferTestCases[ nTest - 1 ].Pool;


	BOOL bRetTest = ExecuteReal
	(
		&curtest,
		CD3D_SRC
	);

	BOOL bRetRef = ExecuteReal
	(
		&curtest,
		CD3D_REF
	);


	bool bRet = bRetTest && bRetRef;
	if( !bRet )
	{
		Fail();
	}

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
//
///////////////////////////////////////////////////////
D3DTESTINIT NormalBuffer::TestInitReal
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
			D3DXMatrixRotationX( &d3dMatOut, 2.0f )
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
			{ 1.0f, 0.0f, 0.0f, 0.0f },  // diffuse
			{ 0.0f, 0.3f, 0.0f, 0.5f },  // ambient
			{ 1.0f, 0.0f, 0.0f, 0.0f },	 // specular
			{ 0.0f, 0.0f, 0.0f, 0.0f },	 // emissive
			0.0f  // sharpness (for specular)
		};

		bResult = SetMaterial(&mtrl, dwFlags);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetMaterial() failed.\n")
			);
			continue;
		}
	}
	while(0);


	return( bResult ? D3DTESTINIT_RUN : D3DTESTINIT_ABORT );
}


///////////////////////////////////////////////////////
// Function:  ExecuteReal
//
// Purpose:
// Handles the heavy lifting in running a test case.
///////////////////////////////////////////////////////
BOOL NormalBuffer::ExecuteReal
(
	const D3DTEST_NORMALBUFFER *lpBufInfo,
	DWORD dwFlags
)
{
	bool bResult = true;
	LPVERTEXBUFFERS pVB = NULL;
	LPINDEXBUFFERS pIB = NULL;
	LPBYTE pbObject = NULL;
	bool bRet = false;


	do
	{
		bResult = CreateVertexBuffer(
			lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX),
			lpBufInfo->Usage,
			lpBufInfo->VBFVF,
			lpBufInfo->Pool,
			&pVB,
			dwFlags
		);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed.\n"),
				lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX),
				lpBufInfo->Usage,
				lpBufInfo->VBFVF,
				lpBufInfo->Pool
			);
			continue;
		}

		bResult = pVB->Lock(
			0,
			lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX),
			&pbObject,
			0,
			dwFlags
		);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("Lock() failed.\n")
			);
			continue;
		}

		if ( pbObject == NULL )
		{
			WriteToLog(
				TEXT("Lock() succeeded, but pbObject == NULL.\n" )
			);
			continue;
		}

#ifdef SWPUREDEVICEHACK
		{
			D3DTLVERTEX *pdata = (D3DTLVERTEX*)pbObject;

			pdata[0].sx = 0.0f;
			pdata[0].sy = 0.0f;
			pdata[0].sz = 0.0f;
			pdata[0].rhw = 1.0f;
			pdata[0].color = D3DCOLOR_ARGB( 0x40, 0x40, 0x40, 0x40 );
			pdata[0].specular = 0.0f;
			pdata[0].tu = 0.0f;
			pdata[0].tv = 0.0f;

			pdata[1].sx = 500.0f;
			pdata[1].sy = 0.0f;
			pdata[1].sz = 0.0f;
			pdata[1].rhw = 1.0f;
			pdata[1].color = D3DCOLOR_ARGB( 0x40, 0x40, 0x40, 0x40 );
			pdata[1].specular = 0.0f;
			pdata[1].tu = 0.0f;
			pdata[1].tv = 0.0f;

			pdata[2].sx = 0.0f;
			pdata[2].sy = 500.0f;
			pdata[2].sz = 0.0f;
			pdata[2].rhw = 1.0f;
			pdata[2].color = D3DCOLOR_ARGB( 0x40, 0x40, 0x40, 0x40 );
			pdata[2].specular = 0.0f;
			pdata[2].tu = 0.0f;
			pdata[2].tv = 0.0f;
		}
#else
		ASSERT( pbObject != NULL );
		ASSERT( lpBufInfo->shape != NULL );
		ASSERT( lpBufInfo->shape->m_pVertices != NULL );
		CopyMemory( pbObject, lpBufInfo->shape->m_pVertices, lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX) );
#endif

		bResult = pVB->Unlock(dwFlags);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("Unlock() failed.\n")
			);
			continue;
		}

		bResult = SetStreamSource(
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


		bResult = CreateIndexBuffer(
			lpBufInfo->shape->m_nIndices * sizeof(WORD),
			lpBufInfo->Usage,
			lpBufInfo->IBFormat,
			lpBufInfo->Pool,
			&pIB,
			dwFlags
		);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,) failed.\n"),
				lpBufInfo->shape->m_nIndices * sizeof(WORD),
				lpBufInfo->Usage,
				lpBufInfo->IBFormat,
				lpBufInfo->Pool
			);
			continue;
		}

		bResult = pIB->Lock(
			0,
			lpBufInfo->shape->m_nIndices * sizeof(WORD),
			&pbObject,
			0,
			dwFlags
		);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("Lock() failed.\n")
			);
			continue;
		}

#ifdef SWPUREDEVICEHACK
		{
			PWORD pdata = (PWORD)pbObject;

			pdata[0] = 0;
			pdata[1] = 1;
			pdata[2] = 2;
		}
#else
		CopyMemory( pbObject, lpBufInfo->shape->m_pIndices, lpBufInfo->shape->m_nIndices * sizeof(WORD) );
#endif

		bResult = pIB->Unlock(dwFlags);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("Unlock() failed.\n")
			);
			continue;
		}

		bResult = SetIndices(
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


#ifdef SWPUREDEVICEHACK
		bResult = SetVertexShader( D3DFVF_TLVERTEX, dwFlags );
#else
		bResult = SetVertexShader( D3DFVF_VERTEX, dwFlags );
#endif
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetVertexShader() failed.\n")
			);
			continue;
		}

		Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0, dwFlags );

		do
		{
			bRet = true;

			bResult = BeginScene( dwFlags );
			if( false == bResult )
			{
				WriteToLog(
					TEXT("BeginScene() failed.\n")
				);
				bRet = false;
				continue;
			}

			bResult = DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
#ifdef SWPUREDEVICEHACK
				0,
				3,
				0,
				1,
#else
				0,
				lpBufInfo->shape->m_nVertices,
				0,
				lpBufInfo->shape->m_nIndices / 3,
#endif
				0,
				dwFlags
			);
			if( false == bResult )
			{
				WriteToLog(
					TEXT("DrawIndexedPrimitive() failed.\n")
				);
				bRet = false;
				//continue;  // don't continue, so EndScene() is always called
			}

			bResult = EndScene( dwFlags );
			if( false == bResult )
			{
				WriteToLog(
					TEXT("EndScene() failed.\n")
				);
				bRet = false;
				continue;
			}
		} while(0);
	} while(0);


	RELEASE(pIB);
	RELEASE(pVB);

	return bRet;
}

