#include "TestApp.h"
#include "PresentWinCore.h"

#pragma warning(push, 4)

//****************************************************************************
// CPresentWinConvertStretchCore::InitTestParameters
//
// Calls into potentially overloaded InitializeTestRFactors fucntion for test group specific parameters.
//
//****************************************************************************
void CPresentWinConvertStretchCore::InitTestParametersImpl()
{
    // Swapchain Back Buffer Dimension factors
    CUserValueSet<int> *pBBDimensions = new CUserValueSet<int>();
    pBBDimensions->AddValue(0);
    RFactor rBBDimension_no_stretch = AddParameter<int>( _T("BBDimensions"), &m_iBBDimIndex, pBBDimensions );

    if( m_bStretchBB )
    {
        pBBDimensions->AddValue( 1 );   // 1.25f x 1.45f  (Back buffer dimension test factors are expressed as a relative value to the presentation window size.)
        pBBDimensions->AddValue( 3 );   // 2.0f x 1.0f
        pBBDimensions->AddValue( 4 );   // 1.0f x 2.0f
    }
    RFactor rBBDimension_stretch_larger_only = AddParameterValueSet<int>( _T("BBDimensions"), pBBDimensions );
    if( m_bStretchBB )
    {
        pBBDimensions->AddValue( 2 );   // 0.85f x 0.75f
        pBBDimensions->AddValue( 5 );   // 0.5f x 1.0f
        pBBDimensions->AddValue( 6 );   // 1.0f x 0.5f
    }
    RFactor rBBDimension_stretch = AddParameterValueSet<int>( _T("BBDimensions"), pBBDimensions );

    //
    // Window size parameters
    //
    CTestCaseParameter<int> *pWindowSizeParameter = AddParameter<int>( _T("Window"), &m_iWindow);
    RFactor rWindowSize_default = AddParameterValue<int>( pWindowSizeParameter, 0 );
    RFactor rWindowSize_less_than_page = AddParameterValue<int>( pWindowSizeParameter, 1 );
    RFactor rWindowSize_one_by_one = AddParameterValue<int>( pWindowSizeParameter, 2 );

    RFactor bbDimensions = rBBDimension_stretch * rWindowSize_default + rWindowSize_less_than_page * rBBDimension_stretch_larger_only  + rWindowSize_one_by_one * rBBDimension_no_stretch ;

    //
    // Need to maintain seperate lists for blt and flip model swap effects.
    // Blt model does not support scrolling, dirty rects, nor DCOMP.
    //
    // Blt model Swap effect factors
    CUserValueSet< DXGISwapEffectRecord > *pSwapEffectBlt = new CUserValueSet< DXGISwapEffectRecord >();
    for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
    {
        if (!IsFlipModel(DXGISwapEffects[i].dxgiSwapEffect))
            pSwapEffectBlt->AddValue(DXGISwapEffects[i]);
    }
    RFactor SwapEffectBltFactors = AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pSwapEffectBlt);

    // Flip model Swap effect factors
    CUserValueSet< DXGISwapEffectRecord > *pSwapEffectFlip = new CUserValueSet< DXGISwapEffectRecord >();
    for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
    {
        if (IsFlipModel(DXGISwapEffects[i].dxgiSwapEffect))
            pSwapEffectFlip->AddValue(DXGISwapEffects[i]);
    }
    RFactor SwapEffectFlipFactors = AddParameterValueSet< DXGISwapEffectRecord >( _T("SwapEffect"), pSwapEffectFlip);

    RFactor SwapEffectsCombined = SwapEffectFlipFactors + SwapEffectBltFactors;

    //
    // We need separate Test factors for choosing between DCOMP (Windowless) and regular Windowed Presents.
    // DCOMP is only supported for FlipModel when DWM is on.
    //
    // (Note that the name "windowless" is somewhat misleading since the visuals themselves are still contained with-in a Window.)
    //

    // Use DCOMP
    CUserValueSet< bool > *pUseDCOMP = new CUserValueSet< bool >();
    pUseDCOMP->AddValue(true);
    RFactor UseDCOMPFactor = AddParameter< bool >( _T("UseDCOMP") , &m_tp_bWindowless, pUseDCOMP);

    // Don't use DCOMP (use regular Windowed Presents)
    CUserValueSet< bool > *pDontUseDCOMP = new CUserValueSet< bool >();
    pDontUseDCOMP->AddValue(false);
    RFactor DontUseDCOMPFactor = AddParameterValueSet< bool >( _T("UseDCOMP"), pDontUseDCOMP);

    // DWM on/off Factors
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
    // the order of test cases are preserved.
    //
    // DWM Off - DCOMP Not Supported
    RFactor Bpp16_DwmOff = m_FeatureLevelFactor * m_globalRFactorDwmOffBpp * v_dwmCompositionOff * DontUseDCOMPFactor * SwapEffectsCombined *  bbDimensions;
    // DWM ON - DCOMP Not Supported on Win7+8IP and below
    RFactor Bpp32_DwmOn_NoDComp = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * v_dwmCompositionOn * DontUseDCOMPFactor*SwapEffectsCombined *  bbDimensions;
    // DWM ON - DCOMP Supported, but only for FlipModel.  for Regular Windowed Presents test both blt and Flip model.
    RFactor Bpp32_DwmOn = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * v_dwmCompositionOn * (DontUseDCOMPFactor*SwapEffectsCombined + UseDCOMPFactor*SwapEffectFlipFactors) *  bbDimensions;
    // DWM Off - DCOMP Not Supported
    RFactor Bpp32_DwmOff = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * dwmCompositionOff_32bpp * DontUseDCOMPFactor * SwapEffectsCombined *  bbDimensions;

    // Only test dwm off cases on Win7+8IP and below
    OSVERSIONINFO versionInfo;
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
    GetVersionEx(&versionInfo);
    if(versionInfo.dwMajorVersion < 6 || (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion < 2) )
    {
        // Check which set of enumerated mode set parameters are valid
        if( Bpp16_DwmOff.Valid() && Bpp32_DwmOn_NoDComp.Valid())
            SetRootTestFactor( Bpp32_DwmOn_NoDComp + Bpp32_DwmOff + Bpp16_DwmOff );
        else if( !Bpp16_DwmOff.Valid() && Bpp32_DwmOn_NoDComp.Valid())
            SetRootTestFactor(Bpp32_DwmOn_NoDComp + Bpp32_DwmOff);
        else if( Bpp16_DwmOff.Valid() && !Bpp32_DwmOn_NoDComp.Valid())
            SetRootTestFactor( Bpp16_DwmOff );
        else
        {
            SetRootTestFactor( m_FeatureLevelFactor );
            m_ParameterError = static_cast<unsigned>(-1);
        }
        AddPriorityPatternFilter(FilterEqual< bool >(_T("DWM"), false ), WeightFactorVerySmall);
    }
    else
    {
        if( Bpp32_DwmOn.Valid())
            SetRootTestFactor( Bpp32_DwmOn );
    }

    AddPriorityPatternFilter(FilterLess< int >(_T("BBDimensions"), 3 ), WeightFactorLarge);
    AddPriorityPatternFilter(( FilterEqual< DXGISwapEffectRecord >(_T("SwapEffect"), DXGISwapEffects[1] ) && FilterEqual< int >(_T("BBDimensions"), 0 )), WeightFactorSmall);

    DeclarePriorityLevelByPriorityValue(1, 5.0f);
	DeclarePriorityLevelByPriorityValue(2, 2.0f);
}

#pragma warning(pop)