// FILE:        dstmod.cpp
// DESC:        pixel shader dst mod conformance tests
//				These are now called output write masks
//				as they are in the SDK doco.
// AUTHOR:		Anthony Clarke
//		added 2.0	Bobby Paulose

#include <ddraw.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "PShader.h"

extern MYTLVERTEX g_FourColorVertices[];
extern MYTLVERTEXTEX2 g_FourColorVerticesTex2[];
extern MYTLVERTEXTEX4 g_FourColorVerticesTex4[];
extern MYTLVERTEXTEX6 g_FourColorVerticesTex6[];

CPSDestMod::CPSDestMod()
{
    m_szTestName = "Output Write Masks";
    m_szCommandKey = "WriteMask";
}

UINT CPSDestMod::TestInitialize()
{
	m_byFormats = TEXFORMAT;
    UINT uResult = CPixelShader::TestInitialize();
    if ( D3DTESTINIT_RUN != uResult )
		return uResult;

    PopulateShaderInfo();

    SetTestRange(1, m_vShaders.size() * m_uTotalFormats * m_uNumShaderVersions);

    return D3DTESTINIT_RUN;
}

HRESULT CPSDestMod::PopulateShaderInfo()
{
    float fConstants[16] = { 1.0f, 0.0f, 0.0f, 0.5f, 
							1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 1.0f, 0.0f, 0.5f,
							0.0f, 0.0f, 1.0f, 0.5f };
    
	// (1) Red, Green, Blue
	AddShaderDef("Write Mask - Red, Green, Blue",
                 "tex t0\n"
                 "mov r0, c0\n"
                 "mov r0.rgb, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 8);

    // (2) Alpha
	AddShaderDef("Write Mask - Alpha",
                 "tex t0\n"
                 "mov r0, t0\n"
                 "mov r0.a, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);

    // (3) Red, Green, Blue, Alpha
	AddShaderDef("Write Mask - Red, Green, Blue, Alpha",
                 "tex t0\n"
                 "mov r0, c1\n"
                 "mov r1, c0\n"
                 "mov r1.rgb, t0\n"
                 "mov r0.rgba, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddConstantsToLastShaderDef(fConstants, 8);


// *************************** Version 1.1 ***************************
	// Can use all the 1.0 shaders as 1.1


// *************************** Version 1.2 ***************************
	// Can use all the 1.0 and 1.1 shaders as 1.2


// *************************** Version 1.3 ***************************
	// Can use all the 1.0, 1.1 and 1.2 shaders as 1.3


// *************************** Version 1.4 ***************************

    // Red
	AddShaderDef("Write Mask - Red",
                 "texld r0, t0\n"
                 "mov r1, c0\n"
                 "mov r1.r, r0\n"
				 "mov r0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);

    // Green
	AddShaderDef("Write Mask - Green",
                 "texld r1, t0\n"
                 "mov r0, c2\n"
                 "mov r0.g, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex, 1);
    AddConstantsToLastShaderDef(fConstants, 16);

    // Blue
	AddShaderDef("Write Mask - Blue",
                 "texld r1, t0\n"
                 "mov r0, c3\n"
                 "mov r0.b, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 16);

	// Alpha
	AddShaderDef("Write Mask - Alpha",
                 "texld r0, t0\n"
                 "mov r0.a, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);

    // Red, Green
	AddShaderDef("Write Mask - Red, Green",
                 "texld r1, t0\n"
                 "mov r0, c0\n"
                 "mov r0.rg, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex, 1);
    AddConstantsToLastShaderDef(fConstants, 4);

    // Red, Blue
	AddShaderDef("Write Mask - Red, Blue",
                 "texld r1, t0\n"
                 "mov r0, c0\n"
                 "mov r0.rb, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);

	// Red, Alpha
	AddShaderDef("Write Mask - Red, Alpha",
                 "texld r0, t0\n"
                 "mov r0.ra, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 8);

	// Red, Green, Blue
	AddShaderDef("Write Mask - Red, Green, Blue",
                 "texld r0, t0\n"
                 "mov r1, c1\n"
				 "mov r1.rgb, r0\n"
                 "mov r0.rgb, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 8);

	// Red, Green, Alpha
	AddShaderDef("Write Mask - Red, Green, Alpha",
                 "texld r5, t5\n"
                 "mov r5.rga, c2\n"
				 "mov r0, r5\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex, 5);
    AddConstantsToLastShaderDef(fConstants, 16);

	 // Red, Blue, Alpha
	AddShaderDef("Write Mask - Red, Blue, Alpha",
                 "texld r2, t2\n"
                 "mov r2.rba, c3\n"
                 "mov r0, r2\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex, 2);
    AddConstantsToLastShaderDef(fConstants, 16);

    // Green, Blue
	AddShaderDef("Write Mask - Green, Blue",
                 "texld r0, t0\n"
                 "mov r1, c0\n"
                 "mov r1.gb, r0\n"
                 "mov r0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);

    // Green, Alpha
	AddShaderDef("Write Mask - Green, Alpha",
                 "texld r2, t2\n"
                 "mov r0, c2\n"
                 "mov r0.ga, r2\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex, 2);
    AddConstantsToLastShaderDef(fConstants, 12);

	// Green, Blue, Alpha
	AddShaderDef("Write Mask - Green, Blue, Alpha",
                 "texld r5, t5\n"
                 "mov r0, c3\n"
                 "mov r0.gba, r5\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex, 5);
    AddConstantsToLastShaderDef(fConstants, 16);

    // Blue, Alpha
	AddShaderDef("Write Mask - Blue, Alpha",
                 "texld r2, t2\n"
                 "mov r0, c3\n"
                 "mov r0.ba, r2\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex, 2);
    AddConstantsToLastShaderDef(&fConstants[12], 4);
    SetConstantsStart(3);

    
	// Red, Green, Blue, Alpha
	AddShaderDef("Write Mask - Red, Green, Blue, Alpha",
                 "texld r1, t1\n"
                 "mov r0, c1\n"
                 "mov r0.xyzw, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex, 1);
    AddConstantsToLastShaderDef(fConstants, 8);



// *************************** Version 2.0 ***************************
// *******************************************************************

	//single masks .r .g .b .a
	AddShaderDef("single masks .r .g .b .a",
                 "def c0, 0.45, 0.0, 0.0, 0.85\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "dcl t1.xy\n"
				 "dcl_2d s1\n"
				 "texld r0.rgba,t0,s0\n"
				 "texld r1,t1,s1\n"
				 "mov r2.r, c0\n"
				 "mov r2.g, r0\n"
				 "mov r2.b, r1\n"
				 "mov r2.a, c0\n"
				 "mov oC0, r2\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	//double masks .rg .rb .ra .gb .ga .ba
	AddShaderDef("double masks .rg .rb .ra .gb .ga .ba ",
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "dcl_2d s1\n"
				 "dcl_2d s2\n"
                 "def c0, 0.33, 0.33, 0.33, 1.0\n"
				 "texld r0.rgba,t0,s0\n"
				 "texld r1,t0,s1\n"
				 "texld r2,t0,s2\n"
				 "mov r0.a, c0\n"
				 "mov r1.a, c0\n"
				 "mov r2.a, c0\n"
				 "mov r3.rg, r0\n"
				 "mov r3.ba, r0\n"
				 "mov r4.rb, r1\n"
				 "mov r4.ga, r1\n"
				 "mov r5.ra, r2\n"
				 "mov r5.gb, r2\n"
				 "mul r3, r3, c0\n"
				 "mul r4, r4, c0\n"
				 "mul r5, r5, c0\n"
				 "add r0, r3, r4\n"
				 "add r0, r0, r5\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);


	//triple masks .rgb .rga .rba .gba
	AddShaderDef("triple masks .rgb .rga .rba .gba ",
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "dcl_2d s1\n"
				 "dcl_2d s2\n"
                 "def c0, 0.25, 0.25, 0.25, 1.0\n"
				 "texld r0.rgba,t0,s0\n"
				 "texld r1,t0,s1\n"
				 "texld r2,t0,s2\n"
				 "mov r0.a, c0\n"
				 "mov r1.a, c0\n"
				 "mov r2.a, c0\n"
				 "mov r3, c0\n"
				 "mov r4, c0\n"
				 "mov r5, c0\n"
				 "mov r3.rgb, r0\n"
				 "mov r4.rga, r1\n"
				 "mov r5.rba, r2\n"
				 "mov r0.gba, c0\n"
				 "mul r3, r3, c0\n"
				 "mul r4, r4, c0\n"
				 //"mul r5, r5, c0\n"
				 "mul r0, r0, c0\n"
				 "add r0, r0, r3\n"
				 "add r0, r0, r4\n"
				 "add r0, r0, r5\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	return S_OK;
}
