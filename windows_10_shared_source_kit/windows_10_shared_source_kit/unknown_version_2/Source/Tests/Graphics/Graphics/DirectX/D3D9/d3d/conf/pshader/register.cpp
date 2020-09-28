// FILE:        register.cpp
// DESC:        pixel shader reg conformance tests
// AUTHOR:      Anthony Clarke

#include <ddraw.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "PShader.h"

extern MYTLVERTEX g_FourColorVertices[];
extern MYTLVERTEXTEX2 g_FourColorVerticesTex2[];
extern MYTLVERTEXTEX4 g_FourColorVerticesTex4[];
extern MYTLVERTEXTEX6 g_FourColorVerticesTex6[];
extern MYTLVERTEXTEX8 g_FourColorVerticesTex8[];
extern MYTLVERTEX3 g_Texm3Vertices[];
extern MYCUBETLVERTEX g_TexmZtests[];


CPSRegister::CPSRegister()
{
    m_szTestName = "Touch all Registers";
    m_szCommandKey = "Register";
}

UINT CPSRegister::TestInitialize()
{
	m_byFormats = TEXFORMAT;
    UINT uResult = CPixelShader::TestInitialize();
    if ( D3DTESTINIT_RUN != uResult )
		return uResult;

    PopulateShaderInfo();

    SetTestRange(1, m_vShaders.size() * m_uTotalFormats * m_uNumShaderVersions);

    return D3DTESTINIT_RUN;
}

HRESULT CPSRegister::PopulateShaderInfo()
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

	// ********************* use each texture register ***************************
    // t0
	AddShaderDef("mov from t0",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mov r0, t0\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);


    // t1
	AddShaderDef("mov from t1",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mov r0, t1\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    
    // t2
	AddShaderDef("mov from t2",
                 "tex t2\n"
                 "mov r0, t2\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);

    // t3
	AddShaderDef("mov from t3",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mov r0, t3\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);


	// ********************* use each color register ***************************
    // v0
	AddShaderDef("mov from v0",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mov r0, v0\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);

    // v1
	AddShaderDef("mov from v1",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mov r0, v1\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);

	// ********************* add registers testing constants ***************************
    // t2 + c0
	AddShaderDef("t2 + c0",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "add r0, t2, c0\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
	AddConstantsToLastShaderDef(fConstants, 4);
   
    // t3 + c1
	AddShaderDef("t3 + c1",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "add r0, t3, c1\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);    
    AddConstantsToLastShaderDef(fConstants, 8);
    SetConstantsStart(0);
    
    // t2 + c2
    AddShaderDef("t2 + c2",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "add r0, t2, c2\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddConstantsToLastShaderDef(&fConstants[8], 4);
    SetConstantsStart(2);
     
    // t3 + c3
    AddShaderDef("t3 + c3",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "add r0, t3, c3\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddConstantsToLastShaderDef(&fConstants[12], 4);
    SetConstantsStart(3);

    // t2 + c4
	AddShaderDef("t2 + c4",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "add r0, t2, c4\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddConstantsToLastShaderDef(&fConstants[16], 4);
    SetConstantsStart(4);

    // t3 + c5
	AddShaderDef("t3 + c5",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "add r0, t3, c5\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddConstantsToLastShaderDef(fConstants, 24);

    // t2 + c6
	AddShaderDef("t2 + c6",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "add r0, t2, c6\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddConstantsToLastShaderDef(&fConstants[24], 4);
    SetConstantsStart(6);


	// t3 + c7
	AddShaderDef("t3 + c7",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "add r0, t3, c7\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddConstantsToLastShaderDef(&fConstants[28], 4);
    SetConstantsStart(7);

	//  ********************* use r1 ***************************

	// lrp to r1
	AddShaderDef("lrp to r1",
                 "tex t0\n"
                 "mov r0, c0\n"
                 "lrp r1, t0, c1, v0\n"
				 "add r0, r0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);
    AddConstantsToLastShaderDef(&fConstants[16], 4);

	//  ******** port checks for constant and temporary ***********
	AddShaderDef("r(n) -> 2 ports",
                 "tex t0\n"
				 "mov r1, t0\n"
				 "mov r0, r1\n"
				 "mul r0, r0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	AddShaderDef("c(n) -> 2 ports",
				 "mad r0, v0, c2, c3\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(&fConstants[8], 8);
	SetConstantsStart(2);


// *************************** Version 1.1 ***************************
	// Can use all the 1.0 shaders as 1.1
	// The only difference between 1.0 and 1.1 shaders is that
	// the texture registers are read/write in 1.1. In 1.0 read only.
	// Also port counts are different see below.
	//mov from t0 to t1 to t2 to t3
	AddShaderDef("mov from t0 to t1 to t2 to t3",
                 "tex t0\n"
                 "mov t1, t0\n"
                 "mov t2, t1\n"
                 "mov t3, t2\n"
                 "mov r0, t3\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 1));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);

	//mov from t3 to t2 to t1 to t0
	AddShaderDef("mov from t3 to t2 to t1 to t0",
                 "tex t3\n"
                 "mov t2, t3\n"
                 "mov t1, t2\n"
                 "mov t0, t1\n"
                 "mov r0, t0\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, PSVERSION(1, 1));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);

	//  *********** port checks for texture and color ***********
	AddShaderDef("t(n) -> 2 ports",
                 "tex t0\n"
				 "tex t1\n"
				 "mul r0, t1, t0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1, 1));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	AddShaderDef("t(n) -> 2 ports",
                 "tex t0\n"
				 "tex t3\n"				 
				 "mul r0, t0, t3\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, PSVERSION(1, 1));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex, 3);

	AddShaderDef("v(n) -> 2 ports",
				 "mul r0, v1, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 1));


// *************************** Version 1.2 ***************************
	// Can use all the 1.0 and 1.1 shaders as 1.2
	
	// Should do port checks here
	// In 1.2 shaders both the temp and texture registers have a port count of 3
	// In 1.1 all registers have a port count of 2.
	// In 1.0 the texture and color register have a port count of 1 all others are 2

	//  ************** port checks for texture and temporary registers ***********
	AddShaderDef("t(n) -> 3 ports",
                 "tex t0\n"
				 "tex t1\n"
				 "tex t2\n"
				 "mad r0, t0, t1, t2\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, PSVERSION(1, 2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);

	AddShaderDef("r(n) -> 3 ports",
                 "tex t0\n"
				 "mov_d2 r1, t0\n"
				 "mov_d2 r0, t0\n"
				 "mad r0, r1, r0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	
// *************************** Version 1.3 ***************************
	// Can use all the 1.0, 1.1 and 1.2 shaders as 1.3


// *************************** Version 1.4 ***************************
	// 1.4 shaders are syntactically different from the preceding 
	// shader versions and hence can not be used in down level
	// versions. However any hardware that supports 1.4 must support
	// all previous 1.x versions.

	// texld from stage 1
	AddShaderDef("texld r1 t0",
                 "texld r1, t0\n"
				 "mov r0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex, 1);

	// texld from stage 2
	AddShaderDef("texld r2 t0",
                 "texld r2, t0\n"
				 "mov r0, r2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
	AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex, 2);
	
	// texld from stage 3
	AddShaderDef("texld r3 t0",
                 "texld r3, t0\n"
				 "mov r0, r3\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex, 3);

	// texld from stage 4
	AddShaderDef("texld r4 t0",
                 "texld r4, t0\n"
				 "mov r0, r4\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));	
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex, 4);

	// texld from stage 5
	AddShaderDef("texld r5 t0",
                 "texld r5, t0\n"
				 "mov r0, r5\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex, 5);
		
	//mov from r0 to r1 to r2 to r3 to r4 to r5
	AddShaderDef("mov from r0 -> r5",
                 "texld r0, t0\n"
                 "mov r1, r0\n"
                 "mov r2, r1\n"
                 "mov r3, r2\n"
				 "mov r4, r3\n"
				 "mov r5, r4\n"
                 "mov r0, r5\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);

	//mov from r5 to r4 to r3 to r2 to r1 to r0
	AddShaderDef("mov from r5 ->r0",
                 "texld r5, t5\n"
				 "mov r4, r5\n"
				 "mov r3, r4\n"
                 "mov r2, r3\n"
                 "mov r1, r2\n"
                 "mov r0, r1\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);

	// texld from stage 5 using TC t0
	AddShaderDef("r5 TC t0",
                 "texld r5, t0\n"
				 "mov r0, r5\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex, 5);

	// texld from stage 0 using TC t5
	AddShaderDef("r0 TC t5",
                 "texld r0, t5\n",
                 MYTLVERTEX3_FVF, g_Texm3Vertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	SetCoordsForLastPSStages(5, 5);
	SetLastShaderWrapAndMip();

	// ********************* use each color register ***************************
    // v0
	AddShaderDef("mov from v0",
                 "mov r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));

    // v1
	AddShaderDef("mov from v1",
                 "mov r0, v1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));

	// ********************* add registers testing constants ***************************
    // r4 + c0
	AddShaderDef("r4 + c0",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
                 "texld r2, t2\n"
                 "texld r3, t3\n"
				 "texld r4, t4\n"
				 "texld r5, t5\n"
                 "add r0, r4, c0\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));    
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);	
	AddConstantsToLastShaderDef(fConstants, 4);
   
    // r5 + c1
	AddShaderDef("r5 + c1",
                 "texld r5, t5\n"
				 "texld r4, t4\n"
                 "texld r3, t3\n"
                 "texld r2, t2\n"
                 "texld r1, t1\n"
				 "texld r0, t0\n"
                 "add r0, r5, c1\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);    
    AddConstantsToLastShaderDef(fConstants, 8);
    SetConstantsStart(0);
    
    // r4 + c2
    AddShaderDef("r4 + c2",
                 "texld r3, t3\n"
				 "texld r1, t1\n"
				 "texld r5, t5\n"
				 "texld r0, t0\n"
				 "texld r4, t4\n"                 
                 "texld r2, t2\n"                 
                 "add r0, r4, c2\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddConstantsToLastShaderDef(&fConstants[8], 4);
    SetConstantsStart(2);
     
    // r5 + c3
    AddShaderDef("r5 + c3",
                 "texld r5, t5\n"
                 "add r0, r5, c3\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddConstantsToLastShaderDef(&fConstants[12], 4);
    SetConstantsStart(3);

    // r4 + c4
	AddShaderDef("r4 + c4",
                 "texld r4, t4\n"
                 "add r0, r4, c4\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
	AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddConstantsToLastShaderDef(&fConstants[16], 4);
    SetConstantsStart(4);

    // r5 + c5
	AddShaderDef("r5 + c5",
                 "texld r0, t0\n"
                 "texld r1, t1\n"
                 "texld r2, t2\n"
                 "texld r5, t5\n"
                 "add r0, r5, c5\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddConstantsToLastShaderDef(fConstants, 24);

    // r4 + c6
	AddShaderDef("r4 + c6",
                 "texld r4, t4\n"
                 "add r0, r4, c6\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddConstantsToLastShaderDef(&fConstants[24], 4);
    SetConstantsStart(6);

	// r5 + c7
	AddShaderDef("r5 + c7",
                 "texld r5, t5\n"
                 "add r0, r5, c7\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
	AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddConstantsToLastShaderDef(&fConstants[28], 4);
    SetConstantsStart(7);

	//  ************** port checks for texture and temporary registers ***********
	AddShaderDef("r(n) -> 3 ports",
                 "texld r0, t0\n"
				 "mov r1, r0\n"
				 "mov r2, r1\n"
				 "mad r0, r1, r2, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	AddShaderDef("c(n) -> 2 ports",
				 "mad r0, v0, c2, c3\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(&fConstants[8], 8);
	SetConstantsStart(2);

	AddShaderDef("v(n) -> 2 ports",
				 "mul r0, v1, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));

// *************************** Version 2.0 ***************************

	// ********************* use oDepth register ***************************
    // oDepth

	AddShaderDef("oDepth register",                 
                 "def c1, 0.25, 0.25, 0, 0 \n"
				 "dcl t0.xyz \n"
				 "dcl_2d s3 \n"
				 "dcl_2d s5 \n"
				 "texld r5, t0, s5 \n"
				 "sub r5, r5, c1 \n"
				 "texld r3, t0, s3 \n"				 
				 "mov oDepth, r3.g \n"
				 "mov oC0, r5 \n",
                 MYCUBETLVERTEX_FVF, g_TexmZtests, SHADE_NEEDZ, PSVERSION(2, 0));	
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex, 3);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex, 5);

	// ********************* use each color register ***************************
    // v0
	AddShaderDef("v0 diffuse register",
				 "dcl v0\n"
                 "mov oC0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));

    // v1
	AddShaderDef("v1 specular register",
				 "dcl v1\n"
                 "mov oC0, v1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));

    // v0 + v1
	AddShaderDef("v0 + v1",
				 "dcl v0\n"
				 "dcl v1\n"
				 "mov r0, v0\n"
				 "add r0, r0, v1\n"
                 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));

    // v1 * v0
	AddShaderDef("v0 * v1",
				 "dcl v0\n"
				 "dcl v1\n"
				 "mov r0, v1\n"
				 "mul r0, r0, v0\n"
                 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));

	// ********************* use each texture register ***************************
	// texld using t0
	AddShaderDef("texld t0 s0",
				 "dcl t0.xy \n"
				 "dcl_2d s0 \n"
                 "texld r0, t0, s0\n"
				 "mov oC0, r0 \n",
                 MYTLVERTEXTEX8_FVF, g_FourColorVerticesTex8, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// texld using t1
	AddShaderDef("texld t1 s0",
				 "dcl t1.xy \n"
				 "dcl_2d s0 \n"
                 "texld r1, t1, s0\n"
				 "mov oC0, r1 \n",
                 MYTLVERTEXTEX8_FVF, g_FourColorVerticesTex8, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	// texld using t2
	AddShaderDef("texld t2 s0",
				 "dcl t2.xy \n"
				 "dcl_2d s0 \n"
                 "texld r2, t2, s0\n"
				 "mov oC0, r2 \n",
                 MYTLVERTEXTEX8_FVF, g_FourColorVerticesTex8, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	// texld using t3
	AddShaderDef("texld t3 s0",
				 "dcl t3.xy \n"
				 "dcl_2d s0 \n"
                 "texld r3, t3, s0\n"
				 "mov oC0, r3 \n",
                 MYTLVERTEXTEX8_FVF, g_FourColorVerticesTex8, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);

	// texld using t4
	AddShaderDef("texld t4 s0",
				 "dcl t4.xy \n"
				 "dcl_2d s0 \n"
                 "texld r4, t4, s0\n"
				 "mov oC0, r4 \n",
                 MYTLVERTEXTEX8_FVF, g_FourColorVerticesTex8, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// texld using t5
	AddShaderDef("texld t5 s0",
				 "dcl t5.xy \n"
				 "dcl_2d s0 \n"
                 "texld r5, t5, s0\n"
				 "mov oC0, r5 \n",
                 MYTLVERTEXTEX8_FVF, g_FourColorVerticesTex8, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	// texld using t6
	AddShaderDef("texld t6 s0",
				 "dcl t6.xy \n"
				 "dcl_2d s0 \n"
                 "texld r6, t6, s0\n"
				 "mov oC0, r6 \n",
                 MYTLVERTEXTEX8_FVF, g_FourColorVerticesTex8, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	// texld using t7
	AddShaderDef("texld t7 s0",
				 "dcl t7.xy \n"
				 "dcl_2d s0 \n"
                 "texld r7, t7, s0\n"
				 "mov oC0, r7 \n",
                 MYTLVERTEXTEX8_FVF, g_FourColorVerticesTex8, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);

	// ********************* use each sampler register ***************************
	// texld from s0
	AddShaderDef("texld t0 s0",
				 "dcl t0.xy \n"
				 "dcl_2d s0 \n"
                 "texld r0, t0, s0 \n"
				 "mov oC0, r0 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex, 0);

	// texld from s1
	AddShaderDef("texld t0 s1",
				 "dcl t0.xy \n"
				 "dcl_2d s1 \n"
                 "texld r1, t0, s1 \n"
				 "mov oC0, r1 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex, 1);
	
	// texld from s2
	AddShaderDef("texld t0 s2",
				 "dcl t0.xy \n"
				 "dcl_2d s2 \n"
                 "texld r2, t0, s2 \n"
				 "mov oC0, r2 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex, 2);

	// texld from s3
	AddShaderDef("texld t0 s3",
				 "dcl t0.xy \n"
				 "dcl_2d s3 \n"
                 "texld r3, t0, s3 \n"
				 "mov oC0, r3 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));	
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex, 3);

	// texld from s4
	AddShaderDef("texld t0 s4",
				 "dcl t0.xy \n"
				 "dcl_2d s4 \n"
                 "texld r4, t0, s4 \n"
				 "mov oC0, r4 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex, 4);

	// texld from s5
	AddShaderDef("texld t0 s5",
				 "dcl t0.xy \n"
				 "dcl_2d s5 \n"
                 "texld r5, t0, s5 \n"
				 "mov oC0, r5 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex, 5);

	// texld from s6
	AddShaderDef("texld t0 s6",
				 "dcl t0.xy \n"
				 "dcl_2d s6 \n"
                 "texld r6, t0, s6 \n"
				 "mov oC0, r6 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex, 6);

	// texld from s7
	AddShaderDef("texld t0 s7",
				 "dcl t0.xy \n"
				 "dcl_2d s7 \n"
                 "texld r7, t0, s7 \n"
				 "mov oC0, r7 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex, 7);
	
	// texld from s8
	AddShaderDef("texld t0 s8",
				 "dcl t0.xy \n"
				 "dcl_2d s8 \n"
                 "texld r8, t0, s8 \n"
				 "mov oC0, r8 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex, 8);

	// texld from s9
	AddShaderDef("texld t0 s9",
				 "dcl t0.xy \n"
				 "dcl_2d s9 \n"
                 "texld r9, t0, s9 \n"
				 "mov oC0, r9 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));	
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex, 9);

	// texld from s10
	AddShaderDef("texld t0 s10",
				 "dcl t0.xy \n"
				 "dcl_2d s10 \n"
                 "texld r10, t0, s10 \n"
				 "mov oC0, r10 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex, 10);

	// texld from s11
	AddShaderDef("texld t0 s11",
				 "dcl t0.xy \n"
				 "dcl_2d s11 \n"
                 "texld r11, t0, s11 \n"
				 "mov oC0, r11 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex, 11);

	// texld from s12
	AddShaderDef("texld t0 s12",
				 "dcl t0.xy \n"
				 "dcl_2d s12 \n"
                 "texld r0, t0, s12 \n"
				 "mov oC0, r0 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex, 12);

	// texld from s13
	AddShaderDef("texld t0 s13",
				 "dcl t0.xy \n"
				 "dcl_2d s13 \n"
                 "texld r1, t0, s13 \n"
				 "mov oC0, r1 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex, 13);
	
	// texld from s14
	AddShaderDef("texld t0 s14",
				 "dcl t0.xy \n"
				 "dcl_2d s14 \n"
                 "texld r4, t0, s14 \n"
				 "mov oC0, r4 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex, 14);

	// texld from s15
	AddShaderDef("texld t0 s15",
				 "dcl t0.xy \n"
				 "dcl_2d s15 \n"
                 "texld r11, t0, s15 \n"
				 "mov oC0, r11 \n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2, 0));	
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex, 15);

	return S_OK;
}
