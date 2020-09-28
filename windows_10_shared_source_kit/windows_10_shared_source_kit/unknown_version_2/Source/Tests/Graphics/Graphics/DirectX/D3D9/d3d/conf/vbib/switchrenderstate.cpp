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
// Statics and globals
//
/////////////////////////////////////////////////////////////////
#if FAST_TEST
const DWORD SwitchRenderState::Usages[] =
{
    0,
    USAGE_WRITEONLY,
    USAGE_DONOTCLIP,
    USAGE_POINTS,
    USAGE_DYNAMIC,
    USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_POINTS | USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_WRITEONLY | USAGE_DYNAMIC,
    USAGE_DONOTCLIP | USAGE_DYNAMIC,
    USAGE_WRITEONLY | USAGE_DONOTCLIP | USAGE_DYNAMIC,
};

const DWORD SwitchRenderState::Pools[] =
{
    POOL_DEFAULT,
    POOL_MANAGED,
    POOL_SYSTEMMEM,
};

const D3DTEST_NORMALBUFFER SwitchRenderState::TestStages[] =
{
    { TEXT("FVF = VERTEX, FMT = INDEX16"), NULL, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_DEFAULT },
};

#else //FAST_TEST

const DWORD SwitchRenderState::Usages[] =
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


const DWORD SwitchRenderState::Pools[] =
{
	POOL_DEFAULT,
	POOL_MANAGED,
	POOL_SYSTEMMEM,
};


const D3DTEST_NORMALBUFFER SwitchRenderState::TestStages[] =
{
	{ TEXT("FVF = VERTEX, FMT = INDEX16"), NULL, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_DEFAULT },
};

#endif //FAST_TEST

static SwitchRenderStateTestCase s_SwitchRenderStateTestCases
[
	COUNTOF( SwitchRenderState::Usages ) *
	COUNTOF( SwitchRenderState::Pools ) *
	COUNTOF( SwitchRenderState::TestStages )
];


/////////////////////////////////////////////////////////////////
//
// Construction / destruction
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Function:  SwitchRenderState
//
// Purpose:
// Constructor.
////////////////////////////////////////////////////////////
SwitchRenderState::SwitchRenderState()
{
	m_szTestName = _T("VBIB Switch Render State");
	m_szCommandKey = _T("SwitchRenderState");

	m_Options.D3DOptions.dwBehaviors = DEVICECREATE_FPU_PRESERVE | DEVICECREATE_SWVP | DEVICECREATE_MIXED | DEVICECREATE_STANDARD;
};


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
UINT SwitchRenderState::TestInitialize()
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
			for( UINT iCurrentStage = 0; iCurrentStage < COUNTOF( TestStages ); ++iCurrentStage )
			{
				// Skip invalid combinations of parameters:
				if
				(
					( Usages[ iCurrentUsage ] & USAGE_DYNAMIC ) && ( Pools[ iCurrentPool ] == POOL_MANAGED )
				)
				{
					continue;
				};

				ASSERT( cTestCases < COUNTOF( s_SwitchRenderStateTestCases ) );

				s_SwitchRenderStateTestCases[ cTestCases ].nTest = cTestCases;

				s_SwitchRenderStateTestCases[ cTestCases ].Usage = Usages[ iCurrentUsage ];
				s_SwitchRenderStateTestCases[ cTestCases ].Pool = Pools[ iCurrentPool ];
				s_SwitchRenderStateTestCases[ cTestCases ].TestStage = iCurrentStage;

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
	// Skip devices that aren't mixed mode.
	//

	if( !(m_pSrcDevice->GetBehaviors() & DEVICECREATE_MIXED ) )
	{
		return D3DTESTINIT_SKIPALL;
	};


	//
	// Initialize our shape.
	//

	m_shape.NewShape( CS_SPHERE, 10 );


	UINT uiRet = D3DTESTINIT_RUN;

	uiRet = TestInitReal(CD3D_SRC);
	if( uiRet != D3DTESTINIT_RUN )
	{
		return uiRet;
	}

	uiRet = TestInitReal(CD3D_REF);
	if( uiRet != D3DTESTINIT_RUN )
	{
		return uiRet;
	}


	return uiRet;
}


////////////////////////////////////////////////////////////
// Function:  ExecuteTest
//
// Purpose:
// Runs a test case.
////////////////////////////////////////////////////////////
#if FAST_TEST
bool SwitchRenderState::ExecuteTest
(
UINT nTest
)
{

    //
    // Begin the test case.
    //

    BeginTestCase("Switch Render State - fast", nTest);


    //
    // Clear the frame in preparation for rendering.
    //

    ClearFrame();


    auto& drawLamda = [=](UINT testIndex) -> bool
    {
        //
        // Validate the test case ordinal.
        //

        if (testIndex > COUNTOF(s_SwitchRenderStateTestCases))
        {
            WriteToLog
                (
                _T("Invalid test ordinal %u."),
                testIndex
                );

            return false;
        };


        //
        // Look up test case data.
        //

        D3DTEST_NORMALBUFFER curtest;

        curtest.Usage = s_SwitchRenderStateTestCases[testIndex - 1].Usage;
        curtest.Pool = s_SwitchRenderStateTestCases[testIndex - 1].Pool;
        curtest.szName = TestStages[s_SwitchRenderStateTestCases[testIndex - 1].TestStage].szName;
        curtest.VBFVF = TestStages[s_SwitchRenderStateTestCases[testIndex - 1].TestStage].VBFVF;
        curtest.IBFormat = TestStages[s_SwitchRenderStateTestCases[testIndex - 1].TestStage].IBFormat;

        //
        // Hand off to actually run the test case.
        //

        BOOL bRetTest = ExecuteReal
            (
            &curtest,
            FALSE,
            CD3D_SRC
            );

        BOOL bRetRef = ExecuteReal
            (
            &curtest,
            TRUE,
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
#else //FAST_TEST
bool SwitchRenderState::ExecuteTest
(
	UINT nTest
)
{
	//
	// Validate the test case ordinal.
	//

	if ( nTest > COUNTOF( s_SwitchRenderStateTestCases ) )
	{
		WriteToLog
		(
			_T( "Invalid test ordinal %u." ),
			nTest
		);

		return false;
	};


	//
	// Look up test case data.
	//

	D3DTEST_NORMALBUFFER curtest;

	curtest.Usage =  s_SwitchRenderStateTestCases[ nTest - 1 ].Usage;
	curtest.Pool =  s_SwitchRenderStateTestCases[ nTest - 1 ].Pool;
	curtest.szName = TestStages[ s_SwitchRenderStateTestCases[ nTest - 1 ].TestStage ].szName;
	curtest.VBFVF = TestStages[ s_SwitchRenderStateTestCases[ nTest - 1 ].TestStage ].VBFVF;
	curtest.IBFormat = TestStages[ s_SwitchRenderStateTestCases[ nTest - 1 ].TestStage ].IBFormat;


	//
	// Build the test case name.
	//

	TCHAR sz[ 256 ] = _T( "" );
	wsprintf
	(
		sz,
		TEXT("Usage = 0x%x, Pool = %d, %s"),
		s_SwitchRenderStateTestCases[ nTest - 1 ].Usage,
		s_SwitchRenderStateTestCases[ nTest - 1 ].Pool,
		TestStages[ s_SwitchRenderStateTestCases[ nTest - 1 ].TestStage ].szName
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
	// Hand off to actually run the test case.
	//

	BOOL bRetTest = ExecuteReal
	(
		&curtest,
		FALSE,
		CD3D_SRC
	);

	BOOL bRetRef = ExecuteReal
	(
		&curtest,
		TRUE,
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
D3DTESTINIT SwitchRenderState::TestInitReal
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
		SetRenderState( D3DRS_CLIPPING, FALSE, dwFlags );  // so donotclip buffers won't die in process vertices

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
	} while(0);


	return( false == bResult ? D3DTESTINIT_ABORT : D3DTESTINIT_RUN );
}


////////////////////////////////////////////////////////////
// Function:  ExecuteReal
//
// Purpose:
// Handles the heavy lifting in running a test case.
////////////////////////////////////////////////////////////
BOOL SwitchRenderState::ExecuteReal
(
	const D3DTEST_NORMALBUFFER *curtest,
	BOOL fReference,
	DWORD dwFlags
)
{
	bool bResult = true;
	LPVERTEXBUFFERS pVB = NULL;
	LPINDEXBUFFERS pIB = NULL;
	LPBYTE pbObject = NULL;
	BOOL bRet = FALSE;
	BOOL dwSWVertProc;
	SIZE_T MemFreeStart;
	SIZE_T MemFreeEnd;

	if( !fReference )
	{
		dwSWVertProc = GetSoftwareVertexProcessing(dwFlags );

		MemFreeStart = GetAvailMemory( curtest->Pool );
	}

	do
	{
		bResult = CreateVertexBuffer(
			m_shape.m_nVertices * sizeof(D3DVERTEX),
			curtest->Usage | (fReference ? 0 : USAGE_SOFTWAREPROCESSING),
			D3DFVF_VERTEX,
			curtest->Pool,
			&pVB,
			dwFlags
		);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed.\n"),
				m_shape.m_nVertices * sizeof(D3DVERTEX),
				curtest->Usage | (fReference ? 0 : USAGE_SOFTWAREPROCESSING),
				D3DFVF_VERTEX,
				curtest->Pool
			);
			continue;
		}

		bResult = pVB->Lock(
			0,
			m_shape.m_nVertices * sizeof(D3DVERTEX),
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

		CopyMemory( pbObject, m_shape.m_pVertices, m_shape.m_nVertices * sizeof(D3DVERTEX) );

		bResult = pVB->Unlock(dwFlags);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("Unlock() failed.\n")
			);
			continue;
		}


		bResult = CreateIndexBuffer(
			m_shape.m_nIndices * sizeof(WORD),
			curtest->Usage | (fReference ? 0 : USAGE_SOFTWAREPROCESSING),
			curtest->IBFormat,
			curtest->Pool,
			&pIB,
			dwFlags
		);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,) failed.\n"),
				m_shape.m_nIndices * sizeof(WORD),
				curtest->Usage | (fReference ? 0 : USAGE_SOFTWAREPROCESSING),
				curtest->IBFormat,
				curtest->Pool
			);
			continue;
		}

		bResult = pIB->Lock(
			0,
			m_shape.m_nIndices * sizeof(WORD),
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

		CopyMemory( pbObject, m_shape.m_pIndices, m_shape.m_nIndices * sizeof(WORD) );

		bResult = pIB->Unlock(dwFlags);
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
				TEXT("SetStreamSource(pVB) failed.\n")
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


		bResult = SetVertexShader( D3DFVF_VERTEX, dwFlags );
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetVertexShader(D3DFVF_VERTEX) failed.\n")
			);
			continue;
		}

		if( !fReference )
		{
			// switch render state
			bResult = SetSoftwareVertexProcessing( !dwSWVertProc, dwFlags );
			if( false == bResult )
			{
				WriteToLog(
					TEXT("SetRenderState( %d ) failed.\n"),
					!dwSWVertProc
				);
				continue;
			}


			// set stream source
			LPVERTEXBUFFERS pVBRet = NULL;
			UINT iStride;

			// get and compare
			bResult = GetStreamSource(
				0,
				&pVBRet,
				&iStride,
				dwFlags
			);
			if( false == bResult )
				WriteToLog( TEXT("GetStreamSource() failed.\n") );
			else if( pVBRet != NULL )
			{
				RELEASE(pVBRet);
				WriteToLog(TEXT("GetStreamSource() didn't return NULL pointer after renderstate change\n"));
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

			// set indices
			LPINDEXBUFFERS pIBRet = NULL;
			UINT iBaseIndex = 0;

			// get and compare
			bResult = GetIndices(
				&pIBRet,
				&iBaseIndex,
				dwFlags
			);
			if( false == bResult )
			{
				WriteToLog(
					TEXT("GetIndices() failed.\n")
				);
			}
			else if( pIBRet != NULL )
			{
				RELEASE(pIBRet);
				WriteToLog(TEXT("GetIndices() didn't return NULL pointer after renderstate change\n"));
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
			

			// set vertex shader
			DWORD dwShader;

			// get and compare
			if( dwFlags & CD3D_SRC )
			bResult = GetFVF( &dwShader, CD3D_SRC );
			if( false == bResult )
			{
				WriteToLog(
					TEXT("GetFVF(CD3D_SRC) failed.\n")
				);
			}
			else if( dwShader != NULL )
			{
				WriteToLog(TEXT("GetFVF(CD3D_SRC) didn't return empty handle after renderstate change\n"));
			}

			// get and compare
			if( dwFlags & CD3D_REF )
			bResult = GetFVF( &dwShader, CD3D_REF );
			if( false == bResult )
			{
				WriteToLog(
					TEXT("GetFVF(CD3D_REF) failed.\n")
				);
			}
			else if( dwShader != NULL )
			{
				WriteToLog(TEXT("GetFVF(CD3D_REF) didn't return empty handle after renderstate change\n"));
			}

			bResult = SetVertexShader( D3DFVF_VERTEX, dwFlags );
			if( false == bResult )
			{
				WriteToLog(
					TEXT("SetVertexShader(D3DFVF_VERTEX) failed.\n")
				);
				continue;
			}
		}

		Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0, dwFlags );

		bRet = TRUE;

		bResult = BeginScene(dwFlags);
		if( false == bResult )
		{
			WriteToLog(
				TEXT("BeginScene() failed.\n")
				);
			bRet = FALSE;
			continue;
		}

		bResult = DrawIndexedPrimitive(
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
			//continue;  // make sure EndScene() is always called
		}

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


	RELEASE(pIB);
	RELEASE(pVB);

	if( !fReference )
	{
		bResult = SetSoftwareVertexProcessing( dwSWVertProc, dwFlags );
		if( false == bResult )
		{
			WriteToLog(
				TEXT("SetSoftwareVertexProcessing( dwSWVertProc ) failed.\n")
			);
			bRet = FALSE;
		}


		// get avail memory, compare to start
		MemFreeEnd = GetAvailMemory( curtest->Pool );

		if( MemFreeEnd < MemFreeStart )
		{
			WriteToLog(
				TEXT("memory leaked: %d\n"),
				MemFreeStart - MemFreeEnd
			);
			bRet = FALSE;
		}
	}


	return true;
}
