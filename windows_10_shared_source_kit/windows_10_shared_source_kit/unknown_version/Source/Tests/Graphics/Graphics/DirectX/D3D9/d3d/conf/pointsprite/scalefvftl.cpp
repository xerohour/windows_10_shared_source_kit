// FILE:        scalefvftl.cpp
// DESC:        scale class methods for point sprite conformance test
// AUTHOR:      Anthony W. Clarke
// COMMENTS:    D3DFVF_TLVERTEX

#include "sprite.h"

// NAME:        CSpriteScaleFVFTL()
// DESC:        constructor for sprite scale class (D3DFVF_TLVERTEX)
// INPUT:       none
// OUTPUT:      none

CSpriteScaleFVFTL::CSpriteScaleFVFTL(VOID)
{
    m_szTestName="Point sprite scale (D3DFVF_TLVERTEX)";
    m_szCommandKey="FVFTL";

    m_Group.Replace(0);
    m_Group.Save();
}

// NAME:        ~CSpriteScaleFVFTL()
// DESC:        destructor for sprite scale class (D3DFVF_TLVERTEX)
// INPUT:       none
// OUTPUT:      none

CSpriteScaleFVFTL::~CSpriteScaleFVFTL(VOID)
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CSpriteScaleFVFTL::CommandLineHelp(void)
{
     CSprite::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteScaleFVFTL::TestInitialize(VOID)
{
    UINT uResult = CSprite::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    // *2 for POINTSPRITEENABLE off/on states /2 because there are always two point sprites on screen

    m_nVariationsPerFormat *= NCLIPTESTS;
    SetTestRange((UINT) 1, (UINT) (m_uCommonTextureFormats*m_nVariationsPerFormat));
    Clear(m_dcBackground);

    // done so sprites clipped against viewport can be seen to be clipped correctly

    SetViewport(CLIPBORDER,
                CLIPBORDER,
                m_pCurrentMode->nBackBufferWidth - (CLIPBORDER*2),
                m_pCurrentMode->nBackBufferHeight - (CLIPBORDER*2));
    return uResult;
}


// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CSpriteScaleFVFTL::ExecuteTest(UINT uTest)
{
    bool bResult = true;
    D3DVALUE dvT;
    UINT uPosition, uSizes;

    uPosition = (uTest-1) % NCLIPTESTS;
    uSizes = m_nVariationsPerFormat/NCLIPTESTS;
    dvT = (D3DVALUE) (((uTest - 1) / NCLIPTESTS) % uSizes);
    dvT /= uSizes;
    m_Sprite.PointSize.f = dvT*(m_dvPointSizeMax * 1.5);

    if (!CSprite::ExecuteTest(uTest))
        return false;

    // turn on/off texture stretching for this point

    SetRenderState(D3DRS_POINTSPRITEENABLE, (DWORD) (uTest & 1));

    sprintf(m_szBuffer, "Point size (state): %f, Format %s", m_Sprite.PointSize.f, m_szFormat);
    BeginTestCase(m_szBuffer, uTest);

    if (BeginScene())
    {
        DWORD dwFormat = MYD3DFVF_TLVERTEX;
        MYD3DTLVERTEX Vertex[4];

        SetPosition(uPosition, &Vertex[0].dvSX, &Vertex[0].dvSY);
        Vertex[0].dvSZ = m_Port.CenterZ();
        Vertex[0].dvRHW = 1.0f;
        Vertex[0].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
        Vertex[0].dcSpecular = m_dcBackground;
        Vertex[0].dvTU = 0.75f;
        Vertex[0].dvTV = 0.50f;

        SetPosition((uPosition+2)%NCLIPTESTS, &Vertex[1].dvSX, &Vertex[1].dvSY);
        Vertex[1].dvSZ = m_Port.CenterZ();
        Vertex[1].dvRHW = 1.0f;
        Vertex[1].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
        Vertex[1].dcSpecular = m_dcBackground;
        Vertex[1].dvTU = 0.0f;
        Vertex[1].dvTV = 0.0f;

        // plus four => diagonally opposite from current position

        SetPosition((uPosition + 4)%NCLIPTESTS, &Vertex[2].dvSX, &Vertex[2].dvSY);
        Vertex[2].dvSZ = m_Port.CenterZ();
        Vertex[2].dvRHW = 1.0f;
        Vertex[2].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(0,0,255,255);
        Vertex[2].dcSpecular = m_dcBackground;
        Vertex[2].dvTU = 0.55f;
        Vertex[2].dvTV = 0.70f;

        Vertex[3].dvSX = (int) m_Port.CenterX();
        Vertex[3].dvSY = (int) m_Port.CenterY();
        Vertex[3].dvSZ = m_Port.CenterZ();
        Vertex[3].dvRHW = 1.0f;
        Vertex[3].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
        Vertex[3].dcSpecular = m_dcBackground;
        Vertex[3].dvTU = 0.0f;
        Vertex[3].dvTV = 0.0f;

        if (!RenderPrimitive(D3DPT_POINTLIST, dwFormat, Vertex, (DWORD) 4, NULL, 0, NULL))
        {
            WriteToLog("RenderPrimitive() failed.\n");
            bResult = false;
        }

        if (!EndScene())
        {
            WriteToLog("EndScene() failed.\n");
            bResult = false;
        }
    }
    else
    {
        WriteToLog("BeginScene() failed.\n");
        bResult = false;
    }

    return bResult;
}
