// FILE:        Sizes.cpp
// COMMENTS:    WLP texture size conformance test

#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "sizes.h"

CD3DWindowFramework App;
CSizes              Test;

MYCOLOR gpColors[] =
{
    { "red",     WLP_RED     },
    { "green",   WLP_GREEN   },
    { "blue",    WLP_BLUE    },
    { "gray",    WLP_GRAY    },
    { "yellow",  WLP_YELLOW  },
    { "cyan",    WLP_CYAN    },
    { "magenta", WLP_MAGENTA },
    { "white",   WLP_WHITE   }
};

// NAME:        CSizes()
// DESC:        constructor
// INPUT:       none
// OUTPUT:      none

CSizes::CSizes()
{
    SetTestType(TESTTYPE_CONF);

    m_szTestName = "WLP Texture Sizes";
    m_szCommandKey = "Range";

    m_pImage = NULL;
    m_pTexture = NULL;

    m_Options.ModeOptions.CubeFmt = FMT_ANY;
    m_Options.ModeOptions.VolumeFmt = FMT_ANY;
    m_Options.ModeOptions.ZBufferFmt = FMT_ANY;
    m_Options.ModeOptions.dwTexInclude = PF_RGB;
    m_Options.ModeOptions.dwTexExclude = (PF_PALETTEINDEXED1 |
                                          PF_PALETTEINDEXED2 |
                                          PF_PALETTEINDEXED4 |
                                          PF_PALETTEINDEXED8 |
                                          PF_PALETTEINDEXEDTO8);

    m_Options.D3DOptions.bZBuffer = true;
    m_Options.D3DOptions.dwWinMode = (WINMODE_WINDOWED | WINMODE_FULLSCREEN);
}

// NAME:        ~CSizes()
// DESC:        destructor
// INPUT:       none
// OUTPUT:      none

CSizes::~CSizes()
{
    SAFEDELETE(m_pImage);
    RELEASE(m_pTexture);
}


// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      true.....if caps check is successful
//              false....otherwise

bool CSizes::CapsCheck(VOID)
{
    bool bPass = true;

    if ((m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY) ||
        (m_pSrcDevice->GetCaps()->dpcLineCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY))
    {
        WriteToLog("Device must support non-square textures for triangles/lines.\n");
        WriteToLog("D3DPTEXTURECAPS_SQUAREONLY must NOT be set.\n");
        bPass = false;
    }

    if ((m_pSrcDevice->GetCaps()->dwMinTextureWidth != 1) ||
        (m_pSrcDevice->GetCaps()->dwMinTextureHeight != 1))
    {
        WriteToLog("Device must support a minimum texture size of 1x1.\n");
        WriteToLog("Reported: dwMinTextureWidth=%d, dwMinTextureHeight=%d.\n",
                   m_pSrcDevice->GetCaps()->dwMinTextureWidth,
                   m_pSrcDevice->GetCaps()->dwMinTextureHeight);
        bPass = false;
    }

    m_dwMaxWidthPow2 = MaxPowerOf2(m_pSrcDevice->GetCaps()->dwMaxTextureWidth);
    m_dwMaxHeightPow2 = MaxPowerOf2(m_pSrcDevice->GetCaps()->dwMaxTextureHeight);
    WriteToLog("(Info) Device supports POW2 textures up to %dx%d.\n",
               1 << m_dwMaxWidthPow2, 1 << m_dwMaxHeightPow2);

	if (KeySet("Mobile"))
	{
		if ((m_dwMaxWidthPow2 < 8) || (m_dwMaxHeightPow2 < 8))
		{
			WriteToLog("Device must support maximum texture size of at least 256x256.\n");
			bPass = false;
		}
	}
	else
	{
		if ((m_dwMaxWidthPow2 < 10) || (m_dwMaxHeightPow2 < 10))
		{
			WriteToLog("Device must support maximum texture size of at least 1024x1024.\n");
			bPass = false;
		}
	}

	// Scale down to a 2048x2048 max, otherwise the space hit is huge
    if ((m_dwMaxWidthPow2 > 11) || (m_dwMaxHeightPow2 > 11))
	{
        WriteToLog("(Info) Clamping textures to 2048x2048 for space and time considerations.\n");
		m_dwMaxWidthPow2 = 11;
		m_dwMaxHeightPow2 = 11;
	}

    m_dwMaxAspectRatio = m_pSrcDevice->GetCaps()->dwMaxTextureAspectRatio;

    if (0 == m_dwMaxAspectRatio)
        WriteToLog("(Info) Max aspect ratio NOT restricted.\n");
    else
        WriteToLog("(Info) Max aspect ratio restricted to %d:1.\n", m_dwMaxAspectRatio);

    if (!(m_pSrcDevice->GetCaps()->dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB))
    {
        WriteToLog("Device must support Gouraud shading.\n");
        WriteToLog("D3DPSHADECAPS_COLORGOURAUDRGB must be set.\n");
        bPass = false;
    }

    return bPass;
}

// NAME:        Setup()
// DESC:        setup
// INPUT:       none
// OUTPUT:      true.....if setup is successful
//              false....otherwise

bool CSizes::Setup(VOID)
{
    if (!SetupMatrices())
    {
        WriteToLog("Unable to set matrices.\n");
        return false;
    }

    // scan for WLP formats:  (8:8:8:8), (4:4:4:4), (1:5:5:5)

    int n1555Format = -1;
    int n4444Format = -1;
	int n8888Format = -1;

    for (int i = 0; i < m_uCommonTextureFormats; i++)
    {
        PIXELFORMAT Format = m_pCommonTextureFormats[i].ddpfPixelFormat;

        if ((Format.dwFlags & PF_RGB) && (Format.dwFlags & PF_ALPHAPIXELS))
        {
            DWORD dwBits[4];

            dwBits[0] = Bits(Format.dwRGBAlphaBitMask);
            dwBits[1] = Bits(Format.dwRBitMask);
            dwBits[2] = Bits(Format.dwGBitMask);
            dwBits[3] = Bits(Format.dwBBitMask);

			if ((dwBits[0] == 8) && (dwBits[1] == 8) && (dwBits[2] == 8) && (dwBits[3] == 8))
				n8888Format = i;

			if ((dwBits[0] == 4) && (dwBits[1] == 4) && (dwBits[2] == 4) && (dwBits[3] == 4))
                n4444Format = i;

            if ((dwBits[0] == 1) && (dwBits[1] == 5) && (dwBits[2] == 5) && (dwBits[3] == 5))
                n1555Format = i;
        }
    }

    if (n1555Format == -1)
    {
        WriteToLog("Device does NOT support a 1:5:5:5 ARGB texture format.\n");
        return false;
    }

    if (n4444Format == -1)
    {
        WriteToLog("Device does NOT support a 4:4:4:4 ARGB texture format.\n");
        return false;
    }

	if (!KeySet("Mobile"))
	{
		if (n8888Format == -1)
		{
			WriteToLog("Device does NOT support a 8:8:8:8 ARGB texture format.\n");
			return false;
		}
	}

    m_pCurrentMode->nTextureFormat = n4444Format;
    return true;
}

// NAME:        TestInitialize()
// DESC:        test initialize
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT* code

UINT CSizes::TestInitialize(VOID)
{
    m_nSeed = GetTickCount();
    srand(m_nSeed);

    // 2 variations per combination of sizes from 1x1 to maxwidth/maxheight

    SetTestRange(1, (UINT) (2*(m_dwMaxWidthPow2 + 1)*(m_dwMaxHeightPow2 + 1)));
    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test executed successfully
//              false....otherwise

bool CSizes::ExecuteTest(UINT uTest)
{
    bool bResult = true;
    DWORD pdwColors[4];
    DWORD dwFirstColor = (uTest - 1)/((m_dwMaxWidthPow2 > m_dwMaxHeightPow2) ? m_dwMaxWidthPow2 : m_dwMaxHeightPow2) * 2 % 8;
    DWORD dwSecondColor = uTest % 8;

    if (dwFirstColor == dwSecondColor)
        dwSecondColor = (uTest + 1) % 8;

    if ((uTest & 1) == 1)
    {
        pdwColors[0] = pdwColors[1] = gpColors[dwFirstColor].dwColor;
        pdwColors[2] = pdwColors[3] = gpColors[dwSecondColor].dwColor;
    } else {
        pdwColors[0] = pdwColors[2] = gpColors[dwFirstColor].dwColor;
        pdwColors[1] = pdwColors[3] = gpColors[dwSecondColor].dwColor;
    }

    m_dwWidth = 1 << ((uTest-1) / ((m_dwMaxWidthPow2+ 1) * 2));
    m_dwHeight = 1 << (((uTest-1) % ((m_dwMaxHeightPow2+1) * 2)) / 2);

    DWORD dwActualAspectRatio;
    dwActualAspectRatio = (m_dwWidth > m_dwHeight) ? m_dwWidth/m_dwHeight : m_dwHeight/m_dwWidth;

    sprintf(m_szDesc,"%dx%d with %s gradient from %s to %s.", 
            m_dwWidth, m_dwHeight,
            ((uTest & 1) == 1) ? "vertical" : "horizontal",
            gpColors[dwFirstColor].szName,
            gpColors[dwSecondColor].szName);

    if ((m_dwMaxAspectRatio > 0) && (dwActualAspectRatio > m_dwMaxAspectRatio))
    {
        WriteToLog("%s exceeds devices's stated max texture aspect ratio.\n", m_szDesc);
        SkipTests(2);
        return false;
    }

    BeginTestCase(m_szDesc, uTest);

    SetTexture(0, NULL);
    SAFEDELETE(m_pImage);
    RELEASE(m_pTexture);
    m_pImage = new CImage;

    if (!m_pImage)
    {
        WriteToLog("Unable to create gradient image.\n");
        return false;
    }

    if (!m_pImage->LoadGradient(m_dwWidth, m_dwHeight, pdwColors))
    {
        WriteToLog("Unable to load gradient image.\n");
        return false;
    }

    DWORD dwLevels = (DWORD) 1;
    DWORD dwPool = FMWK_DEFAULT;
    DWORD dwUsage = (DWORD) 0;
    FORMAT *pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

    if (!CreateTexture(m_dwWidth, m_dwHeight, dwLevels, dwUsage, *pFormat, dwPool, &m_pTexture))
    {
        if (D3DERR_OUTOFVIDEOMEMORY == GetLastError())
        {
            if ((m_dwWidth > 256) || (m_dwHeight > 256))
            {
                
                WriteToLog("Insufficient video memory for %s.\n", m_szDesc);
                SkipTests(1);
            }
        }
        else
        {
            WriteToLog("Unable to create gradient texture for %s (%s).\n",
                       m_szDesc, GLE(GetLastError()));
        }
        
        return false;
    }

    if (!m_pTexture->LoadImage(dwLevels, &m_pImage))
    {
        WriteToLog("Unable to load gradient texture.\n");
        return false;
    }

    return true;
}

// NAME:        Cleanup()
// DESC:        cleanup
// INPUT:       none
// OUTPUT:      none

VOID CSizes::Cleanup(VOID)
{
    RELEASE(m_pTexture);
}

// NAME:        SceneRefresh()
// DESC:        scene refresh
// INPUT:       none
// OUTPUT:      none 

VOID CSizes::SceneRefresh(VOID)
{
    if (BeginScene())
    {
        D3DTLVERTEX Vertices[4];
        DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
        DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;
  
        Vertices[0].dvSX = 0.0f; 
        Vertices[0].dvSY = (float) (dwHeight - 1); 
        Vertices[0].dvSZ = 0.5f;
        Vertices[0].dvRHW = 2.0f;
        Vertices[0].dvTU = 0.0f;
        Vertices[0].dvTV = 0.995f;

        Vertices[1].dvSX = 0.0f; 
        Vertices[1].dvSY = 0.0f; 
        Vertices[1].dvSZ = 0.5f;
        Vertices[1].dvRHW = 2.0f;
        Vertices[1].dvTU = 0.0f;
        Vertices[1].dvTV = 0.0f;

        Vertices[2].dvSX = (float) (dwWidth - 1); 
        Vertices[2].dvSY = (float) (dwHeight - 1); 
        Vertices[2].dvSZ = 0.5f;
        Vertices[2].dvRHW = 2.0f;
        Vertices[2].dvTU = 0.995f;
        Vertices[2].dvTV = 0.995f;

        Vertices[3].dvSX = (float) (dwWidth - 1); 
        Vertices[3].dvSY = 0.0f; 
        Vertices[3].dvSZ = 0.5f;
        Vertices[3].dvRHW = 2.0f;
        Vertices[3].dvTU = 0.995f;
        Vertices[3].dvTV = 0.0f;
 
        LOG(SetTexture(0, m_pTexture));
        LOG(SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1));
        LOG(SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
        LOG(SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE));
        LOG(SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1));
        LOG(SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
        LOG(SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_DIFFUSE));
        LOG(SetTextureStageState(0, D3DTSS_MAGFILTER, (DWORD)TEXF_POINT));

        LOG(SetTexture(1, NULL));
        LOG(SetTextureStageState(1, D3DTSS_COLOROP, (DWORD)D3DTOP_DISABLE));
        LOG(SetTextureStageState(1, D3DTSS_ALPHAOP, (DWORD)D3DTOP_DISABLE));

        if (!RenderPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, Vertices, 4, NULL, 0, NULL))
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
}

// NAME:        SetupMatrices()
// DESC:        setup matrices
// INPUT:       none
// OUTPUT:      true.....if matrices set successfully
//              false....otherwise

bool CSizes::SetupMatrices(VOID)
{
    D3DXMATRIX Matrix;
    D3DVALUE dvAngle = (float) pi/2.0f;
    D3DVALUE dvAspect = ((float) m_pCurrentMode->nBackBufferWidth) /
                        ((float) m_pCurrentMode->nBackBufferHeight);
    D3DXVECTOR3 From(0.0f, 0.0f, -10.0f);
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
