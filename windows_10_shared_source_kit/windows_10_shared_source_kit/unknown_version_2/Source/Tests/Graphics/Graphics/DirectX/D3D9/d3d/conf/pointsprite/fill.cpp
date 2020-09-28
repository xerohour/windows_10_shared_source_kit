// FILE:        fill.cpp
// DESC:        fill class methods for point sprite conformance test
// AUTHOR:      Anthony W. Clarke
// COMMENTS:    checks fill mode has no effect on whether or not point sprites are drawn

#include "sprite.h"

// NAME:        CSpriteFill()
// DESC:        constructor for sprite fill class
// INPUT:       none
// OUTPUT:      none

CSpriteFill::CSpriteFill()
{
    m_szTestName="Fill mode";
    m_szCommandKey="Fill";

    m_Group.Replace(0);
    m_Group.Save();
}

// NAME:        ~CSpriteFill()
// DESC:        destructor for sprite fill class
// INPUT:       none
// OUTPUT:      none

CSpriteFill::~CSpriteFill()
{
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteFill::TestInitialize(VOID)
{
    UINT uResult = CSpriteBatch::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    if (m_Flags.Check(CF_INVALID))
        m_bInvalid = true;
    else
        m_bInvalid = false;

    if (m_Flags.Check(CF_TEXTURE))
        m_bTexture = true;
    else
        m_bTexture = false;

    m_nLastFillMode = -1;
    SetTestRange((UINT) 1, (UINT) (3*m_nVariationsPerFormat));
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test executed successfully
//              false....otherwise

bool CSpriteFill::ExecuteTest(UINT uTest)
{
    UINT uFillMode = (uTest - 1)/(m_nVariationsPerFormat);

    if (m_nLastFillMode != uFillMode)
    {
        if (m_bInvalid)
            m_Flags.Set(CF_INVALID);
        else
            m_Flags.Clear(CF_INVALID);

        if (m_bTexture)
            m_Flags.Set(CF_TEXTURE);
        else
            m_Flags.Clear(CF_TEXTURE);

        if (0 == uFillMode)
        {
            // set invalid to hide manbug 48370, remove code when fixed

            if (m_Flags.Check(CF_EMULATING))
                m_Flags.Set(CF_INVALID);

            SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_POINT);
            WriteToLog("Fill mode is now D3DFILL_POINT\n");
        }
        else if (1 == uFillMode)
        {
            if (!(m_pSrcDevice->GetCaps()->dwLineCaps & D3DLINECAPS_TEXTURE))
                m_Flags.Clear(CF_TEXTURE);

            SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_WIREFRAME);
            WriteToLog("Fill mode is now D3DFILL_WIREFRAME\n");
        }
        else if (2 == uFillMode)
        {
            SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_SOLID);
            WriteToLog("Fill mode is now D3DFILL_SOLID\n");
        }
        else if (uFillMode > 3)
            return false;

        m_nLastFillMode = uFillMode;
    }

    sprintf(m_szBuffer, "%s, Format %s", m_szTestName, m_szFormat);
    BeginTestCase(m_szBuffer, uTest);

    // ensure we only test first texture format, but test it for each cull mode

    return CSpriteBatch::ExecuteTest(uTest % m_nVariationsPerFormat); 
}
