#include <d3d9.h>
#include <d3dx9.h>
#include "PShader.h"

extern MYTLVERTEX g_FourBrickVertices[];
extern MYTLVERTEX g_FourColorVertices[];
extern MYTLVERTEX g_FourColorAlphaVertices[];
extern MYTLVERTEXTEX4 g_FourColorVerticesTex4[];

CPSLegacy::CPSLegacy()
{
    m_szTestName = "Legacy pixel shader behaviors";
    m_szCommandKey = "Legacy";

	// Force test to use relevant DirectX versions
	m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_81;

	// textures
    m_pChecker2Tex = NULL;
    m_pGradientTex = NULL;
}

void CPSLegacy::ReleaseTextures()
{
    RELEASE(m_pChecker2Tex);
    RELEASE(m_pGradientTex);
	CPixelShader::ReleaseTextures();
	return;
}

UINT CPSLegacy::TestInitialize()
{
	m_byFormats = TEXFORMAT;
	UINT uResult = CPixelShader::TestInitialize();
    if ( D3DTESTINIT_RUN != uResult )
		return uResult;

    PopulateShaderInfo();

    SetTestRange(1, 1 + (m_vShaders.size() * m_uTotalFormats * m_uNumShaderVersions));

    return D3DTESTINIT_RUN;
}

HRESULT CPSLegacy::PopulateShaderInfo()
{
    float fConstants[32] =
        {
            0.0f, 0.0f, 0.0f, 0.0f,
            0.2f, 0.2f, 0.2f, 0.2f,  
            0.4f, 0.4f, 0.4f, 0.4f,
            0.5f, 0.5f, 0.5f, 0.5f,
			0.6f, 0.6f, 0.6f, 0.6f,
            0.8f, 0.8f, 0.8f, 0.8f,
            0.9f, 0.9f, 0.9f, 0.9f,			
			1.2f, 1.2f, 1.2f, 1.2f
        };

	float fConstZero[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float fConstLow[4]  = { 0.25f, 0.25f, 0.25f, 0.25f };
	float fConstHalf[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
	float fConstHigh[4] = { 0.75f, 0.75f, 0.75f, 0.75f };

	///////////////////////////// PS 1.0 to PS 1.3////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////

	// def constant behavior using dp3_x2
	AddShaderDef("def constant behavior, dp3_x2",
				 "def c0, 0.1f, 0.1f, 0.1f, 0.4f\n"
                 "tex t0\n"
				 "dp3_x2 r0, c0, t0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);
    AddConstantsToLastShaderDef(fConstHigh);

	// def constant behavior using dp3_x4
	AddShaderDef("def constant behavior, dp3_x4",
				 "def c0, 0.1f, 0.1f, 0.1f, 0.4f\n"
                 "tex t0\n"
				 "dp3_x4 r0, c0, t0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);
    AddConstantsToLastShaderDef(fConstHalf);

	// Legacy shaders
	if (m_dwVersion < 0x0900)
	{
		////////////////////////////////////////////////////////////////////////////////////
		// Legacy Address Shaders
		////////////////////////////////////////////////////////////////////////////////////

		// texcoord 2D
		AddShaderDef("Address: texcoord 2D",
					 "texcoord t0\n"
					 "mov r0, t0\n",
					 MYTLVERTEX_FVF, g_FourColorAlphaVertices, true);

		// Coord Defaults
		AddShaderDef("Address: texcoord Defaults",
					 "tex t1\n"
					 "texcoord t3\n"
					 "add r0, t1, t3_bias\n",
					 MYTLVERTEX_FVF, g_FourColorAlphaVertices, true, SHADE_PLAIN, PSVERSION(1, 1));
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
		SetCoordsForLastPSStages(2, 2);
		SetCoordsForLastPSStages(3, 3);
		SetCoordsForLastPSStages(4, 4);
		SetCoordsForLastPSStages(5, 5);

		////////////////////////////////////////////////////////////////////////////////////
		// Legacy ReadShader Shaders
		////////////////////////////////////////////////////////////////////////////////////

		// neg and bx2 the same constant. 
		AddShaderDef("ReadShader: Neg, bx2 and bias of constant",
					 "def c3, -0.25f, 0.75f, 0.75f, 1.0f\n"
					 "tex t0\n"
					 "mad r0, t0, c3_bx2, -c3_bias\n",
					 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, true, SHADE_PLAIN, PSVERSION(1, 1));
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);		

		// Test that cmp is >= 0.0
		AddShaderDef("ReadShader: cmp >= 0.0",
					 "def c0, 0.4f, 0.5f, 0.5f, 0.4f\n"
					 "tex t0\n"
					 "tex t1\n"
					 "cmp r0, c0_bx2, t0, t1\n",
					 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, true, SHADE_PLAIN, PSVERSION(1, 2));
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);		
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

		////////////////////////////////////////////////////////////////////////////////////
		// Legacy InsMod Shaders
		////////////////////////////////////////////////////////////////////////////////////

		// nop_x2
		AddShaderDef("InsMod: nop_x2",
					 "tex t0\n" 
					 "nop_x2\n"
					 "mov r0, t0\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

		// nop_x4
		AddShaderDef("InsMod: nop_x4",
					 "tex t0\n" 
					 "nop_x4\n"
					 "mov r0, t0\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

		// nop_d2
		AddShaderDef("InsMod: nop_d2",
					 "tex t0\n" 
					 "nop_d2\n"
					 "mov r0, t0\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

		// nop_sat
		AddShaderDef("InsMod: nop_sat",
					 "tex t0\n" 
					 "nop_sat\n"
					 "mov r0, t0\n",
					 MYTLVERTEX_FVF, g_FourBrickVertices);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

		////////////////////////////////////////////////////////////////////////////////////
		// Legacy ArgMod Shaders
		////////////////////////////////////////////////////////////////////////////////////

		// ****************************** Constant Negate ******************************

		// -c0
		AddShaderDef("ArgMod: -c0",
					 "tex t0\n"
					 "add r0, t0, -c0\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
		AddConstantsToLastShaderDef(fConstants, 32);
		
		// -c1
		AddShaderDef("ArgMod: -c1",
					 "tex t0\n"
					 "add r0, t0, -c1\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
		AddConstantsToLastShaderDef(fConstants, 32);

		// -c2
		AddShaderDef("ArgMod: -c2",
					 "tex t0\n"
					 "add r0, t0, -c2\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
		AddConstantsToLastShaderDef(fConstants, 32);

		// -c3
		AddShaderDef("ArgMod: -c3",
					 "tex t0\n"
					 "add r0, t0, -c3\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
		AddConstantsToLastShaderDef(fConstants, 32);

		// -c4
		AddShaderDef("ArgMod: -c4",
					 "tex t0\n"
					 "add r0, t0, -c4\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
		AddConstantsToLastShaderDef(fConstants, 32);

		// -c5
		AddShaderDef("ArgMod: -c5",
					 "tex t0\n"
					 "add r0, t0, -c5\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
		AddConstantsToLastShaderDef(fConstants, 32);

		// -c6
		AddShaderDef("ArgMod: -c6",
					 "tex t0\n"
					 "add r0, t0, -c6\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
		AddConstantsToLastShaderDef(fConstants, 32);

		// -c7
		AddShaderDef("ArgMod: -c7",
					 "tex t0\n"
					 "add r0, t0, -c7\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
		AddConstantsToLastShaderDef(fConstants, 32);

		// ****************************** Constant Invert ******************************

		// 1-c0
		AddShaderDef("ArgMod: 1-c0",
					 "add r0, v0, 1-c0\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// 1-c1
		AddShaderDef("ArgMod: 1-c1",
					 "add r0, v0, 1-c1\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// 1-c2
		AddShaderDef("ArgMod: 1-c2",
					 "add r0, v0, 1-c2\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// 1-c3
		AddShaderDef("ArgMod: 1-c3",
					 "add r0, v0, 1-c3\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// 1-c4
		AddShaderDef("ArgMod: 1-c4",
					 "add r0, v0, 1-c4\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// 1-c5
		AddShaderDef("ArgMod: 1-c5",
					 "add r0, v0, 1-c5\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// 1-c6
		AddShaderDef("ArgMod: 1-c6",
					 "add r0, v0, 1-c6\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// 1-c7
		AddShaderDef("ArgMod: 1-c7",
					 "add r0, v0, 1-c7\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// ****************************** Bias Constant ******************************

		// c0_bias
		AddShaderDef("ArgMod: c0_bias",
					 "add r0, v0, c0_bias\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// c1_bias
		AddShaderDef("ArgMod: c1_bias",
					 "add r0, v0, c1_bias\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// c2_bias
		AddShaderDef("ArgMod: c2_bias",
					 "add r0, v0, c2_bias\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// c3_bias
		AddShaderDef("ArgMod: c3_bias",
					 "add r0, v0, c3_bias\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// c4_bias
		AddShaderDef("ArgMod: c4_bias",
					 "add r0, v0, c4_bias\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// c5_bias
		AddShaderDef("ArgMod: c5_bias",
					 "add r0, v0, c5_bias\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// c6_bias
		AddShaderDef("ArgMod: c6_bias",
					 "add r0, v0, c6_bias\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// c7_bias
		AddShaderDef("ArgMod: c7_bias",
					 "add r0, v0, c7_bias\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// ****************************** Sign Scaling Constant ******************************

		// c0_bx2
		AddShaderDef("ArgMod: c0_bx2",
					 "add r0, v0, c0_bx2\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// c1_bx2
		AddShaderDef("ArgMod: c1_bx2",
					 "add r0, v0, c1_bx2\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// c2_bx2
		AddShaderDef("ArgMod: c2_bx2",
					 "add r0, v0, c2_bx2\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// c3_bx2
		AddShaderDef("ArgMod: c3_bx2",
					 "add r0, v0, c3_bx2\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// c4_bx2
		AddShaderDef("ArgMod: c4_bx2",
					 "add r0, v0, c4_bx2\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// c5_bx2
		AddShaderDef("ArgMod: c5_bx2",
					 "add r0, v0, c5_bx2\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// c6_bx2
		AddShaderDef("ArgMod: c6_bx2",
					 "add r0, v0, c6_bx2\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);

		// c7_bx2
		AddShaderDef("ArgMod: c7_bx2",
					 "add r0, v0, c7_bx2\n",
					 MYTLVERTEX_FVF, g_FourColorVertices);
		AddConstantsToLastShaderDef(fConstants, 32);
	}

	///////////////////////////// PS 1.4/////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////

	// def constant behavior using dp3_x2
	AddShaderDef("def constant behavior, dp3_x2",
                 "def c0, 0.1f, 0.1f, 0.1f, 0.4f\n"
                 "texld r0, t0\n"				 
				 "dp3_x2 r0, c0, r0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);
    AddConstantsToLastShaderDef(fConstHigh);

	// def constant behavior using dp3_x4
	AddShaderDef("def constant behavior, dp3_x4",
                 "def c0, 0.1f, 0.1f, 0.1f, 0.4f\n"
                 "texld r0, t0\n"				 
				 "dp3_x4 r0, c0, r0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);
    AddConstantsToLastShaderDef(fConstHalf);

	// def constant behavior using dp3_x8
	AddShaderDef("def constant behavior, dp3_x8",
                 "def c0, 0.1f, 0.1f, 0.1f, 0.4f\n"
				 "texld r0, t0\n"
				 "dp3_x8 r0, c0, r0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);
    AddConstantsToLastShaderDef(fConstLow);

	// Legacy shaders
	if (m_dwVersion < 0x0900)
	{
		////////////////////////////////////////////////////////////////////////////////////
		// Legacy InsMod Shaders
		////////////////////////////////////////////////////////////////////////////////////

		// nop_sat
		AddShaderDef("nop_sat",
					 "texld r0, t0\n" 
					 "nop_sat\n"
					 "mov r0, r0\n",
					 MYTLVERTEX_FVF, g_FourBrickVertices, PSVERSION(1, 4));
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	}

    return S_OK;
}

bool CPSLegacy::ExecuteTest(UINT uiTest)
{
	if (uiTest == 1)
	{
		BeginTestCase("MaxPixelShaderValue check");

		// Do a special pixel shader 1.4 check for the MaxPixelShaderValue
		if (((PSVERSION(1,4) <= m_pSrcDevice->GetCaps()->dwPixelShaderVersion) && (PSVERSION(0,0) == m_dwRequestedVersion))
			 || (PSVERSION(1,4) == m_dwRequestedVersion))
		{
			if (m_dwVersion < 0x0900)
			{
				// For Dx8, the minimum value is 8.0
				if (m_pSrcDevice->GetCaps()->dvMaxPixelShaderValue < 8.0f)
				{
					WriteToLog( _T("On Dx8, MaxPixelShaderValue: %f, must be >= 8.0 for pixel shader version 1.4.\n"), m_pSrcDevice->GetCaps()->dvMaxPixelShaderValue);
					Fail();
					return false;
				}
			}
		}

		Pass();
		return false;
	}

	return CPixelShader::ExecuteTest(uiTest);
}

bool CPSLegacy::GenerateTextureData()
{
	// Checker2
	if ( !m_Checker2Image.Load("checker2.bmp", 1.0f) )
    {
        WriteToLog(_T("(ERROR) Failed to load checker2.bmp.  Aborting.\n"));
        return false;
    }

	// Gradient
	DWORD dwColors[4] =
    {
        D3DRGBA(0, 1, 0, 1),
        D3DRGBA(0, 1, 0, 1),
        D3DRGBA(1, 1, 1, 0),
        D3DRGBA(1, 1, 1, 0)
    };
    if ( !m_GradientImage.LoadGradient(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwColors) )
    {
        WriteToLog(_T("(ERROR) Failed to create gradient data.  Aborting.\n"));
        return false;
    }

	return CPixelShader::GenerateTextureData();
}

bool CPSLegacy::LoadTextures()
{       
	if (!SetTextureFormat(TEXFORMAT))
		return false;
    
    DWORD dwLevels = (DWORD) 1;
    DWORD dwPool = POOL_DEFAULT;
    DWORD dwUsage = (DWORD) 0;
    FORMAT *pFormat = &m_pTexFormats[m_pCurrentMode->nTextureFormat];

    // Checker2
    if (!CreateTexture(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pChecker2Tex))
    {
        WriteToLog(_T("(ERROR) Failed to create checker2 texture.  Aborting.\n"));
        return false;
    }
    if (!m_pChecker2Tex->LoadImage(dwLevels, &m_Checker2Image))
    {
        WriteToLog(_T("Unable to load checker2 texture.\n"));
        return false;
    }

    // Gradient
    if (!CreateTexture(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pGradientTex))
    {
        WriteToLog(_T("(ERROR) Failed to create gradient texture.  Aborting.\n"));
        return false;
    }
    if (!m_pGradientTex->LoadImage(dwLevels, &m_GradientImage))
    {
        WriteToLog(_T("Unable to load gradient texture.\n"));
        return false;
    }

    return CPixelShader::LoadTextures();
}
