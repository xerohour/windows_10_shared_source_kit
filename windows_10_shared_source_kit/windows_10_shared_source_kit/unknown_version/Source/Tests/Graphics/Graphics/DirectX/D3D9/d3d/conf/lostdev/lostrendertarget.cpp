// ============================================================================
//
//  LostRenderTarget.cpp
//
// ============================================================================

#include "LostDev.h"
#include "CSurface.h"

// ----------------------------------------------------------------------------
// ****************************	CLostRenderTarget *****************************
// ----------------------------------------------------------------------------


CLostRenderTarget::CLostRenderTarget()
:
	m_pRender( NULL )
{
	m_szTestName = _T("LostRenderTarget");
	m_szCommandKey = _T("LostRenderTarget");
	m_TestName = _T("Lost Devices with CreateRenderTarget");

	m_Options.D3DOptions.fMinDXVersion = 6.0f;

	m_bTestManaged = false;
	m_bTestSystemMem = false;
}

// ----------------------------------------------------------------------------

void CLostRenderTarget::SetTestFactors()
{
	m_rCasesManager = m_rLostWays *
							m_rPoolDefault;
}

// ----------------------------------------------------------------------------

bool CLostRenderTarget::CapsCheck()
{
	return CLostDev::CapsCheck();
}

// ----------------------------------------------------------------------------

bool CLostRenderTarget::CreateResource(ULONG lDev)
{
	//
	// Validate members.
	//

	// Verify that we're not about to leak a render target.
	assert( m_pRender == NULL );
	if ( m_pRender != NULL )
	{
		DPF( 1, _T("CLostRenderTarget::CreateResource:  m_pRender should be NULL.\n") );
		return false;
	};


	//
	// Create the render target.
	//

	__try
	{
		if
		(
			!CreateRenderTarget
			(
				256,
				256,
				m_PresParams.BackBufferFormat,
				MULTISAMPLE_NONE,
				true,
				&m_pRender,
				lDev
			) ||
			!m_pRender
		)
		{
			DPF(1, _T("CLostRenderTarget::CreateResource:  CreateRenderTarget failed.\n"));
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
		DPF( 1, _T("CLostRenderTarget::CreateResource:  Exception caught from CreateRenderTarget.\n"));
		return false;
	};

	return true;
}

// ----------------------------------------------------------------------------

bool CLostRenderTarget::DrawResource(ULONG lDev)
{
	return true;
}

// ----------------------------------------------------------------------------

bool CLostRenderTarget::LockData(ULONG lDev)
{
	return true;
}

// ----------------------------------------------------------------------------

bool CLostRenderTarget::FillAndUnlockData(ULONG lDev)
{
	return true;
}

// ----------------------------------------------------------------------------

bool CLostRenderTarget::ReleaseResource(ULONG lDev)
{
	__try
	{
		RELEASE(m_pRender);
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
		DPF( 1, _T("ReleaseResource() - Exception caught\n"));
		return false;
	}

	return true;
}

