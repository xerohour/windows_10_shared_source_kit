// FILE:        address.cpp
// DESC:        pixel shader address conformance tests
// Modified:	Anthony Clarke
// Modified:	Ver2.0 added  Bobby Paulose

#include <ddraw.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "PShader.h"

extern MYTLVERTEX g_FourColorVertices[];
extern MYTLVERTEXTEX2 g_FourColorVerticesTex2[];
extern MYTLVERTEX g_FourColorAlphaVertices[];
extern MYTLVERTEXTEX2 g_FourColorAlphaVerticesTex2[];
extern MYVOLTLVERTEX g_TexKillVertices[];
extern MYCUBETLVERTEX3 g_Cube3Vertices[], g_VolumeVertices[];
extern MYCUBETLVERTEX2 g_Texm2Vertices[];
extern MYTLVERTEX3 g_Texm3Vertices[];
extern MYCUBETLVERTEX g_TexmZtests[];
extern MYCUBETLVERTEX2 g_DP3[];
extern MYTLVERTEXPERSP g_FourColorVerticesPerspective[];
extern MYTLVERTEXPERSP g_FourColorVerticesMipBias[];
extern MYVOLTLVERTEX g_VolTexVertices[];

CPSAddress::CPSAddress()
{
    m_szTestName = "Texture address instructions";
    m_szCommandKey = "Address";

	m_bBump = false;
	m_bCube = false;
	m_bVol = false;

	m_pStripesVecBx2 = NULL;    
	m_pVecSomeBx2 = NULL;
	m_pStripesTex = NULL;
	m_pAddStripesTex = NULL;
	m_pGradientTex = NULL;
    m_pSineBumpTex = NULL;
    m_pCubeTex = NULL;
    m_pVolTex = NULL;

	int i;
    for (i = 0; i < 6; ++i)
        m_pCubeImage[i] = NULL;
    for (i = 0; i < VOLUME_DEPTH; ++i)
        m_pVolImage[i] = NULL;
}

CPSAddress_Bump::CPSAddress_Bump()
{
    m_szTestName = "Texture address instructions (Bump)";
    m_szCommandKey = "Address_Bump";

	m_bBump = true;
}

CPSAddress_Cube::CPSAddress_Cube()
{
    m_szTestName = "Texture address instructions (Cube)";
    m_szCommandKey = "Address_Cube";

	m_bCube = true;
}

CPSAddress_Vol::CPSAddress_Vol()
{
    m_szTestName = "Texture address instructions (Volume)";
    m_szCommandKey = "Address_Volume";

	m_bVol = true;
}

CPSAddress_BumpCube::CPSAddress_BumpCube()
{
    m_szTestName = "Texture address instructions (BumpCube)";
    m_szCommandKey = "Address_BumpCube";

	m_bBump = true;
	m_bCube = true;
}

void CPSAddress::ReleaseTextures()
{
	RELEASE(m_pStripesVecBx2);
	RELEASE(m_pVecSomeBx2);
    RELEASE(m_pCubeTex);
	RELEASE(m_pStripesTex);
	RELEASE(m_pAddStripesTex);
	RELEASE(m_pGradientTex);	
    RELEASE(m_pSineBumpTex);
    RELEASE(m_pVolTex);
    RELEASE(m_pMipsTex);
	CPixelShader::ReleaseTextures();
	return;
}

void CPSAddress::ReleaseTextureData()
{
	int i;
    for (i = 0; i < 6; ++i)
        SAFEDELETE(m_pCubeImage[i]);
    for (i = 0; i < MIP_LEVELS; ++i)
        SAFEDELETE(m_pMipImage[i]);
    for (i = 0; i < VOLUME_DEPTH; ++i)
        SAFEDELETE(m_pVolImage[i]);
    CPixelShader::ReleaseTextureData();
}

UINT CPSAddress::TestInitialize()
{
	m_byFormats = TEXFORMAT;
	
	if (m_bBump)
		m_byFormats |= BUMPFORMAT;
	
	if (m_bCube)
		m_byFormats |= CUBEFORMAT;
	
	if (m_bVol)
		m_byFormats |= VOLUMEFORMAT;

    UINT uResult = CPixelShader::TestInitialize();
    if ( D3DTESTINIT_RUN != uResult )
		return uResult;

	if (m_bBump && m_bCube)
		PopulateShaderInfo_BumpCube();
	else if (m_bVol)
		PopulateShaderInfo_Vol();
	else if (m_bCube)
		PopulateShaderInfo_Cube();
	else if (m_bBump)
		PopulateShaderInfo_Bump();
	else 
		PopulateShaderInfo();

	SetTestRange(1, m_vShaders.size() * m_uTotalFormats * m_uNumShaderVersions);

    return D3DTESTINIT_RUN;
}

HRESULT CPSAddress::PopulateShaderInfo_Bump()
{
    // texbem
	AddShaderDef("Address: texbem",
                 "tex t0\n"
                 "texbem t1, t0\n"
                 "mov r0, t1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, SHADE_BUMP, m_dwMinPSVersion);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pSineBumpTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);
    SetLastShaderTexFormats(TEXFORMAT | BUMPFORMAT);

    // texbeml
	AddShaderDef("Address: texbeml",
                 "tex t0\n"
                 "texbeml t1, t0\n"
                 "mov r0, t1\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, SHADE_LBUMP, m_dwMinPSVersion);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pSineBumpTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);
    SetLastShaderTexFormats(TEXFORMAT | BUMPFORMAT);

	return S_OK;
}

HRESULT CPSAddress::PopulateShaderInfo_Cube()
{
 	//0.7x, 0.1y, 0.5z creates a vector that points more in x and z then in y
	// This means that when it is used for indexing cube environment maps 
	// in spec op the + and - x faces of the cubes will be display/addressed
   float fConstant[4] = { 0.7f, 0.1f, 0.5f, 0.0f };

	// texm3x3pad and texm3x3tex
	AddShaderDef("Address: texm3x3tex _bx2",
                 "tex t0\n"
                 "texm3x3pad t1, t0_bx2\n"
                 "texm3x3pad t2, t0_bx2\n"
                 "texm3x3tex t3, t0_bx2\n"
				 "mov r0, t3\n",						//0th texcoords are not used 1-3 are.
                 MYCUBETLVERTEX3_FVF, g_Cube3Vertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesVecBx2);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);	
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCubeTex);
	SetCoordsForLastPSStages(1, 1);
	SetCoordsForLastPSStages(2, 2);
	SetCoordsForLastPSStages(3, 3);
	SetLastShaderTexFormats(TEXFORMAT | CUBEFORMAT);

    // texm3x3spec
	AddShaderDef("Address: texm3x3spec",
                 "tex t0\n"
                 "texm3x3pad t1, t0_bx2\n"
                 "texm3x3pad t2, t0_bx2\n"
                 "texm3x3spec t3, t0_bx2, c0\n"
                 "mov r0, t3\n",
                 MYCUBETLVERTEX3_FVF, g_Cube3Vertices, SHADE_NOLORA, m_dwMinPSVersion);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesVecBx2);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCubeTex);
    AddConstantsToLastShaderDef(fConstant, 4);
	SetCoordsForLastPSStages(1, 1);
	SetCoordsForLastPSStages(2, 2);
	SetCoordsForLastPSStages(3, 3);
    SetLastShaderTexFormats(TEXFORMAT | CUBEFORMAT);

	// texm3x3pad and texm3x3vspec
	AddShaderDef("Address: texm3x3vspec",
                 "tex t0\n"
                 "texm3x3pad t1, t0_bx2\n"
                 "texm3x3pad t2, t0_bx2\n"
                 "texm3x3vspec t3, t0_bx2\n"
                 "mov r0, t3\n",
                 MYCUBETLVERTEX3_FVF, g_Cube3Vertices, SHADE_NOLORA, m_dwMinPSVersion);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesVecBx2);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCubeTex);
	SetCoordsForLastPSStages(1, 1);
	SetCoordsForLastPSStages(2, 2);
	SetCoordsForLastPSStages(3, 3);
    SetLastShaderTexFormats(TEXFORMAT | CUBEFORMAT);

	//texm3x3tex partial _bx2
	AddShaderDef("Address: texm3x3tex partial _bx2",
                 "tex t0\n"
                 "texm3x3pad t1, t0_bx2\n"
                 "texm3x3pad t2, t0\n"
                 "texm3x3tex t3, t0_bx2\n"
				 "mov r0, t3\n",
                 MYCUBETLVERTEX3_FVF, g_Cube3Vertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pVecSomeBx2);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);	
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCubeTex);
	SetCoordsForLastPSStages(1, 1);
	SetCoordsForLastPSStages(2, 2);
	SetCoordsForLastPSStages(3, 3);
    SetLastShaderTexFormats(TEXFORMAT | CUBEFORMAT);

    // texm3x3pad and texm3x3tex
    // This is done to check that texture co-ordinate stages can be
    // re-ordered in a arbitrary fashion.
	AddShaderDef("Address: Arbitrary TEXCOORDINDEX",
                 "tex t0\n"
                 "texm3x3pad t1, t0_bx2\n"
                 "texm3x3pad t2, t0_bx2\n"
                 "texm3x3tex t3, t0_bx2\n"
				 "mov r0, t3\n",
                 MYCUBETLVERTEX3_FVF, g_Cube3Vertices, PSVERSION(1, 1));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesVecBx2);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);	
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCubeTex);
	SetCoordsForLastPSStages(1, 3);
	SetCoordsForLastPSStages(2, 1);
	SetCoordsForLastPSStages(3, 2);
	SetLastShaderTexFormats(TEXFORMAT | CUBEFORMAT);

	//texreg2rgb_bx2
	AddShaderDef("Address: texreg2rgb_bx2",
				 "tex t0\n"
                 "texreg2rgb t1, t0_bx2\n"
                 "mov r0, t1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesVecBx2);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pCubeTex);
    SetLastShaderTexFormats(TEXFORMAT | CUBEFORMAT);

	//texld r#, t#.xyz Uses xyz of t# to sample 1-D, 2-D or 3-D texture
	AddShaderDef("Address: t0.xyz",
                 "texld r0, t0.xyz\n",
                 MYCUBETLVERTEX2_FVF, g_Texm2Vertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCubeTex);
    SetLastShaderTexFormats(TEXFORMAT | CUBEFORMAT);

	//texld r#, t#.xyw Uses xyw (skipping z) from t# to sample 1-D, 2-D or 3-D texture.
	AddShaderDef("Address: t0.xyw",
                 "texld r0, t0.xyw\n",
                 MYTLVERTEX3_FVF, g_Texm3Vertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCubeTex);
    SetLastShaderTexFormats(TEXFORMAT | CUBEFORMAT);

	// texld cube tex
	AddShaderDef("Address: texld cube tex",
				 "dcl t0.xyz\n"
				 "dcl_cube s0\n"
				 "texld r0,t0,s0\n"
				 "mov oC0, r0\n",
                 MYCUBETLVERTEX2_FVF, g_Texm2Vertices, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCubeTex);
	SetLastShaderTexFormats(TEXFORMAT | CUBEFORMAT);

	return S_OK;
}

HRESULT CPSAddress::PopulateShaderInfo_Vol()
{
    //Check if volume textures are supported.
    if ( m_Flags.Check(VOLUMECAP) )
    {
        // Test a volume texture address op
        // texm3x3pad and texm3x3tex
	    AddShaderDef("Address: texm3x3tex volume",
                 "tex t0\n"
                 "texm3x3pad t1, t0\n"
                 "texm3x3pad t2, t0\n"
                 "texm3x3tex t3, t0\n"
				 "mov r0, t3\n",						//0th texcoords are not used 1-3 are.
                 MYCUBETLVERTEX3_FVF, g_VolumeVertices);
        AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesVecBx2);
        AddTextureToLastShaderDef(NULL);
        AddTextureToLastShaderDef(NULL);	
        AddTextureToLastShaderDef((CnBaseTexture**)&m_pVolTex);
	    SetCoordsForLastPSStages(1, 1);
	    SetCoordsForLastPSStages(2, 2);
	    SetCoordsForLastPSStages(3, 3);
	    SetLastShaderTexFormats(TEXFORMAT | VOLUMEFORMAT);
		
		// Test a volume texture address op
        // texm3x3pad and texm3x3tex
	    AddShaderDef("Address: texm3x3tex volume 1.4 style",
                 "texld r0, t0\n"
                 "texcrd r1.rgb, t1\n"
				 "texcrd r5.rgb, t2\n"
				 "texcrd r3.rgb, t3\n"
				 "dp3 r1, r1, r0\n"
				 "dp3 r5, r5, r0\n"
				 "dp3 r3, r3, r0\n"
				 "mov r0.r, r1\n"
				 "mov r0.g, r5\n"
				 "mov r0.b, r3\n"
                 "phase\n"
				 "texld r3, r0\n"
				 "mov r0, r3\n",
                 MYCUBETLVERTEX3_FVF, g_VolumeVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
        AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesVecBx2);
        AddTextureToLastShaderDef(NULL);
        AddTextureToLastShaderDef(NULL);	
        AddTextureToLastShaderDef((CnBaseTexture**)&m_pVolTex);
	    SetCoordsForLastPSStages(1, 1);
	    SetCoordsForLastPSStages(2, 2);
	    SetCoordsForLastPSStages(3, 3);
	    SetLastShaderTexFormats(TEXFORMAT | VOLUMEFORMAT);
  
		// texld volume tex
		AddShaderDef("Address: texld volume tex",
					 "dcl t0.xyz\n"
					 "dcl_volume s0\n"
					 "texld r0.rgba,t0,s0\n"
					 "mov oC0, r0\n",
					 MYVOLTLVERTEX_FVF, g_VolTexVertices, true, SHADE_PLAIN, PSVERSION(2, 0));
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pVolTex ); 
		SetLastShaderTexFormats(TEXFORMAT | VOLUMEFORMAT);
	}

	return S_OK;
}

HRESULT CPSAddress::PopulateShaderInfo_BumpCube()
{
    //texreg2rgb
	AddShaderDef("Address: texreg2rgb",
				 "tex t0\n"
                 "texreg2rgb t1, t0\n"
                 "mov r0, t1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_NOLORA, PSVERSION(1, 2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pSineBumpTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pCubeTex);
    SetLastShaderTexFormats(BUMPFORMAT | CUBEFORMAT);

	return S_OK;
}

HRESULT CPSAddress::PopulateShaderInfo()
{
    // ****************************** address ******************************
	// tex
    AddShaderDef("Address: tex",
                 "tex t0\n"
                 "mov r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

    // texcoord 3D
    AddShaderDef("Address: texcoord 3D",
                 "texcoord t0\n"
                 "mov r0, t0\n",
                 MYCUBETLVERTEX2_FVF,  g_DP3, true);

    // texcoord clamp
    AddShaderDef("Address: texcoord clamp",
                 "texcoord t0\n"
                 "add r0, v0, t0\n",
                 MYVOLTLVERTEX_FVF, g_TexKillVertices, true);
    
    // texkill
	AddShaderDef("Address: texkill",
                 "texkill t0\n"
                 "mov r0, v0\n",
                 MYVOLTLVERTEX_FVF, g_TexKillVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex, true);

    // texreg2ar
	AddShaderDef("Address: texreg2ar",
				 "tex t0\n"
				 "texreg2ar t1, t0\n"
                 "mov r0, t1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pAddStripesTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);

    // texreg2gb
	AddShaderDef("Address: texreg2gb",
				 "tex t0\n"
                 "texreg2gb t1, t0\n"
                 "mov r0, t1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pAddStripesTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);
	SetLastShaderWrapAndMip();

    // texm3x2pad and texm3x2tex
	AddShaderDef("Address: texm3x2tex",
                 "tex t0\n"
                 "texm3x2pad t1, t0\n"
                 "texm3x2tex t2, t0\n"
                 "mov r0, t2\n",
                 MYCUBETLVERTEX2_FVF, g_Texm2Vertices); //Only the 0th tex coord are used.
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pAddStripesTex);
	SetLastShaderWrapAndMip();

	// Test that D3DTTFF_PROJECTED render state is honored tex projected.
    AddShaderDef("Address: tex projected",
                 "tex t0\n"
                 "mov r0, t0\n",
                 MYTLVERTEX3_FVF, g_Texm3Vertices, SHADE_PROJ, m_dwMinPSVersion);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	SetLastShaderWrapAndMip();
	
// *************************** Version 1.1 ***************************
	// Can use all the 1.0 shaders as 1.1    

	// texreg2ar in this case we wish to test re-use of the source register.
	AddShaderDef("Address: texreg2ar re-use source",
				 "tex t0\n"
				 "texreg2ar t1, t0\n"
				 "texreg2gb t2, t0\n"
				 "mul r0, t1, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 1));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pAddStripesTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);


// *************************** Version 1.2 ***************************
	// Can use all the 1.0 and 1.1 shaders as 1.2

	//texdp3tex
	// Due to the gradient in the top half of the image the g and a 
	// components contribute to the dp3 (ga = yw = vq).
	// as we head for the bottom of the image rgb components contribute
	// The TC vary in u and v from top left to bottom right (0, 0) to (1,1)
	// and z varies from 0 on the left to 1 on the right. 
	AddShaderDef("Address: texdp3tex",
				 "tex t0\n"
                 "texdp3tex t1, t0\n"
                 "mov r0, t1\n",
                 MYCUBETLVERTEX2_FVF, g_DP3, PSVERSION(1, 2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);
	SetLastShaderWrapAndMip();

	//texdp3
	AddShaderDef("Address: texdp3",
				 "tex t0\n"
                 "texdp3 t1, t0_bx2\n"
                 "add r0, v0, t1\n",
                 MYCUBETLVERTEX2_FVF, g_DP3, PSVERSION(1, 2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesVecBx2);	

	// texm3x3
	AddShaderDef("Address: texm3x3",
                 "tex t0\n"
                 "texm3x3pad t1, t0\n"
                 "texm3x3pad t2, t0\n"
                 "texm3x3 t3, t0\n"
				 "mov r0, t3\n",						//0th texcoords are not used 1-3 are.
                 MYCUBETLVERTEX3_FVF, g_Cube3Vertices, PSVERSION(1, 2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesVecBx2);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	SetCoordsForLastPSStages(1, 1);
	SetCoordsForLastPSStages(2, 2);
	SetCoordsForLastPSStages(3, 3);

	// testing _bx2 functionality now all sources to text ops can use _bx2.
    
	// texm3x3 _bx2
	AddShaderDef("Address: texm3x3 _bx2",
                 "tex t0\n"
                 "texm3x3pad t1, t0_bx2\n"
                 "texm3x3pad t2, t0_bx2\n"
                 "texm3x3 t3, t0_bx2\n"
				 "mov r0, t3\n",						//0th texcoords are not used 1-3 are.
                 MYCUBETLVERTEX3_FVF, g_Cube3Vertices, PSVERSION(1, 2));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesVecBx2);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	SetCoordsForLastPSStages(1, 1);
	SetCoordsForLastPSStages(2, 2);
	SetCoordsForLastPSStages(3, 3);

// *************************** Version 1.3 ***************************
	// Can use all the 1.0, 1.1 and 1.2 shaders as 1.3
	
	//texm3x2depth
	AddShaderDef("Address: texm3x2depth",
				 "tex t0\n"				 
				 "texm3x2pad t1, t0\n"
                 "texm3x2depth t2, t0\n"
				 "tex t3\n"
                 "mov r0, t3\n",
                 MYCUBETLVERTEX_FVF, g_TexmZtests, SHADE_NEEDZ, PSVERSION(1, 3));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex, 3);	
	SetCoordsForLastPSStages(1, 1);
	SetCoordsForLastPSStages(2, 2);

// *************************** Version 1.4 ***************************

	// texld from stage 0
    AddShaderDef("Address: r0",
                 "texld r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// texld r3, t5
	AddShaderDef("Address: texld r3, t5",
                 "texld r3, t5\n"
				 "mov r0, r3\n",
                 MYTLVERTEX3_FVF, g_Texm3Vertices, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex, 3);
	SetCoordsForLastPSStages(5, 5);
	SetLastShaderWrapAndMip();

	// texld r5, t3
	AddShaderDef("Address: texld r5, t3",
                 "texld r5, t3\n"
				 "mov r0, r5\n",
                 MYTLVERTEX3_FVF, g_Texm3Vertices, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex, 5);
	SetCoordsForLastPSStages(3, 3);
	SetLastShaderWrapAndMip();

	 // dependent texture read 1
	AddShaderDef("Address: dep read 1",
				 "texcrd r0.rgb, t0\n"
				 "mov r0.rgb, r0_bx2\n"
				 "phase\n"
				 "texld r0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	SetLastShaderWrapAndMip();

	// dependent texture read 2
	AddShaderDef("Address: dep read 2",
				 "texcrd r3.rgb, t0\n"
				 "mov r5.rgb, r3_bias\n"
				 "phase\n"
				 "texld r0, r5\n",
                 MYVOLTLVERTEX_FVF, g_TexKillVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	SetLastShaderWrapAndMip();

    // texcrd 2D
	AddShaderDef("Address: texcrd 2D",
                 "def c0, 0, 0, 0, 0.5\n"
				 "texcrd r0.rgb, t0\n"
				 "mov r0.b, c0.b\n"
				 "mov r0.a, c0.a\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));

	// texcrd 3D
	AddShaderDef("Address: texcrd 3D",
                 "def c0, 0, 0, 0, 0.5\n"
				 "texcrd r0.rgb, t0\n"
				 "mov r0.a, c0.a\n",
                 MYCUBETLVERTEX2_FVF, g_Texm2Vertices, PSVERSION(1, 4));
   
    // Coord Defaults    
    AddShaderDef("Address: Coord Defaults",
                 "texld r1, t0\n"
	             "texcrd r0.rgb, t1\n"
	             "mov r0.a, r1.a\n"
                 "add r0, r1, r0_bias\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorAlphaVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex, 1);
    
    // texkill
	AddShaderDef("Address: texkill",  
                 "texkill t0\n"
				 "mov r0, v0\n",
                 MYVOLTLVERTEX_FVF, g_TexKillVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// texkill
	AddShaderDef("Address: texkill r3",
				 "def c0, 0.25, 0.25, 0.25, 0.25\n"
				 "texcrd r3.rgb, t0\n"
				 "add r3.rgb, r3, c0\n"
                 "phase\n"
				 "texkill r3\n"
				 "mov r0, v0\n",
                 MYVOLTLVERTEX_FVF, g_TexKillVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	//texdepth t0
	// texdepth  :  uses the r and g channel in the specified register to 
	// replace the depth value in the depth buffer.   
	// The r channel is treated as the z value and the g is treated and the w value.  
	// The depth value in the depth buffer is replaced by the result of the r channel 
	// divided by the g channel, z/w.  If the value in g is zero then the depth buffer 
	// is updated with 1.0.
	AddShaderDef("Address: texdepth",                 
                 "def c1, 0.25, 0.25, 0, 0\n"
				 "texld r5, t0\n"
				 "sub r5, r5, c1\n"
				 "phase\n"
				 "texld r3, t0\n"				 
				 "texdepth r5\n"
				 "mov r0, r3\n",
                 MYCUBETLVERTEX_FVF, g_TexmZtests, SHADE_NEEDZ, PSVERSION(1, 4));	
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex, 3);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex, 5);

	//texld r#, t#_dw.xyw Samples 1-D or 2-D texture  at x/w, y/w. (Cubemap lookup result undefined)
	AddShaderDef("Address: t0_dw.xyw",
                 "texld r0, t0_dw.xyw\n",
                 MYTLVERTEX3_FVF, g_Texm3Vertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);	
	SetLastShaderWrapAndMip();
	
	//texld r#, r#_dz.xyz  Samples 1-D or 2-D texture at x/z, y/z (Cubemap lookup result undefined) Same as texld r#, r#_dz
	//Also tests that D3DTTFF_PROJECTED render state is ignored is its ignored /z occurs eles /w
	AddShaderDef("Address: texcrd r1_dz.xyz",
                 "def c0, 0, 0, 0, 0.75\n"
				 "texcrd r1.rgb, t0\n"
				 "phase\n"		 
				 "texld r0, r1_dz.xyz\n",
                 MYTLVERTEX3_FVF, g_Texm3Vertices, true, SHADE_PROJ, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	SetLastShaderWrapAndMip();

	//texcrd r#.rgb, t#.xyz Copies first three channels of t# into r# (4th channel of r# uninitialized) Same as texcrd r#.rgb, t#
	AddShaderDef("Address: texcrd t0.xyz",
                 "def c0, 0, 0, 0, 0.75\n"
				 "texcrd r0.rgb, t0.xyz\n"
				 "mov r0.a, c0.a\n",
                 MYTLVERTEX3_FVF, g_Texm3Vertices, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	//texcrd r#.rgb, t#.xyw Puts 1st, 2nd and 4th components of t# into first three channels of dest (4th  channel of r# uninitialized)
	AddShaderDef("Address: texcrd r0.rgb, t0.xyw",
                 "def c0, 0, 0, 0, 1.0\n"
				 "texcrd r0.rgb, t0.xyw\n"
				 "mov r0.a, c0.a\n",
                 MYTLVERTEX3_FVF, g_Texm3Vertices, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	//texcrd  r#.rg,  t#_dw.xyw Copies x/w and y/w from source into first two channels of dest (3rd and 4th channels of r# uninitialized)
	//also texld   r#, r#.xyz Samples 1-D, 2-D or 3-D texture at source xyz (ignores source w) Same as texld  r#, r#
	AddShaderDef("Address: texcrd t0_dw.xyw / texld r0, r1.xyz",
                 "def c0, 0, 0, 0, 0.75\n"
				 "texcrd r0.rg, t0_dw.xyw\n"
				 "mov r0.b, c0\n"
				 "phase\n"				 
				 "texld r1, r0.xyz\n"
				 "mov r0, r1\n",
                 MYTLVERTEX3_FVF, g_Texm3Vertices, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex, 1);
	SetLastShaderWrapAndMip();

// *************************** Version 2.0 ***************************

	// texld 2d tex
	AddShaderDef("Address: texld 2d tex",
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "texld r0.rgba,t0,s0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	//texld sampler select
	AddShaderDef("Address: texld sampler select",
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "dcl_2d s1\n"
				 "texld r0.rgba,t0,s1\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);

	//texld t# reg select
	AddShaderDef("Address: texld t# reg select",
				 "dcl t0.xy\n"
				 "dcl t1.xy\n"
				 "dcl_2d s0\n"
				 "texld r0.rgba,t1,s0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	SetCoordsForLastPSStages(0, 0);
	SetCoordsForLastPSStages(1, 1);

	//texldp
	AddShaderDef("Address: texldp 2d tex",
				 "dcl t0.xyzw\n"
				 "dcl_2d s0\n"
				 "texldp r0.rgba,t0,s0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEXPERSP_FVF, g_FourColorVerticesPerspective, true, SHADE_PLAIN, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	SetLastShaderWrapAndMip();

	//texldb
	AddShaderDef("Address: texldb 2d tex",
				 "dcl t0.xyzw\n"
				 "dcl_2d s0\n"
				 "texldb r0.rgba,t0,s0\n"
				 "mov oC0, r0\n",
                 MYTLVERTEXPERSP_FVF, g_FourColorVerticesMipBias, true, SHADE_PLAIN_NEEDMIPMAP, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pMipsTex);
	SetLastShaderWrapAndMip();

	//texkill
	AddShaderDef("Address: texkill",
                 "def c0, 0.2, 0.2, 0.2, 0.6\n"
				 "dcl t0.xyzw\n"
				 "dcl v0.rgba\n"
				 "sub r0.rgba,t0,c0\n"
				 "texkill r0\n"
				 "mov oC0, v0\n",
                 MYTLVERTEXPERSP_FVF, g_FourColorVerticesMipBias, true, SHADE_PLAIN, PSVERSION(2, 0));

	// texld dependent read limit 4 in 2.0 test
	AddShaderDef("Address: dependent read limit 4 test",
					"dcl t0.xy \n"
					"dcl_2d s0 \n"
					"dcl_2d s1 \n"
					"dcl_2d s2.rgba \n"
					"dcl_2d s3.rgba \n"
					"def c0, 0.8, 0.8, 0.8, 0.8\n"
					"def c1, 0.2, 0.2, 0.2, 0.2\n"
					"mov r0.xy, t0 \n"
					"texld r1.rgba, r0, s0 \n"
					"texld r2.rgba, r0, s1 \n"
					"texld r3.rgba, r0, s2 \n"
					"texld r4.rgba, r0, s3 \n"
					"mul r0, r1, c0 \n"
					"mad r0, r2, c1, r0 \n"
					"mad r0, r3, c1, r0 \n"
					"mad r0, r4, c1, r0 \n"
					"mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);

	//dcl_pp t# test.

	//The _pp flag does not effect the precision of address instructions (tex* ops) 
	// but will apply to dependent reads. Also affects arithmetic operations.
	// The precision difference, if any, happens only when the data in dcl_pp t# is fed thru 
	// the arithmetic operations. We should not test such cases because the hardware may or
	// may not effect a precision change.


	AddShaderDef("dcl_pp t# test",
				"def c0, 0.33, 0.33, 0.33, 1.0\n"

				"dcl_pp t0.xyzw \n"
				"dcl_2d s0 \n"
				"dcl_2d s1 \n"
				"dcl_2d s2 \n"
				"mov r4, c0 \n"

				"texld r1, t0, s0 \n"
				"texld r2, t0, s1 \n"
				"texld r3, t0, s2 \n"

				"mul r4.rgb, r1, c0\n"
				"mad r4.rgb, r2, c0, r4\n"
				"mad r4.rgb, r3, c0, r4\n"

				"mov oC0, r4\n",
                 MYTLVERTEXPERSP_FVF, g_FourColorVerticesMipBias, true, SHADE_PLAIN, PSVERSION(2, 0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	return S_OK;
}

bool CPSAddress::GenerateTextureData()
{
	//Stripes
	D3DCOLOR pdwColors[] = {
        D3DRGBA(1, 1, 1, 1),	//white
        D3DRGBA(1, 0, 0, 1),	//red
        D3DRGBA(0, 1, 0, 1),	//green
        D3DRGBA(0, 0, 1, 1),	//blue
		D3DRGBA(1, 1, 0, 1),	//yellow
		D3DRGBA(0, 0, 0, 1),	//black
        D3DRGBA(0, 1, 1, 1),	//Brown
	    D3DRGBA(1, 0, 1, 1)
    };

	if ( !m_StripesImage.LoadStripes(LARGE_TEXSIZE, LARGE_TEXSIZE, 4, pdwColors, true, true) )
    {
        WriteToLog("(ERROR) Failed to load LoadStripes.  Aborting.\n");
        return false;
    }

//==============	
	//mip texture images. The images are different for the 4 mip levels.
	//This is done to test mip bias. So if the hardware doesn't do mip bias like
	// refrast, it'll get a totally different image.. (evil smile)
	int i;
    for (i = 0; i < MIP_LEVELS; ++i)
    {
        m_pMipImage[i] = new CImage;
        if (!m_pMipImage[i])
        {
            WriteToLog( _T("Unable to allocate CImage for cubemap") );
            return false;
        }
    }
	pdwColors[0] = D3DRGBA(1, 1, 0, 1);	//yellow
    pdwColors[1] = D3DRGBA(1, 0, 0, 1);	//red
    pdwColors[2] = D3DRGBA(0, 1, 0, 1);	//green
    pdwColors[3] = D3DRGBA(0, 0, 1, 1);	//blue

	float factor = 1.0;
    for (i = 0; i < MIP_LEVELS; ++i)
    {
		if ( !m_pMipImage[i]->LoadStripes(m_nDisplaySizePow2/(DWORD)factor, m_nDisplaySizePow2/(DWORD)factor, 1, &pdwColors[i], false) ) 
		{
			WriteToLog("(ERROR) Failed to load mip map data.  Aborting.\n");
			return false;
		}
		factor = factor * 2.f;
	}

//==========

	//Addressing Stripes Texture. (The stripes are used as texture addresses.
    pdwColors[0] = D3DRGBA(0.1, 0.9, 0.9, 0.1);
    pdwColors[1] = D3DRGBA(0.2, 0.3, 0.3, 0.2);
    pdwColors[2] = D3DRGBA(0.3, 0.2, 0.2, 0.3);
    pdwColors[3] = D3DRGBA(0.9, 0.1, 0.1, 0.9);
	pdwColors[4] = D3DRGBA(0.8, 0.8, 0.1, 0.1);
	pdwColors[5] = D3DRGBA(0.8, 0.8, 0.6, 0.6);

	if ( !m_AddStripesImage.LoadStripes(LARGE_TEXSIZE, LARGE_TEXSIZE, 6, pdwColors, true, true) )
	{
		WriteToLog("(ERROR) Failed to generate address stripes data.  Aborting.\n");
		return false;
	}	
	
	// Gradient
	DWORD dwGradColors[4] =
    {
        D3DRGBA(0, 1, 0, 1),
        D3DRGBA(0, 1, 0, 1),
        D3DRGBA(1, 1, 1, 0),
        D3DRGBA(1, 1, 1, 0)
    };
    if ( !m_GradientImage.LoadGradient(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create gradient data.  Aborting.\n");
        return false;
    }
	
	//Vectors for cubmap intersection.
	//These must be biased to be of any use as vectors ie _bx2 (X-0.5)*2;
	pdwColors[0] = BUMPRGBA(255, 127, 127, 255);	//+X of cube
	pdwColors[1] = BUMPRGBA(0, 127, 127, 255);		//-X of cube
	pdwColors[2] = BUMPRGBA(127, 255, 127, 255);	//+Y of cube
	pdwColors[3] = BUMPRGBA(127, 0, 127, 255);		//-Y of cube
	pdwColors[4] = BUMPRGBA(127, 127, 255, 255);	//+Z of cube
	pdwColors[5] = BUMPRGBA(127, 127, 0, 255);		//-Z of cube
    if ( !m_StripesVecBx2Image.LoadStripes(LARGE_TEXSIZE, LARGE_TEXSIZE, 6, pdwColors, false) )
    {
        WriteToLog("(ERROR) Failed to LoadStripes.  Aborting.\n");
        return false;
    }

	//Vectors for cubmap intersection.
	//All except the y component of this data must be _bx2'ed 
	//to be used as vectors ie _bx2 (X-0.5)*2;
	pdwColors[0] = BUMPRGBA(255, 0, 127, 255);
	pdwColors[1] = BUMPRGBA(0, 0, 127, 255);
	pdwColors[2] = BUMPRGBA(127, 255, 127, 255);
	pdwColors[3] = BUMPRGBA(127, 255, 127, 255);
	pdwColors[4] = BUMPRGBA(127, 0, 255, 255);
	pdwColors[5] = BUMPRGBA(127, 0, 255, 255);
    if ( !m_VecSomeBx2Image.LoadStripes(LARGE_TEXSIZE, LARGE_TEXSIZE, 6, pdwColors, false) )
    {
        WriteToLog("(ERROR) Failed to LoadStripes.  Aborting.\n");
        return false;
    }

	//This is the cubmap data
    for (i = 0; i < 6; ++i)
    {
        m_pCubeImage[i] = new CImage;
        if (!m_pCubeImage[i])
        {
            WriteToLog( _T("Unable to allocate CImage for cubemap") );
            return false;
        }
    }
	pdwColors[0] = D3DRGBA(1, 1, 1, 1);	//white
    pdwColors[1] = D3DRGBA(1, 0, 0, 1);	//red
    pdwColors[2] = D3DRGBA(0, 1, 0, 1);	//green
    pdwColors[3] = D3DRGBA(0, 0, 1, 1);	//blue
	pdwColors[4] = D3DRGBA(1, 1, 0, 1);	//yellow
	pdwColors[5] = D3DRGBA(0, 0, 0, 1);	//black
	if ( !m_pCubeImage[0]->LoadStripes(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, 1, &pdwColors[3], false) ) 
    {
        //+ve x blue
        WriteToLog("(ERROR) Failed to load cube map data.  Aborting.\n");
		return false;
    }
	if ( !m_pCubeImage[1]->LoadStripes(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, 1, &pdwColors[2], false) )
	{
        //-ve x green
        WriteToLog("(ERROR) Failed to load cube map data.  Aborting.\n");
		return false;
    }
    if ( !m_pCubeImage[2]->LoadStripes(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, 1, &pdwColors[0], false) )
	{
        //+ve y white
        WriteToLog("(ERROR) Failed to load cube map data.  Aborting.\n");
		return false;
    }
    if ( !m_pCubeImage[3]->LoadStripes(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, 1, &pdwColors[5], false) )
	{
        //-ve y black
        WriteToLog("(ERROR) Failed to load cube map data.  Aborting.\n");
		return false;
    }
    if ( !m_pCubeImage[4]->LoadStripes(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, 1, &pdwColors[1], false) )
	{
        //+ve z red
        WriteToLog("(ERROR) Failed to load cube map data.  Aborting.\n");
		return false;
    }
    if ( !m_pCubeImage[5]->LoadStripes(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, 1, &pdwColors[4], false) )
    {
        //-ve z yellow
        WriteToLog("(ERROR) Failed to load cube map data.  Aborting.\n");
		return false;
    }

	//Now bump data
	//Sine Texture.
	if ( !GenerateBumpMap(&m_SineBumpImage) )
	{
		WriteToLog("(ERROR) Failed to generate sine data.  Aborting.\n");
		return false;
	}

    pdwColors[6] = D3DRGBA(0, 1, 1, 1);	//Brown
	pdwColors[7] = D3DRGBA(1, 0, 1, 1);	//
    //Now Volume data
    if ( m_Flags.Check(VOLUMECAP) )
    {
        for (i = 0; i < VOLUME_DEPTH; ++i)
        {
            m_pVolImage[i] = new CImage;
            if (!m_pVolImage[i])
            {
                WriteToLog( _T("Unable to allocate CImage for volume") );
                return false;
            }
            m_pVolImage[i]->LoadStripes(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, 2, &pdwColors[(i*2)], false, false);
        }
    }
	return CPixelShader::GenerateTextureData();
}

bool CPSAddress::LoadTextures()
{
	if (!SetTextureFormat(TEXFORMAT))
		return false;

	DWORD dwLevels = (DWORD) 1, dwDepth = VOLUME_DEPTH; // Depth must be a power of 2 for most hardware.
    DWORD dwPool = POOL_DEFAULT;
    DWORD dwUsage = (DWORD) 0;
    FORMAT *pFormat = &m_pTexFormats[m_pCurrentMode->nTextureFormat];

//=========================================

	bool bMipmap = true;

	// On Dx9.L, need to check for the special A1 format
	if ((m_pD3D->GetVersion() >= 9.1f) && (pFormat->d3dfFormat == FMT_A1))
		bMipmap = false;

	//texture with 4 mip levels
	if (!CreateTexture(m_nDisplaySizePow2, m_nDisplaySizePow2, (bMipmap) ? MIP_LEVELS : dwLevels , FMWK_DEFAULT, *pFormat, dwPool, &m_pMipsTex))
	{
        WriteToLog(_T("(ERROR) Failed to create Mips texture.  Aborting.\n"));
        return false;
    }		
	if (!m_pMipsTex->LoadImage(0, m_pMipImage))
	{
		WriteToLog(_T("Unable to load Mip texture levels.\n"));
		return false;
	}
//=========================================

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

	//Addressing Stripes Texture. (The stripes are used as texture addresses.
	if (!CreateTexture(LARGE_TEXSIZE, LARGE_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pAddStripesTex))
    {
        WriteToLog(_T("(ERROR) Failed to create Addressing Stripes texture.  Aborting.\n"));
        return false; 
    }
	if (!m_pAddStripesTex->LoadImage(dwLevels, &m_AddStripesImage))
    {
        WriteToLog(_T("Unable to load addressing stripes texture.\n"));
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
		
	//Vectors for cubmap intersection.
	//These must be biased to be of any use as vectors ie _bx2 (X-0.5)*2;
    if (!CreateTexture(LARGE_TEXSIZE, LARGE_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pStripesVecBx2))   
    {
        WriteToLog(_T("(ERROR) Failed to StripesVecBx2 texture map.  Aborting.\n"));
        return false; 
    }
    if (!m_pStripesVecBx2->LoadImage(dwLevels, &m_StripesVecBx2Image))
    {
        WriteToLog(_T("Unable to load m_pStripesVecBx2 texture.\n"));
        return false;
    }
	
	//Vectors for cubmap intersection.
	//Some components need to be biased
    if (!CreateTexture(LARGE_TEXSIZE, LARGE_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pVecSomeBx2))
    {
        WriteToLog(_T("(ERROR) Failed to VectSomeBx2 texture.  Aborting.\n"));
        return false; 
    }
    if (!m_pVecSomeBx2->LoadImage(dwLevels, &m_VecSomeBx2Image))
    {
        WriteToLog(_T("Unable to load m_pVecSomeBx2 texture.\n"));
        return false;
    }

	// Now do the cube map textures
	if (m_bCube)
	{
		if (!SetTextureFormat(CUBEFORMAT))
		{
			WriteToLog(_T("(ERROR) No common Cube map formats.\n"));
			return false;
		}
		else
		{
			pFormat = &m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat];
	        
			if (!CreateCubeTexture(DEFAULT_TEXSIZE, dwLevels, dwUsage, *pFormat, (D3DPOOL)dwPool, &m_pCubeTex))
			{
				WriteToLog("(ERROR) Failed to create cubemap (%08X).  Aborting.\n", GetLastError());
				return false;
			}
			if (!m_pCubeTex->LoadImage(dwLevels, m_pCubeImage))
			{
				WriteToLog(_T("Unable to load cube texture.\n"));
				return false;
			}

			// Reset to current non cube format
			SetTextureFormat(TEXFORMAT);
		}
	}

	// Now do the bump map textures  
	if (m_bBump)
	{
		if (!SetTextureFormat(BUMPFORMAT))
		{
			WriteToLog(_T("(ERROR) No common BumpDuDv formats.\n"));
			return false;
		}
		else
		{	
			pFormat = &m_pBumpFormats[m_pCurrentMode->nTextureFormat];

			if (!CreateTexture(LARGE_TEXSIZE, LARGE_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pSineBumpTex))
			{
				WriteToLog("(ERROR) Failed to create bump environment map sine texture.  Aborting.\n");
				return false; 
			}
			if (!m_pSineBumpTex->LoadImage(dwLevels, &m_SineBumpImage))
			{
				WriteToLog(_T("Unable to load sine bump texture.\n"));
				return false;
			}

			// Reset to current non bump format
			SetTextureFormat(TEXFORMAT);
		}
	}
    
    // Now do the volume textures
	if (m_bVol)
	{
		if ( m_Flags.Check(VOLUMECAP) )
		{
			if (!SetTextureFormat(VOLUMEFORMAT))
			{
				WriteToLog(_T("No common Volume formats.\n"));
				//There is no need to return here as it is not mandatory that 
				//a driver support volume textures all other format types are 
				//mandatory for any hardware that supports pixel shaders.
			}
			else
			{	
				pFormat = &m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat];

				if (!CreateVolumeTexture(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwDepth, dwLevels, dwUsage, *pFormat, dwPool, &m_pVolTex))        
				{
					WriteToLog("(ERROR) Failed to create volume texture.  Aborting.\n");
					return false; 
				}
				if (!m_pVolTex->LoadImage(1, m_pVolImage, TEXLOAD_UPDATETEXTURE ))
				{
					WriteToLog(_T("Unable to load volume texture.\n"));
					return false;
				}

				// Reset to current non volume format
				SetTextureFormat(TEXFORMAT);
			}
		}
	}

    return CPixelShader::LoadTextures();
}
