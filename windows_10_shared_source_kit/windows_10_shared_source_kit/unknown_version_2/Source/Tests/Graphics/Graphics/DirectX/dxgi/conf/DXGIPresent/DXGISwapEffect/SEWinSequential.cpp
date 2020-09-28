//
// Project headers
//

#include "DXGISwapEffect.h"
#include "TestApp.h"

#pragma warning(disable: 4355)

CSEWinSequential::CSEWinSequential()
    :
m_ValidateNoExtraBuffers(false)
{
    m_bStretchBB = false;
    m_MaxBufferCount = 3;
};


CSEWinSequential::~CSEWinSequential()
{
};

TEST_RESULT CSEWinSequential::ExecuteTestCase()
{

    m_NumberOfPresents = m_BufferCount;

    //
    // Present one more time to make sure the first buffer
    // and not an extra buffer is  presented
    //
    if(m_ValidateNoExtraBuffers)
    {
        m_NumberOfPresents += 1;
    }

    m_TexturesToLoad = m_BufferCount;

    return CPresentWinConvertStretch::ExecuteTestCase();
};

//****************************************************************************
// CSEWinSequential::InitTestParameters
//
// Replaces the CPresentWinConvertStretch base class InitTestParameters with
// this version.
//****************************************************************************
void CSEWinSequential::InitTestParametersImpl()
{
    CUserValueSet< bool > *pValidateNoExtraBuffers = new CUserValueSet< bool >();
    pValidateNoExtraBuffers->AddValue( true );
    pValidateNoExtraBuffers->AddValue( false );

    RFactor validateNoExtraBuffers = AddParameter< bool >
        (
        _T("ValidateNoExtraBuffers"),
        &m_ValidateNoExtraBuffers,
        pValidateNoExtraBuffers
        );

    //
    // We'll need to create different test factors for Blt vs. Flip Model.   Flip-Model does not support the 1 buffer swapchain case.
    //

    // Blt-Model Back Buffer Counts
    RFactor rfBltModelBufferCount = AddParameter
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

    // Flip-Model Back Buffer Counts
    RFactor rfFlipModelBufferCount = AddParameterValueSet
        (
        _T("BufferCount"),
        RangeValueSet
        (
        (UINT)2,
        (UINT)m_MaxBufferCount,
        (UINT)1
        )
        );

    // Blt-Model Swap Effects
    CUserValueSet< DXGISwapEffectRecord > *pBltModelSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();
    for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
    {
        if(DXGISwapEffects[i].dxgiSwapEffect == DXGI_SWAP_EFFECT_SEQUENTIAL)
        {
            pBltModelSwapEffect->AddValue(DXGISwapEffects[i]);
        }
    }

    RFactor rfBltModelSwapEffect =  AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pBltModelSwapEffect);


    // Flip-Model Swap Effects
    CUserValueSet< DXGISwapEffectRecord > *pFlipModelSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();
    for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
    {
        if(DXGISwapEffects[i].dxgiSwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL)
        {
            pFlipModelSwapEffect->AddValue(DXGISwapEffects[i]);
        }
    }

    RFactor rfFlipModelSwapEffect =  AddParameterValueSet< DXGISwapEffectRecord >( _T("SwapEffect") , pFlipModelSwapEffect);

    RFactor rfBltModelCombo =  (rfBltModelSwapEffect * rfBltModelBufferCount);
    RFactor rfFlipModelCombo =  (rfFlipModelSwapEffect * rfFlipModelBufferCount);

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
    RFactor Bpp16_DwmOff = m_FeatureLevelFactor * m_globalRFactorDwmOffBpp * v_dwmCompositionOff * DontUseDCOMPFactor * (rfBltModelCombo + rfFlipModelCombo);
    // DWM ON - DCOMP Not Supported on Win7+8IP and below
    RFactor Bpp32_DwmOn_NoDComp = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * v_dwmCompositionOn   * DontUseDCOMPFactor * (rfBltModelCombo + rfFlipModelCombo);
    // DWM ON - DCOMP Supported, but only for FlipModel.  for Regular Windowed Presents test both blt and Flip model.
    RFactor Bpp32_DwmOn = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * v_dwmCompositionOn   * (DontUseDCOMPFactor * (rfBltModelCombo + rfFlipModelCombo) + UseDCOMPFactor*rfFlipModelCombo);
    // DWM Off - DCOMP Not Supported
    RFactor Bpp32_DwmOff = m_FeatureLevelFactor * m_globalRFactorDwmOnBpp * dwmCompositionOff_32bpp * DontUseDCOMPFactor * (rfBltModelCombo + rfFlipModelCombo);

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
    DeclarePriorityLevelByPriorityValue(2, 1.0f);

}


