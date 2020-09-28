//
// Project headers
//

#include "DXGISwapEffect.h"


#pragma warning(disable: 4355)


CSEFSSequential::CSEFSSequential()
{
    m_bIgnoreRefresh = true;
    m_bAlwaysFilterLowResolution = true;
    m_MaxBufferCount = 3;
};


CSEFSSequential::~CSEFSSequential()
{
};

TEST_RESULT CSEFSSequential::ExecuteTestCase()
{
	
    m_NumberOfPresents = m_BufferCount;
    
    
    //
    // Present one more time to make sure it is the first buffer
    // and not an extra buffer
    //
    if(m_ValidateNoExtraBuffers)
    {
        m_NumberOfPresents += 1;    
    }

    m_TexturesToLoad = m_BufferCount;

    return CPresentFS::ExecuteTestCase();
};

void CSEFSSequential::InitTestParameters()
{
    CPresentFS::InitTestParameters();
};


RFactor CSEFSSequential::IntitTestRFactors()
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


    CUserValueSet< DXGIPresentFlagRecord > *pPresentFlags = new CUserValueSet< DXGIPresentFlagRecord >();
    pPresentFlags->AddValue(DXGIPresentFlags[0]);
    RFactor presentFlags =  AddParameter< DXGIPresentFlagRecord >( _T("PresentFlags") , &m_PresentFlag, pPresentFlags);


    // Buffer count = 1 & DXGI_SWAP_EFFECT_SEQUENTIAL has different code path than others
    AddPriorityPatternFilter(( FilterEqual< DXGISwapEffectRecord >(_T("SwapEffect"), DXGISwapEffects[1] ) && FilterEqual< UINT >(_T("BufferCount"), 1 )), WeightFactorLarge);

    
    return ( ( (rfBltModelSwapEffect * rfBltModelBufferCount) + (rfFlipModelSwapEffect * rfFlipModelBufferCount) ) * presentFlags * validateNoExtraBuffers);


}

