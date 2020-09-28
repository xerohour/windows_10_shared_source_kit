// FILE:        scalefvfls.cpp
// DESC:        scale class methods for point sprite conformance test
// AUTHOR:      Anthony W. Clarke
// COMMENTS:    D3DFVF_LVERTEX | D3DFVF_PSIZE

#include "sprite.h"

// NAME:        CSpriteScaleFVFLS()
// DESC:        constructor for sprite scale class (D3DFVF_LVERTEX | D3DFVF_PSIZE)
// INPUT:       none
// OUTPUT:      none

CSpriteScaleFVFLS::CSpriteScaleFVFLS(VOID)
{
    m_szTestName="Point sprite scale (D3DFVF_LVERTEX | D3DFVF_PSIZE)";
    m_szCommandKey="FVFLS";

    m_Group.Replace(0);
    m_Group.Save();
}

// NAME:        ~CSpriteScaleFVFLS()
// DESC:        destructor for sprite scale class (D3DFVF_LVERTEX | D3DFVF_PSIZE)
// INPUT:       none
// OUTPUT:      none

CSpriteScaleFVFLS::~CSpriteScaleFVFLS(VOID)
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CSpriteScaleFVFLS::CommandLineHelp(void)
{
    CSprite::CommandLineHelp();
    WriteCommandLineHelp("$yinvalid: $wrun invalid parameter tests$c($wYes$c/No)");
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteScaleFVFLS::TestInitialize(VOID)
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
        WriteToLog("Invalid param test.\n");
    }
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CSpriteScaleFVFLS::ExecuteTest(UINT uTest)
{
    bool bResult = true;
    D3DVALUE dvS, dvT;
    UINT uPosition, uSizes;

    uPosition = (uTest-1) % NCLIPTESTS;
    uSizes = m_nVariationsPerFormat/NCLIPTESTS;
    dvT = (D3DVALUE) (((uTest - 1) / NCLIPTESTS) % uSizes);
    dvT /= uSizes;
    dvS = dvT*(m_dvPointSizeMax * 1.5);

    // This is done to avoid precision errors and the ambiguity that
    // exists when point sprites are 0 - 1 in size. Also a larger size
    // allows greater error in applying textures.
    // Even numbered point sprite sizes are used so that an even number
    // of pixels is added to each side of the point sprite. This
    // along with SetPositionWorld avoid precision errors.
    dvS = (int)dvS;
    dvS = (dvS < MIN_POINTSPRITE_SIZE) ? MIN_POINTSPRITE_SIZE : dvS;
    if (((int)dvS) % 2)
        dvS++;

    // RS Point size is set in CSprite::ExecuteTest using m_Sprite.PointSize.f
    // Therefore set m_Sprite.PointSize.f to a non zero value that will show
    // if RS point size is being used rather than the size in the vertex.
    m_Sprite.PointSize.f = 10.0f;
    if (!CSprite::ExecuteTest(uTest))
        return false;

    sprintf(m_szBuffer, "Point size (vertex): %f, Format %s", dvS, m_szFormat);
    BeginTestCase(m_szBuffer, uTest);

    if (BeginScene())
    {
        DWORD dwFormat = MYD3DFVF_LSVERTEX;
        MYD3DLSVERTEX Vertex[3];

        m_Sprite.PointSize.f = dvS; // used in set position world.
        SetPositionWorld(uPosition, &Vertex[0].dvX, &Vertex[0].dvY);
        Vertex[0].dvZ = (float) EPSILON;
        Vertex[0].dvS = dvS;
        Vertex[0].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
        Vertex[0].dcSpecular = m_dcBackground;
        Vertex[0].dvTU = 0.0f;
        Vertex[0].dvTV = 0.0f;

        // +4 means with current array position, two point sprites can be diagonally opposite on screen
        SetPositionWorld((uPosition + 4) % NCLIPTESTS, &Vertex[1].dvX, &Vertex[1].dvY);
        Vertex[1].dvZ = (float) EPSILON;
        Vertex[1].dvS = dvS;
        Vertex[1].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
        Vertex[1].dcSpecular = m_dcBackground;
        Vertex[1].dvTU = 0.0f;
        Vertex[1].dvTV = 0.0f;

        // extra sprite was added to allow for larger matrix multiplication precision errors
        // (rendered at screen center so it is never clipped)
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
            m_Sprite.PointSize.f = 0; // reset

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

bool CSpriteScaleFVFLS::TestTerminate(VOID)
{
    if (bInvalid)
        m_Flags.Set(CF_INVALID);
    else
        m_Flags.Clear(CF_INVALID);

    CleanUpReg();
    return CSprite::TestTerminate();
}

bool CSpriteScaleFVFLS::PreModeSetup(void)
{
#ifdef MANBUG52003
    DWORD dwDisablePSGP = (DWORD) 1;
    HKEY hRegKey = NULL;

    if (SUCCEEDED(RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Direct3D",
                                 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hRegKey, NULL)))
    {
        if (FAILED(RegSetValueEx(hRegKey, "DisablePSGP", 0, REG_DWORD, (CONST BYTE *) &dwDisablePSGP,
                                 sizeof(dwDisablePSGP))))
        {
            WriteToLog("Unable to set ref rast reg key *** abort ***\n");
            RegCloseKey(hRegKey);
            return false;
        }

        RegCloseKey(hRegKey);
    }
    else
    {
        WriteToLog("Unable to create PSGP reg key *** abort ***\n");
        return false;
    }
#endif

    return true;
}

void CSpriteScaleFVFLS::CleanUpReg(void)
{
#ifdef MANBUG52003
    HKEY hRegKey = NULL;

    if (SUCCEEDED(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Direct3D",
                               0, KEY_ALL_ACCESS, &hRegKey)))
    {
        if (FAILED(RegDeleteValue(hRegKey, "DisablePSGP")))
            WriteToLog("Unable to restore PSGP reg key *** abort ***\n"));

        RegCloseKey(hRegKey);
    }
#endif

    return;
}
