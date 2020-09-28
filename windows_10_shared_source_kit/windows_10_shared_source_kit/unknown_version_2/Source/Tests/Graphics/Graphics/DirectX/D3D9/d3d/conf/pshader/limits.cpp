// FILE:        limits.cpp
// DESC:        pixel shader conformance tests for max instruction limit, max register read port limit,
//				max texture stage limit etc.
//				
// Created:		Bobby Paulose  April 30 2002.

#include <ddraw.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "PShader.h"

extern MYTLVERTEX g_FourColorVertices[];
extern MYTLVERTEXTEX4 g_FourColorVerticesTex4[];
extern MYTLVERTEXTEX6 g_FourColorVerticesTex6[];
extern MYTLVERTEXTEX8 g_FourColorVerticesTex8[];
extern MYTLVERTEXPERSP g_FourColorVerticesMipBias[];

CPSLimits::CPSLimits()
{
    m_szTestName = "Max Instr, register read port, texture stage limits";
    m_szCommandKey = "Limits";

	//textures
    m_pGradientTex = NULL;
    m_pChecker2Tex = NULL;
	m_pStripesTex = NULL;
}

void CPSLimits::ReleaseTextures()
{
    RELEASE(m_pGradientTex);
    RELEASE(m_pChecker2Tex);
    RELEASE(m_pStripesTex);
	CPixelShader::ReleaseTextures();
	return;
}

UINT CPSLimits::TestInitialize()
{
	m_byFormats = TEXFORMAT;
	UINT uResult = CPixelShader::TestInitialize();
    if ( D3DTESTINIT_RUN != uResult )
		return uResult;

    PopulateShaderInfo();

    SetTestRange(1, m_vShaders.size() * m_uTotalFormats * m_uNumShaderVersions);

    return D3DTESTINIT_RUN;
}

HRESULT CPSLimits::PopulateShaderInfo()
{


	///////////////////////////// PS1.0/////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////

	//For version 1.0 only, the tex instruction does not count toward the 
	//total number of instructions, although it does count toward the 
	//total number of texture address instructions. 

	//max instructions 8 + 1 tex
	AddShaderDef("max instructions 8 + 1 tex",
                 "def c0, 0.142857, 0.142857, 0.142857, 0.142857\n"
                 "tex t0\n"
                 "mul r1, t0, c0\n"
                 "mov r0, r1\n"
                 "add r0, r0, r1\n"
                 "add r0, r0, r1\n"
                 "add r0, r0, r1\n"
                 "add r0, r0, r1\n"
                 "add r0, r0, r1\n"
                 "add r0, r0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, m_dwMinPSVersion);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);


	//max instructions 8 + 4 tex
	AddShaderDef("max instructions 8 + 4 tex",
                 "def c0, 0.25, 0.25, 0.25, 0.25\n"
                 "def c1, 0.33, 0.33, 0.33, 0.33\n"
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mul r1, t0, c0\n"
                 "mad r1, t1, c0, r1\n"
                 "mad r1, t2, c0, r1\n"
                 "mad r1, t3, c0, r1\n"

                 "mul r1, r1, c1\n"
                 "mov r0, r1\n"
                 "add r0, r0, r1\n"
                 "add r0, r0, r1\n",

                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, m_dwMinPSVersion);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);


	//max instructions 8 + co-issue 8 + 4 tex
	AddShaderDef("max instructions 8 + co-issue 8 + 4 tex",
                 "def c0, 0.25, 0.25, 0.25, 0.0\n"
                 "def c1, 0.33, 0.33, 0.33, 0.0\n"
                 "def c2, 0.0, 0.0, 0.0, 0.125\n"
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mul r1.rgb, t0, c0\n"
					"+ mov r1.a, c2.a\n"
                 "mad r1.rgb, t1, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mad r1.rgb, t2, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mad r1.rgb, t3, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"

                 "mul r1.rgb, r1, c1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mov r0.rgb, r1\n"
					"+ add r0.a, r1.a, c2.a\n"
                 "add r0.rgb, r0, r1\n"
					"+ add r0.a, r0.a, c2.a\n"
                 "add r0.rgb, r0, r1\n"
					"+ add r0.a, r0.a, c2.a\n",

                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, m_dwMinPSVersion);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);



	///////////////////////////// PS1.4/////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////

	//max instructions 8 + co-issue 8 + 6 tex
	AddShaderDef("max instructions 8 + co-issue 8 + 6 tex",
                 "def c0, 0.16667, 0.16667, 0.16667, 0.0\n"
                 "def c1, 1.0, 1.0, 1.0, 0.0\n"
                 "def c2, 0.0, 0.0, 0.0, 0.125\n"
                 "texld r0, t0\n"
                 "texld r1, t1\n"
                 "texld r2, t1\n"
                 "texld r3, t3\n"
                 "texld r4, t4\n"
                 "texld r5, t5\n"
                 "mul r1.rgb, r1, c0\n"
					"+ mov r1.a, c2.a\n"
                 "mad r1.rgb, r0, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mad r1.rgb, r2, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mad r1.rgb, r3, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mad r1.rgb, r4, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mad r1.rgb, r5, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"

                 "mul r1.rgb, r1, c1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mov r0.rgb, r1\n"
					"+ add r0.a, r1.a, c2.a\n",

                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1,4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);


	//instr8+co-issue8+6tex* + phase + instr8+co-issue8+6tex*
	AddShaderDef("instr8+co-issue8+6tex* + phase + instr8+co-issue8+6tex*",
                 "def c0, 0.091, 0.091, 0.091, 0.0\n"
                 "def c1, 1.0, 1.0, 1.0, 0.0\n"
                 "def c2, 0.0, 0.0, 0.0, 0.125\n"
                 "def c3, 1.5, 1.5, 1.5, 0.0\n"
                 "texld r0, t0\n"
                 "texld r1, t1\n"
                 "texld r2, t1\n"
                 "texld r3, t3\n"
                 "texld r4, t4\n"
                 "texld r5, t5\n"
                 "mul r1.rgb, r1, c0\n"
					"+ mov r1.a, c2.a\n"
                 "mad r1.rgb, r0, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mad r1.rgb, r2, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mad r1.rgb, r3, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mad r1.rgb, r4, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mad r1.rgb, r5, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"

                 "mul r1.rgb, r1, c1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mov r0.rgb, r1\n"
					"+ add r0.a, r1.a, c2.a\n"

 				 "phase\n"

                 "texkill t0\n"
                 "texld r1, t1\n"
                 "texld r2, t1\n"
                 "texld r3, t3\n"
                 "texcrd r4.rgb, t4\n"
                 "texld r5, t5\n"
                 "mul r1.rgb, r1, c0\n"
					"+ mov r1.a, c2.a\n"
                 "add r1.rgb, r0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mad r1.rgb, r2, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mad r1.rgb, r3, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mad r1.rgb, r4, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mad r1.rgb, r5, c0, r1\n"
					"+ add r1.a, r1.a, c2.a\n"

                 "mul r1.rgb, r1, c3\n"
					"+ add r1.a, r1.a, c2.a\n"
                 "mov r0.rgb, r1\n"
					"+ add r0.a, r1.a, c2.a\n",


                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1,4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);


	///////////////////////////// PS2.0/////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////

	//32 texture addressing + 64 arithmetic instructions
	AddShaderDef("32 texture addressing + 64 arithmetic instructions",
				//dependent read offsets
				"def c0, 0.1, 0.1, 0.0, 1.0\n"
				"def c1, 0.2, 0.2, 0.0, 1.0\n"
				"def c2, 0.3, 0.3, 0.0, 1.0\n"
				"def c3, 0.15, 0.0, 0.0, 1.0\n"
				"def c4, 0.25, 0.0, 0.0, 1.0\n"
				"def c5, 0.35, 0.0, 0.0, 1.0\n"
				"def c6, 0.0, 0.05, 0.0, 1.0\n"
				"def c7, 0.0, 0.125, 0.0, 1.0\n"
				"def c8, 0.0, 0.2, 0.0, 1.0\n"
				"def c9, 0.0, 0.0, 0.0, 1.0\n"
				"def c10, 1.0, 1.0, 1.0, 1.0\n"
				"def c11, 0.0, 0.0, 0.0, 1.0\n"

				// 1/16
				"def c12, 0.0625, 0.0625, 0.0625, 1.0\n"

				"def c13, 0.0, 0.0, 0.0, 1.0\n"
				"def c14, 0.0, 0.0, 0.0, 1.0\n"
				"def c15, 0.0, 0.0, 0.0, 1.0\n"
				"def c16, 0.0, 0.0, 0.0, 1.0\n"
				"def c17, 0.0, 0.0, 0.0, 1.0\n"
				"def c18, 0.0, 0.0, 0.0, 1.0\n"
				"def c19, 0.0, 0.0, 0.0, 1.0\n"
				"def c20, 0.0, 0.0, 0.0, 1.0\n"
				"def c21, 0.0, 0.0, 0.0, 1.0\n"
				"def c22, 0.0, 0.0, 0.0, 1.0\n"
				"def c23, 0.0, 0.0, 0.0, 1.0\n"
				"def c24, 0.0, 0.0, 0.0, 1.0\n"
				"def c25, 0.0, 0.0, 0.0, 1.0\n"
				"def c26, 0.0, 0.0, 0.0, 1.0\n"
				"def c27, 0.0, 0.0, 0.0, 1.0\n"
				"def c28, 0.0, 0.0, 0.0, 1.0\n"
				"def c29, 0.0, 0.0, 0.0, 1.0\n"
				"def c30, 0.0, 0.0, 0.0, 1.0\n"
				"def c31, 2.0, 2.0, 2.0, 1.0\n"

				"dcl t0.xyzw \n"
				"dcl_2d s0 \n"
				"dcl_2d s1 \n"
				"dcl_2d s2 \n"
				"dcl_2d s3 \n"
				"dcl_2d s4 \n"
				"dcl_2d s5 \n"
				"dcl_2d s6 \n"
				"dcl_2d s7 \n"
				"dcl_2d s8 \n"
				"dcl_2d s9 \n"
				"dcl_2d s10 \n"
				"dcl_2d s11 \n"
				"dcl_2d s12 \n"
				"dcl_2d s13 \n"
				"dcl_2d s14 \n"
				"dcl_2d s15 \n"

				"mov r0, t0 \n"

				"add r1.xy, r0, c0 \n"
				"mov r1.zw, c10 \n"
				"texkill r1 \n"
				"texld r1.rgba, r1, s0 \n"

				"add r2.xy, r0, c1 \n"
				"mov r2.zw, c10 \n"
				"texkill r2 \n"
				"texld r2.rgba, r2, s1 \n"

				"add r3.xy, r0, c2 \n"
				"mov r3.zw, c10 \n"
				"texkill r3 \n"
				"texld r3.rgba, r3, s2 \n"

				"add r4.xy, r0, c3 \n"
				"mov r4.zw, c10 \n"
				"texkill r4 \n"
				"texld r4.rgba, r4, s3 \n"

				"add r5.xy, r0, c4 \n"
				"mov r5.zw, c10 \n"
				"texkill r5 \n"
				"texld r5.rgba, r5, s4 \n"

				"add r6.xy, r0, c5 \n"
				"mov r6.zw, c10 \n"
				"texkill r6 \n"
				"texld r6.rgba, r6, s5 \n"

				"add r7.xy, r0, c6 \n"
				"mov r7.zw, c10 \n"
				"texkill r7 \n"
				"texld r7.rgba, r7, s6 \n"

				"add r8.xy, r0, c7 \n"
				"mov r8.zw, c10 \n"
				"texkill r8 \n"
				"texld r8.rgba, r8, s7 \n"

				"add r9.xy, r0, c8 \n"
				"mov r9.zw, c10 \n"
				"texkill r9 \n"
				"texld r9.rgba, r9, s8 \n"

				"add r10.xy, r0, c9 \n"
				"mov r10.zw, c10 \n"
				"texkill r10 \n"
				"texld r10.rgba, r10, s9 \n"

				//We have run out of r# registers.
				//use r11 to store result upto this point
				"mov r11, c11 \n"
				"mad r11.xyz, r1 , c12 , r11  \n"
				"mad r11.xyz, r2 , c12 , r11  \n"
				"mad r11.xyz, r3 , c12 , r11  \n"
				"mad r11.xyz, r4 , c12 , r11  \n"
				"mad r11.xyz, r5 , c12 , r11  \n"
				"mad r11.xyz, r6 , c12 , r11  \n"
				"mad r11.xyz, r7 , c12 , r11  \n"
				"mad r11.xyz, r8 , c12 , r11  \n"
				"mad r11.xyz, r9 , c12 , r11  \n"
				"mad r11.xyz, r10 , c12 , r11  \n"

				// we have to touch the rest of the samplers too.
				"add r1.xy, r0 , c0  \n"
				"mov r1.zw, c10 \n"
				"texkill t0 \n"
				"texld r1.rgba, r1, s10 \n"

				"add r2.xy, r0 , c1  \n"
				"mov r2.zw, c10 \n"
				"texkill t0 \n"
				"texld r2.rgba, r2, s11 \n"

				"add r3.xy, r0 , c2  \n"
				"mov r3.zw, c10 \n"
				"texkill t0 \n"
				"texld r3.rgba, r3, s12 \n"

				"add r4.xy, r0 , c3  \n"
				"mov r4.zw, c10 \n"
				"texkill t0 \n"
				"texld r4.rgba, r4, s13 \n"

				"add r5.xy, r0 , c4  \n"
				"mov r5.zw, c10 \n"
				"texkill t0 \n"
				"texld r5.rgba, r5, s14 \n"

				"add r6.xy, r0 , c5  \n"
				"mov r6.zw, c10 \n"
				"texkill t0 \n"
				"texld r6.rgba, r6, s15 \n"

				//use r11 to add the remaining results
				"mad r11.xyz, r1 , c12 , r11  \n"
				"mad r11.xyz, r2 , c12 , r11  \n"
				"mad r11.xyz, r3 , c12 , r11  \n"
				"mad r11.xyz, r4 , c12 , r11  \n"
				"mad r11.xyz, r5 , c12 , r11  \n"
				"mad r11.xyz, r6 , c12 , r11  \n"

				// Filler instructions that does nothing to affect output.
				// These just make the arithmetic instruction count 64.
				"add r0, r0, r1\n"
				"sub r0, r0, r1\n"
				"add r0, r0, r1\n"
				"sub r0, r0, r1\n"
				"add r0, r0, r1\n"
				"sub r0, r0, r1\n"
				"add r0, r0, r1\n"
				"sub r0, r0, r1\n"

				// r# port limit 3
				"mul r0.x, r0, r0\n"
				"rcp r0.x, r0.x\n"
				"rsq r0.x, r0.x\n"

				//now r0.xy still has original t0.xy
				//Make r0.xy zero
				"sub r0, r0, t0\n"

				// magnify error and contribute from 
				// filler instructions so that they don't get optimised away.
				"mad r11.xyz, r11, c31, -r0\n"


				"mov oC0, r11\n",
                 MYTLVERTEXPERSP_FVF, g_FourColorVerticesMipBias, true, SHADE_PLAIN, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);
	SetLastShaderWrapAndMip();

	//max textures 8
	AddShaderDef("max textures 8",
                 "def c0, 0.75, 0.75, 0.75, 0.75\n"
                 "def c1, 0.25, 0.25, 0.25, 0.25\n"
				 "dcl t0.xy \n"
				 "dcl t1.xy \n"
				 "dcl t2.xy \n"
				 "dcl t3.xy \n"
				 "dcl t4.xy \n"
				 "dcl t5.xy \n"
				 "dcl t6.xy \n"
				 "dcl t7.xy \n"
				 "dcl_2d s0 \n"
				 "dcl_2d s1 \n"
				 "dcl_2d s2 \n"
				 "dcl_2d s3 \n"
				 "dcl_2d s4 \n"
				 "dcl_2d s5 \n"
				 "dcl_2d s6 \n"
				 "dcl_2d s7 \n"

				 "texld r0, t0, s0 \n"
				 "texld r1, t1, s1 \n"
				 "texld r2, t2, s2 \n"
				 "texld r3, t3, s3 \n"
				 "texld r4, t4, s4 \n"
				 "texld r5, t5, s5 \n"
				 "texld r6, t6, s6 \n"
				 "texld r7, t7, s7 \n"

                 "mul r8, r0, c0\n"
                 "mad r8, r1, c0, r8\n"
                 "mad r8, r2, c0, r8\n"
                 "mad r8, r3, c0, r8\n"

                 "mul r9, r4, c1\n"
                 "mad r9, r5, c1, r9\n"
                 "mad r9, r6, c1, r9\n"
                 "mad r9, r7, c1, r9\n"

				 "add r10, r9, r8\n"

				 "mov oC0, r10\n",
                 MYTLVERTEXTEX8_FVF, g_FourColorVerticesTex8, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);

	//Dependent texld sequence limit test.

	//"A 1st order dependent tex-op is a tex[ld*|kill] instruction in which either: "\
	//"(1) an r# reg is input (NOT t# reg), or "\
	//"(2) output r# reg was previously written, now being written AGAIN. "\
	//"A 2nd order dependent tex-op occurs if: a tex-op reads OR WRITES "\
	//"to an r# reg whose contents, BEFORE executing the tex-op, "\
	//"depend (perhaps indirectly) on the completion of a 1st order dependent tex-op. "\
	//"An (n)th order dependent tex-op derives from an (n-1)th order tex-op. "\
	//"A given tex-op may be dependent to at most 3rd order.",


	AddShaderDef("Dependent texld sequence limit test",
				"def c0, 0.125, 0.125, 1.0, 1.0 \n"
				"dcl t0.xy \n"
				"dcl_2d s0 \n"
				"dcl_2d s1 \n"
				"dcl_2d s2 \n"
				"mov r0.rg, t0 \n"

				"texld r1, r0, s0 \n"
				"add r1, r1, c0 \n"
				"texld r2, r1, s1 \n"
				"add r2, r2, c0 \n"
				"texld r3.rgba, r2, s2 \n"

				"mov oC0, r3\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);
	SetLastShaderWrapAndMip();

    return S_OK;
}

bool CPSLimits::GenerateTextureData()
{
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
	
	//Stripes
	D3DCOLOR pdwColors[] = {
        D3DRGBA(1, 1, 1, 1),	//white
        D3DRGBA(1, 0, 0, 1),	//red
        D3DRGBA(0, 1, 0, 1),	//green
        D3DRGBA(0, 0, 1, 1),	//blue
		D3DRGBA(1, 1, 0, 1),	//yellow
		D3DRGBA(0, 0, 0, 1),	//black
    };

	if ( !m_StripesImage.LoadStripes(LARGE_TEXSIZE, LARGE_TEXSIZE, 4, pdwColors, true, true) )
    {
        WriteToLog(_T("(ERROR) Failed to load LoadStripes.  Aborting.\n"));
        return false;
    }

	// Checker2
	if ( !m_Checker2Image.Load("checker2.bmp", 1.0f) )
    {
        WriteToLog(_T("(ERROR) Failed to load checker2.bmp.  Aborting.\n"));
        return false;
    }

	return CPixelShader::GenerateTextureData();
}

bool CPSLimits::LoadTextures()
{       
	if (!SetTextureFormat(TEXFORMAT))
		return false;
    
    DWORD dwLevels = (DWORD) 1;
    DWORD dwPool = POOL_DEFAULT;
    DWORD dwUsage = (DWORD) 0;
    FORMAT *pFormat = &m_pTexFormats[m_pCurrentMode->nTextureFormat];

    
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

	//Stripes
    if (!CreateTexture(LARGE_TEXSIZE, LARGE_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pStripesTex))
    {
        WriteToLog(_T("(ERROR) Failed to create Stripes texture.  Aborting.\n"));
        return false;
    }	
    if (!m_pStripesTex->LoadImage(dwLevels, &m_StripesImage))
    {
        WriteToLog(_T("Unable to load stripes texture.\n"));
        return false;
    }
	

    return CPixelShader::LoadTextures();
}
