// FILE:        scalefvftls.cpp
// DESC:        scale class methods for point sprite conformance test
// AUTHOR:      Anthony W. Clarke
// COMMENTS:    D3DFVF_TLVERTEX | D3DFVF_PSIZE

#include "sprite.h"

// NAME:        CSpriteScaleFVFTLS()
// DESC:        constructor for sprite scale class (D3DFVF_TLVERTEX | D3DFVF_PSIZE)
// INPUT:       none
// OUTPUT:      none

CSpriteScaleFVFTLS::CSpriteScaleFVFTLS(VOID)
{
    m_szTestName="Point sprite scale (D3DFVF_TLVERTEX | D3DFVF_PSIZE)";
    m_szCommandKey="FVFTLS";

    m_Group.Replace(0);
    m_Group.Save();
}

// NAME:        ~CSpriteScaleFVFTLS()
// DESC:        destructor for sprite scale class (D3DFVF_TLVERTEX | D3DFVF_PSIZE)
// INPUT:       none
// OUTPUT:      none

CSpriteScaleFVFTLS::~CSpriteScaleFVFTLS(VOID)
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CSpriteScaleFVFTLS::CommandLineHelp(void)
{
    CSprite::CommandLineHelp();
    WriteCommandLineHelp("$yinvalid: $wrun invalid parameter tests$c($wYes$c/No)");
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code
// COMMENTS:    m_nVarationsPerFormat:
//                *2 for POINTSPRITEENABLE off/on states
//                /2 as there are always two point sprites on screen

UINT CSpriteScaleFVFTLS::TestInitialize(VOID)
{
    UINT uResult;

    m_Group.Set(CF_VERTEXPOINTSIZE);
    uResult = CSprite::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    m_Sprite.PointSize.f = 1.0f;
    m_nVariationsPerFormat *= NCLIPTESTS;
    SetTestRange((UINT) 1, (UINT) (m_uCommonTextureFormats*m_nVariationsPerFormat));
    Clear(m_dcBackground);
    SetViewport(CLIPBORDER,
                CLIPBORDER,
                m_pCurrentMode->nBackBufferWidth - (CLIPBORDER*2),
                m_pCurrentMode->nBackBufferHeight - (CLIPBORDER*2));

    if (m_Flags.Check(CF_INVALID))
        bInvalid = true;
    else
        bInvalid = false;

    if (!m_Flags.Check(CF_VERTEXPOINTSIZE))
    {
        m_Flags.Set(CF_INVALID);
        WriteToLog("Invalid parameter test.\n");
    }

    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CSpriteScaleFVFTLS::ExecuteTest(UINT uTest)
{
    bool bResult = true;
    D3DVALUE dvS, dvT;
    UINT uPosition, uSizes;

    uPosition = (uTest-1) % NCLIPTESTS;
    uSizes = m_nVariationsPerFormat/NCLIPTESTS;
    dvT = (D3DVALUE) (((uTest - 1) / NCLIPTESTS) % uSizes);
    dvT /= uSizes;
    dvS = dvT*(m_dvPointSizeMax*1.5);

    // ensure RS point size is not set

    m_Sprite.PointSize.f = 0.0;

    if (!CSprite::ExecuteTest(uTest))
        return false;

    // enable(odd)/disable(even) point sprites

    SetRenderState(D3DRS_POINTSPRITEENABLE, (DWORD) (uTest & 1));

    sprintf(m_szBuffer, "Point size (vertex): %f, Format %s", dvS, m_szFormat);
    BeginTestCase(m_szBuffer, uTest);

    if (BeginScene())
    {
        DWORD dwFormat = MYD3DFVF_TLSVERTEX;
        MYD3DTLSVERTEX Vertex[4];

        m_Sprite.PointSize.f = dvS; // used in SetPosition
        SetPosition(uPosition, &Vertex[0].dvSX, &Vertex[0].dvSY);
        Vertex[0].dvSZ = m_Port.CenterZ();
        Vertex[0].dvRHW = 1.0f;
        Vertex[0].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(0,0,255,255);
        Vertex[0].dcSpecular = m_dcBackground;
        Vertex[0].dvTU = 0.75f;
        Vertex[0].dvTV = 0.50f;
        Vertex[0].dvS = dvS;

        SetPosition((uPosition+2)%NCLIPTESTS, &Vertex[1].dvSX, &Vertex[1].dvSY);
        Vertex[1].dvSZ = m_Port.CenterZ();
        Vertex[1].dvRHW = 1.0f;
        Vertex[1].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(0,255,0,255);
        Vertex[1].dcSpecular = m_dcBackground;
        Vertex[1].dvTU = 0.55f;
        Vertex[1].dvTV = 0.70f;
        Vertex[1].dvS = dvS*2;

        // plus four => diagonally opposite from current position

        SetPosition((uPosition+4)%NCLIPTESTS, &Vertex[2].dvSX, &Vertex[2].dvSY);
        Vertex[2].dvSZ = m_Port.CenterZ();
        Vertex[2].dvRHW = 1.0f;
        Vertex[2].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(0,255,0,255);
        Vertex[2].dcSpecular = m_dcBackground;
        Vertex[2].dvTU = 0.55f;
        Vertex[2].dvTV = 0.70f;
        Vertex[2].dvS = dvS;

        Vertex[3].dvSX = (int) m_Port.CenterX();
        Vertex[3].dvSY = (int) m_Port.CenterY();
        Vertex[3].dvSZ = m_Port.CenterZ();
        Vertex[3].dvRHW = 1.0f;
        Vertex[3].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(0,255,0,255);
        Vertex[3].dcSpecular = m_dcBackground;
        Vertex[3].dvTU = 0.55f;
        Vertex[3].dvTV = 0.70f;
        Vertex[3].dvS = dvS*2;

        m_Sprite.PointSize.f = 0.0;

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

// NAME:        TestTerminate()
// DESC:        terminate test
// INPUT:       none
// OUTPUT:      true.....if test is terminated successfully
//              false....otherwise

bool CSpriteScaleFVFTLS::TestTerminate(VOID)
{
    if (bInvalid)
        m_Flags.Set(CF_INVALID);
    else
        m_Flags.Clear(CF_INVALID);

    return CSprite::TestTerminate();
}
