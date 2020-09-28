//////////////////////////////////////////////////////////////////////
// File:  Priority.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of Direct3D vertex buffer and index buffer 
// priority rendering tests.
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
const DWORD Priority::Usages[] =
{
    0,
    USAGE_WRITEONLY,
    USAGE_DONOTCLIP,
    USAGE_POINTS,
    USAGE_RTPATCHES,
    USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_POINTS | USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_RTPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
};


const DWORD Priority::Priorities[] =
{
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    0xff,
    0x100,
    0xffff,
    0x10000,
    0xffffffff
};


const FMT Priority::IndexFormats[] =
{
    FMT_UNKNOWN,
    FMT_INDEX16,
    FMT_INDEX32
};
#else //FAST_TEST

const DWORD Priority::Usages[] =
{
	0,
	USAGE_WRITEONLY,
	USAGE_DONOTCLIP,
	USAGE_POINTS,
	USAGE_RTPATCHES,
	USAGE_NPATCHES,
	USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_POINTS | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_RTPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_NPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_POINTS | USAGE_RTPATCHES | USAGE_NPATCHES,
};


const DWORD Priority::Priorities[] =
{
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	0xff,
	0x100,
	0xffff,
	0x10000,
	0xffffffff
};


const FMT Priority::IndexFormats[] =
{
	FMT_UNKNOWN,
	FMT_INDEX16,
	FMT_INDEX32
};

#endif //FAST_TEST


static PriorityTestCase s_PriorityTestCases
[
	COUNTOF( Priority::Usages ) *
	COUNTOF( Priority::Priorities ) *
	COUNTOF( Priority::IndexFormats )
];


/////////////////////////////////////////////////////////////////
//
// Construction / destruction
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Function:  Priority
//
// Purpose:
// Constructor.
////////////////////////////////////////////////////////////
Priority::Priority()
{
	m_szTestName = _T("VBIB Priority");
	m_szCommandKey = _T("Priority");

	m_Options.D3DOptions.dwBehaviors = DEVICECREATE_FPU_PRESERVE | DEVICECREATE_SWVP | DEVICECREATE_STANDARD | DEVICECREATE_MIXED;
	m_Options.D3DOptions.fMinDXVersion = 8.f;
}


/////////////////////////////////////////////////////////////////
//
// Framework override methods
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Function:  TestInitialize
//
// Purpose:
// Test group initialization.
////////////////////////////////////////////////////////////
UINT Priority::TestInitialize()
{
	//
	// Count the number of valid scenarios, and build our
	// scenario data list.
	//

	UINT cTestCases = 0;
	for ( UINT iCurrentUsage = 0; iCurrentUsage < COUNTOF( Usages ); ++iCurrentUsage )
	{
		for( UINT iCurrentPriority = 0; iCurrentPriority < COUNTOF( Priorities ); ++iCurrentPriority )
		{
			for( UINT iCurrentIndexFormat = 0; iCurrentIndexFormat < COUNTOF( IndexFormats ); ++iCurrentIndexFormat )
			{
				ASSERT( cTestCases < COUNTOF( s_PriorityTestCases ) );

				s_PriorityTestCases[ cTestCases ].nTest = cTestCases;

				s_PriorityTestCases[ cTestCases ].Usage = Usages[ iCurrentUsage ];
				s_PriorityTestCases[ cTestCases ].Priority = Priorities[ iCurrentPriority ];
				s_PriorityTestCases[ cTestCases ].IndexFormat = IndexFormats[ iCurrentIndexFormat ];

				++cTestCases;
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
	// Skip devices that don't allow SW VP.
	//

	if(
		(m_pSrcDevice->GetBehaviors() & DEVICECREATE_PURE) ||
		(m_pRefDevice->GetBehaviors() & DEVICECREATE_PURE) ||
		(!(m_pSrcDevice->GetBehaviors() & DEVICECREATE_SWVP)) ||
		(!(m_pRefDevice->GetBehaviors() & DEVICECREATE_SWVP))
	)
	{
		return D3DTESTINIT_SKIPALL;
	};


	//
	// Initialize our shape.
	//

	m_shape.NewShape( CS_SPHERE, 10 );


	D3DTESTINIT uiRet = TestInitReal();

	return uiRet;
}


////////////////////////////////////////////////////////////
// Function:  ExecuteTest
//
// Purpose:
// Runs a test case.
////////////////////////////////////////////////////////////
#if FAST_TEST
bool Priority::ExecuteTest
(
UINT nTest
)
{
    //
    // Begin the test case.
    //

    BeginTestCase("Priority - fast", nTest);

    //
    // Clear the frame in preparation for rendering.
    //

    ClearFrame();


    auto& drawLamda = [=](UINT testIndex) -> bool
    {
        //
        // Validate the test case ordinal.
        //

        if (testIndex > COUNTOF(s_PriorityTestCases))
        {
            WriteToLog
                (
                TEXT("Invalid test case ordinal %u."),
                testIndex
                );

            return false;
        };


        //
        // Skip invalid tests.
        //

        // skip test if number of vertices > maxindex
        if
            (
            s_PriorityTestCases[testIndex - 1].IndexFormat == FMT_INDEX32 &&
            (
            (0x00010000 > m_pSrcDevice->GetCaps()->dwMaxVertexIndex) ||
            (0x00010000 > m_pRefDevice->GetCaps()->dwMaxVertexIndex)
            )
            )
        {
            WriteToLog
                (
                TEXT("Device does not support 32-bit indices.")
                );

            Skip();

            return false;
        };


        //
        // Look up test case data.
        //

        D3DTEST_NORMALBUFFER curtest;
        curtest.szName = TEXT("");
        curtest.shape = &m_shape;
        curtest.VBFVF = D3DFVF_VERTEX;
        curtest.IBFormat = s_PriorityTestCases[testIndex - 1].IndexFormat;
        curtest.Usage = s_PriorityTestCases[testIndex - 1].Usage;
        curtest.Pool = POOL_MANAGED;


        //
        // Hand off to actually run the test case.
        //

        BOOL bRetTest = ExecuteReal
            (
            &curtest,
            s_PriorityTestCases[testIndex - 1].Priority,
            (m_pSrcDevice->GetBehaviors() & DEVICECREATE_MIXED),
            CD3D_SRC
            );

        BOOL bRetRef = ExecuteReal
            (
            &curtest,
            s_PriorityTestCases[testIndex - 1].Priority,
            (m_pRefDevice->GetBehaviors() & DEVICECREATE_MIXED),
            CD3D_REF
            );


        bool bRet = bRetTest && bRetRef;
        if (!bRet)
        {
            Fail();
        }

        return bRet;
    };

    return DrawMultiTests(drawLamda);
}
#else
bool Priority::ExecuteTest
(
	UINT nTest
)
{
	//
	// Validate the test case ordinal.
	//

	if ( nTest > COUNTOF( s_PriorityTestCases ) )
	{
		WriteToLog
		(
			TEXT( "Invalid test case ordinal %u." ),
			nTest
		);

		return false;
	};


	//
	// Build the test case name.
	//

	TCHAR sz[ 256 ] = _T( "" );
	switch ( s_PriorityTestCases[ nTest - 1 ].IndexFormat )
	{
		case FMT_UNKNOWN:
		{
			wsprintf
			(
				sz,
				TEXT("Usage = 0x%x, Priority = 0x%x, no indices"),
				s_PriorityTestCases[ nTest - 1 ].Usage,
				s_PriorityTestCases[ nTest - 1 ].Priority
			);
		} break;

		case FMT_INDEX16:
		{
			wsprintf
			(
				sz,
				TEXT("Usage = 0x%x, Priority = 0x%x, 16-bit indices"),
				s_PriorityTestCases[ nTest - 1 ].Usage,
				s_PriorityTestCases[ nTest - 1 ].Priority
			);
		} break;

		case FMT_INDEX32:
		{
			wsprintf
			(
				sz,
				TEXT("Usage = 0x%x, Priority = 0x%x, 32-bit indices"),
				s_PriorityTestCases[ nTest - 1 ].Usage,
				s_PriorityTestCases[ nTest - 1 ].Priority
			);
		} break;

		default:
		{
			WriteToLog
			(
				TEXT("Unexpected index format value %d."),
				s_PriorityTestCases[ nTest - 1 ].IndexFormat
			);

			return false;
		} break;
	};


	//
	// Begin the test case.
	//

	BeginTestCase( sz, nTest );


	//
	// Skip invalid tests.
	//

	// skip test if number of vertices > maxindex
	if
	(
		s_PriorityTestCases[ nTest - 1 ].IndexFormat == FMT_INDEX32 &&
		(
			(0x00010000 > m_pSrcDevice->GetCaps()->dwMaxVertexIndex) ||
			(0x00010000 > m_pRefDevice->GetCaps()->dwMaxVertexIndex)
		)
	)
	{
		WriteToLog
		(
			TEXT("Device does not support 32-bit indices.")
		);

		Skip();

		return false;
	};


	//
	// Clear the frame in preparation for rendering.
	//

	ClearFrame();


	//
	// Look up test case data.
	//

	D3DTEST_NORMALBUFFER curtest;
	curtest.szName = TEXT("");
	curtest.shape = &m_shape;
	curtest.VBFVF = D3DFVF_VERTEX;
	curtest.IBFormat = s_PriorityTestCases[ nTest - 1 ].IndexFormat;
	curtest.Usage = s_PriorityTestCases[ nTest - 1 ].Usage;
	curtest.Pool = POOL_MANAGED;


	//
	// Hand off to actually run the test case.
	//

	BOOL bRetTest = ExecuteReal
	(
		&curtest,
		s_PriorityTestCases[ nTest - 1 ].Priority,
		(m_pSrcDevice->GetBehaviors() & DEVICECREATE_MIXED),
		CD3D_SRC
	);

	BOOL bRetRef = ExecuteReal
	(
		&curtest,
		s_PriorityTestCases[ nTest - 1 ].Priority,
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
#endif


////////////////////////////////////////////////////////////
// Function:  TestInitReal
//
// Purpose:
//
////////////////////////////////////////////////////////////
D3DTESTINIT Priority::TestInitReal
(
	DWORD dwFlags
)
{
	bool bResult = false;


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

	return( bResult == false ? D3DTESTINIT_ABORT : D3DTESTINIT_RUN );
};


////////////////////////////////////////////////////////////
// Function:  ExecuteReal
//
// Purpose:
// Handles the heavy lifting in running a test case.
////////////////////////////////////////////////////////////
BOOL Priority::ExecuteReal
(
	const D3DTEST_NORMALBUFFER *lpBufInfo,
	DWORD dwThisPriority,
	BOOL fMixedMode,
	DWORD dwFlags
)
{
	bool bResult = true;
	LPVERTEXBUFFERS pVB = NULL;
	LPVERTEXBUFFERS pVBTL = NULL;
	LPINDEXBUFFERS pIB = NULL;
	LPBYTE pbObject = NULL;
	BOOL bRet = FALSE;
	DWORD * pTempIndices = NULL;

	BOOL fSoftwareUsage = TRUE;

	// determine sw usage
	if( fMixedMode )
		fSoftwareUsage = GetSoftwareVertexProcessing(dwFlags );

	do
	{
		// create D3DVERTEX buffer
		bResult = CreateVertexBuffer(
			lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX),
			lpBufInfo->Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : 0),
			//lpBufInfo->Usage ^ (fMixedMode ? USAGE_SOFTWAREPROCESSING : 0),
			D3DFVF_VERTEX,
			lpBufInfo->Pool,
			&pVB,
			dwFlags
		);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed.\n"),
				lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX),
				lpBufInfo->Usage ^ (fSoftwareUsage ? D3DUSAGE_SOFTWAREPROCESSING : 0),
				D3DFVF_VERTEX,
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

		CopyMemory( pbObject, lpBufInfo->shape->m_pVertices, lpBufInfo->shape->m_nVertices * sizeof(D3DVERTEX) );

		bResult = pVB->Unlock(dwFlags);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("Unlock() returned %s (0x%x)\n")
			);
			continue;
		}

		if (lpBufInfo->IBFormat != FMT_UNKNOWN)
		{
			// create index buffer
			bResult = CreateIndexBuffer(
				lpBufInfo->shape->m_nIndices * (lpBufInfo->IBFormat==D3DFMT_INDEX16 ? sizeof(WORD) : sizeof(DWORD)),
				lpBufInfo->Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : 0),
				//lpBufInfo->Usage ^ (fMixedMode ? USAGE_SOFTWAREPROCESSING : 0),
				lpBufInfo->IBFormat,
				lpBufInfo->Pool,
				&pIB,
				dwFlags
			);
			if( false == bResult )
			{
				WriteToLog(
					TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,) failed.\n"),
					lpBufInfo->shape->m_nIndices * (lpBufInfo->IBFormat==D3DFMT_INDEX16 ? sizeof(WORD) : sizeof(DWORD)),
					lpBufInfo->Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : 0),
					lpBufInfo->IBFormat,
					lpBufInfo->Pool
				);
				continue;
			}

			bResult = pIB->Lock(
				0,
				lpBufInfo->shape->m_nIndices * (lpBufInfo->IBFormat==D3DFMT_INDEX16 ? sizeof(WORD) : sizeof(DWORD)),
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

			if (lpBufInfo->IBFormat==D3DFMT_INDEX16)
			{
				CopyMemory( pbObject, lpBufInfo->shape->m_pIndices, lpBufInfo->shape->m_nIndices * sizeof(WORD) );
			}
			else
			{
				pTempIndices = new DWORD[lpBufInfo->shape->m_nIndices];
				if ( pTempIndices == NULL )
				{
					WriteToLog(
						TEXT("Failed to allocate pTempIndices.\n")
					);
					continue;
				}

				for (int i = 0; i < lpBufInfo->shape->m_nIndices; i++)
				{
					pTempIndices[i] = (DWORD)lpBufInfo->shape->m_pIndices[i];
				}

				CopyMemory( pbObject, pTempIndices, lpBufInfo->shape->m_nIndices * sizeof(DWORD) );
			}
			bResult = pIB->Unlock(dwFlags);
			if( false == bResult )
			{
				WriteToLog(
					TEXT("Unlock() returned %s (0x%x)\n")
				);
				continue;
			}
		}

		// create D3DTLVERTEX buffer and process vertices
		bResult = CreateVertexBuffer(
			lpBufInfo->shape->m_nVertices * sizeof(D3DTLVERTEX),
			lpBufInfo->Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : 0),
			D3DFVF_TLVERTEX,
			lpBufInfo->Pool,
			&pVBTL,
			dwFlags
		);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed.\n"),
				lpBufInfo->shape->m_nVertices * sizeof(D3DTLVERTEX),
				lpBufInfo->Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : 0),
				D3DFVF_TLVERTEX,
				lpBufInfo->Pool
			);
			continue;
		}

		bResult = SetSoftwareVertexProcessing( TRUE, dwFlags );
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetSoftwareVertexProcessing( TRUE ) failed.\n")
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
				TEXT("SetStreamSource(pVB) failed.\n")
			);
			continue;
		}

		if (lpBufInfo->IBFormat != FMT_UNKNOWN)
		{
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
		}

		bResult = SetVertexShader( D3DFVF_VERTEX, dwFlags );
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetVertexShader(D3DFVF_VERTEX) failed.\n")
			);
			continue;
		}

		bResult = ProcessVertices(
			0,
			0,
			lpBufInfo->shape->m_nVertices,
			pVBTL,
			NULL,
			0,
			dwFlags
		);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("ProcessVertices() failed.\n")
			);
			continue;
		}

		bResult = SetSoftwareVertexProcessing( fSoftwareUsage, dwFlags );
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetSoftwareVertexProcessing( fSoftwareUsage==%d ) failed.\n"),
				fSoftwareUsage);
			continue;
		}


		// setup streams and shader for rendering
		bResult = SetStreamSource(
			0,
			pVBTL,
			sizeof(D3DTLVERTEX),
			0,
			dwFlags
		);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetStreamSource(pVBTL) failed.\n")
			);
			continue;
		}

		if (lpBufInfo->IBFormat != FMT_UNKNOWN)
		{
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
		}

		bResult = SetVertexShader( D3DFVF_TLVERTEX, dwFlags );
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetVertexShader(D3DFVF_TLVERTEX) failed.\n")
			);
			continue;
		}

		Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0, dwFlags );

		bResult = BeginScene(dwFlags);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("BeginScene() failed.\n")
			);
			continue;
		}

		bRet = TRUE;

		do
		{
			DWORD dwOldPriority;
			DWORD dwTemp;

			dwOldPriority = pVBTL->GetPriority( dwFlags );
			if( dwOldPriority != 0 )
			{
				WriteToLog(
					TEXT("GetPriority() returned %d, expected %d\n"),
					dwOldPriority,
					0
				);
				bRet = FALSE;
			}

			dwTemp = pVBTL->SetPriority( dwThisPriority, dwFlags );
			if( dwOldPriority != dwTemp )
			{
				WriteToLog(
					TEXT("SetPriority() returned %d, expected %d\n"),
					dwTemp,
					dwOldPriority
				);
				bRet = FALSE;
			}

			dwTemp = pVBTL->GetPriority( dwFlags );
			if( dwTemp != dwThisPriority )
			{
				WriteToLog(
					TEXT("GetPriority() returned %d, expected %d\n"),
					dwTemp,
					dwThisPriority
				);
				bRet = FALSE;
			}

			/*bResult = DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
				0,
				lpBufInfo->shape->m_nVertices,
				0,
				lpBufInfo->shape->m_nIndices / 3,
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
			}*/
			/*RPOPTIONS pOptions;
			pOptions.Pool = lpBufInfo->Pool;
			pOptions.Usage = lpBufInfo->Usage;
			pOptions.uIBBitCount = ( lpBufInfo->IBFormat == FMT_UNKNOWN ? 0 : (lpBufInfo->IBFormat == FMT_INDEX16 ? 16 : 32) );
			pOptions.Flags = 0;

			if (lpBufInfo->IBFormat == FMT_UNKNOWN)
			{
				LPVOID lpVertices;
				lpVertices = NULL;
				lpVertices = CreateVertexArray(lpVertices, lpBufInfo->shape->m_pVertices, lpBufInfo->shape->m_nVertices, sizeof(D3DVERTEX),
								  lpBufInfo->shape->m_pIndices, lpBufInfo->shape->m_nIndices);
				RenderPrimitive(D3DPT_TRIANGLELIST, lpBufInfo->VBFVF,
							  lpVertices, lpBufInfo->shape->m_nIndices, NULL, 
							  lpBufInfo->shape->m_nIndices, &pOptions);
				delete [] lpVertices;
			}
			else
			{
				RenderPrimitive(D3DPT_TRIANGLELIST, lpBufInfo->VBFVF,
					lpBufInfo->shape->m_pVertices, lpBufInfo->shape->m_nVertices, lpBufInfo->IBFormat == FMT_INDEX16 ? (LPVOID)lpBufInfo->shape->m_pIndices : (LPVOID)pTempIndices, 
							  lpBufInfo->shape->m_nIndices, &pOptions);
				if (lpBufInfo->IBFormat == FMT_INDEX32)
					delete [] pTempIndices;
			}*/

			if (lpBufInfo->IBFormat == FMT_UNKNOWN)
			{
				CnVertexBuffer *pNewBuffer;
				pNewBuffer = NULL;
				if (!ConvertToVertexBuffer(&pNewBuffer, *lpBufInfo, lpBufInfo->shape->m_pVertices, lpBufInfo->shape->m_nVertices, sizeof(D3DVERTEX),
								  lpBufInfo->shape->m_pIndices, lpBufInfo->shape->m_nIndices, fMixedMode, dwFlags))
				{
					WriteToLog("ConvertToVertexBuffer() failed. \n");
					bRet = false;
				}
				if
				(
					!DrawPrimitiveVertexBuffer
					(
						D3DPT_TRIANGLELIST,
						pNewBuffer,
						0, 
						lpBufInfo->shape->m_nIndices,
						NULL,
						lpBufInfo->shape->m_nIndices,
						0,
						lpBufInfo->Usage,
						lpBufInfo->Pool,
						false,
						dwFlags
					)
				)
				{
					WriteToLog("DrawPrimitiveVertexBuffer() failed.\n");
					bRet = false;
				}
				RELEASE(pNewBuffer);
			}
			else
			{
				if
				(
					!DrawPrimitiveVertexBuffer
					(
						D3DPT_TRIANGLELIST,
						pVB,
						0,
						lpBufInfo->shape->m_nVertices, 
						lpBufInfo->IBFormat == FMT_INDEX32 ? (LPWORD)pTempIndices : lpBufInfo->shape->m_pIndices,
						lpBufInfo->shape->m_nIndices,
						0, 
						lpBufInfo->Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : 0),
						lpBufInfo->Pool,
						lpBufInfo->IBFormat == FMT_INDEX32 ? true : false,
						dwFlags
					)
				)
				{
					WriteToLog("DrawPrimitiveVertexBuffer() failed.\n");
					bRet = false;
				}

				if ( lpBufInfo->IBFormat == FMT_INDEX32 )
				{
					delete [] pTempIndices;
					pTempIndices = NULL;
				}
			}


			dwTemp = pVBTL->GetPriority(dwFlags);
			if( dwTemp != dwThisPriority )
			{
				WriteToLog(
					TEXT("GetPriority() returned %d, expected %d\n"),
					dwTemp,
					dwThisPriority
				);
				bRet = FALSE;
			}

			dwTemp = pVBTL->SetPriority( dwOldPriority, dwFlags );
			if( dwTemp != dwThisPriority )
			{
				WriteToLog(
					TEXT("SetPriority() returned %d, expected %d\n"),
					dwTemp,
					dwThisPriority
				);
				bRet = FALSE;
			}

			dwTemp = pVBTL->GetPriority(dwFlags);
			if( dwTemp != dwOldPriority )
			{
				WriteToLog(
					TEXT("GetPriority() returned %d, expected %d\n"),
					dwTemp,
					dwOldPriority
				);
				bRet = FALSE;
			}

			dwTemp = pVBTL->SetPriority( dwThisPriority, dwFlags );
			if( dwTemp != dwOldPriority )
			{
				WriteToLog(
					TEXT("SetPriority() returned %d, expected %d\n"),
					dwTemp,
					dwOldPriority
				);
				bRet = FALSE;
			}

			dwTemp = pVBTL->GetPriority(dwFlags);
			if( dwTemp != dwThisPriority )
			{
				WriteToLog(
					TEXT("GetPriority() returned %d, expected %d\n"),
					dwTemp,
					dwThisPriority
				);
				bRet = FALSE;
			}

			dwTemp = pVBTL->GetPriority(dwFlags);
			if( dwTemp != dwThisPriority )
			{
				WriteToLog(
					TEXT("GetPriority() returned %d, expected %d\n"),
					dwTemp,
					dwThisPriority
				);
				bRet = FALSE;
			}

			dwTemp = pVBTL->SetPriority( dwOldPriority, dwFlags );
			if( dwTemp != dwThisPriority )
			{
				WriteToLog(
					TEXT("SetPriority() returned %d, expected %d\n"),
					dwTemp,
					dwThisPriority
				);
				bRet = FALSE;
			}
		}
		while(0);  // execute once, so continue can bust out

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

	GetLastError();

	RELEASE(pIB);
	RELEASE(pVB);
	RELEASE(pVBTL);


	return(bRet);
}


////////////////////////////////////////////////////////////
// Function:  CreateVertexArray
//
// Purpose:
//
////////////////////////////////////////////////////////////
LPVOID Priority::CreateVertexArray
(
	LPVOID lpNewVertexArray,
	LPVOID lpVertices,
	DWORD dwVertexCount,
	DWORD dwVertexSize,
	LPVOID lpIndices,
	DWORD dwIndexCount
)
{
	DWORD   dwRequiredMemory = dwIndexCount * dwVertexSize;
	LPBYTE  p;

	if (NULL != lpNewVertexArray)
	{
		delete [] lpNewVertexArray;
	}

	lpNewVertexArray = new BYTE [dwRequiredMemory];

	if (NULL == lpNewVertexArray)
	{
		return NULL;
	}

	// Dereference vertices to the allocated memory
	p = (LPBYTE)lpNewVertexArray;
	LPWORD		lpwIndices = (LPWORD)lpIndices;
	for (DWORD i = 0; i < dwIndexCount; i++)
	{
		if (dwVertexCount < lpwIndices[i])
			return NULL;
		memcpy(p, (((LPBYTE)lpVertices) + dwVertexSize*lpwIndices[i]), 
			   dwVertexSize);
		p += dwVertexSize;
	}
	return lpNewVertexArray;
}


////////////////////////////////////////////////////////////
// Function:  ConvertToVertexBuffer
//
// Purpose:
//
////////////////////////////////////////////////////////////
bool Priority::ConvertToVertexBuffer
(
	CnVertexBuffer **pNewBuffer,
	D3DTEST_NORMALBUFFER lpBufInfo,
	LPVOID lpVertices,
	DWORD dwVertexCount,
	DWORD dwVertexSize,
	LPVOID lpIndices,
	DWORD dwIndexCount,
	BOOL fMixedMode,
	DWORD dwFlags
)
{
	DWORD dwRequiredMemory = dwIndexCount * dwVertexSize;
	LPBYTE p = NULL;
	BOOL bResult;
	LPVOID lpNewVertexArray = NULL;

	BOOL fSoftwareUsage = TRUE;
	if( fMixedMode )
		fSoftwareUsage = GetSoftwareVertexProcessing(dwFlags );

	lpNewVertexArray = new BYTE [dwRequiredMemory];
	if (NULL == lpNewVertexArray) 
		return false;

	// Dereference vertices to the allocated memory
	p = (LPBYTE)lpNewVertexArray;
	LPWORD		lpwIndices = (LPWORD)lpIndices;
	for (DWORD i = 0; i < dwIndexCount; i++)
	{
		if (dwVertexCount < lpwIndices[i])
		{
			delete [] lpNewVertexArray;
			return false;
		}
		memcpy(p, (((LPBYTE)lpVertices) + dwVertexSize*lpwIndices[i]), 
			   dwVertexSize);
		p += dwVertexSize;
	}

	bResult = CreateVertexBuffer( dwIndexCount * sizeof(D3DVERTEX),
				 lpBufInfo.Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : 0),
				 lpBufInfo.VBFVF, lpBufInfo.Pool,
				 pNewBuffer, dwFlags );
	if( false == bResult )
	{
		WriteToLog(
		   TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed.\n"),
		   dwIndexCount * sizeof(D3DVERTEX),
		   lpBufInfo.Usage, lpBufInfo.VBFVF, lpBufInfo.Pool );
		delete [] lpNewVertexArray;
		return false;
	 }

	 bResult = (*pNewBuffer)->Lock( 0, dwIndexCount * sizeof(D3DVERTEX), &p, 0, dwFlags );
	 if( false == bResult )
	 {
		 WriteToLog( TEXT("Lock() failed.\n") );
		 delete [] lpNewVertexArray;
		 return false;
	 }

	 CopyMemory( p, lpNewVertexArray, dwIndexCount * sizeof(D3DVERTEX) );

	 bResult = (*pNewBuffer)->Unlock(dwFlags);
	 if( false == bResult )
	 {
		WriteToLog( TEXT("Unlock() failed.\n") );
		delete [] lpNewVertexArray;
		return false;
	 }


	delete [] lpNewVertexArray;
	lpNewVertexArray = NULL;


	return true;
}
