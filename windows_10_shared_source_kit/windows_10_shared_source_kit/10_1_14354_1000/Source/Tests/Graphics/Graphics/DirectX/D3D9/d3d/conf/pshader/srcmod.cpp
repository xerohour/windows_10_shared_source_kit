// FILE:        srcmod.cpp
// DESC:        pixel shader src mod conformance tests
// AUTHOR:      Anthony Clarke
//		added 2.0	Bobby Paulose 

#include <ddraw.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "PShader.h"

extern MYTLVERTEX g_FourColorVertices[];
extern MYTLVERTEXTEX2 g_FourColorVerticesTex2[];
extern MYTLVERTEXTEX4 g_FourColorVerticesTex4[];
extern MYTLVERTEXTEX6 g_FourColorVerticesTex6[];
extern MYTLVERTEX g_FourColorAlphaVertices[];
extern MYVOLTLVERTEX g_VolTexVertices[];
extern MYTLVERTEX3 g_Texm3Vertices[];


CPSSrcMod::CPSSrcMod()
{
    m_szTestName = "Argument Modifiers";
    m_szCommandKey = "ArgMod";

    m_pDP3Tex = NULL;
	m_pGradientTex = NULL;
}

void CPSSrcMod::ReleaseTextures()
{
    RELEASE(m_pDP3Tex);
	RELEASE(m_pGradientTex);
	CPixelShader::ReleaseTextures();
	return;
}

UINT CPSSrcMod::TestInitialize()
{
	m_byFormats = TEXFORMAT;
    UINT uResult = CPixelShader::TestInitialize();
    if ( D3DTESTINIT_RUN != uResult )
		return uResult;

    PopulateShaderInfo();

	SetTestRange(1, m_vShaders.size() * m_uTotalFormats * m_uNumShaderVersions);

    return D3DTESTINIT_RUN;
}

HRESULT CPSSrcMod::PopulateShaderInfo()
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

	float fConstants2[32] =
        {
            0.2f, 0.2f, 0.2f, 0.2f,
            0.2f, 0.2f, 0.2f, 0.2f,  
            0.4f, 0.4f, 0.4f, 0.4f,
            0.5f, 0.5f, 0.5f, 0.5f,
			0.6f, 0.6f, 0.6f, 0.6f,
            0.8f, 0.8f, 0.8f, 0.8f,
            0.9f, 0.9f, 0.9f, 0.9f,			
			0.9f, 0.9f, 0.9f, 0.9f
        };

	// ****************************** Texture Negate ******************************
	// -t0
    AddShaderDef(" -t0",
                 "tex t0\n"
				 "tex t1\n"
				 "mov r1, -t0\n"
                 "add r0, t1, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	// -t1
    AddShaderDef("-t1",
                 "tex t1\n"
                 "add r0, v0, -t1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	// -t2
    AddShaderDef("-t2",
                 "tex t2\n"
                 "sub r0, v0, -t2\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	// -t3
    AddShaderDef("-t3",
                 "tex t3\n"
                 "add r0, v0, -t3\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	// ****************************** Color Negate ******************************
	// -v0
    AddShaderDef("-v0",
                 "tex t0\n"
				 "add r0, t0, -v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// -v1
    AddShaderDef("-v1",
				 "tex t0\n" 
				 "add r0, t0, -v1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// ****************************** Texture Invert ******************************
    // 1-t0
    AddShaderDef("1-t0",
                 "tex t0\n"
                 "mov r0, 1-t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	// 1-t1
    AddShaderDef("1-t1",
                 "tex t1\n"
                 "mov r0, 1-t1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	// 1-t2
    AddShaderDef("1-t2",
                 "tex t2\n"
                 "mov r0, 1-t2\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	// 1-t3
    AddShaderDef("1-t3",
                 "tex t3\n"
                 "mov r0, 1-t3\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	// ****************************** Color Invert ******************************
	// 1-v0
    AddShaderDef("1-v0",
                 "mov r0, 1-v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// 1-v1
    AddShaderDef("1-v1",
                 "mov r0, 1-v1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// ****************************** Texture Alpha Replicate ******************************
	// t0.a
    AddShaderDef("Alpha Replicate t0",
                 "tex t0\n"
                 "mul r0, v0, t0.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// t1.a
	AddShaderDef("Alpha Replicate t1",
                 "tex t1\n"
                 "mul r0, v0, t1.a\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// t2.a
	AddShaderDef("Alpha Replicate t2",
                 "tex t2\n"
                 "mul r0, v0, t2.a\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, true);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// t3.a
	AddShaderDef("Alpha Replicate t3",
                 "tex t3\n"
                 "mul r0, v0, t3.a\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, true);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// ****************************** Color Alpha Replicate ******************************
	
	// v0.a
    AddShaderDef("v0.a",
				 "tex t0\n"
                 "mul r0, v0.a, t0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	// v1.a
    AddShaderDef("v1.a",
				 "tex t0\n"
                 "mul r0, v1.a, t0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	
	// ****************************** Constant Alpha Replicate ******************************
	// c0.a
    AddShaderDef("c0.a",
                 "mul r0, v0, c0.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);

	// c1.a
    AddShaderDef("c1.a",
                 "mul r0, v0, c1.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);

	// c2.a
    AddShaderDef("c2.a",
                 "mul r0, v0, c2.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);

	// c3.a
    AddShaderDef("c3.a",
                 "mul r0, v0, c3.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);
	
	// c4.a
    AddShaderDef("c4.a",
                 "mul r0, v0, c4.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);

	// c5.a
    AddShaderDef("c5.a",
                 "mul r0, v0, c5.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);

	// c6.a
    AddShaderDef("c6.a",
                 "mul r0, v0, c6.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);

	// c7.a
    AddShaderDef("c7.a",
                 "mul r0, v0, c7.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);

	// ****************************** Bias Texture ******************************
	// t0_bias
	AddShaderDef("t0_bias",
                 "tex t0\n"
                 "add r0, v0, t0_bias\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddConstantsToLastShaderDef(fConstants, 24);

	// t1_bias
	AddShaderDef("t1_bias",
                 "tex t1\n"
                 "add r0, v0, t1_bias\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddConstantsToLastShaderDef(fConstants, 24);

	// t2_bias
	AddShaderDef("t2_bias",
                 "tex t2\n"
                 "add r0, v0, t2_bias\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	AddConstantsToLastShaderDef(fConstants, 24);

	// t3_bias
	AddShaderDef("t3_bias",
                 "tex t3\n"
                 "add r0, v0, t3_bias\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	AddConstantsToLastShaderDef(fConstants, 24);

	// ****************************** Bias Color ******************************
	// v0_bias
    AddShaderDef("v0_bias",
                 "mov r0, v0_bias\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);

	// v1_bias
    AddShaderDef("v1_bias",
                 "mov r0, v1_bias\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);

	// ****************************** Sign Scaling Texture ******************************
	// t0_bx2 
	AddShaderDef("t0_bx2",
                 "tex t0\n"
                 "mov r0, t0_bx2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// t1_bx2 
	AddShaderDef("t1_bx2",
                 "tex t1\n"
                 "mov r0, t1_bx2\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// t2_bx2 
	AddShaderDef("t2_bx2",
                 "tex t2\n"
                 "mov r0, t2_bx2\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// t3_bx2 
	AddShaderDef("t3_bx2",
                 "tex t3\n"
                 "mov r0, t3_bx2\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// ****************************** Sign Scaling Color ******************************
	// v0_bx2 
	AddShaderDef("v0_bx2",
                 "mov r0, v0_bx2\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices);

	// v1_bx2 
	AddShaderDef("v1_bx2",
                 "mov r0, v1_bx2\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices);


	// ****************************** Testing known hardware issues. ******************************

	//1) Nvidia Gef3 can't do bias on negative values.
    AddShaderDef("negative value bias",
				 "def c0, -0.5f, -0.5f, -0.5f, -0.5f\n"
                 "mov_sat r1, c0\n"
                 "mov r0, r1_bias\n"
				 "mov r0, -r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);

// *************************** Version 1.1 ***************************
	// Can use all the 1.0 shaders as 1.1
	// ****************************** Texture Blue to Alpha ******************************
	// t0.b
    AddShaderDef("Blue to Alpha t0",
                 "tex t0\n"
				 "tex t1\n"
				 "mov r0, t1\n"
                 "mov r0.a, t0.b\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 1));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	
	// t3.b
	AddShaderDef("Blue to Alpha t3",
                 "tex t0\n"
				 "tex t3\n"
				 "mov r0.rgb, t0\n"
                 "+ mov r0.a, t3.b\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, true, SHADE_PLAIN, PSVERSION(1, 1));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex,3);

	// ****************************** Color Blue to Alpha ******************************
	// v0.b
    AddShaderDef("Blue to Alpha v0",
				 "tex t0\n"
				 "mov r0.rgb, t0\n"
                 "+ mov r0.a, v0.b\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, true, SHADE_PLAIN, PSVERSION(1, 1));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// v1.b
    AddShaderDef("Blue to Alpha v1",
				 "tex t0\n"
				 "mov r0.rgb, t0\n"
                 "+ mov r0.a, v1.b\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, true, SHADE_PLAIN, PSVERSION(1, 1));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	
	// ****************************** Constant Blue to Alpha ******************************
	// c2.b
    AddShaderDef("Blue to Alpha c2",		
				 "mov r0.rgb, v0\n"
                 "+mov r0.a, c2.b\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 1));
    AddConstantsToLastShaderDef(fConstants, 32);

	// c7.b
    AddShaderDef("Blue to Alpha c7",
                 "mov r0.rgb, v0\n"
                 "+mov r0.a, c7.b\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 1));
    AddConstantsToLastShaderDef(fConstants, 32);

	// ****************************** Texcoord Blue to Alpha ******************************
	// t0.b
	AddShaderDef("Blue to Alpha texcoord t0",
				 "texcoord t0 \n"
                 "mov r0.a, t0.b \n"
				 "mov r0.rgb, r0.a \n",
                 MYVOLTLVERTEX_FVF, g_VolTexVertices, PSVERSION(1, 1));

	// ****************************** Register Blue to Alpha ******************************
	// r1.b
	AddShaderDef("Blue to Alpha r1",
				 "texcoord t0 \n"
				 "mov r1, t0 \n"
                 "mov r0.a, r1.b \n"
				 "mov r0.rgb, r0.a \n",
                 MYVOLTLVERTEX_FVF, g_VolTexVertices, PSVERSION(1, 1));

// *************************** Version 1.2 ***************************
	// Can use all the 1.0 and 1.1 shaders as 1.2

// *************************** Version 1.3 ***************************
	// Can use all the 1.0, 1.1 and 1.2 shaders as 1.3


// *************************** Version 1.4 ***************************
	// ****************************** Texture Negate ******************************
	// -r0
    AddShaderDef(" -r0",
                 "texld r0, t0\n"
				 "texld r1, t1\n"
                 "add r0, r1, -r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	// -r5
    AddShaderDef("-r5",
                 "texld r5, t5\n"
                 "add r0, v0, -r5\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
	AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	// ****************************** Color Negate ******************************
	// -v0
    AddShaderDef("-v0",
                 "texld r0, t0\n"
				 "add r0, r0, -v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// -v1
    AddShaderDef("-v1",
				 "texld r0, t0\n" 
				 "add r0, r0, -v1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// ****************************** Texture Invert ******************************
    // 1-r0
    AddShaderDef("1-r0",
                 "texld r0, t0\n"
                 "mov r0, 1-r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	// 1-r5
    AddShaderDef("1-r5",
                 "texld r5, t5\n"
                 "mov r0, 1-r5\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
	AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);    
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// ****************************** Color Invert ******************************
	// 1-v0
    AddShaderDef("1-v0",
                 "mov r0, 1-v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// 1-v1
    AddShaderDef("1-v1",
                 "mov r0, 1-v1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// ****************************** Texture Alpha Replicate ******************************
	// t0.a
    AddShaderDef("Alpha Replicate t0",
                 "texld r0, t0\n"
                 "mul r0, v0, r0.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);


	// t5.a
	AddShaderDef("Alpha Replicate t5",
                 "texld r5, t5\n"
                 "mul r0, v0, r5.a\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// ****************************** Color Alpha Replicate ******************************
	
	// v0.a
    AddShaderDef("v0.a",
				 "texld r0, t0\n"
                 "mul r0, v0.a, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	// v1.a
    AddShaderDef("v1.a",
				 "texld r0, t0\n"
                 "mul r0, v1.a, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	
	// ****************************** Constant Alpha Replicate ******************************
	// c0.a
    AddShaderDef("c0.a",
                 "mul r0, v0, c0.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);

	// c7.a
    AddShaderDef("c7.a",
                 "mul r0, v0, c7.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);

	// ****************************** Bias Texture ******************************
	// r0_bias
	AddShaderDef("r0_bias",
                 "texld r0, t0\n"
                 "add r0, v0, r0_bias\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddConstantsToLastShaderDef(fConstants, 24);

	// r5_bias
	AddShaderDef("r5_bias",
                 "texld r5, t5\n"
                 "add r0, v0, r5_bias\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	AddConstantsToLastShaderDef(fConstants, 24);

	// ****************************** Bias Color ******************************
	// v0_bias
    AddShaderDef("v0_bias",
                 "mov r0, v0_bias\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));

	// v1_bias
    AddShaderDef("v1_bias",
                 "mov r0, v1_bias\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));

	// ****************************** Sign Scaling Texture ******************************
	// r0_bx2 
	AddShaderDef("r0_bx2",
                 "texld r0, t0\n"
                 "mov r0, r0_bx2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// r5_bx2 
	AddShaderDef("r5_bx2",
                 "texld r5, t5\n"
                 "mov r0, r5_bx2\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// ****************************** Sign Scaling Color ******************************
	// v0_bx2 
	AddShaderDef("v0_bx2",
                 "mov r0, v0_bx2\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));

	// v1_bx2 
	AddShaderDef("v1_bx2",
                 "mov r0, v1_bx2\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));

	// ****************************** Texture x2 ******************************
	// r0_x2 
	AddShaderDef("r0_x2",
                 "texld r0, t0\n"
                 "mov r0, r0_x2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// r5_x2 
	AddShaderDef("r5_x2",
                 "texld r5, t5\n"
                 "mov r0, r5_x2\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// ****************************** Color x2 ******************************
	// v0_x2 
	AddShaderDef("v0_x2",
                 "mov r0, v0_x2\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));

	// v1_x2 
	AddShaderDef("v1_x2",
                 "mov r0, v1_x2\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));

	// ****************************** Texture Red Replicate ******************************
	// r0.r
    AddShaderDef("Red Replicate r0",
                 "texld r0, t0\n"
                 "mul r0, v0, r0.r\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);


	// r5.r
	AddShaderDef("Red Replicate r5",
                 "texld r5, t5\n"
                 "mul r0, v0, r5.r\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// ****************************** Color Red Replicate ******************************	
	// v0.r
    AddShaderDef("v0.r",
				 "texld r0, t0\n"
                 "mul r0, v0.r, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	// v1.r
    AddShaderDef("v1.r",
				 "texld r0, t0\n"
                 "mul r0, v1.r, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	
	// ****************************** Constant Red Replicate ******************************
	// c0.r
    AddShaderDef("c0.r",
                 "mul r0, v0, c0.r\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);

	// c7.r
    AddShaderDef("c7.r",
                 "mul r0, v0, c7.r\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);

	// ****************************** Texture Green Replicate ******************************
	// r0.g
    AddShaderDef("Green Replicate r0",
                 "texld r0, t0\n"
                 "mul r0, v0, r0.g\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);


	// r5.g
	AddShaderDef("Green Replicate r5",
                 "texld r5, t5\n"
                 "mul r0, v0, r5.g\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// ****************************** Color Green Replicate ******************************	
	// v0.g
    AddShaderDef("v0.g",
				 "texld r0, t0\n"
                 "mul r0, v0.g, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	// v1.g
    AddShaderDef("v1.g",
				 "texld r0, t0\n"
                 "mul r0, v1.g, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	
	// ****************************** Constant Green Replicate ******************************
	// c0.g
    AddShaderDef("c0.g",
                 "mul r0, v0, c0.g\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);

	// c7.g
    AddShaderDef("c7.g",
                 "mul r0, v0, c7.g\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);


	// ****************************** Texture Blue Replicate ******************************
	// r0.b
    AddShaderDef("Blue Replicate r0",
                 "texld r0, t0\n"
                 "mul r0, v0, r0.b\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// r5.b
	AddShaderDef("Blue Replicate r5",
                 "texld r5, t5\n"
                 "mul r0, v0, r5.b\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// ****************************** Color Blue Replicate ******************************	
	// v0.b
    AddShaderDef("v0.b",
				 "texld r0, t0\n"
                 "mul r0, v0.b, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	// v1.b
    AddShaderDef("v1.b",
				 "texld r0, t0\n"
                 "mul r0, v1.b, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	
	// ****************************** Constant Blue Replicate ******************************
	// c0.b
    AddShaderDef("c0.b",
                 "mul r0, v0, c0.b\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);

	// c7.b
    AddShaderDef("c7.b",
                 "mul r0, v0, c7.b\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);

	// ****************************** Texcoord Blue to Alpha ******************************
	// t0.b
	AddShaderDef("Blue to Alpha texcrd t0",
				 "texcrd r1.rgb, t0.xyz \n"
                 "mov r0.a, r1.b \n"
				 "mov r0.rgb, r0.a \n",
                MYVOLTLVERTEX_FVF, g_VolTexVertices, PSVERSION(1, 4));

	// t5.b
	AddShaderDef("Blue to Alpha texcrd t5",
				 "texcrd r1.rgb, t5.xyz \n"
                 "mov r0.a, r1.b \n"
				 "mov r0.rgb, r0.a \n",
                MYTLVERTEX3_FVF, g_Texm3Vertices, PSVERSION(1, 4));

// *************************** Version 2.0 ***************************
// *******************************************************************

	//negate
    AddShaderDef("negate",
                 "def c0, 0.0, 0.0, 0.0, 1.0\n"
                 "def c1, 1.0, 1.0, 1.0, 1.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "texld r0.rgba,t0,s0\n"
				 "add r1, -r0, -c0\n"
				 "mul r1, r1, -c1\n"
				 "mov oC0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);


	//replicate  .r .rrrr etc
    AddShaderDef("replicate .r .rrrr etc",
                 "def c0, 0.33, 0.33, 0.33, 1.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "texld r0.rgba,t0,s0\n"
				 "mov r1, r0.r\n"
				 "mov r2, r0.gggg\n"
				 "mov r3, r0.b\n"
				 "mul r1, r1, c0\n"
				 "mul r2, r2, c0\n"
				 "mul r3, r3, c0\n"
				 "mov r0, r1\n"
				 "add r0, r0, r2\n"
				 "add r0, r0, r3\n"
				 "mov r0.a, c0.aaaa\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);


	//swizzle .rgba .gbra .brga .abgr 
    AddShaderDef("swizzle .rgba .gbra .brga .abgr",
                 "def c0, 0.25, 0.25, 0.25, 0.25\n"
				 "dcl t0.xy\n"
				 "dcl v0.rgba\n"
				 "dcl_2d s0\n"
				 "dcl_2d s1\n"
				 "dcl_2d s2\n"
				 "texld r0,t0,s0\n"
				 "texld r1,t0,s1\n"
				 "texld r2,t0,s2\n"

				 "mov r3, r0.rgba\n"
				 "mov r4, r1.gbra\n"
				 "mov r5, r2.brga\n"
				 "mov r6, v0.abgr\n"
				 "mul r3, r3, c0\n"
				 "mul r4, r4, c0\n"
				 "mul r5, r5, c0\n"
				 "mul r6, r6, c0\n"

				 "mov r0, r3\n"
				 "add r0, r0, r4\n"
				 "add r0, r0, r5\n"
				 "add r0, r0, r6\n"
				 "mov oC0, r0\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);


	//swizzle .rgba .gbra .brga .abgr with write mask
    AddShaderDef("swizzle .rgba .gbra .brga .abgr with write mask",
                 "def c0, 0.25, 0.25, 0.25, 0.25\n"
                 "def c1, 0.0, 0.0, 0.0, 0.0\n"
				 "dcl t0.xy\n"
				 "dcl v0.rgba\n"
				 "dcl_2d s0\n"
				 "dcl_2d s1\n"
				 "dcl_2d s2\n"
				 "texld r0,t0,s0\n"
				 "texld r1,t0,s1\n"
				 "texld r2,t0,s2\n"

				 //initialize r3 r4 r5 r6
				 "mov r3, c1\n"
				 "mov r4, c1\n"
				 "mov r5, c1\n"
				 "mov r6, c1\n"

				 "mov r3.rg, r0.rgba\n"
				 "mov r4.ba, r1.gbra\n"
				 "mov r5.ra, r2.brga\n"
				 "mov r6.a, v0.abgr\n"
				 "mul r3, r3, c0\n"
				 "mul r4, r4, c0\n"
				 "mul r5, r5, c0\n"
				 "mul r6, r6, c0\n"

				 "mov r0, r3\n"
				 "add r0, r0, r4\n"
				 "add r0, r0, r5\n"
				 "add r0, r0, r6\n"
				 "mov oC0, r0\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);

	// ****************************** Texcoord Blue to Alpha ******************************
	// t0.b
	AddShaderDef("Blue to Alpha dcl t0",
				 "dcl t0.xyz \n"
                 "mov r0.a, t0.b \n"
				 "mov r0.rgb, r0.a \n"
				 "mov oC0, r0 \n",
                 MYVOLTLVERTEX_FVF, g_VolTexVertices, PSVERSION(2, 0));

	// t5.b
	AddShaderDef("Blue to Alpha dcl t5",
				 "dcl t5.xyz \n"
                 "mov r0.a, t5.b \n"
				 "mov r0.rgb, r0.a \n"
				 "mov oC0, r0 \n",
                 MYTLVERTEX3_FVF, g_Texm3Vertices, PSVERSION(2, 0));


    return S_OK;
}

bool CPSSrcMod::GenerateTextureData()
{
	// DP3
    if ( !m_DP3Image.Load("DP3.bmp", 1.0f) )
    {
        WriteToLog(_T("(ERROR) Failed to load DP3.bmp.  Aborting.\n"));
        return false;
    }

	// Gradient
	DWORD dwColors[4] =
    {
        D3DRGBA(0, 1, 0, 1),
        D3DRGBA(0, 1, 0, 1),
        D3DRGBA(1, 1, 1, 0.5),
        D3DRGBA(1, 1, 1, 0.5)
    };
    if (NULL == (m_GradientImage.LoadGradient(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwColors)))
    {
        WriteToLog(_T("(ERROR) Failed to create gradient data.  Aborting.\n"));
        return false;
    }
	return CPixelShader::GenerateTextureData();
}

bool CPSSrcMod::LoadTextures()
{       
	if (!SetTextureFormat(TEXFORMAT))
		return false;	

    DWORD dwLevels = (DWORD) 1;
    DWORD dwPool = POOL_DEFAULT;
    DWORD dwUsage = (DWORD) 0;
    FORMAT *pFormat = &m_pTexFormats[m_pCurrentMode->nTextureFormat];

	// DP3
    if (!CreateTexture(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pDP3Tex))
    {
        WriteToLog(_T("(ERROR) Failed to create DP3 texture.  Aborting.\n"));
        return false;
    }
    if (!m_pDP3Tex->LoadImage(dwLevels, &m_DP3Image))
    {
        WriteToLog(_T("Unable to load DP3 texture.\n"));
        return false;
    }
    
	//Gradient
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
