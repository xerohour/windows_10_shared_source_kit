// ============================================================================
//
//  LostStateBlock.cpp
//
// ============================================================================

#include "LostDev.h"

// ----------------------------------------------------------------------------
// *************************** CLostStateBlock ******************************
// ----------------------------------------------------------------------------

CLostStateBlock::CLostStateBlock()
{
	m_szTestName = _T("LostStateBlock");
	m_szCommandKey = _T("LostStateBlock");
	m_TestName = _T("Lost Devices with CreateStateBlock");

	m_bTestManaged = false;
	m_bTestSystemMem = false;
	m_bTestScratch = false;

    m_Options.D3DOptions.fMinDXVersion = 9.0f;
	m_Options.D3DOptions.fMaxDXVersion = 9.9f;
}

// ----------------------------------------------------------------------------

void CLostStateBlock::SetTestFactors()
{
	m_rCasesManager = m_rLostWays * 
							m_rPoolDefault;
}

// ----------------------------------------------------------------------------

bool CLostStateBlock::CreateResource(ULONG lDev)
{
	__try
	{
		// Create the image StateBlock
		if( !CreateStateBlock(D3DSBT_ALL, &m_dwStateBlock, lDev ) )
		{
			DPF( 1, _T("CreateResource() failed\n"));
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
		DPF( 1, _T("CreateResource() - Exception caught\n"));
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CLostStateBlock::CreateResourceB(ULONG lDev)
{
	return true;
}

// ----------------------------------------------------------------------------

bool CLostStateBlock::LockData(ULONG lDev)
{
	return true;
}

// ----------------------------------------------------------------------------

bool CLostStateBlock::FillAndUnlockData(ULONG lDev)
{
	return true;
}

// ----------------------------------------------------------------------------

bool CLostStateBlock::CopyResAToResB(ULONG lDev)
{
	return true;
}

// ----------------------------------------------------------------------------

bool CLostStateBlock::DrawResource(ULONG lDev)
{
	return true;
}
	
// ----------------------------------------------------------------------------

bool CLostStateBlock::DrawResourceB(ULONG lDev)
{
	return true;
}
	
// ----------------------------------------------------------------------------

bool CLostStateBlock::ReleaseResource(ULONG lDev)
{
	__try
	{
		DeleteStateBlock(m_dwStateBlock, lDev);
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
