// ============================================================================
//
//  LostImageSurface.cpp
//
// ============================================================================

#include "LostDev.h"
#include "CSurface.h"

// ----------------------------------------------------------------------------
// *************************** CLostImageSurface ******************************
// ----------------------------------------------------------------------------

CLostImageSurface::CLostImageSurface()
{
	m_szTestName = _T("LostImageSurface");
	m_szCommandKey = _T("LostImageSurface");
	m_TestName = _T("Lost Devices with CreateImageSurface");

	m_bTestManaged = false;
	m_bTestDefault = false;

	m_Options.D3DOptions.fMinDXVersion = 6.0f;
	m_Options.D3DOptions.fMaxDXVersion = 8.9f;
}

// ----------------------------------------------------------------------------

void CLostImageSurface::SetTestFactors()
{
	//This factor is used to test the copy methods
	if(	!CHECKREF(m_rCopyFromAToB = NewFactor( 2 )) ||
		!CHECKREF(m_rSubRectLock = NewFactor( 2 )) )
	{
		DPF(0, "ERROR setting the cases in LostImageSurface()");
	}

	m_rCasesManager = m_rLostWays *
						  (	(m_rPoolSystemMem2Res * m_rSubRectLock * m_rCopyFromAToB) + 
							(m_rPoolSystemMem1Res * m_rSubRectLock)
						  );
}

// ----------------------------------------------------------------------------

bool CLostImageSurface::CapsCheck()
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

bool CLostImageSurface::CreateResource
(
	LPSURFACES & pSurfaces,
	LPSURFACE & pSurf,
	ULONG lDev
)
{
	//
	// Validate parameters.
	//

	// Verify that we're not about to leak a texture.
	assert( pSurfaces == NULL );
	if ( pSurfaces != NULL )
	{
		DPF( 1, _T("CLostImageSurface::CreateResource:  pSurfaces should be NULL.\n") );
		return false;
	};


	// Verify that we're not about to leak a surface.
	assert( pSurf == NULL );
	if ( pSurf != NULL )
	{
		DPF( 1, _T("CLostImageSurface::CreateResource:  pSurf should be NULL.\n") );
		return false;
	};


	//
	// Create the image surface.
	//

	__try
	{
		if
		(
			!CreateImageSurface
			(
				256,
				256,
				m_PresParams.BackBufferFormat/*m_TexFormat*/,
				&pSurfaces,
				lDev
			) ||
			!pSurfaces
		)
		{
			DPF( 1, _T("CLostImageSurface::CreateResource:  CreateImageSurface failed.\n"));
			return false;
		};


		if ( !pSurfaces->GetSurface( &pSurf, lDev - 1 ) ) 
		{
			DPF( 1, _T("CLostImageSurface::CreateResource:  GetSurface failed.\n"));
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
		DPF( 1, _T("CLostImageSurface::CreateResource:  Exception caught from CreateImageSurface or GetSurface.\n"));
		return false;
	};

	return true;
}
