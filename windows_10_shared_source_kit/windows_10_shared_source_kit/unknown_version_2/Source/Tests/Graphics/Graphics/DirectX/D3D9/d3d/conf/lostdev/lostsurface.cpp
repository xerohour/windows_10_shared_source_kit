// ============================================================================
//
//  LostSurface.cpp
//
// ============================================================================

#include "LostDev.h"
#include "CSurface.h"

// ----------------------------------------------------------------------------
// *************************** CLostSurface ******************************
// ----------------------------------------------------------------------------

CLostSurface::CLostSurface()
:
	m_pSurfacesA( NULL ),
	m_pSurfacesB( NULL ),
	m_pSurfA( NULL ),
	m_pSurfB( NULL )
{
};


void CLostSurface::SetFlags()
{
	if (m_pD3D->GetVersion() < 9.0f)
	{
		m_bUseAToBCopyRects = m_rCopyFromAToB->Value() == 1;
	}
	else
	{
		m_bUseAToBCopyRects = false;
	}
	m_bUseSubRectLock = m_rSubRectLock->Value() == 1;
}

// ----------------------------------------------------------------------------

bool CLostSurface::CapsCheck()
{
	if (! CLostDev::CapsCheck() )
		return false;

/*	m_TexFormat = m_PresParams.BackBufferFormat;//GetValidFormat(RTYPE_SURFACE);

	if (m_TexFormat == FMT_ANY)
	{
		DPF(1, "No CubeTexture format supported\n");
		return false;
	}

*/	return true;
}

// ----------------------------------------------------------------------------

bool CLostSurface::CreateResource(ULONG lDev)
{
	//
	// Create the surface.
	//

	if
	(
		!CreateResource
		(
			m_pSurfacesA,
			m_pSurfA,
			lDev
		)
	)
	{
		DPF( 1, _T("CLostSurface::CreateResource:  CreateResource on surface A failed.\n"));
		return false;
	};

	return true;
}

// ----------------------------------------------------------------------------

bool CLostSurface::CreateResourceB(ULONG lDev)
{
	//
	// Create the surface.
	//

	if
	(
		!CreateResource
		(
			m_pSurfacesB,
			m_pSurfB,
			lDev
		)
	)
	{
		DPF( 1, _T("CLostSurface::CreateResource:  CreateResource on surface B failed.\n"));
		return false;
	};

	return true;
}

// ----------------------------------------------------------------------------

bool CLostSurface::LockData(ULONG lDev)
{
	RECT* pRect = NULL;
	RECT rect;
	SURFACEDESC	Desc;

	__try
	{
		if (m_bUseSubRectLock)
		{
			if ( FAILED( m_pSurfA->GetDesc(&Desc) ) )
			{
				DPF( 1, _T("LockData() GetDesc failed\n"));
				return false;
			}

			rect.top = Desc.dwHeight - (Desc.dwHeight / 4);
			rect.left = Desc.dwWidth - (Desc.dwWidth / 4);
			rect.bottom = Desc.dwHeight - 1;
			rect.right = Desc.dwWidth - 1;
			pRect = &rect;
		}

		if (FAILED( m_pSurfA->LockRect(&m_LockedRect, pRect, (m_bLockReadOnly ? D3DLOCK_READONLY : 0)) ) )
		{
			DPF( 1, _T("LockData() LockRect failed\n"));
			return false;
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

bool CLostSurface::FillAndUnlockData(ULONG lDev)
{
	PBYTE ptr;
	SURFACEDESC	Desc;

	__try
	{
		if ( FAILED ( m_pSurfA->GetDesc(&Desc) ) )
		{
			DPF( 1, _T("FillAndUnLockData() GetDesc failed\n"));
			return false;
		}

		if ( ! FillSurface(&Desc, &m_LockedRect, m_bUseSubRectLock) )
		{
			DPF( 1, _T("FillAndUnLockData() FillSurface failed\n"));
			return false;
		}

		if ( FAILED( m_pSurfA->UnlockRect() ) )
		{
			DPF( 1, _T("LockData() UnlockRect failed\n"));
			return false;
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

bool CLostSurface::CopyResAToResB(ULONG lDev)
{
	__try
	{
		if ( ! m_bUseAToBCopyRects )
		{
			LOCKED_RECT lockRectA;
			LOCKED_RECT lockRectB;

			SURFACEDESC	Desc;

			if ( FAILED( m_pSurfA->GetDesc(&Desc) ) )
			{
				DPF( 1, _T("CopyResAToResB() GetLevelDesc failed\n"));
				return false;
			}

			//Lock the 2 surfaces
			if ( FAILED( m_pSurfA->LockRect(&lockRectA, NULL, (m_bLockReadOnly ? D3DLOCK_READONLY : 0)) ) )
			{
				DPF( 1, _T("CopyResAToResB() LockRect A failed\n"));
				return false;
			}

			if ( FAILED( m_pSurfB->LockRect(&lockRectB, NULL, (m_bLockReadOnly ? D3DLOCK_READONLY : 0)) ) )
			{
				DPF( 1, _T("CopyResAToResB() LockRect B failed\n"));
				return false;
			}

			//Copy A to B
			if ( !CopyFromAToB(&lockRectA, &lockRectB, &Desc) )
			{
				DPF( 1, _T("CopyResAToResB() memcpy failed\n"));
				return false;
			}

			//Unlock the 2 surfaces
			if ( FAILED(m_pSurfA->UnlockRect() ) )
			{
				DPF( 1, _T("CopyResAToResB() UnLockRect A failed\n"));
				return false;
			}

			if ( FAILED(m_pSurfB->UnlockRect() ) )
			{
				DPF( 1, _T("CopyResAToResB() UnLockRect B failed\n"));
				return false;
			}

			return true;
		}
		else
		{
			if (! CopyRects(m_pSurfacesA, NULL, 0, m_pSurfacesB, NULL, lDev) )
			{
				DPF( 1, _T("CopyResAToResB() CopyRects Failed\n"));
				return false;
			}

			return true;
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
}

// ----------------------------------------------------------------------------

bool CLostSurface::DrawResource(LPSURFACES pSurfaces, LPSURFACE pSurf, ULONG lDev)
{
	bool bRes = true;
	LPSURFACES pBackSurf = NULL;
	LPSURFACES pTempSurf = NULL;
	LPSURFACE  pSurfScratch = NULL;
	LPSURFACE  pSurfSysmem = NULL;

	__try
	{
		if( ! GetBackBuffer(0, BACKBUFFER_TYPE_MONO  , &pBackSurf, lDev) )
		{
			DPF( 1, _T("DrawResource() GetBackBuffer failed\n"));
			bRes = false;
			goto Error;
		}

		// Copy the Surface to the back buffer
		if (m_pD3D->GetVersion() < 9.0f)
		{
			RECT rect = {0,0,256,256};
			POINT point = {0,0};
			if ( ! CopyRects(pSurfaces, &rect, 1, pBackSurf, &point, lDev) )
			{
				DPF( 1, _T("DrawResource() CopyRects failed\n"));
				bRes = false;
				goto Error;
			}
		}
		else
		{
			if ( m_Pool == D3DPOOL_SCRATCH )
			{
				//Create the temp sysmem Texture
				if(!CreateOffscreenPlainSurface(256, 256, m_PresParams.BackBufferFormat/*m_TexFormat*/, D3DPOOL_SYSTEMMEM, &pTempSurf, lDev) ||
					pTempSurf == NULL)
				{
					DPF( 1, _T("DrawResource() CreateOffscreenPlainSurface failed\n"));
					bRes = false;
					goto Error;
				}

				pSurfaces->GetSurface(&pSurfScratch, lDev-1);
				pTempSurf->GetSurface(&pSurfSysmem, lDev-1);

				//Copy the scratch texture to the temp Sysmem
				LOCKED_RECT lockScratch, lockSysmem;
				SURFACEDESC desc;
				//TODO: Copy all levels
				if (FAILED(pSurfScratch->LockRect(&lockScratch, NULL, 0)))
				{
					DPF( 1, _T("DrawResource() pSurfaces->LockRect failed\n"));
					bRes = false;
					goto Error;
				}
				if (FAILED(pSurfSysmem->LockRect(&lockSysmem, NULL, 0)))
				{
					DPF( 1, _T("DrawResource() pTempSurf->LockRect failed\n"));
					bRes = false;
					goto Error;
				}
				if (FAILED(pSurfSysmem->GetDesc(&desc)))
				{
					DPF( 1, _T("DrawResource() pTempSurf->GetLevelDesc failed\n"));
					bRes = false;
					goto Error;
				}

				CopyFromAToB(&lockScratch, &lockSysmem, &desc);

				if (FAILED(pSurfScratch->UnlockRect()))
				{
					DPF( 1, _T("DrawResource() pSurfaces->UnlockRect failed\n"));
					bRes = false;
					goto Error;
				}
				if (FAILED(pSurfSysmem->UnlockRect()))
				{
					DPF( 1, _T("DrawResource() pTempSurf->UnlockRect failed\n"));
					bRes = false;
					goto Error;
				}
			}
			else
			{
				pTempSurf = pSurfaces;
			}

			if ( ! UpdateSurface(pTempSurf, NULL, pBackSurf, NULL, lDev) )
			{
				DPF( 1, _T("DrawResource() UpdateSurface failed\n"));
				bRes = false;
				goto Error;
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
		DPF( 1, _T("DrawResource() - Exception caught\n"));
		return false;
	}

Error:
	RELEASE(pBackSurf);

	if ( m_Pool == D3DPOOL_SCRATCH )
	{
		RELEASE(pSurfScratch);
		RELEASE(pSurfSysmem);
		RELEASE(pTempSurf);
	}

	return bRes;
}

// ----------------------------------------------------------------------------

bool CLostSurface::DrawResource(ULONG lDev)
{
	if (! DrawResource(m_pSurfacesA, m_pSurfA, lDev) )
	{
		DPF( 1, _T("DrawResource() A failed\n"));
		return false;
	}

	return true;
}
	
// ----------------------------------------------------------------------------

bool CLostSurface::DrawResourceB(ULONG lDev)
{
	if (! DrawResource(m_pSurfacesB, m_pSurfB, lDev) )
	{
		DPF( 1, _T("DrawResource() B failed\n"));
		return false;
	}

	return true;
}
	
// ----------------------------------------------------------------------------

bool CLostSurface::ReleaseResource(ULONG lDev)
{
	bool bOverallResult = true;

	__try
	{
		RELEASE(m_pSurfA);
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
		DPF( 1, _T("ReleaseResource() - Access violation caught while releasing surface A.\n") );
		bOverallResult = false;
	};


	__try
	{
		RELEASE(m_pSurfacesA);
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
		DPF( 1, _T("ReleaseResource() - Access violation caught while releasing surface collection A.\n") );
		bOverallResult = false;
	};

	__try
	{
		RELEASE(m_pSurfB);
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
		DPF( 1, _T( "ReleaseResource() - Access violation caught while releasing surface B.\n") );
		bOverallResult = false;
	};

	__try
	{
		RELEASE(m_pSurfacesB);
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
		DPF( 1, _T("ReleaseResource() - Access violation caught while releasing surface collection B.\n"));
		bOverallResult = false;
	};

	return bOverallResult;
};


void CLostSurface::DumpTestDetails(void)
{
	CLostDev::DumpTestDetails();

	if (m_bUseSubRectLock)
	{
		DPF(1, "    Using SubRect Lock\n");
	}

	//On DX9, we always use memcpy.
	if (m_pD3D->GetVersion() < 9.0f)
	{
		if (m_bUseAToBCopyRects)
		{
			DPF(1, "    A to B using CopyRects\n");
		}
		else
		{
			DPF(1, "    A to B using Memcpy\n");
		}
	}
}

