// ============================================================================
//
//  LostOffscreenPlainSurface.cpp
//
// ============================================================================

#include "LostDev.h"
#include "CSurface.h"

// ----------------------------------------------------------------------------
// *************************** CLostOffscreenPlainSurface ******************************
// ----------------------------------------------------------------------------

CLostOffscreenPlainSurface::CLostOffscreenPlainSurface()
{
	m_szTestName = _T("LostOffscreenPlainSurface");
	m_szCommandKey = _T("LostOffscreenPlainSurface");
	m_TestName = _T("Lost Devices with CreateOffscreenPlainSurface");

	m_bTestManaged = false;

	m_Options.D3DOptions.fMinDXVersion = 9.0f;
	m_Options.D3DOptions.fMaxDXVersion = 9.9f;
}

// ----------------------------------------------------------------------------

void CLostOffscreenPlainSurface::SetTestFactors()
{
	//This factor is used to test the copy methods
	if(	!CHECKREF(m_rSubRectLock = NewFactor( 2 )) )
	{
		DPF(0, "ERROR setting the cases in LostOffscreenPlainSurface()");
	}

	m_rCasesManager = m_rLostWays *
						  (	(m_rPoolSystemMem2Res * m_rSubRectLock) + 
							(m_rPoolSystemMem1Res * m_rSubRectLock) +
					  		(m_rPoolScratch2Res * m_rSubRectLock) + 
							(m_rPoolScratch1Res * m_rSubRectLock) +
							(m_rPoolDefault * m_rSubRectLock)
						  );
}

// ----------------------------------------------------------------------------

bool CLostOffscreenPlainSurface::CapsCheck()
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

bool CLostOffscreenPlainSurface::CreateResource
(
	LPSURFACES & pSurfaces,
	LPSURFACE & pSurf,
	ULONG lDev
)
{
	//
	// Validate parameters.
	//

	// Verify that we're not about to leak a surface.
	assert( pSurfaces == NULL );
	if ( pSurfaces != NULL )
	{
		DPF( 1, _T("CLostOffscreenPlainSurface::CreateResource:  pSurfaces should be NULL.\n") );
		return false;
	};

	// Verify that we're not about to leak a surface.
	assert( pSurf == NULL );
	if ( pSurf != NULL )
	{
		DPF( 1, _T("CLostOffscreenPlainSurface::CreateResource:  pSurf should be NULL.\n") );
		return false;
	};


	//
	// Create the image surface.
	//

	__try
	{
		if
		(
			!CreateOffscreenPlainSurface
			(
				256,
				256,
				m_PresParams.BackBufferFormat/*m_TexFormat*/,
				m_Pool,
				&pSurfaces,
				lDev
			) ||
			!pSurfaces
		)
		{
			DPF( 1, _T("CLostOffscreenPlainSurface::CreateResource:  CreateOffscreenPlainSurface failed.\n"));
			return false;
		};


		if( !pSurfaces->GetSurface( &pSurf, lDev -1 ) ) 
		{
			DPF( 1, _T("CLostOffscreenPlainSurface::CreateResource:  GetSurface failed.\n"));
			return false;
		};
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
		DPF( 1, _T("CLostOffscreenPlainSurface::CreateResource:  Exception caught from CreateOffscreenPlainSurface or GetSurface.\n"));
		return false;
	};

	return true;
}
