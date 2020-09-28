//
// Project headers
//

#include "DXGIPresentFlags.h"


#pragma warning(disable: 4355)


CFSSequential::CFSSequential()
{
    m_NumberOfPresents = 1;
    m_TexturesToLoad = m_NumberOfPresents;
    m_MaxBufferCount = 3;
};


CFSSequential::~CFSSequential()
{
};

void CFSSequential::InitTestParameters()
{
    CPresentFS::InitTestParameters();
};


RFactor CFSSequential::IntitTestRFactors()
{
    RFactor bufferCount =  AddParameter
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

    RFactor presentFlags =  AddParameter< DXGIPresentFlagRecord >( _T("PresentFlags") , &m_PresentFlag, pPresentFlags);

    CUserValueSet< DXGISwapEffectRecord > *pSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();
    for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
    {
        if(DXGISwapEffects[i].dxgiSwapEffect == DXGI_SWAP_EFFECT_SEQUENTIAL)
        {
            pSwapEffect->AddValue(DXGISwapEffects[i]);
        }
    }

    RFactor swapEffect =  AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pSwapEffect);

    return (bufferCount * presentFlags * swapEffect);

}



