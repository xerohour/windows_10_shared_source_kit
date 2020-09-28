// ============================================================================
//
//  LostDepthStencil.cpp
//
// ============================================================================

#include "LostDev.h"
#include "CSurface.h"

// ----------------------------------------------------------------------------
// ****************************	CLostDepthStencil *****************************
// ----------------------------------------------------------------------------


CLostDepthStencil::CLostDepthStencil()
:
	m_pDepth( NULL )
{
	m_szTestName = _T("LostDepthStencil");
	m_szCommandKey = _T("LostDepthStencil");
	m_TestName = _T("Lost Devices with CreateDepthStencilSurface");

	m_bTestManaged = false;
	m_bTestSystemMem = false;
}

// ----------------------------------------------------------------------------

void CLostDepthStencil::SetTestFactors()
{
	m_rCasesManager = m_rLostWays *
							m_rPoolDefault;
}

// ----------------------------------------------------------------------------

bool CLostDepthStencil::CapsCheck()
{
	if (! CLostDev::CapsCheck() )
	{
		WriteToLog
		(
			_T( "CapsCheck failed." )
		);

		return false;
	};

	const DWORD dwZBufferFormatListSize = 7;
	FMT ZBufferFormatList[ dwZBufferFormatListSize ] = 
	{
		FMT_D16_LOCKABLE,
		FMT_D16,
		FMT_D32,
		FMT_D15S1,
		FMT_D24S8,
		FMT_D24X8,
		FMT_D24X4S4,
	};

	MuteLog();

	// Look for a compatible depth format.
	m_DSFormat = FMT_ANY;
	for (int i = 0; i < dwZBufferFormatListSize; i++ )
	{
		if
		(
			m_pD3D->GetVersion() < 8.0f ||
			(
				SUCCEEDED
				(
					m_pD3D->CheckDeviceFormat
					(
						m_pSrcDevice->GetAdapterID(),
						m_pSrcDevice->GetDevType(),
						m_pCurrentMode->pDisplay->Format,
						USAGE_DEPTHSTENCIL,
						RTYPE_SURFACE,
						ZBufferFormatList[ i ]
					)
				) &&
				SUCCEEDED
				(
					m_pD3D->CheckDepthStencilMatch
					(
						m_pSrcDevice->GetAdapterID(),
						m_pSrcDevice->GetDevType(),
						m_pCurrentMode->pDisplay->Format,
						m_pCurrentMode->pDisplay->Format,
						ZBufferFormatList[ i ]
					)
				)
			)
		)
		{
			m_DSFormat = ZBufferFormatList[i];
			break;
		}
	}

	UnMuteLog();

	if (FMT_ANY == m_DSFormat)
	{
		WriteToLog
		(
			_T( "No matching depth stencil format found." )
		);

		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CLostDepthStencil::CreateResource(ULONG lDev)
{
	//
	// Valdiate members.
	//

	// Verify that we're not about to a depth stencil.
	assert( m_pDepth == NULL );
	if ( m_pDepth != NULL )
	{
		DPF( 1, _T("CLostDepthStencil::CreateResource:  m_pDepth should be NULL.\n") );
		return false;
	};


	//
	// Create the depth stencil surface.
	//

	__try
	{
		if
		(
			!CreateDepthStencilSurface
			(
				256,
				256,
				m_DSFormat,
				MULTISAMPLE_NONE,
				&m_pDepth,
				lDev
			) ||
			!m_pDepth
		)
		{
			DPF( 1, _T("CLostDepthStencil::CreateResource:  CreateDepthStencilSurface failed.\n") );
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
		DPF( 1, _T("CLostDepthStencil::CreateResource:  Exception caught from CreateDepthStencilSurface.\n"));
		return false;
	};

	return true;
}

// ----------------------------------------------------------------------------

bool CLostDepthStencil::DrawResource(ULONG lDev)
{
	return true;
}

// ----------------------------------------------------------------------------

bool CLostDepthStencil::LockData(ULONG lDev)
{
	return true;
}

// ----------------------------------------------------------------------------

bool CLostDepthStencil::FillAndUnlockData(ULONG lDev)
{
	return true;
}

// ----------------------------------------------------------------------------

bool CLostDepthStencil::ReleaseResource(ULONG lDev)
{
	__try
	{
		RELEASE(m_pDepth);
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

