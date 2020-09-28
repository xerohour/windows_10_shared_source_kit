// FILE:        specular.cpp
// DESC:        WLP specular conformance test (basic RGB rasterization specular highlights)

#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "specular.h"

CD3DWindowFramework App;
CSpecular           Test;

MYCOLOR gpColors[] =
{
    { "black",   { 0.0f, 0.0f, 0.0f, 1.0f } },
    { "red",     { 1.0f, 0.0f, 0.0f, 1.0f } },
    { "green",   { 0.0f, 1.0f, 0.0f, 1.0f } },
    { "blue",    { 0.0f, 0.0f, 1.0f, 1.0f } },
    { "gray",    { 0.5f, 0.5f, 0.5f, 1.0f } },
    { "yellow",  { 1.0f, 1.0f, 0.0f, 1.0f } },
    { "cyan",    { 0.0f, 1.0f, 1.0f, 1.0f } },
    { "magenta", { 1.0f, 0.0f, 1.0f, 1.0f } },
    { "white",   { 1.0f, 1.0f, 1.0f, 1.0f } }
};

// NAME:        CSpecular()
// DESC:        constructor
// INPUT:       none
// OUTPUT:      none

CSpecular::CSpecular()
{
    SetTestType(TESTTYPE_CONF);

    m_szTestName = "WLP Specular Highlighting";
    m_szCommandKey = "Range";

    ClearStatus();

    m_pStates = NULL;

    m_pImage = NULL;
    m_pTexture = NULL;

    m_Options.ModeOptions.dwTexInclude = PF_RGB;
    m_Options.ModeOptions.dwTexExclude = (PF_PALETTEINDEXED1 |
                                          PF_PALETTEINDEXED2 |
                                          PF_PALETTEINDEXED4 |
                                          PF_PALETTEINDEXED8 |
                                          PF_PALETTEINDEXEDTO8);

    m_Options.ModeOptions.CubeFmt = FMT_ANY;
    m_Options.ModeOptions.VolumeFmt = FMT_ANY;
    m_Options.ModeOptions.ZBufferFmt = FMT_ANY;

    m_Options.D3DOptions.bZBuffer = true;
    m_Options.D3DOptions.dwWinMode = (WINMODE_WINDOWED | WINMODE_FULLSCREEN);

    if (KeySet("Texture"))
    {
        m_pImage = new CImage;

        if (!m_pImage)
        {
            WriteToLog("Unable to create image.\n");
            return;
        }

        if (!m_pImage->Load("256x256SmallChecker.bmp", 0.5f))
        {
            WriteToLog("Unable to load image.\n");
            SAFEDELETE(m_pImage);
        }
    }
}

// NAME:        ~CSpecular()
// DESC:        destructor
// INPUT:       none
// OUTPUT:      none

CSpecular::~CSpecular()
{
    SAFEDELETE(m_pImage);
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CSpecular::CommandLineHelp(VOID)
{
    WriteCommandLineHelp("$y-Texture $wEnable texture");
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      true.....if caps check is successful
//              false....otherwise

bool CSpecular::CapsCheck(VOID)
{
    bool bSuccess = true;

    if (!(m_pSrcDevice->GetCaps()->dwShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB))
    {
        WriteToLog("Device must support Gouraud shading.\n");
        WriteToLog("D3DPSHADECAPS_COLORGOURAUDRGB must be set.\n");
        bSuccess = false;
    }

    if (!(m_pSrcDevice->GetCaps()->dwShadeCaps & D3DPSHADECAPS_SPECULARGOURAUDRGB))
    {
        WriteToLog("Device must support specular Gouraud shading.\n");
        WriteToLog("D3DPSHADECAPS_SPECULARGOURAUDRGB must be set.\n");
        bSuccess = false;
    }

    return bSuccess;
}

// NAME:        Setup()
// DESC:        setup
// INPUT:       none
// OUTPUT:      true.....if setup is successful
//              false....otherwise

bool CSpecular::Setup(VOID)
{
    if (!SetupMatrices())
    {
        WriteToLog("Unable to set matrices.\n");
        return false;
    }

    if (KeySet("Texture"))
    {
        if (!m_pImage)
            return false;

        DWORD dwLevels = (DWORD) 1;
        DWORD dwPool = FMWK_DEFAULT;
        DWORD dwUsage = (DWORD) 0;
        FORMAT *pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

        if (!CreateTexture(256, 256, dwLevels, dwUsage, *pFormat, dwPool, &m_pTexture))
        {
            WriteToLog("Unable to create texture: %s.\n", GLE(GetLastError()));
            return false;
        }

        if (!m_pTexture->LoadImage(dwLevels, &m_pImage))
        {
            WriteToLog("Unable to load texture.\n");
            return false;
        }
    }

    m_pStates = new CStateCascade;

    if (!m_pStates)
    {
        WriteToLog("Unable to create state cascade.\n");
        return false;
    }

    m_pStates->AddEntry(KEY_LIGHT, KEY_LIGHT, 9, true);
    m_pStates->AddEntry(KEY_POWER, KEY_POWER, POWERS, true);
    m_pStates->AddEntry(KEY_SPECULAR, KEY_SPECULAR, 9, true);
    m_pStates->AddEntry(KEY_MATERIAL, KEY_MATERIAL, 9, true);

    m_Sphere.NewShape(CS_SPHERE, 32, 16, 0);
    m_Sphere.Scale(4.0);

    return true;
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpecular::TestInitialize(VOID)
{
    SetTestRange(1, (UINT) m_pStates->StateCount());
    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test was executed successfully
//              false....otherwise

bool CSpecular::ExecuteTest(UINT uTest)
{
    LIGHT Light;
    MATERIAL Material;

    m_pStates->CascadeToState(uTest - 1);
  
    Material.dcvEmissive.r = 0.0f;
    Material.dcvEmissive.g = 0.0f;
    Material.dcvEmissive.b = 0.0f;
    Material.dcvEmissive.a = 0.0f;

    Material.dcvAmbient.r = 0.0f;
    Material.dcvAmbient.g = 0.0f;
    Material.dcvAmbient.b = 0.0f;
    Material.dcvAmbient.a = 0.0f;

    Material.dcvDiffuse.r = gpColors[m_pStates->GetEntryState(KEY_MATERIAL)].dcvColor.r;
    Material.dcvDiffuse.g = gpColors[m_pStates->GetEntryState(KEY_MATERIAL)].dcvColor.g;
    Material.dcvDiffuse.b = gpColors[m_pStates->GetEntryState(KEY_MATERIAL)].dcvColor.b;
    Material.dcvDiffuse.a = gpColors[m_pStates->GetEntryState(KEY_MATERIAL)].dcvColor.a;

    Material.dcvSpecular.r = 3.0f*gpColors[m_pStates->GetEntryState(KEY_SPECULAR)].dcvColor.r;
    Material.dcvSpecular.g = 3.0f*gpColors[m_pStates->GetEntryState(KEY_SPECULAR)].dcvColor.g;
    Material.dcvSpecular.b = 3.0f*gpColors[m_pStates->GetEntryState(KEY_SPECULAR)].dcvColor.b;
    Material.dcvSpecular.a = 3.0f*gpColors[m_pStates->GetEntryState(KEY_SPECULAR)].dcvColor.a;

    Material.dvPower = (float) (BASEPOWER << m_pStates->GetEntryState(KEY_POWER));

    if (!SetMaterial(&Material))
    {
        WriteToLog("Unable to set material: %s.\n", GLE(GetLastError()));
        return false;
    }
    
    Light.dltType = D3DLIGHT_POINT;

    Light.dcvAmbient.r = 0.0f;
    Light.dcvAmbient.g = 0.0f;
    Light.dcvAmbient.b = 0.0f;
    Light.dcvAmbient.a = 0.0f;

    Light.dcvDiffuse.r = gpColors[m_pStates->GetEntryState(KEY_LIGHT)].dcvColor.r;
    Light.dcvDiffuse.g = gpColors[m_pStates->GetEntryState(KEY_LIGHT)].dcvColor.g;
    Light.dcvDiffuse.b = gpColors[m_pStates->GetEntryState(KEY_LIGHT)].dcvColor.b;
    Light.dcvDiffuse.a = gpColors[m_pStates->GetEntryState(KEY_LIGHT)].dcvColor.a;

    Light.dcvSpecular.r = 1.0f;
    Light.dcvSpecular.g = 1.0f;
    Light.dcvSpecular.b = 1.0f;
    Light.dcvSpecular.a = 1.0f;

    Light.dvPosition.x = 0.0f;
    Light.dvPosition.y = 0.0f;
    Light.dvPosition.z = -5.0f;

    Light.dvDirection.x = 0.0;
    Light.dvDirection.y = 0.0;
    Light.dvDirection.z = 0.0;

    Light.dvAttenuation0 = 1.0f;
    Light.dvAttenuation1 = 0.0f;
    Light.dvAttenuation2 = 0.0f;

    Light.dvFalloff = 0.0f;
    Light.dvPhi = 0.0f;
    Light.dvRange = 50.0f;
    Light.dvTheta = 0.0f;

    if (!SetLight((DWORD) 0, &Light))
    {
        WriteToLog("Unable to set light: %s.\n", GLE(GetLastError()));
        return false;
    }
    else if (!LightEnable((DWORD) 0, TRUE))
    {
        WriteToLog("Unable to enable light: %s.\n", GLE(GetLastError()));
        return false;
    }
 
    SetRenderState(D3DRS_AMBIENT, (DWORD) 0);

	if (KeySet("False"))
		SetRenderState(D3DRS_SPECULARENABLE, (DWORD) FALSE);
	else
		SetRenderState(D3DRS_SPECULARENABLE, (DWORD) TRUE);
    
    CHAR szBuffer[128];
    sprintf(szBuffer, "Light: %-8s Material: %-8s Specular: %-8s Power: %d",
            gpColors[m_pStates->GetEntryState(KEY_LIGHT)].szName,
            gpColors[m_pStates->GetEntryState(KEY_MATERIAL)].szName,
            gpColors[m_pStates->GetEntryState(KEY_SPECULAR)].szName,
            BASEPOWER << m_pStates->GetEntryState(KEY_POWER));
    BeginTestCase(szBuffer, uTest);

    return true;
}

// NAME:        SceneRefresh()
// DESC:        draws the scene 
// INPUT:       none
// OUTPUT:      none 

VOID CSpecular::SceneRefresh(VOID)
{
    DWORD dwFormat = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);

    if (BeginScene())
    {
        if (m_pTexture)
        {
            SetTexture(0, m_pTexture);
            SetTextureStageState(0, D3DTSS_COLOROP, (DWORD) D3DTOP_MODULATE);
            SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
            SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD) D3DTA_SPECULAR);
            SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG2);
            SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
            SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD) D3DTA_DIFFUSE);
            SetTexture(1, NULL);
        }
            
        if (!RenderPrimitive(D3DPT_TRIANGLELIST, dwFormat,
                             m_Sphere.m_pVertices, m_Sphere.m_nVertices,
                             m_Sphere.m_pIndices, m_Sphere.m_nIndices,
                             NULL))
        {
            WriteToLog("RenderPrimitive() failed: %s.\n", GLE(GetLastError()));
            Fail();
        }

        if (!EndScene())
        {
            WriteToLog("EndScene() failed: %s.\n", GLE(GetLastError()));
            Fail();
        }
    }
    else
    {
        WriteToLog("BeginScene() failed: %s.\n", GLE(GetLastError()));
        Fail();
    }

    sprintf(m_szStatus[0][0], "$gLight\t");
    sprintf(m_szStatus[0][1], "$y%s", gpColors[m_pStates->GetEntryState(KEY_LIGHT)].szName);
    sprintf(m_szStatus[1][0], "$gPower\t");
    sprintf(m_szStatus[1][1], "$y%d", BASEPOWER << m_pStates->GetEntryState(KEY_POWER));
    sprintf(m_szStatus[2][0], "$gSpecular");
    sprintf(m_szStatus[2][1], "$y%s", gpColors[m_pStates->GetEntryState(KEY_SPECULAR)].szName);
    sprintf(m_szStatus[3][0], "$gMaterial\t");
    sprintf(m_szStatus[3][1], "$y%s", gpColors[m_pStates->GetEntryState(KEY_MATERIAL)].szName);
    sprintf(m_szStatus[4][0], "$gTexture\t");
    sprintf(m_szStatus[4][1], "$y%s", (m_pTexture ? "enabled" : "disabled"));
}

// NAME:        Cleanup()
// DESC:        terminate test
// INPUT:       none
// OUTPUT:      true

VOID CSpecular::Cleanup(VOID)
{
    SAFEDELETE(m_pStates);
    RELEASE(m_pTexture);
}

// NAME:        UpdateStatus()
// DESC:        update status
// INPUT:       none
// OUTPUT:      none

VOID CSpecular::UpdateStatus(VOID)
{
    CD3DTest::UpdateStatus();

    for (UINT i = 0; i < NSTATUS; i++)
        WriteStatus(m_szStatus[i][0], m_szStatus[i][1]);
}

// NAME:        ClearStatus()
// DESC:        clear status
// INPUT:       none
// OUTPUT:      none

VOID CSpecular::ClearStatus(VOID)
{
    for (UINT i = 0; i < NSTATUS; i++)
    {
        sprintf(m_szStatus[i][0], "$gNA\t");
        sprintf(m_szStatus[i][1], "$y-");
    }
}

// NAME:        SetupMatrices()
// DESC:        setup matrices
// INPUT:       none
// OUTPUT:      true.....if matrices set successfully
//              false....otherwise

bool CSpecular::SetupMatrices(VOID)
{
    D3DXMATRIX Matrix;
    D3DVALUE dvAngle = (float) pi/2.0f;
    D3DVALUE dvAspect = ((float) m_pCurrentMode->nBackBufferWidth) /
                        ((float) m_pCurrentMode->nBackBufferHeight);
    D3DXVECTOR3 From(0.0f, 0.0f, -3.0f);
    D3DXVECTOR3 At(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f); 

    D3DXMatrixIdentity(&Matrix);

    if (!SetTransform(D3DTS_WORLD, (D3DMATRIX *) &Matrix))
    {
        WriteToLog("SetTransform(WORLD) failed: %s.\n", GLE(GetLastError()));
        return false;
    }

    D3DXMatrixPerspectiveFovLH(&Matrix, dvAngle, dvAspect, 1.0f, 20.0f);

    float fTemp = (float) 1.0/Matrix.m[2][3];

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            Matrix.m[i][j] *= fTemp;

    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
    {
        WriteToLog("SetTransform(PROJECTION) failed: %s.\n", GLE(GetLastError()));
        return false;
    }
    
    D3DXMatrixLookAtLH(&Matrix, &From, &At, &Up);

    if (!SetTransform(D3DTS_VIEW, (D3DMATRIX *) &Matrix))
    {
        WriteToLog("SetTransform(VIEW) failed: %s.\n", GLE(GetLastError()));
        return false;
    }

    return true;
}
