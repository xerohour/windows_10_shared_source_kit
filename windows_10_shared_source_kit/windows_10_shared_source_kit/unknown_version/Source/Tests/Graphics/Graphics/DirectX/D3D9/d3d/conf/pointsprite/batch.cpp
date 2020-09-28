// FILE:        batch.cpp
// DESC:        batch methods for point sprite conformance test
// AUTHOR:      Anthony W. Clarke
// COMMENTS:    check points sprites are rendered correctly
//              after both render state and vertex data adjustments

#include "sprite.h"

// NAME:        CSpriteBatch()
// DESC:        batch class constructor
// INPUT:       none
// OUTPUT:      none

CSpriteBatch::CSpriteBatch(VOID)
{
    m_szTestName= "Point sprite batches";
    m_szCommandKey= "Batch";

    m_Group.Replace(0);
    m_Group.Save();
}

// NAME:        ~CSpriteBatch()
// DESC:        batch class destructor
// INPUT:       none
// OUTPUT:      none

CSpriteBatch::~CSpriteBatch(VOID)
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CSpriteBatch::CommandLineHelp(void)
{
    CSprite::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteBatch::TestInitialize(VOID)
{   
    UINT nScissor;
    UINT uResult = CSprite::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    nScissor = (UINT) m_Flags.Check(CF_SCISSOR) ? 2 : 1;
    m_nVariationsPerFormat *= NCLIPTESTS/NUMPOINTSPRITES_ONSCREEN;

    SetTestRange((UINT) 1, nScissor*m_nVariationsPerFormat*(m_uCommonTextureFormats + 1));
    SetRenderState(D3DRS_POINTSCALEENABLE, (DWORD) FALSE);
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CSpriteBatch::ExecuteTest(UINT uTest)
{
    bool bResult = true;
    
    // change shade mode every second sample change

    if ((uTest % 4) < 2)
        m_Flags.Clear(CF_FLAT);
    else
        m_Flags.Set(CF_FLAT);

    if (m_Flags.Check(CF_SCISSOR))
    {
        if (uTest <= m_nVariationsPerFormat*(m_uCommonTextureFormats + 1))
        {
            // 10% border

            DWORD dwBorderWidth = (DWORD) (0.1*m_pCurrentMode->nBackBufferWidth);
            DWORD dwBorderHeight = (DWORD) (0.1*m_pCurrentMode->nBackBufferHeight);
            RECT Rect;

            Rect.top = dwBorderHeight;
            Rect.left = dwBorderWidth;
            Rect.bottom = m_pCurrentMode->nBackBufferHeight - dwBorderHeight;
            Rect.right = m_pCurrentMode->nBackBufferWidth - dwBorderWidth;

            if (!EnableScissorTest(&Rect))
                return false;
        }
        else if (!DisableScissorTest())
            return false;
    }

    if (!CSprite::ExecuteTest(uTest))
        return false;

    if (!m_pFramework->m_DXGLog.InTestCase())
    {
        sprintf(m_szBuffer, "%s, Format %s", m_szTestName, m_szFormat);
        BeginTestCase(m_szBuffer, uTest);
    }
    
    // enable(odd)/disable(even) point sprites

    m_dwPointSpriteEnable = (uTest & 1);
    SetRenderState(D3DRS_POINTSPRITEENABLE, (DWORD) m_dwPointSpriteEnable);

    if (BeginScene())
    {
        DWORD dwFormat = MYD3DFVF_TLVERTEX;
        MYD3DTLVERTEX Vertex;
        MYD3DTLVERTEXNOTEX VertexNoTex;

        Vertex.dvSX = 0.0;
        Vertex.dvSY = 0.0;
        Vertex.dvSZ = m_Port.CenterZ();

        if (m_Flags.Check(CF_TEXTURE))
        {
            Vertex.dcColor = m_dcBackground;
            Vertex.dcSpecular = m_dcBackground;
        }
        else
        {
            Vertex.dcColor = D3DCOLOR_RGBA(0,255,0,255);
            Vertex.dcSpecular = D3DCOLOR_RGBA(255,255,255,255);
        }

        Vertex.dvRHW = 1.0f;
        Vertex.dvTU = 0.0f;
        Vertex.dvTV = 0.0f;
        m_Sprite.PointSize.f = m_dvPointSizeMax;

        // render with texture coords

        if (!DrawPointSprites(&Vertex, dwFormat, (uTest-1)%NCLIPTESTS, 2))
        {
            WriteToLog("DrawPointSprites FVF with Texture Co-Ords failed.\n");
            bResult = false;
        }

        // render a vertex, not a point sprite

        m_Sprite.PointSize.f = 1.0;

        if (!DrawPointSprites(&Vertex, dwFormat, (uTest+1)%NCLIPTESTS, 1))
        {
            WriteToLog("DrawPointSprites rendering a vertex FVF with Texture Co-Ords failed.\n");
            bResult = false;
        }

        if (!DrawCenterQuads())
            bResult = false;

        // render without texture coords

        dwFormat = MYD3DFVF_TLVERTEX_NOTEX;
        VertexNoTex.dvSX = 0.0;
        VertexNoTex.dvSY = 0.0;
        VertexNoTex.dvSZ = m_Port.CenterZ();

        if (m_Flags.Check(CF_TEXTURE))
        {
            VertexNoTex.dcColor = m_dcBackground;
            VertexNoTex.dcSpecular = m_dcBackground;
        }
        else
        {
            VertexNoTex.dcColor = D3DCOLOR_RGBA(255,0,0,255);
            VertexNoTex.dcSpecular = D3DCOLOR_RGBA(255,255,255,255);
        }

        VertexNoTex.dvRHW = 1.0f;    
        m_Sprite.PointSize.f = m_dvPointSizeMax;

        if (!DrawPointSprites(&VertexNoTex, dwFormat, (uTest+2)%NCLIPTESTS, 2))
        {
            WriteToLog("DrawPointSprites FVF with out Texture Co-Ords failed.\n");
            bResult = false;
        }

        // render a vertex, not a point sprite

        m_Sprite.PointSize.f = 1;

        if (!DrawPointSprites(&VertexNoTex, dwFormat, (uTest+3)%NCLIPTESTS, 1))
        {
            WriteToLog("DrawPointSprites rendering a vertex FVF with out Texture Co-Ords failed.\n");
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

// NAME:        ProcessFrame()
// DESC:        process frame
// INPUT:       none
// OUTPUT:      true.....if frame processed successfully
//              false....otherwise

bool CSpriteBatch::ProcessFrame(void)
{
    bool bResult;
    DWORD dwMode, dwTexture;

    bResult = CSprite::ProcessFrame();

    if (!bResult)
    {
        if (m_Flags.Check(CF_TEXTURE))
            dwTexture = 1;
        else
            dwTexture = 0;

        if (m_Flags.Check(CF_FLAT))
            dwMode = D3DSHADE_FLAT;
        else
            dwMode = D3DSHADE_GOURAUD;

        WriteToLog("RenderStates: ShadeMode %lu; PointSpriteEnable %lu; Texture %lu\n",
                   dwMode, m_dwPointSpriteEnable, dwTexture);
    }

    return bResult;
}

// NAME:        DrawPointSprites()
// DESC:        draw point sprites at incremental locations
// INPUT:       pVertex..point sprite vertex
//              dwFormat.vertex format
//              nStart...start index into NCLIPTESTS array
//              nSprites.number of point sprites to draw
// OUTPUT:      true.....if point sprites are drawn successfully
//              false....otherwise

bool CSpriteBatch::DrawPointSprites(LPVOID pVertex, DWORD dwFormat, UINT nStart, UINT nSprites)
{
    int i;
    D3DVALUE *pdvSX, *pdvSY;

    if (!(D3DFVF_XYZRHW | dwFormat))
    {
        WriteToLog("DrawPointSprite failed. Points must be TnL\n");
        return false;
    }

    pdvSX = (D3DVALUE *) pVertex;
    pdvSY = (D3DVALUE *) (((BYTE *) pVertex) + sizeof(D3DVALUE));

    for (i = 0; i < nSprites; i++)
    {
        SetPosition((nStart + i) % NCLIPTESTS, pdvSX, pdvSY);
        SetRenderState(D3DRS_POINTSIZE, m_Sprite.PointSize.dw);
        
        if (!RenderPrimitive(D3DPT_POINTLIST, dwFormat, pVertex, (DWORD) 1, NULL, 0, NULL))
        {
            WriteToLog("RenderPrimitive() D3DPT_POINTLIST failed.\n");
            return false;
        }

        m_Sprite.PointSize.f /= 2.0f;
        // clamp to avoid precision errors
        m_Sprite.PointSize.f = (m_Sprite.PointSize.f < 1.0f) ? 1.0f : m_Sprite.PointSize.f;
    }

    return true;
}


// NAME:        DrawCenterQuads()
// DESC:        draw center quads
// INPUT:       none
// OUTPUT:      true.....if point sprites are drawn successfully
//              false....otherwise

bool CSpriteBatch::DrawCenterQuads(VOID)
{
    bool bResult = true;
    MYD3DTLVERTEX pQuadCW[4] =
    {
        {  78.0f, 172.0f, 0.5f, 2.0f, D3DCOLOR_RGBA(255,   0,   0, 255), D3DCOLOR_RGBA(  0,   0,   0, 255), 0.0f, 1.0f },
        {  78.0f, 108.0f, 0.5f, 2.0f, D3DCOLOR_RGBA(  0, 255,   0, 255), D3DCOLOR_RGBA(  0,   0,   0, 255), 0.0f, 0.0f },
        { 142.0f, 172.0f, 0.5f, 2.0f, D3DCOLOR_RGBA(255, 255, 255, 255), D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f, 1.0f },
        { 142.0f, 108.0f, 0.5f, 2.0f, D3DCOLOR_RGBA(  0,   0, 255, 255), D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f, 0.0f }
    };

    MYD3DTLVERTEX pQuadCCW[4] =
    {
        { 242.0f, 172.0f, 0.5, 2.0f, D3DCOLOR_RGBA(255, 255, 255, 255), D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f, 1.0f },
        { 242.0f, 108.0f, 0.5, 2.0f, D3DCOLOR_RGBA(  0,   0, 255, 255), D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f, 0.0f },
        { 178.0f, 172.0f, 0.5, 2.0f, D3DCOLOR_RGBA(255,   0,   0, 255), D3DCOLOR_RGBA(  0,   0,   0, 255), 0.0f, 1.0f },
        { 178.0f, 108.0f, 0.5, 2.0f, D3DCOLOR_RGBA(  0, 255,   0, 255), D3DCOLOR_RGBA(  0,   0,   0, 255), 0.0f, 0.0f }
    };

    // device supports size in vertex? test that device obeys vertex size over render state size

    if ((m_pSrcDevice->GetCaps()->dwFVFCaps & D3DFVFCAPS_PSIZE) &&
        (m_pRefDevice->GetCaps()->dwFVFCaps & D3DFVFCAPS_PSIZE))
    {
        DWORD dwFormat = MYD3DFVF_TLSVERTEX;
        MYD3DTLSVERTEX VertexWidthSize =
        { 120.0f,  80.0f, 0.5f, 2.0f, D3DCOLOR_RGBA(  0, 255,   0,   0), D3DCOLOR_RGBA(255, 255, 255, 255), 0.5f, 0.5f, 1.0f };
        MYD3DTLVERTEX Vertex =
        { 210.0f,  80.0f, 0.5f, 2.0f, D3DCOLOR_RGBA(  0,   0, 255,   0), D3DCOLOR_RGBA(255, 255, 255, 255), 0.5f, 0.5f };

        // set point sprite size to min of (max point size, 32)

        m_Sprite.PointSize.f = (m_Sprite.PointSizeMax.f > 32.0f) ? 32.0f : m_Sprite.PointSizeMax.f;
        SetRenderState(D3DRS_POINTSIZE, m_Sprite.PointSize.dw);
        VertexWidthSize.dvS = m_Sprite.PointSize.f / 2.0f;

        // left/center sprite (vertex with size)

        if (!RenderPrimitive(D3DPT_POINTLIST, MYD3DFVF_TLSVERTEX, (VOID *) &VertexWidthSize, 1, NULL, 0, NULL))
        {
            WriteToLog("RenderPrimitive() D3DPT_POINTLIST failed.\n");
            bResult = false;
        }

        // right/center sprite (vertex without size)

        if (!RenderPrimitive(D3DPT_POINTLIST, MYD3DFVF_TLVERTEX, (VOID *) &Vertex, 1, NULL, 0, NULL))
        {
            WriteToLog("RenderPrimitive() D3DPT_POINTLIST failed.\n");
            bResult = false;
        }
    }

    // set point sprite size something larger than one (min of max point size will do nicely)

    m_Sprite.PointSize.f = MIN_MAXPOINTSIZE;
    SetRenderState(D3DRS_POINTSIZE, m_Sprite.PointSize.dw);

    // left/center clockwise quad

    if (!RenderPrimitive(D3DPT_TRIANGLESTRIP, MYD3DFVF_TLVERTEX, pQuadCW, 4, NULL, 0, NULL))
    {
        WriteToLog("RenderPrimitive(CW) D3DPT_TRIANGLESTRIP failed.\n");
        bResult = false;
    }

    // right/center counter clockwise quad

    if (!RenderPrimitive(D3DPT_TRIANGLESTRIP, MYD3DFVF_TLVERTEX, pQuadCCW, 4, NULL, 0, NULL))
    {
        WriteToLog("RenderPrimitive(CCW) D3DPT_TRIANGLESTRIP failed.\n");
        bResult = false;
    }

    return bResult;
}
