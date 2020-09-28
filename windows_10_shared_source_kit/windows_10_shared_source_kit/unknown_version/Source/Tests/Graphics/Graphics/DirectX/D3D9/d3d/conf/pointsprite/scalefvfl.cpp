// FILE:        scalefvfl.cpp
// DESC:        scale class methods for point sprite conformance test
// AUTHOR:      Anthony W. Clarke
// COMMENTS:    D3DFVF_LVERTEX

#include "sprite.h"

// NAME:        CSpriteScaleFVFL()
// DESC:        constructor for sprite scale class (D3DFVF_LVERTEX)
// INPUT:       none
// OUTPUT:      none

CSpriteScaleFVFL::CSpriteScaleFVFL(VOID)
{
    m_szTestName="Point sprite scale (D3DFVF_LVERTEX)";
    m_szCommandKey="FVFL";

    m_Group.Replace(0);
    m_Group.Save();
}

// NAME:        ~CSpriteScaleFVFL()
// DESC:        destructor for sprite scale class (D3DFVF_LVERTEX)
// INPUT:       none
// OUTPUT:      none

CSpriteScaleFVFL::~CSpriteScaleFVFL(VOID)
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CSpriteScaleFVFL::CommandLineHelp(void)
{
     CSprite::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteScaleFVFL::TestInitialize(VOID)
{
    UINT uResult = CSprite::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

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

bool CSpriteScaleFVFL::ExecuteTest(UINT uTest)
{
    bool bResult = true;
    D3DVALUE dvT;
    UINT uPosition, uSizes;

    uPosition = (uTest-1) % NCLIPTESTS;
    uSizes = m_nVariationsPerFormat/NCLIPTESTS;
    dvT = (D3DVALUE) (((uTest - 1) / NCLIPTESTS) % uSizes);
    dvT /= uSizes;
    m_Sprite.PointSize.f = dvT*(m_dvPointSizeMax * 1.5);

    // This is done to avoid precision errors and the ambiguity that
    // exists when point sprites are 0 - 1 in size. Also a larger size
    // allows greater error in applying textures.
    // Even numbered point sprite sizes are used so that an even number
    // of pixels is added to each side of the point sprite. This
    // along with SetPositionWorld avoid precision errors.
    m_Sprite.PointSize.f = (int)m_Sprite.PointSize.f;
    m_Sprite.PointSize.f = (m_Sprite.PointSize.f < MIN_POINTSPRITE_SIZE) ? MIN_POINTSPRITE_SIZE : m_Sprite.PointSize.f;

    if (((int)m_Sprite.PointSize.f) % 2)
        m_Sprite.PointSize.f++;
    
    if (!CSprite::ExecuteTest(uTest))
        return false;

    // turn on/off texture stretching for this point
    SetRenderState(D3DRS_POINTSPRITEENABLE, (DWORD) (uTest&1));
    sprintf(m_szBuffer, "Point size (state): %f, Format %s", m_Sprite.PointSize.f, m_szFormat);
    BeginTestCase(m_szBuffer, uTest);

    if (BeginScene())
    {
        DWORD dwFormat = MYD3DFVF_LVERTEX;
        MYD3DLVERTEX Vertex[3];

        SetPositionWorld(uPosition, &Vertex[0].dvX, &Vertex[0].dvY);
        Vertex[0].dvZ = (float) EPSILON;
        Vertex[0].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
        Vertex[0].dcSpecular = m_dcBackground;
        Vertex[0].dvTU = 0.0f;
        Vertex[0].dvTV = 0.0f;

        // +4 means with current position array two point sprites can be diagonally opposite on screen
        SetPositionWorld(((uPosition + 4) % NCLIPTESTS), &Vertex[1].dvX, &Vertex[1].dvY);
        Vertex[1].dvZ = (float) EPSILON;
        Vertex[1].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
        Vertex[1].dcSpecular = m_dcBackground;
        Vertex[1].dvTU = 0.0f;
        Vertex[1].dvTV = 0.0f;
        
        // extra sprite added to allow for larger matrix multiplication precision errors
        // (rendered at screen center so that it is never clipped)
        Vertex[2] = Vertex[0];
        Vertex[2].dvX = m_Port.CenterX();
        Vertex[2].dvY = m_Port.CenterY();
        Vertex[2].dvZ = 0;

        if (!AdjustVertex((D3DXVECTOR3 *) &Vertex[2]))
        {
            WriteToLog("Unable to adjust vertex thru AdjustVertex().\n");
            bResult = false;
        }
        else
        {
            Vertex[2].dvZ = (float) EPSILON;
        
            if (!RenderPrimitive(D3DPT_POINTLIST, dwFormat, Vertex, (DWORD) 3, NULL, 0, NULL))
            {
                WriteToLog("RenderPrimitive() failed.\n");
                bResult = false;
            }
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
