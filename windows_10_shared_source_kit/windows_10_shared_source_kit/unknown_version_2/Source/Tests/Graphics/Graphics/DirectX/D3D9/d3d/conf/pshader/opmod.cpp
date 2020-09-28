// FILE:        op.cpp
// DESC:        pixel shader op conformance tests
//				Name has been standardized to Instruction Modifiers
// Modified:	Anthony Clarke
// Modified:	Ver2.0 added  Bobby Paulose

#include <ddraw.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "PShader.h"

#define _STRINGIZE(X) #X
#define STRINGIZE(Y) _STRINGIZE(Y)

#define TWO_PI_VEC_STR  "6.283185307f, 6.283185307f, 6.283185307f, 6.283185307f"

extern MYTLVERTEX g_FourBrickVertices[];
extern MYTLVERTEX g_FourColorVertices[];
extern MYTLVERTEX g_FourColorVertices2[];
extern MYTLVERTEXTEX2 g_FourColorVerticesTex2[];
extern MYTLVERTEXTEX4 g_FourColorVerticesTex4[];
extern MYTLVERTEX g_FourColorAlphaVertices[];
extern MYTLVERTEXTEX2 g_FourColorAlphaVerticesTex2[];
extern MYCUBETLVERTEX g_TexmZtests[];
extern MYCUBETLVERTEX2 g_Texm2Vertices[];
extern MYCUBETLVERTEX3 g_Cube3Vertices[];
extern MYTLVERTEXPERSP g_FourColorVerticesMipBias[];
extern MYTLVERTEXPERSP g_FourColorVerticesPerspective[];

CPSOps::CPSOps()
{
    m_szTestName = "Arithmetic Instructions & Modifiers";
    m_szCommandKey = "InsMod";

    m_pChecker2Tex = NULL;
    m_pDP3Tex = NULL;
    m_pGradientTex = NULL;
	m_pStripesTex = NULL;
    m_pSineBumpTex = NULL;

	m_bBump = false;
}

CPSOps_Bump::CPSOps_Bump()
{
    m_szTestName = "Arithmetic Instructions & Modifiers (Bump)";
    m_szCommandKey = "InsMod_Bump";

	m_bBump = true;
}

void CPSOps::ReleaseTextures()
{
    RELEASE(m_pChecker2Tex);
    RELEASE(m_pDP3Tex);
    RELEASE(m_pGradientTex);
	RELEASE(m_pStripesTex);
    RELEASE(m_pSineBumpTex);
	CPixelShader::ReleaseTextures();
	return;
}

UINT CPSOps::TestInitialize()
{
	m_byFormats = (m_bBump) ? TEXFORMAT | BUMPFORMAT : TEXFORMAT;
	UINT uResult = CPixelShader::TestInitialize();
    if ( D3DTESTINIT_RUN != uResult )
		return uResult;

    PopulateShaderInfo();

    SetTestRange(1, m_vShaders.size() * m_uTotalFormats * m_uNumShaderVersions);

    return D3DTESTINIT_RUN;
}

HRESULT CPSOps_Bump::PopulateShaderInfo()
{
	// ****************************** bem ******************************
	//bem
	AddShaderDef("bem .rg",
				"texld r0, t0\n"
				"texcrd r1.rgb, t1\n"
				"bem r1.rg, r1, r0\n"
				"phase\n"
				"texld r1, r1\n"
				"mov r0, r1\n",
				MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, SHADE_BUMP, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pSineBumpTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);
	SetLastShaderTexFormats(TEXFORMAT | BUMPFORMAT);

	return S_OK;
}

HRESULT CPSOps::PopulateShaderInfo()
{
	// _sat can be added to color and alpha blending operations. 
	// To reduce test length _sat is tested with an example from each group.
	float fConstZero[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float fConstNegLow[4] = { -0.25f, -0.25f, -0.25f, -0.25f };
	float fConstLow[4] = { 0.25f, 0.25f, 0.25f, 0.25f };
	float fConstHalf[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
	float fConstHigh[4] = { 0.75f, 0.75f, 0.75f, 0.75f };
    float fConstNegHalf[4] = { -0.5f, -0.5f, -0.5f, -0.5f };
    // ****************************** add ******************************
	// add
	AddShaderDef("add",
                 "tex t0\n"
                 "add r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

    // add_x2
	AddShaderDef("add_x2",
                 "tex t0\n"
                 "add_x2 r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices2);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // add_x4
	AddShaderDef("add_x4",
                 "tex t0\n"
                 "add_x4 r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices2);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // add_d2
	AddShaderDef("add_d2",
                 "tex t0\n"
                 "add_d2 r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

    // add_sat
	AddShaderDef("add_sat",
                 "tex t0\n"
                 "add_sat r1, t0, v0\n"
				 "mov_d2 r0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);    


	// ****************************** dp3 ******************************
    // dp3
	AddShaderDef("dp3",				 
                 "tex t0\n"	
				 "tex t1\n"
				 "mov r1, t1\n"
				 "dp3 r0, r1, t0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

    // dp3_x2
	AddShaderDef("dp3_x2",
				 "def c0, 0.25f, 0.25f, 0.25f, 0.25f\n"
                 "tex t0\n"
				 "dp3_x2 r0, c0, t0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // dp3_x4
	AddShaderDef("dp3_x4",
                 "tex t0\n"
				 "dp3_x4 r0, c0, t0\n"
				 "mul_d2 r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);
    AddConstantsToLastShaderDef(fConstHigh);

    // dp3_d2
	AddShaderDef("dp3_d2",
                 "tex t0\n"
				 "dp3_d2 r0, c0, t0_bx2\n"
				 "mul_x2 r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

    // dp3_sat
	AddShaderDef("dp3_sat",
                 "tex t0\n"
				 "tex t1\n"
				 "mov r1, t1\n"
				 "dp3_sat r0, r1, t0\n"
				 "mul_d2 r0, v0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

    // dp3 pairing
	AddShaderDef("dp3 pairing",
                 "def c0, 0.5f, 0.5f, 0.5f, 0.5f\n"
                 "tex t0\n"	
				 "tex t1\n"
				 "mov r1, t1\n"
				 "dp3 r0.rgb, r1, t0\n"
				 "+mov r0.a, c0.a\n"
                 "mul r0, v0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
    
	// ****************************** lrp ******************************
	// lrp
    AddShaderDef("lrp",
                 "tex t0\n"
                 "lrp r0, c0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

	// lrp_x2
    AddShaderDef("lrp_x2",
                 "tex t0\n"
                 "lrp_x2 r0, c0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

	// lrp_x4
    AddShaderDef("lrp_x4",
                 "tex t0\n"
                 "lrp_x4 r0, c0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

	// lrp_d2
    AddShaderDef("lrp_d2",
                 "tex t0\n"
                 "lrp_d2 r0, c0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstLow);

	// lrp_sat
    AddShaderDef("lrp_sat",
                 "tex t0\n"
                 "lrp_sat r0, c0, t0, v0\n"
				 "mov_d2 r0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHigh);


    // ****************************** mad ******************************
    // mad
	AddShaderDef("mad",
                 "tex t0\n"
                 "mad r0, v0, t0, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

    // mad_x2
	AddShaderDef("mad_x2",
                 "tex t0\n"
                 "mad_x2 r0, v0, t0, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstNegLow);

    // mad_x4
	AddShaderDef("mad_x4",
                 "tex t0\n"
                 "mad_x4 r0, v0, t0, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstNegLow);

    // mad_d2
	AddShaderDef("mad_d2",
                 "tex t0\n"
                 "mad_d2 r0, v0, t0, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

    // mad_sat
	AddShaderDef("mad_sat",
                 "tex t0\n"
                 "mad_sat r0, v0, t0, c0\n"
				 "mov_d2 r0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);


    // ****************************** mov ******************************
    // (21) mov
    AddShaderDef("mov",
                 "tex t0\n"
                 "mov r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // (22) mov_x2
    AddShaderDef("mov_x2",
                 "tex t0\n"
                 "mov_x2 r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // (23) mov_x4
    AddShaderDef("mov_x4",
                 "tex t0\n"
                 "mov_x4 r1, v0\n"
				 "mul r0, r1, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstLow);

    // (24) mov_d2
    AddShaderDef("mov_d2",
                 "tex t0\n"
                 "mov_d2 r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

    // (25) mov_sat
    AddShaderDef("mov_sat",
                 "tex t0\n"
                 "mov_x2 r0, t0\n"
				 "mov_sat r1, r0\n"
				 "mov_d2 r0.rgb, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);


    // ****************************** mul ******************************
    // mul
    AddShaderDef("mul",
                 "tex t0\n"
                 "mul r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // mul_x2
    AddShaderDef("mul_x2",
                 "tex t0\n"
                 "mul_x2 r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // mul_x4
    AddShaderDef("mul_x4",
                 "tex t0\n"
                 "mul_x4 r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // mul_d2
    AddShaderDef("mul_d2",
                 "tex t0\n"
                 "mul_d2 r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

    // mul_sat
    AddShaderDef("mul_sat",
                 "tex t0\n"
                 "add r1, t0, c0\n"
				 "mul_sat r0, r1, v0\n"
				 "add r0.rgb, r0, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);


    // ****************************** sub ******************************
    // sub
	AddShaderDef("sub",
                 "tex t0\n"
                 "tex t1\n"
                 "mov r1, t1\n"
				 "sub r0, r1, t0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

    // sub_x2
	AddShaderDef("sub_x2",
                 "tex t0\n"
				 "tex t1\n"
				 "mov r1, t1\n"
                 "sub_x2 r0, t0, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

    // sub_x4
	AddShaderDef("sub_x4",
                 "tex t0\n"
                 "sub_x4 r0, t0, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHigh);

    // sub_d2
	AddShaderDef("sub_d2",
                 "tex t0\n"
                 "sub_d2 r0, c0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

    // sub_sat
	AddShaderDef("sub_sat",
                 "tex t0\n"
                 "sub_sat r1, c0, t0\n"
				 "mov_d2 r0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(1.5f, 0.5f, 0.0f, 1.5f);


    // ****************************** nop ******************************
	// nop
    AddShaderDef("nop",
                 "tex t0\n" 
                 "nop\n"
                 "mov r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

    // ****************************** cnd ******************************
    // cnd
	AddShaderDef("cnd",
                 "tex t0\n"
                 "tex t1\n"
                 "mov r0, t0\n"
                 "cnd r0, r0.a, r0, t1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

    // cnd_x2
	AddShaderDef("cnd_x2",
                 "tex t0\n"
                 "tex t1\n"
                 "mov r0, t0\n"
                 "cnd_x2 r0, r0.a, r0, t1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

    // cnd_x4
	AddShaderDef("cnd_x4",
                 "tex t0\n"
                 "tex t1\n"
                 "mov r0, t0\n"
                 "cnd_x4 r0, r0.a, r0, t1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

    // cnd_d2
	AddShaderDef("cnd_d2",
                 "tex t0\n"
                 "tex t1\n"
                 "mov r0, t0\n"
                 "cnd_d2 r0, r0.a, r0, t1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

    // cnd_sat
	AddShaderDef("cnd_sat",
                 "tex t0\n"
                 "tex t1\n"
                 "mov_x2 r0, t0\n"
                 "cnd_sat r1, r0.a, r0, t1\n"
				 "mov_d2 r0, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);


    // *************************** cnd pairing ***************************
    // cnd pairing
	AddShaderDef("cnd: pairing",
				 "def c0, 0, 0, 0, 1.0\n"
				 "def c1, 0, 0, 0, 0.4\n"
				 "tex t0\n"
				 "mov r0, c0\n"
				 "mov r1, t0\n"
				 "mov r0.a, r1.a\n"
				 "+cnd r0.rgb, r0.a, c0.a, r0.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	//t0.a v0.rgb pairing
	AddShaderDef("t0.a v0.rgb pairing",
				 "tex t0\n"
				 "mov r0.a, t0.a\n"
				 "+mov r0.rgb, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	//t0.a t1.rgb pairing
	AddShaderDef("t0.a t1.rgb pairing",
				 "tex t0\n"
				 "tex t1\n"
				 "mov r0.a, t0\n"
				 "+mov r0.rgb, t1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);


	// ****************************** Testing known hardware issues. ******************************

	//1) ATI Radeon8500 can't do lrp with 0.
	AddShaderDef("lrp with 0",
				 "def c0, -0.5f, -0.5f, -0.5f, -0.5f\n"
                 "mov_sat r1, c0\n"
                 "mov r0, r1_bias\n"
				 "mov r0, -r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);

// *************************** Version 1.1 ***************************
	// Can use all the 1.0 shaders as 1.1	
	// ****************************** add ******************************
	// add to t0 
	AddShaderDef("add to t0",
                 "mov r0, c0\n"
                 "add t0, v0, r0\n"
				 "mov r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1,1));
    AddConstantsToLastShaderDef(fConstLow);

	// ****************************** lrp ******************************
	// lrp to t3
	AddShaderDef("lrp to t3",
                 "tex t0\n"
                 "mov r0, c0\n"
                 "lrp t3, t0, c1, v0\n"
				 "add r0, r0, t3\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1,1));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstZero);
    AddConstantsToLastShaderDef(fConstHalf);

	// ****************************** mul ******************************
	// mul to t2
	AddShaderDef("mul to t2",
				 "def c0, 0.0f, 0.0f, 0.0f, 0.0f\n"
                 "tex t0\n"
                 "mov r0, c0\n"
                 "mul t2, t0, v0\n"
				 "add r0, c1, t2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1,1));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);
	SetConstantsStart(1);

	// ****************************** sub ******************************
	// sub to t3 
	AddShaderDef("sub to t3",
                 "def c0, 0.0f, 0.0f, 0.0f, 0.0f\n"
				 "tex t0\n"
                 "mov r1, c0\n"
                 "sub t3, t0, v0\n"
				 "add r0, r1, t3\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1,1));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);


// *************************** Version 1.2 ***************************
	// Can use all the 1.0 and 1.1 shaders as 1.2
	
	// ****************************** cmp ******************************
    // cmp
	AddShaderDef("cmp",
                 "tex t0\n"
                 "tex t1\n"
                 "cmp r0, t0_bx2, v0, t1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1,2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

    // cmp_x2
	AddShaderDef("cmp_x2",
                 "tex t0\n"
                 "tex t1\n"
                 "cmp_x2 r0, t0_bx2, v0, t1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1,2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

    // cmp_x4
	AddShaderDef("cmp_x4",
                 "tex t0\n"
                 "tex t1\n"
                 "cmp_x4 r0, t0_bx2, v0, t1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1,2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

    // cmp_d2
	AddShaderDef("cmp_d2",
                 "tex t0\n"
                 "tex t1\n"
                 "cmp_d2 r0, t0_bx2, v0, t1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1,2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

    // cmp_sat
	AddShaderDef("cmp_sat",
                 "tex t0\n"
                 "tex t1\n"
                 "mov_x2 r0, v0\n"
                 "cmp_sat r1, t0_bx2, r0, t1\n"
				 "mov_d2 r0, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1,2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	// ****************************** dp4 ******************************
	// dp4
	AddShaderDef("dp4",				 
                 "tex t0\n"	
				 "tex t1\n"
				 "dp4 r0, t1, t0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1,2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

    // dp4_x2
	AddShaderDef("dp4_x2",				 
                 "tex t0\n"	
				 "tex t1\n"
				 "dp4_x2 r0, t1, t0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1,2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

    // dp4_x4
	AddShaderDef("dp4_x4",
                 "tex t0\n"	
				 "tex t1\n"
				 "dp4_x4 r0, t1, t0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1,2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// dp4_d2
	AddShaderDef("dp4_d2",
                 "tex t0\n"	
				 "tex t1\n"
				 "dp4_d2 r0, t1, t0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1,2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

    // dp4_sat
	AddShaderDef("dp4_sat",
                 "tex t0\n"
				 "tex t1\n"
				 "mov r1, t1\n"
				 "dp4_sat r0, r1, t0\n"
				 "mul_d2 r0, v0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1,2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	//dp4 can write to alpha in the 1.2 model dp3 cannot.
	// dp4 alpha output
	AddShaderDef("dp4 alpha output",
                 "tex t0\n"
				 "tex t1\n"
				 "mov r0, t0\n"
				 "dp4 r0.a, t1, t0\n"
				 "add r0.rgb, v0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

// *************************** Version 1.3 ***************************
	// Can use all the 1.0, 1.1 and 1.2 shaders as 1.3


// *************************** Version 1.4 ***************************

	// ****************************** add ******************************
	// add
	AddShaderDef("add",
                 "texld r0, t0\n"
                 "add r0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

    // add_x2
	AddShaderDef("add_x2",
                 "texld r0, t0\n"
                 "add_x2 r0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices2, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // add_x4
	AddShaderDef("add_x4",
                 "texld r0, t0\n"
                 "add_x4 r0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices2, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

	// add_x8
	AddShaderDef("add_x8",
                 "texld r0, t0\n"
                 "add_x8 r0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices2, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // add_d2
	AddShaderDef("add_d2",
                 "texld r0, t0\n"
                 "add_d2 r0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	// add_d4
	AddShaderDef("add_d4",
                 "texld r0, t0\n"
                 "add_d4 r0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	// add_d8
	AddShaderDef("add_d8",
                 "texld r0, t0\n"
                 "add_d8 r0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

    // add_sat
	AddShaderDef("add_sat",
                 "texld r0, t0\n"
                 "add_sat r1, r0, v0\n"
				 "mov_d2 r0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);


	// ****************************** sub ******************************
	// sub
	AddShaderDef("sub",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
				 "sub r0, r1, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

    // sub_x2
	AddShaderDef("sub_x2",
                 "texld r0, t0\n"
				 "texld r1, t1\n"
                 "sub_x2 r0, r0, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

    // sub_x4
	AddShaderDef("sub_x4",
                 "texld r0, t0\n"
				 "texld r1, t1\n"
                 "sub_x4 r0, r1, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// sub_x8
	AddShaderDef("sub_x8",
                 "texld r0, t0\n"
				 "texld r1, t1\n"
                 "sub_x8 r0, r1, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

    // sub_d2
	AddShaderDef("sub_d2",
                 "texld r0, t0\n"
                 "sub_d2 r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

	// sub_d4
	AddShaderDef("sub_d4",
                 "texld r0, t0\n"
                 "sub_d4 r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

	// sub_d8
	AddShaderDef("sub_d8",
                 "texld r0, t0\n"
				 "texld r1, t1\n"
                 "sub_d8 r0, r1, r0\n"
				 "add r0, r0, c0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
	AddConstantsToLastShaderDef(fConstLow);

    // sub_sat
	AddShaderDef("sub_sat",
                 "texld r0, t0\n"
                 "sub_sat r1, c0, r0\n"
				 "mov_d2 r0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(1.5f, 0.5f, 0.0f, 1.5f);


	// ****************************** mul ******************************
    // mul
    AddShaderDef("mul",
                 "texld r0, t0\n"
                 "mul r0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // mul_x2
    AddShaderDef("mul_x2",
                 "texld r0, t0\n"
                 "mul_x2 r0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // mul_x4
    AddShaderDef("mul_x4",
                 "texld r0, t0\n"
                 "mul_x4 r0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

	// mul_x8
    AddShaderDef("mul_x8",
                 "texld r0, t0\n"
                 "mul_x8 r0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // mul_d2
    AddShaderDef("mul_d2",
                 "texld r0, t0\n"
                 "mul_d2 r0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	// mul_d4
    AddShaderDef("mul_d4",
                 "texld r0, t0\n"
                 "mul_d4 r0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	// mul_d8
    AddShaderDef("mul_d8",
                 "texld r0, t0\n"
                 "mul_d8 r0, r0, v0\n"
				 "add r0, r0, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddConstantsToLastShaderDef(fConstLow);

    // mul_sat
    AddShaderDef("mul_sat",
                 "texld r0, t0\n"
                 "add r1, r0, c0\n"
				 "mul_sat r0, r1, v0\n"
				 "add r0.xyz, r0, c1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);
    AddConstantsToLastShaderDef(fConstNegHalf);

	// ****************************** mad ******************************
	// mad
	AddShaderDef("mad",
                 "texld r0, t0\n"
                 "mad r0, c0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

    // mad_x2
	AddShaderDef("mad_x2",
                 "texld r0, t0\n"
                 "mad_x2 r0, v0, c0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstLow);

    // mad_x4
	AddShaderDef("mad_x4",
                 "texld r0, t0\n"
                 "mad_x4 r0, v0, c0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);
    AddConstantsToLastShaderDef(fConstLow);

	// mad_x8
	AddShaderDef("mad_x8",
                 "texld r0, t0\n"
                 "mad_x8 r0, v0, c0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);
    AddConstantsToLastShaderDef(fConstLow);

    // mad_d2
	AddShaderDef("mad_d2",
                 "texld r0, t0\n"
                 "mad_d2 r0, c0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

	// mad_d4
	AddShaderDef("mad_d4",
                 "texld r0, t0\n"
                 "mad_d4 r0, c0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

	// mad_d8
	AddShaderDef("mad_d8",
                 "texld r0, t0\n"
                 "mad_d8 r0, c0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddConstantsToLastShaderDef(1.0f, 1.0f, 1.0f, 1.0f);

    // mad_sat
	AddShaderDef("mad_sat",
                 "texld r0, t0\n"
                 "mad_sat r0, c0, r0, v0\n"
				 "mov_d2 r0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

	// ****************************** lrp ******************************
	// lrp
    AddShaderDef("lrp",
                 "texld r0, t0\n"
                 "lrp r0, c0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

	// lrp_x2
    AddShaderDef("lrp_x2",
                 "texld r0, t0\n"
                 "lrp_x2 r0, c0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

	// lrp_x4
    AddShaderDef("lrp_x4",
                 "texld r0, t0\n"
                 "lrp_x4 r0, c0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

	// lrp_x8
    AddShaderDef("lrp_x8",
                 "texld r0, t0\n"
                 "lrp_x8 r0, c0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

	// lrp_d2
    AddShaderDef("lrp_d2",
                 "texld r0, t0\n"
                 "lrp_d2 r0, c0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstLow);

	// lrp_d4
    AddShaderDef("lrp_d4",
                 "texld r0, t0\n"
                 "lrp_d4 r0, c0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

	// lrp_d8
    AddShaderDef("lrp_d8",
                 "texld r0, t0\n"
                 "lrp_d8 r0, c0, r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHigh);

	// lrp_sat
    AddShaderDef("lrp_sat",
                 "texld r0, t0\n"
                 "lrp_sat r0, c0, r0, v0\n"
				 "mov_d2 r0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHigh);

	// ****************************** mov ******************************
	// mov
    AddShaderDef("mov",
                 "texld r1, t1\n"
                 "mov r0, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1, 4));
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // mov_x2
    AddShaderDef("mov_x2",
                 "texld r1, t1\n"
                 "mov_x2 r0, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1, 4));
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // mov_x4
    AddShaderDef("mov_x4",
                 "mov_x4 r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));

	// mov_x8
    AddShaderDef("mov_x8",
                 "mov_x8 r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));

    // mov_d2
    AddShaderDef("mov_d2",
                 "texld r1, t1\n"
                 "mov_d2 r0, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1, 4));
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	// mov_d4
    AddShaderDef("mov_d4",
                 "texld r1, t1\n"
                 "mov_d4 r0, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1, 4));
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// mov_d8
    AddShaderDef("mov_d8",
                 "texld r1, t1\n"
                 "mov_d8 r0, r1\n"
				 "add r0, r0, c0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1, 4));
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddConstantsToLastShaderDef(fConstHalf);

    // mov_sat
    AddShaderDef("mov_sat",
                 "texld r0, t0\n"
                 "mov_x2 r1, r0\n"
				 "mov_sat r2, r1\n"
				 "mov_d2 r0, r2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);


	// ****************************** cnd ******************************
	// cnd
	// As of version 1.4 this is a component wise operation.
	AddShaderDef("cnd",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
                 "cnd r0, r1, r0, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);    

    // cnd_x2
	AddShaderDef("cnd_x2",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
                 "cnd_x2 r0, r1, r0, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    
    // cnd_x4
	AddShaderDef("cnd_x4",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
                 "cnd_x4 r0, r1, r0, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);   

	// cnd_x8
	AddShaderDef("cnd_x8",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
                 "cnd_x8 r0, r1, r0, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);    

    // cnd_d2
	AddShaderDef("cnd_d2",
                 "texld r0, t0\n"
                 "texld r1, t1\n"				 
                 "cnd_d2 r0, r0, r1, r0\n"
				 "add r0, r0, c0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddConstantsToLastShaderDef(fConstHalf);

	// cnd_d4
	AddShaderDef("cnd_d4",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
				 "mov_x8 r1, r1\n"
                 "cnd_d4 r0, r0, r1, r0\n"
				 "add r0, r0, c0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddConstantsToLastShaderDef(fConstHalf);

	// cnd_d8
	AddShaderDef("cnd_d8",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
				 "mov_x8 r1, r1\n"
                 "cnd_d8 r0, r0, r1, r0\n"
				 "add r0, r0, c0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddConstantsToLastShaderDef(fConstHalf);

    // cnd_sat
	AddShaderDef("cnd_sat",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
				 "mov r1, r1_bx2\n"
                 "cnd_sat r1, r0, r1, r0\n"
				 "mov_d2 r0, r1\n"
				 "add r0, r0, c0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddConstantsToLastShaderDef(fConstLow);

	// ****************************** cmp ******************************
	// cmp
	AddShaderDef("cmp",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
                 "cmp r0, r1_bx2, r0, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);    

    // cmp_x2
	AddShaderDef("cmp_x2",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
                 "cmp_x2 r0, r0_bx2, r1, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

    // cmp_x4
	AddShaderDef("cmp_x4",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
                 "cmp_x4 r5, r1_bx2, r0, r1\n"
                 "mov r0, r5\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    

	// cmp_x8
	AddShaderDef("cmp_x8",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
                 "cmp_x8 r0, r0_bx2, r1, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

    // cmp_d2
	AddShaderDef("cmp_d2",
                 "texld r0, t0\n"
                 "texld r1, t1\n"				 
                 "cmp_d2 r0, r0_bx2, r1, r0\n"
				 "add r0, r0, c0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddConstantsToLastShaderDef(fConstHalf);

	// cmp_d4
	AddShaderDef("cmp_d4",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
                 "cmp_d4 r0, r1_bx2, r0, r1\n"
				 "add r0, r0, c0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);    
	AddConstantsToLastShaderDef(fConstHalf);

	// cmp_d8
	AddShaderDef("cmp_d8",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
                 "cmp_d8 r0, r0_bx2, r1, r0\n"
				 "add r0, r0, c0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddConstantsToLastShaderDef(fConstHalf);

    // cmp_sat
	AddShaderDef("cmp_sat",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
                 "cmp_sat r1, r0_bx2, r1, r0\n"
				 "mov_d2 r0, r1\n"
				 "add r0, r0, c0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddConstantsToLastShaderDef(fConstLow);

	// ****************************** dp3 ******************************
    // dp3
	AddShaderDef("dp3",				 
                 "texld r0, t0\n"	
				 "texld r1, t1\n"
				 "dp3 r0, r1, r0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

    // dp3_x2
	AddShaderDef("dp3_x2",
				 "def c0, 0.1f, 0.1f, 0.1f, 0.4f\n"
                 "texld r0, t0\n"
				 "dp3_x2 r0, c0, r0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // dp3_x4
	AddShaderDef("dp3_x4",
				 "def c0, 0.1f, 0.1f, 0.1f, 0.4f\n"
                 "texld r0, t0\n"				 
				 "dp3_x4 r0, c0, r0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

	// dp3_x8
	AddShaderDef("dp3_x8",
                 "def c0, 0.1f, 0.1f, 0.1f, 0.4f\n"
				 "texld r0, t0\n"
				 "dp3_x8 r0, c0, r0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);
    AddConstantsToLastShaderDef(fConstLow);

    // dp3_d2
	AddShaderDef("dp3_d2",
                 "texld r0, t0\n"
				 "dp3_d2 r0, c0, r0_bx2\n"
				 "mul_x2 r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

	// dp3_d4
	AddShaderDef("dp3_d4",
                 "texld r0, t0\n"
				 "dp3_d4 r0, c0, r0_bx2\n"
				 "mul_x2 r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

	// dp3_d8
	AddShaderDef("dp3_d8",
                 "texld r0, t0\n"
				 "dp3_d8 r0, c0, r0_bx2\n"
				 "mul_x2 r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHalf);

    // dp3_sat
	AddShaderDef("dp3_sat",
                 "texld r0, t0\n"
				 "texld r1, t1\n"
				 "dp3_sat r0, r1, r0\n"
				 "mul_d2 r0, v0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	
	//dp3 but write to alpha only.
	// ****************************** dp3 ******************************
    // dp3
	AddShaderDef("dp3 alpha output",				 
                 "texld r0, t0\n"	
				 "texld r1, t1\n"
				 "dp3 r0.a, r1, r0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

    // dp3_x2
	AddShaderDef("dp3_x2 alpha output",
				 "def c0, 0.25f, 0.25f, 0.25f, 0.25f\n"
                 "texld r0, t0\n"
				 "dp3_x2 r0.a, c0, r0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);


	// ****************************** dp4 ******************************
	// dp4
	AddShaderDef("dp4",
                 "texld r0, t0\n"	
				 "texld r1, t1\n"
				 "dp4 r0, r1_bias, r0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorAlphaVerticesTex2, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

    // dp4_x2
	AddShaderDef("dp4_x2",
				 "def c0, 0.1f, 0.1f, 0.1f, 0.1f\n"
                 "texld r0, t0\n"
				 "dp4_x2 r0, c0, r0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

    // dp4_x4
	AddShaderDef("dp4_x4",
				 "def c0, 0.1f, 0.1f, 0.1f, 0.1f\n"
                 "texld r0, t0\n"				 
				 "dp4_x4 r5, c0, r0\n"
				 "mul r0, v0, r5\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

	// dp4_x8
	AddShaderDef("dp4_x8",
                 "def c0, 0.05f, 0.05f, 0.05f, 0.05f\n"
				 "texld r0, t0\n"
				 "dp4_x8 r0, c0, r0\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

    // dp4_d2
	AddShaderDef("dp4_d2",
                 "texld r0, t0\n"
				 "dp4_d2 r0, c0, r0_bx2\n"
				 "mul_x2 r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHigh);

	// dp4_d4
	AddShaderDef("dp4_d4",
                 "texld r0, t0\n"
				 "dp4_d4 r0, c0, r0_bx2\n"
				 "mul_x2 r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHigh);

	// dp4_d8
	AddShaderDef("dp4_d8",
                 "texld r0, t0\n"
				 "dp4_d8 r0, c0, r0_bx2\n"
				 "mul_x2 r0, v0, r0\n"
				 "add r0, r0, c1\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstHigh);
	AddConstantsToLastShaderDef(fConstLow);

    // dp4_sat
	AddShaderDef("dp4_sat",
                 "texld r0, t0\n"
				 "texld r1, t1\n"
				 "dp4_sat r0, r1_bias, r0_bx2\n"
				 "mul_d2 r0, v0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorAlphaVerticesTex2, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	// dp4 swizzle output
	AddShaderDef("dp4 ga",
                 "texld r0, t0\n"
				 "texld r1, t1\n"
				 "dp4 r0.ga, r1, r0\n"
				 "add r0.rb, v0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	// ****************************** nop ******************************
	// nop
    AddShaderDef("nop",
                 "texld r0, t0\n" 
                 "nop\n"
                 "mov r0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

// *************************** Version 2.0 ***************************
// *******************************************************************

	// nop
    AddShaderDef("nop",
				 "dcl v0.rgba\n"
                 "nop\n"
                 "nop\n"
                 "nop\n"
                 "nop\n"
                 "nop\n"
				 "mov oC0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));

	// add
	AddShaderDef("add",
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "dcl v0\n"
				 "texld r0.rgba,t0,s0\n"
                 "add r0, r0, v0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	// sub
	AddShaderDef("sub",
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "dcl t1.xy\n"
				 "dcl_2d s1\n"
				 "texld r0.rgba,t0,s0\n"
				 "texld r1.rgba,t1,s1\n"
				 "sub r0, r1, r0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// mov
    AddShaderDef("mov",
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "texld r0.rgba,t0,s0\n"
                 "mov r1, r0\n"
				 "mov oC0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

    // mul
    AddShaderDef("mul",
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "dcl v0\n"
				 "texld r0.rgba,t0,s0\n"
                 "mul r0, r0, v0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pChecker2Tex);

	// mad
	AddShaderDef("mad",
                 "def c0, 0.5, 0.5, 0.5, 0.5\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "dcl v0\n"
				 "texld r0.rgba,t0,s0\n"
                 "mad r0, c0, r0, v0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	// dp2add	- legacy bem can be implemented using 2 dp2add
	AddShaderDef("dp2add",
                 "def c0, 0.33, 0.33, 0.33, 1.0\n"
                 "def c1, 0.33, 0.33, 0.33, 1.0\n"
                 "def c2, 0.5, 0.5, 0.0, 0.0\n"
                 "def c3, 0.15, 0.15, 0.0, 0.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "dcl_2d s1\n"
				 "texld r0,t0,s0\n"
				 "mul r1, c0, r0.r\n"
				 "mad r1, r0.b, c0, r1\n"
				 "mad r1, r0.g, c0, r1\n"
				 "mov r1.a, c0\n"
				 "sub r1, r1, c1\n"

				 "dp2add r2.x, r1, c2, t0.x\n"
				 "dp2add r2.y, r1, c3, t0.y\n"

				 "texld r3,r2,s1\n"
				 "mov oC0, r3\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	SetLastShaderWrapAndMip();

    // dp3
	AddShaderDef("dp3",				 
				 "dcl t0.xy\n"
				 "dcl t1.xy\n"
				 "dcl_2d s0\n"
				 "dcl_2d s1\n"
				 "dcl v0.rgba\n"
                 "texld r0, t0, s0\n"	
				 "texld r1, t1, s1\n"
				 "dp3 r0, r1, r0\n"
				 "mul r0, v0, r0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

    // dp4
	AddShaderDef("dp4",				 
				 "dcl t0.xyzw\n"
				 "dcl v0.rgba\n"
				 "mov r1,v0\n"
				 "dp4 r0,t0,r1\n"
				 "mov oC0, r0\n",
                 MYTLVERTEXPERSP_FVF, g_FourColorVerticesMipBias, true, SHADE_PLAIN, PSVERSION(2, 0));

	//cmp
	AddShaderDef("cmp",
                 "def c0, 0.4, 0.3, 0.3, 1.0\n"
				 "dcl t0.xyzw\n"
				 "dcl_2d s0\n"
				 "dcl v0.rgba\n"
				 "texld r0.rgba,t0,s0\n"
				 "mov r1,t0\n"
				 "sub r1,r1,c0\n"
				 "cmp r0,r1,r0,v0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEXPERSP_FVF, g_FourColorVerticesMipBias, true, SHADE_PLAIN, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	/////////////These instructions/macros are new for PS2.0////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////
	//note: macros are implemented by driver or hw. So we need WHQL tests for them.


	/////////////////////////instructions///////////////////////////////////////////////

	// frc   
    AddShaderDef("frc",
                 "def c0, 3.0, 3.0, 3.0, 3.0\n"
				 "dcl v0.rgba\n"
                 "add r0, v0, c0\n"
				 "frc r0, r0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));

	// exp   - 2 power r,g,b,a fraction summation
    AddShaderDef("exp",   
                 "def c0, 0.1, 0.1, 0.1, 0.1\n"
				 "dcl v0.rgba\n"
                 "exp r0, v0.r\n"
                 "exp r1, v0.g\n"
                 "exp r2, v0.b\n"
                 "exp r3, v0.a\n"
				 "mul r0, r0, c0\n"
				 "mad r0, r1, c0, r0\n"
				 "mad r0, r2, c0, r0\n"
				 "mad r0, r2, c0, r0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));

	// log   - negative log base 2 of r,g,b,a summation
    AddShaderDef("log",   
                 "def c0, -0.25, -0.25, -0.25, -0.25\n"
				 "dcl v0.rgba\n"
                 "log r0, v0.r\n"
                 "log r1, v0.g\n"
                 "log r2, v0.b\n"
                 "log r3, v0.a\n"
				 "mul r0, r0, c0\n"
				 "mad r0, r1, c0, r0\n"
				 "mad r0, r2, c0, r0\n"
				 "mad r0, r2, c0, r0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));

	// rcp   1/1/g = g
    AddShaderDef("rcp",
				 "dcl v0.rgba\n"
                 "rcp r0, v0.g\n"
                 "rcp r0, r0.r\n"
                 "rcp r0, r0.b\n"
                 "rcp r0, r0.a\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));

	// rsq   - double sqrt b
    AddShaderDef("rsq",
				 "dcl v0.rgba\n"
                 "rsq r0, v0.b\n"
                 "rsq r0, r0.r\n"
                 "rsq r1, r0.g\n"
                 "rsq r2, r0.a\n"
				 "add r0, r0, r1\n"
				 "sub r0, r0, r2\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));

	// min
	AddShaderDef("min",
                 "def c0, 2.0, 2.0, 2.0, 2.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "dcl t1.xy\n"
				 "dcl_2d s1\n"
				 "texld r0.rgba,t0,s0\n"
				 "texld r1.rgba,t1,s1\n"
				 "min r2, r1, r0\n"
				 "mul r2, r2, c0\n"
				 "mov oC0, r2\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// max	- see DXLogo thru the black squares of Checker
	AddShaderDef("max",
                 "def c0, 2.0, 2.0, 2.0, 2.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "dcl t1.xy\n"
				 "dcl_2d s1\n"
				 "texld r0.rgba,t0,s0\n"
				 "mul r0, r0, c0\n"
				 "texld r1,t1,s1\n"
				 "max r2, r1, r0\n"
				 "mov oC0, r2\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	/////////////////////////macros///////////////////////////////////////////////

	// lrp
    AddShaderDef("lrp",
                 "def c0, 0.5, 0.5, 0.5, 0.5\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "dcl v0\n"
				 "texld r0.rgba,t0,s0\n"
                 "lrp r0, c0, r0, v0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	// pow	- eighth of gradient power logo
	AddShaderDef("pow",
                 "def c0, 0.125, 0.125, 0.125, 0.125\n"
                 "def c1, 0.001, 0.001, 0.001, 0.001\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "dcl t1.xy\n"
				 "dcl_2d s1\n"
				 "texld r0.rgba,t0,s0\n"
				 "texld r1,t1,s1\n"
				 "mul r1, r1, c0\n"
                 "add r1, r1, c1\n"
				 "pow r1, r1.g, r0.r\n"
				 "mov oC0, r1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	//crs  - cross product of logotex and its coords
	AddShaderDef("crs",
                 "def c0, 2.0, 2.0, 2.0, 1.0\n"
				 "dcl t0.xyzw\n"
				 "dcl_2d s0\n"
				 "texld r0.rgba,t0,s0\n"
				 "crs r1.rgb, r0, t0\n"
				 "mov r1.a, c0.a\n"
				 "mul r1, r1, c0\n"
				 "mov oC0, r1\n",
                 MYTLVERTEXPERSP_FVF, g_FourColorVerticesPerspective, true, SHADE_PLAIN, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	// nrm		- multiply tex colors by 5. If not normalised will result in all white/yellow.
	AddShaderDef("nrm",
                 "def c0, 5.0, 5.0, 5.0, 1.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "texld r0,t0,s0\n"
                 "mul r0, r0, c0\n"
                 "mov r1.w, c0\n"
                 "nrm r1.xyz, r0\n"
				 "mov oC0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);

	//abs  - abs value of a cross product; neg values not floored to zero.
	AddShaderDef("abs",
                 "def c0, 3.0, 3.0, 3.0, 1.0\n"
				 "dcl t0.xyzw\n"
				 "dcl_2d s0\n"
				 "texld r0.rgba,t0,s0\n"
				 "crs r1.rgb, r0, t0\n"
				 "mov r1.a, c0.a\n"
				 "mul r1, r1, c0\n"
				 "abs r1, r1\n"
				 "mov oC0, r1\n",
                 MYTLVERTEXPERSP_FVF, g_FourColorVerticesPerspective, true, SHADE_PLAIN, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	//sincos - uses Dx9 defines for taylor series approx.
	AddShaderDef("sincos",
                 "def c0," TWO_PI_VEC_STR "\n"
                 "def c1," STRINGIZE(D3DSINCOSCONST1) "\n"
                 "def c2," STRINGIZE(D3DSINCOSCONST2) "\n"
                 "def c3, 0.5, 0.5, 0.5, -1.0\n"
				 "dcl v0.rgba\n"
				 "sub r0, v0, c3\n"
				 "mul r0, r0, c0\n"
                 "sincos r1.xy, r0.x, c1, c2\n"
				 "mov r1.ba, v0\n"
				 "abs r1, r1\n"
				 "mov oC0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));

    // m3x2 constant
	AddShaderDef("m3x2 constant",				 
                 "def c0, 1.0, 1.0, 1.0, 1.0\n"
                 "def c1, 0.5, 0.5, 0.5, 1.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "dcl v0\n"
                 "texld r1, t0, s0\n"	
				 "mov r0, c0\n"
				 "m3x2 r0.xy, r1, c1\n"
				 "mul r0, v0, r0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

    // m3x2 register
	AddShaderDef("m3x2 register",				 
				 "dcl t0.xy\n"
				 "dcl t1.xy\n"
				 "dcl_2d s0\n"
				 "dcl_2d s1\n"
				 "dcl v0.rgba\n"
                 "texld r0, t0, s0\n"	
				 "texld r1, t1, s1\n"
				 "m3x2 r0.xy, r1, r0\n"
				 "mul r0, v0, r0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

    // m3x3 constant
	AddShaderDef("m3x3 constant",				 
                 "def c0, 1.0, 1.0, 1.0, 1.0\n"
                 "def c1, 0.5, 0.5, 0.5, 1.0\n"
                 "def c3, 0.5, 0.5, 0.5, 1.0\n"
				 "dcl t0.xyz\n"
				 "dcl_2d s0\n"
				 "dcl v0\n"
                 "texld r1, t0, s0\n"	
				 "mov r0, c0\n"
				 "m3x3 r0.xyz, r1, c1\n"
				 "mul r0, v0, r0\n"
				 "mov oC0, r0\n",
				 MYCUBETLVERTEX2_FVF, g_Texm2Vertices, PSVERSION(2,0));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	SetLastShaderWrapAndMip();

    // m3x3 register
	AddShaderDef("m3x3 register",				 
				 "dcl t0.xyz\n"
				 "dcl t1.xyz\n"
				 "dcl t2.xyz\n"
				 "dcl_2d s0\n"
				 "dcl_2d s1\n"
				 "dcl_2d s2\n"
				 "dcl v0.rgba\n"
                 "texld r0, t0, s0\n"	
				 "texld r1, t1, s1\n"
				 "texld r2, t2, s2\n"
				 "m3x3 r0.xyz, r1, r0\n"
				 "mul r0, v0, r0\n"
				 "mov oC0, r0\n",
				 MYCUBETLVERTEX2_FVF, g_Texm2Vertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	SetLastShaderWrapAndMip();

    // m3x4 constant
	AddShaderDef("m3x4 constant",				 
                 "def c0, 1.0, 1.0, 1.0, 1.0\n"
                 "def c1, 0.5, 0.5, 0.5, 1.0\n"
                 "def c2, 0.5, 0.5, 0.5, 1.0\n"
				 "dcl t2.xyz\n"
				 "dcl_2d s0\n"
				 "dcl v0\n"
                 "texld r1, t2, s0\n"	
				 "m3x4 r0, r1, c1\n"
				 "mul r0, v0, r0\n"
				 "mov oC0, r0\n",
				 MYCUBETLVERTEX2_FVF, g_Texm2Vertices, PSVERSION(2,0));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	SetLastShaderWrapAndMip();

    // m3x4 register
	AddShaderDef("m3x4 register",				 
				 "dcl t0.xyz\n"
				 "dcl t1.xyz\n"
				 "dcl t2.xyz\n"
				 "dcl t3.xyz\n"
				 "dcl_2d s0\n"
				 "dcl_2d s1\n"
				 "dcl_2d s2\n"
				 "dcl_2d s3\n"
				 "dcl v0.rgba\n"
                 "texld r0, t0, s0\n"	
				 "texld r1, t1, s1\n"
				 "texld r2, t2, s2\n"
				 "texld r3, t3, s3\n"
				 "m3x4 r0, r1, r0\n"
				 "mul r0, v0, r0\n"
				 "mov oC0, r0\n",
				 MYCUBETLVERTEX_FVF, g_TexmZtests, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

    // m4x3 constant
	AddShaderDef("m4x3 constant",				 
                 "def c0, 1.0, 1.0, 1.0, 1.0\n"
                 "def c2, 0.5, 0.5, 0.5, 0.5\n"
                 "def c3, 0.5, 0.5, 0.5, 0.5\n"
				 "dcl t0.xyz\n"
				 "dcl_2d s0\n"
				 "dcl v0\n"
                 "texld r1, t0, s0\n"	
				 "mov r0, c0\n"
				 "m4x3 r0.xyz, r1, c1\n"
				 "mul r0, v0, r0\n"
				 "mov oC0, r0\n",
				 MYCUBETLVERTEX2_FVF, g_Texm2Vertices, PSVERSION(2,0));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	SetLastShaderWrapAndMip();

    // m4x3 register
	AddShaderDef("m4x3 register",				 
				 "dcl t0.xyz\n"
				 "dcl t1.xyz\n"
				 "dcl t2.xyz\n"
				 "dcl_2d s0\n"
				 "dcl_2d s1\n"
				 "dcl_2d s2\n"
				 "dcl v0.rgba\n"
                 "texld r0, t0, s0\n"	
				 "texld r1, t1, s1\n"
				 "texld r2, t2, s2\n"
				 "m4x3 r0.xyz, r1, r0\n"
				 "mul r0, v0, r0\n"
				 "mov oC0, r0\n",
				 MYCUBETLVERTEX2_FVF, g_Texm2Vertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	SetLastShaderWrapAndMip();

    // m4x4 constant
	AddShaderDef("m4x4 constant",				 
                 "def c0, 1.0, 1.0, 1.0, 1.0\n"
                 "def c2, 0.5, 0.5, 0.5, 0.5\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "dcl v0\n"
                 "texld r1, t0, s0\n"	
				 "m4x4 r0, r1, c1\n"
				 "mul r0, v0, r0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

    // m4x4 register
	AddShaderDef("m4x4 register",				 
				 "dcl t0.xy\n"
				 "dcl t1.xy\n"
				 "dcl t2.xy\n"
				 "dcl t3.xy\n"
				 "dcl_2d s0\n"
				 "dcl_2d s1\n"
				 "dcl_2d s2\n"
				 "dcl_2d s3\n"
				 "dcl v0.rgba\n"
                 "texld r0, t0, s0\n"	
				 "texld r1, t1, s1\n"
				 "texld r2, t2, s2\n"
				 "texld r3, t3, s3\n"
				 "m4x4 r0, r1, r0\n"
				 "mul r0, v0, r0\n"
				 "mov oC0, r0\n",
				 MYCUBETLVERTEX3_FVF, g_Cube3Vertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	////////////////////saturate operation modifier test////////////////
	//Since we don't want to add one test case for each instruction's negative and 
	//positive saturation we'll try to club them together.
	//Current strategy of clubbing is like this. There will be 6 saturates in 
	//one test case trying to saturate the three channels rgb top one-third and bottom one-third.

	// add sub mov +/- sat 
	AddShaderDef("add sub mov +/- sat",
                 "def c0, 0.33, 0.33, 0.33, 0.33\n"
                 "def c1, 0.0, 0.0, 0.0, 1.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "texld r0,t0,s0\n"
				 "mov r1, c1 \n"

				 // add + and - sat
                 "add_sat r1.r, r0.r, c0.r\n"
                 "sub r1.r, r1.r, c0.r\n"
                 "add_sat r1.r, r1.r, -c0.r\n"
                 "sub r1.r, r1.r, -c0.r\n"

				 // sub + and - sat
                 "sub_sat r1.g, r0.r, -c0.r\n"
                 "add r1.g, r1.g, -c0.r\n"
                 "sub_sat r1.g, r1.g, c0.r\n"
                 "add r1.g, r1.g, c0.r\n"

				 // mov + and - sat
                 "add r1.b, r0.r, c0.r\n"
				 "mov_sat r1.b, r1.b \n"
                 "sub r1.b, r1.b, c0.r\n"
                 "add r1.b, r1.b, -c0.r\n"
                 "mov_sat r1.b, r1.b\n"
                 "sub r1.b, r1.b, -c0.r\n"

				 "mov oC0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// mul mad dp2add +/- sat 
	AddShaderDef("mul mad dp2add +/- sat",
                 "def c0, 2.0, 2.0, 2.0, 2.0\n"
                 "def c1, 0.5, 0.5, 0.5, 0.5\n"
                 "def c2, 1.0, 1.0, 1.0, 1.0\n"
                 "def c3, 0.0, 0.0, 0.0, 1.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "texld r0,t0,s0\n"
				 "mov r1, c3 \n"

				 // mul + and - sat
                 "mul r1.r, r0.r, c0.r\n"
                 "add r1.r, r1.r, -c1.r\n"
                 "mul_sat r1.r, r1.r, c2.r\n"
                 "add r1.r, r1.r, c1.r\n"
                 "mul r1.r, r1.r, c1.r\n"

				 // mad + and - sat
                 "mul r1.g, r0.r, c0.r\n"
                 "add r1.g, r1.g, -c1.r\n"
                 "mad_sat r1.g, r1.g, c3.a, c3.r\n"
                 "add r1.g, r1.g, c1.r\n"
                 "mul r1.g, r1.g, c1.r\n"

				 // dp2add + and - sat
                 "mul r1.b, r0.r, c0.r\n"
                 "add r1.b, r1.b, -c1.r\n"
                 "dp2add_sat r1.b, r1.b, c3.a, -r1.b\n"
                 "add r1.b, r1.b, c1.r\n"
                 "mul r1.b, r1.b, c1.r\n"

				 "mov oC0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// dp3 dp4 rcp +/- sat 
	AddShaderDef(" dp3 dp4 rcp +/- sat",
                 "def c0, 2.0, 2.0, 2.0, 2.0\n"
                 "def c1, 0.5, 0.5, 0.5, 0.5\n"
                 "def c2, 1.0, 1.0, 1.0, 1.0\n"
                 "def c3, 0.0, 0.0, 0.0, 1.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "texld r0,t0,s0\n"
				 "mov r1, c3 \n"

				 // dp3 + and - sat
                 "mul r1.r, r0.r, c0.r\n"
                 "add r1.r, r1.r, -c1.r\n"
                 "dp3_sat r1.r, r1.r, c3.abgr\n"
                 "add r1.r, r1.r, c1.r\n"
                 "mul r1.r, r1.r, c1.r\n"

				 // dp4 + and - sat
                 "mul r1.g, r0.r, c0.r\n"
                 "add r1.g, r1.g, -c1.r\n"
                 "dp4_sat r1.g, r1.g, c3.abgr\n"
                 "add r1.g, r1.g, c1.r\n"
                 "mul r1.g, r1.g, c1.r\n"

				 // rcp + and - sat
                 "mul r1.b, r0.r, c0.r\n"
                 "add r1.b, r1.b, -c1.r\n"
				 "rcp r1.b, r1.b \n"
				 "rcp_sat r1.b, r1.b \n"
                 "add r1.b, r1.b, c1.r\n"
                 "mul r1.b, r1.b, c1.r\n"

				 "mov oC0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// rsq exp + sat, log +/- sat 
	AddShaderDef(" rsq exp + sat, log +/- sat",
                 "def c0, 2.0, 2.0, 2.0, 2.0\n"
                 "def c1, 0.5, 0.5, 0.5, 0.5\n"
                 "def c2, 1.0, 1.0, 1.0, 1.0\n"
                 "def c3, 0.0, 0.0, 0.0, 1.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "texld r0,t0,s0\n"
				 "mov r1, c3 \n"

				 // rsq + sat (no -ve sat possible for rsq)
                 "mul r1.r, r0.r, c0.r\n"
                 "mul r1.r, r1.r, r1.r\n"
				 "rcp r1.r, r1.r \n"
				 "rsq_sat r1.r, r1.r \n"
                 "mul r1.r, r1.r, c1.r\n"

				 // exp + sat (no -ve sat possible for exp)
                 "mul r1.g, r0.r, c0.r\n"
                 "log r1.g, r1.g\n"
				 "exp_sat r1.g, r1.g \n"
                 "mul r1.g, r1.g, c1.r\n"

				 // log +/- sat 
                 "mul r1.b, r0.r, c0.r\n"
                 "sub r1.b, r1.b, c1.b\n"
				 "exp r1.b, r1.b \n"
                 "log_sat r1.b, r1.b\n"
                 "add r1.b, r1.b, c1.b\n"
                 "mul r1.b, r1.b, c1.r\n"

				 "mov oC0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// cmp, min, max +/- sat 
	AddShaderDef(" cmp, min, max +/- sat",
                 "def c0, 3.0, 3.0, 3.0, 3.0\n"
                 "def c1, 0.33, 0.33, 0.33, 0.33\n"
                 "def c2, 1.0, 1.0, 1.0, 1.0\n"
                 "def c3, 0.0, 0.0, 0.0, 1.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "texld r0,t0,s0\n"
				 "mov r1, c3 \n"

				 // cmp +/- sat 
                 "mul r1.r, r0.r, c0.r\n"
                 "sub r1.r, r1.r, c2.r\n"
				 "cmp_sat r1.r, c0.r, r1.r, c0.r \n"
                 "add r1.r, r1.r, c2.r\n"
                 "mul r1.r, r1.r, c1.r\n"

				 // min +/- sat 
                 "mul r1.g, r0.r, c0.r\n"
                 "sub r1.g, r1.g, c2.r\n"
				 "min_sat r2.g, c0.r, r1.g\n"
                 "add r1.g, r2.g, c2.r\n"
                 "mul r1.g, r1.g, c1.r\n"

				 // max +/- sat 
                 "mul r1.b, r0.r, c0.r\n"
                 "sub r1.b, r1.b, c2.r\n"
				 "max_sat r2.b, -c0.r, r1.b\n"
                 "add r1.b, r2.b, c2.r\n"
                 "mul r1.b, r1.b, c1.r\n"

				 "mov oC0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// lrp, pow, crs +/- sat 
	AddShaderDef(" lrp, pow, crs +/- sat",
                 "def c0, 3.0, 3.0, 3.0, 3.0\n"
                 "def c1, 0.33, 0.33, 0.33, 0.33\n"
                 "def c2, 1.0, 1.0, 3.0, 1.0\n"
                 "def c3, 0.0, 0.0, 0.0, 1.0\n"
                 "def c4, 1.0, 0.0, 1.0, 1.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "texld r0,t0,s0\n"
				 "mov r1, c3 \n"

				 // lrp +/- sat 
                 "mul r1.r, r0.r, c0.r\n"
                 "sub r1.r, r1.r, c2.r\n"
				 "lrp_sat r2.r, c1.r, r1.r, r1.r \n"
                 "add r1.r, r2.r, c2.r\n"
                 "mul r1.r, r1.r, c1.r\n"

				 // pow +/- sat 
                 "mul r1.g, r0.r, c0.r\n"
                 "sub r1.g, r1.g, c2.r\n"
				 "pow_sat r1.g, r1.g, c2.b\n"
                 "add r1.g, r1.g, c2.r\n"
                 "mul r1.g, r1.g, c1.r\n"

				 // crs +/- sat 
                 "mul r1.b, r0.r, c0.r\n"
                 "sub r2, r1.b, c2.r\n"
				 "crs_sat r1.b, r2, c4\n"
                 "add r1.b, r1.b, c2.r\n"
                 "mul r1.b, r1.b, c1.r\n"

				 "mov oC0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// nrm -ve sat, abs +ve sat 
	AddShaderDef(" nrm -ve sat, abs +ve sat",
                 "def c0, 3.0, 3.0, 3.0, 3.0\n"
                 "def c1, 0.33, 0.33, 0.33, 0.33\n"
                 "def c2, 1.0, 1.0, 1.0, 1.0\n"
                 "def c3, 0.0, 0.0, 0.0, 1.0\n"
                 "def c4, 1.0, 0.0, 1.0, 1.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "texld r0,t0,s0\n"
				 "mov r1, c3 \n"

				 // nrm - sat ; nrm cannot have a +ve sat
                 "mul r1.r, r0.r, c0.r\n"
                 "sub r1.r, r1.r, c2.r\n"
				 "nrm_sat r2.xyz, r1\n"
                 "add r1.r, r2.r, c2.r\n"
                 "mul r1.r, r1.r, c1.r\n"

				 // abs + sat ; abs cannot have a -ve sat
                 "mul r1.g, r0.r, c0.r\n"
                 "sub r1.g, r1.g, c2.r\n"
				 "abs_sat r1.g, r1.g\n"
                 "add r1.g, r1.g, c2.r\n"
                 "mul r1.g, r1.g, c1.r\n"

				 "mov oC0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

    return S_OK;
}

bool CPSOps::GenerateTextureData()
{
	// Checker2
	if ( !m_Checker2Image.Load("checker2.bmp", 1.0f) )
    {
        WriteToLog(_T("(ERROR) Failed to load checker2.bmp.  Aborting.\n"));
        return false;
    }
	
    // DP3
	if ( !m_DP3Image.Load("DP3.bmp", 0.1f) )
    {
        WriteToLog(_T("(ERROR) Failed to load DP3.bmp.  Aborting.\n"));
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

	//Now bump data
	//Sine Texture.
	if ( !GenerateBumpMap(&m_SineBumpImage) )
	{
		WriteToLog(_T("(ERROR) Failed to generate sine data.  Aborting.\n"));
		return false;
	}

	return CPixelShader::GenerateTextureData();
}

bool CPSOps::LoadTextures()
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
	
	// Now do the bump map textures 
	if (m_bBump)
	{
		if (!SetTextureFormat(BUMPFORMAT))
		{
			WriteToLog("(WARNING) No common BumpDuDv formats.\n");
			return false;
		}
		else
		{
			pFormat = &m_pBumpFormats[m_pCurrentMode->nTextureFormat];

			if (!CreateTexture(LARGE_TEXSIZE, LARGE_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pSineBumpTex))
			{
				WriteToLog(_T("(ERROR) Failed to create bump environment map sine texture.  Aborting.\n"));
				return false; 
			}
			if (!m_pSineBumpTex->LoadImage(dwLevels, &m_SineBumpImage))
			{
				WriteToLog(_T("Unable to load bump sine texture.\n"));
				return false;
			}
			// Reset to current non bump format
			SetTextureFormat(TEXFORMAT);
		}
	}

    return CPixelShader::LoadTextures();
}
