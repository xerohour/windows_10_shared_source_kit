//////////////////////////////////////////////////////////////////////
// File:  HugeBuffer.cpp
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
// Implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction / destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  HugeBuffer
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
HugeBuffer::HugeBuffer()
:
	m_MemSize( 0 )
{
	m_szTestName = _T("VBIB Huge Buffer");
	m_szCommandKey = _T("HugeBuffer");
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
UINT HugeBuffer::TestInitialize()
{
	//
	// Set the test range.
	//
#if FAST_TEST

    HugeBufferTestCase HugeBufferTestCases[] =
    {
        //  { szName, shape, VBFVF, IBFormat, Usage, Pool },

        { TEXT("Pool = DEFAULT"), &m_shape15bit, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_DEFAULT },
        { TEXT("Pool = MANAGED"), &m_shape15bit, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_MANAGED },
        { TEXT("Pool = SYSTEMMEM"), &m_shape15bit, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_SYSTEMMEM },

        { TEXT("Pool = DEFAULT"), &m_shape16bit, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_DEFAULT },
        { TEXT("Pool = MANAGED"), &m_shape16bit, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_MANAGED },
        { TEXT("Pool = SYSTEMMEM"), &m_shape16bit, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_SYSTEMMEM },

        { TEXT("Pool = DEFAULT"), &m_shape, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_DEFAULT },
        { TEXT("Pool = MANAGED"), &m_shape, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_MANAGED },
        { TEXT("Pool = SYSTEMMEM"), &m_shape, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_SYSTEMMEM },

        { TEXT("Pool = DEFAULT"), &m_shape, 1, D3DFVF_VERTEX, FMT_INDEX32, NULL, POOL_DEFAULT },
        { TEXT("Pool = MANAGED"), &m_shape, 1, D3DFVF_VERTEX, FMT_INDEX32, NULL, POOL_MANAGED },
        { TEXT("Pool = SYSTEMMEM"), &m_shape, 1, D3DFVF_VERTEX, FMT_INDEX32, NULL, POOL_SYSTEMMEM },

        { TEXT("Pool = DEFAULT"), &m_shape, 10, D3DFVF_VERTEX, FMT_INDEX32, NULL, POOL_DEFAULT },
        { TEXT("Pool = MANAGED"), &m_shape, 10, D3DFVF_VERTEX, FMT_INDEX32, NULL, POOL_MANAGED },
        { TEXT("Pool = SYSTEMMEM"), &m_shape, 10, D3DFVF_VERTEX, FMT_INDEX32, NULL, POOL_SYSTEMMEM },
    };

    //Hack To work around weird test not working with Lambda
    memcpy(m_pTestCases, HugeBufferTestCases, sizeof(m_pTestCases));
    //
    // Set the test range.
    //
    m_totalNumTestCases = COUNTOF(HugeBufferTestCases);
    SetTestRange(1, 1);

#else//FAST_TEST
    ExecuteTest( (UINT)(-1) );
#endif//FAST_TEST


	//
	// Initialize our shapes.
	//

	m_shape15bit.NewShape( CS_SPHERE, 127 );
	m_shape16bit.NewShape( CS_SPHERE, 128 );

	m_shape.NewShape( CS_SPHERE, 181 );

    //
	// Determine the amount of total physical memory.
	//

	MEMORYSTATUS memstat;
	GlobalMemoryStatus(&memstat);
	m_MemSize = memstat.dwTotalPhys;


	UINT uiRet = D3DTESTINIT_RUN;

	/*do
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
	}
	while(0);*/
	uiRet = TestInitReal();

	return uiRet;
}


///////////////////////////////////////////////////////
// Function:  ExecuteTest
//
// Purpose:
// Runs a test case.
///////////////////////////////////////////////////////
#if FAST_TEST
bool HugeBuffer::ExecuteTest
(
UINT nTest
)
{

    //
    // Clear the frame in preparation for rendering.
    //

    ClearFrame();

    //
    // Begin the test case.
    //

    BeginTestCase("Huge Buffer - Fast", nTest);

    auto& drawLamda = [=](UINT testIndex) -> bool
    {
        //
        // Look up test case data.
        //

        const HugeBufferTestCase *curtest = &m_pTestCases[testIndex - 1];


        UINT NumVertices = curtest->shape->m_nVertices * curtest->iReps;
        UINT NumPrims = (curtest->shape->m_nIndices * curtest->iReps) / 3;

        // skip if using 32-bit index buffers and they aren't supported
        if (
            (curtest->IBFormat == FMT_INDEX32)
            && (
            (0x10000 > m_pSrcDevice->GetCaps()->dwMaxVertexIndex) ||
            (0x10000 > m_pRefDevice->GetCaps()->dwMaxVertexIndex)
            )
            )
        {
            WriteToLog
                (
                TEXT("skipping test %d: FMT_INDEX32 not supported (MaxVertexIndex is (%x,%x))\n"),
                testIndex,
                m_pSrcDevice->GetCaps()->dwMaxVertexIndex,
                m_pRefDevice->GetCaps()->dwMaxVertexIndex
                );
            return false;
        }

        // skip test if number of vertices > maxindex
        if (
            (NumVertices > m_pSrcDevice->GetCaps()->dwMaxVertexIndex) ||
            (NumVertices > m_pRefDevice->GetCaps()->dwMaxVertexIndex)
            )
        {
            WriteToLog
                (
                TEXT("skipping test %d: NumVertices (%x) greater than supported by device (%x,%x)\n"),
                testIndex,
                NumVertices,
                m_pSrcDevice->GetCaps()->dwMaxVertexIndex,
                m_pRefDevice->GetCaps()->dwMaxVertexIndex
                );
            return false;
        }

        // skip test if number of prim > maxprim
        if (
            (NumPrims > m_pSrcDevice->GetCaps()->dwMaxPrimitiveCount) ||
            (NumPrims > m_pRefDevice->GetCaps()->dwMaxPrimitiveCount)
            )
        {
            WriteToLog
                (
                TEXT("skipping test %d: NumPrimitives (%x) greater than supported by device (%x,%x)\n"),
                testIndex,
                NumPrims,
                m_pSrcDevice->GetCaps()->dwMaxPrimitiveCount,
                m_pRefDevice->GetCaps()->dwMaxPrimitiveCount
                );
            return false;
        }

        // skip larger shapes if not enough memory
        if ((NumVertices > 500000) && (m_MemSize < 0x10000000 /*256M*/))
        {
            return false;
        }

        //
        // Hand off to actually run the test.
        //

        BOOL bSrcRet = ExecuteReal
            (
            curtest,
            (m_pSrcDevice->GetBehaviors() & DEVICECREATE_MIXED),
            CD3D_SRC
            );

        BOOL bRebRet = ExecuteReal
            (
            curtest,
            (m_pRefDevice->GetBehaviors() & DEVICECREATE_MIXED),
            CD3D_REF
            );


        bool bRet = bSrcRet && bRebRet;

        if (!bRet)
        {
            Fail();
        }

        return bRet;
    };

    return DrawMultiTests(drawLamda);
}
#else //FAST_TEST

bool HugeBuffer::ExecuteTest
(
	UINT nTest
)
{
	static const HugeBufferTestCase HugeBufferTestCases[] =
	{
	//  { szName, shape, VBFVF, IBFormat, Usage, Pool },

		{ TEXT("Pool = DEFAULT"), &m_shape15bit, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_DEFAULT },
		{ TEXT("Pool = MANAGED"), &m_shape15bit, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_MANAGED },
		{ TEXT("Pool = SYSTEMMEM"), &m_shape15bit, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_SYSTEMMEM },

		{ TEXT("Pool = DEFAULT"), &m_shape16bit, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_DEFAULT },
		{ TEXT("Pool = MANAGED"), &m_shape16bit, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_MANAGED },
		{ TEXT("Pool = SYSTEMMEM"), &m_shape16bit, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_SYSTEMMEM },

		{ TEXT("Pool = DEFAULT"), &m_shape, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_DEFAULT },
		{ TEXT("Pool = MANAGED"), &m_shape, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_MANAGED },
		{ TEXT("Pool = SYSTEMMEM"), &m_shape, 1, D3DFVF_VERTEX, FMT_INDEX16, NULL, POOL_SYSTEMMEM },

		{ TEXT("Pool = DEFAULT"), &m_shape, 1, D3DFVF_VERTEX, FMT_INDEX32, NULL, POOL_DEFAULT },
		{ TEXT("Pool = MANAGED"), &m_shape, 1, D3DFVF_VERTEX, FMT_INDEX32, NULL, POOL_MANAGED },
		{ TEXT("Pool = SYSTEMMEM"), &m_shape, 1, D3DFVF_VERTEX, FMT_INDEX32, NULL, POOL_SYSTEMMEM },

		{ TEXT("Pool = DEFAULT"), &m_shape, 10, D3DFVF_VERTEX, FMT_INDEX32, NULL, POOL_DEFAULT },
		{ TEXT("Pool = MANAGED"), &m_shape, 10, D3DFVF_VERTEX, FMT_INDEX32, NULL, POOL_MANAGED },
		{ TEXT("Pool = SYSTEMMEM"), &m_shape, 10, D3DFVF_VERTEX, FMT_INDEX32, NULL, POOL_SYSTEMMEM },
	};


	if ( nTest == (UINT) -1 )
	{
		//
		// Set the test range.
		//

		SetTestRange
		(
			1,
			COUNTOF( HugeBufferTestCases )
		);

		return false;
	}
	else if ( nTest > COUNTOF( HugeBufferTestCases ) )
	{
		//
		// Validate the test case ordinal.
		//

		return false;
	};


	//
	// Clear the frame in preparation for rendering.
	//

	ClearFrame();


	//
	// Look up test case data.
	//

	const HugeBufferTestCase *curtest = &HugeBufferTestCases[ nTest - 1 ];


	UINT NumVertices = curtest->shape->m_nVertices * curtest->iReps;
	UINT NumPrims = (curtest->shape->m_nIndices * curtest->iReps) / 3;

	// skip if using 32-bit index buffers and they aren't supported
	if(
		(curtest->IBFormat == FMT_INDEX32)
		&& (
			(0x10000 > m_pSrcDevice->GetCaps()->dwMaxVertexIndex) ||
			(0x10000 > m_pRefDevice->GetCaps()->dwMaxVertexIndex)
		)
	)
	{
		WriteToLog
		(
			TEXT("skipping test %d: FMT_INDEX32 not supported (MaxVertexIndex is (%x,%x))\n"),
			nTest,
			m_pSrcDevice->GetCaps()->dwMaxVertexIndex,
			m_pRefDevice->GetCaps()->dwMaxVertexIndex
		);
		return false;
	}

	// skip test if number of vertices > maxindex
	if(
		(NumVertices > m_pSrcDevice->GetCaps()->dwMaxVertexIndex) ||
		(NumVertices > m_pRefDevice->GetCaps()->dwMaxVertexIndex)
	)
	{
		WriteToLog
		(
			TEXT("skipping test %d: NumVertices (%x) greater than supported by device (%x,%x)\n"),
			nTest,
			NumVertices,
			m_pSrcDevice->GetCaps()->dwMaxVertexIndex,
			m_pRefDevice->GetCaps()->dwMaxVertexIndex
		);
		return false;
	}

	// skip test if number of prim > maxprim
	if(
		(NumPrims > m_pSrcDevice->GetCaps()->dwMaxPrimitiveCount) ||
		(NumPrims > m_pRefDevice->GetCaps()->dwMaxPrimitiveCount)
	)
	{
		WriteToLog
		(
			TEXT("skipping test %d: NumPrimitives (%x) greater than supported by device (%x,%x)\n"),
			nTest,
			NumPrims,
			m_pSrcDevice->GetCaps()->dwMaxPrimitiveCount,
			m_pRefDevice->GetCaps()->dwMaxPrimitiveCount
		);
		return false;
	}

	// skip larger shapes if not enough memory
	if( (NumVertices > 500000) && (m_MemSize < 0x10000000 /*256M*/) )
	{
		return false;
	}


	//
	// Build the test case name.
	//

	TCHAR sz[ MAX_PATH ] = _T( "" );
	wsprintf
	(
		sz,
		TEXT("%d vertices, %d indices: %s"),
		curtest->shape->m_nVertices * curtest->iReps,
		curtest->shape->m_nIndices * curtest->iReps,
		curtest->szName
	);


	//
	// Begin the test case.
	//

	BeginTestCase( sz, nTest );


	//
	// Hand off to actually run the test.
	//

	BOOL bSrcRet = ExecuteReal
	(
		curtest,
		(m_pSrcDevice->GetBehaviors() & DEVICECREATE_MIXED),
		CD3D_SRC
	);

	BOOL bRebRet = ExecuteReal
	(
		curtest,
		(m_pRefDevice->GetBehaviors() & DEVICECREATE_MIXED),
		CD3D_REF
	);


	bool bRet = bSrcRet && bRebRet;

	if( !bRet )
	{
		Fail();
	}

	return bRet;
}
#endif //FAST_TEST


///////////////////////////////////////////////////////
// Function:  TestInitReal
//
// Purpose:
//
///////////////////////////////////////////////////////
D3DTESTINIT HugeBuffer::TestInitReal
(
	DWORD dwFlags
)
{
	bool bResult = false;


	//
	// Initialize our matrices.
	//

	D3DMATRIX matView =
	{
		1.0f,    0,       0,       0,
		0,       1.0f,    0,       0,
		0,       0,       1.0f,    0,
		0,       0,       2.0f,/*10.0f,*/   1.0f
	};


	D3DMATRIX matProj =
	{
		2.0f,    0,       0,       0,
		0,       2.0f,    0,       0,
		0,       0,       1.0f,    1.0f,
		0,       0,       -1.0f,   0.0f
	};


	D3DMATRIX matWorld =
	{
		1.0f,    0,       0,       0,
		0,       1.0f,    0,       0,
		0,       0,       1.0f,    0,
		0,       0,       0,       1.0f
	};

	D3DXMATRIX d3dMat;

	D3DXMATRIX d3dMatOut;


	//
	// Initialize our material.
	//

	MATERIAL mtrl =
	{
		{ 1.0f, 0.0f, 0.0f, 0.0f },  // diffuse
		{ 0.0f, 0.3f, 0.0f, 0.5f },  // ambient
		{ 1.0f, 0.0f, 0.0f, 0.0f },	 // specular
		{ 0.0f, 0.0f, 0.0f, 0.0f },	 // emissive
		0.0f  // sharpness (for specular)
	};


	//
	// Setup our view port.
	//

	bResult =  SetTransform( D3DTS_VIEW, &matView, dwFlags );
	if( !bResult )
	{
		WriteToLog
		(
			TEXT("SetTransform(D3DTS_VIEW,) failed.\n")
		);

		goto Cleanup;
	}

	bResult = SetTransform( D3DTS_PROJECTION, &matProj, dwFlags );
	if( !bResult )
	{
		WriteToLog(
			TEXT("SetTransform(D3DTS_PROJECTION,) failed.\n")
		);

		goto Cleanup;
	}


	d3dMat = matWorld;

	D3DXMatrixMultiply
	(
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
	if( !bResult )
	{
		WriteToLog
		(
			TEXT("SetTransform(D3DTS_WORLD,) failed.\n")
		);

		goto Cleanup;
	}


	//
	// Set render states.
	//

	SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB( 0xff, 0xff, 0xff ), dwFlags );
	SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE, dwFlags );
	SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE, dwFlags );
	SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE, dwFlags );
	SetRenderState( D3DRS_CLIPPING, FALSE, dwFlags );


	//
	// Set our material.
	//

	bResult = SetMaterial(&mtrl, dwFlags);
	if( !bResult )
	{
		WriteToLog
		(
			TEXT("SetMaterial() failed.\n")
		);

		goto Cleanup;
	}


Cleanup:
	return( !bResult ? D3DTESTINIT_ABORT : D3DTESTINIT_RUN );
};


///////////////////////////////////////////////////////
// Function:  ExecuteReal
//
// Purpose:
// Handles the heavy lifting in running a test case.
///////////////////////////////////////////////////////
BOOL HugeBuffer::ExecuteReal
(
	const HugeBufferTestCase *lpBufInfo,
	BOOL fMixedMode,
	DWORD dwFlags
)
{
	bool bResult =  true;
	LPVERTEXBUFFERS pVB = NULL;
	LPINDEXBUFFERS pIB = NULL;
	LPBYTE pVertices = NULL;
	LPBYTE pIndices = NULL;
	BOOL bRet = FALSE;

	BOOL fSoftwareUsage = FALSE;

	// determine sw usage
	if( fMixedMode )
		fSoftwareUsage = GetSoftwareVertexProcessing( dwFlags );
	// determine sw usage
	/*if( fMixedMode )
	{
		DWORD dwSW;
		GetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, &dwSW, dwFlags );
		fSoftwareUsage = dwSW;
	}*/

	UINT nVertices = lpBufInfo->iReps * lpBufInfo->shape->m_nVertices;
	UINT nIndices = lpBufInfo->iReps * lpBufInfo->shape->m_nIndices;

	if( ((nVertices > (UINT)0xffff) && (lpBufInfo->IBFormat == D3DFMT_INDEX16)) )
	{
		WriteToLog(
			TEXT("nVertices (%d) is greater than WORD index can hold, image may look strange\n"),
			nVertices
		);
	}

	do
	{
		//
		// Create the vertex buffer.
		//

		bResult = CreateVertexBuffer(
			nVertices * sizeof(D3DVERTEX),
			lpBufInfo->Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : 0),
			lpBufInfo->VBFVF,
			lpBufInfo->Pool,
			&pVB,
			dwFlags
		);
		if( !bResult )
		{
			WriteToLog(
				TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed.\n"),
				nVertices * sizeof(D3DVERTEX),
				lpBufInfo->Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : 0),
				lpBufInfo->VBFVF,
				lpBufInfo->Pool
			);
			continue;
		}


		//
		// Create the index buffer.
		//

		bResult = CreateIndexBuffer(
			nIndices * ( (lpBufInfo->IBFormat == D3DFMT_INDEX32) ? sizeof(DWORD) : sizeof(WORD) ),
			lpBufInfo->Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : 0),
			lpBufInfo->IBFormat,
			lpBufInfo->Pool,
			&pIB,
			dwFlags
		);
		if( !bResult )
		{
			WriteToLog(
				TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,) failed.\n"),
				nIndices * ( (lpBufInfo->IBFormat == D3DFMT_INDEX32) ? sizeof(DWORD) : sizeof(WORD) ),
				lpBufInfo->Usage ^ (fSoftwareUsage ? USAGE_SOFTWAREPROCESSING : 0),
				lpBufInfo->IBFormat,
				lpBufInfo->Pool
			);
			continue;
		}


		//
		// Lock the vertex buffer.
		//

		bResult = pVB->Lock(
			0,
			nVertices * sizeof(D3DVERTEX),
			&pVertices,
			0,
			dwFlags
		);
		if( !bResult )
		{
			WriteToLog(
				TEXT("Lock() failed.\n")
			);
			break;
		}

		if ( pVertices == NULL )
		{
			WriteToLog(
				TEXT( "Lock succeeded, but pVertices == NULL." )
			);
			break;
		}


		//
		// Lock the index buffer.
		//

		bResult = pIB->Lock(
			0,
			nIndices * ( (lpBufInfo->IBFormat == D3DFMT_INDEX32) ? sizeof(DWORD) : sizeof(WORD) ),
			&pIndices,
			0,
			dwFlags
		);

		if( !bResult )
		{
			WriteToLog(
				TEXT("Lock() failed.\n")
			);
			break;
		}

		if ( pIndices == NULL )
		{
			WriteToLog(
				TEXT( "Lock succeeded, but pIndices == NULL." )
			);
			break;
		}


		ASSERT( lpBufInfo->shape != NULL );
		ASSERT( lpBufInfo->shape->m_pVertices != NULL );
		for(
			UINT iVertStart = 0, iIndexStart = 0;
			((iVertStart + lpBufInfo->shape->m_nVertices) <= nVertices) && ((iIndexStart + lpBufInfo->shape->m_nIndices) <= nIndices);
			iVertStart += lpBufInfo->shape->m_nVertices, iIndexStart += lpBufInfo->shape->m_nIndices
		)
		{
			for( UINT iTemp = 0; iTemp < lpBufInfo->shape->m_nVertices; iTemp++ )
			{
				((D3DVERTEX*)pVertices)[iTemp+iVertStart] = lpBufInfo->shape->m_pVertices[iTemp];
			}

			switch( lpBufInfo->IBFormat)
			{
				case D3DFMT_INDEX16:
				{
					for( UINT iTemp = 0; iTemp < lpBufInfo->shape->m_nIndices; iTemp++ )
					{
						((WORD*)pIndices)[iTemp+iIndexStart] = (WORD)lpBufInfo->shape->m_pIndices[iTemp] + (WORD)iVertStart;
					}
				}
					break;

				case D3DFMT_INDEX32:
				{
					for( UINT iTemp = 0; iTemp < lpBufInfo->shape->m_nIndices; iTemp++ )
					{
						((DWORD*)pIndices)[iTemp+iIndexStart] = (DWORD)lpBufInfo->shape->m_pIndices[iTemp] + (DWORD)iVertStart;
					}
				}
					break;

				default:
					break;
			}
		}


		//
		// Unlock the vertex buffer.
		//

		bResult = pVB->Unlock(dwFlags);
		if( !bResult )
		{
			WriteToLog(
				TEXT("Unlock() failed.\n")
			);
			break;
		}


		//
		// Unlock the index buffer.
		//

		bResult = pIB->Unlock(dwFlags);
		if( !bResult )
		{
			WriteToLog(
				TEXT("Unlock() failed.\n")
			);
			break;
		}


		//
		// Set the stream source to the vertex buffer.
		//

		bResult = SetStreamSource(
			0,
			pVB,
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


		//
		// Set the index source to the index buffer.
		//

		bResult = SetIndices(
			pIB,
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

		bResult = SetVertexShader( D3DFVF_VERTEX, dwFlags );
		if( !bResult )
		{
			WriteToLog(
				TEXT("SetVertexShader() failed.\n")
			);
			continue;
		}


		//
		// Draw the scene.
		//

		bResult = Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0, dwFlags );
		if( !bResult )
		{
			WriteToLog(
				TEXT("Clear() failed.\n")
			);
			continue;
		}

		bResult = BeginScene(dwFlags);
		if( !bResult )
		{
			WriteToLog(
				TEXT("BeginScene() failed.\n")
			);
			bRet = FALSE;
			continue;
		}

		bRet = TRUE;

		bResult = DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			nVertices,
			0,
			nIndices / 3,
			0,
			dwFlags
		);
		if( !bResult )
		{
			WriteToLog(TEXT("DrawIndexedPrimitive() failed.\n"));
			// If we get out outofvideomemory error, tolerate it.
			// It's probably a runtime bug (DirectX #82495)
			//
			if (GetLastError() == D3DERR_OUTOFVIDEOMEMORY)
			{
				WriteToLog( TEXT("This is an acceptable error code.\n") );

			}
			else
				bRet = FALSE;
			//continue;  // so EndScene() is always called
		}

		bResult = EndScene(dwFlags);
		if( !bResult )
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

	if( !bRet )
	{
		Fail();
	}

	return bRet;
}
