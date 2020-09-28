//
// Project headers
//

#include "DXGIPresentFlags.h"


#pragma warning(disable: 4355)


CWinDiscard::CWinDiscard()
{
	m_bStretchBB = false;
	m_NumberOfPresents = 1;
	m_TexturesToLoad = m_NumberOfPresents;
	m_MaxBufferCount = 3;
};


CWinDiscard::~CWinDiscard()
{
};

//****************************************************************************
// CWinDiscard::InitTestParameters
//
//****************************************************************************
void CWinDiscard::InitTestParametersImpl()
{
	RFactor bufferCount = AddParameter
		(
		_T("BufferCount"),
		&m_BufferCount,
		RangeValueSet
		(
		(UINT)1,
		(UINT)m_MaxBufferCount,
		(UINT)1
		)
		);

	CUserValueSet< DXGIPresentFlagRecord > *pPresentFlags = new CUserValueSet< DXGIPresentFlagRecord >();
	for( UINT i = 1; i < cuiNoOfPresentFlags; i++ )
	{
		pPresentFlags->AddValue(DXGIPresentFlags[i]);
	}

	RFactor presentFlags = AddParameter< DXGIPresentFlagRecord >( _T("PresentFlags") , &m_PresentFlag, pPresentFlags);

	CUserValueSet< DXGISwapEffectRecord > *pSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();
	for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
	{
		if(DXGISwapEffects[i].dxgiSwapEffect == DXGI_SWAP_EFFECT_DISCARD)
		{
			pSwapEffect->AddValue(DXGISwapEffects[i]);
		}
	}

	RFactor swapEffect = AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pSwapEffect);

	//
	// Don't test DCOMP (use regular Windowed Presents only).  This test
	// explores DISCARD swap-effects which are Blt-Model only.   DCOMP doesn't
	// support Blt-Model.
	//
	RFactor DontUseDCOMPFactor = AddParameter< bool >( _T("UseDCOMP"), &m_tp_bWindowless, false );

	CUserValueSet< bool > *pDwmCompositionOn = new CUserValueSet< bool >();
	if ( g_Application.IsTerminalServices() == true ||
		GetSystemMetrics( SM_REMOTESESSION) != 0)
	{
		//
		// Dont add the toggle for DWM on and off in the remote case.
		//
	}
	else
	{
		pDwmCompositionOn->AddValue( false );
	}

	// Invlid dwm factor when in the remote case
	RFactor dwmCompositionOff_32bpp = AddParameter< bool >( _T("DWM"), &m_bDWM, pDwmCompositionOn);
	// Always valid dwm factors
	RFactor v_dwmCompositionOn = AddParameterValue< bool >( _T("DWM"), true);
	RFactor v_dwmCompositionOff = AddParameterValue< bool >( _T("DWM"), false);

	// Separate out Bpp32_DwmOn and DwmOff when adding test parameters together so that
	// the order of test cases are preserved (
	RFactor Bpp16_DwmOff = m_FeatureLevelFactor * m_globalRFactorDwmOffBpp * v_dwmCompositionOff     * DontUseDCOMPFactor * bufferCount * presentFlags * swapEffect ;
	RFactor Bpp32_DwmOn = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp  * v_dwmCompositionOn      * DontUseDCOMPFactor * bufferCount * presentFlags * swapEffect ;
	RFactor Bpp32_DwmOff = m_FeatureLevelFactor * m_globalRFactorDwmOnBpp  * dwmCompositionOff_32bpp * DontUseDCOMPFactor * bufferCount * presentFlags * swapEffect ;

	// Only test dwm off cases on Win7+8IP and below
	OSVERSIONINFO versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	GetVersionEx(&versionInfo);
	if(versionInfo.dwMajorVersion < 6 || (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion < 2) )
	{
		// Check which set of enumerated mode set parameters are valid
		if( Bpp16_DwmOff.Valid() && Bpp32_DwmOn.Valid())
			SetRootTestFactor( Bpp32_DwmOn + Bpp32_DwmOff + Bpp16_DwmOff );
		else if( !Bpp16_DwmOff.Valid() && Bpp32_DwmOn.Valid())
			SetRootTestFactor(Bpp32_DwmOn + Bpp32_DwmOff);
		else if( Bpp16_DwmOff.Valid() && !Bpp32_DwmOn.Valid())
			SetRootTestFactor( Bpp16_DwmOff );
		else
		{
			SetRootTestFactor( m_FeatureLevelFactor );
			m_ParameterError = -1;
		}
		AddPriorityPatternFilter(FilterEqual< bool >(_T("DWM"), false ), WeightFactorVerySmall);
	}
	else
	{
		if( Bpp32_DwmOn.Valid())
			SetRootTestFactor( Bpp32_DwmOn );
	}

	DeclarePriorityLevelByPriorityValue(1, 5.0f);
	DeclarePriorityLevelByPriorityValue(2, 2.0f);

}

