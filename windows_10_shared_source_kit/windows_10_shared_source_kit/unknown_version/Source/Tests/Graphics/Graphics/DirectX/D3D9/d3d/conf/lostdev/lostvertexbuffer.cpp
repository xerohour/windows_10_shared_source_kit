// ============================================================================
//
//  LostVertexBuffer.cpp
//
// ============================================================================

#include "LostDev.h"
#include "CasesManager.h"
#include "CBuffers.h"

#define LISTOTHEROPTIONS 0
#define LISTPOOLS 1
#define LISTUSAGE 2

#define LISTLOCKS 0


// ----------------------------------------------------------------------------
// ****************************	CLostVertexBuffer *****************************
// ----------------------------------------------------------------------------

CLostVertexBuffer::CLostVertexBuffer()
{
	m_szTestName = _T("LostVertexBuffer");
	m_szCommandKey = _T("LostVertexBuffer");
	m_TestName = _T("Lost Devices with CreateVertexBuffer");

	m_Options.D3DOptions.fMinDXVersion = 6.f;

	//This factor is used to test with and without index buffer
	//Create the factors for pools and lost ways
	if(	!CHECKREF(m_rIndexBuffer = NewFactor( 2 )) )
	{
		DPF(0, "ERROR setting the cases in LostVertexBuffer()");
	}

	//List of Usage flags
	const CFlagCombinations::FLAG flagUsage[2] =
	{
//		{0,						_T("NONE")},
		{D3DUSAGE_DYNAMIC,		_T("Dynamic")},
		{D3DUSAGE_WRITEONLY,	_T("WriteOnly")}
	};
	//List of Usage flags combinations
	DWORD combUsage[4] = 
	{
		0,
		D3DUSAGE_DYNAMIC,
		D3DUSAGE_WRITEONLY,
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY
	};

	m_flagsUsage.Init(flagUsage, 2);
	m_flagsUsage.SetCombinations( combUsage, 4 );

	//List of Lock flags
	const CFlagCombinations::FLAG flagLock[4] =
	{
//		{0,						_T("NONE")},
		{D3DLOCK_NOSYSLOCK,		_T("NOSYSLOCK")},
		{D3DLOCK_READONLY,		_T("READONLY")},
		{D3DLOCK_DISCARD,		_T("DISCARD")},
		{D3DLOCK_NOOVERWRITE,	_T("NOOVERWRITE")}
	};
	m_flagsUsageNoneLock.Init(flagLock, 4);
	m_flagsUsageDynamicLock.Init(flagLock, 4);
	m_flagsUsageWriteonlyLock.Init(flagLock, 4);
	m_flagsUsageDynamicWriteonlyLock.Init(flagLock, 4);

	//List of Lock flags combinations for Usage with no flags
	DWORD combUsageNoneLock[2] = 
	{
		0,
		D3DLOCK_NOSYSLOCK,
		//D3DLOCK_READONLY,		//No need to test READONLY
		//D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY
	};
	m_flagsUsageNoneLock.SetCombinations( combUsageNoneLock, 2 );

	//List of Lock flags combinations for Usage with DYNAMIC or DYNAMIC | WRITEONLY
	DWORD combUsageDynamicLock[4] = 
	{
		0,
		D3DLOCK_NOSYSLOCK,
		D3DLOCK_DISCARD,
		//D3DLOCK_NOOVERWRITE,	//No need to test NOOVERWRITE
		D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD,
		//D3DLOCK_NOSYSLOCK | D3DLOCK_NOOVERWRITE
	};
	m_flagsUsageDynamicLock.SetCombinations( combUsageDynamicLock, 4 );
	m_flagsUsageDynamicWriteonlyLock.SetCombinations( combUsageDynamicLock, 4 );

	//List of Lock flags combinations for Usage with WRITEONLY
	DWORD combUsageWriteonlyLock[2] = 
	{
		0,
		D3DLOCK_NOSYSLOCK
	};
	m_flagsUsageWriteonlyLock.SetCombinations( combUsageWriteonlyLock, 2 );

	//Create the factors for pools and lost ways
	if(	!CHECKREF(m_rUsageNoneLock = NewFactor( m_flagsUsageNoneLock.Count() )) ||
		!CHECKREF(m_rUsageDynamicLock = NewFactor( m_flagsUsageDynamicLock.Count() )) ||
		!CHECKREF(m_rUsageDynamicWriteonlyLock = NewFactor( m_flagsUsageDynamicWriteonlyLock.Count() )) ||
		!CHECKREF(m_rUsageWriteonlyLock = NewFactor( m_flagsUsageWriteonlyLock.Count() )) 
		)
	{
		DPF(0, "ERROR setting the cases in LostVertexBuffer()");
	}

	
	m_pVBA = NULL; 
	m_pVBB = NULL;
	m_pIBA = NULL; 
	m_pIBB = NULL;
}

// ----------------------------------------------------------------------------

void CLostVertexBuffer::SetTestFactors()
{
	RFactor rManagedUsage = m_rUsageNoneLock + m_rUsageWriteonlyLock;
	RFactor rSystemMemUsage = m_rUsageNoneLock + m_rUsageDynamicLock + m_rUsageDynamicWriteonlyLock + m_rUsageWriteonlyLock;

	m_rCasesManager = m_rLostWays *
						m_rIndexBuffer * 
						  (	(m_rPoolSystemMem * rSystemMemUsage) + 
							(m_rPoolManaged * rManagedUsage) +
							(m_rPoolDefault)
						  );
}

// ----------------------------------------------------------------------------

void CLostVertexBuffer::SetFlags()
{
	if (m_rUsageNoneLock->Active())
	{
		m_dwLockFlags = m_flagsUsageNoneLock[ m_rUsageNoneLock->Value() ];
		m_dwUsageFlags = 0;
	}
	else if(m_rUsageDynamicLock->Active())
	{
		m_dwLockFlags = m_flagsUsageDynamicLock[ m_rUsageDynamicLock->Value() ];
		m_dwUsageFlags = D3DUSAGE_DYNAMIC;
	}
	else if(m_rUsageDynamicWriteonlyLock->Active())
	{
		m_dwLockFlags = m_flagsUsageDynamicWriteonlyLock[ m_rUsageDynamicWriteonlyLock->Value() ];
		m_dwUsageFlags = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
	}
	else if(m_rUsageWriteonlyLock->Active())
	{
		m_dwLockFlags = m_flagsUsageWriteonlyLock[ m_rUsageWriteonlyLock->Value() ];
		m_dwUsageFlags = D3DUSAGE_WRITEONLY;
	}

	strncpy(m_szLockFlags, m_flagsUsageNoneLock.Describe(m_dwLockFlags), 120);
	strncpy(m_szUsageFlags, m_flagsUsage.Describe(m_dwUsageFlags), 120);
}

// ----------------------------------------------------------------------------

bool CLostVertexBuffer::CapsCheck()
{
	return CLostDev::CapsCheck();
}

// ----------------------------------------------------------------------------

UINT CLostVertexBuffer::TestInitialize()
{
	UINT uResult = CLostDev::TestInitialize();

	// Fill the triangle points
	m_Vertices[0].x = 150.0f;
	m_Vertices[0].y = 250.0f;
	m_Vertices[0].z = 0.5f;
	m_Vertices[0].rhw = 1.0f;
	m_Vertices[0].color = 0xffff0000;

	m_Vertices[1].x = 100.0f;
	m_Vertices[1].y = 100.0f;
	m_Vertices[1].z = 0.5f;
	m_Vertices[1].rhw = 1.0f;
	m_Vertices[1].color = 0xff00ff00;

	m_Vertices[2].x = 200.0f;
	m_Vertices[2].y = 100.0f;
	m_Vertices[2].z = 0.5f;
	m_Vertices[2].rhw = 1.0f;
	m_Vertices[2].color = 0xff0000ff;

	// Fill the indices
	m_Indices[0] = 1;
	m_Indices[1] = 2;
	m_Indices[2] = 0;

	return uResult;
}

// ----------------------------------------------------------------------------

bool CLostVertexBuffer::CreateResource
(
	LPVERTEXBUFFERS * ppVB,
	LPINDEXBUFFERS * ppIB,
	ULONG lDev
)
{
	//
	// Validate parameters.
	//

	// Check ppVB against NULL.
	assert( ppVB != NULL );
	if ( ppVB == NULL )
	{
		DPF( 1, _T("CLostVertexBuffer::CreateResource:  ppVB should not be NULL.\n") );
		return false;
	};


	// Check ppIB against NULL.
	assert( ppIB != NULL );
	if ( ppIB == NULL )
	{
		DPF( 1, _T("CLostVertexBuffer::CreateResource:  ppIB should not be NULL.\n") );
		return false;
	};


	// Verify that we're not about to leak a vertex buffer.
	assert( *ppVB == NULL );
	if ( *ppVB != NULL )
	{
		DPF( 1, _T("CLostVertexBuffer::CreateResource:  *ppVB should be NULL.\n") );
		return false;
	};


	// Verify that we're not about to leak an index buffer.
	assert( *ppIB == NULL );
	if ( *ppIB != NULL )
	{
		DPF( 1, _T("CLostVertexBuffer::CreateResource:  *ppIB should be NULL.\n") );
		return false;
	};


	//
	// Create the vertex buffer and index buffer.
	//

	__try
	{
		if(m_pD3D->GetVersion() < 8.0f)
		{
			m_dwUsageFlags = 0;//no vb flags are allowed on dx7 CreateVertexBuffer
			m_bUseIndexBuffer = false;//no index buffers before dx8
		};


		//
		// Create the vertex buffer.
		//
		// N.B.:
		// Here we are allocating enough memory to hold all our 3
		// custom vertices.  We also specify the FVF, so the vertex
		// buffer knows what data it contains.
		//

		if
		(
			!CreateVertexBuffer
			(
				3 * sizeof(CUSTOMVERTEX),
				m_dwUsageFlags, 
				D3DFVF_CUSTOMVERTEX,
				m_Pool,
				ppVB,
				lDev
			)
		)
		{
			DPF( 1, _T("CLostVertexBuffer::CreateResource:  CreateVertexBuffer failed.\n"));
			return false;
		};


		//
		// Create the index buffer if needed.
		//

		if ( m_bUseIndexBuffer )
		{
			if
			(
				!CreateIndexBuffer
				(
					3 * sizeof(WORD), 
					m_dwUsageFlags | D3DUSAGE_POINTS, 
					FMT_INDEX16,
					m_Pool,
					ppIB,
					lDev
				)
			)
			{
				DPF( 1, _T("CLostVertexBuffer::CreateResource:  CreateIndexBuffer failed.\n"));
				return false;
			};
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
		DPF( 1, _T("CLostVertexBuffer::CreateResource:  Exception caught.\n"));
		return false;
	};

	return true;
}


bool CLostVertexBuffer::CreateResource(ULONG lDev)
{
	if ( !CreateResource(&m_pVBA, &m_pIBA, lDev) )
	{
		DPF( 1, _T("CreateResource() - CreateResource A failed\n"));
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CLostVertexBuffer::CreateResourceB(ULONG lDev)
{
	if ( !CreateResource(&m_pVBB, &m_pIBB, lDev) )
	{
		DPF( 1, _T("CreateResourceB() - CreateResource B failed\n"));
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CLostVertexBuffer::LockData(ULONG lDev)
{
	__try
	{
		// Lock the vertex buffer.
		if( ! m_pVBA->Lock( 0, sizeof(m_Vertices), (BYTE**)&m_pVertices, m_dwLockFlags, lDev ) )
		{
			DPF( 1, _T("LockData() VB failed\n"));
			return false;
		}

		// If we're using the index buffer...
		if (m_bUseIndexBuffer)
		{
			// ...then lock it.
			if( ! m_pIBA->Lock( 0, 0, (BYTE**)&m_pIndices, m_dwLockFlags, lDev ) )
			{
				DPF( 1, _T("LockData() IB failed\n"));
				return false;
			}
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
		DPF( 1, _T("LockData() - Exception caught\n"));
		return false;
	}

   return true;
}

// ----------------------------------------------------------------------------

bool CLostVertexBuffer::FillAndUnlockData(ULONG lDev)
{
	__try
	{
		// Now we fill the vertex buffer. To do this, we need to Lock() the VB to
		// gain access to the vertices. This mechanism is required becuase vertex
		// buffers may be in device memory.
		if ( !memcpy( m_pVertices, m_Vertices, sizeof(m_Vertices) ) )
		{
			DPF( 1, _T("FillAndUnlockData() memcpy VB failed\n"));
			return false;
		}
		if (! m_pVBA->Unlock(lDev) )
		{
			DPF( 1, _T("FillAndUnlockData() VB Unlock failed\n"));
			return false;
		}

		//Fill and unlock the index buffer
		if (m_bUseIndexBuffer)
		{
			if ( !memcpy( m_pIndices, m_Indices, sizeof(m_Indices) ) )
			{
				DPF( 1, _T("FillAndUnlockData() memcpy IB failed\n"));
				return false;
			}
			if (! m_pIBA->Unlock(lDev) )
			{
				DPF( 1, _T("FillAndUnlockData() IB Unlock failed\n"));
				return false;
			}
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
		DPF( 1, _T("FillAndUnlockData() - Exception caught\n"));
		return false;
	}

	return true;
}


// ----------------------------------------------------------------------------

bool CLostVertexBuffer::CopyResAToResB(ULONG lDev)
{
	VOID *pVerticesA;
	VOID *pVerticesB;
	VOID *pIndicesA = NULL;
	VOID *pIndicesB = NULL;

	__try
	{
		//Lock the 2 Vertex Buffers
		if ( ! m_pVBA->Lock( 0, sizeof(m_Vertices), (BYTE**)&pVerticesA, 0, lDev ) )
		{
			DPF( 1, _T("CopyResAToResB() Lock VB A failed\n"));
			return false;
		}

		if ( ! m_pVBB->Lock( 0, sizeof(m_Vertices), (BYTE**)&pVerticesB, 0, lDev ) )
		{
			DPF( 1, _T("CopyResAToResB() Lock VB B failed\n"));
			return false;
		}

		//Lock the 2 Index Buffers
		if (m_bUseIndexBuffer)
		{
			if( ! m_pIBA->Lock( 0, 0, (BYTE**)&pIndicesA, 0, lDev ) )
			{
				DPF( 1, _T("CopyResAToResB() Lock IB A failed\n"));
				return false;
			}
			
			if( ! m_pIBB->Lock( 0, 0, (BYTE**)&pIndicesB, 0, lDev ) )
			{
				DPF( 1, _T("CopyResAToResB() Lock IB B failed\n"));
				return false;
			}
		}

		//Copy VB A to B
		if ( !memcpy( pVerticesB, pVerticesA, sizeof(m_Vertices) ) )
		{
			DPF( 1, _T("CopyResAToResB() memcpy VB failed\n"));
			return false;
		}

		//Copy IB A to B
		if ( m_bUseIndexBuffer )
		{
			if ( !memcpy( pIndicesB, pIndicesA, sizeof(m_Indices) ) )
			{
				DPF( 1, _T("CopyResAToResB() memcpy IB failed\n"));
				return false;
			}
		}

		//Unlock the 2 VB
		if (! m_pVBA->Unlock(lDev) )
		{
			DPF( 1, _T("CopyResAToResB() UnLock VB A failed\n"));
			return false;
		}

		if (! m_pVBB->Unlock(lDev) )
		{
			DPF( 1, _T("CopyResAToResB() UnLock VB B failed\n"));
			return false;
		}

		//Unlock the 2 IB
		if (m_bUseIndexBuffer)
		{
			if (! m_pIBA->Unlock(lDev) )
			{
				DPF( 1, _T("CopyResAToResB() UnLock IB A failed\n"));
				return false;
			}

			if (! m_pIBB->Unlock(lDev) )
			{
				DPF( 1, _T("CopyResAToResB() UnLock IB B failed\n"));
				return false;
			}
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
		DPF( 1, _T("CopyResAToResB() - Exception caught\n"));
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CLostVertexBuffer::DrawResource(LPVERTEXBUFFERS pVB, LPINDEXBUFFERS pIB, ULONG lDev)
{
	bool bRes = true;

	__try
	{
		//render the shape
		if (BeginScene(lDev))
		{
			if(m_pD3D->GetVersion() < 8.0f)
			{
				if (! DrawPrimitiveVertexBuffer(D3DPT_TRIANGLELIST, 
												pVB, 
												0, 
												3, 
												NULL, 
												0, 
												0, 
												0, 
												0, 
												false, 
												lDev))
				{
					DPF( 1, _T("DrawPrimitiveVertexBuffer failed\n"));
					bRes = false;
					goto Exit;
				}
			}
			else
			{

				if (! SetStreamSource( 0, pVB, sizeof(CUSTOMVERTEX), 0, lDev ) )
				{
					DPF( 1, _T("DrawResource() SetStreamSource() failed\n"));
					bRes = false;
					goto Exit;
				}
				if (! SetVertexShader( D3DFVF_CUSTOMVERTEX, lDev ) )
				{
					DPF( 1, _T("DrawResource() SetVertexShader() failed\n"));
					bRes = false;
					goto Exit;
				}

				if (m_bUseIndexBuffer)
				{
					if (! SetIndices(pIB, 0, lDev) )
					{
						DPF( 1, _T("DrawResource() SetIndices() failed\n"));
						bRes = false;
						goto Exit;
					}
					if (! DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 3, 0, 1, 0, lDev) )
					{
						DPF( 1, _T("DrawResource() DrawIndexedPrimitive() failed\n"));
						bRes = false;
						goto Exit;
					}
				}
				else
				{
					if (! DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1, lDev) )
					{
						DPF( 1, _T("DrawResource() DrawPrimitive() failed\n"));
						bRes = false;
						goto Exit;
					}
				}
			}

			if (! EndScene(lDev) )
			{
				DPF( 1, _T("DrawResource() EndScene() failed\n"));
				bRes = false;
				goto Exit;
			}
		}
		else
		{
			DPF( 1, _T("DrawResource() BeginScene() failed\n"));
			bRes = false;
			goto Exit;
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
		DPF( 1, _T("DrawResource() - Exception caught\n"));
		bRes = false;
		goto Exit;
	}

Exit:
	if (!bRes)
	{
		EndScene(lDev);
	}

	return bRes;
}

// ----------------------------------------------------------------------------

bool CLostVertexBuffer::DrawResource(ULONG lDev)
{
	if ( !DrawResource(m_pVBA, m_pIBA, lDev) )
	{
		DPF( 1, _T("DrawResource() A failed\n"));
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CLostVertexBuffer::DrawResourceB(ULONG lDev)
{
	if ( !DrawResource(m_pVBB, m_pIBB, lDev) )
	{
		DPF( 1, _T("DrawResource() B failed\n"));
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CLostVertexBuffer::ReleaseResource(ULONG lDev)
{
	bool bOverallResult = true;

	__try
	{
		RELEASE(m_pVBA);
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
		DPF( 1, _T("ReleaseResource() - Access violation caught while releasing vertex buffer A.\n") );
		bOverallResult = false;
	};

	__try
	{
		RELEASE(m_pVBB);
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
		DPF( 1, _T("ReleaseResource() - Access violation caught while releasing vertex buffer B.\n") );
		bOverallResult = false;
	};

	__try
	{
		RELEASE(m_pIBA);
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
		DPF( 1, _T("ReleaseResource() - Access violation caught while releasing index buffer A.\n") );
		bOverallResult = false;
	};

	__try
	{
		RELEASE(m_pIBB);
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
		DPF( 1, _T("ReleaseResource() - Access violation caught while releasing index buffer B.\n"));
		bOverallResult = false;
	};

	return bOverallResult;
}

void CLostVertexBuffer::DumpTestDetails(void)
{
	CLostDev::DumpTestDetails();

	bool bUsingIB = (m_rIndexBuffer->Value() == 1);

	DPF(1, "    Usage :    %s\n", m_szUsageFlags);
	DPF(1, "    Lock :     %s\n", m_szLockFlags);

	if (bUsingIB)
	{
		DPF(1, "    Using Index Buffer\n");
	}
}

bool CLostVertexBuffer::TestTerminate()
{
	//Need to sleep so that the os can recuperate from memleaks (or queing up of refresh calls).  
	Sleep(60000);

	return CLostDev::TestTerminate();
}