////////////////////////////////////////////////////////////////////////////////
// EndOfPipeline.h
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TimingData.h"

class CEndOfPipelineTest : public CTimingDataTest
{
public:
    CEndOfPipelineTest();

    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual void CleanupTestCase();

protected:
    ID3D11Texture2D *m_pRTTex_Large;
    ID3D11RenderTargetView *m_pRTView_Large;
};

