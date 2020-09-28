// FILE:        cull.cpp
// DESC:        cull class methods for point sprite conformance test
// AUTHOR:      Anthony W. Clarke
// COMMENTS:    checks cull mode has no effect on whether or not point sprites are drawn

#include "sprite.h"

const UINT NCULLMODES = 3;

// NAME:        CSpriteCull()
// DESC:        constructor for sprite cull class
// INPUT:       none
// OUTPUT:      none

CSpriteCull::CSpriteCull()
{
    m_szTestName="Cull mode";
    m_szCommandKey="Cull";

    m_Group.Replace(0);
    m_Group.Save();
}

// NAME:        ~CSpriteCull()
// DESC:        constructor for sprite cull class
// INPUT:       none
// OUTPUT:      none

CSpriteCull::~CSpriteCull()
{
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteCull::TestInitialize(VOID)
{
    UINT uResult = CSpriteBatch::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    m_nLastCullMode = -1;
    SetTestRange((UINT) 1, NCULLMODES*m_nVariationsPerFormat);
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test executed successfully
//              false....otherwise

bool CSpriteCull::ExecuteTest(UINT uTest)
{
    UINT uCullMode = (uTest-1)/(m_nVariationsPerFormat);

    if (m_nLastCullMode != uCullMode)
    {
        if (0 == uCullMode)
        {
            if (!(m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps & D3DPMISCCAPS_CULLNONE))
            {
                SkipTestRange(uTest, m_nVariationsPerFormat);
                WriteToLog("Cull mode: D3DCULL_NONE NOT supported.\n");
                return false;
            }

            SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);
            WriteToLog("Cull mode: D3DCULL_NONE.\n");
        }
        else if (1 == uCullMode)
        {
            if (!(m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps & D3DPMISCCAPS_CULLCW))
            {
                SkipTestRange(uTest, m_nVariationsPerFormat*2);
                WriteToLog("Cull mode: D3DCULL_CW NOT supported.\n");
                return false;
            }

            SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_CW);
            WriteToLog("Cull mode: D3DCULL_CW.\n");
        }
        else if (2 == uCullMode)
        {
            if (!(m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps & D3DPMISCCAPS_CULLCCW))
            {
                SkipTestRange(uTest, m_nVariationsPerFormat*3);
                WriteToLog("Cull mode: D3DCULL_CCW NOT supported.\n");
                return false;

            }
            SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_CCW);
            WriteToLog("Cull mode: D3DCULL_CCW.\n");
        }
        else if (uCullMode > 3)
        {
            return false;
        }

        m_nLastCullMode = uCullMode;
    }

    sprintf(m_szBuffer, "%s, Format %s", m_szTestName, m_szFormat);
    BeginTestCase(m_szBuffer, uTest);

    // test first texture format in each cull mode

    return CSpriteBatch::ExecuteTest(uTest% m_nVariationsPerFormat); 
}
